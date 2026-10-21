"""Oraculo independiente del banco CarCtrlSteer63e (lote 63, CCarCtrl parte B1, tanda 5): modela, desde el DESENSAMBLADO del EXE (no desde la fuente C++),
  0x433BA0 SteerAICarParkParallel       (vehicle, float* steer, float* accel, float* brake, bool* handbrake)
  0x433EA0 SteerAICarParkPerpendicular  (igual)
incluidas la lectura de nodos (0x420A10), CVector::Normalise (0x59C910) y CCollision::DistToMathematicalLine (0x412970), que en el banco corren de verdad en el EXE y en la DLL, y la
creacion de las tareas de bajada (0x61A5A0, 0x421150 / 0x61A3B0 inlinado, 0x681AF0), con aritmetica float de 24 bits (x87 con PC=24 == float IEEE para + - * / y raiz).
`expected(name, input, case)` devuelve (observado esperado, estadisticas de cobertura) con el mismo formato que registra Core.hpp. Autoprueba:  python -I fixture_oracle.py
"""
import math
import struct

CASES = 8192
NAMES = ['CCarCtrl::SteerAICarParkParallel', 'CCarCtrl::SteerAICarParkPerpendicular']
NAME = NAMES[0]
K_NEW, K_SETTASK, K_HEADING = 1, 2, 3
VEH_SIZE = 0x900
OUT_WORDS = 8
NUM_TASKS = 9
TASK_WORDS = 5
PATH_WORDS = 48


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


kCanon = 'nan'


def canon(v):
    if isinstance(v, int) and (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF) != 0:
        return kCanon
    return v


def canon_tree(x):
    if isinstance(x, list):
        return [canon_tree(i) for i in x]
    return canon(x)


def normalise3(v):
    """0x59C910: suma (x*x + y*y) + z*z; 0 -> x = 1.0 (y y z intactos); si no, inv = 1 / sqrt(suma) y x*inv, y*inv, z*inv"""
    x, y, z = v
    s = fadd(fadd(fmul(x, x), fmul(y, y)), fmul(z, z))
    if s == 0.0:
        return [1.0, y, z]
    inv = fdiv(1.0, fsqrt(s))
    return [fmul(inv, x), fmul(inv, y), fmul(inv, z)]


def dist_to_line(p1, p2, p3):
    """0x412970 desensamblado (linea p1 -> p2, punto p3)"""
    a, b, c = fsub(p2[0], p1[0]), fsub(p2[1], p1[1]), fsub(p2[2], p1[2])
    x, y, z = fsub(p3[0], p1[0]), fsub(p3[1], p1[1]), fsub(p3[2], p1[2])
    dot = fadd(fadd(fmul(x, a), fmul(z, c)), fmul(y, b))
    s = fadd(fadd(fmul(z, z), fmul(y, y)), fmul(x, x))
    lsq = fadd(fadd(fmul(a, a), fmul(c, c)), fmul(b, b))
    asq = fsub(s, fdiv(fmul(dot, dot), lsq))
    if asq > 0.0 or asq != asq:
        return fsqrt(asq)
    return 0.0


class Arena:
    def __init__(self, inp):
        self.v = bytearray(VEH_SIZE)
        self.out = [0] * OUT_WORDS
        self.out[:4] = inp['outpre']
        self.ev = []
        self.tk = [[0xDEAD0000 + k * 8 + w for w in range(TASK_WORDS)] for k in range(NUM_TASKS)]
        self.tk_base = [list(r) for r in self.tk]

    def w8(self, off, val): self.v[off] = val & 0xFF
    def w32(self, off, val): self.v[off:off + 4] = struct.pack('<I', val & 0xFFFFFFFF)
    def r8(self, off): return self.v[off]
    def out8(self, idx, val):
        w = self.out[idx]
        self.out[idx] = (w & 0xFFFFFF00) | (val & 0xFF)


def build_vehicle(inp):
    a = Arena(inp)
    for k in range(3):
        a.w32(0x04 + 4 * k, inp['vpos'][k])
    a.w32(0x14, 1 if inp['mat'] else 0)
    a.w8(0x3BA, inp['mission']); a.w8(0x3D0, inp['cruise']); a.w8(0x3DC, inp['flags3dc']); a.w8(0x428, inp['flags428'])
    a.w32(0x390, inp['cur']); a.w32(0x394, inp['start'])
    a.w32(0x460, 1 if inp['driver'] else 0)
    for i in range(8):
        a.w32(0x464 + 4 * i, 1 if (inp['pass'] >> i) & 1 else 0)
    return a


def finish(a, base, globals_):
    changes = []
    for w in range(VEH_SIZE // 4):
        new = struct.unpack_from('<I', a.v, 4 * w)[0]
        old = struct.unpack_from('<I', base.v, 4 * w)[0]
        if new != old:
            changes.append(['V', 0, w, new])
    for k in range(NUM_TASKS):
        for w in range(TASK_WORDS):
            if a.tk[k][w] != a.tk_base[k][w]:
                changes.append(['T', k, w, 'VT' if w == 0 else a.tk[k][w]])
    for w in range(OUT_WORDS):
        if a.out[w] != base.out[w]:
            changes.append(['O', 0, w, a.out[w]])
    return {'events': canon_tree(a.ev), 'nevents': len(a.ev), 'changes': canon_tree(changes), 'globals': globals_}


def node_pos(inp, which):
    """posicion (x, y, z) del nodo inicial (which = 0) o actual (1)"""
    addr = inp['start'] if which == 0 else inp['cur']
    array = 0 if (which == 0 or inp['same']) else 1
    idx = addr >> 16
    x, y, z = inp['nodes'][array][idx]
    return [fmul(float(x), 0.125), fmul(float(y), 0.125), fmul(float(z), 0.125)]


def path_globals(inp):
    return []        # la funcion no cambia los punteros por area


def park_tasks(a, inp):
    k = 0

    def one(ped_index):
        nonlocal k
        a.ev.append([K_NEW, 0x14])
        block = None
        if k < NUM_TASKS and (inp['newmask'] >> k) & 1:
            block = k
            a.tk[k][0] = 0x56540000                      # vptr de la tarea (EXE o DLL): se ficha como VT
            a.tk[k][1] = 0
            a.tk[k][2] = 0
            a.tk[k][3] = 0
            a.tk[k][4] = (a.tk[k][4] & 0xFFFF0000) | 0x0001   # bytes +0x10 = 1 (conducir normal), +0x11 = 0; +0x12/+0x13 intactos
        k += 1
        a.ev.append([K_SETTASK, 'I%d+4' % ped_index, 'T%d' % block if block is not None else 0, 3, 0])
    if inp['driver']:
        one(0)
    for i in range(8):
        if (inp['pass'] >> i) & 1:
            one(1 + i)


def park(inp, stats):
    a = build_vehicle(inp)
    base = build_vehicle(inp)
    fn = inp['fn']
    fail = (not inp['loaded'][0]) or (not inp['loaded'][1])
    if fail:
        stats['fail'] = True
        a.out[2] = 0; a.out[1] = 0; a.out[0] = 0
        a.w8(0x3BA, 0x0B)
        return finish(a, base, [])
    start, cur = node_pos(inp, 0), node_pos(inp, 1)
    mission = a.r8(0x3BA)
    px, py = (flt(inp['mpos'][0]), flt(inp['mpos'][1])) if inp['mat'] else (flt(inp['vpos'][0]), flt(inp['vpos'][1]))
    if fn == 0:
        if mission == 0x2E:
            stats['m2e'] = True
            d = normalise3([fsub(start[0], cur[0]), fsub(start[1], cur[1]), fsub(start[2], cur[2])])
            tgt = [fadd(d[0], start[0]), fadd(d[1], start[1])]
        else:
            tgt = [start[0], start[1]]
    else:
        if mission == 0x2D:
            stats['m2d'] = True
            dist = dist_to_line([start[0], start[1], 0.0], [cur[0], cur[1], 0.0], [px, py, 0.0])
            if dist < 6.0:
                stats['to31'] = True
                mission = 0x31
                a.w8(0x3BA, 0x31)
        tgt = [cur[0], cur[1]] if mission == 0x2D else [start[0], start[1]]
    a.ev.append([K_HEADING, 'V', 0, bits(tgt[0]), bits(tgt[1]), 'O', 'O+4', 'O+8', 'O+12'])
    a.out[0], a.out[1], a.out[2] = inp['hout'][0], inp['hout'][1], inp['hout'][2]
    a.out8(3, inp['hout'][3])
    cruise = a.r8(0x3D0)
    a.w8(0x3D0, cruise if cruise < 8 else 8)
    if fn == 0:
        dx, dy = fsub(tgt[0], px), fsub(tgt[1], py)
        dist = fsqrt(fadd(fmul(dx, dx), fmul(dy, dy)))
        if mission == 0x2E:
            if dist < 4.0:
                stats['to32'] = True
                a.w8(0x3BA, 0x32)
            return finish(a, base, [])
    else:
        dx, dy = fsub(start[0], px), fsub(start[1], py)
        dist = fsqrt(fadd(fmul(dx, dx), fmul(dy, dy)))
    if not (dist < 2.0):
        stats['far'] = True
        return finish(a, base, [])
    stats['park'] = True
    if fn == 1:
        a.w8(0x3DC, a.r8(0x3DC) | 2)
    a.w8(0x3BA, 0x0B)
    a.w8(0x428, a.r8(0x428) & 0xAF)
    park_tasks(a, inp)
    stats['tasks'] = bool(inp['driver'] or inp['pass'])
    return finish(a, base, [])


def expected(name, inp, case):
    stats = {}
    if NAMES[inp['fn']] != name:
        raise RuntimeError('funcion inesperada ' + name)
    want = park(inp, stats)
    stats['fn%d' % inp['fn']] = True
    stats['family%d_fn%d' % (inp['fam'], inp['fn'])] = True
    return want, stats


def observed_canon(obs):
    """Observado del banco -> forma canonica (NaN cualquiera == NaN)."""
    return {'events': canon_tree(obs['events']), 'nevents': obs['nevents'], 'changes': canon_tree(obs['changes']), 'globals': canon_tree(obs['globals'])}


def selftest():
    # distancia a una recta: punto a 3 de la recta y = 0 (z = 0)
    assert dist_to_line([0.0, 0.0, 0.0], [10.0, 0.0, 0.0], [5.0, 3.0, 0.0]) == 3.0
    assert dist_to_line([0.0, 0.0, 0.0], [10.0, 0.0, 0.0], [5.0, 0.0, 0.0]) == 0.0
    assert normalise3([0.0, 0.0, 0.0]) == [1.0, 0.0, 0.0]
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
