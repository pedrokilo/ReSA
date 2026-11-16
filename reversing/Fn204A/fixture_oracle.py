"""Oraculo independiente (Python) de Fn204A: reproduce, desde la ENTRADA del caso (parametros, argumentos y guion de la escena) y la lectura del desensamblado del EXE, lo que debe observarse de las 28 funciones del lote 204A
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su
imagen inicial y aplica el mismo guion de escrituras que el banco. La aritmetica x87 (precision de 24 bits, exponente ampliado, redondeo al mas cercano) se reproduce con un emulador exacto (clase X87: racionales, NaN
con las reglas de la x87, ceros con signo e infinitos) y cada secuencia de instrucciones sigue el desensamblado (`reversing/_handoff/dis.sh`).

Funciones (EXE), resumen del desensamblado:
   0 0x66AB40 SetUpIK: GetPad(0), FindPlayerPed(-1); si peaton == jugador y word +10Eh del pad == 0 sale; GetIsOnScreen (AL); bit 0 de +50h; GetLookAtEntity; GetTaskSecondary(5); padre (+4): GetTaskType dos veces (0x395, 0x3AB);
      d = objetivo (+20h) - posicion (matriz +30h o +4); |d|^2 (SquaredMagnitude, ST0) > 9.0; Normalise; (dz*fz + dy*fy) + dx*fx < [0xC18D48]; LookAt con (2dx + tx, ty + 2dy, (2dz + pz) + 0.61); bit 0 de +50h
   1 0x66AD10 QuitIK: con el bit 0 de +50h: IsLooking(ped); si AL: AbortLookAt(ped, 250)
   2 0x66AD40 MakeAbortable: QuitIK en linea; al = 1          3 0x66AD70 CreateNextSubTask: QuitIK en linea; eax = 0
   4 0x66ADE0 ControlSubTask: sin entidad (+0Ch): QuitIK y 0; si no SetUpIK, contador +274h de la inteligencia (con signo) > [86C938h] = 30: QuitIK y 0; (dz*dz + dx*dx) + dy*dy > 225.0: QuitIK y 0; si no, [+8]
   5 0x66D0C0 Clone: CTask::operator new(0x54); si nulo, 0; si no 0x66AA20(ecx = nuevo, [+10h], [+0Ch], &[+20h])
   6 0x672530 CreateFirstSubTask: copia la posicion del peaton a +14h; 0x66FD30; si falla QuitIK y 0; operator new(0x30); CTaskSimpleGoToPoint (0x667CD0) con ([+10h], &[+2Ch], 0.5, 0, 0)
   7 0x676D10 / 9 0x6772C0: destructores con borrado de CTaskInteriorSitInChair / LieInBed: cuerpo (0x675C90 / 0x675E90) y, con el bit 0, CTask::operator delete (0x61A5B0); eax = this
   8 0x676D30 ProcessPed: ver sit_process()
  10 0x605FB0, 11 0x606000, 13 0x606050: [ecx] = 86CD40h; salta a 0x5FF9D0 (CEntityScanner::Clear)       12 0x606010 CObjectScanner constructor
  14 0x606580 escaner de colisiones con peatones (ver ped_coll)      15 0x606BA0 CreateAcquaintanceEvent (ver acq_event)
  16 0x4ADE90 [ecx] = 85AB38h (conserva EAX, ECX, EDX)       17 0x4AE2D0 CEventPotentialGetRunOver::AffectsPed
  18..27 destructores con borrado de eventos (+50h cada uno): cuerpo; con el bit 0 libera la ranura en el pool de eventos [B744ACh] (elementos de 44h); eax = this

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexAvoidEntity::SetUpIK", "CTaskComplexAvoidEntity::QuitIK", "CTaskComplexAvoidEntity::MakeAbortable", "CTaskComplexAvoidEntity::CreateNextSubTask", "CTaskComplexAvoidEntity::ControlSubTask",
         "CTaskComplexAvoidEntity::Clone", "CTaskComplexAvoidEntity::CreateFirstSubTask", "CTaskInteriorSitInChair::ScalarDeletingDestructor", "CTaskInteriorSitInChair::ProcessPed",
         "CTaskInteriorLieInBed::ScalarDeletingDestructor", "CVehicleScanner::DestructorBody", "CPedScanner::DestructorBody", "CObjectScanner::ConstructorExe", "CObjectScanner::DestructorBody",
         "CEventScanner::ScanForPedPotentialCollisionEvents", "CPedAcquaintanceScanner::CreateAcquaintanceEvent", "CEventDeath::DestructorBody", "CEventPotentialGetRunOver::AffectsPed",
         "CEventDeadPed::ScalarDeletingDestructor", "CEventGroupEvent::ScalarDeletingDestructor", "CEventPotentialGetRunOver::ScalarDeletingDestructor", "CEventPotentialWalkIntoVehicle::ScalarDeletingDestructor",
         "CEventPotentialWalkIntoObject::ScalarDeletingDestructor", "CEventPotentialWalkIntoPed::ScalarDeletingDestructor", "CEventVehicleHitAndRun::ScalarDeletingDestructor",
         "CEventPedEnteredMyVehicle::ScalarDeletingDestructor", "CEventChatPartner::ScalarDeletingDestructor", "CEventSexyPed::ScalarDeletingDestructor"]
ENTRIES = (0x66AB40, 0x66AD10, 0x66AD40, 0x66AD70, 0x66ADE0, 0x66D0C0, 0x672530, 0x676D10, 0x676D30, 0x6772C0, 0x605FB0, 0x606000, 0x606010, 0x606050, 0x606580, 0x606BA0, 0x4ADE90, 0x4AE2D0,
           0x4B5E00, 0x4B5E50, 0x4B5EA0, 0x4B5EF0, 0x4B5F40, 0x4B5F90, 0x4B5FE0, 0x4B6030, 0x4B60D0, 0x4B6120)
EV_BODIES = (0x4ADF70, 0x4AE070, 0x4AE270, 0x4AE3C0, 0x4AE670, 0x4AE7A0, 0x4AEA50, 0x4AEB90, 0x4AED60, 0x4AEEA0)
(SETUPIK, QUITIK, MAKEABORT, CREATENEXT, CONTROL, CLONE, CREATEFIRST, SITSDD, SITPROCESS, LIESDD, VEHSCDTOR, PEDSCDTOR, OBJSCCTOR, OBJSCDTOR, PEDCOLL, ACQEVENT, DEATHBODY, RUNOVER,
 EVSDD0) = range(19)
SPAN = (0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x30, 0x30, 0x30, 4, 4, 0x50, 4, 0, 0, 4, 0x18) + (0,) * 10
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
THR, EVPOOL_PTR, TIMER, RIOT = 0xC18D48, 0xB744AC, 0xB7CB84, 0x96913F
GLOBAL_REGIONS = {SETUPIK: [(THR, 4)], CONTROL: [(THR, 4)], SITPROCESS: [(TIMER, 4)], ACQEVENT: [(RIOT & ~3, 4), (TIMER, 4)]}
for _i in range(10):
    GLOBAL_REGIONS[EVSDD0 + _i] = [(EVPOOL_PTR, 4)]
PED_ALIVE, INTEL_TASK, INTEL_MOVE, TASK_ISGO, INTEL_OFF, VT_OFF = 0x484, 0x100, 0x104, 0x1C, 0x200, 0xF00
MARKER = 0xA5A5A5A5
IK = 0xC15448
TAG_STACK = 0x7F000000


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


def f32bits(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


# ------------------------------------------------------------------------------------------------ x87 (precision de 24 bits)
# Valores: ('f', Fraction) finito no nulo; ('z', signo); ('i', signo); ('n', signo, significando de 64 bits con el bit 63 explicito y el bit 62 = "silencioso")
QUIET = 1 << 62
INDEFINITE = ('n', 1, (1 << 63) | QUIET)


def r24(x):
    """redondea un racional exacto a 24 bits de mantisa (al mas cercano, empate al par); el exponente de la x87 es ampliado"""
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


def round_f32(x):
    """bits de un racional exacto tras `fstp dword`: float de 32 bits (con denormales, desbordamiento a infinito)"""
    sign = 1 if x < 0 else 0
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23 if e >= -126 else -149
    scaled = a / (Fraction(2) ** q) if q >= 0 else a * (Fraction(2) ** -q)
    fl = scaled.numerator // scaled.denominator
    rem = scaled - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    if q == -149:
        return (sign << 31) | fl            # fl == 1 << 23 es justo el menor normal
    if fl >= 1 << 24:
        fl >>= 1
        q += 1
    be = q + 23 + 127
    if be >= 255:
        return (sign << 31) | 0x7F800000
    return (sign << 31) | (be << 23) | (fl & 0x7FFFFF)


def from_f32(bits):
    """valor de un float de memoria (un NaN senalizante conserva su condicion)"""
    bits &= M32
    s, ex, m = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if ex == 255:
        if m == 0:
            return ('i', s)
        return ('n', s, (1 << 63) | (m << 40))
    if ex == 0:
        if m == 0:
            return ('z', s)
        return ('f', Fraction(m, 1 << 149) * (-1 if s else 1))
    return ('f', Fraction((1 << 23) | m) * (Fraction(2) ** (ex - 150)) * (-1 if s else 1))


def to_f32(v):
    if v[0] == 'n':
        return (v[1] << 31) | 0x7F800000 | ((v[2] >> 40) & 0x7FFFFF)
    if v[0] == 'i':
        return (v[1] << 31) | 0x7F800000
    if v[0] == 'z':
        return v[1] << 31
    return round_f32(v[1])


def quieted(v):
    return ('n', v[1], v[2] | QUIET) if v[0] == 'n' else v


def nan_result(a, b):
    na, nb = a[0] == 'n', b[0] == 'n'
    if na and nb:
        qa, qb = bool(a[2] & QUIET), bool(b[2] & QUIET)
        if qa != qb:
            return quieted(a if qa else b)
        return quieted(a if a[2] >= b[2] else b)
    return quieted(a if na else b)


def sign_of(v):
    if v[0] in ('z', 'i'):
        return v[1]
    return 1 if v[1] < 0 else 0


def neg(v):
    if v[0] in ('z', 'i'):
        return (v[0], 1 - v[1])
    if v[0] == 'n':
        return ('n', 1 - v[1], v[2])
    return ('f', -v[1])


def arith(op, a, b):
    if a[0] == 'n' or b[0] == 'n':
        return nan_result(a, b)
    if op == 'sub':
        return arith('add', a, neg(b))
    if op == 'add':
        if a[0] == 'i' or b[0] == 'i':
            if a[0] == 'i' and b[0] == 'i' and a[1] != b[1]:
                return INDEFINITE
            return a if a[0] == 'i' else b
        if a[0] == 'z' and b[0] == 'z':
            return ('z', 1 if (a[1] and b[1]) else 0)
        if a[0] == 'z':
            return b
        if b[0] == 'z':
            return a
        x = a[1] + b[1]
        return ('z', 0) if x == 0 else ('f', r24(x))
    if op == 'mul':
        s = sign_of(a) ^ sign_of(b)
        if a[0] == 'i' or b[0] == 'i':
            if a[0] == 'z' or b[0] == 'z':
                return INDEFINITE
            return ('i', s)
        if a[0] == 'z' or b[0] == 'z':
            return ('z', s)
        return ('f', r24(a[1] * b[1]))
    if op == 'div':
        s = sign_of(a) ^ sign_of(b)
        if a[0] == 'i':
            return INDEFINITE if b[0] == 'i' else ('i', s)
        if b[0] == 'i':
            return ('z', s)
        if b[0] == 'z':
            return INDEFINITE if a[0] == 'z' else ('i', s)
        if a[0] == 'z':
            return ('z', s)
        return ('f', r24(a[1] / b[1]))
    raise ValueError(op)


def fsqrt(a):
    if a[0] == 'n':
        return quieted(a)
    if a[0] == 'z':
        return a
    if a[0] == 'i':
        return INDEFINITE if a[1] else a
    if a[1] < 0:
        return INDEFINITE
    x = a[1]
    e = (x.numerator.bit_length() - x.denominator.bit_length()) // 2
    m = x / (Fraction(4) ** e)
    big = 1 << 220
    scaled = m * big * big
    isq = math.isqrt(scaled.numerator // scaled.denominator)
    exact = Fraction(isq * isq) == scaled
    res = Fraction(isq, big) if exact else Fraction(2 * isq + 1, 2 * big)
    return ('f', r24(res * (Fraction(2) ** e)))


def cmp_flags(a, b):
    """(C0, C2, C3) de fcom(a, b)"""
    if a[0] == 'n' or b[0] == 'n':
        return 1, 1, 1

    def key(v):
        if v[0] == 'z':
            return Fraction(0)
        if v[0] == 'i':
            return float('inf') if v[1] == 0 else float('-inf')
        return v[1]
    ka, kb = key(a), key(b)
    if ka > kb:
        return 0, 0, 0
    if ka < kb:
        return 1, 0, 0
    return 0, 0, 1


def parity_even(v):
    return bin(v & 0xFF).count('1') % 2 == 0


def ah(c0, c2, c3):
    return c0 | (c2 << 2) | (c3 << 6)


class X87:
    def __init__(self):
        self.st = []

    def fld32(self, bits):
        self.st.append(quieted(from_f32(bits)))

    def fld_st(self, i):
        self.st.append(self.st[-1 - i])

    def fst32(self):
        return to_f32(self.st[-1])

    def fstp32(self):
        return to_f32(self.st.pop())

    def pop(self):
        self.st.pop()

    def fxch(self, i):
        self.st[-1], self.st[-1 - i] = self.st[-1 - i], self.st[-1]

    def m32(self, op, bits):                       # fadd/fsub/fmul/fdiv dword ptr [m]: st0 = st0 op m
        self.st[-1] = arith(op, self.st[-1], from_f32(bits))

    def m32r(self, op, bits):                      # fdivr dword ptr [m]: st0 = m op st0
        self.st[-1] = arith(op, from_f32(bits), self.st[-1])

    def st_op(self, op, dst, src):                 # fmul st(dst), st(src), fadd st, st ...
        self.st[-1 - dst] = arith(op, self.st[-1 - dst], self.st[-1 - src])

    def op_p(self, op, i=1):                       # faddp st(i), st
        self.st[-1 - i] = arith(op, self.st[-1 - i], self.st[-1])
        self.st.pop()

    def fsqrt(self):
        self.st[-1] = fsqrt(self.st[-1])

    def fabs(self):
        v = self.st[-1]
        self.st[-1] = ('z', 0) if v[0] == 'z' else (('i', 0) if v[0] == 'i' else (('n', 0, v[2]) if v[0] == 'n' else ('f', abs(v[1]))))

    def fcom_m32(self, bits, pop=False):
        r = cmp_flags(self.st[-1], from_f32(bits))
        if pop:
            self.st.pop()
        return r

    def fcompp(self):
        r = cmp_flags(self.st[-1], self.st[-2])
        self.st.pop()
        self.st.pop()
        return r


def sqmag(x, bits):
    """0x406DA0: deja x*x + y*y + z*z (sin redondear a float) en ST0"""
    x.fld32(bits[2]); x.fld32(bits[1]); x.fld32(bits[0])
    x.fld_st(0); x.st_op('mul', 0, 1)
    x.fld_st(2); x.st_op('mul', 0, 3)
    x.op_p('add')
    x.fld_st(3); x.st_op('mul', 0, 4)
    x.op_p('add')
    x.st[-4] = x.st[-1]                              # fstp st(3)
    x.st.pop()
    x.st.pop()
    x.st.pop()


def normalise(bits):
    """0x59C910 sobre un vector de tres dwords; devuelve los tres dwords resultantes"""
    out = list(bits)
    x = X87()
    sqmag(x, bits)
    c0, c2, c3 = x.fcom_m32(0)                    # contra 0.0
    if not parity_even(ah(c0, c2, c3) & 0x41):
        x.pop()
        out[0] = 0x3F800000
        return out
    x.fsqrt()
    x.m32r('div', 0x3F800000)                      # fdivr [1.0]
    for k in range(3):
        x.fld_st(0)
        x.m32('mul', bits[k])
        out[k] = x.fstp32()
    return out


def fnv_str(s):
    h = 2166136261
    for ch in s:
        h ^= ch
        h = (h * 16777619) & M32
    return h


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
    return 0xE00 <= (v >> 20) <= 0xE04


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
            if (v >> 20) == 0xE04:
                return 0xFA400000 | (v & 0xFF)
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
    """libera la ranura del objeto en el pool cuya estructura esta en `pool`: estado |= 80h y cursor = min con signo"""
    data = W.rd(pool, 4)
    idx = div_index(obj - data, magic, shift)
    states = W.rd(pool + 4, 4)
    W.wr(states + idx, 1, W.rd(states + idx, 1) | 0x80)
    if idx < s32(W.rd(pool + 0xC, 4)):
        W.wr(pool + 0xC, 4, idx)
    return idx


MAGIC_EV = (0x78787879, 5)


# ------------------------------------------------------------------------------------------------ CTaskComplexAvoidEntity
def quit_ik(W, a, this, ped, events):
    """0x66AD10 (y los cuerpos en linea de MakeAbortable, CreateNextSubTask, ControlSubTask y CreateFirstSubTask)"""
    if W.rd(this + 0x50, 1) & 1:
        events.append([0x6181A0, IK, W.tagv(ped)])
        if a[10] & 0xFF:
            events.append([0x618280, IK, W.tagv(ped), 0xFA])


def setup_ik(W, a, this, ped, events, marks):
    pad = W.resolve(a[11])
    events.append([0x53FB70, 0])
    events.append([0x56E210, M32])
    player = ped if a[4] == 0 else (W.resolve(a[12]) if a[4] == 1 else 0)
    if ped == player and W.rd(pad + 0x10E, 2) == 0:
        marks.append('gate_player')
        return
    events.append([0x534540, W.tagv(ped)])
    if (a[5] & 0xFF) == 0:
        marks.append('gate_screen')
        return
    if W.rd(this + 0x50, 1) & 1:
        marks.append('gate_ik_on')
        return
    events.append([0x6181D0, IK, W.tagv(ped)])
    if a[6] != 0:
        marks.append('gate_lookat')
        return
    intel = W.rd(ped + 0x47C, 4)
    events.append([0x681810, W.tagv(intel + 4), 5])
    if a[7] != 0:
        marks.append('gate_secondary')
        return
    parent = W.rd(this + 4, 4)
    if parent:
        events.append([0xD0000004, W.tagv(parent)])
        if a[8] == 0x395:
            marks.append('gate_parent1')
            return
        events.append([0xD0000004, W.tagv(parent)])
        if a[9] == 0x3AB:
            marks.append('gate_parent2')
            return
    mat = W.rd(ped + 0x14, 4)
    pos = mat + 0x30 if mat else ped + 4
    tgt = this + 0x20
    d = []
    for k in range(3):
        x = X87()
        x.fld32(W.rd(tgt + 4 * k, 4))
        x.m32('sub', W.rd(pos + 4 * k, 4))
        d.append(x.fstp32())
    x = X87()
    sqmag(x, d)
    c0, c2, c3 = x.fcom_m32(f32bits(9.0))
    if ah(c0, c2, c3) & 0x41:
        marks.append('gate_dist')
        return
    d = normalise(d)
    if not mat:
        raise ValueError('peaton sin matriz tras la comprobacion de distancia (fuera de los fixtures)')
    x = X87()
    x.fld32(d[2]); x.m32('mul', W.rd(mat + 0x18, 4))
    x.fld32(d[1]); x.m32('mul', W.rd(mat + 0x14, 4))
    x.op_p('add')
    x.fld32(d[0]); x.m32('mul', W.rd(mat + 0x10, 4))
    x.op_p('add')
    c0, c2, c3 = x.fcom_m32(W.rd(THR, 4), pop=True)
    if parity_even(ah(c0, c2, c3) & 5):
        marks.append('gate_dot')
        return
    tx, ty = W.rd(tgt, 4), W.rd(tgt + 4, 4)
    x = X87()
    x.fld32(W.rd(pos + 8, 4))
    x.fld32(d[0]); x.st_op('add', 0, 0)
    x.fld32(d[1]); x.st_op('add', 0, 0)
    x.fld32(d[2]); x.st_op('add', 0, 0)
    az = x.fstp32()
    x.fxch(1)
    x.m32('add', tx)
    lx = x.fstp32()
    x.fld32(ty); x.st_op('add', 0, 1)
    ly = x.fstp32()
    x.pop()
    x.fld32(az); x.st_op('add', 0, 1)
    x.m32('add', f32bits(0.6100000143051147))
    lz = x.fstp32()
    x.pop()
    events.append([0x618970, IK, fnv_str(b'TaskAvoidEntity'), W.tagv(ped), 0, 5000, M32, lx, ly, lz, 0, 0x3E800000, 500, 3, 0])
    W.wr(this + 0x50, 1, W.rd(this + 0x50, 1) | 1)
    marks.append('look_at')


def control_sub_task(W, a, this, ped, events, marks):
    entity = W.rd(this + 0xC, 4)
    if entity == 0:
        quit_ik(W, a, this, ped, events)
        marks.append('no_entity')
        return 0
    setup_ik(W, a, this, ped, events, marks)
    intel = W.rd(ped + 0x47C, 4)
    if s32(W.rd(intel + 0x274, 4)) > 30:
        quit_ik(W, a, this, ped, events)
        marks.append('counter_quit')
        return 0
    em = W.rd(entity + 0x14, 4)
    epos = em + 0x30 if em else entity + 4
    pm = W.rd(ped + 0x14, 4)
    ppos = pm + 0x30 if pm else ped + 4
    x = X87()
    for k in range(3):
        x.fld32(W.rd(ppos + 4 * k, 4))
        x.m32('sub', W.rd(epos + 4 * k, 4))
    # st0 = dz, st1 = dy, st2 = dx
    x.fld_st(0); x.st_op('mul', 0, 1)
    x.fld_st(3); x.st_op('mul', 0, 4)
    x.op_p('add')
    x.fld_st(2); x.st_op('mul', 0, 3)
    x.op_p('add')
    x.fld32(f32bits(7.5)); x.m32('mul', f32bits(7.5)); x.m32('mul', f32bits(4.0))
    c0, c2, c3 = x.fcompp()
    if parity_even(ah(c0, c2, c3) & 5):
        marks.append('dist_keep')
        return W.rd(this + 8, 4)
    quit_ik(W, a, this, ped, events)
    marks.append('dist_quit')
    return 0


# ------------------------------------------------------------------------------------------------ CTaskInteriorSitInChair::ProcessPed
def rand_range(r, lo, hi):
    """0x407180: rand() & FFFFh; fild; fmul [858B14h] (1/32768); fimul (hi - lo); _ftol (trunca); + lo"""
    x = X87()
    x.st.append(('f', Fraction(r & 0xFFFF)) if (r & 0xFFFF) else ('z', 0))
    x.m32('mul', 0x38000000)
    rng = s32((hi - lo) & M32)
    x.st[-1] = arith('mul', x.st[-1], ('f', Fraction(rng)) if rng else ('z', 0))
    v = x.st[-1]
    t = 0 if v[0] == 'z' else int(v[1])
    return (lo + t) & M32


def sit_process(W, a, this, ped, events, marks):
    cnt = {'loaded': 0, 'blend': 0, 'limit': 0}
    CB = 0x675DD0
    clump = W.rd(ped + 0x18, 4)
    TIME = W.rd(TIMER, 4)

    def loaded():
        k = cnt['loaded']
        cnt['loaded'] += 1
        events.append([0x5980F0, 0xBAF670, 0])
        return bool(k < 32 and ((a[4] >> k) & 1))

    def blend(anim_id, delta_bits):
        k = cnt['blend']
        cnt['blend'] += 1
        events.append([0x4D4610, W.tagv(clump), 0x54, anim_id, delta_bits])
        an = W.resolve(0xE0200000 | (0x100 * (a[6 + k] if k < 3 else a[8])))
        W.wr(this + 0x14, 4, an)
        events.append([0x4CEBE0, W.tagv(an), CB, W.tagv(this)])
        return an

    def limit_ret():
        k = cnt['limit']
        cnt['limit'] += 1
        x = X87()
        x.fld32(a[9] if k == 0 else a[11])
        return x.fstp32()

    anim = W.rd(this + 0x14, 4)
    ident = s16(W.rd(anim + 0x2C, 2)) if anim else -1
    events.append([0x5DEC00, W.tagv(ped), 1])
    if W.rd(this + 0x29, 1):
        events.append([0x4D68B0, W.tagv(clump), 0x158])
        if a[5] == 0:
            marks.append('assoc_gone')
            return 1
    if W.rd(this + 0x2A, 1):
        if not loaded():
            marks.append('stop_noanims')
            return 1
        if ident == 0x157:
            W.wr(anim + 0x1C, 4, 0xC1000000)
            marks.append('stop_157')
        elif ident == 0x159:
            if W.rd(this + 0x28, 1) == 0:
                events.append([0x4CEBC0, W.tagv(anim), 0x4D1390, 0])
                blend(0x158, 0x447A0000)
                W.wr(this + 0x28, 1, 1)
                marks.append('stop_159_blend')
                return 0
            marks.append('stop_159_wait')
        elif ident == 0x158:
            W.wr(anim + 0x24, 4, 0x40400000)
            marks.append('stop_158')
        else:
            marks.append('stop_other')
    anim = W.rd(this + 0x14, 4)
    if anim == 0:
        if loaded():
            last = s32(W.rd(this + 0x18, 4))
            if last == -1:
                if W.rd(this + 0x10, 1):
                    dur = s32(W.rd(this + 8, 4))
                    if dur >= 0:
                        W.wr(this + 0x1C, 4, TIME)
                        W.wr(this + 0x20, 4, dur & M32)
                        W.wr(this + 0x24, 1, 1)
                    blend(0x159, 0x447A0000)
                    W.wr(this + 0x28, 1, 1)
                    marks.append('start_instant')
                else:
                    blend(0x157, 0x40800000)
                    marks.append('start_in')
            elif last == 0x157:
                dur = s32(W.rd(this + 8, 4))
                if dur >= 0:
                    W.wr(this + 0x1C, 4, TIME)
                    W.wr(this + 0x20, 4, dur & M32)
                    W.wr(this + 0x24, 1, 1)
                blend(0x159, 0x447A0000)
                W.wr(this + 0x28, 1, 1)
                marks.append('start_out')
            else:
                marks.append('start_none')
        else:
            marks.append('start_noanims')
    else:
        if W.rd(this + 0x28, 1):
            mat = W.rd(ped + 0x14, 4)
            pp = mat + 0x30 if mat else ped + 4
            v = [W.rd(pp + 4 * k, 4) for k in range(3)]
            w = v[2]
            if ident == 0x159:
                marks.append('reloc_159')
                events.append([0x59C890, TAG_STACK, W.tagv(mat), 0xC18C78])
                v = [a[12] & M32, a[13] & M32, a[14] & M32]
                x = X87()
                x.fld32(W.rd(ped + 0x558, 4))
                x.m32('add', f32bits(3.1415927410125732))
                events.append([0x53CB50, x.fstp32()])
                res = limit_ret()
                W.wr(ped + 0x55C, 4, res)
                W.wr(ped + 0x558, 4, res)
                if mat:
                    events.append([0x59B020, W.tagv(mat), res])
                else:
                    W.wr(ped + 0x10, 4, res)
            elif ident == 0x158:
                marks.append('reloc_158')
                events.append([0x59C890, TAG_STACK, W.tagv(mat), 0xC18C78])
                v = [a[12] & M32, a[13] & M32, a[14] & M32]
            else:
                marks.append('reloc_other')
            v[2] = w
            events.append([0x4241C0, W.tagv(ped), v[0], v[1], v[2]])
            W.wr(this + 0x28, 1, 0)
        if W.rd(this + 0x24, 1):
            events.append([0x420E30, W.tagv(this + 0x1C)])
            if a[15] & 0xFF:
                cur = W.rd(this + 0x14, 4)
                if s16(W.rd(cur + 0x2C, 2)) != 0x158:
                    events.append([0x4CEBC0, W.tagv(cur), 0x4D1390, 0])
                    blend(0x158, 0x447A0000)
                    W.wr(this + 0x28, 1, 1)
                    marks.append('timer_blend')
        cur = W.rd(this + 0x14, 4)
        if s16(W.rd(cur + 0x2C, 2)) == 0x157:
            info = W.rd(this + 0xC, 4)
            mat = W.rd(ped + 0x14, 4)
            if not mat:
                raise ValueError('peaton sin matriz en la animacion de entrada (fuera de los fixtures)')
            pos = [W.rd(mat + 0x30 + 4 * k, 4) for k in range(3)]
            ip = [W.rd(info + 4 + 4 * k, 4) for k in range(3)]
            dd = []
            for k in range(2):
                x = X87()
                x.fld32(ip[k])
                x.m32('sub', pos[k])
                dd.append(x.fstp32())
            x = X87()
            x.fld32(ip[2]); x.m32('sub', pos[2])
            dz = x.fst32()
            x.m32('mul', dz)
            x.fld32(dd[1]); x.m32('mul', dd[1])
            x.op_p('add')
            x.fld32(dd[0]); x.m32('mul', dd[0])
            x.op_p('add')
            x.fsqrt()
            lb = x.fst32()
            c0, c2, c3 = x.fcom_m32(f32bits(0.019999999552965164), pop=True)
            if parity_even(ah(c0, c2, c3) & 5):
                x.fld32(f32bits(0.019999999552965164))
                marks.append('shift_cap')
            else:
                x.fld32(lb)
                marks.append('shift_len')
            x.fld32(f32bits(1.0))
            x.m32('div', lb)                                   # st0 = inv, st1 = M
            x.fld32(dd[0]); x.st_op('mul', 0, 1)               # dx * inv
            x.fld32(dd[1]); x.st_op('mul', 0, 2)               # dy * inv
            sy = x.fstp32()
            x.fld32(dz); x.st_op('mul', 0, 2)                  # dz * inv
            sz = x.fstp32()
            x.st_op('mul', 0, 2)                               # (dx * inv) * M
            x.st[-2] = x.st[-1]
            x.pop()                                            # fstp st(1): st0 = X1, st1 = M
            x.fld32(sy); x.st_op('mul', 0, 2)                  # Y1
            x.fld32(sz); x.st_op('mul', 0, 3)                  # Z1
            r = [W.rd(mat + 4 * k, 4) for k in range(3)]
            f = [W.rd(mat + 0x10 + 4 * k, 4) for k in range(3)]
            x.fld32(r[2]); x.st_op('mul', 0, 1)                # rz * Z1
            x.fld32(r[1]); x.st_op('mul', 0, 3)                # ry * Y1
            x.op_p('add')
            x.fld_st(3); x.m32('mul', r[0])                    # X1 * rx
            x.op_p('add')
            W.wr(ped + 0x4D8, 4, x.fstp32())
            x.fld32(f[2]); x.st_op('mul', 0, 1)                # fz * Z1
            x.fld32(f[1]); x.st_op('mul', 0, 3)                # fy * Y1
            x.op_p('add')
            x.fxch(3)
            x.m32('mul', f[0])                                 # X1 * fx
            x.op_p('add', 3)
            x.fxch(2)
            W.wr(ped + 0x4DC, 4, x.fstp32())
            a0, a1 = W.rd(info + 0x10, 4), W.rd(info + 0x14, 4)
            events.append([0x53CBE0, a0, a1, 0, 0])
            xr = X87()
            xr.fld32(a[10])
            events.append([0x53CB50, xr.fstp32()])
            W.wr(ped + 0x55C, 4, limit_ret())
            marks.append('shift')
    cur = W.rd(this + 0x14, 4)
    if cur and s16(W.rd(cur + 0x2C, 2)) == 0x159:
        events.append([0x6181A0, IK, W.tagv(ped)])
        if a[22] & 0xFF:
            marks.append('out_looking')
            return 0
        events.append([0x821B1E])
        if s32(rand_range(a[16], 0, 1000)) > 980:
            events.append([0x598240, 0xBAF670, W.tagv(ped)])
            grp = W.resolve(a[18])
            if grp:
                mat = W.rd(ped + 0x14, 4)
                pp = mat + 0x30 if mat else ped + 4
                v = [W.rd(pp + 4 * k, 4) for k in range(3)]
                events.append([0x594A50, W.tagv(grp), 0, v[0], v[1], v[2], 0x41200000, TAG_STACK, TAG_STACK, TAG_STACK])
                info_out = W.resolve(a[19])
                if info_out:
                    events.append([0x821B1E])
                    dur_r = rand_range(a[17], 10000, 20000)
                    iv = [W.rd(info_out + 4 + 4 * k, 4) for k in range(3)]
                    events.append([0x618970, IK, fnv_str(b'TaskSitInChair'), W.tagv(ped), 0, dur_r, M32, iv[0], iv[1], iv[2], 0, 0x3E800000, 500, 3, 0])
                    marks.append('out_lookat')
                else:
                    marks.append('out_noinfo')
            else:
                marks.append('out_nogroup')
        else:
            marks.append('out_norand')
        return 0
    events.append([0x6181A0, IK, W.tagv(ped)])
    if a[22] & 0xFF:
        events.append([0x618280, IK, W.tagv(ped), 0xFA])
    marks.append('final_abort' if a[22] & 0xFF else 'final_none')
    return 0


# ------------------------------------------------------------------------------------------------ 0x606580 y 0x606BA0
def ev_fields(W, ev, kind, entity_tag=None):
    """campos que registra el falso de CEventGroup::Add / CPedGroupIntelligence::AddEvent de un evento ya construido (ver LiveComparison.hpp)"""
    return ev


def ped_coll(W, a, ped, closest, events, marks):
    if closest == 0:
        marks.append('no_closest')
        return
    if not (W.rd(ped + 0x1C, 1) & 1):
        marks.append('no_collision')
        return
    intel = W.rd(ped + 0x47C, 4)
    events.append([0x6819D0, W.tagv(intel + 4)])
    task = W.rd(intel + 4 + 0xFC, 4)
    if task == 0:
        marks.append('no_task')
        return
    events.append([0x61A360, W.tagv(task)])
    if (W.rd(task + TASK_ISGO, 1)) == 0:
        marks.append('not_goto')
        return
    events.append([0x601D70, W.tagv(intel)])
    if W.rd(intel + 0x104, 4) == 1:
        marks.append('still')
        return
    limit = f32bits(6.25)
    best = 0
    k_int = 0
    mat = W.rd(ped + 0x14, 4)
    if not mat:
        raise ValueError('peaton sin matriz (fuera de los fixtures)')
    ppos = mat + 0x30
    tp = task + 0xC
    for i in range(16):
        other = W.rd(intel + 0x130 + 4 * i, 4)
        if not other:
            continue
        events.append([0x5E0170, W.tagv(other)])
        if (W.rd(other + PED_ALIVE, 1)) == 0:
            marks.append('dead')
            continue
        if not (W.rd(other + 0x1C, 1) & 1):
            marks.append('other_nocoll')
            continue
        om = W.rd(other + 0x14, 4)
        opos = om + 0x30 if om else other + 4
        diff = []
        for k in range(3):
            x = X87()
            x.fld32(W.rd(opos + 4 * k, 4))
            x.m32('sub', W.rd(ppos + 4 * k, 4))
            diff.append(x.fstp32())
        x = X87()
        x.fld32(diff[1]); x.m32('mul', W.rd(mat + 0x14, 4))
        x.fld32(diff[2]); x.m32('mul', W.rd(mat + 0x18, 4))
        x.op_p('add')
        x.fld32(diff[0]); x.m32('mul', W.rd(mat + 0x10, 4))
        x.op_p('add')
        c0, c2, c3 = x.fcom_m32(0, pop=True)
        if ah(c0, c2, c3) & 0x41:
            marks.append('behind')
            continue
        events.append([0x40FD10, TAG_STACK, 0x3F333333] + [W.rd(opos + 4 * k, 4) for k in range(3)] + [0, 0, 0xFF])
        events.append([0x4100E0, TAG_STACK] + [W.rd(ppos + 4 * k, 4) for k in range(3)] + [W.rd(tp + 4 * k, 4) for k in range(3)] + [TAG_STACK, TAG_STACK])
        hit = bool((a[20] >> (k_int % 32)) & 1)
        k_int += 1
        if not hit:
            marks.append('no_hit')
            continue
        x = X87()
        sqmag(x, diff)
        c0, c2, c3 = x.fcom_m32(limit)
        if parity_even(ah(c0, c2, c3) & 5):
            x.pop()
            marks.append('not_closer')
            continue
        limit = x.fstp32()
        best = other
        marks.append('closer')
    if best == 0:
        marks.append('no_best')
        return
    binter = W.rd(best + 0x47C, 4)
    events.append([0x6819D0, W.tagv(binter + 4)])
    t2 = W.rd(binter + 4 + 0xFC, 4)
    create = False
    if t2 == 0:
        create = True
        marks.append('make_no_task')
    else:
        events.append([0x61A360, W.tagv(t2)])
        if W.rd(t2 + TASK_ISGO, 1) == 0:
            create = True
            marks.append('make_not_goto')
        else:
            bm = W.rd(best + 0x14, 4)
            if not bm:
                raise ValueError('candidata sin matriz (fuera de los fixtures)')
            x = X87()
            x.fld32(W.rd(mat + 0x18, 4)); x.m32('mul', W.rd(bm + 0x18, 4))
            x.fld32(W.rd(mat + 0x14, 4)); x.m32('mul', W.rd(bm + 0x14, 4))
            x.op_p('add')
            x.fld32(W.rd(mat + 0x10, 4)); x.m32('mul', W.rd(bm + 0x10, 4))
            x.op_p('add')
            c0, c2, c3 = x.fcom_m32(f32bits(0.9229999780654907), pop=True)
            if c0:
                create = True
                marks.append('make_dot')
            else:
                sp = []
                for base in (ped, best):
                    vs = []
                    for k in range(3):
                        x = X87()
                        x.fld32(f32bits(50.0))
                        x.m32('mul', W.rd(base + 0x44 + 4 * k, 4))
                        vs.append(x.fstp32())
                    vs[2] = 0
                    sp.append(vs)
                x = X87()
                sqmag(x, sp[0])
                sq1 = x.fstp32()
                x = X87()
                sqmag(x, sp[1])
                x.m32('add', f32bits(0.25))
                sq2 = x.fstp32()
                x = X87()
                x.fld32(limit)
                c0, c2, c3 = x.fcom_m32(f32bits(1.0), pop=True)
                if ah(c0, c2, c3) & 0x41:
                    create = True
                    marks.append('make_near')
                else:
                    x = X87()
                    x.fld32(sq2)
                    x.m32('sub', 0)
                    x.fabs()
                    c0, c2, c3 = x.fcom_m32(f32bits(0.009999999776482582), pop=True)
                    if not parity_even(ah(c0, c2, c3) & 5):
                        create = True
                        marks.append('make_still')
                    else:
                        x = X87()
                        x.fld32(sq2)
                        c0, c2, c3 = x.fcom_m32(sq1, pop=True)
                        if ah(c0, c2, c3) & 0x41:
                            create = True
                            marks.append('make_speed')
                        else:
                            marks.append('no_make')
    if not create:
        return
    events.append([0x601D70, W.tagv(intel)])
    ms = W.rd(intel + 0x104, 4)
    events.append([0x571B70, W.tagv(best), TAG_STACK])
    events.append([0x4AB420, W.tagv(intel + 0x68), 13, 0, 0, 1, 0xC8, 0xFFFF] + [W.rd(tp + 4 * k, 4) for k in range(3)] + [W.tagv(best), ms, 0])
    events.append([0x571A00, W.tagv(best), TAG_STACK])
    marks.append('event')


def acq_event(W, a, ped, idn, entity, events, marks):
    if idn > 4:
        marks.append('id_big')
        return 0
    intel = W.rd(ped + 0x47C, 4)

    def base_fields(typ, entity_):
        return [typ, 0, 0, 1, 0xC8, 0xFFFF, W.tagv(entity_)]

    def ctor(entity_):
        if entity_:
            events.append([0x571B70, W.tagv(entity_), TAG_STACK])

    def dtor(entity_):
        if entity_:
            events.append([0x571A00, W.tagv(entity_), TAG_STACK])

    if idn <= 3:
        typ = (39, 38, 72, 37)[idn]
        if idn == 2 and W.rd(entity + 0x598, 4) != 6:
            marks.append('cop_not')
            return 0
        ctor(entity)
        events.append([0x4AB420, W.tagv(intel + 0x68)] + base_fields(typ, entity) + [0])
        dtor(entity)
        marks.append('id%d' % idn)
        return 1 if a[25] else 0
    # id 4
    events.append([0x605550, W.tagv(intel), W.tagv(entity), 0])
    x = X87()
    x.fld32(a[20])
    c0, c2, c3 = x.fcom_m32(0)
    if not (ah(c0, c2, c3) & 0x41):
        # visibilidad > 0
        group = (W.rd(RIOT & ~3, 4) >> 24) != 0
        if not group:
            events.append([0x441C10])
            if a[21] & 0xFF:
                events.append([0x603AF0, W.tagv(ped), W.tagv(entity)])
                group = bool(a[22] & 0xFF)
        ctor(entity)
        if not group:
            events.append([0x4AB420, W.tagv(intel + 0x68)] + base_fields(36, entity) + [0])
            dtor(entity)
            marks.append('hate_direct')
            return 1 if a[25] else 0
        events.append([0x5F7E80, W.tagv(ped)])
        grp = W.resolve(a[23])
        if grp:
            fields = base_fields(36, entity)
            fields[4] = 0x5DE
            events.append([0x5F7470, W.tagv(grp + 0x30)] + fields)
            dtor(entity)
            marks.append('hate_group')
            return a[24] & 0xFF
        fields = base_fields(36, entity)
        fields[4] = 0x3E8
        events.append([0x4AB420, W.tagv(intel + 0x68)] + fields + [0])
        dtor(entity)
        marks.append('hate_kill')
        return 1 if a[25] else 0
    x = X87()
    x.fld32(a[20])
    c0, c2, c3 = x.fcom_m32(0, pop=True)
    if parity_even(ah(c0, c2, c3) & 5):
        marks.append('vis_zero')
        return 0
    ctor(entity)
    em = W.rd(entity + 0x14, 4)
    epos = em + 0x30 if em else entity + 4
    point = [W.rd(epos + 4 * k, 4) for k in range(3)]
    fields = base_fields(63, entity) + [W.rd(TIMER, 4)] + point
    events.append([0x4AB420, W.tagv(intel + 0x68)] + fields + [0])
    dtor(entity)
    marks.append('badly_lit')
    return 1 if a[25] else 0


# ------------------------------------------------------------------------------------------------ marcas y modelo
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
KEYS = {}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + (['this_aligned', 'this_unaligned'] if SPAN[_fn] else [])
KEYS[SETUPIK] += ['gate_player', 'gate_screen', 'gate_ik_on', 'gate_lookat', 'gate_secondary', 'gate_parent1', 'gate_parent2', 'gate_dist', 'gate_dot', 'look_at']
KEYS[CONTROL] += ['no_entity', 'counter_quit', 'dist_keep', 'dist_quit', 'gate_player', 'gate_screen', 'gate_ik_on', 'gate_lookat', 'gate_secondary', 'gate_parent1', 'gate_parent2', 'gate_dist', 'gate_dot', 'look_at']
for _fn in (QUITIK, MAKEABORT, CREATENEXT):
    KEYS[_fn] += ['ik_on', 'ik_off', 'looking', 'not_looking']
KEYS[CLONE] += ['new_null', 'new_ok']
KEYS[CREATEFIRST] += ['route_fail', 'new_null', 'built']
KEYS[SITSDD] += ['flag_set', 'flag_clear']
KEYS[LIESDD] += ['flag_set', 'flag_clear']
KEYS[SITPROCESS] += ['assoc_gone', 'stop_noanims', 'stop_157', 'stop_159_blend', 'stop_159_wait', 'stop_158', 'stop_other', 'start_instant', 'start_in', 'start_out', 'start_none', 'start_noanims', 'reloc_159',
                     'reloc_158', 'reloc_other', 'timer_blend', 'shift', 'shift_cap', 'shift_len', 'out_looking', 'out_lookat', 'out_norand', 'final_abort', 'final_none']
KEYS[PEDCOLL] += ['no_closest', 'no_collision', 'no_task', 'not_goto', 'still', 'dead', 'other_nocoll', 'behind', 'no_hit', 'not_closer', 'closer', 'no_best', 'make_no_task', 'make_dot', 'make_near',
                  'make_speed', 'no_make', 'event']
KEYS[ACQEVENT] += ['id_big', 'id0', 'id1', 'id2', 'cop_not', 'id3', 'hate_direct', 'hate_group', 'hate_kill', 'vis_zero', 'badly_lit']
KEYS[RUNOVER] += ['dead_ped', 'attached', 'in_vehicle', 'no_vehicle', 'boat', 'affects']
for _fn in range(EVSDD0, EVSDD0 + 10):
    KEYS[_fn] += ['flag_set', 'flag_clear', 'was_free', 'was_used', 'lowers_cursor', 'keeps_cursor']
KEYS[EVSDD0 + 1] += ['has_ped', 'has_inner']


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
    W.snap()
    vt_masked = False
    if fn == SETUPIK:
        setup_ik(W, a, this, args[0], events, marks)
    elif fn in (QUITIK, MAKEABORT, CREATENEXT):
        on = W.rd(this + 0x50, 1) & 1
        marks.append('ik_on' if on else 'ik_off')
        if on:
            marks.append('looking' if a[10] & 0xFF else 'not_looking')
        quit_ik(W, a, this, args[0], events)
        ret = 1 if fn == MAKEABORT else (0 if fn == CREATENEXT else None)
    elif fn == CONTROL:
        ret = W.tagv(control_sub_task(W, a, this, args[0], events, marks))
    elif fn == CLONE:
        events.append([0x61A5A0, 0x54])
        mem = W.resolve(a[14])
        if mem == 0:
            marks.append('new_null')
            ret = 0
        else:
            marks.append('new_ok')
            events.append([0x66AA20, W.tagv(mem), W.rd(this + 0x10, 4), W.tagv(W.rd(this + 0xC, 4)), W.tagv(this + 0x20)])
            ret = W.tagv(a[16])
    elif fn == CREATEFIRST:
        ped = args[0]
        mat = W.rd(ped + 0x14, 4)
        pp = mat + 0x30 if mat else ped + 4
        for k in range(3):
            W.wr(this + 0x14 + 4 * k, 4, W.rd(pp + 4 * k, 4))
        events.append([0x66FD30, W.tagv(this), W.tagv(ped)])
        if (a[15] & 0xFF) == 0:
            marks.append('route_fail')
            quit_ik(W, a, this, ped, events)
            ret = 0
        else:
            events.append([0x61A5A0, 0x30])
            mem = W.resolve(a[14])
            if mem == 0:
                marks.append('new_null')
                ret = 0
            else:
                marks.append('built')
                W.wr(mem + 4, 4, 0)
                W.wr(mem + 8, 4, W.rd(this + 0x10, 4))
                for k in range(3):
                    W.wr(mem + 0xC + 4 * k, 4, W.rd(this + 0x2C + 4 * k, 4))
                W.wr(mem + 0x18, 4, 0x3F000000)
                W.wr(mem + 0x1C, 1, W.rd(mem + 0x1C, 1) & 0xC0)
                W.wr(mem + 0x20, 1, W.rd(mem + 0x20, 1) & 0xE0)
                vt_masked = True
                ret = W.tagv(mem)
    elif fn in (SITSDD, LIESDD):
        body = 0x675C90 if fn == SITSDD else 0x675E90
        events.append([body, W.tagv(this), W.rd(this, 4)])
        W.wr(this, 4, a[41])
        flags = args[0] & 0xFF
        marks.append('flag_set' if flags & 1 else 'flag_clear')
        if flags & 1:
            events.append([0x61A5B0, W.tagv(this)])
        ret = W.tagv(this)
    elif fn == SITPROCESS:
        ret = sit_process(W, a, this, args[0], events, marks) & 0xFF
    elif fn in (VEHSCDTOR, PEDSCDTOR, OBJSCDTOR):
        W.wr(this, 4, 0x86CD40)
        events.append([0x5FF9D0, W.tagv(this)])
    elif fn == OBJSCCTOR:
        W.wr(this + 4, 4, 0)
        W.wr(this + 8, 4, 0)
        for k in range(16):
            W.wr(this + 0xC + 4 * k, 4, 0)
        W.wr(this + 0x4C, 4, 0)
        W.wr(this + 8, 4, 16)
        W.wr(this, 4, 0x86CD8C)
        ret = W.tagv(this)
    elif fn == PEDCOLL:
        ped_coll(W, a, args[0], args[1], events, marks)
    elif fn == ACQEVENT:
        ret = acq_event(W, a, args[0], args[1], args[2], events, marks) & 0xFF
    elif fn == DEATHBODY:
        W.wr(this, 4, 0x85AB38)
        ret = [c['eax'] & M32, this, c['edx'] & M32]
    elif fn == RUNOVER:
        ped = args[0]
        events.append([0x5E0170, W.tagv(ped)])
        veh = W.rd(this + 0x14, 4)
        if W.rd(ped + PED_ALIVE, 1) == 0:
            marks.append('dead_ped')
            ret = 0
        elif W.rd(ped + 0xFC, 4) != 0:
            marks.append('attached')
            ret = 0
        elif W.rd(ped + 0x46C, 4) & 0x100:
            marks.append('in_vehicle')
            ret = 0
        elif veh == 0:
            marks.append('no_vehicle')
            ret = 0
        else:
            ret = 1 if W.rd(veh + 0x590, 4) != 5 else 0
            marks.append('affects' if ret else 'boat')
    else:
        i = fn - EVSDD0
        body = EV_BODIES[i]
        flags = args[0] & 0xFF
        marks.append('flag_set' if flags & 1 else 'flag_clear')
        if i == 1:
            ped = W.rd(this + 0xC, 4)
            if ped:
                marks.append('has_ped')
                events.append([0x571A00, W.tagv(ped), W.tagv(this + 0xC)])
            inner = W.rd(this + 0x10, 4)
            if inner:
                marks.append('has_inner')
                events.append([0xD0000000, W.tagv(inner), 1])
            vt_masked = True
        else:
            events.append([body, W.tagv(this)])
        if flags & 1:
            pool = W.rd(EVPOOL_PTR, 4)
            data = W.rd(pool, 4)
            states = W.rd(pool + 4, 4)
            before = s32(W.rd(pool + 0xC, 4))
            idx = div_index(this - data, *MAGIC_EV)
            marks.append('was_free' if (W.rd(states + idx, 1) & 0x80) else 'was_used')
            marks.append('lowers_cursor' if idx < before else 'keeps_cursor')
            release_slot(W, pool, this, *MAGIC_EV)
        ret = W.tagv(this)
    if vt_masked and c.get('vt_arena', 255) != 255:
        W.mem[c['vt_arena']].wr(c['vt_off'], 4, MARKER)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(GLOBAL_REGIONS.get(fn, [])), 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        k = next((i for i, (x, y) in enumerate(zip(o['events'], e['events'])) if x != y), min(len(o['events']), len(e['events'])))
        return 'eventos (primer desajuste %d) %s != %s' % (k, o['events'][k:k + 2], e['events'][k:k + 2])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos salvo los de CEventDeath (conserva EAX, ECX y EDX; comprobados en `ret`)"""
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
