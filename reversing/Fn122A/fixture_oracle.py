"""Oraculo independiente (Python) de Fn122A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote 122A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables (leidas del PE y de los llamadores):
  0x532240  66 C7 05 08 15 B7 00 00 00 C3     mov word ptr ds:[0B71508h],0; ret                  (10 B) CDebug::DebugInitTextBuffer, llamada desde CGame::Init1 (0x5BF905, entre CReferences::Init y CTagManager::Init)
  0x5DDBE0  C7 01 A8 C0 86 00 E9 35 AA 00 00  mov [ecx],86C0A8h; jmp 005E8620                    (11 B) cuerpo de ~CCivilianPed (llamado por 0x5DDC30, el destructor con borrado)
  0x6C2740  8B C1 33 C9 89 48 4C 89 48 50 C3  mov eax,ecx; xor ecx,ecx; mov [eax+4Ch],ecx; mov [eax+50h],ecx; ret   cBuoyancy::cBuoyancy (llamada desde 0x853885: inicializador estatico de 0xC1C890)
  0x640330  C7 01 AC EA 86 00 E9 65 DC FF FF  mov [ecx],86EAACh; jmp 0063DFA0                    ~CTaskComplexEnterCarAsDriver (llamado desde 0x643C63, el destructor con borrado)
  0x640380  C7 01 DC EA 86 00 E9 15 DC FF FF  mov [ecx],86EADCh; jmp 0063DFA0                    ~CTaskComplexEnterCarAsPassenger (0x643C83 y funclets 0x4926B8, 0x8389D7)
  0x690D90  C7 01 DC 0A 87 00 E9 35 96 F8 FF  mov [ecx],870ADCh; jmp 0061A3D0                    ~CTaskComplexFacial (0x692F93)
  0x684200  C7 01 54 07 87 00 E9 35 F8 FF FF  mov [ecx],870754h; jmp 00683A40                    ~CTaskComplexPartnerDeal (0x684F83, 0x5FA52C y el funclet 0x83E373)
  0x667E70  C7 01 74 FD 86 00 E9 25 25 FB FF  mov [ecx],86FD74h; jmp 0061A3A0                    ~CTaskSimpleAchieveHeading (0x66DB63)
  0x667A00  C7 01 1C FD 86 00 E9 95 29 FB FF  mov [ecx],86FD1Ch; jmp 0061A3A0                    ~CTaskSimpleGoTo, segundo cuerpo identico al de 0x667D50 (0x66D6D3)
  0x62F5D0  C7 01 50 DD 86 00 E9 C5 AD FE FF  mov [ecx],86DD50h; jmp 0061A3A0                    ~CTaskSimpleSetStayInSamePlace (0x6374A3, 0x4929D6 y el funclet 0x838A33)
Los cuerpos de destructor escriben un dword (la vtable) en [this] y saltan (jmp, no call) al destructor base: el colaborador recibe this en ECX y la vtable ya escrita, y su `ret` vuelve al llamador
original; ningun llamador lee EAX/ECX/EDX tras ellos (solo ESI, callee-saved). El constructor de cBuoyancy devuelve this en EAX y pone a 0 los dwords de +0x4C y +0x50 (m_pAttachMatrix y la bandera
de propiedad de m_EntityMatrix, que empieza en +0x0C). La funcion 0 solo escribe la palabra baja de 0xB71508. Se comprueba el dword escrito, los bytes cambiados (solo los que difieren de la imagen
previa, incluidos los que caen en la guarda posterior de la arena), los eventos del colaborador [direccion, this como etiqueta de arena, dword en [this] al entrar] y, en el constructor, `this`
devuelto como etiqueta de arena (0x10000000 + 0x40 de guarda + desplazamiento).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
NAMES = ['CDebug::DebugInitTextBuffer', 'CCivilianPed::DestructorBody', 'cBuoyancy::Constructor', 'CTaskComplexEnterCarAsDriver::Destructor', 'CTaskComplexEnterCarAsPassenger::Destructor',
         'CTaskComplexFacial::Destructor', 'CTaskComplexPartnerDeal::Destructor', 'CTaskSimpleAchieveHeading::Destructor', 'CTaskSimpleGoTo::DestructorBody', 'CTaskSimpleSetStayInSamePlace::Destructor']
ENTRIES = (0x532240, 0x5DDBE0, 0x6C2740, 0x640330, 0x640380, 0x690D90, 0x684200, 0x667E70, 0x667A00, 0x62F5D0)
# vtable que deja cada cuerpo (leida del `mov [ecx], imm32` del EXE) y destino del `jmp` final (None: no salta)
VTABLE = (None, 0x86C0A8, None, 0x86EAAC, 0x86EADC, 0x870ADC, 0x870754, 0x86FD74, 0x86FD1C, 0x86DD50)
JUMPS_TO = (None, 0x5E8620, None, 0x63DFA0, 0x63DFA0, 0x61A3D0, 0x683A40, 0x61A3A0, 0x61A3A0, 0x61A3A0)
RETURNS_THIS = (False, False, True, False, False, False, False, False, False, False)
GUARD = 0x40
GLOBAL_WORD = 0xB71508
KIND = ('global', 'dtor', 'ctor', 'dtor', 'dtor', 'dtor', 'dtor', 'dtor', 'dtor', 'dtor')


def dword_bytes(v):
    return [(v >> (8 * k)) & 0xFF for k in range(4)]


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    off, kind, fill, win, span = a[0], a[1], a[2], a[4], a[5]
    marks = [KIND[fn], 'fill%d' % fill, 'kind%d' % kind]
    if fn == 0:
        prev = sum((dword_bytes(a[6 + i]) for i in range(4)), [])
        changes = [[2, k, 0] for k in (0, 1) if prev[k] != 0]
        marks.append('changes%d' % len(changes))
        if prev[0] == 0 and prev[1] == 0:
            marks.append('prev_word_zero')
        elif prev[0] == 0xFF and prev[1] == 0xFF:
            marks.append('prev_word_ff')
        else:
            marks.append('prev_word_other')
        if prev[2] or prev[3]:
            marks.append('neighbors_nonzero')
        return {'events': [], 'marks': marks, 'changes': changes, 'ret': None, 'vptr': None}
    marks.append('this_unaligned' if off % 4 else 'this_aligned')
    if off + span > win:
        marks.append('this_spill')
    if fn == 2:
        prev = dword_bytes(a[6]) + dword_bytes(a[7])
        changes = [[0, off + 0x4C + k, 0] for k in range(8) if prev[k] != 0]
        marks.append('changes%d' % len(changes))
        zeros = sum(1 for b in prev if b == 0)
        marks.append('prev_zero' if zeros == 8 else 'prev_ones' if all(b == 0xFF for b in prev) else 'prev_partial' if zeros else 'prev_other')
        return {'events': [], 'marks': marks, 'changes': changes, 'ret': 0x10000000 + GUARD + off, 'vptr': None}
    vt, prev = VTABLE[fn], a[6]
    new, old = dword_bytes(vt), dword_bytes(prev)
    changes = [[0, off + k, new[k]] for k in range(4) if new[k] != old[k]]
    marks.append('changes%d' % len(changes))
    if prev == vt:
        marks.append('prev_same')
    elif prev == 0:
        marks.append('prev_zero')
    elif prev == M32:
        marks.append('prev_ones')
    elif prev == 0xDEADBEEF:
        marks.append('prev_deadbeef')
    elif prev in [v for v in VTABLE if v]:
        marks.append('prev_other_vt')
    elif 0 < len(changes) < 4:
        marks.append('prev_partial')
    else:
        marks.append('prev_other')
    # el colaborador recibe this (ECX) con la vtable ya escrita: [direccion del EXE, this (etiqueta de arena), dword en [this]]
    events = [[JUMPS_TO[fn], 0x10000000 + GUARD + off, vt]]
    return {'events': events, 'marks': marks, 'changes': changes, 'ret': None, 'vptr': vt}


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
