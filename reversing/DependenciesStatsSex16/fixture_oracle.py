"""Fixture finito completo y oraculo independiente de UpdateSexAppealStat (0x55BF20).

El oraculo repite el ensamblador del EXE paso a paso con floats de Python (un redondeo a float por cada punto donde el EXE guarda en memoria; el resto, a
la precision de 24 bits del proceso, equivale a float para estos rangos). Las comparaciones de NaN son ordenadas, como fcom. Si un NaN interviene, el patron
de bits depende de la regla de la x87 (mayor mantisa) y el oraculo no lo predice (devuelve None): en esos casos solo manda la igualdad EXE/DLL del banco.
"""
import math
import struct

CASES = 4096


def f32(x):
    try:
        return struct.unpack('<f', struct.pack('<f', x))[0]
    except OverflowError:
        return math.inf if x > 0 else -math.inf


def r24(x):
    """Redondeo a 24 bits de mantisa con exponente extendido: lo que deja una operacion x87 con PC24 antes de guardarse en memoria."""
    if x == 0 or not math.isfinite(x):
        return x
    m, e = math.frexp(x)
    return math.ldexp(round(m * 16777216.0) / 16777216.0, e)


def bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def floating(x):
    return struct.unpack('<f', struct.pack('<I', x))[0]


CLASSES = (-128, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 127)
# Indices 0..15: los del banco original; 16..31: estres (cero negativo, subnormal minimo, minimo normal, NaN callado con carga, NaN senalizante, NaN negativo, +-inf, +-FLT_MAX, +-1e30, 1e-30, 1, 100, 1024).
HEALTH_BITS = (bits(-10.), bits(0.), bits(1.), bits(499.9999694824219), bits(500.), bits(500.0000305175781), bits(750.), bits(999.9999389648438),
               bits(1000.), bits(1000.0000610351562), bits(1500.), bits(2000.), bits(-1000.), bits(250.), bits(600.), bits(800.),
               0x80000000, 0x00000001, 0x00800000, 0x7FC01234, 0x7F800001, 0xFFC00001, 0x7F800000, 0xFF800000,
               0x7F7FFFFF, 0xFF7FFFFF, bits(1e30), bits(-1e30), bits(1e-30), bits(1.), bits(100.), bits(1024.))
# Indices 0..7: los del banco original; 8..15: estres (cero negativo, subnormal minimo, NaN con carga, NaN senalizante negativo, +-inf, FLT_MAX, 1e-30).
TATTOO_BITS = (bits(-1000.), bits(-1.), bits(0.), bits(1.), bits(500.), bits(1000.), bits(2000.), bits(4000.),
               0x80000000, 0x00000001, 0x7FC0ABCD, 0xFF800001, 0x7F800000, 0xFF800000, 0x7F7FFFFF, bits(1e-30))
# Indices 0..7: los del banco original; 8..15: estres (NaN, +-inf, 1e30, FLT_MAX, subnormal minimo, -34.99999, cero negativo).
DISTANCE_BITS = (bits(0.), bits(1.), bits(34.999996185302734), bits(35.), bits(35.000003814697266), bits(36.), bits(-35.), bits(100.),
                 0x7FC00001, 0x7F800000, 0xFF800000, bits(1e30), 0x7F7FFFFF, 0x00000001, bits(-34.99999), 0x80000000)
# Tabla de 11 bytes de 0x55C0B4 (clase - 1 = 0..10); fuera de rango (ja 0x55C040) usa el indice 3.
TABLE = (0, 1, 1, 2, 0, 2, 0, 3, 1, 2, 0)
FACTOR = (f32(.1), None, f32(.3), .5)  # indices del salto de 0x55C0A4: 0x55C038, 0x55C04C (sin factor), 0x55C030, 0x55C040


def fixture(case):
    p, v = case % 128, case // 128
    return dict(profile=p, value=v, player_mode=p % 4, cache_mode=p // 4 % 3, reference_mode=p // 32 % 3, vehicle_matrix=bool(p & 16), ped_matrix=bool(p & 64),
                cheat=bool(v & 1), vehicle_class=CLASSES[p % 16], health_bits=HEALTH_BITS[v], tattoo_bits=TATTOO_BITS[(p + 3 * v) % 16],
                distance_bits=DISTANCE_BITS[p // 4 % 16])


def player_distance_source(mode):
    """FindPlayerCoors: 0 = sin jugador (vector cero), 1 = jugador fuera, 2 = jugador con bInVehicle pero sin vehiculo (su propia posicion), 3 = vehiculo actual (posicion 0)."""
    return mode in (1, 2)


def result(i):
    """(bits del float final de B793E4 o None si lo decide la regla de NaN de la x87, rutas recorridas)."""
    paths = set()
    live = i['player_mode'] == 3
    cached = live or i['cache_mode'] != 0
    paths.add(('ped_null', 'ped_outside', 'vehicle_null', 'vehicle_live')[i['player_mode']])
    paths.add('cached_present' if cached else 'cached_null')
    nan = False
    local1 = f32(floating(i['tattoo_bits']) * .5)          # fld; fmul 0.5; fstp [esp+0xC]
    nan |= math.isnan(local1)
    st0 = 0.                                                 # [esp] = 0.0f (rutas hacia 0x55C048)
    if cached:
        paths.add('matrix' if i['vehicle_matrix'] else 'placeable')
        paths.add('ped_matrix' if i['ped_matrix'] else 'ped_placeable')
        if live:
            paths.add(('reference_new', 'reference_duplicate', 'reference_existing')[i['reference_mode']])
        d = floating(i['distance_bits']) if player_distance_source(i['player_mode']) else 0.
        # dx = target.x - player.x (target = origen), dy = 0 - 0; dy*dy + dx*dx; fsqrt; fcomp 35.0f; test ah,5; jp: salta si no (distancia < 35)
        dx = r24(0. - d)
        total = r24(r24(0. * 0.) + r24(dx * dx))
        distance = r24(math.sqrt(total)) if total == total else math.nan
        if distance == distance:
            paths.add('distance_below' if distance < 35. else 'distance_equal' if distance == 35. else 'distance_above')
        else:
            nan = True
            paths.add('distance_above')
        if distance < 35.:                                   # NaN: la comparacion es falsa -> salta
            h = floating(i['health_bits'])
            local0 = f32(r24(r24(h + h) - 1000.)) if not math.isnan(h) else math.nan
            if math.isnan(local0):
                nan = True
                paths.add('health_nan')
                st0 = math.nan
            elif 1000. < local0:
                paths.add('health_above')
                st0 = 1000.
            else:
                st0 = local0
                if 0. > local0:
                    paths.add('health_below')
                    st0 = 0.
                else:
                    paths.add('health_zero' if local0 == 0. else 'health_max' if local0 == 1000. else 'health_inside')
            cls = i['vehicle_class']
            index = TABLE[cls - 1] if 1 <= cls <= 11 else 3
            paths.add('class_factor_' + str(index))
            if FACTOR[index] is not None and not math.isnan(st0):
                st0 = r24(st0 * FACTOR[index])
    x = r24(st0 * .5) if not math.isnan(st0) else st0      # 0x55C04C: fmul 0.5; fadd [esp+4]
    x = r24(x + local1)
    if math.isnan(x):
        nan = True
        paths.add('result_nan')
    elif x < 0:
        paths.add('result_below')
        x = 0.
    else:
        paths.add('result_zero' if x == 0 else 'result_above' if x > 1000. else 'result_max' if x == 1000. else 'result_inside')
        if x > 1000.:
            x = 1000.
    paths.add('cheat_on' if i['cheat'] else 'cheat_off')
    if i['cheat']:
        return bits(1000.), paths
    return (None if nan else bits(f32(x))), paths


REQUIRED = {'ped_null', 'ped_outside', 'vehicle_null', 'vehicle_live', 'cached_null', 'cached_present', 'matrix', 'placeable', 'ped_matrix', 'ped_placeable',
            'reference_new', 'reference_duplicate', 'reference_existing', 'distance_below', 'distance_equal', 'distance_above', 'health_below', 'health_zero',
            'health_inside', 'health_max', 'health_above', 'health_nan', 'class_factor_0', 'class_factor_1', 'class_factor_2', 'class_factor_3', 'result_below',
            'result_zero', 'result_inside', 'result_max', 'result_above', 'result_nan', 'cheat_on', 'cheat_off'}
# Entradas de estres que deben aparecer en los casos que de verdad recorren la parte aritmetica (distancia < 35 con vehiculo): NaN con carga, senalizante, +-inf, cero negativo, subnormal.
STRESS_HEALTH = {0x7FC01234, 0x7F800001, 0xFFC00001, 0x7F800000, 0xFF800000, 0x80000000, 0x00000001, 0x7F7FFFFF}
STRESS_TATTOO = {0x7FC0ABCD, 0xFF800001, 0x7F800000, 0xFF800000, 0x80000000, 0x00000001, 0x7F7FFFFF}
STRESS_DISTANCE = {0x7FC00001, 0x7F800000, 0xFF800000, 0x7F7FFFFF, 0x00000001, 0x80000000}


def coverage_of(inputs):
    paths, health, tattoo, distance = set(), set(), set(), set()
    for i in inputs:
        r = result(i)
        paths |= r[1]
        if 'distance_below' in r[1]:
            health.add(i['health_bits'])
        tattoo.add(i['tattoo_bits'])
        if i['player_mode'] in (1, 2) and i['cache_mode'] != 0:
            distance.add(i['distance_bits'])
    return paths, health, tattoo, distance


if __name__ == '__main__':
    paths, health, tattoo, distance = coverage_of(fixture(case) for case in range(CASES))
    missing = REQUIRED - paths
    if missing:
        raise RuntimeError('rutas sin cubrir: ' + str(sorted(missing)))
    for name, wanted, seen in (('salud', STRESS_HEALTH, health), ('apariencia', STRESS_TATTOO, tattoo), ('distancia', STRESS_DISTANCE, distance)):
        if not wanted <= seen:
            raise RuntimeError('estres de %s sin cubrir: %s' % (name, sorted(hex(x) for x in wanted - seen)))
    print('%d fixtures; rutas completas: %d; estres de salud/apariencia/distancia cubierto' % (CASES, len(paths)))
