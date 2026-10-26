"""Compuertas del banco LeaveCarNext70 (lote 70): CTaskComplexLeaveCar::CreateNextSubTask (virtual, slot 8 de la vtable 0x86E828, EXE 0x6419F0), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook V registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada y ranura de la vtable)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CTaskComplexLeaveCar::CreateNextSubTask']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6419F0}
VT_SLOT = 0x86E828 + 8 * 4
# (direccion del EXE, hooked) en el orden del banco: CreateSubTask 0x641530 (hooked libre: la reconstruye otro agente; sin hook se parchea la envoltura de la DLL), constructor de CTaskUtilityLineUpPedWithCar, CPed::IsPlayer, CCarEnterExit::ComputeDoorFlag, CVehicle::SetGettingOutFlags y ClearGettingOutFlags, CPed::SetPedState,
# CCarEnterExit::ComputeTargetDoorToExit, CVehicle::IsPassenger(CPed*) e IsDriver(const CPed*), CCarEnterExit::IsRoomForPedToLeaveCar y CarHasDoorToClose, CDamageManager::GetDoorStatus_Component y SetDoorStatus_Component y, el ultimo, operator new 0x82119A (sin hook: la DLL reenvia a la misma direccion)
STUBBED = {NAMES[0]: [(0x641530, None), (0x64FBB0, True), (0x5DF8F0, True), (0x64E550, True), (0x6D3020, True), (0x6D3060, True), (0x5E4500, True), (0x64F110, True),
                      (0x6D1BD0, True), (0x6D1C40, True), (0x6504C0, True), (0x64EE10, True), (0x6C2250, True), (0x6C21E0, True), (0x82119A, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6419F0, VT_SLOT}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
MAIN_TYPES = (0xCA, 0xCE, 0xD4, 0x2CC, 0x2CF, 0x326, 0x329, 0x32A, 0x32D, 0x32E, 0x32F, 0x330, 0x337)


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    t = inp['sub_type']
    created = [e[2] for e in events if e[0] == 1]          # tipos pedidos a CreateSubTask
    last = created[-1] if created else None
    veh_changes = {c[0] for c in obs['changes'][3]}
    task_changes = {c[0] for c in obs['changes'][0]}
    ped_changes = {c[0] for c in obs['changes'][2]}

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('other_type', t not in MAIN_TYPES)
    add('other_type_returns_null', t not in MAIN_TYPES and obs['eax'] == 0 and k == [17])
    add('pause', t == 0xCA and last == 0x516)
    add('die_type', t == 0xD4 and last == 0x516)
    add('getup_in_vehicle', t == 0xCE and last == 0x330)
    add('getup_not_in_vehicle', t == 0xCE and last == 0x516)
    add('leave_boat_die', t == 0x2CC and last == 0xD4)
    add('leave_boat_finished', t == 0x2CC and last == 0x516)
    add('drive_not_in_vehicle', t == 0x2CF and 10 not in k)
    add('drive_neither_clears_flag', t == 0x2CF and 10 in k and 11 in k and 0x46D in ped_changes)
    add('drive_passenger', t == 0x2CF and 10 in k and 11 not in k)
    add('drive_driver', t == 0x2CF and 11 in k and 0x46D not in ped_changes)
    add('drive_compute_door', t == 0x2CF and 9 in k)
    add('drive_door_set_by_compute', t == 0x2CF and 9 in k and 0x10 in task_changes)
    add('drive_wait_door', t == 0x2CF and last == 0x32A)
    add('drive_door_getting_in_finished', t == 0x2CF and 10 in k and last == 0x516 and inp['force'] == 0)
    add('drive_wait_slow', t == 0x2CF and last == 0x329)
    add('drive_force', t == 0x2CF and inp['force'] == 1 and last == 0x329)
    add('close_door_type', t == 0x326 and last == 0x330)
    add('wait_door_type', t == 0x32A and last == 0x329)
    add('force_out_type', t == 0x32F and last == 0x330)
    add('set_out_die', t == 0x330 and last == 0xD4)
    add('set_out_finished', t == 0x330 and last == 0x516)
    ws = t == 0x329
    add('ws_type', ws)
    add('ws_cannot_step', ws and 16 in k and 12 not in k and last == 0x32E)
    add('ws_first_room_ok', ws and inp['can_step'] and (inp['room_bits'] & 1))
    rooms = [e for e in events if e[0] == 12]
    add('ws_second_room', ws and len(rooms) == 2)
    for d in (0, 8, 9, 10, 11):
        add('ws_alt_door_%d' % d, ws and len(rooms) == 2 and rooms[1][2] == d)
    add('ws_default_door_no_alt', ws and len(rooms) == 1 and inp['can_step'] and not (inp['room_bits'] & 1))
    add('ws_give_up', ws and last == 0x32F)
    add('ws_dragged_out', ws and last == 0x337)
    add('ws_get_out', ws and last == 0x32D)
    add('ws_lineup_created', ws and 3 in k)
    add('ws_lineup_alloc_null', ws and 2 in k and 3 not in k)
    add('ws_player_checks', ws and k.count(4) >= 2)
    add('ws_autopilot_reset', ws and (0x3BA in veh_changes or 0x3D0 in veh_changes))
    add('ws_status_forced', ws and 0x36 in veh_changes)
    add('ws_nodes_check', ws and len(rooms) == 2 and (inp['model_flags'] >> 15) & 1 and inp['v_type'] == 0)
    add('ws_target_door_changed', ws and 0x10 in task_changes)
    go = t == 0x32D
    add('go_type', go)
    add('go_close_door', go and last == 0x326)
    add('go_set_out', go and last == 0x330)
    add('go_no_door_status', go and 14 not in k)
    add('go_door_status_three_calls', go and k.count(14) == 3)
    add('go_door_status_two_calls', go and k.count(14) == 2)
    add('go_door_status_set', go and 15 in k)
    add('go_door_has_door_twice', go and k.count(13) == 2)
    add('go_force_lock_unlocked_skip', go and inp['force'] == 1 and inp['lock'] == 1)
    jo = t == 0x32E
    add('jo_type', jo)
    add('jo_die', jo and inp['die'] == 1 and last == 0xD4)
    add('jo_veh_null', jo and inp['die'] == 0 and inp['veh_null'] == 1 and last == 0x516)
    add('jo_bike', jo and inp['die'] == 0 and inp['veh_null'] == 0 and inp['v_type'] == 9 and last == 0x516)
    add('jo_quad', jo and inp['die'] == 0 and inp['veh_null'] == 0 and inp['v_type'] != 9 and inp['v_sub'] == 2 and last == 0x516)
    add('jo_in_air', jo and inp['die'] == 0 and inp['veh_null'] == 0 and last == 0x330 and inp['in_air'] == 1)
    add('jo_drowning', jo and last == 0x330 and inp['in_air'] == 0)
    add('jo_clear_flags', jo and 7 in k and last == 0xCE)
    bd = t == 0x337
    add('bd_type', bd)
    add('bd_veh_null', bd and inp['veh_null'] == 1)
    add('bd_lock_reset', bd and 0x4F8 in veh_changes)
    add('ret_nonnull', obs['eax'] != 0)
    add('ret_null_with_create', obs['eax'] == 0 and 1 in k)


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
    if b.get('vtable_slot') != VT_SLOT:
        raise RuntimeError('Ranura de vtable inesperada')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        want = STUBBED.get(fn, [])
        if [a for a, _ in got] != [a for a, _ in want] or any(h is not None and h != g for (_, g), (_, h) in zip(got, want)):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        if any(s['hooked'] != (s['dll'] != 0) for s in col[fn]['stubbed'] if s['exe'] != 0x82119A and s['exe'] != 0x641530):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
        if any(s['dll'] == 0 for s in col[fn]['stubbed'] if s['exe'] == 0x641530):
            raise RuntimeError('Cuerpo de CreateSubTask sin resolver')
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 16 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('leavecarnext70', manifest['binaries']['asi'], analyze, 'LeaveCarNext70', timeout=3600))
