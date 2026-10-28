"""Compuertas del banco Veh80C (lote 80C): CVehicle::FireFixedMachineGuns (EXE 0x73DF00, hook S), 18432 pares.

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

NAMES = ['CVehicle::FireFixedMachineGuns']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x73DF00}
# (direccion del EXE, hooked): operator*(CMatrix, CVector) 0x59C890, CWeapon::DoTankDoomAiming 0x73D1E0, FireOneInstantHitRound 0x73AF00, CAudioEngine::ReportWeaponEvent 0x506F40, rand() 0x821B1E
# (hooked=False: el hook queda apagado, la entrada del EXE es codigo original; los cuatro primeros tienen hook RH en la DLL y todos llevan el mismo falso en el cuerpo de la DLL; rand: CGeneral::GetRandomNumber)
STUBBED = {NAMES[0]: [(0x59C890, False), (0x73D1E0, False), (0x73AF00, False), (0x506F40, False), (0x821B1E, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x73DF00}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FLOATS = ('top_x', 'top_y', 'res0x', 'res0y', 'res0z', 'res1x', 'res1y', 'res1z')

KEYS = ['fire', 'nofire', 'edge_equal_nofire', 'edge_plus1_fire', 'gun_sum_wrap', 'null_matrix_nofire',
        'len_clamped', 'len_not_clamped', 'len_exact_min', 'len_nan', 'len_inf', 'dir_zero', 'dir_negzero_comp', 'dir_huge', 'dir_tiny', 'dir_nan_in', 'dir_inf_in',
        'res_special', 'res_nan', 'res_inf', 'res_denormal', 'res_negzero', 'end_nan', 'end_inf', 'end_denormal', 'end_negzero', 'sum_cancel_zero', 'jit_zero',
        'rnd_edge_low0', 'rnd_edge_low255', 'rnd_edge_low128', 'rnd_hi_garbage',
        'ammo_reload', 'ammo_wrap', 'ammo_other', 'reload_clock_mutated', 'reload_gun_mutated',
        'mut_matrix', 'mut_driver', 'mut_ammo', 'mut_gun', 'mut_clock', 'mut_before_second_mult', 'mut_at_first_event', 'mut_at_last_event', 'mut_matrix_affects_second_mult',
        'mut_driver_affects_doom', 'doom_end', 'doom_start', 'doom_both', 'driver_null', 'driver_pedA', 'driver_pedB', 'driver_vehicle', 'driver_garbage']
MUT_EVENT_KEYS = ['mut_event_%d' % i for i in range(13)]


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    exp = oracle.expected(inp)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    now, gun = inp['now'], inp['gun']
    lim = (gun + 150) & 0xFFFFFFFF
    add('fire', exp['fired'])
    add('nofire', not exp['fired'])
    add('edge_equal_nofire', now == lim)
    add('edge_plus1_fire', now == ((lim + 1) & 0xFFFFFFFF))
    add('gun_sum_wrap', gun + 150 > 0xFFFFFFFF)
    add('null_matrix_nofire', inp['has_mat'] == 0 and not exp['fired'])
    d = inp['driver']
    add('driver_null', d == 0)
    add('driver_pedA', d == 1)
    add('driver_pedB', d == 2)
    add('driver_vehicle', d == 3)
    add('driver_garbage', d >= 4)
    if not exp['fired']:
        return
    # el modelo se recalcula aqui para dejar LAST con los intermedios de ESTE caso
    oracle.direction(inp)
    L = oracle.LAST
    add('len_clamped', L['clamped'])
    add('len_not_clamped', not L['clamped'] and L['len'] != oracle.NAN)
    add('len_exact_min', L['len'] != oracle.NAN and L['len'][0] == 'f' and L['len'][1] == oracle.load(oracle.KMIN)[1])
    add('len_nan', L['len'] == oracle.NAN)
    add('len_inf', L['len'] != oracle.NAN and L['len'][0] == 'i')
    tx, ty = inp['top_x'], inp['top_y']
    add('dir_zero', (tx & 0x7FFFFFFF) == 0 and (ty & 0x7FFFFFFF) == 0)
    add('dir_negzero_comp', tx == 0x80000000 or ty == 0x80000000)
    add('dir_huge', any(((x >> 23) & 0xFF) >= 190 and not is_special(x) for x in (tx, ty)))
    add('dir_tiny', any(((x >> 23) & 0xFF) < 40 and (x & 0x7FFFFFFF) != 0 for x in (tx, ty)))
    add('dir_nan_in', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in (tx, ty)))
    add('dir_inf_in', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in (tx, ty)))
    res = [inp[a] for a in FLOATS[2:]]
    add('res_special', any(is_special(x) for x in res))
    add('res_nan', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in res))
    add('res_inf', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in res))
    add('res_denormal', any(is_denormal(x) for x in res))
    add('res_negzero', any(x == 0x80000000 for x in res))
    ends_model = [e[5:8] for e in exp['events'] if e[0] == 3]
    flat = [v for t in ends_model for v in t]
    add('end_nan', any(v is None for v in flat))
    add('end_inf', any(v is not None and is_special(v) and (v & 0x7FFFFF) == 0 for v in flat))
    add('end_denormal', any(v is not None and is_denormal(v) for v in flat))
    add('end_negzero', any(v == 0x80000000 for v in flat))
    add('sum_cancel_zero', any(v is not None and (v & 0x7FFFFFFF) == 0 for v in flat))
    raw = [inp['rnd%d' % i] for i in range(6)]
    add('jit_zero', any((r & 0xFF) == 0x80 for r in raw))
    add('rnd_edge_low0', any((r & 0xFF) == 0 for r in raw))
    add('rnd_edge_low255', any((r & 0xFF) == 0xFF for r in raw))
    add('rnd_edge_low128', any((r & 0xFF) == 0x80 for r in raw))
    add('rnd_hi_garbage', any((r >> 8) != 0 for r in raw))
    a = inp['ammo'] & 0xFF
    add('ammo_reload', a == 1)
    add('ammo_wrap', a == 0)
    add('ammo_other', a > 1)
    add('doom_end', inp['doom_mode'] == 1)
    add('doom_start', inp['doom_mode'] == 2)
    add('doom_both', inp['doom_mode'] == 3)
    for idx, kind in exp['mutated']:
        add({1: 'mut_matrix', 2: 'mut_driver', 3: 'mut_ammo', 4: 'mut_gun', 5: 'mut_clock'}[kind])
        add('mut_event_%d' % idx)
        add('mut_before_second_mult', idx < 6)
        add('mut_at_first_event', idx == 0)
        add('mut_at_last_event', idx == 12)
        add('mut_matrix_affects_second_mult', kind == 1 and idx < 6)
        add('mut_driver_affects_doom', kind == 2 and idx in (0, 1, 2, 3, 6, 7, 8, 9))
        add('reload_clock_mutated', kind == 5 and exp.get('reloaded', False))
        add('reload_gun_mutated', kind == 4 and exp.get('reloaded', False))


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
        sys.exit(runtime_runner.run_case('veh80c', manifest['binaries']['asi'], analyze, 'Veh80C', timeout=3600))
