"""Oraculo independiente del banco Helpers65A (lote 65): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de cinco ayudantes para una fila `input` (la especificacion
semantica del caso que registra Core.hpp) y el numero de caso (la imagen previa de la arena es basura determinista salvo los campos del caso).

0x406CE0 (FloorFloat, cdecl(float), ST0):              0x734A70 (callback de RwFrameForAllChildren, cdecl):
  fld dword [esp+4]; sub esp,8; fstp qword [esp]          push esi; mov esi,[esp+8]; push esi; call 0x7C5160 (hierarchy); add esp,4; test eax,eax; jne L
  call 0x8219F0 (floor del CRT); add esp,8; ret           push [esp+0Ch]; push 0x734A70; push esi; call 0x7F0DC0; add esp,0Ch; mov eax,esi; pop esi; ret
                                                          L: mov ecx,[esp+0Ch]; mov [ecx],eax; xor eax,eax; pop esi; ret
0x606060 (CDecisionMaker::MakeDecision, thiscall ret 24h):  ecx = this + arg1 * 0x3C (imul); empuja arg9..arg2 (ocho) y llama a 0x6040D0
0x5BB9F0 (ReadEventIndices, stdcall ret 8):  rep stosd 96 dwords = 0 en indices; SetDir(""); SetDir("data\\decision\\"); OpenFile(nombre,"r"); SetDir("");
  ReadLine(f,linea,256); mientras AL: si linea[0] != 0 y != 0Ah: sscanf(linea,"%s %d",palabra,&idx); indices[idx] = n++ ; ReadLine(...); al final CloseFile(f)
0x4F4620 (vector VC6 push_back, thiscall ret 4):  first != 0 y (last-first)>>2 (sar) < (end-first)>>2 (comparacion sin signo) -> 0x4F3B30(last,1,&v,this,&v) y last = last_leido_antes + 4;
  si no -> 0x4F4320 thiscall(this; last, 1, &v)

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed`; `stats` son banderas de cobertura.
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
N_FLOOR = 'CHud::FloorFloat'
N_CALLBACK = 'RwHelper::GetAnimHierarchyFromFrameCallback'
N_DECISION = 'CDecisionMaker::MakeDecision'
N_READ = 'CDecisionMakerTypes::ReadEventIndices'
N_PUSH = 'UserTracksLegacyVector::PushBack'
NAMES = [N_FLOOR, N_CALLBACK, N_DECISION, N_READ, N_PUSH]
FN_INDEX = {n: i for i, n in enumerate(NAMES)}
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
M64 = 0xFFFFFFFFFFFFFFFF
MAX_EVENTS = 64
# disposicion de la arena (Fixtures.hpp)
ARENA_WORDS = 1024
FRAMES, FRAME_STRIDE, NODES = 0x100, 0x10, 10
FOUND = 0x1A0
TASK_SLOT, FACIAL_SLOT = 0x240, 0x244
INDICES, INDEX_COUNT, NAME_ZONE, NAME_BYTES = 0x400, 96, 0x980, 64
VECTOR, VALUE_SLOT = 0xA00, 0xA20
assert FRAMES + NODES * FRAME_STRIDE <= FOUND and INDICES + 4 * 350 + 4 <= NAME_ZONE and INDICES - 4 * 64 == 0x300 and NAME_ZONE + NAME_BYTES <= VECTOR


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


def garbage(n, k):
    v = 0xC0000000 | (rand((n + 0x6A09E667) & M32, k) & 0x3FFFFFFF)
    return 0xC0000000 if v == M32 else v


def image_seed(fn, n):
    return (n + 0x500 + fn * 7919) & M32


def pre_word(name, case, inp, w):
    """palabra w (indice de 32 bits) de la imagen previa de la arena"""
    fn = FN_INDEX[name]
    if name == N_CALLBACK and w == FOUND // 4:
        return inp['found0']
    if name == N_DECISION and w == TASK_SLOT // 4:
        return inp['t0']
    if name == N_DECISION and w == FACIAL_SLOT // 4:
        return inp['f0']
    if name == N_PUSH and VECTOR // 4 <= w < VECTOR // 4 + 4:
        return [inp['alloc'], inp['first'], inp['last'], inp['end']][w - VECTOR // 4]
    if name == N_PUSH and w == VALUE_SLOT // 4:
        return inp['val']
    return garbage(image_seed(fn, case), w)


def changes_of(name, case, inp, post):
    """post = {indice de palabra: valor nuevo}; solo las palabras que realmente cambian"""
    return [[w, v] for w, v in sorted(post.items()) if v != pre_word(name, case, inp, w)]


# ------------------------------------------------------------------------------------------------ 0 FloorFloat
def widen_float(x):
    """fld dword; fstp qword: exacto, NaN silenciados"""
    sign = (x >> 31) << 63
    e, f = (x >> 23) & 0xFF, x & 0x7FFFFF
    if e == 255:
        return sign | 0x7FF0000000000000 | (((f << 29) | 0x0008000000000000) if f else 0)
    if e == 0:
        if not f:
            return sign
        bl = f.bit_length()
        exp = bl - 1 - 149 + 1023
        mant = (f << (52 - (bl - 1))) & 0xFFFFFFFFFFFFF
        return sign | (exp << 52) | mant
    return sign | ((e - 127 + 1023) << 52) | (f << 29)


def floor_ret_bits(mode, seed, lo, hi):
    arg = (hi << 32) | lo
    r = (rand(seed, 1) << 32) | rand(seed, 2)
    if mode == 0:
        v = 0
    elif mode == 1:
        v = 0x8000000000000000
    elif mode == 2:
        v = arg
    elif mode == 3:
        v = arg ^ (r & 0x000FFFFFFFFFFFFF)
    elif mode == 4:
        e = 1 + (rand(seed, 3) % 2046)
        v = (r & 0x8000000000000000) | (e << 52) | (r & 0x000FFFFFFFFFFFFF)
    elif mode == 5:
        v = (r & 0x8000000000000000) | (r & 0x000FFFFFFFFFFFFF) | 1
    elif mode == 6:
        v = (r & 0x8000000000000000) | 0x7FF0000000000000
    else:
        v = (r & 0x8000000000000000) | 0x7FF8000000000000 | (r & 0x0007FFFFFFFFFFFF)
    if ((v >> 52) & 0x7FF) == 0x7FF and (v & 0x000FFFFFFFFFFFFF):
        v |= 0x0008000000000000
    return v & M64


def double_to_ext_hex(d):
    """fld qword: los 10 bytes del registro ST0 (mantisa de 64 bits LE y signo+exponente de 16 bits LE) en hexadecimal"""
    sign = d >> 63
    e, f = (d >> 52) & 0x7FF, d & 0xFFFFFFFFFFFFF
    if e == 0x7FF:
        exp, mant = 0x7FFF, (1 << 63) | (f << 11)
    elif e == 0:
        if f == 0:
            exp, mant = 0, 0
        else:
            bl = f.bit_length()
            exp = (bl - 1 - 1074) + 16383
            mant = f << (64 - bl)
    else:
        exp, mant = e - 1023 + 16383, (1 << 63) | (f << 11)
    return (mant.to_bytes(8, 'little') + (((sign << 15) | exp).to_bytes(2, 'little'))).hex()


def exp_floor(inp, case):
    x = inp['x']
    arg = widen_float(x)
    lo, hi = arg & M32, arg >> 32
    ret = floor_ret_bits(inp['mode'], inp['seed'], lo, hi)
    observed = {'events': [[1, lo, hi]], 'nevents': 1, 'x87': {'depth': 1, 'st0': double_to_ext_hex(ret)}, 'changes': []}
    e, f = (x >> 23) & 0xFF, x & 0x7FFFFF
    re_, rf = (ret >> 52) & 0x7FF, ret & 0xFFFFFFFFFFFFF
    stats = {
        'x_nan': e == 255 and f != 0, 'x_snan': e == 255 and f != 0 and not (f >> 22), 'x_inf': e == 255 and f == 0, 'x_zero': e == 0 and f == 0, 'x_denormal': e == 0 and f != 0,
        'x_neg': bool(x >> 31), 'x_ge_2p23': e >= 150 and e != 255, 'x_fraction': 0 < e < 150 and e >= 100,
        'ret_nan': re_ == 0x7FF and rf != 0, 'ret_inf': re_ == 0x7FF and rf == 0, 'ret_denormal': re_ == 0 and rf != 0, 'ret_negzero': ret == 0x8000000000000000,
        'ret_wide': re_ not in (0, 0x7FF) and (abs(re_ - 1023) > 127 or (rf & 0x1FFFFFFF) != 0),
    }
    for m in range(8):
        stats['mode%d' % m] = inp['mode'] == m
    return observed, stats


# ------------------------------------------------------------------------------------------------ 1 GetAnimHierarchyFromFrameCallback
def exp_callback(inp, case):
    n, parent, hier = inp['n'], inp['parent'], inp['hier']
    events, found = [], [None]
    depth = [0, 0]

    def run(node, d):
        depth[1] = max(depth[1], d)
        events.append([1, node])
        if hier[node]:
            found[0] = hier[node]
            return False
        events.append([2, node, 1, 1])
        for child in range(1, n):
            if parent[child] == node:
                if not run(child, d + 1):
                    break
        return True

    ret_ok = run(inp['root'], 0)
    post = {}
    if found[0] is not None:
        post[FOUND // 4] = found[0]
    observed = {'events': events, 'nevents': len(events), 'ret': inp['root'] if ret_ok else -1, 'traps': 0, 'changes': changes_of(N_CALLBACK, case, inp, post)}
    stats = {
        'found_written': found[0] is not None, 'null_return': not ret_ok, 'recursed': any(e[0] == 2 for e in events), 'hier_at_root': bool(hier[inp['root']]),
        'depth_ge3': depth[1] >= 3, 'nodes_ge8': n >= 8, 'rg_zero': inp['rg'] == 0, 'rg_nonzero': inp['rg'] != 0, 'full_walk_none': found[0] is None,
        'found_unchanged': found[0] is not None and found[0] == pre_word(N_CALLBACK, case, inp, FOUND // 4),
    }
    return observed, stats


# ------------------------------------------------------------------------------------------------ 2 CDecisionMaker::MakeDecision
def exp_decision(inp, case):
    off = ((inp['idx'] & M32) * 0x3C) & M32
    a = [v & M32 for v in inp['a']]
    events = [[1, off, inp['src'] & M32, 1 if (inp['veh'] & 0xFF) else 0] + a + [1, 2]]
    post = {}
    wmode = inp['wmode']
    wt, wf = inp['w']
    if wmode & 1:
        post[TASK_SLOT // 4] = (inp['t0'] & 0xFFFF0000) | (wt & 0xFFFF)
    if wmode & 2:
        post[FACIAL_SLOT // 4] = (inp['f0'] & 0xFFFF0000) | (wf & 0xFFFF)
    observed = {'events': events, 'nevents': 1, 'changes': changes_of(N_DECISION, case, inp, post)}
    veh = inp['veh']
    stats = {
        'veh_canonical': veh in (0, 1), 'veh_noncanonical': veh not in (0, 1), 'veh_low_zero_word_nonzero': (veh & 0xFF) == 0 and veh != 0, 'veh_low_nonzero_not_one': (veh & 0xFF) not in (0, 1),
        'idx_negative': inp['idx'] < 0, 'idx_wrap': (inp['idx'] & M32) * 0x3C > M32, 'idx_small': 0 <= inp['idx'] <= 40,
        'task_write': bool(wmode & 1), 'facial_write': bool(wmode & 2), 'no_write': wmode == 0, 'both_write': wmode == 3,
        'task_changed': TASK_SLOT // 4 in [c[0] for c in observed['changes']], 'facial_changed': FACIAL_SLOT // 4 in [c[0] for c in observed['changes']],
        'src_negative': inp['src'] < 0, 'a_minus1': -1 in inp['a'], 'a_200': 200 in inp['a'],
    }
    return observed, stats


# ------------------------------------------------------------------------------------------------ 3 CDecisionMakerTypes::ReadEventIndices
def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def exp_read(inp, case):
    name, tok, lines = inp['name'].encode('latin-1'), inp['tok'], inp['lines']
    events = []
    post = {INDICES // 4 + i: 0 for i in range(INDEX_COUNT)}      # rep stosd
    events.append([1, fnv(b''), 0])
    events.append([1, fnv(b'data\\decision\\'), 14])
    events.append([2, fnv(name), len(name), fnv(b'r'), 1])
    events.append([1, fnv(b''), 0])
    served = 0
    order = 0
    idx = 0
    stats = {'stale_index': False, 'dup_index': False, 'idx_neg': False, 'idx_ge96': False, 'junk_first': False, 'long_line': False, 'blank_lines': 0, 'nul_lines': 0, 'parsed': 0}
    seen = set()
    k = 0
    while True:
        if k < len(lines):
            events.append([3, tok, 256, k])
        else:
            events.append([3, tok, 256, M32])
            break
        text = lines[k]['t'].encode('latin-1')
        first = text[0] if text else 0
        if first != 0 and first != 0x0A:
            events.append([4, fnv(text), len(text), fnv(b'%s %d'), 5, 1])
            if lines[k]['w']:
                idx = lines[k]['i']
            else:
                stats['stale_index'] = True
            stats['idx_neg'] |= idx < 0
            stats['idx_ge96'] |= idx >= INDEX_COUNT
            stats['dup_index'] |= idx in seen
            seen.add(idx)
            stats['parsed'] += 1
            stats['junk_first'] |= not text.startswith(b'EVENT_') and not text.startswith(b'LAST_') and not text.startswith(b'L_')
            stats['long_line'] |= len(text) > 200
            post[(INDICES // 4) + idx] = order
            order += 1
        elif first == 0x0A:
            stats['blank_lines'] += 1
        else:
            stats['nul_lines'] += 1
        k += 1
    events.append([5, tok])
    observed = {'events': events[:MAX_EVENTS], 'nevents': len(events), 'changes': changes_of(N_READ, case, inp, post)}
    stats.update({
        'lines0': not lines, 'lines_ge8': len(lines) >= 8, 'tok_null': tok == 0, 'name_empty': not name, 'name_long': len(name) >= 30, 'name_pedevent': name == b'PedEvent.txt',
        'has_blank': stats['blank_lines'] > 0, 'has_nul': stats['nul_lines'] > 0, 'parsed_ge1': stats['parsed'] >= 1, 'parsed_ge5': stats['parsed'] >= 5,
        'out_of_range_write': any(not (INDICES // 4 <= w < INDICES // 4 + INDEX_COUNT) for w in post),
    })
    stats = {kk: (vv if isinstance(vv, bool) else None) for kk, vv in stats.items() if isinstance(vv, bool)}
    return observed, stats


# ------------------------------------------------------------------------------------------------ 4 UserTracksLegacyVector::PushBack
def sar2(d):
    d &= M32
    return ((d - (1 << 32) if d >= 1 << 31 else d) >> 2) & M32


def exp_push(inp, case):
    first, last, end = inp['first'], inp['last'], inp['end']
    size, cap = sar2(last - first), sar2(end - first)
    vec = [inp['alloc'], first, last, end]
    fk, fv = inp['fk'], inp['fv']

    def mutate():
        if fk == 1:
            vec[2] = fv
        elif fk == 2:
            vec[1] = fv
        elif fk == 3:
            vec[3] = fv

    fast = first != 0 and size < cap          # comparacion sin signo
    if fast:
        events = [[1, last, 1, 2, 1, 2]]
        mutate()
        vec[2] = (last + 4) & M32              # last leido ANTES de la llamada + 4
    else:
        events = [[2, 1, last, 1, 2]]
        mutate()
    post = {VECTOR // 4 + i: v for i, v in enumerate(vec)}
    observed = {'events': events, 'nevents': 1, 'changes': changes_of(N_PUSH, case, inp, post)}
    stats = {
        'fast': fast, 'insert': not fast, 'first_null': first == 0, 'size_negative': bool(size >> 31), 'cap_negative': bool(cap >> 31), 'full': first != 0 and size == cap,
        'size_gt_cap': first != 0 and size > cap, 'signed_flip': first != 0 and bool(size >> 31) != bool(cap >> 31), 'unsigned_flip': first != 0 and ((size >> 31) or (cap >> 31)) and size < cap,
        'unaligned': ((last - first) & 3) != 0 or ((end - first) & 3) != 0, 'fk1': fk == 1, 'fk2': fk == 2, 'fk3': fk == 3, 'fk_fast': fk != 0 and fast, 'fk_insert': fk != 0 and not fast,
        'last_wraps': fast and last > 0xFFFFFFFB, 'val_null': inp['val'] == 0, 'last_equals_first': first != 0 and last == first,
    }
    return observed, stats


def expected(name, inp, case):
    if name not in FN_INDEX:
        raise KeyError(name)
    return {N_FLOOR: exp_floor, N_CALLBACK: exp_callback, N_DECISION: exp_decision, N_READ: exp_read, N_PUSH: exp_push}[name](inp, case)


def selftest():
    assert double_to_ext_hex(0x3FF0000000000000) == '0000000000000080' + 'ff3f'            # 1.0
    assert double_to_ext_hex(0x8000000000000000) == '00' * 8 + '0080'                        # -0.0
    assert double_to_ext_hex(0x7FF0000000000000) == '0000000000000080' + 'ff7f'              # +inf
    assert double_to_ext_hex(0x0000000000000001) == '0000000000000080' + ('%04x' % (16383 - 1074))[2:] + ('%04x' % (16383 - 1074))[:2]   # menor denormal: 2^-1074
    assert widen_float(0x3F800000) == 0x3FF0000000000000 and widen_float(0x00000001) == 0x36A0000000000000 and widen_float(0x7F800001) == 0x7FF8000020000000
    assert widen_float(0x80000000) == 0x8000000000000000 and widen_float(0xFF800000) == 0xFFF0000000000000 and widen_float(0x007FFFFF) == 0x380FFFFFC0000000
    # F0: modo 2 devuelve el propio argumento
    obs, _ = expected(N_FLOOR, {'x': 0x40490FDB, 'mode': 2, 'seed': 1}, 0)
    assert obs['events'] == [[1, 0x60000000, 0x400921FB]] and obs['x87']['st0'] == double_to_ext_hex(widen_float(0x40490FDB))
    # F1: raiz sin jerarquia con dos hijos (el primero con jerarquia detiene el recorrido)
    inp = {'n': 3, 'root': 0, 'parent': [-1, 0, 0] + [-1] * 7, 'hier': [0, 0x1234, 0x5678] + [0] * 7, 'found0': 5, 'rg': 0}
    obs, _ = expected(N_CALLBACK, inp, 0)
    assert obs['events'] == [[1, 0], [2, 0, 1, 1], [1, 1]] and obs['ret'] == 0 and obs['changes'] == [[FOUND // 4, 0x1234]]
    inp['hier'][0] = 9
    obs, _ = expected(N_CALLBACK, inp, 0)
    assert obs['events'] == [[1, 0]] and obs['ret'] == -1 and obs['changes'] == [[FOUND // 4, 9]]
    # F2
    obs, _ = expected(N_DECISION, {'idx': 2, 'src': -1, 'veh': 0x100, 'a': [1, 2, 3, -1], 't0': 0xC0001234, 'f0': 0xC0005678, 'wmode': 1, 'w': [200, -1]}, 0)
    assert obs['events'] == [[1, 0x78, M32, 0, 1, 2, 3, M32, 1, 2]] and obs['changes'] == [[TASK_SLOT // 4, 0xC00000C8]]
    # F4: un solo evento con salto de linea y una linea sin escribir indice no hay (primera siempre escribe): ver ejemplos de fixtures
    # F5: vector vacio con sitio -> camino rapido
    obs, _ = expected(N_PUSH, {'alloc': 7, 'first': 0x1000, 'last': 0x1000, 'end': 0x1010, 'val': 5, 'fk': 0, 'fv': 0}, 0)
    assert obs['events'] == [[1, 0x1000, 1, 2, 1, 2]] and obs['changes'] == [[VECTOR // 4 + 2, 0x1004]]
    obs, _ = expected(N_PUSH, {'alloc': 7, 'first': 0x1000, 'last': 0x1010, 'end': 0x1010, 'val': 5, 'fk': 0, 'fv': 0}, 0)
    assert obs['events'] == [[2, 1, 0x1010, 1, 2]] and obs['changes'] == []
    obs, _ = expected(N_PUSH, {'alloc': 7, 'first': 0, 'last': 3, 'end': 9, 'val': 5, 'fk': 0, 'fv': 0}, 0)
    assert obs['events'][0][0] == 2
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
