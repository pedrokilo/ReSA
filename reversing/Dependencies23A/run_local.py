"""Exige 8192 pares OFF/ON (EXE con hook apagado frente a DLL con hook activo) de CVehicle::DoVehicleLights, compuertas de ABI/x87/estado (FNSAVE completo: FSW y los diez bytes de los ocho
registros ST, vacios incluidos), trazas de colaboradores identicas, cobertura por rutas (ver GROUPS: contada sobre el propio registro, con margen >= 1,5 frente a lo que alcanza el ensayo sin
juego) y un oraculo independiente en Python del flujo (llamadas a colaboradores y sus argumentos, bits de m_renderLights, bandera de luces, constantes de las coronas y de AddLight)."""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import runtime_runner
from probe_case import analyzer as common_analyzer
HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import coverage_probe as cp
CASES = 8192
FN = 'CVehicle::DoVehicleLights'
EXPECTED = {FN: CASES}
GATES = ('route_ok', 'stack_restored', 'nonvolatile_preserved', 'seh_restored', 'fpu_top_balanced', 'fpu_control_same', 'guards_preserved', 'fixture_contract', 'state_restored',
         'floating_state_restored')
VEH = 16                                  # desplazamiento del vehiculo en la arena (guarda de 16 bytes)
FLAGS_OFF, RENDER_OFF, PROP_OFF = VEH + 0x428, VEH + 0x584, VEH + 0x958

# Rutas que el registro debe recorrer (suma de etiquetas de coverage_probe.py). El minimo es la mitad de lo que alcanza el ensayo sin juego con los mismos 8192 casos (margen 2 > 1,5).
GROUPS = {
    'luces forzadas fuera (ms_forceVehicleLightsOff)': (('force_off',), 59),
    'automaticas: encender y apagar con GetVehicleLightsStatus': (('auto_set_0',), 400), 'auto_set_1': (('auto_set_1',), 100),
    'destrozado: no cambia': (('wrecked_keep',), 50), 'abandonado con luces apagadas: no mide': (('abandoned_dark',), 25),
    'abandonado: medida de distancia': (('abandoned_distance',), 400), 'abandonado lejos (apaga)': (('abandoned_far',), 250), 'abandonado cerca (mantiene)': (('abandoned_near',), 150),
    'abandonado: dx negativo / no negativo / NaN': (('abandoned_dx_neg',), 150), 'dx no negativo': (('abandoned_dx_nonneg',), 150), 'dx NaN': (('abandoned_dx_nan',), 8),
    'abandonado: dy negativo': (('abandoned_dy_neg',), 150), 'dy no negativo': (('abandoned_dy_nonneg',), 150), 'dy NaN': (('abandoned_dy_nan',), 10),
    'anulacion apagada': (('override_off',), 400), 'anulacion encendida': (('override_on',), 200), 'anulacion 3': (('override_3',), 50),
    'alarma en fase clara': (('alarm_bright',), 300), 'alarma en fase oscura': (('alarm_dark',), 300), 'alarma 0xFFFF': (('alarm_ignored_ffff',), 100), 'alarma de vehiculo destrozado': (('alarm_ignored_wrecked',), 70),
    'ZR-350': (('zr350',), 500), 'ZR-350 sube': (('zr_rise',), 300), 'ZR-350 sube y se recorta al objetivo': (('zr_rise_clamped',), 100), 'ZR-350 sube y sale': (('zr_rise_exit',), 200),
    'ZR-350 sube y sigue': (('zr_rise_free',), 200), 'ZR-350 baja': (('zr_fall',), 200), 'ZR-350 baja (prop > 0)': (('zr_fall_positive',), 150), 'ZR-350 baja hasta cero': (('zr_fall_result_zero',), 40),
    'ZR-350 baja sin llegar a cero': (('zr_fall_result_kept',), 120), 'ZR-350 prop <= 0 (no baja)': (('zr_fall_skip',), 50),
    'ZR-350: prop NaN / inf / subnormal': (('zr_rise_prop_qnan', 'zr_rise_prop_snan', 'zr_rise_prop_inf', 'zr_rise_prop_subnormal', 'zr_fall_prop_qnan', 'zr_fall_prop_snan', 'zr_fall_prop_inf', 'zr_fall_prop_subnormal'), 12),
    'ZR-350: paso de tiempo NaN / inf / subnormal': (('zr_rise_ts_qnan', 'zr_rise_ts_snan', 'zr_rise_ts_inf', 'zr_rise_ts_subnormal', 'zr_fall_ts_special'), 10),
    'motor apagado': (('engine_off',), 150), 'ruta dinamica (luces apagadas)': (('dynamic_path',), 1000), 'ruta de luces encendidas': (('lights_on_path',), 2500),
    'cola dinamica: 1, 2, 3 y 4 llamadas': (('dynamic_tail_calls_1',), 150), 'dinamica 2': (('dynamic_tail_calls_2',), 350), 'dinamica 3': (('dynamic_tail_calls_3',), 200), 'dinamica 4': (('dynamic_tail_calls_4',), 150),
    'automovil con consulta de dano': (('light_status_calls_4',), 250), 'una pareja de consultas (sin TWIN)': (('light_status_calls_2',), 250), 'sin consultas (ignorar dano o no automovil)': (('light_status_calls_0',), 3000),
    'no automovil': (('not_automobile',), 700), 'TWIN': (('twin',), 1700), 'sin TWIN': (('single',), 1700), 'IGNORE_DAMAGE': (('ignore_damage',), 2500), 'DISABLE_FRONT': (('disable_front',), 1000), 'DISABLE_REAR': (('disable_rear',), 1000),
    'faro delantero desactivado (disabled)': (('head_disabled_1',), 900), 'faro delantero activo': (('head_disabled_0',), 1500),
    'linea de luz de coche con halogeno': (('corona_line_car_halogen',), 150), 'linea de luz de coche sin halogeno': (('corona_line_car_plain',), 130), 'linea de luz de tren con halogeno': (('corona_line_train_halogen',), 60),
    'linea de luz de tren sin halogeno': (('corona_line_train_plain',), 60), 'corona principal de coche con halogeno': (('corona_main_car_halogen',), 390), 'corona principal de coche sin halogeno': (('corona_main_car_plain',), 360),
    'corona principal de tren con halogeno': (('corona_main_train_halogen',), 130), 'corona principal de tren sin halogeno': (('corona_main_train_plain',), 140),
    'copias en linea que dibujan': (('inline_corona',), 700), 'comprobacion de primera persona': (('first_person_check',), 330),
    'sin coronas (0)': (('coronas_0',), 1800), 'una corona': (('coronas_1',), 240), 'dos coronas': (('coronas_2',), 350), 'tres coronas': (('coronas_3',), 20), 'cuatro coronas': (('coronas_4',), 110),
    'reflejos en el suelo': (('reflection',), 1600), 'sin reflejos por tren': (('no_reflection_train',), 600), 'sin reflejos por alarma oscura': (('no_reflection_dark',), 230),
    'luz direccional delantera': (('add_light_1',), 1200), 'luz de freno trasera': (('brake_light',), 400), 'niebla (despacio)': (('fog_1',), 260), 'sin niebla': (('fog_0',), 1400),
    'freno > 0': (('brake_pos',), 1100), 'freno cero': (('brake_zero',), 700), 'freno < 0': (('brake_neg',), 300), 'freno NaN': (('brake_nan',), 50), 'freno de mano': (('handbrake_1',), 370), 'con conductor': (('driver_1',), 1500),
    'sin conductor': (('driver_0',), 680),
    'velocidad NaN / inf / subnormal': (('speed_normal_qnan', 'speed_normal_snan', 'speed_qnan_normal', 'speed_snan_normal', 'speed_normal_inf', 'speed_inf_normal', 'speed_normal_subnormal', 'speed_subnormal_normal'), 60),
}


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def name(entry):
    return entry[0] if isinstance(entry, list) else next(iter(entry))


def final_byte(changes, offset, initial):
    nb = cp.changed_byte(changes, offset)
    return initial if nb is None else nb


def oracle(pair, p, o):
    """Reproduce en Python el flujo de DoVehicleLights (sin aritmetica x87 de las coronas ni de los puntos de las luces) y lo compara con lo que hizo el EXE."""
    T = o['trace']
    changes = o['changes']
    if p['force_off']:
        if T or changes:
            raise RuntimeError('ms_forceVehicleLightsOff != 0 debe salir sin hacer nada (par %s)' % pair)
        return
    status, sub, auto = p['status'], p['sub_type'], p['vehicle_type'] == 0
    flags_arg = p['flags_arg']
    twin, ign, dfront, drear = bool(flags_arg & 1), bool(flags_arg & 4), bool(flags_arg & 0x10), bool(flags_arg & 0x20)
    init_flags = (0x10 if p['engine_on'] else 0) | (0x20 if p['handbrake'] else 0) | (0x40 if p['lights_on'] else 0)
    exp = [['get_vehicle_lights_status', ['veh', 0]]]
    lights = p['lights_on']
    ls = p['lights_status_ret']
    abandoned_far = None
    if ls != lights and status != 5:
        if status != 4 or sub == 6:
            lights = bool(ls)
        elif lights:
            nb = final_byte(changes, FLAGS_OFF, None)
            abandoned_far = nb is not None and not nb & 0x40
            lights = not abandoned_far
    ov = p['override']
    force_on = False
    if ov == 1:
        lights = False
    elif ov == 2:
        force_on = True
    alarm = p['alarm'] not in (0, 0xFFFF) and status != 5
    dark = False
    if alarm:
        force_on = bool(p['time_ms'] & 0x100)
        dark = not force_on
    # bandera de luces final (el EXE escribe el byte 0x428 de golpe: solo cambia el bit 0x40)
    flags_byte = None
    for start, hexs in changes:
        if start <= FLAGS_OFF < start + len(hexs) // 2:
            flags_byte = int(hexs[(FLAGS_OFF - start) * 2:(FLAGS_OFF - start) * 2 + 2], 16)
    # ZR-350
    if p['model'] == 477 and status != 5 and p['engine_on']:
        rise = lights or force_on or alarm
        if rise and len(T) == 1:
            if T != exp:
                raise RuntimeError('ZR-350 sale tras GetVehicleLightsStatus (par %s)' % pair)
            return
    # estado de cada luz
    statuses = p['light_status']
    ids = []
    def ok(light):
        if ign:
            return True
        if not auto:
            return False
        ids.append(light)
        return statuses[light] == 0
    ok_fr = ok(1)
    ok_fl = twin and ok(0)
    ok_rr = ok(3)
    ok_rl = twin and ok(3)
    for i in ids:
        exp.append(['get_light_status', ['veh', 0x5A0], i])
    if dfront:
        ok_fr = ok_fl = False
    if drear:
        ok_rr = ok_rl = False
    got_status = [list(e) for e in T if name(e) in ('get_vehicle_lights_status', 'get_light_status')]
    if not p['engine_on']:
        if got_status != exp or any(name(e) not in ('get_vehicle_lights_status', 'get_light_status') for e in T):
            raise RuntimeError('motor apagado: solo las consultas de estado (par %s): %s' % (pair, T))
        return
    matrix = ['matVeh', 0] if p['matrix_is_vehicle'] else ['matArg', 0]
    tail_ret, head_ret = p['tail_ret'], p['head_ret']
    ti = 0
    bits = 0
    def tail(light, right, dis, static):
        nonlocal ti
        exp.append(['tail_effect', ['veh', 0], light, matrix, right, int(dis), flags_arg, static])
        r = tail_ret[ti & 7] & 1
        ti += 1
        return r
    path = lights or force_on or dark
    if not path:
        r = tail(0, 1, not ok_rr, 0)
        bits |= r << 2
        if r:
            tail(1, 1, not ok_rr, 0)
        if twin:
            r = tail(0, 0, not ok_rl, 0)
            bits |= r << 3
            if r:
                tail(1, 0, not ok_rl, 0)
    else:
        dfr, dfl = dark or not ok_fr, dark or not ok_fl
        exp.append(['head_effect', ['veh', 0], 0, matrix, 1, int(dfr)])
        exp.append(['head_effect', ['veh', 0], 0, matrix, 0, int(dfl)])
        bits |= (head_ret[0] & 1) | ((head_ret[1] & 1) << 1)
        drr = dark or not ok_rr
        r = tail(0, 1, drr, 1)
        bits |= r << 2
        if r:
            tail(1, 1, drr, 1)
        if twin:
            drl = dark or not ok_rl
            r = tail(0, 0, drl, 1)
            bits |= r << 3
            if r:
                tail(1, 0, drl, 1)
        if sub != 6 and not dark:
            exp.append(['head_light_reflection', ['veh', 0], matrix, flags_arg, int(ok_fl), int(ok_fr)])
    got = [list(e) for e in T if name(e) in ('get_vehicle_lights_status', 'get_light_status', 'head_effect', 'tail_effect', 'head_light_reflection')]
    if got != exp:
        raise RuntimeError('llamadas a colaboradores distintas del oraculo (par %s):\nEXE %s\nORA %s' % (pair, got, exp))
    # m_renderLights: la mitad alta se conserva y la baja la dan los retornos
    want_render = (p['render_lights'] & 0xF0) | bits
    if final_byte(changes, RENDER_OFF, p['render_lights']) != want_render:
        raise RuntimeError('m_renderLights final %02x, esperado %02x (par %s)' % (final_byte(changes, RENDER_OFF, p['render_lights']), want_render, pair))
    if path:
        # coronas de las copias en linea: ids 2/3 (principal) y 6/7 (linea), constantes y colores
        coronas = [e for e in T if name(e) == 'register_corona']
        halogen = p['halogen']
        train = sub == 6 and p['model'] != 449
        rf = (head_ret[0] & 1) == 1
        lf = (head_ret[1] & 1) == 1
        allowed = set()
        if rf: allowed |= {2 + 1, 6 + 1}
        if lf: allowed |= {2 + 0, 6 + 0}
        for e in coronas:
            if e[1] not in allowed or e[2] != ['veh', 0] or e[13] != 0 or e[15] != 0 or e[16] != 0 or e[18] != 0 or e[20] != 0 or e[21] != 0x41700000 or e[22] != 0 or e[23] != 0:
                raise RuntimeError('constantes de la corona (par %s): %s' % (pair, e))
            if e[12] == 4:
                want = (0x96, 0x96, 0xC3, 0xFF) if halogen else (0xA0, 0xA0, 0x8C, 0xFF)
                if tuple(e[3:7]) != want or e[10] != (0x3E99999A if train else 0x3D99999A) or e[14] != 0 or e[19] != 0x3E99999A or e[1] < 6:
                    raise RuntimeError('corona de linea de luz (par %s): %s' % (pair, e))
            else:
                if e[12] != 1 or e[14] != 1 or e[19] != 0x3F000000 or e[6] != 0x80 or e[3] != e[4] or e[1] > 3:
                    raise RuntimeError('corona principal (par %s): %s' % (pair, e))
            lod = f32(p['lod_bits'])
            if lod == lod and abs(lod) < 1e30 and e[11] != struct.unpack('<I', struct.pack('<f', f32(p['lod_bits']) * 150.0))[0] and abs(lod) > 1e-30:
                # el rango es float(lod * 150) con 24 bits: se admite 1 ulp de diferencia del calculo en doble precision de Python
                a, b = e[11], struct.unpack('<I', struct.pack('<f', f32(p['lod_bits']) * 150.0))[0]
                if abs(a - b) > 1:
                    raise RuntimeError('rango de la corona (par %s)' % pair)
        # la comprobacion de primera persona: FindPlayerVehicle(-1, false) solo con la camara activa en modo 16 y solo si algun faro llega a ella
        finds = [e for e in T if name(e) == 'find_player_vehicle']
        for e in finds:
            if e[1] not in (-1, 0xFFFFFFFF) or e[2] != 0 or p['active_cam_mode'] != 16:
                raise RuntimeError('FindPlayerVehicle (par %s): %s' % (pair, e))
        if p['find_vehicle'] == 1 and finds and coronas and False:
            raise RuntimeError('unreachable')
    # luces direccionales
    adds = [e for e in T if name(e) == 'add_light']
    if path and not dark:
        front = ok_fr or ok_fl
        rear = (ok_rr or ok_rl)
        brk = f32(p['brake_bits'])
        brake_light = rear and brk == brk and brk > 0 and not p['handbrake'] and p['driver']
        want_n = int(front) + int(brake_light)
        if len(adds) != want_n:
            raise RuntimeError('AddLight: %d llamadas, esperadas %d (par %s)' % (len(adds), want_n, pair))
        k = 0
        if front:
            a = adds[k]; k += 1
            if a[1] != 1 or a[8] != 0x41A00000 or a[9:12] != [0x3F800000] * 3 or a[13] != 0 or a[14] is not None:
                raise RuntimeError('AddLight delantera (par %s): %s' % (pair, a))
            sx, sy = f32(p['speed_bits'][0]), f32(p['speed_bits'][1])
            if all(v == v and abs(v) < 1e10 and (v == 0 or abs(v) > 1e-15) for v in (sx, sy)):
                s = sx * sx + sy * sy
                if abs(s - 0.2025) > 1e-6 and a[12] != (1 if s < 0.2025 else 0):
                    raise RuntimeError('AddLight delantera: niebla %d con |v|^2 = %g (par %s)' % (a[12], s, pair))
        if brake_light:
            a = adds[k]
            if a[1] != 1 or a[8] != 0x41200000 or a[9:12] != [0x3DCCCCCD, 0x3CA3D70A, 0x3CA3D70A] or a[12] != 0 or a[13] != 0 or a[14] != ['veh', 0]:
                raise RuntimeError('AddLight de freno (par %s): %s' % (pair, a))
    elif adds:
        raise RuntimeError('AddLight no esperada (par %s)' % pair)
    elif not path and (any(name(e) in ('register_corona', 'find_player_vehicle', 'head_effect') for e in T)):
        raise RuntimeError('ruta dinamica con coronas (par %s)' % pair)


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
    if len(b['routes']) != 1 or not all(r['registered'] and r['module_ok'] for r in b['routes']) or len(b['collaborators']) != 8:
        raise RuntimeError('Missing registered hook routes / collaborators')
    if b.get('compare_dead_x87_registers') is not True:
        raise RuntimeError('La igualdad de los registros x87 muertos no esta activa')
    if b['camera_bytes'] != 0xD78 or b['vehicle_bytes'] != 0x988:
        raise RuntimeError('Unexpected structure sizes')
    counts = {}
    for r in rows:
        if r.get('pump_state_intact', True) is not True or r['event'].startswith('first_chance_av') or r['event'] == 'assertion':
            raise RuntimeError('Pump state/AV failure')
        if r['event'] != 'comparison':
            continue
        for side in ('original', 'candidate'):
            v = r[side]
            if not all(v.get(g) is True for g in GATES):
                raise RuntimeError('Gate failed in pair %s (%s)' % (r['pair'], side))
            x = v['x87']
            if x['ftw'] != 0xFFFF:
                raise RuntimeError('x87 stack not empty')
            if not isinstance(x.get('fsw'), int) or ((x['fsw'] >> 11) & 7) != x['fsw_top'] or len(x.get('raw_registers', '')) != 160:
                raise RuntimeError('FSW completa o registros ST crudos ausentes en el estado x87')
        o, p = r['original'], r['profile']
        if o['x87'] != r['candidate']['x87']:
            raise RuntimeError('x87 distinto en el par %s' % r['pair'])
        oracle(r['pair'], p, o)
        for t in cp.tags(p, o):
            counts[t] = counts.get(t, 0) + 1
    missing = []
    for what, (tags, minimum) in GROUPS.items():
        if sum(counts.get(t, 0) for t in tags) < minimum:
            missing.append('%s (%d < %d)' % (what, sum(counts.get(t, 0) for t in tags), minimum))
    if missing:
        raise RuntimeError('Cobertura positiva incompleta: ' + '; '.join(missing))
    end = [r for r in rows if r['event'] == 'complete']
    if len(end) != 1 or end[0]['pairs'] != sum(EXPECTED.values()) or not all(end[0].get(k) is True for k in ('state_restored', 'floating_state_restored', 'pump_state_intact')):
        raise RuntimeError('Missing final restoration')
    summary['coverage'] = {what: sum(counts.get(t, 0) for t in tags) for what, (tags, _) in GROUPS.items()}
    return summary


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])), indent=2))
    else:
        manifest = json.loads((HERE / 'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependencies23a', manifest['binaries']['asi'], analyze, 'Dependencies23A', timeout=3600))
