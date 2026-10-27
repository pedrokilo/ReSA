"""Compuertas del banco LeaveCarCtor71 (lote 71): constructor 0x63B8C0 y destructor 0x63B970 de CTaskComplexLeaveCar y constructor 0x62F1A0 de CTaskComplexLeaveCarAndDie (hooks S), 3 x 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks S registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
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

NAMES = ['CTaskComplexLeaveCar::CTaskComplexLeaveCar', 'CTaskComplexLeaveCar::~CTaskComplexLeaveCar', 'CTaskComplexLeaveCarAndDie::CTaskComplexLeaveCarAndDie']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x63B8C0, NAMES[1]: 0x63B970, NAMES[2]: 0x62F1A0}
# (direccion del EXE, hooked) en el orden del banco: constructores = CEntity::RegisterReference 0x571B70; destructor = CVehicle::ClearGettingOutFlags 0x6D3060, CEntity::CleanUpOldReference 0x571A00 y,
# el ultimo, operator delete 0x8214BD (sin hook: la DLL reenvia a la misma direccion)
STUBBED = {NAMES[0]: [(0x571B70, True)], NAMES[1]: [(0x6D3060, True), (0x571A00, True), (0x8214BD, False)], NAMES[2]: [(0x571B70, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def tally(fn, inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos (20 = RegisterReference, 7 = ClearGettingOutFlags, 22 = CleanUpOldReference, 23 = operator delete, 21 = destructor de la subtarea)"""
    k = [e[0] for e in events]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    if fn != NAMES[1]:
        add('veh_null_no_register', inp['veh_kind'] == 0 and k == [])
        add('veh_arena_register', inp['veh_kind'] == 1 and k == [20])
        add('veh_raw_register', inp['veh_kind'] == 2 and k == [20])
        add('register_ref_is_field_0xc', k == [20] and events[0][2] == 0x10000000 + 0x40 + inp['shift'] + 0xC)
        for m in range(8):
            add('fill_mode_%d' % m, inp['fill_mode'] == m)
            add('shift_%d' % m, inp['shift'] == m)
        add('door_zero', inp['door'] == 0)
        add('door_vehicle_doors', inp['door'] in (8, 9, 10, 11))
        add('door_odd', inp['door'] in (0xFFFFFFFF, 0x80000000, 0x100))
        add('delay_zero', inp['delay'] == 0)
        add('delay_negative', inp['delay'] >= 0x80000000)
        add('type_known', obs['type'] != 0xFFFFFFFF and obs['vptr_written'] is True)
        add('eax_is_this', obs['eax_is_this'] is True)
        if fn == NAMES[0]:
            add('bools_zero_one', inp['sensible'] <= 1 and inp['force'] <= 1)
            add('bools_garbage_high', inp['sensible'] > 1 or inp['force'] > 1)
            for sv in (0, 1):
                for fv in (0, 1):
                    add('bools_%d%d' % (sv, fv), (inp['sensible'] & 0xFF) == sv and (inp['force'] & 0xFF) == fv)
    else:
        add('veh_null', inp['veh_kind'] == 0 and 7 not in k and 22 not in k)
        add('veh_arena', inp['veh_kind'] == 1 and k[:1] == [7] and 22 in k)
        add('order_clear_clean_delete_sub', k == [7, 22, 21] or k == [7, 22, 23, 21])
        add('util_null', inp['util_kind'] == 0 and 23 not in k)
        add('util_arena_delete', inp['util_kind'] == 1 and 23 in k)
        add('util_raw_delete', inp['util_kind'] == 2 and 23 in k)
        add('sub_null', inp['sub_kind'] == 0 and 21 not in k)
        add('sub_deleted_flag_1', inp['sub_kind'] == 1 and 21 in k and [e for e in events if e[0] == 21][0][2] == 1)
        add('flags_zero', inp['flags_set'] == 0)
        add('flags_ff', inp['flags_set'] == 0xFF)
        add('num_in_zero', inp['num_in'] == 0)
        add('num_in_ff', inp['num_in'] == 0xFF)
        add('vptr_exe', inp['vptr_kind'] == 0)
        add('vptr_noise', inp['vptr_kind'] == 1)
        for m in range(8):
            add('fill_mode_%d' % m, inp['fill_mode'] == m)
        add('vptr_changed', obs['vptr_changed'] is True)
        add('veh_byte_changed', inp['veh_kind'] == 1 and len(obs['changes'][1]) >= 1)


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
    if b.get('vtable_slot') != 0:
        raise RuntimeError('Hook S: no debe haber ranura de vtable')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        want = STUBBED.get(fn, [])
        if [a for a, _ in got] != [a for a, _ in want] or any(h is not None and h != g for (_, g), (_, h) in zip(got, want)):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        if any(s['hooked'] != (s['dll'] != 0) for s in col[fn]['stubbed'] if s['exe'] != 0x8214BD):
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(fn, row['input'], obs['events'], obs, cov[fn])
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
        sys.exit(runtime_runner.run_case('leavecarctor71', manifest['binaries']['asi'], analyze, 'LeaveCarCtor71', timeout=3600))
