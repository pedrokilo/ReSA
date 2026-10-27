"""Compuertas del banco HeatFire73 (lote 73): CVehicle::FireHeatSeakingMissile (EXE 0x6E05C0, hook S), 21504 pares.

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
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CVehicle::FireHeatSeakingMissile']
CASES = 21504   # 16384 (familias 0..15) + 5 x 1024 (familias 16..20, ampliacion 27/10/2026)
EXPECTED = {n: CASES for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x6E05C0}
# (direccion del EXE, hooked): GetPlaneOrdnanceRateOfFire, GetPlaneOrdnancePosition, CProjectileInfo::AddProjectile, CPad::StartShake (hooked=False: el hook queda apagado, la entrada del EXE es codigo original)
STUBBED = {NAMES[0]: [(0x6D4590, False), (0x6D46E0, False), (0x737C80, False), (0x53F920, False)]}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst} | {0x6E05C0}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def is_special(b):
    return (b & 0x7F800000) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def noncanonical_nan(b):
    return is_special(b) and (b & 0x7FFFFF) != 0 and (b & 0x7FFFFF) != 0x400000


FLOATS = ('ord_x', 'ord_y', 'ord_z', 'mpos_x', 'mpos_y', 'mpos_z', 'fwd_x', 'fwd_y', 'fwd_z', 'ms_x', 'ms_y', 'ms_z', 'step')


def tally(inp, events, obs, cov):
    """cobertura de ramas desde la entrada y la traza de eventos"""
    k = [e[0] for e in events]
    fired = 3 in k

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    limit = (inp['rate'] + (inp['t1'] if inp['type'] == 1 else inp['t2'])) & 0xFFFFFFFF
    checked = (inp['arg2'] & 0xFF) != 0
    add('gate_checked', checked)
    add('gate_unchecked', not checked)
    add('gate_cut', checked and not fired)
    add('gate_pass', checked and fired)
    add('gate_cut_nullmat', checked and not fired and inp['has_mat'] == 0)
    add('gate_equal_cut', checked and inp['time'] == limit)
    add('boundary_exact_valid_matrix', checked and inp['time'] == limit and inp['has_mat'] == 1)
    add('boundary_exact_null_matrix', checked and inp['time'] == limit and inp['has_mat'] == 0)
    add('boundary_exact_valid_matrix_cut', checked and inp['time'] == limit and inp['has_mat'] == 1 and not fired)
    add('gate_plus1_fire', checked and inp['time'] == ((limit + 1) & 0xFFFFFFFF) and fired)
    add('gate_arg2_hi_garbage', (inp['arg2'] >> 8) != 0)
    add('gate_arg2_byte_gt1', checked and (inp['arg2'] & 0xFF) > 1)
    add('limit_wrap', inp['rate'] + (inp['t1'] if inp['type'] == 1 else inp['t2']) > 0xFFFFFFFF)
    add('type1', inp['type'] == 1)
    add('type_other', inp['type'] != 1)
    add('type1_fire', inp['type'] == 1 and fired)
    add('type_other_fire', inp['type'] != 1 and fired)
    add('type_big', inp['type'] > 3)
    add('fired', fired)
    add('not_fired', not fired)
    ci = (inp['cycle'] >> 2) & 3
    for v in range(4):
        add('cycle_%d_fired' % v, fired and ci == v)
    add('cycle_garbage_bits', fired and (inp['cycle'] & 0xF3) != 0)
    add('target_null', fired and inp['tgt_kind'] == 0)
    add('target_entity', fired and inp['tgt_kind'] == 1)
    add('target_other', fired and inp['tgt_kind'] == 2)
    add('target_raw', fired and inp['tgt_kind'] == 3)
    add('driver_none', fired and inp['driver'] == 0)
    add('driver_player0', fired and inp['driver'] == 1 and inp['ped_type'] == 0)
    add('driver_player1', fired and inp['driver'] == 1 and inp['ped_type'] == 1)
    add('driver_other', fired and inp['driver'] == 1 and inp['ped_type'] not in (0, 1))
    add('driver_none_pedtype_player', fired and inp['driver'] == 0 and inp['ped_type'] in (0, 1))
    add('shake', 4 in k)
    add('proj_ret_hi', fired and (inp['proj_ret'] >> 8) != 0)
    if fired:
        ms = [oracle.load(inp['ms_x']), oracle.load(inp['ms_y']), oracle.load(inp['ms_z'])]
        fw = [oracle.load(inp['fwd_x']), oracle.load(inp['fwd_y']), oracle.load(inp['fwd_z'])]
        dot = oracle.add(oracle.add(oracle.mul(ms[2], fw[2]), oracle.mul(ms[1], fw[1])), oracle.mul(ms[0], fw[0]))
        add('dot_nan', dot == oracle.NAN)
        add('dot_negative', oracle.lt0(dot))
        add('dot_nonnegative', dot != oracle.NAN and not oracle.lt0(dot))
        add('dot_zero', dot != oracle.NAN and dot[0] == 'f' and dot[1] == 0)
        add('dot_inf', dot != oracle.NAN and dot[0] == 'i')
        pos = [e for e in events if e[0] == 3][0][3:6]
        add('out_nan', any(is_special(p) and (p & 0x7FFFFF) != 0 for p in pos))
        add('out_inf', any(is_special(p) and (p & 0x7FFFFF) == 0 for p in pos))
        add('out_denormal', any(is_denormal(p) for p in pos))
        add('out_negzero', any(p == 0x80000000 for p in pos))
        add('out_denormal_y_or_z', is_denormal(pos[1]) or is_denormal(pos[2]))
        add('out_denormal_x', is_denormal(pos[0]))
        inter = oracle.spawn(inp) and oracle.LAST   # intermedios del x87 (dot, fs, ty, tz, A, B, Cz, X, Y, Z)
        add('dot_poszero', oracle.is_signed_zero(inter['dot'], 0))
        add('dot_negzero', oracle.is_signed_zero(inter['dot'], 1))
        add('dot_zero_by_cancellation', dot != oracle.NAN and dot[0] == 'f' and dot[1] == 0 and any(inp[a] & 0x7FFFFFFF for a in ('ms_x', 'ms_y', 'ms_z')) and any(inp[a] & 0x7FFFFFFF for a in ('fwd_x', 'fwd_y', 'fwd_z')))
        add('dot_negative_tiny_fixed_to_zero', oracle.lt0(dot) and dot[0] == 'f' and abs(dot[1]) < oracle.Fraction(1, 1 << 20))
        add('dot_positive_tiny', dot != oracle.NAN and dot[0] == 'f' and 0 < dot[1] < oracle.Fraction(1, 1 << 20))
        for nm in ('fs', 'ty', 'tz', 'a', 'b', 'cz', 'x', 'y', 'z'):
            add('inter_tiny_' + nm, oracle.is_tiny(inter[nm]))
        add('inter_tiny_stored_ty_tz_cz_x', any(oracle.is_tiny(inter[nm]) for nm in ('ty', 'tz', 'cz', 'x')))
        add('inter_tiny_unstored_a_b_y_z', any(oracle.is_tiny(inter[nm]) for nm in ('a', 'b', 'y', 'z')))
        add('step_special', is_special(inp['step']))
        add('step_denormal', is_denormal(inp['step']))
        add('step_zero', inp['step'] & 0x7FFFFFFF == 0)
        for grp, names in (('ord', FLOATS[0:3]), ('mpos', FLOATS[3:6]), ('fwd', FLOATS[6:9]), ('ms', FLOATS[9:12])):
            add('special_' + grp, any(is_special(inp[a]) for a in names))
            add('denormal_' + grp, any(is_denormal(inp[a]) for a in names))
            add('nan_noncanonical_' + grp, any(noncanonical_nan(inp[a]) for a in names))
        add('nan_noncanonical_step', noncanonical_nan(inp['step']))
        add('nan_noncanonical', any(noncanonical_nan(inp[a]) for a in FLOATS))
        add('special_any', any(is_special(inp[a]) for a in FLOATS))
        add('denormal_any', any(is_denormal(inp[a]) for a in FLOATS))
        add('zero_input_signed', any(inp[a] == 0x80000000 for a in FLOATS))


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
    if b.get('cycle_layout_ok') is not True:
        raise RuntimeError('El campo de bits m_nOrdnanceCycleIndex no esta en los bits 2-3 del byte +0x4A9')
    if b['pairs'] != sum(EXPECTED.values()) or b['cases'] != EXPECTED or b['functions'] != len(NAMES):
        raise RuntimeError('Numero de pares inesperado')
    if set(b['registered']) != set(HOOKS) or not all(b['registered'].values()) or not all(b['bodies'].values()):
        raise RuntimeError('Hook RH sin registrar o cuerpo sin resolver')
    col = b['collaborators']
    for fn in NAMES:
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if got != STUBBED.get(fn, []):
            raise RuntimeError('Colaboradores sustituidos inesperados en ' + fn)
        # los cuatro colaboradores tienen hook RH en la DLL (apagado durante el banco): su cuerpo en el ASI lleva el mismo falso
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
        obs = row['candidate']['observed']
        tally(row['input'], obs['events'], obs, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(CASES)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 21 or short:
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


def nan_gate():
    """Compuerta previa opcional (HF73_NAN_GATE=1 o --nan-gate): el microtest nan_rules_hw valida contra el hardware x87 la emulacion de NaN de produccion; exige 0 discrepancias."""
    import nan_rules_hw
    res = nan_rules_hw.run()
    print('nan_rules_hw: %d combinaciones, %s' % (res['part1'] + res['part2'], 'OK' if res['ok'] else 'FALLO'))
    if not res['ok']:
        raise RuntimeError('La emulacion de NaN de HeatFire73SpawnCpp difiere del hardware x87:\n' + res['output'][-2000:])


if __name__ == '__main__':
    import os
    if '--nan-gate' in sys.argv or os.environ.get('HF73_NAN_GATE') == '1':
        sys.argv = [a for a in sys.argv if a != '--nan-gate']
        nan_gate()
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('heatfire73', manifest['binaries']['asi'], analyze, 'HeatFire73', timeout=3600))
