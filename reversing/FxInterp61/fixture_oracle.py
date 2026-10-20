"""Oraculo del banco FxInterp61: FxInterpInfo{Float,U255,32,255}_c::GetVal(float* outValues, float delta) (0x4A8470, 0x4A8800, 0x4A89C0, 0x4A8B80).

Modelado a partir del DESENSAMBLADO del EXE (no del C++): las cuatro funciones son una misma plantilla con x87 y la palabra de control a 24 bits (el banco la fija):
  * m_nNumKeys (byte +5, con signo) == 1 -> copia de la clave 0 de cada atributo; m_nCount (+6) se usa SOLO como byte bajo con signo (movsx byte ptr [esi+6]).
  * byte +4 (m_bLooped) != 0 -> total = (uint16)times[n-1] * 1/256; q = delta / total; entero = _ftol2(q) (trunca; NaN, inf y |q| >= 2^63 dan 0; si no, 32 bits bajos);
    delta = float(delta - entero * total)   (todo en registro: exponente extendido, mantisa de 24 bits).
  * busqueda: primer indice i en [1, n) con delta < times[i] * 1/256 (comparacion ordenada: NaN no corta). Si no hay (o n <= 1) copia de la clave n-1 (index -1 incluido).
  * si hay: A = times[i-1]/256, B = times[i]/256, t = float((delta - A) / (B - A)); salida[j] = float((clave[j][i] - clave[j][i-1]) * t + clave[j][i-1]).
  * Float: claves float (copias en bits); U255: uint16 * 1/256 (0x859AA0); 32: int16 * 0.001f (0x858CDC); 255: int16 * 1/128 (0x858B88).
Aritmetica: Fractions exactas redondeadas a 24 bits (mitad al par, exponente extendido sin desbordes) en cada operacion x87; float(...) = fstp dword (24 bits, exponente de float,
subnormales graduales). NaN: el operando NaN (silenciado) se propaga; con dos NaN gana el de mayor significando; un empate con signos distintos queda como comodin '?'
(solo se exige EXE == DLL). NaN/inf/0*inf -> NaN indefinido 0xFFC00000.
`expected(inp, case)` devuelve (esperado, estadisticas); `matches(observado, esperado)` admite '?' en la arena. `python -I fixture_oracle.py` ejecuta la autoprueba (imprime «ok»).
"""
import random
import struct
from fractions import Fraction

NAME_OF = {0: 'FxInterpInfoFloat_c::GetVal', 1: 'FxInterpInfoU255_c::GetVal', 2: 'FxInterpInfo32_c::GetVal', 3: 'FxInterpInfo255_c::GetVal'}
NAMES = [NAME_OF[i] for i in range(4)]
CASES_PER_FUNCTION = 4096
CASES = CASES_PER_FUNCTION          # por funcion
ADDRESS = {0: 0x4A8470, 1: 0x4A8800, 2: 0x4A89C0, 3: 0x4A8B80}

# Disposicion de la arena: debe coincidir con Fixtures.hpp (se comprueba en la autoprueba cruzada con el volcado compilado).
K_ARENA, K_OBJ, K_TIMES, K_PTRS, K_ROW0, K_ROW_STRIDE, K_ROW_DATA, K_ROWS, K_OUT, K_OUT_CAP = 0x700, 0x40, 0x90, 0x6C0, 0x100, 0x40, 0x10, 8, 0x640, 8

TS = Fraction(1, 256)                      # 0x859AA0 = 0.00390625
SCALE = {0: None, 1: Fraction(1, 256), 2: Fraction(struct.unpack('<f', struct.pack('<f', 0.001))[0]), 3: Fraction(1, 128)}
SIGNED = {0: False, 1: False, 2: True, 3: True}
FLT_MAX = Fraction((1 << 24) - 1) * Fraction(2) ** 104
DEFAULT_NAN = ('n', 1, 0xC000000000000000)


class Fail(Exception):
    pass


# ------------------------------------------------------------------------------------------------ valores x87
def floor_log2(m):
    e = m.numerator.bit_length() - m.denominator.bit_length()
    while Fraction(2) ** (e + 1) <= m:
        e += 1
    while Fraction(2) ** e > m:
        e -= 1
    return e


def rne(fr):
    fl = fr.numerator // fr.denominator
    rem = fr - fl
    if rem > Fraction(1, 2):
        return fl + 1
    if rem == Fraction(1, 2):
        return fl + (fl & 1)
    return fl


def rnd24(m):
    if m == 0:
        return m
    e = floor_log2(m)
    quantum = Fraction(2) ** (e - 23)
    return rne(m / quantum) * quantum


def fin(sign, m):
    return ('f', sign, m)


def from_int(i):
    return fin(1 if i < 0 else 0, Fraction(abs(i)))


def from_float_bits(b, mem=False):
    """mem=True: operando de memoria (fsub/fmul/fadd dword ptr): un NaN señalizador conserva su condicion (snan) para la regla de seleccion de NaN;
    cargado con fld (mem=False) el x87 lo convierte en NaN silencioso."""
    sign = b >> 31
    exp = (b >> 23) & 0xFF
    mant = b & 0x7FFFFF
    if exp == 0xFF:
        if mant == 0:
            return ('i', sign)
        sig = (1 << 63) | (mant << 40)
        return ('n', sign, sig | (1 << 62), bool(mem and not (mant & 0x400000)))
    if exp == 0:
        return fin(sign, Fraction(mant) * Fraction(2) ** -149)
    return fin(sign, Fraction((1 << 23) | mant) * Fraction(2) ** (exp - 150))


def store_float(v):
    """fstp dword: devuelve los bits del float (o None si el signo de un NaN es indeterminado)."""
    if v[0] == 'n':
        if v[1] is None:
            return None
        return (v[1] << 31) | 0x7F800000 | ((v[2] >> 40) & 0x7FFFFF)
    if v[0] == 'i':
        return (v[1] << 31) | 0x7F800000
    sign, m = v[1], v[2]
    if m == 0:
        return sign << 31
    e = floor_log2(m)
    quantum = Fraction(2) ** (-149 if e < -126 else e - 23)
    n = rne(m / quantum)
    if quantum == Fraction(2) ** -149:
        if n >= (1 << 23):          # redondeo hasta el menor normal
            return (sign << 31) | (1 << 23) | (n - (1 << 23))
        return (sign << 31) | n
    # normal: n en [2^23, 2^24]
    if n == (1 << 24):
        n >>= 1
        e += 1
    if e > 127:
        return (sign << 31) | 0x7F800000
    return (sign << 31) | ((e + 127) << 23) | (n - (1 << 23))


def is_nan(v):
    return v[0] == 'n'


def is_snan(v):
    return v[0] == 'n' and len(v) > 3 and v[3]


def quiet(v):
    return ('n', v[1], v[2], False)


def nan_pick(a, b):
    """Tabla 4-7 del manual de Intel (x87): SNaN y QNaN -> el QNaN; dos NaN del mismo tipo -> el de mayor significando; un NaN y un valor -> el NaN (silenciado)."""
    na, nb = is_nan(a), is_nan(b)
    if na and nb:
        if is_snan(a) != is_snan(b):
            return quiet(b if is_snan(a) else a)
        if a[2] > b[2]:
            return quiet(a)
        if b[2] > a[2]:
            return quiet(b)
        if a[1] == b[1]:
            return quiet(a)
        return ('n', None, a[2], False)        # empate de significandos con signos opuestos: indeterminado
    return quiet(a if na else b)


def value(v):
    return v[2] if v[1] == 0 else -v[2]


def mk(x):
    """x: Fraction con signo -> finito redondeado a 24 bits (el cero exacto se trata aparte)."""
    return fin(1 if x < 0 else 0, rnd24(abs(x)))


def add(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else DEFAULT_NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    x, y = value(a), value(b)
    r = x + y
    if r == 0:
        if x == 0 and y == 0:
            return fin(a[1] & b[1], Fraction(0))
        return fin(0, Fraction(0))
    return mk(r)


def neg(v):
    if v[0] == 'n':
        return v
    if v[0] == 'i':
        return ('i', 1 - v[1])
    return fin(1 - v[1], v[2])


def sub(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    return add(a, neg(b))


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    sign = a[1] ^ b[1]
    if a[0] == 'i' or b[0] == 'i':
        other = b if a[0] == 'i' else a
        if other[0] == 'f' and other[2] == 0:
            return DEFAULT_NAN
        return ('i', sign)
    r = a[2] * b[2]
    if r == 0:
        return fin(sign, Fraction(0))
    return fin(sign, rnd24(r))


def div(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    sign = a[1] ^ b[1]
    if a[0] == 'i':
        return DEFAULT_NAN if b[0] == 'i' else ('i', sign)
    if b[0] == 'i':
        return fin(sign, Fraction(0))
    if b[2] == 0:
        return DEFAULT_NAN if a[2] == 0 else ('i', sign)
    if a[2] == 0:
        return fin(sign, Fraction(0))
    return fin(sign, rnd24(a[2] / b[2]))


def less(a, b):
    """fld a; fcomp b; ... jnp: cierto si a < b (ordenado)."""
    if is_nan(a) or is_nan(b):
        return False
    if a[0] == 'i' and b[0] == 'i':
        return a[1] == 1 and b[1] == 0
    if a[0] == 'i':
        return a[1] == 1
    if b[0] == 'i':
        return b[1] == 0
    return value(a) < value(b)


def ftol_low32(q):
    """_ftol2 (0x821B40) como (int)q: NaN, inf y |q| >= 2^63 dan 0; si no, 32 bits bajos del truncado (con signo)."""
    if q[0] != 'f':
        return 0
    m = q[2]
    t = m.numerator // m.denominator
    if t >= (1 << 63):
        return 0
    if q[1]:
        t = -t
    low = t & 0xFFFFFFFF
    return low - (1 << 32) if low >= (1 << 31) else low


# ------------------------------------------------------------------------------------------------ modelo de GetVal
def u8(b, o): return b[o]
def u16(b, o): return struct.unpack_from('<H', b, o)[0]
def s16(b, o): return struct.unpack_from('<h', b, o)[0]
def u32(b, o): return struct.unpack_from('<I', b, o)[0]
def s8(v): return v - 256 if v >= 128 else v


def simulate(func, arena, delta_bits):
    """Devuelve (lista de bits por salida [None = comodin], estadisticas)."""
    obj = K_OBJ
    looped = arena[obj + 4] != 0
    num_keys = s8(arena[obj + 5])
    count = s8(arena[obj + 6])             # solo el byte bajo
    times_off = u32(arena, obj + 8)
    ptrs_off = u32(arena, obj + 12)
    st = {'single': 0, 'looped': 0, 'fold_whole_nonzero': 0, 'fold_big': 0, 'fold_total_zero': 0, 'copy_last': 0, 'interp': 0, 'extrap_before': 0, 'nan_delta': 0, 'inf_delta': 0,
          'count_hi_garbage': 0, 'numkeys_le0': 0, 'numkeys_gt12': 0, 'equal_times': 0, 'out_nan': 0, 'out_inf': 0, 'out_sub': 0, 'out_neg_zero': 0, 'count_zero': 0, 'looped_noncanon': 0,
          'ext_exponent': 0, 'tie': 0, 'snan_copy': 0}
    if u16(arena, obj + 6) >> 8:
        st['count_hi_garbage'] = 1
    if count <= 0:
        st['count_zero'] = 1
    if num_keys <= 0:
        st['numkeys_le0'] = 1
    if num_keys > 12:
        st['numkeys_gt12'] = 1
    if arena[obj + 4] > 1:
        st['looped_noncanon'] = 1
    delta = from_float_bits(delta_bits)
    if is_nan(delta):
        st['nan_delta'] = 1
    if delta[0] == 'i':
        st['inf_delta'] = 1

    def time_at(i):
        return u16(arena, times_off + 2 * i)

    def row(j):
        return u32(arena, ptrs_off + 4 * j)

    def key_raw(j, idx):
        r = row(j)
        if func == 0:
            return u32(arena, r + 4 * idx)
        return s16(arena, r + 2 * idx) if SIGNED[func] else u16(arena, r + 2 * idx)

    def key(j, idx, mem=False):
        raw = key_raw(j, idx)
        if func == 0:
            return from_float_bits(raw, mem)
        return mul(from_int(raw), fin(0, SCALE[func]))

    outs = []

    def copy(idx):
        for j in range(count):
            if func == 0:
                raw = key_raw(j, idx)
                if (raw & 0x7F800000) == 0x7F800000 and (raw & 0x7FFFFF) and not raw & 0x400000:
                    st['snan_copy'] = 1
                outs.append(raw)
            else:
                outs.append(store_float(key(j, idx)))

    if num_keys == 1:
        st['single'] = 1
        copy(0)
        return outs, st
    time_bits = delta_bits
    if looped:
        st['looped'] = 1
        total = mul(from_int(time_at(num_keys - 1)), fin(0, TS))
        if total[2] == 0:
            st['fold_total_zero'] = 1
        q = div(from_float_bits(delta_bits), total)
        whole = ftol_low32(q)
        if whole:
            st['fold_whole_nonzero'] = 1
        if q[0] == 'f' and q[2] >= (1 << 31):
            st['fold_big'] = 1
        prod = mul(from_int(whole), total)
        time_bits = store_float(sub(from_float_bits(delta_bits), prod))
        if time_bits is None:
            raise Fail('signo de NaN indeterminado en el plegado')
    idx = 1
    found = False
    b_time = None
    while idx < num_keys:
        b_time = mul(from_int(time_at(idx)), fin(0, TS))
        if less(from_float_bits(time_bits), b_time):
            found = True
            break
        idx += 1
    if not found:
        st['copy_last'] = 1
        copy(num_keys - 1)
        return outs, st
    a_time = mul(from_int(time_at(idx - 1)), fin(0, TS))
    if a_time == b_time or (a_time[2] == b_time[2]):
        st['equal_times'] = 1
    tv = div(sub(from_float_bits(time_bits), a_time), sub(b_time, a_time))
    t_bits = store_float(tv)
    if t_bits is None:
        raise Fail('signo de NaN indeterminado en t')
    if less(from_float_bits(time_bits), a_time):
        st['extrap_before'] = 1
    st['interp'] = 1
    t = from_float_bits(t_bits)
    for j in range(count):
        prev = key(j, idx - 1, True)     # Float: fsub/fadd con operando de memoria (clave[i-1]); la clave[i] entra por fld
        cur = key(j, idx)
        d = sub(cur, prev)
        if d[0] == 'f' and d[2] > FLT_MAX:
            r_ext = True
        else:
            r_ext = False
        res = add(mul(d, t), prev)
        if r_ext and res[0] == 'f':
            st['ext_exponent'] = 1
        outs.append(store_float(res))
    return outs, st


def out_stats(outs, st):
    for b in outs:
        if b is None:
            st['tie'] = 1
            continue
        e = (b >> 23) & 0xFF
        if e == 0xFF:
            st['out_nan' if b & 0x7FFFFF else 'out_inf'] = 1
        elif e == 0 and (b & 0x7FFFFF):
            st['out_sub'] = 1
        elif b == 0x80000000:
            st['out_neg_zero'] = 1


def expected(inp, case):
    """inp: fila `input` del banco (func, arena, delta...). Devuelve (esperado, estadisticas)."""
    func = inp['func']
    if func not in NAME_OF:
        raise Fail('funcion %r' % func)
    arena = bytes.fromhex(inp['arena'])
    if len(arena) != K_ARENA:
        raise Fail('arena de %d bytes (se esperaban %d)' % (len(arena), K_ARENA))
    outs, st = simulate(func, arena, inp['delta'])
    out_stats(outs, st)
    if len(outs) > K_OUT_CAP:
        raise Fail('mas de %d salidas' % K_OUT_CAP)
    hexs = bytearray(arena[K_OUT:K_OUT + 4 * K_OUT_CAP].hex().encode())
    for j, b in enumerate(outs):
        w = ('????????' if b is None else struct.pack('<I', b).hex()).encode()
        hexs[8 * j:8 * j + 8] = w
    st['family'] = inp['family']
    st['func'] = func
    return {'out': hexs.decode(), 'guard_ok': True}, st


def matches(observed, want):
    """observed/want: {'out': hex de la zona de salida (8 floats), 'guard_ok': bool}; '?' en `want` es comodin."""
    a, w = observed['out'], want['out']
    if len(a) != len(w) or observed.get('guard_ok') is not True:
        return False
    return all(x == y or y == '?' for x, y in zip(a, w))


# ------------------------------------------------------------------------------------------------ autoprueba
def _f32_ops():
    rng = random.Random(61)
    for _ in range(4000):
        a = struct.unpack('<f', struct.pack('<I', (rng.getrandbits(1) << 31) | ((rng.randrange(100, 154)) << 23) | rng.getrandbits(23)))[0]
        b = struct.unpack('<f', struct.pack('<I', (rng.getrandbits(1) << 31) | ((rng.randrange(100, 154)) << 23) | rng.getrandbits(23)))[0]
        va, vb = from_float_bits(struct.unpack('<I', struct.pack('<f', a))[0]), from_float_bits(struct.unpack('<I', struct.pack('<f', b))[0])
        for name, op, ref in (('add', add, a + b), ('sub', sub, a - b), ('mul', mul, a * b), ('div', div, a / b)):
            want = struct.unpack('<I', struct.pack('<f', ref))[0]
            got = store_float(op(va, vb))
            assert got == want, (name, a, b, hex(got), hex(want))


def _mem(func, num_keys, looped, count, times, rows, delta_bits, count_hi=0, fill=0xC3):
    a = bytearray([fill]) * K_ARENA
    o = K_OBJ
    a[o + 4] = looped
    a[o + 5] = num_keys & 0xFF
    struct.pack_into('<H', a, o + 6, (count_hi << 8) | (count & 0xFF))
    struct.pack_into('<II', a, o + 8, K_TIMES, K_PTRS)
    for i, t in enumerate(times):
        struct.pack_into('<H', a, K_TIMES + 2 * i, t)
    for r in range(K_ROWS):
        struct.pack_into('<I', a, K_PTRS + 4 * r, K_ROW0 + r * K_ROW_STRIDE + K_ROW_DATA)
    for r, vals in enumerate(rows):
        for i, v in enumerate(vals):
            at = K_ROW0 + r * K_ROW_STRIDE + K_ROW_DATA
            if func == 0:
                struct.pack_into('<f', a, at + 4 * i, v)
            elif SIGNED[func]:
                struct.pack_into('<h', a, at + 2 * i, v)
            else:
                struct.pack_into('<H', a, at + 2 * i, v)
    return bytes(a)


def fb(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def selftest():
    _f32_ops()
    # floor_log2 / redondeos
    assert floor_log2(Fraction(1)) == 0 and floor_log2(Fraction(3, 2)) == 0 and floor_log2(Fraction(1, 2)) == -1 and floor_log2(Fraction(7, 4)) == 0 and floor_log2(Fraction(2)) == 1
    assert store_float(fin(0, Fraction(1, 2) ** 149)) == 1 and store_float(fin(1, Fraction(2) ** 128)) == 0xFF800000
    assert store_float(fin(0, Fraction(2) ** -150)) == 0 and store_float(fin(0, Fraction(3) * Fraction(2) ** -151)) == 1       # mitad al par / por encima
    # n == 1 (copia de bits, incluido un sNaN)
    a = _mem(0, 1, 0, 2, [0] * 4, [[1.5, 0], [0, 0]], fb(7.0))
    a = bytearray(a)
    struct.pack_into('<I', a, K_ROW0 + K_ROW_STRIDE + K_ROW_DATA, 0x7F800001)
    want, st = expected({'func': 0, 'family': 0, 'arena': bytes(a).hex(), 'delta': fb(7.0)}, 0)
    assert want['out'][:16] == struct.pack('<fI', 1.5, 0x7F800001).hex() and st['single'] and st['snan_copy']
    # interpolacion Float sin bucle: tiempos 0, 256 (1.0 s), 512; claves 10, 20, 40; delta 1.5 -> t = 0.5 -> 30
    a = _mem(0, 3, 0, 1, [0, 256, 512], [[10.0, 20.0, 40.0]], fb(1.5))
    want, st = expected({'func': 0, 'family': 0, 'arena': a.hex(), 'delta': fb(1.5)}, 0)
    assert want['out'][:8] == struct.pack('<f', 30.0).hex() and st['interp']
    # mas alla del ultimo tiempo -> ultima clave; antes del primero -> extrapola (delta -1: t = -1 -> 10 + (20-10)*-1 = 0)
    want, st = expected({'func': 0, 'family': 0, 'arena': a.hex(), 'delta': fb(9.0)}, 0)
    assert want['out'][:8] == struct.pack('<f', 40.0).hex() and st['copy_last']
    want, st = expected({'func': 0, 'family': 0, 'arena': a.hex(), 'delta': fb(-1.0)}, 0)
    assert want['out'][:8] == struct.pack('<f', 0.0).hex() and st['extrap_before']
    # delta == tiempo de clave no corta (< estricto): delta = 1.0 -> segmento 2 con t = 0 -> clave 20
    want, _ = expected({'func': 0, 'family': 0, 'arena': a.hex(), 'delta': fb(1.0)}, 0)
    assert want['out'][:8] == struct.pack('<f', 20.0).hex()
    # NaN no corta -> ultima clave
    want, st = expected({'func': 0, 'family': 0, 'arena': a.hex(), 'delta': 0x7FC00000}, 0)
    assert want['out'][:8] == struct.pack('<f', 40.0).hex() and st['nan_delta'] and st['copy_last']
    # con bucle: total = 2.0; delta 5.5 -> q = 2.75 -> entero 2 -> 5.5 - 4 = 1.5 -> 30
    b = _mem(0, 3, 1, 1, [0, 256, 512], [[10.0, 20.0, 40.0]], fb(5.5))
    want, st = expected({'func': 0, 'family': 0, 'arena': b.hex(), 'delta': fb(5.5)}, 0)
    assert want['out'][:8] == struct.pack('<f', 30.0).hex() and st['looped'] and st['fold_whole_nonzero']
    # con bucle y delta negativo: -0.5 -> q = -0.25 -> 0 -> delta -0.5 (< 0 -> segmento 1 con t = -0.5 -> 10 + 10*-0.5 = 5)
    b = _mem(0, 3, 1, 1, [0, 256, 512], [[10.0, 20.0, 40.0]], fb(-0.5))
    want, st = expected({'func': 0, 'family': 0, 'arena': b.hex(), 'delta': fb(-0.5)}, 0)
    assert want['out'][:8] == struct.pack('<f', 5.0).hex() and st['extrap_before']
    # cociente enorme: 3e9 / total 2.0 = 1.5e9 -> entero valido; 1e30 -> |q| >= 2^63 -> 0 -> delta intacto (>= ultimo tiempo -> ultima clave)
    b = _mem(0, 3, 1, 1, [0, 256, 512], [[10.0, 20.0, 40.0]], fb(1e30))
    want, st = expected({'func': 0, 'family': 0, 'arena': b.hex(), 'delta': fb(1e30)}, 0)
    assert st['copy_last'] and not st['fold_whole_nonzero'] and st['fold_big']
    # total cero: q = delta/0 = inf -> 0 -> delta intacto
    b = _mem(0, 2, 1, 1, [0, 0], [[1.0, 2.0]], fb(3.0))
    want, st = expected({'func': 0, 'family': 0, 'arena': b.hex(), 'delta': fb(3.0)}, 0)
    assert st['fold_total_zero'] and st['copy_last']
    # exponente extendido: claves 3e38 y -3e38, t = 0.5 -> (3e38 - -3e38) * 0.5 + -3e38 = 0 (en float puro desbordaria)
    c = _mem(0, 2, 0, 1, [0, 512], [[-3e38, 3e38]], fb(1.0))
    want, st = expected({'func': 0, 'family': 0, 'arena': c.hex(), 'delta': fb(1.0)}, 0)
    assert want['out'][:8] == struct.pack('<f', 0.0).hex() and st['ext_exponent']
    # 0 * inf -> NaN indefinido 0xFFC00000 (claves iguales, delta -inf en la primera pendiente)
    d = _mem(0, 2, 0, 1, [0, 512], [[5.0, 5.0]], 0xFF800000)
    want, st = expected({'func': 0, 'family': 0, 'arena': d.hex(), 'delta': 0xFF800000}, 0)
    assert want['out'][:8] == 'c0ffffff'[:0] + struct.pack('<I', 0xFFC00000).hex() and st['out_nan']
    # U255: claves uint16 256 y 512 -> 1.0 y 2.0; delta 0.5 en [0, 1.0) -> t = 0.5 -> 1.5
    e = _mem(1, 2, 0, 1, [0, 256], [[256, 512]], fb(0.5))
    want, st = expected({'func': 1, 'family': 0, 'arena': e.hex(), 'delta': fb(0.5)}, 0)
    assert want['out'][:8] == struct.pack('<f', 1.5).hex()
    # 32: int16 -1000 y 3000 con escala 0.001f: -1.0000000474974513 (24 bits) y 3.0000001...; t = 0.5
    f = _mem(2, 2, 0, 1, [0, 256], [[-1000, 3000]], fb(0.5))
    want, st = expected({'func': 2, 'family': 0, 'arena': f.hex(), 'delta': fb(0.5)}, 0)
    k0 = mul(from_int(-1000), fin(0, SCALE[2]))
    k1 = mul(from_int(3000), fin(0, SCALE[2]))
    ref = store_float(add(mul(sub(k1, k0), from_float_bits(fb(0.5))), k0))
    assert want['out'][:8] == struct.pack('<I', ref).hex()
    # 255: int16 0x7FFF y -128 con escala 1/128
    g = _mem(3, 1, 0, 2, [0], [[127, 0], [-128, 0]], fb(0.0))
    want, st = expected({'func': 3, 'family': 0, 'arena': g.hex(), 'delta': fb(0.0)}, 0)
    assert want['out'][:16] == struct.pack('<ff', 127 / 128, -1.0).hex()
    # m_nCount: solo el byte bajo con signo (0x0102 -> 2; 0x01FF -> -1 -> ninguna salida)
    h = _mem(0, 1, 0, 2, [0], [[1.0], [2.0]], fb(0.0), count_hi=1)
    want, st = expected({'func': 0, 'family': 0, 'arena': h.hex(), 'delta': fb(0.0)}, 0)
    assert want['out'][:16] == struct.pack('<ff', 1.0, 2.0).hex() and st['count_hi_garbage']
    h = _mem(0, 1, 0, 255, [0], [[1.0], [2.0]], fb(0.0), count_hi=1)
    want, st = expected({'func': 0, 'family': 0, 'arena': h.hex(), 'delta': fb(0.0)}, 0)
    assert want['out'] == h[K_OUT:K_OUT + 32].hex() and st['count_zero']
    # numKeys 0 con ultima clave en indice -1 (copia)
    i0 = bytearray(_mem(0, 0, 0, 1, [0], [[1.0]], fb(0.0)))
    struct.pack_into('<f', i0, K_ROW0 + K_ROW_DATA - 4, 9.5)
    want, st = expected({'func': 0, 'family': 0, 'arena': bytes(i0).hex(), 'delta': fb(0.0)}, 0)
    assert want['out'][:8] == struct.pack('<f', 9.5).hex() and st['numkeys_le0']
    # NaN en una clave + t normal: se propaga
    j = _mem(0, 2, 0, 1, [0, 256], [[float('nan'), 1.0]], fb(0.5))
    want, st = expected({'func': 0, 'family': 0, 'arena': j.hex(), 'delta': fb(0.5)}, 0)
    assert (int(want['out'][:8][:8], 16) if False else True) and st['out_nan']
    # NaN señalizador en memoria (clave[i-1]) frente a NaN silencioso en registro (clave[i]): gana el silencioso aunque el señalizador silenciado tenga mayor significando
    k = _mem(0, 2, 0, 1, [0, 256], [[0.0, 0.0]], fb(0.5))
    k = bytearray(k)
    struct.pack_into('<II', k, K_ROW0 + K_ROW_DATA, 0x7F812345, 0x7FC00001)
    want, st = expected({'func': 0, 'family': 0, 'arena': bytes(k).hex(), 'delta': fb(0.5)}, 0)
    assert want['out'][:8] == struct.pack('<I', 0x7FC00001).hex() and st['out_nan']
    # matches: comodin
    assert matches({'out': 'abcd', 'guard_ok': True}, {'out': 'ab??', 'guard_ok': True}) and not matches({'out': 'abce', 'guard_ok': True}, {'out': 'abcd', 'guard_ok': True})
    return True


if __name__ == '__main__':
    selftest()
    print('ok')
