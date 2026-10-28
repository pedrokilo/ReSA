"""Oraculo independiente (Python) de PlayerAnim80D: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x6098F0..0x609C7C (909 bytes), lo que debe observarse de
CPlayerPed::ProcessAnimGroups (traza de colaboradores EN ORDEN con sus argumentos y bytes cambiados en las once arenas) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Datos del EXE usados: datos del jugador = [jugador+0x480] (RELEIDOS en cada uso); rumbo = float [datos+0x50]; se mira la camara si NO (rumbo > [0x86D1D4]) o NO (rumbo < [0x86D1D0])
(las dos constantes salen de las entradas vigiladas de la fila `begin`; NaN siempre mira la camara); camara activa = byte 0xB6F081, funcion 0x50A850 (AL) y 0x5DF090 (AL); grupo inicial edi =
[jugador+0x4D4]; objeto del arma [+0x4F4] -> 0x732AC0 -> vcall +0x10 (AL == 4) -> edi = [modelo+0x24]; 0x23/0x24 -> 0x39; 0x601110 (puntero) -> 0x46; listas de armas -> 0x3F / 0x42 / 0x3C;
balaclava (0x5A7950 sobre [datos+4], AL) -> 0x45; si no edi = 0x36 y, con [+0x598] == 1, el grupo del modelo [ms_modelInfoPtrs[(int16)[+0x22]]+0x24] si no es 0x36; en otro caso
edi + 0x5A81B0() - 0x36; si [+0x4D4] es distinto se guarda y se llama a 0x609650. Despues el tipo del arma de la ranura con signo [+0x718] (+0x5A0 + ranura*0x1C) -> 0x5E6580 -> 0x743C60;
disparo != 0 o bit 0x100 de [+0x46C] -> suelta el 2o bloque; si no, combo base (byte con signo, [info+0x6E]) y estilo de pelea (byte, [+0x72D]) piden su grupo
(tabla 0xC170D0, 0x88 bytes por fila, fila = valor - 4): bloque del grupo en [0xB4EA34] + grupo*20 (nulo -> 0x4D3A30 + 0x4D3940), indice = (bloque - 0xB5D4A0) >> 5, IsLoaded (+0x10) == 1 ->
0x4D3FB0 y se guarda el grupo en [datos+0x48] / [datos+0x4C] si valia 0; si no 0x4087E0(indice + 0x63E7, 8). Los grupos antiguos se sueltan (0x4D3FD0) al final.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 sus datos, 2 los otros datos, 3 inteligencia, 4 objeto del arma,
5 modelo del arma, 6 modelo de peaton, 7 informacion del arma, 8 arreglo de grupos, 9 ropa, 10 ropa de los otros datos.
"""
import json
import struct
import sys

GUARD = 0x40
ANY = None
M = 0xFFFFFFFF
BLOCKS = 0xB5D4A0
RESOURCE_IFP = 25575


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


PLAYER, PD1, PD2, INTEL, CLUMP, MODEL, PEDMODEL, WINFO, GROUPS, DESC1, DESC2 = [tag(i, 0) for i in range(11)]


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M))[0]


def block_ptr(k1):
    return (BLOCKS + (k1 - 1) * 0x20) if k1 else 0


def block_index(k1):
    d = (block_ptr(k1) - BLOCKS) & M
    if d & 0x80000000:
        d -= 1 << 32
    return (d >> 5) & M      # sar 5 (aritmetico)


def fnv(s):
    h = 2166136261
    for b in s.encode():
        h = ((h ^ b) * 16777619) & M
    return h


def model(inp, c0, c1):
    ev = []

    def E(*a):
        ev.append(list(a))

    st = {'cur': 0, 'done': set(), 'swapped': False}
    ag = [inp['anim_group']]
    pd = [dict(f48=inp['f48'], f4c=inp['f4c']), dict(f48=inp['f48b'], f4c=inp['f4cb'])]
    init_pd = [dict(x) for x in pd]
    init_ag = ag[0]
    gblk = [inp['blk%d' % g] for g in range(24)]
    init_g = list(gblk)

    def cur():
        return pd[st['cur']]

    def mutate(site):
        if inp['mut_site'] != site or site in st['done']:
            return
        st['done'].add(site)
        k = inp['mut_kind']
        if k & 1:
            st['cur'] ^= 1
            st['swapped'] = True
        if k & 2:
            cur()['f48'] = inp['mut_a']
        if k & 4:
            cur()['f4c'] = inp['mut_b']
        if k & 8:
            gblk[inp['mut_g']] = inp['mut_blk']
        if k & 16:
            ag[0] = inp['mut_ag']

    def result():
        chp = {}
        if st['swapped']:
            for i in range(4):
                chp[0x480 + i] = ANY
        for b in range(4):
            o, n = (init_ag >> (8 * b)) & 0xFF, (ag[0] >> (8 * b)) & 0xFF
            if o != n:
                chp[0x4D4 + b] = n
        chd = [{}, {}]
        for i in range(2):
            for base, field in ((0x48, 'f48'), (0x4C, 'f4c')):
                for b in range(4):
                    o, n = (init_pd[i][field] >> (8 * b)) & 0xFF, (pd[i][field] >> (8 * b)) & 0xFF
                    if o != n:
                        chd[i][base + b] = n
        chg = {}
        for g in range(24):
            for b in range(4):
                o, n = (block_ptr(init_g[g]) >> (8 * b)) & 0xFF, (block_ptr(gblk[g]) >> (8 * b)) & 0xFF
                if o != n:
                    chg[g * 20 + b] = n
        return ev, [chp, chd[0], chd[1], {}, {}, {}, {}, {}, chg, {}, {}]

    hd = f32(inp['hdg'])
    check = (not (hd > c1)) or (not (hd < c0))      # NaN: ambas comparaciones desordenadas -> mira la camara
    edi = ag[0]
    eax = 0
    target = None                                   # 'PED' (0x609A1F), 'DEF' (0x609A3B), 'STORE' (0x609A44)
    if check:
        E(1, inp['cam_idx'])
        mutate(1)
        if inp['mouse'] & 0xFF:
            E(2, PLAYER)
            mutate(2)
            if inp['strafe'] & 0xFF:
                target = 'PED' if edi == 0x36 else 'DEF'
    if target is None:
        edi = 0
        if inp['wobj']:
            E(3, CLUMP)
            mutate(3)
            if inp['clump_ret']:
                E(4, MODEL)
                mutate(4)
                if inp['mtype'] & 0xFF == 4:
                    edi = inp['wtype'] & M
                    if edi in (0x23, 0x24):
                        edi = 0x39
                        target = 'DEF'
        if target is None:
            E(5, INTEL)
            mutate(5)
            if inp['jet'] != 0:
                eax = 0x46
                target = 'STORE'
        if target is None:
            if edi in (5, 6, 7):
                edi, target = 0x3F, 'DEF'
            elif edi in (9, 0x25, 0x26):
                edi, target = 0x42, 'DEF'
            elif edi in (0x1F, 0x1E, 0x1B, 0x19, 0x22, 0x21):
                edi, target = 0x3C, 'DEF'
            else:
                E(6, DESC1 if st['cur'] == 0 else DESC2)
                mutate(6)
                if inp['bal'] & 0xFF:
                    eax, target = 0x45, 'STORE'
                else:
                    edi, target = 0x36, 'PED'
    if target == 'PED':
        if inp['ped_type'] == 1:
            eax = inp['anim_type'] & M
            target = 'STORE' if eax != 0x36 else 'DEF'
        else:
            target = 'DEF'
    if target == 'DEF':
        E(7)
        mutate(7)
        eax = (edi + inp['def_group'] - 0x36) & M
    if ag[0] != eax:
        ag[0] = eax
        E(8, PLAYER)
        mutate(8)

    slot = s8(inp['slot'])
    wtype = inp['wt%d' % (slot + 20)]
    E(9, PLAYER)
    mutate(9)
    E(10, wtype, inp['skill'])
    mutate(10)
    rel1 = rel2 = False

    def want(field, row_idx, release_key):
        """pide el grupo `row` para el campo `field`; devuelve True si hay que soltar el anterior"""
        g = inp['row%d' % row_idx]
        c = cur()[field]
        rel = False
        if g != c:
            if c != 0:
                rel = True
            blk = gblk[g]
            if blk == 0:
                E(11, g)
                mutate(11)
                E(12, fnv('AB%d' % g))
                mutate(12)
                blk = 1 + inp['gb']
            idx = block_index(blk)
            if inp['loaded%d' % (blk - 1)] & 0xFF == 1:
                if cur()[field] == 0:
                    E(13, idx)
                    mutate(13)
                    cur()[field] = g
            else:
                E(15, (idx + RESOURCE_IFP) & M, 8)
                mutate(15)
        return rel

    if inp['fire'] != 0 or (inp['pflags'] & 0x100):
        rel1 = rel2 = True      # 0x609C73 salta al cuerpo de la liberacion del 1er bloque
    else:
        combo = s8(inp['base_combo'])
        if combo == 4:
            if cur()['f48'] != 0:
                rel1 = True
        else:
            rel1 = want('f48', combo - 4, 'first')
        style = inp['style'] & 0xFF
        if style == 4:
            if cur()['f4c'] != 0:
                rel2 = True
        else:
            rel2 = want('f4c', style - 4, 'second')
    for flag, field in ((rel1, 'f48'), (rel2, 'f4c')):
        if flag:
            ref = cur()[field]
            if ref != 0:
                E(14, block_index(gblk[ref]))
                mutate(14)
                cur()[field] = 0
    return result()


def match(a, b):
    if len(a) != len(b):
        return False
    return all(x is ANY or x == y for x, y in zip(a, b))


def consts(begin):
    w = {e['address']: e['bytes'] for e in begin['watched_entries']}
    return f32(int.from_bytes(bytes.fromhex(w[0x86D1D0]), 'little')), f32(int.from_bytes(bytes.fromhex(w[0x86D1D4]), 'little'))


def check(inp, side_obs, begin):
    c0, c1 = consts(begin)
    ev, changes = model(inp, c0, c1)
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
    begin = [r for r in rows if r['event'] == 'begin'][0]
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['input'], r[sname], begin)
            if msg:
                bad += 1
                if bad <= 10:
                    print('caso %d (familia %d) lado %s: %s' % (r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
