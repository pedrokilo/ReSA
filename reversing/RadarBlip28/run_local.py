"""Compuertas independientes del banco RadarBlip28 (CRadar::DrawEntityBlip, 0x587000), 12288 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, los 17 colaboradores sustituidos (collaborators_stubbed), todas las
compuertas de ABI/ruta/DF/restauracion en AMBOS lados (route_ok incluye que cada colaborador sustituido salte al ASI durante la llamada), fp_difference vacio, la salida de CADA lado
(traza completa de llamadas a los colaboradores con sus argumentos en bits, fila de la traza del blip, estaticas 0xBAA370..0xBAA383, resto de la memoria vigilada y de los pools falsos
sin cambios) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hook, entradas de codigo parcheadas, regiones globales, x87).
La compuerta kCompareDeadX87Registers es false A PROPOSITO (la funcion usa x87 en el EXE y SSE en la DLL): el estado x87 se compara normalizado, ver Core.hpp y README.md.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza; devuelve None si el registro aun no esta completo.
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {'CRadar::DrawEntityBlip': 0x587000}
SITES = 17
# Cobertura minima: calibrada con el generador de Fixtures.hpp compilado en el anfitrion y el oraculo (12288 casos, 18/10/2026), con margen (aprox. 40-55 % de lo observado).
MINIMUMS = {
    'air_near': 600, 'air_far': 350, 'air_dist_nan': 80, 'air_update': 500, 'air_skip': 100, 'air_wrap': 80, 'air_light': 40, 'air_map': 170, 'air_runway': 90, 'guard_init': 600,
    'circles': 22, 'spot_far': 10, 'spot_map': 35, 'script_idx_neg': 65, 'pickup_idx_neg': 65,
    'zoom_path': 600, 'zoom_with_vehicle': 800, 'zoom_obj_pickup': 130, 'zoom_remain': 100, 'zoom_far_exit': 45, 'zoom_map_exit': 10, 'topdown': 25,
    'zoom_height=low': 120, 'zoom_height=high': 25, 'zoom_height=same': 85,
    'height=0': 300, 'height=1': 80, 'height=2': 280, 'legend': 340, 'marker': 600, 'marker_wrap': 200, 'shortrange_exit': 150, 'disp_exit': 800, 'map_exit': 70,
    'sprite_draw': 500, 'sprite_negative': 190, 'sprite_zero_in_sprite_pass': 500, 'sprite_raw_garbage': 3000,
    'pool_miss': 380, 'ped_foot': 850, 'ped_in_veh': 850, 'ped_veh_extra': 220, 'ped_veh_null': 190, 'entity_enex': 500, 'type3_no_enex': 450, 'enex_ignored_type': 600, 'trace_enex': 1200,
    'matrix': 1200, 'no_matrix': 1200,
    'typ=1': 1000, 'typ=2': 1700, 'typ=3': 300, 'typ=6': 300, 'typ=7': 300, 'typ=8': 900,
    'disp=0': 500, 'disp=1': 500, 'disp=2': 2000, 'disp=3': 2000, 'is_sprite': 3000, 'not_sprite': 3000,
}
for t in (0, 4, 5, 9, 10, 11, 12, 13, 14, 15):
    MINIMUMS['typ=%d' % t] = 30
for name in ('veh', 'centre', 'xform', 'limit', 'screen', 'colour', 'marker', 'enex', 'heading', 'pickup_index', 'script_index', 'trace_height', 'rotating_sprite', 'circle', 'sprite', 'legend', 'revealed'):
    MINIMUMS['call_' + name] = 20


def need(counter, key, minimum, shortfalls):
    if counter.get(key, 0) < minimum:
        shortfalls[key] = (counter.get(key, 0), minimum)


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin los colaboradores sustituidos (este banco sustituye 17 colaboradores por falsos con traza)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if len(b['collaborators']['stubbed']) != SITES:
        raise RuntimeError('Numero de colaboradores sustituidos inesperado')
    seen = set()
    cov = {}

    def bump(key, cond=True):
        if cond:
            cov[key] = cov.get(key, 0) + 1

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAMES[0] or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
            if side['observed'].get('others_unchanged') is not True or side['observed'].get('trace_overflow') is not False:
                raise RuntimeError('Memoria vigilada modificada o traza desbordada en %d' % case)
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, json.dumps(side['observed']), json.dumps(want)))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            if value is True:
                bump(key)
        bump('typ=%d' % stats['typ']); bump('disp=%d' % stats['disp'])
        for key in ('height', 'zoom_height'):
            if key in stats:
                bump('%s=%s' % (key, stats[key]))
        bump('not_sprite', not stats['is_sprite'])
        for call in want['calls']:
            bump('call_' + call[0])
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS.items():
        need(cov, key, m, shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['regions_at_baseline'] is not True):
        raise RuntimeError('El x87, las entradas de codigo parcheadas o las regiones globales no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('radarblip28', manifest['binaries']['asi'], analyze, 'RadarBlip28', timeout=1800))
