"""Espejo exacto de Fixtures.hpp (Dependencies21E): misma tabla, mismo LCG y mismo reparto por indice.

run_local.py compara el fixture que registra el banco en cada fila con el que calcula este modulo, y comprueba la cobertura de rutas y de estres.
Ejecutar `python fixtures.py` valida la cobertura de los fixtures sin necesidad del juego.
"""
import struct

M32 = 0xFFFFFFFF
CASES1, CASES2, CASES3 = 1536, 1536, 3072
FP_STATUS = (0x0000, 0x4700 | (3 << 11), 0x0100 | 0x0020 | (5 << 11), 0x0400 | (7 << 11))


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return float('inf') if x > 0 else float('-inf')


def as_bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def as_float(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


class Lcg:
    def __init__(self, seed):
        self.state = (seed * 2654435761 + 12345) & M32
        self.next()
        self.next()

    def next(self):
        self.state = (self.state * 1664525 + 1013904223) & M32
        return self.state

    def below(self, n):
        return (self.next() >> 8) % n


def s32(x):
    x &= M32
    return x - (1 << 32) if x & 0x80000000 else x


def s16(x):
    x &= 0xFFFF
    return x - (1 << 16) if x & 0x8000 else x


INT_MIN, INT_MAX = -(1 << 31), (1 << 31) - 1
LIMITS = ((1, 5), (1, 5), (0, 1), (1, 2), (5, 1), (-5, 5), (0, 0), (7, 7), (0, 65536), (0, 32768), (-32768, 32767), (INT_MIN, INT_MAX),
          (INT_MAX, INT_MIN), (0, INT_MAX), (INT_MIN, 0), (1, 100000), (0x7FFFFFF0, 0x7FFFFFFF), (-1, -2), (0x40000000, -0x40000000), (123456789, -987654321),
          (2, 4), (10, 20), (1, 6), (3, 3))
RANDOMS = (0, 1, 2, 3, 0x3FFF, 0x4000, 0x7FFE, 0x7FFF, 0x8000, 0xFFFF, 0x12345, 0xFFFF0001, 0x8001, 0x5555, 0x2AAA, 0x7FFF)


def fx1(i):
    lo, hi = LIMITS[i % 24]
    return dict(lower=lo, upper=hi, random=RANDOMS[(i // 24) % 16], fp=(i // 384) % 4)


INT16_POOL = (0, 1, -1, 2, -2, 255, -255, 256, -256, 1000, -1000, 12345, -12345, 16383, -16383, 16384, -16384, 32766, -32766, 32767, -32767, -32768, 32765, -32765,
              -3, 3, 100, -100, 5000, -5000, 30000, -30000)


def fx2(i):
    fp = i % 4
    if i < 256:
        zone = [100, -200, 300, -400, 500, -600, -700, 800]
        zone[i // 32] = INT16_POOL[i % 32]
    elif i < 512:
        m = i - 256
        zone = [32767 if (m >> k) & 1 else -32768 for k in range(8)]
    else:
        r = Lcg(0x22220000 + i)
        zone = []
        for k in range(8):
            pick = r.below(4)
            if pick != 0:
                zone.append(INT16_POOL[r.below(32)])
            else:
                zone.append(s16(r.next() >> 8))
    return dict(zone=zone, fp=fp)


STRESS = (0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000, 0x80800000, 0x3F800000, 0xBF800000, 0x7F7FFFFF, 0xFF7FFFFF,
          0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7FC01234, 0xFFC54321, 0x7F800001, 0xFF800001, 0x7FA00000, 0xFFBFFFFF, 0x7149F2CA, 0xF149F2CA,
          0x0DA24260, 0x8DA24260, 0x40490FDB, 0xC0490FDB, 0x3FC90FDB, 0xBFC90FDB, 0x40C90FDB, 0xC0C90FDB)
AXES = ((0x00000000, 0x3F800000, 0x3F800000, 0x00000000), (0xBEC3EF15, 0x3F6C835E, 0x3F6C835E, 0x3EC3EF15), (0xBF3504F3, 0x3F3504F3, 0x3F3504F3, 0x3F3504F3),
        (0xBF6C835E, 0x3EC3EF15, 0x3EC3EF15, 0x3F6C835E), (0xBF800000, 0x248D3132, 0x248D3132, 0x3F800000), (0xBF6C835E, 0xBEC3EF15, 0xBEC3EF15, 0x3F6C835E),
        (0xBF3504F3, 0xBF3504F3, 0xBF3504F3, 0x3F3504F3), (0xBEC3EF15, 0xBF6C835E, 0xBF6C835E, 0x3EC3EF15), (0xA50D3132, 0xBF800000, 0xBF800000, 0x250D3132),
        (0x3EC3EF15, 0xBF6C835E, 0xBF6C835E, 0xBEC3EF15), (0x3F3504F3, 0xBF3504F3, 0xBF3504F3, 0xBF3504F3), (0x3F6C835E, 0xBEC3EF15, 0xBEC3EF15, 0xBF6C835E),
        (0x3F800000, 0xA553C9CA, 0xA553C9CA, 0xBF800000), (0x3F6C835E, 0x3EC3EF15, 0x3EC3EF15, 0xBF6C835E), (0x3F3504F3, 0x3F3504F3, 0x3F3504F3, 0xBF3504F3),
        (0x3EC3EF15, 0x3F6C835E, 0x3F6C835E, 0xBEC3EF15))
VERTICAL = (0xBE32B8C1, 0xBE32B8C0, 0xBE32B8C2, 0xBE2E147B, 0xBE3851EC, 0x00000000, 0xBF000000, 0x3F000000)
REAL_LIMIT = 0xBE32B8C1
SUB_KINDS = (802, 804, 820, 807, 808, 802, 804, 820, 807, 808)
CAR_SUBTYPES = (0, 9, 5, 4)
STICKS = ((0, 0, 0, 0), (0, 0, 1, 0), (0, 0, 0, 1), (0, 0, 1, 1), (21, 0, 0, 0), (0, -32768, 1, 0), (20, -20, 1, 0), (32767, 5, 0, 1))
PED_TYPES = (0, 1, 7)


def real_bits(r, scale):
    n = r.next()
    u = (n >> 8) / 16777216.0
    return as_bits(f32(f32(f32(u * 2.0) - 1.0) * as_float(as_bits(f32(scale)))))


def pick_bits(r, scale, mode):
    if mode == 0:
        pick = r.below(100)
        if pick < 12:
            return STRESS[r.below(32)]
        return real_bits(r, scale)
    kind = r.below(10)
    if kind < 4:
        return STRESS[r.below(32)]
    if kind == 4:
        payload = r.next() & 0x3FFFFF
        sign = r.next() & 0x80000000
        return 0x7FC00000 | payload | sign
    if kind == 5:
        payload = (r.next() & 0x3FFFFF) | 1
        sign = r.next() & 0x80000000
        return 0x7F800000 | payload | sign
    if kind < 8:
        mantissa = r.next() & 0x7FFFFF
        sign = r.next() & 0x80000000
        return mantissa | sign
    return real_bits(r, scale)


def fx3(i):
    flow = i % 1536
    pass_ = i // 1536
    gate = flow % 16
    sticks = (flow // 16) % 8
    choice0 = (flow // 128) % 4
    ped_profile = (flow // 512) % 3
    f = dict(case=i, pass_=pass_, gate=gate, sticks=sticks, choice0=choice0, ped_profile=ped_profile, fp=i % 4)
    f['car_null'] = gate == 0
    f['car_subtype'] = 3 if gate == 1 else CAR_SUBTYPES[(flow // 3) % 4]
    f['sub_null'] = gate == 2
    f['sub_kind'] = 701 if gate == 3 else 704 if gate == 4 else 9999 if gate == 5 else SUB_KINDS[gate - 6] if gate >= 6 else 802
    f['ped_type'] = PED_TYPES[ped_profile]
    f['ped_matrix'] = bool((i >> 3) & 1)
    f['aim_lr'], f['aim_ud'] = STICKS[sticks][0], STICKS[sticks][1]
    f['accel'] = 0x100 if STICKS[sticks][2] else 0
    f['brake'] = -1 if STICKS[sticks][3] else 0
    r = Lcg(0x33330000 + i)
    mode = pass_
    axis = r.below(16)
    up = [AXES[axis][0], AXES[axis][1]]
    right = [AXES[axis][2], AXES[axis][3], 0]
    right[2] = pick_bits(r, 0.2, mode)
    if mode == 1:
        for k in range(2):
            swap = r.below(2)
            bits = pick_bits(r, 1.0, 1)
            if swap:
                up[k] = bits
        for k in range(2):
            swap = r.below(2)
            bits = pick_bits(r, 1.0, 1)
            if swap:
                right[k] = bits
    pos_scale = 50.0 if mode == 0 else 500.0
    car_pos = [pick_bits(r, pos_scale, mode) for _ in range(3)]
    ped_pos = [pick_bits(r, pos_scale, mode) for _ in range(3)]
    bits = pick_bits(r, 8.0, mode)
    distance = (bits & 0x7FFFFFFF) if mode == 0 else bits
    bits = pick_bits(r, 4.0, mode)
    min_y = (bits | 0x80000000) if mode == 0 else bits
    horizontal = pick_bits(r, 3.2, mode)
    close_to = r.below(100)
    index = r.below(8)
    bits = pick_bits(r, 0.6, mode)
    vertical = VERTICAL[index] if close_to < 45 else bits
    alpha = pick_bits(r, 1.5, mode)
    beta = pick_bits(r, 1.5, mode)
    kind = r.below(4)
    bits = pick_bits(r, 0.3, 1)
    limit = REAL_LIMIT if (mode == 0 or kind < 2) else bits
    f.update(up=up, right=right, car_pos=car_pos, ped_pos=ped_pos, distance=distance, min_y=min_y, horizontal=horizontal, vertical=vertical, alpha=alpha, beta=beta, limit=limit)
    return f


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and not (b & 0x400000)


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


if __name__ == '__main__':
    # cobertura estatica de los fixtures (sin juego)
    seen = {'nan_payload': set(), 'snan': 0, 'denormal': 0, 'inf': 0, 'neg_zero': 0, 'flt_max': 0, 'big': 0}
    for i in range(CASES3):
        f = fx3(i)
        for name in ('distance', 'min_y', 'horizontal', 'vertical', 'alpha', 'beta', 'limit'):
            b = f[name]
            if is_nan(b):
                seen['nan_payload'].add(b)
            seen['snan'] += is_snan(b)
            seen['denormal'] += is_denormal(b)
            seen['inf'] += is_inf(b)
            seen['neg_zero'] += b == 0x80000000
            seen['flt_max'] += (b & 0x7FFFFFFF) == 0x7F7FFFFF
            seen['big'] += (b & 0x7FFFFFFF) in (0x7149F2CA,)
    assert len(seen['nan_payload']) > 40 and seen['snan'] > 20 and seen['denormal'] > 50 and seen['inf'] > 30 and seen['neg_zero'] > 20 and seen['flt_max'] > 10 and seen['big'] > 5, seen
    print('fixtures: %d + %d + %d casos; estres: %s' % (CASES1, CASES2, CASES3, {k: (len(v) if isinstance(v, set) else v) for k, v in seen.items()}))


# ---------------- cobertura de rutas de 0x63A380 (modelo en float64: solo para exigir que los fixtures recorren cada rama; la igualdad la decide el banco) ----------------
import math

OK_KINDS = (802, 804, 820, 807, 808)
PI, TWO_PI, HALF_PI = as_float(0x40490FDB), as_float(0x40C90FDB), as_float(0x3FC90FDB)
EIGHT_DEG = as_float(0x3E0EFA35)
T1 = (as_float(0x3DCCCCCD), as_float(0x3E4CCCCD), as_float(0x3E4CCCCD))   # 0x86E684, indexado por la eleccion
T2 = (as_float(0x3DCCCCCD), as_float(0x3E4CCCCD), as_float(0x3E4CCCCD))   # 0x86E678


def _nan(x):
    return x != x


def paths3(f):
    """Etiquetas de ruta recorridas por el fixture f (dict de fx3)."""
    p = set()
    if f['car_null']:
        return {'car_null'}
    if f['car_subtype'] == 3:
        return {'heli'}
    if f['sub_null']:
        return {'sub_null'}
    if f['sub_kind'] not in OK_KINDS:
        return {'sub_other'}
    p.add('pad%d' % (1 if f['ped_type'] == 1 else 0))
    p.add('sub_%d' % f['sub_kind'])
    choice = f['choice0']
    if abs(f['aim_lr']) > 20:
        choice = 3
        p.add('stick_lr_beyond')
    elif abs(f['aim_ud']) > 20:
        choice = 3
        p.add('stick_ud_beyond')
    elif f['accel']:
        choice = 2
        p.add('accelerate')
    elif f['brake']:
        choice = 1
        p.add('brake')
    else:
        p.add('choice_kept')
    if choice == 3:
        p.add('choice3_exit')
        return p
    p.add('choice%d' % choice)
    p.add('ped_matrix' if f['ped_matrix'] else 'ped_placeable')
    fl = lambda b: as_float(b)
    up, right = [fl(b) for b in f['up']], [fl(b) for b in f['right']]
    car, ped = [fl(b) for b in f['car_pos']], [fl(b) for b in f['ped_pos']]
    dist, min_y, hz, vert, alpha, beta, limit = (fl(f[k]) for k in ('distance', 'min_y', 'horizontal', 'vertical', 'alpha', 'beta', 'limit'))
    try:
        heading = math.atan2(-up[0], up[1]) - HALF_PI
        dx, dy, dz = ped[0] - car[0], ped[1] - car[1], ped[2] - car[2]
        dot = (dz * right[2] + dy * right[1]) + dx * right[0]
        angle = math.atan2(dot, dist) * 0.7
    except (ValueError, OverflowError):
        heading, dot, angle = float('nan'), float('nan'), float('nan')
    # casos con NaN/inf que el modelo float64 no decide con exactitud
    x = angle
    nan_min_y = _nan(min_y)
    if nan_min_y:
        p.add('min_y_nan')
        if is_denormal(f['distance']):
            p.add('min_y_nan_distance_denormal')
    elif -min_y > dist:
        p.add('min_y_beyond')
        x = -EIGHT_DEG if angle > 0 else EIGHT_DEG
    else:
        p.add('min_y_inside')
    target = x + (heading + PI) if choice == 1 else heading - x
    if _nan(target):
        p.add('target_nan')
        if is_denormal(f['horizontal']):
            p.add('target_nan_horizontal_denormal')
        delta = float('nan')
    else:
        if target > hz + PI:
            target -= TWO_PI
            p.add('wrap_minus')
        elif target < hz - PI:
            target += TWO_PI
            p.add('wrap_plus')
        else:
            p.add('wrap_none')
        delta = target - hz
    dot_pos = dot > 0
    if dot_pos == (choice == 1):
        if delta <= -HALF_PI:
            delta = -delta
            p.add('flip_a')
    else:
        if delta >= HALF_PI:
            delta = -delta
            p.add('flip_b')
    t1, t2 = T1[choice], T2[choice]
    if delta > t1:
        delta = t1
        p.add('clamp_high')
    elif delta < -t1:
        delta = -t1
        p.add('clamp_low')
    else:
        p.add('clamp_none')
    if _nan(delta):
        p.add('delta_nan')
        if is_denormal(f['beta']):
            p.add('delta_nan_beta_denormal')
    p.add('beta_denormal' if is_denormal(f['beta']) else 'beta_normal')
    if _nan(vert):
        p.add('vertical_nan')
        if is_denormal(f['limit']):
            p.add('vertical_nan_limit_denormal')
    elif vert > limit:
        p.add('vertical_above')
    else:
        p.add('vertical_below')
    return p


REQUIRED_PATHS3 = {'car_null', 'heli', 'sub_null', 'sub_other', 'pad0', 'pad1', 'sub_802', 'sub_804', 'sub_820', 'sub_807', 'sub_808', 'stick_lr_beyond', 'stick_ud_beyond', 'accelerate',
                   'brake', 'choice_kept', 'choice3_exit', 'choice0', 'choice1', 'choice2', 'ped_matrix', 'ped_placeable', 'min_y_beyond', 'min_y_inside', 'min_y_nan',
                   'wrap_minus', 'wrap_plus', 'wrap_none', 'flip_a', 'flip_b', 'clamp_high', 'clamp_low', 'clamp_none', 'vertical_above', 'vertical_below', 'vertical_nan',
                   'target_nan', 'delta_nan', 'beta_denormal', 'min_y_nan_distance_denormal', 'target_nan_horizontal_denormal', 'delta_nan_beta_denormal',
                   'vertical_nan_limit_denormal'}


def coverage3():
    counts = {}
    for i in range(CASES3):
        for t in paths3(fx3(i)):
            counts[t] = counts.get(t, 0) + 1
    return counts


if __name__ == '__main__':
    counts = coverage3()
    missing = REQUIRED_PATHS3 - set(counts)
    print('rutas de 0x63A380:', {k: counts[k] for k in sorted(counts)})
    assert not missing, 'rutas sin cubrir: %s' % sorted(missing)
