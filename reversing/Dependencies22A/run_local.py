"""Compuertas independientes del banco Dependencies22A (CPointLights: 0x6FFBB0, 0x6FFE70, 0x6FFFF0, 0x7002D0).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer): hook registrado y cuerpos en el ASI, colaboradores sustituidos, todas las compuertas de ABI/ruta/x87, fp_difference vacio,
oraculo independiente de ProcessVerticalLineUsingCache (acierto de cache, llamadas a ProcessVerticalLine, *outZ), cobertura minima de estres por funcion y final con restauracion.
No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true y los diez bytes de cada registro x87 vacio coinciden en todos los pares (dead_registers_equal).
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

GE, MU, CA, FO = ('CPointLights::GenerateLightsAffectingObject', 'CPointLights::GetLightMultiplier', 'CPointLights::ProcessVerticalLineUsingCache', 'CPointLights::RenderFogEffect')
EXPECTED = {GE: oracle.GEN, MU: oracle.MULT, CA: oracle.CACHE, FO: oracle.FOG}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    if begin[0]['pairs'] != sum(EXPECTED.values()) or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Pares o cuerpos RH inesperados')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    seen = {k: set() for k in EXPECTED}
    cov = dict(gen_adds=0, gen_total_changed=0, mult_cases=0, cache_hit=0, cache_miss=0, cache_pvl_fail=0, fog_sprites=0, fog_calc=0, fog_pvl=0, fog_cache=0, fog_early=0, stress_inputs=0)
    dead_equal = dead_total = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        a, b = row['original'], row['candidate']
        for side in (a, b):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d' % (fn, case))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        dead_total += 1
        dead_equal += bool(row['dead_registers_equal'])
        if row['dead_registers_equal'] is not True:
            raise RuntimeError('Registros x87 vacios distintos en %s %d' % (fn, case))
        if a['input'].get('case') != case:
            raise RuntimeError('Entrada con otro numero de caso')
        counts = a['trace']['counts']
        if fn == GE:
            cov['gen_adds'] += counts.get('1', 0) > 0
        elif fn == MU:
            cov['mult_cases'] += 1
        elif fn == CA:
            hit, pvl, out_z, nxt, ret = oracle.cache_oracle(case)
            if counts.get('2', 0) != pvl:
                raise RuntimeError('Llamadas a ProcessVerticalLine distintas del oraculo en %d' % case)
            if a['observed']['out_z'] != out_z or (a['observed']['out_z'] != b['observed']['out_z']):
                raise RuntimeError('*outZ distinto del oraculo en %d' % case)
            if hit >= 0:
                cov['cache_hit'] += 1
            elif ret:
                cov['cache_miss'] += 1
            else:
                cov['cache_pvl_fail'] += 1
        else:
            cov['fog_sprites'] += counts.get('16', 0) > 0
            cov['fog_calc'] += counts.get('13', 0) > 0
            cov['fog_pvl'] += counts.get('2', 0) > 0   # CWorld::ProcessVerticalLine se traza como evento 2 (13 CalcScreenCoors, 15 cache, 16 sprite)
            cov['fog_cache'] += counts.get('15', 0) > 0
            cov['fog_early'] += counts.get('10', 0) == 0
    for fn in EXPECTED:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    minimum = dict(gen_adds=400, mult_cases=1500, cache_hit=400, cache_miss=300, cache_pvl_fail=100, fog_sprites=800, fog_calc=1200, fog_pvl=600, fog_cache=600, fog_early=20)
    short = {k: (cov[k], v) for k, v in minimum.items() if cov[k] < v}
    if short:
        raise RuntimeError('Cobertura insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary.update(coverage=cov, compare_dead_x87_registers=begin[0].get('compare_dead_x87_registers'), dead_registers_equal_pairs=[dead_equal, dead_total],
                   coverage_method='serialized_inputs_trace_counts_and_independent_cache_oracle')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies22a', manifest['binaries']['asi'], analyze, 'Dependencies22A', timeout=1800))
