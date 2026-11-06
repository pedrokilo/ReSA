"""Oraculo independiente (Python) de Fn106A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las siete funciones del lote 106A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.

Funcion 0, CrtPowThunk (0x470000..0x47000D; cdecl, dos floats en la pila; resultado en ST0): `fld [esp+4]; fld [esp+8]; call _CIpow (0x822130); ret`: pow(base, exponente) en ST0 como double (el _CIpow del EXE
  carga en ST0 un double). Reglas de los casos especiales (las del EXE, que difieren de C99 en +-1 elevado a NaN/infinito y NaN elevado a +-0, donde da NaN) y, en los casos finitos, el resultado debe estar a
  un ulp de double del valor exacto (aritmetica decimal de 60 digitos): el _CIpow del EXE y la DLL son los mismos algoritmos y tablas y no estan garantizados al redondeo correcto, asi que el oraculo no exige el
  ultimo bit; la igualdad exacta entre EXE y DLL la exige la comparacion de las filas. NaN: cualquier NaN.
Funcion 1, RoundToDecimals (0x50A0A0..0x50A115; cdecl, (float v, int n); resultado en ST0): p1 = pow(10, n + 1) (n + 1 en 32 bits con vuelta; double del EXE); x = (p1 * v + 5) * 0.1f con la suma de 5 si v > 0, v == 0
  o NaN y la resta si v < 0, cada operacion redondeada a 24 bits (palabra de control del juego) con exponente ampliado; x se guarda como double (desbordamiento a infinito, desnormales) y modf (0x8232F0, rama SSE2)
  le quita la parte fraccionaria (|x| < 1 da +-0 con el signo de x; el infinito y x >= 2^52 quedan; NaN queda); se devuelve ip / pow(10, n) con 24 bits (division por cero: infinito o NaN). pow(10, k) se toma de
  math.pow de Python (autoprueba: a un ulp del valor exacto).
Funcion 2, CTaskSimpleCarSetPedOut constructor (0x6478B0..0x647923; thiscall, ECX = tarea, (vehiculo, puerta, byte) en la pila, `ret 0Ch`; EAX = this): llama a 0x61A390 (this); [+0xC] = puerta; [+0] = 0x86EEB8;
  [+8] = vehiculo; [+0x10] = byte tal cual; [+0x11..+0x15] = 0; si el vehiculo no es nulo llama a 0x571B70 (vehiculo, &[+8]).
Funcion 3, GxtCharToAscii (0x69F7E0..0x69F8BB; cdecl, (cadena, byte); EAX = 0xC1BDD0): puntero = cadena + byte (si el byte no es cero); si no es nulo, hasta 255 caracteres no nulos convertidos con la tabla de
  rangos de bytes (0x80..0x83 +0x40, 0x84..0x8D +0x42, 0x8E..0x91 +0x44, 0x92..0x95 +0x47, 0x96..0x9A +0x49, 0x9B..0xA4 +0x4B, 0xA5..0xA8 +0x4D, 0xA9..0xCC +0x50 (modulo 256), 0xCD -> 0xD1, 0xCE -> 0xF1,
  0xCF -> 0xBF, el resto 0x23); terminador en la posicion siguiente.
Funcion 4, cuerpo del destructor de CEventKnockOffBike (0x4AFE70..0x4AFED0; thiscall): [+0] = 0x85B2E0; si [+0x38] no es nulo, 0x571A00(vehiculo, &[+0x38]); si [+0x30] no es nulo, 0x571A00(peaton, &[+0x30]);
  [+0] = 0x85AB38.
Funcion 5, cuerpo del destructor de CTaskSimpleStandStill (0x62F360; thiscall): [+0] = 0x86DD2C y salta a 0x61A3A0 (this), que deja [+0] = 0x86D48C.
Funcion 6, RwIm2DRenderPrimitiveThunk (0x734E90; cdecl): `mov eax, [0xC97B24]; jmp [eax + 0x30]`: la funcion del dispositivo recibe (tipo, vertices, numero) y su EAX es el retorno.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from decimal import Decimal, Context, MAX_EMAX, MIN_EMIN
from fractions import Fraction

NAN = ('n',)
M32 = 0xFFFFFFFF


def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def ilog2(a):
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    return e


def rnd(v, mbits=23, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    q = ilog2(a) - mbits
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


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def sgn(a):
    return a[1] if a[0] == 'i' else (a[2] if a[1] == 0 else (1 if a[1] < 0 else 0))


def div(a, b):
    """a / b a 24 bits con exponente ampliado (la x87 de 24 bits), con division por cero y operandos no finitos"""
    if a == NAN or b == NAN:
        return NAN
    s = sgn(a) ^ sgn(b)
    if a[0] == 'i':
        return NAN if b[0] == 'i' else ('i', s)
    if b[0] == 'i':
        return ('f', Fraction(0), s)
    if b[1] == 0:
        return NAN if a[1] == 0 else ('i', s)
    return fin(a[1] / b[1], a[2] ^ b[2])


def store_double(a):
    """fstp qword: rinde a 53 bits (desnormales hasta 2^-1074) y desborda a infinito"""
    if a == NAN or a[0] == 'i':
        return a
    v = a[1]
    if v == 0:
        return a
    r = rnd(v, 52, -1074)
    if abs(r) >= Fraction(2) ** 1024:
        return ('i', 1 if v < 0 else 0)
    return ('f', r, 1 if v < 0 else 0) if r != 0 else ('f', Fraction(0), 1 if v < 0 else 0)


def trunc_modf(a):
    """parte entera como el modf del EXE (rama SSE2)"""
    if a == NAN or a[0] == 'i':
        return a
    v = a[1]
    if abs(v) < 1:
        return ('f', Fraction(0), a[2] if v == 0 else (1 if v < 0 else 0))
    if abs(v) >= Fraction(2) ** 52:
        return a
    t = abs(v).numerator // abs(v).denominator
    return ('f', Fraction(t if v > 0 else -t), 1 if v < 0 else 0)


def dbl_class(x):
    """float de Python (double) -> clase del modelo"""
    if x != x:
        return NAN
    if math.isinf(x):
        return ('i', 1 if x < 0 else 0)
    return ('f', Fraction(x), 1 if math.copysign(1.0, x) < 0 else 0)


def pow10(k):
    """pow(10, k) como lo devuelve el _CIpow del EXE: el MXCSR del juego (0x9FB6) tiene FZ=1 y la rama SSE2 vacia a cero los resultados menores que el minimo normal de double"""
    try:
        v = math.pow(10.0, float(k))
    except OverflowError:
        return math.inf
    return 0.0 if v < 2.2250738585072014e-308 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo; NaN = indefinido"""
    if a == NAN:
        return (0xC000000000000000).to_bytes(8, 'little') + (0xFFFF).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, ng = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + ((ng << 15)).to_bytes(2, 'little')
    sign = 1 if v < 0 else 0
    m = abs(v)
    e = ilog2(m)
    mant = m / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return int(mant).to_bytes(8, 'little') + ((sign << 15) | (e + 16383)).to_bytes(2, 'little')


def decode80(raw):
    """(clase, valor) de 10 bytes de x87: 'nan', 'inf' (signo), 'zero' (signo) o 'f' (Fraction)"""
    mant = int.from_bytes(raw[:8], 'little')
    se = int.from_bytes(raw[8:10], 'little')
    sign = se >> 15
    e = se & 0x7FFF
    if e == 0x7FFF:
        return ('inf', sign) if mant == 0x8000000000000000 else ('nan', sign)
    if mant == 0:
        return ('zero', sign)
    ee = e - 16383 if e else -16382
    v = Fraction(mant) * Fraction(2) ** (ee - 63)
    return ('f', -v if sign else v)


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


GUARD = 0x40
A_TASK, A_EV, A_ENTA, A_ENTB, A_STR, A_GLOB, A_VTX = range(7)


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def le32(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


# ------------------------------------------------------------------------------------------------ funcion 0: pow
DCTX = Context(prec=60, Emax=MAX_EMAX, Emin=MIN_EMIN, traps=[])


def fbits(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def is_odd_int(y):
    return math.isfinite(y) and y == math.floor(y) and abs(y) < 2 ** 53 and int(y) % 2 == 1


def is_int(y):
    return math.isfinite(y) and y == math.floor(y)


def pow_model(xb, yb):
    """(tipo, signo|valor exacto): 'nan', 'inf' s, 'zero' s, 'one', 'finite' (Fraction exacto con signo)"""
    x, y = fbits(xb), fbits(yb)
    xn, yn = x != x, y != y
    if xn:
        return ('nan', 0)                                  # tambien NaN elevado a +-0 (el EXE)
    if y == 0:
        return ('exact', Fraction(1))
    if yn:
        return ('nan', 0)
    if abs(x) == 1.0 and math.isinf(y):
        return ('nan', 0)                                  # +-1 elevado a infinito (el EXE)
    if x == 0:
        negz = math.copysign(1.0, x) < 0 and is_odd_int(y)
        if y < 0:
            return ('inf', 1 if negz else 0)
        return ('zero', 1 if negz else 0)
    if math.isinf(x):
        negr = x < 0 and is_odd_int(y)
        if y > 0:
            return ('inf', 1 if negr else 0)
        return ('zero', 1 if negr else 0)
    if math.isinf(y):
        big = abs(x) > 1.0
        if (y > 0) == big:
            return ('inf', 0)
        return ('zero', 0)
    if x < 0 and not is_int(y):
        return ('nan', 0)
    negr = x < 0 and is_odd_int(y)
    if abs(x) == 1.0:
        return ('exact', Fraction(-1 if negr else 1))
    est = DCTX.log10(Decimal(abs(x))) * Decimal(y)   # decimas de magnitud del resultado: fuera de +-400 solo importa que desborda o se anula
    if est > 400:
        return ('inf', 1 if negr else 0)
    if est < -400:
        return ('zero', 1 if negr else 0)
    r = DCTX.power(Decimal(abs(x)), Decimal(y))
    if r.is_infinite():
        return ('inf', 1 if negr else 0)
    if r == 0:
        return ('zero', 1 if negr else 0)
    fr = Fraction(r)
    return ('approx', -fr if negr else fr)


def expected0(c):
    a = c['a']
    m = pow_model(a[0], a[1])
    marks = []
    x, y = fbits(a[0]), fbits(a[1])
    if x != x or y != y:
        marks.append('nan_in')
    elif x == 0:
        marks.append('zero_base')
    elif math.isinf(x):
        marks.append('inf_base')
    elif math.isinf(y):
        marks.append('exp_inf')
    elif x < 0:
        marks.append('neg_base_int' if is_int(y) else 'neg_base_frac')
    else:
        marks.append('pos_base')
    if abs(x) == 1.0 and (y != y or math.isinf(y)):
        marks.append('one_special')
    marks.append('res_' + m[0])
    return {'events': [], 'marks': marks, 'changes': [], 'st0': ('pow', m)}


# ------------------------------------------------------------------------------------------------ funcion 1: redondeo a decimales
def expected1(c):
    a = c['a']
    v = load(a[0])
    n = s32(a[1])
    k1 = s32(n + 1)
    marks = []
    p1 = dbl_class(pow10(k1))
    t = mul(p1, v)
    vneg = v != NAN and ((v[0] == 'i' and v[1] == 1) or (v[0] == 'f' and v[1] < 0))
    five = load(0x40A00000)
    t = sub(t, five) if vneg else add(t, five)
    t = mul(t, load(0x3DCCCCCD))
    td = store_double(t)
    ip = trunc_modf(td)
    pw2 = dbl_class(pow10(n))
    r = div(ip, pw2)
    if v == NAN:
        marks.append('v_nan')
    elif v[0] == 'i':
        marks.append('v_inf')
    elif v[1] == 0:
        marks.append('v_zero')
    else:
        marks.append('v_neg' if v[1] < 0 else 'v_pos')
    marks.append('n_neg' if n < 0 else ('n_zero' if n == 0 else 'n_pos'))
    if abs(n) > 250:
        marks.append('n_extreme')
    if r == NAN:
        marks.append('res_nan')
    elif r[0] == 'i':
        marks.append('res_inf')
    else:
        marks.append('res_zero' if r[1] == 0 else 'res_finite')
    return {'events': [], 'marks': marks, 'changes': [], 'st0': r}


# ------------------------------------------------------------------------------------------------ funcion 2: constructor de CTaskSimpleCarSetPedOut
def changes_from(pre_hex, arena, new):
    """[[arena, desplazamiento, byte nuevo]] de los bytes que difieren de la imagen previa; `new` = {desplazamiento: byte}"""
    pre = bytes.fromhex(pre_hex)
    return [[arena, off, b] for off, b in sorted(new.items()) if pre[off] != b]


def expected2(c, pre, ent_a):
    a = c['a']
    veh = [ent_a, 0, 0x00C0FFEE][a[0]]   # el valor guardado en +8 es la direccion real de la arena (ent_a, la registra el banco); el evento la etiqueta
    marks = [['veh_arena', 'veh_null', 'veh_raw'][a[0]]]
    byte = a[2] & 0xFF
    marks.append('byte_zero' if byte == 0 else ('byte_one' if byte == 1 else 'byte_other'))
    if a[3] & 0xFFFFFF00:
        marks.append('dirty_high')
    new = {}
    for j, b in enumerate(le32(0x86EEB8)):
        new[j] = b
    for j, b in enumerate(le32(veh)):
        new[8 + j] = b
    for j, b in enumerate(le32(a[1])):
        new[12 + j] = b
    new[0x10] = byte
    for off in range(0x11, 0x16):
        new[off] = 0
    ev = [[10, tag(A_TASK, 0)]]
    if veh:
        ev.append([11, tag(A_ENTA, 0) if a[0] == 0 else veh, tag(A_TASK, 8)])
    return {'events': ev, 'marks': marks, 'changes': changes_from(pre, A_TASK, new), 'st0': None, 'ret_eax': tag(A_TASK, 0)}


# ------------------------------------------------------------------------------------------------ funcion 3: GxtCharToAscii
RANGES = [(0x80, 0x83, 0x40), (0x84, 0x8D, 0x42), (0x8E, 0x91, 0x44), (0x92, 0x95, 0x47), (0x96, 0x9A, 0x49), (0x9B, 0xA4, 0x4B), (0xA5, 0xA8, 0x4D), (0xA9, 0xCC, 0x50)]


def gxt_conv(b):
    if b < 0x80:
        return b
    for lo, hi, add_ in RANGES:
        if lo <= b <= hi:
            return (b + add_) & 0xFF
    if b == 0xCD:
        return 0xD1
    if b == 0xCE:
        return 0xF1
    if b == 0xCF:
        return 0xBF
    return 0x23


def expected3(c, obs):
    a = c['a']
    src = bytes.fromhex(obs['src'])
    pre = bytearray.fromhex(obs['buf_pre'])
    marks = []
    start = a[2] & 0xFF
    out = []
    if a[1] == 2:
        marks.append('null_src')
    else:
        i = start
        while len(out) < 255 and i < len(src) and src[i] != 0:
            out.append(gxt_conv(src[i]))
            i += 1
        if start:
            marks.append('offset')
        if len(out) >= 255:
            marks.append('long255')
        if a[1] == 1:
            marks.append('unterminated')
        if not out:
            marks.append('empty')
        if any(0x80 <= b <= 0xCF for b in src[start:start + len(out)]):
            marks.append('accents')
        if any(b >= 0xD0 for b in src[start:start + len(out)]):
            marks.append('high_codes')
        if all(b < 0x80 for b in src[start:start + len(out)]) and out:
            marks.append('ascii')
    new = bytearray(pre)
    for j, b in enumerate(out):
        new[j] = b
    new[len(out)] = 0
    return {'events': [], 'marks': marks, 'changes': [], 'st0': None, 'ret_eax': 0xC1BDD0, 'buf': new.hex(), 'src_len_needed': start + len(out) + 1}


# ------------------------------------------------------------------------------------------------ funcion 4: destructor de CEventKnockOffBike
def expected4(c, pre):
    a = c['a']
    veh = [tag(A_ENTA, 0), 0, 0x00BADC00][a[0]]
    ped = [tag(A_ENTB, 0), 0, 0x00BADC40][a[1]]
    marks = []
    ev = []
    if veh:
        ev.append([20, veh, tag(A_EV, 0x38), 0x85B2E0])
    if ped:
        ev.append([20, ped, tag(A_EV, 0x30), 0x85B2E0])
    marks.append('both' if veh and ped else ('veh_only' if veh else ('ped_only' if ped else 'none')))
    if a[0] == 2:
        marks.append('raw_veh')
    if a[1] == 2:
        marks.append('raw_ped')
    if a[2] == 0x85AB38:
        marks.append('vtable_same')
    new = {j: b for j, b in enumerate(le32(0x85AB38))}
    return {'events': ev, 'marks': marks, 'changes': changes_from(pre, A_EV, new), 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 5: destructor de CTaskSimpleStandStill
def expected5(c, pre):
    a = c['a']
    new = {j: b for j, b in enumerate(le32(0x86D48C))}
    return {'events': [[30, tag(A_TASK, 0), 0x86DD2C]], 'marks': ['vt_expected' if a[0] == 0x86DD2C else 'vt_other'], 'changes': changes_from(pre, A_TASK, new), 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 6: RwIm2DRenderPrimitiveThunk
def expected6(c):
    a = c['a']
    vtx = [tag(A_VTX, 0), 0, 0x00C80468][a[1]]
    marks = ['verts_' + ['arena', 'null', 'raw'][a[1]]]
    marks.append('ret_zero' if a[3] == 0 else ('ret_one' if a[3] == 1 else 'ret_other'))
    if a[2] == 0:
        marks.append('num_zero')
    return {'events': [[40, a[0], vtx, a[2]]], 'marks': marks, 'changes': [], 'st0': None, 'ret_eax': a[3], 'engine_final': tag(A_GLOB, 0)}


def expected(c, obs=None):
    fn = c['fn']
    if fn == 0:
        return expected0(c)
    if fn == 1:
        return expected1(c)
    if fn == 2:
        return expected2(c, obs['pre'] if obs else '00' * 0x18, obs['ent_a'] if obs else 0)
    if fn == 3:
        return expected3(c, obs)
    if fn == 4:
        return expected4(c, obs['pre'] if obs else '00' * 0x3C)
    if fn == 5:
        return expected5(c, obs['pre'] if obs else '00' * 0x20)
    return expected6(c)


def one_ulp_window(exact):
    """(min, max) de las Fraction a un ulp de double del valor exacto (positivo o negativo)"""
    a = abs(exact)
    e = ilog2(a)
    q = max(e - 52, -1074)
    ulp = Fraction(2) ** q
    return exact - ulp, exact + ulp


def check_pow(valid, spec):
    if len(valid) != 1 or valid[0][0] != 0:
        return 'x87: no hay exactamente ST0 valido: %s' % [(st, v.hex()) for st, v in valid]
    kind, val = decode80(valid[0][1])
    t, w = spec
    if t == 'nan':
        return None if kind == 'nan' else 'ST0 deberia ser NaN: %s' % (valid[0][1].hex())
    if t == 'inf':
        return None if (kind == 'inf' and val == w) else 'ST0 deberia ser infinito (signo %d): %s' % (w, valid[0][1].hex())
    if t == 'zero':
        return None if (kind == 'zero' and val == w) else 'ST0 deberia ser cero (signo %d): %s' % (w, valid[0][1].hex())
    if t == 'exact':
        return None if (kind == 'f' and val == w) else 'ST0 deberia ser %s: %s' % (w, valid[0][1].hex())
    # approx
    if kind == 'zero' and abs(w) < Fraction(2) ** -1022 and val == (1 if w < 0 else 0):
        return None   # el MXCSR del juego (0x9FB6) tiene FZ=1: la rama SSE2 del _CIpow vacia a cero los desnormales de double
    if kind != 'f':
        # desbordamiento o subdesbordamiento al redondear a double
        try:
            float(w)
        except OverflowError:
            return None if kind == 'inf' and val == (1 if w < 0 else 0) else 'ST0 deberia desbordar: %s' % valid[0][1].hex()
        lo, hi = one_ulp_window(w)
        if kind == 'zero' and lo <= 0 <= hi and val == (1 if w < 0 else 0):
            return None
        return 'ST0 no es finito y deberia serlo (%s): %s' % (float(w), valid[0][1].hex())
    if abs(w) < Fraction(2) ** -1022:
        return None   # (kind == 'f' aqui es un desnormal) valido
    lo, hi = one_ulp_window(w)
    if not (lo <= val <= hi):
        return 'ST0 fuera de un ulp del valor exacto %s: %s' % (float(w), float(val))
    try:
        if Fraction(float(val)) != val:
            return 'ST0 no es un double: %s' % valid[0][1].hex()
    except OverflowError:
        return 'ST0 desborda double: %s' % valid[0][1].hex()
    return None


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if 'ret_eax' in e and o.get('ret_eax') != e['ret_eax']:
        return 'EAX %s != %s' % (o.get('ret_eax'), e['ret_eax'])
    if 'buf' in e and o.get('buf') != e['buf']:
        return 'buffer: %s != %s' % (str(o.get('buf'))[:200], e['buf'][:200])
    if 'engine_final' in e and o.get('engine_final') != e['engine_final']:
        return 'puntero del motor %s != %s' % (o.get('engine_final'), e['engine_final'])
    top, valid = fp_stack(side_row['post_floating_state'])
    st0 = e['st0']
    if st0 is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    elif isinstance(st0, tuple) and st0 and st0[0] == 'pow':
        return check_pow(valid, st0[1])
    else:
        want = ext80(st0)
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != want:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], want.hex())
    return None


def check_row(row):
    c = row['input']
    for side in ('original', 'candidate'):
        obs = row[side]['observed']
        e = expected(c, obs)
        why = check_side(c, obs, e, row[side])
        if why:
            return side, why
    return None


def marks_of(row):
    return expected(row['input'], row['original']['observed'])['marks']


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 12:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
