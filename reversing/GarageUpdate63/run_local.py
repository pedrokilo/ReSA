"""Compuertas independientes del banco GarageUpdate63 (lote 63b): CGarage::Update (0x44AA50), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que las entradas de los colaboradores sustituidos sean codigo original del EXE y sus
cuerpos de la DLL esten en el ASI), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios de la arena del banco y del estado real que
lee/escribe la funcion) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final.
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
try:
    import fixture_oracle as oracle
except ImportError:
    oracle = None

NAME = 'CGarage::Update'
CASES = 16384
EXPECTED = {NAME: CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
# colaboradores sustituidos (direccion del EXE; hooked=False: su hook queda apagado durante el banco), en el orden de LiveComparison.hpp
STUBBED = [0x56E0D0, 0x56E210, 0x56E010, 0x56E230, 0x448BE0, 0x448D30, 0x44A830, 0x44A9C0, 0x448EE0, 0x449690, 0x449900, 0x44A660, 0x44A750, 0x448E50, 0x4494F0, 0x4495F0, 0x449100, 0x447720,
           0x449220, 0x447D80, 0x448550, 0x448330, 0x4485C0, 0x449A50, 0x449FF0, 0x447B80, 0x4489F0, 0x44A210, 0x4479A0, 0x448660, 0x566A60, 0x55C180, 0x558E40, 0x55A070, 0x506EA0, 0x4087E0, 0x588BE0,
           0x6A0050, 0x5625A0, 0x6D0B70, 0x6D0C00, 0x6D1080, 0x6D2250, 0x4C8500, 0x4AA440, 0x41BD90, 0x821B1E]
WATCHED = set(STUBBED)

MINIMUMS = {}
FAMILIES = 16
for _k in range(FAMILIES):
    MINIMUMS['family%d' % _k] = 256
MINIMUMS['stack_seed'] = 2048
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
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if b['registered'] != {NAME: True} or b['bodies'] != {NAME: True}:
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators'][NAME]
    if col['dll_real'] or [s['exe'] for s in col['stubbed']] != STUBBED or any(s['hooked'] for s in col['stubbed']):
        raise RuntimeError('Colaboradores sustituidos inesperados')
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    if oracle is not None:
        oracle.set_color_table(b['color_table'])
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
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        if oracle is not None:
            want, stats = oracle.expected(NAME, row['input'], case)
            for label, side in (('EXE', a), ('DLL', c)):
                if side['observed'] != want:
                    diff = [k for k in want if side['observed'].get(k) != want[k]]
                    raise RuntimeError('Salida distinta de la del oraculo en %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (case, label, diff, side['observed'], want))
            for key, value in stats.items():
                if value is True:
                    cov[key] = cov.get(key, 0) + 1
        cov['family%d' % row['input']['fam']] = cov.get('family%d' % row['input']['fam'], 0) + 1
        cov['stack_seed'] = cov.get('stack_seed', 0) + 1
    if seen != set(range(CASES)):
        raise RuntimeError('Cobertura incompleta')
    if oracle is not None:
        shortfalls = check_minimums(cov)
        if shortfalls:
            raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'pool_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['pool_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas o el estado real del EXE no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('garageupdate63', manifest['binaries']['asi'], analyze, 'GarageUpdate63', timeout=3600))
