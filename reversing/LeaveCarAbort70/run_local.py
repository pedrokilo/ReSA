"""Compuertas del banco LeaveCarAbort70 (lote 70): CTaskComplexLeaveCar::MakeAbortable (0x641100, virtual, vtable 0x86E828 ranura 6), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados,
fp_difference vacio, que AL del EXE y la secuencia de eventos de los falsos coincidan con el modelo `model` de este archivo, escrito desde el desensamblado 0x641100..0x64152A
(sin mirar el C++), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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
NAMES = ['CTaskComplexLeaveCar::MakeAbortable']
CASES = 16384
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x641100}
# (direccion del EXE, hooked): BlendAnimation(id), ProcessVerticalLine, ClearGettingOutFlags, CVector::Magnitude, CPed::IsPlayer, ComputeTargetDoorToExit, CloseDoorFromOutside ctor y dtor (con hook),
# CloseDoorFromOutside::MakeAbortable y SetPedOut::ProcessPed (hooks V que tambien saltan en la entrada del EXE); sin hook en el EXE: CVector::Magnitude (su hook RH no esta activo), SetPedOut ctor / dtor
STUBBED = {NAMES[0]: [(0x4D4610, True), (0x5674E0, True), (0x6D3060, True), (0x4082C0, False), (0x5DF8F0, True), (0x64F110, True), (0x6464F0, True), (0x646570, True),
                      (0x64B020, True), (0x6478B0, False), (0x647950, False), (0x647D10, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x641100, 0x86E828 + 6 * 4}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
(EV_SUBTYPE, EV_SUBABORT, EV_EVTYPE, EV_CLEAR, EV_WHEELS, EV_MAG, EV_PLAYER, EV_VERT, EV_BLEND, EV_CDOOR, EV_CDCTOR, EV_CDDTOR, EV_CDABORT, EV_SPCTOR, EV_SPDTOR, EV_SPPROC) = range(1, 17)
EV_TRAP = 99
SPEED_LIMIT = 0x3E4CCCCD   # flt_858CC4 = 0.2f
FALL_LIMIT = 0xBE99999A    # flt_858EE4 = -0.3f


def fb(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def model(inp):
    """(AL, tipos de evento) segun el desensamblado 0x641100. GetTaskType y GetEventType se llaman una vez por comprobacion (no se guardan) y
    los falsos devuelven st[i] / et[i] segun el numero de llamada."""
    ev = []
    cnt = {'s': 0, 'e': 0}

    def stype():
        ev.append(EV_SUBTYPE)
        v = inp['st%d' % min(cnt['s'], 5)]
        cnt['s'] += 1
        return v

    def etype():
        ev.append(EV_EVTYPE)
        v = inp['et%d' % min(cnt['e'], 5)]
        cnt['e'] += 1
        return v

    def abort():
        ev.append(EV_SUBABORT)
        return inp['abort_ret'] & 0xFF

    def killed():   # CEventDamage::HasKilledPed 0x4ABCA0: byte +0x40 y byte +0xC distintos de cero
        return (inp['killed'] & 0xFF) != 0 and (inp['grp'] & 0xFF) != 0

    def abort_release():   # 0x641253..0x641299: si acepta, suelta las banderas de puerta y el contador; devuelve el AL de la subtarea
        r = abort()
        if r:
            ev.append(EV_CLEAR)
        return r

    if inp['veh_null']:
        return 1, ev
    prio = inp['prio']
    if prio == 2:
        ev.append(EV_SUBABORT)
        if (inp['ped_flags'] >> 8) & 1:
            if inp['door'] == 0:
                ev.append(EV_CDOOR)
            ev += [EV_CDCTOR, EV_CDABORT, EV_SPCTOR, EV_SPPROC, EV_SPDTOR, EV_CDDTOR]
        ev.append(EV_CLEAR)
        return 1, ev
    if prio != 1:
        return 0, ev
    has_ev = not inp['ev_null']
    if stype() == 0xD4:
        if stype() == 0x2CF:
            return 0, ev
        return abort_release(), ev
    if stype() == 0x329 and has_ev and etype() == 9 and killed():
        return abort_release(), ev
    if stype() == 0xCE and has_ev and etype() == 9 and killed():
        return abort_release(), ev
    if stype() == 0x32E:
        if not has_ev:
            return 0, ev
        in_jump = etype() == 0x21
        if not in_jump:
            in_jump = etype() == 0x42
        if in_jump:
            etype()   # 6413D6: m_bIsInAir = (tipo == 0x21)
            if not abort():
                return 0, ev
            parachute = False
            if inp['wtype'] == 0x2E and etype() == 0x21:
                ev.append(EV_WHEELS)
                parachute = inp['wheels'] == 0
            if parachute:
                fast = False
                if inp['vsub'] == 4:
                    ev.append(EV_MAG)
                    fast = fb(inp['mag']) > fb(SPEED_LIMIT)
                if not fast:
                    ev.append(EV_PLAYER)
                    if (inp['is_player'] & 0xFF) == 0:
                        return 0, ev
                    ev.append(EV_VERT)
                    if (inp['pvl'] & 0xFF) != 0:
                        return 0, ev
                ev.append(EV_BLEND)
                return 0, ev
            if etype() != 0x42:     # 6414A7
                return 0, ev
            if fb(inp['vz']) < fb(FALL_LIMIT):
                ev.append(EV_BLEND)
            return 0, ev
    # 6414F6: resto
    if not has_ev:
        return 0, ev
    if stype() == 0x2CF:
        ev.append(EV_SUBABORT)
        return inp['abort_ret'] & 0xFF, ev
    return 0, ev


def kinds_of(events):
    return [e[0] for e in events]


def tally(inp, events, al, cov):
    """cobertura de ramas desde la entrada y la traza de eventos (listas cuyo primer elemento es el tipo)"""
    k = kinds_of(events)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    prio = inp['prio']
    live = not inp['veh_null']
    add('no_vehicle', inp['veh_null'])
    add('prio_other', live and prio not in (1, 2))
    add('prio_other_no_subtask', live and prio not in (1, 2) and inp['sub_null'])
    add('imm_not_in_vehicle', live and prio == 2 and not (inp['ped_flags'] >> 8) & 1)
    add('imm_in_vehicle', EV_CDCTOR in k)
    add('imm_door_given', EV_CDCTOR in k and EV_CDOOR not in k)
    add('imm_door_computed', EV_CDOOR in k)
    add('imm_sensible_odd', EV_SPCTOR in k and (inp['sens'] & 0xFF) not in (0, 1))
    add('imm_release', live and prio == 2 and EV_CLEAR in k)
    add('flags_odd', EV_CLEAR in k and (inp['dfs'] & 0xFF) not in (0, 1))
    add('count_wrap', EV_CLEAR in k and (inp['ngs'] & 0xFF) > (inp['vng'] & 0xFF))

    def t(i):
        return inp['st%d' % i]
    urgent = live and prio == 1
    add('urgent', urgent)
    add('die_drive_false', urgent and t(0) == 0xD4 and t(1) == 0x2CF)
    add('die_abort_ok', urgent and t(0) == 0xD4 and t(1) != 0x2CF and (inp['abort_ret'] & 0xFF) != 0)
    add('die_abort_fail', urgent and t(0) == 0xD4 and t(1) != 0x2CF and (inp['abort_ret'] & 0xFF) == 0)
    add('die_abort_odd_bool', urgent and t(0) == 0xD4 and t(1) != 0x2CF and (inp['abort_ret'] & 0xFF) not in (0, 1))
    add('wait_type', urgent and t(0) != 0xD4 and t(1) == 0x329)
    add('wait_abort', urgent and t(0) != 0xD4 and t(1) == 0x329 and EV_SUBABORT in k and k.index(EV_SUBABORT) == k.index(EV_EVTYPE) + 1 and k.count(EV_SUBTYPE) == 2)
    add('wait_killed_odd', urgent and t(0) != 0xD4 and t(1) == 0x329 and (inp['killed'] & 0xFF) not in (0, 1) and EV_SUBABORT in k)
    add('wait_not_killed', urgent and t(0) != 0xD4 and t(1) == 0x329 and EV_EVTYPE in k and k.count(EV_SUBTYPE) > 2)
    add('getup_type', urgent and t(0) != 0xD4 and t(1) != 0x329 and t(2) == 0xCE)
    add('getup_abort', urgent and t(0) != 0xD4 and t(1) != 0x329 and t(2) == 0xCE and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 3)
    add('getup_after_wait_fail', urgent and t(0) != 0xD4 and t(1) == 0x329 and t(2) == 0xCE and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 3)
    add('getup_not_killed', urgent and t(0) != 0xD4 and t(1) != 0x329 and t(2) == 0xCE and EV_EVTYPE in k and k.count(EV_SUBTYPE) > 3)
    jump = urgent and t(0) != 0xD4 and t(1) != 0x329 and t(2) != 0xCE and t(3) == 0x32E
    add('jump_type', jump)
    add('jump_no_event', jump and inp['ev_null'])
    add('jump_event_other', jump and not inp['ev_null'] and k.count(EV_SUBTYPE) == 5 and EV_SUBABORT not in k)
    add('jump_final_abort', jump and not inp['ev_null'] and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 5)
    add('jump_abort_fail', jump and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 4 and (inp['abort_ret'] & 0xFF) == 0)
    add('jump_air', jump and not inp['ev_null'] and inp['et0'] == 0x21 and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 4)
    add('jump_water_first', jump and not inp['ev_null'] and inp['et0'] != 0x21 and inp['et1'] == 0x42 and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 4)
    add('jump_parachute_wheels', EV_WHEELS in k)
    add('jump_parachute_plane', EV_MAG in k)
    add('jump_parachute_fast', EV_MAG in k and EV_PLAYER not in k and EV_BLEND in k)
    add('jump_parachute_player', EV_PLAYER in k)
    add('jump_parachute_not_player', EV_PLAYER in k and EV_VERT not in k)
    add('jump_parachute_vert', EV_VERT in k)
    add('jump_parachute_blend', EV_VERT in k and EV_BLEND in k)
    add('jump_parachute_ground', EV_VERT in k and EV_BLEND not in k)
    add('jump_wheels_nonzero', EV_WHEELS in k and inp['wheels'] != 0)
    add('jump_slot_negative', EV_WHEELS in k and (inp['slot'] & 0x80) != 0)
    add('jump_slot_oob', jump and EV_SUBABORT in k and (inp['slot'] & 0xFF) in (12, 13, 0x7F, 0x80, 0xFF, 0xF5, 0xF4, 0xFE, 0x8A) and inp['wtype'] == 0x2E)
    add('jump_water_blend', EV_BLEND in k and EV_PLAYER not in k and EV_MAG not in k)
    add('jump_water_no_blend', jump and EV_SUBABORT in k and k.count(EV_SUBTYPE) == 4 and EV_BLEND not in k and EV_VERT not in k and EV_PLAYER not in k)
    add('mag_nan', EV_MAG in k and ((inp['mag'] >> 23) & 0xFF) == 0xFF and (inp['mag'] & 0x7FFFFF) != 0)
    add('vz_nan', jump and EV_SUBABORT in k and ((inp['vz'] >> 23) & 0xFF) == 0xFF and (inp['vz'] & 0x7FFFFF) != 0)
    add('final_no_event', urgent and t(0) != 0xD4 and inp['ev_null'] and k.count(EV_SUBTYPE) == 4)
    add('final_drive_abort', urgent and k.count(EV_SUBTYPE) == 5 and EV_SUBABORT in k)
    add('final_drive_other', urgent and k.count(EV_SUBTYPE) == 5 and EV_SUBABORT not in k)
    add('urgent_die_released', urgent and t(0) == 0xD4 and EV_CLEAR in k)
    add('result_true', al == 1)
    add('result_false', al == 0)
    add('result_odd', al not in (0, 1))


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
    if col.get('probe_selftest') is not True:
        raise RuntimeError('La autoprueba de los cuerpos sin hook (constructor y destructor de CTaskSimpleCarSetPedOut) fallo')
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
        inp = row['input']
        want_al, want_ev = model(inp)   # modelo del desensamblado contra el lado ORIGINAL (EXE); original == candidato ya esta comprobado arriba
        obs0 = row['original']['observed']
        if obs0['al'] != want_al:
            raise RuntimeError('El modelo difiere del EXE en %s %d: AL %s, modelo %s' % (fn, case, obs0['al'], want_al))
        evs = kinds_of(obs0['events'])
        if evs != want_ev:
            raise RuntimeError('La secuencia de eventos difiere del modelo en %s %d: %s contra %s' % (fn, case, evs, want_ev))
        door = inp['door'] if inp['door'] else inp['cdoor']
        for e in obs0['events']:   # argumentos que el modelo conoce: banderas liberadas, puerta y bool de la tarea de sacar al ped
            if e[0] == EV_CLEAR and e[2] != (inp['dfs'] & 0xFF):
                raise RuntimeError('ClearGettingOutFlags con banderas distintas en %s %d' % (fn, case))
            if e[0] == EV_CDCTOR and (e[3] != door or e[4] != 0):
                raise RuntimeError('Argumentos de CTaskSimpleCarCloseDoorFromOutside inesperados en %s %d' % (fn, case))
            if e[0] == EV_SPCTOR and (e[3] != door or e[4] != (inp['sens'] & 0xFF) or e[5] != 0):
                raise RuntimeError('Argumentos de CTaskSimpleCarSetPedOut inesperados en %s %d' % (fn, case))
        f = inp['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(inp, obs0['events'], obs0['al'], cov[fn])
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
        sys.exit(runtime_runner.run_case('leavecarabort70', manifest['binaries']['asi'], analyze, 'LeaveCarAbort70', timeout=3600))
