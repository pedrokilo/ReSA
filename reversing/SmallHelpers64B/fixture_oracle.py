"""Oraculo independiente del banco SmallHelpers64B: tres funciones pequenas, modeladas desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input`
que registra el banco. Devuelve lo que debe observarse en AMBOS lados (EXE con el hook apagado y DLL con el hook encendido).

--------------------------------------------------------------------------------------------------------------------------------
GetPopcycleGroupPedModel (0x406E80, cdecl (int popcycleGroup, int slot, int worldZone), 38 bytes, `ret` sin limpiar)
  406E80  mov eax,[esp+4]            ; grupo
  406E84  mov ecx,[esp+0Ch]          ; zona
  406E88  lea edx,[ecx+eax*2]        ; zona + 2 grupo
  406E8B  mov ecx,[esp+8]            ; slot
  406E8F  add eax,edx                ; 3 grupo + zona
  406E91  mov eax,[eax*4+8D2540h]    ; g = CPopulation::m_TranslationArray (3 dwords por grupo)
  406E98  imul eax,eax,15h           ; g * 21
  406E9B  add eax,ecx                ; + slot
  406E9D  movzx eax,word [eax*2+0C0F358h]   ; CPopulation::m_PedGroups (21 words por grupo de peatones); EAX = palabra cero-extendida
  406EA5  ret
  Todo el calculo es de 32 bits con envoltura; no hay comprobacion de limites. ECX queda con el slot y EDX con la zona + 2 grupo (solo se registran).

FlushWaterRenderBuffer (0x6E7680, cdecl, sin argumentos, 84 bytes)
  eax = [0xC4B950] (vertices); si eax == 0 salta al final (comparacion por igualdad de 32 bits)
  call 0x541330 (LittleTest: ++[0xB73708])
  call 0x7EF450 (RwIm3DTransform)(0xC4D958, [0xC4B950], 0, 1)   ; cdecl; test eax,eax (32 bits)
  si EAX != 0:  call 0x7EF550 (RwIm3DRenderIndexedPrimitive)(3, 0xC4B958, [0xC4B954]) ; call 0x7EF520 (RwIm3DEnd)
  al final siempre: [0xC4B950] = 0 y [0xC4B954] = 0 (tambien cuando no habia vertices)
  El contador de indices se lee DESPUES de la transformacion (nada lo cambia entre medias en el banco).

CTaskComplex::~CTaskComplex, cuerpo (0x61A3D0, thiscall sin argumentos de pila, ECX = this)
  marco SEH en fs:[0] (se restaura al salir); [this] = 0x86D4CC (vtable de CTaskComplex);
  ecx = [this+8]; si ecx != 0: push 1; call [[ecx]] (destructor virtual con borrado de la subtarea; ESP se limpia en el callee);
  [this+8] = 0; [this] = 0x86D48C (vtable de CTask); sale. No toca ningun otro byte.
"""
M32 = 0xFFFFFFFF
NAMES = ['GetPopcycleGroupPedModel', 'FlushWaterRenderBuffer', 'CTaskComplex::Destructor']
NAME_MODEL, NAME_FLUSH, NAME_TASK = NAMES
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}

# Constantes de arena: IDENTICAS a las de Fixtures.hpp (se comprueban con fixture_mirror.py)
NUM_GROUPS, NUM_PED_GROUPS = 33, 57
TRANS_BASE, TRANS_GUARD, TRANS_MAIN, TRANS_BYTES = 0x8D2540, 64, 396, 524
PED_BASE, PED_GUARD, PED_MAIN, PED_BYTES = 0xC0F358, 128, 2394, 2650
ADDR_VERTS, ADDR_INDICES = 0xC4D958, 0xC4B958
TASK_OBJ, TASK_OBJ_BYTES, TASK_ARENA = 0x20, 0x10, 0x50
VT_COMPLEX, VT_TASK = 0x86D4CC, 0x86D48C
assert NUM_GROUPS * 12 == TRANS_MAIN and TRANS_GUARD + TRANS_MAIN + TRANS_GUARD == TRANS_BYTES
assert NUM_PED_GROUPS * 42 == PED_MAIN and PED_GUARD + PED_MAIN + PED_GUARD == PED_BYTES
assert TASK_OBJ + TASK_OBJ_BYTES + 0x20 == TASK_ARENA == 0x50


# ------------------------------------------------------------------------------------------------ generador de numeros (mismo Mix / Rand / FillWords que Fixtures.hpp)
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
        out += rand(seed, i // 4).to_bytes(4, 'little')
    return bytes(out[:nbytes])


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


STACK_SEEDS = (0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000)


# ------------------------------------------------------------------------------------------------ 0: modelo
def expected_model(inp):
    seed_t, seed_p = inp['seed_t'], inp['seed_p']
    # ventana de traduccion: dword i = (rand(seed_t, i) % 60) - 1
    def trans(window_index):
        return (rand(seed_t, window_index) % 60 - 1) & M32
    ped = fill_words(PED_BYTES, seed_p)
    index = (inp['group'] * 3 + inp['zone']) & M32                       # eax = 3 grupo + zona (32 bits)
    byte_addr = (TRANS_BASE + index * 4) & M32
    off = s32(byte_addr - TRANS_BASE)
    assert -TRANS_GUARD <= off and off + 4 <= TRANS_GUARD + TRANS_MAIN + TRANS_GUARD - TRANS_GUARD, ('ventana de traduccion', inp)
    g = trans((off + TRANS_GUARD) // 4)
    model_index = (g * 21 + inp['slot']) & M32
    addr = (PED_BASE + model_index * 2) & M32
    poff = s32(addr - PED_BASE)
    assert -PED_GUARD <= poff and poff + 2 <= PED_GUARD + PED_MAIN + PED_GUARD - PED_GUARD, ('ventana de grupos', inp)
    word = int.from_bytes(ped[poff + PED_GUARD:poff + PED_GUARD + 2], 'little')
    gs = s32(g)
    stats = dict(
        family0=inp['family'] == 0, family1=inp['family'] == 1, family2=inp['family'] == 2, family3=inp['family'] == 3,
        g_minus1=gs == -1, g_high=gs >= NUM_PED_GROUPS, g_zero=gs == 0, g_mid=0 < gs < NUM_PED_GROUPS - 1,
        slot_neg=s32(inp['slot']) < 0, slot_high=s32(inp['slot']) > 20, zone_out=not (0 <= s32(inp['zone']) <= 2),
        group_neg=s32(inp['group']) < 0, group_high=s32(inp['group']) >= NUM_GROUPS,
        value_high_bit=word >= 0x8000, value_zero=word == 0, value_ffff=word == 0xFFFF,
        stack_seed=inp['stack_seed'] in STACK_SEEDS,
    )
    return {'ret_eax': word, 'tables_unchanged': True}, stats


# ------------------------------------------------------------------------------------------------ 1: vaciado del bufer de agua
def expected_flush(inp):
    nvtx, nidx, draw = inp['n_vtx'], inp['n_idx'], inp['draw']
    events = []
    d = draw
    if nvtx != 0:
        d = (d + 1) & M32
        events.append([1, ADDR_VERTS, nvtx, 0, 1, d])
        if inp['ret_t'] != 0:
            events.append([2, 3, ADDR_INDICES, nidx, d])
            events.append([3, d])
    obs = {'events': events, 'nevents': len(events), 'n_vtx_after': 0, 'n_idx_after': 0, 'draw_after': d}
    stats = dict(
        vtx_zero=nvtx == 0, vtx_pos=0 < nvtx < 0x80000000, vtx_neg=nvtx >= 0x80000000, vtx_hi_bit16=nvtx >= 0x10000 and nvtx < 0x80000000,
        idx_zero=nidx == 0, idx_neg=nidx >= 0x80000000, idx_big=0x10000 <= nidx < 0x80000000,
        ret_zero=inp['ret_t'] == 0, ret_low_zero=inp['ret_t'] != 0 and (inp['ret_t'] & 0xFF) == 0, ret_nonzero=inp['ret_t'] != 0,
        full_flush=nvtx != 0 and inp['ret_t'] != 0, transform_only=nvtx != 0 and inp['ret_t'] == 0,
        draw_wrap=draw == 0xFFFFFFFF and nvtx != 0, draw_signed_edge=draw == 0x7FFFFFFF and nvtx != 0,
        stack_seed=inp['stack_seed'] in STACK_SEEDS,
    )
    return obs, stats


# ------------------------------------------------------------------------------------------------ 2: destructor de CTaskComplex
def expected_task(inp):
    arena = bytearray(fill_words(TASK_ARENA, inp['guard_seed']))
    sub = inp['sub']
    events = []
    if sub:
        # push 1; call [vtable]: el dueno tiene en ese momento la vtable de CTaskComplex, aun apunta a la subtarea y conserva el padre
        events.append([1, sub, 1, VT_COMPLEX, 1, inp['parent']])
    words = [VT_TASK, inp['parent'], 0, inp['tail']]
    obs = {'events': events, 'nevents': len(events), 'obj': words, 'outside_unchanged': True, 'sub_unchanged': True}
    stats = dict(
        sub_null=sub == 0, sub_a=sub == 1, sub_b=sub == 2, vtable_exe=inp['vtable'] == VT_COMPLEX, vtable_zero=inp['vtable'] == 0, vtable_other=inp['vtable'] not in (0, VT_COMPLEX),
        parent_zero=inp['parent'] == 0, parent_set=inp['parent'] != 0, stack_seed=inp['stack_seed'] in STACK_SEEDS,
    )
    return obs, stats


def expected(fn, inp, case, observed=None):
    if fn == NAME_MODEL:
        return expected_model(inp)
    if fn == NAME_FLUSH:
        return expected_flush(inp)
    if fn == NAME_TASK:
        return expected_task(inp)
    raise KeyError(fn)


# ------------------------------------------------------------------------------------------------ autoprueba
def selftest():
    assert mix(0) == 0 and rand(1, 1) == mix((0x9E3779B1 + 0x85EBCA6B + 12345) & M32)
    # modelo: caso a mano con indices pequenos
    want, _ = expected_model(dict(group=0, slot=0, zone=0, seed_t=7, seed_p=9, stack_seed=0, family=0))
    g = (rand(7, TRANS_GUARD // 4) % 60 - 1) & M32
    ped = fill_words(PED_BYTES, 9)
    poff = s32(g * 21 * 2) + PED_GUARD
    assert want['ret_eax'] == int.from_bytes(ped[poff:poff + 2], 'little')
    # el flujo del vaciado: con vertices y Transform != 0 hay 3 eventos; sin vertices ninguno, y el contador de dibujados no cambia
    obs, _ = expected_flush(dict(n_vtx=5, n_idx=6, draw=0xFFFFFFFF, ret_t=0x100, stack_seed=1))
    assert obs['nevents'] == 3 and obs['draw_after'] == 0 and obs['events'][0] == [1, ADDR_VERTS, 5, 0, 1, 0]
    obs, _ = expected_flush(dict(n_vtx=0, n_idx=6, draw=5, ret_t=1, stack_seed=1))
    assert obs['nevents'] == 0 and obs['draw_after'] == 5 and obs['n_idx_after'] == 0
    obs, _ = expected_flush(dict(n_vtx=0x80000000, n_idx=6, draw=5, ret_t=0, stack_seed=1))
    assert obs['nevents'] == 1 and obs['draw_after'] == 6
    # destructor
    obs, _ = expected_task(dict(sub=2, vtable=0, parent=0x1234, tail=9, guard_seed=1, stack_seed=1))
    assert obs['obj'] == [VT_TASK, 0x1234, 0, 9] and obs['events'] == [[1, 2, 1, VT_COMPLEX, 1, 0x1234]]
    print('ok')


if __name__ == '__main__':
    selftest()
