"""Compuertas del banco Fn129A (lote 129A): catorce funciones (EXE 0x463370, 0x4B25E0 (V), 0x4DA510, 0x41BDD0, 0x4E0380, 0x56CE30, 0x4EE9A0, 0x40F700, 0x6C4130, 0x4049C0, 0x43AA40, 0x705480, 0x70F890
y 0x40F030; hook S salvo la V), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados, eventos de los colaboradores, valor devuelto, ST0 de 80 bits en la funcion 2), la cobertura por familia de las
fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS con minimo y coverage.json = minimos calibrados).
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

NAMES = list(oracle.NAMES)
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, oracle.ENTRIES))
SLOTS = ()
# colaboradores sustituidos por falsos: (direccion del EXE, hooked). Con hook RH en la DLL (apagado durante el banco) el mismo falso va en el cuerpo de la DLL; los que no tienen hook (operator delete 0x8214BD)
# los llama la DLL por la direccion del EXE. REAL: colaboradores sin falso (el EXE corre sus bytes; la DLL no los llama).
STUBBED = {n: [] for n in NAMES}
STUBBED[NAMES[7]] = [(0x416400, True), (0x72F430, True), (0x40F070, True), (0x8214BD, False)]
STUBBED[NAMES[9]] = [(0x8213AE, False)]
STUBBED[NAMES[11]] = [(0x7FB020, False), (0x7FB230, False), (0x7F35D0, False)]
REAL = {NAMES[5]: [(0x821D30, False), (0x56CDE0, False)]}
DLL_BODY_WITHOUT_HOOK = set()
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
THIS = ['this_aligned', 'this_unaligned', 'this_spill', 'fill0', 'fill1', 'fill2', 'fill3', 'kind0', 'kind1', 'kind2', 'kind3']
KEYS = {
    NAMES[0]: THIS + ['separate', 'overlap_zero', 'overlap_forward', 'overlap_backward', 'changes0', 'changes_some'],
    NAMES[1]: THIS + ['differ', 'equal', 'differ_high', 'differ_other'],
    NAMES[2]: ['zero', 'negative', 'one', 'normal', 'index_high', 'index_plain'],
    NAMES[3]: THIS + ['broken', 'sound', 'arg_on', 'arg_off', 'was_on', 'was_off', 'changes0', 'changes_some'],
    NAMES[4]: THIS + ['uninit', 'init', 'slot_negative', 'slot_zero', 'slot_positive', 'sound_beyond', 'sound_in_range', 'dirty_high', 'clean_high'],
    NAMES[5]: THIS + ['active0', 'active1', 'active2', 'active3', 'active4', 'active5', 'active8', 'active16', 'changes0', 'changes_some'],
    NAMES[6]: THIS + ['changes_some', 'changes_many', 'handle_was_ones', 'handle_other'],
    NAMES[7]: THIS + ['inactive_flag', 'null_data', 'single_alloc', 'separate_alloc', 'ptr_arena', 'ptr_null', 'ptr_raw'],
    NAMES[8]: THIS + ['angle_zero', 'angle_normal', 'angle_denormal', 'angle_inf', 'angle_nan', 'angle_neg', 'angle_pos', 'angle_huge', 'angle_small', 'angle_mid'],
    NAMES[9]: THIS + ['empty', 'owner', 'not_owner', 'cap_negative', 'cap_zero', 'cap_positive'],
    NAMES[10]: ['slot0', 'slot1', 'slot2', 'slot3', 'duplicate', 'unique', 'byte_zero', 'byte_one', 'byte_other', 'dirty_high', 'clean_high', 'changes0', 'changes1'],
    NAMES[11]: THIS + ['raster_null', 'raster_arena', 'raster_raw', 'tex_null', 'tex_arena', 'tex_raw', 'width_zero', 'width_negative', 'width_positive'],
    NAMES[12]: ['place0', 'place1', 'place2', 'place3', 'place4', 'place5', 'place6', 'place7', 'res_nan', 'res_inf', 'res_zero', 'res_finite', 'in_nan', 'in_inf', 'in_denormal', 'in_plain',
                'overlap', 'apart'],
    NAMES[13]: THIS + ['changes0', 'changes_some', 'flags_kept', 'flags_clear', 'low_bits_set', 'low_bits_clear']}


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
        got = [(s['exe'], s['hooked']) for s in col[fn]['real']]
        if got != REAL.get(fn, []):
            raise RuntimeError('Colaboradores reales inesperados en ' + fn)
        # los colaboradores con hook RH en la DLL (apagado durante el banco) llevan el mismo falso en su cuerpo del ASI; los demas (rutinas de RenderWare) se llaman por su direccion del EXE
        if any(s['dll'] is not (s['hooked'] or s['exe'] in DLL_BODY_WITHOUT_HOOK) for s in col[fn]['stubbed'] + col[fn]['real']):
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
        sys.exit(runtime_runner.run_case('fn129a', manifest['binaries']['asi'], analyze, 'Fn129A', timeout=3600))
