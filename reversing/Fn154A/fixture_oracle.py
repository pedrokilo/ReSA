"""Oraculo independiente (Python) de Fn154A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 15 funciones del lote 154A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) y la aritmetica x87
de 24 bits (fracciones exactas) de CEventObjectCollision::IsHeadOnCollision.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x6644C0  CTaskComplexSignalAtPed (vtable 0x86FA8C) destructor con borrado: push esi; mov esi,ecx; call 660AB0; test byte [esp+8],1; je +; push esi; call 61A5B0; add esp,4; mov eax,esi; pop esi; ret 4
   1  0x6644E0  CTaskGoToVehicleAndLean (vtable 0x86FAC8): cuerpo 660EE0
   2  0x67CB50  CTaskComplexFallAndGetUp (vtable 0x870454): cuerpo 678790 (`mov [ecx],870454h; jmp 61A3D0`)
   3  0x678300  CTaskSimpleFall::GetTaskType (vtable 0x870430, ranura 4): mov eax,0CFh; ret
   4  0x678310  CTaskSimpleFall cuerpo del destructor (SEH): [esi]=870430h; si [esi+14h]: push 0; push 4D1390h; call 4CEBE0 (SetFinishCallback, ret 8, ecx = anim); call 61A3A0 (~CTask: mov [ecx],86D48Ch)
   5  0x4C3740  CEventHandlerHistory::Flush (this = historial): ecx=[esi+8]; si: push 1; call [[ecx]]; ecx=[esi+4]; [esi+8]=0; si: delete virtual; ecx=[esi+0Ch]; [esi+4]=0; si: delete virtual; [esi+0Ch]=0; ret
   6  0x4C3E30  idem (copia)
   7  0x4ACDA0  CEventObjectCollision::GetEventType (vtable 0x85ACA8, ranura 1): mov eax,5; ret
   8  0x4ACDC0  GetEventPriority (ranura 2): mov eax,39h; ret
   9  0x4ACDB0  GetLifeTime (ranura 3): xor eax,eax; ret
  10  0x4ACEF0  TakesPriorityOver (ranura 11): mov al,1; ret 4
  11  0x4ACE80  CEventObjectCollision::IsHeadOnCollision(ped): v = (this[+18h], this[+1Ch], 0); call 59C910 (Normalise, ecx = &v); fwd = [[ped+14h]+10h..]; fld vy; fmul fy; fld vz; fmul fz; faddp; fld vx; fmul fx; faddp; fchs;
            fcomp [85AAE4h] (0.9397f); AL = 1 si -dot > 0.9397f (orden de comparacion; NaN -> 0); ret 4
  12  0x602240  CPedIntelligence::AddInterestingEntity(entity): para i = 2, 1: si [+288h+4i]: CleanUpOldReference (571A00, ecx = valor, arg = &casilla); casilla = casilla[i-1]; si no nula: RegisterReference (571B70, ecx = valor, arg = &casilla);
            despues lo mismo con la casilla 0 y el argumento; ret 4
  13  0x59BD90  CMatrix::AttachRW(matriz, bandera): si [+40h] y dword [+44h]: call 7F2A20 (RwMatrixDestroy, cdecl); [+40h] = matriz; [+44h] = byte de la bandera; si matriz: call 59AD70 (UpdateRwMatrix, ecx = this, arg = matriz); ret 8
  14  0x506CC0  copia de 0x5DE9E0: mov [ecx],859988h; add ecx,8; jmp 4EF660 (~CAESound sobre m_tempSound, ecx = this+8)

Los falsos anotan: cuerpos de destructor [direccion, ECX (etiqueta), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero (etiqueta), dword del puntero]; SetFinishCallback [direccion, ECX,
callback, dato]; destructor virtual falso de Flush [identificador, ECX, bandera, +4, +8, +0Ch del historial en ese instante]; Normalise [direccion, x, y, z de entrada]; CleanUp y RegisterReference [direccion, ECX, argumento];
RwMatrixDestroy [direccion, puntero]; UpdateRwMatrix [direccion, ECX, argumento]; ~CAESound [direccion, ECX].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CTaskComplexSignalAtPed::ScalarDeletingDestructor', 'CTaskGoToVehicleAndLean::ScalarDeletingDestructor', 'CTaskComplexFallAndGetUp::ScalarDeletingDestructor',
         'CTaskSimpleFall::GetTaskType', 'CTaskSimpleFall::DestructorBody', 'CEventHandlerHistory::Flush', 'CEventHandlerHistory::FlushCopy',
         'CEventObjectCollision::GetEventType', 'CEventObjectCollision::GetEventPriority', 'CEventObjectCollision::GetLifeTime', 'CEventObjectCollision::TakesPriorityOver',
         'CEventObjectCollision::IsHeadOnCollision', 'CPedIntelligence::AddInterestingEntity', 'CMatrix::AttachRW', 'CAEAudioEntity::Destructor-3']
ENTRIES = (0x6644C0, 0x6644E0, 0x67CB50, 0x678300, 0x678310, 0x4C3740, 0x4C3E30, 0x4ACDA0, 0x4ACDC0, 0x4ACDB0, 0x4ACEF0, 0x4ACE80, 0x602240, 0x59BD90, 0x506CC0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
SETFINISH, EXE_DEFAULT_ANIM_CB, TASK_DTOR, CLEANUP, REGREF, DESTROY, UPDATERW, NORMALISE, SOUNDDTOR, EV_DTOR = 0x4CEBE0, 0x4D1390, 0x61A3A0, 0x571A00, 0x571B70, 0x7F2A20, 0x59AD70, 0x59C910, 0x4EF660, 0x7E1EA5E1
THRESH = 0x3F70902E

CONST, CONSTAL1, SDD, FALLBODY, FLUSH, HEADON, ADDINT, ATTACH, AUDIODTOR = 'const constal1 sdd fallbody flush headon addint attach audiodtor'.split()
KIND = [SDD, SDD, SDD, CONST, FALLBODY, FLUSH, FLUSH, CONST, CONST, CONST, CONSTAL1, HEADON, ADDINT, ATTACH, AUDIODTOR]
SPANS = [4, 4, 4, 4, 0x18, 0x1C, 0x1C, 4, 4, 4, 4, 0x30, 0x294, 0x48, 0x7C]
CONSTANT = {3: 0xCF, 7: 5, 8: 0x39, 9: 0}
BODY = {0: 0x660AB0, 1: 0x660EE0, 2: 0x678790}
DELETE = 0x61A5B0
FALL_VTABLE = 0x870430


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


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con fracciones exactas (IsHeadOnCollision)
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


def cls(bits):
    """clase de un float: ('nan',) ('inf', signo) ('fin', valor exacto, signo)"""
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return ('nan',) if m else ('inf', s)
    v = Fraction(m, 1 << 149) if e == 0 else Fraction((m | 0x800000) << e, 1 << 150)
    return ('fin', -v if s else v, s)


def f_mul(a, b):
    if a[0] == 'nan' or b[0] == 'nan':
        return ('nan',)
    za = a[0] == 'fin' and a[1] == 0
    zb = b[0] == 'fin' and b[1] == 0
    if (a[0] == 'inf' and zb) or (b[0] == 'inf' and za):
        return ('nan',)
    if a[0] == 'inf' or b[0] == 'inf':
        sa = a[1] if a[0] == 'inf' else (1 if a[1] < 0 or (a[1] == 0 and a[2]) else 0)
        sb = b[1] if b[0] == 'inf' else (1 if b[1] < 0 or (b[1] == 0 and b[2]) else 0)
        return ('inf', sa ^ sb)
    p = a[1] * b[1]
    if p == 0:
        return ('fin', Fraction(0), a[2] ^ b[2])
    return ('fin', round_sig(p, 24), 1 if p < 0 else 0)


def f_add(a, b):
    if a[0] == 'nan' or b[0] == 'nan':
        return ('nan',)
    if a[0] == 'inf' and b[0] == 'inf':
        return ('nan',) if a[1] != b[1] else a
    if a[0] == 'inf':
        return a
    if b[0] == 'inf':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('fin', Fraction(0), 1 if (a[1] == 0 and b[1] == 0 and a[2] and b[2]) else 0)
    return ('fin', round_sig(s, 24), 1 if s < 0 else 0)


def f_neg(a):
    if a[0] == 'nan':
        return a
    if a[0] == 'inf':
        return ('inf', a[1] ^ 1)
    return ('fin', -a[1], a[2] ^ 1)


def head_on(nrm_out, fwd):
    """-(((oy*fy) + (oz*fz)) + (ox*fx)) > 0.9397f con la x87 a 24 bits; devuelve (resultado, marcas)"""
    ox, oy, oz = (cls(v) for v in nrm_out)
    fx, fy, fz = (cls(v) for v in fwd)
    t1 = f_mul(oy, fy)
    t2 = f_mul(oz, fz)
    s1 = f_add(t1, t2)
    t3 = f_mul(ox, fx)
    r = f_neg(f_add(s1, t3))
    c = cls(THRESH)[1]
    marks = []
    if r[0] == 'nan':
        marks.append('nan_result')
        return False, marks
    if r[0] == 'inf':
        marks.append('inf_result')
        return r[1] == 0, marks
    if abs(r[1] - c) <= Fraction(1, 1 << 20):
        marks.append('boundary')
    return r[1] > c, marks


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPANS[fn] > WIN:
        marks.append('spill')
    return marks


KEYS = {}
for _fn in range(15):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned', 'spill']
    _kind = KIND[_fn]
    if _kind == SDD:
        _k += ['prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == FALLBODY:
        _k += ['prev_zero', 'prev_other', 'anim_set', 'anim_null']
    elif _kind == FLUSH:
        _k += ['nontemp_set', 'nontemp_null', 'temp_set', 'temp_null', 'stored_set', 'stored_null', 'shared_object']
    elif _kind == HEADON:
        _k += ['result_true', 'result_false', 'nan_result', 'boundary', 'inf_result']
    elif _kind == ADDINT:
        _k += ['slot2_cleanup', 'slot1_cleanup', 'slot0_cleanup', 'entity_null', 'entity_set', 'register_called']
    elif _kind == ATTACH:
        _k += ['destroy_called', 'destroy_skipped_flag', 'destroy_skipped_null', 'update_called', 'update_skipped', 'flag_high_bits']
    elif _kind == AUDIODTOR:
        _k += ['prev_zero', 'prev_other']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == CONST:
        A.wr(o, 4, a[3])
        W.snap()
        ret = CONSTANT[fn]
    elif k == CONSTAL1:
        A.wr(o, 4, a[3])
        W.snap()
        ret = 1
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        dword_in = A.rd(o, 4)
        events.append([BODY[fn], tag(0, o), dword_in])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([DELETE, tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == FALLBODY:
        A.wr(o, 4, a[3])
        A.wr(o + 0x14, 4, a[6])
        W.snap()
        A.wr(o, 4, FALL_VTABLE)
        if a[6]:
            events.append([SETFINISH, W.tagv(a[6]), EXE_DEFAULT_ANIM_CB, 0])
        events.append([TASK_DTOR, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
        marks.append('anim_set' if a[6] else 'anim_null')
    elif k == FLUSH:
        objs = [W.raw(1, a[6]), W.raw(1, a[7]), W.raw(1, a[8])]
        vt = W.raw(2, a[9])
        non = objs[0] if a[3] else 0
        tmp = {0: 0, 1: objs[1], 2: objs[0]}[a[4]]
        sto = {0: 0, 1: objs[2], 2: objs[0], 3: tmp}[a[5]]
        A.wr(o, 4, a[10])
        A.wr(o + 4, 4, non)
        A.wr(o + 8, 4, tmp)
        A.wr(o + 0xC, 4, sto)
        for ob in set(x for x in (non, tmp, sto) if x):
            W.wr(ob, 4, vt)
        W.snap()
        marks.append('nontemp_set' if non else 'nontemp_null')
        marks.append('temp_set' if tmp else 'temp_null')
        marks.append('stored_set' if sto else 'stored_null')
        if len([x for x in (non, tmp, sto) if x]) != len(set(x for x in (non, tmp, sto) if x)):
            marks.append('shared_object')

        def dtor(ptr):
            events.append([EV_DTOR, W.tagv(ptr), 1, W.tagv(A.rd(o + 4, 4)), W.tagv(A.rd(o + 8, 4)), W.tagv(A.rd(o + 0xC, 4))])
        t_, n_, s_ = A.rd(o + 8, 4), A.rd(o + 4, 4), A.rd(o + 0xC, 4)
        if t_:
            dtor(t_)
        A.wr(o + 8, 4, 0)
        if n_:
            dtor(n_)
        A.wr(o + 4, 4, 0)
        if s_:
            dtor(s_)
            A.wr(o + 0xC, 4, 0)
    elif k == HEADON:
        ped, mat = W.raw(1, a[6]), W.raw(2, a[7])
        for i in range(3):
            A.wr(o + 0x18 + 4 * i, 4, a[3 + i])
            Cc.wr(a[7] + 0x10 + 4 * i, 4, a[8 + i])
        B.wr(a[6] + 0x14, 4, mat)
        W.snap()
        events.append([NORMALISE, a[3], a[4], 0])
        res, mk = head_on([a[16], a[17], a[18]], [a[8], a[9], a[10]])
        marks += mk
        marks.append('result_true' if res else 'result_false')
        ret = 1 if res else 0
    elif k == ADDINT:
        slots = [a[3], a[4], a[5]]
        for i in range(3):
            A.wr(o + 0x288 + 4 * i, 4, slots[i])
        W.snap()
        for i in (2, 1):
            if slots[i]:
                events.append([CLEANUP, W.tagv(slots[i]), tag(0, o + 0x288 + 4 * i)])
                marks.append('slot%d_cleanup' % i)
            slots[i] = slots[i - 1]
            if slots[i]:
                events.append([REGREF, W.tagv(slots[i]), tag(0, o + 0x288 + 4 * i)])
                marks.append('register_called')
        if slots[0]:
            events.append([CLEANUP, W.tagv(slots[0]), tag(0, o + 0x288)])
            marks.append('slot0_cleanup')
        slots[0] = a[6]
        if slots[0]:
            events.append([REGREF, W.tagv(slots[0]), tag(0, o + 0x288)])
            marks.append('register_called')
        marks.append('entity_set' if a[6] else 'entity_null')
        for i in range(3):
            A.wr(o + 0x288 + 4 * i, 4, slots[i])
    elif k == ATTACH:
        A.wr(o + 0x40, 4, a[3])
        A.wr(o + 0x44, 4, a[4])
        W.snap()
        if a[3] and a[4]:
            events.append([DESTROY, W.tagv(a[3])])
            marks.append('destroy_called')
        else:
            marks.append('destroy_skipped_null' if not a[3] else 'destroy_skipped_flag')
        A.wr(o + 0x40, 4, a[5])
        A.wr(o + 0x44, 4, a[6] & 0xFF)
        if a[5]:
            events.append([UPDATERW, tag(0, o), W.tagv(a[5])])
            marks.append('update_called')
        else:
            marks.append('update_skipped')
        if a[6] >> 8:
            marks.append('flag_high_bits')
    elif k == AUDIODTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0x859988)
        events.append([SOUNDDTOR, tag(0, o + 8)])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
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
