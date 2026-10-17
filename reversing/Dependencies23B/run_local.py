"""Compuertas independientes del banco Dependencies23B (CVehicle::FlyingControl 0x6D85F0, 11 rutas x 1536 pares).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer): hook registrado, cuerpo en el ASI, colaboradores sustituidos, todas las compuertas de ABI/ruta/x87,
fp_difference vacio, registros x87 vacios iguales, cobertura MINIMA POR RUTA medida con la traza de colaboradores (1536 pares por ruta = 1,5x los 1024 exigidos)
y final con restauracion. kCompareDeadX87Registers debe ser true.
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

ROUTES = 11
PER_ROUTE = 1536
MODEL_OF_ROUTE = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 40]
NAMES = ['CVehicle::FlyingControl[ruta %d]' % r for r in range(ROUTES)]
EXPECTED = {n: PER_ROUTE for n in NAMES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
# Pares MINIMOS por ruta que deben llamar a cada colaborador. Tipos de traza: 1 ApplyTurnForce, 2 ApplyMoveForce, 3 GetSpeed, 4 HeightAboveCeiling, 5 GetSteeringLeftRight,
# 6 GetSteeringUpDown, 7 GetCarGunUpDown, 8 GetCarGunLeftRight, 9 GetAccelerate, 10 GetBrake, 11 GetLookLeft, 12 GetLookRight, 13 GetColModel, 14 IsPlayer, 15 GetPadFromPlayer.
# Medidos con el arnes sin juego (1536 casos por ruta, mismas fixtures) y fijados a ~60 % de lo observado.
MIN = {
    0: {1: 1024, 2: 1024, 4: 200, 6: 200, 14: 250, 15: 200},
    1: {1: 1024, 2: 1024, 3: 1024, 4: 300, 5: 60, 6: 70, 7: 70, 9: 50, 10: 50, 13: 1024, 14: 250, 15: 180},
    2: {1: 1024, 2: 1024, 4: 1024, 5: 60, 6: 60, 7: 50, 8: 30, 9: 50, 10: 50, 11: 55, 12: 55, 14: 230, 15: 180},
    3: {1: 1024, 2: 1024, 3: 1024, 4: 500, 5: 55, 6: 45, 7: 45, 9: 50, 10: 50, 13: 1024, 14: 250, 15: 180},
    4: {1: 1024, 2: 1024, 3: 1024, 4: 500, 5: 55, 6: 60, 7: 60, 9: 50, 10: 50, 13: 1024, 14: 250, 15: 190},
    5: {1: 1024, 2: 1024, 3: 1024, 4: 500, 5: 55, 6: 55, 7: 55, 9: 55, 10: 55, 13: 1024, 14: 240, 15: 170},
    6: {1: 1024, 2: 1024, 4: 1024, 5: 55, 6: 55, 7: 40, 8: 25, 9: 40, 10: 40, 11: 45, 12: 50, 14: 220, 15: 160},
    7: {1: 1024, 14: 250, 15: 180},
    8: {1: 1024, 2: 1024, 5: 55, 6: 65, 8: 35, 9: 50, 10: 50, 11: 55, 12: 55, 14: 250, 15: 180},
    9: {1: 1024, 14: 250, 15: 180},
    10: {1: 1024, 14: 250, 15: 180},
}
# Colaboradores que la ruta NO debe llamar nunca (prueba de que la ruta ejecutada es la prevista)
ABSENT = {0: (3, 13), 1: (), 2: (3, 13), 3: (), 4: (), 5: (), 6: (3, 13), 7: (2, 3, 4, 13), 8: (3, 4, 13), 9: (2, 3, 4, 13), 10: (2, 3, 4, 13)}


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaboradores sustituidos')
    if begin[0]['pairs'] != ROUTES * PER_ROUTE or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Pares o cuerpos RH inesperados')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    seen = {k: set() for k in EXPECTED}
    cov = {r: {} for r in range(ROUTES)}
    errno_pairs = [0] * ROUTES
    dead_equal = dead_total = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        route = NAMES.index(fn)
        a, b = row['original'], row['candidate']
        for side in (a, b):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d' % (fn, case))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        dead_total += 1
        dead_equal += bool(row['dead_registers_equal'])
        if row['dead_registers_equal'] is not True:
            raise RuntimeError('Registros x87 vacios distintos en %s %d' % (fn, case))
        inp = a['input']
        if inp.get('case') != case or inp.get('route') != route:
            raise RuntimeError('Entrada con otro numero de caso o de ruta')
        if inp['model'] != MODEL_OF_ROUTE[route]:
            raise RuntimeError('Modelo de vuelo distinto del de la ruta en %s %d' % (fn, case))
        for k in a['trace']['counts']:
            cov[route][int(k)] = cov[route].get(int(k), 0) + 1
        errno_pairs[route] += a['observed']['errno'] != 0
    for fn in EXPECTED:
        if seen[fn] != set(range(PER_ROUTE)):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    short = {}
    for r in range(ROUTES):
        for kind, need in MIN[r].items():
            if cov[r].get(kind, 0) < need:
                short['ruta %d tipo %d' % (r, kind)] = (cov[r].get(kind, 0), need)
        for kind in ABSENT[r]:
            if cov[r].get(kind, 0):
                short['ruta %d tipo %d inesperado' % (r, kind)] = (cov[r].get(kind, 0), 0)
        if errno_pairs[r] < 100:
            short['ruta %d errno' % r] = (errno_pairs[r], 100)
    if short:
        raise RuntimeError('Cobertura insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary.update(coverage_by_route=cov, errno_pairs_by_route=errno_pairs, compare_dead_x87_registers=begin[0].get('compare_dead_x87_registers'),
                   dead_registers_equal_pairs=[dead_equal, dead_total], coverage_method='serialized_inputs_and_collaborator_trace_counts_per_route')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies23b', manifest['binaries']['asi'], analyze, 'Dependencies23B', timeout=3600))
