"""Oraculo independiente (Python) de CPlayerPed::FindWeaponLockOnTarget (EXE 0x60DC50..0x60E52D, thiscall sin argumentos, bool en AL) para el banco PlayerPed79.

Lectura del desensamblado (sin usar nada del C++ de produccion):
  tipo = arma activa ([jug+0x5A0 + (int8)[jug+0x718] * 0x1C]) leido ANTES de GetWeaponSkill; GetWeaponInfo(tipo, habilidad); objetivo = [jug+0x71C] y alcance0 = [info+4] leidos DESPUES.
  Con objetivo: dx = obj.x - yo.x, dy = obj.y - yo.y (x87, 24 bits, exponente extendido), d = sqrt(dy*dy + dx*dx) guardado como float; alcance1 = [info+4] (antes de la llamada); si
  !(TargetWeaponRangeMultiplier(obj, yo) * alcance1 >= d): CleanUpOldReference del objetivo releido ([jug+0x71C]), objetivo = 0, devuelve false; si no devuelve true.
  Sin objetivo: rumbo = GetATanOfXY(adelante.x, adelante.y); pad = GetPad(tipo de peaton); stick fuera de la zona muerta si |LR| > 60 o (si no) |UD| > 60 (movsx de 16 bits); si lo esta:
  UD, LR otra vez, rumbo = LimitRadianAngle(float((GetRadianAngleBetweenPoints(0, 0, -LR, UD) - [0xB6F178]) + pi/2)). Bote de spray (tipo 41 releido): centro = adelante * 8 + posicion,
  FindObjectsInRange(centro, 8.0, 0, &n, 15, lista, 1, 0, 0, 0, 0); por etiqueta: IsTag, GetAlpha < 255, d2 = (dx*dx + dz*dz) + dy*dy < alcance0^2, angulo = fpatan(-dx, dy) - rumbo_spray con una
  sola correccion de +-2pi; se queda con el menor (o el primero si NaN); LOSBlockedBetweenPeds(yo, ella) false -> se fija como objetivo (CleanUp del anterior, Register, [datos+0x85] = 1) y true.
  Peatones de atras adelante: se saltan estados 0x36/0x37, en vehiculo (solo moto o byte +0x42D & 4), nunca-objetivo, tipo 0/1 con el byte de dos jugadores, AreInSameGroup(ped, yo);
  angulo1 = |norm(GetATanOfXY(dx, dy) - rumbo)| < 140 grados; punto 3 m por detras (Normalise del adelante, * 3, yo - eso); angulo2 < 90 grados; d = Magnitude(ped - yo) < Multiplier * alcance (leido antes
  de la llamada) -> EvaluateTarget(ped, mejor, prioridad, alcance, rumbo, 0). Objetos: CanBeTargetted, byte +0x140 & 0x40, objeto RW, CanIKReachThisTarget -> EvaluateTarget(..., 1). Vehiculos solo en
  cooperativo (IsCoopGameGoingOn): bit 0x20000000 de +0x40 y subtipo 10 se saltan. Al final, con mejor: fijar objetivo y true; si no, false.
Etiquetas: 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 datos, 2 matriz del jugador, 3 objetivo fijado, 4 su matriz, 5 info, 6/7 cabecera y estados del pool de peatones, 8 peatones
(0x7C4 bytes cada uno), 9..13 matrices de peatones, 14/15 cabecera y estados de vehiculos, 16 vehiculos (0xA18), 17..19 sus matrices, 20/21 objetos, 22 objetos (0x19C), 23..25 sus matrices, 26 etiquetas (0x40), 27..30 sus matrices.
"""
import math
import struct

GUARD = 0x40
ANY = None
NANB = 0xFFC00000
PI_F = 3.1415927410125732
TWOPI_F = 6.2831854820251465
HALFPI_F = 1.5707963705062866


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


def opt(v):
    return ('opt', v)


def dword_changes(out, base, old, new):
    for b in range(4):
        o, n = (old >> (8 * b)) & 0xFF, (new >> (8 * b)) & 0xFF
        if o != n:
            out[base + b] = n


def fb(bits_):
    return struct.unpack('<f', struct.pack('<I', bits_ & 0xFFFFFFFF))[0]


def f32(x):
    """redondeo a float de 32 bits (desbordamiento -> infinito)"""
    if x != x:
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.inf if x > 0 else -math.inf


def bits(x):
    x = f32(x)
    if x != x:
        return NANB
    return struct.unpack('<I', struct.pack('<f', x))[0]


def r24(x):
    """redondeo a 24 bits de mantisa con exponente sin limite (x87 con control de precision de 24 bits)"""
    if x != x or x in (math.inf, -math.inf) or x == 0:
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * (1 << 24)), e - 24)


def r24sqrt(x):
    if x != x or x < 0:
        return math.nan
    if x == math.inf:
        return x
    return r24(math.sqrt(x))


def sext8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def sext16(v):
    v &= 0xFFFF
    return v - 65536 if v & 0x8000 else v


def model1(inp):
    ev = []

    def E(*a):
        ev.append(list(a))
    PL = tag(0, 0)
    TGT = tag(3, 0)
    slotref = tag(0, 0x71C)
    st = dict(slot=sext8(inp['slot']), target=1 if inp['tgt'] else 0, range=inp['rng'], done=set())
    types = {st['slot']: inp['wtype']}
    written = {}
    cnt = dict(atan=0, mult=0, mag=0, ev=0, reach=0)
    flags = dict(set_target=False, target_changed=False)
    best = dict(ent=None, prio=0xC61C4000)
    heading_bits = [0]

    def mutate(site):
        if inp['mut_site'] != site or site in st['done']:
            return
        st['done'].add(site)
        k = inp['mut_kind']
        if k & 1:
            st['target'] = 0 if st['target'] else 1
            flags['target_changed'] = True
        if k & 2:
            st['slot'] = sext8(inp['mut_slot'])
            types[st['slot']] = inp['mut_wtype']
            written[0x5A0 + st['slot'] * 0x1C] = inp['mut_wtype']
        if k & 4:
            st['range'] = inp['mut_range']
        if k & 8:
            types[st['slot']] = inp['mut_wtype']
            written[0x5A0 + st['slot'] * 0x1C] = inp['mut_wtype']

    def curtype():
        return types.get(st['slot'])

    def result(ret):
        pl = {}
        if flags['set_target'] or flags['target_changed'] or (not st['target']) != (not inp['tgt']):
            for i in range(4):
                pl[0x71C + i] = ANY
        if inp['mut_kind'] & 2 and inp['mut_site'] in st['done']:
            old, new = inp['slot'] & 0xFF, inp['mut_slot'] & 0xFF
            if old != new:
                pl[0x718] = new
        for addr, val in written.items():
            init = inp['wtype'] if addr == 0x5A0 + sext8(inp['slot']) * 0x1C else None
            for b in range(4):
                nb = (val >> (8 * b)) & 0xFF
                if init is not None:
                    if ((init >> (8 * b)) & 0xFF) != nb:
                        pl[addr + b] = nb
                else:
                    pl[addr + b] = opt(nb)
        data = {}
        if flags['set_target']:
            data[0x85] = opt(1)
        info = {}
        if inp['mut_kind'] & 4 and inp['mut_site'] in st['done']:
            dword_changes(info, 4, inp['rng'], inp['mut_range'])
        return ev, [pl, data, {}, info], {}, ret

    def set_target(entity_tag):
        if st['target']:
            E(31, TGT, slotref, TGT)
        flags['set_target'] = True
        st['target'] = 2
        E(30, entity_tag, slotref, entity_tag)

    ppos = [fb(inp['px']), fb(inp['py']), fb(inp['pz'])]
    tpos = [fb(inp['tx']), fb(inp['ty']), fb(inp['tz'])]
    type0 = curtype()
    E(1, PL); mutate(1)
    E(2, type0, inp['skill']); mutate(2)
    target = st['target']
    range0 = st['range']
    if target:
        dx = r24(tpos[0] - ppos[0])
        dy = r24(tpos[1] - ppos[1])
        dist = f32(r24sqrt(r24(r24(dy * dy) + r24(dx * dx))))
        range1 = st['range']
        E(3, TGT, PL); mutate(3)
        m = fb(inp['mult0'])
        prod = r24(m * fb(range1))
        if prod >= dist:
            return result(1)
        if st['target']:
            E(31, TGT, slotref, TGT)
        st['target'] = 0
        return result(0)
    # ---- rumbo
    E(4, inp['fx'], inp['fy']); mutate(4)
    heading = fb(inp['atan0'])
    cnt['atan'] = 1
    pad = inp['ptype']
    E(5, pad); mutate(5)
    udn = [0]

    def ud_value():   # el falso devuelve ud1 en la 1a llamada y ud2 en las siguientes
        udn[0] += 1
        return inp['ud1'] if udn[0] == 1 else inp['ud2']
    stick = abs(sext16(inp['lr1'])) > 60.0
    if not stick:
        E(6, pad); mutate(5)
        stick = abs(sext16(ud_value())) > 60.0
    if stick:
        E(6, pad); mutate(5)
        ud = sext16(ud_value())
        E(5, pad); mutate(5)
        lr2 = sext16(inp['lr2'])
        E(7, 0, 0, bits(float(-lr2)), bits(float(ud))); mutate(6)
        adjusted = r24(r24(fb(inp['between']) - fb(inp['cam'])) + HALFPI_F)
        E(8, bits(adjusted)); mutate(7)
        heading = fb(inp['limit_r'])
    heading_bits[0] = bits(heading)
    # ---- bote de spray
    if curtype() == 41:
        fx, fy, fz = fb(inp['fx']), fb(inp['fy']), fb(inp['fz'])
        sprayh = f32(math.atan2(-fx, fy))
        vx = f32(r24(fx * 8.0))
        vy = f32(r24(fy * 8.0))
        vz = r24(fz * 8.0)
        cx = f32(r24(vx + ppos[0]))
        cy = f32(r24(vy + ppos[1]))
        cz = f32(r24(vz + ppos[2]))
        E(16, bits(cx), bits(cy), bits(cz), 0x41000000, 0, 15, 1, 0, 0, 0, 0); mutate(14)
        best_angle = PI_F
        chosen = None
        for i in range(inp['bld_count']):
            mi = inp['bld_list%d' % i] % 4
            et = tag(26, mi * 0x40)
            E(14, et); mutate(13)
            if not inp['bld%d_a' % mi] & 1:
                continue
            E(15, et); mutate(13)
            if inp['bld%d_b' % mi] & 0xFF >= 255:
                continue
            e = [fb(inp['bld%d_x' % mi]), fb(inp['bld%d_y' % mi]), fb(inp['bld%d_z' % mi])]
            dx = r24(e[0] - ppos[0])
            dy = r24(e[1] - ppos[1])
            dz = r24(e[2] - ppos[2])
            d2 = r24(r24(r24(dx * dx) + r24(dz * dz)) + r24(dy * dy))
            rf = fb(range0)
            r2 = r24(rf * rf)
            if not r2 > d2:
                continue
            a = r24(math.atan2(-dx, dy) - sprayh)
            if a < -PI_F:
                a = r24(a + TWOPI_F)
            elif a > PI_F:
                a = r24(a - TWOPI_F)
            if a < best_angle or chosen is None:
                best_angle = f32(a)
                chosen = mi
        if chosen is not None:
            ct = tag(26, chosen * 0x40)
            E(12, PL, ct); mutate(11)
            if not inp['los'] & 1:
                set_target(ct)
                return result(1)
    # ---- pools
    K = fb(0x3C0EFA35)
    view = f32(r24(140.0 * K))
    view2 = f32(r24(90.0 * K))

    def limit(a):
        while a > PI_F:
            a = f32(a - TWOPI_F)
        while a < -PI_F:
            a = f32(a + TWOPI_F)
        return -a if a < 0 else a

    def evaluate(et, rng_bits, arg5):
        E(11, PL, et, best['ent'] if best['ent'] is not None else 0, best['prio'], rng_bits, heading_bits[0], arg5)
        k = cnt['ev'] % 12
        cnt['ev'] += 1
        mutate(10)
        if inp['eval_set%d' % k]:
            best['ent'] = et
            best['prio'] = inp['eval_prio%d' % k]

    for slot in range(inp['ped_size'], 0, -1):
        i = slot - 1
        if not inp['ped%d_pr' % i]:
            continue
        if inp['ped%d_a' % i] in (0x36, 0x37):
            continue
        if inp['ped%d_b' % i]:
            c = inp['ped%d_c' % i]
            if not 1 <= c <= 3:
                continue
            v = c - 1
            if inp['veh%d_c' % v] != 9 and not inp['veh%d_d' % v] & 4:
                continue
        if inp['ped%d_d' % i]:
            continue
        if inp['ped%d_e' % i] in (0, 1) and inp['two_player'] & 0xFF:
            continue
        pt = tag(8, i * 0x7C4)
        E(13, pt, PL); mutate(12)
        if inp['ped%d_f' % i] & 1:
            continue
        pp = [fb(inp['ped%d_x' % i]), fb(inp['ped%d_y' % i]), fb(inp['ped%d_z' % i])]
        dy = f32(r24(pp[1] - ppos[1]))
        dx = f32(r24(pp[0] - ppos[0]))
        E(4, bits(dx), bits(dy)); mutate(4)
        at = fb(inp['atan%d' % (cnt['atan'] % 12)]); cnt['atan'] += 1
        angle = limit(f32(r24(at - heading)))
        if not angle < view:
            continue
        E(9, inp['fx'], inp['fy'], inp['fz']); mutate(8)
        nv = [fb(inp['norm0']), fb(inp['norm1']), fb(inp['norm2'])]
        sc = [f32(r24(v * 3.0)) for v in nv]
        behind = [f32(r24(ppos[k] - sc[k])) for k in range(3)]
        dy2 = f32(r24(pp[1] - behind[1]))
        dx2 = f32(r24(pp[0] - behind[0]))
        E(4, bits(dx2), bits(dy2)); mutate(4)
        at2 = fb(inp['atan%d' % (cnt['atan'] % 12)]); cnt['atan'] += 1
        angle2 = limit(f32(r24(at2 - heading)))
        if not angle2 < view2:
            continue
        to = [f32(r24(pp[k] - ppos[k])) for k in range(3)]
        E(10, bits(to[0]), bits(to[1]), bits(to[2])); mutate(9)
        dist = fb(inp['mag%d' % (cnt['mag'] % 12)]); cnt['mag'] += 1
        range2 = st['range']
        E(3, pt, PL); mutate(3)
        mult = fb(inp['mult%d' % (cnt['mult'] % 12)]); cnt['mult'] += 1
        adj = f32(r24(mult * fb(range2)))
        if not dist < adj:
            continue
        evaluate(pt, bits(adj), 0)
    for slot in range(inp['obj_size'], 0, -1):
        i = slot - 1
        if not inp['obj%d_pr' % i]:
            continue
        ot = tag(22, i * 0x19C)
        E(17, ot); mutate(15)
        if not inp['obj%d_a' % i] & 1:
            continue
        if inp['obj%d_b' % i] & 0x40:
            continue
        if not inp['obj%d_c' % i]:
            continue
        E(18, inp['obj%d_x' % i], inp['obj%d_y' % i], inp['obj%d_z' % i], tag(0, 0x5A0 + st['slot'] * 0x1C), 1); mutate(16)
        k = cnt['reach'] % 12
        cnt['reach'] += 1
        if not inp['reach%d' % k] & 1:
            continue
        evaluate(ot, st['range'], 1)
    E(19); mutate(17)
    if inp['coop'] & 1:
        for slot in range(inp['veh_size'], 0, -1):
            i = slot - 1
            if not inp['veh%d_pr' % i]:
                continue
            if inp['veh%d_a' % i] & 0x20000000:
                continue
            if inp['veh%d_b' % i] == 10:
                continue
            E(18, inp['veh%d_x' % i], inp['veh%d_y' % i], inp['veh%d_z' % i], tag(0, 0x5A0 + st['slot'] * 0x1C), 1); mutate(16)
            k = cnt['reach'] % 12
            cnt['reach'] += 1
            if not inp['reach%d' % k] & 1:
                continue
            evaluate(tag(16, i * 0xA18), st['range'], 1)
    if best['ent'] is None:
        return result(0)
    set_target(best['ent'])
    return result(1)


def check1(inp, obs, compare_changes):
    ev, changes, ranges, ret = model1(inp)
    got = obs['events']
    if len(ev) != len(got) or any(x != y for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if x != y:
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    if obs['ret_al'] != ret:
        return 'retorno AL: esperado %d, observado %d' % (ret, obs['ret_al'])
    return compare_changes(changes, ranges, obs['changes'])
