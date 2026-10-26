"""Compuertas del banco PickUpNext68 (lote 68): CTaskComplexGoPickUpEntity::CreateNextSubTask (virtual, slot 8 de la vtable 0x870B98, EXE 0x691AE0), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook V registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada y ranura de la vtable)/DF/SEH/restauracion
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

NAMES = ['CTaskComplexGoPickUpEntity::CreateNextSubTask']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x691AE0}
VT_SLOT = 0x870B98 + 8 * 4
# (direccion del EXE, hooked): CTask::operator new, CEntity::GetColModel, CTaskManager::GetTaskSecondary, SetTaskSecondary, CTaskSimpleHoldEntity ctor 1, CTaskSimplePickUpEntity ctor 2
STUBBED = {NAMES[0]: [(0x61A5A0, True), (0x535300, True), (0x681810, True), (0x681B60, True), (0x6913A0, True), (0x6917B0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x691AE0, VT_SLOT}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
MAIN_TYPES = (0x134, 0x386, 0x387)


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
    t = inp['sub_type']
    live = inp['ent_null'] == 0

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('ent_null', inp['ent_null'] == 1)
    add('ent_null_no_events', inp['ent_null'] == 1 and not events)
    add('other_type', live and t not in MAIN_TYPES)
    add('other_type_returns_null', live and t not in MAIN_TYPES and obs['eax'] == 0)
    add('pick_type', live and t == 0x134)
    add('pick_secondary_exists', live and t == 0x134 and 3 in k and 1 not in k)
    add('pick_secondary_tagged', live and t == 0x134 and 3 in k and inp['sec_null'] == 0 and inp['sec_kind'] == 0)
    add('pick_secondary_raw', live and t == 0x134 and 3 in k and inp['sec_null'] == 0 and inp['sec_kind'] == 1)
    add('hold_created', 5 in k)
    add('hold_alloc_null', 4 in k and 1 in k and 5 not in k)
    add('goto_type', live and t == 0x387)
    add('goto_created', live and t == 0x387 and 1 in k and obs['eax'] != 0)
    add('goto_alloc_null', live and t == 0x387 and 1 in k and obs['eax'] == 0)
    add('goto_matrix_both', live and t == 0x387 and inp['ped_mat'] and inp['ent_mat'])
    add('goto_compact_both', live and t == 0x387 and not inp['ped_mat'] and not inp['ent_mat'])
    add('goto_nan_delta', live and t == 0x387 and 1 in k and any(is_nan(inp[a]) for a in ('ped_x', 'ped_y', 'ent_x', 'ent_y')))
    add('goto_inf_delta', live and t == 0x387 and 1 in k and any(is_inf(inp[a]) for a in ('ped_x', 'ped_y', 'ent_x', 'ent_y')))
    add('goto_zero_delta', live and t == 0x387 and 1 in k and (inp['ped_x'] & 0x7FFFFFFF) == (inp['ent_x'] & 0x7FFFFFFF) and (inp['ped_y'] & 0x7FFFFFFF) == (inp['ent_y'] & 0x7FFFFFFF))
    try:
        ddx = struct.unpack('<f', struct.pack('<f', f32(inp['ent_x']) - f32(inp['ped_x'])))[0]
        ddy = struct.unpack('<f', struct.pack('<f', f32(inp['ent_y']) - f32(inp['ped_y'])))[0]
    except (OverflowError, struct.error):
        ddx = ddy = 1.0
    add('goto_denormal_delta', live and t == 0x387 and 1 in k and (is_denormal_value(ddx) or is_denormal_value(ddy)))
    add('goto_denormal_dominant', live and t == 0x387 and 1 in k and obs['eax'] != 0 and ((is_denormal_value(ddx) and abs(ddy) > 1e-30) or (is_denormal_value(ddy) and abs(ddx) > 1e-30)
        or (is_denormal_value(ddx) and is_denormal_value(ddy))))
    add('heading_denormal_top', live and t == 0x386 and inp['group'] == 0x51 and is_denormal_value(f32(inp['col_z']) + f32(inp['ent_z'])))
    add('heading_type', live and t == 0x386)
    add('heading_other_group', live and t == 0x386 and inp['group'] != 0x51 and 6 in k and 2 not in k)
    add('heading_group51_colmodel', live and t == 0x386 and inp['group'] == 0x51 and 2 in k)
    pick = [e for e in events if e[0] == 6]
    if pick and live and t == 0x386 and inp['group'] == 0x51:
        g = pick[0][7]
        add('heading_g53', g == 0x53 and pick[0][8] == 0x3E4CCCCD)
        add('heading_g52', g == 0x52 and pick[0][8] == 0x3E888889)
        add('heading_g51_kept', g == 0x51 and pick[0][8] == 0x3F19999A)
    add('heading_pickup_created', 6 in k)
    add('heading_nan_top', live and t == 0x386 and inp['group'] == 0x51 and (is_nan(inp['col_z']) or is_nan(inp['ent_z'])))
    add('heading_nan_ped_z', live and t == 0x386 and inp['group'] == 0x51 and is_nan(inp['ped_z']))
    add('heading_inf', live and t == 0x386 and inp['group'] == 0x51 and any(is_inf(inp[a]) for a in ('col_z', 'ent_z', 'ped_z')))
    add('heading_overflow_sum', live and t == 0x386 and inp['group'] == 0x51 and (inp['col_z'] >> 31) == (inp['ent_z'] >> 31)
        and 0x7E800000 < (inp['col_z'] & 0x7FFFFFFF) < 0x7F800000 and 0x7E800000 < (inp['ent_z'] & 0x7FFFFFFF) < 0x7F800000)
    add('ret_nonnull', obs['eax'] != 0)


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
        sys.exit(runtime_runner.run_case('pickupnext68', manifest['binaries']['asi'], analyze, 'PickUpNext68', timeout=3600))
