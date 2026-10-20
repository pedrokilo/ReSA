"""Oraculo independiente del banco Garage63 (lote 63): modela, DESDE EL DESENSAMBLADO del EXE (0x447600, 0x4486C0, 0x448BE0, 0x448D30, 0x44A830, 0x44A9C0, 0x449690, 0x449C50, 0x449D10),
la salida observable de cada funcion de CGarage para una fila `input` (la especificacion semantica del caso que registra Core.hpp), con las mismas fichas simbolicas de punteros que usa el banco:

  G garaje;  V<i> vehiculo i (V<i>+<campo> si apunta dentro);  M<i> su matriz;  CM<i> / CD<i> su CColModel / CCollisionData;  S<i>.<j> esfera j;  L local de pila;  PED / INT+4 jugador / TaskManager;  numero = valor crudo.

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos de los falsos EN ORDEN con sus argumentos, cambios de la arena,
AL en las funciones bool y ECX/EDX en PlayerArrestedOrDied); `stats` son banderas de cobertura. Aritmetica x87 del EXE: PC 24 bits, exponente extendido (se emula con fracciones exactas).
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
from fractions import Fraction
import math

NAMES = ['CGarage::InitDoorsAtStart', 'CGarage::PlayerArrestedOrDied', 'CGarage::IsEntityEntirelyInside3D', 'CGarage::IsEntityEntirelyOutside', 'CGarage::IsStaticPlayerCarEntirelyInside',
         'CGarage::IsGarageEmpty', 'CGarage::RemoveCarsBlockingDoorNotInside', 'CGarage::TidyUpGarage', 'CGarage::TidyUpGarageClose']
(N_INIT, N_ARRESTED, N_INSIDE3D, N_OUTSIDE, N_STATIC, N_EMPTY, N_REMOVE, N_TIDY, N_TIDYCLOSE) = NAMES
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
MAX_EVENTS = 400
K_COL, K_MAT, K_ALLOC, K_UPD, K_XFORM, K_IPR, K_IPP, K_TOUCH, K_FIND, K_FPV, K_FPP, K_TASK, K_DEL, K_REMOVE, K_DTOR, K_IN3D = range(1, 17)


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

# ---------------------------------------------------------------------------------------------- comparaciones de los helpers
def near(g, r, p):
    """GarageEntityNearCpp (mismas seis comprobaciones de IsEntityTouching3D / IsEntityEntirelyInside3D); g = [izq, der, frente, fondo, z, topZ] como valores"""
    L, R, F, B, Z, T = g
    if gt(sub(L, r), p[0]):
        return False
    if lt(add(r, R), p[0]):
        return False
    if gt(sub(F, r), p[1]):
        return False
    if lt(add(r, B), p[1]):
        return False
    if gt(sub(Z, r), p[2]):
        return False
    if lt(add(r, T), p[2]):
        return False
    return True


def strict(g, r, p):
    """Garage63InsideRectStrictCpp"""
    L, R, F, B, Z, T = g
    return lt(sub(L, r), p[0]) and gt(add(r, R), p[0]) and lt(sub(F, r), p[1]) and gt(add(r, B), p[1])


def abs_like(v):
    return -v if lt(v, Fraction(0)) else v


def static_speed(s):
    ax, ay, az = (abs_like(dec(b)) for b in s)
    for a in (ax, ay, az):
        if gt(a, C001):
            return False
    if any(isinstance(a, NaN) for a in (ax, ay, az)):
        return True          # los productos con NaN dan NaN: la comparacion no rechaza
    zy = r24(r24(az * az) + r24(ay * ay))
    total = r24(zy + r24(ax * ax))
    return not gt(total, C00001)


# ---------------------------------------------------------------------------------------------- modelo
def tf_bits(seed, k, i):
    return rand(seed, k * 4 + i + 1)


def i16(x):
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


class Model:
    def __init__(self, inp):
        self.inp = inp
        self.ev = []
        self.cnt = {}
        self.chg = {}       # (region, ...) -> valor
        self.nsph = [v['nsph'] & 0xFFFF for v in inp['veh']] + [0] * (8 - len(inp['veh']))
        self.matptr = [('M%d' % i) if v['mat'] else 0 for i, v in enumerate(inp['veh'])]
        self.mask = dict(zip(('touch', 'point', 'rad', 'del', 'in'), inp['masks']))
        self.reK, self.reKind, self.reIdx, self.reVal = inp['re']

    def nextk(self, kind):
        k = self.cnt.get(kind, 0)
        self.cnt[kind] = k + 1
        return k

    def emit(self, *a):
        self.ev.append(list(a))

    def bit(self, name, k):
        return (self.mask[name] >> (k & 31)) & 1

    def reenter(self, kind_name, k):
        if self.reKind == (1 if kind_name == 'xform' else 2) and k == self.reK:
            self.nsph[self.reIdx] = self.reVal & 0xFFFF

    # --- fakes
    def xform(self, ent, matok, sph):
        k = self.nextk('xform')
        self.emit(K_XFORM, 'L', matok, 'S%d.%d' % (ent, sph))
        out = [tf_bits(self.inp['seed'], k, i) for i in range(3)]
        self.reenter('xform', k)
        return out

    def ipr(self, pt, rbits):
        k = self.nextk('ipr')
        self.emit(K_IPR, 'G', pt[0], pt[1], pt[2], rbits)
        self.reenter('ipr', k)
        return self.bit('rad', k)

    def ipp(self, pos):
        k = self.nextk('ipp')
        self.emit(K_IPP, 'G', pos[0], pos[1], pos[2])
        return self.bit('point', k)

    def touch(self, ent):
        k = self.nextk('touch')
        self.emit(K_TOUCH, 'G', 'V%d' % ent)
        return self.bit('touch', k)


def entity_pos(v):
    return v['mpos'] if v['mat'] else v['pos']


def gvals(inp):
    gb = inp['gb']
    return [dec(gb[0]), dec(gb[1]), dec(gb[2]), dec(gb[3]), dec(gb[4]), dec(gb[5])]


def garage_words(inp):
    t, st, fl, door, tm = inp['g']
    return {'door': door, 'time': tm, 'tsf': (t & 0xFF) | ((st & 0xFF) << 8) | ((fl & 0xFF) << 16) | (0xA5 << 24)}


def garage_changes(inp, state_only=False):
    """cambios de las palabras 14 (puerta), 15 (tiempo) y 19 (tipo/estado/banderas) del garaje"""
    return None


def expected(name, inp, case):
    if name not in NAMES:
        raise KeyError(name)
    m = Model(inp)
    stats = {}
    t, st, fl, door, tm = inp['g']
    al = None
    ecx = edx = None
    changes = []
    n_events_total = None

    def garage_changes_from(new_door, new_time, new_tsf):
        w = garage_words(inp)
        ch = []
        if new_door != w['door']:
            ch.append(['G', 0, 14, new_door])
        if new_time != w['time']:
            ch.append(['G', 0, 15, new_time])
        if new_tsf != w['tsf']:
            ch.append(['G', 0, 19, new_tsf])
        return ch

    if name == N_INIT:
        flags = ((fl & 0x39) | 0x40) & 0xFF
        state = 0
        door_new = door
        idx = (t - 1) & M32
        if idx <= 0x2C:
            if 1 <= idx <= 4:
                state, door_new = 1, 0x3F800000
            elif idx != 12:
                state, door_new = 0, 0
        changes = garage_changes_from(door_new, 0, (t & 0xFF) | (state << 8) | (flags << 16) | (0xA5 << 24))
        stats = {'type_open': 1 <= idx <= 4, 'type_closed': idx <= 0x2C and not (1 <= idx <= 4) and idx != 12, 'type13': idx == 12, 'type_out': idx > 0x2C, 'type0': t == 0}
    elif name == N_ARRESTED:
        idx = (t - 1) & M32
        new_state = st
        if idx <= 0x2C:
            if 1 <= idx <= 4 or idx == 12:
                if st == 0 or 1 < st <= 3:
                    new_state = 3
            else:
                if st != 0 and st <= 3:
                    new_state = 2
        changes = garage_changes_from(door, tm, (t & 0xFF) | (new_state << 8) | (fl << 16) | (0xA5 << 24))
        ecx, edx = 'G', inp['regs'][3]
        stats = {'type_b': idx <= 0x2C and (1 <= idx <= 4 or idx == 12), 'type_a': idx <= 0x2C and not (1 <= idx <= 4 or idx == 12), 'type_out': idx > 0x2C, 'changed': new_state != st,
                 'state0': st == 0, 'state_hi': st > 3}
    elif name in (N_INSIDE3D, N_OUTSIDE):
        g = gvals(inp)
        r = dec(inp['radius'])
        v = inp['veh'][0]
        p = [dec(b) for b in entity_pos(v)]
        inside3d = name == N_INSIDE3D
        res = None
        if inside3d:
            if not near(g, r, p):
                res = 0
        else:
            if strict(g, r, p):
                res = 0
        stats['early'] = res is not None
        if res is None:
            m.emit(K_COL, 'V0')
            if not v['col']:
                res = 1
            elif i16(m.nsph[0]) <= 0:
                res = 1
            else:
                j = 0
                res = 1
                while True:
                    if inside3d:
                        m.emit(K_MAT, 'V0')
                        matok = 'M0'
                    else:
                        if not m.matptr[0]:
                            m.emit(K_ALLOC, 'V0')
                            m.matptr[0] = 'M0'
                            m.chg[('V', 0, 5)] = 'M0'
                            m.emit(K_UPD, 'V0+4', 'M0')
                        matok = m.matptr[0]
                    out = m.xform(0, matok, j)
                    sr = v['sphr'][j]
                    rb = sub_f32(inp['radius'], sr) if inside3d else add_f32(inp['radius'], sr)
                    ok = m.ipr(out, rb)
                    if inside3d and not ok:
                        res = 0
                        break
                    if not inside3d and ok:
                        res = 0
                        break
                    j += 1
                    if not j < i16(m.nsph[0]):
                        break
        al = res
        stats.update({'result1': res == 1, 'result0': res == 0, 'spheres': v['col'] and i16(inp['veh'][0]['nsph']) > 0, 'nsph_neg': i16(v['nsph']) < 0, 'nsph_zero': v['nsph'] & 0xFFFF == 0,
                      'nocol': not v['col'], 'hasmat': bool(v['mat']), 'nomat': not v['mat'], 'loops': len([e for e in m.ev if e[0] == K_IPR]) > 1, 'alloc': any(e[0] == K_ALLOC for e in m.ev)})
    elif name == N_STATIC:
        v = inp['veh'][0]
        pv, pvnull, task = inp['pv']
        al = 0

        def fpv():
            k = m.nextk('fpv')
            m.emit(K_FPV, M32, 0)
            return pv and k != pvnull
        if not fpv():
            al = 0
        else:
            ok = True
            fpv()
            if v['type'] != 0:
                fpv()
                if v['type'] != 9:
                    ok = False
            if ok:
                m.emit(K_FPP, M32)
                m.emit(K_TASK, 'INT+4', 0x2C0)
                if task:
                    ok = False
            if ok:
                fpv()
                p = [dec(b) for b in entity_pos(v)]
                g = gvals(inp)
                if lt(p[0], g[0]) or gt(p[0], g[1]) or lt(p[1], g[2]) or gt(p[1], g[3]):
                    ok = False
                elif not static_speed(v['speed']):
                    ok = False
                else:
                    m.emit(K_IN3D, 'G', 'V0', 0)
                    al = (inp['masks'][4] >> 0) & 1
                    ok = None
            if ok is not None:
                al = 0
        stats = {'result1': al == 1, 'result0': al == 0, 'noveh': not pv, 'type_other': v['type'] not in (0, 9), 'task': bool(task), 'reached_in3d': any(e[0] == K_IN3D for e in m.ev),
                 'speed_fail': any(e[0] == K_FPV for e in m.ev) and v['type'] in (0, 9) and not task and not any(e[0] == K_IN3D for e in m.ev)}
    elif name == N_EMPTY:
        g = gvals(inp)
        gb = inp['gb']
        m.emit(K_FIND, gb[0], gb[2], gb[4], gb[1], gb[3], gb[5], 'L', 16, 'L', 0, 1, 1, 0, 0)
        cnt = i16(inp['cnt'])
        al = 1
        if cnt > 0:
            i = 0
            while True:
                if m.touch(inp['list'][i] % 8):
                    al = 0
                    break
                i += 1
                if not i < cnt:
                    break
        stats = {'result1': al == 1, 'result0': al == 0, 'cnt_le0': cnt <= 0, 'cnt_ge8': cnt >= 8, 'cnt16': cnt == 16, 'cnt_neg': cnt < 0}
    elif name == N_REMOVE:
        n = inp['nveh']
        al = None
        done = False
        for i in range(n, 0, -1):
            idx = i - 1
            v = inp['veh'][idx]
            if v['free']:
                continue
            if not m.touch(idx):
                continue
            if m.ipp(entity_pos(v)):
                continue
            if v['locked']:
                continue
            k = m.nextk('del')
            m.emit(K_DEL, 'V%d' % idx)
            if not m.bit('del', k):
                continue
            m.emit(K_REMOVE, 'V%d' % idx)
            m.emit(K_DTOR, 'V%d' % idx, 1)
            done = True
            break
        stats = {'removed': done, 'none': not done, 'locked_skip': any(inp['veh'][i - 1]['locked'] and not inp['veh'][i - 1]['free'] for i in range(1, n + 1)), 'pool1': n == 1, 'ev_ge4': len(m.ev) >= 4}
    elif name in (N_TIDY, N_TIDYCLOSE):
        n = inp['nveh']
        removed = 0
        for idx in range(n - 1, 0, -1):
            v = inp['veh'][idx]
            if v['free']:
                continue
            if v['type'] not in (0, 9):
                continue
            wrecked = (v['b36'] & 0xF8) == 0x28
            if name == N_TIDY:
                if not m.ipp(entity_pos(v)):
                    continue
                if not wrecked:
                    if not lt(dec(v['upz']), C05):
                        continue
                m.emit(K_REMOVE, 'V%d' % idx)
                m.emit(K_DTOR, 'V%d' % idx, 1)
                removed += 1
            else:
                if not wrecked:
                    continue
                if not m.touch(idx):
                    continue
                rem = True
                if inp['g'][1] != 0:
                    rem = False
                    m.emit(K_COL, 'V%d' % idx)
                    if i16(m.nsph[idx]) <= 0:
                        continue
                    j = 0
                    while True:
                        out = m.xform(idx, m.matptr[idx], j)
                        ok = m.ipr(out, v['sphr'][j])
                        if not ok:
                            rem = True
                        j += 1
                        if not j < i16(m.nsph[idx]):
                            break
                if rem:
                    m.emit(K_REMOVE, 'V%d' % idx)
                    m.emit(K_DTOR, 'V%d' % idx, 1)
                    removed += 1
        stats = {'removed': removed > 0, 'none': removed == 0, 'multi': removed > 1, 'pool1': n == 1, 'pool_ge5': n >= 5}
        if name == N_TIDYCLOSE:
            stats['state0'] = inp['g'][1] == 0
            stats['state_other'] = inp['g'][1] != 0
    # cambios por reentrada (contador de esferas del CCollisionData)
    for i in range(8):
        orig = inp['veh'][i]['nsph'] & 0xFFFF if i < len(inp['veh']) else None
        if orig is not None and m.nsph[i] != orig:
            changes.append(['D', i, 0, 0xDEAD0000 | m.nsph[i]])
    for (region, i, w), val in sorted(m.chg.items()):
        changes.append([region, i, w, val])
    changes.sort(key=lambda c: (c[0], c[1], c[2]))
    observed = {'events': m.ev[:MAX_EVENTS], 'nevents': len(m.ev), 'changes': changes}
    if al is not None:
        observed['al'] = al
    if ecx is not None:
        observed['ecx'] = ecx
        observed['edx'] = edx
    stats['reentry'] = inp['re'][1] != 0 and inp['re'][0] != M32 and any(
        (e[0] == K_XFORM and inp['re'][1] == 1) or (e[0] == K_IPR and inp['re'][1] == 2) for e in m.ev)
    stats['reentry_changed'] = any(c[0] == 'D' for c in changes)
    return observed, stats


def selftest():
    # aritmetica
    assert enc(dec(0x3F800000)) == 0x3F800000 and enc(dec(1)) == 1 and enc(dec(0x80000001)) == 0x80000001
    assert add_f32(0x3F800000, 0x3F800000) == 0x40000000
    assert sub_f32(0x7F7FFFFF, 0xFF7FFFFF) == 0x7F800000          # desborda a infinito al guardar
    assert sub_f32(0x7F800000, 0x7F800000) == 0xFFC00000          # inf - inf: indefinido
    assert add_f32(0x00000001, 0x00000001) == 2                   # subnormales conservados
    assert near([dec(0x41200000)] * 6, dec(0), [dec(0x41200000)] * 3)
    assert not strict([dec(0x41200000)] * 6, dec(0), [dec(0x41200000)] * 3)
    assert static_speed([0, 0, 0]) and not static_speed([0x3C23D70B, 0, 0]) and static_speed([0x3C23D70A, 0x80000000, 0x7FC00000])
    # PlayerArrestedOrDied / Init
    base = dict(fn=1, fam=0, regs=[0, 0, 0, 77], g=[3, 0, 0x55, 5, 6], gb=[0] * 6, radius=0, nveh=0, masks=[0] * 5, seed=0, re=[M32, 0, 0, 0], pv=[1, 255, 0], cnt=0, list=[0] * 16, veh=[])
    obs, _ = expected(N_ARRESTED, base, 0)
    assert obs['changes'] == [['G', 0, 19, 3 | (3 << 8) | (0x55 << 16) | (0xA5 << 24)]], obs['changes']
    obs, _ = expected(N_INIT, dict(base, fn=0, g=[2, 9, 0xFF, 5, 6]), 0)
    assert obs['changes'] == [['G', 0, 14, 0x3F800000], ['G', 0, 15, 0], ['G', 0, 19, 2 | (1 << 8) | (0x79 << 16) | (0xA5 << 24)]], obs['changes']
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
