"""Oraculo independiente del banco CameraSoundDist30: CCamera::UpdateSoundDistances (0x515BD0). Se modela desde el DESENSAMBLADO (no desde el C++ de la DLL) a partir de la fila `input`.
x87 con control de precision a 24 bits y exponente extendido (fadd/fsub/fmul redondean al par a 24 bits; fstp dword redondea al rango float32):

  515BD6..515C35  modo = word [this + this.activeCam * 0x238 + 0x180]; si modo (16 bits) esta en {10,7,27,28,34,2A,2B,29,2D,2E,22,8,33}h y ([target + 0x36] & 7) == 3 -> escala 0,5 (0x858B8C), si no 5,0 (0x858C80)
  515C48..515CCC  off = fwd * escala (z: fstp float); punto = off + pos (cada suma a 24 bits y fstp float; pos = matriz+0x30 si m_matrix != 0, si no this+4)
  515CD8..515CE3  fase = frameCounter % 12
  515CEC..515D3F  si fase == 0: [160] = [15C]; llama a 0x5674E0(&punto, float(punto.z + 20), &colPoint, &entidad, 1,0,0,0,1,0,0); si devuelve AL != 0: [15C] = float(colPoint.z - punto.z), si no 20,0
  515D49..515D74  t = (fase + 1) * (1/6 del EXE, 0x3E2AAAAB) a 24 bits; [158] = float(((1 - t) * [160]) + (t * [15C])), cada operacion a 24 bits
Solo se escriben +0x158, +0x15C y +0x160 (nada mas del objeto, ni la entidad, ni la matriz, ni el contador). Una operacion con un unico NaN devuelve ese NaN; con dos, el de mayor significando; mismo
significando y signos opuestos -> DESCONOCIDO ('??', se exige EXE == DLL); 0*inf o inf-inf sin NaN dan el indefinido real (0xFFC00000). Los valores desconocidos se propagan.
"""
import random
import math
import struct
from fractions import Fraction

INDEFINITE = 0xFFC00000


class V:
    """Valor x87: kind 'f' finito (val Fraction, neg = signo del cero), 'inf' (neg), 'nan' (bits float32 silencioso), 'unk' (NaN de carga util/signo desconocidos)."""
    __slots__ = ('kind', 'val', 'neg', 'bits')

    def __init__(self, kind, val=Fraction(0), neg=False, bits=0):
        self.kind, self.val, self.neg, self.bits = kind, val, neg, bits

    def is_nan(self):
        return self.kind in ('nan', 'unk')

    def sign(self):
        return self.neg if (self.kind == 'inf' or (self.kind == 'f' and self.val == 0)) else self.val < 0

    def is_zero(self):
        return self.kind == 'f' and self.val == 0


def load(bits):
    """fld dword: exacta (los subnormales se cargan tal cual)."""
    bits &= 0xFFFFFFFF
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 0xFF:
        if man:
            return V('nan', bits=bits | 0x400000)       # sNaN -> qNaN (las fixtures no los generan)
        return V('inf', neg=bool(sign))
    if exp == 0:
        val = Fraction(man, 1 << 149)
    else:
        val = Fraction(man | 0x800000, 1 << 23) * (Fraction(2) ** (exp - 127))
    if sign:
        val = -val
    return V('f', val, neg=bool(sign))


def from_int(n):
    """fild dword."""
    return V('f', Fraction(n), neg=n < 0)


def r24(x):
    """Redondeo exacto (al par) de un racional a 24 bits de mantisa, exponente sin limite (x87 con PC=24)."""
    x = Fraction(x)
    if x == 0:
        return Fraction(0)
    sign = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    m = a / ulp
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    return sign * q * ulp


def nan_pick(a, b):
    """Resultado de una operacion con al menos un NaN."""
    if a.kind == 'unk' or b.kind == 'unk':
        return V('unk')
    if a.is_nan() and b.is_nan():
        ma, mb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if ma > mb:
            return a
        if mb > ma:
            return b
        if (a.bits >> 31) == (b.bits >> 31):
            return a
        return V('unk')                                  # empate con signos opuestos: no resuelto
    return a if a.is_nan() else b


def indefinite():
    return V('nan', bits=INDEFINITE)


def neg(v):
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return V('inf', neg=not v.neg)
    if v.val == 0:
        return V('f', Fraction(0), neg=not v.neg)
    return V('f', -v.val, neg=v.val > 0)


def add(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    if a.kind == 'inf' or b.kind == 'inf':
        if a.kind == 'inf' and b.kind == 'inf':
            return a if a.neg == b.neg else indefinite()
        return a if a.kind == 'inf' else b
    s = a.val + b.val
    if s == 0:
        if a.val == 0 and b.val == 0:
            return V('f', Fraction(0), neg=a.neg and b.neg)
        return V('f', Fraction(0), neg=False)            # cancelacion exacta: +0 (redondeo al mas cercano)
    s = r24(s)
    return V('f', s, neg=s < 0)


def sub(a, b):
    return add(a, neg(b))


def mul(a, b):
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf' or b.kind == 'inf':
        if a.is_zero() or b.is_zero():
            return indefinite()
        return V('inf', neg=sg)
    p = a.val * b.val
    if p == 0:
        return V('f', Fraction(0), neg=sg)
    p = r24(p)
    return V('f', p, neg=p < 0)


def div(a, b):
    """a / b (fdiv / fdivr)."""
    if a.is_nan() or b.is_nan():
        return nan_pick(a, b)
    sg = a.sign() != b.sign()
    if a.kind == 'inf':
        return indefinite() if b.kind == 'inf' else V('inf', neg=sg)
    if b.kind == 'inf':
        return V('f', Fraction(0), neg=sg)
    if b.val == 0:
        return indefinite() if a.val == 0 else V('inf', neg=sg)
    if a.val == 0:
        return V('f', Fraction(0), neg=sg)
    q = r24(a.val / b.val)
    return V('f', q, neg=q < 0)


def fsqrt24(p):
    """sqrt de un racional positivo redondeada al par a 24 bits (exacta, con bit pegajoso)."""
    p = Fraction(p)
    k = 400
    n = p * (Fraction(2) ** (2 * k))
    ni = n.numerator // n.denominator
    r = math.isqrt(ni)
    inexact = r * r != n
    v = Fraction(2 * r + 1, 2) if inexact else Fraction(r)
    return r24(v / (Fraction(2) ** k))


def sqrt(a):
    """fsqrt (solo se llama con a >= 0, +0 o NaN/inf)."""
    if a.is_nan():
        return a
    if a.kind == 'inf':
        return a if not a.neg else indefinite()
    if a.val == 0:
        return a
    if a.val < 0:
        return indefinite()
    s = fsqrt24(a.val)
    return V('f', s, neg=False)


def store(v):
    """fstp dword: bits float32 (None = desconocido)."""
    if v.kind == 'unk':
        return None
    if v.kind == 'nan':
        return v.bits | 0x400000
    if v.kind == 'inf':
        return 0xFF800000 if v.neg else 0x7F800000
    if v.val == 0:
        return 0x80000000 if v.neg else 0
    sign = 0x80000000 if v.val < 0 else 0
    a = abs(v.val)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    if Fraction(2) ** (e + 1) <= a:
        e += 1
    quantum = Fraction(2) ** (max(e, -126) - 23)
    m = a / quantum
    q = m.numerator // m.denominator
    rem = m - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    res = q * quantum
    if res >= Fraction(2) ** 128:
        return sign | 0x7F800000
    if res == 0:
        return sign
    return sign | struct.unpack('<I', struct.pack('<f', float(res)))[0] & 0x7FFFFFFF


def fl(v):
    """store + fld: valor tras pasar por memoria como float32 (V('unk') si el valor es desconocido)."""
    b = store(v)
    return V('unk') if b is None else load(b)


def unordered_or_cmp(a, b):
    """fcom: 'unordered' | 'less' | 'equal' | 'greater' (+0 == -0)."""
    if a.is_nan() or b.is_nan():
        return 'unordered'
    av = math.inf if a.kind == 'inf' and not a.neg else (-math.inf if a.kind == 'inf' else a.val)
    bv = math.inf if b.kind == 'inf' and not b.neg else (-math.inf if b.kind == 'inf' else b.val)
    return 'less' if av < bv else ('greater' if av > bv else 'equal')


ONE = load(0x3F800000)
ZERO = load(0)


def is_float_bits_nan(b):
    return ((b >> 23) & 0xFF) == 0xFF and (b & 0x7FFFFF) != 0


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def hex_wild_match(obs, want):
    """Compara dos cadenas hex (observada, esperada con '??' como comodin por byte)."""
    if len(obs) != len(want):
        return False
    for j in range(0, len(want), 2):
        if want[j:j + 2] != '??' and want[j:j + 2] != obs[j:j + 2]:
            return False
    return True

CASES = 4096
NAME = 'CCamera::UpdateSoundDistances'
NAMES = [NAME]
# Constantes de memoria: DEBEN coincidir con Fixtures.hpp (las comprueba fixture_selfcheck.py contra el volcado compilado de Fixtures.hpp).
LAYOUT = {'guard': 0x40, 'obj_bytes': 0xD78, 'entity_bytes': 0x40, 'matrix_bytes': 0x48, 'arena': 0xF00, 'written_bytes': 12}
LIST_MODES = {0x10, 0x07, 0x27, 0x28, 0x34, 0x2A, 0x2B, 0x29, 0x2D, 0x2E, 0x22, 0x08, 0x33}
SCALE_5, SCALE_HALF, TWENTY, SIXTH, ONE_BITS = 0x40A00000, 0x3F000000, 0x41A00000, 0x3E2AAAAB, 0x3F800000   # 0x858C80, 0x858B8C, 0x858BA4 (y 0x41A00000), 0x85F0A0, 0x858624
FLT_MAX_LIMIT = Fraction(2) ** 128



def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    mode16 = inp['mode'] & 0xFFFF
    ped_scale = mode16 in LIST_MODES and (inp['etype'] & 7) == 3
    scale = load(SCALE_HALF if ped_scale else SCALE_5)
    pos = inp['mat_pos'] if inp['has_matrix'] else inp['place_pos']
    off = [mul(load(inp['fwd'][i]), scale) for i in range(3)]
    zs = fl(off[2])
    px = store(add(off[0], load(pos[0])))
    py = store(add(off[1], load(pos[1])))
    pz = store(add(zs, load(pos[2])))
    phase = inp['frame'] % 12
    new15c, old160 = inp['old15c'], inp['old160']
    trace, calls = [], 0
    delta_nan = False
    if phase == 0:
        old160 = inp['old15c']
        ray_end = None if pz is None else store(add(load(pz), load(TWENTY)))
        calls = 1
        trace = [px, py, pz, ray_end, 1, 0, 0, 0, 1, 0, 0]
        if inp['hit']:
            new15c = None if pz is None else store(sub(load(inp['hit_z']), load(pz)))
            delta_nan = new15c is not None and is_float_bits_nan(new15c)
        else:
            new15c = 0x41A00000
    t = mul(from_int(phase + 1), load(SIXTH))
    inv = sub(load(ONE_BITS), t)
    a = V('unk') if old160 is None else mul(inv, load(old160))
    b = V('unk') if new15c is None else mul(t, load(new15c))
    result = store(add(a, b))
    words = [result, new15c, old160]
    written = ''.join('????????' if w is None else struct.pack('<I', w).hex() for w in words)
    observed = {'written': written, 'rest_ok': True, 'guard_ok': True, 'entity_ok': True, 'matrix_ok': True, 'frame_ok': True, 'calls': calls, 'trace': trace}
    known = [w for w in words if w is not None]
    final_finite = all(((w >> 23) & 0xFF) != 0xFF for w in known) and len(known) == 3
    stats = {'family': inp['family'], 'ran': phase == 0, 'hit': phase == 0 and bool(inp['hit']), 'nohit': phase == 0 and not inp['hit'], 'ped_scale': ped_scale,
             'list_mode_nonped': mode16 in LIST_MODES and not ped_scale, 'mode_hi_bits': mode16 in LIST_MODES and (inp['mode'] >> 16) != 0,
             'ped_nonlist': mode16 not in LIST_MODES and (inp['etype'] & 7) == 3, 'has_matrix': bool(inp['has_matrix']), 'no_matrix': not inp['has_matrix'],
             'ext_exp': any(v.kind == 'f' and abs(v.val) >= FLT_MAX_LIMIT for v in off) and all(p is not None and ((p >> 23) & 0xFF) != 0xFF for p in (px, py, pz)),
             'nan_out': any(is_float_bits_nan(w) for w in known), 'inf_out': any(((w >> 23) & 0xFF) == 0xFF and not (w & 0x7FFFFF) for w in known),
             'subnormal_out': any(((w >> 23) & 0xFF) == 0 and (w & 0x7FFFFF) for w in known), 'unknown': len(known) != 3 or (phase == 0 and (px is None or py is None or pz is None)),
             'blend_ext': any(v.kind == 'f' and abs(v.val) >= FLT_MAX_LIMIT for v in (a, b)) and result is not None and ((result >> 23) & 0xFF) != 0xFF,
             'delta_nan': delta_nan, 'active%d' % inp['active']: True, 'frame_hi': inp['frame'] >= 0x80000000, 'phase%d' % phase: True,
             'point_inf': any(p is not None and ((p >> 23) & 0xFF) == 0xFF and not (p & 0x7FFFFF) for p in (px, py, pz)),
             'final_finite': final_finite}
    return observed, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines ('??' en `written`, None en `trace`)."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if k == 'written':
            if not hex_wild_match(o, w):
                return False
        elif k == 'trace':
            if len(o) != len(w) or any(x is not None and x != y for x, y in zip(w, o)):
                return False
        elif o != w:
            return False
    return True


def f32(v):
    return struct.unpack('<f', struct.pack('<f', v))[0]


def bits(v):
    return struct.unpack('<I', struct.pack('<f', v))[0]


def flt(b):
    return struct.unpack('<f', struct.pack('<I', b))[0]


def row(**kw):
    inp = {'family': 0, 'mode': 4, 'active': 1, 'etype': 3, 'fwd': [bits(1.0), 0, 0], 'has_matrix': 1, 'mat_pos': [bits(10.0), bits(20.0), bits(30.0)], 'place_pos': [bits(-1.0), bits(-2.0), bits(-3.0)],
           'frame': 5, 'old158': 0, 'old15c': bits(2.0), 'old160': bits(1.0), 'hit': 1, 'hit_z': bits(100.0), 'ret_high': 0}
    inp.update(kw)
    return inp


def selftest():
    # fase 5 (sin rayo), modo 4 (escala 5): punto = (1,0,0)*5 + matriz (10,20,30); mezcla t = 6/6 (aprox 1.0000001): [158] = (1 - t)*[160] + t*[15C] ~ [15C]
    want, st = simulate(row())
    assert want['calls'] == 0 and want['trace'] == []
    t = f32(f32(6.0) * flt(SIXTH))
    exp = f32(f32(f32(1.0 - t) * 1.0) + f32(t * 2.0))
    assert want['written'] == struct.pack('<3I', bits(exp), bits(2.0), bits(1.0)).hex(), (want['written'], hex(bits(exp)))
    # fase 0, ped en modo 0x10 (escala 0.5), con choque: [160] = [15C] antiguo; [15C] = float(z_choque - z_punto)
    want, st = simulate(row(mode=0x10, frame=24, fwd=[bits(2.0), bits(4.0), bits(6.0)]))
    assert st['ped_scale'] and want['calls'] == 1
    assert want['trace'] == [bits(11.0), bits(22.0), bits(33.0), bits(53.0), 1, 0, 0, 0, 1, 0, 0], want['trace']
    new = f32(100.0 - 33.0)
    t = f32(1 * flt(SIXTH))
    exp = f32(f32(f32(1.0 - t) * 2.0) + f32(t * new))
    assert want['written'] == struct.pack('<3I', bits(exp), bits(new), bits(2.0)).hex()
    # sin choque: [15C] = 20
    want, st = simulate(row(frame=36, hit=0))
    assert struct.unpack('<3I', bytes.fromhex(want['written']))[1] == 0x41A00000 and want['calls'] == 1
    # modo en la lista pero objetivo no ped (tipo 4): escala 5; basura en el modo (bits altos) sigue en la lista; vecinos fuera
    assert simulate(row(mode=0x10 | (0xABCD << 16), etype=3))[1]['ped_scale']
    assert not simulate(row(mode=0x10, etype=4))[1]['ped_scale'] and not simulate(row(mode=0x11, etype=3))[1]['ped_scale']
    assert simulate(row(mode=0x33, etype=0xFB))[1]['ped_scale']          # byte 0xFB & 7 == 3
    # sin matriz: se usa la posicion propia (+4)
    want, st = simulate(row(has_matrix=0, frame=0, fwd=[0, 0, 0]))
    assert want['trace'][:3] == [bits(-1.0), bits(-2.0), bits(-3.0)] and want['trace'][3] == bits(17.0)
    # exponente extendido: 5 * 2^126 desborda un float pero 5 * 2^126 - 2^127 = 3 * 2^126 no: 0x7F400000
    want, st = simulate(row(frame=0, fwd=[0x7E800000, 0, 0], mat_pos=[0xFF000000, 0, 0], hit=0))
    assert want['trace'][0] == 0x7F400000 and st['ext_exp'], hex(want['trace'][0])
    # NaN y comodines
    want, st = simulate(row(frame=0, fwd=[0x7FC00001, 0, 0], mat_pos=[0xFFC00002, 0, 0]))
    assert want['trace'][0] == 0xFFC00002                       # gana el de mayor significando (con su signo)
    want, st = simulate(row(frame=0, fwd=[0x7FC00001, 0, 0], mat_pos=[0xFFC00001, 0, 0]))
    assert want['trace'][0] is None and want['written'][:8] != '????????' and st['unknown']
    # 0 * inf = indefinido
    want, st = simulate(row(frame=0, fwd=[0x7F800000, 0, 0], mat_pos=[0, 0, 0], place_pos=[0, 0, 0]))
    assert want['trace'][0] == 0x7F800000 and want['trace'][1] == 0
    want, st = simulate(row(frame=0, fwd=[0, 0, 0x7F800000], mat_pos=[0, 0, 0xFF800000]))   # inf - inf (z)
    assert want['trace'][2] == INDEFINITE, hex(want['trace'][2])
    # coherencia con una cadena float32 por operacion para valores tipicos (doble redondeo despreciable: 24 bits en cada paso)
    rng = random.Random(30)
    for _ in range(3000):
        fwd = [f32(rng.uniform(-1, 1)) for _ in range(3)]
        mp = [f32(rng.uniform(-500, 500)) for _ in range(3)]
        mode, etype = rng.choice([4, 0x10, 0x33, 0x2F]), rng.choice([3, 4])
        frame = rng.randrange(0, 5000)
        o160, o15c = f32(rng.uniform(-30, 30)), f32(rng.uniform(-30, 30))
        hz = f32(rng.uniform(-500, 500))
        hit = rng.randrange(2)
        sc = f32(0.5) if (mode in (0x10, 0x33) and etype == 3) else f32(5.0)
        p = [f32(f32(fwd[i] * sc) + mp[i]) for i in range(3)]
        ph = frame % 12
        n15, o16 = o15c, o160
        if ph == 0:
            o16 = o15c
            n15 = f32(hz - p[2]) if hit else 20.0
        t = f32(f32(ph + 1) * flt(SIXTH))
        res = f32(f32(f32(1.0 - t) * o16) + f32(t * n15))
        want, st = simulate(row(mode=mode, etype=etype, frame=frame, fwd=[bits(v) for v in fwd], mat_pos=[bits(v) for v in mp], old15c=bits(o15c), old160=bits(o160), hit=hit, hit_z=bits(hz)))
        assert want['written'] == struct.pack('<3I', bits(res), bits(n15), bits(o16)).hex(), (ph, hit)
        if ph == 0:
            assert want['trace'][:3] == [bits(v) for v in p]
    assert matches({'written': 'abcd', 'trace': [1, 2]}, {'written': 'ab??', 'trace': [1, None]})
    assert not matches({'written': 'abce', 'trace': [1, 2]}, {'written': 'ab??', 'trace': [1, 3]})
    print('ok')


if __name__ == '__main__':
    selftest()
