"""Oraculo independiente (Python) de PlayerGroup78: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x60D350..0x60D59A, lo que debe observarse de
CPlayerPed::ProcessGroupBehaviour (traza de colaboradores EN ORDEN con sus argumentos y bytes cambiados en las doce arenas) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Datos del EXE usados: objetivo = [jugador+0x71C]; con el byte 0xB6EC2E distinto de cero y sin objetivo, [jugador+0x79C]; un objetivo es peaton si ([obj+0x36] & 7) == 3 y es de tipo 8 si
[obj+0x598] == 8 (dword); datos del jugador [jugador+0x480] RELEIDOS en cada uso salvo el puntero edi de las pulsaciones; bit 0x100 de [datos+0x34] = reclutamiento deshabilitado;
contadores uint16: [datos+0x8A] (arriba) y [datos+0x88] (abajo); umbral 0x4B0; el paso es _ftol2 (truncacion a int64, se usa la parte baja) de ((paso * [0x858B38]) * [0x858C4C]) con la
x87 a 24 bits de mantisa (las dos constantes salen de las entradas vigiladas de la fila `begin`); cada 32 fotogramas ([0xB7CB4C] & 0x1F == 6) se recorre el pool de peatones
([0xB74490]: objetos, estados, tamano) de atras adelante, se saltan las ranuras con el bit 7 del estado y los peatones de tipo distinto de 8, el nivel de busqueda es
[[[FindPlayerPed(-1)+0x480]+0]+0x2C] con signo, leido antes de GetPedFlag(6).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 jugador, 1 sus datos, 2 los otros datos, 3 objetivo, 4 objetivo del
raton, 5 pad, 6 jugador de FindPlayerPed, 7 sus datos, 8 CWanted, 9 cabecera del pool, 10 estados de ranura, 11 peatones del pool (0x7C4 bytes cada uno).
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
ANY = None


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


PLAYER, PAD, TGT, MOUSE = tag(0, 0), tag(5, 0), tag(3, 0), tag(4, 0)


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


def f32_exact(bits):
    """Fraction exacta de un float de 32 bits, o 'nan'/'inf'"""
    bits &= 0xFFFFFFFF
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    sign = -1 if bits >> 31 else 1
    if e == 0:
        return sign * Fraction(m, 1 << 149)
    return sign * Fraction((1 << 23) | m, 1) * Fraction(2) ** (e - 150)


def round24(fr):
    """redondea al par mas cercano a 24 bits de mantisa con exponente sin limite (x87 con control de precision a 24 bits)"""
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


def step_add(ts_bits, c1_bits, c2_bits):
    """lo que suma una pulsacion: low16 de ftol2(((ts * c1) * c2))"""
    ts, c1, c2 = f32_exact(ts_bits), f32_exact(c1_bits), f32_exact(c2_bits)
    if ts in ('nan', 'inf'):
        return 0                                   # entero indefinido 0x8000000000000000: parte baja 0
    v = round24(round24(ts * c1) * c2)
    if abs(v) >= 2 ** 63:
        return 0
    t = int(v)                                     # trunca hacia cero
    return t & 0xFFFF


def model(inp, c1, c2):
    ev = []

    def E(*a):
        ev.append(list(a))

    st = {'cur': 0, 'done': set(), 'swap_ptr': False, 'swap_tgt': False}
    data = [dict(flags=inp['flags'], up=inp['up_t'] & 0xFFFF, down=inp['down_t'] & 0xFFFF),
            dict(flags=inp['oflags'], up=inp['oup_t'] & 0xFFFF, down=inp['odown_t'] & 0xFFFF)]
    init = [dict(d) for d in data]
    wanted = [inp['wanted']]

    def mutate(site):
        if inp['mut_site'] != site or site in st['done']:
            return
        st['done'].add(site)
        k = inp['mut_kind']
        if k & 1:
            st['cur'] ^= 1
            st['swap_ptr'] = True
        d = data[st['cur']]
        if k & 2:
            d['flags'] = inp['mut_flags']
        if k & 4:
            d['up'] = inp['mut_up'] & 0xFFFF
            d['down'] = inp['mut_down'] & 0xFFFF
        if k & 8:
            st['swap_tgt'] = True
        if site == 9:
            wanted[0] = inp['mut_wanted']

    def result():
        ch_player = {}
        if st['swap_ptr']:
            for i in range(4):
                ch_player[0x480 + i] = ANY
        if st['swap_tgt']:
            for i in range(4):
                ch_player[0x71C + i] = ANY
                ch_player[0x79C + i] = ANY
        chd = [{}, {}]
        for i in range(2):
            for base, field, size in ((0x34, 'flags', 4), (0x8A, 'up', 2), (0x88, 'down', 2)):
                for b in range(size):
                    old, new = (init[i][field] >> (8 * b)) & 0xFF, (data[i][field] >> (8 * b)) & 0xFF
                    if old != new:
                        chd[i][base + b] = new
        chw = {}
        for b in range(4):
            old, new = (inp['wanted'] >> (8 * b)) & 0xFF, (wanted[0] >> (8 * b)) & 0xFF
            if old != new:
                chw[0x2C + b] = new
        return ev, [ch_player, chd[0], chd[1], {}, {}, {}, {}, {}, chw, {}, {}, {}]

    ebp = TGT if inp['tgt'] else None
    if (inp['mouse'] & 0xFF) and ebp is None:
        ebp = MOUSE if inp['mtgt'] else None

    def is_ped():
        if ebp is None:
            return False
        t = inp['t_type'] if ebp == TGT else inp['m_type']
        return (t & 7) == 3

    def ped_type8():
        return (inp['t_ped'] if ebp == TGT else inp['m_ped']) & 0xFFFFFFFF == 8

    add = step_add(inp['ts'], c1, c2)

    # --- primer bloque: cruceta arriba
    E(1, 0xFFFFFFFF, 0)
    mutate(1)
    if inp['veh1'] == 0:
        E(2, PAD)
        mutate(2)
        edi = st['cur']
        if inp['up'] & 1:
            data[edi]['up'] = (data[edi]['up'] + add) & 0xFFFF
            d = data[st['cur']]
            if d['up'] == 0x4B0 and not d['flags'] & 0x100:
                E(4, PLAYER)
                mutate(3)
        else:
            held = data[edi]['up']
            if held != 0 and held < 0x4B0:
                join = False
                if is_ped():
                    if ped_type8():
                        join = True
                    else:
                        E(7)
                        mutate(4)
                        join = bool(inp['jf'] & 1)
                if join:
                    if not data[edi]['flags'] & 0x100:
                        E(6, PLAYER, ebp)
                        mutate(5)
                else:
                    E(5, PLAYER, 1, 1, 0)
                    mutate(6)
            data[st['cur']]['up'] = 0
    # --- segundo bloque: cruceta abajo (se omite con el reclutamiento deshabilitado, y entonces tampoco hay recorrido del pool)
    if data[st['cur']]['flags'] & 0x100:
        return result()
    E(1, 0xFFFFFFFF, 0)
    mutate(7)
    if inp['veh2'] == 0:
        E(3, PAD)
        mutate(8)
        edi = st['cur']
        if inp['down'] & 1:
            data[edi]['down'] = (data[edi]['down'] + add) & 0xFFFF
            if data[st['cur']]['down'] >= 0x4B0:
                E(4, PLAYER)
                mutate(3)
        else:
            held = data[st['cur']]['down']
            if held != 0 and held < 0x4B0:
                if is_ped() and ped_type8():
                    E(6, PLAYER, ebp)
                    mutate(5)
                else:
                    E(5, PLAYER, 0, 1, 0)
                    mutate(6)
            data[st['cur']]['down'] = 0
    # --- recorrido del pool de peatones cada 32 fotogramas
    if (inp['frame'] & 0x1F) == 6:
        for i in range(inp['pool_size'] - 1, -1, -1):
            if inp['slot%d' % i] & 0x80:
                continue
            if inp['spt%d' % i] & 0xFFFFFFFF != 8:
                continue
            E(8, 0xFFFFFFFF)
            level = s32(wanted[0])
            E(9, 6)
            mutate(9)
            acq = tag(11, i * 0x7C4 + 0x4E0)
            if level > 0:
                E(10, acq, 4, inp['ped_flag'])
            else:
                E(11, acq, 4, inp['ped_flag'])
    return result()


def match(a, b):
    if len(a) != len(b):
        return False
    return all(x is ANY or x == y for x, y in zip(a, b))


def consts(begin):
    w = {e['address']: e['bytes'] for e in begin['watched_entries']}
    return int.from_bytes(bytes.fromhex(w[0x858B38]), 'little'), int.from_bytes(bytes.fromhex(w[0x858C4C]), 'little')


def check(inp, side_obs, begin):
    c1, c2 = consts(begin)
    ev, changes = model(inp, c1, c2)
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
