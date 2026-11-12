"""Oraculo independiente (Python) de Fn177A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 26 funciones del lote 177A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x558F90  CStats::GetFullFavoriteRadioStationList: mov eax,0B78E58h; ret (ECX y EDX intactos)
   1  0x55D0E0  SurfaceInfos_c::LoadAdhesiveLimits (ecx = this = la tabla 6x6): SetDir("") 5387D0; f = OpenFile("data\\surface.dat","rb") 538900; l = LoadLine(f) 536F80; mientras l: si l[0] == ';' o l[0] == 0: sigue;
            sscanf(l,"%s",nombre) (8220AD, el nombre no se usa); avanza l hasta ' ' o '\\t' (sin mirar el fin de cadena); para col = 0..fila: salta ' ' y '\\t'; valor = 0; si l[0] != '-': sscanf(l,"%f",&valor); avanza l hasta
            ' ', '\\t' o 0; tabla[fila*6+col] = tabla[col*6+fila] = valor (dword); fila++; l = LoadLine(f); al final CloseFile(f) 5389D0
   2  0x5F6190  CEventCreatePartnerTask::Constructor(tipo, companero, byte, float) `ret 10h`: ctor de CEvent 4ABFC0 (ecx = this); [+0Ch] = tipo; [+14h] = BYTE bajo del tercero; [this] = 86C6D0h; [+10h] = companero;
            [+18h] = float (dword); si el companero no es nulo: RegisterReference 571B70 (ecx = companero, pila: this+10h); eax = this
   3  0x5F6200  cuerpo del destructor: [this] = 86C6D0h; si [+10h] no es nulo: CleanUpOldReference 571A00 (ecx = [+10h], pila: this+10h); 4AC010 (ecx = this)
   4  0x5F6280  Clone: m = new(1Ch) (4B5620); si m: eax = 5F6190(m; tipo [+0Ch], companero [+10h], BYTE [+14h] extendido, float [+18h]); si no 0
   5  0x5F6300  destructor con borrado: call 5F6200; test byte [esp+8],1; si: delete this (4B5630); eax = this; ret 4
   6  0x62FF00  mov eax,0D5h
   7  0x62FF10  [ecx] = 86DE30h; jmp 62FB40
   8  0x62FF20  CTaskSimpleDrownInCar::Constructor: base 61A390 (ecx = this); [this] = 86DE54h; eax = this
   9  0x62FF40  mov eax,0D8h
  10  0x62FF50  xor al,al; ret 0Ch
  11  0x62FF60  [ecx] = 86DE54h; jmp 61A3A0
  12  0x6378D0  destructor con borrado: call 62FF10; delete (61A5B0) si bit 0; eax = this
  13  0x6378F0  como 12 con el cuerpo 62FF60
  14  0x637910  como 12 con el cuerpo 6300C0
  15  0x637930  como 12 con el cuerpo 6305F0
  16  0x637950  como 12 con el cuerpo 630F60
  17  0x637970  CTaskSimpleTired::ProcessPed(ped): si [+18h] == 0: StartAnim 630F70 (ecx = this, pila: ped); si [+14h] == 0: AL = 0; si [+15h] != 0: [+0Ch] = reloj (0xB7CB84) y [+15h] = 0;
            si reloj >= [+10h] + [+0Ch] (sin signo): llamada virtual [[this]+18h](this; ped, 0, 0) y AL = 1; si no AL = 0; ret 4
  18  0x6379D0  como 12 con el cuerpo 631070
  19  0x640430  CTaskComplexDragPedFromCar::Constructor(ped, tiempo) `ret 8`: base 63A220 (ecx = this; pila: 0, 0, 0, 1, 0); [this] = 86EB6Ch; [+50h] = ped; si ped no es nulo: RegisterReference 571B70 (ecx = ped, pila: this+50h);
            [+20h] = tiempo; eax = this
  20  0x6404C0  mov eax,2BFh
  21  0x6404D0  cuerpo del destructor: [this] = 86EB6Ch; si [+50h] no es nulo: CleanUpOldReference 571A00 (ecx = [+50h], pila: this+50h); 63DFA0 (ecx = this)
  22  0x643950  Clone: m = new(54h) (61A5A0); si m: eax = 640430(m; ped [+50h], tiempo [+20h]); si no 0
  23  0x660ED0  mov eax,4C0h
  24  0x664770  CalcTargetPos(retorno, ped) `ret 8`: veh = [this+0Ch]; modelo = [0A9B0C8h + movsx word [veh+22h] * 4]; caja = [modelo+14h] (minimo +0, maximo +0Ch); delta = pos(ped) - pos(veh) (posicion = [matriz+30h] si hay
            matriz [+14h], si no [+4]); derecha = [matriz] si hay (la del vehiculo), si no (cos(rumbo [veh+10h]), sin(rumbo), 0.0f) con fcos / fsin; producto (rz*dz + ry*dy) + rx*dx en la x87 a 24 bits; si > 0: [this+15h] = 1 y local.x =
            max.x + 0.5f; si no [this+15h] = 0 y local.x = min.x - 0.5f; local = (x, 0, 0); 59C890(retorno, matriz del vehiculo (puede ser nula), &local) (cdecl); eax = retorno
  25  0x6648A0  como 12 con el cuerpo 661030

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (los de dos, cuatro y cinco argumentos de pila anaden los argumentos, sin etiquetar); operator delete [direccion, puntero
(etiqueta), dword del puntero]; operator new [direccion, tamano]; RegisterReference, CleanUpOldReference y StartAnim [direccion, ECX, argumento] (etiquetas); SetDir [direccion, FNV de la cadena]; OpenFile [direccion, FNV de la ruta, FNV del
modo]; LoadLine [direccion, argumento, indice de la linea]; CloseFile [direccion, argumento]; la ranura 6 falsa [0x630FF0, ECX, ped, prioridad, evento]; CMatrix * CVector [direccion, resultado, matriz, vector (los de pila como 0x7F000000),
x, y, z del vector].
sscanf corre real (el CRT del EXE y el de la DLL): el oraculo lo modela para los textos que generan las fixtures (decimales sencillos, '-', 'x' y sufijos).

Modelo x87 de CalcTargetPos: precision de 24 bits (el juego fija _PC_24) con exponente sin limite: cada resta, producto y suma se redondea a 24 bits (al par mas cercano); NaN e infinitos con las reglas IEEE; seno y coseno de la x87
con mas precision que 24 bits (se calculan con 80 digitos decimales y se redondean a 64 bits); fcos / fsin dejan el operando intacto si |h| >= 2^63; el modelo solo cubre |h| < 1024 (y los casos exactos: cero, infinitos, NaN y >= 2^63).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from decimal import Decimal, getcontext
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CStats::GetFullFavoriteRadioStationList", "SurfaceInfos_c::LoadAdhesiveLimits", "CEventCreatePartnerTask::Constructor", "CEventCreatePartnerTask::DestructorBody",
         "CEventCreatePartnerTask::Clone", "CEventCreatePartnerTask::ScalarDeletingDestructor", "CTaskSimpleDrown::GetTaskType", "CTaskSimpleDrown::DestructorBody", "CTaskSimpleDrownInCar::Constructor",
         "CTaskSimpleDrownInCar::GetTaskType", "CTaskSimpleDrownInCar::MakeAbortable", "CTaskSimpleDrownInCar::DestructorBody", "CTaskSimpleDrown::ScalarDeletingDestructor", "CTaskSimpleDrownInCar::ScalarDeletingDestructor",
         "CTaskComplexDie::ScalarDeletingDestructor", "CTaskSimpleDead::ScalarDeletingDestructor", "CTaskSimpleTired::ScalarDeletingDestructor", "CTaskSimpleTired::ProcessPed", "CTaskSimpleSitDown::ScalarDeletingDestructor",
         "CTaskComplexDragPedFromCar::Constructor", "CTaskComplexDragPedFromCar::GetTaskType", "CTaskComplexDragPedFromCar::DestructorBody", "CTaskComplexDragPedFromCar::Clone", "CTaskGoToVehicleAndLean::GetTaskType",
         "CTaskGoToVehicleAndLean::CalcTargetPos", "CTaskLeanOnVehicle::ScalarDeletingDestructor"]
ENTRIES = (0x558F90, 0x55D0E0, 0x5F6190, 0x5F6200, 0x5F6280, 0x5F6300, 0x62FF00, 0x62FF10, 0x62FF20, 0x62FF40, 0x62FF50, 0x62FF60, 0x6378D0, 0x6378F0, 0x637910, 0x637930,
           0x637950, 0x637970, 0x6379D0, 0x640430, 0x6404C0, 0x6404D0, 0x643950, 0x660ED0, 0x664770, 0x6648A0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
SEED = 60                                                        # a[60..63] = semillas de las arenas
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
STACK_TAG = 0x7F000000

CONST, BASECTOR, BODY, MAKEAB, SDD, LOADADH, EVCTOR, EVBODY, EVCLONE, TIREDPROC, DRAGCTOR, DRAGBODY, DRAGCLONE, CALC = \
    'const basector body makeab sdd loadadh evctor evbody evclone tiredproc dragctor dragbody dragclone calc'.split()
KIND = [CONST, LOADADH, EVCTOR, EVBODY, EVCLONE, SDD, CONST, BODY, BASECTOR, CONST, MAKEAB, BODY, SDD, SDD, SDD, SDD, SDD, TIREDPROC, SDD, DRAGCTOR, CONST, DRAGBODY, DRAGCLONE, CONST, CALC, SDD]
CONSTS = {0: 0xB78E58, 6: 0xD5, 9: 0xD8, 20: 0x2BF, 23: 0x4C0}
BODY_VT = {7: (0x86DE30, 0x62FB40), 11: (0x86DE54, 0x61A3A0)}      # vtable que deja el cuerpo y direccion del colaborador al que salta
SDD_BODY = {5: (0x5F6200, 0x4B5630), 12: (0x62FF10, 0x61A5B0), 13: (0x62FF60, 0x61A5B0), 14: (0x6300C0, 0x61A5B0), 15: (0x6305F0, 0x61A5B0), 16: (0x630F60, 0x61A5B0), 18: (0x631070, 0x61A5B0), 25: (0x661030, 0x61A5B0)}
EV_VT, DROWNINCAR_VT, DRAG_VT = 0x86C6D0, 0x86DE54, 0x86EB6C
EVCTOR_ADDR, EVBASE, EVDTOR, EVNEW, REGREF, CLEANREF, TASKBASECTOR, ENTERCARCTOR, ENTERCARDTOR, NEWADDR, STARTANIM, ABORTSLOT, MUL, DRAGCTOR_ADDR = \
    0x5F6190, 0x4ABFC0, 0x4AC010, 0x4B5620, 0x571B70, 0x571A00, 0x61A390, 0x63A220, 0x63DFA0, 0x61A5A0, 0x630F70, 0x630FF0, 0x59C890, 0x640430
SETDIR, OPENFILE, LOADLINE, CLOSEFILE = 0x5387D0, 0x538900, 0x536F80, 0x5389D0
MODEL_TABLE = 0xA9B0C8


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


def fnv(data):
    h = 2166136261
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    """un float que pasa por la x87 (fld / fstp): un NaN senalizante sale silencioso"""
    return (b | 0x400000) & M32 if is_nan(b) else b & M32


def f32_add(abits, bbits):
    """abits + bbits en float de 24 bits (fld a; fadd b; fstp dword), con el redondeo al mas cercano par; NaN: se devuelve el NaN (silenciado); inf - inf: indefinido (0xFFC00000)"""
    na, nb = is_nan(abits), is_nan(bbits)
    if na and nb:
        raise ValueError('dos NaN: ambiguo entre x87 y SSE')
    if na:
        return quiet(abits)
    if nb:
        return quiet(bbits)
    ia, ib = (abits & 0x7FFFFFFF) == 0x7F800000, (bbits & 0x7FFFFFFF) == 0x7F800000
    sa, sb = abits >> 31, bbits >> 31
    if ia and ib:
        return abits if sa == sb else 0xFFC00000
    if ia:
        return abits
    if ib:
        return bbits

    def dec(b):
        s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
        return (-1 if s else 1) * (f if e == 0 else (f | 0x800000)), (-149 if e == 0 else e - 150)

    ma, ea = dec(abits)
    mb, eb = dec(bbits)
    e = min(ea, eb)
    total = (ma << (ea - e)) + (mb << (eb - e))
    if total == 0:
        return 0x80000000 if (sa and sb) else 0
    sign = 0x80000000 if total < 0 else 0
    mag = abs(total)
    length = mag.bit_length()
    shift = max(length - 24, -149 - e)
    if shift > 0:
        q, rem, half = mag >> shift, mag & ((1 << shift) - 1), 1 << (shift - 1)
        if rem > half or (rem == half and (q & 1)):
            q += 1
    else:
        q = mag << (-shift)
    ex = e + shift
    if q >= (1 << 24):
        q >>= 1
        ex += 1
    if q >= (1 << 23):
        biased = ex + 150
        if biased >= 255:
            return sign | 0x7F800000
        return sign | (biased << 23) | (q - (1 << 23))
    return sign | q


# ------------------------------------------------------------------------------------------------ x87 con precision de 24 bits y exponente sin limite
NAN = ('nan',)


def round_bits(fr, nbits):
    """redondea una fraccion a nbits bits significativos (al par mas cercano), sin limite de exponente"""
    if fr == 0:
        return Fraction(0)
    sign = -1 if fr < 0 else 1
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    p = Fraction(1 << e) if e >= 0 else Fraction(1, 1 << -e)
    if p > a:
        e -= 1
    quantum = Fraction(1 << (e - (nbits - 1))) if e - (nbits - 1) >= 0 else Fraction(1, 1 << -(e - (nbits - 1)))
    q = a / quantum
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    return sign * fl * quantum


def f32_value(bits):
    exp, frac = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    sign = -1 if bits >> 31 else 1
    if exp == 0xFF:
        return NAN if frac else ('inf', sign)
    if exp == 0:
        return ('fin', sign * Fraction(frac, 1 << 149))
    m = frac | 0x800000
    e = exp - 150
    return ('fin', sign * (Fraction(m * (1 << e)) if e >= 0 else Fraction(m, 1 << -e)))


def sign_of(v):
    return v[1] if v[0] == 'inf' else (-1 if v[1] < 0 else 1)


def is_zero(v):
    return v[0] == 'fin' and v[1] == 0


def xneg(v):
    if v[0] == 'nan':
        return v
    return ('inf', -v[1]) if v[0] == 'inf' else ('fin', -v[1])


def xmul(a, b):
    if a[0] == 'nan' or b[0] == 'nan':
        return NAN
    if a[0] == 'inf' or b[0] == 'inf':
        if is_zero(a) or is_zero(b):
            return NAN
        return ('inf', sign_of(a) * sign_of(b))
    return ('fin', round_bits(a[1] * b[1], 24))


def xadd(a, b):
    if a[0] == 'nan' or b[0] == 'nan':
        return NAN
    if a[0] == 'inf' and b[0] == 'inf':
        return a if a[1] == b[1] else NAN
    if a[0] == 'inf':
        return a
    if b[0] == 'inf':
        return b
    return ('fin', round_bits(a[1] + b[1], 24))


def xsub(a, b):
    return xadd(a, xneg(b))


def xgt0(v):
    return (v[0] == 'fin' and v[1] > 0) or (v[0] == 'inf' and v[1] > 0)


getcontext().prec = 80
PI = Decimal('3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798')
_SC = {}


def x87_sincos(hbits):
    """(coseno, seno) de la x87 para el rumbo `hbits`: valores extendidos (64 bits de mantisa)"""
    if hbits in _SC:
        return _SC[hbits]
    v = f32_value(hbits)
    if v[0] in ('nan', 'inf'):
        r = (NAN, NAN)
    elif abs(v[1]) >= 2 ** 63:
        r = (v, v)                                              # fcos / fsin: operando fuera de rango (C2 = 1): ST0 queda intacto
    elif v[1] == 0:
        r = (('fin', Fraction(1)), ('fin', Fraction(0)))
    else:
        fr = v[1]
        x = Decimal(fr.numerator) / Decimal(fr.denominator)
        two_pi = 2 * PI
        k = (x / two_pi).to_integral_value()
        t = x - k * two_pi
        t2 = t * t
        eps = Decimal(10) ** -78
        term, s, n = t, t, 1
        while abs(term) > eps:
            term = -term * t2 / ((2 * n) * (2 * n + 1))
            s += term
            n += 1
        term, c, n = Decimal(1), Decimal(1), 1
        while abs(term) > eps:
            term = -term * t2 / ((2 * n - 1) * (2 * n))
            c += term
            n += 1
        r = (('fin', round_bits(Fraction(c), 64)), ('fin', round_bits(Fraction(s), 64)))
    _SC[hbits] = r
    return r


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
        self.mem = tuple(Mem(a[2], a[SEED + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
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
    if _kind in (BASECTOR, BODY):
        _k += ['prev_zero', 'prev_other']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == LOADADH:
        _k += ['rows_zero', 'rows_six', 'rows_other', 'comment_line', 'blank_line', 'tab_name', 'value_dash', 'value_x', 'value_suffix', 'value_num', 'short_row', 'beyond_table']
    elif _kind == EVCTOR:
        _k += ['partner_null', 'partner_set', 'byte_high', 'float_nan']
    elif _kind == EVBODY:
        _k += ['partner_null', 'partner_set']
    elif _kind == EVCLONE:
        _k += ['mem_null', 'mem_set', 'byte_high']
    elif _kind == TIREDPROC:
        _k += ['anim_null', 'anim_set', 'not_started', 'started', 'restart', 'finished', 'running', 'wrap']
    elif _kind in (DRAGCTOR, DRAGBODY):
        _k += ['ped_null', 'ped_set']
    elif _kind == DRAGCLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == CALC:
        _k += ['veh_matrix', 'veh_no_matrix', 'ped_matrix', 'ped_no_matrix', 'ahead', 'behind', 'dot_nan', 'heading_nan', 'heading_huge', 'heading_small', 'heading_zero']
    KEYS[_fn] = _k


def scanf_float(text):
    """sscanf(text, "%f"): el prefijo decimal mas largo (signo, digitos, punto, digitos, exponente); sin coincidencia: None"""
    i, n = 0, len(text)
    while i < n and text[i] in b' \t':
        i += 1
    j = i
    if j < n and text[j] in b'+-':
        j += 1
    d0 = j
    while j < n and 48 <= text[j] <= 57:
        j += 1
    digits = j - d0
    if j < n and text[j] == 0x2E:
        j += 1
        d1 = j
        while j < n and 48 <= text[j] <= 57:
            j += 1
        digits += j - d1
    if digits == 0:
        return None
    if j < n and text[j] in b'eE':
        k = j + 1
        if k < n and text[k] in b'+-':
            k += 1
        e0 = k
        while k < n and 48 <= text[k] <= 57:
            k += 1
        if k > e0:
            j = k
    return struct.unpack('<I', struct.pack('<f', float(text[i:j].decode('ascii'))))[0], j


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
    if k == CONST:
        W.snap()
        ret = CONSTS[fn]
    elif k == MAKEAB:
        W.snap()
        ret = 0
    elif k == BASECTOR:
        W.snap()
        events.append([TASKBASECTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, DROWNINCAR_VT)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BODY:
        W.snap()
        vt, target = BODY_VT[fn]
        A.wr(o, 4, vt)
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == SDD:
        W.snap()
        body, delete = SDD_BODY[fn]
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([delete, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == LOADADH:
        W.snap()
        raw = bytes.fromhex(c['s'])
        lines = raw.split(b'\0')[:-1] if raw else []
        handle = a[4]
        events.append([SETDIR, fnv(b'')])
        events.append([OPENFILE, fnv(b'data\\surface.dat'), fnv(b'rb')])
        idx, row = 0, 0
        while True:
            events.append([LOADLINE, handle, idx])
            if idx >= len(lines):
                break
            line = lines[idx] + b'\0'          # el texto termina en 0 (el EXE lo lee con el 0 final)
            idx += 1
            if line[0] == 0x3B:
                marks.append('comment_line')
                continue
            if line[0] == 0:
                marks.append('blank_line')
                continue
            p = 0
            while line[p] != 0x20:
                if line[p] == 9:
                    marks.append('tab_name')
                    break
                p += 1
            for col in range(row + 1):
                while line[p] in (0x20, 9):
                    p += 1
                val = 0
                if line[p] == 0x2D:
                    marks.append('value_dash')
                else:
                    if line[p] == 0:
                        marks.append('short_row')
                    r = scanf_float(line[p:])
                    if r is None:
                        if line[p] != 0:
                            marks.append('value_x')
                    else:
                        val = r[0]
                        marks.append('value_suffix' if line[p + r[1]] not in (0x20, 9, 0) else 'value_num')
                while line[p] not in (0x20, 9, 0):
                    p += 1
                A.wr(o + 4 * (row * 6 + col), 4, val)
                A.wr(o + 4 * (col * 6 + row), 4, val)
            if row >= 6:
                marks.append('beyond_table')
            row += 1
        events.append([CLOSEFILE, handle])
        marks.append('rows_zero' if row == 0 else ('rows_six' if row == 6 else 'rows_other'))
    elif k == EVCTOR:
        W.snap()
        events.append([EVBASE, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
        A.wr(o + 0xC, 4, a[4])
        A.wr(o + 0x14, 1, a[6] & 0xFF)
        A.wr(o, 4, EV_VT)
        A.wr(o + 0x10, 4, a[5])
        A.wr(o + 0x18, 4, a[7])
        if a[5]:
            events.append([REGREF, W.tagv(a[5]), W.tagv((this + 0x10) & M32)])
            marks.append('partner_set')
        else:
            marks.append('partner_null')
        if a[6] >> 8:
            marks.append('byte_high')
        if is_nan(a[7]):
            marks.append('float_nan')
        ret = W.tagv(this)
    elif k == EVBODY:
        A.wr(o + 0x10, 4, a[4])
        W.snap()
        A.wr(o, 4, EV_VT)
        if a[4]:
            events.append([CLEANREF, W.tagv(a[4]), W.tagv((this + 0x10) & M32)])
            marks.append('partner_set')
        else:
            marks.append('partner_null')
        events.append([EVDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == EVCLONE:
        A.wr(o + 0xC, 4, a[8]); A.wr(o + 0x10, 4, a[9]); A.wr(o + 0x14, 4, a[10]); A.wr(o + 0x18, 4, a[11])
        W.snap()
        events.append([EVNEW, 0x1C])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([EVCTOR_ADDR, W.tagv(mem), a[8], a[9], a[10] & 0xFF, a[11]])
            W.wr(mem, 4, a[7])
            ret = W.tagv(a[6])
        if a[10] >> 8:
            marks.append('byte_high')
    elif k == TIREDPROC:
        anim, started, flag, start, dur, clock, ped = a[4], a[5] & 0xFF, a[6] & 0xFF, a[7], a[8], a[9], a[10]
        A.wr(o + 0xC, 4, start); A.wr(o + 0x10, 4, dur)
        A.wr(o + 0x14, 4, started | (flag << 8) | ((a[11] << 16) & M32))
        A.wr(o + 0x18, 4, anim)
        W.snap()
        if anim == 0:
            events.append([STARTANIM, W.tagv(this), W.tagv(ped)])
            marks.append('anim_null')
        else:
            marks.append('anim_set')
        if started == 0:
            marks.append('not_started')
            ret = 0
        else:
            marks.append('started')
            if flag != 0:
                start = clock
                A.wr(o + 0xC, 4, start); A.wr(o + 0x15, 1, 0)
                marks.append('restart')
            if start + dur > M32:
                marks.append('wrap')
            if clock >= ((start + dur) & M32):
                events.append([ABORTSLOT, W.tagv(this), W.tagv(ped), 0, 0])
                ret = 1
                marks.append('finished')
            else:
                ret = 0
                marks.append('running')
    elif k == DRAGCTOR:
        W.snap()
        events.append([ENTERCARCTOR, W.tagv(this), 0, 0, 0, 1, 0])
        A.wr(o, 4, a[6])
        A.wr(o, 4, DRAG_VT)
        A.wr(o + 0x50, 4, a[4])
        if a[4]:
            events.append([REGREF, W.tagv(a[4]), W.tagv((this + 0x50) & M32)])
            marks.append('ped_set')
        else:
            marks.append('ped_null')
        A.wr(o + 0x20, 4, a[5])
        ret = W.tagv(this)
    elif k == DRAGBODY:
        A.wr(o + 0x50, 4, a[4])
        W.snap()
        A.wr(o, 4, DRAG_VT)
        if a[4]:
            events.append([CLEANREF, W.tagv(a[4]), W.tagv((this + 0x50) & M32)])
            marks.append('ped_set')
        else:
            marks.append('ped_null')
        events.append([ENTERCARDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == DRAGCLONE:
        A.wr(o + 0x20, 4, a[8]); A.wr(o + 0x50, 4, a[9])
        W.snap()
        events.append([NEWADDR, 0x54])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([DRAGCTOR_ADDR, W.tagv(mem), a[9], a[8]])
            W.wr(mem, 4, a[7])
            ret = W.tagv(a[6])
    elif k == CALC:
        veh, ped = W.raw(1, a[4]), W.raw(1, a[5])
        vmat = 0 if a[6] == 0 else W.raw(1, a[7])
        pmat = 0 if a[8] == 0 else W.raw(1, a[9])
        retbuf = W.raw(2, a[39])
        A.wr(o + 0xC, 4, veh)
        W.snap()
        vpos = [a[17], a[18], a[19]] if vmat else [a[10], a[11], a[12]]
        ppos = [a[23], a[24], a[25]] if pmat else [a[20], a[21], a[22]]
        delta = [xsub(f32_value(ppos[i]), f32_value(vpos[i])) for i in range(3)]
        if vmat:
            right = [f32_value(a[14]), f32_value(a[15]), f32_value(a[16])]
            marks.append('veh_matrix')
        else:
            cosv, sinv = x87_sincos(a[13])
            right = [cosv, sinv, ('fin', Fraction(0))]
            marks.append('veh_no_matrix')
            hexp = (a[13] >> 23) & 0xFF
            if is_nan(a[13]) or hexp == 0xFF:
                marks.append('heading_nan')
            elif hexp >= 0xBE:
                marks.append('heading_huge')
            elif (a[13] & 0x7FFFFFFF) == 0:
                marks.append('heading_zero')
            else:
                marks.append('heading_small')
        marks.append('ped_matrix' if pmat else 'ped_no_matrix')
        # (rz * dz + ry * dy) + rx * dx
        t1 = xmul(right[2], delta[2])
        t2 = xmul(right[1], delta[1])
        s1 = xadd(t2, t1)
        t3 = xmul(right[0], delta[0])
        dot = xadd(t3, s1)
        ahead = xgt0(dot)
        if dot[0] == 'nan':
            marks.append('dot_nan')
        minx, maxx = a[30], a[33]
        if ahead:
            A.wr(o + 0x15, 1, 1)
            lx = f32_add(maxx, 0x3F000000)
            marks.append('ahead')
        else:
            A.wr(o + 0x15, 1, 0)
            lx = f32_add(minx, 0xBF000000)
            marks.append('behind')
        events.append([MUL, W.tagv(retbuf), W.tagv(vmat), STACK_TAG, lx, 0, 0])
        for i in range(3):
            W.wr((retbuf + 4 * i) & M32, 4, a[36 + i])
        ret = W.tagv(retbuf)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos, salvo en GetFullFavoriteRadioStationList (mov eax,imm; ret): EAX = la direccion, y ECX y EDX quedan como entraron. Ningun llamador del EXE lee EAX/ECX/EDX tras las demas
    llamadas salvo EAX (valor devuelto, ya comprobado)"""
    if c['fn'] == 0:
        World_ = World(c)
        if residue['eax'] != 0xB78E58 or residue['ecx'] != World_.raw(0, c['a'][0]) or residue['edx'] != c['edx']:
            return 'registros %s' % residue
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
