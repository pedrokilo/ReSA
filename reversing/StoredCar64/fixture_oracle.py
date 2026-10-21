"""Oraculo independiente del banco StoredCar64 (lote 64): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de CStoredCar::StoreCar (0x449760..0x4498F8) y de
CStoredCar::RestoreCar (0x447E40..0x4482FD; tabla de saltos 0x448300) para una fila `input` (las escrituras previas de la arena y los parametros que registra Core.hpp), con las mismas
fichas simbolicas de punteros que usa el banco:

  S la ficha CStoredCar;  V0/V1 vehiculos;  M0/M1 sus matrices;  numero = valor crudo.

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos de los falsos EN ORDEN con sus argumentos, cambios de la arena
del banco, cambios del estado real y, en RestoreCar, el vehiculo devuelto en EAX); `stats` son banderas de cobertura. Aritmetica x87 del EXE: PC 24 bits, exponente extendido (fracciones exactas);
_ftol2 trunca hacia cero y devuelve el entero indefinido (0x80000000 en los 32 bits bajos del resultado de 64) si el valor no cabe o es NaN/infinito.
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
from fractions import Fraction
import math

NAME_STORE = 'CStoredCar::StoreCar'
NAME_RESTORE = 'CStoredCar::RestoreCar'
NAMES = [NAME_STORE, NAME_RESTORE]
CASES_BY_NAME = {NAME_STORE: 4096, NAME_RESTORE: 8192}
M32 = 0xFFFFFFFF

(K_GETREMAP, K_REQMODEL, K_REQUPG, K_HASUPG, K_NEW, K_CTOR, K_SETUP, K_SETREMAP) = range(1, 9)
RS, RV0, RV1, RM0, RM1, RMI0 = 0, 1, 2, 3, 4, 5
MARK_MAT = {0xFFFF0100: RM0, 0xFFFF0101: RM1}
REGION_NAME = {RS: ('S', 0), RV0: ('V', 0), RV1: ('V', 1), RM0: ('M', 0), RM1: ('M', 1), 5: ('MI', 0), 6: ('MI', 1), 7: ('MI', 2), 8: ('MI', 3)}
TM = [5000, 5001, 5002, 5003]
INF = math.inf


# ---------------------------------------------------------------------------------------------- relleno determinista (identico a Fixtures.hpp)
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def garbage(n, k):
    v = 0xC0000000 | (rand((n + 0x6A09E667) & M32, k) & 0x3FFFFFFF)
    return 0xC0000000 if v == 0xFFFFFFFF else v


# ---------------------------------------------------------------------------------------------- aritmetica x87 (24 bits) con fracciones
class NaN:
    def __init__(self, bits):
        self.bits = bits


def dec(bits):
    s = -1 if bits >> 31 else 1
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 255:
        return NaN(bits) if m else s * INF
    if e == 0:
        return s * Fraction(m, 1 << 149)
    return s * Fraction(8388608 + m, 1) * Fraction(2) ** (e - 150)


def _round_scaled(x, scale):
    q = x / scale
    n = q.numerator // q.denominator
    r = q - n
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return n * scale


def r24(x):
    """redondeo a 24 bits de mantisa con exponente sin limite (fmul con PC=24)"""
    if x == 0:
        return Fraction(0)
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    return _round_scaled(x, Fraction(2) ** (e - 23))


def rf32(x):
    """guardado a float (fstp dword): rejilla de subnormales, desbordamiento a infinito"""
    if x == 0:
        return Fraction(0)
    a = abs(x)
    if a < Fraction(1, 1 << 126):
        return _round_scaled(x, Fraction(1, 1 << 149))
    r = r24(x)
    if abs(r) >= Fraction(2) ** 128:
        return INF if r > 0 else -INF
    return r


def enc(v):
    if isinstance(v, NaN):
        return v.bits
    if v == INF:
        return 0x7F800000
    if v == -INF:
        return 0xFF800000
    if v == 0:
        return 0
    s = 0x80000000 if v < 0 else 0
    a = abs(v)
    if a < Fraction(1, 1 << 126):
        return s | int(a * (1 << 149))
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    m = (a / Fraction(2) ** e - 1) * (1 << 23)
    assert m.denominator == 1, v
    return s | ((e + 127) << 23) | int(m)


def mul(a, b):
    if isinstance(a, NaN):
        return NaN(a.bits | 0x400000)
    if isinstance(b, NaN):
        return NaN(b.bits | 0x400000)
    if a in (INF, -INF) or b in (INF, -INF):
        if a == 0 or b == 0:
            return NaN(0xFFC00000)
        return INF if (a > 0) == (b > 0) else -INF
    return r24(a * b)


def store(v):
    """fstp dword -> bits"""
    if isinstance(v, NaN) or v in (INF, -INF):
        return enc(v)
    return enc(rf32(v))


def ftol_low32(v):
    """_ftol2: 32 bits bajos del entero truncado; NaN, infinito o fuera de rango -> entero indefinido (bajos = 0)"""
    if isinstance(v, NaN) or v in (INF, -INF):
        return 0
    t = int(v)    # trunca hacia cero
    if t < -(1 << 63) or t >= (1 << 63):
        return 0
    return t & M32


def quiet(bits):
    """fld dword + fstp dword: un NaN senalizador sale silenciado"""
    if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF):
        return bits | 0x400000
    return bits


def sext(v, bits):
    v &= (1 << bits) - 1
    return v - (1 << bits) if v >> (bits - 1) else v


C_HUNDRED = Fraction(100)
C_001 = dec(0x3C23D70A)      # 0.01f [0x858C58]


# ---------------------------------------------------------------------------------------------- memoria de la arena
class Mem:
    def __init__(self, inp):
        self.gs = inp['gs']
        self.layout = inp['layout']
        self.pre = {}
        for region, off, size, val in inp['pre']:
            self._wr(self.pre, region, off, size, val, base=None)
        self.cur = {}

    def base_word(self, key):
        if key in self.pre:
            return self.pre[key]
        return garbage(self.gs, self.layout[key[0]] + key[1])

    def _wr(self, d, region, off, size, val, base):
        for i in range(size):
            b = (val >> (8 * i)) & 0xFF
            w, sh = (off + i) // 4, 8 * ((off + i) % 4)
            key = (region, w)
            old = d[key] if key in d else (self.base_word(key) if base is None else base(key))
            d[key] = (old & ~(0xFF << sh) & M32) | (b << sh)

    def rd(self, region, off, size):
        v = 0
        for i in range(size):
            w, sh = (off + i) // 4, 8 * ((off + i) % 4)
            key = (region, w)
            word = self.cur[key] if key in self.cur else self.base_word(key)
            v |= ((word >> sh) & 0xFF) << (8 * i)
        return v

    def wr(self, region, off, size, val):
        self._wr(self.cur, region, off, size, val & ((1 << (8 * size)) - 1), base=self.base_word)

    def changes(self):
        out = []
        for key in sorted(self.cur):
            if self.cur[key] != self.base_word(key):
                name, idx = REGION_NAME[key[0]]
                out.append([name, idx, key[1], self.cur[key]])
        return out


def tok(region):
    name, idx = REGION_NAME[region]
    return name + str(idx) if name != 'S' else 'S'


# ---------------------------------------------------------------------------------------------- StoreCar (0x449760)
def store_car(inp, stats):
    m = Mem(inp)
    V = RV0
    ptr = m.rd(V, 0x14, 4)
    assert ptr in MARK_MAT, 'el banco siempre da matriz al vehiculo de StoreCar'
    M = MARK_MAT[ptr]
    events = [[K_GETREMAP, tok(V)]]
    m.wr(RS, 0x12, 2, m.rd(V, 0x22, 2))
    for k in range(3):                       # posicion de la matriz (+0x30), copiada como palabras
        m.wr(RS, 4 * k, 4, m.rd(M, 4 * (12 + k), 4))
    for k in range(3):                       # avance (+0x10/+0x14/+0x18) * 100.0f, _ftol2, byte bajo
        f = dec(m.rd(M, 4 * (4 + k), 4))
        low = ftol_low32(mul(f, C_HUNDRED))
        m.wr(RS, 0x3C + k, 1, low & 0xFF)
        stats['axis_nonfinite'] = stats.get('axis_nonfinite', False) or isinstance(f, NaN) or f in (INF, -INF)
        stats['axis_overflow'] = stats.get('axis_overflow', False) or (not isinstance(f, NaN) and f not in (INF, -INF) and abs(f * 100) >= (1 << 63))
        stats['axis_negative'] = stats.get('axis_negative', False) or (low >> 7 & 1 == 1)
    for k in range(4):
        m.wr(RS, 0x32 + k, 1, m.rd(V, 0x434 + k, 1))
    m.wr(RS, 0x36, 1, m.rd(V, 0x1D2, 1))
    m.wr(RS, 0x0C, 4, m.rd(V, 0x38C, 4))
    m.wr(RS, 0x37, 1, m.rd(V, 0x438, 1))
    m.wr(RS, 0x38, 1, m.rd(V, 0x439, 1))
    phys = m.rd(V, 0x40, 4)
    flags = 0
    if phys & 0x40000:
        flags = 1
        stats['flag01'] = True
    if phys & 0x80000:
        flags |= 2
    if phys & 0x800000:
        flags |= 4
    if phys & 0x100000:
        flags |= 8
    if phys & 0x200000:
        flags |= 0x10
    if m.rd(V, 0x42E, 1) & 0x10:
        flags |= 0x20
    if m.rd(V, 0x38C, 4) & 0x20000:
        flags |= 0x40
    if m.rd(V, 0x38C, 4) & 0x80000:
        flags |= 0x80
    m.wr(RS, 0x10, 2, flags)                 # palabra de 16 bits: el relleno (+0x11) queda en 0
    stats['flags_all'] = stats.get('flags_all', False) or flags == 0xFF
    t = m.rd(V, 0x590, 4)
    if t == 0 or t == 9:
        m.wr(RS, 0x39, 1, m.rd(V, 0x4A8, 1) & 7)
        stats['bomb_stored'] = True
    else:
        stats['bomb_kept'] = True
    for i in range(15):
        m.wr(RS, 0x14 + 2 * i, 2, m.rd(V, 0x43A + 2 * i, 2))
    m.wr(RS, 0x3A, 1, inp['remap'] & 0xFF)   # CVehicle::GetRemapIndex (fake): el byte bajo de EAX
    m.wr(RS, 0x3B, 1, m.rd(V, 0x48A, 1))
    observed = {'events': events, 'nevents': len(events), 'changes': m.changes(), 'globals': []}
    return observed


# ---------------------------------------------------------------------------------------------- RestoreCar (0x447E40)
CAR_TABLE = {0: (0x99C, 0), 1: (0x9BC, 1), 2: (0xA18, 2), 3: (0xA04, 3), 4: (0x7E8, 4), 8: (0x814, 5), 9: (0x838, 6), 10: (0x9F4, 7)}


def restore_car(inp, stats):
    m = Mem(inp)
    model = m.rd(RS, 0x12, 2)
    km = TM.index(model)
    events = [[K_REQMODEL, model, 8]]
    mods = [m.rd(RS, 0x14 + 2 * i, 2) for i in range(15)]
    for mod in mods:
        if mod != 0xFFFF:
            events.append([K_REQUPG, sext(mod, 16) & M32, 0])
    comps0, comps1 = inp['comps']

    def fail(tag):
        stats[tag] = True
        return {'events': events, 'nevents': len(events), 'changes': [], 'globals': [], 'ret': 0}
    if inp['state'][km] != 1:
        return fail('fail_state')
    k = 0
    for i, mod in enumerate(mods):
        if mod != 0xFFFF:
            events.append([K_HASUPG, sext(mod, 16) & M32])
            ok = (inp['has'] >> (k & 31)) & 1
            k += 1
            if not ok:
                return fail('fail_has_first' if k == 1 else 'fail_has_later')
    stats['built'] = True
    g = []
    n0, n1 = m.rd(RS, 0x37, 1), m.rd(RS, 0x38, 1)
    if n0 != comps0:
        g.append(['comps0', n0])
    if n1 != comps1:
        g.append(['comps1', n1])
    idx = (inp['vtype'][km] - 1) & M32
    sel = 99 if idx > 10 else idx
    stats['type%d' % (sel if sel != 99 else 11)] = True
    size, variant = CAR_TABLE.get(sel, (0x988, 8))
    events.append([K_NEW, size])
    ctor_row = [K_CTOR, variant, 'V0', model, 1, 1 if variant == 8 else 0]
    events.append(ctor_row)
    V = RV1 if inp['ctor1'] else RV0
    stats['ctor1' if inp['ctor1'] else 'ctor0'] = True
    ptr = m.rd(V, 0x14, 4)
    M = MARK_MAT[ptr]
    if variant in (5, 6):
        m.wr(V, 0x614, 1, m.rd(V, 0x614, 1) | 0x10)
        stats['bike'] = True
    for kk in range(3):                      # posicion: fld/fstp silencia los NaN senalizadores
        raw = m.rd(RS, 4 * kk, 4)
        q = quiet(raw)
        stats['snan_pos'] = stats.get('snan_pos', False) or q != raw
        m.wr(M, 4 * (12 + kk), 4, q)
    fx = [sext(m.rd(RS, 0x3C + kk, 1), 8) for kk in range(3)]
    fb = [store(mul(Fraction(v), C_001)) for v in fx]
    for kk in range(3):
        m.wr(M, 4 * (4 + kk), 4, fb[kk])     # avance
    m.wr(M, 0, 4, fb[1])                     # perpendicular (y, -x, 0)
    m.wr(M, 4, 4, fb[0] ^ 0x80000000)
    m.wr(M, 8, 4, 0)
    m.wr(V, 0x36, 1, (m.rd(V, 0x36, 1) & 7) | 0x20)
    m.wr(M, 4 * 8, 4, 0)
    m.wr(M, 4 * 9, 4, 0)
    m.wr(M, 4 * 10, 4, 0x3F800000)
    stats['fwd_neg'] = stats.get('fwd_neg', False) or any(v < 0 for v in fx)
    m.wr(V, 0x460, 4, 0)
    m.wr(V, 0x1D2, 1, m.rd(RS, 0x36, 1))
    m.wr(V, 0x38C, 4, m.rd(RS, 0x0C, 4))
    m.wr(V, 0x428, 1, m.rd(V, 0x428, 1) & 0x7F)
    m.wr(V, 0x42A, 1, m.rd(V, 0x42A, 1) | 2)
    m.wr(V, 0x4F8, 4, 1)
    if m.rd(V, 0x590, 4) == 0:
        b = m.rd(V, 0x4A8, 1)
        m.wr(V, 0x4A8, 1, (b & 0xF8) | (m.rd(RS, 0x39, 1) & 7))
        stats['bomb_set'] = True
    else:
        stats['bomb_kept'] = True
    fl = m.rd(RS, 0x10, 1)
    for bit, mask in ((0x01, 0x40000), (0x02, 0x80000), (0x04, 0x800000), (0x08, 0x100000), (0x10, 0x200000)):
        if fl & bit:
            m.wr(V, 0x40, 4, m.rd(V, 0x40, 4) | mask)
    if fl & 0x20:
        m.wr(V, 0x42E, 1, m.rd(V, 0x42E, 1) | 0x10)
        m.wr(V, 0x1BE, 1, 1)
    if fl & 0x40:
        m.wr(V, 0x38C, 4, m.rd(V, 0x38C, 4) | 0x20000)
    if fl & 0x80:
        m.wr(V, 0x38C, 4, m.rd(V, 0x38C, 4) | 0x80000)
    stats['flags_all'] = stats.get('flags_all', False) or fl == 0xFF
    for i in range(15):
        m.wr(V, 0x43A + 2 * i, 2, mods[i])
    events.append([K_SETUP, tok(V)])
    events.append([K_SETREMAP, tok(V), sext(m.rd(RS, 0x3A, 1), 8) & M32])
    m.wr(V, 0x428, 1, m.rd(V, 0x428, 1) & 0xEF)
    m.wr(V, 0x48A, 1, m.rd(RS, 0x3B, 1))
    m.wr(V, 0x434, 1, m.rd(RS, 0x32, 1))
    m.wr(V, 0x435, 1, m.rd(RS, 0x33, 1))
    m.wr(V, 0x436, 1, m.rd(RS, 0x34, 1))
    m.wr(V, 0x42F, 1, m.rd(V, 0x42F, 1) | 4)
    m.wr(V, 0x437, 1, m.rd(RS, 0x35, 1))
    return {'events': events, 'nevents': len(events), 'changes': m.changes(), 'globals': g, 'ret': tok(V)}


def expected(name, inp, case):
    stats = {}
    if name == NAME_STORE:
        return store_car(inp, stats), stats
    if name == NAME_RESTORE:
        return restore_car(inp, stats), stats
    raise KeyError(name)


if __name__ == '__main__':
    # autoprueba sin el juego: valores conocidos del truncamiento y del orden de eventos
    assert ftol_low32(mul(dec(0x3F800000), C_HUNDRED)) == 100
    assert ftol_low32(mul(dec(0xBF800000), C_HUNDRED)) == (-100) & M32
    assert ftol_low32(mul(dec(0x7FC00000), C_HUNDRED)) == 0 and ftol_low32(mul(dec(0x7F800000), C_HUNDRED)) == 0
    assert ftol_low32(mul(dec(0x7F7FFFFF), C_HUNDRED)) == 0           # no cabe en 64 bits
    assert ftol_low32(mul(dec(0x3F8CCCCD), C_HUNDRED)) == 110
    assert enc(dec(0x3C23D70A)) == 0x3C23D70A and quiet(0x7F800001) == 0x7FC00001 and quiet(0x7FC00000) == 0x7FC00000
    assert sext(0xFFFF, 16) == -1 and sext(0x80, 8) == -128
    print('ok')
