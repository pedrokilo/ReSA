"""Oraculo independiente (Python) de Fn229A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 229A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales
(Fixtures.hpp) para conocer su imagen inicial, repite los "pokes" del caso (los objetos escritos por el generador) y modela los colaboradores falsos del banco (LiveComparison.hpp).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh):
   0  0x74E010  RpMaterialStreamGetSize: textura (RwTextureStreamGetSize 0x8045A0 + 0x34, o 0x28), _rwPluginRegistryGetSize (0x808B00, registro 0x8D62A4) + tamano + 0xC.
   1  0x74E150  _rpMaterialListDeinitialize: RpMaterialDestroy (0x74DA20) de cada puesto (con puesto = 0), RwFree del vector, ceros (lista).
   2  0x74E350  _rpMaterialListAppendMaterial.   3  0x74E450  _rpMaterialListStreamGetSize.   4  0x74E600  _rpMaterialListStreamRead.
   5  0x74E970  _rpD3D9AddDynamicGeometry.   6  0x74E9E0  _rpD3D9RemoveDynamicGeometry.   7  0x74EA40  _rpD3D9PluginAttach.   8  0x74EAB0  devuelve 1.   9  0x74EAC0  ret.
  10  0x74EAD0  tuberia de sectores: apertura.  11  0x74EB40  cierre.  12  0x74EB80  tuberia de atomos: apertura.  13  0x74EC00  cierre.
  14  0x74ECA0  _rpWorldSectorDeinstanceAll.  15  0x74ED50  _rpWorldSectorDestroyRecurse.  16  0x74EEC0  _rpSectorDefaultRenderCallBack.  17  0x74F210  RpWorldUnlock.
  18  0x74F4E0  RpWorldSectorGetWorld.  19  0x74F610  RpWorldDestroy.  20  0x74F760  RpWorldCreate.  21  0x74FBC0  RpWorldForAllMaterials.  22  0x74FC70  RpWorldForAllWorldSectors.

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["RpWorld::RpMaterialStreamGetSize", "RpWorld::_rpMaterialListDeinitialize", "RpWorld::_rpMaterialListAppendMaterial", "RpWorld::_rpMaterialListStreamGetSize",
         "RpWorld::_rpMaterialListStreamRead", "RpWorld::_rpD3D9AddDynamicGeometry", "RpWorld::_rpD3D9RemoveDynamicGeometry", "RpWorld::_rpD3D9PluginAttach",
         "RpWorld::_rpD3D9MaterialPipelineOpen", "RpWorld::_rpD3D9MaterialPipelineClose", "RpWorld::_rpD3D9WorldSectorPipelineOpen", "RpWorld::_rpD3D9WorldSectorPipelineClose",
         "RpWorld::_rpD3D9AtomicPipelineOpen", "RpWorld::_rpD3D9AtomicPipelineClose", "RpWorld::_rpWorldSectorDeinstanceAll", "RpWorld::_rpWorldSectorDestroyRecurse",
         "RpWorld::_rpSectorDefaultRenderCallBack", "RpWorld::RpWorldUnlock", "RpWorld::RpWorldSectorGetWorld", "RpWorld::RpWorldDestroy", "RpWorld::RpWorldCreate",
         "RpWorld::RpWorldForAllMaterials", "RpWorld::RpWorldForAllWorldSectors"]
ENTRIES = (0x74E010, 0x74E150, 0x74E350, 0x74E450, 0x74E600, 0x74E970, 0x74E9E0, 0x74EA40, 0x74EAB0, 0x74EAC0, 0x74EAD0, 0x74EB40, 0x74EB80, 0x74EC00, 0x74ECA0, 0x74ED50, 0x74EEC0, 0x74F210,
           0x74F4E0, 0x74F610, 0x74F760, 0x74FBC0, 0x74FC70)
assert len(NAMES) == len(ENTRIES) == 23
(MATSTRSIZE, LISTDEINIT, LISTAPPEND, LISTSTRSIZE, LISTSTRREAD, DYNADD, DYNREMOVE, PLUGINATTACH, MATPIPEOPEN, MATPIPECLOSE, SECTPIPEOPEN, SECTPIPECLOSE, ATOMPIPEOPEN, ATOMPIPECLOSE, DEINSTALL,
 DESTROYREC, DEFRENDER, WORLDUNLOCK, SECTGETWORLD, WORLDDESTROY, WORLDCREATE, FORALLMAT, FORALLSECT) = range(23)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_ENGINE = 0x20000000
STACK_TAG = 0x7F000000
# regiones globales absolutas (direccion, bytes): 0 callback anterior + cabeza + lista libre de las geometrias dinamicas, 1 desplazamiento del modulo de mundos, 2 del modulo de tuberias, 3 sizeOfStruct del registro de
# mundos, 4 del registro de sectores, 5 marca del banco; despues (relativas al motor) 6 = motor + 4 (mundo actual) y 7 = motor + 0x130 (funciones de memoria falsas)
REGIONS_ABS = ((0xC9251C, 12), (0xC9254C, 4), (0xC9BC60, 4), (0x8D62D0, 4), (0x8D736C, 4), (0xA9B0B0, 1))
PREV_CB, DYN_HEAD, DYN_POOL, WORLD_MOD, PIPE_MOD, WORLD_REG, SECTOR_REG, MARKER = 0xC9251C, 0xC92520, 0xC92524, 0xC9254C, 0xC9BC60, 0x8D62D0, 0x8D736C, 0xA9B0B0
REG_MAT = 0x8D62A4
# colas
(Q_ALLOC, Q_MALLOC, Q_REALLOC, Q_FINDOK, Q_FINDLEN, Q_FINDVER, Q_RI32OK, Q_RI32VAL, Q_MATREAD, Q_MATSIZE, Q_TEXSIZE, Q_REGSIZE, Q_MESH, Q_OPT, Q_FLCREATE, Q_PIPECREATE, Q_LOCK, Q_NODEDEF,
 Q_ADDFRAG, Q_PIPEUNLOCK, Q_BOOL0, Q_BOOL1, Q_CBVAL, Q_EXEC, Q_WUNLOCK, Q_CB, Q_INIT) = range(27)
NQ = 27
K_ERRDELTA, K_ARGS, K_Q = 30, 64, 96
EV_ALLOC, EV_FREEL, EV_MALLOC, EV_RWFREE, EV_REALLOC = 0xE144, 0xE148, 0xE134, 0xE138, 0xE13C
RETKIND = {MATSTRSIZE: 'raw', LISTDEINIT: 'tag', LISTAPPEND: 'raw', LISTSTRSIZE: 'raw', LISTSTRREAD: 'tag', DYNADD: None, DYNREMOVE: None, PLUGINATTACH: 'raw', MATPIPEOPEN: 'raw', MATPIPECLOSE: None,
           SECTPIPEOPEN: 'raw', SECTPIPECLOSE: None, ATOMPIPEOPEN: 'raw', ATOMPIPECLOSE: None, DEINSTALL: None, DESTROYREC: None, DEFRENDER: 'tag', WORLDUNLOCK: 'tag', SECTGETWORLD: 'tag',
           WORLDDESTROY: 'raw', WORLDCREATE: 'tag', FORALLMAT: 'tag', FORALLSECT: 'tag'}


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


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


class Mem:
    """arena (o region) dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, guard=GUARD):
        self.kind, self.seed, self.guard, self.m, self.r = kind, seed, guard, {}, {}
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
        return sum(self.get(self.guard + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[self.guard + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self, absolute=0):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - self.guard
            if self.m[i] != ref:
                out.append([rel + absolute, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y las regiones globales: lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = ([x & M32 for x in c['a']] + [0] * 640)[:640]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.engine = c.get('engine') or DEFAULT_ENGINE
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
        self.regions = REGIONS_ABS + ((self.engine + 4, 4), (self.engine + 0x130, 0x20))
        self.reg = [Mem(a[2], (a[90] + r * 17) & M32, guard=0) for r in range(len(self.regions))]
        self.reg[5] = Mem(a[2], 0, guard=0)
        # pokes del caso: [espacio, desplazamiento, tamano, clase, v1, v2]
        p = c['p']
        for i in range(0, len(p), 6):
            space, off, size, vk, v1, v2 = p[i:i + 6]
            value = v1
            if vk:
                value = (self.bases[v1] + GUARD + v2 - (self.engine if vk == 2 else 0)) & M32
            if space < 4:
                self.mem[space].wr(off, size, value)
            elif space == 4:
                self.wr(off, size, value)
            else:
                self.wr((self.engine + off) & M32, size, value)
        self.reg[5].wr(0, 1, 0x5A)

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return self.mem[i], addr - b - GUARD
        for r, (at, n) in enumerate(self.regions):
            if at <= addr < at + n:
                return self.reg[r], addr - at
        raise ValueError('direccion fuera de las arenas y regiones: %x' % addr)

    def rd(self, addr, n=4):
        m, off = self.locate(addr & M32)
        return m.rd(off, n)

    def wr(self, addr, n, v):
        m, off = self.locate(addr & M32)
        m.wr(off, n, v)

    def tagv(self, v):
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def res(self, tok):
        tok &= M32
        if (tok & 0xF0000000) == 0x10000000:
            ar, off = (tok >> 20) & 0xF, tok & 0xFFFFF
            if ar < 4 and off < N:
                return (self.bases[ar] + off) & M32
        return tok

    def snap(self):
        for m in self.mem:
            m.snap()
        for m in self.reg:
            m.snap()


class X:
    """estado de la ejecucion modelada: mundo, eventos, colas y marcas; los metodos f_* son los falsos del banco"""

    def __init__(self, c):
        self.c = c
        self.a = ([x & M32 for x in c['a']] + [0] * 640)[:640]
        self.W = World(c)
        self.W.snap()
        self.events = []
        self.qn = [0] * NQ
        self.marks = ['kind%d' % self.a[1], 'fill%d' % self.a[2]]
        self.engine = self.W.engine

    # --- utilidades
    def ev(self, kind, *vals):
        self.events.append([kind & M32] + [v & M32 for v in vals])

    def q(self, k):
        v = self.a[K_Q + 16 * k + (self.qn[k] & 15)]
        self.qn[k] += 1
        return v

    def tg(self, v):
        return self.W.tagv(v)

    def rd(self, addr, n=4):
        return self.W.rd(addr, n)

    def wr(self, addr, n, v):
        self.W.wr(addr, n, v)

    def arg(self, i):
        k, v1, v2 = self.a[K_ARGS + 3 * i: K_ARGS + 3 * i + 3]
        if k == 0:
            return v1
        if k == 1:
            return (self.W.bases[v1] + GUARD + v2) & M32
        return 0xCB01 + v1          # callback del banco (el valor real es una direccion de la DLL: no se compara)

    def wmod(self):
        return (self.engine + self.rd(WORLD_MOD)) & M32

    def pmod(self):
        return (self.engine + self.rd(PIPE_MOD)) & M32

    # --- funciones de memoria del motor
    def f_malloc(self, size, hint):
        self.ev(EV_MALLOC, size, hint)
        return self.W.res(self.q(Q_MALLOC))

    def f_realloc(self, p, size, hint):
        self.ev(EV_REALLOC, self.tg(p), size, hint)
        return self.W.res(self.q(Q_REALLOC))

    def f_free(self, p):
        self.ev(EV_RWFREE, self.tg(p))

    def f_alloc(self, lst, hint):
        self.ev(EV_ALLOC, self.tg(lst), hint)
        return self.W.res(self.q(Q_ALLOC))

    def f_freel(self, lst, p):
        self.ev(EV_FREEL, self.tg(lst), self.tg(p))

    # --- flujo y errores
    def f_find(self, stream, typ, plen, pver):
        self.ev(0x7ED2D0, self.tg(stream), typ, 1 if plen else 0, 1 if pver else 0)
        ok, ln, ver = self.q(Q_FINDOK), self.q(Q_FINDLEN), self.q(Q_FINDVER)
        return ok, ln, ver

    def f_readint(self, stream, n, buf):
        """RwStreamReadInt32: escribe min(n / 4, 16) palabras de la cola de valores en buf (buf = None: variable local de la pila)"""
        self.ev(0x7ED540, self.tg(stream), n, STACK_TAG if buf is None else self.tg(buf))
        words = []
        for k in range(min(n // 4, 16)):
            words.append(self.q(Q_RI32VAL))
        ok = self.q(Q_RI32OK)
        if buf is not None:
            for k, w in enumerate(words):
                self.wr(buf + 4 * k, 4, w)
        return ok, words

    def f_error(self, code, arg=0):
        self.ev(0x8088D0, code, arg if code == 0x80000013 else 0)
        return (code + self.a[K_ERRDELTA]) & M32

    def f_errorset(self, code_ret):
        self.ev(0x808820, STACK_TAG, 2, code_ret)

    def raise_error(self, code, arg=0):
        self.f_errorset(self.f_error(code, arg))

    # --- otros colaboradores
    def f_texsize(self, tex):
        self.ev(0x8045A0, self.tg(tex))
        return self.q(Q_TEXSIZE)

    def f_regsize(self, reg, obj):
        self.ev(0x808B00, reg, self.tg(obj))
        return self.q(Q_REGSIZE)

    def f_matdestroy(self, mat): self.ev(0x74DA20, self.tg(mat))

    def f_matsize(self, mat):
        self.ev(0x74E010, self.tg(mat))
        return self.q(Q_MATSIZE)

    def f_matread(self, stream):
        self.ev(0x74DD30, self.tg(stream))
        return self.W.res(self.q(Q_MATREAD))

    def f_append(self, lst, mat):
        """falso de _rpMaterialListAppendMaterial: con hueco y vector, guarda, refCount++ y numMaterials++"""
        self.ev(0x74E350, self.tg(lst), self.tg(mat))
        n, space, vec = signed(self.rd(lst + 4)), signed(self.rd(lst + 8)), self.rd(lst)
        if space > n and vec:
            self.wr(vec + 4 * n, 4, mat)
            self.wr(mat + 0x18, 2, (self.rd(mat + 0x18, 2) + 1) & 0xFFFF)
            self.wr(lst + 4, 4, n + 1)
            return n
        return M32

    def f_matlistdeinit_fake(self, lst): self.ev(0x74E150, self.tg(lst))

    def f_matinit(self, lst):
        self.ev(0x74E1B0, self.tg(lst))
        self.wr(lst + 8, 4, 0)
        self.wr(lst, 4, 0)
        self.wr(lst + 4, 4, 0)
        return lst if self.q(Q_INIT) else 0

    def f_flcreate(self, a, b, c, d):
        self.ev(0x801980, a, b, c, d)
        return self.q(Q_FLCREATE)

    def f_pipecreate(self):
        self.ev(0x8057B0)
        return self.W.res(self.q(Q_PIPECREATE))

    def f_lock(self, pipe):
        self.ev(0x806990, self.tg(pipe))
        return self.W.res(self.q(Q_LOCK))

    def f_nodedef(self, addr):
        self.ev(addr)
        return self.W.res(self.q(Q_NODEDEF))

    def f_addfrag(self, locked, b, d, e):
        self.ev(0x806BE0, self.tg(locked), b, self.tg(d), e)
        return self.W.res(self.q(Q_ADDFRAG))

    def f_pipeunlock(self, p):
        self.ev(0x805F40, self.tg(p))
        return self.q(Q_PIPEUNLOCK)

    def f_pipedestroy(self, p): self.ev(0x805820, self.tg(p))

    def f_setdefault(self, addr, p): self.ev(addr, self.tg(p))

    def f_freeres(self, entry): self.ev(0x807DE0, self.tg(entry))
    def f_tie(self, link): self.ev(0x74FFF0, self.tg(link))
    def f_lighttie(self, link): self.ev(0x74FFA0, self.tg(link))
    def f_deinitobj(self, reg, obj): self.ev(0x808740, reg, self.tg(obj))
    def f_initobj(self, reg, obj): self.ev(0x8086E0, reg, self.tg(obj))
    def f_meshdestroy(self, mesh): self.ev(0x758BC0, self.tg(mesh))

    def f_exec(self, pipe, sector, flag):
        self.ev(0x805710, self.tg(pipe), self.tg(sector), flag)
        return self.q(Q_EXEC)

    def f_meshcreate(self, n):
        self.ev(0x758A90, n)
        return self.W.res(self.q(Q_MESH))

    def f_addtri(self, mesh, mat, v0, v1, v2, mi, ti, ri, pi):
        self.ev(0x758C00, self.tg(mesh), self.tg(mat), v0, v1, v2, mi, ti, ri, pi)

    def f_optimise(self, mesh, flag):
        self.ev(0x75D970, self.tg(mesh), flag)
        return self.W.res(self.q(Q_OPT))

    def f_meshbdestroy(self, mesh): self.ev(0x758B80, self.tg(mesh))

    # --- piezas comunes
    def r_deinit(self, lst):
        """cuerpo de _rpMaterialListDeinitialize (real en la DLL; en linea en el EXE)"""
        vec = self.rd(lst)
        if vec:
            n = signed(self.rd(lst + 4))
            i = 0
            while i < n:
                self.f_matdestroy(self.rd(vec + 4 * i))
                self.wr(vec + 4 * i, 4, 0)
                i += 1
            self.f_free(vec)
            self.wr(lst, 4, 0)
        self.wr(lst + 4, 4, 0)
        self.wr(lst + 8, 4, 0)

    def release(self, s):
        """liberacion de un sector con atomos: instancia, ataduras de atomos y de luces, plugins"""
        rep = self.rd(s + 0x34)
        if rep:
            self.f_freeres(rep)
        head = s + 0x38
        link = self.rd(head)
        while link != head:
            cur = link
            link = self.rd(link)
            self.f_tie(cur)
        head = s + 0x40
        link = self.rd(head)
        while link != head:
            cur = link
            link = self.rd(link)
            self.f_lighttie(cur)
        self.f_deinitobj(0x8D736C, s)

    def unregister(self, world):
        wm = self.wmod()
        head = (wm + 4) & M32
        link = self.rd(head)
        if link == head:
            return
        while True:
            entry = (link - 8) & M32
            if self.rd(entry) == world:
                break
            link = self.rd(link)
            if link == head:
                return
        prev, nxt = self.rd(entry + 0xC), self.rd(entry + 8)
        self.wr(prev, 4, nxt)
        self.wr((nxt + 4) & M32, 4, prev)
        self.f_freel(self.rd(wm), entry)

    def destroy_meshes(self, world):
        root = self.rd(world + 0x1C)
        if not root:
            return
        stack = [0] * 70
        depth = 0
        s = root
        while True:
            if signed(self.rd(s)) < 0:
                mesh = self.rd(s + 0x78)
                if mesh:
                    self.f_meshdestroy(mesh)
                    self.wr(s + 0x78, 4, 0)
                s = stack[depth]
                depth -= 1
            else:
                left = self.rd(s + 8)
                depth += 1
                stack[depth] = self.rd(s + 0xC)
                s = left
            if depth < 0:
                break

    def destroy_body(self, world):
        self.unregister(world)
        self.destroy_meshes(world)
        self.f_matlistdeinit_fake(world + 0x10)
        root = self.rd(world + 0x1C)
        if self.rd(world + 3, 1) & 1:
            if root:
                self.ev(0x74ECA0, self.tg(root))
        else:
            if root:
                self.ev(0x74ED50, self.tg(root))
        self.f_deinitobj(0x8D62D0, world)
        self.f_free(world)

    def deinstall(self, s):
        t = signed(self.rd(s))
        if t == -2:
            return
        while True:
            if t == -1:
                self.release(s)
                mesh = self.rd(s + 0x78)
                if mesh:
                    self.f_meshdestroy(mesh)
                    self.wr(s + 0x78, 4, 0)
                return
            left = self.rd(s + 8)
            if left:
                self.deinstall(left)
            s = self.rd(s + 0xC)
            if not s:
                return
            t = signed(self.rd(s))
            if t == -2:
                return

    def destroyrec(self, s):
        t = signed(self.rd(s))
        if t == -2:
            self.f_free(s)
            return
        if t != -1:
            left = self.rd(s + 8)
            if left:
                self.destroyrec(left)
            right = self.rd(s + 0xC)
            self.wr(s + 8, 4, 0)
            if right:
                self.destroyrec(right)
            self.wr(s + 0xC, 4, 0)
            self.f_free(s)
            return
        self.release(s)
        for off in [8, 0xC, 0x30, 4] + [0x10 + 4 * i for i in range(8)]:
            p = self.rd(s + off)
            if p:
                self.f_free(p)
                self.wr(s + off, 4, 0)
        mesh = self.rd(s + 0x78)
        if mesh:
            self.f_meshdestroy(mesh)
            self.wr(s + 0x78, 4, 0)
        self.f_free(s)


def expected(c):
    x = X(c)
    a = x.a
    fn = c['fn']
    marks = x.marks
    tg = x.tg
    ret = None
    if fn == MATSTRSIZE:
        M = x.arg(0)
        tex = x.rd(M)
        size = 0x28
        if tex:
            marks.append('textured')
            size = (x.f_texsize(tex) + 0x34) & M32
        else:
            marks.append('untextured')
        ret = (x.f_regsize(REG_MAT, M) + size + 0xC) & M32
    elif fn == LISTDEINIT:
        L = x.arg(0)
        marks.append('has_vec' if x.rd(L) else 'no_vec')
        if x.rd(L) and signed(x.rd(L + 4)) > 0:
            marks.append('loop')
        x.r_deinit(L)
        ret = L
    elif fn == LISTAPPEND:
        L, mat = x.arg(0), x.arg(1)
        n, space = signed(x.rd(L + 4)), signed(x.rd(L + 8))
        if space > n:
            marks.append('hole')
            vec = x.rd(L)
            x.wr(vec + 4 * n, 4, mat)
            x.wr(mat + 0x18, 2, (x.rd(mat + 0x18, 2) + 1) & 0xFFFF)
            x.wr(L + 4, 4, n + 1)
            ret = n & M32
        else:
            newsize = (space * 4 + 0x50) & M32
            vec = x.rd(L)
            if vec:
                marks.append('grow_realloc')
                blk = x.f_realloc(vec, newsize, 0x1030008)
            else:
                marks.append('grow_malloc')
                blk = x.f_malloc(newsize, 0x1030008)
            if blk == 0:
                marks.append('nomem')
                x.raise_error(0x80000013, newsize)
                ret = M32
            else:
                x.wr(L, 4, blk)
                x.wr(L + 8, 4, (space + 0x14) & M32)
                x.wr(blk + 4 * n, 4, mat)
                x.wr(mat + 0x18, 2, (x.rd(mat + 0x18, 2) + 1) & 0xFFFF)
                x.wr(L + 4, 4, n + 1)
                ret = n & M32
    elif fn == LISTSTRSIZE:
        L = x.arg(0)
        n = signed(x.rd(L + 4))
        size = (n * 4 + 0x10) & M32
        for i in range(n):
            vec = x.rd(L)
            m = x.rd(vec + 4 * i)
            dup = False
            for j in range(i - 1, -1, -1):
                if x.rd(vec + 4 * j) == m:
                    dup = True
                    break
            if dup:
                marks.append('dup')
            else:
                marks.append('unique')
                size = (size + x.f_matsize(m) + 0xC) & M32
        ret = size
    elif fn == LISTSTRREAD:
        ret = expected_liststream(x)
    elif fn == DYNADD:
        G = x.arg(0)
        node = x.rd(DYN_HEAD)
        while node:
            if x.rd(node) == G:
                marks.append('present')
                return finish(x, None, fn)
            node = x.rd(node + 4)
        pool = x.rd(DYN_POOL)
        if not pool:
            marks.append('create_pool')
            pool = x.f_flcreate(8, 0x80, 4, 0x3050D)
            x.wr(DYN_POOL, 4, pool)
        nd = x.f_alloc(pool, 0x3050D)
        x.wr(nd, 4, G)
        x.wr(nd + 4, 4, x.rd(DYN_HEAD))
        x.wr(DYN_HEAD, 4, nd)
        marks.append('inserted')
    elif fn == DYNREMOVE:
        G = x.arg(0)
        head = x.rd(DYN_HEAD)
        if not head:
            marks.append('empty')
            return finish(x, None, fn)
        prev, node = 0, head
        while x.rd(node) != G:
            prev = node
            node = x.rd(node + 4)
            if not node:
                marks.append('absent')
                return finish(x, None, fn)
        if node == head:
            marks.append('head')
            x.wr(DYN_HEAD, 4, x.rd(node + 4))
        else:
            marks.append('middle')
            x.wr(prev + 4, 4, x.rd(node + 4))
        x.wr(node, 4, 0)
        x.f_freel(x.rd(DYN_POOL), node)
    elif fn == PLUGINATTACH:
        ok = False
        x.ev(0x755B90)
        if x.q(Q_BOOL0):
            x.ev(0x7587D0)
            if x.q(Q_BOOL1):
                ok = True
        marks.append('both' if ok else 'not_both')
        x.ev(0x7FAE30)
        cb = x.q(Q_CBVAL)
        x.wr(PREV_CB, 4, cb)
        x.ev(0x7FAE20, 0x74EA80)
        ret = 1 if ok else 0
    elif fn == MATPIPEOPEN:
        ret = 1
    elif fn == MATPIPECLOSE:
        pass
    elif fn in (SECTPIPEOPEN, ATOMPIPEOPEN):
        sector = fn == SECTPIPEOPEN
        slot = 0x58 if sector else 0x54
        node_addr = 0x75E9F0 if sector else 0x7582E0
        pipe = x.f_pipecreate()
        ret = 0
        if not pipe:
            marks.append('create_fail')
            return finish(x, ret, fn)
        x.wr(pipe + 0x2C, 4, 2)
        locked = x.f_lock(pipe)
        done = False
        if locked:
            nd = x.f_nodedef(node_addr)
            frag = x.f_addfrag(locked, 0, nd, 0)
            if x.f_pipeunlock(frag):
                x.wr(x.pmod() + slot, 4, pipe)
                x.f_setdefault(0x75E270 if sector else 0x75E2A0, pipe)
                if sector:
                    ret = 1
                    marks.append('ok')
                    done = True
                else:
                    x.ev(0x755D80)
                    if x.q(Q_BOOL0):
                        x.ev(0x760CF0)
                        ret = 1
                        marks.append('ok')
                        done = True
                    else:
                        marks.append('lights_fail')
            else:
                marks.append('unlock_fail')
        else:
            marks.append('lock_fail')
        if not done:
            x.f_pipedestroy(pipe)
    elif fn == SECTPIPECLOSE:
        x.f_setdefault(0x75E270, 0)
        slot = x.rd(x.pmod() + 0x58)
        if slot:
            marks.append('destroy')
            x.f_pipedestroy(slot)
            x.wr(x.pmod() + 0x58, 4, 0)
        else:
            marks.append('none')
    elif fn == ATOMPIPECLOSE:
        node = x.rd(DYN_HEAD)
        if node:
            marks.append('list')
            while True:
                x.wr(node, 4, 0)
                x.f_freel(x.rd(DYN_POOL), node)
                node = x.rd(node + 4)
                if not node:
                    break
            x.wr(DYN_HEAD, 4, 0)
        pool = x.rd(DYN_POOL)
        if pool:
            marks.append('pool')
            x.ev(0x801B80, tg(pool))
            x.wr(DYN_POOL, 4, 0)
        x.ev(0x75EE60)
        x.ev(0x755FE0)
        x.f_setdefault(0x75E2A0, 0)
        slot = x.rd(x.pmod() + 0x54)
        if slot:
            marks.append('destroy')
            x.f_pipedestroy(slot)
            x.wr(x.pmod() + 0x54, 4, 0)
    elif fn == DEINSTALL:
        x.deinstall(x.arg(0))
        marks.append('root%d' % (signed(x.rd(x.arg(0))) + 2 if signed(x.rd(x.arg(0))) in (-2, -1) else 9))
    elif fn == DESTROYREC:
        r = x.arg(0)
        marks.append('root%d' % (signed(x.rd(r)) + 2 if signed(x.rd(r)) in (-2, -1) else 9))
        x.destroyrec(r)
    elif fn == DEFRENDER:
        s = x.arg(0)
        ret = s
        if x.rd(s + 0x84, 2) == 0:
            marks.append('no_triangles')
        else:
            pipe = x.rd(s + 0x7C)
            if pipe:
                marks.append('sector_pipe')
            else:
                cw = x.rd((x.engine + 4) & M32)
                pipe = x.rd(cw + 0x6C)
                if pipe:
                    marks.append('world_pipe')
                else:
                    marks.append('module_pipe')
                    pipe = x.rd(x.pmod() + 0x40)
            r = x.f_exec(pipe, s, 1)
            if not r:
                marks.append('exec_fail')
                ret = 0
            else:
                marks.append('exec_ok')
    elif fn == WORLDUNLOCK:
        ret = expected_unlock(x)
    elif fn == SECTGETWORLD:
        sector = x.arg(0)
        wm = x.wmod()
        head = (wm + 4) & M32
        link = x.rd(head)
        ret = 0
        if link == head:
            marks.append('empty')
        while link != head:
            entry = (link - 8) & M32
            world = x.rd(entry)
            if x.rd(world + 3, 1) & 1:
                marks.append('single')
                start = world
                end = (start + x.rd(entry + 4)) & M32
                if sector >= start and sector < end:
                    ret = world
                    marks.append('single_hit')
                    break
            else:
                marks.append('tree')
                stack = [0] * 70
                s = x.rd(world + 0x1C)
                depth = 0
                hit = False
                while True:
                    if signed(x.rd(s)) < 0:
                        if s == sector:
                            hit = True
                            break
                        s = stack[depth]
                        depth -= 1
                    else:
                        left = x.rd(s + 8)
                        depth += 1
                        stack[depth] = x.rd(s + 0xC)
                        s = left
                    if depth < 0:
                        break
                if hit:
                    ret = world
                    marks.append('tree_hit')
                    break
            link = x.rd(link)
    elif fn == WORLDDESTROY:
        world = x.arg(0)
        marks.append('single' if x.rd(world + 3, 1) & 1 else 'tree')
        x.destroy_body(world)
        ret = 1
    elif fn == WORLDCREATE:
        ret = expected_create(x)
    elif fn == FORALLMAT:
        world = x.arg(0)
        n = signed(x.rd(world + 0x14))
        i = 0
        while i < n:
            m = x.rd((x.rd(world + 0x10) + 4 * i) & M32)
            x.ev(0xCB01, tg(m), tg(x.arg(2)))
            if not x.q(Q_CB):
                marks.append('stopped')
                break
            marks.append('step')
            i += 1
        ret = world
    elif fn == FORALLSECT:
        world = x.arg(0)
        stack = [0] * 70
        s = x.rd(world + 0x1C)
        depth = 0
        while True:
            if signed(x.rd(s)) < 0:
                x.ev(0xCB02, tg(s), tg(x.arg(2)))
                if not x.q(Q_CB):
                    marks.append('stopped')
                    break
                marks.append('step')
                s = stack[depth]
                depth -= 1
            else:
                left = x.rd(s + 8)
                depth += 1
                stack[depth] = x.rd(s + 0xC)
                s = left
            if depth < 0:
                break
        ret = world
    return finish(x, ret, fn)


def finish(x, ret, fn):
    W = x.W
    gch = []
    for r, (at, _) in enumerate(W.regions):
        gch += W.reg[r].changes(absolute=at)
    kind = RETKIND[fn]
    return {'events': x.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'vptrs': [], 'gchanges': gch,
            'ret': x.tg(ret) if kind == 'tag' else (ret & M32 if kind == 'raw' else None), 'threw': False, 'marks': x.marks}


def expected_liststream(x):
    marks = x.marks
    stream, L = x.arg(0), x.arg(1)
    ok, _, ver = x.f_find(stream, 1, True, True)
    if not ok:
        marks.append('find_fail')
        return 0
    if not 0x34000 <= ver <= 0x36003:
        marks.append('bad_version')
        x.raise_error(0x80000004)
        return 0
    ok, words = x.f_readint(stream, 4, None)
    if not ok:
        marks.append('count_fail')
        return 0
    cnt = signed(words[0])
    x.wr(L + 8, 4, 0)
    x.wr(L, 4, 0)
    x.wr(L + 4, 4, 0)
    if cnt == 0:
        marks.append('empty')
        return L
    if cnt > 0:
        size = (cnt * 4) & M32
        blk = x.f_malloc(size, 0x1030008)
        if blk == 0:
            marks.append('nomem')
            x.raise_error(0x80000013, size)
            x.r_deinit(L)
            return 0
        x.wr(L, 4, blk)
        x.wr(L + 8, 4, cnt)
    isize = (cnt * 4) & M32
    idx = x.f_malloc(isize, 0x10501)
    ok, _ = x.f_readint(stream, isize, idx)
    if not ok:
        marks.append('indices_fail')
        x.f_free(idx)
        x.r_deinit(L)
        return 0
    for i in range(cnt):
        index = signed(x.rd(idx + 4 * i))
        if index < 0:
            ok, _, v2 = x.f_find(stream, 7, False, True)
            if not ok:
                marks.append('mat_find_fail')
                x.f_free(idx)
                x.r_deinit(L)
                return 0
            if not 0x34000 <= v2 <= 0x36003:
                marks.append('mat_bad_version')
                x.raise_error(0x80000004)
                x.f_free(idx)
                x.r_deinit(L)
                return 0
            m = x.f_matread(stream)
            if not m:
                marks.append('mat_read_fail')
                x.f_free(idx)
                x.r_deinit(L)
                return 0
            marks.append('new_material')
        else:
            m = x.rd((x.rd(L) + 4 * index) & M32)
            x.wr(m + 0x18, 2, (x.rd(m + 0x18, 2) + 1) & 0xFFFF)
            marks.append('reuse')
        x.f_append(L, m)
        x.f_matdestroy(m)
    x.f_free(idx)
    marks.append('read_ok')
    return L


def expected_unlock(x):
    marks = x.marks
    world = x.arg(0)
    root = x.rd(world + 0x1C)
    if not root:
        marks.append('no_root')
        return 0
    stack = [0] * 70
    depth = 0
    s = root
    while True:
        if signed(x.rd(s)) < 0:
            vec = x.rd(world + 0x10)
            materials = (vec + 4 * x.rd(s + 0x80, 2)) & M32
            if not x.rd(s + 0x78):
                nt = x.rd(s + 0x84, 2)
                bm = x.f_meshcreate(nt)
                if not bm:
                    marks.append('mesh_null')
                    return 0
                nm = signed(x.rd(world + 0x14))
                if nm != 0:
                    marks.append('materials' if nm > 0 else 'materials_neg')
                    nbytes = (nm * 4) & M32
                    textures = x.f_malloc(nbytes, 0x10006)
                    rasters = x.f_malloc(nbytes, 0x10507)
                    pipes = x.f_malloc(nbytes, 0x10507)
                    ntex = nras = npipe = 0

                    def index_of(lst, count, value):
                        k = 0
                        if count & 0xFFFF:
                            while True:
                                if x.rd(lst + 4 * k) == value:
                                    break
                                k += 1
                                if not k < (count & 0xFFFF):
                                    break
                        return k
                    for i in range(nt):
                        tri = (x.rd(s + 4) + 8 * i) & M32
                        mi = x.rd(tri + 6, 2)
                        material = x.rd((materials + 4 * mi) & M32)
                        texture = x.rd(material)
                        ti = index_of(textures, ntex, texture)
                        if ti == (ntex & 0xFFFF):
                            x.wr(textures + 4 * ti, 4, texture)
                            ntex += 1
                            marks.append('new_texture')
                        raster = x.rd(texture) if texture else 0
                        ri = index_of(rasters, nras, raster)
                        if ri == (nras & 0xFFFF):
                            x.wr(rasters + 4 * ri, 4, raster)
                            nras += 1
                        pipe = x.rd(material + 8)
                        pi = index_of(pipes, npipe, pipe)
                        if pi == (npipe & 0xFFFF):
                            x.wr(pipes + 4 * pi, 4, pipe)
                            npipe += 1
                        x.f_addtri(bm, material, x.rd(tri, 2), x.rd(tri + 2, 2), x.rd(tri + 4, 2), mi, ti, ri, pi)
                        marks.append('triangle')
                    x.f_free(textures)
                    x.f_free(rasters)
                    x.f_free(pipes)
                flag = 1 if x.rd(world + 8, 1) & 1 else 0
                opt = x.f_optimise(bm, flag)
                if not opt:
                    marks.append('optimise_fail')
                    x.f_meshbdestroy(bm)
                    return 0
                marks.append('optimise_ok')
                x.wr(s + 0x78, 4, opt)
            else:
                marks.append('has_mesh')
            s = stack[depth]
            depth -= 1
        else:
            left = x.rd(s + 8)
            depth += 1
            stack[depth] = x.rd(s + 0xC)
            s = left
        if depth < 0:
            return world


def expected_create(x):
    marks = x.marks
    box = x.arg(0)
    s1 = x.rd(WORLD_REG)
    world = x.f_malloc(s1, 0x3000B)
    if not world:
        marks.append('world_nomem')
        x.raise_error(0x80000013, s1)
        return 0
    x.wr(world, 1, 7)
    x.wr(world + 1, 1, 0)
    x.wr(world + 2, 1, 0)
    x.wr(world + 3, 1, 0)
    x.wr(world + 4, 4, 0)
    x.f_matinit(world + 0x10)
    x.wr(world + 8, 4, 0)
    x.wr(world + 0xC, 4, 2)
    s2 = x.rd(SECTOR_REG)
    sector = x.f_malloc(s2, 0x3000B)
    if not sector:
        marks.append('sector_nomem')
        x.raise_error(0x80000013, 4)
        x.f_free(world)
        return 0
    x.wr(sector + 0x3C, 4, sector + 0x38)
    x.wr(sector + 0x38, 4, sector + 0x38)
    x.wr(sector + 0x44, 4, sector + 0x40)
    x.wr(sector, 4, M32)
    x.wr(sector + 0x40, 4, sector + 0x40)
    x.wr(sector + 0x34, 4, 0)
    x.wr(sector + 0x78, 4, 0)
    x.wr(sector + 0x82, 2, 0)
    x.wr(sector + 0x84, 2, 0)
    x.wr(sector + 8, 4, 0)
    x.wr(sector + 4, 4, 0)
    x.wr(sector + 0xC, 4, 0)
    for k in range(8):
        x.wr(sector + 0x10 + 4 * k, 4, 0)
    x.wr(sector + 0x30, 4, 0)
    bits = [x.rd(box + 4 * k) for k in range(6)]
    for k in range(6):
        x.wr(sector + 0x48 + 4 * k, 4, bits[k])
        x.wr(sector + 0x60 + 4 * k, 4, bits[k])
        x.wr(world + 0x50 + 4 * k, 4, bits[k])
    x.wr(sector + 0x7C, 4, 0)
    x.wr(world + 0x1C, 4, sector)
    x.wr(world + 0x20, 4, 0)
    for k in range(3):
        x.wr(world + 0x44 + 4 * k, 4, 0)
    x.wr(world + 0x24, 4, 0)
    x.wr(world + 0x30, 4, world + 0x2C)
    x.wr(world + 0x28, 4, world + 0x2C)
    x.wr(world + 0x2C, 4, world + 0x2C)
    x.wr(world + 0x38, 4, world + 0x34)
    x.wr(world + 0x34, 4, world + 0x34)
    x.wr(world + 0x3C, 4, world + 0x3C)
    x.wr(world + 0x40, 4, world + 0x3C)
    x.wr(world + 0x68, 4, 0x74EEC0)
    x.wr(world + 0x6C, 4, 0)
    wm = x.wmod()
    entry = x.f_alloc(x.rd(wm), 0x40507)
    if entry:
        marks.append('registered')
        x.wr(entry, 4, world)
        x.wr(entry + 4, 4, x.rd(WORLD_REG))
        head = (wm + 4) & M32
        x.wr(entry + 8, 4, x.rd(head))
        x.wr(entry + 0xC, 4, head)
        first = x.rd(head)
        x.wr((first + 4) & M32, 4, (entry + 8) & M32)
        x.wr(head, 4, (entry + 8) & M32)
    else:
        marks.append('entry_null')
    x.f_initobj(0x8D62D0, world)
    x.f_initobj(0x8D736C, sector)
    ev = x.ev
    ev(0x74F210, x.tg(world))
    if x.q(Q_WUNLOCK):
        marks.append('created')
        return world
    marks.append('unlock_fail')
    x.destroy_body(world)
    return 0


KEYS = [[] for _ in NAMES]
KEYS[MATSTRSIZE] = ['textured', 'untextured']
KEYS[LISTDEINIT] = ['has_vec', 'no_vec', 'loop']
KEYS[LISTAPPEND] = ['hole', 'grow_realloc', 'grow_malloc', 'nomem']
KEYS[LISTSTRSIZE] = ['dup', 'unique']
KEYS[LISTSTRREAD] = ['find_fail', 'bad_version', 'count_fail', 'empty', 'nomem', 'indices_fail', 'mat_find_fail', 'mat_bad_version', 'mat_read_fail', 'new_material', 'reuse', 'read_ok']
KEYS[DYNADD] = ['present', 'create_pool', 'inserted']
KEYS[DYNREMOVE] = ['empty', 'absent', 'head', 'middle']
KEYS[PLUGINATTACH] = ['both', 'not_both']
KEYS[SECTPIPEOPEN] = ['create_fail', 'lock_fail', 'unlock_fail', 'ok']
KEYS[SECTPIPECLOSE] = ['destroy', 'none']
KEYS[ATOMPIPEOPEN] = ['create_fail', 'lock_fail', 'unlock_fail', 'lights_fail', 'ok']
KEYS[ATOMPIPECLOSE] = ['list', 'pool', 'destroy']
KEYS[DEINSTALL] = ['root0', 'root1', 'root9']
KEYS[DESTROYREC] = ['root0', 'root1', 'root9']
KEYS[DEFRENDER] = ['no_triangles', 'sector_pipe', 'world_pipe', 'module_pipe', 'exec_fail', 'exec_ok']
KEYS[WORLDUNLOCK] = ['no_root', 'mesh_null', 'materials', 'materials_neg', 'new_texture', 'triangle', 'optimise_fail', 'optimise_ok', 'has_mesh']
KEYS[SECTGETWORLD] = ['empty', 'single', 'single_hit', 'tree', 'tree_hit']
KEYS[WORLDDESTROY] = ['single', 'tree']
KEYS[WORLDCREATE] = ['world_nomem', 'sector_nomem', 'registered', 'entry_null', 'created', 'unlock_fail']
KEYS[FORALLMAT] = ['stopped', 'step']
KEYS[FORALLSECT] = ['stopped', 'step']


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
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
