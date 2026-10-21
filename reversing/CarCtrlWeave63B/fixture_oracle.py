"""Oraculo del banco CarCtrlWeave63B (lote 63, CCarCtrl parte B2, tanda B): las seis funciones de recorrido de listas de sector.

Aqui la verdad es el EXE: el banco exige EXE == DLL bit a bit (eventos de los falsos EN ORDEN con todos sus argumentos, cambios de toda la arena, compuertas de ABI) con todos los colaboradores
sustituidos por falsos identicos. Este modulo valida ademas la coherencia de la entrada y modela, desde el desensamblado, la PRIMERA decision de cada nodo (que nodos pasan los filtros de
exploracion, uso de colision, modelo y rectangulo) para comprobar que el numero de llamadas a GetBoundCentre y a los colaboradores finales coincide con lo observado (`expected_calls`).
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
import struct

CASES = 4096
NAMES = ['CCarCtrl::SlowCarDownForCarsSectorList', 'CCarCtrl::SlowCarDownForObjectsSectorList', 'CCarCtrl::SlowCarDownForPedsSectorList', 'CCarCtrl::WeaveThroughCarsSectorList',
         'CCarCtrl::WeaveThroughObjectsSectorList', 'CCarCtrl::WeaveThroughPedsSectorList']
CASES_BY_NAME = {n: CASES for n in NAMES}
K_BOUND, K_DIST, K_SLOW_OTHER, K_SLOW_OBJ, K_WEAVE_CAR, K_WEAVE_OBJ, K_WEAVE_PED, K_GETMAT, K_FPV, K_FPP, K_REF, K_CLEAN, K_ADD, K_ISPLAYER, K_ANGER = range(1, 16)


def flt(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def visited_nodes(row):
    """indices de entidad de los nodos que superan el filtro de exploracion (codigo de exploracion distinto del actual), en orden; el codigo se marca al visitar"""
    seen = {}
    out = []
    for k in range(row['nodes']):
        i = row['item'][k]
        scan = seen.get(i, row['ent'][i]['scan'])
        if scan == row['scan_now']:
            continue
        out.append((k, i))
    return out


def expected_calls(name, row):
    """numero de llamadas a GetBoundCentre que debe observarse (funciones de coches y objetos), o None si no se modela"""
    if name not in (NAMES[0], NAMES[1], NAMES[3]):
        return None
    scan_now = row['scan_now']
    seen = {}
    n = 0
    veh = 0
    for k in range(row['nodes']):
        i = row['item'][k]
        e = row['ent'][i]
        if name == NAMES[0] and i == veh:
            continue
        scan = seen.get(i, e['scan'])
        if scan == scan_now:
            continue
        if name in (NAMES[0], NAMES[3]) and not (e['flags'] & 1):
            continue
        if name == NAMES[3] and i == row['physical']:
            continue
        seen[i] = scan_now
        if name == NAMES[1]:
            if e['msel'] > 2:
                continue
        n += 1
    return n


def expected(name, row, case):
    """(None: la verdad es el EXE; claves de cobertura desde la entrada)"""
    if row['fn'] != NAMES.index(name):
        raise AssertionError('funcion distinta de la entrada')
    st = {}
    st['nodes_%d' % row['nodes']] = True
    st['player_veh'] = row['player'] != 0
    st['remote'] = row['remote'] != 0
    st['scan_equal_nodes'] = any(row['ent'][row['item'][k]]['scan'] == row['scan_now'] for k in range(row['nodes']))
    st['vehicle_in_list'] = any(row['item'][k] == 0 for k in range(row['nodes']))
    return None, st


if __name__ == '__main__':
    row = {'fn': 0, 'nodes': 2, 'item': [1, 2, 0, 0, 0, 0], 'scan_now': 100, 'physical': 1,
           'ent': [{'scan': 7, 'flags': 1, 'msel': 3}, {'scan': 7, 'flags': 1, 'msel': 3}, {'scan': 100, 'flags': 1, 'msel': 3}] + [{'scan': 7, 'flags': 1, 'msel': 3}] * 4}
    assert expected_calls(NAMES[0], row) == 1
    print('autoprueba del oraculo: ok')
