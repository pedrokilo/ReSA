"""Compuertas del banco FxWood73 (lote 73): Fx_c::AddWood (EXE 0x49EE10, hook S), 16384 pares.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hook S registrado y cuerpo en el ASI, todas las compuertas de ABI/ruta (entrada)/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio, la cobertura por familia de las fixtures y la cobertura de ramas medida sobre las entradas y los eventos (coverage.json = minimos).
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

NAMES = ['Fx_c::AddWood']
CASES = 16384 + 28   # 16 familias de 1024 + familia 16 (28 casos de 65535..1000000 vueltas, con medicion de tiempo)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x49EE10}
# (direccion del EXE, hooked): FxSystem_c::AddParticle(v3d) 0x4AA440 (hook RH apagado: la entrada del EXE es codigo original y la DLL tiene cuerpo propio) y rand() del CRT del EXE 0x821B1E (sin hook; la DLL lo llama por la direccion)
STUBBED = {NAMES[0]: [(0x4AA440, False)]}   # rand 0x821B1E y getptd 0x827B3D NO se sustituyen: son bytes del EXE en ambos lados (solo se vigilan)
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x49EE10, 0x821B1E, 0x827B3D}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_den(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_snan(b):
    return is_nan(b) and (b & 0x400000) == 0


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y lo observado"""
    calls = obs['particle_calls']
    amount = inp['amount'] if inp['amount'] < 0x80000000 else inp['amount'] - (1 << 32)
    ev = obs['events']

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    add('amount_le0', amount <= 0)
    add('amount_le0_state_untouched', amount <= 0 and obs['rand_state'] == inp['rand_seed'])
    add('amount_negative', amount < 0)
    add('amount_min_int', inp['amount'] == 0x80000000)
    add('out_of_range', amount > 0 and calls == 0)
    add('in_range', calls > 0)
    add('one_iteration', calls == 1)
    add('two_to_four', 2 <= calls <= 4)
    add('five_to_twelve', 5 <= calls <= 12)
    add('thirteen_plus', calls >= 13)
    add('max_iterations', calls >= 30)
    add('cam_matrix', inp['has_mat'] == 1)
    add('cam_compact', inp['has_mat'] == 0)
    add('cam_matrix_in_range', inp['has_mat'] == 1 and calls > 0)
    add('cam_compact_in_range', inp['has_mat'] == 0 and calls > 0)
    add('cam_matrix_out', inp['has_mat'] == 1 and amount > 0 and calls == 0)
    add('cam_compact_out', inp['has_mat'] == 0 and amount > 0 and calls == 0)
    add('rand_mode_0_any_state', calls > 0 and inp['rand_mode'] == 0)
    add('rand_mode_1_edge_draw', calls > 0 and inp['rand_mode'] == 1)
    add('rand_mode_2_zero_jitter_pair', calls > 0 and inp['rand_mode'] == 2)
    add('rand_mode_3_zero_jitter_single', calls > 0 and inp['rand_mode'] == 3)
    add('rand_state_advanced', calls > 0 and obs['rand_state'] != inp['rand_seed'])
    add('rand_special_seed', calls > 0 and inp['rand_seed'] in (0, 1, 0x80000000, 0xFFFFFFFF, 0x7FFFFFFF, 0x269EC3))
    add('amount_31_to_256', 31 <= calls <= 256)
    add('amount_257_to_1000', 257 <= calls <= 1000)
    add('amount_1001_to_4096', 1001 <= calls <= 4096)
    add('amount_4097_to_10000', 4097 <= calls <= 10000)
    add('amount_64', calls == 64 and amount == 64)
    add('amount_100', calls == 100 and amount == 100)
    add('amount_255', calls == 255 and amount == 255)
    add('amount_256', calls == 256 and amount == 256)
    add('amount_1000', calls == 1000 and amount == 1000)
    add('amount_4096', calls == 4096 and amount == 4096)
    add('amount_10000', calls == 10000 and amount == 10000)
    add('amount_65535', amount == 65535 and calls == 65535)
    add('amount_65536', amount == 65536 and calls == 65536)
    add('amount_100000', amount == 100000 and calls == 100000)
    add('amount_1000000', amount == 1000000 and calls == 1000000)
    add('events_trace_truncated_hash_only', calls > 256)
    add('swap_slot', inp['swap_slot'] == 1 and calls >= 2)
    add('swap_slot_systems_alternate', inp['swap_slot'] == 1 and calls >= 2 and len({e[1] for e in ev}) == 2)
    add('slot_single_system', inp['swap_slot'] == 0 and calls >= 1 and len({e[1] for e in ev}) == 1)
    for k in range(4):
        add('this_kind_%d' % k, inp['this_kind'] == k)
    pos = inp['pos']; cam = inp['cam']
    add('camera_or_pos_nan', any(is_nan(b) for b in pos + cam))
    add('camera_or_pos_nan_in_range', any(is_nan(b) for b in pos + cam) and calls > 0)
    add('camera_or_pos_inf', any(is_inf(b) for b in pos + cam))
    add('camera_or_pos_inf_out', any(is_inf(b) for b in pos + cam) and not any(is_nan(b) for b in pos + cam) and amount > 0 and calls == 0)
    add('camera_or_pos_tiny', any(is_den(b) or (b & 0x7FFFFFFF) == 0 for b in pos + cam) and calls > 0)
    add('family_exact_625', inp['family'] == 3)
    add('family_around_625', inp['family'] in (2, 3))
    dire = inp['dir']
    add('dir_nan', calls > 0 and any(is_nan(b) for b in dire))
    add('dir_snan', calls > 0 and any(is_snan(b) for b in dire))
    add('dir_inf', calls > 0 and any(is_inf(b) for b in dire))
    add('dir_denormal', calls > 0 and any(is_den(b) for b in dire))
    add('dir_zero', calls > 0 and any((b & 0x7FFFFFFF) == 0 for b in dire))
    add('dir_negative_zero', calls > 0 and any(b == 0x80000000 for b in dire))
    add('dir_huge', calls > 0 and any((b & 0x7F800000) >= 0x7E800000 and not is_inf(b) and not is_nan(b) for b in dire))
    add('vel_denormal_result', calls > 0 and any(is_den(e[6 + i]) for e in ev for i in range(3)))
    add('vel_nan_result', calls > 0 and any(is_nan(e[6 + i]) for e in ev for i in range(3)))
    add('vel_inf_result', calls > 0 and any(is_inf(e[6 + i]) for e in ev for i in range(3)))
    li = inp['light']
    add('light_nan', calls > 0 and is_nan(li))
    add('light_snan', calls > 0 and is_snan(li))
    add('light_inf', calls > 0 and is_inf(li))
    add('light_denormal', calls > 0 and is_den(li))
    add('light_zero', calls > 0 and (li & 0x7FFFFFFF) == 0)
    add('light_negative_zero', calls > 0 and li == 0x80000000)
    add('light_normal', calls > 0 and not (is_nan(li) or is_inf(li) or is_den(li)))
    add('colors_varied', calls >= 2 and len({e[10] for e in ev}) > 1)


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
        raise RuntimeError('rand/getptd del EXE no son los bytes originales (o saltan a la DLL)')
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
        # AddParticle(v3d) tiene hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso; rand/getptd del EXE ya no se sustituyen
        if [s['dll'] for s in col[fn]['stubbed']] != [True]:
            raise RuntimeError('Cuerpo de la DLL de un colaborador sin resolver (o inesperado) en ' + fn)
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
        tally(row['input'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200 and k != 16}
        if len(fam[fn]) != 17 or short or fam[fn].get(16) != 28:
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
        sys.exit(runtime_runner.run_case('fxwood73', manifest['binaries']['asi'], analyze, 'FxWood73', timeout=3600))
