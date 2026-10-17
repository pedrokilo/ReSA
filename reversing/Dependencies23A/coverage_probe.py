"""Cuenta, sobre un registro de comparaciones (jsonl del banco o del ensayo sin juego), cuantos pares recorren cada ruta de DoVehicleLights.
Derivado del propio registro: perfil del caso + traza y cambios del EXE (el original). uso: python coverage_probe.py <jsonl> [--json]"""
import json
import struct
import sys
from collections import Counter

FLAGS_OFF = 16 + 0x428       # arena: guarda de 16 bytes + desplazamiento en el vehiculo
RENDER_OFF = 16 + 0x584
PROP_OFF = 16 + 0x958


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def kind(bits):
    exp, man = (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if exp == 0xFF:
        return ('snan' if not man & 0x400000 else 'qnan') if man else 'inf'
    return 'zero' if bits & 0x7FFFFFFF == 0 else ('subnormal' if exp == 0 else 'normal')


def name(entry):
    return entry[0] if isinstance(entry, list) else next(iter(entry))


def changed_byte(changes, offset):
    """Nuevo valor del byte `offset` si cambio, o None."""
    for start, hexs in changes:
        n = len(hexs) // 2
        if start <= offset < start + n:
            return int(hexs[(offset - start) * 2:(offset - start) * 2 + 2], 16)
    return None


def changed_dword(changes, offset, old):
    """Valor final del dword en `offset` (los bytes que no cambian conservan los de `old`)."""
    value = 0
    for i in range(4):
        b = changed_byte(changes, offset + i)
        value |= (((old >> (8 * i)) & 0xFF) if b is None else b) << (8 * i)
    return value


def delta_class(a, b):
    x, y = f32(a), f32(b)
    d = x - y
    return 'nan' if d != d else ('neg' if d < 0 else 'nonneg')


def sign_class(bits):
    k = kind(bits)
    if k in ('snan', 'qnan'):
        return 'nan'
    return 'neg' if f32(bits) < 0 else 'nonneg'


def tags(p, o):
    """Etiquetas de ruta de un par. `o` es la mitad del EXE (traza y cambios)."""
    t = []
    T = o['trace']
    names = [name(e) for e in T]
    status = p['status']
    sub = p['sub_type']
    auto = p['vehicle_type'] == 0
    twin = bool(p['flags_arg'] & 1)
    ign = bool(p['flags_arg'] & 4)
    dfront = bool(p['flags_arg'] & 0x10)
    drear = bool(p['flags_arg'] & 0x20)
    if p['force_off']:
        t.append('force_off')
        return t
    ls = p['lights_status_ret']
    on0 = p['lights_on']
    lights = on0
    if ls != on0 and status != 5:
        if status != 4 or sub == 6:
            t.append('auto_set_%d' % ls)
            lights = bool(ls)
        elif on0:
            t.append('abandoned_distance')
            cx, cy = p['cam_xy']
            vx, vy = p['veh_xy']
            t.append('abandoned_dx_' + delta_class(cx, vx))
            t.append('abandoned_dy_' + delta_class(cy, vy))
            nb = changed_byte(o['changes'], FLAGS_OFF)
            far = nb is not None and not nb & 0x40
            t.append('abandoned_far' if far else 'abandoned_near')
            lights = not far
        else:
            t.append('abandoned_dark')
    elif ls != on0:
        t.append('wrecked_keep')
    ov = p['override']
    force_on = False
    if ov == 1:
        t.append('override_off'); lights = False
    elif ov == 2:
        t.append('override_on'); force_on = True
    elif ov == 3:
        t.append('override_3')
    alarm_active = p['alarm'] not in (0, 0xFFFF) and status != 5
    alarm_dark = False
    if alarm_active:
        if p['time_ms'] & 0x100:
            t.append('alarm_bright'); force_on = True
        else:
            t.append('alarm_dark'); force_on = False; alarm_dark = True
    elif p['alarm'] != 0:
        t.append('alarm_ignored_%s' % ('wrecked' if status == 5 and p['alarm'] != 0xFFFF else 'ffff'))
    if p['model'] == 477 and status != 5 and p['engine_on']:
        t.append('zr350')
        prop0 = p['prop_bits']
        if lights or force_on or alarm_active:
            t.append('zr_rise')
            newp = changed_dword(o['changes'], PROP_OFF, prop0)
            ts_kind = kind(p['time_step'])
            t.append('zr_rise_ts_' + ts_kind)
            t.append('zr_rise_prop_' + kind(prop0))
            t.append('zr_rise_clamped' if newp == 0x3F32B8C3 else 'zr_rise_free')
            if names == ['get_vehicle_lights_status']:
                t.append('zr_rise_exit')
                return t
        else:
            t.append('zr_fall')
            t.append('zr_fall_prop_' + kind(prop0))
            if f32(prop0) > 0:
                t.append('zr_fall_positive')
                newp = changed_dword(o['changes'], PROP_OFF, prop0)
                t.append('zr_fall_result_zero' if newp == 0 else 'zr_fall_result_kept')
                if kind(p['time_step']) in ('snan', 'qnan', 'inf'):
                    t.append('zr_fall_ts_special')
            else:
                t.append('zr_fall_skip')
    if not p['engine_on']:
        t.append('engine_off')
        return t
    # luces de dano
    nstatus = names.count('get_light_status')
    t.append('light_status_calls_%d' % nstatus)
    t.append('automobile' if auto else 'not_automobile')
    t.append('twin' if twin else 'single')
    if ign:
        t.append('ignore_damage')
    if dfront:
        t.append('disable_front')
    if drear:
        t.append('disable_rear')
    heads = [e for e in T if name(e) == 'head_effect']
    tails = [e for e in T if name(e) == 'tail_effect']
    if heads:
        t.append('lights_on_path')
        t.append('head_disabled_%d' % heads[0][5])
        coronas = [e for e in T if name(e) == 'register_corona']
        types = Counter(e[12] for e in coronas)
        t.append('coronas_%d' % len(coronas))
        if types[4]:
            t.append('corona_line')
        if types[1]:
            t.append('corona_main')
        if any(name(e) == 'find_player_vehicle' for e in T):
            t.append('first_person_check')
        train = sub == 6 and p['model'] != 449
        for e in coronas:
            t.append(('corona_line' if e[12] == 4 else 'corona_main') + ('_train' if train else '_car') + ('_halogen' if p['halogen'] else '_plain'))
        # copias en linea (lightId 1: id + 2/3 / +6/7 respecto de las llamadas directas) frente a las llamadas falsas a 0x6E0A50 (no dibujan nada)
        t.append('head_calls_%d' % len(heads))
        inline = [e for e in coronas if e[1] % 4 in (2, 3) or e[1] in (6, 7)]
        if inline:
            t.append('inline_corona')
        t.append('static_tail')
    else:
        t.append('dynamic_path')
        if tails:
            t.append('dynamic_tail_calls_%d' % len(tails))
    if any(name(e) == 'head_light_reflection' for e in T):
        t.append('reflection')
    elif heads:
        t.append('no_reflection_%s' % ('train' if sub == 6 else 'dark'))
    lights_add = [e for e in T if name(e) == 'add_light']
    if lights_add:
        t.append('add_light_%d' % len(lights_add))
        t.append('fog_%d' % lights_add[0][12])
        if len(lights_add) == 2:
            t.append('brake_light')
    brk = f32(p['brake_bits'])
    if heads and not alarm_dark:
        t.append('brake_' + ('nan' if brk != brk else 'pos' if brk > 0 else 'zero' if brk == 0 else 'neg'))
        t.append('handbrake_%d' % p['handbrake'])
        t.append('driver_%d' % p['driver'])
        t.append('speed_' + kind(p['speed_bits'][0]) + '_' + kind(p['speed_bits'][1]))
    return t


def main():
    rows = [json.loads(l) for l in open(sys.argv[1], encoding='utf-8')]
    cnt = Counter()
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for t in tags(r['profile'], r['original']):
            cnt[t] += 1
    if '--json' in sys.argv:
        print(json.dumps(dict(cnt), indent=1, sort_keys=True))
    else:
        print(n, 'pares')
        for k in sorted(cnt):
            print('%-34s %6d' % (k, cnt[k]))


if __name__ == '__main__':
    main()
