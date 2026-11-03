"""Oraculo independiente (Python) de Fn88A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x515D80..0x516000, lo que debe observarse de
CCam::GetArrestCamCopSourceNormal (thiscall, ECX sin usar; target, cop, dest, source en pila, `ret 10h`; devuelve AL) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Modelo (desensamblado):
  target o cop nulos -> falso sin tocar nada. C = posicion del cop (matriz +0x30 si +0x14 es distinto de cero, si no +4; copia cruda). D = dest - C (guardado como float por componente).
  S = CrossProduct(D, (0, 0, 1)) (0x59C730); Normalise(S) (0x59C910). C.x += 3.0f * S.x y C.y += 3.0f * S.y (producto en el registro), C.z += 3.0f * S.z (producto guardado como float); todos guardados.
  Normalise(D). Si D.z < -0.7071f (0xBF350481, ordenado): D.z = -0.7071f y, si sqrt(D.y*D.y + D.x*D.x) * 1.4142271f > 0 (registro), D.x y D.y se multiplican por 1 / esa norma (guardados), y despues Normalise(D);
  si no y D.z > 0: D.z = 0 y Normalise(D); si no, sin cambios. back = 5.0f * D (x e y en el registro, z guardado como float). C.x = C.x - back.x (guardado), Cy = C.y - back.y y Cz = C.z - back.z (registro).
  D = dest - C'' (cada componente guardado como float). len = sqrt((D.z*D.z + D.y*D.y) + D.x*D.x) (registro); L = len guardado como float. Si len < 8.0f (registro, ordenado) y L > 0: s = 8.0f / L; D.x *= s; D.y *= s (guardados); Zs = s * D.z (registro);
  si no Zs = D.z. source = (dest.x - D.x, dest.y - D.y, dest.z - Zs), cada componente guardado como float. Devuelve verdadero.
  Las mutaciones (una vez) escriben, dentro de un falso, en dest (que el EXE relee) o en el ped (que el EXE copia al empezar y no relee).

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






def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero en esta funcion: norma > 0 o L > 0)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


# ---------------------------------------------------------------- modelo de CCam::GetArrestCamCopSourceNormal
ARENA_CAM, ARENA_ENT, ARENA_PED, ARENA_MAT, ARENA_DEST, ARENA_SRC = 0, 1, 2, 3, 4, 5
STACK = 0x7F000000
DEST_OFF, SRC_OFF = 0x08, 0x10

THREE = load(0x40400000)            # 0x8CC8CC
FIVE = load(0x40A00000)             # 0x8CC8C8
EIGHT = load(0x41000000)            # 0x8CC8D4
STEEP_BITS = 0xBF350481             # 0x8631E8
STEEP = load(STEEP_BITS)
STEEP_SCALE = load(0x3FB50565)      # 0x8631E4
ONE = load(0x3F800000)              # 0x858624
ZERO_F = load(0)                    # 0x858B50
NAN_BITS = 0xFFC00000
UP_Z = 0x3F800000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    dst = [v & M32 for v in c['dest']]
    init_dest = list(dst)
    src_init = [v & M32 for v in c['srcinit']]
    mats = bool(c['cmat'])
    site = c['mutsite']
    mv = c['mutval'] & M32
    done = set()
    cop_pos = [v & M32 for v in c['cpos']]
    cop_mut = [None]            # valor escrito por la mutacion 5 en la ubicacion activa (primer dword)
    out = [None]

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def finish(ret):
        changes = []
        # la mutacion 5 sobrescribe el primer dword de la posicion activa (matriz +0x30, o compacta +4); el puntero de matriz (+0x14) va aparte (mats)
        if cop_mut[0] is not None:
            arena, off = (ARENA_MAT, 0x30) if c['cmat'] else (ARENA_PED, 4)
            old = b4(cop_pos[0])
            new = b4(cop_mut[0])
            changes += [[arena, off + j, new[j]] for j in range(4) if new[j] != old[j]]
        for k in range(3):
            if dst[k] != init_dest[k]:
                old, new = b4(init_dest[k]), b4(dst[k])
                changes += [[ARENA_DEST, DEST_OFF + 4 * k + j, new[j]] for j in range(4) if new[j] != old[j]]
        if out[0] is not None:
            for k in range(3):
                old, new = b4(src_init[k]), b4(out[0][k])
                changes += [[ARENA_SRC, SRC_OFF + 4 * k + j, new[j]] for j in range(4) if new[j] != old[j]]
        changes.sort(key=lambda e: (e[0], e[1]))
        marks.append('ret_%d' % (1 if ret else 0))
        return {'ret': 1 if ret else 0, 'mats': [mats], 'changes': changes, 'events': ev, 'marks': marks}

    if c['tnull']:
        mark('null_target')
        return finish(False)
    if c['cnull']:
        mark('null_cop')
        return finish(False)
    mark('tmat' if mats else 'tnomat')
    C = list(cop_pos)
    D = [bits(sub(load(dst[k]), load(C[k]))) for k in range(3)]
    # CrossProduct(S, D, up)
    ev.append([1, STACK, STACK, STACK] + D + [0, 0, UP_Z])
    if once(4):
        mats = not mats
    S = [v & M32 for v in c['cross']]
    # Normalise(S)
    ev.append([2, STACK] + S)
    if once(1):
        dst[0] = mv
    S = [v & M32 for v in c['ns']]
    sx = mul(load(S[0]), THREE)
    sy = mul(load(S[1]), THREE)
    sz = reload(store(mul(load(S[2]), THREE)))
    C = [bits(add(sx, load(C[0]))), bits(add(sy, load(C[1]))), bits(add(sz, load(C[2])))]
    # Normalise(D)
    ev.append([2, STACK] + D)
    if once(2):
        dst[1] = mv
    if once(3):
        dst[2] = mv
    if once(5):
        cop_mut[0] = mv
    D = [v & M32 for v in c['nd1']]
    dz = load(D[2])
    if dz == NAN:
        mark('z_nan')
    if lt(dz, STEEP):
        mark('a_branch')
        D[2] = STEEP_BITS
        reach = mul(sqrt24(add(mul(load(D[1]), load(D[1])), mul(load(D[0]), load(D[0])))), STEEP_SCALE)
        if gt(reach, ZERO_F):
            mark('a_scale')
            inv = div(ONE, reach)
            D[0] = bits(mul(load(D[0]), inv))
            D[1] = bits(mul(inv, load(D[1])))
        else:
            mark('a_noscale')
        ev.append([2, STACK] + D)
        D = [v & M32 for v in c['nd2']]
    elif gt(dz, ZERO_F):
        mark('b_flat')
        D[2] = 0
        ev.append([2, STACK] + D)
        D = [v & M32 for v in c['nd2']]
    else:
        mark('b_none')
    bx = mul(FIVE, load(D[0]))
    by = mul(FIVE, load(D[1]))
    bz = reload(store(mul(FIVE, load(D[2]))))
    pull_x = reload(bits(sub(load(C[0]), bx)))
    pull_y = sub(load(C[1]), by)
    pull_z = sub(load(C[2]), bz)
    G = [bits(sub(load(dst[0]), pull_x)), bits(sub(load(dst[1]), pull_y)), bits(sub(load(dst[2]), pull_z))]
    D = list(G)
    zz = mul(load(G[2]), load(G[2]))
    yy = mul(load(G[1]), load(G[1]))
    xx = mul(load(G[0]), load(G[0]))
    length = sqrt24(add(add(zz, yy), xx))
    if length == NAN:
        mark('len_nan')
    stored = reload(store(length))
    zs = load(D[2])
    if lt(length, EIGHT):
        mark('len_lt8')
        if gt(stored, ZERO_F):
            mark('scaled')
            scale = div(EIGHT, stored)
            D[0] = bits(mul(load(G[0]), scale))
            D[1] = bits(mul(load(D[1]), scale))
            zs = mul(scale, load(D[2]))
        else:
            mark('len_zero')
    else:
        mark('len_ge8')
    out[0] = [bits(sub(load(dst[0]), load(D[0]))), bits(sub(load(dst[1]), load(D[1]))), bits(sub(load(dst[2]), zs))]
    return finish(True)


CHANGE_KEYS = ('ret', 'mats', 'changes')


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
