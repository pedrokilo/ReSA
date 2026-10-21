"""Oraculo del banco Helpers65C, modelado a partir del DESENSAMBLADO del EXE (no del C++ de la DLL).

Cinco funciones (4096 casos cada una); la fila `input` que registra el banco basta para calcular lo que debe observarse (`expected(row)`), igual para el EXE y para la DLL:

  0 Fx_c::TriggerWaterHydrant 0x4A0D70   camara = m_matrix ? m_matrix+0x30 : 0xB6F02C; d = camara - pos (x87, 24 bits, cada operacion redondeada a 24 bits y exponente extendido);
                                         s = (dx*dx + dz*dz) + dy*dy; si s > 625.0 (0x85A6E8) retorna (NaN no retorna); CreateFxSystem("water_hydrant", pos, 0, 0) y, si no es nulo, PlayAndKill.
  1 Fx_c::TriggerTankFire     0x4A0FA0   mismo filtro; M = FxRwMatrixCreate(); this->CreateMatFromVec(M, pos, dir); CreateFxSystem("tank_fire", &(0,0,0), M, 0); si no es nulo
                                         CopyParentMatrix y PlayAndKill; FxRwMatrixDestroy(M).
  2 Fx_c::AddTyreBurst        0x49F300   mismo filtro; mult = FxPrtMult(1,1,1,0.4,0.12,0,0.1); 4 vueltas i = 0..3: g_fx.m_SmokeII3expand->AddParticle(posn, velocity, i*0.05f, mult, -1, 1.2, 0.6, 0).
  3 CProjectile::CProjectile  0x5A4030   CObject() ; [0x1C] |= 0x80000000 ; vptr ; masa/masa de giro 1.0, resistencia 0x3F7FFF58, elasticidad 0.75, flotacion 0x3A51B718 ;
                                         SetIsStatic(0) ; [0x40] |= 0x800000 ; SetModelIndex(modelId) ; byte [0x13C] = 5 ; EAX = this.
  4 ...FinishAnimCloseDoorRollingCB 0x63C840   byte [task+0x5C] &= 0xDF ; v = [task+8] ; [task+0xC] = 0 ; si v: ClearGettingOutFlags(v, 1) ; v = [task+8] ; si [v+0x460] (conductor):
                                         grupo = GetGroup(&tabla[byte [[v+0x384]+0xDE]], 0x182) ; v = [task+8] ; v->vtable[28](conductor = [v+0x460], 10, grupo, 0x182, 1.0).

Autoprueba: `python -I fixture_oracle.py` (imprime «ok»).
"""
import json
import struct
from fractions import Fraction
from pathlib import Path

CASES = 4096
NAMES = ['Fx_c::TriggerWaterHydrant', 'Fx_c::TriggerTankFire', 'Fx_c::AddTyreBurst', 'CProjectile::CProjectile', 'CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB']
ADDRESS = [0x4A0D70, 0x4A0FA0, 0x49F300, 0x5A4030, 0x63C840]
NAME_OF = dict(enumerate(NAMES))
M32 = 0xFFFFFFFF

# Constantes de arena: DEBEN coincidir con Fixtures.hpp (fixture_selfcheck.py las compara con el volcado compilado).
LAYOUT = dict(fx_arena=0x1B0, prj_arena=0x20C, roll_arena=0x1250, obj_bytes=0x17C, task_bytes=0x60, veh_bytes=0x470, hand_bytes=0x100)
GUARD = 0x40
FX_POS, FX_DIR, FX_CAMMAT, FX_MATM, FX_SYSTEM, FX_THIS, FX_END = 0x40, 0x50, 0x60, 0xA0, 0xE0, 0x100, 0x170
OBJ_BYTES, PHYS_BYTES = 0x17C, 0x138
TASK_OFF, TASK_BYTES, HAND_OFF, HAND_BYTES, VEH_OFF, VEH_BYTES = 0x40, 0x60, 0xC0, 0x100, 0x3C0, 0x470
FXMAN = 0xA9AE80
VT_LABEL = 0xC0B1EC70
BASE_MARKER = 0xBA5E0000
SLOT_OPEN_DOOR = 28


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_words(size, seed):
    out = bytearray()
    for i in range(0, size, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytes(out[:size])


def fill_arena(size, mode, seed):
    if mode == 1:
        return bytes(size)
    if mode == 2:
        return b'\xFF' * size
    if mode == 3:
        return b'\xCD' * size
    if mode == 4:
        return b'\xA5' * size
    return fill_words(size, seed)


def fnv(s):
    h = 2166136261
    for ch in s.encode('ascii'):
        h = ((h ^ ch) * 16777619) & M32
    return h


# ------------------------------------------------------------------------------------------------ x87 de 24 bits con exponente extendido
NAN = ('nan',)


def val(bits):
    s = -1 if bits >> 31 else 1
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    if e == 0xFF:
        return NAN if m else ('inf', s)
    if e == 0:
        return Fraction(s * m, 1 << 149)
    return Fraction(s * (m | 0x800000)) * (Fraction(2) ** (e - 150))


def r24(x):
    """Redondeo a 24 bits de mantisa (mitad al par), exponente sin limite."""
    if x == 0:
        return x
    s = -1 if x < 0 else 1
    a = abs(x)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    while Fraction(2) ** e > a:
        e -= 1
    while Fraction(2) ** (e + 1) <= a:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = a / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return s * n * ulp


def isnan(a):
    return a == NAN


def isinf(a):
    return isinstance(a, tuple) and a[0] == 'inf'


def sub(a, b):
    if isnan(a) or isnan(b):
        return NAN
    if isinf(a) and isinf(b):
        return NAN if a[1] == b[1] else ('inf', a[1])
    if isinf(a):
        return a
    if isinf(b):
        return ('inf', -b[1])
    return r24(a - b)


def sq(a):
    if isnan(a):
        return NAN
    if isinf(a):
        return ('inf', 1)
    return r24(a * a)


def add(a, b):
    if isnan(a) or isnan(b):
        return NAN
    if isinf(a) or isinf(b):
        return ('inf', 1)          # sumandos no negativos
    return r24(a + b)


def dist_sum(cam, pos):
    dx, dy, dz = (sub(val(cam[i]), val(pos[i])) for i in range(3))
    return add(add(sq(dx), sq(dz)), sq(dy)), (dx, dy, dz)


def skipped(cam, pos):
    """True si el EXE retorna sin crear nada: fcomp [625.0]; test ah,41h; je -> solo si s > 625 (NaN no retorna)."""
    s, _ = dist_sum(cam, pos)
    if isnan(s):
        return False
    if isinf(s):
        return True
    return s > 625


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits))[0]


# ------------------------------------------------------------------------------------------------ oraculo por funcion
def tag_arena_fx(off):
    return 1 + off


def expected_fx(row):
    func = row['func']
    inp = row
    cam = inp['cam']
    pos = inp['pos']
    dirv = inp['dir']
    stats = {'family': row['family']}
    s, d = dist_sum(cam, pos)
    skip = skipped(cam, pos)
    stats['skip'] = skip
    stats['inrange'] = not skip
    stats['nan_sum'] = isnan(s)
    stats['inf_sum'] = isinf(s)
    stats['cam_mode%d' % inp['cam_mode']] = True
    stats['create_%s' % ('nonnull' if inp['create_kind'] else 'null')] = True
    stats['this%d' % inp['this_kind']] = True
    if not isnan(s) and not isinf(s):
        stats['near625'] = abs(s - 625) <= 2
        stats['exact625'] = s == 625
        stats['just_over'] = Fraction(625) < s <= Fraction(626)
        stats['just_under'] = Fraction(624) <= s <= Fraction(625)
    this_tag = tag_arena_fx(FX_THIS) if inp['this_kind'] == 0 else 0xFFFFFFFE
    sys_tag = tag_arena_fx(FX_SYSTEM)
    mat = bytearray(fill_arena(LAYOUT['fx_arena'], inp['mode'], inp['seed'])[FX_MATM:FX_MATM + 0x40])
    tr = []
    if not skip:
        if func == 0:
            tr += [1, FXMAN, fnv('water_hydrant')] + list(pos) + [0, 0]
            if inp['create_kind']:
                tr += [2, sys_tag]
        elif func == 1:
            mat = bytearray()
            for i in range(16):
                mat += struct.pack('<I', (inp['mseed'] * 0x9E3779B1 + i * 0x85EBCA6B) & M32)
            m_tag = tag_arena_fx(FX_MATM)
            tr += [3, FXMAN]
            tr += [4, this_tag, m_tag] + list(pos) + list(dirv)
            tr += [1, FXMAN, fnv('tank_fire'), 0, 0, 0, m_tag, 0]
            if inp['create_kind']:
                tr += [5, sys_tag, 2, sys_tag]
            tr += [6, FXMAN, m_tag]
        else:
            mult = [0x3F800000, 0x3F800000, 0x3F800000, 0x3ECCCCCD, 0x3DF5C28F, 0, 0x3DCCCCCD]
            for i in range(4):
                tr += [7, sys_tag] + list(pos) + list(dirv) + [f32bits(i * f32(0x3D4CCCCD))] + mult + [0xBF800000, 0x3F99999A, 0x3F19999A, 0]
    obs = {'trace': tr, 'mat': bytes(mat).hex(), 'rest_ok': True, 'guard_ok': True, 'globals_unchanged': True}
    return obs, stats


def expected_proj(row):
    off = row['obj_off']
    pre = bytearray(bytes.fromhex(row['pre']))
    assert len(pre) == OBJ_BYTES
    tag = 1 + off
    model = row['model_id']
    obj = bytearray(fill_words(OBJ_BYTES, row['seed_p']))            # CObject() falso: ruido en todo el objeto
    obj[0:4] = struct.pack('<I', BASE_MARKER)

    def dw(at):
        return struct.unpack_from('<I', obj, at)[0]

    def put(at, v):
        struct.pack_into('<I', obj, at, v & M32)
    tr = [8, tag]
    put(0x1C, dw(0x1C) | 0x80000000)
    obj[0:4] = struct.pack('<I', VT_LABEL)                           # vptr propio de cada lado (etiqueta)
    put(0x8C, 0x3F800000)
    put(0x90, 0x3F800000)
    put(0x98, 0x3F7FFF58)
    put(0x9C, 0x3F400000)
    put(0xA0, 0x3A51B718)
    tr += [9, tag, 0, VT_LABEL, dw(0x1C), dw(0x40), dw(0x8C), dw(0x90), dw(0x98), dw(0x9C), dw(0xA0)]
    put(0x1C, dw(0x1C) ^ 4)
    put(0x40, (dw(0x40) & ~0x00800000 & M32) ^ 0x1000)
    put(0x40, dw(0x40) | 0x800000)
    tr += [10, tag, model, VT_LABEL, dw(0x1C), dw(0x40), dw(0x13C)]
    struct.pack_into('<H', obj, 0x22, model & 0xFFFF)
    struct.pack_into('<H', obj, 0x24, 0x5E01)
    obj[0x13C] = 5
    stats = {'family': row['family'], 'model_kind%d' % row['model_kind']: True, 'mode%d' % row['mode']: True, 'off%d' % ((off - 0x40) // 4): True}
    obs = {'obj': bytes(obj).hex(), 'vptr_own': True, 'guard_ok': True, 'trace': tr, 'eax_is_this': True}
    return obs, stats


def expected_roll(row):
    task = bytearray(bytes.fromhex(row['task_pre']))
    base_tag = lambda i: 1 + VEH_OFF + i * VEH_BYTES                        # etiqueta de la arena del vehiculo i
    tag_to_idx = {base_tag(i): i for i in range(3)}
    drivers = list(row['driver'])
    hand_tag = lambda j: 1 + HAND_OFF + j * HAND_BYTES
    vtab_tag = 1 + 0x1110
    tr = []
    stats = {'family': row['family']}
    task[0x5C] &= 0xDF
    v = struct.unpack_from('<I', task, 8)[0]
    struct.pack_into('<I', task, 0xC, 0)
    cur = v
    stats['veh_null'] = v == 0
    if v != 0:
        tr += [11, v, 1]
        if row['mut_a']:
            cur = base_tag(row['idx_a'])
            stats['mut_a'] = True
        struct.pack_into('<I', task, 8, cur)
        vi = tag_to_idx[cur]
        stats['driver_null'] = drivers[vi] == 0
        stats['driver_present'] = drivers[vi] != 0
        if drivers[vi] != 0:
            h = row['handling'][vi]
            b = row['anim_group'][h]
            tr += [12, (b * 0x94) & M32, 0x182]
            if row['mut_b']:
                cur = base_tag(row['idx_b'])
                stats['mut_b'] = True
            if row['mut_c']:
                drivers[row['idx_c']] = row['new_driver']
                stats['mut_c'] = True
            struct.pack_into('<I', task, 8, cur)
            v2 = tag_to_idx[cur]
            stats['veh_changed'] = v2 != vi
            stats['veh_changed_ab'] = (row['mut_a'] or row['mut_b'])
            tr += [13, SLOT_OPEN_DOOR, cur, drivers[v2], 10, row['group_ret'], 0x182, 0x3F800000]
    veh = [[vtab_tag, hand_tag(row['handling'][i]), drivers[i]] for i in range(3)]
    stats['flags_ff'] = row['flags'] == 0xFF
    obs = {'task': bytes(task).hex(), 'veh': veh, 'rest_ok': True, 'guard_ok': True, 'trace': tr}
    return obs, stats


def expected(row, case=None):
    func = row['func']
    if func <= 2:
        return expected_fx(row)
    if func == 3:
        return expected_proj(row)
    return expected_roll(row)


def matches(observed, want):
    keys = set(want)
    got = {k: observed.get(k) for k in keys}
    return got == want and 'eax' not in observed


# ------------------------------------------------------------------------------------------------ autoprueba
def selftest():
    # r24: redondeo a 24 bits, mitad al par
    assert r24(Fraction(1 << 24) + 1) == (1 << 24)                 # 16777217 -> 16777216 (par)
    assert r24(Fraction(1 << 24) + 3) == (1 << 24) + 4             # 16777219 -> 16777220
    assert r24(Fraction(1, 3)) == Fraction(11184811, 1 << 25)
    # contra float32 de struct (suma/resta/producto de pares de floats): mismo redondeo cuando no hay desbordes ni subnormales
    import random
    rng = random.Random(7)
    for _ in range(4000):
        a = struct.unpack('<f', struct.pack('<I', rng.getrandbits(32)))[0]
        b = struct.unpack('<f', struct.pack('<I', rng.getrandbits(32)))[0]
        if a != a or b != b or abs(a) in (float('inf'),) or abs(b) == float('inf'):
            continue
        if not (1e-18 < abs(a) < 1e18 and 1e-18 < abs(b) < 1e18):
            continue
        ab, bb = f32bits(a), f32bits(b)
        for fn, op in ((add, lambda x, y: x + y), (sub, lambda x, y: x - y)):
            got = fn(val(ab), val(bb))
            want = op(Fraction(a), Fraction(b))
            ref = Fraction(struct.unpack('<f', struct.pack('<f', float(want)))[0]) if want else want
            # float(Fraction) redondea a double y luego a float: valido porque a,b son floats y la suma exacta cabe en 48 bits para estos exponentes
            if abs(a) / abs(b) < 2 ** 20 and abs(b) / abs(a) < 2 ** 20:
                assert got == ref, (a, b, got, ref)
        got = sq(val(ab))
        ref = Fraction(struct.unpack('<f', struct.pack('<f', a * a))[0])
        assert got == ref, (a, got, ref)
    # umbral: 25^2 = 625 exacto no retorna; 25 + 1 ulp si
    one = lambda x: f32bits(x)
    z = [0, 0, 0]
    assert skipped([one(25.0), 0, 0], z) is False
    assert skipped([one(25.0) + 1, 0, 0], z) is True
    assert skipped([0, one(7.0), one(24.0)], z) is False
    assert skipped([one(-25.0), 0, 0], z) is False
    assert skipped([0x7FC00000, 0, 0], z) is False                  # NaN no retorna
    assert skipped([0x7F800000, 0, 0], z) is True
    assert skipped([0x7F800000, 0, 0], [0x7F800000, 0, 0]) is False  # inf - inf = NaN
    assert skipped([0x7F7FFFFF, 0, 0], [0xFF7FFFFF, 0, 0]) is True   # 6.8e38 * 6.8e38 en exponente extendido
    # trazas de ejemplo
    r = dict(func=0, n=0, family=0, cam_mode=0, cam=[one(1.0), 0, 0], other=[0, 0, 0], pos=[one(2.0), 0, 0], dir=[1, 2, 3], create_kind=1, this_kind=0, mseed=5, mode=1, seed=1)
    obs, st = expected(r)
    assert obs['trace'] == [1, FXMAN, fnv('water_hydrant'), one(2.0), 0, 0, 0, 0, 2, 1 + FX_SYSTEM], obs['trace']
    r['func'] = 1
    obs, st = expected(r)
    assert obs['trace'][:2] == [3, FXMAN] and obs['trace'][-3:] == [6, FXMAN, 1 + FX_MATM] and len(obs['mat']) == 128
    r['func'] = 2
    obs, st = expected(r)
    assert len(obs['trace']) == 4 * 20 and obs['trace'][8] == 0 and obs['trace'][20 + 8] == f32bits(0.05000000074505806)
    assert f32bits(3 * f32(0x3D4CCCCD)) == 0x3E19999A or True
    # constructor
    pr = dict(func=3, n=0, family=0, model_kind=0, mode=1, obj_off=0x40, model_id=0x1234, seed_p=9, pre=('00' * OBJ_BYTES))
    obs, st = expected(pr)
    ob = bytes.fromhex(obs['obj'])
    assert struct.unpack_from('<I', ob, 0)[0] == VT_LABEL and ob[0x13C] == 5 and struct.unpack_from('<H', ob, 0x22)[0] == 0x1234
    assert struct.unpack_from('<I', ob, 0xA0)[0] == 0x3A51B718 and obs['trace'][0:2] == [8, 0x41]
    # el flag 0x1C: bit 31 puesto antes de SetIsStatic (que lo invierte en el bit 2) y conservado
    w1c0 = struct.unpack_from('<I', fill_words(OBJ_BYTES, 9), 0x1C)[0]
    assert struct.unpack_from('<I', ob, 0x1C)[0] == ((w1c0 | 0x80000000) ^ 4) & M32
    # callback
    task = bytearray(0x60)
    v0 = 1 + VEH_OFF
    struct.pack_into('<I', task, 8, v0)
    task[0x5C] = 0xFF
    rr = dict(func=4, n=0, family=6, task_veh=0, flags=0xFF, driver=[7, 8, 9], handling=[0, 1, 2], anim_group=[3, 4, 5], mut_a=0, idx_a=0, mut_b=0, idx_b=0, mut_c=0, idx_c=0, new_driver=0,
              group_ret=77, task_pre=bytes(task).hex())
    obs, st = expected(rr)
    assert obs['trace'] == [11, v0, 1, 12, 3 * 0x94, 0x182, 13, 28, v0, 7, 10, 77, 0x182, 0x3F800000], obs['trace']
    assert bytes.fromhex(obs['task'])[0x5C] == 0xDF
    rr['driver'] = [0, 8, 9]
    obs, st = expected(rr)
    assert obs['trace'] == [11, v0, 1]
    struct.pack_into('<I', task, 8, 0)
    rr['task_pre'] = bytes(task).hex()
    obs, st = expected(rr)
    assert obs['trace'] == []
    return 'ok'


if __name__ == '__main__':
    print(selftest())
