"""Oraculo independiente del banco CarCtrlSteer63d (lote 63, CCarCtrl parte B1, tanda 4): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++),
  0x422B20 SteerAICarBlockingPlayerForwardAndBack (vehicle, float* steer, float* accel, float* brake, bool* handbrake)
incluida la normalizacion CVector::Normalise de 0x59C910 (que en el banco corre de verdad en el EXE y en la DLL), con aritmetica float de 24 bits (x87 con PC=24 == float IEEE para
+ - * / y raiz). `expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el mismo formato que registra Core.hpp.
Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct

CASES = 4096
NAMES = ['CCarCtrl::SteerAICarBlockingPlayerForwardAndBack']
NAME = NAMES[0]
K_SPEED, K_ENTITY, K_ALLOC, K_UPDATE, K_COORS = 1, 2, 3, 4, 5
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


C_01 = flt(0x3DCCCCCD)      # 0.1 (0x858B1C)
C_001 = flt(0x3C23D70A)     # 0.01 (0x858C58)
C_N01 = flt(0xBDCCCCCD)     # -0.1 (0x858EF4)
C_095 = flt(0x3F733333)     # 0.95 (0x858EF0)

kCanon = 'nan'


def canon(v):
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return kCanon
    return v


def canon_tree(x):
    if isinstance(x, list):
        return [canon_tree(i) for i in x]
    return canon(x)


def normalise(v):
    """0x59C910: suma (x*x + y*y) + z*z; 0 -> x = 1.0 (y y z intactos); si no, inv = 1 / sqrt(suma) y x*inv, y*inv, z*inv"""
    x, y, z = v
    s = fadd(fadd(fmul(x, x), fmul(y, y)), fmul(z, z))
    if s == 0.0:
        return [1.0, y, z]
    inv = fdiv(1.0, fsqrt(s))
    return [fmul(inv, x), fmul(inv, y), fmul(inv, z)]


def block(inp, stats):
    out = [0] * OUT_WORDS
    out[:4] = inp['outpre']
    ev = []
    changes_extra = []
    out[0] = 0                                     # *steer = 0
    out[3] = out[3] & 0xFFFFFF00                   # *handbrake = false (byte)
    ev.append([K_SPEED, 0xFFFFFFFF])
    ev.append([K_ENTITY, 0xFFFFFFFF])
    ps = [flt(u) for u in inp['ps']]
    if inp['pmat']:
        pm = inp['mp']
    else:
        stats['alloc'] = True
        ev.append([K_ALLOC, 'PE'])
        ev.append([K_UPDATE, 'PE+4', 'PM'])
        pm = inp['mpalloc']
        changes_extra.append(['PE', 0, 5, 'PM'])
        for w in range(16):
            if pm[w] != 0:
                changes_extra.append(['PM', 0, w, pm[w]])
    ptx, pty = flt(pm[4]), flt(pm[5])
    psx = fadd(fmul(ptx, C_01), ps[0])
    psy = fadd(ps[1], fmul(pty, C_01))
    m1 = [flt(u) for u in inp['m1']]
    R = normalise([m1[0], m1[1], 0.0])
    T = normalise([m1[4], m1[5], 0.0])
    px, py, pz = m1[12], m1[13], m1[14]
    ev.append([K_COORS, 'L', 0xFFFFFFFF])
    pc = [flt(u) for u in inp['pc0']]
    dx, dy, dz = fsub(pc[0], px), fsub(pc[1], py), fsub(pc[2], pz)
    A = fadd(fadd(fmul(R[1], dy), fmul(dx, R[0])), fmul(dz, R[2]))
    C = fadd(fadd(fmul(R[1], psy), fmul(R[0], psx)), fmul(R[2], 0.0))
    if C == 0.0:
        C = C_001
        stats['c_zero'] = True
    t = -fdiv(A, C)

    def done():
        out[3] = out[3]
        ch = list(changes_extra)
        for w in range(OUT_WORDS):
            if out[w] != (inp['outpre'][w] if w < 4 else 0):
                ch.append(['O', 0, w, out[w]])
        # orden de Core.hpp: regiones V, M, PE, PM, PS, O
        order = {'V': 0, 'M': 1, 'PE': 2, 'PM': 3, 'PS': 4, 'O': 5}
        ch.sort(key=lambda c: (order[c[0]], c[2]))
        return {'events': canon_tree(ev), 'nevents': len(ev), 'changes': canon_tree(ch), 'globals': []}

    if t < 0.0:
        stats['t_neg'] = True
        out[1] = 0; out[2] = 0
        return done()
    ev.append([K_COORS, 'L', 0xFFFFFFFF])
    pc2 = [flt(u) for u in inp['pc1']]
    dx2, dy2, dz2 = fsub(pc2[0], px), fsub(pc2[1], py), fsub(pc2[2], pz)
    v1 = [flt(u) for u in inp['sp1']]
    V = fadd(fadd(fmul(T[1], v1[1]), fmul(T[2], v1[2])), fmul(T[0], v1[0]))
    W = fadd(fadd(fmul(T[1], psy), fmul(T[0], psx)), fmul(T[2], 0.0))
    Wt = fmul(W, t)
    E2 = fadd(fadd(fmul(T[1], dy2), fmul(dx2, T[0])), fmul(dz2, T[2]))
    F = fadd(Wt, E2)
    G = fsub(F, fmul(V, t))
    if G > 0.0:
        stats['g_pos'] = True
        x = fmul(G, C_01)
        out[1] = bits(1.0 if 1.0 < x else x)
        out[2] = 0
        return done()
    if V > 0.0:
        stats['v_pos'] = True
        x = fmul(G, C_N01)
        out[1] = 0
        v = 1.0 if 1.0 < x else x
        out[2] = bits(v)
        if v > C_095:
            stats['handbrake'] = True
            out[3] = (out[3] & 0xFFFFFF00) | 1
        return done()
    stats['v_neg'] = True
    x = fmul(G, C_01)
    out[1] = bits(-1.0 if -1.0 > x else x)
    out[2] = 0
    return done()


def expected(name, inp, case):
    stats = {}
    if name != NAME:
        raise RuntimeError('funcion inesperada ' + name)
    want = block(inp, stats)
    stats['fn0'] = True
    stats['family%d_fn0' % inp['fam']] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': canon_tree(obs['globals'])}


def selftest():
    assert normalise([3.0, 4.0, 0.0]) == [0.6000000238418579, 0.800000011920929, 0.0]
    assert normalise([0.0, 5.0, 0.0])[0] == 0.0 and normalise([0.0, 0.0, 0.0]) == [1.0, 0.0, 0.0]
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
