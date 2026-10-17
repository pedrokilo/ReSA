"""Compuertas independientes de D3DResourceSystem::TidyUpD3DTextures (0x7305E0) y TidyUpD3DIndexBuffers (0x730740): rutas, ABI, x87, traza de Release y oraculo."""
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

TEX = 'D3DResourceSystem::TidyUpD3DTextures'
IB = 'D3DResourceSystem::TidyUpD3DIndexBuffers'
CASES = 2048
EXPECTED = {TEX: CASES, IB: CASES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'com_calls_ok', 'globals_restored', 'floating_state_restored')
REQUIRED = {
    TEX: {'zero_count', 'scan_exhausted', 'count_reached', 'null_entry', 'empty_buffer', 'index_wrap', 'reload_after_release', 'index_not_below_count'},
    IB: {'zero_count', 'scan_exhausted', 'count_reached_generic', 'count_reached_slot', 'generic_release', 'generic_null_break', 'generic_empty', 'slot_release',
         'null_entry', 'empty_buffer', 'index_wrap', 'reload_after_release', 'generic_used_as_slot'},
}


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('com_objects_fake') is not True or begin[0].get('real_resources_released') is not False:
        raise RuntimeError('Inicio sin hook registrado o sin objetos falsos')
    if begin[0].get('cases') != CASES or begin[0].get('pairs') != 2 * CASES:
        raise RuntimeError('Numero de casos inesperado')
    if len(begin[0]['target_initial']) != 2 or not all(t['registered'] and t['reconstructed'] and t['route_ok'] for t in begin[0]['target_initial']):
        raise RuntimeError('Los dos objetivos deben estar registrados, reconstruidos y con ruta valida al inicio')
    coverage = {TEX: set(), IB: set()}
    counts_seen = {TEX: set(), IB: set()}
    nums_seen = {TEX: set(), IB: set()}
    n_seen, tex_idx_seen, ib_idx_seen = set(), set(), set()
    mutations = {'n': 0, 'tex_index': 0, 'ib_index': 0}
    pairs, releases, max_releases = set(), {TEX: 0, IB: 0}, 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        function, fx = row['function'], row['fixture']
        if row['pair'] in pairs:
            raise RuntimeError('Par repetido')
        pairs.add(row['pair'])
        if row['memory_bytes_equal'] is not True:
            raise RuntimeError('Memoria observable distinta byte a byte en %d' % row['pair'])
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o gate fallido en %d (%s)' % (row['pair'], side))
            paths = oracle.check(function, fx, result)           # EXE y DLL deben coincidir con el oraculo, no solo entre si
        coverage[function] |= paths
        counts_seen[function].add(fx['count'])
        n_seen.add(fx['n'])
        releases[function] += row['original']['releases']
        max_releases = max(max_releases, row['original']['releases'])
        for slot in fx['tex'] + fx['ib'] + [fx['gen']]:
            nums_seen[function].add(slot[0])
        mut = fx['mutation']
        if mut['at'] and row['original']['releases'] >= mut['at']:          # la mutacion se aplico de verdad
            mutations['n'] += bool(mut['n'])
            mutations['tex_index'] += mut['tex_index'] >= 0
            mutations['ib_index'] += mut['ib_index'] >= 0
            tex_idx_seen.add(mut['tex_index'])
            ib_idx_seen.add(mut['ib_index'])
        # Fuera de la traza y la region el banco compara todo el pool de objetos (contadores de Release) y las 35 listas: memory_bytes_equal.
    if pairs != set(range(2 * CASES)):
        raise RuntimeError('Cobertura incompleta')
    for function in (TEX, IB):
        if oracle.REGION_SIZE != 0x388 or REQUIRED[function] - coverage[function]:
            raise RuntimeError('Faltan rutas en %s: %s' % (function, ', '.join(sorted(REQUIRED[function] - coverage[function]))))
        if set(oracle.COUNTS) - counts_seen[function]:
            raise RuntimeError('Faltan cuentas de estres en %s: %s' % (function, sorted(hex(c) for c in set(oracle.COUNTS) - counts_seen[function])))
        if set(oracle.NUM_EXTREMES) - nums_seen[function]:
            raise RuntimeError('Faltan contadores extremos en %s' % function)
        if releases[function] < 1000:
            raise RuntimeError('Pocos Release observados en %s' % function)
    if not {1, 2, 16, 17} <= n_seen:
        raise RuntimeError('Falta estres de NumTextureBuffers (1, 2, 16, 17)')
    if min(mutations.values()) < 10:
        raise RuntimeError('Pocas mutaciones aplicadas durante Release: %s' % mutations)
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != 2 * CASES or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'helper_routes_ok', 'target_restored')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['complete_paths'] = {f: sorted(p) for f, p in coverage.items()}
    summary['releases_per_function'] = releases
    summary['max_releases_per_pair'] = max_releases
    summary['applied_mutations'] = mutations
    summary['real_lists_protected'] = begin[0].get('real_lists_protected')
    summary['coverage_method'] = 'serialized_fixtures_and_independent_oracle; EXE and DLL both checked against the oracle; registers not compared (no caller reads them)'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies21f', manifest['binaries']['asi'], analyze, 'Dependencies21F', timeout=1800))
