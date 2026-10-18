"""Compuertas independientes del banco DecisionMakeDecision27 (CDecision::MakeDecision, 0x6040D0), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS
lados (route_ok incluye que 0x6007A0 no este enganchado), fp_difference vacio, la salida de CADA lado (objeto completo sin cambios, taskSlot, facialSlot, guardas,
estado de rand del EXE tras la llamada y numero de rand consumidos) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la
restauracion final (hook, entradas de codigo vigiladas, estado de rand del EXE, x87).
La compuerta kCompareDeadX87Registers es false A PROPOSITO (la funcion usa x87): el estado x87 se compara normalizado, ver Core.hpp y README.md.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
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
HOOKS = {'CDecision::MakeDecision': 0x6040D0}


def need(counter, key, minimum, shortfalls):
    if counter.get(key, 0) < minimum:
        shortfalls[key] = (counter.get(key, 0), minimum)


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado, o con colaboradores sustituidos (este banco ejecuta 0x6007A0 y rand reales)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
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
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        n = stats['count']
        used = stats['rand_calls']
        if (n <= 0 or stats['found_seek']) and used != 0:
            raise RuntimeError('rand consumido con count<=0 o seek hallado en %d' % case)
        if n > 0 and not stats['found_seek'] and used != 1:
            raise RuntimeError('rand no consumido con tirada en %d' % case)
        inp = row['input']
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        bump('kind%d' % inp['kind'])
        bump('count%d' % n)
        bump('rand0_count0', n <= 0)
        bump('rand0_seek_found', n > 0 and stats['found_seek'])
        bump('rand1', used == 1)
        bump('seek_given_not_found', n > 0 and stats['seek_given'] and not stats['found_seek'])
        bump('seek_given_empty', n <= 0 and stats['seek_given'])
        bump('veh', stats['veh']); bump('not_veh', not stats['veh'])
        bump('in_veh_raw_garbage', stats['in_veh_raw_garbage'])
        if stats['st_in_range']:
            bump('st%d' % oracle.s32(inp['st']))
        else:
            bump('st_oor'); bump('st_oor_count_pos', n > 0)
            bump('st_oor_neg', oracle.s32(inp['st']) < 0)
        for k in (1, 2, 3):
            bump('avoid%d' % k, stats['avoid%d' % k])
        bump('bool_filtered', stats['bool_filtered']); bump('task_minus1', stats['task_minus1']); bump('prob_zero_filtered', stats['prob_zero_filtered'])
        bump('total_1530', stats['total'] == 1530); bump('all_255', stats['all_255'])
        bump('big_total_ge_255', stats['total'] >= 255)
        if used:
            bump('roll_max', stats['roll_is_max'])
            bump('roll_zero', stats.get('r') == 0)
            bump('miss_total_roll1', stats['miss_total_roll1'])
            bump('near_threshold', stats['near_threshold'])
            bump('old_double_style_differs', stats['old_double_style_differs'])
            bump('sel%d' % stats['selected'])
            if stats['roll_is_max'] and stats['total'] in (41, 47, 55, 61):
                bump('roll1_total_%d' % stats['total'])
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    minimums = {'rand0_count0': 350, 'rand0_seek_found': 250, 'rand1': 2500, 'seek_given_not_found': 250, 'seek_given_empty': 20, 'veh': 1500, 'not_veh': 1500,
                'in_veh_raw_garbage': 150, 'st0': 400, 'st1': 400, 'st2': 400, 'st3': 400, 'st_oor': 300, 'st_oor_count_pos': 100, 'st_oor_neg': 100,
                'avoid1': 150, 'avoid2': 150, 'avoid3': 150, 'bool_filtered': 300, 'task_minus1': 300, 'prob_zero_filtered': 600,
                'total_1530': 100, 'big_total_ge_255': 300, 'roll_max': 150, 'roll_zero': 50, 'miss_total_roll1': 20, 'near_threshold': 500,
                'old_double_style_differs': 10, 'sel5': 40, 'roll1_total_41': 5, 'roll1_total_47': 5, 'roll1_total_55': 5, 'roll1_total_61': 5}
    for k in range(0, 7):
        minimums['count%d' % k] = 250
    for k in range(0, 5):
        minimums['sel%d' % k] = 100
    for k in range(16):
        minimums['kind%d' % k] = 300
    for key, m in minimums.items():
        need(cov, key, m, shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['rand_state'] != b['initial_rand_state'] or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True):
        raise RuntimeError('El estado de rand del EXE, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('decisionmakedecision27', manifest['binaries']['asi'], analyze, 'DecisionMakeDecision27', timeout=1800))
