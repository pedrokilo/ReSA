"""Compuertas independientes del banco EventScanners61 (CMentalState::Process 0x6008A0, CVehiclePotentialCollisionScanner::ScanForVehiclePotentialCollisionEvents 0x603720,
CEventScanner::ScanForEvents 0x607E30, CPedStuckChecker::TestPedStuck 0x602C00 y CCollisionEventScanner::ScanForCollisionEvents 0x604500).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok: la entrada de la funcion salta a la DLL solo en el candidato; collaborators_dll: los colaboradores sustituidos con hook en HEAD son saltos a la DLL y los demas codigo del EXE),
fp_difference vacio, la salida de CADA lado igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas, x87).
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
EXPECTED = dict(oracle.CASES)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(oracle.HOOKS)
STUBBED = dict(oracle.STUBBED)   # funcion -> direcciones EXE sustituidas

# Minimos de cobertura por funcion (claves = banderas de `stats` del oraculo; familyN = casos de la familia N). Calibrados con la corrida del banco (~50 % de lo medido).
MINIMUMS = dict(oracle.MINIMUMS)


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
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(EXPECTED):
        raise RuntimeError('Numero de pares inesperado')
    reg_names = set(b['registered'])
    want_hooks = {oracle.HOOK_NAMES[f] for f in EXPECTED}
    if reg_names != want_hooks or not all(b['registered'].values()) or set(b['bodies']) != want_hooks or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    for k, v in oracle.ARENAS.items():
        if b['arenas'].get(k) != v:
            raise RuntimeError('Constante de arena distinta entre el banco y el oraculo: %s (banco %s, oraculo %s)' % (k, b['arenas'].get(k), v))
    stubbed = b['collaborators']['stubbed']
    want_stub = {a for f in EXPECTED for a in oracle.STUBBED[f]}
    if {s['exe'] for s in stubbed} != want_stub or {w['address'] for w in b['watched_entries']} != want_stub or b['collaborators']['dll_real']:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
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
        for label, side in (('EXE', a), ('DLL', c)):
            bad = oracle.check_observed(fn, side['observed'], want, row['input'])
            if bad:
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, bad, side['observed'], want))
        cv = cov[fn]
        cv['family%d' % row['input']['family']] = cv.get('family%d' % row['input']['family'], 0) + 1
        for key, value in stats.items():
            if value is True:
                cv[key] = cv.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cv['residue_differs'] = cv.get('residue_differs', 0) + 1
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
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('eventscanners61', manifest['binaries']['asi'], analyze, 'EventScanners61', timeout=1800))
