"""Oraculo independiente (Python) de Veh81B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6E0050..0x6E03FD, lo que debe observarse de
CVehicle::UpdateTractorLink (traza de colaboradores EN ORDEN con todos sus argumentos, escrituras en las arenas y paso de tiempo final) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio guardado como float
(fstp / fst dword, con desnormales) y vuelto a cargar donde el EXE lo guarda; lo que el EXE deja en el registro (factor de masas, 1/paso, masa efectiva) no se redondea a float.
La unica carga de NaN que aparece es el indefinido 0xFFC00000 (las fixtures usan un solo patron de NaN): el oraculo exige exactamente ese patron.

Desensamblado leido (EXE, thiscall con dos bool en la pila, `ret 8`):
  6E0056  T = [this+0x4C8]; nulo -> sale.  6E0064  call [T.vtbl+0xEC](&B, 1, this) = GetTowHitchPos; AL == 0 -> sale.  6E00AC  T = [this+0x4C8] (relee);  call [this.vtbl+0xF0](&A, 1, T) = GetTowBarPos; AL == 0 -> sale.
  6E00CC  C = B - A (float). Si word [this+0x22] es 0x20D o 0x213: word [this+0x86C] > word [0x8D313C] (20000) - 100 (con signo de 32 bits) -> sale.
  6E0119  T = [this+0x4C8] (relee); B -= pos(T) (matriz [T+0x14] + 0x30, o T + 4 si es nula); A -= pos(this) (idem). 0x542CE0: velocidad de B en T (ecx = T de 0x6E0119) y de A en this.
  6E01C5  rel = vT - vV. Si arg0 == 0: T = [this+0x4C8] (relee); con arg1: rel = (0.1f * C) / max(1.0f, [0xB7CB5C]) (0x40FE90, 0x420800, 0x4119D0); sin arg1: rel *= (1 - mV / (mT + mV)) * 0.5.
  6E0293  T = [this+0x4C8] (relee); si [T+0x594] == 11 y dword [T+0x9EC] == 0xC47A0000: at = [T+0x14] + 0x20 (sin comprobar nulo): rel -= at * ((rz*az + ry*ay) + rx*ax).
  6E02E8  D = Multiply3x3([this+0x14] (relee, sin comprobar nulo), this+0xA4) (0x59C790); R = rel; Normalise(R) (0x59C910); E = A - D; F = CrossProduct(&F, &E, &R) (0x59C730; se lee por el puntero devuelto);
          k = 1 / ((((Fx*Fx + Fy*Fy) + Fz*Fz) / [this+0x90]) + 1 / [this+0x8C]) (en el registro); force = rel * k; ApplyForce(force, A, 1) (0x542B50); byte [this+0xB8] = 0.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; un puntero fuera de las arenas (pila), 0xFFFF0001.
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
M32 = 0xFFFFFFFF
NAN = ('n',)
QNAN = 0xFFC00000
ONE, HALF, TENTH = 0x3F800000, 0x3F000000, 0x3DCCCCCD   # 1.0f (0x858624), 0.5f (0x871DF8), 0.1f (0x871DFC)
STACK = 0xFFFF0001


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


# etiquetas de arena: 0 tractor, 1 remolque T, 2 remolque T2, 3 matriz A del tractor, 4 matriz B del tractor, 5 matriz A del remolque, 6 matriz B del remolque
VEH, TRA, TRB, MV, MV2, MT, MT2 = (tag(i, 0) for i in range(7))
MAT_OFF = [0x00, 0x04, 0x08, 0x10, 0x14, 0x18, 0x20, 0x24, 0x28, 0x30, 0x34, 0x38]
LUGGAGE_SUB, LUGGAGE_SENT = 11, 0xC47A0000


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
    """redondea al par a 24 bits (exponente ilimitado), o con cuanto minimo 2**quantum_exp_min (desnormales del float)"""
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
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def sub(a, b):
    return add(a, neg(b))


def div(a, b):
    """a / b"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return NAN
    if a[0] == 'i':
        return ('i', a[1] ^ (b[1] if b[0] == 'i' else b[2]))
    if b[0] == 'i':
        return ('f', Fraction(0), a[2] ^ b[1])
    if b[1] == 0:
        if a[1] == 0:
            return NAN
        return ('i', a[2] ^ b[2])
    return fin(a[1] / b[1], a[2] ^ b[2])


def greater(a, b):
    """a > b ordenado (fcomp; test ah, 41h): falso si alguno es NaN"""
    if a == NAN or b == NAN:
        return False

    def key(x):
        if x[0] == 'i':
            return Fraction(10) ** 400 * (-1 if x[1] else 1)
        return x[1]
    return key(a) > key(b)


def store(a):
    """fstp dword: devuelve los bits del float (NaN -> el indefinido)"""
    if a == NAN:
        return QNAN
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


L = load
LAST = {}   # intermedios del ultimo calculo (cobertura de run_local.py)


# ---------------------------------------------------------------- bloques de la funcion (cada uno con los redondeos del EXE)
def sub3(a, b):
    """fld a; fsub b; fstp: componente a componente (bits)"""
    return [store(sub(L(a[i]), L(b[i]))) for i in range(3)]


def scale(rel, tmass, vmass):
    """6E01F5..6E0239: rel *= (1.0 - vmass / (tmass + vmass)) * 0.5; el factor queda en el registro"""
    s = add(L(tmass), L(vmass))
    ratio = div(L(vmass), s)
    f = mul(sub(L(ONE), ratio), L(HALF))
    LAST['factor'] = f
    return [store(mul(L(rel[i]), f)) for i in range(3)]


def damp(c3, ts):
    """6E0241..6E028F: rel = (0.1f * c) / max(1.0f, ts); d = 1.0 / m se guarda como float, pero z usa el d del registro"""
    one, t = L(ONE), L(ts)
    mx = one if greater(one, t) else t
    m = store(mx)
    LAST['max_is_one'] = mx is one
    tz = store(mul(L(TENTH), L(c3[2])))
    ty = store(mul(L(TENTH), L(c3[1])))
    tx = store(mul(L(TENTH), L(c3[0])))
    d = div(one, L(m))
    df = store(d)
    rz = store(mul(d, L(tz)))
    ry = store(mul(L(df), L(ty)))
    rx = store(mul(L(df), L(tx)))
    LAST.update(dict(d=d, d_stored=df))
    return [rx, ry, rz]


def project(rel, at):
    """6E02B1..6E0309: rel -= at * float((rz*az + ry*ay) + rx*ax)"""
    dot = store(add(add(mul(L(rel[2]), L(at[2])), mul(L(rel[1]), L(at[1]))), mul(L(rel[0]), L(at[0]))))
    tz = store(mul(L(dot), L(at[2])))
    ty = store(mul(L(dot), L(at[1])))
    tx = store(mul(L(dot), L(at[0])))
    LAST['dot'] = dot
    return [store(sub(L(rel[0]), L(tx))), store(sub(L(rel[1]), L(ty))), store(sub(L(rel[2]), L(tz)))]


def mul3x3(m, v):
    """0x59C790: m es el diccionario desplazamiento -> bits"""
    x = store(add(add(mul(L(m[0x20]), L(v[2])), mul(L(m[0x10]), L(v[1]))), mul(L(m[0x00]), L(v[0]))))
    y = store(add(add(mul(L(m[0x24]), L(v[2])), mul(L(m[0x04]), L(v[0]))), mul(L(m[0x14]), L(v[1]))))
    z = store(add(add(mul(L(m[0x28]), L(v[2])), mul(L(m[0x08]), L(v[0]))), mul(L(m[0x18]), L(v[1]))))
    return [x, y, z]


def impulse(f, turn, mass, rel):
    """6E035F..6E03E5: k = 1 / (((Fx*Fx + Fy*Fy) + Fz*Fz) / turn + 1 / mass) (registro) y force = rel * k"""
    fx, fy, fz = L(f[0]), L(f[1]), L(f[2])
    s = add(add(mul(fx, fx), mul(fy, fy)), mul(fz, fz))
    q = div(s, L(turn))
    inv = div(L(ONE), L(mass))
    k = div(L(ONE), add(q, inv))
    LAST['k'] = k
    return [store(mul(L(rel[i]), k)) for i in range(3)]


# ---------------------------------------------------------------- el modelo de la funcion
def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def expected(c):
    """devuelve dict: events, ptrs, changes (7 arenas), time, reached, path"""
    LAST.clear()
    # estado mutable (solo lo que la funcion lee o un falso puede cambiar)
    st = dict(
        vtrailer=TRA if c['has_trailer'] else 0, vmat=MV if c['has_vmat'] else 0, tmat=MT if c['has_tmat'] else 0,
        vmass=c['vmass'], vturn=c['vturn'], tmass=list(c['tmass']), model=c['model'] & 0xFFFF, hoist=c['hoist'] & 0xFFFF,
        tsub=list(c['tsub']), tsent=list(c['tsent']), ts=c['ts'],
        mats={MV: list(c['vm']), MV2: list(c['vm2']), MT: list(c['tm']), MT2: list(c['tm2'])})
    # valores iniciales de los campos que un falso puede cambiar: para derivar los cambios observados de las arenas
    init = dict(st)
    init['mats'] = {k: list(v) for k, v in st['mats'].items()}
    init['tsub'], init['tsent'], init['tmass'] = list(st['tsub']), list(st['tsent']), list(st['tmass'])
    ev = []
    out = dict(events=ev, reached=0, mutated=[], path='')
    count = [0]

    def after():
        idx = count[0]
        count[0] += 1
        if idx != c['mut_at']:
            return
        k, v, v2 = c['mut_kind'], c['mut_val'], c['mut_val2']
        out['mutated'].append((idx, k))
        if k == 1:
            st['vtrailer'] = TRB
        elif k == 2:
            st['vmat'] = MV2 if v & 1 else MV
        elif k == 3:
            st['tmat'] = MT2 if v & 1 else MT
        elif k == 4:
            st['vmass'] = v2
        elif k == 5:
            st['vturn'] = v2
        elif k == 6:
            st['tmass'][0] = v2
        elif k == 7:
            st['ts'] = v2
        elif k == 8:
            st['model'] = v & 0xFFFF
        elif k == 9:
            st['hoist'] = v & 0xFFFF
        elif k == 10:
            st['tsub'][0], st['tsent'][0] = LUGGAGE_SUB, LUGGAGE_SENT
        elif k == 11:
            st['tsub'][0] = v
        elif k == 12:
            st['mats'][MV][v % 12] = v2
        elif k == 13:
            st['tmass'][1] = v2
        elif k == 14:
            st['mats'][MT][v % 12] = v2

    def pos_of(obj_ptr, is_vehicle):
        """6E012E / 6E015D: pos = matriz + 0x30 o, si es nula, la posicion de reserva +0x04"""
        if is_vehicle:
            mp, fb = st['vmat'], c['vfb']
        elif obj_ptr == TRA:
            mp, fb = st['tmat'], c['tfb0']
        else:
            mp, fb = MT2, c['tfb1']
        return list(st['mats'][mp][9:12]) if mp else list(fb)

    def tidx(ptr):
        return 0 if ptr == TRA else 1

    def finish(reached, path):
        out['reached'] = reached
        out['path'] = path
        # cambios de las arenas: campos rastreados cuyo valor final difiere del inicial
        ch = [[] for _ in range(7)]

        def words(arena, off, nbytes, old, new):
            for i in range(nbytes):
                ob, nb = (old >> (8 * i)) & 0xFF, (new >> (8 * i)) & 0xFF
                if ob != nb:
                    ch[arena].append((off + i, nb))
        words(0, 0x8C, 4, init['vmass'], st['vmass'])
        words(0, 0x90, 4, init['vturn'], st['vturn'])
        words(0, 0x22, 2, init['model'], st['model'])
        words(0, 0x86C, 2, init['hoist'], st['hoist'])
        for a, ti in ((1, 0), (2, 1)):
            words(a, 0x8C, 4, init['tmass'][ti], st['tmass'][ti])
            words(a, 0x594, 4, init['tsub'][ti], st['tsub'][ti])
            words(a, 0x9EC, 4, init['tsent'][ti], st['tsent'][ti])
        for arena, ptr in ((3, MV), (4, MV2), (5, MT), (6, MT2)):
            for i, off in enumerate(MAT_OFF):
                words(arena, off, 4, init['mats'][ptr][i], st['mats'][ptr][i])
        if reached == 4:
            ch[0].append((0xB8, 0x00))   # el banco siembra 0xA5 en +0xB8
        out['changes'] = [sorted(x) for x in ch]
        out['ptrs'] = [st['vmat'], st['vtrailer'], st['tmat'], MT2]
        out['time'] = st['ts']
        return out

    if not c['has_trailer']:
        return finish(0, 'no_trailer')
    t0 = st['vtrailer']
    zero = [0, 0, 0]
    ev.append([1, t0, STACK] + zero + [1, VEH])
    hitch = list(c['hitch'])
    after()
    if (c['ret_hitch'] & 0xFF) == 0:
        return finish(1, 'hitch_fail')
    t1 = st['vtrailer']
    ev.append([2, VEH, STACK] + zero + [1, t1])
    bar = list(c['bar'])
    after()
    if (c['ret_bar'] & 0xFF) == 0:
        return finish(2, 'bar_fail')
    cc = sub3(hitch, bar)
    if st['model'] in (0x20D, 0x213):
        out['tow_model'] = True
        if st['hoist'] > 20000 - 100:
            return finish(3, 'hoist_cut')
    t2 = st['vtrailer']
    hitch = sub3(hitch, pos_of(t2, False))
    bar = sub3(bar, pos_of(VEH, True))
    ev.append([3, t2, STACK] + hitch)
    spd_t = list(c['spd0'])
    after()
    ev.append([4, VEH, STACK] + bar)
    spd_v = list(c['spd1'])
    after()
    rel = sub3(spd_t, spd_v)
    out['arg0'] = (c['arg0'] & 0xFF) != 0
    out['arg1'] = (c['arg1'] & 0xFF) != 0
    if not out['arg0']:
        t3 = st['vtrailer']
        if out['arg1']:
            rel = damp(cc, st['ts'])
        else:
            rel = scale(rel, st['tmass'][tidx(t3)], st['vmass'])
    t4 = st['vtrailer']
    i4 = tidx(t4)
    out['luggage'] = st['tsub'][i4] == LUGGAGE_SUB and st['tsent'][i4] == LUGGAGE_SENT
    out['trailer_t4'] = i4
    if out['luggage']:
        mp = st['tmat'] if t4 == TRA else MT2
        if not mp:
            raise RuntimeError('caso invalido: matriz del remolque nula con remolque de equipaje')
        m = st['mats'][mp]
        rel = project(rel, [m[6], m[7], m[8]])
    vm = st['vmat']
    if not vm:
        raise RuntimeError('caso invalido: matriz del tractor nula')
    mm = st['mats'][vm]
    d = mul3x3({off: mm[i] for i, off in enumerate(MAT_OFF)}, c['com'])
    r = list(rel)
    ev.append([5, STACK] + r)
    r = list(c['norm'])
    after()
    e = sub3(bar, d)
    ev.append([6, STACK] + e + r + [STACK, STACK])
    f = list(c['cross'])
    after()
    force = impulse(f, st['vturn'], st['vmass'], rel)
    ev.append([7, VEH] + force + bar + [1])
    after()
    return finish(4, 'complete')


def check_side(c, o, exp):
    """compara un lado (observed) con el modelo; devuelve el motivo del primer desacuerdo o None"""
    got = o['events']
    if len(got) != len(exp['events']):
        return 'numero de eventos %d != %d' % (len(got), len(exp['events']))
    for i, (g, w) in enumerate(zip(got, exp['events'])):
        if len(g) != len(w):
            return 'evento %d longitud %s != %s' % (i, g, w)
        for k, (a, b) in enumerate(zip(g, w)):
            if a != b:
                return 'evento %d campo %d: %x != %x (%s)' % (i, k, a, b, g)
    if o['ptrs'] != exp['ptrs']:
        return 'punteros %s != %s' % (o['ptrs'], exp['ptrs'])
    for i in range(7):
        ch = [list(x) for x in o['changes'][i]]
        if ch != [list(x) for x in exp['changes'][i]]:
            return 'cambios de la arena %d: %s != %s' % (i, ch, exp['changes'][i])
    if o['time'] != exp['time']:
        return 'paso de tiempo %x != %x' % (o['time'], exp['time'])
    if o['unexpected_virtuals'] != 0:
        return 'virtual inesperado'
    return None


def check_row(row):
    c = row['input']
    exp = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], exp)
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
                print('DIFIERE', r[0], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
