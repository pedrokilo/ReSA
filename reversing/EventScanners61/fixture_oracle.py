"""Oraculo independiente del banco EventScanners61 (cinco funciones de escaneo de eventos), modelado desde el DESENSAMBLADO del EXE.

Cada `expected_*` recibe la fila `input` que registra el banco y devuelve (observed, stats): `observed` es exactamente lo que cada lado (EXE y DLL) debe registrar en `observed` y
`stats` son las banderas de cobertura. La autoprueba (`python -I fixture_oracle.py`) comprueba el oraculo con casos escritos a mano.

== CMentalState::Process 0x6008A0 (thiscall, ECX = estado -> EBP, ped en la pila -> ESI, `ret 4`) ==
  6008BF  fld [ped+540h] (m_fHealth) ; call 821B40 (_ftol2) ; [estado+10h] = AL                           (salud del peaton, SIEMPRE; byte bajo del int64 truncado)
  6008D0  si [ped+46Ch] & 100h (en vehiculo) y [ped+58Ch] (vehiculo) != 0: fld [veh+4C0h] ; _ftol2 ; [estado+12h] = AL
  6008F7  si NO en vehiculo (test edi=100h): old = [estado+11h]
            old >= 0x32: nuevo = [estado+10h]: < 0x0A -> evento A (vtable 86CBE8, tipo 0x34 REALLY_LOW) ; si no, < 0x32 -> evento B (86CBA0, tipo 0x35 LOW)
            old <  0x32: si old >= 0x0A y nuevo < 0x0A -> evento A
          si en vehiculo: old = movzx [estado+13h] (byte) contra 0x258/0x12C (600/300): un byte nunca llega, las ramas no se cumplen
  600A43  si [estado+1] (anterior) <= 3: [estado+0] > 6 -> evento C (86CC78, tipo 0x33 HIGH_ANGER) ; si no [estado+0] > 3 -> evento D (86CC30, tipo 0x32 LOW_ANGER)
          si no (anterior > 3): si anterior <= 6 y [estado+0] > 6 -> evento C
          (cada evento: ctor 4AC450 + vtable, CEventGroup::Add(&evento, false) sobre [ped+47Ch]+68h (0x4AB420), dtor 4AC480)
  600AFF  [estado+1] = [estado+0] ; [estado+11h] = [estado+10h] ; [estado+13h] = [estado+12h]
  Valores del evento: m_nTimeActive = 0, m_bValid = 0, m_bAddToEventGroup = 1, m_TaskId = 0xC8, m_FacialExpressionType = -1 (ctor 4AC450); prioridades 69/68/34/33.
"""
import math
import struct

import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import oracle_vpc
import oracle_scan
import oracle_stuck

NAME_MENTAL = 'CMentalState::Process'
NAME_VPC = oracle_vpc.NAME
NAME_SCAN = oracle_scan.NAME
NAME_STUCK = oracle_stuck.NAME
ALL_CASES = {NAME_MENTAL: 4096, NAME_VPC: 4096, NAME_SCAN: 4096, NAME_STUCK: 4096}
FN_BITS = {NAME_MENTAL: 1, NAME_VPC: 2, NAME_SCAN: 4, NAME_STUCK: 8}
HOOKS = {NAME_MENTAL: 0x6008A0, NAME_VPC: 0x603720, NAME_SCAN: 0x607E30, NAME_STUCK: 0x602C00}
HOOK_NAMES = {NAME_MENTAL: 'CMentalState::Process', NAME_VPC: 'CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents', NAME_SCAN: 'CEventScanner::ScanForEvents', NAME_STUCK: 'CPedStuckChecker::TestPedStuck'}
# direcciones del EXE sustituidas por falsos en cada funcion
STUBBED = {NAME_MENTAL: (0x4AB420,), NAME_VPC: (0x4AB420, 0x6819D0, 0x61A360, 0x535300, 0x5F5A30, 0x601D70, 0x571B70, 0x571A00),
           NAME_SCAN: (0x603720, 0x606580, 0x606890, 0x607D80, 0x6060A0, 0x603E70, 0x603BF0, 0x6008A0, 0x5F1CB0, 0x4B8CA0, 0x5DF8F0, 0x6819D0, 0x6817D0, 0x73A530, 0x56E210, 0x5E0170, 0x4AB5E0, 0x4AB420, 0x571B70, 0x571A00),
           NAME_STUCK: (0x5DF8F0, 0x601070, 0x601110, 0x601180, 0x59C910, 0x5674E0, 0x56A490, 0x4241C0, 0x5429F0, 0x439A80, 0x53CBE0, 0x53CB50, 0x4AB650, 0x4AB420, 0x571B70, 0x571A00)}
# Minimos de cobertura (~50 % de lo medido en la corrida 20261019-215324-345819 del banco)
MINIMUMS = {NAME_STUCK: {'reset_early': 168, 'standing': 140, 'state0_end': 1080, 'not_stuck_flag': 36, 'swim': 100, 'counter_inc': 1370, 'thr1_count': 1100, 'thr1': 450, 'probe_turn': 320,
                         'probe_nohit': 140, 'probe_not_turn': 215, 'no_water': 690, 'event': 760, 'n_gt1': 285, 'n_eq1': 485, 'wall': 310, 'side_m1': 105, 'negated': 210, 'probe_hit': 180,
                         'probe_moved': 140, 'side_2': 70, 'wall_moved': 110, 'water_block': 35, 'dmg_skip_player': 465, 'counter_restart': 90, 'climb': 28, 'thr2': 85, 'side_1': 75,
                         'wall_head_skip': 30, 'probe_dmg_skip': 17, 'water_low': 67, 'not_player': 67, 'jet': 35, 'probe_head_skip': 21, 'side_0': 54, 'wall_dmg_skip': 6}, NAME_SCAN: {'scan': 1800, 'in_air_event': 400, 'in_air_by_flag': 250, 'in_air_by_geometry': 150, 'not_found': 1270, 'contact': 1340, 'health_le0': 145, 'alive': 90,
                        'timer_due': 60, 'has_event': 30, 'air_clear_other': 130, 'fire': 295, 'fire_player': 145, 'found_call1': 170, 'found_call2': 125, 'found_call3': 115,
                        'found_call4': 100, 'target': 415, 'escalator': 105, 'fire_abortable': 50, 'areas_compared': 235, 'area_event': 100, 'escalator_sign_quirk': 110,
                        'no_scan': 255, 'not_standing_no_air': 165, 'air_clear_nocur': 90, 'timer_not_due': 30, 'area_equal': 135, 'fire_damage': 60, 'boat': 125, 'boat_event': 38,
                        'dead_damage': 28, 'boat_player': 60, 'fire_no_task': 33, 'boat_other_contact': 30, 'air_keep': 33, 'health_nan': 12}, NAME_VPC: {'timer_not_started': 128, 'timer_stopped': 170, 'not_due': 430, 'wrap_sum': 300, 'not_due_by_one': 110, 'due': 1600, 'due_exact': 470, 'inside': 560,
                       'los_called': 530, 'event': 250, 'c1_fail': 350, 'c2_fail': 270, 'path_no_task': 140, 'path_not_goto': 140, 'path_no_veh': 140, 'path_los_clear': 75,
                       'dist_small': 200, 'just_left_car': 215, 'train': 100, 'too_far': 28}, NAME_MENTAL: {'ev_really_low': 550, 'ev_low': 300, 'no_events': 800, 'ped_hp_ge256': 570, 'ev_real_low_from_mid': 190, 'in_vehicle': 370, 'vehicle_updated': 270,
                          'ped_hp_negative': 330, 'ev_high_anger': 600, 'two_events': 360, 'ev_low_anger': 150, 'vehicle_null_in_vehicle': 90, 'ped_hp_nan_inf': 80}}
for _k in range(16):
    MINIMUMS[NAME_MENTAL]['family%d' % _k] = 256
    MINIMUMS[NAME_VPC]['family%d' % _k] = 256
    MINIMUMS[NAME_SCAN]['family%d' % _k] = 256
    MINIMUMS[NAME_STUCK]['family%d' % _k] = 256


def _enabled():
    """Funciones activas en la compilacion: se lee `kEnabled` de Fixtures.hpp (el banco final las activa todas)."""
    text = (Path(__file__).resolve().parent / 'Fixtures.hpp').read_text(encoding='utf-8')
    names = dict(re.findall(r'(kFn\w+)\s*=\s*(\d+)', text))
    expr = re.search(r'kEnabled\s*=\s*([^;]+);', text).group(1)
    for k, v in names.items():
        expr = expr.replace(k, v)
    return eval(expr, {'__builtins__': {}})


CASES = {n: c for n, c in ALL_CASES.items() if _enabled() & FN_BITS[n]}
NAMES = list(CASES)
ARENAS = {'kMsOff': 0x20, 'kMsBytes': 0x14, 'kMsArena': 0x54, 'kMpedOff': 0x40, 'kMpedBytes': 0x5A0, 'kMpedArena': 0x620,
          'kMvehOff': 0x40, 'kMvehBytes': 0x4D0, 'kMvehArena': 0x550, 'kMintOff': 0x40, 'kMintBytes': 0x100, 'kMintArena': 0x180}
ARENAS.update(oracle_vpc.ARENAS)
ARENAS.update(oracle_scan.ARENAS)
ARENAS.update(oracle_stuck.ARENAS)

EV = {'really_low': (52, 69), 'low': (53, 68), 'high_anger': (51, 34), 'low_anger': (50, 33)}


def ftol_byte(bits):
    """Byte bajo del int64 que deja _ftol2 (fistp con truncado): NaN, infinitos y |x| >= 2^63 dan el indefinido 0x8000000000000000 (byte bajo 0)."""
    x = struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]
    if math.isnan(x) or math.isinf(x) or abs(x) >= 2.0 ** 63:
        return 0
    return int(x) & 0xFF   # int() trunca hacia cero


def expected_mental(inp, case):
    st = bytearray(0x14)
    st[0] = inp['anger'] & 0xFF
    st[1] = inp['last_anger'] & 0xFF
    st[0x10] = inp['ped_hp'] & 0xFF
    st[0x11] = inp['old_ped_hp'] & 0xFF
    st[0x12] = inp['veh_hp'] & 0xFF
    st[0x13] = inp['old_veh_hp'] & 0xFF
    in_veh = bool(inp['in_vehicle'])
    st[0x10] = ftol_byte(inp['ped_health_bits'])
    veh_updated = in_veh and not inp['veh_null']
    if veh_updated:
        st[0x12] = ftol_byte(inp['veh_health_bits'])
    events = []
    paths = set()

    def add(kind):
        t, p = EV[kind]
        events.append(['add', 'I+104', t, p, 0, 0, 1, 1, 0xC8, 0xFFFF, 0])
        paths.add(kind)
    old = st[0x11]
    new = st[0x10]
    if not in_veh:
        if old >= 0x32:
            if new < 0x0A:
                add('really_low')
            elif new < 0x32:
                add('low')
        elif old >= 0x0A and new < 0x0A:
            add('really_low')
    last = st[1]
    anger = st[0]
    if last <= 3:
        if anger > 6:
            add('high_anger')
        elif anger > 3:
            add('low_anger')
    elif last <= 6 and anger > 6:
        add('high_anger')
    st[1] = st[0]
    st[0x11] = st[0x10]
    st[0x13] = st[0x12]
    pb = inp['ped_health_bits']
    nan_inf = (pb & 0x7F800000) == 0x7F800000
    stats = {'ev_really_low': 'really_low' in paths, 'ev_low': 'low' in paths, 'ev_high_anger': 'high_anger' in paths, 'ev_low_anger': 'low_anger' in paths,
             'no_events': not events, 'in_vehicle': in_veh, 'vehicle_updated': veh_updated, 'vehicle_null_in_vehicle': in_veh and bool(inp['veh_null']),
             'two_events': len(events) >= 2, 'ped_hp_nan_inf': nan_inf,
             'ped_hp_negative': bool(pb & 0x80000000) and not nan_inf,
             'ped_hp_ge256': (pb & 0x7FFFFFFF) >= 0x43800000 and not nan_inf,
             'ev_real_low_from_mid': 'really_low' in paths and old < 0x32}
    return st, events, stats


def expected(function, inp, case):
    """Devuelve (observed_parcial, stats). `observed_parcial` lleva `events`, `nevents` y `state_bytes` (solo los bytes del estado que el oraculo conoce)."""
    if function == NAME_MENTAL:
        st, events, stats = expected_mental(inp, case)
        return {'events': events, 'nevents': len(events), 'state_known': {str(i): st[i] for i in (0, 1, 0x10, 0x11, 0x12, 0x13)}}, stats
    if function == NAME_VPC:
        return oracle_vpc.expected(inp, case)
    if function == NAME_SCAN:
        return oracle_scan.expected(inp, case)
    if function == NAME_STUCK:
        return oracle_stuck.expected(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def check_observed(function, observed, want, inp):
    """Compara lo que registro un lado con lo esperado; devuelve la lista de campos distintos."""
    bad = []
    for k in ('events', 'nevents'):
        if observed.get(k) != want.get(k):
            bad.append(k)
    if function == NAME_MENTAL:
        got = bytes.fromhex(observed['state'])
        if len(got) != 0x14 or any(got[int(i)] != v for i, v in want['state_known'].items()):
            bad.append('state')
        if observed.get('state_guards_ok') is not True:
            bad.append('state_guards_ok')
        if observed.get('others_intact') is not True:
            bad.append('others_intact')
    if function == NAME_VPC:
        got = bytes.fromhex(observed['timer'])
        if len(got) != 12 or any(got[int(i)] != v for i, v in want['timer_known'].items()):
            bad.append('timer')
        if observed.get('timer_guards_ok') is not True:
            bad.append('timer_guards_ok')
        if observed.get('others_intact') is not True:
            bad.append('others_intact')
    if function == NAME_SCAN:
        for k in ('ped_changes', 'dead_timer', 'time_after'):
            if observed.get(k) != want.get(k):
                bad.append(k)
        for k in ('ped_guards_ok', 'others_intact'):
            if observed.get(k) is not True:
                bad.append(k)
    if function == NAME_STUCK:
        for k in ('ret', 'events', 'nevents', 'mat_changes'):
            if observed.get(k) != want.get(k):
                bad.append(k)
        if observed.get('checker') != want.get('checker_known'):
            bad.append('checker')
        got = list(observed.get('ped_changes', []))
        if want.get('rot') is not None:
            got = [c for c in got if not (0x558 <= c[0] <= 0x55F)]
            if observed.get('rot') != want['rot']:
                bad.append('rot')
        if got != want.get('ped_changes'):
            bad.append('ped_changes')
        for k in ('checker_guards_ok', 'ped_guards_ok', 'mat_guards_ok', 'globals_unchanged', 'others_intact'):
            if observed.get(k) is not True:
                bad.append(k)
    return bad


if __name__ == '__main__':
    def base(**kw):
        d = dict(family=0, anger=0, last_anger=0, ped_hp=0, old_ped_hp=0, veh_hp=0, old_veh_hp=0, ped_health_bits=0x42C80000, veh_health_bits=0x447A0000, in_vehicle=0, veh_null=0,
                 flags_noise=0, timer_fill=0, eax=0, edx=0, stack_seed=0)
        d.update(kw)
        return d
    st, ev, s = expected_mental(base(old_ped_hp=100), 0)
    assert st[0x10] == 100 and ev == [] and st[0x11] == 100
    st, ev, s = expected_mental(base(old_ped_hp=60, ped_health_bits=0x40A00000), 0)
    assert ev == [['add', 'I+104', 52, 69, 0, 0, 1, 1, 0xC8, 0xFFFF, 0]] and s['ev_really_low']
    st, ev, s = expected_mental(base(old_ped_hp=60, ped_health_bits=0x41A00000), 0)
    assert ev[0][2] == 53 and s['ev_low']
    assert expected_mental(base(old_ped_hp=30, ped_health_bits=0x40A00000), 0)[1][0][2] == 52
    assert expected_mental(base(old_ped_hp=9, ped_health_bits=0x40A00000), 0)[1] == []
    assert expected_mental(base(ped_health_bits=0x7FC00000), 0)[0][0x10] == 0
    assert expected_mental(base(ped_health_bits=0xBF800000), 0)[0][0x10] == 0xFF
    assert expected_mental(base(ped_health_bits=0x43800000), 0)[0][0x10] == 0
    assert expected_mental(base(ped_health_bits=0x5F000000), 0)[0][0x10] == 0
    st, ev, s = expected_mental(base(in_vehicle=1, veh_hp=7, old_ped_hp=60, ped_health_bits=0x40A00000), 0)
    assert st[0x12] == (1000 & 0xFF) and ev == [] and st[0x13] == st[0x12]
    st, ev, s = expected_mental(base(in_vehicle=1, veh_null=1, veh_hp=7), 0)
    assert st[0x12] == 7 and s['vehicle_null_in_vehicle']
    assert expected_mental(base(last_anger=0, anger=7), 0)[1][0][2] == 51
    assert expected_mental(base(last_anger=3, anger=4), 0)[1][0][2] == 50
    assert expected_mental(base(last_anger=3, anger=3), 0)[1] == []
    assert expected_mental(base(last_anger=4, anger=7), 0)[1][0][2] == 51
    assert expected_mental(base(last_anger=6, anger=7), 0)[1][0][2] == 51
    assert expected_mental(base(last_anger=7, anger=200), 0)[1] == []
    assert expected_mental(base(last_anger=4, anger=6), 0)[1] == []
    st, ev, s = expected_mental(base(old_ped_hp=60, ped_health_bits=0x40A00000, last_anger=0, anger=7), 0)
    assert [e[2] for e in ev] == [52, 51] and s['two_events']
    assert oracle_vpc.selftest()
    assert oracle_scan.selftest()
    assert oracle_stuck.selftest()
    print('ok')
