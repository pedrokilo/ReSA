"""Oraculo independiente del banco PartnerChatSeq30: CTaskComplexPartnerChat::GetPartnerSequence (0x684380).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco.

0x684380 (thiscall, ECX=this -> ESI, sin argumentos, `ret`; marco SEH propio con fs:[0]; guarda EBX/EBP/ESI/EDI):
  684395..  esi=this ; eax = 61A5A0(0x40) ; edi = eax ? 632BD0(eax) : 0                                  ; CTaskComplexSequence (edi)
  6843C3    ebx=-1 ; eax = 61A5A0(0x28) ; si eax: 66B890(eax, [esi+38h]=partner, [86FC7C]=0.5f, 3A83126Fh=0.001f) ; 632D10(edi, eax-o-0)   ; AddTask(seq, turn)
  68440B    eax = movsx byte [esi+5Bh] ; eax <<= 4 ; fild ; fadd [esi+48h] ; fadd [esi+44h] ; fadd [esi+40h]  ; x87 con FCW de 24 bits: ((n<<4)+z)+y)+x, cada suma redondeada a 24 bits
  684423    call 821B40 (_ftol: fistp qword + correccion = truncar hacia 0; NaN/inf/|x|>=2^63 -> 0x8000000000000000; eax = parte baja)
  684428    eax &= 7Fh ; fild ; fmul [858B88]=1/128 (r) ; fld [858624]=1.0 ; fsub st,st(1) (1-r) ; fstp float [esp+10h] ; fmul [859A94]=3000 ; fadd [8707FC]=3500 ; call _ftol -> ebp = t1
            fld [esp+10h] ; fmul 3000 ; fadd 3500 ; call _ftol -> ebx = t2     (r, 1-r y los productos son exactos en 24 bits: s/128 con s < 128)
  68446F    al=[esi+58h] (m_leadSpeaker, BYTE) ; test al,al ; jne 68448F
            si es 0: ecx=[esi+38h] (partner, SIN comprobar nulo) ; ecx=[ecx+47Ch] (m_pIntelligence) ; push 4B4h ; call 600EE0 (FindTaskByType) ; dl=[eax+74h] ; [esi+74h]=dl  (copia el BYTE)
  68448F    al=[esi+74h] ; test al,al ; je 6845D6                                                                  ; rama de conversacion si el BYTE != 0
  conversacion (68449A):
            eax=movsx byte [esi+5Bh] (n) ; ecx=4n ; bx = word [0B613F8h - 4n] (contexto A = palabra sin signo en 0xB613EC + 2*(6-2n)) ; bp = word [0B613FAh - 4n] (contexto B, 7-2n)
            edx = movsx word [0B613E8h] (longitud) ; si (3 - n) == edx: byte [esi+5Bh] = 0
            al=[esi+58h] ; test ; push 20h
            lead != 0: new(0x20) ; 682CB0(this=obj, esJefe=1, partner=[esi+38h], etapa=movsx [esi+5Bh], ctx=ebx) ; AddTask(seq,obj) ; new(0x20) ; 682CB0(0, partner, etapa, ebx) ; AddTask(seq,obj2)
            lead == 0: new(0x20) ; 682CB0(0, partner, etapa, ebp) ; AddTask ; new(0x20) ; 682CB0(1, partner, etapa, ebp) ; AddTask
  sin conversacion (6845D6):
            lead != 0: new(0x34) ; 5F65A0(obj, t1=ebp) [CTaskSimpleChat(duracion)] ; AddTask ; new(0x20) ; 62F310(obj, t2=ebx, 0, 0, 8.0f) [StandStill] ; AddTask
            lead == 0: new(0x20) ; 62F310(obj, t1=ebp, 0, 0, 8.0f) ; AddTask ; new(0x34) ; 5F65A0(obj, t2=ebx) ; AddTask
  fin       eax = edi (la secuencia) ; restaura SEH ; ret
Cualquier llamador: CTaskComplexPartner::CreateNextSubTask 0x683B46 (`call [eax+34h]`) -> 0x6840AC : `mov ecx,[esp+30h]` (ECX sobrescrito), EDX no se lee, EAX = retorno.

Colaboradores (desensamblados en 0x61A500, 0x632BD0, 0x632D10, 0x66B890, 0x682CB0, 0x62F310, 0x5F65A0, 0x61ABE0, 0x61A390, 0x61A3B0):
  Pool (0x61A500): edi = last+1 ; si edi == cap -> last = 0 y, si ya se dio la vuelta, devuelve 0 (con last = 0), si no al = 1 ; prueba el bit 7 del estado ; si libre:
      estado = (estado+1) & 7F (bit 7 limpio = ocupado, id incrementado) ; devuelve storage + last*128 ; last = indice. Orden de busqueda: last+1..cap-1, luego 0..cap-1.
  Secuencia (0x632BD0, tras 0x61A3B0 {[+4]=0, [+8]=0, vtable}): [+0C]=0, [+30]=0, [+34]=0, byte[+38]=0, [+3C]=0, 8 dwords a 0 desde +10 ; bytes +39..+3B no se escriben.
  AddTask (0x632D10): primera de las 8 posiciones [+10..+2C] con 0 -> la tarea (aunque sea 0).
  Turn (0x66B890, tras 0x61A3B0): [+0C]=partner ; byte[+10]=1 ; si partner != 0 -> RegisterReference(0x571B70)(&[+0C]) ; [+20]=0.5f ; [+24]=0.001f ; el resto queda como estaba (0xCD).
  Chat (0x682CB0, tras 0x61A3B0): byte[+0C]=esJefe ; [+14]=etapa ; [+10]=partner ; vtable ; si partner != 0 -> RegisterReference(&[+10]) ; word[+18]=contexto ; byte[+1A]=0 ; [+1C]=0.
  StandStill (0x62F310, tras 0x61A390 {[+4]=0}): [+08]=tiempo ; [+0C]=0 ; [+10]=0 ; byte[+14]=0 ; byte[+15]=0 ; byte[+18]=looped ; byte[+19]=useAnimIdle ; [+1C]=mezcla (8.0f).
  SimpleChat (0x5F65A0 -> 0x61ABE0 tras 0x61A390): byte[+0C]=((hold&1)<<2)|(byte&F8) ; [+10]=grupo 0 ; [+14]=anim 0xC ; [+18]=4.0f ; [+1C]=-4.0f ; [+20]=duracion ; [+24]=0 ; [+28]=0 ; byte[+2C]=0 ; byte[+2D]=0 ;
      word[+30]=0x1A0 ; [+08]=0 ; vtable 86C78C.
  RegisterReference (cuerpo en CEntity::RegisterReferenceWithResidue): no hace nada si el tipo es edificio, no temporal, no procedural y sin IPL, ni si ya hay una referencia con el mismo
      m_ppEntity ; si no: toma pEmptyList, la inserta en cabeza de m_pReferences con m_ppEntity.
Las vtables (palabra 0 de cada tarea) se sustituyen por GetTaskType: Secuencia 0xF4 (632C60), Turn 0x398 (66B900), Chat 0x4C2 (682D30), SimpleChat 0x1A0 (word +0x30), StandStill 0xCB (62F350).
FindTaskByType (falso del banco): registra [etiqueta de this = la CPedIntelligence falsa 'I+0', tipo] y devuelve la tarea falsa 'O+0' cuyo byte +0x74 fija el caso.
"""
import math
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskComplexPartnerChat::GetPartnerSequence'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
CD = 0xCDCDCDCD
SLOT_WORDS = 32
TYPE_SEQ, TYPE_TURN, TYPE_CHAT, TYPE_SCHAT, TYPE_STAND = 0xF4, 0x398, 0x4C2, 0x1A0, 0xCB
F_HALF, F_MAXHEADING, F_BLEND4, F_BLENDM4, F_BLEND8 = 0x3F000000, 0x3A83126F, 0x40800000, 0xC0800000, 0x41000000
REFS_OFF = 0x24
FIND_TYPE = 0x4B4
OFF_N, OFF_CONV = 0x5B, 0x74
INF = math.inf


def pool_alloc(win, pos, last):
    """Siguiente reserva: devuelve (rango en la ventana o None, nuevo cursor, nuevo pos)."""
    if pos < len(win):
        return pos, win[pos][0], pos + 1
    return None, 0, pos          # sin ranura libre: el cursor queda en 0


# ------------------------------------------------------------------------------------------------ x87 (24 bits de mantisa, exponente extendido)
def bits_to_float(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def round24(x):
    """Redondeo al mas cercano (empates al par) a 24 bits de mantisa, sin limite de exponente (x87 con FCW de 24 bits)."""
    if x != x or x in (INF, -INF) or x == 0:
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0) / 16777216.0, e)


def ftol_low32(x):
    """_ftol (0x821B40): trunca hacia 0; NaN/inf/|x| >= 2^63 -> 0x8000000000000000. Devuelve EAX (parte baja)."""
    if x != x or abs(x) >= 2.0 ** 63:
        v = -(1 << 63)
    else:
        v = int(x)
    return v & M32


def seed_of(n, pt_bits):
    """(semilla & 0x7F, acumulado x87, acumulado exacto en doble precision)."""
    x, y, z = (bits_to_float(b) for b in pt_bits)
    acc = float(n << 4)
    acc = round24(acc + z)
    acc = round24(acc + y)
    acc = round24(acc + x)
    exact = float(n << 4) + z + y + x
    return ftol_low32(acc) & 0x7F, acc, ftol_low32(exact) & 0x7F


def durations(s):
    r = Fraction(s, 128)
    t1 = int(r * 3000 + 3500)
    t2 = int((1 - r) * 3000 + 3500)
    return t1, t2


# ------------------------------------------------------------------------------------------------ bytes de las tareas nuevas (ranura de 0x80 bytes rellena con 0xCD)
def seq_words(labels):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0
    for k in range(4, 12):
        w[k] = 0
    slots = [0] * 8
    for label in labels:          # AddTask: primera posicion libre (con 0); AddTask(0) deja el 0 en su sitio
        for i in range(8):
            if slots[i] == 0:
                slots[i] = label if label is not None else 0
                break
    for i in range(8):
        w[4 + i] = slots[i]
    w[12] = 0
    w[13] = 0
    w[14] = 0xCDCDCD00           # byte [+38] = 0 ; bytes +39..+3B sin escribir
    w[15] = 0
    return w


def turn_words(partner_label):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = partner_label if partner_label is not None else 0
    w[4] = 0xCDCDCD01            # byte [+10] = 1
    w[8] = F_HALF
    w[9] = F_MAXHEADING
    return w


def chat_words(is_chatter, partner_label, stage, ctx):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0xCDCDCD00 | (is_chatter & 0xFF)       # byte [+0C]
    w[4] = partner_label if partner_label is not None else 0
    w[5] = stage & M32
    w[6] = 0xCD000000 | (ctx & 0xFFFF)            # word [+18] = contexto ; byte [+1A] = 0 ; byte [+1B] sin escribir
    w[7] = 0
    return w


def schat_words(duration):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = 0
    w[3] = 0xCDCDCDC8            # byte [+0C] = ((0&1)<<2) | (0xCD & 0xF8)
    w[4] = 0                     # grupo
    w[5] = 0xC                   # anim IDLE_CHAT
    w[6] = F_BLEND4
    w[7] = F_BLENDM4
    w[8] = duration & M32
    w[9] = 0
    w[10] = 0
    w[11] = 0xCDCD0000           # bytes [+2C], [+2D] = 0
    w[12] = 0xCDCD01A0           # word [+30] = 0x1A0
    return w


def stand_words(time):
    w = [CD] * SLOT_WORDS
    w[0] = None
    w[1] = 0
    w[2] = time & M32
    w[3] = 0
    w[4] = 0
    w[5] = 0xCDCD0000            # bytes [+14], [+15] = 0
    w[6] = 0xCDCD0000            # bytes [+18] (looped), [+19] (useAnimIdle) = 0
    w[7] = F_BLEND8
    return w


def expected_chat(inp, case):
    pool = inp['pool']
    cap, last, win, free_total = pool['cap'], pool['last'], pool['win'], pool['free_total']
    if len(win) != min(4, free_total):
        raise RuntimeError('Fixture inconsistente en %d: ventana %d, libres %d' % (case, len(win), free_total))
    if inp['refs_off'] != REFS_OFF:
        raise RuntimeError('Desplazamiento de m_pReferences inesperado: %#x' % inp['refs_off'])
    lead = (inp['lead'] & 0xFF) != 0
    has_partner = bool(inp['has_partner'])
    if not lead and not has_partner:
        raise RuntimeError('Fixture invalida en %d: sin primero en hablar y sin partner (el EXE leeria [0x47C])' % case)
    conv0 = (inp['conv'] if lead else inp['other_conv']) & 0xFF
    conv_path = conv0 != 0
    if bool(inp['conv_eff']) != conv_path:
        raise RuntimeError('Fixture inconsistente en %d: conv_eff' % case)
    n = inp['n']
    if not -128 <= n <= 127:
        raise RuntimeError('n fuera de rango')
    s, acc, s_exact = seed_of(n, inp['pt'])
    t1, t2 = durations(s)
    # --- conversacion
    n_after = n
    ctx_a = ctx_b = 0
    if conv_path:
        if not -1 <= n <= 5:
            raise RuntimeError('Fixture invalida en %d: n=%d con conversacion toca memoria no preparada' % (case, n))
        mem = {}
        for idx, val in inp['conv_writes']:
            mem[idx] = val & 0xFFFF
        ctx_a, ctx_b, length = mem[6 - 2 * n], mem[7 - 2 * n], mem[-2]
        length_signed = length - 0x10000 if length >= 0x8000 else length
        if 3 - n == length_signed:
            n_after = 0
    # --- reservas (orden: secuencia, turn, X, Y)
    pos = 0
    cur = last
    ranks = []
    for _ in range(4):
        r, cur, pos = pool_alloc(win, pos, cur)
        ranks.append(r)
    seq_rank, turn_rank, x_rank, y_rank = ranks
    if seq_rank is None:
        raise RuntimeError('Fixture sin ninguna ranura libre en %d (el EXE se detendria)' % case)
    label = lambda k, off=0: ('W%d+%d' % (k, off)) if k is not None else None
    partner_label = 'E+0' if has_partner else None
    # --- cambios de estado del pool
    changes = []
    for r in ranks:
        if r is not None:
            idx, st = win[r]
            changes.append([idx, ((st & 0x7F) + 1) & 0x7F])
    changes.sort()
    # --- tareas X e Y
    if conv_path:
        if lead:
            kinds = [('chat', 1, ctx_a), ('chat', 0, ctx_a)]
        else:
            kinds = [('chat', 0, ctx_b), ('chat', 1, ctx_b)]
    elif lead:
        kinds = [('schat', t1), ('stand', t2)]
    else:
        kinds = [('stand', t1), ('schat', t2)]
    # --- referencias: Turn (+0xC) y cada CTaskComplexChat (+0x10), en orden de construccion
    chain_idx = inp['refs']['chain']
    old_chain = []
    if has_partner:
        for j in range(inp['refs_len']):
            old_chain.append(['N%d' % j, inp['node_pp'][j]])
    partner_refs = [list(e) for e in old_chain]
    regs = []
    if turn_rank is not None:
        regs.append((turn_rank, 12))
    for rank, kind in ((x_rank, kinds[0]), (y_rank, kinds[1])):
        if rank is not None and kind[0] == 'chat':
            regs.append((rank, 16))
    building_skip = inp['etype'] == 1 and not inp['temp'] and not inp['proc'] and inp['ipl'] == 0
    registered = dup = skipped = 0
    ref_changes = []
    used = 0
    if has_partner:
        for rank, off in regs:
            pp = label(rank, off)
            if building_skip:
                skipped += 1
                continue
            if any(e[1] == pp for e in partner_refs):
                dup += 1
                continue
            node = chain_idx[used]
            ref_changes.append([node, partner_refs[0][0] if partner_refs else 0, pp])
            partner_refs.insert(0, ['R%d' % node, pp])
            used += 1
            registered += 1
    empty_head = 'R%d' % chain_idx[used] if chain_idx[used] >= 0 else 0
    ref_changes.sort()
    # --- tareas
    tasks = []
    seq_labels = [label(turn_rank), label(x_rank), label(y_rank)]
    tasks.append({'type': TYPE_SEQ, 'words': seq_words(seq_labels), 'slot': seq_rank})
    if turn_rank is not None:
        tasks.append({'type': TYPE_TURN, 'words': turn_words(partner_label), 'slot': turn_rank})
    for rank, kind in ((x_rank, kinds[0]), (y_rank, kinds[1])):
        if rank is None:
            continue
        if kind[0] == 'chat':
            tasks.append({'type': TYPE_CHAT, 'words': chat_words(kind[1], partner_label, n_after, kind[2]), 'slot': rank})
        elif kind[0] == 'schat':
            tasks.append({'type': TYPE_SCHAT, 'words': schat_words(kind[1]), 'slot': rank})
        else:
            tasks.append({'type': TYPE_STAND, 'words': stand_words(kind[1]), 'slot': rank})
    # --- `this` y llamadas
    this_changes = []
    if n_after != n:
        this_changes.append([OFF_N, 0])
    if not lead and conv0 != (inp['conv'] & 0xFF):
        this_changes.append([OFF_CONV, conv0])
    this_changes.sort()
    finds = [['I+0', FIND_TYPE]] if not lead else []
    want = {'ret': label(seq_rank, 0), 'last_after': cur, 'state_changes': changes, 'tasks': tasks, 'pool_outside_ok': True, 'first_pool_outside': -1,
            'this_changes': this_changes, 'partner_other_ok': True, 'int_ok': True, 'other_ok': True, 'nodes_ok': True, 'conv_ok': True, 'finds': finds,
            'partner_refs': partner_refs, 'empty_head': empty_head, 'ref_changes': ref_changes}
    nalloc = sum(r is not None for r in ranks)
    stats = {
        'family%d' % inp['family']: True,
        'alloc4': nalloc == 4, 'alloc3': nalloc == 3, 'alloc2': nalloc == 2, 'alloc1': nalloc == 1,
        'turn_null': turn_rank is None, 'x_null': x_rank is None, 'y_null': y_rank is None,
        'partner_null': not has_partner,
        'lead_zero': not lead, 'lead_nonzero': lead, 'lead_noncanon': (inp['lead'] & 0xFF) > 1, 'lead_high': (inp['lead'] & 0xFF) >= 0x80,
        'conv_path': conv_path, 'simple_path': not conv_path,
        'conv_lead': conv_path and lead, 'conv_nonlead': conv_path and not lead, 'simple_lead': not conv_path and lead, 'simple_nonlead': not conv_path and not lead,
        'conv_noncanon': conv0 > 1, 'conv_overwritten': not lead and (inp['conv'] & 0xFF) != 0 and conv0 == 0, 'conv_set_by_other': not lead and (inp['conv'] & 0xFF) == 0 and conv0 != 0,
        'reset': n_after != n, 'n_negative': n < 0, 'n_zero': n == 0, 'n4': n == 4 and conv_path, 'n5': n == 5 and conv_path, 'nm1': n == -1 and conv_path,
        'n_extreme': n in (-128, 127),
        'ctx_high': conv_path and (ctx_a >= 0x8000 or ctx_b >= 0x8000), 'ctx_ffff': conv_path and (ctx_a == 0xFFFF or ctx_b == 0xFFFF),
        'seed_zero': s == 0, 'seed_high': s >= 64, 'seed_max': s == 127,
        'seed_24bit': s != s_exact, 'seed_nonfinite': acc != acc or acc in (INF, -INF), 'seed_huge': abs(acc) >= 2.0 ** 31 if acc == acc else False,
        'seed_neg': acc < 0 if acc == acc else False,
        'registered1': registered == 1, 'registered2': registered == 2, 'registered3': registered == 3,
        'dup_ref': dup > 0, 'skip_building': skipped > 0,
        'refs0': has_partner and inp['refs_len'] == 0 and turn_rank is not None, 'refs1': has_partner and inp['refs_len'] == 1 and turn_rank is not None,
        'refs2': has_partner and inp['refs_len'] == 2 and turn_rank is not None,
        'last_at_cap1': last == cap - 1, 'search_wraps': bool(win) and win[0][0] <= last,
        'id_wrap': any((win[r][1] & 0x7F) == 0x7F for r in ranks if r is not None),
        'cursor_zero_after_fail': nalloc < 4 and cur == 0,
        'pool_mode%d' % inp['pool_mode']: True,
        'stack_seed': True,
    }
    if has_partner:
        stats['etype%d' % inp['etype']] = True
        stats['building_ipl'] = inp['etype'] == 1 and inp['ipl'] != 0 and turn_rank is not None
        stats['building_temp'] = inp['etype'] == 1 and bool(inp['temp']) and turn_rank is not None
        stats['building_proc'] = inp['etype'] == 1 and bool(inp['proc']) and turn_rank is not None
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_chat(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


def _base(**kw):
    win = [[10, 0x80 | 0x7F], [11, 0x85], [12, 0x80], [13, 0x80]]
    inp = dict(family=2, lead=1, has_partner=1, etype=3, temp=0, proc=0, ipl=0, refs_len=0, dup_kind=0, pool_mode=0, pool_k=4, conv=0, other_conv=0, conv_eff=0, n=0,
               pt=[0, 0, 0], conv_writes=[], eax=0, edx=0, stack_seed=0, refs_off=REFS_OFF, this_off=0x40,
               pool=dict(cap=5000, last=9, free_total=1000, win=win), refs=dict(chain=[5, 6, 7, 8]), node_pp=[])
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # 0) constantes del x87: la semilla de (0,0,0) es 0 -> t1 = 3500, t2 = 6500 ; con semilla 127: t1 = 6476, t2 = 3523
    assert durations(0) == (3500, 6500) and durations(127) == (6476, 3523) and durations(64) == (5000, 5000) and durations(1) == (3523, 6476)
    # 1) lead, sin conversacion, peaton: SimpleChat(t1) y StandStill(t2); sin referencias previas: solo el Turn registra
    want, st = expected_chat(_base(), 0)
    t = want['tasks']
    assert [x['slot'] for x in t] == [0, 1, 2, 3] and [x['type'] for x in t] == [TYPE_SEQ, TYPE_TURN, TYPE_SCHAT, TYPE_STAND]
    assert t[0]['words'][4:8] == ['W1+0', 'W2+0', 'W3+0', 0] and t[2]['words'][8] == 3500 and t[3]['words'][2] == 6500 and t[3]['words'][7] == F_BLEND8
    assert want['ret'] == 'W0+0' and want['last_after'] == 13 and want['state_changes'] == [[10, 0], [11, 6], [12, 1], [13, 1]]
    assert want['ref_changes'] == [[5, 0, 'W1+12']] and want['empty_head'] == 'R6' and want['partner_refs'] == [['R5', 'W1+12']] and want['this_changes'] == [] and want['finds'] == []
    assert st['simple_lead'] and st['registered1'] and st['alloc4'] and st['id_wrap']
    # 2) no lead, sin conversacion: FindTaskByType, la otra tarea decide; orden StandStill(t1), SimpleChat(t2); el byte +0x74 se copia
    want, st = expected_chat(_base(lead=0, conv=0xAA, other_conv=0, pt=[0x3F800000, 0, 0]), 1)   # x = 1.0 -> semilla 1
    assert want['finds'] == [['I+0', 0x4B4]] and want['this_changes'] == [[0x74, 0]] and st['conv_overwritten'] and st['simple_nonlead']
    assert [x['type'] for x in want['tasks']] == [TYPE_SEQ, TYPE_TURN, TYPE_STAND, TYPE_SCHAT] and want['tasks'][2]['words'][2] == 3523 and want['tasks'][3]['words'][8] == 6476
    # 3) redondeo a 24 bits: n=2, z=16777215.0 -> 16777247 se redondea a 16777248 (semilla 32, en doble precision seria 31)
    s, acc, sx = seed_of(2, [0, 0, 0x4B7FFFFF])
    assert (s, sx) == (32, 31) and acc == 16777248.0
    want, st = expected_chat(_base(n=2, pt=[0, 0, 0x4B7FFFFF]), 2)
    assert st['seed_24bit'] and want['tasks'][2]['words'][8] == durations(32)[0]
    # 3b) NaN / inf / enormes -> semilla 0 ; negativos: truncar hacia 0 y mascara de dos complementos (-1 & 0x7F = 127)
    assert seed_of(0, [0x7FC00000, 0, 0])[0] == 0 and seed_of(0, [0, 0, 0x7F800000])[0] == 0 and seed_of(0, [0, 0, 0x5F000000])[0] == 0 and seed_of(0, [0, 0, 0x4F000000])[0] == 0
    assert seed_of(0, [0, 0, 0xBF800000])[0] == 127 and seed_of(0, [0, 0, 0xBF000000])[0] == 0 and seed_of(0, [0, 0, 0xC0400000])[0] == 125   # -3.0 -> -3 & 0x7F = 125
    # 4) conversacion, lead: dos CTaskComplexChat con el contexto A, el Turn y los dos Chat registran (3 referencias) ; contexto en palabra sin signo
    mem = [[-2, 7], [6, 0x8001], [7, 0x1234]]
    inp = _base(conv=1, conv_eff=1, n=0, conv_writes=mem, refs_len=1, node_pp=[0xA5A50000])
    want, st = expected_chat(inp, 3)
    t = want['tasks']
    assert [x['type'] for x in t] == [TYPE_SEQ, TYPE_TURN, TYPE_CHAT, TYPE_CHAT] and t[2]['words'][3] == 0xCDCDCD01 and t[3]['words'][3] == 0xCDCDCD00
    assert t[2]['words'][6] == 0xCD008001 and t[3]['words'][6] == 0xCD008001 and t[2]['words'][4] == 'E+0' and t[2]['words'][5] == 0
    assert want['ref_changes'] == [[5, 'N0', 'W1+12'], [6, 'R5', 'W2+16'], [7, 'R6', 'W3+16']] and want['empty_head'] == 'R8'
    assert want['partner_refs'] == [['R7', 'W3+16'], ['R6', 'W2+16'], ['R5', 'W1+12'], ['N0', 0xA5A50000]] and st['registered3'] and st['ctx_high'] and want['this_changes'] == []
    # 5) conversacion, NO lead: contexto B y orden (0, 1) ; longitud == 3-n -> n = 0 ; con n = 2, longitud 1 ; se copia el byte de la otra tarea
    mem = [[-2, 1], [2, 0x00AA], [3, 0x00BB]]    # n = 2 -> indices 6-4=2, 7-4=3
    inp = _base(lead=0, conv=0, conv_eff=1, other_conv=0x80, n=2, conv_writes=mem)
    want, st = expected_chat(inp, 4)
    t = want['tasks']
    assert want['this_changes'] == [[OFF_N, 0], [OFF_CONV, 0x80]] and st['reset'] and st['conv_set_by_other']
    assert t[2]['words'][3] == 0xCDCDCD00 and t[3]['words'][3] == 0xCDCDCD01 and t[2]['words'][6] == 0xCD0000BB and t[2]['words'][5] == 0
    # 5b) n = 4: la longitud (-2) y el contexto A (6-8=-2) son la misma palabra ; reinicio si vale 0xFFFF (3-4 = -1)
    mem = [[-2, 0x1111], [-2, 0xFFFF], [-1, 0x0042]]
    want, st = expected_chat(_base(conv=1, conv_eff=1, n=4, conv_writes=mem), 5)
    assert st['reset'] and st['n4'] and want['tasks'][2]['words'][6] == 0xCD00FFFF and want['tasks'][2]['words'][5] == 0
    mem = [[-2, 0x1111], [-2, 0xFFFE], [-1, 0x0042]]
    want, st = expected_chat(_base(conv=1, conv_eff=1, n=4, conv_writes=mem), 5)
    assert not st['reset'] and want['tasks'][2]['words'][5] == 4 and want['this_changes'] == []
    # 6) edificio sin IPL no registra ; Chat con partner nulo no registra
    want, st = expected_chat(_base(etype=1), 6)
    assert st['skip_building'] and want['ref_changes'] == [] and want['empty_head'] == 'R5'
    want, st = expected_chat(_base(has_partner=0, conv=1, conv_eff=1, conv_writes=[[-2, 3], [6, 1], [7, 2]]), 7)
    assert want['ref_changes'] == [] and want['tasks'][2]['words'][4] == 0 and want['tasks'][1]['words'][3] == 0
    # 7) referencia ya registrada del Turn
    want, st = expected_chat(_base(refs_len=2, dup_kind=1, node_pp=[0xA5A50000, 'W1+12']), 8)
    assert st['dup_ref'] and want['ref_changes'] == [] and want['partner_refs'] == [['N0', 0xA5A50000], ['N1', 'W1+12']]
    # 8) pool casi agotado: una libre -> solo la secuencia ; dos -> secuencia y Turn ; tres -> falta la ultima
    p1 = _base(pool=dict(cap=5000, last=4990, free_total=1, win=[[3, 0x83]]))
    want, st = expected_chat(p1, 9)
    assert [x['slot'] for x in want['tasks']] == [0] and want['last_after'] == 0 and want['tasks'][0]['words'][4:12] == [0] * 8 and want['ref_changes'] == [] and st['alloc1']
    p3 = _base(pool=dict(cap=5000, last=4999, free_total=3, win=[[0, 0x80], [1, 0xFF], [2, 0x81]]))
    want, st = expected_chat(p3, 10)
    assert [x['slot'] for x in want['tasks']] == [0, 1, 2] and want['last_after'] == 0 and want['tasks'][0]['words'][4:7] == ['W1+0', 'W2+0', 0] and st['alloc3'] and st['y_null']
    # 9) fixtures invalidas
    for bad in (_base(lead=0, has_partner=0), _base(conv=1, conv_eff=1, n=9, conv_writes=[[-2, 0], [6, 0], [7, 0]]), _base(pool=dict(cap=5000, last=9, free_total=1, win=[]))):
        try:
            expected_chat(bad, 11)
            raise SystemExit('el oraculo debia rechazar la fixture invalida')
        except (RuntimeError, KeyError):
            pass
    print('ok')
