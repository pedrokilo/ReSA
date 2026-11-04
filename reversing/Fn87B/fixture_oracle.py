"""Oraculo independiente (Python) de CPlayerPed::FindNextWeaponLockOnTarget (EXE 0x60E530..0x60EA7F, thiscall, `ret 8`, bool en AL) para el banco Fn87B.

Reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de colaboradores EN ORDEN con sus argumentos, bytes cambiados en las arenas y AL) y
lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Lectura del desensamblado (arg0 = objetivo actual, arg1 = dword de la pila; ESP0 = pila tras el prologo):
  tipo = arma activa ([jug+0x5A0 + (int8)[jug+0x718] * 0x1C]) leido ANTES de GetWeaponSkill(jug); GetWeaponInfo(tipo, habilidad); alcance = [info+4] (UNA sola lectura, tras volver de la llamada).
  GetATanOfXY(adelante.x, adelante.y) con [matriz+0x10], [matriz+0x14]; el resultado se descarta (fstp st(0)). mejor = nulo, prioridad = -10000.0 (0xC61C4000).
  Con arg0: GetATanOfXY(arg0.x - camara.x, arg0.y - camara.y) (x87 a 24 bits, guardado como float; posicion = [ent+0x14]+0x30 si hay matriz, si no ent+4; camara = TheCamera igual: matriz 0xB6F03C o
  m_placement 0xB6F02C); sin arg0: GetATanOfXY([0xB6F9AC], [0xB6F9B0]) con los dwords tal cual. El rumbo es lo que devuelve el falso (guardado como float).
  Peatones de atras adelante (pool 0xB74490, 0x7C4 bytes; hueco libre = bit 7 de su byte de estado): EBP = el puntero del hueco (0 si libre); se saltan nulo, el propio jugador, arg0, estados 0x36/0x37,
  en vehiculo (bit 8 de +0x46C) sin vehiculo o que no es moto (+0x590 != 9) ni tiene el bit 2 de +0x42D, nunca-objetivo (bit 28 de +0x470); AreInSameGroup(ped, yo) verdadero; tipo de peaton 0/1 con el
  byte 0x96A8B2; LOSBlockedBetweenPeds(yo, ped); CanIKReachThisTarget(posicion del ped, &arma activa, 1) falso. Luego TargetWeaponRangeMultiplier(ped, yo) * alcance (fmul + fstp) y
  EvaluateNeighbouringTarget(ped, &mejor, &prioridad, alcance ajustado, rumbo, arg1).
  Objetos (0xB7449C, 0x19C): CanBeTargetted, byte +0x140 & 0x40, objeto RW (+0x18), CanIKReachThisTarget, y el multiplicador se pide con EBP (el ULTIMO peaton recorrido: el hueco 0 del pool, o 0 si
  esta libre, o arg0 si el pool esta vacio), no con el objeto; EvaluateNeighbouringTarget(objeto, ...). Vehiculos (0xB74494, 0xA18) solo si IsCoopGameGoingOn: bit 0x20000000 de +0x40 y subtipo 10 (+0x594)
  se saltan; CanIKReachThisTarget; multiplicador con EBP; EvaluateNeighbouringTarget.
  Sin mejor: false. Con mejor de tipo peaton (byte +0x36 & 7 == 3) y GetWeaponInfo(tipo actual, 1)->[+0] != 0: GetPedsGroup(mejor); con grupo y AreInSameGroup(mejor, yo) falso: CEvent::operator new(0x18),
  CEventGunAimedAt(yo) (guarda yo en +0x14 con RegisterReference), CEventGroupEvent(mejor, evento) (RegisterReference), AddEvent(grupo+0x30), ~CEventGroupEvent (CleanUpOldReference), ~CEventGunAimedAt
  (CleanUpOldReference) y CEvent::operator delete (real: la ranura es del pool); sin grupo: CEventGunAimedAt local, CEventGroup::Add(inteligencia+0x68, &evento, false), ~CEventGunAimedAt.
  Fija el objetivo ([jug+0x71C]): CleanUpOldReference del anterior si lo hay, se guarda el mejor, RegisterReference, [datos+0x85] = 1; true.
Etiquetas: 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices (orden de TagAdd): 0 jugador, 1 sus datos, 2 su matriz, 3 objetivo fijado, 4 objetivo actual propio, 5 su matriz, 6 info del arma, 7 info 2,
8/9/10 cabecera, estados y peatones (0x7C4 cada uno), 11..15 sus matrices, 16/17/18 cabecera, estados y vehiculos (0xA18), 19..21 sus matrices, 22/23/24 cabecera, estados y objetos (0x19C), 25..27 sus matrices,
28 inteligencia, 29 grupo, 30 evento, 31 matriz de la camara. Un puntero local de la pila se etiqueta 0xDEADC0DE.
Tipos de evento de la traza: 1 GetWeaponSkill, 2 GetWeaponInfo, 3 GetATanOfXY, 4 multiplicador, 5 CanIKReach, 6 LOS, 7 AreInSameGroup, 8 CanBeTargetted, 9 IsCoopGameGoingOn, 10 EvaluateNeighbouringTarget,
11 GetPedsGroup, 12 CEvent::operator new, 13 AddEvent, 14 CEventGroup::Add, 30 RegisterReference, 31 CleanUpOldReference.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import math
import struct
import sys
from fractions import Fraction

GUARD = 0x40
ANY = None
NANB = 0xFFC00000
STACK = 0xDEADC0DE

T_PL, T_DATA, T_PLMAT, T_TGT, T_CUR, T_CURMAT, T_INFO, T_INFO2, T_PEDHDR, T_PEDFLAGS, T_PEDS = range(11)
T_VEHS, T_OBJS, T_INTEL, T_GRP, T_EV = 18, 24, 28, 29, 30
PED_STRIDE, VEH_STRIDE, OBJ_STRIDE = 0x7C4, 0xA18, 0x19C
FLT_MAX = Fraction((2 ** 24 - 1) * 2 ** 104)


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


def opt(v):
    return ('opt', v)


def dword_changes(out, base, old, new):
    for b in range(4):
        o, n = (old >> (8 * b)) & 0xFF, (new >> (8 * b)) & 0xFF
        if o != n:
            out[base + b] = n


def sext8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


# ---- aritmetica x87 a 24 bits (exponente extendido) guardada como float de 32 bits, exacta con fracciones
def fv(bits_):
    return struct.unpack('<f', struct.pack('<I', bits_ & 0xFFFFFFFF))[0]


def round_sig(fr, nbits=24):
    if fr == 0:
        return fr
    sign = -1 if fr < 0 else 1
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length() - nbits
    while a / Fraction(2) ** e >= 2 ** nbits:
        e += 1
    while a / Fraction(2) ** e < 2 ** (nbits - 1):
        e -= 1
    q = a / Fraction(2) ** e
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return sign * n * Fraction(2) ** e


def f32_bits(fr):
    """Fraction -> bits de float (redondeo al mas cercano; desbordamiento -> infinito; subnormales a la rejilla 2^-149). fr != 0"""
    s = 0x80000000 if fr < 0 else 0
    a = abs(fr)
    if a > FLT_MAX:
        return s | 0x7F800000
    if a < Fraction(1, 2 ** 126):
        q = a * 2 ** 149
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
            n += 1
        return s | n
    return s | struct.unpack('<I', struct.pack('<f', float(a)))[0]


def classify(bits_):
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    return 'zero' if (bits_ & 0x7FFFFFFF) == 0 else 'fin'


def sub24(a, b):
    ca, cb = classify(a), classify(b)
    if ca == 'nan' or cb == 'nan':
        return NANB
    sa, sb = a >> 31, b >> 31
    if ca == 'inf' and cb == 'inf':
        return NANB if sa == sb else a
    if ca == 'inf':
        return a
    if cb == 'inf':
        return b ^ 0x80000000
    d = Fraction(fv(a)) - Fraction(fv(b))
    if d == 0:
        return 0x80000000 if (ca == 'zero' and cb == 'zero' and sa == 1 and sb == 0) else 0
    return f32_bits(round_sig(d))


def mul24(a, b):
    ca, cb = classify(a), classify(b)
    if ca == 'nan' or cb == 'nan':
        return NANB
    s = (a ^ b) & 0x80000000
    if ca == 'inf' or cb == 'inf':
        return NANB if (ca == 'zero' or cb == 'zero') else s | 0x7F800000
    if ca == 'zero' or cb == 'zero':
        return s
    return f32_bits(round_sig(Fraction(fv(a)) * Fraction(fv(b))))


def model(inp, stats=None):
    ev = []

    def S(name):
        if stats is not None:
            stats[name] = stats.get(name, 0) + 1

    def E(*a):
        ev.append(list(a))
    PL = tag(T_PL, 0)
    TGT = tag(T_TGT, 0)
    slotref = tag(T_PL, 0x71C)
    st = dict(slot=sext8(inp['slot']), target=1 if inp['tgt'] else 0, range=inp['rng'], done=set())
    types = {st['slot']: inp['wtype']}
    written = {}
    cnt = dict(atan=0, mult=0, ev=0, reach=0, winfo=0)
    flags = dict(set_target=False, target_changed=False)
    best = dict(tag=None, kind=None, idx=None, prio=0xC61C4000)
    arg1 = inp['arg1'] & 0xFF

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

    def wtag():
        return tag(T_PL, 0x5A0 + st['slot'] * 0x1C)

    def result(ret, event_built=False):
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
        data = {0x85: opt(1)} if flags['set_target'] else {}
        info = {}
        if inp['mut_kind'] & 4 and inp['mut_site'] in st['done']:
            dword_changes(info, 4, inp['rng'], inp['mut_range'])
        evc = {}   # el evento reservado es una ranura real del pool (no hay arena que comparar): sus campos se observan en AddEvent
        # arenas: jugador, datos, objetivo fijado, objetivo actual, info, info 2, inteligencia, grupo, evento, peatones, vehiculos, objetos
        return ev, [pl, data, {}, {}, info, {}, {}, {}, evc, {}, {}, {}], ret

    def evaluate(et, kind, idx, adj):
        E(10, PL, et, best['tag'] if best['tag'] is not None else 0, best['prio'], adj, heading_bits[0], arg1)
        k = cnt['ev'] % 12
        cnt['ev'] += 1
        mutate(10)
        if inp['eval_set%d' % k]:
            best['tag'], best['kind'], best['idx'] = et, kind, idx
            best['prio'] = inp['eval_prio%d' % k]

    heading_bits = [0]
    type0 = curtype()
    E(1, PL); mutate(1)
    E(2, type0, inp['skill']); mutate(2)
    cnt['winfo'] = 1
    range0 = st['range']
    # GetATanOfXY(adelante) descartado
    E(3, inp['fx'], inp['fy']); mutate(3)
    cnt['atan'] = 1
    cur = inp['cur']
    cur_tag = 0 if cur == 0 else (tag(T_CUR, 0) if cur == 1 else tag(T_PEDS, (cur - 2) * PED_STRIDE))
    if cur:
        if cur == 1:
            cx, cy = inp['cx'], inp['cy']
        else:
            cx, cy = inp['ped%d_x' % (cur - 2)], inp['ped%d_y' % (cur - 2)]
        E(3, sub24(cx, inp['cam_x']), sub24(cy, inp['cam_y'])); mutate(3)
    else:
        E(3, inp['cam9ac'], inp['cam9b0']); mutate(3)
    heading_bits[0] = inp['atan%d' % (cnt['atan'] % 12)]
    cnt['atan'] += 1
    last = cur_tag
    # ---- peatones
    for slot in range(inp['ped_size'], 0, -1):
        i = slot - 1
        pr = inp['ped%d_pr' % i]
        pt = tag(T_PEDS, i * PED_STRIDE)
        last = pt if pr else 0
        if not pr:
            S('ped_skip_free')
            continue
        if pt == cur_tag:
            S('ped_skip_current')
            continue
        if inp['ped%d_a' % i] in (0x36, 0x37):
            S('ped_skip_state')
            continue
        if inp['ped%d_b' % i]:
            c = inp['ped%d_c' % i]
            if not 1 <= c <= 3:
                S('ped_skip_veh_none')
                continue
            v = c - 1
            if inp['veh%d_c' % v] != 9 and not inp['veh%d_d' % v] & 4:
                S('ped_skip_veh_notbike')
                continue
            S('ped_veh_bike_ok' if inp['veh%d_c' % v] == 9 else 'ped_veh_flag4_ok')
        if inp['ped%d_d' % i]:
            S('ped_skip_never')
            continue
        E(7, pt, PL); mutate(7)
        if inp['ped%d_f' % i] & 1:
            S('ped_skip_samegroup')
            continue
        if inp['ped%d_e' % i] in (0, 1) and inp['two_player'] & 0xFF:
            S('ped_skip_twoplayer')
            continue
        E(6, PL, pt); mutate(6)
        if inp['ped%d_g' % i] & 1:
            S('ped_skip_los')
            continue
        E(5, inp['ped%d_x' % i], inp['ped%d_y' % i], inp['ped%d_z' % i], wtag(), 1); mutate(5)
        k = cnt['reach'] % 12
        cnt['reach'] += 1
        if not inp['reach%d' % k] & 1:
            S('ped_skip_reach')
            continue
        E(4, pt, PL); mutate(4)
        S('ped_evaluated')
        mult = inp['mult%d' % (cnt['mult'] % 12)]
        cnt['mult'] += 1
        evaluate(pt, 'ped', i, mul24(mult, range0))
    # ---- objetos
    for slot in range(inp['obj_size'], 0, -1):
        i = slot - 1
        if not inp['obj%d_pr' % i]:
            S('obj_skip_free')
            continue
        ot = tag(T_OBJS, i * OBJ_STRIDE)
        E(8, ot); mutate(8)
        if not inp['obj%d_a' % i] & 1:
            S('obj_skip_notarget')
            continue
        if inp['obj%d_b' % i] & 0x40:
            S('obj_skip_flag40')
            continue
        if not inp['obj%d_c' % i]:
            S('obj_skip_norw')
            continue
        E(5, inp['obj%d_x' % i], inp['obj%d_y' % i], inp['obj%d_z' % i], wtag(), 1); mutate(5)
        k = cnt['reach'] % 12
        cnt['reach'] += 1
        if not inp['reach%d' % k] & 1:
            S('obj_skip_reach')
            continue
        E(4, last, PL); mutate(4)
        S('obj_evaluated')
        mult = inp['mult%d' % (cnt['mult'] % 12)]
        cnt['mult'] += 1
        evaluate(ot, 'obj', i, mul24(mult, range0))
    # ---- vehiculos
    E(9); mutate(9)
    if inp['coop'] & 1:
        for slot in range(inp['veh_size'], 0, -1):
            i = slot - 1
            if not inp['veh%d_pr' % i]:
                S('veh_skip_free')
                continue
            if inp['veh%d_a' % i] & 0x20000000:
                S('veh_skip_phys')
                continue
            if inp['veh%d_b' % i] == 10:
                S('veh_skip_bmx')
                continue
            E(5, inp['veh%d_x' % i], inp['veh%d_y' % i], inp['veh%d_z' % i], wtag(), 1); mutate(5)
            k = cnt['reach'] % 12
            cnt['reach'] += 1
            if not inp['reach%d' % k] & 1:
                S('veh_skip_reach')
                continue
            E(4, last, PL); mutate(4)
            S('veh_evaluated')
            mult = inp['mult%d' % (cnt['mult'] % 12)]
            cnt['mult'] += 1
            evaluate(tag(T_VEHS, i * VEH_STRIDE), 'veh', i, mul24(mult, range0))
    if best['tag'] is None:
        S('no_best')
        return result(0)
    bt = best['tag']
    kind, idx = best['kind'], best['idx']
    event_built = False
    if inp['%s%d_t' % (kind, idx)] & 7 == 3:
        S('best_type_ped')
        E(2, curtype(), 1); mutate(2)
        if inp['fire'] != 0:
            E(11, bt); mutate(11)
            if inp['has_group']:
                E(7, bt, PL); mutate(7)
                if not inp['same_after'] & 1:
                    S('event_group_built')
                    E(12, 0x18); mutate(12)
                    E(30, PL, tag(T_EV, 0x14), PL)
                    E(30, bt, STACK, bt)
                    E(13, tag(T_GRP, 0x30), 47, bt, tag(T_EV, 0), 31, 0xC8, 0, 1 << 8, 0xFFFF, PL); mutate(13)
                    E(31, bt, STACK, bt)
                    E(31, PL, tag(T_EV, 0x14), PL)
                    event_built = True
                else:
                    S('event_group_same')
            else:
                S('event_stack_built')
                E(30, PL, STACK, PL)
                E(14, tag(T_INTEL, 0x68), 31, 0, 0, 1 << 8, 0xC8, 0xFFFF, PL); mutate(14)
                E(31, PL, STACK, PL)
    if st['target']:
        E(31, TGT, slotref, TGT)
    flags['set_target'] = True
    st['target'] = 2
    E(30, bt, slotref, bt)
    return result(1, event_built)


def compare_changes(changes, ranges, observed):
    if len(changes) != len(observed):
        return 'numero de arenas'
    for i, (exp, g) in enumerate(zip(changes, observed)):
        exp = dict(exp)
        got = {off: val for off, val in g}
        lo, hi = ranges.get(i, (0, 0))
        for off in list(got):
            if lo <= off < hi:
                got.pop(off)
        for off in list(exp):
            v = exp[off]
            if v is ANY:
                exp.pop(off)
                got.pop(off, None)
            elif isinstance(v, tuple):
                exp.pop(off)
                if off in got:
                    if got[off] != v[1]:
                        return 'cambios de la arena %d: byte %#x: esperado opcional %#x, observado %#x' % (i, off, v[1], got[off])
                    got.pop(off)
        if got != exp:
            return 'cambios de la arena %d: esperado %s, observado %s' % (i, {hex(k): v for k, v in exp.items()}, {hex(k): v for k, v in got.items()})
    return None


def expected(inp):
    return model(inp)


def check(fn, inp, side_obs, begin):
    obs = side_obs['observed']
    ev, changes, ret = model(inp)
    got = obs['events']
    if len(ev) != len(got) or any(x != y for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if x != y:
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    if obs['ret_al'] != ret:
        return 'retorno AL: esperado %d, observado %d' % (ret, obs['ret_al'])
    return compare_changes(changes, {}, obs['changes'])


def run(path):
    rows = [json.loads(line) for line in open(path, encoding='utf-8')]
    begin = [r for r in rows if r['event'] == 'begin'][0]
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['function'], r['input'], r[sname], begin)
            if msg:
                bad += 1
                if bad <= 10:
                    print('caso %d (familia %d) lado %s: %s' % (r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
