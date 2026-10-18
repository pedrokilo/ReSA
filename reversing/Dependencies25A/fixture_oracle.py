"""Oraculo independiente del banco Dependencies25A (grupo g25a de BRIEF_21).

Cada funcion se modela aqui desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL), a partir de la fila `input` que registra el banco:
  0 CTask::Destructor                 0x61A3A0  mov dword ptr [ecx], 0x86D48C ; ret
  1 CVehiclePool::GetAt               0x41CC10  mov edx,[ecx+4]; mov eax,[esp+4]; cmp byte [edx+eax],0; jns L; xor eax,eax; ret 4; L: mov edx,[ecx]; imul eax,eax,0xA18; add eax,edx; ret 4
  2 CAutoPilot::RemoveOnePathNode     0x41B950  dec word [ecx+0x88]; ... bucle de 16 bits (ver sim_f2)
  3 CPedGroups::RemoveAllFollowersFromGroup 0x5FB8A0  mov ecx,[esp+4]; mov al,[ecx+0xC098E0]; test al,al; je ret; imul ecx,ecx,0x2D4; add ecx,0xC09920; jmp 0x5FB7D0
  4 CDecisionMakerTypes::FlushDecisionMakerEventResponse 0x604490  edx=[ecx+b*4+0xC034]; eax=a*0x29; edx=(edx+eax)*0x3C; ecx=edx+ecx+4; call 0x600530
  5 CGarages::CountCarsInHideoutGarage 0x44A210  movzx eax,byte [esp+4]; push; xor edx,edx; call 0x4489F0; shl eax,8; add eax,0x96ABEA; cuenta 4 palabras de 2 bytes distintas de 0 (paso 0x40)
  6 FindReplacementUpgradeCB          0x6D3490  tipo 1; GetAtomicId (0x732370) & 0x800; GetModelInfoIndex (0x732250) != -1 -> data[1] = object; nullptr
  7 GetAnimHierarchyFromFrameCallback 0x734A70  RpHAnimFrameGetHierarchy (0x7C5160); si no: RwFrameForAllChildren (0x7F0DC0) con ella misma; devuelve el marco
  8 CAEStreamingDecoder scalar deleting destructor 0x4F2860  [this]=0x85F418; si [this+4]: 0x4DC490 + delete(filename) + delete(stream) + [this+4]=0; si flags&1: delete(this); EAX=this
Mix/Rand/FillWords/Fnv64 repiten exactamente Fixtures.hpp.
"""
import struct

M32 = 0xFFFFFFFF
CASES = 2048
NAMES = ['CTask::Destructor', 'CVehiclePool::GetAt', 'CAutoPilot::RemoveOnePathNode', 'CPedGroups::RemoveAllFollowersFromGroup',
         'CDecisionMakerTypes::FlushDecisionMakerEventResponse', 'CGarages::CountCarsInHideoutGarage', 'FindReplacementUpgradeCB',
         'RwHelper::GetAnimHierarchyFromFrameCallback', 'CAEStreamingDecoder::ScalarDeletingDestructor']


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


def fill_words(seed, nbytes):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rand(seed, i // 4))[:min(4, nbytes - i)]
    return bytes(out)


def fnv64(data):
    h = 0xCBF29CE484222325
    for b in data:
        h ^= b
        h = (h * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - 0x100000000 if v & 0x80000000 else v


def le32(v):
    return struct.pack('<I', v & M32)


def hx(b):
    return bytes(b).hex()


# ---------------------------------------------------------------------------------------------------------------- modelos
def model_f0(row, case):
    obj = bytes.fromhex(row['object_in'])
    return {'object': hx(le32(0x86D48C) + obj[4:]), 'guards_ok': True, 'eax': row['eax'], 'ecx_is_object': True, 'edx': row['edx']}, {}


def model_f1(row, case):
    idx, storage, state = row['idx'], row['storage'], row['state_byte']
    eax = 0 if state & 0x80 else (storage + idx * 0xA18) & M32
    return {'eax': eax, 'pool_unchanged': True, 'states_unchanged': True}, {'null': eax == 0}


def sim_f2(row):
    window = row['window']
    buf = bytearray(fill_words(row['seed'], window))
    struct.pack_into('<H', buf, 0x88, row['count'] & 0xFFFF)
    struct.pack_into('<I', buf, 0x8C, row['w8c'])
    if hx(buf[:0xA0]) != row['head_in']:
        raise RuntimeError('head_in distinto del flujo del oraculo')
    rd16 = lambda o: struct.unpack_from('<H', buf, o)[0]
    rd32 = lambda o: struct.unpack_from('<I', buf, o)[0]
    struct.pack_into('<H', buf, 0x88, (rd16(0x88) - 1) & 0xFFFF)   # dec word ptr [ecx+0x88]
    i = 0
    iterations = 0
    if s16(rd16(0x88)) > 0:                                        # test dx,dx ; jle
        while True:
            slot = i
            nxt = rd32(0x6C + 4 * slot)                            # mov esi,[ecx+edx*4+0x6C]
            i = s16(i + 1)                                         # inc eax
            struct.pack_into('<I', buf, 0x68 + 4 * slot, nxt)      # mov [ecx+edx*4+0x68],esi
            iterations += 1
            if not (i < s16(rd16(0x88))):                          # cmp ax,[ecx+0x88] ; jl
                break
    return buf, iterations


def model_f2(row, case):
    buf, iterations = sim_f2(row)
    window = row['window']
    first = s16((row['count'] - 1) & 0xFFFF)
    return ({'hash': fnv64(buf[:window]), 'head': hx(buf[:0xA0]), 'guard_ok': True, 'count_after': struct.unpack_from('<H', buf, 0x88)[0]},
            {'iterations': iterations, 'skipped': first <= 0, 'aliased': first >= 9 and struct.unpack_from('<H', buf, 0x88)[0] != ((row['count'] - 1) & 0xFFFF)})


def model_f3(row, case):
    region = bytes.fromhex(row['region'])
    at = (0xC098E0 + row['id']) & M32
    if 0xC098D0 <= at < 0xC09920 and region[at - 0xC098D0] != row['byte_at_id']:
        raise RuntimeError('byte_at_id distinto de la region')
    called = row['byte_at_id'] != 0
    trace = [(0xC09920 + (row['id'] * 0x2D4)) & M32] if called else []
    return {'trace': trace, 'region_after': row['region']}, {'called': called, 'in_range': 0 <= row['id'] < 8}


def model_f4(row, case):
    a, b, raw = row['a'], row['b'], row['event_index_raw']
    index_words = (0xC034 + 4 * b)
    if index_words % 4 or not 0 <= index_words < 0xF1C0:
        raise RuntimeError('indice de evento fuera del arena')
    if raw != rand(row['seed'], index_words // 4):
        raise RuntimeError('event_index_raw distinto del flujo del oraculo')
    offset = ((raw + (a * 0x29 & M32)) & M32) * 0x3C & M32
    return {'trace': [(offset + 4) & M32], 'arena_unchanged': True}, {}


MODEL_SPECIAL = [1, 2, 0x100, 0x8000, 0xFF00, 0xFFFF, 0x00FF]


def f5_models(case, row):
    """Palabras de modelo del refugio probado, repitiendo MakeF5/CaseF5 (solo para indices 0..19)."""
    seed, index = row['seed'], row['index']
    if not 0 <= index < 20:
        return None
    zero_mask = rand(case, 4)
    words = []
    for k in range(4):
        r = rand(seed, 1000 + index * 4 + k)
        model = 0 if r % 3 == 0 else MODEL_SPECIAL[(r >> 24) % 7] if (r >> 20) % 5 == 0 else ((r >> 8) | 1) & 0xFFFF
        mode = (zero_mask >> (2 * k)) & 3
        if mode == 0:
            model = 0
        elif mode == 1:
            model = (rand(seed, 2000 + k) | 1) & 0xFFFF
        words.append(model)
    return words


def model_f5(row, case):
    words = row['model_words']
    expected_words = f5_models(case, row)
    if expected_words is not None and expected_words != words:
        raise RuntimeError('model_words distintas de las del oraculo: %s != %s' % (words, expected_words))
    count = sum(1 for w in words if w != 0)
    return {'eax': count, 'trace': [row['type_arg'] & 0xFF], 'table_unchanged': True, 'models_after': words}, {'count': count, 'in_table': 0 <= row['index'] < 20}


def model_f6(row, case):
    data = bytearray.fromhex(row['data_in'])
    trace = []
    eax = row['object_ptr']
    stored = False
    branch = 'not_atomic'
    if row['type'] == 1:
        trace += [1, 1]
        if row['atomic_id'] & 0x800:
            branch = 'upgrade_bit'
        else:
            trace += [2, 1]
            if s32(row['model_index']) == -1:
                branch = 'index_minus_one'
            else:
                stored = True
                eax = 0
                data[4:8] = le32(row['object_ptr'])
                branch = 'stored'
    return {'eax': eax, 'data': hx(data), 'data_guard_ok': True, 'object_unchanged': True, 'trace': trace}, {'branch': branch, 'stored': stored}


def model_f7(row, case):
    n = row['node_count']
    hier, parent = row['hierarchy'], row['parent']
    children = [[] for _ in range(8)]
    for i in range(1, n):
        children[parent[i]].append(i)
    state = {'found': row['found_in'], 'trace': [], 'depth': 0, 'max_depth': 0, 'calls': 0}

    def callback(i):
        state['calls'] += 1
        state['trace'] += [1, i]
        h = hier[i]
        if h:
            state['found'] = h
            return None
        state['trace'] += [2, i, 1, 1]
        state['depth'] += 1
        state['max_depth'] = max(state['max_depth'], state['depth'])
        for c in children[i]:
            if callback(c) is None:
                break
        state['depth'] -= 1
        return i

    r = callback(0)
    return ({'returned_node': -1 if r is None else r, 'found': state['found'], 'trace': state['trace']},
            {'depth': state['max_depth'], 'direct': bool(hier[0]), 'calls': state['calls'], 'null': r is None})


def model_f8(row, case):
    decoder_in = bytes.fromhex(row['decoder_in'])
    stream = bytearray.fromhex(row['stream_in'])
    present = bool(row['stream_present'])
    flags = row['flags']
    trace = []
    decoder_after = le32(0x85F418) + (le32(0) if present else decoder_in[4:8])
    name_field = struct.unpack_from('<I', stream, 8)[0]
    if present:
        if name_field:
            trace.append({1: 3, 2: 0xDEADBEE0, 3: 0x00BADF00}[row['filename_kind']])
            stream[8:12] = le32(0)
        trace.append(2)
    if flags & 1:
        trace.append(1)
    stream[0:4] = le32(0)   # el primer dword (vtable del flujo) se enmascara
    return ({'eax_is_decoder': True, 'decoder': hx(decoder_after), 'stream': hx(stream), 'stream_guard_ok': True, 'trace': trace},
            {'present': present, 'bit0': bool(flags & 1), 'kind': row['filename_kind'] if present else -1, 'deleted_name': present and bool(name_field)})


MODELS = [model_f0, model_f1, model_f2, model_f3, model_f4, model_f5, model_f6, model_f7, model_f8]


def expected(function, row, case):
    """(observed esperado, estadisticas de cobertura)."""
    return MODELS[NAMES.index(function)](row, case)
