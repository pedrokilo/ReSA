"""Fixtures y oraculo independiente del banco Dependencies25E (16 funciones de biblioteca: RenderWare y CRT estatica del EXE).

Las tablas se leen de Fixtures.hpp (una sola fuente); las formulas de los casos, la construccion de los arenas y la semantica de cada funcion se repiten aqui en
Python a partir del desensamblado del EXE (g25e-classification.md), sin mirar el codigo C++ de la reconstruccion:
  * RxHeapGetGlobalHeap 0x8052F0: devuelve la variable 0xC9A6B0.
  * _rwFrameListDeinitialize 0x807720: si numFrames != 0 llama a rwfree(frames) y devuelve la lista.
  * RwD3D9RasterIsCompressed 0x820C90: 1 si el nibble alto del byte [raster + [0xB4E9E0] + 0xA] no es 0.
  * _rwPluginRegistryGetPluginOffset 0x808470: recorre la lista (firstRegEntry +0x10, next +0x30) buscando pluginID (+8); devuelve offset (+0) o -1.
  * _rwSList*: aritmetica de 32 bits sobre {listElements, numElementsFilled, numElementsAlloced, entrySize}.
  * _rwStringClose 0x80A440: no hace nada.
  * _rwStringStreamGetSize 0x80A470: strlen (del motor) de la cadena (o de 0x884F6A si es nula) + 4, con los dos bits bajos a cero.
  * CrtAtexit 0x821D1E: -1 si _onexit(func) devuelve 0, 0 si no.
  * CrtLockFile2 0x823FD1 / CrtUnlockFile2 0x824023: indice < 20 -> _lock/_unlock(16 + indice); si no, Enter/LeaveCriticalSection(stream + 0x20).
  * CrtMalloc 0x824257: _nh_malloc(size, _newmode).
  * CrtErrno 0x82B181: &_getptd()->_terrno = ptd + 8.
`python -B fixture_oracle.py` imprime la cobertura declarada de las fixtures.
"""
import json
import re
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
TEXT = (HERE / 'Fixtures.hpp').read_text(encoding='utf-8')
M32 = 0xFFFFFFFF
CASES = 1024
GUARD = 16
EMPTY_STRING = 0x884F6A


def table(name):
    body = re.search(name + r'\{(.*?)\};', TEXT, re.S).group(1)
    return [int(x, 0) for x in re.findall(r'0x[0-9A-Fa-f]+|\b\d+\b', body)]


WORDS = table('kWords')
PLUGIN_IDS = table('kPluginIds')
LOCK_INDICES = table('kLockIndices')
MALLOC_SIZES = table('kMallocSizes')
MALLOC_FAIL = table('kMallocFailSizes')
NEW_MODES = table('kNewModes')
assert (len(WORDS), len(PLUGIN_IDS), len(LOCK_INDICES), len(MALLOC_SIZES), len(MALLOC_FAIL), len(NEW_MODES)) == (32, 6, 20, 40, 8, 6)

FUNCTIONS = [
    'RwCore::RxHeapGetGlobalHeap', 'RwCore::_rwFrameListDeinitialize', 'RwCore::RwD3D9RasterIsCompressed',
    'RwPlCore::_rwPluginRegistryGetPluginOffset', 'RwPlCore::_rwSListDestroyEndEntries', 'RwPlCore::_rwSListGetNumEntries', 'RwPlCore::_rwSListGetEntry',
    'RwPlCore::_rwSListGetBegin', 'RwPlCore::_rwSListGetEnd', 'RwPlCore::_rwStringClose', 'RwPlCore::_rwStringStreamGetSize',
    'CRT::CrtAtexit', 'CRT::CrtLockFile2', 'CRT::CrtUnlockFile2', 'CRT::CrtMalloc', 'CRT::CrtErrno']
EXPECTED = {f: CASES for f in FUNCTIONS}


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


def word(c, k):
    return rand(c, k + 1000) if (rand(c, k) & 3) == 0 else WORDS[rand(c, k + 2000) % 32]


def seed(c, reg):
    return rand(c, 9000 + reg)


def junk(size, c):
    out = bytearray(size)
    for i in range(0, size, 4):
        out[i:i + 4] = rand(c, 7000 + i // 4).to_bytes(4, 'little')
    return out


def put32(buf, at, v):
    buf[at:at + 4] = (v & M32).to_bytes(4, 'little')


def get32(buf, at):
    return int.from_bytes(buf[at:at + 4], 'little')


def guarded(interior):
    return (bytes([0xA5]) * GUARD + bytes(interior) + bytes([0xA5]) * GUARD).hex()


# ------------------------------------------------------------------ fixtures por funcion (entrada registrada por el banco + arena inicial)
def f_rxheap(c, base=0):
    v = word(c, 1)
    return dict(input={'case': c, 'value': v}, interior=None)


def f_framelist(c, base=0):
    frames, num = word(c, 1), word(c, 2)
    if rand(c, 3) % 8 == 0:
        num = 0
    if rand(c, 4) % 8 == 1:
        frames = 0
    a = junk(48, c)
    put32(a, 16, frames)
    put32(a, 20, num)
    return dict(input={'case': c, 'frames': frames, 'num_frames': num}, interior=a, frames=frames, num=num)


def f_raster(c, base=0):
    flags = c & 255
    sel = (c >> 8) & 3
    delta = [0, 0x30, 0xFFFFFFB0, rand(c, 61)][sel]
    a = junk(64, c)
    a[32] = flags
    return dict(input={'case': c, 'flags': flags, 'delta': delta}, interior=a, flags=flags, delta=delta)


def f_plugin(c, base=0):
    n = rand(c, 10) % 9
    slot = [(j * 3 + rand(c, 11) % 8) % 8 for j in range(8)]
    ids = [PLUGIN_IDS[rand(c, 40 + j) % 6] for j in range(8)]
    offs = [word(c, 30 + j) for j in range(8)]
    mode = rand(c, 50) % 4
    if mode == 0:
        query = ids[rand(c, 51) % n] if n else 0x12345678
    elif mode == 1:
        query = 0x12345678 ^ (rand(c, 52) & 0xFFFF)
    elif mode == 2:
        query = PLUGIN_IDS[rand(c, 53) % 6]
    else:
        query = word(c, 54)
    a = junk(48 + 8 * 0x40, c)
    for j in range(n):
        e = 48 + 0x40 * slot[j]
        put32(a, e + 0, offs[j])
        put32(a, e + 8, ids[j])
        put32(a, e + 0x30, (base + 48 + 0x40 * slot[j + 1]) & M32 if j + 1 < n else 0)
    put32(a, 16 + 0x10, (base + 48 + 0x40 * slot[0]) & M32 if n else 0)
    return dict(input={'case': c, 'n': n, 'mode': mode, 'query': query, 'slot': slot, 'id': ids, 'offset': offs}, interior=a,
                n=n, slot=slot, ids=ids, offs=offs, query=query)


def f_slist(c, base=0):
    elements, filled, alloced, entry_size, arg = word(c, 1), word(c, 2), word(c, 3), word(c, 4), word(c, 5)
    if rand(c, 6) % 4 == 0:
        entry_size, filled, arg = rand(c, 7) % 256, rand(c, 8) % 1024, rand(c, 9) % 1024
    a = junk(48, c)
    put32(a, 16, elements)
    put32(a, 20, filled)
    put32(a, 24, alloced)
    put32(a, 28, entry_size)
    return dict(input={'case': c, 'elements': elements, 'filled': filled, 'alloced': alloced, 'entry_size': entry_size, 'arg': arg}, interior=a,
                elements=elements, filled=filled, entry_size=entry_size, arg=arg)


def f_stringclose(c, base=0):
    return dict(input={'case': c}, interior=junk(32, c))


def f_string(c, base=0):
    null = rand(c, 1) % 8 == 0
    length = word(c, 2)
    if rand(c, 3) % 4 == 0:
        length = rand(c, 4) % 100
    return dict(input={'case': c, 'null': null, 'length': length, 'object': (base + 16) & M32}, interior=junk(64, c), null=null, length=length)


def f_atexit(c, base=0):
    func = word(c, 1)
    k = rand(c, 2) % 4
    result = 0 if k == 0 else func if k == 1 else word(c, 3)
    return dict(input={'case': c, 'func': func, 'result': result}, interior=junk(16, c), func=func, result=result)


def f_lock(c, base=0):
    real = c < 640
    index = c % 20 if real else LOCK_INDICES[(c - 640) % 20]
    return dict(input={'case': c, 'index': index, 'real': real, 'stream_is_section': not real}, interior=None, index=index, real=real)


def f_malloc(c, base=0):
    success = c < 640
    size = MALLOC_SIZES[rand(c, 1) % 40] if success else MALLOC_FAIL[rand(c, 2) % 8]
    newmode = NEW_MODES[rand(c, 3) % 6]
    return dict(input={'case': c, 'success': success, 'size': size, 'newmode': newmode}, interior=None, success=success, size=size, newmode=newmode)


def f_errno(c, base=0):
    return dict(input={'case': c, 'value': word(c, 1)}, interior=None, value=word(c, 1))


FIXTURE = {
    FUNCTIONS[0]: f_rxheap, FUNCTIONS[1]: f_framelist, FUNCTIONS[2]: f_raster, FUNCTIONS[3]: f_plugin,
    FUNCTIONS[4]: f_slist, FUNCTIONS[5]: f_slist, FUNCTIONS[6]: f_slist, FUNCTIONS[7]: f_slist, FUNCTIONS[8]: f_slist,
    FUNCTIONS[9]: f_stringclose, FUNCTIONS[10]: f_string, FUNCTIONS[11]: f_atexit, FUNCTIONS[12]: f_lock, FUNCTIONS[13]: f_lock,
    FUNCTIONS[14]: f_malloc, FUNCTIONS[15]: f_errno}


def fixture(function, c, base=0):
    return FIXTURE[function](c, base)


# ------------------------------------------------------------------ semantica (a partir del desensamblado)
def expected(function, c, base, cs=None):
    """Campos que el banco registra por lado: eax, arena (con guardas), trace y, segun la funcion, campos propios. Solo los que el oraculo conoce."""
    f = fixture(function, c, base)
    out = {'input': f['input']}
    name = function.split('::')[1]
    if f['interior'] is not None:
        out['arena'] = guarded(f['interior'])
    out['trace'] = []
    if name == 'RxHeapGetGlobalHeap':
        out['eax'] = f['input']['value']
        out['state'] = [f['input']['value'].to_bytes(4, 'little').hex()]
    elif name == '_rwFrameListDeinitialize':
        out['eax'] = (base + 16) & M32
        out['trace'] = [f['frames']] if f['num'] else []
    elif name == 'RwD3D9RasterIsCompressed':
        out['eax'] = 1 if (f['flags'] & 0xF0) else 0
        out['state'] = [f['delta'].to_bytes(4, 'little').hex()]
    elif name == '_rwPluginRegistryGetPluginOffset':
        eax = M32
        for j in range(f['n']):
            if f['ids'][j] == f['query']:
                eax = f['offs'][j]
                break
        out['eax'] = eax
    elif name == '_rwSListDestroyEndEntries':
        a = bytearray(f['interior'])
        put32(a, 20, (f['filled'] - f['arg']) & M32)
        out['arena'] = guarded(a)
    elif name == '_rwSListGetNumEntries':
        out['eax'] = f['filled']
    elif name == '_rwSListGetEntry':
        out['eax'] = (f['elements'] + f['entry_size'] * f['arg']) & M32
    elif name == '_rwSListGetBegin':
        out['eax'] = f['elements']
    elif name == '_rwSListGetEnd':
        out['eax'] = (f['elements'] + f['entry_size'] * f['filled']) & M32
    elif name == '_rwStringClose':
        pass
    elif name == '_rwStringStreamGetSize':
        out['eax'] = ((f['length'] + 4) & M32) & ~3 & M32
        out['trace'] = [EMPTY_STRING if f['null'] else (base + 16) & M32]
    elif name == 'CrtAtexit':
        out['eax'] = M32 if f['result'] == 0 else 0
        out['trace'] = [f['func']]
    elif name == 'CrtLockFile2':
        out.update(recursion=1, owner_is_this_thread=True)
        if cs:
            out['lock_count'] = cs[1]   # LockCount de una seccion tomada una vez sin espera (lo mide el banco; depende del SO)
    elif name == 'CrtUnlockFile2':
        out.update(recursion=0, owner_is_this_thread=False)
        if cs:
            out['lock_count'] = cs[0]   # LockCount de una seccion libre (lo mide el banco)
    elif name == 'CrtMalloc':
        out['null'] = not f['success']
        if f['success']:
            out['align8'] = 0
        elif f['size'] > 0xFFFFFFE0:
            out['trace'] = []
        else:
            out['trace'] = [f['size']] if f['newmode'] else []
    elif name == 'CrtErrno':
        out['eax_minus_ptd'] = 8
        out['readback'] = f['value']
    return out


def coverage():
    """Cobertura de estres declarada de las fixtures (lo que run_local.py exige como minimo)."""
    cov = {k: 0 for k in (
        'frame_free_called', 'frame_free_not_called', 'frame_null_frames_nonzero_count', 'raster_compressed', 'raster_plain', 'raster_negative_delta',
        'plugin_found', 'plugin_not_found', 'plugin_duplicate_ids', 'plugin_empty', 'plugin_full', 'slist_wrap_entry', 'slist_wrap_destroy', 'slist_zero_size',
        'string_null', 'string_wrap', 'atexit_zero', 'atexit_nonzero', 'lock_real', 'lock_section', 'lock_boundary_19', 'lock_boundary_20', 'lock_max_index',
        'malloc_success', 'malloc_fail_heap', 'malloc_fail_early', 'malloc_fail_handler', 'malloc_fail_no_handler', 'errno_zero', 'errno_negative')}
    for c in range(CASES):
        f = f_framelist(c)
        cov['frame_free_called' if f['num'] else 'frame_free_not_called'] += 1
        cov['frame_null_frames_nonzero_count'] += bool(f['num'] and f['frames'] == 0)
        r = f_raster(c)
        cov['raster_compressed' if r['flags'] & 0xF0 else 'raster_plain'] += 1
        cov['raster_negative_delta'] += r['delta'] >= 0x80000000
        p = f_plugin(c)
        found = any(p['ids'][j] == p['query'] for j in range(p['n']))
        cov['plugin_found' if found else 'plugin_not_found'] += 1
        cov['plugin_duplicate_ids'] += len(set(p['ids'][:p['n']])) < p['n']
        cov['plugin_empty'] += p['n'] == 0
        cov['plugin_full'] += p['n'] == 8
        s = f_slist(c)
        cov['slist_wrap_entry'] += (s['elements'] + s['entry_size'] * s['arg']) > M32
        cov['slist_wrap_destroy'] += s['filled'] < s['arg']
        cov['slist_zero_size'] += s['entry_size'] == 0
        t = f_string(c)
        cov['string_null'] += t['null']
        cov['string_wrap'] += t['length'] + 4 > M32 or t['length'] >= 0xFFFFFFFC
        a = f_atexit(c)
        cov['atexit_zero' if a['result'] == 0 else 'atexit_nonzero'] += 1
        k = f_lock(c)
        cov['lock_real' if k['real'] else 'lock_section'] += 1
        cov['lock_boundary_19'] += k['index'] == 19
        cov['lock_boundary_20'] += k['index'] == 20
        cov['lock_max_index'] += k['index'] == 0x7FFFFFFF
        m = f_malloc(c)
        if m['success']:
            cov['malloc_success'] += 1
        else:
            cov['malloc_fail_early' if m['size'] > 0xFFFFFFE0 else 'malloc_fail_heap'] += 1
            if m['size'] <= 0xFFFFFFE0:
                cov['malloc_fail_handler' if m['newmode'] else 'malloc_fail_no_handler'] += 1
        e = f_errno(c)
        cov['errno_zero'] += e['value'] == 0
        cov['errno_negative'] += e['value'] >= 0x80000000
    return cov


if __name__ == '__main__':
    if len(sys.argv) == 2 and sys.argv[1] == '--dump':
        print(json.dumps({fn: [fixture(fn, c, 0x100000)['input'] for c in range(4)] for fn in FUNCTIONS}))
    else:
        print(json.dumps(coverage(), indent=2))
