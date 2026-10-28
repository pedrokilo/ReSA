"""Compuertas del banco HeliPre75 (lote 75): CHeli::PreRender (EXE 0x6C5420, hook V), 21504 pares.

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

NAMES = ['CHeli::PreRender']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6C5420}
# (direccion del EXE, hooked) en el orden del banco: hooked=False => la DLL llama por la direccion del EXE, o la DLL llama a su cuerpo sin hook RH (CMatrix::Attach)
STUBBED = {NAMES[0]: [(0x6D6480, True), (0x52CEE0, True), (0x59C890, True), (0x6C45B0, True), (0x535300, True), (0x4C7D20, True), (0x6AA290, True), (0x6B0690, False),
                      (0x59BD10, True), (0x59B0E0, False), (0x59B060, False), (0x59BBB0, False), (0x70BDA0, True)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6C5420, 0x871680 + 4 * 17}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}
FAMILIES = 21


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_nan(b) and (b & 0x7FFFFF) != 0x400000


def tally(inp, obs, cov):
    """cobertura de ramas desde la entrada y el modelo del oraculo"""
    ev, _, _, wild = oracle.expected(inp)
    k = [e[0] for e in ev]
    model = inp['model'] & 0xFFFF
    heavy = model in (0x1D5, 0x1BF, 0x1E7, 0x1E8, 0x1F1)
    Lg = oracle

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    # ---- foco de busqueda
    add('light_flag_clear', (inp['a11'] & 0xFF) == 0)
    add('light_flag_set', (inp['a11'] & 0xFF) != 0)
    add('light_flag_garbage_high_bit', (inp['a11'] & 0x80) != 0 and (inp['a11'] & 0x7F) != 0)
    add('light_intensity_not_positive', (inp['a11'] & 0xFF) != 0 and not Lg.f_gt(inp['intens'], 0))
    add('light_intensity_nan', (inp['a11'] & 0xFF) != 0 and is_nan(inp['intens']))
    add('light_intensity_zero_neighbour', (inp['a11'] & 0xFF) != 0 and inp['intens'] in (0, 0x80000000, 1, 0x80000001))
    add('light_time_called', 2 in k)
    add('light_time_false', 2 in k and (inp['time_ret'] & 0xFF) == 0)
    add('light_time_false_high_bits', 2 in k and (inp['time_ret'] & 0xFF) == 0 and (inp['time_ret'] >> 8) != 0)
    add('light_time_true', 3 in k)
    add('light_time_true_high_bits', 3 in k and (inp['time_ret'] >> 8) != 0)
    add('light_added', 4 in k)
    add('light_matrix_absent', 3 in k and not inp['mat_present'])
    add('light_target_nan', 4 in k and any(is_nan(inp['tgt%d' % i]) for i in range(3)))
    add('light_target_snan_yz', 4 in k and any(is_nan(inp['tgt%d' % i]) and not (inp['tgt%d' % i] & 0x400000) for i in (1, 2)))
    add('light_target_snan_x', 4 in k and is_nan(inp['tgt0']) and not (inp['tgt0'] & 0x400000))
    add('light_origin_special', 4 in k and any(is_special(inp['origin%d' % i]) for i in range(3)))
    # ---- suspension
    add('susp_skipped', (inp['b42b'] & 1) == 0)
    add('susp_done', 6 in k)
    add('susp_b42b_garbage', 6 in k and (inp['b42b'] & 0xFE) != 0)
    direct = bool(inp['flags40'] & 0x4000000) and bool(inp['flags38c'] & 0x20000)
    add('susp_direct_flags', 6 in k and direct)
    add('susp_blend_flags', 6 in k and not direct)
    add('susp_only_phys_flag', 6 in k and bool(inp['flags40'] & 0x4000000) and not (inp['flags38c'] & 0x20000))
    add('susp_only_handling_flag', 6 in k and not (inp['flags40'] & 0x4000000) and bool(inp['flags38c'] & 0x20000))
    if 6 in k:
        for i in range(4):
            r = Lg.div(Lg.load(inp['spr%d' % i]), Lg.load(inp['line%d' % i]))
            q = Lg.sub(Lg.load(0x3F800000), r)
            nn = Lg.sub(Lg.load(inp['comp%d' % i]), q)
            dd = Lg.sub(Lg.load(0x3F800000), q)
            ratio_bits = Lg.store(Lg.div(nn, dd))
            ratio = Lg.reload(ratio_bits)
            add('wheel_ratio_positive', Lg.gt(ratio, Lg.load(0)))
            add('wheel_ratio_not_positive', not Lg.gt(ratio, Lg.load(0)) and ratio != Lg.NAN)
            add('wheel_ratio_nan', ratio == Lg.NAN)
            t = Lg.add(Lg.load(inp['wheel%d_2' % i]), Lg.load(inp['hand_b8']))
            if Lg.gt(ratio, Lg.load(0)):
                t = Lg.sub(t, Lg.mul(ratio, Lg.load(inp['spr%d' % i])))
            wp = Lg.load(inp['wp%d' % i])
            add('wheel_target_above', Lg.gt(t, wp))
            add('wheel_target_not_above_blend', not Lg.gt(t, wp) and not direct)
            add('wheel_target_not_above_direct', not Lg.gt(t, wp) and direct)
            add('wheel_target_nan', t == Lg.NAN)
        add('wheel_nan_result', any(0x838 <= o < 0x848 for o in wild[0]))
        add('wheel_handling_special', is_special(inp['hand_b8']))
        add('wheel_height_special', any(is_special(inp['wheel%d_2' % i]) for i in range(4)))
        add('wheel_inputs_special', any(is_special(inp['%s%d' % (n, i)]) for i in range(4) for n in ('comp', 'spr', 'line', 'wp')))
        add('wheel_line_zero', any((inp['line%d' % i] & 0x7FFFFFFF) == 0 for i in range(4)))
        add('wheel_denormal_input', any(is_denormal(inp['%s%d' % (n, i)]) for i in range(4) for n in ('comp', 'spr', 'line', 'wp')))
    # ---- ruedas, polvo, angulos
    add('wheel_matrices', 8 in k)
    add('dust_called', 9 in k)
    add('dust_skipped_rc', 9 not in k)
    add('model_rc_1d1', model == 0x1D1)
    add('model_rc_1f5', model == 0x1F5)
    add('model_heavy', heavy)
    add('model_1a1', model == 0x1A1)
    add('model_minus_one', model == 0xFFFF)
    add('model_zero', model == 0)
    add('angle_a0_nan', 0x9A0 in wild[0])
    add('angle_a1_nan', 0x9A4 in wild[0])
    add('angle_a0_low', Lg.lt(Lg.load(inp['a0']), Lg.load(0xC0C90FDB)))
    add('angle_a1_high', Lg.gt(Lg.load(inp['a1']), Lg.load(0x40C90FDB)))
    add('step_special', is_special(inp['step']))
    add('step_nan', is_nan(inp['step']))
    add('step_denormal', is_denormal(inp['step']))
    add('rotor_special', is_special(inp['rotor']))
    add('rotor_nan', is_nan(inp['rotor']))
    add('angle_input_nan', is_nan(inp['a0']) or is_nan(inp['a1']))
    # ---- paletas
    nodes = [inp['node%d' % i] for i in range(4)]
    add('blade_all_present', all(nodes))
    add('blade_none_present', not any(nodes))
    for i in range(4):
        add('blade_node%d_present' % i, bool(nodes[i]))
        add('blade_node%d_absent' % i, not nodes[i])
    pos = lambda i, j: inp['frame%d_%d' % (i, 12 + j)]
    add('blade_pos_nan', any(nodes[i] and is_nan(pos(i, j)) for i in range(4) for j in range(3)))
    add('blade_pos_nan_noncanonical', any(nodes[i] and noncanonical_nan(pos(i, j)) for i in range(4) for j in range(3)))
    add('blade_pos_denormal', any(nodes[i] and is_denormal(pos(i, j)) for i in range(4) for j in range(3)))
    add('blade_pos_negzero', any(nodes[i] and pos(i, j) == 0x80000000 for i in range(4) for j in range(3)))
    add('blade_pos_inf', any(nodes[i] and is_special(pos(i, j)) and not is_nan(pos(i, j)) for i in range(4) for j in range(3)))
    add('blade_angle_nan_written', any(wild[3 + i] for i in range(4)))
    add('events_slot_other', 15 in k)


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
        # los colaboradores con hook RH tienen su cuerpo de la DLL parcheado con el mismo falso (y CMatrix::Attach tambien: la DLL llama a su cuerpo)
        if any((s['hooked'] or s['exe'] in (0x59BD10, 0x59B0E0, 0x59B060, 0x59BBB0, 0x6B0690)) and s['dll'] is not True for s in col[fn]['stubbed']):
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
        sys.exit(runtime_runner.run_case('helipre75', manifest['binaries']['asi'], analyze, 'HeliPre75', timeout=3600))
