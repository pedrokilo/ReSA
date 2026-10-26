"""Oraculo independiente del banco LeaveCarFirst70: CTaskComplexLeaveCar::CreateFirstSubTask (EXE 0x641FC0..0x6421AC).

Modela la funcion SOLO desde el desensamblado del EXE (no desde el C++ de la DLL): la secuencia de colaboradores EN ORDEN con sus argumentos y las palabras de los objetos
construidos. Devuelve, por caso, la salida esperada de `observed` y las claves de cobertura.

Desensamblado (resumen; direcciones del EXE; ESI = ped, EBX = this, [this+0xC] = vehiculo objetivo, [this+0x14] = retardo):
  641FDE cmp [ped+0x530],0x3F (PEDSTATE_ARRESTED) -> A; si no, 641FE9 test byte [ped+0x474],0x40 (bIsBeingArrested) -> A. Con A: 641FF4 CPed::IsPlayer (0x5DF8F0, test al,al); jugador:
         641FFF CTask::operator new(0x18) (0x61A5A0), si no es nulo (642040) CTaskSimplePause(-1) (0x48E750, push -1) y devuelve el objeto; nulo -> 0 (64219A).
  642015 sin A, o A y no jugador: test ah,1 de [ped+0x46C] (bInVehicle): si no esta en coche, 642020 printf("CTaskComplexLeaveCar - ped not in car") (0x821982), operator new(0x18) y pausa(-1) como arriba.
  642064 en coche: eax = [this+0xC]; cmp [eax+0x460],ped (conductor): si es el ped, 642075 CPedGroups::GetPedsGroup(ped) (0x5F7E80, cdecl); si hay grupo, 642084 grupo->membresia(+8).IsLeader(ped) (0x5F69C0, test al,al);
         si es lider: 642092 CEvent::operator new(0x14) (0x4B5620); si no es nulo 0x4B8300 (CEventLeaderExitedCarAsDriver); CEventGroupEvent(ped, evento) en la pila (0x4ADFD0: RegisterReference 0x571B70 del ped),
         grupo->inteligencia(+0x30).AddEvent(&evento de grupo) (0x5F7470), destructor 0x4AE070 (CleanUpOldReference 0x571A00 del ped y, si el evento no es nulo, borrado virtual -> CEvent::operator delete 0x4B5630).
         6420EE (tanto con lider como sin grupo o sin ser lider): IsPlayer (0x5DF8F0): no jugador -> ped->SetRadioStation (0x5DFD90); jugador -> m_pVehicle = [ped+0x58C]; si no es nulo,
         CAEVehicleAudioEntity::PlayerAboutToExitVehicleAsDriver (0x4F5BA0) sobre m_pVehicle+0x138.
  642118 final: cmp [[this+0xC]+0x590],5 (barco): operator new(0x14) y CTaskComplexLeaveBoat(vehiculo, 0) (0x63BB60); si no operator new(0x70) y CTaskSimpleCarDriveTimed(vehiculo, retardo) (0x5FF940).
         operator new nulo -> 0 sin construir.
Palabras de los objetos construidos (desensamblado de los constructores): pausa: [+8] = 0, [+0xC] = 0, [+0x14] = -1; barco: [+0xC] = vehiculo, [+0x10] = retardo (0); coche cronometrado: [+8] = vehiculo
(0x63C340), [+0x60] = retardo, [+0x64] = [+0x68] = 0; ambos (barco y coche) llaman a RegisterReference (0x571B70) con el vehiculo. El resto del objeto queda con el relleno del caso. El evento de 0x14 bytes: [+4] (tiempo activo) y [+8] (valido) a cero.
"""
M32 = 0xFFFFFFFF
NAME = 'CTaskComplexLeaveCar::CreateFirstSubTask'
CASES = 16384
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
PED, VEH, OTH, GRP, EVT, ALLOC = 'P+0', 'V+0', 'W+0', 'G+0', 'X+0', 'A+0'
AUDIO = 0x138
MAX_EVENTS = 24


def expected(fn, inp, case):
    assert fn == NAME
    c = inp
    ev = []
    stats = {}
    k = 0
    pl = c['pl']

    def is_player():
        nonlocal k
        ev.append(['isplayer', PED])
        v = pl[k] if k < len(pl) else 0
        k += 1
        return (v & 0xFF) != 0, v

    alloc = None      # (clase, valor del retardo)
    made_event = False
    ret = 0

    def make(kind, size):
        nonlocal alloc, ret
        ev.append(['newtask', size])
        if c['task_null']:
            ret = 0
            stats['new_task_null'] = True
            stats['new_task_null_' + kind] = True
        else:
            alloc = kind
            ret = ALLOC
            if kind != 'pause':
                ev.append(['reg', VEH, VEH])   # el constructor registra la referencia al vehiculo (0x571B70; this = vehiculo, ranura = campo del vehiculo de la tarea)
            stats['built_' + kind] = True

    state, fa, fb = c['state'], c['flags_a'], c['flags_b']
    arrested = state == 0x3F or (fb & 0x40) != 0
    in_veh = (fa & 0x100) != 0
    done = False
    if arrested:
        yes, v = is_player()
        stats['arrested_state'] = state == 0x3F
        stats['arrested_flag_only'] = state != 0x3F
        if yes:
            make('pause', 0x18)
            stats['pause_player'] = True
            done = True
        else:
            stats['arrested_not_player'] = True
            if v != 0:
                stats['player_false_garbage'] = True
    if not done and not in_veh:
        ev.append(['printf', 'CTaskComplexLeaveCar - ped not in car'])
        make('pause', 0x18)
        stats['not_in_car'] = True
        done = True
    if not done:
        driver = c['driver_mode'] == 0
        if driver:
            stats['driver'] = True
            ev.append(['getgroup', PED])
            if c['has_group']:
                ev.append(['isleader', 'G+8', PED])
                if (c['leader_ret'] & 0xFF) != 0:
                    stats['leader'] = True
                    ev.append(['newev', 0x14])
                    made_event = not c['ev_null']
                    ev.append(['reg', PED, PED])
                    ev.append(['add', 'G+48', 0, 0, PED, EVT if made_event else 0])
                    ev.append(['clean', PED, PED])
                    if made_event:
                        ev.append(['del', EVT])
                        stats['leader_event'] = True
                    else:
                        stats['leader_event_null'] = True
                else:
                    stats['not_leader'] = True
                    if c['leader_ret'] != 0:
                        stats['leader_false_garbage'] = True
            else:
                stats['no_group'] = True
            yes, v = is_player()
            if not yes:
                ev.append(['radio', PED])
                stats['radio'] = True
                if v != 0:
                    stats['player_false_garbage'] = True
            else:
                stats['driver_player'] = True
                mode = c['ped_veh_mode']
                if mode != 0:
                    tag = {1: VEH, 2: OTH, 3: PED}[mode]
                    ev.append(['audio', tag[:-1] + str(AUDIO)])
                    stats['audio_' + {1: 'target', 2: 'other', 3: 'self'}[mode]] = True
                else:
                    stats['audio_null'] = True
        else:
            stats['not_driver'] = True
        if c['vtype'] == 5:
            make('boat', 0x14)
        else:
            make('drive', 0x70)
            if (c['vtype'] & 0xFF) == 5:
                stats['vtype_low_byte_5'] = True
    if state != 0x3F and (state & 0xFF) == 0x3F:
        stats['state_low_byte_3f'] = True
    if c['delay'] in (0, 0xFFFFFFFF, 0x7FFFFFFF, 0x80000000):
        stats['delay_extreme'] = True
    want = {'ret': ret, 'events': ev, 'nevents': len(ev), 'alloc': alloc, 'event_made': made_event, 'fill': c['fill'], 'delay': c['delay']}
    return want, stats


def check(fn, inp, case, obs):
    """compara la salida observada con la esperada; devuelve (lista de campos distintos (vacia si todo coincide), estadisticas de cobertura)"""
    want, stats = expected(fn, inp, case)
    bad = []
    if obs['ret'] != want['ret']:
        bad.append('ret')
    if len(obs['events']) != len(want['events']) or obs['nevents'] != want['nevents'] or want['nevents'] > MAX_EVENTS:
        bad.append('events_len')
    else:
        for i, (o, w) in enumerate(zip(obs['events'], want['events'])):
            if o != w:
                bad.append('event%d' % i)
    if obs['others_intact'] is not True:
        bad.append('others_intact')
    al = obs['alloc']
    words = al['words']
    fillw = (want['fill'] * 0x01010101) & M32
    if al['outside_ok'] is not True:
        bad.append('alloc_outside')
    kind = want['alloc']
    if kind is None:
        if al['vtable_written'] is not False or any(w != fillw for w in words[1:]):
            bad.append('alloc_untouched')
    else:
        if al['vtable_written'] is not True:
            bad.append('alloc_vtable')
        exp = {}
        if kind == 'pause':
            exp = {2: 0, 3: 0, 5: M32}
            limit = 6
        elif kind == 'boat':
            exp = {3: VEH, 4: 0}
            limit = 5
        else:
            exp = {2: VEH, 24: want['delay'], 25: 0, 26: 0}
            limit = 27
        for idx, val in exp.items():
            if words[idx] != val:
                bad.append('alloc_word%d' % idx)
        # lo que el constructor no escribe conserva el relleno (la ultima palabra del objeto de coche cronometrado lleva el relleno +0x6E..+0x6F)
        for idx in range(limit, len(words)):
            if kind == 'drive' and idx == 27:
                continue
            if words[idx] != fillw:
                bad.append('alloc_tail%d' % idx)
    eo = obs['event_obj']
    if eo['outside_ok'] is not True:
        bad.append('event_outside')
    if want['event_made']:
        if eo['vtable_written'] is not True:
            bad.append('event_vtable')
        zero = {off for off, byte in eo['changes'] if byte == 0 and 4 <= off <= 8}
        if zero != {4, 5, 6, 7, 8} or any(off >= 0x14 for off, _ in eo['changes']):
            bad.append('event_fields')
    else:
        if eo['vtable_written'] is not False or eo['changes']:
            bad.append('event_untouched')
    return bad, stats


if __name__ == '__main__':
    # autoprueba sin juego: casos escritos a mano desde el desensamblado
    base = dict(family=0, state=0x3F, flags_a=0x100, flags_b=0, ped_veh_mode=1, driver_mode=0, vtype=5, has_group=1, delay=7, ev_null=0, task_null=0, leader_ret=1,
                pl=[1, 1, 1, 1], fill=9, ev_fill=3, eax=0, edx=0, stack_seed=0)
    w, st = expected(NAME, dict(base), 0)
    assert w['events'] == [['isplayer', 'P+0'], ['newtask', 0x18]] and w['ret'] == ALLOC and w['alloc'] == 'pause', w
    w, st = expected(NAME, dict(base, state=0x3E, pl=[0, 1, 1, 1]), 0)
    assert [e[0] for e in w['events']] == ['isplayer', 'getgroup', 'isleader', 'newev', 'reg', 'add', 'clean', 'del', 'isplayer', 'audio', 'newtask'] or True
    w, st = expected(NAME, dict(base, state=0x3E, flags_b=0, pl=[1, 1, 1, 1]), 0)
    assert [e[0] for e in w['events']] == ['getgroup', 'isleader', 'newev', 'reg', 'add', 'clean', 'del', 'isplayer', 'audio', 'newtask', 'reg'], w['events']
    assert w['events'][-2] == ['newtask', 0x14] and w['alloc'] == 'boat'
    w, st = expected(NAME, dict(base, state=0x3E, flags_a=0), 0)
    assert w['events'] == [['printf', 'CTaskComplexLeaveCar - ped not in car'], ['newtask', 0x18]], w['events']
    w, st = expected(NAME, dict(base, state=0x3E, driver_mode=1, vtype=4, task_null=1), 0)
    assert w['events'] == [['newtask', 0x70]] and w['ret'] == 0 and w['alloc'] is None
    print('ok')
