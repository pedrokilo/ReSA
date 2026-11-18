"""Oraculo independiente (Python) de Fn234A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 10 funciones del lote 234A (jdmarker y jdmaster de la
libjpeg 6b de descompresion embebida) y lo compara con las filas de comparisons.jsonl (lado original y candidato). Sigue la semantica de la libjpeg 6b (jdmarker.c y jdmaster.c) y las notas del informe del lote;
no usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas, el contenido de las tablas y del flujo y la escena (Fixtures.hpp / LiveComparison.hpp del banco)
para conocer la imagen inicial.

Escena (arena A, base S = arena A + 0x40 + P_OFF): jpeg_decompress_struct en S+0 (0x1B0 bytes), err S+0x1C0, mem S+0x200, src S+0x240, inputctl S+0x260, marker S+0x280, entropy S+0x330, coef S+0x3E0, upsample S+0x3F0,
6 componentes de 0x54 bytes en S+0x400, master S+0x600, progress S+0x620, idct S+0x640, cconvert S+0x650, cquantize S+0x660, quantizer_1pass S+0x670, quantizer_2pass S+0x680, post S+0x690. Arena B (B0): main 0x00,
asignador (bump) 0x400..0x1C00. Arena D (D0): tablas Huffman (0x120 de separacion). Arena C: flujo de entrada (cabecera estructurada segun la funcion y relleno pseudoaleatorio).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["LibJpegDecompress::get_dht", "LibJpegDecompress::get_dri", "LibJpegDecompress::skip_variable", "LibJpegDecompress::next_marker", "LibJpegDecompress::first_marker",
         "LibJpegDecompress::read_restart_marker", "LibJpegDecompress::jinit_marker_reader", "LibJpegDecompress::jpeg_calc_output_dimensions", "LibJpegDecompress::prepare_for_output_pass",
         "LibJpegDecompress::jinit_master_decompress"]
ENTRIES = (0x5CB5B0, 0x5CBB20, 0x5CC050, 0x5CC0F0, 0x5CC1F0, 0x5CC610, 0x5CC7D0, 0x5CC8C0, 0x5CCB50, 0x5CCCC0)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

_P = ("P_OFF P_KIND P_FILL P_ERRMODE P_BOFF P_DOFF P_COFF P_NOISE A0 A1 A2 A3 S_MODE S_SEED S_MASK IN_OFF IN_LEN "
      "CH_OFF0 CH_OFF1 CH_OFF2 CH_OFF3 CH_OFF4 CH_OFF5 CH_OFF6 CH_OFF7 CH_LEN0 CH_LEN1 CH_LEN2 CH_LEN3 CH_LEN4 CH_LEN5 CH_LEN6 CH_LEN7 "
      "F_GSTATE F_WIDTH F_HEIGHT F_NCOMP F_JCS F_OUTCS F_BUFIMG F_RAW F_QUANT F_TWOPASS F_EN1 F_ENX F_EN2 F_COLORMAP F_PROG F_ARITH F_MINDCT F_UNREAD F_PROGRESS F_PRCOMP F_PRTOTAL "
      "IC_MULTI IC_EOI MK_NEXT MK_DISC UP_CTX RS_RET MS_DUMMY MS_PASS MS_MERGED DCMASK ACMASK "
      "DHT_NT DHT_IDX0 DHT_IDX1 DHT_IDX2 DHT_IDX3 DHT_DESC0 DHT_DESC1 DHT_DESC2 DHT_DESC3 DHT_LENADJ "
      "HDESC0 HDESC1 HDESC2 HDESC3 HDESC4 HDESC5 HDESC6 HDESC7").split()
IDX = {name: i for i, name in enumerate(_P)}
COMP0 = len(_P)
SEED0 = COMP0 + 24
P_COUNT = SEED0 + 4
globals().update(IDX)
CF_H, CF_V, CF_WIB, CF_DCT = range(4)
K_CI, K_ERR, K_MEM, K_SRC, K_INP, K_MRK, K_ENT, K_COEF, K_UP, K_COMPS = 0x000, 0x1C0, 0x200, 0x240, 0x260, 0x280, 0x330, 0x3E0, 0x3F0, 0x400
K_MASTER, K_PROGRESS, K_IDCT, K_CCONV, K_Q0, K_Q1, K_Q2, K_POST = 0x600, 0x620, 0x640, 0x650, 0x660, 0x670, 0x680, 0x690
CI_SIZE = 0x1B0
MAX_COMPS = 6
B_MAIN, B_ALLOC, B_ALLOC_END = 0x00, 0x400, 0x1C00
H_STRIDE = 0x120
STREAM_SIZE = 0x540


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def idiv(a, b):
    """idiv con signo de 32 bits (trunca hacia cero); a, b ya con signo"""
    q = abs(a) // abs(b)
    if (a < 0) != (b < 0):
        q = -q
    return q



def make_bits(desc):
    mode, seed = desc & 7, desc >> 3
    bits = [0] * 17
    bits[0] = rnd(seed, 99) & 0xFF
    if mode == 2:
        dc = [0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0]
        ac = [0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7D]
        for l in range(1, 17):
            bits[l] = dc[l] if seed & 1 else ac[l]
    elif mode == 4:
        pass
    elif mode == 5:
        bits[16] = 255
        bits[15] = 2
    elif mode == 6:
        bits[1 + seed % 16] = 1
    elif mode == 3:
        lv = 1 + rnd(seed, 50) % 8
        free = 1
        for l in range(1, 17):
            free *= 2
            if l < lv:
                c = min(rnd(seed, l) % 3, free & M32)
                bits[l] = c
                free -= c
            elif l == lv:
                bits[l] = min(255, ((free & M32) + 1 + rnd(seed, 51) % 2) & M32)
                free -= bits[l]
            else:
                bits[l] = 0
    else:
        maxc = 3 if mode == 0 else (12 if mode == 1 else 1 + rnd(seed, 60) % 40)
        free = 1
        total = 0
        for l in range(1, 17):
            free = min(free * 2, 100000)
            cap = min(maxc, free)
            c = rnd(seed, l) % (cap + 1)
            if total + c > 256:
                c = 256 - total
            bits[l] = c
            total += c
            free -= c
    return bits



class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios); `lazy` = rangos cuyo
    contenido se calcula al leer (las tablas, el flujo y los arrays de punteros)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}
        self.lazy = []

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def initial(self, i):
        for lo, hi, fn in self.lazy:
            if lo <= i < hi:
                return fn(i - lo)
        return self.base(i)

    def get(self, i):
        v = self.m.get(i)
        return self.initial(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)

    def changes(self):
        """cambios en tramos contiguos: [[desplazamiento del primer byte, bytes nuevos en hexadecimal]]"""
        diff = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.initial(i)
            if self.m[i] != ref:
                diff.append(i)
        out = []
        k = 0
        while k < len(diff):
            j = k
            while j + 1 < len(diff) and diff[j + 1] == diff[j] + 1:
                j += 1
            out.append([diff[k] - GUARD, ''.join('%02x' % self.m[i] for i in diff[k:j + 1])])
            k = j + 1
        return out


class World:
    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[P_FILL], a[SEED0 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()



def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


class Escape(Exception):
    pass


def jdiv(a, b):
    """0x5CF890: (a + b - 1) / b con idiv con signo (trunca hacia cero)"""
    a, b = s32(a), s32(b)
    return idiv(s32(a + b - 1), b) & M32





def huff_val(desc, k):
    r = rnd(desc >> 3, 200 + k)
    if ((desc >> 3) & 4) == 0:
        return r & 0xFF
    return 16 + ((r >> 8) % 240) if r % 40 == 0 else (r >> 8) & 15


def stream_byte(mode, seed, k):
    r = rnd(seed, k)
    b = (r >> 8) & 0xFF
    if mode == 0:
        return r & 0xFF
    if mode == 1:
        t = r % 16
        if t < 4:
            return 0xFF
        if t == 4:
            return 0x00
        if t == 5:
            return 0xD9
        if t == 6:
            return 0xD0 + (b & 7)
        return b
    if mode == 2:
        return 0xFF if r % 4 == 0 else (0x00 if r % 8 == 1 else b)
    if mode == 3:
        return 0xFE if (r & 0xFF) == 0xFF else (r & 0xFF)
    return 0xFF


def stream_head(fn, a):
    """cabecera estructurada del flujo de entrada segun la funcion (el resto es stream_byte)"""
    h = []

    def b2(v):
        h.append((v >> 8) & 0xFF)
        h.append(v & 0xFF)
    if fn == 0:
        nt = min(a[DHT_NT], 4)
        total = 2
        for t in range(nt):
            total += 17 + sum(make_bits(a[DHT_DESC0 + t])[1:])
        b2((total + a[DHT_LENADJ]) & M32)
        for t in range(nt):
            bits = make_bits(a[DHT_DESC0 + t])
            h.append(a[DHT_IDX0 + t] & 0xFF)
            count = 0
            for l in range(1, 17):
                h.append(bits[l] & 0xFF)
                count += bits[l]
            for k in range(count):
                h.append(huff_val(a[DHT_DESC0 + t], k) & 0xFF)
    elif fn == 1:
        b2(a[A0])
        b2(a[A1])
    elif fn == 2:
        b2(a[A0])
    elif fn == 4:
        h.append(a[A0] & 0xFF)
        h.append(a[A1] & 0xFF)
    return h[:STREAM_SIZE]


class Fail(Exception):
    """fill_input_buffer devolvio falso: la funcion sale con FALSE sin sincronizar la fuente"""


class Sim:
    def __init__(self, c):
        self.a = [x & M32 for x in c['a']]
        a = self.a
        self.c = c
        self.fn = c['fn']
        self.seed = c['seed'] & M32
        W = self.W = World(c)
        self.events = []
        self.marks = set()
        self.S = W.raw(0, a[P_OFF])
        self.D0 = W.raw(3, a[P_DOFF])
        self.C0 = W.raw(2, a[P_COFF])
        self.B0 = W.raw(1, a[P_BOFF])
        self.errmode = a[P_ERRMODE]
        self.used = 0
        self.dummy = 0
        self.sarray = 0
        self.fills = 0
        self.build_scene()
        W.snap()

    # ---------------------------------------------------------------- escena
    def build_scene(self):
        a, W, S, D0, C0, B0 = self.a, self.W, self.S, self.D0, self.C0, self.B0
        wr = W.wr
        fn = self.fn
        dmem = W.mem[3]
        d0off = a[P_DOFF]
        for j in range(8):
            lo = GUARD + d0off + H_STRIDE * j
            bits = make_bits(a[HDESC0 + j])
            desc = a[HDESC0 + j]
            dmem.lazy.append((lo, lo + 0x111, lambda t, bits=bits, desc=desc: bits[t] if t <= 16 else huff_val(desc, t - 0x11)))
        for i in range(MAX_COMPS):
            p = S + K_COMPS + 0x54 * i
            k = COMP0 + 4 * i
            wr(p + 0x08, 4, a[k + CF_H]); wr(p + 0x0C, 4, a[k + CF_V]); wr(p + 0x1C, 4, a[k + CF_WIB]); wr(p + 0x24, 4, a[k + CF_DCT])
        # los punteros a los falsos (err, mem, src, inputctl, coef, upsample, idct, cconvert, quantizers, post, main) son direcciones del ASI del banco: no entran en los resumenes FNV ni en los cambios,
        # asi que no se modelan
        wr(S + K_SRC + 0x00, 4, (C0 + a[IN_OFF]) & M32); wr(S + K_SRC + 0x04, 4, a[IN_LEN])
        wr(S + K_INP + 0x10, 1, a[IC_MULTI]); wr(S + K_INP + 0x11, 1, a[IC_EOI])
        wr(S + K_MRK + 0x10, 4, a[MK_NEXT]); wr(S + K_MRK + 0x14, 4, a[MK_DISC])
        wr(S + K_UP + 0x08, 1, a[UP_CTX])
        wr(S + K_MASTER + 0x08, 1, a[MS_DUMMY]); wr(S + K_MASTER + 0x0C, 4, a[MS_PASS]); wr(S + K_MASTER + 0x10, 1, a[MS_MERGED]); wr(S + K_MASTER + 0x14, 4, S + K_Q1); wr(S + K_MASTER + 0x18, 4, S + K_Q2)
        wr(S + K_PROGRESS + 0x0C, 4, a[F_PRCOMP]); wr(S + K_PROGRESS + 0x10, 4, a[F_PRTOTAL])
        ci = S + K_CI
        wr(ci + 0x00, 4, S + K_ERR); wr(ci + 0x04, 4, S + K_MEM); wr(ci + 0x08, 4, (S + K_PROGRESS) & M32 if a[F_PROGRESS] else 0); wr(ci + 0x14, 4, a[F_GSTATE]); wr(ci + 0x18, 4, S + K_SRC)
        wr(ci + 0x1C, 4, a[F_WIDTH]); wr(ci + 0x20, 4, a[F_HEIGHT]); wr(ci + 0x24, 4, a[F_NCOMP]); wr(ci + 0x28, 4, a[F_JCS]); wr(ci + 0x2C, 4, a[F_OUTCS])
        wr(ci + 0x40, 1, a[F_BUFIMG]); wr(ci + 0x41, 1, a[F_RAW]); wr(ci + 0x4A, 1, a[F_QUANT]); wr(ci + 0x50, 1, a[F_TWOPASS]); wr(ci + 0x58, 1, a[F_EN1]); wr(ci + 0x59, 1, a[F_ENX]); wr(ci + 0x5A, 1, a[F_EN2])
        wr(ci + 0x74, 4, a[F_COLORMAP])
        for i in range(4):
            wr(ci + 0xA0 + 4 * i, 4, 0 if (a[DCMASK] >> i) & 1 else D0 + H_STRIDE * i)
            wr(ci + 0xB0 + 4 * i, 4, 0 if (a[ACMASK] >> i) & 1 else D0 + H_STRIDE * (4 + i))
        wr(ci + 0xC4, 4, S + K_COMPS); wr(ci + 0xC8, 1, a[F_PROG]); wr(ci + 0xC9, 1, a[F_ARITH])
        wr(ci + 0x118, 4, a[F_MINDCT]); wr(ci + 0x17C, 4, a[F_UNREAD])
        wr(ci + 0x180, 4, S + K_MASTER); wr(ci + 0x184, 4, B0 + B_MAIN); wr(ci + 0x188, 4, S + K_COEF); wr(ci + 0x18C, 4, S + K_POST); wr(ci + 0x190, 4, S + K_INP); wr(ci + 0x194, 4, S + K_MRK); wr(ci + 0x198, 4, S + K_ENT)
        wr(ci + 0x19C, 4, S + K_IDCT); wr(ci + 0x1A0, 4, S + K_UP); wr(ci + 0x1A4, 4, S + K_CCONV); wr(ci + 0x1A8, 4, S + K_Q0)
        self.ci = ci
        head = stream_head(fn, a)
        lo = GUARD + a[P_COFF]
        smode, sseed = a[S_MODE], a[S_SEED]
        W.mem[2].lazy.append((lo, lo + STREAM_SIZE, lambda t, head=head: head[t] if t < len(head) else stream_byte(smode, sseed, t)))

    # ---------------------------------------------------------------- memoria
    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def tag(self, v):
        return self.W.tagv(v)

    def master_hash(self):
        m = self.rd(self.ci + 0x180)
        t = self.W.tagv(m)
        if 0x10000000 <= t < 0x10400000:
            return fnv(bytes(self.rd(m + i, 1) for i in range(0x1C)))
        return 0

    def ci_hash(self):
        return fnv(bytes(self.rd(self.ci + i, 1) for i in range(CI_SIZE)))

    # ---------------------------------------------------------------- falsos
    def err_exit(self):
        err = self.rd(self.ci)
        self.events.append([1, self.rd(err + 0x14), self.rd(err + 0x18), self.rd(err + 0x1C), self.tag(self.ci)])
        self.marks.add('err_exit')
        if self.errmode == 0:
            self.marks.add('escape')
            raise Escape()
        self.marks.add('err_return')

    def errx(self, code, p0=None, p1=None):
        err = self.rd(self.ci)
        self.wr(err + 0x14, 4, code)
        if p0 is not None:
            self.wr(err + 0x18, 4, p0 & M32)
        if p1 is not None:
            self.wr(err + 0x1C, 4, p1 & M32)
        self.err_exit()

    def trace(self, code, level, parms):
        """TRACEMS*/WARNMS*: msg_code y msg_parm.i[k], luego emit_message(cinfo, level)"""
        err = self.rd(self.ci)
        self.wr(err + 0x14, 4, code)
        for k, v in enumerate(parms):
            self.wr(err + 0x18 + 4 * k, 4, v & M32)
        self.events.append([2, level & M32, self.rd(err + 0x14), self.tag(self.ci)] + [self.rd(err + 0x18 + 4 * k) for k in range(8)])

    def alloc(self, pool, size):
        size &= M32
        self.events.append([4, pool & M32, size, self.tag(self.ci)])
        need = (size + 7) & ~7
        if self.used + need <= B_ALLOC_END - B_ALLOC:
            p = (self.B0 + B_ALLOC + self.used) & M32
            self.used += need
            return p
        self.marks.add('alloc_dummy')
        p = (0x60000000 + 0x100 * self.dummy) & M32
        self.dummy += 1
        return p

    def alloc_sarray(self, pool, spr, rows):
        self.events.append([8, pool & M32, spr & M32, rows & M32, self.tag(self.ci)])
        p = (0x70000000 + 0x100 * self.sarray) & M32
        self.sarray += 1
        return p

    def fill_cb(self):
        a = self.a
        src = self.rd(self.ci + 0x18)
        idx = self.fills
        self.fills += 1
        self.events.append([7, idx, self.tag(self.rd(src)), self.rd(src + 4), self.tag(self.ci)])
        ok = idx < 8 and (a[S_MASK] >> idx) & 1
        if ok:
            self.wr(src, 4, (self.C0 + a[CH_OFF0 + idx]) & M32)
            self.wr(src + 4, 4, a[CH_LEN0 + idx])
        return bool(ok)

    def skip_cb(self, n):
        src = self.rd(self.ci + 0x18)
        self.events.append([6, n & M32, self.tag(self.ci), self.tag(self.rd(src))])
        self.wr(src, 4, (self.rd(src) + n) & M32)
        self.wr(src + 4, 4, (self.rd(src + 4) - n) & M32)

    def resync_cb(self, desired):
        marker = self.rd(self.ci + 0x194)
        self.events.append([5, desired & M32, self.tag(self.ci), self.rd(self.ci + 0x17C)])
        self.wr(self.ci + 0x17C, 4, (0x5A5A0000 + desired) & M32)
        self.wr(marker + 0x10, 4, (self.rd(marker + 0x10) + 1) & M32)
        return self.a[RS_RET] & 0xFF != 0

    def mod1(self, ident):
        self.events.append([ident, self.tag(self.ci), self.ci_hash(), self.master_hash()])

    def mod2(self, ident, arg):
        self.events.append([ident, self.tag(self.ci), arg & M32, self.ci_hash(), self.master_hash()])

    # ---------------------------------------------------------------- entrada (INPUT_VARS / INPUT_BYTE / INPUT_SYNC de jdmarker)
    def inp_open(self):
        self.src = self.rd(self.ci + 0x18)
        self.next = self.rd(self.src)
        self.bytes = self.rd(self.src + 4)

    def inp_byte(self):
        if self.bytes == 0:
            self.marks.add('fill_called')
            if not self.fill_cb():
                self.marks.add('fill_fail')
                raise Fail()
            self.next = self.rd(self.src)
            self.bytes = self.rd(self.src + 4)
        v = self.rd(self.next, 1)
        self.bytes = (self.bytes - 1) & M32
        self.next = (self.next + 1) & M32
        return v

    def inp_2bytes(self):
        hi = self.inp_byte()
        lo = self.inp_byte()
        return (hi << 8) + lo

    def inp_sync(self):
        self.wr(self.src, 4, self.next)
        self.wr(self.src + 4, 4, self.bytes)

    # ---------------------------------------------------------------- jdmarker
    def get_dht(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        self.inp_open()
        length = self.inp_2bytes() - 2
        ntab = 0
        while length > 16:
            index = self.inp_byte()
            self.trace(0x50, 1, [index])
            bits = [0] * 17
            count = 0
            for i in range(1, 17):
                bits[i] = self.inp_byte()
                count += bits[i]
            length -= 17
            self.trace(0x56, 2, bits[1:9])
            self.trace(0x56, 2, bits[9:17])
            if count > 256 or count > length:
                self.marks.add('over256' if count > 256 else 'len_err')
                self.errx(8)
            hv = [self.inp_byte() for _ in range(count)]
            length -= count
            tbl = index
            if index & 0x10:
                tbl -= 0x10
                slot = ci + 0xB0 + 4 * tbl
                self.marks.add('ac_idx')
            else:
                slot = ci + 0xA0 + 4 * tbl
                self.marks.add('dc_idx')
            if tbl < 0 or tbl >= 4:
                self.marks.add('bad_idx')
                self.errx(0x1E, tbl)
            if rd(slot) == 0:
                self.marks.add('new_table')
                p = self.alloc(0, 0x112)
                wr(p + 0x111, 1, 0)
                wr(slot, 4, p)
            else:
                self.marks.add('old_table')
            htbl = rd(slot)
            for i in range(17):
                wr(htbl + i, 1, bits[i])
            for i in range(count):
                wr(htbl + 0x11 + i, 1, hv[i])
            if count == 0:
                self.marks.add('count0')
            ntab += 1
        if ntab >= 2:
            self.marks.add('multi_table')
        if length != 0:
            self.marks.add('length_bad')
            self.errx(0x0B)
        self.inp_sync()
        self.marks.add('ok')
        return True

    def get_dri(self):
        self.inp_open()
        length = self.inp_2bytes()
        if length != 4:
            self.marks.add('len_bad')
            self.errx(0x0B)
        else:
            self.marks.add('len4')
        tmp = self.inp_2bytes()
        self.trace(0x52, 1, [tmp])
        self.wr(self.ci + 0xFC, 4, tmp)
        self.inp_sync()
        self.marks.add('ok')
        return True

    def skip_variable(self):
        self.inp_open()
        length = s32(self.inp_2bytes() - 2)
        self.trace(0x5B, 1, [self.rd(self.ci + 0x17C), length])
        self.inp_sync()
        if length > 0:
            self.marks.add('skip')
            self.skip_cb(length)
        else:
            self.marks.add('no_skip')
        return True

    def next_marker(self):
        ci = self.ci
        self.inp_open()
        marker = self.rd(ci + 0x194)
        while True:
            c = self.inp_byte()
            while c != 0xFF:
                self.marks.add('discard')
                self.wr(marker + 0x14, 4, (self.rd(marker + 0x14) + 1) & M32)
                self.inp_sync()
                c = self.inp_byte()
            while True:
                c = self.inp_byte()
                if c != 0xFF:
                    break
                self.marks.add('ff_run')
            if c != 0:
                break
            self.marks.add('ff00')
            self.wr(marker + 0x14, 4, (self.rd(marker + 0x14) + 2) & M32)
            self.inp_sync()
        if self.rd(marker + 0x14) != 0:
            self.marks.add('warn')
            self.trace(0x74, -1, [self.rd(marker + 0x14), c])
            self.wr(marker + 0x14, 4, 0)
        self.wr(ci + 0x17C, 4, c)
        self.inp_sync()
        self.marks.add('found')
        return True

    def first_marker(self):
        self.inp_open()
        c = self.inp_byte()
        c2 = self.inp_byte()
        if c != 0xFF or c2 != 0xD8:
            self.marks.add('not_soi')
            self.errx(0x35, c, c2)
        else:
            self.marks.add('soi_ok')
        self.wr(self.ci + 0x17C, 4, c2)
        self.inp_sync()
        return True

    def read_restart_marker(self):
        ci = self.ci
        marker = self.rd(ci + 0x194)
        if self.rd(ci + 0x17C) == 0:
            self.marks.add('unread0')
            try:
                self.next_marker()
            except Fail:
                self.marks.add('nm_fail')
                return False
        n = self.rd(marker + 0x10)
        if self.rd(ci + 0x17C) == (n + 0xD0) & M32:
            self.marks.add('match')
            self.trace(0x62, 3, [self.rd(marker + 0x10)])
            self.wr(ci + 0x17C, 4, 0)
        else:
            self.marks.add('resync')
            if not self.resync_cb(n):
                self.marks.add('resync_fail')
                return False
        self.wr(marker + 0x10, 4, (self.rd(marker + 0x10) + 1) & 7)
        self.marks.add('ok')
        return True

    def jinit_marker_reader(self):
        ci = self.ci
        wr = self.wr
        m = self.alloc(0, 0xA8)
        wr(ci + 0x194, 4, m)
        wr(m + 0x00, 4, 0x5CC7A0)
        wr(m + 0x04, 4, 0x5CC290)
        wr(m + 0x08, 4, 0x5CC610)
        wr(m + 0x18, 4, 0x5CC050)
        wr(m + 0x5C, 4, 0)
        for i in range(16):
            wr(m + 0x1C + 4 * i, 4, 0x5CC050)
            wr(m + 0x60 + 4 * i, 4, 0)
        wr(m + 0x1C, 4, 0x5CBF00)
        wr(m + 0x54, 4, 0x5CBF00)
        wr(ci + 0xC4, 4, 0)
        wr(ci + 0x7C, 4, 0)
        wr(ci + 0x17C, 4, 0)
        wr(m + 0x0C, 1, 0)
        wr(m + 0x0D, 1, 0)
        wr(m + 0x14, 4, 0)
        wr(m + 0xA0, 4, 0)

    # ---------------------------------------------------------------- jdmaster
    def calc_output_dimensions(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        gs = rd(ci + 0x14)
        if gs != 0xCA:
            self.marks.add('bad_state')
            self.errx(0x14, gs)
        wr(ci + 0x5C, 4, rd(ci + 0x1C))
        wr(ci + 0x60, 4, rd(ci + 0x20))
        cs = rd(ci + 0x2C)
        self.marks.add('cs%d' % cs if cs <= 5 else 'cs_default')
        comps = {1: 1, 2: 4, 3: 3, 4: 4, 5: 4}.get(cs, rd(ci + 0x24))
        wr(ci + 0x64, 4, comps)
        wr(ci + 0x6C, 4, 1)
        if rd(ci + 0x4A, 1):
            self.marks.add('quant_on')
            wr(ci + 0x68, 4, 1)
        else:
            wr(ci + 0x68, 4, rd(ci + 0x64))

    def prepare_for_output_pass(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        master = rd(ci + 0x180)
        if rd(master + 8, 1):
            self.marks.add('dummy')
            self.errx(0x30)
        else:
            if rd(ci + 0x4A, 1) and rd(ci + 0x74) == 0:
                if rd(ci + 0x50, 1) and rd(ci + 0x5A, 1):
                    self.marks.add('q2')
                    wr(ci + 0x1A8, 4, rd(master + 0x18))
                    wr(master + 8, 1, 1)
                elif rd(ci + 0x58, 1):
                    self.marks.add('q1')
                    wr(ci + 0x1A8, 4, rd(master + 0x14))
                else:
                    self.marks.add('mode_change')
                    self.errx(0x2E)
            elif rd(ci + 0x4A, 1):
                self.marks.add('quant_cmap')
            self.mod1(30)       # idct->start_pass
            self.mod1(38)       # coef->start_output_pass
            if not rd(ci + 0x41, 1):
                if not rd(master + 0x10, 1):
                    self.marks.add('cconv')
                    self.mod1(31)
                else:
                    self.marks.add('merged')
                self.mod1(32)   # upsample->start_pass
                if rd(ci + 0x4A, 1):
                    q = rd(ci + 0x1A8)
                    ident = {self.S + K_Q0: 33, self.S + K_Q1: 34, self.S + K_Q2: 35}[q]
                    self.mod2(ident, rd(master + 8, 1))
                self.mod2(36, 3 if rd(master + 8, 1) else 0)   # post->start_pass
                self.mod2(37, 0)                               # main->start_pass
            else:
                self.marks.add('raw')
        progress = rd(ci + 0x08)
        if progress:
            self.marks.add('progress')
            wr(progress + 0x0C, 4, rd(master + 0x0C))
            wr(progress + 0x10, 4, (rd(master + 0x0C) + (2 if rd(master + 8, 1) else 1)) & M32)
            if rd(ci + 0x40, 1) and not rd(rd(ci + 0x190) + 0x11, 1):
                self.marks.add('buffered_noeoi')
                wr(progress + 0x10, 4, (rd(progress + 0x10) + (2 if rd(ci + 0x5A, 1) else 1)) & M32)

    def jinit_huff_decoder(self):
        ci = self.ci
        wr = self.wr
        e = self.alloc(1, 0xAC)
        wr(ci + 0x198, 4, e)
        wr(e + 0x00, 4, 0x5CA110)
        wr(e + 0x04, 4, 0x5C9CE0)
        for i in range(4):
            wr(e + 0x38 + 4 * i, 4, 0)
            wr(e + 0x28 + 4 * i, 4, 0)

    def main_controller(self):
        """jinit_d_main_controller(cinfo, FALSE) real (0x5CAE30)"""
        ci = self.ci
        rd, wr = self.rd, self.wr
        main = self.alloc(1, 0x38)
        wr(ci + 0x184, 4, main)
        wr(main, 4, 0x5CADC0)
        M = s32(rd(ci + 0x118))
        up = rd(ci + 0x1A0)
        if rd(up + 8, 1) != 0:
            self.marks.add('ctx')
            if M < 2:
                self.marks.add('notimpl_main')
                self.errx(0x2F)
            nc = s32(rd(ci + 0x24))
            xb = self.alloc(1, (nc << 3) & M32)
            wr(main + 0x20, 4, xb)
            wr(main + 0x24, 4, (xb + nc * 4) & M32)
            comp = rd(ci + 0xC4)
            i = 0
            while i < s32(rd(ci + 0x24)):
                c = comp + 0x54 * i
                rg = idiv(s32((rd(c + 0x0C) * rd(c + 0x24)) & M32), M)
                span = s32(((M + 4) * rg) & M32)
                blk = self.alloc(1, (span * 8) & M32)
                x0 = (blk + rg * 4) & M32
                wr(rd(main + 0x20) + 4 * i, 4, x0)
                wr(rd(main + 0x24) + 4 * i, 4, (x0 + span * 4) & M32)
                i += 1
            ngroups = (M + 2) & M32
        else:
            self.marks.add('no_ctx')
            ngroups = M & M32
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = rd(ci + 0xC4) + 0x54 * i
            dct = rd(c + 0x24)
            rg = idiv(s32((rd(c + 0x0C) * dct) & M32), M)
            h = (rg * s32(ngroups)) & M32
            w = (rd(c + 0x1C) * dct) & M32
            r = self.alloc_sarray(1, w, h)
            wr(main + 8 + 4 * i, 4, r)
            i += 1

    def jinit_master_decompress(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        master = self.alloc(1, 0x1C)
        wr(ci + 0x180, 4, master)
        wr(master + 0, 4, 0x5CCB50)
        wr(master + 4, 4, 0x5CCC90)
        wr(master + 8, 1, 0)
        # master_selection
        self.calc_output_dimensions()
        block = self.alloc(1, 0x580)
        table = (block + 0x100) & M32
        wr(ci + 0x120, 4, table)
        for i in range(0x100):
            wr(block + i, 1, 0)
        for i in range(0x100):
            wr(table + i, 1, i)
        for i in range(0x180):
            wr(table + 0x80 + 0x80 + i, 1, 0xFF)
        for i in range(0x180):
            wr(table + 0x80 + 0x200 + i, 1, 0)
        for i in range(0x80):
            wr(table + 0x80 + 0x380 + i, 1, rd(table + i, 1))
        wr(master + 0x0C, 4, 0)
        wr(master + 0x10, 1, 0)
        wr(master + 0x14, 4, 0)
        wr(master + 0x18, 4, 0)
        quant = rd(ci + 0x4A, 1)
        if not quant or not rd(ci + 0x40, 1):
            wr(ci + 0x58, 1, 0)
            wr(ci + 0x59, 1, 0)
            wr(ci + 0x5A, 1, 0)
        if quant:
            self.marks.add('quant')
            if rd(ci + 0x41, 1):
                self.marks.add('raw_quant')
                self.errx(0x2F)
            if rd(ci + 0x64) != 3:
                self.marks.add('q_not3')
                wr(ci + 0x59, 1, 0)
                wr(ci + 0x5A, 1, 0)
                wr(ci + 0x74, 4, 0)
                wr(ci + 0x58, 1, 1)
            elif rd(ci + 0x74) != 0:
                self.marks.add('q_cmap')
                wr(ci + 0x59, 1, 1)
            elif rd(ci + 0x50, 1):
                self.marks.add('q_2pass')
                wr(ci + 0x5A, 1, 1)
            else:
                self.marks.add('q_1pass')
                wr(ci + 0x58, 1, 1)
            if rd(ci + 0x58, 1):
                self.errx(0x30)
            if rd(ci + 0x5A, 1) or rd(ci + 0x59, 1):
                self.errx(0x30)
        if not rd(ci + 0x41, 1):
            self.marks.add('not_raw')
            if rd(master + 0x10, 1):
                self.errx(0x30)
            else:
                self.mod1(20)       # jinit_color_deconverter
                self.mod1(21)       # jinit_upsampler
            self.mod2(22, rd(ci + 0x5A, 1))   # jinit_d_post_controller(cinfo, enable_2pass_quant)
        else:
            self.marks.add('raw')
        self.mod1(23)               # jinit_inverse_dct
        if rd(ci + 0xC9, 1):
            self.marks.add('arith')
            self.errx(1)
        elif rd(ci + 0xC8, 1):
            self.marks.add('prog')
            self.errx(0x30)
        else:
            self.jinit_huff_decoder()
        use_c = 1 if (rd(rd(ci + 0x190) + 0x10, 1) or rd(ci + 0x40, 1)) else 0
        self.mod2(24, use_c)        # jinit_d_coef_controller(cinfo, use_c_buffer)
        if not rd(ci + 0x41, 1):
            self.main_controller()
        self.mod1(25)               # mem->realize_virt_arrays
        self.mod1(26)               # inputctl->start_input_pass

    # ---------------------------------------------------------------- ejecucion de un caso
    def run(self, fn):
        ret = None
        escaped = False
        try:
            if fn in (0, 1, 2, 3, 4, 5):
                try:
                    r = (self.get_dht, self.get_dri, self.skip_variable, self.next_marker, self.first_marker, self.read_restart_marker)[fn]()
                except Fail:
                    r = False
                ret = 1 if r else 0
            elif fn == 6:
                self.jinit_marker_reader()
            elif fn == 7:
                self.calc_output_dimensions()
            elif fn == 8:
                self.prepare_for_output_pass()
            elif fn == 9:
                self.jinit_master_decompress()
        except Escape:
            escaped = True
            ret = None
        return ret, escaped

    def mask_huffval(self):
        """get_dht copia siempre 256 bytes de huffval: los de huffval[count..255] se dejan como estaban (el banco hace lo mismo tras la llamada)"""
        ci = self.ci
        for slot in range(8):
            p = self.rd(ci + (0xA0 if slot < 4 else 0xB0) + 4 * (slot & 3))
            if not p:
                continue
            count = sum(self.rd(p + l, 1) for l in range(1, 17))
            for k in range(count, 256):
                try:
                    i, off = self.W.locate((p + 0x11 + k) & M32)
                except ValueError:
                    continue
                m = self.W.mem[i]
                idx = GUARD + off
                ref = m.r.get(idx)
                if ref is None:
                    ref = m.initial(idx)
                m.m[idx] = ref


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'scene_aligned', 'scene_unaligned']
EXTRA = {
    0: ['dc_idx', 'ac_idx', 'new_table', 'old_table', 'over256', 'len_err', 'bad_idx', 'multi_table', 'length_bad', 'fill_fail', 'fill_called', 'ok', 'count0'],
    1: ['len4', 'len_bad', 'fill_fail', 'fill_called', 'ok'],
    2: ['skip', 'no_skip', 'fill_fail', 'fill_called'],
    3: ['discard', 'ff_run', 'ff00', 'warn', 'found', 'fill_fail', 'fill_called'],
    4: ['soi_ok', 'not_soi', 'fill_fail', 'fill_called'],
    5: ['unread0', 'match', 'resync', 'resync_fail', 'nm_fail', 'ok'],
    6: [],
    7: ['bad_state', 'cs1', 'cs2', 'cs3', 'cs4', 'cs5', 'cs_default', 'quant_on'],
    8: ['dummy', 'q2', 'q1', 'mode_change', 'quant_cmap', 'raw', 'progress', 'buffered_noeoi', 'cconv'],
    9: ['quant', 'raw_quant', 'q_not3', 'q_cmap', 'q_2pass', 'q_1pass', 'not_raw', 'raw', 'arith', 'prog', 'ctx', 'no_ctx', 'notimpl_main', 'bad_state'],
}
KEYS = {fn: COMMON + EXTRA[fn] for fn in range(len(NAMES))}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    sim = Sim(c)
    ret, escaped = sim.run(fn)
    if fn == 0:
        sim.mask_huffval()
    marks = set(sim.marks)
    marks.update(['kind%d' % a[P_KIND], 'fill%d' % a[P_FILL], 'scene_unaligned' if a[P_OFF] % 4 else 'scene_aligned'])
    W = sim.W
    A, B, Cc, D = W.mem
    return {'events': sim.events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'escaped': escaped, 'marks': sorted(marks)}


def check_side(c, o, e, side_row):
    if o.get('escaped') != e['escaped']:
        return 'escaped %s != %s' % (o.get('escaped'), e['escaped'])
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        k = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (n %d vs %d, primer distinto %d) %s != %s' % (len(o['events']), len(e['events']), k, o['events'][k:k + 2], e['events'][k:k + 2])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo AL de las funciones booleanas (ya comprobado como `ret`)"""
    return None


def check_row(row, e=None):
    c = row['input']
    if e is None:
        e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
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
