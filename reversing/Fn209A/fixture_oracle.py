"""Oraculo independiente (Python) de Fn209A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 29 funciones del lote 209A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (sobre el gta-sa.exe canonico, `dumpbin /disasm`). Disposicion de std::string (MSVC 9): +4 puntero o buffer de 16 bytes, +0x14 longitud, +0x18 capacidad; vector: +4 inicio, +8 fin, +0xC limite.
   0  0x4F3AE0  std::fill(first,last,&v): si first != last: bucle [first] = [v] (v se relee), first += 4 hasta last
   1  0x4F3B00  std::copy_backward(out,first,last,destEnd): si first != last: bucle (last -= 4; destEnd -= 4; [destEnd] = [last]) hasta last == first; [out] = destEnd; eax = out
   2  0x4F3B30  uninitialized_fill_n(dest,n,&v,..): si n != 0 (sin signo): n veces: si dest != 0: [dest] = [v]; dest += 4
   3  0x4F3CE0  envoltorio: uninitialized_copy(first,last,dest) (0x4F3940): si first != last: bucle si dest != 0: [dest] = [first]; first += 4; dest += 4; devuelve dest (`ret 0Ch`, ECX sin leer)
   4  0x4F3D80  fill_n y devuelve dest + n*4 (`ret 0Ch`)
   5, 6  0x4F3F30, 0x4F4030  vaciado: si [this+4] != 0: operator delete (0x8214BD); +4, +8, +0xC = 0
   7  0x4F4320  vector::insert(pos, n, &valor) (ver expected)
   8  0x4F3BA0  _Tidy: si capacidad >= 16: delete(puntero); capacidad = 0xF; longitud = 0; byte [+4] = 0
   9  0x4F3BD0  _Grow(nuevo, vaciar BYTE)
  10  0x4F3DB0  erase(pos, n)
  11  0x4F3E80  assign(str, pos, n)
  12  0x4F3F60  assign(char*, n)
  13  0x4F4170  string(const char*): capacidad 0xF, longitud 0, byte [+4] = 0; strlen byte a byte; assign(texto, longitud)
  14, 15  0x4F3D10, 0x4F4130  cuerpo de ~runtime_error: [this] = 887D5C; si capacidad (+0x24) >= 16: delete([+0x10]); capacidad = 0xF; longitud (+0x20) = 0; byte [+0x10] = 0; jmp 0x822E2F
  16  0x4F3D50  what(): capacidad (+0x24) >= 16 ? [+0x10] : this+0x10
  17, 18  0x4F3D60, 0x4F4110  cuerpo (0x4F3D10 / 0x4F4130) y, si el bit 0 de la BANDERA (byte de la pila) esta puesto, delete(this); devuelve this
  19  0x4F4090  runtime_error(const string&): 0x822D97(this); [this] = 887D5C; string +0xC vacio (capacidad 0xF, longitud 0, byte 0); assign(this+0xC, mensaje, 0, -1) (0x4F3E80); devuelve this
  20  0x4F4230  runtime_error(const runtime_error&): 0x822DE5(this, otro); igual; assign(this+0xC, otro+0xC, 0, -1)
  21  0x4F41B0  vector::_Xlen: string local vacio; assign(0x860AB0, 18); runtime_error(local) (0x4F4090); [obj] = 887D68; _CxxThrowException(obj, 0x893634) (no vuelve)
  22  0x4F4CA0  al = ([ecx+428h] >> 5) & 1
  23  0x4F4CE0  ax = [ecx+45Ch]; 0 o 0xFFFF -> 0; ([ecx+36h] & 0F8h) == 28h -> 0; si no 1
  24  0x66BA70  0x6677A0(this); si el bit 0 de la bandera: 0x41B820(this); devuelve this
  25  0x66BA90  MakeAbortable(ped, prioridad, evento): prioridad == 0: [[this+1Ch]] = 0; copia 3 dwords (matriz+0x30 o ped+4) a this+0x24; llama [[this+8]+18h](ped, prioridad, evento) (ranura 6); flags(+0x20) ^= ((al << 1) ^ flags) & 2; devuelve bit 1
  26  0x66BB00  ControlSubTask(ped): si [this+20h] & 3: word [this+12h] = 0; jmp [[this]+24h] (ranura 9); si no devuelve [this+8]
  27  0x6709F0  cuerpo del destructor: [this] = 870114; ruta = [this+1Ch]; si no es nula: 0x41B820(ruta) (antes un iterador de destructores que solo llama a un `ret`); 0x61A3D0(this)
  28  0x66D300  Clone: 0x61A5A0(0x80); sin memoria 0; bit 1 de [+78h]: 0x66C450(mem; +64h, [+70h], [+74h], this+10h, this+28h, [+5Ch], [+38h], (byte[+0Ch] & 10h) >> 4, [+40h]); si no 0x66C3E0(mem; +64h, [+70h], [+74h])

Los falsos anotan [direccion, argumentos...] (ver LiveComparison.hpp); las funciones que no vuelven (_Xran 0x8211FD, _Xlen 0x821255, _CxxThrowException 0x822D5D, vector::_Xlen 0x4F41B0) terminan con la salida
no local del banco (threw = true).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ["CPedAttractor::SArrayFillStride4D", "CPedAttractor::SArrayCopyBackwardStride4D", "CPedAttractor::SArrayFillNStride4D", "CPedAttractor::SArrayUninitCopyWrapStride4D",
         "CPedAttractor::SArrayFillNReturnEndStride4", "CPedAttractor::SArrayTidyStride4B", "CPedAttractor::SArrayTidyStride4C", "UserTracksLegacyVector::Insert",
         "LegacyStdString::Tidy", "LegacyStdString::Grow", "LegacyStdString::Erase", "LegacyStdString::AssignString", "LegacyStdString::AssignChars", "LegacyStdString::ConstructFromChars",
         "LegacyRuntimeError::DestructorBodyA", "LegacyRuntimeError::DestructorBodyB", "LegacyRuntimeError::What", "LegacyRuntimeError::ScalarDeletingDestructorA",
         "LegacyRuntimeError::ScalarDeletingDestructorB", "LegacyRuntimeError::ConstructFromString", "LegacyRuntimeError::CopyConstructor", "LegacyRuntimeError::VectorXlen",
         "CVehicle::IsHandbrakeOn", "CVehicle::IsAlarmActive", "CPatrolRoute::ScalarDeletingDestructor", "CTaskComplexFollowPatrolRoute::MakeAbortable",
         "CTaskComplexFollowPatrolRoute::ControlSubTask", "CTaskComplexFollowPatrolRoute::DestructorBody", "CTaskSimpleSlideToCoord::Clone"]
ENTRIES = (0x4F3AE0, 0x4F3B00, 0x4F3B30, 0x4F3CE0, 0x4F3D80, 0x4F3F30, 0x4F4030, 0x4F4320, 0x4F3BA0, 0x4F3BD0, 0x4F3DB0, 0x4F3E80, 0x4F3F60, 0x4F4170, 0x4F3D10, 0x4F4130, 0x4F3D50,
           0x4F3D60, 0x4F4110, 0x4F4090, 0x4F4230, 0x4F41B0, 0x4F4CA0, 0x4F4CE0, 0x66BA70, 0x66BA90, 0x66BB00, 0x6709F0, 0x66D300)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000, 0x30300000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
(FILL, COPYBACK, FILLN, COPYWRAP, FILLNEND, VECTIDY, INSERT, STRTIDY, GROW, ERASE, ASSIGNSTR, ASSIGNCHARS, FROMCHARS, RTDTOR, RTWHAT, RTSCALAR, RTFROMSTR, RTCOPY, XLEN, HANDBRAKE, ALARM,
 PATROLSD, ABORT, CONTROL, FOLLOWDTOR, CLONE) = \
    'fill copyback filln copywrap fillnend vectidy insert strtidy grow erase assignstr assignchars fromchars rtdtor rtwhat rtscalar rtfromstr rtcopy xlen handbrake alarm patrolsd abort control followdtor clone'.split()
KIND = [FILL, COPYBACK, FILLN, COPYWRAP, FILLNEND, VECTIDY, VECTIDY, INSERT, STRTIDY, GROW, ERASE, ASSIGNSTR, ASSIGNCHARS, FROMCHARS, RTDTOR, RTDTOR, RTWHAT, RTSCALAR, RTSCALAR, RTFROMSTR, RTCOPY,
        XLEN, HANDBRAKE, ALARM, PATROLSD, ABORT, CONTROL, FOLLOWDTOR, CLONE]
RET_AL = {GROW, HANDBRAKE, ALARM, ABORT}                                         # los llamadores del EXE solo leen AL
RET_TAG = {COPYWRAP, FILLNEND, ERASE, ASSIGNSTR, ASSIGNCHARS, FROMCHARS, RTWHAT, RTSCALAR, RTFROMSTR, RTCOPY, PATROLSD, CONTROL, CLONE}
STACK_TAG = 0x7F000000
EV_DELETE, EV_NEW, EV_XRAN, EV_XLEN, EV_COPY = 0x8214BD, 0x82119A, 0x8211FD, 0x821255, 0x4F39C0
EV_ABORT_SLOT, EV_CREATE_SLOT = 0xF0000006, 0xF0000009


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


def put_str(W, ai, off, bi, s):
    """escribe un std::string (a[base..base+5]) en la arena ai, desplazamiento off; su bloque en la arena bi"""
    mode, size, res, blk, seed, raw = (x & M32 for x in s)
    m = W.mem[ai]
    m.wr(off + 0x14, 4, size)
    m.wr(off + 0x18, 4, res)
    if mode == 1:
        m.wr(off + 4, 4, W.raw(bi, blk))
        for i in range(size):
            W.mem[bi].wr(blk + i, 1, str_char(seed, i))
        W.mem[bi].wr(blk + size, 1, 0)
    elif mode == 2:
        m.wr(off + 4, 4, raw)
    else:
        for i in range(size):
            m.wr(off + 4 + i, 1, str_char(seed, i))
        m.wr(off + 4 + size, 1, 0)


def sdata(W, addr):
    """direccion del texto activo de un string situado en `addr` (el objeto, +0)"""
    return W.rd(addr + 4) if W.rd(addr + 0x18) >= 0x10 else (addr + 4) & M32


def sar2(x):
    x &= M32
    if x >= 1 << 31:
        x -= 1 << 32
    return (x >> 2) & M32


def rep_movs(W, src, dst, n):
    """rep movsd (n / 4) y rep movsb (n % 4), elemento a elemento (la memoria se relee tras cada escritura)"""
    for i in range(n >> 2):
        W.wr(dst + 4 * i, 4, W.rd(src + 4 * i, 4))
    base = n & ~3
    for j in range(n & 3):
        W.wr(dst + base + j, 1, W.rd(src + base + j, 1))


COMMON = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
KEYS = {}
_EXTRA = {
    FILL: ['n0', 'n_many'], COPYBACK: ['n0', 'n_many', 'overlap', 'separate'], FILLN: ['n0', 'n_many', 'dest_null', 'dest_raw'], COPYWRAP: ['n0', 'n_many', 'overlap', 'separate', 'dest_null'],
    FILLNEND: ['n0', 'n_many', 'dest_null', 'dest_raw'], VECTIDY: ['null', 'in_arena', 'raw'],
    INSERT: ['empty', 'realloc', 'tail_lt', 'tail_ge', 'count0', 'xlen', 'huge', 'pos_begin', 'pos_end', 'grow_half'],
    STRTIDY: ['sso', 'heap', 'raw', 'odd', 'freed'],
    GROW: ['xlen', 'copy', 'copy_trim', 'trim_free', 'trim_nofree', 'zero', 'nop', 'sso', 'heap'],
    ERASE: ['xran', 'erase_some', 'erase_none', 'erase_all', 'sso', 'heap', 'tail_moved'],
    ASSIGNSTR: ['xran', 'same', 'other_grow', 'other_nogrow', 'sso', 'heap', 'len0'],
    ASSIGNCHARS: ['inside', 'outside', 'copy', 'nocopy', 'edge_end', 'edge_before', 'n0'],
    FROMCHARS: ['len0', 'len_long'],
    RTDTOR: ['freed', 'kept', 'sso', 'heap'], RTWHAT: ['sso', 'heap', 'raw'], RTSCALAR: ['deleted', 'kept', 'freed'], RTFROMSTR: ['msg_sso', 'msg_heap'], RTCOPY: ['msg_sso', 'msg_heap'],
    XLEN: [], HANDBRAKE: ['on', 'off'], ALARM: ['alarm0', 'alarmFFFF', 'wrecked', 'active'],
    PATROLSD: ['deleted', 'kept'], ABORT: ['prio0', 'prio_other', 'matrix', 'no_matrix', 'aborted', 'not_aborted'], CONTROL: ['flags_set', 'flags_clear'],
    FOLLOWDTOR: ['route_null', 'route_set'], CLONE: ['anim', 'noanim', 'nomem'],
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

    if k == FILL:
        n, first = a[4], this
        valptr = W.raw(1, a[5])
        B.wr(a[5], 4, a[6])
        W.snap()
        for i in range(n):
            W.wr(first + 4 * i, 4, W.rd(valptr))
        marks.append('n0' if n == 0 else 'n_many')
    elif k == COPYBACK:
        n = a[4]
        sa = 2 if a[6] == 0 else 3
        first = W.raw(sa, a[5])
        last = (first + 4 * n) & M32
        dest_end = W.raw(3, a[7])
        W.snap()
        eax, ecx = last, dest_end
        if first != last:
            while True:
                esi = W.rd((eax - 4) & M32)
                eax = (eax - 4) & M32
                ecx = (ecx - 4) & M32
                W.wr(ecx, 4, esi)
                if eax == first:
                    break
        A.wr(o, 4, ecx)
        marks.append('n0' if n == 0 else 'n_many')
        marks.append('overlap' if a[6] else 'separate')
    elif k in (FILLN, FILLNEND):
        n = a[4]
        dest = this if a[7] == 0 else (0 if a[7] == 1 else a[8])
        valptr = W.raw(1, a[5])
        B.wr(a[5], 4, a[6])
        W.snap()
        d0 = dest
        for i in range(n):
            if dest != 0:
                W.wr(dest, 4, W.rd(valptr))
            dest = (dest + 4) & M32
        if k == FILLNEND:
            ret = tg((d0 + 4 * n) & M32)
        marks.append('n0' if n == 0 else 'n_many')
        if a[7] == 1:
            marks.append('dest_null')
        if a[7] == 2:
            marks.append('dest_raw')
    elif k == COPYWRAP:
        n = a[4]
        src = W.raw(2, a[5])
        delta = a[7] - (1 << 32) if a[7] >= 1 << 31 else a[7]
        dest = this if a[6] == 0 else (src + 4 * delta) & M32
        if a[8] == 1:
            dest = 0
        W.snap()
        s, d = src, dest
        for i in range(n):
            if d != 0:
                W.wr(d, 4, W.rd(s))
            s = (s + 4) & M32
            d = (d + 4) & M32
        ret = tg(d)
        marks.append('n0' if n == 0 else 'n_many')
        marks.append('overlap' if a[6] else 'separate')
        if a[8] == 1:
            marks.append('dest_null')
    elif k == VECTIDY:
        first = 0 if a[4] == 0 else (W.raw(1, a[5]) if a[4] == 1 else a[6])
        A.wr(o + 4, 4, first); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        W.snap()
        if first != 0:
            delete_ev(first)
        A.wr(o + 4, 4, 0); A.wr(o + 8, 4, 0); A.wr(o + 0xC, 4, 0)
        marks.append('null' if a[4] == 0 else ('in_arena' if a[4] == 1 else 'raw'))
    elif k == INSERT:
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
                events.append([0x4F41B0, tg(this)])
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
                    events.append([0x4F3B30, tg(after), count, value])
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
                        events.append([0x4F3CE0, tg(this), tg(pos), tg(last), tg((pos + nbytes) & M32)])
                        tail = sar2((last - pos) & M32)
                        events.append([0x4F3D80, tg(this), tg(last), (count - tail) & M32, value])
                        A.wr(o + 8, 4, (last + nbytes) & M32)
                        events.append([0x4F3AE0, tg(pos), tg(last), value])
                    else:
                        marks.append('tail_ge')
                        last_n = (last - nbytes) & M32
                        events.append([0x4F3CE0, tg(this), tg(last_n), tg(last), tg(last)])
                        A.wr(o + 8, 4, gret)
                        events.append([0x4F3B00, STACK_TAG, tg(pos), tg(last_n), tg(last)])
                        events.append([0x4F3AE0, tg(pos), tg((pos + nbytes) & M32), value])
        if a[4] == 0:
            marks.append('empty')
        if first != 0 and pos == first:
            marks.append('pos_begin')
        if first != 0 and pos == last:
            marks.append('pos_end')
    elif k in (STRTIDY, GROW, ERASE):
        spec = a[4:10]
        put_str(W, 0, o, 1, spec)
        mode = spec[0]
        W.snap()
        res = W.rd(this + 0x18)
        size = W.rd(this + 0x14)
        if k == STRTIDY:
            if res >= 0x10:
                delete_ev(W.rd(this + 4))
                marks.append('freed')
            A.wr(o + 0x18, 4, 0xF); A.wr(o + 0x14, 4, 0); A.wr(o + 4, 1, 0)
            marks.append(['sso', 'heap', 'raw', 'odd'][mode])
        elif k == GROW:
            ns, trim = a[10], a[11] & 0xFF
            marks.append('heap' if res >= 0x10 else 'sso')
            if ns > 0xFFFFFFFE:
                events.append([EV_XLEN]); threw = True
                marks.append('xlen')
            elif res < ns:
                events.append([EV_COPY, tg(this), ns, 0 if trim else size])
                ret = 1 if ns != 0 else 0
                marks.append('copy_trim' if trim else 'copy')
                marks.append('copy')
            elif trim and ns < 0x10:
                if res >= 0x10:
                    delete_ev(W.rd(this + 4))
                    marks.append('trim_free')
                else:
                    marks.append('trim_nofree')
                A.wr(o + 0x18, 4, 0xF); A.wr(o + 0x14, 4, 0); A.wr(o + 4, 1, 0)
                ret = 1 if ns != 0 else 0
            elif ns == 0:
                A.wr(o + 0x14, 4, 0)
                W.wr(sdata(W, this), 1, 0)
                ret = 0
                marks.append('zero')
            else:
                ret = 1
                marks.append('nop')
        else:
            pos, n = a[10], a[11]
            marks.append('heap' if res >= 0x10 else 'sso')
            if size < pos:
                events.append([EV_XRAN]); threw = True
                marks.append('xran')
            else:
                rem = size - pos
                cnt = min(rem, n)
                if cnt > 0:
                    base = sdata(W, this)
                    tail = [W.rd(base + pos + cnt + j, 1) for j in range(rem - cnt)]
                    for j, v in enumerate(tail):
                        W.wr(base + pos + j, 1, v)
                    newsize = size - cnt
                    A.wr(o + 0x14, 4, newsize)
                    W.wr(base + newsize, 1, 0)
                    marks.append('erase_all' if newsize == 0 else 'erase_some')
                    if tail:
                        marks.append('tail_moved')
                else:
                    marks.append('erase_none')
                ret = tg(this)
    elif k == ASSIGNSTR:
        sspec, tspec = a[4:10], a[10:16]
        toff = a[20]
        put_str(W, 0, o, 1, sspec)
        put_str(W, 2, toff, 3, tspec)
        W.snap()
        other = this if a[18] else W.raw(2, toff)
        pos, n, grow = a[16], a[17], a[19]
        osize = W.rd(other + 0x14)
        marks.append('heap' if W.rd(this + 0x18) >= 0x10 else 'sso')
        if osize < pos:
            events.append([EV_XRAN]); threw = True
            marks.append('xran')
        else:
            length = osize - pos
            if n < length:
                length = n
            if a[18]:
                events.append([0x4F3DB0, tg(this), (pos + length) & M32, 0xFFFFFFFF])
                events.append([0x4F3DB0, tg(this), 0, pos])
                marks.append('same')
            else:
                events.append([0x4F3BD0, tg(this), length, 1])
                if grow & 0xFF:
                    rep_movs(W, (sdata(W, other) + pos) & M32, sdata(W, this), length)
                    A.wr(o + 0x14, 4, length)
                    W.wr((sdata(W, this) + length) & M32, 1, 0)
                    marks.append('other_grow')
                else:
                    marks.append('other_nogrow')
                if length == 0:
                    marks.append('len0')
            ret = tg(this)
    elif k == ASSIGNCHARS:
        spec = a[4:10]
        put_str(W, 0, o, 1, spec)
        W.snap()
        data = sdata(W, this)
        size = W.rd(this + 0x14)
        tm, cnt, grow = a[10], a[12], a[13]
        if tm == 0:
            text = W.raw(2, a[11])
        elif tm == 1:
            text = (data + a[11] % size) & M32
        elif tm == 2:
            text = (data + size) & M32
        elif tm == 3:
            text = (data - 1) & M32
        else:
            text = (data + size + 1) & M32
        inside = data <= text and text < ((size + data) & M32)
        if cnt == 0:
            marks.append('n0')
        if inside:
            events.append([0x4F3E80, tg(this), tg(this), (text - data) & M32, cnt])
            marks.append('inside')
        else:
            events.append([0x4F3BD0, tg(this), cnt, 1])
            marks.append('outside')
            if tm == 2:
                marks.append('edge_end')
            if tm == 3:
                marks.append('edge_before')
            if grow & 0xFF:
                rep_movs(W, text, data, cnt)
                A.wr(o + 0x14, 4, cnt)
                W.wr((sdata(W, this) + cnt) & M32, 1, 0)
                marks.append('copy')
            else:
                marks.append('nocopy')
        ret = tg(this)
    elif k == FROMCHARS:
        length = a[4]
        for i in range(length):
            Cc.wr(a[5] + i, 1, str_char(a[6], i))
        Cc.wr(a[5] + length, 1, 0)
        text = W.raw(2, a[5])
        W.snap()
        A.wr(o + 0x18, 4, 0xF); A.wr(o + 0x14, 4, 0); A.wr(o + 4, 1, 0)
        events.append([0x4F3F60, tg(this), tg(text), length, 0xF, 0, 0])
        ret = tg(this)
        marks.append('len0' if length == 0 else ('len_long' if length > 15 else 'len_short'))
    elif k in (RTDTOR, RTWHAT, RTSCALAR):
        spec = a[4:10]
        A.wr(o, 4, a[10])
        put_str(W, 0, o + 0xC, 1, spec)
        W.snap()
        res = W.rd(this + 0x24)
        if k == RTWHAT:
            ret = tg(W.rd(this + 0x10) if res >= 0x10 else (this + 0x10) & M32)
            marks.append('sso' if res < 0x10 else ('raw' if spec[0] == 2 else 'heap'))
        else:
            body_addr = {14: 0x4F3D10, 15: 0x4F4130, 17: 0x4F3D10, 18: 0x4F4130}[fn]
            if k == RTSCALAR:
                events.append([body_addr, tg(this)])
                if a[11] & 1:
                    delete_ev(this)
                    marks.append('deleted')
                else:
                    marks.append('kept')
                if res >= 0x10:
                    marks.append('freed')
                ret = tg(this)
            else:
                A.wr(o, 4, 0x887D5C)
                if res >= 0x10:
                    delete_ev(W.rd(this + 0x10))
                    marks.append('freed')
                else:
                    marks.append('kept')
                A.wr(o + 0x24, 4, 0xF); A.wr(o + 0x20, 4, 0); A.wr(o + 0x10, 1, 0)
                events.append([0x822E2F, tg(this)])
                marks.append('sso' if res < 0x10 else 'heap')
    elif k in (RTFROMSTR, RTCOPY):
        mspec = a[4:10]
        moff = a[10]
        put_str(W, 2, moff + (0xC if k == RTCOPY else 0), 3, mspec)
        msg = W.raw(2, moff)
        W.snap()
        A.wr(o, 4, 0x887D5C)
        A.wr(o + 0x24, 4, 0xF); A.wr(o + 0x20, 4, 0); A.wr(o + 0x10, 1, 0)
        if k == RTFROMSTR:
            events.append([0x822D97, tg(this)])
            events.append([0x4F3E80, tg((this + 0xC) & M32), tg(msg), 0, 0xFFFFFFFF])
        else:
            events.append([0x822DE5, tg(this), tg(msg)])
            events.append([0x4F3E80, tg((this + 0xC) & M32), tg((msg + 0xC) & M32), 0, 0xFFFFFFFF])
        ret = tg(this)
        marks.append('msg_heap' if mspec[0] in (1, 2) else 'msg_sso')
    elif k == XLEN:
        W.snap()
        events.append([0x4F3F60, STACK_TAG, 0x860AB0, 18, 0xF, 0, 0])
        events.append([0x4F4090, STACK_TAG, STACK_TAG, 0, 0xF])
        events.append([0x822D5D, STACK_TAG, 0x893634, 0x887D68])
        threw = True
    elif k == HANDBRAKE:
        A.wr(o + 0x428, 1, a[4])
        W.snap()
        ret = (a[4] >> 5) & 1
        marks.append('on' if ret else 'off')
    elif k == ALARM:
        A.wr(o + 0x45C, 2, a[4]); A.wr(o + 0x36, 1, a[5])
        W.snap()
        al = a[4] & 0xFFFF
        if al == 0 or al == 0xFFFF:
            ret = 0
            marks.append('alarm0' if al == 0 else 'alarmFFFF')
        elif (a[5] & 0xF8) == 0x28:
            ret = 0
            marks.append('wrecked')
        else:
            ret = 1
            marks.append('active')
    elif k == PATROLSD:
        W.snap()
        events.append([0x6677A0, tg(this)])
        if a[4] & 1:
            events.append([0x41B820, tg(this)])
            marks.append('deleted')
        else:
            marks.append('kept')
        ret = tg(this)
    elif k == ABORT:
        sub, vt, route, ped = W.raw(1, a[4]), W.raw(2, a[5]), W.raw(3, a[6]), W.raw(1, a[9])
        mat = W.raw(2, a[11])
        A.wr(o + 8, 4, sub); A.wr(o + 0x1C, 4, route); A.wr(o + 0x20, 4, a[8])
        D.wr(a[6], 4, a[7])
        B.wr(a[4], 4, vt)
        B.wr(a[9] + 0x14, 4, mat if a[10] else 0)
        W.snap()
        prio, ev = a[12], a[13]
        if prio == 0:
            W.wr(route, 4, 0)
            marks.append('prio0')
        else:
            marks.append('prio_other')
        m = W.rd(ped + 0x14)
        src = (m + 0x30) & M32 if m else (ped + 4) & M32
        marks.append('matrix' if m else 'no_matrix')
        for j in range(3):
            A.wr(o + 0x24 + 4 * j, 4, W.rd(src + 4 * j))
        events.append([EV_ABORT_SLOT, tg(sub), tg(ped), prio, ev])
        al = a[14] & 0xFF
        flags = a[8]
        flags = flags ^ ((((al << 1) & M32) ^ flags) & 2)
        A.wr(o + 0x20, 4, flags)
        ret = (flags >> 1) & 1
        marks.append('aborted' if ret else 'not_aborted')
    elif k == CONTROL:
        A.wr(o, 4, W.raw(2, a[5])); A.wr(o + 8, 4, a[9]); A.wr(o + 0x20, 4, a[8])
        W.snap()
        if a[8] & 3:
            A.wr(o + 0x12, 2, 0)
            events.append([EV_CREATE_SLOT, tg(this), tg(a[10])])
            ret = tg(a[14])
            marks.append('flags_set')
        else:
            ret = tg(a[9])
            marks.append('flags_clear')
    elif k == FOLLOWDTOR:
        route = 0 if a[4] == 0 else (W.raw(3, a[5]) if a[4] == 1 else a[6])
        A.wr(o + 0x1C, 4, route)
        W.snap()
        A.wr(o, 4, 0x870114)
        if route != 0:
            events.append([0x41B820, tg(route)])
            marks.append('route_set')
        else:
            marks.append('route_null')
        events.append([0x61A3D0, tg(this)])
    elif k == CLONE:
        A.wr(o + 0x78, 4, a[4])
        for j in range(3):
            A.wr(o + 0x64 + 4 * j, 4, a[5 + j])
        A.wr(o + 0x70, 4, a[8]); A.wr(o + 0x74, 4, a[9]); A.wr(o + 0x5C, 4, a[10]); A.wr(o + 0x38, 4, a[11]); A.wr(o + 0x40, 4, a[12]); A.wr(o + 0xC, 1, a[13] & 0xFF)
        gnew = 0 if a[14] == 0 else W.raw(3, a[15])
        W.snap()
        events.append([0x61A5A0, 0x80])
        if gnew == 0:
            ret = 0
            marks.append('nomem')
        else:
            if a[4] & 2:
                events.append([0x66C450, tg(gnew), tg(this + 0x64), a[8], a[9], tg(this + 0x10), tg(this + 0x28), a[10], a[11], (a[13] >> 4) & 1, a[12]])
                marks.append('anim')
            else:
                events.append([0x66C3E0, tg(gnew), tg(this + 0x64), a[8], a[9]])
                marks.append('noanim')
            ret = tg(gnew)
    if k in RET_TAG and ret is not None and threw:
        ret = None
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
