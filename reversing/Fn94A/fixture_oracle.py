"""Oraculo independiente (Python) de Fn94A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las cuatro funciones del lote 94A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.

Funcion 0, CTaskComplexUseMobilePhone::Quit (0x634A40..0x634A5C; thiscall, ECX = tarea, ped en pila, `ret 4`; void): si el byte +0x1D es distinto de cero no hace nada; si no, lo pone a 1 y llama a la ranura 6
  (+0x18) de la vtabla de la tarea con (ped, 0, 0) (MakeAbortable, prioridad 0 = leisure, sin evento), ya con el byte +0x1D a 1.
Funcion 1, CTaskComplexKillPedOnFoot::SayGangAttack (0x65E9A0..0x65EA4B; cdecl, (atacante, objetivo); void): sale si alguno es nulo o si el tipo de peaton (+0x598, con signo) del atacante no esta en [7, 16];
  si el del objetivo no esta en [7, 16], solo sigue si FindPlayerPed(0) (0x56E210) devuelve el objetivo; entonces (tipo del objetivo releido) 7 -> Say(0x11), 9 -> Say(0x12), 14 -> Say(0x13) sobre el atacante
  con (0, 1.0f, 0, 0, 0) (0x5EFFE0, thiscall, 6 argumentos de pila).
Funcion 2, CTaskComplexWalkRoundObject::ComputeRouteRoundObject (0x6551D0..0x65528B; thiscall, ECX = tarea, ped en pila, `ret 4`; devuelve float en ST0): guarda el margen 0x8D22B0, lo pone a 0.7 (0x3F333333),
  llama a 0x5F6110(ped, objeto (+0x1C), &punto (+0x10), ruta (+0x20), 0), restaura el margen; 0x5F3660(z del ped, objeto, planos, distancias) con la posicion del ped (matriz +0x30 o placement +4);
  lado = 0x5F3AC0(&posicion, objeto); devuelve ((plano[lado].z * z + plano[lado].y * y) + plano[lado].x * x) + distancia[lado] sin guardarlo (ST0, 24 bits de mantisa).
Funcion 3, CTaskComplexPartner::ComputeTargetPoints (0x681FE0..0x682271; thiscall, ECX = tarea, (ped, &salida1, &salida2) en pila, `ret 0Ch`; void): d = companero - ped (guardada como float);
  punto base c = ped + d * factor (+0x3C): el producto de x se suma sin guardarlo, los de y y z se guardan antes; cx, cy, cz se guardan como float; CVector::Normalise(&d) (0x59C910); si GetTaskType (ranura 4) es 1204
  (charla) no ajusta; si no, hasta 11 pasadas con registros (cx, cy, cz sin guardar): r = raiz(ay*ay + ax*ax) con (cx - ped.x, cy - ped.y); si r < 0.7: w = 0.75 - r, c += d * w (los productos de x e y se guardan
  como float, el de z no); r2 igual con el companero: si r2 < 0.7, c -= d * w2; si no y no hubo empuje del primero, converge. Sin convergencia: byte +0x5C = 1 y sale. Si no: salida1 = c - d * 0.5 y
  salida2 = c + d * 0.5 (con cx y cy en registro para x e y de la salida 1 y x de la 2; cy y cz guardados para el resto).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction
from math import isqrt

NAN = ('n',)


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




STACK = 0x7F000000
GUARD = 0x40
A_TASK, A_PED, A_PMAT, A_OTHER, A_OMAT, A_OBJ, A_ROUTE = range(7)
M32 = 0xFFFFFFFF
DEFAULT_MARGIN = 0x3EB33333   # valor inicial de 0x8D22B0 (0.35f) en las funciones que no lo tocan


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def lt(a, b):
    return gt(b, a)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(x):
    """valor tras guardarlo como float (fstp dword) y volver a cargarlo"""
    return reload(store(x))


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo; NaN = indefinido"""
    if a == NAN:
        return (0xC000000000000000).to_bytes(8, 'little') + (0xFFFF).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + ((neg << 15)).to_bytes(2, 'little')
    sign = 1 if v < 0 else 0
    m = abs(v)
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    mant = m / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return int(mant).to_bytes(8, 'little') + ((sign << 15) | (e + 16383)).to_bytes(2, 'little')


def fp_stack(hexstr):
    """(TOP, lista de (st, 10 bytes)) de las posiciones validas de una imagen fnsave normalizada"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return top, valid


# ------------------------------------------------------------------------------------------------ funcion 0: CTaskComplexUseMobilePhone::Quit (0x634A40)
def expected0(c):
    a = c['a']
    marks, ev, changes = [], [], []
    byte = (a[0] >> 8) & 0xFF
    ped = [tag(A_PED, 0), 0, 0x00BADC00][a[1]]
    marks.append(['ped_arena', 'ped_null', 'ped_raw'][a[1]])
    if a[0] & 0xFFFF00FF:
        marks.append('garbage_neighbors')
    if byte:
        marks.append('quit_set')
    else:
        marks.append('quit_unset')
        changes = [[A_TASK, 0x1D, 1]]
        ev = [[6, tag(A_TASK, 0), ped, 0, 0, 1]]
    return {'events': ev, 'marks': marks, 'changes': changes, 'margin': DEFAULT_MARGIN, 'st0': None}


# ------------------------------------------------------------------------------------------------ funcion 1: CTaskComplexKillPedOnFoot::SayGangAttack (0x65E9A0)
def expected1(c):
    a = c['a']
    marks, ev = [], []
    res = {'events': ev, 'marks': marks, 'changes': [], 'margin': DEFAULT_MARGIN, 'st0': None}
    ped_t = s32(a[2])
    tgt_t = ped_t if a[1] == 2 else s32(a[3])
    if a[0] == 1:
        marks.append('ped_null')
        return res
    if a[1] == 1:
        marks.append('target_null')
        return res
    if not (7 <= ped_t <= 16):
        marks.append('ped_not_gang')
        return res
    if not (7 <= tgt_t <= 16):
        marks.append('target_not_gang')
        ev.append([30, 0])
        pedptr = tag(A_PED, 0)
        tgtptr = pedptr if a[1] == 2 else tag(A_OTHER, 0)
        player = [tgtptr, 0, pedptr, 0x00ABCD00][a[4]]
        if player != tgtptr:
            marks.append('not_player')
            return res
        marks.append('player_match')
    else:
        marks.append('target_gang')
    ctx = {7: 0x11, 9: 0x12, 14: 0x13}.get(tgt_t)
    if ctx is None:
        marks.append('no_say')
    else:
        marks.append('say_%d' % ctx)
        ev.append([31, tag(A_PED, 0), ctx, 0, 0x3F800000, 0, 0, 0])
    return res


# ------------------------------------------------------------------------------------------------ funcion 2: CTaskComplexWalkRoundObject::ComputeRouteRoundObject (0x6551D0)
def expected2(c):
    a = c['a']
    marks, ev = [], []
    has_mat = a[0] != 0
    marks.append('matrix' if has_mat else 'placement')
    posptr = tag(A_PMAT, 0x30) if has_mat else tag(A_PED, 4)
    obj = tag(A_OBJ, 0) if a[11] == 0 else 0x00C0FFEE
    route = tag(A_ROUTE, 0) if a[12] == 0 else 0x00A0B0C0
    if a[11]:
        marks.append('raw_object')
    if a[12]:
        marks.append('raw_route')
    ev.append([40, tag(A_PED, 0), obj, tag(A_TASK, 0x10), route, 0, 0x3F333333])
    ev.append([41, a[3] & M32, obj, STACK, STACK])
    ev.append([42, posptr, obj])
    side = a[29]
    marks.append('side%d' % side)
    pl = [load(a[13 + 3 * side]), load(a[14 + 3 * side]), load(a[15 + 3 * side])]
    pos = [load(a[1]), load(a[2]), load(a[3])]
    dot = load(a[25 + side])
    r = add(add(add(mul(pl[2], pos[2]), mul(pl[1], pos[1])), mul(pl[0], pos[0])), dot)
    if r == NAN:
        marks.append('result_nan')
    elif r[0] == 'i':
        marks.append('result_inf')
    else:
        marks.append('result_zero' if r[1] == 0 else 'result_finite')
    return {'events': ev, 'marks': marks, 'changes': [], 'margin': a[7] & M32, 'st0': r}


# ------------------------------------------------------------------------------------------------ funcion 3: CTaskComplexPartner::ComputeTargetPoints (0x681FE0)
THR = load(0x3F333333)    # 0.7
KK = load(0x3F400000)     # 0.75
HALF = load(0x3F000000)   # 0.5


def expected3(c):
    a = c['a']
    marks, ev = [], []
    marks.append('matrix_ped' if a[0] else 'placement_ped')
    marks.append('matrix_partner' if a[7] else 'placement_partner')
    P = [load(a[1]), load(a[2]), load(a[3])]
    Q = [load(a[8]), load(a[9]), load(a[10])]
    f = load(a[14])
    dn = [load(a[15]), load(a[16]), load(a[17])]
    d = [fl(sub(Q[0], P[0])), fl(sub(Q[1], P[1])), fl(sub(Q[2], P[2]))]
    # punto base: el producto de x se suma sin guardarlo; los de y y z se guardan antes de sumar; las tres coordenadas se guardan como float
    cx_mem = fl(add(mul(d[0], f), P[0]))
    cy_mem = fl(add(fl(mul(d[1], f)), P[1]))
    cz_mem = fl(add(fl(mul(d[2], f)), P[2]))
    ev.append([60, STACK, bits(d[0]), bits(d[1]), bits(d[2])])
    ev.append([4, tag(A_TASK, 0)])
    cx, cy, cz = cx_mem, cy_mem, cz_mem
    orig_flag = a[19] & 0xFF
    changes = []
    res = {'events': ev, 'marks': marks, 'changes': changes, 'margin': DEFAULT_MARGIN, 'st0': None}
    if (a[18] & M32) == 1204:
        marks.append('chat')
    else:
        converged = False
        passes = 0
        for _ in range(11):
            passes += 1
            ax = sub(cx, P[0])
            ay = sub(cy, P[1])
            r = sqrt24(add(mul(ay, ay), mul(ax, ax)))
            moved = lt(r, THR)
            if moved:
                marks.append('pushed_ped')
                w = sub(KK, r)
                mx = fl(mul(dn[0], w))
                my = fl(mul(dn[1], w))
                mz = mul(w, dn[2])
                cx, cy, cz = add(cx, mx), add(cy, my), add(cz, mz)
            ax = sub(cx, Q[0])
            ay = sub(cy, Q[1])
            r2 = sqrt24(add(mul(ay, ay), mul(ax, ax)))
            if lt(r2, THR):
                marks.append('pushed_partner')
                w = sub(KK, r2)
                mx = fl(mul(dn[0], w))
                my = fl(mul(dn[1], w))
                mz = mul(w, dn[2])
                cx, cy, cz = sub(cx, mx), sub(cy, my), sub(cz, mz)
            elif not moved:
                converged = True
                break
        marks.append('iter_%d' % (passes - 1))
        if not converged:
            marks.append('not_converged')
            if orig_flag != 1:
                changes.append([A_TASK, 0x5C, 1])
            return res
        marks.append('converged')
        cz_mem = fl(cz)
        cy_mem = fl(cy)
    if (a[18] & M32) == 1204:
        marks.append('converged_chat')
    hx_r = mul(dn[0], HALF)
    hx_f = fl(hx_r)
    hy_f = fl(mul(dn[1], HALF))
    hz_f = fl(mul(dn[2], HALF))
    out1 = [bits(sub(cx, hx_r)), bits(sub(cy, hy_f)), bits(sub(cz_mem, hz_f))]
    out2 = [bits(add(hx_f, cx)), bits(add(hy_f, cy_mem)), bits(add(hz_f, cz_mem))]
    if any(v == NAN_BITS for v in out1 + out2):
        marks.append('nan_out')
    old = [a[20 + k] & M32 for k in range(6)]
    new = out1 + out2
    for k in range(6):
        for j in range(4):
            ob, nb = (old[k] >> (8 * j)) & 0xFF, (new[k] >> (8 * j)) & 0xFF
            if ob != nb:
                changes.append([A_TASK, 0x40 + 4 * k + j, nb])
    return res


def expected(c):
    return {0: expected0, 1: expected1, 2: expected2, 3: expected3}[c['fn']](c)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('margin_final') != e['margin']:
        return 'margen final %s != %s' % (o.get('margin_final'), e['margin'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        want = ext80(e['st0'])
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != want:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], want.hex())
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
