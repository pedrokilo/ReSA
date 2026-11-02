"""Compuertas del banco Car84C (lote 84C): CCarCtrl::PickNextNodeToFollowPath (EXE 0x427740, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: eventos de DoPathSearch y CalcSpeedScaleFactor con todos sus argumentos, valor devuelto, estado del piloto y del
vehiculo, tablas del mundo de caminos, matriz, zona del clima, reloj y semilla de rand()), que ninguna fixture lea fuera de las tablas del banco, la cobertura por familia de las fixtures y la cobertura
de ramas medida sobre las entradas y el modelo (KEYS con minimo COVER_FLOOR y coverage.json = minimos calibrados). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el
registro aun no existe o no esta completo).
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

NAMES = ['CCarCtrl::PickNextNodeToFollowPath']
CASES = 18432   # 16384 (familias 0..15) + 2 x 1024 (familias 16 y 17)
FAMILIES = 18
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x427740}
# (direccion del EXE, hooked): CPathFind::DoPathSearch 0x4515D0 (hooked=False: el hook queda apagado, la entrada del EXE es codigo original; tiene hook RH en la DLL y lleva el mismo falso en el cuerpo de la DLL)
STUBBED = {NAMES[0]: [(0x4515D0, False), (0x43C710, False)]}
# funciones reales del EXE que corren de verdad (0x44DB00 desplazamiento del carril, srand 0x821B11, rand 0x821B1E, getptd 0x827B3D, _ftol2 0x821B40) o con el hook apagado (RemoveOnePathNode 0x41B950,
# SwitchVehicleToRealPhysics 0x423FC0) y constantes de .rdata (0.125f, 0.01f, 5.4f, 1000.0f, 256.0f): se vigilan sus bytes
EXTRA_WATCHED = {0x44DB00, 0x821B11, 0x821B1E, 0x827B3D, 0x821B40, 0x41B950, 0x423FC0, 0x858C48, 0x858C58, 0x858C50, 0x858C4C, 0x858FB4}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x427740} | EXTRA_WATCHED
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = ['entered', 'matrix', 'no_matrix', 'search', 'no_search', 'vortex', 'count_lt2_true', 'count0_true', 'removed_first', 'removed_second', 'index_0', 'index_n', 'below', 'not_below', 'dist_far',
        'dist_near', 'rand_pass', 'rand_skip', 'lane_inc', 'lane_dec', 'flag8', 'flag10', 'type2', 'type_other', 'area_not_loaded', 'scale_clamped', 'csf_special', 'speed_special', 'seed_srand',
        'mut_applied', 'nan_input', 'inf_input', 'denormal_input', 'negzero_input']
MUT_KINDS = ['mut_kind_%d' % i for i in range(1, 21)]
MUT_EVENT_KEYS = []


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    for m in set(e['marks']):
        add(m)
    flt = list(inp['dest']) + list(inp['mat']) + list(inp['place']) + [inp['speed'], inp['csf']]
    add('nan_input', any(is_special(x) and (x & 0x7FFFFF) != 0 for x in flt))
    add('inf_input', any(is_special(x) and (x & 0x7FFFFF) == 0 for x in flt))
    add('denormal_input', any(is_denormal(x) for x in flt))
    add('negzero_input', any(x == 0x80000000 for x in flt))


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
    if b.get('rng_native') is not True:
        raise RuntimeError('rand() del EXE no nativo')
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # DoPathSearch y CalcSpeedScaleFactor tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
        if any(s['dll'] is not True for s in col[fn]['stubbed']):
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
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS}
        need.update({k: 15 for k in MUT_KINDS})
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('car84c', manifest['binaries']['asi'], analyze, 'Car84C', timeout=3600))
