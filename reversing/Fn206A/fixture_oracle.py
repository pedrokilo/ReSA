"""Oraculo independiente (Python) de Fn206A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 30 funciones del lote 206A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial y la aritmetica x87 de 24 bits (redondeo a 24 bits del significando con exponente ampliado; almacenamiento como float de 32 bits) con fracciones exactas.

Desensamblado del EXE (sobre el gta-sa.exe canonico; dumpbin /disasm). Argumentos: [esp+4] = primero, etc. (stdcall `ret N` salvo indicacion).
   0  0x5EB6F0  (efecto, entidad, lista): lista = [esp+0Ch]; n = [lista+4] ? ([lista+8] - [lista+4]) >> 2 (con signo) : 0; para i < n (con signo): a = [inicio + 4 * i]; si [a+4] == efecto y [a+8] == entidad devuelve a; si no, nulo
   1  0x5EB740  igual; con la primera coincidencia: cola = [a+20h] (inicio), [a+24h] (fin): inicio nulo o (fin - inicio) >> 2 == 0 -> nulo; si no [inicio]; sin seguir buscando
   2  0x5EB7B0  (peaton, efecto, entidad, lista): como 0 pero con la coincidencia llama a 0x5EB4C0 (CPedAttractor::IsRegisteredWithPed, ecx = a, un argumento: el peaton): AL distinto de cero -> devuelve a; si no sigue
   3  0x5EC660  (peaton, atractor, lista): atractor nulo -> 0; busca el atractor por indice (n con signo) en la lista: ausente -> 0; llama por la vtable (ranura 5, `call [eax+14h]`, un argumento: el peaton); suma de las dos colas
            (+10h/+14h y +20h/+24h: inicio nulo -> 0, si no (fin - inicio) >> 2, con signo) en 32 bits; distinta de cero -> 1; si no: busca el atractor por punteros entre [lista+4] y [lista+8] (si inicio == fin, no busca);
            si lo encuentra desplaza los siguientes hacia delante y baja [lista+8] en 4; borra por la vtable (ranura 1, `call [edx+4]`, push 1); devuelve 1
   4  0x5EC740  igual con 0x5EC5B0 (CPedAttractor::DeRegisterPed, ecx = atractor, argumento el peaton) en lugar de la ranura 5
   5-9 0x5EB930, 0x5EB960, 0x5EB990, 0x5EB9D0, 0x5EBA00: (origen, fin, destino): marco de 5 dwords (origen, fin, destino, hueco con el ECX, destino), call 0x5EAE10 / 0x5EAE40 / 0x5EAE70 / 0x5EAEA0 / 0x5EAEE0 (cdecl), `add esp, 14h`, devuelve su EAX
  10  0x5EC820  thiscall: si [this+4] != 0: operator delete (0x8214BD, cdecl, [this+4]); [this+4] = [this+8] = [this+0Ch] = 0
  11  0x5EC4B0  thiscall(banderas) `ret 4`: call 0x5EC410 (cuerpo del destructor, ecx = this); si el bit 0 del BYTE de banderas: libera en el pool de atractores [0B744C4h]: idx = (this - [pool]) / 0C4h (imul por 5397829Dh; sar 6; + signo);
            byte [[pool+4] + idx] |= 80h; si idx < [pool+0Ch] (con signo): [pool+0Ch] = idx; devuelve this
  12  0x61FC20  constructor de CTaskSimpleBeKickedOnGround: base 0x61A390 ([+4] = 0; vtable 86D4A8h); byte [+8] = 0; dword [+0Ch] = 0; vtable 86D7D8h; devuelve this
  13  0x61FC40  mov eax,3EFh; ret
  14  0x61FCC0  MakeAbortable(peaton, prioridad, evento) `ret 0Ch`: prioridad 1 o 2: si [this+0Ch] (animacion): 0x4CEBE0 (SetFinishCallback, ecx = animacion, push 0, push 4D1390h); [this+0Ch] = 0; devuelve 1; otra:
            si animacion: byte [+2Eh] |= 4; dword [+1Ch] = 0C0800000h; devuelve 0
  15  0x61FD20  StartAnim(peaton) `ret 4`: 0x56D270 (KickingWhenDown, AL): 0 -> byte [this+8] = 1; fin. Si no: 0x4D6A70(clump = [ped+18h], 800h) cdecl; id = 24h + (eax != 0 ? 3 : 0); 0x4D4610(clump, 0, id, 8.0f) cdecl -> [this+0Ch];
            0x4CEA80 (SetCurrentTime, ecx = animacion, push 0.0f); byte [anim+2Eh] |= 1, &= F7h; 0x56D270 otra vez: si AL: 0x734A40(clump), 0x7C51A0(jerarquia, [[ped+490h]+14h]), 0x7C5120(jerarquia), 0x7EDD90(&v, &v, 1, matrices + (indice << 6))
            con v = (0, 0, 0) en la pila; al final 0x4CEBE0(ecx = [this+0Ch], 61FD10h, this)
  16  0x61FE00  ProcessPed(peaton) `ret 4`: byte [this+8] != 0 -> 1; si [this+0Ch] == 0: 0x61FD20(peaton, ecx = this); devuelve 0
  17  0x623120  Clone: 0x61A5A0(10h); si no es nulo: 0x61A390, vtable 86D7D8h, byte [+8] = 0, dword [+0Ch] = 0; devuelve el objeto o nulo
  18  0x61FE30  constructor de CTaskSimpleBeHitWhileMoving(modo, direccion) `ret 8`: base 0x61A390; byte [+8] = 0; dword [+14h] = 0; vtable 86D7FCh; [+0Ch] = modo; [+10h] = direccion; devuelve this
  19  0x61FE60  mov eax,3F1h; ret
  20  0x61FEE0  como 14 con la animacion en [this+14h]
  21  0x61FF40  StartAnim(peaton) `ret 4` (ver el arbol de decisiones en expected())
  22  0x620290  como 16 con [this+14h] y 0x61FF40
  23  0x623190  Clone: 0x61A5A0(18h); lee [this+10h] y [this+0Ch]; base 0x61A390; [+10h], [+0Ch]; vtable 86D7FCh; byte [+8] = 0; dword [+14h] = 0
  24  0x6348A0  constructor de CTaskComplexUseMobilePhone(duracion) `ret 4`: base 0x61A3B0 ([+4] = 0, [+8] = 0; vtable 86D4CCh); [+0Ch] = duracion; vtable 86E454h; dwords [+10h] y [+14h], bytes [+18h], [+19h], [+1Ch], [+1Dh] = 0
  25  0x6348D0  mov eax,640h; ret
  26  0x6348E0  [this] = 86E454h; jmp 0x61A3D0 (vtable 86D4CCh; si [this+8]: delete virtual (push 1; call [[sub]+4]); [this+8] = 0; vtable 86D48Ch)
  27  0x6348F0  PutAway(peaton) `ret 4`: 0x5E3990(ecx = peaton, 14Ah); 0x5E6280(ecx = peaton, [peaton+70Ch]); [peaton+70Ch] = 37h; si [peaton+480h]: byte [[peaton+480h]+85h] = 0
  28  0x634930  MakeAbortable(peaton, prioridad, evento) `ret 0Ch`: ver expected()
  29  0x634A60  CreateNextSubTask(peaton) `ret 4`: ver expected()

Los falsos anotan: new [0x61A5A0, tamano]; cuerpo [direccion, ECX (etiqueta), dword de [ECX]]; delete [direccion, puntero]; ranuras virtuales [7E1EA5Ex, ECX, ...]; constructores [direccion, ECX, argumentos]; ...
Las vtables que escriben constructores y destructores se omiten de los cambios y se anotan como clase ("vt": la del EXE o una de la DLL).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = json.loads(r'''["CPedAttractorManager::FindAssociatedAttractorList", "CPedAttractorManager::GetPedUsingEffectList", "CPedAttractorManager::GetRelevantAttractorList", "CPedAttractorManager::BroadcastDepartureList", "CPedAttractorManager::DeRegisterPedList", "CPedAttractor::SArrayUninitCopyWrapStride4A", "CPedAttractor::SArrayUninitCopyWrapStride20", "CPedAttractor::SArrayUninitCopyWrapStride4B", "CPedAttractor::SArrayUninitCopyWrapStride12", "CPedAttractor::SArrayUninitCopyWrapStride4C", "CPedAttractor::SArrayTidyStride4", "CPedAttractor::ScalarDeletingDestructor", "CTaskSimpleBeKickedOnGround::Constructor", "CTaskSimpleBeKickedOnGround::GetTaskType", "CTaskSimpleBeKickedOnGround::MakeAbortable", "CTaskSimpleBeKickedOnGround::StartAnim", "CTaskSimpleBeKickedOnGround::ProcessPed", "CTaskSimpleBeKickedOnGround::Clone", "CTaskSimpleBeHitWhileMoving::Constructor", "CTaskSimpleBeHitWhileMoving::GetTaskType", "CTaskSimpleBeHitWhileMoving::MakeAbortable", "CTaskSimpleBeHitWhileMoving::StartAnim", "CTaskSimpleBeHitWhileMoving::ProcessPed", "CTaskSimpleBeHitWhileMoving::Clone", "CTaskComplexUseMobilePhone::Constructor", "CTaskComplexUseMobilePhone::GetTaskType", "CTaskComplexUseMobilePhone::DestructorBody", "CTaskComplexUseMobilePhone::PutAway", "CTaskComplexUseMobilePhone::MakeAbortable", "CTaskComplexUseMobilePhone::CreateNextSubTask"]''')
ENTRIES = (0x5EB6F0, 0x5EB740, 0x5EB7B0, 0x5EC660, 0x5EC740, 0x5EB930, 0x5EB960, 0x5EB990, 0x5EB9D0, 0x5EBA00, 0x5EC820, 0x5EC4B0, 0x61FC20, 0x61FC40, 0x61FCC0,
           0x61FD20, 0x61FE00, 0x623120, 0x61FE30, 0x61FE60, 0x61FEE0, 0x61FF40, 0x620290, 0x623190, 0x6348A0, 0x6348D0, 0x6348E0, 0x6348F0, 0x634930, 0x634A60)
GUARD = 0x40
WIN = 0x2000
N = GUARD + WIN + GUARD
SEED0 = 92
VT_OBJ, VT_EVT, VT_TASK = 0x1000, 0x1100, 0x1200
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(FIND, PEDUSING, RELEVANT, BCAST, DEREG, UCOPY, TIDY, ADTOR, BKCTOR, TASKTYPE, MAKEABORT, BKSTART, PROCPED, BKCLONE, BHCTOR, BHCLONE, BHSTART, MPCTOR, MPDTORBODY, PUTAWAY, MPABORT, MPNEXT) = \
    ('find pedusing relevant bcast dereg ucopy tidy adtor bkctor tasktype makeabort bkstart procped bkclone bhctor bhclone bhstart mpctor mpdtorbody putaway mpabort mpnext').split()
KIND = [FIND, PEDUSING, RELEVANT, BCAST, DEREG, UCOPY, UCOPY, UCOPY, UCOPY, UCOPY, TIDY, ADTOR, BKCTOR, TASKTYPE, MAKEABORT, BKSTART, PROCPED, BKCLONE,
        BHCTOR, TASKTYPE, MAKEABORT, BHSTART, PROCPED, BHCLONE, MPCTOR, TASKTYPE, MPDTORBODY, PUTAWAY, MPABORT, MPNEXT]
CLOCK, FRAMECTR, ATTRPOOL = 0xB7CB84, 0xB7CB4C, 0xB744C4
EV_VDTOR, EV_SUBTYPE, EV_SUBABORT, EV_EVTYPE, EV_CFST, EV_VBCAST, EV_TRAP = 0x7E1EA5E0, 0x7E1EA5E1, 0x7E1EA5E2, 0x7E1EA5E3, 0x7E1EA5E4, 0x7E1EA5E5, 0x7E1EA5FF
STACK_TAG = 0x7F000000
UCOPY_ADDR = {5: 0x5EAE10, 6: 0x5EAE40, 7: 0x5EAE70, 8: 0x5EAEA0, 9: 0x5EAEE0}
TASKTYPE_VAL = {13: 0x3EF, 19: 0x3F1, 25: 0x640}
ATTR_MAGIC = 0x5397829D
F1000, F500, F100, F2P32 = 0x447A0000, 0x43FA0000, 0x42C80000, 0x4F800000


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


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    return (b | 0x400000) & M32 if is_nan(b) else b & M32


def dec(b):
    s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    return (-1 if s else 1) * (f if e == 0 else (f | 0x800000)), (-149 if e == 0 else e - 150)


def round_sig(x, bits):
    """redondea la fraccion x a `bits` bits de mantisa (al par, exponente sin limite)"""
    if x == 0:
        return x
    sign = -1 if x < 0 else 1
    mag = abs(x)
    e = mag.numerator.bit_length() - mag.denominator.bit_length()
    if Fraction(2) ** e > mag:
        e -= 1
    step = Fraction(2) ** (e - bits + 1)
    q = mag / step
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl & 1):
        fl += 1
    return sign * fl * step


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

    def changes(self, skip=()):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - GUARD
            if self.m[i] != ref and not any(s <= rel < s + 4 for s in skip):
                out.append([rel, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[SEED0 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte
        self.glob_ref = {}

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        addr &= M32
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise KeyError('direccion fuera de las arenas: %08x' % addr)

    def rd(self, addr, n):
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
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

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


# ------------------------------------------------------------------------------------------------ aritmetica x87 de 24 bits (exponente ampliado), valores exactos
# un valor es ('n', Fraction, negativo_si_cero) | ('i', signo) | ('q', bits_de_NaN_silencioso)
DEFAULT_NAN = 0xFFC00000


def ld(bits):
    bits &= M32
    if is_nan(bits):
        return ('q', quiet(bits))
    if (bits & 0x7FFFFFFF) == 0x7F800000:
        return ('i', bits >> 31)
    m, e = dec(bits)
    return ('n', Fraction(m) * Fraction(2) ** e, bool(bits >> 31) and m == 0)


def fild(i):
    return ('n', Fraction(i), False)


def _nan_of(a, b):
    na, nb = a[0] == 'q', b[0] == 'q'
    if na and nb:
        return a                                      # solo importa que sea un NaN: _ftol de un NaN da siempre 0
    return a if na else (b if nb else None)


def fadd(a, b):
    n = _nan_of(a, b)
    if n:
        return n
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else ('q', DEFAULT_NAN)
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('n', Fraction(0), a[1] == 0 and b[1] == 0 and a[2] and b[2])
    return ('n', round_sig(s, 24), False)


def fneg(a):
    if a[0] == 'q':
        return a
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('n', -a[1], (not a[2]) if a[1] == 0 else False)


def fsub(a, b):
    n = _nan_of(a, b)
    return n if n else fadd(a, fneg(b))


def _isneg(v):
    return v[2] if v[1] == 0 else v[1] < 0


def fmul(a, b):
    n = _nan_of(a, b)
    if n:
        return n
    if a[0] == 'i' or b[0] == 'i':
        other = b if a[0] == 'i' else a
        if other[0] == 'n' and other[1] == 0:
            return ('q', DEFAULT_NAN)
        sign = (a[1] if a[0] == 'i' else (1 if _isneg(a) else 0)) ^ (b[1] if b[0] == 'i' else (1 if _isneg(b) else 0))
        return ('i', sign)
    neg = _isneg(a) != _isneg(b)
    p = a[1] * b[1]
    if p == 0:
        return ('n', Fraction(0), neg)
    return ('n', round_sig(p, 24), False)


def ftol(v):
    """_ftol (0x821B40): fistp de 64 bits con truncado; NaN, infinito y fuera de rango dan el entero indefinido (EAX = 0); el EXE usa los 32 bits bajos"""
    if v[0] != 'n':
        return 0
    x = v[1]
    t = int(x)
    if t >= 1 << 63 or t < -(1 << 63):
        return 0
    return t & M32


def pool_index(diff, magic):
    """operator delete en linea del EXE: (this - [pool]) / tamano con la constante magica: imul; sar edx,6; + bit de signo"""
    d = signed(diff)
    hi = (d * magic) >> 32
    q = hi >> 6
    q += 1 if q < 0 else 0
    return q


def lt_zero(bits):
    """fld [x]; fcomp 0.0; fnstsw; test ah,5; jp: la rama `nueva` solo si x < 0 ordenado (NaN y 0 no)"""
    if is_nan(bits):
        return False
    return f32(bits) < 0.0


""" ------------------------------------------------------------------------------------------------ marcas de cobertura """
COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
for _fn in range(len(NAMES)):
    _k = list(COMMON)
    _kind = KIND[_fn]
    if _kind in (FIND, PEDUSING, RELEVANT):
        _k += ['list_null', 'list_empty', 'list_normal', 'list_negative', 'list_endnull', 'match_first', 'match_later', 'no_match', 'fx_differs', 'entity_differs']
        if _kind == PEDUSING:
            _k += ['queue_null', 'queue_empty', 'queue_one', 'queue_negative', 'queue_two', 'stops_at_first']
        if _kind == RELEVANT:
            _k += ['registered', 'not_registered', 'continues_after_unregistered']
    elif _kind in (BCAST, DEREG):
        _k += ['att_null', 'att_absent', 'att_found', 'sum_zero', 'sum_nonzero', 'removed', 'not_removed', 'mod_queues', 'mod_list', 'list_null', 'list_empty', 'removed_middle', 'removed_last', 'removed_first']
    elif _kind == UCOPY:
        _k += ['arg_raw', 'arg_arena', 'arg_null', 'ret_raw', 'ret_arena', 'ret_null']
    elif _kind == TIDY:
        _k += ['begin_null', 'begin_set', 'stale_fields']
    elif _kind == ADTOR:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'idx_zero', 'idx_positive', 'lowers_first_free', 'keeps_first_free', 'rem_nonzero']
    elif _kind == MAKEABORT:
        _k += ['prio_urgent', 'prio_immediate', 'prio_leisure', 'prio_other', 'anim', 'no_anim']
    elif _kind == BKSTART:
        _k += ['kick1_false', 'kick1_true', 'kick2_true', 'kick2_false', 'assoc_flag', 'assoc_none']
    elif _kind == PROCPED:
        _k += ['finished', 'starts', 'has_anim']
    elif _kind in (BKCLONE, BHCLONE):
        _k += ['mem_null', 'mem_set']
    elif _kind == BHCTOR:
        _k += ['mode_special', 'mode_other']
    elif _kind == BHSTART:
        _k += ['special', 'nonspecial', 'skip_random', 'reuse_existing', 'reuse_blend', 'mix_attached', 'mix_found', 'mix_blend', 'time_p0', 'time_p1', 'time_p2', 'time_n0', 'time_n1', 'clock_negative', 'delta_nan',
               'delta_negative', 'nan_total', 'inf_total', 'ftol_zero', 'dirn1', 'dirn2', 'dirn3', 'dirn_other', 'mode2', 'mode4', 'mode_none', 'null_anim_finish']
    elif _kind == MPCTOR:
        _k += ['dur_negative', 'dur_nonneg']
    elif _kind == MPDTORBODY:
        _k += ['sub_null', 'sub_set']
    elif _kind == PUTAWAY:
        _k += ['player_data', 'no_player_data', 'saved_default']
    elif _kind == MPABORT:
        _k += ['prio2', 'prio_other', 'sub_aborts', 'sub_refuses', 'saved_default', 'type_pause', 'event_null', 'event_on_fire_player', 'event_on_fire_other', 'event_other', 'type_phone_out', 'type_other']
    elif _kind == MPNEXT:
        _k += ['type_talk', 'type_in', 'type_out', 'type_pause', 'type_other', 'mem_null', 'mem_set', 'dur_negative', 'quit_set', 'aborting_set', 'out_pause', 'out_clear', 'pause_first', 'pause_quit']
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
    vptrs = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tag = W.tagv
    vtobj, vtevt, vttask = W.raw(3, VT_OBJ), W.raw(3, VT_EVT), W.raw(3, VT_TASK)

    def cnt(begin, end):
        return 0 if begin == 0 else signed((end - begin) & M32) >> 2

    if k in (FIND, PEDUSING, RELEVANT):
        mode, n, bo = a[4], a[5], a[6]
        for s in range(8):
            Cc.wr(0x40 * s + 4, 4, a[16 + s]); Cc.wr(0x40 * s + 8, 4, a[24 + s])
            qm, P = a[40 + s], W.raw(3, 0x20 * s)
            qb, qe = {0: (0, a[56 + s]), 1: (P, P), 2: (P, P + 4), 3: (P, (P - 4) & M32), 4: (P, P + 8)}[qm]
            Cc.wr(0x40 * s + 0x20, 4, qb); Cc.wr(0x40 * s + 0x24, 4, qe & M32)
            if qm:
                D.wr(0x20 * s, 4, a[48 + s])
        begin = 0 if mode == 0 else W.raw(1, bo)
        end = {0: a[12], 1: begin, 2: begin + 4 * n, 3: begin - a[13], 4: 0}[mode] & M32
        if mode:
            for i in range(n):
                B.wr(bo + 4 * i, 4, W.raw(2, 0x40 * (a[32 + i] % 8)))
        A.wr(o + 4, 4, begin); A.wr(o + 8, 4, end)
        if k == RELEVANT:
            ped, fx, ent = a[9], a[7], a[8]
        else:
            fx, ent = a[7], a[8]
        W.snap()
        marks.append({0: 'list_null', 1: 'list_empty', 2: 'list_normal', 3: 'list_negative', 4: 'list_endnull'}[mode])
        count = cnt(begin, end)
        ret, calls, matched = 0, 0, 0
        for i in range(max(count, 0)):
            s = a[32 + i] % 8
            if a[16 + s] != fx:
                marks.append('fx_differs')
                continue
            if a[24 + s] != ent:
                marks.append('entity_differs')
                continue
            obj = W.raw(2, 0x40 * s)
            matched += 1
            marks.append('match_first' if i == 0 else 'match_later')
            if k == FIND:
                ret = tag(obj)
                break
            if k == PEDUSING:
                qm = a[40 + s]
                P = W.raw(3, 0x20 * s)
                qb, qe = {0: (0, a[56 + s]), 1: (P, P), 2: (P, P + 4), 3: (P, (P - 4) & M32), 4: (P, P + 8)}[qm]
                marks.append({0: 'queue_null', 1: 'queue_empty', 2: 'queue_one', 3: 'queue_negative', 4: 'queue_two'}[qm])
                if qb == 0 or cnt(qb, qe & M32) == 0:
                    ret = 0
                else:
                    ret = tag(a[48 + s])
                if count - 1 > i:
                    marks.append('stops_at_first')
                break
            events.append([0x5EB4C0, tag(obj), tag(ped)])
            bit = (a[10] >> calls) & 1 if calls < 32 else 0
            calls += 1
            if bit:
                marks.append('registered')
                ret = tag(obj)
                break
            marks.append('not_registered')
        else:
            if matched and k == RELEVANT:
                marks.append('continues_after_unregistered')
        if not matched:
            marks.append('no_match')
        if ret is None:
            ret = 0
    elif k in (BCAST, DEREG):
        mode, n, bo = a[4], a[5], a[6]
        for s in range(6):
            base = 0x40 * s
            Cc.wr(base, 4, vtobj)
            for (cf, code, bgn, endraw) in ((0x10, a[16 + s], a[32 + s], a[48 + s]), (0x20, a[24 + s], a[40 + s], a[56 + s])):
                pairs = {0: (0, endraw), 1: (bgn, bgn), 2: (bgn, bgn + 4), 3: (bgn, bgn + 8), 4: (bgn, bgn - 4)}[code]
                Cc.wr(base + cf, 4, pairs[0]); Cc.wr(base + cf + 4, 4, pairs[1] & M32)
        begin = 0 if mode == 0 else W.raw(1, bo)
        end = {0: a[14], 1: begin}.get(mode, (begin + 4 * n) & M32)
        if mode:
            for i in range(n):
                B.wr(bo + 4 * i, 4, W.raw(2, 0x40 * i))
        A.wr(o + 4, 4, begin); A.wr(o + 8, 4, end & M32)
        ped = a[7]
        sel = a[8]
        j = a[9] % n if mode >= 2 else a[9] % 6
        att = 0 if sel == 0 else (a[10] if sel == 2 else W.raw(2, 0x40 * j))
        W.snap()
        marks.append('list_null' if mode == 0 else ('list_empty' if mode == 1 else 'list_normal'))
        ret = 0
        count = cnt(begin, end & M32)
        if att == 0:
            marks.append('att_null')
        else:
            idx = None
            for i in range(max(count, 0)):
                if W.rd(begin + 4 * i, 4) == att:
                    idx = i
                    break
            if idx is None:
                marks.append('att_absent')
            else:
                marks.append('att_found')
                s = (att - W.raw(2, 0)) // 0x40
                events.append([EV_VBCAST if k == BCAST else 0x5EC5B0, tag(att), tag(ped)])
                if a[11] & 1:
                    marks.append('mod_queues')
                    Cc.wr(0x40 * s + 0x10, 4, a[12]); Cc.wr(0x40 * s + 0x14, 4, (a[12] + 4 * a[13]) & M32)
                    Cc.wr(0x40 * s + 0x20, 4, a[14]); Cc.wr(0x40 * s + 0x24, 4, (a[14] + 4 * a[15]) & M32)
                if a[11] & 2:
                    marks.append('mod_list')
                    A.wr(o + 8, 4, (begin + 4 * a[22]) & M32)
                q1 = cnt(Cc.rd(0x40 * s + 0x10, 4), Cc.rd(0x40 * s + 0x14, 4))
                q2 = cnt(Cc.rd(0x40 * s + 0x20, 4), Cc.rd(0x40 * s + 0x24, 4))
                total = (q1 + q2) & M32
                ret = 1
                if total != 0:
                    marks.append('sum_nonzero')
                else:
                    marks.append('sum_zero')
                    lb, le = A.rd(o + 4, 4), A.rd(o + 8, 4)
                    if lb != le:
                        p = lb
                        while True:
                            if W.rd(p, 4) == att:
                                nxt = p + 4
                                if nxt != le:
                                    q = nxt
                                    while q != le:
                                        W.wr(q - 4, 4, W.rd(q, 4))
                                        q += 4
                                A.wr(o + 8, 4, (A.rd(o + 8, 4) - 4) & M32)
                                marks.append('removed')
                                marks.append('removed_first' if p == lb else ('removed_last' if p + 4 == le else 'removed_middle'))
                                break
                            p += 4
                            if p == le:
                                marks.append('not_removed')
                                break
                    else:
                        marks.append('not_removed')
                    events.append([EV_VDTOR, tag(att), 1])
        ret = ret & 0xFF
    elif k == UCOPY:
        vals = []
        for i in range(3):
            m, v = a[4 + 2 * i], a[5 + 2 * i]
            vals.append(v if m == 0 else (W.raw(0, v) if m == 1 else (W.raw(1, v) if m == 2 else 0)))
            marks.append('arg_raw' if m == 0 else ('arg_arena' if m in (1, 2) else 'arg_null'))
        m, v = a[10], a[11]
        r = v if m == 0 else (W.raw(3, v) if m == 1 else 0)
        marks.append('ret_raw' if m == 0 else ('ret_arena' if m == 1 else 'ret_null'))
        W.snap()
        events.append([UCOPY_ADDR[fn]] + [tag(x) for x in vals])
        ret = tag(r)
    elif k == TIDY:
        begin = {0: 0, 1: a[5], 2: W.raw(1, a[5])}[a[4]]
        A.wr(o + 4, 4, begin); A.wr(o + 8, 4, a[6]); A.wr(o + 0xC, 4, a[7])
        W.snap()
        if begin:
            marks.append('begin_set')
            events.append([0x8214BD, tag(begin)])
        else:
            marks.append('begin_null')
            if a[6] or a[7]:
                marks.append('stale_fields')
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
    elif k == ADTOR:
        idx, rem = a[9], a[10]
        diff = idx * kAttractorSize + rem
        objects = (this - diff) & M32
        pool = a[7]
        B.wr(pool + 0, 4, objects); B.wr(pool + 4, 4, W.raw(3, a[8])); B.wr(pool + 8, 4, 0x40); B.wr(pool + 0xC, 4, a[11])
        D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) & 0x7F)       # el hueco esta usado
        W.setg(ATTRPOOL, 4, W.raw(1, pool))
        gorder = [(ATTRPOOL, 4)]
        W.snap()
        events.append([0x5EC410, tag(this), A.rd(o, 4)])
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            index = pool_index((this - B.rd(pool, 4)) & M32, ATTR_MAGIC)
            state_off = a[8] + index
            D.wr(state_off, 1, D.rd(state_off, 1) | 0x80)
            first = signed(B.rd(pool + 0xC, 4))
            if index < first:
                B.wr(pool + 0xC, 4, index & M32)
                marks.append('lowers_first_free')
            else:
                marks.append('keeps_first_free')
            marks.append('idx_zero' if index == 0 else 'idx_positive')
            if rem:
                marks.append('rem_nonzero')
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tag(this)
    elif k == BKCTOR:
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 1, 0); A.wr(o + 0xC, 4, 0)
        vptrs.append([0, o, 'vt'])
        ret = tag(this)
    elif k == TASKTYPE:
        W.snap()
        ret = TASKTYPE_VAL[fn]
    elif k == MAKEABORT:
        ao = 0xC if fn == 14 else 0x14
        ap = W.raw(1, a[5]) if a[4] else 0
        A.wr(o + ao, 4, ap)
        if a[4]:
            B.wr(a[5] + 0x1C, 4, a[9]); B.wr(a[5] + 0x2E, 1, a[10])
        W.snap()
        prio = a[6]
        marks.append('anim' if a[4] else 'no_anim')
        if prio in (1, 2):
            marks.append('prio_urgent' if prio == 1 else 'prio_immediate')
            if a[4]:
                events.append([0x4CEBE0, tag(ap), 0x4D1390, 0, a[9]])
                A.wr(o + ao, 4, 0)
            ret = 1
        else:
            marks.append('prio_leisure' if prio == 0 else 'prio_other')
            if a[4]:
                B.wr(a[5] + 0x2E, 1, B.rd(a[5] + 0x2E, 1) | 4)
                B.wr(a[5] + 0x1C, 4, 0xC0800000)
            ret = 0
    elif k == BKSTART:
        po, ao = a[12], a[6]
        A.wr(o + 0xC, 4, a[14])
        clump = 0 if a[7] == M32 else W.raw(3, a[7])
        if a[7] != M32:
            D.wr(a[7], 1, 2)
        Cc.wr(po + 0x18, 4, clump)
        Cc.wr(po + 0x490, 4, W.raw(3, a[13]))
        D.wr(a[13] + 0x14, 4, a[8])
        B.wr(ao + 0x1C, 4, a[15])
        ped = W.raw(2, po)
        W.snap()
        events.append([0x56D270])
        if (a[4] & 0xFF) == 0:
            marks.append('kick1_false')
            A.wr(o + 8, 1, 1)
        else:
            marks.append('kick1_true')
            events.append([0x4D6A70, tag(clump), 0x800])
            marks.append('assoc_flag' if a[5] else 'assoc_none')
            anim = 0x27 if a[5] else 0x24
            events.append([0x4D4610, tag(clump), 0, anim, 0x41000000])
            A.wr(o + 0xC, 4, W.raw(1, ao))
            events.append([0x4CEA80, tag(W.raw(1, ao)), 0])
            fl = B.rd(ao + 0x2E, 1)
            B.wr(ao + 0x2E, 1, (fl | 1) & 0xF7)
            events.append([0x56D270])
            if (a[16] & 0xFF) != 0:
                marks.append('kick2_true')
                events.append([0x734A40, tag(clump)])
                events.append([0x7C51A0, tag(a[9]), a[8]])
                events.append([0x7C5120, tag(a[9])])
                events.append([0x7EDD90, STACK_TAG, STACK_TAG, 1, tag((a[11] + ((a[10] << 6) & M32)) & M32), 0, 0, 0])
            else:
                marks.append('kick2_false')
            events.append([0x4CEBE0, tag(W.raw(1, ao)), 0x61FD10, tag(this), B.rd(ao + 0x1C, 4)])
    elif k == PROCPED:
        ao = 0xC if fn == 16 else 0x14
        A.wr(o + 8, 1, a[4] & 0xFF); A.wr(o + ao, 4, a[5])
        W.snap()
        if (a[4] & 0xFF) != 0:
            marks.append('finished')
            ret = 1
        else:
            ret = 0
            if a[5] == 0:
                marks.append('starts')
                events.append([0x61FD20 if fn == 16 else 0x61FF40, tag(this), tag(a[6])])
            else:
                marks.append('has_anim')
    elif k in (BKCLONE, BHCLONE):
        if k == BHCLONE:
            A.wr(o + 0xC, 4, a[6]); A.wr(o + 0x10, 4, a[7])
        W.snap()
        events.append([0x61A5A0, 0x10 if k == BKCLONE else 0x18])
        if not a[4]:
            marks.append('mem_null')
            ret = 0
        else:
            marks.append('mem_set')
            m = a[5]
            B.wr(m + 4, 4, 0)
            if k == BKCLONE:
                B.wr(m + 8, 1, 0); B.wr(m + 0xC, 4, 0)
            else:
                B.wr(m + 0x10, 4, a[7]); B.wr(m + 0xC, 4, a[6]); B.wr(m + 8, 1, 0); B.wr(m + 0x14, 4, 0)
            vptrs.append([1, m, 'vt'])
            ret = tag(W.raw(1, m))
    elif k == BHCTOR:
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 1, 0); A.wr(o + 0x14, 4, 0); A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
        vptrs.append([0, o, 'vt'])
        marks.append('mode_special' if a[4] in (2, 4) else 'mode_other')
        ret = tag(this)
    elif k == BHSTART:
        po, fo, no, ho = a[28], a[20], a[26], a[24]
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, a[5])
        A.wr(o + 0x14, 4, W.raw(1, no) if a[6] else 0)
        clump = 0 if a[7] == M32 else W.raw(3, a[7])
        if a[7] != M32:
            D.wr(a[7], 1, 2)
        Cc.wr(po + 0x18, 4, clump); Cc.wr(po + 0x20, 2, a[8] & 0xFFFF); Cc.wr(po + 0xFC, 4, a[9]); Cc.wr(po + 0x744, 4, a[29])
        B.wr(fo + 0x1C, 4, a[21]); B.wr(fo + 0x20, 4, a[22]); B.wr(fo + 0x2C, 2, a[23] & 0xFFFF); B.wr(fo + 0x14, 4, W.raw(3, ho))
        D.wr(ho + 0x10, 4, a[25])
        B.wr(no + 0x1C, 4, a[27])
        W.setg(CLOCK, 4, a[10]); W.setg(FRAMECTR, 4, a[11])
        gorder = [(CLOCK, 4), (FRAMECTR, 4)]
        ped = W.raw(2, po)
        F, Nw = W.raw(1, fo), W.raw(1, no)
        W.snap()
        st = {'ip': 0, 'rn': 0}

        def isplayer():
            events.append([0x5DF8F0, tag(ped)])
            v = (a[12] >> (8 * st['ip'])) & 0xFF
            st['ip'] += 1
            return v != 0

        def rand():
            events.append([0x821B1E])
            v = a[13 + st['rn']]
            st['rn'] += 1
            return v

        def clockreg():
            c0 = signed(a[10])
            v = fild(c0)
            if c0 < 0:
                v = fadd(v, ld(F2P32))
                marks.append('clock_negative')
            return v

        def finish():
            assoc = A.rd(o + 0x14, 4)
            if assoc == 0:
                marks.append('null_anim_finish')
            events.append([0x4CEBE0, tag(assoc), 0x61FF30, tag(this), W.rd(assoc + 0x1C, 4) if assoc else 0])

        def reuse(aid):
            events.append([0x4D68B0, tag(clump), aid])
            got = F if a[19] else 0
            A.wr(o + 0x14, 4, got)
            blend = True
            if got:
                marks.append('delta_nan' if is_nan(a[21]) else ('delta_negative' if lt_zero(a[21]) else 'reuse_existing'))
                if not lt_zero(a[21]):
                    blend = False
            if blend:
                marks.append('reuse_blend')
                events.append([0x4D4610, tag(clump), 0, aid, 0x41000000])
                A.wr(o + 0x14, 4, Nw)
            cur = A.rd(o + 0x14, 4)
            events.append([0x4CEA80, tag(cur), 0])
            W.wr(cur + 0x2E, 1, (W.rd(cur + 0x2E, 1) | 1) | 8)

        def timing(found):
            p = isplayer()
            if found == 0:
                marks.append('time_p0' if p else 'time_n0')
                rem = ((Cc.rd(po + 0x20, 2) + a[11]) & M32) % 1000
                Cc.wr(po + 0x744, 4, (rem + a[10] + 1000) & M32)
                return
            total = ld(a[25])
            if a[25] and is_nan(a[25]):
                marks.append('nan_total')
            if (a[25] & 0x7FFFFFFF) == 0x7F800000:
                marks.append('inf_total')
            aid = B.rd(fo + 0x2C, 2)
            if p:
                if aid in (0x6E, 0x6F):
                    marks.append('time_p2')
                    r = fadd(fsub(fadd(fmul(total, ld(F1000)), clockreg()), fmul(ld(a[22]), ld(F1000))), ld(F100))
                else:
                    marks.append('time_p1')
                    r = fadd(fadd(fmul(total, ld(F1000)), clockreg()), ld(F500))
            else:
                marks.append('time_n1')
                rem = ((Cc.rd(po + 0x20, 2) + a[11]) & M32) % 1000
                r = fadd(fadd(fadd(fild(rem), fmul(total, ld(F1000))), clockreg()), ld(F500))
            v = ftol(r)
            if v == 0:
                marks.append('ftol_zero')
            Cc.wr(po + 0x744, 4, v)

        def mix(bid):
            if a[9]:
                marks.append('mix_attached')
                finish()
                return
            events.append([0x5E1950, tag(ped)])
            events.append([0x5DEF20, tag(ped)])
            events.append([0x4D68B0, tag(clump), bid])
            if a[19]:
                marks.append('mix_found')
                events.append([0x4CEA80, tag(F), 0])
                B.wr(fo + 0x2E, 1, B.rd(fo + 0x2E, 1) | 1)
                B.wr(fo + 0x18, 4, 0)
                B.wr(fo + 0x1C, 4, 0x41000000)
                timing(F)
            else:
                marks.append('mix_blend')
                events.append([0x4D4610, tag(clump), 0, bid, 0x41000000])
                A.wr(o + 0x14, 4, Nw)
                timing(0)
            finish()

        mode, dirn = a[4], a[5]
        marks.append({4: 'mode4', 2: 'mode2'}.get(mode, 'mode_none'))
        marks.append({1: 'dirn1', 2: 'dirn2', 3: 'dirn3'}.get(dirn, 'dirn_other'))
        special = False
        if mode in (4, 2):
            if not isplayer():
                if rand() & 1:
                    special = True
            if not special:
                if (rand() & 7) == 0:
                    special = True
                else:
                    marks.append('skip_random')
        if not special:
            marks.append('nonspecial')
            go = True
            if not isplayer():
                if (rand() & 1) == 0:
                    go = False
            if not go:
                events.append([0x821B1E])     # 0x407180 del EXE: un solo rand()
                st['rn'] += 1
                finish()
            else:
                reuse({1: 0x1D, 2: 0x1E, 3: 0x1F}.get(dirn, 0x1C))
                finish()
        else:
            marks.append('special')
            if dirn == 1:
                mix(0x19)
            elif dirn == 2:
                if rand() & 1:
                    reuse(0x22); finish()
                else:
                    mix(0x1A)
            elif dirn == 3:
                mix(0x1B)
            else:
                if mode == 2:
                    mix(0x14)
                elif rand() & 1:
                    reuse(0x25); finish()
                elif rand() & 1:
                    reuse(0x20); finish()
                else:
                    mix(0x13)
    elif k == MPCTOR:
        W.snap()
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, a[4])
        A.wr(o + 0x10, 4, 0); A.wr(o + 0x14, 4, 0); A.wr(o + 0x18, 1, 0); A.wr(o + 0x19, 1, 0); A.wr(o + 0x1C, 1, 0); A.wr(o + 0x1D, 1, 0)
        vptrs.append([0, o, 'vt'])
        marks.append('dur_negative' if a[4] >= 0x80000000 else 'dur_nonneg')
        ret = tag(this)
    elif k == MPDTORBODY:
        sub = W.raw(1, a[5]) if a[4] else 0
        if a[4]:
            B.wr(a[5], 4, vttask)
        A.wr(o + 8, 4, sub)
        W.snap()
        if sub:
            marks.append('sub_set')
            events.append([EV_VDTOR, tag(sub), 1])
        else:
            marks.append('sub_null')
        A.wr(o + 8, 4, 0)
        vptrs.append([0, o, 'vt'])
    elif k == PUTAWAY:
        po = a[4]
        ped = W.raw(2, po)
        Cc.wr(po + 0x70C, 4, a[5])
        Cc.wr(po + 0x480, 4, W.raw(3, a[7]) if a[6] else 0)
        if a[6]:
            D.wr(a[7] + 0x85, 1, a[8])
        W.snap()
        events.append([0x5E3990, tag(ped), 0x14A])
        events.append([0x5E6280, tag(ped), a[5]])
        Cc.wr(po + 0x70C, 4, 0x37)
        if a[6]:
            marks.append('player_data')
            D.wr(a[7] + 0x85, 1, 0)
        else:
            marks.append('no_player_data')
        if a[5] == 0x37:
            marks.append('saved_default')
    elif k == MPABORT:
        so, po = a[4], a[10]
        sub = W.raw(1, so)
        B.wr(so, 4, vttask)
        A.wr(o + 8, 4, sub); A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[22]); A.wr(o + 0x14, 4, a[23]); A.wr(o + 0x18, 1, a[21]); A.wr(o + 0x1C, 1, a[20])
        ped = W.raw(2, po)
        Cc.wr(po + 0x474, 4, a[11]); Cc.wr(po + 0x478, 4, a[12]); Cc.wr(po + 0x70C, 4, a[13])
        ev = 0
        if a[8]:
            ev = W.raw(3, a[9])
            D.wr(a[9], 4, vtevt)
        W.setg(CLOCK, 4, a[14])
        gorder = [(CLOCK, 4)]
        W.snap()
        prio = a[6]

        def clear_flags():
            Cc.wr(po + 0x478, 4, Cc.rd(po + 0x478, 4) & 0xFFFFFFFD)
            Cc.wr(po + 0x474, 4, Cc.rd(po + 0x474, 4) & 0xFFBFFFFF)
        if prio == 2:
            marks.append('prio2')
            events.append([EV_SUBABORT, tag(sub), tag(ped), 2, tag(ev)])
            r = a[17] & 0xFF
            if r:
                marks.append('sub_aborts')
                clear_flags()
                if a[13] != 0x37:
                    events.append([0x6348F0, tag(this), tag(ped)])
                else:
                    marks.append('saved_default')
            else:
                marks.append('sub_refuses')
            ret = r
        else:
            marks.append('prio_other')
            events.append([EV_SUBTYPE, tag(sub)])
            ret = None
            if a[15] == 0xCA:
                marks.append('type_pause')
                clear_flags()
                ret = 1
            else:
                done = False
                if ev:
                    events.append([EV_EVTYPE, tag(ev)])
                    if a[18] == 0x3A:
                        events.append([0x5DF8F0, tag(ped)])
                        if (a[19] & 0xFF) != 0:
                            marks.append('event_on_fire_player')
                            ret = 1
                            done = True
                        else:
                            marks.append('event_on_fire_other')
                    else:
                        marks.append('event_other')
                    if not done:
                        A.wr(o + 0x1C, 1, 1)
                else:
                    marks.append('event_null')
                if not done:
                    A.wr(o + 0x10, 4, a[14]); A.wr(o + 0x14, 4, a[5]); A.wr(o + 0x18, 1, 1)
                    events.append([EV_SUBTYPE, tag(sub)])
                    if a[16] == 0x643:
                        marks.append('type_phone_out')
                        events.append([EV_SUBABORT, tag(sub), tag(ped), prio, tag(ev)])
                    else:
                        marks.append('type_other')
                    ret = 0
    elif k == MPNEXT:
        so, po = a[4], a[7]
        A.wr(o, 4, vttask)
        sub = W.raw(1, so)
        B.wr(so, 4, vttask)
        A.wr(o + 8, 4, sub); A.wr(o + 0xC, 4, a[5]); A.wr(o + 0x10, 4, a[16]); A.wr(o + 0x14, 4, a[17]); A.wr(o + 0x1D, 1, a[9]); A.wr(o + 0x1C, 1, a[10]); A.wr(o + 0x18, 1, a[11])
        ped = W.raw(2, po)
        Cc.wr(po + 0x474, 4, a[18]); Cc.wr(po + 0x478, 4, a[19])
        W.setg(CLOCK, 4, a[12])
        gorder = [(CLOCK, 4)]
        mem = W.raw(3, a[14]) if a[13] else 0
        W.snap()
        t = a[8]

        def clear_flags():
            Cc.wr(po + 0x478, 4, Cc.rd(po + 0x478, 4) & 0xFFFFFFFD)
            Cc.wr(po + 0x474, 4, Cc.rd(po + 0x474, 4) & 0xFFBFFFFF)
        events.append([EV_SUBTYPE, tag(sub)])
        ret = 0
        if t == 0x641:
            marks.append('type_talk')
            events.append([0x61A5A0, 0x20])
            if mem:
                marks.append('mem_set')
                events.append([0x61A900, tag(mem), 0, 0x92, 0x40800000, 0x643, 0x86E48C, 0])
                ret = tag(mem)
            else:
                marks.append('mem_null')
        elif t == 0x642:
            marks.append('type_in')
            if a[5] < 0x80000000:
                A.wr(o + 0x10, 4, a[12]); A.wr(o + 0x14, 4, a[5]); A.wr(o + 0x18, 1, 1)
            else:
                marks.append('dur_negative')
            events.append([0x61A5A0, 0x20])
            if mem:
                marks.append('mem_set')
                events.append([0x61A900, tag(mem), 0, 0x93, 0x40800000, 0x641, 0x86E480, 0])
                ret = tag(mem)
            else:
                marks.append('mem_null')
        elif t == 0x643:
            marks.append('type_out')
            if (a[9] & 0xFF) == 0 and (a[10] & 0xFF) != 0:
                marks.append('out_pause')
                events.append([0x61A5A0, 0x18])
                if mem:
                    marks.append('mem_set')
                    events.append([0x48E750, tag(mem), 0x3E8])
                    ret = tag(mem)
                else:
                    marks.append('mem_null')
            else:
                marks.append('out_clear')
                clear_flags()
        elif t == 0xCA:
            marks.append('type_pause')
            A.wr(o + 0x1C, 1, 0); A.wr(o + 0x18, 1, 0)
            if (a[9] & 0xFF) != 0:
                marks.append('pause_quit')
                clear_flags()
            else:
                marks.append('pause_first')
                events.append([EV_CFST, tag(this), tag(ped)])
                ret = tag(a[15])
        else:
            marks.append('type_other')
        if (a[9] & 0xFF) != 0:
            marks.append('quit_set')
        if (a[10] & 0xFF) != 0:
            marks.append('aborting_set')
    skip = {0: [], 1: [], 2: [], 3: []}
    for ar, off, _ in vptrs:
        skip[ar].append(off)
    return {'events': events, 'changes': {'A': A.changes(skip[0]), 'B': B.changes(skip[1]), 'C': Cc.changes(skip[2]), 'D': D.changes(skip[3])}, 'vptrs': vptrs,
            'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


kAttractorSize = 0xC4


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:16], e['events'][:16])
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
