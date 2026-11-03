"""Oraculo independiente (Python) de Fn87A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE 0x60D850..0x60DC41, lo que debe observarse de
CPlayerPed::ProcessWeaponSwitch (thiscall, un argumento: el CPad*; ret 4; void) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Modelo (desensamblado; esi = jugador, ranura activa = byte +0x718, ranura elegida = byte +0x20 de los datos del jugador +0x480, armas = 0x1C bytes desde +0x5A0 con tipo en +0 y municion en +0xC;
todos los indices con signo, sin comprobar el rango):
  0x60D850: FrenzyOnGoing (0x43D1F0) distinto de cero, o m_pAttachedTo (+0xFC) distinto de cero, o GetTaskJetPack (0x601110, ecx = inteligencia +0x47C) distinto de cero: salta a 0x60DBBC (comprobacion final).
  Si no: sin objetivo (+0x71C), con el dword +0x34 de los datos del jugador sin los bits 0x8 y 0x800 y con el byte +0x85 a cero: CPad::CycleWeaponRightJustDown (0x540640): con el modo de la camara (word 0xB6F858)
  fuera de {0x22,0x2A,7,0x27,8,0x28,0x33,0x34,0x2E}: elegida = activa+1 (byte); si es < 0xD (con signo) recorre: arma con tipo != 0, HasWeaponAmmoToBeUsed (0x73B2A0) y [IsCoopGameGoingOn (0x441390) falso o
  CanBeUsedFor2Player (0x73DEF0)] -> la deja; si no, elegida+1 y, al llegar a >= 0xD, elegida = 0 y sale; si ya era >= 0xD, elegida = 0.  Sin boton derecho: CycleWeaponLeftJustDown (0x540610) con el modo fuera de
  {0x22,7,8,0x33,0x2E}: elegida = activa-1; si es < 0, 0xC; recorre: elegida == 0 -> sale; arma con tipo != 0 y municion y (no cooperativo o utilizable) -> la deja; si no, elegida-1 y, si queda < 0, 0xC.
  0x60D9BE: GetWeaponInfo (0x743C60) del tipo del arma activa con habilidad 1; si su primer dword es 0 -> comprobacion final. CPad::GetWeapon (0x540180, pad en ecx, jugador de argumento): distinto de cero con tipo 0x26
  en el arma activa -> comprobacion final. Municion del arma activa (con signo) > 0 -> comprobacion final. Modo de camara en {0x22,7,8,0x33} -> sale sin comprobacion final. Si el tipo del arma activa es 0x28
  y el tipo de la ranura 8 es 0x27: elegida = 8; si no, elegida = activa-1. Elegida < 0 -> elegida = 0. Si no recorre: ranura 5 con tipo 5 en la ranura 5 -> la deja; municion (con signo) > 0 y ranura distinta de
  0x10/0x11/0x12 -> la deja; si no, elegida-1 y, si queda < 0 (con signo), 0 y sale.
  0x60DBBC: elegida == activa (bytes) -> sale. GetTaskUseGun (0x600F70) distinto de cero: otra llamada, byte +0x10 de la tarea 2 o 3 -> sale; otra llamada y byte +0x10 == 4 con el dword +0x2C distinto de cero -> sale.
  RemoveWeaponAnims (0x5F0250, jugador; ranura activa con signo, -1000.0f). Elegida == -1 -> sale. MakeChangesForNewWeapon (0x60B460, jugador; tipo del arma de la ranura elegida).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
M32 = 0xFFFFFFFF
A_PED, A_PD, A_INTEL, A_PAD, A_TASK, A_INFO = 0, 1, 2, 3, 4, 5


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def s8(x):
    x &= 0xFF
    return x - 256 if x >= 128 else x


def s32(x):
    x &= M32
    return x - (1 << 32) if x >= (1 << 31) else x


def expected(c):
    marks = []
    mark = marks.append
    ev = []

    def E(*a):
        ev.append(list(a))

    wt = [v & M32 for v in c['wtype']]
    wa = [v & M32 for v in c['wammo']]
    active = c['slot'] & 0xFF
    wa[13] = active | ((c['slothi'] & 0xFFFFFF) << 8)          # la municion de la ranura 13 es el dword +0x718
    chosen0 = c['chosen0'] & 0xFF
    st = {'chosen': chosen0}
    cam = c['cam'] & 0xFFFF
    pflags = c['pflags'] & M32

    def has(i):
        return (c['hasmask'] >> i) & 1 if 0 <= i < 32 else 0

    def can2(i):
        return (c['can2mask'] >> i) & 1 if 0 <= i < 32 else 0

    def wtag(i):
        return tag(A_PED, 0x5A0 + i * 0x1C)

    def pickable(i):
        """tipo != 0, HasWeaponAmmoToBeUsed y (no cooperativo o CanBeUsedFor2Player)"""
        if wt[i] == 0:
            return False
        E(5, wtag(i))
        if not has(i):
            return False
        E(6)
        if not c['coop']:
            return True
        E(7, wtag(i))
        return bool(can2(i))

    def result():
        changes = []
        if st['chosen'] != chosen0:
            changes.append([A_PD, 0x20, st['chosen']])
        return {'changes': changes, 'events': ev, 'cam': cam, 'marks': marks}

    skip = False                                   # -> 0x60DBBC
    E(1)
    if c['frenzy']:
        mark('frenzy')
        skip = True
    elif c['attached']:
        mark('attached')
        skip = True
    else:
        E(2, tag(A_INTEL, 0))
        if c['jet']:
            mark('jet')
            skip = True
    if not skip:
        if c['targeted']:
            mark('targeted')
        elif pflags & 8:
            mark('freeaim')
        elif c['dontallow'] & 0xFF:
            mark('dontallow')
        elif pflags & 0x800:
            mark('noswitch')
        else:
            mark('cycle_open')
            E(3, tag(A_PAD, 0))
            if c['right']:
                if cam in (0x22, 0x2A, 7, 0x27, 8, 0x28, 0x33, 0x34, 0x2E):
                    mark('right_cam_blocked')
                else:
                    mark('right')
                    st['chosen'] = (active + 1) & 0xFF
                    if s8(st['chosen']) < 0xD:
                        while True:
                            i = s8(st['chosen'])
                            if pickable(i):
                                mark('right_found')
                                break
                            st['chosen'] = (st['chosen'] + 1) & 0xFF
                            if not s8(st['chosen']) < 0xD:
                                mark('right_wrap')
                                st['chosen'] = 0
                                break
                    else:
                        mark('right_high')
                        st['chosen'] = 0
            else:
                E(4, tag(A_PAD, 0))
                if c['left']:
                    if cam in (0x22, 7, 8, 0x33, 0x2E):
                        mark('left_cam_blocked')
                    else:
                        mark('left')
                        st['chosen'] = (active - 1) & 0xFF
                        if s8(st['chosen']) < 0:
                            mark('left_neg')
                            st['chosen'] = 0xC
                        while True:
                            i = s8(st['chosen'])
                            if i == 0:
                                mark('left_zero')
                                break
                            if pickable(i):
                                mark('left_found')
                                break
                            st['chosen'] = (st['chosen'] - 1) & 0xFF
                            if s8(st['chosen']) < 0:
                                mark('left_wrap')
                                st['chosen'] = 0xC
                else:
                    mark('no_cycle')
        # 0x60D9BE
        a = s8(active)
        t = wt[a]
        E(8, t, 1)
        fire = c['infoval'] & M32 if (c['infomask'] >> (t & 31)) & 1 else 0
        if fire == 0:
            mark('melee')
        else:
            E(9, tag(A_PAD, 0), tag(A_PED, 0))
            if (c['padw'] & M32) != 0 and t == 0x26:
                mark('padw_minigun')
            elif s32(wa[a]) > 0:
                mark('has_ammo')
            elif cam in (0x22, 7, 8, 0x33):
                mark('cam_exit')
                return result_exit(st, chosen0, ev, cam, marks)
            else:
                mark('no_ammo')
                if t == 0x28 and wt[8] == 0x27:
                    mark('detonator')
                    st['chosen'] = 8
                else:
                    mark('prev_slot')
                    st['chosen'] = (active - 1) & 0xFF
                if s8(st['chosen']) < 0:
                    mark('prev_neg')
                    st['chosen'] = 0
                else:
                    while True:
                        i = s8(st['chosen'])
                        if i == 5 and wt[5] == 5:
                            mark('keep_bat')
                            break
                        if s32(wa[i]) > 0 and i not in (0x10, 0x11, 0x12):
                            mark('keep_ammo')
                            break
                        if s32(wa[i]) > 0:
                            mark('skip_1x')
                        st['chosen'] = (st['chosen'] - 1) & 0xFF
                        if s8(st['chosen']) < 0:
                            mark('prev_wrap')
                            st['chosen'] = 0
                            break
    # 0x60DBBC
    if s8(st['chosen']) == s8(active):
        mark('same')
        return result()
    mark('differ')
    E(10, tag(A_INTEL, 0))
    if c['ug']:
        mark('ug')
        E(10, tag(A_INTEL, 0))
        cmd = c['ugcmd'] & 0xFF
        if cmd in (2, 3):
            mark('ug_fire')
            return result()
        E(10, tag(A_INTEL, 0))
        if cmd == 4 and (c['uganim'] & M32):
            mark('ug_reload_anim')
            return result()
        if cmd == 4:
            mark('ug_reload_noanim')
        else:
            mark('ug_other')
    else:
        mark('noug')
    E(11, tag(A_PED, 0), s8(active) & M32, 0xC47A0000)
    ci = s8(st['chosen'])
    if ci == -1:
        mark('chosen_m1')
        return result()
    mark('change')
    E(12, tag(A_PED, 0), wt[ci])
    return result()


def result_exit(st, chosen0, ev, cam, marks):
    changes = []
    if st['chosen'] != chosen0:
        changes.append([A_PD, 0x20, st['chosen']])
    return {'changes': changes, 'events': ev, 'cam': cam, 'marks': marks}


CHANGE_KEYS = ('changes', 'cam')


def check_side(c, o, e):
    for k in CHANGE_KEYS:
        if o.get(k) != e[k]:
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
