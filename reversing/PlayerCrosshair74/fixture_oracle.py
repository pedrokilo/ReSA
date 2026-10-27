"""Oraculo independiente (Python) de PlayerCrosshair74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x56EC80..0x56EF85, lo que debe observarse
(traza de colaboradores, la mira recortada, las banderas del ped y los vectores de FireInstantHit) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio redondeado a 24 bits y los
guardados `fstp dword` redondeados al float (con desnormales). fptan se calcula con math.tan (doble precision) y se consume en la misma operacion. Los NaN solo se comparan como «es NaN»
(la carga la valida el microtest nan_rules_hw contra el hardware); todo lo demas, bit a bit.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 CPlayerInfo, 1 CPad, 2 CPed, 3 matriz.
"""
import json
import math
import sys
from fractions import Fraction

sys.path.insert(0, __import__('os').path.dirname(__file__))
from fixture_x87 import NAN, load, mul, add, sub, div, store, fchs, lt0, rnd, mk  # noqa: E402

GUARD = 0x40
CROSS = 0x158
M32 = 0xFFFFFFFF


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + GUARD + off


INFO, PAD, PED, MAT = (tag(i) for i in range(4))
CAM = 0xB6F028
FOVK = load(0x3C0EFA35)
K200 = load(0x43480000)
KSTEP = load(0x39AEC33E)
LIM = load(0x3F666666)   # 0.9f
NLIM = load(0xBF666666)


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def fint(i):
    return mk(Fraction(i))


def reload(bits):
    return NAN if bits is None else load(bits)


def tanv(a):
    """fptan: devuelve tan(a) (un NaN si a es NaN o infinito); a cero conserva el signo"""
    if a == NAN or a[0] == 'i':
        return NAN
    if a[1] == 0:
        return a
    return mk(Fraction(math.tan(float(a[1]))))


def mul_tan(x, ang):
    t = tanv(ang)
    return mul(x, t)


def step(cur_bits, steer, ts_bits, subtract):
    prod = mul(mul(fint(steer), load(ts_bits)), KSTEP)
    cur = load(cur_bits)
    r = add(fchs(prod), cur) if subtract else add(prod, cur)
    return store(r)


def clamp(bits):
    """recorte a +-0.9f con comparaciones ordenadas; bits None = NaN"""
    if bits is None:
        return None
    v = load(bits)
    if v != NAN:
        if v[0] == 'i':
            return 0x3F666666 if v[1] == 0 else 0xBF666666
        if v[1] > LIM[1]:
            return 0x3F666666
        if v[1] < NLIM[1]:
            return 0xBF666666
    return bits


def aim(c, x_bits, y_bits):
    """(muzzle[3], target[3]) en bits (None = NaN) segun 0x56ED85..0x56EEDC"""
    fov1, fov2 = load(c['fov1']), load(c['fov2'])
    right = [load(c['rx']), load(c['ry']), load(c['rz'])]
    fwd = [load(c['fx']), load(c['fy']), load(c['fz'])]
    up = [load(c['ux']), load(c['uy']), load(c['uz'])]
    pos = [load(c['px']), load(c['py']), load(c['pz'])]
    ang1 = mul(fov1, FOVK)
    u = div(tanv(ang1), load(c['asp']))
    v = mul(u, reload(y_bits))
    s = [store(mul(up[i], v)) for i in range(3)]
    ang2 = mul(fov2, FOVK)
    x = reload(x_bits)
    rx = store(mul(right[0], x))
    ry = store(mul(right[1], x))
    w = mul(x, right[2])
    p0 = store(mul_tan(reload(rx), ang2))
    p1 = store(mul_tan(reload(ry), ang2))
    p2 = mul_tan(w, ang2)
    f0 = sub(fwd[0], reload(p0))
    f1 = store(sub(fwd[1], reload(p1)))
    f2 = store(sub(fwd[2], p2))
    g0 = store(sub(f0, reload(s[0])))
    g1 = store(sub(reload(f1), reload(s[1])))
    g2 = store(sub(reload(f2), reload(s[2])))
    sc0 = mul(reload(g0), K200)
    sc1 = mul(reload(g1), K200)
    sc2 = store(mul(reload(g2), K200))
    t = [store(add(sc0, pos[0])), store(add(pos[1], sc1)), store(add(pos[2], reload(sc2)))]
    return s, t


def expected(c):
    """(eventos, bytes cambiados de la arena CPlayerInfo, mira final (tx, ty), banderas finales del ped, disparado)"""
    ev = []
    if (c['active'] & 0xFF) == 0:
        return ev, None, None, None, False
    ev.append([1, PAD])
    nx = clamp(step(c['tx'], s16(c['lr']), c['ts'], False))
    ev.append([2, PAD])
    ny = clamp(step(c['ty'], s16(c['ud']), c['ts'], (c['inv'] & 0xFF) == 0))
    ev.append([3, PAD])
    flags_final = None
    fired = s16(c['gun']) != 0
    aimv = None
    if fired:
        ev += [[4, CAM, 0x3E4CCCCD], [5, CAM], [5, CAM]]
        aimv = aim(c, nx, ny)
        saved = c['flags']
        f1 = saved & 0xFDFFFFFF
        origin = [c['cpos'][i] for i in range(3)] if c['has_cam_mat'] else [c['ppos'][i] for i in range(3)]
        ev.append(('fire', f1, origin, aimv))
        f2 = f1 ^ c['fire_xor']
        flags_final = (f2 & ~0x02000000 & M32) | (saved & 0x02000000)
    return ev, (nx, ny), flags_final, aimv, fired


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def cmp_bits(got, want):
    """want None = cualquier NaN"""
    return is_nan(got) if want is None else got == want


def main(path):
    bad = checked = 0
    fired_n = nan_n = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        c = dict(row['input'])
        c['cpos'] = [c['cpos_x'], c['cpos_y'], c['cpos_z']]
        c['ppos'] = [c['ppos_x'], c['ppos_y'], c['ppos_z']]
        ev, cross, flags_final, aimv, fired = expected(c)
        fired_n += fired
        for side in ('original', 'candidate'):
            o = row[side]['observed']
            ok = True
            why = ''
            got = o['events']
            if len(got) != len(ev):
                ok = False
                why = 'numero de eventos %d != %d' % (len(got), len(ev))
            else:
                for g, w in zip(got, ev):
                    if isinstance(w, tuple):
                        _, f1, origin, (mz, tg) = w
                        v = g[1:]
                        okf = (v[0] == 31 and v[1] == 0 and v[3] == 5000 and v[4] == 0 and v[5] == 0 and v[6] == 0 and v[7] == 0 and v[8] == tag(2) and v[9] == f1
                               and v[10] == 0xF00D0001 and v[11:14] == origin and v[14] == 0xF00D0001 and v[15:18] == origin and v[18] == 0 and v[19] == 0xF00D0001 and v[23] == 0 and v[24] == 1 and v[25] == 1)
                        okf = okf and all(cmp_bits(v[20 + i], tg[i]) for i in range(3))
                        if not okf:
                            ok = False
                            why = 'FireInstantHit ' + str(g)
                    elif g != w:
                        ok = False
                        why = 'evento %s != %s' % (g, w)
            ch = o['changes']
            # mira final: se reconstruye desde la imagen de entrada y los cambios de la arena de CPlayerInfo
            if cross is None:
                want_all_empty = ch == [[], [], [], []]
                if not want_all_empty:
                    ok = False
                    why = why or 'cambios con la mira inactiva %s' % (ch,)
            else:
                base = bytearray(8)
                base[0:4] = c['tx'].to_bytes(4, 'little')
                base[4:8] = c['ty'].to_bytes(4, 'little')
                cur = bytearray(base)
                for off, b in ch[0]:
                    if CROSS + 4 <= off < CROSS + 12:
                        cur[off - CROSS - 4] = b
                    else:
                        ok = False
                        why = why or 'cambio fuera de la mira %x' % off
                gx = int.from_bytes(cur[0:4], 'little')
                gy = int.from_bytes(cur[4:8], 'little')
                if not (cmp_bits(gx, cross[0]) and cmp_bits(gy, cross[1])):
                    ok = False
                    why = why or 'mira %x,%x != %s' % (gx, gy, cross)
                nan_n += cross[0] is None or cross[1] is None
                if ch[1] or ch[3]:
                    ok = False
                    why = why or 'cambios en pad o matriz'
                curf = bytearray(c['flags'].to_bytes(4, 'little'))
                for off, b in ch[2]:
                    if 0x470 <= off < 0x474:
                        curf[off - 0x470] = b
                    else:
                        ok = False
                        why = why or 'cambio fuera de las banderas del ped %x' % off
                gf = int.from_bytes(curf, 'little')
                wf = flags_final if fired else c['flags']
                if gf != wf:
                    ok = False
                    why = why or 'banderas %x != %x' % (gf, wf)
            if not ok:
                bad += 1
                if bad <= 8:
                    print('DIFIERE', side, row['case'], why)
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad, 'disparos', fired_n, 'miras NaN', nan_n)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
