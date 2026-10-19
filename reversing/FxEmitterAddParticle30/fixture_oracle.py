"""Oraculo independiente del banco FxEmitterAddParticle30: FxEmitter_c::AddParticle(const CVector& pos, const CVector& vel, float, const FxPrtMult_c&, float rotZ, float, bool)
(EXE 0x4A3EA0, thiscall, 7 argumentos de pila, ret 0x1C; slot +0x14 de la vtable de FxEmitter_c).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. Orden de las llamadas (0x4A3EA0..0x4A403F):

  4A3EC8 ProcessEmissionInfo(this = &primBP->m_FxInfoManager, 0.0f, 0.0f, systemBP->m_fLength, m_UseConstTime (byte), &emision)            [traza 1]
  4A3ECD FxRwMatrixCreate x3: A (resultado), B (mundo), C (identidad)                                                                       [traza 2 x3]
  4A3EF7 C: right.x = up.y = at.z = 1.0, flags |= 0x20003, el resto de las columnas a 0, pos = *pos (los rellenos +0x1C, +0x2C, +0x3C intactos)
  4A3F44 RwMatrixUpdate(C)                                                                                                                   [traza 5]
  4A3F49 padre != 0 ? RwMatrixMultiply(B, C, padre)                                                                                          [traza 6]  : B = C (rep movs, 16 palabras)
  4A3F72 FxRwMatrixCreate -> D; GetRWMatrix(primBP, D) [4]; RwMatrixMultiply(A, D, B) [6]; FxRwMatrixDestroy(D) [3]
  4A3FDF CreateParticle(this, &emision, A, &vel, timeSince, &mult, brightness, createLocal) [8]  -> EAX = particula o 0
  4A3FEA particula != 0 y !(rotZ < 0 || NaN)  (fld rotZ; fcomp 0.0; test ah, 1; jne): [particula + 0x36] = low8(_ftol2(rotZ * 0.5f))
  4A4013 FxRwMatrixDestroy(C), (B), (A)                                                                                                      [3 x3]

Los colaboradores sustituidos escriben: PEI las 26 palabras de la emision; Multiply: dst[i] = (fnv(a) ^ (fnv(b) * 31)) + i; GetRWMatrix: 16 palabras; Create reparte las cuatro
matrices del arena en orden. x87 a 24 bits con exponente extendido: rotZ * 0.5f es exacto (potencia de dos, salvo subnormales). NaN solo silenciosos.
"""
import struct
from fractions import Fraction

CASES = 4096
NAME = 'FxEmitter_c::AddParticle(vector)'
NAMES = [NAME]
M32 = 0xFFFFFFFF
K_EMITTER, K_PRIM, K_SYSTEM, K_SYSTEM_BP, K_PARENT, K_MATS, K_POS, K_VEL, K_MULT, K_PART, K_ARENA = 0x000, 0x040, 0x0C0, 0x140, 0x180, 0x1C0, 0x2C0, 0x2D0, 0x2E0, 0x300, 0x340
K_GUARD = 16
# suma de huecos igual a la de Fixtures.hpp (static_assert alli): emisor 0x40, prim 0x80, sistema 0x80, blueprint 0x40, padre 0x40, 4 matrices 0x100, pos 0x10, vel 0x10, mult 0x20, particula 0x40
assert K_PRIM == K_EMITTER + 0x40 and K_SYSTEM == K_PRIM + 0x80 and K_SYSTEM_BP == K_SYSTEM + 0x80 and K_PARENT == K_SYSTEM_BP + 0x40 and K_MATS == K_PARENT + 0x40
assert K_POS == K_MATS + 4 * 0x40 and K_VEL == K_POS + 0x10 and K_MULT == K_VEL + 0x10 and K_PART == K_MULT + 0x20 and K_ARENA == K_PART + 0x40
VTABLE = 0x0085A7A4
EMISSION_WORDS = 26


def MAT(i):
    return K_MATS + 0x40 * i


SPECIAL = [
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x7F800000, 0xFF800000, 0x7FC00000, 0x7FC01234, 0xFFC00001, 0x7FFFFFFF, 0x7F7FFFFF, 0xFF7FFFFF,
    0x3F000000, 0x40000000, 0x41200000, 0x42C80000, 0x3A83126F, 0x437F0000, 0x477FFF00, 0x4B000000, 0x3F7FFFFF, 0x3F800001, 0x3C23D70A, 0x3E800000, 0x437F8000, 0x42C7FFFF, 0x7F000000, 0x00FFFFFF,
    0x3FC90FDB, 0x40490FDB, 0x40C90FDB, 0xC0490FDB, 0x42B40000, 0x43B40000, 0x7149F2CA, 0x0DA24260]
ROTZ = [
    0x00000000, 0x80000000, 0x00000001, 0x80000001, 0xBF800000, 0xC2C80000, 0x3F800000, 0x40000000, 0x40400000, 0x437F0000, 0x43800000, 0x43FF0000, 0x44000000, 0x447FC000, 0x44800000,
    0x4B800000, 0x4F000000, 0x4F7FFFFF, 0x5F000000, 0x5F800000, 0x7F7FFFFF, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00001, 0x3F7FFFFF, 0x437FFFFF, 0x43FFFFFF, 0x3F000000, 0xBF000000, 0xBA83126F,
    0x42C80000, 0x4F800000, 0xCF000000, 0x007FFFFF, 0x7FC01234]


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


def val(c, key, rotz=False):
    fam = c % 8
    dice = rnd(c, key + 1000) % 100
    special, extreme = 4, 3
    if fam == 4:
        special, extreme = 40, 5
    elif fam == 5:
        special, extreme = 5, 40
    elif fam == 6:
        special, extreme = 20, 20
    r = rnd(c, key)
    if rotz and dice < 55:
        return ROTZ[r % len(ROTZ)]
    if not rotz and dice < special:
        return SPECIAL[r % len(SPECIAL)]
    mant = r & 0x7FFFFF
    sign = (r >> 31) & 1
    if dice < special + extreme + (55 if rotz else 0):
        e = 1 + (rnd(c, key + 3000) % 24) if (rnd(c, key + 2000) & 1) else 231 + (rnd(c, key + 3000) % 24)
        return (sign << 31) | (e << 23) | mant
    e = (120 + rnd(c, key + 2000) % 16) if rotz else (118 + rnd(c, key + 2000) % 14)
    return (sign << 31) | (e << 23) | mant


def make(c):
    f = {'family': c % 8}
    f['pos'] = [val(c, 10 + i) for i in range(3)]
    f['vel'] = [val(c, 20 + i) for i in range(3)]
    f['timeSince'] = val(c, 30)
    f['rotZ'] = val(c, 31, True)
    f['brightness'] = val(c, 32)
    f['mult'] = [val(c, 40 + i) for i in range(7)]
    f['length'] = val(c, 50)
    f['useConst'] = rnd(c, 51) & 1
    f['createLocal'] = rnd(c, 52) & 1
    f['parent'] = int(rnd(c, 53) % 3 != 0)
    f['particleMode'] = int((rnd(c, 54) % 4) != 0)
    f['emission'] = [val(c, 100 + i) for i in range(EMISSION_WORDS)]
    f['matInit'] = [[rnd(c, 200 + 16 * m + i) for i in range(16)] for m in range(4)]
    f['parentMat'] = [val(c, 300 + i) for i in range(16)]
    f['getRw'] = [val(c, 320 + i) for i in range(16)]
    f['fill'] = rnd(c, 60) % 4
    return f


# ------------------------------------------------------------------------------------------------------------------------------ x87 minimo (rotZ * 0.5f y _ftol2)
def load(bits):
    bits &= M32
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        return ('nan', bits | 0x400000) if man else ('inf', sign)
    v = Fraction(man, 1 << 149) if exp == 0 else Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127))
    return ('f', -v if sign else v)


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


def ftol_low(v):
    """_ftol2 del EXE: palabra baja del entero de 64 bits truncado hacia cero; 0 si NaN, infinito o fuera de rango."""
    if v[0] in ('nan', 'inf'):
        return 0
    t = int(v[1])
    if not (-(1 << 63) <= t < (1 << 63)):
        return 0
    return t & M32


def rotz_byte(bits):
    kind, x = load(bits)
    if kind in ('nan', 'inf'):
        return 0
    return ftol_low(('f', r24(x * Fraction(1, 2)))) & 0xFF


def rotz_ge_zero(bits):
    """fld rotZ; fcomp 0.0; test ah, 1; jne: salta si rotZ < 0 o NaN. Aqui: lo contrario."""
    kind, x = load(bits)
    if kind == 'nan':
        return False
    if kind == 'inf':
        return x == 0
    return x >= 0                                                      # -0.0 >= 0.0 es verdadero (C3)


def fnv(words):
    h = 2166136261
    for w in words:
        for k in range(4):
            h ^= (w >> (8 * k)) & 0xFF
            h = (h * 16777619) & M32
    return h


def build_arena(base, f):
    fills = [0x5C, 0xC3, 0xA5, 0x00]
    b = bytearray([fills[f['fill']]]) * K_ARENA

    def put(at, v):
        b[at:at + 4] = struct.pack('<I', v & M32)
    put(K_EMITTER + 0, VTABLE)
    put(K_EMITTER + 4, base + K_PRIM)
    put(K_EMITTER + 8, base + K_SYSTEM)
    b[K_EMITTER + 0xC] = 1
    put(K_EMITTER + 0x10, 0x3F000000)
    put(K_SYSTEM + 8, base + K_SYSTEM_BP)
    put(K_SYSTEM + 0xC, (base + K_PARENT) if f['parent'] else 0)
    b[K_SYSTEM + 0x52] = f['useConst'] & 0xFF
    put(K_SYSTEM_BP + 0xC, f['length'])
    for i in range(16):
        put(K_PARENT + 4 * i, f['parentMat'][i])
    for m in range(4):
        for i in range(16):
            put(MAT(m) + 4 * i, f['matInit'][m][i])
    for i in range(3):
        put(K_POS + 4 * i, f['pos'][i])
        put(K_VEL + 4 * i, f['vel'][i])
    for i in range(7):
        put(K_MULT + 4 * i, f['mult'][i])
    return b


def run(f, base):
    """Devuelve (arena: lista de bytes, trace: lista de enteros, stats)."""
    mem = build_arena(base, f)
    trace = []
    stats = {}

    def w(at):
        return struct.unpack_from('<I', mem, at)[0]

    def put(at, v):
        struct.pack_into('<I', mem, at, v & M32)

    def words(at, n):
        return [w(at + 4 * i) for i in range(n)]

    trace.extend([1, K_PRIM + 0x2C, 0, 0, f['length'], f['useConst'] & 0xFF])
    A, B, C = MAT(0), MAT(1), MAT(2)
    for m in (A, B, C):
        trace.extend([2, m])
    flags = w(C + 12)
    for i, v in ((0, 0x3F800000), (1, 0), (2, 0), (4, 0), (5, 0x3F800000), (6, 0), (8, 0), (9, 0), (10, 0x3F800000)):
        put(C + 4 * i, v)
    put(C + 12, flags | 0x20003)
    for i in range(3):
        put(C + 0x30 + 4 * i, f['pos'][i])
    trace.extend([5, C, fnv(words(C, 16))])
    if f['parent']:
        ha, hb = fnv(words(C, 16)), fnv(words(K_PARENT, 16))
        trace.extend([6, B, C, K_PARENT, ha, hb])
        h = (ha ^ ((hb * 31) & M32)) & M32
        for i in range(16):
            put(B + 4 * i, h + i)
        stats['parent'] = 1
    else:
        for i in range(16):
            put(B + 4 * i, w(C + 4 * i))
    D = MAT(3)
    trace.extend([2, D])
    trace.extend([4, K_PRIM, D])
    for i in range(16):
        put(D + 4 * i, f['getRw'][i])
    ha, hb = fnv(words(D, 16)), fnv(words(B, 16))
    trace.extend([6, A, D, B, ha, hb])
    h = (ha ^ ((hb * 31) & M32)) & M32
    for i in range(16):
        put(A + 4 * i, h + i)
    trace.extend([3, D])
    trace.extend([8, K_EMITTER, fnv(f['emission']), A, fnv(words(A, 16)), K_VEL, fnv(words(K_VEL, 3)), f['timeSince'], fnv(f['mult']), f['brightness'], f['createLocal']])
    if f['particleMode']:
        stats['particle'] = 1
        if rotz_ge_zero(f['rotZ']):
            mem[K_PART + 0x36] = rotz_byte(f['rotZ'])
            stats['rot_written'] = 1
            if mem[K_PART + 0x36]:
                stats['rot_nonzero'] = 1
        else:
            stats['rot_skipped'] = 1
    else:
        stats['no_particle'] = 1
    for m in (C, B, A):
        trace.extend([3, m])
    kind, _ = load(f['rotZ'])
    stats['rot_nan'] = int(kind == 'nan')
    stats['rot_neg'] = int(f['rotZ'] >> 31 and kind != 'nan')
    stats['local'] = f['createLocal']
    stats['useconst'] = f['useConst']
    return list(mem), trace, stats


def expected(inp, case, base):
    mem, trace, stats = run(dict(inp), base)
    return {'arena': mem, 'trace': trace}, stats


def matches(observed, want):
    return bytes.fromhex(observed['arena']) == bytes(want['arena']) and list(observed['trace']) == want['trace']


def self_test():
    assert rotz_byte(0x437F0000) == 127 and rotz_byte(0x43800000) == 128 and rotz_byte(0x44000000) == 0 and rotz_byte(0x447FC000) == 255
    assert rotz_byte(0x7F800000) == 0 and rotz_byte(0x4F800000) == 0 and rotz_byte(0x3F7FFFFF) == 0
    assert rotz_byte(0xBF800000) == 0                                      # trunc(-0.5) = 0
    assert rotz_byte(0xC2C80000) == (-50) & 0xFF
    assert rotz_ge_zero(0x00000000) and rotz_ge_zero(0x80000000) and not rotz_ge_zero(0xBF800000) and not rotz_ge_zero(0x7FC00000) and rotz_ge_zero(0x7F800000) and not rotz_ge_zero(0xFF800000)
    assert fnv([0x64636261]) == 0xCE3479BD                              # FNV-1a de 32 bits de 'abcd'
    assert len({make(c)['family'] for c in range(64)}) == 8
    return 'ok'


if __name__ == '__main__':
    print(self_test())
