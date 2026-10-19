"""Compuertas independientes del banco FightProcessPed31 (CTaskSimpleFight::ProcessPed 0x629920), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x629920 salte a la DLL solo en el candidato), fp_difference vacio, la salida de CADA lado (AL de retorno, traza de los falsos en orden:
GetEventType x1/x2, SetDeleteCallback de la anim y del reposo, BlendAnimation con los bits de los argumentos, IsPlayer, FindTargetOnGround; y todos los cambios de bytes en `this`, las dos
asociaciones, el ped, los datos del jugador y el evento, guardas incluidas) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion
final (hook, entradas vigiladas, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x629920}
FN = oracle.NAME
WATCHED = {0x4CEBC0, 0x4D4610, 0x4CEA80, 0x5DF8F0, 0x5DEC00, 0x4D3FB0, 0x4D3FD0, 0x4D3A30, 0x4D3940, 0x4087E0, 0x743C60, 0x4E2BB0, 0x4E69F0, 0x6240B0, 0x61C7F0, 0x61C9B0, 0x624710, 0x624A40, 0x623B10,
           0x609560, 0x540670}
EXE_ONLY = {0x61C7F0, 0x61C9B0, 0x624710, 0x624A40, 0x623B10}      # sin reconstruir: solo existe la entrada del EXE

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'br_aim_camera': 343,
        'br_aim_camera_off': 86,
        'br_aim_none': 547,
        'br_aim_pad_no_target': 66,
        'br_aim_target': 580,
        'br_aim_target_matrix': 529,
        'br_aim_target_nomatrix': 50,
        'br_anim': 870,
        'br_avail_minus1': 513,
        'br_block_by_name': 34,
        'br_block_loaded': 68,
        'br_block_not_loaded': 70,
        'br_chain_m01_ai': 8,
        'br_chain_m01_player': 14,
        'br_chain_m3_ai': 3,
        'br_chain_m3_other': 4,
        'br_chain_m3_start': 7,
        'br_chain_m4_ai': 13,
        'br_chain_m4_player': 7,
        'br_chain_m4_settime': 6,
        'br_chain_m_other': 5,
        'br_chain_next': 70,
        'br_chain_nonext': 47,
        'br_chain_time': 118,
        'br_chainsaw': 155,
        'br_combo_lt4': 244,
        'br_counter_add': 536,
        'br_counter_indefinite': 44,
        'br_counter_reset': 1254,
        'br_counter_wrap': 31,
        'br_finished': 282,
        'br_finished_blend': 117,
        'br_finished_noblend': 25,
        'br_group_21': 56,
        'br_group_differs': 98,
        'br_group_same_referenced': 38,
        'br_group_same_unreferenced': 40,
        'br_hit_branch': 473,
        'br_hit_combo5_alt': 27,
        'br_hit_cross': 127,
        'br_hit_level7': 36,
        'br_hit_move4_adjust': 16,
        'br_hit_next_cleared': 56,
        'br_hit_nothing': 65,
        'br_hit_strike': 91,
        'br_hit_weight_low': 162,
        'br_hit_weight_ok': 311,
        'br_idle_combo_style': 109,
        'br_idle_create': 292,
        'br_idle_create_moving_nonplayer': 101,
        'br_idle_last_finish': 151,
        'br_idle_present': 1086,
        'br_idle_skip': 93,
        'br_last2': 152,
        'br_last2_blendout': 18,
        'br_last2_notplaying': 23,
        'br_last2_playing_nostop': 29,
        'br_last2_release': 40,
        'br_last2_stop': 22,
        'br_lt4_blendout': 13,
        'br_lt4_last3_blendout': 22,
        'br_lt4_last3_next3': 16,
        'br_next_rewritten': 12,
        'br_noanim': 753,
        'br_noanim_ai_attack': 319,
        'br_noanim_command': 585,
        'br_noanim_nocommand': 167,
        'br_noanim_player_attack': 233,
        'br_noanim_player_find': 33,
        'br_noanim_player_idle_next1': 30,
        'br_nocontrol_anim2f': 14,
        'br_nocontrol_next_lt1': 152,
        'br_nocontrol_pass': 112,
        'br_release_ref': 27,
        'br_row_flag': 234,
        'br_row_noflag': 58,
        'events_ge6': 551,
        'ret_false': 1791,
        'ret_true': 282,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288


def aim_close(got, exp):
    """El oraculo no modela atan2 al ulp: tolerancia de 2 ulp (mismo signo; los ceros deben coincidir exactamente). EXE == DLL se exige en bits aparte."""
    if got is None or exp is None:
        return got is exp
    if (got & 0x7FFFFFFF) == 0 or (exp & 0x7FFFFFFF) == 0:
        return got == exp
    if (got >> 31) != (exp >> 31):
        return False
    return abs((got & 0x7FFFFFFF) - (exp & 0x7FFFFFFF)) <= 2


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
        raise RuntimeError('Inicio sin hook registrado, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    stubbed = b['collaborators']['stubbed']
    if {w['address'] for w in b['watched_entries']} != WATCHED or {s['exe'] for s in stubbed} != WATCHED or len(stubbed) != len(WATCHED) or b['collaborators']['exe_real']:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if any(not s['dll'] for s in stubbed if s['exe'] not in EXE_ONLY) or any(s['dll'] for s in stubbed if s['exe'] in EXE_ONLY):
        raise RuntimeError('Cuerpos de la DLL de los colaboradores inesperados (los cinco del EXE no existen en la DLL; el resto si)')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != FN or case in seen:
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
            got = dict(side['observed'])
            exp = dict(want)
            got_aim, exp_aim = got.pop('aim'), exp.pop('aim')
            if got != exp:
                diff = [k for k in exp if got.get(k) != exp[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s: obtenido %s, esperado %s' % (fn, case, label, diff, side['observed'], want))
            if not aim_close(got_aim, exp_aim):
                raise RuntimeError('Angulo de orientacion distinto en %s %d (%s): obtenido %r, esperado %r (tolerancia 2 ulp)' % (fn, case, label, got_aim, exp_aim))
        if a['observed']['aim'] != c['observed']['aim']:
            raise RuntimeError('EXE y DLL difieren en bits en el angulo de orientacion en %s %d' % (fn, case))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    shortfalls = {}
    for key, m in MINIMUMS[FN].items():
        if cov.get(key, 0) < m:
            shortfalls['%s:%s' % (FN, key)] = (cov.get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['globals_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook, la entrada vigilada o la ruta no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fightprocessped31', manifest['binaries']['asi'], analyze, 'FightProcessPed31', timeout=1800))
