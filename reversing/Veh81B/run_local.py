"""Compuertas del banco Veh81B (lote 81B): CVehicle::UpdateTractorLink (EXE 0x6E0050, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores y virtuales, escrituras en las arenas y paso de tiempo),
los microtests de FCOMP del hardware (el maximo de 0x420800 depende de `fld 1.0f; fcomp b; test ah, 41h`), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el
modelo (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
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

NAMES = ['CVehicle::UpdateTractorLink']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6E0050}
# (direccion del EXE, hooked): CPhysical::GetSpeed 0x542CE0, CVector::Normalise 0x59C910, CrossProduct 0x59C730, CPhysical::ApplyForce 0x542B50
# (hooked=False: el hook queda apagado, la entrada del EXE es codigo original; los cuatro tienen hook RH en la DLL y llevan el mismo falso en el cuerpo de la DLL). Los virtuales GetTowHitchPos (+0xEC) y
# GetTowBarPos (+0xF0) se atienden desde la vtabla falsa de las arenas (sin parche de entrada).
STUBBED = {NAMES[0]: [(0x542CE0, False), (0x59C910, False), (0x59C730, False), (0x542B50, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6E0050}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
SPECIAL_FIELDS = ('hitch', 'bar', 'spd0', 'spd1', 'norm', 'cross', 'com')

KEYS = ['complete', 'no_trailer', 'hitch_fail', 'bar_fail', 'hoist_cut', 'hoist_not_cut_tow', 'hoist_edge_19899', 'hoist_edge_19900', 'non_tow_big_hoist',
        'model_0x20D', 'model_0x213', 'model_high_word_garbage', 'hoist_high_word_garbage',
        'arg0_true', 'arg1_damp', 'scale', 'arg_high_garbage', 'ret_al_zero_high_garbage', 'ret_al_nonzero_high_garbage',
        'luggage', 'luggage_wrong_sentinel', 'luggage_wrong_sub',
        'vmat_null_fallback', 'tmat_null_fallback', 'both_mat_null',
        'max_is_one', 'max_is_ts', 'ts_nan', 'ts_inf', 'ts_negative', 'ts_denormal', 'ts_huge', 'ts_exactly_one',
        'cross_decoy', 'cross_normal', 'k_zero', 'k_nan', 'mass_zero', 'mass_nan', 'mass_inf', 'mass_negative',
        'input_nan', 'input_inf', 'input_denormal', 'input_negzero', 'force_nan', 'force_inf', 'force_denormal', 'force_negzero', 'rel_zero',
        'ts_mut_effective']
FINE = ['hoist_edge_19901', 'luggage_on_t2', 'ts_zero', 'd_register_differs', 'k_inf']   # menos de 100 casos, pero al menos 20
MUT_KINDS = ['mut_kind_%d' % i for i in range(1, 15)]
MUT_EVENT_KEYS = ['mut_event_%d' % i for i in range(7)]


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
    add(exp['path'])
    model = inp['model'] & 0xFFFF
    hoist = inp['hoist'] & 0xFFFF
    add('model_0x20D', model == 0x20D and exp['reached'] >= 3)
    add('model_0x213', model == 0x213 and exp['reached'] >= 3)
    add('model_high_word_garbage', (inp['model'] >> 16) != 0 and exp['reached'] >= 3)
    add('hoist_high_word_garbage', (inp['hoist'] >> 16) != 0 and exp['reached'] >= 3)
    if exp['reached'] >= 3:
        tow = model in (0x20D, 0x213)
        add('hoist_edge_19899', tow and hoist == 19899)
        add('hoist_edge_19900', tow and hoist == 19900)
        add('hoist_edge_19901', tow and hoist == 19901)
        add('non_tow_big_hoist', not tow and hoist > 19900)
    add('hoist_not_cut_tow', exp['reached'] == 4 and exp.get('tow_model', False))
    add('ret_al_zero_high_garbage', exp['reached'] in (1, 2) and ((inp['ret_hitch'] if exp['reached'] == 1 else inp['ret_bar']) >> 8) != 0)
    add('ret_al_nonzero_high_garbage', exp['reached'] >= 2 and ((inp['ret_hitch'] >> 8) != 0 or (inp['ret_bar'] >> 8) != 0))
    for idx, kind in exp['mutated']:
        add('mut_kind_%d' % kind)
        add('mut_event_%d' % idx)
    if exp['reached'] != 4:
        return
    add('arg0_true', exp['arg0'])
    add('arg1_damp', not exp['arg0'] and exp['arg1'])
    add('scale', not exp['arg0'] and not exp['arg1'])
    add('arg_high_garbage', (inp['arg0'] >> 8) != 0 or (inp['arg1'] >> 8) != 0)
    add('luggage', exp['luggage'])
    t4 = exp['trailer_t4']
    add('luggage_on_t2', exp['luggage'] and t4 == 1)
    add('luggage_wrong_sentinel', inp['tsub'][t4] == 11 and inp['tsent'][t4] != 0xC47A0000)
    add('luggage_wrong_sub', inp['tsub'][t4] != 11 and inp['tsent'][t4] == 0xC47A0000)
    add('vmat_null_fallback', not inp['has_vmat'])
    add('tmat_null_fallback', not inp['has_tmat'])
    add('both_mat_null', not inp['has_vmat'] and not inp['has_tmat'])
    if not exp['arg0'] and exp['arg1']:
        ts = inp['ts']
        add('max_is_one', L.get('max_is_one', False))
        add('max_is_ts', not L.get('max_is_one', True))
        add('ts_nan', is_special(ts) and (ts & 0x7FFFFF) != 0)
        add('ts_inf', is_special(ts) and (ts & 0x7FFFFF) == 0)
        add('ts_zero', (ts & 0x7FFFFFFF) == 0)
        add('ts_negative', (ts >> 31) == 1 and not is_special(ts))
        add('ts_denormal', is_denormal(ts))
        add('ts_huge', not is_special(ts) and ((ts >> 23) & 0xFF) >= 190)
        add('ts_exactly_one', ts == 0x3F800000)
        d, ds = L.get('d'), L.get('d_stored')
        if d is not None and ds is not None and d != oracle.NAN and d[0] == 'f':
            back = oracle.load(ds)
            add('d_register_differs', back[0] != 'f' or back[1] != d[1])
    add('ts_mut_effective', any(k == 7 for _, k in exp['mutated']))
    add('cross_decoy', inp['cross_mode'] == 1)
    add('cross_normal', inp['cross_mode'] == 0)
    k = L.get('k')
    if k is not None:
        add('k_nan', k == oracle.NAN)
        add('k_inf', k != oracle.NAN and k[0] == 'i')
        add('k_zero', k != oracle.NAN and k[0] == 'f' and k[1] == 0)
    vm, vt = inp['vmass'], inp['vturn']
    add('mass_zero', any((x & 0x7FFFFFFF) == 0 for x in (vm, vt, inp['tmass'][0])))
    add('mass_nan', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in (vm, vt, inp['tmass'][0])))
    add('mass_inf', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in (vm, vt, inp['tmass'][0])))
    add('mass_negative', any((x >> 31) and not is_special(x) for x in (vm, vt, inp['tmass'][0])))
    flat = [x for f in SPECIAL_FIELDS for x in inp[f]]
    add('input_nan', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in flat))
    add('input_inf', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in flat))
    add('input_denormal', any(is_denormal(x) for x in flat))
    add('input_negzero', any(x == 0x80000000 for x in flat))
    force = exp['events'][-1][2:5]
    add('force_nan', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in force))
    add('force_inf', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in force))
    add('force_denormal', any(is_denormal(x) for x in force))
    add('force_negzero', any(x == 0x80000000 for x in force))
    add('rel_zero', all((x & 0x7FFFFFFF) == 0 for x in exp['events'][4][2:5]))


def check_micro(b):
    """los microtests de la x87 del hardware (en `begin`): FSW & 0x4500 de `fld a; fcomp b` frente al orden IEEE"""
    micro = b.get('x87_microtests')
    if not micro or len(micro) < 400:
        raise RuntimeError('Faltan los microtests de la x87')
    wrong = []
    for op, a, c2, result in micro:
        if oracle.is_nan(a) or oracle.is_nan(c2):
            want = 0x4500
        else:
            x, y = oracle.load(a), oracle.load(c2)
            if oracle.greater(x, y):
                want = 0x0000
            elif oracle.greater(y, x):
                want = 0x0100
            else:
                want = 0x4000
        if want != result:
            wrong.append((op, hex(a), hex(c2), hex(result), hex(want)))
    if wrong:
        raise RuntimeError('La x87 del hardware no sigue el modelo del oraculo: %s' % wrong[:5])
    return len(micro)


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
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 20 for k in MUT_EVENT_KEYS + MUT_KINDS + FINE})
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
    summary['x87_microtests'] = check_micro(b)
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
        sys.exit(runtime_runner.run_case('veh81b', manifest['binaries']['asi'], analyze, 'Veh81B', timeout=3600))
