"""Oraculo independiente del banco Player82C: CPlayerPed::SetRealMoveAnim (EXE 0x60A9C0).

Modelo escrito desde el desensamblado (0x60A9C0..0x60B456), sin usar el codigo de la DLL: la secuencia de llamadas observadas a los colaboradores sustituidos (con sus argumentos), los campos que
escribe la funcion en las animaciones, el jugador, los datos del jugador y las globales, y los borrados (destructor virtual, ranura 0). Cada float guardado pasa por la precision de 24 bits de la
x87 (equivale a redondear a float); todo NaN observado se canonicaliza a 0x7FC00000.
`model(inp)` devuelve el resultado esperado; `check(inp, side, begin)` compara un lado de un par con el modelo y devuelve un mensaje de error o None.
Etiquetas de punteros (Tag en Core.hpp): 0x10000000 + (indice de arena << 20) + 0x40 + desplazamiento. Arenas: 0 jugador, 1 datos del jugador, 2..11 animaciones existentes
(walk, run, sprint, walkStart, idle, runStop, runStopR, strafeA, strafeB, tired), 12..15 animaciones nuevas, 16..18 jerarquias (runStop, sprint, walkStart), 19 otra jerarquia, 20 matriz,
21 inteligencia, 22 pad, 23 clump.
"""
import math
import struct

NAN = 0x7FC00000
M32 = 0xFFFFFFFF
WALK, RUN, SPRINT, WSTART, IDLE, RSTOP, RSTOPR, STRA, STRB, TIRED = range(10)
ID_OF = [0, 1, 2, 5, 3, 6, 7, 0x89, 0x8A, 0x0A]
ALL_BRANCHES = [
    'reset_walk', 'reset_skip_null',
    'rs_playing', 'rsr_playing', 'rs_start_play', 'rs_no_start',
    'fade_rs', 'fade_rsr', 'fade_idle_exists', 'fade_idle_created',
    'x_block', 'strafing', 'strafe_left_existing', 'strafe_left_new', 'strafe_right_existing', 'strafe_right_new', 'strafe_idle_deleted', 'strafe_idle_kept', 'x_idle_created', 'x_idle_exists',
    'tired_check', 'busy_fighting', 'busy_usegun', 'busy_duck', 'busy_throw', 'busy_jetpack', 'busy_sphere', 'tired_create_37', 'tired_create_default', 'tired_exists', 'tired_cancel', 'tired_cancel_skip',
    'walkstart_created', 'walkstart_reused', 'idle_branch_tired', 'idle_branch_sprint_deleted', 'runstop_deleted', 'runstopr_deleted', 'strafea_deleted', 'strafeb_deleted', 'walk_created', 'run_created',
    'walkstart_finished_notplaying', 'walkstart_finished_time', 'walkstart_kept', 'state7_walkstart',
    's_block', 's_amount0', 's_ratio_lt04_quot_lt05', 's_ratio_lt04_quot_ge05', 's_delta_ge0', 's_fade_ratio_lt1', 's_fade_ratio_ge1', 's_state_run', 's_state_walk',
    'ws_branch', 'sprint_none_state7', 'sprint_created', 'sprint_run_lt1', 'sprint_run_delta_fix', 'sprint_exists_delta_neg', 'sprint_exists_delta_pos',
    'ratio_lt1', 'ratio_lt2', 'ratio_ge2',
    'adrenalina', 'adrenalina_ended', 'adrenalina_running', 'sprint_cam15', 'sprint_result_min', 'sprint_result_value', 'no_sprint_end',
]


def f32(x):
    if x != x:
        return float('nan')
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(float('inf'), x)


def fb(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', f32(x)))[0]


def canon(b):
    b &= M32
    return NAN if (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) else b


def r24(x):
    """redondeo a 24 bits de mantisa conservando el exponente (la x87 con precision de 24 bits)"""
    if x == 0 or x != x or math.isinf(x):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)) / (1 << 24), e)


def fdiv(a, b):
    if b == 0:
        if a == 0 or a != a:
            return float('nan')
        neg = (math.copysign(1, a) < 0) != (math.copysign(1, b) < 0)
        return float('-inf') if neg else float('inf')
    return f32(a / b)


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 82103) & M32)


def fill_word(seed, byte_off):
    """palabra de relleno de FillWords en el desplazamiento de arena dado (multiplo de 4)"""
    return rand(seed & M32, byte_off // 4)


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + 0x40 + off


CLUMP = tag(23)
PED = tag(0)
PAD = tag(22)
INTEL = tag(21)


class Obj:
    def __init__(self, f, a, d, c, s, t):
        self.f = f & 0xFFFF
        self.a, self.d, self.c, self.s, self.t = a, d, c, s, t   # patrones de 32 bits


def model(inp):
    br = []
    ev = []
    mask = inp['mask']
    A = [None] * 14
    for i in range(10):
        if (mask >> i) & 1:
            A[i] = Obj(inp['flags'][i], inp['amount'][i], inp['delta'][i], inp['cur'][i], inp['speed'][i], inp['step'][i])
    # animaciones nuevas (existen siempre como arenas; los campos no escritos por el caso son el relleno de la arena)
    new = []
    for k in range(4):
        seed = (inp['seed'] * 31 + 30 + k) & M32
        new.append(Obj(inp['new_f%d' % k], inp['new_a%d' % k], inp['new_d%d' % k], fill_word(seed, 0x40 + 0x20), fill_word(seed, 0x40 + 0x24), fill_word(seed, 0x40 + 0x28)))
    # las arenas de las animaciones ausentes (mascara 0) tambien llevan los valores del caso (la funcion no las ve)
    ghosts = {}
    for i in range(10):
        if not (mask >> i) & 1:
            ghosts[i] = Obj(inp['flags'][i], inp['amount'][i], inp['delta'][i], inp['cur'][i], inp['speed'][i], inp['step'][i])
    ped = dict(move=inp['move_state'], head=inp['head_rate'], flags=inp['ped_flags'])
    pd = dict(ratio=inp['ratio'], can=inp['can_run'], flags=inp['pd_flags'], adv=inp['adv_end'])
    glob = dict(scale=inp['time_scale0'], cheat=inp['cheat'] & 0xFF, timer=inp['timer'])
    total = [fb(x) for x in inp['total']]
    state = dict(new=0, aim=0)

    def tg(o):
        for i in range(10):
            if A[i] is o:
                return tag(2 + i)
        for k in range(4):
            if new[k] is o:
                return tag(12 + k)
        raise RuntimeError('objeto sin etiqueta')

    def hier_total(o):
        if o is A[RSTOP]:
            return total[0]
        if o is A[SPRINT]:
            return total[1]
        if o is A[WSTART]:
            return total[2]
        return None   # hierarquia no modelada (no se lee)

    def E(*v):
        ev.append(list(v))

    def get(i):
        E(1, CLUMP, ID_OF[i])
        return A[i]

    def nxt():
        k = min(state['new'], 3)
        state['new'] += 1
        return k

    def blend(group, anim_id, delta):
        k = nxt()
        E(2, CLUMP, group, anim_id, canon(bits(delta)), k)
        return new[k]

    def add(group, anim_id):
        k = nxt()
        E(3, CLUMP, group, anim_id, k)
        return new[k]

    def settime(o):
        E(4, tg(o), 0)

    def restore():
        E(6, PED)

    def delete(o):
        E(20, tg(o), 1)
        o.deleted = True

    def ro(o, name):
        return fb(getattr(o, name))

    def wo(o, name, v):
        setattr(o, name, bits(v))

    grp = inp['group']
    walk = get(WALK)
    run = get(RUN)
    sprint = get(SPRINT)
    wstart = get(WSTART)
    idle = get(IDLE)
    rstop = get(RSTOP)
    rstopr = get(RSTOPR)
    sa = get(STRA)
    sb = get(STRB)
    tired = get(TIRED)
    ratio_of = lambda: fb(pd['ratio'])

    def finish():
        nonlocal sprint
        if pd['flags'] & 2:
            br.append('adrenalina')
            if inp['timer'] > pd['adv'] and not (glob['cheat'] & 0xFF):
                br.append('adrenalina_ended')
                pd['flags'] &= ~2 & M32
                glob['scale'] = 0x3F800000
                speed = 0x3F800000
            else:
                br.append('adrenalina_running')
                glob['scale'] = 0x3EAAAAAB
                speed = 0x40000000
            for o in (wstart, walk, run):
                if o:
                    o.s = speed
            if not sprint:
                br.append('no_sprint_end')
                return
            sprint.s = speed
        if not sprint:
            br.append('no_sprint_end')
            return
        cam = inp['camMode'][inp['cam_idx']] if 'camMode' in inp else inp['cam_mode'][inp['cam_idx']]
        if (cam & 0xFFFF) == 15:
            br.append('sprint_cam15')
            sprint.s = 0x3F333333
            return
        E(19, PED, 0)
        v = fb(inp['sprint_ret'])
        if 1.0 > v:
            br.append('sprint_result_min')
            sprint.s = 0x3F800000
        else:
            br.append('sprint_result_value')
            sprint.s = inp['sprint_ret']

    if ped['flags'] & 1:
        br.append('reset_walk')
        for o in (walk, run, sprint):
            if o:
                settime(o)
            else:
                br.append('reset_skip_null')
        ped['flags'] &= ~1 & M32

    def playing(o):
        return o is not None and (o.f & 1) != 0

    def ge0(x):
        return x >= 0.0

    if playing(rstop) or playing(rstopr):
        br.append('rs_playing' if playing(rstop) else 'rsr_playing')
        ped['move'] = 6
        if rstop and not (rstop.f & 1):
            if total[0] > ro(rstop, 'c'):
                br.append('rs_start_play')
                rstop.f |= 1
            else:
                br.append('rs_no_start')
        finish()
        return result(inp, A, new, ghosts, ped, pd, glob, br, ev)
    if (rstop and ge0(ro(rstop, 'd'))) or (rstopr and ge0(ro(rstopr, 'd'))):
        o = rstop if rstop else rstopr
        br.append('fade_rs' if rstop else 'fade_rsr')
        o.f |= 4
        o.a = 0x3F800000
        o.d = 0xC1000000
        restore()
        if not idle:
            br.append('fade_idle_created')
            idle = blend(grp, 3, 8.0)
        else:
            br.append('fade_idle_exists')
        idle.a = 0
        idle.d = 0x41000000
        finish()
        return result(inp, A, new, ghosts, ped, pd, glob, br, ev)

    if ratio_of() == 0.0 and not sprint:
        br.append('x_block')
        E(7, PED)
        c1 = False
        E(8, PAD)
        if inp['force_ret'] & 0xFF:
            E(7, PED)
            E(9, PAD, 0)
            if inp['aim_ret'][0] & 0xFFFF:
                state['aim'] = 1
                if (inp['cam_mode'][inp['cam_idx']] & 0xFFFF) == 4:
                    c1 = True
        if c1:
            br.append('strafing')
            E(7, PED)
            E(9, PAD, 0)
            v = inp['aim_ret'][1] & 0xFFFF
            v = v - 0x10000 if v & 0x8000 else v
            left = v < 0
            obj = A[STRA] if left else A[STRB]
            ok = obj is not None and not (ro(obj, 'd') < 0.0) and ((not (ro(obj, 'a') < 1.0)) or (not (ro(obj, 'd') <= 0.0)))
            if not ok:
                obj = blend(0, 0x89 if left else 0x8A, 16.0)
                br.append('strafe_left_new' if left else 'strafe_right_new')
            else:
                br.append('strafe_left_existing' if left else 'strafe_right_existing')
            E(7, PED)
            E(9, PAD, 0)
            m = inp['aim_ret'][2] & 0xFFFF
            m = m - 0x10000 if m & 0x8000 else m
            obj.s = bits(abs(float(m)) * 0.0078125)
            if idle:
                if ro(idle, 'a') <= 0.01:
                    br.append('strafe_idle_deleted')
                    delete(idle)
                else:
                    br.append('strafe_idle_kept')
        elif not idle:
            br.append('x_idle_created')
            blend(grp, 3, 4.0)
        else:
            br.append('x_idle_exists')
        notired = True
        if fb(pd['can']) < 0.0:
            br.append('tired_check')
            busy = False
            E(10, INTEL)
            if inp['task_ret'][0]:
                busy = True
                br.append('busy_fighting')
            if not busy:
                E(11, INTEL)
                if inp['task_ret'][1]:
                    busy = True
                    br.append('busy_usegun')
            if not busy:
                E(12, INTEL, 1)
                if inp['task_ret'][2]:
                    busy = True
                    br.append('busy_duck')
            if not busy:
                E(13, INTEL)
                if inp['task_ret'][3]:
                    busy = True
                    br.append('busy_throw')
            if not busy:
                E(14, INTEL)
                if inp['task_ret'][4]:
                    busy = True
                    br.append('busy_jetpack')
            if not busy:
                pos = inp['pm'] if inp['has_matrix'] else inp['pl']
                E(15, canon(pos[0]), canon(pos[1]), canon(pos[2]), 0x3F000000, 0, 1, 0, 0, 0, 0, 0)
                if inp['sphere_ret']:
                    busy = True
                    br.append('busy_sphere')
            notired = busy
        if not notired:
            if not tired:
                E(16)
                if inp['motion_ret'] == 0x37:
                    br.append('tired_create_37')
                    o = blend(0x4C, 0x0A, 4.0)
                else:
                    br.append('tired_create_default')
                    o = blend(0, 0x0A, 4.0)
                o.f |= 1
            else:
                br.append('tired_exists')
        elif tired and ro(tired, 'a') > 0.0 and ge0(ro(tired, 'd')):
            br.append('tired_cancel')
            tired.f &= 0xFFFE
            tired.d = 0xC0000000
        else:
            br.append('tired_cancel_skip')
        ped['move'] = 1
        finish()
        return result(inp, A, new, ghosts, ped, pd, glob, br, ev)

    # rama principal
    if idle:
        if not wstart:
            br.append('walkstart_created')
            wstart = add(grp, 5)
        else:
            br.append('walkstart_reused')
            wstart.a = 0x3F800000
            wstart.d = 0
        if walk:
            settime(walk)
        if run:
            settime(run)
        delete(idle)
        if tired:
            br.append('idle_branch_tired')
            tired.d = 0xC0800000
        if sprint:
            br.append('idle_branch_sprint_deleted')
            delete(sprint)
        sprint = None
        ped['move'] = 4
    if rstop:
        br.append('runstop_deleted')
        delete(rstop)
        restore()
    if rstopr:
        br.append('runstopr_deleted')
        delete(rstopr)
        restore()
    if sa:
        br.append('strafea_deleted')
        delete(sa)
    if sb:
        br.append('strafeb_deleted')
        delete(sb)
    if not walk:
        br.append('walk_created')
        walk = add(grp, 0)
        walk.a = 0
    if not run:
        br.append('run_created')
        run = add(grp, 1)
        run.a = 0
    if wstart:
        if not (wstart.f & 1):
            fin = 'walkstart_finished_notplaying'
            done = True
        else:
            done = total[2] <= r24(ro(wstart, 't') + ro(wstart, 'c'))
            fin = 'walkstart_finished_time'
        if done:
            br.append(fin)
            delete(wstart)
            walk.f |= 1
            run.f |= 1
            wstart = None
        else:
            br.append('walkstart_kept')
    if ped['move'] == 7 and wstart:
        br.append('state7_walkstart')
        ped['move'] = 1

    if sprint and not (ped['move'] == 7 and not (ratio_of() < 0.4)):
        br.append('s_block')
        if ro(sprint, 'a') != 0.0:
            if not (ro(sprint, 'd') < 0.0) or not (ro(sprint, 'a') < 0.8):
                if ratio_of() < 0.4:
                    q = fdiv(ro(sprint, 'c'), total[1])
                    br.append('s_ratio_lt04_quot_lt05' if q < 0.5 else 's_ratio_lt04_quot_ge05')
                    o = add(0, 6 if q < 0.5 else 7)
                    o.a = 0x3F800000
                    E(5, tg(o), 1, PED)
                    ped['head'] = 0
                    sprint.f |= 4
                    sprint.d = 0xC47A0000
                    walk.f &= 0xFFFE
                    run.f &= 0xFFFE
                    walk.a = run.a = 0
                    walk.d = run.d = 0
                elif ge0(ro(sprint, 'd')):
                    br.append('s_delta_ge0')
                    sprint.f |= 4
                    sprint.d = 0xBF800000
                    run.d = 0x3F800000
            elif ratio_of() < 1.0:
                br.append('s_fade_ratio_lt1')
                sprint.d = 0xC1000000
                run.d = 0x41000000
            else:
                br.append('s_fade_ratio_ge1')
        else:
            br.append('s_amount0')
            sprint.f |= 4
            sprint.d = 0xC47A0000
        if ratio_of() > 1.0:
            br.append('s_state_run')
            ped['move'] = 6
        else:
            br.append('s_state_walk')
            ped['move'] = 4
    elif wstart:
        br.append('ws_branch')
        walk.f &= 0xFFFE
        run.f &= 0xFFFE
        walk.a = 0
        run.a = 0
    elif ped['move'] == 7:
        br.append('sprint_none_state7')
        if not sprint:
            if not (ro(run, 'a') < 1.0):
                br.append('sprint_created')
                sprint = blend(grp, 2, 2.0)
                E(17)
            else:
                br.append('sprint_run_lt1')
                if ro(walk, 'a') == 0.0 and ro(run, 'a') == 0.0:
                    walk.a = 0x3F800000
                if ro(run, 'd') <= 0.0:
                    br.append('sprint_run_delta_fix')
                    run = blend(grp, 1, 4.0)
                pd['ratio'] = bits(ro(run, 'd') + 1.0)
        else:
            if ro(sprint, 'd') < 0.0:
                br.append('sprint_exists_delta_neg')
                sprint.d = 0x40000000
                run.d = 0xC0000000
            else:
                br.append('sprint_exists_delta_pos')
            E(17)
    elif ratio_of() < 1.0:
        br.append('ratio_lt1')
        walk.a = 0x3F800000
        run.a = 0
        walk.d = run.d = 0
        ped['move'] = 4
    elif ratio_of() < 2.0:
        br.append('ratio_lt2')
        r = ratio_of()
        walk.a = bits(2.0 - r)
        run.a = bits(r - 1.0)
        walk.d = run.d = 0
        ped['move'] = 6
    else:
        br.append('ratio_ge2')
        walk.a = 0
        run.a = 0x3F800000
        walk.d = run.d = 0
        ped['move'] = 6
        E(18)
    finish()
    return result(inp, A, new, ghosts, ped, pd, glob, br, ev)


def result(inp, A, new, ghosts, ped, pd, glob, br, ev):
    def js(o):
        return [o.f & 0xFFFF, canon(o.a), canon(o.d), canon(o.c), canon(o.s), canon(o.t)]
    assoc = []
    for i in range(10):
        assoc.append(js(A[i] if A[i] else ghosts[i]))
    for k in range(4):
        assoc.append(js(new[k]))
    return dict(branches=br, events=ev, ped=[ped['move'], canon(ped['head']), ped['flags']], pd=[canon(pd['ratio']), canon(pd['can']), pd['flags'], pd['adv']], assoc=assoc,
                globals=[canon(glob['scale']), glob['cheat'], glob['timer']])


def check(inp, side, begin):
    """compara un lado de un par con el modelo: None si coincide, o un mensaje"""
    m = model(inp)
    o = side['observed']
    if o['events'] != m['events']:
        for i, (a, b) in enumerate(zip(o['events'], m['events'])):
            if a != b:
                return 'evento %d: observado %s esperado %s' % (i, a, b)
        return 'eventos: %d observados, %d esperados' % (len(o['events']), len(m['events']))
    for key in ('ped', 'pd', 'globals'):
        if o[key] != m[key]:
            return '%s: observado %s esperado %s' % (key, o[key], m[key])
    if o['assoc'] != m['assoc']:
        for i, (a, b) in enumerate(zip(o['assoc'], m['assoc'])):
            if a != b:
                return 'animacion %d: observada %s esperada %s' % (i, a, b)
    if any(ch for ch in o['changes']):
        return 'bytes no modelados modificados: %s' % [c for c in o['changes'] if c][:2]
    return None
