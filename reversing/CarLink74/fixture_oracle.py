"""Oraculo independiente (Python) de CarLink74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x422760..0x422B18, lo que debe observarse
(argumentos de las tres llamadas a GetATanOfXY, resultado en *arg4 y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga depende de la forma de la instruccion); todo lo demas, bit a bit.
Familias con atan REAL (14 y 15): no hay traza de falso; se comprueba solo que el resultado sea el valor inicial intacto (NaN incluido) o un valor en [0, 2pi].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF

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


def lt0(a):
    """dot < 0 ordenado: falso con NaN; -0 no es menor que cero"""
    if a == NAN:
        return False
    if a[0] == 'i':
        return a[1] == 1
    return a[1] < 0


def fchs(a):
    if a == NAN:
        return a
    if a[0] == 'i':
        return ('i', a[1] ^ 1)
    return ('f', -a[1], a[2] ^ 1)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN (cualquier carga)"""
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
    """un valor guardado como float y vuelto a cargar (NaN se mantiene NaN)"""
    return NAN if bits is None else load(bits)


K1 = load(0x3C23D70A)      # 0x858C58 = 0.01f
HALF = load(0x3F000000)    # 0x858B8C
K3 = load(0x3C3DA12F)      # 0x858EEC
INSET = load(0x3E99999A)   # 0x858C24 = 0.3f
REACH = load(0x40ACCCCD)   # 0x858C50 = 5.4f
PI = load(0x40490FDB)      # 0x858CB8
TWO_PI = load(0x40C90FDB)  # 0x858CBC
NEG_PI = load(0xC0490FDB)  # 0x858CC0
ZERO = ('f', Fraction(0), 0)
LAST = {}   # intermedios del ultimo caso modelado (para la cobertura de run_local.py)


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def integer(n):
    return ('f', Fraction(n), 1 if n < 0 else 0)


def sub(a, b):
    return add(a, fchs(b))


def cmp(a, b):
    """-1, 0, 1 o None (desordenado)"""
    if a == NAN or b == NAN:
        return None

    def key(v):
        if v[0] == 'i':
            return Fraction(10) ** 400 * (-1 if v[1] else 1)
        return v[1]
    ka, kb = key(a), key(b)
    return (ka > kb) - (ka < kb)


def gt(a, b):
    return cmp(a, b) == 1


def lt(a, b):
    return cmp(a, b) == -1


def st(a):
    """guardado como float y recargado"""
    return reload(store(a))


def clip(c):
    """argumentos (x, y) en bits (None = NaN) de las tres llamadas a GetATanOfXY"""
    dirx, diry, arg3 = s8(c['dirx']), s8(c['diry']), s8(c['arg3'])
    a = st(mul(mul(integer(dirx), K1), integer(arg3)))
    b = st(mul(mul(integer(diry), K1), integer(arg3)))
    px, py = (load(c['mpos_x']), load(c['mpos_y'])) if c['has_mat'] else (load(c['ppos_x']), load(c['ppos_y']))
    arg5, arg6 = load(c['arg5']), load(c['arg6'])
    ev = [(store(sub(arg5, px)), store(sub(arg6, py)))]
    cl, al, w = c['flags'] & 7, (c['flags'] >> 3) & 7, c['width'] & 0xFF
    if cl == 0:
        m = st(mul(integer(al), HALF))
        s = m
    elif al == 0:
        m = st(mul(integer(cl), HALF))
        s = m
    elif arg3 == 0:
        m = st(add(mul(integer(w), K3), integer(cl)))
        s = integer(al)
    else:
        m = st(add(mul(integer(w), K3), integer(al)))
        s = integer(cl)
    q = sub(m, INSET)
    p = st(sub(s, INSET))
    x1 = st(add(mul(mul(b, q), REACH), arg5))
    y1 = sub(arg6, mul(mul(q, a), REACH))
    x2 = st(sub(arg5, mul(mul(p, b), REACH)))
    y2 = st(add(mul(mul(p, a), REACH), arg6))
    ev.append((store(sub(x1, px)), store(sub(y1, py))))
    ev.append((store(sub(x2, px)), store(sub(y2, py))))
    LAST.clear()
    LAST.update(q=q, p=p, a=a, b=b, y1=y1, x1=x1, x2=x2, y2=y2)
    return ev


def decide(c):
    """(argumentos de las llamadas, resultado esperado en *arg4 como bits o None si NaN)"""
    ev = clip(c)
    old = load(c['old'])
    a1 = reload(store(load(c['ret1'])))
    a2 = load(c['ret2'])
    d1m = reload(store(sub(a1, old)))
    d2 = sub(a2, old)
    d1 = d1m
    if gt(d1, PI):
        while True:
            d1 = sub(d1, TWO_PI)
            if not gt(d1, PI):
                break
        d1m = st(d1)
    if lt(d1, NEG_PI):
        while True:
            d1 = add(d1, TWO_PI)
            if not lt(d1, NEG_PI):
                break
        d1m = st(d1)
    if gt(d2, PI):
        while True:
            d2 = sub(d2, TWO_PI)
            if not gt(d2, PI):
                break
    if lt(d2, NEG_PI):
        while True:
            d2 = add(d2, TWO_PI)
            if not lt(d2, NEG_PI):
                break
    base = None
    LAST['branch'] = 'none'
    LAST['final'] = 'none'
    if lt(d1, ZERO) and lt(d2, ZERO):
        base = d1m if gt(d1, d2) else d2
        LAST['branch'] = 'neg'
    elif gt(d1, ZERO) and gt(d2, ZERO):
        base = d1m if lt(d1, d2) else d2
        LAST['branch'] = 'pos'
    LAST.update(d1=d1, d2=d2)
    if base is None:
        res = old
    else:
        res = st(add(base, old))
    if res != NAN:
        if lt(res, ZERO):
            LAST['final'] = 'add'
            while True:
                res = add(res, TWO_PI)
                if not lt(res, ZERO):
                    break
            res = st(res)
        if gt(res, TWO_PI):
            LAST['final'] = 'sub'
            while True:
                res = sub(res, TWO_PI)
                if not gt(res, TWO_PI):
                    break
            res = st(res)
    return ev, (None if res == NAN else store(res))


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def check_side(c, o):
    """cadena vacia si el lado observado coincide con el modelo"""
    got_ev, got_res, ch = o['events'], o['result'], o['changes']
    if c['null_node']:
        ev, res = [], c['old']
    elif c['real']:
        ev, res = None, None
    else:
        evs, res = decide(c)
        ev = [[1, k, x, y] for k, (x, y) in enumerate(evs)]
        if res is None:
            res = c['old']   # *arg4 NaN: nunca se escribe un NaN; queda intacto (carga original)
            if not is_nan(c['old']):
                return 'resultado NaN con *arg4 inicial finito'
    why = ''
    if ev is None:
        if got_ev != []:
            return 'la traza deberia estar vacia con atan real'
        if o.get('atan_exe_original') is not True:
            return 'el atan del EXE no es el original'
        if is_nan(c['old']):
            if got_res != c['old']:
                why = 'NaN inicial modificado'
        else:
            v = load(got_res)
            if v == NAN or v[0] == 'i' or not (0 <= v[1] <= TWO_PI[1]):
                if got_res != c['old']:
                    why = 'resultado fuera de [0, 2pi]: %x' % got_res
    else:
        if len(got_ev) != len(ev):
            return 'numero de eventos %d != %d' % (len(got_ev), len(ev))
        for g, w in zip(got_ev, ev):
            if g[0] != 1 or g[1] != w[1]:
                return 'evento %s != %s' % (g, w)
            for k in (2, 3):
                if w[k] is None:
                    if not is_nan(g[k]):
                        return 'argumento %d de la llamada %d deberia ser NaN: %x' % (k - 2, w[1], g[k])
                elif g[k] != w[k]:
                    return 'argumento %d de la llamada %d: %x != %x' % (k - 2, w[1], g[k], w[k])
        if o['atan_calls'] != len(ev):
            return 'numero de llamadas'
        if got_res != res:
            return 'resultado %x != %x' % (got_res, res)
    if not why and (ch[0] or ch[1] or ch[3]):
        why = 'cambios en vehiculo, matriz o enlace: %s' % (ch,)
    if not why:
        for off, _ in ch[2]:
            if not 0 <= off < 4:
                why = 'cambio fuera de arg4: %s' % (ch[2],)
    return why


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        for side in ('original', 'candidate'):
            why = check_side(c, row[side]['observed'])
            if why:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
