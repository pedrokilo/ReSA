"""Oraculo independiente (Python) de Fn228A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 228A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales
(Fixtures.hpp) para conocer su imagen inicial, repite los "pokes" del caso (los objetos escritos por el generador) y modela los colaboradores falsos del banco (LiveComparison.hpp).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh):
   0  0x74C200  RpMorphTargetCalcBoundingSphere: RwBBoxCalculate (0x808F60), centro = (sup+inf)*0.5 (x87: z se guarda como float antes de multiplicar), radio^2 maximo de (dy^2+dx^2)+dz^2 con
                fcom (solo si es mayor), RwSqrt (0x7EDB30) si > 0, radio = valor * 1.001.
   1  0x74C310  RpGeometryAddMorphTargets: tamano por morph target, RwMalloc/RwRealloc (pista 0x3000F), desplazamiento de datos byte a byte, reparto de punteros, esferas a cero.
   2  0x74C690  tres indices de 16 bits.   3  0x74C6C0  indice de material (busqueda 0x74E420 y alta 0x74E350).   4  0x74C7D0  Lock.   5  0x74C800  Unlock (malla de construccion).
   6  0x74CA90  RpGeometryCreate.  7  0x74CCB0  AddRef.  8  0x74CCC0  Destroy.  9, 10  0x74CD70, 0x74CDA0  registros de plugins (0x8D628C).  11  0x74D190  RpGeometryStreamRead.
  12  0x74D6D0  destruccion de un geometry sin referencias.  13  0x74D7D0 / 14  0x74D810  lectura y escritura de derechos de render.  15  0x74D870  propiedades de superficie por defecto.
  16  0x74D990  RpMaterialCreate.  17  0x74DA20  Destroy.  18  0x74DBC0  SetTexture.  19, 20  0x74DBF0, 0x74DC20  registros de plugins (0x8D62A4).  21  0x74DD30  RpMaterialStreamRead.

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import ctypes
import json
import math
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["RpWorld::RpMorphTargetCalcBoundingSphere", "RpWorld::RpGeometryAddMorphTargets", "RpWorld::RpGeometryTriangleSetVertexIndices", "RpWorld::RpGeometryTriangleSetMaterial",
         "RpWorld::RpGeometryLock", "RpWorld::RpGeometryUnlock", "RpWorld::RpGeometryCreate", "RpWorld::RpGeometryAddRef", "RpWorld::RpGeometryDestroy", "RpWorld::RpGeometryRegisterPlugin",
         "RpWorld::RpGeometryRegisterPluginStream", "RpWorld::RpGeometryStreamRead", "RpWorld::_rpGeometryDestroyUnreferenced", "RpWorld::_rpMaterialRightsStreamRead",
         "RpWorld::_rpMaterialRightsStreamWrite", "RpWorld::_rpMaterialSetDefaultSurfaceProperties", "RpWorld::RpMaterialCreate", "RpWorld::RpMaterialDestroy", "RpWorld::RpMaterialSetTexture",
         "RpWorld::RpMaterialRegisterPlugin", "RpWorld::RpMaterialRegisterPluginStream", "RpWorld::RpMaterialStreamRead"]
ENTRIES = (0x74C200, 0x74C310, 0x74C690, 0x74C6C0, 0x74C7D0, 0x74C800, 0x74CA90, 0x74CCB0, 0x74CCC0, 0x74CD70, 0x74CDA0, 0x74D190, 0x74D6D0, 0x74D7D0, 0x74D810, 0x74D870, 0x74D990, 0x74DA20,
           0x74DBC0, 0x74DBF0, 0x74DC20, 0x74DD30)
assert len(NAMES) == len(ENTRIES) == 22
(CALC, ADDMT, SETVI, SETMAT, LOCK, UNLOCK, CREATE, ADDREF, GEODESTROY, GEOREG, GEOREGS, GEOREAD, GEOUNREF, RIGHTSREAD, RIGHTSWRITE, SETDEF, MATCREATE, MATDESTROY, SETTEX, MATREG, MATREGS,
 MATREAD) = range(22)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_ENGINE = 0x20000000
STACK_TAG = 0x7F000000
# regiones globales (direccion, bytes): 0 sizeOfStruct del registro de geometrias, 1 propiedades de superficie por defecto, 2 datos extra de derechos, 3 id de derechos y desplazamiento de la lista de materiales, 4 marca
REGIONS = ((0x8D628C, 4), (0x8D62BC, 12), (0xC924E8, 4), (0xC92510, 8), (0xA9B0B0, 1))
GEO_SIZE_AT, DEFPROPS, RIGHTS_EXTRA, RIGHTS_ID, MODOFF, MARKER = 0x8D628C, 0x8D62BC, 0xC924E8, 0xC92510, 0xC92514, 0xA9B0B0
REG_GEO, REG_MAT = 0x8D628C, 0x8D62A4
# colas
(Q_ALLOC, Q_MALLOC, Q_REALLOC, Q_FINDOK, Q_FINDLEN, Q_FINDVER, Q_READOK, Q_REALOK, Q_RI32OK, Q_RI32VAL, Q_GEOCREATE, Q_ADDMT, Q_MATLIST, Q_UNLOCK, Q_TEX, Q_MESH, Q_OPT, Q_APPEND, Q_INIT,
 Q_WRITEOK) = range(20)
K_RDCHUNKS, K_RIGHTSID, K_RIGHTSEXTRA, K_ERRDELTA, K_REGRET, K_BBOX, K_SWAPADDR, K_SWAPVAL, K_INITREFON, K_INITREFVAL, K_ARGS, K_Q, K_DATA, DATA_WORDS = 26, 27, 28, 30, 31, 32, 40, 41, 42, 43, 64, 128, 460, 160
EV_ALLOC, EV_FREEL, EV_MALLOC, EV_RWFREE, EV_REALLOC = 0xE144, 0xE148, 0xE134, 0xE138, 0xE13C
RETKIND = {CALC: 'tag', ADDMT: 'raw', SETVI: 'tag', SETMAT: 'tag', LOCK: 'tag', UNLOCK: 'tag', CREATE: 'tag', ADDREF: 'tag', GEODESTROY: 'raw', GEOREG: 'raw', GEOREGS: 'raw', GEOREAD: 'tag',
           GEOUNREF: 'raw', RIGHTSREAD: 'tag', RIGHTSWRITE: 'tag', SETDEF: None, MATCREATE: 'tag', MATDESTROY: 'raw', SETTEX: 'tag', MATREG: 'raw', MATREGS: 'raw', MATREAD: 'tag'}


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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def f2b(x):
    """bits del float mas cercano (desbordamiento a infinito)"""
    return struct.unpack('<I', struct.pack('<f', ctypes.c_float(x).value))[0]


def b2f(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def r24(x):
    """redondeo de la x87 con palabra de control de 24 bits: mantisa de 24 bits, exponente sin limite"""
    if x != x or x in (math.inf, -math.inf) or x == 0:
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)) / (1 << 24), e)


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
        self.reg = [Mem(a[2], (a[90] + r * 17) & M32, guard=0) for r in range(len(REGIONS))]
        self.reg[4] = Mem(a[2], 0, guard=0)
        # pokes del caso: [espacio, desplazamiento, tamano, clase, v1, v2]
        p = c['p']
        for i in range(0, len(p), 6):
            space, off, size, vk, v1, v2 = p[i:i + 6]
            value = v1
            if vk:
                value = (self.bases[v1] + GUARD + v2 - (self.engine if vk == 2 else 0)) & M32
            if space < 4:
                self.mem[space].wr(off, size, value)
            else:
                self.wr(off, size, value)
        self.reg[4].wr(0, 1, 0x5A)

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return self.mem[i], addr - b - GUARD
        for r, (at, n) in enumerate(REGIONS):
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
        self.qn = [0] * 20
        self.dpos = 0
        self.nwrite = 0
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
        return v1 if k == 0 else (self.W.bases[v1] + GUARD + v2) & M32

    def matlist(self):
        base = (self.engine + self.rd(MODOFF)) & M32
        return self.rd(base)

    def data_bytes(self, n):
        """n bytes del flujo de datos (palabras a[K_DATA + i] en orden); avanza ceil(n/4) palabras"""
        out = bytearray()
        for k in range((n + 3) // 4):
            idx = self.dpos + k
            w = self.a[K_DATA + idx] if idx < DATA_WORDS else 0
            out += w.to_bytes(4, 'little')
        self.dpos += (n + 3) // 4
        return bytes(out[:n])

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

    def f_read(self, stream, n):
        """RwStreamRead: devuelve (cuenta, bytes copiados al destino)"""
        self.ev(0x7EC9D0, self.tg(stream), n)
        flag = self.q(Q_READOK)
        data = self.data_bytes(min(n, 0x200))
        return (n if flag else (n ^ 1) & M32), data

    def f_readreal(self, stream, buf, n):
        self.ev(0x7ED4F0, self.tg(stream), self.tg(buf), n)
        flag = self.q(Q_REALOK)
        data = self.data_bytes(min(n, 0x200))
        for k, byte in enumerate(data):
            self.wr(buf + k, 1, byte)
        return flag

    def f_readint(self, stream, n, buf):
        self.ev(0x7ED540, self.tg(stream), n, buf)
        val, ok = self.q(Q_RI32VAL), self.q(Q_RI32OK)
        self.wr(buf, 4, val)
        return ok

    def f_writeint(self, stream, ptr, n):
        self.ev(0x7ED460, self.tg(stream), self.tg(ptr), n, self.rd(ptr))
        if self.nwrite == 0 and self.a[K_SWAPADDR]:
            self.wr(self.W.res(self.a[K_SWAPADDR]), 4, self.W.res(self.a[K_SWAPVAL]))
        self.nwrite += 1
        return stream if self.q(Q_WRITEOK) else 0

    def f_error(self, code, arg=0):
        self.ev(0x8088D0, code, arg if code == 0x80000013 else 0)
        return (code + self.a[K_ERRDELTA]) & M32

    def f_errorset(self, code_ret):
        self.ev(0x808820, STACK_TAG, 2, code_ret)

    def raise_error(self, code, arg=0):
        self.f_errorset(self.f_error(code, arg))

    # --- registros de plugins
    def f_init(self, reg, obj):
        self.ev(0x8086E0, reg, self.tg(obj))
        if reg == REG_MAT and self.a[K_INITREFON]:
            self.wr(obj + 0x18, 2, self.a[K_INITREFVAL] & 0xFFFF)

    def f_deinit(self, reg, obj): self.ev(0x808740, reg, self.tg(obj))

    def f_readchunks(self, reg, stream, obj):
        self.ev(0x808980, reg, self.tg(stream), self.tg(obj))
        self.wr(RIGHTS_ID, 4, self.a[K_RIGHTSID])
        self.wr(RIGHTS_EXTRA, 4, self.a[K_RIGHTSEXTRA])
        return self.a[K_RDCHUNKS]

    def f_rights(self, reg, ident, obj, extra): self.ev(0x808AB0, reg, ident, self.tg(obj), extra)

    # --- otros colaboradores
    def f_freeres(self, entry): self.ev(0x807DE0, self.tg(entry))
    def f_meshdestroy(self, mesh): self.ev(0x758BC0, self.tg(mesh))
    def f_matdeinit(self, lst): self.ev(0x74E150, self.tg(lst))
    def f_texdestroy(self, tex): self.ev(0x7F3820, self.tg(tex))

    def f_texstream(self, stream):
        self.ev(0x8046E0, self.tg(stream))
        return self.W.res(self.q(Q_TEX))

    def f_append(self, lst, mat):
        self.ev(0x74E350, self.tg(lst), self.tg(mat))
        return signed(self.q(Q_APPEND))

    def f_matinit(self, lst):
        self.ev(0x74E1B0, self.tg(lst))
        self.wr(lst + 8, 4, 0)
        self.wr(lst, 4, 0)
        self.wr(lst + 4, 4, 0)
        return lst if self.q(Q_INIT) else 0

    def f_meshcreate(self, n):
        self.ev(0x758A90, n)
        return self.W.res(self.q(Q_MESH))

    def f_addtri(self, mesh, mat, v0, v1, v2, mi, ti, ri, pi):
        self.ev(0x758C00, self.tg(mesh), self.tg(mat), v0, v1, v2, mi, ti, ri, pi)

    def f_optimise(self, mesh, flag):
        self.ev(0x75D970, self.tg(mesh), flag)
        return self.W.res(self.q(Q_OPT))

    def f_meshbdestroy(self, mesh): self.ev(0x758B80, self.tg(mesh))

    def f_geocreate(self, nv, nt, fmt):
        self.ev(0x74CA90, nv, nt, fmt)
        return self.W.res(self.q(Q_GEOCREATE))

    def f_addmt_fake(self, geo, n):
        self.ev(0x74C310, self.tg(geo), n)
        return signed(self.q(Q_ADDMT))

    def f_matliststream(self, stream, lst):
        self.ev(0x74E600, self.tg(stream), self.tg(lst))
        return self.W.res(self.q(Q_MATLIST))

    def f_setdef(self, props, words):
        self.ev(0x74D870, STACK_TAG if props else 0, *(words if props else (0, 0, 0)))

    def f_unlock_fake(self, geo):
        self.ev(0x74C800, self.tg(geo))
        return self.W.res(self.q(Q_UNLOCK))

    # --- funciones reales que corren dentro de otras
    def r_lock(self, geo, mode):
        self.wr(geo + 0xC, 2, (self.rd(geo + 0xC, 2) | (mode & 0xFFFF)) & 0xFFFF)
        if mode & 1:
            mesh = self.rd(geo + 0x54)
            if mesh:
                self.f_meshdestroy(mesh)
                self.wr(geo + 0x54, 4, 0)

    def r_find(self, lst, mat):
        n = signed(self.rd(lst + 4))
        index = n
        if index > 0:
            cursor = (self.rd(lst) + 4 * index) & M32
            while True:
                cursor = (cursor - 4) & M32
                cur = self.rd(cursor)
                index -= 1
                if cur == mat:
                    return index
                if not index > 0:
                    break
        return signed(index - 1)

    def r_destroyunref(self, geo):
        mesh = self.rd(geo + 0x54)
        self.wr(geo + 0xE, 2, (self.rd(geo + 0xE, 2) + 1) & 0xFFFF)
        self.wr(geo + 0xC, 2, self.rd(geo + 0xC, 2) | 0xFFF)
        if mesh:
            self.f_meshdestroy(mesh)
            self.wr(geo + 0x54, 4, 0)
        self.f_deinit(REG_GEO, geo)
        mt = self.rd(geo + 0x5C)
        if mt:
            self.f_free(mt)
            self.wr(geo + 0x5C, 4, 0)
        self.f_matdeinit(geo + 0x20)
        self.wr(geo + 0xE, 2, (self.rd(geo + 0xE, 2) - 1) & 0xFFFF)
        self.f_free(geo)

    def r_addmt(self, geo, mt):
        """RpGeometryAddMorphTargets; devuelve el resultado con signo"""
        flags = self.rd(geo + 8)
        if flags & 0x01000000:
            size = 0x1C
        else:
            vb = signed(signed(self.rd(geo + 0x14)) * 12)
            size = signed(vb + 0x1C)
            if flags & 0x10:
                size = signed(size + vb)
        nmt = signed(self.rd(geo + 0x18))
        total = signed(signed(nmt + mt) * size) & M32
        old = self.rd(geo + 0x5C)
        blk = self.f_realloc(old, total, 0x3000F) if old else self.f_malloc(total, 0x3000F)
        if blk == 0:
            self.raise_error(0x80000013, total)
            self.marks.append('addmt_fail')
            return -1
        if old:
            self.marks.append('addmt_realloc')
            old_count = nmt
            data = signed(signed(size - 0x1C) * old_count)
            src = (blk + old_count * size - 1) & M32
            dst = (src + mt * 0x1C) & M32
            while data != 0:
                self.wr(dst, 1, self.rd(src, 1))
                dst = (dst - 1) & M32
                src = (src - 1) & M32
                data = signed(data - 1)
                if data and abs(data) > 0x10000:
                    raise ValueError('bucle de copia enorme')
        else:
            self.marks.append('addmt_malloc')
        newc = signed(nmt + mt)
        self.wr(geo + 0x18, 4, newc)
        self.wr(geo + 0x5C, 4, blk)
        cur = (blk + newc * 0x1C) & M32
        if newc > 0:
            i = 0
            while True:
                t = (self.rd(geo + 0x5C) + 0x1C * i) & M32
                self.wr(t + 0x14, 4, 0)
                self.wr(t + 0x18, 4, 0)
                if not (self.rd(geo + 8) & 0x01000000) and self.rd(geo + 0x14) != 0:
                    self.wr(t + 0x14, 4, cur)
                    cur = (cur + signed(self.rd(geo + 0x14)) * 12) & M32
                    if self.rd(geo + 8) & 0x10:
                        self.wr(t + 0x18, 4, cur)
                        cur = (cur + signed(self.rd(geo + 0x14)) * 12) & M32
                i += 1
                if not i < signed(self.rd(geo + 0x18)):
                    break
        first = signed(signed(self.rd(geo + 0x18)) - mt)
        if first < signed(self.rd(geo + 0x18)):
            while True:
                t = (self.rd(geo + 0x5C) + 0x1C * first) & M32
                for k in range(4):
                    self.wr(t + 4 + 4 * k, 4, 0)
                self.wr(t, 4, geo)
                first += 1
                if not first < signed(self.rd(geo + 0x18)):
                    break
        return signed(signed(self.rd(geo + 0x18)) - mt)

    def geo_abort(self, geo):
        """fallo de lectura de los datos (0x74D522 / 0x74D3AC / 0x74D600)"""
        rc = s16(self.rd(geo + 0xE, 2))
        if rc - 1 > 0:
            self.wr(geo + 0xE, 2, (rc - 1) & 0xFFFF)
            return
        if self.rd(geo + 0x58):
            self.f_freeres(self.rd(geo + 0x58))
        self.wr(geo + 0xE, 2, self.rd(geo + 0xE, 2))
        self.r_lock(geo, 0xFFF)
        self.f_deinit(REG_GEO, geo)
        mt = self.rd(geo + 0x5C)
        if mt:
            self.f_free(mt)
            self.wr(geo + 0x5C, 4, 0)
        self.f_matdeinit(geo + 0x20)
        self.wr(geo + 0xE, 2, (self.rd(geo + 0xE, 2) - 1) & 0xFFFF)
        self.f_free(geo)

    def geo_release(self, geo):
        """fallo tras leer los datos (0x74D5B4 / 0x74D5BE)"""
        rc = s16(self.rd(geo + 0xE, 2))
        if rc - 1 > 0:
            self.wr(geo + 0xE, 2, (rc - 1) & 0xFFFF)
            return
        if self.rd(geo + 0x58):
            self.f_freeres(self.rd(geo + 0x58))
        self.wr(geo + 0xE, 2, (self.rd(geo + 0xE, 2) - 1) & 0xFFFF)
        self.r_destroyunref(geo)


def u32w(data, i):
    return int.from_bytes(data[4 * i:4 * i + 4], 'little')


def expected(c):
    x = X(c)
    a = x.a
    fn = c['fn']
    marks = x.marks
    W = x.W
    tg = x.tg
    ret = None
    if fn == CALC:
        M, S = x.arg(0), x.arg(1)
        G = x.rd(M)
        nv = x.rd(G + 0x14)
        verts = x.rd(M + 0x14)
        x.ev(0x808F60, STACK_TAG, tg(verts), nv)
        bb = [b2f(a[K_BBOX + i]) for i in range(6)]
        sx, sy, sz = r24(bb[0] + bb[3]), r24(bb[1] + bb[4]), r24(bb[2] + bb[5])
        zs = b2f(f2b(sz))
        cx = b2f(f2b(r24(sx * 0.5)))
        cy = b2f(f2b(r24(sy * 0.5)))
        cz = b2f(f2b(r24(zs * 0.5)))
        rsq = 0.0
        if nv != 0:
            marks.append('nv_pos')
            for i in range(nv):
                vx, vy, vz = (b2f(x.rd(verts + 12 * i + 4 * k)) for k in range(3))
                dx, dy, dz = r24(vx - cx), r24(vy - cy), r24(vz - cz)
                d2 = r24(r24(r24(dy * dy) + r24(dx * dx)) + r24(dz * dz))
                if d2 > rsq:
                    rsq = b2f(f2b(d2))
                    marks.append('rsq_update')
        else:
            marks.append('nv_zero')
        if rsq > 0.0:
            x.ev(0x7EDB30, f2b(rsq))
            rsq = b2f(f2b(math.sqrt(rsq)))
            marks.append('sqrt_called')
        else:
            marks.append('sqrt_skipped')
        radius = f2b(r24(rsq * b2f(f2b(1.001))))
        x.wr(S, 4, f2b(cx))
        x.wr(S + 4, 4, f2b(cy))
        x.wr(S + 8, 4, f2b(cz))
        x.wr(S + 12, 4, radius)
        for v in (cx, cy, cz):
            if v != v:
                marks.append('center_nan')
                break
        ret = M
    elif fn == ADDMT:
        geo = x.arg(0)
        mt = signed(x.arg(1))
        flags = x.rd(geo + 8)
        marks.append('native' if flags & 0x01000000 else ('normals' if flags & 0x10 else 'plain'))
        ret = x.r_addmt(geo, mt) & M32
    elif fn == SETVI:
        T = x.arg(1)
        for k in range(3):
            x.wr(T + 2 * k, 2, x.arg(2 + k) & 0xFFFF)
        ret = x.arg(0)
    elif fn == SETMAT:
        G, T, mat = x.arg(0), x.arg(1), x.arg(2)
        if mat == 0:
            marks.append('null_material')
            x.wr(T + 6, 2, 0xFFFF)
            ret = G
        else:
            idx = x.r_find(G + 0x20, mat)
            if idx < 0:
                marks.append('not_found')
                idx = x.f_append(G + 0x20, mat)
                if idx < 0:
                    marks.append('append_fail')
                    ret = 0
                    return finish(x, ret, fn)
                marks.append('appended')
            else:
                marks.append('found')
            x.wr(T + 6, 2, idx & 0xFFFF)
            ret = G
    elif fn == LOCK:
        G, mode = x.arg(0), x.arg(1)
        x.r_lock(G, mode)
        marks.append('mode_polys' if mode & 1 else 'mode_other')
        ret = G
    elif fn == UNLOCK:
        ret = expected_unlock(x)
    elif fn == CREATE:
        ret = expected_create(x)
    elif fn == ADDREF:
        G = x.arg(0)
        x.wr(G + 0xE, 2, (x.rd(G + 0xE, 2) + 1) & 0xFFFF)
        ret = G
    elif fn == GEODESTROY:
        G = x.arg(0)
        rc = s16(x.rd(G + 0xE, 2))
        if rc - 1 > 0:
            marks.append('just_dec')
            x.wr(G + 0xE, 2, (rc - 1) & 0xFFFF)
        else:
            marks.append('destroyed')
            if x.rd(G + 0x58):
                x.f_freeres(x.rd(G + 0x58))
                marks.append('has_rep')
            mesh = x.rd(G + 0x54)
            rc2 = x.rd(G + 0xE, 2)
            x.wr(G + 0xC, 2, x.rd(G + 0xC, 2) | 0xFFF)
            x.wr(G + 0xE, 2, rc2)
            if mesh:
                x.f_meshdestroy(mesh)
                x.wr(G + 0x54, 4, 0)
                marks.append('has_mesh')
            x.f_deinit(REG_GEO, G)
            mt = x.rd(G + 0x5C)
            if mt:
                x.f_free(mt)
                x.wr(G + 0x5C, 4, 0)
                marks.append('has_morph')
            x.f_matdeinit(G + 0x20)
            x.wr(G + 0xE, 2, (x.rd(G + 0xE, 2) - 1) & 0xFFFF)
            x.f_free(G)
        ret = 1
    elif fn == GEOUNREF:
        G = x.arg(0)
        marks.append('has_mesh' if x.rd(G + 0x54) else 'no_mesh')
        marks.append('has_morph' if x.rd(G + 0x5C) else 'no_morph')
        x.r_destroyunref(G)
        ret = 1
    elif fn in (GEOREG, MATREG):
        regs = REG_GEO if fn == GEOREG else REG_MAT
        args = [x.arg(i) for i in range(5)]
        x.ev(0x8084A0, regs, *[tg(v) for v in args])
        ret = a[K_REGRET]
    elif fn in (GEOREGS, MATREGS):
        regs = REG_GEO if fn == GEOREGS else REG_MAT
        args = [x.arg(i) for i in range(4)]
        x.ev(0x8088E0, regs, *[tg(v) for v in args])
        ret = a[K_REGRET]
    elif fn == GEOREAD:
        ret = expected_georead(x)
    elif fn == RIGHTSREAD:
        stream, ln = x.arg(0), x.arg(1)
        ok = x.f_readint(stream, 4, RIGHTS_ID)
        if not ok:
            marks.append('first_fail')
            ret = 0
        elif ln == 8:
            ok2 = x.f_readint(stream, 4, RIGHTS_EXTRA)
            marks.append('len8')
            ret = stream if ok2 else 0
            marks.append('second_ok' if ok2 else 'second_fail')
        else:
            marks.append('len_other')
            ret = stream
    elif fn == RIGHTSWRITE:
        stream, mat = x.arg(0), x.arg(2)
        pipe = x.rd(mat + 8)
        if not x.f_writeint(stream, pipe + 0x2C, 4):
            marks.append('first_fail')
            ret = 0
        else:
            pipe = x.rd(mat + 8)
            if x.f_writeint(stream, pipe + 0x30, 4):
                marks.append('both_ok')
                ret = stream
            else:
                marks.append('second_fail')
                ret = 0
        if x.a[K_SWAPADDR]:
            marks.append('pipe_swapped')
    elif fn == SETDEF:
        p = x.arg(0)
        if p == 0:
            marks.append('null')
            for off in (0, 8, 4):
                x.wr(DEFPROPS + off, 4, 0x3F800000)
        else:
            marks.append('copy')
            vals = [x.rd(p + 4 * k) for k in range(3)]
            for k in range(3):
                x.wr(DEFPROPS + 4 * k, 4, vals[k])
        ret = None
    elif fn == MATCREATE:
        lst = x.matlist()
        m = x.f_alloc(lst, 0x30007)
        if m == 0:
            marks.append('alloc_null')
            ret = 0
        else:
            marks.append('created')
            props = [x.rd(DEFPROPS + 4 * k) for k in range(3)]
            x.wr(m + 0x18, 2, 1)
            x.wr(m + 4, 4, 0xFFFFFFFF)
            x.wr(m, 4, 0)
            x.wr(m + 8, 4, 0)
            for k in range(3):
                x.wr(m + 0xC + 4 * k, 4, props[k])
            x.f_init(REG_MAT, m)
            ret = m
    elif fn == MATDESTROY:
        m = x.arg(0)
        rc = s16(x.rd(m + 0x18, 2))
        if rc == 1:
            marks.append('destroy')
            x.f_deinit(REG_MAT, m)
            tex = x.rd(m)
            if tex:
                x.f_texdestroy(tex)
                marks.append('has_texture')
            x.wr(m, 4, 0)
            x.f_freel(x.matlist(), m)
        else:
            marks.append('just_dec')
            x.wr(m + 0x18, 2, (rc - 1) & 0xFFFF)
        ret = 1
    elif fn == SETTEX:
        m, tex = x.arg(0), x.arg(1)
        if tex:
            x.wr(tex + 0x54, 4, (x.rd(tex + 0x54) + 1) & M32)
            marks.append('new_texture')
        old = x.rd(m)
        if old:
            x.f_texdestroy(old)
            marks.append('old_texture')
            if old == tex:
                marks.append('same_texture')
        x.wr(m, 4, tex)
        ret = m
    elif fn == MATREAD:
        ret = expected_matread(x)
    return finish(x, ret, fn)


def finish(x, ret, fn):
    W = x.W
    gch = []
    for r in range(len(REGIONS)):
        gch += W.reg[r].changes(absolute=REGIONS[r][0])
    kind = RETKIND[fn]
    return {'events': x.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'vptrs': [], 'gchanges': gch,
            'ret': x.tg(ret) if kind == 'tag' else (ret & M32 if kind == 'raw' else None), 'threw': False, 'marks': x.marks}


def expected_unlock(x):
    marks = x.marks
    G = x.arg(0)
    if x.rd(G + 0x54):
        marks.append('has_mesh')
        return G
    nt = signed(x.rd(G + 0x10))
    mesh = x.f_meshcreate(nt)
    if not mesh:
        marks.append('mesh_null')
        return 0
    nm = signed(x.rd(G + 0x24))
    ntex = nras = npipe = 0
    if nm > 0:
        marks.append('materials')
        nbytes = (nm * 4) & M32
        textures = x.f_malloc(nbytes, 0x10006)
        rasters = x.f_malloc(nbytes, 0x10503)
        pipes = x.f_malloc(nbytes, 0x10503)
        if nt > 0:
            marks.append('triangles')
            for ti in range(nt):
                tri = (x.rd(G + 0x2C) + 8 * ti) & M32
                mi = x.rd(tri + 6, 2)
                material = x.rd((x.rd(G + 0x20) + 4 * mi) & M32)
                texture = x.rd(material)

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
                ti_ = index_of(textures, ntex, texture)
                if ti_ == (ntex & 0xFFFF):
                    x.wr(textures + 4 * ti_, 4, texture)
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
                x.f_addtri(mesh, material, x.rd(tri, 2), x.rd(tri + 2, 2), x.rd(tri + 4, 2), mi, ti_, ri, pi)
                if ti_ > 0:
                    marks.append('tex_reuse_or_second')
        x.f_free(textures)
        x.f_free(rasters)
        x.f_free(pipes)
    flag = 1 if x.rd(G + 8) & 1 else 0
    opt = x.f_optimise(mesh, flag)
    if opt:
        marks.append('optimise_ok')
        x.wr(G + 0x54, 4, opt)
        return G
    marks.append('optimise_fail')
    x.f_meshbdestroy(mesh)
    return 0


def expected_create(x):
    marks = x.marks
    nv, nt, fmt = signed(x.arg(0)), signed(x.arg(1)), x.arg(2)
    if nv < 0:
        marks.append('numvert_neg')
        return 0
    if nv >= 0x10000:
        marks.append('numvert_big')
        x.raise_error(6)
        return 0
    if nt < 0:
        marks.append('numtri_neg')
        return 0
    S = x.rd(GEO_SIZE_AT)
    alloc = S & M32
    if fmt & 0xFF0000:
        nsets = (fmt >> 16) & 0xFF
    elif (fmt & 0x80):
        nsets = 2
    else:
        nsets = (fmt >> 2) & 1
    tex = 4 if nsets == 1 else (0x80 if nsets > 1 else 0)
    eff = (fmt & 0x7B) | tex
    native = fmt & 0x01000000
    if not native:
        if eff & 8:
            alloc = (alloc + nv * 4) & M32
        if nsets > 0:
            alloc = (alloc + nsets * nv * 8) & M32
        alloc = (alloc + nt * 8) & M32
        marks.append('data')
    else:
        marks.append('native')
    geo = x.f_malloc(alloc, 0x3000F)
    if geo == 0:
        marks.append('malloc_null')
        return 0
    if not x.f_matinit(geo + 0x20):
        marks.append('init_null')
        return 0
    x.wr(geo + 0x5C, 4, 0)
    x.wr(geo + 0x18, 4, 0)
    x.wr(geo + 4, 4, 0)
    x.wr(geo + 0x58, 4, 0)
    x.wr(geo + 0xC, 2, 0)
    x.wr(geo + 0x54, 4, 0)
    x.wr(geo + 0, 1, 8)
    x.wr(geo + 1, 1, 0)
    x.wr(geo + 2, 1, 0)
    x.wr(geo + 3, 1, 0)
    x.wr(geo + 0xE, 2, 1)
    x.wr(geo + 0x1C, 4, nsets)
    for k in range(8):
        x.wr(geo + 0x34 + 4 * k, 4, 0)
    x.wr(geo + 0x30, 4, 0)
    x.wr(geo + 0x2C, 4, 0)
    x.wr(geo + 0x10, 4, nt)
    x.wr(geo + 8, 4, (fmt & 0x0F000000) | eff)
    x.wr(geo + 0x14, 4, nv)
    if not native:
        cursor = (geo + S) & M32
        if (eff & 8) and nv != 0:
            x.wr(geo + 0x30, 4, cursor)
            cursor = (cursor + nv * 4) & M32
            marks.append('prelit')
        if nsets > 0 and nv != 0:
            for k in range(nsets):
                x.wr(geo + 0x34 + 4 * k, 4, cursor)
                cursor = (cursor + nv * 8) & M32
            marks.append('sets%d' % min(nsets, 9))
        if nt != 0:
            x.wr(geo + 0x2C, 4, cursor)
            for i in range(nt):
                x.wr(((x.rd(geo + 0x2C)) + 8 * i + 6) & M32, 2, 0xFFFF)
            marks.append('tris')
    if x.r_addmt(geo, 1) < 0:
        marks.append('addmt_failed')
        x.f_matdeinit(geo + 0x20)
        x.f_free(geo)
        return 0
    x.f_init(REG_GEO, geo)
    marks.append('created')
    return geo


def expected_georead(x):
    marks = x.marks
    stream = x.arg(0)
    ok, _, ver = x.f_find(stream, 1, 0, 1)
    if not ok:
        marks.append('find_fail')
        return 0
    if not 0x34000 <= ver <= 0x36003:
        marks.append('bad_version')
        x.raise_error(0x80000004)
        return 0
    cnt, d = x.f_read(stream, 0x10)
    if cnt != 0x10:
        marks.append('info_short')
        return 0
    props = None
    if ver < 0x34001:
        cnt2, d2 = x.f_read(stream, 0xC)
        if cnt2 != 0xC:
            marks.append('props_short')
            return 0
        props = [u32w(d2, i) for i in range(3)]
        marks.append('props')
    fmt, nt, nv, nmt = (u32w(d, i) for i in range(4))
    g = x.f_geocreate(nv, nt, fmt)
    if g == 0:
        marks.append('create_null')
        return 0
    if signed(nmt) > 1:
        marks.append('addmt')
        if x.f_addmt_fake(g, (nmt - 1) & M32) < 0:
            marks.append('addmt_fail')
            x.geo_abort(g)
            return 0
    flags = x.rd(g + 8)
    if not (flags & 0x01000000) and x.rd(g + 0x14) != 0:
        numv = x.rd(g + 0x14)
        if fmt & 8:
            n = (numv * 4) & M32
            cnt, data = x.f_read(stream, n)
            dst = x.rd(g + 0x30)
            for k, byte in enumerate(data):
                x.wr(dst + k, 1, byte)
            marks.append('prelit')
            if cnt != n:
                marks.append('prelit_fail')
                x.geo_abort(g)
                return 0
        if signed(x.rd(g + 0x1C)) > 0:
            nbytes = (numv * 8) & M32
            for s in range(signed(x.rd(g + 0x1C))):
                if not x.f_readreal(stream, x.rd(g + 0x34 + 4 * s), nbytes):
                    marks.append('tc_fail')
                    x.geo_abort(g)
                    return 0
            marks.append('texcoords')
        if x.rd(g + 0x10) != 0:
            n = (x.rd(g + 0x10) * 8) & M32
            cnt, data = x.f_read(stream, n)
            dst = x.rd(g + 0x2C)
            for k, byte in enumerate(data):
                x.wr(dst + k, 1, byte)
            if cnt != n:
                marks.append('tri_fail')
                x.geo_abort(g)
                return 0
            ptr = x.rd(g + 0x2C)
            remaining = x.rd(g + 0x10)
            while True:
                w = [x.rd(ptr + 2 * k, 2) for k in range(4)]
                x.wr(ptr, 2, w[1])
                x.wr(ptr + 2, 2, w[0])
                x.wr(ptr + 4, 2, w[3])
                x.wr(ptr + 6, 2, w[2])
                ptr = (ptr + 8) & M32
                remaining = (remaining - 1) & M32
                if remaining == 0:
                    break
            marks.append('tris')
    if signed(x.rd(g + 0x18)) > 0:
        index = 0
        while True:
            t = (x.rd(g + 0x5C) + 0x1C * index) & M32
            cnt, data = x.f_read(stream, 0x18)
            if cnt != 0x18:
                marks.append('morph_short')
                x.geo_abort(g)
                return 0
            for k in range(4):
                x.wr(t + 4 + 4 * k, 4, u32w(data, k))
            hv, hn = u32w(data, 4), u32w(data, 5)
            numv = x.rd(g + 0x14)
            vb = (numv * 12) & M32
            if hv:
                if not x.f_readreal(stream, x.rd(t + 0x14), (vb * 2) & M32 if hn else vb):
                    marks.append('morph_verts_fail')
                    x.geo_abort(g)
                    return 0
                marks.append('morph_verts_normals' if hn else 'morph_verts')
            elif hn:
                if not x.f_readreal(stream, x.rd(t + 0x18), vb):
                    marks.append('morph_normals_fail')
                    x.geo_abort(g)
                    return 0
                marks.append('morph_normals')
            index += 1
            if not index < signed(x.rd(g + 0x18)):
                break
    ok, _, ver2 = x.f_find(stream, 8, 0, 1)
    if not ok:
        marks.append('matlist_find_fail')
        return 0
    if not 0x34000 <= ver2 <= 0x36003:
        marks.append('matlist_bad_version')
        x.geo_abort(g)
        x.raise_error(0x80000004)
        return 0
    if ver2 < 0x34001:
        x.f_setdef(True, props if props else [0, 0, 0])
        marks.append('setdef_props')
    if not x.f_matliststream(stream, g + 0x20):
        marks.append('matlist_fail')
        x.geo_release(g)
        return 0
    if ver2 < 0x34001:
        x.f_setdef(False, None)
    if not x.f_readchunks(REG_GEO, stream, g):
        marks.append('plugins_fail')
        x.geo_release(g)
        return 0
    if not x.f_unlock_fake(g):
        marks.append('unlock_fail')
        x.geo_release(g)
        return 0
    marks.append('read_ok')
    return g


def expected_matread(x):
    marks = x.marks
    stream = x.arg(0)
    ok, ln, ver = x.f_find(stream, 1, 1, 1)
    if not ok:
        marks.append('find_fail')
        return 0
    if not 0x34000 <= ver <= 0x36003:
        marks.append('bad_version')
        x.raise_error(0x80000004)
        return 0
    cnt, data = x.f_read(stream, ln)
    chunk = (data + bytes(0x1C))[:0x1C]
    if cnt != ln:
        marks.append('read_short')
        return 0
    lst = x.matlist()
    m = x.f_alloc(lst, 0x30007)
    if m == 0:
        marks.append('alloc_null')
        return 0
    props = [x.rd(DEFPROPS + 4 * k) for k in range(3)]
    x.wr(m + 0x18, 2, 1)
    x.wr(m + 4, 4, 0xFFFFFFFF)
    x.wr(m, 4, 0)
    x.wr(m + 8, 4, 0)
    for k in range(3):
        x.wr(m + 0xC + 4 * k, 4, props[k])
    x.f_init(REG_MAT, m)
    x.wr(m + 4, 4, u32w(chunk, 1))
    for k in range(3):
        x.wr(m + 0xC + 4 * k, 4, u32w(chunk, 4 + k))
    x.wr(m, 4, 0)
    textured = u32w(chunk, 3)

    def destroy_or_release():
        rc = s16(x.rd(m + 0x18, 2))
        if rc != 1:
            marks.append('release_dec')
            x.wr(m + 0x18, 2, (rc - 1) & 0xFFFF)
            return
        marks.append('cleanup_destroy')
        x.f_deinit(REG_MAT, m)
        if x.rd(m):
            x.f_texdestroy(x.rd(m))
        x.wr(m, 4, 0)
        x.f_freel(lst, m)
    if textured:
        marks.append('textured')
        ok, _, v2 = x.f_find(stream, 6, 0, 1)
        if not ok:
            marks.append('tex_find_fail')
            destroy_or_release()
            return 0
        if not 0x34000 <= v2 <= 0x36003:
            marks.append('tex_bad_version')
            destroy_or_release()
            x.raise_error(0x80000004)
            return 0
        x.wr(m, 4, x.f_texstream(stream))
    x.wr(RIGHTS_ID, 4, 0)
    x.wr(RIGHTS_EXTRA, 4, 0)
    if not x.f_readchunks(REG_MAT, stream, m):
        marks.append('plugins_fail')
        destroy_or_release()
        return 0
    if x.rd(RIGHTS_ID):
        x.f_rights(REG_MAT, x.rd(RIGHTS_ID), m, x.rd(RIGHTS_EXTRA))
        marks.append('rights')
    marks.append('read_ok')
    return m


KEYS = [[] for _ in NAMES]
KEYS[CALC] = ['nv_pos', 'nv_zero', 'rsq_update', 'sqrt_called', 'sqrt_skipped']
KEYS[ADDMT] = ['native', 'normals', 'plain', 'addmt_fail', 'addmt_realloc', 'addmt_malloc']
KEYS[SETMAT] = ['null_material', 'found', 'not_found', 'appended', 'append_fail']
KEYS[LOCK] = ['mode_polys', 'mode_other']
KEYS[UNLOCK] = ['has_mesh', 'mesh_null', 'materials', 'triangles', 'optimise_ok', 'optimise_fail', 'new_texture']
KEYS[CREATE] = ['numvert_neg', 'numvert_big', 'numtri_neg', 'data', 'native', 'malloc_null', 'init_null', 'prelit', 'tris', 'addmt_failed', 'created']
KEYS[GEODESTROY] = ['just_dec', 'destroyed', 'has_rep', 'has_mesh', 'has_morph']
KEYS[GEOUNREF] = ['has_mesh', 'no_mesh', 'has_morph', 'no_morph']
KEYS[GEOREAD] = ['find_fail', 'bad_version', 'info_short', 'props', 'create_null', 'addmt', 'prelit', 'texcoords', 'tris', 'morph_verts', 'morph_verts_normals', 'morph_normals', 'matlist_find_fail',
                 'matlist_bad_version', 'matlist_fail', 'plugins_fail', 'unlock_fail', 'read_ok', 'setdef_props']
KEYS[RIGHTSREAD] = ['first_fail', 'len8', 'second_ok', 'second_fail', 'len_other']
KEYS[RIGHTSWRITE] = ['first_fail', 'both_ok', 'second_fail', 'pipe_swapped']
KEYS[SETDEF] = ['null', 'copy']
KEYS[MATCREATE] = ['alloc_null', 'created']
KEYS[MATDESTROY] = ['destroy', 'just_dec', 'has_texture']
KEYS[SETTEX] = ['new_texture', 'old_texture']
KEYS[MATREAD] = ['find_fail', 'bad_version', 'read_short', 'alloc_null', 'textured', 'tex_find_fail', 'tex_bad_version', 'plugins_fail', 'rights', 'read_ok', 'cleanup_destroy']


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
