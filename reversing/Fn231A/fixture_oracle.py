"""Oraculo independiente (Python) de Fn231A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 18 funciones del lote 231A (la parte de compresion de
la libjpeg 6b embebida: jcmarker.c, jcmaster.c, jcapimin.c y jcomapi.c) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion;
solo reimplementa el RELLENO pseudoaleatorio de las arenas, el contenido de las tablas y la escena (Fixtures.hpp / LiveComparison.hpp del banco) para conocer la imagen inicial.

Escena (arena A, base S = arena A + 0x40 + P_OFF): jpeg_compress_struct en S+0 (0x160 bytes), err S+0x170, mem S+0x1B0, progress S+0x1F8-0x14.., dest S+0x1F8, master S+0x210, marker S+0x230, submodulos S+0x250 (8 bytes
cada uno), 8 componentes de 0x54 bytes en S+0x290. Tablas de cuantificacion (0x90 de separacion, 0x82 bytes) y Huffman (0x120, 0x112 bytes) en la arena D; bufer de salida en la C; destino de alloc_small en la B.

Desensamblado del EXE (sobre el gta-sa.exe canonico; ver README.md del banco para el detalle de cada funcion).
  emit_byte (0x5C54B0, ESI = cinfo): *next++ = v; free--; si free == 0: llama empty_output_buffer(cinfo); si AL == 0: msg_code = 0x18, error_exit(cinfo). emit_marker 0xFF y el marcador; emit_2bytes (v >> 8) y v.
  0x5C55D0 EmitDqt(EAX = cinfo, indice): tabla = quant_tbl_ptrs[indice] (+0x48); si es nula: msg_code = 0x34, parm0 = indice, error_exit; prec = 1 si algun word sin signo > 255; si !sent_table (+0x80):
        marcador 0xDB, longitud 0x83 (prec) o 0x43, byte indice + (prec << 4), 64 valores en orden natural (jpeg_natural_order, 0x86B818), con prec el byte alto antes; sent_table = 1. Devuelve prec en EAX.
  0x5C5780 EmitDht(ECX = cinfo, AL = es AC, indice): tabla = dc (+0x58) o ac (+0x68)[indice]; con AC el indice += 0x10; nula: 0x32 + parm0; si !sent_table (+0x111): marcador 0xC4, longitud = suma(bits[1..16]) + 19,
        byte indice, bits[1..16], huffval[0..longitud), sent_table = 1.
  0x5C5910 EmitDri: 0xDD, 4, restart_interval.   0x5C5940 EmitSof(ECX = cinfo, AL = marcador): marcador, 3n+8, [alto o ancho > 65535 (con signo): 0x29, parm0 = 65535], precision, alto, ancho, n, por componente id,
        (h << 4) + v (byte), quant_tbl_no.   0x5C5AC0 EmitSos: 0xDA, 2n+6, n, por componente id y (td << 4) + ta (en progresivo: Ss == 0 -> ta = 0 y td = 0 si Ah != 0 y no aritmetico; si no td = 0), Ss, Se, (Ah << 4) + Al.
  0x5C5CA0 EmitJfifApp0: 0xE0, 16, "JFIF\\0", version, unidad, densidades, 0, 0.   0x5C5EC0 EmitAdobeApp14: 0xEE, 14, "Adobe", 100, 0, 0, transformada (1 si espacio 3, 2 si 5, si no 0).
  0x5C60B0 WriteFileHeader: SOI, last_restart_interval = 0, JFIF si write_JFIF_header, Adobe si write_Adobe_marker.   0x5C6100 WriteFrameHeader: suma EmitDqt de cada componente; base si !aritmetico, !progresivo, precision 8
        y dc/ac <= 1 (con signo); con suma > 0 y base: msg_code = 0x4B, emit_message(cinfo, 0), deja de ser base; SOF 0xC9 / 0xC2 / 0xC0 / 0xC1.   0x5C62D0 WriteFileTrailer: EOI.
  0x5C63D0 JinitMarkerWriter: alloc_small(1, 0x20) -> +0x14C; 0x5C60B0, 0x5C6100, 0x5C61E0, 0x5C62D0, 0x5C62F0, 0x5C6030, 0x5C6070, 0.
  0x5C6430 initial_setup, 0x5C6600 select_scan_parameters, 0x5C6680 per_scan_setup, 0x5C6850 PrepareForPass, 0x5C69E0 PassStartup, 0x5C6A90 JinitCMasterControl: ver cada funcion abajo.
  0x5C6B50 JpegAbort, 0x5C6B90 JpegDestroy, 0x5C6BB0 / 0x5C6BD0 alloc de tablas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["LibJpegCompress::EmitDqt", "LibJpegCompress::EmitDht", "LibJpegCompress::EmitDri", "LibJpegCompress::EmitSof", "LibJpegCompress::EmitSos",
         "LibJpegCompress::EmitJfifApp0", "LibJpegCompress::EmitAdobeApp14", "LibJpegCompress::WriteFileHeader", "LibJpegCompress::WriteFrameHeader", "LibJpegCompress::WriteFileTrailer",
         "LibJpegCompress::JinitMarkerWriter", "LibJpegCompress::PrepareForPass", "LibJpegCompress::PassStartup", "LibJpegCompress::JinitCMasterControl", "LibJpegCompress::JpegAbort",
         "LibJpegCompress::JpegDestroy", "LibJpegCompress::JpegAllocQuantTable", "LibJpegCompress::JpegAllocHuffTable"]
ENTRIES = (0x5C55D0, 0x5C5780, 0x5C5910, 0x5C5940, 0x5C5AC0, 0x5C5CA0, 0x5C5EC0, 0x5C60B0, 0x5C6100, 0x5C62D0, 0x5C63D0, 0x5C6850, 0x5C69E0, 0x5C6A90, 0x5C6B50, 0x5C6B90, 0x5C6BB0, 0x5C6BD0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
# jpeg_natural_order del EXE (0x86B818), leido del PE
NATURAL = [0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18, 11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51, 58, 59, 52, 45, 38, 31, 39,
           46, 53, 60, 61, 54, 47, 55, 62, 63]

_P = ("P_OFF P_KIND P_FILL P_ARG P_ARG2 P_EAXHI P_COFF P_DSIZE P_DFREE P_EOBRET P_ERRMODE P_BOFF P_DOFF P_QMASK P_DCMASK P_ACMASK P_NOMEM P_NOPROG "
      "F_WIDTH F_HEIGHT F_INCOMP F_PREC F_NCOMP F_CSPACE F_NSCANS F_SCANINFO F_RAWIN F_ARITH F_OPT F_RINT F_RROWS F_JFIF F_JMAJ F_JMIN F_DUNIT F_XD F_YD F_ADOBE F_PROG "
      "F_MAXH F_MAXV F_CINSCAN F_SS F_SE F_AH F_AL F_STATE F_ISDEC M_CALLST M_ISLAST M_PTYPE M_PNUM M_TOTAL M_SCANNO K_LASTRI CUR0 CUR1 CUR2 CUR3 "
      "QDESC0 QDESC1 QDESC2 QDESC3 QSENT0 QSENT1 QSENT2 QSENT3 HDESC0 HDESC1 HDESC2 HDESC3 HDESC4 HDESC5 HDESC6 HDESC7 HSENT0 HSENT1 HSENT2 HSENT3 HSENT4 HSENT5 HSENT6 HSENT7").split()
IDX = {name: i for i, name in enumerate(_P)}
COMP0 = len(_P)
SEED0 = COMP0 + 64
P_COUNT = SEED0 + 4
globals().update(IDX)
CF_ID, CF_H, CF_V, CF_Q, CF_DC, CF_AC, CF_WIB, CF_HIB = range(8)
K_CI, K_ERR, K_MEM, K_PRG, K_DST, K_MST, K_MRK, K_SUBS, K_COMPS = 0x000, 0x170, 0x1B0, 0x1E0, 0x1F8, 0x210, 0x230, 0x250, 0x290
Q_STRIDE, H_BASE, H_STRIDE = 0x90, 0x240, 0x120


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


def quant_word(desc, k):
    mode, seed = desc & 7, desc >> 3
    r = rnd(seed, k)
    pos = rnd(seed, 77) % 64
    if mode == 0:
        return 1 + r % 255
    if mode == 1:
        return r & 0xFFFF
    if mode == 2:
        return 256 + rnd(seed, 78) % 65280 if k == pos else 1 + r % 255
    if mode == 3:
        return 255
    if mode == 4:
        return 256 if k == pos else 255
    if mode == 5:
        return 0
    if mode == 6:
        return r & 0xFF
    return (r >> 8) & 0xFFFF if r & 1 else r & 0xFF


def huff_bits(desc, i):
    mode, seed = desc & 7, desc >> 3
    if i == 0:
        return rnd(seed, 99) & 0xFF
    r = rnd(seed, i)
    if mode == 0:
        return r % 2
    if mode == 1:
        return r % 3
    if mode == 2:
        return r % 9
    if mode == 3:
        return r % 17
    if mode == 4:
        return 0
    if mode == 5:
        return 255 if i == 16 else (1 if i == 15 else 0)
    if mode == 6:
        return 17
    return rnd(seed, 55) % 256 if i == 1 + seed % 16 else 0


def huff_val(desc, k):
    return rnd(desc >> 3, 200 + k) & 0xFF


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios); `lazy` = rangos cuyo
    contenido se calcula al leer (las tablas)"""

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
        # la imagen de referencia = el relleno (con las tablas) + lo escrito al preparar el caso
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
    x = s32(a + b - 1)
    q = abs(x) // abs(b)
    if (x < 0) != (b < 0):
        q = -q
    return q & M32


class Sim:
    def __init__(self, c):
        self.a = [x & M32 for x in c['a']]
        a = self.a
        self.c = c
        W = self.W = World(c)
        self.events = []
        self.marks = set()
        self.S = W.raw(0, a[P_OFF])
        self.D0 = W.raw(3, a[P_DOFF])
        self.C0 = W.raw(2, a[P_COFF])
        self.B0 = W.raw(1, a[P_BOFF])
        self.errmode = a[P_ERRMODE]
        self.bufsize = a[P_DSIZE]
        self.build_scene()
        W.snap()

    # ---------------------------------------------------------------- escena
    def build_scene(self):
        a, W, S, D0, C0 = self.a, self.W, self.S, self.D0, self.C0
        wr = W.wr
        # tablas (contenido bajo demanda en la arena D; la marca sent_table es explicita)
        dmem = W.mem[3]
        d0off = a[P_DOFF]
        for i in range(4):
            lo = GUARD + d0off + Q_STRIDE * i
            desc = a[QDESC0 + i]
            dmem.lazy.append((lo, lo + 0x80, lambda t, desc=desc: (quant_word(desc, t >> 1) >> (8 * (t & 1))) & 0xFF))
            dmem.m[lo + 0x80] = a[QSENT0 + i] & 0xFF
        for j in range(8):
            lo = GUARD + d0off + H_BASE + H_STRIDE * j
            desc = a[HDESC0 + j]
            dmem.lazy.append((lo, lo + 0x111, lambda t, desc=desc: huff_bits(desc, t) if t <= 16 else huff_val(desc, t - 0x11)))
            dmem.m[lo + 0x111] = a[HSENT0 + j] & 0xFF
        # el contenido perezoso debe reflejarse tambien en la imagen de referencia: se hace en Mem.initial()
        for i in range(8):
            p = S + K_COMPS + 0x54 * i
            k = COMP0 + 8 * i
            wr(p + 0x00, 4, a[k + CF_ID]); wr(p + 0x08, 4, a[k + CF_H]); wr(p + 0x0C, 4, a[k + CF_V]); wr(p + 0x10, 4, a[k + CF_Q])
            wr(p + 0x14, 4, a[k + CF_DC]); wr(p + 0x18, 4, a[k + CF_AC]); wr(p + 0x1C, 4, a[k + CF_WIB]); wr(p + 0x20, 4, a[k + CF_HIB])
        wr(S + K_DST + 0x00, 4, (C0 + (a[P_DSIZE] - a[P_DFREE])) & M32); wr(S + K_DST + 0x04, 4, a[P_DFREE])
        wr(S + K_MST + 0x0C, 1, a[M_CALLST]); wr(S + K_MST + 0x0D, 1, a[M_ISLAST]); wr(S + K_MST + 0x10, 4, a[M_PTYPE]); wr(S + K_MST + 0x14, 4, a[M_PNUM])
        wr(S + K_MST + 0x18, 4, a[M_TOTAL]); wr(S + K_MST + 0x1C, 4, a[M_SCANNO])
        wr(S + K_MRK + 0x1C, 4, a[K_LASTRI])
        ci = S + K_CI
        wr(ci + 0x00, 4, S + K_ERR); wr(ci + 0x04, 4, 0 if a[P_NOMEM] else S + K_MEM); wr(ci + 0x08, 4, 0 if a[P_NOPROG] else S + K_PRG)
        wr(ci + 0x10, 1, a[F_ISDEC]); wr(ci + 0x14, 4, a[F_STATE]); wr(ci + 0x18, 4, S + K_DST)
        wr(ci + 0x1C, 4, a[F_WIDTH]); wr(ci + 0x20, 4, a[F_HEIGHT]); wr(ci + 0x24, 4, a[F_INCOMP]); wr(ci + 0x38, 4, a[F_PREC]); wr(ci + 0x3C, 4, a[F_NCOMP]); wr(ci + 0x40, 4, a[F_CSPACE])
        wr(ci + 0x44, 4, S + K_COMPS)
        for i in range(4):
            wr(ci + 0x48 + 4 * i, 4, 0 if (a[P_QMASK] >> i) & 1 else self.D0 + Q_STRIDE * i)
            wr(ci + 0x58 + 4 * i, 4, 0 if (a[P_DCMASK] >> i) & 1 else self.D0 + H_BASE + H_STRIDE * i)
            wr(ci + 0x68 + 4 * i, 4, 0 if (a[P_ACMASK] >> i) & 1 else self.D0 + H_BASE + H_STRIDE * (4 + i))
            wr(ci + 0xE8 + 4 * i, 4, S + K_COMPS + 0x54 * a[CUR0 + i])
        wr(ci + 0xA8, 4, a[F_NSCANS]); wr(ci + 0xAC, 4, a[F_SCANINFO])
        wr(ci + 0xB0, 1, a[F_RAWIN]); wr(ci + 0xB1, 1, a[F_ARITH]); wr(ci + 0xB2, 1, a[F_OPT])
        wr(ci + 0xBC, 4, a[F_RINT]); wr(ci + 0xC0, 4, a[F_RROWS])
        wr(ci + 0xC4, 1, a[F_JFIF]); wr(ci + 0xC5, 1, a[F_JMAJ]); wr(ci + 0xC6, 1, a[F_JMIN]); wr(ci + 0xC7, 1, a[F_DUNIT]); wr(ci + 0xC8, 2, a[F_XD]); wr(ci + 0xCA, 2, a[F_YD]); wr(ci + 0xCC, 1, a[F_ADOBE])
        wr(ci + 0xD4, 1, a[F_PROG]); wr(ci + 0xD8, 4, a[F_MAXH]); wr(ci + 0xDC, 4, a[F_MAXV]); wr(ci + 0xE4, 4, a[F_CINSCAN])
        wr(ci + 0x12C, 4, a[F_SS]); wr(ci + 0x130, 4, a[F_SE]); wr(ci + 0x134, 4, a[F_AH]); wr(ci + 0x138, 4, a[F_AL])
        wr(ci + 0x13C, 4, S + K_MST)
        # los punteros a los falsos (err, mem, dest, marker, submodulos) son direcciones del ASI del banco: ninguna funcion los lee ni los escribe y no entran en los resumenes FNV (cinfo 0..0x160 y maestro), asi que no se modelan
        wr(ci + 0x140, 4, S + K_SUBS + 8 * 6); wr(ci + 0x144, 4, S + K_SUBS + 8 * 2); wr(ci + 0x148, 4, S + K_SUBS + 8 * 5); wr(ci + 0x14C, 4, S + K_MRK)
        wr(ci + 0x150, 4, S + K_SUBS + 8 * 0); wr(ci + 0x154, 4, S + K_SUBS + 8 * 1); wr(ci + 0x158, 4, S + K_SUBS + 8 * 3); wr(ci + 0x15C, 4, S + K_SUBS + 8 * 4)
        self.ci = ci

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
        return fnv(self.bytes_of(self.ci, 0x160)), fnv(self.bytes_of(self.rd(self.ci + 0x13C), 0x20))

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

    def emit_msg(self, level):
        self.events.append([2, level & M32, self.rd(self.rd(self.ci) + 0x14), self.tag(self.ci)])

    def eob(self):
        dest = self.rd(self.ci + 0x18)
        h = fnv(self.bytes_of(self.C0, self.bufsize))
        self.events.append([3, self.tag(self.rd(dest)), self.rd(dest + 4), h, self.tag(self.ci)])
        self.wr(dest, 4, self.C0)
        self.wr(dest + 4, 4, self.bufsize)
        self.marks.add('flush')
        return self.a[P_EOBRET] & 0xFF

    def alloc(self, pool, size):
        self.events.append([4, pool, size, self.tag(self.ci)])
        return self.B0

    # ---------------------------------------------------------------- emision
    def emit_byte(self, v):
        d = self.rd(self.ci + 0x18)
        p = self.rd(d)
        self.wr(p, 1, v & 0xFF)
        nxt = (self.rd(d) + 1) & M32
        left = (self.rd(d + 4) - 1) & M32
        self.wr(d, 4, nxt)
        self.wr(d + 4, 4, left)
        if left == 0:
            if (self.eob() & 0xFF) == 0:
                self.marks.add('cant_suspend')
                self.errx(0x18)

    def emit_marker(self, m):
        self.emit_byte(0xFF)
        self.emit_byte(m)

    def emit_2bytes(self, v):
        self.emit_byte((v & M32) >> 8)
        self.emit_byte(v)

    # ---------------------------------------------------------------- funciones
    def emit_dqt(self, index):
        index = s32(index)
        ptr = self.rd(self.ci + 0x48 + 4 * index)
        if ptr == 0:
            self.marks.add('null_table')
            self.errx(0x34, index)
        prec = 0
        for i in range(64):
            if self.rd(ptr + 2 * i, 2) > 255:
                prec = 1
        self.marks.add('prec1' if prec else 'prec0')
        if self.rd(ptr + 0x80, 1) == 0:
            self.marks.add('table_new')
            self.emit_marker(0xDB)
            self.emit_2bytes(0x83 if prec else 0x43)
            self.emit_byte(index + (prec << 4))
            for i in range(64):
                q = self.rd(ptr + 2 * NATURAL[i], 2)
                if prec:
                    self.emit_byte(q >> 8)
                self.emit_byte(q)
            self.wr(ptr + 0x80, 1, 1)
        else:
            self.marks.add('table_sent')
        return prec

    def emit_dht(self, index, is_ac):
        index = s32(index)
        if is_ac:
            ptr = self.rd(self.ci + 0x68 + 4 * index)
            index += 0x10
            self.marks.add('ac')
        else:
            ptr = self.rd(self.ci + 0x58 + 4 * index)
            self.marks.add('dc')
        if ptr == 0:
            self.marks.add('null_table')
            self.errx(0x32, index)
        if self.rd(ptr + 0x111, 1) == 0:
            self.marks.add('table_new')
            self.emit_marker(0xC4)
            length = sum(self.rd(ptr + i, 1) for i in range(1, 17))
            if length > 256:
                self.marks.add('over256')
            self.emit_2bytes(length + 19)
            self.emit_byte(index)
            for i in range(1, 17):
                self.emit_byte(self.rd(ptr + i, 1))
            for i in range(length):
                self.emit_byte(self.rd(ptr + 0x11 + i, 1))
            self.wr(ptr + 0x111, 1, 1)
        else:
            self.marks.add('table_sent')

    def emit_dri(self):
        self.emit_marker(0xDD)
        self.emit_2bytes(4)
        self.emit_2bytes(self.rd(self.ci + 0xBC))

    def emit_sof(self, code):
        ci = self.ci
        self.emit_marker(code & 0xFF)
        n = s32(self.rd(ci + 0x3C))
        self.emit_2bytes(n * 3 + 8)
        h, w = s32(self.rd(ci + 0x20)), s32(self.rd(ci + 0x1C))
        if h > 65535 or w > 65535:
            self.marks.add('too_big')
            self.errx(0x29, 65535)
        self.emit_byte(self.rd(ci + 0x38))
        self.emit_2bytes(self.rd(ci + 0x20))
        self.emit_2bytes(self.rd(ci + 0x1C))
        self.emit_byte(self.rd(ci + 0x3C))
        i = 0
        while i < s32(self.rd(ci + 0x3C)):
            comp = self.rd(ci + 0x44) + 0x54 * i
            self.emit_byte(self.rd(comp))
            self.emit_byte(((self.rd(comp + 8) & 0xFF) << 4) + (self.rd(comp + 0xC) & 0xFF))
            self.emit_byte(self.rd(comp + 0x10))
            i += 1
        self.marks.add('sof_n%d' % max(0, min(n, 5)))

    def emit_sos(self):
        ci = self.ci
        self.emit_marker(0xDA)
        cs = s32(self.rd(ci + 0xE4))
        self.emit_2bytes(2 * cs + 6)
        self.emit_byte(cs)
        prog = self.rd(ci + 0xD4, 1)
        for i in range(cs):
            comp = self.rd(ci + 0xE8 + 4 * i)
            self.emit_byte(self.rd(comp))
            td, ta = s32(self.rd(comp + 0x14)), s32(self.rd(comp + 0x18))
            if prog:
                self.marks.add('progressive')
                if s32(self.rd(ci + 0x12C)) == 0:
                    ta = 0
                    if s32(self.rd(ci + 0x134)) != 0 and not self.rd(ci + 0xB1, 1):
                        td = 0
                else:
                    td = 0
            self.emit_byte(((td & 0xFF) << 4) + (ta & 0xFF))
        self.emit_byte(self.rd(ci + 0x12C))
        self.emit_byte(self.rd(ci + 0x130))
        self.emit_byte(((self.rd(ci + 0x134) & 0xFF) << 4) + (self.rd(ci + 0x138) & 0xFF))
        self.marks.add('sos_n%d' % max(0, min(cs, 5)))

    def emit_jfif(self):
        ci = self.ci
        self.emit_marker(0xE0)
        self.emit_2bytes(16)
        for b in (0x4A, 0x46, 0x49, 0x46, 0):
            self.emit_byte(b)
        self.emit_byte(self.rd(ci + 0xC5, 1)); self.emit_byte(self.rd(ci + 0xC6, 1)); self.emit_byte(self.rd(ci + 0xC7, 1))
        self.emit_2bytes(self.rd(ci + 0xC8, 2)); self.emit_2bytes(self.rd(ci + 0xCA, 2))
        self.emit_byte(0); self.emit_byte(0)

    def emit_adobe(self):
        ci = self.ci
        self.emit_marker(0xEE)
        self.emit_2bytes(14)
        for b in (0x41, 0x64, 0x6F, 0x62, 0x65):
            self.emit_byte(b)
        self.emit_2bytes(100); self.emit_2bytes(0); self.emit_2bytes(0)
        cs = s32(self.rd(ci + 0x40))
        self.emit_byte(1 if cs == 3 else (2 if cs == 5 else 0))
        self.marks.add('adobe_cs%d' % (cs if cs in (3, 5) else 0))

    def write_file_header(self):
        ci = self.ci
        marker = self.rd(ci + 0x14C)
        self.emit_marker(0xD8)
        self.wr(marker + 0x1C, 4, 0)
        j, ad = self.rd(ci + 0xC4, 1) != 0, self.rd(ci + 0xCC, 1) != 0
        self.marks.add('jfif' if j else 'no_jfif')
        self.marks.add('adobe' if ad else 'no_adobe')
        if j:
            self.emit_jfif()
        if ad:
            self.emit_adobe()

    def write_frame_header(self):
        ci = self.ci
        prec = 0
        n = s32(self.rd(ci + 0x3C))
        for i in range(n):
            comp = self.rd(ci + 0x44) + 0x54 * i
            prec += self.emit_dqt(self.rd(comp + 0x10))
        arith, prog, dprec = self.rd(ci + 0xB1, 1) != 0, self.rd(ci + 0xD4, 1) != 0, s32(self.rd(ci + 0x38))
        if not arith and not prog and dprec == 8:
            base = True
            for i in range(n):
                comp = self.rd(ci + 0x44) + 0x54 * i
                if s32(self.rd(comp + 0x14)) > 1 or s32(self.rd(comp + 0x18)) > 1:
                    base = False
            if prec != 0 and base:
                base = False
                self.marks.add('tables16')
                self.wr(self.rd(ci) + 0x14, 4, 0x4B)
                self.emit_msg(0)
        else:
            base = False
        code = 0xC9 if arith else (0xC2 if prog else (0xC0 if base else 0xC1))
        self.marks.add('sof%02x' % code)
        self.emit_sof(code)

    def write_file_trailer(self):
        self.emit_marker(0xD9)

    def jinit_marker_writer(self):
        m = self.alloc(1, 0x20)
        self.wr(self.ci + 0x14C, 4, m)
        for off, v in ((0, 0x5C60B0), (4, 0x5C6100), (8, 0x5C61E0), (0xC, 0x5C62D0), (0x10, 0x5C62F0), (0x14, 0x5C6030), (0x18, 0x5C6070), (0x1C, 0)):
            self.wr(m + off, 4, v)

    def initial_setup(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        h, w = rd(ci + 0x20), rd(ci + 0x1C)
        if h == 0 or w == 0 or s32(rd(ci + 0x3C)) <= 0 or s32(rd(ci + 0x24)) <= 0:
            self.marks.add('empty_image')
            self.errx(0x20)
        if s32(rd(ci + 0x20)) > 0xFFDC or s32(rd(ci + 0x1C)) > 0xFFDC:
            self.marks.add('image_too_big')
            self.errx(0x29, 0xFFDC)
        if s32(rd(ci + 0x38)) != 8:
            self.marks.add('bad_precision')
            self.errx(0x0F, rd(ci + 0x38))
        if s32(rd(ci + 0x3C)) > 4:
            self.marks.add('too_many_comps')
            self.errx(0x1A, rd(ci + 0x3C), 4)
        wr(ci + 0xD8, 4, 1)
        wr(ci + 0xDC, 4, 1)
        i = 0
        while i < s32(rd(ci + 0x3C)):
            comp = rd(ci + 0x44) + 0x54 * i
            hh, vv = s32(rd(comp + 8)), s32(rd(comp + 0xC))
            if hh <= 0 or hh > 4 or vv <= 0 or vv > 4:
                self.marks.add('bad_sampling')
                self.errx(0x12)
            hh, vv = s32(rd(comp + 8)), s32(rd(comp + 0xC))
            mh = s32(rd(ci + 0xD8))
            wr(ci + 0xD8, 4, mh if mh > hh else hh)
            mv = s32(rd(ci + 0xDC))
            wr(ci + 0xDC, 4, mv if mv > vv else vv)
            i += 1
        i = 0
        while i < s32(rd(ci + 0x3C)):
            comp = rd(ci + 0x44) + 0x54 * i
            wr(comp + 4, 4, i)
            wr(comp + 0x24, 4, 8)
            hw = (s32(rd(comp + 8)) * s32(rd(ci + 0x1C))) & M32
            wr(comp + 0x1C, 4, jdiv(hw, (rd(ci + 0xD8) << 3) & M32))
            vh = (s32(rd(comp + 0xC)) * s32(rd(ci + 0x20))) & M32
            wr(comp + 0x20, 4, jdiv(vh, (rd(ci + 0xDC) << 3) & M32))
            hw = (s32(rd(comp + 8)) * s32(rd(ci + 0x1C))) & M32
            wr(comp + 0x28, 4, jdiv(hw, rd(ci + 0xD8)))
            vh = (s32(rd(comp + 0xC)) * s32(rd(ci + 0x20))) & M32
            wr(comp + 0x2C, 4, jdiv(vh, rd(ci + 0xDC)))
            wr(comp + 0x30, 1, 1)
            i += 1
        wr(ci + 0xE0, 4, jdiv(rd(ci + 0x20), (rd(ci + 0xDC) << 3) & M32))

    def select_scan(self):
        ci = self.ci
        n = s32(self.rd(ci + 0x3C))
        if n > 4:
            self.marks.add('select_too_many')
            self.errx(0x1A, self.rd(ci + 0x3C), 4)
        self.wr(ci + 0xE4, 4, self.rd(ci + 0x3C))
        i = 0
        while i < s32(self.rd(ci + 0x3C)):
            self.wr(ci + 0xE8 + 4 * i, 4, (self.rd(ci + 0x44) + 0x54 * i) & M32)
            i += 1
        self.wr(ci + 0x12C, 4, 0)
        self.wr(ci + 0x130, 4, 63)
        self.wr(ci + 0x134, 4, 0)
        self.wr(ci + 0x138, 4, 0)

    def per_scan(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        cs = s32(rd(ci + 0xE4))
        if cs == 1:
            self.marks.add('scan_single')
            comp = rd(ci + 0xE8)
            wr(ci + 0xF8, 4, rd(comp + 0x1C))
            wr(ci + 0xFC, 4, rd(comp + 0x20))
            v = rd(comp + 0xC)
            rem = rd(comp + 0x20) % v
            for off, val in ((0x34, 1), (0x38, 1), (0x3C, 1), (0x40, 8), (0x44, 1)):
                wr(comp + off, 4, val)
            wr(comp + 0x48, 4, v if rem == 0 else rem)
            wr(ci + 0x100, 4, 1)
            wr(ci + 0x104, 4, 0)
        else:
            self.marks.add('scan_multi')
            if cs <= 0 or cs > 4:
                self.marks.add('scan_bad_count')
                self.errx(0x1A, rd(ci + 0xE4), 4)
            wr(ci + 0xF8, 4, jdiv(rd(ci + 0x1C), (rd(ci + 0xD8) << 3) & M32))
            wr(ci + 0xFC, 4, jdiv(rd(ci + 0x20), (rd(ci + 0xDC) << 3) & M32))
            wr(ci + 0x100, 4, 0)
            i = 0
            while i < s32(rd(ci + 0xE4)):
                comp = rd(ci + 0xE8 + 4 * i)
                hs, vs = rd(comp + 8), rd(comp + 0xC)
                wr(comp + 0x40, 4, (hs << 3) & M32)
                crem = rd(comp + 0x1C) % hs
                mb = (vs * hs) & M32
                wr(comp + 0x34, 4, hs)
                wr(comp + 0x38, 4, vs)
                wr(comp + 0x3C, 4, mb)
                wr(comp + 0x44, 4, hs if crem == 0 else crem)
                rrem = rd(comp + 0x20) % vs
                wr(comp + 0x48, 4, vs if rrem == 0 else rrem)
                if s32(rd(ci + 0x100) + mb) > 10:
                    self.marks.add('bad_mcu_size')
                    self.errx(0x0D)
                while s32(mb) > 0:
                    b = rd(ci + 0x100)
                    wr(ci + 0x104 + 4 * b, 4, i)
                    wr(ci + 0x100, 4, (rd(ci + 0x100) + 1) & M32)
                    mb = (mb - 1) & M32
                i += 1
        rr = s32(rd(ci + 0xC0))
        if rr > 0:
            nominal = s32(rd(ci + 0xF8) * rr)
            wr(ci + 0xBC, 4, nominal if nominal < 0xFFFF else 0xFFFF)
            self.marks.add('restart_rows')

    def start1(self, ident):
        ch, mh = self.state_hash()
        self.events.append([10 + ident, self.tag(self.ci), 0, ch, mh])

    def start2(self, ident, mode):
        ch, mh = self.state_hash()
        self.events.append([10 + ident, self.tag(self.ci), mode & M32, ch, mh])

    def write_hook(self, ident):
        ch, mh = self.state_hash()
        self.events.append([ident, self.tag(self.ci), ch, mh])

    def prepare_for_pass(self):
        ci = self.ci
        rd, wr = self.rd, self.wr
        master = rd(ci + 0x13C)
        pt = rd(master + 0x10)
        stage = pt if pt in (0, 1, 2) else 3
        self.marks.add('pass%d' % stage)
        if stage == 0:
            self.select_scan()
            self.per_scan()
            if not rd(ci + 0xB0, 1):
                self.start1(0); self.start1(1); self.start2(2, 0)
            self.start1(3)
            opt = rd(ci + 0xB2, 1)
            self.start2(4, opt)
            self.start2(5, 0 if s32(rd(master + 0x18)) <= 1 else 3)
            self.start2(6, 0)
            wr(master + 0x0C, 1, 1 if opt == 0 else 0)
        elif stage in (1, 2):
            second = stage == 2
            if stage == 1:
                self.select_scan()
                self.per_scan()
                if s32(rd(ci + 0x12C)) != 0 or s32(rd(ci + 0x134)) == 0 or rd(ci + 0xB1, 1) != 0:
                    self.marks.add('huff_opt')
                    self.start2(4, 1)
                    self.start2(5, 2)
                    wr(master + 0x0C, 1, 0)
                else:
                    self.marks.add('dc_refine')
                    wr(master + 0x10, 4, 2)
                    wr(master + 0x14, 4, (rd(master + 0x14) + 1) & M32)
                    second = True
            if second:
                if not rd(ci + 0xB2, 1):
                    self.select_scan()
                    self.per_scan()
                self.start2(4, 0)
                self.start2(5, 2)
                if rd(master + 0x1C) == 0:
                    self.marks.add('first_scan')
                    self.write_hook(20)
                self.write_hook(21)
                wr(master + 0x0C, 1, 0)
        else:
            self.marks.add('bad_pass')
            self.errx(0x30)
        total = rd(master + 0x18)
        wr(master + 0x0D, 1, 1 if rd(master + 0x14) == (total - 1) & M32 else 0)
        prog = rd(ci + 0x08)
        if prog:
            wr(prog + 0x0C, 4, rd(master + 0x14))
            wr(prog + 0x10, 4, rd(master + 0x18))
            self.marks.add('progress')

    def pass_startup(self):
        ci = self.ci
        self.wr(self.rd(ci + 0x13C) + 0x0C, 1, 0)
        self.write_hook(20)
        self.write_hook(21)

    def jinit_master(self, transcode):
        ci = self.ci
        rd, wr = self.rd, self.wr
        m = self.alloc(1, 0x20)
        wr(ci + 0x13C, 4, m)
        wr(m, 4, 0x5C6850); wr(m + 4, 4, 0x5C69E0); wr(m + 8, 4, 0x5C6A10)
        wr(m + 0x0D, 1, 0)
        self.initial_setup()
        if rd(ci + 0xAC) != 0:
            self.marks.add('scan_info')
            self.errx(0x30)
        else:
            wr(ci + 0xD4, 1, 0)
            wr(ci + 0xA8, 4, 1)
        if rd(ci + 0xD4, 1):
            wr(ci + 0xB2, 1, 1)
        opt = rd(ci + 0xB2, 1)
        if transcode & 0xFF:
            self.marks.add('transcode')
            wr(m + 0x10, 4, 2 if opt == 0 else 1)
        else:
            wr(m + 0x10, 4, 0)
        wr(m + 0x1C, 4, 0)
        wr(m + 0x14, 4, 0)
        if rd(ci + 0xB2, 1):
            wr(m + 0x18, 4, (rd(ci + 0xA8) << 1) & M32)
            self.marks.add('two_passes')
        else:
            wr(m + 0x18, 4, rd(ci + 0xA8))

    def jpeg_abort(self):
        ci = self.ci
        mem = self.rd(ci + 4)
        if mem:
            self.events.append([5, 1, self.tag(ci)])
            self.wr(ci + 0x14, 4, 0xDEAD0002)
            self.wr(ci + 0x10C, 4, 0xDEAD0003)
            if self.rd(ci + 0x10, 1):
                self.marks.add('decompressor')
                self.wr(ci + 0x14, 4, 200)
                self.wr(ci + 0x10C, 4, 0)
            else:
                self.marks.add('compressor')
                self.wr(ci + 0x14, 4, 100)
        else:
            self.marks.add('no_mem')

    def jpeg_destroy(self):
        ci = self.ci
        mem = self.rd(ci + 4)
        if mem:
            self.events.append([6, self.tag(ci)])
            self.wr(ci + 0x14, 4, 0xDEAD0001)
            self.marks.add('has_mem')
        else:
            self.marks.add('no_mem')
        self.wr(ci + 4, 4, 0)
        self.wr(ci + 0x14, 4, 0)

    def alloc_table(self, size, off):
        t = self.alloc(0, size)
        self.wr(t + off, 1, 0)
        return t

    # ---------------------------------------------------------------- ejecucion de un caso
    def run(self, fn):
        a = self.a
        ret = None
        escaped = False
        try:
            if fn == 0:
                ret = self.emit_dqt(a[P_ARG])
            elif fn == 1:
                self.emit_dht(a[P_ARG], (a[P_ARG2] & 0xFF) != 0)
            elif fn == 2:
                self.emit_dri()
            elif fn == 3:
                self.emit_sof(a[P_ARG2] & 0xFF)
            elif fn == 4:
                self.emit_sos()
            elif fn == 5:
                self.emit_jfif()
            elif fn == 6:
                self.emit_adobe()
            elif fn == 7:
                self.write_file_header()
            elif fn == 8:
                self.write_frame_header()
            elif fn == 9:
                self.write_file_trailer()
            elif fn == 10:
                self.jinit_marker_writer()
            elif fn == 11:
                self.prepare_for_pass()
            elif fn == 12:
                self.pass_startup()
            elif fn == 13:
                self.jinit_master(a[P_ARG])
            elif fn == 14:
                self.jpeg_abort()
            elif fn == 15:
                self.jpeg_destroy()
            elif fn == 16:
                ret = self.tag(self.alloc_table(0x82, 0x80))
            elif fn == 17:
                ret = self.tag(self.alloc_table(0x112, 0x111))
        except Escape:
            escaped = True
            ret = None
        return ret, escaped


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'scene_aligned', 'scene_unaligned', 'flush', 'cant_suspend', 'escape', 'err_return', 'dest_free0', 'dest_tiny']
EXTRA = {
    0: ['prec0', 'prec1', 'table_new', 'table_sent', 'null_table', 'idx_huff'],
    1: ['dc', 'ac', 'table_new', 'table_sent', 'null_table', 'over256'],
    2: [], 3: ['too_big', 'sof_n0', 'sof_n1', 'sof_n3', 'sof_n4', 'sof_n5'], 4: ['progressive', 'sos_n0', 'sos_n1', 'sos_n3', 'sos_n4'],
    5: [], 6: ['adobe_cs0', 'adobe_cs3', 'adobe_cs5'],
    7: ['jfif', 'no_jfif', 'adobe', 'no_adobe'],
    8: ['tables16', 'sofc0', 'sofc1', 'sofc2', 'sofc9', 'prec1', 'prec0', 'null_table'],
    9: [], 10: [],
    11: ['pass0', 'pass1', 'pass2', 'pass3', 'huff_opt', 'first_scan', 'scan_single', 'scan_multi', 'progress', 'restart_rows', 'select_too_many', 'bad_pass'],
    12: [],
    13: ['empty_image', 'image_too_big', 'bad_precision', 'too_many_comps', 'bad_sampling', 'scan_info', 'transcode', 'two_passes'],
    14: ['decompressor', 'compressor', 'no_mem'], 15: ['has_mem', 'no_mem'], 16: [], 17: [],
}
EMITTERS = set(range(10))
KEYS = {fn: COMMON[:10] + (['flush'] if fn in EMITTERS else []) + EXTRA[fn] for fn in range(len(NAMES))}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    sim = Sim(c)
    ret, escaped = sim.run(fn)
    marks = set(sim.marks)
    marks.update(['kind%d' % a[P_KIND], 'fill%d' % a[P_FILL], 'scene_unaligned' if a[P_OFF] % 4 else 'scene_aligned'])
    if a[P_DFREE] == 0:
        marks.add('dest_free0')
    if a[P_DSIZE] <= 3:
        marks.add('dest_tiny')
    if fn == 0 and a[P_ARG] >= 4:
        marks.add('idx_huff')
    if fn == 1 and False:
        pass
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX de EmitDqt (valor devuelto, ya comprobado)"""
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
