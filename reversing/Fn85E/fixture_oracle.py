"""Oraculo independiente (Python) de Fn85E: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de
CTaskSimpleFight::GetAvailableComboSet (0x61C7F0, thiscall, ret 8, devuelve AL) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Modelo (desensamblado 0x61C7F0..0x61C99F):
  comando = byte bajo del dword de pila, con signo.
  negativo: si grupo(+0xC) != 0x21 y no hay referencia (+0xA == 0): bloque del grupo (puntero de la tabla; si es nulo GetAnimBlockName + GetAnimationBlock); si IsLoaded != 0
            AddAnimBlockRef(indice) y +0xA = 1. Devuelve 0.
  no negativo y no 0, 2, 0xB..0xE: devuelve 0.
  si no: GetWeaponInfo(tipo del arma de la ranura activa, 1); estilo = byte +0x6E; comando 0xC -> estilo de pelea (+0x72D, que se lee DESPUES de GetWeaponInfo); en otro caso, con estilo 4 y
         comando 0 o 2, estilo de pelea; con comando 0 y sin el bit 2 del byte +0x85 de la fila (estilo - 4) devuelve 4. Fila: grupo (+0) == 0x21 devuelve el estilo; grupo == +0xC: con referencia
         devuelve el estilo; si no: con referencia RemoveAnimBlockRef(indice del bloque del grupo actual, sin comprobar el nulo), +0xA = 0 y +0xC = grupo de la fila (RELEIDO). Despues, con el grupo +0xC:
         bloque (como arriba); cargado (!= 0): AddAnimBlockRef, +0xA = 1 y devuelve el estilo; si no RequestModel(0x63E7 + indice, 8), un comando siguiente (+0x26, con signo) en 0xB..0xE pasa a 0xB
         (RELEIDO tras RequestModel) y devuelve 4.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

GUARD = 0x40
M32 = 0xFFFFFFFF
BLOCKS = 0xB5D4A0
NULL_IDX = ((0 - BLOCKS) >> 5) & M32   # sub ecx,0B5D4A0h ; sar ecx,5 con puntero nulo


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def byte_changes(base, old, new):
    return [[base + j, (new >> (8 * j)) & 0xFF] for j in range(4) if ((old >> (8 * j)) & 0xFF) != ((new >> (8 * j)) & 0xFF)]


def expected(c):
    marks = []
    mark = marks.append
    ev = []
    cmd = s8(c['cmd'] & 0xFF)
    ref = c['ref'] & 0xFF
    cur = c['cur'] & M32
    nxt = c['nextcmd'] & 0xFF
    fstyle = c['fstyle'] & 0xFF
    rows = [[c['rowgroup'][i] & M32, c['rowflags'][i] & 0xFF] for i in range(12)]
    gblk = c['gblk']
    loaded = c['loaded']
    msite = c['mutsite']
    mval = c['mutval']
    done = set()

    def block_of(group):
        """indice del bloque (0..15) del grupo, con los eventos de las funciones que se llaman"""
        if gblk[group]:
            mark('cached_block')
            return gblk[group] - 1
        mark('lookup_block')
        ev.append([2, group])
        ev.append([3, tag(3, (group & 0x3F) * 4)])
        return c['lookup']

    def is_loaded(k):
        v = loaded[k] & 0xFF
        if v > 1:
            mark('loaded_nonbool')
        return v != 0

    def result(ret):
        out = {'ret': ret & 0xFF}
        ch = []
        if (c['ref'] & 0xFF) != ref:
            ch.append([0xA, ref])
        ch += byte_changes(0xC, c['cur'] & M32, cur)
        if (c['nextcmd'] & 0xFF) != nxt:
            ch.append([0x26, nxt])
        out['task_changes'] = ch
        out['ped_changes'] = [[0x72D, fstyle]] if (c['fstyle'] & 0xFF) != fstyle else []
        out['info_changes'] = []
        out['name_changes'] = []
        out['groups_changes'] = []
        out['events'] = ev
        out['marks'] = marks
        mark('ret0' if (ret & 0xFF) == 0 else ('ret4' if (ret & 0xFF) == 4 else 'ret_style'))
        return out

    if cmd < 0:
        mark('neg_cmd')
        group = cur
        if group == 0x21:
            mark('neg_group21')
        elif ref != 0:
            mark('neg_ref')
        else:
            k = block_of(group)
            if is_loaded(k):
                mark('neg_loaded')
                ev.append([4, k])
                ref = 1
            else:
                mark('neg_unloaded')
        return result(0)
    if cmd not in (0, 2) and not (0xB <= cmd <= 0xE):
        mark('invalid_cmd')
        return result(0)
    mark({0: 'cmd0', 2: 'cmd2', 0xB: 'cmd_b', 0xC: 'cmd_c', 0xD: 'cmd_d', 0xE: 'cmd_e'}[cmd])
    ev.append([1, c['wtype'] & M32, 1])
    if msite == 3:
        fstyle = mval & 0xFF
        mark('mut3_applied')
    style = s8(c['binfo'])
    if cmd == 0xC:
        style = s8(fstyle)
        mark('style_from_fstyle')
    else:
        if style == 4 and cmd in (2, 0):
            style = s8(fstyle)
            mark('style_from_fstyle')
        elif style == 4:
            mark('style4_noswitch')
        if cmd == 0:
            r = style - 4
            if not 0 <= r < 12:
                raise ValueError('estilo fuera de las filas fixturadas: %d' % style)
            if not rows[r][1] & 4:
                mark('flag_clear_ret4')
                return result(4)
    row = style - 4
    if not 0 <= row < 12:
        raise ValueError('estilo fuera de las filas fixturadas: %d' % style)
    wanted = rows[row][0]
    if wanted == 0x21:
        mark('group21_ret')
        return result(style)
    if wanted == cur:
        if ref != 0:
            mark('same_group_ref_ret')
            return result(style)
        mark('same_group_noref')
    else:
        mark('diff_group')
        if ref != 0:
            k = gblk[cur]
            idx = (k - 1) if k else NULL_IDX
            if not k:
                mark('remove_null_block')
            ev.append([5, idx])
            mark('remove_ref')
            if msite == 1:
                for r in rows:
                    r[0] = mval & M32
                mark('mut1_applied')
            ref = 0
        cur = rows[row][0]
    group = cur
    k = block_of(group)
    if is_loaded(k):
        mark('add_ref')
        ev.append([4, k])
        ref = 1
        return result(style)
    mark('request_model')
    ev.append([6, k + 0x63E7, 8])
    if msite == 2:
        nxt = mval & 0xFF
        mark('mut2_applied')
    if 0xB <= s8(nxt) <= 0xE:
        nxt = 0xB
        mark('next_lowered')
    else:
        mark('next_untouched')
    return result(4)


CHANGE_KEYS = ('ret', 'task_changes', 'ped_changes', 'info_changes', 'name_changes', 'groups_changes')


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
