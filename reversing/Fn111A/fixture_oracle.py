"""Oraculo independiente (Python) de Fn111A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las trece funciones del lote 111A y lo compara con las filas
de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.

Funcion 0, GxtCharStrcat (0x718630, cdecl (dst, src), devuelve dst): busca el terminador de dst y copia src (y su terminador) a continuacion.
Funcion 1, GxtCharStrcpy (0x718660, cdecl (dst, src), devuelve dst) y funcion 3, TextCopy (0x69F910, cdecl, void): copian src (con su terminador) sobre dst.
Funcion 2, GxtCharStrlen (0x718690, cdecl): longitud hasta el primer cero.
Funcion 4, GxtCharToSafeAscii (0x69F8C0, cdecl, devuelve 0xC1BED0): puntero nulo -> buffer[0] = 0; si no, para n = 0..255 mientras src[n] != 0: >= 0x7F, '/', '*' o '?' pasan a ' '; buffer[n] = 0.
Funcion 5, CTaskCategories::ComputeTaskCategory (0x698680, cdecl, AL): tres consultas de GetTaskType (ranura 4) con comparaciones con signo del rango 0x3FB..0x3FD: cat = 1; 1a en rango -> 4; 2a en rango -> 2; 3a en rango -> 4, si no cat.
Funcion 6, CTaskCategories::IsKillPedTask (0x6985E0, cdecl (tarea, a2, a3)): a2 = a3 = 0; una consulta de GetTaskType; en rango 0x3FB..0x3FD (con signo) -> a2 = 1.
Funcion 7, CTaskCategories::IsFollowPedTask (0x698610, cdecl (tarea, a2, a3)): a2 = a3 = 0.
Funcion 8, CTaskGoToVehicleAndLean::DoTidyUp (0x660F60, thiscall, `ret 4`): vehiculo (+0x0C) no nulo -> byte +0x42E &= 0xFE; subtarea (+0x08) no nula -> ranura 6 (MakeAbortable) con (ped, 2, 0).
Funcion 9, IsDoorInUse (0x646CF0, cdecl (veh, puerta, &entrando, &saliendo)): puerta 8..11 (sin signo tras restar 8) -> mascara 4, 8, 1, 2; si veh+0x486 & mascara -> *entrando = 1; si veh+0x487 & mascara -> *saliendo = 1.
Funcion 10, CTaskComplexBeInCouple::GetSideOfPartner (0x683840, thiscall, `ret 4`, EAX): derecha = matriz del companero; ((ry*qy + rz*qz) + rx*qx) del peaton + el del companero negado ((ry*py + rz*pz) + rx*px), en x87 de 24 bits sin
  guardar entre medias; 2 si el resultado es menor que cero o NaN (fcomp 0.0 + test ah,1), si no 1.
Funciones 11 y 12, CWeaponInfo::GetRLoadA / GetRLoadB (0x73A280 / 0x73A2B0, thiscall, `ret 4`, AX): el byte bajo del argumento elige (agachado) la palabra de la tabla de apuntado (indice int16 en +0x6C, 24 bytes por fila);
  la fila se lee de la memoria real del juego antes de la llamada y se informa en la observacion (`table` = RLoadA, RLoadB, CrouchRLoadA, CrouchRLoadB).

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




STACK = 0x7F000000
GUARD = 0x40
A_TASK, A_PED, A_PMAT, A_OTHER, A_OMAT, A_VEH, A_OUT, A_SUB, A_DST, A_SRC, A_WI = range(11)
M32 = 0xFFFFFFFF
WIN = 0x300
SAFE_BUF = 0xC1BED0


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


SP = [0x2F, 0x2A, 0x3F, 0x7E, 0x7F, 0x80, 0xFF, 0x20, 0x41, 0x61, 0x01, 0x7D, 0x81, 0x2E, 0x5C, 0x7E]


def strbyte(seed, i, mode):
    r = mix((seed * 0x9E3779B1 + i * 0x85EBCA6B + 111) & M32)
    if mode == 0:
        return 1 + r % 255
    if mode == 1:
        return 0x20 + r % 0x5F
    if mode == 2:
        return SP[r % 16]
    return 0x80 + r % 128


def window(pad_seed, off, length, seed, mode):
    """ventana (0x300 bytes) de una arena de cadenas: relleno distinto de cero y la cadena encima"""
    w = [strbyte(pad_seed, i, 0) for i in range(WIN)]
    for i in range(length):
        w[off + i] = strbyte(seed, i, mode)
    w[off + length] = 0
    return w


def diff(arena, old, new):
    return [[arena, i, new[i]] for i in range(len(old)) if old[i] != new[i]]


def in_range(t):
    return 0x3FB <= s32(t) <= 0x3FD


def res(**kw):
    r = {'events': [], 'marks': [], 'changes': [], 'ret': None, 'buf': None, 'st0': None}
    r.update(kw)
    return r


def str_marks(a, with_dst, with_src):
    m = []
    if with_dst:
        m.append('dst_empty' if a[0] == 0 else 'dst_nonempty')
        if a[0] >= 128:
            m.append('dst_long')
        if a[2]:
            m.append('dst_unaligned')
        m.append('dst_mode%d' % a[6])
    if with_src:
        m.append('src_empty' if a[1] == 0 else 'src_nonempty')
        if a[1] >= 128:
            m.append('src_long')
        if a[3]:
            m.append('src_unaligned')
        m.append('src_mode%d' % a[7])
    return m


def expected_str(c, o):
    a, fn = c['a'], c['fn']
    r = res(marks=str_marks(a, fn != 2, True))
    if fn == 2:
        r['ret'] = a[1]
        return r
    old = window(a[8], a[2], a[0], a[4], a[6])
    src = [strbyte(a[5], i, a[7]) for i in range(a[1])]
    new = list(old)
    start = a[2] + a[0] if fn == 0 else a[2]
    for i, b in enumerate(src):
        new[start + i] = b
    new[start + len(src)] = 0
    r['changes'] = diff(A_DST, old, new)
    if fn != 3:
        r['ret'] = tag(A_DST, a[2])
    return r


def expected4(c, o):
    a = c['a']
    buf = [strbyte(c['seed'], i, 0) for i in range(0x101)]
    marks = []
    if a[4]:
        marks.append('null')
        buf[0] = 0
    else:
        src = [strbyte(a[2], i, a[3]) for i in range(a[0])] + [0]
        n = 0
        while n <= 0xFF and src[n] != 0:
            ch = src[n]
            buf[n] = 0x20 if (ch >= 0x7F or ch in (0x2F, 0x2A, 0x3F)) else ch
            n += 1
        marks.append('ends_by_terminator' if n <= 0xFF else 'ends_by_limit')
        if n == 0:
            marks.append('empty')
        buf[n] = 0
        if any(ch >= 0x7F for ch in src[:n]):
            marks.append('has_high')
        if any(ch in (0x2F, 0x2A, 0x3F) for ch in src[:n]):
            marks.append('has_separator')
    marks.append('mode%d' % a[3])
    return res(marks=marks, ret=SAFE_BUF, buf=bytes(buf).hex())


def expected5(c, o):
    a = c['a']
    ev = [[4, tag(A_TASK, 0)] for _ in range(3)]
    cat = 1
    if in_range(a[0]):
        cat = 4
    if in_range(a[1]):
        cat = 2
    out = 4 if in_range(a[2]) else cat
    marks = ['in0' if in_range(a[0]) else 'out0', 'in1' if in_range(a[1]) else 'out1', 'in2' if in_range(a[2]) else 'out2', 'result_%d' % out]
    if any(s32(x) < 0 for x in a[:3]):
        marks.append('negative_type')
    return res(events=ev, marks=marks, ret=out)


def out_bytes(off, init0, init_off):
    """bytes iniciales de la arena de salida: desplazamiento -> valor (Put8(0, init0) y despues Put8(off, init_off))"""
    d = {0: init0 & 0xFF}
    d[off] = init_off & 0xFF
    return d


def apply_out(init, final):
    return [[A_OUT, off, final[off]] for off in sorted(init) if init[off] != final[off]]


def expected6(c, o):
    a = c['a']
    init = out_bytes(a[1], a[2], a[3])
    final = {k: 0 for k in init}
    hit = in_range(a[0])
    if hit:
        final[0] = 1
    marks = ['in_range' if hit else 'out_of_range', 'same_byte' if a[1] == 0 else 'two_bytes']
    return res(events=[[4, tag(A_TASK, 0)]], marks=marks, changes=apply_out(init, final))


def expected7(c, o):
    a = c['a']
    init = out_bytes(a[1], a[2], a[3])
    final = {k: 0 for k in init}
    marks = ['task_%s' % ['arena', 'null', 'raw'][a[0]], 'same_byte' if a[1] == 0 else 'two_bytes']
    return res(marks=marks, changes=apply_out(init, final))


def expected8(c, o):
    a = c['a']
    marks, ev, changes = [], [], []
    if a[1] == 0:
        marks.append('vehicle')
        nb = a[3] & 0xFE
        if nb != a[3]:
            marks.append('bit_cleared')
            changes.append([A_VEH, 0x42E, nb])
        else:
            marks.append('bit_already_clear')
    else:
        marks.append('no_vehicle')
    if a[2] == 0:
        marks.append('subtask')
        ped = [tag(A_PED, 0), 0, 0x00BADC00][a[0]]
        ev.append([6, tag(A_SUB, 0), ped, 2, 0])
    else:
        marks.append('no_subtask')
    marks.append('ped_%s' % ['arena', 'null', 'raw'][a[0]])
    return res(events=ev, marks=marks, changes=changes)


DOOR_MASK = {8: 4, 9: 8, 10: 1, 11: 2}


def expected9(c, o):
    a = c['a']
    init = out_bytes(a[3], a[4], a[5])
    final = dict(init)
    marks = []
    door = a[0] & M32
    if (door - 8) & M32 > 3:
        marks.append('invalid_door')
    else:
        mask = DOOR_MASK[door]
        marks.append('door%d' % door)
        if a[1] & mask:
            final[0] = 1
            marks.append('getting_in')
        if a[2] & mask:
            final[a[3]] = 1
            marks.append('getting_out')
        if not (a[1] & mask) and not (a[2] & mask):
            marks.append('neither')
    marks.append('same_byte' if a[3] == 0 else 'two_bytes')
    return res(marks=marks, changes=apply_out(init, final))


def expected10(c, o):
    a = c['a']
    r = [load(a[10]), load(a[11]), load(a[12])]          # derecha x, y, z
    q = [load(a[1]), load(a[2]), load(a[3])]             # posicion del peaton
    p = q if a[13] else [load(a[7]), load(a[8]), load(a[9])]

    def dot(v):
        return add(add(mul(r[1], v[1]), mul(r[2], v[2])), mul(r[0], v[0]))
    s = add(dot(q), neg(dot(p)))
    marks = ['matrix_ped' if a[0] else 'placement_ped']
    if a[13]:
        marks.append('same_ped')
    if s == NAN:
        out = 2
        marks.append('sum_nan')
    elif num(s) < 0:
        out = 2
        marks.append('sum_negative')
    else:
        out = 1
        marks.append('sum_zero' if num(s) == 0 else 'sum_positive')
    marks.append('side%d' % out)
    return res(marks=marks, ret=out)


def expectedW(c, o):
    a = c['a']
    crouch = (a[0] & 0xFF) != 0
    idx = a[1] & 0xFFFF
    marks = ['crouch' if crouch else 'stand']
    if idx >= 0x8000:
        marks.append('negative_index')
    elif idx >= 12:
        marks.append('large_index')
    if a[0] & 0xFFFFFF00:
        marks.append('garbage_arg')
    if a[2] & 0xFFFF:
        marks.append('garbage_upper_index')
    ret = None
    if o is not None and 'table' in o:
        t = o['table']
        ret = t[(2 if crouch else 0) if c['fn'] == 11 else (3 if crouch else 1)]
    return res(marks=marks, ret=ret)


def expected(c, o=None):
    fn = c['fn']
    if fn in (0, 1, 2, 3):
        return expected_str(c, o)
    return {4: expected4, 5: expected5, 6: expected6, 7: expected7, 8: expected8, 9: expected9, 10: expected10, 11: expectedW, 12: expectedW}[fn](c, o)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if c['fn'] in (11, 12) and 'table' not in o:
        return 'sin tabla en la observacion'
    if e['ret'] is not None and o.get('ret') != e['ret']:
        return 'retorno %s != %s' % (o.get('ret'), e['ret'])
    if e['buf'] is not None and o.get('buf') != e['buf']:
        return 'buffer %s != %s' % (o.get('buf'), e['buf'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    return None


def check_row(row):
    c = row['input']
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        e = expected(c, o)
        why = check_side(c, o, e, row[side])
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
