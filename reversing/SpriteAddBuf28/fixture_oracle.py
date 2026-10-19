"""Oraculo independiente del banco SpriteAddBuf28: CSprite2d::AddToBuffer (0x728200).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. Es aritmetica entera de 32/16 bits (no hay x87):

  728200  esi = [C8045C] (nextBufferVertex, LEIDO ANTES de la llamada)
          push posn, colorx4, u1..v4 ; push &aRadiosityVertexBuffer[esi]  (= 0xC5F958 + esi*0x1C, imul de 32 bits)
  727A00  call SetVertices(ptr, posn, color, color, color, color, u1, v1, u2, v2, u3, v3, u4, v4)       (colaborador, falso en el banco)
  72824C  eax = [C80458] (nextBufferIndex, LEIDO DESPUES de la llamada)
          word[C4B958 + eax*2 + 2] = esi+1 ; word[+0] = esi ; word[+8] = esi ; word[+4] = esi+2 ; word[+6] = esi+3 ; word[+0xA] = esi+2        (truncados a 16 bits)
          [C8045C] = esi+4 ; [C80458] = eax+6
  728299  cmp esi,5FCh ; jg -> jmp 7273D0 (RenderVertexBuffer, llamada de cola) ; cmp eax,0FFAh ; jle ret ; (comparaciones CON SIGNO, sobre los valores YA incrementados)
  7273D0  RenderVertexBuffer: si [C8045C] > 0: dibuja y pone ambos contadores a 0 (con signo: <= 0 no hace nada)

El banco sustituye SetVertices (0x727A00) y RenderVertexBuffer (0x7273D0) por falsos identicos en EXE y DLL (ver Core.hpp). Convenios de la traza:
  SetVertices: [3, off, rect0..3, dword(color1..4), mascara, u1..v4] con off = (ptr-0xC5F958) mod 2^32; escribe 28 dwords Mix(h+k*0x9E3779B1) si off <= 42896-112;
               h = cadena Mix(h ^ w) (w en traza[1:]) desde 0x811C9DC5.
  Render:      [2, nextBufferVertex, nextBufferIndex, crc32(vertices usados), crc32(indices usados)]; reinicia contadores solo si nextBufferVertex > 0.
`python -I fixture_oracle.py` ejecuta la autoprueba (sin juego) y muestra la cobertura que reparten las fixtures (make_f es un espejo EXACTO de Fixtures.hpp).
"""
import json
import struct
import sys
import zlib

M32 = 0xFFFFFFFF
CASES = 4096
NAME = 'CSprite2d::AddToBuffer'
NAMES = [NAME]
K_VERTEX_BUFFER = 0xC5F958
K_INDEX_BUFFER = 0xC4B958
VERTEX_BYTES, VERTEX_COUNT, INDEX_COUNT = 28, 1532, 4096
VERTEX_BUFFER_BYTES, INDEX_BUFFER_BYTES, WINDOW_BYTES = VERTEX_BYTES * VERTEX_COUNT, INDEX_COUNT * 2, 4 * VERTEX_BYTES


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


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


def quiet(b):
    if (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) and not (b & 0x400000):
        b |= 0x400000
    return b


def f2b(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


STACK_SEEDS = [0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555]
SPECIAL_FLOATS = [0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0x7F800000, 0xFF800000, 0x7FC00000, 0xFFC00000, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001,
                  0x80000001, 0x00800000, 0x3F800001, 0x447A0000, 0x44800000, 0xC4800000, 0x3C23D70A, 0x4B000000, 0x7FC00001, 0x42C80000, 0x43000000, 0x3FC00000]
SIGNALING_NANS = [0x7F800001, 0x7FA00000, 0xFF800001, 0xFFBFFFFF]
COLORS = [0x00000000, 0xFFFFFFFF, 0xFF000000, 0x000000FF, 0x01020304, 0x80808080, 0xFF00FF00, 0x00FF00FF, 0xFEFEFEFE, 0x7F7F7F7F, 0x0000FF00, 0x00FF0000]
WEIRD_VERTICES = [-1, -2, -3, -4, -5, -1000, -0x80000000, -0x7FFFFFFF, 0xFFFC, 0xFFFD, 0xFFFE, 0xFFFF, 0x10000, 0x10001, 0x10003, 100000, 0x7FFFFFFB, 0x7FFFFFF0,
                  0x12345678, -0x12345678, 65532, 70000, 0x40000000, -65536]
assert len(SPECIAL_FLOATS) == 24 and len(COLORS) == 12 and len(WEIRD_VERTICES) == 24


def pick_float(c, slot):
    r = rand(c, 200 + slot * 4) % 16
    if r < 6:
        return SPECIAL_FLOATS[rand(c, 201 + slot * 4) % len(SPECIAL_FLOATS)]
    if r < 8:
        return quiet(rand(c, 202 + slot * 4))
    return f2b(float((rand(c, 203 + slot * 4) % 40000) - 10000) / 16.0)


def make_f(n):
    """Espejo exacto de SpriteAddBuf28Fix::MakeF (Fixtures.hpp), devuelto como la fila `input` que escribe el banco."""
    c = n
    kind = n % 32
    r1, r2 = rand(c, 1), rand(c, 2)
    if kind == 0:
        vertex, index = 0, 0
    elif kind == 1:
        vertex, index = 1528, r2 % 4000
    elif kind == 2:
        vertex, index = 1529, r2 % 4000
    elif kind == 3:
        vertex, index = 1529 + r1 % 7, r2 % 4085
    elif kind == 4:
        vertex, index = r1 % 1500, 4084
    elif kind == 5:
        vertex, index = r1 % 1500, 4085
    elif kind == 6:
        vertex, index = r1 % 1500, 4086 + r2 % 5
    elif kind == 7:
        vertex, index = 1529 + r1 % 7, 4085 + r2 % 6
    elif kind == 8:
        vertex, index = r1 % 1529, 0
    elif kind == 9:
        vertex, index = WEIRD_VERTICES[r1 % len(WEIRD_VERTICES)], r2 % 4085
    elif kind == 10:
        vertex, index = WEIRD_VERTICES[r1 % len(WEIRD_VERTICES)], 4085 + r2 % 6
    elif kind == 11:
        vertex, index = 1527, 4084
    elif kind == 12:
        vertex, index = 4 * (r1 % 382), 6 * (r2 % 681)
    elif kind == 13:
        vertex, index = 1524 + r1 % 5, 4078 + r2 % 7
    else:
        vertex, index = rand(c, 3) % 1536, rand(c, 4) % 4091
    rect = [pick_float(c, k) for k in range(4)]
    uv = [pick_float(c, 4 + k) for k in range(8)]
    if n % 64 == 5:
        slot = rand(c, 60) % 12
        v = SIGNALING_NANS[rand(c, 61) % len(SIGNALING_NANS)]
        if slot < 4:
            rect[slot] = v
        else:
            uv[slot - 4] = v
    color = COLORS[rand(c, 70) % len(COLORS)] if n % 4 == 0 else rand(c, 71)
    stack = STACK_SEEDS[n] if n < len(STACK_SEEDS) else rand(n + 0x1000, 99)
    return {'kind': kind, 'vertex': s32(vertex), 'index': index, 'rect': rect, 'color': color, 'uv': uv, 'vseed': rand(c, 80) & 0xFF, 'iseed': rand(c, 81) & 0xFF,
            'eax': rand(c, 90), 'ecx': rand(c, 91), 'edx': rand(c, 92), 'stack_seed': stack}


def pattern(size, mult, seed):
    base = bytes(((i * mult + seed) & 0xFF) for i in range(256))
    return bytearray((base * (size // 256 + 1))[:size])


def expected(inp, case):
    v = s32(inp['vertex'])
    idx = inp['index']
    assert 0 <= idx <= INDEX_COUNT - 6, 'fixture invalida: el indice rebasaria el bufer'
    vseed, iseed = inp['vseed'], inp['iseed']
    # ---- SetVertices (falso)
    off = (v * VERTEX_BYTES) & M32
    color = inp['color'] & M32
    words = [off] + [x & M32 for x in inp['rect']] + [color] * 4 + [15] + [x & M32 for x in inp['uv']]
    trace = [3] + words
    h = 0x811C9DC5
    for w in words:
        h = mix(h ^ w)
    vbuf = pattern(VERTEX_BUFFER_BYTES, 7, vseed)
    v_in_buffer = off <= VERTEX_BUFFER_BYTES - WINDOW_BYTES
    if v_in_buffer:
        vbuf[off:off + WINDOW_BYTES] = b''.join(struct.pack('<I', mix((h + k * 0x9E3779B1) & M32)) for k in range(28))
    # ---- indices (seis words) y contadores
    ibuf = pattern(INDEX_BUFFER_BYTES, 13, iseed)
    for k, val in ((1, v + 1), (0, v), (4, v), (2, v + 2), (3, v + 3), (5, v + 2)):
        struct.pack_into('<H', ibuf, (idx + k) * 2, val & 0xFFFF)
    v2, i2 = v + 4, idx + 6
    stats = {'ran': True}
    full_v, full_i = v2 > VERTEX_COUNT, i2 > 0xFFA
    stats.update(full_v=full_v, full_i=full_i, both=full_v and full_i, neither=not (full_v or full_i), v_oob_window=not v_in_buffer, v_negative=v < 0,
                 v_huge=v > 0xFFFF, v_trunc16=(v + 3) > 0xFFFF or v < 0, v_edge=v in (1527, 1528, 1529), i_edge=idx in (4084, 4085), v_zero=v == 0, i_zero=idx == 0)
    calls = [1, 0]
    reset = False
    if full_v or full_i:
        calls[1] = 1
        nv = 0 if v2 <= 0 else min(v2, VERTEX_COUNT)
        ni = 0 if i2 <= 0 else min(i2, INDEX_COUNT)
        trace += [2, v2 & M32, i2 & M32, zlib.crc32(bytes(vbuf[:nv * VERTEX_BYTES])), zlib.crc32(bytes(ibuf[:ni * 2]))]
        if v2 > 0:
            reset = True
            v2, i2 = 0, 0
    stats['rendered'] = calls[1] == 1
    stats['reset'] = reset
    stats['render_noreset'] = calls[1] == 1 and not reset
    obs = {'vertex_after': s32(v2), 'index_after': s32(i2),
           'index_window': bytes(ibuf[idx * 2:idx * 2 + 12]).hex(),
           'vertex_window': bytes(vbuf[off:off + WINDOW_BYTES]).hex() if v_in_buffer else '',
           'outside_ok': True, 'guard_ok': True, 'trace': trace, 'calls': calls}
    return obs, stats


def coverage():
    cov = {}
    for n in range(CASES):
        _, st = expected(make_f(n), n)
        for k, val in st.items():
            cov[k] = cov.get(k, 0) + int(bool(val))
        cov['kind%d' % (n % 32)] = cov.get('kind%d' % (n % 32), 0) + 1
    return cov


def selftest():
    assert zlib.crc32(b'123456789') == 0xCBF43926
    # caso a mano: bufer vacio
    inp = {'vertex': 0, 'index': 0, 'rect': [0, 1, 2, 3], 'color': 0x11223344, 'uv': list(range(10, 18)), 'vseed': 5, 'iseed': 9}
    obs, st = expected(inp, 0)
    assert obs['vertex_after'] == 4 and obs['index_after'] == 6 and obs['calls'] == [1, 0]
    w = [struct.unpack_from('<H', bytes.fromhex(obs['index_window']), 2 * k)[0] for k in range(6)]
    assert w == [0, 1, 2, 3, 0, 2], w
    assert len(obs['trace']) == 19 and obs['trace'][:2] == [3, 0] and len(obs['vertex_window']) == 224
    # lleno por vertices (1529): Render con contadores 1533/idx+6, ventana fuera del bufer
    obs, st = expected(dict(inp, vertex=1529, index=10), 0)
    assert obs['calls'] == [1, 1] and obs['vertex_after'] == 0 and obs['index_after'] == 0 and obs['vertex_window'] == ''
    assert obs['trace'][19:22] == [2, 1533, 16], obs['trace'][19:22]
    # 1528: 1532 no esta lleno
    obs, st = expected(dict(inp, vertex=1528, index=4084), 0)
    assert obs['calls'] == [1, 0] and obs['vertex_after'] == 1532 and obs['index_after'] == 4090
    # indices llenos con contadores de vertice negativos: Render no reinicia
    obs, st = expected(dict(inp, vertex=-100, index=4085), 0)
    assert obs['calls'] == [1, 1] and obs['vertex_after'] == -96 and obs['index_after'] == 4091 and st['render_noreset']
    # vertice truncado a 16 bits
    obs, st = expected(dict(inp, vertex=0xFFFE, index=0), 0)
    w = [struct.unpack_from('<H', bytes.fromhex(obs['index_window']), 2 * k)[0] for k in range(6)]
    assert w == [0xFFFE, 0xFFFF, 0, 1, 0xFFFE, 0] and obs['calls'] == [1, 1] and st['reset']
    # todas las fixtures son validas para el oraculo
    for n in range(CASES):
        expected(make_f(n), n)
    print('ok')


if __name__ == '__main__':
    selftest()
    print(json.dumps(coverage(), indent=1, sort_keys=True))
