"""Compuertas independientes del banco CameraSoundDist30 (CCamera::UpdateSoundDistances, 0x515BD0), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaborador (0x5674E0) sustituido, todas las compuertas de
ABI/ruta/DF/trampas/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (+0x158..+0x163, resto del objeto, entidad, matriz, guardas, contador de fotogramas y
traza del colaborador) igual a la que modela fixture_oracle.py desde el desensamblado (con comodin SOLO donde dos NaN con signos opuestos empatan: ahi se exige EXE == DLL),
la cobertura de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, contador, x87).
La compuerta kCompareDeadX87Registers es false A PROPOSITO (la funcion usa x87): el estado x87 se compara normalizado, ver Core.hpp y README.md.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta terminado).
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
EXPECTED = {name: oracle.CASES for name in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'traps_zero', 'globals_restored', 'floating_state_restored')
HOOKS = {'CCamera::UpdateSoundDistances': 0x515BD0}

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~40 % de margen.
MINIMUMS = {'active0': 840, 'active1': 802, 'active2': 814, 'blend_ext': 25, 'delta_nan': 29, 'ext_exp': 91, 'final_finite': 2325, 'frame_hi': 87, 'has_matrix': 1246, 'hit': 399, 'inf_out': 66, 'list_mode_nonped': 215,
            'mode_hi_bits': 143, 'nan_out': 73, 'no_matrix': 1210, 'nohit': 229, 'ped_nonlist': 1051, 'ped_scale': 518, 'phase0': 629, 'ran': 629, 'point_inf': 165, 'subnormal_out': 46,
            'phase1': 162, 'phase2': 159, 'phase3': 152, 'phase4': 157, 'phase5': 132, 'phase6': 170, 'phase7': 190, 'phase8': 199, 'phase9': 169, 'phase10': 162, 'phase11': 171}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaborador sustituido')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != [0x5674E0] or len(b['collaborators']['dll']) != 1 or not b['collaborators']['dll'][0]:
        raise RuntimeError('Colaborador 0x5674E0 sin sustituir')
    lay = b['layout']
    # el begin del C++ publica desplazamientos (entity_offset, matrix_offset); los tamanos salen de la disposicion [guarda][obj][guarda][entidad][guarda][matriz][guarda]
    entity_bytes = lay['matrix_offset'] - lay['entity_offset'] - oracle.LAYOUT['guard']; matrix_bytes = lay['arena'] - lay['matrix_offset'] - oracle.LAYOUT['guard']
    if lay['arena'] != oracle.LAYOUT['arena'] or lay['obj_bytes'] != oracle.LAYOUT['obj_bytes'] or entity_bytes != oracle.LAYOUT['entity_bytes'] or matrix_bytes != oracle.LAYOUT['matrix_bytes'] or lay['entity_offset'] != lay['obj_offset'] + lay['obj_bytes'] + oracle.LAYOUT['guard']:
        raise RuntimeError('La arena del banco no coincide con las constantes del oraculo')
    seen = set()
    cov = {}

    def bump(key, n=1):
        if n:
            cov[key] = cov.get(key, 0) + n

    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if row['function'] != NAMES[0] or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES) or not all(side['observed'].get(k) is True for k in ('rest_ok', 'guard_ok', 'entity_ok', 'matrix_ok', 'frame_ok')):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            if key == 'family':
                bump('fam%d' % value)
            else:
                bump(key, bool(value))
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS.items():
        if cov.get(key, 0) < m:
            shortfalls[key] = (cov.get(key, 0), m)
    for k in range(16):
        if cov.get('fam%d' % k, 0) != oracle.CASES // 16:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), oracle.CASES // 16)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['frame'] != b['initial_frame'] or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True
            or state[0]['state_restored'] is not True):
        raise RuntimeError('El contador de fotogramas, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('camerasounddist30', manifest['binaries']['asi'], analyze, 'CameraSoundDist30', timeout=1800))
