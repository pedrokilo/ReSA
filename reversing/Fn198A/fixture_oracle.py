"""Oraculo independiente (Python) de Fn198A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 31 funciones del lote 198A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico; thiscall: ECX = this).
   0..4 0x638590 0x6385B0 0x639E20 0x65A220 0x66DC70  destructores con borrado: push esi; call CUERPO (632630, 632720, 6389F0, 6593A0, 668630); si byte [esp+8] & 1: call 61A5B0 (delete) con this; eax = this; ret 4
   5  0x65A0F0  igual con el cuerpo 659240 (funcion 15)
   6  0x5FFE60  ScanForVehiclesInRange(ped): IsAlive 5E0170 (ecx = ped); si al = 0 sale; vehiculo [ped+58Ch] y bit 8 de [ped+46Ch] (test ah,1): si byte [ped+484h] == 2: 5FFA20 (ecx = this) con (0, ped);
            despues CleanUpOldReference 571A00 (ecx = [this+4Ch], arg this+4Ch) si no es nulo; [this+4Ch] = [ped+58Ch]; RegisterReference 571B70 (ecx = vehiculo, arg this+4Ch); sin vehiculo o a pie: solo 5FFA20(0, ped)
   7  0x5FFF30  ScanForObjectsInRange(ped): vehiculo y bit 8: sale sin IsAlive; si no: IsAlive y, si vive, 5FFA20(2, ped)
   8  0x6034B0  ScanEntityList(lista, ped): bl = 0; nodo = [lista]; por nodo: entidad = [nodo], siguiente = [nodo+4] (se lee antes); tipo = byte [ent+36h] & 7: 4 (objeto): bl = bit 24 de [ent+140h]; sin bit 2 de [ent+1Ch] ni bit 18 se salta;
            con bit 6 de [ent+140h] se salta; modelo = ms_modelInfoPtrs[movsx word [ent+22h]] (0A9B0C8h + 4 * indice); n = byte [modelo+0Dh]; i = 0..n-1: Get2dEffect 4C4C70 (ecx = modelo, i); si byte [efecto+0Ch] == 3 y
            (bit 0 de [efecto+37h] apagado o bl): 5FFFD0 (ecx = this) con (efecto, entidad, ped)
   9  0x603570  FindClosestPedForEffect(efecto): mejor = FLT_MAX; pool [0B74490h] (+0 objetos, +4 mapa, +8 tamano); i de tamano-1 a 0: byte del mapa & 80h salta; ped = objetos + i * 7C4h;
            GetActiveTask 681720 (ecx = [ped+47Ch] + 4): si hay tarea, tipo por llamada virtual (+10h) == E9h salta; posicion = [ped+14h] ? matriz+30h : ped+4; dx = efecto.x - p.x, dy, dz;
            x87 24 bits: ((dz*dz + dy*dy) + dx*dx) guardado como float; solo sigue si es menor (ordenado) que el mejor; GetIndex 6F9F60; lista de usuarios (0C3A200h + 24h * indice): byte +20h == 0 vale;
            si no: modelo (movsx word [ped+22h]) igual a alguna de las 4 entradas, o entrada == -2 con tipo de peaton ([ped+598h]) igual a [entrada + 10h]; SetScale 59AED0 (ecx = matriz local, 1.0f);
            IsApproachable 5EA220 (efecto, &matriz, 0, ped): si al != 0: mejor = esta distancia y este ped; devuelve el mejor ped
   10 0x603A30  CanScanPed(ped): IsAlive; si [ped+484h] == 2 y byte [this+0Ch] == 0: bl = (bit 8 de [ped+46Ch] y byte [this+0Dh]); 608750 (ped); si el resultado != 0 y byte [this+0Eh]: bl = 1; si bl = 0 -> 0;
            si no: bl = 1. Evento actual 4B8CA0 (ecx = [ped+47Ch] + 38h): sin evento -> bl; GetEventType (virtual, +4) != 24h -> bl; [inteligencia+0C4h] == 0 -> 0; 4B5730 (ecx = evento, ped, 6A4h) -> si 0, 0;
            600CF0 (ecx = inteligencia): distinto de 0 -> bl, si no 0
   11 0x658C20  mov eax,64h        14 0x659230  mov eax,6Ah        30 0x620B80  mov eax,3EEh
   12 0x658C30  cuerpo del destructor de Medic: [this] = 86F518; accidente = [this+18h]: si no nulo y [accidente] != 0 y byte [accidente+5] == 0: byte [accidente+4] = 0; CleanUpOldReference 571A00 (ecx = [this+0Ch], arg this+0Ch) si no nulo,
            igual con +10h; 61A3D0 (~CTaskComplex)
   13 0x6591D0  constructor de PresentID(policia): 61A3B0; [this+0Ch] = policia; [this] = 86F544; RegisterReference 571B70 (ecx = policia, arg this+0Ch) siempre; devuelve this
   15 0x659240  cuerpo del destructor: [this] = 86F544; 571A00 (ecx = [this+0Ch], arg this+0Ch) si no nulo; 61A3D0
   16 0x6592A0  ComputeHeading(ped): posiciones (matriz+30h o +4) de ped y de policia ([this+0Ch]); 53CBE0 (cop.x, cop.y, ped.x, ped.y) -> ST0 guardado como float; 53CB50 (ese float) -> ST0 es el resultado
   17 0x659B60  Clone: new(10h) 61A5A0; si nulo -> 0; constructor 6591D0 (ecx = memoria, [this+0Ch]); devuelve el EAX del constructor
   18 0x65A110  CreateSubTask(tipo, ped): 19Dh: new(34h); 61ABE0 (ecx = memoria; 0, 8Eh, 40800000h, 0C0800000h, 0BB8h, 19Dh, 85A2C4h, 0); [memoria] = 85A29Ch; devuelve la memoria;
            386h: new(18h); ComputeHeading 6592A0 (ecx = this, ped) -> float; 667E20 (ecx = memoria; heading, 0.5f, 0.2f); devuelve el EAX del constructor; otro (incluido 516h): 0
   19 0x65AFE0  CreateNextSubTask(ped): tipo = virtual (+10h) de [this+8]: 19Dh -> CreateSubTask(516h); 386h -> Say 5EFFE0 (ecx = ped; 45h, 0, 1.0f, 0, 0, 0) y CreateSubTask(19Dh); otro -> 0
   20 0x65B050  CreateSubTask(386h, ped)      21 0x65B070  si [this+0Ch] != 0: [this+8]; si no CreateSubTask(516h, ped)
   22 0x46FEE0  GetSeekPos(salida, ped): llamada virtual ranura 0 del calculador [this+40h] (ecx = this+40h) con (ped, [this+0Ch], temporal local); copia los 3 dwords del temporal a la salida; devuelve la salida
   23 0x470070  [ped+46Ch] = ([ped+46Ch] & ~400000h) | ((byte arg << 22) & 400000h)
   24 0x4700E0  409C90(indice + 6676h)        25 0x470100  copia 6 dwords a [this+0..17h]; devuelve this
   26 0x668240  [this] = 86FDA8; jmp 61A3D0
   27 0x668250  SelectMoveState(tarea, ped, r1, r2): posicion del ped (matriz+30h o +4); dx = x - [tarea+0Ch], dy = y - [tarea+10h]; dist = dx*dx + dy*dy (x87 24 bits); r1*r1; fcompp: dist < r1^2 ordenado -> [tarea+8] = 4;
            si no dist < r2^2 ordenado -> 6; si no 7
   28 0x6682D0  CreateSubTask(tipo, ped): tipo con signo > 2C0h: 384h -> GO_TO_POINT, otro 0; 2C0h -> LEAVE_CAR; 0CAh -> PAUSE; 0CBh -> STAND_STILL; otro 0
            LEAVE_CAR: new(34h); 63B8C0 (memoria; [ped+58Ch], 0, 0, 1, 0); PAUSE: new(18h); 48E750 (memoria; 1); GO_TO_POINT: new(30h); 667CD0 (memoria; [this+0Ch], this+10h, [this+1Ch], 0, byte [this+24h] & 1);
            bit 3 de [this+24h]: byte [memoria+20h] |= 10h; modo [this+0Ch] == 6 -> radio 1e8 (4CBEBC20h), == 7 -> 10.0f; SelectMoveState 668250 (ecx = this; memoria, ped, [this+20h], radio) solo con 6 o 7; devuelve la memoria.
            STAND_STILL: si bit 1 de [this+24h] y subtarea [this+8] != 0 y su GetTaskType (virtual +10h) == 384h: 40FE60 (salida local, this+10h, posicion del ped) y 40FDB0 (la salida, matriz+10h o 10h) -> dot (float);
            x87 24 bits: [ped+4DCh] < 0.01 (ordenado) -> 8.0; si no dot <= 0.01 o NaN -> 16.0; si no 50 / (((dot / ([ped+4DCh] * 0.5)) - 1.0) * [0B7CB5Ch]) guardado como float y, si el valor (sin redondear a float) > 16.0, 16.0;
            new(20h); 62F310 (memoria; 7D0h, 0, 1, mezcla); sin esa condicion: new(20h); 62F310 (memoria; 1, 0, 0, 8.0f)
   29 0x620B00  constructor de ReactToAttack(a1..a5): 61A3B0; [this+18h] = a3; [+10h] = a1; [+1Ch] = a4; [this] = 86D868; bytes +0Ch, +0Dh = 0; [+14h] = a2; [+20h] = a5; si a2 != 0: 571B70 (ecx = a2, arg this+14h)

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexWaitAtAttractor::ScalarDeletingDestructor", "CTaskComplexUseAttractor::ScalarDeletingDestructor", "CTaskComplexSequence::ScalarDeletingDestructor",
         "CTaskComplexDriveFireTruck::ScalarDeletingDestructor", "CTaskComplexGoToPointAndStandStillTimed::ScalarDeletingDestructor", "CTaskComplexPresentIDToCop::ScalarDeletingDestructor",
         "CVehicleScanner::ScanForVehiclesInRange", "CObjectScanner::ScanForObjectsInRange", "CAttractorScanner::ScanEntityList", "CAttractorScanner::FindClosestPedForEffect",
         "CPedAcquaintanceScanner::CanScanPed", "CTaskComplexMedicTreatInjuredPed::GetTaskType", "CTaskComplexMedicTreatInjuredPed::DestructorBody", "CTaskComplexPresentIDToCop::Constructor",
         "CTaskComplexPresentIDToCop::GetTaskType", "CTaskComplexPresentIDToCop::DestructorBody", "CTaskComplexPresentIDToCop::ComputeHeading", "CTaskComplexPresentIDToCop::Clone",
         "CTaskComplexPresentIDToCop::CreateSubTask", "CTaskComplexPresentIDToCop::CreateNextSubTask", "CTaskComplexPresentIDToCop::CreateFirstSubTask", "CTaskComplexPresentIDToCop::ControlSubTask",
         "CTaskComplexSeekEntityStandard::GetSeekPos", "CPed::SetKindaStayInSamePlace", "CStreaming::SetMissionDoesntRequireStreamedScript", "CBoundingBox::SetMinMax",
         "CTaskComplexGoToPointAndStandStill::DestructorBodyCopy", "CTaskComplexGoToPointAndStandStill::SelectMoveState", "CTaskComplexGoToPointAndStandStill::CreateSubTask",
         "CTaskComplexReactToAttack::Constructor", "CTaskComplexReactToAttack::GetTaskType"]
ENTRIES = (0x638590, 0x6385B0, 0x639E20, 0x65A220, 0x66DC70, 0x65A0F0, 0x5FFE60, 0x5FFF30, 0x6034B0, 0x603570, 0x603A30, 0x658C20, 0x658C30, 0x6591D0, 0x659230, 0x659240, 0x6592A0, 0x659B60,
           0x65A110, 0x65AFE0, 0x65B050, 0x65B070, 0x46FEE0, 0x470070, 0x4700E0, 0x470100, 0x668240, 0x668250, 0x6682D0, 0x620B00, 0x620B80)
GUARD = 0x40
WIN = 0x1000
WIN_C = 0x4000
VTBL = 0xF00
SEED_AT = 92
SIZES = (GUARD + WIN + GUARD, GUARD + WIN + GUARD, GUARD + WIN_C + GUARD, GUARD + WIN + GUARD)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
EV_TYPE, EV_CALC, EV_EVENT = 0x50000001, 0x50000002, 0x50000003
(SDD, CONST, SCANVEH, SCANOBJ, ENTLIST, CLOSEST, CANSCAN, MEDICDTOR, PRESENTCTOR, PRESENTDTOR, HEADING, CLONE, PRESENTSUB, NEXT, FIRST, CONTROL, SEEKPOS, STAY, STREAM, MINMAX, BODYCOPY, SELECT,
 GOTO, REACTCTOR) = range(24)
KIND = [SDD, SDD, SDD, SDD, SDD, SDD, SCANVEH, SCANOBJ, ENTLIST, CLOSEST, CANSCAN, CONST, MEDICDTOR, PRESENTCTOR, CONST, PRESENTDTOR, HEADING, CLONE, PRESENTSUB, NEXT, FIRST, CONTROL, SEEKPOS, STAY,
        STREAM, MINMAX, BODYCOPY, SELECT, GOTO, REACTCTOR, CONST]
CONSTS = {11: 100, 14: 106, 30: 0x3EE}
SDD_BODY = {0: 0x632630, 1: 0x632720, 2: 0x6389F0, 3: 0x6593A0, 4: 0x668630, 5: 0x659240}
DELETE = 0x61A5B0
MODEL_LO = -3
ENT_STRIDE = 0x150
PED_STRIDE = 0x7C4


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


def signed32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def signed16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


# ------------------------------------------------------------------------------------------------ aritmetica x87 de 24 bits (exacta, con fracciones)
# Un valor es ('nan', bits32 silenciado), ('inf', signo) o ('fin', Fraction, cero_negativo). La x87 trabaja con exponente ampliado: los intermedios no desbordan como un float.
def v_load(bits):
    bits &= M32
    e, f = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    sign = -1 if bits >> 31 else 1
    if e == 255:
        return ('inf', sign) if f == 0 else ('nan', bits | 0x400000)
    if e == 0:
        return ('fin', Fraction(sign * f, 1 << 149), sign < 0 and f == 0)
    return ('fin', Fraction(sign * (f | 0x800000)) * (Fraction(2) ** (e - 150)), False)


def _pow2(e):
    return Fraction(1 << e) if e >= 0 else Fraction(1, 1 << -e)


def _exp(a):
    """e tal que 2^e <= a < 2^(e+1) para una fraccion positiva"""
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if _pow2(e) > a:
        e -= 1
    while _pow2(e + 1) <= a:
        e += 1
    return e


def _round_even(x):
    q = x.numerator // x.denominator
    r = x - q
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and (q & 1)):
        q += 1
    return q


def r24(fr):
    """redondea una fraccion al valor mas cercano con 24 bits de significando (empates al par), sin limite de exponente"""
    if fr == 0:
        return fr
    s = -1 if fr < 0 else 1
    a = abs(fr)
    e = _exp(a)
    q = _round_even(a / _pow2(e - 23))
    return s * q * _pow2(e - 23)


def store32(v):
    """guarda un valor en un float de 32 bits (redondeo al mas cercano, desbordamiento a infinito, denormales) y devuelve los bits"""
    if v[0] == 'nan':
        return v[1] & M32
    if v[0] == 'inf':
        return 0x7F800000 if v[1] > 0 else 0xFF800000
    fr = v[1]
    if fr == 0:
        return 0x80000000 if v[2] else 0
    s = 0x80000000 if fr < 0 else 0
    a = abs(fr)
    e = _exp(a)
    if e < -126:
        q = _round_even(a / _pow2(-149))
        return s | q            # q puede llegar a 1 << 23 (el menor normal): la codificacion es continua
    q = _round_even(a / _pow2(e - 23))
    if q == 1 << 24:
        q >>= 1
        e += 1
    if e > 127:
        return s | 0x7F800000
    return s | ((e + 127) << 23) | (q - (1 << 23))


def _nan_pick(x, y):
    nx, ny = x[0] == 'nan', y[0] == 'nan'
    if nx and ny:
        sx, sy = x[1] & 0x7FFFFF, y[1] & 0x7FFFFF
        return x if sx >= sy else y
    return x if nx else y


def _sgn(v):
    """signo de un valor no NaN (los ceros llevan su signo)"""
    if v[0] == 'inf':
        return v[1]
    if v[1] == 0:
        return -1 if v[2] else 1
    return -1 if v[1] < 0 else 1


def v_add(x, y):
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    if x[0] == 'inf' and y[0] == 'inf':
        return x if x[1] == y[1] else ('nan', 0xFFC00000)
    if x[0] == 'inf':
        return x
    if y[0] == 'inf':
        return y
    t = x[1] + y[1]
    if t == 0:
        return ('fin', Fraction(0), bool(x[2] and y[2]) if (x[1] == 0 and y[1] == 0) else False)
    return ('fin', r24(t), False)


def v_neg(x):
    if x[0] == 'nan':
        return x
    if x[0] == 'inf':
        return ('inf', -x[1])
    return ('fin', -x[1], (not x[2]) if x[1] == 0 else False)


def v_sub(x, y):
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    return v_add(x, v_neg(y))


def v_mul(x, y):
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    zero = lambda v: v[0] == 'fin' and v[1] == 0
    if x[0] == 'inf' or y[0] == 'inf':
        if zero(x) or zero(y):
            return ('nan', 0xFFC00000)
        return ('inf', _sgn(x) * _sgn(y))
    if x[1] == 0 or y[1] == 0:
        return ('fin', Fraction(0), _sgn(x) * _sgn(y) < 0)
    return ('fin', r24(x[1] * y[1]), False)


def v_div(x, y):
    """x / y (fdiv)"""
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    zero = lambda v: v[0] == 'fin' and v[1] == 0
    if x[0] == 'inf' and y[0] == 'inf':
        return ('nan', 0xFFC00000)
    if x[0] == 'inf':
        return ('inf', _sgn(x) * _sgn(y))
    if y[0] == 'inf':
        return ('fin', Fraction(0), _sgn(x) * _sgn(y) < 0)
    if zero(y):
        if zero(x):
            return ('nan', 0xFFC00000)
        return ('inf', _sgn(x) * _sgn(y))
    if zero(x):
        return ('fin', Fraction(0), _sgn(x) * _sgn(y) < 0)
    return ('fin', r24(x[1] / y[1]), False)


def v_cmp(x, y):
    """'lt', 'gt', 'eq' o 'un' (comparacion ordenada de la x87)"""
    if x[0] == 'nan' or y[0] == 'nan':
        return 'un'
    fx = x[1] if x[0] == 'fin' else (math.inf * x[1])
    fy = y[1] if y[0] == 'fin' else (math.inf * y[1])
    if fx < fy:
        return 'lt'
    if fx > fy:
        return 'gt'
    return 'eq'


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


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[SEED_AT + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + SIZES[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == CONST:
        _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    elif _kind == SCANVEH:
        _k += ['dead', 'alive', 'in_vehicle', 'on_foot', 'no_vehicle', 'mission', 'not_mission', 'ref_old', 'ref_none']
    elif _kind == SCANOBJ:
        _k += ['dead', 'alive', 'in_vehicle', 'on_foot', 'no_vehicle']
    elif _kind == ENTLIST:
        _k += ['empty', 'one_node', 'many_nodes', 'object', 'building', 'object_skipped_static', 'object_skipped_exploded', 'bl_set', 'bl_carried', 'effect_attractor', 'effect_other',
               'effect_disabled', 'effect_disabled_enabled', 'model_negative', 'model_zero_effects', 'call_made']
    elif _kind == CLOSEST:
        _k += ['empty_pool', 'free_slot', 'busy_ped', 'dist_not_better', 'userlist_none', 'userlist_model', 'userlist_type', 'userlist_reject', 'approachable', 'not_approachable', 'nan_distance',
               'result_found', 'result_none', 'model_negative']
    elif _kind == CANSCAN:
        _k += ['dead', 'alive', 'mission', 'not_mission', 'script_scan', 'status_set', 'allowed', 'denied', 'no_event', 'event_other', 'event_hate', 'dm_zero', 'resp_false', 'friend_true', 'friend_false']
    elif _kind == MEDICDTOR:
        _k += ['accident', 'no_accident', 'treated_cleared', 'treated_kept', 'ref_c', 'ref_10', 'no_refs']
    elif _kind == PRESENTCTOR:
        _k += ['cop_null', 'cop_set']
    elif _kind == PRESENTDTOR:
        _k += ['cop_null', 'cop_set']
    elif _kind == HEADING:
        _k += ['cop_matrix', 'cop_no_matrix', 'ped_matrix', 'ped_no_matrix', 'result_nan']
    elif _kind == CLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == PRESENTSUB:
        _k += ['hands_up', 'achieve', 'finished', 'other_type', 'mem_null', 'mem_set']
    elif _kind == NEXT:
        _k += ['type_hands_up', 'type_achieve', 'type_other']
    elif _kind == CONTROL:
        _k += ['cop_null', 'cop_set']
    elif _kind == STAY:
        _k += ['bit_set', 'bit_cleared', 'byte_two', 'high_bytes']
    elif _kind == MINMAX:
        _k += ['nan_input', 'inf_input']
    elif _kind == SELECT:
        _k += ['walk', 'run', 'sprint', 'matrix', 'no_matrix', 'nan_input', 'inf_input', 'near_equal_r1', 'near_equal_r2']
    elif _kind == GOTO:
        _k += ['go_to_point', 'leave_car', 'pause', 'stand_still', 'finished', 'stop_exactly', 'blend_8', 'blend_16_dot', 'blend_computed', 'blend_clamped', 'blend_nan', 'bit3', 'mode_6', 'mode_7',
               'matrix', 'no_matrix', 'mem_null']
    elif _kind == REACTCTOR:
        _k += ['attacker_null', 'attacker_set']
    KEYS[_fn] = _k


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf_bits(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    if k != CONST:
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    W = World(c)
    A, B, Cc, D = W.mem
    vt = W.raw(3, VTBL)
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    tag = W.tagv
    A.wr(o, 4, a[3])
    if k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    elif k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k in (SCANVEH, SCANOBJ):
        Cc.wr(a[4] + 0x58C, 4, a[5]); Cc.wr(a[4] + 0x46C, 4, a[6]); Cc.wr(a[4] + 0x484, 1, a[7] & 0xFF)
        if k == SCANVEH:
            A.wr(o + 0x4C, 4, a[8])
        W.snap()
        ped = W.raw(2, a[4])
        veh, flags = a[5], a[6]
        in_vehicle = bool(veh) and bool((flags >> 8) & 1)
        alive = (a[9] & 0xFF) != 0
        marks.append('alive' if alive else 'dead')
        marks.append('in_vehicle' if in_vehicle else 'on_foot')
        if not veh:
            marks.append('no_vehicle')
        if k == SCANVEH:
            events.append([0x5E0170, tag(ped)])
            if alive:
                if in_vehicle:
                    mission = (a[7] & 0xFF) == 2
                    marks.append('mission' if mission else 'not_mission')
                    if mission:
                        events.append([0x5FFA20, tag(this), 0, tag(ped)])
                    old = a[8]
                    if old:
                        events.append([0x571A00, tag(old), tag(this + 0x4C), A.rd(o, 4), A.rd(o + 0x4C, 4)])
                        marks.append('ref_old')
                    else:
                        marks.append('ref_none')
                    A.wr(o + 0x4C, 4, veh)
                    events.append([0x571B70, tag(veh), tag(this + 0x4C), A.rd(o, 4), A.rd(o + 0x4C, 4)])
                else:
                    events.append([0x5FFA20, tag(this), 0, tag(ped)])
        else:
            if not in_vehicle:
                events.append([0x5E0170, tag(ped)])
                if alive:
                    events.append([0x5FFA20, tag(this), 2, tag(ped)])
    elif k == ENTLIST:
        n = a[7]
        B.wr(a[4], 4, W.raw(1, a[5]) if n else 0)
        for i in range(n):
            B.wr(a[5] + 8 * i, 4, W.raw(2, a[6] + ENT_STRIDE * i))
            B.wr(a[5] + 8 * i + 4, 4, W.raw(1, a[5] + 8 * (i + 1)) if i + 1 < n else 0)
        for e in range(5):
            ent = a[6] + ENT_STRIDE * e
            Cc.wr(ent + 0x36, 1, a[8 + 4 * e] & 0xFF); Cc.wr(ent + 0x1C, 4, a[9 + 4 * e]); Cc.wr(ent + 0x140, 4, a[10 + 4 * e]); Cc.wr(ent + 0x22, 2, a[11 + 4 * e] & 0xFFFF)
        for s in range(11):
            D.wr(0x40 * s + 0xD, 1, a[40 + s] & 0xFF)
        for i in range(6):
            D.wr(0x400 + 0x40 * i + 0xC, 1, a[52 + i] & 0xFF); D.wr(0x400 + 0x40 * i + 0x37, 1, a[58 + i] & 0xFF)
        W.snap()
        ped = a[28]
        marks.append('empty' if n == 0 else ('one_node' if n == 1 else 'many_nodes'))
        bl = 0
        for i in range(n):
            typ = a[8 + 4 * i] & 0xFF
            f1c, f140 = a[9 + 4 * i], a[10 + 4 * i]
            idx = signed16(a[11 + 4 * i])
            ent = W.raw(2, a[6] + ENT_STRIDE * i)
            if typ & 7 == 4:
                marks.append('object')
                bl = (f140 >> 24) & 1
                if bl:
                    marks.append('bl_set')
                if not (f1c & 4) and not (f1c & 0x40000):
                    marks.append('object_skipped_static')
                    continue
                if f140 & 0x40:
                    marks.append('object_skipped_exploded')
                    continue
            else:
                marks.append('building')
                if bl:
                    marks.append('bl_carried')
            slot = idx - MODEL_LO
            if idx < 0:
                marks.append('model_negative')
            count = a[40 + slot] & 0xFF
            if count == 0:
                marks.append('model_zero_effects')
            for j in range(count):
                events.append([0x4C4C70, tag(W.raw(3, 0x40 * slot)), j])
                etype = a[52 + j] & 0xFF
                eflag = a[58 + j] & 0xFF
                if etype != 3:
                    marks.append('effect_other')
                    continue
                marks.append('effect_attractor')
                if eflag & 1:
                    marks.append('effect_disabled')
                    if not bl:
                        continue
                    marks.append('effect_disabled_enabled')
                events.append([0x5FFFD0, tag(this), tag(W.raw(3, 0x400 + 0x40 * j)), tag(ent), tag(ped)])
                marks.append('call_made')
    elif k == CLOSEST:
        n = a[7]
        B.wr(a[4], 4, W.raw(2, a[5])); B.wr(a[4] + 4, 4, W.raw(3, a[6])); B.wr(a[4] + 8, 4, n)
        for p in range(5):
            b = 13 + 11 * p
            ped = a[5] + PED_STRIDE * p
            mat, intel, task = 0x100 + 0x40 * p, 0x300 + 0x40 * p, 0x500 + 0x40 * p
            D.wr(a[6] + p, 1, a[8 + p] & 0xFF)
            for kk in range(3):
                Cc.wr(ped + 4 + 4 * kk, 4, a[b + 5 + kk]); D.wr(mat + 0x30 + 4 * kk, 4, a[b + 8 + kk])
            Cc.wr(ped + 0x14, 4, W.raw(3, mat) if a[b + 2] else 0)
            Cc.wr(ped + 0x22, 2, a[b] & 0xFFFF); Cc.wr(ped + 0x598, 4, a[b + 1])
            Cc.wr(ped + 0x47C, 4, W.raw(3, intel))
            D.wr(intel + 0x14, 4, W.raw(3, task) if a[b + 3] else 0)
            if a[b + 3]:
                D.wr(task, 4, vt); D.wr(task + 0x30, 4, 0xE9 if a[b + 3] == 1 else a[b + 4])
        for kk in range(3):
            Cc.wr(0x3000 + 4 * kk, 4, a[68 + kk])
        W.snap()
        effect = W.raw(2, 0x3000)
        best_bits, best, call = 0x7F7FFFFF, 0, 0
        if n == 0:
            marks.append('empty_pool')
        for i in range(n - 1, -1, -1):
            if a[8 + i] & 0x80:
                marks.append('free_slot')
                continue
            b = 13 + 11 * i
            pedp = W.raw(2, a[5] + PED_STRIDE * i)
            intel = W.raw(3, 0x300 + 0x40 * i)
            events.append([0x681720, tag(intel + 4)])
            if a[b + 3]:
                taskp = W.raw(3, 0x500 + 0x40 * i)
                events.append([EV_TYPE, tag(taskp)])
                ttype = 0xE9 if a[b + 3] == 1 else a[b + 4]
                if ttype == 0xE9:
                    marks.append('busy_ped')
                    continue
            pos = [a[b + 8], a[b + 9], a[b + 10]] if a[b + 2] else [a[b + 5], a[b + 6], a[b + 7]]
            ef = [a[68], a[69], a[70]]
            d = [v_sub(v_load(ef[q]), v_load(pos[q])) for q in range(3)]
            s = v_add(v_mul(d[2], d[2]), v_mul(d[1], d[1]))
            s = v_add(s, v_mul(d[0], d[0]))
            dist = store32(s)
            if v_cmp(v_load(dist), v_load(best_bits)) != 'lt':
                marks.append('dist_not_better')
                if is_nan_bits(dist):
                    marks.append('nan_distance')
                continue
            events.append([0x6F9F60, tag(effect)])
            if a[76] & 0xFF == 0:
                marks.append('userlist_none')
                allowed = True
            else:
                model = signed16(a[b]) & M32
                allowed = False
                if any(model == a[72 + q] for q in range(4)):
                    allowed = True
                    marks.append('userlist_model')
                else:
                    for q in range(4):
                        if a[72 + q] == 0xFFFFFFFE and a[77 + q] == a[b + 1]:
                            allowed = True
                            marks.append('userlist_type')
                            break
                if not allowed:
                    marks.append('userlist_reject')
            if not allowed:
                continue
            if signed16(a[b]) < 0:
                marks.append('model_negative')
            events.append([0x59AED0, STACK_TAG, 0x3F800000])
            events.append([0x5EA220, tag(effect), STACK_TAG, 0, tag(pedp)])
            res = (a[81] >> (call & 31)) & 1
            call += 1
            if res:
                marks.append('approachable')
                best_bits, best = dist, pedp
            else:
                marks.append('not_approachable')
        marks.append('result_found' if best else 'result_none')
        ret = tag(best)
    elif k == CANSCAN:
        Cc.wr(a[4] + 0x484, 1, a[5] & 0xFF); Cc.wr(a[4] + 0x46C, 4, a[6]); Cc.wr(a[4] + 0x47C, 4, W.raw(3, a[7]))
        D.wr(a[7] + 0x40, 4, W.raw(1, a[14]) if a[13] else 0); D.wr(a[7] + 0xC4, 4, a[16])
        if a[13]:
            B.wr(a[14], 4, vt); B.wr(a[14] + 0x30, 4, a[15])
        A.wr(o + 0xC, 1, a[8] & 0xFF); A.wr(o + 0xD, 1, a[9] & 0xFF); A.wr(o + 0xE, 1, a[10] & 0xFF)
        W.snap()
        ped = W.raw(2, a[4])
        intel = W.raw(3, a[7])
        alive = (a[11] & 0xFF) != 0
        events.append([0x5E0170, tag(ped)])
        if not alive:
            marks.append('dead')
            ret = 0
        else:
            marks.append('alive')
            ok = True
            bl = 1
            if (a[5] & 0xFF) == 2 and not (a[8] & 0xFF):
                marks.append('mission')
                bl = 1 if (((a[6] >> 8) & 1) and (a[9] & 0xFF)) else 0
                events.append([0x608750, tag(ped)])
                if a[12] != 0 and (a[10] & 0xFF):
                    bl = 1
                    marks.append('status_set')
                if not bl:
                    ok = False
                    marks.append('denied')
                    ret = 0
            else:
                marks.append('script_scan' if (a[5] & 0xFF) == 2 else 'not_mission')
            if ok:
                marks.append('allowed')
                events.append([0x4B8CA0, tag(intel + 0x38)])
                if not a[13]:
                    marks.append('no_event')
                    ret = bl
                else:
                    events.append([EV_EVENT, tag(W.raw(1, a[14]))])
                    if a[15] != 0x24:
                        marks.append('event_other')
                        ret = bl
                    else:
                        marks.append('event_hate')
                        if a[16] == 0:
                            marks.append('dm_zero')
                            ret = 0
                        else:
                            events.append([0x4B5730, tag(W.raw(1, a[14])), tag(ped), 0x6A4])
                            if not (a[17] & 0xFF):
                                marks.append('resp_false')
                                ret = 0
                            else:
                                events.append([0x600CF0, tag(intel)])
                                if a[18] & 0xFF:
                                    marks.append('friend_true')
                                    ret = bl
                                else:
                                    marks.append('friend_false')
                                    ret = 0
    elif k == MEDICDTOR:
        A.wr(o + 0x18, 4, W.raw(1, a[5]) if a[4] else 0)
        B.wr(a[5], 4, a[6]); B.wr(a[5] + 4, 1, a[7] & 0xFF); B.wr(a[5] + 5, 1, a[8] & 0xFF)
        A.wr(o + 0xC, 4, a[9]); A.wr(o + 0x10, 4, a[10])
        W.snap()
        A.wr(o, 4, 0x86F518)
        if a[4]:
            marks.append('accident')
            if B.rd(a[5], 4) != 0 and B.rd(a[5] + 5, 1) == 0:
                B.wr(a[5] + 4, 1, 0)
                marks.append('treated_cleared')
            else:
                marks.append('treated_kept')
        else:
            marks.append('no_accident')
        if a[9]:
            events.append([0x571A00, tag(a[9]), tag(this + 0xC), 0x86F518, a[9]])
            marks.append('ref_c')
        if a[10]:
            events.append([0x571A00, tag(a[10]), tag(this + 0x10), 0x86F518, a[9]])
            marks.append('ref_10')
        if not a[9] and not a[10]:
            marks.append('no_refs')
        events.append([0x61A3D0, tag(this), 0x86F518])
        A.wr(o, 4, a[11])
    elif k == PRESENTCTOR:
        W.snap()
        events.append([0x61A3B0, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o + 0xC, 4, a[4])
        A.wr(o, 4, 0x86F544)
        events.append([0x571B70, tag(a[4]), tag(this + 0xC), 0x86F544, a[4]])
        marks.append('cop_set' if a[4] else 'cop_null')
        ret = tag(this)
    elif k == PRESENTDTOR:
        A.wr(o + 0xC, 4, a[4])
        W.snap()
        A.wr(o, 4, 0x86F544)
        if a[4]:
            events.append([0x571A00, tag(a[4]), tag(this + 0xC), 0x86F544, a[4]])
            marks.append('cop_set')
        else:
            marks.append('cop_null')
        events.append([0x61A3D0, tag(this), 0x86F544])
        A.wr(o, 4, a[5])
    elif k == HEADING:
        A.wr(o + 0xC, 4, W.raw(2, a[4]))
        Cc.wr(a[4] + 0x14, 4, W.raw(3, a[7]) if a[6] else 0)
        Cc.wr(a[5] + 0x14, 4, W.raw(3, a[9]) if a[8] else 0)
        for kk in range(3):
            Cc.wr(a[4] + 4 + 4 * kk, 4, a[10 + kk]); D.wr(a[7] + 0x30 + 4 * kk, 4, a[13 + kk])
            Cc.wr(a[5] + 4 + 4 * kk, 4, a[16 + kk]); D.wr(a[9] + 0x30 + 4 * kk, 4, a[19 + kk])
        W.snap()
        cop = (a[13], a[14]) if a[6] else (a[10], a[11])
        ped = (a[19], a[20]) if a[8] else (a[16], a[17])
        events.append([0x53CBE0, cop[0], cop[1], ped[0], ped[1]])
        events.append([0x53CB50, a[22]])
        ret = a[23]
        marks.append('cop_matrix' if a[6] else 'cop_no_matrix')
        marks.append('ped_matrix' if a[8] else 'ped_no_matrix')
        if is_nan_bits(a[23]):
            marks.append('result_nan')
    elif k == CLONE:
        A.wr(o + 0xC, 4, a[4])
        W.snap()
        events.append([0x61A5A0, 0x10])
        if not a[5]:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[6])
            events.append([0x6591D0, tag(mem), tag(a[4]), B.rd(a[6], 4)])
            B.wr(a[6], 4, a[7])
            ret = tag(mem)
    elif k == PRESENTSUB:
        W.snap()
        t = a[4]
        mem = W.raw(1, a[7]) if a[6] else 0
        if t == 0x19D:
            marks.append('hands_up')
            events.append([0x61A5A0, 0x34])
            if mem:
                marks.append('mem_set')
                events.append([0x61ABE0, tag(mem), 0, 0x8E, 0x40800000, 0xC0800000, 0xBB8, 0x19D, 0x85A2C4, 0])
                B.wr(a[7], 4, a[9])
                B.wr(a[7], 4, 0x85A29C)
                ret = tag(mem)
            else:
                marks.append('mem_null')
                ret = 0
        elif t == 0x386:
            marks.append('achieve')
            events.append([0x61A5A0, 0x18])
            if mem:
                marks.append('mem_set')
                events.append([0x6592A0, tag(this), tag(a[5])])
                events.append([0x667E20, tag(mem), a[8], 0x3F000000, 0x3E4CCCCD])
                B.wr(a[7], 4, a[9])
                ret = tag(mem)
            else:
                marks.append('mem_null')
                ret = 0
        else:
            marks.append('finished' if t == 0x516 else 'other_type')
            ret = 0
    elif k == NEXT:
        A.wr(o + 8, 4, W.raw(2, a[4]))
        Cc.wr(a[4], 4, vt); Cc.wr(a[4] + 0x30, 4, a[5])
        W.snap()
        events.append([EV_TYPE, tag(W.raw(2, a[4]))])
        ped = a[6]
        if a[5] == 0x19D:
            marks.append('type_hands_up')
            events.append([0x65A110, tag(this), 0x516, tag(ped)])
            ret = a[7]
        elif a[5] == 0x386:
            marks.append('type_achieve')
            events.append([0x5EFFE0, tag(ped), 0x45, 0, 0x3F800000, 0, 0, 0])
            events.append([0x65A110, tag(this), 0x19D, tag(ped)])
            ret = a[7]
        else:
            marks.append('type_other')
            ret = 0
        ret = tag(ret)
    elif k == FIRST:
        W.snap()
        events.append([0x65A110, tag(this), 0x386, tag(a[4])])
        ret = tag(a[5])
    elif k == CONTROL:
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 8, 4, a[5])
        W.snap()
        if a[4]:
            marks.append('cop_set')
            ret = tag(a[5])
        else:
            marks.append('cop_null')
            events.append([0x65A110, tag(this), 0x516, tag(a[6])])
            ret = tag(a[7])
    elif k == SEEKPOS:
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x40, 4, vt)
        W.snap()
        out = W.raw(1, a[9])
        events.append([EV_CALC, tag(this + 0x40), tag(a[5]), tag(a[4]), STACK_TAG])
        B.wr(a[9], 4, a[6]); B.wr(a[9] + 4, 4, a[7]); B.wr(a[9] + 8, 4, a[8])
        ret = tag(out)
    elif k == STAY:
        A.wr(o + 0x46C, 4, a[4])
        W.snap()
        state = a[5] & 0xFF
        flags = A.rd(o + 0x46C, 4)
        new = (flags & ~0x400000 & M32) | ((state << 22) & 0x400000)
        A.wr(o + 0x46C, 4, new)
        marks.append('bit_set' if state & 1 else 'bit_cleared')
        if state == 2:
            marks.append('byte_two')
        if a[5] >> 8:
            marks.append('high_bytes')
    elif k == STREAM:
        W.snap()
        events.append([0x409C90, (a[4] + 0x6676) & M32])
    elif k == MINMAX:
        W.snap()
        for q in range(6):
            A.wr(o + 4 * q, 4, a[4 + q])
        ret = tag(this)
        if any(is_nan_bits(a[4 + q]) for q in range(6)):
            marks.append('nan_input')
        if any(is_inf_bits(a[4 + q]) for q in range(6)):
            marks.append('inf_input')
    elif k == BODYCOPY:
        W.snap()
        A.wr(o, 4, 0x86FDA8)
        events.append([0x61A3D0, tag(this), 0x86FDA8])
        A.wr(o, 4, a[4])
    elif k == SELECT:
        B.wr(a[4] + 0xC, 4, a[12]); B.wr(a[4] + 0x10, 4, a[13])
        Cc.wr(a[5] + 0x14, 4, W.raw(3, a[7]) if a[6] else 0)
        Cc.wr(a[5] + 4, 4, a[8]); Cc.wr(a[5] + 8, 4, a[9])
        D.wr(a[7] + 0x30, 4, a[10]); D.wr(a[7] + 0x34, 4, a[11])
        W.snap()
        px, py = (a[10], a[11]) if a[6] else (a[8], a[9])
        dx = v_sub(v_load(px), v_load(a[12]))
        dy = v_sub(v_load(py), v_load(a[13]))
        dist = v_add(v_mul(dx, dx), v_mul(dy, dy))
        r1 = v_mul(v_load(a[14]), v_load(a[14]))
        r2 = v_mul(v_load(a[15]), v_load(a[15]))
        c1 = v_cmp(dist, r1)
        if c1 == 'lt':
            state = 4
            marks.append('walk')
        elif v_cmp(dist, r2) == 'lt':
            state = 6
            marks.append('run')
        else:
            state = 7
            marks.append('sprint')
        B.wr(a[4] + 8, 4, state)
        marks.append('matrix' if a[6] else 'no_matrix')
        ins = [px, py, a[12], a[13], a[14], a[15]]
        if any(is_nan_bits(v) for v in ins):
            marks.append('nan_input')
        if any(is_inf_bits(v) for v in ins):
            marks.append('inf_input')
        if c1 == 'eq' or (dist[0] == 'fin' and r1[0] == 'fin' and abs(dist[1] - r1[1]) <= abs(r1[1]) * Fraction(1, 1 << 22)):
            marks.append('near_equal_r1')
        if dist[0] == 'fin' and r2[0] == 'fin' and abs(dist[1] - r2[1]) <= abs(r2[1]) * Fraction(1, 1 << 22):
            marks.append('near_equal_r2')
    elif k == GOTO:
        sub = W.raw(1, a[5]) if a[4] else 0
        A.wr(o + 8, 4, sub)
        if a[4]:
            B.wr(a[5], 4, vt); B.wr(a[5] + 0x30, 4, a[6])
        A.wr(o + 0xC, 4, a[7])
        for q in range(3):
            A.wr(o + 0x10 + 4 * q, 4, a[8 + q])
        A.wr(o + 0x1C, 4, a[11]); A.wr(o + 0x20, 4, a[12]); A.wr(o + 0x24, 1, a[13] & 0xFF)
        Cc.wr(a[15] + 0x14, 4, W.raw(3, a[17]) if a[16] else 0)
        for q in range(3):
            Cc.wr(a[15] + 4 + 4 * q, 4, a[18 + q]); D.wr(a[17] + 0x30 + 4 * q, 4, a[21 + q])
        Cc.wr(a[15] + 0x4DC, 4, a[24]); Cc.wr(a[15] + 0x58C, 4, a[25])
        W.snap()
        mem = W.raw(1, a[31]) if a[30] else 0
        ped = W.raw(2, a[15])
        flags = a[13] & 0xFF
        ptype = a[14]
        # evento de memoria nula
        if not a[30]:
            marks.append('mem_null')

        def newmem(size):
            events.append([0x61A5A0, size])

        def ctor_done():
            B.wr(a[31], 4, a[33])
        if signed32(ptype) > 0x2C0:
            kindsel = 'goto' if ptype == 0x384 else 'null'
        elif ptype == 0x2C0:
            kindsel = 'leave'
        elif ((ptype - 0xCA) & M32) == 0:
            kindsel = 'pause'
        elif ((ptype - 0xCA) & M32) == 1:
            kindsel = 'stand'
        else:
            kindsel = 'null'
        if kindsel == 'null':
            marks.append('finished')
            ret = 0
        elif kindsel == 'pause':
            marks.append('pause')
            newmem(0x18)
            if mem:
                events.append([0x48E750, tag(mem), 1])
                ctor_done()
                ret = tag(mem)
            else:
                ret = 0
        elif kindsel == 'leave':
            marks.append('leave_car')
            newmem(0x34)
            if mem:
                events.append([0x63B8C0, tag(mem), tag(a[25]), 0, 0, 1, 0])
                ctor_done()
                ret = tag(mem)
            else:
                ret = 0
        elif kindsel == 'goto':
            marks.append('go_to_point')
            newmem(0x30)
            events.append([0x667CD0, tag(mem), a[7], tag(this + 0x10), a[11], 0, flags & 1])
            ctor_done()
            if flags & 8:
                B.wr(a[31] + 0x20, 1, B.rd(a[31] + 0x20, 1) | 0x10)
                marks.append('bit3')
            if a[7] == 6 or a[7] == 7:
                marks.append('mode_6' if a[7] == 6 else 'mode_7')
                events.append([0x668250, tag(this), tag(mem), tag(ped), a[12], 0x4CBEBC20 if a[7] == 6 else 0x41200000])
            ret = tag(mem)
        else:
            marks.append('stand_still')
            cond = False
            if flags & 2 and sub:
                marks.append('stop_exactly')
                events.append([EV_TYPE, tag(sub)])
                if a[6] == 0x384:
                    cond = True
            if cond:
                if a[16]:
                    marks.append('matrix')
                    mat = W.raw(3, a[17])
                    posptr, fwdptr = mat + 0x30, mat + 0x10
                else:
                    marks.append('no_matrix')
                    posptr, fwdptr = ped + 4, 0x10
                events.append([0x40FE60, STACK_TAG, tag(this + 0x10), tag(posptr)])
                events.append([0x40FDB0, STACK_TAG, tag(fwdptr)])
                sh = v_load(a[24])
                dot = v_load(a[26])
                if v_cmp(sh, v_load(0x3C23D70A)) == 'lt':
                    blend = 0x41000000
                    marks.append('blend_8')
                else:
                    if v_cmp(dot, v_load(0x3C23D70A)) != 'gt':
                        blend = 0x41800000
                        marks.append('blend_16_dot')
                    else:
                        t = v_mul(sh, v_load(0x3F000000))
                        q = v_div(dot, t)
                        q = v_sub(q, v_load(0x3F800000))
                        q = v_mul(q, v_load(a[32]))
                        y = v_div(v_load(0x42480000), q)
                        blend = store32(y)
                        marks.append('blend_computed')
                        if v_cmp(y, v_load(0x41800000)) == 'gt':
                            blend = 0x41800000
                            marks.append('blend_clamped')
                        if is_nan_bits(blend):
                            marks.append('blend_nan')
                newmem(0x20)
                if mem:
                    events.append([0x62F310, tag(mem), 0x7D0, 0, 1, blend])
                    ctor_done()
                    ret = tag(mem)
                else:
                    ret = 0
            else:
                newmem(0x20)
                if mem:
                    events.append([0x62F310, tag(mem), 1, 0, 0, 0x41000000])
                    ctor_done()
                    ret = tag(mem)
                else:
                    ret = 0
    elif k == REACTCTOR:
        W.snap()
        events.append([0x61A3B0, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
        A.wr(o + 0x18, 4, a[6]); A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x1C, 4, a[7]); A.wr(o + 0x14, 4, a[5]); A.wr(o + 0x20, 4, a[8])
        A.wr(o, 4, 0x86D868)
        A.wr(o + 0xC, 1, 0); A.wr(o + 0xD, 1, 0)
        if a[5]:
            events.append([0x571B70, tag(a[5]), tag(this + 0x14), 0x86D868, a[5]])
            marks.append('attacker_set')
        else:
            marks.append('attacker_null')
        ret = tag(this)
    changes = {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}
    return {'events': events, 'changes': changes, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:8], e['events'][:8])
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
