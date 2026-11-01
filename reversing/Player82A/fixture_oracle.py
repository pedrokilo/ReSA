"""Oraculo independiente del banco Player82A: CPlayerPed::DrawTriangleForMouseRecruitPed (EXE 0x60BA80).

Modelo escrito desde el desensamblado (0x60BA80..0x60C0B8), sin usar el codigo de la DLL: aritmetica de float con un redondeo a 32 bits por operacion (la x87 a 24 bits), la secuencia de
llamadas observadas (estados de render, RwIm3DTransform con los tres vertices, RwIm3DRenderIndexedPrimitive, RwIm3DEnd) y la escritura en el bufer temporal (solo posicion y color de los tres
vertices y tres indices). Todo NaN se canonicaliza a 0x7FC00000 (la x87 y SSE propagan cargas utiles distintas).
`model(inp)` devuelve el resultado esperado; `check(inp, side, begin)` compara un lado de un par con el modelo y devuelve un mensaje de error o None.
"""
import math
import struct

NAN = 0x7FC00000


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0] if not isinstance(x, int) else x


def f32(x):
    """redondea un double a float (con desbordamiento a infinito)"""
    if x != x:
        return float('nan')
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(float('inf'), x)


def fb(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def canon(x):
    """float -> patron de 32 bits con NaN canonico"""
    if x != x:
        return NAN
    return struct.unpack('<I', struct.pack('<f', f32(x)))[0]


def add(a, b):
    return f32(a + b)


def sub(a, b):
    return f32(a - b)


def mul(a, b):
    if (a == 0 or b == 0) and (math.isinf(a) or math.isinf(b)):
        return float('nan')
    return f32(a * b)


def div(a, b):
    if b == 0:
        if a == 0 or a != a:
            return float('nan')
        sign = math.copysign(1.0, a) * math.copysign(1.0, b)
        return math.copysign(float('inf'), sign)
    if math.isinf(a) and math.isinf(b):
        return float('nan')
    return f32(a / b)


def sqrt(a):
    if a != a or a < 0:
        return float('nan')
    return f32(math.sqrt(a))


def sin(a):
    return float('nan') if (a != a or math.isinf(a)) else f32(math.sin(a))


def cos(a):
    return float('nan') if (a != a or math.isinf(a)) else f32(math.cos(a))


def ftol8(x):
    """_ftol2 y AL: truncamiento a entero, byte bajo (los valores solo llegan aqui en 0..255)"""
    return int(x) & 0xFF


def r24(x):
    """redondeo a 24 bits de mantisa (al par) con exponente ampliado: un valor en la pila x87 con la precision a 24 bits"""
    if x != x or x in (0.0, float('inf'), float('-inf')):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0) / 16777216.0, e)


def len2_ext(d):
    x2, y2, z2 = r24(d[0] * d[0]), r24(d[1] * d[1]), r24(d[2] * d[2])
    return r24(r24(x2 + y2) + z2)


def normalise(d):
    """0x59C910: longitud al cuadrado (x*x + y*y) + z*z en la pila x87; <= 0 (ordenado) -> solo x = 1.0; si no k = 1 / sqrt, componente = float(c * k)"""
    l2 = len2_ext(d)
    if l2 <= 0.0:
        return [1.0, d[1], d[2]]
    root = r24(math.sqrt(l2)) if l2 == l2 and l2 != float('inf') else l2
    k = r24(1.0 / root) if root == root and root != 0 else (float('nan') if root != root else float('inf'))
    return [mul_ext(c, k) for c in d]


def mul_ext(c, k):
    if (c == 0 and math.isinf(k)) or (k == 0 and math.isinf(c)):
        return float('nan')
    return f32(r24(c * k))


def positions(inp):
    pos = lambda has, m, pl: [fb(inp['%s%d' % (m if has else pl, i)]) for i in range(3)]
    player = pos(inp['p_has_m'], 'p_m', 'p_pl')
    target = pos(inp['t_has_m'], 't_m', 't_pl')
    cam = pos(inp['c_has_m'], 'c_m', 'c_pl')
    return player, target, cam


def model(inp):
    if not inp['has_target']:
        return None
    player, target, cam = positions(inp)
    health, maxh = fb(inp['health']), fb(inp['max_health'])
    if maxh == 0:
        ratio = float('nan') if (health == 0 or health != health) else math.copysign(float('inf'), math.copysign(1.0, health) * math.copysign(1.0, maxh))
    elif math.isinf(health) and math.isinf(maxh):
        ratio = float('nan')
    else:
        ratio = r24(health / maxh)
    r = g = b = 0
    cls = 'mid'
    if ratio > 1.0:
        ratio = 1.0
        cls = 'gt1'
    if ratio > 0.0:
        t = r24(1.0 - ratio)
        r = ftol8(r24(r24(t * 255.0) + r24(ratio * 0.0)))
        g = ftol8(r24(r24(ratio * 255.0) + r24(t * 0.0)))
        b = ftol8(r24(r24(t * 0.0) + r24(ratio * 0.0)))
    else:
        cls = 'black'
    dx, dy, dz = sub(target[0], player[0]), sub(target[1], player[1]), sub(target[2], player[2])
    dist = sqrt(add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx)))
    size = sub(dist, 10.0)
    if not (size > 0.0):
        size = 0.0
    size = mul(size, f32(0.02))
    if 1.0 < size:
        size = 1.0
    scale = add(mul(size, f32(0.825)), f32(0.175))
    if inp['c_has_m']:
        right = [fb(inp['c_right%d' % i]) for i in range(3)]
    else:
        h = fb(inp['c_heading'])
        right = [cos(h), sin(h), 0.0]
    rx, ry, rz = mul(right[0], scale), mul(right[1], scale), mul(right[2], scale)
    zero = mul(0.0, scale)
    px, py, pz = target
    q = add(pz, 1.0)
    gang = inp['ped_type'] == 7
    if gang:
        v = [[px, py, q],
             [add(sub(px, rx), zero), add(zero, sub(py, ry)), add(sub(q, rz), scale)],
             [add(add(px, rx), zero), add(zero, add(py, ry)), add(scale, add(q, rz))]]
    else:
        v = [[add(px, zero), add(zero, py), add(q, scale)],
             [add(px, rx), add(ry, py), add(q, rz)],
             [sub(px, rx), sub(py, ry), sub(q, rz)]]
    out = []
    zero_len = 0
    for vert in v:
        d = [sub(cam[0], vert[0]), sub(cam[1], vert[1]), sub(cam[2], vert[2])]
        zero_len += len2_ext(d) <= 0.0
        d = normalise(d)
        out.append([add(d[0], vert[0]), add(d[1], vert[1]), add(d[2], vert[2])])
    return dict(color=(r, g, b), verts=out, ratio_class=cls, zero_len=zero_len, gang=gang, size=size, dist=dist)


STATES_BEGIN = [(8, 0), (6, 1), (10, 5), (11, 6), (12, 1), (1, 0), (29, 8)]
STATES_END = [(1, 0), (8, 1), (6, 1), (10, 5), (11, 6), (12, 0), (29, 5)]


def check(inp, side, begin):
    obs = side['observed']
    if any(obs['changes']):
        return 'el peaton, el objetivo o las matrices cambiaron'
    ev = obs['events']
    m = model(inp)
    if m is None:
        if ev:
            return 'sin objetivo debia no llamar a nada: %s' % ev
        if obs['vbuf0'] != obs['vbuf1'] or obs['ibuf0'] != obs['ibuf1']:
            return 'sin objetivo el bufer temporal no debia cambiar'
        return None
    exp = [[1, s, v] for s, v in STATES_BEGIN]
    r, g, b = m['color']
    col0 = 0xFF000000 | (r << 16) | (g << 8) | b
    col1 = (r << 16) | (g << 8) | b
    tr = [0xC4D958, 3, 0, 0x18]
    for i, v in enumerate(m['verts']):
        tr += [canon(v[0]), canon(v[1]), canon(v[2]), col0 if i == 0 else col1]
    tr += [0, 1, 2]
    exp.append([2] + tr)
    if inp['xform'] != 0:
        exp.append([3, 3, 0xC4B958, 3])
        exp.append([4])
    exp += [[1, s, v] for s, v in STATES_END]
    if ev != exp:
        for i, (a, e) in enumerate(zip(ev, exp)):
            if a != e:
                return 'evento %d: obtenido %s esperado %s' % (i, a, e)
        return 'numero de eventos distinto: %d frente a %d' % (len(ev), len(exp))
    v0, v1 = obs['vbuf0'], obs['vbuf1']
    want = list(v0)
    for i, v in enumerate(m['verts']):
        want[9 * i + 0], want[9 * i + 1], want[9 * i + 2] = canon(v[0]), canon(v[1]), canon(v[2])
        want[9 * i + 6] = col0 if i == 0 else col1
    if v1 != want:
        return 'bufer de vertices: solo debian cambiar posicion y color'
    i0, i1 = obs['ibuf0'], obs['ibuf1']
    w1 = i0[1]
    # indices (palabras de 16 bits): 0, 1, 2 en los tres primeros; el cuarto, el alto del segundo dword, no cambia
    want_i = [0 | (1 << 16), (2) | (w1 & 0xFFFF0000)]
    if i1 != want_i:
        return 'bufer de indices %s esperado %s' % (i1, want_i)
    return None
