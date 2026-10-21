"""Compuertas del banco Helpers65D (lote 65: matriz * punto 0x59C890, CreateRouteTask 0x655140, RwTexDictionaryGtaStreamRead 0x730FC0 y FightStrikeDamageExe 0x61C740): 4096 pares por funcion.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio (el resultado float de FightStrikeDamageExe en ST0 forma parte del estado x87 comparado), cobertura por familia de las fixtures y cobertura de ramas
medida sobre las entradas y los eventos de los colaboradores (los falsos registran sus argumentos, en orden). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza
(devuelve None si el registro aun no existe o no esta completo).
"""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent

NAMES = ['operator*(CMatrix,CVector)', 'CTaskComplexWalkRoundObject::CreateRouteTask', 'RwTexDictionaryGtaStreamRead', 'FightStrikeDamageExe']
EXPECTED = {n: 4096 for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x59C890, NAMES[1]: 0x655140, NAMES[2]: 0x730FC0, NAMES[3]: 0x61C740}
STUBBED = {
    NAMES[0]: [],
    NAMES[1]: [(0x61A5A0, True), (0x41B7C0, False)],
    NAMES[2]: [(0x7ED2D0, False), (0x7EC9D0, False), (0x7F3600, False), (0x730E60, False), (0x7F3980, False), (0x7F3730, False), (0x7F36A0, False)],
    NAMES[3]: [(0x5DF8F0, True), (0x559AF0, True)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
READ_WORDS = (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)
# Cobertura minima (calibrada con la primera corrida; con margen): [funcion] -> {clave: minimo de casos}
COVER = {
    NAMES[0]: {'distinct': 2000, 'alias_1': 200, 'alias_2': 200, 'alias_3': 200, 'alias_4': 200, 'alias_5': 60, 'nan_read': 300, 'inf_read': 120, 'denormal_read': 150, 'cancellation': 100, 'negative_zero': 100},
    NAMES[1]: {'no_route': 400, 'built': 2000, 'task_null': 100, 'route_null': 100, 'move_nonstd': 400, 'nan_entries': 100},
    NAMES[2]: {'find_fail': 300, 'read_fail': 300, 'create_fail': 100, 'success': 1000, 'zero_count': 80, 'multi_tex': 400, 'tex_fail': 300, 'len_over_frame': 300, 'len_overwrites_length': 300},
    NAMES[3]: {'player_fixed': 200, 'player_stat': 500, 'weapon_fist': 300, 'weapon_knuckle': 300, 'weapon_other': 300, 'stat_nan': 30, 'attack_nan': 80, 'huge_product': 50, 'tiny_product': 10},
}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def kinds(side):
    return [e[0] for e in side['observed'].get('events', [])]


def tally(fn, row, cov):
    inp = row['input']
    cand = row['candidate']
    k = kinds(cand) if 'events' in cand['observed'] else []

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    if fn == NAMES[0]:
        vals = [inp['m'][i] for i in READ_WORDS] + inp['p']
        add('distinct', inp['alias'] == 0)
        for a in range(1, 6):
            add('alias_%d' % a, inp['alias'] == a)
        add('nan_read', any(is_nan(v) for v in vals))
        add('inf_read', any(is_inf(v) for v in vals))
        add('denormal_read', any(is_denormal(v) for v in vals))
        add('cancellation', inp['family'] == 3)
        add('negative_zero', any(v == 0x80000000 for v in vals))
    elif fn == NAMES[1]:
        built = cand['observed']['eax'] != 0
        add('no_route', inp['num_entries'] == 0)
        add('built', built)
        add('task_null', inp['num_entries'] > 0 and 1 in k and not built and 2 not in k)
        add('route_null', built and 2 in k and inp['route_null'] == 1)
        add('move_nonstd', inp['move_state'] > 7)
        add('nan_entries', any(is_nan(v) for v in inp['entries'][:3 * inp['num_entries']]))
    elif fn == NAMES[2]:
        ev = cand['observed']['events']
        add('find_fail', inp['find_ret'] == 0)
        add('read_fail', 11 in k and 12 not in k)
        add('create_fail', 12 in k and inp['create_ret'] == 0)
        add('success', cand['observed']['eax'] != 0)
        add('zero_count', cand['observed']['eax'] != 0 and 14 not in k)
        add('multi_tex', k.count(14) >= 2)
        add('tex_fail', 16 in k)
        add('len_over_frame', inp['find_ret'] != 0 and inp['chunk_len'] > 12)
        add('len_overwrites_length', inp['find_ret'] != 0 and 5 <= inp['chunk_len'] <= 12)
    elif fn == NAMES[3]:
        ret = [e for e in cand['observed']['events'] if e[0] == 20]
        player = bool(ret) and (inp['is_player_ret'] & 0xFF) != 0
        add('player_fixed', player and (inp['player_byte'] & 2) != 0)
        add('player_stat', player and 21 in k)
        wt = None
        if not player:
            wt = inp['weapon_type']
        if not player:
            add('weapon_fist', wt == 0)
            add('weapon_knuckle', wt == 1)
            add('weapon_other', wt not in (0, 1))
        add('stat_nan', 21 in k and is_nan(inp['stat']))
        add('attack_nan', not player and is_nan(inp['attack']))
        st0 = cand['observed']['st0']
        exp = ((int(st0[18:20], 16) << 8) | int(st0[16:18], 16)) & 0x7FFF   # exponente de 15 bits del registro de 80 bits (bytes 8 y 9, little endian)
        add('huge_product', 0x407E < exp < 0x7FFF)   # por encima de 2^127: fuera del rango de float, dentro del de la x87
        add('tiny_product', 0 < exp < 0x3F81)         # por debajo de 2^-126: subnormal como float, normal en la x87


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
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if col[fn]['dll_real'] or got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(fn, row, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(4096)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 16 or short:
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
        sys.exit(runtime_runner.run_case('helpers65d', manifest['binaries']['asi'], analyze, 'Helpers65D', timeout=3600))
