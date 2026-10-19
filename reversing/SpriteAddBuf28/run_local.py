"""Compuertas independientes del banco SpriteAddBuf28 (CSprite2d::AddToBuffer, 0x728200), 4096 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook registrado y cuerpo en el ASI, colaboradores (0x727A00 y 0x7273D0) sustituidos, todas las compuertas de
ABI/ruta/DF/trampas/restauracion en AMBOS lados, fp_difference vacio, la salida de CADA lado igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura
de las fixtures y la restauracion final (hook, entradas de codigo vigiladas, bufers de vertices/indices, contadores, x87).
Las compuertas kCompareDeadX87Registers y kCompareVolatileRegisters son false A PROPOSITO (ver Core.hpp y README.md).
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
HOOKS = {'CSprite2d::AddToBuffer': 0x728200}

# Minimos de cobertura. Calibrados con la cobertura de fixture_oracle.py (espejo EXACTO de Fixtures.hpp: mismas funciones puras Rand(caso, clave)) dejando ~20-45 % de margen
# por si el generador C++ difiriese en algun detalle de aritmetica. Valores del espejo: full_v 546, full_i 519, both 205, neither 3236, rendered 860, reset 827,
# render_noreset 33, v_oob_window 626, v_negative 104, v_huge 97, v_trunc16 235, v_edge 479, i_edge 447, i_zero 257, v_zero 128, cada kind 128.
MINIMUMS = (('ran', 4096), ('full_v', 400), ('full_i', 400), ('both', 130), ('neither', 2500), ('rendered', 650), ('reset', 620), ('render_noreset', 20), ('v_oob_window', 450),
            ('v_negative', 70), ('v_huge', 60), ('v_trunc16', 160), ('v_edge', 350), ('i_edge', 330), ('i_zero', 180), ('v_zero', 90))


def need(counter, key, minimum, label, shortfalls):
    if counter.get(key, 0) < minimum:
        shortfalls[label] = (counter.get(key, 0), minimum)


def analyze(path):
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
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
    if b['collaborators']['exe'] != [0x727A00, 0x7273D0] or len(b['collaborators']['dll']) != 2 or not all(b['collaborators']['dll']):
        raise RuntimeError('Colaboradores 0x727A00/0x7273D0 sin sustituir')
    seen = set()
    cov = {}

    def bump(key, n=1):
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
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))
            if side['calls'] != want['calls']:
                raise RuntimeError('Numero de llamadas a los colaboradores inesperado en %d (%s): %s' % (case, label, side['calls']))
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        for key, value in stats.items():
            bump(key, int(bool(value)))
        bump('kind%d' % row['input']['kind'])
    if seen != set(range(oracle.CASES)):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {}
    for key, m in MINIMUMS:
        need(cov, key, m, key, shortfalls)
    for k in range(32):
        need(cov, 'kind%d' % k, 100, 'kind%d' % k, shortfalls)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != oracle.CASES or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if (len(state) != 1 or state[0]['vertex_hash'] != b['initial_vertex_hash'] or state[0]['index_hash'] != b['initial_index_hash']
            or state[0]['next_vertex'] != b['initial_next_vertex'] or state[0]['next_index'] != b['initial_next_index']
            or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True):
        raise RuntimeError('Los bufers de vertices/indices, los contadores, el x87 o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('spriteaddbuf28', manifest['binaries']['asi'], analyze, 'SpriteAddBuf28', timeout=1800))
