"""Oraculo independiente del banco GaragesImpound30: CGarages::StoreCarInNearestImpoundingGarage (0x44A3C0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. x87 con control de precision a 24 bits y exponente extendido (ver x87model.py).
cdecl, [esp+4] = CVehicle* (bloque falso); void. NumGarages = N (compara sin signo: `jbe` si es 0). Variables: esi = mejor indice (-1), edx = indice, [esp+0Ch] = distancia minima (float) = 0x47C34FF3.

  44A3F0 para cada garaje i (ecx = &aGarages[i].m_vPosn, paso 0xD8):  al = byte [ecx+4Ch] (m_nType): si al < 0x21 o al > 0x23 se salta
  44A3FB   gx = [ecx] ; gy = float [ecx+4] (fld + fstp) ; eax = [vehiculo+14h] (matriz) : con matriz ptr = matriz + 0x30, sin ella ptr = vehiculo + 4
           dx = ptr.x - gx (fsubr, registro, 24 bits) ; dy = ptr.y - gy (fsub, registro) ; d = fsqrt(dy*dy + dx*dx) (todo en registro, 24 bits)
           fcom [min] ; `test ah,5 ; jp` => solo si d < min (ordenado): min = float(d) (fstp dword) y esi = i  (empate o NaN: no cambia)
  44A44C si esi < 0: sale. Si no: eax = FindSafeHouseIndexForGarageType(byte [aGarages[esi].m_nType]) (0x4489F0; ECX = 0 se conserva) ; ranuras = 0x96ABD8 + eax * 0x100 (4 de 0x40 bytes)
  44A47B cuenta = (modelo0 > 0) + (modelo1 > 0) + (modelo2 > 0) con el modelo uint16 en +0x12 de cada ranura (la cuarta no cuenta) ; si cuenta == 3: ranura0 <- ranura1 (0x40 bytes) ;
         ranura1 <- ranura2 ; cuenta = 2 ; finalmente CStoredCar::StoreCar(&ranura[cuenta], vehiculo) (0x449760, thiscall, ECX = ranura, `ret 4`)

Falsos (identicos en ambos lados): FindSafeHouseIndexForGarageType devuelve find[tipo - 0x21] (registra (1, tipo & 0xFF)); StoreCar registra (2, indice de ranura en la tabla (0..79), 1 si el vehiculo es el bloque falso)
y escribe en la ranura u32 0xC0DE0000 | indice en +0 y u16 0x7777 en +0x12.
Observado por lado: {'changed': [[trozo 0..79, 0x40 bytes en hex], ...] (trozos distintos de la referencia), 'trace': [[tipo, a, b], ...], 'garages_ok', 'num', 'veh_ok'}.
"""
import json
import random
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
from x87model import *  # noqa: E402,F401,F403

CASES = 4096
NAME = 'CGarages::StoreCarInNearestImpoundingGarage'
NAMES = [NAME]
ENTRY_BYTES = 0xD8
MAX_GARAGES = 50
GARAGE_BYTES = MAX_GARAGES * ENTRY_BYTES
GARAGE_ADDRESS = 0x96C048
NUM_ADDRESS = 0x96C024
SAFEHOUSES = 20
SLOTS_PER_HOUSE = 4
SLOT_BYTES = 0x40
HOUSE_BYTES = SLOTS_PER_HOUSE * SLOT_BYTES
TABLE_BYTES = SAFEHOUSES * HOUSE_BYTES
TABLE_ADDRESS = 0x96ABD8
CHUNKS = SAFEHOUSES * SLOTS_PER_HOUSE
VEHICLE_BYTES = 0x40
MATRIX_BYTES = 0x40
INITIAL_MIN = 0x47C34FF3
CONSTANTS = {'entry_bytes': ENTRY_BYTES, 'max_garages': MAX_GARAGES, 'garage_bytes': GARAGE_BYTES, 'garage_address': GARAGE_ADDRESS, 'num_address': NUM_ADDRESS, 'safehouses': SAFEHOUSES,
             'slots_per_house': SLOTS_PER_HOUSE, 'slot_bytes': SLOT_BYTES, 'house_bytes': HOUSE_BYTES, 'table_bytes': TABLE_BYTES, 'table_address': TABLE_ADDRESS, 'chunks': CHUNKS,
             'vehicle_bytes': VEHICLE_BYTES, 'matrix_bytes': MATRIX_BYTES, 'initial_min': INITIAL_MIN, 'cases': CASES}


def _less(dist, minimum):
    """fcom + test ah,5 + jp: solo si dist < minimo y ordenado (NaN o desconocido: no)."""
    if dist.is_nan():
        return False
    return key(dist) < key(minimum)


def simulate(inp):
    """-> (esperado, estadisticas)."""
    garages = inp['garages']
    vx, vy = mem(inp['veh_pos'][0]), load(inp['veh_pos'][1])      # fsubr [eax] (memoria) y fld [eax+4]
    best = -1
    minimum = load(INITIAL_MIN)
    candidates = 0
    ties = 0
    for i, (typ, xb, yb) in enumerate(garages):
        typ &= 0xFF
        if typ < 0x21 or typ > 0x23:
            continue
        candidates += 1
        gx, gy = load(xb), fl(load(yb))
        dx = sub(vx, gx)
        dy = sub(vy, gy)
        dist = sqrt(add(mul(dy, dy), mul(dx, dx)))
        if _less(dist, minimum):
            minimum = fl(dist)
            best = i
        elif not dist.is_nan() and dist.kind != 'unk' and key(dist) == key(minimum) and best >= 0:
            ties += 1
    houses = {idx: bytearray(bytes.fromhex(h)) for idx, h in inp['houses']}
    table = bytearray([inp['fill']] * TABLE_BYTES)
    for idx, b in houses.items():
        table[idx * HOUSE_BYTES:(idx + 1) * HOUSE_BYTES] = b
    ref = bytes(table)
    trace = []
    count = 0
    shifted = False
    chosen = None
    if best >= 0:
        typ = garages[best][0] & 0xFF
        trace.append([1, typ, 0])
        house = inp['find'][typ - 0x21]
        base = house * HOUSE_BYTES
        models = [struct.unpack_from('<H', table, base + s * SLOT_BYTES + 0x12)[0] for s in range(3)]
        count = sum(1 for m in models if m > 0)
        if count == 3:
            table[base:base + SLOT_BYTES] = table[base + SLOT_BYTES:base + 2 * SLOT_BYTES]
            table[base + SLOT_BYTES:base + 2 * SLOT_BYTES] = table[base + 2 * SLOT_BYTES:base + 3 * SLOT_BYTES]
            count = 2
            shifted = True
        chunk = house * SLOTS_PER_HOUSE + count
        trace.append([2, chunk, 1])
        off = chunk * SLOT_BYTES
        table[off:off + 4] = struct.pack('<I', 0xC0DE0000 | chunk)
        table[off + 0x12:off + 0x14] = struct.pack('<H', 0x7777)
        chosen = (typ, house, count)
    changed = []
    for ch in range(CHUNKS):
        a, b = ref[ch * SLOT_BYTES:(ch + 1) * SLOT_BYTES], bytes(table[ch * SLOT_BYTES:(ch + 1) * SLOT_BYTES])
        if a != b:
            changed.append([ch, b.hex()])
    want = {'changed': changed, 'trace': trace, 'garages_ok': True, 'num': len(garages), 'veh_ok': True}
    ins = [vx, vy] + [load(g[1]) for g in garages] + [load(g[2]) for g in garages]
    stats = {'family': inp['kind']}
    stats['n_garages_0'] = len(garages) == 0
    stats['n_garages_ge_40'] = len(garages) >= 40
    stats['n_garages_50'] = len(garages) == MAX_GARAGES
    stats['no_candidate'] = candidates == 0
    stats['has_candidate'] = candidates > 0
    stats['no_best'] = best < 0 and candidates > 0
    stats['best_first'] = best == 0
    stats['best_last'] = best >= 0 and best == len(garages) - 1
    stats['best_found'] = best >= 0
    stats['tie_kept_first'] = ties > 0
    stats['nan_in'] = any(v.is_nan() for v in ins)
    stats['inf_in'] = any(v.kind == 'inf' for v in ins)
    stats['huge_in'] = any(v.kind == 'f' and abs(v.val) >= 10 ** 19 for v in ins)
    stats['tiny_in'] = any(v.kind == 'f' and v.val != 0 and abs(v.val) < Fraction(1, 10 ** 19) for v in ins)
    stats['negzero_in'] = any(v.is_zero() and v.neg for v in ins)
    stats['matrix'] = inp['veh_matrix'] == 1
    stats['placement'] = inp['veh_matrix'] == 0
    stats['shifted'] = shifted
    stats['other_pos_differs'] = inp['veh_other'][:2] != inp['veh_pos'][:2]
    for t in (0x21, 0x22, 0x23):
        stats['type_%x' % t] = chosen is not None and chosen[0] == t
    for c in range(3):
        stats['count_%d' % c] = chosen is not None and chosen[2] == c
    stats['count_2_after_shift'] = shifted
    stats['house_0'] = chosen is not None and chosen[1] == 0
    stats['house_19'] = chosen is not None and chosen[1] == SAFEHOUSES - 1
    stats['trace_2'] = len(trace) == 2
    stats['near_initial_min'] = any(g[0] & 0xFF in (0x21, 0x22, 0x23) and load(g[1]).kind == 'f' and 99999 <= abs(load(g[1]).val) <= 100001 for g in garages)
    stats['type_boundary'] = any((g[0] & 0xFF) in (0x20, 0x24) for g in garages)
    stats['type_high_bit'] = any((g[0] & 0xFF) >= 0x80 for g in garages)
    return want, stats


def expected(inp, case):
    return simulate(inp)


def matches(obs, want):
    return obs == want


def selftest():
    f32 = lambda v: struct.unpack('<f', struct.pack('<f', v))[0]
    bits = lambda v: struct.unpack('<I', struct.pack('<f', v))[0]
    import math

    def house(models, fillbyte=0x5A):
        b = bytearray([fillbyte] * HOUSE_BYTES)
        for s, m in enumerate(models):
            struct.pack_into('<H', b, s * SLOT_BYTES + 0x12, m)
        return b.hex()

    def run(garages, vpos=(0, 0), models=(0, 0, 0, 0), find=(3, 4, 5), matrix=0):
        inp = {'kind': 0, 'garages': [[t, bits(x), bits(y)] for t, x, y in garages], 'veh_matrix': matrix, 'veh_pos': [bits(vpos[0]), bits(vpos[1]), 0], 'veh_other': [1, 2, 3], 'find': list(find),
               'fill': 0xA5, 'houses': [[find[0], house(models)], [find[1], house(models)], [find[2], house(models)]]}
        return simulate(inp)

    # sin garajes o sin garajes de embargo: nada
    want, st = run([])
    assert want['changed'] == [] and want['trace'] == []
    want, st = run([(0x20, 1.0, 1.0), (0x24, 1.0, 1.0), (0xA1, 1.0, 1.0)])
    assert want['changed'] == [] and want['trace'] == []
    # un garaje de embargo 0x22: refugio find[1] = 4, ranuras vacias -> cuenta 0 -> ranura 16 (4*4+0)
    want, st = run([(0x22, 10.0, 10.0)])
    assert want['trace'] == [[1, 0x22, 0], [2, 16, 1]] and [c for c, _ in want['changed']] == [16], want
    assert st['type_22'] and st['count_0']
    # el mas cercano gana; empate: gana el primero
    want, st = run([(0x21, 5.0, 0.0), (0x23, 3.0, 4.0), (0x22, 0.0, 6.0), (0x21, 4.0, 0.0)], vpos=(0.0, 0.0))
    assert want['trace'][0] == [1, 0x21, 0] and want['trace'][1][1] == 3 * 4 + 0, want['trace']   # 4.0 es el mas cercano: tipo 0x21, refugio find[0]=3
    want, st = run([(0x21, 5.0, 0.0), (0x23, 3.0, 4.0)], vpos=(0.0, 0.0))
    assert want['trace'][0] == [1, 0x21, 0] and st['tie_kept_first']
    # ocupacion: cuenta de las tres primeras ranuras; la cuarta no cuenta
    for models, count in (((0, 0, 0, 7), 0), ((1, 0, 0, 0), 1), ((0, 0xFFFF, 0, 0), 1), ((0x8000, 5, 0, 0), 2), ((0, 3, 4, 9), 2)):
        want, st = run([(0x21, 1.0, 0.0)], models=models)
        assert want['trace'][1] == [2, 3 * 4 + count, 1], (models, want['trace'])
    # tres ocupadas: desplazamiento y escritura en la ranura 2
    want, st = run([(0x21, 1.0, 0.0)], models=(1, 2, 3, 4))
    assert want['trace'][1] == [2, 3 * 4 + 2, 1] and st['shifted']
    changed = dict(want['changed'])
    assert set(changed) == {12, 13, 14}
    assert struct.unpack_from('<H', bytes.fromhex(changed[12]), 0x12)[0] == 2 and struct.unpack_from('<H', bytes.fromhex(changed[13]), 0x12)[0] == 3
    assert struct.unpack_from('<IH', bytes.fromhex(changed[14]), 0)[0] == 0xC0DE000E
    # distancia inicial: mas alla de 99999.9 no gana
    want, st = run([(0x21, 100000.0, 0.0)])
    assert want['trace'] == [] and st['no_best']
    want, st = run([(0x21, 99999.0, 0.0)])
    assert want['trace'] != []
    # NaN no gana; infinito no gana
    want, st = run([(0x21, float('nan'), 0.0), (0x22, float('inf'), 0.0)])
    assert want['trace'] == []
    # exponente extendido: 2e-30 y 1e-30 (sus cuadrados desbordan por abajo en float puro y darian ambos 0): en la x87 el segundo es mas cercano y gana (en float puro ganaria el primero)
    want, st = run([(0x21, 2.0e-30, 0.0), (0x22, 1.0e-30, 0.0)], vpos=(0.0, 0.0))
    assert want['trace'][0][1] == 0x22, want['trace']
    # lo mismo con grandes: dx = 3e19 (cuadrado 9e38 desborda float): no gana (d = 3e19 > 99999), sin error
    want, st = run([(0x21, 3.0e19, 0.0)])
    assert want['trace'] == []
    # coherencia con una cadena float32 por operacion en posiciones tipicas
    rng = random.Random(30)
    for _ in range(2000):
        n = rng.randint(1, 8)
        gs = [(rng.choice([0x20, 0x21, 0x22, 0x23, 0x24]), f32(rng.uniform(-300, 300)), f32(rng.uniform(-300, 300))) for _ in range(n)]
        vp = (f32(rng.uniform(-300, 300)), f32(rng.uniform(-300, 300)))
        want, st = run(gs, vpos=vp)
        mn = struct.unpack('<f', struct.pack('<I', INITIAL_MIN))[0]
        best = -1
        for i, (t, gx, gy) in enumerate(gs):
            if not 0x21 <= t <= 0x23:
                continue
            dx, dy = f32(vp[0] - gx), f32(vp[1] - gy)
            d = f32(math.sqrt(f32(f32(dy * dy) + f32(dx * dx))))
            if d < mn:
                mn, best = d, i
        assert (want['trace'][0][1] == gs[best][0]) if best >= 0 else want['trace'] == [], (gs, vp, want['trace'])
    print('ok')


def crosscheck(path):
    """Autoprueba cruzada: el volcado de Fixtures.hpp (fixture_dump.cpp compilado con cl) repite las constantes y produce filas que el oraculo acepta."""
    lines = Path(path).read_text(encoding='utf-8').splitlines()
    consts = json.loads(lines[0])
    assert consts == CONSTANTS, (consts, CONSTANTS)
    rows = [json.loads(s) for s in lines[1:]]
    assert len(rows) == CASES
    for r in rows:
        assert len(r['garages']) <= MAX_GARAGES and all(0 <= f < SAFEHOUSES for f in r['find']) and 1 <= len(r['houses']) <= 3
        assert all(len(h) == 2 * HOUSE_BYTES and 0 <= i < SAFEHOUSES for i, h in r['houses'])
        assert {i for i, _ in r['houses']} == set(r['find'])
        simulate(r)
    print('ok')


if __name__ == '__main__':
    if len(sys.argv) == 3 and sys.argv[1] == '--crosscheck':
        crosscheck(sys.argv[2])
    else:
        selftest()
