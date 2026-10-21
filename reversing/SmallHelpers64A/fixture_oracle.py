"""Oraculo independiente del banco SmallHelpers64A (lote 64): modela, DESDE EL DESENSAMBLADO del EXE, la salida observable de GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70),
RemoveUpgradeCB (0x6D3300) y FindUpgradeCB (0x6D3370) para una fila `input` (la especificacion semantica del caso que registra Core.hpp).

Desensamblado (resumen; ver README.md):
  0x5617C0  call 0x619410 (RsTimer); xor edx,edx; ret                          -> devuelve EDX:EAX = (0, RsTimer())
  0x746F70  eax=[0xC17054]; eax=[eax+1Ch]; si 0 -> ret; Unacquire = [vtbl+20h](eax); vuelve a leer [0xC17054] y [..+1Ch] y llama Release = [vtbl+8](dev);
            vuelve a leer [0xC17054] y escribe [..+1Ch] = 0                      (stdcall: el callee limpia su argumento `this`)
  0x6D3300  esi=atomo; GetAtomicId(atomo)(0x732370); test ah,8 -> si no, ret esi; GetModelInfo(atomo)(0x732260) (1a llamada, sin comprobar nulo);
            ax=word[mi+12h]; ecx=(ax>>10)&1F; data(arg2 sin signo) != ecx -> ret esi; test ah,1 (VehDummy) -> ret esi;
            edi=GetModelInfo(atomo) (2a llamada); edx=[esi+3Ch] (clump); ebx=[esi+4] (marco); RpClumpRemoveAtomic(clump, atomo)(0x74A4C0); RpAtomicDestroy(atomo)(0x749DC0);
            RwFrameDestroy(marco)(0x7F05A0); si edi != 0: CBaseModelInfo::RemoveRef (thiscall ecx=edi)(0x4C4BB0); eax=esi
  0x6D3370  esi=atomo; GetAtomicId(atomo); test ah,8 -> si no, ret esi; GetModelInfo(atomo); ecx=word[mi+12h]>>10 & 1F; edx=data[0]; si edx != ecx -> ret esi;
            data[1]=esi; eax=0; ret

`expected(name, row_input, case)` devuelve (observed, stats): `observed` es EXACTAMENTE lo que registra el C++ en `observed` (ret, eventos EN ORDEN, nevents, cambios de la arena respecto de su imagen inicial,
ps final); los punteros a la arena se codifican como 2^32 + desplazamiento. `stats` son banderas de cobertura. `python -I fixture_oracle.py` ejecuta la autoprueba.
"""
NAME_TIMER = 'GetMillisecondTime'
NAME_MOUSE = 'DIReleaseMouse'
NAME_REMOVE = 'RemoveUpgradeCB'
NAME_FIND = 'FindUpgradeCB'
NAMES = [NAME_TIMER, NAME_MOUSE, NAME_REMOVE, NAME_FIND]
CASES = 4096
CASES_BY_NAME = {n: CASES for n in NAMES}
M32 = 0xFFFFFFFF
LAB = 1 << 32
MAX_EVENTS = 16
FAMILIES = 8
# disposicion de la arena (debe coincidir con Fixtures.hpp: lo comprueba fixture_mirror.py)
O_G0, O_ATOM, O_MI1, O_MI2, O_DATA, O_PS1, O_PS2 = 0x000, 0x040, 0x0C0, 0x100, 0x140, 0x160, 0x1A0
O_DEV = (0x1E0, 0x200, 0x220)
O_G1 = 0x240
ARENA_SIZE = 0x280
PS_MOUSE = 0x1C
MI_FLAGS = 0x12
UPGRADE_BIT = 0x800
LAYOUT = {'atom': O_ATOM, 'mi1': O_MI1, 'mi2': O_MI2, 'data': O_DATA, 'ps1': O_PS1, 'ps2': O_PS2, 'dev_a': O_DEV[0], 'dev_b': O_DEV[1], 'dev_c': O_DEV[2], 'g1': O_G1,
          'size': ARENA_SIZE, 'max_events': MAX_EVENTS, 'v_words': 16, 'families': FAMILIES}
assert O_ATOM == O_G0 + 0x40 and O_MI1 == O_ATOM + 0x80 and O_MI2 == O_MI1 + 0x40 and O_DATA == O_MI2 + 0x40 and O_PS1 == O_DATA + 0x20 and O_PS2 == O_PS1 + 0x40
assert O_DEV[0] == O_PS2 + 0x40 and O_DEV[1] == O_DEV[0] + 0x20 and O_DEV[2] == O_DEV[1] + 0x20 and O_G1 == O_DEV[2] + 0x20 and ARENA_SIZE == O_G1 + 0x40


def lab(off):
    return LAB + off


def observed(ret, events, mem, ps=0):
    return {'ret': ret, 'events': [list(e) for e in events[:MAX_EVENTS]], 'nevents': len(events), 'mem': mem, 'ps': ps}


def timer(inp):
    r = inp['v'][0] & M32
    return observed([r, 0], [[7, r, 0, 0]], []), {'zero': r == 0, 'hi_bit': r >= 0x80000000, 'max': r == M32, 'one': r == 1}


def mouse(inp):
    v = inp['v']
    mouse_dev = [v[0], v[1]]
    cur = 0
    events = []
    stats = {'null_dev': mouse_dev[0] == 0, 'plain': False, 'swap_dev': False, 'swap_ps_unacq': False, 'swap_ps_rel': False, 'rel_clear': False, 'rel_set_dev': False}

    def apply(act, where):
        nonlocal cur
        if act in (1, 2, 3):
            mouse_dev[cur] = act
            stats['swap_dev' if where == 'u' else 'rel_set_dev'] = True
        elif act == 4:
            cur = 1
            stats['swap_ps_unacq' if where == 'u' else 'swap_ps_rel'] = True
        elif act == 5:
            cur = 0
            stats['swap_ps_unacq' if where == 'u' else 'swap_ps_rel'] = True
        elif act == 6:
            mouse_dev[cur] = 0
            stats['rel_clear'] = True
    if mouse_dev[cur] != 0:
        d = mouse_dev[cur]
        b = 2 + 4 * (d - 1)
        events.append([8, d, v[b + 1] & M32, 0])
        apply(v[b], 'u')
        d2 = mouse_dev[cur]
        assert d2 != 0, 'el EXE desreferenciaria un dispositivo nulo'
        b2 = 2 + 4 * (d2 - 1)
        events.append([9, d2, v[b2 + 3] & M32, 0])
        apply(v[b2 + 2], 'r')
        mouse_dev[cur] = 0
        stats['plain'] = not any(stats[k] for k in ('swap_dev', 'swap_ps_unacq', 'swap_ps_rel', 'rel_clear', 'rel_set_dev'))
    init = [v[0], v[1]]
    mem = []
    for i, off in enumerate((O_PS1, O_PS2)):
        if mouse_dev[i] != init[i]:
            mem.append([(off + PS_MOUSE) // 4, lab(O_DEV[mouse_dev[i] - 1]) if mouse_dev[i] else 0])
    mem.sort()
    return observed([], events, mem, lab(O_PS2 if cur else O_PS1)), stats


def upgrade_precheck(inp, events):
    v = inp['v']
    flags = v[0] & M32
    events.append([1, lab(O_ATOM), flags, 0])
    if not (flags & UPGRADE_BIT):          # test ah,8
        return None
    events.append([2, lab(O_ATOM), lab(O_MI1), 0])
    word = v[1] & 0xFFFF
    return (word >> 10) & 0x1F, (word >> 8) & 1


def remove(inp):
    v = inp['v']
    events = []
    stats = {'not_upgrade': False, 'mismatch': False, 'vehdummy': False, 'removed': False, 'owner_null': False, 'owner_other': False, 'owner_same': False}
    pre = upgrade_precheck(inp, events)
    if pre is None:
        stats['not_upgrade'] = True
    else:
        car, veh = pre
        if (v[2] & M32) != car:
            stats['mismatch'] = True
        elif veh:
            stats['vehdummy'] = True
        else:
            stats['removed'] = True
            owner = {0: 0, 1: lab(O_MI2), 2: lab(O_MI1)}[v[3]]
            stats[{0: 'owner_null', 1: 'owner_other', 2: 'owner_same'}[v[3]]] = True
            events.append([2, lab(O_ATOM), owner, 0])
            events.append([3, v[4] & M32, lab(O_ATOM), v[6] & M32])
            events.append([4, lab(O_ATOM), v[7] & M32, 0])
            events.append([5, v[5] & M32, v[8] & M32, 0])
            if owner:
                events.append([6, owner, 0, 0])
    return observed([lab(O_ATOM)], events, []), stats


def find(inp):
    v = inp['v']
    events = []
    stats = {'not_upgrade': False, 'mismatch': False, 'found': False, 'found_vehdummy': False}
    pre = upgrade_precheck(inp, events)
    mem = []
    ret = lab(O_ATOM)
    if pre is None:
        stats['not_upgrade'] = True
    else:
        car, veh = pre
        if (v[2] & M32) == car:
            stats['found'] = True
            stats['found_vehdummy'] = bool(veh)
            assert v[9] != LAB + O_ATOM
            mem.append([O_DATA // 4 + 1, lab(O_ATOM)])
            ret = 0
        else:
            stats['mismatch'] = True
    return observed([ret], events, mem), stats


def expected(name, inp, case):
    if name == NAME_TIMER:
        return timer(inp)
    if name == NAME_MOUSE:
        return mouse(inp)
    if name == NAME_REMOVE:
        return remove(inp)
    if name == NAME_FIND:
        return find(inp)
    raise KeyError(name)


def selftest():
    z = [0] * 16
    obs, _ = expected(NAME_TIMER, {'v': [0xFFFFFFFF] + z[1:]}, 0)
    assert obs['ret'] == [0xFFFFFFFF, 0] and obs['events'] == [[7, 0xFFFFFFFF, 0, 0]]
    # DIReleaseMouse: ps1 con A; sin acciones: Unacquire(A), Release(A), ps1.mouse = 0
    v = [1, 2] + [0, 5, 0, 7] * 3
    obs, _ = expected(NAME_MOUSE, {'v': v}, 0)
    assert obs['events'] == [[8, 1, 5, 0], [9, 1, 7, 0]] and obs['mem'] == [[(O_PS1 + PS_MOUSE) // 4, 0]] and obs['ps'] == lab(O_PS1), obs
    # nulo: nada
    obs, _ = expected(NAME_MOUSE, {'v': [0, 2] + [0] * 12}, 0)
    assert obs['events'] == [] and obs['mem'] == []
    # Unacquire(A) cambia RsGlobal.ps a ps2 (dispositivo B): Release(B) y se anula ps2.mouse
    v = [1, 2, 4, 0, 0, 0] + [0] * 4 + [0] * 4
    obs, _ = expected(NAME_MOUSE, {'v': v}, 0)
    assert obs['events'] == [[8, 1, 0, 0], [9, 2, 0, 0]] and obs['ps'] == lab(O_PS2) and obs['mem'] == [[(O_PS2 + PS_MOUSE) // 4, 0]], obs
    # RemoveUpgradeCB: CarMod 5, sin VehDummy, data 5, flags con 0x800, segundo modelo = modelo 2
    word = 5 << 10
    base = [0x800, word, 5, 1, 0x1234, 0x5678, 11, 12, 13, 0, 0, 0, 0, 0, 0, 0]
    obs, st = expected(NAME_REMOVE, {'v': base}, 0)
    assert [e[0] for e in obs['events']] == [1, 2, 2, 3, 4, 5, 6] and st['removed'] and obs['ret'] == [lab(O_ATOM)], obs
    assert obs['events'][3] == [3, 0x1234, lab(O_ATOM), 11] and obs['events'][5] == [5, 0x5678, 13, 0] and obs['events'][6] == [6, lab(O_MI2), 0, 0]
    b2 = list(base); b2[1] = word | 0x100
    obs, st = expected(NAME_REMOVE, {'v': b2}, 0)
    assert st['vehdummy'] and [e[0] for e in obs['events']] == [1, 2]
    b3 = list(base); b3[2] = 5 | 0x20
    obs, st = expected(NAME_REMOVE, {'v': b3}, 0)
    assert st['mismatch'] and len(obs['events']) == 2
    b4 = list(base); b4[0] = 0xFFFFF7FF
    obs, st = expected(NAME_REMOVE, {'v': b4}, 0)
    assert st['not_upgrade'] and len(obs['events']) == 1
    b5 = list(base); b5[3] = 0
    obs, st = expected(NAME_REMOVE, {'v': b5}, 0)
    assert [e[0] for e in obs['events']] == [1, 2, 2, 3, 4, 5] and st['owner_null']
    # FindUpgradeCB
    obs, st = expected(NAME_FIND, {'v': base}, 0)
    assert st['found'] and obs['ret'] == [0] and obs['mem'] == [[O_DATA // 4 + 1, lab(O_ATOM)]], obs
    obs, st = expected(NAME_FIND, {'v': b4}, 0)
    assert st['not_upgrade'] and obs['ret'] == [lab(O_ATOM)]
    print('autoprueba ok')


if __name__ == '__main__':
    selftest()
