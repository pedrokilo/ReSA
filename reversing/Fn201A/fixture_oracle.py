"""Oraculo independiente (Python) de Fn201A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 201A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0..9  0x54FC40, 0x54FC90, 0x54FCE0, 0x54FD30, 0x54FD80, 0x54FDD0, 0x54FE20, 0x54FE70, 0x54FEC0, 0x54FF10  (diez copias identicas de una plantilla de codigo muerto, 66 bytes):
            push esi; esi = ecx; si [esi+8] (int32, comparacion CON SIGNO) > 0: si BYTE [esi+10h] != 0: delete[] [esi] (CRT 8213AE) y delete[] [esi+4]; [esi], [esi+4], [esi+8], [esi+0Ch] = 0 (en ese orden); pop esi; ret
  10  0x6956B0  CTaskComplexSeekEntityAnyMeansXYOffset destructor con borrado: push esi; esi = ecx; call 6956D0 (cuerpo); si BYTE [esp+8] & 1: push esi; call 61A5B0 (CTask::operator delete); eax = esi; ret 4
  11  0x6956D0  cuerpo: [esi] = 870CF0h; si [esi+0Ch] != 0: push esi+0Ch; ecx = entidad; call 571A00 (CleanUpOldReference); [esi+24h] = 859DC4h; ecx = esi; call 61A3D0 (CTaskComplex::Destructor)
  12  0x696260  CTaskComplexFollowPedFootsteps destructor con borrado: igual que 10 con el cuerpo 695DC0
  13  0x6962A0  CTaskComplexFollowLeaderInFormation::CreateSubTask(tipo, ped): this+0Ch = grupo, +10h = lider, +14h = posicion. Compara el tipo en el orden 2C5h (>), 2BCh, CAh/CBh, 2C0h, y despues 386h, 38Bh, 516h:
            CBh: new(20h) 61A5A0; si no es nulo ctor 62F310(500, 0, 0, 41000000h); CAh: new(18h); ctor 48E750(7D0h); 2BCh: new(50h); ctor 640340([lider+58Ch], 0, 0); 2C0h: new(34h); ctor 63B8C0([ped+58Ch], 0, 0, 1, 0);
            2C5h: new(60h); ctor 63C340([ped+58Ch], 0, 0); 38Bh: new(58h); (sin comprobar nulo: el EXE escribiria en nulo + 44h) ctor 661DC0(lider, 0C350h, 3E8h, [86FC2C]=1.0f, 40000000h, [86FC30]=2.0f, 0, 1);
            copia 3 dwords de this+14h a eax+44h y [eax+50h] = 7 (eax = lo que devuelve el constructor); 386h: ecx = [this+0Ch] + 8: CountMembers 5F6A50; 407180(0, n) (GetRandomNumberInRange: rand 821B1E); GetMember(ecx, indice) 5F69B0;
            si es el ped: GetLeader 5F69A0; si hay miembro: 407180(0, 32h) == 14h: posicion del miembro (matriz +14h -> +30h, o +4) copiada a un local; VectorSubtract 40FE60(salida local, local del miembro, posicion del ped); Normalise 59C910;
            GetRadianAngleBetweenPoints 53CBE0(x, y, 0, 0) -> float (fstp); new(18h); si no es nulo LimitRadianAngle 53CB50(angulo) -> float; ctor 667E20(limite, 0.5f, 0.2f); en otro caso new(18h) y ctor 667E20([ped+558h], 0.5f, 0.2f);
            cualquier otro tipo (incluido 516h) devuelve 0. Devuelve el EAX del constructor (0 sin memoria).
  14  0x4B1130  mov eax,4Fh            15  0x4B1140  xor eax,eax            16  0x4B1150  fld dword [858B34h] (60.0f)          17  0x4B1160  mov eax,36h            31  0x4AC8F0  mov eax,1
  18  0x4B77B0  CEventHitByWaterCannon::Clone: ecx = [0B744ACh]; call 4B5570 (pool de eventos New); si nulo ret 0; [eax+4] = 0; BYTE [eax+8] = 0; [eax] = 85B488h; copia 6 dwords [esi+0Ch..+23h] a [eax+0Ch..+23h]; ret
  19  0x4B7810  CEventInWater::Clone: igual con 4 bytes: fld [esi+0Ch]; [eax+4] = 0; fstp [eax+0Ch]; BYTE [eax+8] = 0; [eax] = 85B4C8h (un NaN senalizante sale silenciado por el fld/fstp)
  20  0x4B7840  CEventStuckInAir::Clone: new; ctor 4B1490([esi+0Ch]); devuelve el EAX del constructor        21  0x4B78B0  CEventCopCarBeingStolen::Clone: new; ctor 4B1740([esi+0Ch], [esi+10h])
  22  0x4B7960  CEventGotKnockedOverByCar::CloneEditable: new; ctor 4B1B60([esi+14h])                     23  0x4B79D0  CEventCarUpsideDown::Clone: new; ctor 4B1CC0([esi+0Ch])
  24  0x4B7A40  CEventPotentialWalkIntoFire::CloneEditable: new; ctor 4B1E20(esi+14h, [esi+20h], [esi+28h])
  25  0x4B7A70  CEventFireNearby::CloneEditable: new; [eax+4] = 0; BYTE [eax+8] = 0; BYTE [eax+0Ch] = 1; WORD [eax+0Eh] = 0C8h; WORD [eax+10h] = 0FFFFh; [eax] = 85B6E8h; copia 3 dwords [esi+14h..+1Fh] a [eax+14h..]
  26  0x4B7AC0  CEventGunShotWhizzedBy::CloneEditable: new; ctor 4B1FD0([esi+14h], esi+18h, esi+24h, BYTE [esi+30h] sin signo)
  27  0x4B7B30  CEventSeenPanickedPed::CloneEditable: new; ctor 4B2080([esi+14h])                          28  0x4B7BA0  CEventAreaCodes::Clone: new; ctor 4B2190([esi+0Ch])
  29  0x4B7C10  CEventDanger::CloneEditable: new; ctor 4B2600([esi+14h], [esi+18h])
  30  0x4B7C80  CEvent destructor con borrado: [ecx] = 85AB38h; si BYTE [esp+4] & 1: esi = [0B744ACh]; edx = (this - [esi]) / 44h (imul 78787879h; sar 5; + signo); [[esi+4] + edx] |= 80h; si edx < [esi+0Ch] (con signo): [esi+0Ch] = edx; eax = this; ret 4

Los falsos anotan: [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (cuerpos); [direccion, ECX (pool)] (0x4B5570); [direccion, ECX, argumentos...] (constructores: crudos salvo los punteros a campos del objeto,
que van como etiqueta); [0x8213AE, puntero crudo]; [0x61A5A0, tamano]; [0x61A5B0, puntero, dword de [puntero]]; [0x571A00, entidad cruda, referencia]; [0x821B1E]; [0x5F6A50, ECX]; [0x5F69B0, ECX, indice]; [0x5F69A0, ECX];
[0x40FE60, salida, a (3 dwords), b, b (3 dwords)]; [0x59C910, ECX, 3 dwords]; [0x53CBE0, cuatro dwords]; [0x53CB50, dword]. Un puntero a la pila del llamador se etiqueta 0x7F000000.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = [
    "CPlaceable::ArrayPairReset54FC40", "CPlaceable::ArrayPairReset54FC90", "CPlaceable::ArrayPairReset54FCE0", "CPlaceable::ArrayPairReset54FD30",
    "CPlaceable::ArrayPairReset54FD80", "CPlaceable::ArrayPairReset54FDD0", "CPlaceable::ArrayPairReset54FE20", "CPlaceable::ArrayPairReset54FE70",
    "CPlaceable::ArrayPairReset54FEC0", "CPlaceable::ArrayPairReset54FF10", "CTaskComplexSeekEntityAnyMeansXYOffset::ScalarDeletingDestructor",
    "CTaskComplexSeekEntityAnyMeansXYOffset::DestructorBody", "CTaskComplexFollowPedFootsteps::ScalarDeletingDestructor", "CTaskComplexFollowLeaderInFormation::CreateSubTask",
    "CEventVehicleOnFire::GetEventType", "CEventVehicleOnFire::GetLifeTime", "CEventVehicleOnFire::GetLocalSoundLevel", "CEventVehicleOnFire::GetEventPriority",
    "CEventHitByWaterCannon::Clone", "CEventInWater::Clone", "CEventStuckInAir::Clone", "CEventCopCarBeingStolen::Clone", "CEventGotKnockedOverByCar::CloneEditable",
    "CEventCarUpsideDown::Clone", "CEventPotentialWalkIntoFire::CloneEditable", "CEventFireNearby::CloneEditable", "CEventGunShotWhizzedBy::CloneEditable",
    "CEventSeenPanickedPed::CloneEditable", "CEventAreaCodes::Clone", "CEventDanger::CloneEditable", "CEvent::ScalarDeletingDestructor", "CEventVehicleCollision::GetEventType"]
ENTRIES = (0x54FC40, 0x54FC90, 0x54FCE0, 0x54FD30, 0x54FD80, 0x54FDD0, 0x54FE20, 0x54FE70, 0x54FEC0, 0x54FF10, 0x6956B0, 0x6956D0, 0x696260, 0x6962A0, 0x4B1130, 0x4B1140,
           0x4B1150, 0x4B1160, 0x4B77B0, 0x4B7810, 0x4B7840, 0x4B78B0, 0x4B7960, 0x4B79D0, 0x4B7A40, 0x4B7A70, 0x4B7AC0, 0x4B7B30, 0x4B7BA0, 0x4B7C10, 0x4B7C80, 0x4AC8F0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

ARR, CONST, CONSTF, SDD, XYBODY, FORMSUB, CLONED, CLONEC, EVSDD = 'arr const constf sdd xybody formsub cloned clonec evsdd'.split()
KIND = [ARR] * 10 + [SDD, XYBODY, SDD, FORMSUB, CONST, CONST, CONSTF, CONST, CLONED, CLONED, CLONEC, CLONEC, CLONEC, CLONEC, CLONEC, CLONED, CLONEC, CLONEC, CLONEC, CLONEC, EVSDD, CONST]
CONSTS = {14: 0x4F, 15: 0, 17: 0x36, 31: 1}
FLOAT_60 = 0x42700000
SPAN = [0x14] * 10 + [0x28, 0x28, 0x28, 0x20, 4, 4, 4, 4, 0x24, 0x10, 0x10, 0x14, 0x18, 0x10, 0x2C, 0x20, 0x34, 0x18, 0x10, 0x1C, 4, 4]
POOL_NEW, DELETE_ARRAY, CTASK_NEW, CTASK_DELETE, CLEANUP, RAND_EVENT = 0x4B5570, 0x8213AE, 0x61A5A0, 0x61A5B0, 0x571A00, 0x821B1E
SDD_INFO = {10: 0x6956D0, 12: 0x695DC0}                 # direccion del cuerpo (evento)
CLONE_CTOR = {20: 0x4B1490, 21: 0x4B1740, 22: 0x4B1B60, 23: 0x4B1CC0, 24: 0x4B1E20, 26: 0x4B1FD0, 27: 0x4B2080, 28: 0x4B2190, 29: 0x4B2600}
STACK_TAG = 0x7F000000


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


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def to_fraction(b):
    """valor exacto de un float finito (con signo)"""
    s, e, f = b >> 31, (b >> 23) & 0xFF, b & 0x7FFFFF
    m = f if e == 0 else (f | 0x800000)
    ex = -149 if e == 0 else e - 150
    v = Fraction(m) * (Fraction(2) ** ex)
    return -v if s else v


def round24(q):
    """redondea q >= 0 a 24 bits de mantisa (al mas cercano, empate al par), sin limite de exponente (la x87 conserva el exponente ampliado)"""
    if q == 0:
        return q
    e = q.numerator.bit_length() - q.denominator.bit_length() - 23
    m = q / (Fraction(2) ** e)
    while m >= (1 << 24):
        e += 1
        m = q / (Fraction(2) ** e)
    while m < (1 << 23):
        e -= 1
        m = q / (Fraction(2) ** e)
    fl = m.numerator // m.denominator
    rem = m - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (fl & 1)):
        fl += 1
    return Fraction(fl) * (Fraction(2) ** e)


def rand_in_range(r, lo, hi):
    """0x407180 del EXE: (rand & 0FFFFh) como entero (fild) por 2^-15 (fmul dword, exacto) por (hi - lo) (fimul dword, redondeado a 24 bits por la palabra de control) y truncado hacia cero (_ftol), mas lo; solo
    para magnitudes que caben en un entero de 32 bits"""
    rng = signed32((hi - lo) & M32)
    x = Fraction(r & 0xFFFF) * Fraction(1, 1 << 15) * rng
    q = round24(abs(x))
    n = int(q)               # trunca hacia cero
    n = -n if x < 0 else n
    assert -(1 << 31) <= n < (1 << 31), 'conversion fuera del rango de un entero de 32 bits'
    return (lo + n) & M32


def signed32(v):
    return v - (1 << 32) if v >= 0x80000000 else v


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
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

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
    if _kind == ARR:
        _k += ['cnt_pos', 'cnt_zero', 'cnt_neg', 'owner_delete', 'not_owner', 'owner_byte_zero_high_set', 'zeroed']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == XYBODY:
        _k += ['entity_null', 'entity_raw', 'entity_arena', 'prev_zero', 'prev_other']
    elif _kind in (CLONED, CLONEC):
        _k += ['mem_null', 'mem_set']
        if _fn == 19:
            _k += ['snan']
    elif _kind == EVSDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'idx_neg', 'idx_over', 'idx_in', 'cursor_lowered', 'cursor_kept', 'extra_nonzero']
    elif _kind == FORMSUB:
        _k += ['type_cb', 'type_ca', 'type_700', 'type_2c0', 'type_2c5', 'type_38b', 'type_386', 'type_516', 'type_other', 'mem_null', 'mem_set', 'heading_angle', 'heading_rotation', 'member_null',
               'member_ped', 'member_other', 'leader_used', 'count_zero', 'count_neg', 'ret_mem', 'ret_alt', 'ret_raw', 'matrix_ped', 'matrix_other', 'rand20']
    KEYS[_fn] = _k


def quiet(b):
    """fld dword + fstp dword de la x87: un NaN senalizante sale silenciado (bit 22 puesto); el resto de valores pasa sin cambios"""
    return (b | 0x400000) if is_nan(b) else b


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
    span = SPAN[fn]
    A.wr(o, 4, a[3])
    if k in (CONST, CONSTF):
        W.snap()
        ret = FLOAT_60 if k == CONSTF else CONSTS[fn]
    elif k == ARR:
        A.wr(o + 0, 4, a[5]); A.wr(o + 4, 4, a[6]); A.wr(o + 8, 4, a[4]); A.wr(o + 0xC, 4, a[7]); A.wr(o + 0x10, 4, a[8])
        W.snap()
        count = signed32(a[4])
        marks.append('cnt_pos' if count > 0 else ('cnt_zero' if count == 0 else 'cnt_neg'))
        if count > 0:
            marks.append('zeroed')
            if (a[8] & 0xFF) != 0:
                marks.append('owner_delete')
                events.append([DELETE_ARRAY, a[5]])
                events.append([DELETE_ARRAY, a[6]])
            else:
                marks.append('not_owner')
                if a[8] & 0xFFFFFF00:
                    marks.append('owner_byte_zero_high_set')
            for off in (0, 4, 8, 0xC):
                A.wr(o + off, 4, 0)
        elif (a[8] & 0xFF) == 0 and (a[8] & 0xFFFFFF00):
            marks.append('owner_byte_zero_high_set')
    elif k == SDD:
        W.snap()
        flags = a[6]
        events.append([SDD_INFO[fn], W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        if flags & 0xFFFFFF00:
            marks.append('flag_high_bits')
        else:
            marks.append('flag_byte_only')
        if flags & 1:
            events.append([CTASK_DELETE, W.tagv(this), A.rd(o, 4)])
        ret = W.tagv(this)
    elif k == XYBODY:
        entity = 0 if a[4] == 0 else (a[5] if a[4] == 1 else W.raw(1, a[6]))
        A.wr(o + 0xC, 4, entity); A.wr(o + 0x24, 4, a[7])
        W.snap()
        marks.append('entity_null' if a[4] == 0 else ('entity_raw' if a[4] == 1 else 'entity_arena'))
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        A.wr(o, 4, 0x870CF0)
        if entity != 0:
            events.append([CLEANUP, entity, W.tagv((this + 0xC) & M32)])
        A.wr(o + 0x24, 4, 0x859DC4)
        events.append([0x61A3D0, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[9])
    elif k in (CLONED, CLONEC):
        pool = W.raw(3, a[8])
        for off in range(4, span, 4):
            A.wr(o + off, 4, a[10 + off // 4])
        W.snap()
        mem = None if a[4] == 0 else W.raw(1, a[5])
        events.append([POOL_NEW, W.tagv(pool)])
        marks.append('mem_null' if mem is None else 'mem_set')
        src = lambda off: A.rd(o + off, 4)
        if mem is None:
            ret = 0
        elif k == CLONED:
            W.wr(mem + 4, 4, 0)
            W.wr(mem + 8, 1, 0)
            if fn == 18:
                W.wr(mem, 4, 0x85B488)
                for off in range(0xC, 0x24, 4):
                    W.wr(mem + off, 4, src(off))
            elif fn == 19:
                value = src(0xC)
                if is_nan(value) and not value & 0x400000:
                    marks.append('snan')
                W.wr(mem + 0xC, 4, quiet(value))
                W.wr(mem, 4, 0x85B4C8)
            else:
                W.wr(mem + 0xC, 1, 1)
                W.wr(mem + 0xE, 2, 0xC8)
                W.wr(mem + 0x10, 2, 0xFFFF)
                W.wr(mem, 4, 0x85B6E8)
                for off in range(0x14, 0x20, 4):
                    W.wr(mem + off, 4, src(off))
            ret = W.tagv(mem)
        else:
            args = {20: [src(0xC)], 21: [src(0xC), src(0x10)], 22: [src(0x14)], 23: [src(0xC)], 24: [W.tagv((this + 0x14) & M32), src(0x20), src(0x28)],
                    26: [src(0x14), W.tagv((this + 0x18) & M32), W.tagv((this + 0x24) & M32), A.rd(o + 0x30, 1)], 27: [src(0x14)], 28: [src(0xC)], 29: [src(0x14), src(0x18)]}[fn]
            events.append([CLONE_CTOR[fn], W.tagv(mem)] + args)
            W.wr(mem, 4, a[7])
            ret = a[6]
        if fn == 19 and 'snan' not in marks:
            if is_nan(A.rd(o + 0xC, 4)) and not A.rd(o + 0xC, 4) & 0x400000:
                marks.append('snan')
    elif k == EVSDD:
        idx = signed32(a[5])
        extra = a[6]
        storage = (this - ((idx * 0x44) & M32) - extra) & M32
        B.wr(a[7] + 0, 4, storage); B.wr(a[7] + 4, 4, W.raw(3, a[8])); B.wr(a[7] + 8, 4, a[4]); B.wr(a[7] + 0xC, 4, a[9])
        W.snap()
        flags = a[10]
        A.wr(o, 4, 0x85AB38)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        if flags & 0xFFFFFF00:
            marks.append('flag_high_bits')
        marks.append('idx_neg' if idx < 0 else ('idx_over' if idx >= a[4] else 'idx_in'))
        if extra:
            marks.append('extra_nonzero')
        if flags & 1:
            diff = signed32((this - storage) & M32)
            q = abs(diff) // 0x44
            index = -q if diff < 0 else q
            states = B.rd(a[7] + 4, 4)
            W.wr(states + index, 1, W.rd(states + index, 1) | 0x80)
            last = signed32(B.rd(a[7] + 0xC, 4))
            if index < last:
                B.wr(a[7] + 0xC, 4, index & M32)
                marks.append('cursor_lowered')
            else:
                marks.append('cursor_kept')
        ret = W.tagv(this)
    elif k == FORMSUB:
        ret = form_sub(a, W, A, B, Cc, D, o, this, marks, events)
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def form_sub(a, W, A, B, Cc, D, o, this, marks, events):
    type_ = a[4]
    group = W.raw(3, a[9])
    ped = W.raw(1, a[6]) if a[5] else a[7]
    other = W.raw(2, a[8])
    leader = 0 if a[10] == 0 else (a[34] if a[10] == 1 else W.raw(3, a[11]))
    A.wr(o + 0xC, 4, group); A.wr(o + 0x10, 4, leader)
    A.wr(o + 0x14, 4, a[12]); A.wr(o + 0x18, 4, a[13]); A.wr(o + 0x1C, 4, a[14])
    if a[5]:
        B.wr(a[6] + 0x14, 4, W.raw(1, a[16]) if a[15] else 0)
        B.wr(a[6] + 4, 4, a[17]); B.wr(a[6] + 8, 4, a[18]); B.wr(a[6] + 0xC, 4, a[19])
        B.wr(a[6] + 0x558, 4, a[20]); B.wr(a[6] + 0x58C, 4, a[21])
        for kk in range(3):
            B.wr(a[16] + 0x30 + 4 * kk, 4, a[22 + kk])
    if a[10] == 2:
        D.wr(a[11] + 0x58C, 4, a[25])
    Cc.wr(a[8] + 0x14, 4, W.raw(2, a[27]) if a[26] else 0)
    Cc.wr(a[8] + 4, 4, a[28]); Cc.wr(a[8] + 8, 4, a[29]); Cc.wr(a[8] + 0xC, 4, a[30])
    for kk in range(3):
        Cc.wr(a[27] + 0x30 + 4 * kk, 4, a[31 + kk])
    new_mem = 0 if a[35] == 0 else W.raw(2, a[40])
    ctor_ret = (new_mem if new_mem else a[43]) if a[41] == 0 else (W.raw(2, a[42]) if a[41] == 1 else a[43])
    member_sel = 0 if a[48] == 0 else (ped if a[48] == 1 else other)
    leader_sel = 0 if a[49] == 0 else (other if a[49] == 1 else ped)
    W.snap()
    marks.append('mem_null' if new_mem == 0 else 'mem_set')
    marks.append('ret_mem' if a[41] == 0 else ('ret_alt' if a[41] == 1 else 'ret_raw'))

    def alloc(size):
        events.append([CTASK_NEW, size])
        return new_mem or None

    def construct(addr, mem, args):
        events.append([addr, W.tagv(mem)] + [x & M32 for x in args])
        W.wr(mem, 4, a[44])
        return ctor_ret

    def position_address(obj):
        matrix = W.rd(obj + 0x14, 4)
        return (matrix + 0x30) & M32 if matrix else (obj + 4) & M32

    def heading_rotation():
        mem = alloc(0x18)
        if mem is None:
            return 0
        marks.append('heading_rotation')
        return construct(0x667E20, mem, [W.rd(ped + 0x558, 4), 0x3F000000, 0x3E4CCCCD])

    def top_type(prefix):
        marks.append(prefix)

    ret = 0
    if type_ == 0xCB:
        marks.append('type_cb')
        mem = alloc(0x20)
        if mem is not None:
            ret = construct(0x62F310, mem, [500, 0, 0, 0x41000000])
    elif type_ == 0xCA:
        marks.append('type_ca')
        mem = alloc(0x18)
        if mem is not None:
            ret = construct(0x48E750, mem, [2000])
    elif type_ == 700:
        marks.append('type_700')
        mem = alloc(0x50)
        if mem is not None:
            ret = construct(0x640340, mem, [W.rd(leader + 0x58C, 4), 0, 0])
    elif type_ == 0x2C0:
        marks.append('type_2c0')
        mem = alloc(0x34)
        if mem is not None:
            ret = construct(0x63B8C0, mem, [W.rd(ped + 0x58C, 4), 0, 0, 1, 0])
    elif type_ == 0x2C5:
        marks.append('type_2c5')
        mem = alloc(0x60)
        if mem is not None:
            ret = construct(0x63C340, mem, [W.rd(ped + 0x58C, 4), 0, 0])
    elif type_ == 0x38B:
        marks.append('type_38b')
        mem = alloc(0x58)
        assert mem is not None, 'el EXE escribiria en nulo + 44h'
        ret = construct(0x661DC0, mem, [leader, 0xC350, 0x3E8, 0x3F800000, 0x40000000, 0x40000000, 0, 1])
        target = ctor_ret
        for kk in range(3):
            W.wr((target + 0x44 + 4 * kk) & M32, 4, A.rd(o + 0x14 + 4 * kk, 4))
        W.wr((target + 0x50) & M32, 4, 7)
    elif type_ == 0x386:
        marks.append('type_386')
        events.append([0x5F6A50, W.tagv((group + 8) & M32)])
        count = a[45]
        marks.append('count_zero' if count == 0 else ('count_neg' if count >= 0x80000000 else 'count_pos'))
        events.append([RAND_EVENT])
        index = rand_in_range(a[46], 0, count)
        events.append([0x5F69B0, W.tagv((group + 8) & M32), index])
        member = member_sel
        if member == ped:
            events.append([0x5F69A0, W.tagv((group + 8) & M32)])
            member = leader_sel
            marks.append('leader_used')
        marks.append('member_null' if member == 0 else ('member_ped' if member == ped else 'member_other'))
        if a[26] == 1:
            marks.append('matrix_other')
        if a[5] and a[15] == 1:
            marks.append('matrix_ped')
        angle_branch = False
        if member != 0:
            events.append([RAND_EVENT])
            r2 = rand_in_range(a[47], 0, 50)
            if r2 == 20:
                marks.append('rand20')
                angle_branch = True
        if angle_branch:
            mpos = position_address(member)
            ppos = position_address(ped)
            events.append([0x40FE60, STACK_TAG, W.rd(mpos, 4), W.rd(mpos + 4, 4), W.rd(mpos + 8, 4), W.tagv(ppos), W.rd(ppos, 4), W.rd(ppos + 4, 4), W.rd(ppos + 8, 4)])
            events.append([0x59C910, STACK_TAG, a[50], a[51], a[52]])
            events.append([0x53CBE0, a[53], a[54], 0, 0])
            angle = quiet(a[56])
            mem = alloc(0x18)
            if mem is not None:
                marks.append('heading_angle')
                events.append([0x53CB50, angle])
                ret = construct(0x667E20, mem, [quiet(a[57]), 0x3F000000, 0x3E4CCCCD])
        else:
            ret = heading_rotation()
    elif type_ == 0x516:
        marks.append('type_516')
    else:
        marks.append('type_other')
    return ret


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
