"""Oraculo independiente (Python) de Fn89D: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x719B40..0x71A0DF, lo que debe observarse de CFont::PrintSegment
(cdecl: left, top, begin, end, spacing en pila; void) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.
La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas; la memoria del juego (0xC716A8..0xC71AD0) con una imagen de bytes.

Modelo (desensamblado):
  0x719B43: color guardado (dword 0xC71A60). Si la textura de estado (word 0xC71ACC) != textura de la fuente (byte 0xC71A94): RenderFontBuffer (0x719840) y word 0xC71ACC = byte 0xC71A94.
  color_previo = dword 0xC71A60 (tras el vaciado). Sombra = byte con signo 0xC71A96: si != 0 (0x719BA5): color = color de sombra (0xC71A97..9A: r en 60, g en 61, b en 62, a en 63), blip (0xC71A7E) = 1,
  sombra = 0; si la inclinacion (0xC71A6C) != 0 (ordenado): referencia += ((int(0xC17044) * 0.0015625f) * o, (int(0xC17048) * 0.0022321429f) * o) (guardada como float) con o = sombra;
  llamada recursiva (left' = (w*kx)*o + left, top' = (h*ky)*o + top); despues sombra = byte, color = color_previo, blip = 0.
  Si no (0x719CA3): contorno = byte con signo 0xC71A9B; si != 0: igual con 0xC71A9B = 0 y OCHO llamadas: (left+dx, top-dy), (left-dx, top-dy), (left+dx, top+dy), (left-dx, top+dy), (left+dx, top), (left-dx, top),
  (left, top+dy), (left, top-dy) con dx = (w*kx)*o y dy = (h*ky)*o; despues contorno = byte, color = color_previo, blip = 0.
  0x719F30: si pEmptyChar (0xC716A8) >= 0xC7187C - (end - begin) (sin signo): RenderFontBuffer. Cabecera de 0x30 bytes en pEmptyChar: +4 left, +8 top, +0xC escala x, +0x10 escala y, +0x14..17 color,
  +0x18 spacing, +0x1C inclinacion, +0x20/+0x24 referencia, +0x28 blip, +0x29 estilo, +0x2A proporcional, +0x2C textura (word), +0x2E contorno (el resto no se escribe); pEmptyChar += 0x30.
  Texto [begin, end): '~' llama a ParseToken (0x718F00: texto, &color (copia de 0xC71A60), byte 0xC71AC8, 0); si el blip (0xC71A7E) y el simbolo (0xC71A54) son 0: color r,g,b = los del color devuelto, los
  guardados r,g,b tambien y el alfa guardado = byte 0xC71A63; simbolo = 0; copia desde la etiqueta hasta el avance; los demas bytes se copian tal cual. Despues un 0, y pEmptyChar se alinea a 4.
  Si el blip es 0: color = guardado (r,g,b) y alfa guardado.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF
NAN = ('n',)
IMG_BASE, IMG_END, STAT_BASE, BUF_START, LIMIT = 0xC716A8, 0xC71AD0, 0xC71A54, 0xC716B0, 0xC7187C
IMG_SIZE = IMG_END - IMG_BASE
FILL = 0xA5
STACK = 0x7F000000


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def fromint(j):
    """fild dword (con signo): exacto"""
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jnp: igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


KX = load(0x3ACCCCCD)   # 0x859520 = 0.0015625f
KY = load(0x3B124925)   # 0x859524 = 0.0022321429f
GUARD = 0x40


def tag_text(off):
    return 0x10000000 + GUARD + off


class Model:
    def __init__(self, c):
        self.c = c
        self.mem = bytearray(IMG_SIZE)
        self.set32(0xC716A8, c['pe'] & M32)
        for i in range(4, STAT_BASE - IMG_BASE):
            self.mem[i] = FILL
        for i, w in enumerate(c['stat']):
            self.set32(STAT_BASE + 4 * i, w & M32)
        self.init = bytes(self.mem)
        self.maxw = c['maxw'] & M32
        self.maxh = c['maxh'] & M32
        self.text = bytes.fromhex(c['text'])
        self.events = []
        self.parse_calls = 0
        self.done = set()
        self.marks = set()
        self.flushes = 0
        self.depth = 0

    # --- memoria
    def idx(self, addr, n):
        i = addr - IMG_BASE
        if i < 0 or i + n > IMG_SIZE:
            raise RuntimeError('fuera de la imagen: %x' % addr)
        return i

    def u8(self, a):
        return self.mem[self.idx(a, 1)]

    def set8(self, a, v):
        self.mem[self.idx(a, 1)] = v & 0xFF

    def u16(self, a):
        i = self.idx(a, 2)
        return self.mem[i] | (self.mem[i + 1] << 8)

    def set16(self, a, v):
        i = self.idx(a, 2)
        self.mem[i] = v & 0xFF
        self.mem[i + 1] = (v >> 8) & 0xFF

    def d32(self, a):
        i = self.idx(a, 4)
        return int.from_bytes(self.mem[i:i + 4], 'little')

    def set32(self, a, v):
        i = a - IMG_BASE
        self.mem[i:i + 4] = (v & M32).to_bytes(4, 'little')

    def once(self, s):
        if self.c['mutsite'] != s or s in self.done:
            return False
        self.done.add(s)
        self.marks.add('mut%d' % s)
        return True

    # --- falsos
    def flush(self):
        c = self.c
        self.events.append([1, self.d32(0xC716A8)])
        self.flushes += 1
        mv = c['mutval'] & M32
        if self.once(1):
            self.set32(0xC71A60, mv)
        if self.once(2):
            self.set32(0xC71A97, mv)
        if self.once(3):
            self.set8(0xC71A96, mv)
        if self.once(4):
            self.set8(0xC71A9B, mv)
        if self.once(5):
            self.set32(0xC71A6C, mv)
        if self.once(6):
            self.maxw = mv
        self.set32(0xC716A8, BUF_START)

    def parse(self, src):
        c = self.c
        cur = self.d32(0xC71A60)
        self.events.append([2, tag_text(src), STACK, cur, self.u8(0xC71AC8), 0])
        k = min(self.parse_calls, 3)
        self.parse_calls += 1
        parsed = cur
        if c['tmode'][k] & 1:
            parsed = c['tcolor'][k] & M32
        if (c['tsym'][k] & M32) != 0xFF:
            self.set8(0xC71A54, c['tsym'][k])
        return src + c['tadv'][k], parsed

    # --- funcion
    def wh(self):
        return s32(self.maxw), s32(self.maxh)

    def offsets(self, o):
        w, h = self.wh()
        dx = mul(mul(fromint(w), KX), fromint(o))
        dy = mul(mul(fromint(h), KY), fromint(o))
        return dx, dy

    def set_effect(self, o):
        """0x719BA5..0x719C3B / 0x719CC5..0x719D5B: color de sombra, blip = 1 y desplazamiento de la referencia de la inclinacion"""
        slant = self.d32(0xC71A6C)
        self.set8(0xC71A62, self.u8(0xC71A99))
        self.set8(0xC71A60, self.u8(0xC71A97))
        self.set8(0xC71A7E, 1)
        self.set8(0xC71A61, self.u8(0xC71A98))
        self.set8(0xC71A63, self.u8(0xC71A9A))
        return not is_zero(load(slant))

    def shift_ref(self, o):
        dx, dy = self.offsets(o)
        self.set32(0xC71A70, bits(add(dx, load(self.d32(0xC71A70)))))
        self.set32(0xC71A74, bits(add(dy, load(self.d32(0xC71A74)))))

    def restore_tail(self, before):
        self.set8(0xC71A60, before)
        self.set8(0xC71A61, before >> 8)
        self.set8(0xC71A62, before >> 16)
        self.set8(0xC71A63, before >> 24)
        self.set8(0xC71A7E, 0)

    def segment(self, left, top, b, e, spacing):
        self.depth += 1
        saved = self.d32(0xC71A60)
        if self.u16(0xC71ACC) != self.u8(0xC71A94):
            self.marks.add('flush_tex')
            self.flush()
            self.set16(0xC71ACC, self.u8(0xC71A94))
        before = self.d32(0xC71A60)
        shadow = s8(self.u8(0xC71A96))
        if shadow != 0:
            self.marks.add('shadow')
            upd = self.set_effect(shadow)
            self.set8(0xC71A96, 0)
            if upd:
                self.marks.add('slant_upd')
                self.shift_ref(shadow)
            dx, dy = self.offsets(shadow)
            self.segment(bits(add(dx, load(left))), bits(add(dy, load(top))), b, e, spacing)
            self.set8(0xC71A96, shadow)
            self.restore_tail(before)
        else:
            outline = s8(self.u8(0xC71A9B))
            if outline != 0:
                self.marks.add('outline')
                upd = self.set_effect(outline)
                self.set8(0xC71A9B, 0)
                if upd:
                    self.marks.add('slant_upd')
                    self.shift_ref(outline)
                L, T = load(left), load(top)
                for k in range(8):
                    dx, dy = self.offsets(outline)
                    if k == 0:
                        nl, nt = bits(add(dx, L)), bits(sub(T, dy))
                    elif k == 1:
                        nl, nt = bits(sub(L, dx)), bits(sub(T, dy))
                    elif k == 2:
                        nl, nt = bits(add(dx, L)), bits(add(dy, T))
                    elif k == 3:
                        nl, nt = bits(sub(L, dx)), bits(add(dy, T))
                    elif k == 4:
                        nl, nt = bits(add(dx, L)), top
                    elif k == 5:
                        nl, nt = bits(sub(L, dx)), top
                    elif k == 6:
                        nl, nt = left, bits(add(dy, T))
                    else:
                        nl, nt = left, bits(sub(T, dy))
                    self.segment(nl, nt, b, e, spacing)
                self.set8(0xC71A9B, outline)
                self.restore_tail(before)
        self.body(left, top, b, e, spacing, saved)
        self.depth -= 1

    def body(self, left, top, b, e, spacing, saved):
        limit = (LIMIT - (e - b)) & M32
        if not (self.d32(0xC716A8) < limit):
            self.marks.add('flush_full')
            self.flush()
        pe = self.d32(0xC716A8)
        self.set32(pe + 4, left)
        self.set32(pe + 8, top)
        self.set32(pe + 0xC, self.d32(0xC71A64))
        self.set32(pe + 0x10, self.d32(0xC71A68))
        for j in range(4):
            self.set8(pe + 0x14 + j, self.u8(0xC71A60 + j))
        self.set32(pe + 0x18, spacing)
        self.set32(pe + 0x1C, self.d32(0xC71A6C))
        self.set32(pe + 0x20, self.d32(0xC71A70))
        self.set32(pe + 0x24, self.d32(0xC71A74))
        self.set8(pe + 0x29, self.u8(0xC71A95))
        self.set8(pe + 0x2A, self.u8(0xC71A7D))
        self.set16(pe + 0x2C, self.u8(0xC71A94))
        self.set8(pe + 0x28, self.u8(0xC71A7E))
        self.set8(pe + 0x2E, self.u8(0xC71A9C))
        pe += 0x30
        self.set32(0xC716A8, pe)
        sr, sg, sb, sa = saved & 0xFF, (saved >> 8) & 0xFF, (saved >> 16) & 0xFF, (saved >> 24) & 0xFF
        bl = sa
        src = b
        if b < e:
            while True:
                ch = self.text[src]
                if ch == 0x7E:
                    nxt, parsed = self.parse(src)
                    self.marks.add('parse')
                    if self.u8(0xC71A7E) == 0 and self.u8(0xC71A54) == 0:
                        self.marks.add('parse_color')
                        self.set8(0xC71A60, parsed)
                        self.set8(0xC71A61, parsed >> 8)
                        self.set8(0xC71A62, parsed >> 16)
                        sr, sg, sb = parsed & 0xFF, (parsed >> 8) & 0xFF, (parsed >> 16) & 0xFF
                        bl = self.u8(0xC71A63)
                    else:
                        self.marks.add('parse_skip')
                    self.set8(0xC71A54, 0)
                    ecx = self.d32(0xC716A8)
                    if src != nxt:
                        while src != nxt:
                            self.set8(ecx, self.text[src])
                            ecx += 1
                            src += 1
                        self.set32(0xC716A8, ecx)
                else:
                    ecx = self.d32(0xC716A8)
                    self.set8(ecx, ch)
                    self.set32(0xC716A8, ecx + 1)
                    src += 1
                if not (src < e):
                    break
        ecx = self.d32(0xC716A8)
        self.set8(ecx, 0)
        ecx += 1
        self.set32(0xC716A8, ecx)
        if ecx & 3:
            while True:
                ecx += 1
                if not (ecx & 3):
                    break
            self.set32(0xC716A8, ecx)
        if self.u8(0xC71A7E) == 0:
            self.marks.add('restore_color')
            self.set8(0xC71A60, sr)
            self.set8(0xC71A61, sg)
            self.set8(0xC71A62, sb)
            self.set8(0xC71A63, bl)
        else:
            self.marks.add('blip_end')


def runs(init, final):
    out = []
    i = 0
    while i < IMG_SIZE:
        if final[i] == init[i]:
            i += 1
            continue
        j = i
        while j < IMG_SIZE and final[j] != init[j]:
            j += 1
        out.append([i, bytes(final[i:j]).hex()])
        i = j
    return out


def expected(c):
    m = Model(c)
    length = s32(c['len'])
    b = c['boff']
    e = b + length
    if length == 0:
        m.marks.add('empty_text')
    if length < 0:
        m.marks.add('neg_len')
    if abs(s32(c['maxw'])) > (1 << 24) or abs(s32(c['maxh'])) > (1 << 24):
        m.marks.add('big_int')
    m.segment(c['left'] & M32, c['top'] & M32, b, e, c['spacing'] & M32)
    if m.depth != 0:
        raise RuntimeError('profundidad')
    marks = sorted(m.marks)
    if 'shadow' in m.marks and 'outline' in m.marks:
        marks.append('both')
    if not m.flushes:
        marks.append('noflush')
    return {'image': runs(m.init, m.mem), 'text_changes': [], 'events': m.events, 'marks': marks}


CHANGE_KEYS = ('image', 'text_changes')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:600], str(e[k])[:600])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
