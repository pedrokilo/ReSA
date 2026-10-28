"""Oraculo independiente (Python) de PlayerArea80B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x60C1E0..0x60C510, lo que debe observarse de
CPlayerPed::KeepAreaAroundPlayerClear (traza de colaboradores EN ORDEN con sus argumentos y bytes cambiados en las arenas) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Lectura del desensamblado (thiscall sin argumentos, ECX = jugador, `ret`, void; SEH propio):
  1. La inteligencia [jug+0x47C] se lee UNA vez ([esp+14h]); 16 vueltas: peaton = [inteligencia_guardada + 0x130 + i*4] (se relee cada vuelta); nulo -> siguiente.
     [ped+0x484] (byte) != 1 -> siguiente; [ped+0x46C] & 0x100 -> siguiente; IsAlive(ped) (0x5E0170) falso -> siguiente; IsMember(0xC09928, ped) (0x5F6A10) verdadero -> siguiente;
     GetIsOnScreen(ped) (0x534540) falso, o (relee) [ped+0x478] & 0x10 -> virtual [ped+0x54] (FlagToDestroyWhenNextProcessed, ranura 21) y siguiente.
     FindTaskByType([ped+0x47C], 0x38F) (0x600EE0): tarea con [tarea+0xC] == jugador -> siguiente. GetEventOfType([ped+0x47C] + 0x68, 0x20) (0x4AB650, se relee la inteligencia):
     evento con [evento+0x10] (tarea) no nula cuyo virtual [tarea+0x10] (GetTaskType) == 0x38F -> siguiente.
     CTask::operator new(0x40) (0x61A5A0); constructor 0x65C430 (jugador, 0, 1000.0f, 100000, [0x86F678], [0xC18CF0]); [tarea+0x30] = 4; evento local CEventScriptCommand(3, tarea, 0)
     (0x4B0A00); Add([ped+0x47C] + 0x68, evento, 0) (0x4AB420; la inteligencia se relee); destructor del evento (0x4B0A50) = destructor de la tarea (CleanUpOldReference) y CTask::operator delete.
  2. Referencia: con [jug+0x46C] & 0x100 y [jug+0x58C] != 0 la posicion del vehiculo ([veh+0x14] ? matriz+0x30 : veh+4), si no la del jugador; copia de tres dwords.
     Centro: SIEMPRE la posicion del jugador (releida). FindObjectsInRange(centro, 15.0f, 1, &cuenta, 6, lista, 0, 1, 0, 0, 0) (0x564A20, cdecl).
  3. Para i < cuenta (int16 con signo, la lista se relee cada vuelta): vehiculo = lista[i]; byte [veh+0x4A4] == 2 -> siguiente; estado = byte [veh+0x36] >> 3 en {0, 9} -> siguiente;
     matriz = [veh+0x14] (una vez); posicion = matriz ? matriz+0x30 : veh+4. ((dz*dz + dy*dy) + dx*dx) > [0x858FE8] (25.0f), con d = pos_veh - ref (x87 a 24 bits): [veh+0x3BB] = 1 (espera) y
     [veh+0x3BC] = [0xB7CB84] + 5000. Si no: ((ref.y - veh.y) * [matriz+0x14] + (ref.x - veh.x) * [matriz+0x10]) > [0x858B50] (0.0f): [veh+0x3BB] = 3, si no 9; tiempo + 2000
     (con la matriz nula el EXE se cae). Despues 0x424F80(veh) (PossiblyRemoveVehicle).
  Con NaN en cualquier operando la comparacion ordenada (fcomp + test ah,41h) da falso.
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 su matriz, 2 su vehiculo, 3 matriz de su vehiculo, 4 inteligencia,
5 inteligencia alterna, 6/7 inteligencias de peaton, 8..11 peatones, 12 tarea hallada, 13 evento pendiente, 14 tarea pendiente, 15 tarea de huida, 16..18 vehiculos, 19..21 sus matrices,
22 CPedGroups (sin guarda: 0x10000000 + (22 << 20) + desplazamiento).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
ANY = None
IGNORE = 'ignore'
PLAYER, PMAT, PVEH, PVMAT, INTEL, INTELALT, PI0, PI1, PED0 = range(9)
FTASK, PEV, PTASK, FLEE, VEH0, VM0, NARENAS = 12, 13, 14, 15, 16, 19, 22
GROUPS = 0x10000000 + (22 << 20) + 8


def tag(a, off=0):
    return 0x10000000 + (a << 20) + GUARD + off


# ------------------------------------------------------------------------------------------------ aritmetica x87 a 24 bits (exponente sin limite)
def f32(bits):
    bits &= 0xFFFFFFFF
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return ('n',) if m else ('i', -1 if bits >> 31 else 1)
    sign = -1 if bits >> 31 else 1
    if e == 0:
        return ('f', sign * Fraction(m, 1 << 149))
    return ('f', sign * Fraction((1 << 23) | m, 1) * Fraction(2) ** (e - 150))


def round24(fr):
    if fr == 0:
        return fr
    sign = -1 if fr < 0 else 1
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = a / ulp
    fl = q.numerator // q.denominator
    rem = q - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    return sign * fl * ulp


NAN = ('n',)


def neg(a):
    if a[0] == 'n':
        return a
    if a[0] == 'i':
        return ('i', -a[1])
    return ('f', -a[1])


def add(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    return ('f', round24(a[1] + b[1]))


def sub(a, b):
    return add(a, neg(b))


def mul(a, b):
    if a[0] == 'n' or b[0] == 'n':
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        o = b if a[0] == 'i' else a
        i = a if a[0] == 'i' else b
        if o[0] == 'f' and o[1] == 0:
            return NAN
        s = i[1] * (o[1] if o[0] == 'i' else (1 if o[1] > 0 else -1))
        return ('i', s)
    return ('f', round24(a[1] * b[1]))


def gt(a, limit):
    """fcomp contra un limite finito: falso si es NaN"""
    if a[0] == 'n':
        return False
    if a[0] == 'i':
        return a[1] > 0
    return a[1] > limit


def far(veh, ref, limit=Fraction(25)):
    dx, dy, dz = (sub(f32(veh[i]), f32(ref[i])) for i in range(3))
    return gt(add(add(mul(dz, dz), mul(dy, dy)), mul(dx, dx)), limit)


def ahead(ref, veh, fwd, limit=Fraction(0)):
    a = mul(sub(f32(ref[1]), f32(veh[1])), f32(fwd[1]))
    b = mul(sub(f32(ref[0]), f32(veh[0])), f32(fwd[0]))
    return gt(add(a, b), limit)


# ------------------------------------------------------------------------------------------------ modelo
def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def ptok(a):
    return 'A%d' % a


def model(inp):
    ev = []
    br = {}

    def B(name):
        br[name] = br.get(name, 0) + 1
    n = {'alive': [0] * 4, 'member': [0] * 4, 'screen': [0] * 4, 'task': [0] * 4, 'ev': [0] * 4}
    S = dict(
        p_intel=ptok(INTEL), p_flags=inp['pf46C'] & 0xFFFFFFFF, p_veh=[0, ptok(PVEH), ptok(VEH0)][inp['pveh']], p_mat=ptok(PMAT) if inp['pmat'] else 0,
        p_pos=list(inp['ppos']), p_mpos=list(inp['pmatpos']),
        v_pos=list(inp['vpos']), v_mpos=list(inp['vmatpos']),
        scan=[ptok(PED0 + s - 1) if s else 0 for s in inp['slot']],
        ped=[dict(created=inp['created'][k] & 0xFF, f46C=inp['f46C'][k], f478=inp['f478'][k], intel=ptok(PI1) if inp['pintel'][k] else ptok(PI0)) for k in range(4)],
        veh=[dict(created=inp['vcreated'][v] & 0xFF, status=inp['vstatus'][v] & 0xFF, pos=list(inp['vehpos%d' % v]), mpos=list(inp['vehmpos%d' % v]), fwd=list(inp['vehfwd%d' % v]),
                  ta=inp['vtemp'][v] & 0xFF, tt=(inp['vtemp'][v] * 0x9E3779B1) & 0xFFFFFFFF, has_mat=bool(inp['vmat_has'][v])) for v in range(3)],
        ft_c=inp['ftinit'], pev=0x5A5A5A5A, timer=inp['timer'], pend_type=0, done=False)
    init = json.loads(json.dumps(S))
    cur = [0]

    def mutate():
        if not inp['mut_at'] or S['done'] or len(ev) != inp['mut_at']:
            return
        S['done'] = True
        k, idx, val = inp['mut_kind'], inp['mut_idx'], inp['mut_val']
        p, v = idx % 4, idx % 3
        mp = inp['mut_pos']
        if k == 1:
            S['p_intel'] = ptok(INTELALT) if S['p_intel'] == ptok(INTEL) else ptok(INTEL)
        elif k == 2:
            S['p_flags'] ^= 0x100
        elif k == 3:
            S['p_veh'] = 0 if val % 3 == 0 else (ptok(PVEH) if val % 3 == 1 else ptok(VEH0))
        elif k == 4:
            S['p_pos'] = list(mp)
        elif k == 5:
            S['p_mat'] = ptok(PMAT) if val & 1 else 0
        elif k == 6:
            S['ped'][p]['f478'] ^= 0x10
        elif k == 7:
            S['ped'][p]['intel'] = ptok(PI1) if val & 1 else ptok(PI0)
        elif k == 8:
            S['ped'][p]['created'] = val & 0xFF
        elif k == 9:
            S['ped'][p]['f46C'] ^= 0x100
        elif k == 10:
            S['scan'][idx % 16] = 0 if val % 5 == 0 else ptok(PED0 + val % 5 - 1)
        elif k == 11:
            S['veh'][v]['created'] = val & 0xFF
        elif k == 12:
            S['veh'][v]['status'] = val & 0xFF
        elif k == 13:
            if S['veh'][v]['has_mat']:
                S['veh'][v]['fwd'] = [mp[0], mp[1]]
        elif k == 14:
            if S['veh'][v]['has_mat']:
                S['veh'][v]['mpos'] = list(mp)
        elif k == 15:
            S['timer'] = val
        else:
            if inp['vmat']:
                S['v_mpos'] = list(mp)
            else:
                S['v_pos'] = list(mp)

    def E(*a):
        ev.append(list(a))

    def tick(counter, k, bits, width):
        c = n[counter][k]
        n[counter][k] += 1
        return (inp[counter if counter != 'ev' else 'evk'][k] >> (width * (c & (31 if width == 1 else 15)))) & ((1 << width) - 1)

    def pos_of(tok):
        """posicion (3 floats) del vehiculo del jugador o de la lista"""
        if tok == ptok(PVEH):
            return S['v_mpos'] if inp['vmat'] else S['v_pos']
        v = S['veh'][0]
        return v['mpos'] if v['has_mat'] else v['pos']

    # ---- 1. peatones del escaner
    scan_arena = S['p_intel']   # leido una sola vez
    assert scan_arena == ptok(INTEL)
    for i in range(16):
        ent = S['scan'][i]
        if ent == 0:
            B('slot_null')
            continue
        k = int(ent[1:]) - PED0
        B('slot_ped')
        p = S['ped'][k]
        if p['created'] != 1:
            B('skip_created')
            continue
        if p['f46C'] & 0x100:
            B('skip_in_vehicle')
            continue
        E(1, tag(PED0 + k))
        cur[0] = k
        r = tick('alive', k, 0, 1)
        mutate()
        if not r:
            B('skip_dead')
            continue
        E(2, GROUPS, tag(PED0 + k))
        r = tick('member', k, 0, 1)
        mutate()
        if r:
            B('skip_member')
            continue
        E(3, tag(PED0 + k))
        r = tick('screen', k, 0, 1)
        mutate()
        if not r or (p['f478'] & 0x10):
            B('destroy_offscreen' if not r else 'destroy_flag')
            E(4, tag(PED0 + k))
            mutate()
            continue
        E(5, tag(int(p['intel'][1:])), 0x38F)
        kind = tick('task', k, 0, 2)
        flee_from_me = False
        if kind == 1:
            S['ft_c'] = ptok(PLAYER)
            flee_from_me = True
        elif kind == 2:
            S['ft_c'] = ptok(PED0 + (k + 1) % 4)
        elif kind == 3:
            S['ft_c'] = 0
        mutate()
        B('found_task_%d' % kind)
        if kind and flee_from_me:
            B('skip_found_task')
            continue
        E(6, tag(int(p['intel'][1:]), 0x68), 0x20)
        kind = tick('ev', k, 0, 2)
        has_task = False
        if kind == 1:
            S['pev'] = 0
        elif kind >= 2:
            S['pev'] = ptok(PTASK)
            S['pend_type'] = inp['ptype'][k] if kind == 2 else 0x38F
            has_task = True
        mutate()
        B('pending_event_%d' % kind)
        if kind and has_task:
            E(14, tag(PTASK))
            mutate()
            if S['pend_type'] & 0xFFFFFFFF == 0x38F:
                B('skip_pending_flee')
                continue
            B('pending_other_type_%s' % ('near' if S['pend_type'] & 0xFFFFFFFF in (0x38E, 0x390, 0x38F | 0x100, 0x38F | 0x80000000) else 'far'))
        B('flee_created')
        E(7, 0x40)
        mutate()
        E(8, tag(PLAYER), tag(FLEE, 0xC), tag(PLAYER))
        mutate()
        E(10, tag(int(p['intel'][1:]), 0x68), 0x20, 0, 0, 3, tag(FLEE), 0, 0, tag(PLAYER), 100000, 0x447A0000, 0, 1000, inp['tol'], 4)
        mutate()
        E(9, tag(PLAYER), tag(FLEE, 0xC), tag(PLAYER))
        mutate()
        E(11, tag(FLEE))
        mutate()
    # ---- 2. referencia y centro
    if (S['p_flags'] & 0x100) and S['p_veh']:
        ref = pos_of(S['p_veh'])
    else:
        ref = S['p_mpos'] if S['p_mat'] else S['p_pos']
    ref = list(ref)
    centre = list(S['p_mpos'] if S['p_mat'] else S['p_pos'])
    B('ref_vehicle' if ((S['p_flags'] & 0x100) and S['p_veh']) else 'ref_own')
    B('count_neg' if s16(inp['count']) < 0 else 'count_%d' % s16(inp['count']))
    E(12, centre[0], centre[1], centre[2], 0x41700000, 1, 6, 0, 1, 0, 0, 0)
    mutate()
    count = s16(inp['count'])
    # ---- 3. vehiculos
    for i in range(max(count, 0)):
        vi = inp['list'][i]
        assert vi < 3, 'entrada nula de la lista bajo la cuenta (el EXE se cae)'
        v = S['veh'][vi]
        if v['created'] == 2:
            B('veh_skip_mission')
            continue
        st = v['status'] >> 3
        if st == 0 or st == 9:
            B('veh_skip_status_%d' % st)
            continue
        pos = v['mpos'] if v['has_mat'] else v['pos']
        B('veh_has_matrix' if v['has_mat'] else 'veh_no_matrix')
        if far(pos, ref):
            B('veh_wait')
            v['ta'] = 1
            v['tt'] = (S['timer'] + 5000) & 0xFFFFFFFF
        else:
            assert v['has_mat'], 'vehiculo sin matriz cerca de la referencia (el EXE se cae)'
            v['ta'] = 3 if ahead(ref, pos, v['fwd']) else 9
            B('veh_reverse' if v['ta'] == 3 else 'veh_goforward')
            v['tt'] = (S['timer'] + 2000) & 0xFFFFFFFF
        E(13, tag(VEH0 + vi))
        mutate()
    return ev, S, init, br


def bytes_of(v, size):
    return [(v >> (8 * i)) & 0xFF for i in range(size)]


def field(out, off, size, old, new):
    if old == new:
        return
    if isinstance(old, str) or isinstance(new, str):
        for i in range(size):
            out[off + i] = ANY
        return
    for i, (o, nb) in enumerate(zip(bytes_of(old, size), bytes_of(new, size))):
        if o != nb:
            out[off + i] = nb


def changes_of(S, I):
    ch = [dict() for _ in range(NARENAS)]
    field(ch[PLAYER], 0x47C, 4, I['p_intel'], S['p_intel'])
    field(ch[PLAYER], 0x46C, 4, I['p_flags'], S['p_flags'])
    field(ch[PLAYER], 0x58C, 4, I['p_veh'], S['p_veh'])
    field(ch[PLAYER], 0x14, 4, I['p_mat'], S['p_mat'])
    for i in range(3):
        field(ch[PLAYER], 4 + 4 * i, 4, I['p_pos'][i], S['p_pos'][i])
        field(ch[PVMAT], 0x30 + 4 * i, 4, I['v_mpos'][i], S['v_mpos'][i])
        field(ch[PVEH], 4 + 4 * i, 4, I['v_pos'][i], S['v_pos'][i])
    for i in range(16):
        field(ch[INTEL], 0x130 + 4 * i, 4, I['scan'][i], S['scan'][i])
    for k in range(4):
        a = PED0 + k
        field(ch[a], 0x478, 4, I['ped'][k]['f478'], S['ped'][k]['f478'])
        field(ch[a], 0x46C, 4, I['ped'][k]['f46C'], S['ped'][k]['f46C'])
        field(ch[a], 0x47C, 4, I['ped'][k]['intel'], S['ped'][k]['intel'])
        field(ch[a], 0x484, 1, I['ped'][k]['created'], S['ped'][k]['created'])
    for v in range(3):
        a, m = VEH0 + v, VM0 + v
        field(ch[a], 0x4A4, 1, I['veh'][v]['created'], S['veh'][v]['created'])
        field(ch[a], 0x36, 1, I['veh'][v]['status'], S['veh'][v]['status'])
        field(ch[a], 0x3BB, 1, I['veh'][v]['ta'], S['veh'][v]['ta'])
        field(ch[a], 0x3BC, 4, I['veh'][v]['tt'], S['veh'][v]['tt'])
        for i in range(3):
            field(ch[m], 0x30 + 4 * i, 4, I['veh'][v]['mpos'][i], S['veh'][v]['mpos'][i])
            field(ch[a], 4 + 4 * i, 4, I['veh'][v]['pos'][i], S['veh'][v]['pos'][i])
        for i in range(2):
            field(ch[m], 0x10 + 4 * i, 4, I['veh'][v]['fwd'][i], S['veh'][v]['fwd'][i])
    field(ch[FTASK], 0xC, 4, I['ft_c'], S['ft_c'])
    field(ch[PEV], 0x10, 4, I['pev'], S['pev'])
    ch[FLEE] = IGNORE   # el constructor y el destructor reales de la tarea de huida escriben sus campos (se comprueban por los argumentos que ve Add)
    return ch


def match(a, b):
    if len(a) != len(b):
        return False
    return all(x is ANY or x == y for x, y in zip(a, b))


def consts(begin):
    return None


def check(inp, side_obs, begin=None):
    ev, S, I, _ = model(inp)
    changes = changes_of(S, I)
    obs = side_obs['observed']
    got = obs['events']
    if len(ev) != len(got) or not all(match(x, y) for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if not match(x, y):
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    if len(changes) != len(obs['changes']):
        return 'numero de arenas'
    for i, (exp, g) in enumerate(zip(changes, obs['changes'])):
        if exp == IGNORE:
            continue
        got_c = {off: val for off, val in g}
        for off in list(exp):
            if exp[off] is ANY:
                exp.pop(off)
                got_c.pop(off, None)
        if got_c != exp:
            return 'cambios de la arena %d: esperado %s, observado %s' % (i, exp, got_c)
    return None


def run(path):
    rows = [json.loads(line) for line in open(path, encoding='utf-8')]
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['input'], r[sname])
            if msg:
                bad += 1
                if bad <= 10:
                    print('caso %d (familia %d) lado %s: %s' % (r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
