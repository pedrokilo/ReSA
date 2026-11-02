"""Oraculo independiente (Python) de Fn85G: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x624710..0x624A3B, lo que debe observarse de
CTaskSimpleFight::ChooseAttackPlayer (thiscall, ret 4; devuelve AX) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) y el fpatan (90 digitos decimales, sin redondear hasta la operacion siguiente) se modelan aqui.

Modelo (desensamblado):
  ataque = -1; se pone a 1 salvo que el comando siguiente (byte +0x26, con signo) este en 0xB..0xE y el combo (+0x24, con signo) sea >= 4. Banderas = byte +0x84 de la fila del combo (0xC17154 + (combo - 4) * 0x88);
  con combo 5..7 se enmascaran con el byte +0x72E del ped. "Sin objetivo" = dword +0x18 de la tarea nulo (se mira al principio).
  Con ataque -1: si hay animacion (+0x1C != 0) y el movimiento (+0x25, con signo) no es 3 ni 4: CPed::SetMoveState(1) (0x5DEC00); tope = 2 con la bandera 2, 1 con la 1, si no 0; TargetInReach (0x61D6F0): si al alcance, ataque -1;
    si no, con siguiente != ultimo (releidos) y contador (+0x15, con signo) <= 2 (0x8D2E48): contador + 1, y low = (movimiento - ultimo + siguiente) & 0xFF: bit 0 -> 0, bit 1 -> 1, si no 2; ataque = 0 si supera el tope;
    en los demas casos ataque = movimiento + 1, o -1 si supera el tope.
  Sin animacion o con movimiento 3 o 4: si el estado de movimiento del ped (+0x534, con signo) > 4: combo = 4 si falta la bandera 8, ataque 4; si no, TargetInReach: sin alcance contador = 0 y ataque 0; con alcance:
    sale con -1 si hay animacion y siguiente != ultimo (releidos); si no combo = 4 si falta la bandera 4 o hay un objetivo con (byte +0x36 & 7) == 3 y dword +0x46C & 0x4000000, y sale con 3.
  Si no ha salido y hay objetivo, devuelve el ataque. Si no hay objetivo, recorre los 16 peds cercanos (+0x47C -> +0x130): no nulo, CPed::IsAlive (0x5E0170) distinto de cero, distancia cuadrada ((dz^2 + dy^2) + dx^2, cada resta
  y cada producto a 24 bits) < 2.0^2, rumbo = fpatan(-dx, dy) sin redondear; d = rumbo - rumbo del ped (+0x558) (24 bits); d > pi: d - 2pi, si no d < -pi: d + 2pi; |d| < mejor (inicial 1000.0f): mejor = |d| (float) y el rumbo
  se guarda como float. Al final, si el rumbo guardado (inicial -1000.0f) > -10.0f se escribe en +0x55C del ped. Devuelve el ataque (AX).
  La posicion es la de la matriz (+0x14, en +0x30) si la hay, si no la compacta (+4).

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


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def byte_map(d, off, value, n=4):
    for j in range(n):
        d[off + j] = (value >> (8 * j)) & 0xFF


def changes(old, new):
    return [[k, new[k]] for k in sorted(new) if new[k] != old[k]]


TAG_TASK = tag(0, 0)
TAG_PED = tag(1, 0)
PI_F = load(0x40490FDB)       # 0x858CB8
TWO_PI_F = load(0x40C90FDB)   # 0x858CBC
MPI_F = load(0xC0490FDB)      # 0x858CC0
LIMIT2 = mul(load(0x40000000), load(0x40000000))     # 0x8D2E8C ^ 2
BEST0 = 0x447A0000     # 1000.0f
ANGLE0 = 0xC47A0000    # -1000.0f
FOUND = load(0xC1200000)   # -10.0f (0x859004)
CHAIN_LIMIT = 2        # int32 en 0x8D2E48


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    chain, combo, move, nxt, last = c['chain'] & 0xFF, c['combo'] & 0xFF, c['move'] & 0xFF, c['next'] & 0xFF, c['last'] & 0xFF
    anim = c['animptr'] & M32
    tgt = bool(c['tgt'])
    ttype, tflags = c['ttype'] & 0xFF, c['tflags'] & M32
    flags = [v & 0xFF for v in c['flags']]
    p72e, mstate, rot, aim = c['p72e'] & 0xFF, c['mstate'] & M32, c['rot'] & M32, c['aim'] & M32
    site = c['mutsite']
    ma, mb, mc, md = c['muta'] & M32, c['mutb'] & M32, c['mutc'] & M32, c['mutd'] & M32
    reach = (c['reach'] & 0xFF) != 0
    task0 = {0x15: chain, 0x24: combo, 0x25: move, 0x26: nxt, 0x27: last}
    byte_map(task0, 0x1C, anim)
    ped0 = {0x558 + j: (rot >> (8 * j)) & 0xFF for j in range(4)}
    byte_map(ped0, 0x55C, aim)
    ped0[0x72E] = p72e
    tgt0 = {0x36: ttype}
    byte_map(tgt0, 0x46C, tflags)
    done = set()

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def reach_call():
        nonlocal nxt, last, move, anim, ttype, tflags
        ev.append([2, TAG_TASK, TAG_PED])
        if once(1):
            nxt, last, move, anim = ma & 0xFF, mb & 0xFF, mc & 0xFF, md
        if once(2):
            ttype, tflags = ma & 0xFF, mb

    attack = -1
    ret = None
    nx = s8(nxt)
    if not (0xB <= nx <= 0xE and s8(combo) >= 4):
        attack = 1
        mark('start_plus')
    else:
        mark('start_minus')
    cs = s8(combo)
    fl = flags[combo]
    if 4 < cs <= 7:
        fl &= p72e
        mark('flags_masked')
    no_target = not tgt
    if attack < 0:
        if anim != 0 and s8(move) != 3 and s8(move) != 4:
            mark('anim_path')
            ev.append([1, TAG_PED, 1])
            if once(3):
                flags[combo] = ma & 0xFF
                p72e = mb & 0xFF
            cap = 2 if fl & 2 else (1 if fl & 1 else 0)
            mark('cap_%d' % cap)
            reach_call()
            if reach:
                mark('anim_reach')
                attack = -1
            elif nxt != last and not s8(chain) > CHAIN_LIMIT:
                mark('anim_cand')
                chain = (chain + 1) & 0xFF
                low = (s8(move) - s8(last) + s8(nxt)) & 0xFF
                cand = 0 if low & 1 else (1 if low & 2 else 2)
                mark('cand_%d' % cand)
                if cand > cap:
                    mark('cand_capped')
                    attack = 0
                else:
                    attack = cand
            else:
                mark('anim_follow_chain' if nxt != last else 'anim_follow_same')
                foll = s8(move) + 1
                if foll > cap:
                    mark('follow_capped')
                    attack = -1
                else:
                    mark('follow_kept')
                    attack = foll
        elif s32(mstate) > 4:
            mark('run')
            if not fl & 8:
                mark('run_set4')
                combo = 4
            else:
                mark('run_keep')
            attack = 4
        else:
            mark('walk')
            reach_call()
            if not reach:
                mark('noreach_zero')
                chain = 0
                attack = 0
            elif anim != 0 and nxt != last:
                mark('reach_ret_m1')
                ret = -1
            else:
                if not fl & 4:
                    mark('ret3_noflag4')
                    combo = 4
                elif tgt and (ttype & 7) == 3 and (tflags & 0x4000000):
                    mark('ret3_target')
                    combo = 4
                else:
                    mark('ret3_keep')
                ret = 3
    if ret is None:
        if not no_target:
            mark('has_target')
            ret = attack
        else:
            mark('scan')
            lst = [k if c['opres'][k] else None for k in range(16)]
            pos = [[c['opos'][k * 3 + j] & M32 for j in range(3)] for k in range(16)]
            sp = [v & M32 for v in c['spos']]
            best = BEST0
            angle = ANGLE0
            calls = 0
            alive_seen = False
            found = 0
            for i in range(16):
                k = lst[i]
                if k is None:
                    continue
                ev.append([3, tag(5 + k, 0)])
                if calls == (ma & 15):
                    if once(4):
                        pos[k] = [v & M32 for v in c['mutpos']]
                    if once(5):
                        rot = mb
                    if once(6):
                        lst[mc & 15] = None if (mb & 1 and (mc & 15) != i) else (mb >> 1) & 15
                calls += 1
                if (c['oalive'][k] & 0xFF) == 0:
                    mark('scan_dead')
                    continue
                alive_seen = True
                op = pos[lst[i]]     # el EXE relee el elemento de la lista tras la llamada
                dz = sub(load(op[2]), load(sp[2]))
                dy = sub(load(op[1]), load(sp[1]))
                dx = sub(load(op[0]), load(sp[0]))
                d2 = add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx))
                if not gt(LIMIT2, d2):
                    mark('scan_far')
                    continue
                found += 1
                ddx = sub(load(op[0]), load(sp[0]))
                ddy = sub(load(op[1]), load(sp[1]))
                a = fpatan2(neg(ddx), ddy)
                d = sub(a, load(rot))
                if gt(d, PI_F):
                    mark('scan_diff_gt_pi')
                    d = sub(d, TWO_PI_F)
                elif lt(d, MPI_F):
                    mark('scan_diff_lt_mpi')
                    d = add(d, TWO_PI_F)
                if d != NAN:
                    d = ('f', abs(d[1]), 0) if d[0] == 'f' else ('i', 0)
                else:
                    mark('scan_nan_diff')
                if lt(d, load(best)):
                    mark('scan_replace' if best != BEST0 else 'scan_first')
                    best = store(d)
                    angle = store(a)
                else:
                    mark('scan_tie' if (d != NAN and best != BEST0 and num(d) == num(load(best))) else 'scan_worse')
            if found == 0:
                mark('scan_none')
            if gt(load(angle), FOUND):
                mark('scan_store')
                aim = angle
            ret = attack
    task1 = {0x15: chain, 0x24: combo, 0x25: move, 0x26: nxt, 0x27: last}
    byte_map(task1, 0x1C, anim)
    ped1 = {0x558 + j: (rot >> (8 * j)) & 0xFF for j in range(4)}
    byte_map(ped1, 0x55C, aim)
    ped1[0x72E] = p72e
    tgt1 = {0x36: ttype}
    byte_map(tgt1, 0x46C, tflags)
    mark('ret_%d' % s16(ret) if s16(ret) in (-1, 0, 1, 2, 3, 4) else 'ret_other')
    return {'ret': s16(ret), 'task_changes': changes(task0, task1), 'ped_changes': changes(ped0, ped1), 'tgt_changes': changes(tgt0, tgt1), 'events': ev, 'marks': marks}


CHANGE_KEYS = ('ret', 'task_changes', 'ped_changes', 'tgt_changes')


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
