"""Compuertas independientes del banco AutoRender75 (lote 75): CAutomobile::Render (EXE 0x6A2B10, virtual), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: el gancho RH virtual registrado y con cuerpo en el ASI, los 7 colaboradores del EXE y los 5 de la DLL sustituidos,
todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados, fp_difference vacio, la salida observada de CADA lado igual (en bits) a la que modela fixture_oracle.py desde el
desensamblado (aritmetica x87 exacta con fracciones), la cobertura minima de las fixtures (coverage.json) y la restauracion final (gancho, entradas de codigo vigiladas, temporizador,
truco 27, x87).
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

NAME = oracle.NAME
EXPECTED = {NAME: oracle.CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'traps_zero', 'globals_restored', 'floating_state_restored')
EXE_COLLABORATORS = [0x7348C0, 0x6D0E60, 0x535300, 0x7F2450, 0x7F0910, 0x6D1080, 0x6E0E20]
DLL_COLLABORATORS = 5
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin gancho registrado o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != {NAME} or not all(b['registered'].values()) or set(b['bodies']) != {NAME} or not all(b['bodies'].values()):
        raise RuntimeError('Gancho RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != EXE_COLLABORATORS or len(b['collaborators']['dll']) != DLL_COLLABORATORS or not all(b['collaborators']['dll']):
        raise RuntimeError('Colaboradores sin sustituir')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        if row['pair'] != case:
            raise RuntimeError('Numeracion de pares inesperada en %d' % case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'])
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        oracle.tally(stats, cov)
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    if cov.get('nan_conflict', 0):
        raise RuntimeError('Hay casos con dos NaN distintos en una operacion (el oraculo no lo garantiza)')
    short = {k: (cov.get(k, 0), m) for k, m in COVER.get(NAME, {}).items() if cov.get(k, 0) < m}
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['timer'] != b['initial_timer'] or state[0]['cheat'] != b['initial_cheat']
            or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True):
        raise RuntimeError('El temporizador, el truco, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {NAME: cov}
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('autorender75', manifest['binaries']['asi'], analyze, 'AutoRender75', timeout=3600))
