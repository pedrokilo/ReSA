"""Oraculo independiente (Python) de Fn607B (COLAB-607A): reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las tres funciones que 0x607A90
(CPedAcquaintanceScanner::ScanEntities) llama y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO
pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):

0x603AF0 CanJoinRiotAgainst(ped, otro), cdecl, `ret`, devuelve AL:
  tipo = [ped+0x598]; si tipo es 6, 0x12 o 0x13: falso. IsPlayer(ped) (0x5DF8F0, ECX = ped): si cierto, falso. Si el byte [ped+0x484] == 2: falso. IsPlayer(otro): si cierto, devuelve (Respects(ECX = [ped+0x47C],
  otro) == 0). Si no: g = GetPedsGroup(otro) (0x5F7E80, cdecl); si g != 0: m = g + 8; l = GetLeader(m) (0x5F69A0); si l != 0: l = GetLeader(m) otra vez; si IsPlayer(l): edi = [ped+0x47C]; l = GetLeader(m) por
  tercera vez; devuelve (Respects(edi, l) == 0). Si no: si IsPedTypeGang(tipo) (0x5FE9C0, REAL: tipos con signo entre 7 y 16) y IsPedTypeGang([otro+0x598]) y tipo == [otro+0x598]: falso; verdadero en el resto.

0x6042B0 HasResponseToAnyEvent(ped, eventos, contador), thiscall (ECX = el CDecisionMakerTypes), `ret 0Ch`, devuelve AL:
  dm = [[ped+0x47C]+0xB4]; AL = 0; para i en [0, contador) con signo mientras AL == 0: e = eventos[i]; di = [ECX + 0xC034 + e*4]; maker = ECX + 0xD4EC si dm == -2; ECX + (0xCB50 si el byte [ped+0x484] == 2 si no
  0xC1B4) si dm == -1; ECX + 4 + dm * 0x99C en el resto; fila = maker + di * 60; AL = 1 si alguno de los seis dwords de la fila es distinto de 200.

0x607560 FindAcquaintance(ped, prioridad, actual, candidato, salidaA, salidaB), thiscall (ECX = el escaner), `ret 18h`, devuelve EAX:
  v = 0x605550(ECX = [ped+0x47C], candidato, 0) (float en ST0, guardado como float); para id = 4..0: si id == actual: devuelve -1. Si prioridad != -1: si prioridad != id, siguiente; si no (o con prioridad == -1 e id != 2
  o tipo del candidato [candidato+0x598] != 6) comprueba la relacion: a = GetAcquaintances(ECX = ped+0x4E0, id) y f = GetPedFlag(tipo); si (a & f) == 0: LaRiotsActiveHere (0x441C10) falso -> siguiente; 0x603AF0(ped,
  candidato) falso -> siguiente. Visibilidad: si v > 0 sigue (ordenado); si no, con id != 4 siguiente, con v == 0 (ordenado) siguiente, con NaN o negativa sigue. *salidaA = candidato; *salidaB = id; si prioridad != -1
  devuelve la prioridad. 0x606BA0(ECX = escaner, ped, id, candidato) -> AL; intervalo = [0x8D2360] si id == 4 y v < 0 (ordenado), si no [0x8D235C]; escaner: +0 = [0xB7CB84], +4 = intervalo, byte +8 = 1; si AL != 0
  devuelve *salidaB (= id); si no, siguiente. Al terminar los ids: devuelve -1.

Los falsos de los colaboradores anotan eventos [direccion, argumentos...] (punteros como etiquetas de arena); GetAcquaintances y GetPedFlag aplican la regla del EXE sobre la memoria; los demas devuelven la
secuencia del caso.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
WINS = (0x1000, 0x1000, 0x1000, 0x18000)       # A: el ped; B: el otro / candidato; C: escaner, inteligencia, lista, grupos, celdas; D: el CDecisionMakerTypes (funcion 1)
DEFAULT_ARENAS = (0x30000000, 0x31000000, 0x32000000, 0x34000000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
NAMES = ['CPedAcquaintanceScanner::CanJoinRiotAgainst', 'CDecisionMakerTypes::HasResponseToAnyEvent', 'CPedAcquaintanceScanner::FindAcquaintance']
ENTRIES = (0x603AF0, 0x6042B0, 0x607560)
EV_ISPLAYER, EV_RESPECTS, EV_GETGROUP, EV_GETLEADER = 0x5DF8F0, 0x601C90, 0x5F7E80, 0x5F69A0
EV_VIS, EV_GETACQ, EV_GETFLAG, EV_RIOTS, EV_F603, EV_F606 = 0x605550, 0x608970, 0x608830, 0x441C10, 0x603AF0, 0x606BA0
TIMER, INTERVALS = 0xB7CB84, 0x8D235C


# ------------------------------------------------------------------------------------------------ PRNG y memoria de las arenas (identicos a Fixtures.hpp)
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def prng(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed, win):
        self.kind, self.seed, self.size, self.m, self.r = kind, seed, GUARD + win + GUARD, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = prng(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales: lectura y escritura por direccion"""

    def __init__(self, a, bases):
        self.bases = bases
        self.mems = [Mem(a[1], a[2 + i], WINS[i]) for i in range(4)]

    def addr(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, ad, n):
        for i, b in enumerate(self.bases):
            if b <= ad and ad + n <= b + self.mems[i].size:
                return i, ad - b - GUARD
        raise KeyError('direccion fuera de las arenas: %#x' % ad)

    def rd(self, ad, n):
        i, off = self.locate(ad & M32, n)
        return self.mems[i].rd(off, n)

    def wr(self, ad, n, v):
        i, off = self.locate(ad & M32, n)
        self.mems[i].wr(off, n, v)

    def tagv(self, ad):
        """etiqueta de un puntero: (arena, desplazamiento) si cae en una arena; si no, en crudo"""
        ad &= M32
        if ad == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= ad < b + self.mems[i].size:
                return 0x10000000 + (i << 20) + (ad - b)
        return ad

    def snap(self):
        for m in self.mems:
            m.snap()


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def ped_flag(t):
    return (1 << (t & 31)) if s32(t) < 32 else 0


def is_gang(t):
    return 7 <= s32(t) <= 16


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'type_cop', 'type_medic', 'type_fireman', 'player_ped', 'created_mission', 'other_player', 'respects_true', 'respects_false',
        'group_null', 'group_leader_null', 'group_leader_notplayer', 'group_leader_player', 'gang_pair_same', 'gang_pair_diff', 'gang_other_not', 'ped_not_gang', 'ret_true', 'ret_false', 'leader_respects_true', 'leader_respects_false'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'count_neg', 'count_zero', 'count_pos', 'count_gt16', 'dm_player', 'dm_mission', 'dm_random', 'dm_index', 'dm_beyond', 'found', 'notfound',
        'found_first', 'found_late', 'row_all200', 'row_one', 'row_unwritten', 'evt_neg', 'evt_beyond', 'last_position_only'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'cur_stop', 'cur_stop_first', 'prio_all', 'prio_one', 'prio_other', 'cop_direct', 'acq_hit', 'riots_false', 'f603_false', 'f603_true',
        'vis_pos', 'vis_neg', 'vis_zero', 'vis_nan', 'vis_neg_id4', 'vis_skip_id', 'written', 'return_prio', 'f606_called', 'f606_true', 'f606_false', 'interval_2360', 'interval_235c', 'ret_minus1', 'ret_id'],
}


def expected(c, mut=frozenset()):
    """`mut`: numeros de las mutaciones de mutate.py (1..6) cuyo comportamiento se modela (solo para la prueba negativa: neg_summary.py); vacio = el EXE"""
    fn = c['fn']
    a, w = c['a'], c['w']
    bases = c.get('arena') or DEFAULT_ARENAS
    W = World(a, bases)
    A, B, Cc, D = W.mems
    marks = ['kind%d' % a[0], 'fill%d' % a[1]]
    events = []
    ped = W.addr(0, a[6])
    counters = {}

    def seqword(base, length, key):
        i = counters.get(key, 0)
        counters[key] = i + 1
        return w[base + i] if i < length else 0

    def seqbool(base, length, key):
        return (seqword(base, length, key) & 0xFF) != 0

    if fn == 0:
        other = W.addr(1, a[7])
        A.wr(a[6] + 0x598, 4, a[8])
        B.wr(a[7] + 0x598, 4, a[9])
        A.wr(a[6] + 0x484, 1, a[10])
        A.wr(a[6] + 0x47C, 4, W.addr(2, a[12]) if a[11] == 0 else a[13])
        group_addr, leader_addr = W.addr(2, a[14]), W.addr(2, a[15])
        W.snap()

        def is_player(ptr):
            events.append([EV_ISPLAYER, W.tagv(ptr)])
            return seqbool(0, 4, 'player')

        def respects(arg):
            intel = W.rd(ped + 0x47C, 4)
            events.append([EV_RESPECTS, W.tagv(intel), W.tagv(arg)])
            return seqbool(7, 1, 'respects')

        def get_leader(m):
            events.append([EV_GETLEADER, W.tagv(m)])
            code = seqword(4, 3, 'leader')
            return {1: ped, 2: other, 3: leader_addr, 4: a[16]}.get(code, 0)

        ret = True
        ptype = W.rd(ped + 0x598, 4)
        if ptype == 6:
            marks.append('type_cop')
            ret = False
        elif ptype == 0x12:
            marks.append('type_medic')
            ret = False
        elif ptype == 0x13 and 1 not in mut:
            marks.append('type_fireman')
            ret = False
        elif is_player(ped):
            marks.append('player_ped')
            ret = False
        elif W.rd(ped + 0x484, 1) == 2:
            marks.append('created_mission')
            ret = False
        elif is_player(other):
            marks.append('other_player')
            r = respects(other)
            marks.append('respects_true' if r else 'respects_false')
            ret = r if 4 in mut else not r
        else:
            events.append([EV_GETGROUP, W.tagv(other)])
            g = group_addr if seqword(8, 1, 'group') == 1 else 0
            done = False
            if g == 0:
                marks.append('group_null')
            else:
                m = (g + 8) & M32
                l = get_leader(m)
                if l == 0:
                    marks.append('group_leader_null')
                else:
                    l = get_leader(m)
                    if not is_player(l):
                        marks.append('group_leader_notplayer')
                    else:
                        marks.append('group_leader_player')
                        l = get_leader(m)
                        r = respects(l)
                        marks.append('leader_respects_true' if r else 'leader_respects_false')
                        ret = not r
                        done = True
            if not done:
                otype = W.rd(other + 0x598, 4)
                if not is_gang(ptype):
                    marks.append('ped_not_gang')
                elif not is_gang(otype):
                    marks.append('gang_other_not')
                elif ptype == otype:
                    marks.append('gang_pair_same')
                    ret = False
                else:
                    marks.append('gang_pair_diff')
        marks.append('ret_true' if ret else 'ret_false')
        return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': 1 if ret else 0, 'marks': marks}

    if fn == 1:
        intel_addr = W.addr(2, a[7])
        A.wr(a[6] + 0x47C, 4, intel_addr)
        A.wr(a[6] + 0x484, 1, a[11])
        Cc.wr(a[7] + 0xB4, 4, a[10])
        n = a[13]
        for j in range(n):
            r = w[6 * j: 6 * j + 6]
            Cc.wr(a[8] + 4 * j, 4, r[0])
            D.wr(a[9] + 0xC034 + 4 * s32(r[0]), 4, r[1])
            if r[3] != 2:
                for k in range(6):
                    D.wr(r[2] + 4 * k, 4, 200)
                if r[3] == 1:
                    D.wr(r[2] + 4 * r[4], 4, r[5])
        W.snap()
        this = W.addr(3, a[9])
        lst = W.addr(2, a[8])
        count = s32(a[12])
        marks.append('count_neg' if count < 0 else ('count_zero' if count == 0 else 'count_pos'))
        if count > 16:
            marks.append('count_gt16')
        dm = s32(W.rd(W.rd(ped + 0x47C, 4) + 0xB4, 4))
        marks.append('dm_player' if dm == -2 else ('dm_mission' if dm == -1 and W.rd(ped + 0x484, 1) == 2 else ('dm_random' if dm == -1 else ('dm_index' if dm < 20 else 'dm_beyond'))))
        found = False
        i = 0
        while i < count and not found:
            evt = s32(W.rd(lst + 4 * i, 4))
            if evt < 0:
                marks.append('evt_neg')
            elif evt >= 96:
                marks.append('evt_beyond')
            di = s32(W.rd(this + 0xC034 + 4 * evt, 4))
            if dm == -2:
                maker = this + 0xD4EC
            elif dm == -1:
                created2 = W.rd(ped + 0x484, 1) == 2
                if 5 in mut:
                    created2 = not created2
                maker = this + (0xCB50 if created2 else 0xC1B4)
            else:
                maker = this + 4 + dm * 0x99C
            row = (maker + di * 60) & M32
            vals = [W.rd(row + 4 * k, 4) for k in range(5 if 2 in mut else 6)]
            hit = any(v != 200 for v in vals)
            if hit:
                found = True
                marks.append('found_first' if i == 0 else 'found_late')
                if all(v == 200 for v in vals[:5]) and not (2 in mut) and W.rd(row + 20, 4) != 200:
                    marks.append('last_position_only')
            i += 1
        # clase de las filas leidas (informativa para la cobertura): solo la primera
        if n and a[13]:
            kind0 = w[3]
            marks.append('row_all200' if kind0 == 0 else ('row_one' if kind0 == 1 else 'row_unwritten'))
        marks.append('found' if found else 'notfound')
        return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': 1 if found else 0, 'marks': marks}

    # ---- funcion 2: FindAcquaintance
    cand = W.addr(1, a[7])
    scanner, out_a, out_b = W.addr(2, a[8]), W.addr(2, a[9]), W.addr(2, a[10])
    intel_addr = W.addr(2, a[11])
    A.wr(a[6] + 0x47C, 4, intel_addr)
    for k in range(5):
        A.wr(a[6] + 0x4E0 + 4 * k, 4, a[15 + k])
    B.wr(a[7] + 0x598, 4, a[14])
    Cc.wr(a[8] + 0, 4, a[25])
    Cc.wr(a[8] + 4, 4, a[26])
    Cc.wr(a[8] + 8, 1, a[27] & 0xFF)
    Cc.wr(a[8] + 9, 1, (a[27] >> 8) & 0xFF)
    Cc.wr(a[9], 4, a[23])
    Cc.wr(a[10], 4, a[24])
    W.snap()
    prio, cur = s32(a[12]), s32(a[13])
    ctype = W.rd(cand + 0x598, 4)
    marks.append('prio_all' if prio == -1 else ('prio_one' if 0 <= prio <= 4 else 'prio_other'))
    events.append([EV_VIS, W.tagv(W.rd(ped + 0x47C, 4)), W.tagv(cand), 0])
    vis = f32(w[0])
    if vis != vis:
        marks.append('vis_nan')
    elif vis == 0:
        marks.append('vis_zero')
    elif vis > 0:
        marks.append('vis_pos')
    else:
        marks.append('vis_neg')
    ret = 0xFFFFFFFF
    idv = 4
    while idv >= 0:
        if idv == cur:
            marks.append('cur_stop_first' if idv == 4 else 'cur_stop')
            ret = 0xFFFFFFFF
            break
        if prio != -1 and prio != idv:
            idv -= 1
            continue
        if prio == -1 and idv == 2 and ctype == 6:
            marks.append('cop_direct')
        else:
            events.append([EV_GETACQ, W.tagv(ped + 0x4E0), idv])
            acq = W.rd(ped + 0x4E0 + 4 * idv, 4)
            events.append([EV_GETFLAG, ctype])
            related = (acq & ped_flag(ctype)) != 0
            if related:
                marks.append('acq_hit')
            else:
                events.append([EV_RIOTS])
                if not seqbool(1, 5, 'riot'):
                    marks.append('riots_false')
                    idv -= 1
                    continue
                events.append([EV_F603, W.tagv(ped), W.tagv(cand)])
                if not seqbool(6, 5, '603'):
                    marks.append('f603_false')
                    idv -= 1
                    continue
                marks.append('f603_true')
        gt = vis > 0
        if not gt:
            skip_id = idv != (3 if 3 in mut else 4)
            if skip_id or vis == 0:
                marks.append('vis_skip_id' if skip_id else 'vis_zero')
                idv -= 1
                continue
        W.wr(out_a, 4, cand)
        W.wr(out_b, 4, idv)
        marks.append('written')
        if prio != -1:
            marks.append('return_prio')
            ret = a[12]
            break
        events.append([EV_F606, W.tagv(scanner), W.tagv(ped), idv, W.tagv(cand)])
        created = seqbool(11, 5, '606')
        marks.append('f606_called')
        neg = vis > 0 if 6 in mut else (vis < 0)
        if idv == 4 and neg:
            interval = a[22]
            marks.append('interval_2360')
            marks.append('vis_neg_id4')
        else:
            interval = a[21]
            marks.append('interval_235c')
        W.wr(scanner + 0, 4, a[20])
        W.wr(scanner + 4, 4, interval)
        W.wr(scanner + 8, 1, 1)
        if created:
            marks.append('f606_true')
            marks.append('ret_id')
            ret = W.rd(out_b, 4)
            break
        marks.append('f606_false')
        idv -= 1
    if ret == 0xFFFFFFFF:
        marks.append('ret_minus1')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret & M32, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        n = next((i for i, (x, y) in enumerate(zip(o['events'], e['events'])) if x != y), min(len(o['events']), len(e['events'])))
        return 'eventos (n=%d/%d, primero distinto %d) %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 2], e['events'][n:n + 2])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
