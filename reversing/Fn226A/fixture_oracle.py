"""Oraculo independiente (Python) de Fn226A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 17 funciones del lote 226A (nucleo de RpAtomic y
RpClump de RenderWare, RpAnisotPluginAttach y CGamma::SetGammaNoFade) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo
reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial, y la aritmetica x87 de 24 bits (cada operacion redondeada a 24 bits de mantisa con exponente extendido;
los almacenamientos a memoria redondean a float).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`). Las lecturas y escrituras se modelan EN EL ORDEN del EXE (importa cuando un falso cambia la memoria entre dos lecturas):
   0  0x747DB0  level = fl(arg * 0.7f) (guardado como float); si [0xC92134] == 0 fin; [0xC92140] = level; si level == [0xC9213C] (orden: con NaN sigue) fin; [0xC9213C] = level; 0x7F9D50() (dispositivo; 0 = fin);
             power = fl(fl(1.0 - level) + 0.2f); si byte [0xC920C8] == 0: lo pone a 1 y, con 0x7F9D50() otra vez, GetGammaRamp(dev2, 0, 0xC8D4C8) (vtable +0x58); para i en 0..255: x = (i+1) * 0.00390625f (double),
             p = pow(x, power) (CRT del EXE: con x == 1 y power NaN o infinito da NaN), v = trunc(r24(p * 65535.0)) & 0xFFFF (0 si NaN, infinito o fuera de int64) en 0xC8DAC8/0xC8DCC8/0xC8DEC8 + 2 i;
             SetGammaRamp(dev1, 0, 1, 0xC8DAC8) (vtable +0x54)
   1  0x748F70  0x7F2BB0(0, 0x127, 0x749000, 0x749010) < 0 -> 0; 0x7F3BB0(1, 0x127, 0x749020, 0x749030, 0x749040) -> [0xC92478], < 0 -> 0; 0x804550(0x127, 0x749060, 0x7490B0, 0x7490F0) -> [0xC9247C], < 0 -> 0;
             0x7FDE60([0xC92478] releido); 1
   2  0x749120  0x7ED540(flujo, 0xC924A8, 4) = 0 -> 0; si longitud == 8: 0x7ED540(flujo, 0xC92480, 4) = 0 -> 0; devuelve el flujo
   3  0x749160  p = [obj+0x6C]; 0x7ED460(flujo, p+0x2C, 4) = 0 -> 0; p = [obj+0x6C] (releido); 0x7ED460(flujo, p+0x30, 4); devuelve (resultado != 0) ? flujo : 0
   4  0x7491F0  si geometria [+0x18] == 0 nada; indices int16 [+0x50] (inicio) y [+0x52] (fin): distintos (16 bits) y ambos < numMorphTargets ([geom+0x18], con signo): esfera interpolada con la x87 de 24 bits:
             s = posicion * recipTime ([+0x5C] * [+0x58]); radio = (r_fin - r_ini) * s + r_ini; centro: d = float(fin - ini), x' = d * s (el eje x queda en el registro, y y z se guardan como float), centro = ini + x';
             si no, y ambos < num: esfera del target de inicio; si no, la del target 0 ([[geom+0x18+0x44] + 4], 4 dwords); [+0x4C] &= ~2; [+3] |= 1
   5  0x749330  al = byte [+0x4C], marco = [+4] (antes de la llamada); al & 2 -> 0x7491F0(atomo); 0x7F0340(marco) == 0 y no ([+3] & 1) -> devuelve +0x2C sin mas; 0x7F0990(marco) = matriz;
             0x7EDD60(+0x2C, +0x1C, matriz); si ([matriz+0xC] & 3) == 3: [+0x38] = [+0x28], [+3] &= 0xFE; si no: longitudes al cuadrado ((x*x + y*y) + z*z, 24 bits) de derecha (se queda en el
             registro), arriba y adelante (float); medio = (arriba < adelante o NaN) ? adelante : arriba; maximo = (derecha < medio o NaN) ? medio : float(derecha); 0x7EDB30(maximo); [+3] &= 0xFE (releido);
             [+0x38] = float(raiz * [+0x28] (releido)); devuelve +0x2C
   6  0x749520  DeInit(0x8D6264, clump); atomos (enlace en +0x40; siguiente leido antes); luces (lista en clump+0x10; desplazamiento [0xC924DC] releido por vuelta; el objeto es enlace - desplazamiento - 4 y los
             datos del plugin estan en enlace - 4: se saca el enlace de la lista y se ponen a cero 3 dwords; 0x7520D0(objeto); un objeto nulo corta); camaras (clump+0x18, [0xC924D8], 0x7EE4B0);
             marco [clump+4] != 0 -> 0x7F08A0; libera el clump con +0x148 (lista +4 de [0xC924AC] + RwEngineInstance)
   7, 15  0x7496A0, 0x749DC0  DeInit(0x8D624C, atomo); [+0x14] != 0 -> 0x807DE0; [+0x18] != 0 -> 0x74CCC0, marco [+4] leido DESPUES, [+0x18] = 0, marco != 0 y 0x751050(atomo) != 0 -> 0x7F0910(marco);
             0x804F40(atomo); +0x148([[0xC924AC] + RwEngineInstance], atomo) (RwEngineInstance y desplazamiento releidos); 0x749DC0 devuelve 1
   8  0x749830  0x7EE450(0xC, 0x10, 0x749880, 0x7498A0, 0) -> [0xC924D8], < 0 -> 0; 0x751D60(0xC, 0x10, 0x7498B0, 0x7498D0, 0) -> [0xC924DC]; devuelve (>= 0)
   9  0x749B20  para cada atomo (enlace en +0x40; el siguiente se lee DESPUES del callback) con byte [+2] & 4: 0x7F0990([+4]); [+0x48](atomo) = 0 -> resultado 0; devuelve el clump o 0
  10  0x749BB0  camaras (lista en clump+0x18; objeto = enlace - [0xC924D8] - 4; siguiente leido ANTES; un resultado nulo corta); devuelve el clump
  11  0x749C00  luces (lista en clump+0x10; [0xC924DC]); igual
  12  0x749C50  +0x144([[0xC924AC] + RwEngineInstance], 0x30014) = 0 -> 0; campos (sync 0x749D20, callback de dibujo 0x7491C0); 0x804EF0(atomo, 0); 0x8086E0(0x8D624C, atomo); devuelve el atomo
  13  0x749D20  [+0x4C] & 2 -> 0x7491F0(atomo); [+3] |= 1 (releido); devuelve el atomo
  14  0x749D40  si geometria != [+0x18]: 0x74CCB0(geom) si no nula; 0x74CCC0([+0x18]) si no nula (releido); [+0x18] = geom; sin el bit 0 de las banderas: esfera del morph target 0 ([[geom+0x5C] + 4], 4 dwords)
             y con marco [+4] != 0 y 0x751050(atomo) != 0: 0x7F0910(marco); devuelve el atomo
  16  0x749E60  0x749C50() = 0 -> 0; copia bytes +0..+3, [+4] = 0, 0x804EF0(clon, 0), [+3] |= 1; geometria del origen (leida DESPUES) distinta de la del clon: 0x74CCB0 (si no nula), 0x74CCC0(del clon, si no nula),
             clon = origen, esfera del morph target 0 (si no nula), marco [clon+4] != 0 y 0x751050(clon) != 0 -> 0x7F0910; esfera local (+0x1C, 4 dwords), +0x48, +0x6C, interpolador (+0x4C, 5 dwords),
             0x808770(0x8D624C, clon, origen)

Los falsos anotan [id, argumentos...] (ver LiveComparison.hpp); tras cada evento puede aplicarse el efecto lateral ("poke") del caso. Los punteros de funcion que el EXE guarda en el atomo (+0x10 sync = 0x749D20,
+0x48 dibujo = 0x7491C0) se comparan por su direccion del EXE (el banco canoniza la direccion del cuerpo de la DLL de esas dos funciones, que es lo que ejecuta el hook).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
import zlib

M32 = 0xFFFFFFFF
NAMES = ["CGamma::SetGammaNoFade", "RpAnisot::RpAnisotPluginAttach", "RpWorld::AtomicRightsStreamRead", "RpWorld::AtomicRightsStreamWrite", "RpWorld::_rpAtomicResyncInterpolatedSphere",
         "RpWorld::RpAtomicGetWorldBoundingSphere", "RpWorld::ClumpFreeListDestroyCallBack", "RpWorld::AtomicFreeListDestroyCallBack", "RpWorld::ClumpPluginAttach", "RpWorld::RpClumpRender",
         "RpWorld::RpClumpForAllCameras", "RpWorld::RpClumpForAllLights", "RpWorld::RpAtomicCreate", "RpWorld::AtomicSyncCallBack", "RpWorld::RpAtomicSetGeometry", "RpWorld::RpAtomicDestroy",
         "RpWorld::RpAtomicClone"]
ENTRIES = (0x747DB0, 0x748F70, 0x749120, 0x749160, 0x7491F0, 0x749330, 0x749520, 0x7496A0, 0x749830, 0x749B20, 0x749BB0, 0x749C00, 0x749C50, 0x749D20, 0x749D40, 0x749DC0, 0x749E60)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(GAMMA, ANISOT, READ, WRITE, RESYNC, WORLDBS, CLUMPFREE, ATOMFREE, CLUMPATTACH, RENDER, FORALL, CREATE, SYNC, SETGEOM, CLONE) = range(15)
KIND = [GAMMA, ANISOT, READ, WRITE, RESYNC, WORLDBS, CLUMPFREE, ATOMFREE, CLUMPATTACH, RENDER, FORALL, FORALL, CREATE, SYNC, SETGEOM, ATOMFREE, CLONE]
GLOBALS = (0xC92134, 0xC920C8, 0xC8D4C8, 0xC92478, 0xC9247C, 0xC924A8, 0xC92480, 0xC924AC, 0xC924D8, 0xC924DC, 0xC97B24)
GSIZE = (0x10, 4, 0xC00, 4, 4, 4, 4, 4, 4, 4, 4)
G_OF = {0: (0, 1, 2), 1: (3, 4), 2: (5, 6), 6: (7, 8, 9, 10), 7: (7, 10), 15: (7, 10), 12: (7, 10), 8: (8, 9), 10: (8,), 11: (9,)}
ENGINE1, ENGINE2 = 0xE0000001, 0xE0000002
RET_EAX = {1, 2, 3, 5, 8, 9, 10, 11, 12, 13, 14, 15, 16}


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


# ------------------------------------------------------------------------------------------------ aritmetica x87 (precision de 24 bits)
class NaNv:
    """NaN con su patron de 32 bits (silencioso)"""
    __slots__ = ('bits',)

    def __init__(self, bits):
        self.bits = (bits | 0x400000) & M32


DEFAULT_NAN = NaNv(0xFFC00000)


def from_bits(b):
    b &= M32
    if (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF):
        return NaNv(b)                      # fld de un NaN de senal lo vuelve silencioso
    return struct.unpack('<f', struct.pack('<I', b))[0]


def to_bits(x):
    if isinstance(x, NaNv):
        return x.bits
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0xFF800000 if x < 0 else 0x7F800000


def round24(x):
    if isinstance(x, NaNv) or x == 0 or math.isinf(x):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)), e - 24)


def nan_pick(a, b):
    """regla de la x87 con operandos NaN: el de mayor significando; con uno solo, ese"""
    if isinstance(a, NaNv) and isinstance(b, NaNv):
        sa, sb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if sa != sb:
            return a if sa > sb else b
        return b if (a.bits >> 31) else a      # significandos iguales: gana el de signo positivo (comprobado en el EXE con 0xFFC00000 y 0x7FC00000)
    return a if isinstance(a, NaNv) else b


def binop(a, b, op):
    if isinstance(a, NaNv) or isinstance(b, NaNv):
        return nan_pick(a, b)
    if op == '+':
        r = a + b
    elif op == '-':
        r = a - b
    else:
        r = a * b
    if math.isnan(r):
        return DEFAULT_NAN
    return round24(r)


def add(a, b):
    return binop(a, b, '+')


def sub(a, b):
    return binop(a, b, '-')


def mul(a, b):
    return binop(a, b, '*')


def lt_or_unordered(a, b):
    """C0 tras fcomp: a < b o no ordenados"""
    if isinstance(a, NaNv) or isinstance(b, NaNv):
        return True
    return a < b


def fl(x):
    """almacenamiento a float y recarga"""
    return from_bits(to_bits(x))


F = from_bits


def gamma_entry(p):
    """trunc(r24(p * 65535.0)) & 0xFFFF con la x87 (producto exacto redondeado a 24 bits, _ftol de 64 bits)"""
    if math.isnan(p) or math.isinf(p) or p == 0:
        return 0
    m, e = math.frexp(p)
    num = int(m * (1 << 53)) * 65535
    exp2 = e - 53
    shift = num.bit_length() - 24
    if shift > 0:
        q, rem, half = num >> shift, num & ((1 << shift) - 1), 1 << (shift - 1)
        if rem > half or (rem == half and (q & 1)):
            q += 1
        num, exp2 = q, exp2 + shift
    t = num << exp2 if exp2 >= 0 else num >> (-exp2)
    return 0 if t >= (1 << 63) else (t & 0xFFFF)


# ------------------------------------------------------------------------------------------------ memoria
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

    def val(self, spec):
        """especificacion de valor del caso -> valor (las tablas y los callbacks se dejan simbolicos)"""
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

    def rget(self, g, off=0, n=4):
        return int.from_bytes(self.R[g][off:off + n], 'little')

    def rput(self, g, off, v, n=4):
        self.R[g][off:off + n] = (v & ((1 << (8 * n)) - 1)).to_bytes(n, 'little')

    def snap(self):
        for m in self.mem:
            m.snap()
        self.Rref = {g: bytes(b) for g, b in self.R.items()}


KEYS = {}
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'obj_aligned', 'obj_unaligned']
_EXTRA = {
    GAMMA: ['disabled', 'equal_target', 'dev_null', 'changed_set', 'changed_clear', 'table_written', 'level_nan', 'level_inf', 'poke_applied', 'vt2'],
    ANISOT: ['fail1', 'fail2', 'fail3', 'ok', 'poke_applied'],
    READ: ['len8', 'len_other', 'fail_first', 'fail_second', 'ok'],
    WRITE: ['fail_first', 'fail_second', 'ok', 'poke_applied'],
    RESYNC: ['geom_null', 'interp', 'same_idx', 'idx_oob', 'idx_neg', 'copy_start', 'copy_zero', 'nan_result'],
    WORLDBS: ['resync', 'clean_skip', 'dirty_frame', 'dirty_flag', 'ortho', 'scaled', 'max_right', 'max_up', 'max_at', 'poke_applied', 'nan_result'],
    CLUMPFREE: ['atoms0', 'atoms_many', 'lights0', 'lights_many', 'cams0', 'cams_many', 'parent_set', 'parent_null', 'poke_applied', 'engine2', 'getworld_yes', 'getworld_no'],
    ATOMFREE: ['res_set', 'res_null', 'geom_set', 'geom_null', 'parent_set', 'getworld_yes', 'getworld_no', 'poke_applied', 'engine2'],
    CLUMPATTACH: ['fail_cam', 'ok_light', 'fail_light', 'poke_applied'],
    RENDER: ['empty', 'one', 'many', 'flag_set', 'flag_clear', 'cb_null_result', 'cb_ok', 'poke_applied'],
    FORALL: ['empty', 'one', 'many', 'stop_early', 'all', 'poke_applied'],
    CREATE: ['alloc_fail', 'alloc_ok', 'poke_applied'],
    SYNC: ['resync', 'no_resync', 'poke_applied'],
    SETGEOM: ['same', 'cur_null', 'arg_null', 'copy_sphere', 'flag_same_sphere', 'getworld_yes', 'getworld_no', 'poke_applied', 'parent_set'],
    CLONE: ['create_fail', 'geom_null', 'geom_copy', 'geom_equal', 'getworld_yes', 'getworld_no', 'poke_applied', 'parent_set'],
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

    def ev(self, eid, args, ptr=0, after=None, ptr2=0):
        self.events.append([eid] + list(args))
        if after:
            after()
        a = self.a
        if a[40] and eid == a[40]:
            n = self.pcount
            self.pcount += 1
            if n == a[41]:
                val = a[46] if a[45] == 0 else self.W.val(a[46])
                if a[42] == 0:
                    self.W.rput(a[43], a[44], val)
                else:
                    at = self.W.raw(a[43], a[44]) if a[42] == 1 else ((ptr if a[42] == 2 else ptr2) + a[44]) & M32
                    self.W.wr(at, 4, val)
                self.marks.append('poke_applied')

    def tag(self, v):
        return self.W.tagv(v)

    def eng_word(self, eng, idx):
        """palabra de la tabla E1 / E2 (las ranuras de funcion no se usan como listas)"""
        return rnd((self.a[47] + (0 if eng == ENGINE1 else 1)) & M32, idx)


def sel_engine(X):
    e = X.W.rget(10)
    if e not in (ENGINE1, ENGINE2):
        raise RuntimeError('RwEngineInstance inesperada %x' % e)
    return e


def free_via_engine(X, node, slot=0):
    """+0x148([[0xC924AC] + RwEngineInstance + 4 slot], nodo): el EXE lee RwEngineInstance y el desplazamiento al reunir los argumentos"""
    eng = sel_engine(X)
    off = X.W.rget(7)
    lst = X.eng_word(eng, ((off + 4 * slot) // 4) & 0x3FFFFFFF)
    X.ev(0xE148 + (0x100 if eng == ENGINE2 else 0), [lst, X.tag(node)], node)


def atom_destroy(X, atom, getworld_ret):
    """cuerpo comun de RpAtomicDestroy (0x749DC0), AtomicFreeListDestroyCallBack (0x7496A0) y del bucle de 0x749520"""
    W = X.W
    X.ev(0x808740, [0x8D624C, X.tag(atom)], atom)
    res = W.rd(atom + 0x14)
    if res:
        X.marks.append('res_set')
        X.ev(0x807DE0, [X.tag(res)], res)
    else:
        X.marks.append('res_null')
    geom = W.rd(atom + 0x18)
    if geom:
        X.marks.append('geom_set')
        X.ev(0x74CCC0, [X.tag(geom)], geom)
        parent = W.rd(atom + 4)
        W.wr(atom + 0x18, 4, 0)
        if parent:
            X.marks.append('parent_set')
            r = getworld_ret()
            X.ev(0x751050, [X.tag(atom)], atom)
            if r:
                X.marks.append('getworld_yes')
                X.ev(0x7F0910, [X.tag(parent)], parent)
            else:
                X.marks.append('getworld_no')
    else:
        X.marks.append('geom_null')
    X.ev(0x804F40, [X.tag(atom)], atom)
    free_via_engine(X, atom)


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
        if g == 0:
            for j, v in enumerate((a[48], a[59], a[60], a[61])):
                W.rput(0, 4 * j, v)
        elif g == 2:
            W.R[2][:] = fill_words(a[62], 0xC00)
        else:
            W.rput(g, 0, a[48 + g])
    big = None

    if k == GAMMA:
        # dispositivos: 1 = objeto A en o (vtable a[12] en C), 2 = objeto D en a[13] (vtable a[14] en C)
        devs = {1: W.raw(0, o), 2: W.raw(3, a[13])}
        vts = {W.raw(2, a[12]): 1, W.raw(2, a[14]): 2}
        A.wr(o, 4, W.raw(2, a[12]))
        D.wr(a[13], 4, W.raw(2, a[14]))
        W.snap()
        calls = [0]

        def dev_reset():
            n = calls[0]
            calls[0] += 1
            v = a[11] if n == 1 else a[10]
            d = devs.get(v, 0)
            X.ev(0x7F9D50, [])
            return d

        def vtid(d):
            return vts[W.rd(d)]
        L = F(a[4])
        if isinstance(L, NaNv):
            marks.append('level_nan')
        elif math.isinf(L):
            marks.append('level_inf')
        level = fl(mul(L, F(0x3F333333)))
        if W.rget(0, 0) == 0:
            marks.append('disabled')
        else:
            W.rput(0, 0xC, to_bits(level))
            tgt = F(W.rget(0, 8))
            equal = (not isinstance(level, NaNv)) and (not isinstance(tgt, NaNv)) and level == tgt
            if equal:
                marks.append('equal_target')
            else:
                W.rput(0, 8, to_bits(level))
                dev1 = dev_reset()
                if dev1 == 0:
                    marks.append('dev_null')
                else:
                    power = fl(add(sub(F(0x3F800000), level), F(0x3E4CCCCD)))
                    if W.rget(1, 0, 1) == 0:
                        marks.append('changed_clear')
                        W.rput(1, 0, 1, 1)
                        dev2 = dev_reset()
                        vid = vtid(dev2)

                        def fill():
                            if a[15]:
                                W.R[2][0:0x600] = fill_words(a[15], 0x600)
                        X.ev(0xC016 + 0x100 * (vid - 1), [X.tag(dev2), 0, 0xC8D4C8], dev2, fill)
                    else:
                        marks.append('changed_set')
                    py = math.nan if isinstance(power, NaNv) else power
                    for i in range(256):
                        x = (i + 1) * 0.00390625
                        if isinstance(power, NaNv) or (x == 1.0 and math.isinf(py)):
                            p = math.nan
                        else:
                            try:
                                p = math.pow(x, py)
                            except OverflowError:
                                p = math.inf
                            except ValueError:
                                p = math.nan
                        v = gamma_entry(p)
                        for base in (0x600, 0x800, 0xA00):
                            W.rput(2, base + 2 * i, v, 2)
                    marks.append('table_written')
                    vid1 = vtid(dev1)
                    X.ev(0xC015 + 0x100 * (vid1 - 1), [X.tag(dev1), 0, 1, 0xC8DAC8, zlib.crc32(bytes(W.R[2][0x600:0xC00])) & M32], dev1)
                    if vid1 == 2:
                        marks.append('vt2')
        big = zlib.crc32(bytes(W.R[2])) & M32
    elif k == ANISOT:
        W.snap()
        X.ev(0x7F2BB0, [0, 0x127, 0x749000, 0x749010])
        if s32(a[4]) < 0:
            marks.append('fail1')
            X.ret = 0
        else:
            X.ev(0x7F3BB0, [1, 0x127, 0x749020, 0x749030, 0x749040])
            W.rput(3, 0, a[5])
            if s32(a[5]) < 0:
                marks.append('fail2')
                X.ret = 0
            else:
                X.ev(0x804550, [0x127, 0x749060, 0x7490B0, 0x7490F0])
                W.rput(4, 0, a[6])
                if s32(a[6]) < 0:
                    marks.append('fail3')
                    X.ret = 0
                else:
                    X.ev(0x7FDE60, [W.rget(3)])
                    marks.append('ok')
                    X.ret = 1
    elif k == READ:
        W.snap()
        stream = a[4]
        vals = [a[11], a[12]]
        calls = [0]

        def read(dest):
            n = calls[0]
            calls[0] += 1
            X.ev(0x7ED540, [X.tag(stream), dest, 4], 0, lambda: W.rput(5 if dest == 0xC924A8 else 6, 0, vals[n]))
            return a[9 + n]
        marks.append('len8' if a[5] == 8 else 'len_other')
        if not read(0xC924A8):
            marks.append('fail_first')
            X.ret = 0
        elif a[5] == 8 and not read(0xC92480):
            marks.append('fail_second')
            X.ret = 0
        else:
            marks.append('ok')
            X.ret = stream
    elif k == WRITE:
        A.wr(o + 0x6C, 4, W.raw(1, a[8]))
        W.snap()
        stream = a[4]
        rets = [a[10], a[11]]
        calls = [0]

        def write(off):
            p = W.rd(this + 0x6C)
            ptr = (p + off) & M32
            n = calls[0]
            calls[0] += 1
            X.ev(0x7ED460, [X.tag(stream), X.tag(ptr), 4, W.rd(ptr)], ptr)
            return rets[n]
        if not write(0x2C):
            marks.append('fail_first')
            X.ret = 0
        elif not write(0x30):
            marks.append('fail_second')
            X.ret = 0
        else:
            marks.append('ok')
            X.ret = stream
    elif k in (RESYNC, WORLDBS):
        geom = W.raw(1, a[12]) if a[4] else 0
        A.wr(o + 0x18, 4, geom)
        A.wr(o + 0x4C, 4, a[8])
        A.wr(o + 0x50, 2, a[6] & 0xFFFF)
        A.wr(o + 0x52, 2, a[7] & 0xFFFF)
        A.wr(o + 0x58, 4, a[9])
        A.wr(o + 0x5C, 4, a[10])
        A.wr(o + 3, 1, a[11] & 0xFF)
        for j in range(4):
            A.wr(o + 0x1C + 4 * j, 4, a[64 + j])
        B.wr(a[12] + 0x18, 4, a[5])
        B.wr(a[12] + 0x5C, 4, W.raw(3, a[13]))
        for i in range(15):
            for j in range(4):
                D.wr(a[13] + 0x1C * (i - 4) + 4 + 4 * j, 4, a[68 + 4 * i + j])
        ltm = 0
        if k == WORLDBS:
            A.wr(o + 4, 4, W.val(a[31]))
            m = a[16]
            ltm = W.raw(2, m)
            for j, off in enumerate((0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28)):
                Cc.wr(m + off, 4, a[18 + j])
            Cc.wr(m + 0xC, 4, a[17])
        W.snap()

        def resync():
            geo = A.rd(o + 0x18, 4)
            if geo == 0:
                marks.append('geom_null')
                return
            di, bp = s16(A.rd(o + 0x50, 2)), s16(A.rd(o + 0x52, 2))
            num = s32(W.rd(geo + 0x18))
            mt = W.rd(geo + 0x5C)

            def sphere(i):
                return [W.rd((mt + 0x1C * i + 4 + 4 * j) & M32) for j in range(4)]
            if di != bp and di < num and bp < num:
                marks.append('interp')
                st, en = sphere(di), sphere(bp)
                s = mul(F(A.rd(o + 0x5C, 4)), F(A.rd(o + 0x58, 4)))
                rad = add(mul(sub(F(en[3]), F(st[3])), s), F(st[3]))
                A.wr(o + 0x28, 4, to_bits(rad))
                diff = [fl(sub(F(en[j]), F(st[j]))) for j in range(3)]
                xp = mul(diff[0], s)
                yp = fl(mul(diff[1], s))
                zp = fl(mul(diff[2], s))
                A.wr(o + 0x1C, 4, to_bits(add(F(st[0]), xp)))
                A.wr(o + 0x20, 4, to_bits(add(F(st[1]), yp)))
                A.wr(o + 0x24, 4, to_bits(add(F(st[2]), zp)))
                if any(isinstance(F(A.rd(o + q, 4)), NaNv) for q in (0x1C, 0x20, 0x24, 0x28)):
                    marks.append('nan_result')
            elif di < num and bp < num:
                marks.append('copy_start')
                for j, v in enumerate(sphere(di)):
                    A.wr(o + 0x1C + 4 * j, 4, v)
            else:
                marks.append('copy_zero')
                if di < -4 or di > 9 or bp < -4 or bp > 9:
                    marks.append('idx_oob')
                for j, v in enumerate(sphere(0)):
                    A.wr(o + 0x1C + 4 * j, 4, v)
            if di == bp:
                marks.append('same_idx')
            if di < 0 or bp < 0:
                marks.append('idx_neg')
            A.wr(o + 0x4C, 4, A.rd(o + 0x4C, 4) & 0xFFFFFFFD)
            A.wr(o + 3, 1, A.rd(o + 3, 1) | 1)

        if k == RESYNC:
            resync()
        else:
            flags = A.rd(o + 0x4C, 1)
            frame = A.rd(o + 4, 4)
            if flags & 2:
                marks.append('resync')
                X.ev(0x7491F0, [X.tag(this)], this)
            dirty = a[15]
            X.ev(0x7F0340, [X.tag(frame)])
            if dirty == 0 and not (A.rd(o + 3, 1) & 1):
                marks.append('clean_skip')
            else:
                marks.append('dirty_frame' if dirty else 'dirty_flag')
                X.ev(0x7F0990, [X.tag(frame)])
                ins = [W.rd(this + 0x1C + 4 * j) for j in range(3)]
                outp = (this + 0x2C) & M32

                def transform():
                    for j in range(3):
                        W.wr(outp + 4 * j, 4, ins[j] ^ a[28 + j])
                X.ev(0x7EDD60, [X.tag(outp), X.tag(this + 0x1C), X.tag(ltm), ins[0], ins[1], ins[2]], 0, transform)
                if (W.rd(ltm + 0xC) & 3) == 3:
                    marks.append('ortho')
                    A.wr(o + 0x38, 4, A.rd(o + 0x28, 4))
                    A.wr(o + 3, 1, A.rd(o + 3, 1) & 0xFE)
                else:
                    marks.append('scaled')

                    def sq(base):
                        x, y, z = F(W.rd(ltm + base)), F(W.rd(ltm + base + 4)), F(W.rd(ltm + base + 8))
                        return add(add(mul(x, x), mul(y, y)), mul(z, z))
                    rsq = sq(0)
                    usq = fl(sq(0x10))
                    asq = fl(sq(0x20))
                    use_at = lt_or_unordered(usq, asq)
                    middle = asq if use_at else usq
                    if lt_or_unordered(rsq, middle):
                        mx = middle
                        marks.append('max_at' if use_at else 'max_up')
                    else:
                        mx = fl(rsq)
                        marks.append('max_right')
                    X.ev(0x7EDB30, [to_bits(mx)], 0)
                    scaled = mul(F(a[27]), F(A.rd(o + 0x28, 4)))
                    A.wr(o + 3, 1, A.rd(o + 3, 1) & 0xFE)
                    A.wr(o + 0x38, 4, to_bits(scaled))
                    if isinstance(scaled, NaNv):
                        marks.append('nan_result')
            X.ret = (this + 0x2C) & M32
    elif k == CLUMPFREE:
        L_a, L_l, L_c = a[4], a[5], a[6]
        bB, sB = a[7], a[8]
        A.wr(o + 4, 4, W.val(a[14]))
        atoms = [W.raw(1, bB + sB * i) for i in range(L_a)]
        head_a = (this + 8) & M32
        links_a = [(x + 0x40) & M32 for x in atoms]
        A.wr(o + 8, 4, links_a[0] if L_a else head_a)
        A.wr(o + 0xC, 4, links_a[-1] if L_a else head_a)
        for i in range(L_a):
            B.wr(bB + sB * i + 0x40, 4, links_a[i + 1] if i + 1 < L_a else head_a)
            B.wr(bB + sB * i + 0x44, 4, links_a[i - 1] if i else head_a)
            p = 64 + 4 * i
            B.wr(bB + sB * i + 0x14, 4, a[p])
            B.wr(bB + sB * i + 0x18, 4, a[p + 1])
            B.wr(bB + sB * i + 4, 4, W.val(a[p + 2]))
        for (cnt, base, sep, arena, headoff) in ((L_l, a[9], a[10], 2, 0x10), (L_c, a[11], a[12], 3, 0x18)):
            head = (this + headoff) & M32
            nodes = [W.raw(arena, base + sep * i) for i in range(cnt)]
            A.wr(o + headoff, 4, nodes[0] if cnt else head)
            A.wr(o + headoff + 4, 4, nodes[-1] if cnt else head)
            mm = W.mem[arena]
            for i in range(cnt):
                mm.wr(base + sep * i, 4, nodes[i + 1] if i + 1 < cnt else head)
                mm.wr(base + sep * i + 4, 4, nodes[i - 1] if i else head)
        W.snap()
        gwm = a[13]
        gwc = [0]

        def getworld_ret():
            n = gwc[0]
            gwc[0] += 1
            return (gwm >> (n % 32)) & 1
        X.ev(0x808740, [0x8D6264, X.tag(this)], this)
        marks.append('atoms0' if L_a == 0 else 'atoms_many')
        marks.append('lights0' if L_l == 0 else 'lights_many')
        marks.append('cams0' if L_c == 0 else 'cams_many')
        if sel_engine(X) == ENGINE2:
            marks.append('engine2')
        link = W.rd(this + 8)
        while link != head_a:
            nxt = W.rd(link)
            atom_destroy(X, (link - 0x40) & M32, getworld_ret)
            link = nxt

        def unlink_loop(headoff, reg, event):
            head = (this + headoff) & M32
            link = W.rd(this + headoff)
            while link != head:
                off = W.rget(reg)
                nxt = W.rd(link)
                obj = (link - off - 4) & M32
                pd = (obj + off) & M32
                prev, nx = W.rd(pd + 8), W.rd(pd + 4)
                W.wr(prev, 4, nx)
                nx2, prev2 = W.rd(pd + 4), W.rd(pd + 8)
                W.wr(nx2 + 4, 4, prev2)
                W.wr(pd + 8, 4, 0)
                W.wr(pd + 4, 4, 0)
                W.wr(pd, 4, 0)
                X.ev(event, [X.tag(obj)], obj)
                if obj == 0:
                    break
                link = nxt
        unlink_loop(0x10, 9, 0x7520D0)
        unlink_loop(0x18, 8, 0x7EE4B0)
        par = W.rd(this + 4)
        if par:
            marks.append('parent_set')
            X.ev(0x7F08A0, [X.tag(par)], par)
        else:
            marks.append('parent_null')
        free_via_engine(X, this, 1)
    elif k == ATOMFREE:
        A.wr(o + 0x14, 4, a[4])
        A.wr(o + 0x18, 4, a[5])
        A.wr(o + 4, 4, W.val(a[6]))
        W.snap()
        if sel_engine(X) == ENGINE2:
            marks.append('engine2')
        atom_destroy(X, this, lambda: 1 if a[7] else 0)
        if fn == 15:
            X.ret = 1
    elif k == CLUMPATTACH:
        W.snap()
        X.ev(0x7EE450, [0xC, 0x10, 0x749880, 0x7498A0, 0])
        W.rput(8, 0, a[4])
        if s32(a[4]) < 0:
            marks.append('fail_cam')
            X.ret = 0
        else:
            X.ev(0x751D60, [0xC, 0x10, 0x7498B0, 0x7498D0, 0])
            W.rput(9, 0, a[5])
            if s32(a[5]) < 0:
                marks.append('fail_light')
                X.ret = 0
            else:
                marks.append('ok_light')
                X.ret = 1
    elif k == RENDER:
        cnt, bB, sB = a[4], a[5], a[6]
        atoms = [W.raw(1, bB + sB * i) for i in range(cnt)]
        head = (this + 8) & M32
        links = [(x + 0x40) & M32 for x in atoms]
        A.wr(o + 8, 4, links[0] if cnt else head)
        A.wr(o + 0xC, 4, links[-1] if cnt else head)
        for i in range(cnt):
            B.wr(bB + sB * i + 0x40, 4, links[i + 1] if i + 1 < cnt else head)
            B.wr(bB + sB * i + 0x44, 4, links[i - 1] if i else head)
            p = 64 + 4 * i
            B.wr(bB + sB * i + 2, 1, a[p] & 0xFF)
            B.wr(bB + sB * i + 4, 4, W.val(a[p + 1]))
            B.wr(bB + sB * i + 0x48, 4, a[p + 2])      # funcion falsa 0xF00000kk: simbolica
        W.snap()
        marks.append('empty' if cnt == 0 else ('one' if cnt == 1 else 'many'))
        result = this
        link = W.rd(this + 8)
        ncalls = 0
        while link != head:
            atom = (link - 0x40) & M32
            if W.rd(atom + 2, 1) & 4:
                marks.append('flag_set')
                X.ev(0x7F0990, [X.tag(W.rd(atom + 4))], 0)
                kk = W.rd(atom + 0x48) & 0xFF
                r = (a[7] >> (ncalls % 32)) & 1
                ncalls += 1
                X.ev(0xCB10 + kk, [X.tag(atom)], atom)
                if not r:
                    result = 0
                    marks.append('cb_null_result')
                else:
                    marks.append('cb_ok')
            else:
                marks.append('flag_clear')
            link = W.rd(link)
        X.ret = result
    elif k == FORALL:
        cnt, base, sep = a[4], a[5], a[6]
        cams = fn == 10
        headoff = 0x18 if cams else 0x10
        reg = 8 if cams else 9
        nodes = [W.raw(2, base + sep * i) for i in range(cnt)]
        head = (this + headoff) & M32
        A.wr(o + headoff, 4, nodes[0] if cnt else head)
        A.wr(o + headoff + 4, 4, nodes[-1] if cnt else head)
        for i in range(cnt):
            Cc.wr(base + sep * i, 4, nodes[i + 1] if i + 1 < cnt else head)
            Cc.wr(base + sep * i + 4, 4, nodes[i - 1] if i else head)
        W.snap()
        marks.append('empty' if cnt == 0 else ('one' if cnt == 1 else 'many'))
        data = W.val(a[9])
        link = W.rd(this + headoff)
        calls = 0
        stopped = False
        while link != head:
            off = W.rget(reg)
            nxt = W.rd(link)
            obj = (link - off - 4) & M32
            r = (a[7] >> (calls % 32)) & 1
            calls += 1
            X.ev(0xCB20 + (a[8] & 0xFF), [X.tag(obj), X.tag(data)], obj, None, link)
            if not r:
                stopped = True
                break
            link = nxt
        marks.append('stop_early' if stopped else 'all')
        X.ret = this
    elif k == CREATE:
        W.snap()
        eng = sel_engine(X)
        off = W.rget(7)
        X.ev(0xE144 + (0x100 if eng == ENGINE2 else 0), [X.eng_word(eng, off // 4), 0x30014])
        if not a[4]:
            marks.append('alloc_fail')
            X.ret = 0
        else:
            marks.append('alloc_ok')
            A.wr(o + 0, 1, 1)
            A.wr(o + 1, 1, 0)
            A.wr(o + 4, 4, 0)
            A.wr(o + 0x10, 4, 0x749D20)
            A.wr(o + 0x14, 4, 0)
            A.wr(o + 2, 1, 5)
            A.wr(o + 3, 1, 1)
            X.ev(0x804EF0, [X.tag(this), 0], this)
            A.wr(o + 3, 1, A.rd(o + 3, 1) | 1)
            A.wr(o + 0x18, 4, 0)
            for q in (0x28, 0x1C, 0x20, 0x24, 0x38, 0x2C, 0x30, 0x34):
                A.wr(o + q, 4, 0)
            A.wr(o + 0x48, 4, 0x7491C0)
            A.wr(o + 0x54, 4, 0x3F800000)
            A.wr(o + 0x58, 4, 0x3F800000)
            A.wr(o + 0x50, 2, 0)
            A.wr(o + 0x52, 2, 0)
            A.wr(o + 0x5C, 4, 0)
            A.wr(o + 0x4C, 4, 3)
            A.wr(o + 0x44, 4, 0)
            A.wr(o + 0x40, 4, 0)
            A.wr(o + 0x3C, 4, 0)
            A.wr(o + 0x6C, 4, 0)
            ll = (this + 0x64) & M32
            A.wr(o + 0x64, 4, ll)
            A.wr(o + 0x68, 4, ll)
            X.ev(0x8086E0, [0x8D624C, X.tag(this)], this)
            X.ret = this
    elif k == SYNC:
        A.wr(o + 0x4C, 4, a[4])
        A.wr(o + 3, 1, a[5] & 0xFF)
        W.snap()
        if A.rd(o + 0x4C, 1) & 2:
            marks.append('resync')
            X.ev(0x7491F0, [X.tag(this)], this)
        else:
            marks.append('no_resync')
        A.wr(o + 3, 1, A.rd(o + 3, 1) | 1)
        X.ret = this
    elif k == SETGEOM:
        G1, G2 = W.raw(1, a[8]), W.raw(1, a[9])
        specs = {0: 0, 1: G1, 2: G2, 3: a[13]}
        cur = specs[a[4]]
        arg = cur if a[5] == 4 else (a[14] if a[5] == 3 else specs[a[5]])
        flags = a[6]
        A.wr(o + 0x18, 4, cur)
        A.wr(o + 4, 4, W.val(a[7]))
        for j in range(4):
            A.wr(o + 0x1C + 4 * j, 4, a[64 + j])
        B.wr(a[8] + 0x5C, 4, W.raw(3, a[10]))
        B.wr(a[9] + 0x5C, 4, W.raw(3, a[11]))
        for j in range(4):
            D.wr(a[10] + 4 + 4 * j, 4, a[68 + j])
            D.wr(a[11] + 4 + 4 * j, 4, a[72 + j])
        W.snap()
        if arg == cur:
            marks.append('same')
        else:
            if cur == 0:
                marks.append('cur_null')
            if arg == 0:
                marks.append('arg_null')
            if arg:
                X.ev(0x74CCB0, [X.tag(arg)], arg)
            c2 = A.rd(o + 0x18, 4)
            if c2:
                X.ev(0x74CCC0, [X.tag(c2)], c2)
            A.wr(o + 0x18, 4, arg)
            if flags & 1:
                marks.append('flag_same_sphere')
            else:
                if arg:
                    marks.append('copy_sphere')
                    mt = W.rd(arg + 0x5C)
                    for j in range(4):
                        A.wr(o + 0x1C + 4 * j, 4, W.rd((mt + 4 + 4 * j) & M32))
                par = A.rd(o + 4, 4)
                if par:
                    marks.append('parent_set')
                    X.ev(0x751050, [X.tag(this)], this)
                    if a[12]:
                        marks.append('getworld_yes')
                        X.ev(0x7F0910, [X.tag(par)], par)
                    else:
                        marks.append('getworld_no')
        X.ret = this
    elif k == CLONE:
        clone = W.raw(1, a[4])
        gc = a[13] if a[6] else 0
        sg = 0 if a[5] == 0 else (W.raw(2, a[10]) if a[5] == 1 else gc)
        A.wr(o + 0x18, 4, sg)
        B.wr(a[4] + 0x18, 4, gc)
        Cc.wr(a[10] + 0x5C, 4, W.raw(3, a[11]))
        for j in range(4):
            D.wr(a[11] + 4 + 4 * j, 4, a[64 + j])
        W.snap()
        X.ev(0x749C50, [])
        if a[7]:
            marks.append('create_fail')
            X.ret = 0
        else:
            for q in range(4):
                B.wr(a[4] + q, 1, A.rd(o + q, 1))
            B.wr(a[4] + 4, 4, 0)
            B.wr(a[4] + 2, 1, A.rd(o + 2, 1))
            X.ev(0x804EF0, [X.tag(clone), 0], clone)
            B.wr(a[4] + 3, 1, B.rd(a[4] + 3, 1) | 1)
            cg = B.rd(a[4] + 0x18, 4)
            sg2 = A.rd(o + 0x18, 4)
            if sg2 == cg:
                marks.append('geom_equal')
            else:
                if sg2:
                    X.ev(0x74CCB0, [X.tag(sg2)], sg2)
                cg2 = B.rd(a[4] + 0x18, 4)
                if cg2:
                    X.ev(0x74CCC0, [X.tag(cg2)], cg2)
                B.wr(a[4] + 0x18, 4, sg2)
                if sg2:
                    marks.append('geom_copy')
                    mt = W.rd(sg2 + 0x5C)
                    for j in range(4):
                        B.wr(a[4] + 0x1C + 4 * j, 4, W.rd((mt + 4 + 4 * j) & M32))
                else:
                    marks.append('geom_null')
                par = B.rd(a[4] + 4, 4)
                if par:
                    marks.append('parent_set')
                    X.ev(0x751050, [X.tag(clone)], clone)
                    if a[9]:
                        marks.append('getworld_yes')
                        X.ev(0x7F0910, [X.tag(par)], par)
                    else:
                        marks.append('getworld_no')
            for j in range(4):
                B.wr(a[4] + 0x1C + 4 * j, 4, A.rd(o + 0x1C + 4 * j, 4))
            B.wr(a[4] + 0x48, 4, A.rd(o + 0x48, 4))
            B.wr(a[4] + 0x6C, 4, A.rd(o + 0x6C, 4))
            for j in range(5):
                B.wr(a[4] + 0x4C + 4 * j, 4, A.rd(o + 0x4C + 4 * j, 4))
            X.ev(0x808770, [0x8D624C, X.tag(clone), X.tag(this)], clone)
            X.ret = clone

    # cambios de las regiones globales vigiladas (las pequenas; la grande va como crc)
    gch = []
    for g in G_OF.get(fn, ()):
        if GSIZE[g] > 0x40 or g == 10:
            continue
        before, after = W.Rref[g], bytes(W.R[g])
        for kb in range(GSIZE[g]):
            if before[kb] != after[kb]:
                gch.append([GLOBALS[g] + kb, after[kb]])
    return {'events': X.events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': X.ret if fn in RET_EAX else None,
            'big': big, 'threw': False, 'marks': marks}


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
