"""Oraculo independiente del banco AutoRender75: CAutomobile::Render (EXE 0x6A2B10, virtual).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco; el oraculo no repite ninguna constante de arena.
Aritmetica: x87 con control de precision sencillo (24 bits de mantisa, exponente extendido) y redondeo al mas proximo par, calculada con fracciones EXACTAS (sin doble redondeo):
cada valor de registro se redondea a 24 bits sin limite de exponente (los desnormales y los valores fuera del rango float sobreviven en el registro) y solo los guardados en memoria
(`fstp dword`) se redondean a float (desnormales, desbordamiento a infinito). Reglas de NaN/infinito de la x87 (un solo NaN: ese NaN silenciado; dos NaN: el de mayor mantisa y, a igualdad,
el positivo; inf*0 e inf-inf: indefinido 0xFFC00000; x/0: infinito con signo; 0/0: indefinido). `fld dword` silencia los NaN de senal.

Resumen del EXE (0x6A2B10..0x6A2EFC): [+0x4D0] = [0xB7CB84] + 3000; RenderStateGet(30, &v) con v = 1 previo y RenderStateSet(30, 1); truco 27 (coche invisible) activo -> solo las ruedas
(nodos 4, 7, 2, 5 y 3, 6 si no son nulos; GetFirstObject dos veces por nodo y renderCallBack del primer atomico), si no CVehicle::Render; modelo 432 (Rhino): 8 pasos (lineas 4..11), con
  ratio = (linea0 - muelle0) / linea0, v = pista - ratio (0.0 si 0.0 > v), u = 1 - v, sx = float(u * inicio.x), sy = float(u * inicio.y), ez = float(v * fin.z),
  x = float(v * fin.x + sx), y = float(v * fin.y + sy), z = float(ez + u * inicio.z) (el producto u * inicio.z queda en el registro),
  move = float(x,y,z - posicion del nodo) (nodo 7 en los 4 primeros, nodo 4 en los otros 4), RwMatrixTranslate(move), RwFrameUpdateObjects, GetFirstObject x2 + renderCallBack, move *= -1.0f
  (fmul + fstp dword), RwMatrixTranslate(move); despues RenderStateSet(30, guardado) y, salvo modelos 441/-2/432, bit 7 de +0x42C y apariencia 3, los haces de faros (+0x584 bit 0, bit 1).
"""
import json
import sys
from fractions import Fraction as Fr

M32 = 0xFFFFFFFF
CASES = 16384
NAME = 'CAutomobile::Render'
FAMILIES = 16
QNAN_INDEFINITE = 0xFFC00000


# ----------------------------------------------------------------------------------------------- aritmetica x87 exacta
class V:
    """k: 'f' finito (v Fraction; s = signo del cero), 'i' infinito (s), 'n' NaN (s, v = mantisa de 23 bits con el bit de silencio)."""
    __slots__ = ('k', 's', 'v')

    def __init__(self, k, s=0, v=Fr(0)):
        self.k, self.s, self.v = k, s, v

    def is_zero(self):
        return self.k == 'f' and self.v == 0


def pow2(q):
    return Fr(1 << q) if q >= 0 else Fr(1, 1 << -q)


def round_sig(x, mant, minq=None):
    """x > 0 (Fraction) redondeado al mas proximo par a `mant` bits significativos; `minq` fija el cuanto minimo 2**minq (desnormales)."""
    e = x.numerator.bit_length() - x.denominator.bit_length()
    while pow2(e) > x:
        e -= 1
    while pow2(e + 1) <= x:
        e += 1
    q = e - (mant - 1)
    if minq is not None and q < minq:
        q = minq
    scaled = x / pow2(q)
    m = scaled.numerator // scaled.denominator
    rem = scaled - m
    if rem > Fr(1, 2) or (rem == Fr(1, 2) and (m & 1)):
        m += 1
    return Fr(m) * pow2(q)


def fin(x, zero_sign=0):
    if x == 0:
        return V('f', zero_sign, Fr(0))
    return V('f', 0, x)


def r24(x):
    """Resultado de una operacion x87 a 24 bits (exponente extendido): x es un Fraction exacto."""
    if x == 0:
        return V('f', 0, Fr(0))
    return V('f', 0, round_sig(abs(x), 24) * (1 if x > 0 else -1))


def load(bits):
    """fld dword: float -> registro (los NaN de senal se silencian)."""
    bits &= M32
    s = bits >> 31
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return V('i', s) if m == 0 else V('n', s, m | 0x400000)
    if e == 0:
        val = Fr(m) * pow2(-149)
    else:
        val = Fr(m | 0x800000) * pow2(e - 150)
    if m == 0 and e == 0:
        return V('f', s, Fr(0))
    return V('f', 0, -val if s else val)


def st(v):
    """fstp dword: registro -> bits de float (desnormales, desbordamiento a infinito)."""
    if v.k == 'n':
        return (v.s << 31) | 0x7F800000 | v.v
    if v.k == 'i':
        return (v.s << 31) | 0x7F800000
    if v.v == 0:
        return v.s << 31
    s = 1 if v.v < 0 else 0
    r = round_sig(abs(v.v), 24, -149)
    if r >= pow2(128):
        return (s << 31) | 0x7F800000
    if r < pow2(-126):
        return (s << 31) | int(r / pow2(-149))
    e = r.numerator.bit_length() - r.denominator.bit_length()
    while pow2(e) > r:
        e -= 1
    while pow2(e + 1) <= r:
        e += 1
    mant = int(r / pow2(e - 23)) - (1 << 23)
    return (s << 31) | ((e + 127) << 23) | mant


def neg_sign(v):
    return (1 if v.v < 0 else 0) if (v.k == 'f' and v.v != 0) else v.s


NAN_CONFLICTS = [0]   # operaciones con dos NaN distintos (la regla de mayor mantisa se usa, pero las fixtures procuran evitarlo)


def nan_pick(a, b):
    if a.k == 'n' and b.k == 'n':
        if (a.s, a.v) != (b.s, b.v):
            NAN_CONFLICTS[0] += 1
        if a.v != b.v:
            return a if a.v > b.v else b
        return a if a.s <= b.s else b
    if a.k == 'n':
        return a
    if b.k == 'n':
        return b
    return None


INDEF = V('n', 1, 0x400000)


def mul(a, b):
    n = nan_pick(a, b)
    if n:
        return n
    sa, sb = neg_sign(a), neg_sign(b)
    s = sa ^ sb
    if a.k == 'i' or b.k == 'i':
        if a.is_zero() or b.is_zero():
            return INDEF
        return V('i', s)
    if a.v == 0 or b.v == 0:
        return V('f', s, Fr(0))
    return r24(a.v * b.v)


def add(a, b):
    n = nan_pick(a, b)
    if n:
        return n
    if a.k == 'i' or b.k == 'i':
        if a.k == 'i' and b.k == 'i':
            return V('i', a.s) if a.s == b.s else INDEF
        return a if a.k == 'i' else b
    if a.v == 0 and b.v == 0:
        return V('f', 1 if (a.s and b.s) else 0, Fr(0))
    return r24(a.v + b.v)


def negate(v):
    if v.k == 'n':
        return v
    if v.k == 'i':
        return V('i', 1 - v.s)
    if v.v == 0:
        return V('f', 1 - v.s, Fr(0))
    return V('f', 0, -v.v)


def sub(a, b):
    n = nan_pick(a, b)
    if n:
        return n
    return add(a, negate(b))


def div(a, b):
    n = nan_pick(a, b)
    if n:
        return n
    s = neg_sign(a) ^ neg_sign(b)
    if a.k == 'i':
        return INDEF if b.k == 'i' else V('i', s)
    if b.k == 'i':
        return V('f', s, Fr(0))
    if b.v == 0:
        return INDEF if a.v == 0 else V('i', s)
    if a.v == 0:
        return V('f', s, Fr(0))
    return r24(a.v / b.v)


def lt_zero(v):
    """0.0 > v (fcomp + test ah,41h): falso con NaN; -0 no es menor."""
    return v.k == 'i' and v.s == 1 or (v.k == 'f' and v.v < 0)


ZERO = V('f', 0, Fr(0))
ONE = V('f', 0, Fr(1))
MINUS_ONE_BITS = 0xBF800000


def runs(old, new, base=0):
    """Tramos contiguos que cambian: [desplazamiento, hex nuevo], como DiffRuns de Core.hpp."""
    out = []
    i = 0
    while i < len(old):
        if old[i] == new[i]:
            i += 1
            continue
        j = i
        while j < len(old) and old[j] != new[j]:
            j += 1
        out.append([base + i, bytes(new[i:j]).hex()])
        i = j
    return out


def matches(observed, expected):
    return observed == expected


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_inf(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def rhino_steps(inp, sse=False):
    """Las 8 vueltas de las orugas. sse=True: cada operacion se redondea a float (modelo alternativo, solo para medir la sensibilidad de las fixtures)."""
    def R(v):
        return load(st(v)) if sse else v

    posbits = {n: [inp['fpos'][n - 2][k] for k in range(3)] for n in (4, 7)}
    L0, S0 = load(inp['line0']), load(inp['spring0'])
    ratio = R(div(R(sub(L0, S0)), L0))
    trace = []
    info = dict(stored=[], regs=[ratio])
    for i in range(8):
        T = load(inp['tracks'][i])
        v = R(sub(T, ratio))
        if lt_zero(v):
            v = ZERO
        u = R(sub(ONE, v))
        line = inp['lines'][4 + i]
        s = [load(x) for x in line[0:3]]
        e = [load(x) for x in line[3:6]]
        sx = load(st(R(mul(u, s[0]))))
        sy = load(st(R(mul(u, s[1]))))
        usz = R(mul(u, s[2]))
        vex = R(mul(v, e[0]))
        vey = R(mul(v, e[1]))
        ez = load(st(R(mul(v, e[2]))))
        xb = st(R(add(vex, sx)))
        yb = st(R(add(vey, sy)))
        zb = st(R(add(ez, usz)))
        node = 7 if i < 4 else 4
        mv = [st(R(sub(load(b), load(p)))) for b, p in zip((xb, yb, zb), posbits[node])]
        trace.append(['xlate', 'M%d' % node] + mv + [2])
        posbits[node] = [st(R(add(load(p), load(m)))) for p, m in zip(posbits[node], mv)]
        trace += [['upd', 'F%d' % node], ['gfo', 'F%d' % node, 'A'], ['gfo', 'F%d' % node, 'B'], ['rcb', 'B%d' % node]]
        ng = [st(R(mul(load(m), load(MINUS_ONE_BITS)))) for m in mv]
        trace.append(['xlate', 'M%d' % node] + ng + [2])
        posbits[node] = [st(R(add(load(p), load(m)))) for p, m in zip(posbits[node], ng)]
        info['stored'] += [st(sx), st(sy), st(ez), xb, yb, zb] + mv + ng + posbits[node]
        info['regs'] += [v, u, usz, vex, vey]
    return trace, posbits, info


def expected_r(inp, sse=False):
    st_ = {}
    trace = [['sget', 30], ['sset', 30, 1]]
    model = inp['model']
    pos = {n: [b for b in inp['fpos'][n - 2]] for n in range(2, 8)}
    st_['cheat'] = bool(inp['cheat'])
    if inp['cheat']:
        order = [4, 7, 2, 5] + ([] if inp['node3_null'] else [3]) + ([] if inp['node6_null'] else [6])
        for n in order:
            trace += [['gfo', 'F%d' % n, 'A'], ['gfo', 'F%d' % n, 'B'], ['rcb', 'B%d' % n]]
        st_['wheel_nodes'] = len(order)
    else:
        trace.append(['vrender', 'car'])
        st_['wheel_nodes'] = 0
    st_['rhino'] = model == 432
    st_['family'] = inp['family']
    st_['denormal_store'] = st_['inf_store'] = st_['nan_store'] = st_['noncanon_nan_store'] = st_['negzero_store'] = False
    st_['ext_big'] = st_['ext_tiny'] = st_['v_clamped'] = False
    st_['ratio_nonfinite'] = False
    if model == 432:
        trace.append(['col', 'car'])
        t, pb, info = rhino_steps(inp, sse)
        trace += t
        for n in pb:
            pos[n] = pb[n]
        for b in info['stored']:
            st_['denormal_store'] |= is_denormal(b)
            st_['inf_store'] |= is_inf(b)
            st_['nan_store'] |= is_nan(b)
            st_['noncanon_nan_store'] |= is_nan(b) and (b & 0x7FFFFFFF) != 0x7FC00000
            st_['negzero_store'] |= b == 0x80000000
        ratio = info['regs'][0]
        st_['ratio_nonfinite'] = ratio.k != 'f'
        for r in info['regs']:
            if r.k == 'f' and r.v != 0:
                a = abs(r.v)
                st_['ext_big'] |= a >= pow2(128)
                st_['ext_tiny'] |= a < pow2(-149)
        tracks = [load(x) for x in inp['tracks']]
        st_['v_clamped'] = any(lt_zero(sub(tr, ratio)) for tr in tracks)
    saved = (inp['get_val'] & M32) if inp['get_writes'] else 1
    trace.append(['sset', 30, saved])
    st_['beam_r'] = st_['beam_l'] = False
    st_['appear'] = False
    if model not in (441, -2, 432) and not (inp['rc'] & 0x80):
        trace.append(['appear', 'car'])
        st_['appear'] = True
        if inp['appearance'] != 3:
            if inp['lights'] & 1:
                trace.append(['beam', 'car', 0, 'matrix', 1])
                st_['beam_r'] = True
            if inp['lights'] & 2:
                trace.append(['beam', 'car', 0, 'matrix', 0])
                st_['beam_l'] = True
    new_time = (inp['time'] + 3000) & M32
    st_['time_wrap'] = inp['time'] + 3000 > M32
    st_['model_class'] = 'rhino' if model == 432 else 'bandit' if model == 441 else 'minus2' if model == -2 else 'other'
    import struct
    car_diff = runs(struct.pack('<I', inp['old4d0']), struct.pack('<I', new_time), 0x4D0)
    frames = [list(pos[n]) for n in range(2, 8)]
    return {'car_diff': car_diff, 'guard_ok': True, 'frames': frames, 'frames_rest_ok': True, 'arenas_unchanged': True, 'trace': trace}, st_


def expected(inp):
    before = NAN_CONFLICTS[0]
    obs, stats = expected_r(inp)
    stats['nan_conflict'] = NAN_CONFLICTS[0] != before
    if inp['model'] == 432:
        alt, _ = expected_r(inp, sse=True)
        stats['sse_differs'] = alt != obs
    else:
        stats['sse_differs'] = False
    return obs, stats


def tally(stats, cov):
    def bump(key, n=1):
        cov[key] = cov.get(key, 0) + n
    for k in ('cheat', 'rhino', 'appear', 'beam_r', 'beam_l', 'time_wrap', 'denormal_store', 'inf_store', 'nan_store', 'noncanon_nan_store', 'negzero_store',
              'ext_big', 'ext_tiny', 'v_clamped', 'ratio_nonfinite', 'sse_differs', 'nan_conflict'):
        bump(k, 1 if stats.get(k) else 0)
    bump('model=' + stats['model_class'])
    bump('family=%d' % stats['family'])
    bump('wheel_nodes=%d' % stats['wheel_nodes'])


def selftest():
    assert st(load(0x3F800000)) == 0x3F800000
    assert st(r24(Fr(1) + pow2(-24))) == 0x3F800000                      # empate -> par
    assert st(r24(Fr(1) + pow2(-24) + pow2(-40))) == 0x3F800001
    assert st(load(1)) == 1 and st(load(0x80000001)) == 0x80000001
    tiny = r24(pow2(-150))                                                # media unidad desnormal -> par = 0
    assert st(tiny) == 0
    assert st(r24(pow2(-150) * Fr(3))) == 2                               # 1.5 unidades -> 2
    assert st(r24(pow2(127) * Fr(2) - pow2(103))) == 0x7F800000          # >= max + media ulp -> infinito
    assert st(add(load(0xBF800000), load(0x3F800000))) == 0              # +0
    assert st(add(load(0x80000000), load(0x80000000))) == 0x80000000
    assert st(mul(load(0x7F800000), load(0))) == QNAN_INDEFINITE
    assert st(sub(load(0x7F800000), load(0x7F800000))) == QNAN_INDEFINITE
    assert st(load(0x7FA00001)) == 0x7FE00001                             # fld silencia
    assert st(div(load(0x3F800000), load(0x80000000))) == 0xFF800000
    assert st(div(load(0), load(0))) == QNAN_INDEFINITE
    assert not lt_zero(load(0x80000000)) and lt_zero(load(0xBF800000)) and not lt_zero(load(0x7FC00001))
    # el producto desnormal queda en el registro: 1e-40 * 0.3 + 1e-40 * 0.2 no se redondea a la rejilla desnormal antes de sumar
    a = mul(load(0x00006D9A), load(0x3E99999A))
    b = mul(load(0x00006D9A), load(0x3E4CCCCD))
    assert a.v != 0 and abs(a.v) < pow2(-126)
    print('selftest ok')


def coverage(path):
    cov = {}
    n = 0
    for line in open(path, encoding='utf-8'):
        r = json.loads(line)
        exp, stats = expected(r['input'])
        json.dumps(exp)
        tally(stats, cov)
        n += 1
    assert n == CASES, n
    print(json.dumps(dict(sorted(cov.items())), indent=1))
    return cov


def reanalyze(path):
    ok = bad = 0
    cov = {}
    for line in open(path, encoding='utf-8'):
        r = json.loads(line)
        if r.get('event') != 'comparison':
            continue
        want, stats = expected(r['input'])
        tally(stats, cov)
        for side in ('original', 'candidate'):
            if matches(r[side]['observed'], want):
                ok += 1
            else:
                bad += 1
    print('lados iguales al modelo: %d; distintos: %d' % (ok, bad))
    print(json.dumps(dict(sorted(cov.items())), indent=1))
    return bad


if __name__ == '__main__':
    if len(sys.argv) == 2 and sys.argv[1] == '--selftest':
        selftest()
    elif len(sys.argv) == 3 and sys.argv[1] == '--coverage':
        coverage(sys.argv[2])
    elif len(sys.argv) == 2:
        sys.exit(1 if reanalyze(sys.argv[1]) else 0)
    else:
        raise SystemExit('uso: fixture_oracle.py --selftest | <comparisons.jsonl>')
