"""Oraculo de CEventScanner::ScanForEvents (0x607E30), modelado desde el desensamblado.

0x607E30 (thiscall, ECX = CEventScanner -> EBP, ped en la pila -> ESI, intel = [ped+47Ch] -> EDI, `ret 4`):
  607E50  si NOT (ahora > [this]) (sin signo): fin sin hacer nada
  607E73  VPC::Scan(this+4, ped, intel+0E0h, 16) (0x603720) ; 0x606580(ped, [intel+170h]) (stdcall) ; 0x606890(this+10h, ped) ; 0x607D80(this+0ACh, ped, intel+130h, 16) ;
          0x6060A0(this+1Ch, ped) ; 0x603E70(this+0C8h, ped) ; CMentalState::Process(intel+174h, ped) (0x6008A0)
  607ECE  si NOT (flags & 1, de pie): si (flags & 200h, en el aire) o IsInAir(ped) (0x5F1CB0): CEventInAir -> Add(intel+68h) ; salta al bloque de fuego
          (de pie, o ni bandera ni IsInAir): si flags & 200h: GetCurrentEvent(intel+38h) ; si nulo o (segunda llamada) GetEventType() != 0x21: flags &= ~200h
  607F5A  si [ped+730h] (fuego): CEventOnFire ; si IsPlayer(ped) (0x5DF8F0): Add ; si no, GetSimplestActiveTask(intel+4): nulo -> Add ; MakeAbortable(ped, 1, &evento) (vtable+18h): AL != 0 -> Add ;
          si no GenerateDamageEvent(ped, [fuego+14h], 0x25, 5, 3, 0) (0x73A530)
  607FF0  0x603BF0(this+0BCh, ped, intel+130h, 16) ;
  608008  FindTaskByType(intel+4, 4, 0x3E8) ; si nulo (3), (1), (2) ; si hay tarea: objetivo = [tarea+10h] ; si objetivo y [objetivo+584h] y [ped+584h]: si byte [+2Fh] distintos:
          CEventAreaCodes(objetivo) (0x4B2190: RegisterReference) -> Add -> dtor (CleanUpOldReference)
  6080B2  si [ped+584h]: modelo = (int16)[contacto+22h] ; si (uint16)[8CD674] == modelo o (uint16)[8CD678] == modelo: CEventEscalator -> Add
  608114  si flags & 100h (en vehiculo) y [ped+58Ch] y [veh+590h] == 5: si NOT IsPlayer(ped): FindPlayerPed(-1) ; si [jugador+584h] == veh: FindPlayerPed(-1) ;
          CEventPedEnteredMyVehicle(jugador, veh, 10) (0x4AEAC0: RegisterReference(jugador), RegisterReference(veh)), m_TaskId = 0x2C2, Add, dtor (CleanUp jugador, CleanUp veh)
  6081A4  si salud <= 0.0 (falso con NaN) y IsAlive(ped) (0x5E0170) y ahora > [0C0B038] (sin signo): CEventDamage(0, 0, 0x37, 3, 0, false, false) ;
          si NOT HasEventOfType(intel+68h, &evento) (0x4AB5E0): GenerateDamageEvent(ped, 0, 0x36, 10, 3, 0) ; [0C0B038] = ahora + 0x7D0
Eventos: InAir tipo 33 prioridad 61 ; OnFire 58/66 ; AreaCodes 67/55 (+0xC ped) ; Escalator 74/56 ; PedEnteredMyVehicle 17/39 (+0x14 ped, +0x18 veh, +0x1C puerta) editable ;
Damage 9/65 editable. Los eventos no editables vuelcan los campos editables como 0.
"""
from oracle_common import bits_to_f, u32, s32

NAME = 'CEventScanner::ScanForEvents'
# Misma suma a mano que Fixtures/FixScan.hpp (guarda + objeto + guarda)
ARENAS = {'kEsOff': 0x20, 'kEsBytes': 0xD4, 'kEsArena': 0x114, 'kSpedOff': 0x40, 'kSpedBytes': 0x740, 'kSpedArena': 0x7C0, 'kSintOff': 0x40, 'kSintBytes': 0x180, 'kSintArena': 0x1C0,
          'kSentOff': 0x20, 'kSentBytes': 0x40, 'kSentArena': 0x80, 'kStgtOff': 0x40, 'kStgtBytes': 0x5A0, 'kStgtArena': 0x620, 'kSktOff': 0x20, 'kSktBytes': 0x20, 'kSktArena': 0x60,
          'kSvehOff': 0x40, 'kSvehBytes': 0x5A0, 'kSvehArena': 0x620, 'kSplOff': 0x40, 'kSplBytes': 0x5A0, 'kSplArena': 0x620, 'kSfireOff': 0x20, 'kSfireBytes': 0x28, 'kSfireArena': 0x68,
          'kStaskOff': 0x20, 'kStaskBytes': 0x20, 'kStaskArena': 0x60, 'kSevOff': 0x20, 'kSevBytes': 0x20, 'kSevArena': 0x60}


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def add_plain(typ, prio, *extras):
    return ['add', 'I+104', typ, prio, 0, 0, 0, 0, 0, 0, 0, *extras]


def expected(inp, case):
    ev = []
    stats = {}
    now = u32(inp['now'])
    flags = inp['flags'] & 0xFFFFFFFF
    out_flags = flags
    dead_timer = u32(inp['dead_timer'])
    if not (now > u32(inp['next_scan'])):
        stats['no_scan'] = True
        return finish(ev, flags, out_flags, dead_timer, now), stats
    stats['scan'] = True
    closest = {0: 0, 1: 'TG+0', 2: 0xA5A5A5A5}[inp['closest_kind']]
    ev.append(['vpc', 'E+4', 'D+0', 'I+224', 16])
    ev.append(['scan_606580', 'D+0', closest])
    ev.append(['scan_object', 'E+16', 'D+0'])
    ev.append(['scan_acquaintance', 'E+172', 'D+0', 'I+304', 16])
    ev.append(['scan_attractor', 'E+28', 'D+0'])
    ev.append(['scan_fire', 'E+200', 'D+0'])
    ev.append(['mental_process', 'I+372', 'D+0'])
    standing = bool(flags & 1)
    air = bool(flags & 0x200)
    in_air_event = False
    if not standing:
        if air:
            in_air_event = True
        else:
            ev.append(['is_in_air', 'D+0'])
            in_air_event = (inp['is_in_air'] & 0xFF) != 0
    if in_air_event:
        ev.append(add_plain(33, 61))
        stats['in_air_event'] = True
        stats['in_air_by_flag'] = air
        stats['in_air_by_geometry'] = not air
    elif not standing and not air:
        stats['not_standing_no_air'] = True
    elif air:
        # de pie con la bandera de aire
        ev.append(['get_current_event', 'I+56'])
        clear = False
        if not inp['has_cur']:
            clear = True
            stats['air_clear_nocur'] = True
        else:
            ev.append(['get_current_event', 'I+56'])
            ev.append(['current_event_type', 'EV+0'])
            if u32(inp['cur_type']) != 0x21:
                clear = True
                stats['air_clear_other'] = True
            else:
                stats['air_keep'] = True
        if clear:
            out_flags = flags & ~0x200
    if inp['has_fire']:
        stats['fire'] = True
        creator = {0: 0, 1: 'PL+0', 2: 0x5A5A5A5A}[inp['creator_kind']]
        ev.append(['is_player', 'D+0'])
        add = True
        if (inp['is_player'] & 0xFF) != 0:
            stats['fire_player'] = True
        else:
            ev.append(['simplest', 'I+4'])
            if not inp['has_task']:
                stats['fire_no_task'] = True
            else:
                ev.append(['make_abortable', 'TK+0', 'D+0', 1, 58])
                if (inp['abort'] & 0xFF) != 0:
                    stats['fire_abortable'] = True
                else:
                    add = False
                    stats['fire_damage'] = True
                    ev.append(['generate_damage', 'D+0', creator, 0x25, 5, 3, 0])
        if add:
            ev.append(add_plain(58, 66))
    ev.append(['scan_sexy', 'E+188', 'D+0', 'I+304', 16])
    # tarea de matar
    found = False
    for k, primary in enumerate((4, 3, 1, 2)):
        ev.append(['find_task', 'I+4', primary, 1000])
        if inp['find'][k]:
            found = True
            stats['found_call%d' % (k + 1)] = True
            break
    if not found:
        stats['not_found'] = True
    if found and not inp['target_null']:
        stats['target'] = True
        if not inp['tgt_contact_null'] and not inp['ped_contact_null']:
            stats['areas_compared'] = True
            if (inp['tgt_area'] & 0xFF) != (inp['ped_area'] & 0xFF):
                stats['area_event'] = True
                ev.append(['regref', 'TG+0'])
                ev.append(add_plain(67, 55, 'TG+0'))
                ev.append(['cleanref', 'TG+0'])
            else:
                stats['area_equal'] = True
    # escalera
    if not inp['ped_contact_null']:
        model = s16(inp['model_index'])
        stats['contact'] = True
        if (inp['g1'] & 0xFFFF) == model or (inp['g2'] & 0xFFFF) == model:
            stats['escalator'] = True
            ev.append(add_plain(74, 56))
        elif model < 0 and ((inp['g1'] & 0xFFFF) == (inp['model_index'] & 0xFFFF) or (inp['g2'] & 0xFFFF) == (inp['model_index'] & 0xFFFF)):
            stats['escalator_sign_quirk'] = True
    # barco
    if (flags & 0x100) and not inp['veh_null'] and (inp['veh_type'] == 5):
        stats['boat'] = True
        ev.append(['is_player', 'D+0'])
        if (inp['is_player'] & 0xFF) == 0:
            ev.append(['find_player', 0xFFFFFFFF])
            if inp['pl_contact_is_veh']:
                ev.append(['find_player', 0xFFFFFFFF])
                ev.append(['regref', 'PL+0'])
                ev.append(['regref', 'V+0'])
                ev.append(['add', 'I+104', 17, 39, 0, 0, 1, 1, 0x2C2, 0xFFFF, 0, 'PL+0', 'V+0', 10])
                ev.append(['cleanref', 'PL+0'])
                ev.append(['cleanref', 'V+0'])
                stats['boat_event'] = True
            else:
                stats['boat_other_contact'] = True
        else:
            stats['boat_player'] = True
    # peaton muerto
    health = bits_to_f(inp['health_bits'])
    if health <= 0.0:
        stats['health_le0'] = True
        ev.append(['is_alive', 'D+0'])
        if (inp['is_alive'] & 0xFF) != 0:
            stats['alive'] = True
            if now > dead_timer:
                stats['timer_due'] = True
                ev.append(['has_event', 'I+104', 9, 65, 0, 0, 1, 1, 0xC8, 0xFFFF, 0, 0, 0, 0x37, 3, 0, 8, 0, 0xBF, 0x41000000, 0x3F800000, 0, 0, 0])
                if (inp['has_event'] & 0xFF) == 0:
                    ev.append(['generate_damage', 'D+0', 0, 0x36, 10, 3, 0])
                    dead_timer = u32(now + 0x7D0)
                    stats['dead_damage'] = True
                else:
                    stats['has_event'] = True
            else:
                stats['timer_not_due'] = True
    elif health != health:
        stats['health_nan'] = True
    return finish(ev, flags, out_flags, dead_timer, now), stats


def finish(ev, flags, out_flags, dead_timer, now):
    changes = []
    if out_flags != flags:
        changes.append([0x46D, (out_flags >> 8) & 0xFF])
    return {'events': ev, 'nevents': len(ev), 'ped_changes': changes, 'dead_timer': dead_timer, 'time_after': now}


def selftest():
    def base(**kw):
        d = dict(family=0, now=100000, next_scan=99999, dead_timer=0, flags=0x1, is_in_air=0, has_cur=1, cur_type=33, has_fire=0, creator_kind=0, is_player=0, has_task=1, abort=0,
                 find=[0, 0, 0, 0], target_null=0, tgt_contact_null=0, tgt_area=1, ped_contact_null=1, ped_area=1, model_index=5, g1=6, g2=7, veh_null=1, veh_type=5, pl_contact_is_veh=0,
                 health_bits=0x42C80000, is_alive=1, has_event=0, closest_kind=0, eax=0, edx=0, stack_seed=0)
        d.update(kw)
        return d
    want, st = expected(base(next_scan=100000), 0)
    assert want['events'] == [] and st['no_scan']
    want, st = expected(base(), 0)
    assert [e[0] for e in want['events']] == ['vpc', 'scan_606580', 'scan_object', 'scan_acquaintance', 'scan_attractor', 'scan_fire', 'mental_process', 'scan_sexy', 'find_task', 'find_task', 'find_task', 'find_task']
    # en el aire por bandera y por geometria
    want, st = expected(base(flags=0x200), 0)
    assert want['events'][7] == add_plain(33, 61) and st['in_air_by_flag'] and 'is_in_air' not in [e[0] for e in want['events']]
    want, st = expected(base(flags=0, is_in_air=0x101), 0)
    assert want['events'][7] == ['is_in_air', 'D+0'] and want['events'][8] == add_plain(33, 61) and st['in_air_by_geometry']
    want, st = expected(base(flags=0, is_in_air=0x100), 0)
    assert want['events'][7] == ['is_in_air', 'D+0'] and want['events'][8][0] == 'scan_sexy'
    # de pie con bandera de aire
    want, st = expected(base(flags=0x201, has_cur=0), 0)
    assert want['events'][7] == ['get_current_event', 'I+56'] and want['ped_changes'] == [[0x46D, 0]] and st['air_clear_nocur']
    want, st = expected(base(flags=0x201, cur_type=33), 0)
    assert st['air_keep'] and want['ped_changes'] == []
    want, st = expected(base(flags=0x201, cur_type=34), 0)
    assert st['air_clear_other'] and want['ped_changes'] == [[0x46D, 0]]
    # fuego
    want, st = expected(base(has_fire=1, is_player=0x101), 0)
    assert st['fire_player'] and want['events'][-6] == add_plain(58, 66), want['events']
    want, st = expected(base(has_fire=1, has_task=0), 0)
    assert st['fire_no_task']
    want, st = expected(base(has_fire=1, abort=0, creator_kind=1), 0)
    assert want['events'][10] == ['generate_damage', 'D+0', 'PL+0', 0x25, 5, 3, 0] and st['fire_damage']
    # tarea de matar y areas
    want, st = expected(base(find=[0, 0, 1, 0], ped_contact_null=0, tgt_area=0x80, ped_area=0x7F), 0)
    assert st['found_call3'] and st['area_event'] and want['events'][-3:] == [['regref', 'TG+0'], add_plain(67, 55, 'TG+0'), ['cleanref', 'TG+0']]
    want, st = expected(base(find=[1, 0, 0, 0], ped_contact_null=0, tgt_area=0x80, ped_area=0x80), 0)
    assert st['area_equal']
    # escalera: 0xFFFF contra g=0xFFFF no coincide (movsx)
    want, st = expected(base(ped_contact_null=0, model_index=0xFFFF, g1=0xFFFF), 0)
    assert 'escalator' not in st and st['escalator_sign_quirk']
    want, st = expected(base(ped_contact_null=0, model_index=0x1234, g2=0x1234), 0)
    assert st['escalator'] and want['events'][-1] == add_plain(74, 56)
    # barco
    want, st = expected(base(flags=0x101, veh_null=0, pl_contact_is_veh=1), 0)
    assert st['boat_event'] and want['events'][-3] == ['add', 'I+104', 17, 39, 0, 0, 1, 1, 0x2C2, 0xFFFF, 0, 'PL+0', 'V+0', 10]
    want, st = expected(base(flags=0x101, veh_null=0, is_player=1), 0)
    assert st['boat_player']
    # peaton muerto
    want, st = expected(base(health_bits=0x80000000, dead_timer=99999), 0)
    assert st['dead_damage'] and want['dead_timer'] == 102000 and want['events'][-1] == ['generate_damage', 'D+0', 0, 0x36, 10, 3, 0]
    want, st = expected(base(health_bits=0x7FC00000), 0)
    assert st['health_nan'] and want['dead_timer'] == 0
    want, st = expected(base(health_bits=0xBF800000, has_event=1, dead_timer=99999), 0)
    assert st['has_event'] and want['dead_timer'] == 99999
    want, st = expected(base(health_bits=0xBF800000, dead_timer=100000), 0)
    assert st['timer_not_due']
    return True


if __name__ == '__main__':
    selftest()
    print('ok')
