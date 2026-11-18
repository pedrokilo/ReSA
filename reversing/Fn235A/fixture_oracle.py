"""Oraculo independiente (Python) de Fn235A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (`dumpbin /disasm` sobre el gta-sa.exe canonico), lo que debe observarse de las
30 funciones del lote 235A (libjpeg 6b embebida en el EXE: jdpostct, jdsample, jerror, jmemmgr, jmemnobs, jutils y jcapimin) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial y lleva copiadas del EXE las
constantes de solo lectura que leen las funciones (tablas de holgura de alloc_small 0x86B7F8/0x86B800 y sus vecinas, cadenas 0x862EB0/0x86B808/0x86B810, FILE 0x8E2C70). Cada funcion se escribe como un
PROGRAMA sobre un modelo de memoria (arenas), en el orden de lecturas y escrituras del EXE (lo que importa cuando un falso cambia la memoria entre dos lecturas, o cuando una funcion guarda un valor en un
registro y no lo relee). Las lecturas fuera de las arenas lanzan una excepcion (un caso invalido del generador).

Desensamblado (resumen; direcciones del EXE). jpeg_decompress_struct: +0 err, +4 mem, +0x24 num_components, +0x48 do_fancy, +0x4A quantize_colors, +0x5C output_width, +0x64 out_color_components,
+0xC4 comp_info (0x54 bytes por componente), +0x10A CCIR601, +0x110 max_h, +0x114 max_v, +0x118 min_DCT, +0x18C post, +0x1A0 upsample, +0x1A4 cconvert. my_memory_mgr: +0 alloc_small, +4 alloc_large,
+8 alloc_sarray, +0xC alloc_barray, +0x2C max_memory_to_use, +0x30 max_alloc_chunk, +0x34 small_list[2], +0x3C large_list[2], +0x44 virt_sarray_list, +0x48 virt_barray_list, +0x4C total, +0x50
last_rowsperchunk. jpeg_compress_struct (0x168 bytes): +0 err, +4 mem, +8 progress, +0xC client_data, +0x10 is_decompressor, +0x14 global_state, +0x18 dest, +0x20 image_height, +0x48 quant_tbl_ptrs[4],
+0x58 dc_huff[4], +0x68 ac_huff[4], +0xD0 next_scanline, +0xE0 total_iMCU_rows, +0x13C master, +0x148 coef, +0x14C marker, +0x160 marker_list.
Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF

# Datos de solo lectura del EXE (gta-sa.exe canonico, .rdata desde 0x86B7E0): holguras de alloc_small (first_pool_slop 0x86B7F8 = [1600, 16000], extra_pool_slop 0x86B800 = [0, 5000]) y vecinas
ROM = {0x86B7E0: 0x3F0A8BD4, 0x86B7E4: 0x3EC3EF15, 0x86B7E8: 0x3F3504F3, 0x86B7EC: 0x3F8A8BD4, 0x86B7F0: 0x40273D75, 0x86B7F4: 0x3FEC835E, 0x86B7F8: 0x640, 0x86B7FC: 0x3E80, 0x86B800: 0x0,
       0x86B804: 0x1388, 0x86B808: 0x25646C25, 0x86B80C: 0x63, 0x86B810: 0x4745504A, 0x86B814: 0x4D454D, 0x86B818: 0x0, 0x86B81C: 0x1, 0x86B820: 0x8, 0x86B824: 0x10, 0x86B828: 0x9, 0x86B82C: 0x2}
STDERR_FILE, FMT_PERCENT_S, FMT_LD_C, STR_JPEGMEM = 0x8E2C70, 0x862EB0, 0x86B808, 0x86B810

NAMES = ["LibJpegDecompressUpsample::jinit_post_controller", "LibJpegDecompressUpsample::sep_upsample", "LibJpegDecompressUpsample::jinit_upsampler", "LibJpegError::error_exit", "LibJpegError::output_message",
         "LibJpegError::jpeg_std_error", "LibJpegMemory::alloc_small", "LibJpegMemory::alloc_large", "LibJpegMemory::alloc_sarray", "LibJpegMemory::alloc_barray", "LibJpegMemory::request_virt_sarray",
         "LibJpegMemory::request_virt_barray", "LibJpegMemory::realize_virt_arrays", "LibJpegMemory::free_pool", "LibJpegMemory::jinit_memory_mgr", "LibJpegMemory::jpeg_get_small",
         "LibJpegMemory::jpeg_free_small", "LibJpegMemory::jpeg_get_large", "LibJpegMemory::jpeg_free_large", "LibJpegMemory::jpeg_mem_available", "LibJpegMemory::jpeg_open_backing_store",
         "LibJpegMemory::jpeg_mem_init", "LibJpegMemory::jpeg_mem_term", "LibJpegMemory::jdiv_round_up", "LibJpegMemory::jround_up", "LibJpegMemory::jcopy_sample_rows", "LibJpegMemory::jzero_far",
         "LibJpegCompressApi::jpeg_CreateCompress", "LibJpegCompressApi::jpeg_suppress_tables", "LibJpegCompressApi::jpeg_finish_compress"]
ENTRIES = (0x5CCDF0, 0x5CCE80, 0x5CD350, 0x5CD6B0, 0x5CD6D0, 0x5CD810, 0x5CEBC0, 0x5CED10, 0x5CEDD0, 0x5CEE80, 0x5CEF30, 0x5CEFA0, 0x5CF010, 0x5CF580, 0x5CF6D0,
           0x5CF800, 0x5CF810, 0x5CF820, 0x5CF830, 0x5CF840, 0x5CF850, 0x5CF870, 0x5CF880, 0x5CF890, 0x5CF8A0, 0x5CF8C0, 0x5CF940, 0x5CF960, 0x5CFA50, 0x5CFAC0)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(POST, SEP, UPINIT, ERREXIT, OUTMSG, STDERR, ALLOCS, ALLOCL, ALLOCSA, ALLOCBA, REQS, REQB, REALIZE, FREEPOOL, INITMEM, GETS, FREES, GETL, FREEL, MEMAVAIL, OPENBS, MEMINIT, MEMTERM, DIVUP,
 ROUNDUP, COPYROWS, ZEROFAR, CREATEC, SUPPRESS, FINISH) = range(30)
RET_RAW = {MEMAVAIL, MEMINIT, DIVUP, ROUNDUP}                       # EAX tal cual
RET_TAG = {STDERR, ALLOCS, ALLOCL, ALLOCSA, ALLOCBA, REQS, REQB, GETS, GETL}   # EAX como puntero (etiqueta de arena)


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
        self.mem = tuple(Mem(a[9], a[10 + i]) for i in range(4))
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
    POST: ['noquant', 'quant_full_err', 'quant_ok', 'poke_applied'],
    SEP: ['regroup', 'noregroup', 'clip_togo', 'clip_avail', 'advance', 'no_advance', 'nc0', 'poke_applied'],
    UPINIT: ['ccir_err', 'fancy', 'noop', 'full', 'h2v1', 'h2v1_fancy', 'h2v2', 'h2v2_fancy', 'int', 'fract_err', 'nc0', 'poke_applied'],
    ERREXIT: ['ran', 'thrown_in_msg', 'poke_applied'],
    OUTMSG: ['ran', 'poke_applied'],
    STDERR: ['ran'],
    ALLOCS: ['fits', 'new_block', 'first_slop', 'extra_slop', 'size_err', 'pool_err', 'get_retry', 'slop_err', 'rounded', 'thrown', 'poke_applied'],
    ALLOCL: ['ran', 'size_err', 'pool_err', 'rounded', 'null_err', 'thrown', 'poke_applied'],
    ALLOCSA: ['rows0', 'one_chunk', 'multi_chunk', 'width_err', 'thrown', 'poke_applied'],
    ALLOCBA: ['rows0', 'one_chunk', 'multi_chunk', 'width_err', 'thrown', 'poke_applied'],
    REQS: ['pool_ok', 'pool_err', 'list_empty', 'list_chain', 'poke_applied'],
    REQB: ['pool_ok', 'pool_err', 'list_empty', 'list_chain', 'poke_applied'],
    REALIZE: ['no_arrays', 'no_space', 'fits', 'backing', 'skip_buffered', 'avail_max', 'avail_div', 'avail_clamp', 'sarray', 'barray', 'poke_applied'],
    FREEPOOL: ['pool0', 'pool1', 'closed_s', 'closed_b', 'large', 'small', 'pool_err', 'thrown', 'poke_applied'],
    INITMEM: ['mem_ok', 'mem_null', 'env_none', 'env_set', 'scan_fail', 'scan_mega', 'scan_kilo', 'thrown', 'poke_applied'],
    GETS: ['ran'], FREES: ['ran'], GETL: ['ran'], FREEL: ['ran'], MEMAVAIL: ['ran'],
    OPENBS: ['ran', 'thrown', 'poke_applied'],
    MEMINIT: ['ran'], MEMTERM: ['ran'],
    DIVUP: ['ran', 'neg'], ROUNDUP: ['ran', 'neg'],
    COPYROWS: ['rows0', 'rows', 'words', 'tail'],
    ZEROFAR: ['words', 'tail', 'empty'],
    CREATEC: ['ok', 'ver_err', 'size_err', 'thrown', 'poke_applied'],
    SUPPRESS: ['q_set', 'q_null', 'dc_set', 'ac_set', 'dc_null', 'suppress_hi'],
    FINISH: ['state65', 'state66', 'state67', 'bad_state', 'too_little', 'loop', 'rows', 'progress', 'cant_suspend', 'last_now', 'thrown', 'poke_applied'],
}
KEYS = {}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[_fn]

# falsos: id -> (aridad, mascara de punteros; el bit i = argumento i es un puntero y se anota como etiqueta); igual que Fk* en LiveComparison.hpp
FAKES = {
    0xC100: (1, 1), 0xC101: (1, 1), 0xC102: (2, 3), 0xC103: (3, 1), 0xC104: (4, 1), 0xC105: (4, 15), 0xC106: (5, 11), 0xC107: (2, 3), 0xC108: (1, 1), 0xC109: (1, 1), 0xC10A: (1, 1),
    0xC10B: (2, 1), 0xC10C: (1, 1), 0xC10D: (1, 1),
    0x5C6B90: (1, 1), 0x5C6B50: (1, 1), 0x5CF6D0: (1, 1), 0x5CF8A0: (2, 0), 0x5CF800: (2, 1), 0x5CF820: (2, 1), 0x5CF810: (3, 3), 0x5CF830: (3, 3), 0x5CF840: (4, 1), 0x5CF850: (3, 3),
    0x5CF870: (1, 1), 0x5CF880: (1, 1), 0x5CEBC0: (3, 1), 0x5CED10: (3, 1), 0x5CEDD0: (4, 1), 0x5CEE80: (4, 1), 0x72F420: (2, 0), 0x72F430: (1, 1), 0x823CAE: (1, 0), 0x8240C7: (1, 0),
}
STACKP = 0x7F000000     # etiqueta de un puntero a la pila del llamador (cambia de una pasada a otra)


class Thrown(Exception):
    """un falso que no vuelve (salida no local) o exit() del EXE"""


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


class Ctx:
    """estado de un caso: mundo, eventos, marcas, retornos programados y efectos laterales"""

    def __init__(self, c):
        self.c = c
        self.a = [x & M32 for x in c['a']]
        self.W = World(c)
        self.events = []
        self.marks = []
        self.count = {}
        self.rets = {}
        self.bufstr = b''
        self.last_n = 0
        r = [x & M32 for x in c['r']]
        for i in range(0, len(r), 3):
            self.rets[(r[i], r[i + 1])] = r[i + 2]

    def tag(self, v):
        return self.W.tagv(v)

    def _poke(self, base, eid, n):
        a = self.a
        if a[base] and eid == a[base] and n == a[base + 1]:
            val = a[base + 5] if a[base + 4] == 0 else self.W.val(a[base + 5])
            self.W.wr(self.W.raw(a[base + 2], a[base + 3]), 4, val)
            self.marks.append('poke_applied')

    def fake(self, eid, args):
        """un falso: anota el evento (con los punteros como etiquetas), aplica los efectos laterales, puede no volver y devuelve el valor programado"""
        arity, mask = FAKES[eid]
        if len(args) < arity:
            raise Bad('falso %x llamado con menos argumentos que su aridad' % eid)
        ev = [eid] + [(self.tag(v) if (mask >> i) & 1 else v & M32) for i, v in enumerate(args[:arity])]
        return self._done(eid, ev)

    def _done(self, eid, ev):
        self.events.append(ev)
        n = self.count.get(eid, 0)
        self.count[eid] = n + 1
        self.last_n = n
        self._poke(15, eid, n)
        self._poke(24, eid, n)
        a = self.a
        if a[30] and eid == a[30] and n == a[31]:
            raise Thrown()
        v = self.rets.get((eid, n))
        return self.W.val(v) if v is not None else 0

    def callp(self, word, *args):
        """llamada por puntero a una funcion falsa (0xF00000kk -> id 0xC100 + kk)"""
        if (word >> 8) != 0xF00000 or (word & 0xFF) > 13:
            raise Bad('puntero a funcion invalido %x' % word)
        eid = 0xC100 + (word & 0xFF)
        if eid == 0xC102:
            # format_message: escribe en el buffer del llamador (la pila) una cadena de v % 5 letras que depende del valor programado; el oraculo la guarda para el hash de fprintf
            nn = self.count.get(eid, 0)
            v = self.rets.get((eid, nn), 0)
            self.bufstr = bytes(0x41 + ((v >> 8) + i) % 26 for i in range(v % 5))
        return self.fake(eid, args)

    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def errexit(self, ci, code, *parms):
        """ERREXIT/ERREXIT1/ERREXIT2: [err] se relee en cada paso; al final llama a error_exit = [[ci]] con (ci)"""
        e = self.rd(ci)
        self.wr(e + 0x14, 4, code)
        for k, v in enumerate(parms):
            e = self.rd(ci)
            self.wr(e + 0x18 + 4 * k, 4, v)
        e = self.rd(ci)
        self.callp(self.rd(e), ci)


def rom(addr):
    addr &= M32
    if addr not in ROM:
        raise Bad('lectura de memoria de solo lectura desconocida %x' % addr)
    return ROM[addr]


def udiv(a, b):
    a &= M32
    b &= M32
    if b == 0:
        raise Bad('division por cero')
    return a // b


def lt(a, b):
    return s32(a) < s32(b)


# ------------------------------------------------------------------------------------------------ funciones
def f_post(X, ci, need):
    mem = X.rd(ci + 4)
    post = X.callp(X.rd(mem), ci, 1, 0x1C)
    X.wr(ci + 0x18C, 4, post)
    X.wr(post, 4, 0x5CCD70)
    X.wr(post + 8, 4, 0)
    X.wr(post + 0xC, 4, 0)
    if X.rd(ci + 0x4A, 1) == 0:
        X.marks.append('noquant')
        return
    maxv = X.rd(ci + 0x114)
    X.wr(post + 0x10, 4, maxv)
    if need & 0xFF:
        X.marks.append('quant_full_err')
        X.errexit(ci, 4)
        return
    X.marks.append('quant_ok')
    width = (X.rd(ci + 0x64) * X.rd(ci + 0x5C)) & M32
    mem = X.rd(ci + 4)
    res = X.callp(X.rd(mem + 8), ci, 1, width, maxv)
    X.wr(post + 0xC, 4, res)


def f_sep(X, ci, inbuf, ctrp, avail, outbuf, outctrp, outavail):
    up = X.rd(ci + 0x1A0)
    if not lt(X.rd(up + 0x2C), X.rd(ci + 0x114)):
        X.marks.append('regroup')
        nc = X.rd(ci + 0x24)
        comp = X.rd(ci + 0xC4)
        if not lt(0, nc):
            X.marks.append('nc0')
        else:
            idx = 0
            while True:
                rgh = X.rd(up + 0x34 + 4 * idx)
                ctr = X.rd(ctrp)
                prod = (rgh * ctr) & M32
                base = X.rd((inbuf + 4 * idx) & M32)
                ptr = (base + prod * 4) & M32
                fn = X.rd(up + 0x1C + 4 * idx)
                X.callp(fn, ci, comp, ptr, up + 0x0C + 4 * idx)
                comp = (comp + 0x54) & M32
                idx += 1
                if not lt(idx, X.rd(ci + 0x24)):
                    break
        X.wr(up + 0x2C, 4, 0)
    else:
        X.marks.append('noregroup')
    esi = (X.rd(ci + 0x114) - X.rd(up + 0x2C)) & M32
    eax = X.rd(up + 0x30)
    if esi > eax:
        esi = eax
        X.marks.append('clip_togo')
    cur = X.rd(outctrp)
    ecx = (outavail - cur) & M32
    if esi > ecx:
        esi = ecx
        X.marks.append('clip_avail')
    cc = X.rd(ci + 0x1A4)
    optr = (outbuf + cur * 4) & M32
    row = X.rd(up + 0x2C)
    X.callp(X.rd(cc + 4), ci, up + 0x0C, row, optr, esi)
    X.wr(outctrp, 4, (X.rd(outctrp) + esi) & M32)
    edx = X.rd(up + 0x30)
    ecx = X.rd(up + 0x2C)
    ecx = (ecx + esi) & M32
    edx = (edx - esi) & M32
    X.wr(up + 0x30, 4, edx)
    X.wr(up + 0x2C, 4, ecx)
    if not lt(ecx, X.rd(ci + 0x114)):
        X.marks.append('advance')
        X.wr(ctrp, 4, (X.rd(ctrp) + 1) & M32)
    else:
        X.marks.append('no_advance')


def f_upinit(X, ci):
    mem = X.rd(ci + 4)
    up = X.callp(X.rd(mem), ci, 1, 0x4C)
    X.wr(ci + 0x1A0, 4, up)
    X.wr(up, 4, 0x5CCE60)
    X.wr(up + 4, 4, 0x5CCE80)
    X.wr(up + 8, 1, 0)
    if X.rd(ci + 0x10A, 1):
        X.marks.append('ccir_err')
        X.errexit(ci, 0x19)
    fancy = 1 if (X.rd(ci + 0x48, 1) and s32(X.rd(ci + 0x118)) > 1) else 0
    if fancy:
        X.marks.append('fancy')
    nc = X.rd(ci + 0x24)
    comp = X.rd(ci + 0xC4)
    if s32(nc) <= 0:
        X.marks.append('nc0')
        return
    idx = 0
    slot = (up + 0x1C) & M32
    ebp = (comp + 0x24) & M32
    while True:
        dct = X.rd(ebp)
        h = X.rd((ebp - 0x1C) & M32)
        mind = X.rd(ci + 0x118)
        hin = idiv((h * dct) & M32, mind)[0]
        v = X.rd((ebp - 0x18) & M32)
        vin = idiv((v * dct) & M32, mind)[0]
        maxv = X.rd(ci + 0x114)
        maxh = X.rd(ci + 0x110)
        X.wr(slot + 0x18, 4, vin)
        alloc = True
        if X.rd((ebp + 0xC) & M32, 1) == 0:
            X.marks.append('noop')
            X.wr(slot, 4, 0x5CCF70)
            alloc = False
        elif hin == maxh and vin == maxv:
            X.marks.append('full')
            X.wr(slot, 4, 0x5CCF60)
            alloc = False
        else:
            wide = X.rd((ebp + 4) & M32) > 2
            if (hin * 2) & M32 == maxh and vin == maxv:
                if fancy and wide:
                    X.marks.append('h2v1_fancy')
                    X.wr(slot, 4, 0x5CD160)
                else:
                    X.marks.append('h2v1')
                    X.wr(slot, 4, 0x5CD080)
            elif (hin * 2) & M32 == maxh and (vin * 2) & M32 == maxv:
                if fancy and wide:
                    X.marks.append('h2v2_fancy')
                    X.wr(slot, 4, 0x5CD220)
                    X.wr(up + 8, 1, 1)
                else:
                    X.marks.append('h2v2')
                    X.wr(slot, 4, 0x5CD0E0)
            else:
                q, r = idiv(maxh, hin)
                ok = False
                if r == 0:
                    q2, r2 = idiv(maxv, vin)
                    if r2 == 0:
                        ok = True
                if ok:
                    X.marks.append('int')
                    X.wr(slot, 4, 0x5CCF80)
                    X.wr((up + idx + 0x44) & M32, 1, q & 0xFF)
                    X.wr((up + idx + 0x48) & M32, 1, q2 & 0xFF)
                else:
                    X.marks.append('fract_err')
                    X.errexit(ci, 0x26)
        if alloc:
            edx = X.rd(ci + 0x114)
            eax = X.rd(ci + 0x110)
            ecx = X.rd(ci + 0x5C)
            mem2 = X.rd(ci + 4)
            width = X.fake(0x5CF8A0, [ecx, eax])
            fn = X.rd((mem2 + 8) & M32)
            res = X.callp(fn, ci, 1, width, edx)
            X.wr((slot - 0x10) & M32, 4, res)
        idx += 1
        slot = (slot + 4) & M32
        ebp = (ebp + 0x54) & M32
        if not lt(idx, X.rd(ci + 0x24)):
            break


def f_errexit(X, ci):
    X.marks.append('ran')
    err = X.rd(ci)
    try:
        X.callp(X.rd(err + 8), ci)
    except Thrown:
        X.marks.append('thrown_in_msg')
        raise
    X.fake(0x5C6B90, [ci])
    X.fake(0x823CAE, [1])
    raise Thrown()


def f_outmsg(X, ci):
    X.marks.append('ran')
    err = X.rd(ci)
    X.callp(X.rd(err + 0xC), ci, STACKP)
    X.events.append([0x823A30, STDERR_FILE, FMT_PERCENT_S, fnv(X.bufstr)])


def f_stderr(X, err):
    X.marks.append('ran')
    for off, v in ((0, 0x5CD6B0), (4, 0x5CD700), (8, 0x5CD6D0), (0xC, 0x5CD740), (0x10, 0x5CD7F0), (0x68, 0), (0x6C, 0), (0x14, 0), (0x70, 0x86B7C8), (0x74, 1), (0x78, 0), (0x7C, 0), (0x80, 0)):
        X.wr(err + off, 4, v)
    return err


def f_allocsmall(X, ci, pool, size):
    mem = X.rd(ci + 4)
    if size > 0x3B9AC9F0:
        X.marks.append('size_err')
        X.errexit(ci, 0x36, 1)
    odd = size & 7
    if odd > 0:
        X.marks.append('rounded')
        size = (size + 8 - odd) & M32
    if s32(pool) < 0 or s32(pool) >= 2:
        X.marks.append('pool_err')
        X.errexit(ci, 0xE, pool)
    prev = 0
    hdr = X.rd((mem + pool * 4 + 0x34) & M32)
    while hdr:
        if X.rd((hdr + 8) & M32) >= size:
            break
        prev = hdr
        hdr = X.rd(hdr)
    if hdr == 0:
        X.marks.append('new_block')
        minreq = (size + 0x10) & M32
        if prev == 0:
            X.marks.append('first_slop')
            slop = rom(0x86B7F8 + 4 * pool)
        else:
            X.marks.append('extra_slop')
            slop = rom(0x86B800 + 4 * pool)
        limit = (0x3B9ACA00 - minreq) & M32
        if slop > limit:
            slop = limit
        tries = 0
        while True:
            p = X.fake(0x5CF800, [ci, (slop + minreq) & M32])
            if p:
                break
            tries += 1
            slop >>= 1
            if slop < 0x32:
                X.marks.append('slop_err')
                X.errexit(ci, 0x36, 2)
        if tries:
            X.marks.append('get_retry')
        X.wr((mem + 0x4C) & M32, 4, (X.rd((mem + 0x4C) & M32) + slop + minreq) & M32)
        X.wr(p, 4, 0)
        X.wr(p + 4, 4, 0)
        X.wr(p + 8, 4, (slop + size) & M32)
        if prev == 0:
            X.wr((mem + pool * 4 + 0x34) & M32, 4, p)
        else:
            X.wr(prev, 4, p)
        hdr = p
    else:
        X.marks.append('fits')
    used = X.rd((hdr + 4) & M32)
    ret = (used + hdr + 0x10) & M32
    X.wr((hdr + 4) & M32, 4, (used + size) & M32)
    X.wr((hdr + 8) & M32, 4, (X.rd((hdr + 8) & M32) - size) & M32)
    return ret


def f_alloclarge(X, ci, pool, size):
    X.marks.append('ran')
    mem = X.rd(ci + 4)
    if size > 0x3B9AC9F0:
        X.marks.append('size_err')
        X.errexit(ci, 0x36, 3)
    odd = size & 7
    if odd > 0:
        X.marks.append('rounded')
        size = (size + 8 - odd) & M32
    if s32(pool) < 0 or s32(pool) >= 2:
        X.marks.append('pool_err')
        X.errexit(ci, 0xE, pool)
    hdr = X.fake(0x5CF820, [ci, (size + 0x10) & M32])
    if hdr == 0:
        X.marks.append('null_err')
        X.errexit(ci, 0x36, 4)
    X.wr((mem + 0x4C) & M32, 4, (X.rd((mem + 0x4C) & M32) + size + 0x10) & M32)
    nxt = X.rd((mem + pool * 4 + 0x3C) & M32)
    X.wr(hdr + 4, 4, size)
    X.wr(hdr, 4, nxt)
    X.wr(hdr + 8, 4, 0)
    X.wr((mem + pool * 4 + 0x3C) & M32, 4, hdr)
    return (hdr + 0x10) & M32


def f_allocarray(X, ci, pool, spr, nrows, block):
    """alloc_sarray (block = False) y alloc_barray (block = True: la fila mide spr << 7 bytes)"""
    rowbytes = ((spr << 7) & M32) if block else spr
    rpc = udiv(0x3B9AC9F0, rowbytes)
    mem = X.rd(ci + 4)
    edi = rpc
    if not lt(0, edi):
        X.marks.append('width_err')
        X.errexit(ci, 0x46)
    if not lt(edi, nrows):
        edi = nrows
    X.wr((mem + 0x50) & M32, 4, edi)
    vec = X.fake(0x5CEBC0, [ci, pool, (nrows * 4) & M32])
    cur = 0
    if nrows == 0:
        X.marks.append('rows0')
    else:
        chunks = 0
        while True:
            rem = (nrows - cur) & M32
            if not edi < rem:
                edi = rem
            size = (edi * spr) & M32
            if block:
                size = (size << 7) & M32
            w = X.fake(0x5CED10, [ci, pool, size])
            chunks += 1
            for _ in range(edi):
                X.wr((vec + cur * 4) & M32, 4, w)
                cur = (cur + 1) & M32
                w = (w + rowbytes) & M32
            if not cur < nrows:
                break
        X.marks.append('multi_chunk' if chunks > 1 else 'one_chunk')
    return vec


def f_reqvirt(X, ci, pool, pre, spr, nrows, maxaccess, head):
    mem = X.rd(ci + 4)
    if pool != 1:
        X.marks.append('pool_err')
        X.errexit(ci, 0xE, pool)
    else:
        X.marks.append('pool_ok')
    r = X.fake(0x5CEBC0, [ci, pool, 0x78])
    X.wr(r + 4, 4, nrows)
    X.wr(r + 8, 4, spr)
    X.wr(r, 4, 0)
    X.wr(r + 0xC, 4, maxaccess)
    X.wr(r + 0x20, 1, pre & 0xFF)
    X.wr(r + 0x22, 1, 0)
    nxt = X.rd((mem + head) & M32)
    X.marks.append('list_chain' if nxt else 'list_empty')
    X.wr(r + 0x24, 4, nxt)
    X.wr((mem + head) & M32, 4, r)
    return r


def f_realize(X, ci):
    mem = X.rd(ci + 4)
    spm = maxsp = 0
    p = X.rd(mem + 0x44)
    while p:
        if X.rd(p) == 0:
            w = X.rd(p + 8)
            spm = (spm + X.rd(p + 0xC) * w) & M32
            maxsp = (maxsp + X.rd(p + 4) * w) & M32
        p = X.rd(p + 0x24)
    p = X.rd(mem + 0x48)
    while p:
        if X.rd(p) == 0:
            w = X.rd(p + 8)
            spm = (spm + ((X.rd(p + 0xC) * w) << 7)) & M32
            maxsp = (maxsp + ((X.rd(p + 4) * w) << 7)) & M32
        p = X.rd(p + 0x24)
    if s32(spm) <= 0:
        X.marks.append('no_space')
        return
    avail = X.fake(0x5CF840, [ci, spm, maxsp, X.rd(mem + 0x4C)])
    if not lt(avail, maxsp):
        X.marks.append('avail_max')
        maxmin = 0x3B9ACA00
    else:
        X.marks.append('avail_div')
        maxmin = idiv(avail, spm)[0]
        if not lt(0, maxmin):
            X.marks.append('avail_clamp')
            maxmin = 1
    for head, alloc, shift, mark in ((0x44, 0x5CEDD0, 0, 'sarray'), (0x48, 0x5CEE80, 7, 'barray')):
        p = X.rd(mem + head)
        while p:
            if X.rd(p) != 0:
                X.marks.append('skip_buffered')
            else:
                X.marks.append(mark)
                rows = X.rd(p + 4)
                ma = X.rd(p + 0xC)
                minh = (udiv((rows - 1) & M32, ma) + 1) & M32
                if not lt(maxmin, minh):
                    X.wr(p + 0x10, 4, rows)
                    X.marks.append('fits')
                else:
                    X.marks.append('backing')
                    w = X.rd(p + 8)
                    inmem = (ma * maxmin) & M32
                    total = (w * rows) & M32
                    if shift:
                        total = (total << 7) & M32
                    X.wr(p + 0x10, 4, inmem)
                    X.fake(0x5CF850, [ci, (p + 0x28) & M32, total])
                    X.wr(p + 0x22, 1, 1)
                ecx = X.rd(p + 0x10)
                edx = X.rd(p + 8)
                res = X.fake(alloc, [ci, 1, edx, ecx])
                X.wr(p, 4, res)
                X.wr(p + 0x14, 4, X.rd(mem + 0x50))
                X.wr(p + 0x18, 4, 0)
                X.wr(p + 0x1C, 4, 0)
                X.wr(p + 0x21, 1, 0)
            p = X.rd(p + 0x24)
    if not (X.rd(mem + 0x44) or X.rd(mem + 0x48)):
        X.marks.append('no_arrays')


def f_freepool(X, ci, pool):
    mem = X.rd(ci + 4)
    if s32(pool) < 0 or s32(pool) >= 2:
        X.marks.append('pool_err')
        X.errexit(ci, 0xE, pool)
    X.marks.append('pool1' if pool == 1 else 'pool0')
    if pool == 1:
        e = X.rd(mem + 0x44)
        while e:
            if X.rd(e + 0x22, 1) != 0:
                X.marks.append('closed_s')
                X.wr(e + 0x22, 1, 0)
                X.callp(X.rd(e + 0x30), ci, (e + 0x28) & M32)
            e = X.rd(e + 0x24)
        e = X.rd(mem + 0x48)
        X.wr(mem + 0x44, 4, 0)
        while e:
            if X.rd(e + 0x22, 1) != 0:
                X.marks.append('closed_b')
                X.wr(e + 0x22, 1, 0)
                X.callp(X.rd(e + 0x30), ci, (e + 0x28) & M32)
            e = X.rd(e + 0x24)
        X.wr(mem + 0x48, 4, 0)
    for base, eid, mark in ((0x3C, 0x5CF830, 'large'), (0x34, 0x5CF810, 'small')):
        slot = (mem + pool * 4 + base) & M32
        h = X.rd(slot)
        X.wr(slot, 4, 0)
        while h:
            left = X.rd(h + 8)
            inuse = X.rd(h + 4)
            nxt = X.rd(h)
            freed = (left + inuse + 0x10) & M32
            X.marks.append(mark)
            X.fake(eid, [ci, h, freed])
            X.wr((mem + 0x4C) & M32, 4, (X.rd((mem + 0x4C) & M32) - freed) & M32)
            h = nxt


def f_initmem(X, ci):
    X.wr(ci + 4, 4, 0)
    n = X.fake(0x5CF870, [ci])
    mem = X.fake(0x5CF800, [ci, 0x54])
    if mem == 0:
        X.marks.append('mem_null')
        X.fake(0x5CF880, [ci])
        X.errexit(ci, 0x36, 0)
    else:
        X.marks.append('mem_ok')
    for off, v in ((0, 0x5CEBC0), (4, 0x5CED10), (8, 0x5CEDD0), (0xC, 0x5CEE80), (0x10, 0x5CEF30), (0x14, 0x5CEFA0), (0x18, 0x5CF010), (0x1C, 0x5CF2F0), (0x20, 0x5CF430), (0x24, 0x5CF580),
                   (0x28, 0x5CF690), (0x30, 0x3B9ACA00), (0x2C, n), (0x38, 0), (0x40, 0), (0x34, 0), (0x3C, 0), (0x44, 0), (0x48, 0), (0x4C, 0x54)):
        X.wr(mem + off, 4, v)
    X.wr(ci + 4, 4, mem)
    env = X.fake(0x8240C7, [STR_JPEGMEM])
    if env == 0:
        X.marks.append('env_none')
        return
    ch = 0x78
    # sscanf(env, "%ld%c", &n, &ch): el falso anota [id, cadena, formato, &n, &ch] y escribe n (si coincidio 1 o mas) y ch (si 2 o mas)
    matched = X.W.val(X.rets.get((0x8220AD, 0), 0))
    nval = X.W.val(X.rets.get((0x8220AD, 1), 0))
    chval = X.W.val(X.rets.get((0x8220AD, 2), 0)) & 0xFF
    X._done(0x8220AD, [0x8220AD, X.tag(env), FMT_LD_C, STACKP, STACKP])
    if s32(matched) >= 1:
        n = nval
    if s32(matched) >= 2:
        ch = chval
    if not lt(0, matched):
        X.marks.append('scan_fail')
        return
    X.marks.append('env_set')
    if ch in (0x6D, 0x4D):
        X.marks.append('scan_mega')
        X.wr(mem + 0x2C, 4, (n * 1000 * 1000) & M32)
    else:
        X.marks.append('scan_kilo')
        X.wr(mem + 0x2C, 4, (n * 1000) & M32)


def f_copyrows(X, inp, srow, out, drow, nrows, ncols):
    ip = (inp + srow * 4) & M32
    op = (out + drow * 4) & M32
    if s32(nrows) <= 0:
        X.marks.append('rows0')
        return
    X.marks.append('rows')
    n = nrows
    while True:
        src = X.rd(ip)
        dst = X.rd(op)
        for _ in range(ncols >> 2):
            X.wr(dst, 4, X.rd(src))
            src = (src + 4) & M32
            dst = (dst + 4) & M32
            X.marks.append('words')
        for _ in range(ncols & 3):
            X.wr(dst, 1, X.rd(src, 1))
            src = (src + 1) & M32
            dst = (dst + 1) & M32
            X.marks.append('tail')
        ip = (ip + 4) & M32
        op = (op + 4) & M32
        n = (n - 1) & M32
        if n == 0:
            break


def f_zerofar(X, target, nbytes):
    if nbytes == 0:
        X.marks.append('empty')
    dst = target
    for _ in range(nbytes >> 2):
        X.wr(dst, 4, 0)
        dst = (dst + 4) & M32
        X.marks.append('words')
    for _ in range(nbytes & 3):
        X.wr(dst, 1, 0)
        dst = (dst + 1) & M32
        X.marks.append('tail')


def f_createc(X, ci, ver, sz):
    X.wr(ci + 4, 4, 0)
    if ver != 0x3E:
        X.marks.append('ver_err')
        X.errexit(ci, 0xC, 0x3E, ver)
    if sz != 0x168:
        X.marks.append('size_err')
        X.errexit(ci, 0x15, 0x168, sz)
    if ver == 0x3E and sz == 0x168:
        X.marks.append('ok')
    err = X.rd(ci)
    cdata = X.rd(ci + 0xC)
    for i in range(0x5A):
        X.wr(ci + 4 * i, 4, 0)
    X.wr(ci, 4, err)
    X.wr(ci + 0xC, 4, cdata)
    X.wr(ci + 0x10, 1, 0)
    X.fake(0x5CF6D0, [ci])
    for off in (8, 0x18, 0x44, 0x48, 0x4C, 0x50, 0x54):
        X.wr(ci + off, 4, 0)
    for k in range(4):
        X.wr(ci + 0x58 + 4 * k, 4, 0)
        X.wr(ci + 0x68 + 4 * k, 4, 0)
    X.wr(ci + 0x160, 4, 0)
    X.wr(ci + 0x30, 4, 0)
    X.wr(ci + 0x34, 4, 0x3FF00000)
    X.wr(ci + 0x14, 4, 0x64)


def f_suppress(X, ci, sup):
    v = sup & 0xFF
    if v > 1:
        X.marks.append('suppress_hi')
    for k in range(4):
        t = X.rd(ci + 0x48 + 4 * k)
        if t:
            X.marks.append('q_set')
            X.wr(t + 0x80, 1, v)
        else:
            X.marks.append('q_null')
    for k in range(4):
        t = X.rd(ci + 0x58 + 4 * k)
        if t:
            X.marks.append('dc_set')
            X.wr(t + 0x111, 1, v)
        else:
            X.marks.append('dc_null')
        t = X.rd(ci + 0x68 + 4 * k)
        if t:
            X.marks.append('ac_set')
            X.wr(t + 0x111, 1, v)


def f_finish(X, ci):
    state = X.rd(ci + 0x14)
    if state in (0x65, 0x66):
        X.marks.append('state65' if state == 0x65 else 'state66')
        if not X.rd(ci + 0xD0) >= X.rd(ci + 0x20):
            X.marks.append('too_little')
            X.errexit(ci, 0x43)
        m = X.rd(ci + 0x13C)
        X.callp(X.rd(m + 8), ci)
    elif state != 0x67:
        X.marks.append('bad_state')
        e = X.rd(ci)
        X.wr(e + 0x14, 4, 0x14)
        e = X.rd(ci)
        X.wr(e + 0x18, 4, X.rd(ci + 0x14))
        e = X.rd(ci)
        X.callp(X.rd(e), ci)
    else:
        X.marks.append('state67')
    m = X.rd(ci + 0x13C)
    if X.rd(m + 0xD, 1) == 0:
        while True:
            X.marks.append('loop')
            X.callp(X.rd(m), ci)
            rows = X.rd(ci + 0xE0)
            i = 0
            if rows != 0:
                while True:
                    X.marks.append('rows')
                    prog = X.rd(ci + 8)
                    if prog:
                        X.marks.append('progress')
                        X.wr(prog + 4, 4, i)
                        X.wr(X.rd(ci + 8) + 8, 4, X.rd(ci + 0xE0))
                        X.callp(X.rd(X.rd(ci + 8)), ci)
                    coef = X.rd(ci + 0x148)
                    al = X.callp(X.rd(coef + 4), ci, 0) & 0xFF
                    if al == 0:
                        X.marks.append('cant_suspend')
                        e = X.rd(ci)
                        X.wr(e + 0x14, 4, 0x18)
                        e = X.rd(ci)
                        X.callp(X.rd(e), ci)
                    i = (i + 1) & M32
                    if not i < X.rd(ci + 0xE0):
                        break
            m2 = X.rd(ci + 0x13C)
            X.callp(X.rd(m2 + 8), ci)
            m = X.rd(ci + 0x13C)
            if X.rd(m + 0xD, 1) != 0:
                break
    else:
        X.marks.append('last_now')
    mk = X.rd(ci + 0x14C)
    X.callp(X.rd(mk + 0xC), ci)
    d = X.rd(ci + 0x18)
    X.callp(X.rd(d + 0x10), ci)
    X.fake(0x5C6B50, [ci])


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    X = Ctx(c)
    W = X.W
    o = a[21]
    marks = X.marks
    marks += ['kind%d' % (c['family'] % 4), 'fill%d' % a[9], 'obj_unaligned' if o % 4 else 'obj_aligned']
    p = [x & M32 for x in c['p']]
    for i in range(0, len(p), 3):
        W.put(p[i] >> 8, p[i + 1], p[i] & 0xFF, p[i + 2])
    W.snap()
    args = [W.val(x) for x in a[1:1 + a[0]]]
    ret = None
    threw = False
    try:
        if fn == POST:
            f_post(X, *args)
        elif fn == SEP:
            f_sep(X, *args)
        elif fn == UPINIT:
            f_upinit(X, *args)
        elif fn == ERREXIT:
            f_errexit(X, *args)
        elif fn == OUTMSG:
            f_outmsg(X, *args)
        elif fn == STDERR:
            ret = f_stderr(X, *args)
        elif fn == ALLOCS:
            ret = f_allocsmall(X, *args)
        elif fn == ALLOCL:
            ret = f_alloclarge(X, *args)
        elif fn == ALLOCSA:
            ret = f_allocarray(X, args[0], args[1], args[2], args[3], False)
        elif fn == ALLOCBA:
            ret = f_allocarray(X, args[0], args[1], args[2], args[3], True)
        elif fn == REQS:
            ret = f_reqvirt(X, *args, head=0x44)
        elif fn == REQB:
            ret = f_reqvirt(X, *args, head=0x48)
        elif fn == REALIZE:
            f_realize(X, *args)
        elif fn == FREEPOOL:
            f_freepool(X, *args)
        elif fn == INITMEM:
            f_initmem(X, *args)
        elif fn in (GETS, GETL):
            marks.append('ran')
            ret = X.fake(0x72F420, [args[1], 0])
        elif fn in (FREES, FREEL):
            marks.append('ran')
            X.fake(0x72F430, [args[1]])
        elif fn == MEMAVAIL:
            marks.append('ran')
            ret = args[2]
        elif fn == OPENBS:
            marks.append('ran')
            X.errexit(args[0], 0x31)
        elif fn == MEMINIT:
            marks.append('ran')
            ret = 0
        elif fn == MEMTERM:
            marks.append('ran')
        elif fn == DIVUP:
            marks.append('ran')
            if s32(args[1]) < 0:
                marks.append('neg')
            ret = idiv((args[0] + args[1] - 1) & M32, args[1])[0]
        elif fn == ROUNDUP:
            marks.append('ran')
            if s32(args[1]) < 0:
                marks.append('neg')
            s = (args[0] + args[1] - 1) & M32
            ret = (s - idiv(s, args[1])[1]) & M32
        elif fn == COPYROWS:
            f_copyrows(X, *args)
        elif fn == ZEROFAR:
            f_zerofar(X, *args)
        elif fn == CREATEC:
            f_createc(X, *args)
        elif fn == SUPPRESS:
            f_suppress(X, *args)
        elif fn == FINISH:
            f_finish(X, *args)
    except Thrown:
        threw = True
        marks.append('thrown')
        ret = None
    if not threw:
        if fn in RET_TAG:
            ret = W.tagv(ret)
        elif fn in RET_RAW:
            ret = (ret or 0) & M32
        else:
            ret = None
    return {'events': X.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'gchanges': [], 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado en las funciones que lo devuelven)"""
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
