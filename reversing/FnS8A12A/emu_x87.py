"""Comprobacion SIN juego de las funciones x87 de los lotes S9A y S11A contra los BYTES DEL EXE.

Emula (Unicorn) cada funcion original con la palabra de control x87 a 24 bits (0x007F) y compara contra un modelo en Python que repite, operacion a operacion, las formulas de X87Math.cpp
(cada operacion se redondea a float32, que equivale a la x87 con precision de 24 bits fuera de los rangos denormal/desbordamiento; esos casos no se generan). Valida la LECTURA del desensamblado
y las formulas; no prueba el codigo C++ compilado (eso solo lo hace un banco en el juego).

    python -I emu_x87.py [casos_por_funcion=3000] [--exe <gta_sa.exe>]
"""
import math
import random
import struct
import sys
from pathlib import Path

from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_PROT_ALL
from unicorn.x86_const import UC_X86_REG_ESP, UC_X86_REG_EIP, UC_X86_REG_FPCW

EXE = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe')
STACK_LO, STACK_HI = 0x7E000000, 0x7E100000
SENTINEL = 0x7FFF0000
ARENA = 0x10000000


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fbits(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def load_image(uc, path):
    d = Path(path).read_bytes()
    pe = struct.unpack_from('<I', d, 0x3C)[0]
    nsec = struct.unpack_from('<H', d, pe + 6)[0]
    opt = struct.unpack_from('<H', d, pe + 20)[0]
    base = struct.unpack_from('<I', d, pe + 24 + 28)[0]
    size = struct.unpack_from('<I', d, pe + 24 + 56)[0]
    uc.mem_map(base, (size + 0xFFF) & ~0xFFF, UC_PROT_ALL)
    so = pe + 24 + opt
    for i in range(nsec):
        n, vs, va, rs, rp = struct.unpack_from('<8sIIII', d, so + i * 40)
        uc.mem_write(base + va, d[rp:rp + min(rs, vs)])


class Emu:
    def __init__(self, exe):
        self.uc = Uc(UC_ARCH_X86, UC_MODE_32)
        load_image(self.uc, exe)
        self.uc.mem_map(ARENA, 0x10000, UC_PROT_ALL)
        self.uc.mem_map(STACK_LO, STACK_HI - STACK_LO, UC_PROT_ALL)
        self.uc.mem_map(SENTINEL, 0x1000, UC_PROT_ALL)
        self.uc.mem_write(SENTINEL, b'\x90' * 16)
        self.uc.hook_add(UC_HOOK_CODE, lambda uc, a, s, u: uc.emu_stop(), begin=SENTINEL, end=SENTINEL)
        # _rwSqrt (0x7EDB30) es una raiz por tabla que necesita el estado del motor: se sustituye por `fld dword [esp+4]; fsqrt; ret`.
        self.uc.mem_write(0x7EDB30, bytes([0xD9, 0x44, 0x24, 0x04, 0xD9, 0xFA, 0xC3]))

    def call(self, addr, args, regions):
        """args: lista de enteros de 32 bits (cdecl). regions: {direccion: bytes}. Devuelve {direccion: bytes tras la llamada}."""
        uc = self.uc
        for a, b in regions.items():
            uc.mem_write(a, b)
        sp = STACK_HI - 0x1000
        for v in reversed(args):
            sp -= 4
            uc.mem_write(sp, struct.pack('<I', v & 0xFFFFFFFF))
        sp -= 4
        uc.mem_write(sp, struct.pack('<I', SENTINEL))
        uc.reg_write(UC_X86_REG_ESP, sp)
        uc.reg_write(UC_X86_REG_FPCW, 0x007F)
        uc.emu_start(addr, SENTINEL, count=100000)
        return {a: bytes(uc.mem_read(a, len(b))) for a, b in regions.items()}


def nan_eq(a, b):
    """Compara patrones de bits de float; todos los NaN se consideran iguales."""
    fa, fb = fbits(a), fbits(b)
    if math.isnan(fa) and math.isnan(fb):
        return True
    return a == b


# ----------------------------------------------------------------------------- modelos (repiten X87Math.cpp)
def wrap_angle(d):
    if d < f32(-3.14159274101257324):
        return f32(d + f32(6.28318548202514648))
    if d > f32(3.14159274101257324):
        return f32(d - f32(6.28318548202514648))
    return d


def lerp_tail(out, in1, in2, alpha, wrap=False):
    for i in range(2, 8):
        d = f32(in2[i] - in1[i])
        if wrap and i == 2:
            d = wrap_angle(d)
        out[i] = f32(f32(d * alpha) + in1[i])


def m_linear_blend(out, in1, in2, alpha):
    lerp_tail(out, in1, in2, alpha)


def m_linear_interp(out, in1, in2, time):
    alpha = f32(f32(time - in1[1]) / f32(in2[1] - in1[1]))
    lerp_tail(out, in1, in2, alpha)


def m_param_blend(out, in1, in2, alpha):
    lerp_tail(out, in1, in2, alpha, True)


def m_param_interp(out, in1, in2, time):
    alpha = f32(f32(time - in1[1]) / f32(in2[1] - in1[1]))
    lerp_tail(out, in1, in2, alpha, True)


def m_linear_add(out, in1, in2):
    out[2] = f32(f32(in2[2] * in1[2]) + f32(in2[4] * in1[3]))
    out[3] = f32(f32(in2[3] * in1[2]) + f32(in2[5] * in1[3]))
    out[4] = f32(f32(in2[4] * in1[5]) + f32(in2[2] * in1[4]))
    out[5] = f32(f32(in2[5] * in1[5]) + f32(in2[3] * in1[4]))
    out[6] = f32(in2[6] + in1[6])
    out[7] = f32(in2[7] + in1[7])


def m_linear_mulrecip(f, s):
    det = f32(f32(s[2] * s[5]) - f32(s[3] * s[4]))
    if det != 0.0:
        inv = f32(1.0 / det) if det != 0 else math.inf
        b, d = f[3], f[5]
        a, c = f[2], f[4]
        idd = f32(inv * s[5])
        nib = -f32(inv * s[3])
        nic = f32(-f32(inv * s[4]))
        f[2] = f32(f32(nib * c) + f32(idd * a))
        f[3] = f32(f32(nib * d) + f32(idd * b))
        f[4] = f32(f32(c + a) * nic)
        f[5] = f32(f32(d + b) * nic)
    f[6] = f32(f[6] - s[6])
    f[7] = f32(f[7] - s[7])


def m_hanim_apply(f):
    x, y, z, w = f[2], f[3], f[4], f[5]
    xx, yy, zz = f32(x * x), f32(y * y), f32(z * z)
    yz, zx, xy, wx = f32(z * y), f32(z * x), f32(y * x), f32(w * x)
    wy, wz = f32(w * y), f32(w * z)
    m = [0.0] * 16
    m[0] = f32(1.0 - f32(2 * f32(zz + yy)))
    m[1] = f32(2 * f32(xy + wz))
    m[2] = f32(2 * f32(zx - wy))
    m[4] = f32(2 * f32(xy - wz))
    m[5] = f32(1.0 - f32(2 * f32(zz + xx)))
    m[6] = f32(2 * f32(wx + yz))
    m[8] = f32(2 * f32(wy + zx))
    m[9] = f32(2 * f32(yz - wx))
    m[10] = f32(1.0 - f32(2 * f32(yy + xx)))
    m[12], m[13], m[14] = f[6], f[7], f[8]
    return m


def m_hanim_add(out, in1, in2):
    out[5] = f32(f32(in2[5] * in1[5]) - f32(f32(f32(in2[3] * in1[3]) + f32(in2[2] * in1[2])) + f32(in2[4] * in1[4])))
    out[2] = f32(f32(in1[3] * in2[4]) - f32(in2[3] * in1[4]))
    out[3] = f32(f32(in2[2] * in1[4]) - f32(in1[2] * in2[4]))
    out[4] = f32(f32(in2[3] * in1[2]) - f32(in2[2] * in1[3]))
    for i, (p, q) in enumerate(((2, 2), (3, 3), (4, 4))):
        out[p] = f32(f32(in1[5] * in2[q]) + out[p])
    for p in (2, 3, 4):
        out[p] = f32(f32(in2[5] * in1[p]) + out[p])
    out[6] = f32(in1[6] + in2[6])
    out[7] = f32(in1[7] + in2[7])
    out[8] = f32(in1[8] + in2[8])


def m_hanim_mulrecip(f, s):
    sx, sy, sz, sw = s[2], s[3], s[4], s[5]
    norm = f32(f32(f32(f32(sx * sx) + f32(sy * sy)) + f32(sz * sz)) + f32(sw * sw))
    t1 = nx = ny = 0.0
    nzr = 0.0
    if norm > 0.0:
        inv = f32(1.0 / norm)
        t1 = f32(inv * sw)
        mi = -inv
        nx = f32(sx * mi)
        ny = f32(sy * mi)
        nzr = f32(mi * sz)
    fx, fy, fz, fw = f[2], f[3], f[4], f[5]
    f[5] = f32(f32(fw * t1) - f32(f32(f32(fy * ny) + f32(fx * nx)) + f32(fz * nzr)))
    f[2] = f32(f32(f32(f32(fz * ny) - f32(fy * nzr)) + f32(fw * nx)) + f32(fx * t1))
    f[3] = f32(f32(f32(f32(fx * nzr) - f32(fz * nx)) + f32(fw * ny)) + f32(fy * t1))
    f[4] = f32(f32(f32(f32(fy * nx) - f32(fx * ny)) + f32(fw * nzr)) + f32(fz * t1))
    f[6] = f32(f[6] - s[6])
    f[7] = f32(f[7] - s[7])
    f[8] = f32(f[8] - s[8])


K = lambda b: fbits(b)


def sin_kernel(x):
    z = f32(x * x)
    v = f32(K(0x2f2ec9d3) * z)
    v = f32(v - K(0x32d72f34)); v = f32(v * z)
    v = f32(v + K(0x3638ef1b)); v = f32(v * z)
    v = f32(v - K(0x39500d01)); v = f32(v * z)
    v = f32(v + K(0x3c088889)); v = f32(v * z)
    v = f32(v - K(0x3e2aaaab))
    return f32(f32(v * f32(z * x)) + x)


def acos_p(z):
    p = f32(K(0x3811ef08) * z)
    for c, sub in ((0x3a4f7f04, 0), (0x3d241146, 1), (0x3e4e0aa8, 0), (0x3ea6b090, 1), (0x3e2aaaab, 0)):
        p = f32(p + K(c)) if not sub else f32(p - K(c))
        p = f32(p * z)
    return p


def acos_q(z):
    q = f32(z * K(0x3d9dc62e))
    q = f32(q - K(0x3f303361)); q = f32(q * z)
    q = f32(q + K(0x4001572d)); q = f32(q * z)
    q = f32(q - K(0x4019d139)); q = f32(q * z)
    return f32(q + 1.0)


def acos_model(x):
    xb = bits(x)
    ab = xb & 0x7fffffff
    sb = xb - (1 << 32) if xb & 0x80000000 else xb
    if ab >= 0x3f800000:
        return 0.0 if sb > 0 else K(0x40490fdb)
    if ab >= 0x3f000000:
        if sb < 0:
            zf = f32(f32(1.0 + x) * 0.5)
            s = f32(math.sqrt(zf))
            r = f32(acos_p(zf) / acos_q(zf))
            t = f32(s + f32(f32(r * s) - K(0x33a22168)))
            return f32(K(0x40490fda) - f32(t + t))
        zf = f32(f32(1.0 - x) * 0.5)
        s = f32(math.sqrt(zf))
        sb2 = bits(f32(s)) & 0xfffff000
        df = fbits(sb2)
        r = f32(acos_p(zf) / acos_q(zf))
        rs = f32(r * s)
        dd = f32(df * df)
        c = f32(f32(zf - dd) / f32(df + s))
        w = f32(rs + c)
        t = f32(w + df)
        return f32(t + t)
    if ab <= 0x23000000:
        return K(0x3fc90fdb)
    z = f32(x * x)
    r = f32(acos_p(z) / acos_q(z))
    rx = f32(r * x)
    inner = f32(K(0x33a22168) - rx)
    sub = f32(x - inner)
    return f32(K(0x3fc90fda) - sub)


def slerp_body(out, in1, in2, alpha):
    dot = f32(in2[2] * in1[2])
    dot = f32(dot + f32(in1[3] * in2[3]))
    dot = f32(dot + f32(in2[4] * in1[4]))
    dot = f32(dot + f32(in1[5] * in2[5]))
    for i in (6, 7, 8):
        out[i] = f32(f32(f32(in2[i] - in1[i]) * alpha) + in1[i])
    if dot < 0.0:
        dot = -dot
        for i in (2, 3, 4, 5):
            in2[i] = -in2[i]
    w1 = f32(1.0 - alpha)
    w2 = alpha
    if not (dot >= K(0x3f7fbe77)):
        omega = acos_model(dot)
        recip = f32(1.0 / sin_kernel(omega))
        n1 = f32(sin_kernel(f32(omega * w1)) * recip)
        n2 = f32(sin_kernel(f32(omega * alpha)) * recip)
        w1, w2 = n1, n2
    out[2] = f32(f32(w2 * in2[2]) + f32(w1 * in1[2]))
    out[3] = f32(f32(w1 * in1[3]) + f32(w2 * in2[3]))
    out[4] = f32(f32(w2 * in2[4]) + f32(w1 * in1[4]))
    out[5] = f32(f32(w1 * in1[5]) + f32(w2 * in2[5]))


def m_slerp_interp(out, in1, in2, time):
    alpha = f32(f32(time - in1[1]) / f32(in2[1] - in1[1]))
    slerp_body(out, in1, in2, alpha)


def m_slerp_blend(out, in1, in2, alpha):
    slerp_body(out, in1, in2, alpha)


# ----------------------------------------------------------------------------- generacion y comparacion
rng = random.Random(0x58373A)


def rnd(scale=4.0):
    k = rng.random()
    if k < 0.04:
        return 0.0
    if k < 0.06:
        return -0.0
    if k < 0.08:
        return 1.0
    if k < 0.10:
        return -1.0
    return f32(rng.uniform(-scale, scale))


def frame(prev=0, quat=False):
    f = [fbits(prev), 0.0] + [rnd() for _ in range(7 if quat else 6)]
    f[1] = f32(rng.uniform(0.0, 3.0))
    if quat:
        q = [rng.gauss(0, 1) for _ in range(4)]
        n = math.sqrt(sum(v * v for v in q)) or 1.0
        scale = rng.choice([1.0, 1.0, 1.0, 0.5, 2.0])
        f[2:6] = [f32(v / n * scale) for v in q]
        if rng.random() < 0.3:  # cuaterniones casi iguales (producto cercano a 1)
            return f, True
    return f, False


def pack(f):
    return b''.join(struct.pack('<I', v) if isinstance(v, int) else struct.pack('<f', v) for v in f)


def unpack(b, n=9):
    n = min(n, len(b) // 4)
    return list(struct.unpack('<%dI' % n, b[:4 * n]))


def run_case(emu, name, addr, model, kind, args_f=(), nin=2, quat=False, trials=3000):
    bad = 0
    for _ in range(trials):
        A, B = ARENA, ARENA + 0x100
        O = ARENA + 0x200
        in1, _n1 = frame(0, quat)
        in2, _n2 = frame(0, quat)
        if quat and rng.random() < 0.3:
            near = [v + rng.uniform(-0.02, 0.02) for v in in1[2:6]]
            n = math.sqrt(sum(v * v for v in near)) or 1.0
            in2[2:6] = [f32(v / n) for v in near]
            in1[2:6] = [f32(v) for v in in1[2:6]]
        if quat and rng.random() < 0.15:
            in2[2:6] = [f32(-v) for v in in1[2:6]]
        in1[1] = f32(rng.uniform(0.0, 1.0))
        in2[1] = f32(in1[1] + rng.uniform(0.05, 2.0))
        out0 = [0] * len(in1)
        scal = [f32(rng.uniform(-0.5, 2.5))] if args_f else []
        if kind == 'inout2':   # funcion (frame, start): modifica el primero
            regions = {A: pack(in1), B: pack(in2)}
            res = emu.call(addr, [A, B], regions)
            exp = [fbits(v) if isinstance(v, int) else v for v in in1]
            exp = [v for v in exp]
            model(exp, [fbits(v) if isinstance(v, int) else v for v in in2])
            got = unpack(res[A], len(in1))
            ok = all(nan_eq(bits(e) if not isinstance(e, int) else e, g) for e, g in zip(exp[1:], got[1:]))
        else:                  # (out, in1, in2, escalar)
            regions = {A: pack(in1), B: pack(in2), O: pack(out0)}
            res = emu.call(addr, [O, A, B] + [bits(s) for s in scal], regions)
            e1 = [fbits(v) if isinstance(v, int) else v for v in in1]
            e2 = [fbits(v) if isinstance(v, int) else v for v in in2]
            eo = [fbits(v) if isinstance(v, int) else v for v in out0]
            if scal:
                model(eo, e1, e2, scal[0])
            else:
                model(eo, e1, e2)
            gout, g2 = unpack(res[O], len(in1)), unpack(res[B], len(in1))
            ok = all(nan_eq(bits(e) if not isinstance(e, int) else e, g) for e, g in zip(eo[2:], gout[2:]))
            ok = ok and all(nan_eq(bits(e) if not isinstance(e, int) else e, g) for e, g in zip(e2[2:], g2[2:]))
        if not ok:
            bad += 1
            if bad <= 3:
                print('  DIFERENCIA', name, 'caso', _, [round(v, 5) for v in (scal or [])])
    print('%-34s 0x%06X  %d casos, %d diferencias' % (name, addr, trials, bad))
    return bad


def run_apply(emu, trials):
    bad = 0
    for _ in range(trials):
        f, _x = frame(0, True)
        M = ARENA + 0x400
        F = ARENA
        res = emu.call(0x7C5B80, [M, F], {F: pack(f), M: b'\xAA' * 64})
        got = unpack(res[M], 16)
        exp = m_hanim_apply([fbits(v) if isinstance(v, int) else v for v in f])
        ok = got[3] == 3
        for i in (0, 1, 2, 4, 5, 6, 8, 9, 10, 12, 13, 14):
            ok = ok and nan_eq(bits(exp[i]), got[i])
        if not ok:
            bad += 1
            if bad <= 3:
                print('  DIFERENCIA apply', _)
    print('%-34s 0x%06X  %d casos, %d diferencias' % ('HAnimKeyFrameApply', 0x7C5B80, trials, bad))
    return bad


def main():
    n = 3000
    exe = EXE
    args = sys.argv[1:]
    for i, a in enumerate(args):
        if a == '--exe':
            exe = Path(args[i + 1])
        elif a.isdigit():
            n = int(a)
    emu = Emu(exe)
    total = 0
    total += run_case(emu, 'UVAnimLinearKeyFrameBlend', 0x7CCAC0, m_linear_blend, 'out', (1,), trials=n)
    total += run_case(emu, 'UVAnimLinearKeyFrameInterpolate', 0x7CCA40, m_linear_interp, 'out', (1,), trials=n)
    total += run_case(emu, 'UVAnimParamKeyFrameBlend', 0x7CC6A0, m_param_blend, 'out', (1,), trials=n)
    total += run_case(emu, 'UVAnimParamKeyFrameInterpolate', 0x7CC600, m_param_interp, 'out', (1,), trials=n)
    total += run_case(emu, 'UVAnimLinearKeyFrameAdd', 0x7CCBE0, m_linear_add, 'out', (), trials=n)
    total += run_case(emu, 'UVAnimLinearKeyFrameMulRecip', 0x7CCB30, m_linear_mulrecip, 'inout2', (), trials=n)
    total += run_apply(emu, n)
    total += run_case(emu, 'HAnimKeyFrameAdd', 0x7C6720, m_hanim_add, 'out', (), quat=True, trials=n)
    total += run_case(emu, 'HAnimKeyFrameMulRecip', 0x7C65C0, m_hanim_mulrecip, 'inout2', (), quat=True, trials=n)
    total += run_case(emu, 'HAnimKeyFrameInterpolate (slerp)', 0x7C5CA0, m_slerp_interp, 'out', (1,), quat=True, trials=n)
    total += run_case(emu, 'HAnimKeyFrameBlend (slerp)', 0x7C60C0, m_slerp_blend, 'out', (1,), quat=True, trials=n)
    print('TOTAL diferencias:', total)
    sys.exit(1 if total else 0)


if __name__ == '__main__':
    main()
