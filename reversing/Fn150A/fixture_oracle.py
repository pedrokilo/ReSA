"""Oraculo independiente (Python) de Fn150A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 21 funciones del lote 150A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (fracciones exactas) de CEntryExit::GetPosition.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable). 0x4F21C0 (CAEStreamingChannel::Stop, ranura 7) ya tenia hook y no entra en el lote.
   0  0x4F27F0  CAEStreamingChannel (vtable 0x85F3F0) destructor con borrado: push esi; mov esi,ecx; call 4F2200; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x4C4AF0  CBaseModelInfo (vtable 0x85BB9C) destructor con borrado: test byte [esp+4],1; push esi; mov esi,ecx; mov [esi],85BB9Ch; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   2  0x4C4C00  CColModel (sin vtable ni llamadores) destructor con borrado: call 40F700; test flags; push esi; call 40FC40 (CColModel::operator delete); ...
   3  0x5329A0  CPtrNodeDoubleLink::AddToList: mov eax,[esp+4]; mov [ecx+8],0; mov edx,[eax]; mov [ecx+4],edx; mov edx,[eax]; test edx,edx; je +; mov [edx+8],ecx; mov [eax],ecx; ret 4
   4  0x404350  CVector::CVector(): mov eax,ecx; ret
   5  0x4043E0  CEntity::GetStatus: movzx eax,byte [ecx+36h]; shr eax,3; ret
   6  0x404420  CEntity::GetModelIndex: movsx eax,word [ecx+22h]; ret
   7  0x404590  CQuadTreeNode (sin vtable ni llamadores) destructor con borrado: call 552520; test flags; push esi; call 552C90; add esp,4; ...
   8  0x43E090  CEntryExit::GetPosition: fld [ecx+10h]; mov eax,[esp+4]; fadd [ecx+8]; fmul [858B8Ch=0.5]; fstp [eax]; fld [ecx+14h]; fadd [ecx+0Ch]; fmul [0.5]; fstp [eax+4]; mov ecx,[ecx+18h]; mov [eax+8],ecx; ret 4
   9  0x61BA80  CTaskSimpleAnim (vtable 0x86D504): cuerpo 61A6F0, CTask::operator delete 61A5B0
  10  0x61BAA0  CTaskSimpleRunAnim (vtable 0x86D528): cuerpo 48DE60
  11  0x61BEF0  CTaskSimpleRunNamedAnim (vtable 0x86D54C): cuerpo 61BF10
  12  0x64B440  CTaskSimpleCarWaitToSlowDown (vtable 0x86ED98): cuerpo 646A00
  13  0x64B460  CTaskSimpleCarWaitForDoorNotToBeInUse (vtable 0x86EDBC): cuerpo 646C00
  14  0x64B480  CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse (vtable 0x86EDE0): cuerpo 646E50
  15  0x448650  mov al,[96BFE4h]; ret
  16  0x448660  CGarages::GivePlayerDetonator: push -1; call 56E210; push 1; push 28h; call 743C60; (eax+14h = ranura); push 1; push 1; push 28h; call 5E6080 (GiveWeapon, thiscall);
            [ped+ranura*1Ch+5A4h]=0; [[ped+480h]+20h]=bl; si [ped+70Ch]!=37h: [ped+70Ch]=28h
  17  0x658890  CTaskSimpleGiveCPR::GetTaskType: mov eax,66h; ret
  18  0x6588A0  CTaskSimpleGiveCPR cuerpo del destructor: [esi]=86F4C8h; si [esi+10h]: push 0; push 4D1390h; call 4CEBE0 (SetFinishCallback, thiscall, ret 8); call 61A3A0 (~CTask: mov [ecx],86D48Ch)
  19  0x653080  CTaskSimpleHitHead::GetTaskType: mov eax,1F4h; ret
  20  0x653090  CTaskSimpleHitHead cuerpo del destructor: [esi]=86F1D0h; si [esi+0Ch]: SetFinishCallback(4D1390h, 0); call 61A3A0

Los falsos de los colaboradores anotan [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos de destructor y ~CTask; deja un dword del caso en [ECX]); operator delete y las liberaciones anotan
[direccion, puntero (etiqueta), dword del puntero] (prueba que se borra DESPUES de destruir); SetFinishCallback [direccion, ECX, callback, dato]; FindPlayerPed [direccion, argumento]; GetWeaponInfo [direccion, tipo,
habilidad]; GiveWeapon [direccion, ECX, tipo, municion, byte bajo de la comprobacion]. Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CAEStreamingChannel::ScalarDeletingDestructor', 'CBaseModelInfo::ScalarDeletingDestructor', 'CColModel::ScalarDeletingDestructor', 'CPtrNodeDoubleLink::AddToList',
         'CVector::DefaultConstructor', 'CEntity::GetStatus', 'CEntity::GetModelIndex', 'CQuadTreeNode::ScalarDeletingDestructor', 'CEntryExit::GetPosition',
         'CTaskSimpleAnim::ScalarDeletingDestructor', 'CTaskSimpleRunAnim::ScalarDeletingDestructor', 'CTaskSimpleRunNamedAnim::ScalarDeletingDestructor',
         'CTaskSimpleCarWaitToSlowDown::ScalarDeletingDestructor', 'CTaskSimpleCarWaitForDoorNotToBeInUse::ScalarDeletingDestructor',
         'CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse::ScalarDeletingDestructor', 'CGarages::CameraShouldBeOutside', 'CGarages::GivePlayerDetonator',
         'CTaskSimpleGiveCPR::GetTaskType', 'CTaskSimpleGiveCPR::DestructorBody', 'CTaskSimpleHitHead::GetTaskType', 'CTaskSimpleHitHead::DestructorBody']
ENTRIES = (0x4F27F0, 0x4C4AF0, 0x4C4C00, 0x5329A0, 0x404350, 0x4043E0, 0x404420, 0x404590, 0x43E090, 0x61BA80, 0x61BAA0, 0x61BEF0, 0x64B440, 0x64B460, 0x64B480,
           0x448650, 0x448660, 0x658890, 0x6588A0, 0x653080, 0x653090)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETFINISH, FINDPED, WEAPINFO, GIVEWEAPON = 0x4CEBE0, 0x56E210, 0x743C60, 0x5E6080
EXE_DEFAULT_ANIM_CB = 0x4D1390
TASK_DTOR = 0x61A3A0

# forma de cada funcion
SDD, ADDLIST, CTOR, STATUS, MODELIDX, POS, BOOL, DETONATOR, CONST, ANIMBODY = 'sdd addlist ctor status modelidx pos bool detonator const animbody'.split()
KIND = [SDD, SDD, SDD, ADDLIST, CTOR, STATUS, MODELIDX, SDD, POS, SDD, SDD, SDD, SDD, SDD, SDD, BOOL, DETONATOR, CONST, ANIMBODY, CONST, ANIMBODY]
# SDD: direccion (EXE) del cuerpo que llama (0 si ninguno) y de la liberacion
BODY = {0: 0x4F2200, 2: 0x40F700, 7: 0x552520, 9: 0x61A6F0, 10: 0x48DE60, 11: 0x61BF10, 12: 0x646A00, 13: 0x646C00, 14: 0x646E50}
DELETE = {0: 0x8214BD, 1: 0x8214BD, 2: 0x40FC40, 7: 0x552C90, 9: 0x61A5B0, 10: 0x61A5B0, 11: 0x61A5B0, 12: 0x61A5B0, 13: 0x61A5B0, 14: 0x61A5B0}
CBASEMODELINFO_VTABLE = 0x85BB9C
CONSTANT = {17: 0x66, 19: 0x1F4}
# cuerpos con SEH: vtable de la clase, desplazamiento de la animacion
ANIMBODY_INFO = {18: (0x86F4C8, 0x10), 20: (0x86F1D0, 0x0C)}
POS_ALIAS = (0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20)
GLOBAL_BOOL = 0x96BFE4


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


KEYS = {}
for _fn in range(21):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind in (SDD, CTOR, ANIMBODY, STATUS, MODELIDX, CONST):
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == ANIMBODY:
        _k += ['prev_zero', 'prev_other', 'anim_null', 'anim_set']
    elif _kind == ADDLIST:
        _k += ['head_null', 'head_first', 'head_self', 'head_in_node_next', 'head_in_node_prev', 'node_unaligned']
    elif _kind == STATUS:
        _k += ['status_low', 'status_high']
    elif _kind == MODELIDX:
        _k += ['word_high_set', 'word_high_clear']
    elif _kind == POS:
        _k += ['out_in_b', 'out_alias', 'nan_operand', 'inf_operand', 'sum_zero', 'sum_beyond_float_range', 'result_denormal_or_zero']
    elif _kind == BOOL:
        _k += ['bool_false', 'bool_true']
    elif _kind == DETONATOR:
        _k += ['slot_detonator', 'slot_negative', 'slot_other', 'saved_unidentified', 'saved_other']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if KIND[fn] in (SDD, CTOR, ANIMBODY, STATUS, MODELIDX, CONST):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        span = {STATUS: 0x38, MODELIDX: 0x24, ANIMBODY: 0x14 if fn == 18 else 0x10}.get(KIND[fn], 4)
        if a[0] + span > WIN:
            marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        if fn == 1:                                       # vtable en linea de CBaseModelInfo; sin cuerpo
            A.wr(o, 4, CBASEMODELINFO_VTABLE)
        else:
            events.append([BODY[fn], tag(0, o), dword_in])
            A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE[fn], tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CTOR:
        A.wr(o, 4, a[3])
        W.snap()
        ret = tag(0, o)
    elif k == ADDLIST:
        node, first = W.raw(0, o), W.raw(2, a[8])
        A.wr(o, 4, a[3]); A.wr(o + 4, 4, a[4]); A.wr(o + 8, 4, a[5])
        Cc.wr(a[8], 4, a[11]); Cc.wr(a[8] + 4, 4, a[10]); Cc.wr(a[8] + 8, 4, a[9])
        mode = a[6]
        if mode == 0:
            B.wr(a[7], 4, 0); head = W.raw(1, a[7]); marks.append('head_null')
        elif mode == 1:
            B.wr(a[7], 4, first); head = W.raw(1, a[7]); marks.append('head_first')
        elif mode == 2:
            B.wr(a[7], 4, node); head = W.raw(1, a[7]); marks.append('head_self')
        elif mode == 3:
            A.wr(o + 4, 4, first); head = W.raw(0, o + 4); marks.append('head_in_node_next')
        else:
            A.wr(o + 8, 4, first); head = W.raw(0, o + 8); marks.append('head_in_node_prev')
        if o % 4:
            marks.append('node_unaligned')
        W.snap()
        # EXE: [nodo+8]=0; edx=*cabeza; [nodo+4]=edx; edx=*cabeza (otra vez); si edx: [edx+8]=nodo; *cabeza=nodo
        W.wr(node + 8, 4, 0)
        W.wr(node + 4, 4, W.rd(head, 4))
        again = W.rd(head, 4)
        if again:
            W.wr(again + 8, 4, node)
        W.wr(head, 4, node)
    elif k == STATUS:
        A.wr(o + 0x34, 4, a[3])
        W.snap()
        ret = A.rd(o + 0x36, 1) >> 3
        marks.append('status_high' if ret >= 16 else 'status_low')
    elif k == MODELIDX:
        A.wr(o + 0x20, 4, a[3])
        W.snap()
        w = A.rd(o + 0x22, 2)
        ret = (w | 0xFFFF0000) if w & 0x8000 else w
        marks.append('word_high_set' if w & 0x8000 else 'word_high_clear')
    elif k == POS:
        for i in range(5):
            A.wr(o + 8 + 4 * i, 4, a[3 + i])
        mode = a[8]
        out = W.raw(1, a[9]) if mode == 0 else W.raw(0, o + POS_ALIAS[mode - 1])
        W.snap()
        marks.append('out_in_b' if mode == 0 else 'out_alias')
        this = W.raw(0, o)
        # fld [+10h]; fadd [+8]; fmul 0.5; fstp [out]
        bits, mk = x87_center(W.rd(this + 0x10, 4), W.rd(this + 0x08, 4))
        marks += mk
        W.wr(out, 4, bits)
        # fld [+14h]; fadd [+0Ch]; fmul 0.5; fstp [out+4]  (se leen despues de escribir el primero)
        bits, mk = x87_center(W.rd(this + 0x14, 4), W.rd(this + 0x0C, 4))
        marks += mk
        W.wr(out + 4, 4, bits)
        # mov ecx,[this+18h]; mov [out+8],ecx
        W.wr(out + 8, 4, W.rd(this + 0x18, 4))
        ret = W.tagv(out)
    elif k == BOOL:
        W.snap()
        ret = a[3] & 0xFF
        marks.append('bool_true' if ret else 'bool_false')
    elif k == DETONATOR:
        slot = a[3] - (1 << 32) if a[3] & 0x80000000 else a[3]
        A.wr(o + 0x480, 4, W.raw(1, a[4]))
        A.wr(o + 0x70C, 4, a[5])
        Cc.wr(a[6] + 0x14, 4, a[3])
        W.snap()
        events.append([FINDPED, 0xFFFFFFFF])
        events.append([WEAPINFO, 0x28, 1])
        events.append([GIVEWEAPON, tag(0, o), 0x28, 1, 1])
        A.wr(o + 0x5A4 + slot * 0x1C, 4, 0)
        B.wr(a[4] + 0x20, 1, a[3] & 0xFF)
        if a[5] != 0x37:
            A.wr(o + 0x70C, 4, 0x28)
        marks.append('slot_detonator' if slot == 12 else 'slot_negative' if slot < 0 else 'slot_other')
        marks.append('saved_unidentified' if a[5] == 0x37 else 'saved_other')
    elif k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k == ANIMBODY:
        vt, aoff = ANIMBODY_INFO[fn]
        A.wr(o, 4, a[3]); A.wr(o + aoff, 4, a[6])
        W.snap()
        A.wr(o, 4, vt)
        if a[6]:
            events.append([SETFINISH, W.tagv(a[6]), EXE_DEFAULT_ANIM_CB, 0])
        events.append([TASK_DTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append('anim_set' if a[6] else 'anim_null')
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
