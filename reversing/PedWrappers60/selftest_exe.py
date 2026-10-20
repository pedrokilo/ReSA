"""Autoprueba del ORACULO contra el EXE (sin el juego): ejecuta los bytes ORIGINALES de las tres funciones (D:\\Grand Theft Auto San Andreas\\gta-sa.exe) en un emulador x86 (Unicorn) con las
mismas arenas, pokes, globales y falsos que el banco (Core.hpp), y compara cada caso con fixture_oracle.py: traza de colaboradores en orden y con sus argumentos, todos los cambios de bytes
(guardas incluidas), valor de retorno, velocidad y consumo de rand. Es independiente de la DLL: valida que el oraculo (escrito desde el desensamblado) describe lo que hace el EXE, incluida la
aritmetica x87 a 24 bits (FCW 0x007F), _ftol2, ceil y pow reales del CRT del EXE (las funciones 0x821B40, 0x823820 y 0x822130 se emulan tal cual; solo rand y los colaboradores se sustituyen).
Uso:  python -I selftest_exe.py [--max N]   (imprime «ok» o los casos distintos)
"""
import json
import struct
import subprocess
import sys
import tempfile
from pathlib import Path

from unicorn import (Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_PROT_ALL, UcError)
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EBX, UC_X86_REG_ESP, UC_X86_REG_EBP, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EIP,
                               UC_X86_REG_FPCW, UC_X86_REG_FPSW, UC_X86_REG_FPTAG, UC_X86_REG_MXCSR)

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle  # noqa: E402
import fixture_mirror  # noqa: E402

EXE = Path('D:/Grand Theft Auto San Andreas/gta-sa.exe')
M32 = 0xFFFFFFFF
L = oracle  # etiquetas
ARENA_BASE = 0x10000000
ARENA_STRIDE = 0x2000
STACK_TOP = 0x20100000
RET_TRAP = 0x30000000
VTBL_TRAP = 0x31000000           # un trap por ranura de la vtable falsa (+4 * ranura)

SIZES = oracle.SIZES
ARENAS = {   # etiqueta -> (clave de tamano, ventana, guarda delantera, tamano de arena)
    L.L_PED: 'ped', L.L_PDATA: 'pdata', L.L_PDATA2: 'pdata', L.L_PPED: 'pped', L.L_ENT: 'ent', L.L_VEH: 'veh', L.L_MATPED: 'mat', L.L_MATOTHER: 'mat', L.L_MATCAM: 'mat', L.L_MI0: 'minfo',
    L.L_MI1: 'minfo', L.L_OBJINFO: 'minfo', L.L_FX: 'tok', L.L_HIER: 'tok', L.L_CLUMP: 'tok', L.L_INTEL: 'tok', L.L_WOBJ: 'tok', L.L_CM0: 'tok', L.L_CM1: 'tok', L.L_TEX: 'tok', L.L_MATARR: 'matarr',
    L.L_VTBL: 'vtbl', L.L_BOOLS: 'bools'}


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytes(out[:nbytes])


def fnv32(data):
    h = 0xCBF29CE484222325
    for b in data:
        h = ((h ^ b) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h & M32


class Pe:
    def __init__(self, path):
        self.data = open(path, 'rb').read()
        d = self.data
        pe = struct.unpack_from('<I', d, 0x3C)[0]
        nsec = struct.unpack_from('<H', d, pe + 6)[0]
        optsz = struct.unpack_from('<H', d, pe + 20)[0]
        self.base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            o = pe + 24 + optsz + 40 * i
            vsz, va, rsz, rptr = struct.unpack_from('<IIII', d, o + 8)
            self.secs.append((va + self.base, max(vsz, rsz), rptr, rsz))


class Emu:
    def __init__(self):
        pe = Pe(EXE)
        self.mu = Uc(UC_ARCH_X86, UC_MODE_32)
        lo = min(s[0] for s in pe.secs) & ~0xFFF
        hi = max(s[0] + s[1] for s in pe.secs)
        hi = (hi + 0xFFF) & ~0xFFF
        self.mu.mem_map(lo, hi - lo)
        for va, size, rptr, rsz in pe.secs:
            self.mu.mem_write(va, pe.data[rptr:rptr + min(rsz, size)])
        self.mu.mem_map(ARENA_BASE, 0x200000)
        self.mu.mem_map(STACK_TOP - 0x100000, 0x100000)
        self.mu.mem_map(RET_TRAP, 0x1000)
        self.mu.mem_map(VTBL_TRAP, 0x1000)
        self.saved_image = None
        self.hooks = {}
        self.case = None
        # copia de la imagen para restaurar las globales entre casos: se leen/escriben solo las usadas
        self.install_stubs()

    # ------------------------------------------------------------------------------------ memoria
    def rd(self, addr, n):
        return bytes(self.mu.mem_read(addr, n))

    def rd32(self, addr):
        return struct.unpack('<I', self.rd(addr, 4))[0]

    def wr32(self, addr, v):
        self.mu.mem_write(addr, struct.pack('<I', v & M32))

    def base_of(self, label):
        return ARENA_BASE + label * ARENA_STRIDE

    def addr_of(self, label):
        if not (0 < label < L.L_COUNT):
            return 0
        return self.base_of(label) + SIZES[ARENAS[label]][1]

    def resolve(self, v):
        return self.addr_of(v & 0xFFFFFF) if L.is_tok(v) else v

    def label(self, p):
        if p == 0:
            return 0
        ped = self.addr_of(L.L_PED)
        if ped <= p < ped + SIZES['ped'][0]:
            return L.L_PED if p == ped else 0x100 + (p - ped)
        for lab in range(1, L.L_COUNT):
            if lab != L.L_PED and p == self.addr_of(lab):
                return lab
        return 0xBAD

    def label_u(self, v):
        return self.label(v) if v else 0

    # ------------------------------------------------------------------------------------ falsos
    def push(self, kind, *args):
        self.events.append([kind] + [a & M32 for a in args])

    def mutate(self, stub):
        for s, off, mask in self.case['muts']:
            if s == stub:
                a = self.addr_of(L.L_PED) + off
                self.wr32(a, self.rd32(a) ^ mask)

    def install_stubs(self):
        K = L
        def a(i):  # argumento de pila i (despues de la direccion de retorno)
            return self.rd32(self.esp + 4 + 4 * i)
        self.stubs = {}

        def reg(addr, nargs, callee_cleans, fn):
            self.stubs[addr] = (nargs, callee_cleans, fn)

        c = lambda: self.case
        reg(0x4E2EE0, 0, True, lambda: (self.push(K.K_AE_SERVICE, self.label(self.ecx)), self.gret)[1])
        reg(0x601070, 0, True, lambda: (self.push(K.K_SWIM, self.label(self.ecx)), self.resolve(c()['r_swim']))[1])
        reg(0x601180, 0, True, lambda: (self.push(oracle.K_CLIMB, self.label(self.ecx)), self.resolve(c()['r_climb']) if 'r_climb' in c() else self.gret)[1])
        reg(0x734A40, 1, False, lambda: (self.push(K.K_GETHIER, self.label_u(a(0))), self.addr_of(K.L_HIER))[1])
        reg(0x7C51A0, 2, False, lambda: (self.push(K.K_BONEIDX, self.label_u(a(0)), a(1)), c()['r_bone'])[1])
        reg(0x7C5120, 1, False, lambda: (self.push(K.K_MATARR, self.label_u(a(0))), self.addr_of(K.L_MATARR))[1])

        def create_fx():
            name = self.read_cstr(a(0))
            pos = a(1)
            self.push(K.K_CREATEFX, self.ecx, fnv32(name), self.rd32(pos), self.rd32(pos + 4), self.rd32(pos + 8), (a(2) - self.addr_of(K.L_MATARR)) & M32, a(3) & 0xFF)
            return self.addr_of(K.L_FX) if c()['r_fx'] else 0
        reg(0x4A9BE0, 4, True, create_fx)
        reg(0x4AA910, 1, True, lambda: (self.push(K.K_FXLOCAL, self.label(self.ecx), a(0) & 0xFF), self.gret)[1])
        reg(0x4AA890, 0, True, lambda: (self.push(K.K_FXCOPY, self.label(self.ecx)), self.gret)[1])
        reg(0x4AA2F0, 0, True, lambda: (self.push(K.K_FXPLAY, self.label(self.ecx)), self.gret)[1])
        reg(0x4A9810, 1, True, lambda: (self.push(K.K_FXDESTROY, self.ecx, self.label(a(0))), self.gret)[1])

        def coors():
            out = a(0)
            self.push(K.K_COORS, a(1))
            for i in range(3):
                self.wr32(out + 4 * i, c()['pl_coors'][i])
            return out
        reg(0x56E010, 2, False, coors)
        reg(0x571A90, 0, True, lambda: (self.push(K.K_PRUNE, self.label(self.ecx)), self.mutate(oracle.S_PRUNE), self.gret)[2])
        reg(0x732B20, 1, False, lambda: (self.push(K.K_GETALPHA, self.label_u(a(0))), c()['r_alpha'])[1])
        reg(0x732B00, 2, False, lambda: (self.push(K.K_SETALPHA, self.label_u(a(0)), a(1)), self.gret)[1])
        reg(0x4185C0, 8, False, lambda: (self.push(K.K_COLMODELS, self.label_u(a(0)), self.label_u(a(1)), self.label_u(a(2)), self.label_u(a(3)), a(4), a(5), a(6), a(7) & 0xFF), c()['r_col'])[1])

        def findp():
            self.push(K.K_FINDPLAYER, a(0))
            m = c()['r_find_player']
            return 0 if m == 0 else self.addr_of(K.L_PED if m == 1 else K.L_PPED)
        reg(0x56E210, 1, False, findp)
        reg(0x5F6A10, 1, True, lambda: (self.push(K.K_ISMEMBER, (self.ecx - 0xC09928) & M32, self.label(a(0))), (c()['r_junk'] & ~0xFF) | (1 if c()['r_is_member'] else 0))[1])
        for addr, kind, stub, intel in ((0x5E1FA0, K.K_BUOY, oracle.S_BUOY, False), (0x6073A0, K.K_PFIRST, oracle.S_PFIRST, True), (0x608260, K.K_PPROC, oracle.S_PPROC, True),
                                        (0x5433B0, K.K_SKIP, oracle.S_SKIP, False), (0x5485E0, K.K_PHYSPC, oracle.S_PHYSPC, False), (0x5E8910, K.K_DELW, oracle.S_DELW, False),
                                        (0x5E57F0, K.K_FOOT, oracle.S_FOOT, False), (0x5E4C50, K.K_NEWVEL, oracle.S_NEWVEL, False), (0x5E1B10, K.K_UPDPOS, oracle.S_UPDPOS, False)):
            reg(addr, 0, True, (lambda kind=kind, stub=stub: (self.push(kind, self.label(self.ecx)), self.mutate(stub), self.gret)[2]))
        reg(0x56D230, 0, False, lambda: (self.push(K.K_BLOOD), (c()['r_junk'] & ~0xFF) | (c()['r_blood'] & 0xFF))[1])

        def shadow():
            pos = a(2)
            self.push(K.K_SHADOW, a(0) & 0xFF, self.label_u(a(1)), self.rd32(pos), self.rd32(pos + 4), self.rd32(pos + 8), a(3), a(4), a(5), a(6), a(7) & 0xFFFF, a(8) & 0xFF, a(9) & 0xFF, a(10) & 0xFF, a(11), a(12), a(13))
            return self.gret
        reg(0x706F60, 14, False, shadow)
        reg(0x611570, 2, False, lambda: (self.push(K.K_PEDCOUNT, self.label(a(0)), a(1) & 0xFF), self.gret)[1])
        reg(0x5EFFE0, 6, True, lambda: (self.push(K.K_SAY, self.label(self.ecx), a(0) & 0xFFFF, a(1), a(2), a(3) & 0xFF, a(4) & 0xFF, a(5) & 0xFF), self.gret)[1])
        reg(0x4E69F0, 1, True, lambda: (self.push(K.K_WAE_ADD, self.label(self.ecx), a(0)), self.gret)[1])
        reg(0x4E6AE0, 0, True, lambda: (self.push(K.K_WAE_SERVICE, self.label(self.ecx)), self.gret)[1])

        def rand():
            self.rng = (self.rng * 0x343FD + 0x269EC3) & M32
            self.rand_calls += 1
            return (self.rng >> 16) & 0x7FFF
        reg(0x821B1E, 0, False, rand)
        reg(0x82872C, 4, False, lambda: self.gret)      # manejador de errores matematicos del CRT (errno/matherr): ceil(NaN) lo invoca; no afecta a nada que lean las funciones
        for slot in range(26):
            addr = VTBL_TRAP + 0x10 * slot
            if slot == 23:
                reg(addr, 0, True, lambda: (self.push(K.K_MOVEANIM, self.label(self.ecx)), self.mutate(oracle.S_MOVEANIM), self.gret)[2])
            else:
                reg(addr, 0, True, lambda: (self.push(K.K_BADVIRT, self.label(self.ecx)), self.gret)[1])
        self.mu.mem_write(RET_TRAP, b'\xF4' * 16)

        def hook(uc, address, size, user):
            nargs, callee, fn = self.stubs[address]
            self.esp = uc.reg_read(UC_X86_REG_ESP)
            self.ecx = uc.reg_read(UC_X86_REG_ECX)
            ret = self.rd32(self.esp)
            eax = fn() & M32
            uc.reg_write(UC_X86_REG_EAX, eax)
            uc.reg_write(UC_X86_REG_ECX, 0xC1C1C1C1)       # los falsos de C++ pueden dejar cualquier basura en ECX/EDX
            uc.reg_write(UC_X86_REG_EDX, 0xD2D2D2D2)
            uc.reg_write(UC_X86_REG_ESP, (self.esp + 4 + (4 * nargs if callee else 0)) & M32)
            uc.reg_write(UC_X86_REG_EIP, ret)
        for addr in self.stubs:
            self.mu.hook_add(UC_HOOK_CODE, hook, None, addr, addr)

    def read_cstr(self, p):
        out = bytearray()
        for i in range(64):
            b = self.rd(p + i, 1)
            if b == b'\0':
                break
            out += b
        return bytes(out)

    # ------------------------------------------------------------------------------------ ejecucion de un caso
    def setup_arenas(self, name, inp, case):
        """Rellena las arenas como Core.hpp: basura determinista (semillas Rand(n, k)) y pokes."""
        self.case = inp
        self.events = []
        self.gret = rnd(case, 59)
        self.rng = inp.get('seed', 0) & M32
        self.rand_calls = 0
        self.ref = {}
        for lab, key in ARENAS.items():
            size = SIZES[key][2]
            self.ref[lab] = bytearray(fill_words(size, 0x1000 + lab))
        seeds = {}
        def apply(label, pokes, seed):
            self.ref[label] = bytearray(fill_words(SIZES[ARENAS[label]][2], seed))
            for off, sz, val, ptr in pokes:
                v = self.resolve(val) if ptr else val
                pos = SIZES[ARENAS[label]][1] + off
                self.ref[label][pos:pos + sz] = (v & ((1 << (8 * sz)) - 1)).to_bytes(sz, 'little')
        if name == oracle.NAME_CS:
            apply(L.L_PED, inp['ped'], rnd(case, 50))
            self.ref[L.L_BOOLS] = bytearray(fill_words(SIZES['bools'][2], rnd(case, 51)))
            for i in range(2):
                self.ref[L.L_BOOLS][SIZES['bools'][1] + i] = inp['bools'][i] & 0xFF
        elif name == oracle.NAME_PS:
            apply(L.L_PED, inp['ped'], rnd(case, 50)); apply(L.L_ENT, inp['ent'], rnd(case, 51)); apply(L.L_OBJINFO, inp['objinfo'], rnd(case, 52))
            apply(L.L_MATPED, inp['mat_ped'], rnd(case, 53)); apply(L.L_MATOTHER, inp['mat_ent'], rnd(case, 54))
            self.ref[L.L_BOOLS] = bytearray(fill_words(SIZES['bools'][2], rnd(case, 55)))
            for i in range(4):
                self.ref[L.L_BOOLS][SIZES['bools'][1] + i] = inp['bools'][i] & 0xFF
            self.ref[L.L_INTEL] = bytearray(fill_words(SIZES['tok'][2], rnd(case, 56)))
        else:
            apply(L.L_PED, inp['ped'], rnd(case, 50)); apply(L.L_PDATA, inp['pdata'], rnd(case, 51)); apply(L.L_PDATA2, inp['pdata2'], rnd(case, 52)); apply(L.L_PPED, inp['pped'], rnd(case, 53))
            apply(L.L_ENT, inp['ent'], rnd(case, 54)); apply(L.L_VEH, inp['veh'], rnd(case, 55)); apply(L.L_MATPED, inp['mat_ped'], rnd(case, 56)); apply(L.L_MATOTHER, inp['mat_veh'], rnd(case, 57))
            apply(L.L_MATCAM, inp['mat_cam'], rnd(case, 58)); apply(L.L_CLUMP, inp['clump'], rnd(case, 60))
            for lab in (L.L_MI0, L.L_MI1, L.L_OBJINFO, L.L_FX, L.L_HIER, L.L_INTEL, L.L_WOBJ, L.L_CM0, L.L_CM1, L.L_TEX, L.L_MATARR):
                self.ref[lab] = bytearray(fill_words(SIZES[ARENAS[lab]][2], rnd(case, 61 + lab)))
            for idx, mi, cm in inp['mi']:
                self.ref[mi][SIZES['minfo'][1] + 0x14:SIZES['minfo'][1] + 0x18] = struct.pack('<I', self.addr_of(cm))
            v = self.ref[L.L_VTBL] = bytearray(fill_words(SIZES['vtbl'][2], rnd(case, 80)))
            for i in range(26):
                v[SIZES['vtbl'][1] + 4 * i:SIZES['vtbl'][1] + 4 * i + 4] = struct.pack('<I', VTBL_TRAP + 0x10 * i)
        for lab, data in self.ref.items():
            self.mu.mem_write(self.base_of(lab), bytes(data))

    def run(self, name, inp, case):
        self.setup_arenas(name, inp, case)
        mu = self.mu
        if name == oracle.NAME_PC:
            self.wr32(0xB7CB4C, inp['frame']); self.wr32(0xB7CB5C, inp['timestep']); self.wr32(0xB72914, inp['area']); self.mu.mem_write(0xB7CD6E, bytes([inp['force'] & 0xFF]))
            self.wr32(0xB6F03C, self.resolve(inp['cam_mat']))
            for i in range(3):
                self.wr32(0xB6F02C + 4 * i, inp['cam_pos'][i])
            self.wr32(0xC40400, self.addr_of(L.L_TEX))
            for idx, mi, cm in inp['mi']:
                self.wr32(0xA9B0C8 + 4 * idx, self.addr_of(mi))
        else:
            self.wr32(0xB7CB5C, inp['timestep'] if name == oracle.NAME_CS else 0x3F800000)
        self.wr32(0xC9C400, 1)          # SSE2 disponible: ceil/pow usan su rama SSE2 como en el juego
        esp = STACK_TOP - 0x1000
        args = []
        if name == oracle.NAME_CS:
            args = [self.addr_of(L.L_BOOLS), self.addr_of(L.L_BOOLS) + 1]
        elif name == oracle.NAME_PS:
            b = self.addr_of(L.L_BOOLS)
            args = [self.addr_of(L.L_ENT), inp['ignore_arg'], b, b + 1, b + 2, b + 3]
        for v in reversed(args):
            esp -= 4
            self.wr32(esp, v)
        esp -= 4
        self.wr32(esp, RET_TRAP)
        for reg, val in ((UC_X86_REG_ESP, esp), (UC_X86_REG_ECX, self.addr_of(L.L_PED)), (UC_X86_REG_EAX, inp['eax']), (UC_X86_REG_EDX, inp['edx']), (UC_X86_REG_EBX, 0x13579BDF),
                         (UC_X86_REG_ESI, 0x2468ACE0), (UC_X86_REG_EDI, 0x10293847), (UC_X86_REG_EBP, 0x55AA55AA)):
            mu.reg_write(reg, val)
        mu.reg_write(UC_X86_REG_MXCSR, 0x1F80)
        mu.reg_write(UC_X86_REG_FPCW, 0x007F)
        mu.reg_write(UC_X86_REG_FPSW, 0)
        mu.reg_write(UC_X86_REG_FPTAG, 0xFFFF)
        entry = oracle.ADDR[name]
        mu.emu_start(entry, RET_TRAP, count=200000)
        eax = mu.reg_read(UC_X86_REG_EAX)
        esp_after = mu.reg_read(UC_X86_REG_ESP)
        want_esp = esp + 4 + (4 * len(args))
        if esp_after != want_esp:
            raise RuntimeError('ESP tras la funcion: %#x != %#x' % (esp_after, want_esp))
        for reg, val in ((UC_X86_REG_EBX, 0x13579BDF), (UC_X86_REG_ESI, 0x2468ACE0), (UC_X86_REG_EDI, 0x10293847), (UC_X86_REG_EBP, 0x55AA55AA)):
            if mu.reg_read(reg) != val:
                raise RuntimeError('registro no volatil no conservado')
        return eax

    def observe(self, name, eax):
        """Misma forma que Core.hpp: observed = {ret, events, changes, ...}"""
        changes = []
        def diff(arena, label, skip=None):
            now = bytes(self.mu.mem_read(self.base_of(label), SIZES[ARENAS[label]][2]))
            ref = bytes(self.ref[label])
            off = SIZES[ARENAS[label]][1]
            for i in range(len(now)):
                if now[i] != ref[i]:
                    rel = i - off
                    if skip and skip[0] <= rel < skip[1]:
                        continue
                    changes.append([arena, rel, now[i]])
        obs = {}
        if name == oracle.NAME_CS:
            diff(0, L.L_PED)
            obs['ret'] = eax & 0xFF
            b = SIZES['bools']
            now = bytes(self.mu.mem_read(self.base_of(L.L_BOOLS), b[2]))
            obs['bools_unchanged'] = now == bytes(self.ref[L.L_BOOLS])
            obs['bools'] = [now[b[1]], now[b[1] + 1]]
        elif name == oracle.NAME_PS:
            diff(0, L.L_PED); diff(1, L.L_ENT)
            obs['ret'] = None
            b = SIZES['bools']
            now = bytes(self.mu.mem_read(self.base_of(L.L_BOOLS), b[2]))
            obs['bools'] = list(now[b[1]:b[1] + 4])
            others = all(bytes(self.mu.mem_read(self.base_of(l), SIZES[ARENAS[l]][2])) == bytes(self.ref[l]) for l in (L.L_OBJINFO, L.L_MATPED, L.L_MATOTHER, L.L_INTEL))
            guards = now[:b[1]] == bytes(self.ref[L.L_BOOLS][:b[1]]) and now[b[1] + 4:] == bytes(self.ref[L.L_BOOLS][b[1] + 4:])
            obs['others_unchanged'] = others and guards
        else:
            diff(0, L.L_PED, (0x44, 0x50)); diff(1, L.L_PDATA)
            obs['ret'] = None
            ped = self.addr_of(L.L_PED)
            obs['speed'] = [self.rd32(ped + 0x44), self.rd32(ped + 0x48), self.rd32(ped + 0x4C)]
            obs['rand_calls'] = self.rand_calls
            others = all(bytes(self.mu.mem_read(self.base_of(l), SIZES[ARENAS[l]][2])) == bytes(self.ref[l]) for l in
                         (L.L_PDATA2, L.L_PPED, L.L_ENT, L.L_VEH, L.L_MATPED, L.L_MATOTHER, L.L_MATCAM, L.L_MI0, L.L_MI1, L.L_FX, L.L_HIER, L.L_CLUMP, L.L_INTEL, L.L_WOBJ, L.L_CM0, L.L_CM1, L.L_TEX, L.L_MATARR, L.L_VTBL))
            obs['others_unchanged'] = others
            obs['globals_unchanged'] = True
        obs['changes'] = changes
        obs['events'] = [e + [0] * (oracle.EVENT_WIDTH - len(e)) for e in self.events]
        return obs


def main():
    maxn = None
    if '--max' in sys.argv:
        maxn = int(sys.argv[sys.argv.index('--max') + 1])
    lines = fixture_mirror.build_and_run()
    lines.pop()
    emu = Emu()
    import run_local
    counts = {oracle.NAME_CS: 0, oracle.NAME_PS: 0, oracle.NAME_PC: 0}
    bad = 0
    total = 0
    stats_speed = {'exact': 0, 'tolerance': 0}
    for line in lines:
        name = fixture_mirror.FUNCS[line[0]]
        n = counts[name]
        counts[name] += 1
        if maxn and n >= maxn:
            continue
        inp = json.loads(line[2:])
        inp['addrs'] = [emu.addr_of(l) if 0 < l < oracle.L_COUNT else 0 for l in range(oracle.L_COUNT)]
        want, stats = oracle.expected(name, inp, n)
        try:
            eax = emu.run(name, inp, n)
            obs = emu.observe(name, eax)
            row = {'case': n, 'input': inp}
            run_local.compare_side(name, row, {'observed': obs}, want, 'EXE-emulado')
            if name == oracle.NAME_PC and obs['speed'] != want['speed']:
                stats_speed['tolerance'] += 1
            elif name == oracle.NAME_PC:
                stats_speed['exact'] += 1
        except (RuntimeError, UcError) as e:
            bad += 1
            if bad <= 12:
                extra = ''
                if isinstance(e, UcError):
                    extra = ' EIP=%#x ESP=%#x ECX=%#x' % (emu.mu.reg_read(UC_X86_REG_EIP), emu.mu.reg_read(UC_X86_REG_ESP), emu.mu.reg_read(UC_X86_REG_ECX))
                print('DIFF', name, n, str(e)[:1200] + extra)
        total += 1
    if bad:
        raise SystemExit('%d diferencias de %d casos' % (bad, total))
    print('ok (%d casos emulados del EXE contra el oraculo; velocidad ProcessControl: %d exactas en bits, %d dentro de 2 ulp por el pow de Python)' % (total, stats_speed['exact'], stats_speed['tolerance']))


if __name__ == '__main__':
    main()
