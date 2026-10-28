"""Oraculo independiente (Python) de Veh80A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse (traza de los colaboradores EN ORDEN con
todos sus argumentos, bytes cambiados de las arenas, reloj y camara) y lo compara con las filas de comparisons.jsonl (lado original y candidato).

Desensamblado leido: CVehicle::DoDriveByShootings 0x741FD0..0x7422FD (814 bytes, thiscall sin argumentos, `ret`, void).

Resumen del desensamblado (direcciones del EXE):
  0x741FD7  esi = [this+0x460] (conductor); nulo -> sale. 0x741FFF  call 0x609FF0 (GetPlayerInfoForThisPlayerPed, ecx = conductor) -> nulo sale; byte [info+0x153] == 0 sale.
  0x74201E  call 0x609560 (GetPadFromPlayer) -> nulo sale. 0x742031  movsx eax, byte [conductor+0x718] (ranura con signo) * 0x1C + conductor + 0x5A0 = arma; type = [arma] leido ANTES de
            call 0x743C60 (GetWeaponInfo(type, 1)); [info+0x14] != 4 sale.
  0x742060  cmp [this+0x594], 10 (subtipo; BMX) y call 0x53FF90 (GetCarGunFired, AX): BMX dispara si AX == 1; el resto si AX != 0.
  0x742087  call [vtabla+0xBC] (GetRideAnimData): no nulo -> bandera de montar, segunda llamada, grupo = [resultado]; ids 0xCA/0xCB/0xCC. Nulo: grupo 0, ids 0x4A/0x4B (0x4C/0x4D si
            [this+0x429] & 8) y 0xBF.
  0x7420ED  call 0x73DB40 (CWeapon::Update(0)). 0x7420F6  camara activa = byte [0xB6F081]; modo = word [camara + 0xC]; si modo == 1 o byte [0xB6F057] != 0 o modo == 0x31: mirar a la izquierda
            = call 0x53FDD0 (AL != 0), a la derecha = call 0x53FE10; si no, bytes [camara+8] / [camara+9].
  0x74215D  con datos de montar basta disparar; sin ellos, mirar a un lado. Si ademas [arma+0xC] > 0 (con signo): id = izquierda, si no derecha, si no fijo; call 0x4D68B0 (GetAssociation(clump
            [conductor+0x18], id)); se mezcla de nuevo (call 0x4D4610 (BlendAnimation(clump, grupo, id, 16.0))) si no existe o si delta [+0x1C] < 0.0 (ordenado); con asociacion: flags [+0x2E] & 1 sale; amount [+0x18]
            > 0.99 (ordenado) si no sale. Despues: sin disparo sale; reloj (0xB7CB84) <= [arma+0x10] (sin signo) sale; call 0x73FA20 (FireFromCar(this, izq, der)); [arma+0x10] = reloj + 0x46; call 0x5DF340 (DoGunFlash(0xFA, 0)).
  0x74216D  si no se dispara: total != 0 -> GetWeaponInfo(type, 1) otra vez, cargador = min(total, movsx(word [info+0x20])) sin signo; y delta = -8.0 en las asociaciones de izquierda, derecha y (con datos de montar) fija.
Etiquetas: un puntero dentro de una arena se escribe 0x10000000 + (indice << 20) + desplazamiento desde el inicio de la arena (guarda de 0x40 incluida).
Indices: 0 this, 1 conductor, 2 info A, 3 info B, 4 info del jugador, 5 ride A, 6 ride B, 7..10 asociaciones, 11 mando.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)       python -I fixture_oracle.py   (sin argumentos: autoprueba, imprime ok)
"""
import json
import struct
import sys

GUARD = 0x40
DRV_OBJ = 0x1000
M32 = 0xFFFFFFFF
T_THIS, T_DRV, T_INFO_A, T_INFO_B, T_PINFO, T_RIDE_A, T_RIDE_B, T_ASSOC0, T_PAD = 0, 1, 2, 3, 4, 5, 6, 7, 11
WEAPON_BASE, WEAPON_SIZE = 0x5A0, 0x1C
CAM_ARRAY, CAM_SIZE = 0xB6F19C, 0x238
K099 = 0x3F7D70A4          # 0.99f (0x862CD0)
MINUS8 = 0xC1000000


def tag(idx, off):
    return 0x10000000 + (idx << 20) + off


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def lt(a_bits, b_bits):
    """a < b ordenado (con NaN, falso)"""
    if is_nan(a_bits) or is_nan(b_bits):
        return False
    return f32(a_bits) < f32(b_bits)


def gt(a_bits, b_bits):
    if is_nan(a_bits) or is_nan(b_bits):
        return False
    return f32(a_bits) > f32(b_bits)


def s8(v):
    v &= 0xFF
    return v - 256 if v & 0x80 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


class Model:
    def __init__(self, c):
        self.c = c
        self.events = []
        self.hash = 2166136261
        self.nev = 0
        self.get_calls = 0
        self.info_calls = 0
        self.ride_calls = 0
        # estado mutable
        self.w = dict(type=c['wtype'], state=c['wstate'], inclip=c['clip0'], total=c['total'], next=c['nextshot'])
        self.timer = c['now']
        self.clump = c['clump']
        self.slot = c['slot'] & 0xFF
        self.drv_ptr = 1 if c['has_driver'] else 0           # solo importa si es nulo al empezar (la funcion lo lee una vez)
        self.drv_field = None
        self.vsub = c['vsub']
        self.vflags = c['vflags'] & 0xFF
        self.can = c['can_drive_by'] & 0xFF
        first_a = c['info_map'] <= 1
        second_a = not (c['info_map'] in (0, 3))
        self.info = {'A': dict(slot=c['infob_slot'], clip=c['infoa_clip'] & 0xFFFF), 'B': dict(slot=c['infob_slot'], clip=c['infoa_clip'] & 0xFFFF)}
        self.info['A' if first_a else 'B']['slot'] = c['info1_slot']
        self.info['A' if second_a else 'B']['clip'] = c['info2_clip'] & 0xFFFF
        self.first_info = 'A' if first_a else 'B'
        self.second_info = 'A' if second_a else 'B'
        self.ride = {'A': c['ride_a_group'], 'B': c['ride_b_group']}
        self.assoc = [dict(flags=c['assoc'][3 * i] & 0xFFFF, amount=c['assoc'][3 * i + 1], delta=c['assoc'][3 * i + 2]) for i in range(4)]
        self.cam_active = c['cam_active'] & 0xFF
        self.cam_flag = c['cam_flag'] & 0xFF
        self.cam_mode = [m & M32 for m in c['cam_mode']]
        self.cam_ll = [v & 0xFF for v in c['cam_ll']]
        self.cam_lr = [v & 0xFF for v in c['cam_lr']]
        self.weapon_off = WEAPON_BASE + s8(c['slot']) * WEAPON_SIZE
        self.weapon_tag = tag(T_DRV, DRV_OBJ + self.weapon_off)
        self.ped_tag = tag(T_DRV, DRV_OBJ)
        self.veh_tag = tag(T_THIS, GUARD)
        self.pad_tag = tag(T_PAD, GUARD)
        self.labels = set()
        self.muts = [(c['mut'][3 * i], c['mut'][3 * i + 1], c['mut'][3 * i + 2]) for i in range(c['nmut'])]

    # ---- eventos y mutaciones
    def push(self, kind, vals, ret=None):
        e = [kind] + [v & M32 for v in vals]
        if len(self.events) < 256:
            self.events.append(e)
        else:
            self.events[255] = e
        idx = self.nev
        self.nev += 1
        for v in [kind, len(vals)] + [v & M32 for v in vals]:
            for b in range(4):
                self.hash ^= (v >> (8 * b)) & 0xFF
                self.hash = (self.hash * 16777619) & M32
        for after, target, value in self.muts:
            if after == idx:
                self.apply(target, value, ret)

    def apply(self, target, v, ret):
        if target == 0:
            self.w['type'] = v
        elif target == 1:
            self.w['total'] = v
        elif target == 2:
            self.w['next'] = v
        elif target == 15:
            self.w['inclip'] = v
        elif target == 3:
            self.timer = v
        elif target == 4:
            self.clump = v
        elif target == 5:
            self.slot = v & 0xFF
        elif target == 6:
            self.drv_field = v
        elif target == 7:
            self.vsub = v
        elif target == 8:
            self.vflags = v & 0xFF
        elif target == 9:
            self.cam_active = v % 3
        elif target == 10:
            if self.cam_active <= 2:
                self.cam_mode[self.cam_active] = v
        elif target == 11:
            self.cam_flag = v & 0xFF
        elif target == 12:
            if self.cam_active <= 2:
                self.cam_ll[self.cam_active] = v & 0xFF
        elif target == 13:
            if self.cam_active <= 2:
                self.cam_lr[self.cam_active] = v & 0xFF
        elif target == 14:
            self.can = v & 0xFF
        elif target in (16, 17, 18) and ret is not None:
            kind, key = ret
            if kind == 'info':
                if target == 16:
                    self.info[key]['slot'] = v
                elif target == 17:
                    self.info[key]['clip'] = v & 0xFFFF
            elif kind == 'ride':
                if target == 16:
                    self.ride[key] = v
            elif kind == 'assoc':
                a = self.assoc[key]
                if target == 16:
                    a['flags'] = v & 0xFFFF
                elif target == 17:
                    a['amount'] = v
                else:
                    a['delta'] = v

    def lab(self, name):
        self.labels.add(name)

    def cam_read(self):
        a = self.cam_active
        if a <= 2:
            return self.cam_mode[a] & 0xFFFF, self.cam_ll[a], self.cam_lr[a]
        r = self.c['cam_read']
        return r[0], r[1], r[2]


def model(c):
    m = Model(c)
    run(m)
    return m


def run(m):
    c = m.c
    if not c['has_driver']:
        m.lab('exit_no_driver')
        return
    m.push(1, [m.ped_tag])
    if c['pinfo_null']:
        m.lab('exit_pinfo_null')
        return
    if (m.can & 0xFF) == 0:
        m.lab('exit_cannot_drive_by')
        return
    m.lab('can_drive_by_nonzero_%s' % ('one' if (m.can & 0xFF) == 1 else 'other'))
    m.push(2, [m.ped_tag])
    if c['pad_null']:
        m.lab('exit_pad_null')
        return
    slot_now = s8(m.slot)
    assert slot_now == s8(c['slot']), 'la ranura cambio antes de leerse (fixture invalida)'
    wtype = m.w['type']                       # [arma] se lee antes de la llamada
    obj = m.first_info
    m.info_calls = 1
    m.push(3, [wtype, 1, 0], ret=('info', obj))
    if m.info[obj]['slot'] & M32 != 4:
        m.lab('exit_not_smg')
        return
    m.lab('slot_negative' if s8(m.slot) < 0 else 'slot_above_12' if s8(m.slot) > 12 else 'slot_0_12')
    sub_is_bmx = (m.vsub & M32) == 10          # se lee tras GetWeaponInfo, antes de GetCarGunFired
    m.push(4, [m.pad_tag])
    ax = c['gun_ret'] & 0xFFFF
    fire = (ax == 1) if sub_is_bmx else (ax != 0)
    m.lab('bmx' if sub_is_bmx else 'not_bmx')
    m.lab('fire_true' if fire else 'fire_false')
    if sub_is_bmx and ax != 1 and ax != 0:
        m.lab('bmx_ax_nonzero_not_one')
    if sub_is_bmx and (c['gun_ret'] & 0xFFFF0000) and ax == 1:
        m.lab('ax_one_with_high_garbage')
    # GetRideAnimData (virtual) dos veces
    m.push(12, [m.veh_tag, 0], ret=('ride', 'A') if c['ride1'] == 1 else ('ride', 'B') if c['ride1'] == 2 else None)
    has_ride = c['ride1'] != 0
    m.lab('ride' if has_ride else 'no_ride')
    if has_ride and c['ride1'] != c['ride2']:
        m.lab('ride_second_call_different_object')
    group = 0
    if has_ride:
        m.push(12, [m.veh_tag, 1], ret=('ride', 'A') if c['ride2'] == 1 else ('ride', 'B'))
        group = m.ride['A' if c['ride2'] == 1 else 'B']
        id_l, id_r, id_f = 0xCA, 0xCB, 0xCC
    else:
        id_f = 0xBF
        if m.vflags & 8:
            id_l, id_r = 0x4C, 0x4D
            m.lab('low_vehicle')
        else:
            id_l, id_r = 0x4A, 0x4B
            m.lab('normal_vehicle')
    m.push(7, [m.weapon_tag, 0])
    mode16, ll_cam, lr_cam = m.cam_read()
    left = right = False
    m.lab('cam_oob' if m.cam_active > 2 else 'cam_in_range')
    if mode16 == 1 or (m.cam_flag & 0xFF) != 0 or mode16 == 0x31:
        m.lab('cam_pad_mode1' if mode16 == 1 else 'cam_pad_flag' if (m.cam_flag & 0xFF) != 0 else 'cam_pad_mode31')
        m.push(5, [m.pad_tag])
        left = (c['ll_ret'] & 0xFF) != 0
        m.push(6, [m.pad_tag])
        right = (c['lr_ret'] & 0xFF) != 0
    else:
        left = ll_cam != 0
        right = lr_cam != 0
        m.lab('cam_bytes')
    want = fire if has_ride else (left or right)
    m.lab('left' if left else 'no_left')
    m.lab('right' if right else 'no_right')
    m.lab('want' if want else 'not_want')
    if want and s32(m.w['total']) <= 0:
        m.lab('want_but_no_ammo')
    if want and s32(m.w['total']) > 0:
        if left:
            aid = id_l
            m.lab('anim_left')
        elif right:
            aid = id_r
            m.lab('anim_right')
        elif has_ride:
            aid = id_f
            m.lab('anim_fixed')
        else:
            aid = None
        a = None
        if aid is not None:
            sel = c['get_sel'][min(m.get_calls, 2)]
            key = sel - 1 if sel else None
            m.push(8, [m.clump, aid, m.get_calls], ret=('assoc', key) if key is not None else None)
            m.get_calls += 1
            a = key
            m.lab('get_found' if a is not None else 'get_null')
            if a is not None and (m.assoc[a]['delta'] & 0x7FFFFFFF) > 0x7F800000:
                m.lab('delta_nan')
            if a is not None and m.assoc[a]['delta'] == 0x80000000:
                m.lab('delta_negative_zero')
            if a is None or lt(m.assoc[a]['delta'], 0):
                m.lab('blend_called' if a is not None else 'blend_called_no_assoc')
                bsel = c['blend_sel']
                bkey = bsel - 1 if bsel else None
                m.push(9, [m.clump, group, aid, 0x41800000], ret=('assoc', bkey) if bkey is not None else None)
                a = bkey
            if a is not None:
                if m.assoc[a]['flags'] & 1:
                    m.lab('exit_playing_flag')
                    return
                if (m.assoc[a]['amount'] & 0x7FFFFFFF) > 0x7F800000:
                    m.lab('amount_nan')
                if m.assoc[a]['amount'] == K099:
                    m.lab('amount_exactly_099')
                if not gt(m.assoc[a]['amount'], K099):
                    m.lab('exit_amount')
                    return
            else:
                m.lab('fire_path_no_assoc')
        if not fire:
            m.lab('exit_not_fire')
            return
        if (m.timer & M32) <= (m.w['next'] & M32):
            m.lab('exit_time')
            if (m.timer & M32) == (m.w['next'] & M32):
                m.lab('exit_time_equal')
            return
        m.lab('fired')
        if s32(m.timer) < s32(m.w['next']):
            m.lab('fired_signed_compare_differs')
        m.push(10, [m.weapon_tag, m.veh_tag, 1 if left else 0, 1 if right else 0])
        m.w['next'] = (m.timer + 0x46) & M32
        m.push(11, [m.ped_tag, 0xFA, 0])
        return
    # sin disparo: recarga del cargador y retirada de las animaciones
    m.lab('nofire_path')
    if m.w['total'] == 0:
        m.lab('nofire_total_zero')
    if m.w['total'] != 0:
        obj = m.second_info
        m.push(3, [m.w['type'], 1, m.info_calls], ret=('info', obj))
        m.info_calls += 1
        clip = s16(m.info[obj]['clip']) & M32
        total = m.w['total']
        m.w['inclip'] = clip if total >= clip else total
        m.lab('reload_clip' if total >= clip else 'reload_total')
        if clip & 0x80000000:
            m.lab('reload_negative_clip')
        if s32(total) < 0:
            m.lab('reload_total_high_bit')
    last = None
    ids = [id_l, id_r] + ([id_f] if has_ride else [])
    m.lab('cleanup_ride' if has_ride else 'cleanup_noride')
    for aid in ids:
        sel = c['get_sel'][min(m.get_calls, 2)]
        key = sel - 1 if sel else None
        m.push(8, [m.clump, aid, m.get_calls], ret=('assoc', key) if key is not None else None)
        m.get_calls += 1
        last = key
        m.lab('cleanup_found' if key is not None else 'cleanup_null')
        if key is not None:
            m.assoc[key]['delta'] = MINUS8
    if last is not None:
        m.assoc[last]['delta'] = MINUS8


def changes(c, m):
    """bytes que cambian por arena: [[desplazamiento respecto al objeto, byte nuevo]] en orden creciente"""
    ini = Model(c)
    out = [[] for _ in range(12)]

    def diff(arena, off, size, old, new):
        for b in range(size):
            o = (old >> (8 * b)) & 0xFF
            n = (new >> (8 * b)) & 0xFF
            if o != n:
                out[arena].append([off + b, n])
    # el puntero al conductor (+0x460) es una direccion real de la arena: sus bytes no se modelan (check_row los omite; EXE y DLL se comparan entre si)
    diff(T_THIS, 0x594, 4, ini.vsub, m.vsub)
    diff(T_THIS, 0x429, 1, ini.vflags, m.vflags)
    diff(T_DRV, 0x18, 4, ini.clump, m.clump)
    diff(T_DRV, 0x718, 1, ini.slot, m.slot)
    for name, off in (('type', 0), ('state', 4), ('inclip', 8), ('total', 0xC), ('next', 0x10)):
        diff(T_DRV, ini.weapon_off + off, 4, ini.w[name], m.w[name])
    diff(T_PINFO, 0x153, 1, ini.can, m.can)
    for idx, key in ((T_INFO_A, 'A'), (T_INFO_B, 'B')):
        diff(idx, 0x14, 4, ini.info[key]['slot'], m.info[key]['slot'])
        diff(idx, 0x20, 2, ini.info[key]['clip'], m.info[key]['clip'])
    for idx, key in ((T_RIDE_A, 'A'), (T_RIDE_B, 'B')):
        diff(idx, 0, 4, ini.ride[key], m.ride[key])
    for i in range(4):
        diff(T_ASSOC0 + i, 0x18, 4, ini.assoc[i]['amount'], m.assoc[i]['amount'])
        diff(T_ASSOC0 + i, 0x1C, 4, ini.assoc[i]['delta'], m.assoc[i]['delta'])
        diff(T_ASSOC0 + i, 0x2E, 2, ini.assoc[i]['flags'], m.assoc[i]['flags'])
    for a in out:
        a.sort()
    return out


def expected(c):
    m = model(c)
    if m.muts:
        m.lab('mutations')
    cam = [m.cam_active, m.cam_flag]
    for i in range(3):
        cam += [m.cam_mode[i], m.cam_ll[i], m.cam_lr[i]]
    return m, dict(events=m.events, ev_hash=m.hash, nev=m.nev, timer=m.timer, cam=cam)


def check_row(line):
    return check_obj(json.loads(line))


def check_obj(row):
    if row.get('event') != 'comparison':
        return None
    c = row['input']
    msgs = []
    m, e = expected(c)
    want_changes = changes(c, m)
    for side in ('original', 'candidate'):
        o = row[side]['observed']
        bad = [k for k in ('events', 'ev_hash', 'nev', 'timer', 'cam') if o[k] != e[k]]
        got = o['changes']
        # la ranura del conductor del vehiculo (+0x460) y sus bytes se omiten: el banco guarda la direccion real
        got_this = [x for x in got[T_THIS] if not (0x460 <= x[0] < 0x464)]
        got_cmp = [got_this] + got[1:]
        if bad or got_cmp != want_changes or o['unexpected_virtual']:
            msgs.append('DIFIERE %s caso %s campos %s changes %s' % (side, row['case'], bad, got_cmp != want_changes))
    return row['case'], msgs


def main(path):
    from concurrent.futures import ProcessPoolExecutor
    bad = 0
    n = 0
    with open(path, encoding='utf-8') as f:
        lines = [l for l in f if '"event":"comparison"' in l]
    with ProcessPoolExecutor() as ex:
        for res in ex.map(check_row, lines, chunksize=256):
            if res is None:
                continue
            n += 1
            if res[1]:
                bad += 1
                if bad <= 20:
                    print(res[0], res[1])
    print('casos revisados', n, 'lados distintos del modelo', bad)
    return 1 if bad else 0


def check_self():
    assert lt(0xBF800000, 0) and not lt(0x7FC00000, 0) and not lt(0, 0) and not lt(0x80000000, 0)
    assert gt(0x3F800000, K099) and not gt(K099, K099) and not gt(0x7FC00000, K099)
    assert s8(0x80) == -128 and s16(0x8000) == -32768
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) == 1:
        check_self()
    else:
        sys.exit(main(sys.argv[1]))
