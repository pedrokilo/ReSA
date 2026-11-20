"""Generador de casos del banco EXT1A (D3DX9 matematico: EXE vs d3dx9_43.dll). Oraculo independiente en doble precision de Python.
Formato (little endian), registro fijo de 8*? bytes: u32 fn, u32 modo (bit0: out==primer operando, bit1: out==segundo operando, bits4-5: desalineo en palabras),
u32 count, u32 clase (0 normal, 1 borde), u32 in[64], f64 exp[64], f64 tol[64]  (tol<0: sin oraculo)."""
import struct, random, sys, math
seed = int(sys.argv[3]) if len(sys.argv) > 3 else 20261119
rng = random.Random(seed)
def f32(x): return struct.unpack('<f', struct.pack('<f', x))[0]
def bits(x): return struct.unpack('<I', struct.pack('<f', x))[0]
def fb(b): return struct.unpack('<f', struct.pack('<I', b))[0]
def rnd_val(kind):
    if kind == 0: return f32(rng.uniform(-10, 10))
    if kind == 1: return f32(rng.choice((-1, 1)) * 2.0 ** rng.uniform(-30, 30))
    if kind == 2: return float(rng.randint(-8, 8))
    if kind == 3: return rng.choice((0.0, -0.0, 1.0, -1.0, 0.5, f32(rng.uniform(-1, 1))))
    return f32(rng.gauss(0, 1))
def vec(n, kind): return [rnd_val(kind) for _ in range(n)]
def mat(kind):
    m = vec(16, kind)
    if rng.random() < 0.2:  # casi identidad / afin
        m = [1.0 if i % 5 == 0 else 0.0 for i in range(16)]
        for i in (12, 13, 14): m[i] = f32(rng.uniform(-5, 5))
        for i in (0, 1, 2, 4, 5, 6, 8, 9, 10): m[i] = f32(m[i] + rng.uniform(-1, 1))
    return m
def rec(fn, modo, count, clase, ins, exp, tol):
    ins = list(ins) + [0] * (64 - len(ins)); exp = list(exp) + [0.0] * (64 - len(exp)); tol = list(tol) + [-1.0] * (64 - len(tol))
    return struct.pack('<4I64I64d64d', fn, modo, count, clase, *ins, *exp, *tol)
UL = 2.0 ** -20
def gen(fn, clase):
    kind = rng.choice((0, 0, 1, 2, 3, 4, 4))
    modo = rng.choice((0, 0, 1, 2)) | (rng.choice((0, 0, 1, 2)) << 4)
    if clase == 1:
        kind = 5
    def edge(vals):
        if clase != 1: return vals
        out = list(vals)
        for _ in range(rng.randint(1, 3)):
            i = rng.randrange(len(out))
            out[i] = rng.choice((float('inf'), float('-inf'), float('nan'), fb(rng.randint(1, 0x7FFFFF)), -fb(rng.randint(1, 0x7FFFFF)), 3.0e38, -3.0e38))
        return out
    if fn == 1:  # D3DXVec3TransformCoord: (x,y,z,1)*M, dividido por w
        v = vec(3, kind if kind < 5 else 0); m = mat(kind if kind < 5 else 0)
        v = edge(v) if clase else v
        if clase == 0:
            w = v[0]*m[3]+v[1]*m[7]+v[2]*m[11]+m[15]
            if abs(w) < 1e-3 or abs(w) < 1e-6 * (abs(v[0]*m[3])+abs(v[1]*m[7])+abs(v[2]*m[11])+abs(m[15])): return gen(fn, clase)
        exp, tol = [], []
        if clase == 0:
            t = [v[0]*m[c]+v[1]*m[4+c]+v[2]*m[8+c]+m[12+c] for c in range(4)]
            s = [abs(v[0]*m[c])+abs(v[1]*m[4+c])+abs(v[2]*m[8+c])+abs(m[12+c]) for c in range(4)]
            w = t[3]
            for c in range(3):
                exp.append(t[c] / w)
                # error relativo: (s[c]/|w| + |t[c]|*s[3]/w^2) * eps
                tol.append((s[c]/abs(w) + abs(t[c])*s[3]/(w*w)) * 8 * UL)
        return rec(fn, modo, 3, clase, [bits(x) for x in v] + [0]*0 + [bits(x) for x in m], exp, tol)
    if fn == 2:  # TransformNormal: (x,y,z,0)*M
        v = vec(3, kind if kind < 5 else 0); m = mat(kind if kind < 5 else 0)
        v = edge(v) if clase else v
        exp, tol = [], []
        if clase == 0:
            for c in range(3):
                exp.append(v[0]*m[c]+v[1]*m[4+c]+v[2]*m[8+c]); tol.append((abs(v[0]*m[c])+abs(v[1]*m[4+c])+abs(v[2]*m[8+c])) * 4 * UL)
        return rec(fn, modo, 3, clase, [bits(x) for x in v] + [bits(x) for x in m], exp, tol)
    if fn in (3, 5):
        a = mat(kind if kind < 5 else 0); b = mat(kind if kind < 5 else 0)
        if clase: a = edge(a)
        exp, tol = [0.0]*16, [-1.0]*16
        if clase == 0:
            for i in range(4):
                for j in range(4):
                    e = sum(a[4*i+k]*b[4*k+j] for k in range(4)); s = sum(abs(a[4*i+k]*b[4*k+j]) for k in range(4))
                    idx = 4*i+j if fn == 3 else 4*j+i
                    exp[idx] = e; tol[idx] = s * 4 * UL
        return rec(fn, modo, 16, clase, [bits(x) for x in a] + [bits(x) for x in b], exp, tol)
    if fn == 4:
        a = mat(kind if kind < 5 else 0)
        if clase: a = edge(a)
        exp = [a[4*(i % 4) + i // 4] for i in range(16)]
        return rec(fn, modo, 16, clase, [bits(x) for x in a], exp, [0.0]*16 if clase == 0 else [-1.0]*16)
    if fn == 6:
        n = rng.choice((1, 2, 3, 4, 5, 7, 8, 9, 15, 16, 17, 31, 63, 64, rng.randint(1, 64)))
        hs = []
        for _ in range(n):
            r = rng.random()
            if clase == 1:
                h = rng.choice((0x7C00, 0xFC00, 0x7E00, 0x7C01, 0xFFFF, 0x0001, 0x03FF, 0x8001, 0x83FF, rng.randint(0, 0xFFFF)))
            elif r < 0.1: h = rng.choice((0, 0x8000))
            elif r < 0.25: h = rng.randint(1, 0x3FF) | rng.choice((0, 0x8000))   # subnormales de medio float
            else:
                e = rng.randint(1, 30); h = (rng.randint(0, 1) << 15) | (e << 10) | rng.randint(0, 0x3FF)
            hs.append(h)
        exp = []
        for h in hs:
            s = -1.0 if h & 0x8000 else 1.0; e = (h >> 10) & 31; m = h & 0x3FF
            if e == 31: exp.append(float('nan')); continue
            exp.append(s * (m / 1024.0 * 2.0 ** -14 if e == 0 else (1 + m / 1024.0) * 2.0 ** (e - 15)))
        words = [hs[i] | (hs[i+1] << 16 if i+1 < n else 0) for i in range(0, n, 2)]
        tol = [0.0 if (hs[i] & 0x7C00) != 0x7C00 else -1.0 for i in range(n)]
        return rec(fn, modo & 0x30, n, clase, words, exp, tol)
out = sys.argv[1]; per = int(sys.argv[2]); edge_n = max(per // 5, 2000)
with open(out, 'wb') as f:
    cnt = 0
    for fn in range(1, 7):
        for _ in range(per): f.write(gen(fn, 0)); cnt += 1
        for _ in range(edge_n): f.write(gen(fn, 1)); cnt += 1
print('casos', cnt, 'semilla', seed)
