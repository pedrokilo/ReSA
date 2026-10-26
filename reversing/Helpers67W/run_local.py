"""Compuertas independientes del banco Helpers67W: CWaterLevel::RenderSeaBedSegment (0x6E6870), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, la salida de CADA lado (vertices, indices, contadores, resto de la region, bytes vigilados fuera de ella) igual a la que modela fixture_oracle.py desde el desensamblado,
la cobertura de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, region del bufer, x87).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'seh_chain_restored', 'direction_flag_clear', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ADDRESS))
FAMILY_MINIMUM = oracle.CASES // 16
STACK_SEED_MINIMUM = 8192

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~40-50 % de margen. Clave = 'f0_<estadistica>'.
MINIMUMS = {
    'f0_asym_x': 300,
    'f0_cancel': 530,
    'f0_idx_high': 2700,
    'f0_idx_max': 65,
    'f0_idx_zero': 95,
    'f0_max_x_special': 450,
    'f0_max_y_special': 480,
    'f0_min_x_special': 500,
    'f0_min_y_special': 460,
    'f0_out_denormal': 280,
    'f0_out_inf': 1200,
    'f0_out_nan': 730,
    'f0_out_negzero': 760,
    'f0_out_zero': 1090,
    'f0_real_segment': 1024,
    'f0_uv_denormal': 280,
    'f0_vtx_high': 2700,
    'f0_vtx_max': 65,
    'f0_vtx_zero': 115,
    'f0_x_big': 1450,
    'f0_x_inexact': 930,
    'f0_x_min': 18,
    'f0_x_neg': 3500,
    'f0_x_zero': 300,
    'f0_y_big': 1450,
    'f0_y_neg': 3800,
}


def check_minimums(cov):
    short = {}
    for k, m in MINIMUMS.items():
        if cov.get(k, 0) < m:
            short[k] = (cov.get(k, 0), m)
    for fam in range(16):
        key = 'f0_fam%d' % fam
        if cov.get(key, 0) < FAMILY_MINIMUM:
            short[key] = (cov.get(key, 0), FAMILY_MINIMUM)
    if cov.get('f0_stack_seed', 0) < STACK_SEED_MINIMUM:
        short['f0_stack_seed'] = (cov.get('f0_stack_seed', 0), STACK_SEED_MINIMUM)
    return short


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores sustituidos (la funcion no tiene colaboradores)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b['collaborators'] != {}:
        raise RuntimeError('Colaboradores inesperados')
    lay = b['layout']
    for key, want in oracle.LAYOUT.items():
        if lay[key] != want:
            raise RuntimeError('La region del banco no coincide con las constantes del oraculo: %s' % key)
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
        fn = row['function']
        if fn not in HOOKS:
            raise RuntimeError('Funcion inesperada %s' % fn)
        key = row['case']
        if key in seen or row['input']['func'] != 0 or row['input']['n'] != row['case']:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, row['case']))
        seen.add(key)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, row['case'], {k: side.get(k) for k in GATES}))
            if side['observed'].get('guard_ok') is not True or side['observed'].get('rest_ok') is not True:
                raise RuntimeError('Bytes vigilados o resto de la region alterados en %s %d' % (fn, row['case']))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, row['case'], row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], row['case'])
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, row['case'], label, diff, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for k, v in stats.items():
            if k == 'family':
                bump('f0_fam%d' % v)
            else:
                bump('f0_%s' % k, bool(v))
        bump('f0_stack_seed')
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = check_minimums(cov)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87 o las entradas vigiladas no se restauraron')
    if state[0]['globals'] != b['initial_globals']:
        raise RuntimeError('La region del bufer temporal de agua (contadores, indices, vertices) no se restauro')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('helpers67w', manifest['binaries']['asi'], analyze, 'Helpers67W', timeout=3600))
