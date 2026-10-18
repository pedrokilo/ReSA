"""Compuertas independientes del banco ModifySpeed27 (CAutoPilot::ModifySpeed, 0x41B980), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaborador (0x43C710) sustituido, todas las compuertas de
ABI/ruta/DF/trampas/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura
de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, ThePaths, temporizador, x87).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'traps_zero', 'globals_restored', 'floating_state_restored')
HOOKS = {'CAutoPilot::ModifySpeed': 0x41B980}


def need(counter, key, minimum, label, shortfalls):
    if counter.get(key, 0) < minimum:
        shortfalls[label] = (counter.get(key, 0), minimum)


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaborador sustituido')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != [0x43C710] or len(b['collaborators']['dll']) != 1 or not b['collaborators']['dll'][0]:
        raise RuntimeError('Colaborador 0x43C710 sin sustituir')
    seen = set()
    cov = {}

    def bump(key, n=1):
        cov[key] = cov.get(key, 0) + n

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
            if side['calls'] != (1 if stats['ran'] else 0):
                raise RuntimeError('Numero de llamadas a 0x43C710 inesperado en %d (%s): %s' % (case, label, side['calls']))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        inp = row['input']
        bump('ran' if stats['ran'] else 'early')
        bump('pres%d' % stats['pres'])
        bump('clamped', stats['clamped']); bump('target_nan', stats['target_nan'])
        bump('elapsed_negative', stats['elapsed_negative']); bump('now_negative', stats['now_negative'])
        bump('speed_scale_negative', inp['speed_scale'] < 0)
        bump('kind%d' % inp['kind'])
        if stats['ran']:
            bump('same_area', stats['same_area']); bump('diff_area', not stats['same_area']); bump('same_addr', stats['same_addr'])
            bump('branch_a%d' % stats['branch_a']); bump('branch_b%d' % stats['branch_b'])
            bump('scale_nonfinite', stats['scale_nonfinite']); bump('new_scale_zero', stats['new_scale_zero'])
            bump('source_style_differs', stats['source_style_differs'])
            bump('lane_negative', inp['lane'] < 0 or inp['next_lane'] < 0)
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in (('ran', 2800), ('early', 400), ('pres0', 200), ('pres1', 200), ('pres2', 2800), ('clamped', 300), ('target_nan', 20), ('elapsed_negative', 300),
                   ('now_negative', 500), ('speed_scale_negative', 300), ('same_area', 300), ('diff_area', 600), ('same_addr', 100), ('scale_nonfinite', 40),
                   ('new_scale_zero', 80), ('lane_negative', 300)):
        need(cov, key, m, key, shortfalls)
    for k in range(1, 4):
        need(cov, 'branch_a%d' % k, 250, 'branch_a%d' % k, shortfalls)
        need(cov, 'branch_b%d' % k, 250, 'branch_b%d' % k, shortfalls)
    for k in range(8):
        need(cov, 'kind%d' % k, 300, 'kind%d' % k, shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['paths_hash'] != b['initial_paths_hash'] or state[0]['timer'] != b['initial_timer']
            or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True):
        raise RuntimeError('ThePaths, el temporizador, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('modifyspeed27', manifest['binaries']['asi'], analyze, 'ModifySpeed27', timeout=1800))
