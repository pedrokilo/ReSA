"""Compuertas del banco PlayerPed79 (lote 79B): CPlayerPed::SetInitialState (EXE 0x60CD20, hook S).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que CADA lado de cada par coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las
entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CPlayerPed::SetInitialState', 'CPlayerPed::FindWeaponLockOnTarget']
BASE_CASES = 16384
FAMILY_SIZES = {16: 1024, 17: 1024, 18: 1024, 19: 1024}   # familias 16..19; las 16 primeras: 1024 cada una
CASES = BASE_CASES + sum(FAMILY_SIZES.values())
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60CD20, NAMES[1]: 0x60DC50}
# (direccion del EXE, hooked) EN EL ORDEN de LiveComparison.hpp
STUBBED = {NAMES[0]: [(0x71D780, False), (0x5DEF20, True), (0x5E1950, True), (0x5393F0, True), (0x5E4500, True), (0x5DEC00, True), (0x601DA0, True), (0x4D6C00, True), (0x61A5A0, True),
                      (0x681AF0, True), (0x60A9C0, False), (0x681B60, True), (0x5FBB70, True), (0x4B57A0, True), (0x4AC490, True), (0x4B5620, False), (0x4B5630, False), (0x5F7470, True),
                      (0x56F210, True), (0x571B70, True), (0x571A00, True), (0x821B1E, False)],
           NAMES[1]: [(0x5E6580, True), (0x743C60, True), (0x73B380, True), (0x53CC70, True), (0x53FC90, True), (0x53FD30, True), (0x53CBE0, True), (0x53CB50, True), (0x59C910, False), (0x4082C0, False),
                      (0x60D020, True), (0x60B550, True), (0x5F7F40, True), (0x49CCE0, True), (0x49CF90, True), (0x564A20, True), (0x59F320, True), (0x609F80, True), (0x441390, True),
                      (0x571B70, True), (0x571A00, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x60CD20, 0x60DC50}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COVER_FLOOR = 1   # todo contador medido debe alcanzarse al menos una vez; coverage.json sube los minimos


def tally1(inp, events, cov, ret):
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('target_kept', inp['tgt'] and ret == 1 and 31 not in k and 30 not in k)
    add('target_cleared', 31 in k and 30 not in k and ret == 0)
    add('target_branch', 3 in k and 4 not in k)
    add('no_target', 4 in k)
    add('stick_active', 7 in k)
    add('stick_idle', 4 in k and 7 not in k)
    add('spray_find', 16 in k)
    add('spray_tag_skipped', 14 in k and 15 not in k)
    add('spray_alpha_checked', 15 in k)
    add('spray_los', 12 in k)
    add('spray_target_set', 12 in k and 30 in k)
    add('spray_los_blocked', 12 in k and 30 not in k)
    add('ped_group_checked', 13 in k)
    add('ped_normalise', 9 in k)
    add('ped_magnitude', 10 in k)
    add('eval_called', 11 in k)
    add('eval_ped', any(e[0] == 11 and e[7] == 0 for e in events))
    add('eval_object_or_vehicle', any(e[0] == 11 and e[7] == 1 for e in events))
    add('object_checked', 17 in k)
    add('reach_called', 18 in k)
    add('coop_checked', 19 in k)
    add('final_target_set', 30 in k and ret == 1)
    add('final_false', ret == 0)
    add('final_true', ret == 1)
    add('cleanup_before_set', 31 in k and 30 in k)
    def cls(bits_):
        e = (bits_ >> 23) & 0xFF
        return 'nan' if e == 0xFF and bits_ & 0x7FFFFF else 'inf' if e == 0xFF else 'zero' if bits_ & 0x7FFFFFFF == 0 else 'finite'
    if inp['tgt'] and 3 in k and 4 not in k:
        add('target_pos_nan', any(cls(inp[x]) == 'nan' for x in ('tx', 'ty', 'px', 'py')))
        add('target_pos_inf', any(cls(inp[x]) == 'inf' for x in ('tx', 'ty', 'px', 'py')))
        add('target_range_' + cls(inp['rng']))
        add('target_mult_' + cls(inp['mult0']))
    add('ped_atan_arg_nan', any(e[0] == 4 and (cls(e[1]) == 'nan' or cls(e[2]) == 'nan') for e in events))
    add('ped_atan_arg_inf', any(e[0] == 4 and (cls(e[1]) == 'inf' or cls(e[2]) == 'inf') for e in events))
    add('spray_forward_nan', 16 in k and (cls(inp['fx']) == 'nan' or cls(inp['fy']) == 'nan'))
    add('eval_prio_changed', any(e[0] == 11 and e[3] != 0xC61C4000 for e in events))
    add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0)
    for b in (1, 2, 4, 8):
        add('mut_kind_bit_%d' % b, inp['mut_site'] != 0 and inp['mut_kind'] & b != 0)
    add('pad_two', 5 in k and any(e[0] == 5 and e[1] == 1 for e in events))
    add('spray_weapon', inp['wtype'] == 41)
    add('slot_negative', inp['slot'] & 0x80 != 0)


def tally(inp, events, cov):
    """cobertura de ramas desde la entrada y la traza de eventos del candidato"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('group_created_skip', inp['arg'] & 0xFF != 0)
    add('arg_high_garbage', inp['arg'] >> 8 != 0)
    add('group_block_ran', 12 in k)
    add('group_block_skipped_flag400', inp['arg'] & 0xFF == 0 and 12 not in k and 11 in k)
    add('will_true', 15 in k)
    add('will_false', 14 in k and 15 not in k)
    add('will_garbage_high', 14 in k and inp['will'] >> 8 != 0)
    add('fire_extinguish', 3 in k)
    add('fire_none', 3 not in k)
    add('task1_fail', inp['task_fail1'] != 0)
    add('task2_fail', inp['task_fail2'] != 0)
    add('task1_ok', inp['task_fail1'] == 0)
    add('task2_ok', inp['task_fail2'] == 0 and 21 in k)
    add('attached_nonzero', inp['fFC'] != 0)
    add('attached_zero', inp['fFC'] == 0)
    add('attached_nonzero_bit0_clear', inp['fFC'] != 0 and inp['f1C'] & 1 == 0)
    add('attached_nonzero_bit0_set', inp['fFC'] != 0 and inp['f1C'] & 1 == 1)
    add('no_data', inp['has_data'] == 0)
    add('player_data_called', 17 in k)
    add('event_delete', 20 in k)
    add('task_id_negative', 16 in k and inp['task_id'] & 0x8000 != 0)
    add('task_id_wide', 16 in k and inp['task_id'] >> 16 != 0)
    add('groups_distinct', 12 in k and inp['group'] != inp['ogroup'])
    for g in range(8):
        add('group_%d' % g, 12 in k and inp['group'] == g)
    add('mut_site_%d' % inp['mut_site'], inp['mut_site'] != 0)
    for b in (1, 2, 4, 8, 16):
        add('mut_kind_bit_%d' % b, inp['mut_site'] != 0 and inp['mut_kind'] & b != 0)
    add('flags_400_initial', inp['dflags'] & 0x400 != 0)


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
        if any(s['hooked'] and s['dll'] == 0 for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador con hook sin resolver en ' + fn)
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
        if fn == NAMES[0]:
            tally(row['input'], row['candidate']['observed']['events'], cov[fn])
        else:
            tally1(row['input'], row['candidate']['observed']['events'], cov[fn], row['candidate']['observed']['ret_al'])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        expected_fam = {f: 1024 for f in range(16)}
        expected_fam.update(FAMILY_SIZES)
        if len(fam[fn]) != 20 or fam[fn] != expected_fam:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
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
        sys.exit(runtime_runner.run_case('playerped79', manifest['binaries']['asi'], analyze, 'PlayerPed79', timeout=3600))
