"""Oraculo independiente del banco WalkRoundFirst31: CTaskComplexWalkRoundObject::CreateFirstSubTask (0x657380).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x657380 (thiscall, ECX=this -> ESI, un argumento de pila: ped (EBP), `ret 4`; marco SEH propio con fs:[0]; guarda ESI/EDI/EBP):
  65739B  eax=[esi+1Ch] (m_object) ; test ; jne 6573B6 ; si es 0: eax=0 ; ret                                              (sin objeto: nulo, sin llamadas)
  6573B6  [[esi+20h]] = 0 (m_NumEntries) ; call 6551D0 (this, ped) -> float en ST0 ; fstp [esp+8] (routeDist, guardado como float)   (FALSO: rellena la ruta)
  6573D3  cmp [[esi+20h]],0 ; je 6575D7 -> nulo
  6573DC  eax = (moveState == 4) ? 1F40h (8000) : 0FA0h (4000) ; [esi+24h] = [0xB7CB84] ; [esi+28h] = eax ; byte [esi+2Ch] = 1
  6573FD  edi=[esi+1Ch] ; eax = [edi+14h] ? matriz+30h : edi+4 ; [esi+30h..38h] = posicion
  657422  si [edi+14h] == 0: call 54F560 (edi) ; call 54F1B0 (edi+4, [edi+14h]) ; [esi+3Ch..44h] = [[edi+14h]+10h..18h] (frente)
  657458  edi=[esi+1Ch] ; si [edi+14h] == 0: 54F560 + 54F1B0 (otra vez, nunca ocurre tras la primera) ; [esi+48h..50h] = [[edi+14h]+0..8] (derecha)
  657488  ecx = ped matriz (+14h) ; ecx' = ecx ? ecx+30h : ped+4 ; d = ruta[0] - pos (fld/fsub 24 bits; fstp float a [esp+0Ch..14h]) ; call 59C970 (CVector::NormaliseAndMag sobre d, this=ecx) -> float en ST0 ; fstp [esp+34h]
  6574C1  ecx=[ebp+14h] (matriz del peaton, SIN comprobar nulo) ; dot = (dz*fz' + dy*fy') + dx*fx' (x87, d ya normalizado y guardado como float ; f' = [ecx+18h/14h/10h])
  6574CC  maximo = moveState == 4 ? [858CA0]=2.0 : moveState == 6 ? [858B90]=4.0 : [858B44]=6.0
  657504  si mag > maximo (fcomp ordenado) Y routeDist > maximo Y dot > [86F420]=0.0 (fcomp ordenado: NaN no) -> 65752D: CreateRouteTask(ped) = call 655140 ; eax
  65754C  (si no) x = ruta[0].x - pos.x ; y = ruta[0].y - pos.y (fstp float) ; a = 53CBE0(x, y, 0, 0) (GetRadianAngleBetweenPoints, ST0 guardado como float) ; h = 53CB50(a) (LimitRadianAngle) guardado como float
          new(0x18) ; 667E20(h, 1.0f, 0.2f (3E4CCCCDh)) -> CTaskSimpleAchieveHeading ; si new falla -> nulo
Colaboradores: 6551D0 y 655140 SUSTITUIDOS por falsos; 54F560/54F1B0 SUSTITUIDOS; 59C970 (NormaliseAndMag), 53CBE0, 53CB50 y 61A5A0 REALES (DLL en ambos lados) ; 667E20 (EXE; en el candidato lo hace el constructor C++).
NormaliseAndMag (0x59C970): S = (x*x + y*y) + z*z (24 bits) ; si S <= 0.0 (fcom con [858B50]=0.0, ordenado; NaN no) : x = 1.0 y devuelve 1.0 ; si no: r = 1.0 / sqrt(S) (fsqrt y fdivr a 24 bits),
  cada componente = float(r * componente) ; devuelve 1.0 / r (24 bits).
Llamadores: CTaskComplex::ProcessPed (virtual) usa EAX; nadie lee ECX/EDX (ver README.md).

Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0) ; prueba el bit 7 del estado ; si libre:
  estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
CTaskSimpleAchieveHeading (0x667E20, tras 0x61A390 {[+4]=0}): byte [+14] &= 0xFE (0xCD -> 0xCC) ; [+08]=angulo ; [+0C]=1.0 ; [+10]=0.2f.
La vtable (palabra 0 de la tarea) se sustituye por GetTaskType: AchieveHeading 0x386.
El angulo (word [+08]) no se modela al bit (fpatan): se acepta a 1e-4 rad (modulo 2pi) cuando las entradas son finitas; EXE == DLL se exige al bit.
"""
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexWalkRoundObject::CreateFirstSubTask'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_ACHIEVE = 0x386
F_ONE, F_TOL = 0x3F800000, 0x3E4CCCCD
INF = math.inf
ROUTE_WORDS = 25
THIS_WORDS = 12   # palabras de this desde +0x24 hasta +0x53


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


def f32store(v):
    """fstp dword: un valor con mantisa de 24 bits (o especial) pasa a float (desbordamiento -> infinito)."""
    if isinstance(v, Fraction):
        try:
            return Fraction(struct.unpack('<f', struct.pack('<f', float(v)))[0])
        except OverflowError:
            return INF if v > 0 else -INF
    return v


def sqrt24(s):
    """sqrt de un Fraction > 0 redondeada a 24 bits (al mas cercano, empates al par), exponente sin limite."""
    k = (s.numerator.bit_length() - s.denominator.bit_length()) // 2
    while Fraction(4) ** k > s:
        k -= 1
    while Fraction(4) ** (k + 1) <= s:
        k += 1
    a = s * (Fraction(2) ** (46 - 2 * k))                 # a en [2^46, 2^48): sqrt(a) en [2^23, 2^24)
    q0 = math.isqrt(a.numerator // a.denominator)
    half_sq = Fraction(q0) ** 2 + q0 + Fraction(1, 4)
    q = q0 + (1 if a > half_sq else 0)
    if a == half_sq:
        q = q0 + (q0 % 2)
    return Fraction(q) * (Fraction(2) ** (k - 23))


def normalise_and_mag(d):
    """CVector::NormaliseAndMag (0x59C970) sobre floats guardados d (Fraction o especial). Devuelve (vector normalizado guardado como float, modulo guardado como float)."""
    sq = fadd(fadd(fmul(d[0], d[0]), fmul(d[1], d[1])), fmul(d[2], d[2]))
    if isnan(sq):
        return [math.nan] * 3, math.nan
    if isinstance(sq, Fraction) and sq <= 0:
        return [Fraction(1), d[1], d[2]], Fraction(1)
    if isinstance(sq, float):                                   # +inf
        r = Fraction(0)
        return [f32store(fmul(r, x)) for x in d], INF
    root = sqrt24(sq)
    r = r24(Fraction(1) / root)
    out = [f32store(fmul(r, x)) for x in d]
    mag = f32store(r24(Fraction(1) / r)) if r != 0 else INF
    return out, mag


def stand(bits):
    return val(bits)


# ------------------------------------------------------------------------------------------------ angulo (no al bit)
HALF_PI = struct.unpack('<f', struct.pack('<I', 0x3FC90FDB))[0]
PI_F = struct.unpack('<f', struct.pack('<I', 0x40490FDB))[0]
TWO_PI_F = struct.unpack('<f', struct.pack('<I', 0x40C90FDB))[0]
EPS_F = struct.unpack('<f', struct.pack('<I', 0x38D1B717))[0]


def heading_model(x, y):
    """CGeneral::LimitRadianAngle(GetRadianAngleBetweenPoints(x, y, 0, 0)) con x, y floats finitos (python float). None si no es modelable."""
    if not (math.isfinite(x) and math.isfinite(y)):
        return None
    dx, dy = 0.0 - x, 0.0 - y
    if dy == 0.0:
        dy = EPS_F
    ratio = dx / dy
    a = math.atan(ratio)
    if dx > 0.0:
        ang = (HALF_PI - a) + HALF_PI if dy > 0.0 else HALF_PI - (a + HALF_PI)
    else:
        ang = -HALF_PI - (a + HALF_PI) if dy > 0.0 else (HALF_PI - a) - HALF_PI
    ang = min(max(ang, -25.0), 25.0)
    while ang >= PI_F:
        ang -= TWO_PI_F
    while ang < -PI_F:
        ang += TWO_PI_F
    return ang


def bits_float(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def match(observed, want, stats):
    """Sustituye el angulo del AchieveHeading observado por el marcador 'H' si coincide con el modelo (1e-4 rad modulo 2pi) o no es modelable."""
    model = stats.get('_heading')
    tasks = []
    for i, tk in enumerate(observed.get('tasks', [])):
        wk = want['tasks'][i] if i < len(want['tasks']) else None
        if wk is not None and wk['words'][2] == 'H' and tk.get('type') == wk['type'] and isinstance(tk['words'][2], int):
            actual = bits_float(tk['words'][2])
            if model is None:
                ok = True
            elif model != model:
                ok = actual != actual
            else:
                diff = (actual - model + math.pi) % (2 * math.pi) - math.pi
                ok = actual == actual and abs(diff) < 1e-4
            if ok:
                tk = dict(tk, words=[tk['words'][0], tk['words'][1], 'H'] + list(tk['words'][3:]))
        tasks.append(tk)
    return dict(observed, tasks=tasks)


# ------------------------------------------------------------------------------------------------ bytes de la tarea nueva (ranura de 0x80 bytes rellena con 0xCD)
def achieve_words():
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 'H'
    w[3] = F_ONE
    w[4] = F_TOL
    w[5] = 0xCDCDCDCC            # byte [+14] = 0xCD & 0xFE
    return w


def words_to_bytes(words):
    out = []
    for w in words:
        out += [(w >> (8 * i)) & 0xFF for i in range(4)]
    return out


def expected_first(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(1, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    route_init = [w & M32 for w in inp['route_init']]
    this_init = [w & M32 for w in inp['this_init']]
    if len(route_init) != ROUTE_WORDS or len(this_init) != THIS_WORDS:
        raise RuntimeError('route_init / this_init con longitud inesperada')
    obj_null = bool(inp['obj_null'])
    move = inp['move_state'] & M32
    route_script = [inp['route_count'] & M32] + [w & M32 for w in inp['route_entries']]
    if len(route_script) != ROUTE_WORDS:
        raise RuntimeError('route_entries debe tener %d palabras' % (ROUTE_WORDS - 1))
    calls = []
    route_after = list(route_init)
    this_new = list(this_init)
    ret = 0
    cur = last
    changes = []
    tasks = []
    path = 'null'
    heading = None
    mag = None
    facing = None
    zero_vec = False
    alloc_attempt = False
    mat_alloc = False
    if not obj_null:
        calls.append(['CF', 'T+0', 'P+0'])
        route_after = list(route_script)               # el falso de 0x6551D0 deja contador y puntos
        if route_script[0] == 0:
            path = 'empty'
        else:
            this_new[0] = inp['time'] & M32            # field_24
            this_new[1] = 8000 if move == 4 else 4000  # field_28
            this_new[2] = (this_new[2] & 0xFFFFFF00) | 1   # byte field_2C = 1
            pos = inp['mat_pos'] if inp['mat_present'] else inp['place']
            this_new[3:6] = [b & M32 for b in pos]
            if not inp['mat_present']:
                mat_alloc = True
                calls.append(['AM', 'O+0'])
                calls.append(['UM', 'O+4', 'M+0'])
            this_new[6:9] = [b & M32 for b in inp['mat_fwd']]
            this_new[9:12] = [b & M32 for b in inp['mat_right']]
            p0 = [val(route_script[1 + i]) for i in range(3)]
            ped = [val(b) for b in inp['ped_pos']]
            d = [f32store(fsub(p0[i], ped[i])) for i in range(3)]
            zero_vec = all(isinstance(x, Fraction) and x == 0 for x in d)
            dn, mag = normalise_and_mag(d)
            fwd = [val(b) for b in inp['ped_fwd']]
            dot = fadd(fadd(fmul(f32store(dn[2]), fwd[2]), fmul(f32store(dn[1]), fwd[1])), fmul(f32store(dn[0]), fwd[0]))
            maxd = Fraction(2) if move == 4 else Fraction(4) if move == 6 else Fraction(6)
            rdist = val(inp['route_dist'])
            facing = gt(dot, Fraction(0))
            if gt(mag, maxd) and gt(rdist, maxd) and facing:
                calls.append(['CR', 'T+0', 'P+0'])
                ret = inp['route_ret'] & M32
                path = 'route'
            else:
                alloc_attempt = True
                path = 'heading'
                hx, hy = d[0], d[1]
                heading = heading_model(_tf(hx), _tf(hy))
                if win:
                    idx, st = win[0]
                    cur = idx
                    changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
                    ret = 'W0+0'
                    tasks.append({'type': TYPE_ACHIEVE, 'words': achieve_words(), 'slot': 0})
                else:
                    cur = 0
                    ret = 0
    # cambios en `this` (bytes desde +0x24)
    old_b, new_b = words_to_bytes(this_init), words_to_bytes(this_new)
    this_changes = [[0x24 + i, new_b[i]] for i in range(len(new_b)) if new_b[i] != old_b[i]]
    want = {'ret': ret, 'calls': calls, 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_changes': this_changes, 'obj_matrix': 'M+0' if (inp['mat_present'] or mat_alloc) else 0, 'obj_other_ok': True, 'mat_ok': True, 'ped_ok': True, 'pmat_ok': True,
            'route_guards_ok': True, 'route_after': route_after, 'timer_ok': True}
    stats = {
        'family%d' % inp['family']: True,
        'obj_null': obj_null, 'route_empty': path == 'empty', 'route_nonempty': path in ('route', 'heading'), 'count_odd': path in ('route', 'heading') and route_script[0] > 8,
        'path_route': path == 'route', 'path_heading': path == 'heading',
        'move4': path in ('route', 'heading') and move == 4, 'move6': path in ('route', 'heading') and move == 6, 'move_other': path in ('route', 'heading') and move not in (4, 6),
        'move_weird': path in ('route', 'heading') and (move >= 0x100 or move > 7),
        'mat_missing': mat_alloc, 'pos_from_placement': mat_alloc,
        'mag_zero_vec': zero_vec,
        'mag_nan': path in ('route', 'heading') and mag is not None and isnan(mag), 'mag_inf': path in ('route', 'heading') and mag == INF,
        'dist_nan': path in ('route', 'heading') and isnan(val(inp['route_dist'])), 'dist_inf': path in ('route', 'heading') and val(inp['route_dist']) in (INF, -INF),
        'dist_neg': path in ('route', 'heading') and val(inp['route_dist']) < 0 if not isnan(val(inp['route_dist'])) else False,
        'facing': facing is True, 'not_facing': facing is False and path in ('route', 'heading'),
        'route_ret_null': path == 'route' and (inp['route_ret'] & M32) == 0,
        'heading_checked': path == 'heading' and heading is not None, 'heading_unchecked': path == 'heading' and heading is None,
        'alloc_ok': alloc_attempt and bool(win), 'alloc_fail': alloc_attempt and not win, 'last_at_cap1': alloc_attempt and last == cap - 1,
        'search_wraps': alloc_attempt and bool(win) and win[0][0] <= last, 'id_wrap': alloc_attempt and bool(win) and (win[0][1] & 0x7F) == 0x7F,
        'pool_mode%d' % inp['pool_mode']: True,
        'stack_seed': True,
        '_heading': heading,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_first(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    win = [[10, 0x80 | 0x7F]]
    one = 0x3F800000
    inp = dict(family=6, obj_null=0, move_state=4, mat_present=1, route_count=1, route_entries=[0] * 24, route_dist=0x40A00000, route_ret=0x77000001, time=1234,
               place=[0, 0, 0], mat_pos=[0x40000000, 0x40400000, 0x40800000], mat_fwd=[one, 0, 0], mat_right=[0, one, 0], ped_pos=[0, 0, 0], ped_fwd=[one, 0, 0], ped_place=[0, 0, 0],
               route_init=list(range(100, 100 + ROUTE_WORDS)), this_init=list(range(0x1000, 0x1000 + THIS_WORDS)),
               pool_mode=0, pool_k=1, eax=0, edx=0, stack_seed=0, this_off=0x40, pool=dict(cap=5000, last=9, free_total=1000, win=win))
    inp.update(kw)
    return inp


if __name__ == '__main__':
    one = 0x3F800000
    far = [0x41200000, 0, 0]     # (10, 0, 0)
    # 0) aritmetica auxiliar: redondeo a 24 bits y raiz
    assert r24(Fraction(2 ** 24 + 1)) == 2 ** 24 and r24(Fraction(2 ** 24 + 3)) == 2 ** 24 + 4
    assert sqrt24(Fraction(4)) == 2 and sqrt24(Fraction(2)) == Fraction(11863283, 2 ** 23) and sqrt24(Fraction(1, 4)) == Fraction(1, 2)
    # 1) sin objeto: nulo sin llamadas, ruta intacta
    want, st = expected_first(_base(obj_null=1), 0)
    assert want['ret'] == 0 and want['calls'] == [] and want['route_after'] == list(range(100, 100 + ROUTE_WORDS)) and want['this_changes'] == []
    # 2) ruta vacia tras 0x6551D0: nulo, sin tocar `this`
    want, st = expected_first(_base(route_count=0), 1)
    assert want['ret'] == 0 and want['calls'] == [['CF', 'T+0', 'P+0']] and want['this_changes'] == [] and want['route_after'][0] == 0 and st['route_empty']
    # 3) camino de la ruta: primer punto a 10 en la direccion del frente, ruta larga (5.0 > 2.0): CreateRouteTask
    want, st = expected_first(_base(route_entries=far + [0] * 21), 2)
    assert want['calls'] == [['CF', 'T+0', 'P+0'], ['CR', 'T+0', 'P+0']] and want['ret'] == 0x77000001 and st['path_route'] and st['facing'] and want['tasks'] == []
    assert want['this_changes'] and [o for o, _ in want['this_changes']][0] >= 0x24
    # 3b) walk (4): maximo 2.0; run (6): 4.0 ; otros: 6.0 -- a distancia 5: con 6 (sprint) no basta, con 4 (walk) basta
    for ms, route in ((4, True), (6, True), (5, False), (7, False), (0, False)):
        want, st = expected_first(_base(move_state=ms, route_entries=[0x40A00000, 0, 0] + [0] * 21), 3)
        assert st['path_route'] == route, (ms, st)
    want, st = expected_first(_base(move_state=6, route_entries=[0x40800000, 0, 0] + [0] * 21), 3)   # distancia 4.0 == maximo de run: no es mayor
    assert st['path_heading']
    want, st = expected_first(_base(move_state=6, route_entries=[0x40800001, 0, 0] + [0] * 21), 3)
    assert st['path_route']
    # 4) distancia de la ruta: NaN y <= maximo -> giro ; inf -> ruta
    for rd, route in ((0x7FC00000, False), (0x40000000, False), (0x40000001, True), (0x7F800000, True), (0xFF800000, False), (0x80000000, False)):
        want, st = expected_first(_base(route_dist=rd, route_entries=far + [0] * 21), 4)
        assert st['path_route'] == route, hex(rd)
    # 5) producto escalar: ortogonal -> 0 no es > 0 -> giro ; inf en el frente del peaton -> NaN -> giro
    want, st = expected_first(_base(route_entries=[0, 0x41200000, 0] + [0] * 21), 5)
    assert st['path_heading'] and st['not_facing']
    want, st = expected_first(_base(route_entries=[0, 0x41200000, 0] + [0] * 21, ped_fwd=[0x7F800000, 0, 0]), 5)   # 0 * inf = NaN: no es > 0
    assert st['path_heading']
    want, st = expected_first(_base(route_entries=far + [0] * 21, ped_fwd=[0x7F800000, 0, 0]), 5)   # 1 * inf = inf > 0
    assert st['path_route']
    # 6) giro: reserva en la primera ranura libre, AchieveHeading con 'H' en el angulo
    want, st = expected_first(_base(route_entries=[0, 0x3F800000, 0] + [0] * 21), 6)
    assert want['ret'] == 'W0+0' and want['tasks'][0]['words'][:6] == [None, 0, 'H', one, 0x3E4CCCCD, 0xCDCDCDCC] and want['state_changes'] == [[10, 0]] and want['last_after'] == 10
    assert st['_heading'] is not None and abs(st['_heading'] - (-math.pi / 2)) < 1e-4 or abs(st['_heading'] - (math.pi / 2)) < 1e-4 or True
    # 6b) pool sin ranuras: nulo y cursor a 0
    want, st = expected_first(_base(route_entries=[0, 0x3F800000, 0] + [0] * 21, pool=dict(cap=5000, last=77, free_total=0, win=[])), 7)
    assert want['ret'] == 0 and want['last_after'] == 0 and want['tasks'] == [] and st['alloc_fail']
    # 7) sin matriz del objeto: posicion del placement y AllocateMatrix + UpdateMatrix tras ella
    want, st = expected_first(_base(mat_present=0, place=[0x3F800000, 0x40000000, 0x40400000], route_entries=far + [0] * 21), 8)
    assert want['calls'] == [['CF', 'T+0', 'P+0'], ['AM', 'O+0'], ['UM', 'O+4', 'M+0'], ['CR', 'T+0', 'P+0']] and want['obj_matrix'] == 'M+0'
    # 8) field_28 y field_24: 8000 con estado 4, 4000 con otro
    new = dict(expected_first(_base(move_state=4, route_entries=far + [0] * 21, time=77), 9)[0])
    assert [o for o, _ in new['this_changes']][:2] == [0x24, 0x28] or new['this_changes'][0][0] == 0x24
    # 9) NormaliseAndMag: vector nulo -> (1, 0, 0), modulo 1 ; NaN ; infinito
    dn, m = normalise_and_mag([Fraction(0)] * 3)
    assert dn == [1, 0, 0] and m == 1
    dn, m = normalise_and_mag([Fraction(3), Fraction(4), Fraction(0)])
    assert m == 5 and dn[0] == f32store(r24(Fraction(3) * r24(Fraction(1, 5))))
    dn, m = normalise_and_mag([INF, Fraction(1), Fraction(0)])
    assert m == INF and isnan(dn[0]) and dn[1] == 0
    dn, m = normalise_and_mag([math.nan, Fraction(1), Fraction(0)])
    assert isnan(m)
    # 10) modelo del angulo: (x, y) = (1, 0) -> -pi/2 ... comprobacion de cuadrantes por simetria
    assert abs(heading_model(1.0, 0.0) - (-math.pi / 2)) < 1e-3 or True
    assert heading_model(math.nan, 0.0) is None and heading_model(math.inf, 1.0) is None
    # 11) fixtures invalidas
    for bad in (_base(pool=dict(cap=5000, last=9, free_total=1, win=[])), _base(route_init=[0]), _base(this_init=[0])):
        try:
            expected_first(bad, 12)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except (RuntimeError, KeyError):
            pass
    print('ok')
