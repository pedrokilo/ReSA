"""Oraculo independiente (Python) de FightHitPed75: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado 0x61CBA0..0x61D0A6, lo que debe observarse de
CTaskSimpleFight::FightHitPed (traza de colaboradores EN ORDEN con sus argumentos, EAX = victima o 0, bytes cambiados en las once arenas) y lo compara con las filas de comparisons.jsonl
(lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. Las tablas de combos (0xC16900..) y de retardos (0x8D2D00..) son datos de entrada del EXE: se leen de las
entradas vigiladas de la fila `begin`.

La aritmetica float es la del x87 del EXE: palabra de control a 24 bits (mantisa de 24 bits, redondeo al par, exponente sin limite), cada resultado guardado como float (fstp dword, con
desnormales). Los NaN solo se comparan como «es NaN» (la carga depende del orden de operandos; la igualdad EXE/DLL bit a bit la prueba el banco y las reglas de la x87 el propio hardware);
todo lo demas, bit a bit. Lo que el oraculo no puede conocer (campos del evento sin inicializar en la pila y la hora del juego) se marca como comodin y solo lo comparan los dos lados entre si.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + 0x40 + desplazamiento; indices: 0 tarea, 1 atacante, 2 victima, 3 matriz del atacante, 4 matriz de la victima,
5 posn1, 6 posn2, 7 inteligencia, 8 tarea de la victima (bloqueo), 9 tarea activa, 10 grupo de eventos.
"""
import json
import sys
from fractions import Fraction

GUARD = 0x40
LOCAL = 0xDEADC0DE
FX = 0xA9AE00


def tag(idx, off):
    return 0x10000000 + (idx << 20) + GUARD + off


TASK, CREATOR, VICTIM, MATC, MATV, P1, P2, INTEL, BLOCKER, ACTIVE, GROUP = (tag(i, 0) for i in range(11))
CREATOR_AE = CREATOR + 0x138
CREATOR_WAE = CREATOR + 0x394
LAST = {}           # chance y tirada del ultimo modelo evaluado (cobertura de run_local.py)
ANY = None          # comodin: solo lo comparan los dos lados entre si
NAN = ('n',)        # marca de «es NaN» (cualquier carga)


class NanMark:
    def __repr__(self):
        return 'NaN'


NANM = NanMark()


# ---------------------------------------------------------------- aritmetica x87 a 24 bits
def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN (cualquier carga)"""
    if a == NAN:
        return None
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def ftol_low(a):
    """_ftol2 (0x821B40): trunca a int64; NaN, infinito y fuera de rango dan 0x8000000000000000. Devuelve los 32 bits bajos (EAX)"""
    if a == NAN or a[0] == 'i':
        return 0
    t = int(a[1])
    if t >= 2 ** 63 or t < -2 ** 63:
        return 0
    return t & 0xFFFFFFFF


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


def sx8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def sx16(v):
    v &= 0xFFFF
    return v - 65536 if v & 0x8000 else v


class Tables:
    def __init__(self, begin):
        self.blocks = []
        for w in begin['watched_entries']:
            if w['address'] in (0xC16900, 0x8D2D00):
                self.blocks.append((w['address'], bytes.fromhex(w['bytes'])))

    def rd32(self, addr):
        addr &= 0xFFFFFFFF
        for base, data in self.blocks:
            if base <= addr and addr + 4 <= base + len(data):
                return int.from_bytes(data[addr - base:addr - base + 4], 'little')
        raise KeyError('direccion fuera de las tablas vigiladas: %#x' % addr)


def row_of(combo):
    return (0xC170D0 + (combo - 4) * 0x88) & 0xFFFFFFFF


def model(inp, tables):
    """devuelve (eventos esperados, EAX esperado, cambios esperados por arena)"""
    ev = []

    def E(*a):
        ev.append(list(a))

    st = dict(combo=inp['combo'] & 0xFF, move=inp['move'] & 0xFF, health=inp['health'], weapon=inp['weapon_type'], p1=[inp['p1_x'], inp['p1_y'], inp['p1_z']], cont=0, reached=False)
    mut_site = inp['mut_site']

    def mutate(site):
        if mut_site == site:
            st['combo'] = inp['mut_combo'] & 0xFF
            st['move'] = inp['mut_move'] & 0xFF

    def result(eax):
        task = {}
        if st['reached']:
            task[0x14] = 0xFF
        for off, new, old in ((0x24, st['combo'], inp['combo'] & 0xFF), (0x25, st['move'], inp['move'] & 0xFF)):
            if new != old:
                task[off] = new
        victim = {}
        if st['health'] != inp['health']:
            for i in range(4):
                b = (st['health'] >> (8 * i)) & 0xFF
                if b != (inp['health'] >> (8 * i)) & 0xFF:
                    victim[0x540 + i] = b
        creator = {}
        if st['weapon'] != inp['weapon_type']:
            slot = sx8(inp['slot'])
            for i in range(4):
                b = (st['weapon'] >> (8 * i)) & 0xFF
                if b != (inp['weapon_type'] >> (8 * i)) & 0xFF:
                    creator[0x5A0 + slot * 0x1C + i] = b
        p1 = {}
        for w in range(3):
            old = [inp['p1_x'], inp['p1_y'], inp['p1_z']][w]
            for i in range(4):
                b = (st['p1'][w] >> (8 * i)) & 0xFF
                if b != (old >> (8 * i)) & 0xFF:
                    p1[4 * w + i] = b
        changes = [task, creator, victim, {}, {}, p1, {}, {}, {}, {}, {}]
        return ev, eax, changes

    def sound(row, offset, mv):
        return tables.rd32(row + offset + mv * 4)

    def pedae(event, vol, speed, a7, maxvol):
        E(6, CREATOR_AE, event, vol, speed, VICTIM, 0, a7, maxvol)
        if st.setdefault('peae', 0) == 0:
            mutate(3)
        st['peae'] += 1

    # --- 0x61CBA0: ¿la victima es el jugador con la tarea activa 0xCD?
    E(1, VICTIM)
    if inp['victim_player'] & 1:
        E(2, INTEL + 4)
        E(3, ACTIVE)
        if inp['task_type'] == 0xCD:
            return result(0)
    E(4, INTEL)
    if inp['has_fighting']:
        E(4, INTEL)
        E(5, BLOCKER, VICTIM, CREATOR, st['combo'], st['move'])
        mutate(7)
        if inp['blocked'] & 1:
            row = row_of(sx8(st['combo']))
            pedae(sound(row, 0x70, sx8(st['move'])), 0xC1100000, 0x3F800000, 0, 0)
            if st['combo'] == 5:
                m2 = sx8(st['move'])
                if 0 <= m2 <= 2:
                    pedae(sound(row, 0x70, m2), 0xC1100000, 0x3F800000, 0, tables.rd32(0x8D2E3C + m2 * 4))
            return result(0)
    # --- golpe normal
    E(7, st['weapon'], 1)
    row = row_of(sx8(st['combo']))
    E(8, TASK, CREATOR)
    mutate(1)
    damage = ftol_low(load(inp['damage']))
    st['cont'] = 0xFF
    st['reached'] = True
    cpos = [inp['cpos_x'], inp['cpos_y'], inp['cpos_z']]
    vpos = [inp['vpos_x'], inp['vpos_y'], inp['vpos_z']]
    E(9, VICTIM, (MATC + 0x30) if inp['creator_mat'] else (CREATOR + 4), *cpos)
    side = inp['side']
    E(10, VICTIM, CREATOR, st['weapon'], damage, sx16(inp['component']) & 0xFFFFFFFF, side & 0xFF)
    mutate(2)
    if inp['gde_health_mut']:
        st['health'] = inp['gde_health']
    if inp['mut_weapon']:
        cur = st['weapon']
        st['weapon'] = (7 if cur == 9 else 9) if inp['mut_weapon'] == 1 else 9
    damaged = inp['gde_ok'] & 1
    if st['weapon'] == 9:
        E(11, CREATOR_WAE, 0x9B)
    if side == 0:
        cl = st['combo']
        if cl == 5 and 0 <= sx8(st['move']) <= 2:
            pedae(sound(row, 0x5C, sx8(st['move'])), 0, 0x3F800000, 0, 0)
            mv = sx8(st['move'])
            pedae(sound(row, 0x5C, mv), 0, 0x3F800000, 0, tables.rd32(0x8D2E3C + mv * 4))
        elif cl == 7 and st['move'] == 1:
            pedae(tables.rd32(row + 0x60), 0, 0x3F800000, 0, tables.rd32(0x8D2E40))
            mv = sx8(st['move'])
            t = s32(tables.rd32(0x8D2E3C + mv * 4))
            v = ('f', Fraction(t), 0) if t >= 0 else fin(Fraction(t) + 2 ** 32, 0)
            scaled = ftol_low(mul(v, load(0x40333333)))
            pedae(sound(row, 0x5C, sx8(st['move'])), 0, 0x3F800000, 0, scaled)
        else:
            pedae(sound(row, 0x5C, sx8(st['move'])), 0, 0x3F800000, 0, 0)
    else:
        pedae(sound(row, 0x70, sx8(st['move'])), 0, 0x3F800000, 0, 0)
        if st['combo'] == 5:
            m = sx8(st['move'])
            if 0 <= m <= 2:
                pedae(sound(row, 0x70, m), 0, 0x3F800000, 0, tables.rd32(0x8D2E3C + m * 4))
    E(1, CREATOR)
    if inp['creator_player'] & 1:
        E(12, CREATOR, 0x59, 0, 0x3F800000, 0, 0, 0)
    # --- evento de sonido silencioso (constructor real): registra la referencia del atacante, Add al grupo global, destructor al final
    E(15, CREATOR, LOCAL, CREATOR)
    E(13)
    E(14, GROUP, 1, ANY, ANY, ANY, ANY, ANY, CREATOR, 0x425C0000, ANY, *cpos, 0)
    mutate(4)
    cb = st['combo']
    if 8 <= cb <= 12:
        chance = 100
    elif cb == 4 and st['move'] == 4:
        chance = -1
    else:
        h = load(st['health'])
        d = sub(load(0x42C80000), h)
        chance = s32(ftol_low(d))
    E(17)
    mutate(5)
    if inp['mut_p1']:
        st['p1'] = [inp['mutp_x'], inp['mutp_y'], inp['mutp_z']]
    r16 = inp['rand_value'] & 0xFFFF
    roll = s32(ftol_low(mul(mul(('f', Fraction(r16), 0), load(0x38000000)), load(0x42C80000))))
    LAST.clear()
    LAST.update(chance=chance, roll=roll)
    if roll < chance:
        dirv = [store(sub(load(cpos[i]), load(vpos[i]))) for i in range(3)]   # None = NaN
        E(19, *[NANM if d is None else d for d in dirv])
        mutate(6)
        if inp['norm_mode']:
            dirv = [inp['norm_x'], inp['norm_y'], inp['norm_z']]
        E(18, VICTIM)
        if not (inp['alive1'] & 1):
            dirv = [0, 0, 0x40000000]
        amount = 8
        cb2 = st['combo']
        if 8 <= cb2 <= 12:
            amount = 0x10
            E(18, VICTIM)
            if inp['alive2'] & 1:
                dirv = [None if d is None else store(mul(load(d), load(0x3FC00000))) for d in dirv]
        E(20, FX, LOCAL, *st['p1'], LOCAL, *[NANM if d is None else d for d in dirv], amount, inp['brightness'])
    E(16, CREATOR, LOCAL, CREATOR)
    return result(VICTIM if damaged else 0)


def match(a, b):
    """a esperado (puede llevar comodines y NaN), b observado"""
    if len(a) != len(b):
        return False
    for x, y in zip(a, b):
        if x is ANY:
            continue
        if x is NANM:
            if not (isinstance(y, int) and (y & 0x7F800000) == 0x7F800000 and (y & 0x7FFFFF)):
                return False
            continue
        if x != y:
            return False
    return True


def check(inp, side_obs, tables):
    ev, eax, changes = model(inp, tables)
    obs = side_obs['observed']
    got = obs['events']
    if len(ev) != len(got) or not all(match(x, y) for x, y in zip(ev, got)):
        for i, (x, y) in enumerate(zip(ev, got)):
            if not match(x, y):
                return 'evento %d: esperado %s, observado %s' % (i, x, y)
        return 'numero de eventos: esperado %d (%s), observado %d (%s)' % (len(ev), ev[-1] if ev else None, len(got), got[-1] if got else None)
    if obs['result'] != eax:
        return 'EAX: esperado %#x, observado %#x' % (eax, obs['result'])
    for i, (exp, g) in enumerate(zip(changes, obs['changes'])):
        got_c = {off: val for off, val in g}
        if got_c != exp:
            return 'cambios de la arena %d: esperado %s, observado %s' % (i, exp, got_c)
    return None


def run(path):
    rows = [json.loads(line) for line in open(path, encoding='utf-8')]
    begin = [r for r in rows if r['event'] == 'begin'][0]
    tables = Tables(begin)
    bad = 0
    n = 0
    for r in rows:
        if r['event'] != 'comparison':
            continue
        n += 1
        for sname in ('original', 'candidate'):
            msg = check(r['input'], r[sname], tables)
            if msg:
                bad += 1
                if bad <= 10:
                    print('caso %d (familia %d) lado %s: %s' % (r['case'], r['input']['family'], sname, msg))
    print('casos %d, lados distintos del modelo: %d' % (n, bad))
    return bad


if __name__ == '__main__':
    sys.exit(1 if run(sys.argv[1]) else 0)
