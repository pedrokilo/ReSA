"""Compuertas independientes del banco FxEmitterCreateParticle30 (FxEmitter_c::CreateParticle, 0x4A2580), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores sustituidos (7 en el EXE y 7 en la DLL), todas las compuertas de
ABI/ruta/DF/restauracion en AMBOS lados, guardas del arena intactas, fp_difference vacio, la fila `input` igual a la fixture que genera fixture_oracle.py (espejo de Fixtures.hpp),
la salida de CADA lado (arena completa, traza de colaboradores, particula devuelta y la siguiente tirada de rand) igual a la que modela fixture_oracle.py desde el desensamblado (con
comodin SOLO en los resultados que dependen de un empate de NaN con signos opuestos: ahi se exige EXE == DLL), la cobertura de las fixtures y la restauracion final (hook, entradas
de codigo vigiladas, semilla de rand, x87).
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
HOOKS = {'FxEmitter_c::CreateParticle': 0x4A2580}
EXE_COLLABORATORS = [0x4A93C0, 0x4A9400, 0x4A9DC0, 0x7ED9B0, 0x7EDDF0, 0x4A1660, 0x4A21D0]

# Minimos de cobertura, calibrados con el modelo de fixture_oracle.py sobre las 4096 fixtures (valores reales entre parentesis) dejando ~40 % de margen.
MINIMUMS = {'particle': 1900,        # 3151
            'box': 900,              # 1533
            'sphere_pos': 540,       # 906
            'sphere_neg': 420,       # 712
            'dir_norm': 800,         # 1317
            'dir_pos': 600,          # 1021
            'vel_override': 480,     # 813
            'local': 800,            # 1380
            'list_nonempty': 1200,   # 2044
            'pool_freeup': 170,      # 298
            'pool_null': 560,        # 945
            'rem0_first': 110,       # 198
            'rem9999_first': 110,    # 196
            'finite_out': 1100,      # 1872
            'inf_out': 450,          # 770
            'nan_out': 400}          # 682
MIN_PER_KIND = 350   # radiusKind 0..7 (reales ~500)
MIN_PER_DIR = 450    # dirKind 0..5 (reales >= 645)
MAX_UNKNOWN = 20     # casos con comodin por empate de NaN (reales 3)


def table_of(begin):
    raw = bytes.fromhex(begin['sin_table'])
    assert len(raw) == 1024
    return [int.from_bytes(raw[4 * i:4 * i + 4], 'little') for i in range(256)]


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado o sin colaboradores sustituidos')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b['collaborators']['exe'] != EXE_COLLABORATORS or len(b['collaborators']['dll']) != 7 or not all(b['collaborators']['dll']):
        raise RuntimeError('Colaboradores sustituidos distintos de los esperados')
    table = table_of(b)
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
        inp = row['input']
        fx = oracle.make(case)
        for key, value in fx.items():
            if inp.get(key) != value:
                raise RuntimeError('La fixture del caso %d no es la del oraculo (%s): %s != %s' % (case, key, inp.get(key), value))
        base = inp['base']
        want, stats = oracle.expected(fx, case, table, base)
        for label, side in (('EXE', a), ('DLL', c)):
            if not oracle.matches(side['observed'], want):
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, {k: v for k, v in side['observed'].items() if k != 'arena'}, {k: v for k, v in want.items() if k != 'arena'}))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            bump(key, bool(value))
        bump('rk%d' % fx['radiusKind'])
        bump('dk%d' % fx['dirKind'])
        bump('fam%d' % fx['family'])
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS.items():
        if cov.get(key, 0) < m:
            shortfalls[key] = (cov.get(key, 0), m)
    for k in range(8):
        if cov.get('rk%d' % k, 0) < MIN_PER_KIND:
            shortfalls['rk%d' % k] = (cov.get('rk%d' % k, 0), MIN_PER_KIND)
        if cov.get('fam%d' % k, 0) != oracle.CASES // 8:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), oracle.CASES // 8)
    for k in range(6):
        if cov.get('dk%d' % k, 0) < MIN_PER_DIR:
            shortfalls['dk%d' % k] = (cov.get('dk%d' % k, 0), MIN_PER_DIR)
    if cov.get('unknown', 0) > MAX_UNKNOWN:
        shortfalls['unknown'] = (cov.get('unknown', 0), MAX_UNKNOWN)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True
            or state[0]['seed'] != b['initial_seed']):
        raise RuntimeError('El x87, la semilla de rand o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fxemittercreateparticle30', manifest['binaries']['asi'], analyze, 'FxEmitterCreateParticle30', timeout=1800))
