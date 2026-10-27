"""Compuertas del banco Helpers66A (lote 66: PlayerWantsToAttack 0x60CC50, EvaluateNeighbouringTarget 0x60D1C0, HasClearLineOfSight 0x621B10, AddPunchImpact 0x49F670 y RsSavePresetView 0x619FA0):
4096 pares por funcion.

Ademas de la igualdad EXE/DLL fila a fila (probe_case.analyzer), exige hooks registrados y cuerpos en el ASI, todas las compuertas de ABI/ruta/DF/SEH/restauracion
en AMBOS lados, fp_difference vacio y cobertura por familia de las fixtures y cobertura de ramas medida sobre las entradas y los eventos de los colaboradores (los falsos registran
sus argumentos, en orden). `python -B run_local.py --analyze <comparisons.jsonl>` solo analiza (devuelve None si el registro aun no existe o no esta completo).
"""
import json, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

NAMES = ['CPlayerPed::PlayerWantsToAttack', 'CPlayerPed::EvaluateNeighbouringTarget', 'CTaskSimpleGangDriveBy::HasClearLineOfSight', 'Fx_c::AddPunchImpact', 'RsSavePresetView']
EXPECTED = {n: 4096 for n in NAMES}
GATES = ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored')
HOOKS = {NAMES[0]: 0x60CC50, NAMES[1]: 0x60D1C0, NAMES[2]: 0x621B10, NAMES[3]: 0x49F670, NAMES[4]: 0x619FA0}
STUBBED = {
    NAMES[0]: [(0x5F6AA0, True), (0x5F8780, True), (0x49CCE0, True), (0x59F320, True), (0x60C5F0, True), (0x5F7CC0, True)],
    NAMES[1]: [(0x60C0C0, True), (0x53CC70, True)],
    NAMES[2]: [(0x56A490, True), (0x821B1E, False)],
    NAMES[3]: [(0x4AA440, True)],
    NAMES[4]: [(0x745470, True), (0x7454E0, True), (0x7451D0, True), (0x7F0990, False), (0x7ED9B0, False), (0x619D60, True), (0x619780, False)],
}
WATCHED = {a for lst in STUBBED.values() for a, _ in lst}
# Cobertura minima de ramas (calibrada con la primera corrida, con margen): coverage.json = {funcion: {clave: minimo de casos}}
COVER = json.loads((HERE / 'coverage.json').read_text()) if (HERE / 'coverage.json').exists() else {}


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def kinds(side):
    return [e[0] for e in side['observed'].get('events', [])]


def tally(fn, row, cov):
    inp = row['input']
    cand = row['candidate']
    k = kinds(cand)

    def add(name, cond=True):
        if cond:
            cov[name] = cov.get(name, 0) + 1
    if fn == NAMES[0]:
        count = inp['count'] - (1 << 32) if inp['count'] >= 1 << 31 else inp['count']
        add('count_fail', count < 1)
        add('flag_fail', count >= 1 and inp['flag_byte'] == 0)
        add('attack_direct', 6 in k and 5 not in k)
        add('attack_found', 6 in k and 5 in k)
        add('istag_exit', 3 in k and 5 not in k and 6 not in k)
        add('object_exit', 4 in k and 5 not in k and 6 not in k)
        add('find_null', 5 in k and 6 not in k)
        add('mouse_target_used', 6 in k and inp['t_kind'] == 0 and inp['m_kind'] == 1 and inp['cam_mouse'] != 0 and 5 not in k)
        add('mouse_target_ignored', 5 in k and inp['t_kind'] == 0 and inp['m_kind'] == 1 and inp['cam_mouse'] == 0)
        add('target_beats_mouse', 6 in k and inp['t_kind'] == 1 and inp['m_kind'] == 1 and inp['cam_mouse'] != 0)
        add('group_nonzero', inp['group'] != 0)
        add('report_called', 2 in k)
    elif fn == NAMES[1]:
        ch = cand['observed']['changes']
        stored = len(ch[5]) > 0 or len(ch[6]) > 0
        a = f32(inp['atan_ret']) - f32(inp['comp'])
        add('out_of_range', not k)
        add('broken_exit', 11 in k and 12 not in k)
        add('atan_called', 12 in k)
        add('stored', stored)
        add('angle_above_pi', 12 in k and a > 3.1415927)
        add('angle_below_minus_pi', 12 in k and a < -3.1415927)
        add('angle_outside_50', 12 in k and abs(a) >= 0.8726646)
        add('angle_nan', 12 in k and (is_nan(inp['atan_ret']) or is_nan(inp['comp'])))
        add('not_stored_priority', 12 in k and abs(a) < 0.8726646 and not stored)
        add('flag_right', 12 in k and (inp['flag'] & 0xFF) != 0)
        add('flag_left', 12 in k and (inp['flag'] & 0xFF) == 0)
        add('flag_high_bits', (inp['flag'] & 0xFFFFFF00) != 0)
        add('cam_matrix', inp['cam_mat'] == 1)
        add('max_nan', is_nan(inp['max_dist']))
        add('tiny_positions', any(is_denormal(v) for v in inp['self'] + inp['tgt']))
    elif fn == NAMES[2]:
        add('skipped', not k)
        add('checked_target', 21 in k)
        add('no_target', 22 in k and 21 not in k)
        add('result_clear', cand['observed']['al'] != 0)
        add('result_blocked', cand['observed']['al'] == 0)
        add('signed_trap', inp['timer'] >= 0x80000000 and inp['los_time'] < 0x80000000 and 22 in k)
        add('y_snan', 21 in k and is_nan(inp['pos'][1]) and (inp['pos'][1] & 0x400000) == 0)
        add('nan_pos', 21 in k and any(is_nan(v) for v in inp['pos']))
        add('blocked_byte_odd', inp['blocked'] not in (0, 1))
        add('matrix', inp['has_mat'] == 1 and 21 in k)
    elif fn == NAMES[3]:
        add('in_range', k.count(31) == 2)
        add('out_of_range', not k)
        add('nan_distance', k.count(31) == 2 and any(is_nan(v) for v in inp['cam'] + inp['pos']))
        add('matrix_camera', inp['cam_mode'] == 1)
        add('denormal', any(is_denormal(v) for v in inp['cam'] + inp['pos']))
    elif fn == NAMES[4]:
        add('open_fail', 43 in k and 44 not in k)
        add('write_fail', 45 in k and 52 in k and 48 not in k)
        add('loaded', 48 in k)
        add('set_view', 49 in k)
        add('load_zero', 48 in k and 49 not in k)
        add('kb_used', 53 in k)
        add('clamp_hi', 51 in k and f32(inp['at'][1]) > 1.0)
        add('clamp_lo', 51 in k and f32(inp['at'][1]) < -1.0)
        add('nan_at', 51 in k and any(is_nan(v) for v in inp['at']))
        add('create_null', inp['create_kind'] == 1)
        add('inf_at', 51 in k and any(is_inf(v) for v in inp['at']))
        add('denormal_at', 51 in k and any(is_denormal(v) for v in inp['at']))


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
        got = [(s['exe'], s['hooked']) for s in col[fn]['stubbed']]
        if col[fn]['dll_real'] or got != STUBBED.get(fn, []):
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
        diff = oracle.check(fn, row)   # modelo del desensamblado contra el lado ORIGINAL (EXE); original == candidato ya esta comprobado arriba
        if diff:
            raise RuntimeError('El oraculo difiere del EXE en %s %d: %s' % (fn, case, diff))
        f = row['input']['family']
        fam[fn][f] = fam[fn].get(f, 0) + 1
        tally(fn, row, cov[fn])
    for fn in NAMES:
        if seen[fn] != set(range(4096)):
            raise RuntimeError('Cobertura incompleta en ' + fn)
        short = {k: v for k, v in fam[fn].items() if v < 200}
        if len(fam[fn]) != 16 or short:
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
        sys.exit(runtime_runner.run_case('helpers66a', manifest['binaries']['asi'], analyze, 'Helpers66A', timeout=3600))
