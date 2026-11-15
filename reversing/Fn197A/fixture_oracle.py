"""Oraculo independiente (Python) de Fn197A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 32 funciones del lote 197A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x5FF490  CEventHealthLow::CloneEditable: marco SEH; push 14h; call 4B5620 (cdecl); si eax != 0: ecx = eax; call 4AC450; [esi] = 86CBA0h; eax = esi; si no eax = 0
   1  0x5FF4E0  destructor con borrado: push esi; mov esi,ecx; call 5FF460 (cuerpo: mov [ecx],86CBA0h; jmp 4AC480); test byte [esp+8],1; je +; push esi; call 4B5630 (cdecl); mov eax,esi; ret 4
   2  0x5FF500  CEventHealthReallyLow ctor: push esi; mov esi,ecx; call 4AC450; mov [esi],86CBE8h; mov eax,esi; ret
   3  0x5FF520  cuerpo del destructor: mov [ecx],86CBE8h; jmp 4AC480
   4  0x5FF550  CloneEditable como 0 con 86CBE8h
   5  0x5FF5A0  destructor con borrado como 1 con el cuerpo 5FF520
   6  0x5FF5C0  CEventLowAngerAtPlayer ctor (86CC30h);  7 0x5FF5E0 cuerpo (86CC30h; jmp 4AC480);  8 0x5FF610 CloneEditable;  9 0x5FF660 destructor con borrado (cuerpo 5FF5E0)
  10  0x5FF680  CEventHighAngerAtPlayer ctor (86CC78h); 11 0x5FF6A0 cuerpo (86CC78h; jmp 4AC480); 12 0x5FF6D0 CloneEditable; 13 0x5FF720 destructor con borrado (cuerpo 5FF6A0)
  14  0x5FF740  CEventOnFire ctor: call 4ABFC0; mov [esi],86CCC0h; eax = esi
  15  0x5FF760  cuerpo del destructor: mov [ecx],86CCC0h; jmp 4AC010
  16  0x5FF770  GetEventType: mov eax,3Ah;  17 0x5FF780 GetLifeTime: xor eax,eax
  18  0x5FF790  Clone: new(0Ch) 4B5620; ctor 4ABFC0; [mem] = 86CCC0h; eax = mem (o 0)
  19  0x5FF7E0  GetLocalSoundLevel: fld dword [858B34h] (= 42700000h, 60.0f)
  20  0x5FF7F0  destructor con borrado (cuerpo 5FF760)
  21  0x5FF820  CEventEscalator ctor: call 4ABFC0; mov [esi],86CD00h; eax = esi
  22  0x5FF840  GetEventType: mov eax,4Ah;  23 0x5FF850 GetLifeTime: xor eax,eax;  24 0x5FF860 cuerpo del destructor: jmp 4AC010 (sin escribir la vtable);  25 0x5FF870 GetEventPriority: mov eax,38h
  26  0x5FF880  CEventSeenCop::GetEventPriority: mov eax,15h;  27 0x5FF890 CEventOnFire::GetEventPriority: mov eax,42h;  28 0x5FF8A0 CEventAcquaintancePedHateBadlyLit::GetEventPriority: mov eax,19h
  29  0x632190  ShouldLoadSunbatheAnims: ped = [B7CD98h + movzx byte [B7CD74h] * 190h]; si [ped+46Ch] bit 8 y [ped+58Ch] != 0: carga x (+44h), y (+48h), z (+4Ch) de ese vehiculo; (x*x + y*y) + z*z con cada operacion
            redondeada a 24 bits (palabra de control de la x87) y exponente ampliado; fcomp con el dword [863244h] = 3D23D70Bh; AL = 0 si es ESTRICTAMENTE mayor (fnstsw: C0 = C3 = 0), si no AL = 1 (con NaN: 1)
  30  0x638290  CTaskComplexSunbathe::CreateSubTask(tipo, ped): si [this+8] != 0 y su GetTaskType (ranura 4, call [vtable+10h]) == tipo: eax = [this+8], nada mas. Si no: cmp tipo,1ADh (con signo): mayor: 1AEh o 516h/otro;
            igual: 1ADh; si no tipo - CAh = 0: CAh; - D8h = 0: 1A2h; - 0Ah = 0: 1ACh; otro: 0.
            1ACh: new(20h) 61A5A0; si hay memoria: ctor 61A900(30h, tipo_banista + F4h, 4.0f, 1ACh, 86E654h, 1); si no 0.  1AEh: igual con id + F9h, 1AEh, 86E634h, hold 0.
            1ADh: si tipo_banista (+20h) = 0 o 2: rand() 821B1E y conversion de 407180(0, 3) y id = azar + FEh (tipo 0) o + 101h (tipo 2); si no id = 1ADh; despues new(20h) y ctor 61A900(30h, id, 4.0f, 1ADh, 86E644h, 1).
            1A2h: id = tipo_banista + EFh; si el bloque [this+28h] tiene el byte +10h distinto de 0 y tipo_banista es 0 o 2: t = GetTimeLeft(this+14h) + 3E8h; si t > 2EE0h (con signo) t = 2EE0h, si no t = GetTimeLeft(this+14h)
            (segunda llamada) + 3E8h; duracion = 407180(0BB8h, t) (rand() 821B1E real o falso); si no duracion = GetTimeLeft(this+14h) + 3E8h. new(34h); ctor 61ABE0(2Fh, id, 4.0f, -4.0f, duracion, 1A2h, 86E098h, 1).
            CAh: si el bloque [this+28h] no esta cargado: ShouldLoad 632190; espera si (no cargado y ShouldLoad) o el bloque [this+24h] no esta cargado: new(18h); ctor 48E750(10000); si no: new(18h); 407180(1000, 5000) (rand() 821B1E); ctor 48E750(resultado).
            eax = el EAX del constructor (nulo si no hubo memoria).
  31  0x638570  CTaskComplexUseEffect destructor con borrado (vtable 86E0D8h): como 1 con el cuerpo 632270 y CTask::operator delete 61A5B0

Los falsos anotan: constructores y cuerpos [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar]; operator delete [direccion, puntero (etiqueta), dword del puntero]; operator new [direccion, tamano];
constructor 48E750 [direccion, ECX, argumento]; 61A900 [direccion, ECX, seis dwords]; 61ABE0 [direccion, ECX, ocho dwords]; rand() 821B1E [direccion]; GetTimeLeft [direccion, ECX]; ShouldLoad [direccion];
GetTaskType de la subtarea [94, ECX]. Las llamadas a ranuras no esperadas de la vtable propia son [90 + k, ECX] y no deben aparecer.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = [
    "CEventHealthLow::CloneEditable", "CEventHealthLow::ScalarDeletingDestructor", "CEventHealthReallyLow::Constructor", "CEventHealthReallyLow::DestructorBody",
    "CEventHealthReallyLow::CloneEditable", "CEventHealthReallyLow::ScalarDeletingDestructor", "CEventLowAngerAtPlayer::Constructor", "CEventLowAngerAtPlayer::DestructorBody",
    "CEventLowAngerAtPlayer::CloneEditable", "CEventLowAngerAtPlayer::ScalarDeletingDestructor", "CEventHighAngerAtPlayer::Constructor", "CEventHighAngerAtPlayer::DestructorBody",
    "CEventHighAngerAtPlayer::CloneEditable", "CEventHighAngerAtPlayer::ScalarDeletingDestructor", "CEventOnFire::Constructor", "CEventOnFire::DestructorBody",
    "CEventOnFire::GetEventType", "CEventOnFire::GetLifeTime", "CEventOnFire::Clone", "CEventOnFire::GetLocalSoundLevel", "CEventOnFire::ScalarDeletingDestructor",
    "CEventEscalator::Constructor", "CEventEscalator::GetEventType", "CEventEscalator::GetLifeTime", "CEventEscalator::DestructorBody", "CEventEscalator::GetEventPriority",
    "CEventSeenCop::GetEventPriority", "CEventOnFire::GetEventPriority", "CEventAcquaintancePedHateBadlyLit::GetEventPriority", "ShouldLoadSunbatheAnims",
    "CTaskComplexSunbathe::CreateSubTask", "CTaskComplexUseEffect::ScalarDeletingDestructor"]
ENTRIES = (0x5FF490, 0x5FF4E0, 0x5FF500, 0x5FF520, 0x5FF550, 0x5FF5A0, 0x5FF5C0, 0x5FF5E0, 0x5FF610, 0x5FF660, 0x5FF680, 0x5FF6A0, 0x5FF6D0, 0x5FF720, 0x5FF740, 0x5FF760,
           0x5FF770, 0x5FF780, 0x5FF790, 0x5FF7E0, 0x5FF7F0, 0x5FF820, 0x5FF840, 0x5FF850, 0x5FF860, 0x5FF870, 0x5FF880, 0x5FF890, 0x5FF8A0, 0x632190, 0x638290, 0x638570)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada

CONST, CONSTF, CTOR, BODY, CLONE, SDD, SHOULD, CREATESUB = 'const constf ctor body clone sdd should createsub'.split()
KIND = [CLONE, SDD, CTOR, BODY, CLONE, SDD, CTOR, BODY, CLONE, SDD, CTOR, BODY, CLONE, SDD, CTOR, BODY,
        CONST, CONST, CLONE, CONSTF, SDD, CTOR, CONST, CONST, BODY, CONST, CONST, CONST, CONST, SHOULD, CREATESUB, SDD]
CONSTS = {16: 0x3A, 17: 0, 22: 0x4A, 23: 0, 25: 0x38, 26: 0x15, 27: 0x42, 28: 0x19}
FLOAT_60 = 0x42700000
EVENT_NEW, EVENT_DELETE, CTASK_NEW, CTASK_DELETE = 0x4B5620, 0x4B5630, 0x61A5A0, 0x61A5B0
CTOR_INFO = {2: (0x4AC450, 0x86CBE8), 6: (0x4AC450, 0x86CC30), 10: (0x4AC450, 0x86CC78), 14: (0x4ABFC0, 0x86CCC0), 21: (0x4ABFC0, 0x86CD00)}   # constructor base y vtable que deja
BODY_INFO = {3: (0x86CBE8, 0x4AC480), 7: (0x86CC30, 0x4AC480), 11: (0x86CC78, 0x4AC480), 15: (0x86CCC0, 0x4AC010), 24: (None, 0x4AC010)}      # vtable que escribe y direccion a la que salta
CLONE_INFO = {0: (0x14, 0x4AC450, 0x86CBA0), 4: (0x14, 0x4AC450, 0x86CBE8), 8: (0x14, 0x4AC450, 0x86CC30), 12: (0x14, 0x4AC450, 0x86CC78), 18: (0xC, 0x4ABFC0, 0x86CCC0)}
SDD_INFO = {1: (0x4AC480, 0x86CBA0, EVENT_DELETE), 5: (0x5FF520, None, EVENT_DELETE), 9: (0x5FF5E0, None, EVENT_DELETE), 13: (0x5FF6A0, None, EVENT_DELETE),
            20: (0x5FF760, None, EVENT_DELETE), 31: (0x632270, None, CTASK_DELETE)}     # direccion del cuerpo (evento), vtable que escribe el cuerpo REAL (solo 1), operator delete
CTOR_PAUSE, CTOR_TIMED, CTOR_ANIM, RAND_EVENT, TIMELEFT, SHOULD_ADDR, SUBTYPE_EVENT = 0x48E750, 0x61ABE0, 0x61A900, 0x821B1E, 0x62EB80, 0x632190, 94
THRESHOLD_BITS = 0x3D23D70B


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


def squared_speed_above(x, y, z):
    """(x*x + y*y) + z*z en la x87 a 24 bits y comparado ESTRICTAMENTE con 0.04000000283 (3D23D70B); con NaN es falso (unordenado); con infinito es verdadero"""
    if is_nan(x) or is_nan(y) or is_nan(z):
        return False, 'nan'
    if is_inf(x) or is_inf(y) or is_inf(z):
        return True, 'inf'
    fx, fy, fz = to_fraction(x), to_fraction(y), to_fraction(z)
    t = round24(round24(fx * fx) + round24(fy * fy))
    t = round24(t + round24(fz * fz))
    return t > to_fraction(THRESHOLD_BITS), 'finite'


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
    if _kind in (CTOR, BODY):
        _k += ['prev_zero', 'prev_other']
    elif _kind == CLONE:
        _k += ['mem_null', 'mem_set']
    elif _kind == SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == SHOULD:
        _k += ['focus0', 'focus1', 'in_vehicle', 'not_in_vehicle', 'veh_null', 'veh_set', 'above', 'not_above', 'speed_nan', 'speed_inf', 'result_false', 'result_true']
    elif _kind == CREATESUB:
        _k += ['no_sub', 'sub_same', 'sub_diff', 'type_ca', 'type_1a2', 'type_1ac', 'type_1ad', 'type_1ae', 'type_516', 'type_other', 'mem_null', 'mem_set', 'ca_wait', 'ca_random', 'tl_two_calls', 'tl_capped',
               'tl_plain', 'bather_even', 'bather_odd', 'ad_rand0', 'ad_rand2', 'ad_own_type']
    KEYS[_fn] = _k


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
    elif k == CONSTF:
        W.snap()
        ret = FLOAT_60
    elif k == CTOR:
        W.snap()
        base, vt = CTOR_INFO[fn]
        events.append([base, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        A.wr(o, 4, vt)
        ret = W.tagv(this)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == BODY:
        W.snap()
        vt, target = BODY_INFO[fn]
        if vt is not None:
            A.wr(o, 4, vt)
        events.append([target, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == CLONE:
        W.snap()
        size, ctor, vt = CLONE_INFO[fn]
        events.append([EVENT_NEW, size])
        if a[4] == 0:
            ret = 0
            marks.append('mem_null')
        else:
            marks.append('mem_set')
            mem = W.raw(1, a[5])
            events.append([ctor, W.tagv(mem), W.rd(mem, 4)])
            W.wr(mem, 4, a[7])
            W.wr(mem, 4, vt)
            ret = W.tagv(mem)
    elif k == SDD:
        W.snap()
        body, vt, delete = SDD_INFO[fn]
        if vt is not None:
            A.wr(o, 4, vt)
        events.append([body, W.tagv(this), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF
        if flags & 1:
            events.append([delete, W.tagv(this), A.rd(o, 4)])
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        ret = W.tagv(this)
    elif k == SHOULD:
        W.snap()
        idx = a[4]
        flags, vehicle_on, speed = (a[6], a[7] != 0, a[9:12]) if idx == 0 else (a[13], a[14] != 0, a[16:19])
        marks.append('focus0' if idx == 0 else 'focus1')
        in_vehicle = (flags >> 8) & 1
        marks.append('in_vehicle' if in_vehicle else 'not_in_vehicle')
        marks.append('veh_set' if vehicle_on else 'veh_null')
        ret = 1
        if in_vehicle and vehicle_on:
            above, why = squared_speed_above(*speed)
            marks.append('above' if above else 'not_above')
            if why != 'finite':
                marks.append('speed_' + why)
            if above:
                ret = 0
        marks.append('result_true' if ret else 'result_false')
    elif k == CREATESUB:
        t = a[4]
        sub = 0 if a[6] == 0 else W.raw(1, a[7])
        W.snap()
        blk24, blk28 = (a[10] & 0xFF) != 0, (a[12] & 0xFF) != 0     # byte +10h del bloque de playa (+24h) y del de tomar el sol (+28h)
        bather = a[9]
        timer = W.tagv((this + 0x14) & M32)
        tl = [a[19], a[20]]
        tl_calls = [0]

        def time_left():
            events.append([TIMELEFT, timer])
            v = tl[0 if tl_calls[0] == 0 else 1]
            tl_calls[0] += 1
            return v

        def alloc(size):
            events.append([CTASK_NEW, size])
            if a[14] == 0:
                marks.append('mem_null')
                return None
            marks.append('mem_set')
            return W.raw(3, a[15])

        def construct(mem, addr, args):
            events.append([addr, W.tagv(mem)] + [x & M32 for x in args])
            W.wr(mem, 4, a[17])
            return W.tagv(a[16])

        def anim(base_id, type_id, name, hold):
            mem = alloc(0x20)
            if mem is None:
                return 0
            return construct(mem, CTOR_ANIM, [0x30, (bather + base_id) & M32, 0x40800000, type_id, name, hold])

        done = False
        if sub:
            events.append([SUBTYPE_EVENT, W.tagv(sub)])
            if a[8] == t:
                marks.append('sub_same')
                ret = W.tagv(sub)
                done = True
            else:
                marks.append('sub_diff')
        else:
            marks.append('no_sub')
        if not done:
            ts = signed32(t)
            if ts > 0x1AD:
                if t == 0x1AE:
                    marks.append('type_1ae')
                    ret = anim(0xF9, 0x1AE, 0x86E634, 0)
                else:
                    marks.append('type_516' if t == 0x516 else 'type_other')
                    ret = 0
            elif t == 0x1AD:
                marks.append('type_1ad')
                if bather == 0 or bather == 2:
                    events.append([RAND_EVENT])
                    marks.append('ad_rand0' if bather == 0 else 'ad_rand2')
                    ident = (rand_in_range(a[18], 0, 3) + (0xFE if bather == 0 else 0x101)) & M32
                else:
                    ident = 0x1AD
                    marks.append('ad_own_type')
                mem = alloc(0x20)
                ret = 0 if mem is None else construct(mem, CTOR_ANIM, [0x30, ident, 0x40800000, 0x1AD, 0x86E644, 1])
            elif t == 0xCA:
                marks.append('type_ca')
                wait = False
                if blk28:
                    check_beach = True
                else:
                    events.append([SHOULD_ADDR])
                    check_beach = (a[21] & 0xFF) == 0
                    wait = not check_beach
                if check_beach:
                    wait = not blk24
                mem = alloc(0x18)
                if mem is None:
                    ret = 0
                elif wait:
                    marks.append('ca_wait')
                    ret = construct(mem, CTOR_PAUSE, [10000])
                else:
                    marks.append('ca_random')
                    events.append([RAND_EVENT])
                    ret = construct(mem, CTOR_PAUSE, [rand_in_range(a[18], 1000, 5000)])
            elif t == 0x1A2:
                marks.append('type_1a2')
                ident = (bather + 0xEF) & M32
                marks.append('bather_even' if bather in (0, 2) else 'bather_odd')
                if blk28 and bather in (0, 2):
                    first = (time_left() + 0x3E8) & M32
                    if signed32(first) > 0x2EE0:
                        limit = 0x2EE0
                        marks.append('tl_capped')
                    else:
                        limit = (time_left() + 0x3E8) & M32
                        marks.append('tl_two_calls')
                    events.append([RAND_EVENT])
                    duration = rand_in_range(a[18], 0xBB8, limit)
                else:
                    duration = (time_left() + 0x3E8) & M32
                    marks.append('tl_plain')
                mem = alloc(0x34)
                ret = 0 if mem is None else construct(mem, CTOR_TIMED, [0x2F, ident, 0x40800000, 0xC0800000, duration, 0x1A2, 0x86E098, 1])
            elif t == 0x1AC:
                marks.append('type_1ac')
                ret = anim(0xF4, 0x1AC, 0x86E654, 1)
            else:
                marks.append('type_other')
                ret = 0
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
