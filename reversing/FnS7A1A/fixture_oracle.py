"""Oraculo independiente (Python) de FnS7A1A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 21 funciones de los lotes S7A y S1A del plan de
sustitucion y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas
(Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, capstone). Las lecturas y escrituras se modelan EN EL ORDEN del EXE (importa cuando un falso cambia la memoria entre dos lecturas):
   0  0x749000  [0xC92474] += 1; devuelve obj (arg 1)
   1  0x749010  [0xC92474] -= 1; devuelve obj
   2  0x749020  byte [obj + [0xC92478]] = 1; devuelve obj
   3  0x749030  devuelve obj
   4  0x749040  dl = byte [src + [0xC92478]]; byte [dst + [0xC92478]] = dl; devuelve dst (el desplazamiento se relee para cada acceso)
   5  0x749060  longitud != 4 o obj == 0 -> 0; 0x7ED540(flujo, &v, 4) (v = la propia longitud, 4, si el falso no escribe) = 0 -> 0; byte [obj + [0xC92478] (releido tras la llamada)] = byte bajo de v; devuelve el flujo
   6  0x7490B0  obj == 0 o [obj] == 0 -> 0; v = byte [obj + [0xC92478]] (antes de la llamada); 0x7ED460(flujo, &v, 4) -> flujo si no es nulo, si no 0
   7  0x7490F0  obj == 0 o [obj] == 0 -> 0; byte [obj + [0xC92478]] != 1 -> 4; si no 0
   8  0x4637C0  primera de 75 entradas de 8 bytes (desde this) con el byte de tipo en 0 -> su direccion; ninguna -> 0
   9  0x4CF9E0  copia 4 palabras de 32 bits de origen a this
  10  0x4D1390  no hace nada
  11  0x4E7B80  R = refCon (arg 2): si pos [R+0x4C] >= longitud [R+0x50] (con signo): byte [R+0x58] = 0 y [R+0x14], [R+0x10], [R+0x24], [R+0x30] = 0; byte [R+0x58] == 0 -> *arg1 = R, AL = 0; si no
             0x81A3C0([R+0x48], [R+0x44], [R+0x54], &salida (R), pos, &C, &A, 0, 0, 0, &B, 0): con el OSErr (palabra baja de EAX) != 0 o salida == 0 hace el mismo borrado y SIGUE; [R+0x10] = A*B, [R+0x4C] = C + A*B,
             [R+0x24] = salida, byte [R+0x59] != 0 (releido) -> [R+0x30] = salida / B (idiv con signo); *arg1 = R; AL = byte [R+0x58]
  12  0x61DAE0  [t+0x1C] == anim -> 0; si no [t+0x20] == anim -> 0; palabra [anim+0x2C] == 0x2F -> byte [t+8] = 1; [t+0x20] == 0 y byte [t+0x27] en (0x10, 0x11, 0xF, 1) -> byte [t+8] = 1
  13  0x61FD10  [t+0xC] = 0; byte [t+8] = 1
  14  0x61FF30  [t+0x14] = 0; byte [t+8] = 1
  15  0x621BE0  [t+0x2C] == anim -> 0
  16  0x675DD0  id = movsx palabra [anim+0x2C]; [t+0x18] = id; id == 0x158 -> [anim+0x1C] = -1000.0f (0xC47A0000), byte [t+0x29] = 1; byte [t+0x2A] != 0 y [t+0x18] == 0x157 -> lo mismo; [t+0x14] = 0
  17  0x6D2950  byte [material + 7] = byte bajo de data; devuelve material
  18  0x734850  geom = [atomo + 0x18]; p = [geom + [0xC3A1E0]]; p != 0 y [p] > 0 (sin signo) -> *data = atomo, 0; si no atomo
  19  0x735140  byte [0xC88658] != 0 -> nada; si no = 1; ForAllAtomics(clump, 0x734810, &atomo); GetSkin([atomo+0x18]); ForAllAtomics(clump, 0x734A20, &jerarquia); tabla[0] = {-1, 0, 0, 0}; NumBones(piel); por hueso i = 1..n-1:
             SkinToBone(piel); copia matriz i; RwMatrixInvert(&inv, &copia); SkinToBone(piel); copia matriz [padre * 64]; RwV3dTransformPoints(&tabla[i].pos, &inv.pos, 1, &copia padre); tabla[i].padre = padre;
             banderas = [[jerarquia+0x10] + 16 i + 8]; PUSH (2): apila el padre; POP (1): padre = desapilado; si no padre = i
  20  0x7352D0  ForAllAtomics(clump, 0x734810, &atomo); GetSkin([atomo+0x18]); ForAllAtomics(clump, 0x734A20, &jerarquia); MatArray(jerarquia); NumBones(piel); por hueso i = 1..n-1:
             RwV3dTransformPoints(&matriz[i].pos, &tabla[i].pos, 1, &matriz[tabla[i].padre * 64])

Los falsos anotan [id, argumentos...] (ver LiveComparison.hpp); un puntero a la pila se anota como 0x7F000000. Tras cada evento puede aplicarse el efecto lateral ("poke") del caso.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
import zlib

M32 = 0xFFFFFFFF
NAMES = ["RpAnisot::EngineConstructor", "RpAnisot::EngineDestructor", "RpAnisot::TextureConstructor", "RpAnisot::TextureDestructor", "RpAnisot::TextureCopy", "RpAnisot::StreamRead",
         "RpAnisot::StreamWrite", "RpAnisot::StreamGetSize", "CMissionCleanup::FindFree", "CQuaternion::Copy", "CDefaultAnimCallback::DefaultAnimCB", "CAEMFDecoder::FillCallback",
         "CTaskSimpleFight::MeleeAnimFinishCB", "CTaskSimpleBeKickedOnGround::FinishAnimCB", "CTaskSimpleBeHitWhileMoving::FinishAnimCB", "CTaskSimpleGangDriveBy::FinishAnimCB",
         "CTaskInteriorSitInChair::FinishAnimCB", "Vehicle::SetCompAlphaCB", "RwHelper::Get2DEffectAtomicCallback", "RwHelper::SkinGetBonePositions", "RwHelper::SkinSetBonePositions"]
ENTRIES = (0x749000, 0x749010, 0x749020, 0x749030, 0x749040, 0x749060, 0x7490B0, 0x7490F0, 0x4637C0, 0x4CF9E0, 0x4D1390, 0x4E7B80, 0x61DAE0, 0x61FD10, 0x61FF30, 0x621BE0, 0x675DD0, 0x6D2950, 0x734850,
           0x735140, 0x7352D0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(ENGINE, TEXCTOR, TEXDTOR, TEXCOPY, STREAD, STWRITE, STSIZE, FINDFREE, QUAT, NOP, FILL, FIGHT, KICK, HIT, GANG, SIT, ALPHA, DFX, SKINGET, SKINSET) = range(20)
KIND = [ENGINE, ENGINE, TEXCTOR, TEXDTOR, TEXCOPY, STREAD, STWRITE, STSIZE, FINDFREE, QUAT, NOP, FILL, FIGHT, KICK, HIT, GANG, SIT, ALPHA, DFX, SKINGET, SKINSET]
GLOBALS = (0xC92474, 0xC92478, 0xC3A1E0, 0xC88258)
GSIZE = (4, 4, 4, 0x401)
G_OF = {0: (0,), 1: (0,), 2: (1,), 4: (1,), 5: (1,), 6: (1,), 7: (1,), 18: (2,), 19: (3,), 20: (3,)}
STACK = 0x7F000000
RET_EAX = {0, 1, 2, 3, 4, 5, 6, 7, 8, 17, 18}
RET_AL = {11}


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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def crc(b):
    return zlib.crc32(bytes(b)) & M32


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


def fill_words(seed, nbytes):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytes(out[:nbytes])


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y las regiones globales vigiladas"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.R = {}
        self.Rref = {}

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
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def rget(self, g, off=0, n=4):
        return int.from_bytes(self.R[g][off:off + n], 'little')

    def rput(self, g, off, v, n=4):
        self.R[g][off:off + n] = (v & ((1 << (8 * n)) - 1)).to_bytes(n, 'little')

    def snap(self):
        for m in self.mem:
            m.snap()
        self.Rref = {g: bytes(b) for g, b in self.R.items()}

    def rdbytes(self, addr, n):
        return bytes(self.rd(addr + k, 1) for k in range(n))


KEYS = {}
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'obj_aligned', 'obj_unaligned']
_EXTRA = {
    ENGINE: ['count_zero', 'count_max', 'count_other'],
    TEXCTOR: ['target_in_window', 'target_in_guard'],
    TEXDTOR: [],
    TEXCOPY: ['same_object', 'cross_arena', 'byte_differs'],
    STREAD: ['len4', 'len_other', 'obj_null', 'read_fail', 'read_ok_write', 'read_ok_nowrite', 'poke_applied'],
    STWRITE: ['obj_null', 'first_zero', 'write_fail', 'write_ok'],
    STSIZE: ['obj_null', 'first_zero', 'byte_is_1', 'size4', 'size0'],
    FINDFREE: ['first0', 'mid', 'last', 'none'],
    QUAT: ['src_B', 'src_C', 'src_A', 'same'],
    NOP: [],
    FILL: ['clear_first', 'no_data', 'call_ok', 'call_err', 'call_out0', 'variable', 'poke_applied'],
    FIGHT: ['hit_1c', 'hit_20', 'neither', 'id2f', 'idle_null', 'cmd_hit', 'finished_set'],
    KICK: ['anim_null', 'anim_set'],
    HIT: ['anim_null', 'anim_set'],
    GANG: ['equal', 'different', 'null_anim'],
    SIT: ['id158', 'id157', 'other_id', 'flag2a', 'blend_set'],
    ALPHA: ['alpha_zero', 'alpha_ff'],
    DFX: ['plugin_null', 'count_zero', 'count_pos', 'found'],
    SKINGET: ['flag_set', 'n_le_1', 'n_many', 'push', 'pop', 'n64'],
    SKINSET: ['n_le_1', 'n_many', 'n64'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


class Ctx:
    """estado de un caso: mundo, eventos, marcas y efecto lateral"""

    def __init__(self, c):
        self.c = c
        self.a = [x & M32 for x in c['a']]
        self.W = World(c)
        self.events = []
        self.marks = []
        self.pcount = 0
        self.ret = None

    def ev(self, eid, args):
        self.events.append([eid] + [x & M32 for x in args])
        a = self.a
        if a[40] and eid == a[40]:
            n = self.pcount
            self.pcount += 1
            if n == a[41]:
                val = a[46]
                if a[42] == 0:
                    self.W.rput(a[43], a[44], val)
                else:
                    self.W.mem[a[43]].wr(a[44], 4, val)
                self.marks.append('poke_applied')

    def tag(self, v):
        return self.W.tagv(v)


def put_node_flags(mem, base, seed, n):
    depth = 0
    for i in range(1, n):
        fl = rnd(seed, i) & 3
        if (fl & 2) and depth >= 24:
            fl &= ~2
        if fl & 2:
            depth += 1
        if (fl & 1) and depth == 0:
            fl &= ~1
        elif fl & 1:
            depth -= 1
        mem.wr(base + 16 * i + 8, 4, fl)


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
    marks += ['kind%d' % a[1], 'fill%d' % a[2], 'obj_unaligned' if a[0] % 4 else 'obj_aligned']
    for g in G_OF.get(fn, ()):
        W.R[g] = bytearray(GSIZE[g])
    # --- preparacion (regiones globales; las arenas se preparan en cada rama, con las mismas escrituras que Setup en LiveComparison.hpp)
    for g in G_OF.get(fn, ()):
        if g == 3:
            W.R[3][:0x400] = fill_words(a[62], 0x400)
            W.R[3][0x400] = a[51] & 0xFF
        else:
            W.rput(g, 0, a[48 + g])
    big = None
    J0, J1 = 0xAAAA5555, 0x3333
    t_in = lambda t: 'target_in_window' if 0 <= t < WIN else 'target_in_guard'

    if k == ENGINE:
        delta = 1 if fn == 0 else -1
        v = a[48]
        marks.append('count_zero' if v == 0 else ('count_max' if v == M32 else 'count_other'))
        W.snap()
        W.rput(0, 0, (v + delta) & M32)
        X.ret = a[4]
    elif k == TEXCTOR:
        W.snap()
        off = W.rget(1)
        t = s32(a[49] + o) if False else ((a[49] + o) & M32)
        t = s32(t)
        marks.append(t_in(t))
        W.wr((this + off) & M32, 1, 1)
        X.ret = this
    elif k == TEXDTOR:
        W.snap()
        X.ret = a[4]
    elif k == TEXCOPY:
        src = this if a[10] else (W.raw(1, 0) + a[9]) & M32
        marks.append('same_object' if a[10] else 'cross_arena')
        W.snap()
        off = W.rget(1)
        byte = W.rd((src + off) & M32, 1)
        dst_byte = W.rd((this + off) & M32, 1)
        if byte != dst_byte:
            marks.append('byte_differs')
        W.wr((this + off) & M32, 1, byte)
        X.ret = this
    elif k == STREAD:
        stream, ln = a[4], a[5]
        obj = this if a[6] else 0
        W.snap()
        if ln != 4:
            marks.append('len_other')
            X.ret = 0
        elif obj == 0:
            marks.append('len4')
            marks.append('obj_null')
            X.ret = 0
        else:
            marks.append('len4')
            value = a[10] if a[9] else 4
            X.ev(0x7ED540, [X.tag(stream), STACK, 4])
            if a[8] == 0:
                marks.append('read_fail')
                X.ret = 0
            else:
                marks.append('read_ok_write' if a[9] else 'read_ok_nowrite')
                off = W.rget(1)
                W.wr((obj + off) & M32, 1, value & 0xFF)
                X.ret = stream
    elif k == STWRITE:
        stream = a[4]
        obj = this if a[6] else 0
        A.wr(o, 4, a[7])
        W.snap()
        if obj == 0:
            marks.append('obj_null')
            X.ret = 0
        elif A.rd(o, 4) == 0:
            marks.append('first_zero')
            X.ret = 0
        else:
            off = W.rget(1)
            byte = W.rd((obj + off) & M32, 1)
            X.ev(0x7ED460, [X.tag(stream), STACK, 4, byte])
            if a[9] == 0:
                marks.append('write_fail')
                X.ret = 0
            else:
                marks.append('write_ok')
                X.ret = stream
    elif k == STSIZE:
        obj = this if a[6] else 0
        A.wr(o, 4, a[7])
        if 1 <= a[9] <= 5:
            A.wr(a[8], 1, (a[10] & 0xFF) if a[9] == 5 else (0, 1, 0, 2, 0xFF, 0)[a[9]])
        W.snap()
        if obj == 0:
            marks.append('obj_null')
            X.ret = 0
        elif A.rd(o, 4) == 0:
            marks.append('first_zero')
            X.ret = 0
        else:
            off = W.rget(1)
            byte = W.rd((obj + off) & M32, 1)
            if byte == 1:
                marks.append('byte_is_1')
            X.ret = 4 if byte != 1 else 0
            marks.append('size4' if X.ret else 'size0')
    elif k == FINDFREE:
        for i in range(75):
            r = rnd(a[5], i)
            t = (r % 255) + 1
            if i == a[4]:
                t = 0
            elif i > a[4] and ((r >> 16) % 3) == 0:
                t = 0
            A.wr(o + 8 * i, 1, t)
        W.snap()
        X.ret = 0
        for i in range(75):
            if A.rd(o + 8 * i, 1) == 0:
                X.ret = (this + 8 * i) & M32
                marks.append('first0' if i == 0 else ('last' if i == 74 else 'mid'))
                break
        else:
            marks.append('none')
    elif k == QUAT:
        arena = (1, 2, 0, 0)[a[4]]
        src = W.raw(arena, a[5]) if a[4] != 3 else this
        marks.append(('src_B', 'src_C', 'src_A', 'same')[a[4]])
        if a[4] != 3:
            for j in range(4):
                W.wr((src + 4 * j) & M32, 4, a[6 + j])
        W.snap()
        words = [W.rd((src + 4 * j) & M32, 4) for j in range(4)]
        for j in range(4):
            A.wr(o + 4 * j, 4, words[j])
    elif k == NOP:
        W.snap()
    elif k == FILL:
        R = this
        A.wr(o + 0x4C, 4, a[5]); A.wr(o + 0x50, 4, a[6]); A.wr(o + 0x44, 4, a[7]); A.wr(o + 0x48, 4, a[8]); A.wr(o + 0x54, 4, a[9])
        A.wr(o + 0x58, 1, a[10]); A.wr(o + 0x59, 1, a[11])
        W.snap()
        outp = W.raw(1, a[4])

        def clear():
            A.wr(o + 0x58, 1, 0)
            for off in (0x14, 0x10, 0x24, 0x30):
                A.wr(o + off, 4, 0)
        pos = A.rd(o + 0x4C, 4)
        if s32(pos) >= s32(A.rd(o + 0x50, 4)):
            marks.append('clear_first')
            clear()
        if A.rd(o + 0x58, 1) == 0:
            marks.append('no_data')
            W.wr(outp, 4, R)
            X.ret = 0
        else:
            hnd, med, siz = A.rd(o + 0x44, 4), A.rd(o + 0x48, 4), A.rd(o + 0x54, 4)
            X.ev(0x81A3C0, [med, hnd, siz, STACK, pos, STACK, STACK, 0, 0, 0, STACK, 0])
            out = R
            if a[14]:
                out = a[13]
            ca, cb, cc = a[15], a[16], a[17]
            err = a[12] & 0xFFFF
            if err != 0 or out == 0:
                marks.append('call_err' if err != 0 else 'call_out0')
                clear()
            else:
                marks.append('call_ok')
            prod = (ca * cb) & M32
            A.wr(o + 0x10, 4, prod)
            A.wr(o + 0x4C, 4, (cc + prod) & M32)
            A.wr(o + 0x24, 4, out)
            if A.rd(o + 0x59, 1) != 0:
                marks.append('variable')
                num, den = s32(out), s32(cb)
                q = abs(num) // abs(den)
                if (num < 0) != (den < 0):
                    q = -q
                A.wr(o + 0x30, 4, q & M32)
            W.wr(outp, 4, R)
            X.ret = A.rd(o + 0x58, 1)
    elif k == FIGHT:
        anim = W.raw(1, a[4])
        B.wr(a[4] + 0x2C, 2, a[5] & 0xFFFF)
        w1c, w20 = a[7], a[8]
        if a[6] == 0:
            w1c = anim
        elif a[6] == 1:
            w20 = anim
        elif a[6] == 3:
            w1c = w20 = anim
        A.wr(o + 0x1C, 4, w1c); A.wr(o + 0x20, 4, w20); A.wr(o + 0x27, 1, a[9]); A.wr(o + 0x8, 1, a[10])
        W.snap()
        if A.rd(o + 0x1C, 4) == anim:
            marks.append('hit_1c')
            A.wr(o + 0x1C, 4, 0)
        elif A.rd(o + 0x20, 4) == anim:
            marks.append('hit_20')
            A.wr(o + 0x20, 4, 0)
        else:
            marks.append('neither')
        if B.rd(a[4] + 0x2C, 2) == 0x2F:
            marks.append('id2f')
            A.wr(o + 8, 1, 1)
        if A.rd(o + 0x20, 4) == 0:
            marks.append('idle_null')
            if A.rd(o + 0x27, 1) in (0x10, 0x11, 0xF, 1):
                marks.append('cmd_hit')
                A.wr(o + 8, 1, 1)
        if A.rd(o + 8, 1) == 1:
            marks.append('finished_set')
    elif k in (KICK, HIT):
        off = 0xC if k == KICK else 0x14
        A.wr(o + off, 4, a[4]); A.wr(o + 8, 1, a[5])
        W.snap()
        marks.append('anim_null' if a[4] == 0 else 'anim_set')
        A.wr(o + off, 4, 0)
        A.wr(o + 8, 1, 1)
    elif k == GANG:
        anim = 0 if a[7] else W.raw(1, a[6])
        A.wr(o + 0x2C, 4, anim if a[5] else a[4])
        W.snap()
        marks.append('null_anim' if a[7] else ('equal' if A.rd(o + 0x2C, 4) == anim else 'different'))
        if A.rd(o + 0x2C, 4) == anim:
            A.wr(o + 0x2C, 4, 0)
    elif k == SIT:
        B.wr(a[4] + 0x2C, 2, a[5] & 0xFFFF); B.wr(a[4] + 0x1C, 4, a[6])
        A.wr(o + 0x2A, 1, a[7]); A.wr(o + 0x29, 1, a[8]); A.wr(o + 0x18, 4, a[9]); A.wr(o + 0x14, 4, a[10])
        W.snap()
        ident = s16(B.rd(a[4] + 0x2C, 2))
        A.wr(o + 0x18, 4, ident & M32)
        marks.append('id158' if ident == 0x158 else ('id157' if ident == 0x157 else 'other_id'))
        if ident == 0x158:
            B.wr(a[4] + 0x1C, 4, 0xC47A0000)
            A.wr(o + 0x29, 1, 1)
            marks.append('blend_set')
        if A.rd(o + 0x2A, 1) != 0:
            marks.append('flag2a')
            if A.rd(o + 0x18, 4) == 0x157:
                B.wr(a[4] + 0x1C, 4, 0xC47A0000)
                A.wr(o + 0x29, 1, 1)
        A.wr(o + 0x14, 4, 0)
    elif k == ALPHA:
        W.snap()
        marks.append('alpha_zero' if a[4] & 0xFF == 0 else ('alpha_ff' if a[4] & 0xFF == 0xFF else 'alpha_other'))
        A.wr(o + 7, 1, a[4] & 0xFF)
        X.ret = this
    elif k == DFX:
        A.wr(o + 0x18, 4, W.raw(1, a[4]))
        B.wr(a[5], 4, W.raw(2, a[6]) if a[6] else 0)
        if a[6]:
            Cc.wr(a[6], 4, a[7])
        W.snap()
        geom = A.rd(o + 0x18, 4)
        off = W.rget(2)
        p = W.rd((geom + off) & M32, 4)
        X.ret = this
        if p == 0:
            marks.append('plugin_null')
        elif W.rd(p, 4) == 0:
            marks.append('count_zero')
        else:
            marks.append('count_pos')
            marks.append('found')
            W.wr(W.raw(3, a[8]), 4, this)
            X.ret = 0
    elif k in (SKINGET, SKINSET):
        n = s32(a[4])
        md = a[10]
        B.wr(a[7] + 0x18, 4, a[6])
        B.wr(a[8] + 0x10, 4, W.raw(2, a[9]))
        put_node_flags(Cc, a[9], a[14], n)
        if k == SKINSET:
            for i in range(1, n):
                W.rput(3, 16 * i, rnd(a[14], 1000 + i) % n)
        W.snap()
        xors = (a[11], a[12], a[13])
        atom, hier = W.raw(1, a[7]), W.raw(1, a[8])
        mats = W.raw(3, md)
        tag_this = X.tag(this)
        if k == SKINGET and W.R[3][0x400] != 0:
            marks.append('flag_set')
        else:
            if k == SKINGET:
                W.R[3][0x400] = 1
            X.ev(0x749B70, [tag_this, 0x734810, STACK])
            X.ev(0x7C7550, [a[6]])
            X.ev(0x749B70, [tag_this, 0x734A20, STACK])
            if k == SKINSET:
                X.ev(0x7C5120, [X.tag(hier)])
            else:
                W.rput(3, 0, M32); W.rput(3, 4, 0); W.rput(3, 8, 0); W.rput(3, 12, 0)
            X.ev(0x7C77E0, [a[5]])
            if n <= 1:
                marks.append('n_le_1')
            else:
                marks.append('n_many')
                if n == 64:
                    marks.append('n64')
            parent = 0
            stack = [0]
            for i in range(1, n):
                if k == SKINGET:
                    X.ev(0x7C7810, [a[5]])
                    src = W.rdbytes((mats + 64 * i) & M32, 64)
                    X.ev(0x7F2070, [STACK, STACK, crc(src)])
                    words = struct.unpack('<16I', src)
                    inv = [(words[j] ^ ((j * 0x01010101 + xors[0]) & M32)) & M32 for j in range(16)]
                    X.ev(0x7C7810, [a[5]])
                    pm = W.rdbytes((mats + ((parent << 6) & M32)) & M32, 64)
                    invpos = struct.pack('<3I', inv[12], inv[13], inv[14])
                    X.ev(0x7EDD90, [0xC8825C + 16 * i, STACK, 1, STACK, crc(invpos), crc(pm)])
                    mcrc = crc(pm)
                    for j in range(3):
                        W.rput(3, 16 * i + 4 + 4 * j, (inv[12 + j] ^ xors[j] ^ mcrc) & M32)
                    W.rput(3, 16 * i, parent & M32)
                    flags = Cc.rd(a[9] + 16 * i + 8, 4)
                    if flags & 2:
                        stack.append(parent)
                        marks.append('push')
                    if flags & 1:
                        parent = stack.pop()
                        marks.append('pop')
                    else:
                        parent = i
                else:
                    parent_i = W.rget(3, 16 * i)
                    pm_addr = (mats + ((parent_i << 6) & M32)) & M32
                    pm = W.rdbytes(pm_addr, 64)
                    srcpos = bytes(W.R[3][16 * i + 4:16 * i + 16])
                    dst = (mats + 64 * i + 0x30) & M32
                    X.ev(0x7EDD90, [X.tag(dst), 0xC8825C + 16 * i - 0 , 1, X.tag(pm_addr), crc(srcpos), crc(pm)])
                    v = struct.unpack('<3I', srcpos)
                    mcrc = crc(pm)
                    for j in range(3):
                        W.wr((dst + 4 * j) & M32, 4, (v[j] ^ xors[j] ^ mcrc) & M32)
    gch = []
    for g in G_OF.get(fn, ()):
        if GSIZE[g] > 0x40:
            big = crc(W.R[g])
            continue
        before, after = W.Rref[g], W.R[g]
        for kb in range(GSIZE[g]):
            if before[kb] != after[kb]:
                gch.append([GLOBALS[g] + kb, after[kb]])
    ret = None
    if fn in RET_EAX:
        ret = X.ret & M32
    elif fn in RET_AL:
        ret = X.ret & 0xFF
    return {'events': X.events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'big': big, 'threw': False, 'marks': marks}


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
    if o.get('big') != e['big']:
        return 'big %s != %s' % (o.get('big'), e['big'])
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
