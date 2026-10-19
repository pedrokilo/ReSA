"""Oraculo independiente del banco GaragesAddOne30: CGarages::AddOne (0x4471E0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido
(ver x87model.py). cdecl: [esp+4] x1, y1, z1, frontX, frontY, x2, y2, z2, type (BYTE), a10 (no se lee), name, flags (solo el byte bajo). g = aGarages[NumGarages] (0x96C048 + 0xD8 * NumGarages):

  4471E0 push ecx (4 bytes de pila local: d)
  Limites X: m = (x1 < fx ? x1 : fx) ; m = (m < x2 ? m : x2) ; d = float((fx + x2) - x1)  [fld fx, fadd x2 (m32), fsub x1 (m32), fstp d] ; g.left  = (m < d ? m : d)         -> [+0x28]
             M = (x1 > fx ? x1 : fx) ; M = (M > x2 ? M : x2)                              ;                                    g.right = (M > d ? M : d)         -> [+0x2C]
  Limites Y: igual con y1, fy, y2 y d = float((fy + y2) - y1)                                                                  -> front [+0x30], back [+0x34]
             (cada seleccion es fld del valor elegido + fstp dword: un NaN senal sale silenciado; `fcomp ; test ah,5 ; jp` => "menor" solo si es estrictamente menor y ordenado;
              `test ah,41h ; jne` => "mayor" solo si es estrictamente mayor y ordenado)
  [+0x00] x1, [+0x04] y1, [+0x08] z1 (mov dword: bits tal cual) ; A = (float(fx - x1), float(fy - y1)) -> [+0x0C], [+0x10] ; B = (float(x2 - x1), float(y2 - y1)) -> [+0x14], [+0x18] ; [+0x1C] z2 (bits)
  4474E8 normaliza A: ay, ax releidos de memoria ; s = fsqrt(ay*ay + ax*ax) (registro, 24 bits) ; [+0x20] = float(s) ; [+0x0C] = float(ax / s) con s del registro ; [+0x10] = float(ay / [+0x20]) (el float guardado)
  44752C normaliza B igual: [+0x24] = float(s), [+0x14] = float(bx / s), [+0x18] = float(by / [+0x24])
  447570 [+0x4C] = [+0x4F] = type (byte bajo) ; 44757C strncpy(g + 0x44, name, 7) (0x821F40: copia hasta 7 caracteres y rellena con ceros hasta 7; [+0x4B] no se toca)
  4475A5 flags: byte [+0x4E]: bit 3 = flags & 1, bit 4 = flags & 2, bit 5 = flags & 4 (los demas bits se conservan); NumGarages += 1 ; EAX = indice anterior (nadie lo lee)

NaN (el banco no puede asegurar el signo/carga util cuando dos NaN empatan en significando con signos opuestos): resultado desconocido '??' y solo se exige EXE == DLL.
Observado por lado: {'entry': 0xD8 bytes de la ranura, 'rest_ok': el resto del array igual al de referencia, 'num': NumGarages, 'name_ok': la arena del nombre intacta}.
"""
import json
import random
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from x87model import *  # noqa: E402,F401,F403

CASES = 4096
NAME = 'CGarages::AddOne'
NAMES = [NAME]
ENTRY_BYTES = 0xD8
MAX_GARAGES = 50
ARRAY_BYTES = MAX_GARAGES * ENTRY_BYTES
ARRAY_ADDRESS = 0x96C048
NUM_ADDRESS = 0x96C024
NAME_ARENA = 32
NAME_OFFSET = 8
CONSTANTS = {'entry_bytes': ENTRY_BYTES, 'max_garages': MAX_GARAGES, 'array_bytes': ARRAY_BYTES, 'array_address': ARRAY_ADDRESS, 'num_address': NUM_ADDRESS,
             'name_arena': NAME_ARENA, 'name_offset': NAME_OFFSET, 'cases': CASES}


def _sel_min(a, b):
    return a if lt(a, b) else b


def _sel_max(a, b):
    return a if gt(a, b) else b


def _limits(a, fa, b, c):
    """a = x1, fa = frontX, b = x2, c = cuarta esquina ya releida. -> (minimo, maximo) como V (pueden ser 'unk')."""
    try:
        m = _sel_min(a, fa)
        m = _sel_min(m, b)
        lo = _sel_min(m, c)
        M = _sel_max(a, fa)
        M = _sel_max(M, b)
        hi = _sel_max(M, c)
    except Unknown:
        return V('unk'), V('unk')
    return lo, hi


def simulate(inp):
    """-> (esperado con comodines, estadisticas)."""
    x1b, y1b, z1b, fxb, fyb, x2b, y2b, z2b = inp['in']
    X1, Y1, FX, FY, X2, Y2 = (load(b) for b in (x1b, y1b, fxb, fyb, x2b, y2b))
    entry = bytearray(bytes.fromhex(inp['entry']))
    wild = set()

    def put(off, bits):
        if bits is None:
            wild.update(range(off, off + 4))
        else:
            entry[off:off + 4] = struct.pack('<I', bits)

    M = [mem(b) for b in (x1b, y1b, x2b, y2b)]            # operandos de memoria de fadd/fsub: un NaN senal no se silencia antes de operar
    cx = fl(sub(add(FX, M[2]), M[0]))
    cy = fl(sub(add(FY, M[3]), M[1]))
    left, right = _limits(X1, FX, X2, cx)
    front, back = _limits(Y1, FY, Y2, cy)
    put(0x28, store(left))
    put(0x2C, store(right))
    put(0x30, store(front))
    put(0x34, store(back))
    put(0x00, x1b)
    put(0x04, y1b)
    put(0x08, z1b)
    put(0x1C, z2b)
    ax, ay = fl(sub(FX, M[0])), fl(sub(FY, M[1]))
    bx, by = fl(sub(X2, M[0])), fl(sub(Y2, M[1]))
    dirs = []
    for (px, py), (ox, oy, ol) in (((ax, ay), (0x0C, 0x10, 0x20)), ((bx, by), (0x14, 0x18, 0x24))):
        s = sqrt(add(mul(py, py), mul(px, px)))
        w = fl(s)
        put(ol, store(s))
        put(ox, store(div(px, s)))
        put(oy, store(div(py, w)))
        dirs.append((px, py, s))
    typ = inp['type'] & 0xFF
    entry[0x4C] = typ
    entry[0x4F] = typ
    name = bytes.fromhex(inp['name'])[:7]
    entry[0x44:0x4B] = name + bytes(7 - len(name))
    flags = entry[0x4E] & 0xC7
    flags |= (inp['flags'] & 1) << 3
    flags |= (inp['flags'] & 2) << 3
    flags |= (inp['flags'] & 4) << 3
    entry[0x4E] = flags
    want = {'entry': hexbytes(entry, wild), 'rest_ok': True, 'num': inp['index'] + 1, 'name_ok': True}
    ins = [X1, Y1, FX, FY, X2, Y2]
    stats = {'family': inp['kind']}
    stats['nan_in'] = any(v.is_nan() for v in ins)
    stats['snan_in'] = any(b & 0x7FC00000 == 0x7F800000 and b & 0x7FFFFF for b in (x1b, y1b, fxb, fyb, x2b, y2b))
    stats['inf_in'] = any(v.kind == 'inf' for v in ins)
    stats['negzero_in'] = any(v.is_zero() and v.neg for v in ins)
    stats['huge_in'] = any(v.kind == 'f' and abs(v.val) >= 10 ** 19 for v in ins)
    stats['tiny_in'] = any(v.kind == 'f' and v.val != 0 and abs(v.val) < Fraction(1, 10 ** 19) for v in ins)
    stats['finite_in'] = all(v.kind == 'f' for v in ins)
    stats['zero_dir_a'] = ax.kind == 'f' and ax.val == 0 and ay.kind == 'f' and ay.val == 0
    stats['zero_dir_b'] = bx.kind == 'f' and bx.val == 0 and by.kind == 'f' and by.val == 0
    stats['width_nan'] = dirs[0][2].is_nan()
    stats['height_nan'] = dirs[1][2].is_nan()
    stats['width_inf'] = dirs[0][2].kind == 'inf'
    stats['height_inf'] = dirs[1][2].kind == 'inf'
    stats['width_zero'] = dirs[0][2].is_zero()
    stats['height_zero'] = dirs[1][2].is_zero()
    stats['limits_nan'] = any(v.is_nan() for v in (left, right, front, back))
    stats['limits_unknown'] = any(v.kind == 'unk' for v in (left, right, front, back))
    stats['corner_x_left'] = left.kind == 'f' and cx.kind == 'f' and left.val == cx.val and not (X1.kind == 'f' and left.val == X1.val)
    stats['corner_x_right'] = right.kind == 'f' and cx.kind == 'f' and right.val == cx.val and not (X1.kind == 'f' and right.val == X1.val)
    stats['corner_y_front'] = front.kind == 'f' and cy.kind == 'f' and front.val == cy.val and not (Y1.kind == 'f' and front.val == Y1.val)
    stats['corner_y_back'] = back.kind == 'f' and cy.kind == 'f' and back.val == cy.val and not (Y1.kind == 'f' and back.val == Y1.val)
    stats['left_x2'] = left.kind == 'f' and X2.kind == 'f' and left.val == X2.val and not (X1.kind == 'f' and left.val == X1.val)
    stats['left_front'] = left.kind == 'f' and FX.kind == 'f' and left.val == FX.val and not (X1.kind == 'f' and left.val == X1.val)
    stats['right_x2'] = right.kind == 'f' and X2.kind == 'f' and right.val == X2.val and not (X1.kind == 'f' and right.val == X1.val)
    stats['right_front'] = right.kind == 'f' and FX.kind == 'f' and right.val == FX.val and not (X1.kind == 'f' and right.val == X1.val)
    stats['index_0'] = inp['index'] == 0
    stats['index_49'] = inp['index'] == MAX_GARAGES - 1
    stats['type_high_garbage'] = (inp['type'] >> 8) != 0
    stats['flags_high_garbage'] = (inp['flags'] >> 3) != 0
    stats['name_long'] = len(bytes.fromhex(inp['name'])) > 7
    stats['name_short'] = len(bytes.fromhex(inp['name'])) < 7
    stats['name_empty'] = len(bytes.fromhex(inp['name'])) == 0
    stats['name_high_bytes'] = any(b >= 0x80 for b in bytes.fromhex(inp['name'])[:7])
    stats['finite_dirs'] = all(v.kind == 'f' for v in (ax, ay, bx, by))
    stats['all_known'] = not wild
    for i in range(8):
        stats['flags_%d' % i] = (inp['flags'] & 7) == i
    return want, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    if set(obs) != set(want):
        return False
    for k, w in want.items():
        o = obs[k]
        if k == 'entry':
            if not match_hex(o, w):
                return False
        elif o != w:
            return False
    return True


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    flt = lambda b: struct.unpack('<f', struct.pack('<I', b))[0]
    u = lambda e, off: struct.unpack_from('<I', e, off)[0]

    def run(coords, z=(0, 0), typ=5, flags=0, name=b'abc', index=3, entry=None):
        inp = {'kind': 0, 'in': [coords[0], coords[1], z[0], coords[2], coords[3], coords[4], coords[5], z[1]], 'type': typ, 'a10': 0, 'flags': flags, 'name': name.hex(), 'index': index,
               'fill': 0xA5, 'entry': (entry or bytes([0x5A] * ENTRY_BYTES)).hex()}
        want, st = simulate(inp)
        return want, bytes.fromhex(want['entry'].replace('??', '00')), st

    one, two, three = bits(1.0), bits(2.0), bits(3.0)
    # rectangulo alineado: x1 = 1, y1 = 1, front = (3, 1), x2 = (1), y2 = 2  -> direcciones A = (2, 0) / 2, B = (0, 1) / 1
    want, e, st = run([one, one, three, one, one, two])
    assert flt(u(e, 0x28)) == 1.0 and flt(u(e, 0x2C)) == 3.0 and flt(u(e, 0x30)) == 1.0 and flt(u(e, 0x34)) == 2.0, [flt(u(e, o)) for o in (0x28, 0x2C, 0x30, 0x34)]
    assert (flt(u(e, 0x0C)), flt(u(e, 0x10)), flt(u(e, 0x20))) == (1.0, 0.0, 2.0)
    assert (flt(u(e, 0x14)), flt(u(e, 0x18)), flt(u(e, 0x24))) == (0.0, 1.0, 1.0)
    assert u(e, 0) == one and u(e, 4) == one and u(e, 0x1C) == 0
    assert e[0x44:0x4B] == b'abc' + bytes(4) and e[0x4B] == 0x5A and e[0x4C] == 5 and e[0x4F] == 5
    assert e[0x4E] == (0x5A & 0xC7) and want['num'] == 4
    # flags: bit 0 -> 0x08, bit 1 -> 0x10, bit 2 -> 0x20, el resto se conserva; type usa solo el byte bajo
    for fl_ in range(8):
        want, e, st = run([one, one, three, one, one, two], flags=fl_ | 0x1234560, typ=0x12345607)
        assert e[0x4E] == (0x5A & 0xC7) | (fl_ << 3) and e[0x4C] == 7, (fl_, hex(e[0x4E]))
    # nombre de 8 o mas caracteres: solo 7, sin terminador
    want, e, st = run([one, one, three, one, one, two], name=b'ABCDEFGHIJ')
    assert e[0x44:0x4B] == b'ABCDEFG' and e[0x4B] == 0x5A
    # cuarta esquina decide: x1 = 0, front = (4, 0), x2 = (1, 3) -> d = 4 + 1 - 0 = 5 > 4: right = 5
    want, e, st = run([0, 0, bits(4.0), 0, bits(1.0), bits(3.0)])
    assert flt(u(e, 0x2C)) == 5.0 and st['corner_x_right']
    # NaN: x1 NaN -> (x1 < fx) falso => m = fx ; cx = NaN ; (m < cx) falso => left = cx (NaN, el de x1)
    nanb = 0x7FC00001
    want, e, st = run([nanb, one, three, one, one, two])
    assert u(e, 0x28) == nanb and st['nan_in']
    # sNaN en las comparaciones se silencia
    want, e, st = run([0x7F800001, one, three, one, one, two])
    assert u(e, 0x28) == 0x7FC00001 and u(e, 0x00) == 0x7F800001
    # longitud cero: A = (0, 0) -> s = 0, 0/0 = indefinido, anchura 0
    want, e, st = run([one, one, one, one, three, two])
    assert u(e, 0x20) == 0 and u(e, 0x0C) == 0xFFC00000 and u(e, 0x10) == 0xFFC00000 and st['zero_dir_a']
    # 24 bits: 16777216 + 1 + 1 no se acumula en 24 bits (cada suma redondea al par)
    want, e, st = run([0, 0, bits(16777216.0), 0, one, 0])
    assert flt(u(e, 0x2C)) == 16777216.0
    # exponente extendido: 3e38 + 3e38 - 3e38 = 3e38 (en float puro desbordaria a inf)
    big = bits(3.0e38)
    want, e, st = run([big, 0, big, 0, big, 0])
    assert flt(u(e, 0x2C)) == f32(3.0e38) and st['huge_in']
    # cuadrados que desbordan float: dx = 3e19 -> s = 3e19 (en float inf)
    want, e, st = run([0, 0, bits(3.0e19), 0, 0, one])
    assert abs(flt(u(e, 0x20)) - 3.0e19) < 1e13 and flt(u(e, 0x0C)) == 1.0, (flt(u(e, 0x20)), flt(u(e, 0x0C)))
    # coherencia con una cadena float32 por operacion (valores tipicos; el doble redondeo es despreciable salvo empates exactos)
    rng = random.Random(30)
    import math
    bad = 0
    for _ in range(3000):
        v = [f32(rng.uniform(-3000, 3000)) for _ in range(6)]
        want, e, st = run([bits(x) for x in v])
        x1, y1, fx, fy, x2, y2 = v
        cxf, cyf = f32(f32(fx + x2) - x1), f32(f32(fy + y2) - y1)
        exp = [min(x1, fx, x2, cxf), max(x1, fx, x2, cxf), min(y1, fy, y2, cyf), max(y1, fy, y2, cyf)]
        got = [flt(u(e, o)) for o in (0x28, 0x2C, 0x30, 0x34)]
        assert got == exp, (v, got, exp)
        ax, ay = f32(fx - x1), f32(fy - y1)
        s = f32(math.sqrt(f32(f32(ay * ay) + f32(ax * ax))))
        if (flt(u(e, 0x20)), flt(u(e, 0x0C)), flt(u(e, 0x10))) != (s, f32(ax / s), f32(ay / s)):
            bad += 1
    assert bad <= 30, bad
    assert matches({'entry': 'abcdab', 'rest_ok': True, 'num': 1, 'name_ok': True}, {'entry': 'ab??ab', 'rest_ok': True, 'num': 1, 'name_ok': True})
    assert not matches({'entry': 'abcdab', 'rest_ok': True, 'num': 2, 'name_ok': True}, {'entry': 'ab??ab', 'rest_ok': True, 'num': 1, 'name_ok': True})
    print('ok')


def crosscheck(path):
    """Autoprueba cruzada: el volcado de Fixtures.hpp (fixture_dump.cpp compilado con cl) repite las constantes y produce filas que el oraculo acepta."""
    lines = Path(path).read_text(encoding='utf-8').splitlines()
    consts = json.loads(lines[0])
    assert consts == CONSTANTS, (consts, CONSTANTS)
    rows = [json.loads(s) for s in lines[1:]]
    assert len(rows) == CASES
    for r in rows:
        assert len(r['entry']) == 2 * ENTRY_BYTES and 0 <= r['index'] < MAX_GARAGES and len(bytes.fromhex(r['name'])) <= NAME_ARENA - NAME_OFFSET - 1 and 0 not in bytes.fromhex(r['name'])
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--crosscheck':
        crosscheck(sys.argv[2])
    else:
        selftest()
