"""Compuertas independientes del banco CarCtrlWeave63C (lote 63, CCarCtrl parte B2, tanda C): UpdateCarOnRails, 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que las entradas de los colaboradores sustituidos sean codigo original del EXE y sus cuerpos
de la DLL esten en el ASI), fp_difference vacio, la ruta de eventos igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura (medida desde
la entrada y desde los eventos/cambios observados) y la restauracion final (hooks, entradas vigiladas, x87, tabla de info de modelos, reloj).
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
HOOKS = {NAMES[0]: 0x436540}
# colaboradores sustituidos (direccion del EXE; hooked=False: su hook queda apagado durante el banco)
_422590, _432B10, _SLOW, _MODIFY, _CURVE, _42EC90 = (0x422590, 0x432B10, 0x434790, 0x41B980, 0x43C900, 0x42EC90)
STUBBED = {NAMES[0]: [_422590, _432B10, _SLOW, _MODIFY, _CURVE, _42EC90]}
WATCHED = {_422590, _432B10, _SLOW, _MODIFY, _CURVE, _42EC90}
EV = {1: '422590', 2: '432b10', 3: 'slow_rails', 4: 'modify', 5: 'curve', 6: '42ec90'}

# Minimos de cobertura. Calibrados con fixture_mirror.py (entrada) y con la corrida (observados): ~50 % de lo medido.
MINIMUMS = {fn: {} for fn in NAMES}
FAMILIES = 12
for _fn in NAMES:
    for _k in range(FAMILIES):
        MINIMUMS[_fn]['family%d' % _k] = 256
    MINIMUMS[_fn]['stack_seed'] = 2048
try:
    MINIMUMS.update({k: {**MINIMUMS[k], **v} for k, v in json.loads((HERE / 'minimums.json').read_text(encoding='utf-8')).items()})
except FileNotFoundError:
    pass


def check_minimums(cov, observed=True):
    """cobertura medida {funcion: {clave: cuenta}} frente a MINIMUMS: devuelve {clave: (medido, minimo)} de lo que no llega (observed=False: solo las claves de la entrada)"""
    short = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
            if not observed and key.startswith(('obs_', 'ev_', 'chg_', 'sweeps_')):
                continue
            if cov[fn].get(key, 0) < m:
                short['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    return short


def observed_stats(side):
    """claves de cobertura a partir de lo observado: tipos de evento, palabras cambiadas, numero de pruebas de barrido"""
    st = {}
    kinds = [e[0] for e in side['events']]
    for k in set(kinds):
        st['ev_' + EV.get(k, str(k)).replace('ev_', '')] = True
    for c in side['changes']:
        st['chg_%s%s' % (c[0], ('_%d' % c[2]) if c[0] in ('E', 'O') else '')] = True
    if not side['changes']:
        st['chg_none'] = True
    if not side['events']:
        st['ev_none'] = True
    return st


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
        if col[fn]['dll_real'] or [s['exe'] for s in stubbed] != STUBBED[fn] or any(s['hooked'] for s in stubbed):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
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
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if a['observed']['nevents'] > 290:
            raise RuntimeError('Demasiados eventos en %s %d' % (fn, case))
        want, stats = oracle.expected(fn, row['input'], case)
        path = oracle.expected_path(row['input'])
        for label, side in (('EXE', a), ('DLL', c)):
            got = [e[0] for e in side['observed']['events']]
            if got != path:
                raise RuntimeError('Ruta de eventos distinta de la del oraculo en %s %d (%s): %s / %s' % (fn, case, label, got, path))
        if want is not None:
            for label, side in (('EXE', a), ('DLL', c)):
                if side['observed'] != want:
                    diff = [k for k in want if side['observed'].get(k) != want[k]]
                    raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
        for key in observed_stats(a['observed']):
            cov[fn][key] = cov[fn].get(key, 0) + 1
        cov[fn]['family%d' % row['input']['fam']] = cov[fn].get('family%d' % row['input']['fam'], 0) + 1
        cov[fn]['stack_seed'] = cov[fn].get('stack_seed', 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = check_minimums(cov)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'navi_table_restored', 'timer_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['navi_table_restored'] is not True \
            or state[0]['timer_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas, la tabla de enlaces de carretera (ThePaths.m_pNaviNodes) o el reloj (0xB7CB84) no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_independent_oracle_for_pure_functions_and_observed_events; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('carctrlweave63c', manifest['binaries']['asi'], analyze, 'CarCtrlWeave63C', timeout=3600))
