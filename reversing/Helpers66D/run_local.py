"""Compuertas independientes del banco Helpers66D: RwHelper::RpClumpGetBoundingSphere (0x734FC0), CGarage::FindDoorsWithGarage (0x449FF0), CCam::GetArrestCamCopSourceSide (0x516010) y
CHeli::CHeli (0x6C4190), 4 x 4096 = 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, colaboradores sustituidos (los esperados por funcion), todas las compuertas de
ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (traza de colaboradores, esfera, puertas, posicion de la camara, objeto, resto de la arena, guardas,
globales) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas de codigo vigiladas, globales del juego, x87).
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
# colaboradores sustituidos por funcion, en orden: (direccion del EXE, tiene cuerpo propio en la DLL)
STUBBED = {
    NAMES[0]: [(0x7498E0, True), (0x749B70, False), (0x7F0990, False), (0x7F2070, False), (0x7EDD90, False)],
    NAMES[1]: [],
    NAMES[2]: [(0x59C910, True), (0x59C730, True), (0x5696C0, True)],
    NAMES[3]: [(0x6B0A90, True), (0x6C21C0, True), (0x821B1E, False)],
}
FAMILY_MINIMUM = oracle.CASES // 16
STACK_SEED_MINIMUM = 2048
FUNC_OF = {name: i for i, name in enumerate(NAMES)}

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~40-50 % de margen. Clave = 'fN_<estadistica>'.
MINIMUMS = {
    'f0_clump_null': 101,
    'f0_cmod_set': 220,
    'f0_count_big': 114,
    'f0_count_nonpos': 163,
    'f0_count_rounded': 104,
    'f0_low_zero_high_nonzero': 357,
    'f0_ltm_on': 1153,
    'f0_ltm_raw_odd': 969,
    'f0_out_inf': 77,
    'f0_out_nan': 104,
    'f0_parent_garbage': 44,
    'f0_parent_null': 41,
    'f0_proceeded': 1640,
    'f0_rad_set': 1090,
    'f0_scaled_denormal': 228,
    'f0_scaled_negzero': 69,
    'f0_sphere_null': 144,
    'f0_sum_denormal_in': 236,
    'f1_corner_nan': 137,
    'f1_free_slot_with_match_byte': 899,
    'f1_mat0_used': 1295,
    'f1_mat1_used': 1299,
    'f1_nan_distance': 276,
    'f1_none_found': 542,
    'f1_null_object_skipped': 118,
    'f1_one_found': 195,
    'f1_rdata_garage': 128,
    'f1_rejected': 828,
    'f1_replaced_first': 717,
    'f1_second_set': 990,
    'f1_selected_ge3': 1088,
    'f1_size0': 261,
    'f1_two_found': 1310,
    'f1_wrong_byte_skipped': 708,
    'f2_early_ret': 235,
    'f2_found_noncanonical': 393,
    'f2_found_snap': 1165,
    'f2_found_written_zero': 245,
    'f2_mat1': 913,
    'f2_nrm_override': 1310,
    'f2_out_inf': 221,
    'f2_out_nan': 130,
    'f3_hunter': 1288,
    'f3_hunter_alias_low16': 18,
    'f3_hunter_alias_low8': 33,
    'f3_not_hunter': 759,
    'f3_rand_max': 99,
    'f3_rand_negative': 454,
    'f3_rand_zero': 44,
}


def check_minimums(cov):
    short = {}
    for fn in NAMES:
        f = FUNC_OF[fn]
        for k, m in MINIMUMS.items():
            if k.startswith('f%d_' % f) and cov.get(k, 0) < m:
                short[k] = (cov.get(k, 0), m)
        for fam in range(16):
            key = 'f%d_fam%d' % (f, fam)
            if cov.get(key, 0) < FAMILY_MINIMUM:
                short[key] = (cov.get(key, 0), FAMILY_MINIMUM)
        if cov.get('f%d_stack_seed' % f, 0) < STACK_SEED_MINIMUM:
            short['f%d_stack_seed' % f] = (cov.get('f%d_stack_seed' % f, 0), STACK_SEED_MINIMUM)
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
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != 4 * oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 4:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        got = [(s['exe'], s['dll'] != 0) for s in col[fn]]
        if got != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn + ': ' + json.dumps(got))
    lay = b['layout']
    for key, want in oracle.LAYOUT.items():
        if lay[key] != want:
            raise RuntimeError('La arena del banco no coincide con las constantes del oraculo: %s' % key)
    dv = b['dll_vptr']
    if not dv.get('in_asi') or dv['vptr'] in (0, 0x871680, oracle.BASE_MARKER):
        raise RuntimeError('vptr de la DLL sin capturar')
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
        if fn not in FUNC_OF:
            raise RuntimeError('Funcion inesperada %s' % fn)
        func = FUNC_OF[fn]
        key = (func, row['case'])
        if key in seen or row['input']['func'] != func or row['input']['n'] != row['case']:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, row['case']))
        seen.add(key)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, row['case'], {k: side.get(k) for k in GATES}))
            if side['observed'].get('guard_ok') is not True or side['observed'].get('rest_ok', True) is not True:
                raise RuntimeError('Guardas o resto de la arena alterados en %s %d' % (fn, row['case']))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, row['case'], row['fp_difference']))
        try:
            want, stats = oracle.expected(fn, row['input'], row['case'])
        except oracle.Ambiguous:
            raise RuntimeError('Caso ambiguo para el oraculo (dos NaN con el mismo significando y distinto signo) en %s %d' % (fn, row['case']))
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, row['case'], label, diff, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for k, v in stats.items():
            if k == 'family':
                bump('f%d_fam%d' % (func, v))
            else:
                bump('f%d_%s' % (func, k), bool(v))
        bump('f%d_stack_seed' % func)
    if seen != {(f, n) for f in range(4) for n in range(oracle.CASES)}:
        raise RuntimeError('Cobertura incompleta')
    shortfalls = check_minimums(cov)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != 4 * oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87 o las entradas vigiladas no se restauraron')
    if state[0]['globals'] != b['initial_globals']:
        raise RuntimeError('Los globales del juego (bUseLTM, pool de objetos, temporizador) no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('helpers66d', manifest['binaries']['asi'], analyze, 'Helpers66D', timeout=3600))
