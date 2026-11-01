"""Compuertas del banco Veh83C (lote 83C): CVehicle::AddSingleWheelParticles (EXE 0x6DE880, hook S), 18432 pares.

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
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
NFUNC = 1
ALL = [('CVehicle::AddSingleWheelParticles', 0x6DE880)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# (direccion del EXE, hooked): colaboradores con hook RH en la DLL (apagados durante el banco: la entrada del EXE es codigo original y la DLL tiene cuerpo propio); rand 0x821B1E y getptd 0x827B3D NO se sustituyen
STUB_ADDRS = (0x56E0D0, 0x55E710, 0x6EB690, 0x55E650, 0x49F040, 0x56E210, 0x506EB0, 0x6D2BF0, 0x6D2D50, 0x4AA440, 0x720930)
STUBBED = {'CVehicle::AddSingleWheelParticles': [(a, False) for a in STUB_ADDRS]}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values()) | {0x821B1E, 0x827B3D, 0x41BD90, 0x4082C0, 0x59C970, 0x4AB290}
COVER_FLOOR = 100
FAMILIES = 18
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_den(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and (b & 0x400000) == 0


KEYS = ['offscreen', 'far', 'gate400', 'gate64', 'arg2_exit', 'water_surface', 'water_level', 'sparks', 'sparks_gas', 'subtype_half', 'subtype_10', 'trail', 'spin_particle',
        'quad_front', 'quad_back', 'skid', 'skid_water_exit', 'wstatus1_exit', 'ws1', 'ws2', 'ws3', 'ws_default', 'ws2_flag4_exit', 'default_exit']
FINE = ['a3_nan', 'gas_nan', 'vel_nan', 'vel_inf', 'denormal_input', 'over_100_particles', 'width_special', 'swap_alternates', 'cam_compact', 'cam_matrix', 'blood_garbage', 'ws_garbage',
        'idx_negative', 'timestep_special', 'pos_nan', 'mat_nan', 'ws1_particles_nan', 'trail_particles_nan']


def tally(fn, inp, obs, cov, path):
    """cobertura de ramas desde la ruta del oraculo y las entradas"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for tok in path.split('+'):
        add(tok)
    ev = obs['events']
    parts = obs['ev_count']
    add('a3_nan', is_nan(inp['a3']))
    add('gas_nan', is_nan(inp['gas']))
    add('vel_nan', any(is_nan(b) for b in inp['vel']))
    add('vel_inf', any(is_inf(b) for b in inp['vel']))
    add('denormal_input', any(is_den(b) for b in inp['vel'] + inp['pos'] + [inp['a2'], inp['a3'], inp['gas'], inp['timestep']]))
    add('over_100_particles', parts > 100)
    add('width_special', any(e[0] == 12 for e in ev) and (is_nan(inp['width']) or is_inf(inp['width']) or is_den(inp['width'])))
    add('swap_alternates', inp['swap_slot'] == 1 and len({e[1] for e in ev if e[0] == 10}) == 2)
    add('cam_compact', inp['has_mat'] == 0 and parts > 2)
    add('cam_matrix', inp['has_mat'] == 1 and parts > 2)
    add('blood_garbage', (inp['blood'] & 0xFF) not in (0, 1) and parts > 2)
    add('ws_garbage', inp['ws'] > 5 and parts > 2)
    add('idx_negative', inp['idx'] & 0x80000000 and parts > 2)
    add('timestep_special', any(e[0] == 10 for e in ev) and (is_nan(inp['timestep']) or is_inf(inp['timestep']) or is_den(inp['timestep'])))
    add('pos_nan', any(is_nan(b) for b in inp['pos']) and parts > 2)
    add('mat_nan', any(is_nan(b) for b in inp['mat']) and parts > 2)
    add('ws1_particles_nan', 'spin_particle' in path and any(any(is_nan(x) for x in e[5:11]) for e in ev if e[0] == 10))
    add('trail_particles_nan', 'trail' in path and any(any(is_nan(x) for x in e[5:11]) for e in ev if e[0] == 10))
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
        sys.exit(runtime_runner.run_case('veh83c', manifest['binaries']['asi'], analyze, 'Veh83C', timeout=3600))
