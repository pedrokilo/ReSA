"""Oraculo independiente (Python) de Fn85H: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x61D6F0..0x61DACE, lo que debe observarse de
CTaskSimpleFight::TargetInReach (thiscall, ret 4; devuelve AL) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) y el fpatan (90 digitos decimales, sin redondear hasta la operacion siguiente) se modelan aqui.

Modelo (desensamblado):
  0x56D270 (KickingWhenDown): si AL = 0 devuelve falso. alcance lejano = ((0xC177EC + fila[combo].+0x3C) + 0.2f) y cercano = ((0xC177EC + fila[combo].+0x3C) + 0.4f), ambos a 24 bits y guardados como float.
  Con objetivo (+0x18 de la tarea): si (byte +0x36 & 7) != 3 devuelve (objetivo == +0x568 del ped). Si es un ped: hueso 3 (0x5E4280); vivo (0x5E0170) y NO (z del ped - 0.2f > z del hueso): sin la bandera 0x4000000 de +0x46C
  del objetivo (releido) devuelve falso, con ella el alcance cercano pasa a (cercano - 0.4f). Tarea mas simple del objetivo (0x6819D0): si existe, GetTaskType (ranura 4) == 0xCF y tiene animacion: tiempo total > actual devuelve falso.
  0x411990 (avance): |((objetivo - ped) . avance)| < cercano (registros de 24 bits, suma ((z + y) + x)); si no, falso. 0x411990 (derecha): delta guardado como float (0x40FE60) y |delta . derecha| < lejano (0x40FDB0) da el resultado.
  Sin objetivo: solo si 0x5DF8F0 (jugador). Recorre los 16 peds cercanos (+0x47C -> +0x130): 0x5E4280 de cada uno; si su estado (+0x530) != 0x37 exige (z del ped - 0.2f > z del hueso); dx y dy guardados como float, dz en el registro;
  |dz*fz + dy*fy + dx*fx| < cercano y |dz*rz + dy*ry + dx*rx| < lejano (avance +0x10 y derecha +0 de la matriz de cada ped); rumbo = fpatan(-dx, dy) guardado como float; d = rumbo - rumbo del ped (+0x558) (24 bits);
  d < -pi: d + 2pi, si no d > pi: d - 2pi; d < 0: d * -1; d < 1.0472f; el mejor (|d| menor, o un candidato con vida > 0.0 si el mejor tiene vida <= 0) se guarda; al final +0x55C del ped = su rumbo (float; el EXE hace pop ebp antes de leer [esp+1Ch]) y devuelve verdadero.
  Si no hay ninguno: verdadero solo si el objetivo apuntado del ped (+0x568) es un objeto (byte +0x36 & 7 == 2) sin el dword +0x590.
  La posicion es la de la matriz (+0x14, en +0x30) si la hay, si no la compacta (+4). Las mutaciones (una vez) cambian la memoria que el EXE relee tras la llamada de un colaborador.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)

# ---------------------------------------------------------------- aritmetica x87 a 24 bits
def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def sqrt24(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def reload(bits):
    return NAN if bits is None else load(bits)


def fromint(j):
    return ('f', Fraction(j), 1 if j < 0 else 0) if j else ('f', Fraction(0), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def lt(a, b):
    """a < b ordenado (fcomp + test ah,5 / jp): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) < num(b)


def gt(a, b):
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)



# ---------------------------------------------------------------- arcotangente (fpatan(x, 1) con precision mucho mayor que la de 64 bits de la x87)
from decimal import Decimal, getcontext
getcontext().prec = 90


def _atan_small(x):
    x2 = x * x
    term = x
    s = x
    k = 1
    eps = Decimal(10) ** -85
    while True:
        term *= -x2
        k += 2
        t = term / k
        if abs(t) < eps:
            return s
        s += t


def _pi():
    return 16 * _atan_small(Decimal(1) / 5) - 4 * _atan_small(Decimal(1) / 239)


PI_DEC = _pi()


def atan_dec(x):
    if x == 0:
        return Decimal(0)
    neg = x < 0
    x = abs(x)
    inv = x > 1
    if inv:
        x = 1 / x
    k = 0
    while x > Decimal('0.05'):
        x = x / (1 + (1 + x * x).sqrt())
        k += 1
    r = _atan_small(x) * (2 ** k)
    if inv:
        r = PI_DEC / 2 - r
    return -r if neg else r


def fpatan(a):
    """fld a; fld1; fpatan: atan(a / 1) SIN redondear a 24 bits (la x87 de 64 bits)"""
    if a == NAN:
        return NAN
    if a[0] == 'i':
        v = Fraction(PI_DEC / 2)
        return ('f', v if a[1] == 0 else -v, a[1])
    v = a[1]
    if v == 0:
        return ('f', Fraction(0), a[2])
    r = atan_dec(Decimal(v.numerator) / Decimal(v.denominator))
    f = Fraction(r)
    return ('f', f, 1 if f < 0 else 0)



# ---------------------------------------------------------------- modelo de CTaskSimpleFight::ChooseAttackPlayer
def fpatan2(y, x):
    """fpatan: atan2(y, x) con y = ST1 y x = ST0, SIN redondear a 24 bits (la x87 de 64 bits); y, x finitos"""
    ys = y[2]
    xs = x[2]
    if y[1] == 0:
        if x[1] > 0 or (x[1] == 0 and xs == 0):
            return ('f', Fraction(0), ys)
        v = Fraction(PI_DEC)
        return ('f', -v if ys else v, ys)
    if x[1] == 0:
        v = Fraction(PI_DEC / 2)
        return ('f', -v if y[1] < 0 else v, 1 if y[1] < 0 else 0)
    r = atan_dec(Decimal(y[1].numerator) / Decimal(y[1].denominator) / (Decimal(x[1].numerator) / Decimal(x[1].denominator)))
    if x[1] < 0:
        r = r + PI_DEC if y[1] > 0 else r - PI_DEC
    f = Fraction(r)
    return ('f', f, 1 if f < 0 else 0)


# ---------------------------------------------------------------- modelo de CTaskSimpleFight::TargetInReach
def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


GUARD = 0x40
M32 = 0xFFFFFFFF
ARENA_TASK, ARENA_PED, ARENA_TGT, ARENA_SMAT, ARENA_TMAT = 0, 1, 2, 4, 5
ARENA_OBJ, ARENA_OTH, ARENA_OMAT = 15, 16, 32
ARENA_FTASK = 10
ARENA_TINT = 8
ARENA_RET1 = 13


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


FAR_ADD = load(0x3E4CCCCD)      # 0.2f (0x858CC4)
NEAR_ADD = load(0x3ECCCCCD)     # 0.4f (0x858EE8)
PI_F = load(0x40490FDB)         # 0x858CB8
TWO_PI_F = load(0x40C90FDB)     # 0x858CBC
MPI_F = load(0xC0490FDB)        # 0x858CC0
MINUS_ONE = load(0xBF800000)    # 0x858C1C
MAX_TURN = load(0x3F860A92)     # 0x8630F8 = 1.0472f
ZERO_F = load(0)
BEST0 = 0x40490FDB              # pi: valor inicial de [esp+1Ch]


class Mem:
    """Memoria modelada: (arena, desplazamiento) -> byte. Solo los campos que lee o escribe la funcion o las mutaciones."""

    def __init__(self):
        self.cur = {}

    def put32(self, arena, off, value):
        for j in range(4):
            self.cur[(arena, off + j)] = (value >> (8 * j)) & 0xFF

    def get32(self, arena, off):
        return sum(self.cur[(arena, off + j)] << (8 * j) for j in range(4))


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    mem = Mem()
    # --- estado inicial
    for e in range(2):
        a, am = ARENA_TGT + e, ARENA_TMAT + e
        mat = bool(c['emat'][e])
        pos = [v & M32 for v in c['epos'][e * 3:e * 3 + 3]]
        alt = [v & M32 for v in c['ealt'][e * 3:e * 3 + 3]]
        for k in range(3):
            mem.put32(am if mat else a, (0x30 if mat else 4) + 4 * k, pos[k])
            mem.put32(a if mat else am, (4 if mat else 0x30) + 4 * k, alt[k])
        mem.put32(a, 0x46C, c['eflags'][e] & M32)
    pmat = bool(c['pmat'])
    for k in range(3):
        mem.put32(ARENA_SMAT if pmat else ARENA_PED, (0x30 if pmat else 4) + 4 * k, c['ppos'][k] & M32)
        mem.put32(ARENA_PED if pmat else ARENA_SMAT, (4 if pmat else 0x30) + 4 * k, c['palt'][k] & M32)
    mem.put32(ARENA_PED, 0x558, c['rot'] & M32)
    mem.put32(ARENA_PED, 0x55C, c['aim'] & M32)
    for i in range(16):
        mem.put32(ARENA_OTH + i, 0x530, c['ostate'][i] & M32)
        mem.put32(ARENA_OTH + i, 0x540, c['ohealth'][i] & M32)
        for k in range(3):
            mem.put32(ARENA_OMAT + i, 0x30 + 4 * k, c['opos'][i * 3 + k] & M32)
    init = dict(mem.cur)
    mats = {'ped': pmat, 0: bool(c['emat'][0]), 1: bool(c['emat'][1])}
    st = {'tp': 0 if c['tmode'] else None, 'target_tag': tag(ARENA_TGT, 0) if c['tmode'] else 0}
    site = c['mutsite']
    ma, mb, mc, md = c['muta'] & M32, c['mutb'] & M32, c['mutc'] & M32, c['mutd'] & M32
    mutpos = [v & M32 for v in c['mutpos']]
    done = set()

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def ent_arenas(who):
        if who == 'ped':
            return ARENA_PED, ARENA_SMAT
        return ARENA_TGT + who, ARENA_TMAT + who

    def pos_of(who):
        a, am = ent_arenas(who)
        if mats[who]:
            return [mem.get32(am, 0x30 + 4 * k) for k in range(3)]
        return [mem.get32(a, 4 + 4 * k) for k in range(3)]

    def put_pos(who, bits):
        a, am = ent_arenas(who)
        for k in range(3):
            if mats[who]:
                mem.put32(am, 0x30 + 4 * k, bits[k])
            else:
                mem.put32(a, 4 + 4 * k, bits[k])

    def toggle(who):
        mats[who] = not mats[who]

    def switch_target():
        st['tp'] = 1
        st['target_tag'] = tag(ARENA_TGT + 1, 0)

    bone_calls = [0]
    mat_calls = [0]

    def bone_call(self_kind, self_idx):
        """self_kind 'tgt' (self_idx = 0/1) u 'oth' (self_idx = k)"""
        arena = ARENA_TGT + self_idx if self_kind == 'tgt' else ARENA_OTH + self_idx
        ev.append([2, tag(arena, 0), 3, 0])
        idx = bone_calls[0]
        bone_calls[0] += 1
        if idx == (ma & 15):
            if once(1):
                if mb & 1:
                    toggle('ped')
                else:
                    put_pos('ped', mutpos)
            if once(2):
                if self_kind == 'oth':
                    for k in range(3):
                        mem.put32(ARENA_OMAT + self_idx, 0x30 + 4 * k, mutpos[k])
                else:
                    put_pos(self_idx, mutpos)
            if once(3):
                mem.put32(ARENA_PED, 0x558, mb)
            if self_kind == 'oth' and once(4):
                mem.put32(ARENA_OTH + self_idx, 0x530, mb)
            if once(5):
                mem.put32(ARENA_OTH + (mc & 15), 0x540, mb)
        if site == 6 and md % 5 == 0 and bone_calls[0] == 1 and 6 not in done:
            once(6)
            switch_target()

    kick_al = c['kick'] & 0xFF
    ev.append([1])
    ret = False
    if kick_al == 0:
        mark('kick_off')
        return finish(c, mem, init, ev, marks, st, mats, False)
    rowsum = add(load(c['limit'] & M32), load(c['rowf'][c['combo']] & M32))
    far_b = store(add(FAR_ADD, rowsum))
    near_b = store(add(rowsum, NEAR_ADD))
    far = reload(far_b)
    near = reload(near_b)

    if c['tmode']:
        tp = st['tp']
        ttype = c['etype'][tp] & 0xFF
        if (ttype & 7) != 3:
            mark('t_other')
            if c['ptarg'] == 1:
                mark('t_other_same')
                ret = True
            else:
                mark('t_other_diff')
                ret = False
            return finish(c, mem, init, ev, marks, st, mats, ret)
        mark('t_ped')
        bone_z = [c['ebone'][tp * 3 + 2] & M32]
        bone_call('tgt', tp)
        bz = load(bone_z[0])
        e = st['tp']
        ev.append([3, tag(ARENA_TGT + e, 0)])
        if site == 6 and md % 5 == 1 and once(6):
            switch_target()
        if once(7):
            mem.put32(ARENA_TGT + st['tp'], 0x46C, mb)
        if (c['alive'] & 0xFF) != 0:
            mark('alive')
            pz = pos_of('ped')[2]
            if gt(sub(load(pz), FAR_ADD), bz):
                mark('z_above')
            else:
                mark('z_below')
                flags = mem.get32(ARENA_TGT + st['tp'], 0x46C)
                if not flags & 0x4000000:
                    mark('flag_missing')
                    return finish(c, mem, init, ev, marks, st, mats, False)
                mark('flag_set')
                near_b = store(sub(reload(near_b), NEAR_ADD))
                near = reload(near_b)
        else:
            mark('dead')
        e = st['tp']
        ev.append([5, tag(ARENA_TINT + e, 4)])
        if site == 6 and md % 5 == 2 and once(6):
            switch_target()
        if c['smode']:
            ev.append([7, tag(ARENA_FTASK, 0)])
            if (c['stype'] & M32) == 0xCF:
                mark('task_cf')
                if c['smode'] == 3:
                    mark('task_anim')
                    if gt(load(c['atotal'] & M32), load(c['acur'] & M32)):
                        mark('anim_blocks')
                        return finish(c, mem, init, ev, marks, st, mats, False)
                    mark('anim_free')
                else:
                    mark('task_noanim')
            else:
                mark('task_other_type')
        else:
            mark('task_none')
        # primer producto: avance del objetivo
        e = st['tp']
        ev.append([6, tag(ARENA_TGT + e, 0)])
        mat_calls[0] += 1
        if site == 6 and md % 5 == 3 and once(6):
            switch_target()
        if once(8):
            toggle(st['tp'])
        e5 = st['tp']
        pp = pos_of('ped')
        tpp = pos_of(e5)
        dx = sub(load(tpp[0]), load(pp[0]))
        dy = sub(load(tpp[1]), load(pp[1]))
        dz = sub(load(tpp[2]), load(pp[2]))
        f = [load(v & M32) for v in c['m1f']]
        dot1 = add(add(mul(dz, f[2]), mul(dy, f[1])), mul(dx, f[0]))
        if not lt(babs(dot1), near):
            mark('dot1_fail')
            return finish(c, mem, init, ev, marks, st, mats, False)
        mark('dot1_ok')
        # segundo producto: derecha del objetivo con el delta guardado como float
        ev.append([6, tag(ARENA_TGT + e5, 0)])
        mat_calls[0] += 1
        if site == 6 and md % 5 == 4 and once(6):
            switch_target()
        e6 = st['tp']
        pp = pos_of('ped')
        tpp = pos_of(e6)
        d = [reload(store(sub(load(tpp[k]), load(pp[k])))) for k in range(3)]
        r = [load(v & M32) for v in c['m2r']]
        dot2 = add(add(mul(d[2], r[2]), mul(d[1], r[1])), mul(d[0], r[0]))
        if lt(babs(dot2), far):
            mark('dot2_ok')
            ret = True
        else:
            mark('dot2_fail')
            ret = False
        return finish(c, mem, init, ev, marks, st, mats, ret)

    # ----------------------------------------------------------- sin objetivo
    ev.append([4, tag(ARENA_PED, 0)])
    if (c['isplayer'] & 0xFF) == 0:
        mark('not_player')
        return finish(c, mem, init, ev, marks, st, mats, False)
    mark('scan')
    best = None
    best_diff = BEST0
    best_angle = 0
    found = 0
    for i in range(16):
        if not c['opres'][i]:
            continue
        bone_call('oth', i)
        bz = load(c['obone'][i * 3 + 2] & M32)
        state = mem.get32(ARENA_OTH + i, 0x530)
        if state != 0x37:
            pz = pos_of('ped')[2]
            if not gt(sub(load(pz), FAR_ADD), bz):
                mark('scan_z_fail')
                continue
            mark('scan_z_ok')
        else:
            mark('scan_state37')
        pp = pos_of('ped')
        op = [mem.get32(ARENA_OMAT + i, 0x30 + 4 * k) for k in range(3)]
        dxb = store(sub(load(op[0]), load(pp[0])))
        dyb = store(sub(load(op[1]), load(pp[1])))
        dz = sub(load(op[2]), load(pp[2]))
        dx = reload(dxb)
        dy = reload(dyb)
        rf = c['orf'][i * 6:i * 6 + 6]
        fw = [load(v & M32) for v in rf[3:6]]
        rt = [load(v & M32) for v in rf[0:3]]
        s1 = add(add(mul(dz, fw[2]), mul(dy, fw[1])), mul(dx, fw[0]))
        if not lt(babs(s1), near):
            mark('scan_dot1_fail')
            continue
        s2 = add(add(mul(dz, rt[2]), mul(dy, rt[1])), mul(dx, rt[0]))
        if not lt(babs(s2), far):
            mark('scan_dot2_fail')
            continue
        found += 1
        a = fpatan2(neg(dx), dy)
        angle_b = store(a)
        angle = reload(angle_b)
        rot = load(mem.get32(ARENA_PED, 0x558))
        dd = sub(angle, rot)
        if lt(dd, MPI_F):
            mark('scan_wrap_up')
            dd = add(dd, TWO_PI_F)
        elif gt(dd, PI_F):
            mark('scan_wrap_down')
            dd = sub(dd, TWO_PI_F)
        if lt(dd, ZERO_F):
            mark('scan_negate')
            dd = mul(dd, MINUS_ONE)
        if not lt(dd, MAX_TURN):
            mark('scan_turn_fail')
            continue
        if best is None:
            mark('scan_first')
            accept = True
        else:
            bh = load(mem.get32(ARENA_OTH + best, 0x540))
            ch = load(mem.get32(ARENA_OTH + i, 0x540))
            if bh != NAN and not gt(bh, ZERO_F) and gt(ch, ZERO_F):
                mark('scan_health_swap')
                accept = True
            else:
                accept = lt(dd, reload(best_diff))
                mark('scan_replace' if accept else 'scan_worse')
        if accept:
            best_diff = store(dd)
            best_angle = angle_b
            best = i
    if found == 0:
        mark('scan_none')
    if best is not None:
        mark('scan_store')
        mem.put32(ARENA_PED, 0x55C, best_angle)
        return finish(c, mem, init, ev, marks, st, mats, True)
    if c['ptarg'] == 0:
        mark('targ_null')
        return finish(c, mem, init, ev, marks, st, mats, False)
    ent = 0 if c['ptarg'] == 1 else 2
    ttype = c['etype'][ent] & 0xFF
    if (ttype & 7) != 2:
        mark('targ_not_object')
        return finish(c, mem, init, ev, marks, st, mats, False)
    if c['e590'][ent] & M32:
        mark('targ_object_590')
        return finish(c, mem, init, ev, marks, st, mats, False)
    mark('targ_object_free')
    return finish(c, mem, init, ev, marks, st, mats, True)


def babs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def finish(c, mem, init, ev, marks, st, mats, ret):
    changes = sorted([[a, o, mem.cur[(a, o)]] for (a, o) in mem.cur if mem.cur[(a, o)] != init[(a, o)]])
    marks.append('ret_%d' % (1 if ret else 0))
    return {'ret': 1 if ret else 0, 'target': st['target_tag'], 'mats': [mats['ped'], mats[0], mats[1]], 'changes': changes, 'events': ev, 'marks': marks}


CHANGE_KEYS = ('ret', 'target', 'mats', 'changes')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
