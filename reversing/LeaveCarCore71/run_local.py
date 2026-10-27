"""Compuertas del banco LeaveCarCore71 (lote 71): CTaskComplexLeaveCar::ControlSubTask (virtual, slot 10 de la vtable 0x86E828, EXE 0x6421B0), 16384 pares.

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

NAMES = ['CTaskComplexLeaveCar::ControlSubTask']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6421B0}
VT_SLOT = 0x86E828 + 10 * 4
# (direccion del EXE, hooked) en el orden del banco: CreateSubTask 0x641530 (hooked libre: sin hook se parchea la envoltura de la DLL)
STUBBED = {NAMES[0]: [(0x641530, None)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6421B0, VT_SLOT}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos (1 = CreateSubTask, 17 = GetTaskType, 18 = MakeAbortable)"""
    k = [e[0] for e in events]
    types = [e for e in events if e[0] == 17]
    creates = [e for e in events if e[0] == 1]
    aborts = [e for e in events if e[0] == 18]

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('veh_null', inp['veh_kind'] == 0 and obs['eax'] == 0 and k == [])
    add('veh_raw', inp['veh_kind'] == 2)
    add('in_veh_sensible_no_calls', inp['veh_kind'] != 0 and inp['in_veh'] == 1 and inp['sensible'] == 1 and k == [])
    add('in_veh_not_sensible_wait_abort', inp['veh_kind'] != 0 and inp['in_veh'] == 1 and inp['sensible'] == 0 and inp['type0'] == 0x329 and len(aborts) == 1 and len(types) == 1)
    add('in_veh_not_sensible_other', inp['veh_kind'] != 0 and inp['in_veh'] == 1 and inp['sensible'] == 0 and inp['type0'] != 0x329 and not aborts and len(types) == 1)
    add('out_one_type_call', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) == 1)
    add('out_two_type_calls', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) == 2)
    add('out_three_type_calls', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) == 3)
    add('out_four_type_calls', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) == 4)
    add('out_five_type_calls', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) == 5)
    add('out_setout_then_sensible_check', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and inp['type0'] == 0x330 and inp['sensible'] == 1 and len(types) == 1)
    add('out_setout_then_unsensible_check', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and inp['type0'] == 0x330 and inp['sensible'] == 0 and len(types) == 2)
    add('out_jumpout_second', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and inp['type0'] not in (0x330,) and inp['type1'] == 0x32E and len(types) >= 2)
    add('out_getup_third', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and inp['type0'] not in (0x330, ) and inp['type1'] not in (0x32E, ) and inp['type2'] == 0xCE and len(types) >= 3)
    add('out_die_fourth', inp['veh_kind'] != 0 and inp['in_veh'] == 0 and len(types) >= 4 and inp['type3'] == 0xD4 and inp['type0'] != 0x330 and inp['type1'] != 0x32E and inp['type2'] != 0xCE)
    add('out_create_setped_out', len(creates) == 1 and creates[0][2] == 0x330)
    add('out_create_returned_task', len(creates) == 1 and obs['eax'] != 0 and inp['create_null'] == 0)
    add('out_create_returned_null', len(creates) == 1 and obs['eax'] == 0 and inp['create_null'] == 1)
    add('abort_args', len(aborts) == 1 and aborts[0][3] == 0 and aborts[0][4] == 0)
    add('abort_ret_true', len(aborts) == 1 and inp['abort_ret'] == 1)
    add('abort_ret_false', len(aborts) == 1 and inp['abort_ret'] == 0)
    add('abort_after_second_type_call', len(aborts) == 1 and len(types) == 2)
    add('returns_subtask', obs['eax'] != 0 and not creates)
    add('returns_null_without_create', obs['eax'] == 0 and not creates)


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
        sys.exit(runtime_runner.run_case('leavecarcore71', manifest['binaries']['asi'], analyze, 'LeaveCarCore71', timeout=3600))
