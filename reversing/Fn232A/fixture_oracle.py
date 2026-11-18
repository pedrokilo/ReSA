"""Oraculo independiente (Python) de Fn232A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (`dumpbin /disasm` sobre el gta-sa.exe canonico), lo que debe observarse de las
15 funciones del lote 232A (libjpeg 6b embebida en el EXE) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo
reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial y lleva copiadas del EXE las tablas de solo lectura que leen jpeg_set_defaults (cuantizacion 0x86B450 y
0x86B350 y las cuatro tablas Huffman estandar 0x86B6E4/0x86B62C/0x86B60C/0x86B550 con sus valores). Cada funcion se escribe como un PROGRAMA sobre un modelo de memoria (arenas), en el orden de lecturas y
escrituras del EXE: lo que importa cuando un falso cambia la memoria entre dos lecturas. Las lecturas fuera de las arenas lanzan una excepcion (un caso invalido del generador).

Desensamblado (resumen; direcciones del EXE). cinfo de compresion: +0 err, +4 mem, +8 progress, +0x14 estado, +0x18 dest, +0x24 input_components, +0x28 in_color_space, +0x38 precision, +0x3C num_components,
+0x40 jpeg_color_space, +0x44 comp_info (0x54 bytes por componente), +0x48 quant_tbl_ptrs[4], +0x58 dc_huff[4], +0x68 ac_huff[4], +0x78/0x88/0x98 arith, +0xA8..+0xCC opciones, +0xD8/+0xDC max_h/max_v,
+0x144 prep, +0x154 downsample. cinfo de descompresion: +0x14 estado, +0x24 num_components, +0x28 jpeg_color_space, +0x2C out_color_space, +0x40 buffered_image, +0x41 raw_data_out, +0x4A quantize_colors,
+0x5C output_width, +0x60 output_height, +0x64 out_color_components, +0x68 output_components, +0x78 output_scanline, +0x7C input_scan_number, +0x84 output_scan_number, +0xC4 comp_info, +0x180 master,
+0x184 main, +0x188 coef, +0x190 inputctl, +0x19C idct, +0x1A4 cconvert.
Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF

# Tablas de solo lectura del EXE (gta-sa.exe canonico): cuantizacion 0x86B450 (luminancia) y 0x86B350 (crominancia); Huffman estandar: DC0 0x86B6E4/0x86B6F8, AC0 0x86B62C/0x86B640, DC1 0x86B60C/0x86B620, AC1 0x86B550/0x86B568 (bits[17], valores)
ROM = {"QT0": [16, 11, 10, 16, 24, 40, 51, 61, 12, 12, 14, 19, 26, 58, 60, 55, 14, 13, 16, 24, 40, 57, 69, 56, 14, 17, 22, 29, 51, 87, 80, 62, 18, 22, 37, 56, 68, 109, 103, 77, 24, 35, 55, 64, 81, 104, 113, 92, 49, 64, 78, 87, 103, 121, 120, 101, 72, 92, 95, 98, 112, 100, 103, 99], "QT1": [17, 18, 24, 47, 99, 99, 99, 99, 18, 21, 26, 66, 99, 99, 99, 99, 24, 26, 56, 99, 99, 99, 99, 99, 47, 66, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99, 99], "HUFF": [[[0, 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0], [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]], [[0, 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 125], [1, 2, 3, 0, 4, 17, 5, 18, 33, 49, 65, 6, 19, 81, 97, 7, 34, 113, 20, 50, 129, 145, 161, 8, 35, 66, 177, 193, 21, 82, 209, 240, 36, 51, 98, 114, 130, 9, 10, 22, 23, 24, 25, 26, 37, 38, 39, 40, 41, 42, 52, 53, 54, 55, 56, 57, 58, 67, 68, 69, 70, 71, 72, 73, 74, 83, 84, 85, 86, 87, 88, 89, 90, 99, 100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118, 119, 120, 121, 122, 131, 132, 133, 134, 135, 136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212, 213, 214, 215, 216, 217, 218, 225, 226, 227, 228, 229, 230, 231, 232, 233, 234, 241, 242, 243, 244, 245, 246, 247, 248, 249, 250]], [[0, 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0], [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11]], [[0, 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 119], [0, 1, 2, 3, 17, 4, 5, 33, 49, 6, 18, 65, 81, 7, 97, 113, 19, 34, 50, 129, 8, 20, 66, 145, 161, 177, 193, 9, 35, 51, 82, 240, 21, 98, 114, 209, 10, 22, 36, 52, 225, 37, 241, 23, 24, 25, 26, 38, 39, 40, 41, 42, 53, 54, 55, 56, 57, 58, 67, 68, 69, 70, 71, 72, 73, 74, 83, 84, 85, 86, 87, 88, 89, 90, 99, 100, 101, 102, 103, 104, 105, 106, 115, 116, 117, 118, 119, 120, 121, 122, 130, 131, 132, 133, 134, 135, 136, 137, 138, 146, 147, 148, 149, 150, 151, 152, 153, 154, 162, 163, 164, 165, 166, 167, 168, 169, 170, 178, 179, 180, 181, 182, 183, 184, 185, 186, 194, 195, 196, 197, 198, 199, 200, 201, 202, 210, 211, 212, 213, 214, 215, 216, 217, 218, 226, 227, 228, 229, 230, 231, 232, 233, 234, 242, 243, 244, 245, 246, 247, 248, 249, 250]]]}

NAMES = ["ExeLibJpeg::jpeg_add_quant_table", "ExeLibJpeg::jpeg_set_colorspace", "ExeLibJpeg::jpeg_default_colorspace", "ExeLibJpeg::jpeg_set_defaults", "ExeLibJpeg::jinit_c_prep_controller",
         "ExeLibJpeg::expand_right_edge", "ExeLibJpeg::jinit_downsampler", "ExeLibJpeg::jpeg_read_scanlines", "ExeLibJpeg::jpeg_start_decompress", "ExeLibJpeg::term_destination",
         "ExeLibJpeg::jinit_d_coef_controller", "ExeLibJpeg::build_ycc_rgb_table", "ExeLibJpeg::null_convert", "ExeLibJpeg::jinit_color_deconverter", "ExeLibJpeg::jinit_inverse_dct"]
ENTRIES = (0x5C6BF0, 0x5C7130, 0x5C7420, 0x5C7590, 0x5C7920, 0x5C79F0, 0x5C7DF0, 0x5C8690, 0x5C87E0, 0x5C88F0, 0x5C8E20, 0x5C8EE0, 0x5C90D0, 0x5C9350, 0x5C9710)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(ADDQ, SETCS, DEFCS, SETDEF, PREP, EXPAND, DOWN, READSL, STARTD, TERM, COEFD, YCC, NULL, DECONV, IDCT) = range(15)
KIND = [ADDQ, SETCS, DEFCS, SETDEF, PREP, EXPAND, DOWN, READSL, STARTD, TERM, COEFD, YCC, NULL, DECONV, IDCT]
RET_EAX = {7}      # EAX completo (filas leidas)
RET_AL = {8}       # solo AL (boolean)


class Bad(Exception):
    """caso invalido: el modelo desreferencia memoria desconocida, divide por cero o llama a un puntero que no es un falso"""


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
    return v - (1 << 32) if v & 0x80000000 else v


def idiv(a, b):
    """cdq + idiv: dividendo y divisor de 32 bits con signo; (cociente, resto) truncados hacia cero; excepcion de hardware si el divisor es 0 o el cociente no cabe"""
    a, b = s32(a), s32(b)
    if b == 0:
        raise Bad('division por cero')
    q = abs(a) // abs(b)
    if (a < 0) != (b < 0):
        q = -q
    if not -(1 << 31) <= q < (1 << 31):
        raise Bad('desbordamiento de idiv')
    return q & M32, (a - b * q) & M32


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba)"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[8], a[9 + i]) for i in range(4))
        self.glob = {}
        self.fsbytes = set()      # direcciones de los bytes que guardan un puntero a funcion falso (simbolico): ninguna escritura del modelo debe tocarlos
        self.putmap = {}          # byte -> (inicio, ancho) de la escritura del caso que lo fijo

    def put(self, dest, off, width, spec):
        """escritura del caso: solo se admite solapar otra identica (mismo inicio y ancho); un puntero a funcion falso queda marcado"""
        for k in range(width):
            prev = self.putmap.get((dest, off + k))
            if prev is not None and prev != (off, width):
                raise Bad('escrituras del caso solapadas')
        for k in range(width):
            self.putmap[(dest, off + k)] = (off, width)
        if dest == 4:
            self.glob[off] = self.val(spec)
            return
        isfs = width == 4 and (spec & M32) >> 8 == 0xF00000
        if isfs and (spec & 0xFF) > 13:
            raise Bad('funcion falsa inexistente')
        for k in range(width):
            a = self.bases[dest] + GUARD + off + k
            if isfs:
                self.fsbytes.add(a)
            else:
                self.fsbytes.discard(a)
        self.mem[dest].wr(off, width, self.val(spec) if width == 4 else spec)

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def val(self, spec):
        """especificacion de valor del caso -> valor real (los falsos por puntero quedan simbolicos)"""
        spec &= M32
        if 0xA <= (spec >> 28) <= 0xD:
            return self.raw((spec >> 28) - 0xA, spec & 0x0FFFFFFF)
        return spec

    def rd(self, addr, n=4):
        addr &= M32
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                if addr + n > b + N:
                    raise Bad('lectura fuera de la arena %x' % addr)
                return self.mem[i].rd(addr - b - GUARD, n)
        if addr in self.glob and n == 4:
            return self.glob[addr]
        raise Bad('lectura de memoria desconocida %x' % addr)

    def wr(self, addr, n, v):
        addr &= M32
        if self.fsbytes and any(((addr + k) & M32) in self.fsbytes for k in range(n)):
            raise Bad('escritura sobre un puntero a funcion falso')
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                if addr + n > b + N:
                    raise Bad('escritura fuera de la arena %x' % addr)
                self.mem[i].wr(addr - b - GUARD, n, v)
                return
        if addr in self.glob and n == 4:
            self.glob[addr] = v & M32
            return
        raise Bad('escritura en memoria desconocida %x' % addr)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
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
        self.gref = dict(self.glob)


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'obj_aligned', 'obj_unaligned']
_EXTRA = {
    0: ['state_ok', 'state_bad', 'idx_bad', 'alloc', 'existing', 'clamp_lo', 'clamp_hi', 'clamp_base', 'baseline', 'poke_applied'],
    1: ['state_ok', 'state_bad', 'unknown', 'gray', 'rgb', 'ycc', 'cmyk', 'ycck', 'bad_cs', 'nc_bad', 'poke_applied'],
    2: ['state_ok', 'state_bad', 'in_unknown', 'in_gray', 'in_rgb', 'in_ycc', 'in_cmyk', 'in_ycck', 'in_bad', 'poke_applied'],
    3: ['state_ok', 'state_bad', 'comp_alloc', 'comp_given', 'quant_alloc', 'quant_given', 'huff_alloc', 'huff_given', 'precision_high', 'poke_applied'],
    4: ['full_err', 'ctx_err', 'nc0', 'loop', 'poke_applied'],
    5: ['rows0', 'numcols0', 'expand', 'multi_row'],
    6: ['ccir_err', 'nc0', 'full', 'h2v1', 'h2v2', 'integral', 'fract_err', 'poke_applied'],
    7: ['state_ok', 'state_bad', 'warn', 'progress', 'no_progress', 'rows_returned', 'poke_applied'],
    8: ['ca', 'cb', 'cc', 'bad_state', 'buffered', 'multi_err', 'dummy_loop', 'raw_out', 'poke_applied'],
    9: ['write', 'nowrite', 'write_short', 'ferror', 'poke_applied'],
    10: ['full_err', 'ok', 'poke_applied'],
    11: ['run', 'poke_applied'],
    12: ['rows0', 'rows_many', 'nc0', 'width0', 'nc_multi'],
    13: ['gray_out', 'rgb_out', 'cmyk_out', 'default_out', 'in_bad', 'conv_err', 'build_tables', 'needed_clear', 'quantize', 'poke_applied'],
    14: ['nc0', 'loop', 'poke_applied'],
}
KEYS = {}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[_fn]


# aridad de cada falso por puntero (id 0xC100 + k), igual que FakeFn en LiveComparison.hpp
FAKE_ARITY = [1, 2, 1, 3, 3, 4, 6, 1, 1, 1, 1, 1, 4, 1]


class Ctx:
    """estado de un caso: mundo, eventos, marcas, retornos programados y efecto lateral"""

    def __init__(self, c):
        self.c = c
        self.a = [x & M32 for x in c['a']]
        self.W = World(c)
        self.events = []
        self.marks = []
        self.count = {}
        self.rets = {}
        r = [x & M32 for x in c['r']]
        for i in range(0, len(r), 3):
            self.rets[(r[i], r[i + 1])] = r[i + 2]

    def tag(self, v):
        return self.W.tagv(v)

    def fake(self, eid, args):
        """un falso: anota el evento, aplica el efecto lateral y devuelve el valor programado"""
        self.events.append([eid] + [x & M32 for x in args])
        n = self.count.get(eid, 0)
        self.count[eid] = n + 1
        a = self.a
        if a[14] and eid == a[14] and n == a[15]:
            val = a[19] if a[18] == 0 else self.W.val(a[19])
            self.W.wr(self.W.raw(a[16], a[17]), 4, val)
            self.marks.append('poke_applied')
        v = self.rets.get((eid, n))
        return self.W.val(v) if v is not None else 0

    def callp(self, word, ci, *rest):
        """llamada por puntero a una funcion falsa de la libjpeg (0xF00000kk -> id 0xC100 + kk) con (cinfo, ...)"""
        if (word >> 8) != 0xF00000 or (word & 0xFF) > 13:
            raise Bad('puntero a funcion invalido %x' % word)
        # el falso solo registra los argumentos que declara (aridad de LiveComparison.hpp: FkCi1 = 1, FkEmit = 2, ...): una funcion de la libjpeg llamada por un puntero que
        # apunta a otro falso (p. ej. emit_message hacia el falso de error_exit) deja un evento con los argumentos de ESE falso
        k = word & 0xFF
        return self.fake(0xC100 + k, ([self.tag(ci)] + list(rest))[:FAKE_ARITY[k]])

    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def errexit(self, ci, code, stores=()):
        """ERREXIT: [err] se relee en cada paso; stores = [(desplazamiento, ('v', valor) | ('m', direccion))] tras escribir el codigo en +0x14; al final error_exit = [[ci]]"""
        e = self.rd(ci)
        self.wr(e + 0x14, 4, code)
        for off, (kind, v) in stores:
            val = self.rd(v) if kind == 'm' else v
            e = self.rd(ci)
            self.wr(e + off, 4, val)
        e = self.rd(ci)
        self.callp(self.rd(e), ci)

    def alloc_small(self, ci, pool, size):
        mem = self.rd(ci + 4)
        return self.callp(self.rd(mem), ci, pool, size)


# ------------------------------------------------------------------------------------------------ funciones
def badstate(X, ci):
    X.errexit(ci, 0x14, [(0x18, ('m', ci + 0x14))])


def f_addq(X, ci, which, getbasic, scale, fb):
    if X.rd(ci + 0x14) != 100:
        X.marks.append('state_bad')
        badstate(X, ci)
    else:
        X.marks.append('state_ok')
    tn = s32(which)
    if tn < 0 or tn >= 4:
        X.marks.append('idx_bad')
        X.errexit(ci, 0x1F, [(0x18, ('v', which & M32))])
    slot = (ci + tn * 4 + 0x48) & M32
    if X.rd(slot) == 0:
        X.marks.append('alloc')
        r = X.fake(0x5C6BB0, [X.tag(ci)])
        X.wr(slot, 4, r)
    else:
        X.marks.append('existing')
    if fb & 0xFF:
        X.marks.append('baseline')
    esi = 6
    base = 0
    while True:
        for j in range(8):
            v = getbasic(base + j)
            t = s32((v * (scale & M32)) & M32)
            t = s32((t + 0x32) & M32)
            p = 0x51EB851F * t                      # imul ecx con eax = 0x51EB851F: edx:eax con signo
            edx = (p >> 32) & M32                   # parte alta (aritmetica: Python usa floor sobre enteros con signo)
            edx = (s32(edx) >> 5) & M32             # sar edx,5
            ecx = (edx >> 31) & 1                   # shr ecx,1Fh
            edx = (edx + ecx) & M32
            d = s32(edx)
            if d <= 0:
                d = 1
                X.marks.append('clamp_lo')
            elif d > 0x7FFF:
                d = 0x7FFF
                X.marks.append('clamp_hi')
            if fb & 0xFF and d > 0xFF:
                d = 0xFF
                X.marks.append('clamp_base')
            tbl = X.rd(slot)
            X.wr(tbl + esi + 2 * j - 6, 2, d & 0xFFFF)
        esi += 0x10
        base += 8
        if not esi < 0x86:
            break
    tbl = X.rd(slot)
    X.wr(tbl + 0x80, 1, 0)


def setcomp(X, comp, cid, h, v, q, d, a):
    X.wr(comp + 0, 4, cid)
    X.wr(comp + 8, 4, h)
    X.wr(comp + 0xC, 4, v)
    X.wr(comp + 0x10, 4, q)
    X.wr(comp + 0x14, 4, d)
    X.wr(comp + 0x18, 4, a)


def f_setcs(X, ci, cs):
    """0x5C7130 (jpeg_set_colorspace) y los cuerpos inlinados en 0x5C7420 (gris e YCCK)"""
    if X.rd(ci + 0x14) != 100:
        X.marks.append('state_bad')
        badstate(X, ci)
    else:
        X.marks.append('state_ok')
    X.wr(ci + 0x40, 4, cs)
    X.wr(ci + 0xC4, 1, 0)
    X.wr(ci + 0xCC, 1, 0)
    if cs > 5:
        X.marks.append('bad_cs')
        X.errexit(ci, 0x0A)
        return
    cp = lambda: X.rd(ci + 0x44)
    if cs == 1:
        X.marks.append('gray')
        X.wr(ci + 0xC4, 1, 1)
        X.wr(ci + 0x3C, 4, 1)
        setcomp(X, cp(), 1, 1, 1, 0, 0, 0)
    elif cs == 2:
        X.marks.append('rgb')
        X.wr(ci + 0x3C, 4, 3)
        X.wr(ci + 0xCC, 1, 1)
        setcomp(X, cp(), 0x52, 1, 1, 0, 0, 0)
        setcomp(X, cp() + 0x54, 0x47, 1, 1, 0, 0, 0)
        setcomp(X, cp() + 0xA8, 0x42, 1, 1, 0, 0, 0)
    elif cs == 3:
        X.marks.append('ycc')
        X.wr(ci + 0x3C, 4, 3)
        X.wr(ci + 0xC4, 1, 1)
        setcomp(X, cp(), 1, 2, 2, 0, 0, 0)
        setcomp(X, cp() + 0x54, 2, 1, 1, 1, 1, 1)
        setcomp(X, cp() + 0xA8, 3, 1, 1, 1, 1, 1)
    elif cs == 4:
        X.marks.append('cmyk')
        X.wr(ci + 0x3C, 4, 4)
        X.wr(ci + 0xCC, 1, 1)
        setcomp(X, cp(), 0x43, 1, 1, 0, 0, 0)
        setcomp(X, cp() + 0x54, 0x4D, 1, 1, 0, 0, 0)
        setcomp(X, cp() + 0xA8, 0x59, 1, 1, 0, 0, 0)
        setcomp(X, cp() + 0xFC, 0x4B, 1, 1, 0, 0, 0)
    elif cs == 5:
        X.marks.append('ycck')
        X.wr(ci + 0x3C, 4, 4)
        X.wr(ci + 0xCC, 1, 1)
        setcomp(X, cp(), 1, 2, 2, 0, 0, 0)
        setcomp(X, cp() + 0x54, 2, 1, 1, 1, 1, 1)
        setcomp(X, cp() + 0xA8, 3, 1, 1, 1, 1, 1)
        setcomp(X, cp() + 0xFC, 4, 2, 2, 0, 0, 0)
    else:
        X.marks.append('unknown')
        eax = X.rd(ci + 0x24)
        X.wr(ci + 0x3C, 4, eax)
        if s32(eax) < 1 or s32(eax) > 4:
            X.marks.append('nc_bad')
            X.errexit(ci, 0x1A, [(0x18, ('m', ci + 0x3C)), (0x1C, ('v', 4))])
        i = 0
        while s32(X.rd(ci + 0x3C)) > i:
            setcomp(X, cp() + 0x54 * i, i, 1, 1, 0, 0, 0)
            i += 1


def f_defcs(X, ci):
    ics = X.rd(ci + 0x28)
    if ics > 5:
        X.marks.append('in_bad')
        X.errexit(ci, 9)
        return
    X.marks.append(['in_unknown', 'in_gray', 'in_rgb', 'in_ycc', 'in_cmyk', 'in_ycck'][ics])
    f_setcs(X, ci, {0: 0, 1: 1, 2: 3, 3: 3, 4: 4, 5: 5}[ics])


def add_huff(X, ci, slot_off, bits, val):
    slot = ci + slot_off
    if X.rd(slot) == 0:
        X.marks.append('huff_alloc')
        r = X.fake(0x5C6BD0, [X.tag(ci)])
        X.wr(slot, 4, r)
    else:
        X.marks.append('huff_given')
    tbl = X.rd(slot)
    for k in range(4):
        X.wr(tbl + 4 * k, 4, bits[4 * k] | bits[4 * k + 1] << 8 | bits[4 * k + 2] << 16 | bits[4 * k + 3] << 24)
    X.wr(tbl + 0x10, 1, bits[16])
    n = sum(bits[1:17])
    assert 1 <= n <= 256
    tbl = X.rd(slot)
    for k in range(n):
        X.wr(tbl + 0x11 + k, 1, val[k])
    tbl = X.rd(slot)
    X.wr(tbl + 0x111, 1, 0)


def f_setdef(X, ci):
    if X.rd(ci + 0x14) != 100:
        X.marks.append('state_bad')
        badstate(X, ci)
    else:
        X.marks.append('state_ok')
    if X.rd(ci + 0x44) == 0:
        X.marks.append('comp_alloc')
        mem = X.rd(ci + 4)
        r = X.callp(X.rd(mem), ci, 0, 0x150)
        X.wr(ci + 0x44, 4, r)
    else:
        X.marks.append('comp_given')
    X.wr(ci + 0x38, 4, 8)
    # dos jpeg_add_quant_table(cinfo, k, tabla del EXE, 50, 1): la copia inlinada del estado y del indice vuelve a comprobarlos
    for k, tab in ((0, ROM['QT0']), (1, ROM['QT1'])):
        sub = ['q']
        saved = X.marks
        X.marks = []
        f_addq(X, ci, k, lambda i, t=tab: t[i], 50, 1)
        sub = X.marks
        X.marks = saved
        X.marks.append('quant_alloc' if 'alloc' in sub else 'quant_given')
        if 'poke_applied' in sub:
            X.marks.append('poke_applied')
    # std_huff_tables
    for off, (bits, val) in ((0x58, ROM['HUFF'][0]), (0x68, ROM['HUFF'][1]), (0x5C, ROM['HUFF'][2]), (0x6C, ROM['HUFF'][3])):
        add_huff(X, ci, off, bits, val)
    for k in range(16):
        X.wr(ci + 0x78 + k, 1, 0)
        X.wr(ci + 0x88 + k, 1, 1)
        X.wr(ci + 0x98 + k, 1, 5)
    high = s32(X.rd(ci + 0x38)) > 8
    X.wr(ci + 0xAC, 4, 0)
    X.wr(ci + 0xA8, 4, 0)
    X.wr(ci + 0xB0, 1, 0)
    X.wr(ci + 0xB1, 1, 0)
    X.wr(ci + 0xB2, 1, 0)
    if high:
        X.marks.append('precision_high')
        X.wr(ci + 0xB2, 1, 1)
    X.wr(ci + 0xB3, 1, 0)
    X.wr(ci + 0xB4, 4, 0)
    X.wr(ci + 0xB8, 4, 0)
    X.wr(ci + 0xBC, 4, 0)
    X.wr(ci + 0xC0, 4, 0)
    X.wr(ci + 0xC5, 1, 1)
    X.wr(ci + 0xC6, 1, 1)
    X.wr(ci + 0xC7, 1, 0)
    X.wr(ci + 0xC8, 2, 1)
    X.wr(ci + 0xCA, 2, 1)
    saved = X.marks
    X.marks = []
    f_defcs(X, ci)
    sub = X.marks
    X.marks = saved
    # las marcas de estado y de entrada de default_colorspace no se cuentan aqui (solo las de esta funcion)
    if 'poke_applied' in sub:
        X.marks.append('poke_applied')


def f_prep(X, ci, need):
    if need & 0xFF:
        X.marks.append('full_err')
        X.errexit(ci, 4)
    prep = X.alloc_small(ci, 1, 0x20)
    X.wr(ci + 0x144, 4, prep)
    X.wr(prep, 4, 0x5C76A0)
    ds = X.rd(ci + 0x154)
    if X.rd(ds + 8, 1):
        X.marks.append('ctx_err')
        X.errexit(ci, 0x30)
        return
    X.wr(prep + 4, 4, 0x5C7710)
    nc = X.rd(ci + 0x3C)
    comp = X.rd(ci + 0x44)
    if s32(nc) <= 0:
        X.marks.append('nc0')
        return
    X.marks.append('loop')
    edi = comp + 8
    ebx = prep + 8
    ebp = 0
    while True:
        eax = X.rd(edi + 0x14)
        eax = (eax * X.rd(ci + 0xD8)) & M32
        edx = X.rd(ci + 0xDC)
        eax = (eax << 3) & M32
        q, _ = idiv(eax, X.rd(edi))
        mem = X.rd(ci + 4)
        r = X.callp(X.rd(mem + 8), ci, 1, q, edx)
        X.wr(ebx, 4, r)
        ebp += 1
        ebx += 4
        edi += 0x54
        if not s32(ebp) < s32(X.rd(ci + 0x3C)):
            break


def f_expand(X, img, rows, inc, outc):
    numcols = s32((outc - inc) & M32)
    if numcols <= 0:
        X.marks.append('numcols0')
        return
    if s32(rows) <= 0:
        X.marks.append('rows0')
        return
    if numcols > 0x400:
        raise Bad('relleno enorme')
    X.marks.append('expand')
    if s32(rows) > 1:
        X.marks.append('multi_row')
    for r in range(s32(rows)):
        ptr = (X.rd(img + 4 * r) + inc) & M32
        v = X.rd(ptr - 1, 1)
        for k in range(numcols):
            X.wr(ptr + k, 1, v)


def f_down(X, ci):
    mem = X.rd(ci + 4)
    ds = X.callp(X.rd(mem), ci, 1, 0x1C)
    X.wr(ci + 0x154, 4, ds)
    X.wr(ds, 4, 0x5C79E0)
    X.wr(ds + 4, 4, 0x5C7A50)
    X.wr(ds + 8, 1, 0)
    if X.rd(ci + 0xB3, 1):
        X.marks.append('ccir_err')
        X.errexit(ci, 0x19)
    nc = X.rd(ci + 0x3C)
    comp = X.rd(ci + 0x44)
    if s32(nc) <= 0:
        X.marks.append('nc0')
        return
    edi = comp + 0xC
    ebx = ds + 0xC
    k = 0
    while True:
        ecx = X.rd(edi - 4)
        eax = X.rd(ci + 0xD8)
        done = False
        if ecx == eax:
            if X.rd(edi) == X.rd(ci + 0xDC):
                X.wr(ebx, 4, 0x5C7C10)
                X.marks.append('full')
                done = True
        if not done:
            edx = (ecx + ecx) & M32
            if edx == eax:
                if X.rd(edi) == X.rd(ci + 0xDC):
                    X.wr(ebx, 4, 0x5C7C60)
                    X.marks.append('h2v1')
                    done = True
                else:
                    edx2 = (X.rd(edi) << 1) & M32
                    if edx2 == X.rd(ci + 0xDC):
                        X.wr(ebx, 4, 0x5C7D10)
                        X.marks.append('h2v2')
                        done = True
        if not done:
            _, rem = idiv(eax, ecx)
            if rem == 0:
                _, rem2 = idiv(X.rd(ci + 0xDC), X.rd(edi))
                if rem2 == 0:
                    X.wr(ebx, 4, 0x5C7AE0)
                    X.marks.append('integral')
                    done = True
        if not done:
            X.marks.append('fract_err')
            X.errexit(ci, 0x26)
        k += 1
        ebx += 4
        edi += 0x54
        if not s32(k) < s32(X.rd(ci + 0x3C)):
            break


def f_readsl(X, ci, scan, maxl):
    if X.rd(ci + 0x14) != 0xCD:
        X.marks.append('state_bad')
        badstate(X, ci)
    else:
        X.marks.append('state_ok')
    ecx = X.rd(ci + 0x78)
    if ecx >= X.rd(ci + 0x60):
        X.marks.append('warn')
        e = X.rd(ci)
        X.wr(e + 0x14, 4, 0x7B)
        e = X.rd(ci)
        X.callp(X.rd(e + 4), ci, 0xFFFFFFFF)
        return 0
    pr = X.rd(ci + 8)
    if pr:
        X.marks.append('progress')
        X.wr(pr + 4, 4, ecx)
        pr = X.rd(ci + 8)
        X.wr(pr + 8, 4, X.rd(ci + 0x60))
        pr = X.rd(ci + 8)
        X.callp(X.rd(pr), ci)
    else:
        X.marks.append('no_progress')
    main = X.rd(ci + 0x184)
    fnp = X.rd(main + 4)
    if (fnp & 0xFF) == 12 and (fnp >> 8) == 0xF00000:
        rc = X.callp(fnp, ci, X.tag(scan), 0x7F000000, maxl)      # el falso escribe row_ctr (valor programado)
    else:
        X.callp(fnp, ci)                                           # otro falso (no escribe row_ctr: queda en 0)
        rc = 0
    X.wr(ci + 0x78, 4, (X.rd(ci + 0x78) + rc) & M32)
    if rc:
        X.marks.append('rows_returned')
    return rc


def pass_setup(X, ci):
    if X.rd(ci + 0x14) != 0xCC:
        m = X.rd(ci + 0x180)
        X.callp(X.rd(m), ci)
        X.wr(ci + 0x78, 4, 0)
        X.wr(ci + 0x14, 4, 0xCC)
    m = X.rd(ci + 0x180)
    steps = 0
    while X.rd(m + 8, 1):
        X.marks.append('dummy_loop')
        steps += 1
        if steps > 3:
            raise Bad('pase de relleno sin fin')
        X.errexit(ci, 0x30)
        m = X.rd(ci + 0x180)
    X.wr(ci + 0x14, 4, 0xCD + (1 if X.rd(ci + 0x41, 1) else 0))
    if X.rd(ci + 0x41, 1):
        X.marks.append('raw_out')
    return 1


def f_startd(X, ci):
    if X.rd(ci + 0x14) == 0xCA:
        X.marks.append('ca')
        X.fake(0x5CCCC0, [X.tag(ci)])
        if X.rd(ci + 0x40, 1):
            X.marks.append('buffered')
            X.wr(ci + 0x14, 4, 0xCF)
            return 1
        X.wr(ci + 0x14, 4, 0xCB)
    st = X.rd(ci + 0x14)
    if st == 0xCB:
        X.marks.append('cb')
        ic = X.rd(ci + 0x190)
        if X.rd(ic + 0x10, 1):
            X.marks.append('multi_err')
            X.errexit(ci, 0x30)
        X.wr(ci + 0x84, 4, X.rd(ci + 0x7C))
        return pass_setup(X, ci)
    if st == 0xCC:
        X.marks.append('cc')
    else:
        X.marks.append('bad_state')
        badstate(X, ci)
    return pass_setup(X, ci)


def f_term(X, ci):
    dest = X.rd(ci + 0x18)
    esi = (0x1000 - X.rd(dest + 4)) & M32
    if esi != 0:
        X.marks.append('write')
        f = X.rd(dest + 0x14)
        buf = X.rd(dest + 0x18)
        r = X.fake(0x823674, [X.tag(buf), 1, esi, X.tag(f)])
        if r != esi:
            X.marks.append('write_short')
            X.errexit(ci, 0x25)
    else:
        X.marks.append('nowrite')
    f = X.rd(dest + 0x14)
    X.fake(0x823E86, [X.tag(f)])
    f = X.rd(dest + 0x14)
    if X.rd(f + 0xC, 1) & 0x20:
        X.marks.append('ferror')
        X.errexit(ci, 0x25)


def f_coefd(X, ci, need):
    coef = X.alloc_small(ci, 1, 0x48)
    X.wr(ci + 0x188, 4, coef)
    X.wr(coef, 4, 0x5C8B40)
    X.wr(coef + 8, 4, 0x5C8B60)
    if need & 0xFF:
        X.marks.append('full_err')
        X.errexit(ci, 0x30)
        return
    X.marks.append('ok')
    mem = X.rd(ci + 4)
    b = X.callp(X.rd(mem + 4), ci, 1, 0x500)
    for off, add in ((0x24, 0x80), (0x28, 0x100), (0x2C, 0x180), (0x30, 0x200), (0x34, 0x280), (0x38, 0x300), (0x20, 0), (0x3C, 0x380), (0x40, 0x400), (0x44, 0x480)):
        X.wr(coef + off, 4, (b + add) & M32)
    X.wr(coef + 4, 4, 0x5C8E10)
    X.wr(coef + 0xC, 4, 0x5C8B70)
    X.wr(coef + 0x10, 4, 0)


def f_ycc(X, ci):
    mem = X.rd(ci + 4)
    cc = X.rd(ci + 0x1A4)
    for off in (8, 0xC, 0x10, 0x14):
        r = X.callp(X.rd(mem), ci, 1, 0x400)
        X.wr(cc + off, 4, r)
        mem = X.rd(ci + 4)
    eax = 0
    stk = 0x5B6900
    edi = 0xFF1DAF00
    edx = 0xFF4D0B80
    ecx = 0x2C8D00
    while True:
        ebp = X.rd(cc + 8)
        X.wr(eax + ebp, 4, (s32(edx) >> 16) & M32)
        ebp = X.rd(cc + 0xC)
        X.wr(eax + ebp, 4, (s32(edi) >> 16) & M32)
        ebp = X.rd(cc + 0x10)
        X.wr(eax + ebp, 4, stk & M32)
        ebp = X.rd(cc + 0x14)
        X.wr(eax + ebp, 4, ecx & M32)
        stk = (stk - 0xB6D2) & M32
        ecx = (ecx - 0x581A) & M32
        edx = (edx + 0x166E9) & M32
        edi = (edi + 0x1C5A2) & M32
        eax = (eax + 4) & M32
        if not s32(ecx) >= s32(0xFFD4CB1A):
            break


def f_null(X, ci, inbuf, inrow, outbuf, nrows):
    nc = s32(X.rd(ci + 0x24))
    ncols = X.rd(ci + 0x5C)
    n = s32((nrows - 1) & M32)
    if n < 0:
        X.marks.append('rows0')
        return
    X.marks.append('rows_many' if n > 0 else 'rows_one')
    if nc <= 0:
        X.marks.append('nc0')
    elif nc > 1:
        X.marks.append('nc_multi')
    if ncols == 0:
        X.marks.append('width0')
    if ncols > 0x1000:
        raise Bad('anchura enorme')
    cnt = n + 1
    edx = (inrow * 4) & M32
    ob = outbuf
    while True:
        if nc > 0:
            for k in range(nc):
                ecx = X.rd(inbuf + 4 * k)
                eax = X.rd(ob)
                ecx = X.rd((edx + ecx) & M32)
                eax = (eax + k) & M32
                for _ in range(ncols):
                    bl = X.rd(ecx, 1)
                    X.wr(eax, 1, bl)
                    ecx = (ecx + 1) & M32
                    eax = (eax + nc) & M32
        ob = (ob + 4) & M32
        edx = (edx + 4) & M32
        cnt -= 1
        if cnt == 0:
            break


def f_deconv(X, ci):
    mem = X.rd(ci + 4)
    cc = X.callp(X.rd(mem), ci, 1, 0x18)
    X.wr(ci + 0x1A4, 4, cc)
    X.wr(cc, 4, 0x5C9340)
    t = (X.rd(ci + 0x28) - 1) & M32
    bad = False
    if t > 4:
        bad = s32(X.rd(ci + 0x24)) < 1
    elif t == 0:
        bad = X.rd(ci + 0x24) != 1
    elif t in (1, 2):
        bad = X.rd(ci + 0x24) != 3
    else:
        bad = X.rd(ci + 0x24) != 4
    if bad:
        X.marks.append('in_bad')
        X.errexit(ci, 0x0A)
    ecx = X.rd(ci + 0x2C)

    def build():
        X.marks.append('build_tables')
        f_ycc(X, ci)

    def convbad():
        X.marks.append('conv_err')
        X.errexit(ci, 0x1B)
    if ecx == 1:
        X.marks.append('gray_out')
        eax = X.rd(ci + 0x28)
        X.wr(ci + 0x64, 4, 1)
        if eax == 1 or eax == 3:
            X.wr(cc + 4, 4, 0x5C9150)
            if s32(X.rd(ci + 0x24)) > 1:
                X.marks.append('needed_clear')
                i = 1
                ecx2 = 0x54
                while True:
                    edx = X.rd(ci + 0xC4)
                    X.wr((ecx2 + edx + 0x30) & M32, 1, 0)
                    edx = X.rd(ci + 0x24)
                    i += 1
                    ecx2 += 0x54
                    if not s32(i) < s32(edx):
                        break
        else:
            convbad()
    elif ecx == 2:
        X.marks.append('rgb_out')
        eax = X.rd(ci + 0x28)
        X.wr(ci + 0x64, 4, 4)
        if eax == 3:
            X.wr(cc + 4, 4, 0x5C8FB0)
            build()
        elif eax == 1:
            X.wr(cc + 4, 4, 0x5C9180)
        else:
            convbad()
    elif ecx == 4:
        X.marks.append('cmyk_out')
        eax = X.rd(ci + 0x28)
        X.wr(ci + 0x64, 4, 4)
        if eax == 5:
            X.wr(cc + 4, 4, 0x5C91E0)
            build()
        elif eax == 4:
            X.wr(cc + 4, 4, 0x5C90D0)
        else:
            convbad()
    else:
        X.marks.append('default_out')
        if ecx != X.rd(ci + 0x28):
            convbad()
        else:
            X.wr(ci + 0x64, 4, X.rd(ci + 0x24))
            X.wr(cc + 4, 4, 0x5C90D0)
    if X.rd(ci + 0x4A, 1):
        X.marks.append('quantize')
        X.wr(ci + 0x68, 4, 1)
    else:
        X.wr(ci + 0x68, 4, X.rd(ci + 0x64))


def f_idct(X, ci):
    mem = X.rd(ci + 4)
    idct = X.callp(X.rd(mem), ci, 1, 0x24)
    X.wr(ci + 0x19C, 4, idct)
    X.wr(idct, 4, 0x5C9510)
    nc = X.rd(ci + 0x24)
    comp = X.rd(ci + 0xC4)
    if s32(nc) <= 0:
        X.marks.append('nc0')
        return
    X.marks.append('loop')
    slot = comp + 0x50
    ebp = idct + 0x14
    ebx = 0
    while True:
        mem = X.rd(ci + 4)
        r = X.callp(X.rd(mem), ci, 1, 0x100)
        X.wr(slot, 4, r)
        for k in range(64):
            X.wr(r + 4 * k, 4, 0)
        X.wr(ebp, 4, 0xFFFFFFFF)
        eax = X.rd(ci + 0x24)
        ebx += 1
        slot += 0x54
        ebp += 4
        if not s32(ebx) < s32(eax):
            break


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    X = Ctx(c)
    W = X.W
    o = a[20]
    marks = X.marks
    marks += ['kind%d' % (c['family'] % 4), 'fill%d' % a[8], 'obj_unaligned' if o % 4 else 'obj_aligned']
    # preparacion: las mismas escrituras que Setup en LiveComparison.hpp
    p = [x & M32 for x in c['p']]
    for i in range(0, len(p), 3):
        W.put(p[i] >> 8, p[i + 1], p[i] & 0xFF, p[i + 2])
    W.snap()
    args = [W.val(x) for x in a[1:1 + a[0]]]
    ret = None
    if k == ADDQ:
        base = args[2]
        f_addq(X, args[0], args[1], lambda i: X.rd(base + 4 * i), args[3], args[4])
    elif k == SETCS:
        f_setcs(X, args[0], args[1])
    elif k == DEFCS:
        f_defcs(X, args[0])
    elif k == SETDEF:
        f_setdef(X, args[0])
    elif k == PREP:
        f_prep(X, args[0], args[1])
    elif k == EXPAND:
        f_expand(X, args[0], args[1], args[2], args[3])
    elif k == DOWN:
        f_down(X, args[0])
    elif k == READSL:
        ret = f_readsl(X, args[0], args[1], args[2])
    elif k == STARTD:
        ret = f_startd(X, args[0])
    elif k == TERM:
        f_term(X, args[0])
    elif k == COEFD:
        f_coefd(X, args[0], args[1])
    elif k == YCC:
        marks.append('run')
        f_ycc(X, W.val(a[21]))
    elif k == NULL:
        f_null(X, args[0], args[1], args[2], args[3], args[4])
    elif k == DECONV:
        f_deconv(X, args[0])
    elif k == IDCT:
        f_idct(X, args[0])
    gch = []
    for addr in sorted(W.glob):
        if W.glob[addr] != W.gref.get(addr):
            gch.append([addr, W.glob[addr]])
    if fn in RET_EAX:
        r = (ret or 0) & M32
    elif fn in RET_AL:
        r = (ret or 0) & 0xFF
    else:
        r = None
    return {'events': X.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'gchanges': gch, 'ret': r, 'threw': False, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:8], e['events'][:8])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX/AL (valor devuelto, ya comprobado en las funciones que lo devuelven)"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
        if why:
            return side, why
    return None
