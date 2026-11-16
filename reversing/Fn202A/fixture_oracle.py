"""Oraculo independiente (Python) de Fn202A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 202A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y de las cadenas (Fixtures.hpp)
para conocer su imagen inicial, y una FPU x87 por software (x87soft.py: 24 bits de mantisa en fmul/fadd/fsub/fsqrt, 64 bits en fpatan/fsin/fcos con mpmath) para las funciones con aritmetica de coma flotante.

Desensamblado del EXE (sobre el gta-sa.exe canonico; ver tambien reversing/PendienteValidar/202A.md).
   0  0x4AC900  GetLifeTime: xor eax,eax          1  0x4AC910  GetEventPriority: mov eax,3Ah          2  0x4AC980  TakesPriorityOver: mov al,1; ret 4          21 0x61AEA0  GetTaskType: mov eax,1A9h
   3..13  Clone / CloneEditable de eventos: ecx = [0xB744AC]; call 0x4B5570 (CEventPool::New); si eax es nulo devuelve 0; si no, llama al constructor con ecx = memoria y los argumentos de abajo; devuelve el EAX del constructor.
        3 CEventPedCollisionWithPed::Clone (ctor 0x4AC990): (word +0xC sin signo, +0x10, +0x14, &+0x18, &+0x24, word +0x30, word +0x32)
        4 CEventObjectCollision::Clone (0x4ACCF0): (word +0xC, +0x10, +0x14, &+0x18, &+0x24, word +0xE)       5 CEventBuildingCollision::Clone (0x4ACF00): igual
        6 CEventDraggedOutCar::CloneEditable (0x4AD250): (+0x18, +0x14, byte +0x1C)      8 CEventDeadPed::CloneEditable (0x4ADEA0): (+0x14, byte +0x18, +0x1C)
        10 CEventPotentialGetRunOver (0x4AE1A0): (+0x14)    11 CEventPotentialWalkIntoVehicle (0x4AE320): (+0x14, +0x18)    12 CEventPotentialWalkIntoObject (0x4AE5D0): (+0x14, +0x18)
        13 CEventPotentialWalkIntoPed (0x4AE6E0): (+0x20, &+0x14, +0x24)
        7 CEventDeath::Clone: sin constructor: [mem+4] = 0, byte [mem+8] = 0, [mem] = 85ADC0h, byte [mem+0xC] = byte [this+0xC], [mem+0x10] = [this+0x10]
        9 CEventGroupEvent::Clone: tras New, call [[this+0x10]+0x10] (Clone del evento interno) y ctor 0x4ADFD0 (+0xC, clon)
  14  0x61AC90  ctor por identificador (7 args): base 0x61A390; byte +0xC = ((arg7 & 1) << 2) | (byte +0xC & 0xF8); +0x10 = a1, +0x14 = a2, +8 = 0, +0x48 = a3, [0] = 86D594h, +0x60 = +0x64 = 0, bytes +0x68 = +0x69 = 0,
              +0x4C = a4, +0x44 = 0, +0x18 = 0, word +0x6C = 1A8h, +0x50 = a5, +0x5C = a6; devuelve this
  15  0x61AD10  ctor por nombre (8 args): base 0x61A390; byte +0xC igual con arg8; +8 = 0; +0x18 = a3; +0x48 = a4; [0] = 86D594h; +0x60/+0x64 = 0; bytes +0x68/+0x69 = 0; +0x50 = a6; +0x5C = a7; word +0x6C = 1A8h; +0x4C = a5;
              copia byte a byte (con NUL) a1 -> +0x1C y a2 -> +0x34; 0x4D3940(+0x34); 0x4D42F0(+0x1C, bloque) -> +0x44; +0x10 = 0; +0x14 = 0BFh; devuelve this
  16  0x61ADF0  StartAnim(ped): con [+0x44] != 0: 0x4D4410(clump = [ped+0x18], jerarquia, [+0x18], [+0x48]); si no 0x4D4610(clump, [+0x10], [+0x14], [+0x48]); [+8] = eax; 0x4CEBE0 (ecx = eax)(0x61A8A0, this)
  17  0x61B890  Clone: push 70h; call 0x61A5A0; nulo -> 0; con [+0x44] != 0: ctor 0x61AD10(&+0x1C, &+0x34, [+0x18], [+0x48], [+0x4C], [+0x50], [+0x5C], (byte [+0xC] & 4) >> 2); si no 0x61AC90([+0x10], [+0x14],
              [+0x48], [+0x4C], [+0x50], [+0x5C], mismo bit); devuelve el EAX del ctor
  18  0x61BC80  destructor con borrado (flags): call 0x61BCA0; si flags & 1: 0x61A5B0(this); devuelve this      19  0x61BCA0  jmp 0x61A6F0
  20  0x61BCB0  ProcessPed(ped): ver model_lm_process
  22  0x61AEB0  cuerpo del destructor de HandSignal: [this] = 86D5B8h; con mano izquierda [+0x1C] != 0: 0x563280(mano); relee +0x1C; si no es nulo: destructor virtual (ranura 0, flags 1); dec word [0xBB4A70]; igual con +0x20; call 0x61A6F0
  23  0x61AF50  jmp 0x61A790 (MakeAbortable)      24  0x61B980  Clone: push 24h; call 0x61A5A0; nulo -> 0; ctor 0x61AE50([+0x10], [+0x14], byte [+0x18], (byte [+0xC] & 4) >> 2); devuelve el EAX del ctor
  25  0x61BD80  destructor con borrado: call 0x61AEB0; si flags & 1: 0x61A5B0(this); devuelve this
  26  0x5921D0  Interior_c ctor: 0x4A8DB0 (+0 y +4 a cero); 0x4A8DD0 (+0x5C, +0x60, +0x64 a cero); words +0x3EC y +0x3F0 = 0FFFFh; devuelve this      27  0x597FE0  InteriorGroup_c ctor: 0x4A8DB0; devuelve this
  28  0x59A840  ConvertToEulerAngles(pX, pY, pZ, banderas)      29  0x59AA40  ConvertFromEulerAngles(x, y, z, banderas): ver model_euler_*
  30  0x5FFFD0  ScanEffect(efecto, entidad, peaton): ver model_scan      31  0x600180  GetBestEffect(pEfecto, pEntidad): ver model_best

Los falsos anotan: [direccion, ECX (etiqueta de arena), argumentos...] (etiquetas: un valor dentro de una arena es (indice de arena, desplazamiento); un puntero a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import x87soft as X

M32 = 0xFFFFFFFF
NAMES = ["CEventVehicleCollision::GetLifeTime", "CEventVehicleCollision::GetEventPriority", "CEventVehicleCollision::TakesPriorityOver", "CEventPedCollisionWithPed::Clone", "CEventObjectCollision::Clone",
         "CEventBuildingCollision::Clone", "CEventDraggedOutCar::CloneEditable", "CEventDeath::Clone", "CEventDeadPed::CloneEditable", "CEventGroupEvent::Clone", "CEventPotentialGetRunOver::CloneEditable",
         "CEventPotentialWalkIntoVehicle::CloneEditable", "CEventPotentialWalkIntoObject::CloneEditable", "CEventPotentialWalkIntoPed::CloneEditable", "LoopedMiddle::ConstructorById", "LoopedMiddle::ConstructorByName",
         "LoopedMiddle::StartAnim", "LoopedMiddle::Clone", "LoopedMiddle::ScalarDeletingDestructor", "LoopedMiddle::DestructorBody", "LoopedMiddle::ProcessPed", "CTaskSimplePlayHandSignalAnim::GetTaskType",
         "CTaskSimplePlayHandSignalAnim::DestructorBodyExe", "CTaskSimplePlayHandSignalAnim::MakeAbortable", "CTaskSimplePlayHandSignalAnim::Clone", "CTaskSimplePlayHandSignalAnim::ScalarDeletingDestructor",
         "Interior_c::Constructor", "InteriorGroup_c::Constructor", "CMatrix::ConvertToEulerAngles", "CMatrix::ConvertFromEulerAngles", "CAttractorScanner::ScanEffect", "CAttractorScanner::GetBestEffect"]
ENTRIES = (0x4AC900, 0x4AC910, 0x4AC980, 0x4B6C40, 0x4B6CC0, 0x4B6D40, 0x4B6DC0, 0x4B6E30, 0x4B6E70, 0x4B6EE0, 0x4B6F40, 0x4B6FB0, 0x4B7020, 0x4B7090, 0x61AC90, 0x61AD10, 0x61ADF0, 0x61B890, 0x61BC80, 0x61BCA0,
           0x61BCB0, 0x61AEA0, 0x61AEB0, 0x61AF50, 0x61B980, 0x61BD80, 0x5921D0, 0x597FE0, 0x59A840, 0x59AA40, 0x5FFFD0, 0x600180)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK = 0x7F000000
(CONST, CLONE, DEATH, GROUP, LMCTORID, LMCTORNAME, LMSTART, LMCLONE, SDD, BODYONLY, LMPROCESS, HSDTOR, HSABORT, HSCLONE, INTERIOR, IGCTOR, EULERTO, EULERFROM, SCAN, BEST) = \
    'const clone death group lmctorid lmctorname lmstart lmclone sdd bodyonly lmprocess hsdtor hsabort hsclone interior igctor eulerto eulerfrom scan best'.split()
KIND = [CONST, CONST, CONST, CLONE, CLONE, CLONE, CLONE, DEATH, CLONE, GROUP, CLONE, CLONE, CLONE, CLONE, LMCTORID, LMCTORNAME, LMSTART, LMCLONE, SDD, BODYONLY, LMPROCESS, CONST, HSDTOR, HSABORT, HSCLONE,
        SDD, INTERIOR, IGCTOR, EULERTO, EULERFROM, SCAN, BEST]
CONST_VALUE = {0: 0, 1: 0x3A, 2: 1, 21: 0x1A9}
# (constructor, argumentos): h = word sin signo en el desplazamiento, b = byte, d = dword, p = dword (puntero: etiqueta), a = direccion this + desplazamiento
CLONE_SPEC = {
    3: (0x4AC990, [('h', 0xC), ('d', 0x10), ('p', 0x14), ('a', 0x18), ('a', 0x24), ('h', 0x30), ('h', 0x32)]),
    4: (0x4ACCF0, [('h', 0xC), ('d', 0x10), ('p', 0x14), ('a', 0x18), ('a', 0x24), ('h', 0xE)]),
    5: (0x4ACF00, [('h', 0xC), ('d', 0x10), ('p', 0x14), ('a', 0x18), ('a', 0x24), ('h', 0xE)]),
    6: (0x4AD250, [('p', 0x18), ('p', 0x14), ('b', 0x1C)]),
    8: (0x4ADEA0, [('p', 0x14), ('b', 0x18), ('d', 0x1C)]),
    10: (0x4AE1A0, [('p', 0x14)]),
    11: (0x4AE320, [('p', 0x14), ('d', 0x18)]),
    12: (0x4AE5D0, [('p', 0x14), ('d', 0x18)]),
    13: (0x4AE6E0, [('p', 0x20), ('a', 0x14), ('d', 0x24)]),
}
# campos del objeto origen que pone el banco (desplazamiento) por funcion, en el orden de a[10..]
FIELDS = {3: [0xC, 0x10, 0x14, 0x30], 4: [0xC, 0x10, 0x14], 5: [0xC, 0x10, 0x14], 6: [0x14, 0x18, 0x1C], 7: [0xC, 0x10], 8: [0x14, 0x18, 0x1C], 9: [0xC], 10: [0x14], 11: [0x14, 0x18], 12: [0x14, 0x18],
          13: [0x20, 0x24], 17: [0xC, 0x10, 0x14, 0x18, 0x44, 0x48, 0x4C, 0x50, 0x5C], 24: [0xC, 0x10, 0x14, 0x18]}
EUL_NEXT = [1, 2, 0, 1, 0, 0, 0, 0]       # 0x866D94 (bytes del EXE)
EUL_FIRST = [0, 1, 2, 0]                  # 0x866D9C
EUL_THRESHOLD = 0x36000000                # 0x866D90
RAIN_LIMIT = 0x3E4CCCCD                   # 0x858CC4
FLT_MAX = 0x7F7FFFFF                      # 0x863A3C
GLOBAL_POOL, GLOBAL_NOTEMP, GLOBAL_CLOCK, GLOBAL_RAIN, GLOBAL_RADII = 0xB744AC, 0xBB4A70, 0xB7CB84, 0xC81324, 0xC39EE0


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix(((c & M32) * 0x9E3779B1 + (k & M32) * 0x85EBCA6B + 100001) & M32)


def strbyte(seed, j):
    return 1 + rnd(seed, 5000 + j) % 255


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


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
        self.mem = tuple(Mem(a[2], a[92 + i]) for i in range(4))
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

    def getg(self, addr, n):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


def ctor_eax(sel, mem, raw):
    return [mem, (mem + 4) & M32, 0, raw][sel]


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
EXTRA = {
    CONST: [],
    CLONE: ['new_null', 'new_ok', 'eax_mem', 'eax_other'],
    DEATH: ['new_null', 'new_ok', 'byte_other'],
    GROUP: ['new_null', 'new_ok', 'eax_mem', 'eax_other'],
    LMCTORID: ['hold1', 'hold0', 'base_c_high'],
    LMCTORNAME: ['name_short', 'name_long', 'group_long', 'same_string', 'hold1', 'hold0'],
    LMSTART: ['hier', 'nohier', 'assoc_null', 'assoc_set'],
    LMCLONE: ['new_null', 'new_ok', 'hier', 'nohier', 'hold1'],
    SDD: ['flag_delete', 'flag_keep'],
    BODYONLY: [],
    LMPROCESS: ['finished', 'noassoc_start', 'found_restart', 'notfound_timer_out', 'notfound_straddle', 'notfound_no_straddle', 'notfound_timer_off', 'cur_nan'],
    HSDTOR: ['left', 'right', 'both', 'none', 'swap', 'same_hand'],
    HSABORT: [],
    HSCLONE: ['new_null', 'new_ok', 'eax_mem', 'eax_other'],
    INTERIOR: [],
    IGCTOR: [],
    EULERTO: ['rep0', 'rep1', 'parity0', 'parity1', 'swap0', 'swap1', 'lock', 'nolock', 'alias_out'],
    EULERFROM: ['rep0', 'rep1', 'parity0', 'parity1', 'swap0', 'swap1'],
    SCAN: ['rain_hi', 'rain_lo', 'type4', 'type7', 'entity', 'no_entity', 'dist_lt', 'dist_ge', 'has_slot', 'no_slot', 'approach', 'no_approach', 'radius_neg', 'radius_cut'],
    BEST: ['refuge', 'scan', 'picked', 'none_picked', 'alias_out'],
}
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _k += EXTRA[KIND[_fn]]
    KEYS[_fn] = _k


# ------------------------------------------------------------------------------------------------ modelos
def model_const(c, W, A, fn, a, o):
    W.snap()
    return [], CONST_VALUE[fn], []


def model_clone(c, W, A, fn, a, o, marks):
    """Clone de eventos y tareas con New + constructor"""
    this = W.raw(0, o)
    k = KIND[fn]
    for i, off in enumerate(FIELDS.get(fn, [])):
        A.wr(o + off, 4, a[10 + i])
    group = k == GROUP
    if group:
        A.wr(o + 0x10, 4, W.raw(2, a[19]))
        W.mem[2].wr(a[19], 4, W.raw(3, a[20]))
    mem = W.raw(1, a[5]) if a[4] else 0
    if fn <= 13:
        W.setg(GLOBAL_POOL, 4, W.raw(3, a[9]))
    W.snap()
    events = []
    sel = a[8]
    marks += ['new_ok' if mem else 'new_null', 'eax_mem' if sel == 0 else 'eax_other']
    if fn <= 13:
        events.append([0x4B5570, W.tagv(W.raw(3, a[9]))])
    elif fn == 17:
        events.append([0x61A5A0, 0x70])
    elif fn == 24:
        events.append([0x61A5A0, 0x24])
    gorder = [(GLOBAL_POOL, 4)] if fn <= 13 else []
    if not mem:
        return events, 0, gorder
    if k == DEATH:
        W.wr(mem + 4, 4, 0)
        W.wr(mem + 8, 1, 0)
        W.wr(mem, 4, 0x85ADC0)
        W.wr(mem + 0xC, 1, A.rd(o + 0xC, 1))
        W.wr(mem + 0x10, 4, A.rd(o + 0x10, 4))
        if A.rd(o + 0xC, 1) not in (0, 1):
            marks.append('byte_other')
        return events, W.tagv(mem), gorder
    if group:
        inner = A.rd(o + 0x10, 4)
        events.append([0x1004, W.tagv(inner)])
        events.append([0x4ADFD0, W.tagv(mem), W.tagv(A.rd(o + 0xC, 4)), W.tagv(a[21])])
    elif fn == 17:
        hier = A.rd(o + 0x44, 4)
        hold = (A.rd(o + 0xC, 1) & 4) >> 2
        marks.append('hold1' if hold else 'hold0')
        if hier:
            marks.append('hier')
            args = [W.tagv(mem), W.tagv(this + 0x1C), W.tagv(this + 0x34)] + [W.tagv(A.rd(o + off, 4)) for off in (0x18, 0x48, 0x4C, 0x50, 0x5C)] + [W.tagv(hold)]
            events.append([0x61AD10] + args)
        else:
            marks.append('nohier')
            args = [W.tagv(mem)] + [W.tagv(A.rd(o + off, 4)) for off in (0x10, 0x14, 0x48, 0x4C, 0x50, 0x5C)] + [W.tagv(hold)]
            events.append([0x61AC90] + args)
    elif fn == 24:
        hold = (A.rd(o + 0xC, 1) & 4) >> 2
        events.append([0x61AE50, W.tagv(mem), W.tagv(A.rd(o + 0x10, 4)), W.tagv(A.rd(o + 0x14, 4)), W.tagv(A.rd(o + 0x18, 1)), W.tagv(hold)])
    else:
        addr, spec = CLONE_SPEC[fn]
        args = [W.tagv(mem)]
        for kind, off in spec:
            if kind == 'h':
                args.append(W.tagv(A.rd(o + off, 2)))
            elif kind == 'b':
                args.append(W.tagv(A.rd(o + off, 1)))
            elif kind == 'a':
                args.append(W.tagv(this + off))
            else:
                args.append(W.tagv(A.rd(o + off, 4)))
        events.append([addr] + args)
    W.wr(mem, 4, a[6])
    return events, W.tagv(ctor_eax(sel, mem, a[7])), gorder


def read_cstr(W, addr, limit=0x100):
    out = bytearray()
    for k in range(limit):
        b = W.rd(addr + k, 1)
        if b == 0:
            break
        out.append(b)
    return bytes(out)


def model_lm_ctor_id(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    events = [[0x61A390, W.tagv(this), a[3]]]
    A.wr(o, 4, a[11]); A.wr(o + 4, 4, a[12]); A.wr(o + 0xC, 1, a[13] & 0xFF)
    hold = a[10] & 1
    marks.append('hold1' if hold else 'hold0')
    if a[13] & 0xFF > 7:
        marks.append('base_c_high')
    old = A.rd(o + 0xC, 1)
    A.wr(o + 0xC, 1, ((hold << 2) | (old & 0xF8)) & 0xFF)
    A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x14, 4, a[5]); A.wr(o + 8, 4, 0); A.wr(o + 0x48, 4, a[6])
    A.wr(o, 4, 0x86D594); A.wr(o + 0x60, 4, 0); A.wr(o + 0x64, 4, 0); A.wr(o + 0x68, 1, 0); A.wr(o + 0x69, 1, 0)
    A.wr(o + 0x4C, 4, a[7]); A.wr(o + 0x44, 4, 0); A.wr(o + 0x18, 4, 0); A.wr(o + 0x6C, 2, 0x1A8)
    A.wr(o + 0x50, 4, a[8]); A.wr(o + 0x5C, 4, a[9])
    return events, W.tagv(this), []


def model_lm_ctor_name(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    name_addr, group_addr = W.raw(2, a[4]), W.raw(2, a[5])
    for j in range(a[12]):
        W.mem[2].wr(a[4] + j, 1, strbyte(a[14], j))
    W.mem[2].wr(a[4] + a[12], 1, 0)
    if not a[15]:
        for j in range(a[13]):
            W.mem[2].wr(a[5] + j, 1, strbyte(a[14] + 1, j))
        W.mem[2].wr(a[5] + a[13], 1, 0)
    W.snap()
    events = [[0x61A390, W.tagv(this), a[3]]]
    A.wr(o, 4, a[16]); A.wr(o + 4, 4, a[17]); A.wr(o + 0xC, 1, a[18] & 0xFF)
    hold = a[11] & 1
    old = A.rd(o + 0xC, 1)
    A.wr(o + 0xC, 1, ((hold << 2) | (old & 0xF8)) & 0xFF)
    A.wr(o + 8, 4, 0); A.wr(o + 0x18, 4, a[6]); A.wr(o + 0x48, 4, a[7]); A.wr(o, 4, 0x86D594); A.wr(o + 0x60, 4, 0); A.wr(o + 0x64, 4, 0); A.wr(o + 0x68, 1, 0); A.wr(o + 0x69, 1, 0)
    A.wr(o + 0x50, 4, a[9]); A.wr(o + 0x5C, 4, a[10]); A.wr(o + 0x6C, 2, 0x1A8); A.wr(o + 0x4C, 4, a[8])
    for src, dst in ((name_addr, this + 0x1C), (group_addr, this + 0x34)):
        data = read_cstr(W, src, 0x200) + b'\0'
        for j, b in enumerate(data):
            W.wr(dst + j, 1, b)
    events.append([0x4D3940, W.tagv(this + 0x34), fnv(read_cstr(W, this + 0x34))])
    events.append([0x4D42F0, W.tagv(this + 0x1C), fnv(read_cstr(W, this + 0x1C)), W.tagv(a[19])])
    A.wr(o + 0x44, 4, a[20]); A.wr(o + 0x10, 4, 0); A.wr(o + 0x14, 4, 0xBF)
    marks.append('hold1' if hold else 'hold0')
    marks.append('name_long' if a[12] >= 24 else 'name_short')
    if a[13] >= 16:
        marks.append('group_long')
    if a[15]:
        marks.append('same_string')
    return events, W.tagv(this), []


def model_lm_start(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.mem[0].wr(o + 0x44, 4, a[4]); A.wr(o + 0x18, 4, a[5]); A.wr(o + 0x10, 4, a[6]); A.wr(o + 0x14, 4, a[7]); A.wr(o + 0x48, 4, a[8])
    A.wr(o + 8, 4, a[12])
    W.mem[2].wr(a[9] + 0x18, 4, a[10])
    ped = W.raw(2, a[9])
    W.snap()
    events = []
    if a[4]:
        events.append([0x4D4410, a[10], W.tagv(a[4]), a[5], a[8]])
        marks.append('hier')
    else:
        events.append([0x4D4610, a[10], a[6], a[7], a[8]])
        marks.append('nohier')
    A.wr(o + 8, 4, a[11])
    events.append([0x4CEBE0, W.tagv(a[11]), 0x61A8A0, W.tagv(this)])
    marks.append('assoc_null' if a[11] == 0 else 'assoc_set')
    return events, None, []


def model_sdd(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    body = 0x61BCA0 if fn == 18 else 0x61AEB0
    events = [[body, W.tagv(this), a[3]]]
    A.wr(o, 4, a[5])
    if a[6] & 1:
        events.append([0x61A5B0, W.tagv(this), a[5]])
        marks.append('flag_delete')
    else:
        marks.append('flag_keep')
    return events, W.tagv(this), []


def model_body_only(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    A.wr(o, 4, a[5])
    return [[0x61A6F0, W.tagv(this), a[3]]], None, []


def soft_cmp_lt(x, y):
    return X.compare(x, y) == 'lt'


def model_lm_process(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    ped = W.raw(2, a[9])
    A.wr(o + 0xC, 4, a[4]); A.wr(o + 8, 4, W.raw(3, a[6]) if a[5] else 0); A.wr(o + 0x14, 4, a[7]); A.wr(o + 0x68, 4, a[8])
    A.wr(o + 0x54, 4, a[20]); A.wr(o + 0x58, 4, a[19]); A.wr(o + 0x4C, 4, a[21]); A.wr(o + 0x50, 4, a[22]); A.wr(o + 0x5C, 4, a[23]); A.wr(o + 0x60, 4, a[26]); A.wr(o + 0x64, 4, a[27])
    D = W.mem[3]
    D.wr(a[6] + 0x20, 4, a[17]); D.wr(a[6] + 0x28, 4, a[18]); D.wr(a[6] + 0x2C, 4, a[24]); D.wr(a[6] + 0x14, 4, W.raw(3, a[14]))
    D.wr(a[13] + 0x14, 4, W.raw(3, a[14])); D.wr(a[14] + 0x10, 4, a[15])
    W.mem[2].wr(a[9] + 0x18, 4, a[10])
    W.setg(GLOBAL_CLOCK, 4, a[16])
    W.snap()
    gorder = [(GLOBAL_CLOCK, 4)]
    events = []
    if a[4] & 1:
        marks.append('finished')
        return events, 1, gorder
    start = True
    assoc = W.raw(3, a[6]) if a[5] else 0
    if assoc:
        sel = 1 if a[7] == 0 else 0
        events.append([0x4D68B0, a[10], sel])
        if a[11] == 0:
            start = False
            if a[8] & 0xFF:
                events.append([0x420E30, W.tagv(this + 0x60)])
                if a[12] & 0xFF:
                    marks.append('notfound_timer_out')
                    return events, 0, gorder
            else:
                marks.append('notfound_timer_off')
            cur, end, step = X.from_f32(a[17]), X.from_f32(a[19]), X.from_f32(a[18])
            if X.compare(cur, end) != 'gt':
                marks.append('notfound_no_straddle')
                if X.compare(cur, end) == 'un':
                    marks.append('cur_nan')
                return events, 0, gorder
            diff = X.sub(cur, step)
            if X.compare(diff, end) not in ('lt', 'eq'):
                marks.append('notfound_no_straddle')
                return events, 0, gorder
            marks.append('notfound_straddle')
            events.append([0x4CEA80, W.tagv(assoc), a[20]])
            D.wr(a[6] + 0x2E, 1, D.rd(a[6] + 0x2E, 1) | 1)
            return events, 0, gorder
        marks.append('found_restart')
    else:
        marks.append('noassoc_start')
    if start:
        events.append([0x61ADF0, W.tagv(this), W.tagv(ped)])
        A.wr(o + 8, 4, W.raw(3, a[13]))
        total = X.from_f32(a[15])
        A.wr(o + 0x54, 4, X.to_f32(X.mul(total, X.from_f32(a[21]))))
        A.wr(o + 0x58, 4, X.to_f32(X.mul(total, X.from_f32(a[22]))))
        A.wr(o + 0x60, 4, a[16])
        A.wr(o + 0x64, 4, A.rd(o + 0x5C, 4))
        A.wr(o + 0x68, 1, 1)
    return events, 0, gorder


def model_hs_dtor(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    h1, h2, h3 = W.raw(2, a[6]), W.raw(2, a[7]), W.raw(2, 0x700)
    A.wr(o + 0x1C, 4, h1 if a[4] else 0)
    A.wr(o + 0x20, 4, 0 if a[5] == 0 else (h2 if a[5] == 1 else h1))
    W.setg(GLOBAL_NOTEMP, 4, a[9])
    W.snap()
    gorder = [(GLOBAL_NOTEMP, 4)]
    events = []
    A.wr(o, 4, 0x86D5B8)
    calls = 0
    swap_off = 0x20 if a[11] else 0x1C
    swap_val = h3 if a[12] else 0
    for off in (0x1C, 0x20):
        hand = A.rd(o + off, 4)
        if hand:
            events.append([0x563280, W.tagv(hand)])
            calls += 1
            if calls == a[10]:
                A.wr(o + swap_off, 4, swap_val)
                marks.append('swap')
            again = A.rd(o + off, 4)
            if again:
                events.append([0x2000, W.tagv(again), 1])
            nt = W.getg(GLOBAL_NOTEMP, 4)
            W.setg(GLOBAL_NOTEMP, 4, (nt & 0xFFFF0000) | (((nt & 0xFFFF) - 1) & 0xFFFF))
    marks.append('left' if a[4] else 'none')
    if a[5] == 1:
        marks.append('right')
    if a[4] and a[5]:
        marks.append('both')
    if a[5] == 2:
        marks.append('same_hand')
    events.append([0x61A6F0, W.tagv(this), A.rd(o, 4)])
    A.wr(o, 4, a[13])
    return events, None, gorder


def model_hs_abort(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    return [[0x61A790, W.tagv(this), W.tagv(a[4]), a[5], W.tagv(a[6])]], a[7] & 0xFF, []


def model_interior(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    A.wr(o, 4, 0); A.wr(o + 4, 4, 0)
    A.wr(o + 0x5C, 4, 0); A.wr(o + 0x60, 4, 0); A.wr(o + 0x64, 4, 0)
    A.wr(o + 0x3EC, 2, 0xFFFF); A.wr(o + 0x3F0, 2, 0xFFFF)
    return [], W.tagv(this), []


def model_ig(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    W.snap()
    A.wr(o, 4, 0); A.wr(o + 4, 4, 0)
    return [], W.tagv(this), []


# ------------------------------------------------------------------------------------------------ CMatrix::ConvertToEulerAngles / ConvertFromEulerAngles
def euler_axes(flags):
    par = (flags >> 2) & 1
    i = EUL_FIRST[(flags >> 3) & 3]
    j = EUL_NEXT[i + par]
    k = EUL_NEXT[i - par + 1]
    return i, j, k, par, (flags >> 1) & 1, flags & 1


def finite_f32(bits):
    return (bits >> 23) & 0xFF != 0xFF


def model_euler_to(c, W, A, fn, a, o, marks):
    offs = [0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28]
    for q in range(9):
        A.wr(o + offs[q], 4, a[6 + q])
    ptrs = []
    for sel, off in ((a[15], a[16]), (a[17], a[18]), (a[19], a[20])):
        ptrs.append(W.raw(0, o + off) if sel == 0 else W.raw(sel, off))
    W.snap()
    flags = a[5]
    i, j, k, par, rep, swap = euler_axes(flags)
    marks.append('rep1' if rep else 'rep0'); marks.append('parity1' if par else 'parity0'); marks.append('swap1' if swap else 'swap0')
    if len({ptrs[0], ptrs[1], ptrs[2]}) < 3 or any(W.locate(p)[0] == 0 for p in ptrs):
        marks.append('alias_out')
    m = [[a[6 + 3 * r + cc] for cc in range(3)] for r in range(3)]     # m[fila][columna] = elemento en el desplazamiento (4*(4*fila + columna) con la fila de 16 bytes)
    if not all(finite_f32(v) for row in m for v in row):
        raise X.Unmodeled('elemento no finito')
    M = lambda r, cc: X.from_f32(m[r][cc])
    thr = X.from_f32(EUL_THRESHOLD)
    st = []

    def fld(v):
        st.append(v)

    def fstp_f32(addr):
        v = st.pop()
        W.wr(addr, 4, X.to_f32(v))
    px, py, pz = ptrs
    # ranuras de la copia local: indices 3*fila + columna
    if rep:
        # s = sqrt(M[i][j]^2 + M[i][k]^2) en ST0
        fld(M(i, k)); fld(M(i, j))
        fld(st[-1]); st[-1] = X.mul(st[-1], st[-2])
        fld(st[-3]); st[-1] = X.mul(st[-1], st[-4])
        s1 = st.pop(); st[-1] = X.add(st[-1], s1)
        st[-1] = X.sqrt(st[-1])
        sv = st.pop(); st.pop(); st.pop(); st.append(sv)       # fstp st(2); fstp st(0): queda s
        s = st[-1]
        cmp = X.compare(s, thr)
        if cmp in ('gt',):
            marks.append('nolock')
            fld(M(i, j)); fld(M(i, k))
            st[-1], st[-2] = st[-1], st[-2]
            # fld [ebp] = M[i][j]; fld [edi] = M[i][k]; fpatan -> atan2(M[i][j], M[i][k])
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(px)
            fld(M(i, i))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(py)
            fld(M(j, i)); fld(M(k, i)); st[-1] = st[-1].neg()
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(pz)
        else:
            marks.append('lock')
            fld(M(j, k)); st[-1] = st[-1].neg(); fld(M(j, j))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(px)
            fld(M(i, i))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(py)
            W.wr(pz, 4, 0)
    else:
        # cy = sqrt(M[i][i]^2 + M[j][i]^2)
        fld(M(j, i)); fld(M(i, i))
        fld(st[-2])                                             # M[j][i]
        t = X.mul(st[-3], st[-1])                               # fmulp st(2),st: ST2 = M[j][i] * M[j][i]
        st[-3] = t; st.pop()
        fld(st[-1]); st[-1] = X.mul(st[-1], st[-2])             # M[i][i]^2
        t = X.add(st[-3], st[-1]); st[-3] = t; st.pop()         # faddp st(2),st: ST2 = ST2 + ST0
        st[-1], st[-2] = st[-2], st[-1]                         # fxch st(1)
        st[-1] = X.sqrt(st[-1])
        sv = st.pop(); st[-1] = sv                              # fstp st(1)
        s = st[-1]
        cmp = X.compare(s, thr)
        if cmp == 'gt':
            marks.append('nolock')
            fld(M(k, j)); fld(M(k, k))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(px)
            fld(M(k, i)); st[-1] = st[-1].neg()
            st[-1], st[-2] = st[-2], st[-1]                     # fxch st(1): ST0 = cy, ST1 = -M[k][i]
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(py)
            fld(M(j, i)); fld(M(i, i))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(pz)
        else:
            marks.append('lock')
            fld(M(j, k)); st[-1] = st[-1].neg(); fld(M(j, j))
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(px)
            fld(M(k, i)); st[-1] = st[-1].neg()
            st[-1], st[-2] = st[-2], st[-1]
            x_, y_ = st.pop(), st.pop()
            st.append(X.fpatan(y_, x_))
            fstp_f32(py)
            W.wr(pz, 4, 0)
    st.clear()
    if par:
        for p in (px, py, pz):
            v = X.from_f32(W.rd(p, 4))
            W.wr(p, 4, X.to_f32(v.neg()))
    if swap:
        edx = W.rd(pz, 4)
        v = X.from_f32(W.rd(px, 4))
        W.wr(px, 4, edx)
        W.wr(pz, 4, X.to_f32(v))
    return [], None, []


def model_euler_from(c, W, A, fn, a, o, marks):
    W.snap()
    flags = a[5]
    i, j, k, par, rep, swap = euler_axes(flags)
    marks.append('rep1' if rep else 'rep0'); marks.append('parity1' if par else 'parity0'); marks.append('swap1' if swap else 'swap0')
    x, y, z = a[6], a[7], a[8]
    if not all(finite_f32(v) for v in (x, y, z)):
        raise X.Unmodeled('angulo no finito')
    F = X.from_f32
    mul, add, sub, f32 = X.mul, X.add, X.sub, X.to_f32
    # memoria local del EXE: [4C] = x, [50] = y, [54] = z (floats)
    mx, my, mz = x, y, z
    if swap:
        va = F(mz)                            # fld [54]: A = z
        mz = mx                               # [54] = x (C)
    else:
        va = F(mx)
    if par:
        va = va.neg()
        vb = F(my).neg()
        mz = f32(F(mz).neg())                 # fld [54]; fchs; fstp [54]
    else:
        vb = F(my)
    cA = f32(X.fcos(va))                      # fld st(1); fcos; fstp [20]
    cB = f32(X.fcos(vb))
    cC = f32(X.fcos(F(mz)))
    sA = f32(X.fsin(va))
    sB = f32(X.fsin(vb))
    sC = X.fsin(F(mz))                        # queda en ST0 sin redondear
    t10 = f32(mul(F(cC), F(cA)))
    t14 = f32(mul(F(cA), sC))
    t18 = f32(mul(F(sA), F(cC)))
    t54 = f32(mul(F(sA), sC))
    out = {}

    def put(r, cc, v):
        out[(r, cc)] = f32(v)
    if rep:
        put(i, i, F(cB))
        put(i, j, mul(F(sB), F(sA)))
        put(i, k, mul(F(sB), F(cA)))
        put(j, i, mul(sC, F(sB)))
        put(j, j, sub(F(t10), mul(F(t54), F(cB))))
        put(j, k, sub(mul(F(t14), F(cB)).neg(), F(t18)))
        put(k, i, mul(F(sB), F(cC)).neg())
        put(k, j, add(mul(F(t18), F(cB)), F(t14)))
        put(k, k, sub(mul(F(t10), F(cB)), F(t54)))
    else:
        put(i, i, mul(F(cC), F(cB)))
        put(i, j, sub(mul(F(t18), F(sB)), F(t14)))
        put(i, k, add(mul(F(t10), F(sB)), F(t54)))
        put(j, i, mul(sC, F(cB)))
        put(j, j, add(mul(F(t54), F(sB)), F(t10)))
        put(j, k, sub(mul(F(t14), F(sB)), F(t18)))
        put(k, i, F(sB).neg())
        put(k, j, mul(F(sA), F(cB)))
        put(k, k, mul(F(cB), F(cA)))
    offs = [0, 4, 8, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28]
    for (r, cc), bits in out.items():
        A.wr(o + offs[3 * r + cc], 4, bits)
    return [], None, []


# ------------------------------------------------------------------------------------------------ CAttractorScanner
def model_scan(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    typ = a[5]
    A.wr(o + 0x18 + 4 * typ, 4, a[28]); A.wr(o + 0x40 + 4 * typ, 4, a[29]); A.wr(o + 0x68 + 4 * typ, 4, a[21])
    effect = W.raw(2, a[8])
    for q in range(3):
        W.mem[2].wr(a[8] + 4 * q, 4, a[9 + q])
    W.mem[2].wr(a[8] + 0x34, 4, a[30])
    entity = W.raw(1, a[7]) if a[6] else 0
    if a[6]:
        W.mem[1].wr(a[7] + 0x14, 4, W.raw(1, a[27]))
    ped = W.raw(3, a[16])
    W.mem[3].wr(a[16] + 0x14, 4, W.raw(3, a[17]) if a[15] else 0)
    for q in range(3):
        if a[15]:
            W.mem[3].wr(a[17] + 0x30 + 4 * q, 4, a[18 + q])
        else:
            W.mem[3].wr(a[16] + 4 + 4 * q, 4, a[18 + q])
    W.setg(GLOBAL_RAIN, 4, a[4])
    W.setg(GLOBAL_RADII + 4 * a[23], 4, a[22])
    W.snap()
    gorder = [(GLOBAL_RAIN, 4), (GLOBAL_RADII + 4 * a[23], 4)]
    if typ == 7:
        marks.append('type7')
    elif typ == 4:
        marks.append('type4')
    rain = X.from_f32(a[4])
    lim = X.from_f32(RAIN_LIMIT)
    rain_lo = X.compare(rain, lim) in ('lt', 'un')          # fcomp + test ah,1 (C0): menor o no ordenado
    marks.append('rain_lo' if rain_lo else 'rain_hi')
    if rain_lo:
        if typ == 4:
            return [], None, gorder
    elif typ != 4:
        return [], None, gorder
    events = []
    if entity:
        marks.append('entity')
        events.append([0x5334F0, W.tagv(entity), STACK, a[9], a[10], a[11], W.tagv(effect)])
        pos = [X.from_f32(a[12]), X.from_f32(a[13]), X.from_f32(a[14])]
    else:
        marks.append('no_entity')
        pos = [X.from_f32(a[9]), X.from_f32(a[10]), X.from_f32(a[11])]
    mat = W.rd(ped + 0x14, 4)
    pbase = (mat + 0x30) if mat else (ped + 4)
    pp = [X.from_f32(W.rd(pbase + 4 * q, 4)) for q in range(3)]
    d = [X.sub(pp[q], pos[q]) for q in range(3)]
    dx, dy, dz = d
    s = X.add(X.add(X.mul(dz, dz), X.mul(dy, dy)), X.mul(dx, dx))        # (dz*dz + dy*dy) + dx*dx, cada paso a 24 bits
    sbits = X.to_f32(s)
    field = X.from_f32(W.rd(this + 0x68 + 4 * typ, 4))
    if X.compare(s, field) != 'lt':
        marks.append('dist_ge')
        return events, None, gorder
    marks.append('dist_lt')
    if typ == 7:
        events.append([0x6F9F60, W.tagv(effect)])
        radius = X.from_f32(a[22])
        if X.compare(radius, X.from_f32(0)) == 'lt':
            marks.append('radius_neg')
        else:
            r2 = X.mul(radius, radius)
            if X.compare(X.from_f32(sbits), r2) != 'lt':
                marks.append('radius_cut')
                return events, None, gorder
    events.append([0x5EE190])
    events.append([0x5EBB00, W.tagv(a[24]), W.tagv(effect), W.tagv(entity)])
    if (a[25] & 0xFF) == 0:
        marks.append('no_slot')
        return events, None, gorder
    marks.append('has_slot')
    if entity:
        events.append([0x59BBC0, STACK, W.tagv(W.rd(entity + 0x14, 4))])
    else:
        events.append([0x59AED0, STACK, 0x3F800000])
    events.append([0x5EA220, W.tagv(effect), STACK, 0, W.tagv(ped)])
    if a[26] & 0xFF:
        marks.append('approach')
        A.wr(o + 0x18 + 4 * typ, 4, entity)
        A.wr(o + 0x68 + 4 * typ, 4, sbits)
        A.wr(o + 0x40 + 4 * typ, 4, effect)
    else:
        marks.append('no_approach')
    return events, None, gorder


def model_best(c, W, A, fn, a, o, marks):
    this = W.raw(0, o)
    for q in range(10):
        A.wr(o + 0x18 + 4 * q, 4, a[4 + q]); A.wr(o + 0x40 + 4 * q, 4, a[14 + q]); A.wr(o + 0x68 + 4 * q, 4, a[24 + q])
    pe = W.raw(0, o + a[35]) if a[34] == 0 else W.raw(a[34], a[35])
    pn = W.raw(0, o + a[37]) if a[36] == 0 else W.raw(a[36], a[37])
    W.snap()
    if a[34] == 0 or a[36] == 0 or pe == pn:
        marks.append('alias_out')
    W.wr(pe, 4, 0)
    W.wr(pn, 4, 0)
    if W.rd(this + 0x28, 4):
        marks.append('refuge')
        W.wr(pe, 4, W.rd(this + 0x50, 4))
        W.wr(pn, 4, W.rd(this + 0x28, 4))
        return [], None, []
    marks.append('scan')
    best = X.from_f32(FLT_MAX)
    picked = False
    for q in range(10):
        field = X.from_f32(W.rd(this + 0x68 + 4 * q, 4))
        if q == 0:
            ok = X.compare(field, X.from_f32(FLT_MAX)) == 'lt'
        else:
            ok = X.compare(best, field) == 'gt'
        if ok and W.rd(this + 0x40 + 4 * q, 4):
            best = X.from_f32(W.rd(this + 0x68 + 4 * q, 4))
            W.wr(pe, 4, W.rd(this + 0x40 + 4 * q, 4))
            W.wr(pn, 4, W.rd(this + 0x18 + 4 * q, 4))
            picked = True
    marks.append('picked' if picked else 'none_picked')
    return [], None, []


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A = W.mem[0]
    o = a[0]
    A.wr(o, 4, a[3])
    unmodeled = False
    try:
        if k == CONST:
            events, ret, gorder = model_const(c, W, A, fn, a, o)
        elif k in (CLONE, DEATH, GROUP, HSCLONE, LMCLONE):
            events, ret, gorder = model_clone(c, W, A, fn, a, o, marks)
        elif k == LMCTORID:
            events, ret, gorder = model_lm_ctor_id(c, W, A, fn, a, o, marks)
        elif k == LMCTORNAME:
            events, ret, gorder = model_lm_ctor_name(c, W, A, fn, a, o, marks)
        elif k == LMSTART:
            events, ret, gorder = model_lm_start(c, W, A, fn, a, o, marks)
        elif k == SDD:
            events, ret, gorder = model_sdd(c, W, A, fn, a, o, marks)
        elif k == BODYONLY:
            events, ret, gorder = model_body_only(c, W, A, fn, a, o, marks)
        elif k == LMPROCESS:
            events, ret, gorder = model_lm_process(c, W, A, fn, a, o, marks)
        elif k == HSDTOR:
            events, ret, gorder = model_hs_dtor(c, W, A, fn, a, o, marks)
        elif k == HSABORT:
            events, ret, gorder = model_hs_abort(c, W, A, fn, a, o, marks)
        elif k == INTERIOR:
            events, ret, gorder = model_interior(c, W, A, fn, a, o, marks)
        elif k == IGCTOR:
            events, ret, gorder = model_ig(c, W, A, fn, a, o, marks)
        elif k == EULERTO:
            events, ret, gorder = model_euler_to(c, W, A, fn, a, o, marks)
        elif k == EULERFROM:
            events, ret, gorder = model_euler_from(c, W, A, fn, a, o, marks)
        elif k == SCAN:
            events, ret, gorder = model_scan(c, W, A, fn, a, o, marks)
        elif k == BEST:
            events, ret, gorder = model_best(c, W, A, fn, a, o, marks)
        else:
            raise RuntimeError(k)
    except X.Unmodeled as why:
        unmodeled = str(why)
        events, ret, gorder = [], None, []
    out = {'events': events, 'changes': {'A': A.changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes(), 'D': W.mem[3].changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}
    if unmodeled:
        out['unmodeled'] = unmodeled
        out['marks'] = marks + ['unmodeled']
    return out


def events_equal(observed, expected):
    """igualdad de eventos; un argumento CRUDO (no puntero) cuyo valor cae en la pila del hilo del juego (0x400000..0xFFFFFF) el banco lo etiqueta como puntero a la pila (0x7F000000): se acepta esa etiqueta
    cuando el modelo espera un valor crudo en ese intervalo (los dos lados anotan lo mismo)"""
    if observed == expected:
        return True
    if len(observed) != len(expected):
        return False
    for ov, ev in zip(observed, expected):
        if ov == ev:
            continue
        if len(ov) != len(ev) or ov[0] != ev[0]:
            return False
        for x, y in zip(ov, ev):
            if x != y and not (x == STACK and 0x400000 <= y < 0x1000000):
                return False
    return True


def check_side(c, o, e, side_row):
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    if e.get('unmodeled'):
        return None          # sin modelo exacto (NaN, infinitos o argumentos fuera del rango de fsin/fcos): solo se exige la igualdad EXE/DLL y la x87 vacia
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if not events_equal(o['events'], e['events']):
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
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
