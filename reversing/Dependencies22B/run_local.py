"""Exige 8192 pares OFF/ON (EXE con hook apagado frente a DLL con hook activo; 4096 por funcion), compuertas de ABI/x87/estado, trazas de colaboradores identicas y cobertura positiva
por rama, con un oraculo independiente en Python para los argumentos constantes de RegisterCorona, los ids, el tamano de la linea de luz, el rango y el valor de retorno."""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
CASES = 4096
HEAD = 'CVehicle::DoHeadLightEffect'
TAIL = 'CVehicle::DoTailLightEffect'
EXPECTED = {HEAD: CASES, TAIL: CASES}
GATES = ('route_ok', 'stack_restored', 'nonvolatile_preserved', 'seh_restored', 'fpu_top_balanced', 'fpu_control_same', 'guards_preserved',
         'fixture_contract', 'state_restored', 'floating_state_restored')
DIRECTED = {'none', 'range', 'tiny', 'bigforward', 'camera-at-point', 'threshold'}


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def bits32(value):
    """float32 mas cercano a value (inf si desborda), como bits."""
    try:
        return struct.unpack('<I', struct.pack('<f', value))[0]
    except OverflowError:
        return 0x7F800000 if value > 0 else 0xFF800000


def kind(bits):
    exp, man = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if exp == 0xFF:
        return ('snan' if not man & 0x400000 else 'qnan') if man else 'inf'
    return 'zero' if bits & 0x7FFFFFFF == 0 else ('subnormal' if exp == 0 else 'normal')


def name(entry):
    """Las entradas de traza son listas [nombre, ...] (tres o mas elementos) u objetos {nombre: valor} (dos elementos, nlohmann)."""
    return entry[0] if isinstance(entry, list) else next(iter(entry))


def coronas(trace):
    return [e for e in trace if name(e) == 'register_corona']


def lod_range_bits(profile):
    lod = f32(profile['lod_bits'])
    if lod != lod:
        return None
    return bits32(lod * 150.0)


def check_corona(c, fn, p):
    """Oraculo independiente de los campos constantes de una corona (indices de la entrada: 1 id, 2 attach, 3..6 rgba, 7..9 pos, 10 size, 11 range, 12 type, 13 flare, 14 refl, 15 los, 16 trail,
    17 angle, 18 neon, 19 pull, 20 fullBright, 21 fade, 22 below, 23 white)."""
    kind_ = c[12]
    base = p['is_right'] & 0xFF
    base += 2 * p['light_id']
    expected_id = base + (4 if fn == HEAD and kind_ == 4 else 8 if fn == TAIL else 0)
    if c[1] != expected_id or c[2] != ['veh', 0]:
        raise RuntimeError('id/attach de la corona: %s esperado %s' % (c[1:3], expected_id))
    train = p['sub_type'] == 6 and p['model'] != 449
    if c[16] != 0 or c[15] != 0 or c[13] != 0 or c[18] != 0 or c[20] != 0 or c[22] != 0 or c[23] != 0 or c[21] != 0x41700000:
        raise RuntimeError('argumentos constantes de la corona')
    if fn == HEAD and kind_ == 4:
        want = (0x96, 0x96, 0xC3, 0xFF) if p['halogen'] else (0xA0, 0xA0, 0x8C, 0xFF)
        if tuple(c[3:7]) != want or c[10] != (0x3E99999A if train else 0x3D99999A) or c[14] != 0 or c[19] != 0x3E99999A:
            raise RuntimeError('corona de linea de luz (tipo 4): %s' % (c,))
    else:
        if kind_ != 1 or c[14] != 1 or c[19] != 0x3F000000 or c[6] != 0x80:
            raise RuntimeError('corona de tipo 1: %s' % (c,))
        if fn == HEAD and (c[3] != c[4]):
            raise RuntimeError('cabeza: rojo y verde distintos')
        if fn == TAIL and (c[4] != 0 or c[5] != 0):
            raise RuntimeError('cola: verde/azul distintos de 0')
    want_range = lod_range_bits(p)
    if want_range is not None and c[11] != want_range:
        raise RuntimeError('rango de la corona %08X esperado %08X' % (c[11], want_range))
    return train


def tail_mode(p):
    brake = f32(p['brake_bits'])
    return 1 if (brake > 0.0 and not p['handbrake'] and p['driver']) else (2 if p['static_emission'] else 0)


def analyze(path):
    summary = common_analyzer(EXPECTED)(path)
    if summary is None:
        return None
    rows = [json.loads(l) for l in path.read_text(encoding='utf-8').splitlines()]
    begin = [r for r in rows if r['event'] == 'begin']
    if len(begin) != 1:
        raise RuntimeError('Missing begin')
    b = begin[0]
    if not all(b.get(k) is True for k in ('registered_hook', 'ready', 'target_routes_ready', 'fixture_contract', 'collaborator_bodies_ok', 'pump_state_intact')):
        raise RuntimeError('Invalid preflight')
    if len(b['routes']) != 2 or not all(r['registered'] and r['module_ok'] for r in b['routes']) or len(b['collaborators']) != 3:
        raise RuntimeError('Missing registered hook routes / collaborators')
    if b['camera_bytes'] != 0xD78 or b['vehicle_bytes'] != 0x988:
        raise RuntimeError('Unexpected structure sizes')
    cov = {fn: {k: set() for k in ('directed', 'light', 'right', 'disabled', 'ret', 'coronas', 'types', 'train', 'halogen', 'vmatrix', 'cmatrix', 'matrix_is_vehicle', 'fp_skip', 'find',
                                   'kinds', 'models', 'panel_calls', 'panel_zero', 'panel_nonzero', 'modes', 'thr_coronas', 'red_zero', 'red_nonzero', 'brake_kinds')} for fn in (HEAD, TAIL)}
    for r in rows:
        if r.get('pump_state_intact', True) is not True or r['event'].startswith('first_chance_av') or r['event'] == 'assertion':
            raise RuntimeError('Pump state/AV failure')
        if r['event'] != 'comparison':
            continue
        for side in ('original', 'candidate'):
            v = r[side]
            if not all(v.get(g) is True for g in GATES):
                raise RuntimeError('Gate failed in pair %s (%s)' % (r['pair'], side))
            if v['x87']['ftw'] != 0xFFFF:
                raise RuntimeError('x87 stack not empty')
            if v['changes']:
                raise RuntimeError('La funcion escribio memoria en el par %s (%s)' % (r['pair'], side))
        o, p, fn = r['original'], r['profile'], r['function']
        c = cov[fn]
        cs = coronas(o['trace'])
        calls = {name(e) for e in o['trace']}
        al = o['x87']['al']
        c['directed'].add(p['directed']); c['light'].add(p['light_id']); c['right'].add(min(p['is_right'], 2)); c['disabled'].add(min(p['disabled_or_alarm'], 1))
        c['ret'].add(al); c['coronas'].add(len(cs)); c['vmatrix'].add(p['vehicle_matrix']); c['cmatrix'].add(p['camera_matrix']); c['matrix_is_vehicle'].add(p['matrix_is_vehicle'])
        c['find'].add('find_player_vehicle' in calls); c['models'].add(p['model'] if p['model'] in (0x1B7, 0x1DB, 0x1C1) else 0)
        # --- oraculo de retorno y de argumentos
        if p['disabled_or_alarm'] != 0 and (al != 0 or cs):
            raise RuntimeError('disabledOrAlarm != 0 debe devolver false sin coronas (par %s)' % r['pair'])
        if 'find_player_vehicle' in calls:
            fp = [e for e in o['trace'] if name(e) == 'find_player_vehicle']
            if len(fp) != 1 or fp[0][1] != 0xFFFFFFFF and fp[0][1] != -1 or fp[0][2] != 0 or p['active_cam_mode'] != 16:
                raise RuntimeError('FindPlayerVehicle: argumentos o condicion (par %s): %s' % (r['pair'], fp))
            if p['find_vehicle'] == 1 and cs:
                raise RuntimeError('con la camara en primera persona sobre el propio vehiculo no se registran coronas')
            c['fp_skip'].add(p['find_vehicle'])
        for e in cs:
            c['types'].add(e[12])
            train = check_corona(e, fn, p)
            c['train'].add(train); c['halogen'].add(p['halogen'])
            for v in e[7:12] + [e[17]]:
                c['kinds'].add(kind(v))
        if fn == HEAD:
            if cs and al != 1:
                raise RuntimeError('cabeza con coronas debe devolver true')
            if [e[12] for e in cs] not in ([], [1], [4, 1]):
                raise RuntimeError('orden/tipos de las coronas de la cabeza: %s' % [e[12] for e in cs])
            if p['directed'] == 'threshold':
                c['thr_coronas'].add(len(cs))
        else:
            if len(cs) > 1:
                raise RuntimeError('la cola registra como mucho una corona')
            if p['model'] in (0x1B7, 0x1DB):
                pc = [e for e in o['trace'] if name(e) == 'get_panel_status']
                if len(pc) != 1 or pc[0][2] != 6:
                    raise RuntimeError('GetPanelStatus: una llamada con el panel 6 (par %s): %s' % (r['pair'], pc))
                c['panel_calls'].add(True)
                (c['panel_nonzero'] if p['panel'] else c['panel_zero']).add(bool(cs))
                if p['panel'] and (cs or al):
                    raise RuntimeError('Stallion/Sabre con el parachoques danado no dibujan')
            elif 'get_panel_status' in calls:
                raise RuntimeError('GetPanelStatus solo para el Stallion y el Sabre')
            if cs:
                mode = tail_mode(p)
                c['modes'].add(mode)
                if al != (1 if mode else 0):
                    raise RuntimeError('cola: retorno %s con modo %s (par %s)' % (al, mode, r['pair']))
                (c['red_nonzero'] if cs[0][3] else c['red_zero']).add(mode)
                brake = f32(p['brake_bits'])
                c['brake_kinds'].add('nan' if brake != brake else 'pos' if brake > 0 else 'zero' if brake == 0 else 'neg')
            elif al:
                raise RuntimeError('cola sin coronas debe devolver false')
    h, t = cov[HEAD], cov[TAIL]
    need = [
        (h['directed'] == DIRECTED and t['directed'] == DIRECTED, 'escenarios dirigidos de ambas funciones'),
        (h['light'] == {0, 1} and t['light'] == {0, 1} and h['right'] == {0, 1, 2} and t['right'] == {0, 1, 2}, 'indice de luz y lado (0, 1, otros bytes)'),
        (h['disabled'] == {0, 1} and t['disabled'] == {0, 1}, 'disabledOrAlarm'),
        (h['ret'] == {0, 1} and t['ret'] == {0, 1}, 'retorno true y false'),
        (h['coronas'] == {0, 1, 2} and t['coronas'] == {0, 1}, 'numero de coronas'),
        (h['types'] == {1, 4} and t['types'] == {1}, 'tipos de corona'),
        (h['train'] == {True, False} and t['train'] == {True, False}, 'tren frente a no tren con coronas'),
        (h['halogen'] == {True, False}, 'halogeno con coronas'),
        (h['vmatrix'] == {True, False} and t['vmatrix'] == {True, False} and h['cmatrix'] == {True, False} and t['cmatrix'] == {True, False}, 'matriz del vehiculo y de la camara presente/ausente'),
        (h['matrix_is_vehicle'] == {True, False} and t['matrix_is_vehicle'] == {True, False}, 'matriz del argumento = la del vehiculo'),
        (h['find'] == {True, False} and t['find'] == {True, False} and h['fp_skip'] == {0, 1, 2} and t['fp_skip'] == {0, 1, 2}, 'primera persona: FindPlayerVehicle nulo / el propio / otro'),
        ({'normal', 'zero', 'subnormal', 'inf'} <= h['kinds'] and {'normal', 'zero', 'inf'} <= t['kinds'], 'estres de coma flotante en los argumentos de las coronas'),
        (t['models'] == {0, 0x1B7, 0x1DB, 0x1C1}, 'modelos especiales'),
        (t['panel_calls'] == {True} and t['panel_zero'] == {True, False} and t['panel_nonzero'] == {False}, 'Stallion/Sabre con y sin parachoques danado'),
        (t['modes'] == {0, 1, 2} and t['red_zero'] >= {0} and t['red_nonzero'] >= {1, 2}, 'modos de la cola (sin luz, freno, emision estatica)'),
        (t['brake_kinds'] >= {'pos', 'zero', 'neg'}, 'pedal de freno positivo, cero y negativo'),
        (h['thr_coronas'] == {0, 1, 2}, 'umbrales de la cabeza: 0, 1 y 2 coronas'),
    ]
    missing = [what for ok, what in need if not ok]
    if missing:
        raise RuntimeError('Cobertura positiva incompleta: ' + '; '.join(missing))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'pump_state_intact')):
        raise RuntimeError('Missing final restoration')
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependencies22b', manifest['binaries']['asi'], analyze, 'Dependencies22B', timeout=1800))
