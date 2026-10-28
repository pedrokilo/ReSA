"""Oraculo independiente (Python) de PlayerGroup75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x60C840..0x60CC42, lo que debe observarse de
CPlayerPed::MakeThisPedJoinOurGroup (traza de colaboradores EN ORDEN con sus argumentos, bytes cambiados en las nueve arenas, separacion maxima escrita en el grupo del jugador) y lo
compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Datos del EXE usados: tipos de evento (32 CEventScriptCommand, 47 CEventGroupEvent, 90 CEventNewGangMember, 94 CEventDontJoinPlayerGroup: las ranuras GetEventType de sus tablas virtuales
0x85B378, 0x85AE48, 0x86D0A0, 0x86D0E0), constructor de la tarea 0x632E50 (+0xC grupo, +0x10 lider, +0x14/+0x18/+0x20 cero, +0x1C = 0xC8, +0x24 = -1; base 0x61A3B0: +4 y +8 a cero),
constructor del evento 0x608F70 (CEvent 0x4ABFC0: +4 y +8 a cero) y la separacion 0x86C6C0 (120.0f, leida de la entrada vigilada de la fila `begin`).
Lo que el oraculo no puede conocer (la direccion del pool de peatones en GetRef y el puntero al peaton dentro del evento reservado) se marca como comodin y solo lo comparan los dos lados.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 sus datos, 2 otro jugador, 3 sus datos, 4 peaton,
5 inteligencia, 6 tarea reservada, 7 evento reservado, 8 otro grupo (GetPedsGroup); 9 = CPedGroups::ms_groups (memoria real, sin guarda: 0x10000000 + (9 << 20) + desplazamiento).
"""
import json
import struct
import sys

GUARD = 0x40
LOCAL = 0xDEADC0DE
ANY = None
EV_SCRIPT, EV_GROUP, EV_NEWMEMBER, EV_DONTJOIN = 32, 47, 90, 94


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


PLAYER, DATA, OPLAYER, ODATA, PED, INTEL, TASK, EVMEM, OGROUP = (tag(i, 0) for i in range(9))


def GRP(g, off=0):
    return 0x10000000 + (9 << 20) + g * 0x2D4 + off


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & 0xFFFFFFFF))[0]


def model(inp):
    """devuelve (eventos esperados, cambios esperados por arena [dict desplazamiento -> byte o ANY], separacion esperada [indice, valor] o None)"""
    ev = []

    def E(*a):
        ev.append(list(a))

    st = dict(group=inp['group'], limit=inp['limit'] & 0xFF, f470=inp['flags470'], f474=inp['flags474'])
    done = dict(joined=False, task=False, evmem=False)

    def mutate(site):
        if inp['mut_site'] == site:
            st['group'] = inp['mut_group']
            st['limit'] = inp['mut_limit'] & 0xFF

    def result(sep=None):
        data = {}
        for i in range(4):
            if (st['group'] >> (8 * i)) & 0xFF != (inp['group'] >> (8 * i)) & 0xFF:
                data[0x38 + i] = (st['group'] >> (8 * i)) & 0xFF
        if st['limit'] != inp['limit'] & 0xFF:
            data[0x43] = st['limit']
        ped = {}
        for base, new, old in ((0x470, st['f470'], inp['flags470']), (0x474, st['f474'], inp['flags474'])):
            for i in range(4):
                nb, ob = (new >> (8 * i)) & 0xFF, (old >> (8 * i)) & 0xFF
                if nb != ob:
                    ped[base + i] = nb
        task = {}
        if done['task']:
            gid = done['gid']
            for off, val in ((4, 0), (5, 0), (6, 0), (7, 0), (8, 0), (9, 0), (10, 0), (11, 0),
                             (0xC, gid & 0xFF), (0xD, (gid >> 8) & 0xFF), (0xE, (gid >> 16) & 0xFF), (0xF, (gid >> 24) & 0xFF), (0x10, 0),
                             (0x14, 0), (0x15, 0), (0x16, 0), (0x17, 0), (0x18, 0), (0x19, 0), (0x1A, 0), (0x1B, 0), (0x1C, 0xC8), (0x1D, 0), (0x1E, 0), (0x1F, 0),
                             (0x20, 0), (0x21, 0), (0x22, 0), (0x23, 0), (0x24, 0xFF), (0x25, 0xFF), (0x26, 0xFF), (0x27, 0xFF)):
                task[off] = val
            for off in (0, 1, 2, 3):
                task[off] = ANY
        evm = {}
        if done['evmem']:
            for off in (4, 5, 6, 7, 8):
                evm[off] = 0
            for off in (0, 1, 2, 3, 0xC, 0xD, 0xE, 0xF):
                evm[off] = ANY
        changes = [{}, data, {}, {}, ped, {}, task, evm, {}]
        return ev, changes, sep

    # --- 0x60C840: ¿el peaton va drogado?
    if inp['flags478'] & 0x400:
        E(1, PLAYER, 0x52, 0, 0x3F800000, 0, 0, 0)
        return result()
    E(2, INTEL + 4, 1000)
    if inp['has_task']:
        return result()
    if not (inp['ped_type'] == 8 or inp['flag_a'] or inp['flag_b'] or inp['flag_c']):
        return result()
    E(3, GRP(st['group'], 8), PED)
    mutate(1)
    if inp['is_member'] & 1:
        return result()
    E(4, 0xFFFFFFFF, 10000, 1, 0xFFFFFFFF, 0xFFFFFFFF)
    mutate(2)
    E(1, PLAYER, 0x7F, 0, 0x3F800000, 1, 0, 0)
    E(5)
    mutate(3)
    limit = s32(inp['max_members'])
    if limit >= st['limit']:
        limit = st['limit']
    E(6, 0xB5)
    if f32(inp['stat1']) == 1.0:
        limit = 0
    else:
        E(6, 0xB5)
        if f32(inp['stat2']) == 2.0:
            limit = 0
    counter = [0]

    def count():
        E(8, GRP(st['group'], 8))
        c = s32(inp['count%d' % min(counter[0], 3)])
        if counter[0] == 0:
            mutate(4)
        counter[0] += 1
        return c

    join = False
    E(7)
    if inp['join_flags'] & 1:
        join = count() < 7
    if not join:
        if count() >= limit or count() >= limit:
            E(1, PED, 0x7D, 2500, 0x3F800000, 1, 0, 0)
            E(18, PLAYER, LOCAL, PLAYER)
            E(13, INTEL + 0x68, EV_DONTJOIN, 0, 0, PLAYER, 0, 0, 0)
            E(19, PLAYER, LOCAL, PLAYER)
            return result()
    # --- union
    E(9, PED)
    if inp['peds_group']:
        E(10, OGROUP + 8, PED)
    E(11, 0x28)
    task = 0
    if not inp['task_fail']:
        E(12, 0xFFFFFFFF)
        task = TASK
        done['task'] = True
        done['gid'] = inp['other_group'] if inp['other_player'] else st['group']
    E(13, INTEL + 0x68, EV_SCRIPT, 0, 0, 3, task, 0, 0)
    E(14, GRP(st['group'], 8), PED)
    mutate(5)
    E(15, GRP(st['group']))
    E(16, PED)
    if inp['mut_flags']:
        st['f470'] = inp['mut_flag_470']
    E(17, 0x10)
    member = 0
    if not inp['event_fail']:
        member = EVMEM
        done['evmem'] = True
        E(18, PED, tag(7, 0xC), PED)
    E(18, PLAYER, LOCAL, PLAYER)
    E(20, GRP(st['group'], 0x30), EV_GROUP, PLAYER, member, EV_NEWMEMBER if member else 0xFFFFFFFF, PED if member else 0, 0)
    st['f470'] &= 0xFFEFFFFF
    E(21, 0xEE, 0x3F800000)
    E(22, 1, 0x150, 0x3F800000)
    color = (((((inp['color_a'] & 0xFF) << 8) | (inp['color_b'] & 0xFF)) << 8) | (inp['color_c'] & 0xFF)) << 8 | 0xFF   # los bytes de la tabla son u8
    E(23, ANY, PED)
    mutate(6)
    E(24, 2, inp['ref'], color, 2)
    mutate(7)
    E(25, inp['blip'], 2)
    E(26, inp['blip'], color)
    E(27, inp['blip'], 1)
    if inp['mut_flags']:
        st['f474'] = inp['mut_flag_474']
    st['f474'] |= 0x2000
    slot = st['group']
    E(1, PED, 0x7E, 2500, 0x3F800000, 1, 0, 0)
    E(19, PLAYER, LOCAL, PLAYER)
    if member:
        E(19, PED, tag(7, 0xC), PED)
        E(29, EVMEM)
    if task:
        E(28, TASK)
    done['joined'] = True
    return result((slot, 0x42F00000))


def match(a, b):
    if len(a) != len(b):
        return False
    return all(x is ANY or x == y for x, y in zip(a, b))


def check(inp, side_obs, begin=None):
    ev, changes, sep = model(inp)
    obs = side_obs['observed']
    got = obs['events']
    if len(ev) != len(got) or not all(match(x, y) for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if not match(x, y):
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    for i, (exp, g) in enumerate(zip(changes, obs['changes'])):
        got_c = {off: val for off, val in g}
        for off in list(exp):
            if exp[off] is ANY:
                exp.pop(off)
                got_c.pop(off, None)
        if got_c != exp:
            return 'cambios de la arena %d: esperado %s, observado %s' % (i, exp, got_c)
    before, after = obs['separation_before'], obs['separation_after']
    want = list(before)
    if sep is not None:
        want[sep[0]] = sep[1]
    if after != want:
        return 'separacion de los grupos: esperado %s, observado %s' % (want, after)
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
