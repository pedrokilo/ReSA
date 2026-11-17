"""Oraculo independiente (Python) de Fn221A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 25 funciones del lote 221A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial,
el MD5 (RFC 1321, con las constantes derivadas del seno) y lee del propio gta-sa.exe canonico los floats de los constructores y las tablas de 0x4024C0 y de 0x401FBB.
Sin x87 (los floats viajan como dwords).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh):
   0..11  constructores (0x5EE960, 0x5EEA30, 0x5EEB00, 0x5EEBD0) `ret 0Ch`: leen siete floats de .rdata y llaman a 0x5EDFB0 (efecto, entidad, estado, 1, siete floats); [this] = vtable de la clase; devuelven this.
          destructores con borrado (0x5EE9D0, 0x5EEAA0, 0x5EEB70, 0x5EEC40) `ret 4`: cuerpo (0x5EEA20 ...); si bit 0 de las banderas: borra en el pool 0xB744C4 (indice = (this - [pool]) / 0xC4 con signo: imul 0x5397829D;
          sar 6; + signo; estado[indice] |= 0x80; si indice < [pool + 0xC] (con signo): [pool + 0xC] = indice); devuelven this.
          cuerpos del destructor (0x5EEA20 ...): [this] = vtable de la clase; salta a 0x5EC410.
   12  0x5EF920  stdcall(peaton, atractor, lista) `ret 0Ch`: atractor nulo -> falso; inicio = [lista + 4] (una sola vez); cuenta = inicio ? ([lista + 8] - inicio) >> 2 (con signo) : 0; recorre desde el inicio buscando el atractor;
             si lo encuentra: CPedAttractor::BroadcastArrival 0x5EEF80 (this = atractor, peaton) y devuelve su AL; si no, AL = 0.
   13  0x5EF980  stdcall(peaton, efecto, entidad, estado, lista) `ret 14h` (marco SEH que solo desenrolla): busca en la lista el primer atractor con [+4] == efecto y [+8] == entidad (inicio leido una vez, cuenta con signo);
             si lo hay: RegisterPed 0x5EEE30 (this = atractor, peaton) y lo devuelve. Si no: tipo = BYTE [efecto + 0x34]; mayor que 9: devuelve nulo (sin tocar nada); si no, CPedAttractorPool::New 0x5EA9F0
             (ecx = [0xB744C4]; busca desde [pool+0xC]+1, con una vuelta; estado = ((estado & 0x7F) + 1) & 0x7F; devuelve [pool] + indice * 0xC4, o nulo con el cursor a 0 si esta lleno), constructor de la clase
             (0x5EE470, 0x5EE540, 0x5EE610, 0x5EE6E0, 0x5EE7C0, 0x5EE890, 0x5EE960, 0x5EEA30, 0x5EEB00, 0x5EEBD0; con el pool lleno no se llama); push_back de punteros 0x5EE400 (&local con el atractor, nulo si pool lleno)
             y, si el atractor es no nulo, RegisterPed. Devuelve el atractor.
   14  0x493870  destructor con borrado de CTaskComplexSeekEntity<RadiusAngleOffset> `ret 4`: cuerpo 0x493890; si bit 0 del BYTE de banderas: CTask::operator delete 0x61A5B0 (this); devuelve this.
   15  0x493960  cdecl(entidad, int32* salida): [salida] = -1; posicion = matriz ([+0x14]) ? [matriz + 0x30..0x38] : [+4..+0xC]; para cada uno de los 8 focos (registro de 0x7C bytes desde 0xA94D68; activo = BYTE +0):
             IsPointWithinSearchLight 0x493280 (posicion local, indice); si da AL != 0: [salida] = (WORD +4) << 16 | indice, AL = 1. Si ninguno, AL = 0.
   16  0x496DC0  thiscall(salida, peaton) `ret 8`: calculador virtual ranura 0 en this+0x40 con (peaton, [this+0xC], temporal local); copia el temporal a la salida; devuelve la salida.
   17  0x4993D0  thiscall(estado, dir, tarea) `ret 0Ch`: CTaskComplexWander 0x66F450 (estado, dir, 1, 0.5f de 0x86FCB0); pone a cero +0x2C, +0x30, +0x34 (BYTE), +0x35 (BYTE), +0x38..+0x48 (dwords), +0x4C, +0x4D (BYTES);
             [+0x28] = tarea; [this] = 0x85A428; devuelve this.
   18  0x401000  cdecl(ctx): [+4] = 0, [+0] = [+4], estado 67452301 EFCDAB89 98BADCFE 10325476 en +8..+0x14.
   19  0x40104A  cdecl(ctx, entrada, longitud): MD5Update (ver md5_update()).
   20  0x40118B  cdecl(ctx): MD5Final (ver expected()).
   21  0x401FBB  cdecl(): 41 veces GetProcAddress (0x81E40C: salto a la IAT) con modulo de 0xC9E408 / 0xC9E410, nombre y destino leidos del propio codigo del EXE; despues llama a 0x402370.
   22  0x402370  cdecl(): ver expected().
   23  0x4024C0  cdecl(): 58 pares (mov eax, valor; mov [destino], eax) leidos del propio codigo del EXE.
   24  0x4028A2  cdecl() -> 0/1: ver expected().

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CPedLookAtAttractor::Constructor", "CPedLookAtAttractor::ScalarDeletingDestructor", "CPedLookAtAttractor::Destructor",
         "CPedScriptedAttractor::Constructor", "CPedScriptedAttractor::ScalarDeletingDestructor", "CPedScriptedAttractor::Destructor",
         "CPedParkAttractor::Constructor", "CPedParkAttractor::ScalarDeletingDestructor", "CPedParkAttractor::Destructor",
         "CPedStepAttractor::Constructor", "CPedStepAttractor::ScalarDeletingDestructor", "CPedStepAttractor::Destructor",
         "CPedAttractorManager::BroadcastArrivalList", "CPedAttractorManager::RegisterPedList",
         "CTaskComplexSeekEntityRadiusAngleOffset::ScalarDeletingDestructor", "CTheScripts::IsEntityWithinAnySearchLight", "CTaskComplexSeekEntityRadiusAngleOffset::GetSeekPos", "CTaskComplexBeCop::Constructor",
         "ExeProtectionStub::MD5Init", "ExeProtectionStub::MD5Update", "ExeProtectionStub::MD5Final",
         "ExeProtectionStub::ResolveKernelImports", "ExeProtectionStub::ResolveLoaderImports", "ExeProtectionStub::InitCallbackTable", "ExeProtectionStub::CheckOsVersionSuite"]
ENTRIES = (0x5EE960, 0x5EE9D0, 0x5EEA20, 0x5EEA30, 0x5EEAA0, 0x5EEAF0, 0x5EEB00, 0x5EEB70, 0x5EEBC0, 0x5EEBD0, 0x5EEC40, 0x5EEC90,
           0x5EF920, 0x5EF980, 0x493870, 0x493960, 0x496DC0, 0x4993D0, 0x401000, 0x40104A, 0x40118B, 0x401FBB, 0x402370, 0x4024C0, 0x4028A2)
assert len(NAMES) == len(ENTRIES) == 25
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_FAKES = (0x31000000, 0x31000010, 0x31000020, 0x31000030, 0x31000040, 0x31000050, 0x31000060, 0x31000070, 0x31000080, 0x31000090)
(ACTOR, ASDD, ABODY, BCAST, REG, SEEKSDD, LIGHT, SEEKPOS, BECOP, MD5INIT, MD5UPD, MD5FIN, RESK, RESL, CBTAB, OSVER) = \
    'actor asdd abody bcast reg seeksdd light seekpos becop md5init md5upd md5fin resk resl cbtab osver'.split()
KIND = [ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY, BCAST, REG, SEEKSDD, LIGHT, SEEKPOS, BECOP, MD5INIT, MD5UPD, MD5FIN, RESK, RESL, CBTAB, OSVER]
CLS = {0: 6, 1: 6, 2: 6, 3: 7, 4: 7, 5: 7, 6: 8, 7: 8, 8: 8, 9: 9, 10: 9, 11: 9}
# vtable del EXE de cada clase (de los constructores `mov [esi], imm32`) y maximo de peatones (`push 1` / `push 5`)
CLS_VT = (0x86C550, 0x86C568, 0x86C580, 0x86C598, 0x86C5B4, 0x86C5CC, 0x86C5E4, 0x86C5FC, 0x86C614, 0x86C62C)
CLS_MAXPEDS = (5, 1, 5, 5, 5, 1, 1, 1, 1, 1)
# direccion de .rdata de los siete floats de cada constructor (los lee de ahi el codigo del EXE: `mov reg, [abs]`)
CLS_FLOAT_AT = (0x86C3EC, 0x86C40C, 0x86C42C, 0x86C44C, 0x86C470, 0x86C490, 0x86C4B0, 0x86C4D0, 0x86C4F0, 0x86C510)
EXE_PATH = 'D:/Grand Theft Auto San Andreas/gta-sa.exe'
STACK_TAG = 0x7F000000
EV_BASECTOR, EV_BASEDTOR = 0x5EDFB0, 0x5EC410
EV_BROADCAST, EV_REGPED, EV_PUSH, EV_TASKDEL, EV_SEEKBODY, EV_SEARCH, EV_WANDER, EV_LOADER, EV_GPA = 0x5EEF80, 0x5EEE30, 0x5EE400, 0x61A5B0, 0x493890, 0x493280, 0x66F450, 0x402370, 0x81E40C
CALC_EVENT = 0xFA11CA1C
ATTRPOOL = 0xB744C4
ATTR_MAGIC = 0x5397829D
kAttractorSize = 0xC4
PROT = 0xC9E000
PROT_BYTES = 0xA00
SEED_ALIAS = (0x00, 0x08, 0x0C, 0x38, 0x40)       # desplazamientos de la salida de GetSeekPos respecto a this (modos 1..5)


# ---------------------------------------------------------------------------------------------- imagen del EXE canonico (solo lectura)
class Exe:
    def __init__(self, path=EXE_PATH):
        self.data = open(path, 'rb').read()
        e = struct.unpack_from('<I', self.data, 0x3C)[0]
        nsec = struct.unpack_from('<H', self.data, e + 6)[0]
        opt = struct.unpack_from('<H', self.data, e + 20)[0]
        self.base = struct.unpack_from('<I', self.data, e + 24 + 28)[0]
        self.secs = []
        for i in range(nsec):
            _, vs, va, rs, ro = struct.unpack_from('<8sIIII', self.data, e + 24 + opt + i * 40)
            self.secs.append((va + self.base, vs, rs, ro))

    def rd(self, va, n):
        for v, vs, rs, ro in self.secs:
            if v <= va < v + max(vs, rs):
                return self.data[ro + va - v: ro + va - v + n] if va - v < rs else bytes(n)
        raise ValueError('direccion fuera de la imagen: %x' % va)

    def u32(self, va):
        return struct.unpack('<I', self.rd(va, 4))[0]


_TABLES = None


def tables():
    """floats de cada constructor, tablas de 0x401FBB / 0x4024C0 y el radio de 0x86FCB0, leidos del codigo y los datos del EXE canonico"""
    global _TABLES
    if _TABLES is None:
        exe = Exe()
        floats = [[exe.u32(at + 4 * i) for i in range(7)] for at in CLS_FLOAT_AT]
        # 0x401FBB: 41 x [lea esi, [nombre]; push esi; mov eax, [modulo]; push eax; call GetProcAddress; mov [destino], eax]
        code = exe.rd(0x401FBB, 0x402370 - 0x401FBB)
        ent, i = [], 0
        while i + 23 <= len(code) and code[i] == 0x8D and code[i + 1] == 0x35 and code[i + 6] == 0x56 and code[i + 7] == 0xA1 and code[i + 12] == 0x50 and code[i + 13] == 0xE8 and code[i + 18] == 0xA3:
            name, mod, dst = struct.unpack_from('<I', code, i + 2)[0], struct.unpack_from('<I', code, i + 8)[0], struct.unpack_from('<I', code, i + 19)[0]
            rel = struct.unpack_from('<i', code, i + 14)[0]
            assert (0x401FBB + i + 18 + rel) & M32 == EV_GPA
            ent.append((mod, name, dst))
            i += 23
        assert len(ent) == 41 and code[i:i + 6] == bytes.fromhex('e801000000c3'), (len(ent), code[i:i + 8].hex())   # call 0x402370; ret
        # 0x4024C0: 58 x [mov eax, valor; mov [destino], eax]
        code = exe.rd(0x4024C0, 0x4028A2 - 0x4024C0)
        st, i = [], 0
        while i + 10 <= len(code) and code[i] == 0xB8 and code[i + 5] == 0xA3:
            st.append((struct.unpack_from('<I', code, i + 6)[0], struct.unpack_from('<I', code, i + 1)[0]))
            i += 10
        assert len(st) == 58 and code[i] == 0xC3
        _TABLES = (floats, ent, st, exe.u32(0x86FCB0))
    return _TABLES


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, guard=GUARD):
        self.kind, self.seed, self.guard, self.m, self.r = kind, seed, guard, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(self.guard + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[self.guard + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self, skip=(), absolute=0):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - self.guard
            if self.m[i] != ref and not any(s <= rel < s + 4 for s in skip):
                out.append([rel + absolute, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba), la copia de los datos del bloque protector y la memoria global: lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
        self.prot = Mem(a[2], a[90], guard=0)
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        addr &= M32
        if PROT <= addr < PROT + PROT_BYTES:
            return self.prot.rd(addr - PROT, n)
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        addr &= M32
        if PROT <= addr < PROT + PROT_BYTES:
            self.prot.wr(addr - PROT, n, v)
            return
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        for m in self.mem:
            m.snap()
        self.prot.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order, prot=False):
        """cambios de las regiones de la funcion tras la llamada respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
        if prot:
            return self.prot.changes(absolute=PROT)
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


def sar2(x):
    """cociente con `sar 2` de una diferencia de 32 bits"""
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    return (x >> 2) & M32


def pool_index(diff, magic):
    """operator delete en linea del EXE: (this - [pool]) / tamano con la constante magica: imul; sar edx,6; + bit de signo"""
    d = signed(diff)
    hi = (d * magic) >> 32
    q = hi >> 6
    q += 1 if q < 0 else 0
    return q


# ---------------------------------------------------------------------------------------------- MD5 (RFC 1321)
_K = [int(abs(math.sin(i + 1)) * 2 ** 32) & M32 for i in range(64)]
_S = [7, 12, 17, 22] * 4 + [5, 9, 14, 20] * 4 + [4, 11, 16, 23] * 4 + [6, 10, 15, 21] * 4


def md5_transform(state, block):
    a, b, c, d = state
    for i in range(64):
        if i < 16:
            f, g = (b & c) | (~b & d), i
        elif i < 32:
            f, g = (d & b) | (~d & c), (5 * i + 1) % 16
        elif i < 48:
            f, g = b ^ c ^ d, (3 * i + 5) % 16
        else:
            f, g = c ^ (b | (~d & M32)), (7 * i) % 16
        f = (f + a + _K[i] + block[g]) & M32
        a, d, c = d, c, b
        b = (b + (((f << _S[i]) | (f >> (32 - _S[i]))) & M32)) & M32
    return [(state[0] + a) & M32, (state[1] + b) & M32, (state[2] + c) & M32, (state[3] + d) & M32]


def fnv_str(rdb, addr, mx=24):
    h = 2166136261
    for i in range(mx):
        b = rdb(addr + i)
        h ^= b
        h = (h * 16777619) & M32
        if b == 0:
            break
    return h


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
THIS = ['this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    ACTOR: THIS + ['state_zero', 'state_other', 'fx_zero', 'entity_zero'],
    ASDD: THIS + ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'idx_zero', 'idx_positive', 'lowers_first_free', 'keeps_first_free', 'rem_nonzero'],
    ABODY: THIS + ['write_zero', 'write_other'],
    BCAST: THIS + ['att_null', 'found', 'not_found', 'first_null', 'match_first', 'match_later', 'count_nonpositive', 'ret_true', 'ret_false'],
    REG: THIS + ['found', 'not_found', 'type_invalid', 'pool_full', 'pool_ok', 'first_null', 'count_nonpositive', 'match_later', 'type0', 'type1', 'type2', 'type3', 'type4', 'type5', 'type6', 'type7', 'type8', 'type9', 'wraps'],
    SEEKSDD: THIS + ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    LIGHT: THIS + ['matrix', 'no_matrix', 'hit', 'miss', 'inactive_skipped', 'hit_first', 'hit_later', 'nan_component'],
    SEEKPOS: THIS + ['out_in_b', 'out_alias', 'snan_component', 'nan_component', 'entity_null', 'ped_null'],
    BECOP: THIS + ['dir_zero', 'dir_high', 'state_zero', 'state_other'],
    MD5INIT: THIS,
    MD5UPD: THIS + ['idx_zero', 'idx_nonzero', 'len_zero', 'direct_path', 'byte_path', 'block_completed', 'carry_low', 'input_unaligned', 'len_big'],
    MD5FIN: THIS + ['idx_lt_56', 'idx_ge_56', 'pad_standard', 'pad_random', 'idx_zero'],
    RESK: ['null_results', 'nonnull_results', 'module_zero'],
    RESL: ['first_fail', 'first_ok_second_ok_mode1', 'mode1_proc1_null', 'mode1_proc2_null', 'mode1_module_null', 'mode1_second_neg', 'mode2', 'mode0', 'flag_cleared'],
    CBTAB: [],
    OSVER: ['major_lt5', 'major_ge5', 'suite_set', 'suite_clear', 'open_fail', 'query1_fail', 'alloc_null', 'query2_fail', 'matched', 'not_matched', 'empty_prefix', 'closed_key', 'freed_buffer'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def md5_update(A, o, input_byte, inp, length, marks):
    """MD5Update del EXE (0x40104A) sobre el contexto de la arena A en el desplazamiento o; input_byte(direccion) lee la entrada. Devuelve la lista de bloques de la transformacion (informativo)"""
    lo, hi = A.rd(o, 4), A.rd(o + 4, 4)
    idx = (lo >> 3) & 0x3F
    if ((lo + length * 8) & M32) < lo:
        hi = (hi + 1) & M32
        marks.append('carry_low')
        A.wr(o + 4, 4, hi)
    lo = (lo + (length << 3)) & M32
    A.wr(o, 4, lo)
    A.wr(o + 4, 4, (A.rd(o + 4, 4) + (length >> 29)) & M32)
    state = [A.rd(o + 8 + 4 * i, 4) for i in range(4)]
    if idx == 0 and (inp & 3) == 0 and length >= 0x40:
        marks.append('direct_path')
    while idx == 0 and (inp & 3) == 0 and length >= 0x40:
        block = [sum(input_byte(inp + 4 * i + k) << (8 * k) for k in range(4)) for i in range(16)]
        state = md5_transform(state, block)
        length = (length - 0x40) & M32
        inp = (inp + 0x40) & M32
    first_byte_path = length != 0
    if first_byte_path:
        marks.append('byte_path')
    while True:
        saved = length
        length = (length - 1) & M32
        if saved == 0:
            break
        A.wr(o + 0x18 + idx, 1, input_byte(inp))
        inp = (inp + 1) & M32
        idx += 1
        if idx == 0x40:
            block = [sum(A.rd(o + 0x18 + 4 * i + k, 1) << (8 * k) for k in range(4)) for i in range(16)]
            state = md5_transform(state, block)
            idx = 0
            marks.append('block_completed')
    for i in range(4):
        A.wr(o + 8 + 4 * i, 4, state[i])


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    floats, ent, stores, wander_radius = tables()
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    if k not in (RESK, RESL, CBTAB, OSVER):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    W = World(c)
    A, B, Cc, D = W.mem
    P = W.prot
    fakes = [x & M32 for x in (c.get('fakes') or DEFAULT_FAKES)]
    events, ret, threw, vptrs = [], None, False, []
    gorder, prot = [], False
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tg = W.tagv

    if k == ACTOR:
        cls = CLS[fn]
        W.snap()
        events.append([EV_BASECTOR, tg(this), a[4], a[5], a[6], CLS_MAXPEDS[cls]] + list(floats[cls]))
        vptrs.append([0, o, cls])
        ret = tg(this)
        marks.append('state_zero' if a[6] == 0 else 'state_other')
        if a[4] == 0:
            marks.append('fx_zero')
        if a[5] == 0:
            marks.append('entity_zero')
    elif k == ASDD:
        cls = CLS[fn]
        idx, rem = a[9], a[10]
        diff = idx * kAttractorSize + rem
        objects = (this - diff) & M32
        pool = a[7]
        B.wr(pool + 0, 4, objects); B.wr(pool + 4, 4, W.raw(3, a[8])); B.wr(pool + 8, 4, 0x40); B.wr(pool + 0xC, 4, a[11])
        D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) & 0x7F)       # el hueco esta usado
        W.setg(ATTRPOOL, 4, W.raw(1, pool))
        gorder = [(ATTRPOOL, 4)]
        W.snap()
        events.append([EV_BASEDTOR, tg(this), CLS_VT[cls]])     # el cuerpo de la clase escribio su vtable del EXE antes de saltar a 0x5EC410
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            index = pool_index((this - B.rd(pool, 4)) & M32, ATTR_MAGIC)
            state_off = a[8] + index
            D.wr(state_off, 1, D.rd(state_off, 1) | 0x80)
            first = signed(B.rd(pool + 0xC, 4))
            if index < first:
                B.wr(pool + 0xC, 4, index & M32)
                marks.append('lowers_first_free')
            else:
                marks.append('keeps_first_free')
            marks.append('idx_zero' if index == 0 else 'idx_positive')
            if rem:
                marks.append('rem_nonzero')
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tg(this)
    elif k == ABODY:
        cls = CLS[fn]
        W.snap()
        events.append([EV_BASEDTOR, tg(this), CLS_VT[cls]])
        A.wr(o, 4, a[6])
        marks.append('write_zero' if a[6] == 0 else 'write_other')
    elif k == BCAST:
        first = 0 if a[4] == 0 else W.raw(1, a[6])
        last = a[24] if first == 0 else (first + 4 * a[5]) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, a[23])
        if first:
            for i in range(12):
                B.wr(a[6] + 4 * i, 4, a[11 + i])
        att = 0 if a[8] == 0 else (a[11 + (a[9] % 12)] if a[8] == 1 else a[25])
        W.snap()
        ret = 0
        if att == 0:
            marks.append('att_null')
        else:
            cnt = 0 if first == 0 else signed(sar2((last - first) & M32))
            if first == 0:
                marks.append('first_null')
            if cnt <= 0:
                marks.append('count_nonpositive')
            found = False
            for i in range(max(cnt, 0)):
                if W.rd((first + 4 * i) & M32) == att:
                    events.append([EV_BROADCAST, tg(att), a[7]])
                    ret = a[10] & 0xFF
                    found = True
                    marks.append('match_first' if i == 0 else 'match_later')
                    break
            marks.append('found' if found else 'not_found')
            marks.append('ret_true' if ret else 'ret_false')
    elif k == REG:
        first = 0 if a[4] == 0 else W.raw(1, a[6])
        last = a[15] if first == 0 else (first + 4 * a[5]) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, a[14])
        fx = W.raw(1, a[7])
        B.wr(a[7] + 0x34, 1, a[8])
        for i in range(8):
            rec = a[12] + 0x20 * i
            B.wr(a[6] + 4 * i, 4, W.raw(2, rec))
            Cc.wr(rec + 4, 4, fx if a[16 + i] & 1 else a[24 + i])
            Cc.wr(rec + 8, 4, a[9] if a[16 + i] & 2 else a[32 + i])
        psz = a[40]
        D.wr(a[41] + 0, 4, W.raw(3, a[42])); D.wr(a[41] + 4, 4, W.raw(3, a[41] + 0x10)); D.wr(a[41] + 8, 4, psz); D.wr(a[41] + 0xC, 4, a[43])
        for i in range(psz):
            D.wr(a[41] + 0x10 + i, 1, a[44 + i])
        W.setg(ATTRPOOL, 4, W.raw(3, a[41]))
        gorder = [(ATTRPOOL, 4)]
        W.snap()
        ped, entity, state = a[10], a[9], a[11]
        cnt = 0 if first == 0 else signed(sar2((last - first) & M32))
        if first == 0:
            marks.append('first_null')
        if cnt <= 0:
            marks.append('count_nonpositive')
        result = 0
        found = False
        for i in range(max(cnt, 0)):
            rec = W.rd((first + 4 * i) & M32)
            if W.rd(rec + 4) == fx and W.rd(rec + 8) == entity:
                result, found = rec, True
                if i > 0:
                    marks.append('match_later')
                break
        if found:
            marks.append('found')
            events.append([EV_REGPED, tg(result), ped])
        else:
            marks.append('not_found')
            t = B.rd(a[7] + 0x34, 1)
            if t > 9:
                marks.append('type_invalid')
            else:
                marks.append('type%d' % t)
                # CPedAttractorPool::New (0x5EA9F0)
                size = psz
                ff = D.rd(a[41] + 0xC, 4)
                lap = 0
                slot = None
                for _ in range(100000):
                    ff = (ff + 1) & M32
                    if ff == size:
                        ff = 0
                        D.wr(a[41] + 0xC, 4, 0)
                        if lap:
                            break
                        lap = 1
                        marks.append('wraps')
                    else:
                        D.wr(a[41] + 0xC, 4, ff)
                    st = D.rd(a[41] + 0x10 + ff, 1)
                    if st & 0x80:
                        slot = ff
                        break
                else:
                    raise RuntimeError('el pool no termina')
                if slot is None:
                    marks.append('pool_full')
                    result = 0
                else:
                    marks.append('pool_ok')
                    off = a[41] + 0x10 + slot
                    st = D.rd(off, 1)
                    D.wr(off, 1, st & 0x7F)
                    D.wr(off, 1, (((st & 0x7F) + 1) & 0x7F))
                    result = (W.raw(3, a[42]) + slot * kAttractorSize) & M32
                    events.append([EV_BASECTOR, tg(result), fx, entity, state, CLS_MAXPEDS[t]] + list(floats[t]))
                    vptrs.append([3, a[42] + kAttractorSize * slot, t])
                events.append([EV_PUSH, tg(this), STACK_TAG, tg(result)])
                if result:
                    events.append([EV_REGPED, tg(result), ped])
        ret = tg(result)
    elif k == SEEKSDD:
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([EV_SEEKBODY, tg(this), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([EV_TASKDEL, tg(this), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tg(this)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == LIGHT:
        A.wr(o + 4, 4, a[6]); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        if a[4] == 0:
            A.wr(o + 0x14, 4, 0)
        else:
            A.wr(o + 0x14, 4, W.raw(1, a[5]))
            B.wr(a[5] + 0x30, 4, a[9]); B.wr(a[5] + 0x34, 4, a[10]); B.wr(a[5] + 0x38, 4, a[11])
        Cc.wr(a[12], 4, a[13])
        W.snap()
        Cc.wr(a[12], 4, M32)
        pos = [a[9], a[10], a[11]] if a[4] else [a[6], a[7], a[8]]
        marks.append('matrix' if a[4] else 'no_matrix')
        for w in pos:
            if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
                marks.append('nan_component')
        ret = 0
        mask = a[14] & 0xFF
        for i in range(8):
            if a[16 + i] & 0xFF:
                events.append([EV_SEARCH] + pos + [i])
                if (mask >> i) & 1:
                    Cc.wr(a[12], 4, ((a[24 + i] & 0xFFFF) << 16) | i)
                    ret = 1
                    marks.append('hit_first' if not any(a[16 + j] & 0xFF for j in range(i)) else 'hit_later')
                    break
            else:
                marks.append('inactive_skipped')
        marks.append('hit' if ret else 'miss')
    elif k == SEEKPOS:
        A.wr(o + 0xC, 4, a[3])
        A.wr(o + 0x40, 4, fakes[9])
        mode = a[8]
        out = W.raw(1, a[9]) if mode == 0 else W.raw(0, o + SEED_ALIAS[mode - 1])
        W.snap()
        marks.append('out_in_b' if mode == 0 else 'out_alias')
        events.append([CALC_EVENT, tg(this + 0x40), a[4], a[3], 0x7F000000])
        for i in range(3):
            W.wr((out + 4 * i) & M32, 4, a[5 + i])
        ret = tg(out)
        for i in range(3):
            w = a[5 + i]
            if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
                marks.append('snan_component' if not (w & 0x400000) else 'nan_component')
        if a[3] == 0:
            marks.append('entity_null')
        if a[4] == 0:
            marks.append('ped_null')
    elif k == BECOP:
        W.snap()
        events.append([EV_WANDER, tg(this), a[4], a[5], 1, wander_radius])
        for off, n in ((0x2C, 4), (0x30, 4), (0x34, 1), (0x35, 1), (0x38, 4), (0x3C, 4), (0x40, 4), (0x44, 4), (0x48, 4), (0x4C, 1), (0x4D, 1)):
            A.wr(o + off, n, 0)
        A.wr(o + 0x28, 4, a[6])
        vptrs.append([0, o, 100])
        ret = tg(this)
        if a[5] == 0:
            marks.append('dir_zero')
        elif a[5] >= 0x80:
            marks.append('dir_high')
        marks.append('state_zero' if a[4] == 0 else 'state_other')
    elif k == MD5INIT:
        W.snap()
        A.wr(o + 4, 4, 0)
        A.wr(o, 4, A.rd(o + 4, 4))
        for i, v in enumerate((0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476)):
            A.wr(o + 8 + 4 * i, 4, v)
    elif k == MD5UPD:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4])
        for i in range(4):
            A.wr(o + 8 + 4 * i, 4, a[5 + i])
        W.snap()
        inp = W.raw(2, a[9])
        length = a[10]
        idx = (a[3] >> 3) & 0x3F
        marks.append('idx_zero' if idx == 0 else 'idx_nonzero')
        if length == 0:
            marks.append('len_zero')
        if inp & 3:
            marks.append('input_unaligned')
        if length >= 512:
            marks.append('len_big')
        md5_update(A, o, lambda ad: W.rd(ad, 1), inp, length, marks)
    elif k == MD5FIN:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4])
        for i in range(4):
            A.wr(o + 8 + 4 * i, 4, a[5 + i])
        for i in range(64):
            P.wr(0x68 + i, 1, (0x80 if i == 0 else 0) if a[25] == 0 else ((a[9 + i // 4] >> (8 * (i % 4))) & 0xFF))
        prot, gorder = True, [(PROT, PROT_BYTES)]
        W.snap()
        bits_lo, bits_hi = a[3], a[4]
        idx = (bits_lo >> 3) & 0x3F
        if idx == 0:
            marks.append('idx_zero')
        marks.append('idx_ge_56' if idx >= 0x38 else 'idx_lt_56')
        marks.append('pad_standard' if a[25] == 0 else 'pad_random')
        pad_len = 0x78 - idx if idx >= 0x38 else 0x38 - idx
        md5_update(A, o, lambda ad: W.rd(ad, 1), 0xC9E068, pad_len, [])
        block = [sum(A.rd(o + 0x18 + 4 * i + kk, 1) << (8 * kk) for kk in range(4)) for i in range(14)] + [bits_lo, bits_hi]
        state = md5_transform([A.rd(o + 8 + 4 * i, 4) for i in range(4)], block)
        for i in range(4):
            A.wr(o + 8 + 4 * i, 4, state[i])
            for kk in range(4):
                A.wr(o + 0x58 + 4 * i + kk, 1, (state[i] >> (8 * kk)) & 0xFF)
    elif k in (RESK, RESL, CBTAB, OSVER):
        prot, gorder = True, [(PROT, PROT_BYTES)]
        if k == RESK:
            P.wr(0x408, 4, a[3]); P.wr(0x410, 4, a[4])
            gpa = [0 if rnd(a[5], i) % 100 < a[6] else (rnd(a[5], i + 500) | 0x10000) for i in range(64)]
            W.snap()
            for n, (mod, name, dst) in enumerate(ent):
                module = W.rd(mod)
                events.append([EV_GPA, module, name, fnv_str(lambda ad: W.rd(ad, 1), name)])
                W.wr(dst, 4, gpa[n])
                marks.append('null_results' if gpa[n] == 0 else 'nonnull_results')
                if module == 0:
                    marks.append('module_zero')
            events.append([EV_LOADER])
        elif k == RESL:
            P.wr(0x408, 4, a[3])
            P.wr(0x5CF, 4, fakes[1]); P.wr(0x1F3, 4, fakes[2])
            r3 = [a[4], a[5], a[6]]
            r1 = [a[7], a[8]]
            gp = [a[9 + i] for i in range(6)]
            W.snap()
            st = {'g': 0, 'c3': 0, 'c1': 0}

            def gpa_call(module, name):
                events.append([EV_GPA, module, name, fnv_str(lambda ad: W.rd(ad, 1), name)])
                v = gp[st['g']]
                st['g'] += 1
                return v

            def call3(x, y, z):
                events.append([0xC9E5CF, x, y, z])
                v = r3[st['c3']]
                st['c3'] += 1
                return v

            def call1(x):
                events.append([0xC9E1F3, x])
                v = r1[st['c1']]
                st['c1'] += 1
                return v

            W.wr(0xC9E82D, 4, gpa_call(W.rd(0xC9E408), 0xC9E818))
            W.wr(0xC9E0F5, 4, 1)
            r = call3(0xC9E0E1, 0xC9E333, 0x4000)
            if r == M32:
                W.wr(0xC9E0F5, 4, 0)
                marks.append('flag_cleared')
            done = False
            if W.rd(0xC9E0F5) != 0:
                r = call3(0xC9E0CA, 0xC9E333, 0x4000)
                if r != M32:
                    m = call1(0xC9E0CA)
                    W.wr(0xC9E0ED, 4, m)
                    if m != 0:
                        p = gpa_call(m, 0xC9E0F9)
                        if p != 0:
                            W.wr(0xC9E14B, 4, p)
                            p = gpa_call(W.rd(0xC9E0ED), 0xC9E130)
                            if p != 0:
                                W.wr(0xC9E14F, 4, p)
                                W.wr(0xC9E153, 4, 1)
                                marks.append('first_ok_second_ok_mode1')
                                done = True
                            else:
                                marks.append('mode1_proc2_null')
                        else:
                            marks.append('mode1_proc1_null')
                    else:
                        marks.append('mode1_module_null')
                else:
                    marks.append('mode1_second_neg')
            else:
                marks.append('first_fail')
            if not done:
                r = call3(0xC9E0D7, 0xC9E333, 0x4000)
                ok2 = False
                if r != M32:
                    m = call1(0xC9E0D7)
                    W.wr(0xC9E0F1, 4, m)
                    if m != 0:
                        p = gpa_call(m, 0xC9E115)
                        if p != 0:
                            W.wr(0xC9E14B, 4, p)
                            p = gpa_call(W.rd(0xC9E0F1), 0xC9E13E)
                            if p != 0:
                                W.wr(0xC9E14F, 4, p)
                                W.wr(0xC9E153, 4, 2)
                                marks.append('mode2')
                                ok2 = True
                if not ok2:
                    W.wr(0xC9E153, 4, 0)
                    marks.append('mode0')
        elif k == CBTAB:
            W.snap()
            for dst, val in stores:
                W.wr(dst, 4, val)
        else:   # OSVER
            for pos, fk in zip((0xC9E61B, 0xC9E83D, 0xC9E862, 0xC9E532, 0xC9E541, 0xC9E84D), fakes[3:9]):
                P.wr(pos - PROT, 4, fk)
            for i in range(4):
                P.wr(0x1A8 + 4 * i, 4, a[32 + i])
            listimg = bytes((a[16 + i // 4] >> (8 * (i % 4))) & 0xFF for i in range(64))
            W.snap()
            alloc_ptr = W.raw(2, a[9]) if a[7] else 0

            def ver_call(p):
                size = W.rd(p)
                events.append([0xC9E61B, p, size])
                W.wr(p + 4, 4, a[3])
                if size == 0x9C:
                    W.wr(p + 0x98, 2, a[4] & 0xFFFF)

            ver_call_p = 0xC9E1FB
            W.wr(ver_call_p, 4, 0x94)
            ver_call(ver_call_p)
            ret = None
            if W.rd(0xC9E1FF) >= 5:
                marks.append('major_ge5')
                W.wr(0xC9E28F, 4, 0x9C)
                ver_call(0xC9E28F)
                mask = W.rd(0xC9E28F + 0x98, 2) & 0x10
                W.wr(0xC9E28F + 0x98, 2, mask)
                ret = 1 if mask else 0
                marks.append('suite_set' if mask else 'suite_clear')
            else:
                marks.append('major_lt5')
                W.wr(0xC9E157, 4, 0)
                events.append([0xC9E83D, 0x80000002, 0xC9E15B, 0xC9E157])
                W.wr(0xC9E157, 4, a[12])
                if a[5] != 0:
                    W.wr(0xC9E1BC, 4, 0)
                    marks.append('open_fail')
                    ret = 0
                else:
                    key = W.rd(0xC9E157)
                    events.append([0xC9E862, key, 0xC9E19B, 0, 0xC9E197, 0, 0xC9E193, W.rd(0xC9E193)])
                    W.wr(0xC9E197, 4, 7)
                    W.wr(0xC9E193, 4, a[15])
                    need_free = False
                    if a[6] != 0:
                        W.wr(0xC9E1BC, 4, 0)
                        marks.append('query1_fail')
                    else:
                        events.append([0xC9E532, 0x40, W.rd(0xC9E193)])
                        W.wr(0xC9E1B8, 4, alloc_ptr)
                        if alloc_ptr == 0:
                            W.wr(0xC9E1BC, 4, 0)
                            marks.append('alloc_null')
                        else:
                            need_free = True
                            key = W.rd(0xC9E157)
                            events.append([0xC9E862, key, 0xC9E19B, 0, 0xC9E197, tg(W.rd(0xC9E1B8)), 0xC9E193, W.rd(0xC9E193)])
                            W.wr(0xC9E197, 4, 7)
                            for i in range(64):
                                W.wr(alloc_ptr + i, 1, listimg[i])
                            if a[8] != 0:
                                W.wr(0xC9E1BC, 4, 0)
                                marks.append('query2_fail')
                            else:
                                entry = W.rd(0xC9E1B8)
                                matched = False
                                for _ in range(100):
                                    ln = 0
                                    while W.rd(entry + ln, 1) != 0:
                                        ln += 1
                                    same = all(W.rd(entry + i, 1) == W.rd(0xC9E1A8 + i, 1) for i in range(ln))
                                    if same:
                                        if ln == 0:
                                            marks.append('empty_prefix')
                                        matched = True
                                        break
                                    entry += ln + 1
                                    if W.rd(entry, 1) == 0:
                                        break
                                W.wr(0xC9E1BC, 4, 1 if matched else 0)
                                marks.append('matched' if matched else 'not_matched')
                    if need_free:
                        events.append([0xC9E541, tg(W.rd(0xC9E1B8))])
                        marks.append('freed_buffer')
                    if W.rd(0xC9E157) != 0:
                        events.append([0xC9E84D, W.rd(0xC9E157)])
                        marks.append('closed_key')
                    ret = 1 if W.rd(0xC9E1BC) != 0 else 0
    else:
        raise RuntimeError(k)
    if threw:
        ret = None
    skip = [[], [], [], []]
    for ar, off, _ in vptrs:
        skip[ar].append(off)
    return {'events': events, 'changes': {'A': A.changes(skip[0]), 'B': B.changes(skip[1]), 'C': Cc.changes(skip[2]), 'D': D.changes(skip[3])}, 'vptrs': vptrs,
            'gchanges': W.gchanges(gorder, prot), 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) y AL (bool)"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
