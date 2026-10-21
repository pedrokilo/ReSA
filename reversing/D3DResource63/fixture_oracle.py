"""Oraculo independiente del banco D3DResource63 (lote 63): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de D3DResourceSystem::GetTotalPixelsSize (0x730660) y
GetTotalIndexDataSize (0x7307F0) para una fila `input` (la especificacion semantica del caso que registra Core.hpp).

0x730660 (GetTotalPixelsSize):                          0x7307F0 (GetTotalIndexDataSize):
  push ebx; mov ebx,[0xC87C60]  ; contador (1 lectura)     push esi; push edi; xor edi,edi; mov esi,0xC87FC8
  push edi; xor edi,edi; test ebx,ebx; je fin              L: sub esi,0x18; mov ecx,esi; call 0x7303B0; add edi,eax
  push esi; mov esi,ebx; imul esi,esi,0x1C                 cmp esi,0xC87E48; jne L           ; 16 buffers, del ultimo (0xC87FB0) al primero
  add esi,0xC87C68                                         mov ecx,0xC87FC8; call 0x7303B0; add eax,edi   ; despues el generico
  L: sub esi,0x1C; mov ecx,esi; call 0x7300A0; add edi,eax; dec ebx; jne L   ; ebx sin signo, dec/jne
  fin: mov eax,edi

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos [tipo, ECX, retorno] EN ORDEN, nevents, cambios del
bloque de globales respecto a la imagen del caso, EAX); `stats` son banderas de cobertura. `python -I fixture_oracle.py` ejecuta la autoprueba.
"""
NAME_PIXELS = 'D3DResourceSystem::GetTotalPixelsSize'
NAME_INDEX = 'D3DResourceSystem::GetTotalIndexDataSize'
NAMES = [NAME_PIXELS, NAME_INDEX]
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
MAX_EVENTS = 520
TEX_BASE, TEX_STRIDE = 0xC87C68, 0x1C
IB_BASE, IB_STRIDE, IB_COUNT, GEN = 0xC87E48, 0x18, 16, 0xC87FC8
GEN_WORD = (GEN - 0xC87C60) // 4
assert GEN_WORD == 218 and IB_BASE + IB_COUNT * IB_STRIDE == GEN


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


def ret_val(mode, seed, k):
    """valor del falso en su llamada k (de 0): el mismo RetVal de Fixtures.hpp"""
    r = rand(seed, k + 1)
    if mode == 0:
        return 0
    if mode == 1:
        return 1
    if mode == 2:
        return r & 0xFFF
    if mode == 3:
        return r
    if mode == 4:
        return M32
    if mode == 5:
        return 0x80000000
    if mode == 6:
        return M32 if (k & 1) else 1
    return 0 if (r & 3) == 0 else (r >> 4)


def reentry_word(kind):
    return {1: 0, 2: GEN_WORD + 4, 3: 100}.get(kind)


def expected(name, inp, case):
    if name not in NAMES:
        raise KeyError(name)
    ntex = inp['ntex']
    mode, seed = inp['mode'], inp['seed']
    re_k, re_kind, re_val = inp['re']
    if name == NAME_PIXELS:
        count = ntex & M32          # una sola lectura (ebx), bucle dec/jne sin signo
        esi = (count * TEX_STRIDE + TEX_BASE) & M32
        kind = 1
        targets = []
        for _ in range(count):
            esi = (esi - TEX_STRIDE) & M32
            targets.append(esi)
    else:
        kind = 2
        targets = [IB_BASE + IB_STRIDE * i for i in range(IB_COUNT - 1, -1, -1)] + [GEN]
    events, total = [], 0
    for k, ecx in enumerate(targets):
        ret = ret_val(mode, seed, k)
        if len(events) < MAX_EVENTS:
            events.append([kind, ecx, ret])
        total = (total + ret) & M32
    # reentrada: la llamada re_k escribe re_val en una palabra del bloque (la imagen del caso es basura salvo la palabra 0, que vale ntex)
    changes = []
    w = reentry_word(re_kind)
    if w is not None and re_k < len(targets) and (w != 0 or re_val != ntex):
        changes.append(['G', w, re_val])
    observed = {'events': events, 'nevents': len(targets), 'changes': changes, 'eax': total}
    stats = {
        'ntex_zero': name == NAME_PIXELS and ntex == 0, 'ntex_one': name == NAME_PIXELS and ntex == 1, 'ntex_17': name == NAME_PIXELS and ntex == 17,
        'ntex_gt17': name == NAME_PIXELS and ntex > 17, 'ntex_ge65': name == NAME_PIXELS and ntex >= 65, 'ntex_2_16': name == NAME_PIXELS and 2 <= ntex <= 16,
        'sum_wraps': sum(ret_val(mode, seed, k) for k in range(len(targets))) > M32,
        'sum_zero_nonempty': bool(targets) and total == 0,
        'reentry_fired': bool(changes) or (w is not None and re_k < len(targets)),
        'reentry_changed': bool(changes), 'reentry_counter': re_kind == 1 and re_k < len(targets), 'reentry_other': re_kind in (2, 3) and re_k < len(targets),
        'mode_all_ones': mode in (4, 6), 'mode_random': mode in (3, 7),
        'index_calls': name == NAME_INDEX,
    }
    for m in range(8):
        stats['mode%d' % m] = mode == m
    return observed, stats


def selftest():
    # GetTotalPixelsSize con 3 buffers, modo 2: llamadas a 0xC87C68+0x1C*2, +0x1C*1, +0
    obs, _ = expected(NAME_PIXELS, {'ntex': 3, 'mode': 1, 'seed': 0, 're': [0xFFFFFFFF, 0, 0]}, 0)
    assert obs['events'] == [[1, 0xC87C68 + 0x38, 1], [1, 0xC87C68 + 0x1C, 1], [1, 0xC87C68, 1]] and obs['eax'] == 3 and obs['changes'] == []
    obs, _ = expected(NAME_PIXELS, {'ntex': 0, 'mode': 3, 'seed': 5, 're': [0xFFFFFFFF, 0, 0]}, 0)
    assert obs['events'] == [] and obs['eax'] == 0 and obs['nevents'] == 0
    # GetTotalIndexDataSize: 16 buffers del ultimo al primero y el generico; 17 llamadas
    obs, _ = expected(NAME_INDEX, {'ntex': 9, 'mode': 6, 'seed': 0, 're': [0xFFFFFFFF, 0, 0]}, 0)
    assert [e[1] for e in obs['events']] == [0xC87E48 + 0x18 * i for i in range(15, -1, -1)] + [0xC87FC8] and obs['nevents'] == 17
    assert obs['eax'] == sum(1 if k % 2 == 0 else M32 for k in range(17)) & M32
    # reentrada del contador: el contador se lee una sola vez
    obs, _ = expected(NAME_PIXELS, {'ntex': 4, 'mode': 1, 'seed': 0, 're': [1, 1, 1000]}, 0)
    assert obs['nevents'] == 4 and obs['changes'] == [['G', 0, 1000]]
    obs, _ = expected(NAME_PIXELS, {'ntex': 4, 'mode': 1, 'seed': 0, 're': [1, 1, 4]}, 0)
    assert obs['changes'] == []
    # contador >= 2^31 no se prueba (miles de millones de llamadas); ret_val coincide con RetVal de Fixtures.hpp (lo verifica fixture_mirror.py)
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
