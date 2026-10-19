"""Compuertas independientes del banco QuatSlerpT29 (CQuaternion::Slerp(from, to, t), 0x59C630), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, los colaboradores del EXE (CalcTheta 0x4D00E0 y Slerp de cinco argumentos 0x59C300)
con su hook RH apagado toda la corrida, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados, guardas de la arena intactas, fp_difference vacio, la salida de CADA lado (ventana
completa con from, to y destino, en bits, y errno del EXE) igual a la que modela fixture_oracle.py desde el desensamblado (con comodin '??' SOLO en los resultados desconocidos: empate de NaN
con signos opuestos, fldpi de 64 bits y fsin fuera de rango; ahi se exige EXE == DLL; tolerancia de 2 ulp en los floats de salida), la cobertura de las fixtures y la restauracion final
(hooks, entradas de codigo vigiladas, x87).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {'CQuaternion::Slerp(const CQuaternion&, const CQuaternion&, float)': 0x59C630}
COLLABORATOR_HOOKS = ('collaborator CAnimBlendNode::CalcTheta', 'collaborator CQuaternion::Slerp(5 args)')
COLLABORATORS = {'exe': ['CAnimBlendNode::CalcTheta 0x4D00E0', 'CQuaternion::Slerp(5 args) 0x59C300'], 'dll': []}

# Minimos de cobertura, calibrados con fixture_mirror.py (espejo en Python del generador) dejando ~40-50 % de margen. El espejo NO se ha comprobado contra el generador C++ (no se compilo):
# si algun minimo falla por poco, revisar las filas `input` registradas frente al espejo antes de bajar el umbral.
# Espejo: alias_inputs 329, alias_out 781, all_known 3941, clamp 875, copy 1161, errno_domain 723, finite_in 3634, finite_out 3191, flip 693, indefinite_out 433, inf_in 101, inf_out 15,
# kind_domain 413, kind_minus1 66, kind_nan 310, kind_normal 2077, kind_plus1 1161, nan_in 396, nan_out 735, negzero_out 467, out_eq_from 388, out_eq_to 391, subnormal_out 155,
# t_nan 163, t_outside 282, t_unit 3651, theta_half_pi 402, theta_small 354, unknown 155, zero_out 1035.
MINIMUMS = {'alias_inputs': 150, 'alias_out': 400, 'all_known': 3000, 'clamp': 450, 'copy': 600, 'errno_domain': 380, 'finite_in': 2000, 'finite_out': 1800, 'flip': 360, 'indefinite_out': 230,
            'inf_in': 50, 'inf_out': 6, 'kind_domain': 220, 'kind_minus1': 35, 'kind_nan': 160, 'kind_normal': 1200, 'kind_plus1': 600, 'nan_in': 200, 'nan_out': 380, 'negzero_out': 250,
            'out_eq_from': 200, 'out_eq_to': 200, 'subnormal_out': 80, 't_nan': 85, 't_outside': 150, 't_unit': 2000, 'theta_half_pi': 210, 'theta_small': 190, 'unknown': 80, 'zero_out': 550}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores sustituidos (los colaboradores son los bytes del EXE, no falsos)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if not all(b['registered'].get(name) is True for name in list(HOOKS) + list(COLLABORATOR_HOOKS)) or set(b['registered']) != set(HOOKS) | set(COLLABORATOR_HOOKS):
        raise RuntimeError('Hook RH (o colaborador) sin registrar')
    if set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Cuerpo sin resolver')
    if b['collaborators'] != COLLABORATORS or b.get('collaborator_hooks_off') is not True:
        raise RuntimeError('Colaboradores inesperados o con el hook encendido')
    watched = {w['address'] for w in b['watched_entries']}
    if not {0x59C630, 0x4D00E0, 0x59C300} <= watched:
        raise RuntimeError('Las entradas del EXE (funcion y colaboradores) no estan vigiladas')
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
        if row['function'] != oracle.NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES) or side['observed'].get('guard_ok') is not True:
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
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('quatslerpt29', manifest['binaries']['asi'], analyze, 'QuatSlerpT29', timeout=1800))
