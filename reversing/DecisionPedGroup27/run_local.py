"""Compuertas independientes del banco DecisionPedGroup27 (CDecisionMakerTypes::MakeDecision Ped 0x606E70 y Group 0x606F80), 2 x 2560 = 5120 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los tres hooks RH (0x606E70, 0x606F80, 0x6040D0) registrados y con cuerpo en el ASI, bytes canonicos del
EXE en las ocho direcciones al inicio, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados (route_ok incluye que 0x606060 y 0x6007A0 sigan siendo los del EXE y que
en el candidato 0x6040D0 y la entrada salten a la DLL), fp_difference vacio, la salida de CADA lado (decision seleccionada, objetos sin cambios, canarios de salida, task/facial o EAX de
retorno de Group, estado de rand del EXE tras la llamada y numero de rand consumidos) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la
restauracion final. EAX/ECX/EDX de salida SOLO se registran (original_residue/candidate_residue), salvo el EAX de Group que es su retorno (README.md).
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
PER_FN = oracle.CASES_PER_FUNCTION
EXPECTED = {name: PER_FN for name in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'seh_restored', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = ('CDecisionMakerTypes::MakeDecision-Ped', 'CDecisionMakerTypes::MakeDecision-Group', 'CDecision::MakeDecision')


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
        raise RuntimeError('Inicio sin hooks registrados, o con colaboradores sustituidos (este banco ejecuta 0x6040D0/0x6007A0/0x606060 y rand reales)')
    b = begin[0]
    if b.get('canonical_bytes_ok') is not True:
        raise RuntimeError('Los bytes de alguna de las ocho direcciones vigiladas no son los canonicos del EXE al inicio')
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False or b.get('compare_group_return_eax') is not True:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado, EAX/ECX/EDX solo registrados salvo el retorno de Group')
    if b['pairs'] != 2 * PER_FN or b['cases'] != EXPECTED or b['functions'] != 2:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    seen = {name: set() for name in NAMES}
    cov = {}

    def bump(key, cond=True):
        if cond:
            cov[key] = cov.get(key, 0) + 1

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case, fn = row['case'], row['function']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        func = NAMES.index(fn)
        inp = row['input']
        if inp['func'] != func or inp['case'] != case:
            raise RuntimeError('Fila de entrada incoherente en %s %d' % (fn, case))
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(inp)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (fn, case, label, side['observed'], want))
        n, used = stats['count'], stats['rand_calls']
        if (n <= 0 or stats['found_seek']) and used != 0:
            raise RuntimeError('rand consumido con count<=0 o seek hallado en %s %d' % (fn, case))
        if n > 0 and not stats['found_seek'] and used != 1:
            raise RuntimeError('rand no consumido con tirada en %s %d' % (fn, case))
        p = ('ped_', 'group_')[func]
        bump(p + 'residue_differs', row['original_residue'] != row['candidate_residue'])
        bump(p + 'family%d' % inp['family'])
        bump(p + 'count%d' % n)
        bump(p + 'rand1', used == 1)
        bump(p + 'rand0_seek_found', n > 0 and stats['found_seek'])
        bump(p + 'seek_given_not_found', n > 0 and stats['seek_given'] and not stats['found_seek'])
        bump(p + 'veh', stats['veh']); bump(p + 'not_veh', not stats['veh'])
        bump(p + 'st%d' % inp['st'])
        for k in (1, 2, 3):
            bump(p + 'avoid%d' % k, stats['avoid%d' % k])
        bump(p + 'total_1530', stats['total'] == 1530)
        if used:
            bump(p + 'roll_max', stats['roll_is_max'])
            bump(p + 'roll_zero', stats.get('r') == 0)
            bump(p + 'miss_total_roll1', stats['miss_total_roll1'])
            bump(p + 'near_threshold', stats['near_threshold'])
            bump(p + 'old_double_style_differs', stats['old_double_style_differs'])
            bump(p + 'sel%d' % stats['selected'])
            if stats['roll_is_max'] and stats['total'] in (41, 47, 55, 61):
                bump(p + 'roll1_total_%d' % stats['total'])
        if func == 0:
            eff = inp['dm_b8'] if inp['use_group'] else inp['dm_b4']
            bump('ped_dm%d' % oracle.s32(eff))
            bump('ped_use_group', inp['use_group'])
            bump('ped_m1_mission', oracle.s32(eff) == -1 and inp['ped_type'] == 2)
            bump('ped_m1_random', oracle.s32(eff) == -1 and inp['ped_type'] == 1)
        else:
            bump('group_id%d' % oracle.s32(inp['group_id']))
            bump('group_m1_mission', oracle.s32(inp['group_id']) == -1 and inp['mission_flag'] == 1)
            bump('group_m1_random', oracle.s32(inp['group_id']) == -1 and inp['mission_flag'] == 0)
    if any(seen[name] != set(range(PER_FN)) for name in NAMES):
        raise RuntimeError('Cobertura incompleta')
    # Minimos ~ la mitad de lo que mide el generador portado a Python (el generador C++ no se compilo): si alguno falla por poco, ajustar la mezcla de la familia.
    shortfalls = {}
    for p in ('ped_', 'group_'):
        minimums = {'rand1': 600, 'rand0_seek_found': 100, 'seek_given_not_found': 100, 'veh': 600, 'not_veh': 600, 'avoid1': 300, 'avoid2': 100, 'avoid3': 100,
                    'roll_max': 70, 'roll_zero': 30, 'miss_total_roll1': 40, 'near_threshold': 120, 'old_double_style_differs': 40, 'total_1530': 2,
                    'roll1_total_41': 8, 'roll1_total_47': 8, 'roll1_total_55': 10, 'roll1_total_61': 8, 'family0': 2048}
        for k in range(1, 9):
            minimums['family%d' % k] = 64
        for k in range(4):
            minimums['st%d' % k] = 600
        for k in range(7):
            minimums['count%d' % k] = 30
        for k in range(6):
            minimums['sel%d' % k] = 12
        for key, m in minimums.items():
            need(cov, p + key, m, shortfalls)
    for key, m in {'ped_dm-2': 300, 'ped_dm-1': 300, 'ped_dm0': 300, 'ped_dm1': 300, 'ped_dm7': 300, 'ped_dm19': 300, 'ped_use_group': 600, 'ped_m1_mission': 100, 'ped_m1_random': 100,
                   'group_id-1': 600, 'group_id0': 300, 'group_id1': 300, 'group_id7': 300, 'group_id19': 300, 'group_m1_mission': 200, 'group_m1_random': 200}.items():
        need(cov, key, m, shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != 2 * PER_FN or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
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
        sys.exit(runtime_runner.run_case('decisionpedgroup27', manifest['binaries']['asi'], analyze, 'DecisionPedGroup27', timeout=2400))
