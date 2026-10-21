"""Compuertas independientes del banco Helpers65A (lote 65): CHud::FloorFloat (0x406CE0), RwHelper::GetAnimHierarchyFromFrameCallback (0x734A70), CDecisionMaker::MakeDecision (0x606060),
CDecisionMakerTypes::ReadEventIndices (0x5BB9F0) y UserTracksLegacyVector::PushBack (0x4F4620), 5 x 4096 = 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos no reconstruidos sean codigo original del EXE y que
los reconstruidos tengan su cuerpo en la DLL), fp_difference vacio, la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios de la arena de 4 KB palabra a palabra,
ST0 de FloorFloat, retorno del callback) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87).
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
HOOKS = {oracle.N_FLOOR: 0x406CE0, oracle.N_CALLBACK: 0x734A70, oracle.N_DECISION: 0x606060, oracle.N_READ: 0x5BB9F0, oracle.N_PUSH: 0x4F4620}
# colaboradores sustituidos por funcion: (direccion del EXE, reconstruido en la DLL con cuerpo propio)
STUBBED = {
    oracle.N_FLOOR: [(0x8219F0, False)],
    oracle.N_CALLBACK: [(0x7C5160, True), (0x7F0DC0, True)],
    oracle.N_DECISION: [(0x6040D0, True)],
    oracle.N_READ: [(0x5387D0, True), (0x538900, True), (0x5389B0, True), (0x5389D0, True), (0x8220AD, False)],
    oracle.N_PUSH: [(0x4F3B30, False), (0x4F4320, False)],
}
WATCHED = {a for fn in STUBBED.values() for a, _ in fn}
FAMILIES = 12

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.N_FLOOR: {'x_nan': 180, 'x_snan': 85, 'x_inf': 95, 'x_zero': 90, 'x_denormal': 180, 'x_neg': 1000, 'x_ge_2p23': 400, 'x_fraction': 800, 'ret_nan': 300, 'ret_inf': 250,
                     'ret_denormal': 270, 'ret_negzero': 250, 'ret_wide': 450},
    oracle.N_CALLBACK: {'found_written': 1000, 'null_return': 520, 'recursed': 1450, 'hier_at_root': 520, 'depth_ge3': 310, 'nodes_ge8': 650, 'rg_zero': 600, 'rg_nonzero': 1400,
                        'full_walk_none': 1000, 'found_unchanged': 50},
    oracle.N_DECISION: {'a_200': 1000, 'a_minus1': 1000, 'both_write': 480, 'facial_changed': 1000, 'facial_write': 1000, 'idx_negative': 400, 'idx_small': 1150, 'idx_wrap': 800,
                        'no_write': 480, 'src_negative': 500, 'task_changed': 1000, 'task_write': 1000, 'veh_canonical': 260, 'veh_low_nonzero_not_one': 1100,
                        'veh_low_zero_word_nonzero': 360, 'veh_noncanonical': 1700},
    oracle.N_READ: {'dup_index': 750, 'has_blank': 720, 'has_nul': 700, 'idx_ge96': 480, 'idx_neg': 500, 'junk_first': 800, 'lines0': 240, 'lines_ge8': 800, 'long_line': 210,
                    'name_empty': 380, 'name_long': 210, 'name_pedevent': 380, 'out_of_range_write': 850, 'parsed_ge1': 1700, 'parsed_ge5': 950, 'stale_index': 650, 'tok_null': 280},
    oracle.N_PUSH: {'cap_negative': 260, 'fast': 900, 'first_null': 160, 'fk1': 100, 'fk2': 110, 'fk3': 110, 'fk_fast': 160, 'fk_insert': 160, 'full': 350, 'insert': 1050,
                    'last_equals_first': 210, 'signed_flip': 150, 'size_gt_cap': 540, 'size_negative': 330, 'unaligned': 500, 'unsigned_flip': 140, 'val_null': 170},
}
for _m in range(8):
    MINIMUMS[oracle.N_FLOOR]['mode%d' % _m] = 230
FAMILY_MINIMUM = 256
STACK_SEED_MINIMUM = 2048


def check_minimums(cov):
    """cobertura medida {funcion: {clave: cuenta}} frente a MINIMUMS: devuelve {clave: (medido, minimo)} de lo que no llega"""
    short = {}
    for fn in NAMES:
        mins = dict(MINIMUMS[fn])
        for k in range(FAMILIES):
            mins['family%d' % k] = FAMILY_MINIMUM
        mins['stack_seed'] = STACK_SEED_MINIMUM
        for key, m in mins.items():
            if cov[fn].get(key, 0) < m:
                short['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    return short


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
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES) or b.get('arena_bytes') != 4096:
        raise RuntimeError('Numero de pares o tamano de la arena inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        got = [(s['exe'], s['dll'] != 0) for s in col[fn]['stubbed']]
        if got != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn + ': ' + json.dumps(got))
    if {w['address'] for w in b['watched_entries'] if w['address'] < 0x1000000} != WATCHED:
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
        cov[fn]['family%d' % row['input']['fam']] = cov[fn].get('family%d' % row['input']['fam'], 0) + 1
        cov[fn]['stack_seed'] = cov[fn].get('stack_seed', 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = check_minimums(cov)
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
        sys.exit(runtime_runner.run_case('helpers65a', manifest['binaries']['asi'], analyze, 'Helpers65A', timeout=3600))
