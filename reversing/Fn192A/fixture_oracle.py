"""Oraculo independiente (Python) de Fn192A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 33 funciones del lote 192A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x67CBE0  CTaskComplexFallAndStayDown::CreateNextSubTask(ped): ecx = [this+8]; eax = [ecx]; call [eax+10h] (GetTaskType); si eax == 0CFh: push 516h; ecx = this; call 678BD0; ret 4 con su eax; si no eax = 0
   1  0x67CC10  CTaskComplexInAirAndLand destructor con borrado: push esi; esi = ecx; call 678CC0; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; eax = esi; ret 4
   2  0x552830  CQuadTreeNode constructor(rect*, nivel): [this+0..0Ch] = los cuatro dwords de *rect; [this+10h] = 0; [this+24h] = nivel; [this+14h..20h] = 0; eax = this; ret 8
   3  0x552AF0  PoolConstructor(n, nombre): [this] = operator new (82119A)(n*40); [this+4] = operator new(n); [this+10h] (byte) = 1; [this+8] = n; [this+0Ch] = -1; si n > 0: n bytes de [this+4] a 80h; eax = this; ret 8
   4  0x552B50  PoolNew: edx = [this+8] (n); al = 0; bucle: edi = [this+0Ch] + 1; [this+0Ch] = edi; si edi == n: [this+0Ch] = 0; si al: devuelve 0; al = 1; si el byte de estados [this+4][[this+0Ch]] tiene el bit 7 a 0: sigue el bucle;
            si no: ese byte &= 7Fh y su contador de 7 bits + 1 (bit 7 a 0); devuelve [this] + [this+0Ch] * 40
   5  0x552BC0  PoolDelete(nodo): idx = (nodo - [this]) / 40 con signo (hacia cero); [[this+4] + idx] |= 80h; si idx < [this+0Ch] (con signo): [this+0Ch] = idx; ret 4
   6  0x552C80  AllocateStorage: ecx = [0B745BCh]; jmp 552B50
   7  0x552C90  DeleteStorage(nodo): pool = [0B745BCh]; como 5 con ese pool; ret (cdecl)
   8  0x552EE0  CRect::GetRandomPoint(out): w = _ftol2(|[+8] - [+0]|) (fsub; fcom 0; fchs solo si es menor que cero; los 32 bajos); h = |[+0Ch] - [+4]| como float; r1 = rand(); out.x = float(r1 % w + [+0]) (fild + fadd);
            r2 = rand(); out.y = float(r2 % _ftol2(h) + [+4]); ret 4
   9  0x553010  CBox::GetRandomPoint(out): wx = _ftol2(|[+0Ch] - [+0]|), wy = _ftol2(|[+10h] - [+4]|), hz = |[+14h] - [+8]| (float); rand() x, y, z: out = (float(r % wx + [+0]), float(r % wy + [+4]), float(r % _ftol2(hz) + [+8])); ret 4
  10  0x5531C0  CRenderer::Init: ret
  11  0x5531D0  CRenderer::Shutdown: ret
  12  0x59F610  CreateAtRef(ref): idx = ref >> 8 (aritmetico); [[this+4]+idx] &= 7Fh; byte = byte ^ ((byte ^ ref) & 7Fh); [this+0Ch] = 0; mientras el byte [this+4][[this+0Ch]] tenga el bit 7 a 0: [this+0Ch]++; ret 4
  13  0x59F660  CObject cuerpo del destructor: [this] = 866F60h; si [this+140h] & 300000h: SetMissionDoesntRequireModel (409C90)(movsx word [0A90CF0h + movsx word [this+168h] * 20] + 6676h); [this+140h] &= FFCFFFFFh;
            46ABC0(this, word [this+168h]); [this+168h] = FFFFh; si byte [this+142h] & 1: 4107D0(byte [[[0A9B0C8h + movsx word [this+22h] * 4] + 14h] + 28h]); ClearBlipForEntity 587C60(3,
            (idx << 8) + byte de estados) con idx = (this - [pool]) / 19Ch del pool [0B7449Ch]; si word [this+14Ah] != FFFFh: ecx = [0A9B0C8h + movsx * 4]; call 4C4BB0; si word [this+16Ah] != FFFFh: 731A30(movsx);
            si byte [this+13Ch] == 3 y word [0BB4A70h] > 0: dec word; ecx = this; call 59F450; si movsx word [this+22h] == movzx word [8CD6B8h]: p = [this+170h]; pos = [p+14h] ? [p+14h]+30h : p+4;
            44D960 (ecx = 96F050h) (x - 12, x + 12, y - 12, y + 12, 0) con fld/fadd/fsub x87 a 24 bits; si [this+164h]: ecx = fuego; call 5393F0; call 542450 (~CPhysical; cadena de destructores base)
  14  0x5A2BD0  CObject destructor con borrado: push esi; push edi; edi = ecx; call 59F660; test byte [esp+0Ch],1; je +; liberacion en linea del pool [0B7449Ch]: idx = (this - [pool]) / 19Ch con signo; [[pool+4]+idx] |= 80h;
            si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx; eax = edi; ret 4
  15  0x5A2CB0  SetCentreOfMass(x, y, z): copia los tres dwords de la pila a [this+0A4h..0ACh]; ret 0Ch
  16  0x5D28B0  CPedSaveStructure destructor: iterador de destructores del CRT (821E02) sobre 13 elementos de 1Ch con un destructor que es solo `ret`: sin efecto
  17  0x5D28D0  CPlayerPedDataSaveStructure constructor: push esi; esi = ecx; ecx = esi + 8; call 5A8020 (CPedClothesDesc: 0x78 bytes a cero); eax = esi; ret
  18  0x5D28F0  CVehicleSaveStructure destructor: jmp 59ACD0
  19  0x4B0A40  CEventScriptCommand::GetLifeTime: xor eax,eax; ret
  20-29 0x4B64D0..0x4B6840  destructores con borrado de eventos: push esi; call CUERPO; test byte [esp+8],1; je +; esi = [0B744ACh]; ebx = [esi]; edx = ecx - ebx; idx = edx / 44h con signo (hacia cero);
            [[esi+4]+idx] |= 80h; si idx < [esi+0Ch] (con signo): [esi+0Ch] = idx; eax = ecx; ret 4 (el cuerpo conserva ECX)
  30  0x625B60  CTaskSimpleThrowControl destructor con borrado: push esi; esi = ecx; call 61F950; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; eax = esi; ret 4
  31  0x625B80  CTaskSimpleBeKickedOnGround destructor con borrado: igual con el cuerpo 61FC50
  32  0x61FC50  CTaskSimpleBeKickedOnGround cuerpo del destructor: [this] = 86D7D8h; si [this+0Ch]: [anim+1Ch] = C0800000h; 4CEBE0 (ecx = anim; push 0; push 4D1390h); ecx = this; call 61A3A0

Los falsos anotan: cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (y dejan el dword del caso en [ECX]); operator delete de tareas [direccion, puntero (etiqueta), dword del puntero];
CleanUpOldReference [direccion, ECX, argumento]; funciones cdecl [direccion, argumentos]; RemoveFromWaitingForScriptBrainArray [direccion, entidad (etiqueta), 16 bits bajos]; SetLinksBridgeLights [direccion, ECX crudo, bits de
cuatro floats, byte del bool]; SetFinishCallback [direccion, ECX, 1 si la funcion es la de por defecto, datos]; operator new [direccion, tamano]; rand [direccion]; GetTaskType de la subtarea [0x67CBE4, ECX].
Los destructores de CTaskSimpleThrowControl (30) y CObject (13) corren reales (el EXE entra por el hook, la DLL por la llamada de la clase): la vtable de la DLL que dejan se anota como constante.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = [
    "CTaskComplexFallAndStayDown::CreateNextSubTask",
    "CTaskComplexInAirAndLand::ScalarDeletingDestructor",
    "CQuadTreeNode::Constructor",
    "CQuadTreeNode::PoolConstructor",
    "CQuadTreeNode::PoolNew",
    "CQuadTreeNode::PoolDelete",
    "CQuadTreeNode::AllocateStorage",
    "CQuadTreeNode::DeleteStorage",
    "CRect::GetRandomPoint",
    "CBox::GetRandomPoint",
    "CRenderer::Init",
    "CRenderer::Shutdown",
    "CObjectPool::CreateAtRef",
    "CObject::DestructorBody",
    "CObject::ScalarDeletingDestructor",
    "CPhysical::SetCentreOfMass",
    "CPedSaveStructure::Destructor",
    "CPlayerPedDataSaveStructure::Constructor",
    "CVehicleSaveStructure::Destructor",
    "CEventScriptCommand::GetLifeTime",
    "CEventInAir::ScalarDeletingDestructor",
    "CEventVehicleDied::ScalarDeletingDestructor",
    "CEventVehicleOnFire::ScalarDeletingDestructor",
    "CEventHitByWaterCannon::ScalarDeletingDestructor",
    "CEventStuckInAir::ScalarDeletingDestructor",
    "CEventCopCarBeingStolen::ScalarDeletingDestructor",
    "CEventVehicleDamage::ScalarDeletingDestructor",
    "CEventGotKnockedOverByCar::ScalarDeletingDestructor",
    "CEventCarUpsideDown::ScalarDeletingDestructor",
    "CEventPotentialWalkIntoFire::ScalarDeletingDestructor",
    "CTaskSimpleThrowControl::ScalarDeletingDestructor",
    "CTaskSimpleBeKickedOnGround::ScalarDeletingDestructor",
    "CTaskSimpleBeKickedOnGround::DestructorBody",
]
ENTRIES = (0x67CBE0, 0x67CC10, 0x552830, 0x552AF0, 0x552B50, 0x552BC0, 0x552C80, 0x552C90, 0x552EE0, 0x553010, 0x5531C0, 0x5531D0, 0x59F610, 0x59F660, 0x5A2BD0, 0x5A2CB0,
           0x5D28B0, 0x5D28D0, 0x5D28F0, 0x4B0A40, 0x4B64D0, 0x4B6520, 0x4B6570, 0x4B65C0, 0x4B6660, 0x4B66B0, 0x4B6700, 0x4B67A0, 0x4B67F0, 0x4B6840, 0x625B60, 0x625B80, 0x61FC50)
EVENT_BODIES = (0x4B0D00, 0x4B0DA0, 0x4B1170, 0x4B1320, 0x4B1520, 0x4B17F0, 0x4B1990, 0x4B1C10, 0x4B1D50, 0x4B1F00)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SEED_AT = 60
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(FALL, SDDTASK, SDDTHROW, QTCTOR, POOLCTOR, POOLNEW, ALLOC, POOLDEL, RANDRECT, RANDBOX, NOP, NOWRITE, CREATEREF, OBJDTOR, OBJSDD, CENTRE, PEDCTOR, MATDTOR, CONST, EVSDD, KICKED) = \
    'fall sddtask sddthrow qtctor poolctor poolnew alloc pooldel randrect randbox nop nowrite createref objdtor objsdd centre pedctor matdtor const evsdd kicked'.split()
KIND = [FALL, SDDTASK, QTCTOR, POOLCTOR, POOLNEW, POOLDEL, ALLOC, POOLDEL, RANDRECT, RANDBOX, NOP, NOP, CREATEREF, OBJDTOR, OBJSDD, CENTRE, NOWRITE, PEDCTOR, MATDTOR, CONST] + [EVSDD] * 10 + [SDDTHROW, SDDTASK, KICKED]
assert len(NAMES) == len(ENTRIES) == len(KIND) == 33
TASK_BODY = {1: 0x678CC0, 31: 0x61FC50}
OBJ_SIZE, EVENT_SIZE, NODE_SIZE = 0x19C, 0x44, 40
MARKER, QT_POOL, OBJ_POOL, EV_POOL, TEMP_OBJECTS, TRAIN_CROSSING = 0xA9B0B0, 0xB745BC, 0xB7449C, 0xB744AC, 0xBB4A70, 0x8CD6B8
VT_DLL = 0xD11D11D1
BRIDGE_THIS = 0x96F050


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


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def round_f32(q):
    """Fraction -> bits de un float32, redondeo al mas cercano con empate al par (un solo redondeo del exacto, como la x87 con precision de 24 bits); desbordamiento a infinito"""
    if q == 0:
        return 0
    sign = 0x80000000 if q < 0 else 0
    q = abs(q)
    e = q.numerator.bit_length() - q.denominator.bit_length()
    two_e = Fraction(1 << e) if e >= 0 else Fraction(1, 1 << -e)
    if two_e > q:
        e -= 1
    if e < -126:
        e = -126
    unit = Fraction(1 << (e - 23)) if e - 23 >= 0 else Fraction(1, 1 << (23 - e))
    m = q / unit
    mi = m.numerator // m.denominator
    rem = m - mi
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and mi & 1):
        mi += 1
    if mi >= 1 << 24:
        mi >>= 1
        e += 1
    if e > 127:
        return sign | 0x7F800000
    if mi < 1 << 23:
        return sign | mi
    return sign | ((e + 127) << 23) | (mi - (1 << 23))


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def is_inf(bits):
    return (bits & 0x7FFFFFFF) == 0x7F800000


def fadd(bits, other):
    """float(bits) + other (otro: Fraction exacta o dato de la x87) con redondeo a 24 bits; NaN se silencia y conserva la carga; infinito se conserva"""
    if is_nan(bits):
        return bits | 0x400000
    if is_inf(bits):
        return bits
    return round_f32(Fraction(f32(bits)) + other)


def fsub_bits(a_bits, b_bits):
    """float(a) - float(b), exacto y redondeado a 24 bits (las entradas del banco son finitas)"""
    return round_f32(Fraction(f32(a_bits)) - Fraction(f32(b_bits)))


def width32(hi_bits, lo_bits):
    """_ftol2(|hi - lo|) (los 32 bajos, con signo) con el valor absoluto del EXE: se niega solo si es menor que cero"""
    d = Fraction(f32(fsub_bits(hi_bits, lo_bits)))
    if d < 0:
        d = -d
    t = int(d)                       # trunca hacia cero
    return s32(t & M32), d


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[SEED_AT + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
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

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == FALL:
        _k += ['type_cf', 'type_other', 'ret_zero', 'ret_nonzero']
    elif _kind in (SDDTASK, SDDTHROW, EVSDD, OBJSDD):
        _k += ['flag_bit0', 'flag_clear']
        if _kind == SDDTHROW:
            _k += ['entity_null', 'entity_arena', 'entity_raw']
        if _kind in (EVSDD, OBJSDD):
            _k += ['lowers_cursor', 'keeps_cursor', 'first_slot', 'last_slot']
        if _kind == EVSDD:
            _k += ['extra_bytes']
    elif _kind == QTCTOR:
        _k += ['nan_in', 'level_negative', 'level_zero']
    elif _kind == POOLCTOR:
        _k += ['size_pos', 'size_zero', 'size_neg']
    elif _kind == POOLNEW:
        _k += ['found', 'full_null', 'wrapped', 'cursor_minus1', 'cursor_last']
    elif _kind == ALLOC:
        _k += ['alloc_null', 'alloc_set']
    elif _kind == POOLDEL:
        _k += ['was_free', 'was_used', 'lowers_cursor', 'keeps_cursor', 'first_slot', 'extra_bytes']
    elif _kind in (RANDRECT, RANDBOX):
        _k += ['swapped', 'ordered', 'big_width', 'rand_zero', 'rand_max']
    elif _kind == CREATEREF:
        _k += ['slot_zero', 'cursor_moved', 'cursor_zero', 'counter_wrap', 'sentinel']
    elif _kind == OBJDTOR:
        _k += ['script', 'no_script', 'no_model', 'with_model', 'ref_model', 'txd', 'temp_dec', 'temp_zero', 'temp_other', 'train', 'no_train', 'fire', 'no_fire', 'matrix_pos', 'dummy_pos']
    elif _kind == CENTRE:
        _k += ['nan_arg', 'zero_arg']
    elif _kind == KICKED:
        _k += ['anim', 'no_anim']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    states = bytes.fromhex(c.get('s') or '')
    if k != OBJDTOR:
        A.wr(o, 4, a[3])
    if k == FALL:
        sub, vt = W.raw(1, a[5]), W.raw(2, a[6])
        B.wr(a[5], 4, vt)
        A.wr(o + 8, 4, sub)
        W.snap()
        events.append([0x67CBE4, W.tagv(sub)])
        if a[4] == 0xCF:
            events.append([0x678BD0, W.tagv(this), 0x516])
            ret = W.tagv(a[7])
            marks.append('type_cf')
        else:
            ret = 0
            marks.append('type_other')
        marks.append('ret_zero' if ret == 0 else 'ret_nonzero')
    elif k == SDDTASK:
        W.snap()
        body = TASK_BODY[fn]
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6]
        if flags & 1:
            events.append([0x61A5B0, W.tagv(this), a[5]])
        marks.append('flag_bit0' if flags & 1 else 'flag_clear')
        ret = W.tagv(this)
    elif k == SDDTHROW:
        entity = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[7])
        A.wr(o + 0xC, 4, entity)
        W.snap()
        if entity:
            events.append([0x571A00, W.tagv(entity), W.tagv(this + 0xC)])
        A.wr(o, 4, VT_DLL)
        if a[6] & 1:
            events.append([0x61A5B0, W.tagv(this), VT_DLL])
        marks += ['flag_bit0' if a[6] & 1 else 'flag_clear', 'entity_null' if a[4] == 0 else ('entity_arena' if a[4] == 1 else 'entity_raw')]
        ret = W.tagv(this)
    elif k == QTCTOR:
        for i in range(4):
            B.wr(a[4] + 4 * i, 4, a[5 + i])
        W.snap()
        for i in range(4):
            A.wr(o + 4 * i, 4, a[5 + i])
        A.wr(o + 0x10, 4, 0)
        A.wr(o + 0x24, 4, a[9])
        for off in (0x14, 0x18, 0x1C, 0x20):
            A.wr(o + off, 4, 0)
        if any(((x >> 23) & 0xFF) == 0xFF and (x & 0x7FFFFF) for x in a[5:9]):
            marks.append('nan_in')
        marks.append('level_negative' if a[9] >= 1 << 31 else ('level_zero' if a[9] == 0 else 'level_pos'))
        ret = W.tagv(this)
    elif k == POOLCTOR:
        size = s32(a[4])
        alloc0 = W.raw(2, a[6]) if size > 0 else a[9]
        alloc1 = W.raw(3, a[7]) if size > 0 else a[10]
        W.snap()
        events.append([0x82119A, (size * 40) & M32])
        events.append([0x82119A, size & M32])
        A.wr(o + 0, 4, alloc0)
        A.wr(o + 4, 4, alloc1)
        A.wr(o + 0x10, 1, 1)
        A.wr(o + 8, 4, size & M32)
        A.wr(o + 0xC, 4, M32)
        for i in range(size if size > 0 else 0):
            D.wr(a[7] + i, 1, 0x80)
        marks.append('size_pos' if size > 0 else ('size_zero' if size == 0 else 'size_neg'))
        ret = W.tagv(this)
    elif k == POOLNEW:
        storage = W.raw(2, a[4])
        A.wr(o + 0, 4, storage); A.wr(o + 4, 4, W.raw(3, a[5])); A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7])
        for i, b in enumerate(states):
            D.wr(a[5] + i, 1, b)
        W.snap()
        cap = a[6]
        cursor = s32(a[7])
        al = 0
        wrapped = False
        result = None
        while True:
            cursor = s32(cursor + 1)
            if cursor == cap:
                cursor = 0
                if al:
                    result = 0
                    break
                al = 1
                wrapped = True
            if D.rd(a[5] + cursor, 1) & 0x80:
                break
        if result is None:
            st = D.rd(a[5] + cursor, 1) & 0x7F
            D.wr(a[5] + cursor, 1, (st + 1) & 0x7F)
            result = (storage + cursor * 40) & M32
            marks.append('found')
        else:
            marks.append('full_null')
        A.wr(o + 0xC, 4, cursor & M32)
        if wrapped:
            marks.append('wrapped')
        if s32(a[7]) == -1:
            marks.append('cursor_minus1')
        if s32(a[7]) == cap - 1:
            marks.append('cursor_last')
        ret = W.tagv(result)
    elif k == ALLOC:
        pool = W.raw(1, a[4])
        W.setg(QT_POOL, 4, pool)
        gorder = [(QT_POOL, 4)]
        W.snap()
        events.append([0x552B50, W.tagv(pool)])
        ret = W.tagv(a[5])
        marks.append('alloc_null' if a[5] == 0 else 'alloc_set')
    elif k == POOLDEL:
        cap, idx = a[6], a[8]
        storage = W.raw(2, a[4])
        A.wr(o + 0, 4, storage); A.wr(o + 4, 4, W.raw(3, a[5])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[7])
        st = D.rd(a[5] + idx, 1)
        D.wr(a[5] + idx, 1, (st & 0x7F) | (0x80 if a[10] else 0))
        node = (storage + idx * NODE_SIZE + a[9]) & M32
        if fn == 7:
            W.setg(QT_POOL, 4, this)
            gorder = [(QT_POOL, 4)]
        W.snap()
        diff = s32(node - storage)
        index = abs(diff) // NODE_SIZE * (1 if diff >= 0 else -1)
        D.wr(a[5] + index, 1, D.rd(a[5] + index, 1) | 0x80)
        cursor = s32(A.rd(o + 0xC, 4))
        if index < cursor:
            A.wr(o + 0xC, 4, index & M32)
            marks.append('lowers_cursor')
        else:
            marks.append('keeps_cursor')
        marks.append('was_free' if a[10] else 'was_used')
        if idx == 0:
            marks.append('first_slot')
        if a[9]:
            marks.append('extra_bytes')
    elif k in (RANDRECT, RANDBOX):
        count = 2 if k == RANDRECT else 3
        w = a[4:4 + 2 * count]
        out_off = a[10] if k == RANDRECT else a[13]
        rands = a[8:8 + count] if k == RANDRECT else a[10:10 + count]
        for i, v in enumerate(w):
            A.wr(o + 4 * i, 4, v)
        W.snap()
        for _ in range(count):
            events.append([0x821B1E])
        big = False
        swapped = False
        for i in range(count):
            lo, hi = w[i], w[i + count]
            wd, absd = width32(hi, lo)
            big |= absd >= 1 << 31
            swapped |= f32(fsub_bits(hi, lo)) < 0
            rem = rands[i] % abs(wd)
            B.wr(out_off + 4 * i, 4, fadd(lo, Fraction(rem)))
        marks.append('swapped' if swapped else 'ordered')
        if big:
            marks.append('big_width')
        if any(r == 0 for r in rands):
            marks.append('rand_zero')
        if any(r == 0x7FFF for r in rands):
            marks.append('rand_max')
    elif k in (NOP, NOWRITE, CONST):
        if k == NOP:
            W.setg(MARKER, 1, 0x5A)
            gorder = [(MARKER, 1)]
        W.snap()
        if k == CONST:
            ret = 0
    elif k == CREATEREF:
        cap, idx = a[6], a[8]
        A.wr(o + 0, 4, W.raw(2, a[4])); A.wr(o + 4, 4, W.raw(3, a[5])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[7])
        for i, b in enumerate(states):
            D.wr(a[5] + i, 1, b)
        W.snap()
        ref = ((idx << 8) | (a[9] & 0xFF)) & M32
        st = D.rd(a[5] + idx, 1) & 0x7F
        D.wr(a[5] + idx, 1, st)                   # `and byte [..],7Fh`
        D.wr(a[5] + idx, 1, st ^ ((st ^ (ref & 0xFF)) & 0x7F))
        cursor = 0
        while not (D.rd(a[5] + cursor, 1) & 0x80):
            cursor += 1
        A.wr(o + 0xC, 4, cursor)
        if idx == 0:
            marks.append('slot_zero')
        marks.append('cursor_zero' if cursor == 0 else 'cursor_moved')
        if (a[9] & 0x7F) == 0x7F:
            marks.append('counter_wrap')
        if cursor == cap:
            marks.append('sentinel')
    elif k in (OBJDTOR, OBJSDD):
        cap, idx = a[5], a[6]
        pool = W.raw(1, a[4])
        thisC = (W.raw(2, a[7]) + idx * OBJ_SIZE) & M32
        toff = a[7] + idx * OBJ_SIZE
        this = thisC
        B.wr(a[4] + 0, 4, W.raw(2, a[7])); B.wr(a[4] + 4, 4, W.raw(3, a[8])); B.wr(a[4] + 8, 4, cap); B.wr(a[4] + 0xC, 4, a[9])
        for i, b in enumerate(states):
            D.wr(a[8] + i, 1, b)
        W.setg(OBJ_POOL, 4, pool)
        gorder = [(OBJ_POOL, 4)]
        if k == OBJSDD:
            W.snap()
            events.append([0x59F660, W.tagv(this), Cc.rd(toff, 4)])
            Cc.wr(toff, 4, a[13])
            flags = a[11]
            if flags & 1:
                D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) | 0x80)
                if idx < s32(B.rd(a[4] + 0xC, 4)):
                    B.wr(a[4] + 0xC, 4, idx)
                    marks.append('lowers_cursor')
                else:
                    marks.append('keeps_cursor')
            marks.append('flag_bit0' if flags & 1 else 'flag_clear')
            if idx == 0:
                marks.append('first_slot')
            if idx == cap - 1:
                marks.append('last_slot')
            ret = W.tagv(this)
        else:
            for off in (0x14, 0x18, 0x24, 0x30, 0xB0, 0x134):
                Cc.wr(toff + off, 4, 0)
            flags = a[10]
            Cc.wr(toff + 0x140, 4, flags); Cc.wr(toff + 0x168, 2, a[11]); Cc.wr(toff + 0x22, 2, a[12]); Cc.wr(toff + 0x14A, 2, a[13]); Cc.wr(toff + 0x16A, 2, a[14])
            Cc.wr(toff + 0x13C, 1, a[15]); Cc.wr(toff + 0x164, 4, a[16])
            mi1, cm1, mi2, cm2 = W.raw(1, a[28]), W.raw(1, a[29]), W.raw(1, a[30]), W.raw(1, a[31])
            B.wr(a[28] + 0x14, 4, cm1); B.wr(a[29] + 0x28, 1, a[32]); B.wr(a[30] + 0x14, 4, cm2); B.wr(a[31] + 0x28, 1, a[33])
            table = {}
            if flags & 0x10000:
                table[a[12] & 0xFFFF] = (mi1, a[32])
            if a[13] != 0xFFFF:
                table[a[13] & 0xFFFF] = (mi2, a[33])
            W.setg(TEMP_OBJECTS, 4, a[26])
            W.setg(TRAIN_CROSSING, 4, a[27])
            gorder = [(OBJ_POOL, 4), (TEMP_OBJECTS, 4), (TRAIN_CROSSING, 4)]
            W.snap()
            if flags & 0x300000:
                events.append([0x409C90, (26230 + s16(a[34])) & M32])
                Cc.wr(toff + 0x140, 4, flags & 0xFFCFFFFF)
                events.append([0x46ABC0, W.tagv(this), a[11] & 0xFFFF])
                Cc.wr(toff + 0x168, 2, 0xFFFF)
                marks.append('script')
            else:
                marks.append('no_script')
            if flags & 0x10000:
                events.append([0x4107D0, table[a[12] & 0xFFFF][1] & 0xFF])
                marks.append('no_model')
            else:
                marks.append('with_model')
            ref = (idx << 8) + (states[idx] if idx < len(states) else 0)
            events.append([0x587C60, 3, ref & M32])
            if a[13] != 0xFFFF:
                events.append([0x4C4BB0, W.tagv(table[a[13] & 0xFFFF][0])])
                marks.append('ref_model')
            if a[14] != 0xFFFF:
                events.append([0x731A30, s16(a[14]) & M32])
                marks.append('txd')
            temp = a[26] & 0xFFFF
            if (a[15] & 0xFF) == 3:
                if temp > 0:
                    W.setg(TEMP_OBJECTS, 2, (temp - 1) & 0xFFFF)
                    marks.append('temp_dec')
                else:
                    marks.append('temp_zero')
            else:
                marks.append('temp_other')
            events.append([0x59F450, W.tagv(this)])
            if s16(a[12]) == (a[27] & 0xFFFF):
                matrix = a[19] != 0
                px, py = (a[23], a[24]) if matrix else (a[20], a[21])
                events.append([0x44D960, BRIDGE_THIS, fadd(px, Fraction(-12)), fadd(px, Fraction(12)), fadd(py, Fraction(-12)), fadd(py, Fraction(12)), 0])
                marks += ['train', 'matrix_pos' if matrix else 'dummy_pos']
            else:
                marks.append('no_train')
            if a[16]:
                events.append([0x5393F0, W.tagv(a[16])])
                marks.append('fire')
            else:
                marks.append('no_fire')
    elif k == CENTRE:
        W.snap()
        for i in range(3):
            A.wr(o + 0xA4 + 4 * i, 4, a[4 + i])
        if any(is_nan(x) for x in a[4:7]):
            marks.append('nan_arg')
        if any(x == 0 for x in a[4:7]):
            marks.append('zero_arg')
    elif k == PEDCTOR:
        W.snap()
        for i in range(30):
            A.wr(o + 8 + 4 * i, 4, 0)
        ret = W.tagv(this)
    elif k == MATDTOR:
        W.snap()
        events.append([0x59ACD0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == EVSDD:
        cap, idx = a[5], a[6]
        pool = W.raw(1, a[4])
        thisC = (W.raw(2, a[7]) + idx * EVENT_SIZE + a[10]) & M32
        toff = a[7] + idx * EVENT_SIZE + a[10]
        B.wr(a[4] + 0, 4, W.raw(2, a[7])); B.wr(a[4] + 4, 4, W.raw(3, a[8])); B.wr(a[4] + 8, 4, cap); B.wr(a[4] + 0xC, 4, a[9])
        for i, b in enumerate(states):
            D.wr(a[8] + i, 1, b)
        W.setg(EV_POOL, 4, pool)
        gorder = [(EV_POOL, 4)]
        W.snap()
        events.append([EVENT_BODIES[fn - 20], W.tagv(thisC), Cc.rd(toff, 4)])
        Cc.wr(toff, 4, a[13])
        flags = a[11]
        if flags & 1:
            D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) | 0x80)
            if idx < s32(B.rd(a[4] + 0xC, 4)):
                B.wr(a[4] + 0xC, 4, idx)
                marks.append('lowers_cursor')
            else:
                marks.append('keeps_cursor')
        marks.append('flag_bit0' if flags & 1 else 'flag_clear')
        if idx == 0:
            marks.append('first_slot')
        if idx == cap - 1:
            marks.append('last_slot')
        if a[10]:
            marks.append('extra_bytes')
        ret = W.tagv(thisC)
    elif k == KICKED:
        anim = W.raw(1, a[5]) if a[4] else 0
        A.wr(o + 0xC, 4, anim)
        W.snap()
        A.wr(o, 4, 0x86D7D8)
        if a[4]:
            B.wr(a[5] + 0x1C, 4, 0xC0800000)
            events.append([0x4CEBE0, W.tagv(anim), 1, 0])
            marks.append('anim')
        else:
            marks.append('no_anim')
        events.append([0x61A3A0, W.tagv(this), 0x86D7D8])
        A.wr(o, 4, a[7])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if KIND[c['fn']] == OBJDTOR and o.get('chain_vtable_dll') is not True:
        return 'la cadena de destructores base no dejo la vtable de la DLL'
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
