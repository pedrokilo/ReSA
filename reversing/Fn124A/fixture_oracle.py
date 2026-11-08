"""Oraculo independiente (Python) de Fn124A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las trece funciones del lote 124A y lo compara
con las filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y llamadores:
  0x617850  push esi; mov esi,ecx; call 4A8DB0; mov eax,esi; pop esi; ret             (12 B) constructor de elemento de BoneNode_c (lo pasa 0x617FC0, el constructor de BoneNodeManager_c, a
            eh_vector_constructor_iterator 0x821D30 con 0x617860 de destructor). 0x4A8DB0 = ListItem_c::ListItem_c: mov eax,ecx; [eax]=0; [eax+4]=0; ret. Devuelve this.
  0x5720C0  add ecx,8; push ecx; mov ecx,0C1B340h; call 6A0050; ret                   (15 B) CZone::GetTranslatedName = TheText.Get(m_TextLabel = this + 8). Llamadores 0x571E35, 0x571E67, 0x572005 (CPlaceName).
  0x5D0F40  mov eax,[esp+4]; imul eax,eax,104h; add eax,0C16368h; ret                  (16 B) GetNameOfSavedGame(slot): direccion del nombre de la ranura (CMenuManager::DrawStandardMenus 0x579BE4).
  0x5F6660  push esi; mov esi,ecx; call 61A3B0; mov [esi],86C7BCh; mov eax,esi; pop esi; ret   (18 B) constructor de CTaskComplexBeStill (vtable 0x86C7BC, GetTaskType 0x5F66D0 = 0x112). Sin llamadores.
  0x538AD0  fld dword [esp+4]; sub esp,8; fstp qword [esp]; call 823820; add esp,8; ret (19 B) ceilf del CRT: ST0 = ceil((double)x). Llamador 0x5A0463 (CObject::SpecialEntityCalcCollisionSteps).
  0x646AB0  mov eax,[ecx+8]; test eax,eax; je +; mov ecx,[esp+4]; call 5DF910; mov al,1; ret 4    (21 B) CTaskSimpleCarWaitToSlowDown::SetPedPosition (vtable 0x86ED98, ranura 8).
  0x420860  fld [ecx+4]; fld [ecx]; fld st0; fmul st,st1; fld st2; fmul st,st3; faddp st1,st; fsqrt; fstp st2; fstp st0; ret   (22 B) CVector2D::Magnitude fuera de linea. Sin llamadores.
  0x5617A0  sub esp,8; lea eax,[esp]; push eax; call [8580C8h]; mov eax,[esp]; mov edx,[esp+4]; add esp,8; ret   (26 B) GetOSWPerformanceTime (CTimer::Initialise la guarda en un puntero).
  0x59E620  push esi; push 59D180h; push 59D170h; push 40h; mov esi,ecx; push 20h; push esi; call 821D30; mov eax,esi; pop esi; ret   (27 B) constructor de BreakManager_c (64 x 0x20 bytes).
            0x59D170 = BreakObject_c::BreakObject_c: [eax]=cl=0 (byte), [eax+1]=0 (byte), [eax+8]=0 (dword). Llamador 0x84FA85 (inicializador estatico de g_breakMan 0xBB4240).
  0x502700  push esi; mov esi,ecx; call 5023F0; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4   (27 B) CAEVorbisDecoder destructor con borrado (ranura 6 de 0x862D70).
  0x503430  igual con 0x503290: CAEWaveDecoder destructor con borrado (ranura 6 de 0x862DE8).
  0x56F850  igual con 0x56F200 (add ecx,21Ch; jmp 4E6E00): CWanted destructor con borrado (0x571840 lo llama sobre m_pWanted); 0x4E6E00 recibe ECX = this + 0x21C.
  0x706850  igual con 0x706440 (~CRealTimeShadow: Destroy 0x705990 y dos CShadowCamera).

x87: las funciones 4 y 6 dejan UN registro valido (ST0) con el resultado en 80 bits; las demas dejan la pila vacia. La palabra de control del juego es de 24 bits de mantisa (redondeo al mas cercano)
y las excepciones enmascaradas: el modelo de la funcion 6 redondea cada operacion a 24 bits con exponente ampliado (x*x, y*y, la suma y la raiz).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from math import isqrt

M32 = 0xFFFFFFFF
NAMES = ['BoneNode_c::ArrayElementConstructor', 'CZone::GetTranslatedName', 'CGenericGameStorage::GetNameOfSavedGame', 'CTaskComplexBeStill::Constructor', 'CObject::CeilFloat',
         'CTaskSimpleCarWaitToSlowDown::SetPedPosition', 'CVector2D::Magnitude', 'CTimer::GetOSWPerformanceTime', 'BreakManager_c::Constructor', 'CAEVorbisDecoder::ScalarDeletingDestructor',
         'CAEWaveDecoder::ScalarDeletingDestructor', 'CWanted::ScalarDeletingDestructor', 'CRealTimeShadow::ScalarDeletingDestructor']
ENTRIES = (0x617850, 0x5720C0, 0x5D0F40, 0x5F6660, 0x538AD0, 0x646AB0, 0x420860, 0x5617A0, 0x59E620, 0x502700, 0x503430, 0x56F850, 0x706850)
SPAN = (8, 0x10, 0, 12, 0, 12, 8, 0, 0x7EC, 0x28, 0x3C, 0x220, 4)
HAS_THIS = (True, True, False, True, False, True, True, False, True, True, True, True, True)
GUARD = 0x40
WIN = 0x1000
BE_STILL_VTABLE = 0x86C7BC
THE_TEXT = 0xC1B340
SLOT_NAMES = 0xC16368
SLOT_SIZE = 0x104
# colaborador (direccion del EXE) y desplazamiento de ECX respecto a this en los destructores con borrado
SCALAR_COLLAB = {9: 0x5023F0, 10: 0x503290, 11: 0x4E6E00, 12: 0x706440}
SCALAR_ADJ = {9: 0, 10: 0, 11: 0x21C, 12: 0}
OPERATOR_DELETE = 0x8214BD


def t0(off):
    """etiqueta de un puntero dentro de la arena del objeto (arena 0): 0x10000000 + guarda + desplazamiento"""
    return 0x10000000 + GUARD + off


def t1(off):
    """etiqueta de un puntero dentro de la arena secundaria (arena 1)"""
    return 0x10000000 + (1 << 20) + GUARD + off


def dword_bytes(v):
    return [(v >> (8 * k)) & 0xFF for k in range(4)]


# ---------------------------------------------------------------------------------------------------- aritmetica x87 (24 bits de mantisa, exponente ampliado)
def f32_parts(bits):
    """(clase, signo, m, e, mant23): clase 'zero' | 'finite' | 'inf' | 'nan'; valor = m * 2**e"""
    sign = bits >> 31
    exp = (bits >> 23) & 0xFF
    man = bits & 0x7FFFFF
    if exp == 255:
        return ('inf' if man == 0 else 'nan', sign, 0, 0, man)
    if exp == 0:
        return ('zero', sign, 0, 0, 0) if man == 0 else ('finite', sign, man, -149, 0)
    return ('finite', sign, man | 0x800000, exp - 150, 0)


def round_to_bits(m, e, bits=24, sticky=False):
    """redondea m * 2**e (+ epsilon si sticky) a `bits` bits de mantisa, al mas cercano, empate al par"""
    if m == 0:
        return (0, 0)
    n = m.bit_length()
    if n <= bits:
        return (m, e)
    drop = n - bits
    q = m >> drop
    rem = m & ((1 << drop) - 1)
    half = 1 << (drop - 1)
    if rem > half or (rem == half and (sticky or (q & 1))):
        q += 1
    if q == (1 << bits):
        q >>= 1
        drop += 1
    return (q, e + drop)


def add_exact(a, b):
    (m1, e1), (m2, e2) = a, b
    if m1 == 0:
        return b
    if m2 == 0:
        return a
    e = min(e1, e2)
    return (m1 << (e1 - e)) + (m2 << (e2 - e)), e


def sqrt_round24(m, e):
    if m == 0:
        return (0, 0)
    if e % 2:
        m <<= 1
        e -= 1
    sh = 70
    big = m << (2 * sh)
    r = isqrt(big)
    return round_to_bits(r, e // 2 - sh, 24, r * r != big)


def ext80(sign, m, e):
    """codificacion de 80 bits de (-1)**sign * m * 2**e (m entero > 0 o 0)"""
    if m == 0:
        return (b'\x00' * 8) + ((sign << 15)).to_bytes(2, 'little')
    nb = m.bit_length()
    sig = m << (64 - nb) if nb <= 64 else m >> (nb - 64)   # m tiene como mucho 24 bits significativos: el desplazamiento a la derecha es exacto
    biased = e + nb - 1 + 16383
    return sig.to_bytes(8, 'little') + ((sign << 15) | biased).to_bytes(2, 'little')


def ext80_nan(sign, man23):
    """NaN silenciado (bit 22 puesto) de un float: significando con bit entero y la mantisa desplazada 40 bits"""
    sig = (1 << 63) | ((man23 | 0x400000) << 40)
    return sig.to_bytes(8, 'little') + ((sign << 15) | 0x7FFF).to_bytes(2, 'little')


def ext80_inf(sign):
    return (1 << 63).to_bytes(8, 'little') + ((sign << 15) | 0x7FFF).to_bytes(2, 'little')


def ceil_st0(bits):
    """ST0 de 80 bits tras ceil((double)float): (bytes, marca)"""
    cls, sign, m, e, man = f32_parts(bits)
    if cls == 'nan':
        return ext80_nan(sign, man), 'nan_q' if man & 0x400000 else 'nan_s'
    if cls == 'inf':
        return ext80_inf(sign), 'inf'
    if cls == 'zero':
        return ext80(sign, 0, 0), 'zero'
    if e >= 0:
        return ext80(sign, m << e, 0), 'integer'
    q, r = divmod(m, 1 << -e)
    if sign == 0:
        n = q + (1 if r else 0)
        return ext80(0, n, 0), ('frac_pos' if r else 'integer') if bits & 0x7F800000 else 'denormal_pos'
    mark = 'neg_to_zero' if q == 0 else ('frac_neg' if r else 'integer')
    return ext80(1, q, 0), mark


def magnitude_st0(bx, by):
    """ST0 de 80 bits tras sqrt(x*x + y*y) en la x87 de 24 bits: (bytes, marca)"""
    cx, sx, mx, ex, nx = f32_parts(bx)
    cy, sy, my, ey, ny = f32_parts(by)
    if cx == 'nan' or cy == 'nan':
        # x87: las dos pilas ya tienen QNaN (fld silencia las SNaN); con dos, gana el de mayor significando; un NaN con un valor: el NaN
        if cx == 'nan' and cy == 'nan':
            qx, qy = nx | 0x400000, ny | 0x400000
            assert qx != qy, 'empate de NaN no modelado'
            return (ext80_nan(sx, nx), 'nan_both') if qx > qy else (ext80_nan(sy, ny), 'nan_both')
        return (ext80_nan(sx, nx), 'nan_x') if cx == 'nan' else (ext80_nan(sy, ny), 'nan_y')
    if cx == 'inf' or cy == 'inf':
        return ext80_inf(0), 'inf'
    p = round_to_bits(mx * mx, 2 * ex) if cx == 'finite' else (0, 0)
    q = round_to_bits(my * my, 2 * ey) if cy == 'finite' else (0, 0)
    s = add_exact(p, q)
    s = round_to_bits(s[0], s[1])
    r = sqrt_round24(s[0], s[1]) if s[0] else (0, 0)
    mark = 'zero' if r[0] == 0 else 'normal'
    if r[0]:
        # fuera del rango de un float normal (el valor devuelto no cabria en un float): denormal o desbordado
        nb = r[0].bit_length()
        top = r[1] + nb - 1
        mark = 'above_float' if top > 127 else 'below_float' if top < -126 else 'normal'
    return ext80(0, r[0], r[1]), mark


def f32_class(bits):
    cls, sign, m, e, man = f32_parts(bits)
    if cls == 'finite':
        return 'denormal' if (bits >> 23) & 0xFF == 0 else 'normal'
    return cls


# ---------------------------------------------------------------------------------------------------- modelo
def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    kind, fill = a[1], a[2]
    marks = ['fill%d' % fill, 'kind%d' % kind] if HAS_THIS[fn] else ['family%d' % c['family']]
    out = {'events': [], 'changes': [], 'ret': None, 'al': None, 'q': None, 'st0': None, 'marks': marks}
    off = a[0]
    if HAS_THIS[fn]:
        marks.append('this_unaligned' if off % 4 else 'this_aligned')
        if off + SPAN[fn] > WIN:
            marks.append('this_spill')
    if fn == 0:
        prev = dword_bytes(a[6]) + dword_bytes(a[7])
        out['changes'] = [[0, off + k, 0] for k in range(8) if prev[k]]
        out['ret'] = t0(off)
        marks.append('changes0' if not out['changes'] else 'changes8' if len(out['changes']) == 8 else 'changes_some')
        marks.append('prev_zero' if not any(prev) else 'prev_ones' if all(b == 0xFF for b in prev) else 'prev_partial')
    elif fn == 1:
        out['events'] = [[0x6A0050, THE_TEXT, t0(off + 8), a[6], a[7]]]
        out['ret'] = a[8]
        marks.append('key_zero' if a[6] == 0 and a[7] == 0 else 'key_ones' if a[6] == M32 and a[7] == M32 else 'key_other')
        marks.append('ret_zero' if a[8] == 0 else 'ret_nonzero')
    elif fn == 2:
        out['ret'] = (SLOT_NAMES + a[0] * SLOT_SIZE) & M32
        slot = a[0] - (1 << 32) if a[0] >> 31 else a[0]
        marks.append('slot_valid' if 0 <= slot < 8 else 'slot_negative' if slot < 0 else 'slot_high')
        marks.append('wraps' if not (0 <= slot * SLOT_SIZE + SLOT_NAMES < (1 << 32)) else 'in_range')
    elif fn == 3:
        new, old = dword_bytes(BE_STILL_VTABLE), dword_bytes(a[6])
        out['changes'] = [[0, off + k, new[k]] for k in range(4) if new[k] != old[k]]
        out['events'] = [[0x61A3B0, t0(off), a[6]]]
        out['ret'] = t0(off)
        marks.append('changes%d' % len(out['changes']))
        marks.append('prev_same' if a[6] == BE_STILL_VTABLE else 'prev_zero' if a[6] == 0 else 'prev_ones' if a[6] == M32 else 'prev_other')
    elif fn == 4:
        out['st0'], mark = ceil_st0(a[0])
        marks.append(mark)
        marks.append('class_' + f32_class(a[0]))
    elif fn == 5:
        mode, raw = a[6], a[7]
        vehicle = 0 if mode == 0 else 1 if mode == 1 else raw
        pmode, praw = a[8], a[9]
        ped = 0 if pmode == 0 else t1(a[10]) if pmode == 1 else praw
        if vehicle:
            out['events'] = [[0x5DF910, ped]]
        out['al'] = 1
        marks.append('vehicle' if vehicle else 'no_vehicle')
        marks.append('ped_null' if pmode == 0 else 'ped_arena' if pmode == 1 else 'ped_raw')
        marks.append('ret_al_hidden' if (a[11] & 0xFF) != 1 else 'ret_al_one')
    elif fn == 6:
        out['st0'], mark = magnitude_st0(a[6], a[7])
        marks.append(mark)
        marks.append('x_' + f32_class(a[6]))
        marks.append('y_' + f32_class(a[7]))
    elif fn == 7:
        out['events'] = [[1, 0, 1]]
        out['q'] = [a[6], a[7]]
        marks.append('bool_one' if a[8] == 1 else 'bool_zero' if a[8] == 0 else 'bool_other')
        marks.append('hi_zero' if a[7] == 0 else 'hi_set')
    elif fn == 8:
        changes = []
        active = 0
        for e in range(64):
            if not ((a[9] | (a[10] << 32)) >> e) & 1:
                continue
            active += 1
            b0 = ((e + 1) * a[6]) & 0xFF
            b1 = ((e + 3) * a[7]) & 0xFF
            d8 = ((e + 1) * a[8]) & M32
            base = off + 0x20 * e
            if b0:
                changes.append([0, base, 0])
            if b1:
                changes.append([0, base + 1, 0])
            changes.extend([0, base + 8 + k, 0] for k, v in enumerate(dword_bytes(d8)) if v)
        out['changes'] = changes
        out['ret'] = t0(off)
        marks.append('active%d' % (active if active < 5 else 5 if active < 8 else 8 if active < 16 else 16 if active < 32 else 32 if active < 64 else 64))
        marks.append('changes0' if not changes else 'changes_some')
    else:
        collab = SCALAR_COLLAB[fn]
        flags = a[7]
        out['events'] = [[collab, t0(off + SCALAR_ADJ[fn]), a[6]]]
        if flags & 1:
            out['events'].append([OPERATOR_DELETE, t0(off)])
        out['ret'] = t0(off)
        marks.append('delete' if flags & 1 else 'keep')
        marks.append('flags_zero' if flags == 0 else 'flags_one' if flags == 1 else 'flags_bit0_more' if flags & 1 else 'flags_upper_only' if flags & ~0xFF and not flags & 0xFF else 'flags_even')
    return out


def fp_stack_valid(hexstr):
    """posiciones validas de una imagen fnsave normalizada: [(st, 10 bytes)]"""
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
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    for k in ('ret', 'al', 'q'):
        if o.get(k) != e[k]:
            return 'campo %s %s != %s' % (k, o.get(k), e[k])
    valid = fp_stack_valid(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != e['st0']:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], e['st0'].hex())
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
