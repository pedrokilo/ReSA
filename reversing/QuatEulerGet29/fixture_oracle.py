"""Oraculo independiente del banco QuatEulerGet29: CQuaternion::Get(float* x, float* y, float* z) (0x59C160).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. Reutiliza, sin copiarlo, el modelo x87 de 24 bits de
reversing/QuatGet28/fixture_oracle.py (V, load, r24, add/sub/mul, store, compute = CQuaternion::Get(RwMatrix*), 0x59C080). `sub esp,40h`; la matriz local m (right = m[0..2], up = m[4..6], at = m[8..10]) la
rellena 0x59C080; luego (a1 = m[1] = [esp+4], b1 = m[5] = [esp+14h], c1 = m[9] = [esp+24h], a0 = m[0], a2 = m[2], b0 = m[4], b2 = m[6]; px = 1er puntero [esp+44h], py = 2o [esp+48h], pz = 3o [esp+4Ch]):

  59C16D fld a1 ; fld b1 ; fpatan                      r1 = atan2(a1, b1)  (64 bits: fpatan no se redondea a 24 bits)
  59C17B fld st ; fstp [pz]                            *pz = float(r1)                                         <- 1a escritura de z
  59C17F fcom 0.0 ; test ah,5 ; jp                     si r1 < 0:  fadd [2*pi float 0x40C90FDB] (a 24 bits) ; fstp [pz]   (NaN, 0 y > 0 no suman; -0 == 0)
  59C198 fld [pz] ; fsin ; fld [pz] ; fcos             sin, cos de z = el float YA guardado, 64 bits
  59C1A4 fld c1 ; fchs                                 -c1 (fchs invierta el signo tambien en NaN)
  59C1AA S = r24(sin * a1) + r24(cos * b1)  (cada fmul a 24 bits, faddp a 24 bits)
  59C1B8 fpatan -> r2 = atan2(-c1, S) ; igual que arriba: fstp [px] ; si r2 < 0: + 2*pi ; fstp [px]
  59C1D7 D = r24(r24(a2 * cos) - r24(b2 * sin)) ; fchs ; E = r24(r24(a0 * cos) - r24(b0 * sin))
  59C1F9 fpatan -> r3 = atan2(-D, E) ; fstp [py] ; si r3 < 0: + 2*pi ; fstp [py]
Orden de escritura en memoria: z, x, y (cada una solo cuenta la ultima escritura); las lecturas de la memoria de la funcion son las de 0x59C080 (cuaternion) antes de todas
las escrituras, y *pz tras escribirse z (nadie escribe entre medias). La matriz es local: los punteros solo pueden pisar la ventana (cuaternion y entre si).
Precision del oraculo: sin/cos/atan2 se calculan en doubles (53 bits) frente a los 64 bits del x87; por eso (1) las salidas float se comparan con tolerancia de ULP_TOL ulp y (2) cada salida
se evalua tambien con perturbaciones relativas de 2^-50 en los resultados trascendentes: si las variantes difieren mas de la tolerancia (o de clase: NaN/signo/cero), la salida es DESCONOCIDA
(comodin '??', solo se exige EXE == DLL). Los NaN siguen las reglas del modelo de QuatGet28 (un NaN pasa tal cual; dos: el de mayor significando; empate con distinto signo: desconocido).
"""
import importlib.util
import math
import random
import struct
from fractions import Fraction
from pathlib import Path

HERE = Path(__file__).resolve().parent
_spec = importlib.util.spec_from_file_location('quatget28_oracle', HERE.parent / 'QuatGet28' / 'fixture_oracle.py')
mat = importlib.util.module_from_spec(_spec)
_spec.loader.exec_module(mat)
V, load, r24, add, sub, mul, store, neg, indefinite, nan_pick = mat.V, mat.load, mat.r24, mat.add, mat.sub, mat.mul, mat.store, mat.neg, mat.indefinite, mat.nan_pick

CASES = 4096
NAME = 'CQuaternion::Get(float*, float*, float*)'
NAMES = [NAME]
WIN_OFFSET, WIN_BYTES, ARENA = 0x40, 0x40, 0xC0
INDEFINITE = 0xFFC00000
TWO_PI = load(0x40C90FDB)
ULP_TOL = 2
EPS = 2.0 ** -50
VARIANTS = 4


def chs(v):
    """fchs: invierte el signo de cualquier valor, tambien de un NaN."""
    if v.kind == 'nan':
        return V('nan', bits=v.bits ^ 0x80000000)
    if v.kind == 'unk':
        return v
    return neg(v)


def to_float(v):
    if v.kind == 'inf':
        return -math.inf if v.neg else math.inf
    if v.kind == 'f' and v.val == 0:
        return -0.0 if v.neg else 0.0
    return float(v.val)


def from_float(d):
    if d == 0:
        return V('f', Fraction(0), neg=math.copysign(1.0, d) < 0)
    if math.isinf(d):
        return V('inf', neg=d < 0)
    return V('f', Fraction(d), neg=d < 0)


def pert(v, d):
    if d == 0 or v.kind != 'f' or v.val == 0:
        return v
    return V('f', v.val * (1 + Fraction(d)), neg=v.val < 0)


def atan2v(y, x):
    """fpatan: atan2(ST1 = y, ST0 = x) en doubles; NaN como en las operaciones aritmeticas."""
    if y.kind == 'unk' or x.kind == 'unk':
        return V('unk')
    if y.is_nan() or x.is_nan():
        return nan_pick(y, x)
    return from_float(math.atan2(to_float(y), to_float(x)))


def sinv(v, d):
    if v.kind == 'unk':
        return v
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return indefinite()
    if v.val == 0:
        return v
    return pert(from_float(math.sin(to_float(v))), d)


def cosv(v, d):
    if v.kind == 'unk':
        return v
    if v.is_nan():
        return v
    if v.kind == 'inf':
        return indefinite()
    if v.val == 0:
        return V('f', Fraction(1))
    return pert(from_float(math.cos(to_float(v))), d)


def wrap(r):
    """fstp [out] ; fcom 0 ; si < 0: fadd 2*pi ; fstp [out]. -> (bits finales o None, valor releido)."""
    if r.kind == 'unk':
        return None, V('unk')
    if r.kind == 'f' and r.val < 0:
        r = add(r, TWO_PI)
    b = store(r)
    return b, (V('unk') if b is None else load(b))


def chain(m, d):
    """m: dict de V por nombre de la matriz; d: 5 perturbaciones. -> (zbits, xbits, ybits) con None = desconocido."""
    r1 = pert(atan2v(m['m1'], m['m5']), d[0])
    zb, zv = wrap(r1)
    sn = sinv(zv, d[1])
    cs = cosv(zv, d[2])
    s_ = add(mul(sn, m['m1']), mul(cs, m['m5']))
    r2 = pert(atan2v(chs(m['m9']), s_), d[3])
    xb, _ = wrap(r2)
    dd = sub(mul(m['m2'], cs), mul(m['m6'], sn))
    ee = sub(mul(m['m0'], cs), mul(m['m4'], sn))
    r3 = pert(atan2v(chs(dd), ee), d[4])
    yb, _ = wrap(r3)
    return zb, xb, yb


def ulp_close(a, b, tol=ULP_TOL):
    if a == b:
        return True
    if (a >> 31) != (b >> 31) or ((a >> 23) & 0xFF) == 0xFF or ((b >> 23) & 0xFF) == 0xFF:
        return False
    return abs((a & 0x7FFFFFFF) - (b & 0x7FFFFFFF)) <= tol


def euler_outputs(q, seed=0):
    """q = [x, y, z, w] bits -> (z, x, y) bits o None. Evaluacion nominal y variantes perturbadas (inestables -> None)."""
    outs = mat.compute(q)
    m = {k: (V('unk') if v is None else load(v)) for k, v in outs.items()}
    nominal = chain(m, (0, 0, 0, 0, 0))
    rng = random.Random(seed * 7919 + 13)
    result = list(nominal)
    for _ in range(VARIANTS):
        d = tuple(rng.choice((-EPS, 0.0, EPS)) for _ in range(5))
        var = chain(m, d)
        for i in range(3):
            if result[i] is None:
                continue
            if var[i] is None or not ulp_close(result[i], var[i]):
                result[i] = None
    return tuple(result), nominal


def hexbytes(b, wild=None):
    wild = wild or set()
    return ''.join('??' if i in wild else '%02x' % v for i, v in enumerate(b))


def simulate(inp):
    """-> (observado esperado con comodines, stats)."""
    win = bytearray(bytes.fromhex(inp['win_in']))
    wild = set()
    q = list(inp['q'])
    qoff, xoff, yoff, zoff = inp['qoff'], inp['xoff'], inp['yoff'], inp['zoff']
    win[qoff:qoff + 16] = struct.pack('<4I', *q)
    for i in range(qoff, qoff + 16):
        wild.discard(i)
    (zb, xb, yb), nominal = euler_outputs(q, inp.get('seed', 0))
    # 0x59C080 lee el cuaternion antes de cualquier escritura; despues se escribe z, x, y (la ultima escritura de cada uno manda)
    for off, bits in ((zoff, zb), (xoff, xb), (yoff, yb)):
        for i in range(off, off + 4):
            wild.discard(i)
        if bits is None:
            wild.update(range(off, off + 4))
        else:
            win[off:off + 4] = struct.pack('<I', bits)
    arena = bytearray([0xC3] * ARENA)
    arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES] = win
    arena_hex = hexbytes(arena, {WIN_OFFSET + i for i in wild})
    qv = [load(b) for b in q]
    stats = {'family': inp['family']}
    stats['nan_in'] = any(v.is_nan() for v in qv)
    stats['inf_in'] = any(v.kind == 'inf' for v in qv)
    stats['finite_in'] = all(v.kind == 'f' for v in qv)
    offs = (qoff, xoff, yoff, zoff)
    stats['alias'] = (xoff == yoff or yoff == zoff or xoff == zoff or any(qoff - 4 < o < qoff + 16 for o in offs[1:]))
    known = [b for b in (zb, xb, yb) if b is not None]
    stats['unknown'] = len(known) != 3
    stats['all_known'] = not stats['unknown']
    stats['nan_out'] = any(((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF) for v in known)
    stats['inf_out'] = any(((v >> 23) & 0xFF) == 0xFF and not (v & 0x7FFFFF) for v in known)
    stats['finite_out'] = all(((v >> 23) & 0xFF) != 0xFF for v in known)
    stats['zero_out'] = any((v & 0x7FFFFFFF) == 0 for v in known)
    stats['negzero_out'] = any(v == 0x80000000 for v in known)
    stats['subnormal_out'] = any(((v >> 23) & 0xFF) == 0 and (v & 0x7FFFFF) for v in known)
    # ramas del +2*pi (fcom < 0) segun el modelo nominal: se recalcula con la cadena para contar cuantas salidas son >= pi (resultado de una suma de 2*pi sobre un negativo)
    stats['wrapped_out'] = any(0x40490FDB < v < 0x40C90FDC for v in known)
    stats['pi_out'] = any(v in (0x40490FDB, 0xC0490FDB) for v in known)
    return {'arena': arena_hex, 'guard_ok': True}, stats


def expected(inp, case):
    inp = dict(inp)
    inp['seed'] = case
    return simulate(inp)


def _within_ulps(a_hex, b_hex, tol):
    x = struct.unpack('<I', bytes.fromhex(a_hex))[0]
    y = struct.unpack('<I', bytes.fromhex(b_hex))[0]
    return ulp_close(x, y, tol)


def matches(obs, want):
    """Compara lo observado con lo esperado respetando los comodines '??' (cadenas) y la tolerancia de ULP_TOL ulp en palabras float finitas del arena."""
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if isinstance(w, str):
            if len(o) != len(w):
                return False
            for i in range(0, len(w), 8):
                ws, os_ = w[i:i + 8], o[i:i + 8]
                if ws == os_ or all(ws[j:j + 2] == '??' or ws[j:j + 2] == os_[j:j + 2] for j in range(0, len(ws), 2)):
                    continue
                if k == 'arena' and len(ws) == 8 and '?' not in ws and _within_ulps(ws, os_, ULP_TOL):
                    continue
                return False
        elif o != w:
            return False
    return True


def run_one(q, qoff=0, xoff=0x10, yoff=0x14, zoff=0x18, win_in=None):
    inp = {'family': 0, 'q': list(q), 'qoff': qoff, 'xoff': xoff, 'yoff': yoff, 'zoff': zoff, 'win_in': win_in or '00' * WIN_BYTES}
    want, st = simulate(inp)
    arena = bytes.fromhex(want['arena'].replace('??', '00'))
    return want, arena[WIN_OFFSET:WIN_OFFSET + WIN_BYTES], st


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    u = lambda win, off: struct.unpack_from('<I', win, off)[0]
    one, half, h7 = 0x3F800000, 0x3F000000, 0x3F3504F3
    # identidad (x,y,z,w) = (0,0,0,1): matriz identidad -> z = atan2(0, 1) = 0, x = atan2(-0, 0 + 1) = -0 (no es < 0), y = atan2(-(0*1 - 0*0), 1) = -0
    want, win, st = run_one([0, 0, 0, one])
    assert (u(win, 0x18), u(win, 0x10), u(win, 0x14)) == (0, 0x80000000, 0x80000000), [hex(u(win, o)) for o in (0x18, 0x10, 0x14)]
    # giro de 90 grados sobre z: z = w = 0,7071 -> m1 = 2*z*w = 1, m5 = 1 - 2*z*z ~ 0 -> z (angulo) = atan2(1, ~0) ~ pi/2
    want, win, st = run_one([0, 0, h7, h7])
    assert abs(flt(u(win, 0x18)) - math.pi / 2) < 1e-6, flt(u(win, 0x18))
    # giro de -90 grados sobre z: m1 = -1 -> atan2(-1, ~0) = -pi/2 < 0 -> + 2*pi = 3*pi/2
    want, win, st = run_one([0, 0, 0xBF3504F3, h7])
    assert abs(flt(u(win, 0x18)) - 3 * math.pi / 2) < 2e-6, flt(u(win, 0x18))
    # Euler contra una cadena float32 (tolerancia 2 ulp) con 800 cuaterniones aleatorios de modulo unidad aproximado
    rng = random.Random(29)
    two_pi = flt(0x40C90FDB)
    for _ in range(800):
        qf = [f32(rng.uniform(-1, 1)) for _ in range(4)]
        n = math.sqrt(sum(v * v for v in qf))
        qf = [f32(v / n) for v in qf]
        x, y, z, w = qf
        x2, y2, z2 = f32(x + x), f32(y + y), f32(z + z)
        a, b, c = f32(x2 * x), f32(y2 * x), f32(z2 * x)
        d, e, f = f32(y2 * y), f32(z2 * y), f32(z2 * z)
        g, y2w, z2w = f32(x2 * w), f32(y2 * w), f32(z2 * w)
        mm = dict(m0=f32(1 - f32(f + d)), m4=f32(b - z2w), m8=f32(y2w + c), m1=f32(z2w + b), m5=f32(1 - f32(f + a)), m9=f32(e - g), m2=f32(c - y2w), m6=f32(g + e))
        r1 = math.atan2(mm['m1'], mm['m5'])
        zz = f32(r1 + two_pi) if r1 < 0 else f32(r1)
        sn, cs = math.sin(zz), math.cos(zz)
        r2 = math.atan2(-mm['m9'], f32(f32(sn * mm['m1']) + f32(cs * mm['m5'])))
        xx = f32(r2 + two_pi) if r2 < 0 else f32(r2)
        dd = f32(f32(mm['m2'] * cs) - f32(mm['m6'] * sn))
        ee = f32(f32(mm['m0'] * cs) - f32(mm['m4'] * sn))
        r3 = math.atan2(-dd, ee)
        yy = f32(r3 + two_pi) if r3 < 0 else f32(r3)
        want, win, st = run_one([bits(v) for v in qf])
        for off, ref in ((0x18, zz), (0x10, xx), (0x14, yy)):
            got = u(win, off)
            if want['arena'][2 * (WIN_OFFSET + off):2 * (WIN_OFFSET + off) + 8] == '????????':
                continue                                   # salida inestable: desconocida
            assert ulp_close(got, bits(ref), 4), (qf, hex(off), flt(got), ref)
    # NaN: z = NaN (w NaN -> matriz toda NaN): las tres salidas son NaN
    want, win, st = run_one([one, 0, 0, 0x7FC00005])
    known = [u(win, o) for o in (0x18, 0x10, 0x14) if want['arena'][2 * (WIN_OFFSET + o):2 * (WIN_OFFSET + o) + 8] != '????????']
    assert known and all((v & 0x7F800000) == 0x7F800000 and (v & 0x7FFFFF) for v in known), [hex(v) for v in known]
    # alias: z (3er puntero) se escribe antes que x e y; x == z -> manda x; y sobre el cuaternion (qoff = 0x10, y en 0x14)
    want, win, st = run_one([0, 0, 0, one], qoff=0x10, xoff=0x20, yoff=0x14, zoff=0x20)
    assert u(win, 0x20) == 0x80000000 and u(win, 0x14) == 0x80000000 and u(win, 0x10) == 0, (hex(u(win, 0x20)), hex(u(win, 0x14)))
    assert matches({'arena': 'abcdab', 'guard_ok': True}, {'arena': 'ab??ab', 'guard_ok': True})
    assert not matches({'arena': 'abcdab', 'guard_ok': False}, {'arena': 'ab??ab', 'guard_ok': True})
    print('ok')


if __name__ == '__main__':
    selftest()
