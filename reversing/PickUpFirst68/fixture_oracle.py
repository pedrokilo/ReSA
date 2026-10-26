"""Oraculo independiente del banco PickUpFirst68: CTaskComplexGoPickUpEntity::CreateFirstSubTask (EXE 0x693610..0x6939CB).

Modela la funcion SOLO desde el desensamblado del EXE (no desde el C++ de la DLL): cada operacion x87 a 24 bits de mantisa con exponente extendido (`r24`), cada resultado guardado como float
(`bits`), las comparaciones ordenadas de la x87 (`fcomp` + `test ah,41h` / `test ah,5`: NaN no cumple ninguna) y la secuencia de colaboradores. Devuelve, por caso, la salida esperada de
`observed` (retorno, eventos EN ORDEN con argumentos, bytes cambiados de la tarea y de la entidad respecto a los valores centinela) y las claves de cobertura.

Desensamblado (resumen; direcciones del EXE):
  693631 sin entidad ([this+0xC]) -> 0 (006939B9), nada escrito.
  693637 [this+0x28] = CTimer::m_snTimeInMilliseconds (0xB7CB84).
  69363F..69367E d = ped - entidad: posicion = (matriz?+0x30 : +4); dz = pz - ez, dy, dx guardados como float.
  69368A caja = ms_modelInfoPtrs[movsx word [ent+0x22]] -> [+0x14]; [this+0x10] = [0x870A74] (-0.2f); [this+0x14] = -min.y - [0x870A78] (0.2f); [this+0x18] = -min.z.
  6936AD 2*max.y < max.x (fcomp + test ah,5 + jp): SetHeading(atan2(-dx, dy)) en linea (con matriz: CMatrix::SetRotateZOnly 0x59B020; sin matriz: [ent+0x10] = rumbo, como float).
  6936EE 2*max.x < max.y: igual con el fpatan menos [0x858FE4] (pi/2 como float) con un solo redondeo a 24 bits antes de guardar.
  69372F si no: GetMatrix (0x54F560 + 0x54F1B0 si no hay matriz), up.z ([matriz+0x28]) < [0x858C20] (0.9f) -> rumbo = atan2(-fwd.x, fwd.y) de la matriz, SetHeading (0x59B020).
  693799 tipo ([ent+0x36]&7) == 4: velocidades [ent+0x44] y [ent+0x50] = 0 (3 dwords cada una) y SetIsStatic(true) (vtable +0x10).
  693812 objeto RW ([ent+0x18]) no nulo: marco = [obj+4]; con matriz CMatrix::UpdateRwMatrix (0x59AD70) si no CSimpleTransform::UpdateRwMatrix (0x54EF40) sobre marco+0x10.
  693839 GetMatrix; derecha = (dz*r.z + dy*r.y) + dx*r.x (guardado como float); GetMatrix; frente = (dz*f.z + dy*f.y) + dx*f.x (sin guardar, 24 bits); a = |frente|.
  6938B9 derecha > a -> pickup.y = 0, pickup.x = [0x870A7C] (0.4f) + max.x.  -a > derecha -> pickup.y = 0, pickup.x = min.x - 0.4f.
         si no pickup.x = 0 y frente > [0x858B50] (0.0f) -> pickup.y = 0.4f + max.y; si no min.y - 0.4f.   pickup.z = min.z + [0x858624] (1.0f).
  693933 GetMatrix; 0x59C890(&salida, matriz, &pickup); CTask::operator new(0x28) (0x61A5A0); si nulo -> 0; si no 0x668120(4, &punto, 0.2f, 0.0f, false, true) y devuelve el objeto.
"""
import math
import struct

M32 = 0xFFFFFFFF
SENT_POS, SENT_PICKUP, SENT_TIME, SENT_HEADING, SENT_SPEED = 0xA5A5A5A5, 0x5A5A5A5A, 0xC3C3C3C3, 0x69696969, 0x96969696
C_NEG02, C_02, C_04, C_ZERO, C_ONE, C_HALFPI, C_09 = 0xBE4CCCCD, 0x3E4CCCCD, 0x3ECCCCCD, 0, 0x3F800000, 0x3FC90FDB, 0x3F666666
NAME = 'CTaskComplexGoPickUpEntity::CreateFirstSubTask'
CASES = 16384
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}


def fv(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def bits(x):
    """double -> bits de float (redondeo al mas cercano, desbordamiento a infinito)"""
    if x != x:
        return 0x7FC00000
    try:
        return struct.unpack('<I', struct.pack('<f', x))[0]
    except OverflowError:
        return 0x7F800000 if x > 0 else 0xFF800000


def r24(x):
    """resultado de una operacion de la x87 con control de precision de 24 bits (exponente extendido)"""
    if x != x or x in (math.inf, -math.inf) or x == 0.0:
        return x
    m, e = math.frexp(x)
    m = round(m * 16777216.0) / 16777216.0
    return math.ldexp(m, e)


def store(x):
    """valor tras guardarlo como float y volver a cargarlo"""
    return fv(bits(x))


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def ordered(b):
    return b if b < 0x80000000 else -(b & 0x7FFFFFFF)


def close(a, b, ulps):
    if is_nan(a) or is_nan(b):
        return is_nan(a) and is_nan(b)
    return abs(ordered(a) - ordered(b)) <= ulps


def neg(b):
    return (b ^ 0x80000000) & M32


def word_bytes(off, new, sent):
    return [[off + i, (new >> (8 * i)) & 0xFF] for i in range(4) if ((new >> (8 * i)) & 0xFF) != ((sent >> (8 * i)) & 0xFF)]


def atan2_bits(y, x):
    """fpatan(y, x) como float (comparacion con tolerancia de 2 ulp: el EXE usa 64 bits de mantisa, aqui 53)"""
    return bits(math.atan2(y, x))


def expected(fn, inp, case):
    assert fn == NAME
    ev = []
    stats = {}
    ret_alloc = False
    task = []
    ent = []
    want_matrix = None
    words = {}       # solo para el chequeo del constructor
    if inp['ent_null']:
        stats['ent_null'] = True
        want = {'ret': 0, 'events': [], 'nevents': 0, 'task_changes': [], 'ent_changes': [], 'ent_matrix': None, 'others_intact': True}
        return want, stats
    c = inp
    ex = [fv(b) for b in c['ent']]
    pe = [fv(b) for b in c['ped']]
    # d = ped - entidad (cada resta a 24 bits y guardada como float)
    dz = store(r24(pe[2] - ex[2]))
    dy = store(r24(pe[1] - ex[1]))
    dx = store(r24(pe[0] - ex[0]))
    box = [fv(b) for b in c['box']]
    minx, miny, minz, maxx, maxy, maxz = box
    # [this+0x10..0x18]
    pos_x = C_NEG02
    pos_y = bits(r24(-miny - fv(C_02)))
    pos_z = neg(c['box'][2])
    time_word = c['time_ms']
    mat = 'M' if c['ent_mat'] else None
    heading_bits = None
    heading_in_ent = False

    def mat_name():
        return 'M+0' if mat == 'M' else 'N+0'

    def get_matrix():
        nonlocal mat
        if mat is None:
            ev.append(['alloc', 'E+0'])
            ev.append(['stmat', 'E+4', 'N+0'])
            mat = 'N'
            stats['alloc_reserved'] = True

    def mvec(kind):
        if mat == 'M':
            return {'right': c['right'], 'fwd': c['fwd'], 'up': c['up']}[kind]
        return {'right': c['a_right'], 'fwd': c['a_fwd'], 'up': c['a_up']}[kind]

    def set_heading(hbits, tag):
        nonlocal heading_bits, heading_in_ent
        if mat is None:
            heading_bits = hbits
            heading_in_ent = True
            stats['heading_%s_placement' % tag] = True
        else:
            ev.append(['setrot', mat_name(), hbits])
            stats['heading_%s_mat' % tag] = True

    lt_a = r24(maxy + maxy) < maxx
    lt_b = (not lt_a) and r24(maxx + maxx) < maxy
    if lt_a:
        set_heading(atan2_bits(-dx, dy), 'a')
    elif lt_b:
        # fpatan - [0x858FE4] con un solo redondeo a 24 bits antes de guardar (la tolerancia cubre la diferencia de 53 contra 64 bits de mantisa)
        set_heading(bits(r24(math.atan2(-dx, dy) - fv(C_HALFPI))), 'b')
    else:
        get_matrix()
        upz = fv(mvec('up')[2])
        if upz < fv(C_09):
            fwd = mvec('fwd')
            set_heading(atan2_bits(-fv(fwd[0]), fv(fwd[1])), 'c')
            stats['c_set'] = True
        else:
            stats['c_skip'] = True
            if upz != upz:
                stats['c_upz_nan'] = True
    # objetos
    if (c['flags'] & 7) == 4:
        ev.append(['static', 'E+0', 1])
        stats['object_static'] = True
    # objeto RW
    if c['rw']:
        if mat is not None:
            ev.append(['matrw', mat_name(), 'F+16'])
            stats['rw_mat'] = True
        else:
            ev.append(['strw', 'E+4', 'F+16'])
            stats['rw_placement'] = True
    else:
        stats['rw_null'] = True
    # productos escalares
    get_matrix()
    r = [fv(b) for b in mvec('right')]
    pr = r24(r24(r24(dz * r[2]) + r24(dy * r[1])) + r24(dx * r[0]))
    proj_right = store(pr)
    get_matrix()
    f = [fv(b) for b in mvec('fwd')]
    proj_fwd = r24(r24(r24(dz * f[2]) + r24(dy * f[1])) + r24(dx * f[0]))
    abs_fwd = abs(proj_fwd)
    if 0.0 < abs_fwd < 1.1754943508222875e-38:
        stats['fwd_subnormal'] = True            # |frente| en el rango subnormal de float: SSE con FTZ lo da por 0, la x87 no
    if 0.0 < abs_fwd < 1.401298464324817e-45:
        stats['fwd_below_float'] = True          # |frente| por debajo del menor float subnormal (solo existe con exponente extendido)
    if 0.0 < abs(proj_right) < 1.1754943508222875e-38:
        stats['right_subnormal'] = True
    if proj_right > abs_fwd:
        pick_y, pick_x = 0, bits(r24(fv(C_04) + maxx))
        stats['pick_right'] = True
    elif -abs_fwd > proj_right:
        pick_y, pick_x = 0, bits(r24(minx - fv(C_04)))
        stats['pick_left'] = True
    else:
        pick_x = 0
        if proj_fwd > 0.0:
            pick_y = bits(r24(fv(C_04) + maxy))
            stats['pick_front'] = True
        else:
            pick_y = bits(r24(miny - fv(C_04)))
            stats['pick_back'] = True
        if proj_right != proj_right or proj_fwd != proj_fwd:
            stats['pick_nan'] = True
    pick_z = bits(r24(minz + 1.0))
    get_matrix()
    ev.append(['xform', mat_name(), 'T+28', pick_x, pick_y, pick_z])
    ev.append(['new', 0x28])
    built = not c['new_null']
    ret = 'A+0' if built else 0
    stats['built' if built else 'new_null'] = True
    # bytes cambiados
    task = []
    for off, new, sent in ((0x10, pos_x, SENT_POS), (0x14, pos_y, SENT_POS), (0x18, pos_z, SENT_POS), (0x1C, pick_x, SENT_PICKUP), (0x20, pick_y, SENT_PICKUP), (0x24, pick_z, SENT_PICKUP), (0x28, time_word, SENT_TIME)):
        task.append(('w', off, new, sent))
    entw = []
    if heading_in_ent:
        entw.append(('w', 0x10, heading_bits, SENT_HEADING))
    if (c['flags'] & 7) == 4:
        for k in range(6):
            entw.append(('w', 0x44 + 4 * k, 0, SENT_SPEED))
    want = {'ret': ret, 'events': ev, 'nevents': len(ev), 'task_words': task, 'ent_words': entw, 'ent_matrix': None, 'others_intact': True}
    want['ent_matrix'] = 'M+0' if c['ent_mat'] else 'N+0' if stats.get('alloc_reserved') else 0
    if built:
        want['alloc_words'] = [4, c['target'][0], c['target'][1], c['target'][2]]
    return want, stats


def decode(changes, words):
    """bytes cambiados -> {desplazamiento de palabra: bits}, partiendo de los centinelas de `words` ((off, new, sent) -> sent)"""
    base = {}
    for _, off, _, sent in words:
        base[off] = sent
    out = dict(base)
    for off, byte in changes:
        w = off & ~3
        if w not in out:
            return None
        sh = 8 * (off - w)
        out[w] = (out[w] & ~(0xFF << sh) & M32) | (byte << sh)
    return out


def check(fn, inp, case, obs):
    """compara la salida observada con la esperada; devuelve la lista de campos distintos (vacia si todo coincide)"""
    want, stats = expected(fn, inp, case)
    bad = []
    if obs['ret'] != want['ret']:
        bad.append('ret')
    if len(obs['events']) != len(want['events']) or obs['nevents'] != want['nevents']:
        bad.append('events_len')
    else:
        for i, (o, w) in enumerate(zip(obs['events'], want['events'])):
            if o[0] != w[0] or len(o) != len(w):
                bad.append('event%d' % i)
                continue
            if w[0] == 'setrot':
                if o[1] != w[1] or not close(o[2], w[2], 2):
                    bad.append('event%d' % i)
            elif w[0] == 'xform':
                if o[1:3] != w[1:3] or not all(a == b or (is_nan(a) and is_nan(b)) for a, b in zip(o[3:], w[3:])):
                    bad.append('event%d' % i)
            elif o != w:
                bad.append('event%d' % i)
    if inp['ent_null']:
        if obs['task_changes'] or obs['ent_changes']:
            bad.append('changes')
        al = obs['alloc']
        if al['vtable_written'] is not False or al['outside_ok'] is not True or any(x != al['words'][1] for x in al['words'][1:]):
            bad.append('alloc_untouched')
        return bad, stats
    for key, words, ch in (('task', want['task_words'], obs['task_changes']), ('ent', want['ent_words'], obs['ent_changes'])):
        got = decode(ch, words)
        if got is None:
            bad.append(key + '_unexpected_bytes')
            continue
        for _, off, new, sent in words:
            g = got[off]
            tol = 2 if (key == 'ent' and off == 0x10) or (key == 'task' and False) else 0
            if not (g == new or (is_nan(g) and is_nan(new)) or (tol and close(g, new, tol))):
                bad.append('%s_word_%x' % (key, off))
    if obs['ent_matrix'] != want['ent_matrix']:
        bad.append('ent_matrix')
    if obs['others_intact'] is not True:
        bad.append('others_intact')
    al = obs['alloc']
    if 'alloc_words' in want:
        w = al['words']
        exp = want['alloc_words']
        if al['vtable_written'] is not True or al['outside_ok'] is not True or w[3] != exp[0] or not all(a == b or (is_nan(a) and is_nan(b)) for a, b in zip(w[4:7], exp[1:])):
            bad.append('alloc')
    else:
        if al['vtable_written'] is not False or al['outside_ok'] is not True or any(x != al['words'][1] for x in al['words'][1:]):
            bad.append('alloc_untouched')
    return bad, stats


if __name__ == '__main__':
    # autoprueba sin juego: el modelo debe dar la secuencia esperada en un caso construido a mano
    inp = dict(ent_null=0, ent_mat=1, flags=4, rw=1, ped_mat=0, ent=[0x41200000, 0x41200000, 0x41200000], ped=[0x41A00000, 0x41200000, 0x41200000],
               box=[0xBF800000, 0xBF800000, 0xBF800000, 0x40000000, 0x3F800000, 0x3F800000], right=[0x3F800000, 0, 0], fwd=[0, 0x3F800000, 0], up=[0, 0, 0x3F800000],
               a_right=[0, 0, 0], a_fwd=[0, 0, 0], a_up=[0, 0, 0], time_ms=1234, new_null=0, target=[1, 2, 3])
    want, st = expected(NAME, inp, 0)
    assert want['events'][0][0] == 'static' or want['events'][0][0] == 'setrot', want['events']
    print('ok', want['events'], sorted(st))
