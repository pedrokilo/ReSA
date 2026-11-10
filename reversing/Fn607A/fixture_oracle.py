"""Oraculo independiente (Python) de Fn607A (CASO-607A90): reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de 0x607A90 (el escaneo de entidades de
CPedAcquaintanceScanner) y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas
(Fixtures.hpp) para conocer su imagen inicial, y la aritmetica x87 de 24 bits (fracciones exactas, exponente ampliado) de la resta y del producto escalar.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico). 0x607A90, thiscall, ECX = escaner; pila: ped, prioridad, CEntity** entidades, contador, int* salidaA, int* salidaB; `ret 18h`:
  *salidaA = 0; n = 0; para i en [0, contador) con signo (el bucle solo entra si contador > 0): e = entidades[i]; si e == 0 o !IsAlive(e) (0x5E0170, ECX = e) siguiente.
  pos(ped) = [ped+0x14] ? [ped+0x14]+0x30 : ped+4 (idem para e); delta = pos(e) - pos(ped) con la x87 (fld, fsub, tres fstp dword); 0x59C910 (Normalise, ECX = &delta) la reescribe;
  fwd = [ped+0x14]+0x10 (LEIDO DESPUES de la llamada); dot = ((z*fz) + (y*fy)) + (x*fx) en la x87 sin guardar; fcomp [0xC0B034]; si dot > umbral (ordenado) sigue; si no: sigue solo si ([ped+0x46C] | [e+0x46C]) & 0x100.
  bl = (prioridad == -1 && [e+0x598] == 6); si !bl: para id = 4..0: si prioridad == -1 o prioridad == id: GetAcquaintances (0x608970, ECX = ped+0x4E0, id) y GetPedFlag (0x608830, tipo) y bl |= (a & f) != 0 (el bucle para al acertar);
  si sigue sin acertar: LaRiotsActiveHere (0x441C10) y, si cierto, 0x603AF0(ped, e): si alguno falla, siguiente. Si [e+0x598] != 6: LaRiots y, si cierto, 0x603AF0(ped, e); si no acepta: 0x4684F0 (sin argumentos) y 0x6042B0
  (ECX = instancia; ped, lista {0x24, 0x25, 0x27, 0x28, resto de pila}, 5). Si acepta: tabla[n++] = e.
  Despues, acc = -1; por cada tabla[k]: si acc != 4: 0x5F1C40(ped, e, bandera = ni el ped ni e tienen 0x100 en +0x46C) y, si cierto, acc = 0x607560(ECX = escaner; ped, prioridad, acc, e, salidaA, salidaB). EAX = n.

Los falsos de los colaboradores anotan eventos [direccion, argumentos...] (punteros como etiquetas de arena); IsAlive, GetAcquaintances y GetPedFlag aplican la regla del EXE sobre la memoria; los demas devuelven la
secuencia del caso; la normalizacion reescribe el vector local y hace, segun el caso, una escritura lateral sobre la matriz del ped; 0x607560 escribe en salidaA y salidaB.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from fractions import Fraction

NAN = ('n',)
M32 = 0xFFFFFFFF
GUARD = 0x40
WINS = (0x1000, 0x8000, 0x1000, 0x1000)       # A: el ped; B: candidatos; C: escaner, instancia, tabla, celdas; D: matrices
DEFAULT_ARENAS = (0x30000000, 0x31000000, 0x32000000, 0x33000000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
NAMES = ['CPedAcquaintanceScanner::ScanEntities']
ENTRIES = (0x607A90,)
HDR = 12
CAND_WORDS = 10
THRESHOLD = 0xC0B034
EV_ALIVE, EV_NORM, EV_GETACQ, EV_GETFLAG, EV_RIOTS, EV_F603, EV_INSTANCE, EV_F6042, EV_CANT, EV_F607 = 0x5E0170, 0x59C910, 0x608970, 0x608830, 0x441C10, 0x603AF0, 0x4684F0, 0x6042B0, 0x5F1C40, 0x607560


# ------------------------------------------------------------------------------------------------ aritmetica x87 (24 bits de mantisa, exponente ampliado)
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
    """fstp dword: devuelve los bits del float, o None si es NaN"""
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


BIG = Fraction(10) ** 400
NAN_BITS = 0xFFC00000       # el NaN indefinido de la x87 (infinito menos infinito, cero por infinito)


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / je): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def sub_bits(a_bits, b_bits):
    """fld a; fsub b; fstp dword -> bits. Con un NaN en un operando el resultado es ese NaN silenciado (los casos nunca llevan dos NaN en la misma resta)"""
    na, nb = is_nan_bits(a_bits), is_nan_bits(b_bits)
    if na and nb:
        # dos NaN (solo cuando el candidato es el propio ped: mismos bits): gana la carga util mayor tras silenciarlos; con las mismas cargas y signos distintos no esta definido (no se genera)
        qa, qb = a_bits | 0x00400000, b_bits | 0x00400000
        if (qa & 0x7FFFFF) != (qb & 0x7FFFFF):
            return qa if (qa & 0x7FFFFF) > (qb & 0x7FFFFF) else qb
        assert qa == qb, 'dos NaN con la misma carga y signos distintos'
        return qa
    if na or nb:
        return (a_bits if na else b_bits) | 0x00400000
    r = store(sub(load(a_bits), load(b_bits)))
    return NAN_BITS if r is None else r


def dot_cmp(v, f, thr):
    """((z*fz) + (y*fy)) + (x*fx) sin guardar contra el umbral: (mayor, igual); ambos falsos con NaN (comparacion ordenada)"""
    V = [load(x) for x in v]
    F = [load(x) for x in f]
    d = add(add(mul(V[2], F[2]), mul(V[1], F[1])), mul(V[0], F[0]))
    T = load(thr)
    return gt(d, T), (d != NAN and T != NAN and not gt(d, T) and not gt(T, d))


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
        self.mems = [Mem(a[2], a[12 + i], WINS[i]) for i in range(4)]

    def addr(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, ad, n):
        for i, b in enumerate(self.bases):
            if b <= ad and ad + n <= b + self.mems[i].size:
                return i, ad - b - GUARD
        raise KeyError('direccion fuera de las arenas: %#x' % ad)

    def rd(self, ad, n):
        i, off = self.locate(ad, n)
        return self.mems[i].rd(off, n)

    def wr(self, ad, n, v):
        i, off = self.locate(ad, n)
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


KEYS = {0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'count_neg', 'count_zero', 'count_pos', 'count_gt16', 'entry_null', 'entry_dead', 'entry_self', 'in_view', 'not_in_view_skip',
            'not_in_view_vehicle', 'dot_tie', 'prio_all', 'prio_one', 'prio_other', 'cop_direct', 'acq_hit', 'riots_false', 'riots_603_false', 'riots_603_true', 'accepted_cop', 'accepted_riots', 'accepted_dm', 'rejected_dm',
            'cant_true', 'cant_false', 'acc4_skip', 'f607_called', 'side1', 'side2', 'nan_delta', 'inf_delta', 'cand_no_matrix', 'table_16']}


def expected(c, mut=frozenset()):
    """`mut`: numeros de las mutaciones de mutate.py (1..6) cuyo comportamiento se modela (solo para la prueba negativa: neg_summary.py); vacio = el EXE"""
    a, w = c['a'], c['w']
    bases = c.get('arena') or DEFAULT_ARENAS
    W = World(a, bases)
    A, B, Cc, D = W.mems
    nc, ns = a[23], a[24]
    seq_norm = HDR + CAND_WORDS * nc + ns
    seq_riot = seq_norm + 3 * ns
    seq_603 = seq_riot + 2 * ns
    seq_6042 = seq_603 + 2 * ns
    seq_cant = seq_6042 + ns
    seq_607 = seq_cant + ns
    ped = W.addr(0, a[0])
    ped_mat, alt_mat = W.addr(3, a[29]), W.addr(3, a[30])
    scanner, instance = W.addr(2, a[9]), W.addr(2, a[10])
    entities, out_a, out_b = W.addr(2, a[6]), W.addr(2, a[7]), W.addr(2, a[8])
    marks = ['kind%d' % a[1], 'fill%d' % a[2]]
    # ---- preparacion del caso (mismo orden que LiveComparison.hpp)
    A.wr(a[0] + 0x14, 4, ped_mat)
    A.wr(a[0] + 0x46C, 4, a[16])
    for k in range(5):
        A.wr(a[0] + 0x4E0 + 4 * k, 4, a[18 + k])
    A.wr(a[0] + 0x530, 4, a[32])
    A.wr(a[0] + 0x598, 4, a[17])
    for i in range(3):
        D.wr(a[29] + 0x10 + 4 * i, 4, w[i])
        D.wr(a[29] + 0x30 + 4 * i, 4, w[3 + i])
        D.wr(a[30] + 0x10 + 4 * i, 4, w[6 + i])
        D.wr(a[30] + 0x30 + 4 * i, 4, w[9 + i])
    cand_obj = []
    for j in range(nc):
        r = w[HDR + CAND_WORDS * j: HDR + CAND_WORDS * (j + 1)]
        cand_obj.append(W.addr(1, r[0]))
        B.wr(r[0] + 0x46C, 4, r[7])
        B.wr(r[0] + 0x530, 4, r[6])
        B.wr(r[0] + 0x598, 4, r[8])
        if r[4]:
            B.wr(r[0] + 0x14, 4, W.addr(3, r[5]))
            for i in range(3):
                D.wr(r[5] + 0x30 + 4 * i, 4, r[1 + i])
        else:
            B.wr(r[0] + 0x14, 4, 0)
            for i in range(3):
                B.wr(r[0] + 4 + 4 * i, 4, r[1 + i])
    for i in range(ns):
        e = w[HDR + CAND_WORDS * nc + i]
        Cc.wr(a[6] + 4 * i, 4, 0 if e == 0xFFFFFFFF else (ped if e == nc else cand_obj[e]))
    Cc.wr(a[7], 4, a[11])
    Cc.wr(a[8], 4, a[31])
    W.snap()
    # ---- la funcion
    events = []
    thr = a[5]
    prio = s32(a[3])
    count = s32(a[4])
    marks.append('count_neg' if count < 0 else ('count_zero' if count == 0 else 'count_pos'))
    if count > 16:
        marks.append('count_gt16')
    marks.append('prio_all' if prio == -1 else ('prio_one' if 0 <= prio <= 4 else 'prio_other'))
    counters = {'norm': 0, 'riot': 0, '603': 0, '6042': 0, 'cant': 0, '607': 0}

    def seqbool(base, length, key):
        i = counters[key]
        counters[key] += 1
        return ((w[base + i] if i < length else 0) & 0xFF) != 0

    W.wr(out_a, 4, 0)
    table = []
    for i in range(max(count, 0)):
        ptr = W.rd(entities + 4 * i, 4)
        if ptr == 0:
            marks.append('entry_null')
            continue
        if ptr == ped:
            marks.append('entry_self')
        state = W.rd(ptr + 0x530, 4)
        events.append([EV_ALIVE, W.tagv(ptr), state])
        if state in (0x36, 0x37):
            marks.append('entry_dead')
            continue
        pm = W.rd(ped + 0x14, 4)
        ppos = pm + 0x30 if pm else ped + 4
        cm = W.rd(ptr + 0x14, 4)
        cpos = cm + 0x30 if cm else ptr + 4
        if not cm:
            marks.append('cand_no_matrix')
        delta = [sub_bits(W.rd(ppos + 4 * k, 4), W.rd(cpos + 4 * k, 4)) if (3 in mut and k == 0) else sub_bits(W.rd(cpos + 4 * k, 4), W.rd(ppos + 4 * k, 4)) for k in range(3)]
        if any(is_nan_bits(d) for d in delta):
            marks.append('nan_delta')
        if any((d & 0x7FFFFFFF) == 0x7F800000 for d in delta):
            marks.append('inf_delta')
        events.append([EV_NORM] + delta)
        k = counters['norm']
        counters['norm'] += 1
        vec = [w[seq_norm + 3 * k + comp] if 3 * k + comp < 3 * ns else 0 for comp in range(3)]
        if a[25] and k == a[26]:
            if a[25] == 1:
                W.wr(W.rd(ped + 0x14, 4) + 0x10, 4, a[27])
                marks.append('side1')
            else:
                W.wr(ped + 0x14, 4, alt_mat)
                marks.append('side2')
        fm = W.rd(ped + 0x14, 4)
        fwd = [W.rd(fm + 0x10 + 4 * q, 4) for q in range(3)]
        pflags, cflags = W.rd(ped + 0x46C, 4), W.rd(ptr + 0x46C, 4)
        greater, tie = dot_cmp(vec, fwd, thr)
        if tie:
            marks.append('dot_tie')
        if greater or (6 in mut and tie):
            marks.append('in_view')
        elif bool((pflags | cflags) & 0x100) != (2 in mut):
            marks.append('not_in_view_vehicle')
        else:
            marks.append('not_in_view_skip')
            continue
        ctype = W.rd(ptr + 0x598, 4)
        related = prio == -1 and ctype == 6
        if related:
            marks.append('cop_direct')
        else:
            idv = 4
            while idv >= 0 and not related:
                if (prio == -1 and 1 not in mut) or prio == idv:
                    events.append([EV_GETACQ, W.tagv(ped + 0x4E0), idv])
                    acq = W.rd(ped + 0x4E0 + 4 * idv, 4)
                    events.append([EV_GETFLAG, ctype])
                    related = (acq & ped_flag(ctype)) != 0
                idv -= 1
            if related:
                marks.append('acq_hit')
            else:
                events.append([EV_RIOTS])
                if not seqbool(seq_riot, 2 * ns, 'riot'):
                    marks.append('riots_false')
                    continue
                events.append([EV_F603, W.tagv(ped), W.tagv(ptr)])
                if not seqbool(seq_603, 2 * ns, '603'):
                    marks.append('riots_603_false')
                    continue
                marks.append('riots_603_true')
        if ctype != 6:
            events.append([EV_RIOTS])
            accepted = False
            if seqbool(seq_riot, 2 * ns, 'riot'):
                events.append([EV_F603, W.tagv(ped), W.tagv(ptr)])
                accepted = seqbool(seq_603, 2 * ns, '603')
            if accepted:
                marks.append('accepted_riots')
            else:
                events.append([EV_INSTANCE])
                events.append([EV_F6042, W.tagv(instance), W.tagv(ped), 5, 0x24, 0x25, 0x27, 0x29 if 5 in mut else 0x28])
                accepted = seqbool(seq_6042, ns, '6042')
                if not accepted:
                    marks.append('rejected_dm')
                    continue
                marks.append('accepted_dm')
        else:
            marks.append('accepted_cop')
        table.append(ptr)
    if len(table) == 16:
        marks.append('table_16')
    acc = 0xFFFFFFFF
    for ptr in table:
        if acc == (3 if 4 in mut else 4):
            marks.append('acc4_skip')
            continue
        flag = 0 if ((W.rd(ped + 0x46C, 4) | W.rd(ptr + 0x46C, 4)) & 0x100) else 1
        events.append([EV_CANT, W.tagv(ped), W.tagv(ptr), flag])
        if seqbool(seq_cant, ns, 'cant'):
            marks.append('cant_true')
            events.append([EV_F607, W.tagv(scanner), W.tagv(ped), a[3], acc, W.tagv(ptr), W.tagv(out_a), W.tagv(out_b)])
            k = counters['607']
            counters['607'] += 1
            vals = [w[seq_607 + 3 * k + q] if 3 * k + q < 3 * ns else 0 for q in range(3)]
            W.wr(out_a, 4, vals[1])
            W.wr(out_b, 4, vals[2])
            acc = vals[0]
            marks.append('f607_called')
        else:
            marks.append('cant_false')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': len(table), 'marks': marks}


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
