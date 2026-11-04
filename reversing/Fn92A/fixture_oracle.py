"""Oraculo independiente (Python) de Fn92A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de cada funcion del lote 92A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Funcion 0, CPedAcquaintanceScanner::ScanForPedAcquaintances (0x607D80..0x607E1E; thiscall, ECX = escaner; ped, entidades y numero en pila, `ret 0Ch`; void):
  Temporizador en linea (CTaskTimer: +0 inicio, +4 intervalo, +8 arrancado, +9 detenido; los booleanos se leen como bytes):
    si el byte +8 es 0:  intervalo = dword [0x8D2358], inicio = hora (dword [0xB7CB84]), arrancado = 1.
    si el byte +9 != 0:  inicio = hora, detenido = 0.
    si hora < inicio + intervalo (suma de 32 bits sin signo): sale (hora >= suma es el vencimiento).
    inicio = hora, intervalo = dword [0x8D2358], arrancado = 1.
  0x603A30(escaner, ped) (thiscall, `ret 4`); si AL == 0 sale.
  0x607A90(escaner, ped, -1, entidades, numero, &local0, &local1) (thiscall, `ret 0x18`) con local0 = 0 y local1 = -1.

Funcion 1, CNearbyFireScanner::ScanForNearbyFires (0x603E70..0x6040BA; thiscall, ECX = escaner, ped en pila, `ret 4`; void):
  Temporizador en linea como la funcion 0 pero sin arrancar = inicio hora, intervalo 0; al vencer: inicio = hora, intervalo = 100, arrancado = 1.
  GetSimplestActiveTask(taskmgr = inteligencia + 4); si hay tarea e IsGoToTask (tipo 900 o 926; el tipo se pide una vez por comparacion): estado = [tarea + 8], si no 1.
  GetActiveTask(taskmgr); FindNearestFire(0xB71F80, posicion del ped, 0, 0). Con fuego: d = fuego - ped (x87 a 24 bits; dz guardado como float), dist2 = (dy*dy + dx*dx) + dz*dz.
  Primer evento (CEventFireNearby(posicion del fuego), tipo 59): dist2 (registro) < 400.0 y |dz| < 2.0. Segundo (CEventPotentialWalkIntoFire(posicion, tamano, estado), tipo 46):
  tarea activa distinta de nula y de tipo != 514, GetSimplestActiveTask otra vez, IsGoToTask, fuego, 16.0 > dist2 (guardado como float) y |dz| < 2.0.
  Los eventos pasan a CEventGroup::Add(inteligencia + 0x68, evento, 0). Radio del segundo = (tamano < 1.0 ? 0.7 : tamano < 2.0 ? 1.0 : 1.5) + 0.35 (NaN toma 1.5).

Funcion 2, CSexyPedScanner::ScanForSexyPeds (0x603BF0..0x603E66; thiscall, ECX = escaner; ped, entidades y numero en pila, `ret 0Ch`; void):
  Temporizador como la funcion 1 pero con 500 ms al arrancar y al vencer, y 3000 ms tras encontrar candidato. Si createdBy != 2 y el bit 8 de ped+0x46C: sale.
  Para cada entidad (indice con signo < numero): no nula, tipo (+0x598) == 5, atractivo (byte con signo +0x27 de las estadisticas) mayor que el del ped, bit 8 de +0x46C apagado,
  d = entidad - ped (dz guardado como float), dist2 = (dz*dz + dy*dy) + dx*dx < 10000.0 (registro); con datos de jugador (ped+0x480) CInterestingEvents::Add(0xC0B058, 9, entidad);
  dist2 (float) < mejor (inicial 0x501502F9); producto con el frente del ped (matriz +0x10): (dz*f.z + dy*f.y) + dx*f.x > 0; GetIsLineOfSightClear(ped, entidad, 1,0,0,1,0,0,0) con AL != 0 => mejor.
  Con mejor: CEventSexyPed(mejor) (RegisterReference), CEventGroup::Add(inteligencia + 0x68, evento, 0), inicio = hora, intervalo 3000, arrancado = 1, y CleanUpOldReference.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

NAN = ('n',)


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


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / jne): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jp: cero solo si es igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero aqui)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]



FIRE_NEARBY, WALK_INTO_FIRE, SEXY_PED = 59, 46, 20
ONE = load(0x3F800000)
TWO = load(0x40000000)
ZERO_F = load(0)
K400 = load(0x43C80000)
K16 = load(0x41800000)
K2 = load(0x40000000)
K10000 = load(0x461C4000)
R07 = load(0x3F333333)
R035 = load(0x3EB33333)
M32 = 0xFFFFFFFF
STACK = 0x7F000000
GUARD = 0x40
A_SCAN, A_PED, A_ENTS, A_PMAT, A_INTEL, A_T1, A_T2, A_ACT, A_FIRE, A_PDATA, A_PSTAT = range(11)
A_ENT0, A_EM0, A_ES0 = 11, 27, 43


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def le(words):
    out = []
    for w in words:
        out += [(w >> (8 * j)) & 0xFF for j in range(4)]
    return out


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def lt(a, b):
    return gt(b, a)


def fabs(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def timer(c, first_interval, interval_after, marks):
    """Temporizador en linea: devuelve (imagen inicial, imagen final, vencido)"""
    img = le(c['scan'])
    now = c['now'] & M32
    start = int.from_bytes(img[0:4], 'little')
    interval = int.from_bytes(img[4:8], 'little')
    started, stopped = img[8], img[9]
    if started == 0:
        marks.append('not_started')
        interval, start, started = first_interval, now, 1
    else:
        marks.append('started')
    if (c['scan'][2] & 0xFF) > 1:
        marks.append('garbage_started')
    if stopped:
        marks.append('stopped')
        if stopped > 1:
            marks.append('garbage_stopped')
        start, stopped = now, 0
    if interval & 0x80000000:
        marks.append('negative_interval')
    total = (start + interval) & M32
    if start + interval > M32:
        marks.append('wrapped')
    if now == total:
        marks.append('boundary')
    expired = now >= total
    marks.append('expired' if expired else 'not_expired')
    if expired:
        start, interval, started = now, interval_after, 1
    final = b4(start) + b4(interval) + [started, stopped] + img[10:16]
    return img, final, expired


def changes_of(img, final):
    return [[A_SCAN, i, final[i]] for i in range(16) if final[i] != img[i]]


def expected0(c):
    marks, ev = [], []
    gint = c['gint'] & M32
    if gint != 500:
        marks.append('gint_odd')
    img, final, expired = timer(c, gint, gint, marks)
    if expired:
        ev.append([1, tag(A_SCAN, 0), tag(A_PED, 0)])
        if c['allowed'] & 0xFF:
            marks += ['allowed', 'scan_called']
            ev.append([2, tag(A_SCAN, 0), tag(A_PED, 0), M32, tag(A_ENTS, 0), c['count'] & M32, STACK, STACK, 0, M32])
        else:
            marks.append('not_allowed')
    return {'changes': changes_of(img, final), 'events': ev, 'marks': marks}


def add_event(ev, etype, payload):
    ev.append([5, tag(A_INTEL, 0x68), etype, 0, 0, 1, 0xC8, 0xFFFF] + payload + [0])


def expected1(c):
    marks, ev = [], []
    img, final, expired = timer(c, 0, 100, marks)
    res = {'changes': changes_of(img, final), 'events': ev, 'marks': marks}
    if not expired:
        return res
    intel_tm = tag(A_INTEL, 4)

    def is_goto(arena, typ):
        ev.append([4, tag(arena, 0)])
        if (typ & M32) == 900:
            return True
        ev.append([4, tag(arena, 0)])
        return (typ & M32) == 926

    ev.append([1, intel_tm])
    move = 1
    if c['t1']:
        marks.append('t1')
        if is_goto(A_T1, c['t1type']):
            marks.append('t1_goto')
            move = c['t1move'] & M32
    ev.append([2, intel_tm])
    posarena = (A_PMAT, 0x30) if c['pmat'] else (A_PED, 4)
    ppos = [v & M32 for v in c['ppos']]
    ev.append([3, 0xB71F80, tag(*posarena)] + ppos + [0, 0])
    fire = bool(c['fire'])
    dist2_store = None
    dz_store = None
    if fire:
        marks.append('fire')
        f = [v & M32 for v in c['fpos']]
        dx = sub(load(f[0]), load(ppos[0]))
        dy = sub(load(f[1]), load(ppos[1]))
        dz = sub(load(f[2]), load(ppos[2]))
        dz_store = reload(bits(dz))
        yy = mul(dy, dy)
        xx = mul(dx, dx)
        s1 = add(yy, xx)
        zz = mul(dz_store, dz_store)
        dist2 = add(s1, zz)
        dist2_store = reload(bits(dist2))
        if lt(dist2, K400) and lt(fabs(dz_store), K2):
            marks.append('event1')
            add_event(ev, FIRE_NEARBY, f)
    if c['act']:
        marks.append('act')
        ev.append([4, tag(A_ACT, 0)])
        if (c['acttype'] & M32) != 514:
            ev.append([1, intel_tm])
            if c['t2'] and is_goto(A_T2, c['t2type']) and fire:
                marks.append('t2_goto')
                if gt(K16, dist2_store) and lt(fabs(dz_store), K2):
                    marks.append('event2')
                    size = load(c['fsize'] & M32)
                    r0 = R07 if lt(size, ONE) else (load(0x3F800000) if lt(size, TWO) else load(0x3FC00000))
                    radius = bits(add(r0, R035))
                    add_event(ev, WALK_INTO_FIRE, [v & M32 for v in c['fpos']] + [c['fsize'] & M32, radius, move])
        else:
            marks.append('act514')
    return res


def expected2(c):
    marks, ev = [], []
    img, final, expired = timer(c, 500, 500, marks)
    res = {'changes': changes_of(img, final), 'events': ev, 'marks': marks}
    if not expired:
        return res
    if (c['created'] & 0xFF) != 2 and (c['pflags'] & 0x100):
        marks.append('in_vehicle_exit')
        return res
    marks.append('scanning')
    ppos = [v & M32 for v in c['ppos']]
    fwd = [load(v & M32) for v in c['fwd']]
    best = None
    min_dist = load(0x501502F9)
    los_calls = 0
    count = s32(c['count'])
    for i in range(min(count, 16) if count > 0 else 0):
        if not c['epresent'][i]:
            continue
        if (c['etype'][i] & M32) != 5:
            continue
        if not s8(c['esex'][i]) > s8(c['psex']):
            continue
        if c['eflags'][i] & 0x100:
            continue
        marks.append('candidate')
        e = [v & M32 for v in c['epos'][3 * i:3 * i + 3]]
        dx = sub(load(e[0]), load(ppos[0]))
        dy = sub(load(e[1]), load(ppos[1]))
        dz = sub(load(e[2]), load(ppos[2]))
        X, Y, Z = reload(bits(dx)), reload(bits(dy)), reload(bits(dz))
        zz = mul(dz, Z)
        yy = mul(Y, Y)
        s1 = add(zz, yy)
        xx = mul(X, X)
        dist2 = add(s1, xx)
        D = reload(bits(dist2))
        if not lt(dist2, K10000):
            continue
        marks.append('near')
        if c['pdata']:
            marks.append('interesting')
            ev.append([6, 0xC0B058, 9, tag(A_ENT0 + i, 0)])
        if not lt(D, min_dist):
            continue
        dot = add(add(mul(Z, fwd[2]), mul(Y, fwd[1])), mul(X, fwd[0]))
        if not gt(dot, ZERO_F):
            continue
        marks.append('ahead')
        ev.append([7, STACK, STACK] + ppos + e + [1, 0, 0, 1, 0, 0, 0])
        bit = (c['los'] >> los_calls) & 1 if los_calls < 32 else 0
        los_calls += 1
        if bit:
            marks.append('los_clear')
            best, min_dist = i, D
    if best is not None:
        marks.append('found')
        bt = tag(A_ENT0 + best, 0)
        ev.append([8, bt, STACK])
        add_event(ev, SEXY_PED, [bt])
        final = final[:]
        final[0:4] = b4(c['now'] & M32)
        final[4:8] = b4(3000)
        final[8] = 1
        ev.append([9, bt, STACK])
        res['changes'] = changes_of(img, final)
    else:
        marks.append('none_found')
    return res


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2}[c['fn']](c)


def check_side(c, o, e):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
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
