"""Oraculo independiente (Python) de Fn89A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x51D770..0x51E4E8 (tabla de saltos en 0x51E4E8), lo que debe observarse de
CCamera::IsItTimeForNewCamera (thiscall, ECX = la camara; camSequence y startTime en pila, `ret 8`; devuelve AL) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Modelo (desensamblado):
  camSequence < 0 (con signo): true. Transcurrido E = fild(reloj con signo) (+ 2^32 en 24 bits si es negativo) - fisub(startTime) en 24 bits; E > 20000 (fcomp): true. Palo derecho del pad 0 (word +4 del estado nuevo,
  fild): |palo| <= 32 arma el aviso (0x8CCDF4 = 1); |palo| > 96 con el aviso armado fija el paso (0x8CC471 = 1 si palo > 0, si no 0xFF), lo desarma y devuelve true. camSequence > 29 (sin signo): false.
  Despacho por la tabla: 0 (vehiculo, rhino, linea de vision hasta la camara activa, tiempo 5000), 1 2 3 5 8 15..19 (transcurrido en float > 15000: true, y el resto segun el desensamblado), 4 6 29 (solo tiempo),
  7 (2000 y GetIsOnScreen), 9..14 (false), 20..23 y 26..28 (Process_DW_* con 1: el resultado negado), 24 y 25 (0xB6F059 = 0, true).
  Los productos escalares con la velocidad del jugador suman en el orden de cada caso (A: (y*sy + sz*0) + x*sx; B: (y*sy + x*sx) + sz*0); la z del desplazamiento solo cuenta en el modulo del caso 18.
  Las mutaciones (una vez) escriben, dentro de un falso, en el origen fijo (LOS), las coordenadas siguientes del jugador (primera FindPlayerCoors), la velocidad (LOS) o el tipo del vehiculo (primera FindPlayerVehicle).

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



# ---------------------------------------------------------------- modelo de CCamera::IsItTimeForNewCamera (EXE 0x51D770..0x51E4E8, tabla de saltos 0x51E4E8)
import struct

STACK = 0x7F000000
ARENA_CAM, ARENA_ENT, ARENA_VEH, ARENA_MAT, ARENA_SPD = 0, 1, 2, 3, 4
SPD_OFF = 0x10
NAN_BITS = 0xFFC00000


def fbits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fc(x):
    return load(fbits(x))


ZERO_F = load(0)
BIG2_32 = load(0x4F800000)          # 0x858C54: 4294967296.0f
LIM_20000 = fc(20000.0)             # 0x8CCDF8
LIM_15000 = fc(15000.0)             # 0x8CCDF0
STICK_LO = fc(32.0)                 # 0x85950C
STICK_HI = fc(96.0)                 # 0x863218
HALF = fc(0.5)                      # 0x858B8C
FLOOR_005 = fc(0.05000000074505806)  # 0x858C28 (0.05f)
NEAR_CLIP_BITS = 0x3E19999A         # push 3E19999Ah
CASE8_BITS = 0x3F19999A             # mov [esi+138h], 3F19999Ah
RWCAM_NORMAL = 0xCAFE0001           # el banco sustituye el puntero real RwCamera* (0xC1703C) por esta marca


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]


def bits(a):
    b = store(a)
    return NAN_BITS if b is None else b


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def abs_(a):
    return neg(a) if lt(a, ZERO_F) else a


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    seq = c['seq'] & M32
    start = c['start'] & M32
    now = c['now'] & M32
    site = c['mutsite']
    mv = c['mutval'] & M32
    done = set()
    st = {'alert': c['alert'] & 0xFF, 'step': c['step'] & 0xFF, 'gate': c['gate'] & 0xFF,
          'w138': None, 'w31': None, 'fixed': [v & M32 for v in c['fixed']], 'speed': [v & M32 for v in c['speed']],
          'vtype': c['vehtype'] & M32, 'coorx': None, 'fpv': 0, 'fpc': 0}
    hasveh = bool(c['hasveh'])
    active = c['active']
    tgt_model = c['tgtmodel'] & 0xFFFF
    veh_model = c['vehmodel'] & 0xFFFF
    pco = [v & M32 for v in c['pcoords']]

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    def finish(ret):
        changes = []
        if st['w138'] is not None:
            old, new = b4(c['i138'] & M32), b4(st['w138'])
            changes += [[ARENA_CAM, 0x138 + j, new[j]] for j in range(4) if new[j] != old[j]]
        if st['w31'] is not None:
            if st['w31'] != (c['i31'] & 0xFF):
                changes.append([ARENA_CAM, 0x31, st['w31']])
        if st['fixed'][0] != (c['fixed'][0] & M32):
            old, new = b4(c['fixed'][0] & M32), b4(st['fixed'][0])
            changes += [[ARENA_CAM, 0x86C + j, new[j]] for j in range(4) if new[j] != old[j]]
        if st['speed'][1] != (c['speed'][1] & M32):
            old, new = b4(c['speed'][1] & M32), b4(st['speed'][1])
            changes += [[ARENA_SPD, SPD_OFF + 4 + j, new[j]] for j in range(4) if new[j] != old[j]]
        if st['vtype'] != (c['vehtype'] & M32):
            old, new = b4(c['vehtype'] & M32), b4(st['vtype'])
            changes += [[ARENA_VEH, 0x590 + j, new[j]] for j in range(4) if new[j] != old[j]]
        changes.sort(key=lambda e: (e[0], e[1]))
        marks.append('s%d_%d' % (min(seq, 31), 1 if ret else 0))
        marks.append('ret_%d' % (1 if ret else 0))
        return {'ret': 1 if ret else 0, 'changes': changes, 'events': ev, 'globals': [st['alert'], st['step'], st['gate']], 'marks': marks}

    # ---- colaboradores
    def fpv():
        ev.append([1, M32, 0])
        if st['fpv'] == 0 and once(4):
            st['vtype'] = mv % 9
        st['fpv'] += 1
        return hasveh

    def coors():
        ev.append([2, STACK, M32])
        x = pco[0] if st['coorx'] is None else st['coorx']
        v = [x, pco[1], pco[2]]
        if st['fpc'] == 0 and once(2):
            st['coorx'] = mv
        st['fpc'] += 1
        return v

    def los(origin_tag, origin, target_tag, target):
        ev.append([4, origin_tag, target_tag] + list(origin) + list(target) + [1, 0, 0, 0, 0, 0, 0])
        if once(1):
            st['fixed'][0] = mv
        if once(3):
            st['speed'][1] = mv
        return bool(c['los'])

    def near_clip():
        ev.append([5, RWCAM_NORMAL, NEAR_CLIP_BITS])

    def boat_true():
        """0x51D90E.. (casos 1, 2, 5, 8): el jugador va en un barco y el objetivo no es el modelo 0x1CC"""
        if fpv():
            fpv()
            if st['vtype'] == 5 and tgt_model != 0x1CC:
                mark('boat_true')
                return True
        return False

    def tpos_ref():
        if c['tmat']:
            return tag(ARENA_MAT, 0x30), [v & M32 for v in c['tpos']]
        return tag(ARENA_ENT, 4), [v & M32 for v in c['tpos']]

    def los_player():
        o = coors()
        return los(STACK, o, tag(ARENA_CAM, 0x86C), st['fixed'])

    def offset(flat):
        o = coors()
        v = [bits(sub(load(o[k]), load(st['fixed'][k]))) for k in range(3)]
        if flat:
            v[2] = 0
        return v

    def mag(v):
        x2 = mul(load(v[0]), load(v[0]))
        y2 = mul(load(v[1]), load(v[1]))
        z2 = mul(load(v[2]), load(v[2]))
        return sqrt24(add(add(x2, y2), z2))

    def speed():
        ev.append([3, M32])
        return st['speed']

    def dot_a(v):
        sp = speed()
        return add(add(mul(load(v[1]), load(sp[1])), mul(load(sp[2]), ZERO_F)), mul(load(v[0]), load(sp[0])))

    def dot_b(v):
        sp = speed()
        return add(add(mul(load(v[1]), load(sp[1])), mul(load(v[0]), load(sp[0]))), mul(load(sp[2]), ZERO_F))

    if s32(seq) < 0:
        mark('neg')
        return finish(True)
    e1 = fromint(s32(now))
    if s32(now) < 0:
        mark('now_neg')
        e1 = add(e1, BIG2_32)
    elapsed = sub(e1, fromint(s32(start)))
    elapsed_f = reload(store(elapsed))
    if gt(elapsed, LIM_20000):
        mark('e20000')
        return finish(True)
    stick = s16(c['stick'])
    sv = fromint(stick)
    if not gt(abs_(sv), STICK_LO):
        mark('stick_arm')
        st['alert'] = 1
    elif gt(abs_(sv), STICK_HI) and st['alert'] != 0:
        st['step'] = 1 if stick > 0 else 0xFF
        st['alert'] = 0
        mark('stick_fire_pos' if stick > 0 else 'stick_fire_neg')
        return finish(True)
    else:
        mark('stick_mid')
    if seq > 29:
        mark('gt29')
        return finish(False)
    longer = gt(elapsed_f, LIM_15000)         # transcurrido (float guardado) > 15000: en todos los casos con esa prueba el EXE devuelve true
    mark('c%d' % seq)

    def dw(k):
        ev.append([10 + k, tag(ARENA_CAM, 0x174 + active * 0x238), 1])
        return not bool(c['dw'])

    def thr(ms):
        return (start + ms) & M32

    if seq == 0:
        if fpv():
            if st['vtype'] == 5 and tgt_model != 0x1CC:
                mark('boat_true')
                return finish(True)
            if veh_model == 0x1B0:
                mark('rhino')
                return finish(True)
            ot, o = tpos_ref()
            if not los(ot, o, tag(ARENA_CAM, 0x310 + active * 0x238), [v & M32 for v in c['csrc']]):
                mark('los_false')
                return finish(True)
        if now > thr(5000):
            return finish(True)
        near_clip()
        return finish(False)
    if seq in (1, 2, 3, 5, 8, 15, 16, 17, 18, 19):
        if longer:
            mark('longer')
            return finish(True)
    if seq in (1, 2, 5, 8):
        if boat_true():
            return finish(True)
    if seq in (15, 16, 17):
        if not fpv():
            mark('novehicle')
            return finish(False)
    if seq == 8:
        ot, o = tpos_ref()
        if not los(ot, o, tag(ARENA_CAM, 0x310 + active * 0x238), [v & M32 for v in c['csrc']]):
            mark('los_false')
            return finish(True)
        if now > thr(1000):
            return finish(True)
        st['w138'] = CASE8_BITS
        st['w31'] = 1
        return finish(False)
    if seq in (4, 6):
        return finish(now > thr(3000))
    if seq == 29:
        return finish(now > thr(5000))
    if seq == 7:
        if not now > thr(2000):
            return finish(False)
        fpv()
        ev.append([6, tag(ARENA_VEH, 0) if hasveh else 0])
        return finish(not bool(c['onscreen']))
    if seq in (9, 10, 11, 12, 13, 14):
        return finish(False)
    if seq in (20, 21, 22, 23, 26, 27, 28):
        k = {20: 0, 21: 1, 22: 2, 23: 3, 26: 4, 27: 5, 28: 6}[seq]
        return finish(dw(k))
    if seq in (24, 25):
        st['gate'] = 0
        return finish(True)

    # casos con linea de vision del jugador al origen fijo: 1, 2, 3, 5, 15..19
    if not los_player():
        mark('los_false')
        return finish(True)
    v = offset(seq != 18)
    m = mag(v)
    if seq == 1:
        if gt(m, fc(40.0)):
            if gt(dot_a(v), ZERO_F):
                return finish(True)
        return finish(lt(m, fc(4.5)))
    if seq == 2:
        if lt(m, fc(2.0)):
            val = mul(m, HALF)
            if lt(val, FLOOR_005):
                val = FLOOR_005
            st['w138'] = bits(val)
            st['w31'] = 1
        if gt(m, fc(29.0)):
            if gt(dot_a(v), ZERO_F):
                return finish(True)
        if lt(m, fc(2.0)):
            return finish(True)
        near_clip()
        return finish(False)
    if seq == 3:
        if gt(m, fc(48.0)):
            if gt(dot_b(v), ZERO_F):
                return finish(True)
        near_clip()
        return finish(False)
    if seq == 5:
        if not gt(m, fc(38.0)):
            return finish(False)
        return finish(gt(dot_a(v), ZERO_F))
    if seq == 15:
        if gt(m, fc(44.0)):
            if gt(dot_b(v), ZERO_F):
                return finish(True)
        return finish(lt(m, fc(3.0)))
    if seq == 16:
        if gt(m, fc(50.0)):
            return finish(True)
        return finish(lt(m, fc(3.0)))
    if seq == 17:
        if gt(m, fc(50.0)):
            if gt(dot_b(v), ZERO_F):
                return finish(True)
        return finish(lt(m, fc(2.0)))
    if seq == 18:
        if gt(m, fc(57.0)):
            return finish(True)
        return finish(lt(m, fc(1.0)))
    if seq == 19:
        if gt(m, fc(36.0)):
            return finish(True)
        return finish(lt(m, fc(2.0)))
    raise RuntimeError('caso sin modelar %d' % seq)


CHANGE_KEYS = ('ret', 'changes', 'globals')


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
