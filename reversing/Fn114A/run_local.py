"""Compuertas del banco Fn114A (lote 114A): catorce funciones sueltas (hook S), 16384 pares cada una.

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

NAMES = ['CPlaceable::GetRightVector', 'CPlaceable::GetUpVector', 'CPhysical::ApplyMoveForce', 'CPedIntelligence::GetPedFOVRange', 'CPedGroups::RegisterKillByPlayer', 'CMissionCleanup::AddEntityToList',
         'CEventDamage::IsSameEventForAI', 'CEntryExitManager::WeAreInInteriorTransition', 'CPointRoute::AddPoint', 'CPointRoute::RemovePoint', 'CPedGroupIntelligence::GetTask',
         'CEventGroupEvent::BaseEventTakesPriorityOverBaseEvent', 'CEvent::TakesPriorityOverBase', 'IsPedPointerValid']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x41CC70, 0x50E420, 0x4ABBA0, 0x4ABEA0, 0x5F7E30, 0x4637E0, 0x4ADBF0, 0x43E400, 0x48E180, 0x5F13A0, 0x5F7660, 0x4AE100, 0x4ABC30, 0x5E8A30)))
# (direccion del EXE, hooked): con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook (hooked False) la DLL llama a la direccion del EXE
STUBBED = {n: [] for n in NAMES}
STUBBED[NAMES[2]] = [(0x5429F0, True)]
STUBBED[NAMES[13]] = [(0x533310, True), (0x56E210, True)]
DLL_BODY_WITHOUT_HOOK = set()
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {
    NAMES[0]: ['matrix', 'heading', 'heading_finite', 'heading_zero', 'heading_nan', 'heading_inf', 'heading_tiny'],
    NAMES[1]: ['matrix', 'no_matrix', 'row_nan', 'row_snan', 'unchanged'],
    NAMES[2]: ['finite', 'nan_component', 'snan_component', 'inf_component'],
    NAMES[3]: ['hearing_greater', 'seeing_greater', 'equal', 'nan_hearing', 'nan_seeing', 'infinite', 'zeros_01', 'zeros_10'],
    NAMES[4]: ['flag_set', 'flag_clear', 'garbage_above_flag', 'counter_wrap'],
    NAMES[5]: ['full', 'slot_first', 'slot_middle', 'slot_last', 'type_zero', 'type_garbage', 'count_wrap', 'count_negative'],
    NAMES[6]: ['same', 'diff_weapon', 'diff_source', 'diff_both', 'alias'],
    NAMES[7]: ['state_zero', 'state_nonzero', 'state_zero_low_byte'],
    NAMES[8]: ['full', 'negative_count', 'overlap_count', 'added_first', 'added_middle', 'added_last', 'nan_point'],
    NAMES[9]: ['passes_0', 'passes_1', 'passes_2', 'passes_3plus', 'negative_index', 'count_nonpositive', 'index_beyond', 'remove_last', 'remove_first', 'remove_middle', 'count_over_8'],
    NAMES[10]: ['match_first', 'match_middle', 'match_last', 'no_match', 'null_search', 'raw_ped', 'duplicate_first_wins'],
    NAMES[11]: ['this_priority', 'this_plain', 'other_priority', 'other_plain', 'same_event', 'virtual_true', 'virtual_false', 'virtual_garbage', 'type_outside'],
    NAMES[12]: ['alias', 'distinct', 'greater', 'equal', 'less', 'negative_priority', 'sign_differs'],
    NAMES[13]: ['null_ped', 'below_storage', 'above_storage', 'index_negative', 'index_beyond_capacity', 'index_truncated_to_zero', 'slot_free', 'in_vehicle', 'vehicle_true', 'vehicle_false',
                'in_vehicle_without_vehicle', 'collision_node', 'player_match', 'player_mismatch'],
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
        sys.exit(runtime_runner.run_case('fn114a', manifest['binaries']['asi'], analyze, 'Fn114A', timeout=3600))
