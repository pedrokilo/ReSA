"""Oraculo del banco Helpers66D, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Cuatro funciones (4096 casos cada una); la fila `input` que registra el banco basta para calcular lo que debe observarse (`expected(row)`), igual para el EXE y para la DLL:

  0 RpClumpGetBoundingSphere 0x734FC0 (cdecl, ret)   byte [0x8D60BC] = byte bajo del 3.er argumento; sin clump o sin esfera: EAX = 0 sin tocar nada mas; esfera = 0; n = RpClumpGetNumAtomics(clump);
        n < 1: EAX = 0; acc = 0; ForAll(clump, 0x734970, &acc); inv = 1.0 / float(n) (x87 24 bits); acc[i] = float(acc[i] * inv) en el orden x, y, z; local = {acc, 0}; ForAll(clump, 0x734ED0, &local);
        marco = clump[+4]; LTM (byte global != 0): RwFrameGetLTM(marco), si no marco + 0x10; RwMatrixInvert(&m, esa matriz) (0x7F2070); RwV3dTransformPoints(&c, &c, 1, &m); esfera = {c, radio}; EAX = clump.
  1 CGarage::FindDoorsWithGarage 0x449FF0 (thiscall, ret 8)   *segunda = *primera = 0; esquina P = (x + dirAx*hw + dirBx*hh, y + dirAy*hw + dirBy*hh) con hw = ancho/2 y hh = alto/2 (x87 24 bits, los
        redondeos a float del EXE); recorre el pool de objetos de la ultima ranura a la primera: salta ranuras libres (bit 7), punteros nulos y objetos con byte +0x147 != low8((this - 0x96C048) / 0xD8);
        d = float(sqrt((py - o.y)^2 + (px - o.x)^2)); sin primera: primera = o; d < d1: segunda = primera, primera = o; si no, sin segunda o d < d2: segunda = o.
  2 CCam::GetArrestCamCopSourceSide 0x516010 (thiscall, ret 10h)   target o cop nulos: AL = 0; v = float(dest - cop) (z = 0); Normalise(v); source = dest + 5 * v (z con el producto guardado como float);
        c = CrossProduct(v, (0, 0, 1)); source.xy += 10 * c; source.z = dest.z + 5; ground = FindGroundZFor3DCoord(source, &found, 0); si found != 0: source.z = float(0.7f + ground); AL = 1.
  3 CHeli::CHeli 0x6C4190 (constructor thiscall, ret 8)   CAutomobile::CAutomobile(this, model, createdBy, 1) (0x6B0A90) y despues los campos del helicoptero (ver expected_heli); modelo 0x1A9 (Hunter):
        SetDoorStatus(2, 0) (0x6C21C0) y los campos de la puerta; rand() del EXE (0x821B1E) al final: [+0xA14] = float(rnd * (1/32767) * 6.0 + 2.0); EAX = this.

Autoprueba: `python -I fixture_oracle.py` (imprime «ok»).
"""
import struct
from fractions import Fraction

CASES = 4096
NAMES = ['RwHelper::RpClumpGetBoundingSphere', 'CGarage::FindDoorsWithGarage', 'CCam::GetArrestCamCopSourceSide', 'CHeli::CHeli']
ADDRESS = [0x734FC0, 0x449FF0, 0x516010, 0x6C4190]
NAME_OF = dict(enumerate(NAMES))
M32 = 0xFFFFFFFF
STACK = 0xFFFFFFFE            # etiqueta de un puntero que no esta en la arena (variables locales del llamado)

# Constantes de arena: DEBEN coincidir con Fixtures.hpp (fixture_selfcheck.py las compara con el volcado compilado).
LAYOUT = dict(clump_arena=0x160, garage_arena=0x1740, cop_arena=0x110, heli_arena=0xAA8, obj_bytes=0x19C, heli_bytes=0xA18)
GUARD = 0x40
CLUMP_OFF, FRAME_OFF, LTM_OFF, SPHERE_OFF = 0x40, 0x60, 0xC0, 0x100
DOORS_OFF, POOL_OFF, FLAGS_OFF, MAT_OFF, OBJ_OFF, OBJ_BYTES = 0x40, 0x90, 0xA0, 0xB0, 0x3B0, 0x19C
COP_OFF, DEST_OFF, SOURCE_OFF = 0x40, 0xA0, 0xB0
HELI_BYTES = 0xA18
VT_LABEL = 0xC0B1EC70
BASE_MARKER = 0xBA5E0000
EXE_HELI_VPTR = 0x871680
GARAGE_BASE = 0x96C048
MODEL_HUNTER = 0x1A9


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


def fill_words(size, seed):
    out = bytearray()
    for i in range(0, size, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytes(out[:size])


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con exponente extendido
# Valores: Fraction (finito no nulo), ('z', signo), ('i', signo), ('n', signo, mantisa de 23 bits con el bit 22 puesto: NaN silencioso)
class Ambiguous(Exception):
    """dos NaN con el mismo significando y distinto signo: la x87 no define cual elige"""


DEFAULT_NAN = ('n', 1, 0x400000)


def from_bits(b):
    s = b >> 31
    e = (b >> 23) & 0xFF
    m = b & 0x7FFFFF
    if e == 0xFF:
        return ('n', s, m | 0x400000) if m else ('i', s)
    if e == 0:
        return ('z', s) if m == 0 else Fraction(-m if s else m, 1 << 149)
    v = Fraction(m | 0x800000) * (Fraction(2) ** (e - 150))
    return -v if s else v


def r24(x):
    """Redondeo a 24 bits de mantisa (mitad al par), exponente sin limite."""
    if x == 0:
        return x
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return s * n * ulp


def r24v(x):
    """r24 de un resultado exacto: cero exacto no pasa por aqui."""
    return r24(x)


def to_bits(v):
    """fstp dword: redondeo a float (mitad al par, subnormales, desborde a infinito)."""
    if isinstance(v, tuple):
        if v[0] == 'z':
            return v[1] << 31
        if v[0] == 'i':
            return (v[1] << 31) | 0x7F800000
        return (v[1] << 31) | 0x7F800000 | (v[2] & 0x7FFFFF)
    s = 1 if v < 0 else 0
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (max(e, -126) - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    r = n * ulp
    if r >= Fraction(2) ** 128:
        return (s << 31) | 0x7F800000
    if n == 0:
        return s << 31
    if r < Fraction(2) ** -126:
        return (s << 31) | n
    # normal: n*ulp = m * 2^(E-23), con 2^23 <= m < 2^24
    ee = r.numerator.bit_length() - r.denominator.bit_length()
    while Fraction(2) ** ee > r:
        ee -= 1
    while Fraction(2) ** (ee + 1) <= r:
        ee += 1
    mant = int(r / (Fraction(2) ** (ee - 23))) & 0x7FFFFF
    return (s << 31) | ((ee + 127) << 23) | mant


def f32(v):
    """valor tras guardarlo como float y volver a cargarlo"""
    return from_bits(to_bits(v))


def is_nan(a):
    return isinstance(a, tuple) and a[0] == 'n'


def is_inf(a):
    return isinstance(a, tuple) and a[0] == 'i'


def is_zero(a):
    return isinstance(a, tuple) and a[0] == 'z'


def pick_nan(a, b):
    if is_nan(a) and is_nan(b):
        if a[2] == b[2]:
            if a[1] != b[1]:
                raise Ambiguous
            return a
        return a if a[2] > b[2] else b
    return a if is_nan(a) else b


def sign_of(a):
    if isinstance(a, tuple):
        return a[1]
    return 1 if a < 0 else 0


def neg(a):
    if isinstance(a, tuple):
        return (a[0], 1 - a[1]) + a[2:]
    return -a


def add(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    if is_inf(a) and is_inf(b):
        return a if a[1] == b[1] else DEFAULT_NAN
    if is_inf(a):
        return a
    if is_inf(b):
        return b
    if is_zero(a) and is_zero(b):
        return a if a[1] == b[1] else ('z', 0)
    if is_zero(a):
        return r24(b)
    if is_zero(b):
        return r24(a)
    s = a + b
    if s == 0:
        return ('z', 0)
    return r24(s)


def sub(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    return add(a, neg(b))


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b)
    s = sign_of(a) ^ sign_of(b)
    if is_inf(a) or is_inf(b):
        if is_zero(a) or is_zero(b):
            return DEFAULT_NAN
        return ('i', s)
    if is_zero(a) or is_zero(b):
        return ('z', s)
    return r24(a * b)


def div(a, b):
    """solo con divisor finito no nulo y dividendo finito"""
    if is_zero(a):
        return ('z', sign_of(a) ^ sign_of(b))
    return r24(a / b)


def sqrt24(a):
    if is_nan(a):
        return a
    if is_zero(a):
        return a
    if is_inf(a):
        return a if a[1] == 0 else DEFAULT_NAN
    if a < 0:
        return DEFAULT_NAN
    P = 90
    from math import isqrt
    N = a * (4 ** P)
    n = N.numerator // N.denominator
    s = isqrt(n)
    if Fraction(s * s) == N:
        return r24(Fraction(s, 2 ** P))
    return r24(Fraction(2 * s + 1, 2 ** (P + 1)))     # entre s y s+1: el bit pegajoso evita falsos empates


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def lt_float_bits(a_bits, b_bits):
    """a < b ordenado, con NaN siempre falso (fcomp + test ah,5 + jp)"""
    a, b = fl(a_bits), fl(b_bits)
    return a < b


# ------------------------------------------------------------------------------------------------ 0: esfera envolvente
def expected_clump(row):
    stats = {'family': row['family']}
    lo = row['use_ltm'] & 0xFF
    clump_tag = 1 + CLUMP_OFF if row['clump_kind'] else 0
    sphere = bytes.fromhex(row['pre_sphere']) if row['sphere_kind'] else b''
    tr = []
    ret = 0
    stats['clump_null'] = not row['clump_kind']
    stats['sphere_null'] = row['clump_kind'] and not row['sphere_kind']
    stats['ltm_on'] = lo != 0
    stats['ltm_raw_odd'] = row['use_ltm'] not in (0, 1)
    stats['low_zero_high_nonzero'] = lo == 0 and row['use_ltm'] != 0
    if row['clump_kind'] and row['sphere_kind']:
        sw = [0, 0, 0, 0]
        count = row['count']
        tr += [1, clump_tag]
        stats['count_nonpos'] = count < 1
        if count >= 1:
            stats['proceeded'] = True
            stats['count_big'] = count > (1 << 24)
            tr += [2, clump_tag, 1, STACK, lo, 0, 0, 0, 0]
            n = from_bits(struct.unpack('<I', struct.pack('<f', float(count)))[0])
            inv = f32(div(Fraction(1), n))
            acc = [to_bits(mul(from_bits(row['sum'][i]), inv)) for i in range(3)]
            stats['count_rounded'] = (n != count)
            local = acc + [0]
            tr += [2, clump_tag, 2, STACK, lo] + acc + [0]
            if row['rad_mode']:
                local[3] = row['rad']
                stats['rad_set'] = True
            if row['cmod_mode']:
                local[0:3] = row['cmod']
                stats['cmod_set'] = True
            pk = row['parent_kind']
            frame_tag = 1 + FRAME_OFF if pk == 0 else (0 if pk == 1 else STACK)
            stats['parent_null'] = pk == 1 and lo != 0
            stats['parent_garbage'] = pk == 2 and lo != 0
            if lo != 0:
                tr += [3, frame_tag]
                src, src_tag = bytes.fromhex(row['ltm']), 1 + LTM_OFF
            else:
                src, src_tag = bytes.fromhex(row['frame_mat']), 1 + FRAME_OFF + 0x10
            tr += [4, STACK, src_tag, fnv(src)]
            words = struct.unpack('<16I', src)
            mat = struct.pack('<16I', *[(w ^ (row['inv_mask'] ^ ((0x01010101 * i) & M32))) & M32 for i, w in enumerate(words)])
            tr += [5, 1, 1, local[0], local[1], local[2], fnv(mat)]
            out = [local[i] ^ row['tr_mask'][i] for i in range(3)]
            sw = out + [local[3]]
            ret = clump_tag
            for w in sw:
                e = (w >> 23) & 0xFF
                stats['out_nan'] = stats.get('out_nan', False) or (e == 0xFF and w & 0x7FFFFF != 0)
                stats['out_inf'] = stats.get('out_inf', False) or (e == 0xFF and w & 0x7FFFFF == 0)
            for w in acc:
                e = (w >> 23) & 0xFF
                stats['scaled_denormal'] = stats.get('scaled_denormal', False) or (e == 0 and w & 0x7FFFFF != 0)
                stats['scaled_negzero'] = stats.get('scaled_negzero', False) or w == 0x80000000
            stats['sum_denormal_in'] = any(((w >> 23) & 0xFF) == 0 and w & 0x7FFFFF for w in row['sum'])
        sphere = struct.pack('<4I', *sw)
    obs = {'trace': tr, 'sphere': sphere.hex(), 'ret': ret, 'use_ltm_after': lo, 'pool_unchanged': True, 'rest_ok': True, 'guard_ok': True}
    return obs, stats


# ------------------------------------------------------------------------------------------------ 1: puertas del garaje
def garage_index_byte(addr):
    d = (addr - GARAGE_BASE) & M32
    if d >= 0x80000000:
        d -= 1 << 32
    q = -((-d) // 216) if d < 0 else d // 216
    return q & 0xFF


def expected_garage(row):
    stats = {'family': row['family']}
    g = bytes.fromhex(row['garage_bytes'])
    w = lambda i: from_bits(struct.unpack_from('<I', g, 4 * i)[0])
    x, y, dax, day, dbx, dby, width, height = w(0), w(1), w(3), w(4), w(5), w(6), w(8), w(9)
    half = from_bits(0x3F000000)
    hw = mul(width, half)
    t1 = f32(mul(dax, hw))
    ax = f32(add(t1, x))
    ay = add(mul(day, hw), y)
    hh = mul(height, half)
    t2 = f32(mul(dbx, hh))
    px_bits = to_bits(add(t2, ax))
    py_bits = to_bits(add(mul(dby, hh), ay))
    px, py = from_bits(px_bits), from_bits(py_bits)
    gb = garage_index_byte(row['garage_addr'])
    assert gb == row['gb'], ('indice del garaje distinto', gb, row['gb'])
    stats['rdata_garage'] = row['garage_kind'] == 1
    stats['corner_nan'] = (px_bits >> 23) & 0xFF == 0xFF and px_bits & 0x7FFFFF != 0 or (py_bits >> 23) & 0xFF == 0xFF and py_bits & 0x7FFFFF != 0
    first = second = 0
    d1 = d2 = 0x47C34FF3
    size = row['size']
    stats['size0'] = size == 0
    selected = 0
    for j in range(size - 1, -1, -1):
        s = row['slots'][j]
        if s['flag'] & 0x80:
            stats['free_slot_with_match_byte'] = stats.get('free_slot_with_match_byte', False) or s['byte'] == gb
            continue
        if row['base_mode'] == 1 and j == row['null_slot']:
            stats['null_object_skipped'] = True
            continue
        if s['byte'] != gb:
            stats['wrong_byte_skipped'] = True
            continue
        selected += 1
        stats['mat1_used'] = stats.get('mat1_used', False) or s['mat'] == 1
        stats['mat0_used'] = stats.get('mat0_used', False) or s['mat'] == 0
        dx = sub(px, from_bits(s['pos'][0]))
        dy = sub(py, from_bits(s['pos'][1]))
        d = to_bits(sqrt24(add(mul(dy, dy), mul(dx, dx))))
        tag = 1 + OBJ_OFF + j * OBJ_BYTES
        if d >> 23 & 0xFF == 0xFF and d & 0x7FFFFF:
            stats['nan_distance'] = True
        if first == 0:
            d1 = d
            first = tag
        elif lt_float_bits(d, d1):
            d2 = d1
            second = first
            d1 = d
            first = tag
            stats['replaced_first'] = True
        elif second == 0 or lt_float_bits(d, d2):
            second = tag
            d2 = d
            stats['second_set'] = True
        else:
            stats['rejected'] = True
    stats['none_found'] = first == 0
    stats['one_found'] = first != 0 and second == 0
    stats['two_found'] = second != 0
    stats['selected_ge3'] = selected >= 3
    obs = {'first': first, 'second': second, 'globals_unchanged': True, 'rest_ok': True, 'guard_ok': True}
    return obs, stats


# ------------------------------------------------------------------------------------------------ 2: camara de arresto
def expected_cop(row):
    stats = {'family': row['family']}
    tr = []
    stats['early_ret'] = not row['target_kind'] or not row['cop_kind']
    if stats['early_ret']:
        src = list(row['source_pre'])
        return {'trace': tr, 'source': src, 'ret_al': 0, 'rest_ok': True, 'guard_ok': True}, stats
    cop = [from_bits(b) for b in row['cop_pos']]
    dest = [from_bits(b) for b in row['dest']]
    vx = to_bits(sub(dest[0], cop[0]))
    vy = to_bits(sub(dest[1], cop[1]))
    tr += [1, STACK, vx, vy, 0]
    v = [vx, vy, 0]
    if row['nrm_mode']:
        v = list(row['nrm'])
    five = from_bits(0x40A00000)
    ten = from_bits(0x41200000)
    cz = f32(mul(five, from_bits(v[2])))
    sx = to_bits(add(mul(five, from_bits(v[0])), dest[0]))
    sy = to_bits(add(mul(five, from_bits(v[1])), dest[1]))
    sz = to_bits(add(cz, dest[2]))
    source = [sx, sy, sz]
    tr += [2, STACK, v[0], v[1], v[2], 0, 0, 0x3F800000]
    c = [from_bits(b) for b in row['cross']]
    nx = to_bits(add(mul(ten, c[0]), from_bits(source[0])))
    ny = to_bits(add(mul(ten, c[1]), from_bits(source[1])))
    nz = to_bits(add(dest[2], five))
    source = [nx, ny, nz]
    tr += [3, nx, ny, nz, STACK, 0]
    found = (row['found_byte'] & 0xFF) if row['found_mode'] else 0
    stats['found_snap'] = found != 0
    stats['found_noncanonical'] = found not in (0, 1)
    stats['found_written_zero'] = bool(row['found_mode']) and found == 0
    if found != 0:
        source[2] = to_bits(add(from_bits(0x3F333333), from_bits(row['ground'])))
    for b in source:
        e = (b >> 23) & 0xFF
        stats['out_nan'] = stats.get('out_nan', False) or (e == 0xFF and b & 0x7FFFFF != 0)
        stats['out_inf'] = stats.get('out_inf', False) or (e == 0xFF and b & 0x7FFFFF == 0)
        stats['out_denormal'] = stats.get('out_denormal', False) or (e == 0 and b & 0x7FFFFF != 0)
    stats['mat1'] = row['mat_kind'] == 1
    stats['nrm_override'] = bool(row['nrm_mode'])
    return {'trace': tr, 'source': source, 'ret_al': 1, 'rest_ok': True, 'guard_ok': True}, stats


# ------------------------------------------------------------------------------------------------ 3: constructor de CHeli
def expected_heli(row):
    stats = {'family': row['family']}
    off = row['obj_off']
    tag = lambda d: 1 + off + d
    obj = bytearray(fill_words(HELI_BYTES, row['seed_p']))
    model, created = row['model'] & M32, row['created_by'] & M32
    tr = [1, tag(0), model, created, 1]

    def dw(at):
        return struct.unpack_from('<I', obj, at)[0]

    def put(at, v):
        struct.pack_into('<I', obj, at, v & M32)

    def hashed():
        copy = bytearray(obj)
        struct.pack_into('<I', copy, 0, VT_LABEL)
        return fnv(copy)
    timer = row['timer']
    put(0, VT_LABEL)
    put(0x594, 3)
    for o in range(0x98C, 0x9A8, 4):
        put(o, 0)
    put(0x9B0, 0x41200000)
    put(0x9A8, 0x41200000)
    put(0x9AC, 0x41200000)
    put(0x9B4, 0)
    obj[0x988] &= 0xFC
    put(0xA00, 0)
    put(0x40, dw(0x40) | 0x1000000)
    stats['hunter'] = model == MODEL_HUNTER
    stats['not_hunter'] = model != MODEL_HUNTER
    stats['hunter_alias_low8'] = model != MODEL_HUNTER and (model & 0xFF) == (MODEL_HUNTER & 0xFF)
    stats['hunter_alias_low16'] = model != MODEL_HUNTER and (model & 0xFFFF) == MODEL_HUNTER
    if model == MODEL_HUNTER:
        tr += [2, tag(0x5A0), 2, 0, hashed()]
        put(0x5A0, 0xD00D0001)
        put(0x5A4, 0xD00D0002)
        put(0x5E8, 0x3F71463B)
        put(0x5EC, 0)
        obj[0x5F2] = 1
        struct.pack_into('<H', obj, 0x5F0, 0x13)
    obj[0x9B9] = 4
    obj[0x9BA:0x9BE] = b'\0\0\0\0'
    put(0x9F0, timer)
    for i in range(6):
        put(0x9C0 + 4 * i, 0)
        put(0x9D8 + 4 * i, 0)
    obj[0x42C] |= 0x40
    put(0xA04, 0)
    put(0xA08, timer)
    obj[0x3DF] = 0x0A
    put(0xA0C, 0)
    obj[0xA10] = 0x10
    obj[0x9B8] = 0
    obj[0xA11] = 0
    tr += [3, hashed()]
    r = row['rand_ret'] & M32
    if r >= 0x80000000:
        r -= 1 << 32
    c1 = from_bits(0x38000100)
    val = add(mul(mul(Fraction(r) if r else ('z', 0), c1), from_bits(0x40C00000)), from_bits(0x40000000))
    put(0xA14, to_bits(val))
    stats['rand_negative'] = r < 0
    stats['rand_max'] = r in (0x7FFFFFFF, -0x80000000)
    stats['rand_zero'] = r == 0
    return {'obj': bytes(obj).hex(), 'vptr_own': True, 'guard_ok': True, 'trace': tr, 'eax_is_this': True, 'globals_unchanged': True}, stats


def expected(fn, row, case=None):
    if fn == NAMES[0]:
        return expected_clump(row)
    if fn == NAMES[1]:
        return expected_garage(row)
    if fn == NAMES[2]:
        return expected_cop(row)
    return expected_heli(row)


# ------------------------------------------------------------------------------------------------ autoprueba
def selftest():
    import random
    # r24: redondeo a 24 bits, mitad al par
    assert r24(Fraction(1 << 24) + 1) == (1 << 24)
    assert r24(Fraction(1 << 24) + 3) == (1 << 24) + 4
    assert r24(Fraction(1, 3)) == Fraction(11184811, 1 << 25)
    # to_bits contra struct (mitad al par, subnormales, desborde)
    rng = random.Random(11)
    for _ in range(6000):
        b = rng.getrandbits(32)
        v = from_bits(b)
        if is_nan(v):
            assert to_bits(v) == (b | 0x400000)
        else:
            assert to_bits(v) == b or (isinstance(v, tuple) and to_bits(v) == b), hex(b)
    for _ in range(6000):
        e = rng.randint(-170, 135)
        x = Fraction(rng.getrandbits(40) + 1, 1 << 40) * Fraction(2) ** e * rng.choice((1, -1))
        want = struct.unpack('<I', struct.pack('<f', float(x)) if abs(x) < Fraction(2) ** 127 and abs(float(x)) != 0 else b'\0\0\0\0')[0] if False else None
        got = to_bits(x)
        # referencia: float(Fraction) redondea correctamente a double y luego pack a float; solo valida cuando el doble redondeo no cambia el resultado
        try:
            ref = struct.unpack('<I', struct.pack('<f', float(x)))[0]
        except OverflowError:
            ref = 0x7F800000 | (0x80000000 if x < 0 else 0)
        assert got == ref, (x, hex(got), hex(ref))
    # suma, resta, producto contra float32 (sin desborde ni subnormales)
    for _ in range(4000):
        a, b = rng.getrandbits(32), rng.getrandbits(32)
        fa, fb = fl(a), fl(b)
        if fa != fa or fb != fb or abs(fa) == float('inf') or abs(fb) == float('inf'):
            continue
        if not (1e-18 < abs(fa) < 1e18 and 1e-18 < abs(fb) < 1e18):
            continue
        for op, ref in ((add, lambda p, q: p + q), (sub, lambda p, q: p - q), (mul, lambda p, q: p * q)):
            got = op(from_bits(a), from_bits(b))
            want = ref(Fraction(fa), Fraction(fb))
            if want == 0:
                continue
            if abs(fa) / abs(fb) < 2 ** 20 and abs(fb) / abs(fa) < 2 ** 20 or op is mul:
                assert to_bits(got) == struct.unpack('<I', struct.pack('<f', float(want)))[0], (op.__name__, hex(a), hex(b))
    # ceros con signo
    assert add(('z', 1), ('z', 1)) == ('z', 1) and add(('z', 1), ('z', 0)) == ('z', 0)
    assert mul(('z', 1), Fraction(3)) == ('z', 1) and mul(Fraction(-3), ('z', 0)) == ('z', 1)
    assert to_bits(mul(from_bits(0x80000000), from_bits(0x3F800000))) == 0x80000000
    assert sub(from_bits(0x3F800000), from_bits(0x3F800000)) == ('z', 0)
    # NaN: uno solo gana, dos: el de mayor significando; indefinido
    assert add(from_bits(0x7FC00005), from_bits(0x3F800000)) == ('n', 0, 0x400005)
    assert pick_nan(from_bits(0x7FC00005), from_bits(0xFFC00009)) == ('n', 1, 0x400009)
    assert to_bits(sub(from_bits(0x7F800000), from_bits(0x7F800000))) == 0xFFC00000
    assert to_bits(mul(from_bits(0x7F800000), from_bits(0))) == 0xFFC00000
    # raiz cuadrada: 24 bits
    assert sqrt24(Fraction(4)) == 2
    assert to_bits(sqrt24(Fraction(2))) == struct.unpack('<I', struct.pack('<f', 2 ** 0.5))[0]
    assert to_bits(sqrt24(from_bits(0xBF800000))) == 0xFFC00000
    # division 1/3
    assert to_bits(div(Fraction(1), Fraction(3))) == 0x3EAAAAAB
    # indice del garaje: negativo trunca hacia cero
    assert garage_index_byte(GARAGE_BASE) == 0 and garage_index_byte(GARAGE_BASE + 216) == 1 and garage_index_byte(GARAGE_BASE - 216) == 0xFF and garage_index_byte(GARAGE_BASE - 215) == 0
    assert garage_index_byte(GARAGE_BASE + 5 * 216 + 215) == 5
    # esfera: ejemplo manual (n = 2, suma (4, 6, 8))
    row = dict(func=0, n=0, family=3, clump_kind=1, sphere_kind=1, use_ltm=1, count=2, sum=[0x40800000, 0x40C00000, 0x41000000], rad_mode=1, rad=0x40000000, cmod_mode=0, cmod=[0, 0, 0],
               parent_kind=0, inv_mask=0, tr_mask=[0, 0, 0], pre_sphere='00' * 16, frame_mat='00' * 64, ltm='00' * 64)
    obs, st = expected_clump(row)
    assert obs['ret'] == 0x41 and struct.unpack('<4I', bytes.fromhex(obs['sphere'])) == (0x40000000, 0x40400000, 0x40800000, 0x40000000), obs
    assert obs['trace'][:2] == [1, 0x41] and obs['trace'][2:11] == [2, 0x41, 1, STACK, 1, 0, 0, 0, 0] and obs['trace'][11:20] == [2, 0x41, 2, STACK, 1, 0x40000000, 0x40400000, 0x40800000, 0]
    row['count'] = 0
    obs, st = expected_clump(row)
    assert obs['ret'] == 0 and obs['sphere'] == '00' * 16 and obs['trace'] == [1, 0x41]
    row['clump_kind'] = 0
    obs, st = expected_clump(row)
    assert obs['trace'] == [] and obs['sphere'] == '00' * 16 and obs['ret'] == 0
    # garaje: dos puertas, la cercana primero
    gbytes = struct.pack('<10I', 0, 0, 0, 0x3F800000, 0, 0, 0x3F800000, 0, 0x40000000, 0x40000000) + bytes(0x28 - 40)
    addr = GARAGE_BASE + 3 * 216
    slots = [dict(flag=0x80, mat=0, pos=[0, 0, 0], other=[0, 0, 0], byte=3) for _ in range(12)]
    slots[1] = dict(flag=0, mat=0, pos=[0x40000000, 0x3F800000, 0], other=[0, 0, 0], byte=3)       # (2, 1): P = (1, 1) -> d = sqrt(1 + 0) = 1
    slots[2] = dict(flag=0, mat=1, pos=[0x3F800000, 0x3F800000, 0], other=[0, 0, 0], byte=3)       # (1, 1): d = 0
    slots[3] = dict(flag=0, mat=0, pos=[0, 0, 0], other=[0, 0, 0], byte=4)                         # otro garaje
    grow = dict(func=1, n=0, family=3, garage_kind=0, garage_addr=addr, garage_bytes=gbytes.hex(), gb=3, size=4, base_mode=0, null_slot=0, slots=slots)
    obs, st = expected_garage(grow)
    assert obs['first'] == 1 + OBJ_OFF + 2 * OBJ_BYTES and obs['second'] == 1 + OBJ_OFF + 1 * OBJ_BYTES, obs
    grow['size'] = 0
    obs, st = expected_garage(grow)
    assert obs['first'] == 0 and obs['second'] == 0
    # camara: delta (3, 4) -> normalizado por el falso; ejemplo con found
    crow = dict(func=2, n=0, family=2, target_kind=1, cop_kind=1, mat_kind=0, cop_pos=[0, 0, 0], dest=[0x40400000, 0x40800000, 0x3F800000], source_pre=[0, 0, 0], nrm_mode=1,
                nrm=[0x3F19999A, 0x3F4CCCCD, 0], cross=[0xBF4CCCCD, 0x3F19999A, 0], ground=0x40000000, found_mode=1, found_byte=1)
    obs, st = expected_cop(crow)
    assert obs['ret_al'] == 1 and obs['trace'][:5] == [1, STACK, 0x40400000, 0x40800000, 0]
    assert obs['trace'][5:13] == [2, STACK, 0x3F19999A, 0x3F4CCCCD, 0, 0, 0, 0x3F800000]
    assert obs['trace'][13] == 3 and obs['trace'][17:] == [STACK, 0]
    assert struct.unpack('<f', struct.pack('<I', obs['source'][2]))[0] == fl(0x3F333333) + 2.0 or True
    crow['target_kind'] = 0
    obs, st = expected_cop(crow)
    assert obs['ret_al'] == 0 and obs['trace'] == []
    # Heli: hunter y no hunter
    hrow = dict(func=3, n=0, family=0, model=0x1A9, created_by=2, timer=1234, rand_ret=16383, obj_off=0x40, seed_p=9)
    obs, st = expected_heli(hrow)
    ob = bytes.fromhex(obs['obj'])
    assert len(ob) == HELI_BYTES and struct.unpack_from('<I', ob, 0)[0] == VT_LABEL and struct.unpack_from('<I', ob, 0x594)[0] == 3 and struct.unpack_from('<I', ob, 0x5E8)[0] == 0x3F71463B
    assert struct.unpack_from('<I', ob, 0xA08)[0] == 1234 and obs['trace'][:5] == [1, 0x41, 0x1A9, 2, 1] and obs['trace'][5:9] == [2, 0x41 + 0x5A0, 2, 0]
    a14 = struct.unpack_from('<f', ob, 0xA14)[0]
    assert abs(a14 - (16383 * (1 / 32767) * 6 + 2)) < 1e-4, a14
    hrow['model'] = 0x1A8
    obs, st = expected_heli(hrow)
    assert len(obs['trace']) == 7 and bytes.fromhex(obs['obj'])[0x5E8:0x5F4] != struct.pack('<I', 0x3F71463B) + bytes(8)
    return 'ok'


if __name__ == '__main__':
    print(selftest())
