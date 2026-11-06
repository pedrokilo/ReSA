"""Compuertas del banco Fn105A (lote 105A): TList_c::AddItem (EXE 0x4A8DF0), RemoveAll (0x4A8EB0), GetNumItems (0x4A8EC0), CMatrix::operator*= (0x411A80), CVector / float (0x4119D0),
CompressedVector a CVector (0x411590), CColPoint::operator= (0x40FC80), iterador de constructores de vectores (0x403D60), CVector::operator-= (0x406D70) y CStreaming::SetModelTxdIsDeletable (0x409C70; hook S),
16384 pares cada una.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py: bytes cambiados de la ventana, eventos de los colaboradores, EAX de salida donde algun llamador lo lee,
x87 vacia), la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y el modelo (KEYS, minimo COVER_FLOOR).
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

NAMES = ['TList_c::AddItem', 'TList_c::RemoveAll', 'TList_c::GetNumItems', 'CMatrix::operator*=', 'CVector / float', 'CompressedVector to CVector', 'CColPoint::operator=', 'vector constructor iterator', 'CVector::operator-=', 'CStreaming::SetModelTxdIsDeletable']
CASES = 16384
FAMILIES = 16
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = dict(zip(NAMES, (0x4A8DF0, 0x4A8EB0, 0x4A8EC0, 0x411A80, 0x4119D0, 0x411590, 0x40FC80, 0x403D60, 0x406D70, 0x409C70)))
# colaboradores sustituidos por falsos: (direccion del EXE, tiene hook RH)
STUBBED = {n: [] for n in NAMES}
STUBBED['CMatrix::operator*='] = [(0x59BE30, True), (0x59BBC0, True), (0x59ACD0, True)]
STUBBED['CStreaming::SetModelTxdIsDeletable'] = [(0x409C10, True)]
WATCHED = set(HOOKS.values()) | {exe for lst in STUBBED.values() for exe, _ in lst}
COVER_FLOOR = 50
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
COMMON = ['nan_in', 'snan_in', 'multi_nan', 'inf_in', 'zero_in', 'neg_zero_in', 'denormal_in']
MIXED = ['snan_mem_reg_nan', 'snan_mem_alone', 'two_qnan']
OUT = ['out_nan', 'out_inf', 'out_zero', 'out_denormal', 'out_finite']
RES = ['res_finite', 'res_nan', 'res_inf', 'res_zero', 'res_over_float', 'res_under_float']
MODES = ['mode%d' % k for k in range(8)]
RECIP = ['recip_inf', 'recip_nan', 'rf_overflow', 'rf_denormal', 'z_uses_register']
KEYS = {
    NAMES[0]: ['empty_list', 'nonempty_list', 'count_wrap', 'double_insert', 'node_in_list'] + MODES,
    NAMES[1]: MODES,
    NAMES[2]: ['count_zero', 'count_big', 'count_small'] + MODES,
    NAMES[3]: ['same_matrix', 'overlap', 'separate'] + MODES,
    NAMES[4]: COMMON + MIXED + OUT + RECIP + MODES,
    NAMES[5]: ['neg_val', 'pos_val', 'zero_val', 'min16', 'max16', 'src_half_offset'] + MODES,
    NAMES[6]: ['overlap', 'separate', 'byte_shift'] + MODES,
    NAMES[7]: ['count_neg', 'count_zero', 'count_pos', 'ecx_in_arena', 'ecx_out', 'stride_neg', 'stride_pos', 'stride_zero'] + MODES,
    NAMES[8]: COMMON + MIXED + OUT + MODES,
    NAMES[9]: ['txd_neg', 'txd_pos', 'txd_zero', 'id_small', 'id_high'] + MODES,
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
        sys.exit(runtime_runner.run_case('fn105a', manifest['binaries']['asi'], analyze, 'Fn105A', timeout=3600))
