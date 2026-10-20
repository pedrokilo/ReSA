"""Compuertas independientes del banco KillPedOnFoot62 (CTaskComplexKillPedOnFoot: ControlSubTask 0x626260, CreateFirstSubTask 0x62B490, CreateNextSubTask 0x62B150, CreateSubTask 0x625E70,
MakeAbortable 0x625E40), 5120 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los cinco hooks registrados (cuatro virtuales y uno simple) con cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/
restauracion en AMBOS lados (route_ok incluye que la entrada salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos con hook en HEAD sean saltos a la DLL en
ambos lados y los sin hook, codigo del EXE, antes de instalar los falsos, que los constructores/ayudantes reales sigan siendolo, que las OTRAS cuatro funciones de la clase y el 0x407180 original
estén apagados), fp_difference vacio, MakeAbortable igual a su modelo del desensamblado, la cobertura de las fixtures (claves observadas, ver fixture_oracle.classify) y la restauracion final.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = oracle.NAMES
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(oracle.ENTRY)
STUBBED = (0x56E230, 0x56E210, 0x72DD50, 0x73B1C0, 0x5EFFE0, 0x5E6080, 0x5E6280, 0x5E61F0, 0x5DEC00, 0x5E0360, 0x6D1E20, 0x6D1C40, 0x6D1BD0, 0x64F110, 0x6504C0, 0x64EEC0, 0x5F7E80, 0x5F69A0,
           0x61A5A0, 0x62F370, 0x4AB420, 0x65E9A0, 0x821B1E, 0)
STUB_HOOKED = [True] * 21 + [False, False, False]
REAL_HOOKED = (0x5DF8F0, 0x420E30, 0x62F310, 0x463540, 0x48E750, 0x621000, 0x621190, 0x5FF940, 0x621C00, 0x660A30, 0x4B2190)
REAL_EXE = (0x5FE9C0, 0x640430, 0x63B8C0, 0x4B2210)
WATCHED = {a for a in STUBBED if a} | set(REAL_HOOKED) | set(REAL_EXE) | {0x407180}

# Minimos de cobertura (claves de fixture_oracle.classify y ramas del modelo, contadas con lo observado en el lado EXE): calibrados con una corrida aprobada: el 35 % de lo medido (minimo 1).
MINIMUMS = {
    'ControlSubTask:abort_prio1': 496,
    'ControlSubTask:alloc_type_2bf': 21,
    'ControlSubTask:alloc_type_2c0': 10,
    'ControlSubTask:alloc_type_3e9': 9,
    'ControlSubTask:alloc_type_3ea': 19,
    'ControlSubTask:alloc_type_3eb': 31,
    'ControlSubTask:alloc_type_4ba': 9,
    'ControlSubTask:alloc_type_c8': 10,
    'ControlSubTask:alloc_type_ca': 91,
    'ControlSubTask:alloc_type_cb': 5,
    'ControlSubTask:ev_abort': 496,
    'ControlSubTask:ev_add': 110,
    'ControlSubTask:ev_c65': 374,
    'ControlSubTask:ev_canopen': 76,
    'ControlSubTask:ev_door': 76,
    'ControlSubTask:ev_drop': 24,
    'ControlSubTask:ev_give': 2,
    'ControlSubTask:ev_group': 7,
    'ControlSubTask:ev_healthy': 49,
    'ControlSubTask:ev_isdriver': 107,
    'ControlSubTask:ev_ispassenger': 29,
    'ControlSubTask:ev_leader': 4,
    'ControlSubTask:ev_melee': 126,
    'ControlSubTask:ev_movestate': 2,
    'ControlSubTask:ev_new': 208,
    'ControlSubTask:ev_nopolice': 9,
    'ControlSubTask:ev_playerped': 9,
    'ControlSubTask:ev_rand': 9,
    'ControlSubTask:ev_room': 76,
    'ControlSubTask:ev_say': 9,
    'ControlSubTask:ev_setwslot': 7,
    'ControlSubTask:ev_setwtype': 3,
    'ControlSubTask:ev_still': 91,
    'ControlSubTask:ev_type': 659,
    'ControlSubTask:ev_wanted': 84,
    'ControlSubTask:new_08': 10,
    'ControlSubTask:new_18': 100,
    'ControlSubTask:new_20': 36,
    'ControlSubTask:new_34': 10,
    'ControlSubTask:new_38': 9,
    'ControlSubTask:new_54': 21,
    'ControlSubTask:new_5c': 19,
    'ControlSubTask:ret_alloc': 208,
    'ControlSubTask:ret_null': 28,
    'ControlSubTask:ret_sub': 480,
    'ControlSubTask:wrote_alloc': 208,
    'ControlSubTask:wrote_ped': 367,
    'ControlSubTask:wrote_sub': 111,
    'ControlSubTask:wrote_this': 370,
    'CreateFirstSubTask:alloc_type_2bf': 32,
    'CreateFirstSubTask:alloc_type_2c0': 25,
    'CreateFirstSubTask:alloc_type_3e9': 46,
    'CreateFirstSubTask:alloc_type_3ea': 42,
    'CreateFirstSubTask:alloc_type_3eb': 109,
    'CreateFirstSubTask:ev_canopen': 92,
    'CreateFirstSubTask:ev_door': 80,
    'CreateFirstSubTask:ev_drop': 256,
    'CreateFirstSubTask:ev_give': 34,
    'CreateFirstSubTask:ev_isdriver': 164,
    'CreateFirstSubTask:ev_ispassenger': 40,
    'CreateFirstSubTask:ev_melee': 92,
    'CreateFirstSubTask:ev_movestate': 34,
    'CreateFirstSubTask:ev_new': 256,
    'CreateFirstSubTask:ev_nopolice': 182,
    'CreateFirstSubTask:ev_room': 80,
    'CreateFirstSubTask:ev_setwtype': 34,
    'CreateFirstSubTask:new_20': 109,
    'CreateFirstSubTask:new_34': 25,
    'CreateFirstSubTask:new_38': 46,
    'CreateFirstSubTask:new_54': 32,
    'CreateFirstSubTask:new_5c': 42,
    'CreateFirstSubTask:ret_alloc': 256,
    'CreateFirstSubTask:ret_null': 11,
    'CreateFirstSubTask:wrote_alloc': 256,
    'CreateFirstSubTask:wrote_ped': 6,
    'CreateFirstSubTask:wrote_this': 236,
    'CreateNextSubTask:alloc_type_2c0': 21,
    'CreateNextSubTask:alloc_type_2cf': 19,
    'CreateNextSubTask:alloc_type_3e9': 35,
    'CreateNextSubTask:alloc_type_3ea': 47,
    'CreateNextSubTask:alloc_type_3eb': 15,
    'CreateNextSubTask:alloc_type_ca': 11,
    'CreateNextSubTask:ev_drop': 10,
    'CreateNextSubTask:ev_give': 11,
    'CreateNextSubTask:ev_isdriver': 1,
    'CreateNextSubTask:ev_ispassenger': 1,
    'CreateNextSubTask:ev_melee': 68,
    'CreateNextSubTask:ev_movestate': 11,
    'CreateNextSubTask:ev_new': 150,
    'CreateNextSubTask:ev_nopolice': 47,
    'CreateNextSubTask:ev_setwtype': 11,
    'CreateNextSubTask:ev_still': 11,
    'CreateNextSubTask:ev_type': 257,
    'CreateNextSubTask:ev_wanted': 43,
    'CreateNextSubTask:new_18': 11,
    'CreateNextSubTask:new_20': 15,
    'CreateNextSubTask:new_34': 21,
    'CreateNextSubTask:new_38': 35,
    'CreateNextSubTask:new_5c': 47,
    'CreateNextSubTask:new_70': 19,
    'CreateNextSubTask:ret_alloc': 150,
    'CreateNextSubTask:ret_null': 117,
    'CreateNextSubTask:wrote_alloc': 150,
    'CreateNextSubTask:wrote_ped': 50,
    'CreateNextSubTask:wrote_this': 88,
    'CreateSubTask:alloc_type_2bf': 21,
    'CreateSubTask:alloc_type_2c0': 21,
    'CreateSubTask:alloc_type_2cf': 21,
    'CreateSubTask:alloc_type_3e9': 22,
    'CreateSubTask:alloc_type_3ea': 45,
    'CreateSubTask:alloc_type_3eb': 23,
    'CreateSubTask:alloc_type_4ba': 43,
    'CreateSubTask:alloc_type_c8': 22,
    'CreateSubTask:alloc_type_ca': 42,
    'CreateSubTask:alloc_type_cb': 22,
    'CreateSubTask:ev_group': 18,
    'CreateSubTask:ev_leader': 11,
    'CreateSubTask:ev_new': 299,
    'CreateSubTask:ev_playerped': 45,
    'CreateSubTask:ev_rand': 45,
    'CreateSubTask:ev_say': 43,
    'CreateSubTask:ev_still': 46,
    'CreateSubTask:new_08': 22,
    'CreateSubTask:new_18': 92,
    'CreateSubTask:new_20': 46,
    'CreateSubTask:new_34': 23,
    'CreateSubTask:new_38': 23,
    'CreateSubTask:new_54': 23,
    'CreateSubTask:new_5c': 45,
    'CreateSubTask:new_70': 23,
    'CreateSubTask:ret_alloc': 285,
    'CreateSubTask:ret_null': 72,
    'CreateSubTask:wrote_alloc': 285,
    'CreateSubTask:wrote_ped': 11,
    'CreateSubTask:wrote_this': 92,
    'MakeAbortable:abort_prio0': 29,
    'MakeAbortable:abort_prio1': 27,
    'MakeAbortable:abort_prio2': 31,
    'MakeAbortable:abort_prio3': 46,
    'MakeAbortable:ev_abort': 134,
    'MakeAbortable:ret_null': 66,
    'MakeAbortable:ret_other': 112,
    'MakeAbortable:wrote_ped': 91,
    'alloc_type_2bf': 74,
    'alloc_type_2c0': 79,
    'alloc_type_2cf': 40,
    'alloc_type_3e9': 112,
    'alloc_type_3ea': 156,
    'alloc_type_3eb': 179,
    'alloc_type_4ba': 52,
    'alloc_type_c8': 32,
    'alloc_type_ca': 145,
    'alloc_type_cb': 27,
    'branch:abort_no_sub': 44,
    'branch:abort_sub': 134,
    'branch:ctl_area_event': 110,
    'branch:ctl_area_null': 129,
    'branch:ctl_area_same': 295,
    'branch:ctl_armed': 58,
    'branch:ctl_armed_air': 1,
    'branch:ctl_armed_moving': 12,
    'branch:ctl_armed_noammo': 8,
    'branch:ctl_armed_noroom': 16,
    'branch:ctl_armed_room': 14,
    'branch:ctl_armed_switch_slot': 7,
    'branch:ctl_armed_to_melee': 26,
    'branch:ctl_armed_unarmed': 1,
    'branch:ctl_backoff_cop': 20,
    'branch:ctl_backoff_wanted': 166,
    'branch:ctl_change_refused': 198,
    'branch:ctl_change_task': 187,
    'branch:ctl_cop_chance': 43,
    'branch:ctl_dead': 47,
    'branch:ctl_dead_cop_say': 2,
    'branch:ctl_dead_leavegroup': 10,
    'branch:ctl_dead_refused': 23,
    'branch:ctl_dead_signal': 6,
    'branch:ctl_dead_signal_task': 8,
    'branch:ctl_default_sub': 95,
    'branch:ctl_destroy': 56,
    'branch:ctl_destroy_drag_flag': 13,
    'branch:ctl_destroy_noroom': 3,
    'branch:ctl_destroy_room': 2,
    'branch:ctl_destroy_timer': 4,
    'branch:ctl_destroy_unhealthy': 2,
    'branch:ctl_drag': 59,
    'branch:ctl_drag_moving': 13,
    'branch:ctl_drag_noroom': 11,
    'branch:ctl_drag_room': 8,
    'branch:ctl_drag_timer': 14,
    'branch:ctl_melee': 52,
    'branch:ctl_melee_air': 1,
    'branch:ctl_melee_locked': 1,
    'branch:ctl_melee_noroom': 8,
    'branch:ctl_melee_room': 10,
    'branch:ctl_melee_to_armed': 27,
    'branch:ctl_new_target': 43,
    'branch:ctl_new_target_refused': 18,
    'branch:ctl_pause': 70,
    'branch:ctl_pause_abort': 11,
    'branch:ctl_tgt_null': 57,
    'branch:ctl_timeout': 11,
    'branch:ctl_timeout_refused': 6,
    'branch:ctl_timer_restart': 26,
    'branch:ctl_to_pause': 187,
    'branch:ctl_waiting_set': 80,
    'branch:first_bike': 3,
    'branch:first_bike_armed': 1,
    'branch:first_boat': 1,
    'branch:first_dont_drag': 7,
    'branch:first_drag': 33,
    'branch:first_far_or_nan': 19,
    'branch:first_heli_plane': 9,
    'branch:first_locked': 8,
    'branch:first_moving': 42,
    'branch:first_near': 7,
    'branch:first_no_room': 40,
    'branch:first_nopolice': 100,
    'branch:first_on_foot': 101,
    'branch:first_ped_in_veh': 40,
    'branch:first_player': 193,
    'branch:first_stay': 5,
    'branch:first_tgt_null': 11,
    'branch:first_veh_member': 149,
    'branch:give_done': 48,
    'branch:give_mission': 35,
    'branch:give_not_gang': 28,
    'branch:give_pd_flag_off': 17,
    'branch:next_default': 12,
    'branch:next_drag_quit': 14,
    'branch:next_drag_weapon': 17,
    'branch:next_drive': 16,
    'branch:next_finish': 82,
    'branch:next_leave_in_veh': 19,
    'branch:next_leave_near': 2,
    'branch:next_leave_nopolice': 22,
    'branch:next_pause_cop': 26,
    'branch:next_pause_finish': 11,
    'branch:next_pause_resume': 10,
    'branch:next_tgt_null': 10,
    'branch:signal_leader_differs': 12,
    'branch:signal_leader_equal': 3,
    'branch:signal_no_group': 10,
    'branch:signal_player': 29,
    'branch:sub_2bf': 23,
    'branch:sub_2c0': 23,
    'branch:sub_2cf': 23,
    'branch:sub_3e9': 23,
    'branch:sub_3ea': 45,
    'branch:sub_3eb': 24,
    'branch:sub_4ba': 45,
    'branch:sub_516': 22,
    'branch:sub_armed_aim': 75,
    'branch:sub_armed_noaim': 80,
    'branch:sub_c8': 22,
    'branch:sub_ca': 46,
    'branch:sub_cb': 22,
    'branch:sub_ffff': 35,
    'branch:sub_pause_long': 61,
    'branch:sub_pause_short': 87,
    'ev_abort': 631,
    'ev_add': 110,
    'ev_c65': 374,
    'ev_canopen': 168,
    'ev_door': 157,
    'ev_drop': 291,
    'ev_give': 48,
    'ev_group': 25,
    'ev_healthy': 49,
    'ev_isdriver': 272,
    'ev_ispassenger': 70,
    'ev_leader': 15,
    'ev_melee': 287,
    'ev_movestate': 48,
    'ev_new': 915,
    'ev_nopolice': 239,
    'ev_playerped': 54,
    'ev_rand': 54,
    'ev_room': 157,
    'ev_say': 53,
    'ev_setwslot': 7,
    'ev_setwtype': 49,
    'ev_still': 149,
    'ev_type': 917,
    'ev_wanted': 128,
    'new_08': 33,
    'new_18': 204,
    'new_20': 208,
    'new_34': 80,
    'new_38': 114,
    'new_54': 77,
    'new_5c': 156,
    'new_70': 42,
    'say_102': 22,
    'say_103': 29,
    'say_e': 2,
}


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    stubbed = col['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or col['dll_real_hooked'] != list(REAL_HOOKED) or col['exe_real'] != list(REAL_EXE)
            or [s['exe'] for s in stubbed] != list(STUBBED) or [s['hooked'] for s in stubbed] != STUB_HOOKED or not all(s['dll'] or not s['hooked'] for s in stubbed)):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    sel = oracle.selftest()
    if sel != 'ok':
        raise RuntimeError('Autoprueba del oraculo fallida')
    oracle.selftest({'arena': b['arena_bytes'], 'offsets': oracle.region_offsets()[0], 'cases': b['pairs'], 'route_cases': list(EXPECTED.values())})
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in EXPECTED or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            obs = side['observed']
            got = oracle.observed_view(fn, obs)
            if got != want:
                raise RuntimeError('Salida distinta del modelo en %s %d (%s): obtenido %s esperado %s entrada %s' % (fn, case, label, got, want, json.dumps(row['input'])))
            if any(r >= len(oracle.REGION_BYTES) or o >= oracle.REGION_BYTES[r] for r, o, v in obs['changes']):
                raise RuntimeError('Escritura fuera de una region (guarda) en %s %d (%s)' % (fn, case, label))
        for key, value in oracle.classify(fn, row['input'], a['observed']).items():
            cov[key] = cov.get(key, 0) + 1
        for key in stats:
            cov['branch:' + key] = cov.get('branch:' + key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(sum(EXPECTED.values()))):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = dict(sorted(cov.items()))
    summary['coverage_method'] = 'observed_events_and_independent_disassembly_selftest; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('killpedonfoot62', manifest['binaries']['asi'], analyze, 'KillPedOnFoot62', timeout=1800))
