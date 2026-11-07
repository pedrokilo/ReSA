"""Compuertas del banco Fn113A (lote 113A): 17 funciones pequenas de tareas, eventos, pools y registro de relaciones (hook S), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: valor devuelto, eventos de los falsos con todos sus argumentos y
bytes cambiados de las arenas), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo y coverage.json = minimos calibrados).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza.
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

NAMES = ['CTaskComplexEnterCar::ComputeTargetDoorOppositeToFlag', 'CTaskComplexWander::RequestDir', 'CTaskComplexWander::RequestNodesFrom', 'CTaskComplexWanderStandard::SetMinNextScanTime',
         'CTaskComplexPartner::GetStateOfPartnerTask', 'GangDrinkAnimOfPed', 'GangSmokeAnimOfPed', 'CTaskAllocator::operator new', 'CTaskAllocator::operator delete', 'CPedIntelligence::operator new',
         'CPedIntelligence::operator delete', 'CPlayerRelationshipRecorder::ClearRelationshipWithPlayer', 'CPlayerRelationshipRecorder::AddRelationship', 'CPlayerRelationshipRecorder::GetRelationshipWithPlayer',
         'CPlayerRelationshipRecorder::Flush', 'CEventScanner::ScanForEventsNow', 'CTaskTimer::GetTimeLeft']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x63AB70, 0x669D30, 0x669D50, 0x66B150, 0x6822B0, 0x65E6F0, 0x65E710, 0x69D980, 0x69D990, 0x6074A0, 0x6074E0, 0x61A150, 0x61A180, 0x61A1A0, 0x61A2A0, 0x6053D0, 0x62EB80)))
# Colaboradores con hook RH en la DLL (apagado durante el banco): el mismo falso va en la direccion del EXE y en el cuerpo de la DLL
STUBBED = {n: [] for n in NAMES}
STUBBED[NAMES[4]] = [(0x681740, True)]
STUBBED[NAMES[15]] = [(0x603720, True)]
# Colaboradores con hook que corren DE VERDAD en ambos lados (el EXE ejecuta sus bytes originales)
REAL = {n: [] for n in NAMES}
REAL[NAMES[7]] = [0x69D8E0]
REAL[NAMES[9]] = [0x605EC0]
DLL_BODY_WITHOUT_HOOK = set()
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a in lst} | set(HOOKS.values())
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
POOL_NEW = ['allocated', 'full', 'wrapped', 'cursor_minus1', 'id_wraps']
POOL_DEL = ['delete', 'cursor_kept_below', 'cursor_equal', 'cursor_lowered', 'interior']
REC = ['null_arg', 'arg', 'no_match', 'match_first', 'match_later', 'entry0_null']
KEYS = {
    NAMES[0]: ['driver', 'passenger', 'door8', 'other_door', 'result_10', 'result_0'],
    NAMES[1]: ['equal', 'different', 'different_high_bits_only', 'flag_already_set'],
    NAMES[2]: ['same', 'diff_w0', 'diff_w1', 'diff_w2', 'diff_w3', 'all_differ', 'flag_already_set'],
    NAMES[3]: ['same_value', 'new_value'],
    NAMES[4]: ['no_partner_task', 'partner_task', 'negative_state', 'state_lt5', 'state_ge5'],
    NAMES[5]: ['female_model', 'other_model'],
    NAMES[6]: ['female_model', 'other_model'],
    NAMES[7]: POOL_NEW,
    NAMES[8]: POOL_DEL,
    NAMES[9]: POOL_NEW,
    NAMES[10]: POOL_DEL,
    NAMES[11]: REC,
    NAMES[12]: ['entry0_used', 'entry0_empty', 'value_high_bits'],
    NAMES[13]: REC,
    NAMES[14]: ['flush'],
    NAMES[15]: ['dont_scan', 'dont_scan_garbage', 'scan', 'timer_started', 'timer_stopped', 'started_garbage'],
    NAMES[16]: ['started', 'not_started', 'negative', 'positive', 'zero', 'stopped_flag'],
}


def tally(inp, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    e = oracle.expected(inp)
    for m in set(e['marks']):
        cov[m] = cov.get(m, 0) + 1


def analyze(path):
    path = Path(path)
    if not path.exists():
        return None
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    b = [r for r in rows if r['event'] == 'begin']
    if len(b) != 1 or b[0].get('registered_hook') is not True or b[0].get('collaborators_stubbed') is not True:
        raise RuntimeError('Inicio sin hooks registrados, o sin los falsos de los colaboradores')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        got_real = [r['exe'] for r in col[fn]['real_with_hook_off']]
        if got_real != REAL.get(fn, []):
            raise RuntimeError('Colaboradores reales inesperados en ' + fn)
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas (rutinas de RenderWare) se llaman por su direccion del EXE
        if any(s['dll'] is not (s['hooked'] or s['exe'] in DLL_BODY_WITHOUT_HOOK) for s in col[fn]['stubbed']):
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver en ' + fn)
    if {w['address'] for w in b['watched_entries']} != WATCHED:
        raise RuntimeError('Entradas vigiladas inesperadas')
    seen = {fn: set() for fn in NAMES}
    fam = {fn: {} for fn in NAMES}
    cov = {fn: {} for fn in NAMES}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido')
        if row['event'] != 'comparison':
            continue
        fn, case = row['function'], row['case']
        if fn not in seen or case in seen[fn]:
            raise RuntimeError('Caso repetido o funcion inesperada %s %d' % (fn, case))
        seen[fn].add(case)
        for side in (row['original'], row['candidate']):
            if not all(side.get(k) is True for k in GATES):
                raise RuntimeError('Compuerta fallida en %s %d: %s' % (fn, case, {k: side.get(k) for k in GATES}))
        if row['fp_difference']:
            raise RuntimeError('Estado x87 distinto en %s %d: %s' % (fn, case, row['fp_difference']))
        if row['original'] != row['candidate']:
            raise RuntimeError('EXE y DLL difieren en %s %d' % (fn, case))
        bad = oracle.check_row(row)
        if bad:
            raise RuntimeError('El oraculo independiente no coincide (%s) en %s %d: %s' % (bad[0], fn, case, bad[1]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        need = {k: COVER_FLOOR for k in KEYS[fn]}
        for key, minimum in COVER.get(fn, {}).items():
            need[key] = max(need.get(key, 0), minimum)
        short = {k: (cov[fn].get(k, 0), m) for k, m in need.items() if cov[fn].get(k, 0) < m}
        if short:
            raise RuntimeError('Cobertura de ramas insuficiente en %s: %s medida: %s' % (fn, json.dumps(short), json.dumps(cov[fn])))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    summary['independent_oracle'] = 'fixture_oracle.py: cada lado de cada par coincide con el modelo'
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('fn113a', manifest['binaries']['asi'], analyze, 'Fn113A', timeout=3600))
