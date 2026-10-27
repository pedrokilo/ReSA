"""Oraculo independiente (Python) de HeatFire73: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6E05C0..0x6E07CE, lo que debe observarse
(traza de colaboradores, posicion del misil y bytes cambiados de las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). Los NaN solo se comparan como «es NaN» (la carga depende de la forma de la instruccion); todo lo demas, bit a bit.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 vehiculo, 1 objetivo, 2 otra entidad, 3 matriz, 4 ped.
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
M32 = 0xFFFFFFFF
PAD_STRIDE = 0x134


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


VEH, TGT, OTH, MAT, PED = (tag(i, 0) for i in range(5))

# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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


LAST = {}   # intermedios del ultimo spawn() (para la cobertura de run_local.py)


def spawn(c):
    """posicion de salida del misil (bits, o None para NaN) segun 0x6E062D..0x6E0745"""
    ordx = load(c['ord_x'])
    if ((c['cycle'] >> 2) & 3) != 0:
        ordx = fchs(ordx)
    ms = [load(c['ms_x']), load(c['ms_y']), load(c['ms_z'])]
    fw = [load(c['fwd_x']), load(c['fwd_y']), load(c['fwd_z'])]
    step = load(c['step'])
    dot = add(add(mul(ms[2], fw[2]), mul(ms[1], fw[1])), mul(ms[0], fw[0]))
    fs = ('f', Fraction(0), 0) if lt0(dot) else dot
    p = mul(fs, fw[0])
    ty = reload(store(mul(fs, fw[1])))
    tz = reload(store(mul(fs, fw[2])))
    a = mul(p, step)
    b = mul(ty, step)
    cz = reload(store(mul(tz, step)))
    x = reload(store(add(a, ordx)))
    y = add(load(c['ord_y']), b)
    z = add(load(c['ord_z']), cz)
    pos = [load(c['mpos_x']), load(c['mpos_y']), load(c['mpos_z'])]
    LAST.clear()
    LAST.update(dot=dot, fs=fs, ty=ty, tz=tz, a=a, b=b, cz=cz, x=x, y=y, z=z)
    return [store(add(pos[0], x)), store(add(pos[1], y)), store(add(pos[2], z))]


def is_tiny(v):
    """magnitud distinta de cero por debajo del menor normal de un float (2**-126): desnormal al guardarse como float, o un valor de exponente extendido"""
    return v != NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < Fraction(2) ** -126


def is_signed_zero(v, neg):
    return v != NAN and v[0] == 'f' and v[1] == 0 and v[2] == neg


def expected(c):
    """(eventos, cambios del vehiculo [(offset, byte)], posicion esperada o None si no se dispara)"""
    ev = []
    veh_changes = []
    fired = False
    limit = (c['rate'] + (c['t1'] if c['type'] == 1 else c['t2'])) & M32
    cut = False
    if c['arg2'] & 0xFF:
        ev.append([1, VEH, c['type']])
        cut = c['time'] <= limit
    pos = None
    if not cut:
        ev.append([2, VEH, c['type']])
        pos = spawn(c)
        victim = {0: 0, 1: TGT, 2: OTH, 3: c['ent_raw']}[c['tgt_kind']]
        # la x de la posicion de salida y las otras dos se comparan despues (NaN = cualquier carga)
        ev.append(('proj', victim))
        if c['driver'] and c['ped_type'] in (0, 1):
            ev.append([4, c['ped_type'] * PAD_STRIDE, 240, 160, 0])
        old = c['cycle']
        new = (old & ~0x0C & 0xFF) | (0x04 if ((old >> 2) & 3) == 0 else 0)
        if new != old:
            veh_changes.append((0x4A9, new))
        fired = True
        base = 0x4FC if c['type'] == 1 else 0x500
        oldt = c['t1'] if c['type'] == 1 else c['t2']
        for i in range(4):
            a, b = (oldt >> (8 * i)) & 0xFF, (c['time'] >> (8 * i)) & 0xFF
            if a != b:
                veh_changes.append((base + i, b))
    return ev, sorted(veh_changes), pos, fired


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def main(path):
    bad = checked = 0
    nan_outs = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = row['input']
        ev, vch, pos, fired = expected(c)
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            got = o['events']
            ok = True
            why = ''
            if len(got) != len(ev):
                ok = False
                why = 'numero de eventos'
            else:
                for g, w in zip(got, ev):
                    if isinstance(w, tuple):
                        # AddProjectile(creador, arma 20, x, y, z, fuerza 0, direccion, victima, direccion[0..2])
                        want_head = [3, VEH, 20]
                        if g[0] != 3 or g[1:3] != want_head[1:3] or g[6] != 0 or g[7] != 0xF00D0001 or g[8] != w[1] or g[9:12] != [c['fwd_x'], c['fwd_y'], c['fwd_z']]:
                            ok = False
                            why = 'AddProjectile cabecera ' + str(g)
                        for k in range(3):
                            if pos[k] is None:
                                if not is_nan(g[3 + k]):
                                    ok = False
                                    why = 'componente %d deberia ser NaN: %x' % (k, g[3 + k])
                            elif g[3 + k] != pos[k]:
                                ok = False
                                why = 'componente %d: %x != %x' % (k, g[3 + k], pos[k])
                    elif g != w:
                        ok = False
                        why = 'evento %s != %s' % (g, w)
            if fired and pos is not None:
                nan_outs += sum(p is None for p in pos)
            changes = o['changes']
            want_ch = [[[off, b] for off, b in vch], [], [], [], []]
            if changes != want_ch:
                ok = False
                why = why or 'cambios %s != %s' % (changes, want_ch)
            if o['cycle'] != (((c['cycle'] >> 2) & 3) if not fired else (1 if ((c['cycle'] >> 2) & 3) == 0 else 0)):
                ok = False
                why = why or 'indice de ordenanza'
            if not ok:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad, 'componentes NaN esperados', nan_outs)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
