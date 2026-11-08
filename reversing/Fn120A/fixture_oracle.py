"""Oraculo independiente (Python) de Fn120A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de los cinco metodos virtuales del lote 120A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables (leidas del PE; cada direccion aparece una sola vez en una vtable):
  0x4E6080  C2 14 00           ret 14h                 vtable 0x85F330 ranura 1 (CAEPedlessSpeechAudioEntity::AddScriptSayEvent: cinco argumentos de pila, el callee los limpia; sin efectos)
  0x661710  B8 C3 04 00 00 C3  mov eax,4C3h; ret       vtable 0x86FB5C ranura 4 (CTaskComplexGangJoinRespond::GetTaskType = 1219)
  0x620EE0  B8 E8 03 00 00 C3  mov eax,3E8h; ret       vtable 0x86D894 ranura 4 (CTaskComplexKillPedOnFoot::GetTaskType = 1000)
  0x681F10  B8 00 01 00 00 C3  mov eax,100h; ret       vtable 0x870664 ranura 4 (CTaskComplexPartner::GetTaskType = 256)
  0x681DF0  B8 B2 04 00 00 C3  mov eax,4B2h; ret       vtable 0x870754 ranura 4 (CTaskComplexPartnerDeal::GetTaskType = 1202)
Ninguna lee ni escribe memoria, ni lee `this` (ECX), ni llama a nada: no hay eventos, ni bytes cambiados en las arenas, y el valor devuelto es la constante (en la primera, ninguno: EAX no se compara).
El caso se ejecuta por la entrada del EXE (a[3] = 0) o por la ranura de la vtable (a[3] = 1).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
TASK_TYPE = {1: 1219, 2: 1000, 3: 256, 4: 1202}
KIND = ('arena', 'null', 'raw')


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = ['this_' + KIND[a[0]], 'via_vtable' if a[3] else 'via_entry', 'fill%d' % a[4]]
    if a[0] == 0 and a[2] % 4:
        marks.append('this_unaligned')
    if a[5]:
        marks.append('vt_set')
    if fn == 0:
        bools = [x & 0xFF for x in a[8:11]]
        if all(bools):
            marks.append('flags_all')
        if not any(bools):
            marks.append('flags_none')
        if a[6] == 0:
            marks.append('evt0')
        if a[6] >= 0x80000000 or a[7] >= 0x80000000:
            marks.append('evt_big')
        ret = None
    else:
        ret = TASK_TYPE[fn]
    return {'events': [], 'marks': marks, 'changes': [], 'ret': ret}


def fp_stack_valid(hexstr):
    """posiciones validas de una imagen fnsave normalizada (la pila x87 debe quedar vacia)"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return valid


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
    if 'ptrs' in e and o.get('ptrs') != e['ptrs']:
        return 'ptrs %s != %s' % (o.get('ptrs'), e['ptrs'])
    valid = fp_stack_valid(side_row['post_floating_state'])
    if valid:
        return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
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
