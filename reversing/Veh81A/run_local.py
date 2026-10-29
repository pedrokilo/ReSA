"""Compuertas del banco Veh81A (lote 81A): CVehicle::UpdateTrailerLink (EXE 0x6DFC50, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores, escrituras en el vehiculo, hora final y extremos de disparo),
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

NAMES = ['CVehicle::UpdateTrailerLink']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6DFC50}
# (direccion del EXE, hooked): CPhysical::GetSpeed 0x542CE0 y CPhysical::ApplyForce 0x542B50 (hooked=False: el hook RH queda apagado y la entrada del EXE es codigo original; ambos tienen
# hook RH en la DLL y llevan el mismo falso en el cuerpo de la DLL). Los virtuales +0xEC/+0xF0/+0xF8 van por la vtabla falsa (sin parches). Se apagan ademas, sin falso, los hooks RH de
# DotProduct 0x40FDB0, CrossProduct 0x59C730 y Normalise 0x59C910 (aritmetica que ejecuta el EXE en el lado original).
STUBBED = {NAMES[0]: [(0x542CE0, False), (0x542B50, False)]}
HOOKS_OFF = {0x40FDB0, 0x59C730, 0x59C910}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6DFC50} | HOOKS_OFF
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = ['p_no_trailer', 'p_status', 'p_hitch', 'p_bar', 'p_far', 'p_forward', 'p_up', 'p_return_tow', 'p_full',
        'status_10', 'status_11', 'status_other', 'ret_hi_garbage', 'ret_al0_hi_nonzero',
        'arg_damp', 'arg_both_false_full', 'arg_both_true_full', 'arg0_true_arg1_false_full', 'arg_hi_garbage',
        'far_equal', 'far_ulp', 'forward_equal', 'up_equal_zero', 'up_negative_zero',
        'model_tow', 'model_tow_return', 'model_tow_pass', 'hoist_below_100', 'angle_eq_limit', 'angle_eq_limit_plus1',
        'subtype11', 'w9ec_match', 'project_applied', 'project_subtype_other', 'project_w9ec_other',
        'ts_nan', 'ts_inf', 'ts_zero', 'ts_negative', 'ts_gt_one', 'ts_lt_one', 'ts_below_min', 'damp_r_one', 'damp_r_ts',
        'hitch_special', 'bar_special', 'speed_special', 'matrix_special', 'centre_special', 'mass_special', 'turn_special',
        'norm_zero', 'force_nan', 'force_inf', 'force_denormal', 'force_negzero', 'rel_cancel_zero',
        'mut_trailer', 'mut_self_matrix', 'mut_tow_matrix', 'mut_step', 'mut_mass', 'mut_turn', 'mut_centre', 'mut_subtype', 'mut_w9ec', 'mut_angle', 'mut_model', 'mut_hoist']
MUT_KEYS = {1: 'mut_trailer', 2: 'mut_self_matrix', 3: 'mut_tow_matrix', 4: 'mut_step', 5: 'mut_mass', 6: 'mut_turn', 7: 'mut_centre', 8: 'mut_subtype', 9: 'mut_w9ec', 10: 'mut_angle', 11: 'mut_model', 12: 'mut_hoist'}
MUT_EVENT_KEYS = ['mut_event_%d' % i for i in range(5)]


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    exp = oracle.expected(inp)
    L = dict(oracle.LAST)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    path = exp['path']
    for p, key in (('break_no_trailer', 'p_no_trailer'), ('break_status', 'p_status'), ('break_hitch', 'p_hitch'), ('break_bar', 'p_bar'), ('break_far', 'p_far'),
                   ('break_forward', 'p_forward'), ('break_up', 'p_up'), ('return_tow', 'p_return_tow'), ('full', 'p_full')):
        add(key, path == p)
    status = (inp['status_byte'] >> 3) & 0x1F
    add('status_10', status == 10)
    add('status_11', status == 11)
    add('status_other', status not in (10, 11))
    add('ret_hi_garbage', (inp['hitch_ret'] >> 8) != 0 or (inp['bar_ret'] >> 8) != 0)
    add('ret_al0_hi_nonzero', ((inp['hitch_ret'] & 0xFF) == 0 and inp['hitch_ret'] != 0) or ((inp['bar_ret'] & 0xFF) == 0 and inp['bar_ret'] != 0))
    a0, a1 = inp['arg0'] & 0xFF, inp['arg1'] & 0xFF
    add('arg_damp', exp['damped'])
    add('arg_both_false_full', path == 'full' and not a0 and not a1)
    add('arg_both_true_full', path == 'full' and a0 and a1)
    add('arg0_true_arg1_false_full', path == 'full' and a0 and not a1)
    add('arg_hi_garbage', (inp['arg0'] >> 8) != 0 or (inp['arg1'] >> 8) != 0)
    if 'sc' in L and L['sc'] != oracle.NAN and L['ln'] != oracle.NAN:
        sc, ln = oracle.value(L['sc']), oracle.value(L['ln'])
        add('far_equal', sc == ln)
        if sc not in (float('inf'), float('-inf')) and ln not in (float('inf'), float('-inf')) and ln != 0:
            add('far_ulp', abs(sc - ln) <= abs(ln) / 2 ** 22 and sc != ln)
    if 'fwd' in L and L['fwd'] != oracle.NAN:
        add('forward_equal', oracle.value(L['fwd']) == oracle.value(oracle.load(oracle.FWD_LIMIT)))
    if 'up' in L and L['up'] != oracle.NAN:
        add('up_equal_zero', oracle.value(L['up']) == 0)
        add('up_negative_zero', oracle.value(L['up']) == 0 and L['up'][0] == 'f' and L['up'][2] == 1)
    model = inp['model'] & 0xFFFF
    add('model_tow', model in (0x20D, 0x213))
    add('model_tow_return', path == 'return_tow')
    add('model_tow_pass', model in (0x20D, 0x213) and path == 'full')
    add('hoist_below_100', model in (0x20D, 0x213) and (inp['hoist'] & 0xFFFF) < 100)
    lim = (inp['hoist'] & 0xFFFF) - 100
    add('angle_eq_limit', model in (0x20D, 0x213) and (inp['angle'] & 0xFFFF) == lim)
    add('angle_eq_limit_plus1', model in (0x20D, 0x213) and (inp['angle'] & 0xFFFF) == lim + 1)
    add('subtype11', inp['subtype'] == 11)
    add('w9ec_match', inp['w9ec'] == 0xC47A0000)
    add('project_applied', exp['projected'])
    add('project_subtype_other', path == 'full' and inp['subtype'] != 11 and inp['w9ec'] == 0xC47A0000)
    add('project_w9ec_other', path == 'full' and inp['subtype'] == 11 and inp['w9ec'] != 0xC47A0000)
    ts = inp['ts']
    tsv = oracle.value(oracle.load(ts))
    add('ts_nan', oracle.load(ts) == oracle.NAN)
    add('ts_inf', tsv in (float('inf'), float('-inf')))
    add('ts_zero', tsv == 0)
    add('ts_negative', tsv is not None and tsv < 0)
    add('ts_gt_one', tsv is not None and tsv > 1)
    add('ts_lt_one', tsv is not None and tsv < 1)
    add('ts_below_min', tsv is not None and tsv <= oracle.value(oracle.load(oracle.STEP_MIN)))
    if exp['damped']:
        add('damp_r_one', L.get('r_one') is True)
        add('damp_r_ts', L.get('r_one') is False)
    add('hitch_special', any(is_special(x) for x in inp['hitch']))
    add('bar_special', any(is_special(x) for x in inp['bar']))
    add('speed_special', any(is_special(x) for x in inp['spd0'] + inp['spd1']))
    add('matrix_special', any(is_special(x) for x in inp['self_m'] + inp['tow_m']))
    add('centre_special', any(is_special(x) for x in inp['centre']))
    add('mass_special', is_special(inp['mass']) or (inp['mass'] & 0x7FFFFFFF) == 0 or is_denormal(inp['mass']) or inp['mass'] >> 31 == 1)
    add('turn_special', is_special(inp['turn']) or (inp['turn'] & 0x7FFFFFFF) == 0 or is_denormal(inp['turn']) or inp['turn'] >> 31 == 1)
    if path == 'full':
        add('norm_zero', L.get('norm_zero') is True)
        fo = exp['events'][-1][2:5]
        add('force_nan', any(v is None for v in fo))
        add('force_inf', any(v is not None and is_special(v) and (v & 0x7FFFFF) == 0 for v in fo))
        add('force_denormal', any(v is not None and is_denormal(v) for v in fo))
        add('force_negzero', any(v == 0x80000000 for v in fo))
        rel = L.get('rel')
        add('rel_cancel_zero', rel is not None and any(v is not None and (v & 0x7FFFFFFF) == 0 for v in rel))
    for idx, kind in exp['mutated']:
        add(MUT_KEYS[kind])
        add('mut_event_%d' % idx)


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
        # los dos colaboradores tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
        # la aritmetica del EXE (DotProduct, CrossProduct, Normalise) debe ser codigo original en la entrada: o su hook RH se apago (hooks_off) o no hay salto (E9) en los bytes vigilados
        off = {h['exe'] for h in col[fn]['hooks_off']}
        watched = {w['address']: w['bytes'] for w in b['watched_entries']}
        for a in HOOKS_OFF:
            if a not in off and watched.get(a, 'e9').startswith('e9'):
                raise RuntimeError('Hook de aritmetica sin apagar en %s: 0x%X' % (fn, a))
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
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 20 for k in MUT_EVENT_KEYS})
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


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('veh81a', manifest['binaries']['asi'], analyze, 'Veh81A', timeout=3600))
