"""Oraculo independiente del banco Dependencies24B (diPadInit 0x7485C0).

Repite EXACTAMENTE las formulas de Mix/Rand/Make/BuildTable/ExpectedResidue de Fixtures.hpp y modela, sin ejecutar nada, lo que debe hacer la funcion segun el desensamblado
del EXE (ver diPadInit.dis.txt): tabla final de pads, punteros de dispositivo finales, traza completa de colaboradores (con el hash FNV de la tabla en cada llamada) y
todos los valores que los falsos ven (cabecera del DIPROPDWORD incluida). run_local.py compara las dos pasadas (EXE y DLL) con este modelo ademas de entre si.
"""
M = 0xFFFFFFFF
CASES = 2048
HR = [0x00000000, 0x00000001, 0x80004005, 0x80004001, 0x8007000E, 0x80070057, 0x8007000C, 0x8007001E, 0x80070002, 0x7FFFFFFF, 0xFFFFFFFF, 0x80000000]
VID = [0, 0xFFFFFFFF, 0x00010000, 0x0000FFFF, 0x045E028E, 0x054C09CC, 0x80000000, 1, 0x12345678, 0xDEADBEEF, 0x0001FFFF, 0xFFFF0000]
SEEDS = [0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555]
GOLDEN = 0x9E3779B1
CONTROLS = 0xB70198
MODES = {0: (0, 0), 1: (1, 0), 2: (0, 2), 3: (1, 2), 4: (1, 1), 5: (2, 1)}


def mix(x):
    x &= M
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M
    x ^= x >> 15
    x = (x * 0x846CA68B) & M
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M)


def make(c):
    f = dict(case=c, dev_mode=c % 6, mutator=(c // 6) % 8, enum_ret=HR[rnd(c, 1) % 12], caps_hr=[0, 0], prop_hr=[0, 0], vid=[0, 0], prop_mode=[0, 0])
    for i in range(2):
        f['caps_hr'][i] = HR[rnd(c, 2 + i) % 12]
        f['prop_hr'][i] = 0 if rnd(c, 4 + i) % 3 == 0 else HR[rnd(c, 6 + i) % 12]
        f['vid'][i] = rnd(c, 10 + i) if rnd(c, 8 + i) % 3 == 0 else VID[rnd(c, 12 + i) % 12]
        r = rnd(c, 14 + i) % 10
        f['prop_mode'][i] = 0 if r < 7 else r - 6
    f['caps_fill'] = rnd(c, 16)
    f['stack_seed'] = SEEDS[c] if c < len(SEEDS) else rnd(c, 17)
    f['table_kind'] = rnd(c, 18) % 5
    f['table_seed'] = rnd(c, 19)
    return f


def build_table(f):
    t = bytearray(0x20)
    k, s = f['table_kind'], f['table_seed']
    if k == 1:
        t[:] = b'\xff' * 0x20
    elif k == 2:
        for i in range(0x20):
            t[i] = rnd(s, i) & 0xFF
    elif k == 3:
        for p in range(2):
            e = 16 * p
            t[e:e + 4] = rnd(s, 100 + p).to_bytes(4, 'little')
            t[e + 4] = t[e + 5] = t[e + 6] = 1
            t[e + 7] = 0x5A
            t[e + 8:e + 12] = (rnd(s, 102 + p) & 0xFFFF).to_bytes(4, 'little')
            t[e + 12:e + 16] = (rnd(s, 104 + p) & 0xFFFF).to_bytes(4, 'little')
    elif k == 4:
        t[:] = b'\xa5' * 0x20
    return t


def slot(seed, k):
    return (seed ^ (k * GOLDEN)) & M


def residue(seed):
    return [slot(seed, 17), slot(seed, 16), slot(seed, 15), slot(seed, 14), slot(seed, 13)]


def fnv(b):
    h = 2166136261
    for x in b:
        h ^= x
        h = (h * 16777619) & M
    return h


def simulate(f):
    """Devuelve (tabla final, [dev1, dev2], traza, estadisticas)."""
    t = build_table(f)
    ps = [2, 1]
    trace = []
    stats = dict(garbage1=0, garbage2=0, dev2_header_residue=0, mutated=0, caps_calls=0, prop_calls=0, init_calls=0)
    for p in (0, 1):
        t[16 * p:16 * p + 4] = bytes(4)
        t[16 * p + 4:16 * p + 7] = bytes(3)
    trace += [1, fnv(t)]
    ps = list(MODES[f['dev_mode']])
    prop = residue(f['stack_seed'])
    m = f['mutator']

    def effect(who, ev):
        before = list(ps)
        if who == 1 and ev == 'caps':
            if m == 1: ps[0] = 2
            if m == 7: ps[1] = 0
        if who == 1 and ev == 'prop':
            if m == 2: ps[1] = 0
            if m == 3: ps[1] = 2
            if m == 4: ps[0] = 0
        if ev == 'init':
            if m == 5: ps[1] = 0
            if m == 6: ps[1] = 1
        if ps != before:
            stats['mutated'] += 1

    def getprop(dev):
        trace.extend([3, dev, 0x18, 1] + prop + [fnv(t)])
        stats['prop_calls'] += 1
        hr, vid, mode = f['prop_hr'][dev - 1], f['vid'][dev - 1], f['prop_mode'][dev - 1]
        ok = (hr & 0x80000000) == 0
        wrote = False
        if mode == 0 and ok:
            prop[4] = vid; wrote = True
        elif mode == 1:
            prop[4] = vid; wrote = True
        elif mode == 3 and ok:
            prop[4] = vid; prop[2] = 0x77; wrote = True
        effect(dev, 'prop')
        return wrote

    if ps[0]:
        t[4] = 1
        dev = ps[0]
        trace += [2, dev, 0x2C, 1, fnv(t)]
        stats['caps_calls'] += 1
        effect(dev, 'caps')
        prop[0:4] = [0x14, 0x10, 0, 0]
        dev = ps[0]
        wrote = getprop(dev)
        stats['garbage1'] += (not wrote)
        t[8:12] = (prop[4] & 0xFFFF).to_bytes(4, 'little')
        t[12:16] = (prop[4] >> 16).to_bytes(4, 'little')
        trace += [4, CONTROLS, 0x20, fnv(t)]
        stats['init_calls'] += 1
        effect(0, 'init')
    elif ps[1]:
        stats['dev2_header_residue'] = 1
    dev = ps[1]
    if dev:
        wrote = getprop(dev)
        stats['garbage2'] += (not wrote)
        t[24:28] = (prop[4] & 0xFFFF).to_bytes(4, 'little')
        t[28:32] = (prop[4] >> 16).to_bytes(4, 'little')
        t[20] = 1
    return t, ps, trace, stats


def input_row(f):
    """Fila `input` que debe registrar el banco para el caso f."""
    table = build_table(f)
    return {'case': f['case'], 'dev_mode': f['dev_mode'], 'mutator': f['mutator'], 'table_kind': f['table_kind'], 'table_in': table.hex(), 'prop_mode': f['prop_mode'],
            'enum_ret': f['enum_ret'], 'caps_hr': f['caps_hr'], 'prop_hr': f['prop_hr'], 'vid': f['vid'], 'caps_fill': f['caps_fill'], 'stack_seed': f['stack_seed'],
            'table_seed': f['table_seed'], 'residue': residue(f['stack_seed'])}


def observed_row(f):
    t, ps, trace, stats = simulate(f)
    return {'table': bytes(t).hex(), 'dev1': ps[0], 'dev2': ps[1], 'traps': 0, 'trace': trace}, stats


if __name__ == '__main__':
    import collections
    c = collections.Counter()
    for i in range(CASES):
        _, st = observed_row(make(i))
        for k, v in st.items():
            c[k] += v
    print(dict(c))
