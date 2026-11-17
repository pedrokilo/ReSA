"""Oraculo independiente (Python) de Fn207A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 207A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas y el mundo de nodos (Fixtures.hpp) para conocer su
imagen inicial, la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas, fsqrt a 24 bits exactos y fsin, fcos y fpatan con
mpmath (64 bits de mantisa).

Desensamblado del EXE (sobre el gta-sa.exe canonico; thiscall: ECX = this).
   0  0x634C10  CreateFirstSubTask(ped): dword [ped+478h] |= 2 y dword [ped+474h] |= 400000h (ambos leidos antes de escribir); IsPlayer 5DF8F0 (ecx = ped): si al: ClearWeaponTarget 609C80; [ped+70Ch] == 37h: ranura =
            movsx byte [ped+718h]; [ped+70Ch] = dword [ped + 5A0h + ranura * 1Ch]; SetCurrentWeapon 5E6280 (ecx = ped, 0); [ped+480h] != 0: byte [datos+85h] = 1; IsPlayer otra vez: si al: ClearPlayerWeaponMode 50AB10
            (ecx = 0B6F028h); StandStill local (vtable 86DD2C; dwords +8, +0Ch, +10h = 0, bytes +14h, +15h, +18h, +19h = 0, float +1Ch = 8.0f) + ProcessPed 62F370 (ecx = local, ped); new(20h) 61A5A0: si no nulo,
            61A900 (ecx = memoria; 0, 91h, 40800000h, 642h, "PhoneIn", 0) y devuelve el EAX del constructor (this); si nulo devuelve 0
   1  0x634D60  ControlSubTask(ped): resultado = [this+8]; [ped+480h] != 0: byte [datos+85h] = 1; tipo = virtual (+10h) de la subtarea; si != 0CAh: 4D68B0 (clump [ped+18h], 91h) y (clump, 92h); con la 91h: si NO (tiempo [+20h] <
            0.85f ordenado) AddWeaponModel 5E5ED0 (ecx = ped, 14Ah); sin ella y con la 92h: tiempo [+20h] >= 1.566 (doble, ordenado) y ([+20h] - [+28h]) < 1.566: 6348F0 (ecx = this, ped); sin ninguna: IsOutOfTime 420E30 (ecx = this+10h)
            y, si al: new(20h) 61A5A0 y, si no nulo, 61A900 (ecx = memoria; 0, 92h, 40800000h, 643h, "PhoneOut", 0) (resultado = su EAX; nulo si new devuelve 0); al final con [ped+480h] != 0: byte [datos+84h] = 1; float [datos+18h] < 0.0f
            (ordenado) -> 0; FindTaskByType 600EE0 (ecx = [ped+47Ch], 0); si no nulo: 6883D0 (ecx = tarea, ped, 1); byte [[ped+480h]+84h] = 0 (releido)
   2  0x636FD0  Clone: new(20h) 61A5A0; si nulo -> 0; duracion = [this+0Ch]; 61A3B0 (ecx = memoria); [m+0Ch] = duracion; [m] = 86E454h; dwords +10h y +14h, bytes +18h, +19h, +1Ch, +1Dh = 0; devuelve la memoria
   3..5 0x639660 0x639680 0x6396A0  destructores con borrado: push esi; call CUERPO (6348E0, 634F20, 635100); si byte [esp+8] & 1: call 61A5B0 (delete) con this; eax = this; ret 4
   6  0x6396C0  [this] = 86E518h; indice = [this+0Ch]; si != -1: tabla = 0C178F0h + (indice << 6); dec dword [tabla+3Ch]; si queda a cero y byte [tabla+38h] != 0: byte [tabla+38h] = 0, Flush 632C10 (ecx = tabla); 61A3D0 (ecx = this)
   7  0x421A50  barrido: X = (vx - fcos(angulo) * velocidad), Y = (vy - fsin(angulo) * velocidad) (x87 24 bits, fcos y fsin de 64 bits); vx' = X * 100.0f, vy' = Y * 100.0f (guardados como float); byte [pila+70h] != 0:
            vx', vy' negados y se intercambian los rectangulos (p1,p2,p3,p4) <-> (p5,p6,p7,p8); primera esquina c = p5 (tras el intercambio, p1): P = lado + c, Q0 = V + c, Q1 = P + V; segmentos (origen, lado, inicio, delta)
            contra (p1, p2) y despues (p3, p4): (c, P - c), (P, Q0 - P), (Q0, Q1 - Q0), (Q1, c - Q1); segunda esquina (p7, p8): P = suma con 40FE30, Q2 = V + c, Q3 = V + P y los mismos cuatro segmentos
            contra (p1, p2) y despues (p3, p4); 4138D0 en cada uno (8 floats: p.x, p.y, lado.x, lado.y, inicio.x, inicio.y, delta.x, delta.y); sale con al = 1 en la primera llamada verdadera; si ninguna, al de la ultima
   8  0x422E10  sonda: inicio = (vel * 50.0f) + posicion (matriz+30h si [avion+14h] != 0, si no avion+4); dir.x = fcos(rumbo), dir.y = fsin(rumbo), dir.z = 0 (floats guardados); Normalise 59C910 (ecx = dir);
            S = fsin(elevacion); z0 = 0.0f * S (guardado), S guardado; C = fcos(elevacion); dy = dir.y * C y dz = C * dir.z (guardados), ex = dir.x * C + z0 (guardado); ey = dy + z0 (registro), ez = dz + S guardado
            (registro); fx = ex * 200.0f y fy = ey * 200.0f (guardados), fz = ez * 200.0f (registro); fin = (fx, fy, fz) + inicio (guardado); ProcessLineOfSight 56BA00 (inicio, fin, punto, entidad, 1,0,0,0,0,0,0,1);
            si al: [salida] = z del punto de choque (fld/fstp) y al = 1; si no al = 0
   9  0x422F80  margen: tabla 8A5B2C (6 floats: 3DD67750h, 3D567750h, 0, BEB2B8C3h, BF32B8C3h, BF860A92h), maximo = 0.0f; sonda 422E10 (avion, elevacion, rumbo, &z); si al: primera ranura -> ST0 = 100000.0f; si no,
            si NO (maximo > z) maximo = z; al final ST0 = maximo
   10 0x424160  indice = (vehiculo - [this]) / 0A18h (imul 6572EC3h, sar 6, +1 si negativo); eax = byte [[this+4] + indice] + (indice << 8); ecx = [this+4]; edx = indice << 8
   11..16       ret / ret 44h / ret 30h / ret 38h / ret 44h sin efectos
   17 0x44E480  x87: suma = x*x + y*y; suma <= 0 ordenado -> [this] = 1.0f; si no inv = 1.0 / fsqrt(suma); [this] = inv * x; [this+4] = inv * y
   18 0x44FA30  limpia (bit 1 de [nodo+19h]) los nodos del tipo (byte bajo: 0 = [0, nv), 1 = [nv, n)) de las 72 areas con nodos; cuenta > 0: veces FindNodeClosestToCoors 44F460 (ecx = this; retorno, pos, tipo, dist, flag0, flag1,
            1, flag2, flag3): si el area de la direccion devuelta es 0FFFFh termina; si no marca el nodo (|= 2), escribe la direccion y avanza
   19 0x44FB60  r2 = radio * radio (guardado); por area con nodos y nodo del tipo: bajo trafico (byte, argumento 6): salta con bit 5 de +18h; byte del argumento 8 != 0: salta con bit 1 de +19h; byte del argumento 9 (cl, leido al
            entrar) != (+18h >> 7): salta; centro - nodo (dx guardado,
            dy, dz en registro), (dz*dz + dy*dy) + dx*dx < r2 ordenado: guarda (area, nodo); cmp contador, maxNum tras cada nodo evaluado (sale si igual); devuelve el contador
   20 0x44FEE0  mejor = 10000.0f; por nodo (filtros +18h bit 5, bit 7 contra bBoats): puntuacion ((|z - pz| * 3.0f) + |y - py|) + |x - px| < mejor (ordenada); con enlaces (nibble de +18h > 0): por enlace (area sin acotar: nulo
            salta; filtros; longitud = fsqrt((dz*dz + dy*dy) + dx*dx) > longitud minima) mejor = puntuacion guardada; al final mejor < maxima: escribe primero y segundo, Normalise 59C910 sobre (x1 - x2, y1 - y2, 0) y rumbo =
            fpatan(-nx, ny) * 57.2957763671875 guardado; si no, las areas a 0FFFFh (palabra) y rumbo 0
   21 0x452090  tabla local de 4 direcciones con la palabra de area 0FFFFh; Record 44FA30 (ecx = this; pos, 0, 4, tabla, 497423FEh, bajo trafico, 0, 0, 1); la primera sin area: palabras 0FFFFh en las dos salidas; si no,
            primera salida = la primera y, de las otras tres con area, la primera NO adyacente (These2NodesAreAdjacent 44D230, ecx = 96F050h) a la segunda salida

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction
from math import isqrt

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexUseMobilePhone::CreateFirstSubTask", "CTaskComplexUseMobilePhone::ControlSubTask", "CTaskComplexUseMobilePhone::Clone", "CTaskComplexUseMobilePhone::ScalarDeletingDestructor",
         "CTaskComplexUseGoggles::ScalarDeletingDestructor", "CTaskComplexInWater::ScalarDeletingDestructor", "CTaskComplexUseSequence::DestructorBody", "CCarCtrl::WeaveForOtherCarSweepOverlaps",
         "CCarCtrl::FlyAIPlaneProbeGround", "CCarCtrl::FlyAIPlaneClearanceAhead", "CVehiclePool::GetRef", "CPathFind::PreparePathData", "CPathFind::AllocatePathFindInfoMem", "CPathFind::StoreNodeInfoCar",
         "CPathFind::StoreNodeInfoPed", "CPathFind::StoreDetachedNodeInfoPed", "CPathFind::StoreDetachedNodeInfoCar", "CVector2D::Normalise", "CPathFind::RecordNodesClosestToCoors",
         "CPathFind::RecordNodesInCircle", "CPathFind::FindNodePairClosestToCoors", "CPathFind::Find2NodesForCarCreation"]
ENTRIES = (0x634C10, 0x634D60, 0x636FD0, 0x639660, 0x639680, 0x6396A0, 0x6396C0, 0x421A50, 0x422E10, 0x422F80, 0x424160, 0x44D0E0, 0x44D2B0, 0x44D2C0, 0x44D2D0, 0x44D2E0, 0x44D2F0, 0x44E480, 0x44FA30,
           0x44FB60, 0x44FEE0, 0x452090)
GUARD = 0x40
WIN_A = 0x2000
WIN = 0x1000
WIN_C = 0x4000
VTBL = 0xF00
SEED_AT = 92
SIZES = (GUARD + WIN_A + GUARD, GUARD + WIN + GUARD, GUARD + WIN_C + GUARD, GUARD + WIN + GUARD)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
EV_TYPE = 0x50000001
(PHONEFIRST, PHONECONTROL, PHONECLONE, SDD, SEQDTOR, SWEEP, PROBE, CLEAR, POOLREF, NOP, NORM, RECORD, CIRCLE, PAIR, FIND2) = range(15)
KIND = [PHONEFIRST, PHONECONTROL, PHONECLONE, SDD, SDD, SDD, SEQDTOR, SWEEP, PROBE, CLEAR, POOLREF, NOP, NOP, NOP, NOP, NOP, NOP, NORM, RECORD, CIRCLE, PAIR, FIND2]
SDD_BODY = {3: 0x6348E0, 4: 0x634F20, 5: 0x635100}
DELETE = 0x61A5B0
SEQ_TABLE = 0xC178F0
PROBE_TABLE = (0x3DD67750, 0x3D567750, 0x00000000, 0xBEB2B8C3, 0xBF32B8C3, 0xBF860A92)     # 0x8A5B2C del EXE (seis elevaciones)
K_NODE_BASE = (0x100, 0x500, 0x900)
K_LINK_BASE = (0x40, 0x340, 0x640)
FLT_1566 = Fraction(1.566)     # el doble 1.566 (0x86E4A0), exacto


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

    def locate(self, ptr):
        """(arena, desplazamiento respecto a la ventana) de un puntero absoluto"""
        ptr &= M32
        for i, b in enumerate(self.bases):
            if b <= ptr < b + SIZES[i]:
                return i, ptr - b - GUARD
        raise ValueError('puntero fuera de las arenas %x' % ptr)

    def rdp(self, ptr, n):
        i, off = self.locate(ptr)
        return self.mem[i].rd(off, n)

    def wrp(self, ptr, n, v):
        i, off = self.locate(ptr)
        self.mem[i].wr(off, n, v)

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == PHONEFIRST:
        _k += ['player1', 'not_player1', 'player2', 'not_player2', 'saved_none', 'saved_other', 'slot_negative', 'player_data', 'no_player_data', 'mem_null', 'mem_set']
    elif _kind == PHONECONTROL:
        _k += ['type_ca', 'type_other', 'in_present', 'in_late', 'in_early', 'out_present', 'out_late', 'out_crossing', 'out_no_crossing', 'timer_out', 'timer_not', 'mem_null', 'mem_set', 'player_data',
               'no_player_data', 'onfoot_found', 'onfoot_none', 'swap', 'timecanrun_negative', 'time_in_nan']
    elif _kind == PHONECLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == SEQDTOR:
        _k += ['index_none', 'index_set', 'ref_to_zero', 'ref_nonzero', 'flush', 'flag_clear', 'ref_wrap']
    elif _kind == SWEEP:
        _k += ['flag_swap', 'flag_noswap', 'hit_first', 'hit_mid', 'hit_last', 'no_hit', 'corner2', 'v_nan', 'speed_zero']
    elif _kind == PROBE:
        _k += ['matrix', 'no_matrix', 'hit', 'no_hit', 'elev_nan', 'head_nan', 'elev_zero', 'dir_nan']
    elif _kind == CLEAR:
        _k += ['first_hit', 'no_hit', 'one_hit', 'many_hits', 'max_replaced', 'max_kept', 'nan_z']
    elif _kind == POOLREF:
        _k += ['index_negative', 'index_zero', 'index_positive', 'rem_zero', 'rem_max', 'diff_negative']
    elif _kind == NOP:
        _k = list(COMMON[:8])
    elif _kind == NORM:
        _k += ['sum_zero', 'sum_nan', 'sum_inf', 'normal', 'negative_x', 'sum_tiny']
    elif _kind == RECORD:
        _k += ['type_veh', 'type_ped', 'type_other', 'count_zero', 'count_negative', 'count_one', 'count_many', 'found_all', 'stopped_invalid', 'marks_cleared', 'low_traffic']
    elif _kind == CIRCLE:
        _k += ['type_veh', 'type_ped', 'low_traffic', 'boats_zero', 'boats_one', 'boats_other', 'inside', 'outside', 'max_hit', 'max_negative', 'nan_radius', 'found_some']
    elif _kind == PAIR:
        _k += ['type_veh', 'type_ped', 'low_traffic', 'boats_zero', 'boats_one', 'found', 'none', 'link_null_area', 'link_scored', 'nan_normalise', 'max_nan']
    elif _kind == FIND2:
        _k += ['none', 'first_only', 'second_found', 'all_adjacent', 'first_invalid_node', 'low_traffic']
    KEYS[_fn] = _k


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf_bits(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def fnv(s):
    h = 2166136261
    for b in s:
        h ^= b
        h = (h * 16777619) & M32
    return h


FNV_IN = fnv(b'PhoneIn')
FNV_OUT = fnv(b'PhoneOut')


# ------------------------------------------------------------------------------------------------ aritmetica auxiliar (x87 de 24 bits y de 64 bits)
def lv(bits):
    return v_load(bits)


def sb(v):
    return store32(v)


ZERO = ('fin', Fraction(0), False)
ONE = ('fin', Fraction(1), False)
INDEF = ('nan', 0xFFC00000)


def v_abs(x):
    if x[0] == 'nan':
        return ('nan', x[1] & 0x7FFFFFFF)
    if x[0] == 'inf':
        return ('inf', 1)
    return ('fin', abs(x[1]), False)


def v_chs(x):
    """fchs: invierte el signo (tambien el de un NaN)"""
    if x[0] == 'nan':
        return ('nan', x[1] ^ 0x80000000)
    return v_neg(x)


def v_isnan(x):
    return x[0] == 'nan'


def rnd_bits(v, bits):
    """redondea al mas cercano, empate a par, a `bits` bits de mantisa (exponente ilimitado)"""
    if v == 0:
        return Fraction(0)
    s = -1 if v < 0 else 1
    a = abs(v)
    e = _exp(a)
    q = _round_even(a / _pow2(e - (bits - 1)))
    return s * q * _pow2(e - (bits - 1))


def sqrt24(v):
    """fsqrt a 24 bits (exacto)"""
    if v[0] == 'nan':
        return v
    if v[0] == 'inf':
        return v if v[1] > 0 else INDEF
    fr = v[1]
    if fr == 0:
        return v
    if fr < 0:
        return INDEF
    e = _exp(fr) // 2
    m = fr * _pow2(46 - 2 * e) if 46 - 2 * e >= 0 else fr / _pow2(2 * e - 46)
    n = isqrt(m.numerator // m.denominator)
    quarter = Fraction(2 * n + 1) ** 2 / 4
    if m > quarter or (m == quarter and (n & 1)):
        n += 1
    return ('fin', n * _pow2(e - 23), False)


_mp = None


def mp_ready():
    global _mp
    if _mp is None:
        import mpmath
        mpmath.mp.prec = 400
        _mp = mpmath
    return _mp


def mp_to_fraction(x):
    sign, man, exp, bc = x._mpf_
    if man == 0:
        return Fraction(0)
    f = Fraction(int(man)) * (_pow2(exp) if exp >= 0 else Fraction(1, 1 << -exp))
    return -f if sign else f


_TRIG = {}


def trig(bits):
    """(fsin, fcos) de un float cargado con fld, con 64 bits de mantisa (precision de la x87); NaN -> el propio NaN silenciado, infinito -> indefinido, |x| >= 2^63 -> sin = cos = x (C2 = 1)"""
    if bits in _TRIG:
        return _TRIG[bits]
    x = v_load(bits)
    e = (bits >> 23) & 0xFF
    if x[0] == 'nan':
        r = (x, x)
    elif x[0] == 'inf':
        r = (INDEF, INDEF)
    elif e >= 127 + 63:
        r = (x, x)
    elif x[1] == 0:
        r = (x, ONE)
    else:
        mp = mp_ready()
        xm = mp.mpf(x[1].numerator) / mp.mpf(x[1].denominator)
        s = mp_to_fraction(mp.sin(xm))
        c = mp_to_fraction(mp.cos(xm))
        r = (('fin', rnd_bits(s, 64), False), ('fin', rnd_bits(c, 64), False))
    _TRIG[bits] = r
    return r


def v_fpatan(y, x):
    """fpatan: atan2(y, x) con 64 bits de mantisa; y es ST1 y x es ST0"""
    if y[0] == 'nan' or x[0] == 'nan':
        return _nan_pick(y, x)
    mp = mp_ready()
    sy = _sgn(y)
    pi = mp_to_fraction(+mp.pi)

    def fin(fr):
        return ('fin', rnd_bits(fr, 64), False)

    def zero(sign):
        return ('fin', Fraction(0), sign < 0)

    if y[0] == 'fin' and y[1] == 0:
        if _sgn(x) > 0:
            return zero(sy)
        return fin(sy * pi)
    if y[0] == 'inf':
        if x[0] == 'inf':
            return fin(sy * (pi / 4 if x[1] > 0 else 3 * pi / 4))
        return fin(sy * pi / 2)
    if x[0] == 'inf':
        return zero(sy) if x[1] > 0 else fin(sy * pi)
    if x[1] == 0:
        return fin(sy * pi / 2)
    ym = mp.mpf(y[1].numerator) / mp.mpf(y[1].denominator)
    xm = mp.mpf(x[1].numerator) / mp.mpf(x[1].denominator)
    return fin(mp_to_fraction(mp.atan2(ym, xm)))


def s8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def tdiv(a, b):
    """division entera truncada hacia cero"""
    q = abs(a) // abs(b)
    return q if (a < 0) == (b < 0) else -q


# ------------------------------------------------------------------------------------------------ el mundo de nodos (misma generacion que Fixtures.hpp)
def make_world(seed):
    na = 1 + rnd(seed, 1) % 3
    ids, ns, nvs = [], [], []
    for k in range(3):
        ids.append((rnd(seed, 10 + k) % 24) * 3 + k)
        r = rnd(seed, 20 + k)
        n = 0 if r % 10 < 2 else (r >> 8) % 8
        q = rnd(seed, 30 + k)
        nv = min(n + 1, 8) if q % 10 == 0 else (q >> 8) % (n + 1)
        ns.append(n)
        nvs.append(nv)
    return {'na': na, 'id': ids, 'n': ns, 'nv': nvs}


def node_coord(seed, k, i, c):
    v = rnd(seed, 1000 + k * 100 + i * 10 + c)
    mode, w = v % 8, v >> 3
    if mode < 4:
        r = ((w % 41) - 20) * 8
    elif mode < 6:
        r = (w % 3201) - 1600
    elif mode == 6:
        r = w & 0xFFFF
    else:
        r = (0, 1, -1, 32767, -32768)[w % 5]
    return signed16(r)


def node_flags18(seed, k, i):
    v = rnd(seed, 2000 + k * 100 + i)
    nl = ((v >> 8) & 0xF) if v % 8 == 0 else ((v >> 8) & 0xF) % 5
    return nl | (((v >> 12) & 1) << 4) | ((1 if (v >> 13) % 4 == 0 else 0) << 5) | (((v >> 14) & 1) << 6) | ((1 if (v >> 16) % 4 == 0 else 0) << 7)


def node_flags19(seed, k, i):
    return rnd(seed, 3000 + k * 100 + i) & 0xFF


def node_link_base(seed, k, i):
    return (rnd(seed, 5000 + k * 100 + i) % 40) - 3


def link_entry(seed, k, j):
    w = make_world(seed)
    v = rnd(seed, 6000 + k * 100 + (j + 3))
    sel = v % 20
    if sel < 14:
        area = w['id'][(v >> 8) % w['na']]
    elif sel < 17:
        area = (v >> 8) % 72
    else:
        area = 72 + (v >> 8) % 8
    return (area & 0xFFFF) | ((((v >> 16) % 8) & 0xFFFF) << 16)


class Nodes:
    """vista del mundo de nodos de una semilla: area -> (indice de area k) y lectura de los campos de un nodo; los bytes +19h (marcas) se leen de la arena C"""

    def __init__(self, W, seed):
        self.W, self.seed = W, seed
        self.w = make_world(seed)
        self.by_area = {self.w['id'][k]: k for k in range(self.w['na'])}

    def init_marks(self):
        """escribe en la arena C el byte +19h de todos los nodos (antes de la instantanea): las marcas del banco parten de ahi"""
        C = self.W.mem[2]
        for k in range(self.w['na']):
            for i in range(8):
                C.wr(K_NODE_BASE[k] + 0x1C * i + 0x19, 1, node_flags19(self.seed, k, i))

    def present(self, area):
        return area in self.by_area

    def count(self, area):
        k = self.by_area[area]
        return self.w['n'][k], self.w['nv'][k]

    def b18(self, k, i):
        return node_flags18(self.seed, k, i)

    def b19(self, k, i):
        return self.W.mem[2].rd(K_NODE_BASE[k] + 0x1C * i + 0x19, 1)

    def set_b19(self, k, i, v):
        self.W.mem[2].wr(K_NODE_BASE[k] + 0x1C * i + 0x19, 1, v)

    def xyz(self, k, i):
        return tuple(node_coord(self.seed, k, i, c) for c in range(3))


def node_range(nodes, area, node_type):
    """[primero, ultimo) de los nodos del tipo (byte bajo 0 = vehiculos, 1 = peatones) o None si el EXE no recorre"""
    n, nv = nodes.count(area)
    if (node_type & 0xFF) == 0:
        return 0, nv
    if (node_type & 0xFF) == 1:
        return nv, n
    return None


def scripted_address(seed, sel):
    if sel % 8 == 0:
        return 0xFFFF | ((sel >> 16) << 16)
    w = make_world(seed)
    return (w['id'][(sel >> 3) % w['na']] & 0xFFFF) | ((((sel >> 8) % 8) & 0xFFFF) << 16)


def exact_coord(r):
    """entero de 16 bits * 0.125f (fild + fmul: exacto)"""
    return ('fin', Fraction(r, 8), False)


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    if k != NOP:
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    tag = W.tagv
    gch = []                      # cambios de la memoria global del juego: [[direccion, byte nuevo]]
    A.wr(o, 4, a[3])
    if k == PHONEFIRST:
        ped = a[4]
        Cc.wr(ped + 0x474, 4, a[5]); Cc.wr(ped + 0x478, 4, a[6]); Cc.wr(ped + 0x70C, 4, a[7]); Cc.wr(ped + 0x718, 1, a[8])
        Cc.wr(ped + 0x5A0 + s8(a[8]) * 0x1C, 4, a[9])
        Cc.wr(ped + 0x480, 4, W.raw(3, a[10]) if a[10] else 0)
        if a[10]:
            D.wr(a[10] + 0x85, 1, a[15])
        W.snap()
        pedp = W.raw(2, ped)
        flags_b = Cc.rd(ped + 0x478, 4)
        flags_a = Cc.rd(ped + 0x474, 4)
        Cc.wr(ped + 0x478, 4, flags_b | 2)
        Cc.wr(ped + 0x474, 4, flags_a | 0x400000)
        events.append([0x5DF8F0, tag(pedp)])
        p1, p2 = a[11] & 1, (a[11] >> 1) & 1
        marks.append('player1' if p1 else 'not_player1')
        marks.append('player2' if p2 else 'not_player2')
        if p1:
            events.append([0x609C80, tag(pedp)])
        if Cc.rd(ped + 0x70C, 4) == 0x37:
            slot = s8(Cc.rd(ped + 0x718, 1))
            Cc.wr(ped + 0x70C, 4, Cc.rd(ped + 0x5A0 + slot * 0x1C, 4))
            events.append([0x5E6280, tag(pedp), 0])
            marks.append('saved_none')
        else:
            marks.append('saved_other')
        if s8(a[8]) < 0:
            marks.append('slot_negative')
        pd = Cc.rd(ped + 0x480, 4)
        if pd:
            W.wrp(pd + 0x85, 1, 1)
            marks.append('player_data')
        else:
            marks.append('no_player_data')
        events.append([0x5DF8F0, tag(pedp)])
        if p2:
            events.append([0x50AB10, 0xB6F028])
        events.append([0x62F370, STACK_TAG, tag(pedp), 0, 0, 0, 0, 0, 0, 0, 0x41000000])
        events.append([0x61A5A0, 0x20])
        if a[12]:
            mem = W.raw(1, a[13])
            events.append([0x61A900, tag(mem), 0, 0x91, 0x40800000, 0x642, FNV_IN, 0])
            B.wr(a[13], 4, a[14])
            ret = tag(mem)
            marks.append('mem_set')
        else:
            ret = 0
            marks.append('mem_null')
    elif k == PHONECONTROL:
        ped, sub = a[4], a[11]
        A.wr(o + 8, 4, W.raw(2, sub))
        Cc.wr(sub, 4, W.raw(3, VTBL)); Cc.wr(sub + 0x30, 4, a[12])
        Cc.wr(ped + 0x18, 4, W.raw(3, a[5])); D.wr(a[5], 1, 2); Cc.wr(ped + 0x47C, 4, W.raw(3, a[6]))
        Cc.wr(ped + 0x480, 4, W.raw(3, a[7]) if a[7] else 0)
        if a[7]:
            D.wr(a[7] + 0x84, 1, a[8]); D.wr(a[7] + 0x85, 1, a[9]); D.wr(a[7] + 0x18, 4, a[10])
        if a[13]:
            D.wr(a[13] + 0x20, 4, a[15]); D.wr(a[13] + 0x28, 4, a[16])
        if a[14]:
            D.wr(a[14] + 0x20, 4, a[17]); D.wr(a[14] + 0x28, 4, a[18])
        W.snap()
        pedp = W.raw(2, ped)
        result = A.rd(o + 8, 4)
        pdptr = Cc.rd(ped + 0x480, 4)
        if pdptr:
            W.wrp(pdptr + 0x85, 1, 1)
            marks.append('player_data')
        else:
            marks.append('no_player_data')
        events.append([EV_TYPE, tag(result)])
        typ = Cc.rd(sub + 0x30, 4)
        if typ != 0xCA:
            marks.append('type_other')
            clump = Cc.rd(ped + 0x18, 4)
            events.append([0x4D68B0, tag(clump), 0x91])
            events.append([0x4D68B0, tag(clump), 0x92])
            ain = W.raw(3, a[13]) if a[13] else 0
            aout = W.raw(3, a[14]) if a[14] else 0
            if ain:
                marks.append('in_present')
                t = lv(D.rd(a[13] + 0x20, 4))
                if t[0] == 'nan':
                    marks.append('time_in_nan')
                cmpv = v_cmp(t, lv(0x3F59999A))
                marks.append('in_late' if cmpv in ('gt', 'eq') else 'in_early')
                if cmpv in ('gt', 'eq'):
                    events.append([0x5E5ED0, tag(pedp), 0x14A])
            elif aout:
                marks.append('out_present')
                t = lv(D.rd(a[14] + 0x20, 4))
                st = lv(D.rd(a[14] + 0x28, 4))
                marks.append('out_late' if v_cmp(t, ('fin', FLT_1566, False)) in ('gt', 'eq') else 'out_early')
                if v_cmp(t, ('fin', FLT_1566, False)) in ('gt', 'eq'):
                    diff = v_sub(t, st)
                    if v_cmp(diff, ('fin', FLT_1566, False)) == 'lt':
                        events.append([0x6348F0, tag(this), tag(pedp)])
                        marks.append('out_crossing')
                    else:
                        marks.append('out_no_crossing')
            else:
                events.append([0x420E30, tag(this + 0x10)])
                if (a[19] & 0xFF) != 0:
                    marks.append('timer_out')
                    events.append([0x61A5A0, 0x20])
                    if a[20]:
                        mem = W.raw(1, a[21])
                        events.append([0x61A900, tag(mem), 0, 0x92, 0x40800000, 0x643, FNV_OUT, 0])
                        B.wr(a[21], 4, a[22])
                        result = mem
                        marks.append('mem_set')
                    else:
                        result = 0
                        marks.append('mem_null')
                else:
                    marks.append('timer_not')
        else:
            marks.append('type_ca')
        pd = Cc.rd(ped + 0x480, 4)
        if pd:
            W.wrp(pd + 0x84, 1, 1)
            pd = Cc.rd(ped + 0x480, 4)
            f = lv(W.rdp(pd + 0x18, 4))
            if v_cmp(f, ZERO) == 'lt':
                W.wrp(pd + 0x18, 4, 0)
                marks.append('timecanrun_negative')
            intel = Cc.rd(ped + 0x47C, 4)
            events.append([0x600EE0, tag(intel), 0])
            if a[23]:
                marks.append('onfoot_found')
                onfoot = W.raw(3, a[23])
                pdc = Cc.rd(ped + 0x480, 4)
                events.append([0x6883D0, tag(onfoot), tag(pedp), 1, tag(pdc), W.rdp(pdc + 0x84, 1), W.rdp(pdc + 0x85, 1), W.rdp(pdc + 0x18, 4)])
                if a[24]:
                    Cc.wr(ped + 0x480, 4, W.raw(3, a[25]))
                    marks.append('swap')
            else:
                marks.append('onfoot_none')
            pd = Cc.rd(ped + 0x480, 4)
            W.wrp(pd + 0x84, 1, 0)
        ret = tag(result)
    elif k == PHONECLONE:
        A.wr(o + 0xC, 4, a[4])
        W.snap()
        events.append([0x61A5A0, 0x20])
        if a[5]:
            mem = W.raw(1, a[6])
            dur = A.rd(o + 0xC, 4)
            events.append([0x61A3B0, tag(mem), B.rd(a[6], 4)])
            B.wr(a[6], 4, a[7])
            B.wr(a[6] + 0xC, 4, dur)
            B.wr(a[6], 4, 0x86E454)
            B.wr(a[6] + 0x10, 4, 0); B.wr(a[6] + 0x14, 4, 0)
            for off in (0x18, 0x19, 0x1C, 0x1D):
                B.wr(a[6] + off, 1, 0)
            ret = tag(mem)
            marks.append('mem_set')
        else:
            ret = 0
            marks.append('mem_null')
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
    elif k == SEQDTOR:
        A.wr(o + 0xC, 4, a[4])
        W.snap()
        old_rc = a[5]
        old_flag = a[6] & 0xFF
        A.wr(o, 4, 0x86E518)
        if a[4] != M32:
            marks.append('index_set')
            entry = (SEQ_TABLE + ((a[4] << 6) & M32)) & M32
            new_rc = (old_rc - 1) & M32
            marks.append('ref_wrap' if old_rc == 0 else 'ref_nonzero')
            new_flag = old_flag
            if new_rc == 0:
                marks.append('ref_to_zero')
                if old_flag:
                    new_flag = 0
                    marks.append('flush')
                else:
                    marks.append('flag_clear')
            if new_flag != old_flag:
                gch.append([entry + 0x38, new_flag])
            for kk in range(4):
                ob, nb = (old_rc >> (8 * kk)) & 0xFF, (new_rc >> (8 * kk)) & 0xFF
                if ob != nb:
                    gch.append([entry + 0x3C + kk, nb])
            gch.sort()
            if new_rc == 0 and old_flag:
                events.append([0x632C10, entry, 0, 0])
        else:
            marks.append('index_none')
        events.append([0x61A3D0, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[7])
    elif k == SWEEP:
        pa = []
        for i in range(8):
            if i < 4:
                B.wr(a[5 + i], 4, a[13 + 3 * i]); B.wr(a[5 + i] + 4, 4, a[14 + 3 * i]); B.wr(a[5 + i] + 8, 4, a[15 + 3 * i])
                pa.append((B, a[5 + i]))
            else:
                Cc.wr(a[5 + i], 4, a[13 + 3 * i]); Cc.wr(a[5 + i] + 4, 4, a[14 + 3 * i]); Cc.wr(a[5 + i] + 8, 4, a[15 + 3 * i])
                pa.append((Cc, a[5 + i]))
        W.snap()
        S, Co = trig(a[4])
        speed = lv(a[39])
        X = v_sub(lv(a[37]), v_mul(Co, speed))
        Y = v_sub(lv(a[38]), v_mul(S, speed))
        h100 = lv(0x42C80000)
        vx = sb(v_mul(X, h100))
        vy = sb(v_mul(Y, h100))
        if X[0] == 'nan' or Y[0] == 'nan':
            marks.append('v_nan')
        if (a[39] & 0x7FFFFFFF) == 0:
            marks.append('speed_zero')
        order = [0, 1, 2, 3, 4, 5, 6, 7]
        if (a[40] & 0xFF) != 0:
            vx = sb(v_chs(lv(vx))); vy = sb(v_chs(lv(vy)))
            order = [4, 5, 6, 7, 0, 1, 2, 3]
            marks.append('flag_swap')
        else:
            marks.append('flag_noswap')

        def vec(i):
            ar, off = pa[order[i]]
            return (ar.rd(off, 4), ar.rd(off + 4, 4), ar.rd(off + 8, 4))

        a1, a2, a3, a4 = vec(0), vec(1), vec(2), vec(3)
        b1, b2, b3, b4 = vec(4), vec(5), vec(6), vec(7)
        calls = []

        def test(origin, side, sx, sy, dx, dy):
            calls.append([0x4138D0, origin[0], origin[1], side[0], side[1], sx, sy, dx, dy])

        def add(x, y):
            return sb(v_add(lv(x), lv(y)))

        def sub(x, y):
            return sb(v_sub(lv(x), lv(y)))

        def segments(cx, cy, px, py, qax, qay, qbx, qby):
            """cuatro segmentos: (c, P - c), (P, Qa - P), (Qa, Qb - Qa), (Qb, c - Qb)"""
            return [(cx, cy, sub(px, cx), sub(py, cy)), (px, py, sub(qax, px), sub(qay, py)), (qax, qay, sub(qbx, qax), sub(qby, qay)), (qbx, qby, sub(cx, qbx), sub(cy, qby))]

        # primera esquina de B
        cx, cy = b1[0], b1[1]
        px, py = add(b2[0], cx), add(cy, b2[1])
        q0x, q0y = add(vx, cx), add(vy, cy)
        q1x, q1y = add(px, vx), add(py, vy)
        segs = [(a1, a2, segments(cx, cy, px, py, q0x, q0y, q1x, q1y)), (a3, a4, segments(cx, cy, px, py, q0x, q0y, q1x, q1y))]
        # segunda esquina de B: P = esquina + lado (CVector 0x40FE30)
        cx2, cy2 = b3[0], b3[1]
        px2, py2 = add(b3[0], b4[0]), add(b3[1], b4[1])
        q2x, q2y = add(vx, cx2), add(vy, cy2)
        q3x, q3y = add(vx, px2), add(vy, py2)
        segs += [(a1, a2, segments(cx2, cy2, px2, py2, q2x, q2y, q3x, q3y)), (a3, a4, segments(cx2, cy2, px2, py2, q2x, q2y, q3x, q3y))]
        n = 0
        res = 0
        done = False
        for origin, side, ss in segs:
            for (sx, sy, dx, dy) in ss:
                test(origin, side, sx, sy, dx, dy)
                bit = (a[41] >> (n & 31)) & 1
                n += 1
                res = bit
                if bit:
                    done = True
                    break
            if done:
                break
        events.extend(calls)
        ret = 1 if done else res
        if not done:
            marks.append('no_hit')
        elif n == 1:
            marks.append('hit_first')
        elif n == 16:
            marks.append('hit_last')
        else:
            marks.append('hit_mid')
        if n > 8:
            marks.append('corner2')
    elif k == PROBE:
        plane = a[4]
        for kk in range(3):
            Cc.wr(plane + 0x44 + 4 * kk, 4, a[18 + kk])
        Cc.wr(plane + 0x14, 4, W.raw(3, a[5]) if a[5] else 0)
        for kk in range(3):
            if a[5]:
                D.wr(a[5] + 0x30 + 4 * kk, 4, a[15 + kk])
            else:
                Cc.wr(plane + 4 + 4 * kk, 4, a[15 + kk])
        W.snap()
        marks.append('matrix' if a[5] else 'no_matrix')
        pos = [lv(a[15 + kk]) for kk in range(3)]
        vel = [lv(a[18 + kk]) for kk in range(3)]
        h50 = lv(0x42480000)
        sx = sb(v_mul(vel[0], h50)); sy = sb(v_mul(vel[1], h50))
        zreg = v_mul(vel[2], h50)
        start = (sb(v_add(lv(sx), pos[0])), sb(v_add(lv(sy), pos[1])), sb(v_add(zreg, pos[2])))
        Sh, Ch = trig(a[7])
        dirv = (sb(Ch), sb(Sh), 0)
        events.append([0x59C910, STACK_TAG, dirv[0], dirv[1], dirv[2]])
        if v_isnan(Ch) or v_isnan(Sh):
            marks.append('head_nan')
        if (a[6] & 0x7FFFFFFF) == 0:
            marks.append('elev_zero')
        if v_isnan(trig(a[6])[0]):
            marks.append('elev_nan')
        gv = [lv(a[9]), lv(a[10]), lv(a[11])]
        if any(v_isnan(g) for g in gv):
            marks.append('dir_nan')
        Se, Ce = trig(a[6])
        z0 = v_mul(ZERO, Se)
        zero_stored = sb(z0)
        sine_stored = sb(Se)
        dy = sb(v_mul(gv[1], Ce))
        xc = v_mul(gv[0], Ce)
        dz = sb(v_mul(Ce, gv[2]))
        ex = sb(v_add(xc, lv(zero_stored)))
        ey = v_add(lv(dy), z0)
        ez = v_add(lv(dz), lv(sine_stored))
        h200 = lv(0x43480000)
        fx = sb(v_mul(lv(ex), h200))
        fy = sb(v_mul(ey, h200))
        fz = v_mul(ez, h200)
        end = (sb(v_add(lv(fx), lv(start[0]))), sb(v_add(lv(fy), lv(start[1]))), sb(v_add(fz, lv(start[2]))))
        events.append([0x56BA00, start[0], start[1], start[2], end[0], end[1], end[2], STACK_TAG, STACK_TAG, 1, 0, 0, 0, 0, 0, 0, 1])
        if (a[12] & 0xFF) != 0:
            B.wr(a[8], 4, sb(lv(a[13])))
            ret = 1
            marks.append('hit')
        else:
            ret = 0
            marks.append('no_hit')
    elif k == CLEAR:
        W.snap()
        mx = 0
        ret = None
        pl = tag(a[4])
        hits = 0
        for i in range(6):
            events.append([0x422E10, pl, PROBE_TABLE[i], a[5], STACK_TAG])
            if (a[6] >> i) & 1:
                hits += 1
                if i == 0:
                    ret = 0x47C35000
                    marks.append('first_hit')
                    break
                z = a[7 + i]
                if v_cmp(lv(mx), lv(z)) != 'gt':
                    mx = z
                    marks.append('max_replaced')
                else:
                    marks.append('max_kept')
                if is_nan_bits(z):
                    marks.append('nan_z')
        if ret is None:
            ret = sb(lv(mx))
            if hits == 0:
                marks.append('no_hit')
            elif hits == 1:
                marks.append('one_hit')
            else:
                marks.append('many_hits')
    elif k == POOLREF:
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, W.raw(3, a[5]))
        idx_in = signed32(a[6])
        D.wr(a[5] + idx_in, 1, a[8])
        diff_in = idx_in * 0xA18
        rem = signed32(a[7])
        diff_in += -rem if idx_in < 0 else (rem if idx_in > 0 else (-rem if (a[8] & 1) else rem))
        vehicle = (a[4] + diff_in) & M32
        W.snap()
        diff = signed32(vehicle - a[4])
        hi = (diff * 0x6572EC3) >> 32
        hi >>= 6
        idx = hi + (1 if hi < 0 else 0)
        flags = A.rd(o + 4, 4)
        byte = W.rdp((flags + idx) & M32, 1)
        shifted = (idx << 8) & M32
        ret = [(byte + shifted) & M32, tag(flags), shifted]
        marks.append('index_negative' if idx < 0 else ('index_zero' if idx == 0 else 'index_positive'))
        marks.append('rem_zero' if a[7] == 0 else ('rem_max' if a[7] == 0xA17 else 'rem_other'))
        if diff < 0:
            marks.append('diff_negative')
    elif k == NOP:
        W.snap()
        ret = None
    elif k == NORM:
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, a[5])
        W.snap()
        x, y = lv(a[4]), lv(a[5])
        s = v_add(v_mul(x, x), v_mul(y, y))
        cmpv = v_cmp(s, ZERO)
        if s[0] == 'nan':
            marks.append('sum_nan')
        elif s[0] == 'inf':
            marks.append('sum_inf')
        elif s[1] == 0:
            marks.append('sum_zero')
        else:
            marks.append('normal')
            if s[1] < Fraction(1, 1 << 100):
                marks.append('sum_tiny')
        if a[4] >> 31:
            marks.append('negative_x')
        if cmpv in ('lt', 'eq'):
            A.wr(o, 4, 0x3F800000)
        else:
            inv = v_div(ONE, sqrt24(s))
            A.wr(o, 4, sb(v_mul(inv, lv(A.rd(o, 4)))))
            A.wr(o + 4, 4, sb(v_mul(inv, lv(A.rd(o + 4, 4)))))
    elif k == RECORD:
        nodes = Nodes(W, a[4])
        nodes.init_marks()
        W.snap()
        nt = a[8]
        marks.append('type_veh' if (nt & 0xFF) == 0 else ('type_ped' if (nt & 0xFF) == 1 else 'type_other'))
        if a[12] & 0xFF:
            marks.append('low_traffic')
        for area in sorted(nodes.by_area):
            kk = nodes.by_area[area]
            rng = node_range(nodes, area, nt)
            if rng is None:
                continue
            for i in range(rng[0], rng[1]):
                nodes.set_b19(kk, i, nodes.b19(kk, i) & 0xFD)
                marks.append('marks_cleared')
        count = signed32(a[9])
        if count <= 0:
            marks.append('count_zero' if count == 0 else 'count_negative')
        else:
            marks.append('count_one' if count == 1 else 'count_many')
            out = a[10]
            call = 0
            cursor = 0
            stopped = False
            while count > 0:
                events.append([0x44F460, tag(this), STACK_TAG, a[5], a[6], a[7], a[8], a[11], a[12], a[13], 1, a[14], a[15]])
                addr = scripted_address(a[4], a[16 + (call & 3)])
                call += 1
                if (addr & 0xFFFF) == 0xFFFF:
                    stopped = True
                    break
                area, node = addr & 0xFFFF, addr >> 16
                kk = nodes.by_area[area]
                nodes.set_b19(kk, node, nodes.b19(kk, node) | 2)
                B.wr(out + 4 * cursor, 4, addr)
                cursor += 1
                count -= 1
            marks.append('stopped_invalid' if stopped else 'found_all')
    elif k == CIRCLE:
        nodes = Nodes(W, a[4])
        for kk in range(3):
            B.wr(a[8] + 4 * kk, 4, a[5 + kk])
        W.snap()
        nt = a[10]
        marks.append('type_veh' if (nt & 0xFF) == 0 else 'type_ped')
        low = a[13] & 0xFF
        skip_marked = a[15] & 0xFF
        boats = a[16] & 0xFF
        marks.append('boats_zero' if boats == 0 else ('boats_one' if boats == 1 else 'boats_other'))
        if low:
            marks.append('low_traffic')
        radius = lv(a[9])
        if radius[0] == 'nan':
            marks.append('nan_radius')
        r2 = sb(v_mul(radius, radius))
        maxnum = signed32(a[11])
        if maxnum < 0:
            marks.append('max_negative')
        found = 0
        out = a[12]
        finished = False
        for area in sorted(nodes.by_area):
            kk = nodes.by_area[area]
            rng = node_range(nodes, area, nt)
            for i in range(rng[0], rng[1]):
                b18 = nodes.b18(kk, i)
                b19 = node_flags19(a[4], kk, i)
                if low and (b18 & 0x20):
                    continue
                if skip_marked != 0 and (b19 & 2):
                    continue
                if boats != (b18 >> 7):
                    continue
                xr, yr, zr = nodes.xyz(kk, i)
                x, y = exact_coord(xr), exact_coord(yr)
                zst = sb(exact_coord(zr))
                cen = [B.rd(a[8] + 4 * q, 4) for q in range(3)]
                dx = sb(v_sub(lv(cen[0]), x))
                dy = v_sub(lv(cen[1]), y)
                dz = v_sub(lv(cen[2]), lv(zst))
                s1 = v_add(v_mul(dz, dz), v_mul(dy, dy))
                dxl = lv(dx)
                total = v_add(s1, v_mul(dxl, dxl))
                if v_cmp(total, lv(r2)) == 'lt':
                    B.wr(out + 4 * found, 4, (area & 0xFFFF) | (i << 16))
                    found += 1
                    marks.append('inside')
                else:
                    marks.append('outside')
                if found == maxnum:
                    marks.append('max_hit')
                    finished = True
                    break
            if finished:
                break
        if found:
            marks.append('found_some')
        ret = found
    elif k == PAIR:
        nodes = Nodes(W, a[4])
        B.wr(a[9], 4, a[20]); B.wr(a[10], 4, a[21]); B.wr(a[11], 4, a[22])
        W.snap()
        nt = a[8]
        marks.append('type_veh' if (nt & 0xFF) == 0 else 'type_ped')
        low = a[14] & 0xFF
        boats = a[16] & 0xFF
        marks.append('boats_zero' if boats == 0 else ('boats_one' if boats == 1 else 'boats_other'))
        if low:
            marks.append('low_traffic')
        pos = [lv(a[5]), lv(a[6]), lv(a[7])]
        h3 = lv(0x40400000)
        best = lv(0x461C4000)
        best_first = best_second = None
        minlen = lv(a[12])
        for area in sorted(nodes.by_area):
            kk = nodes.by_area[area]
            rng = node_range(nodes, area, nt)
            for i in range(rng[0], rng[1]):
                b18 = nodes.b18(kk, i)
                if low and (b18 & 0x20):
                    continue
                if boats != (b18 >> 7):
                    continue
                xr, yr, zr = nodes.xyz(kk, i)
                x, y, z = exact_coord(xr), exact_coord(yr), exact_coord(zr)
                dz3 = v_mul(v_abs(v_sub(z, pos[2])), h3)
                dyabs = v_abs(v_sub(y, pos[1]))
                partial = v_add(dz3, dyabs)
                dxabs = v_abs(v_sub(x, pos[0]))
                score = v_add(partial, dxabs)
                if v_cmp(score, best) != 'lt':
                    continue
                nl = b18 & 0xF
                if nl <= 0:
                    continue
                base = node_link_base(a[4], kk, i)
                for l in range(nl):
                    e = link_entry(a[4], kk, base + l)
                    la = e & 0xFFFF
                    if la not in nodes.by_area:
                        marks.append('link_null_area')
                        continue
                    lk = nodes.by_area[la]
                    ln = e >> 16
                    lb18 = nodes.b18(lk, ln)
                    if low and (lb18 & 0x20):
                        continue
                    if boats != (lb18 >> 7):
                        continue
                    lxr, lyr, lzr = nodes.xyz(lk, ln)
                    nx, ny, nz = x, y, z
                    lx, ly = exact_coord(lxr), exact_coord(lyr)
                    lzs = lv(sb(exact_coord(lzr)))
                    dx = lv(sb(v_sub(nx, lx)))
                    dy = v_sub(ny, ly)
                    dz = v_sub(nz, lzs)
                    s1 = v_add(v_mul(dz, dz), v_mul(dy, dy))
                    total = v_add(s1, v_mul(dx, dx))
                    length = sqrt24(total)
                    if v_cmp(length, minlen) == 'gt':
                        best = lv(sb(score))
                        best_first = (area & 0xFFFF) | (i << 16)
                        best_second = e
                        marks.append('link_scored')
        if v_cmp(best, lv(a[13])) == 'lt':
            marks.append('found')
            B.wr(a[9], 4, best_first)
            B.wr(a[10], 4, best_second)
            fa, fnode = best_first & 0xFFFF, best_first >> 16
            sa, snode = best_second & 0xFFFF, best_second >> 16
            fk, sk = nodes.by_area[fa], nodes.by_area[sa]
            fx, fy, _fz = nodes.xyz(fk, fnode)
            sx, sy, _sz = nodes.xyz(sk, snode)
            dirx = sb(v_sub(exact_coord(fx), exact_coord(sx)))
            diry = sb(v_sub(exact_coord(fy), exact_coord(sy)))
            events.append([0x59C910, STACK_TAG, dirx, diry, 0])
            nxv, nyv = lv(a[17]), lv(a[18])
            if nxv[0] == 'nan' or nyv[0] == 'nan':
                marks.append('nan_normalise')
            ang = v_fpatan(v_chs(nxv), nyv)
            B.wr(a[11], 4, sb(v_mul(ang, lv(0x42652EE0))))
        else:
            marks.append('none')
            if lv(a[13])[0] == 'nan':
                marks.append('max_nan')
            B.wr(a[9], 2, 0xFFFF); B.wr(a[10], 2, 0xFFFF); B.wr(a[11], 4, 0)
    elif k == FIND2:
        W.snap()
        n_rec = a[10]
        closest = [0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF]                 # solo la palabra de area; la del nodo, basura (nunca se lee)
        events.append([0x44FA30, tag(this), a[4], a[5], a[6], 0, 4, STACK_TAG, 0x497423FE, a[9], 0, 0, 1])
        for i in range(min(n_rec, 4)):
            sel = a[11 + i]
            closest[i] = (0xFFFF | ((sel >> 16) << 16)) if sel % 8 == 0 else ((((sel >> 3) % 72)) | ((sel >> 16) << 16))
        if a[9] & 0xFF:
            marks.append('low_traffic')
        first = closest[0]
        if (first & 0xFFFF) == 0xFFFF:
            B.wr(a[7], 2, 0xFFFF); B.wr(a[8], 2, 0xFFFF)
            marks.append('none' if n_rec == 0 else 'first_invalid_node')
        else:
            B.wr(a[7], 4, first)
            marks.append('first_only')
            call = 0
            all_adjacent = True
            for i in range(1, 4):
                if (closest[i] & 0xFFFF) == 0xFFFF:
                    continue
                events.append([0x44D230, 0x96F050, first, closest[i]])
                adj = (a[15] >> (call & 31)) & 1
                call += 1
                if not adj:
                    B.wr(a[8], 4, closest[i])
                    marks.append('second_found')
                    all_adjacent = False
                    break
            if all_adjacent and call:
                marks.append('all_adjacent')
    changes = {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}
    return {'events': events, 'changes': changes, 'gchanges': gch, 'ret': ret, 'marks': marks}


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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado; en GetRef tambien ECX y EDX, comprobados en `ret`)"""
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
