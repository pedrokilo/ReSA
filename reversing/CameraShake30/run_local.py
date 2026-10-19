"""Compuertas independientes del banco CameraShake30 (CCamera::ProcessShake, 0x516560), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaborador (0x50D930) sustituido, todas las compuertas de
ABI/ruta/DF/trampas/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado (front y up de la camara activa en bits, resto del objeto, guardas, tabla de CHandShaker completa,
bandera 0xB70048 y traza del colaborador) igual a la que modela fixture_oracle.py desde el desensamblado (con comodin SOLO donde dos NaN con signos opuestos empatan o el fsin del hardware
es impreciso: ahi se exige EXE == DLL), la cobertura de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, tabla, bandera, x87).
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
HOOKS = {'CCamera::ProcessShake': 0x516560}

# Minimos de cobertura, calibrados con fixture_selfcheck.py (volcado compilado de Fixtures.hpp + oraculo) dejando ~40 % de margen.
MINIMUMS = {
    'active0': 831, 'active1': 799, 'active2': 826, 'angle_huge': 125, 'angle_imprecise': 91, 'angle_inf': 100, 'angle_mid': 146, 'angle_nan': 115, 'angle_normal': 1762,
    'angle_zero': 262, 'ext_exp_hi': 201, 'ext_exp_lo': 76, 'flag0': 449, 'flag1': 2008, 'front_nonfinite': 183, 'intensity_nan': 70, 'intensity_zero': 38,
    'mat_nonfinite': 241, 'nan_out': 560, 'norm1_zero': 121, 'norm2_zero': 120, 'subnormal_out': 10, 'tiny_override': 190, 'type0': 426, 'type1': 391, 'type2': 427,
    'type3': 412, 'type4': 411, 'type5': 387}


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
    if b['collaborators']['exe'] != [0x50D930] or len(b['collaborators']['dll']) != 1 or not b['collaborators']['dll'][0]:
        raise RuntimeError('Colaborador 0x50D930 sin sustituir')
    if len(b.get('exe_helper_hooks', [])) != 2:
        raise RuntimeError('Faltan los hooks de 0x59C910 / 0x59C730 en begin (deben apagarse durante el banco)')
    lay = b['layout']
    if lay['arena'] != oracle.LAYOUT['arena'] or lay['obj_bytes'] != oracle.LAYOUT['obj_bytes'] or lay['table_bytes'] != oracle.LAYOUT['table_bytes']:
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
            if not all(side.get(k) is True for k in GATES) or not all(side['observed'].get(k) is True for k in ('rest_ok', 'guard_ok')):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                short = lambda d: {k: v for k, v in d.items() if k != 'table'}
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, short(side['observed']), short(want)))
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
    if (len(state) != 1 or state[0]['table'] != b['initial_table'] or state[0]['flag'] != b['initial_flag'] or state[0]['floating_state'] != b['initial_fp']
            or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True):
        raise RuntimeError('La tabla de CHandShaker, su bandera, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('camerashake30', manifest['binaries']['asi'], analyze, 'CameraShake30', timeout=1800))
