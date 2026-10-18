"""Compuertas independientes del banco Dependencies25E: 16 funciones de biblioteca (RenderWare y CRT estatica del EXE), 1024 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hooks registrados y cuerpos en el ASI, colaboradores sustituidos/trazados, todas las compuertas
de ABI/x87/ruta en ambos lados, `fp_difference` vacio, la fixture registrada igual a la del oraculo, y el resultado/estado de cada lado igual al del oraculo
independiente (fixture_oracle.py): EAX, arena completo con guardas, traza de colaboradores y los campos propios de cada funcion. No relaja ninguna compuerta:
kCompareDeadX87Registers debe ser true.
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

EXPECTED = oracle.EXPECTED
GATES = ('stack_restored', 'nonvolatile_preserved', 'route_ok', 'guards_ok', 'globals_restored', 'floating_state_restored')
# Minimos de cobertura de estres (sobre las fixtures; el contador sale del oraculo y se exige que cada caso 0..1023 aparezca una vez)
MINIMUM = {
    'frame_free_called': 700, 'frame_free_not_called': 100, 'frame_null_frames_nonzero_count': 50, 'raster_compressed': 800, 'raster_plain': 50, 'raster_negative_delta': 300,
    'plugin_found': 300, 'plugin_not_found': 500, 'plugin_duplicate_ids': 400, 'plugin_empty': 80, 'plugin_full': 80, 'slist_wrap_entry': 400, 'slist_wrap_destroy': 400,
    'slist_zero_size': 10, 'string_null': 100, 'string_wrap': 50, 'atexit_zero': 200, 'atexit_nonzero': 600, 'lock_real': 640, 'lock_section': 384, 'lock_boundary_19': 30,
    'lock_boundary_20': 20, 'lock_max_index': 15, 'malloc_success': 640, 'malloc_fail_heap': 250, 'malloc_fail_early': 70, 'malloc_fail_handler': 200, 'malloc_fail_no_handler': 30,
    'errno_zero': 20, 'errno_negative': 300}
# Campos del oraculo que se comparan con cada lado tal cual (el resto del registro solo se exige igual entre lados)
CHECKED = ('eax', 'arena', 'trace', 'null', 'align8', 'lock_count', 'recursion', 'owner_is_this_thread', 'eax_minus_ptd', 'readback')
# Funciones cuyo EAX es resultado: ningun llamador lee EAX de las demas (void) y no se compara (ver classification)
VOID = {'RwPlCore::_rwSListDestroyEndEntries', 'RwPlCore::_rwStringClose', 'CRT::CrtLockFile2', 'CRT::CrtUnlockFile2'}


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
    if begin[0]['functions'] != 16 or begin[0]['pairs'] != sum(EXPECTED.values()) or begin[0]['cases'] != EXPECTED:
        raise RuntimeError('Numero de funciones, pares o casos inesperado')
    if len(begin[0]['bodies']) != 16 or not all(begin[0]['bodies'].values()):
        raise RuntimeError('Cuerpo RH sin resolver')
    if begin[0]['asi_module'] == begin[0]['original_module']:
        raise RuntimeError('El ASI y el EXE no pueden ser el mismo modulo')
    unlocked, locked = begin[0].get('cs_unlocked_lock_count'), begin[0].get('cs_locked_lock_count')
    # Codificaciones validas de LockCount para una seccion sin otros hilos: (libre, tomada una vez) = (-1, 0) antes de Windows 8 y (-1, -2) desde Windows 8. Cualquier otra medida (p. ej. con esperas) se rechaza.
    if (unlocked, locked) not in ((-1, 0), (-1, -2)):
        raise RuntimeError('Codificacion de LockCount inesperada: %r' % ((unlocked, locked),))
    seen = {f: set() for f in EXPECTED}
    residue_differs = 0
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
        for side, result in (('original', a), ('candidate', b)):
            if result is None or not all(result.get(k) is True for k in GATES):
                raise RuntimeError('Resultado nulo o compuerta fallida en %s %d (%s)' % (fn, case, side))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original_residue']['ecx'] != row['candidate_residue']['ecx'] or row['original_residue']['edx'] != row['candidate_residue']['edx']:
            residue_differs += 1   # informativo: ningun llamador del EXE lee ECX/EDX tras estas llamadas (ver classification.md)
        want = oracle.expected(fn, case, a['base'], (unlocked, locked))
        if a['input'] != want['input']:
            raise RuntimeError('Fixture distinta de la del oraculo en %s %d: %s != %s' % (fn, case, a['input'], want['input']))
        for side, result in (('original', a), ('candidate', b)):
            for key in CHECKED:
                if key == 'eax' and fn in VOID:
                    continue
                if key in want and result.get(key) != want[key]:
                    raise RuntimeError('%s %d (%s): %s = %r, el oraculo espera %r' % (fn, case, side, key, result.get(key), want[key]))
            if 'state' in want and result['state'][:len(want['state'])] != want['state']:
                raise RuntimeError('%s %d (%s): estado global %r distinto del oraculo %r' % (fn, case, side, result['state'][:len(want['state'])], want['state']))
    for fn in EXPECTED:
        if seen[fn] != set(range(EXPECTED[fn])):
            raise RuntimeError('Cobertura incompleta de ' + fn)
    coverage = oracle.coverage()
    short = {k: (coverage[k], v) for k, v in MINIMUM.items() if coverage[k] < v}
    if short:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(short))
    final = [r for r in rows if r['event'] == 'complete']
    if len(final) != 1 or final[0]['pairs'] != sum(EXPECTED.values()) or not all(final[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    summary['coverage'] = coverage
    summary['residue_pairs_where_ecx_edx_differ'] = residue_differs
    summary['compare_dead_x87_registers'] = True
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependencies25e', manifest['binaries']['asi'], analyze, 'Dependencies25E', timeout=1800))
