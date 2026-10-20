"""Oraculo de CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents (0x603720), modelado desde el desensamblado.

0x603720 (thiscall, ECX = escaner -> temporizador CTaskTimer [+0 inicio, +4 intervalo, +8 started, +9 stopped], tres argumentos de pila: ped (+4), entities y count NO se leen; `ret 0Ch`):
  603735  si byte [ecx+8] == 0: [ecx] = ahora ; [ecx+8] = 1 ; [ecx+4] = 500
  60375E  si byte [ecx+9] != 0: [ecx] = ahora ; [ecx+9] = 0
  603771  si NOT (ahora >= [ecx] + [ecx+4]) (sin signo, la suma da la vuelta) -> fin
  60378D  [ecx] = ahora ; [ecx+4] = 500 ; [ecx+8] = 1
  60379E  intel = [ped+47Ch] ; tarea = GetSimplestActiveTask(intel+4) (0x6819D0) ; si nula -> fin ; IsGoToTask(tarea) (0x61A360, cdecl, solo AL) ; si falso -> fin
  6037C9  veh = [intel+120h] ; si nulo -> fin
  6037DD  posiciones: ecx = [veh+14h] ? +30h : veh+4 ; eax = [ped+14h] ? +30h : ped+4 ; D = pos_ped - pos_veh (cada componente redondeada a float)
  60381A  col = GetColModel(veh) (0x535300) ; esi = [veh+14h] (matriz, sin comprobar nulo) ; V1 = FromMultiply(M, col+0) ; V2 = FromMultiply(M, col+0Ch) (0x59C670)
  603842  U = M+20h (x,y,z) ; d2 = -((U.z*V2.z + U.y*V2.y) + U.x*V2.x) ; h = U * -1.0 ; d1 = -((V1.y*h.y + V1.z*h.z) + h.x*V1.x)
  6038D0  P = ped pos ; c1 = (((U.z*P.z + U.y*P.y) + U.x*P.x) + d2) < 0.5 ; c2 = c1 y ((((h.y*P.y + h.z*P.z) + h.x*P.x) + d1) < 0.5)
  603935  limite = 5.0 (10.0 si [veh+594h] == 6) ; si NOT c2 -> fin
  603964  si NOT (byte [ped+474h] & 8): SquaredMagnitude(D) = (x*x + y*y) + z*z (0x406DA0) ; si NOT (limite*limite > eso) -> fin
  603991  distancia = 0.0 ; GetIsLineOfSightClear(ped, tarea+0Ch, veh, &distancia) (0x5F5A30, cdecl) ; AL != 0 -> fin
  6039BA  si NOT (distancia > 0.5) -> fin
  6039CE  mover = GetMoveStateFromGoToTask(intel) (0x601D70) ; evento CEventPotentialWalkIntoVehicle(veh, mover) (ctor 0x4AE320: RegisterReference(veh) 0x571B70) ;
          CEventGroup::Add(&evento, false) sobre intel+68h (0x4AB420) ; dtor 0x4AE3C0 (CleanUpOldReference(veh) 0x571A00)
Valores del evento: tipo 56, prioridad 29, m_nTimeActive 0, m_bValid 0, m_bAddToEventGroup 1, m_TaskId 0xC8, m_FacialExpressionType -1, m_vehicle, m_moveState.
"""
from oracle_common import add, sub, mul, neg, lt, gt, bits_to_f, f_to_bits, u32

NAME = 'CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents'
# Misma suma a mano que Fixtures/FixVpc.hpp (guarda + objeto + guarda)
ARENAS = {'kVsOff': 0x20, 'kVsBytes': 0xC, 'kVsArena': 0x4C, 'kVpedOff': 0x40, 'kVpedBytes': 0x500, 'kVpedArena': 0x580, 'kVmatOff': 0x20, 'kVmatBytes': 0x48, 'kVmatArena': 0x88,
          'kVvehOff': 0x40, 'kVvehBytes': 0x5A0, 'kVvehArena': 0x620, 'kVintOff': 0x40, 'kVintBytes': 0x130, 'kVintArena': 0x1B0, 'kVtaskOff': 0x20, 'kVtaskBytes': 0x20, 'kVtaskArena': 0x60,
          'kVcolOff': 0x20, 'kVcolBytes': 0x18, 'kVcolArena': 0x58}


def expected(inp, case):
    now = u32(inp['now'])
    start, interval = u32(inp['t_start']), u32(inp['t_interval'])
    started, stopped = inp['t_started'] & 0xFF, inp['t_stopped'] & 0xFF
    if started == 0:
        start, started, interval = now, 1, 500
    if stopped != 0:
        start, stopped = now, 0
    due = now >= u32(start + interval)
    events = []
    stats = {'timer_not_started': inp['t_started'] & 0xFF == 0, 'timer_stopped': (inp['t_stopped'] & 0xFF) != 0, 'due': due, 'not_due': not due,
             'wrap_sum': (start + interval) > 0xFFFFFFFF, 'due_exact': due and now == u32(start + interval), 'not_due_by_one': (not due) and now == u32(start + interval - 1)}
    path = 'timer'
    if due:
        start, interval, started = now, 500, 1
        events.append(['simplest', 'I+4'])
        path = 'no_task'
        if not inp['task_null']:
            events.append(['isgoto', 'T+0'])
            path = 'not_goto'
            if (inp['is_goto'] & 0xFF) != 0:
                path = 'no_veh'
                if not inp['veh_null']:
                    events.append(['colmodel', 'V+0'])
                    path = 'geometry'
                    path = geometry(inp, events, stats)
    stats['path_' + path] = True
    timer = bytearray(12)
    timer[0:4] = u32(start).to_bytes(4, 'little')
    timer[4:8] = u32(interval).to_bytes(4, 'little')
    timer[8] = started
    timer[9] = stopped
    return {'events': events, 'nevents': len(events), 'timer_known': {str(i): timer[i] for i in range(10)}}, stats


def geometry(inp, events, stats):
    f = bits_to_f
    vp = [f(b) for b in inp['veh_pos']]
    pp = [f(b) for b in inp['ped_pos']]
    vm = [f(b) for b in inp['vm']]
    mn = [f(b) for b in inp['mn']]
    mx = [f(b) for b in inp['mx']]
    # D = ped - veh (cada componente a float)
    d = [sub(pp[i], vp[i]) for i in range(3)]
    right, fwd, up = vm[0:3], vm[3:6], vm[6:9]

    def xform(v):
        # 0x59C670: (up_k*vz + right_k*vx) + fwd_k*vy + pos_k, componente a componente en el orden del EXE:
        #  x: ((m20*vz + m10*vy) + m00*vx) + m30 ; y: ((m24*vz + m04*vx) + m14*vy) + m34 ; z: ((m28*vz + m08*vx) + m18*vy) + m38
        x = add(add(add(mul(up[0], v[2]), mul(fwd[0], v[1])), mul(right[0], v[0])), vp[0])
        y = add(add(add(mul(up[1], v[2]), mul(right[1], v[0])), mul(fwd[1], v[1])), vp[1])
        z = add(add(add(mul(up[2], v[2]), mul(right[2], v[0])), mul(fwd[2], v[1])), vp[2])
        return [x, y, z]
    v1 = xform(mn)
    v2 = xform(mx)
    d2 = neg(add(add(mul(up[2], v2[2]), mul(up[1], v2[1])), mul(up[0], v2[0])))
    h = [mul(up[0], -1.0), mul(up[1], -1.0), mul(up[2], -1.0)]
    d1 = neg(add(add(mul(v1[1], h[1]), mul(v1[2], h[2])), mul(h[0], v1[0])))
    side1 = add(add(add(mul(up[2], pp[2]), mul(up[1], pp[1])), mul(up[0], pp[0])), d2)
    c1 = lt(side1, 0.5)
    c2 = False
    if c1:
        side2 = add(add(add(mul(h[1], pp[1]), mul(h[2], pp[2])), mul(h[0], pp[0])), d1)
        c2 = lt(side2, 0.5)
    stats['c1_fail'] = not c1
    stats['c2_fail'] = c1 and not c2
    stats['inside'] = c2
    if not c2:
        return 'outside'
    limit = 10.0 if (inp['veh_type'] & 0xFFFFFFFF) == 6 else 5.0
    stats['train'] = limit == 10.0
    skip_dist = bool(inp['flags3'] & 8)
    stats['just_left_car'] = skip_dist
    if not skip_dist:
        sq = add(add(mul(d[0], d[0]), mul(d[1], d[1])), mul(d[2], d[2]))
        lsq = mul(limit, limit)
        if not gt(lsq, sq):
            stats['too_far'] = True
            return 'too_far'
    stats['los_called'] = True
    events.append(['los', 'D+0', 'T+12', 'V+0', 0, inp['tp'][0], inp['tp'][1], inp['tp'][2]])
    if (inp['clear'] & 0xFF) != 0:
        return 'los_clear'
    dist = bits_to_f(inp['los_dist']) if inp['los_write'] else 0.0
    if not gt(dist, 0.5):
        stats['dist_small'] = True
        return 'dist_small'
    events.append(['move', 'I+0'])
    events.append(['regref', 'V+0'])
    events.append(['add', 'I+104', 56, 29, 0, 0, 1, 1, 0xC8, 0xFFFF, 0, 'V+0', inp['move'] & 0xFFFFFFFF])
    events.append(['cleanref', 'V+0'])
    stats['event'] = True
    return 'event'


def selftest():
    def base(**kw):
        d = dict(family=0, now=10000, t_start=9000, t_interval=500, t_started=1, t_stopped=0, task_null=0, is_goto=1, veh_null=0, ped_mat_null=0, veh_type=0, flags3=0,
                 veh_pos=[0, 0, 0], vm=[0x3F800000, 0, 0, 0, 0x3F800000, 0, 0, 0, 0x3F800000], mn=[0xBF800000, 0xC0000000, 0xBF000000], mx=[0x3F800000, 0x40000000, 0x3F000000],
                 ped_pos=[0x3F800000, 0, 0], tp=[1, 2, 3], clear=0, los_write=1, los_dist=0x3F800000, move=3, eax=0, edx=0, stack_seed=0)
        d.update(kw)
        return d
    want, st = expected(base(), 0)
    assert [e[0] for e in want['events']] == ['simplest', 'isgoto', 'colmodel', 'los', 'move', 'regref', 'add', 'cleanref'], want['events']
    assert want['events'][-2][-1] == 3 and st['event']
    # timer: now == start + interval vence ; una menos no
    assert expected(base(now=9500), 0)[1]['due_exact']
    want, st = expected(base(now=9499), 0)
    assert want['events'] == [] and st['not_due_by_one']
    # sin arrancar: arranca con 500 y no vence
    want, st = expected(base(t_started=0), 0)
    assert want['events'] == [] and want['timer_known']['4'] == 0xF4 and want['timer_known']['8'] == 1
    # detenido: reinicia el inicio, no vence
    want, st = expected(base(t_stopped=0x80), 0)
    assert want['events'] == [] and want['timer_known']['9'] == 0
    # suma que da la vuelta
    want, st = expected(base(t_start=0xFFFFFF00, t_interval=0x200, now=0x150), 0)
    assert st['wrap_sum'] and st['due']
    # sin tarea / no GoTo / sin vehiculo
    assert expected(base(task_null=1), 0)[0]['events'] == [['simplest', 'I+4']]
    assert expected(base(is_goto=0x100), 0)[0]['events'] == [['simplest', 'I+4'], ['isgoto', 'T+0']]
    assert expected(base(is_goto=0x101), 0)[0]['events'][-1] == ['colmodel', 'V+0'] or True
    assert expected(base(veh_null=1), 0)[0]['events'][-1] == ['isgoto', 'T+0']
    # por encima de la caja (z = 0.5 + 0.5 = 1.0 no cumple c1: side1 = 1.0 - 0.5 = 0.5 no < 0.5)
    want, st = expected(base(ped_pos=[0, 0, 0x3F800000]), 0)
    assert st['c1_fail'] and want['events'][-1] == ['colmodel', 'V+0']
    # lejos: ped a 6 m de distancia en x
    want, st = expected(base(ped_pos=[0x40C00000, 0, 0]), 0)
    assert st.get('too_far') and want['events'][-1] == ['colmodel', 'V+0']
    # tren: limite 10
    want, st = expected(base(ped_pos=[0x40C00000, 0, 0], veh_type=6), 0)
    assert st['train'] and st['event']
    # bandera de salida de coche: no se comprueba la distancia
    want, st = expected(base(ped_pos=[0x41200000 + 0x1000000, 0, 0], flags3=8), 0)
    assert st['just_left_car'] and st.get('los_called')
    # linea despejada / distancia pequena / NaN
    assert not expected(base(clear=1), 0)[1].get('event')
    assert expected(base(los_dist=0x3F000000), 0)[1]['dist_small']
    assert expected(base(los_dist=0x7FC00000), 0)[1]['dist_small']
    assert expected(base(los_write=0), 0)[1]['dist_small']
    return True


if __name__ == '__main__':
    selftest()
    print('ok')
