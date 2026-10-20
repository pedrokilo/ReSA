"""Oraculo independiente del banco AutomobileWrappers60: tres funciones de CAutomobile (0x44D520, 0x6A2B10, 0x6AA290).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco; el oraculo no repite ninguna constante de arena
(las arenas viven en Core.hpp). Aritmetica float: cada operacion se redondea a 24 bits de mantisa (x87 con control de precision sencillo); +,-,*,/ entre float32 calculados en
doble y redondeados a float32 son exactos (53 >= 2*24+2). fsin, fpatan y _CIasin no se modelan al ulp: los valores que dependen de ellos van como {"approx": bits} y
`matches` acepta 2 ulp (EXE == DLL se exige en bits en run_local/probe_case).

Resumen del EXE (direcciones del desensamblado):
 0x44D520 HasCarStoppedBecauseOfLight: byte+0x36>>3 en {2,3}; PathNodes[cur.area]/[start.area] != 0; recorre los enlaces del nodo `cur` buscando el enlace == start y el de
   `end` buscando == cur (end.area == 0xFFFF o area descargada -> false); acierta si naviNodes[navi>>10][navi&0x3FF].byte[0xC] & 3 y PathNodes[navi>>10] != 0.
 0x6A2B10 Render: ver expected_r (estado 30 ALPHATESTFUNCTIONREF, ruedas con el truco de coche invisible, orugas del Rhino, haces de faros).
 0x6AA290 UpdateWheelMatrix: ver expected_w (6AA307.. direccion, 6AA495.. escala, 6AA6D6.. inclinacion / agua).
"""
import math
import struct
import sys

M32 = 0xFFFFFFFF
CASES = 4096
NAMES = ['CAutomobile::HasCarStoppedBecauseOfLight', 'CAutomobile::Render', 'CAutomobile::UpdateWheelMatrix']
KINDS = 8


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def bits(x):
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0x7F800000 if x > 0 else 0xFF800000


def f32(x):
    return fl(bits(x))


def mul(a, b):
    return f32(a * b)


def add(a, b):
    return f32(a + b)


def sub(a, b):
    return f32(a - b)


def div(a, b):
    if b == 0:
        if a == 0 or math.isnan(a):
            return fl(0xFFC00000)
        neg = (math.copysign(1.0, a) < 0) != (math.copysign(1.0, b) < 0)
        return -math.inf if neg else math.inf
    return f32(a / b)


def sin_(x):
    if math.isinf(x):
        return fl(0xFFC00000)       # fsin(+-inf): NaN indefinido
    return math.sin(x)


C = {k: fl(v) for k, v in dict(pi=0x40490FDB, half_pi=0x3FC90FDB, neg_half_pi=0xBFC90FDB, c06=0x3F19999A, c065=0x3F266666, c08=0x3F4CCCCD, c11=0x3F8CCCCD, c125=0x3FA00000,
                               c3=0x40400000, c17=0x3FD9999A, c045=0x3EE66666, c03=0x3E99999A, c07=0x3F333333, c02=0x3E4CCCCD, c005=0x3D4CCCCD, c12=0x3F99999A).items()}


def runs(old, new, base=0):
    """Tramos contiguos que cambian: [desplazamiento, hex nuevo], como DiffRuns de Core.hpp."""
    out = []
    i = 0
    while i < len(old):
        if old[i] == new[i]:
            i += 1
            continue
        j = i
        while j < len(old) and old[j] != new[j]:
            j += 1
        out.append([base + i, bytes(new[i:j]).hex()])
        i = j
    return out


def matches(observed, expected):
    """Igualdad estructural; {"approx": bits} acepta como mucho 2 ulp (o NaN/NaN, o igual infinito)."""
    if isinstance(expected, dict):
        if 'approx' in expected:
            return isinstance(observed, int) and approx_equal(observed, expected['approx'])
        return isinstance(observed, dict) and set(observed) == set(expected) and all(matches(observed[k], expected[k]) for k in expected)
    if isinstance(expected, list):
        return isinstance(observed, list) and len(observed) == len(expected) and all(matches(o, e) for o, e in zip(observed, expected))
    return observed == expected


def ordered(b):
    b &= M32
    return (~b & M32) if b & 0x80000000 else (b | 0x80000000)


def approx_equal(a, b):
    x, y = fl(a), fl(b)
    if math.isnan(x) or math.isnan(y):
        return math.isnan(x) and math.isnan(y)
    if math.isinf(x) or math.isinf(y):
        return a == b
    return abs(ordered(a) - ordered(b)) <= 2


# =============================================================================================== 0x44D520
def expected_s(inp):
    st = {}
    status = (inp['status'] >> 3) & 0x1F
    pal = {a['id']: a for a in inp['pal'] if a['loaded']}
    poison, beyond = inp['poison_link'], inp['beyond_cl']

    def done(result, why):
        st['why'] = why
        st['result'] = result
        return {'al': 1 if result else 0, 'car_unchanged': True, 'guard_ok': True, 'arenas_unchanged': True, 'tables_unchanged': True, 'trace': []}, st

    def scan(frm, tgt):
        area = pal[frm[0]]
        node = frm[1]
        num = area['num'][node] & 0xF
        base = area['base'][node]
        st['links_scanned'] = st.get('links_scanned', 0) + num
        for i in range(num):
            idx = base + i
            inside = 0 <= idx < len(area['la'])
            la = area['la'][idx] if inside else poison
            ln = area['ln'][idx] if inside else poison
            if (la, ln) != (tgt[0], tgt[1]):
                continue
            st['link_matches'] = st.get('link_matches', 0) + 1
            raw = area['navi'][idx] if inside else poison
            narea = raw >> 10
            if narea not in pal:
                st['navi_area_null'] = st.get('navi_area_null', 0) + 1
                continue
            cl = pal[narea]['cl']
            byte = cl[raw & 0x3FF] if (raw & 0x3FF) < len(cl) else beyond
            if byte & 3:
                return True
        return False

    if status not in (2, 3):
        return done(False, 'status')
    cur, start, end = inp['cur'], inp['start'], inp['end']
    if cur[0] not in pal:
        return done(False, 'cur_null')
    if start[0] not in pal:
        return done(False, 'start_null')
    if scan(cur, start):
        return done(True, 'part1')
    if end[0] == 0xFFFF:
        return done(False, 'end_ffff')
    if end[0] not in pal:
        return done(False, 'end_null')
    if scan(end, cur):
        return done(True, 'part2')
    return done(False, 'part2_false')


# =============================================================================================== 0x6A2B10
def expected_r(inp):
    st = {}
    trace = [['sget', 30], ['sset', 30, 1]]
    model = inp['model']
    pos = {n: [fl(v) for v in inp['fpos'][n - 2]] for n in range(2, 8)}
    st['cheat'] = bool(inp['cheat'])
    if inp['cheat']:
        order = [4, 7, 2, 5] + ([] if inp['node3_null'] else [3]) + ([] if inp['node6_null'] else [6])
        for n in order:
            trace += [['gfo', 'F%d' % n, 'A'], ['gfo', 'F%d' % n, 'B'], ['rcb', 'B%d' % n]]
        st['wheel_nodes'] = len(order)
    else:
        trace.append(['vrender', 'car'])
    st['rhino'] = model == 432
    st['nan_track'] = False
    st['v_clamped'] = 0
    if model == 432:
        trace.append(['col', 'car'])
        tracks = [fl(t) for t in inp['tracks']]
        line0, spring0 = fl(inp['line0']), fl(inp['spring0'])
        ratio = div(sub(line0, spring0), line0)
        st['nan_track'] = any(math.isnan(t) or math.isinf(t) for t in tracks) or math.isnan(ratio)
        for i in range(8):
            v = sub(tracks[i], ratio)
            if 0.0 > v:
                v = 0.0
                st['v_clamped'] += 1
            u = sub(1.0, v)
            line = inp['lines'][4 + i]
            s = [fl(x) for x in line[0:3]]
            e = [fl(x) for x in line[3:6]]
            sx, sy = mul(u, s[0]), mul(u, s[1])
            ez = mul(v, e[2])
            x = add(mul(v, e[0]), sx)
            y = add(mul(v, e[1]), sy)
            z = add(ez, mul(u, s[2]))
            node = 7 if i < 4 else 4
            p = pos[node]
            move = [sub(x, p[0]), sub(y, p[1]), sub(z, p[2])]
            trace.append(['xlate', 'M%d' % node] + [bits(m) for m in move] + [2])
            pos[node] = [add(p[k], move[k]) for k in range(3)]
            trace += [['upd', 'F%d' % node], ['gfo', 'F%d' % node, 'A'], ['gfo', 'F%d' % node, 'B'], ['rcb', 'B%d' % node]]
            neg = [mul(m, -1.0) for m in move]
            trace.append(['xlate', 'M%d' % node] + [bits(m) for m in neg] + [2])
            pos[node] = [add(pos[node][k], neg[k]) for k in range(3)]
    saved = (inp['get_val'] & M32) if inp['get_writes'] else 1
    trace.append(['sset', 30, saved])
    st['beam_r'] = st['beam_l'] = False
    st['appear'] = False
    if model not in (441, -2, 432) and not (inp['rc'] & 0x80):
        trace.append(['appear', 'car'])
        st['appear'] = True
        if inp['appearance'] != 3:
            if inp['lights'] & 1:
                trace.append(['beam', 'car', 0, 'matrix', 1])
                st['beam_r'] = True
            if inp['lights'] & 2:
                trace.append(['beam', 'car', 0, 'matrix', 0])
                st['beam_l'] = True
    new_time = (inp['time'] + 3000) & M32
    st['time_wrap'] = inp['time'] + 3000 > M32
    st['model_class'] = 'rhino' if model == 432 else 'bandit' if model == 441 else 'minus2' if model == -2 else 'other'
    car_diff = runs(struct.pack('<I', inp['old4d0']), struct.pack('<I', new_time), 0x4D0)
    frames = [[bits(v) for v in pos[n]] for n in range(2, 8)]
    return {'car_diff': car_diff, 'guard_ok': True, 'frames': frames, 'frames_rest_ok': True, 'arenas_unchanged': True, 'trace': trace}, st


# =============================================================================================== 0x6AA290
def expected_w(inp):
    st = {'ran': False}
    node, flags = inp['node'], inp['flags']
    frame = list(inp['mat'])
    trace = []

    def result():
        return {'car_diff': [], 'guard_ok': True, 'frame': frame, 'arenas_unchanged': True, 'trace': trace}, st

    if not inp['has_frame']:
        st['why'] = 'null_frame'
        return result()
    if not (2 <= node <= 7):
        st['why'] = 'bad_node'
        return result()
    st['ran'] = True
    st['node%d' % node] = True
    hf, mf = inp['hflags'], inp['mflags']
    rearsteer, hbsteer = bool(hf & 0x20), bool(hf & 0x40)
    steer_in, steer2_in = fl(inp['steer']), fl(inp['steer2'])
    front = rear = fixed = False
    pi = C['pi']
    if node == 5:
        wheel, sign, front = 0, -1.0, True
        steer, fixed = (pi, True) if rearsteer else (steer_in, False)
    elif node == 2:
        wheel, sign, front = 2, 1.0, True
        steer, fixed = (0.0, True) if rearsteer else (steer_in, False)
    elif node in (6, 7):
        wheel, sign, rear = 1, -1.0, True
        if rearsteer:
            steer, fixed = -steer_in, False
        elif hbsteer:
            steer, fixed = steer2_in, False
        else:
            steer, fixed = pi, True
    else:
        wheel, sign, rear = 3, 1.0, True
        if rearsteer:
            steer, fixed = -steer_in, False
        elif hbsteer:
            steer, fixed = steer2_in, False
        else:
            steer, fixed = 0.0, True
    st['steer_fixed'] = fixed
    if not fixed:
        if not (steer == 0.0) and not (flags & 2):
            neg_side = sign < 0
            scale_it = (steer > 0.0) if (neg_side == rear) else (steer < 0.0)
            st['scale_it'] = scale_it
            if scale_it:
                steer = mul(C['c06'], steer)
            if neg_side:
                steer = add(steer, pi)
        else:
            steer = pi if sign < 0 else 0.0
            st['mirrored'] = bool(flags & 2)
    trace.append(['attach', 'L', 'FM', 0])
    mat = inp['mat']
    off = [fl(mat[12]), fl(mat[13]), fl(inp['wpos'][wheel])]
    ws = fl(inp['wheel_scale'])
    mi_front, mi_rear = fl(inp['mi_front']), fl(inp['mi_rear'])
    wheel_size = ws
    if inp['mi_wheel_model'] == -1:
        if rear:
            wheel_size = mul(div(mi_rear, mi_front), wheel_size)
        x_scale = div(mi_front, mul(ws, C['c07']))
        st['default_wheel'] = True
    else:
        x_scale = 1.0
        wheel_size = mi_rear if rear else mi_front
    model = inp['model']
    if model == 532 and node in (6, 3):
        wheel_size = div(C['c17'], mi_front)
        off[2] = add(mul(sub(C['c17'], mi_rear), C['c045']), off[2])
        steer = mul(sub(steer, pi) if sign < 0 else steer, 0.5)
        if sign < 0:
            steer = add(steer, pi)
        st['harvester'] = True
    elif model == 513 and node == 5:
        steer = 0.0
        st['m513'] = True
    if inp['subtype'] in (1, 2):
        x_scale = 1.0
        st['mtruck_quad'] = True
    elif rear and (hf & 0xF000):
        x_scale = mul(x_scale, C['c065'] if hf & 0x1000 else C['c08'] if hf & 0x2000 else C['c11'] if hf & 0x4000 else C['c125'])
        st['wide_rear'] = True
    elif front and (hf & 0x0F00):
        x_scale = mul(x_scale, C['c065'] if hf & 0x100 else C['c08'] if hf & 0x200 else C['c11'] if hf & 0x400 else C['c125'])
        st['wide_front'] = True
    if model == 571:
        x_scale = mul(x_scale, C['c3'])
        st['kart'] = True
    sx = mul(x_scale, wheel_size)
    trace.append(['scale', bits(sx), bits(wheel_size), bits(wheel_size)])
    if inp['subtype'] == 3:
        trace.append(['rot', 0, 0, bits(steer)])
        st['heli'] = True
    else:
        status = inp['wstat'][wheel]
        angle = mul(sign, fl(inp['rot'][wheel]))
        if status == 1:
            z = {'approx': bits(add(mul(sin_(angle), C['c03']), steer))}
            st['burst'] = True
            st['sin_inf'] = math.isinf(angle)
        else:
            z = bits(steer)
        trace.append(['rot', bits(angle), 0, z])
    monster = bool(inp['auto_flags'] & 0x80) or bool(hf & 0x20000)
    solid = monster
    if not solid:
        solid = (front and bool(mf & 0x20000)) or (rear and bool(mf & 0x200000))
    if solid:
        other = wheel - 2 if wheel > 1 else wheel + 2
        wp = [fl(v) for v in inp['wpos']]
        two_abs_y = mul(abs(off[1]), 2.0)
        diff = mul(mul(sub(wp[wheel], wp[other]), sign), -1.0)
        trace.append(['roty', {'approx': bits(math.atan2(diff, two_abs_y))}])
        st['branch'] = 'solid'
    elif (inp['cheat'] or model == 539) and fl(inp['comp'][wheel]) < 1.0 and not (inp['vflags'] & 0x40000000):
        st['branch'] = 'water'
        if not (flags & 4):
            trace.append(['roty', bits(mul(sign, C['neg_half_pi']))])
        if not (flags & 8):
            st['water_effects'] = True
            cp = [fl(v) for v in inp['cp'][wheel]]
            right = [fl(v) for v in inp['right']]
            move = [fl(v) for v in inp['move']]
            r = [mul(sign, x) for x in right]
            h = [mul(0.5, x) for x in move]
            vel = [add(h[k], r[k]) for k in range(3)]
            surf = inp['surf'][wheel]
            trace.append(['iswater', surf])
            water_m = [1.0, 1.0, 1.0, C['c02'], 0.5, 0.0, C['c005']]
            if (surf & 3) == 1:
                st['is_water'] = True
                if ((inp['frame_counter'] - 1) & M32) & 1:
                    vel[2] = add(vel[2], 2.0)
                    sysl = 'WS'
                    st['frame_even'] = True
                else:
                    sysl = 'SH'
                trace.append(['addp', sysl] + [bits(v) for v in cp] + [bits(v) for v in vel] + [0] + [bits(v) for v in water_m] + [bits(-1.0), bits(C['c12']), bits(C['c06']), 0])
            else:
                trace.append(['issand', surf])
                if (surf & 3) == 2:
                    st['is_sand'] = True
                    sand_m = [fl(0x3F4F5C29), fl(0x3F2B851F), fl(0x3F11EB85), fl(0x3EB33333), 0.5, 0.0, C['c005']]
                    trace.append(['addp', 'SA'] + [bits(v) for v in cp] + [bits(v) for v in vel] + [0] + [bits(v) for v in sand_m] + [bits(-1.0), bits(C['c12']), bits(C['c06']), 0])
    elif not (flags & 1) and not (front and (mf & 0x10000)) and not (rear and (mf & 0x100000)):
        st['branch'] = 'tilt'
        raised = add(off[2], fl(inp['front_h']))
        height = sub(raised, mul(mi_front if front else mi_rear, 0.5))
        reversed_ = (front and bool(mf & 0x80000)) or (rear and bool(mf & 0x800000))
        arg = mul(mul(height, -1.0), sign)
        st['reversed'] = reversed_
        if arg > 1.0:
            clamped = 1.0
            st['clamp_hi'] = True
        elif arg < -1.0:
            clamped = -1.0
            st['clamp_lo'] = True
        else:
            clamped = arg
        if math.isnan(clamped):
            ang = fl(0xFFC00000)
            st['asin_nan'] = True
        else:
            ang = f32(math.asin(clamped))
        trace.append(['roty', {'approx': bits(-ang if reversed_ else ang)}])
    else:
        st['branch'] = 'none'
    final = [add(0.0, off[0]), add(0.0, off[1]), add(0.0, off[2])]
    frame[0], frame[1], frame[2] = bits(sx), 0, 0
    frame[4], frame[5], frame[6] = 0, bits(wheel_size), 0
    frame[8], frame[9], frame[10] = 0, 0, bits(wheel_size)
    frame[12], frame[13], frame[14] = bits(final[0]), bits(final[1]), bits(final[2])
    trace.append(['upd', 'L'])
    return result()


def expected(inp, fn):
    return (expected_s, expected_r, expected_w)[fn](inp)


def tally(fn, st, cov):
    """Cuenta de cobertura por funcion a partir de las estadisticas del oraculo (compartida con run_local.py)."""
    def bump(key, n=1):
        cov[key] = cov.get(key, 0) + n
    if fn == 0:
        bump('why=' + st['why'])
        bump('result_true' if st['result'] else 'result_false')
        bump('link_match', 1 if st.get('link_matches') else 0)
        bump('navi_area_null', 1 if st.get('navi_area_null') else 0)
    elif fn == 1:
        for k in ('cheat', 'rhino', 'nan_track', 'appear', 'beam_r', 'beam_l', 'time_wrap'):
            bump(k, 1 if st.get(k) else 0)
        bump('model=' + st['model_class'])
        bump('v_clamped', 1 if st.get('v_clamped') else 0)
        bump('wheel_nodes=%d' % st['wheel_nodes'] if st.get('cheat') else 'wheel_nodes=0')
    else:
        bump('ran' if st['ran'] else 'early:' + st.get('why', '?'))
        for k in ('steer_fixed', 'mirrored', 'scale_it', 'default_wheel', 'harvester', 'm513', 'mtruck_quad', 'wide_front', 'wide_rear', 'kart', 'heli', 'burst', 'reversed',
                  'clamp_hi', 'clamp_lo', 'asin_nan', 'water_effects', 'is_water', 'is_sand', 'frame_even'):
            bump(k, 1 if st.get(k) else 0)
        bump('branch=' + st['branch']) if st['ran'] else None
        for n in range(2, 8):
            bump('node%d' % n, 1 if st.get('node%d' % n) else 0)


# Cobertura minima por funcion (calibrada con dump_fixtures.exe + `--selftest`, con margen): ver README.md
COVERAGE_MIN = [
    {'why=status': 300, 'why=cur_null': 300, 'why=start_null': 300, 'why=end_ffff': 300, 'why=end_null': 300, 'why=part1': 300, 'why=part2': 250, 'why=part2_false': 300,
     'result_true': 600, 'result_false': 2500, 'link_match': 900},
    {'cheat': 1500, 'rhino': 900, 'appear': 1300, 'beam_r': 500, 'beam_l': 500, 'time_wrap': 1000, 'model=rhino': 900, 'model=bandit': 150, 'model=minus2': 150, 'model=other': 1500,
     'v_clamped': 100, 'nan_track': 15},
    {'ran': 3000, 'early:null_frame': 100, 'early:bad_node': 300, 'branch=solid': 400, 'branch=tilt': 400, 'branch=water': 400, 'branch=none': 200, 'burst': 500, 'heli': 250,
     'mirrored': 400, 'scale_it': 250, 'steer_fixed': 800, 'default_wheel': 800, 'harvester': 80, 'm513': 50, 'mtruck_quad': 400, 'wide_front': 200, 'wide_rear': 400, 'kart': 100,
     'reversed': 60, 'clamp_hi': 30, 'clamp_lo': 30, 'water_effects': 150, 'is_water': 50, 'is_sand': 50, 'frame_even': 25,
     'node2': 300, 'node3': 300, 'node4': 300, 'node5': 300, 'node6': 300, 'node7': 300},
]


def selftest(path):
    import json
    cov = [dict(), dict(), dict()]
    count = [0, 0, 0]
    for line in open(path, encoding='utf-8'):
        r = json.loads(line)
        exp, st = expected(r['input'], r['fn'])
        # el observado esperado debe ser serializable (JSON) y de formas coherentes
        json.dumps(exp)
        count[r['fn']] += 1
        tally(r['fn'], st, cov[r['fn']])
    assert count == [CASES] * 3, count
    short = {}
    for fn in range(3):
        for k, m in COVERAGE_MIN[fn].items():
            if cov[fn].get(k, 0) < m:
                short['%d:%s' % (fn, k)] = (cov[fn].get(k, 0), m)
    for fn in range(3):
        print(fn, NAMES[fn], dict(sorted(cov[fn].items())))
    if short:
        print('COBERTURA INSUFICIENTE', short)
        sys.exit(1)
    print('selftest ok')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--selftest':
        selftest(sys.argv[2])
        sys.exit(0)
    # autoprueba minima del modelo (sin juego)
    assert bits(add(0.0, -0.0)) == 0
    assert approx_equal(0x3F800000, 0x3F800002) and not approx_equal(0x3F800000, 0x3F800003)
    assert matches({'a': [1, 0x3F800000]}, {'a': [1, {'approx': 0x3F800001}]})
    assert runs(b'\x01\x02\x03\x04', b'\x01\xff\xff\x04', 0x10) == [[0x11, 'ffff']]
    print('ok')
