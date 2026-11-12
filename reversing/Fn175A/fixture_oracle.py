"""Oraculo independiente (Python) de Fn175A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 4 funciones del lote 175A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE).
   0  0x552520  cuerpo del destructor de CQuadTreeNode<void*> (thiscall sin argumentos de pila, `ret`, marco SEH que solo desenrolla): esi = this+0x14, ebx = 4; por cada hijo [esi] (esi += 4): si no es nulo,
                ecx = hijo; call 0x552520 (este mismo cuerpo, recursion); ecx = [0xB745BC] (el pool, releido); push hijo; call 0x552BC0 (liberacion en el pool, thiscall `ret 4`); al final ecx = this+0x10;
                call 0x552400 (Flush de la lista simple). No escribe en memoria ni lee el nivel (+0x24).
   1  0x66D440  CPatrolRoute::Constructor (thiscall sin argumentos de pila, `ret`): [this] = 0; vector constructor 0x821D30(this+4, 0x28, 8, ctor 0x46AE30, dtor 0x46A380) (cada entrada: byte +0 = 0 y byte +0x18 = 0); despues
                bytes a cero en +4, +0x1C, +0x2C, +0x44, +0x54, +0x6C, +0x7C, +0x94, +0xA4, +0xBC, +0xCC, +0xE4, +0xF4, +0x10C, +0x11C, +0x134; devuelve this.
   2  0x66D4B0  CPatrolRoute::Assign (thiscall, un puntero en la pila, `ret 4`): n = [origen]; [this] = n; si n <= 0 (con signo) termina; para i = 0 mientras i < [this] (releido): copia los tres dwords de origen+0x144+12*i a
                this+0x144+12*i (leer y escribir de uno en uno), y las cadenas C (hasta el NUL incluido, byte a byte, sin limite) de origen+4+0x28*i a this+4+0x28*i y de origen+0x1C+0x28*i a this+0x1C+0x28*i.
   3  0x674930  CTaskComplexFollowPatrolRoute constructor (thiscall, cinco dwords de pila (a, ruta, b, c, d), `ret 14h`, marco SEH): call 0x61A3B0 (ecx = this); word [this+0x10] = a; [this] = 0x870114; word [this+0xC] = b;
                dword [this+0x20] &= ~2; eax = 0x41B810(0x1A4) (cdecl); si no es nulo: eax = 0x66D440(ecx = eax) (devuelve EAX) y si es nulo eax = 0; [this+0x1C] = eax; call 0x66D4B0(ecx = eax, ruta) SIN comprobar nulo;
                [this+0x14] = c; dword [this+0x20] |= 1 (releido); [this+0x18] = d; word [this+0x12] = 0; word [this+0xE] = 0; devuelve this.

Los falsos de los colaboradores anotan: 0x552400 [direccion, ECX (etiqueta)]; 0x552BC0 [direccion, ECX (pool, crudo), nodo (etiqueta)]; 0x61A3B0 [direccion, ECX (etiqueta)] (deja tres dwords del caso en +0, +4 y +8);
0x41B810 [direccion, tamano] (devuelve el nodo del caso o nulo); 0x66D440 [direccion, ECX (etiqueta), dword de [ECX]] (deja un dword del caso en [ECX]; devuelve ECX o un valor del caso); 0x66D4B0 [direccion, ECX (etiqueta),
argumento (etiqueta)].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CQuadTreeNode::DestructorBody', 'CPatrolRoute::Constructor', 'CPatrolRoute::Assign', 'CTaskComplexFollowPatrolRoute::ConstructorExe']
ENTRIES = (0x552520, 0x66D440, 0x66D4B0, 0x674930)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
FLUSH, POOLFREE, BASECTOR, NEWROUTE, ROUTECTOR, ROUTEASSIGN = 0x552400, 0x552BC0, 0x61A3B0, 0x41B810, 0x66D440, 0x66D4B0
DEFAULT_POOL = 0x00A00000      # solo para la autoprueba sin juego: el banco anota el valor real de [0xB745BC] en la entrada de cada caso

QUAD, PRCTOR, PRASSIGN, FPR = 'quad prctor prassign fpr'.split()
KIND = [QUAD, PRCTOR, PRASSIGN, FPR]
SPAN = {QUAD: 0x24, PRCTOR: 0x138, PRASSIGN: 0x200, FPR: 0x30}
SRCLEN = 0x200
QUADSLOT, QUADBASE = 0x50, 0x40


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
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
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


KEYS = {}
for _fn in range(4):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind == QUAD:
        _k += ['no_children', 'one_child', 'four_children', 'child_b', 'child_c', 'grandchildren', 'shared_child', 'depth3', 'list_head_set']
    elif _kind == PRCTOR:
        _k += ['spill']
    elif _kind == PRASSIGN:
        _k += ['count_zero', 'count_negative', 'count_1_to_8', 'count_over_8', 'src_in_b', 'src_in_c', 'src_same_route', 'src_in_a', 'long_names', 'short_names', 'spill']
    elif _kind == FPR:
        _k += ['spill', 'new_null', 'new_ok', 'route_b', 'route_c', 'route_raw', 'route_null', 'ctor_ret_node', 'ctor_ret_other', 'ctor_ret_null', 'flags_bit1_set', 'flags_bit1_clear', 'a_high_bits', 'b_high_bits']
    KEYS[_fn] = _k


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
    if a[0] + SPAN[KIND[fn]] > WIN:
        marks.append('spill')
    return marks


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret = [], None
    o = a[0]
    if k == QUAD:
        pool = c.get('pool', DEFAULT_POOL) & M32

        def slot_off(sl):
            return QUADBASE + a[8] + QUADSLOT * sl

        def child_addr(e):
            if e == 0:
                return 0
            return W.raw(1, slot_off(e - 1)) if e <= 8 else W.raw(2, slot_off(e - 9))

        A.wr(o + 0x10, 4, a[3])
        for j in range(4):
            A.wr(o + 0x14 + 4 * j, 4, child_addr(a[4 + j]))
        for sl in range(8):
            for j in range(4):
                B.wr(slot_off(sl) + 0x14 + 4 * j, 4, child_addr(a[16 + 4 * sl + j]))
        for sl in range(8):
            for j in range(4):
                Cc.wr(slot_off(sl) + 0x14 + 4 * j, 4, 0)
        W.snap()

        def dtor(node, depth):
            deepest = depth
            for j in range(4):
                child = W.rd(node + 0x14 + 4 * j, 4)
                if child:
                    deepest = max(deepest, dtor(child, depth + 1))
                    events.append([POOLFREE, pool, W.tagv(child)])
            events.append([FLUSH, W.tagv(node + 0x10)])
            return deepest

        depth = dtor(W.raw(0, o), 1)
        kids = [a[4 + j] for j in range(4)]
        nz = [e for e in kids if e]
        marks.append('no_children' if not nz else 'one_child' if len(nz) == 1 else 'four_children' if len(nz) == 4 else 'several_children')
        if any(1 <= e <= 8 for e in nz):
            marks.append('child_b')
        if any(e >= 9 for e in nz):
            marks.append('child_c')
        if len(set(nz)) < len(nz):
            marks.append('shared_child')
        if depth >= 3:
            marks.append('depth3')
        if depth >= 2 and any(1 <= e <= 8 for e in nz):
            marks.append('grandchildren')
        if a[3]:
            marks.append('list_head_set')
    elif k == PRCTOR:
        A.wr(o, 4, a[3])
        W.snap()
        A.wr(o, 4, 0)
        for i in range(8):
            A.wr(o + 4 + 0x28 * i, 1, 0)
            A.wr(o + 4 + 0x28 * i + 0x18, 1, 0)
        ret = W.tagv(W.raw(0, o))
    elif k == PRASSIGN:
        src_arena, src_off = a[17], a[18]
        # imagen de la ruta origen (como la prepara el banco): bytes de relleno sin ceros, un NUL por cada nombre y el contador
        image = bytearray(SRCLEN)
        for kk in range(SRCLEN):
            b = (rnd(a[4], kk >> 2) >> (8 * (kk & 3))) & 0xFF
            image[kk] = b if b else 1
        for j in range(24):
            start = 4 + 0x28 * (j >> 1) + (0x18 if j & 1 else 0)
            mod = 0x18 if (j & 1) else (0x30 if a[5] else 0x18)
            image[start + rnd((a[4] + 0x100) & M32, j) % mod] = 0
        image[0:4] = a[3].to_bytes(4, 'little')
        S = W.mem[src_arena]
        for kk in range(SRCLEN):
            S.wr(src_off + kk, 1, image[kk])
        W.snap()
        src = W.raw(src_arena, src_off)
        dst = W.raw(0, o)
        n = W.rd(src, 4)
        W.wr(dst, 4, n)
        sn = n - (1 << 32) if n & 0x80000000 else n
        if sn <= 0:
            marks.append('count_zero' if sn == 0 else 'count_negative')
        else:
            marks.append('count_1_to_8' if sn <= 8 else 'count_over_8')
            i = 0
            while True:
                for q in range(3):
                    W.wr(dst + 0x144 + 12 * i + 4 * q, 4, W.rd(src + 0x144 + 12 * i + 4 * q, 4))
                for off in (4, 0x1C):
                    p, q = src + off + 0x28 * i, dst + off + 0x28 * i
                    while True:
                        b = W.rd(p, 1)
                        W.wr(q, 1, b)
                        p += 1
                        q += 1
                        if b == 0:
                            break
                i += 1
                cur = W.rd(dst, 4)
                cur = cur - (1 << 32) if cur & 0x80000000 else cur
                if not i < cur:
                    break
        marks.append({0: 'src_in_a', 1: 'src_in_b', 2: 'src_in_c'}[src_arena])
        if src == dst:
            marks.append('src_same_route')
        marks.append('long_names' if a[5] else 'short_names')
    elif k == FPR:
        A.wr(o, 4, a[3])
        A.wr(o + 0x20, 4, a[4])
        route = {0: W.raw(1, a[7]), 1: W.raw(2, a[8]), 2: a[9], 3: 0}[a[6]]
        gnew = W.raw(1, a[18]) if a[17] else 0
        W.snap()
        this = W.raw(0, o)
        # constructor base falso: tres dwords del caso en +0, +4 y +8
        events.append([BASECTOR, W.tagv(this)])
        A.wr(o, 4, a[22])
        A.wr(o + 4, 4, a[23])
        A.wr(o + 8, 4, a[24])
        A.wr(o + 0x10, 2, a[5] & 0xFFFF)
        A.wr(o, 4, 0x870114)
        A.wr(o + 0xC, 2, a[10] & 0xFFFF)
        A.wr(o + 0x20, 4, A.rd(o + 0x20, 4) & ~2 & M32)
        events.append([NEWROUTE, 0x1A4])
        if gnew:
            marks.append('new_ok')
            events.append([ROUTECTOR, W.tagv(gnew), W.rd(gnew, 4)])
            W.wr(gnew, 4, a[25])
            mode = a[19]
            eax = {0: gnew, 1: W.raw(2, a[20]), 2: 0, 3: a[21]}[mode]
            marks.append({0: 'ctor_ret_node', 1: 'ctor_ret_other', 2: 'ctor_ret_null', 3: 'ctor_ret_other'}[mode])
        else:
            marks.append('new_null')
            eax = 0
        A.wr(o + 0x1C, 4, eax)
        events.append([ROUTEASSIGN, W.tagv(eax), W.tagv(route)])
        A.wr(o + 0x14, 4, a[11])
        A.wr(o + 0x20, 4, A.rd(o + 0x20, 4) | 1)
        A.wr(o + 0x18, 4, a[16])
        A.wr(o + 0x12, 2, 0)
        A.wr(o + 0xE, 2, 0)
        ret = W.tagv(this)
        marks.append({0: 'route_b', 1: 'route_c', 2: 'route_raw', 3: 'route_null'}[a[6]])
        marks.append('flags_bit1_set' if a[4] & 2 else 'flags_bit1_clear')
        if a[5] >> 16:
            marks.append('a_high_bits')
        if a[10] >> 16:
            marks.append('b_high_bits')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
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
