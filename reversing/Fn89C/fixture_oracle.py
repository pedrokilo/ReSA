"""Oraculo independiente (Python) de Fn89C: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x627CC0..0x62833E (y de los ayudantes reales 0x59C890, 0x59C910, 0x40FE90, 0x40FE30
y 0x5DF8F0), lo que debe observarse de CTaskSimpleGangDriveBy::FireGun (thiscall, ECX = tarea; ped en pila, `ret 4`; void) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits, exponente ampliado) se modela con fracciones exactas.

Modelo (desensamblado):
  0x627CC3..0x627D1C: muzzle = CWeaponInfo +0x24 (3 floats); jerarquia = 0x734A40(ped+0x18); indice = 0x7C51A0(jerarquia, [ped+0x4A0]+0x14); matrices = 0x7C5120(jerarquia); 0x7EDD90(muzzle, muzzle, 1, matrices + indice*0x40).
  q = muzzle - posicion del vehiculo (matriz +0x30), cada componente guardado como float. caja = [ms_modelInfoPtrs[(int16)vehiculo+0x22]+0x14].
  0x5DF8F0: jugador si ped+0x598 es 0 o 1. Jugador: camara activa (byte 0xB6F081, 0x238 por camara desde 0xB6F1A8): modo (word) 0x31: rango = |pos del ped - origen de la camara| (sqrt((c*c + b*b) + a*a)) + 20.0 (0x858BA4),
  0x37: rango 20.0, otro modo: sale sin disparar. 0x514970(rango, muzzle por valor, &source, &target); direccion = target - muzzle; datos del jugador +0x2C = 0.
  IA: entidad (+0x34) con matriz (+0x14): objetivo = 0x59C890(matriz, coords) y direccion = objetivo - muzzle; sin entidad: coords - muzzle.
  Direccion normalizada con 0x59C910 ((x*x + y*y) + z*z; <= 0 deja x = 1.0 y no toca y, z; si no, inv = 1.0/sqrt, cada componente guardado como float).
  Cara segun la asociacion de animacion (+0x2C: word en +0x2C de la asociacion) y m_nFakeShootDirn (byte +0x14): E7/EC o 1 izquierda, EB/E8 o 3 derecha, EA/EE o 2 atras, otro adelante.
  Producto con el eje (derecho +0/+4/+8 o adelante +0x10/+0x14/+0x18 de la matriz del vehiculo): (d.z*e.z + d.y*e.y) + d.x*e.x, negado en izquierda y atras; si no supera 0.1f (ordenado) el impacto es el propio muzzle.
  Si no: t = ((cara - ((q.y*e.y + q.z*e.z) + q.x*e.x)) -/+ 0.2f) / producto. Izquierda/derecha: x = d.x*t + p.x (producto en el registro); y, z con el producto guardado como float.
  Atras/adelante: tf = float(t); p + float(tf * d) por componente (0x40FE90 y 0x40FE30). Despues ped+0x528 |= 1; arma = ped + 0x5A0 + (int8)ranura * 0x1C.
  Coordenadas todas == 0 (ordenado): Fire(ped, &muzzle, &muzzle, entidad, nullptr, &impacto); si no, objetivo = coords (o 0x59C890(matriz, coords) con entidad): Fire(..., &objetivo, &impacto).
  DoGunFlash(ped, word [0x8D2E90] = 250, 0).
  Las mutaciones (una vez) escriben, dentro de un falso, en la tarea (entidad, coords.x, asociacion) o en el vehiculo (modelo); el EXE lee todo eso despues de esas llamadas.

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


# ---------------------------------------------------------------- constantes del EXE
K_RANGE = load(0x41A00000)      # 0x627DA6: 20.0f (valor por omision del rango)
K_RANGE_ADD = load(0x41A00000)  # 0x858BA4 = 20.0
K_01 = load(0x3DCCCCCD)         # 0x858B1C = 0.1f
K_02 = load(0x3E4CCCCD)         # 0x858CC4 = 0.2f
ONE = load(0x3F800000)          # 0x858624
ZERO_F = load(0)                # 0x858B50
RANGE_DEFAULT_BITS = 0x41A00000
FLASH_DURATION = 250            # word 0x8D2E90
CAM_BASE = 0xB6F028

STACK = 0x7F000000
# etiquetas de arenas (orden de TagAdd en LiveComparison.hpp)
A_TASK, A_PED, A_PD, A_WI, A_ENT, A_EMAT, A_VEH, A_VMAT, A_MIA, A_MIB, A_COLA, A_COLB, A_CLUMP, A_BONE, A_HIER, A_MA, A_ASSOC, A_PMAT = range(18)
MODEL_BASE = 19980


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def normalise(v):
    """CVector::Normalise 0x59C910: devuelve (los tres dwords resultantes, si la suma de cuadrados fue <= 0)"""
    x, y, z = load(v[0]), load(v[1]), load(v[2])
    total = add(add(mul(x, x), mul(y, y)), mul(z, z))
    if total != NAN and not gt(total, ZERO_F):
        return [0x3F800000, v[1], v[2]], True
    inv = div(ONE, sqrt24(total))
    return [bits(mul(inv, x)), bits(mul(inv, y)), bits(mul(inv, z))], False


def mat_times(m, p):
    """CMatrix * CVector 0x59C890 (m = 16 dwords de la matriz, p = 3 dwords): tres dwords"""
    px, py, pz = load(p[0]), load(p[1]), load(p[2])
    x = add(add(add(mul(load(m[8]), pz), mul(load(m[4]), py)), mul(load(m[0]), px)), load(m[12]))
    y = add(add(add(mul(load(m[9]), pz), mul(load(m[1]), px)), mul(load(m[5]), py)), load(m[13]))
    z = add(add(add(mul(load(m[10]), pz), mul(load(m[2]), px)), mul(load(m[6]), py)), load(m[14]))
    return [bits(x), bits(y), bits(z)]


def sub3(a, b):
    return [bits(sub(load(a[k]), load(b[k]))) for k in range(3)]


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    site = c['mutsite']
    mv = c['mutval'] & M32
    done = set()
    st = {'ent': bool(c['ent']), 'assoc': bool(c['assoc']), 'vmodel': c['vmodel'], 'coords': [v & M32 for v in c['coords']], 'task_changes': [], 'veh_changes': []}

    def once(s):
        if site != s or s in done:
            return False
        done.add(s)
        mark('mut%d' % s)
        return True

    changes = []
    cm = [v & M32 for v in c['xform']]
    idx = s32(c['boneidx'])
    ev.append([1, tag(A_CLUMP, 0)])
    if once(1):
        st['ent'] = not st['ent']
    ev.append([2, tag(A_HIER, 0), c['bonetag'] & M32])
    if once(2):
        st['vmodel'] = c['vmodel2']
    ev.append([3, tag(A_HIER, 0)])
    ev.append([4, STACK, STACK, 1, tag(A_MA, 0x80 + idx * 0x40)] + [v & M32 for v in c['fireoff']] + [1])
    if once(3):
        st['coords'][0] = mv
    vmat = [v & M32 for v in c['vmat']]
    muzzle = cm
    q = sub3(muzzle, vmat[12:15])
    box = [v & M32 for v in (c['boxa'] if st['vmodel'] % 2 == 0 else c['boxb'])]
    player = c['ptype'] in (0, 1)
    mark('player' if player else 'ai')
    pd_clear = False
    if player:
        mode = c['cmode'][c['cam']] & 0xFFFF
        rng = RANGE_DEFAULT_BITS
        if mode == 0x31:
            mark('cam31')
            pos = [v & M32 for v in c['ppos']]
            src = [v & M32 for v in c['csrc'][c['cam'] * 3:c['cam'] * 3 + 3]]
            a = sub(load(pos[0]), load(src[0]))
            b = sub(load(pos[1]), load(src[1]))
            cc = sub(load(pos[2]), load(src[2]))
            length = sqrt24(add(add(mul(cc, cc), mul(b, b)), mul(a, a)))
            rng = bits(add(length, K_RANGE_ADD))
        elif mode == 0x37:
            mark('cam37')
        else:
            mark('cam_other')
            return finish_early(c, ev, marks, st, changes, site, done)
        ev.append([5, CAM_BASE, rng] + muzzle + [STACK, STACK])
        if once(4):
            st['assoc'] = not st['assoc']
        direction = sub3([v & M32 for v in c['ftgt']], muzzle)
        pd_clear = True
    else:
        if st['ent']:
            mark('ent')
            target = mat_times([v & M32 for v in c['emat']], st['coords'])
            direction = sub3(target, muzzle)
        else:
            mark('noent')
            direction = sub3(st['coords'], muzzle)
    nd, zero_norm = normalise(direction)
    if zero_norm:
        mark('norm_zero')
    if any(load(v) == NAN for v in nd):
        mark('norm_nan')
    d = nd
    animid = c['animid'] & 0xFFFF
    fdir = c['fdir'] & 0xFF
    has = st['assoc']
    mark('assoc' if has else 'noassoc')
    if (has and animid in (0xE7, 0xEC)) or fdir == 1:
        side = 0
    elif (has and animid in (0xEB, 0xE8)) or fdir == 3:
        side = 1
    elif (has and animid in (0xEA, 0xEE)) or fdir == 2:
        side = 2
    else:
        side = 3
    mark('side%d' % side)
    ax = vmat[0:3] if side < 2 else vmat[4:7]
    dot = add(add(mul(load(d[2]), load(ax[2])), mul(load(d[1]), load(ax[1]))), mul(load(d[0]), load(ax[0])))
    if side in (0, 2):
        dot = neg(dot)
    if dot == NAN:
        mark('dot_nan')
    if not gt(dot, K_01):
        mark('noimpact')
        hit = list(muzzle)
    else:
        mark('impact')
        mark('impact%d' % side)
        dq = add(add(mul(load(q[1]), load(ax[1])), mul(load(q[2]), load(ax[2]))), mul(load(q[0]), load(ax[0])))
        face = load(box[{0: 0, 1: 3, 2: 1, 3: 4}[side]])
        base = sub(face, dq)
        numer = sub(base, K_02) if side in (0, 2) else add(base, K_02)
        t = div(numer, dot)
        if side < 2:
            hit = [bits(add(mul(load(d[0]), t), load(muzzle[0]))),
                   bits(add(reload(store(mul(load(d[1]), t))), load(muzzle[1]))),
                   bits(add(reload(store(mul(load(d[2]), t))), load(muzzle[2])))]
        else:
            tf = reload(store(t))
            prod = [bits(mul(tf, load(d[k]))) for k in range(3)]
            hit = [bits(add(load(muzzle[k]), load(prod[k]))) for k in range(3)]
    ik_new = (c['ikinit'] | 1) & M32
    old, new = b4(c['ikinit'] & M32), b4(ik_new)
    changes += [[A_PED, 0x528 + j, new[j]] for j in range(4) if new[j] != old[j]]
    weapon = tag(A_PED, 0x5A0 + c['slot'] * 0x1C)
    ped = tag(A_PED, 0)
    entag = tag(A_ENT, 0) if st['ent'] else 0
    coords = st['coords']
    if all(is_zero(load(v)) for v in coords):
        mark('zero_coords')
        ev.append([6, weapon, ped, 1] + muzzle + muzzle + [entag, 0, 0, 0, 0] + hit + [0])
    else:
        mark('nz_coords')
        target = list(coords)
        if st['ent']:
            target = mat_times([v & M32 for v in c['emat']], coords)
        ev.append([6, weapon, ped, 1] + muzzle + muzzle + [entag, 1] + target + hit + [0])
    ev.append([7, ped, FLASH_DURATION, 0])
    return finish(c, ev, marks, st, changes, pd_clear, site, done)


def mut_changes(c, st, site, done, changes):
    if 2 in done:
        old, new = b4((MODEL_BASE + c['vmodel']) & 0xFFFF), b4((MODEL_BASE + c['vmodel2']) & 0xFFFF)
        changes += [[A_VEH, 0x22 + j, new[j]] for j in range(2) if new[j] != old[j]]
    if 3 in done:
        old, new = b4(c['coords'][0] & M32), b4(st['coords'][0])
        changes += [[A_TASK, 0x38 + j, new[j]] for j in range(4) if new[j] != old[j]]


def result(c, ev, marks, st, changes, pd_clear, site, done):
    if pd_clear:
        old, new = b4(c['atkinit'] & M32), [0, 0, 0, 0]
        changes += [[A_PD, 0x2C + j, new[j]] for j in range(4) if new[j] != old[j]]
    mut_changes(c, st, site, done, changes)
    changes.sort(key=lambda e: (e[0], e[1]))
    return {'changes': changes, 'events': ev, 'task_ptrs': [st['assoc'], st['ent']], 'marks': marks}


def finish_early(c, ev, marks, st, changes, site, done):
    marks.append('no_fire')
    return result(c, ev, marks, st, changes, False, site, done)


def finish(c, ev, marks, st, changes, pd_clear, site, done):
    marks.append('fired')
    return result(c, ev, marks, st, changes, pd_clear, site, done)


CHANGE_KEYS = ('changes', 'task_ptrs')


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
