"""Compuertas del banco PickUpControl68 (lote 68): CTaskComplexGoPickUpEntity::ControlSubTask (0x691D50, virtual, vtable 0x870B98 ranura 10), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que la salida del EXE (EAX = subtarea o nulo) sea la que predice el modelo `predict` de este archivo desde el desensamblado, la cobertura por familia de las
fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza
(devuelve None si el registro aun no existe o no esta completo).
"""
import json
import math
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
NAMES = ['CTaskComplexGoPickUpEntity::ControlSubTask']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x691D50}
# (direccion del EXE, hooked): GetAnimBlockName, GetAnimationBlock(const char*), RequestModel, AddAnimBlockRef, GetPadFromPlayer, GetPedWalkUpDown, GetPedWalkLeftRight,
# GetRadianAngleBetweenPoints, JumpJustDown, SprintJustDown (todos con hook RH)
STUBBED = {NAMES[0]: [(0x4D3A30, True), (0x4D3940, True), (0x4087E0, True), (0x4D3FB0, True), (0x609560, True), (0x53FD30, True), (0x53FC90, True), (0x53CBE0, True), (0x540770, True), (0x5407F0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima de ramas (calibrada con la primera corrida, con margen): coverage.json = {funcion: {clave: minimo de casos}}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
SUB_TAG = 0x10000000 + (1 << 20) + 0x40   # Tag() de la subtarea: arena 1 (taskA = 0), desplazamiento de la guarda 0x40
EV_NAME, EV_BLOCK, EV_REQUEST, EV_ADDREF, EV_TYPE, EV_ABORT, EV_PAD, EV_UD, EV_LR, EV_RAD, EV_JUMP, EV_SPRINT, EV_TRAP = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 99


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def fb(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def angle_model(inp):
    """modelo del cuerpo 0x691E0F..0x691F6C: devuelve (abort, rama de ajuste, palanca larga)"""
    ud, lr = s16(inp['ud']), s16(inp['lr'])
    mag = f32(f32(math.sqrt(f32(f32(ud * ud) + f32(lr * lr)))) * 0.0078125)
    px, py = (fb(inp['mx']), fb(inp['my'])) if inp['has_mat'] else (fb(inp['px']), fb(inp['py']))
    dx = f32(fb(inp['tx']) - px)
    dy = f32(fb(inp['ty']) - py)
    try:
        t = f32(math.atan2(-dx, dy))
    except ValueError:
        t = math.nan
    a = f32(fb(inp['rad']) - fb(inp['cam']))
    pi, two_pi, quarter = fb(0x40490FDB), fb(0x40C90FDB), fb(0x3F490FDB)
    branch = 'none'
    if f32(t + pi) < a:
        a = f32(a - two_pi)
        branch = 'minus'
    elif f32(t - pi) > a:
        a = f32(a + two_pi)
        branch = 'plus'
    jump, sprint = (inp['jump'] & 0xFF) != 0, (inp['sprint'] & 0xFF) != 0
    long_stick = mag > 0.75
    if jump or sprint:
        return True, branch, long_stick
    if not long_stick:
        return False, branch, long_stick
    return f32(a - t) > quarter, branch, long_stick


def predict(inp):
    """EAX esperado (etiqueta) segun el modelo del desensamblado"""
    if not inp['sub_kind']:
        return 0
    now, start = inp['now'], inp['start']
    typ = inp['sub_type']
    if typ == 0x134:
        if inp['has_assoc'] or (inp['dropped'] & 0xFF):
            return SUB_TAG
        return 0 if now > (start + 8000) & 0xFFFFFFFF else SUB_TAG
    if typ != 0x387:
        return SUB_TAG
    if now > (start + 5000) & 0xFFFFFFFF:
        return 0
    if not inp['pdata'] or not inp['pad_ptr']:
        return SUB_TAG
    return 0 if angle_model(inp)[0] else SUB_TAG


def events_model(inp):
    """secuencia de tipos de evento de los falsos que ejecuta el EXE (orden del desensamblado)"""
    ev = []
    if inp['group'] and not (inp['refd'] & 0xFF):
        if not inp['via_group']:
            ev += [EV_NAME, EV_BLOCK]
        loaded = {0: (inp['loaded'] & 0xFF) != 0, 1: True, 2: False}[inp['blk']]
        ev.append(EV_ADDREF if loaded else EV_REQUEST)
    if not inp['sub_kind']:
        return ev
    ev.append(EV_TYPE)
    now, start, typ = inp['now'], inp['start'], inp['sub_type']
    if typ == 0x134:
        if not inp['has_assoc'] and not (inp['dropped'] & 0xFF) and now > (start + 8000) & 0xFFFFFFFF:
            ev.append(EV_ABORT)
        return ev
    if typ != 0x387:
        return ev
    if now > (start + 5000) & 0xFFFFFFFF:
        return ev + [EV_ABORT]
    if not inp['pdata']:
        return ev
    ev.append(EV_PAD)
    if not inp['pad_ptr']:
        return ev
    ev += [EV_UD, EV_LR, EV_RAD, EV_JUMP]
    if (inp['jump'] & 0xFF):
        return ev + [EV_ABORT]
    ev.append(EV_SPRINT)
    if (inp['sprint'] & 0xFF):
        return ev + [EV_ABORT]
    ab = angle_model(inp)[0]
    return ev + [EV_ABORT] if ab else ev


def kinds_of(events):
    return [e[0] for e in events]


def tally(inp, events, ret, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = kinds_of(events)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    typ = inp['sub_type']
    add('no_subtask', not inp['sub_kind'])
    add('group_zero', inp['group'] == 0)
    add('block_request', EV_REQUEST in k)
    add('block_request_group_block', EV_REQUEST in k and inp['via_group'] == 1)
    add('block_request_by_name', EV_REQUEST in k and inp['via_group'] == 0)
    add('block_request_exe_block', EV_REQUEST in k and inp['blk'] == 2)
    add('block_loaded_byte_odd', EV_ADDREF in k and inp['blk'] == 0 and (inp['loaded'] & 0xFF) not in (0, 1))
    add('block_addref', EV_ADDREF in k)
    add('block_addref_negative_index', EV_ADDREF in k and inp['blk'] == 1)
    add('block_already_referenced', inp['group'] != 0 and (inp['refd'] & 0xFF) != 0)
    add('block_referenced_byte_odd', inp['group'] != 0 and (inp['refd'] & 0xFF) not in (0, 1))
    add('block_by_name', EV_BLOCK in k)
    add('type_other', inp['sub_kind'] and typ not in (0x134, 0x387))
    add('type_other_high_bits', inp['sub_kind'] and typ in (0x134 | 0x10000, 0x387 | 0x80000000))
    add('pick_assoc', inp['sub_kind'] and typ == 0x134 and inp['has_assoc'])
    add('pick_dropped', inp['sub_kind'] and typ == 0x134 and not inp['has_assoc'] and (inp['dropped'] & 0xFF) != 0)
    add('pick_dropped_byte_odd', inp['sub_kind'] and typ == 0x134 and not inp['has_assoc'] and (inp['dropped'] & 0xFF) not in (0, 1))
    add('pick_in_time', inp['sub_kind'] and typ == 0x134 and not inp['has_assoc'] and (inp['dropped'] & 0xFF) == 0 and ret == SUB_TAG)
    add('pick_abort', EV_ABORT in k and typ == 0x134)
    add('pick_abort_wrap', EV_ABORT in k and typ == 0x134 and inp['start'] + 8000 > 0xFFFFFFFF)
    add('goto_abort_time', EV_ABORT in k and typ == 0x387 and EV_PAD not in k)
    add('goto_abort_time_wrap', EV_ABORT in k and typ == 0x387 and EV_PAD not in k and inp['start'] + 5000 > 0xFFFFFFFF)
    add('goto_no_playerdata', inp['sub_kind'] and typ == 0x387 and EV_PAD not in k and ret == SUB_TAG and not inp['pdata'])
    add('goto_no_pad', EV_PAD in k and EV_UD not in k)
    add('goto_stick_read', EV_UD in k)
    add('goto_jump_abort', EV_JUMP in k and EV_SPRINT not in k and EV_ABORT in k)
    add('goto_sprint_abort', EV_SPRINT in k and EV_ABORT in k)
    if EV_UD in k:
        ab, branch, long_stick = angle_model(inp)
        both_false = (inp['jump'] & 0xFF) == 0 and (inp['sprint'] & 0xFF) == 0
        add('goto_short_stick', both_false and not long_stick and ret == SUB_TAG)
        add('goto_long_stick', both_false and long_stick)
        add('goto_angle_abort', both_false and long_stick and EV_ABORT in k)
        add('goto_angle_return', both_false and long_stick and EV_ABORT not in k)
        add('goto_adjust_minus', branch == 'minus')
        add('goto_adjust_plus', branch == 'plus')
        add('goto_no_adjust', branch == 'none')
        add('goto_adjust_minus_abort', branch == 'minus' and both_false and long_stick and EV_ABORT in k)
        add('goto_adjust_plus_abort', branch == 'plus' and both_false and long_stick and EV_ABORT in k)
        add('goto_nan_angle', both_false and long_stick and any(x != x for x in (fb(inp['rad']), fb(inp['cam']))))
        add('goto_nan_heading', any(x != x for x in (fb(inp['tx']), fb(inp['ty']))))
        add('goto_with_matrix', inp['has_mat'] == 1)
        add('goto_without_matrix', inp['has_mat'] == 0)
        add('goto_stick_garbage_high_bits', (inp['ud'] >> 16) != 0 or (inp['lr'] >> 16) != 0)
    add('result_subtask', ret == SUB_TAG)
    add('result_null', ret == 0)


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
        if col[fn]['dll_real'] or got != STUBBED.get(fn, []):
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
        want = predict(row['input'])   # modelo del desensamblado contra el lado ORIGINAL (EXE); original == candidato ya esta comprobado arriba
        got = row['original']['observed']['ret']
        if got != want:
            raise RuntimeError('El modelo difiere del EXE en %s %d: EAX %s, modelo %s' % (fn, case, got, want))
        evs = kinds_of(row['original']['observed']['events'])
        if evs != events_model(row['input']):
            raise RuntimeError('La secuencia de eventos difiere del modelo en %s %d: %s contra %s' % (fn, case, evs, events_model(row['input'])))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs['ret'], cov[fn])
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
        sys.exit(runtime_runner.run_case('pickupcontrol68', manifest['binaries']['asi'], analyze, 'PickUpControl68', timeout=3600))
