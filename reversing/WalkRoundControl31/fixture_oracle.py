"""Oraculo independiente del banco WalkRoundControl31: CTaskComplexWalkRoundObject::ControlSubTask (0x6575F0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x6575F0 (thiscall, ECX=this -> ESI, un argumento de pila: ped (EBX), `ret 4`; sin marco SEH; guarda EBX/ESI/EDI):
  6575F8  eax = [esi+1Ch] (m_object) ; test ; je 657718                                          (sin objeto: aborto)
  657604  GetTaskType = call [[esi+8]+10h] ; cmp eax,389h ; jne 65764A ; al=[esi+2Ch] ; test ; je 65764A
  65761A  al=[esi+2Dh] ; test ; je 65762E ; [esi+24h] = [0xB7CB84] (reloj) ; byte [esi+2Dh] = 0
  65762E  eax=[esi+24h] ; edx=[esi+28h]+eax (32 bits) ; edi=[0xB7CB84] ; cmp edi,edx ; sbb al,al ; inc al ; test ; jne 657718       (aborto si NO reloj < suma, sin signo)
  65764A  eax=[ped+47Ch] ; ecx=[eax+274h] ; cmp ecx,[86C938h] (=30) ; jg 657718                            (aborto si contador > 30, con signo)
  657662  GetTaskType (otra vez) ; cmp eax,0CBh ; je 65773C                                           (reposo: devuelve la subtarea)
  657675  edi=[esi+1Ch] ; si [edi+14h] (m_matrix) == 0: call 54F560 (AllocateMatrix, this=edi) ; call 54F1B0 (UpdateMatrix, this=edi+4, arg=[edi+14h])
  657692  ecx=[[esi+1Ch]+14h] ; edi=[edi+14h] (matriz) ; eax = ecx ? ecx+30h : objeto+4 (posicion)
  6576A7  dx=[esi+30h]-[eax], dy=[esi+34h]-[eax+4], dz=[esi+38h]-[eax+8] (fsub en la x87, 24 bits) ; (dz*dz + dy*dy) + dx*dx sin guardar ; fcomp [86F15C]=0.0625 ; test ah,41h ; je 657718   (aborto si mayor, ordenado)
  6576DB  (fwd.z*[esi+44h] + fwd.y*[esi+40h]) + [esi+3Ch]*fwd.x contra [86F160]=0.9f ; fnstsw ; test ah,5 ; jnp 657718            (aborto si menor, ordenado; NaN no)
  6576FE  call 40FDB0 (DotProduct(&[esi+48h], matriz)) = (a.z*b.z + a.y*b.y) + a.x*b.x ; fcomp [86F160] ; test ah,5 ; jp 65773C        (devuelve la subtarea si no es menor; NaN devuelve)
  657718  call [[esi+8]+18h](ped, 1 (URGENT), 0) ; test al,al ; je 65773C                              (MakeAbortable: solo AL)
  657729  call 655290(this, 0CBh, ped) -> new(0x20) ; 62F310(500, 0, 0, 8.0f) ; ret                  (CTaskSimpleStandStill; si new falla -> 0)
  65773C  eax=[esi+8] ; ret 4                                                                       (devuelve la subtarea)
Llamadores: CTaskComplex::ProcessPed (0x61... / virtual) usan EAX; nadie lee ECX/EDX (ver README.md).

Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0) ; prueba el bit 7 del estado ; si libre:
  estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
StandStill (0x62F310, tras 0x61A390 {[+4]=0}): [+08]=tiempo ; [+0C]=0 ; [+10]=0 ; byte[+14]=0 ; byte[+15]=0 ; byte[+18]=looped ; byte[+19]=useAnimIdle ; [+1C]=mezcla (8.0f).
La vtable (palabra 0 de la tarea) se sustituye por GetTaskType: StandStill 0xCB (62F350).
"""
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexWalkRoundObject::ControlSubTask'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_STAND = 0xCB
TYPE_FOLLOW = 0x389
F_BLEND8 = 0x41000000
LIMIT = 30
OFF_F24, OFF_F2D = 0x24, 0x2D
INF = math.inf


# ------------------------------------------------------------------------------------------------ x87 (24 bits de mantisa, exponente extendido)
def r24(x):
    """Redondeo al mas cercano (empates al par) de un Fraction a 24 bits de mantisa, sin limite de exponente."""
    if x == 0:
        return Fraction(0)
    s = -1 if x < 0 else 1
    a = abs(x)
    k = a.numerator.bit_length() - a.denominator.bit_length()
    if a < Fraction(2) ** k:
        k -= 1
    scaled = a / (Fraction(2) ** (k - 23))
    q = scaled.numerator // scaled.denominator
    rem = scaled - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return s * Fraction(q) * (Fraction(2) ** (k - 23))


def val(bits):
    f = struct.unpack('<f', struct.pack('<I', bits & M32))[0]
    return Fraction(f) if math.isfinite(f) else f


def _tf(x):
    return float(x) if isinstance(x, Fraction) else x


def fadd(a, b):
    return r24(a + b) if isinstance(a, Fraction) and isinstance(b, Fraction) else _tf(a) + _tf(b)


def fsub(a, b):
    return r24(a - b) if isinstance(a, Fraction) and isinstance(b, Fraction) else _tf(a) - _tf(b)


def fmul(a, b):
    return r24(a * b) if isinstance(a, Fraction) and isinstance(b, Fraction) else _tf(a) * _tf(b)


def isnan(x):
    return isinstance(x, float) and x != x


def gt(a, b):
    return False if isnan(a) or isnan(b) else a > b


def lt(a, b):
    return False if isnan(a) or isnan(b) else a < b


def moved(start, cur):
    """(dz*dz + dy*dy) + dx*dx > 0.0625, dx = start.x - cur.x ... (0x6576A7)."""
    d = [fsub(val(start[i]), val(cur[i])) for i in range(3)]
    s = fadd(fadd(fmul(d[2], d[2]), fmul(d[1], d[1])), fmul(d[0], d[0]))
    return gt(s, Fraction(1, 16))


def dot_below(a, b):
    """(a.z*b.z + a.y*b.y) + a.x*b.x < 0.9f (0x86F160)."""
    s = fadd(fadd(fmul(val(a[2]), val(b[2])), fmul(val(a[1]), val(b[1]))), fmul(val(a[0]), val(b[0])))
    return lt(s, val(0x3F666666))


# ------------------------------------------------------------------------------------------------ bytes de la tarea nueva (ranura de 0x80 bytes rellena con 0xCD)
def stand_words(time):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = time & M32
    w[3] = 0
    w[4] = 0
    w[5] = 0xCDCD0000            # bytes [+14], [+15] = 0
    w[6] = 0xCDCD0000            # bytes [+18] (looped), [+19] (useAnimIdle) = 0
    w[7] = F_BLEND8
    return w


def le_bytes(v):
    return [(v >> (8 * i)) & 0xFF for i in range(4)]


def expected_control(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(1, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    if inp['limit'] != LIMIT:
        raise RuntimeError('El limite estatico 0x86C938 no vale 30: %r' % inp['limit'])
    obj_null = bool(inp['obj_null'])
    sub_type = inp['sub_type'] & M32
    f2c, f2d, f24, f28, time = inp['f2c'] & 0xFF, inp['f2d'] & 0xFF, inp['f24'] & M32, inp['f28'] & M32, inp['time'] & M32
    counter = inp['counter']
    if not -(1 << 31) <= counter < (1 << 31):
        raise RuntimeError('contador fuera de rango')
    calls = []
    new_f24, new_f2d = f24, f2d
    abort = False
    timer_branch = False
    timer_reset = False
    timer_expired = False
    counter_abort = False
    early_stand = False
    matrix_alloc = False
    moved_abort = fwd_abort = right_abort = False
    reached_geometry = False
    if obj_null:
        abort = True
    else:
        calls.append(['GT', 'S+0'])
        if sub_type == TYPE_FOLLOW and f2c:
            timer_branch = True
            if f2d:
                new_f24 = time
                new_f2d = 0
                timer_reset = True
            if not (time < ((new_f24 + f28) & M32)):
                abort = True
                timer_expired = True
        if not abort:
            if counter > LIMIT:
                abort = True
                counter_abort = True
            else:
                calls.append(['GT', 'S+0'])
                if sub_type == TYPE_STAND:
                    early_stand = True
                else:
                    reached_geometry = True
                    if not inp['mat_present']:
                        matrix_alloc = True
                        calls.append(['AM', 'O+0'])
                        calls.append(['UM', 'O+4', 'M+0'])
                    if moved(inp['start_pos'], inp['pos']):
                        abort = True
                        moved_abort = True
                    elif dot_below(inp['fwd'], inp['start_fwd']):
                        abort = True
                        fwd_abort = True
                    elif dot_below(inp['start_right'], inp['right']):
                        abort = True
                        right_abort = True
    alloc_attempt = False
    ret = 'S+0'
    cur = last
    changes = []
    tasks = []
    abortable = False
    if abort:
        calls.append(['MA', 'S+0', 'P+0', 1, 0])
        abortable = (inp['abort_res'] & 0xFF) != 0
        if abortable:
            alloc_attempt = True
            if win:
                idx, st = win[0]
                cur = idx
                changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
                ret = 'W0+0'
                tasks.append({'type': TYPE_STAND, 'words': stand_words(500), 'slot': 0})
            else:
                cur = 0
                ret = 0
    this_changes = []
    for i, (o, n_) in enumerate(zip(le_bytes(f24), le_bytes(new_f24))):
        if o != n_:
            this_changes.append([OFF_F24 + i, n_])
    if new_f2d != f2d:
        this_changes.append([OFF_F2D, new_f2d])
    want = {'ret': ret, 'calls': calls, 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_changes': this_changes, 'sub_ok': True, 'obj_matrix': 'M+0' if (matrix_alloc or (not obj_null and inp['mat_present'])) else 0, 'obj_other_ok': True,
            'mat_ok': True, 'ped_ok': True, 'int_ok': True, 'timer_ok': True}
    if obj_null:
        want['obj_matrix'] = 'M+0' if inp['mat_present'] else 0
    stats = {
        'family%d' % inp['family']: True,
        'obj_null': obj_null, 'obj_present': not obj_null,
        'type_follow': (not obj_null) and sub_type == TYPE_FOLLOW, 'type_stand': (not obj_null) and sub_type == TYPE_STAND, 'type_other': (not obj_null) and sub_type not in (TYPE_FOLLOW, TYPE_STAND),
        'timer_branch': timer_branch, 'timer_reset': timer_reset, 'timer_expired': timer_expired, 'timer_ok_path': timer_branch and not timer_expired,
        'timer_wrap': timer_branch and ((new_f24 + f28) > M32), 'timer_equal': timer_branch and time == ((new_f24 + f28) & M32),
        'f2c_noncanon': timer_branch and f2c > 1, 'f2d_noncanon': timer_branch and f2d > 1,
        'counter_abort': counter_abort, 'counter_eq': (not obj_null) and not timer_expired and counter == LIMIT, 'counter_gt': (not obj_null) and not timer_expired and counter == LIMIT + 1,
        'counter_neg': counter < 0, 'counter_extreme': counter in (-(1 << 31), (1 << 31) - 1),
        'early_stand': early_stand, 'geometry': reached_geometry, 'matrix_alloc': matrix_alloc,
        'moved_abort': moved_abort, 'fwd_abort': fwd_abort, 'right_abort': right_abort, 'all_pass': reached_geometry and not abort,
        'abort': abort, 'abortable': abort and abortable, 'not_abortable': abort and not abortable, 'abort_res_hi': abort and (inp['abort_res'] & 0xFFFFFF00) != 0,
        'abort_res_hi_al0': abort and (inp['abort_res'] & 0xFF) == 0 and (inp['abort_res'] & 0xFFFFFF00) != 0,
        'alloc_ok': alloc_attempt and bool(win), 'alloc_fail': alloc_attempt and not win, 'last_at_cap1': alloc_attempt and last == cap - 1,
        'search_wraps': alloc_attempt and bool(win) and win[0][0] <= last, 'id_wrap': alloc_attempt and bool(win) and (win[0][1] & 0x7F) == 0x7F,
        'pool_mode%d' % inp['pool_mode']: True,
        'nonfinite_geometry': reached_geometry and any(((b >> 23) & 0xFF) == 0xFF for v in (inp['pos'], inp['fwd'], inp['right'], inp['start_pos'], inp['start_fwd'], inp['start_right']) for b in v),
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_control(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    win = [[10, 0x80 | 0x7F]]
    one = 0x3F800000
    inp = dict(family=11, obj_null=0, sub_type=0x386, abort_res=1, f2c=0, f2d=0, f24=0, f28=0, time=0, counter=0, mat_present=1, limit=30,
               start_pos=[0, 0, 0], start_fwd=[one, 0, 0], start_right=[0, one, 0], pos=[0, 0, 0], fwd=[one, 0, 0], right=[0, one, 0], place=[0, 0, 0],
               pool_mode=0, pool_k=1, eax=0, edx=0, stack_seed=0, this_off=0x40,
               pool=dict(cap=5000, last=9, free_total=1000, win=win))
    inp.update(kw)
    return inp


if __name__ == '__main__':
    one = 0x3F800000
    # 1) todo en orden: devuelve la subtarea, sin cambios
    want, st = expected_control(_base(), 0)
    assert want['ret'] == 'S+0' and want['calls'] == [['GT', 'S+0'], ['GT', 'S+0']] and want['tasks'] == [] and want['last_after'] == 9 and st['all_pass'] and want['obj_matrix'] == 'M+0'
    # 2) sin objeto: aborto -> MakeAbortable; verdadero -> StandStill(500) en la primera ranura libre
    want, st = expected_control(_base(obj_null=1), 1)
    assert want['calls'] == [['MA', 'S+0', 'P+0', 1, 0]] and want['ret'] == 'W0+0' and want['tasks'][0]['words'][2] == 500 and want['last_after'] == 10 and want['state_changes'] == [[10, 0]]
    assert want['tasks'][0]['words'][7] == F_BLEND8 and st['alloc_ok'] and st['id_wrap'] and st['obj_null'] and want['obj_matrix'] == 'M+0'
    # 2b) MakeAbortable falso (solo AL cuenta) -> devuelve la subtarea sin tocar el pool
    want, st = expected_control(_base(obj_null=1, abort_res=0xFFFFFF00), 2)
    assert want['ret'] == 'S+0' and want['last_after'] == 9 and want['state_changes'] == [] and st['not_abortable'] and st['abort_res_hi_al0']
    # 2c) pool sin ranuras: new falla -> 0 y el cursor queda en 0
    want, st = expected_control(_base(obj_null=1, pool=dict(cap=5000, last=77, free_total=0, win=[])), 3)
    assert want['ret'] == 0 and want['last_after'] == 0 and want['tasks'] == [] and st['alloc_fail']
    # 3) plazo: ruta con field_2C, reinicio de field_2D: field_24 = reloj; reloj < reloj + field_28 -> sigue
    want, st = expected_control(_base(sub_type=0x389, f2c=1, f2d=5, f24=0x11223344, f28=100, time=0x1000), 4)
    assert want['this_changes'] == [[0x24, 0x00], [0x25, 0x10], [0x26, 0x00], [0x27, 0x00], [0x2D, 0]] or want['this_changes'][-1] == [0x2D, 0], want['this_changes']
    assert st['timer_reset'] and not st['timer_expired'] and st['all_pass']
    # 3b) plazo vencido: reloj >= suma (sin signo): aborto sin llamar a GetTaskType la segunda vez
    want, st = expected_control(_base(sub_type=0x389, f2c=2, f2d=0, f24=100, f28=50, time=150), 5)
    assert want['calls'] == [['GT', 'S+0'], ['MA', 'S+0', 'P+0', 1, 0]] and st['timer_expired'] and want['ret'] == 'W0+0'
    # 3c) envoltura de la suma a 32 bits: 0xFFFFFFF0 + 0x20 = 0x10 ; reloj 0xFFFFFFF5 no es menor -> aborto ; reloj 5 si
    want, st = expected_control(_base(sub_type=0x389, f2c=1, f24=0xFFFFFFF0, f28=0x20, time=0xFFFFFFF5), 6)
    assert st['timer_expired'] and st['timer_wrap']
    want, st = expected_control(_base(sub_type=0x389, f2c=1, f24=0xFFFFFFF0, f28=0x20, time=5), 6)
    assert not st['timer_expired'] and st['timer_wrap'] and st['all_pass']
    # 4) contador: > 30 con signo aborta ; 30 no ; negativo no
    for c, ab in ((30, False), (31, True), (-1, False), (-(1 << 31), False), ((1 << 31) - 1, True)):
        want, st = expected_control(_base(counter=c), 7)
        assert st['counter_abort'] == ab, c
        assert (want['calls'][0] == ['GT', 'S+0']) and (len(want['calls']) == (2 if not ab else 2) or True)
    # 5) reposo: devuelve la subtarea sin tocar la matriz
    want, st = expected_control(_base(sub_type=0xCB, mat_present=0), 8)
    assert want['ret'] == 'S+0' and want['calls'] == [['GT', 'S+0'], ['GT', 'S+0']] and st['early_stand'] and want['obj_matrix'] == 0
    # 6) sin matriz: AllocateMatrix + UpdateMatrix antes de leer la posicion
    want, st = expected_control(_base(mat_present=0), 9)
    assert want['calls'] == [['GT', 'S+0'], ['GT', 'S+0'], ['AM', 'O+0'], ['UM', 'O+4', 'M+0']] and want['obj_matrix'] == 'M+0' and st['matrix_alloc']
    # 7) posicion: 0.25 en un eje -> distancia^2 = 0.0625 exacto, no aborta ; el siguiente float aborta
    want, st = expected_control(_base(pos=[0x3E800000, 0, 0]), 10)
    assert not st['moved_abort']
    want, st = expected_control(_base(pos=[0x3E800001, 0, 0]), 10)
    assert st['moved_abort'] and want['calls'][-1][0] == 'MA'
    want, st = expected_control(_base(pos=[0x7FC00000, 0, 0]), 10)   # NaN no aborta (comparacion ordenada)
    assert not st['moved_abort'] and st['all_pass']
    want, st = expected_control(_base(pos=[0x7F800000, 0, 0]), 10)   # inf aborta
    assert st['moved_abort']
    # 8) frente: 0.9f exacto no aborta ; el float anterior si ; derecha igual
    want, st = expected_control(_base(fwd=[0x3F666666, 0, 0]), 11)
    assert not st['fwd_abort']
    want, st = expected_control(_base(fwd=[0x3F666665, 0, 0]), 11)
    assert st['fwd_abort']
    want, st = expected_control(_base(right=[0, 0x3F666665, 0]), 11)
    assert st['right_abort'] and not st['fwd_abort']
    want, st = expected_control(_base(right=[0, 0x7FC00000, 0]), 11)
    assert not st['right_abort'] and st['all_pass']
    # 9) exponente extendido: (1e30*1e30) - (1e30*1e30) no desborda en la x87 (daria NaN con floats)
    big = 0x7149F2CA   # 1e30
    want, st = expected_control(_base(fwd=[big, 0, 0], start_fwd=[big, 0, 0]), 12)   # 1e60 en x87 > 0.9 -> no aborta
    assert not st['fwd_abort']
    want, st = expected_control(_base(fwd=[big, big, 0], start_fwd=[big, 0xF149F2CA, 0]), 12)   # 1e60 + (-1e60) = 0 < 0.9 -> aborta (con floats seria NaN y no abortaria)
    assert st['fwd_abort']
    # 10) fixtures invalidas
    for bad in (_base(limit=29), _base(pool=dict(cap=5000, last=9, free_total=1, win=[]))):
        try:
            expected_control(bad, 13)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except (RuntimeError, KeyError):
            pass
    # 11) redondeo a 24 bits: (2^24 + 1) no es representable -> 2^24 ; empate al par
    assert r24(Fraction(2 ** 24 + 1)) == 2 ** 24 and r24(Fraction(2 ** 24 + 3)) == 2 ** 24 + 4 and r24(Fraction(1, 3)) == Fraction(11184811, 2 ** 25)
    print('ok')
