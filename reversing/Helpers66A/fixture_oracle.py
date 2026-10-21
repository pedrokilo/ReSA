"""Oraculo del banco Helpers66A: modelo en Python de las cinco funciones DESDE EL DESENSAMBLADO del EXE (no del C++), aplicado a la fila `input` de cada caso.

Devuelve lo que el banco observa en el lado ORIGINAL (EXE): la traza de eventos de los falsos (`events`), AL donde lo lee un llamador, y los cambios de las arenas que son
predecibles (prioridad guardada, bytes de la tarea del tiroteo desde coche). run_local.py lo compara con la fila del EXE (y, como original == candidato, con ambos lados).

Aritmetica x87 del juego: precision de 24 bits con exponente extendido (r24 sobre float de Python = doble, exacto para productos y casi siempre para sumas); donde la x87 usa
fpatan/acos (RsSavePresetView) el oraculo es aproximado (2 ulp de float): solo se compara con tolerancia. Las cargas utiles de los NaN se comparan como "cualquier NaN".

Autoprueba:  python -I fixture_oracle.py   (comprueba el propio modelo con casos escritos a mano a partir del desensamblado; imprime ok)
"""
import math
import struct

KGUARD = 0x40


def base(i):
    return 0x10000000 + (i << 20)


def f32(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def bits(x):
    """float de Python (doble) -> patron de 32 bits redondeando a float como un fstp dword (con exponente extendido previo)."""
    if x != x:
        return 0x7FC00000
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0x7F800000 if x > 0 else 0xFF800000


def r24(x):
    """Redondeo a 24 bits de mantisa (al par), exponente sin limite: x87 con precision de 24 bits."""
    if x != x or x in (math.inf, -math.inf) or x == 0.0:
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)) / (1 << 24), e)


def ulp32(x):
    """Tamano de la ultima posicion de un float alrededor de x (x finito)."""
    b = bits(abs(x))
    return abs(f32(b + 1) - f32(b)) if b < 0x7F7FFFFF else abs(f32(b) - f32(b - 1))


def mulss(a, b):
    """mulss con MXCSR.FTZ: producto a float; un resultado subnormal se vacia a cero con el signo del producto."""
    r = f32(bits(a * b)) if a == a and b == b else math.nan
    if r != 0.0 and r == r and abs(r) < 1.1754943508222875e-38:
        return math.copysign(0.0, r)
    return r


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    return b | 0x400000 if is_nan(b) else b


def s8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v >= 1 << 31 else v


def fnv(data):
    if data is None:
        return 0
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & 0xFFFFFFFF
    return h


def fnv_s(s):
    return fnv(s.encode('latin-1'))


def dbl_words(fbits):
    """double(float): patron de 64 bits como par (bajo, alto); un NaN se silencia (fld/cvtss2sd)."""
    sign = (fbits >> 31) & 1
    exp = (fbits >> 23) & 0xFF
    man = fbits & 0x7FFFFF
    if exp == 0xFF:
        d = (sign << 63) | (0x7FF << 52) | ((man | (0x400000 if man else 0)) << 29)
    elif exp == 0:
        if man == 0:
            d = sign << 63
        else:
            n = man.bit_length()
            e = 1023 - 126 - (23 - n + 1)
            d = (sign << 63) | (e << 52) | (((man << (53 - n)) & ((1 << 52) - 1)))
    else:
        d = (sign << 63) | ((exp - 127 + 1023) << 52) | (man << 29)
    return (d & 0xFFFFFFFF, d >> 32)


# ================================================================================================ 0: PlayerWantsToAttack (0x60CC50)
def f0(inp):
    ev = []
    g = inp['group']
    grp = base(5) + g * 0x2D4
    ped, entA, entB, entC = base(0) + KGUARD, base(2) + KGUARD, base(3) + KGUARD, base(4) + KGUARD
    ev.append([1, grp + 8])
    if s32(inp['count']) < 1:
        return dict(events=ev)
    if (inp['flag_byte'] & 0xFF) == 0:
        return dict(events=ev)
    ev.append([2, grp + 0x30])
    target = (entA, inp['t_type']) if inp['t_kind'] else None
    if (inp['cam_mouse'] & 0xFF) != 0 and target is None:
        target = (entB, inp['m_type']) if inp['m_kind'] else None
    victim = None
    if target:
        ptr, tb = target
        if (tb & 7) == 3:
            victim = ptr
        else:
            ev.append([3, ptr])
            if inp['is_tag_ret'] & 0xFF:
                return dict(events=ev)
            if (tb & 7) == 4:
                ev.append([4, ptr])
                if inp['can_ret'] & 0xFF:
                    return dict(events=ev)
    if victim is None:
        ev.append([5, ped])
        victim = 0 if inp['find_kind'] == 0 else (entC if inp['find_kind'] == 1 else inp['find_raw'])
        if victim == 0:
            return dict(events=ev)
    ev.append([6, grp, ped, victim])
    return dict(events=ev)


# ================================================================================================ 1: EvaluateNeighbouringTarget (0x60D1C0)
PI = f32(0x40490FDB)
TWO_PI = f32(0x40C90FDB)
LIMIT50 = f32(0x3F5F66F3)


def f1(inp):
    ev = []
    selfT, tgtT = base(0) + KGUARD, base(1) + KGUARD
    s = [f32(v) for v in inp['self']]
    t = [f32(v) for v in inp['tgt']]
    c = [f32(v) for v in inp['cam']]
    d = [r24(t[i] - s[i]) for i in range(3)]
    sq = r24(r24(r24(d[2] * d[2]) + r24(d[1] * d[1])) + r24(d[0] * d[0]))
    dist = r24(math.sqrt(sq)) if sq == sq and sq >= 0 else sq
    maxd = f32(inp['max_dist'])
    if not (dist <= maxd):
        return dict(events=ev, stored=False, prio_changes=[])
    weapon = (0x5A0 + s8(inp['slot']) * 0x1C) & 0xFFFFFFFF
    ev.append([11, selfT, tgtT, weapon])
    if inp['broken_ret'] & 0xFF:
        return dict(events=ev, stored=False, prio_changes=[])
    dx = bits(r24(t[0] - c[0]))
    dy = bits(r24(t[1] - c[1]))
    ev.append([12, dx, dy])
    angle = r24(f32(inp['atan_ret']) - f32(inp['comp']))
    if angle > PI:
        angle = r24(angle - TWO_PI)
        while angle > PI:
            angle = r24(angle - TWO_PI)
    if angle < -PI:
        angle = r24(angle + TWO_PI)
        while angle < -PI:
            angle = r24(angle + TWO_PI)
    absa = -angle if angle < 0 else angle
    if not (absa < LIMIT50):
        return dict(events=ev, stored=False, prio_changes=[])
    right = (inp['flag'] & 0xFF) != 0
    if right:
        prio = -angle if angle > 0 else -100000.0
    else:
        prio = angle if angle < 0 else -100000.0
    cur = f32(inp['out_prio'])
    stored = prio > cur
    changes = []
    if stored:
        new = bits(prio)
        old = inp['out_prio']
        changes = [[i, (new >> (8 * i)) & 0xFF] for i in range(4) if ((new >> (8 * i)) & 0xFF) != ((old >> (8 * i)) & 0xFF)]
    return dict(events=ev, stored=stored, prio_changes=changes)


# ================================================================================================ 2: HasClearLineOfSight (0x621B10)
def f2(inp):
    ev = []
    if inp['timer'] <= inp['los_time']:
        return dict(events=ev, al=1 if (inp['blocked'] & 0xFF) == 0 else 0, task_changes=None)
    blocked = 0
    ignore = 0
    if inp['t_kind']:
        pos = inp['pos']
        oz = pos[2]
        if is_nan(oz):
            oz = quiet(oz)
        else:
            oz = bits(r24(f32(oz) + 0.5))
        ignore = (base(4) + KGUARD) if inp['t_kind'] == 1 else inp['t_raw']
        ev.append([21, pos[0], quiet(pos[1]), oz] + list(inp['aim']) + [base(3) + KGUARD, 1, 0, 0, 0, 0, 1, 0, ignore])
        if (inp['los_ret'] & 0xFF) == 0:
            blocked = 1
    ev.append([22])
    r = inp['rand_ret'] & 0xFFFF
    jitter = int(r * (1.0 / 32768.0) * -500.0)   # trunc hacia cero: _ftol2
    new_time = (inp['timer'] - jitter + 1750) & 0xFFFFFFFF
    old = inp['los_time']
    changes = [[0x18 + i, (new_time >> (8 * i)) & 0xFF] for i in range(4) if ((new_time >> (8 * i)) & 0xFF) != ((old >> (8 * i)) & 0xFF)]
    if (inp['blocked'] & 0xFF) != blocked:
        changes.append([0x1C, blocked])
    return dict(events=ev, al=1 if blocked == 0 else 0, task_changes=changes)


# ================================================================================================ 3: AddPunchImpact (0x49F670)
def f3(inp):
    c = [f32(v) for v in inp['cam']]
    p = [f32(v) for v in inp['pos']]
    d = [r24(c[i] - p[i]) for i in range(3)]
    s = r24(r24(r24(d[0] * d[0]) + r24(d[2] * d[2])) + r24(d[1] * d[1]))
    if s > 625.0:
        return dict(events=[])
    smoke = base(3) + KGUARD
    mult = [0x3F800000, 0x3F800000, 0x3F800000, 0x3ECCCCCD, 0x3DCCCCCD, 0, 0x3DCCCCCD]
    ev = []
    for tsince in (0, 0x3D4CCCCD):
        ev.append([31, smoke] + list(inp['pos']) + list(inp['vel']) + [tsince] + mult + [0xBF800000, 0x3F99999A, 0x3F19999A, 0])
    return dict(events=ev)


# ================================================================================================ 4: RsSavePresetView (0x619FA0)
FMT = "%0.6f %0.6f %0.6f  %0.6f %0.6f  %0.6f %0.6f %s%d\n"
X87_ACOS_DEG = f32(0x42652EE0)


def acos_deg(s):
    if s != s:
        return math.nan
    return r24(r24(math.acos(s)) * X87_ACOS_DEG)


def f4(inp):
    ev = []
    cam = base(0) + KGUARD
    ev.append([41, fnv_s('./views.txt')])
    path_null = inp['create_kind'] == 1
    ev.append([43, 0 if path_null else 1, 0 if path_null else fnv_s('views.txt'), fnv_s('a')])
    file = 0 if inp['fopen_kind'] == 0 else (0x55 if inp['fopen_kind'] == 1 else 0x7FFFE000 + (inp['seed'] & 0xFFF0))
    ev.append([42, 0 if path_null else 1])
    if file == 0:
        if inp['kb_used']:
            ev.append([53, 0x1D, 0x420])
        ev.append([52, fnv_s('Cannot open preset view file')])
        return dict(events=ev, al=0, approx=False)
    frame = (0x7FFF0000 | (inp['seed'] & 0xFFFF)) if inp['frame_kind'] else base(2) + KGUARD
    ev.append([50, frame])
    at = [f32(v) for v in inp['at']]
    # d = (0 * at.x + 1 * at.y) + 0 * at.z
    p0 = r24(0.0 * at[0])
    p1 = r24(1.0 * at[1])
    p2 = r24(0.0 * at[2])
    dotY = r24(r24(p0 + p1) + p2)
    stored = f32(bits(dotY))
    if dotY > 1.0:
        stored = 1.0
    elif stored < -1.0:
        stored = -1.0
    s = stored
    rot_x = bits(r24(90.0 - acos_deg(s))) if s == s else 0x7FC00000
    prod = [f32(bits(r24(y * s))) for y in (0.0, 1.0, 0.0)]
    t = [f32(bits(r24(at[i] - prod[i]))) for i in range(3)]
    ev.append([51] + [bits(v) for v in t])
    k = f32(inp['norm_scale'])
    tn = [mulss(v, k) for v in t]   # el falso de RwV3dNormalize multiplica por norm_scale con SSE en float; el juego corre con MXCSR FTZ (los resultados subnormales pasan a cero)
    # (0 * tn.y + 0 * tn.x) + 1 * tn.z
    dotZ = r24(r24(r24(0.0 * tn[1]) + r24(0.0 * tn[0])) + r24(1.0 * tn[2]))
    if dotZ > 1.0:
        dotZ = 1.0
    elif dotZ < -1.0:
        dotZ = -1.0
    yaw = acos_deg(dotZ) if dotZ == dotZ else math.nan
    dotX = r24(r24(r24(0.0 * tn[1]) + r24(1.0 * tn[0])) + r24(0.0 * tn[2]))
    rot_y = bits(-yaw if dotX < 0 else yaw)
    name = fnv_s('View')
    # orden del EXE: x, y, z, rotX, rotY, near, far
    words = []
    for fb in list(inp['pos']) + [rot_x, rot_y, inp['near'], inp['far']]:
        words += list(dbl_words(fb))
    ev.append([46, fnv_s(FMT)] + words + [name, inp['num_before']])
    h = words[0] ^ words[1] ^ words[2] ^ words[3] ^ words[4] ^ words[5] ^ words[10] ^ words[11] ^ words[12] ^ words[13]
    text = 'line-%08x-%d' % (h & 0xFFFFFFFF, s32(inp['num_before']))
    ev.append([47, fnv_s(text)])
    ev.append([45, fnv_s(text), len(text), file])
    put_ok = s32(inp['fputs_ret']) > 0
    if not put_ok:
        if inp['kb_used']:
            ev.append([53, 0x1D, 0x420])
        ev.append([52, fnv_s('Cannot write to preset view file')])
        ev.append([44, file])
        return dict(events=ev, al=0, approx=True)
    ev.append([48, inp['num_before']])
    al = inp['load_ret'] & 0xFF
    if al:
        ev.append([49, cam, (inp['num_after'] - 1) & 0xFFFFFFFF])
    ev.append([44, file])
    return dict(events=ev, al=1 if al else 0, approx=True)


EXPECTED = {
    'CPlayerPed::PlayerWantsToAttack': f0,
    'CPlayerPed::EvaluateNeighbouringTarget': f1,
    'CTaskSimpleGangDriveBy::HasClearLineOfSight': f2,
    'Fx_c::AddPunchImpact': f3,
    'RsSavePresetView': f4,
}


def expected(function, inp):
    return EXPECTED[function](inp)


# ================================================================================================ comparacion con lo observado
def _same_words(kind, i, a, e):
    if a == e:
        return True
    # cualquier NaN vale por cualquier NaN (la x87 elige la carga util)
    if kind in (12, 51) and is_nan(a) and is_nan(e):
        return True
    return False


def events_match(function, actual, exp):
    if len(actual) != len(exp):
        return 'numero de eventos %d != %d' % (len(actual), len(exp))
    for n, (a, e) in enumerate(zip(actual, exp)):
        if a[0] != e[0] or len(a) != len(e):
            return 'evento %d: %s != %s' % (n, a, e)
        if a[0] == 46:
            if a[1] != e[1] or a[-2:] != e[-2:]:
                return 'evento %d (sprintf): %s != %s' % (n, a, e)
            for i in range(7):
                aw, ew = a[2 + 2 * i:4 + 2 * i], e[2 + 2 * i:4 + 2 * i]
                da = struct.unpack('<d', struct.pack('<II', *aw))[0]
                de = struct.unpack('<d', struct.pack('<II', *ew))[0]
                if aw == ew or (da != da and de != de):
                    continue
                # rotX = 90 - acos * 57.29578 (la resta casi cancela: el error de 1 ulp del acos aparece como 1 ulp de 90) y rotY = acos * 57.29578: acos no se modela al ulp
                if i in (3, 4) and da == da and de == de and abs(da - de) <= 4 * ulp32(90.0 if i == 3 else max(abs(da), abs(de))):
                    continue
                return 'evento %d (sprintf) palabra %d: %s != %s' % (n, i, aw, ew)
            continue
        for i in range(1, len(a)):
            if not _same_words(a[0], i, a[i], e[i]):
                return 'evento %d palabra %d: %s != %s' % (n, i, a, e)
    return None


def check(function, row):
    """Compara el lado original del banco con el modelo; devuelve None si coincide o un texto con la diferencia."""
    inp = row['input']
    exp = expected(function, inp)
    obs = row['original']['observed']
    # el texto de la linea depende de las palabras de rotX/rotY solo a traves de las que el oraculo calcula exactamente (x, y, z, near, far)
    msg = events_match(function, obs['events'], exp['events'])
    if msg:
        return msg
    if 'al' in exp and obs.get('al') != exp['al']:
        return 'AL %s != %s' % (obs.get('al'), exp['al'])
    if function == 'CPlayerPed::EvaluateNeighbouringTarget':
        ch = obs['changes']
        if bool(ch[5]) != exp['stored']:
            return 'almacenado %s != %s' % (bool(ch[5]), exp['stored'])
        if ch[6] != exp['prio_changes']:
            return 'prioridad %s != %s' % (ch[6], exp['prio_changes'])
    if function == 'CTaskSimpleGangDriveBy::HasClearLineOfSight' and exp['task_changes'] is not None and obs['changes'][0] != exp['task_changes']:
        return 'tarea %s != %s' % (obs['changes'][0], exp['task_changes'])
    if function == 'RsSavePresetView' and any(obs['changes']):
        return 'cambios en arenas: %s' % obs['changes']
    return None


# ================================================================================================ autoprueba
def _selftest():
    # F0: sin miembros -> solo el evento 1
    base0 = dict(group=3, flag_byte=1, cam_mouse=0, count=0, t_kind=1, t_type=0x23, m_kind=0, m_type=0, is_tag_ret=0, can_ret=0, find_kind=1, find_raw=0x12345, seed=0)
    assert f0(base0)['events'] == [[1, base(5) + 3 * 0x2D4 + 8]]
    ok = dict(base0, count=2)
    assert [e[0] for e in f0(ok)['events']] == [1, 2, 6]                                  # ped (tipo 3): ataque directo
    assert [e[0] for e in f0(dict(ok, t_type=0x24, can_ret=0))['events']] == [1, 2, 3, 4, 5, 6]   # objeto no apuntable: busca ped
    assert [e[0] for e in f0(dict(ok, t_type=0x24, can_ret=0x81))['events']] == [1, 2, 3, 4]       # objeto apuntable: sale
    assert [e[0] for e in f0(dict(ok, t_type=0x22, is_tag_ret=2))['events']] == [1, 2, 3]            # etiqueta: sale
    assert [e[0] for e in f0(dict(ok, t_kind=0, cam_mouse=1, m_kind=1, m_type=0x03))['events']] == [1, 2, 6]
    assert [e[0] for e in f0(dict(ok, t_kind=0, cam_mouse=0, m_kind=1, m_type=0x03))['events']] == [1, 2, 5, 6]
    # F1: (3,4,0) a distancia 5; maxDist 5.0 entra; 4.9999995 no
    p = dict(self=[0, 0, 0], tgt=[0x40400000, 0x40800000, 0], cam=[0, 0, 0], max_dist=0x40A00000, comp=0, flag=1, out_prio=0xFF800000, atan_ret=bits(0.5), broken_ret=0, slot=0)
    r = f1(p)
    assert [e[0] for e in r['events']] == [11, 12] and r['stored'] and r['prio_changes'] == [[i, (bits(-0.5) >> (8 * i)) & 0xFF] for i in range(4) if ((bits(-0.5) >> (8 * i)) & 0xFF) != (0xFF800000 >> (8 * i)) & 0xFF]
    assert f1(dict(p, max_dist=0x40A00000 - 1))['events'] == []
    assert not f1(dict(p, flag=0, out_prio=0x80000000))['stored']                      # izquierda con angulo > 0: prioridad -100000, no supera a -0.0
    # F2: la comparacion es SIN signo
    q = dict(timer=0x80000000, los_time=0x7FFFFFFF, blocked=0, t_kind=0, t_raw=0, los_ret=0, rand_ret=0, pos=[0, 0, 0], aim=[0, 0, 0], ignore_init=0)
    assert [e[0] for e in f2(q)['events']] == [22]
    assert f2(dict(q, timer=5, los_time=5))['events'] == []
    # F3: (25, 0, 0) -> 625 exactamente entra; 625 + ulp no
    cam = [0, 0, 0]
    assert len(f3(dict(cam=cam, pos=[bits(25.0), 0, 0], vel=[1, 2, 3]))['events']) == 2
    assert f3(dict(cam=cam, pos=[bits(25.000002), 0, 0], vel=[1, 2, 3]))['events'] == []
    # F4: NaN y dbl_words
    assert dbl_words(0x3F800000) == (0, 0x3FF00000)
    assert dbl_words(0x7FA00000)[1] >> 19 & 1 == 1                                       # silenciado
    assert dbl_words(0x00000001) == (0, 0x36A00000)
    # r24
    assert r24(1.0 + 2.0 ** -24) == 1.0 and r24(1.0 + 3 * 2.0 ** -25) == 1.0 + 2.0 ** -23
    print('ok')


if __name__ == '__main__':
    _selftest()
