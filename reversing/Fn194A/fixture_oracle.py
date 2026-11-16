"""Oraculo independiente (Python) de Fn194A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 33 funciones del lote 194A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x683440  CTaskComplexWaitForPed::CreateNextSubTask: xor eax,eax; ret 4
   1  0x683450  CreateFirstSubTask: [this+0Ch] nulo -> eax = 0; si no [this+1Ch] = [0B7CB84h], [this+20h] = [this+14h], byte [this+24h] = 1, dword [this+28h] = 0; 61A5A0(0x20); nulo -> 0; si no 62F310 (ecx = nueva; 999999 (0F423Fh), 0, 0, 8.0f);
            eax = el suyo
   2  0x6834E0  ControlSubTask(ped) `ret 4`: [this+0Ch] nulo -> 0; 5E0170 (ecx = esperado): AL == 0 -> CEventDeadPed local (4ADEA0: esperado, 0, [0B7CB84h]; +0Eh = 258h), 4AB420 (ecx = [ped+47Ch] + 68h; evento, 0), 4ADF70; 0.
            Si byte [this+24h]: 420E30 (ecx = this+1Ch): AL != 0 -> 0. 600EE0 (ecx = [esperado+47Ch], 4B8h); con tarea: vector [tarea+14h] por la matriz [ped+14h] (59C890 (salida, matriz, vector), sin comprobar nulo)
            menos la posicion del esperado (matriz [esperado+14h] ? matriz+30h : esperado+4); sin tarea: posicion del ped menos la del esperado (x, y, z con fld/fsub); T = ((dz*dz + dy*dy) + dx*dx) en la x87 (24 bits);
            fcompp con radio*radio ([this+10h]): T < r2 -> 0 (NaN sigue). Si byte [this+18h] y dword [this+28h] == 0: 40FE60 (salida, posicion del esperado, posicion del ped) (floats), 53CBE0 (x, y, 0, 0) (float en ST0 que
            pasa por una variable de pila), 53CB50, fstp [ped+55Ch]. contador = [this+28h] + 1; si > 10 (con signo) 0. eax = [this+8]
   3  0x669FB0  CanJoinGang: [this+30h] == 0 -> 0; [this+31h] != 0 -> [this+28h] = [0B7CB84h], [this+31h] = 0; AL = ([0B7CB84h] >= [this+28h] + [this+2Ch]) sin signo
   4  0x669FF0  GetDistSqOfClosestPathNodeToPed(ped) `ret 4`: float en ST0. siguiente (+1Ch area, +1Eh id): area == 0FFFFh o [096F854h + area*4] == 0 -> 999999.0f (0x86FEC0), si no 420A10 (nodo = tabla[area] + id*1Ch;
            x = word [+8]*0.125, y = word [+0Ah], z = word [+0Ch]); d = nodo - ped (ped: matriz [ped+14h] ? +30h : ped+4); T = (dz*dz + dy*dy) + dx*dx; se guarda como float. anterior (+18h, +1Ah) igual sin guardar; fcom [siguiente]: si
            ST0 < siguiente (ordenado) devuelve el anterior, si no recarga el siguiente
   5  0x66F4A0  CTaskComplexWander destructor con borrado: push esi; esi = ecx; call 669D20; si byte [esp+8] & 1: push esi; call 61A5B0; eax = esi; ret 4
   6  0x673EF0  GetWanderTaskByPedTypeId(tipo) cdecl: idx = tipo - 6 (sin signo); tabla de bytes 6740C4: 0 -> Cop (0x50, 460C80 (4, dir)), 12 -> Medic (0x28, 658770 (4, dir, 1)), 14 -> Criminal (0x38, 48E610), 16 -> Prostitute
            (0x3C, 672690), cualquier otro -> Standard (0x38, 48E4F0 (4, dir, 1)); 61A5A0(size); nulo -> 0; direccion = 407180(0, 8) (rand 821B1E & 0FFFFh) o en linea rand & 0FFFFh * (1/32768) * 8 truncado; constructor; eax = el suyo
   7..15 constructores de almacenes: push esi; esi = ecx; 821D30 (esi+4, tamano, cuenta, ctor de elemento, destructor); [esi] = 0; eax = esi. Cada elemento: 4C4A60 ([+0] = 85BB9Ch, word [+8] = 0, word [+0Ah] = 0FFFFh) y su constructor
   16 0x4C6200  mov eax,[ecx]; jmp [eax+1Ch]
   17..24 destructores con borrado de model infos: push esi; esi = ecx; call cuerpo; si byte [esp+8] & 1: push esi; call 8214BD; eax = esi; ret 4
   25 0x4C62F0  cuerpo de ~CPedModelInfo: [this] = 85BDC0h; edi = [this+34h]; si != 0: 40F700 (ecx = edi), 40FC40 (edi); [this] = 85BB9Ch
   26 0x4C5790  [this] = 85BB9Ch
   27 0x4C6B10  FreeModelInfoIds: 8214BD([this]); [this] = 0
   28 0x4C6B70  GetModelInfoIdFile: 538900 (this+6, "rb"); byte [this+1Bh] = (handle > 0 con signo); 82119A(0A08Ch); [this] = eso; 0A08Ch bytes a cero; si [this+1Bh]: 538950 (handle, bloque, 0A08Ch), 5389D0 (handle); AL = [this+1Bh]
   29 0x4C6BD0  EndOfLoadPhase: si [this+1Bh] == 0: 538910 (this+6); handle > 0 (con signo): 538970 (handle, [this], 0A08Ch), 5389D0 (handle); 8214BD([this]); [this] = 0
   30 0x66CA40  constructor de CTaskComplexSeekEntity<FixedPos> (8 argumentos, `ret 20h`): 61A3B0; +0Ch..+1Ch = argumentos 1 a 5, +20h = 0, +24h = argumento 6, +28h/+2Ch/+34h/+38h = 0, bytes +30h/+31h/+3Ch/+3Dh = 0, [+0] = 87004Ch, [+40h] = 86FD18h,
            +44h/+48h/+4Ch = 0, +50h = 6, byte +54h = (viejo & 0F0h) | ((arg8 & 1) << 1) | (arg7 & 1) (BYTES bajos), 571B70 (ecx = entidad; arg &this+0Ch) si entidad != 0; eax = this
   31 0x66CB90  destructor con borrado: cuerpo 66CBB0; 61A5B0 si bit 0
   32 0x66CBB0  cuerpo: [this] = 87004Ch; 571A00 (ecx = [this+0Ch], &this+0Ch) si no nulo; [this+40h] = 859DC4h; 61A3D0 (sin efecto con subtarea nula). Las vtables (+0 y +40h) no se comparan (la DLL deja las suyas)

Los falsos anotan los eventos [direccion, ...] como se describe en LiveComparison.hpp (Tag: etiqueta de arena o valor crudo; los punteros a la pila del llamador, 0x7F000000).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexWaitForPed::CreateNextSubTask", "CTaskComplexWaitForPed::CreateFirstSubTask", "CTaskComplexWaitForPed::ControlSubTask", "CTaskComplexWanderGang::CanJoinGang",
         "CTaskComplexWander::GetDistSqOfClosestPathNodeToPed", "CTaskComplexWander::ScalarDeletingDestructor", "CTaskComplexWander::GetWanderTaskByPedTypeId",
         "CModelInfo::AtomicModelInfoStoreConstructor", "CModelInfo::DamageAtomicModelInfoStoreConstructor", "CModelInfo::LodAtomicModelInfoStoreConstructor", "CModelInfo::TimeModelInfoStoreConstructor",
         "CModelInfo::LodTimeModelInfoStoreConstructor", "CModelInfo::WeaponModelInfoStoreConstructor", "CModelInfo::ClumpModelInfoStoreConstructor", "CModelInfo::VehicleModelInfoStoreConstructor",
         "CModelInfo::PedModelInfoStoreConstructor", "CBaseModelInfo::ShutdownThunk", "CAtomicModelInfo::ScalarDeletingDestructor", "CDamageAtomicModelInfo::ScalarDeletingDestructor",
         "CLodAtomicModelInfo::ScalarDeletingDestructor", "CTimeModelInfo::ScalarDeletingDestructor", "CLodTimeModelInfo::ScalarDeletingDestructor", "CClumpModelInfo::ScalarDeletingDestructor",
         "CWeaponModelInfo::ScalarDeletingDestructor", "CPedModelInfo::ScalarDeletingDestructor", "CPedModelInfo::DestructorBody", "CWeaponModelInfo::DestructorBody",
         "CModelInfoAccelerator::FreeModelInfoIds", "CModelInfoAccelerator::GetModelInfoIdFile", "CModelInfoAccelerator::EndOfLoadPhase", "CTaskComplexSeekEntity<FixedPos>::Constructor",
         "CTaskComplexSeekEntity<FixedPos>::ScalarDeletingDestructor", "CTaskComplexSeekEntity<FixedPos>::Destructor"]
ENTRIES = (0x683440, 0x683450, 0x6834E0, 0x669FB0, 0x669FF0, 0x66F4A0, 0x673EF0,
           0x4C5CB0, 0x4C5D10, 0x4C5D70, 0x4C5DD0, 0x4C5E30, 0x4C5E90, 0x4C5EF0, 0x4C5F50, 0x4C67D0,
           0x4C6200, 0x4C6210, 0x4C6230, 0x4C6250, 0x4C6270, 0x4C6290, 0x4C62B0, 0x4C62D0, 0x4C6350, 0x4C62F0, 0x4C5790,
           0x4C6B10, 0x4C6B70, 0x4C6BD0, 0x66CA40, 0x66CB90, 0x66CBB0)
GUARD = 0x40
WIN = 0x1000
BIGWIN = 0x71000
SIZES = (GUARD + WIN + GUARD,) * 4 + (GUARD + BIGWIN + GUARD,)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000, 0x30400000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000
(NEXT, FIRST, CONTROL, CANJOIN, DISTSQ, SDD, WANDERNEW, STORE, THUNK, WEAPONBODY, PEDBODY, FREEIDS, GETFILE, ENDLOAD, SEEKCTOR, SEEKBODY) = range(16)
KIND = [NEXT, FIRST, CONTROL, CANJOIN, DISTSQ, SDD, WANDERNEW] + [STORE] * 9 + [THUNK] + [SDD] * 8 + [PEDBODY, WEAPONBODY, FREEIDS, GETFILE, ENDLOAD, SEEKCTOR, SDD, SEEKBODY]
assert len(KIND) == 33
SDD_BODY = {5: 0x669D20, 17: 0x4C5590, 18: 0x4C55D0, 19: 0x4C5620, 20: 0x4C5680, 21: 0x4C56E0, 22: 0x4C5750, 23: 0x4C5790, 24: 0x4C62F0, 31: 0x66CBB0}
SDD_DELETE = {5: 0x61A5B0, 31: 0x61A5B0, **{f: 0x8214BD for f in range(17, 25)}}
STORE_CAP = {7: 14000, 8: 70, 9: 1, 10: 169, 11: 1, 12: 51, 13: 92, 14: 212, 15: 278}
STORE_SIZE = {7: 0x20, 8: 0x24, 9: 0x24, 10: 0x24, 11: 0x28, 12: 0x28, 13: 0x24, 14: 0x308, 15: 0x44}
FILE_BLOCK = 0xA08C
F_999999 = 0x497423F0
WANDER = {0: (0x50, 0x460C80, 2), 12: (0x28, 0x658770, 3), 14: (0x38, 0x48E610, 3), 16: (0x3C, 0x672690, 3)}
WANDER_DEFAULT = (0x38, 0x48E4F0, 3)


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




class World:
    """las cinco arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(5))

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


def quiet(bits):
    """lo que queda de un float tras pasar por ST0 (fld convierte un NaN senalizante en silencioso)"""
    bits &= M32
    return bits | 0x400000 if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) else bits


def fnv_str(s):
    h = 2166136261
    for b in s:
        h = ((h ^ b) * 16777619) & M32
    return h


def has_nan(*vals):
    return any((v & 0x7F800000) == 0x7F800000 and (v & 0x7FFFFF) for v in vals)


def elem_writes(fn):
    w = [(0, 4, 0x85BB9C), (8, 2, 0), (0xA, 2, 0xFFFF)]
    if fn == 7:
        w += [(0, 4, 0x85BBF0)]
    elif fn == 8:
        w += [(0, 4, 0x85BC30)]
    elif fn == 9:
        w += [(0x20, 2, 0), (0x22, 2, 0), (0, 4, 0x85BC70)]
    elif fn == 10:
        w += [(0, 4, 0x85BCB0), (0x22, 2, 0xFFFF)]
    elif fn == 11:
        w += [(0x20, 2, 0), (0x22, 2, 0), (0, 4, 0x85BCF0), (0x26, 2, 0xFFFF)]
    elif fn == 12:
        w += [(0, 4, 0x85BD78)]
    elif fn == 13:
        w += [(0, 4, 0x85BD30)]
    elif fn == 14:
        w += [(0x5C, 4, 0), (0x2D0, 1, 0), (0x4E, 1, 0), (0, 4, 0x85C5C8), (0x304, 4, M32)]
        w += [(0x2D6 + 4 * i, 4, M32) for i in range(9)]
        w += [(0x2FA, 4, M32), (0x2FE, 4, M32)]
    elif fn == 15:
        w += [(0, 4, 0x85BDC0), (0x34, 4, 0)]
    return w


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind == FIRST:
        _k += ['ped_null', 'ped_set', 'new_null', 'new_arena', 'new_raw']
    elif _kind == CONTROL:
        _k += ['ped_null', 'dead', 'timer_out', 'alongside', 'no_alongside', 'matrix_ped', 'near', 'far', 'rotate', 'no_rotate', 'counter_wrap', 'nan_in', 'inf_in']
    elif _kind == CANJOIN:
        _k += ['not_started', 'restarted', 'out_of_time', 'not_out', 'wrapped']
    elif _kind == DISTSQ:
        _k += ['next_invalid', 'next_loaded', 'next_unloaded', 'last_invalid', 'last_loaded', 'last_unloaded', 'same_area', 'last_wins', 'next_wins', 'matrix_ped', 'nan_in']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == WANDERNEW:
        _k += ['cop', 'medic', 'criminal', 'prostitute', 'standard', 'new_null', 'new_set', 'rand_high_bits']
    elif _kind == STORE:
        _k += ['count_dirty']
    elif _kind == WEAPONBODY:
        _k += ['prev_same', 'prev_other']
    elif _kind == PEDBODY:
        _k += ['col_null', 'col_arena', 'col_raw']
    elif _kind == FREEIDS:
        _k += ['ptr_null', 'ptr_arena', 'ptr_raw']
    elif _kind == GETFILE:
        _k += ['open_ok', 'open_fail', 'open_negative']
    elif _kind == ENDLOAD:
        _k += ['loaded_flag', 'write_ok', 'open_fail', 'ptr_null']
    elif _kind == SEEKCTOR:
        _k += ['entity_null', 'entity_set', 'tired', 'not_tired', 'face', 'not_face']
    elif _kind == SEEKBODY:
        _k += ['entity_null', 'entity_set']
    KEYS[_fn] = _k


def node_pos(w0, w1, w2):
    """CPathNode::GetPosition (0x420A10): int16 * 0.125 (exacto) como bits de float"""
    return tuple(struct.unpack('<I', struct.pack('<f', signed16(v) * 0.125))[0] for v in (w0, w1, w2))


def sub3(p, q):
    return [v_sub(v_load(p[i]), v_load(q[i])) for i in range(3)]


def dist_t(dx, dy, dz):
    """(dz*dz + dy*dy) + dx*dx en la x87 a 24 bits"""
    return v_add(v_add(v_mul(dz, dz), v_mul(dy, dy)), v_mul(dx, dx))


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D, E = W.mem
    events, ret, extra = [], None, {}
    o = a[0]
    this = W.raw(4 if k == STORE else 0, o)
    if k == STORE:
        E.wr(o, 4, a[3])
    else:
        A.wr(o, 4, a[3])
    if k == NEXT:
        W.snap()
        ret = 0
    elif k == FIRST:
        ped = a[4]
        A.wr(o + 0xC, 4, ped); A.wr(o + 0x14, 4, a[5])
        new = 0 if a[7] == 0 else (W.raw(1, a[8]) if a[7] == 1 else a[9])
        W.snap()
        marks.append('ped_null' if ped == 0 else 'ped_set')
        ret = 0
        if ped:
            A.wr(o + 0x1C, 4, a[6]); A.wr(o + 0x20, 4, a[5]); A.wr(o + 0x24, 1, 1); A.wr(o + 0x28, 4, 0)
            events.append([0x61A5A0, 0x20])
            marks.append('new_null' if new == 0 else ('new_arena' if a[7] == 1 else 'new_raw'))
            if new:
                events.append([0x62F310, W.tagv(new), 0xF423F, 0, 0, 0x41000000])
                ret = W.tagv(a[10])
    elif k == CONTROL:
        has_ped = a[5] != 0
        waited = W.raw(1, a[6]) if has_ped else 0
        A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, waited); A.wr(o + 0x10, 4, a[7]); A.wr(o + 0x18, 1, a[8] & 0xFF); A.wr(o + 0x24, 1, a[9] & 0xFF); A.wr(o + 0x28, 4, a[10])
        Cc.wr(a[11] + 4, 4, a[14]); Cc.wr(a[11] + 8, 4, a[15]); Cc.wr(a[11] + 0xC, 4, a[16])
        mat_c = W.raw(3, a[13]) if a[12] else 0
        Cc.wr(a[11] + 0x14, 4, mat_c)
        if mat_c:
            D.wr(a[13] + 0x30, 4, a[23]); D.wr(a[13] + 0x34, 4, a[24]); D.wr(a[13] + 0x38, 4, a[25])
        Cc.wr(a[11] + 0x47C, 4, a[17])
        mat_b = 0
        if has_ped:
            B.wr(a[6] + 4, 4, a[18]); B.wr(a[6] + 8, 4, a[19]); B.wr(a[6] + 0xC, 4, a[20])
            mat_b = W.raw(3, a[22]) if a[21] else 0
            B.wr(a[6] + 0x14, 4, mat_b)
            if mat_b:
                D.wr(a[22] + 0x30, 4, a[26]); D.wr(a[22] + 0x34, 4, a[27]); D.wr(a[22] + 0x38, 4, a[28])
            B.wr(a[6] + 0x47C, 4, a[29])
        task = 0
        if a[32]:
            D.wr(a[33] + 0x14, 4, a[41]); D.wr(a[33] + 0x18, 4, a[42]); D.wr(a[33] + 0x1C, 4, a[43])
            task = W.raw(3, a[33])
        W.snap()
        ret = 0
        if not has_ped:
            marks.append('ped_null')
        else:
            events.append([0x5E0170, W.tagv(waited)])
            if (a[30] & 0xFF) == 0:
                marks.append('dead')
                events.append([0x571B70, W.tagv(waited), STACK_TAG])
                events.append([0x4AB420, W.tagv((a[17] + 0x68) & M32), STACK_TAG, 0, 0, 0, 1, 0x258, 0xFFFF, W.tagv(waited), 0, a[49]])
                events.append([0x571A00, W.tagv(waited), STACK_TAG])
            else:
                out = False
                if (a[9] & 0xFF) != 0:
                    events.append([0x420E30, W.tagv((this + 0x1C) & M32)])
                    if (a[31] & 0xFF) != 0:
                        marks.append('timer_out')
                        out = True
                if not out:
                    events.append([0x600EE0, W.tagv(a[29]), 0x4B8])
                    ped_pos = (a[23], a[24], a[25]) if mat_c else (a[14], a[15], a[16])
                    wpos = (a[26], a[27], a[28]) if mat_b else (a[18], a[19], a[20])
                    if mat_b:
                        marks.append('matrix_ped')
                    if task:
                        marks.append('alongside')
                        events.append([0x59C890, STACK_TAG, W.tagv(mat_c), a[41], a[42], a[43]])
                        d = sub3((a[44], a[45], a[46]), wpos)
                    else:
                        marks.append('no_alongside')
                        d = sub3(ped_pos, wpos)
                    t = dist_t(d[0], d[1], d[2])
                    r = v_load(a[7])
                    r2 = v_mul(r, r)
                    if has_nan(a[7], *(a[14:17] + a[18:21] + a[23:29] + a[41:47])):
                        marks.append('nan_in')
                    if any(((v & 0x7F800000) == 0x7F800000 and not (v & 0x7FFFFF)) for v in a[14:17] + a[18:21] + a[23:29] + a[41:47]):
                        marks.append('inf_in')
                    if v_cmp(t, r2) == 'lt':
                        marks.append('near')
                    else:
                        marks.append('far')
                        counter = A.rd(o + 0x28, 4)
                        if (A.rd(o + 0x18, 1) & 0xFF) != 0 and counter == 0:
                            marks.append('rotate')
                            diff = [store32(v_sub(v_load(wpos[i]), v_load(ped_pos[i]))) for i in range(3)]
                            events.append([0x53CBE0, diff[0], diff[1], 0, 0])
                            events.append([0x53CB50, quiet(a[47])])
                            Cc.wr(a[11] + 0x55C, 4, quiet(a[48]))
                        else:
                            marks.append('no_rotate')
                        nc = signed32((counter + 1) & M32)
                        A.wr(o + 0x28, 4, nc & M32)
                        if nc > 10:
                            A.wr(o + 0x28, 4, 0)
                            marks.append('counter_wrap')
                        ret = W.tagv(A.rd(o + 8, 4))
    elif k == CANJOIN:
        A.wr(o + 0x30, 1, a[4] & 0xFF); A.wr(o + 0x31, 1, a[5] & 0xFF); A.wr(o + 0x28, 4, a[6]); A.wr(o + 0x2C, 4, a[7])
        W.snap()
        clock = a[8]
        if (a[4] & 0xFF) == 0:
            marks.append('not_started')
            ret = 0
        else:
            if (a[5] & 0xFF) != 0:
                A.wr(o + 0x28, 4, clock); A.wr(o + 0x31, 1, 0)
                marks.append('restarted')
            end = (A.rd(o + 0x28, 4) + A.rd(o + 0x2C, 4)) & M32
            ret = 1 if clock >= end else 0
            marks.append('out_of_time' if ret else 'not_out')
            if A.rd(o + 0x28, 4) + A.rd(o + 0x2C, 4) > M32:
                marks.append('wrapped')
    elif k == DISTSQ:
        ped_off = a[18]
        B.wr(ped_off + 4, 4, a[19]); B.wr(ped_off + 8, 4, a[20]); B.wr(ped_off + 0xC, 4, a[21])
        mat = W.raw(2, a[23]) if a[22] else 0
        B.wr(ped_off + 0x14, 4, mat)
        if mat:
            Cc.wr(a[23] + 0x30, 4, a[24]); Cc.wr(a[23] + 0x34, 4, a[25]); Cc.wr(a[23] + 0x38, 4, a[26])
        next_valid, last_valid = a[4] != 0, a[7] != 0
        same = next_valid and last_valid and a[5] == a[8]
        next_mode = a[4]
        last_mode = a[4] if same else a[7]
        if next_mode == 1:
            for i in range(3):
                Cc.wr(a[10] + a[6] * 0x1C + 8 + 2 * i, 2, a[12 + i])
        if last_mode == 1:
            for i in range(3):
                if same:
                    Cc.wr(a[10] + a[9] * 0x1C + 8 + 2 * i, 2, a[15 + i])
                else:
                    B.wr(a[11] + a[9] * 0x1C + 8 + 2 * i, 2, a[15 + i])
        A.wr(o + 0x1C, 2, a[5] if next_valid else 0xFFFF); A.wr(o + 0x1E, 2, a[6]); A.wr(o + 0x18, 2, a[8] if last_valid else 0xFFFF); A.wr(o + 0x1A, 2, a[9])
        W.snap()
        pos = (a[24], a[25], a[26]) if mat else (a[19], a[20], a[21])
        marks.append('matrix_ped' if mat else 'no_matrix')
        marks.append(['next_invalid', 'next_loaded', 'next_unloaded'][next_mode])
        marks.append(['last_invalid', 'last_loaded', 'last_unloaded'][last_mode])
        if same:
            marks.append('same_area')
        if has_nan(*pos):
            marks.append('nan_in')

        def node_value(valid, mode, base_arena, base_off, nid):
            if not valid or mode != 1:
                return v_load(F_999999)
            at = (base_off + nid * 0x1C)
            ar = [Cc, B][base_arena]
            w = [ar.rd(at + 8 + 2 * i, 2) for i in range(3)]
            p = node_pos(*w)
            d = sub3(p, pos)
            return dist_t(d[0], d[1], d[2])
        nxt = node_value(next_valid, next_mode, 0, a[10], a[6])
        nxt_bits = store32(nxt)
        if same:
            last = node_value(last_valid, last_mode, 0, a[10], a[9])
        else:
            last = node_value(last_valid, last_mode, 1, a[11], a[9])
        if v_cmp(last, v_load(nxt_bits)) == 'lt':
            res = last
            marks.append('last_wins')
        else:
            res = v_load(nxt_bits)
            marks.append('next_wins')
        ret = store32(res)
    elif k == SDD:
        W.snap()
        events.append([SDD_BODY[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([SDD_DELETE[fn], W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == WANDERNEW:
        idx = (a[4] - 6) & M32
        size, ctor, nargs = WANDER.get(idx, WANDER_DEFAULT)
        marks.append({0: 'cop', 12: 'medic', 14: 'criminal', 16: 'prostitute'}.get(idx, 'standard'))
        new = 0 if a[5] == 0 else W.raw(1, a[6])
        W.snap()
        events.append([0x61A5A0, size])
        ret = 0
        if new == 0:
            marks.append('new_null')
        else:
            marks.append('new_set')
            if a[7] > 0xFFFF:
                marks.append('rand_high_bits')
            events.append([0x821B1E])
            direction = ((a[7] & 0xFFFF) * 8) >> 15
            events.append([ctor, W.tagv(new), 4, direction] + ([1] if nargs == 3 else []))
            ret = W.tagv(a[8])
    elif k == STORE:
        W.snap()
        cap, sz = STORE_CAP[fn], STORE_SIZE[fn]
        writes = elem_writes(fn)
        samples = []
        for q in range(8):
            idx = a[4 + q]
            at = o + 4 + idx * sz
            ref = bytearray(E.get(GUARD + at + i) for i in range(sz))
            new = bytearray(ref)
            for off, ln, val in writes:
                for t in range(ln):
                    new[off + t] = (val >> (8 * t)) & 0xFF
            samples.append([idx, [[i, new[i]] for i in range(sz) if new[i] != ref[i]]])
        extra['store'] = {'count': 0, 'allok': True, 'outside': 0, 'samples': samples}
        if a[3]:
            marks.append('count_dirty')
        ret = W.tagv(this)
    elif k == THUNK:
        W.snap()
        events.append([0x4C6207, W.tagv(this)])
    elif k == WEAPONBODY:
        W.snap()
        A.wr(o, 4, 0x85BB9C)
        marks.append('prev_same' if a[3] == 0x85BB9C else 'prev_other')
    elif k == PEDBODY:
        ptr = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        A.wr(o + 0x34, 4, ptr)
        W.snap()
        marks.append(['col_null', 'col_arena', 'col_raw'][a[4]])
        if ptr:
            events.append([0x40F700, W.tagv(ptr)])
            events.append([0x40FC40, W.tagv(ptr)])
        A.wr(o, 4, 0x85BB9C)
    elif k == FREEIDS:
        ptr = 0 if a[4] == 0 else (W.raw(4, a[5]) if a[4] == 1 else a[6])
        A.wr(o, 4, ptr)
        W.snap()
        marks.append(['ptr_null', 'ptr_arena', 'ptr_raw'][a[4]])
        events.append([0x8214BD, W.tagv(ptr)])
        A.wr(o, 4, 0)
    elif k == GETFILE:
        ln = a[6]
        path = bytes((ord('a') + (c['seed'] + i * 7) % 26) for i in range(ln))
        for i in range(ln):
            A.wr(o + 6 + i, 1, path[i])
        A.wr(o + 6 + ln, 1, 0)
        newptr = W.raw(4, a[5])
        W.snap()
        handle = a[4]
        ok = signed32(handle) > 0
        marks.append('open_ok' if ok else ('open_negative' if signed32(handle) < 0 else 'open_fail'))
        events.append([0x538900, W.tagv((this + 6) & M32), fnv_str(path), fnv_str(b'rb')])
        events.append([0x82119A, FILE_BLOCK])
        A.wr(o + 0x1B, 1, 1 if ok else 0)
        A.wr(o, 4, newptr)
        head = bytearray(32)
        if ok:
            events.append([0x538950, handle, W.tagv(newptr), FILE_BLOCK])
            marker = b''.join(struct.pack('<I', a[7 + i]) for i in range(4))
            head[:16] = marker
            events.append([0x5389D0, handle])
        extra['block'] = {'head': head.hex(), 'tail_zero': True, 'outside': 0}
        ret = 1 if ok else 0
    elif k == ENDLOAD:
        ptr = 0 if a[4] == 0 else (W.raw(4, a[5]) if a[4] == 1 else a[6])
        ln = a[9]
        path = bytes((ord('a') + (c['seed'] + i * 7) % 26) for i in range(ln))
        A.wr(o, 4, ptr); A.wr(o + 0x1B, 1, a[7] & 0xFF)
        for i in range(ln):
            A.wr(o + 6 + i, 1, path[i])
        A.wr(o + 6 + ln, 1, 0)
        W.snap()
        if ptr == 0:
            marks.append('ptr_null')
        if (a[7] & 0xFF) != 0:
            marks.append('loaded_flag')
        else:
            events.append([0x538910, W.tagv((this + 6) & M32), fnv_str(path)])
            if signed32(a[8]) > 0:
                marks.append('write_ok')
                events.append([0x538970, a[8], W.tagv(ptr), FILE_BLOCK])
                events.append([0x5389D0, a[8]])
            else:
                marks.append('open_fail')
        events.append([0x8214BD, W.tagv(ptr)])
        A.wr(o, 4, 0)
    elif k == SEEKCTOR:
        old54 = A.rd(o + 0x54, 1)
        W.snap()
        ent = a[4]
        marks.append('entity_null' if ent == 0 else 'entity_set')
        marks.append('tired' if a[10] & 1 else 'not_tired')
        marks.append('face' if a[11] & 1 else 'not_face')
        A.wr(o, 4, 0x87004C); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0)
        A.wr(o + 0xC, 4, ent); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x18, 4, a[7]); A.wr(o + 0x1C, 4, a[8]); A.wr(o + 0x20, 4, 0); A.wr(o + 0x24, 4, a[9])
        A.wr(o + 0x28, 4, 0); A.wr(o + 0x2C, 4, 0); A.wr(o + 0x30, 1, 0); A.wr(o + 0x31, 1, 0); A.wr(o + 0x34, 4, 0); A.wr(o + 0x38, 4, 0); A.wr(o + 0x3C, 1, 0); A.wr(o + 0x3D, 1, 0)
        A.wr(o + 0x40, 4, 0x86FD18); A.wr(o + 0x44, 4, 0); A.wr(o + 0x48, 4, 0); A.wr(o + 0x4C, 4, 0); A.wr(o + 0x50, 4, 6)
        A.wr(o + 0x54, 1, (old54 & 0xF0) | (((a[11] & 1)) << 1) | (a[10] & 1))
        if ent:
            events.append([0x571B70, W.tagv(ent), W.tagv((this + 0xC) & M32)])
        ret = W.tagv(this)
    elif k == SEEKBODY:
        A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, a[4])
        # las vtables (+0 y +0x40) no se comparan (la DLL deja las suyas): el banco las pone a cero antes y despues
        A.wr(o, 4, 0); A.wr(o + 0x40, 4, 0)
        W.snap()
        marks.append('entity_null' if a[4] == 0 else 'entity_set')
        if a[4]:
            events.append([0x571A00, W.tagv(a[4]), W.tagv((this + 0xC) & M32)])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks, 'extra': extra}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:8], e['events'][:8])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    for key in ('store', 'block'):
        if key in e['extra']:
            got, want = o.get(key), e['extra'][key]
            if got is None or any(got.get(f) != v for f, v in want.items()):
                return '%s: %s != %s' % (key, str(got)[:300], str(want)[:300])
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
