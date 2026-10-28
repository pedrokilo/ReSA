"""Compuertas del banco CarNodes75 (lote 75): CCarCtrl::FindNodesThisCarIsNearestTo (EXE 0x42BD20, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas
medida sobre las entradas y el modelo del oraculo (coverage.json = minimos).
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

NAMES = ['CCarCtrl::FindNodesThisCarIsNearestTo']
CASES = 18432   # 18 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x42BD20}
# (direccion del EXE, hooked): CPathFind::FindXRegionForCoors / FindYRegionForCoors / FindXCoorsForRegion / FindYCoorsForRegion, CCollision::DistToLine y CVector::Normalise
# (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x44D890, False), (0x44D8C0, False), (0x44D8F0, False), (0x44D910, False), (0x417610, False), (0x59C910, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x42BD20}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


FLOATS = ('mpos_x', 'mpos_y', 'mpos_z', 'ppos_x', 'ppos_y', 'ppos_z', 'fwd_x', 'fwd_y', 'fwd_z', 'xcoord', 'ycoord')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo (oracle.LAST lo dejo la ultima comprobacion del mismo caso)"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    last = oracle.LAST
    add('has_mat', inp['has_mat'] == 1)
    add('no_mat', inp['has_mat'] == 0)
    for nm in FLOATS:
        add('special_' + nm, is_special(inp[nm]))
        add('denormal_' + nm, is_denormal(inp[nm]))
        add('nan_noncanonical_' + nm, noncanonical_nan(inp[nm]))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('zero_signed_input', any(inp[a] == 0x80000000 for a in FLOATS))
    rx, ry = oracle.s32(inp['region_x']), oracle.s32(inp['region_y'])
    add('region_negative', rx < 0 or ry < 0)
    add('region_above_7', rx > 7 or ry > 7)
    add('region_wrap', rx in (-2 ** 31, 2 ** 31 - 1) or ry in (-2 ** 31, 2 ** 31 - 1))
    add('range_empty_x', last['min_x'] > last['high_x'])
    add('range_empty_y', last['min_x'] <= last['high_x'] and last['min_y'] > last['high_y'])
    add('range_wide_x', last['high_x'] > last['min_x'])
    add('range_wide_y', last['high_y'] > last['min_y'])
    add('range_clamp_min', last['min_x'] == 0 or last['min_y'] == 0)
    add('range_clamp_max', last['high_x'] == 7 or last['high_y'] == 7)
    add('scanned_0', last['scanned'] == 0)
    add('scanned_1', last['scanned'] == 1)
    add('scanned_2_to_4', 2 <= last['scanned'] <= 4)
    add('nodes_in_range_0', last['nodes_in_range'] == 0)
    add('nodes_in_range_some', last['nodes_in_range'] > 0)
    add('links_any', last['links'] > 0)
    add('links_valid_any', last['links_valid'] > 0)
    add('links_unloaded_target', last['links'] > last['links_valid'])
    add('links_over_10', last['links_valid'] > 10)
    add('best_changes_0', last['best_changes'] == 0)
    add('best_changes_1', last['best_changes'] == 1)
    add('best_changes_2_or_more', last['best_changes'] >= 2)
    add('found', obs['out1'][0] != 0xFFFF)
    add('not_found', obs['out1'][0] == 0xFFFF)
    add('out1_area_nonzero', obs['out1'][0] not in (0, 0xFFFF))
    add('out1_node_nonzero', obs['out1'][1] not in (None, 0))
    add('dist_special', any(is_special(v) for v in inp['dist']))
    add('dist_nan', any(is_nan(v) for v in inp['dist']))
    add('norm_special', any(is_special(v) for t in inp['norm'] for v in t))
    add('norm_nan', any(is_nan(v) for t in inp['norm'] for v in t))
    add('slot_unloaded', any(not s[1] for s in inp['slots']))
    add('count_nonpositive', any(oracle.s32(s[2]) <= 0 for s in inp['slots']))
    add('flags_high_nibble', any(n[4] & 0xF0 for s in inp['slots'] for n in s[4]))
    add('link_base_negative', any(n[3] < 0 for s in inp['slots'] for n in s[4]))
    add('score_ties', last['dists'] > 1 and len(set(inp['dist'])) == 1)
    add('dxr_low', oracle.lt(oracle.st(oracle.sub(oracle.load(inp['ppos_x'] if not inp['has_mat'] else inp['mpos_x']), oracle.load(inp['xcoord']))), oracle.K200))


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b.get('world_layout_ok') is not True:
        raise RuntimeError('La disposicion de ThePaths / CMatrix / CNodeAddress no es la esperada')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los seis tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
        if any(s['dll'] is not True for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
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
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
        for side in ('original', 'candidate'):
            why = oracle.check_side(row['input'], row[side]['observed'])
            if why:
                raise RuntimeError('El lado %s de %s %d difiere del oraculo: %s' % (side, fn, case, why))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 18 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('carnodes75', manifest['binaries']['asi'], analyze, 'CarNodes75', timeout=3600))
