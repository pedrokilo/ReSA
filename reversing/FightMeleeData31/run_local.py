"""Compuertas independientes del banco FightMeleeData31 (CTaskSimpleFight::LoadMeleeData 0x6239F0), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x6239F0 salte a la DLL solo en el candidato), fp_difference vacio, la salida de CADA lado (AL de retorno, traza de los falsos en orden:
GetEventType x1/x2, SetDeleteCallback de la anim y del reposo, BlendAnimation con los bits de los argumentos, IsPlayer, FindTargetOnGround; y todos los cambios de bytes en `this`, las dos
asociaciones, el ped, los datos del jugador y el evento, guardas incluidas) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures y la restauracion
final (hook, entradas vigiladas, x87). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
EXPECTED = dict(oracle.CASES_BY_NAME)
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x5BEDC0}
FN = oracle.NAME
WATCHED = {0x538900, 0x536F80, 0x5389D0, 0x4D3A20}

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'combos_closed_0': 180,
        'combos_closed_13_plus': 165,
        'combos_closed_1_5': 1270,
        'combos_closed_6_12': 670,
        'ended_by_end_melee_data': 1950,
        'eof': 320,
        'lines_over_100': 550,
        'lines_over_40': 1430,
        'num_groups_negative': 420,
        'num_groups_positive': 1670,
        'num_groups_zero': 210,
        'seen_block_stale': 295,
        'seen_default_line': 355,
        'seen_f1c_skipped': 2100,
        'seen_f1c_stored': 1950,
        'seen_flags_stale': 300,
        'seen_group_count_nonpositive': 580,
        'seen_group_found': 1430,
        'seen_group_not_found': 1530,
        'seen_hit_level0': 2000,
        'seen_hit_level1': 1950,
        'seen_hit_level2': 1930,
        'seen_hit_level3': 1930,
        'seen_hit_level7': 2040,
        'seen_hit_lines': 2100,
        'seen_hit_stale': 1000,
        'seen_ignored_outside': 570,
        'seen_level_overflow': 60,
        'seen_level_partial': 740,
        'seen_level_vectors': 780,
        'seen_range_stale': 560,
        'seen_skipped': 1170,
        'seen_sound_default': 2030,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1 or begin[0].get('registered_hook') is not True or begin[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hook registrado, o sin los falsos de los colaboradores')
    b = begin[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y EAX/ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    stubbed = b['collaborators']['stubbed']
    if {w['address'] for w in b['watched_entries']} != WATCHED or {s['exe'] for s in stubbed} != WATCHED or len(stubbed) != 4 or b['collaborators']['exe_real'] != [{'sscanf': 0x8220AD}]:
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if any(not s['dll'] for s in stubbed):
        raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver')
    reg = b['region']
    if reg != {'base': 0xC17090, 'bytes': 0x8B0, 'table': 0xC170D0, 'levels': 0xC177D0, 'group_count': 0xB4EA28}:
        raise RuntimeError('Region observada inesperada')
    seen = set()
    cov = {}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn != FN or case in seen:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen.add(case)
        a, c = row['original'], row['candidate']
        for side in (a, c):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        want, stats = oracle.expected(fn, row['input'], case)
        for label, side in (('EXE', a), ('DLL', c)):
            if side['observed'] != want:
                diff = [k for k in want if side['observed'].get(k) != want[k]]
                raise RuntimeError('Salida distinta de la del oraculo en %s %d (%s), campos %s:\n  obtenido %s\n  esperado %s' % (fn, case, label, diff, side['observed'], want))
        for key, value in stats.items():
            if value is True:
                cov[key] = cov.get(key, 0) + 1
        if row['original_residue'] != row['candidate_residue']:
            cov['residue_differs'] = cov.get('residue_differs', 0) + 1
    if seen != set(range(EXPECTED[FN])):
        raise RuntimeError('Cobertura incompleta en ' + FN)
    shortfalls = {}
    for key, m in MINIMUMS[FN].items():
        if cov.get(key, 0) < m:
            shortfalls['%s:%s' % (FN, key)] = (cov.get(key, 0), m)
    if shortfalls:
        raise RuntimeError('Cobertura de estres insuficiente: ' + json.dumps(shortfalls))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['region_restored'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook, la entrada vigilada o la ruta no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fightmeleedata31', manifest['binaries']['asi'], analyze, 'FightMeleeData31', timeout=1800))
