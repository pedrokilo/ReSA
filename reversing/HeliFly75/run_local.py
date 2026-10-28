"""Compuertas del banco HeliFly75 (lote 75): CHeli::ProcessFlyingCarStuff (EXE 0x6C4E60, hook V), 21504 pares.

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

NAMES = ['CHeli::ProcessFlyingCarStuff']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C4E60}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE, o la DLL llama a su cuerpo sin hook RH (CMatrix::Attach)
STUBBED = {NAMES[0]: [(0x6D85F0, True), (0x59BD10, False), (0x7F1200, False), (0x7491F0, False), (0x6E2E50, True), (0x72C8B0, True), (0x4F6420, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C4E60, 0x871680 + 4 * 68}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_nan(b) and (b & 0x7FFFFF) != 0x400000


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    ev, _, _ = oracle.expected(inp)
    L = dict(oracle.LAST)
    k = [e[0] for e in ev]
    status = inp['status'] >> 3
    model = inp['model'] & 0xFFFF
    rc = model in (0x1D1, 0x1F5)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    alive = status in (0, 8, 3)
    add('status_player', status == 0)
    add('status_remote', status == 8)
    add('status_wrecked', status == 3)
    add('status_other', not alive)
    add('exit_no_handling_flag', not alive and not (inp['hand_flags'] & 0x2000000))
    add('handling_flag_set', not alive and bool(inp['hand_flags'] & 0x2000000))
    add('handling_flag_neighbours', not alive and (inp['hand_flags'] | 0x2000000) == 0xFFFFFFFF and not (inp['hand_flags'] & 0x2000000))
    add('engine_bit_cleared', not alive and bool(inp['hand_flags'] & 0x2000000) and bool(inp['b428'] & 0x10))
    rotor = inp['rotor']
    add('rotor_nan', is_nan(rotor))
    add('rotor_inf', is_special(rotor) and not is_nan(rotor))
    add('rotor_denormal', is_denormal(rotor))
    add('rotor_negzero', rotor == 0x80000000)
    add('rotor_negative', not is_nan(rotor) and (rotor >> 31) == 1 and rotor != 0x80000000)
    add('step_nan', is_nan(inp['step']))
    add('step_denormal', is_denormal(inp['step']))
    add('step_inf', is_special(inp['step']) and not is_nan(inp['step']))
    if not alive and (inp['hand_flags'] & 0x2000000):
        f = oracle.mul(oracle.load(inp['step']), oracle.load(0x3A102DE0))
        sub_ok = oracle.lt(f, oracle.load(rotor))
        add('decay_subtracts', sub_ok)
        add('decay_to_zero', not sub_ok)
        add('decay_result_denormal', sub_ok and oracle.is_tiny(oracle.sub(oracle.load(rotor), f)))
    if alive:
        add('alive_rotor_below_022', oracle.f_lt(rotor, 0x3E6147AE))
        add('alive_bit8_blocks_add', oracle.f_lt(rotor, 0x3E6147AE) and bool(inp['flags40'] & 0x100))
        add('alive_add_rc', oracle.f_lt(rotor, 0x3E6147AE) and not (inp['flags40'] & 0x100) and rc)
        add('alive_add_other', oracle.f_lt(rotor, 0x3E6147AE) and not (inp['flags40'] & 0x100) and not rc)
        add('flying_called', 1 in k)
        fl = [e for e in ev if e[0] == 1]
        add('flying_model2', bool(fl) and fl[0][2] == 2)
        add('flying_model6', bool(fl) and fl[0][2] == 6)
        add('flying_none_fast_rotor', oracle.f_gt(rotor, 0x3E19999A) and not fl)
        add('flying_rotor_slow', not oracle.f_gt(rotor, 0x3E19999A))
        add('flying_special_model', bool(inp['flags40'] & 0x8000000) and model in (0x1BF, 0x1A1))
        add('flying_contact_wheels_ge4', oracle.f_gt(rotor, 0x3E19999A) and not (inp['b42c'] & 0x80) and inp['b960'] >= 4)
        add('flying_accel_positive', oracle.f_gt(rotor, 0x3E19999A) and not (inp['b42c'] & 0x80) and oracle.f_gt(inp['ctl3'], 0))
        add('flying_velocity_branch', bool(fl) and fl[0][2] == 6 and inp['b960'] >= 4 and not oracle.f_gt(inp['ctl3'], 0))
        add('flying_ctl_special', bool(fl) and any(is_special(inp['ctl%d' % i]) for i in range(4)))
        add('flying_ctl_noncanonical_nan', bool(fl) and any(noncanonical_nan(inp['ctl%d' % i]) for i in range(4)))
        add('blade_node_absent', oracle.f_gt(rotor, 0x3C75C290) and not inp['node_present'])
        add('blade_node_present', 2 in k and 3 in k)
        add('blade_atomic_absent', 3 in k and not inp['atomic'])
        add('blade_atomic_present', 3 in k and bool(inp['atomic']))
        add('blade_resync_called', 4 in k)
        add('blade_resync_skipped', 3 in k and bool(inp['atomic']) and not (inp['atom_flags'] & 2))
        add('blade_collision_called', 5 in k)
        bl = [e for e in ev if e[0] == 5]
        add('blade_radius_not_above', 3 in k and bool(inp['atomic']) and not bl and status in (0, 8))
        add('blade_status_wrecked_no_collision', 3 in k and bool(inp['atomic']) and status == 3 and not bl)
        add('blade_scale_rc', bool(bl) and bl[0][8] == 0x3F666666)
        add('blade_scale_08', bool(bl) and bl[0][8] == 0x3F4CCCCD)
        add('blade_scale_05', bool(bl) and bl[0][8] == 0x3F000000)
        add('blade_scale_1', bool(bl) and bl[0][8] == 0x3F800000)
        add('blade_radius_nan', 3 in k and bool(inp['atomic']) and is_nan(inp['radius_after'] if inp['atom_flags'] & 2 else inp['radius']))
        add('blade_radius_special', 3 in k and bool(inp['atomic']) and is_special(inp['radius_after'] if inp['atom_flags'] & 2 else inp['radius']))
        add('blade_matrix_absent', bool(bl) and not inp['mat_present'])
        add('blade_matrix_present', bool(bl) and bool(inp['mat_present']))
        wl = [e for e in ev if e[0] == 6]
        add('wind_called', bool(wl))
        add('wind_power_clamped', bool(wl) and wl[0][5] == 0x3F800000)
        add('wind_power_t', bool(wl) and wl[0][5] != 0x3F800000)
        add('wind_status_wrecked', bool(wl) and status == 3)
        add('wind_matrix_absent', bool(wl) and not inp['mat_present'])
        add('wind_skipped_slow_rotor', 3 in k and not wl and status in (0, 3))
    if L.get('tail'):
        add('tail_reached', True)
        add('tail_near_fail', not L['near'])
        add('tail_near_ok', L['near'])
        add('tail_diff_fail', L['near'] and not L['diff'])
        add('tail_diff_ok', L['near'] and L['diff'])
        add('tail_multiply_reached', L.get('reached', False))
        add('tail_fire', L.get('fire', False))
        add('tail_no_fire', L.get('reached', False) and not L.get('fire', False))
        add('tail_camera_matrix', bool(inp['cam_mat']))
        add('tail_camera_compact', not inp['cam_mat'])
        add('tail_heli_matrix_absent', not inp['mat_present'])
        add('tail_pos_special', any(is_special(inp['pos%d' % i]) or is_special(inp['cam%d' % i]) for i in range(3)))
        add('tail_pos_nan_noncanonical', any(noncanonical_nan(inp['pos%d' % i]) or noncanonical_nan(inp['cam%d' % i]) for i in range(3)))
        add('tail_pos_denormal', any(is_denormal(inp['pos%d' % i]) or is_denormal(inp['cam%d' % i]) for i in range(3)))
        add('tail_matrix_special', L.get('reached', False) and any(is_special(inp['mat%d' % i]) for i in range(12) if i % 4 != 3))
        add('tail_axis_special', L.get('reached', False) and any(is_special(inp['frame%d' % i]) for i in range(3)))
        add('tail_audio_event', 7 in k)
    add('tail_rc_model_skips', rc and oracle.f_lt(rotor, 0x3E1DB22D) and oracle.f_gt(rotor, 0x3B902DE0))
    add('tail_rotor_out_of_band', not rc and not (oracle.f_lt(rotor, 0x3E1DB22D) and oracle.f_gt(rotor, 0x3B902DE0)))
    add('tail_node_absent', not rc and oracle.f_lt(rotor, 0x3E1DB22D) and oracle.f_gt(rotor, 0x3B902DE0) and not inp['node_present'])
    add('no_events', not ev)


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
        # los colaboradores con hook RH tienen su cuerpo de la DLL parcheado con el mismo falso (y CMatrix::Attach tambien: la DLL llama a su cuerpo)
        if any((s['hooked'] or s['exe'] == 0x59BD10) and s['dll'] is not True for s in col[fn]['stubbed']):
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
        sys.exit(runtime_runner.run_case('helifly75', manifest['binaries']['asi'], analyze, 'HeliFly75', timeout=3600))
