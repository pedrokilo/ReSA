"""Oraculo independiente del banco MissionCleanup64: modela CMissionCleanup::Process (0x468560) y CheckIfCollisionHasLoadedForMissionObjects (0x4652D0) desde el desensamblado del EXE.

`expected(nombre, entrada, caso)` devuelve (salida_esperada, estadisticas). La salida tiene la misma forma que `observed` de Core.hpp:
  events  (lista de [tipo, argumentos...] de los colaboradores falsos EN ORDEN; punteros como fichas simbolicas), nevents, tail ([cuenta, hash FNV-1a] de las ~1960 llamadas del final de Process),
  changes (palabras de la arena del banco que cambian: [bloque, indice, palabra, valor]) y real (bytes de la memoria real del EXE que cambian: [direccion, byte]).
`python -I fixture_oracle.py` ejecuta una autoprueba.
"""
M32 = 0xFFFFFFFF
CASES_P = 2048
CASES_C = 2048
NAME_P = 'CMissionCleanup::Process'
NAME_C = 'CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects'
CASES = {NAME_P: CASES_P, NAME_C: CASES_C}

# tipos de colaborador (mismo orden que el enum de Fixtures.hpp)
(FADE, DISABLE_TRAINS, RELEASE_TRAINS, AMBIENT_PLANES, FILL_ZONES, CLEAR_GANGWAR, RESPRAYS, FPW, PICKUPS, ROADBLOCKS, COPBIKES, RELEASE_NODES, UNMARK, TIDY_NODES, RECORDINGS, WIDE_OFF, INIT_SCRIPTABLE,
 DUCKING, POSTFX, EXTRA_COLOUR, MISSION_AUDIO, WEATHER, SPECIAL_CHAR, SUPPRESSED, FORCE_RENDER, ALLOCATOR, DECISION_MAKER, ALWAYS_FOLLOW, NEVER_FOLLOW, REAPPEAR, SPEECH, SPEECH_SCRIPT, FIND_TASK,
 TASK_FUNC, CLEAR_SKIP, FPP, RESPAWN, FIRE_FLAGS, STUCK_INIT, CAM_RESTORE, SAFE, HELP, CLEAN_VEH, CLEAN_PED, CLEAN_OBJ, GET_IDX, FX_KILL, REMOVE_FX, REMOVE_GROUP, FLUSH, UNLOAD_DM, REMOVE_SEARCH,
 REMOVE_CHECKPOINT, REMOVE_TXD, REMOVE_ENTITY, PED_FLAG, ACQ_ONE, ACQ_ALL, TASK_TYPE, HAS_COL, HAVE_IPL, IS_STATIC, MOVING, PLACE_AUTO, PLACE_BIKE) = range(1, 66)
KINDS = 66

# regiones de memoria real (direccion, bytes), en el orden de Fixtures.hpp
REGIONS = [
    (0x86C6C0, 4), (0x8A5B20, 4), (0x8A625C, 4), (0x8CCB84, 4), (0x8CDE56, 1), (0x8D0934, 4), (0x8D0A24, 1), (0x8D2530, 8),
    (0x9654B0, 1), (0x96A7C8, 1), (0x96A8A8, 1), (0x96A8B0, 1), (0x96AB90, 4), (0x96C008, 1),
    (0xA43F68, 0x10), (0xA44114, 0x1C), (0xA44488, 2), (0xA4448A, 1), (0xA4448C, 8), (0xA44494, 3), (0xA44498, 8), (0xA444A0, 4), (0xA444A5, 1), (0xA444A8, 1),
    (0xA447EC, 4), (0xA447F0, 1), (0xA4892C, 0x30), (0xA44B60, 7),
    (0xB6EC2C, 1), (0xB6F06D, 9), (0xB6FD18, 1), (0xB728E0, 4), (0xB72914, 4), (0xB72994, 1), (0xB73458, 0x134),
    (0xB7CD74, 1), (0xB7CDDD, 2), (0xB7CE48, 4), (0xB7CE74, 1), (0xB7CEEB, 1), (0xB7CFD8, 4),
    (0xBA18D9, 1), (0xBA677C, 1), (0xBAA3F8, 4), (0xBAB1DC, 2), (0xBB4A72, 1),
    (0xC0FCB2, 3), (0xC0994C, 4), (0xC09C20, 4), (0xC09EF4, 4), (0xC0A1C8, 4),
    (0xC17898, 4), (0xC17928, 8), (0xC17968, 8), (0xC179A8, 8), (0xC179E8, 8),
    (0xC1CC00, 2), (0xC1CC18, 1), (0xC3A1A0, 4), (0xC7C70C, 2), (0xC87ACD, 1)]
SEQ_BASE, SEQ_STRIDE, FX_BASE, PAD = 0xC178F0, 0x40, 0xA44114, 0xB73458
K_ENTRIES, K_SLOTS = 75, 6


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


def bg_byte(n, a):
    return (rand((n + 0x6A09E667) & M32, a) >> 8) & 0xFF


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def ped_flag_of(t):
    return (t * 0x9E3779B1 + 7) & M32


class Image:
    """memoria real del EXE: bytes de fondo + controles del caso; lecturas y escrituras little-endian"""

    def __init__(self, n):
        self.b = {}
        for a, sz in REGIONS:
            for j in range(sz):
                self.b[a + j] = bg_byte(n, a + j)
        self.init = dict(self.b)

    def poke(self, a, v, sz):
        for j in range(sz):
            self.b[a + j] = (v >> (8 * j)) & 0xFF

    def rd(self, a, sz):
        return sum(self.b[a + j] << (8 * j) for j in range(sz))

    def changes(self):
        out = []
        for a, sz in REGIONS:
            for j in range(sz):
                if self.b[a + j] != self.init[a + j]:
                    out.append([a + j, self.b[a + j]])
        return out


def tail_hash(words, h):
    for w in words:
        h = ((h ^ (w & M32)) * 16777619) & M32
    return h


def process(inp, stats):
    ev = []
    mem = Image(inp['n'])
    mem.poke(0xA4448A, inp['c4448A'], 1)
    mem.poke(0xB7CE74, inp['mode'], 1)
    mem.poke(0x96AB91, inp['g96AB91'], 1)
    mem.poke(0x96AB90, inp['g96AB90'], 1)
    mem.poke(0xB72914, inp['v72914'], 4)
    mem.poke(0x96A8B0, inp['g96A8B0'], 1)
    mem.poke(0xA444A8, inp['gA444A8'], 1)
    mem.poke(0xB7CD74, inp['focus'], 1)
    mem.poke(0xB7CE48 + inp['focus'] * 0x190, inp['money'], 4)
    mem.poke(0x86C6C0, inp['f86C6C0'], 4)
    for i in range(4):
        mem.poke(SEQ_BASE + i * SEQ_STRIDE + 0x3C, inp['seq3C'][i], 4)
        mem.poke(FX_BASE + i * 8, inp['fx'][i], 4)
    mem.init = dict(mem.b)   # el estado "aplicado": fondo + controles del caso
    # arena: palabras que la funcion modifica (el resto del jugador falso no cambia)
    pd34, pd40, w14, w1C, p734 = inp['pd34'], inp['pd40'], inp['w14'], inp['w1C'], inp['p734']
    init_arena = dict(pd34=pd34, pd40=pd40, w14=w14, w1C=w1C, p734=p734)
    w = mem.poke
    # 0x468571
    if mem.rd(0xA4448A, 1) != 0:
        mode = mem.rd(0xB7CE74, 1)
        w(0xA4448A, 0, 1)
        stats['fade_branch'] = True
        if mode != 2 and mode != 1:
            stats['fade'] = True
            ev.append([FADE, 0xB6F028, 0x3F000000, 1])
            w(PAD + 0x10E, mem.rd(PAD + 0x10E, 1) & 0xDF, 1)
    ev.append([DISABLE_TRAINS, 0])
    ev.append([RELEASE_TRAINS])
    ev.append([AMBIENT_PLANES, 1])
    gang = mem.rd(0x96AB91, 1)
    w(0x8D2530, 0x3F800000, 4)
    w(0x8A5B20, 0x3F800000, 4)
    if gang != 0:
        disable = 1 if mem.rd(0x96AB90, 1) == 0 else 0
        w(0x96AB91, 0, 1)
        ev.append([FILL_ZONES, disable])
        stats['gang_colours_' + str(disable)] = True
    w(0x8D2534, M32, 4)
    w(0x96AB93, 0, 1)
    ev.append([CLEAR_GANGWAR])
    for a in (0xC0FCB2, 0xC0FCB3, 0xC0FCB4, 0x96C008):
        w(a, 0, 1)
    ev.append([RESPRAYS, 1])
    w(0xC87ACD, 0, 1)
    ev.append([FPW, M32])
    w14 = 0x3F800000
    ev.append([PICKUPS])
    ev.append([ROADBLOCKS])
    ev.append([COPBIKES, 0])
    w(0xB72994, 1, 1)
    w(0xBB4A72, 0, 1)
    w(0x8D0A24, 1, 1)
    ev.append([RELEASE_NODES, 0x96F050])
    ev.append([UNMARK, 0x96F050])
    ev.append([TIDY_NODES, 0x96F050])
    ev.append([RECORDINGS])
    ev.append([WIDE_OFF, 0xB6F028])
    w(0xB6F06E, 0x32, 2)
    w(0xB6F070, 0x31, 2)
    w(0xB6F072, 3, 2)
    w(0xB6F074, 0x30, 2)
    w(0xB6F06D, 0, 1)
    w(0xB6FD18, 0, 1)
    ev.append([INIT_SCRIPTABLE, 0xB6F028])
    ev.append([DUCKING, 0xB6F028, 0])
    w(0x8CCB84, 0x3F800000, 4)
    w(0xB6EC2C, 0, 1)
    w(0x96A8A8, 0, 1)
    w(0xA44490, 0, 4)
    w(0xC7C70C, 0, 1)
    w(0xC7C70D, 0, 1)
    ev.append([POSTFX])
    no_extra = mem.rd(0xB72914, 4) == 0
    w(0x96A7C8, 0, 1)
    if no_extra:
        ev.append([EXTRA_COLOUR, 0])
        stats['extra_colour'] = True
    for i in range(4):
        ev.append([MISSION_AUDIO, 0xB6BC90, i])
    ev.append([WEATHER])
    w(0xB728E0, 0x1869F, 4)
    for i in range(10):
        ev.append([SPECIAL_CHAR, i])
    ev.append([SUPPRESSED])
    w(0xA4448C, M32, 4)
    w(0x9654B0, 0, 1)
    w(0xBAB1DC, 0xFFFF, 2)
    for a in (0xBAA3FB, 0xBAA3F9, 0xBAA3F8, 0xBAA3FA, 0xBA677C, 0xA444A3, 0xA444A2, 0xA444A1):
        w(a, 0, 1)
    ev.append([FORCE_RENDER, 0])
    w(0xA444A0, 1, 1)
    w(0xA4449C, 0, 4)
    w(0xA44498, 0, 4)
    for a in (0xA44496, 0xA44495, 0xA44494):
        w(a, 1, 1)
    w(0xBA18D9, 0, 1)
    w(0xA44B66, 0, 1)
    w(0xA44B64, 0, 2)
    w(0xA44B60, 0, 2)
    w(0xA44489, 1, 1)
    w(0xA44488, 1, 1)
    # grupo del jugador
    group = s32(inp['pdGroup']) * 0x2D4
    intel = (0xC09950 + group) & M32
    ev.append([ALLOCATOR, intel, 1])
    ev.append([DECISION_MAKER, intel, M32])
    w((0xC0994C + group) & M32, mem.rd(0x86C6C0, 4), 4)
    pd34 &= 0xFFFFFEFF
    ev.append([ALWAYS_FOLLOW, 'PED', 0])
    ev.append([NEVER_FOLLOW, 'PED', 0])
    ev.append([REAPPEAR, 'PED'])
    pd40 = (pd40 & 0x00FFFFFF) | 0x63000000
    p734 = 0x3F800000
    w(0xB7CDDD, 1, 1)
    w(0xB7CDDE, 0, 1)
    ev.append([SPEECH, 'PED'])
    ev.append([SPEECH_SCRIPT, 'PED'])
    w(PAD + 0x74, 0, 4)
    for o in range(0x118, 0x120):
        w(PAD + o, 0, 1)
    w(0xB7CEEB, 1, 1)
    ev.append([FIND_TASK, 'INTEL+4', 3, 0x640])
    if inp['hasTask']:
        ev.append([TASK_TYPE, 'TASK'])
        stats['task_found'] = True
        if inp['taskType'] == 0x640:
            ev.append([TASK_FUNC, 'TASK', 'PED'])
            stats['task_func'] = True
    w(0xC1CC00, 0, 1)
    w(0xC1CC01, 0, 1)
    ev.append([CLEAR_SKIP, 1])
    mode = mem.rd(0x96A8B0, 1)
    if mode != 2 and mode != 1:
        ev.append([FPP, M32])
        if inp['state530'] != 0x37:
            ev.append([FPP, M32])
            if inp['state530'] != 0x36:
                ev.append([RESPAWN])
                stats['respawn'] = True
    w(0xA444A5, 0, 1)
    ev.append([FIRE_FLAGS, 0xB71F80])
    w(0xA447EC, M32, 4)
    w(0xA447F0, 1, 1)
    for i in range(6):
        w(0xA4892C + 8 * i, M32, 4)
        w(0xA4892C + 8 * i + 4, 0, 4)
    ev.append([STUCK_INIT, 0xA90AB0])
    skip = mem.rd(0xA444A8, 1) != 0
    w(0x8CDE56, 1, 1)
    w(0x8A625C, 0xBF800000, 4)
    w(0x8D0934, 0x43480000, 4)
    w(0xC1CC18, 0, 1)
    if not skip:
        stats['camera_block'] = True
        focus = mem.rd(0xB7CD74, 1)
        if mem.rd(0xB7CE48 + focus * 0x190, 4) == 0:
            ev.append([CAM_RESTORE, 0xB6F028])
            stats['cam_restore'] = True
        b = (w1C >> 16) & 0xFF
        b = (b & 0xFE) & 0xFB
        w1C = (w1C & 0xFF00FFFF) | (b << 16)
        ev.append([SAFE, 0xB7CD98, 0, 0x461C4000])
        ev.append([HELP, 0, 1, 0, 0])
    # entradas
    pools = [inp['flags'][0], inp['flags'][1], inp['flags'][2]]
    kinds_seen = set()
    gk = 0
    gsi = inp['gsi']

    def get_idx(h, t):
        nonlocal gk
        ev.append([GET_IDX, h, t])
        r = gsi[gk % 16]
        gk += 1
        return r

    for i in range(K_ENTRIES):
        t = inp['type'][i] & 0xFF
        h = inp['handle'][i]
        if t == 0:
            continue
        kinds_seen.add(t)
        if t in (1, 2, 3):
            slot = s32(h) >> 8
            if pools[t - 1][slot] == (h & 0xFF):
                ev.append([(CLEAN_VEH, CLEAN_PED, CLEAN_OBJ)[t - 1], '%s%d' % ('VPO'[t - 1], slot)])
                stats['clean_pool_%d' % t] = True
            else:
                stats['clean_miss_%d' % t] = True
        elif t == 4:
            r = get_idx(h, 1)
            if s32(r) >= 0:
                fx = mem.rd(FX_BASE + r * 8, 4)
                if fx != 0:
                    ev.append([FX_KILL, fx])
                    ev.append([REMOVE_FX, h])
                    stats['fx_kill'] = True
        elif t == 5:
            r = get_idx(h, 8)
            if s32(r) >= 0:
                ev.append([REMOVE_GROUP, r])
        elif t == 7:
            r = get_idx(h, 6)
            if s32(r) >= 0:
                w(0xC3A1A0 + r, 0, 1)
        elif t == 8:
            r = get_idx(h, 4)
            if s32(r) >= 0:
                seq = SEQ_BASE + r * SEQ_STRIDE
                if mem.rd(seq + 0x3C, 4) == 0:
                    w(seq + 0x38, 0, 1)
                    ev.append([FLUSH, seq])
                    stats['seq_flush'] = True
                else:
                    w(seq + 0x38, 1, 1)
                    stats['seq_keep'] = True
                w(0xC17898 + r, 0, 1)
                w(0xA43F68 + r * 4, 0, 1)
        elif t == 9:
            r = get_idx(h, 7)
            if s32(r) >= 0:
                ev.append([UNLOAD_DM, r])
        elif t == 11:
            r = get_idx(h, 2)
            if s32(r) >= 0:
                ev.append([REMOVE_SEARCH, h])
        elif t == 12:
            r = get_idx(h, 3)
            if s32(r) >= 0:
                ev.append([REMOVE_CHECKPOINT, h])
        elif t == 13:
            ev.append([REMOVE_TXD])
        ev.append([REMOVE_ENTITY, 'CLEAN', h, t])
        stats['entry_type_%d' % t] = True
    stats['dense_entries'] = len(kinds_seen) >= 8
    words = []
    count = 0
    for ped_type in range(0x18):
        for other in range(0x18, 0x20):
            for ident in range(5):
                f = ped_flag_of(other)
                words += [PED_FLAG, other, ACQ_ONE, ident, ped_type, f]
                count += 2
    for other in range(0x18, 0x20):
        for ident in range(5):
            words += [ACQ_ALL, ident, other]
            count += 1
    h = tail_hash(words, 2166136261)
    # cambios de la arena (palabras que difieren del estado inicial)
    changes = []
    if p734 != init_arena['p734']:
        changes.append(['PED', 0, 0x734 // 4, p734])
    if pd34 != init_arena['pd34']:
        changes.append(['PD', 0, 13, pd34])
    if pd40 != init_arena['pd40']:
        changes.append(['PD', 0, 16, pd40])
    if w14 != init_arena['w14']:
        changes.append(['W', 0, 5, w14])
    if w1C != init_arena['w1C']:
        changes.append(['W', 0, 7, w1C])
    return {'events': ev, 'nevents': len(ev), 'tail': [count, h], 'changes': changes, 'real': mem.changes()}


def check_if(inp, stats):
    ev = []
    pools = inp['flags']
    ents = inp['ent']
    flags = [e['flags'] for e in ents]
    k = {'hcl': 0, 'ipl': 0, 'stat': 0}
    for i in range(K_ENTRIES):
        t = inp['type'][i] & 0xFF
        h = inp['handle'][i]
        if t not in (1, 2, 3):
            continue
        slot = s32(h) >> 8
        if pools[t - 1][slot] != (h & 0xFF):
            stats['miss'] = True
            continue
        idx = (t - 1) * K_SLOTS + slot
        e = ents[idx]
        name = '%s%d' % ('VPO'[t - 1], slot)
        if not flags[idx] & 0x40000:
            stats['no_waiting_flag'] = True
            continue
        pos = 'M%d+48' % idx if e['mat'] else name + '+4'
        stats['matrix' if e['mat'] else 'no_matrix'] = True
        area = e['area']
        ev.append([HAS_COL, pos, area])
        if not (inp['hcl'] >> (k['hcl'] & 31)) & 1:
            k['hcl'] += 1
            stats['col_false'] = True
            continue
        k['hcl'] += 1
        ev.append([HAVE_IPL, pos, area])
        if not (inp['ipl'] >> (k['ipl'] & 31)) & 1:
            k['ipl'] += 1
            stats['ipl_false'] = True
            continue
        k['ipl'] += 1
        flags[idx] &= 0xFFFBFFFF
        ev.append([IS_STATIC, name])
        st = (inp['stat'] >> (k['stat'] & 31)) & 1
        k['stat'] += 1
        if not st:
            ev.append([MOVING, name])
            stats['moving'] = True
        else:
            stats['static'] = True
        stats['cleared_%d' % t] = True
        if t == 1:
            vt = e['vtype']
            if vt in (0, 11):
                ev.append([PLACE_AUTO, name])
                stats['place_auto_%d' % vt] = True
            elif vt == 9:
                ev.append([PLACE_BIKE, name])
                stats['place_bike'] = True
            else:
                stats['place_none'] = True
    changes = []
    for idx in range(3 * K_SLOTS):
        if flags[idx] != ents[idx]['flags']:
            changes.append(['VPO'[idx // K_SLOTS], idx % K_SLOTS, 7, flags[idx]])
    return {'events': ev, 'nevents': len(ev), 'tail': [0, 2166136261], 'changes': changes, 'real': []}


def expected(name, inp, case):
    stats = {}
    if name == NAME_P:
        out = process(inp, stats)
    elif name == NAME_C:
        out = check_if(inp, stats)
    else:
        raise ValueError(name)
    return out, stats


if __name__ == '__main__':
    assert rand(1, 2) == mix((1 * 0x9E3779B1 + 2 * 0x85EBCA6B + 12345) & M32)
    assert ped_flag_of(0x18) == (0x18 * 0x9E3779B1 + 7) & M32
    print('oraculo ok')
