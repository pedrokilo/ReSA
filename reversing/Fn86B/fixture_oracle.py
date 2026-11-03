"""Oraculo independiente (Python) de Fn86B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x6EABA0..0x6EAE12, lo que debe observarse de
CPopulation::CreateSunbatheObject (cdecl: posicion, tipo; devuelve el objeto en EAX o 0) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Modelo (desensamblado):
  Si el word 0xBB4A70 (temporales) >= 0x96 (150): devuelve 0 sin llamar a nada. Modelo = word 0x8CD638 e indicador = 0.
  Tipo 5: rand() del EXE (0x821B1E) & 7 -> tipo efectivo 3,2,3,4,3,4,3,2 para 0..7. Tipo 0xB: rand() & 7 -> 7,8,9,10,7,10,9,8 para 0..7 y, si temporales >= 0x91 (145), devuelve 0.
  Cualquier otro tipo se usa tal cual. Tipo efectivo 2/3/4: modelo = word 0x8CD67C/0x8CD680/0x8CD684, indicador 0; 6/7/8/9/10: 0x8CD688/0x8CD68C/0x8CD690/0x8CD694/0x8CD698 e indicador 1; otro: 0x8CD638 e indicador 0.
  operator new (0x5A1EE0, 0x17C bytes): si devuelve nulo, devuelve 0. Constructor 0x5A1D70 (objeto, modelo sin signo de 16 bits, 1). Posicion: en la matriz (+0x14, en +0x30) si la hay, si no la compacta (+4).
  Con objeto RW (+0x18): CMatrix::UpdateRwMatrix 0x59AD70 (con matriz) o CSimpleTransform::UpdateRwMatrix 0x54EF40 (sin ella), con el RwMatrix del marco padre + 0x10. Velocidad (+0x44) y giro (+0x50) a cero, byte +0x13C = 3,
  SetIsStatic (ranura 4, con el indicador), contador de temporales + 1 (16 bits), +0x150 = hora + 43200000 y CWorld::Add (0x563220).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
M32 = 0xFFFFFFFF
ARENA_OBJ, ARENA_MAT, ARENA_RW, ARENA_FRAME, ARENA_POS = 0, 1, 2, 3, 4
SIZES = {ARENA_OBJ: GUARD + 0x180 + GUARD, ARENA_MAT: GUARD + 0x50 + GUARD, ARENA_RW: GUARD + 0x10 + GUARD, ARENA_FRAME: GUARD + 0x60 + GUARD, ARENA_POS: GUARD + 0x10 + GUARD}


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand32(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 86008) & M32)


def fill(n, seed):
    out = bytearray()
    for i in range(0, n, 4):
        out += rand32(seed, i // 4).to_bytes(4, 'little')
    return bytearray(out[:n])


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    cnt = c['count'] & 0xFFFF
    seed = c['seed']
    img = {a: fill(SIZES[a], (seed * 13 + 50 + a) & M32) for a in (ARENA_OBJ, ARENA_MAT, ARENA_RW, ARENA_FRAME)}
    cur = {a: bytearray(img[a]) for a in img}

    def w32(a, off, v):
        cur[a][GUARD + off:GUARD + off + 4] = (v & M32).to_bytes(4, 'little')

    def result(ret):
        changes = []
        for a in (ARENA_OBJ, ARENA_MAT, ARENA_RW, ARENA_FRAME):
            for i in range(SIZES[a]):
                off = i - GUARD
                if a == ARENA_OBJ and (0 <= off < 4 or 0x14 <= off < 0x1C):
                    continue
                if cur[a][i] != img[a][i]:
                    changes.append([a, off, cur[a][i]])
        mark('ret_1' if ret else 'ret_0')
        return {'ret': ret, 'changes': changes, 'events': ev, 'count': cnt, 'marks': marks}

    if cnt >= 150:
        mark('cnt_full')
        return result(0)
    t = c['type'] & M32
    rv = c['randv'] & M32
    if t == 5:
        mark('t5')
        ev.append([1])
        mark('t5_r%d' % (rv & 7))
        t = (3, 2, 3, 4, 3, 4, 3, 2)[rv & 7]
    elif t == 11:
        mark('t11')
        ev.append([1])
        mark('t11_r%d' % (rv & 7))
        t = (7, 8, 9, 10, 7, 10, 9, 8)[rv & 7]
        if cnt >= 145:
            mark('t11_full')
            return result(0)
    else:
        mark('t_direct')
        mark('t_direct_valid' if 1 <= t <= 10 else 't_direct_invalid')
    slot = {2: 1, 3: 2, 4: 3, 6: 4, 7: 5, 8: 6, 9: 7, 10: 8}.get(t, 0)
    mark('m%d' % slot)
    model = c['models'][slot] & 0xFFFF
    static = 1 if t in (6, 7, 8, 9, 10) else 0
    mark('static' if static else 'nostatic')
    ev.append([2, 0x17C])
    if c['newnull']:
        mark('newnull')
        return result(0)
    ev.append([3, tag(ARENA_OBJ, 0), model, 1])
    pos = [v & M32 for v in c['pos']]
    mat = bool(c['hasmat'])
    rw = bool(c['hasrw'])
    mark('mat' if mat else 'nomat')
    mark('rw' if rw else 'norw')
    for k in range(3):
        if mat:
            w32(ARENA_MAT, 0x30 + 4 * k, pos[k])
        else:
            w32(ARENA_OBJ, 4 + 4 * k, pos[k])
    if rw:
        mark('rw_mat' if mat else 'rw_simple')
        if mat:
            ev.append([4, tag(ARENA_MAT, 0), tag(ARENA_FRAME, 0x10)] + pos)
        else:
            ev.append([5, tag(ARENA_OBJ, 4), tag(ARENA_FRAME, 0x10)] + pos)
    for off in range(0x44, 0x5C, 4):
        w32(ARENA_OBJ, off, 0)
    cur[ARENA_OBJ][GUARD + 0x13C] = 3
    ev.append([6, tag(ARENA_OBJ, 0), static, 3, 0, 0])
    cnt = (cnt + 1) & 0xFFFF
    removal = (c['time'] + 43200000) & M32
    if c['time'] + 43200000 > M32:
        mark('time_wrap')
    w32(ARENA_OBJ, 0x150, removal)
    ev.append([7, tag(ARENA_OBJ, 0), cnt, removal, 3])
    return result(tag(ARENA_OBJ, 0))


CHANGE_KEYS = ('ret', 'changes', 'count')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
