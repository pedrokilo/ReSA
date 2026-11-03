"""Compuertas del banco Fn85I (lote 85I): CTaskSimpleFight::StartAnim (EXE 0x623B10; hook S), 16384 pares.

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

NAMES = ['CTaskSimpleFight::StartAnim']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x623B10,)))
# (direccion del EXE, hooked): con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; sin hook (hooked False) la DLL llama a la direccion del EXE
STUBBED = {NAMES[0]: [(0x4CEBC0, True), (0x4CEBE0, True), (0x4CEA80, True), (0x4D4610, True), (0x5DF8F0, True), (0x5DEC00, True), (0x559AF0, True), (0x61C9B0, True), (0x6010A0, True),
                    (0x743C60, True), (0x4D3A30, True), (0x4D3940, True), (0x4D3FB0, True), (0x4D3FD0, False), (0x4087E0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | set(HOOKS.values())
COVER_FLOOR = 100
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}

KEYS = {
    NAMES[0]: ['neg_attack', 'old_anim_released', 'no_old_anim', 'cmd_oob_neg', 'cmd_oob_pos', 'st0', 'st0_mstate_hi', 'st0_finish', 'st0_mstate_hi_notplayer', 'st0_idle_new', 'st0_idle_redo',
               'st0_idle_keep_dlt', 'st0_idle_keep_amt', 'st0_player', 'st0_notplayer', 'gacs_ret4_flag', 'gacs_group21', 'gacs_same_ref', 'gacs_same_noref', 'gacs_diff_ref', 'gacs_diff_noref',
               'gacs_remove_null_block', 'gacs_cached', 'gacs_lookup', 'gacs_add_ref', 'gacs_request', 'gacs_next_lowered', 'gacs_next_untouched',
               'st1', 'st1_pd', 'st1_nopd', 'st1_ratio2', 'st1_ratio1', 'st1_ratio_none', 'st1_ms6', 'st1_ms4', 'st1_ms1', 'st1_blend1', 'st1_blend0', 'st1_blendF', 'st1_duck', 'st1_duck_notask',
               'st1_duck_noflag', 'st1_blend3', 'st1_idle', 'st1_noidle', 'st2', 'st2_flag2_clear', 'st2_flag2_set', 'st3', 'st3_player', 'st3_notplayer', 'st3_cmd3', 'st3_cmd4', 'st3_cmd5',
               'st3_cmd6', 'st3_cmd7', 'st3_cmd8', 'st3_cmd9', 'st3_cmd10', 'st3_delete', 'st3_finish', 'st4', 'st4_move3', 'st4_move3_time0', 'st4_move3_timenz', 'st4_move3_timenan', 'st4_pd',
               'st4_nopd', 'st4_stat_snan', 'st4_move_lt4', 'st4_move_ge4', 'mut1', 'mut2', 'mut3', 'mut4', 'mut5', 'mut6', 'mut7'],
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
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas se llaman por su direccion del EXE
        if not all(s['dll'] is True for s in col[fn]['stubbed']):
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
        sys.exit(runtime_runner.run_case('fn85i', manifest['binaries']['asi'], analyze, 'Fn85I', timeout=3600))
