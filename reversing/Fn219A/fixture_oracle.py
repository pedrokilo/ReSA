"""Oraculo independiente (Python) de Fn219A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 30 funciones del lote 219A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, D:\\Original\\exe_full_116.dis). Listas: TList_c<T> (+0 cabeza, +4 cola, +8 recuento) y nodos (+0 anterior, +4 siguiente). Vector de MSVC 6: +4 inicio, +8 fin, +0xC limite.
   0  0x4A8DC0  `ret`
   1  0x4A8DD0  mov eax,ecx; xor ecx,ecx; [eax] = [eax+4] = [eax+8] = 0 (en ese orden); devuelve this
   2  0x4A8DE0  `ret`
   3  0x4A8E30  RemoveItem(item): edx = [item+4] (siguiente), eax = [item] (anterior); si edx: [edx] = eax; si no [lista+4] = eax; si eax: [eax+4] = edx, si no [lista] = [item+4] (releido);
             cuenta = [lista+8]; [lista+8] = cuenta - 1; devuelve la cuenta anterior (`ret 4`)
   4  0x4A8E70  RemoveHead: eax = [lista]; vacia: devuelve 0; eax == [lista+4]: [+4] = 0, [lista] = 0, [lista+8] = cuenta - 1 (calculada antes); si no edx = [eax+4], [lista] = edx, si edx: [edx] = 0; dec [lista+8]; devuelve eax
   5  0x4A9010  GetItemOffset(BYTE desdeCabeza, int desplazamiento) `ret 8`: byte != 0: eax = [lista], si desplazamiento <= 0 devuelve eax; do { si eax == 0 devuelve 0; eax = [eax+4]; ecx++ } while (ecx < desplazamiento);
             byte == 0: igual desde [lista+4] con [eax] (anterior)
   6  0x4A9090  push esi; esi = ecx; call 0x4A9F70 (mov eax,ecx; [eax+10h] = 0; ret); eax = esi
   7-15  0x5EC040 ...  vaciado: si [this+4] != 0: operator delete (0x8214BD); +4, +8, +0xC = 0
   16, 18, 20  0x5EC070, 0x5EC110, 0x5EC1B0  fill_n (0x5EB2A0 / 0x5EB310 / 0x5EB390, cdecl, 5 argumentos): si n != 0 (sin signo): n veces: si dest != 0: [dest] = [valor]; dest += 4; devuelve dest + n * 4 (`ret 0Ch`)
   17  0x5EC0E0  igual con 20 bytes (0x5EB2E0, `rep movsd` de 5 palabras); devuelve dest + n * 20
   19  0x5EC140  igual con 12 bytes (0x5EB340, tres palabras en orden); devuelve dest + n * 12
   21  0x5EC220  erase(salida, posicion): edx = [this+8]; eax = posicion + 4; si eax != edx: do { [eax-4] = [eax]; eax += 4 } while (eax != edx); [this+8] -= 4 (releido); [salida] = posicion; devuelve salida (`ret 8`)
   22-26  0x5ECAB0 ...  vector::_Xlen: string local vacio; assign(0x860AB0, 18) (0x4F3F60); runtime_error(local) (0x4F4090); [obj] = 887D68; _CxxThrowException(obj, 0x893634) (no vuelve)
   27, 29  0x5ECF10, 0x5ED4B0  vector<T*>::insert(pos, n, &valor) (mismo cuerpo que 0x4F4320 con otros colaboradores)
   28  0x5ED1B0  vector<20 bytes>::insert(pos, n, &valor): el valor (5 palabras) se copia a una variable local al entrar; cocientes con signo entre 20; maximo 0xCCCCCCC; si la capacidad nueva no alcanza,
             nueva = size() (0x5E9800) + n

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp); las funciones que no vuelven (vector::_Xlen, _CxxThrowException) terminan con la salida no local del banco (threw = true).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["ListItem_c::Destructor", "TList_c::ListConstructorExe", "TList_c::ListDestructorExe", "TList_c::ListRemoveItemExe", "TList_c::ListRemoveHeadExe", "TList_c::ListGetItemOffsetExe",
         "FxFrustumInfo_c::Constructor",
         "CPedAttractor::SArrayTidyStride4D", "CPedAttractor::SArrayTidyStride4E", "CPedAttractor::SArrayTidyStride4F", "CPedAttractor::SArrayTidyStride4G", "CPedAttractor::SArrayTidyStride4H",
         "CPedAttractor::SArrayTidyStride4I", "CPedAttractor::SArrayTidyStride4J", "CPedAttractor::SArrayTidyStride4K", "CPedAttractor::SArrayTidyStride4L",
         "CPedAttractor::SArrayFillNReturnEndStride4B", "CPedAttractor::SArrayFillNReturnEndStride20", "CPedAttractor::SArrayFillNReturnEndStride4C", "CPedAttractor::SArrayFillNReturnEndStride12",
         "CPedAttractor::SArrayFillNReturnEndStride4D", "CPedAttractor::SArrayEraseStride4",
         "LegacyRuntimeError::VectorXlenB", "LegacyRuntimeError::VectorXlenC", "LegacyRuntimeError::VectorXlenD", "LegacyRuntimeError::VectorXlenE", "LegacyRuntimeError::VectorXlenF",
         "CPedAttractor::SArrayInsertNStride4A", "CPedAttractor::SArrayInsertNStride20", "CPedAttractor::SArrayInsertNStride4B"]
ENTRIES = (0x4A8DC0, 0x4A8DD0, 0x4A8DE0, 0x4A8E30, 0x4A8E70, 0x4A9010, 0x4A9090,
           0x5EC040, 0x5EC0B0, 0x5EC180, 0x5EC1F0, 0x5EC260, 0x5EC2D0, 0x5EC340, 0x5EC3B0, 0x5EC3E0,
           0x5EC070, 0x5EC0E0, 0x5EC110, 0x5EC140, 0x5EC1B0, 0x5EC220,
           0x5ECAB0, 0x5ECB30, 0x5ECBB0, 0x5ECC30, 0x5ECCB0, 0x5ECF10, 0x5ED1B0, 0x5ED4B0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(EMPTY, LISTCTOR, REMOVEITEM, REMOVEHEAD, GETOFFSET, FRUSTUM, VECTIDY, FILLNEND4, FILLNEND20, FILLNEND12, ERASE, XLEN, INSERT4, INSERT20) = \
    'empty listctor removeitem removehead getoffset frustum vectidy fillnend4 fillnend20 fillnend12 erase xlen insert4 insert20'.split()
KIND = [EMPTY, LISTCTOR, EMPTY, REMOVEITEM, REMOVEHEAD, GETOFFSET, FRUSTUM] + [VECTIDY] * 9 + [FILLNEND4, FILLNEND20, FILLNEND4, FILLNEND12, FILLNEND4, ERASE] + [XLEN] * 5 + [INSERT4, INSERT20, INSERT4]
RET_RAW = {REMOVEITEM}                                                           # el valor devuelto es un entero (no se etiqueta como puntero)
RET_TAG = {LISTCTOR, REMOVEHEAD, GETOFFSET, FRUSTUM, FILLNEND4, FILLNEND20, FILLNEND12, ERASE}
STACK_TAG = 0x7F000000
EV_DELETE, EV_NEW = 0x8214BD, 0x82119A
# direcciones de los colaboradores de cada instancia: (fill_n, envoltorio de copia, fill_n que devuelve el final, fill, copy_backward, _Xlen)
INSERT4_ADDR = {27: (0x5EB2A0, 0x5EB930, 0x5EC070, 0x5EAAA0, 0x5EB140, 0x5ECAB0), 29: (0x5EB310, 0x5EB990, 0x5EC110, 0x5EAAF0, 0x5EB1B0, 0x5ECBB0)}
STRIDE = {FILLNEND4: 4, FILLNEND20: 20, FILLNEND12: 12}


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


def str_char(seed, i):
    return 0x61 + rnd(seed, i) % 26


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
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = tuple(Mem(a[2], a[36 + i]) for i in range(4))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n=4):
        i, off = self.locate(addr & M32)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr & M32)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        v &= M32
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


def sar2(x):
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    return (x >> 2) & M32


def sdiv20(x):
    """cociente CON signo (trunca hacia cero) de una diferencia de 32 bits entre 20 (imul 0x66666667, sar 3, suma del signo)"""
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    q = abs(x) // 20
    return (-q if x < 0 else q) & M32


def s32(x):
    x &= M32
    return x - (1 << 32) if x >= 1 << 31 else x


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    EMPTY: [], LISTCTOR: [], FRUSTUM: [], XLEN: [],
    REMOVEITEM: ['next_null', 'next_set', 'prev_null', 'prev_set', 'both_null', 'overlap', 'item_is_list'],
    REMOVEHEAD: ['empty', 'single', 'multi_null', 'multi_next', 'raw_head', 'next_overlap'],
    GETOFFSET: ['from_head', 'from_tail', 'off_le0', 'off_pos', 'null_hit', 'found', 'cycle', 'broken'],
    VECTIDY: ['null', 'in_arena', 'raw'],
    FILLNEND4: ['n0', 'n_many', 'dest_null', 'dest_raw', 'dest_overlap'], FILLNEND20: ['n0', 'n_many', 'dest_null', 'dest_raw', 'dest_overlap'], FILLNEND12: ['n0', 'n_many', 'dest_null', 'dest_raw', 'dest_overlap'],
    ERASE: ['single', 'pos_first', 'pos_last', 'pos_mid', 'out_c', 'out_last', 'out_elem', 'out_first', 'out_end'],
    INSERT4: ['empty', 'realloc', 'tail_lt', 'tail_ge', 'count0', 'xlen', 'huge', 'pos_begin', 'pos_end', 'grow_half'],
    INSERT20: ['empty', 'realloc', 'tail_lt', 'tail_ge', 'count0', 'xlen', 'huge', 'pos_begin', 'pos_end', 'grow_half', 'size_call'],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c)
    A, B, Cc, D = W.mem
    events, ret, threw = [], None, False
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tg = W.tagv

    def delete_ev(p):
        events.append([EV_DELETE, tg(p)])

    def ptr_mode(mode, off, item=0):
        """modo de puntero de RemoveItem / RemoveHead: 0 nulo, 1 nodo en C, 2 nodo en D, 3 la lista, 4 el propio elemento, 5 this + 4 (RemoveItem) o this + 8 (RemoveHead)"""
        if mode == 0:
            return 0
        if mode == 1:
            return W.raw(2, off)
        if mode == 2:
            return W.raw(3, off)
        if mode == 3:
            return this
        if mode == 4:
            return item
        return (this + (4 if k == REMOVEITEM else 8)) & M32

    if k == EMPTY:
        W.snap()
    elif k == LISTCTOR:
        W.snap()
        A.wr(o, 4, 0); A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0)
        ret = tg(this)
    elif k == FRUSTUM:
        W.snap()
        A.wr(o + 0x10, 4, 0)
        ret = tg(this)
    elif k == REMOVEITEM:
        item = this if a[4] == 1 else (W.raw(1, a[5]) if a[4] == 0 else W.raw(2, a[5]))
        nxt = ptr_mode(a[6], a[7], item)
        prv = ptr_mode(a[8], a[9], item)
        if a[6] == 6:
            nxt = (this + 8) & M32
        if a[8] == 6:
            prv = (this + 8) & M32
        A.wr(o + 8, 4, a[10])
        W.wr(item + 4, 4, nxt)
        W.wr(item, 4, prv)
        W.snap()
        nxt = W.rd(item + 4)
        prv = W.rd(item)
        if nxt:
            W.wr(nxt, 4, prv)
        else:
            W.wr(this + 4, 4, prv)
        if prv:
            W.wr(prv + 4, 4, nxt)
        else:
            W.wr(this, 4, W.rd(item + 4))
        count = W.rd(this + 8)
        W.wr(this + 8, 4, (count - 1) & M32)
        ret = count
        marks.append('next_set' if nxt else 'next_null')
        marks.append('prev_set' if prv else 'prev_null')
        if not nxt and not prv:
            marks.append('both_null')
        if a[6] in (3, 4, 5, 6) or a[8] in (3, 4, 5, 6):
            marks.append('overlap')
        if a[4] == 1:
            marks.append('item_is_list')
    elif k == REMOVEHEAD:
        sc = a[4]
        if sc == 0:
            head, tail = 0, a[8]
        elif sc == 4:
            head, tail = a[10], a[10]
        elif sc == 1:
            head = W.raw(1, a[5]); tail = head
        else:
            head = W.raw(1, a[5]); tail = a[8]
            if tail == head:
                tail ^= 4
        A.wr(o, 4, head); A.wr(o + 4, 4, tail); A.wr(o + 8, 4, a[9])
        nxt = 0
        if sc in (2, 3):
            nxt = ptr_mode(a[6] if sc == 3 else 0, a[7], head)
            if a[6] == 5 and sc == 3:
                nxt = (this + 8) & M32
            B.wr(a[5] + 4, 4, nxt)
        W.snap()
        eax = W.rd(this)
        if eax == 0:
            ret = 0
            marks.append('empty')
        elif eax == W.rd(this + 4):
            count = (W.rd(this + 8) - 1) & M32
            W.wr(this + 4, 4, 0)
            W.wr(this, 4, 0)
            W.wr(this + 8, 4, count)
            ret = tg(eax)
            marks.append('raw_head' if sc == 4 else 'single')
        else:
            edx = W.rd(eax + 4)
            W.wr(this, 4, edx)
            if edx:
                W.wr(edx, 4, 0)
            W.wr(this + 8, 4, (W.rd(this + 8) - 1) & M32)
            ret = tg(eax)
            marks.append('multi_next' if edx else 'multi_null')
            if sc == 3 and a[6] in (3, 4, 5):
                marks.append('next_overlap')
    elif k == GETOFFSET:
        length, base, sep, flags, brk = a[4], a[5], a[6], a[7], a[8]
        node = [W.raw(1, base + sep * i) for i in range(length)]
        head = node[0] if length else 0
        tail = node[-1] if length else 0
        if flags & 1:
            head = 0
        if flags & 2:
            tail = 0
        if flags & 32:
            head, tail = a[11], a[12]
        A.wr(o, 4, head); A.wr(o + 4, 4, tail)
        for i in range(length):
            nx = node[i + 1] if i + 1 < length else (node[0] if flags & 4 else 0)
            pv = node[i - 1] if i > 0 else (node[-1] if flags & 4 else 0)
            if i == brk % length:
                if flags & 8:
                    nx = 0
                if flags & 16:
                    pv = 0
            B.wr(base + sep * i, 4, pv)
            B.wr(base + sep * i + 4, 4, nx)
        W.snap()
        byte = a[9] & 0xFF
        off = s32(a[10])
        step = 4 if byte else 0
        eax = W.rd(this + (0 if byte else 4))
        ecx = 0
        marks.append('from_head' if byte else 'from_tail')
        marks.append('off_le0' if off <= 0 else 'off_pos')
        if off <= 0:
            ret = tg(eax)
        else:
            while True:
                if eax == 0:
                    ret = 0
                    marks.append('null_hit')
                    break
                eax = W.rd((eax + step) & M32)
                ecx += 1
                if not ecx < off:
                    ret = tg(eax)
                    if eax:
                        marks.append('found')
                    break
        if flags & 4:
            marks.append('cycle')
        if flags & 24:
            marks.append('broken')
    elif k == VECTIDY:
        first = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        W.snap()
        if first != 0:
            delete_ev(first)
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        marks.append('null' if a[4] == 0 else ('in_arena' if a[4] == 1 else 'raw'))
    elif k in (FILLNEND4, FILLNEND20, FILLNEND12):
        stride = STRIDE[k]
        n = a[4]
        valptr = W.raw(1, a[5])
        words = [a[6], a[11], a[12], a[13], a[14]][:stride // 4]
        for i, w in enumerate(words):
            B.wr(a[5] + 4 * i, 4, w)
        if a[7] == 0:
            dest = this
        elif a[7] == 1:
            dest = 0
        elif a[7] == 2:
            dest = a[8]
        else:
            dest = (valptr + 4 * s32(a[10])) & M32
        W.snap()
        d0 = dest
        for _ in range(n):
            if dest != 0:
                for i in range(stride // 4):
                    W.wr((dest + 4 * i) & M32, 4, W.rd((valptr + 4 * i) & M32))
            dest = (dest + stride) & M32
        ret = tg((d0 + stride * n) & M32)
        marks.append('n0' if n == 0 else 'n_many')
        if a[7] == 1:
            marks.append('dest_null')
        if a[7] == 2:
            marks.append('dest_raw')
        if a[7] == 3:
            marks.append('dest_overlap')
    elif k == ERASE:
        size = a[4]
        first = W.raw(1, a[5])
        last = (first + 4 * size) & M32
        end = (first + 4 * (size + a[9])) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, end)
        where = (first + 4 * a[6]) & M32
        mode = a[7]
        out = W.raw(2, a[8]) if mode == 0 else (
            (this + 8) & M32 if mode == 1 else (((first + 4 * a[8]) & M32) if mode == 2 else ((this + 4) & M32 if mode == 3 else (this + 0xC) & M32)))
        W.snap()
        edx = W.rd(this + 8)
        src = (where + 4) & M32
        if src != edx:
            while True:
                W.wr((src - 4) & M32, 4, W.rd(src))
                src = (src + 4) & M32
                if src == edx:
                    break
        W.wr(this + 8, 4, (W.rd(this + 8) - 4) & M32)
        W.wr(out, 4, where)
        ret = tg(out)
        if size == 1:
            marks.append('single')
        if a[6] == 0:
            marks.append('pos_first')
        if a[6] == size - 1:
            marks.append('pos_last')
        if 0 < a[6] < size - 1:
            marks.append('pos_mid')
        marks.append(['out_c', 'out_last', 'out_elem', 'out_first', 'out_end'][mode])
    elif k == XLEN:
        W.snap()
        events.append([0x4F3F60, STACK_TAG, 0x860AB0, 18, 0xF, 0, 0])
        events.append([0x4F4090, STACK_TAG, STACK_TAG, 0, 0xF])
        events.append([0x822D5D, STACK_TAG, 0x893634, 0x887D68])
        threw = True
    elif k == INSERT4:
        fill_n, copy_wrap, fill_n_end, fill, copy_back, xlen = INSERT4_ADDR[fn]
        first = 0 if a[4] == 0 else W.raw(1, a[7])
        size_el, cap_el = a[6], a[5]
        last = 0 if first == 0 else (first + 4 * size_el) & M32
        end = a[10] if first == 0 else (first + 4 * cap_el) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, end)
        value = a[10]
        Cc.wr(a[11], 4, value)
        pos = 0 if first == 0 else (first + 4 * a[8]) & M32
        count = a[9]
        gnew = W.raw(3, a[12])
        gret = a[13]
        W.snap()

        def ucopy(src, lim, dest):
            while src != lim:
                if dest != 0:
                    W.wr(dest, 4, W.rd(src))
                src = (src + 4) & M32
                dest = (dest + 4) & M32
            return dest
        capacity = 0 if first == 0 else sar2((end - first) & M32)
        if count == 0:
            marks.append('count0')
        else:
            size = 0 if first == 0 else sar2((last - first) & M32)
            if ((0x3FFFFFFF - size) & M32) < count:
                events.append([xlen, tg(this)])
                threw = True
                marks.append('xlen')
            else:
                need = (size + count) & M32
                if capacity < need:
                    marks.append('realloc')
                    half = capacity >> 1
                    newcap = (capacity + half) & M32 if ((0x3FFFFFFF - half) & M32) >= capacity else 0
                    if newcap < need:
                        newcap = need
                    else:
                        marks.append('grow_half')
                    nbytes = (newcap << 2) & M32
                    events.append([EV_NEW, nbytes])
                    block = gnew
                    after = ucopy(first, pos, block)
                    events.append([fill_n, tg(after), count, value])
                    ucopy(pos, last, (after + 4 * count) & M32)
                    newsize = (count + size) & M32
                    if first != 0:
                        delete_ev(first)
                    A.wr(o + 0xC, 4, (nbytes + block) & M32)
                    A.wr(o + 8, 4, (block + 4 * newsize) & M32)
                    A.wr(o + 4, 4, block)
                    if count > 64:
                        marks.append('huge')
                else:
                    nbytes = (count * 4) & M32
                    elems_after = sar2((last - pos) & M32)
                    if elems_after < count:
                        marks.append('tail_lt')
                        events.append([copy_wrap, tg(this), tg(pos), tg(last), tg((pos + nbytes) & M32)])
                        tail = sar2((last - pos) & M32)
                        events.append([fill_n_end, tg(this), tg(last), (count - tail) & M32, value])
                        A.wr(o + 8, 4, (last + nbytes) & M32)
                        events.append([fill, tg(pos), tg(last), value])
                    else:
                        marks.append('tail_ge')
                        last_n = (last - nbytes) & M32
                        events.append([copy_wrap, tg(this), tg(last_n), tg(last), tg(last)])
                        A.wr(o + 8, 4, gret)
                        events.append([copy_back, STACK_TAG, tg(pos), tg(last_n), tg(last)])
                        events.append([fill, tg(pos), tg((pos + nbytes) & M32), value])
        if a[4] == 0:
            marks.append('empty')
        if first != 0 and pos == first:
            marks.append('pos_begin')
        if first != 0 and pos == last:
            marks.append('pos_end')
    elif k == INSERT20:
        first = 0 if a[4] == 0 else W.raw(1, a[7])
        size_el, cap_el = a[6], a[5]
        last = 0 if first == 0 else (first + 20 * size_el) & M32
        end = a[10] if first == 0 else (first + 20 * cap_el) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, end)
        vw = [a[10 + i] for i in range(5)]
        for i in range(5):
            Cc.wr(a[15] + 4 * i, 4, vw[i])
        pos = 0 if first == 0 else (first + 20 * a[8]) & M32
        count = a[9]
        gnew = W.raw(3, a[16])
        gret = a[17]
        W.snap()

        def ucopy20(src, lim, dest):
            while src != lim:
                if dest != 0:
                    for i in range(5):
                        W.wr((dest + 4 * i) & M32, 4, W.rd((src + 4 * i) & M32))
                src = (src + 20) & M32
                dest = (dest + 20) & M32
            return dest
        capacity = 0 if first == 0 else sdiv20((end - first) & M32)
        if count == 0:
            marks.append('count0')
        else:
            size = 0 if first == 0 else sdiv20((last - first) & M32)
            if ((0xCCCCCCC - size) & M32) < count:
                events.append([0x5ECB30, tg(this)])
                threw = True
                marks.append('xlen')
            else:
                need = (size + count) & M32
                if capacity < need:
                    marks.append('realloc')
                    half = capacity >> 1
                    newcap = (capacity + half) & M32 if ((0xCCCCCCC - half) & M32) >= capacity else 0
                    if newcap < need:
                        newcap = ((0 if first == 0 else sdiv20((last - first) & M32)) + count) & M32
                        marks.append('size_call')
                    else:
                        marks.append('grow_half')
                    nbytes = (newcap * 20) & M32
                    events.append([EV_NEW, nbytes])
                    block = gnew
                    after = ucopy20(first, pos, block)
                    events.append([0x5EB2E0, tg(after), count] + vw)
                    ucopy20(pos, last, (after + 20 * count) & M32)
                    newsize = (count + (0 if first == 0 else sdiv20((last - first) & M32))) & M32
                    if first != 0:
                        delete_ev(first)
                    A.wr(o + 0xC, 4, (nbytes + block) & M32)
                    A.wr(o + 8, 4, (block + 20 * newsize) & M32)
                    A.wr(o + 4, 4, block)
                    if count > 14:
                        marks.append('huge')
                else:
                    nbytes = (count * 20) & M32
                    q = sdiv20((last - pos) & M32)
                    if q < count:
                        marks.append('tail_lt')
                        events.append([0x5EB960, tg(this), tg(pos), tg(last), tg((pos + nbytes) & M32)])
                        tail = sdiv20((last - pos) & M32)
                        events.append([0x5EC0E0, tg(this), tg(last), (count - tail) & M32] + vw)
                        A.wr(o + 8, 4, (last + nbytes) & M32)
                        events.append([0x5EAAC0, tg(pos), tg(last)] + vw)
                    else:
                        marks.append('tail_ge')
                        last_n = (last - nbytes) & M32
                        events.append([0x5EB960, tg(this), tg(last_n), tg(last), tg(last)])
                        A.wr(o + 8, 4, gret)
                        events.append([0x5EB170, STACK_TAG, tg(pos), tg(last_n), tg(last)])
                        events.append([0x5EAAC0, tg(pos), tg((pos + nbytes) & M32)] + vw)
        if a[4] == 0:
            marks.append('empty')
        if first != 0 and pos == first:
            marks.append('pos_begin')
        if first != 0 and pos == last:
            marks.append('pos_end')
    if threw:
        ret = None
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': Cc.changes(), 'D': D.changes()}, 'gchanges': [], 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:4], e['events'][:4])
    if o.get('threw') != e['threw']:
        return 'threw %s != %s' % (o.get('threw'), e['threw'])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_residue(c, residue):
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado) o AL"""
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
        why = check_residue(c, row[side + '_residue'])
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
