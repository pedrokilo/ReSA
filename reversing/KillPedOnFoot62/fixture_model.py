"""Modelo independiente (escrito desde el DESENSAMBLADO del EXE, no desde el C++ de la DLL) de las cinco funciones de CTaskComplexKillPedOnFoot:
ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70 y MakeAbortable 0x625E40.

Parte de la fila `input` que registra el banco. Predice: los NOMBRES de los eventos de los falsos en orden, el tamano pedido a operator new, los contextos de Say, la clase del retorno
(None = nulo, 'alloc' = el objeto reservado, 'sub' = la subtarea, numero = AL de MakeAbortable) y marca las ramas alcanzadas (cobertura). La coma flotante se modela con redondeo a 24 bits
de mantisa y exponente sin limite (x87 con la palabra de control a 24 bits). Los colaboradores sustituidos devuelven el bit k-esimo de su mascara en su llamada k-esima.

Desensamblado resumido (mismas direcciones que fixture_oracle.py / README.md):
  CreateSubTask 0x625E70: 0xC8 new(8) | 0xCA temporal StandStill.ProcessPed, new(0x18) | 0xCB new(0x20) | 0x2BF new(0x54), timer | 0x2C0 new(0x34) | 0x2CF new(0x70) | 0x3E9 new(0x38), timer
    | 0x3EA new(0x5C), [obj+0x35] = bit 4 de this+0xC, timer | 0x3EB new(0x20) | 0x4BA FindPlayerPed / Say / GetPedsGroup / GetLeader, rand, new(0x18) | 0x516 limpia el bit 22 de ped+0x474.
  MakeAbortable 0x625E40: ped+0x474 &= ~0x400000; sin subtarea -> 1; si no, subtarea->MakeAbortable (vtable +0x18).
  CreateNextSubTask 0x62B150, CreateFirstSubTask 0x62B490 y ControlSubTask 0x626260: ver los comentarios de cada metodo.
"""
import math
import struct

U32 = 0xFFFFFFFF
T_NONE, T_PAUSE, T_STAND, T_DRAG, T_LEAVECAR, T_DRIVE, T_MELEE, T_ARMED, T_DESTROY, T_SIGNAL, T_FINISHED = 0xC8, 0xCA, 0xCB, 0x2BF, 0x2C0, 0x2CF, 0x3E9, 0x3EA, 0x3EB, 0x4BA, 0x516


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & U32))[0]


def r24(x):
    """Redondea un double a 24 bits de mantisa (exponente sin limite, como el x87 con la palabra de control a 24 bits)."""
    if x != x or x in (math.inf, -math.inf) or x == 0:
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)) / (1 << 24), e)


def s32(v):
    v &= U32
    return v - (1 << 32) if v & 0x80000000 else v


FIVE = 5.0
TENTH = fl(0x3DCCCCCD)


class Model:
    def __init__(s, inp):
        s.i = inp
        s.ev = []
        s.cnt = {}
        s.st = {}
        s.tf = inp['tflags'] & 0xFF
        s.nnew = 0

    # ---- ayudantes
    def b(s, k):
        s.st[k] = True

    def next_k(s, which):
        k = s.cnt.get(which, 0)
        s.cnt[which] = k + 1
        return k

    def bit(s, mask, which):
        return (s.i[mask] >> (s.next_k(which) & 31)) & 1

    def new(s, size):
        s.ev.append(['new', size])
        k = s.nnew
        s.nnew += 1
        return None if (k > 0 or s.i['alloc_null']) else 'alloc'

    def gettype(s):
        s.ev.append(['type'])
        return s.i['sub_type'] & U32

    def abort(s):
        s.ev.append(['abort'])
        return s.bit('m_abort', 'abort')

    def melee(s):
        s.ev.append(['melee'])
        return s.bit('m_melee', 'melee')

    @staticmethod
    def isplayer(t):
        return (t & U32) in (0, 1)

    @staticmethod
    def gang(t):
        return 7 <= s32(t) <= 16

    def pos(s, key):
        return [fl(v) for v in s.i[key]]

    def dist(s):
        p, t = s.pos('p_pos'), s.pos('t_pos')
        dx, dy, dz = r24(t[0] - p[0]), r24(t[1] - p[1]), r24(t[2] - p[2])
        acc = r24(r24(dz * dz) + r24(dy * dy))
        acc = r24(acc + r24(dx * dx))
        return r24(math.sqrt(acc)) if acc == acc else acc

    def speed(s):
        vx, vy = fl(s.i['vx']), fl(s.i['vy'])
        acc = r24(r24(vx * vx) + r24(vy * vy))
        return r24(math.sqrt(acc)) if acc == acc else acc

    def stay(s):
        return (s.i['p_f46c'] >> 21) & 1

    def target_in_veh_bit(s):
        return (s.i['t_f46c'] >> 8) & 1

    def timer_out(s):
        i = s.i
        if (i['tm_started'] & 0xFF) == 0:
            return False
        start = i['time_ms'] if (i['tm_stopped'] & 0xFF) else i['tm_start']
        return (i['time_ms'] & U32) >= ((start + i['tm_interval']) & U32)

    # ---- colaboradores con respuesta por mascara
    def nopolice(s):
        s.ev.append(['nopolice'])
        return s.bit('m_no_police', 'nopolice')

    def driver(s):
        s.ev.append(['isdriver'])
        return s.bit('m_driver', 'driver')

    def passenger(s):
        s.ev.append(['ispassenger'])
        return s.bit('m_passenger', 'passenger')

    def canopen(s):
        s.ev.append(['canopen'])
        return s.bit('m_can_open', 'canopen')

    def room(s):
        s.ev.append(['door'])
        s.ev.append(['room'])
        r = s.bit('m_room', 'room')
        s.tf = (s.tf & ~4) | (r << 2)
        return r

    def give_check(s):
        """Bloque comun 0x62B380 (Next) / 0x62B591 (First): pistola para el pandillero / policia si el objetivo jugador esta parado."""
        i = s.i
        if not (i['t_pd_flags'] & 1):
            s.b('give_pd_flag_off')
            return
        pt = i['p_type']
        if not (s.gang(pt) or s32(pt) == 6):
            s.b('give_not_gang')
            return
        if (i['p_created'] & 0xFF) == 2:
            s.b('give_mission')
            return
        s.ev += [['give'], ['setwtype'], ['movestate']]
        s.b('give_done')

    def near_stand(s):
        i = s.i
        return i['t_stand'] != 0 and i['t_stand'] != i['p_stand']

    # ---- CreateSubTask 0x625E70
    def create_sub(s, tid):
        i = s.i
        tid &= U32
        if tid == T_NONE:
            return s.new(8)
        if tid == T_PAUSE:
            s.ev.append(['still'])
            r = s.new(0x18)
            s.b('sub_pause_long' if (s.tf & 0x60) == 0x60 else 'sub_pause_short')
            return r
        if tid == T_STAND:
            return s.new(0x20)
        if tid == T_DRAG:
            return s.new(0x54)
        if tid == T_LEAVECAR:
            return s.new(0x34)
        if tid == T_DRIVE:
            return s.new(0x70)
        if tid == T_MELEE:
            return s.new(0x38)
        if tid == T_ARMED:
            r = s.new(0x5C)
            s.b('sub_armed_aim' if (s.tf & 0x10) else 'sub_armed_noaim')
            s.tf &= ~0x10
            return r
        if tid == T_DESTROY:
            return s.new(0x20)
        if tid == T_SIGNAL:
            k = s.next_k('playerped')
            s.ev.append(['playerped'])
            code = (i['sel_player'] >> (2 * (k & 15))) & 3
            tgt_addr = 0 if i['tgt_null'] else 1
            if code == tgt_addr:
                s.ev.append(['say', 0x103])
                s.b('signal_player')
            else:
                skip = False
                s.ev.append(['group'])
                g = (i['sel_group'] >> (s.next_k('group') & 31)) & 1
                if g:
                    s.ev.append(['group'])
                    s.next_k('group')
                    s.ev.append(['leader'])
                    lcode = (i['sel_leader'] >> (2 * (s.next_k('leader') & 15))) & 3
                    k2 = s.next_k('playerped')
                    s.ev.append(['playerped'])
                    code2 = (i['sel_player'] >> (2 * (k2 & 15))) & 3
                    skip = lcode == code2
                    s.b('signal_leader_equal' if skip else 'signal_leader_differs')
                else:
                    s.b('signal_no_group')
                if not skip:
                    s.ev.append(['say', 0x102])
            s.ev.append(['rand'])
            return s.new(0x18)
        return None   # 0x516 (limpia el bit en ped+0x474) y cualquier otro

    # ---- CreateFirstSubTask 0x62B490
    def finish1(s, tid):
        r = s.create_sub(tid)
        s.ev.append(['drop'])
        return r

    def create_first(s):
        i = s.i
        s.tf = (s.tf & ~2) | 4
        if i['tgt_null']:
            s.b('first_tgt_null')
            return None
        if fl(i['t_health']) <= 0:
            s.tf |= 8
        d = s.dist()
        tp = s.isplayer(i['t_type'])
        if i['p_veh'] and (i['p_f46c'] >> 8) & 1:
            s.b('first_ped_in_veh')
            return s.finish1(T_LEAVECAR)
        if tp:
            s.b('first_player')
            if s.nopolice():
                s.b('first_nopolice')
                s.give_check()
            elif s.near_stand():
                if d < FIVE:
                    s.b('first_near')
                    s.give_check()
                else:
                    s.b('first_far_or_nan')
        if s.target_in_veh_bit() and i['t_veh']:
            if s.driver() or s.passenger():
                s.b('first_veh_member')
                s.tf |= 4
                if i['v_sub'] in (3, 4):
                    s.b('first_heli_plane')
                    return s.finish1(T_DESTROY)
                if s.stay():
                    s.b('first_stay')
                    return s.finish1(T_DESTROY)
                if s.speed() > TENTH:
                    s.b('first_moving')
                    return s.finish1(T_DESTROY)
                if not s.canopen():
                    s.b('first_locked')
                    return s.finish1(T_DESTROY)
                if i['v_type'] == 9:
                    s.b('first_bike')
                    if not s.melee():
                        s.b('first_bike_armed')
                        return s.finish1(T_DESTROY)
                if i['v_type'] == 5:
                    s.b('first_boat')
                    return s.finish1(T_DESTROY)
                if not s.room():
                    s.b('first_no_room')
                    return s.finish1(T_DESTROY)
                if (i['t_f46c'] >> 29) & 1:
                    s.b('first_dont_drag')
                    return s.finish1(T_DESTROY)
                s.b('first_drag')
                return s.finish1(T_DRAG)
        s.b('first_on_foot')
        return s.finish1(T_MELEE if s.melee() else T_ARMED)

    # ---- CreateNextSubTask 0x62B150
    def create_next(s):
        i = s.i
        if i['tgt_null']:
            s.b('next_tgt_null')
            return s.create_sub(T_FINISHED)
        t = s.gettype()
        if t == T_LEAVECAR:
            tp = s.isplayer(i['t_type'])
            if (i['p_f46c'] >> 8) & 1:
                s.b('next_leave_in_veh')
                return s.create_sub(T_DRIVE)
            d = s.dist()
            if s.nopolice():
                s.b('next_leave_nopolice')
                if tp:
                    s.give_check()
            elif s.near_stand() and d < FIVE:
                s.b('next_leave_near')
                if tp:
                    s.give_check()
            return s.create_sub(T_MELEE if s.melee() else T_ARMED)
        if t == T_PAUSE:
            if s32(i['p_type']) == 6 and (i['p_cop'] & 0xFF):
                s.ev.append(['wanted'])
                s.b('next_pause_cop')
                return s.create_sub(T_ARMED if (i['wp_chance'] & 0xFFFF) else T_PAUSE)
            if (s.tf & 0x20) and (s.tf & 0x40):
                s.ev.append(['wanted'])
                if not (i['wp_bits'] & 4):
                    s.tf &= ~0x40
                    s.b('next_pause_resume')
                    return s.create_first()
            s.b('next_pause_finish')
            return s.create_sub(T_FINISHED)
        if t == T_DRAG:
            if not (i['sub_b10'] & 4):
                s.b('next_drag_weapon')
                return s.create_sub(T_MELEE if s.melee() else T_ARMED)
            s.b('next_drag_quit')
            return s.create_sub(T_DESTROY)
        if t == T_DRIVE:
            s.b('next_drive')
            return s.create_sub(T_LEAVECAR)
        if t in (T_NONE, T_STAND, T_DESTROY, T_MELEE, T_ARMED):
            s.b('next_finish')
            return s.create_sub(T_FINISHED)
        s.b('next_default')
        return None

    # ---- ControlSubTask 0x626260
    def ctl_finish(s, nt):
        i = s.i
        if not i['tgt_null']:
            t_contact = {0: None, 1: 'CT', 2: 'CP', 3: 'CT'}[i['t_contact']]
            p_contact = {0: None, 1: 'CP', 2: 'CT', 3: 'CP'}[i['p_contact']]
            if t_contact and p_contact:
                code = {'CP': i['cp_code'], 'CT': i['ct_code']}
                if (code[t_contact] & 0xFF) != (code[p_contact] & 0xFF):
                    s.ev.append(['add'])
                    s.b('ctl_area_event')
                else:
                    s.b('ctl_area_same')
            else:
                s.b('ctl_area_null')
        if nt != T_NONE:
            if s.abort():
                s.b('ctl_change_task')
                return s.create_sub(nt)
            s.b('ctl_change_refused')
        if not i['tgt_null']:
            s.ev.append(['c65'])
        return 'sub'

    def control(s):
        i = s.i
        if i['tgt_null']:
            s.b('ctl_tgt_null')
            return s.ctl_finish(T_FINISHED)
        t = s.gettype()
        if t == T_PAUSE:
            s.b('ctl_pause')
            if (s.tf & 0x20) and (s.tf & 0x40):
                s.ev.append(['wanted'])
                if not (i['wp_bits'] & 4):
                    s.abort()
                    s.b('ctl_pause_abort')
            return 'sub'
        tm = s32(i['time_t'])
        if tm > 0 and (i['time_ms'] & U32) > ((i['start_t'] + tm) & U32):
            s.b('ctl_timeout')
            if not s.abort():
                s.b('ctl_timeout_refused')
                return s.ctl_finish(T_NONE)
            return s.new(0x20)
        if s.tf & 2:
            s.b('ctl_new_target')
            if not s.abort():
                s.b('ctl_new_target_refused')
                return s.ctl_finish(T_NONE)
            return s.create_first()
        nt = T_NONE
        run_switch = True
        if not (s.tf & 8):
            if fl(i['t_health']) <= 0:
                s.b('ctl_dead')
                if s.abort():
                    s.tf |= 8
                    if s.gettype() == T_SIGNAL:
                        s.b('ctl_dead_signal')
                        return 'sub'
                    if s32(i['p_type']) == 6 and not s.isplayer(i['t_type']):
                        s.ev.append(['say', 0xE])
                        s.b('ctl_dead_cop_say')
                    if (i['p_f478'] >> 12) & 1:
                        s.b('ctl_dead_signal_task')
                        return s.create_sub(T_SIGNAL)
                    s.b('ctl_dead_leavegroup')
                    return s.new(8)
                s.b('ctl_dead_refused')
            elif s.isplayer(i['t_type']):
                back = (i['tw_bits'] & 4) != 0
                if back:
                    s.b('ctl_backoff_wanted')
                elif s32(i['p_type']) == 6 and (i['p_cop'] & 0xFF):
                    s.ev.append(['wanted'])
                    back = (i['wp_chance'] & 0xFFFF) == 0
                    s.b('ctl_backoff_cop' if back else 'ctl_cop_chance')
                if back:
                    if s.gettype() != T_PAUSE:
                        nt = T_PAUSE
                        s.b('ctl_to_pause')
                        if (s.tf & 0x20) and (i['tw_bits'] & 4):
                            s.tf |= 0x40
                            s.b('ctl_waiting_set')
                        run_switch = False
        if run_switch:
            nt = s.ctl_switch(s.gettype())
        s.tf &= ~1
        return s.ctl_finish(nt)

    def ctl_switch(s, t):
        i = s.i
        nt = T_NONE
        inveh = s.target_in_veh_bit()
        tveh = i['t_veh']
        if t == T_MELEE:
            s.b('ctl_melee')
            if not s.melee():
                s.b('ctl_melee_to_armed')
                return T_ARMED
            if inveh and i['v_sub'] in (3, 4):
                s.b('ctl_melee_air')
                return T_DESTROY
            if not inveh or not tveh:
                return nt
            if i['v_type'] == 5 or s.stay():
                return nt
            if not s.canopen():
                s.b('ctl_melee_locked')
                return nt
            r = s.room()
            s.b('ctl_melee_room' if r else 'ctl_melee_noroom')
            return T_DRAG if r else T_DESTROY
        if t == T_ARMED:
            s.b('ctl_armed')
            melee = s.melee()
            if melee and not s.isplayer(i['p_type']):
                nt = T_MELEE
                s.b('ctl_armed_to_melee')
            else:
                ammo = i['p_ammo'][i['p_slot']] & U32
                if ammo == 0 and not s.isplayer(i['p_type']):
                    s.b('ctl_armed_noammo')
                    found = next((k for k in range(13) if s32(i['p_ammo'][k]) > 0), None)
                    if found is not None:
                        s.ev.append(['setwslot'])
                        s.b('ctl_armed_switch_slot')
                    else:
                        s.ev.append(['setwtype'])
                        nt = T_MELEE
                        s.b('ctl_armed_unarmed')
            if not inveh or not tveh:
                return nt
            if not (s.driver() or s.passenger()):
                return nt
            if i['v_sub'] in (3, 4):
                s.b('ctl_armed_air')
                return T_DESTROY
            if i['v_type'] in (9, 5) or s.stay():
                return nt
            if not (s.speed() <= TENTH):
                s.b('ctl_armed_moving')
                return nt
            if not s.canopen():
                return nt
            r = s.room()
            s.b('ctl_armed_room' if r else 'ctl_armed_noroom')
            return T_DRAG if r else T_DESTROY
        if t == T_DRAG:
            s.b('ctl_drag')
            if not inveh or not tveh:
                return nt
            if not (s.driver() or s.passenger()):
                return nt
            if s.speed() > TENTH:
                s.b('ctl_drag_moving')
                return T_DESTROY
            if not s.timer_out():
                s.b('ctl_drag_timer')
                return nt
            if not (s.passenger() or s.driver()):
                return nt
            r = s.room()
            s.b('ctl_drag_room' if r else 'ctl_drag_noroom')
            if not r:
                nt = T_DESTROY
            s.b('ctl_timer_restart')
            return nt
        if t == T_DESTROY:
            s.b('ctl_destroy')
            if not inveh:
                return T_MELEE if s.melee() else T_ARMED
            if tveh:
                s.ev.append(['healthy'])
                if not s.bit('m_healthy', 'healthy'):
                    s.b('ctl_destroy_unhealthy')
                    return T_SIGNAL
            if (i['t_f46c'] >> 29) & 1:
                return nt
            if i['v_sub'] in (3, 4) or i['v_type'] in (9, 5) or s.stay():
                return nt
            if not (s.speed() <= TENTH):
                return nt
            if not s.canopen():
                return nt
            if s.tf & 4:
                s.b('ctl_destroy_drag_flag')
                return T_DRAG
            if not s.timer_out():
                s.b('ctl_destroy_timer')
                return nt
            r = s.room()
            s.b('ctl_destroy_room' if r else 'ctl_destroy_noroom')
            if r:
                nt = T_DRAG
            s.b('ctl_timer_restart')
            return nt
        s.b('ctl_default_sub')
        return nt


def run_model(fn, inp):
    m = Model(inp)
    short = fn.split('::')[-1]
    if short == 'MakeAbortable':
        if inp['sub_null']:
            m.b('abort_no_sub')
            return m, 1
        m.ev.append(['abort'])
        m.b('abort_sub')
        return m, m.bit('m_abort', 'abort')
    if short == 'CreateSubTask':
        m.b('sub_%x' % (inp['task_id'] & U32 if (inp['task_id'] & U32) in (0xC8, 0xCA, 0xCB, 0x2BF, 0x2C0, 0x2CF, 0x3E9, 0x3EA, 0x3EB, 0x4BA, 0x516) else 0xFFFF))
        return m, m.create_sub(inp['task_id'])
    if short == 'CreateNextSubTask':
        return m, m.create_next()
    if short == 'CreateFirstSubTask':
        return m, m.create_first()
    return m, m.control()


def expected(fn, inp):
    """(esperado, ramas): nombres de eventos en orden, tamanos de `new`, contextos de `say` y clase del retorno."""
    m, ret = run_model(fn, inp)
    want = {'names': [e[0] for e in m.ev], 'ret': ret, 'say': [e[1] for e in m.ev if e[0] == 'say'], 'new': [e[1] for e in m.ev if e[0] == 'new']}
    return want, m.st
