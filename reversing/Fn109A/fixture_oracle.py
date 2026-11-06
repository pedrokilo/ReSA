"""Oraculo independiente (Python) de Fn109A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 16 funciones del lote 109A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.
Reconstruye ademas las arenas del banco (relleno pseudoaleatorio de FillWords y las escrituras de preparacion) para calcular los bytes cambiados.

Desensamblado (EXE canonico):
  0x4997A0/0x4997E0/0x499820/0x499860 (thiscall, ECX = pieza, puntero de salida en la pila, `ret 4`): lee dos palabras con signo (movsx) en +0xC/+0xE, +0x10/+0x12, +0x14/+0x16, +0x18/+0x1A (x, y),
     las convierte con fild y multiplica por 0.25 (0x858C84): [salida] = x * 0.25, [salida + 4] = y * 0.25; EAX = puntero de salida.
  0x509C50 (thiscall): beta = float +0xBC: si beta > pi (0x858CB8) -> beta - 2pi (0x858CBC); si no, si beta < -pi (0x858CC0) -> beta + 2pi; con NaN no cambia nada.
  0x509DC0 (thiscall): objetivo = dword +0x21C; nulo -> 0; (byte +0x36 & 7) != 3 -> 0; si no el byte [objetivo + 0x5B4 + 0x1C * movsx(byte +0x718)] (AL).
  0x509A30 (cdecl): 1 si el argumento (32 bits) coincide con alguna de las 14 PALABRAS sin signo (movzx) de 0x8CD518, 51C, 520, 5FC, 614, 528, 52C, 620, 4FC, 504, 508, 50C, 514, 510; si no 0.
  0x4DC670 (cdecl, dos punteros): 1 si x, y y z son iguales (fcomp; un NaN nunca es igual; cero con signo igual a cero). 0x509760: el contrario exacto (`!iguales`).
  0x4ABCA0 (thiscall): 1 si el byte +0x40 y el byte +0x0C son ambos distintos de cero.
  0x5FFEE0 / 0x5FFF20 (thiscall): EAX = dword +0x4C.
  0x41B7C0 (cdecl) = `mov ecx, [0xB744B0]; jmp 0x41B5B0` (CPool::New del pool de rutas: busca desde cursor + 1 con una sola vuelta, marca el estado sin el bit 7, incrementa la referencia de 7 bits y deja el cursor
     en el indice; devuelve objetos + 100 * indice; pool lleno -> cursor 0 y 0).
  0x41B7D0 (cdecl, puntero): indice = (puntero - objetos) / 100 (con signo); estado |= 0x80; si indice < cursor (con signo) cursor = indice.
  0x5A8050 / 0x5A8080 (thiscall, `ret 0Ch`): GetTextureDependency (0x5A7EA0: parte sin signo > 17 -> 10; tabla de bytes 0x5A7F18 y de saltos 0x5A7EF0); la version por nombre calcula primero el hash de la textura y el
     del modelo (0x53CF30, solo si el nombre no es nulo); luego claves de textura en +0x28 + 4 * parte (sin limite) y, si la dependencia no es 10, clave de modelo en 4 * dependencia.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

NAN = ('n',)


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


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
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


def sqrt24(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN"""
    if a == NAN:
        return None
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


def reload(bits):
    return NAN if bits is None else load(bits)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / jne): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jp: cero solo si es igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero aqui)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]




STACK = 0x7F000000
GUARD = 0x40
A_TASK, A_PED, A_PMAT, A_OTHER, A_OMAT, A_OBJ, A_ROUTE = range(7)
M32 = 0xFFFFFFFF
DEFAULT_MARGIN = 0x3EB33333   # valor inicial de 0x8D22B0 (0.35f) en las funciones que no lo tocan


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def lt(a, b):
    return gt(b, a)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(x):
    """valor tras guardarlo como float (fstp dword) y volver a cargarlo"""
    return reload(store(x))


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo; NaN = indefinido"""
    if a == NAN:
        return (0xC000000000000000).to_bytes(8, 'little') + (0xFFFF).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + ((neg << 15)).to_bytes(2, 'little')
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



import struct

GUARD = 0x40
WIN = 0x760
ARENA = GUARD + WIN + GUARD
NARENAS = 7
M32 = 0xFFFFFFFF
LAMP = [0x8CD518, 0x8CD51C, 0x8CD520, 0x8CD5FC, 0x8CD614, 0x8CD528, 0x8CD52C, 0x8CD620, 0x8CD4FC, 0x8CD504, 0x8CD508, 0x8CD50C, 0x8CD514, 0x8CD510]
# Tabla de 0x5A7F18 (18 bytes) y de saltos 0x5A7EF0 -> modelo (10 = ninguno)
DEP_BYTE = [0, 1, 2, 3, 4, 4, 4, 4, 4, 4, 4, 4, 4, 5, 6, 7, 8, 9]
DEP_JUMP = [0, 1, 3, 4, 10, 5, 6, 7, 8, 9]
PI_BITS, TWO_PI_BITS, NPI_BITS = 0x40490FDB, 0x40C90FDB, 0xC0490FDB


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32_16(v):
    """extension con signo de una palabra a 32 bits"""
    return s16(v) & M32


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 109001) & M32)


def fill(seed):
    out = bytearray(ARENA)
    for i in range(0, ARENA, 4):
        out[i:i + 4] = rand(seed & M32, i // 4).to_bytes(4, 'little')
    return out


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def fnv_key(data):
    return mix(fnv(data) ^ 0x5A5A1234)


def f32bits(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


class Img:
    """Imagen perezosa de las arenas: relleno de FillWords calculado solo para los bytes que se tocan."""
    def __init__(self, seed):
        self.sd = (seed * 13 + 50) & M32
        self.base = {}      # escrituras de preparacion (antes de la imagen de referencia)
        self.writes = {}    # escrituras del modelo (despues)
        self.snapped = False
        self.words = {}

    def fill_byte(self, ar, idx):
        w = (ar, idx // 4)
        if w not in self.words:
            self.words[w] = rand((self.sd + ar) & M32, idx // 4).to_bytes(4, 'little')
        return self.words[w][idx % 4]

    def ref_byte(self, ar, idx):
        return self.base[(ar, idx)] if (ar, idx) in self.base else self.fill_byte(ar, idx)

    def _put(self, ar, off, data):
        tgt = self.writes if self.snapped else self.base
        for j, b in enumerate(data):
            tgt[(ar, GUARD + off + j)] = b

    def put32(self, ar, off, v):
        self._put(ar, off, (v & M32).to_bytes(4, 'little'))

    def put16(self, ar, off, v):
        self._put(ar, off, (v & 0xFFFF).to_bytes(2, 'little'))

    def put8(self, ar, off, v):
        self._put(ar, off, bytes([v & 0xFF]))

    def snap(self):
        self.snapped = True

    def changes(self):
        out = []
        for (ar, idx) in sorted(self.writes):
            b = self.writes[(ar, idx)]
            if b != self.ref_byte(ar, idx):
                out.append([ar, idx - GUARD, b])
        return out


def feq(x, y):
    """igualdad ordenada de dos floats (fcomp + test ah,44h / jp): un NaN nunca es igual; +0 y -0 si"""
    a, b = load(x & M32), load(y & M32)
    if a == NAN or b == NAN:
        return False
    if a[0] == 'i' or b[0] == 'i':
        return a[0] == b[0] and a[1] == b[1]
    return a[1] == b[1]


def dep_of(part):
    p = part & M32
    return 10 if p > 17 else DEP_JUMP[DEP_BYTE[p]]


def expected_pool(c, img, res):
    fn, a = c['fn'], c['a']
    marks, ev = res['marks'], res['events']
    N = a[0]
    img.snap()
    ff = s32(a[1])
    flags = [a[2 + k] & 0xFF for k in range(16)]
    if fn == 12:
        wrapped = False
        found = None
        while True:
            ff += 1
            if ff == N:
                ff = 0
                if wrapped:
                    break
                wrapped = True
            if flags[ff] & 0x80:
                found = ff
                break
        if found is None:
            res['ret'] = 0
            marks.append('null')
        else:
            flags[ff] = ((flags[ff] & 0x7F) + 1) & 0x7F
            res['ret'] = tag(5, ff * 100)
            marks.append('found')
            if (a[2 + ff] & 0x7F) == 0x7F:
                marks.append('ref_wrap')
        if wrapped:
            marks.append('wrapped')
        if s32(a[1]) < 0:
            marks.append('first_free_neg')
    else:
        idx = a[18]
        flags[idx] |= 0x80
        if idx < ff:
            ff = idx
            marks.append('cursor_lowered')
        else:
            marks.append('cursor_kept')
        if a[19] == 0:
            marks.append('offset_zero')
        if a[19] == 99:
            marks.append('offset_last')
    changes = []
    old, new = a[1] & M32, ff & M32
    for j in range(4):
        if ((old >> (8 * j)) & 0xFF) != ((new >> (8 * j)) & 0xFF):
            changes.append([3, 0xC + j, (new >> (8 * j)) & 0xFF])
    for k in range(16):
        if flags[k] != (a[2 + k] & 0xFF):
            changes.append([4, k, flags[k]])
    res['changes'] = changes
    return res


def expected(c):
    fn, a = c['fn'], c['a']
    img = Img(c['seed'])
    marks, ev = [], []
    res = {'events': ev, 'marks': marks, 'changes': [], 'ret': None, 'al': None, 'st0': None}
    if fn <= 3:
        off = 0xC + 4 * fn
        img.put16(0, off, a[0]); img.put16(0, off + 2, a[1])
        img.snap()
        x, y = s16(a[0]), s16(a[1])
        img.put32(2, 0, f32bits(x * 0.25)); img.put32(2, 4, f32bits(y * 0.25))
        res['ret'] = tag(2, 0)
        marks.append('x_neg' if x < 0 else ('x_zero' if x == 0 else 'x_pos'))
        marks.append('y_neg' if y < 0 else ('y_zero' if y == 0 else 'y_pos'))
        if (a[0] & 0xFFFF) in (0x7FFF, 0x8000) or (a[1] & 0xFFFF) in (0x7FFF, 0x8000):
            marks.append('extreme')
    elif fn == 4:
        img.put32(0, 0xBC, a[0])
        img.snap()
        beta = load(a[0])
        pi, twopi, npi = load(PI_BITS), load(TWO_PI_BITS), load(NPI_BITS)
        if beta == NAN:
            marks.append('nan')
        elif gt(beta, pi):
            marks.append('above')
            img.put32(0, 0xBC, bits(sub(beta, twopi)))
        elif gt(npi, beta):
            marks.append('below')
            img.put32(0, 0xBC, bits(add(beta, twopi)))
        else:
            marks.append('inside')
        if beta != NAN and beta[0] == 'i':
            marks.append('inf')
        elif beta != NAN and beta[1] == 0:
            marks.append('zero')
        elif beta != NAN and abs(beta[1]) > 100:
            marks.append('big')
    elif fn == 5:
        slot = s32(a[2] & 0xFF if not (a[2] & 0x80) else (a[2] & 0xFF) | 0xFFFFFF00)
        img.put32(0, 0x21C, 0)
        img.put8(1, 0x36, a[1]); img.put8(1, 0x718, a[2]); img.put8(1, 0x5A0 + 0x14 + slot * 0x1C, a[3])
        img.snap()
        if a[0] == 1:
            res['al'] = 0
            marks.append('target_null')
        elif (a[1] & 7) != 3:
            res['al'] = 0
            marks.append('not_ped')
        else:
            res['al'] = 1 if (a[3] & 0xFF) else 0
            marks.append('flag_set' if res['al'] else 'flag_clear')
            if slot < 0:
                marks.append('slot_negative')
            if (a[3] & 0xFF) > 1:
                marks.append('flag_nonbool')
    elif fn == 6:
        img.snap()
        arg = a[14] & M32
        hit = any(arg == (a[k] & 0xFFFF) for k in range(14))
        res['al'] = 1 if hit else 0
        marks.append('match' if hit else 'nomatch')
        if arg & 0x80000000:
            marks.append('arg_negative')
        if any((a[k] & 0xFFFF) >= 0x8000 for k in range(14)):
            marks.append('word_high')
        if (not hit) and any(arg == s32_16(a[k]) for k in range(14)):
            marks.append('signext_trap')
    elif fn in (7, 8):
        img.snap()
        eq = all(feq(a[k], a[3 + k]) for k in range(3))
        res['al'] = (1 if eq else 0) if fn == 8 else (0 if eq else 1)
        marks.append('equal' if eq else 'differ')
        if any(load(a[k]) == NAN or load(a[3 + k]) == NAN for k in range(3)):
            marks.append('nan_involved')
    elif fn == 9:
        img.put8(0, 0x40, a[0]); img.put8(0, 0x0C, a[1])
        img.snap()
        hit = (a[0] & 0xFF) != 0 and (a[1] & 0xFF) != 0
        res['al'] = 1 if hit else 0
        marks.append('both' if hit else ('first_zero' if (a[0] & 0xFF) == 0 else 'second_zero'))
    elif fn in (10, 11):
        img.put32(0, 0x4C, a[0])
        img.snap()
        res['ret'] = a[0] & M32
        marks.append('null' if a[0] == 0 else 'nonnull')
    elif fn in (12, 13):
        return expected_pool(c, img, res)
    elif fn in (14, 15):
        part = s32(a[2])
        if fn == 14:
            img.put32(0, 0, a[3])
            img.snap()
            tex, mod = a[0] & M32, a[1] & M32
        else:
            chars = b'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_'

            def text(mode, ln, seed):
                if mode == 2:
                    return b''
                return bytes(chars[rand(seed, i) % len(chars)] for i in range(ln))
            st, sm = text(a[0], a[3], a[5]), text(a[1], a[4], a[6])
            img.snap()
            if a[0] == 1:
                tex = 0
            else:
                tex = fnv_key(st)
                ev.append([50, tag(6, 0), fnv(st)])
            if a[1] == 1:
                mod = 0
            else:
                mod = fnv_key(sm)
                ev.append([50, tag(6, 0x20), fnv(sm)])
        dep = dep_of(a[2])
        img.put32(0, 0x28 + 4 * part, tex)
        if dep != 10:
            img.put32(0, 4 * dep, mod)
            marks.append('dep_%d' % dep)
        else:
            marks.append('dep_none')
        marks.append('part_oob' if (part < 0 or part > 17) else 'part_ok')
        if fn == 15:
            marks.append('tex_null' if a[0] == 1 else ('tex_empty' if a[0] == 2 else 'tex_text'))
            marks.append('model_null' if a[1] == 1 else ('model_empty' if a[1] == 2 else 'model_text'))
    res['changes'] = img.changes()
    return res


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if o.get('al') != e['al']:
        return 'al %s != %s' % (o.get('al'), e['al'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
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
