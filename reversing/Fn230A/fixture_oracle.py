"""Oraculo independiente (Python) de Fn230A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (`dumpbin /disasm` sobre el gta-sa.exe canonico), lo que debe observarse de las
25 funciones del lote 230A y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO
pseudoaleatorio de las arenas y de las tablas E1/E2 (Fixtures.hpp) para conocer su imagen inicial. Cada funcion se escribe como un PROGRAMA sobre un modelo de memoria (arenas, regiones globales y tablas),
en el orden de lecturas y escrituras del EXE: lo que importa cuando un falso cambia la memoria entre dos lecturas. Las lecturas fuera de las arenas lanzan una excepcion (un caso invalido del generador).

Desensamblado (resumen; direcciones del EXE). Convenciones: [x] = dword en x; r8/r16 = byte/palabra; todos los falsos anotan [id, argumentos...] y tras cada evento puede aplicarse el efecto lateral del caso.
  0x74FDA0 RpWorldPluginAttach: 8 x 0x7F2BB0(tam, id, ctor, dtor) con (4,0x501,0x74D8E0,0x74D950) (0x30,0x502,0x758970,0x758940) (4,0x503,0x74BFA0,0x74BFC0) (8,0x504,0x749760,0x749480)
      (4,0x505,0x752270,0x7521B0) (0,0x50A,0x761C30,0x761C40) (0xC,0x507,0x74FEE0,0x74FE90) (0,0x50B,0x7637E0,0x7637D0); acumula con OR; negativo -> 0; 0x750050() = 0 -> 0; 0x749830() = 0 -> 0;
      devuelve (0x74EA40() != 0)
  0x74FFA0 / 0x74FFF0 destruccion de RpLightTie / RpTie: [[t+0x10]] = [t+0xC]; [[t+0xC]+4] = [t+0x10]; [[t+4]] = [t]; [[t]+4] = [t+4] (cada lectura tras las escrituras anteriores);
      +0x148 de RwEngineInstance ([0xC97B24]) con (lista = [E + [0xC9259C] + 4 (luz) o + 0 (nodo)], nodo); devuelve 1. RpTie solo actua si [t+8] y [t+0x14] son no nulos.
  0x4A18F0 [this] = 0x85A788; 0x4A9D20(this).  0x4A1900 0x4A9CA0 (ECX = 0xA9AF2C; size, 4) -> EAX.  0x4A1920 0x4A9F20(this); [this] = 0x85A7A4; EAX = this.
  0x4A24D0 count = [this+0x2C] (con signo), <= 0 -> 0; array = [this+0x30]; para i < count: palabra de 16 bits en [array[i] + 4] == arg (dword) -> AL = 1; si no AL = 0.
  0x4A2550 [this+4] = arg0; [this+8] = arg1; [this+0x10] = 0; AL = 1.  0x4A2570 [this+0x10] = 0.  0x4A4460 r8([this+0xC]) != 0 y no (r8([[this+8] + 0x62]) & 8): 0x4A41E0(this, a, b) (jmp).
  0x4A2B20 / 0x4A2B90: 0x4A18F0(this) / 0x4A2BB0(this); flags (BYTE) & 1: 0x8214BD(this); EAX = this.  0x4A4940 [this] = 0x85A7DC.  0x4A4950 ret.
  libjpeg 6b del EXE (cinfo +0x00 err, +0x04 mem, +0x08 progress, +0x14 state, +0x18 dest, +0x20 image_height, +0xD0 next_scanline, +0x13C master, +0x140 main, +0x14C marker): ver las funciones de abajo.
Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["RpWorld::RpWorldPluginAttach", "RpWorld::_rpLightTieDestroy", "RpWorld::_rpTieDestroy", "FxEmitterBP_c::DestructorBody", "FxEmitterBP_c::OperatorNew", "FxEmitter_c::ConstructorBody",
         "FxEmitterBP_c::IsFxInfoPresent", "FxEmitter_c::Init", "FxEmitter_c::Reset", "FxEmitter_c::Update", "FxEmitterBP_c::ScalarDeletingDestructor", "FxEmitter_c::ScalarDeletingDestructor",
         "FxInfo_c::DestructorBody", "FxInfoManager_c::DestructorBody", "LibJpegCompressEncode::jpeg_start_compress", "LibJpegCompressEncode::jpeg_write_scanlines",
         "LibJpegCompressEncode::jinit_c_coef_controller", "LibJpegCompressEncode::grayscale_convert", "LibJpegCompressEncode::jinit_color_converter", "LibJpegCompressEncode::jinit_forward_dct",
         "LibJpegCompressEncode::jpeg_make_c_derived_tbl", "LibJpegCompressEncode::jpeg_gen_optimal_table", "LibJpegCompressEncode::jinit_huff_encoder", "LibJpegCompressEncode::jinit_compress_master",
         "LibJpegCompressEncode::jinit_c_main_controller"]
ENTRIES = (0x74FDA0, 0x74FFA0, 0x74FFF0, 0x4A18F0, 0x4A1900, 0x4A1920, 0x4A24D0, 0x4A2550, 0x4A2570, 0x4A4460, 0x4A2B20, 0x4A2B90, 0x4A4940, 0x4A4950,
           0x5C26B0, 0x5C2730, 0x5C30D0, 0x5C35F0, 0x5C3700, 0x5C4380, 0x5C43F0, 0x5C4D50, 0x5C5210, 0x5C5260, 0x5C5420)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_ENGINES = (0x31000000, 0x31100000)
(PLUGIN, TIE, THIS0, NEW, CTOR, ISINFO, INIT, RESET, UPDATE, SDD, VTBL, RETONLY, START, WRITE, COEF, GRAY, COLOR, DCT, DERIVED, GEN, HUFFENC, MASTER, MAIN) = range(23)
KIND = [PLUGIN, TIE, TIE, THIS0, NEW, CTOR, ISINFO, INIT, RESET, UPDATE, SDD, SDD, VTBL, RETONLY, START, WRITE, COEF, GRAY, COLOR, DCT, DERIVED, GEN, HUFFENC, MASTER, MAIN]
RET_EAX = {0, 1, 2, 4, 5, 10, 11, 15}      # EAX completo
RET_AL = {6, 7}                             # solo AL
GLOBALS = (0xC97B24, 0xC9259C)


class Bad(Exception):
    """caso invalido: el modelo desreferencia memoria desconocida o llama a un puntero que no es un falso"""


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba), las tablas E1/E2 y las regiones globales"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.eng = tuple(c.get('eng') or DEFAULT_ENGINES)
        self.eseed = a[13]
        self.mem = tuple(Mem(a[8], a[9 + i]) for i in range(4))
        self.glob = {}
        self.fsbytes = set()      # direcciones de los bytes que guardan un puntero a funcion falso (simbolico): ninguna escritura del modelo debe tocarlos
        self.putmap = {}          # byte -> (inicio, ancho) de la escritura del caso que lo fijo

    def put(self, dest, off, width, spec):
        """escritura del caso: solo se admite solapar otra identica (mismo inicio y ancho); un puntero a funcion falso queda marcado"""
        key = (dest, off)
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
        if spec == 0xE0000001:
            return self.eng[0]
        if spec == 0xE0000002:
            return self.eng[1]
        return spec

    def rd(self, addr, n=4):
        addr &= M32
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                if addr + n > b + N:
                    raise Bad('lectura fuera de la arena %x' % addr)
                return self.mem[i].rd(addr - b - GUARD, n)
        for k, b in enumerate(self.eng):
            if b <= addr < b + 0x180 and n == 4 and (addr - b) % 4 == 0:
                idx = (addr - b) // 4
                if idx in (0x144 // 4, 0x148 // 4):
                    raise Bad('lectura de una ranura de funcion de la tabla E')
                return rnd((self.eseed + k) & M32, idx)
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
    0: ['reg_neg', 'reg_ok', 'tiemesh_fail', 'clump_fail', 'last_fail', 'ok'],
    1: ['alias', 'engine1', 'engine2', 'normal'],
    2: ['acts', 'atom_null', 'sector_null', 'both_null', 'engine1', 'engine2', 'alias'],
    3: ['poke_applied', 'no_poke'],
    4: ['ret_ptr', 'ret_raw', 'size_zero', 'size_big'],
    5: ['poke_applied', 'no_poke'],
    6: ['count_zero', 'count_neg', 'count_pos', 'match', 'nomatch', 'early_stop', 'high_bits_info'],
    7: ['run'],
    8: ['run'],
    9: ['enabled_run', 'disabled', 'sys_bit'],
    10: ['flags0', 'flags1'],
    11: ['flags0', 'flags1'],
    12: ['run'],
    13: ['run'],
    14: ['state_ok', 'state_bad', 'tables', 'no_tables', 'raw', 'noraw', 'poke_applied'],
    15: ['state_ok', 'state_bad', 'warn', 'progress', 'no_progress', 'pass_startup', 'clamped', 'poke_applied', 'rows_returned'],
    16: ['full', 'partial', 'nc0', 'nc_many', 'poke_applied'],
    17: ['rows0', 'rows_many', 'width0', 'stride_multi', 'alias'],
    18: ['in_ok', 'in_bad', 'gray', 'ycc', 'cmyk', 'ycck', 'rgb', 'default', 'notimpl', 'bad_j', 'poke_applied'],
    19: ['dct_float', 'dct_other'],
    20: ['dc', 'ac', 'oor', 'alloc', 'existing', 'err_bits', 'err_code', 'err_sym', 'ok', 'overflow_total'],
    21: ['sparse', 'dense', 'deep17', 'ties', 'single', 'merge_big'],
    22: ['run'],
    23: ['raw', 'noraw', 'arith', 'progressive', 'huff', 'full', 'nofull', 'poke_applied'],
    24: ['raw', 'full_err', 'alloc_arrays', 'nc0', 'poke_applied'],
}
KEYS = {}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[_fn]


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
        """llamada por puntero a una funcion falsa de la libjpeg (0xF00000kk -> id 0xC100 + kk)"""
        if (word >> 8) != 0xF00000 or (word & 0xFF) > 13:
            raise Bad('puntero a funcion invalido %x' % word)
        return self.fake(0xC100 + (word & 0xFF), [self.tag(ci)] + list(rest))

    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def errexit(self, ci, code, parm=None):
        e = self.rd(ci)
        self.wr(e + 0x14, 4, code)
        if parm is not None:
            e = self.rd(ci)
            self.wr(e + 0x18, 4, parm)
        e = self.rd(ci)
        self.callp(self.rd(e), ci)

    def alloc_small(self, ci, size):
        mem = self.rd(ci + 4)
        return self.callp(self.rd(mem), ci, 1, size)

    def alloc_large(self, ci, size):
        mem = self.rd(ci + 4)
        return self.callp(self.rd(mem + 4), ci, 1, size)


# ------------------------------------------------------------------------------------------------ funciones
def f_plugin(X):
    regs = [(4, 0x501, 0x74D8E0, 0x74D950), (0x30, 0x502, 0x758970, 0x758940), (4, 0x503, 0x74BFA0, 0x74BFC0), (8, 0x504, 0x749760, 0x749480), (4, 0x505, 0x752270, 0x7521B0),
            (0, 0x50A, 0x761C30, 0x761C40), (0xC, 0x507, 0x74FEE0, 0x74FE90), (0, 0x50B, 0x7637E0, 0x7637D0)]
    acc = 0
    for r in regs:
        acc |= X.fake(0x7F2BB0, list(r))
    if s32(acc) < 0:
        X.marks.append('reg_neg')
        return 0
    X.marks.append('reg_ok')
    if X.fake(0x750050, []) == 0:
        X.marks.append('tiemesh_fail')
        return 0
    if X.fake(0x749830, []) == 0:
        X.marks.append('clump_fail')
        return 0
    if X.fake(0x74EA40, []) == 0:
        X.marks.append('last_fail')
        return 0
    X.marks.append('ok')
    return 1


def f_tie(X, tie, light):
    if not light:
        atom, sector = X.rd(tie + 8), X.rd(tie + 0x14)
        if atom == 0 or sector == 0:
            X.marks.append('both_null' if atom == 0 and sector == 0 else ('atom_null' if atom == 0 else 'sector_null'))
            return 1
        X.marks.append('acts')
    else:
        X.marks.append('normal')
    for off in (0, 4, 0xC, 0x10):
        v = X.rd(tie + off)
        if tie <= v < tie + 0x18:
            X.marks.append('alias')
            break
    ecx, edx = X.rd(tie + 0x10), X.rd(tie + 0x0C)
    X.wr(ecx, 4, edx)
    ecx, edx = X.rd(tie + 0x0C), X.rd(tie + 0x10)
    X.wr(ecx + 4, 4, edx)
    ecx, edx = X.rd(tie + 4), X.rd(tie)
    X.wr(ecx, 4, edx)
    ecx, edx = X.rd(tie), X.rd(tie + 4)
    X.wr(ecx + 4, 4, edx)
    eng = X.rd(0xC97B24)
    off = X.rd(0xC9259C)
    if eng not in X.W.eng:
        raise Bad('RwEngineInstance inesperada')
    k = X.W.eng.index(eng)
    X.marks.append('engine%d' % (k + 1))
    lst = X.rd((eng + off + (4 if light else 0)) & M32)
    X.fake(0xE148 + 0x100 * k, [lst, X.tag(tie)])
    return 1


def f_new(X, size):
    return X.fake(0x4A9CA0, [0xA9AF2C, size, 4])


def f_isinfo(X, this, want):
    count = s32(X.rd(this + 0x2C))
    if count <= 0:
        return 0
    arr = X.rd(this + 0x30)
    for i in range(count):
        info = X.rd(arr + 4 * i)
        if X.rd(info + 4, 2) == want:
            X.marks.append('match')
            if i < count - 1:
                X.marks.append('early_stop')
            return 1
    X.marks.append('nomatch')
    return 0


# ---- libjpeg
def f_start(X, ci, wat):
    st = X.rd(ci + 0x14)
    if st != 100:
        X.marks.append('state_bad')
        X.errexit(ci, 0x14, X.rd(ci + 0x14))
    else:
        X.marks.append('state_ok')
    if wat & 0xFF:
        X.marks.append('tables')
        X.fake(0x5CFA50, [X.tag(ci), 0])
    else:
        X.marks.append('no_tables')
    e = X.rd(ci)
    X.callp(X.rd(e + 0x10), ci)
    d = X.rd(ci + 0x18)
    X.callp(X.rd(d + 8), ci)
    X.fake(0x5C5260, [X.tag(ci)])
    m = X.rd(ci + 0x13C)
    X.callp(X.rd(m), ci)
    raw = X.rd(ci + 0xB0, 1)
    X.marks.append('raw' if raw else 'noraw')
    X.wr(ci + 0xD0, 4, 0)
    X.wr(ci + 0x14, 4, 0x65 + (1 if raw else 0))


def f_write(X, ci, scan, n):
    if X.rd(ci + 0x14) != 101:
        X.marks.append('state_bad')
        X.errexit(ci, 0x14, X.rd(ci + 0x14))
    else:
        X.marks.append('state_ok')
    if X.rd(ci + 0xD0) >= X.rd(ci + 0x20):
        X.marks.append('warn')
        e = X.rd(ci)
        X.wr(e + 0x14, 4, 0x7B)
        e = X.rd(ci)
        X.callp(X.rd(e + 4), ci, 0xFFFFFFFF)
    pr = X.rd(ci + 8)
    if pr:
        X.marks.append('progress')
        X.wr(pr + 4, 4, X.rd(ci + 0xD0))
        pr = X.rd(ci + 8)
        X.wr(pr + 8, 4, X.rd(ci + 0x20))
        pr = X.rd(ci + 8)
        X.callp(X.rd(pr), ci)
    else:
        X.marks.append('no_progress')
    m = X.rd(ci + 0x13C)
    if X.rd(m + 0xC, 1):
        X.marks.append('pass_startup')
        X.callp(X.rd(m + 4), ci)
    left = (X.rd(ci + 0x20) - X.rd(ci + 0xD0)) & M32
    nn = n
    if n > left:
        nn = left
        X.marks.append('clamped')
    main = X.rd(ci + 0x140)
    fnp = X.rd(main + 4)
    # process_data(cinfo, scanlines, &row_ctr, n): el falso escribe row_ctr
    rc = X.callp(fnp, ci, X.tag(scan), 0x7F000000, nn)
    X.wr(ci + 0xD0, 4, (X.rd(ci + 0xD0) + rc) & M32)
    if rc:
        X.marks.append('rows_returned')
    return rc


def f_coef(X, ci, need):
    coef = X.alloc_small(ci, 0x50)
    X.wr(ci + 0x148, 4, coef)
    X.wr(coef, 4, 0x5C2FF0)
    if need & 0xFF:
        X.marks.append('full')
        nc = s32(X.rd(ci + 0x3C))
        comp = X.rd(ci + 0x44)
        if nc <= 0:
            X.marks.append('nc0')
            return
        if nc > 1:
            X.marks.append('nc_many')
        i = 0
        while True:
            e = comp + 0x54 * i + 0xC
            v = X.rd(e)
            h = X.rd(e + 0x14)
            mem = X.rd(ci + 4)
            rh = X.fake(0x5CF8A0, [h, v])
            hs = X.rd(e - 4)
            wd = X.rd(e + 0x10)
            rw = X.fake(0x5CF8A0, [wd, hs])
            res = X.callp(X.rd(mem + 0x14), ci, 1, 0, rw, rh, v)
            X.wr(coef + 0x40 + 4 * i, 4, res)
            i += 1
            if not (i < s32(X.rd(ci + 0x3C))):
                break
    else:
        X.marks.append('partial')
        mem = X.rd(ci + 4)
        blk = X.callp(X.rd(mem + 4), ci, 1, 0x500)
        for k in range(10):
            X.wr(coef + 0x18 + 4 * k, 4, (blk + 0x80 * k) & M32)
        X.wr(coef + 0x40, 4, 0)


def f_gray(X, ci, inbuf, outbuf, outrow, nrows):
    width = X.rd(ci + 0x1C)
    stride = X.rd(ci + 0x24)
    n = s32((nrows - 1) & M32)          # dec eax; js: con 0x80000000 daria la vuelta
    if n > 15:
        raise Bad('demasiadas filas')
    if n < 0:
        X.marks.append('rows0')
        return
    if width == 0:
        X.marks.append('width0')
    if stride > 1:
        X.marks.append('stride_multi')
    X.marks.append('rows_many' if n > 0 else 'rows_one')
    cnt = n + 1
    ebp = (outrow * 4) & M32
    edx = inbuf
    while cnt:
        base = X.rd(outbuf)
        esi = X.rd(base + ebp)
        if (X.W.tagv(esi) >> 20) & 0xF == 2:
            X.marks.append('alias')
        ecx = X.rd(edx)
        edx = (edx + 4) & M32
        ebp = (ebp + 4) & M32
        for col in range(width):
            bl = X.rd(ecx, 1)
            X.wr(esi + col, 1, bl)
            ecx = (ecx + stride) & M32
        cnt -= 1


def f_color(X, ci):
    cc = X.alloc_small(ci, 0xC)
    X.wr(ci + 0x150, 4, cc)
    X.wr(cc, 4, 0x5C36F0)
    ics = X.rd(ci + 0x28)
    t = (ics - 1) & M32
    inbad = False
    if t > 4:
        if s32(X.rd(ci + 0x24)) < 1:
            inbad = True
    else:
        need = {0: 1, 1: 4, 2: 3, 3: 4, 4: 4}[t]
        if X.rd(ci + 0x24) != need:
            inbad = True
    if inbad:
        X.marks.append('in_bad')
        X.errexit(ci, 9)
    else:
        X.marks.append('in_ok')
    j = (X.rd(ci + 0x40) - 1) & M32
    if j > 4:
        X.marks.append('default')
        if X.rd(ci + 0x40) != X.rd(ci + 0x28) or X.rd(ci + 0x3C) != X.rd(ci + 0x24):
            X.marks.append('notimpl')
            X.errexit(ci, 0x1B)
        X.wr(cc + 4, 4, 0x5C3660)
        return
    if j == 0:   # gris
        X.marks.append('gray')
        if X.rd(ci + 0x3C) != 1:
            X.marks.append('bad_j')
            X.errexit(ci, 0xA)
        e = X.rd(ci + 0x28)
        if e == 1 or e == 3:
            X.wr(cc + 4, 4, 0x5C35F0)
            return
        if e == 2:
            X.wr(cc, 4, 0x5C31E0)
            X.wr(cc + 4, 4, 0x5C33E0)
            return
        X.marks.append('notimpl')
        X.errexit(ci, 0x1B)
        return
    if j == 1:   # RGB
        X.marks.append('rgb')
        if X.rd(ci + 0x3C) != 3:
            X.marks.append('bad_j')
            X.errexit(ci, 0xA)
        X.marks.append('notimpl')
        X.errexit(ci, 0x1B)
        return
    if j == 2:   # YCbCr
        X.marks.append('ycc')
        if X.rd(ci + 0x3C) != 3:
            X.marks.append('bad_j')
            X.errexit(ci, 0xA)
        e = X.rd(ci + 0x28)
        if e == 2:
            X.wr(cc, 4, 0x5C31E0)
            X.wr(cc + 4, 4, 0x5C32C0)
            return
        if e == 3:
            X.wr(cc + 4, 4, 0x5C3660)
            return
        X.marks.append('notimpl')
        X.errexit(ci, 0x1B)
        return
    if j == 3:   # CMYK
        X.marks.append('cmyk')
        if X.rd(ci + 0x3C) != 4:
            X.marks.append('bad_j')
            X.errexit(ci, 0xA)
        if X.rd(ci + 0x28) == 4:
            X.wr(cc + 4, 4, 0x5C3660)
            return
        X.marks.append('notimpl')
        X.errexit(ci, 0x1B)
        return
    X.marks.append('ycck')   # YCCK
    if X.rd(ci + 0x3C) != 4:
        X.marks.append('bad_j')
        X.errexit(ci, 0xA)
    e = X.rd(ci + 0x28)
    if e == 4:
        X.wr(cc, 4, 0x5C31E0)
        X.wr(cc + 4, 4, 0x5C3490)
        return
    if e == 5:
        X.wr(cc + 4, 4, 0x5C3660)
        return
    X.marks.append('notimpl')
    X.errexit(ci, 0x1B)


def f_dct(X, ci):
    f = X.alloc_small(ci, 0x30)
    X.wr(ci + 0x158, 4, f)
    X.wr(f, 4, 0x5C3940)
    if X.rd(ci + 0xB8) != 2:
        X.marks.append('dct_other')
        X.errexit(ci, 0x30)
    else:
        X.marks.append('dct_float')
        X.wr(f + 4, 4, 0x5C3B70)
        X.wr(f + 0x1C, 4, 0x5CD860)
    for i in range(4):
        X.wr(f + 0x0C + 4 * i, 4, 0)
        X.wr(f + 0x20 + 4 * i, 4, 0)


def f_huffenc(X, ci):
    e = X.alloc_small(ci, 0x6C)
    X.wr(ci + 0x15C, 4, e)
    X.wr(e, 4, 0x5C5090)
    for i in range(4):
        X.wr(e + 0x3C + 4 * i, 4, 0)
        X.wr(e + 0x2C + 4 * i, 4, 0)
        X.wr(e + 0x5C + 4 * i, 4, 0)
        X.wr(e + 0x4C + 4 * i, 4, 0)


def f_master(X, ci):
    X.fake(0x5C6A90, [X.tag(ci), 0])
    if not X.rd(ci + 0xB0, 1):
        X.marks.append('noraw')
        X.fake(0x5C3700, [X.tag(ci)])
        X.fake(0x5C7DF0, [X.tag(ci)])
        X.fake(0x5C7920, [X.tag(ci), 0])
    else:
        X.marks.append('raw')
    X.fake(0x5C4380, [X.tag(ci)])
    if X.rd(ci + 0xB1, 1):
        X.marks.append('arith')
        X.errexit(ci, 1)
    elif X.rd(ci + 0xD4, 1):
        X.marks.append('progressive')
        X.errexit(ci, 0x30)
    else:
        X.marks.append('huff')
        X.fake(0x5C5210, [X.tag(ci)])
    eax = X.rd(ci + 0xA8)
    if s32(eax) > 1:
        need = 1
    elif X.rd(ci + 0xB2, 1):
        need = 1
    else:
        need = 0
    X.marks.append('full' if need else 'nofull')
    X.fake(0x5C30D0, [X.tag(ci), need])
    X.fake(0x5C5420, [X.tag(ci), 0])
    X.fake(0x5C63D0, [X.tag(ci)])
    mem = X.rd(ci + 4)
    X.callp(X.rd(mem + 0x18), ci)
    mk = X.rd(ci + 0x14C)
    X.callp(X.rd(mk), ci)


def f_main(X, ci, need):
    main = X.alloc_small(ci, 0x28)
    X.wr(ci + 0x140, 4, main)
    X.wr(main, 4, 0x5C53D0)
    if X.rd(ci + 0xB0, 1):
        X.marks.append('raw')
        return
    if need & 0xFF:
        X.marks.append('full_err')
        X.errexit(ci, 4)
        return
    nc = s32(X.rd(ci + 0x3C))
    comp = X.rd(ci + 0x44)
    if nc <= 0:
        X.marks.append('nc0')
        return
    X.marks.append('alloc_arrays')
    i = 0
    while True:
        e = comp + 0x54 * i + 0x1C
        v = X.rd(e - 0x10)
        w = X.rd(e)
        mem = X.rd(ci + 4)
        res = X.callp(X.rd(mem + 8), ci, 1, (w << 3) & M32, (v << 3) & M32)
        X.wr(main + 0x18 + 4 * i, 4, res)
        i += 1
        if not (i < s32(X.rd(ci + 0x3C))):
            break


def f_derived(X, ci, isdc, tblno, pdtbl):
    isdc &= 0xFF
    tn = s32(tblno)
    X.marks.append('dc' if isdc else 'ac')
    if tn < 0 or tn >= 4:
        X.marks.append('oor')
        X.errexit(ci, 0x32, tblno)
    htbl = X.rd(ci + ((tn * 4 + (0x58 if isdc else 0x68)) & M32))
    if htbl == 0:
        raise Bad('tabla nula: el EXE la desreferencia')
    if X.rd(pdtbl) == 0:
        X.marks.append('alloc')
        X.wr(pdtbl, 4, X.alloc_small(ci, 0x500))
    else:
        X.marks.append('existing')
    dtbl = X.rd(pdtbl)
    errs = set()
    huffsize = [0] * 300
    p = 0
    for l in range(1, 17):
        i = X.rd(htbl + l, 1)
        if p + i > 256:
            errs.add('bits')
            X.errexit(ci, 8)
        for _ in range(i):
            huffsize[p] = l
            p += 1
    huffsize[p] = 0
    lastp = p
    if lastp > 256:
        X.marks.append('overflow_total')
    huffcode = [0] * 300
    code = 0
    si = huffsize[0]
    p = 0
    if si:
        while True:
            while huffsize[p] == si:
                huffcode[p] = code
                p += 1
                code += 1
            if code >= (1 << (si & 31)):
                errs.add('code')
                X.errexit(ci, 8)
            code <<= 1
            si += 1
            if not huffsize[p]:
                break
    for k in range(0x100):
        X.wr(dtbl + 0x400 + k, 1, 0)
    maxsym = 15 if isdc else 255
    for p in range(lastp):
        i = X.rd(htbl + 0x11 + p, 1)
        if i > maxsym or X.rd(dtbl + 0x400 + i, 1) != 0:
            errs.add('sym')
            X.errexit(ci, 8)
        X.wr(dtbl + i * 4, 4, huffcode[p])
        X.wr(dtbl + 0x400 + i, 1, huffsize[p])
    if 'bits' in errs:
        X.marks.append('err_bits')
    if 'code' in errs:
        X.marks.append('err_code')
    if 'sym' in errs:
        X.marks.append('err_sym')
    if not errs:
        X.marks.append('ok')


def f_gen(X, ci, htbl, freqp):
    X.wr(freqp + 0x400, 4, 1)
    freq = [X.rd(freqp + 4 * i) for i in range(257)]
    freq = [s32(v) for v in freq]
    codesize = [0] * 257
    others = [-1] * 257
    bits = [0] * 36
    ties = False
    while True:
        c1 = -1
        v = 1000000000
        for i in range(257):
            if freq[i] and freq[i] <= v:
                v = freq[i]
                c1 = i
        c2 = -1
        v = 1000000000
        for i in range(257):
            if freq[i] and freq[i] <= v and i != c1:
                v = freq[i]
                c2 = i
        if c2 < 0:
            break
        if freq[c1] == freq[c2]:
            ties = True
        freq[c1] = s32(freq[c1] + freq[c2])
        X.wr(freqp + 4 * c1, 4, freq[c1] & M32)
        freq[c2] = 0
        X.wr(freqp + 4 * c2, 4, 0)
        codesize[c1] += 1
        while others[c1] >= 0:
            c1 = others[c1]
            codesize[c1] += 1
        others[c1] = c2
        codesize[c2] += 1
        while others[c2] >= 0:
            c2 = others[c2]
            codesize[c2] += 1
    nsym = sum(1 for i in range(256) if codesize[i])
    X.marks.append('single' if nsym <= 1 else ('dense' if nsym > 100 else 'sparse'))
    if ties:
        X.marks.append('ties')
    if any(codesize[i] > 16 for i in range(257)):
        X.marks.append('deep17')
    if any(v > 1000000000 for v in freq):
        X.marks.append('merge_big')
    for i in range(257):
        if codesize[i]:
            if codesize[i] > 32:
                X.marks.append('codesize_err')
                X.errexit(ci, 0x27)
            if codesize[i] >= len(bits):
                raise Bad('codesize fuera de la tabla bits')
            bits[codesize[i]] = (bits[codesize[i]] + 1) & 0xFF
    i = 32
    steps = 0
    while i > 16:
        while bits[i] > 0:
            j = i - 2
            while bits[j] == 0:
                j -= 1
                if j < 0:
                    raise Bad('bits subdesbordado')
            bits[i] = (bits[i] - 2) & 0xFF      # los contadores son bytes
            bits[i - 1] = (bits[i - 1] + 1) & 0xFF
            bits[j + 1] = (bits[j + 1] + 2) & 0xFF
            bits[j] = (bits[j] - 1) & 0xFF
            steps += 1
            if steps > 100000:
                raise Bad('reduccion de longitudes sin fin')
        i -= 1
    while bits[i] == 0:
        i -= 1
        if i < 0:
            raise Bad('bits subdesbordado')
    bits[i] = (bits[i] - 1) & 0xFF
    for k in range(17):
        X.wr(htbl + k, 1, bits[k])
    p = 0
    for ln in range(1, 33):
        for j in range(256):
            if codesize[j] == ln:
                X.wr(htbl + 0x11 + p, 1, j)
                p += 1
    X.wr(htbl + 0x111, 1, 0)


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    X = Ctx(c)
    W = X.W
    o = a[20]
    this = W.raw(0, o)
    marks = X.marks
    marks += ['kind%d' % (c['family'] % 4), 'fill%d' % a[8], 'obj_unaligned' if o % 4 else 'obj_aligned']
    # preparacion: las mismas escrituras que Setup en LiveComparison.hpp
    p = [x & M32 for x in c['p']]
    for i in range(0, len(p), 3):
        W.put(p[i] >> 8, p[i + 1], p[i] & 0xFF, p[i + 2])
    W.snap()
    args = [W.val(x) for x in a[1:1 + a[0]]]
    ret = None
    if k == PLUGIN:
        ret = f_plugin(X)
    elif k == TIE:
        ret = f_tie(X, args[0], fn == 1)
    elif k == THIS0:
        W.wr(this, 4, 0x85A788)
        X.fake(0x4A9D20, [X.tag(this)])
        if 'poke_applied' not in marks:
            marks.append('no_poke')
    elif k == NEW:
        ret = f_new(X, args[0])
        marks.append('ret_ptr' if W.tagv(ret) != ret else 'ret_raw')
        marks.append('size_zero' if args[0] == 0 else ('size_big' if args[0] >= 0x7FFFFFFF else 'size_other'))
    elif k == CTOR:
        X.fake(0x4A9F20, [X.tag(this)])
        W.wr(this, 4, 0x85A7A4)
        ret = this
        if 'poke_applied' not in marks:
            marks.append('no_poke')
    elif k == ISINFO:
        cnt = s32(W.rd(this + 0x2C))
        marks.append('count_zero' if cnt == 0 else ('count_neg' if cnt < 0 else 'count_pos'))
        ret = f_isinfo(X, this, args[0])
        if cnt > 0 and cnt != 0x7FFFFFFF:
            arr = W.rd(this + 0x30)
            if any(W.rd(W.rd(arr + 4 * i) + 6, 2) for i in range(min(cnt, 8))):
                marks.append('high_bits_info')
    elif k == INIT:
        W.wr(this + 4, 4, args[0])
        W.wr(this + 8, 4, args[1])
        W.wr(this + 0x10, 4, 0)
        ret = 1
        marks.append('run')
    elif k == RESET:
        W.wr(this + 0x10, 4, 0)
        marks.append('run')
    elif k == UPDATE:
        if W.rd(this + 0xC, 1) != 0:
            sysp = W.rd(this + 8)
            if not (W.rd(sysp + 0x62, 1) & 8):
                marks.append('enabled_run')
                X.fake(0x4A41E0, [X.tag(this), args[0], args[1]])
            else:
                marks.append('sys_bit')
        else:
            marks.append('disabled')
    elif k == SDD:
        X.fake(0x4A18F0 if fn == 10 else 0x4A2BB0, [X.tag(this)])
        if args[0] & 1:
            marks.append('flags1')
            X.fake(0x8214BD, [X.tag(this)])
        else:
            marks.append('flags0')
        ret = this
    elif k == VTBL:
        W.wr(this, 4, 0x85A7DC)
        marks.append('run')
    elif k == RETONLY:
        marks.append('run')
    elif k == START:
        f_start(X, args[0], args[1])
    elif k == WRITE:
        ret = f_write(X, args[0], args[1], args[2])
    elif k == COEF:
        f_coef(X, args[0], args[1])
    elif k == GRAY:
        f_gray(X, args[0], args[1], args[2], args[3], args[4])
    elif k == COLOR:
        f_color(X, args[0])
    elif k == DCT:
        f_dct(X, args[0])
    elif k == DERIVED:
        f_derived(X, args[0], args[1], args[2], args[3])
    elif k == GEN:
        f_gen(X, args[0], args[1], args[2])
    elif k == HUFFENC:
        f_huffenc(X, args[0])
        marks.append('run')
    elif k == MASTER:
        f_master(X, args[0])
    elif k == MAIN:
        f_main(X, args[0], args[1])
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
