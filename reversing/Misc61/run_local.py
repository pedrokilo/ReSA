"""Compuertas independientes del banco Misc61 (lote 61): CMemoryHeap::IntegrityCheck (0x72E8E0), CSearchLight::IsSpottedEntity (0x493900), CTaskSimpleSlideToCoord::ProcessPed (0x66C4E0, virtual)
y CTrainNode::GetLightingFromCollision (0x6F5F80), 4 x 4096 = 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos con hook sean saltos a la DLL en ambos lados y los
sin hook (0x493280) codigo del EXE), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios en las arenas, AL de salida y, en
IntegrityCheck, el orden de primera lectura de cada pagina con PAGE_GUARD) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion
final (hooks, entradas vigiladas, x87, CTimer). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME_HEAP: 0x72E8E0, oracle.NAME_SPOT: 0x493900, oracle.NAME_SLIDE: 0x66C4E0, oracle.NAME_NODE: 0x6F5F80}
# colaboradores sustituidos por funcion: (direccion del EXE, tiene hook en la DLL)
STUBBED = {
    oracle.NAME_HEAP: [],
    oracle.NAME_SPOT: [(0x4839A0, True), (0x493280, False)],
    oracle.NAME_SLIDE: [(0x61BF20, True), (0x62F370, True), (0x53CB50, True)],
    oracle.NAME_NODE: [(0x5674E0, True)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.NAME_HEAP: {
        'blocks0': 331, 'blocks_many': 644, 'cs_many': 363, 'cs_nodes': 890, 'free0': 349, 'free_block_garbage_memid': 626, 'free_many': 684, 'has_cs': 1024, 'inuse_noncanon': 1229,
        'memid_m1': 1007, 'memid_valid': 1417, 'no_cs': 1024, 'order_not_ascending': 910, 'prev0': 512, 'prev1': 434, 'prev2': 1024, 'twopage_block': 894,
    },
    oracle.NAME_SPOT: {
        'al_noncanon': 571, 'al_zero_upper_nonzero': 349, 'idx_min': 89, 'idx_minus1': 92, 'idx_zero': 101, 'matrix': 1013, 'no_matrix': 1034, 'path_false': 685, 'path_neg': 687,
        'path_true': 675, 'pos_den': 624, 'pos_inf': 354, 'pos_nan': 644, 'pos_snan': 230,
    },
    oracle.NAME_SLIDE: {
        'anim_noncanon': 197, 'anim_upper_garbage_zero': 366, 'any_den': 772, 'any_inf': 255, 'any_nan': 238, 'any_snan': 76, 'arg_den': 32, 'bits_high_set': 2010,
        'branch_far': 1077, 'branch_nan': 47, 'branch_near': 923, 'first': 1024, 'invalid_intel_unused': 801, 'lim_edge': 269, 'lim_nan': 37, 'matrix': 1551,
        'no_matrix': 496, 'not_first': 1024, 'not_player': 1140, 'not_running': 1024, 'now_edge': 399, 'path_angle_false': 152, 'path_angle_true': 402, 'path_anim_fail': 337,
        'path_not_reached': 665, 'path_timeout': 491, 'player': 907, 'player_first': 449, 'running': 1024, 'shift_den': 75, 'shift_inf': 158, 'shift_nan': 143,
        'shift_nonzero': 1058, 'special_inf': 265, 'special_nan': 244, 'timer_eq_now': 268, 'timer_set_2000': 404, 'timer_set_500': 229, 'timer_unset': 801,
        'timer_unset_anim_fail': 167, 'timer_wrap': 92,
    },
    oracle.NAME_NODE: {
        'col_noncanon': 512, 'col_upper_garbage': 771, 'coord_extreme': 538, 'coord_neg': 1795, 'found_noncanon': 433, 'light_0x48': 127, 'light_changed': 988,
        'path_cached': 512, 'path_col': 1024, 'path_nocol': 512,
    },
}
for _fn in NAMES:
    for _k in range(16):
        MINIMUMS[_fn]['family%d' % _k] = 256
    MINIMUMS[_fn]['stack_seed'] = 2048
for _k in range(8):
    MINIMUMS[oracle.NAME_SLIDE]['regime%d' % _k] = 200


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
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if col[fn]['dll_real'] or [(s['exe'], s['hooked']) for s in stubbed] != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        for s in stubbed:
            if s['hooked'] and not s['dll']:
                raise RuntimeError('Colaborador con hook sin cuerpo de la DLL en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
            if cov[fn].get(key, 0) < m:
                shortfalls['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('misc61', manifest['binaries']['asi'], analyze, 'Misc61', timeout=3600))
