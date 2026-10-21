"""Oraculo independiente del banco Remote64 (lote 64): modela, desde el DESENSAMBLADO del EXE (0x45AB10..0x45AD33 y 0x45AE80..0x45AF35), lo que hacen
CRemote::GivePlayerRemoteControlledCar y CRemote::TakeRemoteControlledCarFromPlayer con los colaboradores sustituidos por falsos.

expected(nombre, entrada, caso) -> (observado, estadisticas): `observado` es EXACTAMENTE el objeto que Core.hpp registra por lado (eventos de los falsos EN ORDEN con sus argumentos, cambios de la
arena del banco y cambios del estado real del EXE); `estadisticas` son las claves de cobertura. `python -I fixture_oracle.py` ejecuta la autoprueba.
"""
import struct

M32 = 0xFFFFFFFF
NAME_GIVE = 'CRemote::GivePlayerRemoteControlledCar'
NAME_TAKE = 'CRemote::TakeRemoteControlledCarFromPlayer'
NAMES = (NAME_GIVE, NAME_TAKE)
CASES_BY_NAME = {NAME_GIVE: 8192, NAME_TAKE: 4096}

VEH_SIZE = 0xA18
FOCUS_ADDR = 0xB7CD74
PLAYERS = 0xB7CD98
STRIDE = 0x190
CAMERA = 0xB6F028
CLEANUP = 0xA90850
STATES_BASE = 24
NONE = 0xFF
# kinds (mismo orden que Fixtures.hpp)
(K_ISHELI, K_ISPLANE, K_NEW, K_CTORHELI, K_CTORPLANE, K_CTORAUTO, K_GROUND, K_DIST, K_ROTZ, K_JOIN, K_ADD, K_FPV, K_REGREF, K_TAKECTL, K_ZOOM, K_SETCREATED, K_REMOVE) = range(1, 18)
REAL_NAMES = ['focus', 'timer', 'pool', 'p0_remote', 'p0_dd', 'p0_de', 'p0_df', 'p0_time', 'p1_remote', 'p1_dd', 'p1_de', 'p1_df', 'p1_time']


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


def veh_junk(n, i, w):
    return rand(n ^ 0x51ED, i * 0x1000 + w)


def mat_junk(n, i, w):
    return rand(n ^ 0x3A7, 0x100 * i + w + 77)


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def bits_of(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def is_nan(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x007FFFFF) != 0


def add_f32(a_bits, b_bits):
    """suma de dos float a 24 bits (x87 PC24 o SSE: mismo resultado para los valores del banco); un NaN sale silenciado"""
    if is_nan(a_bits):
        return a_bits | 0x00400000
    if is_nan(b_bits):
        return b_bits | 0x00400000
    return bits_of(f32(a_bits) + f32(b_bits))


def ext80_of_f32(bits):
    """imagen de 10 bytes (hex) del float en un registro x87: solo cero y normales (los valores del banco)"""
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    frac = bits & 0x7FFFFF
    if exp == 0 and frac == 0:
        return '00' * 8 + ('0080' if sign else '0000')
    assert 0 < exp < 255
    mant = (1 << 63) | (frac << 40)
    e = exp - 127 + 16383
    return mant.to_bytes(8, 'little').hex() + ((sign << 15) | e).to_bytes(2, 'little').hex()


def tok(p):
    if p == 0 or p is None:
        return 0
    if isinstance(p, int):
        return p
    if p[0] == 'V':
        return 'V%d' % p[1] if p[2] == 0 else 'V%d+%d' % (p[1], p[2])
    if p[0] == 'M':
        return 'M%d' % p[1]
    if p[0] == 'L':
        return 'L'
    raise ValueError(p)


class Mem:
    """memoria de la arena: palabras iniciales por formula (mas los pokes) y escrituras a nivel de byte; las matrices a nivel de palabra"""

    def __init__(self, n, inp):
        self.n = n
        self.pokes = {}
        for i, off, val in inp['pokes']:
            if i != NONE:
                self.pokes[(i, off)] = val & 0xFF
        self.vw = {}
        self.wr = {}
        self.mw = {}

    def init_byte(self, i, o):
        if (i, o) in self.pokes:
            return self.pokes[(i, o)]
        w = o // 4
        assert w != 5, 'el oraculo no modela la palabra del puntero de matriz'
        key = (i, w)
        if key not in self.vw:
            self.vw[key] = veh_junk(self.n, i, w)
        return (self.vw[key] >> (8 * (o % 4))) & 0xFF

    def get(self, i, o):
        return self.wr[(i, o)] if (i, o) in self.wr else self.init_byte(i, o)

    def put(self, i, o, v):
        self.wr[(i, o)] = v & 0xFF

    def put32(self, i, o, v):
        for k in range(4):
            self.put(i, o + k, (v >> (8 * k)) & 0xFF)

    def changes(self):
        out = []
        words = sorted({(i, o // 4) for (i, o) in self.wr})
        for i, w in words:
            old = 0
            new = 0
            for k in range(4):
                old |= self.init_byte(i, 4 * w + k) << (8 * k)
                new |= self.get(i, 4 * w + k) << (8 * k)
            if old != new:
                out.append(['V', i, w, new])
        for (i, w) in sorted(self.mw):
            if self.mw[(i, w)] != mat_junk(self.n, i, w):
                out.append(['M', i, w, self.mw[(i, w)]])
        return out


def ptr_of(slot):
    return 0 if slot[0] == NONE else ('V', slot[0], slot[1])


def real_changes(inp, focus, remote, flags, timer_before, timer_after=None):
    """lista de cambios del estado real respecto al estado aplicado por el caso"""
    init_focus = inp['focus']
    init_remote = [ptr_of(s) for s in inp['slots']]
    init_flags = [list(f) for f in inp['fl']]
    rows = {}
    if focus != init_focus:
        rows[0] = ['focus', focus]
    for p in range(2):
        if remote[p] != init_remote[p]:
            rows[3 + 5 * p] = [REAL_NAMES[3 + 5 * p], tok(remote[p])]
        for k in range(4):
            if flags[p][k] != init_flags[p][k]:
                rows[4 + k + 5 * p] = [REAL_NAMES[4 + k + 5 * p], flags[p][k]]
    return [rows[k] for k in sorted(rows)]


def expected(name, inp, n):
    if name == NAME_GIVE:
        return expected_give(inp, n)
    return expected_take(inp, n)


def expected_give(inp, n):
    pv_flag, pv_idx = inp['pv']
    m = Mem(n, inp)
    ev = []
    stats = {}
    model = inp['model'] & 0xFFFF
    pos = inp['pos']
    ev.append([K_ISHELI, model])
    new = ('V', inp['new'], 0)
    if inp['heli']:
        kind = 'heli'
        ev.append([K_NEW, 0xA18])
        ev.append([K_CTORHELI, tok(new), model, 2])
    else:
        ev.append([K_ISPLANE, model])
        if inp['plane']:
            kind = 'plane'
            ev.append([K_NEW, 0xA04])
            ev.append([K_CTORPLANE, tok(new), model, 2])
        else:
            kind = 'auto'
            ev.append([K_NEW, 0x988])
            ev.append([K_CTORAUTO, tok(new), model, 2, 1])
    vi = inp['new'] if inp['ret'] == NONE else inp['ret']          # el EXE sigue con el valor que devuelve el constructor
    vp = ('V', vi, 0)
    stats[kind] = True
    stats['ret_other'] = inp['ret'] != NONE
    stats['model_high'] = model >= 0x8000
    stats['model_upper_garbage'] = (inp['model'] >> 16) != 0
    probe_z = add_f32(pos[2], 0x40000000)                           # z + 2.0f (fld/fadd [0x858CA0]/fstp)
    ev.append([K_GROUND, pos[0], pos[1], probe_z, 'L', 0])
    ev.append([K_DIST, tok(vp)])
    z = add_f32(inp['dist'], inp['ground'])                          # fadd [z] sobre la distancia; fstp
    has_mat = inp['mat'][vi] != 0
    ev.append([K_ROTZ, tok(('M', vi)) if has_mat else 0, inp['rot']])
    if has_mat:
        m.mw[(vi, 12)] = pos[0]
        m.mw[(vi, 13)] = pos[1]
        m.mw[(vi, 14)] = z
    else:
        m.put32(vi, 4, pos[0])
        m.put32(vi, 8, pos[1])
        m.put32(vi, 12, z)
    stats['matrix'] = has_mat
    stats['no_matrix'] = not has_mat
    stats['pos_xy_odd'] = any(is_nan(pos[k]) or (pos[k] & 0x7F800000) == 0x7F800000 for k in (0, 1))
    stats['pos_z_nan'] = is_nan(pos[2])
    m.put(vi, 0x36, (m.get(vi, 0x36) & 7) | 0x40)
    m.put(vi, 0x428, m.get(vi, 0x428) | 8)
    ev.append([K_JOIN, tok(vp)])
    m.put(vi, 0x428, m.get(vi, 0x428) ^ inp['j'][0])                 # el falso reescribe campos del vehiculo
    m.put(vi, 0x42D, m.get(vi, 0x42D) ^ inp['j'][1])
    stats['join_428'] = inp['j'][0] != 0
    stats['join_42D'] = inp['j'][1] != 0
    bit2 = (m.get(vi, 0x42D) & 2) != 0                               # se lee tras la llamada
    for o in (0x3BA, 0x3BB, 0x3B9, 0x3B7, 0x3B8):
        m.put(vi, o, 0)
    m.put32(vi, 0x3C8, 0x41100000)
    m.put(vi, 0x3D0, 9)
    b = m.get(vi, 0x428)
    m.put(vi, 0x428, (b & 0xEF) if bit2 else (b | 0x10))
    stats['bit2_set'] = bit2
    stats['bit2_clear'] = not bit2
    ev.append([K_ADD, tok(vp)])
    ev.append([K_FPV, 0xFFFFFFFF, 0])
    stats['pv'] = bool(pv_flag)
    stats['pv_none'] = not pv_flag
    if pv_flag:
        ev.append([K_FPV, 0xFFFFFFFF, 0])
        m.put(pv_idx, 0x36, (m.get(pv_idx, 0x36) & 7) | 0x48)
        stats['pv_is_subject'] = pv_idx == vi
        stats['pv_other'] = pv_idx != vi
    focus = inp['focus']
    remote = [ptr_of(s) for s in inp['slots']]
    flags = [list(f) for f in inp['fl']]
    remote[focus] = vp
    ev.append([K_REGREF, tok(vp), PLAYERS + focus * STRIDE + 0xB0])
    ev.append([K_TAKECTL, CAMERA, tok(vp), 0x12, 1, 1])
    ev.append([K_ZOOM, CAMERA, 1])
    stats['focus1'] = focus == 1
    stats['focus0'] = focus == 0
    stats['slot_was_null'] = inp['slots'][focus][0] == NONE
    stats['gflag'] = bool(inp['gflag'])
    want = {'events': ev, 'nevents': len(ev), 'changes': m.changes(), 'globals': real_changes(inp, focus, remote, flags, inp['timer'])}
    stats['z_bits'] = z
    return want, stats


def expected_take(inp, n):
    m = Mem(n, inp)
    ev = []
    stats = {}
    focus = inp['focus']
    remote = [ptr_of(s) for s in inp['slots']]
    flags = [list(f) for f in inp['fl']]
    storage = ('V', inp['stor'][0], inp['stor'][1])
    r1 = remote[focus]
    created = m.get(r1[1], r1[2] + 0x4A4)
    stats['created_mission'] = created == 2
    stats['created_other'] = created != 2

    def mutate(mu):
        kind, mf, pl, idx, off = mu
        if kind & 2:
            remote[pl] = 0 if idx == NONE else ('V', idx, off)
        if kind & 1:
            return mf
        return None

    if created == 2:
        ev.append([K_SETCREATED, tok(r1), 1])
        nf = mutate(inp['mA'])
        if nf is not None:
            focus = nf
        stats['mA_focus'] = bool(inp['mA'][0] & 1)
        stats['mA_remote'] = bool(inp['mA'][0] & 2)
        r2 = remote[focus]
        diff = (r2[1] - storage[1]) * VEH_SIZE + r2[2] - storage[2]
        idx = abs(diff) // VEH_SIZE * (1 if diff >= 0 else -1)         # division con signo hacia cero
        state = inp['states'][STATES_BASE + idx]
        handle = (state + ((idx << 8) & M32)) & M32
        ev.append([K_REMOVE, CLEANUP, handle, 1])
        nf = mutate(inp['mB'])
        if nf is not None:
            focus = nf
        stats['mB_focus'] = bool(inp['mB'][0] & 1)
        stats['mB_remote'] = bool(inp['mB'][0] & 2)
        stats['idx_neg'] = idx < 0
        stats['idx_zero'] = idx == 0
        stats['idx_pos'] = idx > 0
        stats['unaligned'] = r2[2] != 0
        stats['handle_nonzero'] = handle != 0
    r3 = remote[focus]
    m.put(r3[1], r3[2] + 0x428, m.get(r3[1], r3[2] + 0x428) & 0xF7)
    f4 = focus
    flags[f4] = [1, inp['barg'] & 0xFF, 1, inp['timer']]
    stats['arg1'] = (inp['barg'] & 0xFF) == 1
    stats['arg0'] = (inp['barg'] & 0xFF) == 0
    stats['focus1'] = inp['focus'] == 1
    stats['focus0'] = inp['focus'] == 0
    stats['focus_changed'] = f4 != inp['focus']
    want = {'events': ev, 'nevents': len(ev), 'changes': m.changes(), 'globals': real_changes(inp, f4, remote, flags, inp['timer'])}
    return want, stats


def expected_dead_x87(name, inp, n):
    """imagen hex de ST(7) (= T-1, registro muerto) tras la funcion; None si no se modela"""
    if name != NAME_GIVE:
        return None
    z = expected_give(inp, n)[1]['z_bits']
    return ext80_of_f32(z)


if __name__ == '__main__':
    assert mix(1) == mix(1) and rand(1, 1) == rand(1, 1)
    assert ext80_of_f32(0x3F800000) == '0000000000000080ff3f'
    assert add_f32(0x3FC00000, 0x40000000) == 0x40600000
    assert add_f32(0x7F800001, 0x40000000) == 0x7FC00001
    print('oraculo ok')
