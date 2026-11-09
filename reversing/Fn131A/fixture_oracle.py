"""Oraculo independiente (Python) de Fn131A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las cuatro funciones del lote 131A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial y toma de la fila `begin` las direcciones base de las tres arenas (los punteros de las listas son direcciones absolutas).

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x5DE920  CAEPedAudioEntity::Destructor (thiscall, sin argumentos, `ret`, marco SEH)
       [this] = 0x86C2A8; twin = this + 0xA8 (CAETwinLoopSoundEntity, destructor 0x4F2B80 en linea): [twin] = 0x85F438; si el WORD [twin+0x88] != 0: para +0xA0 y luego +0xA4: p = [twin+off]; si p != 0:
       StopSoundAndForget (0x4EF850, ecx = p) y [twin+off] = 0; despues WORD [twin+0x88] = 0; [twin] = 0x859988; ~CAESound (0x4EF660, ecx = twin+8); y de vuelta en el destructor:
       [this] = 0x859988; ~CAESound (0x4EF660, ecx = this+8).
  1  0x563130  CSector::Destructor (thiscall, `ret`, marco SEH): Flush de la lista doble en this+4 (0x552470) y despues Flush de la lista simple en this+0 (0x552400).
  2  0x552400  Flush de lista simple (thiscall, ecx = &cabeza, `ret`): mientras haya cabeza: cur = cabeza; next = [cur+4] (leido ANTES de desenlazar); si cabeza == cur: cabeza = next; si no, busca cur desde la
       cabeza (x = [x+4] hasta x.next == cur) y pone [x+4] = [cur+4]; libera en el pool [0xB74484]: idx = ((cur - [pool]) >> 3) (aritmetico, 32 bits), byte [[pool+4] + idx] |= 0x80 y, si idx < dword [pool+0xC]
       (con signo), [pool+0xC] = idx.
  3  0x552470  Flush de lista doble (thiscall, ecx = &cabeza, `ret`): mientras haya cabeza: cur = cabeza; next = [cur+4]; si cabeza == cur: cabeza = next; prev = [cur+8]: si prev != 0: [prev+4] = [cur+4];
       n = [cur+4]: si n != 0: [n+8] = [cur+8]; libera en el pool [0xB74488]: idx = (cur - [pool]) / 12 (con signo, truncado hacia cero), byte [[pool+4] + idx] |= 0x80 y, si idx < dword [pool+0xC]
       (con signo), [pool+0xC] = idx.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CAEPedAudioEntity::Destructor', 'CSector::Destructor', 'CPtrNodeSingleLinkPool::Flush', 'CPtrNodeDoubleLinkPool::Flush']
ENTRIES = (0x5DE920, 0x563130, 0x552400, 0x552470)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
POOL_S, POOL_D = 0xE00, 0xE40      # pools falsos en la arena C (desplazamiento)
DEFAULT_BASES = (0x10000000, 0x20000000, 0x30000000)   # solo para la autoprueba sin juego (las marcas no dependen de ellas)


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


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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


class Space:
    """las tres arenas por direccion absoluta; un acceso fuera de ellas es un fallo del fixture (no modelado)"""

    def __init__(self, mems, bases):
        self.mems, self.bases = mems, bases

    def loc(self, addr, n):
        addr &= M32
        for k, b in enumerate(self.bases):
            if b <= addr and addr + n <= b + N:
                return self.mems[k], addr - b - GUARD
        raise RuntimeError('acceso no modelado a 0x%08x' % addr)

    def rd(self, addr, n):
        m, o = self.loc(addr, n)
        return m.rd(o, n)

    def wr(self, addr, n, v):
        m, o = self.loc(addr, n)
        m.wr(o, n, v)

    def inside(self, addr, n=4):
        addr &= M32
        return any(b <= addr and addr + n <= b + N for b in self.bases)

    def tag(self, addr):
        """etiqueta de un puntero dentro de una arena: 0x10000000 + (indice << 20) + guarda + desplazamiento; si no, el valor crudo"""
        addr &= M32
        if not addr:
            return 0
        for k, b in enumerate(self.bases):
            if b <= addr < b + N:
                return 0x10000000 + (k << 20) + (addr - b)
        return addr

    def safe_bits(self, addr):
        return self.rd(addr, 4) if self.inside(addr, 4) else 0xDEAD0000


def ptr_of(spec, bases):
    v = spec & 0x0FFFFFFF
    t = spec >> 28
    if t == 1:
        return bases[1] + GUARD + v
    if t == 2:
        return bases[2] + GUARD + v
    if t == 3:
        return bases[0] + GUARD + v
    if t == 4:
        return v
    return 0


KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'flag_zero', 'flag_set', 'flag_upper_only', 'stop_none', 'stop_first_only', 'stop_second_only', 'stop_both', 'same_ptr',
        'ptr_skipped_flag_zero', 'ptr_raw', 'ptr_in_self', 'this_unaligned', 'spill'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'single_empty', 'single_1', 'single_many', 'double_empty', 'double_1', 'double_many', 'ff_lowered_s', 'ff_kept_s', 'ff_lowered_d',
        'ff_kept_d', 'idx_neg_s', 'idx_neg_d', 'diff_nonmultiple_s', 'diff_nonmultiple_d', 'prev_nonnull', 'head_unaligned'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'single_empty', 'single_1', 'single_many', 'ff_lowered_s', 'ff_kept_s', 'idx_neg_s', 'diff_nonmultiple_s', 'head_unaligned'],
    3: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'double_empty', 'double_1', 'double_many', 'ff_lowered_d', 'ff_kept_d', 'idx_neg_d', 'diff_nonmultiple_d', 'prev_nonnull',
        'head_unaligned'],
}


def expected(c, bases=DEFAULT_BASES):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    s = bytes.fromhex(c['s'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    sp = Space((A, B, C), bases)
    events = []

    def absA(off):
        return bases[0] + GUARD + off

    def absB(off):
        return bases[1] + GUARD + off

    def absC(off):
        return bases[2] + GUARD + off

    if fn == 0:
        o = a[0]
        tw = o + 0xA8
        flag = a[3] & 0xFFFF
        p0, p1 = ptr_of(a[4], bases), ptr_of(a[5], bases)
        A.wr(tw + 0x88, 2, flag)
        A.wr(tw + 0xA0, 4, p0)
        A.wr(tw + 0xA4, 4, p1)
        A.snap(); B.snap(); C.snap()

        def ev(address, selfaddr):
            events.append([address, sp.tag(selfaddr), sp.safe_bits(selfaddr), A.rd(o, 4), A.rd(tw, 4), A.rd(tw + 0x88, 2), sp.tag(A.rd(tw + 0xA0, 4)), sp.tag(A.rd(tw + 0xA4, 4))])

        A.wr(o, 4, 0x86C2A8)
        A.wr(tw, 4, 0x85F438)
        called = []
        if flag != 0:
            for off in (0xA0, 0xA4):
                p = A.rd(tw + off, 4)
                if p:
                    ev(0x4EF850, p)
                    A.wr(tw + off, 4, 0)
                    called.append(off)
            A.wr(tw + 0x88, 2, 0)
        A.wr(tw, 4, 0x859988)
        ev(0x4EF660, absA(tw + 8))
        A.wr(o, 4, 0x859988)
        ev(0x4EF660, absA(o + 8))
        marks.append('flag_set' if flag else 'flag_zero')
        if flag and (flag & 0xFF) == 0:
            marks.append('flag_upper_only')
        if flag:
            marks.append('stop_both' if len(called) == 2 else 'stop_first_only' if called == [0xA0] else 'stop_second_only' if called == [0xA4] else 'stop_none')
            if p0 and p0 == p1:
                marks.append('same_ptr')
        elif p0 or p1:
            marks.append('ptr_skipped_flag_zero')
        if (a[4] >> 28) == 4 or (a[5] >> 28) == 4:
            marks.append('ptr_raw')
        if ((a[4] >> 28) == 3 and (a[4] & 0x0FFFFFFF) in range(o, o + 0x15C)) or ((a[5] >> 28) == 3 and (a[5] & 0x0FFFFFFF) in range(o, o + 0x15C)):
            marks.append('ptr_in_self')
        if o % 4:
            marks.append('this_unaligned')
        if o + 0x15C > WIN:
            marks.append('spill')
    else:
        ns, nd = a[3], a[4]
        so = [struct.unpack_from('<h', s, 2 * i)[0] for i in range(ns)]
        do = [struct.unpack_from('<h', s, 2 * ns + 2 * i)[0] for i in range(nd)]
        spec = [struct.unpack_from('<H', s, 2 * ns + 2 * nd + 2 * i)[0] for i in range(nd)]
        for i in range(ns):
            A.wr(so[i] + 4, 4, absA(so[i + 1]) if i + 1 < ns else 0)
        for i in range(nd):
            B.wr(do[i] + 4, 4, absB(do[i + 1]) if i + 1 < nd else 0)
            B.wr(do[i] + 8, 4, 0 if spec[i] == 0 else ((absB(do[i - 1]) if i > 0 else 0) if spec[i] == 1 else absC(spec[i])))
        headS = absA(so[0]) if ns else 0
        headD = absB(do[0]) if nd else 0
        o = a[0]
        if fn == 1:
            A.wr(o, 4, headS)
            A.wr(o + 4, 4, headD)
        elif fn == 2:
            A.wr(o, 4, headS)
        else:
            A.wr(o, 4, headD)
        poolS, poolD = absC(POOL_S), absC(POOL_D)
        C.wr(POOL_S + 0, 4, absA(0) - a[5]); C.wr(POOL_S + 4, 4, absC(a[7])); C.wr(POOL_S + 0xC, 4, a[9])
        C.wr(POOL_D + 0, 4, absB(0) - a[6]); C.wr(POOL_D + 4, 4, absC(a[8])); C.wr(POOL_D + 0xC, 4, a[10])
        A.snap(); B.snap(); C.snap()
        stats = {'idx_s': [], 'idx_d': [], 'ff_s': False, 'ff_d': False, 'diff_nm_s': False, 'diff_nm_d': False, 'prev': False}

        def free(pool, cur, single):
            storage = sp.rd(pool, 4)
            diff = s32(cur - storage)
            if single:
                idx = diff >> 3
                if diff % 8:
                    stats['diff_nm_s'] = True
            else:
                idx = abs(diff) // 12 * (1 if diff >= 0 else -1)
                if diff % 12:
                    stats['diff_nm_d'] = True
            states = sp.rd(pool + 4, 4)
            addr = (states + idx) & M32
            sp.wr(addr, 1, sp.rd(addr, 1) | 0x80)
            if idx < s32(sp.rd(pool + 0xC, 4)):
                sp.wr(pool + 0xC, 4, idx & M32)
                stats['ff_s' if single else 'ff_d'] = True
            stats['idx_s' if single else 'idx_d'].append(idx)

        def flush_single(head):
            node = sp.rd(head, 4)
            guard = 0
            while node:
                guard += 1
                if guard > 5000:
                    raise RuntimeError('lista simple no termina')
                cur = node
                node = sp.rd(cur + 4, 4)
                first = sp.rd(head, 4)
                if first == cur:
                    sp.wr(head, 4, node)
                else:
                    while first:
                        after = sp.rd(first + 4, 4)
                        if after == cur:
                            sp.wr(first + 4, 4, sp.rd(cur + 4, 4))
                            break
                        first = after
                free(poolS, cur, True)

        def flush_double(head):
            nxt = sp.rd(head, 4)
            guard = 0
            while nxt:
                guard += 1
                if guard > 5000:
                    raise RuntimeError('lista doble no termina')
                cur = nxt
                nxt = sp.rd(cur + 4, 4)
                if sp.rd(head, 4) == cur:
                    sp.wr(head, 4, nxt)
                prev = sp.rd(cur + 8, 4)
                if prev:
                    stats['prev'] = True
                    sp.wr(prev + 4, 4, sp.rd(cur + 4, 4))
                after = sp.rd(cur + 4, 4)
                if after:
                    sp.wr(after + 8, 4, sp.rd(cur + 8, 4))
                free(poolD, cur, False)

        head = absA(o)
        if fn == 1:
            flush_double(head + 4)
            flush_single(head)
        elif fn == 2:
            flush_single(head)
        else:
            flush_double(head)

        def shape(prefix, count):
            marks.append(prefix + ('_empty' if count == 0 else '_1' if count == 1 else '_many'))

        if fn in (1, 2):
            shape('single', ns)
            if ns:
                marks.append('ff_lowered_s' if stats['ff_s'] else 'ff_kept_s')
            if any(i < 0 for i in stats['idx_s']):
                marks.append('idx_neg_s')
            if stats['diff_nm_s']:
                marks.append('diff_nonmultiple_s')
        if fn in (1, 3):
            shape('double', nd)
            if nd:
                marks.append('ff_lowered_d' if stats['ff_d'] else 'ff_kept_d')
            if any(i < 0 for i in stats['idx_d']):
                marks.append('idx_neg_d')
            if stats['diff_nm_d']:
                marks.append('diff_nonmultiple_d')
            if stats['prev']:
                marks.append('prev_nonnull')
        if o % 4:
            marks.append('head_unaligned')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': None, 'marks': marks}


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


def check_row(row, bases=DEFAULT_BASES):
    c = row['input']
    e = expected(c, bases)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    bases = DEFAULT_BASES
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') == 'begin':
            bases = tuple(row['arena_bases'])
        if row.get('event') != 'comparison':
            continue
        r = check_row(row, bases)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
