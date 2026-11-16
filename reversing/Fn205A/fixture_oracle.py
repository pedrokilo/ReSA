"""Oraculo independiente (Python) de Fn205A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 205A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0..9  0x4B6170 + 0x50 * k (k = 0..9; clases CEventSexyVehicle, PedToChase, PedToFlee, Attractor, VehicleToSteal, AcquaintancePed, VehicleThreat, KnockOffBike, GunAimedAt, ScriptCommand), 76 bytes iguales: push esi; push edi; edi = ecx;
            call <cuerpo>; si byte [esp+0Ch] & 1: pool = [0B744ACh]; ecx = this - [pool]; edx:eax = ecx * 78787879h; edx = sar(edx, 5); edx += (edx >>> 31) (indice = this - almacenamiento dividido por 44h con signo);
            byte [[pool+4] + indice] |= 80h; si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; eax = this; ret 4. Cuerpos: 4AF0A0, 4AF1C0, 4AF2D0, 4AF400, 4AF700, 4AF890, 4AFBF0, 4AFE70, 4B07B0, 4B0A50.
  10  0x66F650  CTaskComplexWanderGang destructor con borrado: push esi; esi = ecx; call 66F670; si byte [esp+8] & 1: push esi; call 61A5B0; add esp, 4; eax = esi; ret 4
  11  0x66F670  cuerpo del destructor: mov [ecx], 86FE84h; jmp 61A3D0
  12  0x66F680  CTaskComplexAvoidOtherPedWhileWandering destructor con borrado: igual con el cuerpo 66A1D0
  13  0x66F6A0  CTaskComplexAvoidEntity destructor con borrado: igual con el cuerpo 66AAE0
  14  0x66F6C0  ComputeSphere(salida, peds[16]) `ret 8` (ECX sin leer): para cada ped no nulo (en orden 0..15): n++; pos = [ped+14h] ? [ped+14h]+30h : ped+4; sum.x/y/z = (fld sum; fadd pos; fstp sum) (float);
            inv = 1.0f / (float) n (fild n; fdivr [858624h], queda en ST0); centro.x = sum.x * inv; centro.y = sum.y * inv (fld; fmul st, st(1); fstp); centro.z = 0; max = 0.0f ([858B50h]); para cada ped no nulo: dx = pos.x - centro.x;
            dy = pos.y - centro.y (floats de la x87, fsub); r = [[[0A9B0C8h + movsx word [ped+22h] * 4] + 14h] + 24h]; T = ((dy*dy) + (dx*dx)) + (r*r) en la x87 (24 bits, exponente ampliado); fst dword [tmp] (float);
            fcomp max: si NO (T <= max o desordenado): max = el float guardado; al final fsqrt(max); + 0.7f ([86FCC8h]); fstp dword; CColSphere::Set (40FD10; ecx = salida; pila: radio, &centro, 0, 0, 0FFh)
  15  0x66FD30  ComputeRouteRoundSphere(ped) `ret 4`: array de 16 punteros en la pila con el primero = [this+0Ch] y quince a cero; esfera = 66F6C0 (ecx = this, salida, array); [esfera+10h] = z del ped
            (matriz [ped+14h] ? [matriz+38h] : [ped+0Ch], copia de dword); 5F1890 (cdecl: ped, esfera, this+14h, this+20h, this+38h, this+2Ch); eax = el suyo
  16  0x5E9680  IsAtHeadOfQueue: mov al, 1; ret 4
  17  0x5EAFE0  CPedAttractor::operator delete(obj): pool = [0B744C4h]; ecx = obj - [pool]; edx:eax = ecx * 5397829Dh; edx = sar(edx, 6); edx += (edx >>> 31); byte [[pool+4] + indice] |= 80h; si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; ret
  18,20,23  0x5EB140 / 0x5EB1B0 / 0x5EB260  copy_backward 4 bytes (out, first, last, destEnd): si first != last: hacer { dword [destEnd-4] = dword [last-4]; last -= 4; destEnd -= 4 } mientras last != first; [out] = destEnd; eax = out
  19  0x5EB170  envoltorio: llama 5EAC50(out, first, last, destEnd, destEnd) (cdecl); eax = out (esi)
  21  0x5EB1E0  fill 12 bytes (first, last, valor*): si first != last: hacer { [first] = [valor]; [first+4] = [valor+4]; [first+8] = [valor+8] (leyendo y escribiendo en ese orden); first += 12 } mientras first != last
  22  0x5EB220  envoltorio hacia 5EACC0, igual que 19
  24,26,28  0x5EB2A0 / 0x5EB310 / 0x5EB390  uninitialized_fill_n 4 bytes (dest, n, valor*, relleno, relleno): si n == 0 (SIN signo, jbe) nada; hacer n veces { si dest != 0: [dest] = [valor]; dest += 4 }
  25  0x5EB2E0  igual con rep movsd de 5 dwords (dest += 14h)
  27  0x5EB340  igual con tres dwords en orden (dest += 0Ch)
  29  0x5EB5F0  RemoveEffect(fx, SArray*) `ret 8`: cuenta = [v+4] ? sar([v+8] - [v+4], 2) : 0; para i = 0..cuenta-1 (con signo; relee [v+4] en cada vuelta): atractor = [[v+4] + 4i]; si [atractor+4] == fx: call 5EAF60 (ecx = atractor); al = 0
  30  0x5EB640  IsPedRegistered(ped, SArray*) `ret 8`: cursor = [v+4] (una lectura); para cada atractor: call 5EB4C0 (ecx = [cursor], ped); si al != 0: al = 1; al = 0
  31  0x5EB690  IsPedRegisteredWithEffect(ped, fx, entidad, SArray*) `ret 10h`: para cada atractor con [+4] == fx y [+8] == entidad: call 5EB4C0 (ecx = atractor, ped); si al != 0: al = 1; al = 0

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CEventSexyVehicle::ScalarDeletingDestructor", "CEventPedToChase::ScalarDeletingDestructor", "CEventPedToFlee::ScalarDeletingDestructor", "CEventAttractor::ScalarDeletingDestructor",
         "CEventVehicleToSteal::ScalarDeletingDestructor", "CEventAcquaintancePed::ScalarDeletingDestructor", "CEventVehicleThreat::ScalarDeletingDestructor", "CEventKnockOffBike::ScalarDeletingDestructor",
         "CEventGunAimedAt::ScalarDeletingDestructor", "CEventScriptCommand::ScalarDeletingDestructor", "CTaskComplexWanderGang::ScalarDeletingDestructor", "CTaskComplexWanderGang::DestructorBody",
         "CTaskComplexAvoidOtherPedWhileWandering::ScalarDeletingDestructor", "CTaskComplexAvoidEntity::ScalarDeletingDestructor", "CTaskComplexAvoidEntity::ComputeSphere",
         "CTaskComplexAvoidEntity::ComputeRouteRoundSphere", "CPedShelterAttractor::IsAtHeadOfQueue", "CPedAttractor::operator delete", "SArray::CopyBackwardStride4A", "SArray::CopyBackwardStride20Wrapper",
         "SArray::CopyBackwardStride4B", "SArray::FillStride12", "SArray::CopyBackwardStride12Wrapper", "SArray::CopyBackwardStride4C", "SArray::FillNStride4A", "SArray::FillNStride20", "SArray::FillNStride4B",
         "SArray::FillNStride12", "SArray::FillNStride4C", "CPedAttractorManager::RemoveEffect<CPedAttractor>", "CPedAttractorManager::IsPedRegistered<CPedAttractor>",
         "CPedAttractorManager::IsPedRegisteredWithEffect<CPedAttractor>"]
ENTRIES = (0x4B6170, 0x4B61C0, 0x4B6210, 0x4B6260, 0x4B62B0, 0x4B6300, 0x4B6350, 0x4B63A0, 0x4B63F0, 0x4B6440, 0x66F650, 0x66F670, 0x66F680, 0x66F6A0, 0x66F6C0, 0x66FD30,
           0x5E9680, 0x5EAFE0, 0x5EB140, 0x5EB170, 0x5EB1B0, 0x5EB1E0, 0x5EB220, 0x5EB260, 0x5EB2A0, 0x5EB2E0, 0x5EB310, 0x5EB340, 0x5EB390, 0x5EB5F0, 0x5EB640, 0x5EB690)
GUARD = 0x40
WIN = 0x1000
WIN_A = 0x1000
SIZES = (GUARD + WIN_A + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
(EVTSDD, TASKSDD, BODY, SPHERE, ROUTE, BOOL1, ATTRDEL, COPYBACK, WRAP, FILL12, FILLN, REMOVE, ISREG, ISREGEFF) = range(14)
KIND = [EVTSDD] * 10 + [TASKSDD, BODY, TASKSDD, TASKSDD, SPHERE, ROUTE, BOOL1, ATTRDEL, COPYBACK, WRAP, COPYBACK, FILL12, WRAP, COPYBACK, FILLN, FILLN, FILLN, FILLN, FILLN, REMOVE, ISREG, ISREGEFF]
SDD_BODY = {0: 0x4AF0A0, 1: 0x4AF1C0, 2: 0x4AF2D0, 3: 0x4AF400, 4: 0x4AF700, 5: 0x4AF890, 6: 0x4AFBF0, 7: 0x4AFE70, 8: 0x4B07B0, 9: 0x4B0A50, 10: 0x66F670, 12: 0x66A1D0, 13: 0x66AAE0}
WRAP_TARGET = {19: 0x5EAC50, 22: 0x5EACC0}
ELEM_DW = {24: 1, 25: 5, 26: 1, 27: 3, 28: 1}
DELETE, BODYTARGET, BODY_VT = 0x61A5B0, 0x61A3D0, 0x86FE84
SETFN, ROUTEFN, SPHEREFN, ABORT, ISREGWITHPED = 0x40FD10, 0x5F1890, 0x66F6C0, 0x5EAF60, 0x5EB4C0
EVT_POOL_GLOBAL, ATTR_POOL_GLOBAL = 0xB744AC, 0xB744C4
MODEL_TABLE = 0xA9B0C8
F_ONE, F_ZERO, F_PAD07 = 0x3F800000, 0x00000000, 0x3F333333       # [858624h], [858B50h], [86FCC8h]
SEL_WORDS, SLOT_WORDS = 16, 8
MI_BASE, MI_STRIDE, CM_BASE, CM_STRIDE, PED_STRIDE, MAT_STRIDE = 0x400, 0x20, 0x600, 0x30, 0x40, 0x40


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


def v_dbl(x):
    """fadd st, st: exacto (duplicar no redondea) salvo NaN, que se silencia"""
    if x[0] != 'fin':
        return x
    return ('fin', x[1] * 2, x[2])


def v_mul(x, y):
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    if x[0] == 'inf' or y[0] == 'inf':
        z = (x[0] == 'fin' and x[1] == 0) or (y[0] == 'fin' and y[1] == 0)
        if z:
            return ('nan', 0xFFC00000)
        sx = x[1] if x[0] == 'inf' else (1 if x[1] > 0 else -1)
        sy = y[1] if y[0] == 'inf' else (1 if y[1] > 0 else -1)
        return ('inf', sx * sy)
    return ('fin', r24(x[1] * y[1]), False)


def sqrt24(fr):
    if fr == 0:
        return Fraction(0)
    s = 0
    while fr * (1 << (2 * s)) < (1 << 70):
        s += 1
    n = (fr * (1 << (2 * s))).numerator // (fr * (1 << (2 * s))).denominator
    r = math.isqrt(n)
    exact = (r * r == fr * (1 << (2 * s)))
    val = Fraction(2 * r + (0 if exact else 1), 2 * (1 << s))
    return r24(val)


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


def fnv_changes(changes):
    h = 2166136261
    for off, byte in changes:
        for b in (off & M32).to_bytes(4, 'little') + bytes([byte]):
            h = ((h ^ b) * 16777619) & M32
    return h


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta; memoria global con sus bytes"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))
        self.glob = {}
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + SIZES[i]:
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
            if b <= v < b + SIZES[i]:
                return 0x10000000 + (i << 20) + (v - b)
        return v

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



# ------------------------------------------------------------------------------------------------ operaciones x87 adicionales
def v_div(x, y):
    if x[0] == 'nan' or y[0] == 'nan':
        return _nan_pick(x, y)
    if x[0] == 'inf' and y[0] == 'inf':
        return ('nan', 0xFFC00000)
    sx = x[1] if x[0] == 'inf' else (-1 if (x[1] < 0 or (x[1] == 0 and x[2])) else 1)
    sy = y[1] if y[0] == 'inf' else (-1 if (y[1] < 0 or (y[1] == 0 and y[2])) else 1)
    if x[0] == 'inf':
        return ('inf', sx * sy)
    if y[0] == 'inf':
        return ('fin', Fraction(0), sx * sy < 0)
    if y[1] == 0:
        if x[1] == 0:
            return ('nan', 0xFFC00000)
        return ('inf', sx * sy)
    q = x[1] / y[1]
    if q == 0:
        return ('fin', Fraction(0), sx * sy < 0)
    return ('fin', r24(q), False)


def v_sqrt(x):
    if x[0] == 'nan':
        return x
    if x[0] == 'inf':
        return x if x[1] > 0 else ('nan', 0xFFC00000)
    if x[1] == 0:
        return x
    if x[1] < 0:
        return ('nan', 0xFFC00000)
    return ('fin', sqrt24(x[1]), False)


def v_cmp(x, y):
    """fcomp: 'gt', 'lt', 'eq' o 'un'"""
    if x[0] == 'nan' or y[0] == 'nan':
        return 'un'

    def val(v):
        if v[0] == 'inf':
            return v[1] * (1 << 100000)
        return v[1]
    a, b = val(x), val(y)
    return 'gt' if a > b else ('lt' if a < b else 'eq')


def f_bits(v):
    return store32(v)


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == EVTSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'idx_negative', 'idx_zero', 'idx_positive', 'cursor_lowered', 'cursor_kept', 'state_was_set', 'extra_offset']
    elif _kind == ATTRDEL:
        _k += ['idx_negative', 'idx_zero', 'idx_positive', 'cursor_lowered', 'cursor_kept', 'state_was_set', 'extra_offset']
    elif _kind == TASKSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == BODY:
        _k += ['prev_zero', 'prev_other']
    elif _kind == SPHERE:
        _k += ['count_zero', 'count_one', 'count_many', 'uses_matrix', 'uses_placement', 'model_negative', 'model_high', 'center_nan', 'radius_inf', 'radius_finite', 'dup_ped']
    elif _kind == ROUTE:
        _k += ['matrix', 'placement', 'entity_null', 'entity_set', 'ret_true', 'ret_false']
    elif _kind == COPYBACK:
        _k += ['empty', 'one', 'many', 'out_separate', 'out_in_dest', 'out_in_source', 'overlap', 'unaligned_data']
    elif _kind == WRAP:
        _k += ['arena_args', 'raw_args']
    elif _kind == FILL12:
        _k += ['empty', 'one', 'many', 'value_separate', 'value_in_dest']
    elif _kind == FILLN:
        _k += ['n_zero', 'dest_null', 'dest_set', 'value_separate', 'value_in_dest', 'n_many']
    elif _kind == REMOVE:
        _k += ['count_zero', 'count_many', 'fx_match', 'no_match', 'first_null', 'negative_count']
    elif _kind in (ISREG, ISREGEFF):
        _k += ['count_zero', 'count_many', 'found', 'not_found', 'first_null', 'negative_count']
    KEYS[_fn] = _k


def words_of(hexs):
    b = bytes.fromhex(hexs)
    return [int.from_bytes(b[i:i + 4], 'little') for i in range(0, len(b), 4)]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k in (EVTSDD, ATTRDEL):
        evt = k == EVTSDD
        stride = 0x44 if evt else 0xC4
        magic, shift = (0x78787879, 5) if evt else (0x5397829D, 6)
        delta = signed32(a[5])
        idx0 = abs(delta) // stride * (1 if delta >= 0 else -1)
        B.wr(a[4] + 0, 4, (this - a[5]) & M32); B.wr(a[4] + 4, 4, W.raw(3, a[6])); B.wr(a[4] + 8, 4, a[12]); B.wr(a[4] + 0xC, 4, a[10])
        D.wr(a[6] + idx0, 1, a[11])
        W.snap()
        do_free = True
        if evt:
            events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
            A.wr(o, 4, a[9])
            flags = a[7] & 0xFF
            marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[7] >> 8 else 'flag_byte_only')
            do_free = bool(flags & 1)
        if do_free:
            storage = B.rd(a[4], 4)
            diff = signed32((this - storage) & M32)
            prod = diff * magic
            edx = (prod >> 32) >> shift            # sar: aritmetico (Python desplaza con signo)
            idx = signed32(edx + ((edx & M32) >> 31))
            state_ptr = B.rd(a[4] + 4, 4)
            st = W.rd((state_ptr + idx) & M32, 1)
            marks.append('state_was_set' if st & 0x80 else 'state_clear')
            W.wr((state_ptr + idx) & M32, 1, st | 0x80)
            last = signed32(B.rd(a[4] + 0xC, 4))
            if idx < last:
                B.wr(a[4] + 0xC, 4, idx & M32)
                marks.append('cursor_lowered')
            else:
                marks.append('cursor_kept')
            marks.append('idx_negative' if idx < 0 else ('idx_zero' if idx == 0 else 'idx_positive'))
            if diff % stride:
                marks.append('extra_offset')
        if evt:
            ret = W.tagv(this)
    elif k == TASKSDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == BODY:
        W.snap()
        A.wr(o, 4, BODY_VT)
        events.append([BODYTARGET, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BOOL1:
        W.snap()
        ret = 1
    elif k == SPHERE:
        s = words_of(c['s'])
        out = W.raw(1, a[4])
        W.snap()
        sel = s[:SEL_WORDS]
        pool = s[SEL_WORDS + 16 * SLOT_WORDS: SEL_WORDS + 16 * SLOT_WORDS + 8]
        rads = s[SEL_WORDS + 16 * SLOT_WORDS + 8: SEL_WORDS + 16 * SLOT_WORDS + 16]
        peds = []
        for i in range(SEL_WORDS):
            if sel[i] == 0:
                continue
            j = sel[i] - 1
            sl = s[SEL_WORDS + j * SLOT_WORDS: SEL_WORDS + (j + 1) * SLOT_WORDS]
            pos = (sl[4], sl[5], sl[6]) if sl[3] else (sl[0], sl[1], sl[2])
            q = sl[7]
            peds.append((pos, signed16(pool[q]), rads[q], j, bool(sl[3])))
        n = len(peds)
        marks.append('count_zero' if n == 0 else ('count_one' if n == 1 else 'count_many'))
        if len({p[3] for p in peds}) < n:
            marks.append('dup_ped')
        if any(p[4] for p in peds):
            marks.append('uses_matrix')
        if any(not p[4] for p in peds):
            marks.append('uses_placement')
        if any(p[1] < 0 for p in peds):
            marks.append('model_negative')
        if any(p[1] >= 20000 for p in peds):
            marks.append('model_high')
        sums = [F_ZERO, F_ZERO, F_ZERO]
        for p in peds:
            for t in range(3):
                sums[t] = store32(v_add(v_load(sums[t]), v_load(p[0][t])))
        inv = v_div(v_load(F_ONE), ('fin', Fraction(n), False))
        cx = store32(v_mul(v_load(sums[0]), inv))
        cy = store32(v_mul(v_load(sums[1]), inv))
        if cx & 0x7F800000 == 0x7F800000 and cx & 0x7FFFFF or cy & 0x7F800000 == 0x7F800000 and cy & 0x7FFFFF:
            marks.append('center_nan')
        mx = v_load(F_ZERO)
        for p in peds:
            dx = v_sub(v_load(p[0][0]), v_load(cx))
            dy = v_sub(v_load(p[0][1]), v_load(cy))
            r = v_load(p[2])
            t = v_add(v_add(v_mul(dy, dy), v_mul(dx, dx)), v_mul(r, r))
            tbits = store32(t)
            if v_cmp(t, mx) == 'gt':
                mx = v_load(tbits)
        rad = store32(v_add(v_sqrt(mx), v_load(F_PAD07)))
        if rad == 0x7F800000:
            marks.append('radius_inf')
        elif rad & 0x7F800000 != 0x7F800000:
            marks.append('radius_finite')
        events.append([SETFN, W.tagv(out), rad, cx, cy, 0, 0, 0, 0xFF])
    elif k == ROUTE:
        ped = W.raw(1, a[5])
        A.wr(o + 0xC, 4, a[4])
        B.wr(a[5] + 4, 4, a[6]); B.wr(a[5] + 8, 4, a[7]); B.wr(a[5] + 0xC, 4, a[8])
        matrix = W.raw(2, a[10]) if a[9] else 0
        B.wr(a[5] + 0x14, 4, matrix)
        if matrix:
            Cc.wr(a[10] + 0x30, 4, a[11]); Cc.wr(a[10] + 0x34, 4, a[12]); Cc.wr(a[10] + 0x38, 4, a[13])
        W.snap()
        marks.append('matrix' if matrix else 'placement')
        marks.append('entity_set' if a[4] else 'entity_null')
        events.append([SPHEREFN, W.tagv(this), STACK_TAG, STACK_TAG, W.tagv(a[4])] + [0] * 15)
        z = a[13] if matrix else a[8]
        events.append([ROUTEFN, W.tagv(ped), a[14], a[15], z, a[17], W.tagv((this + 0x14) & M32), W.tagv((this + 0x20) & M32), W.tagv((this + 0x38) & M32), W.tagv((this + 0x2C) & M32)])
        ret = a[18] & 0xFF
        marks.append('ret_true' if ret else 'ret_false')
    elif k == COPYBACK:
        cnt = a[6]
        sa = a[7] % 4
        sfirst = a[8] + a[9]
        slast = sfirst + 4 * cnt
        mode = a[10]
        if mode == 0:
            da = (sa + 1 + a[11] % 3) % 4
            dend = a[11] + a[12] + 4 * cnt
        elif mode == 1:
            da = sa
            dend = slast + 4 * signed32(a[13])
        else:
            da = sa
            dend = slast + 0x300
        dstart = dend - 4 * cnt
        if a[4] == 1 and cnt:
            oa, ooff = da, dstart + 4 * a[14]
        elif a[4] == 2 and cnt:
            oa, ooff = sa, sfirst + 4 * a[14]
        else:
            oa, ooff = 1, a[5]
        out = W.raw(oa, ooff)
        W.snap()
        marks.append('empty' if cnt == 0 else ('one' if cnt == 1 else 'many'))
        marks.append('out_separate' if (a[4] == 0 or cnt == 0) else ('out_in_dest' if a[4] == 1 else 'out_in_source'))
        if mode == 1 and cnt:
            marks.append('overlap')
        if a[9] or a[12]:
            marks.append('unaligned_data')
        last_p, dest_p, first_p = W.raw(sa, slast), W.raw(da, dend), W.raw(sa, sfirst)
        if first_p != last_p:
            while True:
                val = W.rd((last_p - 4) & M32, 4)
                last_p = (last_p - 4) & M32
                dest_p = (dest_p - 4) & M32
                W.wr(dest_p, 4, val)
                if last_p == first_p:
                    break
        W.wr(out, 4, dest_p)
        ret = W.tagv(out)
    elif k == WRAP:
        args = []
        for i in range(4):
            if a[8 + i]:
                args.append(W.raw((a[8 + i] >> 16) - 1, a[8 + i] & 0xFFFF))
            else:
                args.append(a[4 + i])
        W.snap()
        marks.append('arena_args' if any(a[8:12]) else 'raw_args')
        events.append([WRAP_TARGET[fn]] + [W.tagv(v) for v in args])
        ret = W.tagv(args[0])
    elif k == FILL12:
        cnt = a[5]
        first = W.raw(1, a[4] + a[8])
        if a[6] == 0:
            val = W.raw(2, a[7] + a[9])
        elif a[6] == 1:
            val = (first + 4 * signed32(a[7])) & M32
        else:
            val = a[10]
        W.snap()
        marks.append('empty' if cnt == 0 else ('one' if cnt == 1 else 'many'))
        marks.append('value_in_dest' if a[6] == 1 else 'value_separate')
        for e in range(cnt):
            dst = (first + 12 * e) & M32
            for t in range(3):
                W.wr((dst + 4 * t) & M32, 4, W.rd((val + 4 * t) & M32, 4))
    elif k == FILLN:
        e = ELEM_DW[fn]
        cnt = a[6]
        dest = 0 if a[4] == 0 else W.raw(1, a[5] + a[9])
        if a[7] == 0:
            val = W.raw(2, a[8] + a[10])
        elif a[7] == 1:
            val = (dest + 4 * signed32(a[8])) & M32
        else:
            val = a[11]
        W.snap()
        marks.append('n_zero' if cnt == 0 else ('n_many' if cnt > 1 else 'n_one'))
        marks.append('dest_null' if dest == 0 else 'dest_set')
        marks.append('value_in_dest' if a[7] == 1 else 'value_separate')
        for _ in range(cnt):
            if dest:
                for t in range(e):
                    W.wr((dest + 4 * t) & M32, 4, W.rd((val + 4 * t) & M32, 4))
            dest = (dest + 4 * e) & M32
    elif k in (REMOVE, ISREG, ISREGEFF):
        m = a[9]
        fx, ent, ped = a[6], a[7], a[8]
        first_arr = W.raw(3, a[5])
        for i in range(m):
            D.wr(a[5] + 4 * i, 4, W.raw(1, 0x40 * a[10 + i]))
        for slot in range(8):
            B.wr(0x40 * slot + 4, 4, fx if a[18 + slot] == 0 else (fx + 1 + slot) & M32)
            B.wr(0x40 * slot + 8, 4, ent if a[26 + slot] == 0 else (ent + 1 + slot) & M32)
        mode = a[35]
        if mode == 1:
            first, last = 0, 0x12345678
        elif mode == 2:
            first = first_arr
            last = (first - 4 * (1 + a[9] % 8)) & M32
        elif mode == 3:
            first = last = first_arr
        elif mode == 4:
            first = first_arr
            last = (first + 0x80000000 + 4 * a[9]) & M32
        else:
            first = first_arr
            last = (first + 4 * m) & M32
        Cc.wr(a[4] + 4, 4, first); Cc.wr(a[4] + 8, 4, last)
        W.snap()
        count = 0 if first == 0 else (signed32((last - first) & M32) >> 2)
        marks.append('count_zero' if count <= 0 else 'count_many')
        if mode == 1:
            marks.append('first_null')
        if count < 0 and first:
            marks.append('negative_count')
        calls = 0
        mask = a[34]
        if k == REMOVE:
            hit = False
            for i in range(max(count, 0)):
                att = W.rd((first + 4 * i) & M32, 4)
                if W.rd((att + 4) & M32, 4) == fx:
                    hit = True
                    events.append([ABORT, W.tagv(att), W.rd(att, 4)])
            marks.append('fx_match' if hit else 'no_match')
            ret = 0
        else:
            ret = 0
            for i in range(max(count, 0)):
                att = W.rd((first + 4 * i) & M32, 4)
                if k == ISREGEFF and not (W.rd((att + 4) & M32, 4) == fx and W.rd((att + 8) & M32, 4) == ent):
                    continue
                events.append([ISREGWITHPED, W.tagv(att), W.tagv(ped)])
                res = (mask >> calls) & 1
                calls += 1
                if res:
                    ret = 1
                    break
            marks.append('found' if ret else 'not_found')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX/AL (valor devuelto, ya comprobado)"""
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
