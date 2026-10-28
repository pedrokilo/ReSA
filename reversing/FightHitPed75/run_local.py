"""Compuertas del banco FightHitPed75 (lote 75): CTaskSimpleFight::FightHitPed (EXE 0x61CBA0, hook S).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las
entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CTaskSimpleFight::FightHitPed']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 2352, 19: 1024}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x61CBA0}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp: IsPlayer, GetActiveTask, GetTaskFighting, BeHitWhileBlocking (sin hook), CAEPedAudioEntity::AddAudioEvent (sin hook),
# GetWeaponInfo, FightStrikeDamage, ComputePedShotSide, GenerateDamageEvent, CAEPedWeaponAudioEntity::AddAudioEvent (sin hook), Say, GetEventGlobalGroup, CEventGroup::Add,
# RegisterReference, CleanUpOldReference, rand (sin hook), IsAlive, CVector::Normalise (sin hook), Fx_c::AddBlood (hook S desde el lote 79A)
STUBBED = {NAMES[0]: [(0x5DF8F0, True), (0x681720, True), (0x600F30, True), (0x61C650, False), (0x4E2BB0, False), (0x743C60, True), (0x61C740, True), (0x5F13F0, True), (0x73A530, True),
                      (0x4E69F0, False), (0x5EFFE0, True), (0x4ABA50, True), (0x4AB420, True), (0x571B70, True), (0x571A00, True), (0x821B1E, False), (0x5E0170, True), (0x59C910, False),
                      (0x49EB00, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x61CBA0, 0xC16900, 0x8D2D00}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_snan(b):
    return is_nan(b) and not b & 0x400000


def is_denormal_bits(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def s32(v):
    return v - (1 << 32) if v & 0x80000000 else v


def sx8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def tally(inp, events, result, cov, sets):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    normal = 7 in k
    combo, move = sx8(inp['combo']), sx8(inp['move'])
    add('player_victim_checked', 3 in k)
    add('player_task_exit', 3 in k and 7 not in k and 4 not in k)
    add('player_task_continue', 3 in k and 7 in k or (3 in k and 5 in k))
    add('fighting_checked', k.count(4) == 2)
    add('not_fighting', k.count(4) == 1 and 4 in k)
    add('behit_called', 5 in k)
    add('blocked_exit', 5 in k and not normal)
    add('blocked_combo5_second_sound', 5 in k and not normal and k.count(6) == 2)
    add('blocked_single_sound', 5 in k and not normal and k.count(6) == 1)
    add('fighting_not_blocked', 5 in k and normal)
    add('normal_path', normal)
    add('weapon_info_called', 7 in k)
    add('damage_called', 8 in k)
    add('shot_side_called', 9 in k)
    add('generate_called', 10 in k)
    add('generate_true', normal and result != 0)
    add('generate_false', normal and result == 0)
    add('generate_high_garbage', normal and inp['gde_hi'] != 0)
    add('chainsaw_audio', 11 in k)
    add('chainsaw_lost_after_mutation', normal and 11 not in k and inp['mut_weapon'] == 1 and inp['weapon_type'] == 9)
    add('chainsaw_only_after_mutation', 11 in k and inp['mut_weapon'] != 0 and inp['weapon_type'] != 9)
    add('slot_negative', normal and (inp['slot'] & 0x80) != 0)
    add('slot_out_of_range', normal and (inp['slot'] & 0xFF) > 12 and (inp['slot'] & 0x80) == 0)
    add('component_negative', normal and (inp['component'] & 0x8000) != 0)
    add('component_high_garbage', normal and (inp['component'] >> 16) != 0)
    side = inp['side']
    pe = [e for e in events if e[0] == 6]
    if normal:
        add('side_zero', side == 0)
        add('side_nonzero', side != 0)
        add('side_low_byte_zero_nonzero', side != 0 and side & 0xFF == 0)
        add('side_negative', s32(side) < 0)
        add('side_zero_combo5_two_sounds', side == 0 and (inp['combo'] & 0xFF) == 5 and 0 <= sx8(inp['move']) <= 2 and len(pe) == 2 and inp['mut_site'] != 3)
        add('side_zero_combo7_move1', side == 0 and (inp['combo'] & 0xFF) == 7 and (inp['move'] & 0xFF) == 1 and inp['mut_site'] != 3)
        add('side_zero_plain', side == 0 and len(pe) == 1)
        add('side_nonzero_combo5_two_sounds', side != 0 and (inp['combo'] & 0xFF) == 5 and 0 <= sx8(inp['move']) <= 2 and len(pe) == 2 and inp['mut_site'] != 3)
        add('side_nonzero_plain', side != 0 and len(pe) == 1)
        add('combo5_move_out_of_range', (inp['combo'] & 0xFF) == 5 and not 0 <= sx8(inp['move']) <= 2 and inp['mut_site'] == 0)
        add('combo7_other_move', (inp['combo'] & 0xFF) == 7 and (inp['move'] & 0xFF) != 1 and side == 0 and inp['mut_site'] == 0)
        add('creator_player_say', 12 in k)
        add('creator_not_player', 12 not in k)
        add('group_add', 14 in k)
        add('rand_called', 17 in k)
        add('chance_100', 17 in k and 8 <= (inp['combo'] & 0xFF) <= 12 and inp['mut_site'] == 0)
        add('chance_minus1', 17 in k and (inp['combo'] & 0xFF) == 4 and (inp['move'] & 0xFF) == 4 and inp['mut_site'] == 0)
        add('chance_from_health', 17 in k and not 8 <= (inp['combo'] & 0xFF) <= 12 and not ((inp['combo'] & 0xFF) == 4 and (inp['move'] & 0xFF) == 4) and inp['mut_site'] == 0)
        add('blood_added', 20 in k)
        if 17 in k and 'roll' in oracle.LAST:
            chance, roll = oracle.LAST['chance'], oracle.LAST['roll']
            add('roll_below_chance_by_1', roll == chance - 1)
            add('roll_equals_chance', roll == chance)
            add('roll_above_chance_by_1', roll == chance + 1)
            add('chance_negative', chance < 0)
            add('chance_over_roll_range', chance >= 200)
            add('roll_max', roll >= 198)
            add('roll_zero', roll == 0)
        add('blood_skipped', 17 in k and 20 not in k)
        add('victim_dead_blood', 20 in k and not inp['alive1'] & 1)
        add('victim_alive_blood', 20 in k and inp['alive1'] & 1)
        add('combo_scale_alive', 20 in k and k.count(18) == 2 and inp['alive2'] & 1)
        add('combo_scale_dead', 20 in k and k.count(18) == 2 and not inp['alive2'] & 1)
        add('alive_flip', 20 in k and k.count(18) == 2 and (inp['alive1'] & 1) != (inp['alive2'] & 1))
        add('norm_replaced', 19 in k and inp['norm_mode'] == 1)
        add('norm_passthrough', 19 in k and inp['norm_mode'] == 0)
        add('mut_p1_blood', 20 in k and inp['mut_p1'] == 1)
        add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0 and ((inp['mut_combo'] & 0xFF) != (inp['combo'] & 0xFF) or (inp['mut_move'] & 0xFF) != (inp['move'] & 0xFF)))
        add('damage_nan', is_nan(inp['damage']))
        add('damage_inf', is_inf(inp['damage']))
        add('damage_denormal', is_denormal_bits(inp['damage']))
        add('damage_negative', not is_nan(inp['damage']) and inp['damage'] >> 31 == 1 and f32(inp['damage']) <= -1.0)
        add('damage_over_int32', not is_nan(inp['damage']) and not is_inf(inp['damage']) and abs(f32(inp['damage'])) >= 2147483648.0)
        add('damage_over_int64', not is_nan(inp['damage']) and not is_inf(inp['damage']) and abs(f32(inp['damage'])) >= 9223372036854775808.0)
        add('health_nan', 17 in k and is_nan(inp['health']) and not 8 <= (inp['combo'] & 0xFF) <= 12)
        add('health_inf', 17 in k and is_inf(inp['health']) and not 8 <= (inp['combo'] & 0xFF) <= 12)
        add('brightness_nan', 20 in k and is_nan(inp['brightness']))
        add('brightness_snan', 20 in k and is_snan(inp['brightness']))
        add('brightness_noncanonical_nan', 20 in k and is_nan(inp['brightness']) and inp['brightness'] not in (0x7FC00000, 0xFFC00000))
        add('posn1_nan_blood', 20 in k and any(is_nan(e) for e in events[[i for i, x in enumerate(k) if x == 20][0]][3:6]))
        add('creator_matrix', inp['creator_mat'] == 1)
        add('creator_compact', inp['creator_mat'] == 0)
        add('victim_matrix', 19 in k and inp['victim_mat'] == 1)
        add('victim_compact', 19 in k and inp['victim_mat'] == 0)
        for i, ax in enumerate('xyz'):
            a, b = inp['cpos_' + ax], inp['vpos_' + ax]
            if 19 in k:
                if is_nan(a) and is_nan(b):
                    add('sub_two_nan_axes')
                    add('sub_two_nan_qnan_qnan', not is_snan(a) and not is_snan(b))
                    add('sub_two_nan_snan_snan', is_snan(a) and is_snan(b))
                    add('sub_two_nan_mixed_type', is_snan(a) != is_snan(b))
                    add('sub_two_nan_same_sign', (a >> 31) == (b >> 31))
                    add('sub_two_nan_opposite_sign', (a >> 31) != (b >> 31))
                    add('sub_two_nan_same_significand', ((a | 0x400000) & 0x7FFFFF) == ((b | 0x400000) & 0x7FFFFF) and a != b)
                    add('sub_two_nan_attacker_larger', ((a | 0x400000) & 0x7FFFFF) > ((b | 0x400000) & 0x7FFFFF))
                    add('sub_two_nan_victim_larger', ((b | 0x400000) & 0x7FFFFF) > ((a | 0x400000) & 0x7FFFFF))
                    norm_in = events[[j for j, x in enumerate(k) if x == 19][0]][1 + i]
                    add('sub_two_nan_result_from_attacker', norm_in == (a | 0x400000))
                    add('sub_two_nan_result_from_victim', norm_in == (b | 0x400000))
                elif is_nan(a) or is_nan(b):
                    add('sub_one_nan_noncanonical', (a if is_nan(a) else b) not in (0x7FC00000, 0xFFC00000))
                    add('sub_one_snan', is_snan(a) or is_snan(b))
                elif is_inf(a) or is_inf(b):
                    add('sub_inf')
                elif is_denormal_bits(a) or is_denormal_bits(b):
                    add('sub_denormal')
        if 20 in k and k.count(18) == 2 and inp['alive2'] & 1 and inp['alive1'] & 1:
            d = events[[i for i, x in enumerate(k) if x == 20][0]][6:9]
            add('scale_nan_component', any(is_nan(v) for v in d))
            add('scale_inf_component', any(is_inf(v) for v in d))
            add('scale_denormal_component', any(is_denormal_bits(v) for v in d))
    # recuentos de valores DISTINTOS
    if inp['family'] == 18:
        sets['grid_side_zero' if side == 0 else 'grid_side_nonzero'].add((combo, move))
    if normal and 4 <= combo <= 15 and 0 <= move <= 4:
        sets['real_pairs'].add((combo, move))
    add('grid_case', inp['family'] == 18)
    add('edge_case', inp['family'] == 19)


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
        if any(s['hooked'] != (s['dll'] != 0) for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    tables = oracle.Tables(b)
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    sets = {fn: {k: set() for k in ('grid_side_zero', 'grid_side_nonzero', 'real_pairs')} for fn in NAMES}
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
        for sname in ('original', 'candidate'):
            msg = oracle.check(row['input'], row[sname], tables)
            if msg:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs['result'], cov[fn], sets[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 20 or short or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        cov[fn]['distinct_grid_side_zero'] = len(sets[fn]['grid_side_zero'])
        cov[fn]['distinct_grid_side_nonzero'] = len(sets[fn]['grid_side_nonzero'])
        cov[fn]['distinct_real_pairs'] = len(sets[fn]['real_pairs'])
        need = {k: COVER_FLOOR for k in cov[fn]}
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
        sys.exit(runtime_runner.run_case('fighthitped75', manifest['binaries']['asi'], analyze, 'FightHitPed75', timeout=3600))
