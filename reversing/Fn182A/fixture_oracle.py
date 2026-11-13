"""Oraculo independiente (Python) de Fn182A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 23 funciones del lote 182A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado) con fracciones exactas, y el `_ftol2` del EXE (0x821B40).

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x53BC60  CGame::ActivateMenuOnSetFocus: si byte [0BA6831h] == 0 y byte [0BA67A4h] == 0: byte [0BA677Bh] = 1
   1  0x53C860  ReleaseBlockA: esi = ecx; eax = [esi+4]; si no es nulo: free (8214BD) con eax; [esi+4] = 0; [esi+8] = 0; [esi+0Ch] = 0
   2  0x53C890  ReleaseBlockB: identica
   3  0x572130  GetCurrentZoneLockedOrUnlocked(x, y): para x e y: fld; fadd [859A94h] (3000.0f); fmul [865060h] (0.0016666667f); call _ftol2 (821B40); cx = al, cy = al (movzx);
            ecx = 10*cx - cy; al = byte [ecx + 0BA3739h]; devuelve al != 0. _ftol2: fistp (al mas cercano) y ajuste por la parte fraccionaria: trunc hacia cero; fuera del rango de 64 bits o NaN: eax = 0
   4  0x572DB0  CPad::IsEscJustPressed: word [0B733A8h] != 0 y word [0B73138h] == 0 -> 1, si no 0
   5  0x572E70  CPad::IsMouseRButtonPressed: byte [0B73419h] != 0 y byte [0B7342Dh] == 0 -> 1, si no 0
   6  0x5D3020  CSaveGarage::CopyGarageIntoSaveGarage(g): bytes [g+4Ch], [g+4Dh], [g+4Eh] a [this+0], [this+1], [this+2]; dwords [g+0] .. [g+3Ch] a [this+4] .. [this+40h]; cadena desde [g+44h] a [this+44h] con su NUL
            (byte a byte hasta el NUL, incluido); byte [g+4Fh] a [this+4Ch]
   7  0x5D30C0  CSaveGarage::CopyGarageOutOfSaveGarage(g): bytes [this+0], [+1], [+2] a [g+4Ch], [g+4Dh], [g+4Eh]; dwords [this+4] .. [this+40h] a [g+0] .. [g+3Ch]; [g+40h] = 0; cadena [this+44h] a [g+44h]; byte [this+4Ch] a [g+4Fh]
   8  0x634440  CTaskSimpleTriggerLookAt::Constructor(entidad, tiempo, hueso, x, y, z, torso, velocidad, mezcla, prioridad): call 61A390; [+0Ch] = tiempo; [+10h] = hueso; [+14h..+1Ch] = x, y, z; byte [+20h] = torso;
            [+24h] = velocidad; [+28h] = mezcla; [+0] = 86E3CCh; [+8] = entidad; byte [+2Dh] = prioridad; con entidad: RegisterReference (571B70; ecx = entidad) con this+8 y byte [+2Ch] = 1; si no byte [+2Ch] = 0; eax = this
   9  0x6344F0  GetTaskType: mov eax, 10Dh
  10  0x634560  CTaskSimpleTriggerLookAt::Clone: tiempo = [+0Ch]; hueso = [+10h]; si hueso > -1 (con signo) y [+8] == 0: hueso = -1, tiempo = 100; new (61A5A0) con 30h; si es nulo devuelve 0; si no el constructor 634440
            con ([+8], tiempo, hueso, [+14h], [+18h], [+1Ch], byte [+20h] sin signo, [+24h], [+28h], byte [+2Dh] con signo); devuelve el EAX del constructor
  11  0x634610  MakeAbortable: mov al, 1; ret 0Ch
  12  0x634620  ProcessPed(ped): si byte [+2Ch] == 0 o [+8] != 0: IKChainManager_c::LookAt (618970; ecx = 0C15448h) con ("TaskTriggerLookAt" 86E3F0, ped, [+8], [+0Ch], [+10h], this+14h, byte [+20h] sin signo,
            [+24h], [+28h], byte [+2Dh] con signo, 0); devuelve 1
  13  0x654840  CTaskComplexWalkRoundBuildingAttempt::ConstructorEntity(estado, entidad, &desplazamiento, &pos, &normal, bandera): call 61A3B0; [+0] = 86F334h; [+20h..+28h] = pos; [+2Ch..+34h] = normal; [+38h] = entidad;
            [+3Ch..+44h] = desplazamiento; byte [+48h] = estado; byte [+49h] = (byte & 0C4h) | (bandera & 1) | 4; byte [+4Ah] = 0; [+34h] = 0; Normalise (59C910; ecx = this+2Ch); new (41B7C0) con 64h: si no es nulo [ruta] = 0; [+0Ch] = ruta;
            otra igual a [+10h]; RegisterReference (571B70; ecx = entidad, aunque sea nula) con this+38h; eax = this
  14  0x655C10  CTaskComplexWalkRoundObject::Clone: new (61A5A0) con 54h; si es nulo devuelve 0; constructor 655020 con ([+0Ch], this+10h, [+1Ch]); devuelve su EAX
  15  0x657200  destructor con borrado de CTaskComplexWalkRoundObject: cuerpo 6550D0; delete 61A5B0 si bit 0 del byte de banderas; devuelve this
  16  0x657750  destructor con borrado de CTaskComplexMoveBackAndJump: cuerpo 655370; delete 61A5B0 si bit 0; devuelve this
  17  0x72FEC0  D3DTextureBuffer::Clear: eax = [this+14h]; si eax > 0 (con signo): repite: obj = [[this+18h] + i*4]; Release = [[obj]+8] (stdcall con obj); eax = [this+14h]; i++; mientras i < eax (con signo); [+14h] = 0; [+10h] = 0
  18  0x72FEF0  D3DTextureBuffer::Destroy: igual; despues eax = [this+18h]; [+14h] = 0; [+10h] = 0; si eax != 0: free (8214BD) con eax; [+18h] = 0
  19  0x7301D0  D3DIndexDataBuffer::Clear: igual con numero [this+10h], pila [this+14h] y contador [this+0Ch]
  20  0x730200  D3DIndexDataBuffer::Destroy: igual con la pila [this+14h]
  21  0x730450  GrowableArrayFree: eax = [this+8]; si no es nulo: free (8214BD) con eax; [+8] = 0; [+4] = 0
  22  0x7304B0  GrowableArrayResize(nueva): si nueva == [this] no hace nada; si no: new (82119A) con nueva*4 (32 bits); i = 0; mientras [this+4] > i (primera vuelta [this+4] > 0) y i < nueva (con signo): fresh[i] = [[this+8] + i*4];
            (se relee [this+4] tras cada copia); free (8214BD) con [this+8] (incluso nulo); [this+8] = fresh; [this] = nueva

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
import zlib
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = [
    "CGame::ActivateMenuOnSetFocus", "CGame::ReleaseBlockA", "CGame::ReleaseBlockB", "CTheZones::GetCurrentZoneLockedOrUnlocked", "CPad::IsEscJustPressed", "CPad::IsMouseRButtonPressed",
    "CSaveGarage::CopyGarageIntoSaveGarage", "CSaveGarage::CopyGarageOutOfSaveGarage", "CTaskSimpleTriggerLookAt::Constructor", "CTaskSimpleTriggerLookAt::GetTaskType",
    "CTaskSimpleTriggerLookAt::Clone", "CTaskSimpleTriggerLookAt::MakeAbortable", "CTaskSimpleTriggerLookAt::ProcessPed", "CTaskComplexWalkRoundBuildingAttempt::ConstructorEntity",
    "CTaskComplexWalkRoundObject::Clone", "CTaskComplexWalkRoundObject::ScalarDeletingDestructor", "CTaskComplexMoveBackAndJump::ScalarDeletingDestructor", "D3DTextureBuffer::Clear",
    "D3DTextureBuffer::Destroy", "D3DIndexDataBuffer::Clear", "D3DIndexDataBuffer::Destroy", "D3DResourceSystem::GrowableArrayFree", "D3DResourceSystem::GrowableArrayResize"]
ENTRIES = (0x53BC60, 0x53C860, 0x53C890, 0x572130, 0x572DB0, 0x572E70, 0x5D3020, 0x5D30C0, 0x634440, 0x6344F0, 0x634560, 0x634610, 0x634620, 0x654840, 0x655C10, 0x657200, 0x657750,
           0x72FEC0, 0x72FEF0, 0x7301D0, 0x730200, 0x730450, 0x7304B0)
GUARD = 0x40
WIN = 0x1000
SIZES = (GUARD + WIN + GUARD,) * 4
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
(MENU, REL, ZONE, ESC, MOUSER, GARIN, GAROUT, TLACTOR, CONST, TLACLONE, RETTRUE, TLAPROC, BLDCTOR, WROCLONE, SDD, BUFTEX, BUFIDX, GROWFREE, GROWRESIZE) = range(19)
KIND = [MENU, REL, REL, ZONE, ESC, MOUSER, GARIN, GAROUT, TLACTOR, CONST, TLACLONE, RETTRUE, TLAPROC, BLDCTOR, WROCLONE, SDD, SDD, BUFTEX, BUFTEX, BUFIDX, BUFIDX, GROWFREE, GROWRESIZE]
SDD_BODY = {15: 0x6550D0, 16: 0x655370}
DELETE, FREE, NEW_OP, COM_RELEASE = 0x61A5B0, 0x8214BD, 0x82119A, 0xC0000002
ZONE_REGION, ZONE_SIZE, ZONES_VISITED = 0xBA3600, 0xC00, 0xBA3730
ZONE_SNAP = None      # imagen de la region que lee la funcion de zonas (0xBA3600..0xBA41FF) al empezar el banco: la fija run_local.analyze desde el evento `begin`; la autoprueba usa una imagen sintetica
LOOKAT_NAME = b'TaskTriggerLookAt'
FIXED_VTABLE = {8: 0x86E3CC, 13: 0x86F334}


def set_zone_snapshot(hexstr):
    global ZONE_SNAP
    ZONE_SNAP = bytes.fromhex(hexstr) if hexstr else None


def zone_base():
    return ZONE_SNAP if ZONE_SNAP is not None else bytes((i * 7 + 3) & 0xFF if (i % 5) else 0 for i in range(ZONE_SIZE))


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
_KIND_KEYS = {
    MENU: ['menu_sets', 'menu_main_blocks', 'menu_active_blocks', 'menu_prev_one'],
    REL: ['block_null', 'block_arena', 'block_raw'],
    ZONE: ['cell_in_map', 'cell_out_of_map', 'cell_negative', 'cell_wrapped', 'cell_nan_inf', 'beyond_int64', 'visited', 'not_visited', 'fraction_edge'],
    ESC: ['esc_new_set', 'esc_new_clear', 'esc_old_set', 'esc_old_clear', 'esc_true', 'esc_false', 'esc_high_byte_only'],
    MOUSER: ['mouse_new_set', 'mouse_new_clear', 'mouse_old_set', 'mouse_old_clear', 'mouse_true', 'mouse_false'],
    GARIN: ['name_empty', 'name_short', 'name_long', 'name_overflows_field'],
    GAROUT: ['name_empty', 'name_short', 'name_long', 'name_overflows_field'],
    TLACTOR: ['entity_null', 'entity_arena', 'entity_raw', 'torso_set', 'torso_clear', 'prio_negative', 'nan_offset', 'prev_dword'],
    CONST: [],
    TLACLONE: ['entity_null', 'entity_set', 'bone_reset', 'bone_kept', 'mem_null', 'mem_set', 'torso_other', 'prio_negative'],
    RETTRUE: [],
    TLAPROC: ['look_exists_null', 'look_no_entity_flag', 'look_entity', 'skipped', 'torso_other', 'prio_negative'],
    BLDCTOR: ['entity_null', 'entity_arena', 'entity_raw', 'flag_set', 'flag_clear', 'route1_null', 'route1_set', 'route2_null', 'route2_set', 'bits_kept'],
    WROCLONE: ['mem_null', 'mem_set'],
    SDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    BUFTEX: ['size_positive', 'size_zero', 'size_negative', 'stack_null', 'stack_arena', 'stack_raw', 'mut_none', 'mut_shrink', 'mut_negative', 'mut_swap', 'mut_swap_grow', 'mut_null', 'mut_raw'],
    BUFIDX: ['size_positive', 'size_zero', 'size_negative', 'stack_null', 'stack_arena', 'stack_raw', 'mut_none', 'mut_shrink', 'mut_negative', 'mut_swap', 'mut_swap_grow', 'mut_null', 'mut_raw'],
    GROWFREE: ['data_null', 'data_arena', 'data_raw'],
    GROWRESIZE: ['unchanged', 'copies', 'no_copy_count', 'no_copy_cap', 'count_negative', 'cap_negative', 'data_null_freed', 'shrinks'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = COMMON + _KIND_KEYS[KIND[_fn]]


def sx8(v):
    v &= 0xFF
    return (v - 0x100 if v >= 0x80 else v) & M32


def ftol2_eax(v):
    """eax que deja _ftol2 (0x821B40) con ST0 = v (tupla del modelo x87): fistp al mas cercano (par en empates); fuera del rango de 64 bits o NaN: indefinido (eax = 0); luego el ajuste a truncado hacia cero"""
    if v[0] != 'fin':
        return 0
    fr = v[1]
    if abs(fr) >= Fraction(1 << 63):
        r = _round_even(abs(fr))
        r = -r if fr < 0 else r
        if not (-(1 << 63) <= r < (1 << 63)):
            return 0
    t = abs(fr).numerator // abs(fr).denominator
    t = -t if fr < 0 else t
    return t & M32


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    extra = {}
    o = a[0]
    this = W.raw(0, o)
    if k not in (GROWFREE, GROWRESIZE):
        A.wr(o, 4, a[3])

    def entity(at):
        mode = a[at]
        ent = 0 if mode == 0 else (W.raw(1, a[at + 1]) if mode == 1 else a[at + 2])
        marks.append('entity_null' if ent == 0 else ('entity_arena' if mode == 1 else 'entity_raw'))
        return ent

    def gset(addr, n, v):
        for i in range(n):
            W.glob[addr + i] = (v >> (8 * i)) & 0xFF

    if k == MENU:
        gset(0xBA6831, 1, a[4]); gset(0xBA67A4, 1, a[5]); gset(0xBA677B, 1, a[6])
        gorder = [(0xBA6831, 1), (0xBA67A4, 1), (0xBA677B, 1)]
        W.snap()
        if (a[4] & 0xFF) == 0 and (a[5] & 0xFF) == 0:
            gset(0xBA677B, 1, 1)
            marks.append('menu_sets')
            if (a[6] & 0xFF) == 1:
                marks.append('menu_prev_one')
        elif (a[4] & 0xFF) != 0:
            marks.append('menu_main_blocks')
        else:
            marks.append('menu_active_blocks')
    elif k == REL:
        ptr = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        A.wr(o + 4, 4, ptr); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        W.snap()
        marks.append('block_null' if ptr == 0 else ('block_arena' if a[4] == 1 else 'block_raw'))
        if ptr:
            events.append([FREE, W.tagv(ptr)])
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
    elif k == ZONE:
        seeds = b''.join(a[6 + w].to_bytes(4, 'little') for w in range(25))
        W.snap()
        zone = bytearray(zone_base())
        zone[ZONES_VISITED - ZONE_REGION:ZONES_VISITED - ZONE_REGION + 100] = seeds
        extra['zone_crc'] = zlib.crc32(bytes(zone)) & M32
        gorder = [(ZONES_VISITED, 100)]
        for i in range(100):
            W.glob[ZONES_VISITED + i] = seeds[i]
        W.glob_ref = dict(W.glob)
        cells = []
        for bits in (a[4], a[5]):
            v = v_mul(v_add(v_load(bits), v_load(0x453B8000)), v_load(0x3ADA740E))
            e = ftol2_eax(v)
            cells.append((e, v))
        cx, cy = cells[0][0] & 0xFF, cells[1][0] & 0xFF
        index = 10 * cx - cy
        addr = 0xBA3739 + index
        byte = zone[addr - ZONE_REGION]
        ret = 1 if byte else 0
        marks.append('cell_in_map' if cx < 10 and cy < 10 else 'cell_out_of_map')
        if any(signed32(e) < 0 for e, _ in cells):
            marks.append('cell_negative')
        if any(0 < signed32(e) and (e & M32) > 255 for e, _ in cells):
            marks.append('cell_wrapped')
        if any(v[0] != 'fin' for _, v in cells):
            marks.append('cell_nan_inf')
        if any(v[0] == 'fin' and abs(v[1]) >= Fraction(1 << 63) for _, v in cells):
            marks.append('beyond_int64')
        if any(v[0] == 'fin' and v[1] != 0 and v[1] == int(v[1]) and abs(v[1]) < 300 for _, v in cells):
            marks.append('fraction_edge')
        marks.append('visited' if ret else 'not_visited')
    elif k == ESC:
        gorder = [(0xB733A8, 2), (0xB73138, 2)]
        gset(0xB733A8, 2, a[4]); gset(0xB73138, 2, a[5])
        W.snap()
        new, old = a[4] & 0xFFFF, a[5] & 0xFFFF
        ret = 1 if new != 0 and old == 0 else 0
        marks.append('esc_new_set' if new else 'esc_new_clear')
        marks.append('esc_old_set' if old else 'esc_old_clear')
        marks.append('esc_true' if ret else 'esc_false')
        if new and not (new & 0xFF) or old and not (old & 0xFF):
            marks.append('esc_high_byte_only')
    elif k == MOUSER:
        gorder = [(0xB73419, 1), (0xB7342D, 1)]
        gset(0xB73419, 1, a[4]); gset(0xB7342D, 1, a[5])
        W.snap()
        new, old = a[4] & 0xFF, a[5] & 0xFF
        ret = 1 if new != 0 and old == 0 else 0
        marks.append('mouse_new_set' if new else 'mouse_new_clear')
        marks.append('mouse_old_set' if old else 'mouse_old_clear')
        marks.append('mouse_true' if ret else 'mouse_false')
    elif k in (GARIN, GAROUT):
        name = b''.join(a[6 + w].to_bytes(4, 'little') for w in range(8))[:a[5]] + b'\0'
        marks.append('name_empty' if a[5] == 0 else ('name_short' if a[5] < 8 else 'name_long'))
        if a[5] + 1 > 8:
            marks.append('name_overflows_field')
        if k == GARIN:
            src, dst, soff, doff = B, A, a[4], o        # src = CGarage en B, dst = this (CSaveGarage) en A
        else:
            src, dst, soff, doff = A, B, o, a[4]        # src = this (CSaveGarage) en A, dst = CGarage en B
        for i, b in enumerate(name):
            src.wr(soff + 0x44 + i, 1, b)
        W.snap()
        if k == GARIN:
            for i in range(3):
                dst.wr(doff + i, 1, src.rd(soff + 0x4C + i, 1))
            for d in range(16):
                dst.wr(doff + 4 + 4 * d, 4, src.rd(soff + 4 * d, 4))
            for i in range(0x40):
                b = src.rd(soff + 0x44 + i, 1)
                dst.wr(doff + 0x44 + i, 1, b)
                if b == 0:
                    break
            dst.wr(doff + 0x4C, 1, src.rd(soff + 0x4F, 1))
        else:
            for i in range(3):
                dst.wr(doff + 0x4C + i, 1, src.rd(soff + i, 1))
            for d in range(16):
                dst.wr(doff + 4 * d, 4, src.rd(soff + 4 + 4 * d, 4))
            dst.wr(doff + 0x40, 4, 0)
            for i in range(0x40):
                b = src.rd(soff + 0x44 + i, 1)
                dst.wr(doff + 0x44 + i, 1, b)
                if b == 0:
                    break
            dst.wr(doff + 0x4F, 1, src.rd(soff + 0x4C, 1))
    elif k in (TLACTOR, TLACLONE, TLAPROC, BLDCTOR, WROCLONE):
        pass
    if k == TLACTOR:
        ent = entity(4)
        W.snap()
        A.wr(o + 4, 4, 0)           # base CTaskSimple real (0x61A390): [+4] = 0
        A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[8]); A.wr(o + 0x14, 4, a[9]); A.wr(o + 0x18, 4, a[10]); A.wr(o + 0x1C, 4, a[11])
        A.wr(o + 0x20, 1, a[12]); A.wr(o + 0x24, 4, a[13]); A.wr(o + 0x28, 4, a[14]); A.wr(o + 0x2D, 1, a[15])
        A.wr(o, 4, FIXED_VTABLE[fn]); extra['vtable_ok'] = True
        A.wr(o + 8, 4, ent)
        if ent:
            events.append([0x571B70, W.tagv(ent), W.tagv(this + 8)])
            A.wr(o + 0x2C, 1, 1)
        else:
            A.wr(o + 0x2C, 1, 0)
        ret = W.tagv(this)
        marks.append('torso_set' if a[12] & 0xFF else 'torso_clear')
        if a[15] & 0x80:
            marks.append('prio_negative')
        if any((x & 0x7F800000) == 0x7F800000 and (x & 0x7FFFFF) for x in a[9:12]):
            marks.append('nan_offset')
        if a[3]:
            marks.append('prev_dword')
    elif k == CONST:
        W.snap()
        ret = 0x10D
    elif k == TLACLONE:
        ent = entity(4)
        A.wr(o + 8, 4, ent)
        A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[8]); A.wr(o + 0x14, 4, a[9]); A.wr(o + 0x18, 4, a[10]); A.wr(o + 0x1C, 4, a[11])
        A.wr(o + 0x20, 1, a[12]); A.wr(o + 0x24, 4, a[13]); A.wr(o + 0x28, 4, a[14]); A.wr(o + 0x2D, 1, a[15])
        W.snap()
        marks[-1] = 'entity_null' if ent == 0 else 'entity_set'
        time, bone = a[7], a[8]
        if signed32(bone) > -1 and ent == 0:
            bone, time = M32, 100
            marks.append('bone_reset')
        else:
            marks.append('bone_kept')
        events.append([0x61A5A0, 0x30])
        if a[16] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            mem = W.raw(1, a[17])
            events.append([0x634440, W.tagv(mem), W.tagv(ent), time, bone, a[9], a[10], a[11], a[12] & 0xFF, a[13], a[14], sx8(a[15])])
            W.wr(mem, 4, a[19])
            ret = W.tagv(a[18])
            marks.append('mem_set')
        if (a[12] & 0xFF) > 1:
            marks.append('torso_other')
        if a[15] & 0x80:
            marks.append('prio_negative')
    elif k == RETTRUE:
        W.snap()
        ret = 1
    elif k == TLAPROC:
        ent = entity(4)
        marks.pop()
        A.wr(o + 8, 4, ent)
        A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[8]); A.wr(o + 0x14, 4, a[9]); A.wr(o + 0x18, 4, a[10]); A.wr(o + 0x1C, 4, a[11])
        A.wr(o + 0x20, 1, a[12]); A.wr(o + 0x24, 4, a[13]); A.wr(o + 0x28, 4, a[14]); A.wr(o + 0x2C, 1, a[15]); A.wr(o + 0x2D, 1, a[16])
        W.snap()
        exists = a[15] & 0xFF
        if exists == 0 or ent != 0:
            events.append([0x618970, 0xC15448, zlib.crc32(LOOKAT_NAME) & M32, W.tagv(a[17]), W.tagv(ent), a[7], a[8], W.tagv(this + 0x14), a[12] & 0xFF, a[13], a[14], sx8(a[16]), 0])
            marks.append('look_entity' if ent else ('look_exists_null' if exists == 0 else 'look_no_entity_flag'))
            if exists == 0 and ent == 0:
                marks.append('look_no_entity_flag')
        else:
            marks.append('skipped')
        if (a[12] & 0xFF) > 1:
            marks.append('torso_other')
        if a[16] & 0x80:
            marks.append('prio_negative')
        ret = 1
    elif k == BLDCTOR:
        ent = entity(5)
        marks[-1] = 'entity_null' if ent == 0 else ('entity_arena' if a[5] == 1 else 'entity_raw')
        for i in range(3):
            Cc.wr(a[11] + 4 * i, 4, a[8 + i]); B.wr(a[15] + 4 * i, 4, a[12 + i]); D.wr(a[19] + 4 * i, 4, a[16 + i])
        route1 = 0 if a[23] == 0 else W.raw(2, a[24])
        route2 = 0 if a[25] == 0 else W.raw(3, a[26])
        W.snap()
        old49 = A.rd(o + 0x49, 1)
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0)   # base CTaskComplex real (0x61A3B0): [+4] = 0, [+8] = 0
        A.wr(o, 4, FIXED_VTABLE[fn]); extra['vtable_ok'] = True
        for i in range(3):
            A.wr(o + 0x20 + 4 * i, 4, a[12 + i]); A.wr(o + 0x2C + 4 * i, 4, a[16 + i]); A.wr(o + 0x3C + 4 * i, 4, a[8 + i])
        A.wr(o + 0x38, 4, ent)
        A.wr(o + 0x48, 1, a[4])
        A.wr(o + 0x49, 1, (old49 & 0xC4) | (a[20] & 1) | 4)
        A.wr(o + 0x4A, 1, 0)
        A.wr(o + 0x34, 4, 0)
        events.append([0x59C910, W.tagv(this + 0x2C), a[16], a[17], 0])
        events.append([0x41B7C0, 0x64])
        if route1:
            W.wr(route1, 4, 0)
        A.wr(o + 0xC, 4, route1)
        events.append([0x41B7C0, 0x64])
        if route2:
            W.wr(route2, 4, 0)
        A.wr(o + 0x10, 4, route2)
        events.append([0x571B70, W.tagv(ent), W.tagv(this + 0x38)])
        ret = W.tagv(this)
        marks.append('flag_set' if a[20] & 1 else 'flag_clear')
        marks.append('route1_set' if route1 else 'route1_null')
        marks.append('route2_set' if route2 else 'route2_null')
        if old49 & 0xC0:
            marks.append('bits_kept')
    elif k == WROCLONE:
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x18, 4, a[7]); A.wr(o + 0x1C, 4, a[8])
        W.snap()
        events.append([0x61A5A0, 0x54])
        if a[9] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            mem = W.raw(1, a[10])
            events.append([0x655020, W.tagv(mem), a[4], W.tagv(this + 0x10), W.tagv(a[8])])
            W.wr(mem, 4, a[12])
            ret = W.tagv(a[11])
            marks.append('mem_set')
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
    elif k in (BUFTEX, BUFIDX):
        off_count, off_size, off_stack = (0x10, 0x14, 0x18) if k == BUFTEX else (0xC, 0x10, 0x14)
        destroy = fn in (18, 20)
        size0 = signed32(a[5])
        arr1 = W.raw(1, a[7]); arr2 = W.raw(1, a[7] + 0x40)
        stack = 0 if a[6] == 0 else (arr1 if a[6] == 1 else a[13])
        A.wr(o + off_count, 4, a[4]); A.wr(o + off_size, 4, a[5]); A.wr(o + off_stack, 4, stack)
        for j in range(8):
            B.wr(a[7] + 4 * j, 4, W.raw(2, a[8] + 8 * j))
            B.wr(a[7] + 0x40 + 4 * j, 4, W.raw(2, a[8] + 8 * (8 + j)))
        W.snap()
        marks.append('size_positive' if size0 > 0 else ('size_zero' if size0 == 0 else 'size_negative'))
        marks.append('stack_null' if a[6] == 0 else ('stack_arena' if a[6] == 1 else 'stack_raw'))
        mut = a[10]
        marks.append(['mut_none', 'mut_shrink', 'mut_negative', 'mut_swap', 'mut_swap_grow', 'mut_null', 'mut_raw'][mut])
        size = size0
        i = 0
        rel = 0
        if size > 0:
            while True:
                obj = B.rd(A.rd(o + off_stack, 4) - W.bases[1] - GUARD + 4 * i, 4)
                events.append([COM_RELEASE, W.tagv(obj)])
                if mut and rel == a[11]:
                    if mut == 1:
                        A.wr(o + off_size, 4, a[12])
                    elif mut == 2:
                        A.wr(o + off_size, 4, a[12] | 0x80000000)
                    elif mut == 3:
                        A.wr(o + off_stack, 4, arr2)
                    elif mut == 4:
                        A.wr(o + off_stack, 4, arr2); A.wr(o + off_size, 4, 8)
                    elif mut == 5:
                        A.wr(o + off_stack, 4, 0); A.wr(o + off_size, 4, 0)
                    else:
                        A.wr(o + off_stack, 4, a[13]); A.wr(o + off_size, 4, 0)
                rel += 1
                size = signed32(A.rd(o + off_size, 4))
                i += 1
                if not i < size:
                    break
        final_stack = A.rd(o + off_stack, 4)
        A.wr(o + off_size, 4, 0); A.wr(o + off_count, 4, 0)
        if destroy and final_stack:
            events.append([FREE, W.tagv(final_stack)])
            A.wr(o + off_stack, 4, 0)
    elif k == GROWFREE:
        data = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else a[8])
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, a[5]); A.wr(o + 8, 4, data)
        W.snap()
        marks.append('data_null' if data == 0 else ('data_arena' if a[6] == 1 else 'data_raw'))
        if data:
            events.append([FREE, W.tagv(data)])
            A.wr(o + 8, 4, 0)
        A.wr(o + 4, 4, 0)
    elif k == GROWRESIZE:
        data = 0 if a[6] == 0 else (W.raw(1, a[7]) if a[6] == 1 else a[11])
        A.wr(o, 4, a[4]); A.wr(o + 4, 4, a[5]); A.wr(o + 8, 4, data)
        fresh = 0 if a[10] == 0 else W.raw(2, a[9])
        W.snap()
        cap, count, new = signed32(a[4]), signed32(a[5]), signed32(a[8])
        if new == cap:
            marks.append('unchanged')
        else:
            events.append([NEW_OP, (a[8] << 2) & M32])
            i = 0
            if count <= 0:
                marks.append('no_copy_count')
                if count < 0:
                    marks.append('count_negative')
            elif new <= 0:
                marks.append('no_copy_cap')
                if new < 0:
                    marks.append('cap_negative')
            else:
                marks.append('copies')
                n = min(count, new)
                if new < count:
                    marks.append('shrinks')
                for i in range(n):
                    W.wr(fresh + 4 * i, 4, W.rd(data + 4 * i, 4))
            events.append([FREE, W.tagv(data)])
            if data == 0:
                marks.append('data_null_freed')
            A.wr(o + 8, 4, fresh)
            A.wr(o, 4, a[8])
    changes = {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}
    return {'events': events, 'changes': changes, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks, 'extra': extra}


def tag_eq(got, want):
    """igualdad con una tolerancia unica: el banco etiqueta como 0x7F000000 cualquier valor dentro de la pila del hilo (Tag); un dword crudo del fixture (0x400000..0xFFFFFF) puede caer dentro de ella por casualidad"""
    if isinstance(got, list) and isinstance(want, list):
        return len(got) == len(want) and all(tag_eq(a, b) for a, b in zip(got, want))
    return got == want or (got == STACK_TAG and isinstance(want, int) and 0x400000 <= want < 0x1000000)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if not tag_eq(o['events'], e['events']):
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
    if not tag_eq(o.get('ret'), e['ret']):
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    for key, val in e.get('extra', {}).items():
        if o.get(key) != val:
            return '%s %s != %s' % (key, o.get(key), val)
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo AL / EAX (valor devuelto, ya comprobado)"""
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
        if row.get('event') == 'begin':
            set_zone_snapshot(row.get('zone_region'))
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
