"""Exige 11264 pares OFF/ON (EXE con hook apagado frente a DLL con hook activo), compuertas de ABI/x87/estado y cobertura positiva por rama."""
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
CLS = 'CTaskSimpleGangDriveBy::'
EXPECTED = {CLS + 'Constructor': 1024, CLS + 'PlayerTarget': 2048, CLS + 'AITarget': 4096, CLS + 'AimGun': 4096}
GATES = ('route_ok', 'stack_restored', 'nonvolatile_preserved', 'seh_restored', 'fpu_top_balanced', 'fpu_control_same', 'guards_preserved', 'vptr_ok',
         'fixture_contract', 'state_restored', 'floating_state_restored')


def name(entry):
    """Las entradas de traza son listas [nombre, ...] o, con dos elementos, objetos {nombre: valor} (nlohmann)."""
    return entry[0] if isinstance(entry, list) else next(iter(entry))


def kind(bits):
    exp, man = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if exp == 0xFF:
        return ('snan' if not man & 0x400000 else 'qnan') if man else 'inf'
    return 'zero' if bits & 0x7FFFFFFF == 0 else ('subnormal' if exp == 0 else 'normal')


def trace_kinds(trace):
    """Clases de valores float (como bits) que llegaron a los colaboradores: exige estres real (NaN con y sin senalizar, inf, ceros, subnormales)."""
    seen = set()
    for e in trace:
        values = e[1:] if isinstance(e, list) else []
        for v in values:
            if isinstance(v, int) and 0x10000 < v <= 0xFFFFFFFF:
                seen.add(kind(v))
    return seen


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(l) for l in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1:
        raise RuntimeError('Missing begin')
    b = begin[0]
    if not all(b.get(k) is True for k in ('registered_hook', 'ready', 'target_routes_ready', 'pump_state_intact')):
        raise RuntimeError('Invalid preflight')
    if len(b['routes']) != 4 or not all(r['registered'] and r['module_ok'] for r in b['routes']):
        raise RuntimeError('Missing registered hook routes')
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('La igualdad de los registros x87 muertos no esta activa')
    cov = {k: set() for k in ('ctor_target', 'ctor_pos', 'ctor_style', 'pt_mode', 'pt_player', 'pt_coop', 'pt_vm', 'pt_pd', 'pt_anim', 'pt_calls',
                              'ai_style', 'ai_kind', 'ai_tm', 'ai_pm', 'ai_los', 'ai_in', 'ai_next', 'ai_anim', 'ai_calls',
                              'am_player', 'am_bike', 'am_kind', 'am_calls', 'am_flag', 'am_radian', 'am_anim', 'am_kinds', 'errno')}
    for r in rows:
        if r.get('pump_state_intact', True) is not True or r['event'].startswith('first_chance_av'):
            raise RuntimeError('Pump state/AV failure')
        if r['event'] != 'comparison':
            continue
        for side in ('original', 'candidate'):
            v = r[side]
            if not all(v.get(g) is True for g in GATES):
                raise RuntimeError('Gate failed in pair %s (%s)' % (r['pair'], side))
            if v['x87']['ftw'] != 0xFFFF:
                raise RuntimeError('x87 stack not empty')
            if not isinstance(v['x87'].get('fsw'), int) or ((v['x87']['fsw'] >> 11) & 7) != v['x87']['fsw_top'] or len(v['x87'].get('raw_registers', '')) != 160:
                raise RuntimeError('FSW completa o registros ST crudos ausentes en el estado x87')
        o, p, fn = r['original'], r['profile'], r['function']
        calls = {name(e) for e in o['trace']}
        cov['errno'].add(o['errno'])
        if fn.endswith('Constructor'):
            cov['ctor_target'].add(p['has_target']); cov['ctor_pos'].add(p['has_position']); cov['ctor_style'].add(p['style'])
            if not o['return'] or not o['changes']:
                raise RuntimeError('Constructor wrote nothing / returned null')
        elif fn.endswith('PlayerTarget'):
            cov['pt_mode'].add(p['cam_mode']); cov['pt_player'].add(p['is_player']); cov['pt_coop'].add(p['coop']); cov['pt_vm'].add(p['vehicle_matrix']); cov['pt_pd'].add(p['player_data'])
            cov['pt_anim'].add(o['task_after'][4]); cov['pt_calls'] |= calls
        elif fn.endswith('AITarget'):
            cov['ai_style'].add(p['style']); cov['ai_kind'].add(p['target_kind']); cov['ai_tm'].add(p['target_matrix']); cov['ai_pm'].add(p['ped_matrix']); cov['ai_los'].add(p['los'])
            cov['ai_in'].add(o['task_after'][0]); cov['ai_next'].add(o['task_after'][3]); cov['ai_anim'].add(o['task_after'][4]); cov['ai_calls'] |= calls
        else:
            cov['am_player'].add(p['player']); cov['am_bike'].add(p['bike']); cov['am_kind'].add(p['target_kind']); cov['am_calls'] |= calls; cov['am_anim'].add(p['anim'])
            for e in o['trace']:
                if name(e) == 'point_gun_in_direction':
                    cov['am_flag'].add(e[4])
            cov['am_radian'].add(sum(1 for e in o['trace'] if name(e) == 'radian_angle'))
        cov['am_kinds'] |= trace_kinds(o['trace']) if fn.endswith('AimGun') else set()
    need = [
        (cov['ctor_target'] == {True, False} and cov['ctor_pos'] == {True, False} and cov['ctor_style'] >= set(range(9)), 'constructor: objetivo/posicion/estilos'),
        ({0x37, 0x31} <= cov['pt_mode'] and len(cov['pt_mode']) >= 4 and cov['pt_player'] == {True, False} and cov['pt_coop'] == {True, False} and cov['pt_vm'] == {True, False} and cov['pt_pd'] == {True, False}, 'PlayerTarget: modos de camara y banderas'),
        ({231, 232, 233, 234, 235, 236, 237, 238} <= cov['pt_anim'], 'PlayerTarget: las ocho animaciones'),
        ({'set_new_player_weapon_mode', 'two_player_aim', 'is_player', 'player_info', 'is_coop'} <= cov['pt_calls'], 'PlayerTarget: colaboradores'),
        (set(range(9)) <= cov['ai_style'] and len(cov['ai_style']) >= 12 and cov['ai_kind'] == set(range(5)) and cov['ai_tm'] == {True, False} and cov['ai_pm'] == {True, False} and cov['ai_los'] == {True, False}, 'AITarget: estilos/objetivos/matrices/LOS'),
        (cov['ai_in'] == {0, 1} and 7 in cov['ai_next'] and {231, 232, 233, 234, 235, 236, 237, 238, 191} <= cov['ai_anim'], 'AITarget: zona de tiro, aborto y animaciones'),
        ({'line_of_sight', 'allocate_matrix', 'update_matrix'} <= cov['ai_calls'], 'AITarget: colaboradores'),
        (cov['am_player'] == {True, False} and cov['am_bike'] == {True, False} and cov['am_kind'] == set(range(5)) and cov['am_flag'] == {0, 1} and {1, 2} <= cov['am_radian'], 'AimGun: jugador/moto/objetivos/bandera/elevacion'),
        ({'find_3rd_person_cam_target_vector', 'get_bone_position', 'allocate_matrix', 'point_gun_in_direction', 'radian_angle', 'is_player'} <= cov['am_calls'], 'AimGun: colaboradores'),
        ({'snan', 'qnan', 'inf', 'normal'} <= cov['am_kinds'], 'AimGun: estres de coma flotante (NaN senalizante y silencioso, inf)'),
        ({231, 232, 233, 234, 235, 236, 237, 238} <= cov['am_anim'] | set(), 'AimGun: animaciones'),
        (33 in cov['errno'] and 0 in cov['errno'], 'errno EDOM del asin del EXE'),
    ]
    missing = [what for ok, what in need if not ok]
    if missing:
        raise RuntimeError('Cobertura positiva incompleta: ' + '; '.join(missing))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'pump_state_intact')):
        raise RuntimeError('Missing final restoration')
    summary['compare_dead_x87_registers'] = True
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependencies21d', manifest['binaries']['asi'], analyze, 'Dependencies21D', timeout=1800))
