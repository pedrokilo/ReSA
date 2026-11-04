"""Compuertas independientes del banco Fn87B (lote 87B): CPlayerPed::FindNextWeaponLockOnTarget (EXE 0x60E530; hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py: AL, traza de colaboradores con todos sus argumentos y bytes cambiados en las arenas), la cobertura por familia y la
cobertura de ramas medida sobre las entradas y la traza (REQUIRED con minimo y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo)."""
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

NAMES = ['CPlayerPed::FindNextWeaponLockOnTarget']
CASES = 20480
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60E530}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp: con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook la DLL llama a la direccion del EXE
STUBBED = {NAMES[0]: [(0x5E6580, True), (0x743C60, True), (0x53CC70, True), (0x73B380, True), (0x609F80, True), (0x60B550, True), (0x5F7F40, True), (0x59F320, True), (0x441390, True),
                      (0x60D1C0, True), (0x5F7E80, True), (0x4B5620, False), (0x5F7470, True), (0x4AB420, True), (0x571B70, True), (0x571A00, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x60E530}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
REQUIRED = ['ret_true', 'ret_false', 'heading_cur', 'heading_cam', 'cur_pool_ped_skipped', 'ped_group_checked', 'ped_los_checked', 'ped_reach', 'ped_mult', 'ped_eval', 'obj_checked', 'obj_eval', 'veh_checked_coop',
            'veh_coop_off', 'veh_eval', 'stale_mult_cur', 'stale_mult_ped', 'event_group_path', 'event_stack_path', 'event_melee', 'event_not_ped', 'best_ped', 'best_obj', 'best_veh',
            'target_cleanup', 'no_prev_target', 'arg1_garbage', 'arg1_true', 'arg1_false', 'cam_matrix', 'cam_placement', 'cur_matrix', 'cur_placement', 'slot_negative', 'atan_arg_nan', 'atan_arg_inf',
            'mult_nan', 'mult_inf', 'range_nan', 'range_zero', 'eval_prio_changed', 'pool_empty_peds', 'ped0_free_with_peds', 'fire_nonzero_high', 'two_player_byte_zero_dword_nonzero']
REQUIRED += ['ped_skip_free', 'ped_skip_current', 'ped_skip_state', 'ped_skip_veh_none', 'ped_skip_veh_notbike', 'ped_veh_bike_ok', 'ped_veh_flag4_ok', 'ped_skip_never', 'ped_skip_samegroup', 'ped_skip_twoplayer',
             'ped_skip_los', 'ped_skip_reach', 'ped_evaluated', 'obj_skip_free', 'obj_skip_notarget', 'obj_skip_flag40', 'obj_skip_norw', 'obj_skip_reach', 'obj_evaluated', 'veh_skip_free', 'veh_skip_phys',
             'veh_skip_bmx', 'veh_skip_reach', 'veh_evaluated', 'no_best', 'best_type_ped', 'event_group_built', 'event_group_same', 'event_stack_built']
REQUIRED += ['mut_site_%d' % s for s in range(1, 15)] + ['mut_kind_bit_%d' % b for b in (1, 2, 4, 8)]


def cls(bits_):
    e = (bits_ >> 23) & 0xFF
    return 'nan' if e == 0xFF and bits_ & 0x7FFFFF else 'inf' if e == 0xFF else 'zero' if bits_ & 0x7FFFFFFF == 0 else 'finite'


def tally(inp, events, ret):
    cov = {}
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    arena = lambda t: (t >> 20) & 0xFF if t >= 0x10000000 else None
    add('ret_true', ret == 1)
    add('ret_false', ret == 0)
    add('heading_cur', inp['cur'] != 0)
    add('heading_cam', inp['cur'] == 0)
    add('cur_pool_ped_skipped', inp['cur'] >= 2 and (inp['cur'] - 2) < inp['ped_size'] and inp['ped%d_pr' % (inp['cur'] - 2)] == 1)
    add('ped_group_checked', 7 in k)
    add('ped_los_checked', 6 in k)
    add('ped_reach', 5 in k)
    mults = [e for e in events if e[0] == 4]
    evals = [e for e in events if e[0] == 10]
    add('ped_mult', any(arena(e[1]) == oracle.T_PEDS and arena(e[2]) == oracle.T_PL for e in mults))
    add('ped_eval', any(arena(e[2]) == oracle.T_PEDS for e in evals))
    add('obj_checked', 8 in k)
    add('obj_eval', any(arena(e[2]) == oracle.T_OBJS for e in evals))
    add('veh_checked_coop', 9 in k and inp['coop'] & 1 == 1)
    add('veh_coop_off', 9 in k and inp['coop'] & 1 == 0)
    add('veh_eval', any(arena(e[2]) == oracle.T_VEHS for e in evals))
    # el multiplicador de objetos / vehiculos se pide con el ultimo peaton (o el objetivo actual si el pool de peatones esta vacio)
    cur_tag = 0 if inp['cur'] == 0 else (oracle.tag(oracle.T_CUR, 0) if inp['cur'] == 1 else oracle.tag(oracle.T_PEDS, (inp['cur'] - 2) * oracle.PED_STRIDE))
    add('stale_mult_cur', inp['ped_size'] == 0 and cur_tag != 0 and any(e[1] == cur_tag for e in mults))
    add('stale_mult_ped', any(events[i][0] == 10 and arena(events[i][2]) in (oracle.T_OBJS, oracle.T_VEHS) and events[i - 1][0] == 4 and arena(events[i - 1][1]) == oracle.T_PEDS for i in range(1, len(events))))
    add('event_group_path', 13 in k)
    add('event_stack_path', 14 in k)
    n2 = sum(1 for x in k if x == 2)
    add('event_melee', n2 == 2 and 11 not in k)
    add('event_not_ped', n2 == 1 and ret == 1)
    reg = [e for e in events if e[0] == 30]
    if ret == 1 and reg:
        a = arena(events[-1][1])
        add('best_ped', a == oracle.T_PEDS)
        add('best_obj', a == oracle.T_OBJS)
        add('best_veh', a == oracle.T_VEHS)
    add('target_cleanup', ret == 1 and any(e[0] == 31 and e[1] == oracle.tag(oracle.T_TGT, 0) and e[2] == oracle.tag(oracle.T_PL, 0x71C) for e in events))
    add('no_prev_target', ret == 1 and not any(e[0] == 31 and e[1] == oracle.tag(oracle.T_TGT, 0) and e[2] == oracle.tag(oracle.T_PL, 0x71C) for e in events))
    add('arg1_garbage', inp['arg1'] >> 8 != 0)
    add('arg1_true', inp['arg1'] & 0xFF != 0)
    add('arg1_false', inp['arg1'] & 0xFF == 0)
    add('cam_matrix', inp['cam_mt'] == 1)
    add('cam_placement', inp['cam_mt'] == 0)
    add('cur_matrix', inp['cur'] == 1 and inp['cmt'] == 1)
    add('cur_placement', inp['cur'] == 1 and inp['cmt'] == 0)
    add('slot_negative', inp['slot'] & 0x80 != 0)
    add('atan_arg_nan', any(e[0] == 3 and (cls(e[1]) == 'nan' or cls(e[2]) == 'nan') for e in events))
    add('atan_arg_inf', any(e[0] == 3 and (cls(e[1]) == 'inf' or cls(e[2]) == 'inf') for e in events))
    add('mult_nan', any(cls(inp['mult%d' % i]) == 'nan' for i in range(12)) and len(mults) > 0)
    add('mult_inf', any(cls(inp['mult%d' % i]) == 'inf' for i in range(12)) and len(mults) > 0)
    add('range_nan', cls(inp['rng']) == 'nan' and len(mults) > 0)
    add('range_zero', cls(inp['rng']) == 'zero' and len(mults) > 0)
    add('eval_prio_changed', any(e[3] != 0xC61C4000 for e in evals))
    add('pool_empty_peds', inp['ped_size'] == 0)
    add('ped0_free_with_peds', inp['ped_size'] > 0 and inp['ped0_pr'] == 0 and (inp['obj_size'] > 0 or inp['veh_size'] > 0))
    add('fire_nonzero_high', inp['fire'] != 0 and inp['fire'] & 0xFF == 0)
    add('two_player_byte_zero_dword_nonzero', inp['two_player'] != 0 and inp['two_player'] & 0xFF == 0)
    add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0)
    for b in (1, 2, 4, 8):
        add('mut_kind_bit_%d' % b, inp['mut_site'] != 0 and inp['mut_kind'] & b != 0)
    return cov


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if [(s['exe'], s['hooked']) for s in stubbed] != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        for s in stubbed:
            if s['hooked'] and not s['dll']:
                raise RuntimeError('Colaborador con hook sin cuerpo de la DLL en ' + fn)
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
        for sname in ('original', 'candidate'):
            msg = oracle.check(fn, row['input'], row[sname], b)
            if msg:
                raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (sname, fn, case, msg))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        stats = {}
        oracle.model(row['input'], stats)
        stats.update(tally(row['input'], row['candidate']['observed']['events'], row['candidate']['observed']['ret_al']))
        for key, v in stats.items():
            cov[fn][key] = cov[fn].get(key, 0) + v
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        expected_fam = {f: 1024 for f in range(20)}
        if fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in REQUIRED}
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
        sys.exit(runtime_runner.run_case('fn87b', manifest['binaries']['asi'], analyze, 'Fn87B', timeout=3600))
