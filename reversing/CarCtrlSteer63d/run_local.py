"""Compuertas independientes del banco CarCtrlSteer63d (lote 63, CCarCtrl parte B1, tanda 4): SteerAICarBlockingPlayerForwardAndBack (0x422B20), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que las entradas de los colaboradores sustituidos sean codigo original del EXE y sus
cuerpos de la DLL esten en el ASI), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios de la arena del banco y del estado real que
lee la funcion) igual a la que modela fixture_oracle.py desde el desensamblado (NaN cualquiera == NaN), la cobertura de las fixtures y la restauracion final.
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

NAMES = ['CCarCtrl::SteerAICarBlockingPlayerForwardAndBack']
PER_FN = 4096
CASES = PER_FN * len(NAMES)
EXPECTED = {n: PER_FN for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
# colaboradores sustituidos por funcion (direccion del EXE; hooked=False: su hook queda apagado durante el banco), en el orden de LiveComparison.hpp
STUBBED = {
    NAMES[0]: [0x56E090, 0x56E120, 0x54F560, 0x54F1B0, 0x56E010, 0x59C910],
}
WATCHED = {a for v in STUBBED.values() for a in v}

MINIMUMS = {}
for _k in range(8):
    for _f in range(1):
        MINIMUMS['family%d_fn%d' % (_k, _f)] = 256
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
    if b['pairs'] != CASES or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if b['registered'] != {n: True for n in NAMES} or b['bodies'] != {n: True for n in NAMES}:
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    for n in NAMES:
        col = b['collaborators'][n]
        if col['dll_real'] or [s['exe'] for s in col['stubbed']] != STUBBED[n] or any(s['hooked'] for s in col['stubbed']):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + n)
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
        if row['function'] not in EXPECTED or case in seen or NAMES[case // PER_FN] != row['function']:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row.get('nan_raw_difference'):
            # solo se aceptan diferencias entre EXE y DLL que desaparecen al tratar cualquier NaN como la clase NaN
            cov['nan_raw_difference'] = cov.get('nan_raw_difference', 0) + 1
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        if oracle is not None:
            want, stats = oracle.expected(row['function'], row['input'], case)
            for label, side in (('EXE', a), ('DLL', c)):
                got = oracle.observed_canon(side['observed'])
                if got != want:
                    diff = [k for k in want if got.get(k) != want[k]]
                    raise RuntimeError('Salida distinta de la del oraculo en %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (case, label, diff, got, want))
            for key, value in stats.items():
                if value is True:
                    cov[key] = cov.get(key, 0) + 1
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
        sys.exit(runtime_runner.run_case('carctrlsteer63d', manifest['binaries']['asi'], analyze, 'CarCtrlSteer63d', timeout=3600))
