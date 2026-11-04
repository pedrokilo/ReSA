"""Oraculo independiente (Python) de Fn92C: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de la funcion del lote 92C y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Funcion 0, CAttractorScanner::ScanForAttractors (0x6060A0..0x606573; thiscall, ECX = escaner de 0x90 bytes, ped en pila, `ret 4`; void):
  Escaner: +0 bandera (byte; 0 = apagado), +4 CTaskTimer (inicio +4, intervalo +8, arrancado +0xC, detenido +0xD; los booleanos se leen como bytes), +0x10 efecto en uso, +0x14 dword, +0x18..+0x90 tres tablas de 10 dwords.
  Sale si la bandera es 0 o si el bit 8 de ped+0x46C (en vehiculo). GetActiveTask(inteligencia + 4) (0x681720); si hay tarea y su GetTaskType (virtual, una vez) es 233: inicio = hora, intervalo 3000, arrancado 1 y sale.
  Temporizador: sin arrancar = inicio hora, intervalo 1500, arrancado 1; detenido = inicio hora, detenido 0; vence si hora >= inicio + intervalo (32 bits sin signo); al vencer: inicio = hora, intervalo 1500, arrancado 1.
  Clear (0x5FFF90): +0x18 y +0x40 a cero, +0x68 a 25.0 salvo las posiciones 4 y 7 (15.0 * 15.0).
  Celdas: ftol(floor((coordenada -+ 15.0) * 0.02 + 60.0)) (x87 a 24 bits; fstp qword, floor del CRT, _ftol2: palabra baja de la conversion de 64 bits, 0 con NaN, infinito o |valor| >= 2^63);
  minimos = max(0, celda) y maximos = min(119, celda), con signo. Por cada y en [minY, maxY] y x en [minX, maxX]: 0x6034B0(escaner, &sector[y * 120 + x] (0xB7D0B8, 8 bytes), ped) y
  0x6034B0(escaner, &repeticion[((y & 15) << 4) + (x & 15)] + 8 (0xB992B8, 12 bytes), ped), con los indices acotados a [0, 119].
  Para cada i de 0 a 63 con el byte 0xC3A1A0 + i != 0: sin lista de usuarios (byte +0x20 de 0xC3A200 + 0x24 * i) -> 0x5FFFD0(escaner, 0xC3AB00 + 0x40 * i, 0, ped); con lista: si el indice de modelo del ped (word
  +0x22, con signo) esta en los cuatro dwords de la lista, igual; si no, 0x5FE960(lista, tipo de ped +0x598) (AL) decide.
  0x600180(escaner, &efecto, &entidad) con las dos variables a cero. Sin efecto sale; el mismo efecto y la misma entidad que los guardados (+0x10, +0x14) salen. Tipo (byte efecto + 0x34) 7: 0x603570(efecto) debe
  devolver el ped, y se crea CEventScriptedAttractor(efecto, entidad) (RegisterReference); si no, CEventAttractor(efecto, entidad, false). CEventGroup::Add(inteligencia + 0x68, evento, 0) (0x4AB420); si devuelve
  distinto de cero: +0x14 = entidad y +0x10 = efecto. Destructor del evento (CleanUpOldReference).

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



EVENT_ATTRACTOR, EVENT_SCRIPTED_ATTRACTOR = 27, 48
ONE = load(0x3F800000)
ZERO_F = load(0)
K15 = load(0x41700000)
K002 = load(0x3CA3D70A)
K60 = load(0x42700000)
M32 = 0xFFFFFFFF
STACK = 0x7F000000
GUARD = 0x40
A_SCAN, A_PED, A_PMAT, A_INTEL, A_ACT, A_EFF, A_ENT = range(7)
OTHER_INUSE, OTHER_F14, OTHER_ENTITY, OTHER_USER = 0x00BEEF00, 0x00FEED00, 0x00C0FFEE, 0x00ABCD00


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


def timer_image(img, now, first_interval, interval_after, flags_word, marks):
    """Temporizador en linea en +4..+0xD: modifica la imagen (lista de bytes) y devuelve si vencio"""
    start = int.from_bytes(img[4:8], 'little')
    interval = int.from_bytes(img[8:12], 'little')
    started, stopped = img[12], img[13]
    if started == 0:
        marks.append('not_started')
        interval, start, started = first_interval, now, 1
    else:
        marks.append('started')
    if (flags_word & 0xFF) > 1:
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
    img[4:8] = b4(start)
    img[8:12] = b4(interval)
    img[12] = started
    img[13] = stopped
    return expired


def ftol(v):
    """fstp qword + floor del CRT + _ftol2: palabra baja (con signo) de la conversion de 64 bits; 0 con NaN, infinito o |valor| >= 2^63"""
    if v == NAN or v[0] == 'i':
        return 0
    fl = v[1].numerator // v[1].denominator
    if abs(fl) >= (1 << 63) or (fl == -(1 << 63)):
        return 0
    return s32(fl & M32)


def cell(bits, plus):
    x = load(bits & M32)
    e = add(K15, x) if plus else sub(x, K15)
    return ftol(add(mul(e, K002), K60))


def other_value(kind, other):
    return 0 if kind == 0 else other


def expected0(c):
    marks, ev = [], []
    now = c['now'] & M32
    img = list(le([c['s0']] + [v & M32 for v in c['scan'][:3]] + [0, 0] + [v & M32 for v in c['arr']]))
    # estado inicial de los dos punteros (+0x10 efecto en uso, +0x14 dword): etiquetas
    inuse = tag(A_EFF, 0) if c['inuse'] == 1 else other_value(c['inuse'], OTHER_INUSE)
    f14 = tag(A_ENT, 0) if c['f14'] == 1 else other_value(c['f14'], OTHER_F14)
    ptrs = [inuse, f14]
    res = {'events': ev, 'marks': marks, 'ptrs': ptrs}

    def done():
        res['changes'] = [[A_SCAN, i, img[i]] for i in range(0x90) if not (0x10 <= i < 0x18) and img[i] != orig[i]]
        return res

    orig = list(img)
    if (c['s0'] & 0xFF) == 0:
        marks.append('disabled')
        return done()
    if c['pflags'] & 0x100:
        marks.append('in_vehicle')
        return done()
    intel_tm = tag(A_INTEL, 4)
    ev.append([2, intel_tm])
    if c['act']:
        ev.append([4, tag(A_ACT, 0)])
        if (c['acttype'] & M32) == 233:
            marks.append('use_effect')
            img[4:8] = b4(now)
            img[8:12] = b4(3000)
            img[12] = 1
            return done()
    if not timer_image(img, now, 1500, 1500, c['scan'][2], marks):
        return done()
    # Clear (0x5FFF90)
    marks.append('cleared')
    for i in range(10):
        img[0x18 + 4 * i:0x1C + 4 * i] = b4(0)
        img[0x40 + 4 * i:0x44 + 4 * i] = b4(0)
        img[0x68 + 4 * i:0x6C + 4 * i] = b4(0x43610000 if i in (4, 7) else 0x41C80000)
    ppos = [v & M32 for v in c['ppos']]
    cx = cell(ppos[0], False)
    mnx = cx if cx > 0 else 0
    cy = cell(ppos[1], False)
    mny = cy if cy > 0 else 0
    cx = cell(ppos[0], True)
    mxx = cx if cx < 119 else 119
    cy = cell(ppos[1], True)
    mxy = cy if cy < 119 else 119
    count = 0
    for y in range(mny, mxy + 1):
        for x in range(mnx, mxx + 1):
            yc = min(max(y, 0), 119)
            xc = min(max(x, 0), 119)
            ev.append([21, tag(A_SCAN, 0), 0xB7D0B8 + 8 * (yc * 120 + xc), tag(A_PED, 0)])
            ev.append([21, tag(A_SCAN, 0), 0xB992B8 + 12 * (((y & 15) << 4) + (x & 15)) + 8, tag(A_PED, 0)])
            count += 1
    marks.append('sectors' if count else 'no_cells')
    ul = {}
    flat = c['ul']
    for k in range(0, len(flat), 6):
        ul[flat[k]] = flat[k + 1:k + 6]
    m16 = c['pmodel'] & 0xFFFF
    model = m16 - 0x10000 if m16 & 0x8000 else m16
    ulcalls = 0
    for i in range(64):
        if not (c['actv'][i] & 0xFF):
            continue
        types = ul[i][:4]
        flag = ul[i][4] & 0xFF
        scan = True
        if flag:
            marks.append('ulist')
            if any((t & M32) == (model & M32) for t in types):
                marks.append('ul_model')
            else:
                marks.append('ul_type')
                ev.append([23, 0xC3A200 + 0x24 * i, c['ptype'] & M32])
                bit = (c['ulmask'] >> ulcalls) & 1 if ulcalls < 32 else 0
                ulcalls += 1
                scan = bool(bit)
        if scan:
            marks.append('effects_scan')
            ev.append([22, tag(A_SCAN, 0), 0xC3AB00 + 0x40 * i, 0, tag(A_PED, 0)])
    # 0x600180
    ev.append([24, tag(A_SCAN, 0), STACK, STACK, 0, 0])
    effect = tag(A_EFF, 0) if c['reseffect'] else 0
    entity = [0, tag(A_ENT, 0), OTHER_ENTITY][c['resentity']]
    if not effect:
        marks.append('no_effect')
        return done()
    if effect == inuse and entity == f14:
        marks.append('same_inuse')
        return done()
    scripted = (c['eftype'] & 0xFF) == 7
    if scripted:
        marks.append('scripted')
        ev.append([25, effect])
        user = [tag(A_PED, 0), 0, OTHER_USER][c['user']]
        if user != tag(A_PED, 0):
            marks.append('scripted_other_ped')
            return done()
    else:
        marks.append('attractor')
    if entity:
        ev.append([8, entity, STACK])
    ev.append([5, tag(A_INTEL, 0x68), EVENT_SCRIPTED_ATTRACTOR if scripted else EVENT_ATTRACTOR, 0, 0, 1, 0xC8, 0xFFFF, effect, entity, 0, 0])
    if entity:
        ev.append([9, entity, STACK])
    if c['addret'] & M32:
        marks.append('added')
        ptrs[0] = effect
        ptrs[1] = entity
    else:
        marks.append('not_added')
    return done()


def expected(c):
    return {0: expected0}[c['fn']](c)


def check_side(c, o, e):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('ptrs') != e['ptrs']:
        return 'punteros: %s != %s' % (o.get('ptrs'), e['ptrs'])
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
