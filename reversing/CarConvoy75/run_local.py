"""Compuertas del banco CarConvoy75 (lote 75): CCarCtrl::CreateConvoy (EXE 0x42C740, hook S), 21504 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la igualdad de ambos lados con el oraculo independiente (fixture_oracle.py: modelo Python de la funcion, con aritmetica x87 a 24 bits y memoria de las arenas),
la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
`python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CCarCtrl::CreateConvoy']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x42C740}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE o no hay hook
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x421440, True), (0x5674E0, True), (0x568B80, True), (0x4217C0, True), (0x59BBC0, False), (0x563220, True), (0x6BEEB0, True), (0x6AF420, True),
                      (0x61A5A0, True), (0x63CB10, True), (0x681AF0, False), (0x571B70, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x42C740}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    oracle.expected(inp)
    L = oracle.LAST['L']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    created = L['created']
    add('created_%d' % created)
    add('created_any', created > 0)
    add('three_iterations', L['count'] == 3)
    add('two_iterations', L['count'] == 2)
    r = inp['rnd'] & 0xFFFF
    add('roll_16711', r == 16711)
    add('roll_16712', r == 16712)
    add('roll_garbage_high', (inp['rnd'] >> 16) != 0)
    add('null_car', L.get('null_car'))
    add('null_mask_nonzero', inp['null_mask'] != 0)
    add('no_ground_abort', L.get('no_ground'))
    add('colliding_abort', L.get('colliding'))
    add('second_nearer', L.get('second_nearer', 0) > 0)
    add('aborts_any', L['aborts'] > 0)
    add('hit_garbage_high', any((inp['hit%d' % i] >> 8) != 0 for i in range(6)))
    add('hit_special_z', any(is_special(inp['hitz%d' % i]) for i in range(6)))
    add('hit_nan_z', any(is_nan(inp['hitz%d' % i]) for i in range(6)))
    add('hit_1e9', any(inp['hitz%d' % i] in (0x4E6E6B28, 0x4E6E6B29, 0x4E6E6B27) for i in range(6)))
    add('height_special', any(is_special(inp['height%d' % i]) for i in range(3)))
    add('height_nan_noncanonical', any(noncanonical_nan(inp['height%d' % i]) for i in range(3)))
    add('fwd_special', any(is_special(inp['fwd_' + k]) for k in 'xyz'))
    add('fwd_denormal', any(is_denormal(inp['fwd_' + k]) for k in 'xyz'))
    add('fwd_nan_noncanonical', any(noncanonical_nan(inp['fwd_' + k]) for k in 'xyz'))
    add('pos_special', any(is_special(inp['pos_' + k]) for k in 'xyz'))
    add('pos_nan_noncanonical', any(noncanonical_nan(inp['pos_' + k]) for k in 'xyz'))
    add('two_nan_point', any(is_nan(inp['fwd_' + k]) for k in 'xyz') and any(is_nan(inp['pos_' + k]) for k in 'xyz'))
    add('radius_special', is_special(inp['radius']))
    add('radius_nan', is_nan(inp['radius']))
    add('speed_special', is_special(inp['speed']))
    add('speed_nan', is_nan(inp['speed']))
    add('speed_negative_result', created > 0 and oracle.lt(oracle.sub(oracle.load(inp['speed']), oracle.load_int(0)), oracle.ZERO))
    add('cruise_zero', inp['cruise'] == 0)
    add('cruise_255', inp['cruise'] == 255)
    add('bike', any(oracle.sgn32(inp['ctype%d' % i]) == 9 for i in range(3)) and created > 0)
    add('automobile', any(oracle.sgn32(inp['ctype%d' % i]) == 0 for i in range(3)) and created > 0)
    add('other_type', any(oracle.sgn32(inp['ctype%d' % i]) not in (0, 9) for i in range(3)) and created > 0)
    add('car_without_matrix', any(inp['cmat%d' % i] == 0 for i in range(3)) and created > 0)
    add('car_with_matrix', any(inp['cmat%d' % i] == 1 for i in range(3)) and created > 0)
    add('task_null', any(inp['ctask_null%d' % i] for i in range(3)) and created > 0)
    add('ped_type_extreme', inp['ped_type'] in (0x80000000, 0x7FFFFFFF, 0xFFFFFFFF))


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
    if b.get('layout_ok') is not True:
        raise RuntimeError('La disposicion de los campos de bits / accesores no es la esperada')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn + ': ' + str(got))
        # los colaboradores con hook RH tienen su cuerpo de la DLL parcheado con el mismo falso
        if any(s['hooked'] and s['dll'] is not True for s in col[fn]['stubbed']):
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
        why, _ = oracle.check_row(row)
        if why:
            raise RuntimeError('El oraculo independiente discrepa en %s %d: %s' % (fn, case, why[:3]))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(row['input'], row['candidate']['observed'], cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != FAMILIES or short:
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
        sys.exit(runtime_runner.run_case('carconvoy75', manifest['binaries']['asi'], analyze, 'CarConvoy75', timeout=3600))
