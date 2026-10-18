"""Fixtures y oraculo independiente del banco Dependencies25C (20 funciones de biblioteca de RenderWare).

Las tablas y constantes se leen de Fixtures.hpp (una sola fuente); las formulas de los casos (Make) y la semantica de las 20 funciones, modelada desde el desensamblado del EXE
(ver original-disassembly.txt), se repiten aqui en Python. El oraculo NO usa el arena de C++: calcula el valor de retorno, la traza de colaboradores y los cambios de memoria
unicamente a partir de la fixture (los campos que importan se colocan en el arena con valores explicitos, el resto es relleno).
`python -B fixture_oracle.py --dump N` imprime las fixtures de los casos 0..N-1 de cada funcion en JSON para compararlas con el registro (README.md, Comprobacion cruzada).
"""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
TEXT = (HERE / 'Fixtures.hpp').read_text(encoding='utf-8')
M32 = 0xFFFFFFFF


def table(name):
    body = re.search(name + r'\{(.*?)\};', TEXT, re.S).group(1)
    return [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|[0-9]+', body)]


def const(name):
    return int(re.search(name + r'[ ]*=[ ]*(0x[0-9A-Fa-f]+|[0-9]+)', TEXT).group(1), 0)


W32, RET, FLOAT, STEP = table('kW32'), table('kRet'), table('kFloat'), table('kStep')
assert (len(W32), len(RET), len(FLOAT), len(STEP)) == (32, 16, 24, 12)
ARENA = const('kArena')
ENGINE_BLOCK, MESH_BASE, STRUCT_BASE = const('kEngineBlock'), const('kMeshBase'), const('kStructBase')
CAMERA, OFFSET, PARENT = const('kCameraBase'), const('kOffsetBase'), const('kParentBase')
FRAME, ROOT0, ROOT1, FRAME_ALT = const('kFrameBase'), const('kRootOther0'), const('kRootOther1'), const('kFrameAlt')
HANIM_FRAME0, HANIM_TARGET0 = const('kHAnimFrame0'), const('kHAnimTarget0')

NAMES = ['RpWorld::_rpMeshHeaderCreate', 'RpWorld::RpBuildMeshGenerateDefaultTriStrip', 'RpWorld::RxNodeDefinitionGetD3D9WorldSectorAllInOne', 'RpWorld::RpWorldSectorRegisterPlugin',
         'RpWorld::RpWorldSectorRegisterPluginStream', 'RpHAnim::RpHAnimFrameGetHierarchy', 'RpSkin::_rpSkinDeinitialize', 'RpUVAnim::_rpUVAnimCustomDataStreamGetSize',
         'RpUVAnim::RpUVAnimKeyFrameStreamGetSize', 'RtAnim::RtAnimAnimationGetNumNodes', 'RtAnim::RtAnimInterpolatorDestroy', 'RtDict::RtDictSchemaDestruct',
         'RtDict::RtDictSchemaGetCurrentDict', 'RwPlCore::RwStreamWriteInt32', 'RwCore::RwCameraSetViewOffset', 'RwCore::RwCameraRegisterPlugin', 'RwCore::RwFrameDirty',
         'RwPlCore::RwMatrixOrthoNormalize', 'RwPlCore::RwEngineRegisterPlugin', 'RwPlCore::RwEngineGetPluginOffset']
(MESH, BUILD, NODE, SECTOR, SECTOR_STREAM, HANIM, SKIN, UV_SIZE, UV_KEYS, NUM_NODES, INTERP_DESTROY, DICT_DESTRUCT, DICT_CURRENT, STREAM_WRITE, VIEW_OFFSET, CAMERA_REG,
 FRAME_DIRTY, ORTHO, ENGINE_REG, ENGINE_OFFSET) = range(20)
CASES = [int(x) for x in re.search(r'kCases\{(.*?)\};', TEXT, re.S).group(1).split(',')]
assert len(CASES) == 20 and sum(CASES) == 23296
TOTAL = sum(CASES)
EXPECTED = {NAMES[i]: CASES[i] for i in range(20)}
# Direcciones del EXE que reciben los colaboradores como primer argumento
REG_SECTOR, REG_CAMERA, REG_ENGINE = 0x8D736C, 0x8E222C, 0x8E2298
NODE_DEF = 0x8D63B8
MESH_HINT = 0x30502
# Ids de colaborador (Core.hpp)
C_MALLOC, C_FREE, C_MESHGEN, C_ADD, C_ADDSTREAM, C_GETOFFSET, C_STREAMWRITE, C_SLIST, C_UPDATE, C_ORTHO, C_UV = range(1, 12)


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 0x2C1B3C6D) & M32)


def w(c, k):
    if c % 4 == 0:
        return rand(c, 10 + k)
    return W32[rand(c, 30 + k) % 32] if rand(c, 20 + k) & 1 else rand(c, 40 + k)


def ret_of(c):
    return rand(c, 5) if c % 4 == 0 else RET[rand(c, 6) % 16]


RAW, PTR = 0, 1


def make(fn, c):
    """Misma formula que Make() de Fixtures.hpp; devuelve el diccionario que InputJson() escribe en el registro."""
    a = []
    x = [0] * 6
    if fn == MESH:
        a = [[RAW, w(c, 0)]]
    elif fn == BUILD:
        a = [[PTR, MESH_BASE + 4 * (rand(c, 50) % 64)] if c % 3 == 0 else [RAW, w(c, 0)], [RAW, w(c, 1)]]
    elif fn == NODE:
        a = []
    elif fn in (SECTOR, CAMERA_REG):
        a = [[RAW, w(c, i)] for i in range(5)]
    elif fn == SECTOR_STREAM:
        a = [[RAW, w(c, i)] for i in range(4)]
    elif fn == HANIM:
        x[0] = HANIM_TARGET0 + 4 * (rand(c, 60) % 256)
        x[1] = w(c, 7)
        x[2] = rand(c, 61) % 3
        a = [[PTR, HANIM_FRAME0 + 4 * (rand(c, 62) % 32)] if x[2] == 0 else [RAW, rand(c, 63)] if x[2] == 1 else [RAW, 0]]
    elif fn in (SKIN, UV_SIZE, INTERP_DESTROY):
        a = [[RAW, w(c, 0)]]
    elif fn == UV_KEYS:
        a = [[PTR, STRUCT_BASE + 4 * (rand(c, 50) % 32)]]
        x[0], x[1], x[2] = w(c, 1), w(c, 2), c % 6
    elif fn == NUM_NODES:
        m = (c // 12) % 13
        s = STEP[c % 12] if m == 0 else STEP[1 + c % 11]
        x[0], x[1], x[2] = s, m, c // 156
        a = [[PTR, 0x40 + 0x40 * (x[2] % 2)]]
    elif fn in (DICT_DESTRUCT, DICT_CURRENT):
        a = [[PTR, STRUCT_BASE + 4 * (rand(c, 50) % 32)]]
        x[0] = w(c, 1)
    elif fn == STREAM_WRITE:
        a = [[RAW, w(c, i)] for i in range(3)]
    elif fn == VIEW_OFFSET:
        x[0], x[1], x[2], x[3] = FLOAT[c % 24], FLOAT[(c // 24) % 24], (c // 576) % 4, (c + c // 24 + c // 576) % 3
        a = [[PTR, CAMERA], [PTR, OFFSET if x[2] == 0 else CAMERA + 0x78 if x[2] == 1 else CAMERA + 0x7C if x[2] == 2 else CAMERA + 0x74]]
    elif fn == FRAME_DIRTY:
        x[0], x[1] = c % 256, (c // 256) % 4
        a = [[PTR, FRAME_ALT if x[1] == 3 else FRAME]]
    elif fn == ORTHO:
        a = [[PTR, MESH_BASE + 16 * (rand(c, 50) % 32)] if c % 3 == 0 else [RAW, w(c, 0)], [PTR, MESH_BASE + 16 * (rand(c, 51) % 32)] if c % 5 == 0 else [RAW, w(c, 1)]]
    elif fn == ENGINE_REG:
        a = [[RAW, w(c, i)] for i in range(4)]
    elif fn == ENGINE_OFFSET:
        a = [[RAW, w(c, 0)]]
    return {'fn': fn, 'c': c, 'seed': [rand(c, 1), rand(c, 2), rand(c, 3)], 'prime': rand(c, 4), 'ret': ret_of(c), 'a': a, 'x': x}


def enc(spec):
    """Como Enc() de Core.hpp: un valor dentro del arena se registra como desplazamiento con el bit 32 activo."""
    return (1 << 32) | spec[1] if spec[0] == PTR else spec[1]


def raw(v):
    return v


def camera_words(f):
    """Palabras de la camara +0x74..+0x87 (5) antes de la llamada segun el oraculo: relleno aleatorio salvo los campos que coloca la fixture."""
    words = {o: rand(f['c'], 1000 + (CAMERA + o) // 4) for o in range(0x74, 0x88, 4)}
    X, Y, alias = f['x'][0], f['x'][1], f['x'][2]
    if alias == 1:
        words[0x78], words[0x7C] = X, Y
    elif alias == 2:
        words[0x7C], words[0x80] = X, Y
    elif alias == 3:
        words[0x74], words[0x78] = X, Y
    return [words[o] for o in range(0x74, 0x88, 4)]


def view_offset_after(f):
    """Repite el orden del EXE: lee offset.x, escribe viewOffset.x, lee offset.y, escribe viewOffset.y (importa si se solapan)."""
    words = dict(zip(range(0x74, 0x88, 4), camera_words(f)))
    alias = f['x'][2]
    if alias == 0:
        read = {0: f['x'][0], 4: f['x'][1]}
        words[0x78] = read[0]
        words[0x7C] = read[4]
    else:
        src = {1: 0x78, 2: 0x7C, 3: 0x74}[alias]
        words[0x78] = words[src]
        words[0x7C] = words[src + 4]
    return [words[o] for o in range(0x74, 0x88, 4)]


def expected(f, base=0):
    """Resultado que modela el oraculo desde el desensamblado: (eax | None, traza). `base`: direccion del arena (para los punteros absolutos de EAX)."""
    fn, a, x, ret = f['fn'], f['a'], f['x'], f['ret']
    A = [enc(s) for s in a]
    if fn == MESH:
        return ret, [[C_MALLOC, A[0], MESH_HINT]]
    if fn == BUILD:
        return ret, [[C_MESHGEN, A[0], 0, 1]]
    if fn == NODE:
        return NODE_DEF, []
    if fn == SECTOR:
        return ret, [[C_ADD, REG_SECTOR] + A]
    if fn == SECTOR_STREAM:
        return ret, [[C_ADDSTREAM, REG_SECTOR] + A]
    if fn == HANIM:
        return x[1], []
    if fn == SKIN:
        return a[0][1], []
    if fn == UV_SIZE:
        return 0x40, []
    if fn == UV_KEYS:
        r = 0x40 if x[2] == 5 else ret
        return ((x[0] << 5) + r + 4) & M32, [] if x[2] == 5 else [[C_UV, x[1]]]
    if fn == NUM_NODES:
        return x[1], []
    if fn == INTERP_DESTROY:
        return None, [[C_FREE, A[0]]]
    if fn == DICT_DESTRUCT:
        return 1, [[C_SLIST, x[0]]]
    if fn == DICT_CURRENT:
        return x[0], []
    if fn == STREAM_WRITE:
        return a[0][1], [[C_STREAMWRITE] + A]
    if fn == VIEW_OFFSET:
        parent = [] if x[3] == 0 else [[C_UPDATE, (1 << 32) | PARENT]] if x[3] == 1 else [[C_UPDATE, rand(f['c'], 70) | 1]]
        return (base + CAMERA) & M32, parent
    if fn == CAMERA_REG:
        return ret, [[C_ADD, REG_CAMERA] + A]
    if fn == FRAME_DIRTY:
        return x[0] & 3, []
    if fn == ORTHO:
        return ret, [[C_ORTHO] + A]
    if fn == ENGINE_REG:
        return ret, [[C_ADD, REG_ENGINE] + A + [0]]
    return ret, [[C_GETOFFSET, REG_ENGINE] + A]


def coverage(fn):
    """Cobertura declarada de las fixtures (la que run_local.py exige tambien en el registro)."""
    cov = {}
    fx = [make(fn, c) for c in range(CASES[fn])]
    if fn == MESH:
        cov['size_extremes'] = sum(1 for f in fx if f['a'][0][1] in (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF))
        cov['ret_variants'] = len({f['ret'] for f in fx})
    elif fn == HANIM:
        cov.update({'frame_arena': sum(1 for f in fx if f['x'][2] == 0), 'frame_raw': sum(1 for f in fx if f['x'][2] == 1), 'frame_null': sum(1 for f in fx if f['x'][2] == 2),
                    'offset_wraps': sum(1 for f in fx if ((0x10000000 + f['x'][0] - 4 - (f['a'][0][1] if f['a'][0][0] == RAW else 0x10000000 + f['a'][0][1])) & M32) >= 0x80000000)})
    elif fn == UV_KEYS:
        cov.update({'real_collaborator': sum(1 for f in fx if f['x'][2] == 5), 'shift_overflow': sum(1 for f in fx if f['x'][0] >= 0x08000000),
                    'sum_overflow': sum(1 for f in fx if f['x'][2] != 5 and ((f['x'][0] << 5) & M32) + f['ret'] + 4 > M32)})
    elif fn == NUM_NODES:
        cov.update({'zero_nodes': sum(1 for f in fx if f['x'][1] == 0), 'many_nodes': sum(1 for f in fx if f['x'][1] >= 10), 'step_zero': sum(1 for f in fx if f['x'][0] == 0),
                    'steps': len({f['x'][0] for f in fx}), 'nodes': len({f['x'][1] for f in fx}), 'layouts': len({f['x'][2] for f in fx})})
    elif fn == VIEW_OFFSET:
        nan = lambda v: (v & 0x7F800000) == 0x7F800000 and (v & 0x007FFFFF)
        cov.update({'alias%d' % k: sum(1 for f in fx if f['x'][2] == k) for k in range(4)})
        cov.update({'parent%d' % k: sum(1 for f in fx if f['x'][3] == k) for k in range(3)})
        cov.update({'nan_x': sum(1 for f in fx if nan(f['x'][0])), 'snan': sum(1 for f in fx if nan(f['x'][0]) and not f['x'][0] & 0x00400000),
                    'subnormal': sum(1 for f in fx if 0 < f['x'][0] & 0x7FFFFFFF < 0x00800000), 'inf': sum(1 for f in fx if f['x'][0] & 0x7FFFFFFF == 0x7F800000),
                    'negzero': sum(1 for f in fx if f['x'][0] == 0x80000000)})
    elif fn == FRAME_DIRTY:
        cov.update({'flags': len({f['x'][0] for f in fx}), 'root_self': sum(1 for f in fx if f['x'][1] in (0, 3)), 'root_other': sum(1 for f in fx if f['x'][1] in (1, 2))})
    elif fn in (SECTOR, SECTOR_STREAM, CAMERA_REG, ENGINE_REG, ENGINE_OFFSET, STREAM_WRITE, BUILD, ORTHO, SKIN, UV_SIZE, INTERP_DESTROY):
        cov['distinct_args'] = sum(1 for f in fx if len({s[1] for s in f['a']}) == len(f['a']))
        cov['ret_variants'] = len({f['ret'] for f in fx})
    elif fn in (DICT_DESTRUCT, DICT_CURRENT):
        cov['word_variants'] = len({f['x'][0] for f in fx})
    return cov


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--dump':
        n = int(sys.argv[2])
        print(json.dumps({NAMES[fn]: [make(fn, c) for c in range(min(n, CASES[fn]))] for fn in range(20)}))
    else:
        print(json.dumps({NAMES[fn]: coverage(fn) for fn in range(20)}, indent=1))
