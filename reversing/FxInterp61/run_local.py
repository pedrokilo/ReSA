"""Compuertas independientes del banco FxInterp61: las cuatro GetVal(float* outValues, float delta) de FxInterpInfo{Float,U255,32,255}_c (0x4A8470, 0x4A8800, 0x4A89C0, 0x4A8B80),
4 x 4096 = 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados, el resto
de la arena intacto (guard_ok), fp_difference vacio, la zona de salida de CADA lado (8 floats, en bits) igual a la que modela fixture_oracle.py desde el desensamblado (comodin '?'
SOLO en los empates de NaN con signos opuestos: ahi se exige EXE == DLL), la cobertura de las fixtures y la restauracion final (hooks, entradas de codigo vigiladas, x87).
La compuerta kCompareDeadX87Registers es false A PROPOSITO (el EXE y la DLL usan x87): el estado x87 se compara normalizado, ver Core.hpp y README.md.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta terminado).
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
HOOKS = {oracle.NAME_OF[i]: oracle.ADDRESS[i] for i in range(4)}

# Minimos de cobertura, calibrados con host_check.py / el volcado compilado de Fixtures.hpp (fixture_dump.exe), dejando ~40-50 % de margen.
# ALL = suma de las cuatro funciones; PERFUNC = minimo POR funcion; FLOAT = solo FxInterpInfoFloat_c (claves float).
MINIMUMS_ALL = {'single': 1200, 'interp': 6000, 'extrap_before': 2500, 'copy_last': 2700, 'looped': 4000, 'fold_whole_nonzero': 1500, 'fold_total_zero': 300, 'fold_big': 450,
                'numkeys_le0': 900, 'numkeys_gt12': 180, 'count_hi_garbage': 1000, 'count_zero': 750, 'looped_noncanon': 500, 'nan_delta': 400, 'inf_delta': 290, 'equal_times': 150,
                'out_inf': 400, 'out_nan': 180}
MINIMUMS_PERFUNC = {'single': 250, 'interp': 1300, 'copy_last': 600, 'looped': 900, 'fold_whole_nonzero': 330, 'extrap_before': 550, 'numkeys_le0': 190, 'count_hi_garbage': 220,
                    'nan_delta': 85, 'inf_delta': 60, 'fold_big': 90, 'fold_total_zero': 60}
MINIMUMS_FLOAT = {'ext_exponent': 60, 'out_sub': 70, 'out_nan': 150, 'out_inf': 150, 'snan_copy': 35, 'out_neg_zero': 12}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores sustituidos (las funciones no tienen colaboradores)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != 4 * oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 4:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b['collaborators'] != {'exe': [], 'dll': []}:
        raise RuntimeError('Las funciones no deben tener colaboradores sustituidos')
    seen = set()
    cov = {}

    def bump(key, n=1):
        if n:
            cov[key] = cov.get(key, 0) + n

    names = {n: i for i, n in oracle.NAME_OF.items()}
    names = {v: k for k, v in oracle.NAME_OF.items()}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        if row['function'] not in names:
            raise RuntimeError('Funcion inesperada %s' % row['function'])
        func = names[row['function']]
        key = (func, row['case'])
        if key in seen or row['input']['func'] != func:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], row['case']))
        seen.add(key)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES) or side['observed'].get('guard_ok') is not True:
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (row['function'], row['case'], {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (row['function'], row['case'], row['fp_difference']))
        want, stats = oracle.expected(row['input'], row['case'])
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s):\n  obtenido %s\n  esperado %s' % (row['function'], row['case'], label, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for k, v in stats.items():
            if k == 'family':
                bump('fam%d' % v)
            elif k == 'func':
                continue
            else:
                bump(k, bool(v))
                bump('f%d_%s' % (func, k), bool(v))
    if seen != {(f, n) for f in range(4) for n in range(oracle.CASES)}:
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for k, m in MINIMUMS_ALL.items():
        if cov.get(k, 0) < m:
            shortfalls[k] = (cov.get(k, 0), m)
    for f in range(4):
        for k, m in MINIMUMS_PERFUNC.items():
            if cov.get('f%d_%s' % (f, k), 0) < m:
                shortfalls['f%d_%s' % (f, k)] = (cov.get('f%d_%s' % (f, k), 0), m)
    for k, m in MINIMUMS_FLOAT.items():
        if cov.get('f0_%s' % k, 0) < m:
            shortfalls['f0_' + k] = (cov.get('f0_%s' % k, 0), m)
    for k in range(16):
        if cov.get('fam%d' % k, 0) != 4 * oracle.CASES // 16:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), 4 * oracle.CASES // 16)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != 4 * oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fxinterp61', manifest['binaries']['asi'], analyze, 'FxInterp61', timeout=1800))
