"""Oraculo independiente (Python) de PlaneFiring74: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6E3440..0x6E37ED, lo que debe observarse de
CVehicle::GetPlaneWeaponFiringStatus (traza de colaboradores EN ORDEN, bool y tipo de ordenanza escritos, objetivo devuelto en EAX, campos del vehiculo cambiados, mira global,
posicion de la bengala) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

La aritmetica float de la bengala es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado intermedio
guardado como float (fstp dword, con desnormales) y vuelto a cargar. Los NaN solo se comparan como «es NaN» (la carga depende del orden de operandos; esas reglas las valida
flare_nan_hw contra el hardware); todo lo demas, bit a bit.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 vehiculo, 1 objetivo, 2 otra entidad, 3 matriz, 4 salidas.
"""
import json
import struct
import sys
from fractions import Fraction

GUARD = 0x40
M32 = 0xFFFFFFFF
PAD_STRIDE = 0x134
CROSS_FILL = 0xA5
CROSS_BYTES = 0x58


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


VEH, TGT, OTH, MAT, OUT = (tag(i, 0) for i in range(5))
OUT_STATUS, OUT_ORD = 4, 0x10     # desplazamientos del bool y del tipo en la ventana de salida
GARBAGE = 0xA0000010

# ---------------------------------------------------------------- aritmetica x87 a 24 bits
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
    """redondea al par a 24 bits (exponente ilimitado), o con cuanto minimo 2**quantum_exp_min (desnormales del float)"""
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


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN (cualquier carga)"""
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


LAST = {}   # intermedios del ultimo flare() (cobertura de run_local.py)


def flare(c):
    """posicion de la bengala (bits, o None para NaN) segun 0x6E3527..0x6E35D7: pos + (fwd * -2.5 + up * -1.0), cada paso guardado como float"""
    back = load(0xC0200000)
    down = load(0xBF800000)
    out = []
    LAST.clear()
    for ax in 'xyz':
        t1 = reload(store(mul(back, load(c['fwd_' + ax]))))
        t2 = reload(store(mul(down, load(c['up_' + ax]))))
        s = reload(store(add(t2, t1)))
        r = store(add(s, load(c['mpos_' + ax])))
        LAST[ax] = dict(t1=t1, t2=t2, s=s)
        out.append(r)
    return out


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0


def is_tiny(v):
    """magnitud distinta de cero por debajo del menor normal de un float (2**-126)"""
    return v != NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < Fraction(2) ** -126


# ---------------------------------------------------------------- el modelo de la funcion
def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def kind_of(model):
    s = s16(model)
    if ((s - 425) & M32) > 0x97:
        return 4
    return {425: 0, 576: 0, 447: 1, 464: 1, 487: 1, 497: 1, 548: 1, 476: 2, 520: 3}.get(s, 4)


def code_ptr(code):
    if code == 0:
        return 0
    if code == 1:
        return TGT
    if code == 2:
        return OTH
    if code == 3:
        return VEH
    return GARBAGE + 0x10 * (code - 4)


def dword_changes(base, old, new):
    return [(base + i, (new >> (8 * i)) & 0xFF) for i in range(4) if ((old >> (8 * i)) & 0xFF) != ((new >> (8 * i)) & 0xFF)]


def expected(c):
    """devuelve dict: events, veh (cambios del vehiculo), out (cambios de salida), ret, cross (hex), lock_target (etiqueta), flare (posicion o None), fired"""
    ev = []
    pad = (c['slot'] * PAD_STRIDE) & M32
    calls = {'gf': 0, 'jd': 0, 'scan': 0}
    veh = []
    status = None      # byte escrito en el bool
    ordn = None        # dword escrito en el tipo
    ret = 0
    cross = bytearray([CROSS_FILL]) * CROSS_BYTES
    lock_start, lock_target = c['lock_start'], c['lock_target']
    flare_pos = None
    fired = False

    ev.append([1, VEH])

    def gf():
        v = c['gf%d' % min(calls['gf'], 1)] & 0xFFFF
        calls['gf'] += 1
        ev.append([2, pad])
        return v

    def jd():
        v = c['jd%d' % min(calls['jd'], 1)] & 0xFFFF
        calls['jd'] += 1
        ev.append([3, pad])
        return v

    def scan(entity_code):
        r = c['scan%d' % min(calls['scan'], 1)]
        calls['scan'] += 1
        ev.append([7, VEH, code_ptr(entity_code)])
        return r

    def mark(red, green, blue, rotation):
        cross[4:8] = struct.pack('<I', 0)
        cross[0x14:0x17] = bytes([red, green, blue])
        cross[0x24:0x28] = struct.pack('<I', rotation)

    model = c['model'] & 0xFFFF
    k = kind_of(model)
    now = c['now']
    early = False
    if k == 2:
        if gf() == 2:
            status = 1
        ordn = 0
    elif k == 0:
        if gf() == 2:
            status = 1
        elif jd() == 1:
            ordn = 1
        else:
            status, ordn = 0, 0
    elif k == 1:
        if gf() == 2:
            status = 1
        if model == 0x1D0 and (c['flag_a'] & 0xFF) != 0 and (c['flag_b'] & 0xFF) != 0:
            if gf() == 1:
                status = 1
        ordn = 0
    elif k == 3:
        ordn = 0
        if jd() == 1 and now > ((c['gun'] + 2000) & M32):
            flare_pos = flare(c)
            ev.append(('proj',))
            veh += dword_changes(0x4D4, c['gun'], now)
            fired = True
        if jd() == 2:
            ev.append([6, 0])
            if (c['locked'] & 0xFF) != 0 and lock_start != 0:
                t = scan(lock_target)
                if t != 0 and t == lock_target and ((now - lock_start) & M32) > 0x5DC:
                    ordn = 2
                    mark(0xFF, 0, 0, 0x3F800000)
                    ret = code_ptr(t)
                    early = True
            if not early:
                ordn = 1
        else:
            ev.append([4, pad])
            if (c['et'] & 0xFF) != 0:
                lock_start, lock_target = now, 0
            else:
                ev.append([5, pad])
                if (c['tg'] & 0xFF) != 0:
                    if lock_start == 0:
                        lock_start = now
                    t = scan(lock_target)
                    if t == 0 or t != lock_target:
                        lock_start = now
                    if ((now - lock_start) & M32) > 0x5DC:
                        mark(0xFF, 0, 0, 0x3F800000)
                    else:
                        mark(0xFF, 0xFF, 0xFF, 0)
                    lock_target = t
                else:
                    lock_start, lock_target = 0, 0
    else:
        status, ordn = 0, 0
    veh = veh + dword_changes(0x9DC, c['lock_start'], lock_start)
    out = []
    if status is not None and status != (c['st_init'] & 0xFF):
        out.append((OUT_STATUS, status))
    if ordn is not None:
        out += dword_changes(OUT_ORD, c['ord_init'], ordn)
    return dict(events=ev, veh=sorted(veh), out=sorted(out), ret=ret, cross=bytes(cross).hex(), lock_target=code_ptr(lock_target), flare=flare_pos, fired=fired,
                status=status, ordn=ordn, path=dict(kind=k, early=early))


def check_side(c, o, exp):
    """compara un lado (observed) con el modelo; devuelve el motivo del primer desacuerdo o None"""
    got = o['events']
    if len(got) != len(exp['events']):
        return 'numero de eventos %d != %d' % (len(got), len(exp['events']))
    for g, w in zip(got, exp['events']):
        if isinstance(w, tuple):
            # AddProjectile(creador, arma 58 = WEAPON_FLARE, x, y, z, fuerza 0, direccion = matriz + 0x10, victima nula, direccion[0..2])
            if g[0] != 8 or g[1:3] != [VEH, 58] or g[6] != 0 or g[7] != tag(3, 0x10) or g[8] != 0 or g[9:12] != [c['fwd_x'], c['fwd_y'], c['fwd_z']]:
                return 'AddProjectile cabecera ' + str(g)
            for k in range(3):
                p = exp['flare'][k]
                if p is None:
                    if not is_nan(g[3 + k]):
                        return 'componente %d deberia ser NaN: %x' % (k, g[3 + k])
                elif g[3 + k] != p:
                    return 'componente %d: %x != %x' % (k, g[3 + k], p)
        elif g != w:
            return 'evento %s != %s' % (g, w)
    if o['ret'] != exp['ret']:
        return 'EAX %x != %x' % (o['ret'], exp['ret'])
    ch = o['changes']
    veh = [[off, b] for off, b in ch[0] if not (0x9E0 <= off < 0x9E4)]
    if veh != [[off, b] for off, b in exp['veh']]:
        return 'cambios del vehiculo %s != %s' % (veh, exp['veh'])
    if ch[1] or ch[2] or ch[3]:
        return 'cambios en objetivo, otra entidad o matriz: %s' % (ch[1:4],)
    if ch[4] != [[off, b] for off, b in exp['out']]:
        return 'cambios de salida %s != %s' % (ch[4], exp['out'])
    if o['cross'] != exp['cross']:
        return 'mira %s != %s' % (o['cross'], exp['cross'])
    if o['lock_target'] != exp['lock_target']:
        return 'objetivo fijado %x != %x' % (o['lock_target'], exp['lock_target'])
    if o['flags'] != [c['flag_a'] & 0xFF, c['flag_b'] & 0xFF] or o['time'] != c['now']:
        return 'globales cambiadas'
    return None


def check_row(row):
    c = row['input']
    exp = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], exp)
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
                print('DIFIERE', r[0], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
