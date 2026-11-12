"""Oraculo independiente (Python) de Fn170A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 22 funciones del lote 170A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x659390  CTaskComplexDriveFireTruck::GetTaskType (vtable 0x86F570, ranura 4): mov eax,6Bh
   1  0x6593A0  CTaskComplexDriveFireTruck cuerpo del destructor: [this] = 86F570h; si [this+0Ch]: push this+0Ch; call 571A00 (ecx = vehiculo); si [this+14h]: push this+14h; call 571A00 (ecx = companero); ecx = this; call 61A3D0
   2  0x659410  CTaskComplexUseSwatRope constructor sin helicoptero: push esi; mov esi,ecx; call 61A3B0; eax = [esp+8]; cl = [esi+14h]; [esi+0Ch] = eax; eax = 0; [esi+10h] = 0; cl &= FEh; [esi+18h] = 0; [esi] = 86F59Ch;
             [esi+14h] = cl; eax = esi; ret 4
   3  0x659440  CTaskComplexUseSwatRope::GetTaskType (vtable 0x86F59C, ranura 4): mov eax,6Ch
   4  0x659450  CTaskComplexUseSwatRope::IsInterruptable (ranura 11): xor al,al; ret 4
   5  0x659460  CTaskComplexUseSwatRope::AbortUrgently (ranura 12): xor al,al; ret 4
   6  0x65A3C0  CTaskComplexUseSwatRope destructor con borrado: push esi; mov esi,ecx; call 6594D0; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   7  0x631990  CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask (thiscall, ret 4, marco SEH): tipo == 0E0h: new 61A5A0(18h); si no nulo: base 61A3B0, [obj] = 86DF98h, +0Ch = +10h = 0, bytes +14h y +15h = 0, devuelve obj;
             tipo == 386h: new 61A5A0(18h); si no nulo: ctor 667E20(obj; rumbo = [this+10h], [86FC7Ch] = 0.5f, [86FC80h] = 0.2f) y devuelve SU EAX; cualquier otro tipo (516h incluido): 0
   8  0x61DE60  CTaskSimpleUseGun constructor (thiscall, ret 1Ch, marco SEH): base 61A390; [+0Fh] = orden (byte); [this] = 86D724h; bytes +8 = 0, +9 = 1, +0Ah..+0Eh = 0; +10h = FFh; +14h = +18h = 0; +1Ch = entidad; si entidad: 571B70
             (ecx = entidad, push this+1Ch); +20h..+28h = vector; +34h (word) = rafaga; +2Ch = +30h = 0; +36h (word) = 0; +38h = FFh; +39h = +3Ah = 0; +3Bh = inmediato (byte); devuelve this
   9  0x624DA0  CTaskSimpleUseGun destructor con borrado: cuerpo 61DF30; delete 61A5B0 si bit 0
  10  0x61C520  CTaskSimpleFight::GetTaskType (vtable 0x86D684, ranura 4): mov eax,3F8h
  11  0x61DB30  CWeaponInfo::GetBaseComboByName (cdecl): ocho comparaciones repe cmpsb con (nombre + terminador): UNARMED -> 4, BBALLBAT -> 8, KNIFE -> 9, GOLFCLUB -> 0Ah, SWORD -> 0Bh, CHAINSAW -> 0Ch, DILDO -> 0Dh,
             y la octava (FLOWERS): si coincide 0Eh, si no 4
  12  0x6726D0  CTaskComplexWanderProstitute destructor con borrado: cuerpo 6726F0; delete 61A5B0 si bit 0
  13  0x6726F0  CTaskComplexWanderProstitute cuerpo del destructor: mov [ecx],86FE84h; jmp 61A3D0
  14  0x61C0D0  CTaskComplexPlayHandSignalAnim destructor con borrado: cuerpo 61BDF0; delete 61A5B0 si bit 0
  15  0x61C150  CWeaponInfo::GetAnimLoopFire (thiscall, ret 4): si el byte [esp+4] != 0: fld dword [ecx+54h]; si no: fld dword [ecx+48h]
  16  0x667720  CPointRoute::Reverse: n = [this] - 1; si n > 0 (con signo): invierte las entradas de 12 bytes (a partir de this+4) con dos indices (i = 0, j = n; mientras i < j)
  17  0x667900  CNodeRoute::Reverse: igual con entradas de 4 bytes
  18  0x6677A0  CPatrolRoute cuerpo del destructor: eh vector destructor iterator (821E02) de 8 elementos de 28h a partir de this+4 con el destructor 46A380 (ret): no escribe nada
  19  0x667980  CEntitySeekPosCalculator cuerpo del destructor (copia): mov dword [ecx],859DC4h
  20  0x6218C0  CTaskSimpleGangDriveBy cuerpo del destructor (marco SEH): [this] = 86D944h; si byte [this+9]: indice = ((([B4EA34h] + 14h * [this+28h]) -> dword) - B5D4A0h) sar 5; push indice; call 4D3FD0; si [this+2Ch]: ecx = anim;
             push 0; push 4D1390h; call 4CEBC0; si [this+34h]: ecx = objetivo; push this+34h; call 571A00; ecx = this; call 61A3A0
  21  0x623750  CTaskComplexKillAllThreats::Clone (marco SEH): edi = [this+0Ch], ebp = [this+10h], ebx = [this+14h]; new 61A5A0(18h); si no nulo: base 61A3B0; [obj+10h] = ebp; [obj+14h] = ebx; [obj] = 86DA1Ch;
             [obj+0Ch] = edi; devuelve obj; si nulo: 0

Los falsos anotan: ~CTask, base y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; CleanUpOldReference y RegisterReference [direccion, ECX, argumento]; operator delete [0x61A5B0, puntero (etiqueta), dword del puntero];
operator new [0x61A5A0, tamano]; el constructor de CTaskSimpleAchieveHeading [0x667E20, ECX, rumbo, velocidad, tolerancia]; RemoveAnimBlockRef [0x4D3FD0, indice]; SetDeleteCallback [0x4CEBC0, ECX, funcion (0x4D1390), datos].
Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ["CTaskComplexDriveFireTruck::GetTaskType",
         "CTaskComplexDriveFireTruck::DestructorBody",
         "CTaskComplexUseSwatRope::ConstructorNoHeli",
         "CTaskComplexUseSwatRope::GetTaskType",
         "CTaskComplexUseSwatRope::IsInterruptable",
         "CTaskComplexUseSwatRope::AbortUrgently",
         "CTaskComplexUseSwatRope::ScalarDeletingDestructor",
         "CTaskComplexObserveTrafficLightsAndAchieveHeading::CreateSubTask",
         "CTaskSimpleUseGun::Constructor",
         "CTaskSimpleUseGun::ScalarDeletingDestructor",
         "CTaskSimpleFight::GetTaskType",
         "CWeaponInfo::GetBaseComboByName",
         "CTaskComplexWanderProstitute::ScalarDeletingDestructor",
         "CTaskComplexWanderProstitute::DestructorBody",
         "CTaskComplexPlayHandSignalAnim::ScalarDeletingDestructor",
         "CWeaponInfo::GetAnimLoopFire",
         "CPointRoute::Reverse",
         "CNodeRoute::Reverse",
         "CPatrolRoute::Destructor",
         "CEntitySeekPosCalculator::DestructorCopy-2",
         "CTaskSimpleGangDriveBy::DestructorBody",
         "CTaskComplexKillAllThreats::Clone"]
ENTRIES = (0x659390, 0x6593A0, 0x659410, 0x659440, 0x659450, 0x659460, 0x65A3C0, 0x631990, 0x61DE60, 0x624DA0, 0x61C520, 0x61DB30, 0x6726D0, 0x6726F0, 0x61C0D0, 0x61C150, 0x667720, 0x667900, 0x6677A0, 0x667980,
           0x6218C0, 0x623750)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CLEANUP, REGREF, BASECTOR_SIMPLE, BASECTOR, BASEDTOR, CTASKDTOR, TASKNEW, TASKDELETE = 0x571A00, 0x571B70, 0x61A390, 0x61A3B0, 0x61A3D0, 0x61A3A0, 0x61A5A0, 0x61A5B0
ACHCTOR, REMOVEREF, SETDELETE, DEFAULT_CB = 0x667E20, 0x4D3FD0, 0x4CEBC0, 0x4D1390
ACH_RATE, ACH_TOL = 0x3F000000, 0x3E4CCCCD       # 0.5f y 0.2f leidos de 0x86FC7C y 0x86FC80 del EXE

# forma de cada funcion
(CONST, BOOL, FTBODY, SWATCTOR, SDD, OBSSUB, GUNCTOR, COMBO, FLOAT, ROUTEPT, ROUTENODE, EMPTY, VTONLY, BODYBASE, GANGBODY, KILLCLONE) = (
    'const bool ftbody swatctor sdd obssub gunctor combo float routept routenode empty vtonly bodybase gangbody killclone'.split())
KIND = [CONST, FTBODY, SWATCTOR, CONST, BOOL, BOOL, SDD, OBSSUB, GUNCTOR, SDD, CONST, COMBO, SDD, BODYBASE, SDD, FLOAT, ROUTEPT, ROUTENODE, EMPTY, VTONLY, GANGBODY, KILLCLONE]
CONSTS = {0: 0x6B, 3: 0x6C, 10: 0x3F8}
SDD_BODY = {6: 0x6594D0, 9: 0x61DF30, 12: 0x6726F0, 14: 0x61BDF0}
COMBO_NAMES = ['UNARMED', 'BBALLBAT', 'KNIFE', 'GOLFCLUB', 'SWORD', 'CHAINSAW', 'DILDO', 'FLOWERS']
COMBO_VALUES = [4, 8, 9, 0xA, 0xB, 0xC, 0xD, 0xE]


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


def f32_to_ext(b):
    """float de 32 bits -> (exponente con signo, mantisa de 64 bits) del x87 tras `fld dword`: los SNaN salen como QNaN, los denormales se normalizan"""
    sign, exp, man = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    if exp == 0xFF:
        if man == 0:
            return (sign << 15) | 0x7FFF, 0x8000000000000000
        return (sign << 15) | 0x7FFF, 0xC000000000000000 | (man << 40)
    if exp == 0:
        if man == 0:
            return sign << 15, 0
        n = man.bit_length()
        return (sign << 15) | ((n - 1) - 149 + 16383), man << (64 - n)
    return (sign << 15) | (exp - 127 + 16383), (0x800000 | man) << 40


def float_class(b):
    exp, man = (b >> 23) & 0xFF, b & 0x7FFFFF
    if exp == 0xFF:
        return 'inf' if man == 0 else ('qnan' if man & 0x400000 else 'snan')
    if exp == 0:
        return 'zero' if man == 0 else 'denormal'
    return 'normal'


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


KEYS = {}
for _fn in range(22):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind in (BOOL, EMPTY, VTONLY, BODYBASE):
        _k += ['prev_zero', 'prev_other']
    elif _kind == FTBODY:
        _k += ['veh_null', 'veh_in_b', 'veh_raw', 'mate_null', 'mate_in_c', 'mate_raw', 'both_refs', 'no_refs', 'driver_zero', 'driver_set']
    elif _kind == SWATCTOR:
        _k += ['rope_zero', 'rope_nonzero', 'byte_bit0_set', 'byte_bit0_clear', 'prev_zero', 'prev_other']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _kind == OBSSUB:
        _k += ['type_e0', 'type_386', 'type_516', 'type_other', 'new_null', 'new_set', 'ach_ret_new', 'ach_ret_raw', 'ach_ret_zero']
    elif _kind == GUNCTOR:
        _k += ['ent_null', 'ent_in_b', 'ent_raw', 'cmd_low_zero_high_set', 'burst_high_bits', 'aim_low_zero_high_set', 'aim_set', 'registered']
    elif _kind == COMBO:
        _k += ['match_' + n for n in COMBO_NAMES] + ['no_match', 'no_terminator', 'empty', 'flowers_default']
    elif _kind == FLOAT:
        _k += ['sel_second', 'sel_first', 'arg_low_zero_high_set', 'cls_normal', 'cls_zero', 'cls_inf', 'cls_qnan', 'cls_snan', 'cls_denormal']
    elif _kind in (ROUTEPT, ROUTENODE):
        _k += ['count_noop', 'count_two', 'count_full', 'count_beyond', 'count_odd', 'count_even']
    elif _kind == GANGBODY:
        _k += ['used', 'not_used', 'assoc_null', 'assoc_in_b', 'assoc_raw', 'target_null', 'target_set', 'idx_negative', 'idx_positive', 'idx_zero']
    elif _kind == KILLCLONE:
        _k += ['new_null', 'new_in_b']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    return ['kind%d' % kind, 'fill%d' % fill, 'this_unaligned' if a[0] % 4 else 'this_aligned']


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret, st0 = [], None, None
    o = a[0]
    this = W.raw(0, o)
    prevmark = lambda v: marks.append('prev_zero' if v == 0 else 'prev_other')
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTS[fn]
    elif k == BOOL:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 0
        prevmark(a[3])
    elif k == EMPTY:
        A.wr(o, 4, a[3])
        W.snap()
        prevmark(a[3])
    elif k == VTONLY:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x859DC4)
        prevmark(a[3])
    elif k == BODYBASE:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x86FE84)
        events.append([BASEDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        prevmark(a[3])
    elif k == FTBODY:
        ent1 = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        ent2 = 0 if a[7] == 0 else (W.raw(2, a[8]) if a[7] == 1 else a[9])
        A.wr(o, 4, a[3]); A.wr(o + 0xC, 4, ent1); A.wr(o + 0x10, 4, a[16]); A.wr(o + 0x14, 4, ent2); A.wr(o + 0x18, 4, a[17])
        W.snap()
        A.wr(o, 4, 0x86F570)
        if ent1:
            events.append([CLEANUP, W.tagv(ent1), W.tagv((this + 0xC) & M32)])
        if ent2:
            events.append([CLEANUP, W.tagv(ent2), W.tagv((this + 0x14) & M32)])
        events.append([BASEDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[11])
        marks.append('veh_null' if a[4] == 0 else ('veh_in_b' if a[4] == 1 else 'veh_raw'))
        marks.append('mate_null' if a[7] == 0 else ('mate_in_c' if a[7] == 1 else 'mate_raw'))
        marks.append('both_refs' if ent1 and ent2 else ('no_refs' if not ent1 and not ent2 else 'one_ref'))
        marks.append('driver_zero' if a[16] == 0 else 'driver_set')
    elif k == SWATCTOR:
        A.wr(o, 4, a[3]); A.wr(o + 0xC, 4, a[10]); A.wr(o + 0x10, 4, a[5]); A.wr(o + 0x14, 4, a[6]); A.wr(o + 0x18, 4, a[7])
        W.snap()
        events.append([BASECTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
        flags = A.rd(o + 0x14, 1) & 0xFE
        A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, 0); A.wr(o + 0x18, 4, 0)
        A.wr(o, 4, 0x86F59C)
        A.wr(o + 0x14, 1, flags)
        ret = W.tagv(this)
        marks.append('rope_zero' if a[4] == 0 else 'rope_nonzero')
        marks.append('byte_bit0_set' if a[6] & 1 else 'byte_bit0_clear')
        prevmark(a[3])
    elif k == SDD:
        body = SDD_BODY[fn]
        A.wr(o, 4, a[3])
        W.snap()
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([TASKDELETE, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
        prevmark(a[3])
    elif k == OBSSUB:
        newp = 0 if a[6] == 0 else W.raw(1, a[7])
        A.wr(o, 4, a[5]); A.wr(o + 0x10, 4, a[4])
        W.snap()
        t = a[3]
        ret = 0
        if t == 0xE0:
            events.append([TASKNEW, 0x18])
            if newp:
                events.append([BASECTOR, W.tagv(newp), W.rd(newp, 4)])
                W.wr(newp, 4, a[10])
                W.wr(newp, 4, 0x86DF98); W.wr(newp + 0xC, 4, 0); W.wr(newp + 0x10, 4, 0); W.wr(newp + 0x14, 1, 0); W.wr(newp + 0x15, 1, 0)
                ret = W.tagv(newp)
            marks.append('type_e0')
        elif t == 0x386:
            events.append([TASKNEW, 0x18])
            if newp:
                events.append([ACHCTOR, W.tagv(newp), a[4], ACH_RATE, ACH_TOL])
                gret = newp if a[8] == 0 else (a[9] if a[8] == 1 else 0)
                ret = W.tagv(gret)
                marks.append('ach_ret_new' if a[8] == 0 else ('ach_ret_raw' if a[8] == 1 else 'ach_ret_zero'))
            marks.append('type_386')
        elif t == 0x516:
            marks.append('type_516')
        else:
            marks.append('type_other')
        if t in (0xE0, 0x386):
            marks.append('new_set' if newp else 'new_null')
    elif k == GUNCTOR:
        ent = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        A.wr(o, 4, a[3])
        W.snap()
        events.append([BASECTOR_SIMPLE, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[18])
        A.wr(o + 0xF, 1, a[10] & 0xFF)
        A.wr(o, 4, 0x86D724)
        A.wr(o + 8, 1, 0); A.wr(o + 9, 1, 1)
        for off in (0xA, 0xB, 0xC, 0xD, 0xE):
            A.wr(o + off, 1, 0)
        A.wr(o + 0x10, 1, 0xFF); A.wr(o + 0x14, 4, 0); A.wr(o + 0x18, 4, 0)
        A.wr(o + 0x1C, 4, ent)
        if ent:
            events.append([REGREF, W.tagv(ent), W.tagv((this + 0x1C) & M32)])
        A.wr(o + 0x20, 4, a[7]); A.wr(o + 0x24, 4, a[8]); A.wr(o + 0x28, 4, a[9])
        A.wr(o + 0x34, 2, a[11] & 0xFFFF)
        A.wr(o + 0x2C, 4, 0); A.wr(o + 0x30, 4, 0)
        A.wr(o + 0x36, 2, 0)
        A.wr(o + 0x39, 1, 0); A.wr(o + 0x3A, 1, 0)
        A.wr(o + 0x3B, 1, a[16] & 0xFF)
        A.wr(o + 0x38, 1, 0xFF)
        ret = W.tagv(this)
        marks.append('ent_null' if a[4] == 0 else ('ent_in_b' if a[4] == 1 else 'ent_raw'))
        if ent:
            marks.append('registered')
        if (a[10] & 0xFF) == 0 and (a[10] >> 8):
            marks.append('cmd_low_zero_high_set')
        if a[11] >> 16:
            marks.append('burst_high_bits')
        if (a[16] & 0xFF) == 0 and (a[16] >> 8):
            marks.append('aim_low_zero_high_set')
        if a[16] & 0xFF:
            marks.append('aim_set')
    elif k == COMBO:
        s = bytes.fromhex(c['s']) if isinstance(c['s'], str) else bytes(c['s'])
        for i, b in enumerate(s):
            Cc.wr(o + i, 1, b)
        W.snap()
        val = None
        for idx, name in enumerate(COMBO_NAMES):
            want = name.encode() + b'\0'
            got = bytes(Cc.rd(o + i, 1) for i in range(len(want)))
            if got == want:                                 # repe cmpsb: iguales los len + 1 bytes (si difiere alguno, sigue con la siguiente cadena)
                val = COMBO_VALUES[idx]
                marks.append('match_' + name)
                break
        if val is None:
            val = 4
            marks.append('flowers_default')
            marks.append('no_match')
        if c['a'][3] == 1:
            marks.append('no_terminator')
        if len(s) <= 1:
            marks.append('empty')
        ret = val
    elif k == FLOAT:
        A.wr(o + 0x48, 4, a[4]); A.wr(o + 0x54, 4, a[5])
        W.snap()
        second = (a[3] & 0xFF) != 0           # solo el byte bajo del argumento decide
        bits = a[5] if second else a[4]
        st0 = f32_to_ext(bits)
        marks += ['sel_second' if second else 'sel_first', 'cls_' + float_class(bits)]
        if not second and a[3]:
            marks.append('arg_low_zero_high_set')
    elif k in (ROUTEPT, ROUTENODE):
        A.wr(o, 4, a[3])
        W.snap()
        size = 12 if k == ROUTEPT else 4
        count = s32(a[3])
        n = count - 1
        if n > 0:
            i, j = 0, n
            while i < j:
                pi, pj = o + 4 + size * i, o + 4 + size * j
                x, y = [A.rd(pi + q, 1) for q in range(size)], [A.rd(pj + q, 1) for q in range(size)]
                for q in range(size):
                    A.wr(pi + q, 1, y[q]); A.wr(pj + q, 1, x[q])
                i += 1
                j -= 1
        if count < 2:
            marks.append('count_noop')
        else:
            marks.append('count_two' if count == 2 else ('count_full' if count <= 8 else 'count_beyond'))
            marks.append('count_odd' if count & 1 else 'count_even')
    elif k == GANGBODY:
        assoc = 0 if a[8] == 0 else (W.raw(1, a[9]) if a[8] == 1 else a[10])
        target = 0 if a[16] == 0 else a[17]
        used = ((a[4] >> 8) & 0xFF) != 0
        A.wr(o, 4, a[3]); A.wr(o + 8, 4, a[4]); A.wr(o + 0x28, 4, a[5]); A.wr(o + 0x2C, 4, assoc); A.wr(o + 0x34, 4, target)
        if used:
            Cc.wr(a[7] + 20 * a[5], 4, a[6])
        W.snap()
        A.wr(o, 4, 0x86D944)
        if used:
            v = s32((a[6] - 0xB5D4A0) & M32) >> 5
            events.append([REMOVEREF, v & M32])
            marks.append('used')
            marks.append('idx_negative' if v < 0 else ('idx_zero' if v == 0 else 'idx_positive'))
        else:
            marks.append('not_used')
        if assoc:
            events.append([SETDELETE, W.tagv(assoc), DEFAULT_CB, 0])
        if target:
            events.append([CLEANUP, W.tagv(target), W.tagv((this + 0x34) & M32)])
        events.append([CTASKDTOR, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[18])
        marks.append('assoc_null' if a[8] == 0 else ('assoc_in_b' if a[8] == 1 else 'assoc_raw'))
        marks.append('target_null' if target == 0 else 'target_set')
    elif k == KILLCLONE:
        newp = 0 if a[6] == 0 else W.raw(1, a[7])
        A.wr(o + 0xC, 4, a[3]); A.wr(o + 0x10, 4, a[4]); A.wr(o + 0x14, 4, a[5])
        W.snap()
        events.append([TASKNEW, 0x18])
        ret = 0
        if newp:
            events.append([BASECTOR, W.tagv(newp), W.rd(newp, 4)])
            W.wr(newp, 4, a[8])
            W.wr(newp + 0x10, 4, a[4]); W.wr(newp + 0x14, 4, a[5])
            W.wr(newp, 4, 0x86DA1C)
            W.wr(newp + 0xC, 4, a[3])
            ret = W.tagv(newp)
        marks.append('new_in_b' if newp else 'new_null')
    out = {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks, 'st0': st0}
    return out


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
    if e['st0'] is None:
        if ftw != 0xFFFF:
            return 'x87 no vacia (ftw %04x)' % ftw
    else:
        top = (int.from_bytes(img[4:6], 'little') >> 11) & 7
        exp, man = e['st0']
        # etiqueta de ST0 (registro fisico 7) que calcula FNSAVE: 00 valido, 01 cero, 10 especial (infinito o NaN); los demas registros vacios (11)
        t7 = 1 if man == 0 and exp & 0x7FFF == 0 else 2 if exp & 0x7FFF == 0x7FFF else 0
        if ftw != (0x3FFF | (t7 << 14)) or top != 7:
            return 'x87: se esperaba un solo valor en ST0 (ftw %04x top %d)' % (ftw, top)
        got = img[28:38]   # FNSAVE guarda los registros en orden ST(0)..ST(7)
        if struct.unpack('<QH', got) != (man, exp):
            return 'ST0 %s != %s' % (struct.unpack('<QH', got), (man, exp))
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
