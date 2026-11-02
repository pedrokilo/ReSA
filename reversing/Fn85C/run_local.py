"""Compuertas del banco Fn85C (lote 85C): dos metodos de CGarage y dos de CTaskSimpleFight (EXE 0x4494F0, 0x449A50, 0x61C650, 0x624A40; hook S), 65536 pares (16384 por funcion).

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: valor devuelto, eventos de los falsos con todos sus argumentos,
bytes cambiados de las arenas, posicion de la puerta bit a bit con la aritmetica x87 a 24 bits), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas
y el modelo (KEYS con minimo y coverage.json = minimos calibrados).
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

NAMES = ['CGarage::IsAnyCarBlockingDoor', 'CGarage::StoreAndRemoveCarsForThisImpoundingGarage', 'CTaskSimpleFight::BeHitWhileBlocking', 'CTaskSimpleFight::ChooseAttackAI']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x4494F0, 0x449A50, 0x61C650, 0x624A40)))
# (direccion del EXE, hooked): con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook (hooked False) la DLL llama a la direccion del EXE
STUBBED = {NAMES[0]: [(0x448EE0, True), (0x535300, True), (0x59C890, True), (0x4487D0, True)],
           NAMES[1]: [(0x448740, True), (0x449050, True), (0x449760, True), (0x56E860, True), (0x563280, True)],
           NAMES[2]: [],
           NAMES[3]: [(0x821B1E, False), (0x41BE60, True), (0x61D6F0, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = {
    NAMES[0]: ['size_zero', 'size_8', 'free_skipped', 'touch_false', 'touch_true', 'no_spheres', 'neg_count', 'with_matrix_veh', 'no_matrix_veh', 'multi_sphere', 'reenter_applied', 'last_idx3',
               'blocked_first_sphere', 'blocked_later_sphere', 'ret_true', 'ret_false'],
    NAMES[1]: ['size_zero', 'size_8', 'free_skipped', 'outside_skipped', 'mission_skipped', 'with_matrix', 'no_matrix', 'sphere_outside', 'stored', 'store_blocked_by_lim', 'removed', 'full_four', 'lim_neg', 'lim_zero', 'lim_over', 'lim_mid'],
    NAMES[2]: ['bad_lastcmd', 'anim_null', 'anim_flag', 'total_not_greater', 'total_nan', 'behind', 'not_behind', 'dot_nan', 'set_group1', 'set_group9', 'set_group7', 'set_bad', 'set_ok', 'move1_blocked',
               'ret_true', 'ret_false', 'creator_mat', 'creator_nomat', 'nan_input', 'inf_input', 'denormal_input'],
    NAMES[3]: ['path_a', 'a_no40', 'a_notarget', 'a_nonped', 'a_nopd', 'a_wanted_le0', 'a_health_low', 'a_ok', 'path_b', 'b_reach_flag4_ret3', 'b_reach_false', 'ret0', 'ret1', 'ret2', 'ret3',
               'mask_applied', 'cset_neg', 'cset_ge16', 'roll_big', 'roll_gt08', 'roll_gt05_only'],
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
    if b.get('rng_native') is not True:
        raise RuntimeError('rand() del EXE no nativo')
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
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas se llaman por su direccion del EXE
        if any(s['dll'] is not s['hooked'] for s in col[fn]['stubbed']):
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
        sys.exit(runtime_runner.run_case('fn85c', manifest['binaries']['asi'], analyze, 'Fn85C', timeout=3600))
