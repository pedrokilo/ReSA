"""Oraculo independiente del banco Player82B: CPlayerPed::Compute3rdPersonMouseTarget (EXE 0x60B650).

Modelo escrito desde el desensamblado (0x60B650..0x60BA73), sin usar el codigo de la DLL: la secuencia de llamadas observadas a los colaboradores sustituidos (con sus argumentos), la
aritmetica x87 a 24 bits de la linea de trazado sin vector de camara (cada valor en la pila con mantisa de 24 bits y exponente ampliado; cada valor guardado, como float), los campos que escriben
los constructores de CEventGunAimedAt y CEventGroupEvent y el estado final (objetivo del raton, plazo, banderas de trazado). Todo NaN se canonicaliza a 0x7FC00000.
`model(inp)` devuelve el resultado esperado; `check(inp, side, begin)` compara un lado de un par con el modelo y devuelve un mensaje de error o None.
Etiquetas de punteros (Tag en Core.hpp): 0x10000000 + (indice de arena << 20) + desplazamiento respecto al inicio de la arena (la guarda es 0x40, el objeto empieza en +0x40).
Arenas: 0 jugador, 1 golpeado, 2 anterior, 3 otro, 4 inteligencia, 5..7 tareas, 8 grupo, 9..11 estructuras de arma, 12 matriz del jugador, 13 evento reservado (ranura del pool, sin guarda: el objeto empieza en +0).
"""
import math
import struct

NAN = 0x7FC00000
UNSTACKED_REF = 0xFFFFFFFE   # ranura de un objeto de la pila (no es una arena)


def tag(idx, off=0):
    return 0x10000000 + (idx << 20) + 0x40 + off


def tag_event(off=0):
    return 0x10000000 + (13 << 20) + off


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0] if not isinstance(x, int) else x


def f32(x):
    if x != x:
        return float('nan')
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(float('inf'), x)


def fb(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def canon(x):
    if isinstance(x, int):
        return NAN if (x & 0x7F800000) == 0x7F800000 and (x & 0x7FFFFF) else x
    if x != x:
        return NAN
    return struct.unpack('<I', struct.pack('<f', f32(x)))[0]


def r24(x):
    """redondeo a 24 bits de mantisa (al par) con exponente ampliado: un valor en la pila x87 con la precision a 24 bits"""
    if x != x or x in (0.0, float('inf'), float('-inf')):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0) / 16777216.0, e)


def sub_ext(a, b):
    if math.isinf(a) and math.isinf(b) and (a > 0) == (b > 0):
        return float('nan')
    return r24(a - b)


def add_ext(a, b):
    if math.isinf(a) and math.isinf(b) and (a > 0) != (b > 0):
        return float('nan')
    return r24(a + b)


def mul_ext(a, b):
    if (a == 0 and math.isinf(b)) or (b == 0 and math.isinf(a)):
        return float('nan')
    return r24(a * b)


def i8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def player_pos(inp):
    if inp['p_has_m']:
        return [fb(inp['p_m%d' % i]) for i in range(3)], tag(12, 0x30)
    return [fb(inp['p_pl%d' % i]) for i in range(3)], tag(0, 4)


def aim_line(origin, src, front, rng):
    """0x60B6FF..0x60B7FB: devuelve (fuente, objetivo, con_ajuste)"""
    px, py, pz = src
    qx, qy, qz = front
    dz = sub_ext(pz, origin[2])
    dy = sub_ext(py, origin[1])
    dx = sub_ext(px, origin[0])
    partial = add_ext(mul_ext(qy, dy), mul_ext(qz, dz))
    dot = add_ext(partial, mul_ext(dx, qx))
    adjusted = dot < 0.0
    if adjusted:
        qz_dot = mul_ext(qz, dot)
        qy_dot = mul_ext(qy, dot)
        qx_dot = mul_ext(dot, qx)
        px = f32(sub_ext(px, qx_dot))
        py = f32(sub_ext(py, qy_dot))
        pz = f32(sub_ext(pz, qz_dot))
    qxr = mul_ext(qx, rng)
    qyr = mul_ext(qy, rng)
    qzr = f32(mul_ext(qz, rng))
    tgt = [f32(add_ext(px, qxr)), f32(add_ext(qyr, py)), f32(add_ext(qzr, pz))]
    return [px, py, pz], tgt, adjusted


def model(inp):
    ev = []
    out = {'branches': set()}
    br = out['branches']
    PL = tag(0)
    old_ptr = {0: 0, 1: tag(1), 2: tag(2), 3: tag(3)}[inp['old_mode']]
    target_ptr, t7a0 = old_ptr, inp['f7a0']
    timer = inp['timer']
    tyres, dead = inp['car_tyres'] & 0xFF, inp['dead_peds'] & 0xFF
    bikers = (inp['car_tyres'] ^ inp['dead_peds'] ^ 0x5A) & 0xFF
    ignore = inp['ignore_old']
    event_written = False
    hit_ped = 0

    def finish():
        out.update(events=ev, target=target_ptr, t7a0=t7a0 & 0xFFFFFFFF, globals=[ignore, bikers, tyres, dead], event_written=event_written)
        return out

    if inp['use_mouse'] & 0xFF:
        br.add('mouse_on')
        ev.append([1, PL])
        ev.append([2, inp['w_type'], inp['skill_ret']])
        origin, pos_tag = player_pos(inp)
        rng = fb(inp['range0'])
        cam = inp['cam_idx']
        if inp['melee_arg'] & 0xFF:
            br.add('melee_path')
            ev.append([3, 1, canon(inp['range0']), canon(origin[0]), canon(origin[1]), canon(origin[2])])
            src = [fb(inp['f3_src%d' % i]) for i in range(3)]
            tgt = [fb(inp['f3_tgt%d' % i]) for i in range(3)]
        else:
            br.add('aim_path')
            csrc = [fb(inp['cam_src%d_%d' % (i, cam)]) for i in range(3)]
            cfront = [fb(inp['cam_front%d_%d' % (i, cam)]) for i in range(3)]
            src, tgt, adjusted = aim_line(origin, csrc, cfront, rng)
            br.add('aim_adjusted' if adjusted else 'aim_unadjusted')
            out['aim'] = (src, tgt)
        hit = 0
        losok = inp['los_ret'] & 0xFF
        if losok or inp['los_writes']:
            hit = {0: 0, 1: tag(1), 2: PL, 3: tag(3)}[inp['hit_mode']]
        ev.append([4] + [canon(v) for v in src] + [canon(v) for v in tgt] + [0, 0, 1, 0, 0, 0, 0, 0] + [PL, 1, tyres, dead, 0])
        if losok:
            br.add('los_true')
            if hit != PL:
                ev.append([6, hit])
                if inp['alive_ret'] & 0xFF:
                    hit_ped = hit
                    br.add('alive')
                else:
                    br.add('dead')
            else:
                br.add('hit_self')
        else:
            br.add('los_false')
        ev.append([5])
        ignore, bikers = PL, 1
        if hit_ped:
            br.add('hit_ped')
            if hit_ped != target_ptr:
                br.add('target_changed')
                if target_ptr:
                    ev.append([7, target_ptr, tag(0, 0x79C), target_ptr])
                    br.add('old_target_cleaned')
                target_ptr = hit_ped
                ev.append([8, hit_ped, tag(0, 0x79C), hit_ped])
            else:
                br.add('target_same')
            t7a0 = (timer + 1000) & 0xFFFFFFFF
            ev.append([2, inp['w_type'], 1])
            if inp['fire1'] == 0:
                br.add('fire_melee')
                return finish()
            ev.append([1, PL])
            ev.append([2, inp['w_type'], inp['skill_ret']])
            if not (inp['flags2'] & 1):
                br.add('flag_off')
                return finish()
            ev.append([9, tag(4), pos_tag] + [canon(v) for v in origin])
            if not (inp['see_ret'] & 0xFF):
                br.add('not_seen')
                return finish()
            for i in range(3):
                if (inp['task_mask'] >> i) & 1:
                    ev.append([17, tag(5 + i)])
                    br.add('task_present')
                    if inp['task_ret%d' % i] == 0x259:
                        br.add('task_react')
                        return finish()
                    break
            if inp['w_type'] != 0x17:
                ev.append([10, PL, 0xB0, 0, 0x3F800000, 0, 0, 0])
                br.add('say')
            else:
                br.add('no_say')
            ev.append([11, hit_ped])
            if inp['group_mode']:
                br.add('group')
                ev.append([12, hit_ped, PL])
                if inp['same_ret'] & 0xFF:
                    br.add('same_group')
                    return finish()
                ev.append([13, 0x18])
                event = 0 if inp['new_null'] else tag_event()
                if event:
                    ev.append([8, PL, tag_event(0x14), PL])
                    event_written = True
                else:
                    br.add('event_null')
                ev.append([8, hit_ped, UNSTACKED_REF, hit_ped])
                add = [15, tag(8, 0x30), 0, 0, hit_ped, event]
                if event:
                    add += [0, 0, 1, 0xC8, 0xFFFF, PL]
                ev.append(add)
                ev.append([7, hit_ped, UNSTACKED_REF, hit_ped])
                if event:
                    ev.append([7, PL, tag_event(0x14), PL])
            else:
                br.add('no_group')
                ev.append([8, PL, UNSTACKED_REF, PL])
                ev.append([16, tag(4, 0x68), 0, 0, 1, 0xC8, 0xFFFF, PL, 0])
                ev.append([7, PL, UNSTACKED_REF, PL])
            return finish()
    else:
        br.add('mouse_off')
    # 0x60BA3A: caducidad del objetivo anterior
    if target_ptr and (t7a0 & 0xFFFFFFFF) < timer:
        target_ptr = 0
        br.add('expired')
    elif target_ptr:
        br.add('kept')
    return finish()


def check(inp, side, begin):
    obs = side['observed']
    if any(obs['changes']):
        return 'cambiaron bytes de las arenas fuera de los esperados: %s' % [c[:4] for c in obs['changes'] if c]
    m = model(inp)
    if obs['events'] != m['events']:
        for i, (a, e) in enumerate(zip(obs['events'], m['events'])):
            if a != e:
                return 'evento %d: obtenido %s esperado %s' % (i, a, e)
        return 'numero de eventos distinto: %d frente a %d' % (len(obs['events']), len(m['events']))
    if obs['target'] != m['target']:
        return 'objetivo del raton: obtenido %s esperado %s' % (obs['target'], m['target'])
    if obs['t7a0'] != m['t7a0']:
        return 'plazo (+0x7A0): obtenido %s esperado %s' % (obs['t7a0'], m['t7a0'])
    if obs['globals'] != m['globals']:
        return 'globales de trazado: obtenido %s esperado %s' % (obs['globals'], m['globals'])
    return None
