"""Oraculo independiente (Python) de Fn95A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de los nueve metodos del lote 95A
(D3DTextureBuffer::Setup 0x72FE80, Pop 0x72FF60, Push 0x72FFF0, Resize 0x730020, GetTotalDataSize 0x7300A0, PushWithoutIncreasingCounter 0x730AD0 y
D3DIndexDataBuffer::Pop 0x730270, Resize 0x730330, GetTotalDataSize 0x7303B0) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Desensamblado (EXE canonico; D3DTextureBuffer: +0 formato, +4 ancho, +8 niveles, +0xC capacidad, +0x10 contador, +0x14 cantidad, +0x18 pila; D3DIndexDataBuffer: +0 formato, +4 campo4,
+8 capacidad, +0xC contador, +0x10 cantidad, +0x14 pila). Las ranuras COM son stdcall: Release 2, GetLevelCount 13 (0x34), GetLevelDesc 17 (0x44), IndexBuffer GetDesc 13 (0x34):
  0x72FE80: [+0]=formato [+4]=ancho [+8]=niveles; pila = new(capacidad*4); [+0xC]=capacidad [+0x14]=0 [+0x10]=0
  0x72FF60: n=[+0x14]; si 0 -> 0; bucle desde n-1 hacia 0: t=pila[i]; c=t->GetLevelCount(); si (c==1) == niveles: t->GetLevelDesc(0,&d); si d.formato,ancho,alto coinciden: [+0x14]--; si [+0x14]>0 e i != [+0x14]:
            pila[i]=pila[[+0x14]]; devuelve t; si no hay, 0
  0x72FFF0: [+0x10]++; si [+0x14] (con signo) < [+0xC]: pila[[+0x14]]=t; [+0x14]++; AL=1; si no AL=0
  0x730AD0: igual sin tocar [+0x10]
  0x730020: si capacidad == [+0xC] sale; p=new(capacidad*4); copia pila[i] a p[i] mientras i < [+0x14] e i < capacidad (con signo); si capacidad < [+0x14]: Release de pila[capacidad..[+0x14]-1];
            delete(pila vieja, aunque sea nula); pila=p; [+0xC]=capacidad (no toca [+0x14])
  0x7300A0: total=0; formato==0: para i < [+0x14] (con signo): t->GetLevelDesc(0,&d); t->GetLevelCount(); si cuenta==0: p=(int32)(alto*ancho); v=fild(p); si p<0: v+=2^32; v*=4/3f; v+=total (fiadd);
            total=ftol(v) (cada operacion x87 redondeada a 24 bits); si no: total+=alto*ancho (32 bits). Otro formato: si [+0x14]==0 -> 0; GetLevelDesc de pila[0]; s=[+0x14]*alto*ancho (32 bits);
            GetLevelCount de pila[0] == 0 -> ftol(fild(s)*4/3f) si no s
  Indice 0x730270: n=[+0x10]; mejor=-1; mejorTam=0x7FFFFFFF; minimo=indices*2 (32 bits); si n==0 -> 0; bucle desde n-1: ib->GetDesc(&d); si d.tam < mejorTam y d.tam >= minimo (sin signo): mejor=i;
            si mejor==-1 -> 0; r=pila[mejor]; [+0x10]--; si [+0x10]>0 y [+0x10] != mejor: pila[mejor]=pila[[+0x10]]; devuelve r
  Indice 0x730330: como 0x730020 con los campos del indice
  Indice 0x7303B0: formato==0: suma de d.tam de GetDesc de pila[0..[+0x10]-1] (32 bits); otro formato: si [+0x10]==0 -> 0; GetDesc de pila[0]; [+0x10]*d.tam (32 bits)

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['D3DTextureBuffer::Setup', 'D3DTextureBuffer::Pop', 'D3DTextureBuffer::Push', 'D3DTextureBuffer::Resize', 'D3DTextureBuffer::GetTotalDataSize',
         'D3DTextureBuffer::PushWithoutIncreasingCounter', 'D3DIndexDataBuffer::Pop', 'D3DIndexDataBuffer::Resize', 'D3DIndexDataBuffer::GetTotalDataSize']
ADDRESS = [0x72FE80, 0x72FF60, 0x72FFF0, 0x730020, 0x7300A0, 0x730AD0, 0x730270, 0x730330, 0x7303B0]
CASES = 16384
SLOTS = 24
GUARD = 8
REST = 0x5C5C5C5C
GUARDW = 0x6D6D6D6D
BLOCK = 0xB1B1B1B1
# etiquetas (Tag de Core.hpp): arena 1 = pila vieja, 2 = objetos (0x40 bytes), 3 = pila nueva; el puntero de la pila apunta tras las 8 guardas
STK_PTR = 0x10000000 + (1 << 20) + GUARD * 4
NEW_PTR = 0x10000000 + (3 << 20) + GUARD * 4
OBJ_BASE = 0x10000000 + (2 << 20)

def f32_value(bits):
    s = -1 if bits >> 31 else 1
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    assert 0 < e < 0xFF
    return s * Fraction(0x800000 | m, 1 << 23) * (Fraction(2) ** (e - 127))


FACTOR = f32_value(0x3FAAAAAB)          # 4/3 como float (0x85F09C)
TWO32 = f32_value(0x4F800000)           # 2^32 como float (0x858C54)
assert FACTOR == Fraction(11184811, 8388608) and TWO32 == 1 << 32


def r24(x):
    """Redondeo al mas cercano (empate al par) a 24 bits de mantisa: precision de la x87 con el dispositivo D3D."""
    if x == 0:
        return Fraction(0)
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    scale = Fraction(2) ** (e - 23)
    q = a / scale
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n & 1):
        n += 1
    return s * n * scale


def ftol(x):
    """__ftol: truncamiento hacia cero y palabra baja de 32 bits."""
    n = x.numerator // x.denominator if x >= 0 else -((-x.numerator) // x.denominator)
    return n & M32


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def mk(c):
    """Estado inicial desde la entrada: palabras del buffer, pila (24 entradas), objetos."""
    fn = c['fn']
    tex = fn <= 5
    nw = 7 if tex else 6
    buf = list(c['buf'])
    assert len(buf) == nw and buf[-1] == STK_PTR
    stk = list(c.get('stk', []))
    stk += [REST] * (SLOTS - len(stk))
    return tex, nw, buf, stk, c.get('obj', [])


def obj_index(v):
    assert OBJ_BASE <= v < OBJ_BASE + 10 * 0x40 and (v - OBJ_BASE) % 0x40 == 0, hex(v)
    return (v - OBJ_BASE) // 0x40


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    tex, nw, buf, stk, objs = mk(c)
    cap_i, ctr_i, size_i = (3, 4, 5) if tex else (2, 3, 4)
    full_stk = [GUARDW] * GUARD + stk + [GUARDW] * GUARD
    init_stk = list(full_stk)
    events = []
    ret = None
    blk = {}
    args = c['args']
    size = buf[size_i]
    cap = buf[cap_i]

    def get(i):
        return full_stk[GUARD + i] if -GUARD <= i < SLOTS + GUARD else None

    def put(i, v):
        assert -GUARD <= i < SLOTS + GUARD
        full_stk[GUARD + i] = v

    if fn == 0:
        buf[0], buf[1], buf[2] = args[0], args[1], args[2]
        events.append([1, (args[3] * 4) & M32])
        buf[3] = args[3]
        buf[5] = 0
        buf[4] = 0
        buf[6] = NEW_PTR
        mark('cap_zero' if args[3] == 0 else ('cap_16' if args[3] == 16 else 'cap_other'))
        if args[3] & 0x80000000:
            mark('cap_big')
    elif fn in (2, 5):
        if fn == 2:
            buf[4] = (buf[4] + 1) & M32
        if s32(size) < s32(cap):
            put(s32(size), args[0])
            buf[5] = (size + 1) & M32
            ret = 1
            mark('stored')
            if s32(size) < 0:
                mark('stored_guard')
        else:
            ret = 0
            mark('full')
        if fn == 2:
            mark('counter_incremented')
    elif fn == 1:
        fmt, w, h, levels = args
        n = size
        res = 0
        if n == 0:
            mark('empty')
        else:
            i = n
            while True:
                i -= 1
                t = stk[i]
                o = objs[obj_index(t)]
                events.append([4, t])
                one = 1 if o[0] == 1 else 0
                mark('one_level' if one else 'multi_level')
                if one == levels:
                    events.append([5, t, 0])
                    if o[1] == fmt and o[2] == w and o[3] == h:
                        last = (buf[5] - 1) & M32
                        buf[5] = last
                        if s32(last) > 0 and last != i:
                            put(i, get(last))
                            mark('swap')
                        else:
                            mark('no_swap')
                        res = t
                        break
                    mark('desc_mismatch')
                else:
                    mark('levels_mismatch')
                if i == 0:
                    break
            mark('found' if res else 'not_found')
        ret = res
    elif fn in (3, 7):
        nc = args[0]
        if nc == cap:
            mark('same_capacity')
        else:
            events.append([1, (nc * 4) & M32])
            n = s32(size)
            ncs = s32(nc)
            i = 0
            while n > i and i < ncs:
                blk[i] = stk[i]
                i += 1
            copied = i
            if copied:
                mark('copies')
            if ncs < n:
                for j in range(ncs, n):
                    events.append([3, stk[j]])
                mark('releases')
            events.append([2, STK_PTR])
            buf[-1] = NEW_PTR
            buf[cap_i] = nc
            if nc > cap:
                mark('grows')
            elif nc < cap:
                mark('shrinks')
    elif fn == 4:
        fmt = buf[0]
        n = s32(size)
        total = 0
        if fmt == 0:
            mark('generic')
            i = 0
            while i < n:
                t = stk[i]
                o = objs[obj_index(t)]
                events.append([5, t, 0])
                events.append([4, t])
                prod = (o[3] * o[2]) & M32
                if o[0] == 0:
                    p = s32(prod)
                    v = Fraction(p)
                    if p < 0:
                        v = r24(v + TWO32)
                        mark('unsigned_fixup')
                    v = r24(v * FACTOR)
                    v = r24(v + total)
                    total = s32(ftol(v))
                    mark('x87')
                else:
                    total = s32(total + prod)
                    mark('plain')
                i += 1
            ret = total & M32
            if n <= 0:
                mark('empty')
        else:
            mark('single_format')
            if size == 0:
                ret = 0
                mark('empty')
            else:
                t = stk[0]
                o = objs[obj_index(t)]
                events.append([5, t, 0])
                s = (size * o[3] * o[2]) & M32
                events.append([4, t])
                if o[0] == 0:
                    v = r24(Fraction(s32(s)) * FACTOR)
                    ret = ftol(v)
                    mark('x87_single')
                else:
                    ret = s
                    mark('plain_single')
                if s32(size) < 0:
                    mark('negative_size')
    elif fn == 6:
        count = args[0]
        n = size
        best = -1
        best_size = 0x7FFFFFFF
        minimum = (count * 2) & M32
        res = 0
        if n == 0:
            mark('empty')
        else:
            i = n
            while True:
                i -= 1
                t = stk[i]
                sz = objs[obj_index(t)][0]
                events.append([6, t])
                if sz < best_size and sz >= minimum:
                    best_size = sz
                    best = i
                if i == 0:
                    break
            if best == -1:
                mark('not_found')
            else:
                mark('found')
                res = stk[best]
                last = (buf[4] - 1) & M32
                buf[4] = last
                if s32(last) > 0 and last != best:
                    put(best, get(last))
                    mark('swap')
                else:
                    mark('no_swap')
        ret = res
    else:
        fmt = buf[0]
        n = s32(size)
        if fmt == 0:
            mark('generic')
            total = 0
            for i in range(max(n, 0)):
                t = stk[i]
                events.append([6, t])
                total = (total + objs[obj_index(t)][0]) & M32
            ret = total
            if n <= 0:
                mark('empty')
        else:
            mark('single_format')
            if size == 0:
                ret = 0
                mark('empty')
            else:
                t = stk[0]
                events.append([6, t])
                ret = (size * objs[obj_index(t)][0]) & M32
    out = {'events': events, 'buf': buf, 'marks': marks, 'guard': [], 'objs_intact': True}
    if ret is not None:
        out['ret'] = ret
    out['stk_changes'] = [[i - GUARD, full_stk[i]] for i in range(len(full_stk)) if full_stk[i] != init_stk[i]]
    out['blk_changes'] = [[i, v] for i, v in sorted(blk.items())]
    return out


def check_side(c, o, e):
    for k in ('ret', 'buf', 'guard', 'stk_changes', 'blk_changes', 'objs_intact'):
        if k in e and o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if 'ret' not in e and 'ret' in o:
        return 'ret inesperado'
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (str(o['events'])[:300], str(e['events'])[:300])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
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
