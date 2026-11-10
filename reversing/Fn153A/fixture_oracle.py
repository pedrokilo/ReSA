"""Oraculo independiente (Python) de Fn153A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 24 funciones del lote 153A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (fracciones exactas) de CVector::Sum y del volumen de CAEVehicleAudioEntity.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x4B14F0  CEventStuckInAir::GetEventType (vtable 0x85B508, ranura 1): mov eax,5Dh; ret
   1  0x4B1500  GetLifeTime (ranura 3): xor eax,eax; ret
   2  0x4B1510  CanBeInterruptedBySameEvent (ranura 15): mov al,1; ret
   3  0x65FF50  CTaskGangHasslePed::GetTaskType (vtable 0x86FA00, ranura 4): mov eax,4BCh; ret
   4  0x410390  CColStore::FindColSlot(): or eax,0FFFFFFFFh; ret
   5  0x6D0B30  CRideAnimData::~CRideAnimData: ret
   6  0x6D0B10  CRideAnimData::CRideAnimData(): mov eax,ecx; xor ecx,ecx; mov [eax+0],ecx .. mov [eax+18h],ecx; ret
   7  0x4F63B0  CAETwinLoopSoundEntity::CAETwinLoopSoundEntity(): mov eax,ecx; xor ecx,ecx; mov [eax+10h],ecx; mov [eax],85F438h; mov [eax+88h],cx; mov [eax+0A0h],ecx; mov [eax+0A4h],ecx; ret
   8  0x6D0AF0  CTrailer::IsBaggageTrailer: cmp dword [ecx+9ECh],0C47A0000h; jne +; mov al,1; ret; xor al,al; ret
   9  0x40FDD0  CVector::Sum: mov eax,[esp+4]; fld [eax]; mov edx,[esp+8]; fadd [edx]; fstp [ecx]; (y: +4) (z: +8); ret 8
  10  0x40FD50  CColDisk::Set: [ecx+0Ch]=a0; copia [a1+0..8] a [ecx+0..8]; copia [a2+0..8] a [ecx+14h..1Ch]; [ecx+20h]=a3; [ecx+10h]=byte a4; [ecx+11h]=byte a5; [ecx+12h]=byte a6; ret 1Ch
  11  0x4F56D0  CAEVehicleAudioEntity (+0D8h m_EventVolume): movsx eax,word [esp+4]; fld [esp+0Ch]; fadd [ecx+0D8h]; mov ecx,[ecx+eax*8+0E8h]; test; fstp [ecx+14h]; mov [ecx+1Ch],[esp+8]; ret 0Ch
  12  0x4B5630  CEvent::operator delete: pool [0B744ACh]: indice = (obj - [pool]) / 44h (con signo); [[pool+4]+indice] |= 80h; si indice < [pool+0Ch] (con signo): [pool+0Ch] = indice; ret
  13  0x4B1640  CEventInteriorUseInfo::IsValid (vtable 0x8701A8, ranura 14): con ped: call 5E0170 (IsAlive, ecx = ped) y su AL; sin ped: byte [this+8] != 0 -> 1; si no, call [vtable+0Ch] (GetLifeTime) y [this+4] <= eax (jle) -> 1, si no 0; ret 4
  14  0x4B1680  idem (codigo muerto: sin llamadores ni ranura de vtable)
  15  0x4CEFA0  CAnimBlendAssociation (vtable 0x85C6D0) destructor con borrado: push esi; mov esi,ecx; call 4CECF0; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
  16  0x6846D0  CTaskComplexChat (vtable 0x8706A0): cuerpo 682D40; CTask::operator delete 61A5B0
  17  0x65A620  CTaskComplexUseWaterCannon (vtable 0x86F5D0): cuerpo 6597B0
  18  0x664480  CTaskComplexStareAtPed (vtable 0x86FA2C): cuerpo 660310
  19  0x6644A0  CTaskSimpleDoHandSignal (vtable 0x86FA68): cuerpo 6608B0
  20  0x4B5670  CEventEditableResponse (vtable 0x85AB80): push esi; call 4AC480; test byte [esp+8],1; je +; (liberacion del pool de eventos en linea con ECX = this tras la llamada); mov eax,esi; pop esi; ret 4
  21  0x4CECF0  CAnimBlendAssociation cuerpo: eax=[esi+10h]; [esi]=85C6D0h; si eax: call 72F4F0 (FreeAlign, cdecl); eax=[esi+8]; si eax: [eax]=[esi+4]; eax=[esi+4]; si eax: [eax+4]=[esi+8]; [esi+4]=0; [esi+8]=0;
            si bit 14 de la palabra [esi+2Eh]: call 4D3FD0 (RemoveAnimBlockRef, cdecl, id = [[esi+14h]+0Ch]); ret
  22  0x6594D0  CTaskComplexUseSwatRope cuerpo (SEH): [esi]=86F59Ch; si bit 0 de [esi+14h] y [esi+18h]: call 571A00 (CleanUpOldReference, ecx = [esi+18h], arg = esi+18h); call 61A3D0 (~CTaskComplex)
  23  0x684320  CTaskComplexPartnerChat cuerpo (SEH): [esi]=8707C4h; si byte [esi+74h] y byte [esi+76h]: call 4E52A0 (ReleasePedConversation, cdecl); call 683A40

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor; deja un dword del caso en [ECX]); operator delete y FreeAlign
[direccion, puntero (etiqueta), dword del puntero] (operator delete) o [direccion, puntero] (FreeAlign); RemoveAnimBlockRef [direccion, id]; CleanUpOldReference [direccion, ECX, argumento (etiquetas)];
ReleasePedConversation [direccion]; IsAlive [direccion, ECX]; GetLifeTime falso [identificador, ECX]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CEventStuckInAir::GetEventType', 'CEventStuckInAir::GetLifeTime', 'CEventStuckInAir::CanBeInterruptedBySameEvent', 'CTaskGangHasslePed::GetTaskType', 'CColStore::FindColSlot',
         'CRideAnimData::Destructor', 'CRideAnimData::DefaultConstructor', 'CAETwinLoopSoundEntity::DefaultConstructor', 'CTrailer::IsBaggageTrailer', 'CVector::Sum', 'CColDisk::Set',
         'CAEVehicleAudioEntity::UpdateVehicleEngineSoundExe', 'CEvent::operator delete', 'CEventInteriorUseInfo::IsValid', 'CEvent::IsValidAliveOrBase',
         'CAnimBlendAssociation::ScalarDeletingDestructor', 'CTaskComplexChat::ScalarDeletingDestructor', 'CTaskComplexUseWaterCannon::ScalarDeletingDestructor',
         'CTaskComplexStareAtPed::ScalarDeletingDestructor', 'CTaskSimpleDoHandSignal::ScalarDeletingDestructor', 'CEventEditableResponse::ScalarDeletingDestructor',
         'CAnimBlendAssociation::DestructorBody', 'CTaskComplexUseSwatRope::DestructorBody', 'CTaskComplexPartnerChat::DestructorBody']
ENTRIES = (0x4B14F0, 0x4B1500, 0x4B1510, 0x65FF50, 0x410390, 0x6D0B30, 0x6D0B10, 0x4F63B0, 0x6D0AF0, 0x40FDD0, 0x40FD50, 0x4F56D0, 0x4B5630, 0x4B1640, 0x4B1680,
           0x4CEFA0, 0x6846D0, 0x65A620, 0x664480, 0x6644A0, 0x4B5670, 0x4CECF0, 0x6594D0, 0x684320)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
FREEALIGN, REMOVEREF, CLEANUP, RELEASECONV, ISALIVE, EV_LIFE = 0x72F4F0, 0x4D3FD0, 0x571A00, 0x4E52A0, 0x5E0170, 0x7E1EA5E0

CONST, CONSTAL, NOOP, RIDECTOR, TWINCTOR, TRAILER, SUM, DISKSET, ENGINE, POOLDELETE, ISVALID, SDD, SDDPOOL, ANIMBODY, SWATBODY, PARTNERBODY = 'const constal noop ridector twinctor trailer sum diskset engine pooldelete isvalid sdd sddpool animbody swatbody partnerbody'.split()
KIND = [CONST, CONST, CONSTAL, CONST, CONST, NOOP, RIDECTOR, TWINCTOR, TRAILER, SUM, DISKSET, ENGINE, POOLDELETE, ISVALID, ISVALID, SDD, SDD, SDD, SDD, SDD, SDDPOOL, ANIMBODY, SWATBODY, PARTNERBODY]
CONSTANT = {0: 0x5D, 1: 0, 2: 1, 3: 0x4BC, 4: 0xFFFFFFFF}
# SDD: direccion (EXE) del cuerpo que llama y de la liberacion
BODY = {15: 0x4CECF0, 16: 0x682D40, 17: 0x6597B0, 18: 0x660310, 19: 0x6608B0, 20: 0x4AC480}
DELETE = {15: 0x8214BD, 16: 0x61A5B0, 17: 0x61A5B0, 18: 0x61A5B0, 19: 0x61A5B0}
KALIAS = (0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20)
SPAN = {RIDECTOR: 0x1C, TWINCTOR: 0xA8, TRAILER: 0x9F0, SUM: 0x20, DISKSET: 0x24, ENGINE: 0x1B0, ISVALID: 0x10, ANIMBODY: 0x30, SWATBODY: 0x1C, PARTNERBODY: 0x78}


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


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


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
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

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


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con fracciones exactas
def f32_class(bits):
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return ('nan', bits) if m else ('inf', s)
    if e == 0:
        v = Fraction(m, 1 << 149)
    else:
        v = Fraction((m | 0x800000) << e, 1 << 150)
    return ('fin', -v if s else v, s)


def _exp2floor(m):
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    return e


def round_sig(fr, bits):
    """redondeo al par mas cercano de `fr` a `bits` bits de mantisa, sin limite de exponente"""
    if fr == 0:
        return fr
    sign = -1 if fr < 0 else 1
    m = abs(fr)
    e = _exp2floor(m)
    unit = Fraction(2) ** (e - (bits - 1))
    q = m / unit
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
        n += 1
    return sign * n * unit


def to_f32_bits(fr, zero_sign=0):
    """float de 32 bits (como fstp m32): redondeo al par, denormales y desbordamiento a infinito"""
    if fr == 0:
        return zero_sign << 31
    s = 1 if fr < 0 else 0
    m = abs(fr)
    e = _exp2floor(m)
    if e < -126:
        q = m * (1 << 149)                      # cuanto 2^-149
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
            n += 1
        return (s << 31) | n                     # n = 2^23 da el menor normal (exponente 1, mantisa 0)
    q = m / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
        n += 1
    if n == 1 << 24:
        n >>= 1
        e += 1
    if e > 127:
        return (s << 31) | 0x7F800000
    return (s << 31) | ((e + 127) << 23) | (n & 0x7FFFFF)


def x87_center(hi_bits, lo_bits):
    """fld [hi]; fadd [lo]; fmul 0.5; fstp float, con la x87 a 24 bits de mantisa (exponente ampliado). A lo sumo un operando NaN. Devuelve (bits, marcas)"""
    a, b = f32_class(hi_bits), f32_class(lo_bits)
    marks = []
    if a[0] == 'nan' or b[0] == 'nan':
        if a[0] == 'nan' and b[0] == 'nan':
            raise ValueError('dos NaN: fuera del modelo')
        nb = (a if a[0] == 'nan' else b)[1]
        marks.append('nan_operand')
        return (nb & 0x80000000) | 0x7FC00000 | (nb & 0x7FFFFF), marks   # QNaN con la misma carga y signo
    if a[0] == 'inf' or b[0] == 'inf':
        if a[0] == 'inf' and b[0] == 'inf' and a[1] != b[1]:
            marks.append('inf_minus_inf')
            return 0xFFC00000, marks                                      # indefinido real
        marks.append('inf_operand')
        s = a[1] if a[0] == 'inf' else b[1]
        return (s << 31) | 0x7F800000, marks
    s_sum = a[1] + b[1]
    if s_sum == 0:
        # suma exacta cero: +0 salvo (-0) + (-0)
        zs = 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0
        marks.append('sum_zero')
        return zs << 31, marks
    total = round_sig(s_sum, 24)                   # PC = 24 bits
    if abs(s_sum) >= (1 << 128):
        marks.append('sum_beyond_float_range')
    half = total / 2                              # fmul 0.5: exacto en la x87
    bits = to_f32_bits(half)
    e = (bits >> 23) & 0xFF
    if e == 0xFF:
        marks.append('result_inf')
    elif e == 0:
        marks.append('result_denormal_or_zero')
    return bits, marks


def x87_add(st0_bits, mem_bits):
    """fld [st0]; fadd [mem]; fstp float, con la x87 a 24 bits de mantisa (exponente ampliado). A lo sumo un operando NaN. Devuelve (bits, marcas)"""
    a, b = f32_class(st0_bits), f32_class(mem_bits)
    marks = []
    if a[0] == 'nan' or b[0] == 'nan':
        if a[0] == 'nan' and b[0] == 'nan':
            raise ValueError('dos NaN: fuera del modelo')
        nb = (a if a[0] == 'nan' else b)[1]
        marks.append('nan_operand')
        return (nb & 0x80000000) | 0x7FC00000 | (nb & 0x7FFFFF), marks   # QNaN con la misma carga y signo
    if a[0] == 'inf' or b[0] == 'inf':
        if a[0] == 'inf' and b[0] == 'inf' and a[1] != b[1]:
            marks.append('inf_minus_inf')
            return 0xFFC00000, marks                                      # indefinido real
        marks.append('inf_operand')
        s = a[1] if a[0] == 'inf' else b[1]
        return (s << 31) | 0x7F800000, marks
    s_sum = a[1] + b[1]
    if s_sum == 0:
        zs = 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0       # suma exacta cero: +0 salvo (-0) + (-0)
        marks.append('sum_zero')
        return zs << 31, marks
    total = round_sig(s_sum, 24)                   # PC = 24 bits
    if abs(s_sum) >= (1 << 128):
        marks.append('sum_beyond_float_range')
    bits = to_f32_bits(total)
    e = (bits >> 23) & 0xFF
    if e == 0xFF:
        marks.append('result_inf')
    elif e == 0:
        marks.append('result_denormal_or_zero')
    return bits, marks


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def ctrunc_div(a, b):
    q = abs(a) // abs(b)
    return q if (a >= 0) == (b > 0) else -q


KEYS = {}
for _fn in range(24):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (CONST, CONSTAL, NOOP):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    elif _kind in (RIDECTOR, TWINCTOR):
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other']
    elif _kind == TRAILER:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'baggage_true', 'baggage_false']
    elif _kind == SUM:
        _k += ['out_in_b', 'out_alias', 'nan_operand', 'inf_operand', 'sum_zero', 'sum_beyond_float_range', 'result_denormal_or_zero']
    elif _kind == DISKSET:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'start_in_b', 'start_alias', 'end_in_c', 'end_alias', 'radius_nan']
    elif _kind == ENGINE:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'sound_null', 'sound_set', 'type_negative', 'type_positive', 'nan_operand']
    elif _kind == POOLDELETE:
        _k += ['index_negative', 'index_zero', 'index_positive', 'cursor_lowered', 'cursor_kept']
    elif _kind == ISVALID:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'ped_set', 'ped_null', 'valid_flag_set', 'valid_flag_clear', 'life_enough', 'life_expired']
    elif _kind == SDD:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == SDDPOOL:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only',
               'index_negative', 'index_zero', 'index_positive', 'cursor_lowered', 'cursor_kept']
    elif _kind == ANIMBODY:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'nodes_set', 'nodes_null', 'next_null', 'next_set', 'prev_null', 'prev_set', 'ref_flag_set', 'ref_flag_clear', 'hier_self', 'link_self']
    elif _kind == SWATBODY:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'on_heli_bit', 'on_heli_clear', 'heli_set', 'heli_null', 'cleanup_called']
    elif _kind == PARTNERBODY:
        _k += ['this_aligned', 'this_unaligned', 'spill', 'conversation_released', 'conversation_skipped', 'enabled_only', 'loaded_only']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    if k not in (SUM, POOLDELETE):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        if a[0] + SPAN.get(k, 4) > WIN:
            marks.append('spill')
    return marks


def pool_free(W, A, B, Cc, obj, d, cursor, cell_c, pool_b, marks):
    """liberacion de la ranura en el pool falso: indice = (obj - almacenamiento) / 0x44 con signo; estados[indice] |= 0x80; cursor = min(cursor, indice) con signo"""
    idx = ctrunc_div(s32(d), 68)
    # el pool falso: +0 almacenamiento = obj - d, +4 estados = celda C - indice (la celda de estados[indice] cae en C), +0xC cursor
    flag = Cc.rd(cell_c, 1)
    Cc.wr(cell_c, 1, flag | 0x80)
    if idx < s32(cursor):
        B.wr(pool_b + 0xC, 4, idx & M32)
        marks.append('cursor_lowered')
    else:
        marks.append('cursor_kept')
    marks.append('index_negative' if idx < 0 else 'index_zero' if idx == 0 else 'index_positive')


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k in (CONST, CONSTAL, NOOP):
        A.wr(o, 4, a[3])
        W.snap()
        ret = None if k == NOOP else CONSTANT[fn]
    elif k == RIDECTOR:
        A.wr(o, 4, a[3])
        W.snap()
        for i in range(7):
            A.wr(o + 4 * i, 4, 0)
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == TWINCTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o + 0x10, 4, 0)
        A.wr(o, 4, 0x85F438)
        A.wr(o + 0x88, 2, 0)
        A.wr(o + 0xA0, 4, 0)
        A.wr(o + 0xA4, 4, 0)
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == TRAILER:
        A.wr(o + 0x9EC, 4, a[3])
        W.snap()
        ret = 1 if a[3] == 0xC47A0000 else 0
        marks.append('baggage_true' if ret else 'baggage_false')
    elif k == SUM:
        mode = a[9]
        left = W.raw(0, o)
        for i in range(3):
            A.wr(o + 4 * i, 4, a[3 + i])
            Cc.wr(a[16] + 4 * i, 4, a[6 + i])
        right = left if mode in (6, 7) else W.raw(2, a[16])
        out = {0: W.raw(1, a[10]), 1: left, 2: right, 3: left + 4, 4: left + 8, 5: right + 4, 6: W.raw(1, a[10]), 7: left}[mode]
        W.snap()
        marks.append('out_in_b' if mode in (0, 6) else 'out_alias')
        for i in range(3):
            bits, mk = x87_add(W.rd(left + 4 * i, 4), W.rd(right + 4 * i, 4))
            marks += mk
            W.wr(out + 4 * i, 4, bits)
    elif k == DISKSET:
        this = W.raw(0, o)
        sm, em = a[8], a[9]
        start = W.raw(1, a[10]) if sm == 0 else W.raw(0, o + KALIAS[sm - 1])
        end = W.raw(2, a[11]) if em == 0 else W.raw(0, o + KALIAS[em - 1])
        W.snap()
        marks.append('start_in_b' if sm == 0 else 'start_alias')
        marks.append('end_in_c' if em == 0 else 'end_alias')
        marks.append('radius_nan' if (a[3] & 0x7F800000) == 0x7F800000 and a[3] & 0x7FFFFF else 'radius_ok')
        W.wr(this + 0xC, 4, a[3])
        for i in range(3):
            W.wr(this + 4 * i, 4, W.rd(start + 4 * i, 4))
        for i in range(3):
            W.wr(this + 0x14 + 4 * i, 4, W.rd(end + 4 * i, 4))
        W.wr(this + 0x20, 4, a[4])
        W.wr(this + 0x10, 1, a[5] & 0xFF)
        W.wr(this + 0x11, 1, a[6] & 0xFF)
        W.wr(this + 0x12, 1, a[7] & 0xFF)
    elif k == ENGINE:
        st = a[3] & 0xFFFF
        st = st - 0x10000 if st & 0x8000 else st
        A.wr(o + 0xD8, 4, a[6])
        cell = o + 0xE8 + 8 * st
        snd = W.raw(1, a[8]) if a[7] else 0
        A.wr(cell, 4, snd)
        W.snap()
        marks.append('sound_set' if a[7] else 'sound_null')
        marks.append('type_negative' if st < 0 else 'type_positive')
        ev = A.rd(o + 0xD8, 4)
        if snd:
            bits, mk = x87_add(a[5], ev)
            marks += [m for m in mk if m == 'nan_operand']
            W.wr(snd + 0x14, 4, bits)
            W.wr(snd + 0x1C, 4, a[4])
    elif k == POOLDELETE:
        obj = W.raw(0, o)
        pool = a[6]
        d, cursor, cell = a[3], a[4], a[5]
        B.wr(pool + 0, 4, (obj - d) & M32)
        B.wr(pool + 4, 4, (W.raw(2, cell) - ctrunc_div(s32(d), 68)) & M32)
        B.wr(pool + 8, 4, a[7])
        B.wr(pool + 0xC, 4, cursor)
        W.snap()
        pool_free(W, A, B, Cc, obj, d, cursor, cell, pool, marks)
    elif k == ISVALID:
        A.wr(o + 4, 4, a[4])
        A.wr(o + 8, 4, a[5])
        W.snap()
        if a[6]:
            events.append([ISALIVE, W.tagv(W.raw(1, a[7]))])
            ret = a[8] & 0xFF
            marks += ['ped_set']
        else:
            marks.append('ped_null')
            if a[5] & 0xFF:
                ret = 1
                marks.append('valid_flag_set')
            else:
                marks.append('valid_flag_clear')
                events.append([EV_LIFE, tag(0, o)])
                ret = 1 if s32(a[4]) <= s32(a[3]) else 0
                marks.append('life_enough' if ret else 'life_expired')
    elif k in (SDD, SDDPOOL):
        A.wr(o, 4, a[3])
        if k == SDDPOOL:       # pool falso (lo escribe Setup antes de la instantanea): +0 almacenamiento = this - d, +4 estados (celda en C - indice), +8 relleno, +0xC cursor
            obj = W.raw(0, o)
            d, cursor, cell, pool = a[7], a[8], a[9], a[10]
            B.wr(pool + 0, 4, (obj - d) & M32)
            B.wr(pool + 4, 4, (W.raw(2, cell) - ctrunc_div(s32(d), 68)) & M32)
            B.wr(pool + 8, 4, a[11])
            B.wr(pool + 0xC, 4, cursor)
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            if k == SDD:
                events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
            else:
                pool_free(W, A, B, Cc, W.raw(0, o), a[7], a[8], a[9], a[10], marks)
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == ANIMBODY:
        self_ptr = W.raw(0, o)
        nodes = W.raw(1, a[6]) if a[5] else 0
        nxt = {0: 0, 1: W.raw(2, a[8]), 2: self_ptr, 3: self_ptr + 4}[a[7]]
        prv = {0: 0, 1: W.raw(2, a[10]), 2: self_ptr, 3: self_ptr + 4}[a[9]]
        hier = self_ptr if a[16] else W.raw(1, a[17])
        A.wr(o, 4, a[3])
        A.wr(o + 4, 4, nxt)
        A.wr(o + 8, 4, prv)
        A.wr(o + 0x10, 4, nodes)
        A.wr(o + 0x14, 4, hier)
        A.wr(o + 0x2C, 4, a[11])
        W.wr(hier + 0xC, 4, a[18])
        W.snap()
        marks.append('nodes_set' if nodes else 'nodes_null')
        marks.append('next_set' if nxt else 'next_null')
        marks.append('prev_set' if prv else 'prev_null')
        marks.append('hier_self' if a[16] else 'hier_other')
        if a[7] >= 2 or a[9] >= 2:
            marks.append('link_self')
        nodes_now = A.rd(o + 0x10, 4)
        A.wr(o, 4, 0x85C6D0)
        if nodes_now:
            events.append([FREEALIGN, W.tagv(nodes_now)])
        p = A.rd(o + 8, 4)
        if p:
            W.wr(p, 4, A.rd(o + 4, 4))
        n = A.rd(o + 4, 4)
        if n:
            W.wr(n + 4, 4, A.rd(o + 8, 4))
        A.wr(o + 4, 4, 0)
        A.wr(o + 8, 4, 0)
        if (A.rd(o + 0x2E, 2) >> 14) & 1:
            h = A.rd(o + 0x14, 4)
            events.append([REMOVEREF, W.rd(h + 0xC, 4)])
            marks.append('ref_flag_set')
        else:
            marks.append('ref_flag_clear')
    elif k == SWATBODY:
        A.wr(o, 4, a[3])
        A.wr(o + 0x14, 4, a[6])
        heli = W.raw(1, a[8]) if a[7] else 0
        A.wr(o + 0x18, 4, heli)
        W.snap()
        marks.append('on_heli_bit' if a[6] & 1 else 'on_heli_clear')
        marks.append('heli_set' if heli else 'heli_null')
        A.wr(o, 4, 0x86F59C)
        if (a[6] & 1) and heli:
            events.append([CLEANUP, W.tagv(heli), tag(0, o + 0x18)])
            marks.append('cleanup_called')
        events.append([0x61A3D0, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == PARTNERBODY:
        A.wr(o, 4, a[3])
        A.wr(o + 0x74, 4, a[6])
        W.snap()
        en, ld = a[6] & 0xFF, (a[6] >> 16) & 0xFF
        A.wr(o, 4, 0x8707C4)
        if en and ld:
            events.append([RELEASECONV])
            marks.append('conversation_released')
        else:
            marks.append('conversation_skipped')
        if en and not ld:
            marks.append('enabled_only')
        if ld and not en:
            marks.append('loaded_only')
        events.append([0x683A40, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
