"""Oraculo independiente (Python) de Fn224A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 14 funciones del lote 224A (controlador de rasters
D3D9 de RenderWare incrustado en el EXE) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO
pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh). Las lecturas y escrituras a memoria se modelan EN EL ORDEN del EXE (importa cuando un falso cambia la
memoria o una global entre dos lecturas):
   0  0x4CB530  fl=[0xB4EA00]; nodo = [[0xC97B24]+0x144](fl, 0x30411); cabeza = [0xB4E9FC] (leida DESPUES); [nodo] = raster; [nodo+4] = cabeza; [0xB4E9FC] = nodo
   1  0x4CB5E0  mientras haya cabeza: sig = [nodo+4] (antes de liberar); [[0xC97B24]+0x148]([0xB4EA00] releida, nodo); [0xB4E9FC] = sig. Despues, si [0xB4EA00] != 0: 0x801B80(lista) y [0xB4EA00] = 0
   2  0x4CB640  por nodo: raster = [nodo], sig = [nodo+4]; ext = [0xB4E9E0] + raster; si [raster] == raster y byte [raster+0x20] <= 5: tipos 0 y 4: recurso [ext] != 0 y D3DFORMAT [ext+0x18] en [0x46, 0x50] (con
             signo): Release (ranura 2 de la vtable del objeto) y [ext] = 0; 5: igual sin el formato; 1: [ext] != 0 y != [0xC97C2C]: Release y [ext] = 0; 2: objeto [ext+0x1C] != 0: Release (sin poner a cero)
   3  0x4CBB00  ext = [0xB4E9E0] + raster; para etapa 0..7: si 0x7FDE50(etapa) == raster: 0x7FDCD0(0, etapa). Si [raster] != raster: solo con cType 2 quita de la lista (0x4CB560); devuelve 1. cType > 5 o 3: 0.
             Tipos 0 y 4 (0x4CBB4B, 0x4CBBBD): cFlags bit 7 -> 1 sin hacer nada; paleta [ext+4] != 0: quitar de la lista, indice = [[ext+4]+0x400], 0x4CB730(indice), liberar [ext+4] releida con
             +0x148 sobre [0xB4E9F8], [ext+4] = 0; si no, formato [ext+0x18] en [0x46, 0x50]: quitar de la lista; despues textura [ext]: != 0: Release (tipo 0) o 0x730B70 (tipo 4); devuelve 1.
             Tipo 1: bandera -> 1; quitar; superficie [ext] distinta de [0xC97C2C] y no nula: Release. Tipo 2: quitar. Tipo 5: bandera -> 1; quitar; textura: Release.
             0x4CB560 (quitar): busca el nodo con [nodo] == raster desde la cabeza; si es la cabeza [0xB4E9FC] = sig, si no [anterior+4] = sig; libera el nodo con +0x148([0xB4EA00], nodo).
             0x4CB730 (indice): si [0xB4E9E8] < [0xB4E9EC] (sin signo): [[0xB4E9F4] + 2*cuenta] = indice (16 bits), cuenta + 1; si no capacidad += 0x200; sin datos malloc(+0x134)(2*cap, 0x1030411),
             con datos realloc(+0x13C)(datos, 2*cap, 0x1030411); cuenta RELEIDA; [nuevo + 2*cuenta] = indice; [0xB4E9F4] = nuevo; cuenta + 1
   4  0x4CBD80  CheckDeviceFormat(uso 2, tipo 1, fmt) por la vtable de [0xC97C20]; si >= 0: CheckDepthStencilMatch(adaptFmt, adaptFmt, fmt); el formato del adaptador se lee UNA vez (esi); devuelve (hr >= 0)
   5  0x4CBDE0  CheckDeviceFormat(0, 3, fmt)
   6  0x4CBE20  CheckDeviceFormat(1, 3, fmt); si >= 0: CheckDepthStencilMatch(adaptFmt, fmt, [0xC9C068] releido)
   7  0x4CBE80  CheckDeviceFormat(2, 3, fmt); si >= 0: CheckDepthStencilMatch(adaptFmt, adaptFmt, fmt)
   8  0x4CBF20  CheckDeviceFormat(1, 5, fmt); si >= 0: CheckDepthStencilMatch(adaptFmt, fmt, [0xC9C068] releido)
   9, 10, 11  0x4CBF80, 0x4CBFD0, 0x4CC020  sin el bit 30 de [0xC9BF0C]: 0; si no CheckDeviceFormat(0x400 / 0x401 / 0x400, 3 / 3 / 5, fmt) y 1 solo si el HRESULT es exactamente 0
  12  0x4CC150  [0xB4EA00] = 0x801980(8, 0x7F, 4, 0x30411)
  13  0x4CC170  0x4CB5E0(); datos = [0xB4E9F4]; [0xB4E9E8] = [0xB4E9EC] = [0xB4E9F0] = 0; si datos != 0: +0x138(datos) y [0xB4E9F4] = 0; lista de paletas = [0xB4E9F8] (releida): si != 0: 0x801B80 y [0xB4E9F8] = 0

Los falsos anotan [id, argumentos...] (ver LiveComparison.hpp); tras cada evento puede aplicarse el efecto lateral ("poke") del caso.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["RwD3D9RasterDriver::RasterListPush", "RwD3D9RasterDriver::RasterListDestroy", "RwD3D9RasterDriver::RasterListReleaseResources", "RwD3D9RasterDriver::RasterDestroy",
         "RwD3D9RasterDriver::CheckDepthStencilSurfaceFormat", "RwD3D9RasterDriver::CheckTextureFormat", "RwD3D9RasterDriver::CheckRenderTargetTextureFormat",
         "RwD3D9RasterDriver::CheckDepthStencilTextureFormat", "RwD3D9RasterDriver::CheckRenderTargetCubeFormat", "RwD3D9RasterDriver::CheckTextureFormatUsage400",
         "RwD3D9RasterDriver::CheckTextureFormatUsage401", "RwD3D9RasterDriver::CheckCubeFormatUsage400", "RwD3D9RasterDriver::RasterListInit", "RwD3D9RasterDriver::RasterListShutdown"]
ENTRIES = (0x4CB530, 0x4CB5E0, 0x4CB640, 0x4CBB00, 0x4CBD80, 0x4CBDE0, 0x4CBE20, 0x4CBE80, 0x4CBF20, 0x4CBF80, 0x4CBFD0, 0x4CC020, 0x4CC150, 0x4CC170)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(PUSH, LDESTROY, RELEASE, RDESTROY, CHECK, BUMP, INIT, SHUT) = 'push ldestroy release rdestroy check bump init shut'.split()
KIND = [PUSH, LDESTROY, RELEASE, RDESTROY, CHECK, CHECK, CHECK, CHECK, CHECK, BUMP, BUMP, BUMP, INIT, SHUT]
GLOBALS = (0x8E2428, 0xB4E9E0, 0xB4E9E8, 0xB4E9EC, 0xB4E9F0, 0xB4E9F4, 0xB4E9F8, 0xB4E9FC, 0xB4EA00, 0xC97B24, 0xC97C20, 0xC97C24, 0xC97C2C, 0xC9BEF0, 0xC9BF0C, 0xC9C068)
G_OF = {PUSH: (7, 8, 9), LDESTROY: (7, 8, 9), RELEASE: (1, 7, 12), RDESTROY: (1, 2, 3, 5, 6, 7, 8, 9, 12), CHECK: (0, 10, 11, 13, 14, 15), BUMP: (0, 10, 11, 13, 14, 15), INIT: (8,), SHUT: (2, 3, 4, 5, 6, 9)}
ENGINE = 0xE0000000    # marcador interno del RwEngineInstance falso (nunca cambia)
HINT = 0x30411
HINT_PAL = 0x1030411


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


def no_ps(v):
    return v ^ 0x50000000 if 0xA <= (v >> 28) <= 0xD else v


def PS(arena, off):
    return ((0xA + arena) << 28) | off


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
        self.m = dict(self.m)

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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y las globales vigiladas"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.g = {}
        self.gref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def val(self, spec):
        """especificacion de puntero del caso -> valor"""
        spec &= M32
        if 0xA <= (spec >> 28) <= 0xD:
            return self.raw((spec >> 28) - 0xA, spec & 0x0FFFFFFF)
        return spec

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
        self.gref = dict(self.g)


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    PUSH: ['head_null', 'head_node', 'head_self', 'head_other', 'poke_applied'],
    LDESTROY: ['empty', 'one', 'many', 'fl_null', 'fl_set', 'head_cut', 'poke_fl', 'poke_next', 'poke_scribble'],
    RELEASE: ['empty', 'one', 'many', 't0', 't1', 't2', 't3', 't4', 't5', 't_gt5', 'nonroot', 'res_null', 'res_default', 'fmt_in', 'fmt_out', 'release_res', 'release_obj', 'zeroed', 'poke_applied'],
    RDESTROY: ['t0', 't1', 't2', 't3', 't4', 't5', 't_gt5', 'nonroot', 'flag_set', 'flag_clear', 'pal_block', 'pal_none', 'fmt_in', 'fmt_out', 'in_list', 'not_in_list', 'list_head', 'list_mid',
               'stack_fits', 'stack_realloc', 'stack_malloc', 'tex_null', 'stage_match', 'stage_none', 'poke_applied', 'surface_default', 'destroy_texture', 'release'],
    CHECK: ['first_ok', 'first_fail', 'second_ok', 'second_fail', 'swapped', 'poke_global', 'poke_vtable'],
    BUMP: ['caps_off', 'caps_on', 'hr_zero', 'hr_nonzero', 'poke_global'],
    INIT: ['create_null', 'create_set'],
    SHUT: ['data_null', 'data_set', 'pal_null', 'pal_set', 'poke_applied'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]
KEYS[5] = [k for k in KEYS[5] if k not in ('second_ok', 'second_fail', 'swapped')]


class Ctx:
    """estado de un caso: mundo, eventos, marcas y efecto lateral"""

    def __init__(self, c):
        self.c = c
        self.a = [x & M32 for x in c['a']]
        self.W = World(c)
        self.events = []
        self.marks = []
        self.pcount = 0
        self.vt = {}
        self.ret = None

    def G(self, g):
        return self.W.g[g]

    def setG(self, g, v):
        self.W.g[g] = v & M32

    def ev(self, eid, args, ptr=0):
        self.events.append([eid] + list(args))
        a = self.a
        if a[40] and eid == a[40]:
            n = self.pcount
            self.pcount += 1
            if n == a[41]:
                val = a[46] if a[45] == 0 else self.W.val(a[46])
                if a[42] == 0:
                    self.setG(a[43], val)
                else:
                    at = self.W.raw(a[43], a[44]) if a[42] == 1 else (ptr + a[44]) & M32
                    self.W.wr(at, 4, val)
                self.marks.append('poke_applied')

    def tag(self, v):
        return self.W.tagv(v)

    def release(self, obj):
        """Release por la vtable (ranura 2) del objeto: el falso de la vtable 1 o 2"""
        vt = self.W.rd(obj)
        vid = self.vt[vt]
        self.ev(0xC008 + 0x100 * (vid - 1), [self.tag(obj)], obj)


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    X = Ctx(c)
    W = X.W
    A, B, Cc, D = W.mem
    o = a[0]
    this = W.raw(0, o)
    marks = X.marks
    marks += ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    A.wr(o, 4, a[3])
    for g in G_OF[k]:
        W.g[g] = ENGINE if g == 9 else W.val(a[48 + g])
    G = X.G
    setG = X.setG

    if k == PUSH:
        node = this
        raster = W.val(a[4])
        hs = a[48 + 7]
        marks.append('head_null' if hs == 0 else ('head_self' if hs == PS(0, o) else ('head_node' if 0xA <= (hs >> 28) <= 0xD else 'head_other')))
        W.snap()
        X.ev(0xE144, [X.tag(G(8)), HINT])
        head = G(7)
        W.wr(node, 4, raster)
        W.wr(node + 4, 4, head)
        setG(7, node)
    elif k == LDESTROY:
        L, base, sep = a[4], a[5], a[6]
        nodes = [W.raw(1, base + sep * i) for i in range(L)]
        for i in range(L):
            B.wr(base + sep * i + 4, 4, nodes[i + 1] if i + 1 < L else 0)
        setG(7, nodes[0] if (L and a[7] == 0) else 0)
        W.snap()
        marks.append('empty' if L == 0 else ('one' if L == 1 else 'many'))
        marks.append('fl_set' if a[48 + 8] else 'fl_null')
        if L and a[7]:
            marks.append('head_cut')
        if a[40]:
            marks.append('poke_fl' if a[42] == 0 else ('poke_next' if a[42] == 1 else 'poke_scribble'))
        cur = G(7)
        steps = 0
        if cur:
            while True:
                nx = W.rd(cur + 4)
                X.ev(0xE148, [X.tag(G(8)), X.tag(cur)], cur)
                setG(7, nx)
                cur = nx
                steps += 1
                if steps > 64:
                    raise RuntimeError('lista sin fin en el caso')
                if cur == 0:
                    break
        fl = G(8)
        if fl:
            X.ev(0x801B80, [X.tag(fl)])
            setG(8, 0)
    elif k == RELEASE:
        L, base, sep, rbase, rsep, E = a[4], a[5], a[6], a[7], a[8], a[48 + 1]
        X.vt = {W.raw(2, a[10]): 1, W.raw(2, a[11]): 2}
        for i in range(6):
            D.wr(a[12] + 0x20 * i, 4, W.raw(2, a[11] if a[64 + 8 * i + 6] else a[10]))
            D.wr(a[12] + 0x20 * i + 0x10, 4, W.raw(2, a[11] if a[64 + 8 * i + 7] else a[10]))
        if (a[48 + 12] >> 28) == 0xD:
            D.wr(a[48 + 12] & 0x0FFFFFFF, 4, W.raw(2, a[10]))
        for i in range(L):
            B.wr(base + sep * i, 4, W.raw(0, rbase + rsep * i))
            B.wr(base + sep * i + 4, 4, W.raw(1, base + sep * (i + 1)) if i + 1 < L else 0)
        setG(7, W.raw(1, base) if L else 0)
        for i in range(L):
            p = 64 + 8 * i
            r = rbase + rsep * i
            self_ = W.raw(0, r)
            A.wr(r, 4, self_ if a[p] == 0 else (a[13] if a[p] == 1 else (self_ + 4) & M32))
            A.wr(r + 0x20, 1, a[p + 1])
            A.wr(r + 0x21, 1, a[p + 2])
            e = r + E
            A.wr(e, 4, 0 if a[p + 3] == 0 else (W.raw(3, a[12] + 0x20 * i) if a[p + 3] == 1 else W.val(a[48 + 12])))
            A.wr(e + 0x18, 4, a[p + 4])
            A.wr(e + 0x1C, 4, 0 if a[p + 5] == 0 else W.raw(3, a[12] + 0x20 * i + 0x10))
        W.snap()
        marks.append('empty' if L == 0 else ('one' if L == 1 else 'many'))
        cur = G(7)
        steps = 0
        while cur:
            r = W.rd(cur)
            nx = W.rd(cur + 4)
            ext = (G(1) + r) & M32
            if W.rd(r) == r:
                t = W.rd(r + 0x20, 1)
                marks.append('t%d' % t if t <= 5 else 't_gt5')
                if t in (0, 4, 5):
                    res = W.rd(ext)
                    go = res != 0
                    if res == 0:
                        marks.append('res_null')
                    if go and t != 5:
                        fm = s32(W.rd(ext + 0x18))
                        ok = 0x46 <= fm <= 0x50
                        marks.append('fmt_in' if ok else 'fmt_out')
                        go = ok
                    if go:
                        X.release(res)
                        W.wr(ext, 4, 0)
                        marks.append('release_res')
                        marks.append('zeroed')
                elif t == 1:
                    res = W.rd(ext)
                    if res == 0:
                        marks.append('res_null')
                    elif res == G(12):
                        marks.append('res_default')
                    else:
                        X.release(res)
                        W.wr(ext, 4, 0)
                        marks.append('release_res')
                        marks.append('zeroed')
                elif t == 2:
                    obj = W.rd(ext + 0x1C)
                    if obj:
                        X.release(obj)
                        marks.append('release_obj')
            else:
                marks.append('nonroot')
            cur = nx
            steps += 1
            if steps > 16:
                raise RuntimeError('lista sin fin en el caso')
    elif k == RDESTROY:
        E = a[48 + 1]
        X.vt = {W.raw(2, a[19]): 1}
        if (a[48 + 12] >> 28) == 0xD:
            D.wr(a[48 + 12] & 0x0FFFFFFF, 4, W.raw(2, a[19]))
        D.wr(a[20], 4, W.raw(2, a[19]))
        Cc.wr(a[10] + 0x400, 4, a[11])
        Cc.wr(a[25] + 0x400, 4, a[26])
        raster = this
        A.wr(o, 4, raster if a[5] == 0 else (a[27] if a[5] == 1 else (raster + 4) & M32))
        A.wr(o + 0x20, 1, a[6])
        A.wr(o + 0x21, 1, a[7])
        A.wr(o + E, 4, 0 if a[8] == 0 else (W.raw(3, a[20]) if a[8] == 1 else W.val(a[48 + 12])))
        A.wr(o + E + 4, 4, W.raw(2, a[10]) if a[9] else 0)
        A.wr(o + E + 0x18, 4, a[12])
        L, base, sep = a[14], a[15], a[16]
        for i in range(L):
            B.wr(base + sep * i, 4, raster if i == a[17] else no_ps(rnd(a[18], i)))
            B.wr(base + sep * i + 4, 4, W.raw(1, base + sep * (i + 1)) if i + 1 < L else 0)
        setG(7, W.raw(1, base) if L else 0)
        stage = [raster if (a[13] >> s) & 1 else a[24] for s in range(8)]
        gmalloc, grealloc = W.raw(3, a[22]), W.raw(3, a[23])
        W.snap()
        ext = (raster + G(1)) & M32      # la extension se calcula UNA vez al entrar (el falso puede cambiar [0xB4E9E0] despues)

        def remove(r):
            """0x4CB560: quita de la lista el nodo del raster y lo libera"""
            head = G(7)
            node = head
            prev = 0
            if not node:
                marks.append('not_in_list')
                return
            steps = 0
            while W.rd(node) != r:
                prev = node
                node = W.rd(node + 4)
                steps += 1
                if not node or steps > 16:
                    marks.append('not_in_list')
                    return
            marks.append('in_list')
            if prev:
                W.wr(prev + 4, 4, W.rd(node + 4))
                marks.append('list_mid')
            elif head == node:
                setG(7, W.rd(node + 4))
                marks.append('list_head')
            X.ev(0xE148, [X.tag(G(8)), X.tag(node)], node)

        def push_index(idx):
            """0x4CB730: devuelve un indice de paleta a la pila"""
            count, cap = G(2), G(3)
            if count < cap:
                marks.append('stack_fits')
                data = G(5)
                W.wr((data + 2 * count) & M32, 2, idx & 0xFFFF)
                setG(2, count + 1)
                return
            cap = (cap + 0x200) & M32
            setG(3, cap)
            data = G(5)
            if data == 0:
                marks.append('stack_malloc')
                X.ev(0xE134, [(2 * cap) & M32, HINT_PAL])
                new = gmalloc
            else:
                marks.append('stack_realloc')
                X.ev(0xE13C, [X.tag(data), (2 * cap) & M32, HINT_PAL])
                new = grealloc
            count = G(2)
            W.wr((new + 2 * count) & M32, 2, idx & 0xFFFF)
            setG(5, new)
            setG(2, count + 1)

        for s in range(8):
            X.ev(0x7FDE50, [s])
            if stage[s] == raster:
                X.ev(0x7FDCD0, [0, s])
        marks.append('stage_match' if any(v == raster for v in stage) else 'stage_none')
        ret = None
        if W.rd(raster) != raster:
            marks.append('nonroot')
            if W.rd(raster + 0x20, 1) == 2:
                marks.append('t2')
                remove(raster)
            ret = 1
        else:
            t = W.rd(raster + 0x20, 1)
            marks.append('t%d' % t if t <= 5 else 't_gt5')
            if t > 5 or t == 3:
                ret = 0
            elif t in (0, 4):
                if W.rd(raster + 0x21, 1) & 0x80:
                    marks.append('flag_set')
                else:
                    marks.append('flag_clear')
                    pal = W.rd(ext + 4)
                    if pal:
                        marks.append('pal_block')
                        remove(raster)
                        idx = W.rd((W.rd(ext + 4) + 0x400) & M32)
                        push_index(idx)
                        p = W.rd(ext + 4)
                        X.ev(0xE148, [X.tag(G(6)), X.tag(p)], p)
                        W.wr(ext + 4, 4, 0)
                    else:
                        marks.append('pal_none')
                        fm = s32(W.rd(ext + 0x18))
                        ok = 0x46 <= fm <= 0x50
                        marks.append('fmt_in' if ok else 'fmt_out')
                        if ok:
                            remove(raster)
                    tex = W.rd(ext)
                    if tex:
                        if t == 0:
                            X.release(tex)
                            marks.append('release')
                        else:
                            X.ev(0x730B70, [X.tag(tex)])
                            marks.append('destroy_texture')
                    else:
                        marks.append('tex_null')
                ret = 1
            elif t == 1:
                if W.rd(raster + 0x21, 1) & 0x80:
                    marks.append('flag_set')
                else:
                    marks.append('flag_clear')
                    remove(raster)
                    surf = W.rd(ext)
                    if surf == G(12):
                        marks.append('surface_default')
                    elif surf != 0:
                        X.release(surf)
                        marks.append('release')
                    else:
                        marks.append('tex_null')
                ret = 1
            elif t == 2:
                remove(raster)
                ret = 1
            else:   # 5
                if W.rd(raster + 0x21, 1) & 0x80:
                    marks.append('flag_set')
                else:
                    marks.append('flag_clear')
                    remove(raster)
                    tex = W.rd(ext)
                    if tex:
                        X.release(tex)
                        marks.append('release')
                    else:
                        marks.append('tex_null')
                ret = 1
        X.ret = ret
    elif k in (CHECK, BUMP):
        X.vt = {W.raw(2, a[7]): 1, W.raw(2, a[8]): 2}
        A.wr(o, 4, W.raw(2, a[7]))
        D.wr(a[9], 4, W.raw(2, a[8]))
        hr1, hr2 = a[5], a[6]
        fmt = a[4]
        W.snap()
        if a[40]:
            marks.append('poke_vtable' if a[42] == 1 else 'poke_global')
        usage, rtype, mode = {4: (2, 1, 'sa'), 5: (0, 3, 'one'), 6: (1, 3, 'rt'), 7: (2, 3, 'sa'), 8: (1, 5, 'rt'), 9: (0x400, 3, 'bump'), 10: (0x401, 3, 'bump'), 11: (0x400, 5, 'bump')}[fn]

        def query():
            dev = G(0)
            d3d = G(10)
            vid = X.vt[W.rd(d3d)]
            return dev, d3d, vid

        if mode == 'bump':
            if not (G(14) & 0x40000000):
                marks.append('caps_off')
                X.ret = 0
            else:
                marks.append('caps_on')
                dev, d3d, vid = query()
                afmt = G(13)
                adapter = G(11)
                X.ev(0xC00A + 0x100 * (vid - 1), [X.tag(d3d), adapter, dev, afmt, usage, rtype, fmt], d3d)
                marks.append('hr_zero' if hr1 == 0 else 'hr_nonzero')
                X.ret = 1 if hr1 == 0 else 0
        else:
            dev, d3d, vid = query()
            afmt = G(13)         # el formato del adaptador se lee una sola vez
            adapter = G(11)
            if vid == 2:
                marks.append('vt2_first')
            X.ev(0xC00A + 0x100 * (vid - 1), [X.tag(d3d), adapter, dev, afmt, usage, rtype, fmt], d3d)
            hr = s32(hr1)
            marks.append('first_ok' if hr >= 0 else 'first_fail')
            if mode != 'one' and hr >= 0:
                dev2, d3d2, vid2 = query()
                adapter2 = G(11)
                if d3d2 != d3d or vid2 != vid:
                    marks.append('swapped')
                if mode == 'sa':
                    args = [X.tag(d3d2), adapter2, dev2, afmt, afmt, fmt]
                else:
                    args = [X.tag(d3d2), adapter2, dev2, afmt, fmt, G(15)]
                X.ev(0xC00C + 0x100 * (vid2 - 1), args, d3d2)
                hr = s32(hr2)
                marks.append('second_ok' if hr >= 0 else 'second_fail')
            X.ret = 1 if hr >= 0 else 0
    elif k == INIT:
        W.snap()
        marks.append('create_null' if W.val(a[4]) == 0 else 'create_set')
        X.ev(0x801980, [8, 0x7F, 4, HINT])
        setG(8, W.val(a[4]))
    elif k == SHUT:
        W.snap()
        marks.append('data_set' if G(5) else 'data_null')
        marks.append('pal_set' if G(6) else 'pal_null')
        X.ev(0x4CB5E0, [])
        data = G(5)
        setG(2, 0)
        setG(3, 0)
        setG(4, 0)
        if data:
            X.ev(0xE138, [X.tag(data)])
            setG(5, 0)
        pal = G(6)
        if pal:
            X.ev(0x801B80, [X.tag(pal)])
            setG(6, 0)
    # cambios de las globales vigiladas (orden ascendente de direccion)
    gch = []
    for g in G_OF[k]:
        before, after = W.gref[g], W.g[g]
        for kb in range(4):
            if ((before >> (8 * kb)) & 0xFF) != ((after >> (8 * kb)) & 0xFF):
                gch.append([GLOBALS[g] + kb, (after >> (8 * kb)) & 0xFF])
    return {'events': X.events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': X.ret, 'threw': False, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
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
