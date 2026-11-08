"""Compuertas del banco Fn124A (lote 124A): trece funciones diminutas (EXE 0x617850, 0x5720C0, 0x5D0F40, 0x5F6660, 0x538AD0, 0x646AB0 (V), 0x420860, 0x5617A0, 0x59E620, 0x502700, 0x503430, 0x56F850 y
0x706850; hook S salvo la V), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion en AMBOS lados, fp_difference vacio,
que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados, eventos de los colaboradores, valor devuelto, ST0 de 80 bits en las funciones 4 y 6), la cobertura por familia de las
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
STUBBED = {
    NAMES[0]: [], NAMES[1]: [(0x6A0050, True)], NAMES[2]: [], NAMES[3]: [(0x61A3B0, True)], NAMES[4]: [], NAMES[5]: [(0x5DF910, True)], NAMES[6]: [], NAMES[7]: [], NAMES[8]: [],
    NAMES[9]: [(0x5023F0, True), (0x8214BD, False)], NAMES[10]: [(0x503290, True), (0x8214BD, False)], NAMES[11]: [(0x4E6E00, True), (0x8214BD, False)], NAMES[12]: [(0x706440, True), (0x8214BD, False)]}
REAL = {NAMES[0]: [(0x4A8DB0, False)], NAMES[4]: [(0x823820, False)], NAMES[8]: [(0x59D170, False), (0x821D30, False)], NAMES[11]: [(0x56F200, False)]}
DLL_BODY_WITHOUT_HOOK = set()
WATCHED = set(HOOKS.values()) | set(SLOTS) | {a for lst in STUBBED.values() for a, _ in lst} | {a for lst in REAL.values() for a, _ in lst} | {0x8580C8}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
THIS = ['this_aligned', 'this_unaligned', 'this_spill', 'fill0', 'fill1', 'fill2', 'fill3', 'kind0', 'kind1', 'kind2', 'kind3']
SCALAR = THIS + ['delete', 'keep', 'flags_zero', 'flags_one', 'flags_bit0_more', 'flags_upper_only', 'flags_even']
KEYS = {
    NAMES[0]: THIS + ['changes0', 'changes8', 'changes_some', 'prev_zero', 'prev_ones', 'prev_partial'],
    NAMES[1]: THIS + ['key_zero', 'key_ones', 'key_other', 'ret_zero', 'ret_nonzero'],
    NAMES[2]: ['slot_valid', 'slot_negative', 'slot_high', 'wraps', 'in_range'],
    NAMES[3]: THIS + ['changes0', 'changes1', 'changes2', 'changes3', 'changes4', 'prev_same', 'prev_zero', 'prev_ones', 'prev_other'],
    NAMES[4]: ['nan_q', 'nan_s', 'inf', 'zero', 'integer', 'frac_pos', 'frac_neg', 'neg_to_zero', 'denormal_pos', 'class_denormal', 'class_normal'],
    NAMES[5]: THIS + ['vehicle', 'no_vehicle', 'ped_null', 'ped_arena', 'ped_raw', 'ret_al_hidden', 'ret_al_one'],
    NAMES[6]: THIS + ['nan_x', 'nan_y', 'nan_both', 'inf', 'zero', 'normal', 'above_float', 'below_float', 'x_denormal', 'x_zero', 'x_inf', 'y_denormal', 'y_zero', 'y_inf'],
    NAMES[7]: ['bool_one', 'bool_zero', 'bool_other', 'hi_zero', 'hi_set'],
    NAMES[8]: THIS + ['active0', 'active1', 'active2', 'active3', 'active4', 'active5', 'active8', 'active16', 'active32', 'active64', 'changes0', 'changes_some'],
    NAMES[9]: SCALAR, NAMES[10]: SCALAR, NAMES[11]: SCALAR, NAMES[12]: SCALAR}


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
        sys.exit(runtime_runner.run_case('fn124a', manifest['binaries']['asi'], analyze, 'Fn124A', timeout=3600))
