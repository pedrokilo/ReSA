"""Compuertas independientes del banco WalkRoundFirst31 (CTaskComplexWalkRoundObject::CreateFirstSubTask 0x657380), 4608 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige: hook virtual registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS
lados (route_ok incluye que la entrada 0x657380 salte a la DLL solo en el candidato; collaborators_dll exige que operator new 0x61A5A0, LimitRadianAngle 0x53CB50 y GetRadianAngleBetweenPoints 0x53CBE0
sean saltos a la DLL en ambos), fp_difference vacio, la salida de CADA lado (retorno, llamadas a los falsos de AllocateMatrix/UpdateMatrix/0x6551D0/CreateRouteTask, pool, tarea nueva con
todos sus bytes (el angulo se acepta a 1e-4 rad: fpatan no se modela al bit; EXE == DLL si se exige al bit), cambios en `this`, objeto/matrices/peaton falsos, la ruta y el reloj) igual a la que modela fixture_oracle.py desde el desensamblado, la cobertura de las fixtures
y la restauracion final (hook, entradas vigiladas, x87, pool, reloj).
Si el oraculo discrepa SOLO en bytes de la tarea nueva (campo `tasks`), el defecto es del constructor reconstruido de CTaskSimpleAchieveHeading, no de CreateFirstSubTask: ver README.md.
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
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
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {oracle.NAME: 0x657380}
FN = oracle.NAME
COLLABORATORS = {0x61A5A0, 0x53CB50, 0x53CBE0}
WATCHED_ONLY = {0x667E20, 0x59C970, 0x54F560, 0x54F1B0, 0x6551D0, 0x655140}
WATCHED = COLLABORATORS | WATCHED_ONLY

# Minimos de cobertura. Calibrados con fixture_mirror.py (el generador real de Fixtures.hpp compilado aparte): ~45-60 % de lo medido.
MINIMUMS = {
    FN: {
        'alloc_fail': 213, 'alloc_ok': 1187, 'count_odd': 86, 'dist_inf': 47, 'dist_nan': 82, 'dist_neg': 68, 'facing': 914, 'heading_checked': 1349,
        'heading_unchecked': 51, 'id_wrap': 54, 'last_at_cap1': 235, 'mag_inf': 7, 'mag_nan': 67, 'mag_zero_vec': 166, 'mat_missing': 343, 'move4': 722, 'move6': 568,
        'move_other': 566, 'move_weird': 238, 'not_facing': 942, 'obj_null': 169, 'path_heading': 1400, 'path_route': 456, 'pos_from_placement': 343, 'route_empty': 277,
        'route_nonempty': 1857, 'route_ret_null': 41, 'search_wraps': 428,
    },
}
for _k in range(16):
    MINIMUMS[FN]['family%d' % _k] = 288
MINIMUMS[FN]['pool_mode0'] = 539
MINIMUMS[FN]['pool_mode1'] = 263
MINIMUMS[FN]['pool_mode2'] = 297
MINIMUMS[FN]['pool_mode3'] = 289
MINIMUMS[FN]['pool_mode4'] = 282
MINIMUMS[FN]['pool_mode5'] = 26
MINIMUMS[FN]['pool_mode6'] = 267
MINIMUMS[FN]['pool_mode7'] = 339


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
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md): x87 normalizado y ECX/EDX solo registrados')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != 1:
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or set(b['bodies']) != set(HOOKS) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    stubbed = col['stubbed']
    if ({w['address'] for w in b['watched_entries']} != WATCHED or set(col['dll_real']) != COLLABORATORS or set(col['watched_only']) != WATCHED_ONLY
            or sorted(s['exe'] for s in stubbed) != sorted(WATCHED_ONLY - {0x667E20, 0x59C970}) or not all(s['dll'] for s in stubbed if s['exe'] in (0x54F560, 0x54F1B0))):
        raise RuntimeError('Colaboradores o entradas vigiladas inesperados')
    if b['pool_capacity'] < 256:
        raise RuntimeError('Pool de tareas insuficiente')
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
            observed = oracle.match(side['observed'], want, stats)
            if observed != want:
                diff = [k for k in want if observed.get(k) != want[k]]
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
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, el hook, las entradas vigiladas, el pool o el reloj no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['coverage'] = cov
    summary['coverage_method'] = 'serialized_inputs_and_independent_oracle; no_instruction_branch_tracing'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('walkroundfirst31', manifest['binaries']['asi'], analyze, 'WalkRoundFirst31', timeout=1800))
