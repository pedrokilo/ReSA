"""Oraculo independiente del banco Vehicle66 (lote 66): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20),
AddVehicleUpgrade (0x6E3290), PossiblyDropFreeFallBombForPlayer (0x6E07E0) y ProcessSirenAndHorn (0x6E0950) para una fila `input` (la especificacion semantica del caso que registra Core.hpp).

Desensamblado (resumen; ver README.md):
  0x6DF930  ebp=idx; info=ms_modelInfoPtrs[idx]; carMod=(word[info+12h]>>10)&1F; ClearVehicleUpgradeFlags(this,idx,carMod) (0x6D3210): si AL!=0 sale; FindOtherUpgrade(&ms_linkedUpgrades,idx) (0x4C74D0)
            -> AX con signo; bit 8 de la palabra: RemoveReplacementUpgrade(this,carMod) (0x6D39E0) y, si carMod==2, m_fWheelScale=1.0 (+0x458) y componentes 5,4,7; si no, RpClumpForAllAtomics
            (clump=[this+18h], RemoveUpgradeCB 0x6D3300, data=carMod). Con enlazado != -1 lo mismo para su CarMod (sin la rueda). Al final las 15 ranuras int16 de +0x43A que valgan idx pasan a -1.
  0x6DFA20  vehInfo=info[(int16)[this+22h]]; desc=[vehInfo+5Ch]+0xB4+idx*0x20; GetFrameFromId(clump,[desc+1C]) (0x4C53C0); RpClumpForAllAtomics(clump,RemoveUpgradeCB,idx);
            CreateUpgradeAtomic(this,upgradeInfo,desc,frame,0) (0x6D3510); si [desc+1C]==1 sale; para idx 0,1,2,6,12 usa el descriptor idx+3,+3,+3,+1,+1 si su padre != -1;
            CreateUpgradeAtomic(...,frame,1); si frame: RwFrameForAllObjects(frame,0x6D2690,1) (0x7F1200), RwFrameForAllChildren(frame,0x6D26D0,1) (0x7F0DC0);
            SetupClumpAfterVehicleUpgrade(clump,[vehInfo+24h],byte[vehInfo+31h]) (0x6FDFE0)
  0x6E3290  replaced=-1; SetVehicleUpgradeFlags(this,model,carMod,&replaced) (0x6D30E0): AL!=0 salta al final; FindOtherUpgrade; bit 8: replaced=GetReplacementUpgrade(this,carMod) (0x6D3A50),
            AddReplacementUpgrade(this,model,carMod) (0x6D3830), carMod==2: m_fWheelScale=[info[(int16)[this+22h]]+40h], componentes 5,4,7 y, si [this+594h]==0: model==word[0x8CD758] ->
            [this+38Ch]|=0x100000; si no, y [[this+384h]+0xD0]&0x100000==0 -> [this+38Ch]&=~0x100000; sin bit 8: replaced=GetUpgrade(this,carMod) (0x6D3650), AddUpgrade(this,model,carMod) (0x6DFA20);
            enlazado != -1: AddReplacementUpgrade(this,linked,carMod2) o AddUpgrade(this,linked,carMod2). Bucle de 15 ranuras (+0x43A): si == replaced -> -1 y se guarda; o 0xFFFF -> se guarda;
            guardar = ranura <- low16(nuevo), nuevo <- -1. Devuelve EAX = replaced.
  0x6E07E0  type=ebx; last=[this+4FCh] si type==1, si no [this+500h]; si arg1(AL): GetPlaneOrdnanceRateOfFire(this,type) (0x6D4590) + last (uint32): sale si CTimer(0xB7CB84) <= suma;
            pos=matriz?[matriz+30h]:[this+4]; GetPlaneOrdnancePosition(this,&out,type) (0x6D46E0); lado=((byte[this+4A9h]>>2)&3)!=0; x=pos.x+(lado?-out.x:out.x) (fadd), y=out.y+pos.y, z=out.z+pos.z (floats);
            byte[this+4A9h] bits 2-3 = (lado==0); AddProjectile(this,21,x,y,z,0.0f,matriz+10h,0) (0x737C80); si [this+460h] y IsPlayer ([ped+598h]==0 o 1) -> CPad::GetPad(tipo)->StartShake(240,160,0) (0x53F920);
            type==1 -> [this+4FCh]=CTimer, si no [this+500h]=CTimer
  0x6E0950  UsesSiren(this) (0x6D8470): si AL!=0: i=byte[0xB7356E]; h(k)=byte[0xB73569+k] (k hasta 255); h(i)==0: h((i+4)%5)==0 o h((i+1)%5)!=0 -> [this+514h]=0; si no [this+514h]=0 y
            byte[this+42Dh]^=0x80; h(i)!=0: h((i+4)%5)==0 o h((i+3)%5)==0 -> 0; si no 1. Sin sirena: si byte arg0==0 sale; alarma=word[this+45Ch]: si !=0 y !=0xFFFF y (byte[this+36h]&F8)!=28 sale;
            GetHorn(&Pads[0]) (0x53FEE0): [this+514h]=(AL!=0)

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (ret, eventos EN ORDEN, nevents, cambios de la arena respecto de su imagen inicial);
los punteros a la arena se codifican como 2^32 + desplazamiento. `stats` son banderas de cobertura. `python -I fixture_oracle.py` ejecuta la autoprueba.
"""
import struct

NAME_REMOVE = 'RemoveVehicleUpgrade'
NAME_ADDUP = 'AddUpgrade'
NAME_ADDVEH = 'AddVehicleUpgrade'
NAME_DROP = 'PossiblyDropFreeFallBombForPlayer'
NAME_SIREN = 'ProcessSirenAndHorn'
NAMES = [NAME_REMOVE, NAME_ADDUP, NAME_ADDVEH, NAME_DROP, NAME_SIREN]
FN_INDEX = {n: i for i, n in enumerate(NAMES)}
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
LAB = 1 << 32
MAX_EVENTS = 24
EV_ARGS = 8
FAMILIES = 8
# disposicion de la arena (debe coincidir con Fixtures.hpp: lo comprueba fixture_mirror.py)
O_G0, O_VEH, O_VMI, O_VST, O_U0, O_HND, O_PED, O_MAT, O_CLUMP, O_G1 = 0x000, 0x040, 0x600, 0x680, 0x940, 0xA40, 0xB20, 0x10C0, 0x1120, 0x1140
ARENA_SIZE = 0x1180
V_POS, V_MATRIX, V_CLUMP, V_MODEL, V_STATUS, V_HANDLING, V_FLAGS38C, V_FLAGS42D, V_UPGRADES = 0x04, 0x14, 0x18, 0x22, 0x36, 0x384, 0x38C, 0x42D, 0x43A
V_WHEEL_SCALE, V_ALARM, V_DRIVER, V_ORDN, V_TIME1, V_TIME2, V_HORN, V_SUBTYPE = 0x458, 0x45C, 0x460, 0x4A9, 0x4FC, 0x500, 0x514, 0x594
MI_PLATE_MAT, MI_PLATE_TYPE, MI_WHEEL_FRONT, MI_STRUCT, MI_FLAGS = 0x24, 0x31, 0x40, 0x5C, 0x12
ST_UPGRADES, DESC_PARENT, HND_FLAGS, PED_TYPE, MAT_POS = 0xB4, 0x1C, 0xD0, 0x598, 0x30
VEH_MODEL_SLOT, FIRST_UPGRADE_SLOT = 15, 16
PADS0 = 0xB73458
LINKED_LIST = 0xB4E6D8
LAYOUT = {'veh': O_VEH, 'vmi': O_VMI, 'vst': O_VST, 'u0': O_U0, 'hnd': O_HND, 'ped': O_PED, 'mat': O_MAT, 'clump': O_CLUMP, 'g1': O_G1, 'size': ARENA_SIZE, 'max_events': MAX_EVENTS, 'ev_args': EV_ARGS,
          'v_words': 40, 'families': FAMILIES}
assert O_VEH == O_G0 + 0x40 and O_VMI == O_VEH + 0x5C0 and O_VST == O_VMI + 0x80 and O_U0 == O_VST + 0x2C0 and O_HND == O_U0 + 4 * 0x40 and O_PED == O_HND + 0xE0
assert O_MAT == O_PED + 0x5A0 and O_CLUMP == O_MAT + 0x60 and O_G1 == O_CLUMP + 0x20 and ARENA_SIZE == O_G1 + 0x40


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


def horn_fill(n, i):
    r = rand((n + 0x44000) & M32, 500 + i)
    return 0 if (r & 1) else 1 + ((r >> 8) % 255)


def sext16(x):
    x &= 0xFFFF
    return x - 0x10000 if x & 0x8000 else x


class Arena:
    """Imagen de la arena en palabras; un puntero a la arena se guarda como LAB + desplazamiento (las escrituras parciales nunca caen en uno)."""

    def __init__(self, fn, n):
        self.w = [fill_word(fn, n, i) for i in range(ARENA_SIZE // 4)]
        self.init = list(self.w)

    def s32(self, off, v):
        assert off % 4 == 0
        self.w[off // 4] = v if v >= LAB else v & M32

    def g32(self, off):
        return self.w[off // 4]

    def _patch(self, off, size, v):
        wi, sh = off // 4, (off % 4) * 8
        assert self.w[wi] < LAB, 'escritura parcial sobre un puntero'
        mask = ((1 << (8 * size)) - 1) << sh
        self.w[wi] = (self.w[wi] & ~mask & M32) | ((v << sh) & mask)

    def s8(self, off, v):
        self._patch(off, 1, v & 0xFF)

    def s16(self, off, v):
        assert off % 4 in (0, 1, 2)
        self._patch(off, 2, v & 0xFFFF)

    def g8(self, off):
        return (self.w[off // 4] >> ((off % 4) * 8)) & 0xFF

    def g16(self, off):
        assert off % 4 in (0, 1, 2)
        return (self.w[off // 4] >> ((off % 4) * 8)) & 0xFFFF

    def mem(self):
        return [[i, self.w[i]] for i in range(len(self.w)) if self.w[i] != self.init[i]]


def build(fn, n, v):
    a = Arena(fn, n)
    a.s8(O_CLUMP, 2)
    a.s16(O_VEH + V_MODEL, VEH_MODEL_SLOT)
    a.s32(O_VEH + V_HANDLING, lab(O_HND))
    a.s32(O_VMI + MI_STRUCT, lab(O_VST))
    if fn == 0:
        for i in range(4):
            a.s16(O_U0 + 0x40 * i + MI_FLAGS, v[1 + i])
        for i in range(15):
            a.s16(O_VEH + V_UPGRADES + 2 * i, v[8 + i])
        a.s32(O_VEH + V_WHEEL_SCALE, v[7])
        a.s32(O_VEH + V_CLUMP, lab(O_CLUMP) if v[23] else 0)
    elif fn == 1:
        a.s32(O_VMI + MI_PLATE_MAT, v[2])
        a.s8(O_VMI + MI_PLATE_TYPE, v[3])
        for i in range(15):
            a.s32(O_VST + ST_UPGRADES + 0x20 * i + DESC_PARENT, v[4 + i])
        a.s32(O_VEH + V_CLUMP, lab(O_CLUMP) if v[19] else 0)
    elif fn == 2:
        for i in range(4):
            a.s16(O_U0 + 0x40 * i + MI_FLAGS, v[1 + i])
        a.s32(O_VMI + MI_WHEEL_FRONT, v[11])
        a.s32(O_VEH + V_SUBTYPE, v[12])
        a.s32(O_HND + HND_FLAGS, v[14])
        a.s32(O_VEH + V_FLAGS38C, v[15])
        a.s32(O_VEH + V_WHEEL_SCALE, v[16])
        for i in range(15):
            a.s16(O_VEH + V_UPGRADES + 2 * i, v[17 + i])
    elif fn == 3:
        a.s32(O_VEH + V_MATRIX, lab(O_MAT) if v[9] else 0)
        for i in range(3):
            a.s32(O_MAT + MAT_POS + 4 * i, v[10 + i])
            a.s32(O_VEH + V_POS + 4 * i, v[13 + i])
        a.s32(O_VEH + V_TIME1, v[3])
        a.s32(O_VEH + V_TIME2, v[4])
        a.s8(O_VEH + V_ORDN, v[16])
        a.s32(O_VEH + V_DRIVER, lab(O_PED) if v[17] else 0)
        a.s32(O_PED + PED_TYPE, v[18])
    else:
        a.s16(O_VEH + V_ALARM, v[8])
        a.s8(O_VEH + V_STATUS, v[9])
        a.s8(O_VEH + V_FLAGS42D, v[11])
        a.s32(O_VEH + V_HORN, v[12])
    a.init = list(a.w)
    return a


def f32_neg(b):
    return b ^ 0x80000000


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


def f32_add(a, b):
    """suma de floats (bits) como x87 con precision de 24 bits / SSE: un NaN se propaga silenciado; inf-inf da el NaN indefinido 0xFFC00000. Nunca se generan dos NaN."""
    na, nb = is_nan(a), is_nan(b)
    assert not (na and nb)
    if na:
        return a | 0x00400000
    if nb:
        return b | 0x00400000
    fa = struct.unpack('<f', struct.pack('<I', a))[0]
    fb = struct.unpack('<f', struct.pack('<I', b))[0]
    return f32_from_double(fa + fb)


def observed(ret, events, mem):
    return {'ret': ret, 'events': [list(e) + [0] * (1 + EV_ARGS - len(e)) for e in events[:MAX_EVENTS]], 'nevents': len(events), 'mem': mem}


def info_of(a, u):
    flags = a.g16(O_U0 + 0x40 * u + MI_FLAGS)
    return (flags >> 10) & 0x1F, (flags >> 8) & 1


def remove(inp, n):
    v = [x & M32 for x in inp['v']]
    a = build(0, n, v)
    this = lab(O_VEH)
    arg = v[0]
    u = arg - FIRST_UPGRADE_SLOT
    car, veh = info_of(a, u)
    clump = a.g32(O_VEH + V_CLUMP)
    ev = [[1, this, arg, car]]
    stats = {'cleared': False, 'main_vehdummy': False, 'main_atomic': False, 'carmod2': False, 'linked_none': False, 'linked_vehdummy': False, 'linked_atomic': False,
             'slot_none': False, 'slot_one': False, 'slot_multi': False}
    if v[5] & 0xFF:
        stats['cleared'] = True
        return observed([], ev, a.mem()), stats
    linked = sext16(v[6] & 0xFFFF)
    ev.append([2, LINKED_LIST, arg])
    if veh:
        stats['main_vehdummy'] = True
        ev.append([3, this, car])
        if car == 2:
            stats['carmod2'] = True
            a.s32(O_VEH + V_WHEEL_SCALE, 0x3F800000)
            for node in (5, 4, 7):
                ev.append([3, this, node])
    else:
        stats['main_atomic'] = True
        ev.append([4, clump, 1, car])
    if linked != -1:
        lcar, lveh = info_of(a, linked - FIRST_UPGRADE_SLOT)
        if lveh:
            stats['linked_vehdummy'] = True
            ev.append([3, this, lcar])
        else:
            stats['linked_atomic'] = True
            ev.append([4, clump, 1, lcar])
    else:
        stats['linked_none'] = True
    hits = 0
    for i in range(15):
        if sext16(a.g16(O_VEH + V_UPGRADES + 2 * i)) == arg:
            a.s16(O_VEH + V_UPGRADES + 2 * i, 0xFFFF)
            hits += 1
    stats['slot_none'] = hits == 0
    stats['slot_one'] = hits == 1
    stats['slot_multi'] = hits >= 2
    return observed([], ev, a.mem()), stats


ALT = {0: 3, 1: 4, 2: 5, 6: 7, 12: 13}


def add_upgrade(inp, n):
    v = [x & M32 for x in inp['v']]
    a = build(1, n, v)
    this = lab(O_VEH)
    model, idx = v[0], v[1]
    info = lab(O_U0 + 0x40 * (model - FIRST_UPGRADE_SLOT))
    desc_off = O_VST + ST_UPGRADES + 0x20 * idx
    parent = a.g32(desc_off + DESC_PARENT)
    clump = a.g32(O_VEH + V_CLUMP)
    frame = v[20]
    ev = [[5, clump, parent], [4, clump, 1, idx], [6, this, info, lab(desc_off), frame, 0]]
    stats = {'parent1': False, 'alt_used': False, 'alt_kept': False, 'plain': False, 'frame_null': frame == 0, 'frame_set': frame != 0, 'idx_alt_capable': idx in ALT}
    if parent == 1:
        stats['parent1'] = True
        return observed([], ev, a.mem()), stats
    desc2 = desc_off
    if idx in ALT:
        alt_off = O_VST + ST_UPGRADES + 0x20 * ALT[idx]
        if a.g32(alt_off + DESC_PARENT) != M32:
            desc2 = alt_off
            stats['alt_used'] = True
        else:
            stats['alt_kept'] = True
    else:
        stats['plain'] = True
    ev.append([6, this, info, lab(desc2), frame, 1])
    if frame:
        ev.append([7, frame, 2, 1])
        ev.append([8, frame, 3, 1])
    ev.append([9, clump, a.g32(O_VMI + MI_PLATE_MAT), a.g8(O_VMI + MI_PLATE_TYPE)])
    return observed([], ev, a.mem()), stats


def add_vehicle(inp, n):
    v = [x & M32 for x in inp['v']]
    a = build(2, n, v)
    this = lab(O_VEH)
    model = v[0]
    car, veh = info_of(a, model - FIRST_UPGRADE_SLOT)
    replaced = M32
    ev = [[10, this, model, car, replaced]]
    stats = {'flags_done': False, 'vehdummy': False, 'vehdummy_c2': False, 'c2_offroad': False, 'c2_auto_off': False, 'c2_keep': False, 'atomic': False, 'linked_vd': False,
             'linked_atomic': False, 'linked_none': False, 'set_writes': False, 'repl_in_slots': False, 'stored_free': False, 'no_store': False}
    if v[6]:
        replaced = v[7]
        stats['set_writes'] = True
    if v[5] & 0xFF:
        stats['flags_done'] = True
    else:
        linked = sext16(v[8] & 0xFFFF)
        ev.append([2, LINKED_LIST, model])
        if veh:
            stats['vehdummy'] = True
            replaced = v[9]
            ev.append([11, this, car])
            ev.append([12, this, model, car])
            if car == 2:
                stats['vehdummy_c2'] = True
                a.s32(O_VEH + V_WHEEL_SCALE, v[11])
                for node in (5, 4, 7):
                    ev.append([12, this, model, node])
                if a.g32(O_VEH + V_SUBTYPE) == 0:
                    flags = a.g32(O_VEH + V_FLAGS38C)
                    if model == (v[13] & 0xFFFF):
                        a.s32(O_VEH + V_FLAGS38C, flags | 0x100000)
                        stats['c2_offroad'] = True
                    elif not (a.g32(O_HND + HND_FLAGS) & 0x100000):
                        a.s32(O_VEH + V_FLAGS38C, flags & 0xFFEFFFFF)
                        stats['c2_auto_off'] = True
                    else:
                        stats['c2_keep'] = True
        else:
            stats['atomic'] = True
            replaced = v[10]
            ev.append([13, this, car])
            ev.append([14, this, model, car])
        if linked != -1:
            lcar, lveh = info_of(a, linked - FIRST_UPGRADE_SLOT)
            if lveh:
                stats['linked_vd'] = True
                ev.append([12, this, linked & M32, lcar])
            else:
                stats['linked_atomic'] = True
                ev.append([14, this, linked & M32, lcar])
        else:
            stats['linked_none'] = True
    new = model
    stored = False
    for i in range(15):
        off = O_VEH + V_UPGRADES + 2 * i
        cx = a.g16(off)
        if sext16(cx) == sext32(replaced):
            a.s16(off, 0xFFFF)
            stats['repl_in_slots'] = True
            store = True
        else:
            store = cx == 0xFFFF
            if store:
                stats['stored_free'] = True
        if store:
            a.s16(off, new & 0xFFFF)
            new = M32
            stored = True
    stats['no_store'] = not stored
    return observed([replaced & M32], ev, a.mem()), stats


def sext32(x):
    x &= M32
    return x - (1 << 32) if x & 0x80000000 else x


def drop(inp, n):
    v = [x & M32 for x in inp['v']]
    a = build(3, n, v)
    this = lab(O_VEH)
    typ, arg1, time = v[0], v[1], v[2]
    last = v[3] if typ == 1 else v[4]
    ev = []
    stats = {'type1': typ == 1, 'type_other': typ != 1, 'checked': bool(arg1 & 0xFF), 'blocked': False, 'fired': False, 'side0': False, 'side1': False, 'null_matrix': v[9] == 0,
             'driver_none': False, 'driver_other': False, 'shake0': False, 'shake1': False, 'wrap': False, 'nan_out': False}
    if arg1 & 0xFF:
        ev.append([15, this, typ])
        total = (v[5] + last) & M32
        stats['wrap'] = v[5] + last > M32
        if time <= total:
            stats['blocked'] = True
            return observed([], ev, a.mem()), stats
    stats['fired'] = True
    matrix = a.g32(O_VEH + V_MATRIX)
    pos = [a.g32((O_MAT + MAT_POS if matrix else O_VEH + V_POS) + 4 * i) for i in range(3)]
    ord_ = v[6:9]
    ev.append([16, this, typ])
    byte = a.g8(O_VEH + V_ORDN)
    side = ((byte >> 2) & 3) != 0
    stats['side1' if side else 'side0'] = True
    ox = f32_neg(ord_[0]) if side else ord_[0]
    sx = f32_add(pos[0], ox)
    sy = f32_add(ord_[1], pos[1])
    sz = f32_add(ord_[2], pos[2])
    stats['nan_out'] = any(is_nan(x) for x in (sx, sy, sz))
    a.s8(O_VEH + V_ORDN, (byte & ~0x0C) | ((1 if not side else 0) << 2))
    direction = lab(O_MAT + 0x10) if matrix else 0x10
    ev.append([17, this, 21, sx, sy, sz, 0, direction, 0])
    driver = a.g32(O_VEH + V_DRIVER)
    if driver:
        ptype = a.g32(O_PED + PED_TYPE)
        if ptype in (0, 1):
            ev.append([18, ptype * 0x134, 240, 160, 0])
            stats['shake0' if ptype == 0 else 'shake1'] = True
        else:
            stats['driver_other'] = True
    else:
        stats['driver_none'] = True
    a.s32(O_VEH + (V_TIME1 if typ == 1 else V_TIME2), time)
    return observed([], ev, a.mem()), stats


def siren(inp, n):
    v = [x & M32 for x in inp['v']]
    a = build(4, n, v)
    this = lab(O_VEH)
    index = v[2] & 0xFF

    def hist(k):
        return v[3 + k] & 0xFF if k < 5 else (index if k == 5 else horn_fill(n, k))
    ev = [[19, this]]
    stats = {'siren': False, 'nosiren': False, 'cur_on': False, 'cur_off': False, 'counter1': False, 'toggle': False, 'zero': False, 'idx_over4': index >= 5, 'arg0_false': False,
             'blocked': False, 'horn_on': False, 'horn_off': False}
    if v[1] & 0xFF:
        stats['siren'] = True
        if hist(index) == 0:
            stats['cur_off'] = True
            if hist((index + 4) % 5) == 0 or hist((index + 1) % 5) != 0:
                a.s32(O_VEH + V_HORN, 0)
                stats['zero'] = True
            else:
                a.s32(O_VEH + V_HORN, 0)
                a.s8(O_VEH + V_FLAGS42D, a.g8(O_VEH + V_FLAGS42D) ^ 0x80)
                stats['toggle'] = True
        else:
            stats['cur_on'] = True
            if hist((index + 4) % 5) == 0 or hist((index + 3) % 5) == 0:
                a.s32(O_VEH + V_HORN, 0)
                stats['zero'] = True
            else:
                a.s32(O_VEH + V_HORN, 1)
                stats['counter1'] = True
        return observed([], ev, a.mem()), stats
    stats['nosiren'] = True
    if (v[0] & 0xFF) == 0:
        stats['arg0_false'] = True
        return observed([], ev, a.mem()), stats
    alarm = a.g16(O_VEH + V_ALARM)
    if alarm != 0 and alarm != 0xFFFF and (a.g8(O_VEH + V_STATUS) & 0xF8) != 0x28:
        stats['blocked'] = True
        return observed([], ev, a.mem()), stats
    ev.append([20, 0])
    on = (v[10] & 0xFF) != 0
    stats['horn_on' if on else 'horn_off'] = True
    a.s32(O_VEH + V_HORN, 1 if on else 0)
    return observed([], ev, a.mem()), stats


def expected(name, inp, case):
    if name == NAME_REMOVE:
        return remove(inp, case)
    if name == NAME_ADDUP:
        return add_upgrade(inp, case)
    if name == NAME_ADDVEH:
        return add_vehicle(inp, case)
    if name == NAME_DROP:
        return drop(inp, case)
    if name == NAME_SIREN:
        return siren(inp, case)
    raise KeyError(name)


def selftest():
    z = [0] * 40
    # ProcessSirenAndHorn sin sirena, con bocina: contador = 1, un GetHorn
    v = list(z); v[0] = 1; v[1] = 0; v[10] = 0x100; v[12] = 7
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert obs['events'][1][:2] == [20, 0] and st['horn_off'] and obs['mem'] == [[(O_VEH + V_HORN) // 4, 0]], obs
    v[10] = 0x101
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert st['horn_on'] and obs['mem'] == [[(O_VEH + V_HORN) // 4, 1]]
    # alarma activa en un vehiculo no destruido: no hay GetHorn
    v[8] = 5; v[9] = 0x00
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert st['blocked'] and obs['nevents'] == 1
    # con el estado WRECKED (5 << 3) si lo hay
    v[9] = 0x28 | 3
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert st['horn_on'] and obs['nevents'] == 2
    # sirena: indice 0, actual pulsada, i+4 e i+3 pulsadas (historial 0 = 1, 4 = 1, 3 = 1) -> contador 1
    v = list(z); v[1] = 1; v[2] = 0; v[3] = 1; v[6] = 1; v[7] = 1
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert st['counter1'] and obs['mem'] == [[(O_VEH + V_HORN) // 4, 1]], obs
    # actual suelta, (i+4)%5 pulsada, (i+1)%5 suelta -> conmuta el bit 7 de +0x42D
    v = list(z); v[1] = 1; v[2] = 0; v[3] = 0; v[7] = 1; v[4] = 0; v[11] = 0x12; v[12] = 9
    obs, st = expected(NAME_SIREN, {'v': v}, 5)
    assert st['toggle'] and len(obs['mem']) == 2, obs
    # RemoveVehicleUpgrade con enlazado -1, mejora atomica, sin ranuras coincidentes
    v = list(z); v[0] = 16; v[1] = (5 << 10); v[6] = 0xFFFF; v[23] = 1
    for i in range(15):
        v[8 + i] = 0xFFFF
    obs, st = expected(NAME_REMOVE, {'v': v}, 3)
    assert [e[0] for e in obs['events']] == [1, 2, 4] and obs['events'][2][:5] == [4, lab(O_CLUMP), 1, 5, 0] and st['linked_none'] and st['slot_none'], obs
    v[8] = 16; v[9] = 16
    obs, st = expected(NAME_REMOVE, {'v': v}, 3)
    assert st['slot_multi'] and len(obs['mem']) == 2, obs   # ranuras 0 (mitad alta de +0x438) y 1 (mitad baja de +0x43C)
    # carMod 2 con VehDummy
    v[1] = (2 << 10) | 0x100
    obs, st = expected(NAME_REMOVE, {'v': v}, 3)
    assert [e[0] for e in obs['events']] == [1, 2, 3, 3, 3, 3] and st['carmod2'], obs
    # AddUpgrade: padre 1 vuelve tras la primera copia
    v = list(z); v[0] = 17; v[1] = 2
    for i in range(15):
        v[4 + i] = 0xFFFFFFFF
    v[4 + 2] = 1
    obs, st = expected(NAME_ADDUP, {'v': v}, 3)
    assert [e[0] for e in obs['events']] == [5, 4, 6] and st['parent1'], obs
    v[4 + 2] = 4; v[4 + 5] = 8; v[20] = 0x77
    obs, st = expected(NAME_ADDUP, {'v': v}, 3)
    assert [e[0] for e in obs['events']] == [5, 4, 6, 6, 7, 8, 9] and st['alt_used'] and obs['events'][3][3] == lab(O_VST + ST_UPGRADES + 0x20 * 5), obs
    # AddVehicleUpgrade: SetFlags devuelve true y no escribe: reemplazada -1; hueco libre = primera ranura 0xFFFF
    v = list(z); v[0] = 16; v[5] = 1
    for i in range(15):
        v[17 + i] = 0xFFFF if i != 0 else 7
    obs, st = expected(NAME_ADDVEH, {'v': v}, 3)
    assert st['flags_done'] and obs['ret'] == [M32] and len(obs['events']) == 1 and obs['mem'], obs
    # Drop: arg1 falso, tipo 1, sin matriz
    v = list(z); v[0] = 1; v[1] = 0; v[2] = 99; v[6] = 0x3F800000; v[7] = 0x40000000; v[8] = 0x40400000; v[13] = 0x3F800000; v[14] = 0x3F800000; v[15] = 0x3F800000; v[16] = 0
    obs, st = expected(NAME_DROP, {'v': v}, 3)
    assert st['fired'] and st['null_matrix'] and obs['events'][1] == [17, lab(O_VEH), 21, 0x40000000, 0x40400000, 0x40800000, 0, 0x10, 0], obs
    v[16] = 4   # lado 1: x = pos.x - ord.x = 0
    obs, st = expected(NAME_DROP, {'v': v}, 3)
    assert obs['events'][1][3] == 0 and st['side1'], obs
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
