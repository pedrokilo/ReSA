"""Compuertas del banco PresetLoad74 (lote 74): RsLoadPresetViews (EXE 0x619D60, hook S), 20480 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['RsLoadPresetViews']
CASES = 20480   # 20 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x619D60}
# (direccion del EXE, hooked, cuerpo en la DLL): RsDestroyPresetViews 0x6198C0 (reconstruida sin hook), psPathnameCreate, psPathnameDestroy, isprint del CRT 0x8228B5 (sin cuerpo en la DLL: la DLL llama a la direccion del EXE),
# CMemoryMgr::Malloc, CMemoryMgr::Free 0x72F430 (reconstruida sin hook), RsErrorMessage. hooked=False: el hook queda apagado, la entrada del EXE es codigo original.
STUBBED = {NAMES[0]: [(0x6198C0, False, True), (0x745470, False, True), (0x7454E0, False, True), (0x8228B5, False, False), (0x72F420, False, True), (0x72F430, False, True), (0x619B00, False, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _, _ in lst} | {0x619D60}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    ev, num, head, heap, free, isp, al, closed = oracle.expected(inp)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('fopen_fail', not inp['fopen_ok'])
    add('path_null', inp['path_null'] == 1)
    add('path_null_fopen_ok', inp['path_null'] == 1 and inp['fopen_ok'] == 1)
    add('fopen_ok', inp['fopen_ok'] == 1)
    add('no_lines', inp['fopen_ok'] == 1 and inp['n_lines'] == 0)
    add('closed', closed)
    add('oom_node', 13 in k and k.count(9) == k.index(13) and 11 not in k and not closed and k.count(9) >= 1 and events[k.index(13) - 1][0] == 9)
    add('oom_desc_free', 11 in k)
    add('nodes_1', num == 1)
    add('nodes_2plus', num >= 2)
    add('nodes_5', num == 5)
    add('ret_ok_true', al == 1)
    add('ret_false_with_nodes_oom', al == 0 and num > 0)
    add('ret_false_empty', al == 0 and num == 0)
    add('sprintf_default_name', 8 in k)
    add('sscanf_ret_lt7', any(inp['ret%d' % i] in tuple(range(0, 7)) or inp['ret%d' % i] == 0xFFFFFFFF for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('sscanf_eof_minus1', any(inp['ret%d' % i] == 0xFFFFFFFF for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('empty_line', any(len(inp['line%d' % i]) == 0 for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('isprint_calls', isp[0] > 0)
    add('line_filtered', any(bytes.fromhex(inp['line%d' % i]) != bytes(ch for ch in bytes.fromhex(inp['line%d' % i]) if 0x20 <= ch <= 0x7E) for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('line_high_bytes', any(any(ch >= 0x80 for ch in bytes.fromhex(inp['line%d' % i])) for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('line_long', any(len(inp['line%d' % i]) >= 400 for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    add('name_40', any(inp['ret%d' % i] == 8 and len(inp['name%d' % i]) == 80 for i in range(inp['n_lines'])) and inp['fopen_ok'] == 1)
    fl = [int.from_bytes(bytes.fromhex(inp['floats%d' % i])[4 * j:4 * j + 4], 'little') for i in range(inp['n_lines']) for j in range(7)]
    add('float_nan_noncanonical', num > 0 and any(noncanonical_nan(v) for v in fl))
    add('float_snan', num > 0 and any(is_special(v) and (v & 0x7FFFFF) and not (v & 0x400000) for v in fl))
    add('float_inf', num > 0 and any(is_special(v) and not (v & 0x7FFFFF) for v in fl))
    add('float_denormal', num > 0 and any(is_denormal(v) for v in fl))
    add('float_negzero', num > 0 and any(v == 0x80000000 for v in fl))
    add('num_before_nonzero', inp['num_before'] != 0)
    add('num_before_zero', inp['num_before'] == 0)
    add('list_before_nonnull', inp['list_before'] != 0)
    add('cur_before_other', inp['cur_before'] != 0xFFFFFFFF)


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
        got = [(s['exe'], s['hooked'], s['dll']) for s in col[fn]['stubbed']]
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
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 20 or short:
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
        sys.exit(runtime_runner.run_case('presetload74', manifest['binaries']['asi'], analyze, 'PresetLoad74', timeout=3600))
