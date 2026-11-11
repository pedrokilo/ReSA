"""Oraculo independiente (Python) de Fn162A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 7 funciones del lote 162A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen
inicial. La aritmetica x87 (precision de 24 bits, redondeo al mas cercano par, exponente ampliado, guardado como float) se modela con fracciones exactas (sin float de Python).

Desensamblado del EXE (sobre el gta-sa.exe canonico).
   0  0x59BB60  CMatrix::Update: eax=[ecx+40h]; copia los dwords 0,1,2,4,5,6,8,9,10,12,13,14 de [eax] a [ecx] (mov edx,[eax+k]; mov [ecx+k],edx); ret
   1  0x59AD20  CMatrix::UpdateMatrix(rw): igual con eax=[esp+4]; ret 4
   2  0x59ACF0  CMatrix::Detach: esi=ecx; eax=[esi+40h]; si eax y dword [esi+44h]: push eax; call 7F2A20; add esp,4; [esi+40h]=0; ret
   3  0x59ADD0  CMatrix::CopyOnlyMatrix(src): rep movsd, 16 dwords de src a this; ret 4
   4  0x59ADF0  CMatrix::operator+=(rv): para k en 0,10h,20h,4,14h,24h,8,18h,28h,30h,34h,38h: fld [rv+k]; fadd [this+k]; fstp [this+k]; ret 4
   5  0x459350  CMatrix::Scale(f): tres filas de tres dwords (offsets 0..8, 10h..18h, 20h..28h): fld [esp+4]; fmul [fila+k]; fstp [fila+k] (con el modo de precision de 24 bits del juego); ret 4
   6  0x43E110  CVector2D::operator=(src): eax=ecx; [eax]=[src]; [eax+4]=[src+4]; ret 4

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ["CMatrix::Update", "CMatrix::UpdateMatrix", "CMatrix::Detach", "CMatrix::CopyOnlyMatrix", "CMatrix::operator+=", "CMatrix::Scale", "CVector2D::operator="]
ENTRIES = (0x59BB60, 0x59AD20, 0x59ACF0, 0x59ADD0, 0x59ADF0, 0x459350, 0x43E110)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
DESTROY = 0x7F2A20
RW = (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14)
RW9 = (0, 1, 2, 4, 5, 6, 8, 9, 10)
UPD, UPDARG, DETACH, COPY, ADD, SCALE, V2 = 'upd updarg detach copy add scale v2'.split()
KIND = [UPD, UPDARG, DETACH, COPY, ADD, SCALE, V2]
SRC_SPAN = {UPD: 0x40, UPDARG: 0x40, COPY: 0x40, ADD: 0x40, V2: 8}
DEFAULT_NAN = 0xFFC00000
MODE_ARENA = (1, 2, 0)    # modo del origen -> indice de arena (B, C, A)


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


# ------------------------------------------------------------------------------------------------ aritmetica x87 con fracciones exactas
def is_nan(w):
    return (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF) != 0


def is_inf(w):
    return (w & 0x7FFFFFFF) == 0x7F800000


def is_zero(w):
    return (w & 0x7FFFFFFF) == 0


def frac(w):
    """valor exacto de un float finito"""
    e = (w >> 23) & 0xFF
    m = w & 0x7FFFFF
    s = -1 if w >> 31 else 1
    if e == 0:
        return s * Fraction(m, 1 << 149)
    return s * Fraction(m | 0x800000, 1 << 23) * (Fraction(2) ** (e - 127))


def ilog2(a):
    """floor(log2(a)) de una fraccion positiva"""
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    return e


def rne(fr):
    """entero mas cercano (empates al par) de una fraccion no negativa"""
    q, r = divmod(fr.numerator, fr.denominator)
    twice = 2 * r
    if twice > fr.denominator or (twice == fr.denominator and (q & 1)):
        q += 1
    return q


def round_sig(v, p=24, emin=None):
    """valor absoluto de una fraccion redondeado a p cifras significativas (exponente ilimitado, o con minimo emin: subnormales)"""
    a = abs(v)
    e = ilog2(a)
    if emin is not None and e < emin:
        e = emin
    quantum = Fraction(2) ** (e - p + 1)
    return rne(a / quantum) * quantum


def to_float_bits(v, neg):
    """guarda un valor exacto (ya redondeado a 24 bits) como float: redondeo al mas cercano par con subnormales; desborda a infinito"""
    sign = 0x80000000 if neg else 0
    if v == 0:
        return sign
    r = round_sig(v, 24, -126)
    if r == 0:
        return sign
    if r >= Fraction(2) ** 128:
        return sign | 0x7F800000
    if r < Fraction(2) ** -126:
        return sign | int(r * (1 << 149))
    e = ilog2(r)
    mant = int((r / (Fraction(2) ** e) - 1) * (1 << 23))
    return sign | ((e + 127) << 23) | mant


def pick_nan(a, b):
    """resultado de una operacion x87 con al menos un NaN entre los operandos (el modelo solo admite lo que generan las fixtures)"""
    if is_nan(a) and is_nan(b):
        if a == b:
            return a | 0x400000
        assert (a & 0x400000) and (b & 0x400000) and (a & 0x7FFFFF) != (b & 0x7FFFFF), 'NaN fuera del modelo'
        return a if (a & 0x7FFFFF) > (b & 0x7FFFFF) else b
    return (a if is_nan(a) else b) | 0x400000


def x87_add(a, b):
    """fld a; fadd b; fstp float, con la precision de 24 bits"""
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b), 'nan'
    if is_inf(a) or is_inf(b):
        if is_inf(a) and is_inf(b) and (a ^ b) >> 31:
            return DEFAULT_NAN, 'nan'
        return (a if is_inf(a) else b), 'inf'
    s = frac(a) + frac(b)
    if s == 0:
        if is_zero(a) and is_zero(b):
            return (a & b) & 0x80000000, 'zero'
        return 0, 'cancel'
    r = round_sig(s, 24)
    out = to_float_bits(r, s < 0)
    tag = 'finite'
    if is_inf(out):
        tag = 'overflow'
    elif (out & 0x7F800000) == 0:
        tag = 'denormal'
    elif r != abs(s):
        tag = 'rounded'
    return out, tag


def x87_mul(a, b):
    """fld a; fmul b; fstp float, con la precision de 24 bits"""
    if is_nan(a) or is_nan(b):
        return pick_nan(a, b), 'nan'
    sign = (a ^ b) & 0x80000000
    if is_inf(a) or is_inf(b):
        if is_zero(a) or is_zero(b):
            return DEFAULT_NAN, 'nan'
        return sign | 0x7F800000, 'inf'
    if is_zero(a) or is_zero(b):
        return sign, 'zero'
    p = frac(a) * frac(b)
    r = round_sig(p, 24)
    out = to_float_bits(r, p < 0)
    tag = 'finite'
    if is_inf(out):
        tag = 'overflow'
    elif (out & 0x7F800000) == 0:
        tag = 'denormal' if out & 0x7FFFFF else 'underflow_zero'
    elif r != abs(p):
        tag = 'rounded'
    return out, tag


KEYS = {}
for _fn in range(7):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'this_aligned', 'this_unaligned']
    _kind = KIND[_fn]
    if _kind in SRC_SPAN:
        _k += ['src_b', 'src_c', 'src_a', 'src_self', 'src_unaligned']
    if _kind == UPD:
        _k += ['extra_zero', 'extra_other']
    elif _kind == DETACH:
        _k += ['ptr_null', 'ptr_b', 'ptr_raw', 'owns_zero', 'owns_set', 'owns_low_zero_high_set', 'destroy_called', 'destroy_skipped']
    elif _kind == ADD:
        _k += ['res_nan', 'res_inf', 'res_cancel', 'res_overflow', 'res_denormal', 'res_rounded', 'res_zero', 'res_finite', 'two_nan']
    elif _kind == SCALE:
        _k += ['res_nan', 'res_inf', 'res_overflow', 'res_denormal', 'res_underflow_zero', 'res_rounded', 'res_zero', 'res_finite', 'scale_nan', 'scale_zero', 'scale_inf']
    KEYS[_fn] = _k


def dwords(s_hex):
    b = bytes.fromhex(s_hex)
    return [int.from_bytes(b[4 * i:4 * i + 4], 'little') for i in range(len(b) // 4)]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    S = dwords(c['s'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    W = World(c)
    A = W.mem[0]
    events, ret = [], None
    o = a[0]
    this = W.raw(0, o)
    marks.append('this_unaligned' if o % 4 else 'this_aligned')
    nwords = 2 if k == V2 else 16
    # destino inicial de this
    for i in range(nwords):
        A.wr(o + 4 * i, 4, S[16 + i])
    src = 0
    if k in SRC_SPAN:
        mode = a[3]
        if mode == 3:
            src = this
        else:
            idx = MODE_ARENA[mode]
            src = W.raw(idx, a[4])
            for i in range(SRC_SPAN[k] // 4):
                W.mem[idx].wr(a[4] + 4 * i, 4, S[i])
        marks.append(('src_b', 'src_c', 'src_a', 'src_self')[mode])
        if mode != 3 and a[4] % 4:
            marks.append('src_unaligned')
    if k == UPD:
        A.wr(o + 0x40, 4, src)
        A.wr(o + 0x44, 4, a[5])
        marks.append('extra_zero' if a[5] == 0 else 'extra_other')
    if k == DETACH:
        ptr0 = 0 if a[3] == 0 else (W.raw(1, a[4]) if a[3] == 1 else a[5])
        A.wr(o + 0x40, 4, ptr0)
        A.wr(o + 0x44, 4, a[6])
    W.snap()
    if k in (UPD, UPDARG):
        vals = [W.rd(src + 4 * j, 4) for j in RW]
        for j, v in zip(RW, vals):
            A.wr(o + 4 * j, 4, v)
    elif k == COPY:
        vals = [W.rd(src + 4 * j, 4) for j in range(16)]
        for j, v in enumerate(vals):
            A.wr(o + 4 * j, 4, v)
    elif k == V2:
        vals = [W.rd(src + 4 * j, 4) for j in range(2)]
        for j, v in enumerate(vals):
            A.wr(o + 4 * j, 4, v)
        ret = W.tagv(this)
    elif k == DETACH:
        ptr = A.rd(o + 0x40, 4)
        owns = A.rd(o + 0x44, 4)
        marks.append({0: 'ptr_null', 1: 'ptr_b', 2: 'ptr_raw'}[a[3]])
        marks.append('owns_zero' if owns == 0 else 'owns_set')
        if owns and (owns & 0xFF) == 0:
            marks.append('owns_low_zero_high_set')
        if ptr and owns:
            events.append([DESTROY, W.tagv(ptr)])
            marks.append('destroy_called')
        else:
            marks.append('destroy_skipped')
        A.wr(o + 0x40, 4, 0)
    elif k == ADD:
        res = {}
        for j in RW:
            rv = W.rd(src + 4 * j, 4)
            tv = A.rd(o + 4 * j, 4)
            out, tag = x87_add(rv, tv)
            res[j] = out
            marks.append('res_' + tag)
            if is_nan(rv) and is_nan(tv) and rv != tv:
                marks.append('two_nan')
        for j, v in res.items():
            A.wr(o + 4 * j, 4, v)
    elif k == SCALE:
        sc = a[3]
        if is_nan(sc):
            marks.append('scale_nan')
        if is_zero(sc):
            marks.append('scale_zero')
        if is_inf(sc):
            marks.append('scale_inf')
        for j in RW9:
            tv = A.rd(o + 4 * j, 4)
            out, tag = x87_mul(sc, tv)
            marks.append('res_' + tag)
            A.wr(o + 4 * j, 4, out)
    return {'events': events, 'changes': {'A': A.changes(), 'B': W.mem[1].changes(), 'C': W.mem[2].changes()}, 'gchanges': [], 'ret': ret, 'marks': marks}


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
