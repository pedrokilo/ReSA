"""Oraculo independiente del banco WaterLevel62: cinco funciones de CWaterLevel, modeladas desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL)
a partir de la fila `input` que registra el banco. Devuelve lo que debe observarse en AMBOS lados (EXE con el hook apagado y DLL con el hook encendido).

--------------------------------------------------------------------------------------------------------------------------------
CWaterLevel::FillQuadsAndTrianglesList (0x6E7B30, cdecl, void)
  para a = 0..11 (x), b = 0..11 (y):  xLo = 500a - 3000, xHi = xLo + 500, yLo = 500b - 3000, yHi = yLo + 500 (floats exactos en la x87)
    6E7B81  si NumWaterQuads (0xC22888, con signo) <= 0 salta los cuadrilateros; si no, para i = 0..:
            6E7B90..6E7C0B  v1.x > xLo, v0.x < xHi, v2.y > yLo, v0.y < yHi (comparaciones ordenadas de fild de movsx word; vertice = 20 bytes desde 0xC22910, indice extendido con signo)
            -> call 0x6E5750(a, b, i, 1) cdecl;  6E7C1A: relee NumWaterQuads tras cada indice (jl con signo)
    6E7C2B  edi = NumWaterTriangles (0xC22884) UNA vez por bloque; si <= 0 nada; para i = 0..edi-1 (triangulos de 8 bytes desde 0xC22854):
            v1.x > xLo, v0.x < xHi, max(v0.y, v2.y) > yLo, min(v0.y, v2.y) < yHi (y comparadas como int16 con signo) -> call 0x6E5750(a, b, i, 2)

CWaterLevel::GetWaterDepth (0x6EA960, cdecl (const CVector* pos, float* depth, float* level, float* ground), bool en AL)
  6E8580(pos.x, pos.y, pos.z, &nivel, 0, 0) cdecl (nivel = la ranura de pila del primer argumento, que antes contenia el puntero pos); AL == 0 -> return 0
  6EA8A0(pos, &suelo, 0, 30.0f) cdecl; AL == 0 -> suelo = -100.0f (0x859014), si no el valor que dejo en la ranura (fld: un SNaN sale silenciado)
  si depth != 0: *depth = nivel - suelo (fld nivel; fsub st,st(1); fstp, 24 bits); si level != 0: *level = nivel (copia de 32 bits); si ground != 0: *ground = suelo (fstp)
  return 1. Orden de escrituras: depth, level, ground (si hay alias gana la ultima).

CWaterLevel::RenderHighDetailWaterRectangle (0x6EB810, cdecl, minX, maxX, Y1, Y2, CRenPar x4 por valor)
  centro = (float(fild(minX + maxX) * 0.5), float(fild(Y1 + Y2) * 0.5), P1.z); radio = float(sqrt(hy*hy + hx*hx)), hx = fild(maxX - minX) * 0.5, hy = fild(Y1 - Y2) * 0.5 (24 bits por operacion)
  420C40(centro, radio, 0xB6FA74) thiscall (ECX = 0xB6F028); si AL == 0 y el byte 0xB6F998 != 0: 420C40(centro, radio, 0xB6FABC); si sigue siendo 0: return
  minY = min(Y1, Y2), maxY = max; hx = (maxX - minX)/2 y hy = (maxY - minY)/2 (truncando, envoltura de 32 bits); twice = 2*hy*hx; tri = (hy+1)*(hx+1)
  si 3*twice < 0x1000 y tri < 0x800 (con signo): 6EA9F0(minX, minY, maxX, maxY) y 6E91D0(minX, maxX, Y1, Y2, P1..P4, capa, twice, tri, hx, hy) para capa = 0 y 1
  si no, si hx > hy: 6E73A0(minX + (hx/2)*2, minX, maxX, Y1, Y2, P1..P4)
  si no (Y): splitY = minY + (hy/2)*2; t = float(fild(splitY - Y1) / fidiv(Y2 - Y1)); P13 = 6E5280(P1, P3, t), P24 = 6E5280(P2, P4, t): 6EC5D0(minX, maxX, Y1, splitY, P1, P2, P13, P24);
  6EC5D0(minX, maxX, splitY, Y2, P13, P24, P3, P4). lerp: (1 - t) * a + t * b (24 bits por operacion), flowX = flowY = 0.

CWaterLevel::RenderHighDetailWaterTriangle (0x6EDDC0, cdecl, X1, Y1, P1, X2, Y2, P2, X3, Y3, P3)
  centro = (float(fild(X1+X2+X3) * 0.33333331f), float(fild(Y1+Y2+Y3) * 0.33333331f), P1.z); radio = float(fild(X2 - X1) * 0.71f); mismas dos llamadas a 420C40
  hw = (X2 - X1)/2 (truncando); sq = hw*hw; T = 1 + 2 + .. + (hw + 1) (0 si hw + 1 < 1); si 3*sq < 0x1000 y T < 0x800: 6E8780(X1, Y1, P1, X2, Y2, P2, X3, Y3, P3, capa, sq, T, hw) para capa 0 y 1
  si no: 6ECF00(X1 + (hw/2)*2, X1, Y1, P1, X2, Y2, P2, X3, Y3, P3)

CWaterLevel::TestLineAgainstWater (0x6E61B0, cdecl, CVector from, CVector to por valor, CVector* out), bool en AL
  zMin, yMin = (from < to) ? from : to; zMax, yMax, xMax = (from > to) ? from : to; xMin = (from < to) ? from : to (NaN -> to)
  bloques = _ftol2 (32 bits bajos del entero de 64 bits; NaN / inf / fuera de rango -> 0) de v * 0.002f + 6.0f; para ix = bloque(xMin)..bloque(xMax), iy = bloque(yMin)..bloque(yMax):
    row = ix * 12 (envoltura); dentro de la cuadricula si 0 <= row < 144 y 0 <= iy < 12: info = word[row + iy] (tipo = 2 bits altos): 1 = cuadrilatero unico, 3 = combinacion (secuencia hasta tipo 0), otros: nada
      cuadrilatero: si from.z * to.z < 0: cruce (escribe SIEMPRE out) y prueba x0 <= X <= x1, y0 <= Y <= y2 (vertices 0, 1, 2) -> true
    fuera: si zMin < 0 y zMax > 0: cruce; true si bloque(X) == ix y bloque(Y) == iy
  cruce: dx = to.x - from.x, dy, dz (float) ; t = float(|from.z| / (zMax - zMin)); X = float(float(dx*t) + from.x), Y = float(dy*t + from.y), Z = float(dz*t + from.z)
x87 a 24 bits con exponente extendido: cada operacion redondea el significando a 24 bits sin limitar el exponente; solo los fstp dword pasan a float. A lo sumo un NaN con carga propia por caso.
"""
from fractions import Fraction as Fr
import math

M32 = 0xFFFFFFFF
NAMES = ['CWaterLevel::FillQuadsAndTrianglesList', 'CWaterLevel::GetWaterDepth', 'CWaterLevel::RenderHighDetailWaterRectangle',
         'CWaterLevel::RenderHighDetailWaterTriangle', 'CWaterLevel::TestLineAgainstWater']
NAME_FILL, NAME_DEPTH, NAME_RECT, NAME_TRI, NAME_LINE = NAMES
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}


# ------------------------------------------------------------------------------------------------ generador de numeros (mismo Mix / Rand / FillWords que Fixtures.hpp)
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += rand(seed, i // 4).to_bytes(4, 'little')
    return bytes(out[:nbytes])


def s32(v):
    v &= M32
    return v - (1 << 32) if v >= 1 << 31 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v >= 0x8000 else v


def tdiv2(v):
    """division entera por 2 truncando hacia cero (cdq; sub eax,edx; sar eax,1)"""
    return v // 2 if v >= 0 else -((-v) // 2)


# ------------------------------------------------------------------------------------------------ x87 minimo (24 bits de mantisa, exponente extendido)
class X:
    """k: 'f' finito (s = signo 0/1, v = Fraction >= 0, el cero lleva signo), 'i' infinito (s), 'n' NaN silencioso (v = sus 32 bits)."""
    __slots__ = ('k', 's', 'v')

    def __init__(self, k, s=0, v=None):
        self.k, self.s, self.v = k, s, v


def FIN(s, mag):
    return X('f', s, Fr(mag))


def INF(s):
    return X('i', s)


def NAN(bits):
    return X('n', bits >> 31, bits)


DEFAULT_NAN = NAN(0xFFC00000)
ZERO = FIN(0, 0)


def fild(i):
    return FIN(1 if i < 0 else 0, abs(i))


def ld32(u):
    s = u >> 31
    e = (u >> 23) & 0xFF
    m = u & 0x7FFFFF
    if e == 255:
        if m == 0:
            return INF(s)
        return NAN(u | 0x400000)   # fld silencia un SNaN
    if e == 0:
        return FIN(s, Fr(m, 1 << 149))
    return FIN(s, Fr((m | 0x800000) * (2 ** e), 1 << 150) if e >= 0 else 0)


def value(x):
    return -x.v if x.s else x.v


def _exp2(mag):
    """floor(log2(mag)) de una fraccion positiva"""
    e = mag.numerator.bit_length() - mag.denominator.bit_length()
    if Fr(2) ** e > mag:
        e -= 1
    elif Fr(2) ** (e + 1) <= mag:
        e += 1
    return e


def round_bits(mag, bits, emin=None):
    """redondeo al mas cercano (empate al par) de una fraccion positiva a `bits` bits de significando; con emin, el exponente no baja de emin (denormales)"""
    if mag == 0:
        return Fr(0)
    e = _exp2(mag)
    if emin is not None and e < emin:
        e = emin
    ulp = Fr(2) ** (e - bits + 1)
    q = mag / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fr(1, 2) or (rem == Fr(1, 2) and n % 2 == 1):
        n += 1
    return n * ulp


def r24(x):
    if x.k != 'f':
        return x
    return FIN(x.s, round_bits(x.v, 24))


def nan2(a, b):
    if a.k == 'n' and b.k == 'n':
        sa, sb = a.v & 0x7FFFFF, b.v & 0x7FFFFF
        if sa == sb and a.v != b.v:
            raise RuntimeError('dos NaN con el mismo significando y distinto signo: fixture no soportada')
        return a if sa >= sb else b
    return a if a.k == 'n' else b


def add(a, b):
    if a.k == 'n' or b.k == 'n':
        return nan2(a, b)
    if a.k == 'i' or b.k == 'i':
        if a.k == 'i' and b.k == 'i':
            return a if a.s == b.s else DEFAULT_NAN
        return a if a.k == 'i' else b
    va, vb = value(a), value(b)
    total = va + vb
    if total == 0:
        if va == 0 and vb == 0 and a.s == 1 and b.s == 1:
            return FIN(1, 0)
        return FIN(0, 0)
    return r24(FIN(1 if total < 0 else 0, abs(total)))


def neg(x):
    if x.k == 'n':
        return x
    return X(x.k, 1 - x.s, x.v)


def sub(a, b):
    if a.k == 'n' or b.k == 'n':
        return nan2(a, b)
    return add(a, neg(b))


def mul(a, b):
    if a.k == 'n' or b.k == 'n':
        return nan2(a, b)
    s = a.s ^ b.s
    if a.k == 'i' or b.k == 'i':
        other = b if a.k == 'i' else a
        if other.k == 'f' and other.v == 0:
            return DEFAULT_NAN
        return INF(s)
    return r24(FIN(s, a.v * b.v))


def div(a, b):
    if a.k == 'n' or b.k == 'n':
        return nan2(a, b)
    s = a.s ^ b.s
    if a.k == 'i':
        return DEFAULT_NAN if b.k == 'i' else INF(s)
    if b.k == 'i':
        return FIN(s, 0)
    if b.v == 0:
        return DEFAULT_NAN if a.v == 0 else INF(s)
    return r24(FIN(s, a.v / b.v))


def fabs(x):
    if x.k == 'n':
        return NAN(x.v & 0x7FFFFFFF)
    return X(x.k, 0, x.v)


def sqrt(x):
    if x.k == 'n':
        return x
    if x.k == 'i':
        return x if x.s == 0 else DEFAULT_NAN
    if x.v == 0:
        return x
    if x.s == 1:
        return DEFAULT_NAN
    # sqrt exacta truncada a 200 bits (+ pegajoso): sin empates posibles en 24 bits
    p, q = x.v.numerator, x.v.denominator
    shift = 400
    scaled = (p << shift) // q
    root = math.isqrt(scaled)
    sticky = root * root != scaled or (p << shift) % q != 0
    mag = Fr(root, 1 << (shift // 2))
    if sticky:
        mag += Fr(1, 1 << (shift // 2 + 2))
    return r24(FIN(0, mag))


def cmp(a, b):
    """'lt', 'gt', 'eq' o 'un' (fcom / fcomp: un NaN da 'un')"""
    if a.k == 'n' or b.k == 'n':
        return 'un'
    ka = float('inf') * (1 if a.s == 0 else -1) if a.k == 'i' else None
    kb = float('inf') * (1 if b.s == 0 else -1) if b.k == 'i' else None
    if ka is not None or kb is not None:
        va = ka if ka is not None else 0.0
        vb = kb if kb is not None else 0.0
        if ka is not None and kb is not None:
            return 'eq' if ka == kb else ('lt' if ka < kb else 'gt')
        if ka is not None:
            return 'gt' if ka > 0 else 'lt'
        return 'lt' if kb > 0 else 'gt'
    va, vb = value(a), value(b)
    return 'lt' if va < vb else 'gt' if va > vb else 'eq'


def st32(x):
    """fstp dword: patron de 32 bits"""
    if x.k == 'n':
        return x.v
    if x.k == 'i':
        return (x.s << 31) | 0x7F800000
    sign = x.s << 31
    if x.v == 0:
        return sign
    mag = round_bits(x.v, 24, emin=-126)
    if mag >= Fr(2) ** 128:
        return sign | 0x7F800000
    if mag < Fr(2) ** -126:
        n = mag * (Fr(2) ** 149)
        assert n.denominator == 1
        return sign | int(n)
    e = _exp2(mag)
    frac = mag / (Fr(2) ** e) - 1
    m = frac * (1 << 23)
    assert m.denominator == 1
    return sign | ((e + 127) << 23) | int(m)


def f32(x):
    """x tras fstp dword + fld dword"""
    return ld32(st32(x))


def ftol_low(x):
    """32 bits bajos (con signo) del entero de 64 bits truncado por _ftol2; NaN / inf / fuera de rango -> indefinido 0x8000000000000000 -> 0"""
    if x.k != 'f':
        return 0
    t = int(x.v)   # trunca hacia cero (v >= 0)
    if t >= 1 << 63 or (x.s and t > 1 << 63):
        return 0
    if x.s:
        t = -t
    return s32(t)


C_HALF = ld32(0x3F000000)
C_THIRD = ld32(0x3EAAAAAA)
C_071 = ld32(0x3F35C28F)
C_0002 = ld32(0x3B03126F)   # 0.002f
C_ONE = ld32(0x3F800000)
C_SIX = ld32(0x40C00000)
NEG100 = 0xC2C80000


# ------------------------------------------------------------------------------------------------ 0: FillQuadsAndTrianglesList
def fnv(events):
    h = 0xCBF29CE484222325
    for ev in events:
        for v in ev:
            for k in range(4):
                h ^= (v >> (8 * k)) & 0xFF
                h = (h * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def expected_fill(inp):
    vx, vy = inp['vx'], inp['vy']
    vm1 = (inp['vm1x'], inp['vm1y'])
    quads, tris = inp['quads'], inp['tris']

    def vert(idx):
        i = s16(idx)
        if i == -1:
            return s16(vm1[0]), s16(vm1[1])
        if not 0 <= i < len(vx):
            raise RuntimeError('indice de vertice fuera del banco')
        return s16(vx[i]), s16(vy[i])

    nq, nt = inp['num_q'], inp['num_t']
    calls = 0
    events = []
    stats = dict(trigger_fired=False)

    def add_call(a, b, i, t):
        nonlocal calls, nq, nt
        events.append((a, b, i, t))
        calls += 1
        if inp['trig_kind'] and calls == inp['trig_at']:
            if inp['trig_kind'] == 1:
                nq = inp['trig_val']
            else:
                nt = inp['trig_val']
            stats['trigger_fired'] = True
    for a in range(12):
        x_lo = 500 * a - 3000
        x_hi = x_lo + 500
        for b in range(12):
            y_lo = 500 * b - 3000
            y_hi = y_lo + 500
            if s32(nq) > 0:
                i = 0
                while True:
                    q = quads[5 * i:5 * i + 5]
                    if vert(q[1])[0] > x_lo and vert(q[0])[0] < x_hi and vert(q[2])[1] > y_lo and vert(q[0])[1] < y_hi:
                        add_call(a, b, i, 1)
                    i += 1
                    if not i < s32(nq):
                        break
            count = s32(nt)
            for i in range(max(count, 0)):
                t = tris[4 * i:4 * i + 4]
                y0, y2 = vert(t[0])[1], vert(t[2])[1]
                ymin = y0 if y0 < y2 else y2
                ymax = y2 if y0 <= y2 else y0
                if vert(t[1])[0] > x_lo and vert(t[0])[0] < x_hi and ymax > y_lo and ymin < y_hi:
                    add_call(a, b, i, 2)
    want = dict(count=len(events), hash='%016x' % fnv(events), first=[list(e) for e in events[:24]], num_q_after=nq & M32, num_t_after=nt & M32, atlas_unchanged=True)
    stats.update(events_zero=len(events) == 0, events_some=len(events) > 0, events_many=len(events) > 24, quads_none=len(inp['quads']) == 0, tris_none=len(inp['tris']) == 0,
                 quads_many=len(inp['quads']) >= 25, num_q_neg=s32(inp['num_q']) < 0, num_q_zero=inp['num_q'] == 0, num_t_neg=s32(inp['num_t']) < 0, num_t_zero=inp['num_t'] == 0,
                 minus1_index=any(s16(v) == -1 for v in inp['quads'] + inp['tris']), kind_q=any(e[3] == 1 for e in events), kind_t=any(e[3] == 2 for e in events),
                 trig_q=inp['trig_kind'] == 1, trig_t=inp['trig_kind'] == 2)
    return want, stats


# ------------------------------------------------------------------------------------------------ 1: GetWaterDepth
def expected_depth(inp, case, observed):
    pos = inp['pos']
    events = [[1, pos[0], pos[1], pos[2], 1, 0, 0]]
    arena = bytearray(fill_words(0xC0, rand(case, 50)))
    obj = 0x40
    for i in range(3):
        arena[obj + 4 * i:obj + 4 * i + 4] = pos[i].to_bytes(4, 'little')
    stats = dict(ret_a_false=(inp['ret_a'] & 0xFF) == 0, ret_a_true=(inp['ret_a'] & 0xFF) != 0)
    want = dict(events=events, nevents=1, ret_al=0, obj=None, obj_ptr=observed['obj_ptr'] if observed is not None else 0, outside_unchanged=True)
    if (inp['ret_a'] & 0xFF) != 0:
        events.append([2, 1, 1, 0, 0x41F00000])
        want['nevents'] = 2
        obj_ptr = observed['obj_ptr'] if observed is not None else 0
        level_bits = inp['level'] if inp['write_level'] else obj_ptr
        found = (inp['ret_g'] & 0xFF) != 0
        ground_bits = inp['ground'] if found else NEG100
        level = ld32(level_bits)
        ground = ld32(ground_bits)
        depth = st32(sub(level, ground))
        outs = inp['out']
        if outs[0] != 0xFFFFFFFF:
            arena[obj + outs[0]:obj + outs[0] + 4] = depth.to_bytes(4, 'little')
        if outs[1] != 0xFFFFFFFF:
            arena[obj + outs[1]:obj + outs[1] + 4] = level_bits.to_bytes(4, 'little')
        if outs[2] != 0xFFFFFFFF:
            arena[obj + outs[2]:obj + outs[2] + 4] = st32(ground).to_bytes(4, 'little')
        want['ret_al'] = 1
        stats.update(found_ground=found, no_ground=not found, level_written=bool(inp['write_level']), level_unwritten=not inp['write_level'],
                     out_null=any(o == 0xFFFFFFFF for o in outs), out_alias=len({o for o in outs if o != 0xFFFFFFFF}) < len([o for o in outs if o != 0xFFFFFFFF]),
                     out_in_pos=any(o != 0xFFFFFFFF and o < 12 for o in outs), depth_nan=(depth & 0x7F800000) == 0x7F800000 and (depth & 0x7FFFFF) != 0,
                     depth_inf=(depth & 0x7FFFFFFF) == 0x7F800000, depth_zero=(depth & 0x7FFFFFFF) == 0, depth_den=(depth & 0x7FFFFFFF) != 0 and (depth & 0x7F800000) == 0,
                     level_nan=ld32(inp['level']).k == 'n' and bool(inp['write_level']), ground_nan=ld32(inp['ground']).k == 'n', ground_snan=(inp['ground'] & 0x7FC00000) == 0x7F800000 and (inp['ground'] & 0x7FFFFF) != 0)
    want['obj'] = [int.from_bytes(arena[obj + 4 * i:obj + 4 * i + 4], 'little') for i in range(0x10)]
    return want, stats


# ------------------------------------------------------------------------------------------------ 2 y 3: RenderHighDetailWaterRectangle / Triangle
def ren(p, i):
    return [p[4 * i], p[4 * i + 1], p[4 * i + 2], p[4 * i + 3] & 0xFFFF]


def lerp(a, b, t):
    """6E5280: (1 - t) * a + t * b por componente (24 bits), flowX = flowY = 0; a y b = [z, big, small, flow]"""
    omt = sub(C_ONE, t)
    out = []
    for k in range(3):
        out.append(st32(add(mul(ld32(a[k]), omt), mul(ld32(b[k]), t))))
    out.append(0)
    return out


CAMERA = 0xB6F028
MATRIX_A, MATRIX_B = 0xB6FA74, 0xB6FABC


def sphere_events(inp, center, radius):
    events = [[1, CAMERA, center[0], center[1], center[2], radius, MATRIX_A]]
    r1 = inp['ret1'] & 0xFF
    visible = r1 != 0
    if not visible:
        if inp['mirror'] & 0xFF:
            events.append([1, CAMERA, center[0], center[1], center[2], radius, MATRIX_B])
            visible = (inp['ret2'] & 0xFF) != 0
    return events, visible


def expected_rect(inp):
    min_x, max_x, y1, y2 = [s32(v) for v in inp['ints']]
    p = inp['p']
    cx = st32(mul(fild(s32(min_x + max_x)), C_HALF))
    cy = st32(mul(fild(s32(y1 + y2)), C_HALF))
    hx_f = mul(fild(s32(max_x - min_x)), C_HALF)
    hy_f = mul(fild(s32(y1 - y2)), C_HALF)
    radius = st32(sqrt(add(mul(hy_f, hy_f), mul(hx_f, hx_f))))
    events, visible = sphere_events(inp, [cx, cy, p[0]], radius)
    stats = dict(invisible=not visible, visible=visible, first_true=(inp['ret1'] & 0xFF) != 0, via_mirror=(inp['ret1'] & 0xFF) == 0 and (inp['mirror'] & 0xFF) != 0 and (inp['ret2'] & 0xFF) != 0,
                 mirror_noncanon=inp['mirror'] not in (0, 1), mirror_off_invisible=(inp['ret1'] & 0xFF) == 0 and (inp['mirror'] & 0xFF) == 0,
                 ret_noncanon=(inp['ret1'] & 0xFF) not in (0, 1) or (inp['ret2'] & 0xFF) not in (0, 1),
                 p_nan=any(ld32(v).k == 'n' for i in range(4) for v in ren(p, i)[:3]), p_inf=any(ld32(v).k == 'i' for i in range(4) for v in ren(p, i)[:3]),
                 p_den=any(0 < (v & 0x7FFFFFFF) < 0x800000 for i in range(4) for v in ren(p, i)[:3]))
    path = None
    if visible:
        min_y, max_y = (y1, y2) if y1 < y2 else (y2, y1)
        hx = tdiv2(s32(max_x - min_x))
        hy = tdiv2(s32(max_y - min_y))
        twice = s32(s32(hy * hx) * 2)
        tri = s32((hy + 1) * (hx + 1))
        if s32(twice * 3) < 0x1000 and tri < 0x800:
            path = 'small'
            events.append([2, inp['ints'][0], min_y & M32, inp['ints'][1], max_y & M32])
            for layer in (0, 1):
                ev = [3] + list(inp['ints']) + sum((ren(p, i) for i in range(4)), []) + [layer, twice & M32, tri & M32, hx & M32, hy & M32]
                events.append(ev)
        elif hx > hy:
            path = 'xsplit'
            split = s32(min_x + tdiv2(hx) * 2)
            events.append([4, split & M32] + list(inp['ints']) + sum((ren(p, i) for i in range(4)), []))
        else:
            path = 'ysplit'
            split = s32(min_y + tdiv2(hy) * 2)
            num, den = s32(split - y1), s32(y2 - y1)
            t = f32(div(fild(num), fild(den)))
            p13 = lerp(ren(p, 0), ren(p, 2), t)
            p24 = lerp(ren(p, 1), ren(p, 3), t)
            events.append([5, inp['ints'][0], inp['ints'][1], inp['ints'][2], split & M32] + ren(p, 0) + ren(p, 1) + p13 + p24)
            p13 = lerp(ren(p, 0), ren(p, 2), t)
            p24 = lerp(ren(p, 1), ren(p, 3), t)
            events.append([5, inp['ints'][0], inp['ints'][1], split & M32, inp['ints'][3]] + p13 + p24 + ren(p, 2) + ren(p, 3))
            stats.update(y_t_zero=(st32(t) & 0x7FFFFFFF) == 0, y_t_nonfinite=t.k != 'f', y_t_outside=t.k == 'f' and (t.v > 1 or t.s == 1 and t.v != 0))
        stats.update(small=path == 'small', xsplit=path == 'xsplit', ysplit=path == 'ysplit', hx_neg=hx < 0, hy_zero=hy == 0, hx_eq_hy=hx == hy, wrap=abs(max_x - min_x) >= 1 << 31 or abs(max_y - min_y) >= 1 << 31)
    return dict(events=events, nevents=len(events)), stats


def expected_tri(inp):
    x1, y1, x2, y2, x3, y3 = [s32(v) for v in inp['ints']]
    p = inp['p']
    cx = st32(mul(fild(s32(x1 + x2 + x3)), C_THIRD))
    cy = st32(mul(fild(s32(y1 + y2 + y3)), C_THIRD))
    radius = st32(mul(fild(s32(x2 - x1)), C_071))
    events, visible = sphere_events(inp, [cx, cy, p[0]], radius)
    stats = dict(invisible=not visible, visible=visible, first_true=(inp['ret1'] & 0xFF) != 0, via_mirror=(inp['ret1'] & 0xFF) == 0 and (inp['mirror'] & 0xFF) != 0 and (inp['ret2'] & 0xFF) != 0,
                 mirror_noncanon=inp['mirror'] not in (0, 1), ret_noncanon=(inp['ret1'] & 0xFF) not in (0, 1) or (inp['ret2'] & 0xFF) not in (0, 1),
                 p_nan=any(ld32(v).k == 'n' for i in range(3) for v in ren(p, i)[:3]), p_inf=any(ld32(v).k == 'i' for i in range(3) for v in ren(p, i)[:3]),
                 p_den=any(0 < (v & 0x7FFFFFFF) < 0x800000 for i in range(3) for v in ren(p, i)[:3]), radius_neg=(radius >> 31) == 1 and (radius & 0x7FFFFFFF) != 0)
    if visible:
        hw = tdiv2(s32(x2 - x1))
        sq = s32(hw * hw)
        n = s32(hw + 1)
        tri = s32(((n * (n + 1)) // 2) & M32) if n >= 1 else 0
        stats.update(hw_neg=hw < 0, hw_big=abs(hw) > 4096, tri_zero=tri == 0)
        if s32(sq * 3) < 0x1000 and tri < 0x800:
            for layer in (0, 1):
                events.append([6] + [inp['ints'][0], inp['ints'][1]] + ren(p, 0) + [inp['ints'][2], inp['ints'][3]] + ren(p, 1) + [inp['ints'][4], inp['ints'][5]] + ren(p, 2) + [layer, sq & M32, tri & M32, hw & M32])
            stats['small'] = True
        else:
            split = s32(x1 + tdiv2(hw) * 2)
            events.append([7, split & M32, inp['ints'][0], inp['ints'][1]] + ren(p, 0) + [inp['ints'][2], inp['ints'][3]] + ren(p, 1) + [inp['ints'][4], inp['ints'][5]] + ren(p, 2))
            stats['split'] = True
    return dict(events=events, nevents=len(events)), stats


# ------------------------------------------------------------------------------------------------ 4: TestLineAgainstWater
def block_of(x):
    return ftol_low(add(mul(x, C_0002), C_SIX))


def expected_line(inp):
    fr = [ld32(b) for b in inp['from']]
    to = [ld32(b) for b in inp['to']]
    z_min = fr[2] if cmp(fr[2], to[2]) == 'lt' else to[2]
    z_max = fr[2] if cmp(fr[2], to[2]) == 'gt' else to[2]
    y_min = fr[1] if cmp(fr[1], to[1]) == 'lt' else to[1]
    y_max = fr[1] if cmp(fr[1], to[1]) == 'gt' else to[1]
    x_min = fr[0] if cmp(fr[0], to[0]) == 'lt' else to[0]
    x_max = fr[0] if cmp(fr[0], to[0]) == 'gt' else to[0]
    b_min_y, b_max_x, b_max_y, b_min_x = block_of(y_min), block_of(x_max), block_of(y_max), block_of(x_min)
    vx, vy = inp['vx'], inp['vy']
    vm1 = (inp['vm1x'], inp['vm1y'])
    quads = inp['quads']
    blocks = dict(zip(inp['block_idx'], inp['block_val']))
    combos = inp['combos']

    def vert(idx):
        i = s16(idx)
        if i == -1:
            return s16(vm1[0]), s16(vm1[1])
        if not 0 <= i < len(vx):
            raise RuntimeError('indice de vertice fuera del banco')
        return s16(vx[i]), s16(vy[i])
    out = list(inp['out_init'])
    stats = dict(row_wrap=False, box_eq_x0=False, box_eq_x1=False, box_eq_y0=False, box_eq_y2=False, crossings=0, hit_single=False, hit_combo=False, hit_outside=False, single_quad_tested=False, single_tri_skipped=False, combo_tested=False, outside_tested=False,
                 no_blocks=b_min_x > b_max_x or b_min_y > b_max_y)

    def cross():
        dx = sub(to[0], fr[0])
        dy = sub(to[1], fr[1])
        dz = f32(sub(to[2], fr[2]))
        rng = sub(z_max, z_min)
        t = f32(div(fabs(fr[2]), rng))
        tdx = f32(mul(dx, t))
        ty = mul(dy, t)
        tz = mul(dz, t)
        out[0] = st32(add(tdx, fr[0]))
        out[1] = st32(add(ty, fr[1]))
        out[2] = st32(add(tz, fr[2]))
        stats['crossings'] += 1

    def in_quad(qid):
        q = quads[5 * qid:5 * qid + 5]
        v0, v1, v2 = vert(q[0]), vert(q[1]), vert(q[2])
        ox, oy = ld32(out[0]), ld32(out[1])
        c = [cmp(fild(v0[0]), ox), cmp(fild(v1[0]), ox), cmp(fild(v0[1]), oy), cmp(fild(v2[1]), oy)]
        for name, r in zip(('box_eq_x0', 'box_eq_x1', 'box_eq_y0', 'box_eq_y2'), c):
            if r == 'eq':
                stats[name] = True
        return c[0] in ('lt', 'eq') and c[1] in ('gt', 'eq') and c[2] in ('lt', 'eq') and c[3] in ('gt', 'eq')

    def product_neg():
        return cmp(mul(fr[2], to[2]), ZERO) == 'lt'
    result = False

    def search():
        for ix in range(b_min_x, b_max_x + 1):
            row = s32(ix * 12)
            for iy in range(b_min_y, b_max_y + 1):
                if row < 0 or row >= 144 or iy < 0 or iy >= 12:
                    if cmp(z_min, ZERO) != 'lt' or cmp(z_max, ZERO) != 'gt':
                        continue
                    stats['outside_tested'] = True
                    cross()
                    if block_of(ld32(out[0])) == ix and block_of(ld32(out[1])) == iy:
                        stats['hit_outside'] = True
                        return True
                    continue
                if not 0 <= ix < 12:
                    stats['row_wrap'] = True   # la fila envuelve a la cuadricula con un indice de bloque lejano
                info = blocks.get(row + iy, 0)
                typ = info >> 14
                if typ == 1:
                    if not product_neg():
                        continue
                    stats['single_quad_tested'] = True
                    cross()
                    if in_quad(info & 0x3FFF):
                        stats['hit_single'] = True
                        return True
                elif typ == 3:
                    start = info & 0x3FFF
                    if (combos[start] if start < len(combos) else 0) >> 14 == 0:
                        continue
                    k = start
                    while True:
                        w = combos[k] if k < len(combos) else 0
                        if w >> 14 == 1 and product_neg():
                            stats['combo_tested'] = True
                            cross()
                            if in_quad(w & 0x3FFF):
                                stats['hit_combo'] = True
                                return True
                        k += 1
                        if (combos[k] if k < len(combos) else 0) >> 14 == 0:
                            break
                elif typ == 2:
                    stats['single_tri_skipped'] = True
        return False
    result = search()
    want = dict(ret_al=1 if result else 0, out=out, outside_unchanged=True, atlas_unchanged=True)
    stats.update(result_true=result, result_false=not result, in_grid=any(0 <= s32(ix * 12) < 144 and 0 <= iy < 12 for ix in range(b_min_x, min(b_max_x, b_min_x + 40) + 1) for iy in range(b_min_y, min(b_max_y, b_min_y + 40) + 1)),
                 nan_input=any(v.k == 'n' for v in fr + to), inf_input=any(v.k == 'i' for v in fr + to), crossings_many=stats['crossings'] > 1, out_written=stats['crossings'] > 0,
                 out_nan=any(ld32(w).k == 'n' for w in out[:3]) and stats['crossings'] > 0, z_zero=(cmp(fr[2], ZERO) == 'eq') or (cmp(to[2], ZERO) == 'eq'),
                 range_zero=cmp(z_max, z_min) == 'eq', blocks_many=(b_max_x - b_min_x + 1) * (b_max_y - b_min_y + 1) > 4, minus1_vertex=any(s16(v) == -1 for v in quads[0::5]))
    return want, stats


def expected(fn, inp, case, observed=None):
    if fn == NAME_FILL:
        return expected_fill(inp)
    if fn == NAME_DEPTH:
        return expected_depth(inp, case, observed)
    if fn == NAME_RECT:
        return expected_rect(inp)
    if fn == NAME_TRI:
        return expected_tri(inp)
    if fn == NAME_LINE:
        return expected_line(inp)
    raise KeyError(fn)


# ------------------------------------------------------------------------------------------------ autoprueba
def selftest():
    # redondeos y conversiones basicos
    assert st32(ld32(0x3F800000)) == 0x3F800000
    assert st32(ld32(0x7F800001)) == 0x7FC00001           # fld/fstp silencia un SNaN
    assert st32(ld32(0x00000001)) == 0x00000001 and st32(ld32(0x80000000)) == 0x80000000
    assert st32(add(ld32(0x3F800000), ld32(0x3F800000))) == 0x40000000
    assert st32(mul(ld32(0x7F7FFFFF), ld32(0x40000000))) == 0x7F800000      # desborda al guardar como float
    assert st32(mul(ld32(0x7F7FFFFF), ld32(0x40000000))) == 0x7F800000 and mul(ld32(0x7F7FFFFF), ld32(0x40000000)).k == 'f'   # en registro sigue finito
    assert st32(sub(ld32(0x7F800000), ld32(0x7F800000))) == 0xFFC00000
    assert st32(div(fild(1), fild(3))) == 0x3EAAAAAB and st32(div(fild(2), fild(3))) == 0x3F2AAAAB
    assert st32(sqrt(fild(2))) == 0x3FB504F3 and st32(sqrt(fild(4))) == 0x40000000
    assert ftol_low(FIN(0, Fr(5, 2))) == 2 and ftol_low(FIN(1, Fr(5, 2))) == -2 and ftol_low(INF(0)) == 0 and ftol_low(DEFAULT_NAN) == 0
    assert ftol_low(FIN(0, 1 << 40)) == 0 and ftol_low(FIN(0, (1 << 40) + 5)) == 5
    assert block_of(ld32(0)) == 6 and block_of(ld32(0x45FA0000)) == 22 and block_of(ld32(0xC5FA0000)) == -10   # 0, 8000, -8000 (trunca hacia cero: -10.0000008 -> -10)
    assert block_of(ld32(0xC4FA0000)) == 2   # -2000 * 0.002f = -4.0 (24 bits) + 6 = 2.0
    assert round_bits(Fr(16777217), 24) == 16777216 and round_bits(Fr(16777219), 24) == 16777220
    assert tdiv2(-3) == -1 and tdiv2(3) == 1 and tdiv2(-2147483648) == -1073741824
    # modelo: un caso a mano de GetWaterDepth y de FillQuads
    want, _ = expected_depth(dict(pos=[0, 0, 0], ret_a=0, ret_g=0, write_level=1, level=0x41200000, ground=0, out=[0x10, 0x14, 0x18]), 0, None)
    assert want['ret_al'] == 0 and want['nevents'] == 1
    want, _ = expected_depth(dict(pos=[1, 2, 3], ret_a=1, ret_g=0, write_level=1, level=0x41200000, ground=0, out=[0x10, 0x14, 0x18]), 0, dict(obj_ptr=0x1000))
    assert want['obj'][4] == 0x42DC0000 and want['obj'][5] == 0x41200000 and want['obj'][6] == NEG100, want['obj'][4:7]   # 10 - (-100) = 110
    print('ok')


if __name__ == '__main__':
    selftest()
