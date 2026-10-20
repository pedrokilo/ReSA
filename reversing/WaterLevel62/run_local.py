"""Compuertas independientes del banco WaterLevel62 (lote 62): CWaterLevel::FillQuadsAndTrianglesList (0x6E7B30), GetWaterDepth (0x6EA960), RenderHighDetailWaterRectangle (0x6EB810),
RenderHighDetailWaterTriangle (0x6EDDC0) y TestLineAgainstWater (0x6E61B0), 5 x 4096 = 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos con hook sean saltos a la DLL en ambos lados y los
sin hook (0x6E5750, 0x6EA8A0, 0x6E91D0, 0x6E8780) codigo del EXE), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, resultados, salidas
bit a bit y atlas sin cambios) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87).
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

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME_FILL: 0x6E7B30, oracle.NAME_DEPTH: 0x6EA960, oracle.NAME_RECT: 0x6EB810, oracle.NAME_TRI: 0x6EDDC0, oracle.NAME_LINE: 0x6E61B0}
# colaboradores sustituidos por funcion: (direccion del EXE, tiene hook en la DLL)
STUBBED = {
    oracle.NAME_FILL: [(0x6E5750, False)],
    oracle.NAME_DEPTH: [(0x6E8580, True), (0x6EA8A0, False)],
    oracle.NAME_RECT: [(0x420C40, True), (0x6EA9F0, True), (0x6E91D0, False), (0x6E73A0, True), (0x6EC5D0, True)],
    oracle.NAME_TRI: [(0x420C40, True), (0x6E8780, False), (0x6ECF00, True)],
    oracle.NAME_LINE: [],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.NAME_FILL: {
        'events_many': 1274, 'events_some': 1686, 'events_zero': 361, 'kind_q': 1206, 'kind_t': 1062, 'minus1_index': 239, 'num_q_neg': 128, 'num_q_zero': 356,
        'num_t_neg': 96, 'num_t_zero': 609, 'quads_many': 1084, 'quads_none': 257, 'stack_seed': 945, 'trig_q': 183, 'trig_t': 195, 'trigger_fired': 284,
        'tris_none': 516,
    },
    oracle.NAME_DEPTH: {
        'depth_den': 30, 'depth_inf': 97, 'depth_nan': 115, 'depth_zero': 73, 'found_ground': 996, 'ground_nan': 66, 'ground_snan': 18, 'level_nan': 55,
        'level_unwritten': 87, 'level_written': 1438, 'no_ground': 529, 'out_alias': 979, 'out_in_pos': 331, 'out_null': 232, 'ret_a_false': 522, 'ret_a_true': 1525,
        'stack_seed': 945,
    },
    oracle.NAME_RECT: {
        'first_true': 1544, 'hx_eq_hy': 104, 'hx_neg': 75, 'hy_zero': 64, 'invisible': 345, 'mirror_noncanon': 805, 'mirror_off_invisible': 263, 'p_den': 1134,
        'p_inf': 824, 'p_nan': 59, 'ret_noncanon': 1642, 'small': 742, 'stack_seed': 945, 'via_mirror': 158, 'visible': 1702, 'wrap': 40, 'xsplit': 455, 'ysplit': 504,
    },
    oracle.NAME_TRI: {
        'first_true': 1474, 'hw_big': 585, 'hw_neg': 50, 'invisible': 422, 'mirror_noncanon': 767, 'p_den': 1288, 'p_inf': 894, 'p_nan': 79, 'radius_neg': 152,
        'ret_noncanon': 1566, 'small': 586, 'split': 1039, 'stack_seed': 945, 'tri_zero': 50, 'via_mirror': 151, 'visible': 1625,
    },
    oracle.NAME_LINE: {
        'blocks_many': 316, 'box_eq_x0': 21, 'box_eq_x1': 22, 'box_eq_y0': 23, 'box_eq_y2': 18, 'combo_tested': 855, 'crossings_many': 750, 'hit_combo': 345,
        'hit_outside': 188, 'hit_single': 351, 'in_grid': 1929, 'inf_input': 46, 'minus1_vertex': 207, 'nan_input': 47, 'no_blocks': 17, 'out_nan': 28,
        'out_written': 1646, 'outside_tested': 278, 'range_zero': 41, 'result_false': 1163, 'result_true': 884, 'row_wrap': 62, 'single_quad_tested': 886,
        'single_tri_skipped': 248, 'stack_seed': 945, 'z_zero': 19,
    },
}
for _fn in NAMES:
    for _k in range(16):
        MINIMUMS[_fn]['family%d' % _k] = 256


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
        for label, side in (('EXE', a), ('DLL', c)):
            want, stats = oracle.expected(fn, row['input'], case, side['observed'])
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
        for key in ('family',):
            if key in row['input']:
                k2 = '%s%d' % (key, row['input'][key])
                cov[fn][k2] = cov[fn].get(k2, 0) + 1
        cov[fn]['stack_seed'] = cov[fn].get('stack_seed', 0) + (1 if row['input']['stack_seed'] in (0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000) else 0)
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
        sys.exit(runtime_runner.run_case('waterlevel62', manifest['binaries']['asi'], analyze, 'WaterLevel62', timeout=3600))
