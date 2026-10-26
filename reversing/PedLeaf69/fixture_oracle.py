"""Oraculo del banco PedLeaf69: modelo en Python de las cuatro funciones DESDE EL DESENSAMBLADO del EXE (no del C++), aplicado a la fila `input` de cada caso.

Devuelve lo que el banco observa en el lado ORIGINAL (EXE): AL / EAX de retorno, la traza de eventos del falso de CEntity::CleanUpOldReference y los cambios predecibles de las
arenas (ranuras puestas a cero). run_local.py lo compara con la fila del EXE (y, como original == candidato, con ambos lados).

Aritmetica x87 del juego (IsInHearingRange): precision de 24 bits con exponente extendido. Con operandos finitos se calcula EXACTO con fracciones (round24 = redondeo al par a 24
bits de mantisa, sin limite de exponente); con NaN/inf se usa la aritmetica IEEE de Python (misma que la x87 para estos casos).

Autoprueba:  python -I fixture_oracle.py   (comprueba el propio modelo con casos escritos a mano a partir del desensamblado; imprime ok)
"""
import math
import struct
from fractions import Fraction

KGUARD = 0x40
KENTITIES = 0x288
STRIDE = 0x34
COUNT = 43
CASES = 4096
NAMES = ['CPedIntelligence::IsInHearingRange', 'CPedIntelligence::HasInterestingEntites', 'CPedIntelligence::RemoveAllInterestingEntities', 'CPedStats::FindIndexWithPedStat']
ADDRESS = [0x600C00, 0x602080, 0x602320, 0x608940]


def base(i):
    """Etiqueta de una arena registrada (Tag): 0x10000000 + (indice << 20)."""
    return 0x10000000 + (i << 20)


def f32(b):
    return struct.unpack('<f', struct.pack('<I', b & 0xFFFFFFFF))[0]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def is_denormal(b):
    return (b & 0x7F800000) == 0 and (b & 0x7FFFFF) != 0


def round24(x):
    """Redondeo al par a 24 bits de mantisa de una Fraction (exponente sin limite): la x87 con precision de 24 bits."""
    if x == 0:
        return x
    sign = -1 if x < 0 else 1
    x = abs(x)
    # exponente e con 2^e <= x < 2^(e+1)
    e = x.numerator.bit_length() - x.denominator.bit_length()
    if Fraction(2) ** e > x:
        e -= 1
    scale = Fraction(2) ** (e - 23)
    q = x / scale
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return sign * n * scale


def hearing(rng, ped, posn):
    """0x600C15..0x600C4C: d = posn - ped (tres restas), limite = rango*rango, (dz*dz + dx*dx) + dy*dy, limite > suma ordenada. Devuelve (resultado, suma o None)."""
    vals = [f32(b) for b in (rng, *ped, *posn)]
    if all(math.isfinite(v) for v in vals):
        r = Fraction(vals[0])
        p = [Fraction(v) for v in vals[1:4]]
        q = [Fraction(v) for v in vals[4:7]]
        dx, dy, dz = (round24(q[i] - p[i]) for i in range(3))
        limit = round24(r * r)
        s = round24(round24(round24(dz * dz) + round24(dx * dx)) + round24(dy * dy))
        return limit > s, (limit, s)
    # con NaN / inf: IEEE (la x87 con exponente extendido da el mismo resultado de la comparacion ordenada)
    r = vals[0]
    dx, dy, dz = (vals[4 + i] - vals[1 + i] for i in range(3))
    limit = r * r
    s = (dz * dz + dx * dx) + dy * dy
    return bool(limit > s), None


def order_sensitive(rng, ped, posn):
    """True si algun otro orden de la suma (o la suma en float) cambiaria el resultado: sirve de cobertura de la fidelidad del orden."""
    vals = [f32(b) for b in (rng, *ped, *posn)]
    if not all(math.isfinite(v) for v in vals):
        return False
    r = Fraction(vals[0])
    p = [Fraction(v) for v in vals[1:4]]
    q = [Fraction(v) for v in vals[4:7]]
    dx, dy, dz = (round24(q[i] - p[i]) for i in range(3))
    limit = round24(r * r)
    sq = [round24(dx * dx), round24(dy * dy), round24(dz * dz)]
    first = hearing(rng, ped, posn)[0]
    for a, b, c in ((2, 1, 0), (1, 0, 2), (0, 1, 2), (1, 2, 0), (0, 2, 1)):      # z+y+x, y+x+z, x+y+z, y+z+x, x+z+y
        if (limit > round24(round24(sq[a] + sq[b]) + sq[c])) != first:
            return True
    return False


def f0(inp):
    res, parts = hearing(inp['range'], inp['ped'], inp['posn'])
    stats = {'in_range': res, 'out_of_range': not res, 'matrix': inp['has_mat'] == 1, 'no_matrix': inp['has_mat'] == 0,
             'nan_input': any(is_nan(v) for v in (inp['range'], *inp['ped'], *inp['posn'])),
             'inf_input': any((v & 0x7FFFFFFF) == 0x7F800000 for v in (inp['range'], *inp['ped'], *inp['posn'])),
             'denormal_input': any(is_denormal(v) for v in (inp['range'], *inp['ped'], *inp['posn'])),
             'tie': parts is not None and parts[0] == parts[1],
             'order_sensitive': order_sensitive(inp['range'], inp['ped'], inp['posn']),
             'huge': any(((v >> 23) & 0xFF) >= 127 + 60 and ((v >> 23) & 0xFF) != 0xFF for v in (*inp['ped'], *inp['posn'])),
             'negative_range': (inp['range'] >> 31) == 1 and not is_nan(inp['range'])}
    return {'al': 1 if res else 0, 'events': [], 'stats': stats}


def nonzero(kind, raw):
    return kind != 0 and (kind < 4 or raw != 0)


def f1(inp):
    nz = [nonzero(inp['kind'][i], inp['raw'][i]) for i in range(3)]
    res = any(nz)
    stats = {'any': res, 'none': not res, 'slot0': nz[0], 'slot1': nz[1], 'slot2': nz[2], 'only_last': nz == [False, False, True], 'only_first': nz == [True, False, False],
             'neighbour_nonzero_only': not res and (inp['before'] == 2 or inp['after'] == 2), 'raw_low_bytes_zero': any(inp['kind'][i] == 4 and (inp['raw'][i] & 0xFF) == 0 for i in range(3)),
             'all_three': all(nz)}
    return {'al': 1 if res else 0, 'events': [], 'stats': stats}


def entity_tag(kind, raw):
    """Tag() del valor de una ranura: entA/entB/entC son las arenas 1, 2 y 3 (intelA = 0); el objeto empieza tras la guarda."""
    if kind == 0:
        return 0
    if kind < 4:
        return base(kind) + KGUARD
    return raw


def f2(inp):
    events = []
    nz = [nonzero(inp['kind'][i], inp['raw'][i]) for i in range(3)]
    for i in range(3):
        if nz[i]:
            v = entity_tag(inp['kind'][i], inp['raw'][i])
            events.append([71, v, base(0) + KGUARD + KENTITIES + 4 * i, v])
    stats = {'none': not any(nz), 'calls1': sum(nz) == 1, 'calls2': sum(nz) == 2, 'calls3': sum(nz) == 3, 'slot0': nz[0], 'slot1': nz[1], 'slot2': nz[2],
             'gap_pattern': nz == [True, False, True], 'duplicate': sum(nz) >= 2 and len({(inp['kind'][i], inp['raw'][i]) for i in range(3) if nz[i]}) < sum(nz),
             'neighbour_nonzero': inp['before'] == 2 or inp['after'] == 2, 'raw_slot': any(inp['kind'][i] == 4 for i in range(3))}
    return {'events': events, 'slots_after': [0, 0, 0], 'other_changes': [], 'stats': stats}


def f3(inp):
    mask = 0xFFFFFFFF
    if inp['base_kind'] == 0:
        # tabla en la arena: argumento = base + delta (el valor bruto nunca cae sobre la arena)
        found = None
        if inp['arg_kind'] == 0:
            for i in range(COUNT):
                if inp['delta'] == (i * STRIDE) & mask:
                    found = i
                    break
        rel = inp['arg_kind'] == 0
    else:
        b = 0 if inp['base_kind'] == 1 else inp['base_raw']
        arg = (b + inp['delta']) & mask if inp['arg_kind'] == 0 else inp['arg_raw']
        found = None
        for i in range(COUNT):
            if arg == (b + i * STRIDE) & mask:
                found = i
                break
        rel = True
    eax = found if found is not None else 0
    stats = {'found': found is not None, 'not_found': found is None, 'found_zero': found == 0 and inp['arg_kind'] == 0 and inp['delta'] == 0, 'found_last': found == COUNT - 1,
             'found_mid': found is not None and 0 < found < COUNT - 1, 'null_table': inp['base_kind'] == 1, 'raw_table': inp['base_kind'] == 2, 'arena_table': inp['base_kind'] == 0,
             'raw_arg': inp['arg_kind'] == 1, 'just_past_end': inp['base_kind'] == 0 and inp['arg_kind'] == 0 and inp['delta'] == COUNT * STRIDE,
             'inside_entry': inp['base_kind'] == 0 and inp['arg_kind'] == 0 and inp['delta'] % STRIDE != 0 and inp['delta'] < COUNT * STRIDE,
             'before_table': inp['base_kind'] == 0 and inp['arg_kind'] == 0 and inp['delta'] >= 0x80000000}
    return {'eax': eax, 'events': [], 'stats': stats}


EXPECTED = dict(zip(NAMES, (f0, f1, f2, f3)))


def expected(function, inp, case=0):
    """(salida esperada de `observed` del lado EXE, estadisticas de cobertura)."""
    e = dict(EXPECTED[function](inp))
    stats = e.pop('stats')
    stats['family'] = inp['family']
    return e, stats


def check(function, row):
    """Compara el lado original del banco con el modelo; devuelve None si coincide o un texto con la diferencia."""
    inp = row['input']
    exp, _ = expected(function, inp)
    obs = row['original']['observed']
    if obs['events'] != exp['events']:
        return 'eventos %s != %s' % (obs['events'], exp['events'])
    for key in ('al', 'eax', 'slots_after', 'other_changes'):
        if key in exp and obs.get(key) != exp[key]:
            return '%s %s != %s' % (key, obs.get(key), exp[key])
    if function != NAMES[2]:
        if any(obs['changes']):
            return 'cambios en arenas: %s' % obs['changes']
    else:
        # solo las ranuras no nulas cambian; ninguna otra arena
        if any(obs['changes'][1:]):
            return 'cambios en arenas de entidades: %s' % obs['changes'][1:]
    return None


# ================================================================================================ autoprueba
def _selftest():
    one = 0x3F800000
    zero = 0
    # (3, 4, 0) a distancia 5: rango 5.0 -> 25 > 25 es falso; rango 5.000001 es verdadero
    assert hearing(0x40A00000, [zero] * 3, [0x40400000, 0x40800000, zero])[0] is False
    assert hearing(0x40A00001, [zero] * 3, [0x40400000, 0x40800000, zero])[0] is True
    # NaN -> falso; rango negativo: el cuadrado es positivo (-5 equivale a 5)
    assert hearing(0x7FC00000, [zero] * 3, [one, zero, zero])[0] is False
    assert hearing(0x41200000, [0x7FC00000, zero, zero], [one, zero, zero])[0] is False
    assert hearing(0xC0A00001, [zero] * 3, [0x40400000, 0x40800000, zero])[0] is True
    # los cuadrados enormes no desbordan (exponente extendido): 1e30^2 = 1e60 < (2e30)^2
    big = 0x7149F2CA
    assert hearing(0x71C9F2CA, [zero] * 3, [big, zero, zero])[0] is True
    # inf: rango inf contra coordenada finita -> verdadero; inf - inf -> NaN -> falso
    assert hearing(0x7F800000, [zero] * 3, [one, zero, zero])[0] is True
    assert hearing(0x7F800000, [0x7F800000, zero, zero], [0x7F800000, zero, zero])[0] is False
    # round24
    assert round24(Fraction(1) + Fraction(1, 2 ** 24)) == 1 and round24(Fraction(1) + Fraction(3, 2 ** 25)) == 1 + Fraction(1, 2 ** 23)
    assert round24(Fraction(-3, 7)) == -round24(Fraction(3, 7))
    # F1
    assert f1(dict(kind=[0, 0, 0], raw=[0, 0, 0], before=2, after=2))['al'] == 0
    assert f1(dict(kind=[0, 0, 4], raw=[0, 0, 0x100], before=0, after=0))['al'] == 1
    # F2: dos ranuras (1 y 3)
    ev = f2(dict(kind=[1, 0, 4], raw=[0, 0, 7], before=0, after=0))['events']
    assert ev == [[71, base(1) + 0x40, base(0) + 0x40 + 0x288, base(1) + 0x40], [71, 7, base(0) + 0x40 + 0x288 + 8, 7]]
    # F3
    p = dict(base_kind=0, base_raw=0, arg_kind=0, delta=5 * 0x34, arg_raw=0)
    assert f3(p)['eax'] == 5 and f3(dict(p, delta=43 * 0x34))['eax'] == 0 and f3(dict(p, delta=5 * 0x34 + 1))['eax'] == 0
    assert f3(dict(p, base_kind=1, delta=0x34 * 42))['eax'] == 42 and f3(dict(p, base_kind=1, delta=0))['eax'] == 0
    assert f3(dict(p, base_kind=2, base_raw=0xFFFFFF00, delta=0x34 * 9))['eax'] == 9
    assert f3(dict(p, arg_kind=1, arg_raw=0xFFFFFFFF))['eax'] == 0
    print('ok')


if __name__ == '__main__':
    _selftest()
