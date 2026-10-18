"""Compuertas independientes del banco Dependencies25D: 20 funciones de biblioteca (RenderWare SDK / controlador D3D9 de RenderWare), 1024 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, colaboradores sustituidos en ambos lados, todas las compuertas
de ABI/x87/ruta, la fixture registrada igual a la del oraculo (fixture_oracle.py), y que el resultado de CADA lado (retorno, ventana final del arena, traza de colaboradores,
seis globales del motor) sea el que calcula el oraculo independiente a partir de la ventana inicial registrada. Cobertura minima de las clases de estres y restauracion final.
No relaja ninguna compuerta: kCompareDeadX87Registers debe ser true.
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

EXPECTED = {name: oracle.CASES for name in oracle.NAMES}
GATES = ('fp_ip_route_ok', 'stack_restored', 'nonvolatile_preserved', 'route_ok', 'globals_restored', 'floating_state_restored', 'outside_unchanged')
SIDES = ('original', 'candidate')
MINIMUM = {'gen_rv_zero': 200, 'gen_rv_nonzero': 700, 'ras_sp_in_stack': 500, 'ras_sp_out_of_stack': 200, 'dyn_empty': 100, 'dyn_match': 300, 'dyn_no_match': 200,
           'dyn_duplicate_vb': 30, 'dyn_null_vb_match': 20, 'hr_negative': 400, 'hr_nonnegative': 300, 'depth_equal': 150, 'depth_call': 700, 'depth_mut1_call': 150,
           'depth_mut2_call': 150, 'pixel_mode0': 300, 'pixel_mode1': 300, 'pixel_mode2': 300, 'pixel_null_func': 100, 'pixel_hr_negative': 400, 'lock_ret_zero': 100,
           'lock_wrote': 300, 'lock_not_wrote_nonzero': 200, 'rgba_wrote': 500, 'rgba_not_wrote': 250, 'frame_no_parent': 150, 'plugin_ret_big': 200}
MINIMUM.update({'frame_topo%d_parent' % t: 80 for t in range(8)})


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
    if begin[0]['cases'] != EXPECTED or begin[0]['pairs'] != sum(EXPECTED.values()) or begin[0]['functions'] != len(EXPECTED):
        raise RuntimeError('Numero de casos o de pares inesperado')
    if set(begin[0]['bodies']) != set(oracle.NAMES) or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    base = begin[0]['arena_base']
    if begin[0]['arena_size'] != oracle.ARENA:
        raise RuntimeError('Tamano de arena inesperado')
    index = {name: i for i, name in enumerate(oracle.NAMES)}
    seen = {name: set() for name in oracle.NAMES}
    residue_differs = 0
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        name, case = row['function'], row['case']
        fn = index[name]
        if case in seen[name]:
            raise RuntimeError('Caso repetido %s %d' % (name, case))
        seen[name].add(case)
        for side in SIDES:
            result = row[side]
            if result is None or not all(result.get(k) is True for k in GATES) or result.get('traps') != 0:
                raise RuntimeError('Resultado nulo, compuerta fallida o trampa en %s %d (%s)' % (name, case, side))
        a, b = row['original'], row['candidate']
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (name, case, row['fp_difference']))
        if row['original_residue'] != row['candidate_residue']:
            residue_differs += 1   # informativo: ningun llamador del EXE lee ECX/EDX (ni EAX de las funciones void) tras estas llamadas (ver g25d-classification.md)
        f = oracle.fixture(fn, case)
        extras = oracle.input_extras(fn, case, f, base)
        want = dict(f)
        want.update(extras)
        if row['input'] != want:
            raise RuntimeError('Fixture distinta de la del oraculo en %s %d: %s' % (name, case, {k: (row['input'].get(k), want.get(k)) for k in want if row['input'].get(k) != want[k]}))
        expected = oracle.expect(fn, f, row['before'], base)
        for side, result in (('original', a), ('candidate', b)):
            if result['after'] != expected['after']:
                raise RuntimeError('Ventana final distinta de la del oraculo en %s %d (%s)' % (name, case, side))
            if result['trace'] != expected['trace']:
                raise RuntimeError('Traza distinta de la del oraculo en %s %d (%s): %s / %s' % (name, case, side, result['trace'], expected['trace']))
            if result['globals'] != oracle.globals_json(expected['globals']):
                raise RuntimeError('Globales del motor distintos de los del oraculo en %s %d (%s)' % (name, case, side))
            if fn in oracle.VOID:
                if 'eax' in result:
                    raise RuntimeError('Una funcion void no debe comparar EAX: %s' % name)
            elif result['eax'] != expected['eax']:
                raise RuntimeError('Retorno distinto del oraculo en %s %d (%s): %#x / %#x' % (name, case, side, result['eax'], expected['eax']))
    for name in oracle.NAMES:
        if seen[name] != set(range(oracle.CASES)):
            raise RuntimeError('Cobertura incompleta de ' + name)
    declared = oracle.coverage_declared()
    short = {k: (declared.get(k, 0), v) for k, v in MINIMUM.items() if declared.get(k, 0) < v}
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['coverage'] = declared
    summary['residue_pairs_where_ecx_edx_or_void_eax_differ'] = residue_differs
    summary['compare_dead_x87_registers'] = True
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies25d', manifest['binaries']['asi'], analyze, 'Dependencies25D', timeout=1800))
