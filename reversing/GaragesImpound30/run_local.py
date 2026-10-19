"""Compuertas independientes del banco GaragesImpound30 (CGarages::StoreCarInNearestImpoundingGarage 0x44A3C0), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook "StoreCarInNearestImpoundingGarage" registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion
en AMBOS lados, fp_difference vacio, la salida de CADA lado (trozos de la tabla de coches guardados que cambian, traza de los dos falsos, array de garajes y NumGarages intactos, bloques de
vehiculo y matriz intactos) igual a la que modela fixture_oracle.py desde el desensamblado, las constantes del banco iguales a las del oraculo, la cobertura de las fixtures y la restauracion final
(hook, entradas de codigo vigiladas, tablas del juego, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no termino).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = oracle.NAMES
EXPECTED = {name: oracle.CASES for name in NAMES}
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x44A3C0}
WATCHED = {0x44A3C0, 0x4489F0, 0x449760}
STUBBED = {0x4489F0, 0x449760}

# Minimos de cobertura. Calibrados con fixture_dump.cpp (std puro, compilado con cl: vuelca los 4096 `input`) + fixture_oracle.py (recuentos medidos en README.md), con ~45 % de margen.
# Si alguno falla por poco, comparar primero las filas `input` registradas con el volcado antes de bajar el umbral.
MINIMUMS = {
    'best_first': 503, 'best_found': 1564, 'best_last': 554, 'count_0': 180, 'count_1': 525, 'count_2': 858, 'count_2_after_shift': 315, 'has_candidate': 1939, 'house_0': 238, 'house_19': 235,
    'huge_in': 101, 'inf_in': 69, 'matrix': 1108, 'n_garages_0': 140, 'n_garages_50': 40, 'n_garages_ge_40': 78, 'nan_in': 142, 'near_initial_min': 140, 'negzero_in': 130, 'no_best': 375,
    'no_candidate': 312, 'other_pos_differs': 2252, 'placement': 1144, 'shifted': 315, 'tie_kept_first': 189, 'tiny_in': 240, 'trace_2': 1564, 'type_21': 542, 'type_22': 497, 'type_23': 524,
    'type_boundary': 391, 'type_high_bit': 205,
}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin colaboradores sustituidos (este banco los sustituye)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b.get('constants') != oracle.CONSTANTS:
        raise RuntimeError('Constantes del banco distintas de las del oraculo: %s != %s' % (b.get('constants'), oracle.CONSTANTS))
    watched = {w['address'] for w in b['watched_entries']}
    if watched != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas: %s' % sorted(watched))
    stubbed = b['collaborators']['stubbed']
    if {s['exe'] for s in stubbed} != STUBBED or not all(s['dll'] and s['dll'] != s['exe'] for s in stubbed) or b['collaborators']['exe_real'] != []:
        raise RuntimeError('Colaboradores sustituidos / reales inesperados: %s' % json.dumps(b['collaborators']))
    seen = set()
    cov = {}

    def bump(key, n=1):
        if n:
            cov[key] = cov.get(key, 0) + n

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            if key == 'family':
                bump('fam%d' % value)
            else:
                bump(key, bool(value))
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    for k in range(16):
        if cov.get('fam%d' % k, 0) != oracle.CASES // 16:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), oracle.CASES // 16)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['tables_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas, las tablas de garajes / coches guardados o su contador no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('garagesimpound30', manifest['binaries']['asi'], analyze, 'GaragesImpound30', timeout=1800))
