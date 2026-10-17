"""Fixtures y oraculo independiente del banco Dependencies21G.

Las tablas se leen de Fixtures.hpp (una sola fuente); las formulas de los casos, la construccion de los arenas (Arenas.hpp) y la semantica de las tres
funciones se repiten aqui en Python:
  * GetFirstTexture (0x734940): primera textura de la lista del diccionario o nulo.
  * RpGeometryReplaceOldMaterialWithNewMaterial (0x734DE0): el recorrido del EXE (sin firstMeshOffset), FindIndex -> Destroy -> materials[i] = new -> mesh = new -> ++new.refCount.
  * CQuaternion::Slerp (0x59C300): solo la rama theta == 0 (copia de `to`) tiene oraculo de valores; el resto se decide por igualdad EXE/DLL (x87 de 24 bits).
`python -B fixture_oracle.py --dump` imprime las fixtures de los casos 0..N en JSON para compararlas con el volcado de C++ (README.md, seccion Comprobacion cruzada).
"""
import json
import re
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
TEXT = (HERE / 'Fixtures.hpp').read_text(encoding='utf-8')
M32 = 0xFFFFFFFF
SLERP_CASES, FIRST_TEXTURE_CASES, REPLACE_CASES = 4096, 1024, 1536


def table(name):
    body = re.search(r'kQuat\{\{(.*?)\}\};' if name == 'kQuat' else name + r'\{(.*?)\};', TEXT, re.S).group(1)
    values = [int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]+', body)]
    return values


THETA, T, INV = table('kTheta'), table('kT'), table('kInv')
QUAT_FLAT = table('kQuat')
QUAT = [QUAT_FLAT[i * 8:(i + 1) * 8] for i in range(16)]
REFS = table('kRefCounts')
assert len(THETA) == 32 and len(T) == 16 and len(INV) == 8 and len(QUAT_FLAT) == 128 and len(REFS) == 8


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


# ------------------------------------------------------------------ Slerp
def slerp_fixture(c):
    q = (c * 5 + c // 32) % 16
    return dict(case=c, theta=THETA[c % 32], t=T[c // 32 % 16], inv=INV[c // 512 % 8], quat=q, alias=(c // 3 + c // 7) % 3,
                src=QUAT[q][:4], dst=QUAT[q][4:])


def slerp_path(c):
    """'copy' si theta es +-0 (rama que copia `to`), 'math' en cualquier otro caso (incluidos NaN)."""
    return 'copy' if THETA[c % 32] & 0x7FFFFFFF == 0 else 'math'


# ------------------------------------------------------------------ GetFirstTexture
TEX_ARENA, TEX_DICT, TEX_SLOT0, TEX_SLOT = 0x600, 0x10, 0x100, 0x80
MUL = (1, 8, 5, 2, 4, 7)


def first_texture_fixture(c):
    return dict(case=c, count=c % 9, pad=c // 9 % 4, order=c // 36 % 6, junk=c // 216 % 5, prev=c // 7 % 2)


def junk_word(junk, index):
    return (0, 0xFFFFFFFF, 0xA5A5A5A5, 0x5A5A5A5A)[junk] if junk < 4 else (0xC3000000 | (index & 0xFFFFFF))


def texture_slot(order, i):
    return (i * MUL[order]) % 9


def build_first_texture(base, f):
    a = bytearray(TEX_ARENA)

    def put(at, v):
        a[at:at + 4] = struct.pack('<I', v & M32)

    for i in range(0, TEX_ARENA, 4):
        put(i, junk_word(f['junk'], i // 4))
    dict_at = TEX_DICT + 4 * f['pad']
    sentinel = dict_at + 8
    put(dict_at, 6)
    put(dict_at + 4, 0)
    for s in range(9):
        t = TEX_SLOT0 + TEX_SLOT * s
        put(t, 0xCAFE0000 + s)
        put(t + 4, base + dict_at)
        for k in range(8):
            put(t + 0x10 + 4 * k, 0x41414100 + s * 16 + k)
        put(t + 0x50, 0x1106)
        put(t + 0x54, 1 + s)
    previous = sentinel
    for i in range(f['count']):
        link = TEX_SLOT0 + TEX_SLOT * texture_slot(f['order'], i) + 8
        put(previous, base + link)
        put(link + 4, base + previous if f['prev'] else 0xFEEDF00D + i)
        previous = link
    put(previous, base + sentinel)
    put(sentinel + 4, base + previous if f['prev'] else 0xDEC0DED0)
    put(dict_at + 0x10, 0x0BADF00D)
    put(dict_at + 0x14, 0x0BADF11D)
    return bytes(a)


def expected_first_texture(f):
    return -1 if f['count'] == 0 else TEX_SLOT0 + TEX_SLOT * texture_slot(f['order'], 0)


# ------------------------------------------------------------------ Replace
REP_ARENA, REP_LIST, REP_HEADER, REP_MESHES, REP_POOL, REP_MAT = 0x240, 0x0A0, 0x0C0, 0x0D0, 0x140, 0x20


def pool(k):
    return REP_POOL + REP_MAT * k


def replace_fixture(c):
    meshes, materials = c % 9, c // 9 % 7
    old = rand(c, 1) % 6
    kind = rand(c, 2) % 4
    new = old if kind == 0 else 6 if kind == 1 else 7 if kind == 2 else rand(c, 3) % 8
    absent = rand(c, 4) % 5 == 0
    mesh_mat = [old if rand(c, 20 + i) % 3 == 0 else rand(c, 10 + i) % 8 for i in range(8)]
    list_mat = []
    for j in range(6):
        m = rand(c, 30 + j) % 8
        if absent and m == old:
            m = (m + 1) % 8
        list_mat.append(m)
    if not absent and materials > 0:
        list_mat[rand(c, 5) % materials] = old
    refs = [REFS[rand(c, 40 + k) % 8] for k in range(8)]
    return dict(case=c, meshes=meshes, materials=materials, mesh_mat=mesh_mat, list_mat=list_mat, old=old, new=new, ref_counts=refs,
                first_mesh_offset=0x30 if rand(c, 6) % 4 == 0 else 0, serial=rand(c, 7) & 0xFFFF, absent=absent)


def build_replace(base, f):
    a = bytearray(b'\xA5' * REP_ARENA)

    def put(at, v):
        a[at:at + 4] = struct.pack('<I', v & M32)

    def put16(at, v):
        a[at:at + 2] = struct.pack('<H', v & 0xFFFF)

    a[0x090:0x0A0] = b'\xC3' * 16
    a[0x0B8:0x0C0] = b'\xC3' * 8
    put(0x20, base + REP_LIST)
    put(0x24, f['materials'])
    put(0x28, 6)
    put(0x54, base + REP_HEADER)
    for j in range(6):
        put(REP_LIST + 4 * j, base + pool(f['list_mat'][j]))
    put(REP_HEADER, 0xBEEF)
    put16(REP_HEADER + 4, f['meshes'])
    put16(REP_HEADER + 6, f['serial'])
    put(REP_HEADER + 8, 0x1234)
    put(REP_HEADER + 12, f['first_mesh_offset'])
    for i in range(8):
        m = REP_MESHES + 12 * i
        put(m, 0xDA000000 | i)
        put(m + 4, 0x10 + i)
        put(m + 8, base + pool(f['mesh_mat'][i]))
    for k in range(8):
        p = pool(k)
        put(p, 0x7E000000 | k)
        put(p + 4, 0xFF00FF00 | k)
        put(p + 8, 0)
        put(p + 0xC, 0x3F800000)
        put(p + 0x10, 0x3F000000)
        put(p + 0x14, 0x3E800000)
        put16(p + 0x18, f['ref_counts'][k])
        put16(p + 0x1A, 0x5AA5)
    return a


def oracle_replace(base, f):
    """Devuelve (arena final, retorno, traza) segun el recorrido del EXE con los stubs del banco (FindIndex real hacia atras; Destroy sin liberar)."""
    a = build_replace(base, f)

    def get(at):
        return struct.unpack_from('<I', a, at)[0]

    def put(at, v):
        a[at:at + 4] = struct.pack('<I', v & M32)

    def ref(p):
        return struct.unpack_from('<H', a, p - base + 0x18)[0]

    def set_ref(p, v):
        struct.pack_into('<H', a, p - base + 0x18, v & 0xFFFF)

    trace, replaced = [], False
    old, new = base + pool(f['old']), base + pool(f['new'])
    n_meshes = struct.unpack_from('<H', a, REP_HEADER + 4)[0]
    for i in range(n_meshes):
        m = REP_MESHES + 12 * i
        if get(m + 8) != old:
            continue
        n = struct.unpack_from('<i', a, 0x24)[0]
        trace += [1, 0x20, old - base]   # FindIndex(&geometry->matList, material): la lista esta en +0x20 de la geometria (que ocupa el desplazamiento 0)
        idx = n - 1 if n <= 0 else next((j for j in range(n - 1, -1, -1) if get(REP_LIST + 4 * j) == old), -1)
        trace += [2, old - base, 0]
        if ref(old) != 1:
            set_ref(old, ref(old) - 1)
        put(REP_LIST + 4 * idx, new)     # idx == -1 escribe en 0x09C (relleno anterior)
        put(m + 8, new)
        set_ref(new, ref(new) + 1)
        replaced = True
    return bytes(a), replaced, trace


def coverage(kind):
    """Cobertura declarada de las fixtures (la exige run_local.py)."""
    if kind == 'replace':
        rows = [replace_fixture(c) for c in range(REPLACE_CASES)]
        return dict(meshes_0=sum(r['meshes'] == 0 for r in rows), absent=sum(r['absent'] for r in rows), new_is_old=sum(r['old'] == r['new'] for r in rows),
                    first_mesh_offset=sum(r['first_mesh_offset'] != 0 for r in rows), wrap_refs=sum(r['ref_counts'][r['new']] in (0x7FFF, 0xFFFF) for r in rows),
                    destroy_last=sum(r['ref_counts'][r['old']] == 1 for r in rows), empty_list=sum(r['materials'] == 0 for r in rows))
    if kind == 'first_texture':
        rows = [first_texture_fixture(c) for c in range(FIRST_TEXTURE_CASES)]
        return dict(empty=sum(r['count'] == 0 for r in rows), many=sum(r['count'] == 8 for r in rows), orders=len({r['order'] for r in rows}), junk=len({r['junk'] for r in rows}))
    raise KeyError(kind)


if __name__ == '__main__':
    if '--dump' in sys.argv:
        limit = int(sys.argv[sys.argv.index('--dump') + 1]) if len(sys.argv) > sys.argv.index('--dump') + 1 else 64
        out = {'slerp': [slerp_fixture(c) for c in range(limit)], 'first_texture': [first_texture_fixture(c) for c in range(limit)],
               'replace': [replace_fixture(c) for c in range(limit)]}
        json.dump(out, sys.stdout)
    else:
        print(json.dumps({'replace': coverage('replace'), 'first_texture': coverage('first_texture')}, indent=2))
