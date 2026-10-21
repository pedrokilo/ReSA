"""Oraculo independiente del banco CarCtrlSteer63b (lote 63, CCarCtrl parte B1, tanda 2): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++),
  0x432DD0 SteerAICarWithPhysicsFollowPreRecordedPath (vehicle, float* steer, float* accel, float* brake, bool* handbrake)
con aritmetica float de 24 bits (x87 con PC=24 == float IEEE para + - * / y raiz). `expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el mismo formato
que registra Core.hpp. Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct

CASES = 4096
NAMES = ['CCarCtrl::SteerAICarWithPhysicsFollowPreRecordedPath']
NAME = NAMES[0]
K_ATAN, K_WEAVE, K_MAXSTEER, K_STOP = 1, 2, 3, 4
VEH_SIZE = 0x900
OUT_WORDS = 8
REC_BASE_WORD_INDEX = (0x97D7C0 - 0x97D6C0) // 4     # PlaybackIndex[0] dentro de la imagen de 112 palabras


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
def fsqrt(a):
    if a != a or a < 0:
        return math.nan
    return f32(math.sqrt(a))


def i16(u):
    u &= 0xFFFF
    return u - 0x10000 if u & 0x8000 else u


C_PI = flt(0x40490FDB)
C_2PI = flt(0x40C90FDB)
C_K = flt(0x38800100)
C_01 = flt(0x3E800000)      # 0.25 (0x858C84)
C_0125 = flt(0x3E000000)    # 0.125 (0x858C48)
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


class Arena:
    def __init__(self, inp):
        self.v = bytearray(VEH_SIZE)
        self.out = [0] * OUT_WORDS
        self.out[:4] = inp['outpre']
        self.ev = []

    def w8(self, off, val): self.v[off] = val & 0xFF
    def w32(self, off, val): self.v[off:off + 4] = struct.pack('<I', val & 0xFFFFFFFF)
    def out8(self, idx, val):
        w = self.out[idx]
        self.out[idx] = (w & 0xFFFFFF00) | (val & 0xFF)


def build_vehicle(inp):
    a = Arena(inp)
    for k in range(3):
        a.w32(0x04 + 4 * k, inp['vpos'][k])
        a.w32(0x44 + 4 * k, inp['msp'][k])
    a.w32(0x14, 1)
    a.w8(0x3BA, inp['mission']); a.w8(0x424, inp['recid'])
    return a


def finish(a, base, globals_):
    changes = []
    for w in range(VEH_SIZE // 4):
        new = struct.unpack_from('<I', a.v, 4 * w)[0]
        old = struct.unpack_from('<I', base.v, 4 * w)[0]
        if new != old:
            changes.append(['V', 0, w, new])
    for w in range(OUT_WORDS):
        if a.out[w] != base.out[w]:
            changes.append(['O', 0, w, a.out[w]])
    return {'events': canon_tree(a.ev), 'nevents': len(a.ev), 'changes': canon_tree(changes), 'globals': canon_tree(globals_)}


def follow(inp, stats):
    a = build_vehicle(inp)
    base = build_vehicle(inp)
    glob = []
    f0, nf = inp['f0'], inp['nf']
    px, py, pz = [flt(u) for u in inp['mpos']]

    def frame(i):
        if f0 <= i < f0 + 12 and i < 52:     # el banco escribe los 12 registros de la ventana (nf solo fija el tamano de la grabacion)
            return inp['frames'][i - f0]
        return [0, 0, 0, 0, 0]

    def stop():
        a.out[2] = 0; a.out[1] = 0; a.out[0] = 0; a.out8(3, 0)
        a.w8(0x3BA, 0xB)

    recid = inp['recid']
    if recid >= 0x80:
        stats['neg_id'] = True
        stop()
        return finish(a, base, glob)
    if not inp['base']:
        stats['null_base'] = True
        stop()
        return finish(a, base, glob)
    if inp['paused']:
        stats['paused'] = True
        a.out[1] = 0; a.out[0] = 0; a.out[2] = bits(0.5); a.out8(3, 0)
        return finish(a, base, glob)

    size = (f0 + nf) * 0x20 + inp['extra']
    start_off = inp['start'] * 0x20
    cur_off = start_off

    def dist(off, nxt_frame):
        fr = frame(off // 0x20 + (1 if nxt_frame else 0))
        x, y, z = flt(fr[0]), flt(fr[1]), flt(fr[2])
        dx, dy, dz = fsub(px, x), fsub(py, y), fsub(pz, z)
        return fsqrt(fadd(fadd(fmul(dz, dz), fmul(dy, dy)), fmul(dx, dx)))

    near = dist(start_off, False)
    nxt = dist(start_off, True)
    advances = 0
    while near < 10.0 or nxt < near:
        if near < 10.0:
            stats['loop_near'] = True
        else:
            stats['loop_next'] = True
        cur_off = cur_off + 0x20
        limit = (size - 0x20) & 0xFFFFFFFF
        advances += 1
        if cur_off >= limit:
            stats['end'] = True
            glob.append([REC_BASE_WORD_INDEX + inp['slot'], cur_off])
            a.ev.append([K_STOP, inp['slot']])
            a.w8(0x424, 0xFF)
            stop()
            return finish(a, base, glob)
        near = dist(cur_off, False)
        nxt = dist(cur_off, True)
        assert advances < 100
    if advances:
        glob.append([REC_BASE_WORD_INDEX + inp['slot'], cur_off])
        stats['advance1' if advances == 1 else 'advance2plus'] = True
    else:
        stats['noadvance'] = True

    rec = frame(inp['start'])
    # rumbo del vehiculo (top de la matriz) y rumbo al registro (el primero, no el avanzado)
    top = [flt(u) for u in inp['mtop']]
    a.ev.append([K_ATAN, bits(top[0]), bits(top[1])])
    H1 = f32(dflt(inp['atan'][0]))
    x, y = flt(rec[0]), flt(rec[1])
    a.ev.append([K_ATAN, bits(fsub(x, px)), bits(fsub(y, py))])
    A = f32(dflt(inp['atan'][1]))
    a.ev.append([K_WEAVE, 'V', 0, bits(A), bits(H1), 0x40000000])
    diff = fsub(flt(inp['weave']), H1)
    n = 0
    while diff < -C_PI:
        diff = fadd(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_lo'] = True
    while diff > C_PI:
        diff = fsub(diff, C_2PI); n += 1
        assert n < 100000
        stats['loop_hi'] = True
    a.ev.append([K_MAXSTEER, 'V'])
    M = flt(inp['maxsteer'])
    if diff < -M:
        diff = -M; stats['clamp_lo'] = True
    if diff > M:
        diff = M; stats['clamp_hi'] = True

    a4 = fmul(float(i16(rec[3])), C_K)
    a6 = fmul(float(i16(rec[4])), C_K)
    s = fsqrt(fadd(fmul(a4, a4), fmul(a6, a6)))
    T = fmul(fmul(s, flt(inp['speed'])), C_60)
    if not (cur_off > 0x320):
        if not (T > 5.0):
            T = 5.0
            stats['floor'] = True
    else:
        stats['late'] = True
    a.out[2] = 0
    vx, vy = flt(inp['msp'][0]), flt(inp['msp'][1])
    cur = fmul(fsqrt(fadd(fmul(vx, vx), fmul(vy, vy))), C_60)
    err = fsub(T, cur)
    if err > 0.0:
        k = C_01 if cur < 2.0 else C_0125
        stats['accel_a' if cur < 2.0 else 'accel_b'] = True
        sc = fmul(err, k)
        a.out[1] = bits(1.0 if 1.0 < sc else sc)
    else:
        stats['brake'] = True
        sc = fmul(err, C_N005)
        a.out[1] = 0
        a.out[2] = bits(0.5 if 0.5 < sc else sc)
    a.out[0] = bits(diff)
    a.out8(3, 0)
    stats['steer'] = True
    return finish(a, base, glob)


def expected(name, inp, case):
    stats = {}
    if name != NAME:
        raise RuntimeError('funcion inesperada ' + name)
    want = follow(inp, stats)
    stats['fn0'] = True
    stats['family%d_fn0' % inp['fam']] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': canon_tree(obs['globals'])}


def selftest():
    zero = [0, 0, 0, 0, 0]
    inp = dict(fam=0, recid=0xFF, slot=0, base=1, paused=0, f0=0, nf=3, start=0, extra=0, speed=0, vpos=[0, 0, 0], mpos=[0, 0, 0], mtop=[0, 0, 0], msp=[0, 0, 0], mission=5, frames=[zero] * 12,
               atan=[0, 0], weave=0, maxsteer=0, outpre=[0xC0000001, 0xC0000002, 0xC0000003, 0xC0000004])
    want, st = expected(NAME, inp, 0)
    assert st.get('neg_id') and any(c[0] == 'V' and c[2] == 0xEE and ((c[3] >> 16) & 0xFF) == 0xB for c in want['changes']), want
    assert [c for c in want['changes'] if c[0] == 'O'] == [['O', 0, 0, 0], ['O', 0, 1, 0], ['O', 0, 2, 0], ['O', 0, 3, 0xC0000000]], want
    inp2 = dict(inp, recid=3, slot=3, paused=1)
    want2, st2 = expected(NAME, inp2, 0)
    assert st2.get('paused') and ['O', 0, 2, 0x3F000000] in want2['changes'], want2
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
