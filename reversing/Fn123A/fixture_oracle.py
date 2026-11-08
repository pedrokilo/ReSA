"""Oraculo independiente (Python) de Fn123A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de ~CTaskComplexPartner (0x683A40, lote 123A) y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico), 0x683A40..0x683ACD (141 B), thiscall, `ret`, marco SEH (tabla 0x8468B8; el estado 0 solo cubre la llamada final y el funclet solo
desenrolla: salta a 0x61A3D0 con ECX = this guardado en [ebp-10h]):
  push -1 / push 8468B8h / mov eax,fs:[0] / push eax / mov fs:[0],esp / push ecx / push ebp
  mov ebp,ecx ; mov [esp+4],ebp ; mov dword ptr [ebp],870664h                    vtable de CTaskComplexPartner (0x870664)
  mov ecx,[ebp+38h] ; test ecx,ecx ; lea eax,[ebp+38h] ; mov [esp+10h],0 ; je L1   companero
  push eax ; call 00571A00                                                        CEntity::CleanUpOldReference(ECX = companero, [esp] = &this->m_partner), `ret 4`
L1: mov al,[ebp+6Eh] ; test al,al ; je L3                                         byte entero m_requiredAnimsStreamedIn
  push esi ; push edi ; lea eax,[ebp+5Eh] ; mov edi,858B54h ; mov esi,eax ; mov ecx,1 ; xor edx,edx ; repe cmps byte ptr [esi],byte ptr es:[edi] ; pop edi ; pop esi
  je L2                                                                           el primer byte del nombre (+0x5E) coincide con el byte de 0x858B54 (que vale 0): no hace nada
  push eax ; call 004D3990 ; push eax ; call 004D3FD0 ; add esp,8                 GetAnimationBlockIndex(nombre) (cdecl) y RemoveAnimBlockRef(indice) (cdecl)
L2: mov byte ptr [ebp+6Eh],0
L3: mov ecx,ebp ; mov [esp+10h],-1 ; call 0061A3D0                                CTaskComplex::Destructor (ECX = this)
  mov ecx,[esp+8] ; pop ebp ; mov fs:[0],ecx ; add esp,10h ; ret
Llamadores (ninguno lee EAX/ECX/EDX tras la llamada): 0x684206, 0x684286 y 0x684756 (jmp de cola de cuerpos de destructor de subclases), 0x684368 y 0x684F63 (`call`; despues solo se usan ESI/[esp+8]),
0x846983 (funclet SEH, jmp).
Se comprueba el dword escrito en [this], los bytes cambiados (solo los que difieren de la imagen previa, incluidos los que caen en la guarda posterior de la arena: el byte +0x6E), y los eventos de los
colaboradores en su orden: [0x571A00, companero, &this->m_partner, dword [this] al entrar], [0x4D3990, &nombre, FNV-1a de los 16 bytes del nombre], [0x4D3FD0, indice devuelto por el anterior] y
[0x61A3D0, this, [this], byte +0x6E, dword +0x38].

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CTaskComplexPartner::Destructor']
ENTRIES = (0x683A40,)
VTABLE = 0x870664
OTHER_VTABLES = (0x870754, 0x870800, 0x86C0A8, 0x86EAAC, 0x870ADC, 0x86FD74, 0x86DD50, 0x86D894)
GUARD = 0x40
OFF_PARTNER, OFF_NAME, OFF_FLAG = 0x38, 0x5E, 0x6E
EV_CLEANUP, EV_BLOCK_INDEX, EV_REMOVE_REF, EV_BASE = 0x571A00, 0x4D3990, 0x4D3FD0, 0x61A3D0


def dword_bytes(v):
    return [(v >> (8 * k)) & 0xFF for k in range(4)]


def fnv1a(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def expected(c):
    a = [x & M32 for x in c['a']]
    off, kind, fill = a[0], a[1], a[2]
    partner, flag, prev_vt, idx = a[3], a[4] & 0xFF, a[5], a[10]
    win, span = a[11], a[12]
    name = [b for w in a[6:10] for b in dword_bytes(w)]
    this = 0x10000000 + GUARD + off
    marks = ['dtor', 'fill%d' % fill, 'kind%d' % kind, 'this_unaligned' if off % 4 else 'this_aligned']
    if off + span > win:
        marks.append('this_spill')
    events = []
    if partner == 0:
        marks.append('partner_null')
    else:
        marks.append('partner_arena' if (partner >> 28) == 1 else 'partner_raw')
        if partner == this:
            marks.append('partner_self')
        # CleanUpOldReference: ECX = companero, argumento = &this->m_partner, [this] ya vale la vtable
        events.append([EV_CLEANUP, partner, this + OFF_PARTNER, VTABLE])
    changes = []
    new, old = dword_bytes(VTABLE), dword_bytes(prev_vt)
    changes += [[0, off + k, new[k]] for k in range(4) if new[k] != old[k]]
    if flag == 0:
        marks.append('flag_zero')
        marks.append('anim_skip_flag')
    else:
        marks.append('flag_set')
        if name[0] == 0:
            marks.append('anim_skip_name')
        else:
            marks.append('anim_calls')
            marks.append('idx_zero' if idx == 0 else 'idx_neg' if idx & 0x80000000 else 'idx_other')
            events.append([EV_BLOCK_INDEX, this + OFF_NAME, fnv1a(bytes(name))])
            events.append([EV_REMOVE_REF, idx])
        changes.append([0, off + OFF_FLAG, 0])
    marks.append('name_empty' if name[0] == 0 else 'name_set')
    marks.append('changes%d' % len(changes))
    if prev_vt == VTABLE:
        marks.append('prev_same')
    elif prev_vt == 0:
        marks.append('prev_zero')
    elif prev_vt == M32:
        marks.append('prev_ones')
    elif prev_vt == 0xDEADBEEF:
        marks.append('prev_deadbeef')
    elif prev_vt in OTHER_VTABLES:
        marks.append('prev_other_vt')
    elif len([k for k in range(4) if new[k] != old[k]]) < 4:
        marks.append('prev_partial')
    else:
        marks.append('prev_other')
    # CTaskComplex::Destructor: ECX = this, [this] = vtable, byte +0x6E ya a cero, dword +0x38 intacto (el companero)
    events.append([EV_BASE, this, VTABLE, 0, partner])
    changes.sort(key=lambda e: e[1])
    return {'events': events, 'marks': marks, 'changes': changes, 'ret': None, 'vptr': VTABLE}


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
        return 'eventos %s != %s' % (o['events'][:2], e['events'][:2])
    if o.get('vptr') != e['vptr']:
        return 'vtable escrita %s != %s' % (o.get('vptr'), e['vptr'])
    if o.get('ret') != e['ret']:
        return 'valor devuelto %s != %s' % (o.get('ret'), e['ret'])
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
