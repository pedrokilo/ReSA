"""Oraculo independiente del banco Fn237A: emulacion del codigo ORIGINAL del EXE (gta-sa.exe sin parchear) con Unicorn.

Para cada caso reconstruye la misma escena que el banco C++ (arenas A..D, memoria global del EXE, rellenos pseudoaleatorios, estructura y elecciones de bank_spec.py), ejecuta la funcion del EXE
(ProcessCommands1200To1299 o 1900To1999) desde su direccion de entrada con los colaboradores 'F' sustituidos por los mismos falsos que usa el banco (registro de eventos y respuestas
deterministas por semilla) y devuelve lo observable: bytes cambiados de las arenas, bytes cambiados de la memoria global, eventos de los falsos y AL. No usa nada de la DLL.
Los colaboradores 'R' (reales) se ejecutan tal cual desde los bytes del EXE.
"""
import json
import struct
import sys
from pathlib import Path

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_MEM_WRITE, UC_HOOK_MEM_READ_UNMAPPED, UC_HOOK_MEM_WRITE_UNMAPPED, UC_HOOK_MEM_FETCH_UNMAPPED, UC_PROT_ALL, UcError
from unicorn.x86_const import (UC_X86_REG_EAX, UC_X86_REG_ECX, UC_X86_REG_EDX, UC_X86_REG_EBX, UC_X86_REG_ESI, UC_X86_REG_EDI, UC_X86_REG_EBP, UC_X86_REG_ESP, UC_X86_REG_EIP,
                               UC_X86_REG_FS, UC_X86_REG_SS, UC_X86_REG_DS, UC_X86_REG_ES, UC_X86_REG_GDTR, UC_X86_REG_FPCW, UC_X86_REG_EFLAGS)

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import bank_spec as S

EXE_PATH = r'D:\Grand Theft Auto San Andreas\gta-sa.exe'
M32 = 0xFFFFFFFF
FNVP = 16777619


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x85EBCA6B) & M32
    x ^= x >> 13
    x = (x * 0xC2B2AE35) & M32
    x ^= x >> 16
    return x


def ans(seed, ident, k):
    return mix((seed * 0x9E3779B1 + ident * 0x7FEB352D + k * 0x846CA68B + 0x1234567) & M32)


def pickhash(seed, i):
    return mix((seed ^ ((i * 0x9E3779B1) & M32)) & M32)


def fill_bytes(seed, n, fset=None):
    s = seed & M32 or 1
    out = bytearray()
    for _ in range((n + 3) // 4):
        s ^= (s << 13) & M32
        s ^= s >> 17
        s ^= (s << 5) & M32
        out += (fset[(s >> 8) % len(fset)] if fset else s).to_bytes(4, 'little')
    return bytes(out[:n])


def fnv(b):
    h = 2166136261
    for c in b:
        h ^= c
        h = (h * FNVP) & M32
    return h


def window(size):
    n = S.GUARD + size + S.GUARD
    return (n + 0xFFF) & ~0xFFF


WINDOWS = [window(s) for s in S.ARENA_SIZE]
KINDS = {'A': 0, 'B': 1, 'C': 2, 'D': 3}

# ------------------------------------------------------------------------------------------------ especificacion procesada
IDS = {c['name']: i for i, c in enumerate(S.COLLABS)}
BY_ADDR = {c['exe']: i for i, c in enumerate(S.COLLABS) if c['exe'] and c['mode'] == 'F'}
NFAKES = len(S.COLLABS)
LAYOUT = {}   # nombre de colaborador -> (conv, args, ret)


class Image:
    def __init__(self):
        d = open(EXE_PATH, 'rb').read()
        pe = struct.unpack_from('<I', d, 0x3C)[0]
        n = struct.unpack_from('<H', d, pe + 6)[0]
        opt = struct.unpack_from('<H', d, pe + 20)[0]
        self.secs = [struct.unpack_from('<8sIIII', d, pe + 24 + opt + i * 40) for i in range(n)]
        self.base = 0x400000
        end = max(vaa + max(vs, rs) for nm, vs, vaa, rs, ro in self.secs)
        self.size = (end + 0xFFF) & ~0xFFF
        self.mem = bytearray(self.size)
        for nm, vs, vaa, rs, ro in self.secs:
            self.mem[vaa:vaa + rs] = d[ro:ro + rs]


IMG = None


def image():
    global IMG
    if IMG is None:
        IMG = Image()
    return IMG


STACK_LO, STACK_HI = 0x00100000, 0x00180000
TIB = 0x00010000
GDT = 0x00011000
SENTINEL = 0x00020000
RET_STUB = 0x00021000
FLD_STUB = 0x00021010
FLD_SLOT = 0x00021100
VFAKE = 0x00030000            # direcciones de los falsos virtuales: VFAKE + 16 * id
LABEL_SCRATCH = 0x00040000


class Machine:
    def __init__(self, trace_calls=False):
        img = image()
        self.uc = uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.trace_calls = trace_calls
        uc.mem_map(img.base, img.size, UC_PROT_ALL)
        uc.mem_write(img.base, bytes(img.mem))
        for a, n in ((TIB, 0x1000), (GDT, 0x1000), (SENTINEL, 0x1000), (RET_STUB, 0x1000), (VFAKE, 0x10000), (LABEL_SCRATCH, 0x1000)):
            uc.mem_map(a, n, UC_PROT_ALL)
        uc.mem_map(STACK_LO, STACK_HI - STACK_LO, UC_PROT_ALL)
        uc.mem_write(RET_STUB, b'\xC3')
        uc.mem_write(FLD_STUB, b'\xD9\x05' + struct.pack('<I', FLD_SLOT) + b'\xC3')
        uc.mem_write(SENTINEL, b'\x90')
        # FS -> TIB mediante GDT
        flat = struct.pack('<HHBBBB', 0xFFFF, 0, 0, 0x92, 0xCF, 0)
        gdt = struct.pack('<Q', 0) + self._desc(TIB, 0xFFF) + flat
        uc.mem_write(GDT, gdt)
        uc.reg_write(UC_X86_REG_GDTR, (0, GDT, len(gdt), 0))
        uc.reg_write(UC_X86_REG_FS, 8)
        for r in (UC_X86_REG_SS, UC_X86_REG_DS, UC_X86_REG_ES):
            uc.reg_write(r, 0x10)
        self.arena_base = [0] * 4
        self.mapped = []
        self.dirty = set()
        uc.hook_add(UC_HOOK_MEM_WRITE, self._on_write)
        uc.hook_add(UC_HOOK_MEM_READ_UNMAPPED | UC_HOOK_MEM_WRITE_UNMAPPED | UC_HOOK_MEM_FETCH_UNMAPPED, self._on_unmapped)
        self.fail = None
        self.events = []
        self.cnt = {}
        self.case = None
        for c in S.COLLABS:
            if c['mode'] == 'F':
                i = IDS[c['name']]
                addr = c['exe'] if c['exe'] else VFAKE + 16 * i
                uc.hook_add(UC_HOOK_CODE, self._on_fake, begin=addr, end=addr, user_data=i)
        self.executed_calls = set()
        uc.hook_add(UC_HOOK_CODE, self._on_toupper, begin=0x823A0E, end=0x823A0E)   # toupper del CRT (usa datos de localizacion del sistema): solo ASCII, igual que la configuracion "C"
        if trace_calls:
            uc.hook_add(UC_HOOK_CODE, self._on_code)

    @staticmethod
    def _desc(base, limit):
        # descriptor de datos, lectura/escritura, 32 bits, presente, DPL 0
        return struct.pack('<HHBBBB', limit & 0xFFFF, base & 0xFFFF, (base >> 16) & 0xFF, 0x92, 0xC0 | ((limit >> 16) & 0xF), (base >> 24) & 0xFF)

    def _on_toupper(self, uc, address, size, user):
        esp = uc.reg_read(UC_X86_REG_ESP)
        c = self.r32(esp + 4)
        uc.reg_write(UC_X86_REG_EAX, c - 32 if 0x61 <= c <= 0x7A else c)
        uc.reg_write(UC_X86_REG_EIP, RET_STUB)

    def _on_code(self, uc, address, size, user):
        b = uc.mem_read(address, size)
        if b[0] == 0xE8 and size == 5:
            self.executed_calls.add((address + 5 + struct.unpack('<i', bytes(b[1:5]))[0]) & M32)

    def _on_write(self, uc, access, address, size, value, user):
        self.dirty.add(address & ~0xFFF)
        if (address + size - 1) & ~0xFFF != address & ~0xFFF:
            self.dirty.add((address + size - 1) & ~0xFFF)

    def _on_unmapped(self, uc, access, address, size, value, user):
        self.fail = 'acceso a memoria sin mapear %#x (acceso %d) en eip=%#x' % (address, access, uc.reg_read(UC_X86_REG_EIP))
        return False

    # -------------------------------------------------------------------------------------------- memoria
    def r32(self, a):
        return struct.unpack('<I', bytes(self.uc.mem_read(a, 4)))[0]

    def w32(self, a, v):
        self.uc.mem_write(a, struct.pack('<I', v & M32))

    def readbytes(self, a, n):
        return bytes(self.uc.mem_read(a, n))

    def tag(self, v):
        if v == 0:
            return 0
        if STACK_LO <= v < STACK_HI:
            return 0x7F000000
        for i in range(4):
            b = self.arena_base[i]
            if b <= v < b + WINDOWS[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v & M32

    def resolve(self, v, seed):
        if isinstance(v, int):
            return v & M32
        if isinstance(v, tuple):
            kind, off = v
            if kind == 'F':
                return VFAKE + 16 * IDS[off]
            return (self.arena_base[KINDS[kind]] + S.GUARD + off) & M32
        if isinstance(v, str) and v.startswith('KEY:'):
            j = int(v[4:])
            if j == 100:
                j = ans(seed, IDS['ReadTextLabelFromScript'], 0) % len(S.LABELS)
            return self.keys[j]
        raise ValueError(v)

    # -------------------------------------------------------------------------------------------- escena
    def setup(self, inp):
        """construye la escena del caso y devuelve la imagen de referencia"""
        seed = inp['seed']
        uc = self.uc
        # liberar arenas anteriores
        if self.mapped != [(b, WINDOWS[i]) for i, b in enumerate(inp['arena'])]:
            for a, n in self.mapped:
                uc.mem_unmap(a, n)
            self.mapped = []
            for i in range(4):
                uc.mem_map(inp['arena'][i], WINDOWS[i], UC_PROT_ALL)
                self.mapped.append((inp['arena'][i], WINDOWS[i]))
        self.arena_base = list(inp['arena'])
        mems = [bytearray(b'\xAA' * w) for w in WINDOWS]
        for i in (0, 1):
            mems[i][S.GUARD:S.GUARD + S.ARENA_SIZE[i]] = bytes(S.ARENA_SIZE[i])
        mems[2][S.GUARD:S.GUARD + S.ARENA_SIZE[2]] = b'\xCD' * S.ARENA_SIZE[2]
        mems[3][S.GUARD:S.GUARD + S.ARENA_SIZE[3]] = b'\xDD' * S.ARENA_SIZE[3]
        for i in range(4):
            uc.mem_write(self.arena_base[i], bytes(mems[i]))
        # relleno aleatorio
        for k, f in enumerate(S.FILLS):
            a, off, n = f[0], f[1], f[2]
            data = fill_bytes(mix(seed ^ ((k + 1) * 0x9E3779B1)), n, S.FSET if len(f) > 3 else None)
            self._put(a, off, data)
        for (a, off, size, val) in S.STRUCT:
            self._poke(a, off, size, self.resolve(val, seed))
        for i, (a, off, size, vals) in enumerate(S.CHOICES):
            v = vals[pickhash(seed, i) % len(vals)]
            self._poke(a, off, size, self.resolve(v, seed))
        self.refs = [self.readbytes(self.arena_base[i], WINDOWS[i]) for i in range(4)]
        self.gref = [(addr, self.readbytes(addr, n)) for addr, n in S.GLOBALS]

    def _put(self, a, off, data):
        addr = (self.arena_base[KINDS[a]] + S.GUARD + off) if a in KINDS else off
        self.uc.mem_write(addr, data)

    def _poke(self, a, off, size, val):
        self._put(a, off, (val & ((1 << (8 * size)) - 1)).to_bytes(size, 'little'))

    # -------------------------------------------------------------------------------------------- falsos
    def _on_fake(self, uc, address, size, ident):
        try:
            self._fake(uc, ident)
        except UcError as e:
            self.fail = 'error en falso %s: %s' % (S.COLLABS[ident]['name'], e)
            uc.emu_stop()

    def ansv(self, ident, j):
        return ans(self.case['seed'], ident, self.cnt[ident] * 16 + j)

    def _fake(self, uc, ident):
        c = S.COLLABS[ident]
        esp = uc.reg_read(UC_X86_REG_ESP)
        ecx = uc.reg_read(UC_X86_REG_ECX)
        ret = self.r32(esp)
        args = [self.r32(esp + 4 + 4 * i) for i in range(len(c['args']))]
        self.cnt.setdefault(ident, 0)
        ev = [ident]
        if c['conv'] == 'T':
            ev.append(self.tag(ecx))
        name = c['name']
        a0 = self.ansv(ident, 0)
        eax = 0
        floatret = None
        sp = 0xA43C78
        letters = c['args']
        # argumentos
        k = 0
        for i, ch in enumerate(letters):
            v = args[i]
            if ch == 'w':
                ev.append(self.tag(v))
            elif ch == 'f':
                ev.append(v)
            elif ch == 'B':
                ev.append(v & 0xFF)
            elif ch == 'H':
                ev.append(v & 0xFFFF)
            elif ch == 'v':
                ev.extend(struct.unpack('<3I', self.readbytes(v, 12)))
            elif ch == 'u':
                ev.extend(struct.unpack('<2I', self.readbytes(v, 8)))
            elif ch == 's':
                ev.append(fnv(self.cstr(v)))
            elif ch == 'm':
                mm = self.readbytes(v, 0x30)
                ev.extend(struct.unpack_from('<I', mm, o)[0] for o in (0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28))
            elif ch == 'x':
                pass
            elif ch == 'L':
                label = S.LABELS[a0 % len(S.LABELS)].encode()
                n = args[i + 1] & 0xFF
                uc.mem_write(v, label[:max(n - 1, 0)] + b'\x00')
                ev.append(S.LABELS.index(S.LABELS[a0 % len(S.LABELS)]))
            elif ch == 'o':
                uc.mem_write(v, struct.pack('<I', S.NODES[self.ansv(ident, 1 + k) % len(S.NODES)])); k += 1
            elif ch == 'g':
                uc.mem_write(v, struct.pack('<I', S.FLOATS[self.ansv(ident, 1 + k) % len(S.FLOATS)])); k += 1
            elif ch == 'i':
                uc.mem_write(v, struct.pack('<I', self.ansv(ident, 1 + k) & 7)); k += 1
            elif ch == 'b':
                if v:
                    uc.mem_write(v, bytes([self.ansv(ident, 1 + k) & 1]))
                k += 1
            elif ch == 'O':
                uc.mem_write(v, struct.pack('<3I', *[S.FLOATS[self.ansv(ident, 1 + k + j) % len(S.FLOATS)] for j in range(3)])); k += 3
                eax = v
            elif ch == 'N':
                uc.mem_write(v, struct.pack('<I', S.NODES[self.ansv(ident, 1 + k) % len(S.NODES)])); k += 1
                eax = v
            else:
                raise ValueError(ch)
        # semantica especial
        if name == 'CollectParameters':
            for i in range(args[0]):
                self.w32(sp + 4 * i, self.case['params'][i])
        elif name == 'StoreParameters':
            ev.extend(self.r32(sp + 4 * i) for i in range(args[0]))
        # valor devuelto
        rk = c['ret']
        if rk == 'v':
            pass
        elif rk == 'w':
            eax = a0 & 0x7FFFF   # 19 bits: nunca se parece a un puntero a tabla virtual del EXE ni del ASI
        elif rk == 'b':
            eax = a0 & 1
        elif rk == 'n':
            eax = a0 % 8
        elif rk == 'f':
            floatret = S.FLOATS[a0 % len(S.FLOATS)]
        elif rk == 'z':
            eax = S.ZONES[a0 % len(S.ZONES)]
        elif rk == 't':
            eax = S.THINGS[a0 % len(S.THINGS)]
        elif rk.startswith('o:'):
            setname = rk[2:]
            if setname == 'TASKMEM':
                slot = self.next_slot
                if (self.case['na'] and a0 % 4 == 0) or slot >= S.NSLOT_C:
                    eax = 0
                else:
                    self.next_slot += 1
                    eax = (self.arena_base[2] + S.GUARD + S.SLOT_C * slot) & M32
            else:
                el = S.PTRSETS[setname][a0 % len(S.PTRSETS[setname])]
                eax = 0 if el is None else (el if isinstance(el, int) else self.resolve(el, 0))
        else:
            raise ValueError(rk)
        self.events.append(ev)
        self.cnt[ident] += 1
        # volver al llamador
        pop = 4 * len(letters) if c['conv'] in 'TS' else 0
        esp = esp + pop
        self.w32(esp, ret)
        uc.reg_write(UC_X86_REG_ESP, esp)
        uc.reg_write(UC_X86_REG_EAX, eax & M32)
        if floatret is not None:
            self.w32(FLD_SLOT, floatret)
            uc.reg_write(UC_X86_REG_EIP, FLD_STUB)
        else:
            uc.reg_write(UC_X86_REG_EIP, RET_STUB)

    def cstr(self, p):
        out = bytearray()
        for i in range(64):
            b = self.readbytes(p + i, 1)[0]
            if b == 0:
                break
            out.append(b)
        return bytes(out)

    # -------------------------------------------------------------------------------------------- ejecucion
    def run(self, inp, entry=None, cmd=None):
        uc = self.uc
        self.case = inp
        self.fail = None
        self.events = []
        self.cnt = {}
        self.next_slot = 0
        self.dirty.clear()
        self.setup(inp)
        # estado de CPU
        uc.reg_write(UC_X86_REG_FPCW, 0x007F)
        sp = STACK_HI - 0x1000
        # siembra de la pila bajo ESP (como Gate)
        seedv = inp.get('stack', 0) & M32
        s = 0
        for i in range(64):
            s = (s + 0x9E3779B1) & M32
            self.w32(sp - 4 * (i + 1), seedv ^ s)
        self.w32(sp, SENTINEL)
        self.w32(sp + 4, inp['cmd'])
        self.w32(TIB, 0xFFFFFFFF)
        script = (self.arena_base[0] + S.GUARD + S.SCRIPT) & M32
        for r, v in ((UC_X86_REG_ECX, script), (UC_X86_REG_EAX, inp.get('eax', 0)), (UC_X86_REG_EDX, inp.get('edx', 0)), (UC_X86_REG_EBX, 0x13579BDF), (UC_X86_REG_ESI, 0x2468ACE0),
                     (UC_X86_REG_EDI, 0x10293847), (UC_X86_REG_EBP, 0x55AA55AA), (UC_X86_REG_ESP, sp)):
            uc.reg_write(r, v)
        uc.reg_write(UC_X86_REG_EFLAGS, 0x202)
        fn = S.ENTRIES[inp['fn']]
        try:
            uc.emu_start(fn, SENTINEL, count=2000000)
        except UcError as e:
            self.fail = self.fail or ('UcError %s en eip=%#x' % (e, uc.reg_read(UC_X86_REG_EIP)))
        out = {'fail': self.fail}
        if not self.fail:
            if uc.reg_read(UC_X86_REG_EIP) != SENTINEL:
                out['fail'] = 'no llego al centinela: eip=%#x' % uc.reg_read(UC_X86_REG_EIP)
        out['esp_ok'] = uc.reg_read(UC_X86_REG_ESP) == sp + 8
        out['nonvolatile_ok'] = (uc.reg_read(UC_X86_REG_EBX) == 0x13579BDF and uc.reg_read(UC_X86_REG_ESI) == 0x2468ACE0 and uc.reg_read(UC_X86_REG_EDI) == 0x10293847 and uc.reg_read(UC_X86_REG_EBP) == 0x55AA55AA)
        out['seh_ok'] = self.r32(TIB) == 0xFFFFFFFF
        out['ret'] = uc.reg_read(UC_X86_REG_EAX) & 0xFF
        out['eax'] = uc.reg_read(UC_X86_REG_EAX)
        ch = {}
        for i, nm in enumerate(S.ARENA_NAMES):
            now = self.readbytes(self.arena_base[i], WINDOWS[i])
            if i == 2:
                now = norm_code_pointers(self.refs[i], now)
            ch[nm] = runs(self.refs[i], now, S.GUARD)
        out['changes'] = ch
        g = []
        for (addr, ref), (a2, n) in zip(self.gref, S.GLOBALS):
            now = self.readbytes(addr, n)
            for k in range(n):
                if now[k] != ref[k]:
                    g.append([addr + k, now[k]])
        out['gchanges'] = g
        out['events'] = self.events
        # restaurar las paginas del EXE tocadas
        img = image()
        for p in self.dirty:
            if img.base <= p < img.base + img.size:
                uc.mem_write(p, bytes(img.mem[p - img.base:p - img.base + 0x1000]))
        return out


EXE_RDATA = (0x858000, 0x8A4000)   # tablas virtuales del EXE (.rdata); en la DLL son direcciones del ASI: ambas se normalizan


def norm_code_pointers(ref, now):
    """arena de reservas de tareas: (1) los dwords alineados que cambian y apuntan a una tabla virtual (EXE .rdata; en la DLL, el ASI fuera de las arenas) se sustituyen por 0xFEEDF00D; (2) los bytes
    que quedan a 0x00 se consideran sin escribir (rellenos de alineacion: el EXE no los escribe y la DLL los pone a cero al inicializar el miembro)"""
    out = bytearray(now)
    for i in range(0, len(out) - 3, 4):
        d = struct.unpack_from('<I', out, i)[0]
        if d != struct.unpack_from('<I', ref, i)[0] and EXE_RDATA[0] <= d < EXE_RDATA[1]:
            struct.pack_into('<I', out, i, 0xFEEDF00D)
    for i in range(len(out)):
        if out[i] == 0:
            out[i] = ref[i]
    return bytes(out)


def runs(ref, now, guard):
    out = []
    i, n = 0, len(ref)
    while i < n:
        if ref[i] == now[i]:
            i += 1
            continue
        s = i
        while i < n and ref[i] != now[i]:
            i += 1
        out.append([s - guard, now[s:i].hex()])
    return out


_MACHINE = None


def machine():
    global _MACHINE
    if _MACHINE is None:
        _MACHINE = Machine()
        _MACHINE.keys = [0] * 8
    return _MACHINE


def uppercase_key(m, label):
    """GetUppercaseKey (0x53CF30, cdecl, un argumento) ejecutado desde los bytes del EXE"""
    uc = m.uc
    uc.mem_write(LABEL_SCRATCH, label.encode() + b'\x00')
    sp = STACK_HI - 0x1000
    m.w32(sp, SENTINEL)
    m.w32(sp + 4, LABEL_SCRATCH)
    uc.reg_write(UC_X86_REG_ESP, sp)
    uc.reg_write(UC_X86_REG_FPCW, 0x007F)
    uc.emu_start(0x53CF30, SENTINEL, count=100000)
    return uc.reg_read(UC_X86_REG_EAX) & M32


def expected(inp):
    """observables del EXE para un caso (diccionario `input` del banco)"""
    m = machine()
    if not any(m.keys):
        m.keys = [uppercase_key(m, S.LABELS[j]) for j in range(8)]
    return m.run(inp)


if __name__ == '__main__':
    inp = json.loads(sys.argv[1])
    print(json.dumps(expected(inp), indent=1)[:4000])
