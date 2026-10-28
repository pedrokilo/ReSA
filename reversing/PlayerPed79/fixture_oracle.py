"""Oraculo independiente (Python) de PlayerPed79: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de cada funcion (traza de
colaboradores EN ORDEN con sus argumentos y bytes cambiados en las arenas) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni
rama del C++ de produccion.

Funcion 0 - CPlayerPed::SetInitialState (0x60CD20..0x60CFF6, thiscall, `ret 4`, void). Lectura del desensamblado:
  ClearDrunkBlur (0x71D780); [0xB7CB64] = 1.0f; [jug+0x1C] |= 1; [jug+0x40] |= 2; ClearAimFlag; ClearLookFlag; [jug+0x46C] |= 0x2000; con [jug+0x730] != 0: Extinguish(fuego);
  SetPedState(1); SetMoveState(1); (se cargan [0x474], [0x46C] y [0x47C] DESPUES de las dos llamadas) [0x474] = ([0x474] & ~0x42) | 0x4000000; [0x46C] &= ~0x4000000; FlushIntelligence(ecx = [0x47C]);
  RemoveAllAssociations([jug+0x18]); CTask::operator new(0x1C) y, si no es nulo, constructor de CTaskSimplePlayerOnFoot; SetTask([0x47C]+4, tarea, 4, 0);
  ([0x46C] y [0x480] se leen DESPUES de SetTask) [0x46C] &= ~0x100000; [0x4D4] = 0x36; con datos: [datos+0x34] &= ~2; SetRealMoveAnim (0x60A9C0); [[0x59C]+0x25] = 0x32;
  si [0xFC] != 0 (leido ahora): [0x1C] |= 1; [0xFC] = 0; [0x784] = 0; CTask::operator new(0x20) y, si no es nulo, constructor de CTaskComplexFacial (1 rand); SetTaskSecondary([0x47C]+4, tarea, 3);
  si el byte bajo del argumento es 0 y ([[0x480]+0x34] & 0x400) == 0: [grupo([datos]) + 4] = 1 (el indice de grupo [[0x480]+0x38] se relee en CADA uso); SetDefaultTaskAllocatorType(grupo+0x30, 5);
  evento CEventPlayerCommandToGroupAttack en la pila; ComputeResponseTaskType(evento, grupo) (deja el word +0xE); WillRespond (AL); si AL != 0: CEvent::operator new(0x1C) + constructor del evento
  Gather (+0xC = 1, +0xE = word del evento de la pila, +0x10 = 0xFFFF, +0x14 = 2, +0x18 = 0), CEventGroupEvent(jugador, evento) (RegisterReference), AddEvent(grupo+0x30, evento de grupo),
  destructor (CleanUpOldReference y CEvent::operator delete); con [0x480] != 0: CPlayerPedData::SetInitialState(datos).
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 sus datos, 2 los otros datos, 3 inteligencia, 4 otra inteligencia,
5 estadisticas, 6 fuego, 7 tarea A, 8 tarea B, 9 evento; 10 grupos de CPedGroups (sin guarda: 0x10000000 + (10 << 20) + desplazamiento). Un puntero local de la pila se etiqueta 0xDEADC0DE.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
ANY = None
STACK = 0xDEADC0DE


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


def tagg(off):
    return 0x10000000 + (10 << 20) + off


def opt(v):
    return ('opt', v)


def dword_changes(out, base, old, new):
    for b in range(4):
        o, n = (old >> (8 * b)) & 0xFF, (new >> (8 * b)) & 0xFF
        if o != n:
            out[base + b] = n


# ------------------------------------------------------------------------------------------------ funcion 0
def model0(inp):
    ev = []

    def E(*a):
        ev.append(list(a))

    f = dict(f1C=inp['f1C'], f40=inp['f40'], f46C=inp['f46C'], f474=inp['f474'], fFC=inp['fFC'], f784=inp['f784'], f4D4=inp['f4D4'])
    init = dict(f)
    st = dict(data=1 if inp['has_data'] else 0, intel=3, done=set(), swap_data=False, swap_intel=False)
    dfl = [inp['dflags'], inp['odflags']]
    dflags_init = list(dfl)
    group = [inp['group'], inp['ogroup']]
    after = {'groups': None}

    def mutate(site):
        if inp['mut_site'] != site or site in st['done']:
            return
        st['done'].add(site)
        k = inp['mut_kind']
        if k & 1:
            st['data'] = 2 if st['data'] == 1 else 1
            st['swap_data'] = True
        if k & 2:
            st['intel'] = 4 if st['intel'] == 3 else 3
            st['swap_intel'] = True
        if k & 4:
            f['f46C'] = inp['mut_f46C']
            f['f474'] = inp['mut_f474']
        if k & 8:
            f['f1C'] = inp['mut_f1C']
            f['fFC'] = inp['mut_fFC']
        if k & 16 and st['data'] in (1, 2):
            dfl[st['data'] - 1] = inp['mut_dflags']

    groups = [0] * 8   # se rellena abajo a partir de groups_before
    wrote_group = [None]
    tasks = {'a': False, 'b': False}
    event = {'built': False}

    E(22)
    f['f1C'] |= 1
    f['f40'] |= 2
    E(1, tag(0, 0)); mutate(1)
    E(2, tag(0, 0)); mutate(2)
    f['f46C'] |= 0x2000
    if inp['fire']:
        E(3, tag(6, 0)); mutate(3)
    E(4, tag(0, 0), 1); mutate(4)
    E(5, tag(0, 0), 1); mutate(5)
    f['f474'] = (f['f474'] & 0xFFFFFFBD) | 0x4000000
    f['f46C'] &= 0xFBFFFFFF
    E(6, tag(st['intel'], 0)); mutate(6)
    E(7, inp['clump']); mutate(7)
    E(8, 0x1C); mutate(8)
    if not inp['task_fail1']:
        tasks['a'] = True
    E(9, tag(st['intel'], 4), 0 if inp['task_fail1'] else tag(7, 0), 4, 0); mutate(9)
    f['f46C'] &= 0xFFEFFFFF
    f['f4D4'] = 0x36
    if st['data']:
        dfl[st['data'] - 1] &= 0xFFFFFFFD
    E(10, tag(0, 0)); mutate(10)
    if f['fFC'] != 0:
        if not f['f1C'] & 1:
            f['f1C'] |= 1
    f['fFC'] = 0
    f['f784'] = 0
    E(8, 0x20); mutate(11)
    if not inp['task_fail2']:
        tasks['b'] = True
        E(21)
    E(11, tag(st['intel'], 4), 0 if inp['task_fail2'] else tag(8, 0), 3); mutate(12)
    gset = None
    if (inp['arg'] & 0xFF) == 0:
        d = st['data']
        if d and not dfl[d - 1] & 0x400:
            g1 = group[d - 1]
            gset = g1
            g2 = group[st['data'] - 1]
            E(12, tagg(g2 * 0x2D4 + 0x30), 5); mutate(13)
            g3 = group[st['data'] - 1]
            E(13, STACK, tagg(g3 * 0x2D4)); st['_dummy'] = 0; mutate(14)
            E(14, STACK); mutate(15)
            if inp['will'] & 0xFF:
                E(15, 0x1C); mutate(16)
                event['built'] = True
                E(18, tag(0, 0), STACK, tag(0, 0))
                g4 = group[st['data'] - 1]
                E(16, tagg(g4 * 0x2D4 + 0x30), 47, tag(0, 0), tag(9, 0), 76, inp['task_id'] & 0xFFFF); mutate(17)
                E(19, tag(0, 0), STACK, tag(0, 0))
                E(20, tag(9, 0))
    if st['data']:
        E(17, tag(st['data'], 0)); mutate(18)

    # ---- cambios
    pl = {}
    for key, off in (('f1C', 0x1C), ('f40', 0x40), ('f46C', 0x46C), ('f474', 0x474), ('fFC', 0xFC), ('f784', 0x784), ('f4D4', 0x4D4)):
        dword_changes(pl, off, init[key], f[key])
    if st['swap_data'] or (inp['has_data'] == 0 and st['data']):
        for i in range(4):
            pl[0x480 + i] = ANY
    if st['swap_intel']:
        for i in range(4):
            pl[0x47C + i] = ANY
    dd = [{}, {}]
    for i in range(2):
        dword_changes(dd[i], 0x34, dflags_init[i], dfl[i])
    stat = {0x25: opt(0x32)}
    tk = [{}, {}]
    ev_ch = {}
    if event['built']:
        ev_ch = {0: ANY, 1: ANY, 2: ANY, 3: ANY}
        tid = inp['task_id'] & 0xFFFF
        for off, val in ((4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (0xC, 1), (0xE, tid & 0xFF), (0xF, tid >> 8), (0x10, 0xFF), (0x11, 0xFF), (0x14, 2), (0x15, 0), (0x16, 0), (0x17, 0), (0x18, 0), (0x19, 0), (0x1A, 0), (0x1B, 0)):
            ev_ch[off] = opt(val) if val == 0xCC else val
        if tid & 0xFF == 0xCC:
            ev_ch[0xE] = opt(0xCC)
        if tid >> 8 == 0xCC:
            ev_ch[0xF] = opt(0xCC)
    # arenas: jugador, datos, otros datos, inteligencia, otra inteligencia, estadisticas, fuego, tarea A, tarea B, evento
    changes = [pl, dd[0], dd[1], {}, {}, stat, {}, tk[0], tk[1], ev_ch]
    ranges = {7: (0, 0x1C) if tasks['a'] else (0, 0), 8: (0, 0x20) if tasks['b'] else (0, 0)}
    return ev, changes, ranges, gset


def check0(inp, obs):
    ev, changes, ranges, gset = model0(inp)
    got = obs['events']
    if len(ev) != len(got) or any(x != y for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if x != y:
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    msg = compare_changes(changes, ranges, obs['changes'])
    if msg:
        return msg
    gb, ga = obs['groups_before'], obs['groups_after']
    exp = list(gb)
    if gset is not None:
        exp[gset] = 1
    if ga != exp:
        return 'grupos: esperado %s, observado %s' % (exp, ga)
    if obs['time_scale'] != 0x3F800000:
        return 'escala de tiempo %s' % obs['time_scale']
    if obs['drunk'] != inp['drunk']:
        return 'borrachera %s' % obs['drunk']
    return None


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


import oracle_fn1


def check1(inp, obs):
    return oracle_fn1.check1(inp, obs, compare_changes)


MODELS = {'CPlayerPed::SetInitialState': check0, 'CPlayerPed::FindWeaponLockOnTarget': check1}


def check(fn, inp, side_obs, begin):
    return MODELS[fn](inp, side_obs['observed'])


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
