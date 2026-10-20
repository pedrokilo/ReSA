"""Compuertas independientes del banco AutoPreRender60 (CAutomobile::PreRender, 0x6AAB50, virtual), 8192 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer; el oraculo de la funcion es el propio EXE con el gancho RH apagado), exige: el gancho RH virtual registrado y con cuerpo en el ASI,
los 24 colaboradores del EXE sustituidos y 23 cuerpos de la DLL sustituidos (el unico sin gancho, AddSingleWheelParticles 0x6DE880, es una envoltura al EXE), todas las compuertas de
ABI/ruta/DF/restauracion en AMBOS lados, fp_difference vacio, que los parametros de cada caso sean los que regenera fixture_oracle.py, la cobertura minima (modelos, valores extremos,
frontera y llamadas a cada colaborador, ninguna a una entrada virtual falsa no prevista) y la restauracion final (gancho, entradas de codigo vigiladas, globales, camara, tabla de modelos, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza; devuelve None si el registro aun no existe o no esta terminado.
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
HOOKS = {'CAutomobile::PreRender': 0x6AAB50}
EXE_COLLABORATORS = [0x6D6480, 0x535300, 0x6DE880, 0x72DDB0, 0x72DDC0, 0x6DDF60, 0x6DE240, 0x6D2A80, 0x6FC580, 0x7000E0, 0x6D1080, 0x6E1A60, 0x56E0D0, 0x50AED0, 0x70BDA0,
                     0x6DBA30, 0x6D1230, 0x6AA290, 0x6A9D70, 0x6F49A0, 0x6F4040, 0x4C7D20, 0x4C53C0, 0x6DBBB0]
EXTRA_DLL = [0x6D1080]   # miembro real CVehicle::GetVehicleAppearance (el gancho RH es la envoltura GetVehicleAppearance_Reversed)
NO_DLL_BODY = 0x6DE880


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin gancho registrado o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Gancho RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != EXE_COLLABORATORS + EXTRA_DLL or len(b['collaborators']['dll']) != len(EXE_COLLABORATORS) + len(EXTRA_DLL) or b.get('no_dll_body') != NO_DLL_BODY:
        raise RuntimeError('Colaboradores sin sustituir')
    for exe, dll in zip(b['collaborators']['exe'], b['collaborators']['dll']):  # incluye el miembro real extra
        if (dll == 0) != (exe == NO_DLL_BODY):
            raise RuntimeError('Cuerpo de la DLL sin resolver para %#x' % exe)
    seen = set()
    cov = {}
    sizes = set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        case = row['case']
        if case in seen:
            raise RuntimeError('Caso repetido %d' % case)
        seen.add(case)
        if row['pair'] != case:
            raise RuntimeError('Numeracion de pares inesperada en %d' % case)
        if row['input'] != oracle.expected_input(case):
            raise RuntimeError('Parametros del caso %d distintos de los que regenera fixture_oracle.py' % case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
            sizes.add(side['observed']['mem_size'])
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        if a['observed']['calls'][oracle.CALL_NAMES.index('BadVirtual')] or c['observed']['calls'][oracle.CALL_NAMES.index('BadVirtual')]:
            raise RuntimeError('Llamada virtual no prevista en %d' % case)
        oracle.tally(row['input'], a['observed'], cov)
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta de ' + NAMES[0])
    if len(sizes) != 1:
        raise RuntimeError('Tamano de la memoria comparada variable: %s' % sizes)
    shortfalls = {}
    for key, minimum in {**oracle.COVERAGE_MIN, **oracle.CALL_MIN}.items():
        if cov.get(key, 0) < minimum:
            shortfalls[key] = (cov.get(key, 0), minimum)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['state_hash'] != b['initial_state_hash'] or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True:
        raise RuntimeError('Los globales, la camara, la tabla de modelos, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = {NAMES[0]: cov}
    summary['coverage_method'] = 'serialized_inputs_regenerated_by_oracle_and_collaborator_call_counts; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('autoprerender60', manifest['binaries']['asi'], analyze, 'AutoPreRender60', timeout=2400))
