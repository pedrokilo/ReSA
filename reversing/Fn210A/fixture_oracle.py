"""Oraculo independiente (Python) de Fn210A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 17 funciones del lote 210A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits; raiz cuadrada exacta) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico; thiscall: ECX = this).
   0  0x670A60  mov eax,3A3h                     14  0x66B780  mov eax,396h
   1  0x670A70  GetNextSubTaskType(): n = dword [[this+1Ch]] (ruta); n == 0 -> 516h; k = movsx word [this+12h]; k+1 < n (con signo) -> 384h; k+1 == n -> 387h; k != n -> 0C8h; si k == n: pasadas ([this+0Eh], 16 bits) += 1;
            modo = movsx word [this+0Ch] (> 3 sin signo -> 0C8h): 0 -> 516h; 1 -> 516h si las pasadas (16 bits) != 1, si no sigue como 2; 2 -> Reverse 66D550 (ecx = ruta) y sigue como 3; 3 -> word [this+12h] = 0 y vuelve a empezar
   2  0x670B00  CreateSubTask(tipo): comparaciones con signo: 387h (> : 38Ah -> FollowNodeRoute, 516h -> 0, otro -> 0), C8h, 191h, 384h, otro -> 0; cada rama con CTask::operator new (61A5A0) y, sin memoria, devuelve 0
            384h: new(30h); 667CD0 (ecx = memoria; movsx [this+10h], ruta+144h+12*movsx [this+12h], [this+14h], 0, 0); 191h: new(64h); 61A990 (nombre1 = ruta+4+28h*k, nombre2 = nombre1+18h, 58h, 4.0f, -1, 0, 0, 0, 0);
            C8h: new(8); 61A390 (ecx = memoria) y [memoria] = 859CFCh; 387h: radio = [this+18h], modo = movsx [this+0Ch]: 1 con pasadas == 0, 2 o 3 -> 0.0f; new(28h); 668120 (ms, punto k, [this+14h], radio, 0, 0);
            38Ah: ruta vacia -> 0; si movsx k == n: k = k - 1 (16 bits, se guarda en [this+12h]); copia del punto k; new(60h); 66EA30 (ms, &copia, 0.5f, 3.0f, 2.0f, 0, -1, 1)
   3  0x670D80  CreateNextSubTask(ped): tipo = virtual 10h de [this+8]: 38Ah -> salta; 0C8h -> CreateSubTask(516h); 191h -> salta; primer byte del nombre del nodo (ruta+4+28h*k) != 0 -> CreateSubTask(191h); salta: word [this+12h] += 1;
            luego CreateSubTask(GetNextSubTaskType())
   4  0x670E20  CreateFirstSubTask(ped): [this+20h] &= ~1; n = [ruta] == 0 -> CreateSubTask(0C8h); posicion del ped = matriz+30h o ped+4; mejor = 7F7FFFFFh; candidato = -1; por segmento i (B = punto i, A = punto (i+1) % n):
            d = A - B (x, y float; z con fst), longitud al cuadrado = (z*z + x*x) + y*y (float); Normalise 59C910 (ecx = d); t = (rz*dz + rx*dx) + ry*dy con r = ped - B (x87 24 bits); t > 0 (ST0) y sqrt(longitud2) > t (float guardado);
            Q = (d*t + B) con qx guardado como float, dz*t guardado, qy y qz en registros; e = ped - Q (ex y ey guardados, ez en registro); D = (ez*ez + ey*ey) + ex*ex; si D < mejor*mejor (24 bits): mejor = float(D), candidato = (i+1) % n;
            sin candidato: por punto: D = (dz*dz + dy*dy) + dx*dx; mejor inicial = FLT_MAX en registro; D < mejor*mejor -> mejor = float(D), candidato = i; [this+12h] = candidato (16 bits);
            bit 1 de [this+20h]: se apaga y CreateSubTask(38Ah); si no se apaga y CreateSubTask(GetNextSubTaskType())
   5  0x672BB0  destructores con borrado: push esi; call CUERPO (6709F0); si byte [esp+8] & 1: call 61A5B0 (delete) con this; eax = this; ret 4
   6 0x670830 (66B960)   7 0x6711F0 (66BC00)   8 0x6712C0 (66C4C0)   9 0x6712E0 (66C7A0)  idem
  10  0x671330  Clone: new(58h); 66CA40 (ecx = memoria; [+0Ch], [+10h], [+14h], [+18h], [+1Ch], [+24h], byte [+54h] & 1, 1); copia +44h..4Ch, +50h, bit 3 de +54h y +20h (fld/fstp); devuelve la memoria
  11  0x673070  CreateSubTask(tipo, ped): 2CFh: new(70h) 5FF940 (vehiculo [ped+58Ch], 7D0h); 398h: new(28h) 66B890 (entidad [this+0Ch], 0.5f, 0.2f); 387h: new(28h), calculador virtual [this+40h] (ped, entidad, temporal), 668120 (ms [this+50h],
            &temporal, [+18h], [+1Ch], 0, 0) y bit 3 de [this+54h] -> bit 3 de [memoria+24h]; 38Ah: new(60h), calculador, 66EA30 (ms, &temporal, [+18h], [+1Ch], [+24h], 0, -1, 1); F4h: new(40h) 632BD0; con el bit 1 de [this+54h]:
            new(28h) 66B890 y AddTask 632D10; new(20h) 62F310 (64h, 0, 0, 8.0f) y AddTask; 2C0h: new(34h) 63B8C0 (vehiculo, 0, 0, 1, 0); CBh: new(20h) 62F310 (7D0h, 0, 0, 8.0f); DBh: new(1Ch) 630F20 (3E8h); otro -> 0
  12  0x673540  ControlSubTask(ped): ver el bloque de ControlSubTask en expected()
  13  0x673BA0  GetSeekPos(salida, ped): llamada virtual ranura 0 del calculador [this+40h] (ecx = this+40h) con (ped, [this+0Ch], temporal local); copia los 3 dwords del temporal a la salida; devuelve la salida
  15  0x6A2F80  DebugCode(): modelo = tabla[movsx word [this+22h]]; FindPlayerVehicle(-1, 0); es el del jugador si (veh == this y el remoto del jugador en foco == 0) o el remoto == this; entonces con h = handling[byte [modelo+4Ah]]:
            [this+8Ch] = h+4, [this+90h] = h+0Ch, [this+0A0h] = h+24h; arrastre = h+10h: si > 0.01 (ST0, ordenado) -> [this+98h] = arrastre / 1000 * 0.5 (24 bits), si no el dword tal cual
  16  0x6A3030  RemoveRefsToVehicle(entidad): las cuatro ruedas [this+8E8h..8F4h] iguales a la entidad pasan a 0

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexFollowPatrolRoute::GetTaskType", "CTaskComplexFollowPatrolRoute::GetNextSubTaskType", "CTaskComplexFollowPatrolRoute::CreateSubTask", "CTaskComplexFollowPatrolRoute::CreateNextSubTask",
         "CTaskComplexFollowPatrolRoute::CreateFirstSubTask", "CTaskComplexFollowPatrolRoute::ScalarDeletingDestructor", "CTaskComplexTurnToFaceEntityOrCoord::ScalarDeletingDestructor",
         "CTaskComplexGotoDoorAndOpen::ScalarDeletingDestructor", "CTaskSimpleSlideToCoord::ScalarDeletingDestructor", "CTaskComplexInvestigateDisturbance::ScalarDeletingDestructor",
         "CTaskComplexSeekEntity<FixedPos>::Clone", "CTaskComplexSeekEntity<FixedPos>::CreateSubTask", "CTaskComplexSeekEntity<FixedPos>::ControlSubTask", "CTaskComplexSeekEntity<FixedPos>::GetSeekPos",
         "CTaskComplexGoToPointAnyMeans::GetTaskType", "CAutomobile::DebugCode", "CAutomobile::RemoveRefsToVehicle"]
ENTRIES = (0x670A60, 0x670A70, 0x670B00, 0x670D80, 0x670E20, 0x672BB0, 0x670830, 0x6711F0, 0x6712C0, 0x6712E0, 0x671330, 0x673070, 0x673540, 0x673BA0, 0x66B780, 0x6A2F80, 0x6A3030)
GUARD = 0x40
WIN = 0x1000
WIN_C = 0x4000
VTBL = 0xF00
SEED_AT = 124
SIZES = (GUARD + WIN + GUARD, GUARD + WIN + GUARD, GUARD + WIN_C + GUARD, GUARD + WIN + GUARD)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
EV_TYPE, EV_CALC, EV_EVENT, EV_ABORT = 0x50000001, 0x50000002, 0x50000003, 0x50000004
(CONST, SDD, FPNEXT, FPCREATE, FPNEXTSUB, FPFIRST, CLONE, SEEKCREATE, SEEKCONTROL, SEEKPOS, DEBUG, REMOVE) = range(12)
KIND = [CONST, FPNEXT, FPCREATE, FPNEXTSUB, FPFIRST, SDD, SDD, SDD, SDD, SDD, CLONE, SEEKCREATE, SEEKCONTROL, SEEKPOS, CONST, DEBUG, REMOVE]
CONSTS = {0: 0x3A3, 14: 0x396}
SDD_BODY = {5: 0x6709F0, 6: 0x66B960, 7: 0x66BC00, 8: 0x66C4C0, 9: 0x66C7A0}
DELETE = 0x61A5B0
MODEL_LO = -3
HANDLING_IDS = (0, 1, 2, 5, 0x40, 0x80, 0xFE, 0xFF)
PLAYER_COUNT = 4
ZERO = 0x00000000
FLT_MAX = 0x7F7FFFFF


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


def signed8(v):
    v &= 0xFF
    return v - 0x100 if v >= 0x80 else v


def v_sqrt(x):
    """raiz cuadrada de la x87 (fsqrt) con redondeo a 24 bits del significando: exacta (entera)"""
    if x[0] == 'nan':
        return x
    if x[0] == 'inf':
        return x if x[1] > 0 else ('nan', 0xFFC00000)
    fr = x[1]
    if fr == 0:
        return x
    if fr < 0:
        return ('nan', 0xFFC00000)
    e = _exp(fr) // 2
    val = fr * _pow2(2 * (25 - e)) if 2 * (25 - e) >= 0 else fr / _pow2(-2 * (25 - e))
    n = val.numerator // val.denominator
    s = math.isqrt(n)
    inexact = not (val.denominator == 1 and s * s == val.numerator)
    q, rem = s >> 2, s & 3
    if rem > 2 or (rem == 2 and (inexact or (q & 1))):
        q += 1
    return ('fin', Fraction(q) * _pow2(e - 23) if e - 23 >= 0 else Fraction(q, 1 << (23 - e)), False)


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == CONST:
        _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == FPNEXT:
        _k += ['empty', 'next_go', 'next_stand', 'next_none', 'wrap_m0', 'wrap_m1_first', 'wrap_m1_later', 'wrap_m2', 'wrap_m3', 'wrap_other', 'reverse', 'recurse', 'negative_count']
    elif _kind == FPCREATE:
        _k += ['go_to_point', 'named_anim', 'none_task', 'stand_still', 'follow_node', 'other_type', 'mem_null', 'mem_set', 'follow_empty', 'follow_k_eq_n', 'radius_zero_m1', 'radius_zero_m23', 'radius_kept']
    elif _kind == FPNEXTSUB:
        _k += ['type_follow', 'type_none', 'type_anim', 'type_other', 'name_nonempty', 'name_empty']
    elif _kind == FPFIRST:
        _k += ['empty', 'n1', 'n2', 'n3', 'n4', 'n5', 'n6_8', 'n9_12', 'n_negative', 'seg_found', 'phase2', 'phase2_found', 'cand_none', 'flag_bit1', 'flag_bit1_clear', 'matrix', 'no_matrix', 'cond_t_pos', 'cond_len']
    elif _kind == CLONE:
        _k += ['tired_bit', 'tracking_bit', 'nan_min']
    elif _kind == SEEKCREATE:
        _k += ['turn', 'goto', 'follow', 'seq_face', 'seq_noface', 'leave', 'cardrive', 'stand', 'tired', 'other_type', 'mem_null', 'tracking_bit']
    elif _kind == SEEKCONTROL:
        _k += ['entity_null', 'entity_not_ped', 'same_group', 'boat_found', 'boat_attach', 'boat_abort_fails', 'dettach', 'boat_attached_keep', 'type_other', 'no_entity_abort', 'seek_out_of_time', 'seek_tired',
               'seek_nul', 'scan_not_due', 'scan_due', 'follow_settarget', 'follow_abort', 'stand_gotopoint', 'stand_follow', 'stand_ok', 'say_checked', 'say_done', 'min_zero', 'min_nonzero']
    elif _kind == SEEKPOS:
        pass
    elif _kind == DEBUG:
        _k += ['player_vehicle', 'not_player', 'remote_set', 'drag_high', 'drag_low', 'model_negative']
    elif _kind == REMOVE:
        _k += ['hit_none', 'hit_one', 'hit_many']
    KEYS[_fn] = _k


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf_bits(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def sel3(s, v1, v2, raw):
    return 0 if s == 0 else (v1 if s == 1 else (v2 if s == 2 else raw))


def ftol16(bits):
    """_ftol2 del EXE (fistp de 64 bits, parte baja): los 16 bits bajos de la conversion; NaN e infinitos dan 0"""
    bits &= M32
    if (bits & 0x7F800000) == 0x7F800000:
        return 0
    return int(f32(bits)) & 0xFFFF


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
    L, S = v_load, store32
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE, tag(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = tag(this)
    elif k == FPNEXT:
        route = a[4]
        A.wr(o + 0x1C, 4, W.raw(1, route)); B.wr(route, 4, a[5])
        A.wr(o + 0xC, 2, a[7]); A.wr(o + 0xE, 2, a[8]); A.wr(o + 0x12, 2, a[6]); A.wr(o + 0x10, 2, a[9]); A.wr(o + 0x20, 4, a[10])
        W.snap()
        rptr = W.raw(1, route)

        def nxt(first):
            n = signed32(B.rd(route, 4))
            if n == 0:
                marks.append('empty')
                return 0x516
            if n < 0:
                marks.append('negative_count')
            kk = signed16(A.rd(o + 0x12, 2))
            if kk + 1 < n:
                marks.append('next_go')
                return 0x384
            if kk + 1 == n:
                marks.append('next_stand')
                return 0x387
            if kk != n:
                marks.append('next_none')
                return 0xC8
            mode = signed16(A.rd(o + 0xC, 2))
            passes = (A.rd(o + 0xE, 2) + 1) & 0xFFFF
            A.wr(o + 0xE, 2, passes)
            if (mode & M32) > 3:
                marks.append('wrap_other')
                return 0xC8
            if mode == 0:
                marks.append('wrap_m0')
                return 0x516
            if mode == 1:
                if passes != 1:
                    marks.append('wrap_m1_later')
                    return 0x516
                marks.append('wrap_m1_first')
            elif mode == 2:
                marks.append('wrap_m2')
            else:
                marks.append('wrap_m3')
            if mode in (1, 2):
                events.append([0x66D550, tag(rptr), B.rd(route, 4)])
                marks.append('reverse')
            A.wr(o + 0x12, 2, 0)
            marks.append('recurse')
            return nxt(False)
        ret = nxt(True)
    elif k == FPCREATE:
        route = a[5]
        A.wr(o + 0x1C, 4, W.raw(1, route)); B.wr(route, 4, a[6])
        A.wr(o + 0xC, 2, a[7]); A.wr(o + 0xE, 2, a[8]); A.wr(o + 0x10, 2, a[9]); A.wr(o + 0x12, 2, a[10])
        A.wr(o + 0x14, 4, a[11]); A.wr(o + 0x18, 4, a[12]); A.wr(o + 0x20, 4, a[13])
        W.snap()
        rptr = W.raw(1, route)
        mem = W.raw(3, a[15]) if a[14] else 0
        t = signed32(a[4])
        msx = signed16(A.rd(o + 0x10, 2)) & M32

        def pointk():
            return (rptr + 0x144 + 12 * signed16(A.rd(o + 0x12, 2))) & M32

        def ctor_done():
            D.wr(a[15], 4, a[16])
        marks.append('mem_set' if mem else 'mem_null')
        if t > 0x387:
            if t == 0x38A:
                marks.append('follow_node')
                n = B.rd(route, 4)
                if n == 0:
                    marks.append('follow_empty')
                    ret = 0
                else:
                    if signed16(A.rd(o + 0x12, 2)) == signed32(n):
                        marks.append('follow_k_eq_n')
                        A.wr(o + 0x12, 2, (A.rd(o + 0x12, 2) - 1) & 0xFFFF)
                    kk = signed16(A.rd(o + 0x12, 2))
                    pbase = route + 0x144 + 12 * kk
                    pt = [B.rd(pbase + 4 * q, 4) for q in range(3)]
                    events.append([0x61A5A0, 0x60])
                    if mem:
                        events.append([0x66EA30, tag(mem), msx, STACK_TAG] + pt + [0x3F000000, 0x40400000, 0x40000000, 0, M32, 1])
                        ctor_done()
                        ret = tag(mem)
                    else:
                        ret = 0
            else:
                marks.append('other_type')
                ret = 0
        elif t == 0x387:
            marks.append('stand_still')
            mode = signed16(A.rd(o + 0xC, 2))
            radius = a[12]
            if mode == 1:
                if (A.rd(o + 0xE, 2)) == 0:
                    radius = 0
                    marks.append('radius_zero_m1')
                else:
                    marks.append('radius_kept')
            elif mode in (2, 3):
                radius = 0
                marks.append('radius_zero_m23')
            else:
                marks.append('radius_kept')
            events.append([0x61A5A0, 0x28])
            if mem:
                events.append([0x668120, tag(mem), msx, tag(pointk()), a[11], radius, 0, 0])
                ctor_done()
                ret = tag(mem)
            else:
                ret = 0
        elif t == 0xC8:
            marks.append('none_task')
            events.append([0x61A5A0, 8])
            if mem:
                events.append([0x61A390, tag(mem)])
                ctor_done()
                D.wr(a[15], 4, 0x859CFC)
                ret = tag(mem)
            else:
                ret = 0
        elif t == 0x191:
            marks.append('named_anim')
            name = (rptr + 4 + 0x28 * signed16(A.rd(o + 0x12, 2))) & M32
            events.append([0x61A5A0, 0x64])
            if mem:
                events.append([0x61A990, tag(mem), tag(name), tag((name + 0x18) & M32), 0x58, 0x40800000, M32, 0, 0, 0, 0])
                ctor_done()
                ret = tag(mem)
            else:
                ret = 0
        elif t == 0x384:
            marks.append('go_to_point')
            events.append([0x61A5A0, 0x30])
            if mem:
                events.append([0x667CD0, tag(mem), msx, tag(pointk()), a[11], 0, 0])
                ctor_done()
                ret = tag(mem)
            else:
                ret = 0
        else:
            marks.append('other_type')
            ret = 0
    elif k == FPNEXTSUB:
        A.wr(o + 8, 4, W.raw(2, a[4]))
        Cc.wr(a[4], 4, vt); Cc.wr(a[4] + 0x30, 4, a[5])
        A.wr(o + 0x1C, 4, W.raw(1, a[9])); A.wr(o + 0x12, 2, a[10])
        nameoff = a[9] + 4 + 0x28 * signed16(a[10])
        B.wr(nameoff, 1, a[11] & 0xFF)
        W.snap()
        sub = tag(W.raw(2, a[4]))

        def vtype():
            events.append([EV_TYPE, sub])
            return a[5]
        t1 = vtype()
        done = False
        if t1 != 0x38A:
            t2 = vtype()
            if t2 == 0xC8:
                marks.append('type_none')
                events.append([0x670B00, tag(this), 0x516])
                ret = a[7]
                done = True
            else:
                t3 = vtype()
                if t3 != 0x191:
                    if B.rd(nameoff, 1) != 0:
                        marks.append('name_nonempty')
                        marks.append('type_other')
                        events.append([0x670B00, tag(this), 0x191])
                        ret = a[7]
                        done = True
                    else:
                        marks.append('name_empty')
                else:
                    marks.append('type_anim')
                if not done:
                    A.wr(o + 0x12, 2, (A.rd(o + 0x12, 2) + 1) & 0xFFFF)
        else:
            marks.append('type_follow')
        if not done:
            events.append([0x670A70, tag(this)])
            events.append([0x670B00, tag(this), a[8]])
            ret = a[7]
        ret = tag(ret)
    elif k == FPFIRST:
        route = a[14]
        A.wr(o + 0x1C, 4, W.raw(1, route)); B.wr(route, 4, a[13])
        for p in range(12):
            for q in range(3):
                B.wr(route + 0x144 + 12 * p + 4 * q, 4, a[15 + 3 * p + q])
        A.wr(o + 0x20, 4, a[87]); A.wr(o + 0x12, 2, a[88]); A.wr(o + 0xC, 2, a[89]); A.wr(o + 0xE, 2, a[90]); A.wr(o + 0x10, 2, a[91])
        for q in range(3):
            Cc.wr(a[4] + 4 + 4 * q, 4, a[7 + q]); D.wr(a[6] + 0x30 + 4 * q, 4, a[10 + q])
        Cc.wr(a[4] + 0x14, 4, W.raw(3, a[6]) if a[5] else 0)
        W.snap()
        flags = A.rd(o + 0x20, 4) & 0xFFFFFFFE
        A.wr(o + 0x20, 4, flags)
        ncount = signed32(a[13])
        marks.append('empty' if ncount == 0 else ('n_negative' if ncount < 0 else ('n1' if ncount == 1 else ('n2' if ncount == 2 else ('n3' if ncount == 3 else ('n4' if ncount == 4 else ('n5' if ncount == 5 else ('n6_8' if ncount <= 8 else 'n9_12'))))))))
        marks.append('matrix' if a[5] else 'no_matrix')
        if ncount == 0:
            events.append([0x670B00, tag(this), 0xC8])
            ret = a[92]
        else:
            pp = [a[10 + q] for q in range(3)] if a[5] else [a[7 + q] for q in range(3)]
            P = lambda p, q: B.rd(route + 0x144 + 12 * p + 4 * q, 4)
            best, cand, ncall = FLT_MAX, -1, 0
            if ncount > 0:
                for i in range(ncount):
                    nx = (i + 1) % ncount
                    pa = [P(nx, q) for q in range(3)]
                    pb = [P(i, q) for q in range(3)]
                    dx = S(v_sub(L(pa[0]), L(pb[0]))); dy = S(v_sub(L(pa[1]), L(pb[1])))
                    zv = v_sub(L(pa[2]), L(pb[2])); zb = S(zv)
                    zz = v_mul(zv, L(zb))
                    s = v_add(zz, v_mul(L(dx), L(dx)))
                    s = v_add(s, v_mul(L(dy), L(dy)))
                    len2 = S(s)
                    events.append([0x59C910, STACK_TAG, dx, dy, zb])
                    ci = 3 * min(ncall, 11)
                    ncall += 1
                    nxv, nyv, nzv = a[51 + ci], a[52 + ci], a[53 + ci]
                    rx = v_sub(L(pp[0]), L(pb[0])); ry = v_sub(L(pp[1]), L(pb[1])); rz = v_sub(L(pp[2]), L(pb[2]))
                    t = v_add(v_add(v_mul(rz, L(nzv)), v_mul(rx, L(nxv))), v_mul(ry, L(nyv)))
                    tb = S(t)
                    if v_cmp(t, L(ZERO)) != 'gt':
                        continue
                    marks.append('cond_t_pos')
                    if v_cmp(v_sqrt(L(len2)), L(tb)) != 'gt':
                        continue
                    marks.append('cond_len')
                    xt = v_mul(L(nxv), L(tb)); yt = v_mul(L(nyv), L(tb)); zt = v_mul(L(nzv), L(tb)); ztb = S(zt)
                    qx = v_add(xt, L(pb[0])); qxb = S(qx); qy = v_add(yt, L(pb[1])); qz = v_add(L(ztb), L(pb[2]))
                    ex = v_sub(L(pp[0]), L(qxb)); exb = S(ex); ey = v_sub(L(pp[1]), qy); eyb = S(ey); ez = v_sub(L(pp[2]), qz)
                    dd = v_add(v_add(v_mul(ez, ez), v_mul(L(eyb), L(eyb))), v_mul(L(exb), L(exb)))
                    if v_cmp(dd, v_mul(L(best), L(best))) == 'lt':
                        best = S(dd)
                        cand = nx
                        marks.append('seg_found')
            if cand == -1:
                marks.append('phase2')
                bestv = L(FLT_MAX)
                for p in range(max(ncount, 0)):
                    d0 = v_sub(L(pp[0]), L(P(p, 0))); d1 = v_sub(L(pp[1]), L(P(p, 1))); d2 = v_sub(L(pp[2]), L(P(p, 2)))
                    dd = v_add(v_add(v_mul(d2, d2), v_mul(d1, d1)), v_mul(d0, d0))
                    df = S(dd)
                    if v_cmp(dd, v_mul(bestv, bestv)) == 'lt':
                        bestv = L(df)
                        cand = p
                        marks.append('phase2_found')
            if cand == -1:
                marks.append('cand_none')
            A.wr(o + 0x12, 2, cand & 0xFFFF)
            f2 = A.rd(o + 0x20, 4)
            A.wr(o + 0x20, 4, f2 & 0xFFFFFFFD)
            if f2 & 2:
                marks.append('flag_bit1')
                events.append([0x670B00, tag(this), 0x38A])
            else:
                marks.append('flag_bit1_clear')
                events.append([0x670A70, tag(this)])
                events.append([0x670B00, tag(this), a[93]])
            ret = a[92]
        ret = tag(ret)
    elif k == CLONE:
        for off, idx in ((0xC, 4), (0x10, 5), (0x14, 6), (0x18, 7), (0x1C, 8), (0x20, 9), (0x24, 10), (0x44, 11), (0x48, 12), (0x4C, 13), (0x50, 14)):
            A.wr(o + off, 4, a[idx])
        A.wr(o + 0x54, 1, a[15] & 0xFF)
        W.snap()
        mem = W.raw(3, a[16])
        events.append([0x61A5A0, 0x58])
        events.append([0x66CA40, tag(mem), a[4], a[5], a[6], a[7], a[8], a[10], a[15] & 1, 1])
        D.wr(a[16], 4, a[17])
        D.wr(a[16] + 0x44, 4, a[11]); D.wr(a[16] + 0x48, 4, a[12]); D.wr(a[16] + 0x4C, 4, a[13])
        D.wr(a[16] + 0x50, 4, a[14])
        cl = D.rd(a[16] + 0x54, 1)
        D.wr(a[16] + 0x54, 1, (cl & ~8) | ((a[15] & 0xFF) & 8))
        D.wr(a[16] + 0x20, 4, a[9])
        marks.append('tired_bit' if a[15] & 1 else 'tired_clear')
        if a[15] & 8:
            marks.append('tracking_bit')
        if is_nan_bits(a[9]):
            marks.append('nan_min')
        ret = tag(mem)
    elif k == SEEKCREATE:
        A.wr(o + 0xC, 4, a[6]); A.wr(o + 0x18, 4, a[7]); A.wr(o + 0x1C, 4, a[8]); A.wr(o + 0x24, 4, a[9]); A.wr(o + 0x50, 4, a[10]); A.wr(o + 0x54, 1, a[11] & 0xFF); A.wr(o + 0x40, 4, vt)
        Cc.wr(a[5] + 0x58C, 4, a[12])
        W.snap()
        ped = W.raw(2, a[5])
        allocs = [W.raw(3, a[17 + 2 * i]) if a[16 + 2 * i] else 0 for i in range(3)]
        state = {'i': 0}

        def newmem(size):
            events.append([0x61A5A0, size])
            idxm = min(state['i'], 2)
            state['i'] += 1
            return allocs[idxm], idxm

        def marker(idx):
            D.wr(a[17 + 2 * idx], 4, a[22])
        t = signed32(a[4])
        vec = [a[13], a[14], a[15]]

        def calc():
            events.append([EV_CALC, tag(this + 0x40), tag(ped), tag(a[6]), STACK_TAG])
        flags = a[11] & 0xFF
        if t > 0x2CF:
            if t > 0x398:
                marks.append('other_type'); ret = 0
            elif t == 0x398:
                marks.append('turn')
                m, ix = newmem(0x28)
                if m:
                    events.append([0x66B890, tag(m), a[6], 0x3F000000, 0x3E4CCCCD]); marker(ix); ret = tag(m)
                else:
                    marks.append('mem_null'); ret = 0
            elif t == 0x387:
                marks.append('goto')
                m, ix = newmem(0x28)
                calc()
                events.append([0x668120, tag(m), a[10], STACK_TAG] + vec + [a[7], a[8], 0, 0])
                marker(ix)
                D.wr(a[17 + 2 * ix] + 0x24, 1, (D.rd(a[17 + 2 * ix] + 0x24, 1) & ~8) | (flags & 8))
                if flags & 8:
                    marks.append('tracking_bit')
                ret = tag(m)
            elif t == 0x38A:
                marks.append('follow')
                m, ix = newmem(0x60)
                if m:
                    calc()
                    events.append([0x66EA30, tag(m), a[10], STACK_TAG] + vec + [a[7], a[8], a[9], 0, M32, 1])
                    marker(ix); ret = tag(m)
                else:
                    marks.append('mem_null'); ret = 0
            else:
                marks.append('other_type'); ret = 0
        elif t == 0x2CF:
            marks.append('cardrive')
            m, ix = newmem(0x70)
            if m:
                events.append([0x5FF940, tag(m), a[12], 0x7D0]); marker(ix); ret = tag(m)
            else:
                marks.append('mem_null'); ret = 0
        elif t > 0xF4:
            if t == 0x2C0:
                marks.append('leave')
                m, ix = newmem(0x34)
                if m:
                    events.append([0x63B8C0, tag(m), tag(a[12]), 0, 0, 1, 0]); marker(ix); ret = tag(m)
                else:
                    marks.append('mem_null'); ret = 0
            else:
                marks.append('other_type'); ret = 0
        elif t == 0xF4:
            seq, sx = newmem(0x40)
            events.append([0x632BD0, tag(seq)]); marker(sx)
            if flags & 2:
                marks.append('seq_face')
                m, ix = newmem(0x28)
                if m:
                    events.append([0x66B890, tag(m), a[6], 0x3F000000, 0x3E4CCCCD]); marker(ix)
                events.append([0x632D10, tag(seq), tag(m)])
            else:
                marks.append('seq_noface')
            m, ix = newmem(0x20)
            if m:
                events.append([0x62F310, tag(m), 0x64, 0, 0, 0x41000000]); marker(ix)
            events.append([0x632D10, tag(seq), tag(m)])
            ret = tag(seq)
        else:
            s = t - 0xCB
            if s == 0:
                marks.append('stand')
                m, ix = newmem(0x20)
                if m:
                    events.append([0x62F310, tag(m), 0x7D0, 0, 0, 0x41000000]); marker(ix); ret = tag(m)
                else:
                    marks.append('mem_null'); ret = 0
            elif s - 0x10 == 0:
                marks.append('tired')
                m, ix = newmem(0x1C)
                if m:
                    events.append([0x630F20, tag(m), 0x3E8]); marker(ix); ret = tag(m)
                else:
                    marks.append('mem_null'); ret = 0
            else:
                marks.append('other_type'); ret = 0
    elif k == SEEKCONTROL:
        eP, v1, v2, pP, ppP = W.raw(2, a[5]), W.raw(2, a[10]), W.raw(2, a[11]), W.raw(2, a[16]), W.raw(2, a[78])
        entity = 0 if a[4] else eP
        sub = W.raw(3, a[65])
        A.wr(o + 0xC, 4, entity)
        for off, idx in ((0x14, 58), (0x18, 59), (0x1C, 60), (0x20, 61), (0x24, 62), (0x28, 50), (0x2C, 51)):
            A.wr(o + off, 4, a[idx])
        A.wr(o + 0x30, 1, a[52]); A.wr(o + 0x31, 1, a[53]); A.wr(o + 0x34, 4, a[54]); A.wr(o + 0x38, 4, a[55]); A.wr(o + 0x3C, 1, a[56]); A.wr(o + 0x3D, 1, a[57])
        A.wr(o + 0x40, 4, vt); A.wr(o + 0x50, 4, a[63]); A.wr(o + 0x54, 1, a[64])
        A.wr(o + 8, 4, sub)
        D.wr(a[65], 4, vt); D.wr(a[65] + 0x30, 4, a[66]); D.wr(a[65] + 0x24, 1, a[67])
        Cc.wr(a[5] + 0x36, 1, a[6]); Cc.wr(a[5] + 0x40, 4, a[7])
        Cc.wr(a[5] + 0x568, 4, sel3(a[8], v1, v2, eP)); Cc.wr(a[5] + 0x584, 4, sel3(a[9], v1, v2, eP))
        Cc.wr(a[10] + 0x36, 1, a[12]); Cc.wr(a[10] + 0x590, 4, a[13]); Cc.wr(a[11] + 0x36, 1, a[14]); Cc.wr(a[11] + 0x590, 4, a[15])
        vOff, vMat, vMode, vPtr = (a[10], a[11]), (a[36], a[44]), (a[35], a[43]), (v1, v2)
        for i in range(2):
            pos, mpos = (40, 45) if i else (32, 37)
            for q in range(3):
                Cc.wr(vOff[i] + 4 + 4 * q, 4, a[pos + q]); D.wr(vMat[i] + 0x30 + 4 * q, 4, a[mpos + q])
            Cc.wr(vOff[i] + 0x14, 4, W.raw(3, vMat[i]) if vMode[i] else 0)
        p568, p584 = sel3(a[17], v1, v2, (a[48] | 1) & M32), sel3(a[18], v1, v2, (a[49] | 1) & M32)
        Cc.wr(a[16] + 0x568, 4, p568); Cc.wr(a[16] + 0x584, 4, p584); Cc.wr(a[16] + 0xFC, 4, a[19])
        for q in range(3):
            Cc.wr(a[16] + 4 + 4 * q, 4, a[22 + q]); D.wr(a[21] + 0x30 + 4 * q, 4, a[25 + q])
        Cc.wr(a[16] + 0x14, 4, W.raw(3, a[21]) if a[20] else 0)
        Cc.wr(a[16] + 0x534, 4, a[28]); Cc.wr(a[16] + 0x20, 2, a[29]); Cc.wr(a[16] + 0x718, 1, a[30]); Cc.wr(a[16] + 0x5A0 + 0x1C * (a[30] % 13), 4, a[31])
        Cc.wr(a[78] + 0x480, 4, W.raw(3, a[79])); D.wr(a[79] + 0x38, 4, a[80])
        W.snap()
        now = a[48]
        abort_state = {'n': 0}

        def vtype():
            events.append([EV_TYPE, tag(sub)])
            return a[66]

        def abort():
            events.append([EV_ABORT, tag(sub), tag(pP), 1, 0])
            bit = (a[68] >> (abort_state['n'] & 31)) & 1
            abort_state['n'] += 1
            return bit

        def deref(ptr, off, n):
            base = W.bases[2] + GUARD
            return Cc.rd(ptr - base + off, n)

        def pos_of(is_matrix, mat_off, obj_off, arena_obj):
            return (W.raw(3, mat_off) + 0x30) if is_matrix else (W.raw(2, obj_off) + 4)
        result = sub
        stage = 'main'          # 'main' | 'say' | 'return'
        seek = [a[69], a[70], a[71]]
        if entity != 0 and (Cc.rd(a[5] + 0x36, 1) & 7) == 3:
            marks.append('same_group')
            events.append([0x5F7F40, tag(pP), tag(entity)])
            if (a[73] & 0xFF) != 0:
                boat = 0
                eflags = Cc.rd(a[5] + 0x40, 4)
                if not ((eflags >> 8) & 1):
                    e568 = Cc.rd(a[5] + 0x568, 4)
                    second = True
                    if e568 != 0 and (deref(e568, 0x36, 1) & 7) == 2:
                        if p568 == e568 or p584 == e568:
                            second = False
                            if deref(e568, 0x590, 4) == 5:
                                boat = e568
                    if second:
                        e584 = Cc.rd(a[5] + 0x584, 4)
                        if e584 != 0 and (deref(e584, 0x36, 1) & 7) == 2 and (p568 == e584 or p584 == e584) and deref(e584, 0x590, 4) == 5:
                            boat = e584
                if boat:
                    marks.append('boat_found')
                attached = Cc.rd(a[16] + 0xFC, 4)
                go_attach = False
                if attached != 0:
                    if boat == 0:
                        events.append([0x5E7EC0, tag(pP)])
                        marks.append('dettach')
                    else:
                        marks.append('boat_attached_keep')
                elif boat != 0:
                    tt = vtype()
                    if tt != 0xCB:
                        if abort():
                            go_attach = True
                        else:
                            marks.append('boat_abort_fails')
                if go_attach:
                    marks.append('boat_attach')
                    bi = 0 if boat == v1 else 1
                    pedpos = (W.raw(3, a[21]) + 0x30) if a[20] else (pP + 4)
                    bmode, bmat, bpos = vMode[bi], vMat[bi], (32 if bi == 0 else 40)
                    boatpos = (W.raw(3, bmat) + 0x30) if bmode else (boat + 4)
                    events.append([0x40FE00, STACK_TAG, tag(boatpos), tag(pedpos)])
                    diff = [a[81], a[82], a[83]]
                    events.append([0x59C910, STACK_TAG] + diff)
                    dr = [a[84], a[85], a[86]]
                    q25 = L(0x3E800000)
                    xq, yq, zq = v_mul(L(dr[0]), q25), v_mul(L(dr[1]), q25), v_mul(L(dr[2]), q25)
                    vec = [S(v_sub(L(diff[0]), xq)), S(v_sub(L(diff[1]), yq)), S(v_sub(L(diff[2]), zq))]
                    # posicion del barco (matriz o simple): se lee de la memoria del modelo
                    if bmode:
                        bpx = [D.rd(bmat + 0x30 + 4 * q, 4) for q in range(3)]
                    else:
                        bpx = [Cc.rd(vOff[bi] + 4 + 4 * q, 4) for q in range(3)]
                    npos = [S(v_sub(L(bpx[q]), L(vec[q]))) for q in range(3)]
                    events.append([0x616920, STACK_TAG] + npos)
                    if a[87] & 0xFF:
                        npos = [npos[0], npos[1], a[88]]
                    off = [S(v_sub(L(npos[q]), L(bpx[q]))) for q in range(3)]
                    events.append([0x59B920, tag(Cc.rd(vOff[bi] + 0x14, 4)), STACK_TAG])
                    events.append([0x59C6D0, STACK_TAG, STACK_TAG, STACK_TAG] + off)
                    mul = [a[89], a[90], a[91]]
                    events.append([0x53CBE0, dr[0], dr[1], 0, 0])
                    angle = S(L(a[93]))
                    events.append([0x53CB50, angle])
                    heading = ftol16(a[94])
                    slot = signed8(a[30])
                    weapon = Cc.rd(a[16] + 0x5A0 + 0x1C * slot, 4)
                    events.append([0x5E7CB0, tag(pP), tag(boat)] + mul + [heading, 0x3E4CCCCD, weapon])
                    events.append([0x61A5A0, 0x20])
                    m = W.raw(3, a[76]) if a[75] else 0
                    if m:
                        events.append([0x62F310, tag(m), 0x7D0, 0, 0, 0x41000000])
                        D.wr(a[76], 4, a[77])
                        res = m
                    else:
                        res = 0
                    events.append([0x59ACD0, STACK_TAG])
                    result = res
                    stage = 'return'
            else:
                marks.append('same_group_no')
        else:
            marks.append('entity_null' if entity == 0 else 'entity_not_ped')
        if stage == 'main':
            tt = vtype()
            proceed = tt == 0x38A
            if not proceed:
                proceed = vtype() == 0x387
            if not proceed:
                marks.append('type_other')
                stage = 'say'
            elif entity == 0:
                marks.append('no_entity_abort')
                result = 0 if abort() else sub
                stage = 'say'
            else:
                started = A.rd(o + 0x3C, 1) != 0
                out_seek = False
                if started:
                    if A.rd(o + 0x3D, 1) != 0:
                        A.wr(o + 0x34, 4, now); A.wr(o + 0x3D, 1, 0)
                    edx = (A.rd(o + 0x38, 4) + A.rd(o + 0x34, 4)) & M32
                    out_seek = not (now < edx)
                if out_seek:
                    marks.append('seek_out_of_time')
                    if not abort():
                        result = sub
                    elif A.rd(o + 0x54, 1) & 1:
                        marks.append('seek_tired')
                        events.append([0x61A5A0, 0x1C])
                        m = W.raw(3, a[76]) if a[75] else 0
                        if m:
                            events.append([0x630F20, tag(m), 0x3E8]); D.wr(a[76], 4, a[77]); result = m
                        else:
                            result = 0
                    else:
                        marks.append('seek_nul')
                        result = 0
                    stage = 'say'
                else:
                    if A.rd(o + 0x30, 1) == 0:
                        marks.append('scan_not_due'); stage = 'say'
                    else:
                        if A.rd(o + 0x31, 1) != 0:
                            A.wr(o + 0x28, 4, now); A.wr(o + 0x31, 1, 0)
                        edx = (A.rd(o + 0x2C, 4) + A.rd(o + 0x28, 4)) & M32
                        if not (now >= edx):
                            marks.append('scan_not_due'); stage = 'say'
                        else:
                            marks.append('scan_due')
                            A.wr(o + 0x2C, 4, A.rd(o + 0x14, 4)); A.wr(o + 0x28, 4, now); A.wr(o + 0x30, 1, 1)
                            events.append([0x673BA0, tag(this), STACK_TAG, tag(pP)])
                            pxy = [D.rd(a[21] + 0x30 + 4 * q, 4) for q in range(2)] if a[20] else [Cc.rd(a[16] + 4 + 4 * q, 4) for q in range(2)]
                            dx = v_sub(L(pxy[0]), L(seek[0])); dy = v_sub(L(pxy[1]), L(seek[1]))
                            dist = S(v_add(v_mul(dx, dx), v_mul(dy, dy)))
                            tt = vtype()
                            minD = A.rd(o + 0x20, 4)
                            marks.append('min_zero' if v_cmp(L(minD), L(ZERO)) == 'eq' else 'min_nonzero')
                            settarget, gotopoint = False, False
                            if tt == 0x38A:
                                if v_cmp(L(minD), L(ZERO)) == 'eq':
                                    settarget = True
                                else:
                                    mm = v_sub(L(minD), L(0x3F800000))
                                    m2 = v_mul(mm, mm)
                                    if v_cmp(L(dist), m2) == 'lt':
                                        if not abort():
                                            result = sub
                                            stage = 'say'
                                        else:
                                            marks.append('follow_abort')
                                            events.append([0x673070, tag(this), 0x387, tag(pP)])
                                            result = a[92]
                                            stage = 'return'
                                    else:
                                        settarget = True
                                if settarget:
                                    marks.append('follow_settarget')
                                    events.append([0x671750, tag(sub), tag(pP), STACK_TAG] + seek + [A.rd(o + 0x18, 4), A.rd(o + 0x1C, 4), A.rd(o + 0x24, 4), 0])
                                    result = sub
                                    stage = 'say'
                            else:
                                if D.rd(a[65] + 0x24, 1) & 0x10:
                                    marks.append('stand_ok'); result = sub; stage = 'say'
                                else:
                                    if v_cmp(L(minD), L(ZERO)) == 'eq':
                                        gotopoint = True
                                    else:
                                        mx = v_add(L(minD), L(0x3F800000))
                                        m2 = v_mul(mx, mx)
                                        if v_cmp(L(dist), m2) != 'gt':
                                            gotopoint = True
                                        elif not abort():
                                            result = sub; stage = 'say'
                                        else:
                                            marks.append('stand_follow')
                                            events.append([0x61A5A0, 0x60])
                                            m = W.raw(3, a[76]) if a[75] else 0
                                            if not m:
                                                result = 0
                                            else:
                                                events.append([0x673BA0, tag(this), STACK_TAG, tag(pP)])
                                                events.append([0x66EA30, tag(m), A.rd(o + 0x50, 4), STACK_TAG] + seek + [A.rd(o + 0x18, 4), A.rd(o + 0x1C, 4), A.rd(o + 0x24, 4), 0, M32, 1])
                                                D.wr(a[76], 4, a[77])
                                                result = m
                                            stage = 'return'
                                    if gotopoint:
                                        marks.append('stand_gotopoint')
                                        events.append([0x46FE60, tag(sub), STACK_TAG] + seek + [A.rd(o + 0x18, 4), A.rd(o + 0x1C, 4), 0])
                                        result = sub
                                        stage = 'say'
        if stage == 'say':
            ent = A.rd(o + 0xC, 4)
            if ent != 0 and (Cc.rd(a[5] + 0x36, 1) & 7) == 3:
                marks.append('say_checked')
                events.append([0x5DF8F0, tag(ent)])
                if a[72] & 0xFF:
                    events.append([0x56E210, M32])
                    ecx = (a[80] * 0x2D4 + 0xC09928) & M32
                    events.append([0x5F6A10, tag(ecx), tag(pP)])
                    if a[74] & 0xFF:
                        if Cc.rd(a[16] + 0x534, 4) in (5, 6, 7):
                            if ((a[49] + (a[29] & 0xFFFF)) & 0x3FFF) == 0:
                                marks.append('say_done')
                                events.append([0x5EFFE0, tag(pP), 0x5B, 0, 0x3F800000, 0, 0, 0])
        ret = tag(result)
    elif k == SEEKPOS:
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x40, 4, vt)
        W.snap()
        out = W.raw(1, a[9])
        events.append([EV_CALC, tag(this + 0x40), tag(a[5]), tag(a[4]), STACK_TAG])
        B.wr(a[9], 4, a[6]); B.wr(a[9] + 4, 4, a[7]); B.wr(a[9] + 8, 4, a[8])
        ret = tag(out)
    elif k == DEBUG:
        A.wr(o + 0x22, 2, a[4])
        for kk in range(11):
            D.wr(0x40 * kk + 0x4A, 1, HANDLING_IDS[a[46 + kk] & 7])
        focus = a[6] & 0xFF
        other = a[44]
        remote = [sel3(a[7 + q], this, other, other) for q in range(PLAYER_COUNT)]
        veh = sel3(a[11], this, other, other)
        W.snap()
        slot = signed16(a[4]) - MODEL_LO
        events.append([0x56E0D0, M32, 0])
        # FindPlayerVehicle: o el vehiculo coincide y no hay remoto, o el remoto del jugador en foco es este
        isplayer = (veh == this and remote[focus] == 0) or remote[focus] == this
        if signed16(a[4]) < 0:
            marks.append('model_negative')
        if remote[focus] != 0:
            marks.append('remote_set')
        if isplayer:
            marks.append('player_vehicle')
            e = a[46 + slot] & 7
            mass, turn, drag, buoy = a[12 + 4 * e], a[13 + 4 * e], a[14 + 4 * e], a[15 + 4 * e]
            A.wr(o + 0x8C, 4, mass); A.wr(o + 0x90, 4, turn); A.wr(o + 0xA0, 4, buoy)
            if v_cmp(L(drag), L(0x3C23D70A)) == 'gt':
                marks.append('drag_high')
                A.wr(o + 0x98, 4, S(v_mul(v_div(L(drag), L(0x447A0000)), L(0x3F000000))))
            else:
                marks.append('drag_low')
                A.wr(o + 0x98, 4, drag)
        else:
            marks.append('not_player')
    elif k == REMOVE:
        other = a[9]
        vals = [0 if a[5 + q] == 0 else (a[4] if a[5 + q] == 1 else other) for q in range(4)]
        for q in range(4):
            A.wr(o + 0x8E8 + 4 * q, 4, vals[q])
        W.snap()
        hits = 0
        for q in range(4):
            if vals[q] == a[4]:
                A.wr(o + 0x8E8 + 4 * q, 4, 0)
                hits += 1
        marks.append('hit_none' if hits == 0 else ('hit_one' if hits == 1 else 'hit_many'))
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
