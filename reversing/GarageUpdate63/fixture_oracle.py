"""Oraculo independiente del banco GarageUpdate63 (lote 63b): modela, DESDE EL DESENSAMBLADO del EXE (0x44AA50..0x44C8BF; tablas 0x44C7D4..0x44C8BC), la salida observable de CGarage::Update
para una fila `input` (la especificacion semantica del caso que registra Core.hpp), con las mismas fichas simbolicas de punteros que usa el banco:

  G garaje;  V<i> vehiculo i;  M<i> su matriz;  PED jugador;  W CWanted;  L local de pila;  numero = valor crudo.

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos de los falsos EN ORDEN con sus argumentos, cambios de la
arena del banco y del estado real del EXE que lee/escribe la funcion); `stats` son banderas de cobertura (una por etiqueta del EXE alcanzada). El flujo es un automata de etiquetas
con las direcciones del EXE (L_xxxx). Aritmetica x87 del EXE: PC 24 bits, exponente extendido (fracciones exactas).
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
from fractions import Fraction
import math

NAME = 'CGarage::Update'
NAMES = [NAME]
CASES = 16384
CASES_BY_NAME = {NAME: CASES}
M32 = 0xFFFFFFFF


# ---------------------------------------------------------------------------------------------- aritmetica x87 (24 bits) con fracciones
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


class NaN:
    def __init__(self, bits):
        self.bits = bits


INF = math.inf


def dec(bits):
    s = -1 if bits >> 31 else 1
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 255:
        return NaN(bits) if m else s * INF
    if e == 0:
        return s * Fraction(m, 1 << 149)
    return s * Fraction(8388608 + m, 1) * Fraction(2) ** (e - 150)


def _round_scaled(x, scale):
    q = x / scale
    n = q.numerator // q.denominator
    r = q - n
    if r > Fraction(1, 2) or (r == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return n * scale


def r24(x):
    """redondeo a 24 bits de mantisa con exponente sin limite (fadd/fsub/fmul con PC=24)"""
    if x == 0:
        return Fraction(0)
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    return _round_scaled(x, Fraction(2) ** (e - 23))


def rf32(x):
    """guardado a float (fstp dword): rejilla de subnormales, desbordamiento a infinito"""
    if x == 0:
        return Fraction(0)
    a = abs(x)
    if a < Fraction(1, 1 << 126):
        return _round_scaled(x, Fraction(1, 1 << 149))
    r = r24(x)
    if abs(r) >= Fraction(2) ** 128:
        return INF if r > 0 else -INF
    return r


def enc(v):
    if isinstance(v, NaN):
        return v.bits
    if v == INF:
        return 0x7F800000
    if v == -INF:
        return 0xFF800000
    if v == 0:
        return 0
    s = 0x80000000 if v < 0 else 0
    a = abs(v)
    if a < Fraction(1, 1 << 126):
        return s | int(a * (1 << 149))
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    m = (a / Fraction(2) ** e - 1) * (1 << 23)
    assert m.denominator == 1, v
    return s | ((e + 127) << 23) | int(m)


def _nan_of(a, b):
    if isinstance(a, NaN):
        return NaN(a.bits | 0x400000)
    return NaN(b.bits | 0x400000)


def add(a, b):
    if isinstance(a, NaN) or isinstance(b, NaN):
        return _nan_of(a, b)
    if a in (INF, -INF) or b in (INF, -INF):
        if a in (INF, -INF) and b in (INF, -INF) and a != b:
            return NaN(0xFFC00000)
        return a if a in (INF, -INF) else b
    return r24(a + b)


def sub(a, b):
    if isinstance(b, NaN):
        return add(a, b)
    return add(a, -b)


def mul(a, b):
    if isinstance(a, NaN) or isinstance(b, NaN):
        return _nan_of(a, b)
    if a in (INF, -INF) or b in (INF, -INF):
        if a == 0 or b == 0:
            return NaN(0xFFC00000)
        return INF if (a > 0) == (b > 0) else -INF
    return r24(a * b)


def store(v):
    """fstp dword -> bits"""
    if isinstance(v, NaN) or v in (INF, -INF):
        return enc(v)
    return enc(rf32(v))


def gt(a, b):
    return not isinstance(a, NaN) and not isinstance(b, NaN) and a > b


def lt(a, b):
    return not isinstance(a, NaN) and not isinstance(b, NaN) and a < b


def add_f32(a_bits, b_bits):
    return store(add(dec(a_bits), dec(b_bits)))


def sub_f32(a_bits, b_bits):
    return store(sub(dec(a_bits), dec(b_bits)))


C001 = dec(0x3C23D70A)      # 0.01f
C00001 = dec(0x38D1B717)    # 0.0001f
C05 = dec(0x3F000000)       # 0.5f


# ---------------------------------------------------------------------------------------------- constantes del EXE
C_0_4 = dec(0x3ECCCCCD)
K_INV255 = 0x3B808081
COLOR_TABLE = [0] * 256


def set_color_table(table):
    global COLOR_TABLE
    COLOR_TABLE = list(table)


(K_FPV, K_FPP, K_FPC, K_FPW, K_IN3D, K_OUT, K_STATIC, K_EMPTY, K_TOUCH, K_REMOVECARS, K_STOREHIDE, K_SLIDEOPEN, K_SLIDECLOSED, K_ISOUTSIDE, K_BLOCKING, K_COUNTCENTER, K_OTHERPED, K_OTHERCAR, K_THROW,
 K_CALCDIST, K_RESTOREHIDE, K_NEATLY, K_RESTOREIMP, K_STOREIMP, K_FINDDOORS, K_MSG, K_FINDSAFE, K_COUNTHIDE, K_SPRAYABLE, K_DETONATOR, K_CALLOFF, K_INCSTAT, K_GETSTAT, K_SETSTAT, K_AUDIO,
 K_REQMODEL, K_HELP, K_TEXTGET, K_CLEARWANTED, K_GETREMAP, K_SETREMAP, K_APPEARANCE, K_DESTROY, K_CHOOSE, K_ADDPARTICLE, K_RANDF, K_FIX, K_RAND, K_KINDS) = range(1, 50)
FFFFFFFF = 0xFFFFFFFF


def center_dist_sq(gb, x1, x2, y3, y4):
    """Garage63CenterDistSqCpp: gb = [izq, der, frente, fondo, z, topZ] (bits); devuelve el valor exacto del x87 (24 bits por operacion)"""
    L, R, F, B = dec(gb[0]), dec(gb[1]), dec(gb[2]), dec(gb[3])
    half_y = mul(add(F, B), Fraction(1, 2))
    dy1 = dec(store(sub(dec(y3), half_y)))        # fstp dword
    dy2 = sub(dec(y4), half_y)
    t = mul(dy2, dy1)
    half_x = mul(add(R, L), Fraction(1, 2))
    dx2 = sub(dec(x2), half_x)
    dx1 = sub(dec(x1), half_x)
    p = mul(dx2, dx1)
    return add(t, p)


def door_dist_sq(d0x, v0x, d1x, v1x, d2y, v2y, d3y, v3y):
    a = sub(dec(d3y), dec(v3y))
    b = sub(dec(d2y), dec(v2y))
    t = mul(a, b)
    dx1 = sub(dec(d1x), dec(v1x))
    dx0 = sub(dec(d0x), dec(v0x))
    p = mul(dx1, dx0)
    return add(t, p)


def strict_rect(gb, r_bits, pos):
    L, R, F, B = dec(gb[0]), dec(gb[1]), dec(gb[2]), dec(gb[3])
    r = dec(r_bits)
    p = [dec(b) for b in pos]
    return lt(sub(L, r), p[0]) and gt(add(r, R), p[0]) and lt(sub(F, r), p[1]) and gt(add(r, B), p[1])


def gt_c(v, f):
    return gt(v, dec(f)) if not isinstance(v, int) else gt(dec(v), dec(f))


def f32(x):
    return float(x)


def i16(x):
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


def u32(x):
    return x & 0xFFFFFFFF


def bf(x):
    """bits de un float python (valores exactos de las constantes)"""
    import struct
    return struct.unpack('<I', struct.pack('<f', x))[0]


class Done(Exception):
    pass


class Model:
    def __init__(self, inp):
        self.inp = inp
        self.ev = []
        self.cnt = {}
        self.t, self.st, self.fl, self.door, self.time = inp['g']
        self.target = inp['target']          # indice o 0xFF
        self.gb = inp['gb']
        self.gid = inp['gid']
        self.timer = inp['timer']
        self.frame = inp['frame']
        self.mask = inp['mask']
        # estado real que Update lee / escribe
        self.glob = {'money': inp['money'], 'noResprays': inp['glob'][0], 'respraysFree': inp['glob'][1], 'bombsFree': inp['glob'][2], 'lastGarage': inp['glob'][3], 'lastHelp': inp['glob'][4],
                     'camOutside': inp['glob'][5], 'camIn': inp['glob'][6], 'camOut': inp['glob'][7], 'bombShop': inp['glob'][8], 'padW': inp['pad'][0], 'padB': inp['pad'][1]}
        self.glob0 = dict(self.glob)
        self.veh = [dict(v) for v in inp['veh']]
        self.veh0 = [dict(v) for v in inp['veh']]
        self.mat = [{'right': list(v['mright']), 'up': list(v['mup'])} for v in inp['veh']]
        self.wbyte = inp['wanted'][0]
        self.wbyte0 = self.wbyte
        self.hp = [v['hp'] for v in inp['veh']]
        self.f4b0 = [v['f4B0'] for v in inp['veh']]
        self.f8e4 = [v['f8E4'] for v in inp['veh']]
        self.f7bc = [v['f7BC'] for v in inp['veh']]
        self.f42e = [v['f42E'] for v in inp['veh']]
        self.f42f = [v['f42F'] for v in inp['veh']]
        self.col = [list(v['col']) for v in inp['veh']]
        self.f4a8 = [v['f4A8'] for v in inp['veh']]
        self.f4cc = [v['f4CC'] for v in inp['veh']]
        self.stats = {}

    # ---- utilidades de los falsos
    def k(self, kind):
        c = self.cnt.get(kind, 0)
        self.cnt[kind] = c + 1
        return c

    def e(self, *a):
        self.ev.append(list(a))

    def res(self, kind):
        return (self.mask[kind] >> (self.k(kind) & 31)) & 1

    def vtok(self, i):
        return 'V%d' % i

    # ---- jugador y mundo
    def fpv(self):
        self.e(K_FPV, FFFFFFFF, 0)
        self.k(K_FPV)
        return self.inp['pv'][1] if self.inp['pv'][0] else None

    def fpp(self):
        self.e(K_FPP, FFFFFFFF)
        self.k(K_FPP)
        return None if self.inp['pv'][2] else 'PED'

    def fpc(self):
        self.e(K_FPC, FFFFFFFF)
        return self.inp['coord'][self.k(K_FPC) & 7]

    def fpw(self):
        self.e(K_FPW, FFFFFFFF)
        self.k(K_FPW)

    def pos_of(self, i):
        v = self.veh[i]
        return v['mpos'] if v['mat'] else v['pos']

    # ---- colaboradores del garaje
    def in3d(self, ent, rbits):
        self.e(K_IN3D, 'G', self.vtok(ent) if isinstance(ent, int) else ent, rbits)
        return self.res(K_IN3D)

    def out(self, ent, rbits):
        self.e(K_OUT, 'G', self.vtok(ent) if isinstance(ent, int) else ent, rbits)
        return self.res(K_OUT)

    def simple(self, kind):
        self.e(kind, 'G')
        return self.res(kind)

    def withv(self, kind, veh):
        self.e(kind, 'G', 0 if veh is None else (self.vtok(veh) if isinstance(veh, int) else veh))
        return self.res(kind)

    def msg(self, key):
        self.e(K_MSG, key, FFFFFFFF, 0xFA0, FFFFFFFF)
        self.k(K_MSG)

    def audio(self, event):
        self.e(K_AUDIO, event, 0, 0x3F800000)
        self.k(K_AUDIO)

    def stored(self, typ):
        self.e(K_FINDSAFE, typ)
        self.k(K_FINDSAFE)
        return 0x96ABD8 + ((typ * 5 + 1) % 17) * 0x100

    def pad_bit(self, setit):
        b = self.glob['padB']
        self.glob['padB'] = (b | 4) if setit else (b & 0xFB)

    def wanted_bit(self, setit):
        self.fpw()
        self.wbyte = (self.wbyte | 2) if setit else (self.wbyte & 0xFD)

    def money_set(self, delta):
        m = self.glob['money']
        m = m - 2 ** 32 if m & 0x80000000 else m
        if m > 0:
            m -= delta
            if m < 0:
                m = 0
        self.glob['money'] = u32(m)

    def money(self):
        m = self.glob['money']
        return m - 2 ** 32 if m & 0x80000000 else m

    # ---- Update
    def run(self):
        try:
            label = 'START'
            guard = 0
            while True:
                guard += 1
                if guard > 200:
                    raise RuntimeError('bucle de etiquetas')
                self.stats['label_' + label] = True
                label = getattr(self, 'L_' + label)()
        except Done:
            pass

    def end(self):
        raise Done()

    def L_START(self):
        t = self.t
        if t == 13 or self.st > 5 or self.fpp() is None or (self.fl & 0x20):
            return 'ABB6'
        vehicle = self.fpv()
        subject = self.fpp()
        self.stats['bookkeeping'] = True
        subject_is_ped = True
        self.fpp()                                  # lectura de +0x46C (tercera llamada)
        ped_flags = self.inp['pv'][3]
        if ped_flags & 0x100:
            self.fpp()
            ped_veh = self.inp['pv'][4]
            if ped_veh < 8:
                self.fpp()
                if self.veh[ped_veh]['model'] == 0x23B:
                    self.fpp()
                    subject = ped_veh
                    subject_is_ped = False
                    self.stats['subject_vehicle'] = True
        if self.in3d(subject if subject != 'PED' else 'PED', 0x3E800000):
            self.glob['camOutside'] = 1
            self.glob['camIn'] = 'G'
        if vehicle is not None:
            if not self.out(vehicle, 0):
                self.glob['camOut'] = 'G'
            if self.veh[vehicle]['model'] == 0x1A7:
                self.stats['model_1A7'] = True
                if strict_rect(self.gb, 0x3F000000, self.pos_of(vehicle)):
                    self.stats['rect_hit'] = True
                    self.glob['camOutside'] = 1
                    self.glob['camIn'] = 'G'
        return 'ABB6'

    def L_ABB6(self):
        if (self.fl & 2) and self.st == 0:
            self.end()
        if self.fl & 8:
            if (self.st == 3 and gt(dec(self.door), C_0_4)) or self.st == 1:
                self.fl &= 0xBF
            else:
                self.fl |= 0x40
        idx = (self.t - 1) & 0xFFFFFFFF
        if idx > 0x2C:
            self.end()
        case = [0, 1, 1, 1, 2, 13, 13, 13, 13, 13, 3, 13, 13, 4, 5, 6, 6, 6, 7, 8, 4, 13, 9, 6, 6, 6, 6, 6, 6, 6, 6, 6, 10, 10, 10, 11, 11, 11, 6, 6, 6, 6, 12, 6, 6][idx]
        return {0: 'B7EE', 1: 'B419', 2: 'AC16', 3: 'B9FF', 4: 'BA5C', 5: 'BCA4', 6: 'BCDE', 7: 'BCAA', 8: 'C1FF', 9: 'C2F3', 10: 'C395', 11: 'C564', 12: 'C70A', 13: 'C7C4'}[case]

    def L_C7C4(self):
        self.end()

    def table(self, entries):
        return entries.get(self.st, 'C7C4')

    # ---- tipo 5
    def L_AC16(self):
        c = self.fpc()
        if not lt(dec(c[2]), dec(bf(950.0))):
            self.end()
        return self.table({0: 'AEC0', 1: 'AC50', 2: 'AE0C', 3: 'B79C', 4: 'B40C'})

    def L_AC50(self):
        if self.glob['noResprays']:
            self.end()
        if self.simple(K_STATIC):
            v = self.fpv()
            self.e(K_SPRAYABLE, 0 if v is None else self.vtok(v))
            if self.res(K_SPRAYABLE):
                if self.money() < 100 and not self.glob['respraysFree']:
                    self.msg(0x859A58)
                    self.st = 4
                    self.audio(0xE)
                    return 'AD2D'
                self.st = 2
                self.pad_bit(True)
                v = self.fpv()
                self.veh[v]['f4B0'] = 0
                self.f4b0[v] = 0
                return 'AD2D'
            v = self.fpv()
            self.msg(0x859A50 if self.veh[v]['t594'] == 0xA else 0x859A48)
            self.st = 4
            self.audio(0xF)
            return 'AD2D'
        self.e(K_ISOUTSIDE, 'G', 0)
        if self.res(K_ISOUTSIDE):
            if (self.gid & FFFFFFFF) == (self.glob['lastGarage'] & FFFFFFFF):
                self.wanted_bit(False)
            return 'ADA3'
        self.wanted_bit(True)
        self.glob['lastGarage'] = self.gid
        return 'ADA3'

    def L_AD2D(self):
        self.wanted_bit(True)
        self.glob['lastGarage'] = self.gid
        return 'ADA3'

    def L_ADA3(self):
        if self.fpv() is None:
            self.end()
        va = self.fpv()
        pa = self.pos_of(va)
        vb = self.fpv()
        pb = self.pos_of(vb)
        self.e(K_CALCDIST, 'G', pb[0], pa[1])
        d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        if not lt(d, dec(bf(64.0))):
            self.end()
        return 'B3C1'

    def L_AE0C(self):
        if self.fpv() is not None:
            v = self.fpv()
            self.e(K_THROW, 'G', self.vtok(v))
            self.k(K_THROW)
        if self.simple(K_SLIDECLOSED):
            self.st = 0
            self.audio(0x10)
            self.time = u32(self.timer + 0x7D0)
            self.e(K_GETSTAT, 0xB0)
            self.k(K_GETSTAT)
            self.e(K_INCSTAT, 0xB1, self.inp['stat'])
            self.k(K_INCSTAT)
            self.e(K_SETSTAT, 0xB0, 0)
            self.k(K_SETSTAT)
        if self.fpv() is None:
            return 'B3C1'
        v = self.fpv()
        self.f8e4[v] = 0
        v = self.fpv()
        self.f42e[v] |= 0x80
        return 'B3C1'

    def L_AEC0(self):
        if self.glob['noResprays']:
            self.end()
        if not self.timer > self.time:
            return 'B3C1'
        self.st = 3
        self.fixed_final = False
        self.fpw()
        has_wanted = self.inp['wanted'][1] != 0
        if has_wanted:
            self.fixed_final = True
            self.fpw()
            self.e(K_CLEARWANTED, 'W')
            self.k(K_CLEARWANTED)
        self.hasw = has_wanted
        v = self.fpv()
        if v is None:
            return 'B2DA'
        v = self.fpv()
        if self.veh[v]['t590'] != 0:
            v = self.fpv()
            if self.veh[v]['t590'] != 9:
                return 'B2DA'
        v = self.fpv()
        if lt(dec(self.hp[v]), dec(bf(970.0))):
            self.fixed_final = True
        v = self.fpv()
        h = dec(self.hp[v])
        v = self.fpv()
        # 0x420800: devuelve 1000.0f solo si 1000.0f > salud; con NaN devuelve la salud
        self.hp[v] = bf(1000.0) if gt(dec(bf(1000.0)), h) else self.hp[v]
        v = self.fpv()
        if self.veh[v]['t590'] == 0:
            v = self.fpv()
            self.f8e4[v] = 0
        else:
            v = self.fpv()
            self.f7bc[v] = 0
        v = self.fpv()
        self.e(K_FIX, self.vtok(v))
        self.k(K_FIX)
        self.e(K_INCSTAT, 0x8A, bf(1.0))
        self.k(K_INCSTAT)
        v = self.fpv()
        if lt(dec(self.mat[v]['up'][2]), Fraction(0)):
            self.stats['flipped'] = True
            for comp in ('up0', 'up1', 'up2', 'r0', 'r1', 'r2'):
                a = self.fpv()
                b = self.fpv()
                arr, idx = (self.mat[a]['up'], int(comp[2])) if comp[0] == 'u' else (self.mat[a]['right'], int(comp[1]))
                val = arr[idx]
                tgt = self.mat[b]['up'] if comp[0] == 'u' else self.mat[b]['right']
                tgt[idx] = val ^ 0x80000000
        v = self.fpv()
        repainted = False
        vd = self.veh[v]
        if not (vd['f868'] & 2):
            self.e(K_GETREMAP, self.vtok(v))
            remap = self.inp['remap'][self.k(K_GETREMAP) % 2]
            if remap & 0x80000000:
                colors = self.inp['colors']
                self.e(K_CHOOSE, 1, 1)
                self.k(K_CHOOSE)
                cur = self.col[v]
                if cur[0] != colors[0] or cur[1] != colors[1] or cur[2] != colors[2] or cur[3] != colors[3]:
                    repainted = True
                self.col[v] = [colors[0] & 0xFF, colors[1] & 0xFF, colors[2] & 0xFF, colors[3] & 0xFF]
                self.e(K_SETREMAP, self.vtok(v), FFFFFFFF)
                self.k(K_SETREMAP)
                if repainted:
                    self.stats['particles'] = True
                    self.particles(v)
        self.f42e[v] &= 0x7F
        self.f4b0[v] = 0
        self.flag_bl = repainted
        return 'B2DE'

    def particles(self, v):
        c = COLOR_TABLE[self.col[v][0]]
        k = K_INV255
        # r,g,b = canal * (1/255) con un solo redondeo a float (fild / fmul / fstp dword)
        red = store(mul(Fraction(c & 0xFF), dec(k)))
        green = store(mul(Fraction((c >> 8) & 0xFF), dec(k)))
        blue = store(mul(Fraction((c >> 16) & 0xFF), dec(k)))
        mult = [red, green, blue, bf(0.6), bf(0.7), bf(1.0), bf(0.4)]
        # FxPrtMult_c{1.0f, 0, 0, 0.6f, 0.7f, 1.0f, 0.4f} y despues se sobrescriben rojo, verde y azul
        pos = self.pos_of(v)
        for _ in range(10):
            self.e(K_RANDF, bf(-3.0), bf(3.0))
            x = store(add(dec(self.inp['randf'][self.k(K_RANDF) % 16]), dec(pos[0])))
            self.e(K_RANDF, bf(-3.0), bf(3.0))
            y = store(add(dec(self.inp['randf'][self.k(K_RANDF) % 16]), dec(pos[1])))
            self.e(K_RANDF, 0, bf(0.05))
            vz = self.inp['randf'][self.k(K_RANDF) % 16]
            self.e(K_ADDPARTICLE, 1, x, y, pos[2], 0, 0, vz, 0, *mult, bf(-1.0), bf(1.2), bf(0.6), 0)
            self.k(K_ADDPARTICLE)

    def L_B2DA(self):
        self.flag_bl = self.hasw
        return 'B2DE'

    def L_B2DE(self):
        if self.fl & 0x80:
            self.msg(0x859A34)
            return 'B38D'
        if self.fixed_final and not self.glob['respraysFree']:
            self.money_set(100)
            self.e(K_INCSTAT, 0x10, bf(100.0))
            self.k(K_INCSTAT)
            self.msg(0x859A2C if self.hasw else 0x859A24)
            return 'B38D'
        if self.flag_bl:
            self.e(K_RAND)
            self.msg(0x859A1C if ((self.mask[K_RAND] >> (self.k(K_RAND) & 31)) & 1) else 0x859A14)
        return 'B38D'

    def L_B38D(self):
        self.fl |= 4
        v = self.fpv()
        if v is not None:
            v = self.fpv()
            self.f42f[v] |= 1
        return 'B3C1'

    def L_B3C1(self):
        gb = self.gb
        L, R, F, B = dec(gb[0]), dec(gb[1]), dec(gb[2]), dec(gb[3])
        ten = dec(bf(10.0))
        x1, y1 = store(sub(L, ten)), store(sub(F, ten))
        x2, y2 = store(add(R, ten)), store(add(B, ten))
        self.e(K_CALLOFF, x1, y1, x2, y2)
        self.k(K_CALLOFF)
        self.end()

    def L_B79C(self):
        if self.simple(K_SLIDEOPEN):
            self.st = 4
        if not gt(dec(self.door), dec(bf(0.5))):
            self.end()
        self.pad_bit(False)
        self.wanted_bit(False)
        self.end()

    def L_B40C(self):
        self.e(K_ISOUTSIDE, 'G', 0)
        if not self.res(K_ISOUTSIDE):
            self.end()
        self.st = 1
        self.end()

    # ---- tipos 2, 3 y 4
    def L_B419(self):
        return self.table({0: 'B5CA', 1: 'B42D', 2: 'B564', 3: 'B79C', 4: 'B40C'})

    def L_B42D(self):
        if not self.simple(K_STATIC):
            self.end()
        v = self.fpv()
        if v is None:
            self.end()
        v = self.fpv()
        if self.veh[v]['t594'] == 9:
            self.end()
        v = self.fpv()
        if self.veh[v]['t594'] == 0xA:
            self.end()
        v = self.fpv()
        if self.f4a8[v] & 7:
            self.msg(0x859A04)
            self.st = 4
            self.audio(0x11)
            self.end()
        if not self.glob['bombsFree'] and self.money() < 500:
            self.msg(0x859A0C)
            self.st = 4
            self.audio(0xE)
            self.end()
        self.st = 2
        self.pad_bit(True)
        self.wanted_bit(True)
        self.end()

    def L_B564(self):
        if self.fpv() is not None:
            v = self.fpv()
            self.e(K_THROW, 'G', self.vtok(v))
            self.k(K_THROW)
        if self.simple(K_SLIDECLOSED):
            self.st = 0
            self.time = u32(self.timer + 0x7D0)
        if self.t != 4:
            self.end()
        return 'B5AE'

    def L_B5AE(self):
        self.e(K_REQMODEL, 0x16C, 2)
        self.k(K_REQMODEL)
        self.end()

    def L_B5CA(self):
        if not self.timer > self.time:
            self.end()
        if self.t == 4 and self.glob['bombShop'] != 1:
            return 'B5AE'
        if self.t == 2:
            self.audio(0x12)
        elif self.t == 3:
            self.audio(0x13)
        elif self.t == 4:
            self.audio(0x14)
        self.st = 3
        if not self.glob['bombsFree']:
            self.money_set(500)
        v = self.fpv()
        if v is not None:
            v = self.fpv()
            ok = self.veh[v]['t590'] == 0
            if not ok:
                v = self.fpv()
                ok = self.veh[v]['t590'] == 9
            if ok:
                v = self.fpv()
                bomb = self.f4a8[v]
                self.f4a8[v] = (bomb ^ (((self.t - 1) ^ bomb) & 7)) & 0xFF
                owner = self.fpp()
                v = self.fpv()
                self.f4cc[v] = 0 if owner is None else 'PED'
                if self.t == 4:
                    self.e(K_DETONATOR)
                    self.k(K_DETONATOR)
                self.e(K_INCSTAT, 0x7F, bf(10.0))
                self.k(K_INCSTAT)
        if self.t in (2, 3):
            b = self.inp['pad'][0]
            if b <= 2:
                key = 0x8599DC if self.t == 2 else 0x8599EC
            elif b == 3:
                key = 0x8599E4 if self.t == 2 else 0x8599F4
            else:
                self.end()
        elif self.t == 4:
            key = 0x8599FC
        else:
            self.end()
        self.e(K_TEXTGET, key)
        self.k(K_TEXTGET)
        self.e(K_HELP, key + 0x1000, 0, 0, 1)
        self.k(K_HELP)
        self.end()

    # ---- tipo 1
    def L_B7EE(self):
        return self.table({0: 'B9AB', 1: 'B802', 2: 'B963', 3: 'BCBE'})

    def coords4(self):
        c1 = self.fpc()
        c2 = self.fpc()
        c3 = self.fpc()
        c4 = self.fpc()
        return gt(center_dist_sq(self.gb, c1[0], c2[0], c3[1], c4[1]), Fraction(900))

    def L_B802(self):
        far = self.coords4()
        if not far:
            return 'B8BE'
        if self.frame & 0x1F:
            self.end()
        if self.target == 0xFF:
            return 'C662'
        self.e(K_TOUCH, 'G', self.vtok(self.target))
        if self.res(K_TOUCH):
            self.end()
        return 'C662'

    def L_B8BE(self):
        target = self.target
        v = self.fpv()
        if v == (None if target == 0xFF else target):
            self.end()
        if target == 0xFF:
            self.end()
        if not self.in3d(target, 0):
            self.end()
        v = self.fpv()
        if v is not None:
            v = self.fpv()
            who = self.vtok(v)
        else:
            self.fpp()
            who = 'PED' if not self.inp['pv'][2] else 0
        if not self.out(who, bf(2.0)):
            self.end()
        self.pad_bit(True)
        self.wanted_bit(True)
        self.fl &= 0xFE
        self.st = 2
        self.end()

    def L_B963(self):
        if self.target != 0xFF:
            self.e(K_THROW, 'G', self.vtok(self.target))
            self.k(K_THROW)
        if not self.simple(K_SLIDECLOSED):
            self.end()
        if self.fl & 1:
            return 'C6AC'
        if self.target == 0xFF:
            return 'BBC8'
        self.st = 5
        self.e(K_DESTROY, self.vtok(self.target))
        self.k(K_DESTROY)
        self.target = 0xFF
        return 'BBCC'

    def L_B9AB(self):
        v = self.fpv()
        tgt = None if self.target == 0xFF else self.target
        if v != tgt:
            self.end()
        if tgt is None:
            self.end()
        return 'B9C9'

    def L_B9C9(self):
        va = self.fpv()
        pa = self.pos_of(va)
        vb = self.fpv()
        pb = self.pos_of(vb)
        self.e(K_CALCDIST, 'G', pb[0], pa[1])
        d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        if not lt(d, dec(bf(64.0))):
            self.end()
        self.st = 3
        self.end()

    # ---- tipo 11
    def L_B9FF(self):
        if self.st == 1:
            return 'BA3C'
        if self.st == 2:
            return 'BA0D'
        if self.st == 3:
            return 'BCBE'
        self.end()

    def L_BA0D(self):
        if self.simple(K_SLIDECLOSED):
            self.st = 0
        if self.simple(K_EMPTY):
            self.end()
        self.st = 3
        self.end()

    def L_BA3C(self):
        if not self.simple(K_EMPTY):
            self.end()
        self.st = 2
        self.end()

    # ---- tipos 14 y 21
    def L_BA5C(self):
        return self.table({0: 'BBFB', 1: 'BA70', 2: 'BB83', 3: 'BCBE', 5: 'BC7B'})

    def L_BA70(self):
        if self.coords4():
            return 'C662'
        if self.target == 0xFF:
            return 'C662'
        v = self.fpv()
        if self.target != v:
            self.end()
        if not self.simple(K_STATIC):
            self.end()
        if self.simple(K_BLOCKING):
            self.end()
        self.pad_bit(True)
        self.wanted_bit(True)
        self.fl &= 0xFE
        self.st = 2
        self.end()

    def L_BB83(self):
        if self.target != 0xFF:
            self.e(K_THROW, 'G', self.vtok(self.target))
            self.k(K_THROW)
        if not self.simple(K_SLIDECLOSED):
            self.end()
        if self.fl & 1:
            return 'C6AC'
        if self.target == 0xFF:
            return 'BBC8'
        self.st = 5
        self.time = u32(self.timer + 0x7D0)
        self.target = 0xFF
        return 'BBCC'

    def L_BBC8(self):
        self.st = 0
        return 'BBCC'

    def L_BBCC(self):
        self.pad_bit(False)
        self.wanted_bit(False)
        self.end()

    def L_BBFB(self):
        v = self.fpv()
        tgt = None if self.target == 0xFF else self.target
        if v != tgt:
            self.end()
        if tgt is None:
            self.end()
        va = self.fpv()
        pa = self.pos_of(va)
        vb = self.fpv()
        pb = self.pos_of(vb)
        self.e(K_CALCDIST, 'G', pb[0], pa[1])
        d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        if not lt(d, dec(bf(289.0))):
            self.end()
        self.st = 3
        self.end()

    def L_BC7B(self):
        if self.t != 14:
            self.end()
        if not self.timer > self.time:
            self.end()
        self.st = 3
        self.end()

    # ---- scripts
    def L_BCA4(self):
        if self.st != 3:
            self.end()
        return 'BCBE'

    def L_BCAA(self):
        if self.st == 2:
            return 'C69D'
        if self.st != 3:
            self.end()
        return 'BCBE'

    def L_BCBE(self):
        if not self.simple(K_SLIDEOPEN):
            self.end()
        self.st = 1
        self.end()

    # ---- refugios
    def L_BCDE(self):
        return self.table({0: 'BE56', 1: 'BCF2', 2: 'BDF1', 3: 'BCBE'})

    def L_BCF2(self):
        c1 = self.fpc()
        c2 = self.fpc()
        self.e(K_CALCDIST, 'G', c1[0], c2[1])
        self.d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        d = self.d
        if gt(d, dec(bf(225.0))):
            if not self.simple(K_BLOCKING):
                return 'C7C0'
            return 'BD79'
        if not gt(d, dec(bf(16.0))):
            return 'BD79'
        v = self.fpv()
        if v is not None:
            v = self.fpv()
            if self.veh[v]['t594'] != 0xA:
                return 'BD79'
        if not self.simple(K_BLOCKING):
            return 'C7C0'
        return 'BD79'

    def L_BD79(self):
        if self.fpv() is not None:
            needed = (1 if self.t != 0x10 else 0) * 2 + 2
            v = self.fpv()
            self.e(K_COUNTCENTER, 'G', self.vtok(v))
            if self.inp['cc'][self.k(K_COUNTCENTER) % 3] >= needed:
                self.e(K_ISOUTSIDE, 'G', bf(0.25))
                if self.res(K_ISOUTSIDE):
                    return 'C7C0'
        if not gt(self.d, dec(bf(4900.0))):
            self.end()
        self.st = 2
        self.e(K_REMOVECARS, 'G')
        self.k(K_REMOVECARS)
        self.end()

    def L_BDF1(self):
        self.simple(K_SLIDECLOSED)
        self.e(K_ISOUTSIDE, 'G', 0)
        if not self.res(K_ISOUTSIDE):
            return 'C6F9'
        dp = dec(self.door)
        if isinstance(dp, NaN) or dp != 0:
            self.end()
        self.st = 0
        if self.t == 0x2C:
            self.end()
        cars = self.stored(self.t)
        self.e(K_STOREHIDE, 'G', cars, 4)
        self.k(K_STOREHIDE)
        self.end()

    def L_BE56(self):
        c1 = self.fpc()
        if not lt(dec(c1[2]), dec(bf(950.0))):
            self.end()
        c2 = self.fpc()
        c3 = self.fpc()
        self.e(K_CALCDIST, 'G', c2[0], c3[1])
        d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        if not lt(d, dec(bf(12.25))):
            if not lt(d, dec(bf(100.0))):
                self.end()
            v = self.fpv()
            if v is None:
                self.end()
            v = self.fpv()
            if self.veh[v]['t594'] == 0xA:
                self.end()
        if self.fpv() is None:
            return 'C1C3'
        if self.t == 0x2C:
            return 'C1C3'
        needed = (1 if self.t != 0x10 else 0) * 2 + 2
        self.e(K_COUNTHIDE, self.t)
        if self.inp['ch'][self.k(K_COUNTHIDE) % 2] < needed:
            return 'C1C3'
        self.e(K_FINDDOORS, 'G', 'L', 'L')
        self.k(K_FINDDOORS)
        d1, d2 = self.inp['doors']
        if d1 != 0xFF:
            if self.door_near(d1):
                return 'C13E'
        if d2 == 0xFF:
            self.end()
        if not self.door_near(d2):
            self.end()
        return 'C13E'

    def door_near(self, d):
        pts = []
        for i in range(4):
            dp = self.pos_of(d)
            v = self.fpv()
            vp = self.pos_of(v)
            pts.append((dp, vp))
        (d0, v0), (d1, v1), (d2, v2), (d3, v3) = pts
        s = door_dist_sq(d0[0], v0[0], d1[0], v1[0], d2[1], v2[1], d3[1], v3[1])
        return lt(s, Fraction(25))

    def L_C13E(self):
        if not u32(self.timer - self.glob['lastHelp']) > 0x4650:
            self.end()
        v = self.fpv()
        self.e(K_APPEARANCE, self.vtok(v))
        if self.inp['appear'][self.k(K_APPEARANCE) % 4] == 3:
            self.end()
        v = self.fpv()
        self.e(K_APPEARANCE, self.vtok(v))
        if self.inp['appear'][self.k(K_APPEARANCE) % 4] == 5:
            self.end()
        self.stats['help_printed'] = True
        self.e(K_TEXTGET, 0x8599C0)
        self.k(K_TEXTGET)
        self.e(K_HELP, 0x8599C0 + 0x1000, 0, 0, 1)
        self.k(K_HELP)
        self.glob['lastHelp'] = self.timer
        self.end()

    def L_C1C3(self):
        if self.t != 0x2C:
            cars = self.stored(self.t)
            self.e(K_RESTOREHIDE, 'G', cars)
            if not self.res(K_RESTOREHIDE):
                self.end()
        self.st = 3
        self.end()

    # ---- tipo 20
    def L_C1FF(self):
        return self.table({0: 'B9AB', 1: 'C213', 2: 'C2E3', 3: 'BCBE'})

    def L_C213(self):
        if not self.coords4():
            self.end()
        if self.target == 0xFF:
            self.end()
        if not self.out(self.target, 0):
            self.end()
        self.fl |= 1
        self.st = 2
        self.end()

    def L_C2E3(self):
        if self.target != 0xFF:
            self.e(K_THROW, 'G', self.vtok(self.target))
            self.k(K_THROW)
        return 'C69D'

    # ---- tipo 23
    def L_C2F3(self):
        if self.st == 1:
            return 'C331'
        if self.st != 2:
            if self.st != 3:
                self.end()
            return 'BCBE'
        if not self.simple(K_SLIDECLOSED):
            self.end()
        self.st = 0
        self.pad_bit(False)
        self.end()

    def L_C331(self):
        if self.target == 0xFF:
            self.end()
        if not self.in3d(self.target, 0):
            self.end()
        if self.simple(K_BLOCKING):
            self.end()
        self.e(K_ISOUTSIDE, 'G', 0)
        if not self.res(K_ISOUTSIDE):
            self.end()
        self.pad_bit(True)
        self.fl &= 0xFE
        self.st = 2
        self.end()

    # ---- embargo
    def impound_flag(self):
        c1 = self.fpc()
        c2 = self.fpc()
        self.e(K_CALCDIST, 'G', c1[0], c2[1])
        d = dec(self.inp['dist'][self.k(K_CALCDIST) % 6])
        c3 = self.fpc()
        bl = False
        topz, posz = dec(self.gb[5]), dec(self.gb[4])
        if gt(sub(topz, Fraction(2)), dec(c3[2])):
            c4 = self.fpc()
            if gt(dec(c4[2]), dec(self.gb[4])):
                bl = True
        return d, bl

    def L_C395(self):
        if self.st == 0:
            return 'C477'
        if self.st > 2:
            self.end()
        d, bl = self.impound_flag()
        if not gt(d, dec(bf(4225.0))) and bl:
            if self.st != 2:
                self.end()
        self.st = 0
        cars = self.stored(self.t)
        self.e(K_STOREIMP, 'G', cars, 3)
        self.k(K_STOREIMP)
        self.end()

    def L_C477(self):
        d, bl = self.impound_flag()
        if not lt(d, dec(bf(3600.0))):
            self.end()
        if not bl:
            self.end()
        cars = self.stored(self.t)
        self.e(K_NEATLY, 'G', cars)
        self.k(K_NEATLY)
        cars = self.stored(self.t)
        self.e(K_RESTOREIMP, 'G', cars)
        if not self.res(K_RESTOREIMP):
            self.end()
        self.st = 1
        self.end()

    # ---- tuning
    def L_C564(self):
        return self.table({0: 'C6BD', 1: 'C578', 2: 'C67B', 3: 'BCBE'})

    def L_C578(self):
        if not self.coords4():
            self.end()
        if self.frame & 0x1F:
            self.end()
        v = self.fpv()
        self.e(K_OTHERCAR, 'G', 0 if v is None else self.vtok(v))
        if self.res(K_OTHERCAR):
            v = self.fpv()
            self.e(K_TOUCH, 'G', 0 if v is None else self.vtok(v))
            if self.res(K_TOUCH):
                self.end()
        self.e(K_OTHERPED, 'G', 0)
        if self.res(K_OTHERPED):
            self.end()
        return 'C662'

    def L_C67B(self):
        if self.fpv() is not None:
            v = self.fpv()
            self.e(K_THROW, 'G', self.vtok(v))
            self.k(K_THROW)
        return 'C69D'

    def L_C6BD(self):
        v = self.fpv()
        self.e(K_OTHERCAR, 'G', 0 if v is None else self.vtok(v))
        if not self.res(K_OTHERCAR):
            self.end()
        return 'B9C9'

    # ---- burlaria
    def L_C70A(self):
        if self.st == 1:
            return 'C71D'
        if self.st == 2:
            return 'C67B'
        if self.st != 3:
            self.end()
        return 'BCBE'

    def L_C71D(self):
        if not self.coords4():
            self.end()
        self.e(K_OTHERPED, 'G', 0)
        if self.res(K_OTHERPED):
            self.end()
        return 'C7C0'

    # ---- colas
    def L_C662(self):
        self.fl |= 1
        self.st = 2
        self.end()

    def L_C69D(self):
        if not self.simple(K_SLIDECLOSED):
            self.end()
        return 'C6AC'

    def L_C6AC(self):
        self.st = 0
        self.end()

    def L_C6F9(self):
        self.st = 3
        self.end()

    def L_C7C0(self):
        self.st = 2
        self.end()


def expected(name, inp, case):
    m = Model(inp)
    m.hasw = False
    m.fixed_final = False
    m.flag_bl = False
    m.d = None
    m.run()
    observed, stats = finish(m, inp)
    stats.update(m.stats)
    return observed, stats


def finish(m, inp):
    changes = []
    # garaje: palabras 15 (tiempo), 16 (objetivo) y 19 (tipo/estado/banderas/relleno 0xA5)
    time0 = inp['g'][4]
    if m.time != time0:
        changes.append(['G', 0, 15, m.time])
    tgt0 = inp['target']
    tgt_new = m.target
    if tgt_new != tgt0:
        changes.append(['G', 0, 16, 0 if tgt_new == 0xFF else 'V%d' % tgt_new])
    w19 = (inp['g'][0] & 0xFF) | ((m.st & 0xFF) << 8) | ((m.fl & 0xFF) << 16) | (0xA5 << 24)
    w19_0 = (inp['g'][0] & 0xFF) | ((inp['g'][1] & 0xFF) << 8) | ((inp['g'][2] & 0xFF) << 16) | (0xA5 << 24)
    if w19 != w19_0:
        changes.append(['G', 0, 19, w19])
    for i, v in enumerate(inp['veh']):
        words = {}
        if m.f4b0[i] != v['f4B0']:
            words[0x4B0 // 4] = m.f4b0[i]
        if m.hp[i] != v['hp']:
            words[0x4C0 // 4] = m.hp[i]
        if m.f4cc[i] != v['f4CC']:
            words[0x4CC // 4] = m.f4cc[i]
        if m.f7bc[i] != v['f7BC']:
            words[0x7BC // 4] = m.f7bc[i]
        if m.f8e4[i] != v['f8E4']:
            words[0x8E4 // 4] = m.f8e4[i]
        if m.f42e[i] != v['f42E'] or m.f42f[i] != v['f42F']:
            words[0x42C // 4] = (m.f42e[i] << 16) | (m.f42f[i] << 24)
        if m.col[i] != v['col']:
            words[0x434 // 4] = m.col[i][0] | (m.col[i][1] << 8) | (m.col[i][2] << 16) | (m.col[i][3] << 24)
        if m.f4a8[i] != v['f4A8']:
            words[0x4A8 // 4] = m.f4a8[i]
        for w, val in words.items():
            orig = {0x4B0 // 4: v['f4B0'], 0x4C0 // 4: v['hp'], 0x4CC // 4: v['f4CC'], 0x7BC // 4: v['f7BC'], 0x8E4 // 4: v['f8E4'], 0x42C // 4: (v['f42E'] << 16) | (v['f42F'] << 24),
                    0x434 // 4: v['col'][0] | (v['col'][1] << 8) | (v['col'][2] << 16) | (v['col'][3] << 24), 0x4A8 // 4: v['f4A8']}[w]
            if val != orig:
                changes.append(['V', i, w, val])
        for w, (arr, idx0) in {0: ('right', 0), 1: ('right', 1), 2: ('right', 2), 8: ('up', 0), 9: ('up', 1), 10: ('up', 2)}.items():
            a0 = (v['mright'] if arr == 'right' else v['mup'])[idx0]
            a1 = m.mat[i][arr][idx0]
            if a0 != a1:
                changes.append(['M', i, w, a1])
    if m.wbyte != m.wbyte0:
        # +0x1E: byte dentro de la palabra 7 del bloque del CWanted (el resto de la palabra es 0)
        changes.append(['W', 0, 7, (m.wbyte << 16) | (0 << 0)])
    changes.sort(key=lambda c: ('GVMWX'.index(c[0]), c[1], c[2]))   # mismo orden que Core.hpp: garaje, vehiculos, matrices, CWanted, resto
    glob = []
    order = ['money', 'noResprays', 'respraysFree', 'bombsFree', 'lastGarage', 'lastHelp', 'camOutside', 'camIn', 'camOut', 'bombShop', 'padW', 'padB']
    # el orden y los nombres de Core.hpp (kRealName): timer, frame, focus, money, noResprays, respraysFree, bombsFree, lastGarage, lastHelp, camOutside, camIn, camOut, bombShop, padW, padB
    for name in order:
        if m.glob[name] != m.glob0[name]:
            glob.append([name, m.glob[name]])
    observed = {'events': m.ev[:600], 'nevents': len(m.ev), 'changes': changes, 'globals': glob}
    return observed, {}


def selftest():
    assert enc(dec(0x3F800000)) == 0x3F800000
    gb = [bf(0.0), bf(10.0), bf(0.0), bf(10.0), bf(0.0), bf(5.0)]
    # centro (5,5): jugador en (5,5) -> distancia 0; en (35,5) -> 900 exactamente (no mayor que 900)
    assert center_dist_sq(gb, bf(5.0), bf(5.0), bf(5.0), bf(5.0)) == 0
    assert center_dist_sq(gb, bf(35.0), bf(35.0), bf(5.0), bf(5.0)) == 900
    assert door_dist_sq(bf(3.0), bf(0.0), bf(3.0), bf(0.0), bf(4.0), bf(0.0), bf(4.0), bf(0.0)) == 25
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
