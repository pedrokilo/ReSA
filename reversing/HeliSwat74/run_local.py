"""Compuertas del banco HeliSwat74 (lote 74): CHeli::SendDownSwat (EXE 0x6C69C0, hook S), 21504 pares.

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

NAMES = ['CHeli::SendDownSwat']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C69C0}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE o no hay hook
STUBBED = {NAMES[0]: [(0x821B1E, False), (0x6C4760, True), (0x5696C0, True), (0x6E8580, True), (0x556B40, True), (0x612710, True), (0x61A5A0, True), (0x632BD0, True), (0x632D10, True), (0x659470, True), (0x460C80, True), (0x407180, False), (0x681AF0, False), (0x4D4610, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C69C0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('tgt_x', 'tgt_y', 'tgt_z', 'hel_x', 'hel_y', 'hel_z', 'spd_x', 'spd_y', 'spd_z', 'rel_x', 'rel_y', 'rel_z', 'm_rx', 'm_ry', 'm_rz', 'm_fx', 'm_fy', 'm_fz', 'm_ux', 'm_uy', 'm_uz', 'ground', 'water_lvl')
MAT = ('m_rx', 'm_ry', 'm_rz', 'm_fx', 'm_fy', 'm_fz', 'm_ux', 'm_uy', 'm_uz')


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada, el oraculo y la traza de eventos"""
    k = [e[0] for e in obs['events']]
    passed, _, _, V = oracle.expected(inp)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    num = inp['num_swat'] & 0xFF
    add('gate_numswat_zero', num == 0)
    add('gate_submerged', num != 0 and bool(inp['phys'] & 0x100))
    add('gate_not_loaded', num != 0 and not inp['phys'] & 0x100 and (inp['stream'] & 0xFF) != 1)
    add('gate_loaded_other_values', (inp['stream'] & 0xFF) not in (0, 1))
    add('gate_rand_rejects', 1 in k and not inp['rnd'] & 0x7F == 0 and 2 not in k)
    add('gate_rand_accepts', 2 in k)
    add('rand_high_garbage', 1 in k and (inp['rnd'] >> 8) != 0 and inp['rnd'] & 0x7F == 0)
    add('rand_bit7_set', 2 in k and bool(inp['rnd'] & 0x80))
    if 1 in k and not inp['rnd'] & 0x7F:
        hel = [oracle.load(inp[a]) for a in ('hel_x', 'hel_y', 'hel_z')]
        tgt = [oracle.load(inp[a]) for a in ('tgt_x', 'tgt_y', 'tgt_z')]
        spd = [oracle.load(inp[a]) for a in ('spd_x', 'spd_y', 'spd_z')]
        d = [oracle.sub(hel[i], tgt[i]) for i in range(3)]
        dist = oracle.sqrt(oracle.add(oracle.add(oracle.mul(d[0], d[0]), oracle.mul(d[1], d[1])), oracle.mul(d[2], d[2])))
        speed = oracle.sqrt(oracle.add(oracle.add(oracle.mul(spd[0], spd[0]), oracle.mul(spd[1], spd[1])), oracle.mul(spd[2], spd[2])))
        add('gate_dist_rejects', 2 not in k and oracle.gt(dist, oracle.load(oracle.B50)))
        add('gate_dist_nan_passes', dist == oracle.NAN)
        add('dist_exactly_50', oracle.num(dist) == 50)
        add('dist_inside_by_ulp', oracle.num(dist) is not None and oracle.num(dist) != float('inf') and 49.9999 < oracle.num(dist) < 50)
        add('dist_outside_by_ulp', oracle.num(dist) is not None and oracle.num(dist) != float('inf') and 50 < oracle.num(dist) < 50.0001)
        add('gate_speed_rejects', 2 not in k and not oracle.gt(dist, oracle.load(oracle.B50)) and oracle.gt(speed, oracle.load(oracle.B01)))
        add('gate_speed_nan_passes', speed == oracle.NAN)
        add('speed_exactly_0_1', oracle.num(speed) == oracle.load(oracle.B01)[1])
        add('speed_inside_by_ulp', oracle.num(speed) is not None and oracle.num(speed) != float('inf') and 0.0999999 < oracle.num(speed) <= oracle.load(oracle.B01)[1])
        add('speed_outside_by_ulp', oracle.num(speed) is not None and oracle.num(speed) != float('inf') and oracle.load(oracle.B01)[1] < oracle.num(speed) < 0.1000001)
    add('rotation_reached', 3 in k)
    if 3 in k:
        ground = oracle.load(inp['ground'])
        tz = oracle.load(inp['tgt_z'])
        diff = oracle.sub(tz, ground)
        add('gate_ground_rejects', 4 not in k)
        add('gate_ground_diff_exactly_2_5', oracle.num(diff) is not None and abs(oracle.num(diff)) == oracle.load(oracle.B25)[1])
        add('gate_ground_diff_negative_passes', 4 in k and oracle.lt(diff, oracle.load(0)))
        add('gate_ground_nan_rejects', 4 not in k and (ground == oracle.NAN or tz == oracle.NAN))
    add('water_reached', 4 in k)
    add('water_none', 4 in k and not inp['water'])
    add('water_found_rejects', 4 in k and inp['water'] and 5 not in k)
    add('water_found_passes', 5 in k and bool(inp['water']))
    add('water_equal_rejects', 4 in k and inp['water'] and inp['water_lvl'] == inp['ground'] and 5 not in k)
    add('water_nan_rejects', 4 in k and inp['water'] and oracle.is_nan(inp['water_lvl']) and 5 not in k)
    add('passes', bool(obs['ret_al'] & 1))
    add('fails', not obs['ret_al'] & 1)
    add('passes_tgt_matrix', bool(obs['ret_al']) and inp['tgt_mat'] == 1)
    add('passes_tgt_compact', bool(obs['ret_al']) and inp['tgt_mat'] == 0)
    for v in (1, 2, 3, 4):
        add('passes_numswat_%d' % v, bool(obs['ret_al']) and num == v)
    add('passes_numswat_gt4', bool(obs['ret_al']) and num > 4)
    add('passes_numswat_ge_0x80', bool(obs['ret_al']) and num >= 0x80)
    add('passes_numswat_0xFF', bool(obs['ret_al']) and num == 0xFF)
    add('alloc_seq_null', bool(obs['ret_al']) and inp['null_seq'] == 1)
    add('alloc_rope_null', bool(obs['ret_al']) and inp['null_rope'] == 1)
    add('alloc_wander_null', bool(obs['ret_al']) and inp['null_wander'] == 1)
    add('alloc_all_null', bool(obs['ret_al']) and inp['null_seq'] == 1 and inp['null_rope'] == 1 and inp['null_wander'] == 1)
    add('alloc_none_null', bool(obs['ret_al']) and not (inp['null_seq'] or inp['null_rope'] or inp['null_wander']))
    add('ped_collision_cleared', bool(obs['ret_al']) and bool(inp['ped_flags'] & 1))
    add('range_0', bool(obs['ret_al']) and inp['range'] == 0)
    add('range_7', bool(obs['ret_al']) and inp['range'] == 7)
    add('range_8', bool(obs['ret_al']) and inp['range'] == 8)
    add('rope_ret_1', bool(obs['ret_al']) and inp['rope_ret'] == 1)
    add('rope_ret_0_continues', bool(obs['ret_al']) and inp['rope_ret'] == 0)
    if 3 in k and V is not None:
        add('v_nan', any(p is None for p in V))
        add('v_all_nan', all(p is None for p in V))
        add('v_inf', any(p is not None and is_special(p) and not p & 0x7FFFFF for p in V))
        add('v_denormal', any(p is not None and is_denormal(p) for p in V))
        add('v_negzero', any(p == 0x80000000 for p in V))
        add('v_poszero', any(p == 0 for p in V))
        inter = oracle.LAST
        add('inter_tiny_unstored', any(oracle.is_tiny(inter[a]) for a in ('x', 'y', 'z')))
        add('inter_tiny_stored', any(oracle.is_tiny(p) for p in inter['v']))
        add('inter_signed_zero_neg', any(oracle.is_signed_zero(p, 1) for p in inter['v']))
        add('inter_signed_zero_pos', any(oracle.is_signed_zero(p, 0) for p in inter['v']))
    add('special_mat', 3 in k and any(is_special(inp[a]) for a in MAT))
    add('denormal_mat', 3 in k and any(is_denormal(inp[a]) for a in MAT))
    add('nan_noncanonical_mat', 3 in k and any(noncanonical_nan(inp[a]) for a in MAT))
    add('special_rel', 3 in k and any(is_special(inp[a]) for a in ('rel_x', 'rel_y', 'rel_z')))
    add('denormal_rel', 3 in k and any(is_denormal(inp[a]) for a in ('rel_x', 'rel_y', 'rel_z')))
    add('nan_noncanonical_rel', 3 in k and any(noncanonical_nan(inp[a]) for a in ('rel_x', 'rel_y', 'rel_z')))
    add('special_hel', 3 in k and any(is_special(inp[a]) for a in ('hel_x', 'hel_y', 'hel_z')))
    add('denormal_hel', 3 in k and any(is_denormal(inp[a]) for a in ('hel_x', 'hel_y', 'hel_z')))
    add('nan_noncanonical_hel', 3 in k and any(noncanonical_nan(inp[a]) for a in ('hel_x', 'hel_y', 'hel_z')))
    add('negzero_input', 3 in k and any(inp[a] == 0x80000000 for a in MAT + ('rel_x', 'rel_y', 'rel_z', 'hel_x', 'hel_y', 'hel_z')))
    add('nan_noncanonical_any', any(noncanonical_nan(inp[a]) for a in FLOATS))
    add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))


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
        sys.exit(runtime_runner.run_case('heliswat74', manifest['binaries']['asi'], analyze, 'HeliSwat74', timeout=3600))
