"""Oraculo independiente (Python) de Fn90A: reproduce, desde la ENTRADA de cada caso y la lectura del desensamblado del EXE, lo que debe observarse de las diez funciones del lote piloto 90A
(CPad::GetPad 0x53FB70, CRGBA::Constructor 0x7170C0, CTaskSimple::Constructor 0x61A390, CTaskComplex::Constructor 0x61A3B0, CAnimManager::RemoveAnimBlockRef 0x4D3FD0,
Fx_c::GetFxQuality 0x49EA50, Fx_c::SetFxQuality 0x49EA40, CSphere::Set 0x40FCF0, FxPrtMult_c::Constructor 0x4AB270, CPedDamageResponseCalculator::Destructor 0x4AD420)
y lo compara con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion: los modelos salen del desensamblado del EXE:

  0x53FB70  mov eax,[esp+4]; imul eax,eax,134h; add eax,0B73458h; ret
  0x7170C0  mov dl,[esp+8]; mov eax,ecx; mov cl,[esp+4]; mov [eax],cl; mov cl,[esp+0Ch]; mov [eax+1],dl; mov dl,[esp+10h]; mov [eax+2],cl; mov [eax+3],dl; ret 10h
  0x61A390  mov eax,ecx; mov dword [eax+4],0; mov dword [eax],86D4A8h; ret
  0x61A3B0  mov eax,ecx; xor ecx,ecx; mov [eax+4],ecx; mov dword [eax],86D4CCh; mov [eax+8],ecx; ret
  0x4D3FD0  mov eax,[esp+4]; shl eax,5; add eax,0B5D4A0h; dec word [eax+12h]; ret
  0x49EA50  mov eax,[ecx+54h]; ret
  0x49EA40  mov eax,[esp+4]; mov [ecx+54h],eax; ret 4
  0x40FCF0  mov eax,[esp+4]; mov edx,[esp+8]; mov [ecx+0Ch],eax; mov eax,[edx]; mov [ecx],eax; mov eax,[edx+4]; mov [ecx+4],eax; mov edx,[edx+8]; mov [ecx+8],edx; ret 8
  0x4AB270  mov eax,ecx; mov ecx,3F800000h; mov [eax],ecx; ... mov [eax+18h],ecx; ret      (7 dwords)
  0x4AD420  ret

Registros: solo se exigen los que el EXE NO escribe (ECX, EDX) y EAX cuando es retorno definido o queda intacto.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import sys

M32 = 0xFFFFFFFF
GUARD = 0x40
WIN = 0x100
ARENA = GUARD + WIN + GUARD
PADS, PAD_SIZE, ANIM = 0xB73458, 0x134, 0xB5D4A0

NAMES = ['CPad::GetPad', 'CRGBA::Constructor', 'CTaskSimple::Constructor', 'CTaskComplex::Constructor', 'CAnimManager::RemoveAnimBlockRef',
         'Fx_c::GetFxQuality', 'Fx_c::SetFxQuality', 'CSphere::Set', 'FxPrtMult_c::Constructor', 'CPedDamageResponseCalculator::Destructor']
# EAX: 'ret' valor definido por el modelo, 'this', 'keep' (intacto) o None (resto sin contrato)
EAX = ['ret', 'this', 'this', 'this', None, 'ret', None, None, 'this', 'keep']
ECX_KEEP = [True, False, True, False, True, True, True, True, False, True]
EDX_KEEP = [True, False, True, True, True, True, True, False, True, True]


def tag(base, v):
    if base <= v < base + ARENA:
        d = v - (base + GUARD)
        return 'A%s%d' % ('-' if d < 0 else '+', abs(d))
    return v


def untag(base, v):
    if isinstance(v, str):
        d = int(v[2:])
        return (base + GUARD + (-d if v[1] == '-' else d)) & M32
    return v


def le32(win, off, v):
    for k in range(4):
        win[off + k] = (v >> (8 * k)) & 0xFF


def rd32(win, off):
    return sum(win[off + k] << (8 * k) for k in range(4))


def expected(c):
    fn = c['fn']
    base = c['base']
    pre = bytearray.fromhex(c['pre'])
    win = bytearray(pre)
    off = c['off']
    args = [untag(base, a) for a in c['args']]
    this = (base + GUARD + off) & M32
    marks = []
    eax = None
    if fn == 0:
        a = args[0]
        eax = (PADS + ((a * PAD_SIZE) & M32)) & M32
        marks.append('pad_small' if a < 4 else 'pad_other')
        marks.append('neg' if a >= 0x80000000 else 'pos')
        marks.append('wrap' if a * PAD_SIZE > M32 else 'nowrap')
    elif fn == 1:
        for k in range(4):
            win[off + k] = args[k] & 0xFF
        eax = this
        marks.append('garbage_high' if any(a > 0xFF for a in args) else 'bytes_only')
        marks.append('all_zero' if all((a & 0xFF) == 0 for a in args) else 'nonzero')
    elif fn == 2:
        le32(win, off + 4, 0)
        le32(win, off, 0x86D4A8)
        eax = this
    elif fn == 3:
        le32(win, off + 4, 0)
        le32(win, off, 0x86D4CC)
        le32(win, off + 8, 0)
        eax = this
    elif fn == 4:
        idx = args[0]
        target = (ANIM + ((idx << 5) & M32)) & M32
        woff = (target + 0x12 - (base + GUARD)) & M32
        assert woff + 2 <= WIN, 'bloque fuera de la ventana'
        old = win[woff] | (win[woff + 1] << 8)
        new = (old - 1) & 0xFFFF
        win[woff] = new & 0xFF
        win[woff + 1] = new >> 8
        marks.append('wrap' if old == 0 else 'dec')
        marks.append('to_signed_min' if old == 0x8000 else 'other_old')
        marks.append('high_bits' if idx >> 27 else 'no_high_bits')
    elif fn == 5:
        eax = rd32(win, off + 0x54)
        marks.append('q_zero' if eax == 0 else 'q_other')
    elif fn == 6:
        le32(win, off + 0x54, args[0])
        marks.append('q_small' if args[0] < 5 else 'q_big')
    elif fn == 7:
        radius, p = args[0], args[1] - (base + GUARD)
        le32(win, off + 0xC, radius)
        for k in (0, 4, 8):
            le32(win, off + k, rd32(win, p + k))
        d = p - off
        marks.append('alias' if -12 < d < 16 else 'separate')
        marks.append('radius_nan' if (radius & 0x7F800000) == 0x7F800000 and (radius & 0x7FFFFF) else 'radius_num')
    elif fn == 8:
        for k in range(7):
            le32(win, off + 4 * k, 0x3F800000)
        eax = this
    else:
        eax = c['eax']
        marks.append('noop')
    changes = [[i, win[i]] for i in range(WIN) if win[i] != pre[i]]
    obs = {}
    mode = EAX[fn]
    obs['eax'] = tag(base, eax) if mode in ('ret', 'this', 'keep') else None
    obs['ecx'] = c['ecx_in'] if ECX_KEEP[fn] else None
    obs['edx'] = tag(base, c['edx']) if EDX_KEEP[fn] else None
    obs['changes'] = changes
    obs['marks'] = marks
    return obs


def check_side(c, o, e):
    for k in ('eax', 'ecx', 'edx', 'changes'):
        if o.get(k) != e[k]:
            return '%s: %s != %s' % (k, str(o.get(k))[:300], str(e[k])[:300])
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
