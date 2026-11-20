"""Compuertas del banco FnPW1 (banco FnPW1): CVehicle::ProcessWheel (EXE 0x6D6C00, hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: traza de colaboradores EN ORDEN con todos sus argumentos, sorteos de rand(), sistema del hueco de g_fx
y arenas sin cambios), los microtests de la x87 del hardware, la cobertura por familia de las fixtures y la cobertura de ramas (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import re
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import terminal_event, digest
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle
CASES = 20480   # 16384 (familias 0..15) + 4 x 1024 (familias 16..19)
NFUNC = 1
ALL = [('CVehicle::ProcessWheel', 0x6D6C00)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# (direccion del EXE, hooked): colaboradores con hook RH en la DLL (apagados durante el banco: la entrada del EXE es codigo original y la DLL tiene cuerpo propio); rand 0x821B1E y getptd 0x827B3D NO se sustituyen
STUB_ADDRS = (0x5429F0, 0x542A50)
STUBBED = {'CVehicle::ProcessWheel': [(a, False) for a in STUB_ADDRS]}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values()) | {0x821B1E, 0x827B3D}
COVER_FLOOR = 100
FAMILIES = 20
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_den(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and (b & 0x400000) == 0


KEYS = ['lat', 'lat_zero', 'burst', 'drive', 'brake', 'brake_nan', 'idle_bike', 'idle_plane', 'idle_light', 'idle_rc', 'idle_car', 'coast_gas', 'limit', 'limit_pos', 'limit_neg', 'over_adh', 'fixed',
        'circle_out', 'circle_in', 'state_spin', 'state_skid', 'tlim_double', 'tl_one', 'tl_gas', 'noforce', 'forces', 'antidive', 'norm', 'tnorm', 'nofwd', 'prev_state', 'prev_spin_loss', 'clamp_pos', 'clamp_neg']
FINE = ['norm_skip', 'tnorm_skip',
        'brake_nan_pos', 'brake_nan_neg', 'brake_inf_pos', 'brake_inf_neg', 'brake_zero', 'brake_negzero', 'adh_zero', 'adh_negative', 'adh_nan', 'adh_tiny', 'thrust_nan', 'thrust_inf',
        'cs_denormal', 'fwd_denormal', 'right_denormal', 'cs_near_zero', 'wheels_0', 'wheels_1', 'wheels_2', 'wheels_3', 'wheels_4', 'wheels_odd', 'wstate_0', 'wstate_1', 'wstate_2', 'wstate_3',
        'static_ac_0', 'static_ac_1', 'static_ad_0', 'static_ad_1', 'static_ae_0', 'static_ae_1', 'veh_car', 'veh_bike', 'veh_plane', 'veh_rcbandit', 'gas_zero', 'gas_nonzero',
        'status_player', 'status_remote', 'status_other', 'wheel_status_burst', 'wheel_status_normal', 'sadm_nan', 'sadm_zero', 'sadm_neg', 'cp_nan', 'mass_special']


def tally(fn, inp, obs, cov, path):
    """cobertura de ramas desde la ruta del oraculo y de las entradas obligatorias del encargo"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for tok in path.split('+'):
        add(tok)
    b = inp['brake']
    add('brake_nan_pos', is_nan(b) and not b & 0x80000000)
    add('brake_nan_neg', is_nan(b) and b & 0x80000000)
    add('brake_inf_pos', b == 0x7F800000)
    add('brake_inf_neg', b == 0xFF800000)
    add('brake_zero', b == 0)
    add('brake_negzero', b == 0x80000000)
    a = inp['adh']
    add('adh_zero', (a & 0x7FFFFFFF) == 0)
    add('adh_negative', (a & 0x80000000) != 0 and not is_nan(a) and (a & 0x7FFFFFFF) != 0)
    add('adh_nan', is_nan(a))
    add('adh_tiny', (a & 0x7FFFFFFF) != 0 and (a & 0x7FFFFFFF) < 0x00800000)
    add('thrust_nan', is_nan(inp['thrust']))
    add('thrust_inf', is_inf(inp['thrust']))
    add('cs_denormal', any(is_den(x) for x in inp['cs']))
    add('fwd_denormal', any(is_den(x) for x in inp['fwd']))
    add('right_denormal', any(is_den(x) for x in inp['right']))
    add('cs_near_zero', any((x & 0x7FFFFFFF) < 0x20000000 for x in inp['cs']))
    w = inp['wheels']
    for k in range(5):
        add('wheels_%d' % k, w == k)
    add('wheels_odd', w > 4)
    for k in range(4):
        add('wstate_%d' % k, inp['wstate'] == k)
    add('static_ac_0', inp['af'] == 0)
    add('static_ac_1', inp['af'] != 0)
    add('static_ad_0', inp['b1'] == 0)
    add('static_ad_1', inp['b1'] != 0)
    add('static_ae_0', inp['b2'] == 0)
    add('static_ae_1', inp['b2'] != 0)
    add('veh_car', inp['vtype'] != 9 and inp['subtype'] != 4)
    add('veh_bike', inp['vtype'] == 9)
    add('veh_plane', inp['vtype'] != 9 and inp['subtype'] == 4)
    add('veh_rcbandit', (inp['model'] & 0xFFFF) == 0x1B9)
    add('gas_zero', (inp['gas'] & 0x7FFFFFFF) == 0)
    add('gas_nonzero', (inp['gas'] & 0x7FFFFFFF) != 0)
    st = (inp['vstat'] & 0xFF) >> 3
    add('status_player', st == 0)
    add('status_remote', st == 8)
    add('status_other', st not in (0, 8))
    add('wheel_status_burst', (inp['status'] & 0xFFFF) == 1)
    add('wheel_status_normal', (inp['status'] & 0xFFFF) != 1)
    add('sadm_nan', is_nan(inp['sadm']))
    add('sadm_zero', (inp['sadm'] & 0x7FFFFFFF) == 0)
    add('sadm_neg', (inp['sadm'] & 0x80000000) != 0 and not is_nan(inp['sadm']) and (inp['sadm'] & 0x7FFFFFFF) != 0)
    add('cp_nan', any(is_nan(x) for x in inp['cp']))
    add('mass_special', any(is_nan(inp[k]) or is_inf(inp[k]) or is_den(inp[k]) for k in ('mass', 'turn', 'hmass', 'tl', 'fric', 'ts')))
    add('family_%d' % inp['family'])


def check_micro(b):
    """los microtests de la x87 del hardware (en `begin`) siguen las reglas que supone el oraculo"""
    micro = b.get('x87_microtests')
    if not micro or len(micro) < 600:
        raise RuntimeError('Faltan los microtests de la x87')
    import fixture_oracle as model
    wrong = []
    for op, a, c2, result in micro:
        x, y = model.fl(a), model.mem(c2)
        if op == 0:
            want = model.to_f32_bits(model.add(x, y))
        elif op == 1:
            want = model.to_f32_bits(model.sub(x, y))
        elif op == 2:
            want = model.to_f32_bits(model.mul(x, y))
        elif op == 3:
            want = model.to_f32_bits(model.div(x, y))
        elif op == 4:
            want = model.to_f32_bits(model.sqrt_v(x))
        elif op == 5:
            want = model.ftol_low32(x) & 0xFFFFFFFF
        else:
            want = model.fabs_bits(a)
        if want != result:
            wrong.append((op, hex(a), hex(c2), hex(result), hex(want)))
    if wrong:
        raise RuntimeError('La x87 del hardware no sigue el modelo del oraculo: %s' % wrong[:5])
    return len(micro)


def analyze(path):
    """Analisis en una sola pasada (el registro completo pesa cientos de MB): nada se acumula salvo contadores"""
    path = Path(path)
    if not path.exists() or not terminal_event(path):
        return None
    total = sum(EXPECTED.values())
    begin = None
    ends, states, fails, post_complete = [], [], [], []
    seen = {fn: bytearray(CASES) for fn in NAMES}
    pairs_seen = bytearray(total)
    counts = {fn: 0 for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    with path.open(encoding='utf-8') as stream:
        for line in stream:
            if not line.endswith('\n'):
                continue
            row = json.loads(line)
            ev = row['event']
            if ev == 'comparison':
                fn, case = row['function'], row['case']
                if fn not in seen or seen[fn][case] or row['pair'] >= total or pairs_seen[row['pair']]:
                    raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
                seen[fn][case] = 1
                pairs_seen[row['pair']] = 1
                counts[fn] += 1
                if not (row['equal'] and row['accepted'] and row['original'] == row['candidate']):
                    raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
                for side in (row['original'], row['candidate']):
                    if not all(side.get(k) is True for k in GATES):
                        raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
                if row['fp_difference']:
                    raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
                f = row['input']['family']
                fam[fn][f] = fam[fn].get(f, 0) + 1
                bad, opath = oracle.check_row_obj(row)
                if bad:
                    raise RuntimeError('El oraculo independiente no coincide en %s %d: %s' % (fn, case, bad[0]))
                tally(fn, row['input'], row['candidate']['observed'], cov[fn], opath)
            elif ev == 'begin':
                if begin is not None:
                    raise RuntimeError('Dos inicios')
                begin = row
            elif ev == 'function_summary':
                if row['failures']:
                    fails.append('%s: %d/%d' % (row['function'], row['failures'], row['cases']))
            elif ev in ('complete', 'error', 'assertion'):
                if any(r['event'] == 'complete' for r in ends):
                    # Despues de `complete` (comparaciones hechas, hooks y estado restaurados) el juego sigue corriendo hasta que el corredor lo cierra: una asercion del propio juego
                    # (p. ej. CShadowCamera::DrawOutlineBorder) en ese margen no pertenece al banco; se registra en el resumen
                    post_complete.append({k: row.get(k) for k in ('event', 'expression', 'file', 'line')} | {'top_of_stack': (row.get('stack') or [''])[:4]})
                else:
                    ends.append(row)
            elif ev == 'global_state_final':
                states.append(row)
            elif ev.startswith('first_chance_av'):
                raise RuntimeError('Acceso invalido')
    if fails and ends:
        raise RuntimeError('Funciones con diferencias: ' + '; '.join(fails))
    if any(r['event'] in ('assertion', 'error') for r in ends):
        raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed = [r for r in ends if r['event'] == 'complete']
    if not completed:
        return None
    b = begin
    if b is None or b.get('registered_hook') is not True or b.get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    n_micro = check_micro(b)
    if b.get('rng_native') is not True:
        raise RuntimeError('rand/getptd del EXE no son los bytes originales (o saltan a la DLL)')
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != total or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los colaboradores con hook RH (apagado durante el banco) y CSimpleTransform::UpdateMatrix (cuerpo de la DLL por su direccion) llevan el mismo falso en el ASI
        if [s['dll'] for s in col[fn]['stubbed']] != [True] * len(STUBBED[fn]):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver (o inesperado) en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    if counts != EXPECTED or sum(pairs_seen) != total:
        raise RuntimeError('Cobertura incompleta: ' + json.dumps(counts))
    for fn in NAMES:
        short = {k: v for k, v in fam[fn].items() if v < min(200, CASES // 32)}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 20 for k in FINE})
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = completed
    if len(end) != 1 or end[0]['pairs'] != total or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    if len(states) != 1 or states[0]['floating_state'] != b['initial_fp'] or states[0]['watched_intact'] is not True or states[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    return dict(pairs=total, pairs_per_function=counts, all_equal=True, state_restored=True, log_sha256=digest(path), compare_dead_x87_registers=False,
                x87_microtests=n_micro, families=fam, branch_coverage=cov, events_after_complete=post_complete)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fnpw1', manifest['binaries']['asi'], analyze, 'FnPW1', timeout=3600))
