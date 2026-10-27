"""Compuertas del banco PlayerCrosshair74 (lote 74): CPlayerInfo::ProcessCrosshair (EXE 0x56EC80, hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CPlayerInfo::ProcessCrosshair']
CASES = 20480   # 20 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x56EC80}
# (direccion del EXE, hooked): GetSteeringLeftRight, GetSteeringUpDown, GetCarGunFired, CamShakeNoPos, FindCamFOV, FireInstantHit (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x53FB80, False), (0x53FBD0, False), (0x53FF90, False), (0x50A970, False), (0x50AD20, False), (0x73FB10, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x56EC80}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('tx', 'ty', 'ts', 'fov1', 'fov2', 'asp', 'rx', 'ry', 'rz', 'fx', 'fy', 'fz', 'ux', 'uy', 'uz', 'px', 'py', 'pz', 'cpos_x', 'cpos_y', 'cpos_z', 'ppos_x', 'ppos_y', 'ppos_z')


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    active = (inp['active'] & 0xFF) != 0

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('inactive', not active)
    add('inactive_garbage_hi', not active and (inp['active'] >> 8) != 0)
    add('active', active)
    add('active_byte_gt1', active and (inp['active'] & 0xFF) > 1)
    fired = 6 in k
    add('no_fire', active and not fired)
    add('fire', fired)
    add('gun_hi_garbage_no_fire', active and not fired and (inp['gun'] >> 16) != 0)
    add('gun_hi_garbage_fire', fired and (inp['gun'] >> 16) != 0)
    add('gun_negative_ax', fired and (inp['gun'] & 0x8000) != 0)
    add('player0', fired and inp['player'] == 0)
    add('player1', fired and inp['player'] == 1)
    add('invert_zero', active and (inp['inv'] & 0xFF) == 0)
    add('invert_one', active and (inp['inv'] & 0xFF) == 1)
    add('invert_other', active and (inp['inv'] & 0xFF) > 1)
    add('cam_matrix', fired and inp['has_cam_mat'] == 1)
    add('cam_compact', fired and inp['has_cam_mat'] == 0)
    if active:
        want = oracle.expected(dict(inp, cpos=[inp['cpos_x'], inp['cpos_y'], inp['cpos_z']], ppos=[inp['ppos_x'], inp['ppos_y'], inp['ppos_z']]))
        cross = want[1]
        add('cross_x_nan', cross[0] is None)
        add('cross_y_nan', cross[1] is None)
        add('cross_x_clamped_hi', cross[0] == 0x3F666666 and not oracle.cmp_bits(inp['tx'], 0x3F666666))
        add('cross_x_clamped_lo', cross[0] == 0xBF666666 and not oracle.cmp_bits(inp['tx'], 0xBF666666))
        add('cross_y_clamped_hi', cross[1] == 0x3F666666)
        add('cross_y_clamped_lo', cross[1] == 0xBF666666)
        add('steer_neg_min', oracle.s16(inp['lr']) == -32768 or oracle.s16(inp['ud']) == -32768)
        add('steer_pos_max', oracle.s16(inp['lr']) == 32767 or oracle.s16(inp['ud']) == 32767)
        add('steer_zero', oracle.s16(inp['lr']) == 0 or oracle.s16(inp['ud']) == 0)
        add('ts_special', is_special(inp['ts']))
        add('ts_zero', inp['ts'] & 0x7FFFFFFF == 0)
        add('ts_denormal', is_denormal(inp['ts']))
        add('cross_special_in', is_special(inp['tx']) or is_special(inp['ty']))
        add('cross_denormal_in', is_denormal(inp['tx']) or is_denormal(inp['ty']))
        add('cross_negzero_in', inp['tx'] == 0x80000000 or inp['ty'] == 0x80000000)
        add('nan_noncanonical', any(noncanonical_nan(inp[a]) for a in FLOATS))
        add('special_any', any(is_special(inp[a]) for a in FLOATS))
        add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
        add('zero_input_signed', any(inp[a] == 0x80000000 for a in FLOATS))
    if fired:
        add('fov_special', is_special(inp['fov1']) or is_special(inp['fov2']))
        add('fov_zero', inp['fov1'] & 0x7FFFFFFF == 0 or inp['fov2'] & 0x7FFFFFFF == 0)
        add('fov_differ', inp['fov1'] != inp['fov2'])
        add('asp_special', is_special(inp['asp']))
        add('asp_zero', inp['asp'] & 0x7FFFFFFF == 0)
        add('matrix_special', any(is_special(inp[a]) for a in FLOATS[6:18]))
        add('matrix_denormal', any(is_denormal(inp[a]) for a in FLOATS[6:18]))
        add('pos_special', any(is_special(inp[a]) for a in FLOATS[18:24]))
        fire = [e for e in events if e[0] == 6][0]
        aim = fire[16:19] + fire[21:24]
        add('aim_nan', any(is_special(p) and (p & 0x7FFFFF) != 0 for p in aim))
        add('aim_inf', any(is_special(p) and (p & 0x7FFFFF) == 0 for p in aim))
        add('aim_denormal', any(is_denormal(p) for p in aim))
        add('aim_negzero', any(p == 0x80000000 for p in aim))
        add('flags_bit25_set_before', (inp['flags'] >> 25) & 1 == 1)
        add('flags_bit25_clear_before', (inp['flags'] >> 25) & 1 == 0)
        add('fake_toggles_bit25', (inp['fire_xor'] >> 25) & 1 == 1)
        add('fake_toggles_other', (inp['fire_xor'] & ~0x02000000) != 0)


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
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 20 or short:
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
        sys.exit(runtime_runner.run_case('playercrosshair74', manifest['binaries']['asi'], analyze, 'PlayerCrosshair74', timeout=3600))
