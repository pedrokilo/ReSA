"""Compuertas del banco ForcePedOutVt71 (lote 71): virtuales de CTaskSimpleCarForcePedOut (vtable 0x86EE94): cuerpo del destructor 0x647790 (hook S), Clone 0x649EE0, MakeAbortable 0x647780
y ProcessPed 0x6477F0 (hooks V, ranuras 1, 6 y 7) y destructor con borrado (ranura 0: 0x64BF50 del EXE contra el de la DLL, sin hook). 5 x 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference
vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CTaskSimpleCarForcePedOut::Destructor', 'CTaskSimpleCarForcePedOut::Clone', 'CTaskSimpleCarForcePedOut::MakeAbortable', 'CTaskSimpleCarForcePedOut::ProcessPed',
         'CTaskSimpleCarForcePedOut::DeletingDestructor']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x647790, NAMES[1]: 0x649EE0, NAMES[2]: 0x647780, NAMES[3]: 0x6477F0, NAMES[4]: 0x64BF50}   # la ultima no tiene hook (ruta viva)
VT_SLOTS = [0, 0x86EE98, 0x86EEAC, 0x86EEB0, 0]
# (direccion del EXE, hooked): CleanUpOldReference y CTask::~CTask (0x61A3A0: sin cuerpo en la DLL); CTask::operator new y constructor; sin colaboradores; IsDriver, PositionPedOutOfCollision,
# ComputeOppositeDoorFlag, ComputePassengerIndexFromCarDoor; cuerpo del destructor (sin hook activo en esa ruta) y CTask::operator delete
STUBBED = {NAMES[0]: [(0x571A00, True), (0x61A3A0, True)],
           NAMES[1]: [(0x61A5A0, True), (0x647710, True)],
           NAMES[3]: [(0x6D1C40, True), (0x5E0820, True), (0x64E610, True), (0x64F1E0, True)],
           NAMES[4]: [(0x647790, False), (0x61A5B0, True)]}
EXE_ONLY = {(NAMES[0], 0x61A3A0)}   # sin cuerpo propio en la DLL (integrado en linea)
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values()) | {0x86EE94}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_denormal_value(v):
    return v == v and v not in (float('inf'), float('-inf')) and v != 0 and abs(v) < 1.1754943508222875e-38


def tally(fn, inp, obs, cov):
    """cobertura de ramas desde la entrada y las observaciones"""
    ev = obs['events']
    k = [e[0] for e in ev]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    veh = inp['veh_kind']
    add('veh_null', veh == 0)
    add('veh_arena', veh == 1)
    add('veh_raw', veh == 2)
    add('prefill_seed', inp['prefill'] == 0)
    add('prefill_cd', inp['prefill'] == 1)
    add('prefill_zero', inp['prefill'] == 2)
    add('prefill_ff', inp['prefill'] == 3)
    add('eax_seed_special', inp['eax'] in (0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 1, 0xFFFFFF00, 0xFF, 0xDEADBEEF))
    add('edx_seed_special', inp['edx'] in (0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 1, 0xFFFFFF00, 0xFF, 0xDEADBEEF))
    if fn == 0:
        add('dtor_cleanup_called', k == [3])
        add('dtor_cleanup_skipped', k == [])
        add('dtor_vptr_ok', obs['vptr_ok'] is True)
        add('dtor_ref_is_field_8', len(ev) == 1 and ev[0][2] == 0x10000000 + 0x40 + 8)
    elif fn == 1:
        add('clone_created', k == [1, 20] and obs['eax'] == 0x10000000 + 3 * 0x100000 + 0x40)
        add('clone_alloc_null', k == [1] and obs['eax'] == 0)
        add('clone_size_ok', len(ev) >= 1 and ev[0] == [1, 0x10])
        add('clone_args_vehicle_door', k == [1, 20] and ev[1][3] == inp['door'])
    elif fn == 2:
        add('abort_false', obs['al'] == 0)
        add('abort_prio_extreme', inp['prio'] >= 0x80000000 or inp['prio'] > 5)
        add('abort_ped_null', inp['ped_kind'] == 0)
        add('abort_ped_raw', inp['ped_kind'] == 2)
        add('abort_no_events', not ev)
    elif fn == 3:
        add('proc_veh_null_true', veh == 0 and obs['al'] == 1 and not ev)
        add('proc_driver_true', k == [10, 11] and obs['al'] == 1)
        add('proc_not_driver', len(k) >= 2 and k[0] == 10 and k[1] == 12)
        add('proc_overlap_driver_false', k == [10, 12] and obs['al'] == 0 and inp['driver'] != 0 and (inp['opp_ret'] & inp['getting_out'] & 0xFF) != 0)
        add('proc_overlap_seats_called', k[:3] == [10, 12, 13])
        add('proc_overlap_pass_false', k[:3] == [10, 12, 13] and obs['al'] == 0)
        add('proc_overlap_pass_true', k == [10, 12, 13, 11] and obs['al'] == 1)
        add('proc_no_overlap_true', k == [10, 12, 11] and obs['al'] == 1)
        add('proc_seats_negative', k[:3] == [10, 12, 13] and inp['seats_ret'] >= 0x80000000)
        add('proc_seats_large', k[:3] == [10, 12, 13] and 3 <= inp['seats_ret'] <= 12)
        add('proc_opp_high_bits', len(k) > 1 and k[1] == 12 and (inp['opp_ret'] & 0xFF) == 0 and inp['opp_ret'] > 0xFF)
        add('proc_isdriver_garbage_high', len(k) > 0 and k[0] == 10 and inp['is_driver_ret'] > 0xFF)
        add('proc_pos_flag_one', any(e[0] == 11 and e[4] == 1 for e in ev))
    elif fn == 4:
        add('del_flag_bit0', inp['del_flags'] & 1 != 0 and k == [41, 2])
        add('del_flag_no_bit0', inp['del_flags'] & 1 == 0 and k == [41])
        add('del_flag_high_garbage', inp['del_flags'] > 0xFF)
        add('del_ret_is_this', obs['eax'] == 0x10000000 + 0x40)


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
        if any(s['dll'] == 0 and (fn, s['exe']) not in EXE_ONLY for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if b.get('vtable_slots') != VT_SLOTS or b.get('live_target_in_asi') is not True:
        raise RuntimeError('Ranuras de vtable o ruta viva inesperadas')
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
        tally(NAMES.index(fn), row['input'], obs, cov[fn])
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
        sys.exit(runtime_runner.run_case('forcepedoutvt71', manifest['binaries']['asi'], analyze, 'ForcePedOutVt71', timeout=3600))
