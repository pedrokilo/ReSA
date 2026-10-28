"""Compuertas del banco HeliUpdate78 (lote 78C): CHeli::UpdateHelis (EXE 0x6C79A0, hook S), 21504 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la igualdad de ambos lados con el oraculo independiente (fixture_oracle.py: modelo Python de la funcion, con aritmetica x87 a 24 bits y memoria de las arenas),
la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CHeli::UpdateHelis']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C79A0}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE o no hay hook
STUBBED = {NAMES[0]: [(0x56E230, True), (0x561FA0, True), (0x72DDC0, True), (0x56E210, True), (0x6C6520, True), (0x571B70, True), (0x56E010, True), (0x563280, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C79A0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('player_x', 'player_y', 'player_z') + tuple('s%d_%s' % (i, a) for i in range(2) for a in 'xyz')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    oracle.expected(inp)
    L = oracle.LAST

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('count_0', L['count'] == 0)
    add('count_1', L['count'] == 1)
    add('count_2', L['count'] == 2)
    add('flag_police_heli', L['flag_police'])
    add('news_heli_clears_flag', L['news_clears'])
    add('use_news_with_flag', L['use_news_flag'])
    add('use_news_without_flag', not L['use_news_flag'])
    add('zero_norain', L['zero_norain'])
    add('zero_area', L['zero_area'])
    add('zero_allowed', L['zero_allowed'])
    add('zero_weather_old', L['zero_weather_old'])
    add('zero_weather_new', L['zero_weather_new'])
    add('weather_16bit_garbage', ((inp['w_old'] >> 16) != 0 and (inp['w_old'] & 0xFFFF) == 0x13) or ((inp['w_new'] >> 16) != 0 and (inp['w_new'] & 0xFFFF) == 0x13))
    add('weather_13_in_32bit_only', (inp['w_old'] & 0xFFFF) != 0x13 and (inp['w_old'] >> 16) == 0x13 or (inp['w_new'] & 0xFFFF) != 0x13 and (inp['w_new'] >> 16) == 0x13)
    add('max_negative', L['max'] < 0)
    add('max_zero', L['max'] == 0)
    add('max_positive', L['max'] > 0)
    add('spawn_allowed_byte_ne1', not L['ok'] and (inp['b73a4'] != 1 or inp['b72f0'] != 1))
    add('timer_elapsed', L['elapsed'])
    add('timer_not_elapsed', not L['elapsed'])
    add('timer_equal', inp['now'] == inp['test'])
    add('timer_wrap_extreme', inp['now'] in (0, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF))
    add('spawn_try', L['spawn_try'])
    add('spawn_try_no_cap', L['spawn_try'] and not L['spawn_cap'])
    add('spawned', L['spawned'])
    add('spawned_flag_true', L['spawned'] and L['flag_passed'])
    add('spawned_flag_false', L['spawned'] and not L['flag_passed'])
    add('spawned_slot0', L['spawn_slot'] == 0)
    add('spawned_slot1', L['spawn_slot'] == 1)
    add('spawned_leaked', L['spawn_slot'] == 2)
    for item in L['loop']:
        add('loop_' + item[0])
        if item[0] == 'flagged':
            add('flagged_flags40_only', item[1] and not item[2])
            add('flagged_b42b_only', item[2] and not item[1])
            add('flagged_both', item[1] and item[2])
        if item[0] in ('far', 'near'):
            add('dist_nan', item[1])
            add('dist_matrix', item[3] == 1)
            add('dist_compact', item[3] == 0)
            d = item[2]
            add('dist_exact_170', d is not None and d == 170)
            add('dist_inside_ulp', d is not None and 169.99 < d < 170)
            add('dist_outside_ulp', d is not None and 170 < d < 170.01)
            add('dist_infinite', d is not None and d > 1e30)
    for t in L['tail']:
        add('tail_' + t)
    add('new_heli_in_loop', 3 in L['ptrs'] and any(i[0] != 'keep_mission' for i in L['loop']) and L['spawned'])
    add('lights_dirty_initial', inp['lights'] > 4)
    add('special_any', any(is_special(inp[a]) for a in FLOATS))
    add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('negzero_input', any(inp[a] == 0x80000000 for a in FLOATS))


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b.get('layout_ok') is not True:
        raise RuntimeError('La disposicion de los campos de bits / accesores no es la esperada')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn + ': ' + str(got))
        # los colaboradores con hook RH tienen su cuerpo de la DLL parcheado con el mismo falso
        if any(s['hooked'] and s['dll'] is not True for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
        why, _ = oracle.check_row(row)
        if why:
            raise RuntimeError('El oraculo independiente discrepa en %s %d: %s' % (fn, case, why[:3]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('heliupdate78', manifest['binaries']['asi'], analyze, 'HeliUpdate78', timeout=3600))
