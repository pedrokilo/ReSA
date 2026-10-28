"""Compuertas del banco FxWheel77 (lote 77): funciones de Fx_c del EXE (hook S), 16384 pares por funcion.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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
_FIX = (HERE / 'Fixtures.hpp').read_text(encoding='utf-8')
CASES = int(re.search(r'constexpr unsigned kCases = (\d+);', _FIX).group(1))
NFUNC = int(re.search(r'constexpr unsigned kFunctions = (\d+);', _FIX).group(1))
ALL = [('Fx_c::AddWheelDust', 0x4A09C0), ('Fx_c::AddWheelSand', 0x4A0610), ('Fx_c::AddGlass', 0x49F970), ('Fx_c::AddDebris', 0x49F750),
       ('Fx_c::AddWheelGrass', 0x49FF20), ('Fx_c::AddWheelGravel', 0x4A0170), ('Fx_c::AddWheelMud', 0x4A03C0), ('Fx_c::TriggerGunshot', 0x4A0DE0)]
NAMES = [n for n, _ in ALL[:NFUNC]]
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(ALL[:NFUNC])
# (direccion del EXE, hooked): colaboradores con hook RH en la DLL (apagados durante el banco: la entrada del EXE es codigo original y la DLL tiene cuerpo propio); rand 0x821B1E y getptd 0x827B3D NO se sustituyen
AP, EP, PED, VEH = 0x4AA440, 0x4AA610, 0x56E210, 0x56E0D0
STUBBED = {'Fx_c::AddWheelDust': [(AP, False), (VEH, False)], 'Fx_c::AddWheelSand': [(AP, False), (VEH, False)], 'Fx_c::AddGlass': [(AP, False)], 'Fx_c::AddDebris': [(AP, False), (EP, False)],
           'Fx_c::AddWheelGrass': [(AP, False), (PED, False), (VEH, False)], 'Fx_c::AddWheelGravel': [(AP, False), (PED, False), (VEH, False)], 'Fx_c::AddWheelMud': [(AP, False), (PED, False), (VEH, False)],
           'Fx_c::TriggerGunshot': [(0x54F560, False), (0x54F1B0, False), (0x4A9440, False), (0x4A9460, False), (0x49E950, False), (0x4A9BE0, False), (0x4AA890, False), (0x4AA3D0, False)]}
WATCHED = {a for fn in NAMES for a, _ in STUBBED.get(fn, [])} | set(HOOKS.values()) | {0x821B1E, 0x827B3D}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_den(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and (b & 0x400000) == 0


def tally(fn, inp, obs, cov):
    """cobertura de ramas desde la entrada y lo observado"""
    calls = obs['particle_calls']
    ev = obs['events']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('in_range', calls > 0)
    add('no_particles', calls == 0)
    add('rand_state_advanced', obs['rand_state'] != inp['rand_seed'])
    add('rand_mode_1_edge_draw', inp['rand_mode'] == 1 and calls > 0)
    add('swap_slot_systems_alternate', inp['swap_slot'] == 1 and calls >= 2 and len({e[1] for e in ev if e[0] == 31}) == 2)
    add('this_kind_%d' % inp['this_kind'])
    pos = inp['pos']; cam = inp['cam']
    add('camera_or_pos_nan', any(is_nan(b) for b in pos + cam))
    add('camera_or_pos_inf', any(is_inf(b) for b in pos + cam))
    add('camera_or_pos_tiny', any(is_den(b) or (b & 0x7FFFFFFF) == 0 for b in pos + cam) and calls > 0)
    add('light_nan', calls > 0 and fn not in ('Fx_c::AddGlass', 'Fx_c::AddDebris') and is_nan(inp['light']))
    add('light_snan', calls > 0 and fn not in ('Fx_c::AddGlass', 'Fx_c::AddDebris') and is_snan(inp['light']))
    add('light_inf', calls > 0 and fn not in ('Fx_c::AddGlass', 'Fx_c::AddDebris') and is_inf(inp['light']))
    add('light_denormal', calls > 0 and fn not in ('Fx_c::AddGlass', 'Fx_c::AddDebris') and is_den(inp['light']))
    add('over_256_particles', calls > 256)
    if fn not in ('Fx_c::AddGlass', 'Fx_c::AddDebris'):
        for tag in ('vel', 'gas', 'timestep'):
            v = inp[tag] if isinstance(inp[tag], list) else [inp[tag]]
            add(tag + '_nan', calls > 0 and any(is_nan(b) for b in v))
            add(tag + '_snan', calls > 0 and any(is_snan(b) for b in v))
            add(tag + '_inf', calls > 0 and any(is_inf(b) for b in v))
            add(tag + '_denormal', calls > 0 and any(is_den(b) for b in v))
            add(tag + '_zero', calls > 0 and any((b & 0x7FFFFFFF) == 0 for b in v))
    if fn in ('Fx_c::AddWheelDust', 'Fx_c::AddWheelSand'):
        q = inp['quality'] if inp['quality'] < 0x80000000 else inp['quality'] - (1 << 32)
        if inp['this_kind'] == 0:
            add('quality_negative', q < 0)
            add('quality_zero', q == 0)
            add('quality_positive', q >= 1)
            add('quality_negative_draws', q < 0 and calls > 0)
            add('quality_zero_draws', q == 0 and calls > 0)
            add('quality_positive_draws', q >= 1 and calls > 0)
        add('spinning_set', calls > 0 and (inp['spinning'] & 0xFF) != 0)
        add('spinning_clear', calls > 0 and (inp['spinning'] & 0xFF) == 0)
        add('spinning_garbage_high_bytes', calls > 0 and (inp['spinning'] >> 8) != 0)
        add('subtype_bmx', calls > 0 and inp['subtype'] == 10)
        add('subtype_bike_quad', calls > 0 and inp['subtype'] in (9, 2))
        add('subtype_other', calls > 0 and inp['subtype'] not in (9, 2, 10))
        add('count_1', calls == 1)
        add('count_2_to_10', 2 <= calls <= 10)
        add('count_11_to_100', 11 <= calls <= 100)
        add('count_101_plus', calls >= 101)
        add('pv_null_in_range', inp['pv_kind'] == 0 and calls > 0)
        add('pv_present_in_range', inp['pv_kind'] != 0 and calls > 0)
    if fn in ('Fx_c::AddWheelGrass', 'Fx_c::AddWheelGravel', 'Fx_c::AddWheelMud'):
        fpp = [e for e in ev if e[0] == 12]
        add('driver_player0', len(fpp) == 1 and calls > 0)
        add('driver_player1', len(fpp) == 2 and calls > 0)
        add('driver_neither', len(fpp) == 2 and calls == 0)
        add('three_particles', calls == 3)
        add('pv_null_in_range', inp['pv_kind'] == 0 and calls > 0)
        add('pv_present_in_range', inp['pv_kind'] != 0 and calls > 0)
    if fn in ('Fx_c::AddGlass', 'Fx_c::AddDebris'):
        amount = inp['amount'] if inp['amount'] < 0x80000000 else inp['amount'] - (1 << 32)
        add('amount_le0', amount <= 0)
        add('amount_negative', amount < 0)
        add('amount_min_int', inp['amount'] == 0x80000000)
        add('one_iteration', calls == 1)
        add('two_to_twelve', 2 <= calls <= 12)
        add('thirteen_plus', calls >= 13)
        add('cam_matrix_in_range', inp['has_mat'] == 1 and calls > 0)
        add('cam_compact_in_range', inp['has_mat'] == 0 and calls > 0)
        add('scale_nan', calls > 0 and is_nan(inp['scale']))
        add('scale_snan', calls > 0 and is_snan(inp['scale']))
        add('color_all_ff', calls > 0 and inp['color'] == 0xFFFFFFFF)
        add('color_all_zero', calls > 0 and inp['color'] == 0)
        if fn == 'Fx_c::AddDebris':
            add('counter_outside_0_3', calls > 0 and inp['counter'] > 3)
            add('counter_0', calls > 0 and inp['counter'] == 0)
            add('counter_3', calls > 0 and inp['counter'] == 3)
    if fn == 'Fx_c::TriggerGunshot':
        kinds = [e[0] for e in ev]
        n26 = kinds.count(26)
        add('gun_out_of_range_or_no_effect', not kinds)
        add('gun_in_range', bool(kinds))
        add('gun_entity_null', inp['ent_kind'] == 0 and 25 in kinds)
        add('gun_entity_null_created_null', inp['ent_kind'] == 0 and 25 in kinds and 24 not in kinds)
        add('gun_entity_null_created_destroyed', inp['ent_kind'] == 0 and 24 in kinds)
        add('gun_entity_no_matrix_allocates', 21 in kinds and 22 in kinds)
        add('gun_entity_rw_null_stops', inp['ent_kind'] == 1 and 21 in kinds and n26 == 0 and inp['rw_kind'] == 0)
        add('gun_entity_with_matrix_systems', inp['ent_kind'] == 1 and inp['has_ent_matrix'] == 1 and n26 >= 1)
        add('gun_entity_no_matrix_systems', inp['ent_kind'] == 1 and inp['has_ent_matrix'] == 0 and n26 >= 1)
        add('gun_flash_and_smoke', n26 == 2)
        add('gun_smoke_only', n26 == 1)
        add('gun_copy_parent_without_entity', 27 in kinds and inp['ent_kind'] == 0)
        add('gun_play_and_kill', 28 in kinds)
        add('gun_system_null_results', n26 >= 1 and 28 not in kinds)
        add('gun_flash_byte_nonzero_garbage', n26 == 2 and (inp['flash'] >> 8) != 0)
        add('gun_entity_pos_nan', inp['ent_kind'] == 1 and any(is_nan(b) for b in inp['epos']) and n26 >= 1)
        add('gun_entity_pos_inf', inp['ent_kind'] == 1 and any(is_inf(b) for b in inp['epos']) and n26 >= 1)
        add('gun_entity_pos_denormal', inp['ent_kind'] == 1 and any(is_den(b) for b in inp['epos']) and n26 >= 1)
        add('gun_target_nan', inp['ent_kind'] == 0 and 25 in kinds and any(is_nan(b) for b in inp['target']))
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
                tally(fn, row['input'], row['candidate']['observed'], cov[fn])
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
        if len(fam[fn]) != 16 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
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
        sys.exit(runtime_runner.run_case('fxwheel77', manifest['binaries']['asi'], analyze, 'FxWheel77', timeout=3600))
