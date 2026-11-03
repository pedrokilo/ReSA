"""Compuertas independientes del banco Fn86C (lote 86C): CPathFind::GeneratePedCreationCoors (EXE 0x44E790; hook S), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado coincida con el oraculo independiente (fixture_oracle.py: valor devuelto, salidas, eventos de los falsos con todos sus argumentos y cambios de las arenas), la cobertura por familia y la
cobertura de ramas medida sobre las entradas y el modelo (REQUIRED con minimo y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo)."""
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

NAMES = ['CPathFind::GeneratePedCreationCoors']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x44E790}
# (direccion del EXE, hooked): con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook (hooked False) la DLL llama a la direccion del EXE
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x5696C0, True), (0x420C40, True), (0x420D40, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
REQUIRED = ['success', 'fail', 'aborted', 'exhausted', 'no_area', 'nped0', 'node_far', 'traffic_fail', 'nolinks', 'cross_skip', 'link_area_big', 'link_unloaded', 'switched_skip', 'lowtraffic_pass',
            'link_traffic_fail', 'd1_pass', 'd1_fail_sqrt_fail', 'd1_fail_sqrt_pass', 'link_reached', 'cam_visible', 'cam_hidden', 'vis_min_fail', 'vis_max_fail', 'vis_ok', 'hid_min_fail',
            'hid_max_fail', 'hid_parity_fail', 'hid_ok', 'ground_called', 'ground_notfound', 'ground_found', 'ground_far', 'ground_ok', 'multi_ground', 'neg_base', 'thr_zero', 'thr_high',
            'matrix_mode_0', 'matrix_mode_1', 'many_rand', 'special_arg', 'ret_0', 'ret_1'] + ['rand_mode%d' % k for k in range(8)] + ['cam_mode%d' % k for k in range(8)]


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
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if [(s['exe'], s['hooked']) for s in stubbed] != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        for s in stubbed:
            if s['hooked'] and not s['dll']:
                raise RuntimeError('Colaborador con hook sin cuerpo de la DLL en ' + fn)
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
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', row['original']), ('DLL', row['candidate'])):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:' % (fn, case, label, diff) + '\n  obtenido ' + str({k: side['observed'].get(k) for k in diff}) + '\n  esperado ' + str({k: want[k] for k in diff}))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        stats['stack_seed'] = True
        for key in stats:
            cov[fn][key] = cov[fn].get(key, 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in REQUIRED}
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['globals_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas o los globales vigilados no se restauraron')
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
        sys.exit(runtime_runner.run_case('fn86c', manifest['binaries']['asi'], analyze, 'Fn86C', timeout=3600))
