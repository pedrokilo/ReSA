"""Oraculo del banco LeaveCar67: modelo en Python de CTaskSimpleGangDriveBy::SetupAnim (0x627B20) DESDE EL DESENSAMBLADO del EXE (no del C++), aplicado a la fila `input` de cada caso.

Devuelve lo que el banco observa en el lado ORIGINAL (EXE): la traza de eventos de los falsos (`events`), AL y los cambios de las arenas (solo se escriben ocho campos: la tarea
(+9, +0xF, +0x10, +0x11, +0x16, +0x2C) y el byte bajo de las banderas (+0x2E) de las dos asociaciones). run_local.py lo compara con la fila del EXE (y, como original == candidato,
con ambos lados).

Desensamblado (resumen): grupo -> ms_aAnimAssocGroups[grupo].m_AnimBlock (0xB4EA34, grupos de 0x14 bytes); si es nulo, GetAnimationBlock(GetAnimBlockName(grupo)) (0x4D3940, 0x4D3A30);
indice = (bloque - 0xB5D4A0) >> 5 (aritmetico); IsLoaded (byte +0x10) != 1 -> RequestModel(indice + 0x63E7, 8) y AL = 0; si +9 == 0 -> AddAnimBlockRef(indice) y +9 = 1;
id requerido 0xBF sin asociacion -> AL = 0; sin asociacion: reemplaza si (int16)+0x16 <= 0 y +0xC != 0; con asociacion: reemplaza si movsx(id) != requerido y la orden siguiente es 1, 2 o 3
y (char)ultima orden < 4. Reemplazo: SetDeleteCallback(actual, 0x4D1390, 0) y banderas &= 0xFE (si habia), BlendAnimation(clump, grupo, id, 4.0), SetDeleteCallback(nueva, 0x621BE0, this),
rafaga = GetRandomNumberInRange(2, (int16)cargador) si orden siguiente == 3 y (char)rafaga <= 1; con asociacion previa: SetCurrentTime(nueva, [0x8D2E7C]), banderas &= 0xFE, ultima = 1, siguiente = 0,
IsPlayer(ped) y, si no, +0x16 = 100; sin ella: ultima = siguiente, siguiente = 0. Final: AL = peso > 0.9f (0x858C20) y delta >= 0.0f (0x858B50) de la asociacion actual (comparaciones ordenadas).

Autoprueba:  python -I fixture_oracle.py   (casos escritos a mano a partir del desensamblado; imprime ok)
"""
import struct

KGUARD = 0x40
M = 0xFFFFFFFF
BLOCKS = 0xB5D4A0
NAME_HASH = None


def base(i):
    return 0x10000000 + (i << 20)


def tag(i):
    return base(i) + KGUARD


T_TASK, T_PED, T_A, T_B, T_WEAPON, T_GROUPS, T_BLOCK = range(7)


def f32(b):
    return struct.unpack('<f', struct.pack('<I', b & M))[0]


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def s8(v):
    v &= 0xFF
    return v - 256 if v >= 128 else v


def s16(v):
    v &= 0xFFFF
    return v - 65536 if v >= 32768 else v


def s32(v):
    v &= M
    return v - (1 << 32) if v >= 1 << 31 else v


def fnv_s(s):
    h = 2166136261
    for b in s.encode('latin-1'):
        h = ((h ^ b) * 16777619) & M
    return h


MAGIC_TIME_BITS = bits(0.2667)       # float en 0x8D2E7C (0.26669999957084656)
AMP_LIMIT = f32(0x3F666666)           # 0x858C20
DEL_LIMIT = 0.0                       # 0x858B50


def range_int(rand_ret, lo, hi):
    """0x407180: (rand & 0xFFFF) * (1/32768) * (hi - lo) con la x87 de 24 bits (un solo redondeo a float) y _ftol2 (trunca), mas lo."""
    r = rand_ret & 0xFFFF
    rng = s32(hi - lo)
    exact = r * rng / 32768.0          # exacto en doble (|r * rng| < 2^32)
    rounded = f32(bits(exact))         # redondeo al par a 24 bits
    return s32(lo + int(rounded))      # int() trunca hacia cero


def expected(inp, blocks, objs=None):
    """blocks = [direccion del bloque de la arena, bloque del EXE con IsLoaded == 1, bloque del EXE con IsLoaded == 0] (observed['blocks'])."""
    ev = []
    chg = {}                            # cambios de la tarea: desplazamiento -> (viejo, nuevo)
    a_chg = []
    b_chg = []
    out = dict(events=ev, al=0, task=[], a=a_chg, b=b_chg)

    def done(al):
        out['al'] = al
        out['task'] = [[off, new] for off, (old, new) in sorted(chg.items()) if old != new]
        return out

    kind = inp['blk']
    addr = blocks[kind]
    if not inp['via_group']:
        ev.append([1, inp['group']])
        ev.append([2, fnv_s('ped67')])
    index = (s32(addr - BLOCKS)) >> 5
    loaded = (inp['loaded'] & 0xFF) if kind == 0 else (1 if kind == 1 else 0)
    if loaded != 1:
        ev.append([3, (index + 0x63E7) & M, 8])
        return done(0)
    if (inp['refd'] & 0xFF) == 0:
        ev.append([4, index & M])
        chg[9] = (0, 1)
    req = inp['req_id']
    has = bool(inp['has_assoc'])
    if req == 0xBF and not has:
        return done(0)
    nxt, last, burst = inp['next'] & 0xFF, inp['last'] & 0xFF, inp['burst'] & 0xFF
    if not has:
        replace = s16(inp['timer']) <= 0 and (inp['in_range'] & 0xFF) != 0
    elif s16(inp['a_id']) == s32(req):
        replace = False
    else:
        replace = nxt in (1, 2, 3) and s8(last) < 4
    cur = T_A if has else None
    if replace:
        if has:
            ev.append([6, tag(T_A), 0x4D1390, 0])
            if (inp['a_flags'] & 1):
                a_chg.append([0x2E, inp['a_flags'] & 0xFE])
        ev.append([5, inp['clump'], inp['group'], req, 0x40800000])
        ev.append([6, tag(T_B), 0x621BE0, tag(T_TASK)])
        cur = T_B
        if nxt == 3 and s8(burst) <= 1:
            ev.append([7])
            chg[0x11] = (burst, range_int(inp['rand_ret'], 2, s16(inp['clip'])) & 0xFF)
        if has:
            ev.append([8, tag(T_B), MAGIC_TIME_BITS])
            if (inp['b_flags'] & 1):
                b_chg.append([0x2E, inp['b_flags'] & 0xFE])
            chg[0x10] = (last, 1)
            chg[0xF] = (nxt, 0)
            ev.append([9, tag(T_PED)])
            if (inp['is_player_ret'] & 0xFF) == 0:
                old = inp['timer'] & 0xFFFF
                chg[0x16] = (old & 0xFF, 100)
                chg[0x17] = ((old >> 8) & 0xFF, 0)
        else:
            chg[0x10] = (last, nxt)
            chg[0xF] = (nxt, 0)
        # los punteros guardados en la memoria de la tarea son direcciones reales (los eventos llevan etiquetas)
        old_ptr = (objs[T_A] if objs else tag(T_A)) if has else 0
        new_ptr = objs[T_B] if objs else tag(T_B)
        for i in range(4):
            chg[0x2C + i] = ((old_ptr >> (8 * i)) & 0xFF, (new_ptr >> (8 * i)) & 0xFF)
    if cur is None:
        return done(0)
    amp, dl = (inp['a_amp'], inp['a_del']) if cur == T_A else (inp['b_amp'], inp['b_del'])
    return done(1 if (f32(amp) > AMP_LIMIT and f32(dl) >= DEL_LIMIT) else 0)


def check(row):
    """Compara el lado original del banco con el modelo; devuelve None si coincide o un texto con la diferencia."""
    inp = row['input']
    obs = row['original']['observed']
    exp = expected(inp, obs['blocks'], obs.get('objs'))
    if obs['events'] != exp['events']:
        return 'eventos %s != %s' % (obs['events'], exp['events'])
    if obs['al'] != exp['al']:
        return 'AL %s != %s' % (obs['al'], exp['al'])
    want = [exp['task'], [], exp['a'], exp['b'], [], [], []]
    if obs['changes'] != want:
        return 'cambios %s != %s' % (obs['changes'], want)
    return None


def _selftest():
    blocks = [0x20000000, 0x859000, 0x858000]
    base_in = dict(group=5, via_group=1, blk=0, loaded=1, refd=1, req_id=0x20, has_assoc=0, a_id=0, a_flags=0, a_amp=0, a_del=0, b_id=0, b_flags=0xFFFF, b_amp=bits(1.0), b_del=0,
                   next=2, last=0, burst=5, timer=0, in_range=1, w_kind=1, clip=30, clump=0x12345, is_player_ret=0, rand_ret=0, seed=0)
    # no cargado: RequestModel(indice + 0x63E7, 8)
    r = expected(dict(base_in, loaded=0), blocks)
    assert r['events'] == [[3, ((0x20000000 - BLOCKS) >> 5) + 0x63E7, 8]] and r['al'] == 0 and r['task'] == []
    # bloque del EXE por debajo de la tabla: indice negativo con desplazamiento aritmetico
    r = expected(dict(base_in, blk=2), blocks)
    assert r['events'] == [[3, (((0x858000 - BLOCKS) >> 5) + 0x63E7) & M, 8]]
    # sin asociacion, crea: BlendAnimation, SetDeleteCallback; ultima = siguiente, siguiente = 0; peso 1.0 y delta 0 -> AL 1
    r = expected(base_in, blocks)
    assert [e[0] for e in r['events']] == [5, 6] and r['al'] == 1
    assert r['task'] == [[0xF, 0], [0x10, 2]] + [[0x2C + i, (tag(T_B) >> (8 * i)) & 0xFF] for i in range(4) if (tag(T_B) >> (8 * i)) & 0xFF]
    # sin referencia: la anade
    r = expected(dict(base_in, refd=0, timer=1), blocks)
    assert r['events'][0] == [4, ((0x20000000 - BLOCKS) >> 5) & M] and r['task'] == [[9, 1]] and r['al'] == 0
    # 0xBF sin asociacion
    assert expected(dict(base_in, req_id=0xBF), blocks)['events'] == [] and expected(dict(base_in, req_id=0xBF), blocks)['al'] == 0
    # rafaga: orden 3 y rafaga <= 1 (con signo): rand 0xFFFF con cargador 30 -> 2 + trunc(65535 * 28 / 32768) = 57
    r = expected(dict(base_in, next=3, burst=0x80, rand_ret=0xFFFF), blocks)
    assert [e[0] for e in r['events']] == [5, 6, 7] and [0x11, 57] in r['task']
    # con asociacion distinta: reemplaza, SetCurrentTime, IsPlayer falso -> temporizador 100
    r = expected(dict(base_in, has_assoc=1, a_id=3, a_flags=0x0101, next=1, last=0xFF, b_flags=0x0003), blocks)
    assert [e[0] for e in r['events']] == [6, 5, 6, 8, 9] and r['a'] == [[0x2E, 0]] and r['b'] == [[0x2E, 2]]
    assert [0x10, 1] in r['task'] and [0x16, 100] in r['task']
    # misma animacion: no reemplaza; resultado con los valores de la asociacion actual
    r = expected(dict(base_in, has_assoc=1, a_id=0x20, a_amp=bits(0.95)), blocks)
    assert r['events'] == [] and r['al'] == 1
    # la comparacion de id es movsx de 16 bits contra el dword requerido
    r = expected(dict(base_in, has_assoc=1, a_id=0x8000, req_id=0xFFFF8000, a_amp=bits(0.95)), blocks)
    assert r['events'] == [] and r['al'] == 1
    r = expected(dict(base_in, has_assoc=1, a_id=0x8000, req_id=0x8000, next=1, a_amp=bits(0.95)), blocks)
    assert r['events'] != []
    # 0.9f exacto no supera; NaN no; delta subnormal negativo no
    assert expected(dict(base_in, timer=1, b_amp=0x3F666666), blocks)['al'] == 0
    assert expected(dict(base_in, has_assoc=1, a_id=0x20, a_amp=0x3F666667, a_del=0x80000001), blocks)['al'] == 0
    assert expected(dict(base_in, has_assoc=1, a_id=0x20, a_amp=0x3F666667, a_del=0x80000000), blocks)['al'] == 1
    assert expected(dict(base_in, has_assoc=1, a_id=0x20, a_amp=0x7FC00000), blocks)['al'] == 0
    print('ok')


if __name__ == '__main__':
    _selftest()
