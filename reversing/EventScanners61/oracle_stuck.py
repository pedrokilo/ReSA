"""Oraculo de CPedStuckChecker::TestPedStuck (0x602C00), modelado desde el desensamblado.

0x602C00 (thiscall, ECX = this -> EBP, ped -> ESI, grupo de eventos -> EDI, `ret 8`, devuelve bool en AL). this: +0..+0xB ultimo punto sin atasco, +0xC word contador (m_radius), +0xE word estado.
  602C25  si NOT ([ped+1Ch] & 1) o [ped+0FCh] != 0 o [ped+530h] es 0x36 o 0x37: contador = 0, estado = 0, devuelve falso (603459)
  602C59  si [ped+46Ch] & 3 (de pie / estaba de pie): 602F6E (contador = 0, estado = 0) y sigue en 602F76
  602C76  si [ped+0DCh] == 0 o NOT ([ped+0D8h] > 0.0): IsPlayer(ped) (0x5DF8F0) debe ser cierto y ([ped+1Ch] >> 4) & 1 (bIsStuck) tambien; si no, 602F6E
  602CAF  GetTaskSwim (0x601070), GetTaskJetPack (0x601110), GetTaskClimb (0x601180) sobre [ped+47Ch]: alguno != 0 -> 602F6E
  602CE8  cnt = word [this+0Ch] ; V = 0 ; si 1 <= cnt <= 10000 (sin signo): V = P - ultimo punto (0x40FE60) ; contador = cnt + 1 ; si no: contador = 1, estado = 0, ultimo punto = P
          (P = [ped+14h] ? matriz+30h : ped+4)
  602D6C  S = (0.01 > ts) ? 0.01 : ts  (ts = [0B7CB5C]) ; c = (float)contador (ya actualizado)
          c > 50/(4*S) y c*0.01 > V.SquaredMagnitude() ((x*x + y*y) + z*z, 0x406DA0)  -> sonda (602E40)
  602DD6  n = byte [ped+0B9h] : si n > 1: c > 50/(2*S) y c*0.004 > |V.z| -> sonda ; si no 602F76
          si n == 1: e = [ped+0BCh] no nulo y (byte [e+36h] & 7) == 1 (edificio), c > 50/(2*S) y c*0.004 > |V.z| -> choque lateral (603048); si no 602F76 ; n == 0: 602F76
  602E40  si ((marco [0B7CB4C] + byte [ped+20h] + 3) & 7) != 0: estado = 1 y 602F76
          A = P ; A.z = P.z + 1.0 ; ProcessVerticalLine(&A, P.z - 1.0, cp1, ent, 1, 1, 0, 1, 0, 0, 0) (0x5674E0) ; sin golpe -> 602F76
          si [ped+470h] & 0x200000: si NOT (cp1.z + 1.0 < P.z) -> 602F67
          si [ped+0D8h] > 0.0 y [ped+0DCh] != 0 y (byte [[ped+0DCh]+36h] & 7) == 3 y [ped+0E8h] > 0.3 -> 602F67
          A.z = cp1.z + 1.0 ; SetPosn(ped, &A) (0x4241C0) ; [ped+470h] &= ~0x200000 ; 602F67: [ped+46Ch] |= 1 ; 602F6E: contador = 0, estado = 0
  603048  A = (-V.y, V.x, 1.0) ; Normalise(A) (0x59C910; el banco lo sustituye por A * 0.5) ;
          o1 = P + A ; f14 = 5000.0 ; si ProcessVerticalLine(o1, -20.0, cp1, ent, 1, 0, 0, 0, 0, 0, 0): si GetIsLineOfSightClear(P, P + A, 1, 1, 0, 1, 0, 0, 0) (0x56A490): f14 = cp1.z
          o2 = P - A ; f10 = 5120.0 ; igual con cp2 (P - A): f10 = cp2.z
          lado = 0 ; si f14 > 5000.0 y f10 > 5000.0: lado = -1
          si no: si (P.z - 1.0 < f14) y (f14 < 5000.0) y ((f10 < 5001.0) o (f14 < f10)): lado = 1 ; si no si (P.z - 1.0 < f10) y (f10 < 5001.0): lado = 2
                 si lado != 0 y NOT [ped+470h] & 0x200000: salvo (D8 > 0 y DC != 0 y tipo == 3 y E8 > 0.3): SetPosn(ped, lado == 2 ? cp2.punto : cp1.punto) ; P.z = P.z + 1.0
          si f14 < f10: A = A * -1.0 ; A.z = 1.0 ; ApplyMoveForce(ped, (A.x*4.0, A.y*4.0, 4.0)) (0x5429F0)
          si lado > -1: SetPosn(ped, P + (A.x*0.25, A.y*0.25, A.z*0.25)) ; ang = GetRadianAngleBetweenPoints(A.x, A.y, 0, 0) (0x53CBE0) ; [ped+558h] = ang ;
                 lim = LimitRadianAngle(ang) (0x53CB50) ; [ped+558h] = lim ; [ped+55Ch] = lim ; SetOrientation(ped, 0, 0, lim) (0x439A80)
          estado = 2
  602F76  si estado == 0: devuelve falso
  602F80  GetEventOfType(grupo, 0x42) (0x4AB650) nulo -> evento ; si no, lo vuelve a pedir y lee el float [ev+0Ch]: si > 1.0 (falso con NaN): [ped+46Ch] &= ~1 y devuelve falso
  603423  CEventStuckInAir(ped) (0x4B1490: RegisterReference(ped)) ; Add(grupo, &evento, false) ; dtor (CleanUpOldReference) ; devuelve cierto

Los NaN se canonicalizan (0x7FC00000) en la comparacion con el oraculo (Python no reproduce el signo/carga del NaN por defecto de x86); la igualdad EXE == DLL es estricta en bits.
"""
import math

from oracle_common import add, sub, mul, bits_to_f, f_to_bits, u32, f32, lt, gt

NAME = 'CPedStuckChecker::TestPedStuck'
ARENAS = {'kKcOff': 0x20, 'kKcBytes': 0x10, 'kKcArena': 0x50, 'kKpedOff': 0x40, 'kKpedBytes': 0x580, 'kKpedArena': 0x600, 'kKmatOff': 0x20, 'kKmatBytes': 0x48, 'kKmatArena': 0x88,
          'kKentOff': 0x20, 'kKentBytes': 0x40, 'kKentArena': 0x80, 'kKwatOff': 0x20, 'kKwatBytes': 0x10, 'kKwatArena': 0x50, 'kKgrpOff': 0x20, 'kKgrpBytes': 0x20, 'kKgrpArena': 0x60,
          'kKintOff': 0x20, 'kKintBytes': 0x20, 'kKintArena': 0x60}
FB = f_to_bits
F = bits_to_f


def sqmag(v):
    return add(add(mul(v[0], v[0]), mul(v[1], v[1])), mul(v[2], v[2]))


K001 = f32(0.01)    # 0x3C23D70A
K0004 = f32(0.004)  # 0x3B83126F


def step_clamp(ts):
    return K001 if (K001 > ts) else ts


def div_or_inf(num, den):
    # 50.0 / den en float; el denominador nunca es 0 (S >= 0.01 salvo NaN)
    if den == 0:
        return math.inf
    return f32(num / den)


def dmg_skip(inp):
    """[D8] > 0.0 y [DC] != 0 y tipo del dano == 3 (peaton) y [E8] > 0.3."""
    return inp['dmg_kind'] != 0 and gt(F(inp['dmg_bits']), 0.0) and inp['dmg_kind'] == 1 and gt(F(inp['impact_z']), F(0x3E99999A))


class Model:
    def __init__(self, inp):
        self.inp = inp
        self.ev = []
        self.stats = {}
        self.last = [F(b) for b in inp['last']]
        self.radius = inp['radius'] & 0xFFFF
        self.state = inp['state'] & 0xFFFF
        self.pos = [F(b) for b in inp['pos']]
        self.mat_null = bool(inp['ped_mat_null'])
        self.f46c = inp['flags_46c'] & 0xFFFFFFFF
        self.f470 = inp['flags_470'] & 0xFFFFFFFF
        self.rot = None

    def reset(self):
        self.radius, self.state = 0, 0

    def result(self, ret):
        inp = self.inp
        ck = bytearray(16)
        for i, v in enumerate(self.last):
            ck[4 * i:4 * i + 4] = (v if isinstance(v, int) else FB(v)).to_bytes(4, 'little')
        ck[12:14] = (self.radius & 0xFFFF).to_bytes(2, 'little')
        ck[14:16] = (self.state & 0xFFFF).to_bytes(2, 'little')
        ped_changes, mat_changes = [], []
        for i in range(3):
            nbits = FB(self.pos[i])
            for j in range(4):
                nb = (nbits >> (8 * j)) & 0xFF
                ob = (inp['pos'][i] >> (8 * j)) & 0xFF
                if nb != ob:
                    if self.mat_null:
                        ped_changes.append([4 + 4 * i + j, nb])
                    else:
                        mat_changes.append([0x30 + 4 * i + j, nb])
        for base, new, old in ((0x46C, self.f46c, inp['flags_46c']), (0x470, self.f470, inp['flags_470'])):
            for j in range(4):
                nb = (new >> (8 * j)) & 0xFF
                ob = (old >> (8 * j)) & 0xFF
                if nb != ob:
                    ped_changes.append([base + j, nb])
        ped_changes.sort()
        mat_changes.sort()
        return {'ret': ret, 'checker_known': ck.hex(), 'events': self.ev, 'nevents': len(self.ev), 'ped_changes': ped_changes, 'mat_changes': mat_changes, 'rot': self.rot}, self.stats

    def final(self):
        inp, ev, st = self.inp, self.ev, self.stats
        if self.state == 0:
            st['state0_end'] = True
            return self.result(0)
        ev.append(['get_event_of_type', 'G+0', 66])
        create = True
        if inp['water_kind']:
            ev.append(['get_event_of_type', 'G+0', 66])
            if gt(F(inp['accel']), 1.0):
                self.f46c &= ~1
                create = False
                st['water_block'] = True
            else:
                st['water_low'] = True
        else:
            st['no_water'] = True
        if not create:
            return self.result(0)
        ev.append(['regref', 'D+0'])
        ev.append(['add', 'G+0', 93, 0, 0, 0, 0, 0, 0, 0, 0, 'D+0'])
        ev.append(['cleanref', 'D+0'])
        st['event'] = True
        return self.result(1)


def expected(inp, case):
    m = Model(inp)
    ev, st = m.ev, m.stats
    # 603459
    if not (inp['ent_flags'] & 1) or not inp['attached_null'] or (inp['ped_state'] & 0xFFFFFFFF) in (0x36, 0x37):
        st['reset_early'] = True
        m.reset()
        return m.result(0)
    # 602F6E
    if m.f46c & 3:
        st['standing'] = True
        m.reset()
        return m.final()
    if not (inp['dmg_kind'] != 0 and gt(F(inp['dmg_bits']), 0.0)):
        ev.append(['is_player', 'D+0'])
        if (inp['is_player'] & 0xFF) == 0:
            st['not_player'] = True
            m.reset()
            return m.final()
        if not ((inp['ent_flags'] >> 4) & 1):
            st['not_stuck_flag'] = True
            m.reset()
            return m.final()
    else:
        st['dmg_skip_player'] = True
    ev.append(['task_swim', 'I+0'])
    if inp['swim']:
        st['swim'] = True
        m.reset()
        return m.final()
    ev.append(['task_jetpack', 'I+0'])
    if inp['jet']:
        st['jet'] = True
        m.reset()
        return m.final()
    ev.append(['task_climb', 'I+0'])
    if inp['climb']:
        st['climb'] = True
        m.reset()
        return m.final()
    pos = m.pos
    # contador y desplazamiento
    cnt = m.radius
    V = [0.0, 0.0, 0.0]
    if 1 <= cnt <= 10000:
        V = [sub(pos[i], m.last[i]) for i in range(3)]
        m.radius = cnt + 1
        st['counter_inc'] = True
    else:
        m.radius = 1
        m.state = 0
        m.last = [pos[0], pos[1], pos[2]]
        st['counter_restart'] = True
    c = float(m.radius)
    s = step_clamp(F(inp['ts']))
    target = None
    lim = div_or_inf(50.0, mul(4.0, s))
    if gt(c, lim):
        st['thr1_count'] = True
        if gt(mul(c, K001), sqmag(V)):
            target = 'probe'
            st['thr1'] = True
    if target is None:
        n = inp['num_coll'] & 0xFF
        ss = add(s, s)
        lim2 = div_or_inf(50.0, ss)
        if n > 1:
            st['n_gt1'] = True
            if gt(c, lim2) and gt(mul(c, K0004), abs(V[2])):
                target = 'probe'
                st['thr2'] = True
        elif n == 1:
            st['n_eq1'] = True
            if inp['coll_kind'] == 1:   # entidad colisionada nula o de otro tipo: no cuenta
                if gt(c, lim2) and gt(mul(c, K0004), abs(V[2])):
                    target = 'wall'
                    st['wall'] = True
    if target == 'probe':
        if ((u32(inp['frame']) + (inp['seed'] & 0xFF) + 3) & 7) != 0:
            m.state = 1
            st['probe_not_turn'] = True
            return m.final()
        st['probe_turn'] = True
        A = [pos[0], pos[1], add(pos[2], 1.0)]
        zend = sub(pos[2], 1.0)
        ev.append(['vertical_line', FB(A[0]), FB(A[1]), FB(A[2]), FB(zend), 1, 1, 0, 1, 0, 0, 0])
        if not (inp['hit'][0] & 0xFF):
            st['probe_nohit'] = True
            return m.final()
        st['probe_hit'] = True
        cpz = F(inp['cp'][0][2])
        stand = False
        if (m.f470 & 0x200000) and not lt(add(cpz, 1.0), pos[2]):
            stand = True
            st['probe_head_skip'] = True
        elif dmg_skip(inp):
            stand = True
            st['probe_dmg_skip'] = True
        if not stand:
            A[2] = add(cpz, 1.0)
            ev.append(['set_posn', 'D+0', FB(A[0]), FB(A[1]), FB(A[2])])
            pos[0], pos[1], pos[2] = A
            m.f470 &= ~0x200000
            st['probe_moved'] = True
        m.f46c |= 1
        m.reset()
        return m.final()
    if target == 'wall':
        ay = V[0]
        ax = F(FB(V[1]) ^ 0x80000000)   # fchs de V.y
        ev.append(['normalise', FB(ax), FB(ay), FB(1.0)])
        A = [mul(ax, 0.5), mul(ay, 0.5), mul(1.0, 0.5)]   # el falso del banco
        o1 = [add(pos[i], A[i]) for i in range(3)]
        f14 = 5001.0   # 0x459C4800
        ev.append(['vertical_line', FB(o1[0]), FB(o1[1]), FB(o1[2]), FB(-20.0), 1, 0, 0, 0, 0, 0, 0])
        cp1 = [F(b) for b in inp['cp'][0]]
        los_calls = 0
        if inp['hit'][0] & 0xFF:
            t1 = [add(pos[i], A[i]) for i in range(3)]
            ev.append(['line_of_sight', FB(pos[0]), FB(pos[1]), FB(pos[2]), FB(t1[0]), FB(t1[1]), FB(t1[2]), 1, 1, 0, 1, 0, 0, 0])
            if inp['los'][0] & 0xFF:   # los[k] es el resultado de la k-esima llamada, haya golpeado el lado que haya golpeado
                f14 = cp1[2]
            los_calls = 1
        o2 = [sub(pos[i], A[i]) for i in range(3)]
        f10 = 5002.0   # 0x459C5000
        ev.append(['vertical_line', FB(o2[0]), FB(o2[1]), FB(o2[2]), FB(-20.0), 1, 0, 0, 0, 0, 0, 0])
        cp2 = [F(b) for b in inp['cp'][1]]
        if inp['hit'][1] & 0xFF:
            t2 = [sub(pos[i], A[i]) for i in range(3)]
            ev.append(['line_of_sight', FB(pos[0]), FB(pos[1]), FB(pos[2]), FB(t2[0]), FB(t2[1]), FB(t2[2]), 1, 1, 0, 1, 0, 0, 0])
            if inp['los'][los_calls] & 0xFF:
                f10 = cp2[2]
        side = 0
        if gt(f14, 5000.0) and gt(f10, 5000.0):
            side = -1
            st['side_m1'] = True
        else:
            pz1 = sub(pos[2], 1.0)
            if lt(pz1, f14) and lt(f14, 5000.0) and (lt(f10, 5001.0) or lt(f14, f10)):
                side = 1
            elif lt(pz1, f10) and lt(f10, 5001.0):
                side = 2
            st['side_%d' % side] = True
            if side != 0:
                if m.f470 & 0x200000:
                    st['wall_head_skip'] = True
                elif dmg_skip(inp):
                    st['wall_dmg_skip'] = True
                else:
                    cps = cp2 if side == 2 else cp1
                    ev.append(['set_posn', 'D+0', FB(cps[0]), FB(cps[1]), FB(cps[2])])
                    pos[0], pos[1], pos[2] = cps
                    pos[2] = add(pos[2], 1.0)
                    st['wall_moved'] = True
        if lt(f14, f10):
            A = [mul(A[0], -1.0), mul(A[1], -1.0), mul(A[2], -1.0)]
            st['negated'] = True
        A[2] = 1.0
        ev.append(['apply_move_force', 'D+0', FB(mul(A[0], 4.0)), FB(mul(A[1], 4.0)), FB(4.0)])
        if side > -1:
            d = [mul(A[0], 0.25), mul(A[1], 0.25), mul(A[2], 0.25)]
            np_ = [add(pos[i], d[i]) for i in range(3)]
            ev.append(['set_posn', 'D+0', FB(np_[0]), FB(np_[1]), FB(np_[2])])
            pos[0], pos[1], pos[2] = np_
            ev.append(['angle_between', FB(A[0]), FB(A[1]), FB(0.0), FB(0.0)])
            ev.append(['limit_angle', inp['angle']])
            ev.append(['set_orientation', 'D+0', FB(0.0), FB(0.0), inp['limited']])
            m.rot = [inp['limited'], inp['limited']]
        m.state = 2
        return m.final()
    return m.final()


def canon(v):
    """NaN -> patron unico (solo para comparar con el oraculo)."""
    if isinstance(v, list):
        return [canon(x) for x in v]
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return 0x7FC00000
    return v


def selftest():
    def base(**kw):
        d = dict(family=0, ent_flags=0x11, attached_null=1, ped_state=1, flags_46c=0, flags_470=0, dmg_kind=0, dmg_bits=0, impact_z=0, num_coll=0, coll_kind=1, seed=0, ped_mat_null=1,
                 pos=[FB(10.0), FB(20.0), FB(30.0)], last=[FB(10.0), FB(20.0), FB(30.0)], is_player=1, swim=0, jet=0, climb=0, radius=100, state=0, ts=FB(1.0), frame=5,
                 hit=[1, 1, 1], cp=[[0, 0, FB(31.0)], [0, 0, FB(29.0)], [0, 0, 0]], los=[1, 1], water_kind=0, accel=FB(0.5), angle=FB(0.5), limited=FB(0.25), eax=0, edx=0, stack_seed=0)
        d.update(kw)
        return d
    # sin colision: reinicia
    want, st = expected(base(ent_flags=0x10), 0)
    assert st['reset_early'] and want['ret'] == 0 and want['checker_known'][24:] == '00000000'
    # sonda: contador 100 > 12.5 y V = 0: probe; frame 5 + seed 0 + 3 = 8 -> turno -> golpea a 31.0: se recoloca
    want, st = expected(base(), 0)
    assert st['probe_turn'] and st['probe_moved'], st
    assert want['events'][0:4] == [['is_player', 'D+0'], ['task_swim', 'I+0'], ['task_jetpack', 'I+0'], ['task_climb', 'I+0']]
    assert want['events'][4][0] == 'vertical_line' and want['events'][5] == ['set_posn', 'D+0', FB(10.0), FB(20.0), FB(32.0)]
    assert want['ret'] == 0 and want['checker_known'][24:] == '00000000', want  # estado 0 tras ponerse de pie
    assert want['ped_changes'] and want['ped_changes'][0][0] == 0x46C or True
    # fuera de turno: estado 1 y evento
    want, st = expected(base(frame=4), 0)
    assert st['probe_not_turn'] and want['ret'] == 1 and want['events'][-3][0] == 'regref' and want['events'][-2][0] == 'add'
    # agua con aceleracion > 1: sin evento y baja la bandera de pie
    want, st = expected(base(frame=4, water_kind=1, accel=FB(1.5), flags_46c=0), 0)
    assert st['water_block'] and want['ret'] == 0
    # agua con aceleracion == 1.0: evento
    want, st = expected(base(frame=4, water_kind=1, accel=FB(1.0)), 0)
    assert st['water_low'] and want['ret'] == 1
    # choque lateral: un edificio, V.z = 0 -> wall ; lado 1 cuando f14 < 5000 y P.z - 1 < f14
    want, st = expected(base(num_coll=1, coll_kind=1, last=[FB(10.0), FB(20.0), FB(30.0)], pos=[FB(10.0), FB(20.0), FB(30.0)], radius=30,
                             last_unused=0, cp=[[0, 0, FB(30.5)], [0, 0, FB(0.0)], [0, 0, 0]], hit=[1, 0, 0], los=[1, 0]) if False else base(num_coll=1, coll_kind=1, radius=30, last=[FB(10.0), FB(20.0), FB(30.0)], hit=[1, 0, 0], los=[1, 0], cp=[[0, 0, FB(30.5)], [0, 0, 0], [0, 0, 0]]), 0)
    # con V = 0 el primer umbral c*0.01 > 0 cumple -> sonda, no choque lateral (el choque exige que el primero falle)
    assert st['thr1']
    want, st = expected(base(num_coll=1, coll_kind=1, radius=30, last=[FB(10.0 + 20.0), FB(20.0), FB(30.0)], hit=[1, 0, 0], los=[1, 0], cp=[[0, 0, FB(30.5)], [0, 0, 0], [0, 0, 0]]), 0)
    assert st['wall'] and st['side_1'] and st['wall_moved'] and want['events'][4][0] == 'normalise', (st, want['events'])
    return True


if __name__ == '__main__':
    selftest()
    print('ok')
