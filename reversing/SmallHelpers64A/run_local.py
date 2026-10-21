"""Compuertas independientes del banco SmallHelpers64A (lote 64): GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70), RemoveUpgradeCB (0x6D3300) y FindUpgradeCB (0x6D3370), 4 x 4096 = 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos sean codigo original del EXE y su cuerpo de la DLL este en el ASI),
fp_difference vacio, la salida de CADA lado (EAX/EDX exigidos, eventos EN ORDEN de los falsos, cambios de la arena, RsGlobal.ps final) igual a la que modela fixture_oracle.py desde el desensamblado,
la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87, psGlobal real).
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
HOOKS = {oracle.NAME_TIMER: 0x5617C0, oracle.NAME_MOUSE: 0x746F70, oracle.NAME_REMOVE: 0x6D3300, oracle.NAME_FIND: 0x6D3370}
# colaboradores sustituidos por funcion, EN ORDEN (direccion del EXE; hooked=False: su hook queda apagado durante el banco)
_ATOM_ID, _MODEL, _CLUMP, _ATOM_DESTROY, _FRAME, _REF = 0x732370, 0x732260, 0x74A4C0, 0x749DC0, 0x7F05A0, 0x4C4BB0
STUBBED = {oracle.NAME_TIMER: [0x619410], oracle.NAME_MOUSE: [], oracle.NAME_REMOVE: [_ATOM_ID, _MODEL, _CLUMP, _ATOM_DESTROY, _FRAME, _REF], oracle.NAME_FIND: [_ATOM_ID, _MODEL]}
# colaboradores con hook RH en la DLL (su cuerpo en el ASI debe estar parcheado: dll=true); los demas los llama la DLL por la direccion del EXE (dll=false)
DLL_HOOKED = {0x619410, _ATOM_ID, _MODEL, _REF}
WATCHED = {0x619410, _ATOM_ID, _MODEL, _CLUMP, _ATOM_DESTROY, _FRAME, _REF}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.NAME_TIMER: {'zero': 256, 'hi_bit': 640, 'max': 256, 'one': 256},
    oracle.NAME_MOUSE: {'null_dev': 450, 'plain': 280, 'rel_clear': 80, 'rel_set_dev': 390, 'swap_dev': 410, 'swap_ps_rel': 310, 'swap_ps_unacq': 230},
    oracle.NAME_REMOVE: {'mismatch': 590, 'not_upgrade': 256, 'owner_null': 310, 'owner_other': 310, 'owner_same': 310, 'removed': 940, 'vehdummy': 256},
    oracle.NAME_FIND: {'found': 1000, 'found_vehdummy': 620, 'mismatch': 760, 'not_upgrade': 256},
}
for _fn in NAMES:
    for _k in range(oracle.FAMILIES):
        MINIMUMS[_fn]['family%d' % _k] = 256
    MINIMUMS[_fn]['stack_seed'] = 2048


def check_minimums(cov):
    """cobertura medida {funcion: {clave: cuenta}} frente a MINIMUMS: devuelve {clave: (medido, minimo)} de lo que no llega"""
    short = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
            if cov[fn].get(key, 0) < m:
                short['%s:%s' % (fn, key)] = (cov[fn].get(key, 0), m)
    return short


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
        if col[fn]['dll_real'] or [s['exe'] for s in stubbed] != STUBBED[fn] or any(s['hooked'] for s in stubbed) or any(s['dll'] != (s['exe'] in DLL_HOOKED) for s in stubbed):
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
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
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
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'region_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['region_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas o el psGlobal real (RsGlobal.ps y sus 0x28 bytes) no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('smallhelpers64a', manifest['binaries']['asi'], analyze, 'SmallHelpers64A', timeout=3600))
