"""Oraculo independiente del banco ObjectCtor28: CObject::CObject(int32 modelId, bool bCreate) en 0x5A1D70.

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x5A1D70 (thiscall, ECX = this; tras `push -1 / push 83D2C8h / fs:[0] / push ecx / push esi`: [esp+18h] = modelId, [esp+1Ch] = bCreate; `ret 8`):
  5A1D87 esi = ecx
  5A1D8D call 542260                    CPhysical::CPhysical(this)                         (colaborador SUSTITUIDO por el falso del banco)
  5A1D92 cl = byte [esp+1Ch]            SOLO el byte bajo del argumento bCreate
  5A1D96 eax = 0 ; cmp cl,al ; [esp+10h] = 0 (estado de desenrollado)
  5A1D9E [esi]       = 866F60h          vptr de CObject
  5A1DA4 [esi+170h]  = eax (0)          m_pDummyObject
  5A1DAA je 5A1DBA                      byte bajo == 0 -> NoCreate
  5A1DAC (bCreate) eax = [esp+18h] ; push eax ; ecx = esi ; call 532AE0     CEntity::SetModelIndex(modelId)          (SUSTITUIDO)
  5A1DBA (no create) ecx = [esp+18h] ; push ecx ; ecx = esi ; call 533700    CEntity::SetModelIndexNoCreate(modelId)  (SUSTITUIDO)
  5A1DC6 ecx = esi ; call 59F840        CObject::Init(this)                                                                (SUSTITUIDO)
  5A1DCD ecx = [esp+8] (fs:[0] anterior) ; eax = esi ; esi restaurado ; fs:[0] = ecx ; add esp,10h ; ret 8      -> EAX = this
El constructor no escribe nada mas (el byte +0x13C = 0 es del constructor por defecto 0x5A1D10, no de este).

Falsos (iguales en ambos lados, ver Core.hpp):
  FakePhys(this):          dwords [0,0x138) = Rand(seed_p, k) con k = indice de dword; dword 0 = 0xBA5E0000; traza [1, rel, 0, 0, 0]
  FakeSetModel(this, i):   traza [2, rel, i, vptr (etiquetado), dword +0x170]; word +0x22 = i & 0xFFFF; word +0x24 = 0x5E01
  FakeNoCreate(this, i):   traza [3, rel, i, vptr (etiquetado), dword +0x170]; word +0x22 = i & 0xFFFF; word +0x24 = 0x5E02
  FakeInit(this):          traza [4, rel, vptr (etiquetado), dword +0x170, word +0x22]; dwords de indice IMPAR k de [0x138,0x17C) = Rand(seed_i, k)
El vptr visto por los falsos (y el final) es la etiqueta 0xC0B1EC70 solo cuando es el vptr propio del lado.
"""
import sys

M32 = 0xFFFFFFFF
CASES = 6144
NAME = 'CObject::CObject'
NAMES = [NAME]
CASES_BY_FN = {NAME: CASES}
OBJ = 0x17C
PHYS = 0x138
INIT_AT = 0x138
DUMMY_AT = 0x170
MODEL_AT = 0x22
MARK_AT = 0x24
GUARD = 0x40
ARENA = GUARD + 0x10 + OBJ + GUARD
OBJ_OFFSETS = (0x40, 0x44, 0x48, 0x4C)
BASE_VPTR_MARKER = 0xBA5E0000
VT_LABEL = 0xC0B1EC70
EXE_VPTR = 0x866F60

SPECIAL_MODELS = (
    0, 1, 2, 0xFFFFFFFF, 0x80000000, 0x7FFFFFFF, 0xFFFF, 0x10000, 0x7FFF, 0x8000, 0xFFFF8000, 0xFFFF7FFF, 19999, 20000, 0xFFFFFF00, 0x00010000, 0xDEADBEEF,
    0xFFFF0000, 0x0000FF00, 0x000000FF, 0x100, 0x80, 0x7F, 0x8000FFFF, 0xFFFFFFFE, 0x80000001, 0x7FFFFFFE, 18631, 1337, 0x00FFFFFF, 0x01000000,
)
CREATE_NONZERO_LOW = (0x02, 0x80, 0xFF, 0x7F, 0x01010101, 0xAB0000CD, 0xFFFFFF01, 0x101, 0x12345601, 0x81)
CREATE_ZERO_LOW = (0x100, 0x01000000, 0xFFFFFF00, 0xAB000000, 0x00010000, 0x7FFFFF00, 0x80000000, 0x00FF0000, 0x12345600, 0xDEADBE00)


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


def fill_arena(mode, seed):
    if mode == 1:
        return bytes(ARENA)
    if mode == 2:
        return b'\xff' * ARENA
    if mode == 3:
        return b'\xcd' * ARENA
    if mode == 4:
        return b'\xa5' * ARENA
    return fill_words(ARENA, seed)


STACK_SEEDS = (0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555)


def stack_seed(n):
    return STACK_SEEDS[n] if n < len(STACK_SEEDS) else rand(n + 0x1000, 99)


def make(n):
    """Espejo EXACTO de ObjectCtor28Fix::Make (Fixtures.hpp); se contrasta con el generador C++ (ver README.md) y sirve para calibrar la cobertura sin el juego."""
    f = {'n': n}
    mk = rand(n, 1) % 8
    f['model_kind'] = mk
    if mk == 0:
        model = rand(n, 11) % 41
    elif mk in (1, 2):
        model = rand(n, 11) % 20000
    elif mk in (3, 4):
        model = SPECIAL_MODELS[rand(n, 11) % len(SPECIAL_MODELS)]
    elif mk == 5:
        model = rand(n, 11)
    elif mk == 6:
        model = (-(1 + rand(n, 11) % 40000)) & M32
    else:
        model = rand(n, 11) & 0xFFFF
    f['model_id'] = model
    r = rand(n, 2) % 10
    ck = 0 if r < 4 else 1 if r < 8 else 2 if r == 8 else 3
    f['create_kind'] = ck
    f['create'] = (0, 1, CREATE_NONZERO_LOW[rand(n, 12) % len(CREATE_NONZERO_LOW)], CREATE_ZERO_LOW[rand(n, 12) % len(CREATE_ZERO_LOW)])[ck]
    f['mode'] = rand(n, 3) % 6
    f['obj_off'] = OBJ_OFFSETS[rand(n, 4) % 4]
    seed_arena = rand(n, 5) | 1
    f['seed_p'] = rand(n, 6)
    f['seed_i'] = rand(n, 7)
    f['eax'] = 0 if rand(n, 8) % 5 == 0 else rand(n, 9)
    f['edx'] = 0xFFFFFFFF if rand(n, 10) % 5 == 0 else rand(n, 13)
    f['stack_seed'] = stack_seed(rand(n, 14) % 12)
    arena = fill_arena(f['mode'], seed_arena)
    f['pre'] = arena[f['obj_off']:f['obj_off'] + OBJ].hex()
    return f


def check_input(inp, case):
    """Coherencia de la fila `input` con las familias del generador (rechaza una fila fabricada o un generador roto)."""
    mk, ck, model, create = inp['model_kind'], inp['create_kind'], inp['model_id'] & M32, inp['create'] & M32
    ok = (0 <= mk < 8 and 0 <= ck < 4 and 0 <= inp['mode'] < 6 and inp['obj_off'] in OBJ_OFFSETS and len(bytes.fromhex(inp['pre'])) == OBJ)
    ok = ok and {0: model < 41, 1: model < 20000, 2: model < 20000, 3: model in SPECIAL_MODELS, 4: model in SPECIAL_MODELS, 5: True,
                 6: ((-model) & M32) in range(1, 40001), 7: model <= 0xFFFF}[mk]
    ok = ok and {0: create == 0, 1: create == 1, 2: (create & 0xFF) != 0 and create != 1, 3: (create & 0xFF) == 0 and create != 0}[ck]
    if not ok:
        raise RuntimeError('Fila input incoherente en el caso %d: %s' % (case, {k: v for k, v in inp.items() if k != 'pre'}))


def build(inp):
    """Objeto final, traza y rama de un caso, segun el desensamblado + los falsos del banco."""
    obj = bytearray(bytes.fromhex(inp['pre']))
    model, create = inp['model_id'] & M32, inp['create'] & M32
    trace = []
    # 5A1D8D  CPhysical::CPhysical(this)  (falso)
    trace.append([1, 0, 0, 0, 0])
    obj[0:PHYS] = fill_words(PHYS, inp['seed_p'])
    obj[0:4] = BASE_VPTR_MARKER.to_bytes(4, 'little')
    # 5A1D9E  vptr ; 5A1DA4  m_pDummyObject = 0
    obj[0:4] = VT_LABEL.to_bytes(4, 'little')
    obj[DUMMY_AT:DUMMY_AT + 4] = bytes(4)
    # 5A1D92..5A1DC1  seleccion por el BYTE BAJO de bCreate
    creates = (create & 0xFF) != 0
    trace.append([2 if creates else 3, 0, model, VT_LABEL, 0])
    obj[MODEL_AT:MODEL_AT + 2] = (model & 0xFFFF).to_bytes(2, 'little')
    obj[MARK_AT:MARK_AT + 2] = (0x5E01 if creates else 0x5E02).to_bytes(2, 'little')
    # 5A1DC8  CObject::Init(this)  (falso)
    trace.append([4, 0, VT_LABEL, 0, model & 0xFFFF])
    for k in range(1, (OBJ - INIT_AT) // 4, 2):
        obj[INIT_AT + 4 * k:INIT_AT + 4 * k + 4] = rand(inp['seed_i'], k).to_bytes(4, 'little')
    return bytes(obj), trace, creates


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    check_input(inp, case)
    obj, trace, creates = build(inp)
    model, create = inp['model_id'] & M32, inp['create'] & M32
    pre = bytes.fromhex(inp['pre'])
    stats = {
        'create_branch': creates, 'nocreate_branch': not creates,
        'model_neg': bool(model & 0x80000000), 'model_ge_65536_pos': 0x10000 <= model < 0x80000000, 'model_real': model < 20000,
        'model_20000_65535': 20000 <= model <= 0xFFFF, 'model_u16_trunc': model != (model & 0xFFFF), 'model_zero': model == 0,
        'model_minus1': model == 0xFFFFFFFF, 'model_int_min': model == 0x80000000, 'model_int_max': model == 0x7FFFFFFF,
        'model_low16_zero': (model & 0xFFFF) == 0 and model != 0, 'model_low16_sign': bool(model & 0x8000),
        'create_noncanon_true': creates and create != 1, 'create_noncanon_false': (not creates) and create != 0,
        'create_high_bits_set': (create >> 8) != 0, 'pre_all_zero': set(pre) == {0}, 'pre_all_ff': set(pre) == {0xFF},
        'eax_zero': inp['eax'] == 0, 'edx_ff': inp['edx'] == 0xFFFFFFFF,
    }
    for key in ('model_kind', 'create_kind', 'mode'):
        stats['%s%d' % (key, inp[key])] = True
    stats['obj_off_%x' % inp['obj_off']] = True
    return {'obj': obj.hex(), 'vptr_own': True, 'guard_ok': True, 'trace': trace, 'eax_is_this': True}, stats


if __name__ == '__main__':
    # 1) caso minimo escrito a mano: byte bajo != 0 -> SetModelIndex; el resto de los bytes del objeto salen de los falsos
    pre = bytes(range(256)) * 2
    inp = dict(model_kind=5, create_kind=2, mode=0, obj_off=0x40, model_id=0xFFFF8123, create=0x101, seed_p=7, seed_i=9, pre=pre[:OBJ].hex(), eax=0, edx=0, stack_seed=0)
    want, st = expected(NAME, inp, 0)
    obj = bytes.fromhex(want['obj'])
    assert want['trace'] == [[1, 0, 0, 0, 0], [2, 0, 0xFFFF8123, VT_LABEL, 0], [4, 0, VT_LABEL, 0, 0x8123]], want['trace']
    assert obj[0:4] == VT_LABEL.to_bytes(4, 'little') and obj[0x22:0x26] == bytes([0x23, 0x81, 0x01, 0x5E])
    assert obj[4:0x22] == fill_words(PHYS, 7)[4:0x22] and obj[0x26:0x138] == fill_words(PHYS, 7)[0x26:0x138]
    assert obj[0x170:0x174] == bytes(4)                                   # k = 14, par: lo deja el constructor
    assert obj[0x138:0x13C] == pre[0x138:0x13C]                           # k = 0, par: no lo toca Init
    assert obj[0x13C:0x140] == rand(9, 1).to_bytes(4, 'little')           # k = 1, impar: ruido de Init
    assert obj[0x174:0x178] == rand(9, 15).to_bytes(4, 'little')          # k = 15, impar
    assert obj[0x178:0x17C] == pre[0x178:0x17C]                           # k = 16, par: conserva la fuente
    assert st['create_branch'] and st['create_noncanon_true'] and st['model_neg'] and st['model_u16_trunc']
    # 2) byte bajo == 0 -> NoCreate aunque el resto del dword sea != 0
    want, st = expected(NAME, dict(inp, create_kind=3, create=0xFFFFFF00), 0)
    assert want['trace'][1][0] == 3 and bytes.fromhex(want['obj'])[0x24:0x26] == bytes([0x02, 0x5E]) and st['nocreate_branch'] and st['create_noncanon_false']
    # 3) cobertura del generador completo (espejo de Fixtures.hpp) y coherencia de todas las filas
    cov = {}
    for n in range(CASES):
        f = make(n)
        _, s = expected(NAME, f, n)
        for k, v in s.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
    assert cov['create_branch'] > 2500 and cov['nocreate_branch'] > 2500 and cov['model_neg'] > 800 and cov['model_real'] > 1500 and cov.get('create_noncanon_false', 0) > 300
    if len(sys.argv) > 1 and sys.argv[1] == '--coverage':
        import json
        print(json.dumps(cov, indent=1, sort_keys=True))
    print('ok')
