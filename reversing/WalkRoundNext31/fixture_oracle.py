"""Oraculo independiente del banco WalkRoundNext31: CTaskComplexWalkRoundObject::CreateNextSubTask (0x657220).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x657220 (thiscall, ECX=this -> ESI, un argumento de pila: ped (EBP), `ret 4`; marco SEH propio con fs:[0]; guarda ESI/EDI/EBP/EBX):
  657239  esi=this ; edi=0 ; eax=[esi+1Ch] (m_object) ; test ; jne 65725A ; si es 0: eax=0 ; ret                          (sin objeto: nulo)
  65725A  GetTaskType = call [[esi+8]+10h] ; cmp eax,387h ; jg 657356 ; je 65728E ; cmp eax,0CBh ; je 65735D ; cmp eax,386h ; jne 65735F
  657356  (> 0x387) cmp eax,389h ; jne 65735F ; 65735D: edi=0                                                            (0x389 y 0xCB: nulo; cualquier otro: edi=0 inicial = nulo)
  65728E  (0x387) CreateRouteTask(ped) -> edi
  657286  (0x386) ecx=[esi+20h] ; cmp [ecx],0 ; je 6572A1 ; si el contador != 0 -> 65728E (CreateRouteTask)
  6572A1  CP = 5F3B70(ped, objeto, &punto) ; side1 = 5F3AC0(&punto, objeto) ; side2 = 5F3AC0(&[esi+10h] (destino), objeto) ; cmp side1,side2 (32 bits) ; jne 657312
  6572DA  (iguales) 48E180(ruta, &punto) (anade si contador < 8, con signo) ; 5F36F0(&[esi+10h], objeto, &punto) ; 48E180(ruta, &punto) ; CreateRouteTask(ped) -> edi
  657312  (distintos) new(0x28) ; 668120(4, &punto, [86FC84]=0.5f, [86FC88]=2.0f, 0, 0) ; edi = eax (o 0 si new falla)
  65735F  eax = edi ; restaura SEH ; ret 4
Colaboradores sustituidos por falsos: 5F3B70 (escribe el punto P1), 5F3AC0 (devuelve side1 y luego side2), 5F36F0 (escribe el punto P2), 655140 CreateRouteTask (devuelve un valor fijo).
Llamadores: CTaskComplex::ProcessPed (virtual) usa EAX; nadie lee ECX/EDX (ver README.md).

Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0) ; prueba el bit 7 del estado ; si libre:
  estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
CTaskComplexGoToPointAndStandStill (0x668120, tras 0x61A3B0 {[+4]=0, [+8]=0}): [+0C]=moveState ; [+10..+1B]=punto ; [+1C]=radio ; [+20]=radio de estado ; byte [+24] = ((old & 0xE4) | 4) -> 0xC4 sobre 0xCD
  (con una ranura recien rellena de 0xCD: 509760 / los fcomp contra 0xCDCDCDCD nunca coinciden, asi que siempre se copia el punto y se pone el bit 2) ; bytes [+25..+27] y [+28..] sin escribir.
La vtable (palabra 0 de la tarea) se sustituye por GetTaskType: GoToPointAndStandStill 0x387.
"""
import math
import struct
import sys

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexWalkRoundObject::CreateNextSubTask'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_GOTO = 0x387
TYPE_ACHIEVE = 0x386
F_HALF, F_TWO = 0x3F000000, 0x40000000
ROUTE_WORDS = 25


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def goto_words(point):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 4                     # PEDMOVE_WALK
    w[4], w[5], w[6] = (p & M32 for p in point)
    w[7] = F_HALF
    w[8] = F_TWO
    w[9] = 0xCDCDCDC4            # byte [+24] = (0xCD & 0xE4) | 4 ; bytes +25..+27 sin escribir
    return w


def expected_next(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(1, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    obj_null = bool(inp['obj_null'])
    sub_type = inp['sub_type'] & M32
    route = [w & M32 for w in inp['route_init']]
    if len(route) != ROUTE_WORDS:
        raise RuntimeError('route_init debe tener %d palabras' % ROUTE_WORDS)
    count0 = route[0]
    target, p1, p2 = inp['target'], inp['p1'], inp['p2']
    side1, side2 = s32(inp['side1']), s32(inp['side2'])
    calls = []
    ret = 0
    route_ret = inp['route_ret'] & M32
    alloc_attempt = False
    cur = last
    changes = []
    tasks = []
    path = 'null'
    equal_sides = False
    if not obj_null:
        calls.append(['GT', 'S+0'])
        if sub_type == TYPE_GOTO:
            calls.append(['CR', 'T+0', 'P+0'])
            ret = route_ret
            path = 'route_direct'
        elif sub_type == TYPE_ACHIEVE:
            if count0 != 0:
                calls.append(['CR', 'T+0', 'P+0'])
                ret = route_ret
                path = 'route_loaded'
            else:
                calls.append(['CP', 'P+0', 'O+0'])
                calls.append(['HS', p1[0], p1[1], p1[2], 'O+0'])
                calls.append(['HS', target[0], target[1], target[2], 'O+0'])
                if side1 == side2:
                    equal_sides = True
                    route[0] = 1                     # 48E180: anade si contador < 8 (aqui 0)
                    route[1], route[2], route[3] = p1
                    calls.append(['CQ', target[0], target[1], target[2], 'O+0'])
                    route[0] = 2
                    route[4], route[5], route[6] = p2
                    calls.append(['CR', 'T+0', 'P+0'])
                    ret = route_ret
                    path = 'route_built'
                else:
                    alloc_attempt = True
                    path = 'goto'
                    if win:
                        idx, st = win[0]
                        cur = idx
                        changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
                        ret = 'W0+0'
                        tasks.append({'type': TYPE_GOTO, 'words': goto_words(p1), 'slot': 0})
                    else:
                        cur = 0
                        ret = 0
    want = {'ret': ret, 'calls': calls, 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_ok': True, 'sub_ok': True, 'obj_ok': True, 'ped_ok': True, 'route_guards_ok': True, 'route_after': route}
    stats = {
        'family%d' % inp['family']: True,
        'obj_null': obj_null, 'type_goto': (not obj_null) and sub_type == TYPE_GOTO, 'type_achieve': (not obj_null) and sub_type == TYPE_ACHIEVE,
        'type_other': (not obj_null) and sub_type not in (TYPE_GOTO, TYPE_ACHIEVE), 'type_stand': (not obj_null) and sub_type == 0xCB, 'type_follow': (not obj_null) and sub_type == 0x389,
        'route_direct': path == 'route_direct', 'route_loaded': path == 'route_loaded', 'route_built': path == 'route_built', 'goto': path == 'goto',
        'count_zero': (not obj_null) and sub_type == TYPE_ACHIEVE and count0 == 0, 'count_nonzero': (not obj_null) and sub_type == TYPE_ACHIEVE and count0 != 0,
        'count_neg': (not obj_null) and sub_type == TYPE_ACHIEVE and count0 >= 0x80000000, 'count_ge8': (not obj_null) and sub_type == TYPE_ACHIEVE and 8 <= count0 < 0x80000000,
        'sides_equal': equal_sides, 'sides_differ': path == 'goto', 'side_negative': path in ('goto', 'route_built') and (side1 < 0 or side2 < 0),
        'side_extreme': path in ('goto', 'route_built') and (side1 in (-(1 << 31), (1 << 31) - 1) or side2 in (-(1 << 31), (1 << 31) - 1)),
        'side_low_equal_high_differ': path == 'goto' and (side1 & 0xFFFF) == (side2 & 0xFFFF),
        'route_ret_null': path in ('route_direct', 'route_loaded', 'route_built') and route_ret == 0,
        'alloc_ok': alloc_attempt and bool(win), 'alloc_fail': alloc_attempt and not win, 'last_at_cap1': alloc_attempt and last == cap - 1,
        'search_wraps': alloc_attempt and bool(win) and win[0][0] <= last, 'id_wrap': alloc_attempt and bool(win) and (win[0][1] & 0x7F) == 0x7F,
        'pool_mode%d' % inp['pool_mode']: True,
        'nonfinite_points': path in ('goto', 'route_built') and any(((b >> 23) & 0xFF) == 0xFF for v in (p1, p2, target) for b in v),
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_next(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    win = [[10, 0x80 | 0x7F]]
    inp = dict(family=3, obj_null=0, sub_type=0x386, route_init=[0] + list(range(1, ROUTE_WORDS)), route_ret=0x77000001, target=[1, 2, 3], p1=[10, 11, 12], p2=[20, 21, 22],
               side1=1, side2=1, move_state=0, pool_mode=0, pool_k=1, eax=0, edx=0, stack_seed=0, this_off=0x40,
               pool=dict(cap=5000, last=9, free_total=1000, win=win))
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 1) AchieveHeading, ruta vacia, mismo lado: anade P1 y P2 y crea la ruta
    want, st = expected_next(_base(), 0)
    assert want['calls'] == [['GT', 'S+0'], ['CP', 'P+0', 'O+0'], ['HS', 10, 11, 12, 'O+0'], ['HS', 1, 2, 3, 'O+0'], ['CQ', 1, 2, 3, 'O+0'], ['CR', 'T+0', 'P+0']]
    assert want['route_after'][:7] == [2, 10, 11, 12, 20, 21, 22] and want['route_after'][7:] == list(range(7, ROUTE_WORDS)) and want['ret'] == 0x77000001 and st['route_built'] and st['sides_equal']
    assert want['last_after'] == 9 and want['tasks'] == []
    # 2) lados distintos: GoToPointAndStandStill hacia P1 en la primera ranura libre
    want, st = expected_next(_base(side2=2), 1)
    assert want['calls'] == [['GT', 'S+0'], ['CP', 'P+0', 'O+0'], ['HS', 10, 11, 12, 'O+0'], ['HS', 1, 2, 3, 'O+0']] and want['route_after'][0] == 0
    assert want['ret'] == 'W0+0' and want['tasks'][0]['type'] == 0x387 and want['tasks'][0]['words'][3:10] == [4, 10, 11, 12, F_HALF, F_TWO, 0xCDCDCDC4] and want['state_changes'] == [[10, 0]]
    assert want['last_after'] == 10 and st['goto'] and st['alloc_ok'] and st['id_wrap']
    # 2b) lados con los 16 bajos iguales pero distintos: son distintos (comparacion de 32 bits)
    want, st = expected_next(_base(side1=0x10001, side2=1), 1)
    assert st['goto'] and st['side_low_equal_high_differ']
    want, st = expected_next(_base(side1=-(1 << 31), side2=-(1 << 31)), 1)
    assert st['route_built'] and st['side_extreme']
    # 2c) pool sin ranuras: nulo y cursor a 0
    want, st = expected_next(_base(side2=2, pool=dict(cap=5000, last=77, free_total=0, win=[])), 2)
    assert want['ret'] == 0 and want['last_after'] == 0 and want['tasks'] == [] and st['alloc_fail']
    # 3) ruta ya cargada (contador != 0, tambien con signo / enorme): CreateRouteTask
    for cnt in (1, 8, 9, 0x80000000, 0xFFFFFFFF):
        want, st = expected_next(_base(route_init=[cnt] + [5] * 24), 3)
        assert want['calls'] == [['GT', 'S+0'], ['CR', 'T+0', 'P+0']] and want['route_after'][0] == cnt and st['route_loaded']
    # 4) GoToPointAndStandStill (0x387): CreateRouteTask directo, sea cual sea el contador
    want, st = expected_next(_base(sub_type=0x387), 4)
    assert want['calls'] == [['GT', 'S+0'], ['CR', 'T+0', 'P+0']] and want['ret'] == 0x77000001 and st['route_direct']
    # 5) otros tipos: nulo tras una sola llamada a GetTaskType ; sin objeto: nulo sin llamadas
    for t in (0xCB, 0x389, 0x385, 0x388, 0x0, 0xFFFFFFFF):
        want, st = expected_next(_base(sub_type=t), 5)
        assert want['ret'] == 0 and want['calls'] == [['GT', 'S+0']]
    want, st = expected_next(_base(obj_null=1), 6)
    assert want['ret'] == 0 and want['calls'] == [] and st['obj_null']
    # 6) fixtures invalidas
    for bad in (_base(pool=dict(cap=5000, last=9, free_total=1, win=[])), _base(route_init=[0])):
        try:
            expected_next(bad, 7)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except (RuntimeError, KeyError):
            pass
    print('ok')
