"""Oraculo independiente (Python) de Fn186A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 18 funciones del lote 186A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; constantes y cadenas leidas del propio EXE).
   0  0x49ABD0  GetChangingStatIndex(cadena) cdecl: seis `repe cmps` con el NUL incluido contra "-" (0x85A50C, 2 bytes), "fat" (0x85A508), "respect" (0x85A500), "sexy" (0x85A4F8), "health" (0x85A4F0), "stamina" (0x85A4E8)
                y "calories" (0x85A4DC), en ese orden: "-" -> -1, "fat" -> 0, "respect" -> 1, "sexy" -> 2, "health" -> 3, "stamina" -> 4, "calories" -> 5; sin coincidencia -> -1
   1  0x49ACD0  RemovePriceModifier(clave): n = [0xA9A7D0] (con signo); i = 0; si n > 0 busca la primera entrada de 0xA98650 (8 bytes: clave, precio) con esa clave; si i == n termina; n -= 1 (se guarda);
                si n > 0: la entrada i = la entrada n (clave y precio)
   2  0x49BDD0  AddPriceModifier(clave, precio): n = [0xA9A7D0]; i = 0; si n > 0 busca la clave: si esta, precio de esa entrada = precio; si i == n: entrada n = (clave, precio) y n + 1 (se guarda); despues m = [0xA9A7CC];
                si m > 0 busca la clave en 0xA986F0 (0x18 bytes por entrada; clave en +0): si esta, su precio (+4) = precio
   3  0x49C3B0  pool de saltos acrobaticos GetAt(indice) thiscall `ret 4`: estado = BYTE [[this+4] + indice]; si es negativo (bit 7) -> 0; si no: [this] + indice * 0x44 (32 bits)
   4  0x49C440  Flush (segunda copia): si [this+8] > 0 (con signo): con BYTE [this+10h] != 0: delete[] [this], delete[] [this+4]; despues +0, +4, +8 y +0Ch = 0
   5  0x5A2F80  IsPtInTriangle2D (x87, thiscall con ECX sin usar, `ret 3Ch`): ver tri_expected
   6  0x5A3070  constructor de ProcObjectListItem: [this] = [this+4] = 0; eax = this
   7  0x5A3090  destructor de ProcObjectMan_c: no escribe memoria
   8  0x5A3E20  constructor de ProcObjectMan_c: 128 x (+8 + 0x48 * i + 0x3C: tres dwords a cero), 512 x (+0x2408 + 0x14 * j: +0 y +4 a cero), lista (+0x4C08: tres dwords a cero); eax = this
   9  0x635CF0  CTaskSimpleStandStill::Clone: new(20h) 61A5A0; si no es nulo: base 61A390 ([+4] = 0), +8 = [this+8], +0Ch..+13h = 0, +14h, +15h = 0, BYTE +18h y +19h copiados, +1Ch = [this+1Ch]; vtable en +0 (enmascarada)
  10  0x635DA0  CTaskSimpleDie::Clone: new(28h); con [this+10h] != 0: ctor 62FAF0(+10h, +14h, +18h, +1Ch); si no: ctor 62FA00(+8, +0Ch, +18h, +1Ch); eax = lo que devuelve el constructor
  11  0x635E60  CTaskSimpleDrown::Clone: new(2Ch); base 61A390; +8, +10h, +14h, +1Ch, +24h = 0; BYTE +20h &= 0FCh; +0Ch = 8Ch; +18h = 4.0f; BYTE +28h = 1; vtable en +0 (enmascarada)
  12  0x63B960  CTaskComplexLeaveCar::GetTaskType: mov eax,2C0h
  13  0x63BA00  RegisterPedLeaving(ped): ver reg_leave
  14  0x63BAE0  CreateLineUpUtility(ped sin usar): new(1Ch) 82119A; con memoria: ctor 64FBB0(&vector0, 0, 0, [this+10h]); [this+1Ch] = eax o 0
  15  0x63D9E0  CTaskComplexLeaveCar::Clone: new(34h) 61A5A0; ctor 63B8C0([+0Ch], [+10h], [+14h], BYTE +18h ampliado, BYTE +19h ampliado); eax = lo que devuelve; sin memoria 0
  16  0x6410E0  destructor con borrado de LeaveCar: call 63B970; si el bit 0 del BYTE de banderas: delete 61A5B0; eax = this
  17  0x642250  destructor con borrado de LeaveBoat: call 63BBD0; idem

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar o argumentos]; operator delete [direccion, puntero, dword del puntero]; operator new [direccion, tamano].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["GetChangingStatIndex", "CShopping::RemovePriceModifier-Key", "CShopping::AddPriceModifier-Key", "CStuntJumpManager::PoolGetAt", "CStuntJumpManager::PoolFlushSecondCopy", "ProcSurfaceInfo_c::IsPtInTriangle2D",
         "ProcObjectListItem::Constructor", "ProcObjectMan_c::Destructor", "ProcObjectMan_c::Constructor", "CTaskSimpleStandStill::Clone", "CTaskSimpleDie::Clone", "CTaskSimpleDrown::Clone",
         "CTaskComplexLeaveCar::GetTaskType", "CTaskComplexLeaveCar::RegisterPedLeaving", "CTaskComplexLeaveCar::CreateLineUpUtility", "CTaskComplexLeaveCar::Clone",
         "CTaskComplexLeaveCar::ScalarDeletingDestructor", "CTaskComplexLeaveBoat::ScalarDeletingDestructor"]
ENTRIES = (0x49ABD0, 0x49ACD0, 0x49BDD0, 0x49C3B0, 0x49C440, 0x5A2F80, 0x5A3070, 0x5A3090, 0x5A3E20, 0x635CF0, 0x635DA0, 0x635E60, 0x63B960, 0x63BA00, 0x63BAE0, 0x63D9E0, 0x6410E0, 0x642250)
GUARD = 0x40
WIN = 0x1000
WINBIG = 0x5000
SEED = 60                                                        # a[60..63] = semillas de las arenas
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
GB, GSIZE = 0xA98650, 0x2184                                     # memoria global de CShopping
MODTAB, PRICETAB, NUMPRICES, NUMMODS = 0xA98650, 0xA986F0, 0xA9A7CC, 0xA9A7D0
STATS = [(b'-\x00', -1), (b'fat\x00', 0), (b'respect\x00', 1), (b'sexy\x00', 2), (b'health\x00', 3), (b'stamina\x00', 4), (b'calories\x00', 5)]   # en el orden de las comparaciones del EXE
NEW_TASK, DEL_TASK, NEW_CRT = 0x61A5A0, 0x61A5B0, 0x82119A

(STAT, REMMOD, ADDMOD, GETAT, FLUSH, TRI, LICTOR, MANDTOR, MANCTOR, STANDCLONE, DIECLONE, DROWNCLONE, CONST, REGLEAVE, LINEUP, LCCLONE, SDD) = range(17)
KIND = [STAT, REMMOD, ADDMOD, GETAT, FLUSH, TRI, LICTOR, MANDTOR, MANCTOR, STANDCLONE, DIECLONE, DROWNCLONE, CONST, REGLEAVE, LINEUP, LCCLONE, SDD, SDD]
SDD_BODY = {16: 0x63B970, 17: 0x63BBD0}


def win_a(fn):
    return WINBIG if fn in (7, 8) else WIN


def signed32(v):
    v &= M32
    return v - 0x100000000 if v & 0x80000000 else v


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


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
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
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out

    def mask_both(self, off, n, b):
        for k in range(n):
            self.m[GUARD + off + k] = b
            self.r[GUARD + off + k] = b


class Glob(Mem):
    """memoria global del juego (0xA98650..0xA9A7D4): direcciones absolutas, sin guardas"""

    def rda(self, addr, n):
        return sum(self.get(addr - GB + k) << (8 * k) for k in range(n))

    def wra(self, addr, n, v):
        for k in range(n):
            self.m[addr - GB + k] = (v >> (8 * k)) & 0xFF

    def changes_abs(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([GB + i, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.sizes = (GUARD + win_a(c['fn']) + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD)
        self.mem = tuple(Mem(a[2], a[SEED + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + self.sizes[i]:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + self.sizes[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


# ------------------------------------------------------------------------------------------------ x87 de 24 bits (precision de significando 24, rango de exponente ampliado)
def _pow2(e):
    return Fraction(1 << e) if e >= 0 else Fraction(1, 1 << -e)


def _floor_log2(x):
    e = x.numerator.bit_length() - x.denominator.bit_length()
    if _pow2(e) > x:
        e -= 1
    elif _pow2(e + 1) <= x:
        e += 1
    return e


def _round_half_even(x):
    fl = x.numerator // x.denominator
    rem = x - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl & 1):
        fl += 1
    return fl


def r24(x):
    e = _floor_log2(x)
    step = _pow2(e - 23)
    return _round_half_even(x / step) * step


INDEF = ('n', 1, 0x400000)


def ld(bits):
    s, e, m = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 0xFF:
        return ('i', s, None) if m == 0 else ('n', s, m | 0x400000)
    if e == 0:
        return ('z', s, None) if m == 0 else ('f', s, Fraction(m) * _pow2(-149))
    return ('f', s, Fraction(0x800000 | m) * _pow2(e - 150))


def _sv(v):
    if v[0] == 'z':
        return Fraction(0)
    return -v[2] if v[1] else v[2]


def _mk(x):
    if x == 0:
        return ('z', 0, None)
    return ('f', 1 if x < 0 else 0, r24(abs(x)))


def _nan(a, b):
    na, nb = a[0] == 'n', b[0] == 'n'
    if na and nb:
        # la x87 elige el NaN de mayor significando; con significandos iguales el de signo positivo (medido en el hardware)
        if a[2] != b[2]:
            return a if a[2] > b[2] else b
        return a if a[1] == 0 else b
    return a if na else b


def neg(a):
    return (a[0], a[1] ^ 1, a[2])


def add(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan(a, b)
    if a[0] == 'i' or b[0] == 'i':
        if a[0] == 'i' and b[0] == 'i':
            return a if a[1] == b[1] else INDEF
        return a if a[0] == 'i' else b
    if a[0] == 'z' and b[0] == 'z':
        return ('z', a[1] & b[1], None)
    return _mk(_sv(a) + _sv(b))


def sub(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan(a, b)   # fsub no cambia el signo de un NaN operando
    return add(a, neg(b))


def mul(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan(a, b)
    s = a[1] ^ b[1]
    if a[0] == 'i' or b[0] == 'i':
        if a[0] == 'z' or b[0] == 'z':
            return INDEF
        return ('i', s, None)
    if a[0] == 'z' or b[0] == 'z':
        return ('z', s, None)
    return _mk(_sv(a) * _sv(b))


def div(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return _nan(a, b)
    s = a[1] ^ b[1]
    if a[0] == 'i':
        return INDEF if b[0] == 'i' else ('i', s, None)
    if b[0] == 'i':
        return ('z', s, None)
    if b[0] == 'z':
        return INDEF if a[0] == 'z' else ('i', s, None)
    if a[0] == 'z':
        return ('z', s, None)
    return _mk(_sv(a) / _sv(b))


def le0(v):
    """fcomp contra 0.0 y salto si C0 | C3 es par: acepta si v <= 0; NaN rechaza"""
    if v[0] == 'n':
        return False
    if v[0] == 'z':
        return True
    return v[1] == 1


def st32(v):
    """fstp dword: redondeo a float (con denormales) o NaN con la carga de 23 bits"""
    s = v[1]
    if v[0] == 'n':
        return (s << 31) | 0x7F800000 | (v[2] & 0x7FFFFF)
    if v[0] == 'i':
        return (s << 31) | 0x7F800000
    if v[0] == 'z':
        return s << 31
    x = v[2]
    e = _floor_log2(x)
    if e < -126:
        step = _pow2(-149)
        q = _round_half_even(x / step)
        return (s << 31) | q if q else (s << 31)
    step = _pow2(e - 23)
    q = _round_half_even(x / step)
    if q == 1 << 24:
        q >>= 1
        e += 1
    if e > 127:
        return (s << 31) | 0x7F800000
    return (s << 31) | ((e + 127) << 23) | (q - (1 << 23))


def tri_expected(a):
    """a[3..16] = x, y, v1, v2, v3, normal; devuelve (acepta, bits de z o None, borde que rechaza)"""
    f = [ld(a[3 + i] & M32) for i in range(14)]
    x, y = f[0], f[1]
    v1x, v1y, v1z = f[2], f[3], f[4]
    v2x, v2y = f[5], f[6]
    v3x, v3y = f[8], f[9]
    nx, ny, nz = f[11], f[12], f[13]
    e1 = add(mul(sub(v1x, v2x), sub(v1y, y)), mul(sub(v2y, v1y), sub(v1x, x)))
    if not le0(e1):
        return False, None, 1
    e2 = add(mul(sub(v2x, v3x), sub(v2y, y)), mul(sub(v3y, v2y), sub(v2x, x)))
    if not le0(e2):
        return False, None, 2
    e3 = add(mul(sub(v1y, v3y), sub(v3x, x)), mul(sub(v3x, v1x), sub(v3y, y)))
    if not le0(e3):
        return False, None, 3
    t1 = sub(neg(mul(nx, x)), mul(ny, y))
    u = add(add(mul(v1z, nz), mul(ny, v1y)), mul(nx, v1x))
    z = div(sub(t1, neg(u)), nz)
    return True, st32(z), 0


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_FLAGS = ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
_EXTRA = {
    STAT: ['idx_dash', 'idx_fat', 'idx_respect', 'idx_sexy', 'idx_health', 'idx_stamina', 'idx_calories', 'no_match', 'string_at_edge'],
    REMMOD: ['count_negative', 'count_zero', 'count_one', 'count_many', 'found', 'absent', 'found_at_end', 'found_in_middle', 'duplicate_key'],
    ADDMOD: ['count_negative', 'count_zero', 'mod_found', 'mod_new', 'count_over_20', 'prices_hit', 'prices_miss', 'prices_empty', 'mod_duplicate'],
    GETAT: ['state_free', 'state_used', 'index_negative', 'index_zero', 'index_large', 'mul_wraps'],
    FLUSH: ['size_positive', 'size_zero', 'size_negative', 'owns_set', 'owns_clear', 'owns_high_bits_only'],
    TRI: ['inside', 'outside', 'edge1_reject', 'edge2_reject', 'edge3_reject', 'nz_zero', 'z_nan', 'z_inf', 'z_denormal', 'inputs_nan', 'z_negative_zero'],
    LICTOR: [], MANDTOR: [], MANCTOR: [],
    STANDCLONE: ['mem_null', 'mem_set', 'looped_hi', 'idle_hi', 'blend_nan'],
    DIECLONE: ['mem_null', 'mem_set', 'hier_set', 'hier_null'],
    DROWNCLONE: ['mem_null', 'mem_set'],
    CONST: [],
    REGLEAVE: ['driver_null', 'driver_is_ped', 'driver_other', 'ped_player', 'driver_player', 'sensible_set', 'sensible_clear', 'cleared_cruise', 'forced_stop', 'counter_wrap', 'flags_high_bits'],
    LINEUP: ['mem_null', 'mem_set'],
    LCCLONE: ['mem_null', 'mem_set', 'bytes_hi'],
    SDD: _FLAGS,
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def fnv_changes(ch):
    h = 2166136261
    for off, b in ch:
        for x in struct.pack('<iB', off, b):
            h = ((h ^ x) * 16777619) & M32
    return [len(ch), h]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, gch = [], None, []
    o = a[0]
    this = W.raw(0, o)
    tag = W.tagv
    compact = False

    if k == STAT:
        for i in range(4):
            D.wr(a[3] + 4 * i, 4, a[4 + i])
        W.snap()
        s = bytes(D.rd(a[3] + j, 1) for j in range(16))
        ret = M32
        names = ['dash', 'fat', 'respect', 'sexy', 'health', 'stamina', 'calories']
        hit = None
        for n, (const, idx) in enumerate(STATS):
            if s[:len(const)] == const:
                hit = n
                ret = idx & M32
                break
        marks.append('no_match' if hit is None else 'idx_' + names[hit])
        if a[3] > WIN - 16:
            marks.append('string_at_edge')
    elif k in (REMMOD, ADDMOD):
        G = Glob(a[2], a[SEED + 1])
        key = a[4]
        count = signed32(a[3])
        G.wra(NUMMODS, 4, a[3])
        if k == REMMOD:
            G.wra(NUMPRICES, 4, a[8])
            if a[5] >= 1 and count > 0:
                G.wra(MODTAB + 8 * a[6], 4, key)
                if a[5] == 2:
                    G.wra(MODTAB + 8 * a[7], 4, key)
        else:
            G.wra(NUMPRICES, 4, a[9])
            if a[6] >= 1 and count > 0:
                G.wra(MODTAB + 8 * a[7], 4, key)
                if a[6] == 2:
                    G.wra(MODTAB + 8 * a[8], 4, key)
            if a[10] >= 1 and signed32(a[9]) > 0:
                G.wra(PRICETAB + 0x18 * a[11], 4, key)
                if a[10] == 2:
                    G.wra(PRICETAB + 0x18 * a[12], 4, key)
        W.snap()
        G.snap()
        marks.append('count_negative' if count < 0 else ('count_zero' if count == 0 else None))
        if k == REMMOD:
            if count == 1:
                marks.append('count_one')
            if count > 1:
                marks.append('count_many')
            i = 0
            if count > 0:
                while True:
                    if G.rda(MODTAB + 8 * i, 4) == key:
                        break
                    i += 1
                    if not i < count:
                        break
            if i != count:
                marks.append('found')
                n2 = (count - 1)
                G.wra(NUMMODS, 4, n2 & M32)
                if n2 > 0:
                    if i == n2:
                        marks.append('found_at_end')
                    else:
                        marks.append('found_in_middle')
                    G.wra(MODTAB + 8 * i, 4, G.rda(MODTAB + 8 * n2, 4))
                    G.wra(MODTAB + 8 * i + 4, 4, G.rda(MODTAB + 8 * n2 + 4, 4))
                if a[5] == 2 and count > 0 and a[7] != a[6]:
                    marks.append('duplicate_key')
            else:
                marks.append('absent')
        else:
            price = a[5]
            i = 0
            if count > 0:
                while True:
                    if G.rda(MODTAB + 8 * i, 4) == key:
                        G.wra(MODTAB + 8 * i + 4, 4, price)
                        break
                    i += 1
                    if not i < count:
                        break
            if i == count:
                marks.append('mod_new')
                G.wra(MODTAB + 8 * count, 4, key)
                G.wra(MODTAB + 8 * count + 4, 4, price)
                G.wra(NUMMODS, 4, (count + 1) & M32)
                if count >= 20:
                    marks.append('count_over_20')
            else:
                marks.append('mod_found') if count > 0 else None
                if a[6] == 2 and a[8] != a[7]:
                    marks.append('mod_duplicate')
            m = signed32(G.rda(NUMPRICES, 4))
            hitp = False
            if m > 0:
                j = 0
                while True:
                    if G.rda(PRICETAB + 0x18 * j, 4) == key:
                        G.wra(PRICETAB + 0x18 * j + 4, 4, price)
                        hitp = True
                        break
                    j += 1
                    if not j < m:
                        break
            marks.append('prices_hit' if hitp else ('prices_miss' if m > 0 else 'prices_empty'))
        marks[:] = [x for x in marks if x]
        gch = G.changes_abs()
    elif k == GETAT:
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, (W.raw(3, a[6]) - a[4]) & M32); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8]); A.wr(o + 0x10, 4, a[9])
        D.wr(a[6], 1, a[5] & 0xFF)
        W.snap()
        idx = signed32(a[4])
        states = A.rd(o + 4, 4)
        st = W.rd((states + a[4]) & M32, 1)
        if st & 0x80:
            ret = 0
            marks.append('state_free')
        else:
            prod = (a[4] * 0x44) & M32
            if a[4] * 0x44 > M32:
                marks.append('mul_wraps')
            ret = tag((A.rd(o, 4) + prod) & M32)
            marks.append('state_used')
        if idx < 0:
            marks.append('index_negative')
        elif idx == 0:
            marks.append('index_zero')
        if a[4] > 0xFFFF and idx > 0:
            marks.append('index_large')
    elif k == FLUSH:
        A.wr(o, 4, 0x5A5A5A5A); A.wr(o + 4, 4, 0x5A5A5A5A)
        A.wr(o + 8, 4, a[3]); A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[4])
        W.snap()
        if signed32(a[3]) > 0:
            for off in (0, 4, 8, 0xC):
                A.wr(o + off, 4, 0)
            marks.append('size_positive')
        else:
            marks.append('size_zero' if a[3] == 0 else 'size_negative')
        marks.append('owns_set' if a[4] & 0xFF else 'owns_clear')
        if (a[4] & 0xFF) == 0 and a[4]:
            marks.append('owns_high_bits_only')
    elif k == TRI:
        W.snap()
        ok, zbits, edge = tri_expected(a)
        ret = 1 if ok else 0
        if ok:
            marks.append('inside')
            D.wr(a[17], 4, zbits)
            if (zbits & 0x7F800000) == 0x7F800000:
                marks.append('z_nan' if zbits & 0x7FFFFF else 'z_inf')
            elif (zbits & 0x7F800000) == 0 and zbits & 0x7FFFFF:
                marks.append('z_denormal')
            if zbits == 0x80000000:
                marks.append('z_negative_zero')
        else:
            marks.append('outside')
            marks.append('edge%d_reject' % edge)
        if a[16] & 0x7FFFFFFF == 0:
            marks.append('nz_zero')
        if any((a[i] & 0x7F800000) == 0x7F800000 and a[i] & 0x7FFFFF for i in range(3, 17)):
            marks.append('inputs_nan')
    elif k == LICTOR:
        W.snap()
        A.wr(o, 4, 0); A.wr(o + 4, 4, 0)
        ret = tag(this)
    elif k == MANDTOR:
        W.snap()
        compact = True
    elif k == MANCTOR:
        W.snap()
        for i in range(128):
            for d in range(3):
                A.wr(o + 8 + 0x48 * i + 0x3C + 4 * d, 4, 0)
        for j in range(512):
            A.wr(o + 0x2408 + 0x14 * j, 4, 0); A.wr(o + 0x2408 + 0x14 * j + 4, 4, 0)
        for d in range(3):
            A.wr(o + 0x4C08 + 4 * d, 4, 0)
        ret = tag(this)
        compact = True
    elif k == STANDCLONE:
        A.wr(o + 8, 4, a[5]); A.wr(o + 0x18, 1, a[6] & 0xFF); A.wr(o + 0x19, 1, a[7] & 0xFF); A.wr(o + 0x1C, 4, a[8])
        W.snap()
        events.append([NEW_TASK, 0x20])
        if a[6] >> 8:
            marks.append('looped_hi')
        if a[7] >> 8:
            marks.append('idle_hi')
        if (a[8] & 0x7F800000) == 0x7F800000 and a[8] & 0x7FFFFF:
            marks.append('blend_nan')
        if a[3] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[4])
            B.wr(a[4] + 4, 4, 0)
            B.wr(a[4] + 8, 4, a[5]); B.wr(a[4] + 0xC, 4, 0); B.wr(a[4] + 0x10, 4, 0); B.wr(a[4] + 0x14, 1, 0); B.wr(a[4] + 0x15, 1, 0)
            B.wr(a[4] + 0x18, 1, a[6] & 0xFF); B.wr(a[4] + 0x19, 1, a[7] & 0xFF); B.wr(a[4] + 0x1C, 4, a[8])
            B.mask_both(a[4], 4, 0x77)
            ret = tag(mem)
    elif k == DIECLONE:
        A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[8]); A.wr(o + 0x18, 4, a[9]); A.wr(o + 0x1C, 4, a[10])
        W.snap()
        events.append([NEW_TASK, 0x28])
        marks.append('hier_set' if a[5] else 'hier_null')
        if a[3] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[4])
            if a[5]:
                events.append([0x62FAF0, tag(mem), a[5], a[8], a[9], a[10]])
            else:
                events.append([0x62FA00, tag(mem), a[6], a[7], a[9], a[10]])
            B.wr(a[4], 4, a[12])
            ret = tag(a[11])
    elif k == DROWNCLONE:
        W.snap()
        events.append([NEW_TASK, 0x2C])
        if a[3] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[4])
            m = a[4]
            old20 = B.rd(m + 0x20, 1)
            B.wr(m + 4, 4, 0)
            for off in (8, 0x10, 0x14, 0x1C, 0x24):
                B.wr(m + off, 4, 0)
            B.wr(m + 0x20, 1, old20 & 0xFC)
            B.wr(m + 0xC, 4, 0x8C)
            B.wr(m + 0x18, 4, 0x40800000)
            B.wr(m + 0x28, 1, 1)
            B.mask_both(m, 4, 0x77)
            ret = tag(mem)
    elif k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 0x2C0
    elif k == REGLEAVE:
        ped = W.raw(2, a[7]) if a[17] == 0 else a[18]
        driver = 0 if a[6] == 0 else (ped if a[6] == 1 else (W.raw(2, a[8]) if a[6] == 2 else a[9]))
        vb = a[3]
        veh = W.raw(1, vb)
        A.wr(o + 0xC, 4, veh); A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x18, 1, a[5] & 0xFF); A.wr(o + 0x20, 4, a[19])
        B.wr(vb + 0x36, 1, a[13] & 0xFF); B.wr(vb + 0x3BA, 1, a[14] & 0xFF); B.wr(vb + 0x3D0, 1, a[15] & 0xFF); B.wr(vb + 0x460, 4, driver); B.wr(vb + 0x485, 1, a[16] & 0xFF)
        W.snap()
        pl = [a[10], a[11]]
        calls = 0

        def is_player(who):
            nonlocal calls
            events.append([0x5DF8F0, tag(who)])
            r = pl[0 if calls == 0 else 1]
            calls += 1
            return r & 0xFF
        events.append([0x64E550, tag(veh), a[4], 1])
        flags = a[12] & 0xFF
        A.wr(o + 0x20, 1, flags)
        events.append([0x6D3020, tag(veh), flags])
        A.wr(o + 0x21, 1, 1)
        cnt = B.rd(vb + 0x485, 1)
        B.wr(vb + 0x485, 1, (cnt + 1) & 0xFF)
        if cnt == 0xFF:
            marks.append('counter_wrap')
        drv = B.rd(vb + 0x460, 4)
        marks.append('driver_null' if drv == 0 else ('driver_is_ped' if drv == ped else 'driver_other'))
        marks.append('sensible_set' if a[5] & 0xFF else 'sensible_clear')
        if a[5] >> 8:
            marks.append('flags_high_bits')
        if drv != 0:
            if is_player(drv) == 0:
                if ped == B.rd(vb + 0x460, 4) and (a[5] & 0xFF) != 0:
                    B.wr(vb + 0x3D0, 1, 0)
                    B.wr(vb + 0x3BA, 1, 0)
                    marks.append('cleared_cruise')
            else:
                marks.append('driver_player')
        if is_player(ped):
            marks.append('ped_player')
            if ped == B.rd(vb + 0x460, 4):
                st = B.rd(vb + 0x36, 1)
                B.wr(vb + 0x36, 1, (st & 7) | 0x48)
                marks.append('forced_stop')
    elif k == LINEUP:
        A.wr(o + 0x10, 4, a[3])
        W.snap()
        events.append([NEW_CRT, 0x1C])
        if a[4] == 0:
            marks.append('mem_null')
            A.wr(o + 0x1C, 4, 0)
        else:
            marks.append('mem_set')
            mem = W.raw(2, a[5])
            events.append([0x64FBB0, tag(mem), 0x7F000000, 0, 0, 0, 0, 0, a[3]])
            Cc.wr(a[5], 4, a[7])
            A.wr(o + 0x1C, 4, a[6])
    elif k == LCCLONE:
        A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[6]); A.wr(o + 0x14, 4, a[7]); A.wr(o + 0x18, 1, a[8] & 0xFF); A.wr(o + 0x19, 1, a[9] & 0xFF)
        W.snap()
        events.append([NEW_TASK, 0x34])
        if (a[8] >> 8) or (a[9] >> 8):
            marks.append('bytes_hi')
        if a[3] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[4])
            events.append([0x63B8C0, tag(mem), a[5], a[6], a[7], a[8] & 0xFF, a[9] & 0xFF])
            B.wr(a[4], 4, a[11])
            ret = tag(a[10])
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([SDD_BODY[fn], tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DEL_TASK, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    chA = A.changes()
    return {'events': events, 'changes': {'A': fnv_changes(chA) if compact else chA, 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
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
