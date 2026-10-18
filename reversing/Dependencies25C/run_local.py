"""Compuertas independientes del banco Dependencies25C: 20 funciones de biblioteca de RenderWare (23.296 pares).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores sustituidos en ambos lados (EntryPatch del falso en
el EXE y en la entrada del envoltorio de la DLL), todas las compuertas de ABI/ruta/x87 en ambos lados, la entrada de cada fila igual a la fixture del oraculo
(fixture_oracle.py), el valor de retorno, la traza de colaboradores y los cambios de memoria de CADA lado iguales a los que modela el oraculo desde el desensamblado,
la misma direccion x87 (FIP) en los dos lados, la cobertura de las fixtures de estres y la restauracion final (hooks, variables globales del motor, x87).
No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true. ECX/EDX al volver solo se registran: ningun llamador del EXE los lee (g25c-classification.md);
EAX se compara en todas las funciones salvo RtAnimInterpolatorDestroy (void).
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

M32 = 0xFFFFFFFF
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_ok', 'globals_restored', 'floating_state_restored')
# Minimos de cobertura de las fixtures (sobre el conteo declarado por fixture_oracle.coverage)
MINIMUM = {
    oracle.MESH: {'size_extremes': 40, 'ret_variants': 100},
    oracle.HANIM: {'frame_arena': 250, 'frame_raw': 250, 'frame_null': 250, 'offset_wraps': 100},
    oracle.UV_KEYS: {'real_collaborator': 256, 'shift_overflow': 500, 'sum_overflow': 200},
    oracle.NUM_NODES: {'zero_nodes': 100, 'many_nodes': 300, 'step_zero': 8, 'steps': 12, 'nodes': 13, 'layouts': 14},
    oracle.VIEW_OFFSET: {'alias0': 500, 'alias1': 500, 'alias2': 500, 'alias3': 500, 'parent0': 600, 'parent1': 600, 'parent2': 600, 'nan_x': 400, 'snan': 150, 'subnormal': 200,
                         'inf': 150, 'negzero': 60},
    oracle.FRAME_DIRTY: {'flags': 256, 'root_self': 400, 'root_other': 400},
    oracle.DICT_DESTRUCT: {'word_variants': 400}, oracle.DICT_CURRENT: {'word_variants': 400},
}
for _fn in (oracle.BUILD, oracle.SECTOR, oracle.SECTOR_STREAM, oracle.CAMERA_REG, oracle.ENGINE_REG, oracle.STREAM_WRITE, oracle.ORTHO):
    MINIMUM[_fn] = {'distinct_args': 800, 'ret_variants': 100}
for _fn in (oracle.SKIN, oracle.UV_SIZE, oracle.INTERP_DESTROY, oracle.ENGINE_OFFSET):
    MINIMUM[_fn] = {'ret_variants': 100}


def analyze(path):
    summary = common_analyzer(oracle.EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados o sin colaboradores sustituidos')
    if begin[0].get('compare_dead_x87_registers') is not True:
        raise RuntimeError('Compuerta relajada: los registros x87 vacios deben compararse enteros')
    if begin[0].get('no_raw_collision') is not True:
        raise RuntimeError('Algun valor crudo de las fixtures cae dentro del arena')
    if begin[0]['cases'] != oracle.EXPECTED or begin[0]['pairs'] != oracle.TOTAL or begin[0]['names'] != oracle.NAMES:
        raise RuntimeError('Numero de casos o de pares inesperado')
    if len(begin[0]['bodies']) != 20 or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    seen = {name: set() for name in oracle.NAMES}
    residue_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        name, case = row['function'], row['case']
        fn = oracle.NAMES.index(name)
        if case in seen[name]:
            raise RuntimeError('Caso repetido %s %d' % (name, case))
        seen[name].add(case)
        a, b = row['original'], row['candidate']
        for side, result in (('original', a), ('candidate', b)):
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o compuerta fallida en %s %d (%s)' % (name, case, side))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (name, case, row['fp_difference']))
        if row['original_fp_ip'] != row['candidate_fp_ip']:
            raise RuntimeError('FIP distinto en %s %d' % (name, case))
        f = oracle.make(fn, case)
        if a['input'] != f or b['input'] != f:
            raise RuntimeError('Fixture distinta de la del oraculo en %s %d' % (name, case))
        base = a['base']
        if b['base'] != base:
            raise RuntimeError('Arena distinto entre lados en %s %d' % (name, case))
        eax, trace = oracle.expected(f, base)
        if a['trace'] != trace or b['trace'] != trace:
            raise RuntimeError('Traza de colaboradores distinta de la del oraculo en %s %d: %s / %s / %s' % (name, case, a['trace'], b['trace'], trace))
        if eax is None:
            if a['eax'] is not None or b['eax'] is not None:
                raise RuntimeError('EAX registrado en una funcion void (%s %d)' % (name, case))
        else:
            if a['eax'] != eax or b['eax'] != eax or row['original_residue']['eax'] != eax or row['candidate_residue']['eax'] != eax:
                raise RuntimeError('EAX distinto del oraculo en %s %d: %s / %s / %s' % (name, case, a['eax'], b['eax'], eax))
        if (row['original_residue']['ecx'], row['original_residue']['edx']) != (row['candidate_residue']['ecx'], row['candidate_residue']['edx']):
            residue_differs += 1   # informativo: ningun llamador del EXE lee ECX/EDX tras estas llamadas
        if fn == oracle.VIEW_OFFSET:
            if a['camera_before'] != oracle.camera_words(f) or b['camera_before'] != a['camera_before']:
                raise RuntimeError('Camara inicial distinta de la del oraculo en %d' % case)
            if a['camera_after'] != oracle.view_offset_after(f) or b['camera_after'] != a['camera_after']:
                raise RuntimeError('viewOffset final distinto del oraculo en %d: %s / %s' % (case, a['camera_after'], oracle.view_offset_after(f)))
            if not (a['rest_before'] == a['rest_after'] == b['rest_before'] == b['rest_after']):
                raise RuntimeError('Memoria fuera de viewOffset modificada en %d' % case)
        else:
            if not (a['hash_before'] == a['hash_after'] == b['hash_before'] == b['hash_after']):
                raise RuntimeError('La funcion modifico el arena en %s %d (ninguna de las otras 19 escribe memoria)' % (name, case))
        if fn == oracle.HANIM:
            frame = (base + f['a'][0][1]) & M32 if f['a'][0][0] == oracle.PTR else f['a'][0][1]
            want = (base + f['x'][0] - 4 - frame) & M32
            if a['global_offset'] != want or b['global_offset'] != want:
                raise RuntimeError('Desplazamiento global distinto del oraculo en %d' % case)
    for name in oracle.NAMES:
        if seen[name] != set(range(oracle.EXPECTED[name])):
            raise RuntimeError('Cobertura incompleta de ' + name)
    declared = {}
    for fn, minimum in MINIMUM.items():
        cov = oracle.coverage(fn)
        short = {k: (cov.get(k, 0), v) for k, v in minimum.items() if cov.get(k, 0) < v}
        if short:
            raise RuntimeError('Cobertura de estres insuficiente en %s: %s' % (oracle.NAMES[fn], json.dumps(short)))
        declared[oracle.NAMES[fn]] = cov
    final_state = [r for r in rows if r['event'] == 'global_state_final']
    if len(final_state) != 1 or final_state[0]['state_restored'] is not True or final_state[0]['watched_intact'] is not True:
        raise RuntimeError('Estado global final no restaurado')
    if final_state[0]['engine_instance'] != begin[0]['engine_instance'] or final_state[0]['hanim_offset'] != begin[0]['hanim_offset']:
        raise RuntimeError('Variables globales del motor distintas de las iniciales')
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != oracle.TOTAL or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['declared_fixture_coverage'] = declared
    summary['residue_pairs_where_ecx_edx_differ'] = residue_differs
    summary['compare_dead_x87_registers'] = True
    summary['offline'] = bool(begin[0].get('offline'))
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies25c', manifest['binaries']['asi'], analyze, 'Dependencies25C', timeout=1800))
