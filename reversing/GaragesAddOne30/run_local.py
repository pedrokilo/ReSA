"""Compuertas independientes del banco GaragesAddOne30 (CGarages::AddOne 0x4471E0), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook "AddOne" registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS lados (route_ok
incluye que strncpy 0x821F40 del EXE no este enganchado), fp_difference vacio, la salida de CADA lado (ranura completa en bits, resto del array intacto, NumGarages y arena del nombre) igual a la que
modela fixture_oracle.py desde el desensamblado (con comodin '??' SOLO en resultados que dependen de un empate de NaN con signos opuestos; ahi se exige EXE == DLL), la cobertura de las fixtures y la
restauracion final (hook, entradas de codigo vigiladas, array de garajes y contador del juego, x87).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no termino).
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
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x4471E0}
WATCHED = {0x4471E0, 0x821F40}

# Minimos de cobertura. Calibrados con fixture_dump.cpp (std puro, compilado con cl: vuelca los 4096 `input`) + fixture_oracle.py (recuentos medidos en README.md), con ~45 % de margen.
# Si alguno falla por poco, comparar primero las filas `input` registradas con el volcado antes de bajar el umbral.
MINIMUMS = {
    'all_known': 3000, 'corner_x_left': 675, 'corner_x_right': 715, 'corner_y_back': 730, 'corner_y_front': 641, 'finite_dirs': 1748, 'finite_in': 1772,
    'flags_0': 260, 'flags_1': 278, 'flags_2': 277, 'flags_3': 286, 'flags_4': 284, 'flags_5': 303, 'flags_6': 277, 'flags_7': 283, 'flags_high_garbage': 1115,
    'height_inf': 134, 'height_nan': 299, 'height_zero': 66, 'huge_in': 298, 'index_0': 102, 'index_49': 30, 'inf_in': 163, 'left_front': 299, 'left_x2': 324,
    'limits_nan': 371, 'limits_unknown': 13, 'name_empty': 41, 'name_high_bytes': 1672, 'name_long': 575, 'name_short': 1373, 'nan_in': 319, 'negzero_in': 163,
    'right_front': 377, 'right_x2': 320, 'snan_in': 172, 'tiny_in': 270, 'type_high_garbage': 852, 'width_inf': 136, 'width_nan': 292, 'width_zero': 89, 'zero_dir_a': 89, 'zero_dir_b': 66,
}


def analyze(path):
    if not path.exists():
        return None                                   # el juego aun no ha creado el registro
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None                                   # corrida sin terminar
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not False:
        raise RuntimeError('Inicio sin hook registrado o con colaboradores sustituidos (este banco no sustituye ninguno)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != oracle.CASES or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    consts = {'entry_bytes': b.get('entry_bytes'), 'max_garages': b.get('array_bytes', 0) // max(b.get('entry_bytes', 1), 1), 'array_bytes': b.get('array_bytes'),
              'array_address': b.get('array_address'), 'num_address': b.get('num_address'), 'name_arena': b.get('name_arena'), 'name_offset': b.get('name_offset'), 'cases': b['pairs']}
    if consts != oracle.CONSTANTS:
        raise RuntimeError('Constantes del banco distintas de las del oraculo: %s != %s' % (consts, oracle.CONSTANTS))
    watched = {w['address'] for w in b['watched_entries']}
    if watched != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas: %s' % sorted(watched))
    if b['collaborators'].get('stubbed') != [] or set(b['collaborators'].get('exe_real', [])) != {0x821F40}:
        raise RuntimeError('Colaboradores inesperados: %s' % json.dumps(b['collaborators']))
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
        if row['function'] != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (row['function'], case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
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
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    for k in range(16):
        if cov.get('fam%d' % k, 0) != oracle.CASES // 16:
            shortfalls['fam%d' % k] = (cov.get('fam%d' % k, 0), oracle.CASES // 16)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['array_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas, el array de garajes o su contador no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('garagesaddone30', manifest['binaries']['asi'], analyze, 'GaragesAddOne30', timeout=1800))
