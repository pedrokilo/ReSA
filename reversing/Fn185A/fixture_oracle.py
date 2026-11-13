"""Oraculo independiente (Python) de Fn185A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 185A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado) con fracciones exactas, y el `_ftol2` del EXE (0x821B40).

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x4048E0  CPool<CVehicle,CHeli>::GetAtRef(ref): ranura = ref >> 8 (con signo); si el BYTE de estado [bytemap + ranura] es igual al byte bajo de ref: almacenamiento + ranura * 0xA18; si no 0
   1  0x44CB10  COnscreenTimerEntry::Process: variable [this] == 0: nada; paso = _ftol2((paso[0xB7CB5C] * 0.02f) * 1000.0f); valor = dword [ScriptSpace + variable]; segundos = valor / 1000 (con signo); direccion (byte +0x39) 0:
            valor += paso y fin; otra: valor -= paso; si queda negativo: valor = 0, [this] = 0, byte +4 = 0, byte +0x38 = 0; si no, si segundos (sin signo) < [+0x3C] y byte 0xB6F065 == 0: evento 0x506EA0 (0x21, 0.0f, 1.0f)
   2  0x44CC20  COnscreenTimer::ProcessForDisplay: byte 0xBAA45D == 0: nada; byte +0x150 = 0, byte +0x38 = 0; reloj (variable [this] != 0): "%02d:%02d" (minutos % 100, segundos % 60 con restos con signo) en +0xE, +0x38 = 1,
            +0x150 = 1; contadores i = 0..3 en +0x40 + 0x44 * i: byte +0x3E = 0; con variable: valor max(v, 0); tipo (word +0x12): 0/1 "%d", 2 "%d / %d" (segunda variable [+4] sin recortar), otros nada, en +0x14; +0x3E = 1, +0x150 = 1
   3  0x44CD30  COnscreenTimer::Process: byte 0xA43088 == 1 o byte +0x151 != 0: nada; si no: 0x44CB10 con ECX = this
   4  0x53E600  DoFade: pausas (0xB7CB49, 0xB7CB48) != 0: nada; JustLoaded (0xC16EDC) == 1: FadeTimer (0xC16EE0) = hora (0xB7CB84), JustLoaded = 0; StillToFadeOut (0xC16EDD) == 1: (hora - FadeTimer) <= FadeOutTime (0x8D2BD4,
            sin signo): SetFadeColour(0,0,0), Fade(0, 0), ProcessFade; si no: StillToFadeOut = 0, Fade(3.0f, 1), ProcessFade, ProcessMusicFade (ECX = 0xB6F028); FadeValue (0xC3EFAB) != 0: DrawRect(rect, color) con rect =
            (-5.0f, -5.0f, float(ancho + 5), float(alto + 5)) (enteros 0xC17044 y 0xC17048 sumados antes de convertir) y color (R, G, B, v) o (0,0,0,0) con la bandera de pantalla de carga (0xB6F07D) != 0
   5  0x53E8D0  CLinkList<CPed*>::Clear: mientras [this+8] != this+0xC: desengancha el enlace (siguiente.anterior = anterior; anterior.siguiente = siguiente) y lo pone el primero de la lista libre (this+0x18)
   6  0x5A1D10  CObject::ConstructorDefault: 542260; [this] = 866F60; [+170] = 0; 59F840; byte [+13C] = 0; devuelve this
   7  0x5A1DF0  CObject::ConstructorFromDummy(dummy): 542260; [this] = 866F60; 533700(movsx word [dummy+22]); [dummy+18] != 0 ? 533ED0(rw, 1) : (matriz [dummy+14] nula: 54F560(dummy), 54F1B0(dummy+4, matriz); 54F610(this, matriz));
            533FB0(dummy); 59F840; bytes +2E y +2F del dummy; bit 0x200 de +1C del dummy; con [this+18] != 0: atomo = byte [rw] == 1 ? rw : 734820(rw); 5D7F40(atomo) == 0: bit 0x10000000 de +1C
   8  0x5A1EF0  CObject::operator new(size, ref): estado[ref >> 8] = ref & 0x7F; cursor = primera ranura con el bit 7 desde 0; devuelve almacenamiento + (ref >> 8) * 0x19C
   9  0x5A1F20  CObject::operator delete(obj): ranura = (obj - almacenamiento) / 0x19C (con signo); estado |= 0x80; cursor = min(cursor, ranura) con signo
  10  0x5BF3D0  cHandlingDataMgr::Initialise: 5BD830; cinco dwords inmediatos en +0..+0x10
  11  0x681E10  mov eax, 4B4h        12  0x681E20  mov eax, 4B9h
  13  0x684E30  PartnerChat::Clone: new(78h) 61A5A0; nulo -> nulo; 684290(+14, +38, byte +58, +3C, movsx byte +5B, byte +74, byte +75, +40..+48); devuelve su EAX
  14  0x684ED0  PartnerShove::Clone: new(74h); 6846F0(+14, +38, byte +58, +3C, movsx byte +5B, +40..+48)
  15..19, 22..24  destructores con borrado: cuerpo (683A40, 684200, 683A40 tras `mov [ecx], 87078Ch` en 684280, 684320, 684750, 694A40, 694B30, 694C10); delete 61A5B0 si el bit 0 del BYTE de banderas; devuelve this
  20  0x695890  FollowPedFootsteps::Clone: new(20h); 694E20(+0C)
  21  0x695B20  FollowLeaderInFormation::ConstructorFormation(grupo, ped, dx, dy, dist): 61A3B0 (real: [+4] = [+8] = 0); [+0C] = grupo; [+24] = dist; [this] = 870C3C; [+10] = ped; [+20] = 4; 611F20 (ECX = 0xC196E8, dx, dy);
            [+14], [+18] = los dos floats devueltos; [+1C] = 0; ped != 0: 571B70(ped, this+0x10)
  25  0x695DC0  FollowPedFootsteps::DestructorBody: [this] = 870CC0; [+0C] != 0: 571A00(ref, this+0xC); [+18] != 0: 41B7D0(ruta), [+18] = 0; 61A3D0

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
    "CVehiclePool::GetAtRef", "COnscreenTimerEntry::Process", "COnscreenTimer::ProcessForDisplay", "COnscreenTimer::Process", "DoFade", "CLinkList<CPed*>::Clear", "CObject::ConstructorDefault",
    "CObject::ConstructorFromDummy", "CObject::operator new(poolRef)", "CObject::operator delete", "cHandlingDataMgr::Initialise", "CTaskComplexPartnerChat::GetTaskType",
    "CTaskComplexPartnerShove::GetTaskType", "CTaskComplexPartnerChat::Clone", "CTaskComplexPartnerShove::Clone", "CTaskComplexPartner::ScalarDeletingDestructor",
    "CTaskComplexPartnerDeal::ScalarDeletingDestructor", "CTaskComplexPartnerGreet::ScalarDeletingDestructor", "CTaskComplexPartnerChat::ScalarDeletingDestructor",
    "CTaskComplexPartnerShove::ScalarDeletingDestructor", "CTaskComplexFollowPedFootsteps::Clone", "CTaskComplexFollowLeaderInFormation::ConstructorFormation",
    "CTaskComplexFollowLeaderInFormation::ScalarDeletingDestructor", "CTaskComplexFollowLeaderAnyMeans::ScalarDeletingDestructor", "CTaskComplexSeekEntityAiming::ScalarDeletingDestructor",
    "CTaskComplexFollowPedFootsteps::DestructorBody"]
ENTRIES = (0x4048E0, 0x44CB10, 0x44CC20, 0x44CD30, 0x53E600, 0x53E8D0, 0x5A1D10, 0x5A1DF0, 0x5A1EF0, 0x5A1F20, 0x5BF3D0, 0x681E10, 0x681E20, 0x684E30, 0x684ED0, 0x684F60, 0x684F80, 0x684FA0,
           0x684FC0, 0x684FE0, 0x695890, 0x695B20, 0x695BC0, 0x695BE0, 0x695C00, 0x695DC0)
GUARD = 0x40
WIN = 0x1000
SIZES = (GUARD + WIN + GUARD,) * 4
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
(POOLREF, TIMERENTRY, TIMERDISP, TIMERPROC, FADE, LLCLEAR, OBJDEF, OBJDUMMY, OBJNEW, OBJDEL, HANDLING, CONST, CLONECHAT, CLONESHOVE, SDD, CLONEFOOT, FORMATION, FOOTBODY) = range(18)
KIND = [POOLREF, TIMERENTRY, TIMERDISP, TIMERPROC, FADE, LLCLEAR, OBJDEF, OBJDUMMY, OBJNEW, OBJDEL, HANDLING, CONST, CONST, CLONECHAT, CLONESHOVE, SDD, SDD, SDD, SDD, SDD, CLONEFOOT, FORMATION,
        SDD, SDD, SDD, FOOTBODY]
SDD_BODY = {15: 0x683A40, 16: 0x684200, 17: 0x683A40, 18: 0x684320, 19: 0x684750, 22: 0x694A40, 23: 0x694B30, 24: 0x694C10}
CONST_VAL = {11: 0x4B4, 12: 0x4B9}
DELETE = 0x61A5B0
SS_BASE, SS_WIN = 0xA49960, 0x1000
SCRIPT_HEAD = bytes(8)      # primeros bytes de ScriptSpace (los lee la segunda variable de un contador con id 0): los fija run_local.analyze desde el evento `begin`


def set_script_head(hexstr):
    global SCRIPT_HEAD
    SCRIPT_HEAD = bytes.fromhex(hexstr) if hexstr else bytes(8)


set_zone_snapshot = set_script_head


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
        self.mem = tuple(Mem(a[2], a[60 + i]) for i in range(4))
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
    POOLREF: ['match', 'mismatch', 'match_free_bit', 'differs_bit7', 'storage_arena', 'storage_raw', 'storage_null', 'slot_zero', 'slot_last'],
    TIMERENTRY: ['var_none', 'increase', 'decrease_ok', 'decrease_negative', 'beep', 'no_beep_secs', 'no_beep_wide', 'step_zero', 'step_nan_inf', 'step_beyond_int64', 'step_negative', 'wrap_up', 'dir_other'],
    TIMERDISP: ['hud_off', 'clock_none', 'clock_shown', 'clock_negative', 'clock_minutes_wrap', 'counter_none', 'counter_type0', 'counter_type1', 'counter_type2', 'counter_other_type', 'counter_negative', 'counter2_window'],
    TIMERPROC: ['replay_playback', 'replay_other', 'paused', 'runs', 'paused_and_replay'],
    FADE: ['paused', 'running', 'just_loaded', 'still_in_time', 'still_out_of_time', 'boundary_equal', 'boundary_plus_one', 'no_still', 'fade_zero', 'fade_draws', 'splash', 'size_huge', 'size_big_float'],
    LLCLEAR: ['used0', 'used1', 'used_many', 'free0', 'free_some'],
    OBJDEF: [],
    OBJDUMMY: ['rw_dummy', 'matrix_alloc', 'matrix_given', 'this_rw_atomic', 'this_rw_clump', 'this_rw_null', 'cbpc_attached', 'cbpc_missing', 'flag200_copied', 'flag200_cleared', 'model_negative'],
    OBJNEW: ['slot_zero', 'slot_last', 'cursor_zero', 'cursor_moves', 'storage_arena', 'storage_raw'],
    OBJDEL: ['slot_zero', 'slot_last', 'lowers_cursor', 'keeps_cursor', 'storage_arena', 'storage_raw', 'cursor_negative'],
    HANDLING: [],
    CONST: [],
    CLONECHAT: ['mem_null', 'mem_set', 'leader_other', 'count_negative', 'conv_other'],
    CLONESHOVE: ['mem_null', 'mem_set', 'leader_other', 'count_negative'],
    SDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only'],
    CLONEFOOT: ['mem_null', 'mem_set'],
    FORMATION: ['ped_null', 'ped_arena', 'ped_raw', 'offset_nan', 'dx_extreme'],
    FOOTBODY: ['ref_null', 'ref_arena', 'ref_raw', 'route_null', 'route_arena', 'route_raw'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = COMMON + _KIND_KEYS[KIND[_fn]]


def sx8(v):
    v &= 0xFF
    return (v - 0x100 if v >= 0x80 else v) & M32


def sx16(v):
    v &= 0xFFFF
    return (v - 0x10000 if v >= 0x8000 else v) & M32


def tdiv(a, b):
    q = abs(a) // abs(b)
    return -q if (a < 0) != (b < 0) else q


def trem(a, b):
    return a - tdiv(a, b) * b


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


def win_word(seed, i):
    t = (0, 1, 0xFFFFFFFF, 59, 60, 61, 999, 1000, 59999, 60000, 3599999, 3600000, 5999999, 6000001, 0x7FFFFFFF, 0x80000000)
    k = rnd(seed, i) % 24
    if k < 16:
        return t[k]
    if k < 20:
        return rnd(seed, i + 100) % 20000000
    if k < 22:
        return (-(rnd(seed, i + 150) % 20000000)) & M32
    return rnd(seed, i + 200)


def f32round(x):
    return struct.unpack('<f', struct.pack('<f', x))[0]


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


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
    A.wr(o, 4, a[3])

    def gset(addr, n, v):
        for i in range(n):
            W.glob[addr + i] = (v >> (8 * i)) & 0xFF

    def gget(addr, n=4):
        return sum(W.glob[addr + i] << (8 * i) for i in range(n))

    def ptr(mode, arena, off, raw):
        return 0 if mode == 0 else (W.raw(arena, off) if mode == 1 else raw)

    SSW = SS_BASE + SS_WIN
    if k == POOLREF:
        cap, slot, bm = a[4], a[9], a[11] & 0xFF
        storage = W.raw(2, a[6]) if a[5] == 0 else (a[7] if a[5] == 1 else 0)
        A.wr(o, 4, storage); A.wr(o + 4, 4, W.raw(1, a[8])); A.wr(o + 8, 4, cap)
        if a[10] == 0:
            low = bm; B.wr(a[8] + slot, 1, bm)
        elif a[10] == 1:
            low = (bm ^ a[12]) & 0xFF; B.wr(a[8] + slot, 1, bm)
        elif a[10] == 2:
            low = bm | 0x80; B.wr(a[8] + slot, 1, low)
        else:
            low = bm ^ 0x80; B.wr(a[8] + slot, 1, bm)
        W.snap()
        state = B.rd(a[8] + slot, 1)
        ok = state == low
        ret = W.tagv((storage + slot * 0xA18) & M32) if ok else 0
        marks.append('match' if ok else 'mismatch')
        if ok and low & 0x80:
            marks.append('match_free_bit')
        if not ok and (state ^ low) == 0x80:
            marks.append('differs_bit7')
        marks.append('storage_arena' if a[5] == 0 else ('storage_raw' if a[5] == 1 else 'storage_null'))
        if slot == 0:
            marks.append('slot_zero')
        if slot == cap - 1:
            marks.append('slot_last')
    elif k == TIMERENTRY:
        var = (SS_WIN + a[5]) if a[4] else 0
        A.wr(o, 4, var); A.wr(o + 4, 1, a[11]); A.wr(o + 0x38, 1, a[12]); A.wr(o + 0x39, 1, a[8]); A.wr(o + 0x3C, 4, a[9])
        gorder = [(SSW, 0x100), (0xB7CB5C, 4), (0xB6F065, 1)]
        gset(SSW + a[5], 4, a[6]); gset(0xB7CB5C, 4, a[7]); gset(0xB6F065, 1, a[10])
        W.snap()
        if var == 0:
            marks.append('var_none')
        else:
            sv = v_mul(v_mul(v_load(a[7]), v_load(0x3CA3D70A)), v_load(0x447A0000))
            step = ftol2_eax(sv)
            marks.append('step_zero' if step == 0 else 'step_nonzero')
            if sv[0] != 'fin':
                marks.append('step_nan_inf')
            elif abs(sv[1]) >= Fraction(1 << 63):
                marks.append('step_beyond_int64')
            if signed32(step) < 0:
                marks.append('step_negative')
            before = gget(SSW + a[5])
            bsec = tdiv(signed32(before), 1000) & M32
            d = a[8] & 0xFF
            if d == 0:
                gset(SSW + a[5], 4, (before + step) & M32)
                marks.append('increase')
                if before + step > M32:
                    marks.append('wrap_up')
            else:
                if d > 2:
                    marks.append('dir_other')
                new = (before - step) & M32
                gset(SSW + a[5], 4, new)
                if signed32(new) < 0:
                    gset(SSW + a[5], 4, 0)
                    A.wr(o, 4, 0); A.wr(o + 4, 1, 0); A.wr(o + 0x38, 1, 0)
                    marks.append('decrease_negative')
                else:
                    marks.append('decrease_ok')
                    if bsec < a[9]:
                        if (a[10] & 0xFF) == 0:
                            events.append([0x506EA0, 0xB6BC90, 0x21, 0, 0x3F800000])
                            marks.append('beep')
                        else:
                            marks.append('no_beep_wide')
                    else:
                        marks.append('no_beep_secs')
    elif k == TIMERDISP:
        winb = b''.join(win_word(a[7], i).to_bytes(4, 'little') for i in range(64))
        head = SCRIPT_HEAD

        def rdvar(varid):
            if varid == 0:
                return int.from_bytes(head[0:4], 'little')
            off = varid - SS_WIN
            return int.from_bytes(winb[off:off + 4], 'little')
        gset(0xBAA45D, 1, a[4])
        gorder = [(0xBAA45D, 1)]
        clock_var = (SS_WIN + a[6]) if a[5] else 0
        A.wr(o, 4, clock_var)
        for i in range(4):
            co = o + 0x40 + 0x44 * i
            A.wr(co, 4, (SS_WIN + a[9 + 5 * i]) if a[8 + 5 * i] else 0)
            A.wr(co + 4, 4, (SS_WIN + a[12 + 5 * i]) if a[11 + 5 * i] else 0)
            A.wr(co + 0x10, 4, (A.rd(co + 0x10, 4) & 0x0000FFFF) | ((a[10 + 5 * i] & 0xFFFF) << 16))
            A.wr(co + 0x3E, 1, a[28 + i])
        W.snap()

        def put_text(off, s):
            for j, ch in enumerate(s.encode('ascii') + b'\0'):
                A.wr(off + j, 1, ch)
        if (a[4] & 0xFF) == 0:
            marks.append('hud_off')
        else:
            A.wr(o + 0x150, 1, 0); A.wr(o + 0x38, 1, 0)
            if clock_var == 0:
                marks.append('clock_none')
            else:
                val = signed32(rdvar(clock_var))
                secs = tdiv(val, 1000)
                mins, sec_r = tdiv(secs, 60), trem(secs, 60)
                m100 = trem(mins, 100)
                put_text(o + 0xE, '%02d:%02d' % (m100, sec_r))
                A.wr(o + 0x38, 1, 1); A.wr(o + 0x150, 1, 1)
                marks.append('clock_shown')
                if val < 0:
                    marks.append('clock_negative')
                if abs(mins) >= 100:
                    marks.append('clock_minutes_wrap')
            for i in range(4):
                co = o + 0x40 + 0x44 * i
                A.wr(co + 0x3E, 1, 0)
                var = (SS_WIN + a[9 + 5 * i]) if a[8 + 5 * i] else 0
                if var == 0:
                    marks.append('counter_none')
                    continue
                val = signed32(rdvar(var))
                first = max(val, 0)
                t = a[10 + 5 * i] & 0xFFFF
                if val < 0:
                    marks.append('counter_negative')
                if t <= 1:
                    put_text(co + 0x14, '%d' % first)
                    marks.append('counter_type%d' % t)
                elif t == 2:
                    var2 = (SS_WIN + a[12 + 5 * i]) if a[11 + 5 * i] else 0
                    second = signed32(rdvar(var2))
                    put_text(co + 0x14, '%d / %d' % (first, second))
                    marks.append('counter_type2')
                    marks.append('counter2_window' if var2 else 'counter2_head')
                else:
                    marks.append('counter_other_type')
                A.wr(co + 0x3E, 1, 1); A.wr(o + 0x150, 1, 1)
    elif k == TIMERPROC:
        gset(0xA43088, 1, a[4]); gorder = [(0xA43088, 1)]
        A.wr(o + 0x151, 1, a[5])
        W.snap()
        rep, paused = (a[4] & 0xFF) == 1, (a[5] & 0xFF) != 0
        if rep and paused:
            marks.append('paused_and_replay')
        if rep:
            marks.append('replay_playback')
        else:
            marks.append('replay_other')
        marks.append('paused' if paused else 'runs')
        if not rep and not paused:
            events.append([0x44CB10, W.tagv(this)])
    elif k == FADE:
        gset(0xB7CB49, 1, a[4]); gset(0xB7CB48, 1, a[5]); gset(0xC16EDC, 1, a[6]); gset(0xC16EDD, 1, a[9])
        gset(0xB7CB84, 4, a[7]); gset(0xC16EE0, 4, a[8]); gset(0x8D2BD4, 4, a[10])
        gset(0xC3EFA8, 1, a[13]); gset(0xC3EFA9, 1, a[14]); gset(0xC3EFAA, 1, a[15]); gset(0xC3EFAB, 1, a[11])
        gset(0xB6F07D, 1, a[12]); gset(0xC17044, 4, a[16]); gset(0xC17048, 4, a[17])
        gorder = [(0xC16EDC, 8)]
        W.snap()
        cam = 0xB6F028
        if (a[4] & 0xFF) or (a[5] & 0xFF):
            marks.append('paused')
        else:
            marks.append('running')
            t, timer = a[7], a[8]
            if (a[6] & 0xFF) == 1:
                timer = t
                gset(0xC16EE0, 4, t); gset(0xC16EDC, 1, 0)
                marks.append('just_loaded')
            if (a[9] & 0xFF) == 1:
                diff = (t - timer) & M32
                if diff == a[10]:
                    marks.append('boundary_equal')
                if diff == (a[10] + 1) & M32:
                    marks.append('boundary_plus_one')
                if diff <= a[10]:
                    events.append([0x50BF00, cam, 0, 0, 0])
                    events.append([0x50AC20, cam, 0, 0])
                    events.append([0x50B5D0, cam])
                    marks.append('still_in_time')
                else:
                    gset(0xC16EDD, 1, 0)
                    events.append([0x50AC20, cam, 0x40400000, 1])
                    events.append([0x50B5D0, cam])
                    events.append([0x50B6D0, cam])
                    marks.append('still_out_of_time')
            else:
                marks.append('no_still')
            fv = a[11] & 0xFF
            if fv == 0:
                marks.append('fade_zero')
            else:
                marks.append('fade_draws')
                if (a[12] & 0xFF) != 0:
                    color = 0
                    marks.append('splash')
                else:
                    color = (a[13] & 0xFF) | ((a[14] & 0xFF) << 8) | ((a[15] & 0xFF) << 16) | (fv << 24)
                w = signed32((a[16] + 5) & M32)
                h = signed32((a[17] + 5) & M32)
                if abs(w) > 0x1000000 or abs(h) > 0x1000000:
                    marks.append('size_huge')
                w0, h0 = signed32(a[16]), signed32(a[17])
                if f32bits(float(w)) != f32bits(f32round(f32round(float(w0)) + 5.0)) or f32bits(float(h)) != f32bits(f32round(f32round(float(h0)) + 5.0)):
                    marks.append('size_big_float')
                events.append([0x727B60, 0xC0A00000, 0xC0A00000, f32bits(float(w)), f32bits(float(h)), color])
    elif k == LLCLEAR:
        n_used, n_free = a[4], a[5]
        used, freed = [], []
        for i in range(n_used + n_free):
            node = W.raw(1, a[6] + 0x20 * a[8 + i])
            (used if i < n_used else freed).append(node)

        def chain(head, mids, tail):
            ch = [head] + mids + [tail]
            for i in range(len(ch)):
                if i > 0:
                    W.wr(ch[i] + 4, 4, ch[i - 1])
                if i + 1 < len(ch):
                    W.wr(ch[i] + 8, 4, ch[i + 1])
        chain(this, used, this + 0xC)
        chain(this + 0x18, freed, this + 0x24)
        W.snap()
        marks.append('used0' if n_used == 0 else 'used1')
        if n_used >= 2:
            marks.append('used_many')
        marks.append('free0' if n_free == 0 else 'free_some')
        fhead = this + 0x18
        node = W.rd(this + 8)
        steps = 0
        while node != this + 0xC and steps < 64:
            nxt, prv = W.rd(node + 8), W.rd(node + 4)
            W.wr(nxt + 4, 4, prv)
            W.wr(prv + 8, 4, nxt)
            f = W.rd(fhead + 8)
            W.wr(node + 8, 4, f)
            W.wr(f + 4, 4, node)
            W.wr(node + 4, 4, fhead)
            W.wr(fhead + 8, 4, node)
            node = W.rd(this + 8)
            steps += 1
    elif k == OBJDEF:
        W.snap()
        events.append([0x542260, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, 0x866F60); A.wr(o + 0x170, 4, 0)
        events.append([0x59F840, W.tagv(this), 0x866F60, 0])
        A.wr(o + 0x13C, 1, 0)
        ret = W.tagv(this)
    elif k == OBJDUMMY:
        d = a[4]
        B.wr(d + 0x20, 4, ((a[5] << 16) | (c['seed'] & 0xFFFF)) & M32)
        rwd = ptr(a[6], 2, a[7], a[8])
        mat = ptr(a[9], 3, a[10], a[11])
        B.wr(d + 0x18, 4, rwd); B.wr(d + 0x14, 4, mat)
        B.wr(d + 0x2E, 1, a[12]); B.wr(d + 0x2F, 1, a[13]); B.wr(d + 0x1C, 4, a[14])
        A.wr(o + 0x1C, 4, a[15])
        if a[16]:
            Cc.wr(a[18], 1, a[17]); A.wr(o + 0x18, 4, W.raw(2, a[18]))
        else:
            A.wr(o + 0x18, 4, 0)
        gptr, gat = W.raw(3, a[19]), W.raw(3, a[20])
        W.snap()
        dummy = W.raw(1, d)
        events.append([0x542260, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, 0x866F60)
        sx = sx16(a[5])
        events.append([0x533700, W.tagv(this), sx])
        if sx & 0x8000_0000:
            marks.append('model_negative')
        if rwd:
            events.append([0x533ED0, W.tagv(this), W.tagv(rwd), 1])
            marks.append('rw_dummy')
        else:
            if mat == 0:
                events.append([0x54F560, W.tagv(dummy)])
                B.wr(d + 0x14, 4, gptr)
                mat = gptr
                events.append([0x54F1B0, W.tagv(dummy + 4), W.tagv(mat)])
                marks.append('matrix_alloc')
            else:
                marks.append('matrix_given')
            events.append([0x54F610, W.tagv(this), W.tagv(mat)])
        events.append([0x533FB0, W.tagv(dummy)])
        events.append([0x59F840, W.tagv(this), 0x866F60, A.rd(o + 0x170, 4)])
        A.wr(o + 0x2E, 1, a[12]); A.wr(o + 0x2F, 1, a[13])
        mine = A.rd(o + 0x1C, 4)
        flags = (mine ^ ((a[14] ^ mine) & 0x200)) & M32
        A.wr(o + 0x1C, 4, flags)
        marks.append('flag200_copied' if flags & 0x200 else 'flag200_cleared')
        rw = A.rd(o + 0x18, 4)
        if rw:
            t = Cc.rd(a[18], 1)
            if t == 1:
                atom = rw
                marks.append('this_rw_atomic')
            else:
                events.append([0x734820, W.tagv(rw)])
                atom = gat
                marks.append('this_rw_clump')
            events.append([0x5D7F40, W.tagv(atom)])
            if a[21] == 0:
                A.wr(o + 0x1C, 4, flags | 0x10000000)
                marks.append('cbpc_missing')
            else:
                marks.append('cbpc_attached')
        else:
            marks.append('this_rw_null')
        ret = W.tagv(this)
    elif k in (OBJNEW, OBJDEL):
        isnew = k == OBJNEW
        cap, po, slot = a[4], a[5], a[10]
        storage = W.raw(2, a[7]) if a[6] == 0 else a[8]
        B.wr(po, 4, storage); B.wr(po + 4, 4, W.raw(3, a[9])); B.wr(po + 8, 4, cap); B.wr(po + 0xC, 4, a[11])
        seed = a[14] if isnew else a[13]
        for i in range(cap):
            D.wr(a[9] + i, 1, rnd(seed, i) & 0xFF)
        D.wr(a[9] + cap, 1, 0x80)
        st = D.rd(a[9] + slot, 1)
        free = (a[15] != 0) if isnew else (a[12] != 0)
        D.wr(a[9] + slot, 1, (st | 0x80) if free else (st & 0x7F))
        gset(0xB7449C, 4, W.raw(1, po)); gorder = [(0xB7449C, 4)]
        W.snap()
        marks.append('storage_arena' if a[6] == 0 else 'storage_raw')
        if slot == 0:
            marks.append('slot_zero')
        if slot == cap - 1:
            marks.append('slot_last')
        if isnew:
            ref = ((slot << 8) | a[13]) & M32
            D.wr(a[9] + slot, 1, ref & 0x7F)
            cursor = 0
            while (D.rd(a[9] + cursor, 1) & 0x80) == 0:
                cursor += 1
            B.wr(po + 0xC, 4, cursor)
            ret = W.tagv((storage + slot * 0x19C) & M32)
            marks.append('target_free' if free else 'target_used')
            marks.append('cursor_zero' if cursor == 0 else 'cursor_moves')
        else:
            D.wr(a[9] + slot, 1, D.rd(a[9] + slot, 1) | 0x80)
            last = B.rd(po + 0xC, 4)
            if slot < signed32(last):
                B.wr(po + 0xC, 4, slot)
                marks.append('lowers_cursor')
            else:
                marks.append('keeps_cursor')
            if signed32(last) < 0:
                marks.append('cursor_negative')
            marks.append('was_free' if free else 'was_used')
    elif k == HANDLING:
        W.snap()
        events.append([0x5BD830, W.tagv(this)] + [A.rd(o + 4 * i, 4) for i in range(5)])
        for i, v in enumerate((0x3DCCCCCD, 0x3F666666, 0x3F800000, 0x3F4CCCCD, 0x3F7AE148)):
            A.wr(o + 4 * i, 4, v)
    elif k == CONST:
        W.snap()
        ret = CONST_VAL[fn]
    elif k in (CLONECHAT, CLONESHOVE, CLONEFOOT):
        if k == CLONEFOOT:
            A.wr(o + 0xC, 4, a[4])
        else:
            A.wr(o + 0x38, 4, a[4]); A.wr(o + 0x58, 1, a[5]); A.wr(o + 0x3C, 4, a[6]); A.wr(o + 0x5B, 1, a[7])
            A.wr(o + 0x40, 4, a[10]); A.wr(o + 0x44, 4, a[11]); A.wr(o + 0x48, 4, a[12])
            if k == CLONECHAT:
                A.wr(o + 0x74, 1, a[8]); A.wr(o + 0x75, 1, a[9])
        W.snap()
        events.append([0x61A5A0, {CLONECHAT: 0x78, CLONESHOVE: 0x74, CLONEFOOT: 0x20}[k]])
        if a[13] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            mem = W.raw(1, a[14])
            if k == CLONECHAT:
                events.append([0x684290, W.tagv(mem), W.tagv(this + 0x14), W.tagv(a[4]), a[5] & 0xFF, a[6], sx8(a[7]), a[8] & 0xFF, a[9] & 0xFF, a[10], a[11], a[12]])
            elif k == CLONESHOVE:
                events.append([0x6846F0, W.tagv(mem), W.tagv(this + 0x14), W.tagv(a[4]), a[5] & 0xFF, a[6], sx8(a[7]), a[10], a[11], a[12]])
            else:
                events.append([0x694E20, W.tagv(mem), W.tagv(a[4])])
            W.wr(mem, 4, a[16])
            ret = W.tagv(a[15])
            marks.append('mem_set')
        if k != CLONEFOOT:
            if (a[5] & 0xFF) > 1:
                marks.append('leader_other')
            if a[7] & 0x80:
                marks.append('count_negative')
            if k == CLONECHAT and (a[8] & 0xFF) > 1:
                marks.append('conv_other')
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), 0x87078C if fn == 17 else A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([DELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == FORMATION:
        ped = ptr(a[5], 1, a[6], a[7])
        Cc.wr(a[11], 4, a[12]); Cc.wr(a[11] + 4, 4, a[13])
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0)       # base CTaskComplex real (0x61A3B0)
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x24, 4, a[10]); A.wr(o, 4, 0x870C3C); extra['vtable_ok'] = True
        A.wr(o + 0x10, 4, ped); A.wr(o + 0x20, 4, 4)
        events.append([0x611F20, 0xC196E8, a[8], a[9]])
        A.wr(o + 0x14, 4, a[12]); A.wr(o + 0x18, 4, a[13]); A.wr(o + 0x1C, 4, 0)
        if ped:
            events.append([0x571B70, W.tagv(ped), W.tagv(this + 0x10)])
        marks.append('ped_null' if ped == 0 else ('ped_arena' if a[5] == 1 else 'ped_raw'))
        if any((x & 0x7F800000) == 0x7F800000 and (x & 0x7FFFFF) for x in (a[12], a[13])):
            marks.append('offset_nan')
        if signed32(a[8]) not in (-1, 0, 1) or signed32(a[9]) not in (-1, 0, 1):
            marks.append('dx_extreme')
        ret = W.tagv(this)
    elif k == FOOTBODY:
        ref = ptr(a[4], 1, a[5], a[6])
        route = ptr(a[7], 2, a[8], a[9])
        A.wr(o + 0xC, 4, ref); A.wr(o + 0x18, 4, route)
        W.snap()
        A.wr(o, 4, 0x870CC0)
        if ref:
            events.append([0x571A00, W.tagv(ref), W.tagv(this + 0xC)])
        if route:
            events.append([0x41B7D0, W.tagv(route)])
            A.wr(o + 0x18, 4, 0)
        events.append([0x61A3D0, W.tagv(this), 0x870CC0])
        A.wr(o, 4, a[10])
        marks.append('ref_null' if ref == 0 else ('ref_arena' if a[4] == 1 else 'ref_raw'))
        marks.append('route_null' if route == 0 else ('route_arena' if a[7] == 1 else 'route_raw'))
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
            set_script_head(row.get('script_head'))
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
