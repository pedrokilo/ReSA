"""Comprobacion SIN juego del oraculo de FnS7A1A contra los BYTES DEL EXE: emula (Unicorn) cada una de las 21 funciones originales con el mismo estado inicial que el banco (arenas y regiones
que fija el oraculo en el instante de la instantanea), los mismos falsos de colaboradores y las mismas llamadas, y compara lo observado (bytes cambiados de las arenas, regiones globales, eventos de los
falsos, valor devuelto) con `fixture_oracle.expected`. No sustituye al banco dentro del juego (que tambien prueba la DLL, el estado x87 y la ABI): descubre errores de lectura del desensamblado en el oraculo.

    python -I emu_check.py [casos_por_funcion=1024] [--exe <gta_sa.exe>]

Necesita `unicorn` y compila fixture_dump.cpp con cl (como fixture_selfcheck.py).
"""
import struct
import sys
import time
from pathlib import Path

import unicorn
from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_PROT_ALL
from unicorn.x86_const import *

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
import fixture_selfcheck as selfcheck

EXE = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')
M32 = 0xFFFFFFFF
STACK_LO, STACK_HI = 0x7E000000, 0x7E100000
SENTINEL = 0x7FFF0000
J0, J1 = 0xAAAA5555, 0x3333


def load_image(uc, path):
    d = Path(path).read_bytes()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    size = struct.unpack_from('<I', d, pe + 24 + 56)[0]
    uc.mem_map(base, (size + 0xFFF) & ~0xFFF, UC_PROT_ALL)
    so = pe + 24 + opt
    for i in range(nsec):
        n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, so + i * 40)
        uc.mem_write(base + va, d[rp:rp + min(rs, vs)])
    return base


class Emu:
    def __init__(self, exe):
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        load_image(self.uc, exe)
        for b in oracle.DEFAULT_ARENAS:
            self.uc.mem_map(b, 0x2000, UC_PROT_ALL)
        self.uc.mem_map(STACK_LO, STACK_HI - STACK_LO, UC_PROT_ALL)
        self.uc.mem_map(SENTINEL, 0x1000, UC_PROT_ALL)
        self.uc.mem_write(SENTINEL, b'\x90' * 16)
        self.uc.hook_add(UC_HOOK_CODE, lambda uc, a, s, u: uc.emu_stop(), begin=SENTINEL, end=SENTINEL)
        self.fakes = {}
        for addr in (0x7ED540, 0x7ED460, 0x81A3C0, 0x749B70, 0x7C7550, 0x7C77E0, 0x7C7810, 0x7C5120, 0x7F2070, 0x7EDD90):
            self.uc.hook_add(UC_HOOK_CODE, self.fake_hook, begin=addr, end=addr)
        self.c = None

    # --------------------------------------------------------------------------- utilidades
    def rd32(self, a):
        return struct.unpack('<I', bytes(self.uc.mem_read(a & M32, 4)))[0]

    def wr32(self, a, v):
        self.uc.mem_write(a & M32, struct.pack('<I', v & M32))

    def tag(self, v):
        v &= M32
        if v == 0:
            return 0
        if STACK_LO <= v < STACK_HI:
            return 0x7F000000
        for i, b in enumerate(oracle.DEFAULT_ARENAS):
            if b <= v < b + oracle.N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def event(self, eid, args):
        self.events.append([eid] + [x & M32 for x in args])
        a = self.a
        if a[40] and eid == a[40]:
            n = self.pcount
            self.pcount += 1
            if n == a[41]:
                if a[42] == 0:
                    self.uc.mem_write(oracle.GLOBALS[a[43]] + a[44], struct.pack('<I', a[46]))
                else:
                    self.uc.mem_write(oracle.DEFAULT_ARENAS[a[43]] + oracle.GUARD + a[44], struct.pack('<I', a[46]))

    # --------------------------------------------------------------------------- falsos (mismos que LiveComparison.hpp)
    def fake_hook(self, uc, address, size, user):
        esp = uc.reg_read(UC_X86_REG_ESP)
        ret = self.rd32(esp)
        arg = lambda n: self.rd32(esp + 4 + 4 * n)
        a = self.a
        eax = 0
        if address == 0x7ED540:
            self.events.append([0x7ED540, self.tag(arg(0)), self.tag(arg(1)), arg(2) & M32])
            if a[9]:
                self.wr32(arg(1), a[10])
            self.poke(0x7ED540)
            eax = a[8]
        elif address == 0x7ED460:
            v = self.rd32(arg(1))
            self.events.append([0x7ED460, self.tag(arg(0)), self.tag(arg(1)), arg(2), v])
            self.poke(0x7ED460)
            eax = a[9]
        elif address == 0x81A3C0:
            self.events.append([0x81A3C0, arg(0), arg(1), arg(2), self.tag(arg(3)), arg(4), self.tag(arg(5)), self.tag(arg(6)), arg(7), arg(8), arg(9), self.tag(arg(10)), arg(11)])
            self.wr32(arg(6), a[15])
            self.wr32(arg(10), a[16])
            self.wr32(arg(5), a[17])
            if a[14]:
                self.wr32(arg(3), a[13])
            self.poke(0x81A3C0)
            eax = a[12]
        elif address == 0x749B70:
            self.events.append([0x749B70, self.tag(arg(0)), arg(1), self.tag(arg(2))])
            if arg(1) == 0x734810:
                self.wr32(arg(2), self.bases[1] + oracle.GUARD + a[7])
            elif arg(1) == 0x734A20:
                self.wr32(arg(2), self.bases[1] + oracle.GUARD + a[8])
            self.poke(0x749B70)
            eax = arg(0)
        elif address == 0x7C7550:
            self.events.append([0x7C7550, arg(0)])
            self.poke(0x7C7550)
            eax = a[5]
        elif address == 0x7C77E0:
            self.events.append([0x7C77E0, arg(0)])
            self.poke(0x7C77E0)
            eax = a[4]
        elif address == 0x7C7810:
            self.events.append([0x7C7810, arg(0)])
            self.poke(0x7C7810)
            eax = self.bases[3] + oracle.GUARD + a[10]
        elif address == 0x7C5120:
            self.events.append([0x7C5120, self.tag(arg(0))])
            self.poke(0x7C5120)
            eax = self.bases[3] + oracle.GUARD + a[10]
        elif address == 0x7F2070:
            dst, src = arg(0), arg(1)
            w = struct.unpack('<16I', bytes(uc.mem_read(src, 64)))
            self.events.append([0x7F2070, self.tag(dst), self.tag(src), oracle.crc(bytes(uc.mem_read(src, 64)))])
            uc.mem_write(dst, struct.pack('<16I', *[(w[k] ^ ((k * 0x01010101 + a[11]) & M32)) & M32 for k in range(16)]))
            self.poke(0x7F2070)
            eax = dst
        elif address == 0x7EDD90:
            dst, src, cnt, mat = arg(0), arg(1), arg(2), arg(3)
            v = struct.unpack('<3I', bytes(uc.mem_read(src, 12)))
            mcrc = oracle.crc(bytes(uc.mem_read(mat, 64)))
            self.events.append([0x7EDD90, self.tag(dst), self.tag(src), cnt, self.tag(mat), oracle.crc(bytes(uc.mem_read(src, 12))), mcrc])
            xors = (a[11], a[12], a[13])
            uc.mem_write(dst, struct.pack('<3I', *[(v[i] ^ xors[i] ^ mcrc) & M32 for i in range(3)]))
            self.poke(0x7EDD90)
            eax = dst
        uc.reg_write(UC_X86_REG_EAX, eax & M32)
        uc.reg_write(UC_X86_REG_ESP, esp + 4)
        uc.reg_write(UC_X86_REG_EIP, ret)

    def poke(self, eid):
        a = self.a
        if a[40] and eid == a[40]:
            n = self.pcount
            self.pcount += 1
            if n == a[41]:
                if a[42] == 0:
                    self.uc.mem_write(oracle.GLOBALS[a[43]] + a[44], struct.pack('<I', a[46]))
                else:
                    self.uc.mem_write(self.bases[a[43]] + oracle.GUARD + a[44], struct.pack('<I', a[46]))

    # --------------------------------------------------------------------------- un caso
    def args_of(self, c, this):
        a = [x & M32 for x in c['a']]
        raw = lambda arena, off: (self.bases[arena] + oracle.GUARD + off) & M32
        k = oracle.KIND[c['fn']]
        E = oracle
        if k in (E.ENGINE, E.TEXDTOR):
            return [a[4], a[5], a[6]], False
        if k == E.TEXCTOR:
            return [this, a[5], a[6]], False
        if k == E.TEXCOPY:
            return [this, this if a[10] else (raw(1, 0) + a[9]) & M32, J0, J1], False
        if k in (E.STREAD, E.STWRITE):
            return [a[4], a[5], this if a[6] else 0, J0, J1], False
        if k == E.STSIZE:
            return [this if a[6] else 0, J0, J1], False
        if k == E.FINDFREE:
            return [], True
        if k == E.QUAT:
            src = raw((1, 2, 0, 0)[a[4]], a[5]) if a[4] != 3 else this
            return [src], True
        if k == E.NOP:
            p1 = raw(1, 0x20 + (a[4] & 7)) if a[6] == 0 else (this if a[6] == 1 else raw(0, 0x60 + (a[4] & 3)))
            p2 = raw(1, 0x40 + (a[5] & 7)) if a[7] == 0 else (raw(2, 0x10 + (a[5] & 3)) if a[7] == 1 else raw(0, 0x200 + (a[5] & 7)))
            return [p1, p2], False
        if k == E.FILL:
            return [raw(1, a[4]), this], False
        if k == E.FIGHT:
            return [raw(1, a[4]), this], False
        if k in (E.KICK, E.HIT):
            return [raw(1, a[6]), this], False
        if k == E.GANG:
            return [0 if a[7] else raw(1, a[6]), this], False
        if k == E.SIT:
            return [raw(1, a[4]), this], False
        if k == E.ALPHA:
            return [this, a[4]], False
        if k == E.DFX:
            return [this, raw(3, a[8])], False
        return [this], False

    def run_case(self, c, holder):
        uc = self.uc
        self.c = c
        self.a = [x & M32 for x in c['a']]
        self.bases = oracle.DEFAULT_ARENAS
        self.events = []
        self.pcount = 0
        fn = c['fn']
        # estado inicial (el del oraculo en la instantanea)
        refs = []
        for i, (kind, seed, m) in enumerate(holder['mem']):
            mem = oracle.Mem(kind, seed)
            mem.m = dict(m)
            img = bytes(mem.get(j) for j in range(oracle.N))
            uc.mem_write(self.bases[i], img)
            refs.append(img)
        for g, rb in holder['R'].items():
            uc.mem_write(oracle.GLOBALS[g], rb)
        this = self.bases[0] + oracle.GUARD + self.a[0]
        args, thiscall = self.args_of(c, this)
        uc.mem_write(STACK_HI - 0x3000, b'\0' * 0x2000)   # la pila local arranca a cero, como en el modelo
        esp = STACK_HI - 0x1000
        for v in reversed(args):
            esp -= 4
            uc.mem_write(esp, struct.pack('<I', v & M32))
        esp -= 4
        uc.mem_write(esp, struct.pack('<I', SENTINEL))
        for reg, v in ((UC_X86_REG_EAX, c['eax']), (UC_X86_REG_ECX, this if thiscall else c['ecx']), (UC_X86_REG_EDX, c['edx']), (UC_X86_REG_EBX, 0x13579BDF),
                       (UC_X86_REG_ESI, 0x2468ACE0), (UC_X86_REG_EDI, 0x10293847), (UC_X86_REG_EBP, 0x7E0F0000), (UC_X86_REG_ESP, esp)):
            uc.reg_write(reg, v & M32)
        try:
            uc.emu_start(oracle.ENTRIES[fn], SENTINEL, count=200000)
        except unicorn.UcError as ex:
            raise RuntimeError('emulacion: %s en eip=%08x (funcion %s caso %d)' % (ex, uc.reg_read(UC_X86_REG_EIP), oracle.NAMES[fn], c['n']))
        eax = uc.reg_read(UC_X86_REG_EAX) & M32
        changes = {}
        for i, nm in enumerate('ABCD'):
            img = bytes(uc.mem_read(self.bases[i], oracle.N))
            changes[nm] = [[j - oracle.GUARD, img[j]] for j in range(oracle.N) if img[j] != refs[i][j]]
        gch, big = [], None
        for g in oracle.G_OF.get(fn, ()):
            after = bytes(uc.mem_read(oracle.GLOBALS[g], oracle.GSIZE[g]))
            if oracle.GSIZE[g] > 0x40:
                big = oracle.crc(after)
                continue
            before = holder['R'][g]
            gch += [[oracle.GLOBALS[g] + kb, after[kb]] for kb in range(oracle.GSIZE[g]) if before[kb] != after[kb]]
        ret = None
        if fn in oracle.RET_EAX:
            ret = eax
        elif fn in oracle.RET_AL:
            ret = eax & 0xFF
        return {'events': self.events, 'changes': changes, 'gchanges': gch, 'ret': ret, 'big': big, 'threw': False}


def capture(c):
    holder = {}
    orig = oracle.World.snap

    def snap(self):
        orig(self)
        holder['mem'] = [(m.kind, m.seed, dict(m.m)) for m in self.mem]
        holder['R'] = {g: bytes(b) for g, b in self.R.items()}
    oracle.World.snap = snap
    try:
        e = oracle.expected(c)
    finally:
        oracle.World.snap = orig
    return e, holder


def main():
    per = 1024
    exe = EXE
    argv = sys.argv[1:]
    if '--exe' in argv:
        exe = Path(argv[argv.index('--exe') + 1])
    nums = [x for x in argv if x.isdigit()]
    if nums:
        per = int(nums[0])
    rows = selfcheck.dump_rows()
    emu = Emu(exe)
    bad = total = 0
    t0 = time.time()
    seen = {}
    for row in rows:
        fn = row['fn']
        seen[fn] = seen.get(fn, 0) + 1
        if seen[fn] > per:
            continue
        e, holder = capture(row)
        o = emu.run_case(row, holder)
        total += 1
        for key in ('changes', 'gchanges', 'events', 'ret', 'big'):
            if o[key] != e[key]:
                bad += 1
                if bad <= 12:
                    print('DIFIERE', oracle.NAMES[fn], 'caso', row['n'], key, 'EXE:', str(o[key])[:260], 'oraculo:', str(e[key])[:260])
                break
    print('casos emulados', total, 'con diferencias respecto al oraculo', bad, 'en %.0f s' % (time.time() - t0))
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
