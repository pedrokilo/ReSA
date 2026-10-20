"""Oraculo independiente del banco Misc61: cuatro funciones, modeladas desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL)
a partir de la fila `input` que registra el banco. Devuelve lo que debe observarse en AMBOS lados (EXE con el hook apagado y DLL con el hook encendido).

--------------------------------------------------------------------------------------------------------------------------------
CMemoryHeap::IntegrityCheck (0x72E8E0, thiscall, void; guarda ESI/EDI; reserva 0x50 bytes de pila = 20 acumuladores)
  edx = this; eax = [this+18h] (m_FreeListBegin.m_Next); ecx = this+20h (&m_FreeListEnd)
  72E8EB  mientras eax != ecx: eax = [eax+10h] (m_Next)                                   -> lee la pagina de cada nodo libre, en orden
  72E8F7  eax = [this+38h] (m_paCommonSizes); si != 0: ecx = eax+18h; esi = 64
          repetir 64 veces: eax = [ecx-8] (=entry+10h: m_ListStart.m_Next); mientras eax != ecx: eax = [eax+10h]; luego ecx += 3Ch
  72E91D  pone a cero 20 dwords locales (rep stos ecx=14h)
  72E92A  eax = [this] (m_FirstBlock); edx = [this+4] (m_LastBlock); mientras eax != edx:
            si byte [eax+4] != 0 y word [eax+6] != 0FFFFh: acum[movsx word [eax+6]] += [eax] + 10h   (indice con signo, sin comprobar rango)
            eax = eax + [eax] + 10h
  72E962  eax = [edx+0Ch] (m_LastBlock->m_PrevBlock); mientras eax != 0: eax = [eax+0Ch]
  No escribe en el monton. Llamadores: solo ParseHeap (0x72EA05), que luego usa ESI (this, conservado por la funcion): ningun llamador lee EAX/ECX/EDX de salida.
  Observable: orden de la PRIMERA lectura de cada pagina (cada estructura en su propia pagina con PAGE_GUARD), y que la arena queda intacta.

CSearchLight::IsSpottedEntity (0x493900, cdecl)
  eax = 4839A0(index, 2) ; si eax < 0 (signo): return 0
  vec local = posicion (matriz [entity+14h] != 0 ? matriz+30h : entity+4); return AL = (493280(&vec, eax) AL != 0) ? 1 : 0   (cdecl, add esp,8)

CTrainNode::GetLightingFromCollision (0x6F5F80, thiscall)
  si byte [this+9]: return byte [this+8]
  pos = (movsx word [this] * 0.125, movsx word [this+2] * 0.125, movsx word [this+4] * 0.125 + 1.0)
  5674E0(&pos, -1000.0f, &cp, &entity, 1, 0, 0, 0, 0, 0, 0)  (cdecl, 11 argumentos, add esp,2Ch)
  AL = 0 -> return 48h ; si no: byte [this+8] = byte [cp+25h] (m_nLightingB); byte [this+9] = 1; return byte [cp+25h]

CTaskSimpleSlideToCoord::ProcessPed (0x66C4E0, thiscall, `ret 4`, marco SEH)
  anim_ok = 1 ; si byte[this+78h] & 2: anim_ok = AL de 61BF20(this, ped)
  si [this+7Ch] == -1: si !(byte78 & 2): [7Ch] = now + 7D0h ; si no si anim_ok: [7Ch] = now + 1F4h        (now = [B7CB84])
  si byte78 & 1: StandStill local (ctor 62F310(20000, 0, 0, 8.0f)); 62F370(local, ped);
       si ped->IsPlayer() (word [ped+598h] 0 o 1): [[ped+47Ch]+14h]->vtable[+18h](ped, 2, 0);  [ped+55Ch] = [this+70h]; byte78 &= FE; dtor
  pos = matriz [ped+14h] ? matriz+30h : ped+4
  A = px - [this+64h]; B = py - [this+68h]; sum = B*B + A*A ; c2 = 0.05f*0.05f ; fcompp(sum, c2): C0 (sum < c2 o NaN) -> desplazamiento {0,0}
  si no: D1 = tx - px; D2 = ty - py; D3 = tz - pz; D3 -> float; V1 = D1*speed -> float; V2 = D2*speed (registro); V3 = D3f*speed (registro; tambien float V3f)
         forward = (V3*m18 + V2*m14) + V1f*m10 ; right = (V3f*m8 + V2*m4) + V1f*m0 ; [ped+4D8h] = right ; [ped+4DCh] = forward   (m = matriz del ped)
  si [this+7Ch] < now (sin signo): return 1
  si !anim_ok: return 0 ; si !(sum < c2) (orden estricto, NaN -> false): return 0
  arg = float(cur - aim) ([ped+558h] - [ped+55Ch]) ; r = 53CB50(arg) ; return |r| < 0.1f (orden estricto, NaN -> false)
x87 a 24 bits con exponente extendido: cada operacion redondea el significando a 24 bits sin limitar el exponente; solo los fstp dword (D3, V1, V3, resultados) pasan a float.
Un NaN con carga propia por caso, nunca junto a infinitos (los infinitos si pueden generar el indefinido 0xFFC00000).
"""
import math
import struct

M32 = 0xFFFFFFFF
NAME_HEAP = 'CMemoryHeap::IntegrityCheck'
NAME_SPOT = 'CSearchLight::IsSpottedEntity'
NAME_SLIDE = 'CTaskSimpleSlideToCoord::ProcessPed'
NAME_NODE = 'CTrainNode::GetLightingFromCollision'
CASES = 4096
NAMES = [NAME_HEAP, NAME_SPOT, NAME_SLIDE, NAME_NODE]
CASES_BY_NAME = {n: CASES for n in NAMES}


# ------------------------------------------------------------------------------------------------ x87 minimo (24 bits, exponente extendido)
class N:
    """NaN silencioso con sus 32 bits (signo + significando)."""
    __slots__ = ('bits',)

    def __init__(self, bits):
        self.bits = bits


DEFAULT_NAN = N(0xFFC00000)


def ld32(u):
    e = (u >> 23) & 0xFF
    m = u & 0x7FFFFF
    if e == 255:
        if m == 0:
            return float('-inf') if u >> 31 else float('inf')
        return N(u | 0x400000)
    return struct.unpack('<f', struct.pack('<I', u))[0]


def r24(x):
    if x == 0 or x != x or x in (float('inf'), float('-inf')):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0), e - 24)


def _nan2(a, b):
    if isinstance(a, N) and isinstance(b, N):
        sa, sb = a.bits & 0x7FFFFF, b.bits & 0x7FFFFF
        if sa == sb and a.bits != b.bits:
            raise RuntimeError('Dos NaN con el mismo significando y distinto signo: fixture no soportada')
        return N(a.bits) if sa >= sb else N(b.bits)
    return a if isinstance(a, N) else b


def _op(a, b, f):
    if isinstance(a, N) or isinstance(b, N):
        return _nan2(a, b)
    r = f(a, b)
    if r != r:
        return DEFAULT_NAN
    return r24(r)


def add(a, b):
    return _op(a, b, lambda x, y: x + y)


def sub(a, b):
    return _op(a, b, lambda x, y: x - y)


def mul(a, b):
    return _op(a, b, lambda x, y: x * y)


def cmp(a, b):
    if isinstance(a, N) or isinstance(b, N):
        return 'un'
    return 'lt' if a < b else 'gt' if a > b else 'eq'


def st32(x):
    """fstp dword: bits del float."""
    if isinstance(x, N):
        return x.bits
    if x == float('inf'):
        return 0x7F800000
    if x == float('-inf'):
        return 0xFF800000
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0xFF800000 if x < 0 else 0x7F800000


def f32(x):
    return ld32(st32(x))


C_REACH = r24(ld32(0x3D4CCCCD) * ld32(0x3D4CCCCD))   # 0x86FD10 al cuadrado (fld; fmul)
LIMIT_01 = ld32(0x3DCCCCCD)                          # 0x86FD14


def _is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def _is_inf_bits(b):
    return (b & 0x7FFFFFFF) == 0x7F800000


def _is_den_bits(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def _s32(v):
    return v - (1 << 32) if v & 0x80000000 else v


def _s16(v):
    return v - 0x10000 if v & 0x8000 else v


# ------------------------------------------------------------------------------------------------ CMemoryHeap::IntegrityCheck
def expected_heap(inp, case):
    n_blk = len(inp['blk_page'])
    if not (len(inp['free_page']) == len(inp['free_off']) and len(inp['cs_entry']) == len(inp['cs_page']) == len(inp['cs_off'])
            and n_blk == len(inp['blk_len']) == len(inp['blk_inuse']) == len(inp['blk_memid']) and len(inp['ghost_page']) == len(inp['ghost_off'])):
        raise RuntimeError('Fixture invalida (longitudes) en %d' % case)
    used = inp['used_pages']
    pages = [0] + ([1] if inp['has_cs'] else []) + inp['free_page'] + inp['cs_page'] + inp['blk_page'] + [inp['term_page']] + inp['ghost_page']
    if max(pages) >= used or used > 48 or len(set(pages)) != len(pages):
        raise RuntimeError('Fixture invalida (paginas) en %d' % case)
    for p, l in zip(inp['blk_page'], inp['blk_len']):
        if l not in (1, 2):
            raise RuntimeError('Longitud de bloque invalida')
    for k in range(n_blk):
        nxt = inp['blk_page'][k + 1] if k + 1 < n_blk else inp['term_page']
        if inp['blk_page'][k] + inp['blk_len'][k] != nxt:
            raise RuntimeError('Bloques no contiguos en %d' % case)
    for inuse, mid in zip(inp['blk_inuse'], inp['blk_memid']):
        if inuse and mid != 0xFFFF and mid >= 20:
            raise RuntimeError('memId en uso fuera de rango en %d' % case)
    order = []
    seen = set()

    def touch(p):
        if p not in seen:
            seen.add(p)
            order.append(p)

    touch(0)                                    # [this+18h]
    for p in inp['free_page']:                  # eax = [eax+10h]
        touch(p)
    if inp['has_cs']:                           # [this+38h]
        entries = {}
        for e, p in zip(inp['cs_entry'], inp['cs_page']):
            entries.setdefault(e, []).append(p)
        for e in range(64):
            touch(1)                            # eax = [ecx-8]
            for p in entries.get(e, []):
                touch(p)
    for p in inp['blk_page']:                   # byte [eax+4]
        touch(p)
    touch(inp['term_page'])                     # eax = [edx+0Ch]
    if inp['prev_mode'] == 1:
        for p in reversed(inp['blk_page']):
            touch(p)
    elif inp['prev_mode'] == 2:
        for p in inp['ghost_page']:
            touch(p)
    stats = {
        'family%d' % inp['family']: True,
        'has_cs': bool(inp['has_cs']), 'no_cs': not inp['has_cs'],
        'free0': not inp['free_page'], 'free_many': len(inp['free_page']) >= 3,
        'cs_nodes': bool(inp['cs_page']), 'cs_many': len(inp['cs_page']) >= 4,
        'blocks0': n_blk == 0, 'blocks_many': n_blk >= 4,
        'prev0': inp['prev_mode'] == 0, 'prev1': inp['prev_mode'] == 1 and n_blk > 0, 'prev2': inp['prev_mode'] == 2,
        'inuse_noncanon': any(b > 1 for b in inp['blk_inuse']),
        'memid_m1': any(i and m == 0xFFFF for i, m in zip(inp['blk_inuse'], inp['blk_memid'])),
        'memid_valid': any(i and m != 0xFFFF for i, m in zip(inp['blk_inuse'], inp['blk_memid'])),
        'free_block_garbage_memid': any((not i) and 20 <= m < 0xFFFF for i, m in zip(inp['blk_inuse'], inp['blk_memid'])),
        'twopage_block': 2 in inp['blk_len'],
        'order_not_ascending': order != sorted(order),
        'stack_seed': True,
    }
    return {'trace': order, 'arena_unchanged': True, 'completed': True}, stats


# ------------------------------------------------------------------------------------------------ CSearchLight::IsSpottedEntity
def expected_spot(inp, case):
    ev = [['script_index', inp['index'], 2]]
    gi = _s32(inp['get_ret'])
    cret = inp['c_ret']
    if gi < 0:
        al = 0
        path = 'neg'
    else:
        ev.append(['spotted', inp['pos'][0], inp['pos'][1], inp['pos'][2], inp['get_ret'], 0])
        al = 1 if cret & 0xFF else 0
        path = 'true' if al else 'false'
    want = {'events': ev, 'nevents': len(ev), 'ret_al': al, 'entity_changes': [], 'matrix_changes': []}
    pos = inp['pos']
    stats = {
        'family%d' % inp['family']: True,
        'path_' + path: True,
        'idx_min': inp['get_ret'] == 0x80000000, 'idx_minus1': inp['get_ret'] == 0xFFFFFFFF, 'idx_zero': inp['get_ret'] == 0,
        'matrix': bool(inp['has_matrix']), 'no_matrix': not inp['has_matrix'],
        'al_zero_upper_nonzero': gi >= 0 and (cret & 0xFF) == 0 and cret != 0, 'al_noncanon': gi >= 0 and (cret & 0xFF) > 1,
        'pos_nan': any(_is_nan_bits(b) for b in pos), 'pos_inf': any(_is_inf_bits(b) for b in pos), 'pos_den': any(_is_den_bits(b) for b in pos),
        'pos_snan': any(_is_nan_bits(b) and not (b & 0x400000) for b in pos),
        'stack_seed': True,
    }
    return want, stats


# ------------------------------------------------------------------------------------------------ CTrainNode::GetLightingFromCollision
def expected_node(inp, case):
    ev = []
    changes = []
    if inp['found'] & 0xFF:
        al = inp['cached'] & 0xFF
        path = 'cached'
    else:
        x, y, z = _s16(inp['x'] & 0xFFFF), _s16(inp['y'] & 0xFFFF), _s16(inp['z'] & 0xFFFF)
        ox, oy = st32(float(x) * 0.125), st32(float(y) * 0.125)
        oz = st32(float(z) * 0.125 + 1.0)
        ev.append(['pvl', ox, oy, oz, 0xC47A0000, 1, 0, 0, 0, 0, 0, 0, 1, 1])
        if (inp['pvl_ret'] & 0xFF) == 0:
            al = 0x48
            path = 'nocol'
        else:
            light = inp['cp_light'] & 0xFF
            if light != inp['cached'] & 0xFF:
                changes.append([8, light])
            changes.append([9, 1])
            al = light
            path = 'col'
    want = {'events': ev, 'nevents': len(ev), 'ret_al': al, 'node_changes': changes}
    stats = {
        'family%d' % inp['family']: True,
        'path_' + path: True,
        'found_noncanon': (inp['found'] & 0xFF) > 1,
        'col_noncanon': path == 'col' and (inp['pvl_ret'] & 0xFF) > 1,
        'col_upper_garbage': path in ('col', 'nocol') and (inp['pvl_ret'] >> 8) != 0,
        'light_changed': path == 'col' and any(c[0] == 8 for c in changes),
        'light_0x48': (inp['cp_light'] & 0xFF) == 0x48 and path == 'col',
        'coord_neg': any(_s16(inp[k] & 0xFFFF) < 0 for k in 'xyz'), 'coord_extreme': any((inp[k] & 0xFFFF) in (0x7FFF, 0x8000) for k in 'xyz'),
        'stack_seed': True,
    }
    return want, stats


# ------------------------------------------------------------------------------------------------ CTaskSimpleSlideToCoord::ProcessPed
def slide_numeric(pos, tgt, speed_b, right, fwd):
    """Devuelve (reached, [shift_x_bits, shift_y_bits], branch). branch: 'near' | 'far' | 'nan'."""
    px, py, pz = (ld32(b) for b in pos)
    tx, ty, tz = (ld32(b) for b in tgt)
    a = sub(px, tx)
    b = sub(py, ty)
    total = add(mul(b, b), mul(a, a))
    c = cmp(total, C_REACH)
    if c in ('lt', 'un'):
        return c == 'lt', [0, 0], ('near' if c == 'lt' else 'nan')
    speed = ld32(speed_b)
    d1, d2, d3 = sub(tx, px), sub(ty, py), sub(tz, pz)
    d3f = f32(d3)
    v1f = f32(mul(d1, speed))
    v2 = mul(d2, speed)
    v3 = mul(d3f, speed)
    v3f = f32(v3)
    r = [ld32(b) for b in right]
    f = [ld32(b) for b in fwd]
    fwd_dot = add(add(mul(v3, f[2]), mul(v2, f[1])), mul(v1f, f[0]))
    right_dot = add(add(mul(v3f, r[2]), mul(v2, r[1])), mul(v1f, r[0]))
    return False, [st32(right_dot), st32(fwd_dot)], 'far'


def _byte_changes(base, old_bytes, new_bytes):
    return [[base + i, nb] for i, (ob, nb) in enumerate(zip(old_bytes, new_bytes)) if ob != nb]


def expected_slide(inp, case):
    ev = []
    bits_old = inp['bits78'] & 0xFF
    bits = bits_old
    run = bool(bits & 2)
    first = bool(bits & 1)
    now = inp['now']
    anim_ok = True
    if run:
        ev.append(['anim', 'task', 'ped'])
        anim_ok = (inp['anim_ret'] & 0xFF) != 0
    timer_old = inp['timer']
    timer = timer_old
    if timer == M32:
        if not run:
            timer = (now + 0x7D0) & M32
        elif anim_ok:
            timer = (now + 0x1F4) & M32
    aim_new = inp['aim']
    player = inp['ped_type'] in (0, 1)
    if first:
        ev.append(['stand', 1, 'ped', [0, 20000, 0, 0, 0, 0, 0, 0, 0x41000000]])
        if player:
            if not inp['intel_valid']:
                raise RuntimeError('Fixture invalida (inteligencia) en %d' % case)
            ev.append(['abort', 'primary', 'ped', 2, 0])
        aim_new = inp['task_aim']
        bits &= 0xFE
    reached, shift, branch = slide_numeric(inp['pos'], inp['tgt'], inp['speed'], inp['right'], inp['fwd'])
    if branch == 'far' and not inp['has_matrix']:
        raise RuntimeError('Fixture invalida (sin matriz al deslizar) en %d' % case)
    if timer < now:
        ret = 1
        path = 'timeout'
    elif not anim_ok:
        ret = 0
        path = 'anim_fail'
    elif not reached:
        ret = 0
        path = 'not_reached'
    else:
        arg = st32(sub(ld32(inp['cur']), ld32(aim_new)))
        ev.append(['limit', arg])
        lim = ld32(inp['lim_ret'])
        ret = 1 if (not isinstance(lim, N) and abs(lim) < LIMIT_01) else 0
        path = 'angle_true' if ret else 'angle_false'
    task_changes = []
    if bits != bits_old:
        task_changes.append([0x78, bits])
    task_changes += _byte_changes(0x7C, list(struct.pack('<I', timer_old)), list(struct.pack('<I', timer)))
    ped_changes = []
    ped_changes += _byte_changes(0x4D8, list(struct.pack('<II', *inp['shift_init'])), list(struct.pack('<II', *shift)))
    ped_changes += _byte_changes(0x55C, list(struct.pack('<I', inp['aim'])), list(struct.pack('<I', aim_new)))
    want = {'events': ev, 'nevents': len(ev), 'ret_al': ret, 'task_changes': task_changes, 'ped_changes': ped_changes, 'aux_unchanged': True}
    allf = list(inp['pos']) + list(inp['tgt']) + [inp['speed'], inp['cur'], inp['aim'], inp['task_aim']] + list(inp['right']) + list(inp['fwd'])
    stats = {
        'family%d' % inp['family']: True, 'regime%d' % inp['regime']: True,
        'path_' + path: True, 'branch_' + branch: True,
        'first': first, 'not_first': not first, 'running': run, 'not_running': not run,
        'player': player, 'not_player': not player, 'player_first': first and player,
        'timer_unset': timer_old == M32, 'timer_set_2000': timer_old == M32 and not run, 'timer_set_500': timer_old == M32 and run and anim_ok,
        'timer_unset_anim_fail': timer_old == M32 and run and not anim_ok,
        'timer_wrap': timer_old == M32 and ((now + (0x7D0 if not run else 0x1F4)) > M32),
        'anim_noncanon': run and (inp['anim_ret'] & 0xFF) > 1, 'anim_upper_garbage_zero': run and (inp['anim_ret'] & 0xFF) == 0 and inp['anim_ret'] != 0,
        'special_nan': inp['special'] == 1, 'special_inf': inp['special'] == 2,
        'any_nan': any(_is_nan_bits(b) for b in allf), 'any_inf': any(_is_inf_bits(b) for b in allf), 'any_den': any(_is_den_bits(b) for b in allf),
        'any_snan': any(_is_nan_bits(b) and not (b & 0x400000) for b in allf),
        'shift_nonzero': shift != [0, 0], 'shift_nan': any(_is_nan_bits(b) for b in shift), 'shift_inf': any(_is_inf_bits(b) for b in shift),
        'shift_den': any(_is_den_bits(b) for b in shift),
        'no_matrix': not inp['has_matrix'], 'matrix': bool(inp['has_matrix']),
        'lim_nan': _is_nan_bits(inp['lim_ret']), 'lim_edge': inp['lim_ret'] in (0x3DCCCCCC, 0x3DCCCCCD, 0x3DCCCCCE, 0xBDCCCCCC, 0xBDCCCCCD, 0xBDCCCCCE),
        'arg_den': path.startswith('angle') and _is_den_bits(arg), 'arg_nan': path.startswith('angle') and _is_nan_bits(arg), 'arg_inf': path.startswith('angle') and _is_inf_bits(arg),
        'now_edge': now in (0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE),
        'timer_eq_now': timer == now, 'bits_high_set': (bits_old & 0xFC) != 0,
        'invalid_intel_unused': not inp['intel_valid'],
        'stack_seed': True,
    }
    return want, stats


def expected(function, inp, case):
    if function == NAME_HEAP:
        return expected_heap(inp, case)
    if function == NAME_SPOT:
        return expected_spot(inp, case)
    if function == NAME_NODE:
        return expected_node(inp, case)
    if function == NAME_SLIDE:
        return expected_slide(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


# ------------------------------------------------------------------------------------------------ autoprueba
def _slide(**kw):
    inp = dict(family=0, regime=2, special=0, bits78=0, timer=0xFFFFFFFF, now=1000, anim_ret=1, ped_type=2, intel_valid=1, has_matrix=1,
               pos=[0x40000000, 0x40000000, 0], decoy=[0, 0, 0], tgt=[0x40000000, 0x40000000, 0], speed=0x3F800000,
               right=[0x3F800000, 0, 0], fwd=[0, 0x3F800000, 0], cur=0, aim=0, task_aim=0x3F800000, lim_ret=0, shift_init=[1, 2], eax=0, edx=0, stack_seed=0)
    inp.update(kw)
    return inp


if __name__ == '__main__':
    # --- aritmetica x87 minima
    assert r24(1.0 + 2.0 ** -24) == 1.0 and r24(1.0 + 3 * 2.0 ** -24) == 1.0 + 2.0 ** -22   # empate al par
    assert st32(1e-40) == 0x000116C2
    assert st32(3.5e38) == 0x7F800000 and st32(-3.5e38) == 0xFF800000 and st32(0.1) == 0x3DCCCCCD
    assert st32(add(float('inf'), float('-inf'))) == 0xFFC00000 and st32(mul(0.0, float('inf'))) == 0xFFC00000
    assert st32(sub(ld32(0x7F800001), 1.0)) == 0x7FC00001
    assert abs(C_REACH - 0.0025000000745058) < 1e-9
    # --- tarea: rama cercana sin deslizar, ruta de angulo
    want, st = expected_slide(_slide(lim_ret=0x3DCCCCCC), 0)
    assert want['ret_al'] == 1 and want['events'] == [['limit', 0]] and want['ped_changes'] == [[0x4D8, 0], [0x4DC, 0]]
    assert want['task_changes'] == [[0x7C + i, b] for i, b in enumerate(struct.pack('<I', 3000)) if b != 0xFF]
    want, st = expected_slide(_slide(lim_ret=0x3DCCCCCD), 1)
    assert want['ret_al'] == 0                       # |r| == 0.1f no es menor
    want, st = expected_slide(_slide(lim_ret=0x7FC00123), 1)
    assert want['ret_al'] == 0                       # NaN: orden estricto -> false
    # temporizador vencido
    want, st = expected_slide(_slide(timer=999, now=1000), 2)
    assert want['ret_al'] == 1 and want['events'] == [] and want['task_changes'] == []
    # animacion: se llama y el temporizador usa 500 solo si anim_ok
    want, st = expected_slide(_slide(bits78=2, anim_ret=0x100), 3)
    assert want['events'][0] == ['anim', 'task', 'ped'] and want['ret_al'] == 0 and want['task_changes'] == []
    want, st = expected_slide(_slide(bits78=2, anim_ret=0x80), 3)
    assert want['task_changes'][0][0] == 0x7C and struct.pack('<I', 1500)[0] == want['task_changes'][0][1]
    # primera vez: stand, abort (jugador), bit 0 limpio, rotacion del ped
    want, st = expected_slide(_slide(bits78=0xA5, ped_type=1), 4)
    assert want['events'][:2] == [['stand', 1, 'ped', [0, 20000, 0, 0, 0, 0, 0, 0, 0x41000000]], ['abort', 'primary', 'ped', 2, 0]]
    assert [0x78, 0xA4] in want['task_changes'] and [0x55E, 0x80] in want['ped_changes'] and [0x55F, 0x3F] in want['ped_changes']
    # deslizamiento: lejos (dx = 3), matriz identidad -> right = vx, forward = vy
    far = dict(pos=[0, 0, 0], tgt=[0x40400000, 0x40800000, 0], speed=0x3F800000, right=[0x3F800000, 0, 0], fwd=[0, 0x3F800000, 0])
    reached, shift, br = slide_numeric(far['pos'], far['tgt'], far['speed'], far['right'], far['fwd'])
    assert not reached and shift == [0x40400000, 0x40800000] and br == 'far'
    # NaN en la suma: desplazamiento cero y no alcanzado
    reached, shift, br = slide_numeric([0x7FC00055, 0, 0], [0, 0, 0], 0x3F800000, [0] * 3, [0] * 3)
    assert not reached and shift == [0, 0] and br == 'nan'
    # borde exacto (suma == 0.05^2 exacto no se alcanza el borde del radio): 'eq' calcula el deslizamiento y no alcanza
    # --- montones
    heap = dict(family=1, has_cs=1, this_off=0, free_page=[5, 3], free_off=[0, 0], cs_entry=[2, 2, 40], cs_page=[7, 6, 4], cs_off=[0, 0, 0],
                blk_page=[8, 10], blk_len=[2, 1], blk_inuse=[1, 0], blk_memid=[3, 0x7FFF], term_page=11, prev_mode=2, ghost_page=[13, 12], ghost_off=[0, 0],
                used_pages=14, eax=0, edx=0, stack_seed=0)
    want, st = expected_heap(heap, 0)
    assert want['trace'] == [0, 5, 3, 1, 7, 6, 4, 8, 10, 11, 13, 12], want['trace']
    heap2 = dict(heap, has_cs=0, cs_entry=[], cs_page=[], cs_off=[], prev_mode=1, ghost_page=[], ghost_off=[], blk_page=[2], blk_len=[1], blk_inuse=[1], blk_memid=[0xFFFF],
                 free_page=[], free_off=[], term_page=3, used_pages=4)
    want, st = expected_heap(heap2, 1)
    assert want['trace'] == [0, 2, 3]
    # --- IsSpottedEntity
    sp = dict(family=0, index=0x00010002, get_ret=0xFFFFFFFF, c_ret=0xFFFFFF00, has_matrix=1, pos=[1, 2, 3], decoy=[4, 5, 6], eax=0, ecx=0, edx=0, stack_seed=0)
    want, st = expected_spot(sp, 0)
    assert want['events'] == [['script_index', 0x00010002, 2]] and want['ret_al'] == 0
    want, st = expected_spot(dict(sp, get_ret=0), 0)
    assert want['events'][1] == ['spotted', 1, 2, 3, 0, 0] and want['ret_al'] == 0 and st['al_zero_upper_nonzero']
    want, st = expected_spot(dict(sp, get_ret=7, c_ret=0x80), 0)
    assert want['ret_al'] == 1 and want['events'][1][4] == 7
    # --- CTrainNode
    nd = dict(family=0, x=0x8000, y=8, z=0xFFF8, found=0, cached=0x11, pvl_ret=0x100, cp_light=0x33, cp_seed=0, eax=0, edx=0, stack_seed=0)
    want, st = expected_node(nd, 0)
    assert want['ret_al'] == 0x48 and want['node_changes'] == [] and want['events'][0][1:5] == [st32(-4096.0), st32(1.0), st32(-0.0 + (-1.0) + 1.0), 0xC47A0000]
    want, st = expected_node(dict(nd, pvl_ret=0x82), 0)
    assert want['ret_al'] == 0x33 and want['node_changes'] == [[8, 0x33], [9, 1]]
    want, st = expected_node(dict(nd, found=0x80), 0)
    assert want['ret_al'] == 0x11 and want['events'] == []
    print('ok')
