"""Compuertas independientes del banco PickupsGenNew28 (CPickups::GenerateNewOne 0x456F20), 6144 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook "GenerateNewOne" registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/restauracion en AMBOS
lados (route_ok incluye que _ftol2 0x821B40 del EXE no este enganchado), fp_difference vacio, la salida de CADA lado (EAX = handle, ranuras cambiadas de la tabla, traza de los falsos con
la primera ranura distinta y sus 32 bytes en cada llamada, hora, mensaje y objetos falsos sin tocar) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las
fixtures y la restauracion final (hook, entradas de codigo vigiladas, tabla de pickups y hora del juego, x87).
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
EXPECTED = dict(oracle.CASES)
NAME = oracle.NAME
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {NAME: 0x456F20}
WATCHED = {0x456F20, 0x821B40, 0x455500, 0x454C70, 0x4567E0, 0x563280, 0x563220}
STUBBED = {0x455500, 0x454C70, 0x4567E0, 0x563280, 0x563220}

# Minimos de cobertura. Calibrados con fixture_dump.cpp (std puro, compilado aparte: vuelca los 6144 `input`) + fixture_oracle.py, dejando ~60 % de margen sobre los recuentos
# medidos (ver README.md). Si alguno falla por poco, bajar el umbral o ajustar la mezcla de la familia en Fixtures.hpp.
MINIMUMS = {
    'coord_huge': 400, 'coord_inf': 150, 'coord_int16_wrap': 550, 'coord_nan': 200, 'coord_neg': 1500, 'coord_nonzero': 2000,
    'empty_bit1': 200, 'empty_nonzero_bit0_clear': 80, 'empty_zero_high_garbage': 800, 'front_none': 150, 'invisible': 1000,
    'model_gt_16bit': 200, 'money_gt_16bit': 200, 'msg_null': 240, 'multi_free': 800, 'none': 150, 'now_wraps': 50,
    'old_flags_bit3': 1000, 'old_flags_low': 1700, 'old_flags_text': 1700, 'old_obj_nonzero_free': 750,
    'pass1_front': 650, 'pass1_top': 500, 'pass2': 450, 'pass2_with_t3': 420, 'pass3': 300, 'pass3_type4': 150, 'pass3_type5': 150,
    'ptype_0': 60, 'ptype_4': 60, 'ptype_5': 55, 'ptype_8': 65, 'ptype_9': 55, 'ptype_10': 60, 'ptype_11': 170, 'ptype_12': 60, 'ptype_13': 170, 'ptype_ge128': 100, 'ptype_ge14': 800,
    'ptype_high_garbage': 950, 'ref_fffd': 6, 'ref_ge8000': 950, 'ref_wrap': 15, 'replaced_null': 300, 'replaced_obj': 450,
    'slot_boundary': 570, 'slot_first': 70, 'slot_last': 40, 'text_ret_ge8': 380, 'top_failed_none': 5, 'top_failed_then_replace': 120,
    'top_trigger_empty': 280, 'top_trigger_ptype11': 170, 'top_trigger_ptype13': 170, 'visible': 950, 'visible_give_null': 300, 'visible_give_obj': 650,
}
for _k in range(16):
    MINIMUMS['kind%d' % _k] = 300


def analyze(path):
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin colaboradores sustituidos (este banco los sustituye)')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    if b.get('slots') != 620 or b.get('table_address') != 0x9788C0:
        raise RuntimeError('Tabla de pickups inesperada')
    watched = {w['address'] for w in b['watched_entries']}
    if watched != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas: %s' % sorted(watched))
    stubbed = b['collaborators']['stubbed']
    if {s['exe'] for s in stubbed} != STUBBED or not all(s['dll'] and s['dll'] != s['exe'] for s in stubbed) or set(b['collaborators']['exe_real']) != {0x821B40}:
        raise RuntimeError('Colaboradores sustituidos / reales inesperados: %s' % json.dumps(b['collaborators']))
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != NAME or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %d: %s' % (case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %d: %s' % (case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                raise RuntimeError('Salida distinta de la del oraculo en %d (%s):\n  obtenido %s\n  esperado %s' % (case, label, side['observed'], want))

        def bump(key, cond=True):
            if cond:
                cov[key] = cov.get(key, 0) + 1
        bump('residue_differs', row['original_residue'] != row['candidate_residue'])
        bump('kind%d' % row['input']['kind'])
        for key, value in stats.items():
            if value is True:
                bump(key)
    if seen != set(range(EXPECTED[NAME])):
        raise RuntimeError('Cobertura incompleta')
    shortfalls = {k: (cov.get(k, 0), m) for k, m in MINIMUMS.items() if cov.get(k, 0) < m}
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True or state[0]['table_restored'] is not True:
        raise RuntimeError('El x87, los hooks, las entradas vigiladas, la tabla de pickups o la hora no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('pickupsgen28', manifest['binaries']['asi'], analyze, 'PickupsGenNew28', timeout=1800))
