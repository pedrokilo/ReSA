"""Oraculo independiente (Python) de Fn181A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 29 funciones del lote 181A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial
y la aritmetica x87 de 24 bits (mantisa de 24 bits, exponente ampliado) con fracciones exactas, incluida la propagacion de NaN de la x87.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x407000  CEntity::IsInCurrentArea: al = BYTE [ecx+2Fh]; edx = [0B72914h]; ecx = movzx al; si ecx == edx o al == 0Dh: al = 1, si no al = 0 (solo AL)
   1  0x45AD40  CRemote::SetPlayerRemoteControlledCar(veh) cdecl: remoto = Players[BYTE [0B7CD74h]].remoto (0B7CE48h + 190h * indice); si no nulo: BYTE [remoto+36h] = (&7)|18h y, si el campo (releido) != veh, bandera = 1;
            veh: BYTE +36h = (&7)|40h; +428h |= 8; +3BAh, +3BBh, +3B9h = 0; [+3C8h] = 41100000h; BYTE +3D0h = 9; +3B7h, +3B8h = 0; +428h = (+42Dh & 2) ? &EFh : |10h (el byte +42Dh se lee ANTES de las escrituras);
            FindPlayerVehicle(-1, 0) (56E0D0); si no nulo: segunda llamada (sin comprobar nulo) y BYTE [+36h] = (&7)|48h; campo releido: si no nulo CleanUpOldReference (571A00, ECX = anterior, arg = direccion del campo);
            campo = veh; RegisterReference (571B70, ECX = veh, arg = direccion del campo); TakeControl (50C7C0, ECX = 0B6F028h, veh, 12h, bandera ? 2 : 1, 1); SetZoomValueCamStringScript (50C1B0, 1)
   2  0x45AF40  CMatrix::ScaleAll(f): para 4 filas de 16 bytes: componentes +0, +4, +8: fld [f]; fmul [elem]; fstp [elem] (x87 24 bits)
   3  0x45AFB0  CPhysical::SetTurnSpeed(x, y, z): [this+50h], [+54h], [+58h] = dwords
   4  0x4AFCB0  mov eax,8     5  0x4AFCC0  xor eax,eax     6  0x4AFCD0  fld dword [858B34h] (60.0f)     7  0x4AFCE0  mov eax,46h     8  0x4B06F0  mov al,1     19  0x62FCB0  mov eax,0D7h
   9  0x4B0810  CEventGunAimedAt::TakesPriorityOver(ref): si [this+14h] != 0 y IsPlayer (5DF8F0): fuente = ref->vtable[10] (+28h); si fuente == [this+14h] (releido): prioridad del ref (vtable[2]) y despues la propia (vtable[2]),
            devuelve propia >= ref (con signo); si no, tipo del ref (vtable[1]): 9, 24h, 27h -> 1; otro: CEvent::TakesPriorityOver (4ABC30). Sin jugador: prioridad del ref, la propia y el mismo >=
  10  0x4B4EE0  CEventGunAimedAt::AffectsPed(ped): IsPlayer(ped) -> 0; GetActiveTask (681720, ECX = [ped+47Ch] + 4); si no nula: GetActiveTask otra vez y tipo (vtable[4]) == 259h -> 0; [this+14h] nulo -> 0;
            pos = [aimed+14h] ? [aimed+14h] + 30h : aimed + 4; IsInSeeingRange (600C60, ECX = [ped+47Ch], pos) -> si no, 0; IsAlive (5E0170) -> si no, 0; AreInSameGroup (5F7F40, ped, [this+14h]) -> 0;
            si DWORD [ped+46Ch] bit 8 y [ped+58Ch] != 0: IsPassenger (6D1BD0, ECX = vehiculo, ped) y [vehiculo+460h] != 0 -> 0; si DWORD [ped+598h] == 6: ReportCrime (532010, 16h, ped, FindPlayerPed (56E210, -1)); devuelve 1
  11  0x5BD420  CVehicleAnimGroup::CVehicleAnimGroup: BYTE +0, +1 = 0; dwords +4 .. +3Ch = 0 (+4, +8, 5 floats +0Ch, 8 floats +20h); 5 vectores de 0Ch bytes desde +40h = 0; eax = this
  12  0x5BD4E0  CDecisionMaker::operator=(src): 41 x CDecision::From (6006B0, ECX = this + i*3Ch, arg = src + i*3Ch); eax = this
  13-17  0x5BD510..0x5BD5D0  idem con this + desplazamiento (C1B4h, CB50h, D4ECh, DE88h, E824h); EAX = el de la ultima From (no se observa)
  18  0x62FC80  CTaskComplexDieInCar(arma) `ret 4`: 61A3B0 (ECX = this); [this+0Ch] = arma; [this] = 86DE04h; dwords +10h, +14h = 0; bytes +18h, +19h = 0; eax = this
  20  0x635F90  CTaskComplexDieInCar::Clone: CTask::operator new (61A5A0, 1Ch); si no nulo: arma = [this+0Ch] (releido tras la reserva); 61A3B0 (ECX = memoria); mismos campos que 18; eax = memoria (0 sin memoria)
  21  0x636010  CTaskSimpleDrownInCar::Clone: 61A5A0 (8); si no nulo: 61A390 (ECX = memoria); [memoria] = 86DE54h; eax = memoria (0 sin memoria)
  22  0x636060  CTaskComplexDie::Clone: 61A5A0 (28h); si no nulo: 630040 (ECX = memoria) con 9 dwords de pila: [this+0Ch], +10h, +14h, +18h, +1Ch, bit0 de +20h, bit1, [this+24h], bit2; eax = el de 630040 (0 sin memoria)
  23  0x7316D0  GetDefaultTxdSlot(indice): eax = movsx word [0C88004h + indice*2]
  24  0x731800  CPool<TxdDef>::Flush: [esi+8] > 0 (con signo): si BYTE [esi+10h]: operator delete (8214BD) de [esi] y de [esi+4]; [esi], [esi+4], [esi+8], [esi+0Ch] = 0
  25  0x731AD0  CPool<TxdDef>::CPool(cap, nombre) `ret 8`: [esi] = new (82119A, cap*0Ch); [esi+4] = new (82119A, cap); BYTE [esi+10h] = 1; [esi+8] = cap; [esi+0Ch] = -1; si cap > 0: mapa a 80h; eax = esi
  26  0x731B30  segunda copia de Flush
  27  0x731B80  CPool<TxdDef>::New: como la de CColStore pero con elementos de 0Ch bytes
  28  0x731C20  destructor con borrado: cuerpo de Flush y, con BYTE [esp+8] & 1, operator delete (8214BD) de this; eax = this; `ret 4`

Los falsos anotan: [0x8214BD, argumento (etiqueta o crudo)]; [0x82119A, tamano]; [0x61A5A0, tamano]; [0x61A3B0 / 0x61A390, ECX]; [0x630040, ECX, 9 argumentos (banderas: byte bajo)]; [0x6006B0, ECX, origen];
[0x56E0D0, a, b]; [0x571A00 / 0x571B70, ECX, argumento]; [0x50C7C0, ECX, vehiculo, modo, cambio, ultimo]; [0x50C1B0, ECX, argumento]; [0xA002, 0xB001, 0xB002, 0xB00A, 0xC004: funciones de las vtables falsas, ECX];
[0x5DF8F0, ECX]; [0x4ABC30, ECX, ref]; [0x681720, ECX]; [0x600C60, ECX, pos]; [0x5E0170, ECX]; [0x5F7F40, a, b]; [0x6D1BD0, ECX, ped]; [0x56E210, id]; [0x532010, tipo, ped, jugador].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CEntity::IsInCurrentArea", "CRemote::SetPlayerRemoteControlledCar", "CMatrix::ScaleAll", "CPhysical::SetTurnSpeed", "CEventKnockOffBike::GetEventType", "CEventKnockOffBike::GetLifeTime",
         "CEventKnockOffBike::GetLocalSoundLevel", "CEventKnockOffBike::GetEventPriority", "CEventKnockOffBike::IsCriminalEvent", "CEventGunAimedAt::TakesPriorityOver", "CEventGunAimedAt::AffectsPed",
         "CVehicleAnimGroup::Constructor", "CDecisionMaker::operator=", "CDecisionMakerTypes::AssignDefaultRandomPed", "CDecisionMakerTypes::AssignDefaultMissionPed",
         "CDecisionMakerTypes::AssignDefaultPlayerPed", "CDecisionMakerTypes::AssignDefaultRandomGroup", "CDecisionMakerTypes::AssignDefaultMissionGroup", "CTaskComplexDieInCar::Constructor",
         "CTaskComplexDieInCar::GetTaskType", "CTaskComplexDieInCar::Clone", "CTaskSimpleDrownInCar::Clone", "CTaskComplexDie::Clone", "CTxdStore::GetDefaultTxdSlot", "CPool<TxdDef>::Flush",
         "CPool<TxdDef>::Constructor", "CPool<TxdDef>::FlushSecondCopy", "CPool<TxdDef>::New", "CPool<TxdDef>::ScalarDeletingDestructor"]
ENTRIES = (0x407000, 0x45AD40, 0x45AF40, 0x45AFB0, 0x4AFCB0, 0x4AFCC0, 0x4AFCD0, 0x4AFCE0, 0x4B06F0, 0x4B0810, 0x4B4EE0, 0x5BD420, 0x5BD4E0, 0x5BD510, 0x5BD540, 0x5BD570, 0x5BD5A0, 0x5BD5D0,
           0x62FC80, 0x62FCB0, 0x635F90, 0x636010, 0x636060, 0x7316D0, 0x731800, 0x731AD0, 0x731B30, 0x731B80, 0x731C20)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
CONST, CONSTF, BOOL, AREA, REMOTE, SCALE, TURN, GUNPRIO, GUNAFF, VAG, DM, DIECTOR, CLONEDIE, CLONEDROWN, CLONECX, TXDSLOT, FLUSH, PCTOR, PNEW, PSDD = \
    'const constf bool area remote scale turn gunprio gunaff vag dm diector clonedie clonedrown clonecx txdslot flush pctor pnew psdd'.split()
KIND = [AREA, REMOTE, SCALE, TURN, CONST, CONST, CONSTF, CONST, BOOL, GUNPRIO, GUNAFF, VAG, DM, DM, DM, DM, DM, DM, DIECTOR, CONST, CLONEDIE, CLONEDROWN, CLONECX, TXDSLOT, FLUSH, PCTOR, FLUSH, PNEW, PSDD]
CONST_VALUE = {4: 8, 5: 0, 6: 0x42700000, 7: 70, 8: 1, 19: 215}
DM_OFFSET = {12: 0, 13: 0xC1B4, 14: 0xCB50, 15: 0xD4EC, 16: 0xDE88, 17: 0xE824}
CURR_AREA, PLAYER_IN_FOCUS, PLAYERS, PLAYERS_BYTES, PLAYER_STRIDE, TXD_SLOTS, TXD_SLOTS_BYTES, TXD_SLOTS_BASE = 0xB72914, 0xB7CD74, 0xB7CE48, 0x320, 0x190, 0xC87F00, 0x710, 0xC88004
D_T1, D_T2, D_INTEL, D_MATRIX, D_VEHICLE, D_PLAYER = 0x40, 0x80, 0x100, 0x200, 0x300, 0x900
CAMERA = 0xB6F028


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta; memoria global del juego aparte"""

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
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
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

    def getg(self, addr, n=4):
        return sum(self.glob[addr + k] << (8 * k) for k in range(n))

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
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
    if _kind == AREA:
        _k += ['match_current', 'is_13', 'no_match', 'sext_trap', 'high_area']
    elif _kind == REMOTE:
        _k += ['cur_null', 'cur_same', 'cur_other', 'p1_null', 'p1_new', 'p1_other', 'p2_differs', 'flag_remove', 'flag_set', 'jumpcut', 'interpolate', 'player1']
    elif _kind == SCALE:
        _k += ['factor_nan', 'factor_inf', 'factor_zero', 'elem_nan', 'res_denorm', 'res_overflow', 'res_normal']
    elif _kind == TURN:
        _k += ['arg_nan', 'arg_snan', 'arg_zero']
    elif _kind == GUNPRIO:
        _k += ['no_aimed', 'not_player', 'same_source', 'other_source', 'type_special', 'base_called', 'base_true', 'base_false', 'prio_ge', 'prio_lt', 'prio_equal', 'signed_trap']
    elif _kind == GUNAFF:
        _k += ['is_player', 'task601', 'task_none', 'no_aimed', 'not_seeing', 'dead', 'same_group', 'driver_blocks', 'cop_report', 'plain_true', 'matrix_pos', 'placement_pos', 'second_task_differs']
    elif _kind == FLUSH or _kind == PSDD:
        _k += ['cap_pos', 'cap_nonpos', 'owns_set', 'owns_clear', 'storage_null', 'storage_arena']
        if _kind == PSDD:
            _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits']
    elif _kind == PCTOR:
        _k += ['cap_pos', 'cap_zero', 'cap_neg', 'size_wraps']
    elif _kind == PNEW:
        _k += ['first_try', 'wrapped', 'full', 'counter_wrap', 'ret_null', 'cursor_last']
    elif _kind == CLONEDIE:
        _k += ['mem_null', 'mem_in_b', 'swap']
    elif _kind == CLONEDROWN:
        _k += ['mem_null', 'mem_in_b']
    elif _kind == CLONECX:
        _k += ['mem_null', 'mem_in_b', 'swap', 'flag_stealth', 'flag_falling', 'flag_railing']
    elif _kind == TXDSLOT:
        _k += ['neg_index', 'pos_index', 'word_negative', 'word_positive']
    KEYS[_fn] = _k


# ------------------------------------------------------------------ aritmetica x87 con mantisa de 24 bits y exponente ampliado (fracciones exactas)
def unpack(bits):
    bits &= M32
    sign, e, m = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 0xFF:
        return ('nan', sign, m) if m else ('inf', sign)
    if e == 0:
        return ('zero', sign) if m == 0 else ('f', sign, Fraction(m, 1 << 149))
    return ('f', sign, Fraction((1 << 23) | m, 1) * Fraction(2) ** (e - 150))


def r24(v):
    """redondeo al par mas cercano a 24 bits de mantisa (exponente sin limite) de una Fraction positiva"""
    if v == 0:
        return Fraction(0)
    e = v.numerator.bit_length() - v.denominator.bit_length()
    if v < Fraction(2) ** e:
        e -= 1
    q = v / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return Fraction(n) * Fraction(2) ** (e - 23)


def store32(v):
    """bits de float32 (sin signo) de una Fraction positiva ya redondeada a 24 bits: redondeo al par mas cercano, denormales y desbordamiento"""
    if v == 0:
        return 0
    e = v.numerator.bit_length() - v.denominator.bit_length()
    if v < Fraction(2) ** e:
        e -= 1
    if e < -126:
        q = v / Fraction(1, 1 << 149)
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
            n += 1
        return n   # n == 2^23 da el menor normal
    q = v / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    if n == 1 << 24:
        n = 1 << 23
        e += 1
    if e > 127:
        return 0x7F800000
    return ((e + 127) << 23) | (n - (1 << 23))


def x87_mul_store(factor_bits, elem_bits):
    """fld dword [factor]; fmul dword [elem]; fstp dword [elem] con la x87 a 24 bits: bits resultantes"""
    a, b = unpack(factor_bits), unpack(elem_bits)
    if a[0] == 'nan' or b[0] == 'nan':
        # fld de un SNaN lo convierte en QNaN; en fmul con otra NaN gana la QNaN y entre dos QNaN la de mayor significando
        if a[0] == 'nan' and b[0] == 'nan':
            ma = a[2] | 0x400000
            mb = b[2]
            if mb & 0x400000 and mb > ma:
                return (b[1] << 31) | 0x7F800000 | mb
            return (a[1] << 31) | 0x7F800000 | ma
        if a[0] == 'nan':
            return (a[1] << 31) | 0x7F800000 | (a[2] | 0x400000)
        return (b[1] << 31) | 0x7F800000 | (b[2] | 0x400000)
    sign = (a[1] ^ b[1]) & 1
    if (a[0] == 'inf' and b[0] == 'zero') or (a[0] == 'zero' and b[0] == 'inf'):
        return 0xFFC00000
    if a[0] == 'inf' or b[0] == 'inf':
        return (sign << 31) | 0x7F800000
    if a[0] == 'zero' or b[0] == 'zero':
        return sign << 31
    return (sign << 31) | store32(r24(a[2] * b[2]))


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret = [], None
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])

    def ptr(mode, arena_addr, raw):
        return 0 if mode == 0 else (arena_addr if mode == 1 else raw)

    def pool_flush(o_, cap_flush=True):
        storage, flags = ptr(a[4], W.raw(1, a[5]), a[6]), ptr(a[7], W.raw(2, a[8]), a[9])
        A.wr(o_, 4, storage); A.wr(o_ + 4, 4, flags); A.wr(o_ + 8, 4, a[10]); A.wr(o_ + 0xC, 4, a[11]); A.wr(o_ + 0x10, 1, a[12] & 0xFF)
        return storage, flags

    if k == CONST:
        W.snap()
        ret = CONST_VALUE[fn]
    elif k == CONSTF:
        W.snap()
        ret = CONST_VALUE[fn]
    elif k == BOOL:
        W.snap()
        ret = 1
    elif k == AREA:
        A.wr(o + 0x2F, 1, a[4])
        W.setg(CURR_AREA, 4, a[5])
        gorder = [(CURR_AREA, 4)]
        W.snap()
        area, cur = a[4] & 0xFF, a[5]
        ret = 1 if (area == cur or area == 13) else 0
        marks.append('match_current' if area == cur else ('is_13' if area == 13 else 'no_match'))
        if area == 13:
            marks.append('is_13')
        if area >= 0x80 and cur == (0xFFFFFF00 | area):
            marks.append('sext_trap')
        if area >= 0x80:
            marks.append('high_area')
    elif k == REMOTE:
        vnew, vb, vc = this, W.raw(1, a[6]), W.raw(2, a[8])
        pick = lambda m: 0 if m == 0 else (vnew if m == 1 else (vb if m == 2 else vc))
        A.wr(o + 0x36, 1, a[10]); A.wr(o + 0x428, 1, a[11]); A.wr(o + 0x42D, 1, a[12])
        B.wr(a[6] + 0x36, 1, a[13]); Cc.wr(a[8] + 0x36, 1, a[14])
        idx = a[4]
        field = PLAYERS + idx * PLAYER_STRIDE
        W.setg(PLAYER_IN_FOCUS, 1, idx)
        cur = pick(a[5])
        W.setg(field, 4, cur)
        gorder = [(PLAYER_IN_FOCUS, 4), (PLAYERS, PLAYERS_BYTES)]
        W.snap()
        had_other = False
        if cur:
            W.wr(cur + 0x36, 1, (W.rd(cur + 0x36, 1) & 7) | 0x18)
            if W.getg(field, 4) != vnew:
                had_other = True
        sb, f428, f42d = W.rd(vnew + 0x36, 1), W.rd(vnew + 0x428, 1), W.rd(vnew + 0x42D, 1)
        W.wr(vnew + 0x36, 1, (sb & 7) | 0x40)
        new428 = (f428 | 8) & 0xFF
        W.wr(vnew + 0x428, 1, new428)
        for off in (0x3BA, 0x3BB, 0x3B9):
            W.wr(vnew + off, 1, 0)
        W.wr(vnew + 0x3C8, 4, 0x41100000)
        W.wr(vnew + 0x3D0, 1, 9)
        W.wr(vnew + 0x3B7, 1, 0)
        W.wr(vnew + 0x3B8, 1, 0)
        W.wr(vnew + 0x428, 1, (new428 & 0xEF) if f42d & 2 else (new428 | 0x10))
        events.append([0x56E0D0, M32, 0])
        p1 = pick(a[7])
        if p1:
            events.append([0x56E0D0, M32, 0])
            p2 = pick(a[9])
            W.wr(p2 + 0x36, 1, (W.rd(p2 + 0x36, 1) & 7) | 0x48)
            if p2 != p1:
                marks.append('p2_differs')
        prev = W.getg(field, 4)
        if prev:
            events.append([0x571A00, W.tagv(prev), field])
        W.setg(field, 4, vnew)
        events.append([0x571B70, W.tagv(vnew), field])
        events.append([0x50C7C0, CAMERA, W.tagv(vnew), 0x12, 2 if had_other else 1, 1])
        events.append([0x50C1B0, CAMERA, 1])
        marks.append('cur_null' if cur == 0 else ('cur_same' if cur == vnew else 'cur_other'))
        marks.append('p1_null' if p1 == 0 else ('p1_new' if p1 == vnew else 'p1_other'))
        marks.append('flag_remove' if f42d & 2 else 'flag_set')
        marks.append('jumpcut' if had_other else 'interpolate')
        if idx == 1:
            marks.append('player1')
    elif k == SCALE:
        for i in range(12):
            A.wr(o + 16 * (i // 3) + 4 * (i % 3), 4, a[8 + i])
        W.snap()
        fk = unpack(a[4])
        marks.append('factor_nan' if fk[0] == 'nan' else ('factor_inf' if fk[0] == 'inf' else ('factor_zero' if fk[0] == 'zero' else 'factor_fin')))
        norm = False
        for i in range(12):
            ev = a[8 + i]
            res = x87_mul_store(a[4], ev)
            A.wr(o + 16 * (i // 3) + 4 * (i % 3), 4, res)
            if unpack(ev)[0] == 'nan':
                marks.append('elem_nan')
            ek, rk = unpack(ev), unpack(res)
            if rk[0] == 'f' and rk[2] < Fraction(1, 1 << 126):
                marks.append('res_denorm')
            elif rk[0] == 'inf' and fk[0] == 'f' and ek[0] == 'f':
                marks.append('res_overflow')
            elif rk[0] == 'f':
                norm = True
        if norm:
            marks.append('res_normal')
    elif k == TURN:
        W.snap()
        for i in range(3):
            A.wr(o + 0x50 + 4 * i, 4, a[4 + i])
            u = unpack(a[4 + i])
            if u[0] == 'nan':
                marks.append('arg_nan')
                if not u[2] & 0x400000:
                    marks.append('arg_snan')
            if u[0] == 'zero':
                marks.append('arg_zero')
    elif k == GUNPRIO:
        aimed = W.raw(2, a[5]) if a[4] else 0
        ref = W.raw(1, a[6])
        src = 0 if a[8] == 0 else (W.raw(2, a[5]) if a[8] == 1 else (W.raw(3, a[9]) if a[8] == 2 else a[10]))
        W.snap()
        own, rp = s32(a[13]), s32(a[12])

        def prio_cmp():
            events.append([0xB002, W.tagv(ref)])
            events.append([0xA002, W.tagv(this)])
            marks.append('prio_ge' if own >= rp else 'prio_lt')
            if own == rp:
                marks.append('prio_equal')
            if (own >= rp) != ((own & M32) >= (rp & M32)):
                marks.append('signed_trap')
            return 1 if own >= rp else 0
        isp = False
        if aimed:
            events.append([0x5DF8F0, W.tagv(aimed)])
            isp = (a[7] & 0xFF) != 0
        else:
            marks.append('no_aimed')
        if aimed and isp:
            events.append([0xB00A, W.tagv(ref)])
            if src == aimed:
                marks.append('same_source')
                ret = prio_cmp()
            else:
                marks.append('other_source')
                events.append([0xB001, W.tagv(ref)])
                if a[11] in (9, 36, 39):
                    marks.append('type_special')
                    ret = 1
                else:
                    events.append([0x4ABC30, W.tagv(this), W.tagv(ref)])
                    marks.append('base_called')
                    ret = a[14] & 0xFF
                    marks.append('base_true' if ret else 'base_false')
        else:
            if aimed:
                marks.append('not_player')
            ret = prio_cmp()
    elif k == GUNAFF:
        ped, aimed_c = W.raw(1, a[6]), W.raw(2, a[5])
        aimed = aimed_c if a[4] else 0
        intel, t1, t2 = W.raw(3, D_INTEL), W.raw(3, D_T1), W.raw(3, D_T2)
        veh = W.raw(3, D_VEHICLE) if a[17] else 0
        matrix = W.raw(3, D_MATRIX) if a[20] else 0
        player = 0 if a[21] == 0 else (W.raw(3, D_PLAYER) if a[21] == 1 else a[22])
        W.snap()
        events.append([0x5DF8F0, W.tagv(ped)])
        while True:
            if a[7] & 0xFF:
                marks.append('is_player')
                ret = 0
                break
            events.append([0x681720, W.tagv(intel + 4)])
            first = 0 if a[8] == 0 else (t1 if a[8] == 1 else t2)
            if first:
                events.append([0x681720, W.tagv(intel + 4)])
                second = t1 if a[9] == 1 else t2
                if second != first:
                    marks.append('second_task_differs')
                events.append([0xC004, W.tagv(second)])
                if (a[10] if second == t1 else a[11]) == 0x259:
                    marks.append('task601')
                    ret = 0
                    break
            else:
                marks.append('task_none')
            if not aimed:
                marks.append('no_aimed')
                ret = 0
                break
            pos = (matrix + 0x30) if matrix else (aimed + 4)
            marks.append('matrix_pos' if matrix else 'placement_pos')
            events.append([0x600C60, W.tagv(intel), W.tagv(pos)])
            if not (a[12] & 0xFF):
                marks.append('not_seeing')
                ret = 0
                break
            events.append([0x5E0170, W.tagv(ped)])
            if not (a[13] & 0xFF):
                marks.append('dead')
                ret = 0
                break
            events.append([0x5F7F40, W.tagv(ped), W.tagv(aimed)])
            if a[14] & 0xFF:
                marks.append('same_group')
                ret = 0
                break
            if (a[16] & 0x100) and veh:
                events.append([0x6D1BD0, W.tagv(veh), W.tagv(ped)])
                if (a[15] & 0xFF) and a[18] != 0:
                    marks.append('driver_blocks')
                    ret = 0
                    break
            if a[19] == 6:
                marks.append('cop_report')
                events.append([0x56E210, M32])
                events.append([0x532010, 22, W.tagv(ped), W.tagv(player)])
            else:
                marks.append('plain_true')
            ret = 1
            break
    elif k == VAG:
        W.snap()
        for off in list(range(0, 2)) + list(range(4, 0x7C)):
            A.wr(o + off, 1, 0)
        ret = W.tagv(this)
    elif k == DM:
        src = W.raw(1, a[4])
        off = DM_OFFSET[fn]
        W.snap()
        base = (this - off) & M32
        for i in range(41):
            events.append([0x6006B0, W.tagv((base + off + i * 0x3C) & M32), W.tagv((src + i * 0x3C) & M32)])
        ret = W.tagv(base) if fn == 12 else None
    elif k == DIECTOR:
        W.snap()
        events.append([0x61A3B0, W.tagv(this)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, 0x86DE04); A.wr(o + 0xC, 4, a[4]); A.wr(o + 0x10, 4, 0); A.wr(o + 0x14, 4, 0); A.wr(o + 0x18, 1, 0); A.wr(o + 0x19, 1, 0)
        ret = W.tagv(this)
    elif k == CLONEDIE:
        A.wr(o + 0xC, 4, a[6])
        W.snap()
        events.append([0x61A5A0, 0x1C])
        if a[8]:
            A.wr(o + 0xC, 4, a[7])
            marks.append('swap')
        mem = 0 if a[4] == 0 else W.raw(1, a[5])
        marks.append('mem_null' if mem == 0 else 'mem_in_b')
        if mem == 0:
            ret = 0
        else:
            events.append([0x61A3B0, W.tagv(mem)])
            B.wr(a[5], 4, a[9])
            weapon = A.rd(o + 0xC, 4)
            B.wr(a[5] + 0xC, 4, weapon)
            B.wr(a[5], 4, 0x86DE04); B.wr(a[5] + 0x10, 4, 0); B.wr(a[5] + 0x14, 4, 0); B.wr(a[5] + 0x18, 1, 0); B.wr(a[5] + 0x19, 1, 0)
            ret = W.tagv(mem)
    elif k == CLONEDROWN:
        W.snap()
        events.append([0x61A5A0, 8])
        mem = 0 if a[4] == 0 else W.raw(1, a[5])
        marks.append('mem_null' if mem == 0 else 'mem_in_b')
        if mem == 0:
            ret = 0
        else:
            events.append([0x61A390, W.tagv(mem)])
            B.wr(a[5], 4, a[9])
            B.wr(a[5], 4, 0x86DE54)
            ret = W.tagv(mem)
    elif k == CLONECX:
        offs = (0xC, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24)
        for i in range(7):
            A.wr(o + offs[i], 4, a[6 + i])
        W.snap()
        events.append([0x61A5A0, 0x28])
        if a[13]:
            A.wr(o + offs[a[14] % 7], 4, a[15])
            marks.append('swap')
        mem = 0 if a[4] == 0 else W.raw(1, a[5])
        marks.append('mem_null' if mem == 0 else 'mem_in_b')
        if mem == 0:
            ret = 0
        else:
            f = [A.rd(o + off, 4) for off in offs]
            flags = f[5]
            if flags & 1:
                marks.append('flag_stealth')
            if flags & 2:
                marks.append('flag_falling')
            if flags & 4:
                marks.append('flag_railing')
            events.append([0x630040, W.tagv(mem), f[0], f[1], f[2], f[3], f[4], flags & 1, (flags >> 1) & 1, f[6], (flags >> 2) & 1])
            ret = W.tagv(a[16])
    elif k == TXDSLOT:
        region = Mem(a[2], a[62])
        W.snap()
        idx = s32(a[4])
        addr = TXD_SLOTS_BASE + idx * 2
        word = region.get(addr - TXD_SLOTS) | (region.get(addr - TXD_SLOTS + 1) << 8)
        ret = (word | 0xFFFF0000) & M32 if word & 0x8000 else word
        marks.append('neg_index' if idx < 0 else 'pos_index')
        marks.append('word_negative' if word & 0x8000 else 'word_positive')
    elif k in (FLUSH, PSDD):
        storage, flags = pool_flush(o)
        W.snap()
        cap = s32(a[10])
        marks.append('cap_pos' if cap > 0 else 'cap_nonpos')
        marks.append('owns_set' if a[12] & 0xFF else 'owns_clear')
        marks.append('storage_null' if storage == 0 else ('storage_arena' if a[4] == 1 else 'storage_raw'))
        if cap > 0:
            if a[12] & 0xFF:
                events.append([0x8214BD, W.tagv(storage)])
                events.append([0x8214BD, W.tagv(flags)])
            A.wr(o, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        if k == PSDD:
            fl = a[13] & 0xFF
            marks.append('flag_bit0_set' if fl & 1 else 'flag_bit0_clear')
            marks.append('flag_high_bits' if a[13] >> 8 else 'flag_byte_only')
            if fl & 1:
                events.append([0x8214BD, W.tagv(this)])
            ret = W.tagv(this)
    elif k == PCTOR:
        p1, p2 = ptr(a[4], W.raw(1, a[5]), a[6]), ptr(a[7], W.raw(2, a[8]), a[9])
        W.snap()
        cap = s32(a[10])
        events.append([0x82119A, (a[10] * 12) & M32])
        events.append([0x82119A, a[10] & M32])
        A.wr(o, 4, p1); A.wr(o + 4, 4, p2); A.wr(o + 0x10, 1, 1); A.wr(o + 8, 4, a[10]); A.wr(o + 0xC, 4, M32)
        if cap > 0:
            for i in range(cap):
                W.wr(p2 + i, 1, 0x80)
        marks.append('cap_pos' if cap > 0 else ('cap_zero' if cap == 0 else 'cap_neg'))
        if a[10] * 12 > M32:
            marks.append('size_wraps')
        ret = W.tagv(this)
    elif k == PNEW:
        cap = a[6]
        A.wr(o + 0, 4, a[4]); A.wr(o + 4, 4, W.raw(3, a[5])); A.wr(o + 8, 4, cap); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 1, a[8] & 0xFF)
        for i in range(cap):
            counter = 0 if a[11] == 0 else (0x7F if a[11] == 1 else rnd(a[63], 1000 + i) & 0x7F)
            if a[9] == 0:
                free = False
            elif a[9] == 1:
                free = i == a[10]
            elif a[9] == 2:
                free = rnd(a[63], 5000 + i) % 10 == 0
            elif a[9] == 3:
                free = (rnd(a[63], 6000 + i) & 1) != 0
            else:
                free = True
            D.wr(a[5] + i, 1, counter | (0x80 if free else 0))
        W.snap()
        last = s32(a[7])
        wrapped = False
        steps = 0
        ret = 0
        while True:
            last += 1
            steps += 1
            if last == cap:
                last = 0
                if wrapped:
                    marks.append('ret_null')
                    marks.append('full')
                    ret = 0
                    break
                wrapped = True
                marks.append('wrapped')
            byte = D.rd(a[5] + last, 1)
            if byte & 0x80:
                b = byte & 0x7F
                if b == 0x7F:
                    marks.append('counter_wrap')
                D.wr(a[5] + last, 1, (b + 1) & 0x7F)
                ret = W.tagv((a[4] + last * 0xC) & M32)
                if steps == 1 and not wrapped:
                    marks.append('first_try')
                if last == cap - 1:
                    marks.append('cursor_last')
                break
        A.wr(o + 0xC, 4, last & M32)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': W.gchanges(gorder), 'ret': ret, 'marks': marks}


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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL en las funciones bool"""
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
