"""Oraculo independiente de las fixtures del banco Dependencies22C (repite Fixtures.hpp y Arenas.hpp, sin tocar el juego).

- WRAP: valor final de CCam::WrapAngle (aritmetica de float32: cada paso es un solo redondeo de un resultado exacto) y clase de la ultima instruccion x87 esperada.
- CREATE: estado final del arena, traza de colaboradores e intensidad de FxEmitter_c::CreateParticles, con la aritmetica x87 a 24 bits (cada operacion redondeada a 24 bits
  significativos, exponente sin limite, guardado final a float32) sobre fracciones exactas; NaN/infinitos/ceros con signo se tratan con las reglas IEEE (la carga de un NaN no
  se predice: solo se comprueba que lo es).
Uso: python -B fixture_oracle.py   (imprime la cobertura declarada de las fixtures)
"""
import math
import struct
from fractions import Fraction

M32 = 0xFFFFFFFF
WRAP_CASES, CREATE_CASES = 2048, 4096
WRAP_LIMIT = 16777216.0


def u32(x): return x & M32
def mix(x):
    x = u32(x); x ^= x >> 16; x = u32(x * 0x7FEB352D); x ^= x >> 15; x = u32(x * 0x846CA68B); x ^= x >> 16; return x
def rand(c, k): return mix(u32(c * 0x9E3779B1) + u32(k * 0x85EBCA6B) + 12345)
def F(bits): return struct.unpack('<f', struct.pack('<I', u32(bits)))[0]
def B(f): return struct.unpack('<I', struct.pack('<f', f))[0]
def r32(x):
    """Redondeo de un double a float32 (un solo redondeo si x es el resultado exacto de una operacion de dos floats)."""
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


# ------------------------------------------------------------------ tablas (copia de Fixtures.hpp)
K_ANGLE = [
    0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x40400000, 0xC0400000, 0x40490FDA, 0x40490FDB, 0x40490FDC, 0xC0490FDA, 0xC0490FDB, 0xC0490FDC,
    0x40C90FDA, 0x40C90FDB, 0x40C90FDC, 0xC0C90FDA, 0xC0C90FDB, 0xC0C90FDC, 0x4116CBE3, 0x4116CBE4, 0x4116CBE5, 0xC116CBE4, 0x41490FDB, 0xC1490FDB,
    0x42C80000, 0xC2C80000, 0x447A0000, 0xC47A0000, 0x4640E400, 0xC640E400, 0x49742400, 0xC9742400, 0x4A989680, 0xCA989680, 0x4B000000, 0xCB000000,
    0x4B800000, 0xCB800000, 0x4B7FFFFF, 0x4B000001, 0x00000001, 0x007FFFFF, 0x80000001, 0x807FFFFF, 0x00800000, 0x80800000, 0x3FC90FDB, 0xBFC90FDB,
    0x3F000000, 0xBF000000, 0x3D800000, 0x358637BD, 0x7FC00000, 0xFFC00000, 0x7FC01234, 0xFFC00001, 0x7F800001, 0xFF800123, 0x7FFFFFFF, 0xFFFFFFFF,
    0x4546F000, 0xC546F000, 0x41D59FBE, 0xC0E80000, 0x40A00000, 0xC0A00000, 0x412AAAAB, 0xC12AAAAB, 0x4796E7C2, 0xC796E7C2, 0x4B189680, 0xCB189680]
K_DIST = [0x00000000, 0x3F800000, 0x41200000, 0x42C80000, 0x7149F2CA, 0xC0A00000, 0x7FC01234, 0x7F800001, 0x7F800000, 0xFF800000, 0x000003E8, 0x80000000, 0x7F7FFFFF, 0x40600000, 0x40E00000, 0x41FFEB85]
K_LOD = [(1000, 0, 0), (1000, 0, 6400), (65535, 640, 3200), (1, 64, 64), (0, 0, 64), (500, 128, 64), (65535, 0, 65535), (3000, 32, 96),
         (20000, 6400, 12800), (255, 1, 2), (100, 0, 1), (65535, 65535, 65535), (777, 640, 641), (1000, 1000, 2000), (40000, 100, 6400), (8, 3200, 3201)]
K_COUNT = [0x00000000, 0x3F800000, 0x3F000000, 0x41200000, 0x42C80000, 0xBF800000, 0x7FC00001, 0x7F800002, 0x7F800000, 0xFF800000, 0x0DA24260, 0x00000003, 0x7F7FFFFF, 0x7149F2CA, 0x80000000, 0x3CA3D70A]
K_INIT = [0x00000000, 0x3E800000, 0x3F000000, 0x3F7FBE77, 0x3F800000, 0x40000000, 0x40B00000, 0x7FC00002, 0xBF800000, 0x80000000, 0x7F800000, 0x7149F2CA, 0x000116C2, 0x38D1B717, 0x42C80000, 0x7F7FFFFF]
K_WEATHER_VALUE = [0x00000000, 0x3F000000, 0x3F800000, 0x40000000, 0xBF800000, 0x7FC00003, 0x7F800000, 0x000116C2]
K_WEATHER_LIMITS = [
    (0x00000000, 0x40000000, 0x00000000, 0x40000000), (0x00000000, 0x3ECCCCCD, 0x00000000, 0x40000000), (0x3F19999A, 0x40000000, 0x00000000, 0x40000000),
    (0x00000000, 0x40000000, 0x00000000, 0x3ECCCCCD), (0x00000000, 0x40000000, 0x3F19999A, 0x40000000), (0x7FC00000, 0x40000000, 0x00000000, 0x40000000),
    (0x00000000, 0xFFC00001, 0x00000000, 0x40000000), (0x00000000, 0x40000000, 0x7FC01234, 0x40000000), (0x00000000, 0x40000000, 0x00000000, 0x7F800002),
    (0xFF800000, 0x7F800000, 0xFF800000, 0x7F800000), (0x7F800000, 0xFF800000, 0x00000000, 0x40000000), (0x00000000, 0x40000000, 0x7F800000, 0xFF800000),
    (0x000116C2, 0x40000000, 0x000116C2, 0x40000000), (0xBF800000, 0x40400000, 0xBF800000, 0x40400000), (0x40000000, 0x40000000, 0x40000000, 0x40000000),
    (0x00000000, 0x40000000, 0x00000000, 0x40000000)]
K_DELTA = [0x00000000, 0x3CA3D70A, 0x3F000000, 0x3F800000, 0x42053333, 0xBF800000, 0x7FC00004, 0x7149F2CA]
K_BENIGN_COUNT = [0x3F800000, 0x40000000, 0x40400000, 0x40A00000, 0x40F00000, 0x41000000]
K_BENIGN_INIT = [0x00000000, 0x3F000000, 0x3F800000, 0x3FC00000, 0x40000000, 0x00000001]
PI, TWO_PI = F(0x40490FDB), F(0x40C90FDB)


# ------------------------------------------------------------------ WRAP
def wrap_fixture(c):
    f = dict(case=c, slot=rand(c, 9) % 4, fill=rand(c, 10) % 4, edx=u32(rand(c, 11) | 1))
    if c < len(K_ANGLE) * 4:
        f['kind'] = 0; f['angle'] = K_ANGLE[c % len(K_ANGLE)]
    elif c % 4 == 0:
        f['kind'] = 1
        e = 119 + rand(c, 1) % 31; m = rand(c, 2) & 0x7FFFFF; s = rand(c, 3) & 1
        f['angle'] = (s << 31) | (e << 23) | m
    elif c % 4 == 1:
        f['kind'] = 2
        k = int(rand(c, 4) % 81) - 40
        base = r32(float(k) * TWO_PI)
        d = int(rand(c, 5) % 7) - 3
        f['angle'] = u32(B(base) + d)
    elif c % 4 == 2:
        f['kind'] = 3
        sign = (rand(c, 6) & 1) << 31
        center = 0x40490FDB if (rand(c, 7) & 1) else 0x40C90FDB
        d = int(rand(c, 8) % 9) - 4
        f['angle'] = u32(sign | u32(center + d))
    else:
        f['kind'] = 1
        v = r32(float(int(rand(c, 12) % 4001) - 2000) / float(1 + rand(c, 13) % 16))
        f['angle'] = B(v)
    return f


def _wrap_down(v):
    """Equivalente rapido de `while v >= pi: v = r32(v - 2pi)` (el bucle del EXE): por encima de 16 cada paso resta una constante entera de ulps dentro de cada
    decada binaria (el desempate al par solo aparece en [8, 16), que se recorre paso a paso), asi que se salta de golpe hasta cerca del limite inferior de la decada."""
    while v >= PI:
        if v >= 16.0:
            e = math.frexp(v)[1] - 1
            u = 2.0 ** (e - 23)
            q = int(Fraction(13176795, 2 ** (e - 2)) + Fraction(1, 2))   # c / u = 13176795 / 2^(e-2) (sin empate para e >= 4)
            if q >= 1:
                m = int(v / u)
                k = (m - (1 << 23) - q - 2) // q
                if k >= 1:
                    v = (m - k * q) * u
                    continue
        v = r32(v - TWO_PI)
    return v


def _wrap_up(v):
    while v < -PI:
        if v <= -16.0:
            e = math.frexp(-v)[1] - 1
            u = 2.0 ** (e - 23)
            q = int(Fraction(13176795, 2 ** (e - 2)) + Fraction(1, 2))
            if q >= 1:
                m = int(-v / u)
                k = (m - (1 << 23) - q - 2) // q
                if k >= 1:
                    v = -(m - k * q) * u
                    continue
        v = r32(v + TWO_PI)
    return v


def wrap_expected_slow(bits):
    a = F(bits)
    if a != a:
        return bits, 1
    if a >= PI:
        v = a
        while True:
            v = r32(v - TWO_PI)
            if not (v >= PI):
                break
        a = v
    cls = 1
    if a < -PI:
        v = a
        while True:
            v = r32(v + TWO_PI)
            if not (v < -PI):
                break
        a = v; cls = 2
    return B(a), cls


def wrap_expected(bits):
    """(bits finales, clase de la ultima x87: 1 = fcomp m32, 2 = fstp m32)."""
    a = F(bits)
    if a != a:
        return bits, 1
    if a >= PI:
        a = _wrap_down(a)
    cls = 1
    if a < -PI:
        a = _wrap_up(a); cls = 2
    return B(a), cls


def wrap_in_domain(f):
    a = F(f['angle'])
    return a != a or abs(a) <= WRAP_LIMIT


# ------------------------------------------------------------------ CREATE
def create_fixture(c):
    f = dict(case=c)
    f['benign'] = c % 4 == 0
    f['time'] = rand(c, 20) & 0x3FFFFFFF
    f['parent'] = rand(c, 5) % 2; f['create_local'] = rand(c, 6) % 2; f['flags_other'] = rand(c, 21) & 0xFD; f['emission_fill'] = rand(c, 22)
    f['guarded'] = False
    if f['benign']:
        f['dist'] = K_DIST[1 + rand(c, 1) % 3]; f['rate'] = 1000; f['lod_start'] = 640; f['lod_end'] = 6400
        f['count'] = K_BENIGN_COUNT[rand(c, 2) % len(K_BENIGN_COUNT)]; f['init'] = K_BENIGN_INIT[rand(c, 3) % len(K_BENIGN_INIT)]
        f['wind'] = K_WEATHER_VALUE[1 + rand(c, 4) % 3]; f['rain'] = K_WEATHER_VALUE[1 + rand(c, 7) % 3]
        f['min_wind'], f['max_wind'], f['min_rain'], f['max_rain'] = K_WEATHER_LIMITS[0]
        f['delta'] = K_DELTA[1 + rand(c, 8) % 4]
        return f
    f['dist'] = K_DIST[c % 16]
    lod = K_LOD[c // 16 % 16]; f['rate'], f['lod_start'], f['lod_end'] = lod
    f['count'] = K_COUNT[c // 256 % 16]; f['init'] = K_INIT[rand(c, 1) % 16]
    f['wind'] = K_WEATHER_VALUE[rand(c, 3) % 8]; f['rain'] = K_WEATHER_VALUE[rand(c, 4) % 8]
    f['min_wind'], f['max_wind'], f['min_rain'], f['max_rain'] = K_WEATHER_LIMITS[rand(c, 2) % 16]
    f['delta'] = K_DELTA[rand(c, 7) % 8]
    s = predicted_sum(f)
    if s == s and s >= 16.0 and s < 1.0e19:
        f['guarded'] = True; f['wind'] = 0x3F000000; f['min_wind'] = 0x7F800000
    return f


def _ddiv(a, b):
    if b == 0.0:
        if a == 0.0 or a != a: return math.nan
        return math.copysign(math.inf, a) * math.copysign(1.0, b)
    return a / b


def predicted_sum(f):
    rate = float(f['rate']) * F(0x3A83126F)
    lod_start = float(f['lod_start']) * 0.015625; lod_end = float(f['lod_end']) * 0.015625; dist = F(f['dist'])
    if lod_start > dist: vis = 1.0
    elif lod_end < dist: vis = 0.0
    else: vis = 1.0 - _ddiv(dist - lod_start, lod_end - lod_start)
    return float(F(f['count'])) * vis * rate + float(F(f['init']))


# ---- aritmetica x87 a 24 bits: valores = float de Python (cero con signo, inf, nan) o Fraction (finito no nulo)
def _is_special(v): return isinstance(v, float)
def _kind(v):
    if not isinstance(v, float): return 'fin'
    if v != v: return 'nan'
    if math.isinf(v): return 'inf'
    return 'zero'
def _sgn(v): return -1.0 if (math.copysign(1.0, v) < 0 if isinstance(v, float) else v < 0) else 1.0
def _to_float(v): return v if isinstance(v, float) else float(v)


def round_sig(fr, bits=24):
    """Redondeo al par a `bits` cifras significativas (exponente sin limite)."""
    if fr == 0:
        return 0.0
    sign = -1 if fr < 0 else 1
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a: e -= 1
    while Fraction(2) ** (e + 1) <= a: e += 1
    quantum = Fraction(2) ** (e - bits + 1)
    q = a / quantum
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1): n += 1
    return sign * n * quantum


def x_add(a, b):
    ka, kb = _kind(a), _kind(b)
    if ka == 'nan' or kb == 'nan': return math.nan
    if ka == 'inf' or kb == 'inf':
        return _to_float(a) + _to_float(b) if (ka != 'fin' and kb != 'fin') else (a if ka == 'inf' else b)
    if ka == 'zero' and kb == 'zero':
        return -0.0 if (_sgn(a) < 0 and _sgn(b) < 0) else 0.0
    if ka == 'zero': return b
    if kb == 'zero': return a
    r = a + b
    return 0.0 if r == 0 else round_sig(r)
def x_neg(a): return -a
def x_sub(a, b): return x_add(a, x_neg(b))
def x_mul(a, b):
    ka, kb = _kind(a), _kind(b)
    if ka == 'nan' or kb == 'nan': return math.nan
    sign = _sgn(a) * _sgn(b)
    if ka == 'inf' or kb == 'inf':
        return math.nan if (ka == 'zero' or kb == 'zero') else math.copysign(math.inf, sign)
    if ka == 'zero' or kb == 'zero': return math.copysign(0.0, sign)
    return round_sig(a * b)
def x_div(a, b):
    ka, kb = _kind(a), _kind(b)
    if ka == 'nan' or kb == 'nan': return math.nan
    sign = _sgn(a) * _sgn(b)
    if ka == 'inf': return math.nan if kb == 'inf' else math.copysign(math.inf, sign)
    if kb == 'inf': return math.copysign(0.0, sign)
    if kb == 'zero': return math.nan if ka == 'zero' else math.copysign(math.inf, sign)
    if ka == 'zero': return math.copysign(0.0, sign)
    return round_sig(a / b)
def x_load(bits):
    f = F(bits)
    if f != f or math.isinf(f) or f == 0.0: return f
    return Fraction(f)


def store32(v):
    """Guardado a float32 de un valor x87 (bits; los NaN devuelven None: la carga no se predice)."""
    if _is_special(v):
        if v != v: return None
        return B(v)   # inf o cero con signo
    sign = 0x80000000 if v < 0 else 0
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a: e -= 1
    while Fraction(2) ** (e + 1) <= a: e += 1
    quantum = Fraction(2) ** (max(e, -126) - 23)
    q = a / quantum
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1): n += 1
    r = n * quantum
    if r >= Fraction(2) ** 128: return sign | 0x7F800000
    if r == 0: return sign
    return sign | B(float(r)) & 0x7FFFFFFF


def ordered_less(a, b): return a < b   # floats de Python: False con NaN


def create_model(f):
    """Modelo del EXE: devuelve dict(continued, sum_bits, intensity_bits_final, loop, exit_reason, ...)."""
    rate = x_mul(float(f['rate']) if f['rate'] == 0 else Fraction(f['rate']), x_load(0x3A83126F))
    lod_start = x_mul(Fraction(f['lod_start']) if f['lod_start'] else 0.0, x_load(0x3C800000))
    lod_end = x_mul(Fraction(f['lod_end']) if f['lod_end'] else 0.0, x_load(0x3C800000))
    dist = F(f['dist']); fs = float(lod_start); fe = float(lod_end)
    if fs > dist: vis = Fraction(1); path = 'near'
    elif fe < dist: vis = 0.0; path = 'far'
    else:
        num = x_sub(x_load(f['dist']), lod_start); den = x_sub(lod_end, lod_start)
        vis = x_sub(Fraction(1), x_div(num, den)); path = 'mid'
    s = x_add(x_mul(x_mul(x_load(f['count']), vis), rate), x_load(f['init']))
    sum_bits = store32(s)
    sum_f = x_load(sum_bits) if sum_bits is not None else math.nan
    wind, rain = F(f['wind']), F(f['rain'])
    reason = None
    if wind < F(f['min_wind']): reason = 'wind_min'
    elif wind > F(f['max_wind']): reason = 'wind_max'
    elif rain < F(f['min_rain']): reason = 'rain_min'
    elif rain > F(f['max_rain']): reason = 'rain_max'
    elif _to_float(sum_f) < 1.0: reason = 'sum_lt_1'
    out = dict(path=path, sum_bits=sum_bits, sum_is_nan=sum_bits is None, reason=reason, continued=reason is None)
    if reason is not None:
        out['final_bits'] = sum_bits; out['loop'] = 0
        return out
    # _ftol2: palabra baja del entero truncado; NaN, infinitos y |v| >= 2^63 dan el entero indefinido (EAX = 0)
    sf = _to_float(sum_f)
    if sf != sf or math.isinf(sf) or abs(sf) >= 2.0 ** 63: n = 0
    else:
        n = int(sf) & M32
        n = n - (1 << 32) if n >= (1 << 31) else n
    out['loop'] = max(n, 0)
    # timeSince y valor final de la intensidad
    times = []
    delta = x_load(f['delta'])
    for i in range(max(n, 0)):
        t = x_mul(x_div(Fraction(i) if i else 0.0, sum_f), delta)
        times.append(store32(t))
    out['times'] = times
    out['n'] = n
    out['final_bits'] = store32(x_sub(sum_f, Fraction(n) if n else 0.0)) if sum_bits is not None else None
    return out


# ---- arena (copia de Arenas.hpp) y traza de los stubs
K_CP_EMITTER, K_CP_PRIM, K_CP_SYSTEM, K_CP_SYSTEMBP, K_CP_PARENT, K_CP_MATS, K_CP_ARENA = 0x000, 0x040, 0x0C0, 0x140, 0x160, 0x1A0, 0x280
def cp_matrix(i): return K_CP_MATS + 0x40 * i
def build_wrap(f):
    fills = (0x00, 0xFF, 0xA5, 0x5A)
    buf = bytearray([fills[f['fill']]]) * 48
    buf[8 + 4 * f['slot']:12 + 4 * f['slot']] = struct.pack('<I', f['angle'])
    return buf
def build_create(base, f):
    buf = bytearray([0x5C]) * K_CP_ARENA
    def put32(at, v): buf[at:at + 4] = struct.pack('<I', u32(v))
    def put16(at, v): buf[at:at + 2] = struct.pack('<H', v & 0xFFFF)
    put32(K_CP_EMITTER + 0, 0x0085A7A4); put32(K_CP_EMITTER + 4, base + K_CP_PRIM); put32(K_CP_EMITTER + 8, base + K_CP_SYSTEM)
    buf[K_CP_EMITTER + 0xC] = 1; put32(K_CP_EMITTER + 0x10, f['init'])
    put16(K_CP_PRIM + 0x38, f['lod_start']); put16(K_CP_PRIM + 0x3A, f['lod_end'])
    put32(K_CP_SYSTEM + 0, 0); put32(K_CP_SYSTEM + 4, 0)
    put32(K_CP_SYSTEM + 8, base + K_CP_SYSTEMBP); put32(K_CP_SYSTEM + 0xC, base + K_CP_PARENT if f['parent'] else 0)
    for i in range(16): put32(K_CP_SYSTEM + 0x10 + 4 * i, 0x3F000000 + f['emission_fill'] + i)
    buf[K_CP_SYSTEM + 0x52] = f['emission_fill'] & 1
    put32(K_CP_SYSTEM + 0x58, f['dist']); put16(K_CP_SYSTEM + 0x5E, f['rate'])
    buf[K_CP_SYSTEM + 0x62] = (f['flags_other'] & 0xFD) | (2 if f['create_local'] else 0)
    put32(K_CP_SYSTEMBP + 0xC, 0x41200000)
    for i in range(16): put32(K_CP_PARENT + 4 * i, 0x3E000000 + f['time'] + 3 * i)
    for m in range(3): buf[cp_matrix(m):cp_matrix(m) + 0x40] = bytes([0x77]) * 0x40
    return buf
def emission_words(f):
    w = [mix(u32(f['emission_fill'] + i * 77)) for i in range(26)]
    w[0] = f['count']; w[22] = f['min_wind']; w[23] = f['max_wind']; w[24] = f['min_rain']; w[25] = f['max_rain']
    return w
def fnv(data):
    h = 2166136261
    for b in data: h = ((h ^ b) * 16777619) & M32
    return h


def oracle_create(base, f):
    """(arena esperado, traza esperada, modelo) para un caso."""
    model = create_model(f)
    buf = build_create(base, f)
    em = emission_words(f)
    em_bytes = struct.pack('<26I', *em)
    trace = [1, K_CP_PRIM + 0x2C, f['time'], f['delta'], 0x41200000, f['emission_fill'] & 1]
    if model['final_bits'] is not None or model['sum_is_nan']:
        pass
    def intensity_bits(bits):
        buf[K_CP_EMITTER + 0x10:K_CP_EMITTER + 0x14] = struct.pack('<I', bits)
    if model['sum_bits'] is not None: intensity_bits(model['sum_bits'])
    if not model['continued']:
        return buf, trace, model
    m = [cp_matrix(0), cp_matrix(1), cp_matrix(2)]
    mat = lambda i: bytes(buf[m[i]:m[i] + 0x40])
    local = bytes(buf[K_CP_SYSTEM + 0x10:K_CP_SYSTEM + 0x50])
    trace += [2, m[0], 2, m[1], 5, K_CP_SYSTEM + 0x10, fnv(local)]
    if f['parent']:
        parent = bytes(buf[K_CP_PARENT:K_CP_PARENT + 0x40])
        h = fnv(local) ^ ((fnv(parent) * 31) & M32); h &= M32
        trace += [6, m[1], K_CP_SYSTEM + 0x10, K_CP_PARENT, fnv(local), fnv(parent)]
        buf[m[1]:m[1] + 0x40] = struct.pack('<16I', *[u32(h + i) for i in range(16)])
    else:
        buf[m[1]:m[1] + 0x40] = local
    trace += [2, m[2], 4, K_CP_PRIM, m[2]]
    buf[m[2]:m[2] + 0x40] = struct.pack('<16I', *[u32(0x600DF000 + i * 7 + 3) for i in range(16)])
    h = fnv(mat(2)) ^ ((fnv(mat(1)) * 31) & M32); h &= M32
    trace += [6, m[0], m[2], m[1], fnv(mat(2)), fnv(mat(1))]
    buf[m[0]:m[0] + 0x40] = struct.pack('<16I', *[u32(h + i) for i in range(16)])
    trace += [3, m[2]]
    prt = fnv(struct.pack('<7I', *([0x3F800000] * 7)))
    for i in range(model['loop']):
        trace += [8, K_CP_EMITTER, fnv(em_bytes), m[0], fnv(mat(0)), 0, model['times'][i], prt, 0x3F99999A, 1 if f['create_local'] else 0]
    trace += [3, m[1], 3, m[0]]
    if model['final_bits'] is not None: intensity_bits(model['final_bits'])
    return buf, trace, model


def guarded(arena): return (bytes([0xA5]) * 16 + bytes(arena) + bytes([0xA5]) * 16).hex()


def coverage(kind):
    out = {}
    def bump(k): out[k] = out.get(k, 0) + 1
    if kind == 'wrap':
        for c in range(WRAP_CASES):
            f = wrap_fixture(c)
            assert wrap_in_domain(f), (c, hex(f['angle']))
            a = F(f['angle'])
            bump('nan' if a != a else 'in_range' if -PI <= a < PI else 'above' if a >= PI else 'below')
            if a == a and abs(a) >= 1.0e6: bump('big')
            if a == a and 0 < abs(a) < 1.17549435e-38: bump('subnormal')
            if wrap_expected(f['angle'])[1] == 2: bump('class_fstp')
    else:
        for c in range(CREATE_CASES):
            f = create_fixture(c)
            m = create_model(f)
            bump('path_' + m['path'])
            bump('continued' if m['continued'] else 'exit_' + m['reason'])
            if m['continued'] and m['loop']: bump('loop_%d' % min(m['loop'], 15))
            if m['continued']: bump('parent' if f['parent'] else 'no_parent'); bump('local' if f['create_local'] else 'global')
            if f['guarded']: bump('guarded')
            if m['sum_is_nan']: bump('sum_nan')
            if F(f['dist']) != F(f['dist']): bump('dist_nan')
    return out


if __name__ == '__main__':
    import json
    print(json.dumps({'wrap': coverage('wrap'), 'create': coverage('create')}, indent=1, sort_keys=True))
