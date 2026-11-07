"""Compuertas del banco Fn116A (lote 116A): dieciocho funciones sueltas (hook S), 16384 pares cada una.

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

NAMES = ['CBoundingBox::IsPointWithin', 'CColBox::Set', 'CColBox::operator=', 'tColLighting::GetCurrentLighting', 'CPad::sub_540530', 'FightHitLevel', 'FightHitSound', 'approxEqual', 'ConvertPedNode2BoneTag', 'CStuckCarCheck::ResetArrayElement', 'CEventDamage::ComputeHeadShot', 'FindCutsceneAudioTrackId', 'CSimpleTransform::Invert', 'TransformVectors', 'CGarages::IsPointWithinAnyGarage', 'CGarages::IsPointInAGarageCameraZone', 'IsTeleportPointValid', 'COnscreenTimer::Init']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x49C300, 0x40EE70, 0x40EEC0, 0x59F0C0, 0x540530, 0x5BD360, 0x5BD3B0, 0x4EEA80, 0x4D58A0, 0x463970, 0x4ADD60, 0x5AFA50, 0x54EF90, 0x54EE30, 0x448990, 0x449BA0, 0x43EA90, 0x44CBC0)))
# (direccion del EXE, hooked): con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook (hooked False) la DLL llama a la direccion del EXE
STUBBED = {n: [] for n in NAMES}
STUBBED[NAMES[14]] = [(0x448740, True)]
STUBBED[NAMES[15]] = [(0x4487D0, True)]
STUBBED[NAMES[16]] = [(0x569E20, True), (0x56A490, True)]
DLL_BODY_WITHOUT_HOOK = set()
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
KEYS = {
    NAMES[0]: ['above_max', 'below_min', 'boundary', 'infinite', 'inside', 'inverted_box', 'nan'],
    NAMES[1]: ['garbage_high_bytes', 'inf_alias', 'inf_separate', 'nan_vector', 'sup_alias', 'sup_separate'],
    NAMES[2]: ['overlap', 'self', 'src_inside', 'src_separate'],
    NAMES[3]: ['balance_outside_unit', 'beyond_float_range', 'day_nonzero', 'day_zero', 'garbage_high_bytes', 'inf_input', 'nan_input', 'night_nonzero', 'night_zero', 'result_finite', 'result_inf', 'result_nan', 'result_zero'],
    NAMES[4]: ['down_new', 'mode_1', 'mode_out_of_range', 'mode_select', 'no_new', 'select_new', 'select_not_new', 'up_new'],
    NAMES[5]: ['level_0', 'level_1', 'level_2', 'level_3', 'level_7', 'word_pair'],
    NAMES[6]: ['out_of_range', 'table_1', 'table_2', 'table_3', 'table_4', 'table_5', 'table_6', 'table_7', 'table_8', 'wrap_edge'],
    NAMES[7]: ['difference_beyond_float', 'difference_infinite', 'equal', 'greater', 'less', 'nan'],
    NAMES[8]: ['node_1', 'node_2', 'node_3', 'node_4', 'node_5', 'node_6', 'node_7', 'node_8', 'node_9', 'node_10', 'node_11', 'node_12', 'node_13', 'node_14', 'node_15', 'node_16', 'node_17', 'node_18', 'unknown_node'],
    NAMES[9]: ['garbage_high_word', 'index_first', 'index_last', 'index_middle'],
    NAMES[10]: ['firearm', 'headshot', 'melee', 'no_effect', 'no_headshot', 'weapon_outside_table'],
    NAMES[11]: ['case_mix', 'empty', 'exact', 'found', 'found_other_case', 'name_plus', 'not_found', 'one_char', 'one_letter_case', 'prefix', 'random_bytes'],
    NAMES[12]: ['alias', 'heading_inf', 'heading_nan', 'heading_norm', 'heading_sub', 'heading_zero', 'inf_component', 'nan_component', 'separate', 'z_signaling'],
    NAMES[13]: ['alias', 'count_0', 'count_1', 'count_many', 'heading_inf', 'heading_nan', 'heading_norm', 'heading_sub', 'heading_zero', 'inf_component', 'nan_component', 'separate'],
    NAMES[14]: ['high_type', 'hit_at_first', 'hit_at_later', 'hit_last_garage', 'many_calls', 'no_active', 'no_hit'],
    NAMES[15]: ['high_type', 'hit_at_first', 'hit_at_later', 'hit_last_garage', 'many_calls', 'no_active', 'no_hit'],
    NAMES[16]: ['garbage_high_bytes', 'los_blocked', 'los_clear', 'sphere_blocked'],
    NAMES[17]: ['init'],
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
        sys.exit(runtime_runner.run_case('fn116a', manifest['binaries']['asi'], analyze, 'Fn116A', timeout=3600))
