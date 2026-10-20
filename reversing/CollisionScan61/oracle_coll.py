"""Oraculo de CCollisionEventScanner::ScanForCollisionEvents (0x604500), modelado desde el desensamblado del EXE.

0x604500 (thiscall, ECX = escaner -> [esp+20h], peaton (victima) -> ESI, grupo de eventos en [esp+138h], entidad danadora [esi+0DCh] -> EDI, `ret 8`, void).
  604523  si [ped+0DCh] == 0 o [ped+0D8h] == 0.0 (fcomp + test ah,44h + jnp: solo el igual salta; con NaN sigue): 6052C4
  604557  tarea = GetSimplestActiveTask(intel+4) (0x6819D0) ; si no es nula e IsGoToTask(tarea) (0x61A360, AL): ms = [tarea+8] ; si no, ms = 1   (EBP)
  604581  switch ([edi+36h] & 7) - 1 (tabla 0x6052E4: 1 edificio 605102, 2 vehiculo 60459D, 3 peaton 604B56, 4 objeto 604F70; otros: 60515F)
  Edificio (605102): CEventBuildingCollision(word [ped+0F8h], [ped+0D8h], edi, ped+0E0h, ped+0ECh, ms) (0x4ACF00) ; Add(grupo, &ev, 0) ; dtor 4ACFF0 ; 60515E
  Vehiculo (60459D): si byte [escaner] != 0: 60515E
      v = SquaredMagnitude(edi+44h) = (x*x + y*y) + z*z (0x406DA0) ; umbral = [86C920/86C924] = 0x3B23D70B (con y sin [edi+4C4h])
      si [edi+594h] == 6 y [edi+40h] & 4 y ([ped+1Ch] >> 4) & 1 y [ped+46Ch] & 1 y [ped+568h] == 0 y v > [858FC4] (1e-4): KillPedWithCar(ped, edi, 15.0, 0) (0x5F0360)
      si v > umbral (fcomp + test ah,41h + jne: con NaN no):
         d = [ped+0D8h] ; si [ped+46Ch] & 1: dot = E4*[ped+554h] + E0*[ped+550h] ; si dot < 0.0: val = dot*[ped+8Ch] + d ; d = (0.0 > val) ? 0.0 : val
         IsPlayer(ped) (0x5DF8F0): si:  d > 20.0: d = 20.0
            heading = GetHeading(edi) (0x441DB0) ; GetColModel(edi) x2 (0x535300): min = [col+0..8], max = [col+0Ch..14h]
            suma = (min.x+max.x, min.y+max.y, max.z+min.z) ; mitad = suma * (1.0/2.0) (0x4119D0) ; centro = M*mitad (0x59C890: x' = ((up.x*z + fwd.x*y) + right.x*x) + pos.x ;
              y' = ((up.y*z + right.y*x) + fwd.y*y) + pos.y ; z' = ((up.z*z + right.z*x) + fwd.z*y) + pos.z ; M = [edi+14h] con right +0, fwd +10h, up +20h, pos +30h)
            P = ped pos ; d1 = centro - P (0x40FE60) ; rel = LimitRadianAngle(heading - atan2(-d1.x, d1.y)) (0x53CB50; fpatan sin redondear y un solo redondeo)
            caja = atan2(max.x - min.x, max.y - min.y) ; V = P - pos del vehiculo, Normalise(V) (0x59C910)
            si |rel| < caja: v2 = dot(V, vel) ; si no, si |rel| > pi - caja (0x858CB8): v2 = dot(V, vel) ; si no, si rel > 0.0: v2 = dot(-1.0*right, vel) ; si no: v2 = dot(right, vel)
               (dot(V, vel) = (V.y*vel.y + V.z*vel.z) + V.x*vel.x ; dot(a, b) = (a.z*b.z + a.y*b.y) + a.x*b.x ; la rama rel <= 0 calcula ademas dot(right, adelante del peaton) sin efecto)
            si v2 > 0.1 (858B1C): KillPedWithCar(ped, edi, d, 0)
         si no es jugador: si [ped+0FCh] y ([[ped+0FCh]+36h] & 7) == 2: bloque de dano por vehiculo ; si no KillPedWithCar(ped, edi, [ped+0D8h], 0)
      si no (v <= umbral o NaN): si [ped+0FCh] y tipo 2: bloque de dano por vehiculo ; si no, si IsPlayer(ped): fin (60515E) ; si no:
         m = GetMoveStateFromGoToTask([ped+47Ch]) ; CEventVehicleCollision(word [ped+0F8h], [ped+0D8h], edi, ped+0E0h, ped+0ECh, m, 0) (0x4AC840) ; Add ; dtor 4AC920
      bloque de dano por vehiculo: lado = ComputePedHitSide(ped, edi) (0x5F3640) ; CPedDamageResponseCalculator(edi, [ped+0D8h], 0x31, 3, 0) (0x4AD3F0) ;
         CEventDamage(edi, hora, 0x31, 3, lado, 0, bit 8 de [ped+46Ch]) (0x4AD830) ; si AffectsPed(ped) (0x4B35A0): ComputeDamageResponse(ped, ev+38h, 1) (0x4B5AC0) ; dtors
  Peaton (604B56): m1 = GetMoveStateFromGoToTask([edi+47Ch]) ; IsPlayer(ped): si (victima jugadora): CEventPlayerCollisionWithPed(word, [ped+0D8h], edi, ped+0E0h, ped+0ECh, ms, m1) (0x5FEE40) ; Add ;
        Say(edi, 0x1C, 0, 1.0, 0, 0, 0) (0x5EFFE0) ; dtor
      si no: IsPlayer(edi): si (danador jugador): CEventPedCollisionWithPlayer (0x5FED40) ; Add ; IncrementAngerAtPlayer([ped+47Ch], 1) (0x421050) ; Say(ped, 0x1C, ...) ;
           a = GetAcquaintances(ped+4E0h, 4) (0x608970) ; f = GetPedFlag([edi+598h]) (0x608830) ; si a & f: CEventAcquaintancePedHate(edi) (0x420E70) ; Add ; dtor
         si no: CEventPedCollisionWithPed (0x4AC990) ; Add
      604D3E: si [edi+0DCh] != 0: 60515E ; v = E0 * -1.0 (por componentes) ; IsPlayer(edi): CEventPlayerCollisionWithPed(word, D8, ped, v, ped+0ECh, ms, m1) ; Add([edi+47Ch]+68h) ;
         si no, IsPlayer(ped): CEventPedCollisionWithPlayer(...) ; Add([edi+47Ch]+68h) ; IncrementAngerAtPlayer([edi+47Ch], 1) ; a = GetAcquaintances(edi+4E0h, 4) ; f = GetPedFlag([ped+598h]) ;
              si a & f: CEventAcquaintancePedHate(ped) ; Add([edi+47Ch]+68h)
         si no: CEventPedCollisionWithPed(...) ; Add([edi+47Ch]+68h)
  Objeto (604F70): d = [ped+0D8h] ; est = [ped+46Ch] & 1 ; si est y NOT GetIsStatic(edi) (0x4633E0: [edi+1Ch] & 4 o & 40000h): dot como arriba ; si dot < 0.0: d = max0(dot*masa + d)
      limite = [ped+480h] ? 2.0 : 1.0 ; si d > limite y NOT estatico(edi) y est y edi != [ped+584h] y NOT ([edi+0FCh] != 0 y [edi+0FCh] == edi):
         d = d / limite * 10.0 ; dir = GetLocalDirection(ped, (-E0, -E4)) (0x5DEF60) ; GenerateDamageEvent(ped, [ped+0DCh], 0x36, (int)d (_ftol2, dword bajo), 3, dir) (0x73A530) ; [ped+128h] = [ped+0DCh]
      si no: CEventObjectCollision(word, [ped+0D8h], edi, ped+0E0h, ped+0ECh, ms) (0x4ACCF0) ; Add ; dtor 4ACDD0
  60515F  si [ped+480h] y tipo de [ped+0DCh] es 1 o 4 y GetIsWearingBalaclava([[ped+480h]+4]) (0x5A7950): e = [ped+0DCh]
      si GetIsTypePhysical(e) (tipo 2..4) y [e+40h] & 20h: si |[e+58h]| > 0.04 (8D23A4) y hora > [0C0B1B4] + 2000 (sin signo): nivel = 40.0 ; [0C0B1B4] = hora
      si no: si D8 > 1.0 y hora > [0C0B1B0] + 1000: nivel = (D8 > 3.0) ? 40.0 : 30.0 ; [0C0B1B0] = hora
      con nivel: CEventSoundQuiet(ped, nivel, -1, {0,0,0}) (0x5E05B0: hora actual y posicion del peaton) ; GetEventGlobalGroup() (0x4ABA50).Add(&ev, 0) ; dtor 5DEA00
  6052C4  byte [escaner] = 0
Eventos de pila: ctor -> RegisterReference(entidad en +14h) si no es nula (0x571B70) ; dtor -> CleanUpOldReference (0x571A00).
Volcado (DumpEvent de Core.hpp): ['add'|'affects_ped', grupo, tipo, prioridad, tiempo, valido, editable, anade, tarea, expresion, bool, extras...].
"""
import math
import struct

from oracle_common import add, sub, mul, bits_to_f, f_to_bits, f32, u32

NAME = 'CCollisionEventScanner::ScanForCollisionEvents'
ARENAS = {'kCsOff': 0x20, 'kCsBytes': 0x1, 'kCsArena': 0x41, 'kVOff': 0x40, 'kVBytes': 0x5A0, 'kVArena': 0x620, 'kVmOff': 0x20, 'kVmBytes': 0x48, 'kVmArena': 0x88,
          'kIOff': 0x40, 'kIBytes': 0x100, 'kIArena': 0x180, 'kTOff': 0x20, 'kTBytes': 0x20, 'kTArena': 0x60, 'kDOff': 0x40, 'kDBytes': 0x5A0, 'kDArena': 0x620,
          'kDmOff': 0x20, 'kDmBytes': 0x48, 'kDmArena': 0x88, 'kDiOff': 0x40, 'kDiBytes': 0x100, 'kDiArena': 0x180, 'kColOff': 0x20, 'kColBytes': 0x18, 'kColArena': 0x58,
          'kPdOff': 0x20, 'kPdBytes': 0x20, 'kPdArena': 0x60, 'kClOff': 0x20, 'kClBytes': 0x20, 'kClArena': 0x60, 'kAOff': 0x20, 'kABytes': 0x40, 'kAArena': 0x80,
          'kGOff': 0x20, 'kGBytes': 0x20, 'kGArena': 0x60}
FB = f_to_bits
F = bits_to_f
K_SPEED = bits_to_f(0x3B23D70B)       # 0x86C920 / 0x86C924
K_KILL = bits_to_f(0x38D1B717)        # 0x858FC4 (0.0001)
K_PI = bits_to_f(0x40490FDB)          # 0x858CB8
K_TENTH = bits_to_f(0x3DCCCCCD)       # 0x858B1C
K_TURN = bits_to_f(0x3D23D70A)        # 0x8D23A4 (0.04)
ONE = bits_to_f(0x3F800000)
MASK = 0xFFFFFFFF


def neg(x):
    return -x


def dot_abc(a, b):
    """0x40FDB0: (a.z*b.z + a.y*b.y) + a.x*b.x"""
    return add(add(mul(a[2], b[2]), mul(a[1], b[1])), mul(a[0], b[0]))


def ftol_low(x):
    """_ftol2: int64 truncado; NaN, infinitos y |x| >= 2^63 dan 0x8000000000000000 (dword bajo 0)."""
    if math.isnan(x) or math.isinf(x) or abs(x) >= 2.0 ** 63:
        return 0
    return int(x) & MASK


def canon(bits):
    return 0x7FC00000 if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) else bits


def add_ev(group, typ, prio, extras, editable=False):
    if editable:
        return ['add', group, typ, prio, 0, 0, 1, 1, 0xC8, 0xFFFF, 0, *extras]
    return ['add', group, typ, prio, 0, 0, 0, 0, 0, 0, 0, *extras]


def expected(inp, case):
    ev = []
    st = {}
    now = inp['now'] & MASK
    last_a = inp['last_a'] & MASK
    last_b = inp['last_b'] & MASK
    dmg_bits = inp['dmg_bits'] & MASK
    dmg = F(dmg_bits)
    ignored = None
    piece = inp['piece'] & 0xFFFF
    e0b = [x & MASK for x in inp['e0']]
    ecb = [x & MASK for x in inp['ec']]
    e0 = [F(x) for x in e0b]
    pos = [F(x) for x in inp['pos']]
    kind = (inp['kind'] & 7) if not inp['dmg_null'] else None
    victim_player = (inp['is_player_v'] & 0xFF) != 0
    damager_player = (inp['is_player_d'] & 0xFF) != 0

    def done():
        want = {'events': ev, 'nevents': len(ev), 'scanner': 0, 'ped_changes': [], 'ignored': ignored, 'last_a': last_a, 'last_b': last_b}
        return want, st

    if inp['dmg_null']:
        st['early_null'] = True
        return done()
    if dmg == 0.0:
        st['early_zero'] = True
        return done()
    if dmg != dmg:
        st['dmg_nan'] = True
    ev.append(['simplest', 'I+4'])
    ms = 1
    if not inp['task_null']:
        ev.append(['isgoto', 'T+0'])
        if (inp['is_goto'] & 0xFF) != 0:
            ms = inp['task_move'] & MASK
            st['goto'] = True
    st['task_none' if inp['task_null'] else 'task_other'] = True
    group = 'G+0'
    moves = [inp['move'][0] & MASK, inp['move'][1] & MASK]
    move_n = [0]

    def get_move(label):
        ev.append(['move', label])
        v = moves[move_n[0]] if move_n[0] < 2 else 0
        move_n[0] += 1
        return v

    def ref(label):
        ev.append(['regref', label])

    def unref(label):
        ev.append(['cleanref', label])

    st['kind%d' % kind] = True
    if kind == 1:
        st['building'] = True
        ref('D+0')
        ev.append(add_ev(group, 6, 59, [piece, ms & 0xFFFF, dmg_bits, 'D+0', *e0b, *ecb]))
        unref('D+0')
    elif kind == 2:
        vehicle(inp, ev, st, dict(ms=ms, now=now, dmg=dmg, dmg_bits=dmg_bits, piece=piece, e0=e0, e0b=e0b, ecb=ecb, pos=pos, victim_player=victim_player, group=group, get_move=get_move, ref=ref, unref=unref))
    elif kind == 3:
        ignored = ped_case(inp, ev, st, dict(ms=ms, piece=piece, dmg_bits=dmg_bits, e0b=e0b, ecb=ecb, e0=e0, victim_player=victim_player, damager_player=damager_player, group=group, get_move=get_move, ref=ref, unref=unref))
    elif kind == 4:
        ignored = object_case(inp, ev, st, dict(ms=ms, piece=piece, dmg=dmg, dmg_bits=dmg_bits, e0b=e0b, ecb=ecb, e0=e0, group=group, ref=ref, unref=unref))
    else:
        st['no_branch'] = True
    if kind == 2 and (inp['hit_by_car'] & 0xFF):
        st['hit_by_car_skip'] = True
    # 60515F: ruido del jugador con pasamontanas
    if inp['has_player']:
        st['tail_player'] = True
        hit_type = inp['kind'] & 7
        if hit_type in (1, 4):
            st['tail_type'] = True
            ev.append(['balaclava', 'CL+0'])
            if (inp['bala'] & 0xFF) != 0:
                st['tail_bala'] = True
                level = None
                physical = hit_type == 4 and (inp['d_phys'] & 0x20)
                if hit_type == 4 and not (inp['d_phys'] & 0x20):
                    st['tail_object_nophys'] = True
                if physical:
                    st['tail_phys'] = True
                    tz = abs(F(inp['d_turn_z']))
                    if tz > K_TURN:
                        st['tail_phys_fast'] = True
                        if now > ((last_b + 2000) & MASK):
                            level = 40.0
                            last_b = now
                            st['tail_phys_noise'] = True
                        else:
                            st['tail_phys_time_fail'] = True
                else:
                    st['tail_nonphys'] = True
                    if dmg > ONE:
                        st['tail_dmg_gt1'] = True
                        if now > ((last_a + 1000) & MASK):
                            level = 40.0 if dmg > 3.0 else 30.0
                            last_a = now
                            st['tail_nonphys_noise40' if level == 40.0 else 'tail_nonphys_noise30'] = True
                        else:
                            st['tail_nonphys_time_fail'] = True
                if level is not None:
                    st['tail_noise'] = True
                    ref('V+0')
                    ev.append(add_ev('GG+0', 62, 8, ['V+0', FB(level), now, *[x & MASK for x in inp['pos']]], editable=True))
                    unref('V+0')
    return done()


def damage_block(inp, ev, st, c):
    """Bloque de dano por vehiculo (peaton sujeto a un vehiculo): 604925 / 604A17."""
    ev.append(['hit_side', 'V+0', 'D+0'])
    c['ref']('D+0')
    ev.append(['affects_ped', 'V+0', 9, 65, 0, 0, 1, 1, 0xC8, 0xFFFF, 0, 'D+0', c['now'], 49, 3, inp['hit_side'] & 0xFF, 8, 0, 0xBF, 0x41000000, 0x3F800000, 0, 0, 0])
    if (inp['affects'] & 0xFF) != 0:
        st['damage_computed'] = True
        ev.append(['compute_damage', 'D+0', c['dmg_bits'], 3, 49, 0, 'V+0', 0x38, 1])
    else:
        st['damage_not_affected'] = True
    c['unref']('D+0')


def vehicle(inp, ev, st, c):
    st['vehicle'] = True
    if inp['hit_by_car'] & 0xFF:
        return
    sp = [F(x) for x in inp['d_speed']]
    sq = add(add(mul(sp[0], sp[0]), mul(sp[1], sp[1])), mul(sp[2], sp[2]))
    if inp['d_subtype'] == 6:
        st['train'] = True
        if (inp['d_phys'] & 4):
            st['train_phys'] = True
            if (inp['ent_flags'] >> 4) & 1:
                if inp['flags_46c'] & 1:
                    if inp['standing_on'] == 0:
                        st['train_cond'] = True
                        if sq > K_KILL:
                            st['train_kill'] = True
                            ev.append(['kill_ped_with_car', 'V+0', 'D+0', f_to_bits(f32(15.0)), 0])
    attached = bool(inp['attach_kind']) and (inp['attach_type'] & 7) == 2
    if sq > K_SPEED:
        st['v_high'] = True
        d = c['dmg']
        if inp['flags_46c'] & 1:
            dot = add(mul(e_y(c), F(inp['shift_y'])), mul(e_x(c), F(inp['shift_x'])))
            if dot < 0.0:
                st['v_dot_neg'] = True
                val = add(mul(dot, F(inp['mass'])), d)
                d = 0.0 if 0.0 > val else val
                if 0.0 > val:
                    st['v_clamp0'] = True
        ev.append(['is_player', 'V+0'])
        if c['victim_player']:
            st['v_high_player'] = True
            if d > 20.0:
                st['v_clamp20'] = True
                d = 20.0
            ev.append(['heading', 'D+0'])
            ev.append(['colmodel', 'D+0'])
            ev.append(['colmodel', 'D+0'])
            heading = F(inp['heading'])
            bmin = [F(x) for x in inp['box_min']]
            bmax = [F(x) for x in inp['box_max']]
            ssum = [add(bmin[0], bmax[0]), add(bmin[1], bmax[1]), add(bmax[2], bmin[2])]
            half = [mul(ssum[0], 0.5), mul(ssum[1], 0.5), mul(ssum[2], 0.5)]
            vm = [F(x) for x in inp['d_vm']]
            right, fwd, up = vm[0:3], vm[3:6], vm[6:9]
            vpos = [F(x) for x in inp['d_vpos']]
            cx = add(add(add(mul(up[0], half[2]), mul(fwd[0], half[1])), mul(right[0], half[0])), vpos[0])
            cy = add(add(add(mul(up[1], half[2]), mul(right[1], half[0])), mul(fwd[1], half[1])), vpos[1])
            cz = add(add(add(mul(up[2], half[2]), mul(right[2], half[0])), mul(fwd[2], half[1])), vpos[2])
            d1 = [sub(cx, c['pos'][0]), sub(cy, c['pos'][1])]
            a = math.atan2(-d1[0], d1[1])
            rel_in = f32(heading - a)
            ev.append(['limit_angle', f_to_bits(rel_in)])
            rel = F(inp['limited'])
            box = f32(math.atan2(sub(bmax[0], bmin[0]), sub(bmax[1], bmin[1])))
            away = [sub(c['pos'][i], vpos[i]) for i in range(3)]
            ev.append(['normalise', *[f_to_bits(x) for x in away]])
            away = [mul(x, 0.5) for x in away]
            ar = abs(rel)
            if ar < box:
                st['dot_front'] = True
                v2 = add(add(mul(away[1], sp[1]), mul(away[2], sp[2])), mul(away[0], sp[0]))
            elif ar > sub(K_PI, box):
                st['dot_back'] = True
                v2 = add(add(mul(away[1], sp[1]), mul(away[2], sp[2])), mul(away[0], sp[0]))
            elif rel > 0.0:
                st['side_pos'] = True
                v2 = dot_abc([mul(-1.0, x) for x in right], sp)
            else:
                st['side_neg'] = True
                if rel != rel:
                    st['side_nan'] = True
                v2 = dot_abc(right, sp)
            if v2 > K_TENTH:
                st['v_player_kill'] = True
                ev.append(['kill_ped_with_car', 'V+0', 'D+0', f_to_bits(d), 0])
        else:
            st['v_high_nonplayer'] = True
            if attached:
                st['v_high_attached'] = True
                damage_block(inp, ev, st, c)
            else:
                st['v_high_kill'] = True
                ev.append(['kill_ped_with_car', 'V+0', 'D+0', c['dmg_bits'], 0])
    else:
        st['v_low'] = True
        if sq != sq:
            st['v_low_nan'] = True
        if attached:
            st['v_low_attached'] = True
            damage_block(inp, ev, st, c)
        else:
            ev.append(['is_player', 'V+0'])
            if c['victim_player']:
                st['v_low_player'] = True
            else:
                m = c['get_move']('I+0')
                c['ref']('D+0')
                ev.append(add_ev(c['group'], 1, 58, [c['piece'], 0, c['dmg_bits'], 'D+0', *c['e0b'], *c['ecb'], m & 0xFF]))
                c['unref']('D+0')
                st['v_low_event'] = True


def e_x(c):
    return c['e0'][0]


def e_y(c):
    return c['e0'][1]


def ped_extras(c, ptr, vel, ms, vms):
    return [c['piece'], c['dmg_bits'], ptr, *vel, *c['ecb'], ms & 0xFFFF, vms & 0xFFFF]


def ped_case(inp, ev, st, c):
    st['ped'] = True
    m1 = c['get_move']('DI+0')
    ev.append(['is_player', 'V+0'])
    ms = c['ms']
    if c['victim_player']:
        st['ped_vplayer'] = True
        c['ref']('D+0')
        ev.append(add_ev(c['group'], 4, 60, ped_extras(c, 'D+0', c['e0b'], ms, m1)))
        ev.append(['say', 'D+0', 0x1C, 0, 0x3F800000, 0, 0, 0])
        c['unref']('D+0')
    else:
        ev.append(['is_player', 'D+0'])
        if c['damager_player']:
            st['ped_dplayer'] = True
            c['ref']('D+0')
            ev.append(add_ev(c['group'], 3, 60, ped_extras(c, 'D+0', c['e0b'], ms, m1)))
            ev.append(['anger', 'I+0', 1])
            ev.append(['say', 'V+0', 0x1C, 0, 0x3F800000, 0, 0, 0])
            acq = inp['acq'][0] & MASK
            ev.append(['acquaintances', 'V+1248', 4])
            flag = inp['ped_flag'][0] & MASK
            ev.append(['ped_flag', inp['d_ped_type'] & MASK])
            if acq & flag:
                st['ped_hate'] = True
                c['ref']('D+0')
                ev.append(add_ev(c['group'], 36, 26, ['D+0'], editable=True))
                c['unref']('D+0')
            else:
                st['ped_nohate'] = True
            c['unref']('D+0')
        else:
            st['ped_plain'] = True
            c['ref']('D+0')
            ev.append(add_ev(c['group'], 2, 60, ped_extras(c, 'D+0', c['e0b'], ms, m1)))
            c['unref']('D+0')
    # 604D3E
    if not inp['d_dam_null']:
        st['phase2_skip'] = True
        return None
    st['phase2'] = True
    rev = [f_to_bits(mul(x, -1.0)) for x in c['e0']]
    g2 = 'DI+104'
    ev.append(['is_player', 'D+0'])
    if c['damager_player']:
        st['phase2_dplayer'] = True
        c['ref']('V+0')
        ev.append(add_ev(g2, 4, 60, ped_extras(c, 'V+0', rev, ms, m1)))
        c['unref']('V+0')
    else:
        ev.append(['is_player', 'V+0'])
        if c['victim_player']:
            st['phase2_vplayer'] = True
            c['ref']('V+0')
            ev.append(add_ev(g2, 3, 60, ped_extras(c, 'V+0', rev, ms, m1)))
            ev.append(['anger', 'DI+0', 1])
            acq = inp['acq'][0] & MASK
            ev.append(['acquaintances', 'D+1248', 4])
            flag = inp['ped_flag'][0] & MASK
            ev.append(['ped_flag', inp['ped_type'] & MASK])
            if acq & flag:
                st['phase2_hate'] = True
                c['ref']('V+0')
                ev.append(add_ev(g2, 36, 26, ['V+0'], editable=True))
                c['unref']('V+0')
            c['unref']('V+0')
        else:
            st['phase2_plain'] = True
            c['ref']('V+0')
            ev.append(add_ev(g2, 2, 60, ped_extras(c, 'V+0', rev, ms, m1)))
            c['unref']('V+0')
    return None


def object_case(inp, ev, st, c):
    st['object'] = True
    d = c['dmg']
    standing = bool(inp['flags_46c'] & 1)
    static = bool(inp['d_flags'] & 4) or bool(inp['d_flags'] & 0x40000)
    if static:
        st['obj_static'] = True
    if standing and not static:
        st['obj_dot_eval'] = True
        dot = add(mul(c['e0'][1], F(inp['shift_y'])), mul(c['e0'][0], F(inp['shift_x'])))
        if dot < 0.0:
            st['obj_dot_neg'] = True
            val = add(mul(dot, F(inp['mass'])), d)
            d = 0.0 if 0.0 > val else val
            if 0.0 > val:
                st['obj_clamp0'] = True
    limit = 2.0 if inp['has_player'] else 1.0
    contact_same = inp['contact_kind'] == 1
    att_same = inp['d_att_kind'] == 1
    if d > limit:
        st['obj_over_limit'] = True
    if d > limit and not static and standing and not contact_same and not att_same:
        st['obj_hard'] = True
        scaled = mul(f32(d / limit), 10.0)
        ev.append(['local_direction', 'V+0', (c['e0b'][0] ^ 0x80000000) & MASK, (c['e0b'][1] ^ 0x80000000) & MASK])
        ev.append(['generate_damage', 'V+0', 'D+0', 54, ftol_low(scaled), 3, inp['local_dir'] & 0xFF])
        if ftol_low(scaled) == 0:
            st['obj_ftol_zero'] = True
        return 'D+0'
    if d > limit and not static and standing and contact_same:
        st['obj_contact_skip'] = True
    if d > limit and not static and standing and att_same and not contact_same:
        st['obj_att_skip'] = True
    st['obj_event'] = True
    c['ref']('D+0')
    ev.append(add_ev(c['group'], 5, 57, [c['piece'], c['ms'] & 0xFFFF, c['dmg_bits'], 'D+0', *c['e0b'], *c['ecb']]))
    c['unref']('D+0')
    return None


def check_observed(observed, want):
    """Compara lo que registro un lado con lo esperado; devuelve la lista de campos distintos."""
    bad = []

    def canon_events(events):
        out = []
        for e in events:
            e = list(e)
            if e[0] == 'limit_angle':
                e[1] = canon(e[1])
            elif e[0] == 'normalise':
                e[1:4] = [canon(x) for x in e[1:4]]
            elif e[0] == 'kill_ped_with_car':
                e[3] = canon(e[3])
            out.append(e)
        return out
    if canon_events(observed.get('events', [])) != canon_events(want['events']):
        bad.append('events')
    for k in ('nevents', 'scanner', 'ped_changes', 'ignored', 'last_a', 'last_b'):
        if observed.get(k) != want.get(k):
            bad.append(k)
    for k in ('scanner_guards_ok', 'ped_guards_ok', 'time_unchanged', 'others_intact'):
        if observed.get(k) is not True:
            bad.append(k)
    return bad


def selftest():
    def base(**kw):
        d = dict(family=0, hit_by_car=0, dmg_bits=F_(5.0), dmg_null=0, kind=1, type_noise=0, e0=[F_(1.0), F_(2.0), F_(3.0)], ec=[F_(10.0), F_(20.0), F_(30.0)], piece=7, attach_kind=0, attach_type=0,
                 flags_46c=0, ent_flags=0, standing_on=0, mass=F_(100.0), shift_x=F_(0.0), shift_y=F_(0.0), has_player=0, ped_mat_null=0, pos=[F_(1.0), F_(2.0), F_(3.0)], place=[0, 0, 0],
                 ped_type=3, contact_kind=0, task_null=1, is_goto=0, task_move=5, d_flags=0, d_phys=0, d_speed=[0, 0, 0], d_turn_z=0, d_subtype=0, d_tow=0, d_att_kind=0, d_ped_type=4, d_dam_null=0,
                 d_vm=[F_(1.0), 0, 0, 0, F_(1.0), 0, 0, 0, F_(1.0)], d_vpos=[0, 0, 0], box_min=[F_(-1.0)] * 3, box_max=[F_(1.0)] * 3, heading=0, limited=0,
                 is_player_v=0, is_player_d=0, hit_side=0, affects=0, bala=0, local_dir=0, acq=[0, 0], ped_flag=[0, 0], move=[2, 3], now=1000, last_a=0, last_b=0, eax=0, edx=0, stack_seed=0)
        d.update(kw)
        return d
    # intensidad 0: sin eventos; NaN sigue
    w, s = expected(base(dmg_bits=0), 0)
    assert w['events'] == [] and s['early_zero']
    w, s = expected(base(dmg_bits=0x80000000), 0)
    assert w['events'] == []
    w, s = expected(base(dmg_bits=0x7FC00000), 0)
    assert w['events'][0] == ['simplest', 'I+4']
    w, s = expected(base(dmg_null=1), 0)
    assert w['events'] == [] and s['early_null']
    # edificio
    w, s = expected(base(), 0)
    assert w['events'] == [['simplest', 'I+4'], ['regref', 'D+0'], ['add', 'G+0', 6, 59, 0, 0, 0, 0, 0, 0, 0, 7, 1, F_(5.0), 'D+0', F_(1.0), F_(2.0), F_(3.0), F_(10.0), F_(20.0), F_(30.0)], ['cleanref', 'D+0']]
    w, s = expected(base(task_null=0, is_goto=1), 0)
    assert w['events'][1] == ['isgoto', 'T+0'] and w['events'][3][12] == 5
    # vehiculo lento no jugador: evento
    w, s = expected(base(kind=2, d_speed=[0, 0, 0]), 0)
    assert [e[0] for e in w['events']] == ['simplest', 'is_player', 'move', 'regref', 'add', 'cleanref'] and w['events'][4][2] == 1 and w['events'][4][-1] == 2
    # vehiculo rapido, no jugador, no sujeto: atropello con la intensidad cruda
    w, s = expected(base(kind=2, d_speed=[F_(1.0), 0, 0]), 0)
    assert w['events'][-1] == ['kill_ped_with_car', 'V+0', 'D+0', F_(5.0), 0]
    # tren quieto
    w, s = expected(base(kind=2, d_subtype=6, d_phys=4, ent_flags=0x10, flags_46c=1, d_speed=[F_(0.011), 0, 0]), 0)
    assert w['events'][1] == ['kill_ped_with_car', 'V+0', 'D+0', F_(15.0), 0] and s['train_kill']
    # peaton: sin jugadores
    w, s = expected(base(kind=3, d_dam_null=0), 0)
    assert [e[0] for e in w['events']] == ['simplest', 'move', 'is_player', 'is_player', 'regref', 'add', 'cleanref'] and w['events'][5][2] == 2
    w, s = expected(base(kind=3, d_dam_null=1), 0)
    assert s['phase2_plain'] and w['events'][-2][1] == 'DI+104'
    # objeto: evento
    w, s = expected(base(kind=4), 0)
    assert w['events'][2][2] == 5 and w['events'][2][1] == 'G+0'
    # objeto: golpe fuerte
    w, s = expected(base(kind=4, flags_46c=1, dmg_bits=F_(3.0), local_dir=2), 0)
    assert w['events'][-2:] == [['local_direction', 'V+0', F_(1.0) ^ 0x80000000, F_(2.0) ^ 0x80000000], ['generate_damage', 'V+0', 'D+0', 54, 30, 3, 2]] and w['ignored'] == 'D+0'
    # ruido
    w, s = expected(base(kind=1, has_player=1, bala=1, dmg_bits=F_(2.0), now=5000, last_a=0), 0)
    assert w['events'][-1] == ['cleanref', 'V+0'] and w['last_a'] == 5000 and w['events'][-2][2] == 62
    assert ftol_low(1e19) == 0 and ftol_low(3.9) == 3 and ftol_low(-3.9) == 0xFFFFFFFD
    return True


def F_(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


if __name__ == '__main__':
    assert selftest()
    print('ok')
