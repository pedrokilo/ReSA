"""Compuertas del banco CarPolice75 (lote 75): CCarCtrl::CreatePoliceChase (EXE 0x42C2B0, hook S), 21504 pares.

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

NAMES = ['CCarCtrl::CreatePoliceChase']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x42C2B0}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE, a su propia funcion sin hook o no hay hook
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x421980, True), (0x421440, True), (0x420A10, True), (0x5674E0, True), (0x534540, True), (0x420D40, False), (0x568B80, True), (0x4217C0, True), (0x59BBC0, False),
                      (0x4241C0, True), (0x6D2330, True), (0x563220, True), (0x6BEEB0, True), (0x6AF420, True), (0x41C070, True), (0x61A5A0, True), (0x63CB10, True), (0x681AF0, False), (0x4AB420, True),
                      (0x571B70, True), (0x571A00, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x42C2B0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    oracle.expected(inp)
    L = oracle.LAST['L']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    path = L['path']
    add('path_' + path)
    ped = oracle.sgn32(inp['ped_type'])
    gz = (inp['gangs'] & 0x7FFFFFFF) == 0
    add('gangs_zero', gz)
    add('gangs_nonzero', not gz)
    add('gangs_nan', is_nan(inp['gangs']))
    add('gangs_negzero', inp['gangs'] == 0x80000000)
    add('gangs_denormal', is_denormal(inp['gangs']))
    add('ped_in_14_23', 14 <= ped <= 23)
    add('ped_below_14', ped < 14)
    add('ped_above_23', ped > 23)
    add('ped_boundary', ped in (13, 14, 23, 24))
    add('roll_applied', L.get('roll'))
    add('roll_rand_8191', L.get('roll') and (inp['rnd'] & 0xFFFF) == 8191)
    add('roll_rand_8192', L.get('roll') and (inp['rnd'] & 0xFFFF) == 8192)
    add('roll_rand_garbage_high', L.get('roll') and (inp['rnd'] >> 16) != 0)
    m = oracle.sgn32(inp['model'])
    add('model_negative', m < 0)
    add('model_zero', m == 0)
    add('loaded_not_one', m >= 0 and (inp['loaded'] & 0xFF) != 1)
    add('loaded_byte_gt1', m >= 0 and (inp['loaded'] & 0xFF) > 1)
    add('vehicle_null', path == 'vehicle_null')
    add('hit1_only', L.get('hit1') and not L.get('hit2'))
    add('hit2_only', L.get('hit2') and not L.get('hit1'))
    add('hit_both', L.get('hit1') and L.get('hit2'))
    add('hit_none', 'hit1' in L and not L['hit1'] and not L['hit2'])
    add('hit_garbage_high', 'hit1' in L and ((inp['hit1'] >> 8) != 0 or (inp['hit2'] >> 8) != 0))
    add('second_nearer', L.get('second_nearer'))
    add('second_not_nearer', L.get('hit2') and not L.get('second_nearer'))
    add('ground_nan', 'ground' in L and is_nan(L['ground']))
    add('ground_special', 'ground' in L and is_special(L['ground']))
    add('ground_1e9', L.get('ground') == 0x4E6E6B28)
    add('path_no_ground', path == 'no_ground')
    add('zp_nan', 'zp' in L and L['zp'] is None)
    add('height_special', 'zp' in L and is_special(inp['height']))
    add('height_nan_noncanonical', 'zp' in L and noncanonical_nan(inp['height']))
    add('two_nan_sum', 'zp' in L and is_nan(inp['height']) and is_nan(L['ground']))
    add('sphere_checked', 'onscreen' in L and not L['onscreen'])
    add('onscreen', L.get('onscreen'))
    add('visible_abort', path == 'visible')
    add('colliding_abort', path == 'colliding')
    add('colliding_garbage_only', path != 'colliding' and 'zp' in L and (inp['colliding'] >> 16) != 0 and (inp['colliding'] & 0xFFFF) == 0)
    add('created', path == 'created')
    add('created_bike', path == 'created' and oracle.sgn32(inp['ptype']) == 9)
    add('created_not_bike', path == 'created' and oracle.sgn32(inp['ptype']) != 9)
    add('task_null', path == 'created' and inp['task_null'])
    add('cruise_wrap', path == 'created' and L.get('cruise_wrap'))
    add('cruise_246', path == 'created' and inp['cruise'] == 246)
    add('template_matrix', path == 'created' and inp['tmat'] == 1)
    add('template_no_matrix', path == 'created' and inp['tmat'] == 0)
    sp = L.get('speed', [])
    add('speed_nan', any(x is None for x in sp))
    add('speed_unknown_trig', any(x == oracle.UNKNOWN for x in sp))
    add('forward_special', path == 'created' and inp['tmat'] == 1 and any(is_special(inp[k]) for k in ('fwd_x', 'fwd_y', 'fwd_z')))
    add('forward_denormal', path == 'created' and inp['tmat'] == 1 and any(is_denormal(inp[k]) for k in ('fwd_x', 'fwd_y', 'fwd_z')))
    add('heading_special', path == 'created' and inp['tmat'] == 0 and is_special(inp['head']))
    add('passengers_full', path == 'created' and inp['pcount'] == 8)
    add('passengers_none', path == 'created' and inp['pcount'] == 0)
    add('passengers_some_null', path == 'created' and inp['pcount'] > 0 and (inp['pmask'] & ((1 << inp['pcount']) - 1)) != (1 << inp['pcount']) - 1)
    add('node_pos_special', 'hit1' in L and any(is_special(inp[k]) for k in ('node_x', 'node_y', 'node_z')))
    add('node_area_high', inp['area'] >= 32)
    add('node_id_high', inp['node'] >= 10)
    add('radius_special', path in ('visible', 'colliding', 'created') and is_special(inp['radius']))


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
        sys.exit(runtime_runner.run_case('carpolice75', manifest['binaries']['asi'], analyze, 'CarPolice75', timeout=3600))
