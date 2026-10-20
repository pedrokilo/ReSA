"""Compuertas independientes del banco AutomobileWrappers60 (0x44D520 HasCarStoppedBecauseOfLight, 0x6A2B10 Render, 0x6AA290 UpdateWheelMatrix), 3 x 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los tres ganchos RH registrados y con cuerpo en el ASI, los 15 colaboradores del EXE y los 13 de la DLL
sustituidos, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados, fp_difference vacio, la salida observada de CADA lado igual a la que modela fixture_oracle.py desde el
desensamblado (las magnitudes que dependen de fsin/fpatan/_CIasin se aceptan a 2 ulp contra el oraculo; EXE == DLL se exige en bits), la cobertura minima de las fixtures y la
restauracion final (ganchos, entradas de codigo vigiladas, ThePaths, temporizador, trucos, contador de fotogramas, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza; devuelve None si el registro aun no existe o no esta terminado.
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
HOOKS = {'CAutomobile::HasCarStoppedBecauseOfLight': 0x44D520, 'CAutomobile::Render': 0x6A2B10, 'CAutomobile::UpdateWheelMatrix': 0x6AA290}
EXE_COLLABORATORS = [0x7348C0, 0x6D0E60, 0x535300, 0x7F2450, 0x7F0910, 0x6D1080, 0x6E0E20, 0x59BD10, 0x59AF00, 0x59B460, 0x59B2C0, 0x59BBB0, 0x55E710, 0x55E6F0, 0x4AA440]
DLL_COLLABORATORS = 13


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin ganchos registrados o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != 3 * oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 3:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Gancho RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != EXE_COLLABORATORS or len(b['collaborators']['dll']) != DLL_COLLABORATORS or not all(b['collaborators']['dll']):
        raise RuntimeError('Colaboradores sin sustituir')
    seen = [set(), set(), set()]
    cov = [dict(), dict(), dict()]
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn = NAMES.index(row['function'])
        case = row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (row['function'], case))
        seen[fn].add(case)
        if row['pair'] != fn * oracle.CASES + case:
            raise RuntimeError('Numeracion de pares inesperada en %s %d' % (row['function'], case))
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (row['function'], case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (row['function'], case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], fn)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (row['function'], case, label, side['observed'], want))
        oracle.tally(fn, stats, cov[fn])
    for fn in range(3):
        if seen[fn] != set(range(oracle.CASES)):
            raise RuntimeError('Cobertura incompleta de ' + NAMES[fn])
    shortfalls = {}
    for fn in range(3):
        for key, minimum in oracle.COVERAGE_MIN[fn].items():
            if cov[fn].get(key, 0) < minimum:
                shortfalls['%s:%s' % (NAMES[fn], key)] = (cov[fn].get(key, 0), minimum)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != 3 * oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['paths_hash'] != b['initial_paths_hash'] or state[0]['timer'] != b['initial_timer'] or state[0]['cheats'] != b['initial_cheats']
            or state[0]['frame_counter'] != b['initial_frame_counter'] or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True):
        raise RuntimeError('ThePaths, el temporizador, los trucos, el contador de fotogramas, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {NAMES[fn]: cov[fn] for fn in range(3)}
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('automobilewrappers60', manifest['binaries']['asi'], analyze, 'AutomobileWrappers60', timeout=2400))
