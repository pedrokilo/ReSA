"""Generador de casos del banco Fn238A: escribe fn238a-cases.jsonl (una linea JSON por caso; el banco la lee tal cual y el oraculo parte de la misma linea).

  python -B fixture_cases.py <salida.jsonl> [casos_por_funcion]       (16384 por defecto)

Familia = n % 16: tipo = familia % 4 (colocacion del objeto principal) y relleno = familia // 4 (aleatorio, ceros, unos, 0xCD).
fn 0: MD5Transform(estado, bloque): estado en la arena A y bloque en la B; el tipo elige alineado, desalineado, al borde de la ventana (la escritura entra en la guarda) o bloque que
      solapa el estado (en el mismo sitio o desplazado).
fn 1: ProcessCommands1700To1799: n % 128 elige el comando (0..99 -> 1700..1799; 100..127 -> valores fuera de rango); el mundo (pools, entidades, modelos, globales) se escribe entero en cada
      caso; los parametros de script los fija el falso de CollectParameters (campo `sp`), los retornos programados de los colaboradores van en `r`.
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_common as FC
from fixture_common import M32, rnd, PS, f2b
import fixture_oracle as O

STACK_SEEDS = [0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555]
OUTSIDE = [0, 1, 1699, 1800, 1801, 1899, 2000, 0xFFFFFFFF, 0xFFFFFF4C, 0x7FFFFFFF, 0x80000000, 0x800006A4, 0x100006A4, 0x1000, 99, 1700 - 4096, 3000, 65535, 0x6A4 + 0x10000, 1698,
           1802, 0xFFFF06A4, 1700 + 100, 1700 - 1, 5000, 100000, 0x7FFFFFFE, 0xFFFE0000]


class Gen:
    def __init__(self, n, salt):
        self.n, self.k = n, salt

    def nk(self):
        self.k += 1
        return self.k

    def R(self, m):
        return rnd(self.n, self.nk()) % m

    def P(self, pct):
        return self.R(100) < pct

    def W(self):
        w = rnd(self.n, self.nk())
        if (w >> 24) == 0xFA:
            w ^= 0x05000000
        return w

    def pick(self, lst):
        return lst[self.R(len(lst))]

    def f(self, nan=False):
        """flotante: valores notables y aleatorios en rango normal (sin denormales ni desbordes)"""
        r = self.R(100)
        if r < 6:
            return self.pick([0.0, -0.0, 1.0, -1.0, 100.0, -100.0, 0.5, 2.0, 50.0, 1000.0])
        if nan and r < 10:
            return float('nan')
        e = self.R(41) - 20
        m = 1.0 + (self.R(1 << 23) / float(1 << 23))
        v = m * (2.0 ** e)
        return -v if self.R(2) else v


def fb(x):
    return 0xFFC00000 if x != x else f2b(x)


class Case:
    def __init__(self, fn, n):
        self.fn, self.n = fn, n
        self.p = []
        self.r = []
        self.sp = [0] * 32
        self.tx = [0, 0, 0, 0]
        self.args = []
        self.ecx = 0

    def put(self, dest, off, width, val):
        self.p.append((dest << 8) | width)
        self.p.append(off & M32)
        self.p.append(val & M32)

    def a(self, arena, off, v, w=4):
        self.put(arena, off, w, v)

    def g(self, addr, v, w=4):
        self.put(4, addr, w, v)

    def ret(self, cid, n, v):
        self.r += [cid, n, v & M32]


def stack_seed(n):
    return STACK_SEEDS[n % 13] if n % 13 < 8 else rnd(n + 0x1000, 99)


def header(c, fn, n):
    fam = n % 16
    seed = (n * 13 + 50 + fn * 7919) & M32
    return dict(fn=fn, n=n, family=fam, eax=rnd(n, 1 + fn) & M32, ecx=0, edx=rnd(n, 2 + fn) & M32, stack=stack_seed(n), fill=[fam // 4, seed, seed + 1, seed + 2, seed + 3])


def gen_md5(n):
    c = Case(0, n)
    g = Gen(n, 100)
    fam = n % 16
    kind = fam % 4
    # estado (16 bytes) en A y bloque (64 bytes) en B (o en A solapado)
    if kind == 0:
        os_, ob, bar = 4 * g.R((FC.WIN - 16) // 4 + 1), 4 * g.R((FC.WIN - 64) // 4 + 1), 1
    elif kind == 1:
        os_, ob, bar = 4 * g.R((FC.WIN - 16) // 4 - 1) + 1 + g.R(3), 4 * g.R((FC.WIN - 64) // 4 - 1) + 1 + g.R(3), 1
    elif kind == 2:
        os_, ob, bar = FC.WIN - 16 - g.R(0x30) * 0 - 4 * g.R(2), FC.WIN - 64 - 4 * g.R(2), 1
    else:
        os_ = 4 * (4 + g.R(0x7E0))
        bar = 0
        ob = os_ if g.P(40) else os_ + 4 * (1 + g.R(3)) * (1 if g.P(50) else -1) + (g.R(4) if g.P(30) else 0)
    words = []
    for i in range(20):
        w = g.pick([0, M32, 0x80000000, 0x7FFFFFFF, 1, rnd(n, 300 + i), rnd(n, 400 + i), rnd(n, 500 + i), rnd(n, 600 + i), rnd(n, 700 + i)])
        if (w >> 24) == 0xFA:
            w ^= 0x05000000   # no una especificacion de puntero
        words.append(w)
    for i in range(4):
        c.a(0, os_ + 4 * i, words[i])
    for i in range(16):
        c.a(bar, ob + 4 * i, words[4 + i])
    c.args = [PS(0, os_), PS(bar, ob)]
    d = header(c, 0, n)
    d.update(args=c.args, p=c.p, r=[], sp=[0] * 32, tx=[0] * 4)
    return d


# ----------------------------------------------------------------------------------------------- mundo del despachador
def build_world(c, g, n):
    """pools, entidades, modelos y globales del caso; devuelve el diccionario de entidades"""
    w = {}
    # ---- pools: cabecera (almacen, estados, capacidad, ultimo libre, banderas)
    for hdr, slots, arena, base, cap, gp in ((FC.A_PEDPOOL, FC.A_PEDSLOT, 1, FC.B_PED, FC.NPED, 0xB74490), (FC.A_VEHPOOL, FC.A_VEHSLOT, 2, FC.C_VEH, FC.NVEH, 0xB74494),
                                             (FC.A_OBJPOOL, FC.A_OBJSLOT, 2, FC.C_OBJ, FC.NOBJ, 0xB7449C)):
        c.a(0, hdr, PS(arena, base))
        c.a(0, hdr + 4, PS(0, slots))
        c.a(0, hdr + 8, cap)
        c.a(0, hdr + 12, M32)
        c.a(0, hdr + 16, 0)
        c.g(gp, PS(0, hdr))
    pedstate, vehstate, objstate = [], [], []
    for k in range(FC.NPED):
        s = g.R(0x7F) | (0x80 if (g.P(20) and k >= 2) else 0)
        pedstate.append(s)
        c.a(0, FC.A_PEDSLOT + k, s, 1)
    for k in range(FC.NVEH):
        s = g.R(0x7F)
        vehstate.append(s)
        c.a(0, FC.A_VEHSLOT + k, s, 1)
    for k in range(FC.NOBJ):
        s = g.R(0x7F)
        objstate.append(s)
        c.a(0, FC.A_OBJSLOT + k, s, 1)
    w['ped'] = [(k << 8) | pedstate[k] for k in range(FC.NPED) if not pedstate[k] & 0x80]
    w['veh'] = [(k << 8) | vehstate[k] for k in range(FC.NVEH)]
    w['obj'] = [(k << 8) | objstate[k] for k in range(FC.NOBJ)]
    # ---- peds
    for k in range(FC.NPED):
        b = FC.B_PED + k * FC.PED_STRIDE
        usem = g.P(50)
        c.a(1, b + 0x24, 0)
        c.a(1, b + 0x14, PS(3, FC.D_MAT + k * 0x60) if usem else 0)
        for o in (0x4, 0x8, 0xC):
            c.a(1, b + o, fb(g.f()))
        if usem:
            for o in (0x30, 0x34, 0x38):
                c.a(3, FC.D_MAT + k * 0x60 + o, fb(g.f()))
        c.a(1, b + 0x46C, g.W() & ~0x100 | (0x100 if g.P(50) else 0))
        c.a(1, b + 0x58C, g.pick([0, PS(2, FC.C_VEH), PS(2, FC.C_VEH + FC.VEH_STRIDE)]))
        for o in (0x44, 0x48, 0x4C):
            c.a(1, b + o, fb(g.f()))
        c.a(1, b + 0x480, PS(0, FC.A_PLAYERDATA))
    c.a(0, FC.A_PLAYERDATA + 0x84, g.W(), 4)
    # ---- vehiculos
    for k in range(FC.NVEH):
        b = FC.C_VEH + k * FC.VEH_STRIDE
        c.a(2, b + 0x24, 0)
        for o in (0x44, 0x48, 0x4C):
            c.a(2, b + o, fb(g.f()))
        c.a(2, b + 0x22, g.pick(FC.MODELS), 2)
        c.a(2, b + 0x3BA, g.pick([0x39, 0x3A, 0x39, 0x3A, 0, 9, 0x21, g.R(256)]), 1)
        c.a(2, b + 0x36, g.R(256), 1)
        c.a(2, b + 0x3D0, g.pick([0, 1, 2, 255, g.R(256)]), 1)
        c.a(2, b + 0x3AC, g.W())
    for k in range(FC.NOBJ):
        c.a(2, FC.C_OBJ + k * FC.OBJ_STRIDE + 0x24, 0)
    # ---- modelos
    for m in FC.MODELS:
        c.g(0xA9B0C8 + 4 * m, PS(3, FC.D_MI[FC.MODEL_MI[m]]))
    for mi in FC.D_MI:
        c.a(3, mi + 0x12, g.W() & 0xFFFF, 2)
        for i in range(18):
            c.a(3, mi + 0x2D6 + 2 * i, g.pick([0xFFFF, 0, g.R(0x10000)]), 2)
    # ---- globales
    c.g(0x8D237C, g.pick([6, 0, 1, 2, 3, 7, g.W()]))
    c.g(0xB72958, g.W())
    c.g(0x8A5B28, g.W())
    c.g(0x8A5E48, g.W())
    c.g(0x8A5E4C, g.W())
    c.g(0x96A8B0, g.W())
    c.g(0xB5F84C, g.pick([0, 1, 2, 2, g.W()]))
    c.g(0xB7CB84, g.W())
    c.g(0x8D2E98, g.R(64))
    c.g(0xC098E0, g.pick([0x01010101, 0, 0x01000101, g.W()]))
    c.g(0xC098E4, g.pick([0x01010101, 0, 0x00010100, g.W()]))
    for pl in range(2):
        c.g(0xB7CD98 + 0x190 * pl, PS(1, FC.B_PED + g.R(FC.NPED) * FC.PED_STRIDE))
    for k in range(1, 5):
        c.g(0xA44B88 + 0x1C * k, g.pick(FC.MODELS))
    for gi in range(8):
        c.g(0xC0994C + 0x2D4 * gi, g.W())
    for i in range(32):
        c.g(0xA43C78 + 4 * i, g.W())
    # script: bandera de limpieza de mision y SceneSkipIP
    return w


def rnd_int(g):
    return g.pick([0, 1, M32, 0x80000000, 0x7FFFFFFF, 2, 100, g.W(), g.W(), g.R(1000), (0x10000 - g.R(4))])


def param(kind, g, w):
    if kind == 'ped':
        return g.pick(w['ped'])
    if kind == 'veh':
        return g.pick(w['veh'])
    if kind == 'obj':
        return g.pick(w['obj'])
    if kind in ('pedm1',):
        return M32 if g.P(30) else g.pick(w['ped'])
    if kind == 'pedn':
        return g.pick([M32, 0xFFFFFFFE, 0x80000000]) if g.P(35) else g.pick(w['ped'])
    if kind == 'vehn':
        return g.pick([M32, 0xFFFFFFFE, 0x80000000]) if g.P(30) else g.pick(w['veh'])
    if kind == 'vehm1':
        return M32 if g.P(30) else g.pick(w['veh'])
    if kind == 'hdl':
        return g.pick([M32, 0, 1, 0x100, g.W(), g.R(0x10000)])
    if kind in ('i', 'rec', 'th', 'grp'):
        return rnd_int(g)
    if kind == 'dm':
        return M32 if g.P(40) else rnd_int(g)
    if kind == 'b':
        return 0 if g.P(45) else g.pick([1, g.W() | 1, 2, 0x100, g.W()])
    if kind in ('f', 'fa'):
        return fb(g.f())
    if kind in ('fz',):
        r = g.R(100)
        if r < 25:
            return fb(g.pick([-100.0, -100.0000076, -99.99999, -99.99999237060547, -1.0e30, 0.0]))
        if r < 32:
            return 0xFFC00000
        return fb(g.f())
    if kind == 'fn':
        return fb(g.f(nan=True)) if g.P(90) else g.pick([0x7FC00000, 0xFFC00000, 0x7F800000, 0xFF800000])
    if kind == 'fr':
        r = g.R(100)
        if r < 5:
            return 0xFFC00000
        if r < 8:
            return g.pick([0x7F800000, 0xFF800000])
        if r < 30:
            return fb(g.pick([0.0, 1.0, 5.0, 10.0, 100.0, 1.0e10, 1.0e-10, 3.0e18]))
        return fb(abs(g.f()))
    if kind in ('t', 'dur', 'tl'):
        return g.pick([M32, 0xFFFFFFFE, 0xFFFFFFFD, 0, 1, 1000, 0x7FFFFFFF, 0x80000000, g.R(100000)])
    if kind == 'pl':
        return g.R(2)
    if kind == 'n':
        return g.R(18)
    if kind == 'n3':
        return g.pick([0, 1, 2, 3, 5, M32, g.R(1000)])
    if kind == 'sm':
        return g.pick(FC.MODELS) if g.P(55) else (0x100000000 - g.R(4) - 1) & M32
    raise ValueError(kind)


def pret(kind, g, w):
    if kind == 'thing':
        return g.pick([M32, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0x3B, 0x3C, 0x3D, 100, g.W()]) if g.P(70) else g.pick([0, 1, 2, 3, 4, 5, 6, 7])
    if kind in ('int', 'node'):
        return g.W() if g.P(50) else g.R(10)
    if kind == 'int16':
        return g.pick([0xFFFF, 0, 1, g.W() & 0xFFFF, g.W()])
    if kind == 'bool':
        return g.R(2)
    if kind == 'f':
        return fb(g.f())
    if kind == 'task':
        return PS(3, FC.D_TASK)
    if kind == 'ptrA':
        return PS(0, 0x540 + g.pick([0, 0, 4, 1, 2, 3]))
    if kind == 'grp':
        return g.pick([0, PS(0, 0x580), PS(0, 0x584)])
    if kind in ('vehptr', 'trainptr'):
        return PS(2, FC.C_VEH + g.R(FC.NVEH) * FC.VEH_STRIDE)
    if kind == 'fireptr':
        return PS(0, FC.A_FIRE)
    if kind == 'p':
        return 0
    raise ValueError(kind)


def gen_cmd(n):
    c = Case(1, n)
    g = Gen(n, 1000)
    fam = n % 16
    kind = fam % 4
    i = (n + (n >> 7)) % 128   # cada comando una vez por ventana de 128 casos y con familias (colocacion del script) distintas en cada vuelta
    num = 1700 + i if i < 100 else g.pick(OUTSIDE)
    if i >= 100:
        num = OUTSIDE[(i - 100) % len(OUTSIDE)] if g.P(70) else g.pick(OUTSIDE)
    w = build_world(c, g, n)
    # script (this) en A
    if kind == 0:
        os_ = 4 * g.R(0xB0)
    elif kind == 1:
        os_ = 4 * g.R(0xB0) + 1 + g.R(3)
    elif kind == 2:
        os_ = 4 * g.R(8)
    else:
        os_ = 0x2C0 - 4 * g.R(8)
    c.a(0, os_ + 0xC6, g.pick([0, 1, 0x7F, 0xFF, 0x80]) if g.P(70) else g.R(2), 1)
    c.a(0, os_ + 0xD8, g.W())
    for k in range(3):
        c.a(0, FC.A_FIRE + 4 * k, fb(g.f()))
    c.ecx = PS(0, os_)
    c.args = [num]
    # parametros y retornos programados
    if num in O.PARAMS:
        k, kinds = O.PARAMS[num]
        for j, kd in enumerate(kinds):
            c.sp[j] = param(kd, g, w)
        for (cid, rk) in O.RETS[num]:
            for call in range(3):
                c.ret(cid, call, pret(rk, g, w))
        if num == 1784:
            for call in range(2):
                for kk, cid in enumerate((0x4505E0 | 0x02000000, 0x4505E0 | 0x03000000, 0x4505E0 | 0x04000000)):
                    c.ret(cid, call, fb(g.f()))
                c.ret(0x4505E0 | 0x05000000, call, g.R(2))
        # texto de las etiquetas (1745/1746): NUL en una posicion aleatoria; el resto, basura
        ln = g.pick([0, 1, 7, 8, 15, 16, g.R(17)])
        txt = [g.R(255) + 1 for _ in range(16)]
        for j in range(ln, 16):
            txt[j] = 0 if j == ln else g.R(256)
        c.tx = [txt[4 * q] | (txt[4 * q + 1] << 8) | (txt[4 * q + 2] << 16) | (txt[4 * q + 3] << 24) for q in range(4)]
    d = header(c, 1, n)
    d.update(ecx=c.ecx, args=c.args, p=c.p, r=c.r, sp=c.sp, tx=c.tx)
    return d


def make_case(fn, n):
    return gen_md5(n) if fn == 0 else gen_cmd(n)


if __name__ == '__main__':
    out = Path(sys.argv[1])
    cases = int(sys.argv[2]) if len(sys.argv) > 2 else 16384
    with out.open('w', encoding='utf-8', newline='\n') as fh:
        for fn in (0, 1):
            for n in range(cases):
                fh.write(json.dumps(make_case(fn, n), separators=(',', ':')) + '\n')
    print(out, 2 * cases, 'casos')
