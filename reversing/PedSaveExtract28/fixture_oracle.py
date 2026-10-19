"""Oraculo independiente del banco PedSaveExtract28: CPedSaveStructure::Extract (0x5D44B0).

Se modela desde el DESENSAMBLADO del EXE (bash reversing/_handoff/dis.sh 0x5D44B0 0x5D4620), no desde el C++ de la DLL, a partir de la fila `input` que registra el banco.

CPedSaveStructure::Extract(CPed* ped): thiscall, ECX = estructura (0x18C bytes), un argumento en pila, `ret 4`. Guarda y restaura EBX, EDI, EBP, ESI. Pila x87 vacia al salir.
  5D44B6  edi = ped ; eax = [ped+14h] (CPlaceable::m_matrix) ; ebx = this ; fld [this+8] ; fld [this+4] ; fld [this+0]   (z, y, x)
  5D44C5  si matriz != 0: fstp [m+30h] (x) ; fstp [m+34h] (y) ; fstp [m+38h] (z)    si no: fstp [ped+4] (x) ; fstp [ped+8] ; fstp [ped+0Ch]
          (fld/fstp de 32 bits: un NaN de senalizacion pasaria a silencioso; las fixtures no lo generan)
  5D44E1  dword [ped+540h] = [this+0Ch] (vida) ; dword [ped+548h] = [this+10h] (armadura) ; byte [ped+718h] = [this+181h] (ranura de arma activa)
  5D44FF  edx = (u8)[this+180h] ; CPed::SetCharCreatedBy(createdBy) 0x5E47E0 (thiscall, ecx = ped, 1 argumento, edx zero-extendido)
  5D4511  byte [ped+72Dh] = [this+188h] (estilo de lucha) ; byte [ped+72Eh] = [this+189h] (movimientos de ataque permitidos)
  5D4529  ebp = ped+5A8h ; esi = this+14h ; 13 veces (esi += 1Ch, ebp += 1Ch):
            t = [esi]; si t == 0 siguiente
            m1 = CWeaponInfo::GetWeaponInfo(t, 1)->m_nModelId1 (0x743C60, cdecl, push 1 ; push t ; +0Ch)
            si m1 != -1: CStreaming::RequestModel(m1, 8) (0x4087E0) ; CStreaming::LoadAllRequestedModels(false) (0x40EA10)
            m2 = CWeaponInfo::GetWeaponInfo([esi], 1)->m_nModelId2 (+10h) ; si m2 != -1: RequestModel(m2, 8) ; LoadAllRequestedModels(false)
            CPed::GiveWeapon(t = [esi], ammo = [esi+0Ch], false) (0x5E6080, thiscall, 3 argumentos: push 0 ; push ammo ; push type)
            dword [ebp] = [esi+8] (municion del cargador del arma i) -- ebp = ped+5A8h+1Ch*i = &ped->m_aWeapons[i].m_AmmoInClip
  5D45B9  CPed::SetCurrentWeapon((int8)[this+181h] extendido con signo) (0x5E61F0, thiscall, 1 argumento)
  5D45C8  byte [ped+2Fh] = [this+182h] (codigo de area)
  5D45D1  idx = [this+184h] ; si idx == -1: [ped+78Ch] = 0.
          si no: pool = [0x96A7D8] ; si (i8)[[pool+4] + idx] < 0 (bit 7 = libre): [ped+78Ch] = 0 ; si no: [ped+78Ch] = [pool] + idx*3Ch.
          Sin comprobar el rango del indice.

Falsos del banco (iguales en los dos lados) y sus efectos, que este oraculo reproduce:
  GetWeaponInfo -> blocque del tipo en la tabla del caso (modelo1/modelo2) ; sin efectos.
  SetCharCreatedBy -> ped[72Dh]=A1h ped[72Eh]=A2h ped[2Fh]=A3h (el EXE los reescribe despues: prueba el orden).
  GiveWeapon -> ped.arma[ncalls % 13].municion_cargador = C0DE0000h | ncalls ; ncalls += 1 (se reescribe despues para las armas guardadas: prueba el orden).
  SetCurrentWeapon -> ped[718h] = (u8)ranura ; ped[2Fh] = A5h.
  RequestModel, LoadAllRequestedModels -> sin efectos.
  Todos registran [tipo, a, b, c, d, resumen] con el resumen FNV-32 del estado del ped en la entrada de la llamada (antes de sus efectos).
"""
import sys

M32 = 0xFFFFFFFF
NAME = 'CPedSaveStructure::Extract'
NAMES = [NAME]
CASES = {NAME: 4608}
PED, SAVE, MAT = 0x79C, 0x18C, 0x48
NWEAP = 13

EV_CREATED, EV_INFO, EV_REQUEST, EV_LOAD, EV_GIVE, EV_SETCUR = 1, 2, 3, 4, 5, 6
PED_LABEL, BAD_LABEL = 1, 0xBAD

# rangos del ped que se publican en hexadecimal; el resto se resume con FNV-32
RANGE_HEAD = (0x00, 0x40)       # +14h enmascarado
RANGE_HP = (0x540, 0x550)
RANGE_WEP = (0x5A0, 0x720)
RANGE_STYLE = (0x728, 0x730)
RANGE_REST = ((0x40, 0x540), (0x550, 0x5A0), (0x720, 0x728), (0x730, 0x78C), (0x790, 0x79C))


class Fail(Exception):
    """Caso inconsistente con la fila `input`."""


def mix(x):
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def fill_words(size, seed):
    out = bytearray()
    for i in range(0, size, 4):
        out += rand(seed, i // 4).to_bytes(4, 'little')
    return bytes(out[:size])


def fill_pre(size, mode, seed, const):
    if mode == 0:
        return bytes(size)
    if mode == 1:
        return b'\xff' * size
    if mode == 2:
        return fill_words(size, seed)
    return bytes([const & 0xFF]) * size


def fnv32(data, h=2166136261):
    for b in data:
        h ^= b
        h = (h * 16777619) & M32
    return h


def u32(b, off):
    return int.from_bytes(b[off:off + 4], 'little')


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def digest(ped, mat):
    d = bytes(ped[4:16]) + bytes(mat[0x30:0x3C]) + bytes(ped[0x540:0x544]) + bytes(ped[0x548:0x54C]) + bytes([ped[0x718], ped[0x72D], ped[0x72E], ped[0x2F]])
    for i in range(NWEAP):
        d += bytes(ped[0x5A0 + 0x1C * i + 8:0x5A0 + 0x1C * i + 12])
    return fnv32(d)


def rest_hash(ped):
    h = 2166136261
    for a, b in RANGE_REST:
        h = fnv32(ped[a:b], h)
    return h


def expected_extract(inp, case):
    save = bytes.fromhex(inp['save'])
    if len(save) != SAVE:
        raise Fail('estructura de tamano inesperado')
    infos = {t: (m1, m2) for t, m1, m2 in inp['infos']}
    ped = bytearray(fill_pre(PED, inp['ped_mode'], inp['ped_seed'], inp['ped_const']))
    mat = bytearray(fill_pre(MAT, inp['mat_mode'], inp['mat_seed'], inp['mat_const']))
    has_matrix = bool(inp['has_matrix'])
    ped[0x14:0x18] = bytes(4)          # el banco publica este puntero como etiqueta: se enmascara a cero en lo observado
    ped[0x78C:0x790] = bytes(4)
    trace = []
    st = dict(matrix=has_matrix, no_matrix=not has_matrix)
    ncalls = 0

    def ev(kind, a=0, b=0, c=0, d=0):
        trace.append([kind, a & M32, b & M32, c & M32, d & M32, digest(ped, mat)])

    # 5D44C5: posicion
    if has_matrix:
        mat[0x30:0x3C] = save[0:12]
    else:
        ped[4:16] = save[0:12]
    ped[0x540:0x544] = save[0x0C:0x10]
    ped[0x548:0x54C] = save[0x10:0x14]
    ped[0x718] = save[0x181]
    # 5D4507: SetCharCreatedBy
    ev(EV_CREATED, save[0x180], PED_LABEL)
    ped[0x72D], ped[0x72E], ped[0x2F] = 0xA1, 0xA2, 0xA3
    ped[0x72D] = save[0x188]
    ped[0x72E] = save[0x189]
    nweap = 0
    for i in range(NWEAP):
        off = 0x14 + 0x1C * i
        t = u32(save, off)
        if t == 0:
            continue
        nweap += 1
        if t not in infos:
            raise Fail('tipo %#x sin entrada en la tabla de modelos' % t)
        m1, m2 = infos[t]
        ev(EV_INFO, t, 1)
        if m1 != -1:
            ev(EV_REQUEST, m1, 8)
            ev(EV_LOAD, 0)
        ev(EV_INFO, t, 1)
        if m2 != -1:
            ev(EV_REQUEST, m2, 8)
            ev(EV_LOAD, 0)
        ev(EV_GIVE, PED_LABEL, t, u32(save, off + 0xC), 0)
        k = ncalls % NWEAP
        ped[0x5A0 + 0x1C * k + 8:0x5A0 + 0x1C * k + 12] = (0xC0DE0000 | ncalls).to_bytes(4, 'little')
        ncalls += 1
        ped[0x5A0 + 0x1C * i + 8:0x5A0 + 0x1C * i + 12] = save[off + 8:off + 12]
        st['m1_none' if m1 == -1 else 'm1_req'] = True
        st['m2_none' if m2 == -1 else 'm2_req'] = True
        if m1 not in (-1,) and not (0 <= m1 < 20000):
            st['m1_odd'] = True
        if m2 not in (-1,) and not (0 <= m2 < 20000):
            st['m2_odd'] = True
        if t > 0x7FFFFFFF or t > 255:
            st['type_big'] = True
        if t > 46:
            st['type_gt46'] = True
        if u32(save, off + 0xC) in (0, 0xFFFFFFFF, 0x80000000) or u32(save, off + 8) in (0, 0xFFFFFFFF, 0x80000000):
            st['ammo_edge'] = True
    st['wcount%d' % nweap] = True
    # 5D45B9: SetCurrentWeapon con la ranura extendida con signo
    slot = save[0x181]
    ev(EV_SETCUR, PED_LABEL, (slot - 256) if slot >= 128 else slot)
    ped[0x718] = slot
    ped[0x2F] = 0xA5
    ped[0x2F] = save[0x182]
    # 5D45D1: entrada/salida
    idx = s32(u32(save, 0x184))
    if idx != inp['exit_idx']:
        raise Fail('indice de salida inconsistente con la entrada')
    enex = None
    if idx == -1:
        st['idx_none'] = True
    else:
        if not inp['state_valid'] or not (-16 <= idx <= 2047):
            raise Fail('indice fuera del rango leible')
        sbyte = inp['state_byte'] & 0xFF
        if sbyte & 0x80:
            st['idx_free'] = True
        else:
            st['idx_used'] = True
            enex = s32(idx * 0x3C)
        if idx < 0:
            st['idx_neg_other'] = True
        if idx == 0:
            st['idx_0'] = True
        if idx == 2047:
            st['idx_2047'] = True
        if sbyte in (0x7F, 0x80, 0xFF, 0x00):
            st['state_edge'] = True
    slot_b = save[0x181]
    if slot_b >= 128:
        st['slot_ge128'] = True
    if save[0x180] >= 4:
        st['created_ge4'] = True
    if save[0x188] >= 128 or save[0x189] >= 128:
        st['style_ge128'] = True
    for k in range(3):
        w = u32(save, 4 * k)
        if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
            st['pos_nan'] = True
        if (w & 0x7FFFFFFF) == 0x7F800000:
            st['pos_inf'] = True
        if (w & 0x7F800000) == 0 and (w & 0x7FFFFF):
            st['pos_denormal'] = True
        if w == 0x80000000:
            st['pos_negzero'] = True
    for off in (0xC, 0x10):
        w = u32(save, off)
        if (w & 0x7F800000) == 0x7F800000 and (w & 0x7FFFFF):
            st['hp_nan'] = True
    head = bytearray(ped[RANGE_HEAD[0]:RANGE_HEAD[1]])
    want = {
        'ped_head': bytes(head).hex(),
        'ped_hp': bytes(ped[RANGE_HP[0]:RANGE_HP[1]]).hex(),
        'ped_wep': bytes(ped[RANGE_WEP[0]:RANGE_WEP[1]]).hex(),
        'ped_style': bytes(ped[RANGE_STYLE[0]:RANGE_STYLE[1]]).hex(),
        'ped_rest': rest_hash(ped),
        'enex': enex,
        'mat': bytes(mat).hex(),
        'trace': trace,
        'ped_guard_ok': True, 'mat_guard_ok': True, 'save_ok': True, 'states_ok': True, 'pool_ok': True, 'mat_ptr_kept': True,
    }
    st['events'] = len(trace)
    return want, st


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    try:
        return expected_extract(inp, case)
    except Fail as e:
        raise RuntimeError('Fixture invalido en el caso %d: %s' % (case, e))


def compare(obs, want):
    """Lista de claves en las que lo observado difiere de lo esperado."""
    return [k for k, w in want.items() if obs.get(k) != w]


# ---------------------------------------------------------------------------------------------------- autoprueba
def _input(save, **kw):
    inp = dict(family=0, save=save.hex(), has_matrix=1, ped_mode=0, ped_seed=0, ped_const=0, mat_mode=1, mat_seed=0, mat_const=0, state_valid=0, state_byte=0, exit_idx=-1,
               infos=[], eax=0, edx=0, stack_seed=0)
    inp.update(kw)
    return inp


def _selftest():
    # Mezcla/relleno: valores conocidos del generador de C++ (Fixtures.hpp), calculados una vez con el volcado compilado
    assert mix(0) == 0, mix(0)
    assert len(fill_words(10, 7)) == 10 and fill_words(8, 3)[:4] == rand(3, 0).to_bytes(4, 'little')
    # 1) sin armas, con matriz, sin salida: posicion en la matriz, ranura/area/estilo, 1 evento (creador) + 1 (arma actual)
    save = bytearray(SAVE)
    save[0:12] = (0x3F800000).to_bytes(4, 'little') + (0x40000000).to_bytes(4, 'little') + (0x40400000).to_bytes(4, 'little')
    save[0xC:0x10] = (0x42C80000).to_bytes(4, 'little')
    save[0x10:0x14] = (0x41200000).to_bytes(4, 'little')
    save[0x180], save[0x181], save[0x182] = 2, 0xFE, 9
    save[0x184:0x188] = (0xFFFFFFFF).to_bytes(4, 'little')
    save[0x188], save[0x189] = 6, 7
    want, st = expected_extract(_input(bytes(save)), 0)
    mat = bytes.fromhex(want['mat'])
    assert mat[0x30:0x3C] == bytes(save[0:12]) and mat[:0x30] == b'\xff' * 0x30 and mat[0x3C:] == b'\xff' * (MAT - 0x3C), want['mat']
    assert want['ped_head'] == '00' * 0x2F + '09' + '00' * 0x10, want['ped_head']       # ped[2F] = area 9 ; resto a 0 (modo 0)
    assert want['ped_hp'][:8] == '0000c842' and want['ped_hp'][16:24] == '00002041'
    assert want['ped_style'] == '00' * 5 + '0607' + '00', want['ped_style']
    assert want['enex'] is None and [e[0] for e in want['trace']] == [EV_CREATED, EV_SETCUR]
    assert want['trace'][0][1:5] == [2, 1, 0, 0] and want['trace'][1][1:3] == [1, M32 - 1]       # (i8)0xFE = -2 extendido a 32 bits
    # ped[718] = 0xFE (la ranura se escribe antes y la reescribe SetCurrentWeapon con el mismo valor)
    assert want['ped_wep'][(0x718 - 0x5A0) * 2:(0x718 - 0x5A0) * 2 + 2] == 'fe'
    # 2) sin matriz: la posicion va al ped[4..0F]
    want, st = expected_extract(_input(bytes(save), has_matrix=0), 0)
    assert want['ped_head'][8:32] == bytes(save[0:12]).hex() and want['mat'] == 'ff' * MAT
    # 3) dos armas (ranuras 0 y 3), modelos (-1, 400) y (300, -1): orden de eventos y municion
    save2 = bytearray(save)
    for i, (t, clip, total) in ((0, (30, 7, 99)), (3, (46, 0xFFFFFFFF, 0x80000000))):
        off = 0x14 + 0x1C * i
        save2[off:off + 4] = t.to_bytes(4, 'little')
        save2[off + 8:off + 12] = clip.to_bytes(4, 'little')
        save2[off + 0xC:off + 0x10] = total.to_bytes(4, 'little')
    want, st = expected_extract(_input(bytes(save2), infos=[[30, -1, 400], [46, 300, -1]]), 0)
    kinds = [e[0] for e in want['trace']]
    assert kinds == [EV_CREATED, EV_INFO, EV_INFO, EV_REQUEST, EV_LOAD, EV_GIVE, EV_INFO, EV_REQUEST, EV_LOAD, EV_INFO, EV_GIVE, EV_SETCUR], kinds
    assert want['trace'][3][1:3] == [400, 8] and want['trace'][5][1:5] == [1, 30, 99, 0] and want['trace'][10][1:5] == [1, 46, 0x80000000, 0]
    wep = bytes.fromhex(want['ped_wep'])
    assert int.from_bytes(wep[8:12], 'little') == 7                                    # arma 0: municion del cargador guardada
    assert int.from_bytes(wep[3 * 0x1C + 8:3 * 0x1C + 12], 'little') == 0xFFFFFFFF     # arma 3
    # la 2.a llamada a GiveWeapon dejo la marca C0DE0001 en el arma 1 (no se reescribe: no hay arma guardada en la ranura 1)
    assert int.from_bytes(wep[0x1C + 8:0x1C + 12], 'little') == 0xC0DE0001
    # y la 1.a marca (C0DE0000 en el arma 0) la reescribio la municion guardada
    # 4) salida: usada / libre / negativa
    for idx, sb, exp in ((5, 0x00, 5 * 0x3C), (5, 0x7F, 5 * 0x3C), (5, 0x80, None), (5, 0xFF, None), (-16, 0x10, -16 * 0x3C), (2047, 0x01, 2047 * 0x3C), (0, 0x00, 0)):
        s3 = bytearray(save)
        s3[0x184:0x188] = (idx & M32).to_bytes(4, 'little')
        want, st = expected_extract(_input(bytes(s3), exit_idx=idx, state_valid=1, state_byte=sb), 0)
        assert want['enex'] == exp, (idx, sb, want['enex'])
    # 5) un tipo que no esta en la tabla es un fixture invalido
    try:
        expected_extract(_input(bytes(save2), infos=[[30, -1, 400]]), 0)
        raise AssertionError('debia fallar')
    except Fail:
        pass
    print('ok')


if __name__ == '__main__':
    _selftest()
