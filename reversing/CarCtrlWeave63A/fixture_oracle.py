"""Oraculo independiente del banco CarCtrlWeave63A (lote 63, CCarCtrl parte B2, tanda A), escrito desde el desensamblado del EXE (no desde el C++ de la DLL).

Modela con aritmetica de float de 24 bits (el x87 del juego trabaja con la palabra de control a 24 bits: cada operacion redondea a float) las dos pruebas puras
  CCarCtrl::TestCollisionBetween2MovingRects                  0x425B30
  CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper  0x425F70
y devuelve para cada caso la salida esperada de cada lado (`observed`: ST0 de retorno como bits de float, sin eventos ni cambios: son funciones puras que no llaman a nadie) y las claves
de cobertura. Para SlowCarDownForOtherCar, WeaveForOtherCar y WeaveForObject la verdad es el EXE (el banco exige EXE == DLL bit a bit, con todos los colaboradores sustituidos por falsos
identicos que registran sus argumentos); de ellas el oraculo solo valida la coherencia de la entrada y calcula la cobertura a partir de la entrada (`stats_input`).
`python -I fixture_oracle.py` ejecuta la autoprueba (casos analiticos a mano).
"""
import ctypes
import math
import struct

CASES = 4096
NAMES = ['CCarCtrl::TestCollisionBetween2MovingRects', 'CCarCtrl::TestCollisionBetween2MovingRects_OnlyFrontBumper', 'CCarCtrl::SlowCarDownForOtherCar', 'CCarCtrl::WeaveForOtherCar',
         'CCarCtrl::WeaveForObject']
N_TC, N_FB, N_SLOW, N_WOC, N_WFO = NAMES
CASES_BY_NAME = {n: CASES for n in NAMES}
PURE = (N_TC, N_FB)


def bits(f):
    return struct.unpack('<I', struct.pack('<f', float(f)))[0]


def flt(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


class F32(float):
    """float de 24 bits: cada operacion se redondea a float (el doble tiene de sobra para que +, -, *, / y sqrt salgan bien redondeados)."""
    def __new__(cls, v=0.0):
        return float.__new__(cls, ctypes.c_float(float(v)).value)

    def __add__(self, o): return F32(float(self) + float(o))
    def __radd__(self, o): return F32(float(o) + float(self))
    def __sub__(self, o): return F32(float(self) - float(o))
    def __rsub__(self, o): return F32(float(o) - float(self))
    def __mul__(self, o): return F32(float(self) * float(o))
    def __rmul__(self, o): return F32(float(o) * float(self))
    def __neg__(self): return F32(-float(self))

    def __truediv__(self, o):
        a, b = float(self), float(o)
        try:
            return F32(a / b)
        except ZeroDivisionError:
            if a != a or a == 0.0:
                return F32(float('nan'))
            return F32(math.copysign(float('inf'), a) * math.copysign(1.0, b))

    def __rtruediv__(self, o):
        return F32(o) / self


def pos_of(obj):
    return [F32(flt(b)) for b in (obj['mpos'] if obj['mat'] else obj['pos'])]


def box_of(obj):
    return [F32(flt(b)) for b in obj['box']]


def v3(bits3):
    return [F32(flt(b)) for b in bits3]


def test_collision(row):
    """0x425B30: dos rectangulos en movimiento; devuelve el menor tiempo de choque (empieza en 1.0f) y las claves de cobertura."""
    o1, o2 = row['obj'][row['ia']], row['obj'][row['ib']]
    arg3, arg4 = F32(flt(row['a'][0])), F32(flt(row['a'][1]))
    A, B = v3(row['vecA']), v3(row['vecB'])
    p1, p2 = pos_of(o1), pos_of(o2)
    d_x, d_y = p1[0] - p2[0], p1[1] - p2[1]
    b2, b1 = box_of(o2), box_of(o1)
    half_x2, half_y2, neg_min_y2 = b2[3], b2[4], -b2[1]
    half_y1, half_x1 = b1[4], b1[3]
    move_across = arg3 * A[1] - arg4 * A[0]
    move_along = arg3 * A[0] + arg4 * A[1]
    best = F32(1.0)
    one = F32(1.0)
    st = {}

    def mark(k):
        st[k] = True

    for pas in range(2):
        if pas == 0:
            ca = (half_y1 * B[0] + half_x1 * B[1]) + d_x
            cb = (half_y1 * B[1] + d_y) - half_x1 * B[0]
        else:
            ca = (half_y1 * B[0] + d_x) - half_x1 * B[1]
            cb = (half_x1 * B[0] + half_y1 * B[1]) + d_y
        # primer eje
        enter1 = F32(0.0)
        rel1 = ca * A[1] - cb * A[0]
        if rel1 > half_x2:
            mark('ax1_hi')
            if move_across < 0.0:
                inv = one / move_across
                t = -((rel1 - half_x2) * inv)
                if t < 1.0:
                    enter1 = t
                    u = t - inv * (half_x2 + half_x2)
                    exit1 = u if u < 1.0 else F32(1.0)
                    mark('ax1_hi_hit')
                else:
                    enter1, exit1 = F32(1.0), F32(1.0)
                    mark('ax1_hi_late')
            else:
                enter1, exit1 = F32(1.0), F32(1.0)
                mark('ax1_hi_away')
        elif -half_x2 > rel1:
            mark('ax1_lo')
            if move_across > 0.0:
                inv = one / move_across
                t = -((rel1 + half_x2) * inv)
                if t < 1.0:
                    enter1 = t
                    u = inv * (half_x2 + half_x2) + t
                    exit1 = u if u < 1.0 else F32(1.0)
                    mark('ax1_lo_hit')
                else:
                    enter1, exit1 = F32(1.0), F32(1.0)
                    mark('ax1_lo_late')
            else:
                enter1, exit1 = F32(1.0), F32(1.0)
                mark('ax1_lo_away')
        else:
            mark('ax1_in')
            if move_across > 0.0:
                exit1 = (half_x2 - rel1) / move_across
            elif move_across < 0.0:
                exit1 = -((rel1 + half_x2) / move_across)
            else:
                exit1 = F32(1.0)
                mark('ax1_in_still')
        # segundo eje
        exit2 = F32(1.0)
        rel2 = cb * A[1] + ca * A[0]
        if rel2 > half_y2:
            mark('ax2_hi')
            if move_along < 0.0:
                inv = one / move_along
                v = -((rel2 - half_y2) * inv)
                if v < 1.0:
                    w = v - (neg_min_y2 + half_y2) * inv
                    enter2 = v
                    exit2 = w if w < 1.0 else F32(1.0)
                    mark('ax2_hi_hit')
                else:
                    enter2 = F32(1.0)
                    mark('ax2_hi_late')
            else:
                enter2 = F32(1.0)
                mark('ax2_hi_away')
        elif -neg_min_y2 > rel2:
            mark('ax2_lo')
            if move_along > 0.0:
                inv = one / move_along
                v = -((rel2 + neg_min_y2) * inv)
                if v < 1.0:
                    w = (neg_min_y2 + half_y2) * inv + v
                    enter2 = v
                    exit2 = w if w < 1.0 else F32(1.0)
                    mark('ax2_lo_hit')
                else:
                    enter2 = F32(1.0)
                    mark('ax2_lo_late')
            else:
                enter2 = F32(1.0)
                mark('ax2_lo_away')
        else:
            mark('ax2_in')
            enter2 = F32(0.0)
            if move_along > 0.0:
                exit2 = (half_y2 - rel2) / move_along
            elif move_along < 0.0:
                exit2 = -((rel2 + neg_min_y2) / move_along)
            else:
                mark('ax2_in_still')
        enter = enter1 if enter1 > enter2 else enter2
        if enter < exit1 and enter < exit2 and not (best < enter):
            best = enter
            mark('improved')
    if best < 1.0:
        mark('hit')
    if best != best:
        mark('nan')
    return best, st


def only_front_bumper(row):
    """0x425F70: las cuatro esquinas del vehiculo 1 contra el rectangulo del 2 (solo parachoques delantero)."""
    o1, o2 = row['obj'][row['ia']], row['obj'][row['ib']]
    arg3, arg4 = F32(flt(row['a'][0])), F32(flt(row['a'][1]))
    A, B = v3(row['vecA']), v3(row['vecB'])
    p1, p2 = pos_of(o1), pos_of(o2)
    x1, y1 = p1[0], p1[1]
    x2, y2 = p2[0], p2[1]
    b2, b1 = box_of(o2), box_of(o1)
    half_x2, half_y2 = b2[3], b2[4]
    half_y1, half_x1, neg_min_y1 = b1[4], b1[3], -b1[1]
    q1 = half_y2 * A[0]
    wx2ay = half_x2 * A[1]
    c1x = (q1 + wx2ay) + x2
    wy2ay = half_y2 * A[1]
    wx2ax = half_x2 * A[0]
    c1y = (wy2ay + y2) - wx2ax
    c2x = (q1 + x2) - wx2ay
    c2y = y2 + (wx2ax + wy2ay)
    best = F32(1.0)
    st = {}
    for corner in range(4):
        if corner == 0:
            u = (half_x1 * B[1] + half_y1 * B[0]) + x1
            v = (half_y1 * B[1] + y1) - half_x1 * B[0]
        elif corner == 1:
            u = (half_y1 * B[0] + x1) - half_x1 * B[1]
            v = (half_y1 * B[1] + half_x1 * B[0]) + y1
        elif corner == 2:
            u = (x1 - neg_min_y1 * B[0]) + half_x1 * B[1]
            v = (y1 - neg_min_y1 * B[1]) - half_x1 * B[0]
        else:
            u = (x1 - neg_min_y1 * B[0]) - half_x1 * B[1]
            v = (y1 - neg_min_y1 * B[1]) + half_x1 * B[0]
        moved_u, moved_v = u + arg3, v + arg4
        before = (v - c1y) * A[1] + (u - c1x) * A[0]
        after = (moved_v - c1y) * A[1] + (moved_u - c1x) * A[0]
        if not (before > 0.0):
            st['d1_fail'] = True
            continue
        if not (after < 0.0):
            st['d2_fail'] = True
            continue
        side_b = (c2x - u) * arg4 - (c2y - v) * arg3
        side_a = (c1x - u) * arg4 - (c1y - v) * arg3
        if not (side_b * side_a < 0.0):
            st['cross_fail'] = True
            continue
        t = before / (before - after)
        st['corner%d' % corner] = True
        if not (best < t):
            best = t
            st['improved'] = True
    if best < 1.0:
        st['hit'] = True
    return best, st


def expected(name, row, case):
    """(observed esperado de cada lado o None si la verdad es el EXE, claves de cobertura)"""
    if row['fn'] != NAMES.index(name):
        raise AssertionError('funcion distinta de la entrada')
    if name == N_TC:
        best, st = test_collision(row)
    elif name == N_FB:
        best, st = only_front_bumper(row)
    else:
        return None, stats_input(name, row)
    return {'events': [], 'nevents': 0, 'changes': [], 'ret': bits(best)}, st


def stats_input(name, row):
    """cobertura de las funciones no modeladas, a partir de la entrada (las claves observadas en los eventos las anade run_local.py)"""
    st = {}
    ia, ib = row['ia'], row['ib']
    a, b = row['obj'][ia], row['obj'][ib]
    if name == N_SLOW:
        st['car1_no_matrix'] = not a['mat']
        st['car1_before_car2'] = ia < ib
        st['car2_before_car1'] = ib < ia
        st['player_none'] = row['player'] == 0
        st['player_car1'] = row['player'] == ia + 1
    elif name == N_WOC:
        st['mission_ram_close'] = b['ap'][1] == 3
        st['mission_ramcar_close'] = b['ap'][1] == 16
        st['mission_follow_close'] = b['ap'][1] == 0x35
        st['mission_kill_ped_close'] = b['ap'][1] == 0x37
        st['entity_escort'] = a['ap'][1] in (0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20)
    elif name == N_WFO:
        st['model_special%d' % a['msel']] = a['msel'] < 5
        st['model_own'] = a['msel'] == 5
        st['model_sways'] = a['msel'] == 5 and (a['sways'] != 0)
        st['model_no_sways'] = a['msel'] == 5 and (a['sways'] == 0)
        st['no_matrix'] = not a['mat']
    return st


def selftest():
    def row(a3, a4, A, B, p1, p2, box1, box2):
        mk = lambda p, bx: {'mat': 1, 'mpos': [bits(v) for v in p], 'pos': [0, 0, 0], 'box': [bits(v) for v in bx]}
        return {'fn': 0, 'ia': 0, 'ib': 1, 'a': [bits(a3), bits(a4)], 'vecA': [bits(A[0]), bits(A[1]), 0], 'vecB': [bits(B[0]), bits(B[1]), 0], 'obj': [mk(p1, box1), mk(p2, box2)]}
    box = [-1.0, -2.0, -1.0, 1.0, 2.0, 1.0]
    # v1 a 8 m en +y de v2; A = (0,1) es el eje de v2 y B = (0,-1) el rumbo de v1 (sus esquinas delanteras miran a v2); velocidad relativa (0,-8)
    r = row(0.0, -8.0, (0.0, 1.0), (0.0, -1.0), (0.0, 8.0, 0.0), (0.0, 0.0, 0.0), box, box)
    r['fn'] = 0
    best, _ = test_collision(r)
    # los rectangulos se tocan cuando la distancia en y baja a half_y1 + half_y2 = 4: tiempo = (8 - 4) / 8 = 0.5
    assert abs(float(best) - 0.5) < 1e-6, best
    # sin movimiento relativo: no hay choque (1.0)
    r2 = row(0.0, 0.0, (0.0, 1.0), (0.0, -1.0), (0.0, 8.0, 0.0), (0.0, 0.0, 0.0), box, box)
    assert float(test_collision(r2)[0]) == 1.0
    # al alejarse: 1.0
    r3 = row(0.0, 8.0, (0.0, 1.0), (0.0, -1.0), (0.0, 8.0, 0.0), (0.0, 0.0, 0.0), box, box)
    assert float(test_collision(r3)[0]) == 1.0
    # parachoques delantero: dos coches enfrentados, rumbo +y para el 1 y -y para el 2
    r4 = row(0.0, -8.0, (0.0, -1.0), (0.0, 1.0), (0.0, 8.0, 0.0), (0.0, 0.0, 0.0), box, box)
    best4, _ = only_front_bumper(dict(r4, fn=1))
    assert 0.0 <= float(best4) <= 1.0
    print('autoprueba del oraculo: ok')


if __name__ == '__main__':
    selftest()


# ------------------------------------------------------------------------------------------------ WeaveForObject (parte determinista, para detectar bucles de angulo que no terminan)
WFO_OFFSETS = {0: (0x403D3F7D, 0x3E16872B), 1: (0x3F3E76C9, 0), 2: (0x3D3020C5, 0), 3: (0x3F924DD3, 0x3E147AE1), 4: (0, 0xBD449BA6)}


def wfo_geometry(row):
    """(margen, angulo) de WeaveForObject o None si la funcion sale antes (modelo propio sin SwaysInWind). Mismo orden de operaciones que el EXE (ver README)."""
    a, v = row['obj'][row['ia']], row['obj'][row['ib']]
    if a['msel'] < 5:
        side, fwd_off = (F32(flt(x)) for x in WFO_OFFSETS[a['msel']])
    else:
        if a['sways'] == 0:
            return None
        side, fwd_off = F32(0.0), F32(0.0)
    pre = [F32(flt(b)) for b in (a['mpos'] if a['mat'] else a['pos'])]
    right, fwd = v3(a['right']), v3(a['fwd'])
    obj_x = (fwd_off * fwd[0] + side * right[0]) + pre[0]
    obj_y = (side * right[1] + fwd_off * fwd[1]) + pre[1]
    vp = pos_of(v)
    dx, dy = obj_x - vp[0], obj_y - vp[1]
    vb = box_of(v)
    half_width = vb[3] * F32(flt(0x4019999A)) + F32(flt(0x3E99999A))
    dist = F32(math.sqrt(float(dy * dy + dx * dx)))
    ratio = half_width / dist
    return ratio * F32(0.5), (dx, dy)
