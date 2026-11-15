"""Oraculo independiente (Python) de Fn199A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 30 funciones del lote 199A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x620BF0  CTaskComplexReactToAttack::MakeAbortable: si el byte [this+0Dh] != 0: AL = 1; si no, con prioridad distinta de 1 y de 2: AL = 0 (sin tocar el byte); si no llama a [[this+8]+18h](ped, prioridad, evento)
            y guarda su AL en [this+0Dh] (retorno = AL)
   1  0x620C30  CreateSubTask(tipo): tipo > 3F0h (con signo): 0; = 3F0h: new(28h) 61A5A0 y ctor 620780(agresor si [this+14h] != 0 y ([agresor+36h] & 7) == 3, si no 0; [this+20h]; [this+1Ch]; _ftol([this+18h]) (64 bits, parte baja));
            = C8h: new(8) y ctor de CTaskSimple 61A390 y [mem] = 859CFCh; = D0h: new(18h) y ctor 6786C0(1Ah si [this+1Ch] == 2 si no 14h, 0, 3E8h); otro: 0. Sin memoria: 0 (los constructores no se llaman). EAX = el del ctor
            (BeHit y Fall) o la memoria (None)
   2  0x620E00  ControlSubTask(ped): si [this+0Ch] != 0: devuelve [[this]+24h](ped); si no llama a [[this+8]+10h]() descartando EAX y devuelve [this+8] releido
   3  0x623300  Clone: new(24h) 61A5A0; si hay memoria: ctor 620B00([this+10h], [+14h], [+18h], [+1Ch], [+20h]); EAX = el del ctor; si no 0
   4  0x623370  KillPedOnFoot::Clone: new(38h); ctor 620E30([+10h], [+28h], [+14h], [+18h], [+1Ch], movsx byte [+20h]); con el EAX del ctor (= la memoria): byte +0Ch = (viejo & CFh) | ([this+0Ch] & 30h)
   5  0x4AEA00  mov eax,0Eh;   6 0x4AEA10 xor eax,eax;   8 0x4AEA30 fld [858628h] = 42C80000h (100.0f);   9 0x4AEA40 mov eax,0Eh
   7  0x4AEA20  AffectsPed(ped): CPed::IsAlive 5E0170 = [ped+530h] distinto de 36h y 37h
  10  0x4AEC00  IsAlive(ped); [ped+58Ch] != 0; bit 8 de [ped+46Ch]; [ped+58Ch] == [this+18h]; [this+14h] != 0
  11  0x4B7170  CloneEditable: mov ecx,[B744ACh]; call 4B5570 (New); ctor 4AEAC0([+14h], [+18h], [+1Ch]); EAX = el del ctor; sin memoria 0
  12  0x4AEDC0  IsAlive(ped) y [this+10h] != 0
  13  0x4B7210  Clone: New 4B5570; ctor 4AECD0(byte [+0Ch] ampliado a cero, [+10h])
  14  0x4AEF00  SexyPed::AffectsPed: IsAlive(ped); [this+14h] != 0; IsAlive(atractivo); IsLooking 6181A0 (ECX = C15448h, ped) y, si AL, GetLookAtEntity 6181D0 == atractivo => 0; FindActiveTaskByType 681740
            (ECX = [[ped+47Ch]]+4: tipos 4B2h, 4BFh, 4B3h parando en el primero) en el gestor del ped (sin tarea => 1) y despues en el del atractivo (sin tarea => 1); GetTaskType (ranura +10h) de la tarea del
            ATRACTIVO primero y despues la del ped; iguales => 0, si no 1
  15  0x4B7280  CloneEditable: New 4B5570; ctor 4AEDF0([+14h]);  16 0x4AF100 IsAlive(ped) y [this+0Ch] != 0;  17 0x4B72F0 Clone: New; ctor 4AF010([+0Ch]);  18 0x4B7360 ctor 4AF130([+0Ch]);  19 0x4B73D0 ctor 4AF240([+0Ch])
  20  0x5E0540  CreateAtRef(ref) (ECX = pool: +4 estados, +0Ch cursor): estado[ref >> 8] = ref & 7Fh (bit 7 a cero); cursor = 0 y avanza mientras el bit 7 del estado[cursor] este a cero
  21  0x5E47A0  CPed::operator delete(ptr, ref) (cdecl): pool = [B74490h]; indice = (ptr - [pool]) / 7C4h (con signo, hacia cero); estado[indice] |= 80h; si indice < [pool+0Ch] (con signo) => [pool+0Ch] = indice
  22  0x5E88E0  CanWeRunAndFireWithWeapon: ranura = movsx byte [this+718h]; tipo = [this + ranura*1Ch + 5A0h] (una vez); habilidad = 5E3B60(this, tipo); info = 743C60(tipo, habilidad); AL = bit 1 de [info+18h]
  23  0x5E8C80  destructor con borrado: 5E8620(this); si bit 0 del byte de banderas: la liberacion de 5E47A0 EN LINEA (pool [B74490h]); EAX = this
  24  0x50A870  CCamera cuerpo del destructor: [this] = 8630E8h; [this+14h] = 0; seis 59ACD0 (this+0ADCh, 0A94h, 0A4Ch, 0A04h, 09BCh, 0974h); ~CPlaceable 54F490: [this] = 863C40h, con [this+14h] == 0 sin RemoveMatrix,
            [this+14h] = B74240h
  25  0x514010  destructor con borrado de CCamera: 50A870(this); si bit 0: operator delete 8214BD(this); EAX = this
  26  0x513220  TwoPlayerCameraVisible(x, y, z) stdcall: [8CCB80h] = 5; ProcessLineOfSight 56BA00(origen = fuente, destino = posicion del jugador 0 ([B7CD98h]; matriz +14h ? matriz+30h : ped+4), colpoint, entidad,
            1, 0, 0, 0, 0, 1, 1, 0); si AL != 0: 0; si no, igual con el jugador 1 ([B7CF28h]) y devuelve (AL == 0)
  27  0x5132D0  TwoPlayerCameraGeometry(beta, fuente*, frente*, objetivo*), ECX = camara (+0ACh = alfa): ver geometry(); x87 con la palabra de control a 24 bits
  28  0x517740  CHandShaker::Constructor: [this+7Ch] = 0; [this+80h] = 0; 517330(this); EAX = this
  29  0x517760  CIdleCam::Constructor: 50E6D0(this); EAX = this

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), (dword de [ECX] al entrar | argumentos crudos)]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano];
CEventPool::New [4B5570, ECX == [B744AC]]; vtables sinteticas: GetTaskType [94, ECX], MakeAbortable [96, ECX, ped, prioridad, evento], CreateFirstSubTask [99, ECX, ped], GetTaskType de las tareas del ped y del atractivo
[95, ECX]; ranuras no esperadas [90 + k, ECX] (no deben aparecer); GetWeaponSkill [5E3B60, ECX, tipo]; GetWeaponInfo [743C60, tipo, habilidad]; ProcessLineOfSight [56BA00, origen x y z, destino x y z, ocho banderas];
IsLooking / GetLookAtEntity [6181A0 / 6181D0, ECX, ped]; FindActiveTaskByType [681740, ECX, tipo]; Normalise [59C910, x, y, z].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from functools import lru_cache
from math import isqrt

M32 = 0xFFFFFFFF
NAMES = [
    "CTaskComplexReactToAttack::MakeAbortable", "CTaskComplexReactToAttack::CreateSubTask", "CTaskComplexReactToAttack::ControlSubTask", "CTaskComplexReactToAttack::Clone", "CTaskComplexKillPedOnFoot::Clone",
    "CEventVehicleHitAndRun::GetEventType", "CEventVehicleHitAndRun::GetLifeTime", "CEventVehicleHitAndRun::AffectsPed", "CEventVehicleHitAndRun::GetLocalSoundLevel", "CEventVehicleHitAndRun::GetEventPriority",
    "CEventPedEnteredMyVehicle::AffectsPed", "CEventPedEnteredMyVehicle::CloneEditable", "CEventChatPartner::AffectsPed", "CEventChatPartner::Clone", "CEventSexyPed::AffectsPed", "CEventSexyPed::CloneEditable",
    "CEventSexyVehicle::AffectsPed", "CEventSexyVehicle::Clone", "CEventPedToChase::Clone", "CEventPedToFlee::Clone", "CPedPool::CreateAtRef", "CPed::operator delete-poolRef", "CPed::CanWeRunAndFireWithWeapon",
    "CPed::ScalarDeletingDestructor", "CCamera::DestructorBody", "CCamera::ScalarDeletingDestructor", "CCam::TwoPlayerCameraVisible", "CCam::TwoPlayerCameraGeometry", "CHandShaker::Constructor", "CIdleCam::Constructor"]
ENTRIES = (0x620BF0, 0x620C30, 0x620E00, 0x623300, 0x623370, 0x4AEA00, 0x4AEA10, 0x4AEA20, 0x4AEA30, 0x4AEA40, 0x4AEC00, 0x4B7170, 0x4AEDC0, 0x4B7210, 0x4AEF00,
           0x4B7280, 0x4AF100, 0x4B72F0, 0x4B7360, 0x4B73D0, 0x5E0540, 0x5E47A0, 0x5E88E0, 0x5E8C80, 0x50A870, 0x514010, 0x513220, 0x5132D0, 0x517740, 0x517760)
GUARD = 0x40
WIN = 0x1000
WIN_A = 0x3000
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

(RA_ABORT, RA_CREATE, RA_CONTROL, RA_CLONE, KP_CLONE, VHR_TYPE, VHR_LIFE, VHR_AFFECTS, VHR_SOUND, VHR_PRIO, PEMV_AFFECTS, PEMV_CLONE, CP_AFFECTS, CP_CLONE, SP_AFFECTS, SP_CLONE,
 SV_AFFECTS, SV_CLONE, PTC_CLONE, PTF_CLONE, POOL_CREATE, PED_DELETE, PED_CANRUN, PED_SDD, CAM_DTOR, CAM_SDD, CAM_VIS, CAM_GEO, HS_CTOR, IC_CTOR) = range(30)
CONSTS = {VHR_TYPE: 14, VHR_LIFE: 0, VHR_PRIO: 14}
FLOAT_100 = 0x42C80000
TASK_NEW, EVENT_POOL_NEW, EVENT_DELETE_NONE = 0x61A5A0, 0x4B5570, 0
CLONE_EVENTS = {   # fn: (tamano de pedido, direccion del constructor, [(desplazamiento del campo, mascara)] en orden de argumento)
    PEMV_CLONE: (0x4AEAC0, [(0x14, M32), (0x18, M32), (0x1C, M32)]), CP_CLONE: (0x4AECD0, [(0x0C, 0xFF), (0x10, M32)]), SP_CLONE: (0x4AEDF0, [(0x14, M32)]),
    SV_CLONE: (0x4AF010, [(0x0C, M32)]), PTC_CLONE: (0x4AF130, [(0x0C, M32)]), PTF_CLONE: (0x4AF240, [(0x0C, M32)])}
MATRIX_OFFSETS = (0xADC, 0xA94, 0xA4C, 0xA04, 0x9BC, 0x974)
POOL_PED_STRIDE = 0x7C4
ONE_BITS, HALF_BITS, QUARTER_BITS, C067_BITS, SEVEN_BITS, TENTH_BITS = 0x3F800000, 0x3F000000, 0x3E800000, 0x3F2B851F, 0x40E00000, 0x3DCCCCCD


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


def slot_byte(seed, i):
    s = rnd(seed, i)
    return (s & 0x7F) | (0x80 if ((s >> 8) % 100) < 30 else 0)


def signed32(v):
    v &= M32
    return v - (1 << 32) if v >= 0x80000000 else v


def sext8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


# ------------------------------------------------------------------------------------------------ aritmetica de la x87 (24 bits de mantisa, exponente ampliado, redondeo al mas cercano-par)
def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def to_fraction(b):
    """valor exacto de un float finito (con signo)"""
    s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    m = f if e == 0 else (f | 0x800000)
    ex = -149 if e == 0 else e - 150
    v = Fraction(m) * (Fraction(2) ** ex)
    return -v if s else v


def _exp2floor(a):
    """e tal que 2^e <= a < 2^(e+1) (a > 0)"""
    e = a.numerator.bit_length() - a.denominator.bit_length()
    p = Fraction(2) ** e
    while p > a:
        e -= 1
        p = Fraction(2) ** e
    while p * 2 <= a:
        e += 1
        p = Fraction(2) ** e
    return e


def _rne(x):
    """entero mas cercano a x >= 0 (empate al par)"""
    fl = x.numerator // x.denominator
    rem = x - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    return fl


def round_bits(q, nbits):
    """redondea q a nbits de mantisa (al mas cercano, empate al par), sin limite de exponente"""
    if q == 0:
        return Fraction(0)
    neg = q < 0
    a = -q if neg else q
    e = _exp2floor(a) - (nbits - 1)
    m = _rne(a / (Fraction(2) ** e))
    r = Fraction(m) * (Fraction(2) ** e)
    return -r if neg else r


def f32_bits_of(q, neg_zero=False):
    """bits del float de 32 bits mas cercano a q (con denormales); sin desbordar (la autoprueba lo comprueba)"""
    if q == 0:
        return 0x80000000 if neg_zero else 0
    neg = q < 0
    a = -q if neg else q
    e = _exp2floor(a)
    if e < -126:
        m = _rne(a / (Fraction(2) ** -149))
        bits = m
    else:
        m = _rne(a / (Fraction(2) ** (e - 23)))
        if m == (1 << 24):
            e += 1
            m = 1 << 23
        if e + 127 >= 255:
            raise OverflowError('float de 32 bits desbordado')
        bits = ((e + 127) << 23) | (m & 0x7FFFFF)
    return bits | (0x80000000 if neg else 0)


class Z:
    """valor de la x87 (finito): q exacto y, solo si es cero, su signo"""
    __slots__ = ('q', 'neg')

    def __init__(self, q=0, neg=False):
        self.q = Fraction(q)
        self.neg = bool(neg) if self.q == 0 else False

    def negative(self):
        return self.q < 0 or (self.q == 0 and self.neg)


def z_of_bits(b):
    assert not is_nan(b) and not is_inf(b), 'el oraculo de la geometria solo modela floats finitos'
    q = to_fraction(b)
    return Z(q, q == 0 and bool(b >> 31))


def z_bits(z):
    return f32_bits_of(z.q, z.neg)


def z_neg(a):
    return Z(-a.q, (not a.neg) if a.q == 0 else False)


def z_mul(a, b):
    return Z(round_bits(a.q * b.q, 24), a.negative() != b.negative())


def z_add(a, b):
    q = a.q + b.q
    if q == 0:
        return Z(0, a.q == 0 and b.q == 0 and a.neg and b.neg)
    return Z(round_bits(q, 24), False)


def z_sub(a, b):
    return z_add(a, z_neg(b))


def z_sqrt(a):
    """fsqrt a 24 bits (la raiz de un cero conserva el signo)"""
    if a.q == 0:
        return Z(0, a.neg)
    assert a.q > 0
    k = 200
    n = (a.q * (1 << (2 * k)))
    nf = n.numerator // n.denominator
    r = isqrt(nf)
    exact = (r * r == nf) and n.denominator == 1
    v = Fraction(2 * r + (0 if exact else 1), 2 * (1 << k))     # si no es exacta, un cuarto de unidad mas: sin empates falsos
    return Z(round_bits(v, 24), False)


def f32_half(b):
    """float de 32 bits (como bits) multiplicado por 0.5f (mulss: exacto salvo denormales, que se redondean al par)"""
    z = z_of_bits(b)
    return f32_bits_of(z.q / 2, z.neg)


# ------------------------------------------------------------------------------------------------ seno y coseno de la x87 (precision ampliada de 64 bits; reduccion de argumento exacta)
_SHIFT = 420


def _atan_inv(x):
    one = 1 << _SHIFT
    term = one // x
    total = term
    x2 = x * x
    n = 1
    sign = -1
    while term:
        term //= x2
        n += 2
        total += sign * (term // n)
        sign = -sign
    return total


_PI = 16 * _atan_inv(5) - 4 * _atan_inv(239)     # pi * 2^_SHIFT


def _sincos_reduced(r):
    """seno y coseno de r (entero en punto fijo de _SHIFT bits, |r| <= pi/4 + epsilon) por Taylor"""
    one = 1 << _SHIFT
    r2 = (r * r) >> _SHIFT
    s = c = 0
    # coseno
    term = one
    n = 0
    while term:
        c += term
        n += 2
        term = -((term * r2) >> _SHIFT) // (n * (n - 1))
    term = r
    n = 1
    while term:
        s += term
        n += 2
        term = -((term * r2) >> _SHIFT) // (n * (n - 1))
    return s, c


@lru_cache(maxsize=None)
def trig_ext(bits):
    """(cos, sin) del float `bits` como Z redondeados a 64 bits de mantisa (el seno de +-0 es +-0)"""
    z = z_of_bits(bits)
    if z.q == 0:
        return Z(1), Z(0, z.neg)
    x = z.q
    half_pi = Fraction(_PI, 2 << _SHIFT)
    k = _rne(abs(x) / half_pi) * (1 if x > 0 else -1)
    r = x - k * half_pi
    rfix = (r.numerator << _SHIFT) // r.denominator
    s, c = _sincos_reduced(rfix)
    quad = k % 4
    if quad == 0:
        sv, cv = s, c
    elif quad == 1:
        sv, cv = c, -s
    elif quad == 2:
        sv, cv = -s, -c
    else:
        sv, cv = -c, s
    one = Fraction(1, 1 << _SHIFT)
    return Z(round_bits(Fraction(cv) * one, 64)), Z(round_bits(Fraction(sv) * one, 64))


def ftol_low(bits):
    """_ftol del EXE (64 bits, trunca hacia cero) y parte baja: 0 con NaN, infinitos o |x| >= 2^63"""
    if is_nan(bits) or is_inf(bits):
        return 0
    q = to_fraction(bits)
    n = int(q)                 # trunca hacia cero
    if not (-(1 << 63) <= n < (1 << 63)):
        return 0
    return n & M32


def geometry(a, w, events):
    """TwoPlayerCameraGeometry: devuelve {(arena, desplazamiento): [bits x, y, z]} de las tres salidas"""
    alpha_bits, beta_bits = a[4], a[5]
    ca, sa = trig_ext(alpha_bits)[0], trig_ext(alpha_bits)[1]
    cb, sb = trig_ext(beta_bits)
    fx = z_bits(z_neg(z_mul(cb, ca)))
    fy = z_bits(z_neg(z_mul(sb, ca)))
    fz = z_bits(sa)
    flat = [fx, fy, 0]
    events.append([0x59C910] + flat)
    flat_h = [f32_half(b) for b in flat]
    p0 = [z_of_bits(a[15 + i]) for i in range(3)]
    p1 = [z_of_bits(a[18 + i]) for i in range(3)]
    dx, dy, dz = z_sub(p0[0], p1[0]), z_sub(p0[1], p1[1]), z_sub(p0[2], p1[2])
    t1 = z_add(z_mul(dz, dz), z_mul(dy, dy))
    t2 = z_add(t1, z_mul(dx, dx))
    dist = z_add(z_mul(z_sqrt(t2), z_of_bits(C067_BITS)), z_of_bits(SEVEN_BITS))
    dbits = z_bits(dist)
    sepx, sepy = z_bits(z_sub(p0[0], p1[0])), z_bits(z_sub(p0[1], p1[1]))
    events.append([0x59C910, sepx, sepy, 0])
    sep_h = [f32_half(sepx), f32_half(sepy), f32_half(0)]
    t_y = z_mul(z_of_bits(sep_h[1]), z_of_bits(flat_h[1]))
    t_x = z_mul(z_of_bits(sep_h[0]), z_of_bits(flat_h[0]))
    total = z_add(z_add(t_y, t_x), z_mul(z_of_bits(flat_h[2]), z_of_bits(sep_h[2])))
    weight = z_bits(z_sub(z_of_bits(HALF_BITS), z_mul(total, z_of_bits(QUARTER_BITS))))
    wz = z_of_bits(weight)
    comp = z_sub(z_of_bits(ONE_BITS), wz)
    cp1x = z_of_bits(z_bits(z_mul(comp, p1[0])))
    cp1y = z_of_bits(z_bits(z_mul(comp, p1[1])))
    cp1z = z_mul(comp, p1[2])
    wp0 = [z_mul(wz, p0[i]) for i in range(3)]
    wp0z32 = z_of_bits(z_bits(wp0[2]))
    tx = z_bits(z_add(wp0[0], cp1x))
    ty = z_bits(z_add(wp0[1], cp1y))
    tz = z_bits(z_add(wp0z32, cp1z))
    dz_ = z_of_bits
    dd = z_of_bits(dbits)
    dfx, dfy = z_mul(dd, dz_(fx)), z_mul(dd, dz_(fy))
    dfz32 = dz_(z_bits(z_mul(dd, dz_(fz))))
    sx = z_bits(z_sub(dz_(tx), dfx))
    sy = z_bits(z_sub(dz_(ty), dfy))
    sz_pre = z_bits(z_sub(dz_(tz), dfz32))
    sz = z_bits(z_add(z_mul(dd, z_of_bits(TENTH_BITS)), dz_(sz_pre)))
    return {'source': [sx, sy, sz], 'front': [fx, fy, fz], 'target': [tx, ty, tz]}


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, win):
        self.kind, self.seed, self.win, self.m, self.r = kind, seed, win, {}, {}
        self.words = {}
        self.n = GUARD + win + GUARD

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
        wins = (WIN_A, WIN, WIN, WIN)
        self.mem = tuple(Mem(a[2], a[44 + i], wins[i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def rd(self, addr, n):
        i = self.locate(addr)
        return self.mem[i[0]].rd(i[1], n)

    def wr(self, addr, n, v):
        i = self.locate(addr)
        self.mem[i[0]].wr(i[1], n, v)

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + self.mem[i].n:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + self.mem[i].n:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
EXTRA = {
    RA_ABORT: ['flag_done', 'prio_bad', 'prio_urgent', 'prio_immediate', 'abort_true', 'abort_false'],
    RA_CREATE: ['type_behit', 'type_none', 'type_fall', 'type_other', 'mem_null', 'mem_set', 'attacker_ped', 'attacker_other', 'attacker_null', 'fall_anim_1a', 'fall_anim_14', 'ftol_big', 'ftol_neg', 'ftol_nan'],
    RA_CONTROL: ['first_sub_task', 'get_type', 'rewrite', 'no_rewrite'],
    RA_CLONE: ['mem_null', 'mem_set'],
    KP_CLONE: ['bits_set', 'bits_clear'],
    VHR_AFFECTS: ['alive', 'dead36', 'dead37'],
    PEMV_AFFECTS: ['ped_dead', 'no_vehicle', 'on_foot', 'other_vehicle', 'no_entered', 'true'],
    PEMV_CLONE: ['mem_null', 'mem_set'],
    CP_AFFECTS: ['ped_dead', 'no_partner', 'true'],
    CP_CLONE: ['mem_null', 'mem_set', 'flag_high'],
    SP_AFFECTS: ['ped_dead', 'no_sexy', 'sexy_dead', 'looking', 'looking_at_sexy', 'no_ped_task', 'no_sexy_task', 'same_type', 'diff_type', 'ped_task_4b3', 'sexy_task_4b3'],
    SP_CLONE: ['mem_null', 'mem_set'],
    SV_AFFECTS: ['ped_dead', 'no_vehicle', 'true'],
    SV_CLONE: ['mem_null', 'mem_set'], PTC_CLONE: ['mem_null', 'mem_set'], PTF_CLONE: ['mem_null', 'mem_set'],
    POOL_CREATE: ['idx_first', 'idx_last', 'cursor_zero', 'cursor_far'],
    PED_DELETE: ['lowers_cursor', 'keeps_cursor', 'cursor_negative', 'rem_nonzero'],
    PED_CANRUN: ['slot_negative', 'slot_positive', 'aim_true', 'aim_false'],
    PED_SDD: ['flag_bit0_set', 'flag_bit0_clear', 'lowers_cursor', 'keeps_cursor'],
    CAM_DTOR: ['link_zero', 'link_set'],
    CAM_SDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    CAM_VIS: ['hit_first', 'second_visible', 'second_hit', 'matrix0', 'no_matrix0', 'matrix1', 'no_matrix1', 'byte_was_5', 'byte_not_5'],
    CAM_GEO: ['matrix0', 'no_matrix0', 'matrix1', 'no_matrix1', 'same_position', 'alpha_small', 'beta_large', 'pos_huge'],
}
KEYS = {fn: list(COMMON) + EXTRA.get(fn, []) for fn in range(len(NAMES))}


def pool_states(seed, cap):
    return [slot_byte(seed, i) for i in range(cap)]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, gch = [], None, []
    o = a[0]
    this = W.raw(0, o)
    tv = W.tagv
    if fn in (RA_ABORT, RA_CREATE, RA_CLONE, KP_CLONE, VHR_TYPE, VHR_LIFE, VHR_SOUND, VHR_PRIO, VHR_AFFECTS, PEMV_AFFECTS, PEMV_CLONE, CP_AFFECTS, CP_CLONE, SP_AFFECTS, SP_CLONE, SV_AFFECTS, SV_CLONE, PTC_CLONE, PTF_CLONE,
              PED_SDD, CAM_DTOR, CAM_SDD, HS_CTOR, IC_CTOR):
        A.wr(o, 4, a[3])

    if fn == RA_ABORT:
        sub = W.raw(1, a[4])
        A.wr(o + 8, 4, sub)
        A.wr(o + 0xD, 1, a[5])
        W.snap()
        if a[5] & 0xFF:
            marks.append('flag_done')
            ret = 1
        elif a[6] not in (1, 2):
            marks.append('prio_bad')
            ret = 0
        else:
            marks.append('prio_urgent' if a[6] == 1 else 'prio_immediate')
            events.append([96, tv(sub), a[7], a[6], a[8]])
            al = a[9] & 0xFF
            A.wr(o + 0xD, 1, al)
            marks.append('abort_true' if al else 'abort_false')
            ret = al
    elif fn == RA_CREATE:
        attacker = 0 if a[5] == 0 else W.raw(1, a[6])
        A.wr(o + 0x14, 4, attacker)
        if a[5]:
            B.wr(a[6] + 0x36, 1, a[7])
        A.wr(o + 0x18, 4, a[8]); A.wr(o + 0x1C, 4, a[9]); A.wr(o + 0x20, 4, a[10])
        W.snap()
        t = a[4]
        mem = None if a[11] == 0 else W.raw(3, a[12])
        marks.append('mem_null' if mem is None else 'mem_set')
        ts = signed32(t)
        if ts > 0x3F0:
            marks.append('type_other')
            ret = 0
        elif t == 0x3F0:
            marks.append('type_behit')
            events.append([TASK_NEW, 0x28])
            if mem is None:
                ret = 0
            else:
                is_ped = attacker != 0 and (a[7] & 7) == 3
                marks.append('attacker_ped' if is_ped else ('attacker_null' if attacker == 0 else 'attacker_other'))
                v = ftol_low(a[8])
                marks.append('ftol_nan' if is_nan(a[8]) else ('ftol_big' if (a[8] & 0x7FFFFFFF) >= 0x4F000000 else ('ftol_neg' if (a[8] >> 31) and v else 'ftol_plain')))
                events.append([0x620780, tv(mem), attacker if is_ped else 0, a[10], a[9], v])
                W.wr(mem, 4, a[14])
                ret = tv(a[13])
        elif t == 0xC8:
            marks.append('type_none')
            events.append([TASK_NEW, 8])
            if mem is None:
                ret = 0
            else:
                events.append([0x61A390, tv(mem), W.rd(mem, 4)])
                W.wr(mem, 4, a[14])
                W.wr(mem, 4, 0x859CFC)
                ret = tv(mem)
        elif t == 0xD0:
            marks.append('type_fall')
            events.append([TASK_NEW, 0x18])
            if mem is None:
                ret = 0
            else:
                anim = 0x1A if a[9] == 2 else 0x14
                marks.append('fall_anim_1a' if a[9] == 2 else 'fall_anim_14')
                events.append([0x6786C0, tv(mem), anim, 0, 1000])
                W.wr(mem, 4, a[14])
                ret = tv(a[13])
        else:
            marks.append('type_other')
            ret = 0
    elif fn == RA_CONTROL:
        sub = W.raw(1, a[5])
        A.wr(o, 4, 0)       # la vtable de `this` es una direccion real que el oraculo no conoce: nunca se lee ni cambia
        A.wr(o + 8, 4, sub)
        A.wr(o + 0xC, 1, a[4])
        W.snap()
        if a[4] & 0xFF:
            marks.append('first_sub_task')
            events.append([99, tv(this), a[6]])
            ret = tv(a[7])
        else:
            marks.append('get_type')
            events.append([94, tv(sub)])
            if a[8]:
                A.wr(o + 8, 4, a[9])
                marks.append('rewrite')
                ret = tv(a[9])
            else:
                marks.append('no_rewrite')
                ret = tv(sub)
    elif fn == RA_CLONE:
        for i in range(5):
            A.wr(o + 0x10 + 4 * i, 4, a[8 + i])
        W.snap()
        events.append([TASK_NEW, 0x24])
        if a[4] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([0x620B00, tv(mem), a[8], a[9], a[10], a[11], a[12]])
            W.wr(mem, 4, a[7])
            ret = tv(a[6])
    elif fn == KP_CLONE:
        for off, idx in ((0x10, 6), (0x14, 7), (0x18, 8), (0x1C, 9), (0x20, 10), (0x28, 11)):
            A.wr(o + off, 4, a[idx])
        A.wr(o + 0xC, 1, a[12])
        W.snap()
        mem = W.raw(1, a[4])
        events.append([TASK_NEW, 0x38])
        events.append([0x620E30, tv(mem), a[6], a[11], a[7], a[8], a[9], sext8(a[10]) & M32])
        W.wr(mem, 4, a[5])
        old = B.rd(a[4] + 0xC, 1)
        new = (old & 0xCF) | (a[12] & 0x30)
        B.wr(a[4] + 0xC, 1, new)
        marks.append('bits_set' if a[12] & 0x30 else 'bits_clear')
        ret = tv(mem)
    elif fn in (VHR_TYPE, VHR_LIFE, VHR_PRIO):
        W.snap()
        ret = CONSTS[fn]
    elif fn == VHR_SOUND:
        W.snap()
        ret = FLOAT_100
    elif fn in (VHR_AFFECTS, PEMV_AFFECTS, CP_AFFECTS, SV_AFFECTS):
        if fn == VHR_AFFECTS:
            state = a[5]; ped = a[4]
            B.wr(ped + 0x530, 4, state)
            W.snap()
            alive = state not in (0x36, 0x37)
            marks.append('alive' if alive else ('dead36' if state == 0x36 else 'dead37'))
            ret = 1 if alive else 0
        elif fn == PEMV_AFFECTS:
            A.wr(o + 0x14, 4, a[4]); A.wr(o + 0x18, 4, a[5])
            B.wr(a[6] + 0x530, 4, a[7]); B.wr(a[6] + 0x58C, 4, a[8]); B.wr(a[6] + 0x46C, 4, a[9])
            W.snap()
            if a[7] in (0x36, 0x37):
                marks.append('ped_dead'); ret = 0
            elif a[8] == 0:
                marks.append('no_vehicle'); ret = 0
            elif not (a[9] >> 8) & 1:
                marks.append('on_foot'); ret = 0
            elif a[8] != a[5]:
                marks.append('other_vehicle'); ret = 0
            elif a[4] == 0:
                marks.append('no_entered'); ret = 0
            else:
                marks.append('true'); ret = 1
        elif fn == CP_AFFECTS:
            A.wr(o + 0x10, 4, a[4])
            B.wr(a[5] + 0x530, 4, a[6])
            W.snap()
            if a[6] in (0x36, 0x37):
                marks.append('ped_dead'); ret = 0
            elif a[4] == 0:
                marks.append('no_partner'); ret = 0
            else:
                marks.append('true'); ret = 1
        else:
            A.wr(o + 0xC, 4, a[4])
            B.wr(a[5] + 0x530, 4, a[6])
            W.snap()
            if a[6] in (0x36, 0x37):
                marks.append('ped_dead'); ret = 0
            elif a[4] == 0:
                marks.append('no_vehicle'); ret = 0
            else:
                marks.append('true'); ret = 1
    elif fn in CLONE_EVENTS:
        ctor, fields = CLONE_EVENTS[fn]
        if fn == PEMV_CLONE:
            for off, idx in ((0x14, 8), (0x18, 9), (0x1C, 10)):
                A.wr(o + off, 4, a[idx])
        elif fn == CP_CLONE:
            A.wr(o + 0xC, 1, a[8]); A.wr(o + 0x10, 4, a[9])
        elif fn == SP_CLONE:
            A.wr(o + 0x14, 4, a[8])
        else:
            A.wr(o + 0xC, 4, a[8])
        W.snap()
        events.append([EVENT_POOL_NEW, 1])
        if a[4] == 0:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            args = []
            for off, mask in fields:
                args.append(A.rd(o + off, 4 if mask == M32 else 1) & mask)
            if fn == CP_CLONE and (args[0] & 0xF8):
                marks.append('flag_high')
            events.append([ctor, tv(mem)] + args)
            W.wr(mem, 4, a[7])
            ret = tv(a[6])
    elif fn == SP_AFFECTS:
        sexy = 0 if a[4] == 0 else W.raw(3, a[5])
        A.wr(o + 0x14, 4, sexy)
        ped = W.raw(1, a[6])
        B.wr(a[6] + 0x530, 4, a[7]); B.wr(a[6] + 0x47C, 4, W.raw(2, a[8]))
        D.wr(a[5] + 0x530, 4, a[9]); D.wr(a[5] + 0x47C, 4, W.raw(2, a[10]))
        W.snap()
        mgr0, mgr1 = (W.raw(2, a[8]) + 4) & M32, (W.raw(2, a[10]) + 4) & M32
        look_ent = sexy if a[15] else a[14]
        ret = 0
        if a[7] in (0x36, 0x37):
            marks.append('ped_dead')
        elif sexy == 0:
            marks.append('no_sexy')
        elif a[9] in (0x36, 0x37):
            marks.append('sexy_dead')
        else:
            events.append([0x6181A0, 0xC15448, tv(ped)])
            blocked = False
            if a[13] & 0xFF:
                marks.append('looking')
                events.append([0x6181D0, 0xC15448, tv(ped)])
                if look_ent == sexy:
                    marks.append('looking_at_sexy')
                    blocked = True
            if not blocked:
                found_p = None
                for i, t in enumerate((0x4B2, 0x4BF, 0x4B3)):
                    events.append([0x681740, tv(mgr0), t])
                    if a[16 + i]:
                        found_p = i
                        break
                if found_p is None:
                    marks.append('no_ped_task'); ret = 1
                else:
                    if found_p == 2:
                        marks.append('ped_task_4b3')
                    found_s = None
                    for i, t in enumerate((0x4B2, 0x4BF, 0x4B3)):
                        events.append([0x681740, tv(mgr1), t])
                        if a[19 + i]:
                            found_s = i
                            break
                    if found_s is None:
                        marks.append('no_sexy_task'); ret = 1
                    else:
                        if found_s == 2:
                            marks.append('sexy_task_4b3')
                        events.append([95, tv(W.raw(2, a[12]))])
                        events.append([95, tv(W.raw(2, a[11]))])
                        same = a[23] == a[22]
                        marks.append('same_type' if same else 'diff_type')
                        ret = 0 if same else 1
    elif fn == POOL_CREATE:
        cap, idx = a[5], a[7]
        A.wr(o, 4, a[9]); A.wr(o + 4, 4, W.raw(1, a[4])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[6])
        states = pool_states(a[10], cap)
        states[idx] |= 0x80
        anchor = rnd(a[10], 0x3E7) % cap
        if anchor == idx:
            anchor = (idx + 1) % cap
        states[anchor] |= 0x80
        for i, s in enumerate(states):
            B.wr(a[4] + i, 1, s)
        W.snap()
        states[idx] = a[8] & 0x7F
        cur = 0
        while not states[cur] & 0x80:
            cur += 1
        B.wr(a[4] + idx, 1, states[idx])
        A.wr(o + 0xC, 4, cur)
        marks.append('idx_first' if idx == 0 else ('idx_last' if idx == cap - 1 else 'idx_mid'))
        marks.append('cursor_zero' if cur == 0 else 'cursor_far')
        ret = None
    elif fn == PED_DELETE:
        cap, idx = a[5], a[7]
        A.wr(o, 4, a[9]); A.wr(o + 4, 4, W.raw(1, a[4])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[6])
        states = pool_states(a[10], cap)
        states[idx] &= 0x7F
        for i, s in enumerate(states):
            B.wr(a[4] + i, 1, s)
        W.snap()
        B.wr(a[4] + idx, 1, states[idx] | 0x80)
        cur = signed32(a[6])
        if idx < cur:
            A.wr(o + 0xC, 4, idx)
            marks.append('lowers_cursor')
        else:
            marks.append('keeps_cursor')
        if cur < 0:
            marks.append('cursor_negative')
        if a[8]:
            marks.append('rem_nonzero')
        ret = None
    elif fn == PED_CANRUN:
        slot = sext8(a[4])
        A.wr(o + 0x718, 1, a[4])
        A.wr(o + 0x5A0 + slot * 0x1C, 4, a[5])
        info = W.raw(2, a[7])
        Cc.wr(a[7] + 0x18, 4, a[8])
        W.snap()
        events.append([0x5E3B60, tv(this), a[5]])
        events.append([0x743C60, a[5], a[6]])
        bit = (a[8] >> 1) & 1
        marks.append('slot_negative' if slot < 0 else 'slot_positive')
        marks.append('aim_true' if bit else 'aim_false')
        ret = bit
    elif fn == PED_SDD:
        idx, rem, cap = a[9], a[10], a[7]
        thisv = this
        Cc.wr(a[5], 4, (thisv - idx * POOL_PED_STRIDE - rem) & M32); Cc.wr(a[5] + 4, 4, W.raw(3, a[6])); Cc.wr(a[5] + 8, 4, cap); Cc.wr(a[5] + 0xC, 4, a[8])
        states = pool_states(a[12], cap)
        states[idx] &= 0x7F
        for i, s in enumerate(states):
            D.wr(a[6] + i, 1, s)
        W.snap()
        events.append([0x5E8620, tv(this), A.rd(o, 4)])
        A.wr(o, 4, a[11])
        flags = a[4] & 0xFF
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        if flags & 1:
            D.wr(a[6] + idx, 1, states[idx] | 0x80)
            cur = signed32(a[8])
            if idx < cur:
                Cc.wr(a[5] + 0xC, 4, idx)
                marks.append('lowers_cursor')
            else:
                marks.append('keeps_cursor')
        ret = tv(this)
    elif fn == CAM_DTOR:
        A.wr(o + 0x14, 4, a[4])
        W.snap()
        marks.append('link_zero' if a[4] == 0 else 'link_set')
        for off in MATRIX_OFFSETS:
            events.append([0x59ACD0, tv((this + off) & M32), A.rd(o + off, 4)])
            A.wr(o + off, 4, a[5])
        A.wr(o, 4, 0x863C40)
        A.wr(o + 0x14, 4, 0xB74240)
        ret = None
    elif fn == CAM_SDD:
        A.wr(o + 0x14, 4, a[6])
        W.snap()
        events.append([0x50A870, tv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[4] & 0xFF
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[4] >> 8 else 'flag_byte_only')
        if flags & 1:
            events.append([0x8214BD, tv(this), A.rd(o, 4)])
        ret = tv(this)
    elif fn == CAM_VIS:
        for i in range(3):   # la posicion que lee la funcion (la de la matriz si el ped la tiene, si no la del ped) lleva el valor del caso; la otra, basura
            B.wr(a[4] + 4 + 4 * i, 4, (a[10 + i] ^ 0x5A5A5A5A) if a[8] else a[10 + i]); B.wr(a[6] + 0x30 + 4 * i, 4, a[10 + i] if a[8] else (a[10 + i] ^ 0x5A5A5A5A))
            B.wr(a[5] + 4 + 4 * i, 4, (a[13 + i] ^ 0x5A5A5A5A) if a[9] else a[13 + i]); B.wr(a[7] + 0x30 + 4 * i, 4, a[13 + i] if a[9] else (a[13 + i] ^ 0x5A5A5A5A))
        B.wr(a[4] + 0x14, 4, W.raw(1, a[6]) if a[8] else 0)
        B.wr(a[5] + 0x14, 4, W.raw(1, a[7]) if a[9] else 0)
        W.snap()
        marks.append('matrix0' if a[8] else 'no_matrix0')
        marks.append('matrix1' if a[9] else 'no_matrix1')
        pos0 = [a[10 + i] for i in range(3)]
        pos1 = [a[13 + i] for i in range(3)]
        flags = [1, 0, 0, 0, 0, 1, 1, 0]
        events.append([0x56BA00, a[16], a[17], a[18]] + pos0 + flags)
        if a[19] & 0xFF:
            marks.append('hit_first')
            ret = 0
        else:
            events.append([0x56BA00, a[16], a[17], a[18]] + pos1 + flags)
            if a[20] & 0xFF:
                marks.append('second_hit')
                ret = 0
            else:
                marks.append('second_visible')
                ret = 1
        if (a[21] & 0xFF) != 5:
            gch.append([kLosAddr, 5])
            marks.append('byte_not_5')
        else:
            marks.append('byte_was_5')
    elif fn == CAM_GEO:
        A.wr(o + 0xAC, 4, a[4])
        for i in range(3):
            B.wr(a[6] + 4 + 4 * i, 4, (a[15 + i] ^ 0x5A5A5A5A) if a[13] else a[15 + i]); B.wr(a[11] + 0x30 + 4 * i, 4, a[15 + i] if a[13] else (a[15 + i] ^ 0x5A5A5A5A))
            B.wr(a[7] + 4 + 4 * i, 4, (a[18 + i] ^ 0x5A5A5A5A) if a[14] else a[18 + i]); B.wr(a[12] + 0x30 + 4 * i, 4, a[18 + i] if a[14] else (a[18 + i] ^ 0x5A5A5A5A))
        B.wr(a[6] + 0x14, 4, W.raw(1, a[11]) if a[13] else 0)
        B.wr(a[7] + 0x14, 4, W.raw(1, a[12]) if a[14] else 0)
        W.snap()
        marks.append('matrix0' if a[13] else 'no_matrix0')
        marks.append('matrix1' if a[14] else 'no_matrix1')
        if a[18:21] == a[15:18]:
            marks.append('same_position')
        if ((a[4] >> 23) & 0xFF) < 127 + 2 and (a[4] & 0x7FFFFFFF) != 0:
            marks.append('alpha_small')
        if ((a[5] >> 23) & 0xFF) >= 127 + 6:
            marks.append('beta_large')
        if any(((x >> 23) & 0xFF) >= 127 + 40 for x in a[15:21]):
            marks.append('pos_huge')
        out = geometry(a, W, events)
        src, fnt, tgt = W.raw(2, a[8]), W.raw(2, a[9]), W.raw(3, a[10])
        for base, key in ((fnt, 'front'), (tgt, 'target'), (src, 'source')):
            for i in range(3):
                W.wr(base + 4 * i, 4, out[key][i])
        ret = None
    elif fn == HS_CTOR:
        W.snap()
        A.wr(o + 0x7C, 4, 0); A.wr(o + 0x80, 4, 0)
        events.append([0x517330, tv(this), A.rd(o, 4)])
        A.wr(o, 4, (~a[3]) & M32)
        ret = tv(this)
    elif fn == IC_CTOR:
        W.snap()
        events.append([0x50E6D0, tv(this), A.rd(o, 4)])
        A.wr(o, 4, (~a[3]) & M32)
        ret = tv(this)
    else:
        raise ValueError(fn)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


kLosAddr = 0x8CCB80


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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL (bool)"""
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
