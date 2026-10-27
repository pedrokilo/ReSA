"""Compuertas del banco CarLink74 (lote 74): CCarCtrl::ClipTargetOrientationToLink (EXE 0x422760, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo del oraculo (coverage.json = minimos).
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

NAMES = ['CCarCtrl::ClipTargetOrientationToLink']
CASES = 18432   # 18 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x422760}
# (direccion del EXE, hooked): CGeneral::GetATanOfXY (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x53CC70, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x422760}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


FLOATS = ('mpos_x', 'mpos_y', 'ppos_x', 'ppos_y', 'arg5', 'arg6')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('null_node', inp['null_node'] == 1)
    add('real_atan', inp['real'] == 1)
    add('has_mat', inp['has_mat'] == 1 and not inp['null_node'])
    add('no_mat', inp['has_mat'] == 0 and not inp['null_node'])
    add('arg3_hi_garbage', (inp['arg3'] >> 8) != 0)
    add('arg2_hi_garbage', inp['hi16'] != 0)
    a3 = oracle.s8(inp['arg3'])
    if inp['null_node']:
        add('null_node_no_calls', obs['atan_calls'] == 0 and obs['events'] == [])
        return
    add('arg3_zero', a3 == 0)
    add('arg3_negative', a3 < 0)
    add('arg3_positive', a3 > 0)
    cl, al = inp['flags'] & 7, (inp['flags'] >> 3) & 7
    add('lanes_a_zero', cl == 0)
    add('lanes_b_zero', cl != 0 and al == 0)
    add('lanes_both_arg3_zero', cl != 0 and al != 0 and a3 == 0)
    add('lanes_both_arg3_nonzero', cl != 0 and al != 0 and a3 != 0)
    add('lanes_flag_bits67', (inp['flags'] >> 6) != 0)
    add('dir_extreme', oracle.s8(inp['dirx']) in (-128, 127) or oracle.s8(inp['diry']) in (-128, 127))
    add('dir_zero', oracle.s8(inp['dirx']) == 0 and oracle.s8(inp['diry']) == 0)
    for nm in FLOATS + ('old',):
        add('special_' + nm, is_special(inp[nm]))
        add('denormal_' + nm, is_denormal(inp[nm]))
        add('nan_noncanonical_' + nm, noncanonical_nan(inp[nm]))
    add('inf_any', any(is_special(inp[a]) and not is_nan(inp[a]) for a in FLOATS))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
    add('zero_signed_input', any(inp[a] == 0x80000000 for a in FLOATS + ('old',)))
    if inp['real']:
        add('real_result_in_range', obs['events'] == [])
        return
    for k in range(3):
        add('ret%d_special' % k, is_special(inp['ret%d' % k]))
    add('ret_nan_noncanonical', any(noncanonical_nan(inp['ret%d' % k]) for k in range(3)))
    add('ret_denormal', any(is_denormal(inp['ret%d' % k]) for k in range(3)))
    add('arg_event_nan', any(is_nan(e[i]) for e in obs['events'] for i in (2, 3)))
    add('arg_event_denormal', any(is_denormal(e[i]) for e in obs['events'] for i in (2, 3)))
    add('arg_event_inf', any(is_special(e[i]) and not is_nan(e[i]) for e in obs['events'] for i in (2, 3)))
    px, py = (inp['mpos_x'], inp['mpos_y']) if inp['has_mat'] else (inp['ppos_x'], inp['ppos_y'])
    add('dual_nan_x', is_nan(inp['arg5']) and is_nan(px))
    add('dual_nan_y', is_nan(inp['arg6']) and is_nan(py))
    add('dual_nan_any', (is_nan(inp['arg5']) and is_nan(px)) or (is_nan(inp['arg6']) and is_nan(py)))
    add('d1_wrap', oracle.gt(oracle.sub(oracle.load(inp['ret1']), oracle.load(inp['old'])), oracle.PI) or oracle.lt(oracle.sub(oracle.load(inp['ret1']), oracle.load(inp['old'])), oracle.NEG_PI))
    add('d2_wrap', oracle.gt(oracle.sub(oracle.load(inp['ret2']), oracle.load(inp['old'])), oracle.PI) or oracle.lt(oracle.sub(oracle.load(inp['ret2']), oracle.load(inp['old'])), oracle.NEG_PI))
    oracle.decide(inp)
    last = oracle.LAST
    add('branch_neg', last['branch'] == 'neg')
    add('branch_pos', last['branch'] == 'pos')
    add('branch_none', last['branch'] == 'none')
    add('final_add_2pi', last['final'] == 'add')
    add('final_sub_2pi', last['final'] == 'sub')
    add('result_written', obs['result'] != inp['old'])
    add('result_unchanged', obs['result'] == inp['old'])
    add('old_nan_unchanged', is_nan(inp['old']) and obs['result'] == inp['old'])
    d1, d2 = last['d1'], last['d2']
    add('d1_zero', d1 != oracle.NAN and d1[0] == 'f' and d1[1] == 0)
    add('d2_zero', d2 != oracle.NAN and d2[0] == 'f' and d2[1] == 0)
    add('d_nan', d1 == oracle.NAN or d2 == oracle.NAN)
    add('d_tie', d1 != oracle.NAN and d2 != oracle.NAN and d1[0] == 'f' and d2[0] == 'f' and d1[1] == d2[1] and d1[1] != 0)
    add('d_tiny', any(d != oracle.NAN and d[0] == 'f' and d[1] != 0 and abs(d[1]) < oracle.Fraction(1, 1 << 100) for d in (d1, d2)))
    for nm in ('q', 'p', 'a', 'b', 'x1', 'y1', 'x2', 'y2'):
        v = last[nm]
        add('inter_tiny_' + nm, oracle.is_tiny(v) if hasattr(oracle, 'is_tiny') else (v != oracle.NAN and v[0] == 'f' and v[1] != 0 and abs(v[1]) < oracle.Fraction(1, 1 << 126)))


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
        raise RuntimeError('Inicio sin hooks registrados, o sin el falso del colaborador')
    b = b[0]
    if b.get('compare_dead_x87_registers') is not False or b.get('compare_volatile_registers') is not False:
        raise RuntimeError('Configuracion de compuertas inesperada (ver README.md)')
    if b.get('paths_layout_ok') is not True:
        raise RuntimeError('ThePaths.m_pPathNodes / m_pNaviNodes no estan en 0x96F854 / 0x96F974')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # GetATanOfXY tiene hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
        if any(s['dll'] is not True for s in col[fn]['stubbed']):
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 18 or short:
            raise RuntimeError('Cobertura por familia insuficiente en %s: %s' % (fn, fam[fn]))
        for key, minimum in COVER.get(fn, {}).items():
            if cov[fn].get(key, 0) < minimum:
                raise RuntimeError('Cobertura de ramas insuficiente en %s: %s %d < %d' % (fn, key, cov[fn].get(key, 0), minimum))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'entries_intact')):
        raise RuntimeError('Final sin restauracion exacta')
    state = [r for r in rows if r['event'] == 'global_state_final']
    if len(state) != 1 or state[0]['floating_state'] != b['initial_fp'] or state[0]['watched_intact'] is not True or state[0]['state_restored'] is not True:
        raise RuntimeError('El x87, los hooks o las entradas vigiladas no se restauraron')
    summary['compare_dead_x87_registers'] = False
    summary['families'] = fam
    summary['branch_coverage'] = cov
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('carlink74', manifest['binaries']['asi'], analyze, 'CarLink74', timeout=3600))
