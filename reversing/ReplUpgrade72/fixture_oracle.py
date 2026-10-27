"""Oraculo independiente del banco ReplUpgrade72 (lote 72): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de CVehicle::AddReplacementUpgrade (0x6D3830)
para una fila `input` (la especificacion semantica del caso que registra Core.hpp).

Desensamblado (0x6D3830..0x6D39D4, thiscall, ret 8, sin valor de retorno; EBX=this, EDI=marco, EBP=model info, ESI=atomo; locales A=[esp+10h] flags, C=[esp+14h] marco original):
  frame = GetFrameFromId([this+18h], nodeId)  (0x4C53C0, cdecl)
  A = 0; RwFrameForAllObjects(frame, 0x6D33B0, &A) (0x7F1200); RwFrameForAllChildren(frame, 0x6D3420, &A) (0x7F0DC0)
  mi = ms_modelInfoPtrs[modelIndex] (0xA9B0C8); atomo = mi->vtable[+2Ch]() (CreateInstance); mi->AddRef (0x4C4BA0, thiscall); C = atomo[+4] (marco original)
  RpClumpAddAtomic([this+18h], atomo) (0x74A490); RpAtomicSetFrame(atomo, frame) (0x74BF20); RwFrameDestroy(C) (0x7F05A0)
  SetAtomicId(atomo, A & ~3) (0x732290); SetAtomicFlag(atomo, 1) (0x7322B0)
  byte alfa = 0; RpGeometryForAllMaterials([atomo+18h], 0x4C77C0, &alfa) (0x74C790); SetAtomicRenderCallback(atomo, alfa ? 0x733F80 : 0x733240) (0x7328A0);
  SetRenderPipelinesCB(atomo, 0) (0x4C88F0); byte [0xA9B0B0] = 0
  nodeId == 0x13: si [[this+384h]+0CCh] & 0x2000: r = CreateReplacementAtomic(this, mi, frame, A, 0, 1) (0x6D3700, thiscall ret 14h); marco de r = [r+4];
                  x = float[frame+40h] * float[0x858B18] (-2.0f); float[marco de r + 40h] = x; RwMatrixUpdate(marco de r + 10h) (0x7F18A0)
  nodeId == 0xC o 0xD: info = ms_modelInfoPtrs[(int16)[this+22h]]; SetupClumpAfterVehicleUpgrade([this+18h], [info+24h], byte[info+31h]) (0x6FDFE0)
  si mi->vtable[+8]() (AsDamageAtomicModelInfoPtr) != 0 (EAX completo): CreateReplacementAtomic(this, mi, frame, A, 1, 0); si frame: RwFrameForAllObjects(frame, 0x6D2690, 1),
                  RwFrameForAllChildren(frame, 0x6D26D0, 1)

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (eventos EN ORDEN, nevents, cambios de la arena respecto de su
imagen inicial, el byte final de ms_bCreateDamagedVersion); los punteros a la arena se codifican como 2^32 + desplazamiento. `stats` son banderas de cobertura.
`python -I fixture_oracle.py` ejecuta la autoprueba.
"""
import struct

NAME = 'AddReplacementUpgrade'
NAMES = [NAME]
FN_INDEX = {n: i for i, n in enumerate(NAMES)}
CASES = 16384
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
LAB = 1 << 32
MAX_EVENTS = 32
EV_ARGS = 8
FAMILIES = 16
# disposicion de la arena (debe coincidir con Fixtures.hpp: lo comprueba fixture_mirror.py)
O_G0, O_VEH, O_VMI, O_U0, O_HND, O_F0, O_F1, O_FA, O_AT0, O_AT1, O_AT2, O_CLUMP, O_G1 = 0x000, 0x040, 0x600, 0x680, 0x780, 0x860, 0x8C0, 0x920, 0x980, 0x9C0, 0xA00, 0xA40, 0xA60
ARENA_SIZE = 0xAA0
V_CLUMP, V_MODEL, V_HANDLING = 0x18, 0x22, 0x384
MI_PLATE_MAT, MI_PLATE_TYPE, HND_FLAGS = 0x24, 0x31, 0xCC
AT_PARENT, AT_GEOMETRY, FR_POSX, FR_MATRIX = 0x04, 0x18, 0x40, 0x10
VEH_MODEL_SLOT, FIRST_UPGRADE_SLOT = 15, 16
LAYOUT = {'veh': O_VEH, 'vmi': O_VMI, 'u0': O_U0, 'hnd': O_HND, 'f0': O_F0, 'f1': O_F1, 'fa': O_FA, 'at0': O_AT0, 'at1': O_AT1, 'at2': O_AT2, 'clump': O_CLUMP, 'g1': O_G1,
          'size': ARENA_SIZE, 'max_events': MAX_EVENTS, 'ev_args': EV_ARGS, 'v_words': 24, 'families': FAMILIES}
assert O_VEH == O_G0 + 0x40 and O_VMI == O_VEH + 0x5C0 and O_U0 == O_VMI + 0x80 and O_HND == O_U0 + 4 * 0x40 and O_F0 == O_HND + 0xE0 and O_F1 == O_F0 + 0x60 and O_FA == O_F1 + 0x60
assert O_AT0 == O_FA + 0x60 and O_AT1 == O_AT0 + 0x40 and O_AT2 == O_AT1 + 0x40 and O_CLUMP == O_AT2 + 0x40 and O_G1 == O_CLUMP + 0x20 and ARENA_SIZE == O_G1 + 0x40
# identificadores normalizados de callbacks
CB_REMOVE_OBJ, CB_REMOVE_FRAME, CB_VIS_OBJ, CB_VIS_FRAME, CB_HAS_ALPHA, CB_HIDETAIL, CB_HIDETAIL_ALPHA = 1, 2, 3, 4, 5, 6, 7
STACK_MARK = 0xDA7A   # `data` apunta a una local de la pila: se registra una marca


def lab(off):
    return LAB + off


def mix(x):
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_word(fn, n, i):
    return 0xC0000000 | (rand((n + fn * 0x20000 + 0x777) & M32, 1000 + i) >> 2)


class Arena:
    """Imagen de la arena en palabras; un puntero a la arena se guarda como LAB + desplazamiento."""

    def __init__(self, fn, n):
        self.w = [fill_word(fn, n, i) for i in range(ARENA_SIZE // 4)]
        self.init = list(self.w)

    def s32(self, off, v):
        assert off % 4 == 0
        self.w[off // 4] = v if v >= LAB else v & M32

    def g32(self, off):
        return self.w[off // 4]

    def s8(self, off, v):
        wi, sh = off // 4, (off % 4) * 8
        assert self.w[wi] < LAB
        self.w[wi] = (self.w[wi] & ~(0xFF << sh) & M32) | ((v & 0xFF) << sh)

    def s16(self, off, v):
        wi, sh = off // 4, (off % 4) * 8
        assert self.w[wi] < LAB and off % 4 in (0, 1, 2)
        self.w[wi] = (self.w[wi] & ~(0xFFFF << sh) & M32) | ((v & 0xFFFF) << sh)

    def g8(self, off):
        return (self.w[off // 4] >> ((off % 4) * 8)) & 0xFF

    def mem(self):
        return [[i, self.w[i]] for i in range(len(self.w)) if self.w[i] != self.init[i]]


def is_nan(b):
    return (b & 0x7FFFFFFF) > 0x7F800000


def f32_from_double(x):
    if x != x:
        return 0xFFC00000
    if x in (float('inf'), float('-inf')):
        return 0x7F800000 if x > 0 else 0xFF800000
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0x7F800000 if x > 0 else 0xFF800000


def f32_mul_m2(b):
    """b * -2.0f (bits): un NaN silencioso se propaga con su carga; el producto por -2 es exacto salvo desborde"""
    if is_nan(b):
        return b | 0x00400000
    f = struct.unpack('<f', struct.pack('<I', b))[0]
    return f32_from_double(f * -2.0)


def build(fn, n, v):
    a = Arena(fn, n)
    a.s8(O_CLUMP, 2)
    a.s16(O_VEH + V_MODEL, VEH_MODEL_SLOT)
    a.s32(O_VEH + V_HANDLING, lab(O_HND))
    a.s32(O_VEH + V_CLUMP, lab(O_CLUMP) if v[7] else 0)
    a.s32(O_VMI + MI_PLATE_MAT, v[4])
    a.s8(O_VMI + MI_PLATE_TYPE, v[5])
    a.s32(O_HND + HND_FLAGS, v[6])
    a.s32(O_AT0 + AT_PARENT, v[12])
    a.s32(O_AT0 + AT_GEOMETRY, v[13])
    a.s32(O_AT1 + AT_PARENT, lab(O_F1))
    a.s32(O_F0 + FR_POSX, v[16])
    a.init = list(a.w)
    return a


def expected(name, inp, case):
    assert name == NAME
    v = [x & M32 for x in inp['v']]
    a = build(0, case, v)
    this, clump = lab(O_VEH), a.g32(O_VEH + V_CLUMP)
    model, node = v[0], v[1]
    mi = lab(O_U0 + 0x40 * (model - FIRST_UPGRADE_SLOT))
    kind = v[2]
    frame = 0 if kind == 0 else (lab(O_F0) if kind == 1 else v[3])
    dmg = v[17] & 0xFF
    stats = {'frame_null': frame == 0, 'frame_arena': kind == 1, 'frame_other': kind == 2, 'obj_writes': bool(v[8]), 'ch_writes': bool(v[10]), 'flags_nonzero': False,
             'alpha': False, 'no_alpha': False, 'wheel': False, 'wheel_flag_off': False, 'plate': False, 'plate_c': node == 0xC, 'plate_d': node == 0xD, 'node_other': False,
             'damage': False, 'no_damage': False, 'damage_frame': False, 'damage_no_frame': False, 'clump_null': clump == 0, 'wheel_nan': False, 'wheel_inf': False, 'wheel_zero': False,
             'dmg_init_set': dmg != 0, 'dmg_bits_only': False}
    ev = []
    ev.append([1, clump, node])
    flags = 0
    ev.append([2, frame, CB_REMOVE_OBJ, STACK_MARK])
    if v[8]:
        flags = v[9]
    ev.append([3, frame, CB_REMOVE_FRAME, STACK_MARK])
    if v[10]:
        flags = v[11]
    stats['flags_nonzero'] = flags != 0
    ev.append([4, mi, dmg])
    ev.append([5, mi])
    atomic = lab(O_AT0)
    old_frame = a.g32(O_AT0 + AT_PARENT)
    ev.append([6, clump, atomic])
    ev.append([7, atomic, frame])
    a.s32(O_AT0 + AT_PARENT, frame)
    ev.append([8, old_frame])
    ev.append([9, atomic, flags & ~3 & M32])
    ev.append([10, atomic, 1])
    ev.append([11, a.g32(O_AT0 + AT_GEOMETRY), CB_HAS_ALPHA])
    alpha = (v[14] & 0xFF) != 0
    stats['alpha' if alpha else 'no_alpha'] = True
    ev.append([12, atomic, CB_HIDETAIL_ALPHA if alpha else CB_HIDETAIL])
    ev.append([13, atomic, 0])
    dmg = 0
    hnd = a.g32(O_HND + HND_FLAGS)
    if node == 0x13:
        if hnd & 0x2000:
            stats['wheel'] = True
            assert kind == 1
            ev.append([14, this, mi, frame, flags, dmg, 0, 1])
            x = f32_mul_m2(a.g32(O_F0 + FR_POSX))
            stats['wheel_nan'] = is_nan(x)
            stats['wheel_inf'] = (x & 0x7FFFFFFF) == 0x7F800000
            stats['wheel_zero'] = (x & 0x7FFFFFFF) == 0
            a.s32(O_F1 + FR_POSX, x)
            ev.append([15, lab(O_F1 + FR_MATRIX)])
        else:
            stats['wheel_flag_off'] = True
    elif node == 0xC or node == 0xD:
        stats['plate'] = True
        ev.append([16, clump, a.g32(O_VMI + MI_PLATE_MAT), a.g8(O_VMI + MI_PLATE_TYPE)])
    else:
        stats['node_other'] = True
    ev.append([17, mi])
    if v[15]:
        stats['damage'] = True
        stats['dmg_bits_only'] = (v[15] & 0xFF) == 0 and v[15] != 0
        ev.append([14, this, mi, frame, flags, dmg, 1, 0])
        if frame:
            stats['damage_frame'] = True
            ev.append([2, frame, CB_VIS_OBJ, 1])
            ev.append([3, frame, CB_VIS_FRAME, 1])
        else:
            stats['damage_no_frame'] = True
    else:
        stats['no_damage'] = True
    # nota: el falso de CreateReplacementAtomic devuelve AT1 y no escribe en la arena; los de RwFrame* no escriben salvo `data` (variable local)
    obs = {'ret': [], 'events': [list(e) + [0] * (1 + EV_ARGS - len(e)) for e in ev[:MAX_EVENTS]], 'nevents': len(ev), 'mem': a.mem(), 'dmg': dmg}
    return obs, stats


def selftest():
    z = [0] * 24
    v = list(z)
    v[0] = 16
    v[1] = 0x13
    v[2] = 1
    v[6] = 0x2000
    v[7] = 1
    v[16] = 0x3F800000   # 1.0
    v[15] = 0
    obs, st = expected(NAME, {'v': v}, 3)
    kinds = [e[0] for e in obs['events']]
    assert kinds == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17], kinds
    assert st['wheel'] and st['no_alpha'] and st['no_damage']
    assert [O_F1 // 4 + FR_POSX // 4, 0xC0000000] and any(m[0] == (O_F1 + FR_POSX) // 4 and m[1] == 0xC0000000 for m in obs['mem']), obs['mem']
    v[15] = 1
    v[1] = 0xC
    obs, st = expected(NAME, {'v': v}, 3)
    kinds = [e[0] for e in obs['events']]
    assert kinds == [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 14, 2, 3], kinds
    assert st['plate'] and st['damage_frame']
    assert f32_mul_m2(0x7F800000) == 0xFF800000 and f32_mul_m2(0x7FC12345) == 0x7FC12345 and f32_mul_m2(0x7F7FFFFF) == 0xFF800000
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
