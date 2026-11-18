"""Oraculo independiente (Python) de Fn233A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 12 funciones del lote 233A (la parte de descompresion
de la libjpeg 6b embebida: jdhuff.c, jdinput.c y jdmainct.c) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion;
solo reimplementa el RELLENO pseudoaleatorio de las arenas, el contenido de las tablas y la escena (Fixtures.hpp / LiveComparison.hpp del banco) para conocer la imagen inicial.

Escena (arena A, base S = arena A + 0x40 + P_OFF): jpeg_decompress_struct en S+0 (0x1B0 bytes), err S+0x1C0, mem S+0x200, src S+0x240, inputctl S+0x260, marker S+0x280, entropy S+0x330, coef S+0x3E0, upsample S+0x3F0,
6 componentes de 0x54 bytes en S+0x400. Arena B (B0): main 0x00, xbuffer[0] 0x40, xbuffer[1] 0x50, state 0x60, ranura de pdtbl 0x80, asignador (bump) 0x400..0x1C00, dtbl existente 0x1000. Arena D (D0): tablas Huffman
(0x120 de separacion) y de cuantificacion (0x900 + 0x90*k), zona pub 0x1400. Arena C: flujo de entrada de 0x540 bytes.

Desensamblado del EXE (sobre el gta-sa.exe canonico, dumpbin /disasm; ver README.md del banco para el detalle de cada funcion).
  0x5C9790 jpeg_make_d_derived_tbl(cinfo, isDC byte, tblno, pdtbl): tblno < 0 o >= 4: msg_code = 0x32, parm0 = tblno, error_exit y SIGUE; htbl = dc (+0xA0) o ac (+0xB0)[tblno]; nula: igual; *pdtbl nulo: alloc_small(1, 0x590);
        pub (+0x8C) = htbl; huffsize (suma <= 256 por nivel, si no ERREXIT(8) y sigue), huffcode (code >= 1 << si: ERREXIT(8)), maxcode (+4l), valoffset (+0x48+4l), maxcode[17] = 0xFFFFF (+0x44), look_nbits (+0x90, 256 int,
        a cero) y look_sym (+0x490) de 8 bits; con isDC los simbolos recorren numsymbols y > 15 dan ERREXIT(8).
  0x5C9A60 jpeg_fill_bit_buffer, 0x5C9B80 jpeg_huff_decode, 0x5CA260 jinit_huff_decoder, 0x5CA6E0 start_input_pass (per_scan_setup 0x5CA490, latch_quant_tables 0x5CA630), 0x5CA720 consume_markers (initial_setup 0x5CA2A0),
  0x5CA7E0 reset_input_controller, 0x5CA840 jinit_input_controller, 0x5CA940 make_funny_pointers, 0x5CAA90 set_wraparound_pointers, 0x5CAB70 set_bottom_pointers, 0x5CAE30 jinit_d_main_controller (alloc_funny_pointers 0x5CA890).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["LibJpegDecompress::jpeg_make_d_derived_tbl", "LibJpegDecompress::jpeg_fill_bit_buffer", "LibJpegDecompress::jpeg_huff_decode", "LibJpegDecompress::jinit_huff_decoder",
         "LibJpegDecompress::start_input_pass", "LibJpegDecompress::consume_markers", "LibJpegDecompress::reset_input_controller", "LibJpegDecompress::jinit_input_controller",
         "LibJpegDecompress::make_funny_pointers", "LibJpegDecompress::set_wraparound_pointers", "LibJpegDecompress::set_bottom_pointers", "LibJpegDecompress::jinit_d_main_controller"]
ENTRIES = (0x5C9790, 0x5C9A60, 0x5C9B80, 0x5CA260, 0x5CA6E0, 0x5CA720, 0x5CA7E0, 0x5CA840, 0x5CA940, 0x5CAA90, 0x5CAB70, 0x5CAE30)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

_P = ("P_OFF P_KIND P_FILL P_ERRMODE P_BOFF P_DOFF P_COFF P_NOISE A0 A1 A2 A3 A4 S_MODE S_SEED S_MASK IN_OFF IN_LEN "
      "CH_OFF0 CH_OFF1 CH_OFF2 CH_OFF3 CH_OFF4 CH_OFF5 CH_OFF6 CH_OFF7 CH_LEN0 CH_LEN1 CH_LEN2 CH_LEN3 CH_LEN4 CH_LEN5 CH_LEN6 CH_LEN7 "
      "F_WIDTH F_HEIGHT F_NCOMP F_INSCAN F_OUTSCAN F_PREC F_PROG F_MAXH F_MAXV F_MINDCT F_TOTROWS F_CINS F_UNREAD F_COEFB F_CUR0 F_CUR1 F_CUR2 F_CUR3 "
      "IC_MULTI IC_EOI IC_INHDR MK_SAWSOF MK_RET EN_WARN UP_CTX CO_CONSUME MN_WHICH MN_ROWS QMASK DCMASK ACMASK QDESC0 QDESC1 QDESC2 QDESC3 "
      "HDESC0 HDESC1 HDESC2 HDESC3 HDESC4 HDESC5 HDESC6 HDESC7 DT_EXIST DT_DESC").split()
IDX = {name: i for i, name in enumerate(_P)}
COMP0 = len(_P)
SEED0 = COMP0 + 60
P_COUNT = SEED0 + 4
globals().update(IDX)
CF_H, CF_V, CF_Q, CF_WIB, CF_HIB, CF_DCT, CF_DSW, CF_DSH, CF_NEED, CF_QT = range(10)
K_CI, K_ERR, K_MEM, K_SRC, K_INP, K_MRK, K_ENT, K_COEF, K_UP, K_COMPS = 0x000, 0x1C0, 0x200, 0x240, 0x260, 0x280, 0x330, 0x3E0, 0x3F0, 0x400
CI_SIZE = 0x1B0
MAX_COMPS = 6
B_MAIN, B_X0, B_X1, B_STATE, B_SLOT, B_ALLOC, B_ALLOC_END, B_DTBL = 0x00, 0x40, 0x50, 0x60, 0x80, 0x400, 0x1C00, 0x1000
H_STRIDE, Q_BASE, Q_STRIDE, PUB = 0x120, 0x900, 0x90, 0x1400
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


def huff_val(desc, k):
    r = rnd(desc >> 3, 200 + k)
    if ((desc >> 3) & 4) == 0:
        return r & 0xFF
    return 16 + ((r >> 8) % 240) if r % 40 == 0 else (r >> 8) & 15


def quant_word(desc, k):
    return rnd(desc >> 3, 300 + k) & 0xFFFF


def maxcode_val(desc, l):
    r = rnd(desc >> 3, 400 + l)
    m = desc & 3
    if m == 0:
        v = min(0x3FF, (1 << l) - 1) - r % 4
        return -1 if v < -1 else v
    if m == 1:
        return -1 if r % 4 == 0 else r % 0x400
    if m == 2:
        return -1 if l < 9 else r % 0x400
    return r % 0x401 - 1


def valoff_val(desc, l):
    return rnd(desc >> 3, 500 + l) % 0x600 - 0x300


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


def buf_word(seed, ci, k):
    return (rnd((seed + 77 * (ci + 1)) & M32, 600 + k) | 1) & M32


def block_word(seed, ci, k):
    return (rnd((seed + 99 * (ci + 1)) & M32, 700 + k) | 2) & M32


def make_layout(a):
    """disposicion de fn 8..10 en la arena B: (buf, xb0, xb1, bufWords, blockWords, blockOff) por componente"""
    M = s32(a[F_MINDCT])
    cur = B_ALLOC
    L = []
    for ci in range(4):
        k = COMP0 + 10 * ci
        rg = idiv(s32((a[k + CF_V] * a[k + CF_DCT]) & M32), M) if M != 0 else 0
        if rg > 0:
            bw = rg * (M + 2)
            buf = cur
            cur += 4 * bw
            blw = 2 * rg * (M + 4)
            bo = cur
            xb0 = cur + 4 * rg
            xb1 = xb0 + 4 * rg * (M + 4)
            cur += 4 * blw
            L.append((buf, xb0, xb1, bw, blw, bo))
        else:
            L.append((cur, cur, cur, 0, 0, cur))
    return L


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
        for i in range(4):
            lo = GUARD + d0off + Q_BASE + Q_STRIDE * i
            desc = a[QDESC0 + i]
            dmem.lazy.append((lo, lo + 0x82, lambda t, desc=desc: (quant_word(desc, t >> 1) >> (8 * (t & 1))) & 0xFF))
        for i in range(MAX_COMPS):
            p = S + K_COMPS + 0x54 * i
            k = COMP0 + 10 * i
            wr(p + 0x08, 4, a[k + CF_H]); wr(p + 0x0C, 4, a[k + CF_V]); wr(p + 0x10, 4, a[k + CF_Q]); wr(p + 0x1C, 4, a[k + CF_WIB]); wr(p + 0x20, 4, a[k + CF_HIB]); wr(p + 0x24, 4, a[k + CF_DCT])
            wr(p + 0x28, 4, a[k + CF_DSW]); wr(p + 0x2C, 4, a[k + CF_DSH]); wr(p + 0x30, 4, a[k + CF_NEED]); wr(p + 0x4C, 4, a[k + CF_QT])
        # los punteros a los falsos (err, mem, src, marker, entropy, coef) son direcciones del ASI del banco: la funcion no las lee ni las escribe, salvo por las llamadas, y no entran en los resumenes FNV, asi que no se modelan
        # (los campos de funcion de err/mem/src/marker/entropy/coef estan fuera de cinfo e inputctl)
        wr(S + K_SRC + 0x00, 4, (C0 + 0x20) & M32); wr(S + K_SRC + 0x04, 4, 0)
        wr(S + K_INP + 0x10, 1, a[IC_MULTI]); wr(S + K_INP + 0x11, 1, a[IC_EOI]); wr(S + K_INP + 0x14, 1, a[IC_INHDR])
        wr(S + K_MRK + 0x0D, 1, a[MK_SAWSOF])
        wr(S + K_ENT + 0x08, 1, a[EN_WARN])
        wr(S + K_COEF + 0x04, 4, a[CO_CONSUME])
        wr(S + K_UP + 0x08, 1, a[UP_CTX])
        ci = S + K_CI
        wr(ci + 0x00, 4, S + K_ERR); wr(ci + 0x04, 4, S + K_MEM); wr(ci + 0x18, 4, S + K_SRC)
        wr(ci + 0x1C, 4, a[F_WIDTH]); wr(ci + 0x20, 4, a[F_HEIGHT]); wr(ci + 0x24, 4, a[F_NCOMP]); wr(ci + 0x7C, 4, a[F_INSCAN]); wr(ci + 0x84, 4, a[F_OUTSCAN]); wr(ci + 0x8C, 4, a[F_COEFB])
        for i in range(4):
            wr(ci + 0x90 + 4 * i, 4, 0 if (a[QMASK] >> i) & 1 else D0 + Q_BASE + Q_STRIDE * i)
            wr(ci + 0xA0 + 4 * i, 4, 0 if (a[DCMASK] >> i) & 1 else D0 + H_STRIDE * i)
            wr(ci + 0xB0 + 4 * i, 4, 0 if (a[ACMASK] >> i) & 1 else D0 + H_STRIDE * (4 + i))
            wr(ci + 0x128 + 4 * i, 4, S + K_COMPS + 0x54 * a[F_CUR0 + i])
        wr(ci + 0xC0, 4, a[F_PREC]); wr(ci + 0xC4, 4, S + K_COMPS); wr(ci + 0xC8, 1, a[F_PROG])
        wr(ci + 0x110, 4, a[F_MAXH]); wr(ci + 0x114, 4, a[F_MAXV]); wr(ci + 0x118, 4, a[F_MINDCT]); wr(ci + 0x11C, 4, a[F_TOTROWS]); wr(ci + 0x124, 4, a[F_CINS])
        wr(ci + 0x17C, 4, a[F_UNREAD])
        wr(ci + 0x184, 4, B0 + B_MAIN); wr(ci + 0x188, 4, S + K_COEF); wr(ci + 0x190, 4, S + K_INP); wr(ci + 0x194, 4, S + K_MRK); wr(ci + 0x198, 4, S + K_ENT); wr(ci + 0x1A0, 4, S + K_UP)
        self.ci = ci
        if fn == 0:
            wr(B0 + B_SLOT, 4, B0 + B_DTBL if a[DT_EXIST] else 0)
        elif fn in (1, 2):
            st = B0 + B_STATE
            wr(st + 0x00, 4, C0 + a[IN_OFF]); wr(st + 0x04, 4, a[IN_LEN]); wr(st + 0x10, 4, S)
            smode, sseed = a[S_MODE], a[S_SEED]
            lo = GUARD + a[P_COFF]
            W.mem[2].lazy.append((lo, lo + STREAM_SIZE, lambda t: stream_byte(smode, sseed, t)))
            if fn == 2:
                d = B0 + B_ALLOC
                for l in range(1, 17):
                    wr(d + 4 * l, 4, maxcode_val(a[DT_DESC], l) & M32)
                    wr(d + 0x48 + 4 * l, 4, valoff_val(a[DT_DESC], l) & M32)
                wr(d + 0x44, 4, 0xFFFFF)
                wr(d + 0x8C, 4, D0 + PUB)
        elif fn in (8, 9, 10):
            L = make_layout(a)
            m = B0 + B_MAIN
            bmem = W.mem[1]
            boff = a[P_BOFF]
            for i in range(4):
                buf, xb0, xb1, bw, blw, bo = L[i]
                wr(m + 0x08 + 4 * i, 4, B0 + buf)
                wr(B0 + B_X0 + 4 * i, 4, B0 + xb0)
                wr(B0 + B_X1 + 4 * i, 4, B0 + xb1)
                if bw:
                    lo = GUARD + boff + buf
                    bmem.lazy.append((lo, lo + 4 * bw, lambda t, i=i: (buf_word(self.seed, i, t >> 2) >> (8 * (t & 3))) & 0xFF))
                    lo2 = GUARD + boff + bo
                    bmem.lazy.append((lo2, lo2 + 4 * blw, lambda t, i=i: (block_word(self.seed, i, t >> 2) >> (8 * (t & 3))) & 0xFF))
            wr(m + 0x20, 4, B0 + B_X0); wr(m + 0x24, 4, B0 + B_X1); wr(m + 0x28, 4, a[MN_WHICH]); wr(m + 0x30, 4, a[MN_ROWS])

    # ---------------------------------------------------------------- memoria
    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def tag(self, v):
        return self.W.tagv(v)

    def bytes_of(self, addr, n):
        return bytes(self.rd(addr + i, 1) for i in range(n))

    def state_hash(self):
        return fnv(self.bytes_of(self.ci, CI_SIZE)), fnv(self.bytes_of(self.rd(self.ci + 0x190), 0x18))

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

    def emit_msg(self, ci, level):
        self.events.append([2, level & M32, self.rd(self.rd(ci) + 0x14), self.tag(ci)])

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

    def fill_cb(self, ci):
        a = self.a
        src = self.rd(ci + 0x18)
        idx = self.fills
        self.fills += 1
        self.events.append([7, idx, self.tag(self.rd(src)), self.rd(src + 4), self.tag(ci)])
        ok = idx < 8 and (a[S_MASK] >> idx) & 1
        if ok:
            self.wr(src, 4, (self.C0 + a[CH_OFF0 + idx]) & M32)
            self.wr(src + 4, 4, a[CH_LEN0 + idx])
        return bool(ok)

    def start_cb(self, ident):
        ch, ih = self.state_hash()
        self.events.append([ident, self.tag(self.ci), ch, ih])

    def read_markers(self):
        ch, ih = self.state_hash()
        self.events.append([12, self.tag(self.ci), ch, ih])
        return self.a[MK_RET]

    def reset_marker(self):
        ch, ih = self.state_hash()
        self.events.append([13, self.tag(self.ci), ch, ih])
        self.wr(self.rd(self.ci + 0x190) + 0x10, 1, 0x66)
        self.wr(self.ci + 0x8C, 4, 0xAAAA0002)

    def reset_err(self):
        ch, ih = self.state_hash()
        self.events.append([14, self.tag(self.ci), ch, ih])
        self.wr(self.rd(self.ci + 0x190) + 0x11, 1, 0x77)
        self.wr(self.ci + 0x8C, 4, 0xAAAA0001)

    # ---------------------------------------------------------------- jdhuff
    def make_d_derived_tbl(self, isdc_arg, tblno_arg, pdtbl):
        ci = self.ci
        rd, wr = self.rd, self.wr
        isdc = (isdc_arg & 0xFF) != 0
        tbl = s32(tblno_arg)
        self.marks.add('dc' if isdc else 'ac')
        if tbl < 0 or tbl >= 4:
            self.marks.add('bad_tblno')
            self.errx(0x32, tbl)
        htbl = rd(ci + (0xA0 if isdc else 0xB0) + tbl * 4)
        if htbl == 0:
            self.marks.add('null_tbl')
            self.errx(0x32, tbl)
        if rd(pdtbl) == 0:
            self.marks.add('new_dtbl')
            wr(pdtbl, 4, self.alloc(1, 0x590))
        else:
            self.marks.add('old_dtbl')
        d = rd(pdtbl)
        wr(d + 0x8C, 4, htbl)
        bits = [0] + [rd(htbl + l, 1) for l in range(1, 17)]
        huffsize = []
        p = 0
        for l in range(1, 17):
            i = bits[l]
            if p + i > 256:
                self.marks.add('over256')
                self.errx(8)
            if i:
                huffsize += [l] * i
                p += i
        numsymbols = p
        huffsize.append(0)
        hc = [0] * (numsymbols + 1)
        code = 0
        si = huffsize[0]
        p = 0
        if si != 0:
            while True:
                while huffsize[p] == si:
                    hc[p] = code
                    p += 1
                    code += 1
                if code >= (1 << (si & 31)):
                    self.marks.add('bad_code')
                    self.errx(8)
                code <<= 1
                si += 1
                if huffsize[p] == 0:
                    break
        p = 0
        for l in range(1, 17):
            if bits[l] != 0:
                wr(d + 0x48 + 4 * l, 4, (p - hc[p]) & M32)
                p += bits[l]
                wr(d + 4 * l, 4, hc[p - 1] & M32)
            else:
                wr(d + 4 * l, 4, 0xFFFFFFFF)
        wr(d + 0x44, 4, 0xFFFFF)
        for k in range(0x100):
            wr(d + 0x90 + 4 * k, 4, 0)
        p = 0
        for l in range(1, 9):
            for _ in range(bits[l]):
                lb = hc[p] << (8 - l)
                for _c in range(1 << (8 - l)):
                    wr(d + 0x90 + 4 * lb, 4, l)
                    wr(d + 0x490 + lb, 1, rd(htbl + 0x11 + p, 1))
                    lb += 1
                p += 1
                self.marks.add('lookahead')
        if isdc:
            for i in range(numsymbols):
                if rd(htbl + 0x11 + i, 1) > 15:
                    self.marks.add('dc_bad_sym')
                    self.errx(8)

    def fill_bit_buffer(self, state, get_buffer, bits_left, nbits):
        rd, wr = self.rd, self.wr
        ci = rd(state + 0x10)
        nxt = rd(state)
        left = rd(state + 4)
        buf = get_buffer & M32
        bl = s32(bits_left)
        nb = s32(nbits)
        marker = rd(ci + 0x17C) != 0
        check = marker
        if marker:
            self.marks.add('marker_pending')
        elif bl < 25:
            while True:
                if left == 0:
                    self.marks.add('fill_called')
                    if not self.fill_cb(ci):
                        self.marks.add('fill_fail')
                        return False
                    src = rd(ci + 0x18)
                    nxt, left = rd(src), rd(src + 4)
                c = rd(nxt, 1)
                left = (left - 1) & M32
                nxt = (nxt + 1) & M32
                if c == 0xFF:
                    self.marks.add('saw_ff')
                    while True:
                        if left == 0:
                            self.marks.add('fill_called')
                            if not self.fill_cb(ci):
                                self.marks.add('fill_fail')
                                return False
                            src = rd(ci + 0x18)
                            nxt, left = rd(src), rd(src + 4)
                        c = rd(nxt, 1)
                        left = (left - 1) & M32
                        nxt = (nxt + 1) & M32
                        if c != 0xFF:
                            break
                        self.marks.add('ff_run')
                    if c != 0:
                        wr(ci + 0x17C, 4, c)
                        marker = True
                        check = True
                        self.marks.add('marker_found')
                        break
                    c = 0xFF
                    self.marks.add('ff00')
                buf = ((buf << 8) | c) & M32
                bl += 8
                if bl >= 25:
                    break
        else:
            self.marks.add('bl_ge25')
        if check and nb > bl:
            if rd(rd(ci + 0x198) + 8, 1) == 0:
                self.marks.add('warn')
                wr(rd(ci) + 0x14, 4, 0x75)
                self.emit_msg(ci, -1)
                wr(rd(ci + 0x198) + 8, 1, 1)
            else:
                self.marks.add('warned_before')
            buf = (buf << ((25 - bl) & 31)) & M32
            bl = 25
            self.marks.add('shift')
        wr(state + 4, 4, left)
        wr(state, 4, nxt)
        wr(state + 0xC, 4, bl & M32)
        wr(state + 8, 4, buf)
        self.marks.add('fill_ok')
        return True

    def huff_decode(self, state, get_buffer, bits_left, dtbl, min_bits):
        rd, wr = self.rd, self.wr
        l = s32(min_bits)
        buf = get_buffer & M32
        bits = s32(bits_left)
        if bits < l:
            self.marks.add('fill_first')
            if not self.fill_bit_buffer(state, buf, bits, l):
                self.marks.add('huff_fail')
                return 0xFFFFFFFF
            buf = rd(state + 8)
            bits = s32(rd(state + 0xC))
        else:
            self.marks.add('direct')
        bits -= l
        code = (s32(buf) >> (bits & 31)) & (((1 << (l & 31)) - 1) & M32)
        code = s32(code)
        while code > s32(rd(dtbl + 4 * l)):
            self.marks.add('loop_iter')
            code = s32((code << 1) & M32)
            if bits < 1:
                self.marks.add('fill_loop')
                if not self.fill_bit_buffer(state, buf, bits, 1):
                    self.marks.add('huff_fail')
                    return 0xFFFFFFFF
                buf = rd(state + 8)
                bits = s32(rd(state + 0xC))
            bits -= 1
            code = s32(code | ((s32(buf) >> (bits & 31)) & 1))
            l += 1
        wr(state + 8, 4, buf)
        wr(state + 0xC, 4, bits & M32)
        if l > 16:
            self.marks.add('l_gt16')
            ci = rd(state + 0x10)
            wr(rd(ci) + 0x14, 4, 0x76)
            self.emit_msg(rd(state + 0x10), -1)
            return 0
        self.marks.add('huff_ok')
        addr = (rd(dtbl + 0x48 + 4 * l) + rd(dtbl + 0x8C) + code + 0x11) & M32
        return rd(addr, 1)

    def jinit_huff_decoder(self):
        ci = self.ci
        ent = self.alloc(1, 0xAC)
        self.wr(ci + 0x198, 4, ent)
        self.wr(ent, 4, 0x5CA110)
        self.wr(ent + 4, 4, 0x5C9CE0)
        for i in range(4):
            self.wr(ent + 0x38 + 4 * i, 4, 0)
            self.wr(ent + 0x28 + 4 * i, 4, 0)

    # ---------------------------------------------------------------- jdinput
    def initial_setup(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        if s32(rd(ci + 0x20)) > 0xFFDC or s32(rd(ci + 0x1C)) > 0xFFDC:
            self.marks.add('image_too_big')
            self.errx(0x29, 0xFFDC)
        if s32(rd(ci + 0xC0)) != 8:
            self.marks.add('bad_precision')
            self.errx(0x0F, rd(ci + 0xC0))
        if s32(rd(ci + 0x24)) > 4:
            self.marks.add('too_many_comps')
            self.errx(0x1A, rd(ci + 0x24), 4)
        wr(ci + 0x110, 4, 1)
        wr(ci + 0x114, 4, 1)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            h, v = s32(rd(c + 8)), s32(rd(c + 0xC))
            if h <= 0 or h > 4 or v <= 0 or v > 4:
                self.marks.add('bad_sampling')
                self.errx(0x12)
            mh, h2 = s32(rd(ci + 0x110)), s32(rd(c + 8))
            wr(ci + 0x110, 4, (mh if mh > h2 else h2) & M32)
            mv, v2 = s32(rd(ci + 0x114)), s32(rd(c + 0xC))
            wr(ci + 0x114, 4, (mv if mv > v2 else v2) & M32)
            i += 1
        wr(ci + 0x118, 4, 8)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            wr(c + 0x24, 4, 8)
            h = s32(rd(c + 8))
            wr(c + 0x1C, 4, jdiv((h * rd(ci + 0x1C)) & M32, (rd(ci + 0x110) << 3) & M32))
            v = s32(rd(c + 0xC))
            wr(c + 0x20, 4, jdiv((v * rd(ci + 0x20)) & M32, (rd(ci + 0x114) << 3) & M32))
            h = s32(rd(c + 8))
            wr(c + 0x28, 4, jdiv((h * rd(ci + 0x1C)) & M32, rd(ci + 0x110)))
            v = s32(rd(c + 0xC))
            wr(c + 0x2C, 4, jdiv((v * rd(ci + 0x20)) & M32, rd(ci + 0x114)))
            wr(c + 0x30, 1, 1)
            wr(c + 0x4C, 4, 0)
            i += 1
        wr(ci + 0x11C, 4, jdiv(rd(ci + 0x20), (rd(ci + 0x114) << 3) & M32))
        inp = rd(ci + 0x190)
        if s32(rd(ci + 0x124)) < s32(rd(ci + 0x24)) or rd(ci + 0xC8, 1) != 0:
            self.marks.add('multi_scans')
            wr(inp + 0x10, 1, 1)
        else:
            wr(inp + 0x10, 1, 0)

    def per_scan(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        cs = s32(rd(ci + 0x124))
        if cs == 1:
            self.marks.add('scan_single')
            c = rd(ci + 0x128)
            wr(ci + 0x138, 4, rd(c + 0x1C))
            wr(ci + 0x13C, 4, rd(c + 0x20))
            v = rd(c + 0x0C)
            wr(c + 0x40, 4, rd(c + 0x24))
            rem = rd(c + 0x20) % v
            for off in (0x34, 0x38, 0x3C, 0x44):
                wr(c + off, 4, 1)
            wr(c + 0x48, 4, v if rem == 0 else rem)
            wr(ci + 0x140, 4, 1)
            wr(ci + 0x144, 4, 0)
            return
        self.marks.add('scan_multi')
        if cs <= 0 or cs > 4:
            self.marks.add('bad_cs')
            self.errx(0x1A, rd(ci + 0x124), 4)
        wr(ci + 0x138, 4, jdiv(rd(ci + 0x1C), (rd(ci + 0x110) << 3) & M32))
        wr(ci + 0x13C, 4, jdiv(rd(ci + 0x20), (rd(ci + 0x114) << 3) & M32))
        wr(ci + 0x140, 4, 0)
        i = 0
        while i < s32(rd(ci + 0x124)):
            c = rd(ci + 0x128 + 4 * i)
            h = rd(c + 8)
            wr(c + 0x40, 4, (rd(c + 0x24) * h) & M32)
            v = rd(c + 0x0C)
            mb = (v * h) & M32
            wr(c + 0x34, 4, h)
            wr(c + 0x38, 4, v)
            wr(c + 0x3C, 4, mb)
            rem = rd(c + 0x1C) % h
            wr(c + 0x44, 4, h if rem == 0 else rem)
            rem = rd(c + 0x20) % v
            wr(c + 0x48, 4, v if rem == 0 else rem)
            if s32(rd(ci + 0x140) + mb) > 10:
                self.marks.add('bad_mcu')
                self.errx(0x0D)
            n = mb
            while s32(n) > 0:
                idx = rd(ci + 0x140)
                wr(ci + 0x144 + 4 * s32(idx), 4, i)
                wr(ci + 0x140, 4, (rd(ci + 0x140) + 1) & M32)
                n = (n - 1) & M32
            i += 1

    def latch(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        i = 0
        while i < s32(rd(ci + 0x124)):
            c = rd(ci + 0x128 + 4 * i)
            if rd(c + 0x4C) == 0:
                q = s32(rd(c + 0x10))
                if q < 0 or q >= 4 or rd(ci + 0x90 + 4 * q) == 0:
                    self.marks.add('bad_quant')
                    self.errx(0x34, q)
                self.marks.add('latch_alloc')
                t = self.alloc(1, 0x82)
                src = rd(ci + 0x90 + 4 * q)
                data = [rd(src + k, 1) for k in range(0x82)]
                for k in range(0x82):
                    wr(t + k, 1, data[k])
                wr(c + 0x4C, 4, t)
            else:
                self.marks.add('latch_skip')
            i += 1

    def start_input_pass(self):
        ci = self.ci
        self.per_scan()
        self.latch()
        self.start_cb(10)
        self.start_cb(11)
        coef = self.rd(ci + 0x188)
        self.wr(self.rd(ci + 0x190), 4, self.rd(coef + 4))

    def consume_markers(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        inp = rd(ci + 0x190)
        if rd(inp + 0x11, 1):
            self.marks.add('eoi')
            return 2
        val = self.read_markers()
        if val == 1:
            self.marks.add('val1')
            if rd(inp + 0x14, 1):
                self.marks.add('initial')
                self.initial_setup()
                wr(inp + 0x14, 1, 0)
            else:
                self.marks.add('start')
                if not rd(inp + 0x10, 1):
                    self.marks.add('eoi_expected')
                    self.errx(0x23)
                self.start_input_pass()
        elif val == 2:
            self.marks.add('val2')
            ih = rd(inp + 0x14, 1)
            wr(inp + 0x11, 1, 1)
            if ih:
                if rd(rd(ci + 0x194) + 0x0D, 1):
                    self.marks.add('sof_err')
                    self.errx(0x3B)
            else:
                scan = rd(ci + 0x7C)
                if s32(rd(ci + 0x84)) > s32(scan):
                    self.marks.add('scan_clip')
                    wr(ci + 0x84, 4, scan)
        else:
            self.marks.add('val_other' if val != 0 else 'val0')
        return val

    def reset_input_controller(self):
        ci = self.ci
        inp = self.rd(ci + 0x190)
        self.wr(inp, 4, 0x5CA720)
        self.wr(inp + 0x10, 1, 0)
        self.wr(inp + 0x11, 1, 0)
        self.wr(inp + 0x14, 1, 1)
        self.reset_err()
        self.reset_marker()
        self.wr(ci + 0x8C, 4, 0)

    def jinit_input_controller(self):
        ci = self.ci
        p = self.alloc(0, 0x18)
        self.wr(ci + 0x190, 4, p)
        for off, v in ((0, 0x5CA720), (4, 0x5CA7E0), (8, 0x5CA6E0), (0xC, 0x5CA820)):
            self.wr(p + off, 4, v)
        self.wr(p + 0x10, 1, 0)
        self.wr(p + 0x11, 1, 0)
        self.wr(p + 0x14, 1, 1)

    # ---------------------------------------------------------------- jdmainct
    def rgroup(self, c):
        return idiv(s32((s32(self.rd(c + 0x0C)) * s32(self.rd(c + 0x24))) & M32), s32(self.rd(self.ci + 0x118)))

    def make_funny_pointers(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        M = s32(rd(ci + 0x118))
        main = rd(ci + 0x184)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            rg = self.rgroup(c)
            self.marks.add('rg_pos' if rg > 0 else 'rg_nonpos')
            x0 = rd(rd(main + 0x20) + 4 * i)
            x1 = rd(rd(main + 0x24) + 4 * i)
            buf = rd(main + 8 + 4 * i)
            for k in range(max(0, s32(((M + 2) * rg) & M32))):
                v = rd(buf + 4 * k)
                wr(x1 + 4 * k, 4, v)
                wr(x0 + 4 * k, 4, v)
            for k in range(max(0, 2 * rg)):
                wr(x1 + 4 * ((M - 2) * rg + k), 4, rd(buf + 4 * (rg * M + k)))
                wr(x1 + 4 * (rg * M + k), 4, rd(buf + 4 * ((M - 2) * rg + k)))
            for k in range(max(0, rg)):
                wr(x0 + 4 * (k - rg), 4, rd(x0))
            i += 1

    def set_wraparound_pointers(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        M = s32(rd(ci + 0x118))
        main = rd(ci + 0x184)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            rg = self.rgroup(c)
            self.marks.add('rg_pos' if rg > 0 else 'rg_nonpos')
            x0 = rd(rd(main + 0x20) + 4 * i)
            x1 = rd(rd(main + 0x24) + 4 * i)
            for k in range(max(0, rg)):
                wr(x0 + 4 * (k - rg), 4, rd(x0 + 4 * (rg * (M + 1) + k)))
                wr(x1 + 4 * (k - rg), 4, rd(x1 + 4 * (rg * (M + 1) + k)))
                wr(x0 + 4 * (rg * (M + 2) + k), 4, rd(x0 + 4 * k))
                wr(x1 + 4 * (rg * (M + 2) + k), 4, rd(x1 + 4 * k))
            i += 1

    def set_bottom_pointers(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        main = rd(ci + 0x184)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            imh = (s32(rd(c + 0x0C)) * s32(rd(c + 0x24))) & M32
            rg = idiv(s32(imh), s32(rd(ci + 0x118)))
            rows = rd(c + 0x2C) % imh
            self.marks.add('rg_pos')
            if rows == 0:
                rows = imh
                self.marks.add('rows_zero')
            if i == 0:
                wr(main + 0x30, 4, (idiv(s32(rows) - 1, rg) + 1) & M32)
            xbuf = rd(rd(main + 0x20 + 4 * s32(rd(main + 0x28))) + 4 * i)
            self.marks.add('which%d' % (s32(rd(main + 0x28)) & 1))
            for k in range(max(0, 2 * rg)):
                wr(xbuf + 4 * (rows + k), 4, rd(xbuf + 4 * (rows - 1)))
            i += 1

    def alloc_funny(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        M = s32(rd(ci + 0x118))
        main = rd(ci + 0x184)
        xb = self.alloc(1, (rd(ci + 0x24) << 3) & M32)
        wr(main + 0x20, 4, xb)
        wr(main + 0x24, 4, (xb + rd(ci + 0x24) * 4) & M32)
        comp = rd(ci + 0xC4)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = comp + 0x54 * i
            rg = self.rgroup(c)
            span = ((M + 4) * rg) & M32
            blk = self.alloc(1, (span * 8) & M32)
            x0 = (blk + rg * 4) & M32
            wr(rd(main + 0x20) + 4 * i, 4, x0)
            x1 = (x0 + span * 4) & M32
            wr(rd(main + 0x24) + 4 * i, 4, x1)
            i += 1

    def jinit_d_main_controller(self, need_full):
        ci = self.ci
        rd, wr = self.rd, self.wr
        main = self.alloc(1, 0x38)
        wr(ci + 0x184, 4, main)
        wr(main, 4, 0x5CADC0)
        if need_full & 0xFF:
            self.marks.add('full_buffer')
            self.errx(4)
        up = rd(ci + 0x1A0)
        if rd(up + 8, 1) != 0:
            self.marks.add('ctx')
            if s32(rd(ci + 0x118)) < 2:
                self.marks.add('notimpl')
                self.errx(0x2F)
            self.alloc_funny()
            ngroups = (rd(ci + 0x118) + 2) & M32
        else:
            self.marks.add('no_ctx')
            ngroups = rd(ci + 0x118)
        i = 0
        while i < s32(rd(ci + 0x24)):
            c = rd(ci + 0xC4) + 0x54 * i
            dct = rd(c + 0x24)
            rg = self.rgroup(c)
            h = (rg * s32(ngroups)) & M32
            w = (rd(c + 0x1C) * dct) & M32
            r = self.alloc_sarray(1, w, h)
            wr(main + 8 + 4 * i, 4, r)
            self.marks.add('sarray')
            i += 1

    # ---------------------------------------------------------------- ejecucion de un caso
    def run(self, fn):
        a = self.a
        ret = None
        escaped = False
        B0, S = self.B0, self.S
        try:
            if fn == 0:
                self.make_d_derived_tbl(a[A0] | (a[P_NOISE] & 0xFFFFFF00), a[A1], B0 + B_SLOT)
            elif fn == 1:
                ret = 1 if self.fill_bit_buffer(B0 + B_STATE, a[A0], a[A1], a[A2]) else 0
            elif fn == 2:
                ret = self.huff_decode(B0 + B_STATE, a[A0], a[A1], B0 + B_ALLOC, a[A2])
            elif fn == 3:
                self.jinit_huff_decoder()
            elif fn == 4:
                self.start_input_pass()
            elif fn == 5:
                ret = self.consume_markers() & M32
            elif fn == 6:
                self.reset_input_controller()
            elif fn == 7:
                self.jinit_input_controller()
            elif fn == 8:
                self.make_funny_pointers()
            elif fn == 9:
                self.set_wraparound_pointers()
            elif fn == 10:
                self.set_bottom_pointers()
            elif fn == 11:
                self.jinit_d_main_controller(a[A0] | (a[P_NOISE] & 0xFFFFFF00))
        except Escape:
            escaped = True
            ret = None
        return ret, escaped


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'scene_aligned', 'scene_unaligned']
EXTRA = {
    0: ['dc', 'ac', 'new_dtbl', 'old_dtbl', 'over256', 'bad_code', 'dc_bad_sym', 'bad_tblno', 'null_tbl', 'lookahead'],
    1: ['marker_pending', 'fill_called', 'fill_fail', 'marker_found', 'ff00', 'warn', 'bl_ge25', 'shift', 'fill_ok', 'saw_ff'],
    2: ['fill_first', 'fill_loop', 'huff_fail', 'direct', 'loop_iter', 'l_gt16', 'huff_ok'],
    3: [],
    4: ['scan_single', 'scan_multi', 'latch_alloc', 'latch_skip', 'bad_cs', 'bad_quant', 'bad_mcu'],
    5: ['eoi', 'val0', 'val1', 'val2', 'val_other', 'initial', 'start', 'eoi_expected', 'sof_err', 'scan_clip', 'multi_scans'],
    6: [], 7: [],
    8: ['rg_pos', 'rg_nonpos'], 9: ['rg_pos', 'rg_nonpos'], 10: ['rg_pos', 'which0', 'which1', 'rows_zero'],
    11: ['full_buffer', 'ctx', 'no_ctx', 'notimpl', 'sarray', 'alloc_dummy'],
}
KEYS = {fn: COMMON + EXTRA[fn] for fn in range(len(NAMES))}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    sim = Sim(c)
    ret, escaped = sim.run(fn)
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo AL de jpeg_fill_bit_buffer y EAX de jpeg_huff_decode y consume_markers (ya comprobados)"""
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
