"""Oraculo independiente del banco CarCtrlSteer63 (lote 63, CCarCtrl parte B1): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++), las tres rutas
  0x428990 SteerAICarWithPhysicsTryingToBlockTarget_Stop      (vehicle, x, y, a4, a5, o6, o7, o8, o9)
  0x433280 SteerAICarWithPhysicsHeadingForTarget              (vehicle, target, x, y, steer, accel, brake, handbrake)
  0x4335E0 SteerAICarWithPhysicsTryingToBlockTarget           (vehicle, unused, a3, a4, a5, a6, steer, accel, brake, handbrake)
con aritmetica float de 24 bits (x87 con PC=24 == float IEEE para + - * / y raiz; el fpatan de 0x53CC70 queda SIN redondear en ST0: la resta final de HeadingForTarget se hace exacta y
se redondea una sola vez). `expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el mismo formato que registra Core.hpp.
Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct
from fractions import Fraction

CASES = 3 * 4096
PER_FN = 4096
NAMES = ['CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget_Stop', 'CCarCtrl::SteerAICarWithPhysicsHeadingForTarget', 'CCarCtrl::SteerAICarWithPhysicsTryingToBlockTarget']
NAME = NAMES[0]

# tipos de evento (Fixtures.hpp)
K_FPV, K_GETACTIVE, K_UPSIDE, K_TELL, K_TASKTYPE, K_ATAN, K_WEAVE, K_MAXSTEER, K_SPEEDMUL, K_HEADING = 1, 2, 3, 4, 5, 6, 7, 8, 9, 10
VEH_SIZE = 0x900
OUT_WORDS = 8


def f32(x):
    if isinstance(x, Fraction):
        return f32_exact(x)
    if x != x:
        return x
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.copysign(math.inf, x)


def f32_exact(q):
    """Fraction -> float de 24 bits con un solo redondeo (al par)."""
    if q == 0:
        return 0.0
    sign = -1 if q < 0 else 1
    q = abs(q)
    e = q.numerator.bit_length() - q.denominator.bit_length()
    # q en [2^(e-1), 2^(e+1)); ajusta e para que 2^e <= q < 2^(e+1)
    while Fraction(2) ** e > q:
        e -= 1
    while Fraction(2) ** (e + 1) <= q:
        e += 1
    shift = e - 23
    if e < -126:
        shift = -149
    scaled = q / (Fraction(2) ** shift)
    n = scaled.numerator // scaled.denominator
    rem = scaled - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and (n & 1)):
        n += 1
    v = n * (Fraction(2) ** shift)
    if v >= Fraction(2) ** 128:
        return sign * math.inf
    return sign * float(v)


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


C_01 = flt(0x3DCCCCCD)
C_03 = flt(0x3E99999A)
C_07 = flt(0x3F333333)
C_005 = flt(0x3D4CCCCD)
C_013 = flt(0x3E051EB8)
C_PI = flt(0x40490FDB)
C_2PI = flt(0x40C90FDB)
C_60 = 60.0
C_16667 = flt(0x41855555)
C_1E4 = flt(0x38D1B717)
C_4E4 = flt(0x39D1B717)
C_N005 = flt(0xBD4CCCCD)

kCanon = 'nan'


def canon(v):
    """Una palabra u32 con patron de NaN se compara como NaN cualquiera (la carga de NaN de los fakes y del x87 depende del camino)."""
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return kCanon
    return v


def canon_tree(x):
    if isinstance(x, list):
        return [canon_tree(i) for i in x]
    return canon(x)


class Arena:
    def __init__(self, inp):
        self.v = bytearray(VEH_SIZE)
        self.out = [0] * OUT_WORDS
        self.out[:4] = inp['outpre']
        self.ev = []

    def w8(self, off, val): self.v[off] = val & 0xFF
    def w16(self, off, val): self.v[off:off + 2] = struct.pack('<H', val & 0xFFFF)
    def w32(self, off, val): self.v[off:off + 4] = struct.pack('<I', val & 0xFFFFFFFF)
    def r8(self, off): return self.v[off]
    def r16(self, off): return struct.unpack_from('<H', self.v, off)[0]
    def r32(self, off): return struct.unpack_from('<I', self.v, off)[0]
    def rf(self, off): return flt(self.r32(off))
    def out8(self, idx, val):
        w = self.out[idx]
        self.out[idx] = (w & 0xFFFFFF00) | (val & 0xFF)


def build_vehicle(inp):
    a = Arena(inp)
    for k in range(3):
        a.w32(0x04 + 4 * k, inp['vpos'][k])
        a.w32(0x44 + 4 * k, inp['msp'][k])
    a.w32(0x14, 1 if inp['mat'] else 0)
    a.w8(0x3B9, inp['style']); a.w8(0x3BA, inp['mission']); a.w8(0x3D0, inp['cruise']); a.w8(0x428, inp['flags'])
    a.w32(0x460, 1 if inp['driver'] else 0)
    a.w16(0x4DC, inp['cops'])
    a.w32(0x594, inp['sub']); a.w32(0x818, inp['f818'])
    return a


def eff_pos(inp):
    p = inp['mpos'] if inp['mat'] else inp['vpos']
    return flt(p[0]), flt(p[1])


def finish(a, base):
    changes = []
    for w in range(VEH_SIZE // 4):
        new = struct.unpack_from('<I', a.v, 4 * w)[0]
        old = struct.unpack_from('<I', base.v, 4 * w)[0]
        if new != old:
            changes.append(['V', 0, w, new])
    for w in range(OUT_WORDS):
        if a.out[w] != base.out[w]:
            changes.append(['O', 0, w, a.out[w]])
    return {'events': canon_tree(a.ev), 'nevents': len(a.ev), 'changes': canon_tree(changes), 'globals': []}


def ftol16(v):
    """(uint16)(int64)v como el __ftol2 del EXE: 0 para NaN/inf/|v| >= 2^63."""
    if v != v or math.isinf(v) or abs(v) >= 2.0 ** 63:
        return 0
    return int(v) & 0xFFFF


def speed3(x, y, z, order):
    sq = {'x': fmul(x, x), 'y': fmul(y, y), 'z': fmul(z, z)}
    a, b, c = order
    return fsqrt(fadd(fadd(sq[a], sq[b]), sq[c]))


def stop(inp, stats):
    a = build_vehicle(inp)
    base = build_vehicle(inp)
    x, y, a4, a5 = [flt(u) for u in inp['in']]
    px, py = eff_pos(inp)
    a.out[0] = 0; a.out[1] = 0; a.out[2] = 0x3F800000; a.out8(3, 1)
    d1 = fsub(py, y); d2 = fsub(py, y)
    t = fmul(d1, d2)
    e1 = fsub(px, x); e2 = fsub(px, x)
    dist = fadd(t, fmul(e1, e2))
    mission = a.r8(0x3BA)
    if dist > 100.0:
        a.w8(0x3BA, 0x12 if mission == 0x13 else 5)
        stats['far'] = True
        return finish(a, base)
    fpv_calls = [0]
    pvmask = inp['pvmask']

    def fpv():
        k = fpv_calls[0]; fpv_calls[0] += 1
        a.ev.append([K_FPV, 0xFFFFFFFF, 0])
        return (pvmask >> (k & 31)) & 1

    def tell():
        a.ev.append([K_TELL, 'V'])
        a.w8(0x3D0, 0); a.w8(0x3BA, 0)

    if mission == 0x13:
        stats['m13'] = True
        if inp['driver']:
            a.ev.append([K_GETACTIVE, 4 if inp['intel_null'] else 'I+4'])
            if inp['task'] != 0:
                a.ev.append([K_TASKTYPE, 'T'])
                if inp['task'] == 1:
                    stats['m13_killcriminal'] = True
                    return finish(a, base)
        sp = fadd(fmul(flt(inp['msp'][0]), flt(inp['msp'][0])), fmul(flt(inp['msp'][1]), flt(inp['msp'][1])))
        if not (sp < C_1E4):
            stats['m13_fast'] = True
            return finish(a, base)
        inq = fadd(fmul(a4, a4), fmul(a5, a5))
        if not (inq < C_4E4):
            stats['m13_input'] = True
            return finish(a, base)
        if not (a.r8(0x428) & 1):
            stats['m13_notlaw'] = True
            return finish(a, base)
        stats['m13_tell'] = True
        tell()
        return finish(a, base)

    pv = [flt(u) for u in inp['pv']]

    def pv_below():
        return speed3(pv[0], pv[1], pv[2], 'xyz') < C_005

    if fpv():
        assert (pvmask >> (fpv_calls[0])) & 1, 'fixture: FindPlayerVehicle nulo desreferenciado'
        fpv()
        if pv_below():
            stats['timer_update'] = True
            v = fadd(fmul(flt(inp['step']), C_16667), float(a.r16(0x4DC)))
            a.w16(0x4DC, ftol16(v))
        else:
            stats['timer_zero_fast'] = True
            a.w16(0x4DC, 0)
    else:
        stats['timer_zero_nopv'] = True
        a.w16(0x4DC, 0)

    if fpv():
        k_target = fpv_calls[0]
        fpv()
        a.ev.append([K_UPSIDE, 'PV' if (pvmask >> k_target) & 1 else 0])
        up = inp['upmask'] & 1
        if not up:
            assert (pvmask >> fpv_calls[0]) & 1, 'fixture: FindPlayerVehicle nulo desreferenciado (2)'
            fpv()
            if not pv_below():
                stats['notbelow'] = True
                return finish(a, base)
            if a.r16(0x4DC) <= 0x9C4:
                stats['timer_low'] = True
                return finish(a, base)
            stats['timer_high'] = True
        else:
            stats['upside'] = True
    else:
        stats['nopv2'] = True
    if not (a.r8(0x428) & 1):
        stats['notlaw'] = True
        return finish(a, base)
    if not (dist < 100.0):
        stats['dist_ge'] = True
        return finish(a, base)
    stats['tell'] = True
    tell()
    return finish(a, base)


def fp_events_atan(a, x, y):
    a.ev.append([K_ATAN, bits(x), bits(y)])


def heading_core(a, inp, stats, vehicle_args, outs_base=0):
    """HeadingForTarget sobre la arena `a` (compartido con nada mas: TryingToBlockTarget lo sustituye por un falso)."""
    x, y = [flt(u) for u in inp['in'][:2]]
    px, py = eff_pos(inp)
    top = [flt(u) for u in inp['mtop']]
    a.out8(3, 0)
    # direccion de avance normalizada (0x433290..0x4332DE)
    tx, ty = top[0], top[1]
    ln = fsqrt(fadd(fmul(ty, ty), fmul(tx, tx)))
    if ln == 0.0:
        fx, fy = 1.0, ty
        stats['len0'] = True
    else:
        inv = fdiv(1.0, ln)
        fx, fy = fmul(inv, tx), fmul(inv, ty)
    # primer fpatan: rumbo al objetivo; segundo: rumbo del vehiculo
    dx1, dy1 = fsub(x, px), fsub(y, py)
    fp_events_atan(a, dx1, dy1)
    A = f32(dflt(inp['atan'][0]))
    fp_events_atan(a, fx, fy)
    Hf = f32(dflt(inp['atan'][1]))
    style = inp['style']
    if style in (2, 5, 6):
        stats['weave'] = True
        a.ev.append([K_WEAVE, 'V', 'PV' if inp['target'] else 0, bits(A), bits(Hf), 0x3F800000])
        ang = flt(inp['weave'])
    else:
        ang = A
    diff = fsub(ang, Hf)
    n = 0
    while diff < -C_PI:
        diff = fadd(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_lo'] = True
    while diff > C_PI:
        diff = fsub(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_hi'] = True
    ms = [flt(u) for u in inp['msp']]
    spd = speed3(ms[0], ms[1], ms[2], 'xyz')
    if spd > C_03:
        stats['fast'] = True
        ab = -diff if diff < 0 else diff
        if ab > C_07:
            a.out8(3, 1)
            stats['handbrake'] = True
    a.ev.append([K_MAXSTEER, 'V'])
    M = flt(inp['maxsteer'])
    if diff < -M:
        diff = -M; stats['clamp_lo'] = True
    if diff > M:
        diff = M; stats['clamp_hi'] = True
    # tercer fpatan: la resta con el rumbo del vehiculo se hace sobre el valor sin redondear
    dx2, dy2 = fsub(x, px), fsub(y, py)
    fp_events_atan(a, dx2, dy2)
    d2 = dflt(inp['atan'][2])
    if d2 != d2:
        r = math.nan
    elif math.isinf(d2) or Hf != Hf:
        r = d2 - Hf
    else:
        r = f32_exact(Fraction(d2) - Fraction(Hf))
    a.ev.append([K_SPEEDMUL, bits(r), 0x3ECCCCCD, 0x3F99999A, 0x3ECCCCCD])
    R = flt(inp['speedmul'])
    ms2 = fadd(fadd(fmul(ms[2], ms[2]), fmul(ms[1], ms[1])), fmul(ms[0], ms[0]))
    V = fmul(fsqrt(ms2), C_60)
    err = fsub(fmul(R, float(inp['cruise'])), V)
    a.out[2] = 0
    if err > 0.0:
        stats['accel'] = True
        if V < 25.0:
            sc = fmul(C_01, err)
            a.out[1] = bits(1.0 if 1.0 < sc else sc)
            stats['accel_lin'] = True
        else:
            a.out[1] = bits(1.0)
        if a.r32(0x594) == 10 and err > 3.0:
            t = a.rf(0x818)
            if t <= 0.0:
                a.w32(0x818, 0x41200000)
                stats['rearm'] = True
    else:
        stats['brake'] = True
        sc = fmul(err, C_N005)
        a.out[1] = 0
        a.out[2] = bits(0.5 if 0.5 < sc else sc)
    a.out[0] = bits(diff)


def heading(inp, stats):
    a = build_vehicle(inp)
    base = build_vehicle(inp)
    for k in range(3):   # matriz: solo importa la direccion y la posicion de la matriz, que viven fuera del vehiculo y no cambian
        pass
    heading_core(a, inp, stats, None)
    return finish(a, base)


def block(inp, stats):
    a = build_vehicle(inp)
    base = build_vehicle(inp)
    a3, a4, a5, a6 = [flt(u) for u in inp['in']]
    px, py = eff_pos(inp)
    n = fsqrt(fadd(fmul(a5, a5), fmul(a6, a6)))
    dx, dy = a5, a6
    if n > C_013:
        stats['scaled'] = True
        s = fdiv(C_013, n)
        dx, dy = fmul(a5, s), fmul(a6, s)
    X = fadd(fmul(dx, C_60), a3)
    Y = fadd(fmul(dy, C_60), a4)
    a.w8(0x3B9, 2)
    a.ev.append([K_HEADING, 'V', 0, bits(X), bits(Y), 'O', 'O+4', 'O+8', 'O+12'])
    a.out[0], a.out[1], a.out[2] = inp['hout'][0], inp['hout'][1], inp['hout'][2]
    a.out8(3, inp['hout'][3])
    d = fadd(fmul(fsub(Y, py), fsub(Y, py)), fmul(fsub(X, px), fsub(X, px)))
    if d < 25.0:
        stats['near'] = True
        a.w8(0x3BA, 0x13 if a.r8(0x3BA) == 0x12 else 6)
        stats['m12' if base.r8(0x3BA) == 0x12 else 'mother'] = True
    return finish(a, base)


def expected(name, inp, case):
    stats = {}
    fn = inp['fn']
    if NAMES[fn] != name and name != NAME:
        raise RuntimeError('funcion inesperada ' + name)
    if fn == 0:
        want = stop(inp, stats)
    elif fn == 1:
        want = heading(inp, stats)
    else:
        want = block(inp, stats)
    stats['fn%d' % fn] = True
    stats['family%d_fn%d' % (inp['fam'], fn)] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': obs['globals']}


def selftest():
    # Stop lejos: mision 0x13 -> 0x12; el resto -> 5; salidas fijas
    inp = dict(fn=0, fam=0, mat=1, driver=0, intel_null=0, task=0, vpos=[0, 0, 0], mpos=[bits(0.0), bits(0.0), 0], mtop=[0, 0, 0], msp=[0, 0, 0], style=0, mission=0x13, cruise=7, flags=1, cops=0, sub=0, f818=0,
               **{'in': [bits(20.0), bits(0.0), 0, 0]}, pvmask=0, upmask=0, pv=[0, 0, 0], step=0, atan=[0, 0, 0], weave=0, maxsteer=0, speedmul=0, hout=[0, 0, 0, 0], outpre=[0xC0000001, 0xC0000002, 0xC0000003, 0xC0000004], target=0)
    want, _ = expected(NAME, inp, 0)
    assert want['events'] == [], want
    assert any(c[0] == 'O' and c[2] == 2 and c[3] == 0x3F800000 for c in want['changes']), want
    assert any(c[0] == 'V' and c[2] == 0xEE and ((c[3] >> 16) & 0xFF) == 0x12 for c in want['changes']), want
    # f32_exact: redondeo al par y subnormales
    assert f32_exact(Fraction(1) + Fraction(1, 2 ** 24)) == 1.0
    assert f32_exact(Fraction(1) + Fraction(3, 2 ** 24)) == 1.0 + 2.0 ** -22
    assert f32(0.1) == flt(0x3DCCCCCD)
    # TryingToBlockTarget cerca: mision 0x12 -> 0x13
    inp2 = dict(inp, fn=2, mission=0x12, hout=[1, 2, 3, 4], **{'in': [bits(0.0), bits(0.0), bits(0.0), bits(0.0)]})
    want2, st = expected(NAMES[2], inp2, 0)
    assert st.get('near') and want2['events'][0][0] == K_HEADING and want2['events'][0][3] == 0, want2
    assert any(c[0] == 'V' and c[2] == 0xEE and ((c[3] >> 16) & 0xFF) == 0x13 for c in want2['changes']), want2
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
