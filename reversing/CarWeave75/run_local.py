"""Compuertas del banco CarWeave75 (lote 75): CCarCtrl::FindAngleToWeaveThroughTraffic (EXE 0x4325C0, hook S), 18432 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, que CADA lado coincida con el oraculo independiente (fixture_oracle.py), la cobertura por familia de las fixtures y la cobertura de ramas
medida sobre las entradas y el modelo del oraculo (coverage.json = minimos).
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

NAMES = ['CCarCtrl::FindAngleToWeaveThroughTraffic']
CASES = 18432   # 18 familias x 1024
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x4325C0}
# (direccion del EXE, hooked): WeaveThrough{Cars,Peds,Objects}SectorList y CWorld::ClearScanCodes (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x42D680, False), (0x42D7E0, False), (0x42D950, False), (0x563470, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x4325C0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


FLOATS = ('mpos_x', 'mpos_y', 'ppos_x', 'ppos_y', 'vx', 'vy', 'scale', 'arg3', 'arg4')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo (oracle.LAST lo dejo la ultima comprobacion del mismo caso)"""
    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    last = oracle.LAST
    add('has_mat', inp['has_mat'] == 1)
    add('no_mat', inp['has_mat'] == 0)
    add('style_6', (inp['style'] & 0xFF) == 6)
    add('style_other', (inp['style'] & 0xFF) != 6)
    add('phys_null', inp['phys'] == 0)
    add('phys_arena', inp['phys'] == 1)
    add('phys_raw', inp['phys'] == 2)
    add('scan_clear', (inp['scan'] & 0xFFFF) == 0xFFFF)
    add('scan_normal', (inp['scan'] & 0xFFFF) != 0xFFFF)
    add('scan_zero', (inp['scan'] & 0xFFFF) == 0)
    for nm in FLOATS:
        add('special_' + nm, is_special(inp[nm]))
        add('denormal_' + nm, is_denormal(inp[nm]))
        add('nan_noncanonical_' + nm, noncanonical_nan(inp[nm]))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('zero_signed_input', any(inp[a] == 0x80000000 for a in FLOATS))
    add('arg3_init', inp['arg3'] == 0xC61C3F9A)
    calls, sweeps = last['calls'], last['sweeps']
    add('calls_0', calls == 0)
    add('calls_1_to_8', 1 <= calls <= 8)
    add('calls_9_to_200', 9 <= calls <= 200)
    add('calls_over_200', calls > 200)
    add('calls_over_10000', calls > 10000)
    add('sweeps_0', sweeps == 0)
    add('sweeps_1', sweeps == 1)
    add('sweeps_2', sweeps == 2)
    add('sweeps_3_or_more', sweeps >= 3)
    add('clear_called', last['clears'] == 1)
    for k in ('xmin', 'ymin', 'xmax', 'ymax'):
        add('bound_nan', last[k] is None)
        add('bound_inf', last[k] is not None and (last[k] & 0x7FFFFFFF) == 0x7F800000)
    add('empty_x', last['minX'] > last['maxX'])
    add('empty_y', last['minY'] > last['maxY'])
    add('min_clamped_0', last['minX'] == 0 or last['minY'] == 0)
    add('max_clamped_119', last['maxX'] == 0x77 or last['maxY'] == 0x77)
    add('min_huge', last['minX'] > 119 or last['minY'] > 119)
    add('max_negative', last['maxX'] < 0 or last['maxY'] < 0)
    add('written_slots', calls > 0 and inp['n_writes'] > 0 and any(w[0] < calls for w in inp['writes']))
    add('write_special_value', any(is_special(v) for w in inp['writes'] for v in w[2:]))
    add('write_partial', any(w[1] in (1, 2) for w in inp['writes']))
    add('branch_A', last.get('branch') == 'A')
    add('branch_near', last.get('branch') == 'near')
    add('branch_s50', last.get('branch') == 's50')
    add('branch_s40', last.get('branch') == 's40')
    for nm in ('x', 'v', 'a', 'b'):
        val = last.get(nm)
        add('big_' + nm, val is not None and val != oracle.NAN and val[0] == 'f' and abs(val[1]) > 3.5)
    add('arg_diff_wrap', last.get('x') is not None and last['x'] != oracle.NAN and last['x'][0] == 'f' and abs(last['x'][1]) > 3.2)
    add('result_is_A', last.get('branch') == 'A' and obs['ret_st0'] is not None)


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
    if b.get('world_layout_ok') is not True:
        raise RuntimeError('La disposicion de CRepeatSector / CWorld / CVehicle no es la esperada')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los cuatro tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
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
        for side in ('original', 'candidate'):
            why = oracle.check_side(row['input'], row[side]['observed'])
            if why:
                raise RuntimeError('El lado %s de %s %d difiere del oraculo: %s' % (side, fn, case, why))
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
        sys.exit(runtime_runner.run_case('carweave75', manifest['binaries']['asi'], analyze, 'CarWeave75', timeout=3600))
