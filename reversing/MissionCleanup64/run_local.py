"""Compuertas independientes del banco MissionCleanup64: CMissionCleanup::Process (0x468560) y CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects (0x4652D0), 2048 + 2048 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que las entradas de los colaboradores sustituidos sean codigo original del EXE y sus
cuerpos de la DLL esten en el ASI), fp_difference vacio (x87 comparado con los registros vacios crudos), la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cuenta y hash
de las llamadas del final de Process, cambios de la arena del banco y de la memoria real del EXE) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures
y la restauracion final.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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

NAME_P = oracle.NAME_P
NAME_C = oracle.NAME_C
EXPECTED = {NAME_P: oracle.CASES_P, NAME_C: oracle.CASES_C}
PAIRS = sum(EXPECTED.values())
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
# colaboradores sustituidos (direccion del EXE; hooked=False: su hook queda apagado durante el banco), en el orden de LiveComparison.hpp
STUBBED = {
    NAME_P: [0x50AC20, 0x6F5DB0, 0x6F6B60, 0x6CCC50, 0x572440, 0x443FF0, 0x448B30, 0x56E230, 0x456DE0, 0x460EC0, 0x407D10, 0x44DD00, 0x44D400, 0x44D3B0, 0x45A160, 0x50C150, 0x50D2D0, 0x50CEF0,
             0x7010F0, 0x55FF20, 0x5072F0, 0x72A510, 0x40B490, 0x46A7C0, 0x722870, 0x5FBB70, 0x5F7340, 0x60C7C0, 0x60C800, 0x60A4B0, 0x5EFF70, 0x5EFF90, 0x6817D0, 0x634A40, 0x441560, 0x56E210,
             0x460840, 0x5397A0, 0x4639E0, 0x50B930, 0x56E870, 0x588BE0, 0x486670, 0x486300, 0x4866C0, 0x4839A0, 0x4AA3F0, 0x492FD0, 0x5FB870, 0x632C10, 0x607A70, 0x493160, 0x4936C0, 0x465A40,
             0x4654B0, 0x608830, 0x6089F0, 0x608A20],
    NAME_C: [0x410CE0, 0x405600, 0x4633E0, 0x542800, 0x6AF420, 0x6BEEB0]}
WATCHED = set(STUBBED[NAME_P]) | set(STUBBED[NAME_C])

MINIMUMS = {}
try:
    MINIMUMS.update(json.loads((HERE / 'minimums.json').read_text(encoding='utf-8')))
except FileNotFoundError:
    pass


def check_minimums(cov):
    """cobertura medida {clave: cuenta} frente a MINIMUMS: devuelve {clave: (medido, minimo)} de lo que no llega"""
    return {key: (cov.get(key, 0), m) for key, m in MINIMUMS.items() if cov.get(key, 0) < m}


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
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not True or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 con registros vacios crudos y ECX/EDX solo registrados')
    if b['pairs'] != PAIRS or b['cases'] != EXPECTED or b['functions'] != 2:
        raise RuntimeError('Numero de pares inesperado')
    if b['registered'] != {NAME_P: True, NAME_C: True} or b['bodies'] != {NAME_P: True, NAME_C: True}:
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    for name in EXPECTED:
        col = b['collaborators'][name]
        if col['dll_real'] or [s['exe'] for s in col['stubbed']] != STUBBED[name] or any(s['hooked'] for s in col['stubbed']):
            raise RuntimeError('Colaboradores sustituidos inesperados en %s' % name)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        name = row['function']
        if name not in EXPECTED or (name, case) in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (name, case))
        seen.add((name, case))
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (name, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (name, case, row['fp_difference']))
        want, stats = oracle.expected(name, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (name, case, label, diff, json.dumps(side['observed'])[:3000], json.dumps(want)[:3000]))
        for key, value in stats.items():
            if value is True:
                cov[name + ':' + key] = cov.get(name + ':' + key, 0) + 1
        cov[name + ':cases'] = cov.get(name + ':cases', 0) + 1
        cov['stack_seed'] = cov.get('stack_seed', 0) + 1
    if seen != {(n, i) for n, c in EXPECTED.items() for i in range(c)}:
        raise RuntimeError('Cobertura incompleta')
    shortfalls = check_minimums(cov)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != PAIRS or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'pool_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['pool_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas o el estado real del EXE no se restauraron')
    summary['compare_dead_x87_registers'] = True
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('missioncleanup64', manifest['binaries']['asi'], analyze, 'MissionCleanup64', timeout=3600))
