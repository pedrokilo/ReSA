"""Compuertas del banco Fn103A (lote 103A): CMatrix::SetRotateXOnly (EXE 0x59AFA0), SetRotateYOnly (0x59AFE0), Rotate (0x59B460), ForceUpVector (0x59B7E0), SetRotate(quat) (0x59BBF0),
operator+(CMatrix, CMatrix) (0x59BFA0) y CVector::FromMultiply3x3 (0x59C6D0; hook S), 16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados de la ventana, EAX de salida, x87 vacia), la cobertura por familia de las
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

NAMES = ['CMatrix::SetRotateXOnly', 'CMatrix::SetRotateYOnly', 'CMatrix::Rotate', 'CMatrix::ForceUpVector', 'CMatrix::SetRotate(quat)', 'operator+(CMatrix)', 'CVector::FromMultiply3x3']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x59AFA0, 0x59AFE0, 0x59B460, 0x59B7E0, 0x59BBF0, 0x59BFA0, 0x59C6D0)))
# colaboradores sustituidos por falsos: (direccion del EXE, tiene hook RH)
STUBBED = {n: [] for n in NAMES}
WATCHED = set(HOOKS.values()) | {exe for lst in STUBBED.values() for exe, _ in lst}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COMMON = ['nan_in', 'snan_in', 'multi_nan', 'inf_in', 'zero_in', 'neg_zero_in', 'denormal_in']
MIXED = ['snan_mem_reg_nan', 'snan_mem_alone', 'two_qnan']
OUT = ['out_nan', 'out_inf', 'out_zero', 'out_denormal', 'out_finite']
RES = ['res_finite', 'res_nan', 'res_inf', 'res_zero', 'res_over_float', 'res_under_float']
OVER = ['mode3', 'mode4', 'mode5', 'mode6', 'mode7']
ANG = ['ang_nan', 'ang_inf', 'ang_zero', 'ang_denormal', 'ang_huge', 'ang_large', 'ang_mid', 'ang_small', 'ang_neg', 'ang_snan']
KEYS = {
    NAMES[0]: ANG + ['out_nan', 'out_zero', 'out_denormal', 'out_finite', 'mode0'],
    NAMES[1]: ANG + ['out_nan', 'out_zero', 'out_denormal', 'out_finite', 'mode0'],
    NAMES[2]: ANG + COMMON + MIXED + OUT + ['out_overflow_inf', 'out_underflow_zero'],
    NAMES[3]: COMMON + MIXED + OUT,
    NAMES[4]: COMMON + MIXED + OUT + OVER,
    NAMES[5]: COMMON + MIXED + OUT + ['out_overflow_inf'] + OVER,
    NAMES[6]: COMMON + MIXED + OUT + ['out_overflow_inf', 'out_underflow_zero'] + OVER,
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
        raise RuntimeError('Inicio sin hooks registrados')
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
        sys.exit(runtime_runner.run_case('fn103a', manifest['binaries']['asi'], analyze, 'Fn103A', timeout=3600))
