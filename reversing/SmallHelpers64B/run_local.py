"""Compuertas independientes del banco SmallHelpers64B (lote 64B): GetPopcycleGroupPedModel (0x406E80), FlushWaterRenderBuffer (0x6E7680) y CTaskComplex::Destructor (0x61A3D0),
3 x 4096 = 12288 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los colaboradores sustituidos (RwIm3DTransform 0x7EF450,
RwIm3DRenderIndexedPrimitive 0x7EF550, RwIm3DEnd 0x7EF520: codigo del EXE sin hook, llamados por direccion) sigan siendolo en ambos lados), fp_difference vacio, la salida de CADA lado
(eventos de los falsos EN ORDEN con sus argumentos, resultados, tablas y objetos sin cambios) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures y la restauracion final (hooks, entradas vigiladas, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
HOOKS = {oracle.NAME_MODEL: 0x406E80, oracle.NAME_FLUSH: 0x6E7680, oracle.NAME_TASK: 0x61A3D0}
# colaboradores sustituidos por funcion: (direccion del EXE, tiene hook en la DLL)
STUBBED = {
    oracle.NAME_MODEL: [],
    oracle.NAME_FLUSH: [(0x7EF450, False), (0x7EF550, False), (0x7EF520, False)],
    oracle.NAME_TASK: [],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
MINIMUMS = {
    oracle.NAME_MODEL: {
        'family0': 1024, 'family1': 1024, 'family2': 1024, 'family3': 1024, 'g_high': 60, 'g_mid': 1890, 'g_minus1': 33, 'g_zero': 35, 'group_high': 18, 'group_neg': 35,
        'slot_high': 95, 'slot_neg': 89, 'stack_seed': 945, 'value_high_bit': 1000, 'zone_out': 300,
    },
    oracle.NAME_FLUSH: {
        'draw_signed_edge': 128, 'draw_wrap': 100, 'full_flush': 1358, 'idx_big': 496, 'idx_neg': 527, 'idx_zero': 256, 'ret_low_zero': 775, 'ret_nonzero': 1806, 'ret_zero': 242,
        'stack_seed': 945, 'transform_only': 178, 'vtx_hi_bit16': 380, 'vtx_neg': 643, 'vtx_pos': 892, 'vtx_zero': 512,
    },
    oracle.NAME_TASK: {
        'parent_set': 1349, 'parent_zero': 699, 'stack_seed': 945, 'sub_a': 695, 'sub_b': 679, 'sub_null': 673, 'vtable_exe': 496, 'vtable_other': 1043, 'vtable_zero': 508,
    },
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
    if set(col) != set(NAMES):
        raise RuntimeError('Colaboradores inesperados')
    for fn in NAMES:
        stubbed = col[fn]['stubbed']
        if col[fn]['dll_real'] or [(s['exe'], s['hooked']) for s in stubbed] != STUBBED[fn]:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        for s in stubbed:
            if s['hooked'] and not s['dll']:
                raise RuntimeError('Colaborador con hook sin cuerpo de la DLL en ' + fn)
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
        for label, side in (('EXE', a), ('DLL', c)):
            want, stats = oracle.expected(fn, row['input'], case, side['observed'])
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[fn][key] = cov[fn].get(key, 0) + 1
    for fn in NAMES:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta en ' + fn)
    shortfalls = {}
    for fn in NAMES:
        for key, m in MINIMUMS[fn].items():
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
        sys.exit(runtime_runner.run_case('smallhelpers64b', manifest['binaries']['asi'], analyze, 'SmallHelpers64B', timeout=3600))
