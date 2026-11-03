"""Oraculo independiente (Python) de Fn85I: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE (0x623B10..0x624082, tabla de saltos 0x624084 y de indices 0x624098),
lo que debe observarse de CTaskSimpleFight::StartAnim (thiscall, ret 8; no devuelve nada que lean los llamadores) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Modelo (desensamblado):
  ataque (dword de pila) negativo: solo +0x26 = 0.
  si hay animacion (+0x1C): SetDeleteCallback(anim, 0x4D1390, 0) y +0x1C = 0. bl = +0x26 (con signo, releido tras la llamada); fuera de 0..0x12 solo el cierre.
  tabla de indices 0x624098: 0 -> caso 0; 1, 15..18 -> caso 1; 2 -> caso 2; 3..10 -> caso 3; 11..14 -> caso 4.
  cierre: +0x27 = +0x26 (releido), +0x26 = 0.
  caso 0: +0x24 = 0, +0x25 = 0, +0x14 = 0; si [ped+0x534] >= 4 (con signo) y IsPlayer: +8 = 1 y cierre. Combo disponible = GetAvailableComboSet(ped, 0) en linea (GetWeaponInfo(tipo, 1) ...,
          ver Fn85E); +0x24 = combo; sin animacion de reposo (+0x20): BlendAnimation(clump, grupo de la fila, 0xDF, 8.0), +0x20 = resultado y SetDeleteCallback(resultado, 0x61DAE0, tarea); con ella: si su
          peso (+0x18) < 1.0 y su retardo (+0x1C) <= 0.0 (ordenadas) BlendAnimation de nuevo y +0x20 = resultado (sin callback). IsPlayer: datos (+0x480, RELEIDO) con +0xC = +0x10 = 0 y FindTargetOnGround(tarea, ped).
          +0x24 = 0 y cierre.
  caso 1: datos de jugador (+0x480) != 0: +0xC = +0x10 = 0, FindTargetOnGround, y con 0x11 / 0x10 +0x14 = 2.0 / 1.0 de los datos RELEIDOS; sin datos: SetMoveState(6 con 0x11, 4 con 0x10, 1 en otro caso)
          y +0x538 = +0x534. Despues BlendAnimation(clump, grupo del ped +0x4D4, 1, 8.0) con 0x11, (grupo, 0, 8.0) con 0x10, (grupo, 3, 4.0) con 0xF; con 0x12 y bit 26 de +0x46C: GetTaskDuck(inteligencia, 1)
          y, si devuelve algo, (0, 0x37, 4.0); en otro caso (grupo, 3, 2.0). Con animacion de reposo (+0x20, releida) se apaga el bit 0 de sus banderas (+0x2E); sin ella +8 = 1. +0x26 = 0x10 y cierre.
  caso 2: +0x25 = 0, +0x14 = 0; si la fila del combo (+0x24, con signo, menos 4) no tiene el bit 1 de +0x85: +0x24 = 4. BlendAnimation(clump, grupo de la fila del combo, 0xDE, 8.0), +0x1C = resultado y
          SetFinishCallback(resultado, 0x61DAE0, tarea).
  caso 3: IsPlayer: si es jugador cierre. +0x14 = 0, +0x24 = 1; al = +0x26 RELEIDO: 7 -> movimiento 0, anim 0x2D; 9 -> 2, 0x2E; 8 -> 1; 10 -> 2; otro -> al - 3 (byte); salvo 7 y 9 la anim es movimiento (con signo) + 0x29.
          BlendAnimation(clump, 0, anim, 8.0), +0x1C = resultado; con +0x26 (releido) == 3: SetDeleteCallback(+0x1C, 0x61DAE0, tarea) y banderas (+0x1C releido) |= 4; si no, banderas &= 0xFD, &= 0xDF, |= 8
          y SetFinishCallback(+0x1C, 0x61DAE0, tarea).
  caso 4: +0x25 = byte bajo del ataque, +0x14 = 1; BlendAnimation(clump, grupo de la fila del combo, movimiento (con signo) + 0xD6, 8.0), +0x1C = resultado y SetFinishCallback(resultado, 0x61DAE0, tarea). Con
          +0x25 == 3 y el tiempo (+0x20) de la animacion (+0x1C, RELEIDA) distinto de 0.0 (NaN cuenta): SetCurrentTime(anim, +0x44 de la fila del combo RELEIDO). Con datos de jugador (+0x480): GetFatAndMuscleModifier(3)
          (silencia un NaN senalizante: fld/fstp), +0x24 de la animacion RELEIDA = resultado y, con +0x25 < 4 (con signo), SetMoveState(1) (el falso escribe +0x534).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

GUARD = 0x40
M32 = 0xFFFFFFFF
NULL_IDX = ((0 - 0xB5D4A0) >> 5) & M32   # sub ecx,0B5D4A0h ; sar ecx,5 con puntero nulo
T_TASK, T_PED, T_PDA, T_PDB, T_OLD, T_ALT, T_IDLE, T_NEW, T_INFO, T_NAME, T_GROUPS, T_INT = range(12)
MELEE = 0x61DAE0
DEFAULT_CB = 0x4D1390


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def f32(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def quiet(bits):
    """fld + fstp de un float: un NaN senalizante se silencia"""
    bits &= M32
    if (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF):
        bits |= 0x400000
    return bits


def tag(arena, off=0):
    return 0x10000000 + (arena << 20) + GUARD + off


def arena_of(t):
    return (t - 0x10000000) >> 20


class Mem:
    """memoria rastreada: solo los campos que la funcion puede escribir (con su valor inicial conocido)"""

    def __init__(self):
        self.init = {}
        self.cur = {}

    def set8(self, a, o, v):
        self.init[(a, o)] = v & 0xFF
        self.cur[(a, o)] = v & 0xFF

    def set32(self, a, o, v):
        for j in range(4):
            self.set8(a, o + j, (v >> (8 * j)) & 0xFF)

    def get8(self, a, o):
        return self.cur[(a, o)]

    def get32(self, a, o):
        return sum(self.cur[(a, o + j)] << (8 * j) for j in range(4))

    def put8(self, a, o, v):
        if (a, o) not in self.cur:
            raise ValueError('escritura en un campo sin valor inicial: %s' % ((a, o),))
        self.cur[(a, o)] = v & 0xFF

    def put32(self, a, o, v):
        for j in range(4):
            self.put8(a, o + j, (v >> (8 * j)) & 0xFF)

    def changes(self):
        out = []
        for (a, o) in sorted(self.cur):
            if self.cur[(a, o)] == self.init[(a, o)]:
                continue
            if a == T_TASK and 0x1C <= o < 0x24:
                continue
            if a == T_PED and 0x480 <= o < 0x484:
                continue
            out.append([a, o, self.cur[(a, o)]])
        return out


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    M = Mem()
    M.set8(T_TASK, 0x8, c['finished']); M.set8(T_TASK, 0xA, c['ref']); M.set32(T_TASK, 0xC, c['cur']); M.set8(T_TASK, 0x14, c['cont'])
    M.set8(T_TASK, 0x24, c['combo']); M.set8(T_TASK, 0x25, c['move']); M.set8(T_TASK, 0x26, c['cmd']); M.set8(T_TASK, 0x27, c['last'])
    M.set32(T_PED, 0x534, c['mstate']); M.set32(T_PED, 0x538, c['mswim'])
    for a, k in ((T_PDA, 'pda'), (T_PDB, 'pdb')):
        for i in range(3):
            M.set32(a, 0xC + 4 * i, c[k][i])
    for i in range(4):
        M.set32(T_OLD + i, 0x24, c['aspd'][i])
        M.set8(T_OLD + i, 0x2E, (c['afl'][i] >> 16) & 0xFF)
    st = {'anim': tag(T_OLD) if c['hasanim'] else 0, 'idle': tag(T_IDLE) if c['hasidle'] else 0, 'pd': tag(T_PDA) if c['haspd'] else 0}
    done = set()
    calls = {'player': 0}
    rows = [[c['rowgroup'][i] & M32, c['rowflags'][i] & 0xFF, c['rowloop'][i] & M32] for i in range(12)]
    mutsite = c['mutsite']
    mutb = c['mutb'] & 0xFF

    def mut(site):
        if mutsite != site or site in done:
            return False
        done.add(site)
        mark('mut%d' % site)
        return True

    # ---- falsos
    def delete_cb(self_tag, cb, data):
        ev.append([1, self_tag, cb, data])
        if cb == MELEE:
            if mut(2):
                st['anim'] = tag(T_ALT)
            if mut(3):
                M.put8(T_TASK, 0x24, mutb)

    def finish_cb(self_tag, cb, data):
        ev.append([2, self_tag, cb, data])
        if cb == MELEE:
            if mut(2):
                st['anim'] = tag(T_ALT)
            if mut(3):
                M.put8(T_TASK, 0x24, mutb)

    def blend(group, aid, delta):
        ev.append([4, c['clump'] & M32, group & M32, aid & M32, delta])
        if mut(1):
            M.put8(T_TASK, 0x26, mutb)
        return tag(T_NEW)

    def player():
        ev.append([5, tag(T_PED)])
        calls['player'] += 1
        if calls['player'] == 1:
            if mut(7):
                M.put8(T_TASK, 0x26, mutb)
            if mut(5):
                st['pd'] = tag(T_PDB)
        return (c['isplayer'] & 0xFF) != 0

    def find():
        ev.append([14, tag(T_TASK), tag(T_PED)])
        if mut(4):
            st['pd'] = tag(T_PDB)

    def move_state(state):
        ev.append([6, tag(T_PED), state])
        M.put32(T_PED, 0x534, state)

    def epilogue():
        M.put8(T_TASK, 0x27, M.get8(T_TASK, 0x26))
        M.put8(T_TASK, 0x26, 0)

    def result():
        return {'anim': st['anim'], 'idle': st['idle'], 'pd': st['pd'], 'changes': M.changes(), 'events': ev, 'marks': marks}

    def row_of_combo():
        r = s8(M.get8(T_TASK, 0x24)) - 4
        if not 0 <= r < 12:
            raise ValueError('combo fuera de las filas fixturadas: %d' % (r + 4))
        return rows[r]

    # ---- GetAvailableComboSet(ped, 0) en linea
    def gacs():
        ev.append([8, c['wtype'] & M32, 1])
        style = s8(c['binfo'])
        if style == 4:
            style = s8(c['fstyle'])
        r = style - 4
        if not 0 <= r < 12:
            raise ValueError('estilo fuera de las filas fixturadas: %d' % style)
        if not rows[r][1] & 4:
            mark('gacs_ret4_flag')
            return 4
        wanted = rows[r][0]
        if wanted == 0x21:
            mark('gacs_group21')
            return style
        cur = M.get32(T_TASK, 0xC)
        if wanted == cur:
            if M.get8(T_TASK, 0xA) != 0:
                mark('gacs_same_ref')
                return style
            mark('gacs_same_noref')
        else:
            if M.get8(T_TASK, 0xA) != 0:
                k = c['gblk'][cur]
                idx = (k - 1) if k else NULL_IDX
                if not k:
                    mark('gacs_remove_null_block')
                ev.append([12, idx])
                M.put8(T_TASK, 0xA, 0)
                mark('gacs_diff_ref')
            else:
                mark('gacs_diff_noref')
            M.put32(T_TASK, 0xC, wanted)
        group = M.get32(T_TASK, 0xC)
        if c['gblk'][group]:
            k = c['gblk'][group] - 1
            mark('gacs_cached')
        else:
            ev.append([9, group])
            ev.append([10, tag(T_NAME, (group & 0x3F) * 4)])
            k = c['lookup']
            mark('gacs_lookup')
        if (c['loaded'][k] & 0xFF) != 0:
            ev.append([11, k])
            M.put8(T_TASK, 0xA, 1)
            mark('gacs_add_ref')
            return style
        ev.append([13, k + 0x63E7, 8])
        mark('gacs_request')
        nxt = M.get8(T_TASK, 0x26)
        if 0xB <= s8(nxt) <= 0xE:
            M.put8(T_TASK, 0x26, 0xB)
            mark('gacs_next_lowered')
        else:
            mark('gacs_next_untouched')
        return 4

    # ---- cuerpo
    attack = c['attack'] & M32
    if attack & 0x80000000:
        mark('neg_attack')
        M.put8(T_TASK, 0x26, 0)
        return result()
    if st['anim']:
        mark('old_anim_released')
        delete_cb(st['anim'], DEFAULT_CB, 0)
        st['anim'] = 0
    else:
        mark('no_old_anim')
    cmd = s8(M.get8(T_TASK, 0x26))
    if cmd < 0 or cmd > 0x12:
        mark('cmd_oob_neg' if cmd < 0 else 'cmd_oob_pos')
        epilogue()
        return result()
    if cmd == 0:
        mark('st0')
        M.put8(T_TASK, 0x24, 0); M.put8(T_TASK, 0x25, 0); M.put8(T_TASK, 0x14, 0)
        if s32(M.get32(T_PED, 0x534)) >= 4:
            mark('st0_mstate_hi')
            if player():
                mark('st0_finish')
                M.put8(T_TASK, 0x8, 1)
                epilogue()
                return result()
            mark('st0_mstate_hi_notplayer')
        combo = gacs()
        M.put8(T_TASK, 0x24, combo)
        group = rows[s8(combo) - 4][0]
        if not st['idle']:
            mark('st0_idle_new')
            st['idle'] = blend(group, 0xDF, 0x41000000)
            delete_cb(st['idle'], MELEE, tag(T_TASK))
        else:
            amt = f32(c['aamt'][2])
            dlt = f32(c['adlt'][2])
            if amt < 1.0 and dlt <= 0.0:
                mark('st0_idle_redo')
                st['idle'] = blend(group, 0xDF, 0x41000000)
            elif amt < 1.0:
                mark('st0_idle_keep_dlt')
            else:
                mark('st0_idle_keep_amt')
        if player():
            mark('st0_player')
            pd = arena_of(st['pd'])
            M.put32(pd, 0xC, 0); M.put32(pd, 0x10, 0)
            find()
        else:
            mark('st0_notplayer')
        M.put8(T_TASK, 0x24, 0)
        epilogue()
        return result()
    if cmd in (1, 15, 16, 17, 18):
        mark('st1')
        if st['pd']:
            mark('st1_pd')
            pd = arena_of(st['pd'])
            M.put32(pd, 0xC, 0); M.put32(pd, 0x10, 0)
            find()
            pd2 = arena_of(st['pd'])
            if cmd == 0x11:
                M.put32(pd2, 0x14, 0x40000000); mark('st1_ratio2')
            elif cmd == 0x10:
                M.put32(pd2, 0x14, 0x3F800000); mark('st1_ratio1')
            else:
                mark('st1_ratio_none')
        else:
            mark('st1_nopd')
            state = 6 if cmd == 0x11 else (4 if cmd == 0x10 else 1)
            mark('st1_ms%d' % state)
            move_state(state)
            M.put32(T_PED, 0x538, M.get32(T_PED, 0x534))
        agroup = c['agroup'] & M32
        if cmd == 0x11:
            mark('st1_blend1'); blend(agroup, 1, 0x41000000)
        elif cmd == 0x10:
            mark('st1_blend0'); blend(agroup, 0, 0x41000000)
        elif cmd == 0xF:
            mark('st1_blendF'); blend(agroup, 3, 0x40800000)
        elif cmd == 0x12 and (c['pflags'] & 0x4000000):
            ev.append([15, tag(T_INT), 1])
            if c['duck']:
                mark('st1_duck'); blend(0, 0x37, 0x40800000)
            else:
                mark('st1_duck_notask'); blend(agroup, 3, 0x40000000)
        else:
            mark('st1_duck_noflag' if cmd == 0x12 else 'st1_blend3')
            blend(agroup, 3, 0x40000000)
        if st['idle']:
            mark('st1_idle')
            a = arena_of(st['idle'])
            M.put8(a, 0x2E, M.get8(a, 0x2E) & 0xFE)
        else:
            mark('st1_noidle')
            M.put8(T_TASK, 0x8, 1)
        M.put8(T_TASK, 0x26, 0x10)
        epilogue()
        return result()
    if cmd == 2:
        mark('st2')
        M.put8(T_TASK, 0x25, 0); M.put8(T_TASK, 0x14, 0)
        if not row_of_combo()[1] & 2:
            mark('st2_flag2_clear')
            M.put8(T_TASK, 0x24, 4)
        else:
            mark('st2_flag2_set')
        st['anim'] = blend(row_of_combo()[0], 0xDE, 0x41000000)
        finish_cb(st['anim'], MELEE, tag(T_TASK))
        epilogue()
        return result()
    if 3 <= cmd <= 10:
        mark('st3')
        if player():
            mark('st3_player')
            epilogue()
            return result()
        mark('st3_notplayer')
        M.put8(T_TASK, 0x14, 0)
        al = M.get8(T_TASK, 0x26)
        M.put8(T_TASK, 0x24, 1)
        if al == 7:
            M.put8(T_TASK, 0x25, 0); aid = 0x2D
        elif al == 9:
            M.put8(T_TASK, 0x25, 2); aid = 0x2E
        else:
            if al == 8:
                M.put8(T_TASK, 0x25, 1)
            elif al == 0xA:
                M.put8(T_TASK, 0x25, 2)
            else:
                M.put8(T_TASK, 0x25, (al - 3) & 0xFF)
            aid = (s8(M.get8(T_TASK, 0x25)) + 0x29) & M32
        mark('st3_cmd%d' % cmd)
        st['anim'] = blend(0, aid, 0x41000000)
        if M.get8(T_TASK, 0x26) == 3:
            mark('st3_delete')
            delete_cb(st['anim'], MELEE, tag(T_TASK))
            a = arena_of(st['anim'])
            M.put8(a, 0x2E, M.get8(a, 0x2E) | 4)
        else:
            mark('st3_finish')
            a = arena_of(st['anim'])
            M.put8(a, 0x2E, M.get8(a, 0x2E) & 0xFD)
            M.put8(a, 0x2E, M.get8(a, 0x2E) & 0xDF)
            M.put8(a, 0x2E, M.get8(a, 0x2E) | 8)
            finish_cb(st['anim'], MELEE, tag(T_TASK))
        epilogue()
        return result()
    # 11..14
    mark('st4')
    group = row_of_combo()[0]
    move = s8(attack & 0xFF)
    M.put8(T_TASK, 0x25, attack & 0xFF); M.put8(T_TASK, 0x14, 1)
    st['anim'] = blend(group, (move + 0xD6) & M32, 0x41000000)
    finish_cb(st['anim'], MELEE, tag(T_TASK))
    if M.get8(T_TASK, 0x25) == 3:
        mark('st4_move3')
        ai = arena_of(st['anim']) - T_OLD
        t = f32(c['atime'][ai])
        if t != 0.0:
            mark('st4_move3_timenan' if t != t else 'st4_move3_timenz')
            ev.append([3, st['anim'], row_of_combo()[2]])
        else:
            mark('st4_move3_time0')
    if st['pd']:
        mark('st4_pd')
        ev.append([7, 3])
        if mut(6):
            st['anim'] = tag(T_ALT)
        ai = arena_of(st['anim'])
        stat = quiet(c['stat'])
        if stat != (c['stat'] & M32):
            mark('st4_stat_snan')
        M.put32(ai, 0x24, stat)
        if s8(M.get8(T_TASK, 0x25)) < 4:
            mark('st4_move_lt4')
            move_state(1)
        else:
            mark('st4_move_ge4')
    else:
        mark('st4_nopd')
    epilogue()
    return result()


CHANGE_KEYS = ('anim', 'idle', 'pd', 'changes')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if k in e and o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e)
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
