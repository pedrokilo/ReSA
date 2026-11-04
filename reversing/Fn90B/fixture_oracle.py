"""Oraculo independiente (Python) de Fn90B: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de las ocho funciones del lote 90B
(CNodeAddress::operator== 0x420920, operator!= 0x420950, operator< 0x420980, CWorld::AdvanceCurrentScanCode 0x4072E0 y los constructores CTaskSimpleScratchHead 0x48DF30,
CTaskSimpleAchieveHeading 0x667E20, CColModel 0x40FB60 y CEventDeath(bool) 0x4ADDF0) y lo compara con las filas de comparisons.jsonl (lado original y candidato).
No usa ninguna constante ni rama del C++ de produccion.

Desensamblado (EXE canonico):
  0x420920: mov dx,[ecx]; mov eax,[esp+4]; cmp dx,[eax]; jne 0; mov cx,[ecx+2]; cmp cx,[eax+2]; jne 0; eax=1       -> igual en area y nodo
  0x420950: igual que el anterior con los resultados invertidos
  0x420980: ax=[ecx]; dx=[otra]; cmp ax,dx; jb 1; jne 0; ax=[ecx+2]; cmp ax,[otra+2]; jb 1; si no 0              -> menor sin signo (area y luego nodo)
  0x4072E0: cmp word [0xB7CD78],0xFFFF; jae wrap; inc word [0xB7CD78]; ret; wrap: call 0x563470; mov word [0xB7CD78],1
  0x48DF30: llama a 0x61A900 (CTaskSimpleRunAnim, args 0,0x87,4.0f,0x1A5,"nombre",0) -> 0x61A390 (CTask: [+4]=0, vtable); [+8]=0, [+0xC]=(old&0xF8)|(arg6&1)<<2, [+0x10]=0, [+0x14]=0x87,
            [+0x18]=4.0f, word [+0x1C]=0x1A5; despues vtable [+0]=0x85A100; devuelve this
  0x667E20: llama a 0x61A390; [+0x14]&=0xFE; [+8]=arg1; vtable=0x86FD74; [+0xC]=arg2; [+0x10]=arg3; devuelve this
  0x40FB60: llama a 0x40FAF0 ([+0..+8]=1.0f, [+0xC..+0x14]=-1.0f); [+0x28]=0; [+0x2C]=0; [+0x29]=(old&0xFC)|4; devuelve this
  0x4ADDF0: [+4]=0; [+8]=0 (byte); [+0xC]=byte bajo del argumento; vtable=0x85ADC0; [+0x10]=dword 0xB7CB84; devuelve this

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys

M32 = 0xFFFFFFFF
NAMES = ['CNodeAddress::operator==', 'CNodeAddress::operator!=', 'CNodeAddress::operator<', 'CWorld::AdvanceCurrentScanCode',
         'CTaskSimpleScratchHead::CTaskSimpleScratchHead', 'CTaskSimpleAchieveHeading::CTaskSimpleAchieveHeading', 'CColModel::CColModel', 'CEventDeath::CEventDeath']
ADDRESS = [0x420920, 0x420950, 0x420980, 0x4072E0, 0x48DF30, 0x667E20, 0x40FB60, 0x4ADDF0]
CASES = 4096
WIN = 0x40


def f32class(b):
    e = (b >> 23) & 0xFF
    m = b & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0 and m:
        return 'denormal'
    if b == 0x80000000:
        return 'negzero'
    return None


def put32(w, off, v):
    w[off:off + 4] = struct.pack('<I', v & M32)


def expected(c):
    fn = c['fn']
    marks = []
    mark = marks.append
    out = {'events': [], 'b_changes': [], 'time': c['time'], 'scan': c['scan'], 'vtable_ok': True, 'changes': [], 'marks': marks}
    if fn <= 2:
        a0, a1 = c['a'] & 0xFFFF, c['a'] >> 16
        b0, b1 = c['b'] & 0xFFFF, c['b'] >> 16
        if c['alias']:
            mark('alias')
            assert (b0, b1) == (a0, a1)
        eq = a0 == b0 and a1 == b1
        lt = a0 < b0 or (a0 == b0 and a1 < b1)

        def s16(v):
            return v - 0x10000 if v & 0x8000 else v
        slt = s16(a0) < s16(b0) or (s16(a0) == s16(b0) and s16(a1) < s16(b1))
        if slt != lt:
            mark('sign_trap')
        mark('area_eq' if a0 == b0 else 'area_diff')
        mark('node_eq' if a1 == b1 else 'node_diff')
        r = [eq, not eq, lt][fn]
        mark('true' if r else 'false')
        if fn == 2:
            mark('lt_by_area' if a0 < b0 else ('lt_by_node' if lt else 'not_lt'))
        out['ret'] = 1 if r else 0
        out['changes'] = []
        return out
    if fn == 3:
        w = c['scan'] & 0xFFFF
        hi = c['scan'] & 0xFFFF0000
        if hi:
            mark('upper_nonzero')
        if w >= 0xFFFF:
            mark('wrap')
            out['events'] = [[1, 0xFFFF]]
            nw = 1
        else:
            mark('inc')
            if w == 0xFFFE:
                mark('inc_to_max')
            if w == 0:
                mark('inc_from_zero')
            nw = w + 1
        out['scan'] = hi | nw
        return out
    win = bytearray.fromhex(c['win'])
    assert len(win) == WIN
    init = bytes(win)
    out['ret'] = 1
    if fn == 4:
        c0 = init[0xC]
        mark('c_bits_set' if c0 & 7 else 'c_bits_clear')
        put32(win, 0, 0x85A100)
        put32(win, 4, 0)
        put32(win, 8, 0)
        win[0xC] = c0 & 0xF8
        put32(win, 0x10, 0)
        put32(win, 0x14, 0x87)
        put32(win, 0x18, 0x40800000)
        win[0x1C:0x1E] = struct.pack('<H', 0x1A5)
    elif fn == 5:
        for k, key in (('h', 'h'), ('rate', 'rate'), ('tol', 'tol')):
            cl = f32class(c[key])
            if cl:
                mark(cl + '_arg')
        b14 = init[0x14]
        mark('bit0_set' if b14 & 1 else 'bit0_clear')
        put32(win, 0, 0x86FD74)
        put32(win, 4, 0)
        put32(win, 8, c['h'])
        put32(win, 0xC, c['rate'])
        put32(win, 0x10, c['tol'])
        win[0x14] = b14 & 0xFE
    elif fn == 6:
        b29 = init[0x29]
        mark('flags_low_set' if b29 & 3 else 'flags_low_clear')
        mark('bit2_set' if b29 & 4 else 'bit2_clear')
        out['vtable_ok'] = True
        put32(win, 0, 0x3F800000)
        put32(win, 4, 0x3F800000)
        put32(win, 8, 0x3F800000)
        put32(win, 0xC, 0xBF800000)
        put32(win, 0x10, 0xBF800000)
        put32(win, 0x14, 0xBF800000)
        win[0x28] = 0
        put32(win, 0x2C, 0)
        win[0x29] = (b29 & 0xFC) | 4
    else:
        d = c['drown']
        mark('drown1' if d & 1 else 'drown0')
        if d >> 8:
            mark('garbage_high')
        if c['time'] == 0:
            mark('time0')
        if c['time'] == 0xFFFFFFFF:
            mark('timemax')
        put32(win, 0, 0x85ADC0)
        put32(win, 4, 0)
        win[8] = 0
        win[0xC] = d & 0xFF
        put32(win, 0x10, c['time'])
    out['changes'] = [[i, win[i]] for i in range(WIN) if win[i] != init[i]]
    # La ventana empieza en el objeto; los bytes de la guarda no cambian
    return out


def check_side(c, o, e):
    for k in ('ret', 'changes', 'b_changes', 'scan', 'time', 'vtable_ok'):
        if k in e and o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'], e['events'])
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
