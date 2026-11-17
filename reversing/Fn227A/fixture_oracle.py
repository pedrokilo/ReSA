"""Oraculo independiente (Python) de Fn227A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 15 funciones del lote 227A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las regiones globales
(Fixtures.hpp) para conocer su imagen inicial, repite los "pokes" del caso (los objetos escritos por el generador) y modela los colaboradores falsos del banco (LiveComparison.hpp).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh):
   0  0x749F70  RpClumpClone(clump): RpClumpCreate (0x74A290), _rwFrameCloneAndLinkClones (0x7EFFB0) del marco [clump+4]; sin marco clonado: DeInitObject, RpAtomicDestroy de los atomicos, luces con
                RpClumpRemoveLight+RpLightDestroy, camaras con 0x74A200, marco y memoria; con marco: por atomico RpAtomicClone (0x749E60), sin marco -> libera el clon, error 5 y corta con exito=1;
                _rwObjectHasFrameSetFrame(clon, marco->root), privateFlags |= 1, RpClumpAddAtomic; _rwFramePurgeClone SIEMPRE; exito: callback, CopyObject; fallo: deshace con 0x74A1C0 para las luces.
   1  0x74A1C0  desenlaza la luz (enlace en luz + [0xC924DC]), pone a cero {clump, next, prev}, RpLightDestroy; devuelve la luz.
   2  0x74A290  [RwEngineInstance+0x144](lista de clumps, 0x30010); listas vacias, tipo 2, callback 0x74A260, InitObject(0x8D6264).
   3  0x74A310  DeInitObject, atomicos (almacenamiento propio), luces, camaras (desenlazan y destruyen), marco, lista libre; devuelve 1.
   4  0x74A490  inserta el atomico al principio de la lista del clump; 5  0x74A4C0 lo desenlaza (sin poner a cero el enlace); 6  0x74A520 desenlaza la luz (sin destruirla).
   7  0x74AE50  RpGeometryDestroy de cada geometria (numero con signo) y rwfree ([engine+0x138]) del vector.
   8  0x74B060  lectura de un atomico del flujo con las listas de marcos y geometrias ya leidas.
   9  0x74B420  lectura de un clump del flujo (con todos sus caminos de error).
  10  0x74BCC0  destruye el almacenamiento de un atomico.
  11..14  0x74BDA0, 0x74BDD0, 0x74BE00, 0x74BE70  reenvian a _rwPluginRegistryAddPlugin (0x8084A0) o _rwPluginRegistryAddPluginStream (0x8088E0) con el registro 0x8D624C (atomico) o 0x8D6264 (clump).

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["RpWorld::RpClumpClone", "RpWorld::ClumpLightDestroyCallBack", "RpWorld::RpClumpCreate", "RpWorld::RpClumpDestroy", "RpWorld::RpClumpAddAtomic", "RpWorld::RpClumpRemoveAtomic",
         "RpWorld::RpClumpRemoveLight", "RpWorld::_rpGeometryListDeinitialize", "RpWorld::ClumpAtomicStreamRead", "RpWorld::RpClumpStreamRead", "RpWorld::ClumpAtomicDestroyCallBack",
         "RpWorld::RpAtomicRegisterPlugin", "RpWorld::RpClumpRegisterPlugin", "RpWorld::RpAtomicRegisterPluginStream", "RpWorld::RpClumpRegisterPluginStream"]
ENTRIES = (0x749F70, 0x74A1C0, 0x74A290, 0x74A310, 0x74A490, 0x74A4C0, 0x74A520, 0x74AE50, 0x74B060, 0x74B420, 0x74BCC0, 0x74BDA0, 0x74BDD0, 0x74BE00, 0x74BE70)
assert len(NAMES) == len(ENTRIES) == 15
(CLONE, LIGHTCB, CREATE, DESTROY, ADDATOM, REMATOM, REMLIGHT, GEODEINIT, ATOMREAD, STREAMREAD, ATOMCB, REGATOM, REGCLUMP, REGATOMS, REGCLUMPS) = range(15)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DEFAULT_ENGINE = 0x20000000
STACK_TAG = 0x7F000000
# regiones globales (direccion, bytes): 0 dato extra de derechos, 1 id de derechos y desplazamiento del modulo, 2 desplazamientos de camara y luz, 3 marca del banco
REGIONS = ((0xC92480, 4), (0xC924A8, 8), (0xC924D8, 8), (0xA9B0B0, 1))
RIGHTS_EXTRA, RIGHTS_ID, MODOFF, CAMOFF, LIGHTOFF, MARKER = 0xC92480, 0xC924A8, 0xC924AC, 0xC924D8, 0xC924DC, 0xA9B0B0
REG_ATOM, REG_CLUMP = 0x8D624C, 0x8D6264
# colas
(Q_ALLOC, Q_MALLOC, Q_FINDOK, Q_FINDLEN, Q_FINDVER, Q_READCNT, Q_READDATA, Q_RI32OK, Q_RI32VAL, Q_ATOM, Q_GEO, Q_LIGHT, Q_CAM, Q_WORLD, Q_CLUMP, Q_FRAMEOK) = range(16)
K_OFFLIGHT, K_OFFCAM, K_FRAMELISTPTR, K_FRAMELISTNUM, K_RDCHUNKS, K_RIGHTSID, K_RIGHTSEXTRA, K_CLONEFRAME, K_ERRDELTA, K_REGRET, K_ARGS, K_Q = 16, 17, 24, 25, 26, 27, 28, 29, 30, 31, 64, 128
EV_ALLOC, EV_FREEL, EV_MALLOC, EV_RWFREE = 0xE144, 0xE148, 0xE134, 0xE138


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
        a = ([x & M32 for x in c['a']] + [0] * 384)[:384]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.engine = c.get('engine') or DEFAULT_ENGINE
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
        self.reg = [Mem(a[2], (a[90] + r * 17) & M32, guard=0) for r in range(len(REGIONS))]
        self.reg[3] = Mem(a[2], 0, guard=0)
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
        self.reg[3].wr(0, 1, 0x5A)

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
    """estado de la ejecucion modelada: mundo, eventos, colas y marcas; los metodos f_* son los falsos del banco y los r_* las funciones reales que corren dentro de las probadas"""

    def __init__(self, c):
        self.c = c
        self.a = ([x & M32 for x in c['a']] + [0] * 384)[:384]
        self.W = World(c)
        self.W.snap()
        self.events = []
        self.qn = [0] * 16
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

    def freelists(self):
        base = (self.engine + self.rd(MODOFF)) & M32
        return self.rd(base), self.rd(base + 4)      # atomicos, clumps

    # --- falsos (cdecl)
    def f_freel(self, lst, p): self.ev(EV_FREEL, self.tg(lst), self.tg(p))
    def f_rwfree(self, p): self.ev(EV_RWFREE, self.tg(p))

    def f_alloc(self, lst, hint):
        self.ev(EV_ALLOC, self.tg(lst), hint)
        return self.W.res(self.q(Q_ALLOC))

    def f_malloc(self, size, hint):
        self.ev(EV_MALLOC, size, hint)
        return self.W.res(self.q(Q_MALLOC))

    def f_find(self, stream, typ, plen, pver):
        self.ev(0x7ED2D0, self.tg(stream), typ, 1 if plen else 0, 1 if pver else 0)
        ok, ln, ver = self.q(Q_FINDOK), self.q(Q_FINDLEN), self.q(Q_FINDVER)
        return ok, ln, ver

    def f_read(self, stream, n):
        self.ev(0x7EC9D0, self.tg(stream), n)
        cnt = self.q(Q_READCNT)
        nn = min(n, 16)
        data = bytearray()
        for k in range((nn + 3) // 4):
            w = self.q(Q_READDATA)
            data += (w & M32).to_bytes(4, 'little')
        return cnt, bytes(data[:nn])

    def f_readint(self, stream, n=4):
        self.ev(0x7ED540, self.tg(stream), n)
        val, ok = self.q(Q_RI32VAL), self.q(Q_RI32OK)
        return ok, val

    def f_error(self, code, arg=0):
        self.ev(0x8088D0, code, arg if code == 0x80000013 else 0)
        return (code + self.a[K_ERRDELTA]) & M32

    def f_errorset(self, code_ret):
        self.ev(0x808820, STACK_TAG, 2, code_ret)

    def raise_error(self, code, arg=0):
        self.f_errorset(self.f_error(code, arg))

    def f_init(self, reg, obj): self.ev(0x8086E0, reg, self.tg(obj))
    def f_deinit(self, reg, obj): self.ev(0x808740, reg, self.tg(obj))
    def f_copy(self, reg, dst, src): self.ev(0x808770, reg, self.tg(dst), self.tg(src))

    def f_readchunks(self, reg, stream, obj):
        self.ev(0x808980, reg, self.tg(stream), self.tg(obj))
        self.wr(RIGHTS_ID, 4, self.a[K_RIGHTSID])
        self.wr(RIGHTS_EXTRA, 4, self.a[K_RIGHTSEXTRA])
        return self.a[K_RDCHUNKS]

    def f_rights(self, reg, ident, obj, extra): self.ev(0x808AB0, reg, ident, self.tg(obj), extra)
    def f_atomic_destroy(self, at): self.ev(0x749DC0, self.tg(at))

    def f_atomic_clone(self, at):
        self.ev(0x749E60, self.tg(at))
        return self.W.res(self.q(Q_ATOM))

    def f_atomic_create(self):
        self.ev(0x749C50)
        return self.W.res(self.q(Q_ATOM))

    def f_setgeometry(self, at, g, flags): self.ev(0x749D40, self.tg(at), self.tg(g), flags)
    def f_forall(self, addr, clump, cb): self.ev(addr, self.tg(clump), cb, 0)

    def f_cloneandlink(self, frame):
        self.ev(0x7EFFB0, self.tg(frame))
        return self.W.res(self.a[K_CLONEFRAME])

    def f_purge(self, frame): self.ev(0x7F01A0, self.tg(frame))
    def f_desthier(self, frame): self.ev(0x7F08A0, self.tg(frame))
    def f_update(self, frame): self.ev(0x7F0910, self.tg(frame))

    def f_setframe(self, obj, frame):
        self.ev(0x804EF0, self.tg(obj), self.tg(frame))
        self.wr(obj + 4, 4, frame)

    def f_release(self, obj): self.ev(0x804F40, self.tg(obj))
    def f_freeres(self, entry): self.ev(0x807DE0, self.tg(entry))
    def f_geodestroy(self, g): self.ev(0x74CCC0, self.tg(g))

    def f_geostream(self, stream):
        self.ev(0x74D190, self.tg(stream))
        return self.W.res(self.q(Q_GEO))

    def f_getworld(self, at):
        self.ev(0x751050, self.tg(at))
        return self.q(Q_WORLD)

    def f_lightdestroy(self, l): self.ev(0x7520D0, self.tg(l))
    def f_cameradestroy(self, c): self.ev(0x7EE4B0, self.tg(c))

    def f_lightstream(self, stream):
        self.ev(0x751F00, self.tg(stream))
        return self.W.res(self.q(Q_LIGHT))

    def f_camstream(self, stream):
        self.ev(0x808DE0, self.tg(stream))
        return self.W.res(self.q(Q_CAM))

    def f_framelistread(self, stream):
        self.ev(0x807970, self.tg(stream), STACK_TAG)
        ok = self.q(Q_FRAMEOK)
        return ok, (self.W.res(self.a[K_FRAMELISTPTR]), self.a[K_FRAMELISTNUM])

    def f_framelistdeinit(self, fl): self.ev(0x807720, STACK_TAG, self.tg(fl[0]), fl[1])
    def f_clumpcreate(self):
        self.ev(0x74A290)
        return self.W.res(self.q(Q_CLUMP))

    def f_clumpdestroy(self, c): self.ev(0x74A310, self.tg(c))

    def f_atomicstreamread(self, stream, fl, gl):
        self.ev(0x74B060, self.tg(stream), STACK_TAG, STACK_TAG, self.tg(fl[0]), fl[1], self.tg(gl[0]), gl[1])
        return self.W.res(self.q(Q_ATOM))

    # --- funciones reales que corren dentro de otras
    def r_addatomic(self, clump, at):
        head = clump + 8
        first = self.rd(clump + 8)
        self.wr(at + 0x44, 4, head)
        self.wr(at + 0x40, 4, first)
        first = self.rd(head)
        self.wr(first + 4, 4, at + 0x40)
        self.wr(head, 4, at + 0x40)
        self.wr(at + 0x3C, 4, clump)
        return clump

    def unlink(self, entry):
        """desenlace del enlace de plugin {clump, next, prev} en `entry` (pone a cero los tres)"""
        prev = self.rd(entry + 8)
        nxt = self.rd(entry + 4)
        self.wr(prev, 4, nxt)
        nxt = self.rd(entry + 4)
        prev = self.rd(entry + 8)
        self.wr(nxt + 4, 4, prev)
        self.wr(entry + 8, 4, 0)
        self.wr(entry + 4, 4, 0)
        self.wr(entry, 4, 0)

    def r_removelight(self, clump, light):
        self.unlink((light + self.rd(LIGHTOFF)) & M32)
        return clump

    def r_lightcb(self, light):
        self.unlink((light + self.rd(LIGHTOFF)) & M32)
        self.f_lightdestroy(light)
        return light

    def r_addref(self, g):
        self.wr(g + 0xE, 2, (self.rd(g + 0xE, 2) + 1) & 0xFFFF)

    def geolist_deinit_mem(self, lst):
        n = signed(self.rd(lst + 4))
        if n > 0:
            i = 0
            while True:
                vec = self.rd(lst)
                self.f_geodestroy(self.rd(vec + 4 * i))
                n = signed(self.rd(lst + 4))
                i += 1
                if not i < n:
                    break
        vec = self.rd(lst)
        if vec:
            self.f_rwfree(vec)
            self.wr(lst, 4, 0)

    def geolist_deinit_local(self, vec, num):
        """_rpGeometryListDeinitialize sobre la lista local de 0x74B420 (vector y numero en la pila); devuelve el vector nuevo"""
        i = 0
        while i < signed(num):
            self.f_geodestroy(self.rd(vec + 4 * i))
            i += 1
        if vec:
            self.f_rwfree(vec)
            vec = 0
        return vec

    # --- trozos comunes
    def destroy_atoms_rp(self, clump):
        """bucle de RpAtomicDestroy (0x749DC0) sobre la lista de atomicos: lee el siguiente antes de destruir; corta si el atomico es nulo"""
        end = clump + 8
        link = self.rd(end)
        if link == end:
            return
        while True:
            nxt = self.rd(link)
            at = (link - 0x40) & M32
            self.f_atomic_destroy(at)
            if at == 0 or nxt == end:
                break
            link = nxt

    def destroy_storage(self, at):
        """cuerpo de 0x74BCC0: destructores de plugins, entrada de recursos, geometria, marco, lista libre"""
        self.f_deinit(REG_ATOM, at)
        r = self.rd(at + 0x14)
        if r:
            self.f_freeres(r)
        g = self.rd(at + 0x18)
        if g:
            self.f_geodestroy(g)
            frame = self.rd(at + 4)
            self.wr(at + 0x18, 4, 0)
            if frame:
                if self.f_getworld(at):
                    self.f_update(frame)
        self.f_release(at)
        self.f_freel(self.freelists()[0], at)

    def free_cams_frame_mem(self, clump):
        self.f_forall(0x749BB0, clump, 0x74A200)
        p = self.rd(clump + 4)
        if p:
            self.f_desthier(p)
        self.f_freel(self.freelists()[1], clump)


def expected(c):
    x = X(c)
    a = x.a
    fn = c['fn']
    marks = x.marks
    W = x.W
    tg = x.tg
    ret = None
    kind = None
    if fn == CLONE:
        S = x.arg(0)
        Nn = x.f_clumpcreate()
        if Nn == 0:
            ret = 0
            marks.append('create_null')
        else:
            x.wr(Nn + 4, 4, x.f_cloneandlink(x.rd(S + 4)))
            if x.rd(Nn + 4) == 0:
                marks.append('no_frame')
                x.f_deinit(REG_CLUMP, Nn)
                x.destroy_atoms_rp(Nn)
                end = Nn + 0x10
                link = x.rd(end)
                if link != end:
                    while True:
                        off = x.rd(LIGHTOFF)
                        nxt = x.rd(link)
                        light = (link - off - 4) & M32
                        owner = x.rd((link - 4) & M32)
                        x.r_removelight(owner, light)
                        x.f_lightdestroy(light)
                        if light == 0 or nxt == end:
                            break
                        link = nxt
                x.free_cams_frame_mem(Nn)
                ret = 0
            else:
                success = 1
                end = S + 8
                link = x.rd(end)
                if link != end:
                    marks.append('has_atoms')
                    while True:
                        nxt = x.rd(link)
                        at = (link - 0x40) & M32
                        cl = x.f_atomic_clone(at)
                        if cl == 0:
                            success = 0
                            marks.append('clone_atom_fail')
                            break
                        sf = x.rd(at + 4)
                        if sf == 0:
                            marks.append('atom_no_frame')
                            x.f_deinit(REG_ATOM, cl)
                            r = x.rd(cl + 0x14)
                            if r:
                                x.f_freeres(r)
                            x.f_setgeometry(cl, 0, 0)
                            x.f_release(cl)
                            x.f_freel(x.freelists()[0], cl)
                            x.raise_error(5)
                            break
                        x.f_setframe(cl, x.rd(sf + 0xA0))
                        x.wr(cl + 3, 1, x.rd(cl + 3, 1) | 1)
                        x.r_addatomic(Nn, cl)
                        marks.append('atom_cloned')
                        if nxt == end:
                            break
                        link = nxt
                x.f_purge(x.rd(S + 4))
                if success:
                    x.wr(Nn + 0x28, 4, x.rd(S + 0x28))
                    x.f_copy(REG_CLUMP, Nn, S)
                    ret = Nn
                    marks.append('clone_ok')
                else:
                    x.f_deinit(REG_CLUMP, Nn)
                    x.destroy_atoms_rp(Nn)
                    end2 = Nn + 0x10
                    link = x.rd(end2)
                    if link != end2:
                        while True:
                            off = x.rd(LIGHTOFF)
                            nxt = x.rd(link)
                            light = (link - off - 4) & M32
                            if not x.r_lightcb(light):
                                break
                            if nxt == end2:
                                break
                            link = nxt
                    x.free_cams_frame_mem(Nn)
                    ret = 0
                    marks.append('clone_failed')
        kind = 'tag'
    elif fn == LIGHTCB:
        light = x.arg(0)
        ret = x.r_lightcb(light)
        kind = 'tag'
    elif fn == CREATE:
        lst = x.freelists()[1]
        p = x.f_alloc(lst, 0x30010)
        if p == 0:
            ret = 0
            marks.append('alloc_null')
        else:
            for off, n, v in ((8, 4, p + 8), (0xC, 4, p + 8), (0x10, 4, p + 0x10), (0x14, 4, p + 0x10), (0, 1, 2), (0x18, 4, p + 0x18), (1, 1, 0), (2, 1, 0), (3, 1, 0), (4, 4, 0), (0x1C, 4, p + 0x18),
                              (0x24, 4, 0), (0x20, 4, 0), (0x28, 4, 0x74A260)):
                x.wr(p + off, n, v)
            x.f_init(REG_CLUMP, p)
            ret = p
            marks.append('created')
        kind = 'tag'
    elif fn == DESTROY:
        clump = x.arg(0)
        x.f_deinit(REG_CLUMP, clump)
        end = clump + 8
        link = x.rd(end)
        if link != end:
            while True:
                nxt = x.rd(link)
                x.destroy_storage((link - 0x40) & M32)
                if nxt == end:
                    break
                link = nxt
            marks.append('has_atoms')
        for head, offaddr, destroy, tag in ((clump + 0x10, LIGHTOFF, x.f_lightdestroy, 'has_lights'), (clump + 0x18, CAMOFF, x.f_cameradestroy, 'has_cams')):
            link = x.rd(head)
            if link != head:
                marks.append(tag)
                while True:
                    off = x.rd(offaddr)
                    nxt = x.rd(link)
                    obj = (link - off - 4) & M32
                    x.unlink((obj + off) & M32)
                    destroy(obj)
                    if obj == 0 or nxt == head:
                        break
                    link = nxt
        p = x.rd(clump + 4)
        if p:
            x.f_desthier(p)
            marks.append('has_frame')
        x.f_freel(x.freelists()[1], clump)
        ret = 1
    elif fn == ADDATOM:
        clump, at = x.arg(0), x.arg(1)
        ret = x.r_addatomic(clump, at)
        kind = 'tag'
    elif fn == REMATOM:
        clump, at = x.arg(0), x.arg(1)
        prev = x.rd(at + 0x44)
        nxt = x.rd(at + 0x40)
        x.wr(prev, 4, nxt)
        nxt = x.rd(at + 0x40)
        prev = x.rd(at + 0x44)
        x.wr(nxt + 4, 4, prev)
        x.wr(at + 0x3C, 4, 0)
        ret = clump
        kind = 'tag'
    elif fn == REMLIGHT:
        clump, light = x.arg(0), x.arg(1)
        ret = x.r_removelight(clump, light)
        kind = 'tag'
    elif fn == GEODEINIT:
        lst = x.arg(0)
        x.geolist_deinit_mem(lst)
        marks.append('n_pos' if signed(x.rd(lst + 4)) > 0 else 'n_nonpos')
        ret = lst
        kind = 'tag'
    elif fn == ATOMCB:
        at = x.arg(0)
        x.destroy_storage(at)
        ret = at
        kind = 'tag'
    elif fn in (REGATOM, REGCLUMP):
        regs = REG_ATOM if fn == REGATOM else REG_CLUMP
        args = [x.arg(i) for i in range(5)]
        x.ev(0x8084A0, regs, *[tg(v) for v in args])
        ret = a[K_REGRET]
    elif fn in (REGATOMS, REGCLUMPS):
        regs = REG_ATOM if fn == REGATOMS else REG_CLUMP
        args = [x.arg(i) for i in range(4)]
        x.ev(0x8088E0, regs, *[tg(v) for v in args])
        ret = a[K_REGRET]
    elif fn == ATOMREAD:
        ret = expected_atomread(x)
        kind = 'tag'
    elif fn == STREAMREAD:
        ret = expected_streamread(x)
        kind = 'tag'
    gch = []
    for r in range(len(REGIONS)):
        gch += W.reg[r].changes(absolute=REGIONS[r][0])
    return {'events': x.events, 'changes': {'A': W.mem[0].changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'vptrs': [], 'gchanges': gch,
            'ret': tg(ret) if kind == 'tag' else ret, 'threw': False, 'marks': marks}


def expected_atomread(x):
    a, marks = x.a, x.marks
    stream, fl, gl = x.arg(0), x.arg(1), x.arg(2)
    ok, ln, ver = x.f_find(stream, 1, 1, 1)
    if not ok:
        x.raise_error(0x8000001A)
        marks.append('find_fail')
        return 0
    if not 0x34000 <= ver <= 0x36003:
        x.raise_error(0x80000004)
        marks.append('bad_version')
        return 0
    cnt, data = x.f_read(stream, ln)
    buf = (data + bytes(16))[:16]
    if ln != cnt:
        x.raise_error(0x8000001A)
        marks.append('read_short')
        return 0
    at = x.f_atomic_create()
    if at == 0:
        marks.append('create_null')
        return 0
    frame_idx = int.from_bytes(buf[0:4], 'little')
    geom_idx = int.from_bytes(buf[4:8], 'little')
    flags = buf[8]
    x.wr(at + 2, 1, flags)
    if x.rd(fl + 4):
        frames = x.rd(fl)
        x.f_setframe(at, x.rd(frames + 4 * frame_idx))
        x.wr(at + 3, 1, x.rd(at + 3, 1) | 1)
        marks.append('has_frames')
    if x.rd(gl + 4):
        marks.append('geo_list')
        geoms = x.rd(gl)
        g = x.rd(geoms + 4 * geom_idx)
        if g != x.rd(at + 0x18):
            marks.append('geo_list_new')
            if g:
                x.r_addref(g)
            old = x.rd(at + 0x18)
            if old:
                x.f_geodestroy(old)
            x.wr(at + 0x18, 4, g)
            if g:
                morph = x.rd(g + 0x5C)
                for k in range(4):
                    x.wr(at + 0x1C + 4 * k, 4, x.rd(morph + 4 + 4 * k))
            # con geometria nula el EXE salta a 0x74B175: el marco se mira igual
            frame = x.rd(at + 4)
            if frame and x.f_getworld(at):
                x.f_update(frame)
    else:
        marks.append('geo_stream')
        ok2, _, gver = x.f_find(stream, 0xF, 0, 1)

        def fail_storage(code):
            x.destroy_storage(at)
            x.raise_error(code)
            return 0
        if not ok2:
            marks.append('geo_find_fail')
            return fail_storage(0x8000001A)
        if not 0x34000 <= gver <= 0x36003:
            marks.append('geo_bad_version')
            return fail_storage(0x80000004)
        g = x.f_geostream(stream)
        if g == 0:
            marks.append('geo_null')
            return fail_storage(0x8000001A)
        if g != x.rd(at + 0x18):
            marks.append('geo_new')
            x.r_addref(g)
            old = x.rd(at + 0x18)
            if old:
                x.f_geodestroy(old)
            x.wr(at + 0x18, 4, g)
            morph = x.rd(g + 0x5C)
            for k in range(4):
                x.wr(at + 0x1C + 4 * k, 4, x.rd(morph + 4 + 4 * k))
            frame = x.rd(at + 4)
            if frame and x.f_getworld(at):
                x.f_update(frame)
        else:
            marks.append('geo_same')
        x.f_geodestroy(g)
    x.wr(RIGHTS_ID, 4, 0)
    x.wr(RIGHTS_EXTRA, 4, 0)
    if not x.f_readchunks(REG_ATOM, stream, at):
        x.raise_error(0x8000001A)
        marks.append('chunks_fail')
        return 0
    ident = x.rd(RIGHTS_ID)
    if ident:
        x.f_rights(REG_ATOM, ident, at, x.rd(RIGHTS_EXTRA))
        marks.append('rights')
    marks.append('read_ok')
    return at


def expected_streamread(x):
    a, marks = x.a, x.marks
    stream = x.arg(0)
    ok, ln, ver = x.f_find(stream, 1, 1, 1)
    if not ok:
        x.raise_error(0x8000001A)
        marks.append('find_fail')
        return 0
    if not 0x34000 <= ver <= 0x36003:
        x.raise_error(0x80000004)
        marks.append('bad_version')
        return 0
    cnt, data = x.f_read(stream, 12)
    if cnt != 12:
        x.raise_error(0x8000001A)
        marks.append('read_short')
        return 0
    nat, nli, nca = (signed(int.from_bytes(data[4 * i:4 * i + 4], 'little')) for i in range(3))
    N = x.f_clumpcreate()
    if N == 0:
        marks.append('create_null')
        return 0

    def tail1():
        x.f_forall(0x749C00, N, 0x74A1C0)
        x.f_forall(0x749BB0, N, 0x74A200)
        p = x.rd(N + 4)
        if p:
            x.f_desthier(p)
        x.f_freel(x.freelists()[1], N)
        x.raise_error(0x8000001A)

    def abort_before_frames(tag):
        marks.append(tag)
        x.f_deinit(REG_CLUMP, N)
        x.destroy_atoms_rp(N)
        tail1()
        return 0

    def abort_with_frames(fl, tag):
        marks.append(tag)
        x.f_framelistdeinit(fl)
        x.f_deinit(REG_CLUMP, N)
        x.f_forall(0x749B70, N, 0x74BCC0)
        x.f_forall(0x749C00, N, 0x74A1C0)
        x.f_forall(0x749BB0, N, 0x74A200)
        p = x.rd(N + 4)
        if p:
            x.f_desthier(p)
        x.f_freel(x.freelists()[1], N)
        x.raise_error(0x8000001A)
        return 0

    ok, _, _ = x.f_find(stream, 0xE, 0, 1)
    if not ok:
        return abort_before_frames('frames_chunk_fail')
    fok, fl = x.f_framelistread(stream)
    if not fok:
        return abort_before_frames('framelist_fail')
    frames, nframes = fl
    x.wr(N + 4, 4, x.rd(frames))
    ok, _, _ = x.f_find(stream, 0x1A, 0, 1)
    if not ok:
        x.f_framelistdeinit(fl)
        marks.append('geomlist_chunk_fail')
        x.f_deinit(REG_CLUMP, N)
        x.destroy_atoms_rp(N)
        tail1()
        return 0
    ok, _, gver = x.f_find(stream, 1, 1, 1)
    if not ok:
        return abort_with_frames(fl, 'geomlist_struct_fail')
    if not 0x34000 <= gver <= 0x36003:
        x.raise_error(0x80000004)
        return abort_with_frames(fl, 'geomlist_bad_version')
    cnt2, d2 = x.f_read(stream, 4)
    if cnt2 != 4:
        return abort_with_frames(fl, 'geomlist_count_short')
    count = signed(int.from_bytes(d2, 'little'))
    vec, num = 0, 0
    if count > 0:
        vec = x.f_malloc((count * 4) & M32, 0x3000F)
        if vec == 0:
            x.raise_error(0x80000013, (count * 4) & M32)
            return abort_with_frames(fl, 'malloc_null')
        marks.append('geoms')
    for i in range(count if count > 0 else 0):
        ok, _, gv = x.f_find(stream, 0xF, 0, 1)
        if not ok:
            vec = x.geolist_deinit_local(vec, num)
            return abort_with_frames(fl, 'geom_chunk_fail')
        if not 0x34000 <= gv <= 0x36003:
            vec = x.geolist_deinit_local(vec, num)
            return abort_with_frames(fl, 'geom_bad_version')
        g = x.f_geostream(stream)
        x.wr(vec + 4 * i, 4, g)
        if g == 0:
            vec = x.geolist_deinit_local(vec, num)
            return abort_with_frames(fl, 'geom_null')
        num += 1

    def abort_with_geoms(tag):
        nonlocal vec
        vec = x.geolist_deinit_local(vec, num)
        return abort_with_frames(fl, tag)
    for i in range(nat if nat > 0 else 0):
        ok, _, _ = x.f_find(stream, 0x14, 0, 1)
        if not ok:
            return abort_with_geoms('atom_chunk_fail')
        at = x.f_atomicstreamread(stream, (frames, nframes), (vec, num))
        if at == 0:
            return abort_with_geoms('atom_null')
        x.r_addatomic(N, at)
        marks.append('atom_added')
    for i in range(nli if nli > 0 else 0):
        ok, _, _ = x.f_find(stream, 1, 0, 0)
        if not ok:
            return abort_with_geoms('light_struct_fail')
        ok, idx = x.f_readint(stream, 4)
        if not ok:
            return abort_with_geoms('light_index_fail')
        ok, _, _ = x.f_find(stream, 0x12, 0, 0)
        if not ok:
            return abort_with_geoms('light_chunk_fail')
        light = x.f_lightstream(stream)
        if light == 0:
            return abort_with_geoms('light_null')
        x.f_setframe(light, x.rd(frames + 4 * idx))
        off = x.rd(LIGHTOFF)
        entry = (light + off) & M32
        head = N + 0x10
        first = x.rd(head)
        x.wr(entry + 8, 4, head)
        x.wr(entry + 4, 4, first)
        first = x.rd(head)
        x.wr(first + 4, 4, entry + 4)
        x.wr(head, 4, entry + 4)
        x.wr(entry, 4, N)
        marks.append('light_added')
    for i in range(nca if nca > 0 else 0):
        ok, _, _ = x.f_find(stream, 1, 0, 0)
        if not ok:
            return abort_with_geoms('cam_struct_fail')
        ok, idx = x.f_readint(stream, 4)
        if not ok:
            return abort_with_geoms('cam_index_fail')
        ok, _, _ = x.f_find(stream, 5, 0, 0)
        if not ok:
            return abort_with_geoms('cam_chunk_fail')
        cam = x.f_camstream(stream)
        if cam == 0:
            return abort_with_geoms('cam_null')
        x.f_setframe(cam, x.rd(frames + 4 * idx))
        off = x.rd(CAMOFF)
        entry = (cam + off) & M32
        head = N + 0x18
        first = x.rd(head)
        x.wr(entry + 8, 4, head)
        x.wr(entry + 4, 4, first)
        first = x.rd(head)
        x.wr(first + 4, 4, entry + 4)
        x.wr(head, 4, entry + 4)
        x.wr(entry, 4, N)
        marks.append('cam_added')
    vec = x.geolist_deinit_local(vec, num)
    x.f_framelistdeinit(fl)
    if not x.f_readchunks(REG_CLUMP, stream, N):
        x.f_clumpdestroy(N)
        x.raise_error(0x8000001A)
        marks.append('plugin_chunks_fail')
        return 0
    marks.append('read_ok')
    return N


KEYS = [[] for _ in NAMES]
KEYS[CLONE] = ['create_null', 'no_frame', 'has_atoms', 'clone_atom_fail', 'atom_no_frame', 'atom_cloned', 'clone_ok', 'clone_failed']
KEYS[CREATE] = ['alloc_null', 'created']
KEYS[DESTROY] = ['has_atoms', 'has_lights', 'has_cams', 'has_frame']
KEYS[GEODEINIT] = ['n_pos', 'n_nonpos']
KEYS[ATOMREAD] = ['find_fail', 'bad_version', 'read_short', 'create_null', 'has_frames', 'geo_list', 'geo_list_new', 'geo_stream', 'geo_find_fail', 'geo_bad_version', 'geo_null', 'geo_new', 'geo_same',
                  'chunks_fail', 'rights', 'read_ok']
KEYS[STREAMREAD] = ['find_fail', 'bad_version', 'read_short', 'create_null', 'frames_chunk_fail', 'framelist_fail', 'geomlist_chunk_fail', 'geomlist_struct_fail', 'geomlist_bad_version',
                    'geomlist_count_short', 'malloc_null', 'geoms', 'geom_chunk_fail', 'geom_bad_version', 'geom_null', 'atom_chunk_fail', 'atom_null', 'atom_added', 'light_struct_fail',
                    'light_index_fail', 'light_chunk_fail', 'light_null', 'light_added', 'cam_struct_fail', 'cam_index_fail', 'cam_chunk_fail', 'cam_null', 'cam_added', 'plugin_chunks_fail', 'read_ok']


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
