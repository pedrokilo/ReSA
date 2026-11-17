"""Oraculo independiente (Python) de Fn220A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 29 funciones del lote 220A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.
Sin x87 (los floats de los constructores viajan como dwords).

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`, reversing/_handoff/dis.sh). Vector de MSVC 6: +0 asignador (sin leer), +4 inicio, +8 fin, +0xC limite.
   0  0x5ED750  vector<12 bytes>::_Insert_n(pos, n, &x) `ret 0Ch`: copia x (12 bytes) a un local; capacidad = inicio ? (limite - inicio) / 12 : 0 (cociente CON signo); n == 0: sale; tamano = inicio ? (fin - inicio) / 12 : 0;
             si (0x15555555 - tamano) < n (SIN signo): _Xlen 0x5ECC30 (no vuelve); si capacidad < tamano + n (SIN signo): nueva = (0x15555555 - capacidad / 2 >= capacidad) ? capacidad + capacidad / 2 : 0; si nueva < tamano + n:
             nueva = size() (0x5E9850) + n; operator new (0x82119A) de nueva * 12; copia [inicio, pos) (0x5EAEA0), fill_n (0x5EB340, dest, n, &x), copia [pos, fin) a dest + n * 12 (0x5EAEA0); delete (0x8214BD) del viejo
             si hay; limite = bloque + bytes, fin = bloque + (n + tamano) * 12, inicio = bloque. Con capacidad: si (fin - pos) / 12 < n: envoltorio de copia 0x5EB9D0 (pos, fin, pos + n * 12), fill_n que devuelve el final
             0x5EC140 (fin, n - (fin - pos) / 12, &x), fin += n * 12, fill 0x5EB1E0 (pos, fin viejo, &x); si no: fin = 0x5EB9D0 (fin - n * 12, fin, fin), copy_backward 0x5EB220 (&salida, pos, fin - n * 12, fin viejo),
             fill 0x5EB1E0 (pos, pos + n * 12, &x)
   1  0x5EDA50  igual con 4 bytes (instancia C): cocientes con `sar 2`, maximo 0x3FFFFFFF, _Xlen 0x5ECCB0, copia 0x5EAEE0, fill_n 0x5EB390, envoltorio 0x5EBA00, fill_n que devuelve el final 0x5EC1B0, copy_backward 0x5EB260,
             fill 0x5EAB10; sin size() (nueva = tamano + n en linea)
   2  0x5EDD90  vector<20 bytes>::insert(salida, pos, &x) `ret 0Ch`: indice = 0; si inicio: cuenta = (fin - inicio) / 20; si cuenta != 0: indice = (pos - inicio) / 20 (con signo); llama a 0x5ED1B0 (pos, 1, &x) (thiscall);
             *salida = [this + 4] (releido) + indice * 20; devuelve la salida
   3  0x5EDE50  igual con 12 bytes (llama a 0x5ED750)
   4  0x5EDF10  constructor de CPedAttractorManager: marco SEH; pone a cero 30 dwords (+4, +8, +0xC de cada una de las diez listas, de 0x10 en 0x10); devuelve this
 5, 7, 9  0x5EE200, 0x5EE300, 0x5EE400  push_back de 4 bytes: tamano = inicio ? (fin - inicio) >> 2 : 0; si inicio y tamano < (limite - inicio) >> 2 (SIN signo): fill_n 0x5EB2A0 / 0x5EB310 / 0x5EB390 (fin, 1, &x),
             fin += 4; si no _Insert_n 0x5ECF10 / 0x5ED4B0 / 0x5EDA50 (fin, 1, &x) (thiscall)
   6  0x5EE270  push_back de 20 bytes: igual con cocientes entre 20, fill_n 0x5EB2E0, y lento insert 0x5EDD90 (&salida local, fin, &x)
   8  0x5EE370  push_back de 12 bytes: fill_n 0x5EB340, lento insert 0x5EDE50
 10, 13, 16, 19, 22, 26  constructores de CPedATM/Seat/Stop/Pizza/Shelter/TriggerScriptAttractor (0x5EE470, 0x5EE540, 0x5EE610, 0x5EE6E0, 0x5EE7C0, 0x5EE890) `ret 0Ch`: lee siete floats de .rdata y llama a
             0x5EDFB0 (efecto, entidad, estado, maximo de peatones, siete floats); [this] = vtable de la clase; devuelve this
 11, 14, 17, 20, 24, 27  destructores con borrado (0x5EE4E0, 0x5EE5B0, 0x5EE680, 0x5EE760, 0x5EE830, 0x5EE900) `ret 4`: cuerpo (0x5EE530 ...); si bit 0 de los banderas: borra en el pool 0xB744C4
             (indice = (this - [pool]) / 0xC4 con signo: imul 0x5397829D; sar 6; + signo; estado[indice] |= 0x80; si indice < [pool + 0xC] (con signo): [pool + 0xC] = indice); devuelve this
 12, 15, 18, 21, 25, 28  cuerpos del destructor (0x5EE530 ...): [this] = vtable de la clase; salta a 0x5EC410
 23  0x5EE820  `mov eax, 4; ret`

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp); las funciones que no vuelven (vector::_Xlen) terminan con la salida no local del banco (threw = true).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CPedAttractor::SArrayInsertNStride12", "CPedAttractor::SArrayInsertNStride4C", "CPedAttractor::SArrayInsertStride20", "CPedAttractor::SArrayInsertStride12", "CPedAttractorManager::Constructor",
         "CPedAttractor::SArrayPushBackStride4A", "CPedAttractor::SArrayPushBackStride20", "CPedAttractor::SArrayPushBackStride4B", "CPedAttractor::SArrayPushBackStride12", "CPedAttractor::SArrayPushBackStride4C",
         "CPedATMAttractor::Constructor", "CPedATMAttractor::ScalarDeletingDestructor", "CPedATMAttractor::DestructorBody",
         "CPedSeatAttractor::Constructor", "CPedSeatAttractor::ScalarDeletingDestructor", "CPedSeatAttractor::DestructorBody",
         "CPedStopAttractor::Constructor", "CPedStopAttractor::ScalarDeletingDestructor", "CPedStopAttractor::DestructorBody",
         "CPedPizzaAttractor::Constructor", "CPedPizzaAttractor::ScalarDeletingDestructor", "CPedPizzaAttractor::DestructorBody",
         "CPedShelterAttractor::Constructor", "CPedShelterAttractor::GetType", "CPedShelterAttractor::ScalarDeletingDestructor", "CPedShelterAttractor::DestructorBody",
         "CPedTriggerScriptAttractor::Constructor", "CPedTriggerScriptAttractor::ScalarDeletingDestructor", "CPedTriggerScriptAttractor::DestructorBody"]
ENTRIES = (0x5ED750, 0x5EDA50, 0x5EDD90, 0x5EDE50, 0x5EDF10, 0x5EE200, 0x5EE270, 0x5EE300, 0x5EE370, 0x5EE400,
           0x5EE470, 0x5EE4E0, 0x5EE530, 0x5EE540, 0x5EE5B0, 0x5EE600, 0x5EE610, 0x5EE680, 0x5EE6D0, 0x5EE6E0, 0x5EE760, 0x5EE7B0,
           0x5EE7C0, 0x5EE820, 0x5EE830, 0x5EE880,
           0x5EE890, 0x5EE900, 0x5EE950)
assert len(NAMES) == len(ENTRIES) == 29
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(INSN12, INSN4, INS20, INS12, MGR, PB4, PB20, PB12, ACTOR, ASDD, ABODY, TYPE) = \
    'insn12 insn4 ins20 ins12 mgr pb4 pb20 pb12 actor asdd abody type'.split()
KIND = [INSN12, INSN4, INS20, INS12, MGR, PB4, PB20, PB4, PB12, PB4,
        ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY, ACTOR, ASDD, ABODY,
        ACTOR, TYPE, ASDD, ABODY,
        ACTOR, ASDD, ABODY]
CLS = {10: 0, 11: 0, 12: 0, 13: 1, 14: 1, 15: 1, 16: 2, 17: 2, 18: 2, 19: 3, 20: 3, 21: 3, 22: 4, 24: 4, 25: 4, 26: 5, 27: 5, 28: 5}
CLS_VT = (0x86C550, 0x86C568, 0x86C580, 0x86C598, 0x86C5B4, 0x86C5CC)     # vtable del EXE de cada clase (de los constructores 0x5EE4B9, 0x5EE589, ...)
CLS_MAXPEDS = (5, 1, 5, 5, 5, 1)                                         # `push 5` / `push 1` de cada constructor
# siete floats que cada constructor lee de .rdata (0x86C3EC.., 0x86C40C.., 0x86C42C.., 0x86C44C.., 0x86C470.., 0x86C490..), leidos del PE del EXE
CLS_FLOATS = (
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3E4CCCCD, 0x3E19999A, 0x3DCCCCCD, 0x3DCCCCCD),
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3E000000, 0x3DCCCCCD, 0x3DCCCCCD, 0x3DCCCCCD),
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3E4CCCCD, 0x3DCCCCCD, 0x3DCCCCCD, 0x3DCCCCCD),
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3E4CCCCD, 0x3DCCCCCD, 0x3DCCCCCD, 0x3DCCCCCD),
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3F000000, 0x40C8F5C3, 0x3DCCCCCD, 0x3DCCCCCD),
    (0x3F800000, 0x46EA6000, 0x453B8000, 0x3E000000, 0x3DCCCCCD, 0x3DCCCCCD, 0x3DCCCCCD))
STACK_TAG = 0x7F000000
EV_DELETE, EV_NEW, EV_BASECTOR, EV_BASEDTOR = 0x8214BD, 0x82119A, 0x5EDFB0, 0x5EC410
ATTRPOOL = 0xB744C4
ATTR_MAGIC = 0x5397829D
kAttractorSize = 0xC4
MGR_ZEROED = tuple(0x10 * i + k for i in range(10) for k in (4, 8, 0xC))
# colaboradores de las funciones de la STL: (fill_n, envoltorio de copia, fill_n que devuelve el final, fill, copy_backward, _Xlen) de cada _Insert_n; (fill_n, insert) de cada push_back
INSN_ADDR = {0: (0x5EB340, 0x5EB9D0, 0x5EC140, 0x5EB1E0, 0x5EB220, 0x5ECC30), 1: (0x5EB390, 0x5EBA00, 0x5EC1B0, 0x5EAB10, 0x5EB260, 0x5ECCB0)}
INS1_CALL = {2: 0x5ED1B0, 3: 0x5ED750}
PB_ADDR = {5: (0x5EB2A0, 0x5ECF10), 6: (0x5EB2E0, 0x5EDD90), 7: (0x5EB310, 0x5ED4B0), 8: (0x5EB340, 0x5EDE50), 9: (0x5EB390, 0x5EDA50)}
STRIDE = {INSN12: 12, INSN4: 4, INS20: 20, INS12: 12, PB4: 4, PB20: 20, PB12: 12}


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


def signed(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


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

    def changes(self, skip=()):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            rel = i - GUARD
            if self.m[i] != ref and not any(s <= rel < s + 4 for s in skip):
                out.append([rel, self.m[i]])
        return out


class World:
    """las cuatro arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c, stl):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        # las funciones de la STL copian de la arena B: siempre datos aleatorios (los demas rellenos harian invisibles las copias)
        self.mem = tuple(Mem(0 if (stl and i == 1) else a[2], a[92 + i]) for i in range(4))
        self.glob = {}      # memoria global: direccion -> byte (el valor inicial es el que prepara el caso)
        self.glob_ref = {}

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

    def setg(self, addr, n, v):
        for k in range(n):
            self.glob[addr + k] = (v >> (8 * k)) & 0xFF

    def snap(self):
        for m in self.mem:
            m.snap()
        self.glob_ref = dict(self.glob)

    def gchanges(self, order):
        """cambios de las regiones de la funcion respecto a la imagen preparada: [[direccion, byte]] en el orden de las regiones"""
        out = []
        for addr, n in order:
            for k in range(n):
                if self.glob.get(addr + k) != self.glob_ref.get(addr + k):
                    out.append([addr + k, self.glob[addr + k]])
        return out


def sar2(x):
    """cociente con `sar 2` de una diferencia de 32 bits"""
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    return (x >> 2) & M32


def sdivn(x, n):
    """cociente CON signo (trunca hacia cero) de una diferencia de 32 bits entre n (imul con la constante magica, sar, suma del signo)"""
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    q = abs(x) // n
    return (-q if x < 0 else q) & M32


def divs(x, stride):
    return sar2(x) if stride == 4 else sdivn(x, stride)


def pool_index(diff, magic):
    """operator delete en linea del EXE: (this - [pool]) / tamano con la constante magica: imul; sar edx,6; + bit de signo"""
    d = signed(diff)
    hi = (d * magic) >> 32
    q = hi >> 6
    q += 1 if q < 0 else 0
    return q


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    INSN12: ['empty', 'realloc', 'tail_lt', 'tail_ge', 'count0', 'xlen', 'huge', 'pos_begin', 'pos_end', 'grow_half', 'size_call'],
    INSN4: ['empty', 'realloc', 'tail_lt', 'tail_ge', 'count0', 'xlen', 'huge', 'pos_begin', 'pos_end', 'grow_half'],
    INS20: ['first_null', 'first_set', 'count0', 'pos_raw', 'out_c', 'out_first', 'out_last', 'out_end', 'mod_first', 'idx_zero', 'idx_positive'],
    INS12: ['first_null', 'first_set', 'count0', 'pos_raw', 'out_c', 'out_first', 'out_last', 'out_end', 'mod_first', 'idx_zero', 'idx_positive'],
    MGR: [],
    PB4: ['fast', 'slow_full', 'slow_over', 'slow_null', 'cap_negative', 'size_negative'],
    PB20: ['fast', 'slow_full', 'slow_over', 'slow_null', 'cap_negative', 'size_negative'],
    PB12: ['fast', 'slow_full', 'slow_over', 'slow_null', 'cap_negative', 'size_negative'],
    ACTOR: ['state_zero', 'state_other', 'fx_zero', 'entity_zero'],
    ASDD: ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'idx_zero', 'idx_positive', 'lowers_first_free', 'keeps_first_free', 'rem_nonzero'],
    ABODY: ['write_zero', 'write_other'],
    TYPE: [],
}
for _fn in range(len(NAMES)):
    KEYS[_fn] = list(COMMON) + _EXTRA[KIND[_fn]]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = ['kind%d' % a[1], 'fill%d' % a[2], 'this_unaligned' if a[0] % 4 else 'this_aligned']
    W = World(c, k in STRIDE)
    A, B, Cc, D = W.mem
    events, ret, threw, vptrs = [], None, False, []
    gorder = []
    o = a[0]
    this = W.raw(0, o)
    A.wr(o, 4, a[3])
    tg = W.tagv

    def delete_ev(p):
        events.append([EV_DELETE, tg(p)])

    if k in (INSN12, INSN4):
        S = STRIDE[k]
        words = S // 4
        fill_n, copy_wrap, fill_n_end, fill, copy_back, xlen = INSN_ADDR[fn]
        maxsize = 0x15555555 if S == 12 else 0x3FFFFFFF
        first = 0 if a[4] == 0 else W.raw(1, a[7])
        size_el, cap_el = a[6], a[5]
        last = 0 if first == 0 else (first + S * size_el) & M32
        end = a[18] if first == 0 else (first + S * cap_el) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, end)
        vw = [a[10 + i] for i in range(words)]
        for i, w in enumerate(vw):
            Cc.wr(a[15] + 4 * i, 4, w)
        pos = 0 if first == 0 else (first + S * a[8]) & M32
        count = a[9]
        gnew = W.raw(3, a[16])
        gret = a[17]
        W.snap()

        def ucopy(src, lim, dest):
            while src != lim:
                if dest != 0:
                    for i in range(words):
                        W.wr((dest + 4 * i) & M32, 4, W.rd((src + 4 * i) & M32))
                src = (src + S) & M32
                dest = (dest + S) & M32
            return dest
        capacity = 0 if first == 0 else divs((end - first) & M32, S)
        if count == 0:
            marks.append('count0')
        else:
            size = 0 if first == 0 else divs((last - first) & M32, S)
            if ((maxsize - size) & M32) < count:
                events.append([xlen, tg(this)])
                threw = True
                marks.append('xlen')
            else:
                need = (size + count) & M32
                if capacity < need:
                    marks.append('realloc')
                    half = capacity >> 1
                    newcap = (capacity + half) & M32 if ((maxsize - half) & M32) >= capacity else 0
                    if newcap < need:
                        newcap = need
                        if S == 12:
                            marks.append('size_call')
                    else:
                        marks.append('grow_half')
                    nbytes = (newcap * S) & M32
                    events.append([EV_NEW, nbytes])
                    block = gnew
                    after = ucopy(first, pos, block)
                    events.append([fill_n, tg(after), count] + vw)
                    ucopy(pos, last, (after + S * count) & M32)
                    newsize = (count + size) & M32
                    if first != 0:
                        delete_ev(first)
                    A.wr(o + 0xC, 4, (nbytes + block) & M32)
                    A.wr(o + 8, 4, (block + S * newsize) & M32)
                    A.wr(o + 4, 4, block)
                    if count > 12:
                        marks.append('huge')
                else:
                    nbytes = (count * S) & M32
                    elems_after = divs((last - pos) & M32, S)
                    if elems_after < count:
                        marks.append('tail_lt')
                        events.append([copy_wrap, tg(this), tg(pos), tg(last), tg((pos + nbytes) & M32)])
                        tail = divs((last - pos) & M32, S)
                        events.append([fill_n_end, tg(this), tg(last), (count - tail) & M32] + vw)
                        A.wr(o + 8, 4, (last + nbytes) & M32)
                        events.append([fill, tg(pos), tg(last)] + vw)
                    else:
                        marks.append('tail_ge')
                        last_n = (last - nbytes) & M32
                        events.append([copy_wrap, tg(this), tg(last_n), tg(last), tg(last)])
                        A.wr(o + 8, 4, gret)
                        events.append([copy_back, STACK_TAG, tg(pos), tg(last_n), tg(last)])
                        events.append([fill, tg(pos), tg((pos + nbytes) & M32)] + vw)
        if a[4] == 0:
            marks.append('empty')
        if first != 0 and pos == first:
            marks.append('pos_begin')
        if first != 0 and pos == last:
            marks.append('pos_end')
    elif k in (INS20, INS12):
        S = STRIDE[k]
        words = S // 4
        first = 0 if a[4] == 0 else W.raw(1, a[7])
        last = a[22] if first == 0 else (first + S * a[6]) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, a[21])
        vw = [a[10 + i] for i in range(words)]
        for i, w in enumerate(vw):
            Cc.wr(a[15] + 4 * i, 4, w)
        pos = a[19] if a[9] == 1 else (0 if first == 0 else (first + S * a[8]) & M32)
        mode = a[16]
        out = W.raw(2, a[17]) if mode == 0 else ((this + 4) & M32 if mode == 1 else ((this + 8) & M32 if mode == 2 else (this + 0xC) & M32))
        valptr = W.raw(2, a[15])
        W.snap()
        index = 0
        if first != 0:
            count = divs((last - first) & M32, S)
            if count != 0:
                index = divs((pos - first) & M32, S)
        events.append([INS1_CALL[fn], tg(this), tg(pos), 1, tg(valptr)] + vw)
        if a[18]:
            A.wr(o + 4, 4, a[20])
            marks.append('mod_first')
        newfirst = A.rd(o + 4, 4)
        W.wr(out, 4, (newfirst + index * S) & M32)
        ret = tg(out)
        marks.append('first_null' if first == 0 else 'first_set')
        if a[9] == 1:
            marks.append('pos_raw')
        marks.append(['out_c', 'out_first', 'out_last', 'out_end'][mode])
        marks.append('idx_zero' if index == 0 else 'idx_positive')
        if first != 0 and divs((last - first) & M32, S) == 0:
            marks.append('count0')
    elif k == MGR:
        W.snap()
        for off in MGR_ZEROED:
            A.wr(o + off, 4, 0)
        ret = tg(this)
    elif k in (PB4, PB20, PB12):
        S = STRIDE[k]
        words = S // 4
        fill_n, ins = PB_ADDR[fn]
        first = 0 if a[4] == 0 else W.raw(1, a[7])
        last = a[19] if first == 0 else (first + S * a[6]) & M32
        end = a[18] if first == 0 else (first + S * a[5]) & M32
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, last); A.wr(o + 0xC, 4, end)
        vw = [a[10 + i] for i in range(words)]
        for i, w in enumerate(vw):
            Cc.wr(a[15] + 4 * i, 4, w)
        valptr = W.raw(2, a[15])
        W.snap()
        size = 0 if first == 0 else divs((last - first) & M32, S)
        cap = 0 if first == 0 else divs((end - first) & M32, S)
        if first != 0 and size < cap:
            marks.append('fast')
            events.append([fill_n, tg(last), 1] + vw)
            A.wr(o + 8, 4, (last + S) & M32)
        else:
            if S == 4:
                events.append([ins, tg(this), tg(last), 1, tg(valptr)] + vw)
            else:
                events.append([ins, tg(this), STACK_TAG, tg(last), tg(valptr)] + vw)
            marks.append('slow_null' if first == 0 else ('slow_full' if size == cap else 'slow_over'))
        if first != 0 and signed(a[5]) < 0:
            marks.append('cap_negative')
        if first != 0 and signed(a[6]) < 0:
            marks.append('size_negative')
    elif k == ACTOR:
        cls = CLS[fn]
        W.snap()
        events.append([EV_BASECTOR, tg(this), a[4], a[5], a[6], CLS_MAXPEDS[cls]] + list(CLS_FLOATS[cls]))
        vptrs.append([0, o, 'vt'])
        ret = tg(this)
        marks.append('state_zero' if a[6] == 0 else 'state_other')
        if a[4] == 0:
            marks.append('fx_zero')
        if a[5] == 0:
            marks.append('entity_zero')
    elif k == ASDD:
        cls = CLS[fn]
        idx, rem = a[9], a[10]
        diff = idx * kAttractorSize + rem
        objects = (this - diff) & M32
        pool = a[7]
        B.wr(pool + 0, 4, objects); B.wr(pool + 4, 4, W.raw(3, a[8])); B.wr(pool + 8, 4, 0x40); B.wr(pool + 0xC, 4, a[11])
        D.wr(a[8] + idx, 1, D.rd(a[8] + idx, 1) & 0x7F)       # el hueco esta usado
        W.setg(ATTRPOOL, 4, W.raw(1, pool))
        gorder = [(ATTRPOOL, 4)]
        W.snap()
        events.append([EV_BASEDTOR, tg(this), CLS_VT[cls]])     # el cuerpo de la clase escribio su vtable del EXE antes de saltar a 0x5EC410
        A.wr(o, 4, a[6])
        if a[4] & 1:
            marks.append('flag_bit0_set')
            index = pool_index((this - B.rd(pool, 4)) & M32, ATTR_MAGIC)
            state_off = a[8] + index
            D.wr(state_off, 1, D.rd(state_off, 1) | 0x80)
            first = signed(B.rd(pool + 0xC, 4))
            if index < first:
                B.wr(pool + 0xC, 4, index & M32)
                marks.append('lowers_first_free')
            else:
                marks.append('keeps_first_free')
            marks.append('idx_zero' if index == 0 else 'idx_positive')
            if rem:
                marks.append('rem_nonzero')
        else:
            marks.append('flag_bit0_clear')
        if a[4] >> 8:
            marks.append('flag_high_bits')
        ret = tg(this)
    elif k == ABODY:
        cls = CLS[fn]
        W.snap()
        events.append([EV_BASEDTOR, tg(this), CLS_VT[cls]])
        A.wr(o, 4, a[6])
        marks.append('write_zero' if a[6] == 0 else 'write_other')
    else:   # TYPE
        W.snap()
        ret = 4
    if threw:
        ret = None
    skip = [[], [], [], []]
    for ar, off, _ in vptrs:
        skip[ar].append(off)
    return {'events': events, 'changes': {'A': A.changes(skip[0]), 'B': B.changes(skip[1]), 'C': Cc.changes(skip[2]), 'D': D.changes(skip[3])}, 'vptrs': vptrs,
            'gchanges': W.gchanges(gorder), 'ret': ret, 'threw': threw, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('vptrs') != e['vptrs']:
        return 'vptrs: %s != %s' % (o.get('vptrs'), e['vptrs'])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:6], e['events'][:6])
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
    """sin contratos de registros exigidos: ningun llamador del EXE lee EAX/ECX/EDX tras estas llamadas salvo EAX (valor devuelto, ya comprobado)"""
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
