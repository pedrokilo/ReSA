"""Compuertas independientes del banco Dependencies22C: CCam::WrapAngle (0x509BE0) y FxEmitter_c::CreateParticles (0x4A41E0).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores sustituidos en ambos lados (stubs trazados), todas las
compuertas de ABI/x87/ruta, la fixture registrada igual a la del oraculo, el estado final de cada arena, la traza de colaboradores y la intensidad iguales a las del oraculo
independiente (fixture_oracle.py: aritmetica x87 a 24 bits sobre fracciones exactas), EDX conservado y ECX = puntero en WrapAngle, la clase de la ultima instruccion x87, la
cobertura de las fixtures de estres y la restauracion final (hooks, globales del tiempo, x87). No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true.
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

WR, CR = 'CCam::WrapAngle', 'FxEmitter_c::CreateParticles'
EXPECTED = {WR: oracle.WRAP_CASES, CR: oracle.CREATE_CASES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored')
GUARD, GUARD_BYTE = 16, 0xA5
NAN_MASK = 0x7F800000


def is_nan_bits(v):
    return (v & NAN_MASK) == NAN_MASK and (v & 0x7FFFFF) != 0


def trace_matches(expected, got):
    """Traza esperada (None = timeSince NaN: solo se exige que sea un NaN) frente a la registrada."""
    if len(expected) != len(got):
        return False
    for x, y in zip(expected, got):
        if x is None:
            if not is_nan_bits(y):
                return False
        elif x != y:
            return False
    return True


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if begin[0]['cases'] != {'Wrap': oracle.WRAP_CASES, 'Create': oracle.CREATE_CASES}:
        raise RuntimeError('Numero de casos inesperado')
    if begin[0]['pairs'] != sum(EXPECTED.values()):
        raise RuntimeError('Numero de pares inesperado')
    for name, body in begin[0]['bodies'].items():
        if not body:
            raise RuntimeError('Cuerpo RH sin resolver: ' + name)
    seen = {WR: set(), CR: set()}
    coverage = {'wr_above': 0, 'wr_below': 0, 'wr_in_range': 0, 'wr_nan': 0, 'wr_big': 0, 'wr_subnormal': 0, 'wr_class_fstp': 0, 'wr_class_fcomp': 0,
                'cr_continued': 0, 'cr_exit_wind_min': 0, 'cr_exit_wind_max': 0, 'cr_exit_rain_min': 0, 'cr_exit_rain_max': 0, 'cr_exit_sum_lt_1': 0,
                'cr_near': 0, 'cr_mid': 0, 'cr_far': 0, 'cr_parent': 0, 'cr_no_parent': 0, 'cr_local': 0, 'cr_global': 0, 'cr_sum_nan': 0, 'cr_dist_nan': 0, 'cr_loop_total': 0}
    loops = {i: 0 for i in range(1, 9)}
    residue_differs = 0
    eax_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if case in seen[fn]:
            raise RuntimeError('Caso repetido %s %d' % (fn, case))
        seen[fn].add(case)
        for side in ('original', 'candidate'):
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o compuerta fallida en %s %d (%s)' % (fn, case, side))
        a, b = row['original'], row['candidate']
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original_residue'] != row['candidate_residue']:
            residue_differs += 1
            if row['original_residue']['eax'] != row['candidate_residue']['eax']:
                eax_differs += 1   # informativo: ningun llamador del EXE lee EAX (ver classification.md)
        if fn == WR:
            f = oracle.wrap_fixture(case)
            want = {'case': case, 'angle': f['angle'], 'kind': f['kind'], 'slot': f['slot'], 'fill': f['fill'], 'edx': f['edx']}
            if a['input'] != want:
                raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (fn, case))
            if not oracle.wrap_in_domain(f):
                raise RuntimeError('Fixture fuera del dominio (el EXE no termina) en %d' % case)
            expected_bits, expected_class = oracle.wrap_expected(f['angle'])
            arena = oracle.build_wrap(f)
            at = 8 + 4 * f['slot']
            arena[at:at + 4] = expected_bits.to_bytes(4, 'little')
            if a['state'] != oracle.guarded(arena) or b['state'] != a['state']:
                raise RuntimeError('Memoria final distinta de la del oraculo en WrapAngle %d' % case)
            for side in (a, b):
                if side['edx_preserved'] is not True or side['edx_after'] != f['edx']:
                    raise RuntimeError('EDX no conservado en WrapAngle %d' % case)
                if side['ecx_is_pointer'] is not True:
                    raise RuntimeError('ECX no es el puntero en WrapAngle %d' % case)
                if side['last_x87_class'] != expected_class:
                    raise RuntimeError('Ultima instruccion x87 inesperada en WrapAngle %d: %s / %s' % (case, side['last_x87_class'], expected_class))
            ang = oracle.F(f['angle'])
            if ang != ang:
                coverage['wr_nan'] += 1
            else:
                coverage['wr_in_range' if -oracle.PI <= ang < oracle.PI else 'wr_above' if ang >= oracle.PI else 'wr_below'] += 1
                if abs(ang) >= 1.0e6:
                    coverage['wr_big'] += 1
                if 0 < abs(ang) < 1.17549435e-38:
                    coverage['wr_subnormal'] += 1
            coverage['wr_class_fstp' if expected_class == 2 else 'wr_class_fcomp'] += 1
        else:
            f = oracle.create_fixture(case)
            keys = ('dist', 'rate', 'lod_start', 'lod_end', 'count', 'init', 'wind', 'rain', 'min_wind', 'max_wind', 'min_rain', 'max_rain', 'delta', 'time', 'parent', 'create_local',
                    'flags_other', 'emission_fill', 'benign', 'guarded')
            want = {'case': case}
            want.update({k: f[k] for k in keys})
            if a['input'] != want:
                raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (fn, case))
            buf, expected_trace, model = oracle.oracle_create(a['base'], f)
            ignore = set()
            nan_intensity = model['sum_is_nan'] if not model['continued'] else model['final_bits'] is None
            if nan_intensity:
                word = a['intensity']
                if not is_nan_bits(word):
                    raise RuntimeError('La intensidad deberia ser NaN en %d' % case)
                ignore = set(range(GUARD + 0x10, GUARD + 0x14))
                coverage['cr_sum_nan'] += 1
            expected_state = bytes.fromhex(oracle.guarded(buf))
            for side in (a, b):
                state = bytes.fromhex(side['state'])
                if len(state) != len(expected_state) or any(g != e for i, (g, e) in enumerate(zip(state, expected_state)) if i not in ignore):
                    raise RuntimeError('Memoria final distinta de la del oraculo en CreateParticles %d' % case)
                if not trace_matches(expected_trace, side['trace']):
                    raise RuntimeError('Traza de colaboradores distinta de la del oraculo en %d: %s / %s' % (case, side['trace'], expected_trace))
                if side['continued'] is not model['continued']:
                    raise RuntimeError('Camino distinto del oraculo en %d' % case)
                if side['last_x87_class'] != (2 if model['continued'] else 1):
                    raise RuntimeError('Ultima instruccion x87 inesperada en CreateParticles %d: %s' % (case, side['last_x87_class']))
            coverage['cr_' + model['path']] += 1
            if model['continued']:
                coverage['cr_continued'] += 1
                coverage['cr_parent' if f['parent'] else 'cr_no_parent'] += 1
                coverage['cr_local' if f['create_local'] else 'cr_global'] += 1
                coverage['cr_loop_total'] += model['loop']
                if 1 <= model['loop'] <= 8:
                    loops[model['loop']] += 1
            else:
                coverage['cr_exit_' + model['reason']] += 1
            if oracle.F(f['dist']) != oracle.F(f['dist']):
                coverage['cr_dist_nan'] += 1
    for fn in EXPECTED:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    minimum = {'wr_above': 600, 'wr_below': 600, 'wr_in_range': 200, 'wr_nan': 20, 'wr_big': 50, 'wr_subnormal': 10, 'wr_class_fstp': 600, 'wr_class_fcomp': 600,
               'cr_continued': 1500, 'cr_exit_wind_min': 300, 'cr_exit_wind_max': 200, 'cr_exit_rain_min': 150, 'cr_exit_rain_max': 100, 'cr_exit_sum_lt_1': 400,
               'cr_near': 500, 'cr_mid': 500, 'cr_far': 300, 'cr_parent': 600, 'cr_no_parent': 600, 'cr_local': 600, 'cr_global': 600, 'cr_sum_nan': 500, 'cr_dist_nan': 300}
    short = {k: (coverage[k], v) for k, v in minimum.items() if coverage[k] < v}
    short.update({'loop_%d' % k: (v, 20) for k, v in loops.items() if v < 20})
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['wind'] != begin[0]['initial_wind'] or state[0]['rain'] != begin[0]['initial_rain']:
        raise RuntimeError('CWeather::Wind/Rain no restaurados')
    summary['coverage'] = coverage
    summary['loop_cases'] = loops
    summary['declared_fixture_coverage'] = {'wrap': oracle.coverage('wrap'), 'create': oracle.coverage('create')}
    summary['residue_pairs_where_ecx_edx_eax_differ'] = residue_differs
    summary['residue_pairs_where_eax_differs'] = eax_differs
    summary['compare_dead_x87_registers'] = True
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies22c', manifest['binaries']['asi'], analyze, 'Dependencies22C', timeout=1800))
