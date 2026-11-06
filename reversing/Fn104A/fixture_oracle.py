"""Oraculo independiente (Python) de Fn104A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE (0x6A2530..0x6A27EC), lo que debe observarse de
CAutomobile::SetRandomDamage y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

CAutomobile::SetRandomDamage (0x6A2530, thiscall, ECX = coche, bRemoveStuff en pila (se lee el BYTE bajo), `ret 4`; void):
  - tirada = rand() del CRT del EXE (0x821B1E) & 0xFFFF, como float por 1/32768 (0x858B14); cada uso la multiplica por una constante (o por un entero: fimul) y trunca (_ftol2).
  - con bRemoveStuff: puertas = trunc(t * 6.0); si puertas != 0: paneles = trunc(t2 * 4.0); si no: paneles = 1 - trunc(t2 * -3.0).
    sin bRemoveStuff: puertas = trunc(t); si != 0: paneles = trunc(t2); si no: paneles = 1 (sin segunda tirada).
  - puertas: por cada una, tirada, n = trunc(t * (6 - i)); la primera puerta es n; las demas, el n-esimo (base 0) de las no elegidas aun. Se llama a
    GetCarNodeIndexFromDoor(puerta) (0x6C26F0), GetDoorStatus(gestor = coche + 0x5A0, puerta) (0x6C2230); si el estado es 0 y el bit (nodo & 31) de la mascara +0x310 de la estructura del model info
    (word +0x22 del coche, con signo, en la tabla 0xA9B0C8; +0x5C) esta puesto: SetDoorStatus(gestor, puerta, 2) (0x6C21C0) y SetComponentVisibility(coche, dword en coche + 0x648 + nodo * 4, 2) (0x6D2700).
  - paneles: igual con n = trunc(t * m), m = 4, 3, 2, 1...; el n-esimo (base 0) entre los paneles 0, 1, 5 y 6 aun no elegidos (2, 3 y 4 = traseros y parabrisas, nunca);
    GetCarNodeIndexFromPanel (0x6C26A0), GetPanelStatus (0x6C2180), SetPanelStatus(gestor, panel, 1) (0x6C2150), SetComponentVisibility(coche, marco, 2).
  - la x87 queda vacia y los bytes de las arenas no cambian (todo cambio de estado pasa por los colaboradores).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
GUARD = 0x40
CAR_DM = 0x5A0
CAR_NODES = 0x648
SELF_CAR = 0x10000000 + (0 << 20) + GUARD
SELF_DM = SELF_CAR + CAR_DM
ENTRY_AFTER = 0x10000000 + (1 << 20) + GUARD     # la entrada de la tabla queda con el model info propio (arena 1)


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand_word(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 104001) & M32)


def trunc(r, num):
    """trunc(r * (1/32768) * num): r de 15 bits por 1/32768 y por una constante de pocos bits: exacto en 24 bits"""
    v = Fraction(r * num, 32768)
    return int(v)   # int() de una Fraction trunca hacia cero


def expected(c):
    a = c['a']
    flag = (a[0] & 0xFF) != 0
    mask = a[13] & M32
    seed = (c['seed'] * 13 + 50) & M32      # la arena del coche es la primera que se llena
    ev = []
    marks = []
    idx = [0]

    def draw():
        i = idx[0]
        ev.append([1, i])
        idx[0] += 1
        return (a[1 + i] & 0xFFFF) if i < 11 else 0

    def damageable(node):
        return (mask >> (node & 31)) & 1

    def frame(node):
        return rand_word(seed, (GUARD + CAR_NODES + 4 * node) // 4)

    marks.append('flag_set' if flag else 'flag_clear')
    if a[0] >> 8:
        marks.append('arg_garbage')
    if any(v >> 16 for v in a[1:12]):
        marks.append('rand_garbage')
    if flag:
        nd = trunc(draw(), 6)
        if nd != 0:
            npn = trunc(draw(), 4)
        else:
            npn = 1 - trunc(draw(), -3)
            marks.append('panels_from_negative')
    else:
        nd = trunc(draw(), 1)
        if nd != 0:
            npn = trunc(draw(), 1)
        else:
            npn = 1
    marks.append('doors_%d' % nd)
    marks.append('panels_%d' % npn)

    picked = set()
    for i in range(nd):
        pick = trunc(draw(), 6 - i)
        if i == 0:
            door = pick
        else:
            free = [d for d in range(6) if d not in picked]
            door = free[pick]
            marks.append('door_scan')
        ev.append([2, door])
        node = a[14 + door]
        ev.append([4, SELF_DM, door])
        st = a[27 + door]
        if st != 0:
            marks.append('door_skip_status')
        elif not damageable(node):
            marks.append('door_skip_mask')
        else:
            marks.append('door_damaged')
            ev.append([6, SELF_DM, door, 2])
            ev.append([8, SELF_CAR, frame(node), 2])
            if node >= 32:
                marks.append('node_wrap')
        picked.add(door)

    picked = {2, 3, 4}
    mult = 4
    for i in range(npn):
        pick = trunc(draw(), mult)
        free = [p for p in range(7) if p not in picked]
        panel = free[pick]
        if panel >= 5:
            marks.append('panel_bumper')
        ev.append([3, panel])
        node = a[20 + panel]
        ev.append([5, SELF_DM, panel])
        st = a[33 + panel]
        if st != 0:
            marks.append('panel_skip_status')
        elif not damageable(node):
            marks.append('panel_skip_mask')
        else:
            marks.append('panel_damaged')
            ev.append([7, SELF_DM, panel, 1])
            ev.append([8, SELF_CAR, frame(node), 2])
            if node >= 32:
                marks.append('node_wrap')
        picked.add(panel)
        mult -= 1
    marks.append('rands_%d' % idx[0])
    return {'events': ev, 'changes': [], 'entry_after': ENTRY_AFTER, 'marks': marks}


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


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('entry_after') != e['entry_after']:
        return 'entrada de la tabla de modelos %s != %s' % (o.get('entry_after'), e['entry_after'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
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
