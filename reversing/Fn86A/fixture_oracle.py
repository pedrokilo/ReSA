"""Oraculo independiente (Python) de Fn86A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x512EF0..0x513212, lo que debe observarse de
CCam::ProcessArrestCamCheese (thiscall, ret 8: from, initialise; devuelve AL) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Modelo (desensamblado):
  Con `initialise` (byte bajo del argumento de pila): rand() del EXE (0x821B1E) * 0x858C7C (float): mayor que 0x858F50 (0.65f) devuelve falso. El objetivo de la camara (0xB6F980, releido) debe ser un ped
  (byte +0x36 & 7 == 3) y `from` no nulo, si no falso. t = ((fild(tiempo 0xB7CB84) [+ 2^32 si es negativo]) - 0xB6EC60) - 0xB70024) / 1000.0f (registro de 24 bits; el tiempo entra como entero con signo),
  guardado como float; t > 1.0 (registro) deja t = 1.0f y conmuta 0xB70020; t < 0 (float guardado) deja t = 0.0f y pone 0xB70020 a 1; t en [0, 1) o NaN pone 1; t == 1.0f conmuta (0 -> 1, otro -> 0).
  0xB7001C: bit 0 clara -> se pone y 0xB70010..18 = 0; bit 1 clara -> se pone y 0xB70004..0C = (0, 0, -0.5f). source = posicion del objetivo (matriz +0x30 si +0x14 es distinto de cero, si no +4); FOV = 100.0f;
  Pz = ((-0.06f + z del hueso 5) - 0.5f * t) guardado como float; x e y del hueso se copian en crudo; source += (0xB70004..0C) (cada suma guardada como float); front = P - source (guardado como float);
  Normalise(front) (0x59C910), up = (0, 0, 1), CrossProduct(tmp, front, up) (0x59C730), Normalise(tmp), CrossProduct(out, tmp, front) y up = *out. Con `initialise`: 0x56A490(source, P, 1, 1, 0, 1, 0, 0, 1) falso -> falso;
  0x56A490(source, P, 1, 0, 0, 1, 0, 0, 0) falso -> source vuelve al valor anterior. Al final el objetivo (releido) pierde el bit 0x80 de +0x1C y devuelve verdadero.
  Las mutaciones (una vez) cambian la memoria que el EXE relee tras la llamada de un colaborador.

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
    """a / b a 24 bits con exponente ampliado (b es siempre finito y no nulo en esta funcion: 1000.0f)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return a
    return fin(a[1] / b[1], a[2] ^ b[2])


# ---------------------------------------------------------------- modelo de CCam::ProcessArrestCamCheese
ARENA_CAM, ARENA_ENT0, ARENA_ENT1, ARENA_PED, ARENA_MAT0, ARENA_MAT1, ARENA_BONE = 0, 1, 2, 3, 4, 5, 6
STACK = 0x7F000000
CAM_FRONT, CAM_SOURCE, CAM_UP, CAM_FOV = 0x190, 0x19C, 0x1B4, 0xB4
STATIC_WORDS = 12          # 0xB70000..0xB7002F


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 86008) & M32)


RANDOM_SCALE = load(0x38000100)     # 0x858C7C
MAX_CHANCE = load(0x3F266666)       # 0x858F50
TWO32 = load(0x4F800000)            # 0x858C54
THOUSAND = load(0x447A0000)         # 0x8CCCB0
ONE = load(0x3F800000)              # 0x858624
ZERO_F = load(0)                    # 0x858B50
RAISE = load(0xBD75C28F)            # 0x8CCCAC (-0.06f)
HALF = load(0x3F000000)             # 0x8CCCA8
FOV = 0x42C80000
NAN_BITS = 0xFFC00000


class Mem:
    """Memoria modelada: (arena, desplazamiento) -> byte. Solo los campos que lee o escribe la funcion o las mutaciones."""

    def __init__(self):
        self.cur = {}

    def put32(self, arena, off, value):
        for j in range(4):
            self.cur[(arena, off + j)] = (value >> (8 * j)) & 0xFF

    def put8(self, arena, off, value):
        self.cur[(arena, off)] = value & 0xFF

    def get32(self, arena, off):
        return sum(self.cur[(arena, off + j)] << (8 * j) for j in range(4))

    def put3(self, arena, off, vals):
        for k in range(3):
            self.put32(arena, off + 4 * k, vals[k] & M32)

    def get3(self, arena, off):
        return [self.get32(arena, off + 4 * k) for k in range(3)]


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    mem = Mem()
    seed = c['seed'] & M32
    # --- estado inicial
    mem.put3(ARENA_CAM, CAM_SOURCE, c['csrc'])
    mem.put3(ARENA_CAM, CAM_FRONT, c['cfront'])
    mem.put3(ARENA_CAM, CAM_UP, c['cup'])
    mem.put32(ARENA_CAM, CAM_FOV, c['cfov'] & M32)
    mats = [bool(c['emat'][0]), bool(c['emat'][1])]
    for e in range(2):
        a, am = ARENA_ENT0 + e, ARENA_MAT0 + e
        pos = c['epos'][e * 3:e * 3 + 3]
        alt = c['ealt'][e * 3:e * 3 + 3]
        mem.put32(a, 0x1C, c['eflags'][e] & M32)
        mem.put8(a, 0x36, c['etype'][e])
        mem.put3(am if mats[e] else a, 0x30 if mats[e] else 4, pos)
        mem.put3(a if mats[e] else am, 4 if mats[e] else 0x30, alt)
    mem.put32(ARENA_PED, 0x18, c['clump'] & M32)
    idx = c['idx'] & M32
    mem.put3(ARENA_BONE, idx * 0x40 + 0x30, c['bone'])
    # globales: 12 dwords de 0xB70000 (relleno por semilla) con los campos del caso, 0xB6EC60, objetivo 0xB6F980 y tiempo 0xB7CB84
    gl = [rand(seed, 500 + i) for i in range(STATIC_WORDS)]
    for k in range(3):
        gl[1 + k] = c['voff'][k] & M32
        gl[4 + k] = c['vzero'][k] & M32
    gl[7] = c['st'] & M32
    gl[8] = (gl[8] & 0xFFFFFF00) | (c['b20'] & 0xFF)
    gl[9] = c['f2'] & M32
    init = dict(mem.cur)
    st = {'f1': c['f1'] & M32, 'ms': c['ms'] & M32, 'tgt': 0}
    site = c['mutsite']
    mv = c['mutval'] & M32
    done = set()

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def finish(ret):
        changes = sorted([[a, o, mem.cur[(a, o)]] for (a, o) in mem.cur
                          if mem.cur[(a, o)] != init[(a, o)] and not (a in (ARENA_ENT0, ARENA_ENT1) and 0x14 <= o < 0x18)])
        marks.append('ret_%d' % (1 if ret else 0))
        return {'ret': 1 if ret else 0,
                'globals': gl + [st['f1'], tag(ARENA_ENT0 + st['tgt'], 0), st['ms']],
                'mats': list(mats), 'changes': changes, 'events': ev, 'marks': marks}

    if (c['init'] & 0xFF) != 0:
        mark('init')
        ev.append([1])
        if once(1):
            st['tgt'] = 1
        if once(2):
            st['ms'] = mv
        if once(3):
            st['f1'] = mv
        if once(4):
            gl[9] = mv
        if once(5):
            mem.put32(ARENA_PED, 0x18, mv)
        if once(6):
            gl[7] = mv
        if once(7):
            gl[1] = mv
        if once(8):
            mats[st['tgt']] = not mats[st['tgt']]
        if once(9):
            mem.put8(ARENA_ENT0 + st['tgt'], 0x36, mv)
        chance = mul(fromint(c['rnd'] & M32), RANDOM_SCALE)
        if gt(chance, MAX_CHANCE):
            mark('rand_fail')
            return finish(False)
        mark('rand_ok')
    else:
        mark('noinit')
    tgt = st['tgt']
    if (mem.cur[(ARENA_ENT0 + tgt, 0x36)] & 7) != 3:
        mark('not_ped')
        return finish(False)
    if c['fromnull']:
        mark('from_null')
        return finish(False)
    # --- tiempo
    ms = st['ms']
    signed = ms - (1 << 32) if ms & 0x80000000 else ms
    t = fromint(signed)
    if signed < 0:
        mark('ms_neg')
        t = add(t, TWO32)
    t = sub(t, reload(st['f1']))
    t = sub(t, reload(gl[9]))
    d = div(t, THOUSAND)
    tfb = bits(d)
    tfv = reload(store(d))
    if gt(d, ONE):
        mark('t_gt1')
        tfb = 0x3F800000
        toggle = True
    elif lt(tfv, ZERO_F):
        mark('t_neg')
        tfb = 0
        toggle = False
    elif tfv == NAN:
        mark('t_nan')
        toggle = False
    elif lt(tfv, ONE):
        mark('t_mid')
        toggle = False
    else:
        mark('t_one')
        toggle = True
    b20 = gl[8] & 0xFF
    if toggle:
        new = 1 if b20 == 0 else 0
        mark('blink_on' if new else 'blink_off')
    else:
        new = 1
        mark('blink_set')
    gl[8] = (gl[8] & 0xFFFFFF00) | new
    if not gl[7] & 1:
        mark('st0_clr')
        gl[7] |= 1
        gl[4] = gl[5] = gl[6] = 0
    else:
        mark('st0_set')
    if not gl[7] & 2:
        mark('st1_clr')
        gl[7] |= 2
        gl[1] = gl[2] = 0
        gl[3] = 0xBF000000
    else:
        mark('st1_set')
    # --- posicion del objetivo, FOV y punto mirado
    am, a = ARENA_MAT0 + tgt, ARENA_ENT0 + tgt
    mark('tmat' if mats[tgt] else 'tnomat')
    prev = mem.get3(ARENA_CAM, CAM_SOURCE)
    pos = mem.get3(am, 0x30) if mats[tgt] else mem.get3(a, 4)
    mem.put3(ARENA_CAM, CAM_SOURCE, pos)
    mem.put32(ARENA_CAM, CAM_FOV, FOV)
    hier = c['hier'] & M32
    ev.append([2, mem.get32(ARENA_PED, 0x18)])
    ev.append([3, hier, 5])
    ev.append([4, hier])
    if once(10):
        mem.put32(ARENA_BONE, idx * 0x40 + 0x38, mv)
    mark('idx0' if idx == 0 else 'idx_nz')
    bx, by, bz = mem.get3(ARENA_BONE, idx * 0x40 + 0x30)
    pz = bits(sub(add(RAISE, reload(bz)), mul(HALF, reload(tfb))))
    src = mem.get3(ARENA_CAM, CAM_SOURCE)
    src = [bits(add(reload(gl[1 + k]), reload(src[k]))) for k in range(3)]
    mem.put3(ARENA_CAM, CAM_SOURCE, src)
    front = [bits(sub(reload(bx), reload(src[0]))), bits(sub(reload(by), reload(src[1]))), bits(sub(reload(pz), reload(src[2])))]
    mem.put3(ARENA_CAM, CAM_FRONT, front)
    # --- ejes: Normalise(front); up = (0, 0, 1); CrossProduct; Normalise; CrossProduct
    ev.append([5, tag(ARENA_CAM, CAM_FRONT)] + front)
    mem.put3(ARENA_CAM, CAM_FRONT, c['n1'])
    mem.put3(ARENA_CAM, CAM_UP, [0, 0, 0x3F800000])
    ev.append([6, STACK, tag(ARENA_CAM, CAM_FRONT), tag(ARENA_CAM, CAM_UP)] + [v & M32 for v in c['n1']] + [0, 0, 0x3F800000])
    side = [v & M32 for v in c['c1']]
    ev.append([5, STACK] + side)
    side = [v & M32 for v in c['n2']]
    ev.append([6, STACK, STACK, tag(ARENA_CAM, CAM_FRONT)] + side + [v & M32 for v in c['n1']])
    mem.put3(ARENA_CAM, CAM_UP, c['c2'])
    # --- lineas de vista
    pt = [bx, by, pz]
    if c['init'] & 0xFF:
        ev.append([7, tag(ARENA_CAM, CAM_SOURCE)] + src + [STACK] + pt + [1, 1, 0, 1, 0, 0, 1])
        if once(11):
            st['tgt'] = 1
        if (c['los1'] & 0xFF) == 0:
            mark('los1_fail')
            return finish(False)
        mark('los1_ok')
        ev.append([7, tag(ARENA_CAM, CAM_SOURCE)] + src + [STACK] + pt + [1, 0, 0, 1, 0, 0, 0])
        if (c['los2'] & 0xFF) == 0:
            mark('los2_blocked')
            mem.put3(ARENA_CAM, CAM_SOURCE, prev)
        else:
            mark('los2_ok')
    tgt = st['tgt']
    cur = mem.get32(ARENA_ENT0 + tgt, 0x1C)
    mem.put32(ARENA_ENT0 + tgt, 0x1C, cur & 0xFFFFFF7F)
    return finish(True)


CHANGE_KEYS = ('ret', 'globals', 'mats', 'changes')


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
