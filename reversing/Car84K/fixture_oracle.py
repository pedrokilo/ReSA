"""Oraculo independiente (Python) de Car84K: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x42F9C0..0x42FBB2, lo que debe observarse de
CCarCtrl::GenerateEmergencyServicesCar (los eventos de los falsos con todos sus argumentos, los ultimos tiempos de ambulancia y bomberos, los contadores y que ninguna arena cambie) y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion salvo las TABLAS de modelos por nivel de zona (0x8A5AB4 y 0x8A5AD4: datos
del EXE que el banco vuelca en el evento `begin` y el analizador entrega a `set_tables`).

La funcion no hace aritmetica float: todo son comparaciones de enteros (suma de 32 bits con comparacion con signo, tiempos sin signo, el nivel de busqueda con signo) y copias crudas de tres palabras.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
FFFFFFFF = 0xFFFFFFFF
GUARD = 0x40
MGR_TAG = 0x10000000 + GUARD          # Tag(objeto de la arena del gestor): arena 0 de la tabla, desplazamiento kGuard
FIRE_MANAGER = 0xB71F80               # CFireManager global (direccion cruda)
TABLES = {'ambulance': None, 'fire': None}


def set_tables(tables):
    TABLES['ambulance'] = [int(x) for x in tables['ambulance']]
    TABLES['fire'] = [int(x) for x in tables['fire']]


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def isnan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x007FFFFF) != 0


def expected(c):
    """Devuelve {'events': [...], 'statics': [lastA, lastF, amb, fire], 'marks': [...]} para la entrada c"""
    marks = []
    events = []
    last_a, last_f = c['lastA'], c['lastF']
    amb, fire = c['amb'], c['fire']

    def mark(m):
        marks.append(m)

    def result():
        return {'events': events, 'statics': [last_a, last_f, amb, fire], 'marks': marks,
                'changes': [[] for _ in range(8)]}

    mark('entered')
    if not (c['allow'] & 0xFF):
        mark('allow_off')
        return result()
    events.append([1])
    if c['gw'] & 0xFF:
        mark('gangwar')
        return result()
    events.append([2, FFFFFFFF])
    if s32(c['wl']) > 3:
        mark('wanted_high')
        return result()
    if c['area'] != 0:
        mark('area_nonzero')
        return result()
    level = c['level']
    if level == 0:
        mark('level_zero')
        return result()
    if level > 3:
        mark('level_high')
    total = (amb + fire + c['parked'] + c['mission'] + c['law'] + c['random']) & M32
    if s32(total) > s32(c['maxc']):
        mark('too_many')
        if (amb + fire + c['parked'] + c['mission'] + c['law'] + c['random']) > M32:
            mark('sum_wrap')
        return result()
    if (amb + fire + c['parked'] + c['mission'] + c['law'] + c['random']) > M32:
        mark('sum_wrap')
    if s32(total) == s32(c['maxc']):
        mark('sum_equal')

    now = c['now']
    st = 0       # llamadas a Stream* hechas
    gn = 0       # llamadas a GenerateOne hechas

    def sret():
        nonlocal st
        k = st if st < 2 else 1
        st += 1
        return c['sret'][k] & 1

    def gret():
        nonlocal gn
        k = gn if gn < 2 else 1
        gn += 1
        return c['gret'][k] & 1

    # ---- ambulancia
    if amb == 0:
        events.append([3])
        events.append([4, MGR_TAG])
        if s32(c['nfree']) >= 2:
            mark('amb_accident_path')
            events.append([5, FFFFFFFF])
            events.append([3])
            events.append([6, MGR_TAG, c['player'][0], c['player'][1], c['player'][2], 0])
            if not (c['nearest'] & 1):
                mark('amb_no_accident')
            else:
                events.append([7, 1])
                if not sret():
                    mark('amb_stream_fail')
                elif not (now > ((last_a + 30000) & M32)):
                    mark('amb_time_fail')
                    if now == ((last_a + 30000) & M32):
                        mark('amb_time_equal')
                    if last_a + 30000 > M32:
                        mark('amb_time_wrap')
                else:
                    if last_a + 30000 > M32:
                        mark('amb_time_wrap')
                    if c['hasMat'] & 1:
                        mark('amb_mat')
                        pos = c['mpos']
                    else:
                        mark('amb_placement')
                        pos = c['ppos']
                    events.append([9, TABLES['ambulance'][level], pos[0], pos[1], pos[2]])
                    if gret():
                        mark('amb_gen_ok')
                        last_a = now
                    else:
                        mark('amb_gen_fail')
        else:
            mark('amb_nfree_low')
            events.append([7, 0])
            sret()
    else:
        mark('amb_skip')

    # ---- bomberos
    if fire != 0:
        mark('fire_skip')
        return finish(c, marks, result)
    events.append([10, FIRE_MANAGER])
    if (c['nfires'] & 0xFFFF) < 3:
        mark('fire_few')
        if c['nfires'] >> 16:
            mark('nfires_upper_garbage')
        events.append([8, 0])
        sret()
        return finish(c, marks, result)
    if c['nfires'] >> 16:
        mark('nfires_upper_garbage')
    events.append([5, FFFFFFFF])
    events.append([11, FIRE_MANAGER, c['player'][0], c['player'][1], c['player'][2], 1, 1])
    if not (c['firePresent'] & 1):
        mark('fire_no_fire')
        return finish(c, marks, result)
    events.append([8, 1])
    if not sret():
        mark('fire_stream_fail')
        return finish(c, marks, result)
    if not (now > ((last_f + 0x88B8) & M32)):
        mark('fire_time_fail')
        if now == ((last_f + 0x88B8) & M32):
            mark('fire_time_equal')
        if last_f + 0x88B8 > M32:
            mark('fire_time_wrap')
        return finish(c, marks, result)
    if last_f + 0x88B8 > M32:
        mark('fire_time_wrap')
    pos = c['fpos']
    events.append([9, TABLES['fire'][level], pos[0], pos[1], pos[2]])
    if gret():
        mark('fire_gen_ok')
        last_f = now
    else:
        mark('fire_gen_fail')
    return finish(c, marks, result)


def finish(c, marks, result):
    if gen_both(marks):
        marks.append('both_gen')
    for x in list(c['player']) + list(c['mpos']) + list(c['ppos']) + list(c['fpos']):
        if isnan(x):
            marks.append('nan_input')
        elif (x & 0x7F800000) == 0x7F800000:
            marks.append('inf_input')
        elif (x & 0x7F800000) == 0 and (x & 0x7FFFFF):
            marks.append('denormal_input')
        elif x == 0x80000000:
            marks.append('negzero_input')
    return result()


def gen_both(marks):
    return any(m.startswith('amb_gen') for m in marks) and any(m.startswith('fire_gen') for m in marks)


def check_side(c, o, e):
    if o['changes'] != e['changes']:
        return 'arenas cambiadas: %s != %s' % (str(o['changes'])[:300], str(e['changes'])[:300])
    if o['statics'] != e['statics']:
        return 'estaticas: %s != %s' % (o['statics'], e['statics'])
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
        if row.get('event') == 'begin':
            set_tables(row['model_tables'])
            continue
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
