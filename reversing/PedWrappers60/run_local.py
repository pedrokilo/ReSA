"""Compuertas independientes del banco PedWrappers60 (CPed::ProcessControl 0x5E8CD0, SpecialEntityPreCollisionStuff 0x5E3C30, SpecialEntityCalcCollisionSteps 0x5E3E90), 14336 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: los tres hooks virtuales registrados y con cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato), fp_difference vacio, la salida de CADA lado (retorno, traza de los falsos en orden, todos los cambios de
bytes en las arenas, guardas incluidas, bools, estado de rand del EXE, globales) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion
final (hooks, entradas vigiladas, rand, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
HOOKS = dict(oracle.ADDR)
# colaboradores sustituidos por el mismo falso en ambos lados (orden de Stubs::All en Core.hpp); solo existen en el EXE los dos de RenderWare
STUBBED = [0x4E2EE0, 0x601070, 0x601180, 0x734A40, 0x7C51A0, 0x7C5120, 0x4A9BE0, 0x4AA910, 0x4AA890, 0x4AA2F0, 0x4A9810, 0x56E010, 0x571A90, 0x732B20, 0x732B00, 0x4185C0, 0x56E210, 0x5F6A10, 0x5E1FA0,
           0x6073A0, 0x608260, 0x5433B0, 0x5485E0, 0x5E8910, 0x5E57F0, 0x5E4C50, 0x5E1B10, 0x56D230, 0x706F60, 0x611570, 0x5EFFE0, 0x4E69F0, 0x4E6AE0]
EXE_ONLY = {0x7C51A0, 0x7C5120}
WATCHED = set(STUBBED) | {0x5E3E90, 0x5E3C30, 0x5E8CD0, 0x821B1E, 0x821B11, 0x827B3D}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    'CPed::SpecialEntityCalcCollisionSteps': {
        'early_attached': 248,
        'early_slow': 330,
        'family0': 256,
        'family1': 256,
        'family10': 256,
        'family11': 256,
        'family12': 256,
        'family13': 256,
        'family14': 256,
        'family15': 256,
        'family2': 256,
        'family3': 256,
        'family4': 256,
        'family5': 256,
        'family6': 256,
        'family7': 256,
        'family8': 256,
        'family9': 256,
        'mode0': 498,
        'mode1': 381,
        'mode2': 384,
        'ret_ge2': 1087,
    },
    'CPed::SpecialEntityPreCollisionStuff': {
        'attached_to_other': 73,
        'both_attached': 34,
        'family0': 256,
        'family1': 256,
        'family10': 256,
        'family11': 256,
        'family12': 256,
        'family13': 256,
        'family14': 256,
        'family15': 256,
        'family2': 256,
        'family3': 256,
        'family4': 256,
        'family5': 256,
        'family6': 256,
        'family7': 256,
        'family8': 256,
        'family9': 256,
        'flag_set': 793,
        'ig_garbage': 185,
        'ignored': 149,
        'ignored_airborne': 31,
        'im_ignore': 65,
        'im_stuck': 45,
        'inf_mass': 165,
        'lamp_fallen': 148,
        'mf_ignore': 37,
        'mf_stuck': 35,
        'mf_unable': 136,
        'model_156_below': 44,
        'model_ignored': 82,
        'move_force': 234,
        'not_object': 425,
        'object': 646,
        'object_still': 20,
        'object_stuck': 135,
        'other_attached_to_me': 42,
        'other_stuck': 111,
        'tail_climb_flag': 444,
        'uproot_or_dcf': 300,
        'veh_knocked': 70,
    },
    'CPed::ProcessControl': {
        'attack_wave': 162,
        'attack_wave_area_exit': 46,
        'attack_wave_high_exit': 20,
        'bleed_frame': 270,
        'bleed_gate': 1203,
        'bleed_near': 130,
        'bleed_noblood': 276,
        'bleed_outside': 884,
        'chainsaw': 194,
        'events_ge10': 2334,
        'events_ge20': 190,
        'fade_floor': 22,
        'fade_in': 2049,
        'fade_in_cap': 211,
        'fade_out': 128,
        'fall_decay': 26,
        'fall_player': 45,
        'fall_speed': 71,
        'family0': 384,
        'family1': 384,
        'family10': 384,
        'family11': 384,
        'family12': 384,
        'family13': 384,
        'family14': 384,
        'family15': 384,
        'family2': 384,
        'family3': 384,
        'family4': 384,
        'family5': 384,
        'family6': 384,
        'family7': 384,
        'family8': 384,
        'family9': 384,
        'gunflash': 1246,
        'gunflash_sub': 644,
        'gunflash_zero': 619,
        'head_check': 84,
        'head_cleared': 44,
        'knocked_off': 217,
        'knocked_off_cleared': 18,
        'knocked_off_colliding': 20,
        'knocked_off_cond': 114,
        'knocked_off_noveh': 29,
        'knocked_off_slow': 68,
        'mission': 1201,
        'mission_member': 305,
        'molotov': 407,
        'molotov_create': 207,
        'molotov_destroy': 152,
        'molotov_fx_null': 57,
        'molotov_fx_ok': 149,
        'molotov_keep': 20,
        'molotov_swim': 45,
        'mutated': 291,
        'not_standing_clear': 1181,
        'pedcount_clear_invehicle': 38,
        'pedcount_vehicle': 71,
        'player_data': 1773,
        'postponed_safe': 258,
        'postponed_train': 55,
        'prune': 318,
        'replay_exit': 67,
        'say': 57,
        'skip_physics': 30,
        'standing_on': 511,
        'wet_dec': 849,
        'wet_inc': 53,
    },
}


def pad(ev):
    return list(ev) + [0] * (oracle.EVENT_WIDTH - len(ev))


def speed_close(got, exp):
    """pow del CRT: el oraculo usa el de Python (no coincide siempre al ulp): 2 ulp de tolerancia (NaN con NaN, cero con cero exactos). EXE == DLL se exige en bits aparte."""
    for a, b in zip(got, exp):
        if (a & 0x7FFFFFFF) > 0x7F800000 and (b & 0x7FFFFFFF) > 0x7F800000:
            continue
        if (a & 0x7FFFFFFF) == 0 or (b & 0x7FFFFFFF) == 0 or (a >> 31) != (b >> 31):
            if a != b:
                return False
            continue
        if abs((a & 0x7FFFFFFF) - (b & 0x7FFFFFFF)) > 2:
            return False
    return True


def compare_side(function, row, side, want, label):
    got = side['observed']
    if function == oracle.NAME_CS:
        checks = {'ret': (got['ret'], want['ret']), 'changes': (got['changes'], want['changes']), 'bools': (got['bools'], [row['input']['bools'][0] & 0xFF, row['input']['bools'][1] & 0xFF]),
                  'bools_unchanged': (got['bools_unchanged'], True)}
    elif function == oracle.NAME_PS:
        checks = {'events': (got['events'], [pad(e) for e in want['events']]), 'changes': (got['changes'], want['changes']), 'bools': (got['bools'], want['bools']),
                  'others_unchanged': (got['others_unchanged'], True), 'ret': (got['ret'], None)}
    else:
        checks = {'events': (got['events'], [pad(e) for e in want['events']]), 'changes': (got['changes'], want['changes']), 'rand_calls': (got['rand_calls'], want['rand_calls']),
                  'others_unchanged': (got['others_unchanged'], True), 'globals_unchanged': (got['globals_unchanged'], True), 'ret': (got['ret'], None)}
    for key, (g, w) in checks.items():
        if g != w:
            raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campo %s:\n  obtenido %s\n  esperado %s' % (function, row['case'], label, key, json.dumps(g)[:1500], json.dumps(w)[:1500]))
    if function == oracle.NAME_PC and not speed_close(got['speed'], want['speed']):
        raise RuntimeError('Velocidad distinta de la del oraculo en %s %d (%s): obtenido %s, esperado %s (tolerancia 2 ulp)' % (function, row['case'], label, got['speed'], want['speed']))


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
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 3:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(NAMES) or not all(b['registered'].values()) or set(b['bodies']) != set(NAMES) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    stubbed = b['collaborators']['stubbed']
    if [s['exe'] for s in stubbed] != STUBBED or {w['address'] for w in b['watched_entries']} != WATCHED or sorted(b['collaborators']['exe_real']) != sorted([0x406DA0, 0x40FE60, 0x40FEC0, 0x821B1E]):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if any(s['dll'] for s in stubbed if s['exe'] in EXE_ONLY) or any(not s['dll'] for s in stubbed if s['exe'] not in EXE_ONLY):
        raise RuntimeError('Cuerpos de la DLL de los colaboradores inesperados (los dos de RenderWare no existen en la DLL; el resto si)')
    seen = {n: set() for n in NAMES}
    cov = {n: {} for n in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        compare_side(fn, row, a, want, 'EXE')
        compare_side(fn, row, c, want, 'DLL')
        if fn == oracle.NAME_PC and a['observed']['speed'] != c['observed']['speed']:
            # NaN: la carga util puede diferir entre el pow del EXE y el de la DLL; el resto (incluido el signo del cero) debe coincidir en bits
            for x, y in zip(a['observed']['speed'], c['observed']['speed']):
                if x != y and not ((x & 0x7FFFFFFF) > 0x7F800000 and (y & 0x7FFFFFFF) > 0x7F800000):
                    raise RuntimeError('EXE y DLL difieren en bits en la velocidad en %s %d: %s != %s' % (fn, case, a['observed']['speed'], c['observed']['speed']))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov[fn]['residue_differs'] = cov[fn].get('residue_differs', 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = {}
    for fn in NAMES:
        for key, m in MINIMUMS.get(fn, {}).items():
            if cov[fn].get(key, 0) < m:
                shortfalls['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['globals_intact'] is not True or state[0]['state_restored'] is not True
            or state[0]['rand_state'] != b['initial_rand_state']):
        raise RuntimeError('El x87, los hooks, las entradas vigiladas, rand o los globales no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('pedwrappers60', manifest['binaries']['asi'], analyze, 'PedWrappers60', timeout=1800))
