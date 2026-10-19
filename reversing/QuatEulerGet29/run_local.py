"""Compuertas independientes del banco QuatEulerGet29 (CQuaternion::Get(float*, float*, float*), 0x59C160), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados,
guardas de la arena intactas, fp_difference vacio, la salida de CADA lado (ventana completa con cuaternion y los tres floats de salida, en bits) igual a la que modela fixture_oracle.py desde el
desensamblado (con tolerancia de 2 ulp en los floats y comodin '??' SOLO en los resultados inestables o con empate de NaN: ver fixture_oracle.py; ahi se exige EXE == DLL), la cobertura de las fixtures
y la restauracion final (hook, entradas de codigo vigiladas, x87).
La compuerta kCompareDeadX87Registers es false A PROPOSITO (la funcion usa x87): el estado x87 se compara normalizado, ver Core.hpp y README.md.
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
HOOKS = {'CQuaternion::Get(float*, float*, float*)': 0x59C160}
MATRIX = 'CQuaternion::Get(RwMatrix*) [colaborador, hook apagado]'   # 0x59C080: ambos lados lo ejecutan apagado en el EXE / directo en C++

# Minimos de cobertura, calibrados con fixture_mirror.py (espejo en Python del generador; resultado: alias 625, all_known 3193, finite_in 3101, finite_out 3093, inf_in 393, nan_in 650,
# nan_out 1003, negzero_out 804, pi_out 359, subnormal_out 59, unknown 903, wrapped_out 2249, zero_out 1011) dejando ~40-50 % de margen. El espejo NO se ha comprobado contra el generador
# C++ (no se compilo): si algun minimo falla por poco, revisar las filas `input` registradas frente al espejo antes de bajar el umbral.
MINIMUMS = {'alias': 330, 'all_known': 2100, 'finite_in': 2000, 'finite_out': 2000, 'inf_in': 230, 'nan_in': 380, 'nan_out': 600, 'negzero_out': 450, 'pi_out': 200, 'subnormal_out': 30,
            'unknown': 500, 'wrapped_out': 1400, 'zero_out': 600}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores sustituidos (0x59C080 se ejecuta real en ambos lados)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    wanted = set(HOOKS) | {MATRIX}
    if set(b['registered']) != wanted or not all(b['registered'].values()) or set(b['bodies']) != wanted or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if len(b['collaborators']['exe']) != 1 or len(b['collaborators']['dll']) != 1 or not b['collaborators']['exe'][0].startswith('0x59C080'):
        raise RuntimeError('El unico colaborador debe ser 0x59C080 real (EXE con el hook Get-matrix apagado / C++ directo)')
    if len([w for w in b['watched_entries'] if w['address'] == 0x59C080]) != 1:
        raise RuntimeError('Falta vigilar la entrada de 0x59C080')
    seen = set()
    cov = {}

    def bump(key, n=1):
        if n:
            cov[key] = cov.get(key, 0) + n

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != oracle.NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES) or side['observed'].get('guard_ok') is not True:
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            if key == 'family':
                bump('fam%d' % value)
            else:
                bump(key, bool(value))
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS.items():
        if cov.get(key, 0) < m:
            shortfalls[key] = (cov.get(key, 0), m)
    for k in range(16):
        if cov.get('fam%d' % k, 0) != oracle.CASES // 16:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), oracle.CASES // 16)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
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
        sys.exit(runtime_runner.run_case('quateulerget29', manifest['binaries']['asi'], analyze, 'QuatEulerGet29', timeout=1800))
