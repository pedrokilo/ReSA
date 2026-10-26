"""Compuertas del banco LeaveCarSub70 (lote 70): CTaskComplexLeaveCar::CreateSubTask(eTaskType, CPed*) (NO virtual, EXE 0x641530, hook S), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['CTaskComplexLeaveCar::CreateSubTask']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x641530}
# (direccion del EXE, hooked): CTask::operator new, CPed::IsPlayer, CEntity::RegisterReference, ctor Pause, GetUpAndStandStill, Die(1), LeaveBoat, CarDriveTimed, CloseDoorFromOutside, WaitToSlowDown,
# WaitForDoorNotToBeInUse, CarGetOut, CarJumpOut, CarForcePedOut (sin clase en la DLL: solo EXE), ComplexCarSlowBeDraggedOut
STUBBED = {NAMES[0]: [(0x61A5A0, True), (0x5DF8F0, True), (0x571B70, True), (0x48E750, True), (0x678130, True), (0x62FA00, True), (0x63BB60, True), (0x5FF940, True), (0x6464F0, True),
                      (0x646990, True), (0x646B70, True), (0x6471D0, True), (0x64BE40, True), (0x647710, False), (0x648490, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x641530}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
# tipo -> (nombre, tamano reservado, clase de evento del constructor)
TYPES = {202: ('pause', 0x18, 10), 206: ('getup', 0xC, 11), 212: ('die', 0x28, 12), 716: ('boat', 0x14, 13), 719: ('drive', 0x70, 14), 806: ('closedoor', 0x1C, 15),
         809: ('waitslow', 0x14, 16), 810: ('waitdoor', 0x14, 17), 813: ('getout', 0x20, 18), 814: ('jumpout', 0x24, 19), 815: ('forceout', 0x10, 20), 816: ('setpedout', 0x18, 3),
         823: ('slowdrag', 0x1C, 21)}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


def is_denormal_value(v):
    return v == v and v not in (float('inf'), float('-inf')) and v != 0 and abs(v) < 1.1754943508222875e-38


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    t = inp['type']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    main = t in TYPES
    for ty, (nm, size, ek) in TYPES.items():
        if t == ty:
            add(nm + '_type')
            add(nm + '_created', 1 in k and obs['eax'] != 0)
            add(nm + '_alloc_null', 1 in k and obs['eax'] == 0)
            new = [e for e in events if e[0] == 1]
            add(nm + '_size_ok', len(new) == 1 and new[0][1] == size)
    add('other_type', not main)
    add('other_type_no_events', not main and not events)
    add('other_type_returns_null', not main and obs['eax'] == 0)
    add('type_negative', t >= 0x80000000)
    add('ret_nonnull', obs['eax'] != 0)
    add('veh_null', inp['veh_kind'] == 0 and main)
    add('veh_arena', inp['veh_kind'] == 1 and main)
    add('veh_raw', inp['veh_kind'] == 2 and main)
    add('line_null', inp['line_kind'] == 0 and t in (806, 813, 814))
    add('line_arena', inp['line_kind'] == 1 and t in (806, 813, 814))
    add('line_raw', inp['line_kind'] == 2 and t in (806, 813, 814))
    add('noncanonical_flag_byte', t == 816 and inp['sensible'] > 1)
    add('slow_die', t == 809 and inp['die'] != 0 and 2 not in k)
    add('slow_sensible_player', t == 809 and inp['die'] == 0 and inp['sensible'] != 0 and 2 in k and (inp['is_player_ret'] & 0xFF) != 0 and 1 in k)
    add('slow_sensible_not_player', t == 809 and inp['die'] == 0 and inp['sensible'] != 0 and 2 in k and (inp['is_player_ret'] & 0xFF) == 0 and 1 in k)
    add('slow_not_sensible', t == 809 and inp['die'] == 0 and inp['sensible'] == 0 and 2 not in k)
    add('slow_player_before_new', t == 809 and 2 in k and 1 in k and k.index(2) < k.index(1))
    add('slow_player_alloc_null', t == 809 and 2 in k and 1 in k and obs['eax'] == 0)
    add('setpedout_real_ctor', obs.get('type_real') == 816)
    add('setpedout_regref', t == 816 and 3 in k)
    add('die_float_special', t == 212 and 12 in k and any(((inp[f] & 0x7F800000) == 0x7F800000) or (inp[f] & 0x7FFFFFFF) < 0x800000 for f in ('blend', 'speed')))
    add('die_float_snan', t == 212 and 12 in k and any((inp[f] & 0x7F800000) == 0x7F800000 and (inp[f] & 0x7FFFFF) != 0 and not (inp[f] & 0x400000) for f in ('blend', 'speed')))
    add('door_extreme', main and (inp['door'] >= 0x80000000 or inp['door'] == 0))


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
        sys.exit(runtime_runner.run_case('leavecarsub70', manifest['binaries']['asi'], analyze, 'LeaveCarSub70', timeout=3600))
