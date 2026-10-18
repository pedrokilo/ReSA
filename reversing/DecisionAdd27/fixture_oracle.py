"""Oraculo independiente del banco DecisionAdd27: CDecision::Add (0x600600) y CDecisionMakerTypes::AddEventResponse (0x6044C0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

CDecision::Add, ECX = this (edi), args: [esp+10h]=taskId, [esp+14h]=chances (ebx), [esp+18h]=flags; `ret 0Ch` (tras push ebx/esi/edi):
  600605 esi = 0 ; edx = this+30h
  600610 cmp dword [edi+esi*4], -1 ; jne -> siguiente (ranura con tarea != -1 esta ocupada)
  60061C ecx = edi+esi*4+18h ; eax=0..3: cmp byte [ecx+eax],0 ; ja -> siguiente (probabilidad > 0 SIN signo: 0x80..0xFF ocupan)
  600630 eax=0..1: cmp byte [edx+eax],0 ; jne -> siguiente (cualquier byte != 0 en los bools ocupa)
  600641 [edi+esi*4]=taskId
  60064C 4 veces: fld dword [ebx+4j] ; call 821B40 (_ftol2) ; mov byte [edi+esi*4+18h+j],al     (solo el byte bajo del truncado a int64)
  60067B eax=flags: cmp dword [eax],0 ; setne cl -> byte [edi+esi*2+30h] ; ecx=[eax+4] ; test ecx,ecx ; setne dl -> byte [edi+esi*2+31h]
  600695 ret 0Ch       |  si las 6 ranuras estan ocupadas: 6006A8 ret 0Ch sin escribir nada ; chances y flags nunca se escriben
_ftol2 0x821B40 (version x87 del CRT): fistp qword (REDONDEO al mas cercano segun FCW) ; eax=low32(q), edx=float de x ; si eax==0 y (high32(q)&7FFFFFFF)==0 sale con eax=0
  (cubre x en [-0.5,0.5] e indefinido 0x8000000000000000 de NaN/inf/|x|>=2^63) ; si no: diff = x - q (24 bits, exacto) ; x negativa: ecx=bits(diff)^80000000, ecx+=7FFFFFFF, adc eax,0 ;
  x >= 0: ecx=bits(diff), ecx+=7FFFFFFF, sbb eax,0.  Resultado = truncado hacia cero. Aqui se implementa el algoritmo literal y se contrasta con math.trunc en la autoprueba.

CDecisionMakerTypes::AddEventResponse, ECX = this ; args (en la entrada): +4 decisionMakerIndex, +8 eventType, +0Ch taskId, +10h chances, +14h flags ; `ret 14h`:
  6044C0 reempuja flags, chances, taskId ; edx = decisionMakerIndex*41 (imul 29h) ; eax = [ecx+eventType*4+0C034h] ; eax += edx ; eax *= 3Ch ; ecx = eax+ecx+4 ;
  6044EA call 600600 (CDecision::Add) ; ret 14h.   Toda la aritmetica es de 32 bits (da la vuelta), sin comprobar limites.
"""
import math
import struct
import sys

M32 = 0xFFFFFFFF
CASES_ADD = 6144
CASES_AER = 4608
NAME_ADD = 'CDecision::Add'
NAME_AER = 'CDecisionMakerTypes::AddEventResponse'
NAMES = [NAME_ADD, NAME_AER]
CASES = {NAME_ADD: CASES_ADD, NAME_AER: CASES_AER}
OBJ = 0x3C
K_MEM = 0x18000
K_THIS = 0x3000
K_EVENT_TABLE = 0xC034


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def ftol_byte(bits):
    """Byte bajo del resultado de _ftol2 (0x821B40) para el float de bits `bits`, siguiendo el algoritmo del EXE (FCW al mas cercano)."""
    x = fl(bits)
    if math.isnan(x) or math.isinf(x):
        return 0                                  # fistp -> indefinido 0x8000000000000000: eax=0, high=80000000 -> sale con eax=0
    q = round(x)                                  # fistp: redondeo al par
    if q >= 1 << 63 or q < -(1 << 63):
        return 0                                  # indefinido
    q64 = q & 0xFFFFFFFFFFFFFFFF
    eax, high = q64 & M32, q64 >> 32
    if eax == 0 and (high & 0x7FFFFFFF) == 0:
        return 0
    diff = f32bits(x - float(q))                  # x - q exacto en 24 bits
    if bits & 0x80000000:                         # test edx,edx ; jns
        carry = ((diff ^ 0x80000000) + 0x7FFFFFFF) > M32
        eax = (eax + carry) & M32                 # adc eax,0
    else:
        carry = (diff + 0x7FFFFFFF) > M32
        eax = (eax - carry) & M32                 # sbb eax,0
    return eax & 0xFF


def add_model(pre, task, chances, flags):
    """Devuelve (objeto final, indice de ranura rellenada o -1, motivos por los que se salto cada ranura anterior)."""
    obj = bytearray(pre)
    skipped = []
    for i in range(6):
        t = s32(int.from_bytes(obj[4 * i:4 * i + 4], 'little'))
        probs = list(obj[0x18 + 4 * i:0x18 + 4 * i + 4])
        bools = list(obj[0x30 + 2 * i:0x30 + 2 * i + 2])
        if t != -1:
            skipped.append(('task', t, probs, bools))
            continue
        if any(p > 0 for p in probs):
            skipped.append(('prob', t, probs, bools))
            continue
        if any(b != 0 for b in bools):
            skipped.append(('bool', t, probs, bools))
            continue
        obj[4 * i:4 * i + 4] = (task & M32).to_bytes(4, 'little')
        for j in range(4):
            obj[0x18 + 4 * i + j] = ftol_byte(chances[j])
        for k in range(2):
            obj[0x30 + 2 * i + k] = 1 if (flags[k] & M32) != 0 else 0
        return bytes(obj), i, skipped
    return bytes(obj), -1, skipped


def add_stats(inp, pre, post, slot, skipped, stats):
    free = 0
    for i in range(6):
        t = s32(int.from_bytes(pre[4 * i:4 * i + 4], 'little'))
        if t == -1 and not any(pre[0x18 + 4 * i:0x18 + 4 * i + 4]) and not any(pre[0x30 + 2 * i:0x30 + 2 * i + 2]):
            free += 1
    stats['free_count'] = free
    stats['written'] = slot >= 0
    stats['slot'] = slot
    stats['no_free'] = slot < 0
    stats['kind'] = inp['kind']
    for why, t, probs, bools in skipped:
        nz = (t != -1) + sum(1 for p in probs if p) + sum(1 for b in bools if b)
        if why == 'task':
            stats['skip_task'] = True
        if why == 'prob':
            stats['skip_prob_any'] = True
            for j in range(4):
                if probs[j]:
                    stats['skip_prob%d' % j] = True
            if any(p >= 0x80 for p in probs):
                stats['skip_prob_high'] = True
        if why == 'bool':
            for k in range(2):
                if bools[k]:
                    stats['skip_bool%d' % k] = True
            if any(b not in (0, 1) for b in bools):
                stats['skip_bool_noncanon'] = True
        if nz == 1:
            stats['near_free_skip'] = True
    if slot < 0:
        return
    task = s32(inp['task'])
    stats['task_minus1'] = task == -1
    stats['task_min'] = task == -(1 << 31)
    stats['task_max'] = task == (1 << 31) - 1
    stats['task_ge_65536'] = task >= 0x10000
    stats['task_200'] = task == 200
    f0, f1 = (inp['flags'][0] & M32) != 0, (inp['flags'][1] & M32) != 0
    stats['flags_%d%d' % (f0, f1)] = True
    for v in inp['flags']:
        v &= M32
        if v != 0 and (v & 0xFF) == 0:
            stats['flag_low_byte_zero_nonzero'] = True
        if v == 0x80000000:
            stats['flag_sign_only'] = True
        if v not in (0, 1) and v != 0:
            stats['flag_not_0_1'] = True
    for j, bits in enumerate(inp['chances']):
        x = fl(bits)
        byte = post[0x18 + 4 * slot + j]
        stats['pos%d_nonzero_byte' % j] = stats.get('pos%d_nonzero_byte' % j, False) or byte != 0
        if math.isnan(x):
            stats['ch_nan'] = True
        elif math.isinf(x):
            stats['ch_inf'] = True
        elif x >= 2.0 ** 63 or x < -(2.0 ** 63):
            stats['ch_huge'] = True
        else:
            if x < 0:
                stats['ch_neg'] = True
                if byte:
                    stats['ch_neg_nonzero_byte'] = True
            if 256 <= x:
                stats['ch_ge256'] = True
                if byte:
                    stats['ch_ge256_nonzero_byte'] = True
            if 255 <= x < 256:
                stats['ch_band255'] = True
            if 65536 <= x and byte:
                stats['ch_ge65536_nonzero_byte'] = True
            if 0 < abs(x) < 1:
                stats['ch_sub1'] = True
            if 0 < x < 255 and x != math.floor(x):
                stats['ch_frac'] = True
            if bits == 0x80000000:
                stats['ch_negzero'] = True
            if bits != 0 and (bits & 0x7F800000) == 0:
                stats['ch_denormal'] = True
        if byte >= 0x80:
            stats['byte_ge128'] = True
        if byte == 0xFF:
            stats['byte_ff'] = True
        if byte == 0x80:
            stats['byte_80'] = True
        if byte == 0:
            stats['byte_zero'] = True


def expected_add(inp, case):
    pre = bytes.fromhex(inp['pre'])
    assert len(pre) == OBJ
    post, slot, skipped = add_model(pre, s32(inp['task']), inp['chances'], inp['flags'])
    stats = {}
    add_stats(inp, pre, post, slot, skipped, stats)
    return {'obj': post.hex(), 'guard_ok': True, 'args_ok': True}, stats


def window_of(this_off, dm, idx):
    return (this_off + 4 + (((idx + dm * 41) & M32) * 60 & M32)) & M32


def table_of(this_off, ev):
    return (this_off + K_EVENT_TABLE + ((ev & M32) * 4 & M32)) & M32


def expected_aer(inp, case):
    pre = bytes.fromhex(inp['pre'])
    this_off, dm, ev, idx = inp['this_off'], s32(inp['dm']), s32(inp['ev_type']), inp['ev_index'] & M32
    w, p = window_of(this_off, dm, idx), table_of(this_off, ev)
    if w != inp['window_off'] or p != inp['table_off']:
        raise RuntimeError('Fixture inconsistente en %d: W=%#x/%#x P=%#x/%#x' % (case, w, inp['window_off'], p, inp['table_off']))
    if not (0x40 <= w <= K_MEM - OBJ - 0x40) or not (0x40 <= p <= K_MEM - 0x44) or not (p + 4 <= w or p >= w + OBJ):
        raise RuntimeError('Fixture fuera de la arena o con P dentro de la ventana en %d' % case)
    post, slot, skipped = add_model(pre, s32(inp['task']), inp['chances'], inp['flags'])
    stats = {}
    add_stats(inp, pre, post, slot, skipped, stats)
    stats['mode'] = inp['mode']
    stats['dm_neg'] = dm < 0 and abs(dm) < (1 << 20)
    stats['dm_ge20'] = 20 <= dm < (1 << 20)
    stats['dm_wrap'] = abs(dm) >= (1 << 20)
    stats['idx_neg'] = -(1 << 20) < s32(idx) < 0
    stats['idx_ge41'] = 41 <= s32(idx) < (1 << 20)
    stats['idx_wrap'] = abs(s32(idx)) >= (1 << 20)
    stats['ev_wrap'] = abs(ev) >= (1 << 20)
    stats['ev_real_table'] = 0 <= ev < 96
    stats['ev_elsewhere'] = not (0 <= ev < 96) and abs(ev) < (1 << 20)
    stats['window_nongrid'] = (w - this_off - 4) % 0x3C != 0
    stats['window_below_this'] = w < this_off
    return {'window': post.hex(), 'outside_ok': True, 'first_outside': -1, 'args_ok': True, 'generator_ok': True}, stats


def expected(function, inp, case):
    if function == NAME_ADD:
        return expected_add(inp, case)
    if function == NAME_AER:
        return expected_aer(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    # 1) el algoritmo literal de _ftol2 coincide con el truncado hacia cero (byte bajo) en valores de todo tipo
    import random
    rnd = random.Random(1)
    for _ in range(200000):
        bits = rnd.getrandbits(32)
        x = fl(bits)
        if math.isnan(x) or math.isinf(x):
            assert ftol_byte(bits) == 0
            continue
        t = math.trunc(x)
        want = 0 if (t >= 1 << 63 or t < -(1 << 63)) else t & 0xFF
        assert ftol_byte(bits) == want, (hex(bits), ftol_byte(bits), want)
    for v, b in ((255.9, 255), (256.0, 0), (-1.0, 255), (-0.5, 0), (0.99999994, 0), (-0.99999994, 0), (65536.0 + 7, 7), (-255.5, 1), (1.5, 1), (2.5, 2), (-1.5, 255)):
        assert ftol_byte(f32bits(v)) == b, (v, ftol_byte(f32bits(v)), b)
    # 2) Add: primera libre (ranura 1), las demas intactas, flags como dword
    pre = bytearray(OBJ)
    for i in range(6):
        pre[4 * i:4 * i + 4] = (0xFFFFFFFF).to_bytes(4, 'little')
    pre[0:4] = (7).to_bytes(4, 'little')                    # ranura 0 ocupada por la tarea
    inp = dict(kind=0, pre=bytes(pre).hex(), task=-5, chances=[f32bits(255.9), f32bits(256.0), f32bits(-1.0), 0x7FC00000], flags=[0x80000000, 0], eax=0, edx=0, stack_seed=0)
    want, st = expected_add(inp, 0)
    post = bytes.fromhex(want['obj'])
    assert st['slot'] == 1 and post[4:8] == (0xFFFFFFFB).to_bytes(4, 'little')
    assert list(post[0x1C:0x20]) == [255, 0, 255, 0] and list(post[0x32:0x34]) == [1, 0] and post[0:4] == pre[0:4]
    # 3) ninguna libre
    pre2 = bytearray(pre)
    for i in range(6):
        pre2[0x18 + 4 * i] = 0x80
    want, st = expected_add(dict(inp, pre=bytes(pre2).hex()), 0)
    assert st['no_free'] and want['obj'] == bytes(pre2).hex() and st['skip_prob_high']
    # 4) AddEventResponse: la vuelta en 32 bits (k*2^30) no cambia W ni P
    base = (K_THIS, 1, 3, 2)
    w0, p0 = window_of(K_THIS, 1, 2), table_of(K_THIS, 3)
    assert w0 == K_THIS + 4 + (2 + 41) * 60 and p0 == K_THIS + K_EVENT_TABLE + 12
    for k in (1, 2, 3, -1, -2):
        assert window_of(K_THIS, 1 + (k << 30), 2) == w0 and window_of(K_THIS, 1, 2 + (k << 30)) == w0 and table_of(K_THIS, 3 + (k << 30)) == p0
    print('ok')
