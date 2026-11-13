"""Oraculo independiente (Python) de Fn176A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 176A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x4AD800  CPedDamageResponseCalculator::IsBleedingWeapon(ped): push esi; esi = ecx; ecx = [esp+8]; call 5DF8F0 (CPed::IsPlayer); si al != 0 -> devuelve 0; eax = [esi+0Ch]; 4, 8 o 9 -> 1; si no 0
   1  0x4B5CF0  CEventDamage destructor con borrado: call 4AD960 (cuerpo); si byte [esp+0Ch] & 1: pool = [0B744ACh]; ecx = this - [pool]; edx:eax = ecx * 78787879h; edx = sar(edx, 5); edx += (edx >>> 31)
            (indice = this - almacenamiento, dividido por 44h con signo); byte [[pool+4] + indice] |= 80h; si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; devuelve this
   2  0x502F80  CAEWaterCannonAudioEntity::UpdateGenericWaterCannonSound(splash(byte), id(int16), banco, sfx, velocidad, volumen, x, y, z, rolloff) `ret 28h`: ranura = [this + 90h + id*4];
            con splash: si la ranura es nula: CAESound::Initialise (4EFE50) sobre this+8 con (banco, sfx(dword crudo), this, x, y, z, volumen, 1.0, 1.0, 1.0, 0, 0, 0, 0); [this+1Ch] = volumen; [this+20h] = rolloff;
            [this+24h] = velocidad; word [this+5Eh] = 4; RequestNewSound (4EFB10; ecx = 0B62CB0h) con this+8; ranura = eax; si no es nula: [sonido+14h] = volumen; [sonido+1Ch] = velocidad; SetPosition (4EF880) con (x, y, z);
            sin splash: si la ranura no es nula: StopSound (4EF1C0)
   3  0x591260  FurnitureManager_c::Constructor: iteradores de vectores del CRT (821D30): 9 grupos de 0Ch bytes (3 dwords a 0), 512 elementos de 10h bytes desde +6Ch (+0 y +4 a 0); lista +206Ch (3 dwords a 0); eax = this
   4  0x5912E0  FurnitureManager_c::Destructor: solo llama a destructores que son `ret`: no escribe memoria
   5  0x591360  Interior_c::Destructor: igual
   6  0x646F40  ComputeTarget(salida): puerta = GetPositionToOpenCarDoor (64E740) con (coche = [[this+0Ch]+8], puerta = [[this+0Ch]+0Ch]); posicion del coche P = matriz ([coche+14h] != 0: matriz+30h) o coche+4;
            x87 a 24 bits: dx = Dx - Px; dy = Dy - Py; dz = Dz - Pz (dz se guarda como float); out.x = float(2*dx) + Px (2*dx guardado como float); out.y = Py + 2*dy; out.z = Pz + 2*float(dz) (cada resultado guardado como float)
   7  0x646FE0  CTaskSimpleCarSetPedInAsPassenger::Constructor(vehiculo, puerta, utilidad): call 61A390; [this+14h] = puerta; [this] = 86EE04h; byte [+8] = 0; [+0Ch] = 0; [+10h] = vehiculo; [+18h] = utilidad;
            bytes +1Ch, +1Dh, +1Eh = 0; si vehiculo != 0: RegisterReference (571B70) con ecx = vehiculo y el argumento this+10h; devuelve this
   8  0x64B5B0  CTaskSimpleCarSetPedInAsPassenger destructor con borrado: cuerpo 647080; delete 61A5B0 si bit 0; devuelve this
   9  0x653AA0  CTaskComplexHitPedWithCar::HitKillsPed(ped): IsPlayer (5DF8F0); fld 20.0 (si al != 0) o 12.0; fcomp [this+10h]; test ah, 5; jp -> 0; si no 1 (umbral < impacto, ordenado)
  10  0x6563B0  CTaskSimpleKillPedWithCar destructor con borrado: cuerpo 653C90
  11  0x6563D0  ShakePadIfPlayerVehicle: FindPlayerVehicle(-1, 0) (56E0D0) == [this+8]? si: invMasa = float(1.0 / [coche+8Ch]); s = sqrt((x*x + y*y) + z*z) con x = [coche+44h], y = [+48h], z = [+4Ch];
            v = (s * invMasa) * 200000.0 + 80.0; si v > 250.0: v = 250.0; f = _ftol(v) & 0FFh; GetPad (53FB70) con 0; StartShake (53F920) con (40000 / f, f, 0)
  12  0x657B60  CTaskComplexWalkRoundCar destructor con borrado: cuerpo 656B00
  13  0x675C80  CTaskInteriorSitInChair::GetTaskType: mov eax, 57Fh
  14  0x675C90  CTaskInteriorSitInChair cuerpo del destructor: [this] = 870314h; si [this+14h] != 0: SetDeleteCallback (4CEBC0) con (4D1390h, 0); ~CTask (61A3A0)
  15  0x675CF0  CTaskInteriorSitInChair::Clone: operator new (61A5A0) con 2Ch; si es nulo devuelve 0; constructor (675C30) con ([this+8], [this+0Ch], byte [this+10h] sin signo); devuelve el EAX del constructor
  16  0x675D60  CTaskInteriorSitInChair::MakeAbortable(ped, prioridad, evento) `ret 0Ch`: prioridad == 2: IsLooking (6181A0; ecx = 0C15448h); si al != 0: AbortLookAt (618280) con (ped, 250); si [this+14h] != 0: [anim+1Ch] = -1000.0;
            SetDeleteCallback (4CEBC0) con (4D1390h, 0); [this+14h] = 0; devuelve 1; si no: byte [this+2Ah] = 1; devuelve 0
  17  0x691460  CTaskSimpleHoldEntity::GetTaskType: mov eax, 133h
  18  0x691630  CTaskSimpleHoldEntity cuerpo del destructor: [this] = 870B2Ch; si byte [this+35h] == 0 y [this+8] != 0: entidad.banderas(+1Ch) &= FFFFFBFFh; CleanUpOldReference (571A00; ecx = entidad) con this+8;
            banderas |= 800h; [this+8] = 0; si [this+38h] != 0: SetFinishCallback (4CEBE0) con (4D1390h, 0); si [this+2Ch] != 0: RemoveAnimBlockRef (4D3FD0) con ((bloque - 0B5D4A0h) >> 5 aritmetico); ~CTask (61A3A0)
  19  0x693590  CTaskSimplePickUpEntity destructor con borrado: call 6935B0 (jmp 691630); delete 61A5B0 si bit 0
  20  0x6935C0  CTaskSimplePutDownEntity destructor con borrado: call 6935E0 (jmp 691630)
  21  0x6935F0  CTaskComplexGoPickUpEntity destructor con borrado: cuerpo 691A50
  22  0x6939D0  CTaskSimpleDuck destructor con borrado: cuerpo 692030
  23  0x728AB0  CAEWaterCannonAudioEntity cuerpo del destructor: [this] = 872A60h; Terminate (5030A0); [this] = 859988h; CAESound::~CAESound (4EF660) sobre this+8
  24  0x7448B0  HandlePadButtonDown (EAX = estado): GetPad (53FB70) con [estado]; padTwo = byte [0B73400h] != 0; UpdateJoyButtonState (52F510; ecx = 0B70198h) con 0/1; para i = 1..16: boton = i si byte [0B70CF3h + i] == 1, si no 0;
            si byte [0BA67A4h] != 0 o padTwo: UpdateJoyInConfigMenus_ButtonDown (52DAB0) con (boton, 0/1); si no UpdateJoy_ButtonDown (530ED0) con (boton, 3); eax = 1
  25  0x744930  HandlePadButtonUp (EAX = estado): igual con bCam = modo de la camara activa (word [0B6F1A8h + [0B6F081h]*238h]) en {15, 17} leido ANTES de UpdateJoyButtonState; bucle i = 2..16, boton = i si byte == 0;
            si menu o padTwo o bCam: UpdateJoyInConfigMenus_ButtonUp (52DC20) con (boton, 0/1); si no AffectControllerStateOn_ButtonUp (531070) con (boton, 3)

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = [l for l in open(__file__.replace('fixture_oracle.py', 'names.txt'), encoding='utf-8').read().split('\n') if l] if False else [
    "CPedDamageResponseCalculator::IsBleedingWeapon", "CEventDamage::ScalarDeletingDestructor", "CAEWaterCannonAudioEntity::UpdateGenericWaterCannonSound", "FurnitureManager_c::Constructor",
    "FurnitureManager_c::Destructor", "Interior_c::Destructor", "CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ComputeTarget", "CTaskSimpleCarSetPedInAsPassenger::Constructor",
    "CTaskSimpleCarSetPedInAsPassenger::ScalarDeletingDestructor", "CTaskComplexHitPedWithCar::HitKillsPed", "CTaskSimpleKillPedWithCar::ScalarDeletingDestructor",
    "CTaskSimpleKillPedWithCar::ShakePadIfPlayerVehicle", "CTaskComplexWalkRoundCar::ScalarDeletingDestructor", "CTaskInteriorSitInChair::GetTaskType", "CTaskInteriorSitInChair::Destructor",
    "CTaskInteriorSitInChair::Clone", "CTaskInteriorSitInChair::MakeAbortable", "CTaskSimpleHoldEntity::GetTaskType", "CTaskSimpleHoldEntity::Destructor",
    "CTaskSimplePickUpEntity::ScalarDeletingDestructor", "CTaskSimplePutDownEntity::ScalarDeletingDestructor", "CTaskComplexGoPickUpEntity::ScalarDeletingDestructor",
    "CTaskSimpleDuck::ScalarDeletingDestructor", "CAEWaterCannonAudioEntity::DestructorBody", "Input::HandlePadButtonDown", "Input::HandlePadButtonUp"]
ENTRIES = (0x4AD800, 0x4B5CF0, 0x502F80, 0x591260, 0x5912E0, 0x591360, 0x646F40, 0x646FE0, 0x64B5B0, 0x653AA0, 0x6563B0, 0x6563D0, 0x657B60, 0x675C80, 0x675C90, 0x675CF0, 0x675D60, 0x691460,
           0x691630, 0x693590, 0x6935C0, 0x6935F0, 0x6939D0, 0x728AB0, 0x7448B0, 0x744930)
GUARD = 0x40
WIN = 0x1000
WIN_A = 0x2200
SIZES = (GUARD + WIN_A + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD, GUARD + WIN + GUARD)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
(BLEED, EVTSDD, WATER, FURN, EMPTY, TARGET, PASSCTOR, SDD, HITKILLS, SHAKE, CONST, SITDTOR, SITCLONE, SITABORT, HOLDDTOR, WCDTOR, PADDOWN, PADUP) = range(18)
KIND = [BLEED, EVTSDD, WATER, FURN, EMPTY, EMPTY, TARGET, PASSCTOR, SDD, HITKILLS, SDD, SHAKE, SDD, CONST, SITDTOR, SITCLONE, SITABORT, CONST, HOLDDTOR, SDD, SDD, SDD, SDD, WCDTOR, PADDOWN, PADUP]
CONSTS = {13: 0x57F, 17: 0x133}
SDD_BODY = {1: 0x4AD960, 8: 0x647080, 10: 0x653C90, 12: 0x656B00, 19: 0x691630, 20: 0x691630, 21: 0x691A50, 22: 0x692030}
DELETE = 0x61A5B0
POOL_GLOBAL = 0xB744AC
DEFAULT_CB = 0x4D1390


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


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == BLEED:
        _k += ['weapon_bleeds', 'weapon_other', 'is_player', 'not_player', 'player_high_bytes_only']
    elif _kind == EVTSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'idx_negative', 'idx_zero', 'idx_positive', 'cursor_lowered', 'cursor_kept', 'state_was_set', 'extra_offset']
    elif _kind == WATER:
        _k += ['splash_on', 'splash_off', 'slot_null', 'slot_set', 'id_in_range', 'id_out_of_range', 'created', 'updated', 'stopped', 'nothing']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == TARGET:
        _k += ['matrix', 'no_matrix', 'result_nan', 'result_inf', 'nan_input', 'inf_input', 'denormal_input', 'store_matters_x', 'store_matters_y']
    elif _kind == PASSCTOR:
        _k += ['vehicle_null', 'vehicle_set', 'prev_zero', 'prev_other']
    elif _kind == HITKILLS:
        _k += ['player', 'not_player', 'kills', 'survives', 'nan_impact', 'equal_threshold', 'negative_impact']
    elif _kind == SHAKE:
        _k += ['player_vehicle', 'other_vehicle', 'no_vehicle', 'capped', 'uncapped', 'zero_speed']
    elif _kind == SITDTOR:
        _k += ['anim_null', 'anim_set']
    elif _kind == SITCLONE:
        _k += ['mem_null', 'mem_set', 'mode_high_bytes']
    elif _kind == SITABORT:
        _k += ['immediate', 'not_immediate', 'looking', 'not_looking', 'anim_null', 'anim_set']
    elif _kind == HOLDDTOR:
        _k += ['no_process', 'process', 'entity_null', 'entity_set', 'anim_null', 'anim_set', 'block_null', 'block_set', 'block_negative']
    elif _kind in (PADDOWN, PADUP):
        _k += ['pad_two', 'pad_one', 'menu_on', 'menu_off', 'button_pressed', 'button_released']
        if _kind == PADUP:
            _k += ['cam_mode_flyby', 'cam_mode_fixed', 'cam_mode_other']
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
    compact_a = False
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    if k == BLEED:
        A.wr(o + 0xC, 4, a[4])
        W.snap()
        events.append([0x5DF8F0, W.tagv(a[5])])
        if a[6] & 0xFF:
            ret = 0
            marks.append('is_player')
            if a[6] & 0xFFFFFF00 and not a[6] & 0xFF:
                marks.append('player_high_bytes_only')
        else:
            marks.append('not_player')
            ret = 1 if a[4] in (4, 8, 9) else 0
            marks.append('weapon_bleeds' if ret else 'weapon_other')
        if not a[6] & 0xFF and a[6]:
            marks.append('player_high_bytes_only')
    elif k == EVTSDD:
        delta = signed32(a[5])
        idx0 = abs(delta) // 0x44 * (1 if delta >= 0 else -1)
        B.wr(a[4] + 0, 4, (this - a[5]) & M32); B.wr(a[4] + 4, 4, W.raw(3, a[6])); B.wr(a[4] + 8, 4, a[12]); B.wr(a[4] + 0xC, 4, a[10])
        D.wr(a[6] + idx0, 1, a[11])
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
        flags = a[7] & 0xFF
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[7] >> 8 else 'flag_byte_only')
        if flags & 1:
            storage = B.rd(a[4], 4)
            diff = signed32((this - storage) & M32)
            prod = diff * 0x78787879
            edx = (prod >> 32) >> 5            # sar: aritmetico (Python desplaza con signo)
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
            if diff % 0x44:
                marks.append('extra_offset')
        ret = W.tagv(this)
    elif k == WATER:
        ident = signed32(a[5])
        slot = o + 0x90 + 4 * ident
        sound = 0 if a[14] == 0 else W.raw(1, a[15])
        A.wr(slot, 4, sound)
        if sound:
            B.wr(a[15] + 0x14, 4, a[17]); B.wr(a[15] + 0x1C, 4, a[18])
        W.snap()
        splash = a[4] & 0xFF
        marks.append('splash_on' if splash else 'splash_off')
        marks.append('id_in_range' if 0 <= ident <= 3 else 'id_out_of_range')
        cur = A.rd(slot, 4)
        marks.append('slot_set' if cur else 'slot_null')
        if splash:
            if cur == 0:
                events.append([0x4EFE50, W.tagv(this + 8), a[6], a[7], W.tagv(this), a[10], a[11], a[12], a[9], 0x3F800000, 0x3F800000, 0x3F800000, 0, 0, 0, 0])
                A.wr(o + 0x1C, 4, a[9]); A.wr(o + 0x20, 4, a[13]); A.wr(o + 0x24, 4, a[8]); A.wr(o + 0x5E, 2, 4)
                events.append([0x4EFB10, 0xB62CB0, W.tagv(this + 8)])
                A.wr(slot, 4, a[16])
                marks.append('created')
            else:
                B.wr(a[15] + 0x14, 4, a[9]); B.wr(a[15] + 0x1C, 4, a[8])
                events.append([0x4EF880, W.tagv(cur), a[10], a[11], a[12]])
                marks.append('updated')
        else:
            if cur:
                events.append([0x4EF1C0, W.tagv(cur)])
                marks.append('stopped')
            else:
                marks.append('nothing')
    elif k == FURN:
        W.snap()
        for g in range(9):
            for d in range(3):
                A.wr(o + 12 * g + 4 * d, 4, 0)
        for i in range(512):
            A.wr(o + 0x6C + 16 * i, 4, 0); A.wr(o + 0x6C + 16 * i + 4, 4, 0)
        for d in range(3):
            A.wr(o + 0x206C + 4 * d, 4, 0)
        ret = W.tagv(this)
        compact_a = True
    elif k == EMPTY:
        W.snap()
    elif k == TARGET:
        task = W.raw(1, a[4])
        A.wr(o + 0xC, 4, task)
        veh = W.raw(2, a[5])
        B.wr(a[4] + 8, 4, veh); B.wr(a[4] + 0xC, 4, a[6])
        Cc.wr(a[5] + 4, 4, a[7]); Cc.wr(a[5] + 8, 4, a[8]); Cc.wr(a[5] + 0xC, 4, a[9])
        matrix = 0 if a[10] == 0 else W.raw(3, a[11])
        Cc.wr(a[5] + 0x14, 4, matrix)
        if matrix:
            D.wr(a[11] + 0x30, 4, a[12]); D.wr(a[11] + 0x34, 4, a[13]); D.wr(a[11] + 0x38, 4, a[14])
        W.snap()
        events.append([0x64E740, STACK_TAG, W.tagv(veh), a[6]])
        pos = [a[12], a[13], a[14]] if matrix else [a[7], a[8], a[9]]
        door = [a[15], a[16], a[17]]
        dx = v_sub(v_load(door[0]), v_load(pos[0]))
        dy = v_sub(v_load(door[1]), v_load(pos[1]))
        dz = v_sub(v_load(door[2]), v_load(pos[2]))
        dz32 = v_load(store32(dz))
        dx2 = v_load(store32(v_dbl(dx)))
        dy2 = v_dbl(dy)
        dz2 = v_dbl(dz32)
        ox = store32(v_add(dx2, v_load(pos[0])))
        oy = store32(v_add(v_load(pos[1]), dy2))
        oz = store32(v_add(v_load(pos[2]), dz2))
        out = a[18]
        B.wr(out, 4, ox); B.wr(out + 4, 4, oy); B.wr(out + 8, 4, oz)
        marks.append('matrix' if matrix else 'no_matrix')
        res = (ox, oy, oz)
        if any((r & 0x7F800000) == 0x7F800000 and (r & 0x7FFFFF) for r in res):
            marks.append('result_nan')
        if any((r & 0x7FFFFFFF) == 0x7F800000 for r in res):
            marks.append('result_inf')
        ins = list(pos) + list(door)
        if any((r & 0x7F800000) == 0x7F800000 and (r & 0x7FFFFF) for r in ins):
            marks.append('nan_input')
        if any((r & 0x7FFFFFFF) == 0x7F800000 for r in ins):
            marks.append('inf_input')
        if any(0 < (r & 0x7FFFFFFF) < 0x800000 for r in ins):
            marks.append('denormal_input')
        # el doble de la diferencia desborda el float: x (se guarda como float antes de sumar) da infinito; y (sin guardar) da un resultado finito; la suma sin guardar de x tambien seria finita
        if dx[0] == 'fin' and abs(dx[1] * 2) >= Fraction(2) ** 128 and pos[0] & 0x7F800000 != 0x7F800000:
            if v_add(v_dbl(dx), v_load(pos[0]))[0] == 'fin' and abs(v_add(v_dbl(dx), v_load(pos[0]))[1]) < Fraction(2) ** 128:
                marks.append('store_matters_x')
        if dy[0] == 'fin' and abs(dy[1] * 2) >= Fraction(2) ** 128 and (res[1] & 0x7F800000) != 0x7F800000:
            marks.append('store_matters_y')
        ret = W.tagv(W.raw(1, out))
    elif k == PASSCTOR:
        W.snap()
        events.append([0x61A390, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[7])
        A.wr(o + 0x14, 4, a[5])
        A.wr(o, 4, 0x86EE04)
        A.wr(o + 8, 1, 0); A.wr(o + 0xC, 4, 0)
        A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x18, 4, a[6])
        A.wr(o + 0x1C, 1, 0); A.wr(o + 0x1D, 1, 0); A.wr(o + 0x1E, 1, 0)
        if a[4]:
            events.append([0x571B70, W.tagv(a[4]), W.tagv(this + 0x10)])
            marks.append('vehicle_set')
        else:
            marks.append('vehicle_null')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        ret = W.tagv(this)
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == HITKILLS:
        A.wr(o + 0x10, 4, a[4])
        W.snap()
        events.append([0x5DF8F0, W.tagv(a[5])])
        player = (a[6] & 0xFF) != 0
        threshold = 20.0 if player else 12.0
        impact = f32(a[4])
        ret = 1 if (not math.isnan(impact)) and threshold < impact else 0
        marks.append('player' if player else 'not_player')
        marks.append('kills' if ret else 'survives')
        if math.isnan(impact):
            marks.append('nan_impact')
        if impact == threshold:
            marks.append('equal_threshold')
        if (a[4] >> 31) and not math.isnan(impact):
            marks.append('negative_impact')
    elif k == SHAKE:
        car = W.raw(1, a[5]) if a[4] == 0 else a[6]
        A.wr(o + 8, 4, car)
        if a[4] == 0:
            B.wr(a[5] + 0x44, 4, a[9]); B.wr(a[5] + 0x48, 4, a[10]); B.wr(a[5] + 0x4C, 4, a[11]); B.wr(a[5] + 0x8C, 4, a[12])
        player_vehicle = car if a[7] == 0 else (0 if a[7] == 1 else a[8])
        pad = W.raw(2, a[14]) if a[13] == 0 else a[15]
        W.snap()
        events.append([0x56E0D0, 0xFFFFFFFF, 0])
        if player_vehicle == car:
            marks.append('player_vehicle')
            x, y, z = (v_load(a[9]), v_load(a[10]), v_load(a[11]))
            mass = v_load(a[12])
            inv = v_load(store32(('fin', Fraction(0), False) if mass[0] == 'inf' else ('fin', r24(Fraction(1) / mass[1]), False)))
            s2 = v_add(v_add(v_mul(x, x), v_mul(y, y)), v_mul(z, z))
            s = ('fin', sqrt24(s2[1]), False)
            val = v_add(v_mul(v_mul(s, inv), v_load(0x48435000)), v_load(0x42A00000))   # 200000.0 y 80.0
            if val[1] > 250:
                val = ('fin', Fraction(250), False)
                marks.append('capped')
            else:
                marks.append('uncapped')
            if all(v[1] == 0 for v in (x, y, z)):
                marks.append('zero_speed')
            freq = int(val[1]) & 0xFF
            events.append([0x53FB70, 0])
            events.append([0x53F920, W.tagv(pad), 40000 // freq, freq, 0])
        else:
            marks.append('no_vehicle' if player_vehicle == 0 else 'other_vehicle')
    elif k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == SITDTOR:
        anim = a[5] if a[4] else 0
        A.wr(o + 0x14, 4, anim)
        W.snap()
        A.wr(o, 4, 0x870314)
        if anim:
            events.append([0x4CEBC0, W.tagv(anim), DEFAULT_CB, 0])
            marks.append('anim_set')
        else:
            marks.append('anim_null')
        events.append([0x61A3A0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[7])
    elif k == SITCLONE:
        A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[6])
        W.snap()
        events.append([0x61A5A0, 0x2C])
        if a[7] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            mem = W.raw(1, a[8])
            events.append([0x675C30, W.tagv(mem), a[4], a[5], a[6] & 0xFF])
            W.wr(mem, 4, a[10])
            ret = W.tagv(a[9])
            marks.append('mem_set')
        if a[6] >> 8:
            marks.append('mode_high_bytes')
    elif k == SITABORT:
        anim = 0 if a[4] == 0 else W.raw(1, a[5])
        A.wr(o + 0x14, 4, anim)
        A.wr(o + 0x2A, 1, a[10])
        W.snap()
        pri, ped = a[6], a[7]
        if pri == 2:
            marks.append('immediate')
            events.append([0x6181A0, 0xC15448, W.tagv(ped)])
            if a[9] & 0xFF:
                events.append([0x618280, 0xC15448, W.tagv(ped), 250])
                marks.append('looking')
            else:
                marks.append('not_looking')
            if anim:
                B.wr(a[5] + 0x1C, 4, 0xC47A0000)
                events.append([0x4CEBC0, W.tagv(anim), DEFAULT_CB, 0])
                A.wr(o + 0x14, 4, 0)
                marks.append('anim_set')
            else:
                marks.append('anim_null')
            ret = 1
        else:
            marks.append('not_immediate')
            A.wr(o + 0x2A, 1, 1)
            ret = 0
    elif k == HOLDDTOR:
        A.wr(o + 0x35, 1, a[4])
        entity = 0 if a[5] == 0 else W.raw(1, a[6])
        A.wr(o + 8, 4, entity)
        if entity:
            B.wr(a[6] + 0x1C, 4, a[7])
        A.wr(o + 0x38, 4, a[8]); A.wr(o + 0x2C, 4, a[9])
        W.snap()
        A.wr(o, 4, 0x870B2C)
        if A.rd(o + 0x35, 1) == 0:
            marks.append('no_process')
            if entity:
                marks.append('entity_set')
                flags = B.rd(a[6] + 0x1C, 4) & 0xFFFFFBFF
                B.wr(a[6] + 0x1C, 4, flags)
                events.append([0x571A00, W.tagv(entity), W.tagv(this + 8)])
                B.wr(a[6] + 0x1C, 4, B.rd(a[6] + 0x1C, 4) | 0x800)
                A.wr(o + 8, 4, 0)
            else:
                marks.append('entity_null')
        else:
            marks.append('process')
            marks.append('entity_set' if entity else 'entity_null')
        if a[8]:
            events.append([0x4CEBE0, W.tagv(a[8]), DEFAULT_CB, 0])
            marks.append('anim_set')
        else:
            marks.append('anim_null')
        if a[9]:
            idx = signed32((a[9] - 0xB5D4A0) & M32) >> 5
            events.append([0x4D3FD0, idx & M32])
            marks.append('block_set')
            if idx < 0:
                marks.append('block_negative')
        else:
            marks.append('block_null')
        events.append([0x61A3A0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[11])
    elif k == WCDTOR:
        W.snap()
        A.wr(o, 4, 0x872A60)
        events.append([0x5030A0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, 0x859988)
        events.append([0x4EF660, W.tagv(this + 8), A.rd(o + 8, 4)])
    elif k in (PADDOWN, PADUP):
        B.wr(a[15], 4, a[4])
        W.snap()
        pad_num = a[5] & 0xFF
        pad_two = pad_num != 0
        pn = 1 if pad_two else 0
        menu = (a[6] & 0xFF) != 0
        states = b''.join(a[7 + w].to_bytes(4, 'little') for w in range(4))
        bcam = False
        if k == PADUP:
            mode = a[12 + (a[11] & 0xFF)] & 0xFFFF
            bcam = mode in (15, 17)
            marks.append('cam_mode_flyby' if mode == 15 else ('cam_mode_fixed' if mode == 17 else 'cam_mode_other'))
        events.append([0x53FB70, a[4]])
        events.append([0x52F510, 0xB70198, pn])
        marks.append('pad_two' if pad_two else 'pad_one')
        marks.append('menu_on' if menu else 'menu_off')
        for i in range(1 if k == PADDOWN else 2, 17):
            if k == PADDOWN:
                btn = i if states[i - 1] == 1 else 0
                cond = menu or pad_two
                events.append([0x52DAB0 if cond else 0x530ED0, 0xB70198, btn, pn if cond else 3])
            else:
                btn = i if states[i - 1] == 0 else 0
                cond = menu or pad_two or bcam
                events.append([0x52DC20 if cond else 0x531070, 0xB70198, btn, pn if cond else 3])
            marks.append('button_pressed' if btn else 'button_released')
        ret = 1
    changes = {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}
    if compact_a:
        changes['A'] = {'n': len(changes['A']), 'h': fnv_changes(changes['A'])}
    return {'events': events, 'changes': changes, 'gchanges': [], 'ret': ret, 'marks': marks}


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
