"""Compuertas del banco HeliGen74 (lote 74): CHeli::GenerateHeli (EXE 0x6C6520, hook S), 21504 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la igualdad de ambos lados con el oraculo independiente (fixture_oracle.py: modelo Python de la funcion, con aritmetica x87 a 24 bits y memoria de las arenas),
la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CHeli::GenerateHeli']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C6520}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE o no hay hook
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x6E2D50, True), (0x6C4190, True), (0x56E210, True), (0x569660, True), (0x59AF40, False), (0x563220, True), (0x6D1A50, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C6520}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('base_x', 'base_y', 'base_z', 'gz')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    oracle.expected(inp)
    L = oracle.LAST

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    news = inp['news'] & 0xFF
    add('news_true', news != 0)
    add('news_false', news == 0)
    add('news_byte_gt1', news > 1)
    add('news_garbage_high', (inp['news'] >> 8) != 0)
    add('news_garbage_high_false', (inp['news'] >> 8) != 0 and news == 0)
    add('target_matrix', inp['tgt_mat'] == 1)
    add('target_compact', inp['tgt_mat'] == 0)
    add('rnd_low_byte_0', (inp['rnd'] & 0xFF) == 0)
    add('rnd_low_byte_255', (inp['rnd'] & 0xFF) == 255)
    add('rnd_high_garbage', (inp['rnd'] >> 8) != 0)
    add('adjust_taken', L['out'])
    add('adjust_not_taken', not L['out'])
    px, py = L['px'], L['py']
    add('out_px_low', px is not None and oracle.lt(oracle.load(px), oracle.load(oracle.A90)))
    add('out_px_high', px is not None and oracle.gt(oracle.load(px), oracle.load(oracle.A94)))
    add('out_py_low', py is not None and oracle.lt(oracle.load(py), oracle.load(oracle.A90)))
    add('out_py_high', py is not None and oracle.gt(oracle.load(py), oracle.load(oracle.A94)))
    add('first_xy_nan_not_adjusted', not L['out'] and (px is None or py is None))
    add('boundary_exact_3000', px in (0x453B8000, 0xC53B8000) or py in (0x453B8000, 0xC53B8000))
    add('near_chiliad', L['near'])
    add('not_near_chiliad', not L['near'])
    dist = oracle.sqrt(oracle.add(oracle.mul(L['d2'], L['d2']), oracle.mul(L['d1'], L['d1'])))
    add('near_dist_nan', dist == oracle.NAN)
    add('near_boundary_exact_350', oracle.num(dist) == 350)
    add('near_boundary_inside_ulp', oracle.num(dist) is not None and oracle.num(dist) != float('inf') and 349.99 < oracle.num(dist) < 350)
    add('near_boundary_outside_ulp', oracle.num(dist) is not None and oracle.num(dist) != float('inf') and 350 < oracle.num(dist) < 350.01)
    base_z = oracle.load(inp['base_z'])
    if L['near']:
        lifted = oracle.add(base_z, oracle.load(oracle.C200))
        add('lifted_above_560', oracle.gt(lifted, oracle.load(oracle.C560)))
        add('lifted_equal_560', oracle.num(lifted) == 560)
        add('lifted_below_560', oracle.lt(lifted, oracle.load(oracle.C560)))
        add('lifted_nan', lifted == oracle.NAN)
    low = L['low']
    cand = L['cand']
    add('z_candidate_above_ground', oracle.gt(cand, low))
    add('z_ground_above_candidate', not oracle.gt(cand, low))
    add('z_candidate_equal_ground', oracle.num(cand) is not None and oracle.num(cand) == oracle.num(low))
    add('candidate_nan', cand == oracle.NAN)
    add('ground_low_nan', low == oracle.NAN)
    add('ground_low_inf', low != oracle.NAN and low[0] == 'i')
    add('z_nan', L['z'] is None)
    add('x_nan', L['x'] is None)
    add('y_nan', L['y'] is None)
    for nm in ('base_x', 'base_y', 'base_z', 'gz'):
        add('special_' + nm, is_special(inp[nm]))
        add('denormal_' + nm, is_denormal(inp[nm]))
        add('nan_noncanonical_' + nm, noncanonical_nan(inp[nm]))
    add('negzero_input', any(inp[a] == 0x80000000 for a in FLOATS))
    add('special_any', any(is_special(inp[a]) for a in FLOATS))
    add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('base_huge', any(oracle.num(oracle.load(inp[a])) is not None and abs(oracle.num(oracle.load(inp[a]))) > 1000000 for a in ('base_x', 'base_y')))
    add('base_tiny', any(oracle.num(oracle.load(inp[a])) is not None and 0 < abs(oracle.num(oracle.load(inp[a]))) < 1e-30 for a in ('base_x', 'base_y')))


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
    if b.get('layout_ok') is not True:
        raise RuntimeError('La disposicion de los campos de bits / accesores no es la esperada')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn + ': ' + str(got))
        # los colaboradores con hook RH tienen su cuerpo de la DLL parcheado con el mismo falso
        if any(s['hooked'] and s['dll'] is not True for s in col[fn]['stubbed']):
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
        why, _ = oracle.check_row(row)
        if why:
            raise RuntimeError('El oraculo independiente discrepa en %s %d: %s' % (fn, case, why[:3]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
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
        sys.exit(runtime_runner.run_case('heligen74', manifest['binaries']['asi'], analyze, 'HeliGen74', timeout=3600))
