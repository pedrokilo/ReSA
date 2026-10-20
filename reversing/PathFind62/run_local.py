"""Compuertas independientes del banco PathFind62 (lote 62): seis funciones de CPathFind, 6 x 4096 = 24576 pares.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo)."""
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

NAMES = ['CPathFind::FindNodeClosestToCoors', 'CPathFind::FindNthNodeClosestToCoors', 'CPathFind::FindNextNodeWandering',
         'CPathFind::CompleteNewInterior', 'CPathFind::FindParkingNodeInArea', 'CPathFind::GeneratePedCreationCoors_Interior']
CASES = 4096
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x44F460, NAMES[1]: 0x44F8C0, NAMES[2]: 0x451B70, NAMES[3]: 0x452270, NAMES[4]: 0x4513F0, NAMES[5]: 0x44ECA0}
STUBBED = {
    NAMES[0]: [(0x44F2C0, False)],
    NAMES[1]: [(0x44F460, True)],
    NAMES[2]: [(0x44F460, True)],
    NAMES[3]: [(0x72F420, True), (0x450FB0, False), (0x44E000, True)],
    NAMES[4]: [],
    NAMES[5]: [(0x821B1E, False), (0x5696C0, True)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst if a != 0x44F460}
# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    NAMES[0]: {'calls_many': 959, 'edge_dist': 407, 'found': 1014, 'huge_pos': 187, 'interior': 1201, 'maxdist_default': 557, 'maxdist_nan': 80, 'nan_inf_pos': 164, 'nt_other': 1521, 'region_clamped': 1219, 'rings': 1105, 'skip_rings': 942, 'u6_garbage_hi': 1519},
    NAMES[1]: {'calls_many': 258, 'early_invalid': 1175, 'marked': 1304, 'nth_big': 464, 'nth_huge': 32, 'nth_neg': 117, 'nth_nonpos': 221, 'out_set': 895, 'type_other': 666, 'type_ped': 693, 'type_veh': 688},
    NAMES[2]: {'dir_q0': 344, 'dir_q1': 332, 'dir_q2': 353, 'dir_q3': 346, 'nlinks0': 202, 'no_node': 470, 'od0': 128, 'od1': 109, 'od2': 114, 'od3': 51, 'od4': 114, 'od5': 101, 'od6': 120, 'od7': 52, 'origin_unloaded': 201, 'picked': 680, 'same_node_swap': 269, 'search': 1031, 'stay': 816},
    NAMES[3]: {'building': 1024, 'built': 1994, 'empty': 53, 'ext': 1176, 'ext_many': 320, 'flood_ext': 1176, 'links': 1638, 'n_1': 294, 'n_64': 119, 'n_ge32': 329, 'neg_ext': 170, 'no_out': 513, 'symmetry': 1363, 'x_special': 825},
    NAMES[4]: {'cursor_hit_not_first': 560, 'found': 1170, 'many': 919, 'nan_box': 299, 'neg_cursor': 138, 'none': 877, 'selected': 731, 'wrap': 1281},
    NAMES[5]: {'fail': 1202, 'ground_called': 1072, 'ground_nan': 414, 'link_reached': 1072, 'many_attempts': 572, 'multi_try': 778, 'no_area': 208, 'rand_called': 1418, 'rand_mode0': 249, 'rand_mode1': 245, 'rand_mode2': 250, 'rand_mode3': 258, 'rand_mode4': 268, 'rand_mode5': 255, 'rand_mode6': 259, 'rand_mode7': 262, 'success': 846},
}
for _fn in NAMES:
    for _k in range(16):
        MINIMUMS[_fn]['family%d' % _k] = 256
    MINIMUMS[_fn]['stack_seed'] = 2048


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
    if b.get('sincos_mismatches') != 0:
        raise RuntimeError('fsin/fcos del x87 distinto de sin/cos de la DLL para algun valor de dir: %s' % b.get('sincos_mismatches'))
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
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', row['original']), ('DLL', row['candidate'])):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:' % (fn, case, label, diff) + '\n  obtenido ' + str({k: side['observed'].get(k) for k in diff}) + '\n  esperado ' + str({k: want[k] for k in diff}))
        stats['family%d' % row['input']['family']] = True
        stats['stack_seed'] = True
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
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['globals_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas o los globales vigilados no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('pathfind62', manifest['binaries']['asi'], analyze, 'PathFind62', timeout=3600))
