"""Compuertas del banco PlaneFiring74 (lote 74): CVehicle::GetPlaneWeaponFiringStatus (EXE 0x6E3440, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores, escrituras de salida, EAX, vehiculo, mira y bengala),
la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
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

NAMES = ['CVehicle::GetPlaneWeaponFiringStatus']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6E3440}
# (direccion del EXE, hooked): FindPlayerSlotWithVehiclePointer, GetCarGunFired, CarGunJustDown, GetEnterTargeting, GetTarget, IsLockedOn, ScanAndMarkTargetForHeatSeekingMissile, AddProjectile
# (hooked=False: el hook queda apagado, la entrada del EXE es codigo original; todos tienen hook RH en la DLL y su cuerpo lleva el mismo falso)
STUBBED = {NAMES[0]: [(0x564000, False), (0x53FF90, False), (0x53FFE0, False), (0x5406B0, False), (0x540670, False), (0x742BD0, False), (0x6E0400, False), (0x737C80, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6E3440}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FLOATS = ('fwd_x', 'fwd_y', 'fwd_z', 'up_x', 'up_y', 'up_z', 'mpos_x', 'mpos_y', 'mpos_z')

KEYS = ['kind_hunter', 'kind_guns', 'kind_rustler', 'kind_hydra', 'kind_default', 'model_576', 'model_464', 'model_447', 'model_487', 'model_497', 'model_548', 'model_425',
        'model_below_range', 'model_above_range', 'model_negative_word', 'hunter_l1', 'hunter_circle', 'hunter_none', 'hunter_l1_over_circle',
        'guns_l1', 'guns_baron_extra_fire', 'guns_baron_flags_but_no_fire', 'guns_baron_no_flags', 'guns_non_baron_flags', 'rustler_l1', 'rustler_none',
        'hydra_flare', 'hydra_flare_edge_equal_nofire', 'hydra_flare_edge_plus1_fire', 'hydra_flare_wrap', 'hydra_jd0_one_but_early', 'hydra_jd0_not_one',
        'hydra_l1', 'hydra_l1_success', 'hydra_l1_fallback', 'hydra_l1_not_locked', 'hydra_l1_lockstart_zero', 'hydra_l1_target_changed', 'hydra_l1_target_null', 'hydra_l1_time_equal_1500', 'hydra_l1_time_1501',
        'hydra_l1_time_wrap', 'hydra_enter_targeting', 'hydra_target_hold_red', 'hydra_target_hold_white', 'hydra_target_lockstart_zero', 'hydra_target_changed', 'hydra_target_edge_1500',
        'hydra_target_edge_1501', 'hydra_clear', 'hydra_null_matrix', 'slot_zero', 'slot_one', 'slot_other', 'slot_wrap_pad',
        'pad_ret_hi_garbage', 'bool_ret_hi_garbage', 'bool_ret_gt1', 'status_untouched_neighbours', 'out_status_written', 'out_status_not_written', 'out_ord_written', 'out_ord_not_written',
        'target_returned', 'target_returned_garbage', 'flags_ab_both', 'flags_a_only', 'flags_b_only']
FLARE_KEYS = ['out_nan', 'out_inf', 'out_denormal', 'out_negzero', 'special_any', 'denormal_any', 'nan_noncanonical', 'nan_two_operands', 'nan_snan_in_position', 'zero_input_signed',
              'inter_tiny_t1', 'inter_tiny_t2', 'inter_tiny_s', 'sum_zero_cancel', 'sum_negzero']


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def snan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x400000) == 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    exp = oracle.expected(inp)
    k = exp['path']['kind']
    ev = [e[0] if isinstance(e, list) else 8 for e in exp['events']]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    model = inp['model'] & 0xFFFF
    add('kind_hunter', k == 0)
    add('kind_guns', k == 1)
    add('kind_rustler', k == 2)
    add('kind_hydra', k == 3)
    add('kind_default', k == 4)
    for m in (576, 464, 447, 487, 497, 548, 425):
        add('model_%d' % m, model == m)
    add('model_below_range', model < 425)
    add('model_above_range', 576 < model < 0x8000)
    add('model_negative_word', bool(model & 0x8000))
    gf0, gf1, jd0, jd1 = inp['gf0'] & 0xFFFF, inp['gf1'] & 0xFFFF, inp['jd0'] & 0xFFFF, inp['jd1'] & 0xFFFF
    if k == 0:
        add('hunter_l1', gf0 == 2)
        add('hunter_circle', gf0 != 2 and jd0 == 1)
        add('hunter_none', gf0 != 2 and jd0 != 1)
        add('hunter_l1_over_circle', gf0 == 2 and jd0 == 1)
    if k == 1:
        add('guns_l1', gf0 == 2)
        baron = model == 464
        flags = (inp['flag_a'] & 0xFF) != 0 and (inp['flag_b'] & 0xFF) != 0
        add('guns_baron_extra_fire', baron and flags and gf1 == 1)
        add('guns_baron_flags_but_no_fire', baron and flags and gf1 != 1)
        add('guns_baron_no_flags', baron and not flags)
        add('guns_non_baron_flags', (not baron) and flags)
        add('flags_ab_both', flags)
        add('flags_a_only', (inp['flag_a'] & 0xFF) != 0 and (inp['flag_b'] & 0xFF) == 0)
        add('flags_b_only', (inp['flag_a'] & 0xFF) == 0 and (inp['flag_b'] & 0xFF) != 0)
    if k == 2:
        add('rustler_l1', gf0 == 2)
        add('rustler_none', gf0 != 2)
    now, gun = inp['now'], inp['gun']
    lim = (gun + 2000) & 0xFFFFFFFF
    if k == 3:
        add('hydra_flare', exp['fired'])
        add('hydra_flare_edge_equal_nofire', jd0 == 1 and now == lim)
        add('hydra_flare_edge_plus1_fire', jd0 == 1 and now == ((lim + 1) & 0xFFFFFFFF) and exp['fired'])
        add('hydra_flare_wrap', exp['fired'] and gun + 2000 > 0xFFFFFFFF)
        add('hydra_jd0_one_but_early', jd0 == 1 and not exp['fired'])
        add('hydra_jd0_not_one', jd0 != 1)
        add('hydra_null_matrix', inp['has_mat'] == 0)
        ls = inp['lock_start']
        d = (now - ls) & 0xFFFFFFFF
        if jd1 == 2:
            add('hydra_l1')
            add('hydra_l1_success', exp['ordn'] == 2)
            add('hydra_l1_fallback', exp['ordn'] == 1)
            add('hydra_l1_not_locked', (inp['locked'] & 0xFF) == 0)
            add('hydra_l1_lockstart_zero', (inp['locked'] & 0xFF) != 0 and ls == 0)
            if (inp['locked'] & 0xFF) != 0 and ls != 0:
                add('hydra_l1_target_changed', inp['scan0'] != inp['lock_target'] and inp['scan0'] != 0)
                add('hydra_l1_target_null', inp['scan0'] == 0)
                add('hydra_l1_time_equal_1500', d == 1500)
                add('hydra_l1_time_1501', d == 1501)
                add('hydra_l1_time_wrap', now < ls)
        else:
            if (inp['et'] & 0xFF) != 0:
                add('hydra_enter_targeting')
            elif (inp['tg'] & 0xFF) != 0:
                add('hydra_target_lockstart_zero', ls == 0)
                changed = inp['scan0'] == 0 or inp['scan0'] != inp['lock_target']
                add('hydra_target_changed', changed)
                held = oracle.M32 & (now - (now if (ls == 0 or changed) else ls))
                add('hydra_target_hold_red', held > 0x5DC)
                add('hydra_target_hold_white', held <= 0x5DC)
                add('hydra_target_edge_1500', not changed and ls != 0 and d == 1500)
                add('hydra_target_edge_1501', not changed and ls != 0 and d == 1501)
            else:
                add('hydra_clear')
    slot = inp['slot']
    add('slot_zero', slot == 0)
    add('slot_one', slot == 1)
    add('slot_other', slot not in (0, 1))
    add('slot_wrap_pad', slot >= 0x1000)
    add('pad_ret_hi_garbage', any((inp[x] >> 16) != 0 for x in ('gf0', 'gf1', 'jd0', 'jd1')))
    add('bool_ret_hi_garbage', any((inp[x] >> 8) != 0 for x in ('et', 'tg', 'locked')))
    add('bool_ret_gt1', any((inp[x] & 0xFF) > 1 for x in ('et', 'tg', 'locked')))
    add('out_status_written', exp['status'] is not None)
    add('out_status_not_written', exp['status'] is None)
    add('out_ord_written', exp['ordn'] is not None)
    add('out_ord_not_written', exp['ordn'] is None)
    add('status_untouched_neighbours', exp['status'] is not None)
    add('target_returned', exp['ret'] != 0)
    add('target_returned_garbage', exp['ret'] >= oracle.GARBAGE)
    if exp['fired']:
        pos = [e for e in obs['events'] if e[0] == 8][0][3:6]
        add('out_nan', any(is_special(p) and (p & 0x7FFFFF) != 0 for p in pos))
        add('out_inf', any(is_special(p) and (p & 0x7FFFFF) == 0 for p in pos))
        add('out_denormal', any(is_denormal(p) for p in pos))
        add('out_negzero', any(p == 0x80000000 for p in pos))
        add('special_any', any(is_special(inp[a]) for a in FLOATS))
        add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
        add('nan_noncanonical', any(noncanonical_nan(inp[a]) for a in FLOATS))
        add('nan_snan_in_position', any(snan(inp[a]) for a in ('mpos_x', 'mpos_y', 'mpos_z')))
        add('zero_input_signed', any(inp[a] == 0x80000000 for a in FLOATS))
        two = False
        for ax in 'xyz':
            nn = sum(is_special(inp[a + '_' + ax]) and (inp[a + '_' + ax] & 0x7FFFFF) != 0 for a in ('fwd', 'up', 'mpos'))
            two |= nn >= 2
        add('nan_two_operands', two)
        for nm in ('t1', 't2', 's'):
            add('inter_tiny_' + nm, any(oracle.is_tiny(oracle.LAST[ax][nm]) for ax in 'xyz'))
        add('sum_zero_cancel', any(oracle.LAST[ax]['s'] != oracle.NAN and oracle.LAST[ax]['s'][0] == 'f' and oracle.LAST[ax]['s'][1] == 0
                                  and oracle.LAST[ax]['t1'] != oracle.NAN and oracle.LAST[ax]['t1'][0] == 'f' and oracle.LAST[ax]['t1'][1] != 0 for ax in 'xyz'))
        add('sum_negzero', any(oracle.LAST[ax]['s'] != oracle.NAN and oracle.LAST[ax]['s'][0] == 'f' and oracle.LAST[ax]['s'][1] == 0 and oracle.LAST[ax]['s'][2] == 1 for ax in 'xyz'))


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
        # los ocho colaboradores tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
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
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS + FLARE_KEYS}
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


def nan_gate():
    """Compuerta previa opcional (PF74_NAN_GATE=1 o --nan-gate): el microtest flare_nan_hw valida contra el hardware x87 la emulacion de NaN de produccion; exige 0 discrepancias."""
    import flare_nan_hw
    res = flare_nan_hw.run()
    print('flare_nan_hw: %d combinaciones, %s' % (res['combos'], 'OK' if res['ok'] else 'FALLO'))
    if not res['ok']:
        raise RuntimeError('La emulacion de NaN de PlaneFiring74FlareCpp difiere del hardware x87:\n' + res['output'][-2000:])


if __name__ == '__main__':
    import os
    if '--nan-gate' in sys.argv or os.environ.get('PF74_NAN_GATE') == '1':
        sys.argv = [a for a in sys.argv if a != '--nan-gate']
        nan_gate()
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('planefiring74', manifest['binaries']['asi'], analyze, 'PlaneFiring74', timeout=3600))
