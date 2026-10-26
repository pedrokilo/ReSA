"""Compuertas independientes del banco Helpers67S: CDecisionMakerTypes::CDecisionMakerTypes (0x4650F0), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, el hook de CDecision::SetDefault (0x600530) encendido (cuerpo en la DLL), los tres
colaboradores del EXE (0x821D30, 0x4650A0, 0x6040C0) sin hook y SIN llamadas desde el candidato (candidate_exe_calls == 0 en cada fila), todas las compuertas de ABI/ruta/DF/SEH/
restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (CRC-32 del objeto de 0xF1C0 bytes, arena fuera del objeto, region de globales de 0x70 bytes, EAX = this,
ECX = fs:[0] de entrada, EDX = 0) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, region de
globales, x87).
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
COLLABORATORS = [0x821D30, 0x4650A0, 0x6040C0]
FAMILY_MINIMUM = oracle.CASES // 16
STACK_SEED_MINIMUM = oracle.CASES

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~40-50 % de margen. Clave = 'f0_<estadistica>'.
MINIMUMS = {
    'f0_arena_a5': 740,
    'f0_arena_cd': 740,
    'f0_arena_ff': 750,
    'f0_arena_noise': 6000,
    'f0_arena_noise5': 2100,
    'f0_arena_zero': 740,
    'f0_eax_zero': 1700,
    'f0_edx_ffff': 1800,
    'f0_g_80': 750,
    'f0_g_ff': 730,
    'f0_g_final': 730,
    'f0_g_inverse': 750,
    'f0_g_noise': 5200,
    'f0_g_zero': 750,
    'f0_globals_changed': 8000,
    'f0_late_offset': 3200,
    'f0_misaligned': 3100,
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
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado; EAX/ECX/EDX se exigen en observed')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b.get('set_default_hook_on') is not True:
        raise RuntimeError('El hook de CDecision::SetDefault (0x600530) no estaba encendido durante el banco')
    got = [(c['exe'], c['dll'], c['hooked']) for c in b['collaborators']]
    if got != [(a, 0, False) for a in COLLABORATORS]:
        raise RuntimeError('Colaboradores inesperados: ' + json.dumps(got))
    lay = b['layout']
    for key, want in oracle.LAYOUT.items():
        if lay[key] != want:
            raise RuntimeError('La arena del banco no coincide con las constantes del oraculo: %s' % key)
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
            if side['observed'].get('rest_ok') is not True:
                raise RuntimeError('Arena fuera del objeto alterada en %s %d' % (fn, row['case']))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, row['case'], row['fp_difference']))
        if row.get('candidate_exe_calls') != 0:
            raise RuntimeError('El candidato llamo a un colaborador del EXE en %s %d: %s' % (fn, row['case'], row.get('candidate_exe_calls')))
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
    if state[0]['region'] != b['initial_region']:
        raise RuntimeError('La region de globales del juego (banderas de grupo, ScriptReferenceIndex, m_IsActive, instancia) no se restauro')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('helpers67s', manifest['binaries']['asi'], analyze, 'Helpers67S', timeout=3600))
