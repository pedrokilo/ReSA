"""Compuertas independientes del banco D3DResource62 (lote 62): D3DResourceSystem::CancelBuffering (0x730900), CreateIndexBuffer (0x7306A0), CreateTexture (0x730510),
DestroyIndexBuffer (0x730D30), DestroyTexture (0x730B70), Init (0x730830) y Shutdown (0x730A00), 7 x 4096 = 28672 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados
(route_ok incluye que la entrada del EXE salte a la DLL solo en el candidato; collaborators_dll exige que los 9 colaboradores sustituidos sean codigo original del EXE), fp_difference vacio,
la salida de CADA lado (eventos de los falsos EN ORDEN con sus argumentos, cambios del bloque de globales 0xC87C60..0xC87FE8 y de la arena del banco, byte 0x8D6084 y EAX de los HRESULT)
igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion final (hooks, entradas vigiladas, x87, estado real de las caches y del
dispositivo D3D). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
HOOKS = {oracle.NAME_CANCEL: 0x730900, oracle.NAME_CREATE_IB: 0x7306A0, oracle.NAME_CREATE_TEX: 0x730510, oracle.NAME_DESTROY_IB: 0x730D30, oracle.NAME_DESTROY_TEX: 0x730B70,
         oracle.NAME_INIT: 0x730830, oracle.NAME_SHUTDOWN: 0x730A00}
# colaboradores sustituidos (direccion del EXE, tiene hook en la DLL): los mismos para las 7 funciones; ninguno tiene hook
STUBBED = [(0x72FF60, False), (0x72FE80, False), (0x72FFF0, False), (0x730AD0, False), (0x730020, False), (0x730270, False), (0x730330, False), (0x82119A, False), (0x8214BD, False)]
WATCHED = {a for a, _ in STUBBED}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~50 % de lo medido.
_CANCEL = {
    'frees_ge_1': 2048, 'frees_ge_10': 2048, 'gen_released': 1357, 'ib_released': 2048, 'ntex_17': 256, 'ntex_many': 704, 'ntex_neg': 512, 'ntex_one': 320, 'ntex_zero': 256,
    'null_ptr_zero_size': 1897, 'reentry_fired': 335, 'reentry_kind1': 95, 'reentry_kind2': 83, 'reentry_kind3': 81, 'reentry_kind4': 75, 'releases_ge_10': 2048, 'releases_ge_30': 1763,
    'size_neg_seen': 1356, 'tex_released': 1158, 'use_other': 576, 'use_zero': 597,
}
_SHUTDOWN = {k: v for k, v in _CANCEL.items() if not k.startswith('frees_')}
MINIMUMS = {
    oracle.NAME_CANCEL: dict(_CANCEL),
    oracle.NAME_SHUTDOWN: dict(_SHUTDOWN),
    oracle.NAME_CREATE_IB: {
        'boundary_100': 912, 'cache_empty': 559, 'cache_group': 1484, 'dev_fail': 330, 'dev_ok': 470, 'dev_write0': 279, 'dev_write1': 262, 'dev_write2': 260, 'device': 801,
        'generic_group': 564, 'group0': 435, 'group15': 176, 'group_ge16': 564, 'hit': 1247, 'num_huge': 256, 'num_zero': 128, 'pop_hit': 322, 'pop_null': 242,
    },
    oracle.NAME_CREATE_TEX: {
        'dev_fail': 301, 'dev_ok': 487, 'dev_write0': 267, 'dev_write1': 251, 'dev_write2': 269, 'device': 788, 'generic_pop': 887, 'generic_pop_hit': 513, 'generic_pop_null': 374,
        'levels_0_1': 461, 'levels_gt1': 934, 'levels_neg': 652, 'match': 1161, 'match_empty': 414, 'match_pop': 747, 'near_fmt': 626, 'near_levels': 632, 'near_width': 770,
        'nomatch_square_multi': 311, 'nonsquare': 439, 'ntex_big': 227, 'ntex_le1': 84, 'ntex_neg': 87, 'square': 1609, 'w_ge_2p24': 290, 'w_neg': 188, 'width_rounding': 125,
    },
    oracle.NAME_DESTROY_IB: {
        'bytes_zero': 172, 'cache_stored': 511, 'cap_huge': 116, 'cap_neg': 78, 'cap_zero': 133, 'dropped': 579, 'gen_stored': 341, 'group_ge16': 567, 'group_lt16': 864, 'odd_bytes': 292,
        'resize_called': 757, 'resize_grew': 178, 'size_ge_cap': 757, 'stored': 852, 'use0': 616,
    },
    oracle.NAME_DESTROY_TEX: {
        'gen_dropped': 62, 'gen_resize': 92, 'gen_store': 145, 'lc_gt1': 706, 'loop_near_miss': 79, 'match_resize': 311, 'match_resize_dropped': 207, 'match_resize_stored': 104,
        'match_store': 408, 'new_buffer': 182, 'ntex_ge17': 138, 'ntex_neg': 56, 'release_big': 152, 'release_big_square': 172, 'square': 1121, 'use0': 614, 'w_big_exact': 92, 'w_ge_2p31': 77,
    },
    oracle.NAME_INIT: {
        'new_all_pool': 982, 'new_null': 323, 'new_raw_some': 1066, 'prior0': 1024, 'prior1': 512, 'prior2': 512, 'use_was_zero': 508,
    },
}
FAMILIES = {oracle.NAME_CANCEL: (8, 256), oracle.NAME_SHUTDOWN: (8, 256), oracle.NAME_CREATE_IB: (16, 128), oracle.NAME_CREATE_TEX: (16, 128), oracle.NAME_DESTROY_IB: (8, 256),
            oracle.NAME_DESTROY_TEX: (8, 256), oracle.NAME_INIT: (4, 512)}
for _fn in NAMES:
    _n, _m = FAMILIES[_fn]
    for _k in range(_n):
        MINIMUMS[_fn]['family%d' % _k] = _m
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
        if col[fn]['dll_real'] or [(s['exe'], s['hooked']) for s in stubbed] != STUBBED:
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        if any(s['dll'] for s in stubbed):
            raise RuntimeError('Colaborador con cuerpo de la DLL inesperado en ' + fn)
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
            if side['observed'].get('device_unchanged') is not True:
                raise RuntimeError('El dispositivo falso fue modificado en %s %d' % (fn, case))
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
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact', 'd3d_state_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['d3d_state_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas o el estado real de las caches D3D no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('d3dresource62', manifest['binaries']['asi'], analyze, 'D3DResource62', timeout=3600))
