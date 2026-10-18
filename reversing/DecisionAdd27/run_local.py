"""Compuertas independientes del banco DecisionAdd27 (CDecision::Add 0x600600 + CDecisionMakerTypes::AddEventResponse 0x6044C0), 6144 + 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS
lados (route_ok incluye que el otro colaborador de la ruta, 0x600600 en AddEventResponse, y _ftol2 0x821B40 no esten enganchados), fp_difference vacio, la salida de CADA lado
(objeto/ventana final, guardas, chances/flags sin modificar, nada escrito fuera de la ventana) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures y la restauracion final (hooks, entradas de codigo vigiladas, x87).
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
EXPECTED = dict(oracle.CASES)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {'CDecision::Add': 0x600600, 'CDecisionMakerTypes::AddEventResponse': 0x6044C0}
ADD, AER = oracle.NAME_ADD, oracle.NAME_AER

# Minimos de cobertura (por funcion). NO se comprobaron contra el generador (el banco no se compilo): son conservadores (~40-50 % de lo estimado a mano).
# Si alguno falla por poco, bajar el umbral o ajustar la mezcla de la familia en Fixtures.hpp.
MINIMUMS = {
    ADD: {
        'no_free': 300, 'written': 5000, 'skip_task': 1000, 'skip_prob_high': 250, 'skip_prob_any': 600, 'skip_bool0': 250, 'skip_bool1': 250, 'skip_bool_noncanon': 200,
        'near_free_skip': 500, 'ch_nan': 150, 'ch_inf': 60, 'ch_huge': 250, 'ch_neg': 500, 'ch_neg_nonzero_byte': 400, 'ch_ge256': 400, 'ch_ge256_nonzero_byte': 300,
        'ch_band255': 300, 'ch_ge65536_nonzero_byte': 200, 'ch_sub1': 200, 'ch_frac': 400, 'ch_negzero': 50, 'ch_denormal': 80,
        'byte_ge128': 1500, 'byte_ff': 400, 'byte_80': 300, 'byte_zero': 1500,
        'flags_00': 500, 'flags_01': 500, 'flags_10': 500, 'flags_11': 500, 'flag_low_byte_zero_nonzero': 500, 'flag_sign_only': 100, 'flag_not_0_1': 1000,
        'task_minus1': 80, 'task_min': 40, 'task_max': 40, 'task_ge_65536': 150, 'task_200': 40,
        'pos0_nonzero_byte': 2000, 'pos1_nonzero_byte': 2000, 'pos2_nonzero_byte': 2000, 'pos3_nonzero_byte': 2000,
    },
    AER: {
        'no_free': 250, 'written': 3500, 'skip_task': 600, 'skip_prob_high': 150, 'skip_prob_any': 300, 'skip_bool0': 150, 'skip_bool1': 150, 'skip_bool_noncanon': 100,
        'near_free_skip': 350, 'ch_nan': 100, 'ch_inf': 40, 'ch_huge': 180, 'ch_neg': 350, 'ch_neg_nonzero_byte': 250, 'ch_ge256': 250, 'ch_ge256_nonzero_byte': 200,
        'ch_band255': 200, 'ch_ge65536_nonzero_byte': 120, 'ch_sub1': 150, 'ch_frac': 300, 'ch_negzero': 30, 'ch_denormal': 50,
        'byte_ge128': 800, 'byte_ff': 300, 'byte_80': 200, 'byte_zero': 800,
        'flags_00': 300, 'flags_01': 300, 'flags_10': 300, 'flags_11': 300, 'flag_low_byte_zero_nonzero': 300, 'flag_sign_only': 60, 'flag_not_0_1': 500,
        'task_minus1': 50, 'task_min': 25, 'task_max': 25, 'task_ge_65536': 100, 'task_200': 25,
        'dm_neg': 250, 'dm_ge20': 150, 'dm_wrap': 500, 'idx_neg': 150, 'idx_ge41': 120, 'idx_wrap': 700, 'ev_wrap': 600, 'ev_real_table': 3000, 'ev_elsewhere': 150,
        'window_nongrid': 400, 'window_below_this': 150,
        'pos0_nonzero_byte': 1500, 'pos1_nonzero_byte': 1500, 'pos2_nonzero_byte': 1500, 'pos3_nonzero_byte': 1500,
    },
}
for _fn in MINIMUMS:
    for _k in range(0, 7):
        MINIMUMS[_fn]['free%d' % _k] = 250
    for _k in range(0, 6):
        MINIMUMS[_fn]['slot%d' % _k] = 200
    for _k in range(16):
        MINIMUMS[_fn]['kind%d' % _k] = 250
    for _k in range(4):
        MINIMUMS[_fn]['skip_prob%d' % _k] = 150
for _k in range(8):
    MINIMUMS[AER]['mode%d' % _k] = 500


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hooks registrados, o con colaboradores sustituidos (este banco ejecuta _ftol2 real)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 2:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    watched = {w['address'] for w in b['watched_entries']}
    if watched != {0x600600, 0x6044C0, 0x821B40}:
        raise RuntimeError('Entradas vigiladas inesperadas: %s' % sorted(watched))
    seen = {n: set() for n in NAMES}
    cov = {n: {} for n in NAMES}

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (fn, case, label, side['observed'], want))
        cv = cov[fn]

        def bump(key, cond=True):
            if cond:
                cv[key] = cv.get(key, 0) + 1
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        bump('kind%d' % row['input']['kind'])
        bump('free%d' % stats['free_count'])
        if stats['written']:
            bump('slot%d' % stats['slot'])
        if fn == AER:
            bump('mode%d' % stats['mode'])
        for key, value in stats.items():
            if value is True:
                bump(key)
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
            if cov[fn].get(key, 0) < m:
                shortfalls['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('decisionadd27', manifest['binaries']['asi'], analyze, 'DecisionAdd27', timeout=1800))
