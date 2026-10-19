"""Oraculo independiente del banco FxEmitterCreateParticle30: FxEmitter_c::CreateParticle (EXE 0x4A2580, thiscall, 7 argumentos de pila, ret 0x1C).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente
extendido: cada fadd/fsub/fmul/fdiv/fsqrt se redondea (al par) a 24 bits de mantisa sin limite de exponente; fstp dword redondea ademas al rango float32 (subnormales de cuanto
2^-149, desbordes a infinito). Las cargas fld dword / fild son exactas. rand() es el del CRT del EXE (0x821B1E: seed = seed * 0x343FD + 0x269EC3; (seed >> 16) & 0x7FFF) y cada tirada
se usa como `rand() % 10000` (cdq; idiv 2710h). Ordenes de las tiradas y de las llamadas, de 0x4A2580 a 0x4A2B17:

  4A2589 GetParticle(0) [stub traza 1]; si no hay hueco: [0x62 de m_System] & 0x20 ? FreeUpParticle [2] + GetParticle(0) [1] : devuelve 0
  4A25C7 r1 = rand: [esi+0xC] = 0; [esi+8] = ((fild r1 * 0.0002f - 1.0f) * lifeBias + life) * mult.life ; [esi+0x28] = m_System
  4A2609 bytes [2C..31] = low8(ftol2(x * 255.0f)) con x = red, green, blue, alpha, size, rot ; [37] = createLocal
  4A2675 r2, r3, r4: bytes [32], [33], [34] = low8(ftol2((fild r * 0.0001f) * 255.0f)) ; [35] = low8(ftol2(brightness * 100.0f))
  4A2702 r5: [36] = 0xFF ; [38] = (fild r5 * 0.0001f) * (rotMax - rotMin) + rotMin
  4A2738 si [37] != 0: GetRWMatrix(wld) [3]
  4A2743 si !(|radius| >= 0.001 (double)): caja con r6, r7, r8; si no esfera con r6, r7, r8 y, solo si !(radius < 0), r9 (cuarta tirada)
         caja: x = float(r6 * 0.0001f * (maxX - minX) + minX), y idem, z (en registro) = r8 * 0.0001f * (maxZ - minZ) + minZ
         esfera: a, b, c = float(rem * 0.0002f - 1.0f); inv = 1.0 / sqrt((c*c + b*b) + a*a); escala = radius < 0 ? inv * radius : ((r9 * 0.0001f) * float(inv)) * radius;
                 x = float(a * escala), y = float(b * escala), z = escala * c
         pos = ((z' * at + y' * up) + x' * right) + wld.pos con x' = x + off.x, y' = y + off.y, z' = z + off.z -> floats en [0x10], [0x14], [0x18]
  4A2915 con velOverride: [0x1C..0x24] = copia; si no: rA, rB: randomAngle = float((rA * 0.0001f) * 6.28318f); minRad = float(angMin * 0.0174533f);
         ang = (rB * 0.0001f) * (angMax * 0.0174533f - minRad) + minRad; prodB = ang * 40.7437f; sinB = T[low8(ftol2(prodB))]; prodA = randomAngle * 40.7437f;
         v = {T[low8(ftol2(prodA + 64))] * sinB, T[low8(ftol2(prodB + 64))], T[low8(ftol2(float(prodA)))] * sinB}; (T = tabla de senos 0xBB3E00)
         dir.x > 10 (ordenado) ? vin = pos de la particula : vin = direccion y RwV3dNormalize(vin, vin) [4]
         RwV3dTransformVectors(vout, vin, 1, wld) [5]; RotateVecIntoVec(rot, v, vout) [6]; rC = rand; k = ((fild rC * 0.0002f - 1.0f) * speedBias) + speed; vel = float(rot * k)
  4A2AD2 vel[i] = float(velAdd[i] + vel[i]); UpdateParticle(timeSince, prt) [7]; lista de m_PrimBP+0x20: AddItem(prt); devuelve la particula.

NaN (solo silenciosos en las fixtures; los senalizantes dependen de la forma de la instruccion x87, memoria o registro): una operacion con un unico NaN devuelve ese NaN tal cual; con dos
NaN devuelve el de mayor significando; si tienen el mismo significando y distinto signo el resultado es DESCONOCIDO (comodin None en la salida: solo se exige EXE == DLL); operacion invalida
(inf - inf, 0 * inf, 0/0, inf/inf, raiz de negativo) sin NaN de entrada devuelve el indefinido real (0xFFC00000). Un resultado desconocido contamina los que dependen de el.
"""
import math
import struct
from fractions import Fraction

CASES = 4096
NAME = 'FxEmitter_c::CreateParticle'
NAMES = [NAME]
M32 = 0xFFFFFFFF
INDEFINITE = 0xFFC00000

# ------------------------------------------------------------------------------------------------------------------------------ espejo de Fixtures.hpp
K_EMITTER, K_PRIM, K_SYSTEM, K_EMISSION, K_MULT, K_WLD, K_VEL, K_PART_A, K_PART_B, K_ITEM, K_ARENA = 0x000, 0x040, 0x0C0, 0x140, 0x1C0, 0x1E0, 0x220, 0x230, 0x270, 0x2B0, 0x2F0
K_GUARD = 16
# suma de huecos igual a la de Fixtures.hpp (static_assert alli; aqui la misma comprobacion): emisor 0x40, prim 0x80, sistema 0x80, emision 0x80, mult 0x20, wld 0x40, vel 0x10, 2 particulas 0x40 y elemento 0x40
assert (K_PRIM, K_SYSTEM, K_EMISSION, K_MULT, K_WLD, K_VEL, K_PART_A, K_PART_B, K_ITEM, K_ARENA) == (0x40, 0xC0, 0x140, 0x1C0, 0x1E0, 0x220, 0x230, 0x270, 0x2B0, 0x2F0)
assert K_PRIM == K_EMITTER + 0x40 and K_SYSTEM == K_PRIM + 0x80 and K_EMISSION == K_SYSTEM + 0x80 and K_MULT == K_EMISSION + 0x80 and K_WLD == K_MULT + 0x20 and K_VEL == K_WLD + 0x40
assert K_PART_A == K_VEL + 0x10 and K_PART_B == K_PART_A + 0x40 and K_ITEM == K_PART_B + 0x40 and K_ARENA == K_ITEM + 0x40
VTABLE = 0x0085A7A4
EMISSION_WORDS = 26
SPECIAL = [
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7FFFFFFF, 0x7F7FFFFF, 0xFF7FFFFF,
    0x3F000000, 0x40000000, 0x41200000, 0x42C80000, 0x3A83126F, 0x437F0000, 0x477FFF00, 0x4B000000, 0x3F7FFFFF, 0x3F800001, 0x3C23D70A, 0x3E800000, 0x437F8000, 0x42C7FFFF, 0x7F000000, 0x00FFFFFF,
    0x3FC90FDB, 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0x7149F2CA, 0x0DA24260]
K_UNIT, K_ANGLE, K_LIFE, K_COLOR, K_SCALE, K_POS = range(6)
RANGES = [(118, 130), (126, 137), (120, 130), (115, 127), (112, 127), (120, 133)]
EMISSION_KINDS = [K_UNIT, K_SCALE, K_POS, K_POS, K_POS, K_POS, K_POS, K_POS, K_UNIT, K_UNIT, K_UNIT, K_UNIT, K_UNIT, K_ANGLE, K_ANGLE, K_LIFE, K_UNIT, K_POS, K_POS, K_POS, K_ANGLE, K_ANGLE, K_UNIT, K_UNIT, K_UNIT, K_UNIT]


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


def val(c, key, kind):
    fam = c % 8
    dice = rnd(c, key + 1000) % 100
    special, extreme = 3, 2
    if fam == 4:
        special, extreme = 40, 5
    elif fam == 5:
        special, extreme = 5, 40
    elif fam == 6:
        special, extreme = 20, 20
    elif fam == 7:
        special, extreme = 10, 5
    r = rnd(c, key)
    if dice < special:
        return SPECIAL[r % len(SPECIAL)]
    mant = r & 0x7FFFFF
    sign = (r >> 31) & 1
    if dice < special + extreme:
        e = 1 + (rnd(c, key + 3000) % 24) if (rnd(c, key + 2000) & 1) else 231 + (rnd(c, key + 3000) % 24)
        return (sign << 31) | (e << 23) | mant
    lo, hi = RANGES[kind]
    e = lo + rnd(c, key + 2000) % (hi - lo + 1)
    s = ((1 if (r >> 29) % 8 == 0 else 0) if kind in (K_COLOR, K_SCALE, K_LIFE) else sign)
    return (s << 31) | (e << 23) | mant


def radius_value(c, kind):
    if kind == 0:
        return 0
    if kind == 1:
        return 0x3A03126F if (rnd(c, 60) & 1) else 0xBA03126F
    if kind == 2:
        return 0x3A83126F
    if kind == 3:
        return 0x3A83126E
    if kind == 4:
        return val(c, 61, K_SCALE) & 0x7FFFFFFF
    if kind == 5:
        return val(c, 62, K_SCALE) | 0x80000000
    if kind == 6:
        return SPECIAL[rnd(c, 63) % len(SPECIAL)]
    return 0xBA83126F


def dirx_value(c, kind):
    if kind == 0:
        return val(c, 70, K_UNIT)
    if kind == 1:
        return (0x41200001 + (rnd(c, 71) % 64)) & M32
    if kind == 2:
        return 0x41200000
    if kind == 3:
        return 0x7FC00000 | (rnd(c, 72) & 0x3FFFFF)
    if kind == 4:
        return 0x7F800000 if (rnd(c, 73) & 1) else 0xFF800000
    return (0x41200000 + (rnd(c, 74) % 5000)) & M32


def make(c):
    f = {'family': c % 8}
    em = [val(c, 100 + i, EMISSION_KINDS[i]) for i in range(EMISSION_WORDS)]
    f['radiusKind'] = rnd(c, 50) % 8
    f['dirKind'] = rnd(c, 51) % 6
    em[1] = radius_value(c, f['radiusKind'])
    em[10] = dirx_value(c, f['dirKind'])
    f['emission'] = em
    f['mult'] = [val(c, 200 + i, K_SCALE if i in (4, 5) else (K_LIFE if i == 6 else K_COLOR)) for i in range(7)]
    f['wld'] = [val(c, 300 + i, K_UNIT) for i in range(16)]
    f['getRw'] = [val(c, 400 + i, K_UNIT) for i in range(12)]
    f['velOverride'] = [val(c, 500 + i, K_UNIT) for i in range(3)]
    f['velAdd'] = [val(c, 510 + i, K_UNIT) for i in range(3)]
    f['timeSince'] = val(c, 520, K_LIFE)
    f['brightness'] = val(c, 521, K_COLOR)
    f['hasVel'] = int(rnd(c, 530) % 4 == 0)
    f['createLocal'] = int(rnd(c, 531) % 3 == 0)
    f['mustCreate'] = rnd(c, 532) & 1
    f['poolMode'] = 0 if (rnd(c, 533) % 10) < 7 else (1 if (rnd(c, 534) & 1) else 2)
    f['listMode'] = rnd(c, 535) & 1
    f['flagsOther'] = rnd(c, 536) & 0xDF
    f['fill'] = rnd(c, 537) % 4
    seed = rnd(c, 540)
    if c % 16 == 3 or c % 16 == 11:
        want = 0 if c % 16 == 3 else 9999
        for i in range(100000):
            nxt = (seed * 0x343FD + 0x269EC3) & M32
            if ((nxt >> 16) & 0x7FFF) % 10000 == want:
                break
            seed = mix((seed + i + 1) & M32)
    f['seed'] = seed
    return f


# ------------------------------------------------------------------------------------------------------------------------------ x87
class V:
    """Valor x87: kind 'f' finito (val Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits float32 silencioso), 'unk' (NaN de carga util/signo desconocidos)."""
    __slots__ = ('kind', 'val', 'neg', 'bits')

    def __init__(self, kind, val=Fraction(0), neg=False, bits=0):
        self.kind, self.val, self.neg, self.bits = kind, val, neg, bits

    def is_nan(self):
        return self.kind in ('nan', 'unk')

    def sign(self):
        return self.neg if (self.kind == 'inf' or (self.kind == 'f' and self.val == 0)) else self.val < 0

    def is_zero(self):
        return self.kind == 'f' and self.val == 0


def load(bits):
    """fld dword: exacta."""
    bits &= M32
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man:
            return V('nan', bits=bits | 0x400000)
        return V('inf', neg=bool(sign))
    if exp == 0:
        v = Fraction(man, 1 << 149)
    else:
        v = Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127))
    if sign:
        v = -v
    return V('f', v, neg=bool(sign))


def fild(i):
    return V('f', Fraction(i), neg=False)


def r24(x):
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


def nan_pick(a, b):
    if a.kind == 'unk' or b.kind == 'unk':
        return V('unk')
    if a.is_nan() and b.is_nan():
        ma, mb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if ma > mb:
            return a
        if mb > ma:
            return b
        if (a.bits >> 31) == (b.bits >> 31):
            return a
        return V('unk')
    return a if a.is_nan() else b


def indefinite():
    return V('nan', bits=INDEFINITE)


def neg(v):
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return V('inf', neg=not v.neg)
    if v.val == 0:
        return V('f', Fraction(0), neg=not v.neg)
    return V('f', -v.val, neg=v.val > 0)


def add(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf':
            return a if a.neg == b.neg else indefinite()
        return a if a.kind == 'inf' else b
    s = a.val + b.val
    if s == 0:
        if a.val == 0 and b.val == 0:
            return V('f', Fraction(0), neg=a.neg and b.neg)
        return V('f', Fraction(0), neg=False)
    s = r24(s)
    return V('f', s, neg=s < 0)


def sub(a, b):
    return add(a, neg(b))


def mul(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf' or b.kind == 'inf':
        if a.is_zero() or b.is_zero():
            return indefinite()
        return V('inf', neg=sg)
    p = a.val * b.val
    if p == 0:
        return V('f', Fraction(0), neg=sg)
    p = r24(p)
    return V('f', p, neg=p < 0)


def div(a, b):
    """a / b."""
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf':
        return indefinite() if b.kind == 'inf' else V('inf', neg=sg)
    if b.kind == 'inf':
        return V('f', Fraction(0), neg=sg)
    if b.is_zero():
        return indefinite() if a.is_zero() else V('inf', neg=sg)
    if a.is_zero():
        return V('f', Fraction(0), neg=sg)
    q = r24(a.val / b.val)
    return V('f', q, neg=q < 0)


def sqrt(a):
    if a.is_nan():
        return a
    if a.kind == 'inf':
        return indefinite() if a.neg else a
    if a.val == 0:
        return a
    if a.val < 0:
        return indefinite()
    x = a.val
    # x = n / d con d potencia de dos (todo es diadico): se escala a un entero con un numero par de bits fraccionarios y mas de 80 bits
    n, d = x.numerator, x.denominator
    e = d.bit_length() - 1
    assert d == 1 << e
    s = max(0, (80 - n.bit_length() + 1) // 2 + 1)
    if (e + 2 * s) % 2:
        s += 1
    big = n << (2 * s)
    t = math.isqrt(big)
    exact = t * t == big
    # valor = t / 2^(s + e/2 ...): big/2^(e+2s) = x * 4^s ... raiz = sqrt(big) / 2^(s + e/2); e puede ser impar: ajustar multiplicando por 2
    if e % 2:
        big = n << (2 * s + 1)
        t = math.isqrt(big)
        exact = t * t == big
        scale = Fraction(1, 1 << (s + (e + 1) // 2))
    else:
        scale = Fraction(1, 1 << (s + e // 2))
    r = (Fraction(t) + (Fraction(0) if exact else Fraction(1, 2))) * scale
    r = r24(r)
    return V('f', r, neg=False)


def store_float(v):
    """fstp dword: bits del float32 (NaN/inf sin cambios; desbordes a infinito; subnormales con cuanto 2^-149)."""
    if v.kind == 'unk':
        return None
    if v.kind == 'nan':
        return v.bits
    if v.kind == 'inf':
        return 0xFF800000 if v.neg else 0x7F800000
    if v.val == 0:
        return 0x80000000 if v.neg else 0
    s = 1 if v.val < 0 else 0
    a = abs(v.val)
    if a >= Fraction(2) ** 128:
        return (s << 31) | 0x7F800000
    if a < Fraction(2) ** -126:
        q = a / Fraction(1, 1 << 149)
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
            n += 1
        return (s << 31) | n                                   # n == 0x800000 pasa a ser el menor normal
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    m = a / (Fraction(2) ** (e - 23))            # entero de 24 bits (el valor ya viene redondeado a 24 bits)
    assert m.denominator == 1 and 1 << 23 <= m.numerator < 1 << 24, (a, m)
    return (s << 31) | ((e + 127) << 23) | (m.numerator & 0x7FFFFF)


def ftol_low(v):
    """_ftol2 del EXE: palabra baja del entero de 64 bits truncado hacia cero; 0 si NaN, infinito o fuera de rango (entero indefinido). None si desconocido."""
    if v.kind == 'unk':
        return None
    if v.kind in ('nan', 'inf'):
        return 0
    t = int(v.val)            # trunca hacia cero
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    return t & M32


def fbits(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


# ------------------------------------------------------------------------------------------------------------------------------ constantes del EXE (floats exactos)
C_0001 = load(0x38D1B717)      # 0x858FC4
C_0002 = load(0x3951B717)      # 0x85A7D0
C_ONE = load(0x3F800000)       # 0x858624
C_255 = load(0x437F0000)       # 0x859AAC
C_100 = load(0x42C80000)       # 0x858628
C_TWOPI = load(0x40C90FD0)     # 0x85A7C0
C_DEG = load(0x3C8EFA2E)       # 0x85A7BC
C_TABSCALE = load(0x4222F983)     # 0x85A778
C_64 = load(0x42800000)        # 0x859A44
THRESH_D = float(struct.unpack('<f', struct.pack('<I', 0x3A83126F))[0])   # qword 0x85A7C8 = (double)0.001f = 0.0010000000474974513


def fnv(words):
    """FNV-1a de 32 bits sobre los bytes (little endian) de una lista de palabras; None si alguna palabra es desconocida."""
    if any(w is None for w in words):
        return None
    h = 2166136261
    for w in words:
        for k in range(4):
            h ^= (w >> (8 * k)) & 0xFF
            h = (h * 16777619) & M32
    return h


def half_bits(v):
    e = (v >> 23) & 0xFF
    if e == 0xFF:
        return (v | 0x400000) if (v & 0x7FFFFF) else v
    if e >= 2:
        return (v - 0x00800000) & M32
    return v & 0x80000000


def rem_stream(seed):
    state = [seed & M32]

    def nxt():
        state[0] = (state[0] * 0x343FD + 0x269EC3) & M32
        return ((state[0] >> 16) & 0x7FFF) % 10000

    def raw_next():
        state[0] = (state[0] * 0x343FD + 0x269EC3) & M32
        return (state[0] >> 16) & 0x7FFF
    return nxt, raw_next


def thresh_ge(bits):
    """fld [radius]; fabs; fcomp qword [0x85A7C8]; NOT(C0): |radio| >= 0.001 (ordenado)."""
    v = load(bits)
    if v.is_nan():
        return False
    if v.kind == 'inf':
        return True
    return abs(v.val) >= Fraction(THRESH_D)


def less_zero(bits):
    v = load(bits)
    if v.is_nan():
        return False
    if v.kind == 'inf':
        return v.neg
    return v.val < 0 if v.val != 0 else False


def greater_ten(bits):
    v = load(bits)
    if v.is_nan():
        return False
    if v.kind == 'inf':
        return not v.neg
    return v.val > 10


# ------------------------------------------------------------------------------------------------------------------------------ arena y funcion
def build_arena(base, f):
    """Mismo diseno que BuildArena de Fixtures.hpp; `base` = direccion real del arena en el lado medido. Devuelve una lista de palabras de 32 bits (kArena / 4)."""
    fills = [0x5C, 0xC3, 0xA5, 0x00]
    b = bytearray([fills[f['fill']]]) * K_ARENA

    def put(at, v):
        b[at:at + 4] = struct.pack('<I', v & M32)
    put(K_EMITTER + 0, VTABLE)
    put(K_EMITTER + 4, base + K_PRIM)
    put(K_EMITTER + 8, base + K_SYSTEM)
    b[K_EMITTER + 0xC] = 1
    put(K_EMITTER + 0x10, 0x3F000000)
    b[K_SYSTEM + 0x62] = (f['flagsOther'] | (0x20 if f['mustCreate'] else 0)) & 0xFF
    for i in range(3):
        put(K_SYSTEM + 0x68 + 4 * i, f['velAdd'][i])
    for i in range(EMISSION_WORDS):
        put(K_EMISSION + 4 * i, f['emission'][i])
    for i in range(7):
        put(K_MULT + 4 * i, f['mult'][i])
    for i in range(16):
        put(K_WLD + 4 * i, f['wld'][i])
    for i in range(3):
        put(K_VEL + 4 * i, f['velOverride'][i])
    if f['listMode'] == 1:
        put(K_PRIM + 0x20, base + K_ITEM)
        put(K_PRIM + 0x24, base + K_ITEM)
        put(K_PRIM + 0x28, 1)
        put(K_ITEM + 0, 0)
        put(K_ITEM + 4, 0)
    else:
        put(K_PRIM + 0x20, 0)
        put(K_PRIM + 0x24, 0)
        put(K_PRIM + 0x28, 0)
    return b


def run(f, table, base):
    """Ejecuta CreateParticle sobre el modelo. Devuelve (arena: lista de bytes o None, trace: lista de enteros o None, ret: desplazamiento o 0xFFFFFFFF, rand_next, stats)."""
    mem = bytearray(build_arena(base, f))
    unk = set()                                    # desplazamientos de bytes desconocidos
    trace = []
    stats = {}
    nxt0, raw_next = rem_stream(f['seed'])
    draws = []

    def nxt():
        d = nxt0()
        draws.append(d)
        return d

    def w32(at):
        return struct.unpack_from('<I', mem, at)[0] if not any((at + k) in unk for k in range(4)) else None

    def put32(at, v):
        if v is None:
            for k in range(4):
                unk.add(at + k)
            return
        for k in range(4):
            unk.discard(at + k)
        struct.pack_into('<I', mem, at, v & M32)

    def put8(at, v):
        if v is None:
            unk.add(at)
            return
        unk.discard(at)
        mem[at] = v & 0xFF

    def words(at, n):
        return [w32(at + 4 * i) for i in range(n)]

    def ldw(at):
        w = w32(at)
        return V('unk') if w is None else load(w)

    em = f['emission']
    mult = f['mult']
    ex = lambda i: load(em[i])
    mu = lambda i: load(mult[i])

    ret = 0xFFFFFFFF
    # --- pool
    slot = {'0': (K_PART_A, None), '1': (None, K_PART_B), '2': (None, None)}[str(f['poolMode'])]
    trace.extend([1, 0])
    prt = slot[0]
    stats['list_nonempty'] = f['listMode']
    stats['local'] = f['createLocal']
    if prt is None:
        if not mem[K_SYSTEM + 0x62] & 0x20:
            stats['pool_null'] = 1
            return list(mem), trace, ret, raw_next(), stats, unk
        trace.append(2)
        trace.extend([1, 0])
        prt = slot[1]
        if prt is None:
            stats['pool_null'] = 1
            return list(mem), trace, ret, raw_next(), stats, unk
        stats['pool_freeup'] = 1
    stats['particle'] = 1
    # --- vida
    r = nxt()
    put32(prt + 0xC, 0)
    life = mul(add(mul(sub(mul(fild(r), C_0002), C_ONE), ex(16)), ex(15)), mu(6))
    put32(prt + 8, store_float(life))
    put32(prt + 0x28, base + K_SYSTEM)
    # --- colores
    for i, at in enumerate((0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31)):
        put8(prt + at, ftol_low(mul(mu(i), C_255)))
    put8(prt + 0x37, f['createLocal'])
    for at in (0x32, 0x33, 0x34):
        r = nxt()
        put8(prt + at, ftol_low(mul(mul(fild(r), C_0001), C_255)))
    put8(prt + 0x35, ftol_low(mul(load(f['brightness']), C_100)))
    r = nxt()
    put8(prt + 0x36, 0xFF)
    rotation = add(mul(mul(fild(r), C_0001), sub(ex(21), ex(20))), ex(20))
    put32(prt + 0x38, store_float(rotation))
    if f['createLocal']:
        trace.extend([3, K_PRIM, K_WLD])
        g = f['getRw']
        for i in range(3):
            put32(K_WLD + 4 * i, g[i])
            put32(K_WLD + 0x10 + 4 * i, g[3 + i])
            put32(K_WLD + 0x20 + 4 * i, g[6 + i])
            put32(K_WLD + 0x30 + 4 * i, g[9 + i])
    # --- posicion
    wld = [ldw(K_WLD + 4 * i) for i in range(16)]

    def transform(x, y, z):
        """x', y' (floats ya guardados + off) y z' (registro): ((z'*at + y'*up) + x'*right) + pos -> 3 floats."""
        X = add(x, ex(17))
        Y = add(y, ex(18))
        Z = add(z, ex(19))
        out = []
        for k in range(3):
            t = add(mul(Z, wld[8 + k]), mul(Y, wld[4 + k]))
            t = add(t, mul(X, wld[0 + k]))
            t = add(t, wld[12 + k])
            out.append(store_float(t))
        return out

    box = not thresh_ge(em[1])
    stats['box'] = int(box)
    if box:
        rx, ry, rz = nxt(), nxt(), nxt()
        # eje a eje: (rem * 0.0001f) * (max - min) + min ; x e y pasan por float, z no
        x = add(mul(mul(fild(rx), C_0001), sub(ex(5), ex(2))), ex(2))
        y = add(mul(mul(fild(ry), C_0001), sub(ex(6), ex(3))), ex(3))
        z = add(mul(mul(fild(rz), C_0001), sub(ex(7), ex(4))), ex(4))
        xb, yb = store_float(x), store_float(y)
        px = V('unk') if xb is None else load(xb)
        py = V('unk') if yb is None else load(yb)
        pos = transform(px, py, z)
    else:
        rx, ry, rz = nxt(), nxt(), nxt()
        a = sub(mul(fild(rx), C_0002), C_ONE)
        b = sub(mul(fild(ry), C_0002), C_ONE)
        c3 = sub(mul(fild(rz), C_0002), C_ONE)
        ab, bb, cb = store_float(a), store_float(b), store_float(c3)
        a = V('unk') if ab is None else load(ab)
        b = V('unk') if bb is None else load(bb)
        c3 = V('unk') if cb is None else load(cb)
        ssum = add(add(mul(c3, c3), mul(b, b)), mul(a, a))
        inv = div(load(0x3F800000), sqrt(ssum))
        radius = load(em[1])
        if less_zero(em[1]):
            scale = mul(inv, radius)
            stats['sphere_neg'] = 1
        else:
            r9 = nxt()
            ib = store_float(inv)
            invf = V('unk') if ib is None else load(ib)
            scale = mul(mul(mul(fild(r9), C_0001), invf), radius)
            stats['sphere_pos'] = 1
        xs = store_float(mul(a, scale))
        ys = store_float(mul(b, scale))
        z = mul(scale, c3)
        pos = transform(V('unk') if xs is None else load(xs), V('unk') if ys is None else load(ys), z)
    for i in range(3):
        put32(prt + 0x10 + 4 * i, pos[i])
    # --- velocidad
    if f['hasVel']:
        stats['vel_override'] = 1
        for i in range(3):
            put32(prt + 0x1C + 4 * i, f['velOverride'][i])
    else:
        rA, rB = nxt(), nxt()
        ra = store_float(mul(mul(fild(rA), C_0001), C_TWOPI))
        randomAngle = V('unk') if ra is None else load(ra)
        mr = store_float(mul(ex(13), C_DEG))
        minRad = V('unk') if mr is None else load(mr)
        maxRad = mul(ex(14), C_DEG)
        ang = add(mul(mul(fild(rB), C_0001), sub(maxRad, minRad)), minRad)
        prodB = mul(ang, C_TABSCALE)
        idx = ftol_low(prodB)
        sinB = V('unk') if idx is None else load(table[idx & 0xFF])
        prodA = mul(randomAngle, C_TABSCALE)
        prodAs = store_float(prodA)
        def tab(v):
            i = ftol_low(v)
            return V('unk') if i is None else load(table[i & 0xFF])
        vx = store_float(mul(tab(add(prodA, C_64)), sinB))
        vy = store_float(tab(add(prodB, C_64)))
        vz = store_float(mul(tab(V('unk') if prodAs is None else load(prodAs)), sinB))
        rvec = [V('unk') if t is None else load(t) for t in (vx, vy, vz)]
        vbits = [vx, vy, vz]
        stats['dir_norm' if not greater_ten(em[10]) else 'dir_pos'] = 1
        if not greater_ten(em[10]):
            vin = [em[10], em[11], em[12]]
            trace.extend([4, 0xFFFFFFFE, 0xFFFFFFFE, fnv(vin), 1])
            vin = [None if w is None else half_bits(w) for w in vin]
        else:
            vin = [w32(prt + 0x10 + 4 * i) for i in range(3)]
        vold_in = vin
        trace.extend([5, 0xFFFFFFFE, 0xFFFFFFFE, 1, K_WLD, fnv(vin), fnv(words(K_WLD, 16))])
        vout = [None if w is None else half_bits(w) for w in vin]
        trace.extend([6, 0xFFFFFFFE, 0xFFFFFFFE, 0xFFFFFFFE, fnv(vbits), fnv(vout)])
        rot = [None if w is None else half_bits(w) for w in vbits]
        rC = nxt()
        k = add(mul(sub(mul(fild(rC), C_0002), C_ONE), ex(9)), ex(8))
        for i in range(3):
            rv = V('unk') if rot[i] is None else load(rot[i])
            put32(prt + 0x1C + 4 * i, store_float(mul(rv, k)))
    for i in range(3):
        va = load(f['velAdd'][i])
        cur = ldw(prt + 0x1C + 4 * i)
        put32(prt + 0x1C + 4 * i, store_float(add(va, cur)))
    trace.extend([7, K_PRIM, f['timeSince'], prt, fnv(words(prt, 15))])
    # --- lista: AddItem
    head = w32(K_PRIM + 0x20)
    put32(K_PRIM + 0x20, base + prt)
    put32(prt + 0, 0)
    put32(prt + 4, head)
    if head:
        put32((head - base) + 0, base + prt)
    else:
        put32(K_PRIM + 0x24, base + prt)
    put32(K_PRIM + 0x28, (w32(K_PRIM + 0x28) + 1) & M32)
    ret = prt
    stats['rem0_first'] = int(draws[0] == 0)
    stats['rem9999_first'] = int(draws[0] == 9999)
    pw = [w32(prt + 0x10 + 4 * i) for i in range(3)] + [w32(prt + 0x1C + 4 * i) for i in range(3)]
    stats['nan_out'] = int(any(w is not None and (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) for w in pw))
    stats['inf_out'] = int(any(w is not None and (w & 0x7FFFFFFF) == 0x7F800000 for w in pw))
    stats['finite_out'] = int(all(w is not None and (w & 0x7F800000) != 0x7F800000 for w in pw))
    stats['unknown'] = int(bool(unk))
    return list(mem), trace, ret, raw_next(), stats, unk


# ------------------------------------------------------------------------------------------------------------------------------ interfaz con run_local.py
def expected(inp, case, table, base):
    """`inp` = la fila `input` registrada por el banco (los mismos campos que make()). Devuelve (want, stats); want['arena'] y want['trace'] llevan None donde el valor es desconocido."""
    f = dict(inp)
    mem, trace, ret, rand_next, stats, unk = run(f, table, base)
    arena = [None if i in unk else mem[i] for i in range(K_ARENA)]
    return {'arena': arena, 'trace': trace, 'ret': ret, 'rand_next': rand_next}, stats


def matches(observed, want):
    """observed = {'arena': hex, 'trace': [...], 'ret': int, 'rand_next': int}."""
    arena = bytes.fromhex(observed['arena'])
    if len(arena) != K_ARENA:
        return False
    for got, exp in zip(arena, want['arena']):
        if exp is not None and got != exp:
            return False
    tr = observed['trace']
    if len(tr) != len(want['trace']) or any(e is not None and g != e for g, e in zip(tr, want['trace'])):
        return False
    return observed['ret'] == want['ret'] and observed['rand_next'] == want['rand_next']


def default_table():
    """Tabla de senos sintetica del arnes offline (en el juego la tabla real se registra en el evento `begin`)."""
    out = []
    for i in range(256):
        v = mix((i * 7919 + 3) & M32)
        out.append(((v >> 3) & 0x007FFFFF) | (0x3F000000 if (v & 1) else 0xBF000000))
    return out


def self_test():
    """Autoprueba: valores exactos calculados a mano y propiedades del modelo x87."""
    # redondeo a 24 bits al par
    assert r24(Fraction(1) + Fraction(1, 1 << 24)) == 1                       # empate exacto: al par (1.0)
    assert r24(Fraction(1) + Fraction(3, 1 << 24)) == 1 + Fraction(1, 1 << 22)
    # fild * 0.0001f * 255.0f para rem = 9999: ((9999 * 0.0001f) rounded) * 255
    assert store_float(load(0x3F800000)) == 0x3F800000
    assert store_float(V('f', Fraction(2) ** 128)) == 0x7F800000                 # desborde a infinito
    assert store_float(V('f', Fraction(1, 1 << 149))) == 1                       # menor subnormal
    assert store_float(V('f', Fraction(3, 1 << 150))) == 2                       # 1.5 cuantos: al par (2)
    assert ftol_low(V('f', Fraction(-7, 2))) == (-3) & M32 and ftol_low(V('f', Fraction(2 ** 63))) == 0 and ftol_low(V('inf')) == 0
    assert store_float(sqrt(V('f', Fraction(2)))) == 0x3FB504F3                  # sqrt(2) a 24 bits
    assert store_float(sqrt(V('f', Fraction(1, 4)))) == 0x3F000000
    assert store_float(div(V('f', Fraction(1)), V('f', Fraction(3)))) == 0x3EAAAAAB
    assert store_float(sqrt(V('f', Fraction(-1)))) == INDEFINITE
    assert store_float(div(V('f', Fraction(1)), V('f', Fraction(0)))) == 0x7F800000
    assert store_float(mul(V('inf'), V('f', Fraction(0)))) == INDEFINITE
    # generador del CRT del EXE: srand(1) -> 41, 18467, 6334 (los de siempre de MSVC)
    nx, rw = rem_stream(1)
    assert [rw(), rw(), rw()] == [41, 18467, 6334]
    # media de float por bits (colaboradores)
    assert half_bits(0x3F800000) == 0x3F000000 and half_bits(0x00800000) == 0 and half_bits(0x7F800000) == 0x7F800000 and half_bits(0x7F800001) == 0x7FC00001
    # fixtures: 4096 casos con las 8 familias
    assert len({make(c)['family'] for c in range(64)}) == 8
    return 'ok'


if __name__ == '__main__':
    print(self_test())
