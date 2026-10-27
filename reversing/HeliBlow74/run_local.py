"""Compuertas del banco HeliBlow74 (lote 74): CHeli::BlowUpCar (EXE 0x6C6D30, hook V), 21504 pares.

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

NAMES = ['CHeli::BlowUpCar']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C6D30}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE o no hay hook
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x56E210, True), (0x56E0D0, True), (0x55C180, True), (0x7322F0, True), (0x6C25D0, True), (0x6B1350, True), (0x6B1600, True), (0x6A8580, True), (0x7F1200, False), (0x50A9F0, True), (0x6D1C80, True), (0x6D2330, True), (0x53A050, True), (0x43DF20, True), (0x736A50, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C6D30, 0x871680 + 4 * 41}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('pos_x', 'pos_y', 'pos_z', 'chase0', 'chase1')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    ev, _, g = oracle.expected(inp)
    L = oracle.LAST
    k = [e[0] for e in ev]
    model = inp['model'] & 0xFFFF
    rc = model in (0x1D1, 0x1F5)
    status = inp['status'] & 0xF8

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    can = bool(inp['flags42a'] & 0x20)
    add('exit_cannot_be_damaged', not can)
    add('can_damage_bit5_only_set', (inp['flags42a'] & 0xFF) == 0x20)
    add('can_damage_other_bits_clear_bit5', not can and (inp['flags42a'] & 0xDF) != 0)
    early = can and status != 0 and not (inp['mission'] & 0xFF == 0x3A or rc)
    add('exit_crash_and_burn', early)
    add('status_nonplayer_mission_3a_continues', can and status != 0 and (inp['mission'] & 0xFF) == 0x3A and 1 in k)
    add('status_nonplayer_rc_continues', can and status != 0 and rc and 1 in k)
    add('status_nonplayer_rc_mission_not_3a', can and status != 0 and rc and (inp['mission'] & 0xFF) != 0x3A)
    add('status_player_main', can and status == 0)
    add('main_reached', 1 in k)
    add('damager_is_ped', 1 in k and L['is_player'] and L['damager'] == L['ped_ret'])
    add('damager_is_vehicle', 1 in k and L['is_player'] and L['damager'] != L['ped_ret'])
    add('damager_null_equals_null', 1 in k and inp['dmg_kind'] == 0 and inp['fpp_kind'] == 1)
    add('damager_not_player', 1 in k and not L['is_player'])
    add('find_vehicle_called', 2 in k)
    add('find_vehicle_skipped', 1 in k and 2 not in k)
    add('damager_heli', 1 in k and inp['dmg_kind'] == 5)
    add('damager_raw', 1 in k and inp['dmg_kind'] == 4)
    add('player_focus_0', 4 in k and inp['focus'] == 0)
    add('player_focus_1', 4 in k and inp['focus'] == 1)
    if 4 in k:
        st = [e for e in ev if e[0] == 4][0]
        r = inp['rnd']
        add('rand_negative', r >= 0x80000000)
        add('rand_zero', r == 0)
        add('rand_large', 0x8000 <= r < 0x80000000)
        add('stat_amount_below_4000', st[2] is not None and oracle.load(st[2])[1] < 4000)
        add('havoc_wraps', g['havoc'][inp['focus']] < inp['havoc%d' % inp['focus']])
        c0 = inp['chase%d' % inp['focus']]
        add('chase_nan', is_special(c0) and (c0 & 0x7FFFFF) != 0)
        add('chase_noncanonical_nan', noncanonical_nan(c0))
        add('chase_denormal', is_denormal(c0))
        add('chase_inf', is_special(c0) and (c0 & 0x7FFFFF) == 0)
        add('chase_negzero', c0 == 0x80000000)
        add('chase_cancels_to_zero', c0 == 0xC1200000)
    add('news_model_1e8', 1 in k and model == 0x1E8)
    add('news_flag_cleared', 1 in k and model == 0x1E8 and inp['news'] != 0)
    add('news_flag_untouched_other_model', 1 in k and model != 0x1E8 and inp['news'] != 0)
    add('model_police_1f1', 1 in k and model == 0x1F1)
    add('model_rc_1d1', 1 in k and model == 0x1D1)
    add('model_rc_1f5', 1 in k and model == 0x1F5)
    add('model_neighbours', 1 in k and model in (0x1D2, 0x1F4, 0x1E9, 0x1E7, 0x11D1, 0x81F5))
    add('status0_clears_motion', 1 in k and status == 0)
    add('status0_flags_all_ones', 1 in k and status == 0 and inp['ent1c'] == 0xFFFFFFFF)
    add('non_rc_branch', 7 in k)
    add('rc_branch', 1 in k and 7 not in k)
    add('atomic_present', 10 in k and bool(inp['atomic']))
    add('atomic_absent', 10 in k and not inp['atomic'])
    if 1 in k:
        f = inp['flags428']
        add('ambulance_on_duty', bool(f & 2))
        add('firetruck_on_duty', bool(f & 4))
        add('both_on_duty', (f & 6) == 6)
        add('neither_on_duty', (f & 6) == 0)
        add('ambulance_counter_wraps', bool(f & 2) and inp['ambulances'] == 0)
        add('firetruck_counter_wraps', bool(f & 4) and inp['trucks'] == 0)
        add('engine_and_lights_set', (f & 0x50) == 0x50)
        add('engine_lights_clear', (f & 0x50) == 0)
        add('siren_set', bool(inp['flags42d'] & 0x80))
        add('taxi_light_set', bool(inp['b868'] & 1))
        add('bomb_bits_set', bool(inp['b4a8'] & 7))
        add('override_lights_set', bool(inp['b4a8'] & 0x18))
        add('b4a8_high_bits_set', bool(inp['b4a8'] & 0xE0))
        add('matrix_present', inp['mat_present'] == 1)
        add('matrix_absent', inp['mat_present'] == 0)
        add('explosion_type_7', 16 in k and not rc)
        add('explosion_type_12', 16 in k and rc)
        add('hide_arg_nonzero', inp['hide'] != 0)
        add('hide_arg_high_garbage', (inp['hide'] >> 8) != 0)
        add('time_zero', inp['time'] == 0)
        add('time_max', inp['time'] == 0xFFFFFFFF)
        for nm in ('pos_x', 'pos_y', 'pos_z'):
            add('special_pos', is_special(inp[nm]))
            add('denormal_pos', is_denormal(inp[nm]))
            add('nan_noncanonical_pos', noncanonical_nan(inp[nm]))
            add('negzero_pos', inp[nm] == 0x80000000)
        add('special_pos_with_matrix', inp['mat_present'] == 1 and any(is_special(inp[a]) for a in ('pos_x', 'pos_y', 'pos_z')))
        add('special_pos_compact', inp['mat_present'] == 0 and any(is_special(inp[a]) for a in ('pos_x', 'pos_y', 'pos_z')))


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
        sys.exit(runtime_runner.run_case('heliblow74', manifest['binaries']['asi'], analyze, 'HeliBlow74', timeout=3600))
