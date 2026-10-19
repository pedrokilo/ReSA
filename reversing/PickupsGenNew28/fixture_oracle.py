"""Oraculo independiente del banco PickupsGenNew28: CPickups::GenerateNewOne (0x456F20).

Se modela desde el DESENSAMBLADO del EXE (bash reversing/_handoff/dis.sh 0x456F20 0x457380), no desde el C++ de la DLL, a partir de la fila `input` del banco.

Pila (cdecl, `ret` normal; el llamador retira 0x24): +04 x, +08 y, +0C z (float), +10 modelId, +14 pickupType (BYTE: `mov cl,[esp+14h]`), +18 ammo, +1C moneyPerDay (solo la palabra baja),
+20 isEmpty (BYTE: `mov dl,[esp+28h]` tras dos push), +24 message. Ranura = 0x20 bytes en 0x9788C0 + 0x20*i, i < 0x26C (620):
  +00 float ingresos, +04 m_pObject, +08 ammo, +0C regeneracion, +10 x (int16), +12 y, +14 z, +16 dinero/dia (u16), +18 modelo (u16), +1A referencia (u16), +1C tipo, +1D banderas, +1E/+1F sin uso.
Busqueda (0x456F20..0x4570A0):
  456F20..456F3A  tipo == 0Dh o 0Bh, o byte isEmpty != 0 -> busca el ULTIMO tipo 0 (456F3E..456F5A: ebx=0x26B, baja de 0x20 en 0x20 mientras tipo != 0); si no, el PRIMERO (456F62..456FF4, desenrollado x10).
  Si no hay ninguno: 457000 -> primer tipo 8 (45700B..); 457097 -> primer tipo 4 o 5 (4570A0..); 4570EF.. -> -1 (eax = 0FFFFFFFFh).
  En 2 y 3 (45719A): si m_pObject != 0: CWorld::Remove(obj) (0x563280), se relee m_pObject y si != 0 se llama al destructor virtual [vtbl+0] con 1 (borrado); m_pObject = 0.
Relleno (4571DF..4572F3), en este orden de escritura:
  [+08] = ammo ; [+16] = word(moneyPerDay) ; banderas = (banderas & F8) | ((isEmpty & 1) << 1) ; [+1C] = tipo ; [+00] = 0 ; [+0C] = ahora (0xB7CB84)
  tipo 4: [+0C] = ahora + 4E20h ; tipo 5: + 1D4C0h ; tipo 8: + 7530h ; tipo 9 o 0Ah: [+1C] = 9, [+0C] = ahora + 5DCh ; tipo 0Bh o 0Ch: [+1C] = 0Bh, [+0C] = ahora + 5DCh (todo mod 2^32)
  [+18] = word(modelId) ; call 0x455500 (FindTextIndexForString(message), cdecl) ; banderas = (banderas & 8F) | ((al << 4) & 70h)
  x, y, z: fld ; fmul 8.0 (0x859000) ; call 0x821B40 (_ftol2) ; mov word -> [+10], [+12], [+14] (en ese orden)
  call 0x454C70 (IsVisible; ECX = ranura; solo cuenta AL) ; banderas = (banderas & F7) | ((al & 1) << 3) ; [+04] = 0
  si banderas & 8: call 0x4567E0 (GiveUsAPickUpObject; ECX = ranura, push &m_pObject, push -1) ; si m_pObject != 0: CWorld::Add(obj) (0x563220)
Final (457341..): ref = word [+1A] ; si ref >= 0FFFEh -> ref = 1 si no ref + 1 ; [+1A] = ref ; eax = (ref << 16) | indice.

El banco sustituye FindTextIndexForString, IsVisible, GiveUsAPickUpObject, CWorld::Remove, CWorld::Add y el destructor virtual (objetos falsos con vtbl falsa) por falsos
idénticos en ambos lados que anotan la traza [tipo de evento, a, b, c, indice de la primera ranura distinta de la inicial, 32 bytes de esa ranura]; eso fija el ORDEN de las escrituras
respecto de cada llamada. Eventos: 1 Remove(obj) ; 2 destructor(obj, flag) ; 3 FindText(mensaje nulo?0:1, fnv32) ; 4 IsVisible(ranura) ; 5 GiveUs(ranura, &m_pObject - ranura, slot=-1 como u32) ; 6 Add(obj).
m_pObject se anota como ETIQUETA (0, 0x0B100001..4 = objetos falsos, o el valor crudo).
"""
import math
import struct
import sys

M32 = 0xFFFFFFFF
CASES_TOTAL = 6144
NAME = 'CPickups::GenerateNewOne'
NAMES = [NAME]
CASES = {NAME: CASES_TOTAL}
N = 620
SL = 32
LABEL_BASE = 0x0B100000
BOUNDARY = (0, 1, 9, 10, 11, 309, 310, 609, 610, 618, 619)


def fnv32(s):
    h = 2166136261
    for ch in s.encode('latin-1'):
        h = ((h ^ ch) * 16777619) & M32
    return h


def u32(t, o):
    return int.from_bytes(t[o:o + 4], 'little')


def w32(t, o, v):
    t[o:o + 4] = (v & M32).to_bytes(4, 'little')


def w16(t, o, v):
    t[o:o + 2] = (v & 0xFFFF).to_bytes(2, 'little')


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def pos16(bits):
    """Palabra baja de _ftol2(float(bits) * 8.0f) (0x821B40: trunca a int64; NaN, inf y |v| >= 2^63 dan el indefinido 0x8000000000000000 -> palabra baja 0)."""
    x = fl(bits)
    if math.isnan(x) or math.isinf(x):
        return 0
    v = x * 8.0                                   # exacto en doble; el fmul del EXE da float: desborda a inf si |v| >= 2^128
    if abs(v) >= 2.0 ** 128 or abs(v) >= 2.0 ** 63:
        return 0
    return int(v) & 0xFFFF                        # int() trunca hacia cero


def build_table(inp):
    t = bytearray([0xA5] * (N * SL))
    for i in range(N):
        t[i * SL + 0x1C] = inp['filler_type']
    for idx, hx in inp['slots']:
        b = bytes.fromhex(hx)
        assert len(b) == SL and 0 <= idx < N
        t[idx * SL:(idx + 1) * SL] = b
    return t


def find_slot(types, ptype, empty):
    """Devuelve (indice o -1, pasada 1/2/3, desde_el_final)."""
    top = ptype in (13, 11) or empty != 0
    order = range(N - 1, -1, -1) if top else range(N)
    for i in order:
        if types[i] == 0:
            return i, 1, top
    for i in range(N):
        if types[i] == 8:
            return i, 2, top
    for i in range(N):
        if types[i] in (4, 5):
            return i, 3, top
    return -1, 0, top


def simulate(inp):
    t = build_table(inp)
    pre = bytes(t)
    ptype, empty = inp['ptype'] & 0xFF, inp['empty'] & 0xFF
    now = inp['now'] & M32
    msg = inp['msg']
    ev = []

    def snap():
        for i in range(N):
            if t[i * SL:(i + 1) * SL] != pre[i * SL:(i + 1) * SL]:
                return i, bytes(t[i * SL:(i + 1) * SL]).hex()
        return 0xFFFF, '00' * SL

    def push(kind, a=0, b=0, c=0):
        i, h = snap()
        ev.append([kind, a & M32, b & M32, c & M32, i, h])

    types = [t[i * SL + 0x1C] for i in range(N)]
    idx, pas, top = find_slot(types, ptype, empty)
    info = dict(idx=idx, pas=pas, top=top, replaced_obj=False, replaced_null=False, events=0)
    if idx < 0:
        return dict(ret=M32, changed=[], trace=[], time_ok=True, msg_ok=True, objs_ok=True), info, pre
    b = idx * SL
    if pas in (2, 3):
        obj = u32(t, b + 4)
        if obj != 0:
            push(1, obj)
            push(2, obj, 1)
            info['replaced_obj'] = True
        else:
            info['replaced_null'] = True
        w32(t, b + 4, 0)
    w32(t, b + 8, inp['ammo'])
    w16(t, b + 0x16, inp['money'])
    f = (t[b + 0x1D] & 0xF8) | ((empty & 1) << 1)
    t[b + 0x1D] = f
    t[b + 0x1C] = ptype
    w32(t, b, 0)
    regen = now
    if ptype == 4:
        regen = now + 20000
    elif ptype == 5:
        regen = now + 120000
    elif ptype == 8:
        regen = now + 30000
    elif ptype in (9, 10):
        t[b + 0x1C] = 9
        regen = now + 1500
    elif ptype in (11, 12):
        t[b + 0x1C] = 11
        regen = now + 1500
    w32(t, b + 0xC, regen)
    w16(t, b + 0x18, inp['model'])
    push(3, 0 if msg is None else 1, 0 if msg is None else fnv32(msg))
    f = (f & 0x8F) | ((inp['text_ret'] << 4) & 0x70)
    t[b + 0x1D] = f
    w16(t, b + 0x10, pos16(inp['x']))
    w16(t, b + 0x12, pos16(inp['y']))
    w16(t, b + 0x14, pos16(inp['z']))
    push(4, idx)
    f = (f & 0xF7) | ((inp['visible_ret'] & 1) << 3)
    t[b + 0x1D] = f
    w32(t, b + 4, 0)
    if f & 8:
        push(5, idx, 4, M32)
        g = inp['give_obj']
        if g != 0:
            w32(t, b + 4, g)
            push(6, g)
    ref = int.from_bytes(t[b + 0x1A:b + 0x1C], 'little')
    ref = 1 if ref >= 0xFFFE else ref + 1
    w16(t, b + 0x1A, ref)
    info['events'] = len(ev)
    changed = [[i, bytes(t[i * SL:(i + 1) * SL]).hex()] for i in range(N) if t[i * SL:(i + 1) * SL] != pre[i * SL:(i + 1) * SL]]
    return dict(ret=((ref << 16) | idx) & M32, changed=changed, trace=ev, time_ok=True, msg_ok=True, objs_ok=True), info, pre


def stats_of(inp, want, info, pre):
    s = {}
    idx = info['idx']
    ptype, empty = inp['ptype'] & 0xFF, inp['empty'] & 0xFF
    s['kind'] = inp['kind']
    s['none'] = idx < 0
    s['pass1_front'] = idx >= 0 and info['pas'] == 1 and not info['top']
    s['pass1_top'] = idx >= 0 and info['pas'] == 1 and info['top']
    s['pass2'] = info['pas'] == 2
    s['pass3'] = info['pas'] == 3
    s['top_trigger_ptype13'] = ptype == 13
    s['top_trigger_ptype11'] = ptype == 11
    s['top_trigger_empty'] = empty != 0 and ptype not in (11, 13)
    s['top_failed_then_replace'] = info['top'] and info['pas'] in (2, 3)
    s['top_failed_none'] = info['top'] and idx < 0
    s['front_none'] = (not info['top']) and idx < 0
    s['replaced_obj'] = info['replaced_obj']
    s['replaced_null'] = info['replaced_null']
    s['pass2_with_t3'] = info['pas'] == 2 and any(pre[i * SL + 0x1C] in (4, 5) for i in range(N))
    if idx >= 0:
        s['slot_boundary'] = idx in BOUNDARY
        s['slot_first'] = idx == 0
        s['slot_last'] = idx == N - 1
        nfree = sum(1 for i in range(N) if pre[i * SL + 0x1C] == 0)
        s['multi_free'] = info['pas'] == 1 and nfree > 1
        s['pass3_type4'] = info['pas'] == 3 and pre[idx * SL + 0x1C] == 4
        s['pass3_type5'] = info['pas'] == 3 and pre[idx * SL + 0x1C] == 5
        s['empty_bit1'] = (empty & 1) == 1
        s['empty_nonzero_bit0_clear'] = empty != 0 and (empty & 1) == 0
        s['empty_zero_high_garbage'] = empty == 0 and (inp['empty'] >> 8) != 0
        s['ptype_high_garbage'] = (inp['ptype'] >> 8) != 0
        for p in (0, 4, 5, 8, 9, 10, 11, 12, 13):
            s['ptype_%d' % p] = ptype == p
        s['ptype_ge14'] = ptype >= 14
        s['ptype_ge128'] = ptype >= 128
        s['now_wraps'] = (inp['now'] + 1500 > M32 and ptype in (9, 10, 11, 12)) or (inp['now'] + 30000 > M32 and ptype == 8) or (inp['now'] + 20000 > M32 and ptype == 4) or (inp['now'] + 120000 > M32 and ptype == 5)
        s['model_gt_16bit'] = inp['model'] > 0xFFFF
        s['money_gt_16bit'] = inp['money'] > 0xFFFF
        s['msg_null'] = inp['msg'] is None
        s['text_ret_ge8'] = (inp['text_ret'] & M32) >= 8
        s['visible'] = (inp['visible_ret'] & 1) == 1
        s['visible_give_null'] = (inp['visible_ret'] & 1) == 1 and inp['give_obj'] == 0
        s['visible_give_obj'] = (inp['visible_ret'] & 1) == 1 and inp['give_obj'] != 0
        s['invisible'] = (inp['visible_ret'] & 1) == 0
        old = pre[idx * SL + 0x1D]
        s['old_flags_bit3'] = bool(old & 8)
        s['old_flags_text'] = bool(old & 0x70)
        s['old_flags_low'] = bool(old & 7)
        s['old_obj_nonzero_free'] = info['pas'] == 1 and u32(pre, idx * SL + 4) != 0
        ref0 = int.from_bytes(pre[idx * SL + 0x1A:idx * SL + 0x1C], 'little')
        s['ref_wrap'] = ref0 >= 0xFFFE
        s['ref_fffd'] = ref0 == 0xFFFD
        s['ref_ge8000'] = 0x8000 <= ref0 < 0xFFFE
        for j, key in enumerate(('x', 'y', 'z')):
            x = fl(inp[key])
            if math.isnan(x):
                s['coord_nan'] = True
            elif math.isinf(x):
                s['coord_inf'] = True
            elif abs(x * 8.0) >= 2.0 ** 63:
                s['coord_huge'] = True
            elif abs(x * 8.0) >= 32768:
                s['coord_int16_wrap'] = True
            if x < 0:
                s['coord_neg'] = True
            if pos16(inp[key]) != 0:
                s['coord_nonzero'] = True
    return s


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    want, info, pre = simulate(inp)
    return want, stats_of(inp, want, info, pre)


if __name__ == '__main__':
    import random
    # 1) conversion de coordenadas: x*8 truncado, palabra baja
    f32 = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    assert pos16(f32(0.125)) == 1 and pos16(f32(-0.125)) == 0xFFFF and pos16(f32(0.124)) == 0 and pos16(f32(4096.0)) == 0x8000
    assert pos16(f32(8192.0)) == 0 and pos16(f32(8191.875)) == 0xFFFF and pos16(0x7FC00000) == 0 and pos16(0x7F800000) == 0 and pos16(f32(1.2e18)) == 0 and pos16(0x7F7FFFFF) == 0
    rnd = random.Random(7)
    for _ in range(20000):
        v = (rnd.random() - 0.5) * 20000
        assert pos16(f32(v)) == int(struct.unpack('<f', struct.pack('<f', v))[0] * 8.0) & 0xFFFF

    def slot(type_, obj=0, ref=0, flags=0, fill=0x11):
        b = bytearray([fill] * SL)
        b[0x1C] = type_
        w32(b, 4, obj)
        w16(b, 0x1A, ref)
        b[0x1D] = flags
        return bytes(b).hex()
    base = dict(kind=0, x=f32(10.0), y=f32(-3.5), z=f32(0.8), model=0x1234, ptype=2, ammo=77, money=0x12345, empty=0, msg=None, text_ret=1, visible_ret=1, give_obj=0,
                now=1000, filler_type=1, slots=[], eax=0, ecx=0, edx=0, stack_seed=0)
    # 2) hueco libre mas bajo (busqueda desde el principio) en el 7, con otro libre en el 600; referencia 0xFFFE vuelve a 1
    inp = dict(base, slots=[[600, slot(0)], [7, slot(0, 0xDEADBEEF, 0xFFFE, 0xFF)]], visible_ret=0x100, ptype=0x102)
    want, info, pre = simulate(inp)
    assert want['ret'] == (1 << 16 | 7) and info['pas'] == 1 and not info['top'] and [c[0] for c in want['changed']] == [7]
    post = bytes.fromhex(want['changed'][0][1])
    assert post[0x1C] == 2 and u32(post, 8) == 77 and u32(post, 0xC) == 1000 and int.from_bytes(post[0x16:0x18], 'little') == 0x2345 and int.from_bytes(post[0x18:0x1A], 'little') == 0x1234
    assert int.from_bytes(post[0x10:0x12], 'little') == 80 and int.from_bytes(post[0x12:0x14], 'little') == (-28 & 0xFFFF) and int.from_bytes(post[0x14:0x16], 'little') == 6
    assert post[0x1D] == 0x90, post[0x1D]            # 0xFF -> F8 (isEmpty 0) -> 88 | 10 (texto 1) -> 98 -> bit 3 = 0 (invisible) -> 90
    assert [e[0] for e in want['trace']] == [3, 4] and u32(post, 4) == 0
    # 3) desde el final con ptype 13 o isEmpty != 0
    for kw in (dict(ptype=13), dict(empty=0x80), dict(ptype=11)):
        w2, i2, _ = simulate(dict(inp, **kw))
        assert i2['idx'] == 600 and i2['top'], kw
    w3, i3, _ = simulate(dict(inp, empty=0x100))
    assert i3['idx'] == 7 and not i3['top']                     # solo cuenta el byte bajo
    # 4) sin libres: sustituye el primer dinero (con objeto: Remove + destructor), nulo el siguiente, y Add si es visible con objeto
    inp = dict(base, slots=[[5, slot(4, 0x0B100002)], [9, slot(8, 0x0B100001)], [300, slot(8, 0)]], ptype=8, visible_ret=1, give_obj=0x0B100003, msg='PROP_3', empty=0)
    want, info, pre = simulate(inp)
    assert info['pas'] == 2 and info['idx'] == 9 and info['replaced_obj']
    assert [e[:4] for e in want['trace']] == [[1, 0x0B100001, 0, 0], [2, 0x0B100001, 1, 0], [3, 1, fnv32('PROP_3'), 0], [4, 9, 0, 0], [5, 9, 4, M32], [6, 0x0B100003, 0, 0]]
    assert want['trace'][0][4] == 0xFFFF and want['trace'][1][4] == 0xFFFF and want['trace'][2][4] == 9
    post = bytes.fromhex(want['changed'][0][1])
    assert post[0x1C] == 8 and u32(post, 0xC) == 31000 and u32(post, 4) == 0x0B100003 and want['ret'] == (1 << 16 | 9) | 0
    # 5) sin libres ni dinero: primer tipo 4/5; sin nada: -1 y tabla intacta
    w5, i5, _ = simulate(dict(inp, slots=[[50, slot(5, 0)], [20, slot(4, 0x0B100004)], [3, slot(9)]], ptype=4))
    assert i5['pas'] == 3 and i5['idx'] == 20 and w5['trace'][0][:3] == [1, 0x0B100004, 0] and bytes.fromhex(w5['changed'][0][1])[0x1C] == 4
    w6, i6, _ = simulate(dict(inp, slots=[[3, slot(9)], [4, slot(12)]]))
    assert w6['ret'] == M32 and w6['changed'] == [] and w6['trace'] == []
    # 6) tipos 9/10 -> 9 y 11/12 -> 11 (+1500); 5 -> +120000
    for pt, tp, dt in ((10, 9, 1500), (12, 11, 1500), (5, 5, 120000), (9, 9, 1500), (11, 11, 1500), (0xFF, 0xFF, 0)):
        w7, i7, _ = simulate(dict(base, slots=[[0, slot(0)]], ptype=pt, now=0xFFFFFFFF))
        p7 = bytes.fromhex(w7['changed'][0][1])
        assert p7[0x1C] == tp and u32(p7, 0xC) == (0xFFFFFFFF + dt) & M32, (pt, p7[0x1C], u32(p7, 0xC))
    print('ok')
