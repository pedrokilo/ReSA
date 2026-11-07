"""Oraculo independiente (Python) de Fn116A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las dieciocho funciones del lote 116A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente
ampliado) se modela con fracciones exactas (NaN con signo y carga util; el seno y el coseno con series de Taylor de 100 digitos redondeadas a 64 bits de mantisa, |h| <= 8).

Funcion 0, CBoundingBox::IsPointWithin (0x49C300..0x49C35F; thiscall, ECX = caja (min +0, max +0xC), &punto en pila, `ret 4`; AL): x >= minx, y >= miny, z >= minz, x <= maxx, y <= maxy, z <= maxz con
  fcomp (un NaN hace falsa la comparacion; -0 == +0); devuelve 1 solo si se cumplen las seis. No deja nada en la x87.
Funcion 1, CColBox::Set (0x40EE70..0x40EEB4; thiscall, `ret 14h`): copia sup (3 dwords, cada lectura seguida de su escritura) a +0, inf a +0xC y guarda los bytes bajos de material (+0x18), pieza (+0x19) e
  iluminacion (+0x1A); no toca +0x1B.
Funcion 2, CColBox::operator= (0x40EEC0..0x40EF06; thiscall, `ret 4`; EAX = this): copia 3 dwords de +0, 3 dwords de +0xC, la palabra de +0x18 y el byte de +0x1A, cada lectura seguida de su escritura.
Funcion 3, tColLighting::GetCurrentLighting (0x59F0C0..0x59F10C; cdecl, byte de iluminacion en la pila (dia = nibble bajo, noche = nibble alto), factor float; ST0): ((dia*f)*K)*(1-b) + ((noche*f)*K)*b con
  K = 0x3D888889 (0.06666667f), b = balance (dword en 0x8D12C0) y cada operacion redondeada a 24 bits; el resultado queda en ST0 sin guardar.
Funcion 4, CPad::sub_540530 (0x540530..0x540581; thiscall; AL): modo = palabra en +0x10A (sin signo); modos 0, 2 y 3: (palabra +0x1A != 0 y +0x4A == 0); modo 1: (+0x10 != 0 y +0x40 == 0) o (+0x12 != 0 y
  +0x42 == 0); mayor que 3: 0.
Funcion 5, FightHitLevel (0x5BD360..0x5BD3A3; cdecl, char*; AL): primer caracter 'H' -> 0, 'L' -> 1, 'G' -> 2, 'B' -> 3; cualquier otro -> 7 (las comparaciones de 16 bits posteriores nunca se cumplen).
Funcion 6, FightHitSound (0x5BD3B0..0x5BD3F0; cdecl, int; EAX): (n - 1) sin signo > 7 -> 0x3E; si no la tabla 0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44.
Funcion 7, approxEqual (0x4EEA80..0x4EEA9D; cdecl; EAX = 1 o 0): fabs(f1 - f2) < epsilon (resta redondeada a 24 bits con exponente ampliado, fcomp ordenado: con NaN devuelve 0).
Funcion 8, ConvertPedNode2BoneTag (0x4D58A0..0x4D591F; cdecl; EAX): nodo 1..18 -> 3, 5, 32, 22, 34, 24, 41, 51, 43, 53, 52, 42, 33, 23, 31, 21, 4, 8; otro valor (comparacion sin signo de nodo - 1 con 17) -> -1.
Funcion 9, CStuckCarCheck::ResetArrayElement (0x463970..0x4639D5; thiscall, `ret 4`): entrada = lista + (palabra baja del argumento) * 0x24: +4/+8/+0xC = -5000.0f, +0 = -1, +0x10 = -1, +0x14 = 0, +0x18 = 0 y los
  bytes +0x1C..+0x21 = 0 (no toca +0x22 y +0x23).
Funcion 10, CEventDamage::ComputeHeadShot (0x4ADD60..0x4ADD9A; thiscall, `ret 4`): escribe 0 en el byte de salida; con el arma (+0x1C, sin signo) <= 0x34 y su clase en la tabla (0 cuerpo a cuerpo: escribe 1 si el
  byte +0x41 es distinto de cero; 1 armas de fuego: ademas pieza (+0x20) == 9; 2: nada) puede escribir 1.
Funcion 11, FindCutsceneAudioTrackId (0x5AFA50..0x5AFA8B; cdecl, char*; AX): recorre la tabla del EXE (0x8D0AA8: puntero al nombre y palabra de id cada 8 bytes) y devuelve el id de la primera entrada igual
  (_stricmp) o -1.
Funcion 12, CSimpleTransform::Invert (0x54EF90..0x54EFC9; thiscall, `ret 4`): c = fcos(h), s = fsin(h) (h = base +0xC) sin redondear; destino +0 = (-(c*bx)) - (s*by) (guardado), +4 = (s*bx) - (c*by) (bx releido tras
  escribir +0), +8 = -bz y +0xC = -h (fld + fchs + fstp: un NaN senalizante sale silencioso).
Funcion 13, TransformVectors (0x54EE30..0x54EE96; cdecl, salida, cuenta, &transformacion, entrada): c y s = fcos y fsin de h (+0xC) guardados como float; por vector: x' = (c*x) - (s*y) guardado, y' = (s*x) + (c*y)
  con x releido tras escribir x', z copiada como dword; con cuenta 0 no hace nada.
Funcion 14, CGarages::IsPointWithinAnyGarage (0x448990..0x4489E0; cdecl, &punto; AL): para cada uno de los 50 garajes (0xD8 bytes desde 0x96C048) con tipo (+0x4C) distinto de cero llama a 0x448740
  (ECX = garaje, punto por valor); el primero que devuelve AL distinto de cero da 1; si no 0.
Funcion 15, CGarages::IsPointInAGarageCameraZone (0x449BA0..0x449BFC; cdecl, x, y, z por valor; AL): igual, con 0x4487D0 (punto y 0.5f) y todos los garajes de tipo distinto de cero.
Funcion 16, IsTeleportPointValid (0x43EA90..0x43EAEF; cdecl, &origen, &destino; AL): TestSphereAgainstWorld (0x569E20: destino por valor, 0.35f, 0, 1, 1, 1, 1, 1, 0); si devuelve distinto de cero (EAX completo)
  devuelve 0; si no, GetIsLineOfSightClear (0x56A490: &origen, &destino, 1, 1, 0, 1, 1, 0, 0) y devuelve su AL.
Funcion 17, COnscreenTimer::Init (0x44CBC0..0x44CC16; thiscall): +0x151 = 0; por cada uno de los cuatro contadores (base +0x40 + 0x44 * j): dwords +0, +4, +8, +0xC = 0, palabras +0x10 y +0x12 = 0, byte +0x3E = 0
  y byte +0x3F = 1; despues dwords +0, +4, +8 = 0, palabra +0xC = 0, bytes +0x38 = 0 y +0x39 = 1 y dword +0x3C = 12.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from decimal import Decimal, getcontext
from fractions import Fraction

getcontext().prec = 100
M32 = 0xFFFFFFFF
GUARD = 0x40
A_OBJ, A_A1, A_A2, A_IN, A_OUT, A_STR = range(6)
ARENA_WIN = [0x260, 0x40, 0x40, 0x80, 0x80, 0x60]
GARAGE_BASE, GARAGE_SIZE, GARAGE_COUNT = 0x96C048, 0xD8, 50

# ------------------------------------------------------------------------------------------------ modelo x87 (24 bits, exponente ampliado)
# valor: ('f', Fraction, signo) | ('i', signo) | ('n', signo, significando de 64 bits con el bit 63 explicito y el 62 = silencioso)
INDEF = ('n', 1, 0xC000000000000000)


def load(bits):
    """fld dword: los NaN se silencian y conservan signo y carga util"""
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        if m == 0:
            return ('i', neg)
        return ('n', neg, (1 << 63) | (1 << 62) | (m << 40))
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rndn(v, mant_bits, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - (mant_bits - 1)
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def rnd(v, quantum_exp_min=None):
    return rndn(v, 24, quantum_exp_min)


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def nan_pick(a, b):
    """dos NaN: gana el de mayor significando; con significandos iguales gana el de signo positivo (medido en el EXE: Invert con rumbo NaN)"""
    if a[0] == 'n' and b[0] == 'n':
        if a[2] != b[2]:
            return a if a[2] > b[2] else b
        return a if a[1] == 0 else (b if b[1] == 0 else a)
    return a if a[0] == 'n' else b


def is_nan(a):
    return a[0] == 'n'


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return INDEF
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else INDEF
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def chs(a):
    """fchs: invierte el signo (tambien de NaN, infinitos y ceros)"""
    if a[0] == 'n':
        return ('n', 1 - a[1], a[2])
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    """fsub / fsubp: a - b (con un NaN devuelve el NaN sin cambiar su signo)"""
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    return add(a, chs(b))


def fabs_(a):
    if a[0] == 'n':
        return ('n', 0, a[2])
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def ge(a, b):
    """a >= b ordenado (fcomp + test ah,1): falso con NaN"""
    if is_nan(a) or is_nan(b):
        return False
    return num(a) >= num(b)


def le(a, b):
    if is_nan(a) or is_nan(b):
        return False
    return num(a) <= num(b)


def lt(a, b):
    if is_nan(a) or is_nan(b):
        return False
    return num(a) < num(b)


def store(a):
    """fstp dword: bits del float"""
    if a[0] == 'n':
        return (a[1] << 31) | 0x7F800000 | ((a[2] >> 40) & 0x7FFFFF)
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo"""
    if a[0] == 'n':
        return a[2].to_bytes(8, 'little') + (((a[1] << 15) | 0x7FFF)).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + (neg << 15).to_bytes(2, 'little')
    sign = 1 if v < 0 else 0
    m = abs(v)
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    mant = m / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return int(mant).to_bytes(8, 'little') + ((sign << 15) | (e + 16383)).to_bytes(2, 'little')


def fp_stack(hexstr):
    """(TOP, lista de (st, 10 bytes)) de las posiciones validas de una imagen fnsave normalizada"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return top, valid


# seno y coseno de la x87 (series de Taylor en 100 digitos, redondeados a 64 bits de mantisa; |a| <= 8)
def sin_cos_dec(a):
    a2 = a * a
    eps = Decimal(10) ** -95
    s, c = a, Decimal(1)
    ts, tc = a, Decimal(1)
    n = 1
    while True:
        tc = -tc * a2 / (n * (n + 1))
        ts = -ts * a2 / ((n + 1) * (n + 2))
        n += 2
        c += tc
        s += ts
        if abs(tc) < eps and abs(ts) < eps:
            return s, c


_trig_cache = {}


def sin_cos(h_bits):
    """(seno, coseno) de la x87 como valores del modelo (64 bits de mantisa); NaN/infinito segun fsin/fcos"""
    if h_bits in _trig_cache:
        return _trig_cache[h_bits]
    h = load(h_bits)
    if h[0] == 'n':
        res = (h, h)
    elif h[0] == 'i':
        res = (INDEF, INDEF)
    else:
        v = h[1]
        if v == 0:
            res = (('f', Fraction(0), h[2]), ('f', Fraction(1), 0))
        else:
            assert abs(v) <= 8, 'rumbo fuera del dominio del modelo'
            d = Decimal(v.numerator) / Decimal(v.denominator)
            s_, c_ = sin_cos_dec(d)
            fs = rndn(Fraction(s_), 64)
            fc = rndn(Fraction(c_), 64)
            res = (('f', fs, 1 if fs < 0 else 0), ('f', fc, 1 if fc < 0 else 0))
    _trig_cache[h_bits] = res
    return res


# ------------------------------------------------------------------------------------------------ memoria de las arenas
def rmix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rrand(c, k):
    """misma mezcla que Rand() de Fixtures.hpp (sal 116001)"""
    return rmix(((c & M32) * 0x9E3779B1 + (k & M32) * 0x85EBCA6B + 116001) & M32)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


class Arenas:
    """imagen de las seis arenas: relleno pseudoaleatorio de la semilla del caso mas las escrituras (puts) previas a la llamada; solo se registran los bytes escritos por la funcion"""
    def __init__(self, c):
        self.fill = {}
        self.seed = (c['seed'] * 13 + 50) & M32
        self.init = {}
        self.cur = {}
        self.written = set()

    def _byte0(self, arena, off):
        i = off + GUARD
        assert 0 <= i < GUARD + ARENA_WIN[arena] + GUARD, (arena, off)
        w = rrand((self.seed + arena) & M32, i // 4)
        return (w >> (8 * (i % 4))) & 0xFF

    def rd8(self, arena, off):
        k = (arena, off)
        if k in self.cur:
            return self.cur[k]
        return self._byte0(arena, off)

    def put(self, arena, off, v, n):
        """escritura previa (parte de la imagen inicial)"""
        for j in range(n):
            self.init[(arena, off + j)] = (v >> (8 * j)) & 0xFF
            self.cur[(arena, off + j)] = (v >> (8 * j)) & 0xFF

    def rd(self, arena, off, n):
        return sum(self.rd8(arena, off + j) << (8 * j) for j in range(n))

    def wr(self, arena, off, v, n):
        for j in range(n):
            k = (arena, off + j)
            if k not in self.init:
                self.init[k] = self._byte0(arena, off + j)
            self.cur[k] = (v >> (8 * j)) & 0xFF
            self.written.add(k)

    def changes(self):
        return [[a, o, self.cur[(a, o)]] for (a, o) in sorted(self.written) if self.cur[(a, o)] != self.init[(a, o)]]


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def cls_float(bits_):
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0:
        return 'zero' if m == 0 else 'sub'
    return 'norm'


def base_result(mem, marks, ev=None, ret=None, st0=None):
    return {'events': ev or [], 'marks': marks, 'changes': mem.changes() if mem else [], 'ret': ret, 'st0': st0}


# ------------------------------------------------------------------------------------------------ funcion 0: CBoundingBox::IsPointWithin (0x49C300)
def expected0(c):
    a = c['a']
    marks = []
    v = [load(x & M32) for x in a[:9]]
    mn, mx, p = v[0:3], v[3:6], v[6:9]
    ok = all(ge(p[k], mn[k]) for k in range(3)) and all(le(p[k], mx[k]) for k in range(3))
    classes = [cls_float(x & M32) for x in a[:9]]
    has_nan = any(cl == 'nan' for cl in classes)
    if ok:
        marks.append('inside')
    if has_nan:
        marks.append('nan')
    elif any(cl == 'inf' for cl in classes):
        marks.append('infinite')
    if not has_nan:
        if any(num(mn[k]) > num(mx[k]) for k in range(3)):
            marks.append('inverted_box')
        if any(not ge(p[k], mn[k]) for k in range(3)):
            marks.append('below_min')
        if any(not le(p[k], mx[k]) for k in range(3)):
            marks.append('above_max')
        if any(num(p[k]) == num(mn[k]) or num(p[k]) == num(mx[k]) for k in range(3)):
            marks.append('boundary')
    return base_result(None, marks, ret=1 if ok else 0)


# ------------------------------------------------------------------------------------------------ funcion 1: CColBox::Set (0x40EE70)
def expected1(c):
    a = c['a']
    m = Arenas(c)
    for k in range(3):
        m.put(A_A1, 4 * k, a[k] & M32, 4)
        m.put(A_A2, 4 * k, a[3 + k] & M32, 4)
    marks = []
    sup = (A_OBJ, s32(a[10])) if a[9] & 1 else (A_A1, 0)
    inf = (A_OBJ, s32(a[11])) if a[9] & 2 else (A_A2, 0)
    for k in range(3):
        m.wr(A_OBJ, 4 * k, m.rd(sup[0], sup[1] + 4 * k, 4), 4)
    for k in range(3):
        m.wr(A_OBJ, 0xC + 4 * k, m.rd(inf[0], inf[1] + 4 * k, 4), 4)
    m.wr(A_OBJ, 0x18, a[6] & 0xFF, 1)
    m.wr(A_OBJ, 0x19, a[7] & 0xFF, 1)
    m.wr(A_OBJ, 0x1A, a[8] & 0xFF, 1)
    marks.append('sup_alias' if a[9] & 1 else 'sup_separate')
    marks.append('inf_alias' if a[9] & 2 else 'inf_separate')
    if (a[6] | a[7] | a[8]) & 0xFFFFFF00:
        marks.append('garbage_high_bytes')
    if any(cls_float(x & M32) == 'nan' for x in a[:6]):
        marks.append('nan_vector')
    return base_result(m, marks)


# ------------------------------------------------------------------------------------------------ funcion 2: CColBox::operator= (0x40EEC0)
def expected2(c):
    a = c['a']
    m = Arenas(c)
    src = (A_OBJ, s32(a[1])) if a[0] else (A_A1, 0)
    marks = ['src_inside' if a[0] else 'src_separate']
    # lecturas y escrituras en el orden del EXE
    for k in range(3):
        m.wr(A_OBJ, 4 * k, m.rd(src[0], src[1] + 4 * k, 4), 4)
    for k in range(3):
        m.wr(A_OBJ, 0xC + 4 * k, m.rd(src[0], src[1] + 0xC + 4 * k, 4), 4)
    m.wr(A_OBJ, 0x18, m.rd(src[0], src[1] + 0x18, 2), 2)
    m.wr(A_OBJ, 0x1A, m.rd(src[0], src[1] + 0x1A, 1), 1)
    if a[0] and s32(a[1]) == 0:
        marks.append('self')
    elif a[0] and abs(s32(a[1])) < 0x1C:
        marks.append('overlap')
    return base_result(m, marks, ret=tag(A_OBJ, 0))


# ------------------------------------------------------------------------------------------------ funcion 3: tColLighting::GetCurrentLighting (0x59F0C0)
K_LIGHT = 0x3D888889


def expected3(c):
    a = c['a']
    marks = []
    low = a[0] & 0xFF
    day, night = low & 0xF, low >> 4
    scale, bal = load(a[1] & M32), load(a[2] & M32)
    k = load(K_LIGHT)
    one = load(0x3F800000)
    t1 = mul(('f', Fraction(day), 0), scale)
    t1 = mul(t1, k)
    inv = sub(one, bal)
    u = mul(t1, inv)
    t2 = mul(('f', Fraction(night), 0), scale)
    t2 = mul(t2, k)
    t2 = mul(t2, bal)
    r = add(u, t2)
    cs, cb = cls_float(a[1] & M32), cls_float(a[2] & M32)
    marks.append('day_%s' % ('zero' if day == 0 else 'nonzero'))
    marks.append('night_%s' % ('zero' if night == 0 else 'nonzero'))
    if cs == 'nan' or cb == 'nan':
        marks.append('nan_input')
    if cs == 'inf' or cb == 'inf':
        marks.append('inf_input')
    if r[0] == 'n':
        marks.append('result_nan')
    elif r[0] == 'i':
        marks.append('result_inf')
    elif r[1] == 0:
        marks.append('result_zero')
    else:
        marks.append('result_finite')
        if abs(r[1]) > Fraction(2) ** 128:
            marks.append('beyond_float_range')
    if cb == 'norm' and (F(a[2]) < 0 or F(a[2]) > 1):
        marks.append('balance_outside_unit')
    if (a[0] & 0xFFFFFF00):
        marks.append('garbage_high_bytes')
    return base_result(None, marks, st0=ext80(r))


def F(bits_):
    import struct
    return struct.unpack('<f', struct.pack('<I', bits_ & M32))[0]


# ------------------------------------------------------------------------------------------------ funcion 4: CPad::sub_540530 (0x540530)
def expected4(c):
    a = c['a']
    mode = a[0] & 0xFFFF
    w = [x & 0xFFFF for x in a[1:7]]   # +0x10, +0x12, +0x1A, +0x40, +0x42, +0x4A
    marks = []
    if mode > 3:
        marks.append('mode_out_of_range')
        ret = 0
    elif mode == 1:
        marks.append('mode_1')
        up = w[0] != 0 and w[3] == 0
        down = w[1] != 0 and w[4] == 0
        ret = 1 if (up or down) else 0
        marks.append('up_new' if up else ('down_new' if down else 'no_new'))
    else:
        marks.append('mode_select')
        ret = 1 if (w[2] != 0 and w[5] == 0) else 0
        marks.append('select_new' if ret else 'select_not_new')
    marks.append('ret_%d' % ret)
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 5: FightHitLevel (0x5BD360)
def expected5(c):
    ch = c['a'][0] & 0xFF
    table = {0x48: 0, 0x4C: 1, 0x47: 2, 0x42: 3}
    ret = table.get(ch, 7)
    marks = ['level_%d' % ret]
    if ret == 7:
        marks.append('other_char')
    if ch == 0x48 and (c['a'][1] & 0xFF) == 0x4C or ch == 0x4C and (c['a'][1] & 0xFF) == 0x4C or ch == 0x47 and (c['a'][1] & 0xFF) == 0x4C:
        marks.append('word_pair')
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 6: FightHitSound (0x5BD3B0)
def expected6(c):
    n = c['a'][0] & M32
    marks = []
    if ((n - 1) & M32) > 7:
        ret = 0x3E
        marks.append('out_of_range')
    else:
        ret = [0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44][n - 1]
        marks.append('table_%d' % n)
    if n == 0x80000000 or n == 0:
        marks.append('wrap_edge')
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 7: approxEqual (0x4EEA80)
def expected7(c):
    a = c['a']
    marks = []
    f1, f2, eps = load(a[0] & M32), load(a[1] & M32), load(a[2] & M32)
    d = sub(f1, f2)
    ad = fabs_(d)
    ret = 1 if lt(ad, eps) else 0
    if any(cls_float(x & M32) == 'nan' for x in a[:3]):
        marks.append('nan')
    if ad[0] == 'n':
        pass
    elif ad[0] == 'i':
        marks.append('difference_infinite')
    else:
        if ad[1] > Fraction(2) ** 128 - Fraction(2) ** 104 and not is_nan(eps):
            marks.append('difference_beyond_float')
        if not is_nan(eps):
            marks.append('less' if num(ad) < num(eps) else ('equal' if num(ad) == num(eps) else 'greater'))
    marks.append('ret_%d' % ret)
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 8: ConvertPedNode2BoneTag (0x4D58A0)
NODE_TABLE = [3, 5, 32, 22, 34, 24, 41, 51, 43, 53, 52, 42, 33, 23, 31, 21, 4, 8]


def expected8(c):
    n = c['a'][0] & M32
    if ((n - 1) & M32) <= 0x11:
        ret = NODE_TABLE[n - 1]
        marks = ['node_%d' % n]
    else:
        ret = 0xFFFFFFFF
        marks = ['unknown_node']
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 9: CStuckCarCheck::ResetArrayElement (0x463970)
def expected9(c):
    idx = c['a'][0] & 0xFFFF
    m = Arenas(c)
    base = idx * 0x24
    for off in (4, 8, 0xC):
        m.wr(A_OBJ, base + off, 0xC59C4000, 4)
    m.wr(A_OBJ, base, 0xFFFFFFFF, 4)
    m.wr(A_OBJ, base + 0x10, 0xFFFFFFFF, 4)
    m.wr(A_OBJ, base + 0x14, 0, 4)
    m.wr(A_OBJ, base + 0x18, 0, 4)
    for off in range(0x1C, 0x22):
        m.wr(A_OBJ, base + off, 0, 1)
    marks = ['index_first' if idx == 0 else ('index_last' if idx == 15 else 'index_middle')]
    if c['a'][0] & 0xFFFF0000:
        marks.append('garbage_high_word')
    return base_result(m, marks)


# ------------------------------------------------------------------------------------------------ funcion 10: CEventDamage::ComputeHeadShot (0x4ADD60)
WEAPON_CLASS = [0] * 16 + [2] * 6 + [1] * 13 + [2] * 3 + [1] + [2] * 7 + [0] + [2] * 5 + [1]


def expected10(c):
    a = c['a']
    m = Arenas(c)
    weapon, piece, force = a[0] & M32, a[1] & M32, a[2] & 0xFF
    m.put(A_A1, 0, a[3] & 0xFF, 1)
    m.wr(A_A1, 0, 0, 1)
    marks = []
    result = 0
    if weapon > 0x34:
        marks.append('weapon_outside_table')
    else:
        cl = WEAPON_CLASS[weapon]
        marks.append(['melee', 'firearm', 'no_effect'][cl])
        if cl == 0:
            result = 1 if force else 0
        elif cl == 1:
            result = 1 if (force and piece == 9) else 0
    if result:
        m.wr(A_A1, 0, 1, 1)
        marks.append('headshot')
    else:
        marks.append('no_headshot')
    return base_result(m, marks)


# ------------------------------------------------------------------------------------------------ funcion 11: FindCutsceneAudioTrackId (0x5AFA50)
def _load_table():
    path = __file__.replace('fixture_oracle.py', 'cutscene_table.json')
    return [tuple(x) for x in json.load(open(path, encoding='utf-8'))]


_TABLE = None


def _low(ch):
    return ch + 32 if 0x41 <= ch <= 0x5A else ch


def expected11(c):
    global _TABLE
    if _TABLE is None:
        _TABLE = _load_table()
    a = c['a']
    s = bytearray()
    for k in range(6):
        for j in range(4):
            s.append((a[4 + k] >> (8 * j)) & 0xFF)
    n = a[0]
    name = bytes(s[:n])
    marks = []
    ret = 0xFFFF
    matched = b''
    for nm, ident in _TABLE:
        b = nm.encode('latin1')
        if len(b) == len(name) and all(_low(x) == _low(y) for x, y in zip(name, b)):
            ret = ident
            matched = b
            break
    kinds = ['exact', 'case_mix', 'prefix', 'name_plus', 'random_bytes', 'empty', 'one_char', 'one_letter_case']
    marks.append('found' if ret != 0xFFFF else 'not_found')
    marks.append(kinds[a[1]])
    if ret != 0xFFFF and any(x != y for x, y in zip(name, matched)):
        marks.append('found_other_case')
    return base_result(None, marks, ret=ret)


# ------------------------------------------------------------------------------------------------ funcion 12: CSimpleTransform::Invert (0x54EF90)
def expected12(c):
    a = c['a']
    m = Arenas(c)
    bar = A_OBJ if a[4] else A_A1
    for k in range(4):
        m.put(bar, 4 * k, a[k] & M32, 4)
    marks = ['alias' if a[4] else 'separate']
    h = a[3] & M32
    sn, cs = sin_cos(h)
    bx = load(m.rd(bar, 0, 4))
    by = load(m.rd(bar, 4, 4))
    t1 = chs(mul(cs, bx))
    t2 = mul(sn, by)
    m.wr(A_OBJ, 0, store(sub(t1, t2)), 4)
    bx = load(m.rd(bar, 0, 4))      # releido tras escribir +0 (con alias es el valor nuevo)
    by = load(m.rd(bar, 4, 4))
    u1 = mul(sn, bx)
    u2 = mul(cs, by)
    m.wr(A_OBJ, 4, store(sub(u1, u2)), 4)
    bz = load(m.rd(bar, 8, 4))
    m.wr(A_OBJ, 8, store(chs(bz)), 4)
    bh = load(m.rd(bar, 0xC, 4))
    m.wr(A_OBJ, 0xC, store(chs(bh)), 4)
    hc = cls_float(h)
    marks.append('heading_' + hc)
    if hc == 'zero':
        marks.append('heading_zero')
    if any(cls_float(a[k] & M32) == 'nan' for k in range(3)):
        marks.append('nan_component')
    if cls_float(a[2] & M32) == 'nan' and not (a[2] & 0x00400000):
        marks.append('z_signaling')
    if any(cls_float(a[k] & M32) == 'inf' for k in range(3)):
        marks.append('inf_component')
    return base_result(m, marks)


# ------------------------------------------------------------------------------------------------ funcion 13: TransformVectors (0x54EE30)
def expected13(c):
    a = c['a']
    m = Arenas(c)
    n = a[0]
    for k in range(18):
        m.put(A_IN, 4 * k, a[4 + k] & M32, 4)
    h = a[1] & M32
    sn, cs = sin_cos(h)
    cf = load(store(cs))
    sf = load(store(sn))
    out = A_IN if a[2] else A_OUT
    marks = ['alias' if a[2] else 'separate']
    marks.append('count_0' if n == 0 else ('count_1' if n == 1 else 'count_many'))
    for i in range(n):
        x = load(m.rd(A_IN, 12 * i, 4))
        y = load(m.rd(A_IN, 12 * i + 4, 4))
        m.wr(out, 12 * i, store(sub(mul(cf, x), mul(sf, y))), 4)
        x = load(m.rd(A_IN, 12 * i, 4))          # releido tras escribir x'
        y = load(m.rd(A_IN, 12 * i + 4, 4))
        m.wr(out, 12 * i + 4, store(add(mul(sf, x), mul(cf, y))), 4)
        m.wr(out, 12 * i + 8, m.rd(A_IN, 12 * i + 8, 4), 4)
    hc = cls_float(h)
    marks.append('heading_' + hc)
    if n and any(cls_float(a[4 + k] & M32) == 'nan' for k in range(3 * n)):
        marks.append('nan_component')
    if n and any(cls_float(a[4 + k] & M32) == 'inf' for k in range(3 * n)):
        marks.append('inf_component')
    return base_result(m, marks)


# ------------------------------------------------------------------------------------------------ funciones 14 y 15: garajes
def _types(a):
    out = []
    for k in range(GARAGE_COUNT):
        out.append((a[k // 4] >> (8 * (k % 4))) & 0xFF)
    return out


def _garage(c, radius):
    a = c['a']
    types = _types(a)
    mask = (a[13] & M32) | ((a[14] & M32) << 32)
    ev = []
    ret = 0
    pt = [a[15] & M32, a[16] & M32, a[17] & M32]
    marks = []
    calls = 0
    for k in range(GARAGE_COUNT):
        if types[k] == 0:
            continue
        calls += 1
        if radius:
            ev.append([51, k] + pt + [0x3F000000])
        else:
            ev.append([50, k] + pt)
        if (mask >> k) & 1:
            ret = 1
            marks.append('hit_at_%s' % ('first' if calls == 1 else 'later'))
            if k == GARAGE_COUNT - 1:
                marks.append('hit_last_garage')
            break
    if not ret:
        marks.append('no_hit')
    if calls == 0:
        marks.append('no_active')
    if calls >= 10:
        marks.append('many_calls')
    if any(t >= 0x80 for t in types):
        marks.append('high_type')
    return base_result(None, marks, ev=ev, ret=ret)


def expected14(c):
    return _garage(c, False)


def expected15(c):
    return _garage(c, True)


# ------------------------------------------------------------------------------------------------ funcion 16: IsTeleportPointValid (0x43EA90)
def expected16(c):
    a = c['a']
    org = [a[0] & M32, a[1] & M32, a[2] & M32]
    tgt = [a[3] & M32, a[4] & M32, a[5] & M32]
    marks = []
    ev = [[52] + tgt + [0x3EB33333, 0, 1, 1, 1, 1, 1, 0]]
    if (a[6] & M32) != 0:
        marks.append('sphere_blocked')
        return base_result(None, marks, ev=ev, ret=0)
    ev.append([53, tag(A_A1, 0), tag(A_A2, 0)] + org + tgt + [1, 1, 0, 1, 1, 0, 0])
    ret = a[7] & 0xFF
    marks.append('los_clear' if ret else 'los_blocked')
    if a[7] & 0xFFFFFF00:
        marks.append('garbage_high_bytes')
    return base_result(None, marks, ev=ev, ret=1 if ret else 0)


# ------------------------------------------------------------------------------------------------ funcion 17: COnscreenTimer::Init (0x44CBC0)
def expected17(c):
    m = Arenas(c)
    m.wr(A_OBJ, 0x151, 0, 1)
    for j in range(4):
        b = 0x40 + 0x44 * j
        for off in (0, 4, 8, 0xC):
            m.wr(A_OBJ, b + off, 0, 4)
        m.wr(A_OBJ, b + 0x10, 0, 2)
        m.wr(A_OBJ, b + 0x12, 0, 2)
        m.wr(A_OBJ, b + 0x3E, 0, 1)
        m.wr(A_OBJ, b + 0x3F, 1, 1)
    for off in (0, 4, 8):
        m.wr(A_OBJ, off, 0, 4)
    m.wr(A_OBJ, 0xC, 0, 2)
    m.wr(A_OBJ, 0x38, 0, 1)
    m.wr(A_OBJ, 0x39, 1, 1)
    m.wr(A_OBJ, 0x3C, 12, 4)
    return base_result(m, ['init'])


EXPECTED = [expected0, expected1, expected2, expected3, expected4, expected5, expected6, expected7, expected8, expected9, expected10, expected11, expected12, expected13, expected14, expected15, expected16, expected17]


def expected(c):
    return EXPECTED[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != e['st0']:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], e['st0'].hex())
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
