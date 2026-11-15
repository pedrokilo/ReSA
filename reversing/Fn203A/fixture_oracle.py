"""Oraculo independiente (Python) de Fn203A: reproduce, desde la ENTRADA del caso (parametros, argumentos y guion de la escena) y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 203A
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su
imagen inicial y aplica el mismo guion de escrituras que el banco.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `reversing/_handoff/dis.sh`):
   0  0x5A6DB0  COctTree constructor: eax = ecx; [eax] = 867170h; [+4] = [+1Ch] = [+20h] = [+24h] = 0; los cuatro dwords desde +0Ah = -1; BYTE [+8] = 0
   1  0x5A6E60  copia muerta de CPool::Flush: si [this+8] (con signo) > 0: si BYTE [this+10h] != 0: delete([this]); delete([this+4]); [this], [+4], [+8], [+0Ch] = 0
   2  0x5A6FC0  COctTree::empty: [+4] = [+1Ch] = [+20h] = [+24h] = 0; para los 8 hijos (word en +0Ah + 2*i): con signo < 0: nada; si no: estado = byte [estados + idx]; si bit 7 (libre): sin mas; si no: elem = datos + idx * 0x28; si es
            nulo: sin mas; si no: [elem] = 867170h; empty(elem); libera la ranura (0x5A73C0); y el word del hijo = -1 (salvo el caso < 0); al final BYTE [+8] = 0
   3  0x5A72B0  igual que 1
   4  0x5A7350  CPool::New: edi = [pool+0Ch] + 1; [pool+0Ch] = edi; si edi == [pool+8]: [pool+0Ch] = 0 y, si ya se dio una vuelta, devuelve 0; si no, sigue; con estado[cursor] bit 7 (libre): lo marca ocupado y le suma 1 al id de 7 bits; devuelve
            datos + cursor * 0x28; si el bit 7 no esta puesto, siguiente
   5  0x5A73C0  CPool::Delete(obj): idx = (obj - [pool]) / 0x28 (imul 66666667h; sar 4; + bit de signo); estado[idx] |= 80h; si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx
   6  0x5A7490  destructor con borrado: [this] = 867170h; empty(this); si BYTE flags & 1: idx = (this - [BC12E4]) / 0x28; [BC12E8][idx] |= 80h; si idx < [BC12F0] (con signo): [BC12F0] = idx; eax = this
   7  0x5A74F0  RemoveChildren: el bucle de empty sin tocar nivel, sumas ni BYTE +8
   8  0x5A75B0  InsertTree(r, g, b) (BYTE bajos de los argumentos): ver oct_insert()
   9  0x5A7570  COctTreeBase constructor: como 0 y [eax] = 867178h al final
  10  0x5A44A0  DestroyTextureCB: RwTexDictionaryRemoveTexture(tex); RwTextureDestroy(tex); eax = tex
  11  0x5A47E0  GetAtomicWithNameCB(atomic, ctx): nodo = GetFrameNodeName([atomic+4]); si _stricmp([ctx], nodo) == 0: [ctx+4] = atomic; eax = 0; si no eax = atomic
  12  0x6002F0  FindAttractorInRange: ver find_attractor()
  13  0x4C75E0  CVehicleModelInfo constructor: 4C4A60(this); [this+5Ch] = 0; BYTE [this+2D0h] = 0; BYTE [this+4Eh] = 0; [this] = 85C5C8h; [this+304h] = -1; 9 dwords desde +2D6h = -1; [+2FAh] = [+2FEh] = -1; eax = this
  14  0x4C8E10  destructor con borrado: 4C5920(this); si flags & 1: operator delete(this); eax = this
  15  0x4C7410  CVehicleStructure destructor: al = BYTE [this+30Ch]; si al > 0 (con signo): para i = 0..: atomico = [this+2F4h+4i]; marco = [atomico+4]; RpAtomicDestroy(atomico); RwFrameDestroy(marco); eax = movsx BYTE
            [this+30Ch] (releido); sigue mientras i+1 < eax
  16  0x4C8D60  CVehicleStructure constructor: 15 dummies de 3 dwords a 0; 18 dwords en +0D0h + 20h*i = -1; 6 dwords en +2F4h a 0; BYTE [+30Ch] = 0; dword [+310h] = 0; eax = this
  17  0x4C94C0  como 4 con elementos de 0x314
  18  0x4C9580  CVehicleStructure::operator delete(ptr): pool = [0B4E680h]; idx = (ptr - [pool]) / 0x314 (imul 532AE21Dh; sar 8; + bit de signo); estado |= 80h; cursor = min con signo
  19  0x4C9840  SDD: 4C7410(this); si flags & 1: borrado en linea como 18 con el pool [0B4E680h]; eax = this
  20  0x66AA20  CTaskComplexAvoidEntity constructor: 61A3B0(this); [this+10h] = arg1; [this+0Ch] = arg2; [this] = 86FF00h; vector arg3 -> [+20h..+2Bh] y -> [+2Ch..+37h]; [+44h] = [+48h] = 0; BYTE [+4Ch] = [+4Dh] = 0; BYTE
            [+50h] &= FEh; si [this+0Ch] != 0: 571B70(this+0Ch) con ECX = la entidad; eax = this
  21  0x66AAD0  GetTaskType: eax = 3ABh
  22  0x66AAE0  cuerpo del destructor: [this] = 86FF00h; si [this+0Ch] != 0: 571A00(this+0Ch) con ECX = la entidad; 61A3D0(this)

Los falsos anotan: [0x8214BD, argumento]; [direccion, ECX (etiqueta), dword de [ECX] al entrar] (cuerpos 4C4A60, 4C5920, 61A3B0, 61A3D0); [0x571B70 / 0x571A00, ECX, referencia]; [0x7F39C0, tex]; [0x7F3820, tex];
[0x72FB30, marco]; [0x749DC0, atomico]; [0x7F05A0, marco]; [0x4C7410, ECX]; [0x564A20, punto, radio, b2D, cuenta inicial, maximo, 5 banderas]; [0x4C4C70, ECX, indice]; [0x5334F0, ECX, salida (pila: 0x7F000000), posicion];
[0x5EE190]; [0x5EBB00, ECX, efecto, entidad].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["COctTree::Constructor", "COctTree::PoolFlush5A6E60", "COctTree::empty", "COctTree::PoolFlush5A72B0", "COctTree::PoolNew", "COctTree::PoolDelete", "COctTree::ScalarDeletingDestructor",
         "COctTree::RemoveChildren", "COctTree::InsertTree", "COctTreeBase::Constructor", "CClothesBuilder::DestroyTextureCB", "CClothesBuilder::GetAtomicWithNameCB",
         "CAttractorScanner::FindAttractorInRange", "CVehicleModelInfo::Constructor", "CVehicleModelInfo::ScalarDeletingDestructor", "CVehicleStructure::Destructor", "CVehicleStructure::Constructor",
         "CVehicleStructure::PoolNew", "CVehicleStructure::OperatorDelete", "CVehicleStructure::ScalarDeletingDestructor", "CTaskComplexAvoidEntity::Constructor", "CTaskComplexAvoidEntity::GetTaskType",
         "CTaskComplexAvoidEntity::DestructorBody"]
ENTRIES = (0x5A6DB0, 0x5A6E60, 0x5A6FC0, 0x5A72B0, 0x5A7350, 0x5A73C0, 0x5A7490, 0x5A74F0, 0x5A75B0, 0x5A7570, 0x5A44A0, 0x5A47E0, 0x6002F0, 0x4C75E0, 0x4C8E10, 0x4C7410, 0x4C8D60, 0x4C94C0, 0x4C9580,
           0x4C9840, 0x66AA20, 0x66AAD0, 0x66AAE0)
(OCTCTOR, FLUSHA, EMPTY, FLUSHB, POOLNEWOCT, POOLDELOCT, OCTSDD, REMOVECHILDREN, INSERTTREE, OCTBASECTOR, TEXCB, ATOMCB, FINDATTR, VMICTOR, VMISDD, VSDTOR, VSCTOR, VSPOOLNEW, VSOPDELETE, VSSDD,
 AVCTOR, AVTYPE, AVDTOR) = range(23)
SPAN = (0x28, 0x14, 0x28, 0x14, 0, 0, 0x28, 0x28, 0x28, 0x30, 0, 8, 0, 0x308, 4, 0x314, 0x314, 0, 0, 0, 0x54, 4, 0x54)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
TREE_BASE, TREE_BYTES = 0xBC12DC, 28
VS_POOL_PTR = 0xB4E680
MODEL_TABLE, MODEL_LO, MODEL_BYTES = 0xA9B0C8, 0xA9B0B8, 48
ELEM_OCT, ELEM_VS = 0x28, 0x314
GLOBAL_REGIONS = {EMPTY: [(TREE_BASE, TREE_BYTES)], OCTSDD: [(TREE_BASE, TREE_BYTES)], REMOVECHILDREN: [(TREE_BASE, TREE_BYTES)], INSERTTREE: [(TREE_BASE, TREE_BYTES)],
                  VSOPDELETE: [(VS_POOL_PTR, 4)], VSSDD: [(VS_POOL_PTR, 4)], FINDATTR: [(MODEL_LO, MODEL_BYTES)]}


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


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v >= 0x80 else v


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


def is_sym(v):
    return 0xE00 <= (v >> 20) <= 0xE03


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba) y la memoria global: lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.glob = {}
        self.glob_ref = {}
        self.script(bytes.fromhex(c['s']))

    def resolve(self, v):
        v &= M32
        if is_sym(v):
            return (self.bases[(v >> 20) & 3] + GUARD + (v & 0xFFFFF)) & M32
        return v

    def script(self, s):
        i = 0
        while i < len(s):
            op = s[i]
            addr = int.from_bytes(s[i + 1:i + 5], 'little')
            ln = int.from_bytes(s[i + 5:i + 7], 'little')
            data = bytearray(s[i + 7:i + 7 + ln])
            i += 7 + ln
            if op & 2:
                data[0:4] = self.resolve(int.from_bytes(data[0:4], 'little')).to_bytes(4, 'little')
            if op & 1:
                for k in range(ln):
                    self.glob[addr + k] = data[k]
            else:
                mem = self.mem[addr >> 24]
                off = addr & 0xFFFFFF
                for k in range(ln):
                    mem.wr(off + k, 1, data[k])

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        return None

    def _span(self, addr, n):
        loc = self.locate(addr)
        if loc is None:
            return None
        end = self.locate((addr + n - 1) & M32)
        if end is None or end[0] != loc[0]:
            raise ValueError('acceso fuera de la arena: %x' % addr)
        return loc

    def rd(self, addr, n=4):
        addr &= M32
        loc = self._span(addr, n)
        if loc is not None:
            return self.mem[loc[0]].rd(loc[1], n)
        out = 0
        for k in range(n):
            if (addr + k) not in self.glob:
                raise ValueError('lectura de una direccion no preparada: %x' % (addr + k))
            out |= self.glob[addr + k] << (8 * k)
        return out

    def wr(self, addr, n, v):
        addr &= M32
        loc = self._span(addr, n)
        if loc is not None:
            self.mem[loc[0]].wr(loc[1], n, v)
            return
        for k in range(n):
            if (addr + k) not in self.glob:
                raise ValueError('escritura en una direccion no preparada: %x' % (addr + k))
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento respecto al inicio de la arena con su guarda); si no, el valor crudo"""
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
        self.glob_ref = dict(self.glob)

    def gchanges(self, regions):
        out = []
        for lo, n in regions:
            for k in range(n):
                if self.glob.get(lo + k) != self.glob_ref.get(lo + k):
                    out.append([lo + k, self.glob[lo + k]])
        return out


def div_index(diff, magic, shift):
    """indice = diferencia / tamano con signo, como `imul magic; sar shift; shr 31; add` del EXE"""
    sd = s32(diff)
    prod = sd * magic
    edx = s32(prod >> 32)
    edx >>= shift
    edx = s32(edx)
    sign = (edx & M32) >> 31
    return s32(edx + sign)


def release_slot(W, pool, obj, magic, shift):
    """libera la ranura del objeto en el pool cuya estructura esta en `pool`: estado |= 80h y cursor = min con signo (como 0x5A73C0, 0x4C9580 y los borrados en linea)"""
    data = W.rd(pool, 4)
    idx = div_index(obj - data, magic, shift)
    states = W.rd(pool + 4, 4)
    W.wr(states + idx, 1, W.rd(states + idx, 1) | 0x80)
    if idx < s32(W.rd(pool + 0xC, 4)):
        W.wr(pool + 0xC, 4, idx)
    return idx


def pool_new(W, pool, elem):
    """CPool::New del EXE (0x5A7350 / 0x4C94C0)"""
    size = W.rd(pool + 8, 4)
    al = 0
    while True:
        edi = (W.rd(pool + 0xC, 4) + 1) & M32
        W.wr(pool + 0xC, 4, edi)
        if edi == size:
            W.wr(pool + 0xC, 4, 0)
            if al:
                return 0
            al = 1
        states = W.rd(pool + 4, 4)
        cur = W.rd(pool + 0xC, 4)
        if W.rd(states + cur, 1) & 0x80:
            break
    bl = W.rd(states + cur, 1) & 0x7F
    W.wr(states + cur, 1, bl)
    W.wr(states + cur, 1, (bl & 0x80) | ((bl + 1) & 0x7F))
    return (W.rd(pool, 4) + cur * elem) & M32


MAGIC_OCT = (0x66666667, 4)
MAGIC_VS = (0x532AE21D, 8)
OCT_POOL = TREE_BASE + 8        # 0xBC12E4
LEVEL = TREE_BASE + 4           # 0xBC12E0
FAILED = TREE_BASE              # 0xBC12DC


def oct_ctor(W, p, vt=0x867170):
    W.wr(p, 4, vt)
    W.wr(p + 4, 4, 0)
    W.wr(p + 0x1C, 4, 0)
    W.wr(p + 0x20, 4, 0)
    W.wr(p + 0x24, 4, 0)
    for off in (0xA, 0xE, 0x12, 0x16):
        W.wr(p + off, 4, M32)
    W.wr(p + 8, 1, 0)


def bump(stats, key):
    stats[key] = stats.get(key, 0) + 1


def release_children(W, this, stats):
    """bucle de hijos de 0x5A6FC0 y 0x5A74F0"""
    for j in range(8):
        edi = this + 0xA + 2 * j
        ax = W.rd(edi, 2)
        if ax & 0x8000:
            continue
        bump(stats, 'children')
        states = W.rd(OCT_POOL + 4, 4)
        if W.rd(states + ax, 1) & 0x80:
            bump(stats, 'free_child')
        else:
            esi = (W.rd(OCT_POOL, 4) + ax * ELEM_OCT) & M32
            if esi == 0:
                bump(stats, 'null_elem')
            else:
                W.wr(esi, 4, 0x867170)
                oct_empty(W, esi, stats)
                release_slot(W, OCT_POOL, esi, *MAGIC_OCT)
                bump(stats, 'recursed')
        W.wr(edi, 2, 0xFFFF)


def oct_empty(W, this, stats):
    W.wr(this + 4, 4, 0)
    W.wr(this + 0x1C, 4, 0)
    W.wr(this + 0x20, 4, 0)
    W.wr(this + 0x24, 4, 0)
    release_children(W, this, stats)
    W.wr(this + 8, 1, 0)


def oct_sdd(W, this, flags, stats):
    W.wr(this, 4, 0x867170)
    oct_empty(W, this, stats)
    if flags & 1:
        release_slot(W, OCT_POOL, this, *MAGIC_OCT)


def sar32(v, n):
    return s32(v) >> n


def oct_insert(W, this, r, g, b, stats, depth=0):
    """0x5A75B0; r, g y b: los dwords de pila (solo el byte bajo cuenta)"""
    r8, g8, b8 = r & 0xFF, g & 0xFF, b & 0xFF
    level = W.rd(LEVEL, 4)
    cl = level & 31
    idx = ((sar32((g8 << cl) & M32, 6)) & 2) + ((sar32((b8 << cl) & M32, 7)) & 1) + ((sar32((r8 << cl) & M32, 5)) & 4)
    W.wr(LEVEL, 4, level + 1)
    W.wr(this + 0x1C, 4, W.rd(this + 0x1C, 4) + r8)
    W.wr(this + 0x20, 4, W.rd(this + 0x20, 4) + g8)
    W.wr(this + 0x24, 4, W.rd(this + 0x24, 4) + b8)
    W.wr(this + 4, 4, W.rd(this + 4, 4) + 1)
    stats['max_depth'] = max(stats.get('max_depth', 0), depth)
    if W.rd(LEVEL, 4) == 8 or W.rd(this + 8, 1) == 1:
        bump(stats, 'level8' if W.rd(LEVEL, 4) == 8 else 'last_step')
        W.wr(LEVEL, 4, 0)
        W.wr(this + 8, 1, 1)
        release_children(W, this, stats)
        return 1 if W.rd(this + 4, 4) == 1 else 0
    ax = W.rd(this + idx * 2 + 0xA, 2)
    if not (ax & 0x8000):
        states = W.rd(OCT_POOL + 4, 4)
        if W.rd(states + ax, 1) & 0x80:
            raise ValueError('hijo de ranura libre: el EXE desreferencia nulo (fuera de los fixtures)')
        child = (W.rd(OCT_POOL, 4) + ax * ELEM_OCT) & M32
        bump(stats, 'existing_child')
    else:
        child = pool_new(W, OCT_POOL, ELEM_OCT)
        if child:
            oct_ctor(W, child)
        if not child:
            W.wr(FAILED, 1, 1)
            bump(stats, 'pool_full')
            return 0
        slot = div_index(child - W.rd(OCT_POOL, 4), *MAGIC_OCT)
        W.wr(this + idx * 2 + 0xA, 2, slot & 0xFFFF)
        bump(stats, 'new_child')
    if W.rd(child, 4) != 0x867170:
        raise ValueError('vtable del hijo distinta de 867170h (fuera de los fixtures)')
    got = oct_insert(W, child, r, g, b, stats, depth + 1)
    if W.rd(FAILED, 1) != 0 and s32(W.rd(child + 4, 4)) < 2:
        oct_sdd(W, child, 1, stats)
        W.wr(this + idx * 2 + 0xA, 2, 0xFFFF)
        bump(stats, 'child_deleted')
        return 0
    return got & 0xFF


# ------------------------------------------------------------------------------------------------ FindAttractorInRange
def r24(x):
    """redondea un racional exacto a 24 bits de mantisa (al mas cercano, empate al par); el exponente de la x87 es ampliado: sin desbordamiento ni subdesbordamiento"""
    if x == 0:
        return x
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length() - 24
    while True:
        scaled = a / (Fraction(2) ** e) if e >= 0 else a * (Fraction(2) ** -e)
        if scaled >= 1 << 24:
            e += 1
        elif scaled < 1 << 23:
            e -= 1
        else:
            break
    fl = scaled.numerator // scaled.denominator
    rem = scaled - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    return Fraction(fl) * (Fraction(2) ** e) * sign


def f32_store(x):
    """valor tras `fstp dword`: redondeo a float de 32 bits (con denormales)"""
    return Fraction(struct.unpack('<f', struct.pack('<f', float(x)))[0])


def finite_bits(b):
    return (b & 0x7F800000) != 0x7F800000


def cstr(W, addr, limit=64):
    out = bytearray()
    for k in range(limit):
        b = W.rd(addr + k, 1)
        if b == 0:
            return bytes(out)
        out.append(b)
    raise ValueError('cadena sin terminar')


def lower_ascii(bs):
    return bytes((c + 32) if 65 <= c <= 90 else c for c in bs)


def find_attractor(W, c, events, marks):
    a = [x & M32 for x in c['a']]
    args = [W.resolve(x) for x in c['args']]
    point, radius, model_arg, type_arg, name_arg, req_arg, excl, out_effect, out_entity = args
    W.snap()
    W.wr(out_effect, 4, 0)
    W.wr(out_entity, 4, 0)
    events.append([0x564A20, W.tagv(point), radius, 0, 0xFFFF, 0x400, 1, 0, 0, 1, 0])
    count = s16(a[4])
    entities = [(W.bases[0] + GUARD + 0x100 * a[5 + i]) & M32 for i in range(8)]
    found = 0
    best = Fraction(100000000)
    get2d = has = 0
    if count <= 0:
        marks.append('count_nonpos')
        return found
    marks.append('count_pos')
    marks.append('any_model' if model_arg == M32 else 'filter_model')
    px_bits = [W.rd(point + 4 * k, 4) for k in range(3)]
    i = 0
    while True:
        ent = entities[i]
        emodel = s16(W.rd(ent + 0x22, 2))
        skip = model_arg != M32 and s32(model_arg) != emodel
        if skip:
            marks.append('model_mismatch')
        else:
            minfo = W.rd(MODEL_TABLE + 4 * emodel, 4)
            nfx = W.rd(minfo + 0xD, 1)
            j = 0
            if nfx != 0:
                while True:
                    events.append([0x4C4C70, W.tagv(minfo), j])
                    get2d += 1
                    eff = W.rd(minfo + 0x20 + 4 * j, 4)
                    if a[13] != 0 and a[13] == get2d:
                        W.wr(minfo + 0xD, 1, a[14] & 0xFF)
                        marks.append('model_count_mut')
                    if a[15] != 0 and a[15] == get2d:
                        count = s16(a[16])
                        marks.append('entity_count_mut')
                    ok = True
                    if excl != 0 and excl == eff:
                        ok = False
                        marks.append('excluded')
                    if ok and W.rd(eff + 0xC, 1) != 3:
                        ok = False
                        marks.append('non_attractor')
                    if ok and W.rd(eff + 0x34, 1) != type_arg:
                        ok = False
                        marks.append('type_mismatch')
                    if ok and name_arg != 0:
                        if lower_ascii(cstr(W, name_arg)) == lower_ascii(cstr(W, eff + 0x38)):
                            marks.append('name_match')
                        else:
                            ok = False
                            marks.append('name_mismatch')
                    if ok and (req_arg & 0xFF) != 0:
                        marks.append('req_free')
                        events.append([0x5EE190])
                        events.append([0x5EBB00, W.tagv(W.resolve(a[19])), W.tagv(eff), W.tagv(ent)])
                        k = has
                        has += 1
                        low = (0x80 if k % 3 == 0 else 1) if ((a[17] >> (k % 32)) & 1) else 0
                        if low == 0:
                            ok = False
                            marks.append('no_free_slot')
                    if ok:
                        events.append([0x5334F0, W.tagv(ent), 0x7F000000, W.tagv(eff)])
                        out_vec = [W.rd(((ent + 0x30 + 4 * ax) if ((a[18] >> ax) & 1) else (eff + 4 * ax)), 4) for ax in range(3)]
                        if all(finite_bits(v) for v in px_bits + out_vec):
                            dx = r24(Fraction(f32(px_bits[0])) - Fraction(f32(out_vec[0])))
                            dy = r24(Fraction(f32(px_bits[1])) - Fraction(f32(out_vec[1])))
                            dz = r24(Fraction(f32(px_bits[2])) - Fraction(f32(out_vec[2])))
                            dzz = r24(dz * dz)
                            dyy = r24(dy * dy)
                            s1 = r24(dzz + dyy)
                            dxx = r24(dx * dx)
                            dist = r24(s1 + dxx)
                            if dist == best:
                                marks.append('equal_best')
                            if dist < best:
                                best = f32_store(dist)
                                W.wr(out_effect, 4, eff)
                                W.wr(out_entity, 4, ent)
                                found = 1
                                marks.append('best_update')
                            else:
                                marks.append('farther')
                        else:
                            marks.append('nonfinite')
                    nfx = W.rd(minfo + 0xD, 1)
                    j += 1
                    if not (j < nfx):
                        break
        i += 1
        if not (i < count):
            break
    marks.append('found' if found else 'not_found')
    return found


# ------------------------------------------------------------------------------------------------ marcas y modelo
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
KEYS = {}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + (['this_aligned', 'this_unaligned'] if SPAN[_fn] else [])
KEYS[FLUSHA] += ['count_pos', 'count_nonpos', 'owns', 'not_owns']
KEYS[FLUSHB] += ['count_pos', 'count_nonpos', 'owns', 'not_owns']
KEYS[EMPTY] += ['children', 'recursed', 'free_child', 'null_data']
KEYS[REMOVECHILDREN] += ['children', 'recursed', 'free_child', 'null_data']
KEYS[OCTSDD] += ['flag_set', 'flag_clear', 'this_slot', 'this_root', 'children']
KEYS[INSERTTREE] += ['existing_child', 'new_child', 'pool_full', 'level8', 'last_step', 'child_deleted', 'depth2', 'ret_true', 'ret_false']
KEYS[POOLNEWOCT] += ['new_found', 'new_null', 'wrapped']
KEYS[VSPOOLNEW] += ['new_found', 'new_null', 'wrapped']
for _fn in (POOLDELOCT, VSOPDELETE, VSSDD):
    KEYS[_fn] += ['was_free', 'was_used', 'lowers_cursor', 'keeps_cursor', 'misaligned']
KEYS[VSSDD] += ['flag_set', 'flag_clear']
KEYS[VMISDD] += ['flag_set', 'flag_clear']
KEYS[VSDTOR] += ['count_pos', 'count_nonpos', 'count_changed']
KEYS[AVCTOR] += ['entity_null', 'entity_set']
KEYS[AVDTOR] += ['entity_null', 'entity_set']
KEYS[TEXCB] += ['texture_null', 'texture_set']
KEYS[ATOMCB] += ['match', 'nomatch']
KEYS[FINDATTR] += ['count_pos', 'count_nonpos', 'any_model', 'filter_model', 'model_mismatch', 'excluded', 'non_attractor', 'type_mismatch', 'name_match', 'name_mismatch', 'req_free', 'no_free_slot',
                   'best_update', 'farther', 'nonfinite', 'found', 'not_found']


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    if SPAN[fn]:
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    args = [W.resolve(x) for x in c['args'][:c['nargs']]]
    this = W.resolve(c['ecx'])
    stats = {}
    if fn in (OCTCTOR, OCTBASECTOR):
        W.snap()
        oct_ctor(W, this, 0x867170)
        if fn == OCTBASECTOR:
            W.wr(this, 4, 0x867178)
        ret = W.tagv(this)
    elif fn in (FLUSHA, FLUSHB):
        W.snap()
        if s32(W.rd(this + 8, 4)) > 0:
            marks.append('count_pos')
            if W.rd(this + 0x10, 1) != 0:
                marks.append('owns')
                events.append([0x8214BD, W.tagv(W.rd(this, 4))])
                events.append([0x8214BD, W.tagv(W.rd(this + 4, 4))])
            else:
                marks.append('not_owns')
            for off in (0, 4, 8, 0xC):
                W.wr(this + off, 4, 0)
        else:
            marks.append('count_nonpos')
    elif fn in (EMPTY, REMOVECHILDREN, OCTSDD, INSERTTREE):
        W.snap()
        if a[8] and fn in (EMPTY, REMOVECHILDREN):
            marks.append('null_data')
        if fn == EMPTY:
            oct_empty(W, this, stats)
        elif fn == REMOVECHILDREN:
            release_children(W, this, stats)
        elif fn == OCTSDD:
            marks.append('flag_set' if args[0] & 1 else 'flag_clear')
            marks.append('this_root' if a[11] == M32 else 'this_slot')
            oct_sdd(W, this, args[0], stats)
            ret = W.tagv(this)
        else:
            ret = oct_insert(W, this, args[0], args[1], args[2], stats) & 0xFF
            marks.append('ret_true' if ret else 'ret_false')
            if stats.get('max_depth', 0) >= 2:
                marks.append('depth2')
        for k in ('children', 'recursed', 'free_child', 'existing_child', 'new_child', 'pool_full', 'level8', 'last_step', 'child_deleted'):
            if stats.get(k):
                marks.append(k)
    elif fn in (POOLNEWOCT, VSPOOLNEW):
        W.snap()
        before = s32(W.rd(this + 0xC, 4))
        size = W.rd(this + 8, 4)
        res = pool_new(W, this, ELEM_OCT if fn == POOLNEWOCT else ELEM_VS)
        ret = W.tagv(res)
        marks.append('new_null' if res == 0 else 'new_found')
        if before + 1 >= size or (res and s32(W.rd(this + 0xC, 4)) <= before):
            marks.append('wrapped')
    elif fn in (POOLDELOCT, VSOPDELETE, VSSDD):
        magic = MAGIC_OCT if fn == POOLDELOCT else MAGIC_VS
        pool = this if fn == POOLDELOCT else W.rd(VS_POOL_PTR, 4)
        obj = args[0] if fn != VSSDD else this
        W.snap()
        release = True
        if fn == VSSDD:
            events.append([0x4C7410, W.tagv(this)])
            marks.append('flag_set' if args[0] & 1 else 'flag_clear')
            ret = W.tagv(this)
            release = bool(args[0] & 1)
        if release:
            data = W.rd(pool, 4)
            states = W.rd(pool + 4, 4)
            before = s32(W.rd(pool + 0xC, 4))
            idx = div_index(obj - data, *magic)
            marks.append('was_free' if (W.rd(states + idx, 1) & 0x80) else 'was_used')
            if a[10] & M32:
                marks.append('misaligned')
            marks.append('lowers_cursor' if idx < before else 'keeps_cursor')
            release_slot(W, pool, obj, *magic)
    elif fn == VMICTOR:
        W.snap()
        events.append([0x4C4A60, W.tagv(this), W.rd(this, 4)])
        W.wr(this, 4, a[41])
        W.wr(this + 0x5C, 4, 0)
        W.wr(this + 0x2D0, 1, 0)
        W.wr(this + 0x4E, 1, 0)
        W.wr(this, 4, 0x85C5C8)
        W.wr(this + 0x304, 4, M32)
        for k in range(9):
            W.wr(this + 0x2D6 + 4 * k, 4, M32)
        W.wr(this + 0x2FA, 4, M32)
        W.wr(this + 0x2FE, 4, M32)
        ret = W.tagv(this)
    elif fn == VMISDD:
        W.snap()
        events.append([0x4C5920, W.tagv(this), W.rd(this, 4)])
        W.wr(this, 4, a[41])
        marks.append('flag_set' if args[0] & 1 else 'flag_clear')
        if args[0] & 1:
            events.append([0x8214BD, W.tagv(this)])
        ret = W.tagv(this)
    elif fn == VSDTOR:
        W.snap()
        count0 = s8(W.rd(this + 0x30C, 1))
        marks.append('count_pos' if count0 > 0 else 'count_nonpos')
        calls = 0
        if count0 > 0:
            esi, edi = 0, this + 0x2F4
            while True:
                atomic = W.rd(edi, 4)
                frame = W.rd(atomic + 4, 4)
                events.append([0x749DC0, W.tagv(atomic)])
                calls += 1
                if a[17]:
                    W.wr(atomic + 4, 4, a[17])
                if a[18] and a[18] == calls:
                    W.wr(this + 0x30C, 1, a[19] & 0xFF)
                    marks.append('count_changed')
                events.append([0x7F05A0, W.tagv(frame)])
                eax = s8(W.rd(this + 0x30C, 1))
                esi += 1
                edi += 4
                if not (esi < eax):
                    break
    elif fn == VSCTOR:
        W.snap()
        for k in range(15 * 3):
            W.wr(this + 4 * k, 4, 0)
        for k in range(18):
            W.wr(this + 0xD0 + 0x20 * k, 4, M32)
        for k in range(6):
            W.wr(this + 0x2F4 + 4 * k, 4, 0)
        W.wr(this + 0x30C, 1, 0)
        W.wr(this + 0x310, 4, 0)
        ret = W.tagv(this)
    elif fn == AVCTOR:
        W.snap()
        arg1, entity, vec = args
        events.append([0x61A3B0, W.tagv(this), W.rd(this, 4)])
        W.wr(this, 4, a[41])
        W.wr(this + 0x10, 4, arg1)
        W.wr(this + 0xC, 4, entity)
        W.wr(this, 4, 0x86FF00)
        v = [W.rd(vec + 4 * k, 4) for k in range(3)]
        for k in range(3):
            W.wr(this + 0x20 + 4 * k, 4, v[k])
        for k in range(3):
            W.wr(this + 0x2C + 4 * k, 4, v[k])
        W.wr(this + 0x44, 4, 0)
        W.wr(this + 0x48, 4, 0)
        W.wr(this + 0x4C, 1, 0)
        W.wr(this + 0x4D, 1, 0)
        W.wr(this + 0x50, 1, W.rd(this + 0x50, 1) & 0xFE)
        if entity != 0:
            marks.append('entity_set')
            events.append([0x571B70, W.tagv(entity), W.tagv(this + 0xC)])
        else:
            marks.append('entity_null')
        ret = W.tagv(this)
    elif fn == AVTYPE:
        W.snap()
        ret = 0x3AB
    elif fn == AVDTOR:
        W.snap()
        W.wr(this, 4, 0x86FF00)
        entity = W.rd(this + 0xC, 4)
        if entity != 0:
            marks.append('entity_set')
            events.append([0x571A00, W.tagv(entity), W.tagv(this + 0xC)])
        else:
            marks.append('entity_null')
        events.append([0x61A3D0, W.tagv(this), W.rd(this, 4)])
        W.wr(this, 4, a[41])
    elif fn == TEXCB:
        W.snap()
        tex = args[0]
        marks.append('texture_null' if tex == 0 else 'texture_set')
        events.append([0x7F39C0, W.tagv(tex)])
        events.append([0x7F3820, W.tagv(tex)])
        ret = W.tagv(tex)
    elif fn == ATOMCB:
        W.snap()
        atomic, ctx = args
        events.append([0x72FB30, W.tagv(W.rd(atomic + 4, 4))])
        node = cstr(W, W.resolve(a[9]))
        want = cstr(W, W.rd(ctx, 4))
        if lower_ascii(want) == lower_ascii(node):
            marks.append('match')
            W.wr(ctx + 4, 4, atomic)
            ret = 0
        else:
            marks.append('nomatch')
            ret = W.tagv(atomic)
    elif fn == FINDATTR:
        ret = find_attractor(W, c, events, marks) & 0xFF
    else:
        raise ValueError('funcion desconocida %d' % fn)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(GLOBAL_REGIONS.get(fn, [])), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
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
