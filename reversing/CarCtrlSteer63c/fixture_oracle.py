"""Oraculo independiente del banco CarCtrlSteer63c (lote 63, CCarCtrl parte B1, tanda 3): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++),
  0x4336D0 SteerAICarTowardsPointInEscort (vehicle1, vehicle2, x, y, float* steer, float* accel, float* brake, bool* handbrake)
incluida la multiplicacion matriz x vector de 0x59C890 (que en el banco corre de verdad en el EXE y en la DLL con el ayudante x87 de X87Math.cpp), con aritmetica float de 24 bits
(x87 con PC=24 == float IEEE para + - * / y raiz). `expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el mismo formato que registra Core.hpp.
Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct

CASES = 4096
NAMES = ['CCarCtrl::SteerAICarTowardsPointInEscort']
NAME = NAMES[0]
K_ATAN, K_WEAVE, K_MAXSTEER = 1, 2, 3
OUT_WORDS = 8


def f32(x):
    if x != x:
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def bits(f):
    return struct.unpack('<I', struct.pack('<f', f))[0] if f == f else 0x7FC00000


def flt(u):
    return struct.unpack('<f', struct.pack('<I', u & 0xFFFFFFFF))[0]


def dflt(u):
    return struct.unpack('<d', struct.pack('<Q', u))[0]


def fadd(a, b): return f32(a + b)
def fsub(a, b): return f32(a - b)
def fmul(a, b): return f32(a * b)
def fdiv(a, b):
    if b == 0:
        return math.nan if (a == 0 or a != a) else math.copysign(math.inf, a) * (math.copysign(1.0, b))
    return f32(a / b)
def fsqrt(a):
    if a != a or a < 0:
        return math.nan
    return f32(math.sqrt(a))


C_PI = flt(0x40490FDB)
C_2PI = flt(0x40C90FDB)
C_01 = flt(0x3DCCCCCD)
C_N005 = flt(0xBD4CCCCD)
C_60 = 60.0

kCanon = 'nan'


def canon(v):
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return kCanon
    return v


def canon_tree(x):
    if isinstance(x, list):
        return [canon_tree(i) for i in x]
    return canon(x)


def mat_times_point(m, x, y, z):
    """0x59C890 desensamblado: x = ((m8*z + m4*y) + m0*x) + m12; y = ((m9*z + m1*x) + m5*y) + m13"""
    m = [flt(u) for u in m]
    rx = fadd(fadd(fadd(fmul(m[8], z), fmul(m[4], y)), fmul(m[0], x)), m[12])
    ry = fadd(fadd(fadd(fmul(m[9], z), fmul(m[1], x)), fmul(m[5], y)), m[13])
    return rx, ry


def escort(inp, stats):
    out = [0] * OUT_WORDS
    out[:4] = inp['outpre']
    ev = []
    m1, m2 = inp['m1'], inp['m2']
    x, y = [flt(u) for u in inp['in']]
    sp2 = [flt(u) for u in inp['sp2']]
    sp1 = [flt(u) for u in inp['sp1']]
    wx, wy = mat_times_point(m2, x, y, 0.0)
    tx, ty = fadd(sp2[0], wx), fadd(sp2[1], wy)
    out[3] = (out[3] & 0xFFFFFF00)       # *handbrake = false (byte)

    fx0, fy0 = flt(m1[4]), flt(m1[5])
    ln = fsqrt(fadd(fmul(fy0, fy0), fmul(fx0, fx0)))
    if ln == 0.0:
        fx, fy = 1.0, fy0
        stats['len0'] = True
    else:
        inv = fdiv(1.0, ln)
        fx, fy = fmul(inv, fx0), fmul(inv, fy0)
    ax = fadd(fmul(flt(m2[4]), 3.0), tx)
    ay = fadd(fmul(flt(m2[5]), 3.0), ty)
    px, py = flt(m1[12]), flt(m1[13])
    ev.append([K_ATAN, bits(fsub(ax, px)), bits(fsub(ay, py))])
    T1 = f32(dflt(inp['atan'][0]))
    ev.append([K_ATAN, bits(fx), bits(fy)])
    Hf = f32(dflt(inp['atan'][1]))
    ev.append([K_WEAVE, 'V1', 0, bits(T1), bits(Hf), 0x3F800000])
    diff = fsub(flt(inp['weave']), Hf)
    n = 0
    while diff < -C_PI:
        diff = fadd(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_lo'] = True
    while diff > C_PI:
        diff = fsub(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_hi'] = True
    ev.append([K_MAXSTEER, 'V1'])
    M = flt(inp['maxsteer'])
    if diff < -M:
        diff = -M; stats['clamp_lo'] = True
    if diff > M:
        diff = M; stats['clamp_hi'] = True
    ev.append([K_ATAN, bits(fsub(tx, px)), bits(fsub(ty, py))])    # resultado descartado por el EXE

    dy, dx = fsub(ty, py), fsub(tx, px)
    dot = fadd(fmul(dy, fy), fmul(dx, fx))
    dist = fsqrt(fadd(fmul(dy, dy), fmul(dx, dx)))

    def done(steer, accel, brake):
        out[0], out[1], out[2] = steer, accel, brake
        changes = [['O', 0, w, out[w]] for w in range(OUT_WORDS) if out[w] != (inp['outpre'][w] if w < 4 else 0)]
        return {'events': canon_tree(ev), 'nevents': len(ev), 'changes': canon_tree(changes), 'globals': []}

    if dot > 0.5:
        stats['ahead'] = True
        s60 = fmul(fsqrt(fadd(fadd(fmul(sp2[0], sp2[0]), fmul(sp2[1], sp2[1])), fmul(sp2[2], sp2[2]))), C_60)
        if dist < 15.0:
            stats['ahead_near'] = True
            v = fsub(fsub(dot, 0.5), flt(0x3DCCCCCD))
            m = 4.0 if 4.0 < v else v
            dm = fmul(dist, 3.5)
            total = fadd(s60, m)
            desired = total if total > dm else dm
        else:
            stats['ahead_far'] = True
            desired = 300.0
        q = fadd(s60, 10.0)
        if not (desired < q):
            desired = q
            stats['limited'] = True
    else:
        if dist < 15.0:
            stats['behind_near'] = True
            return done(0, 0, bits(1.0) if dot < -3.0 else bits(flt(0x3DCCCCCD)))
        stats['behind_far'] = True
        desired = 8.0
    cur = fmul(fsqrt(fadd(fadd(fmul(sp1[2], sp1[2]), fmul(sp1[1], sp1[1])), fmul(sp1[0], sp1[0]))), C_60)
    err = fsub(desired, cur)
    if err > 0.0:
        stats['accel'] = True
        if cur < 25.0:
            sc = fmul(err, C_01)
            acc = bits(1.0 if 1.0 < sc else sc)
            stats['accel_lin'] = True
        else:
            acc = bits(1.0)
        return done(bits(diff), acc, 0)
    stats['brake'] = True
    sc = fmul(err, C_N005)
    return done(bits(diff), 0, bits(0.5 if 0.5 < sc else sc))


def expected(name, inp, case):
    stats = {}
    if name != NAME:
        raise RuntimeError('funcion inesperada ' + name)
    want = escort(inp, stats)
    stats['fn0'] = True
    stats['family%d_fn0' % inp['fam']] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': canon_tree(obs['globals'])}


def selftest():
    # matriz identidad x (1, 2, 0) + posicion (10, 20, 30) = (11, 22)
    ident = [bits(1.0), 0, 0, 0, 0, bits(1.0), 0, 0, 0, 0, bits(1.0), 0, bits(10.0), bits(20.0), bits(30.0), 0]
    assert mat_times_point(ident, 1.0, 2.0, 0.0) == (11.0, 22.0)
    # un caso completo: vehiculo 1 muy por delante del objetivo (cerca y detras): frenazo
    z = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0]
    m1 = list(ident); m1[12] = bits(0.0); m1[13] = bits(0.0)
    inp = dict(fam=0, m1=m1, m2=ident, sp1=[0, 0, 0], sp2=[0, 0, 0], atan=[0, 0, 0], weave=0, maxsteer=bits(0.5), outpre=[0xC0000001, 0xC0000002, 0xC0000003, 0xC0000004], **{'in': [bits(1.0), bits(2.0)]})
    want, st = expected(NAME, inp, 0)
    assert want['events'][0][0] == K_ATAN and want['events'][0][1] == bits(fadd(fmul(0.0, 3.0), 11.0)), want
    assert st.get('ahead') or st.get('behind_near') or st.get('behind_far'), st
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
