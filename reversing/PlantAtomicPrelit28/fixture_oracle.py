"""Oraculo independiente del banco PlantAtomicPrelit28: CPlantMgr::AtomicCreatePrelitIfNeeded (0x5DD100).

Se modela desde el DESENSAMBLADO del EXE (reversing/_handoff/dis.sh 0x5DD100 0x5DD1E0), no desde el C++ de la DLL, a partir de la fila `input` que registra el banco.

void AtomicCreatePrelitIfNeeded(RpAtomic* atomic), cdecl, `ret` normal; el EXE devuelve el atomo en EAX (ningun llamador lo lee). Desensamblado resumido:
  5DD108  ebx = atomic->geometry ([atomic+18h])
  5DD10B  test byte [ebx+8],8 ; jne 5DD1D8   -> geometria YA prelit (rpGEOMETRYPRELIT = 8): no se hace nada (salida con eax = atomic)
  5DD115  numTri = [ebx+10h] ; numVert = [ebx+14h] ; oldMorph = [ebx+5Ch]
  5DD12D  new = RpGeometryCreate(numVert, numTri, 0Dh)                         (0x74CA90)   [orden de pila: formato, triangulos, vertices]
  5DD134  newTex = [new+34h] ; newMorph = [new+5Ch]
  5DD14A  rep movs: numVert*12 bytes   newMorph->verts ([newMorph+14h]) <- oldMorph->verts ([oldMorph+14h])
  5DD16B  rep movs: numVert*2 dwords   newTex <- oldTex ([ebx+34h])
  5DD181  rep movs: numTri*2 dwords    [new+2Ch] <- [ebx+2Ch]   (triangulos de 8 bytes: 3 indices u16 + matIndex u16)
  5DD185  si numTri <= 0 (con signo) salta el bucle; en otro caso, para i = 0..numTri-1 (tri = old.triangles + 8*i):
            m = RpGeometryTriangleGetMaterial(old, tri)                     (0x74C760)
            RpGeometryTriangleSetMaterial(new, new.triangles + 8*i, m)      (0x74C6C0)
  5DD1B5  RpGeometryUnlock(new)                                             (0x74C800)
  5DD1BA  [new+8] |= 2                                                      (rpGEOMETRYPOSITIONS, DESPUES de Unlock)
  5DD1CB  RpAtomicSetGeometry(atomic, new, 1)                               (0x749D40)

El banco sustituye esas cinco funciones por falsos (ver Core.hpp). Este oraculo reconstruye, a partir de `input`, el estado exacto que debe quedar:
  * la traza de llamadas [tipo, a, b, c, d] (tipos: 1 Create(numV, numT, formato), 2 GetMaterial(etiqueta geometria, indice de triangulo), 3 SetMaterial(etiqueta, indice, material),
    4 Unlock(etiqueta, hash64 bajo, hash64 alto, flags de la nueva en ese instante), 5 SetGeometry(etiqueta atomo, etiqueta geometria, flags, flags de la nueva en ese instante)),
  * el hash FNV-1a de 64 bits de TODA la arena de la geometria nueva (con los cinco dwords de puntero a cero): vertices, coordenadas y triangulos copiados, matIndex escritos por el
    falso de SetMaterial, flags |= 2 y todos los bytes que NO deben tocarse (guardas, resto de la geometria),
  * que los punteros de la arena nueva sigan intactos y que la arena vieja (geometria, datos y atomo) no cambie.
"""
import struct
import sys

M32 = 0xFFFFFFFF
NAME = 'CPlantMgr::AtomicCreatePrelitIfNeeded'
NAMES = [NAME]
CASES = {NAME: 4096}
MAX_V = MAX_T = 100
# Disposicion de las arenas (identica a Fixtures.hpp)
GEOM, MORPH, VERTS, TEX, TRI, ARENA = 0x40, 0xC0, 0x100, 0x5D0, 0x910, 0xC70
PTR_DWORDS = (GEOM + 0x2C, GEOM + 0x34, GEOM + 0x5C, MORPH + 0x00, MORPH + 0x14)
OLD_LABEL, NEW_LABEL = 1, 2


class Fail(Exception):
    pass


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rand(seed, i // 4))
    return bytearray(out[:nbytes])


def fill_data(nbytes, seed, mode, kind):
    m = mode & 7
    if m == 1:
        return bytearray(nbytes)
    if m == 2:
        return bytearray(b'\xff' * nbytes)
    if m == 6:
        return bytearray(((i * 31 + kind * 7 + seed) & 0xFF) for i in range(nbytes))
    d = fill_words(nbytes, (seed + kind * 7919) & M32)
    if m == 3 and kind == 2:
        for o in range(0, nbytes - 7, 8):
            d[o + 6] = d[o + 7] = 0xFF
    if m == 4 and kind == 0:
        for o in range(0, nbytes - 3, 4):
            d[o + 3] = (d[o + 3] & 0x80) | 0x7F
            d[o + 2] |= 0x80
    if m == 5 and kind == 1:
        for o in range(0, nbytes - 3, 4):
            d[o + 3] &= 0x80
            d[o + 2] &= 0x7F
    return d


def fnv64(b):
    h = 14695981039346656037
    for x in b:
        h ^= x
        h = (h * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


def masked_hash(img):
    t = bytearray(img)
    for at in PTR_DWORDS:
        t[at:at + 4] = b'\0\0\0\0'
    return fnv64(t)


def material(i, mat_index):
    return 0x4D000000 | ((i * 0x101 + mat_index + 0x1234) & 0xFFFF)


def expected_prelit(inp, case):
    v, t = inp['num_v'], inp['num_t']
    if not (0 <= v <= MAX_V and 0 <= t <= MAX_T):
        raise RuntimeError('fixture invalido: contadores fuera de la arena (%d, %d)' % (v, t))
    for k in ('flags', 'new_flags', 'old_seed', 'data_seed', 'new_seed'):
        if not 0 <= inp[k] <= M32:
            raise RuntimeError('fixture invalido: %s' % k)
    img = fill_words(ARENA, inp['new_seed'])
    img[GEOM + 8:GEOM + 12] = struct.pack('<I', inp['new_flags'])
    for at in PTR_DWORDS:
        img[at:at + 4] = b'\0\0\0\0'
    stats = {}
    mode = inp['mode'] & 7
    stats['mode%d' % mode] = True
    stats['g%d' % inp['group']] = True
    stats['fam%d' % inp['family']] = True
    prelit = bool(inp['flags'] & 8)
    stats['early'] = prelit
    stats['not_early'] = not prelit
    stats['flags_hi'] = bool(inp['flags'] >> 31)
    stats['flags_all_ones'] = inp['flags'] == M32
    stats['new_flags_has2'] = bool(inp['new_flags'] & 2)
    stats['new_flags_no2'] = not (inp['new_flags'] & 2)
    stats['v0'] = v == 0
    stats['t0'] = t == 0
    stats['v0_t0'] = v == 0 and t == 0
    stats['v0_t_pos'] = v == 0 and t > 0
    stats['v_pos_t0'] = v > 0 and t == 0
    stats['t1'] = t == 1
    stats['v1'] = v == 1
    stats['v_ge32'] = v >= 32
    stats['t_ge32'] = t >= 32
    stats['v_ge64'] = v >= 64
    stats['t_ge64'] = t >= 64
    stats['v_max'] = v == MAX_V
    stats['t_max'] = t == MAX_T
    stats['v_not_mult4'] = v % 4 != 0
    if prelit:
        want = {'trace': [], 'ev_lost': 0, 'new_hash': '%016x' % masked_hash(img), 'new_ptrs_ok': True, 'old_ok': True}
        return want, stats
    verts = fill_data(12 * v, inp['data_seed'], mode, 0)
    tex = fill_data(8 * v, inp['data_seed'], mode, 1)
    tri = fill_data(8 * t, inp['data_seed'], mode, 2)
    img[VERTS:VERTS + 12 * v] = verts
    img[TEX:TEX + 8 * v] = tex
    img[TRI:TRI + 8 * t] = tri
    trace = [[1, v, t, 0xD, 0]]
    mat_ff = 0
    for i in range(t):
        m = struct.unpack_from('<H', tri, 8 * i + 6)[0]
        mat_ff += m == 0xFFFF
        val = material(i, m)
        trace.append([2, OLD_LABEL, i, 0, 0])
        trace.append([3, NEW_LABEL, i, val, 0])
        img[TRI + 8 * i + 6:TRI + 8 * i + 8] = struct.pack('<H', val & 0xFFFF)
    stats['mat_ffff'] = mat_ff > 0
    h = masked_hash(img)
    trace.append([4, NEW_LABEL, h & M32, h >> 32, inp['new_flags']])
    nf = (inp['new_flags'] | 2) & M32
    img[GEOM + 8:GEOM + 12] = struct.pack('<I', nf)
    trace.append([5, OLD_LABEL, NEW_LABEL, 1, nf])
    stats['calls_total'] = len(trace)
    want = {'trace': trace, 'ev_lost': 0, 'new_hash': '%016x' % masked_hash(img), 'new_ptrs_ok': True, 'old_ok': True}
    return want, stats


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('funcion desconocida %s' % function)
    return expected_prelit(inp, case)


def compare(obs, want):
    """Claves de `want` cuyo valor observado difiere."""
    return sorted(k for k in want if obs.get(k) != want[k])


def _inp(v, t, flags=0, mode=0, new_flags=0x10, group=4, family=0, old_seed=1, data_seed=2, new_seed=3):
    return {'group': group, 'family': family, 'mode': mode, 'flags': flags, 'num_v': v, 'num_t': t, 'old_seed': old_seed, 'data_seed': data_seed, 'new_seed': new_seed,
            'new_flags': new_flags, 'eax': 0, 'ecx': 0, 'edx': 0, 'stack_seed': 0}


def _selftest():
    assert fnv64(b'a') == 0xaf63dc4c8601ec8c and fnv64(b'') == 14695981039346656037
    assert mix(0) == 0 and rand(0, 0) == mix(12345)
    # prelit: nada se llama y la arena nueva queda como el relleno
    want, st = expected(NAME, _inp(5, 5, flags=0x8), 0)
    assert want['trace'] == [] and st['early']
    want, st = expected(NAME, _inp(5, 5, flags=0xFFFFFFFF), 0)
    assert want['trace'] == [] and st['flags_all_ones']
    # no prelit, 0 y 0: Create, Unlock, SetGeometry
    want, st = expected(NAME, _inp(0, 0), 0)
    assert [e[0] for e in want['trace']] == [1, 4, 5] and want['trace'][0] == [1, 0, 0, 13, 0] and want['trace'][2] == [5, 1, 2, 1, 0x12] and st['v0_t0']
    # bit 2 ya puesto en la nueva: se conserva; Unlock ve el valor sin tocar
    want, st = expected(NAME, _inp(1, 1, new_flags=0x3), 0)
    assert want['trace'][-2][4] == 0x3 and want['trace'][-1][4] == 0x3
    # T triangulos: Create + 2T + Unlock + SetGeometry; GetMaterial sobre la VIEJA, SetMaterial sobre la NUEVA, en orden
    want, st = expected(NAME, _inp(7, 3), 0)
    tr = want['trace']
    assert len(tr) == 1 + 6 + 2 and tr[1][:3] == [2, 1, 0] and tr[2][:3] == [3, 2, 0] and tr[5][:3] == [2, 1, 2] and tr[6][:3] == [3, 2, 2]
    # los matIndex nuevos son los del falso (low16 del material), no los copiados
    want2, _ = expected(NAME, _inp(7, 3, mode=3), 0)       # modo 3: matIndex viejos 0xFFFF
    assert want2['trace'][2][3] == material(0, 0xFFFF) and st['mode0'] and _ is not None
    # tamano maximo y valores limite
    want, st = expected(NAME, _inp(100, 100), 0)
    assert len(want['trace']) == 1 + 200 + 2 and st['v_max'] and st['t_max']
    for bad in (_inp(101, 0), _inp(0, 101), _inp(-1, 0)):
        try:
            expected(NAME, bad, 0)
            raise AssertionError('debia rechazar')
        except RuntimeError:
            pass
    # el hash depende de la copia: dos semillas de datos distintas dan hashes distintos; lo no copiado (guardas) no cambia entre dos conteos con la misma semilla de arena
    a, _ = expected(NAME, _inp(4, 4, data_seed=10), 0)
    b, _ = expected(NAME, _inp(4, 4, data_seed=11), 0)
    assert a['new_hash'] != b['new_hash']
    # rellenos de datos
    assert fill_data(16, 5, 1, 0) == bytearray(16) and fill_data(16, 5, 2, 0) == bytearray(b'\xff' * 16)
    d = fill_data(32, 5, 3, 2)
    assert all(d[o + 6] == 0xFF and d[o + 7] == 0xFF for o in (0, 8, 16, 24))
    d = fill_data(48, 5, 4, 0)
    for o in range(0, 48, 4):
        assert (struct.unpack_from('<I', d, o)[0] >> 23) & 0xFF == 0xFF
    d = fill_data(48, 5, 5, 1)
    for o in range(0, 48, 4):
        assert (struct.unpack_from('<I', d, o)[0] >> 23) & 0xFF == 0
    # comparador
    assert compare({'a': 1, 'b': 2}, {'a': 1, 'b': 3}) == ['b'] and compare({'a': 1}, {'a': 1}) == []
    print('ok')


def _check_dump(path):
    """Contrasta con el volcado de dump_fixtures.cpp (el generador C++ real): campos de entrada, hashes de relleno y cobertura. Imprime la cobertura."""
    import json
    cov, n = {}, 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        n += 1
        inp = {k: row[k] for k in ('group', 'family', 'mode', 'flags', 'num_v', 'num_t', 'old_seed', 'data_seed', 'new_seed', 'new_flags', 'eax', 'ecx', 'edx', 'stack_seed')}
        assert row['fill_arena'] == '%016x' % fnv64(fill_words(ARENA, inp['new_seed'])), row['n']
        assert row['fill_old'] == '%016x' % fnv64(fill_words(0xD60, inp['old_seed'])), row['n']
        v, t = inp['num_v'], inp['num_t']
        assert row['data_verts'] == '%016x' % fnv64(fill_data(12 * v, inp['data_seed'], inp['mode'], 0)), row['n']
        assert row['data_tex'] == '%016x' % fnv64(fill_data(8 * v, inp['data_seed'], inp['mode'], 1)), row['n']
        assert row['data_tri'] == '%016x' % fnv64(fill_data(8 * t, inp['data_seed'], inp['mode'], 2)), row['n']
        want, stats = expected(NAME, inp, row['n'])
        for k, val in stats.items():
            if val is True:
                cov[k] = cov.get(k, 0) + 1
    assert n == CASES[NAME], n
    print(json.dumps(dict(sorted(cov.items())), indent=0))


def _check_host(path):
    """Contrasta la salida de host_selftest.cpp (transcripcion literal del desensamblado + los falsos del banco, sin juego) con este oraculo."""
    import json
    n = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        want, _ = expected(NAME, row['input'], row['case'])
        diff = compare(row['observed'], want)
        assert not diff, (row['case'], diff, {k: row['observed'][k] for k in diff}, {k: want[k] for k in diff})
        n += 1
    assert n == CASES[NAME], n
    print('ok host', n)


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--check-host':
        _check_host(sys.argv[2])
    elif len(sys.argv) == 3 and sys.argv[1] == '--check-dump':
        _check_dump(sys.argv[2])
    else:
        _selftest()
