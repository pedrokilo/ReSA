"""Espejo en Python del generador de Fixtures.hpp (MakeF) para calibrar la cobertura de run_local.py SIN juego.

Reproduce en aritmetica de 32 bits exactamente el generador C++ (mismos Mix/Rand/RandFloat/Pick/Perturb/Delta); NO se ha comprobado contra la compilacion C++ (el banco no se compilo): si el
coordinador ve que `comparisons.jsonl` trae filas `input` distintas de las de este espejo, hay que corregir uno de los dos. `python -I fixture_mirror.py` imprime la cobertura de las
4096 fixtures segun el oraculo y falla si alguna fila produce un valor invalido.
"""
import json
import struct
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

M = 0xFFFFFFFF
CASES = 4096
WIN_BYTES = 0x60
KSPECIAL = [0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x3F3504F3, 0xBF3504F3, 0x40000000, 0x3E800000, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001, 0x80000001,
            0x007FFFFF, 0x00800000, 0x3F7FFFFF, 0x3F800001, 0x34000000, 0x33800000, 0x4B800000, 0x7F000000, 0x7E800000, 0x5F800000, 0x1F800000, 0x20000000, 0x3FC00000, 0xC0400000]
KINF = [0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000]
KNAN = [0x7FC00000, 0xFFC00000, 0x7FC00001, 0xFFC00001, 0x7FC12345, 0xFFC54321, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FC00002, 0x7FE00000, 0xFFE00000, 0x7FC7FFFE]
KZEROONE = [0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x3F800000, 0xBF800000]
KT = [0x00000000, 0x80000000, 0x3F800000, 0x3F000000, 0xBF800000, 0x40000000, 0x3F7FFFFF, 0x3F800001, 0x3F4CCCCD, 0x00000001,
      0x0DA24260, 0x41000000, 0xC1000000, 0x42C80000, 0x7FC00000, 0xFFC00012, 0x34000000, 0x3EAAAAAB, 0xBE800000, 0x47C35000]
KUNIT = [[0x00000000, 0x00000000, 0x00000000, 0x3F800000], [0x3F800000, 0x00000000, 0x00000000, 0x00000000], [0x00000000, 0x3F800000, 0x00000000, 0x00000000],
         [0x00000000, 0x00000000, 0x3F800000, 0x00000000], [0x3F000000, 0x3F000000, 0x3F000000, 0x3F000000], [0x3F000000, 0x3F000000, 0x3F000000, 0xBF000000],
         [0x3F000000, 0xBF000000, 0x3F000000, 0xBF000000], [0x3F19999A, 0x3F4CCCCD, 0x00000000, 0x00000000], [0x3F3504F3, 0x00000000, 0x00000000, 0x3F3504F3],
         [0x00000000, 0x3F3504F3, 0x3F3504F3, 0x00000000]]
KORTH = [[0, 1], [0, 2], [1, 2], [1, 3], [2, 3], [4, 6]]
KZ = [0x00000000, 0x80000000, 0x00000001, 0x80000001, 0x007FFFFF, 0x00800000, 0x3F800000, 0xBF800000, 0x20000000, 0x5F800000, 0x3F000000]
KSTACK = [0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555]


def mix(x):
    x &= M
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M
    x ^= x >> 15
    x = (x * 0x846CA68B) & M
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M)


def fill_words(n, seed):
    out = bytearray()
    for i in range(0, n, 4):
        out += struct.pack('<I', rand(seed, i // 4))
    return bytes(out[:n])


def stack_seed(n):
    return KSTACK[n] if n < 8 else rand(n + 0x1000, 99)


def rand_float(c, k, lo, hi):
    r, s = rand(c, k), rand(c, k + 1000)
    span = hi - lo + 1
    e = (lo + (s % span) + 127) & M
    return (((r >> 31) << 31) | (e << 23) | (r & 0x7FFFFF)) & M


def pick(a, c, k):
    return a[rand(c, k) % len(a)]


def quiet(b):
    if (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) and not (b & 0x400000):
        b |= 0x400000
    return b


def mix_component(c, k):
    r = rand(c, k) % 7
    if r == 0:
        return pick(KNAN, c, k + 100)
    if r == 1:
        return pick(KINF, c, k + 100)
    if r == 2:
        return pick(KZEROONE, c, k + 100) & 0x80000000
    if r == 3:
        return rand(c, k + 100) & 0x807FFFFF
    if r == 4:
        return rand_float(c, k + 100, 100, 127)
    if r == 5:
        return KSPECIAL[rand(c, k + 100) % len(KSPECIAL)]
    return rand_float(c, k + 100, -6, 1)


def perturb(bits, d):
    if (bits & 0x7FFFFFFF) == 0:
        return bits
    mag = (bits & 0x7FFFFFFF) + d
    mag = max(mag, 1)
    mag = min(mag, 0x7F7FFFFF)
    return (bits & 0x80000000) | mag


def delta(c, k, m):
    return rand(c, k) % (2 * m + 1) - m


def make(n):
    c = n
    fam = n % 16
    fo, to, oo = 0x00, 0x10, 0x20
    win = bytearray(fill_words(WIN_BYTES, rand(c, 25)))
    v = (n // 16) % 4
    if v == 1:
        win = bytearray(WIN_BYTES)
    elif v == 2:
        win = bytearray([0xFF] * WIN_BYTES)
    elif v == 3:
        for i in range(0, WIN_BYTES, 4):
            win[i:i + 4] = struct.pack('<I', 0x7FC00000 + i)
    a = [rand_float(c, 41 + i, -4, -1) for i in range(4)]
    b = [rand_float(c, 45 + i, -4, -1) for i in range(4)]
    t = rand_float(c, 49, -8, -1) & 0x7FFFFFFF
    ui = rand(c, 70) % 10
    sa = 0x80000000 if (rand(c, 71) & 1) else 0
    unit = [KUNIT[ui][i] ^ sa for i in range(4)]
    if fam == 0:
        pass
    elif fam == 1:
        for i in range(4):
            a[i] = unit[i]
            b[i] = perturb(unit[i], delta(c, 72 + i, 4))
    elif fam == 2:
        if rand(c, 72) & 1:
            for i in range(4):
                a[i] = unit[i]
                b[i] = unit[i]
        else:
            for i in range(4):
                a[i] = rand_float(c, 41 + i, -3, 0)
                b[i] = a[i]
    elif fam == 3:
        for i in range(4):
            a[i] = unit[i]
            b[i] = perturb(unit[i], delta(c, 72 + i, 4)) ^ 0x80000000
    elif fam == 4:
        pr = rand(c, 73) % 6
        sb = 0x80000000 if (rand(c, 74) & 1) else 0
        for i in range(4):
            a[i] = KUNIT[KORTH[pr][0]][i] ^ sa
            b[i] = KUNIT[KORTH[pr][1]][i] ^ sb
        if rand(c, 75) & 1:
            k = rand(c, 76) % 4
            a[k] = perturb(a[k], delta(c, 77, 3))
    elif fam == 5:
        for i in range(4):
            a[i] = rand_float(c, 41 + i, -2, 2)
            b[i] = rand_float(c, 45 + i, -2, 2)
    elif fam == 6:
        for i in range(4):
            if rand(c, 50 + i) % 2:
                a[i] = mix_component(c, 60 + i * 7)
            if rand(c, 54 + i) % 2:
                b[i] = mix_component(c, 110 + i * 7)
    elif fam == 7:
        for i in range(4):
            a[i] = pick(KZ, c, 41 + i)
            b[i] = pick(KZ, c, 45 + i)
    elif fam == 8:
        if rand(c, 72) & 1:
            for i in range(4):
                b[i] ^= 0x80000000
        t = pick(KT, c, 49)
    elif fam == 9:
        for i in range(4):
            a[i] = unit[i]
            b[i] = perturb(unit[i], delta(c, 72 + i, 64))
    elif fam == 10:
        fo, oo, to = 0x10, 0x10, 0x30
        if rand(c, 80) & 1:
            for i in range(4):
                a[i] = unit[i]
                b[i] = perturb(unit[i], delta(c, 72 + i, 3))
    elif fam == 11:
        to, oo, fo = 0x10, 0x10, 0x30
        if rand(c, 80) & 1:
            for i in range(4):
                a[i] = unit[i]
                b[i] = perturb(unit[i], delta(c, 72 + i, 3))
    elif fam == 12:
        fo, to = 0x10, 0x10
        oo = 0x10 if (rand(c, 80) & 1) else 0x30
        b = list(a)
    elif fam == 13:
        fo, to, oo = (rand(c, 80) % 21) * 4, (rand(c, 81) % 21) * 4, (rand(c, 82) % 21) * 4
        if rand(c, 83) % 3 == 0:
            for i in range(4):
                a[i] = unit[i]
                b[i] = perturb(unit[i], delta(c, 72 + i, 3))
    elif fam == 14:
        for i in range(4):
            a[i] = quiet(rand(c, 40 + i))
            b[i] = quiet(rand(c, 44 + i))
        t = quiet(rand(c, 48))
    else:
        for i in range(4):
            if rand(c, 50 + i) % 4:
                a[i] = pick(KNAN, c, 60 + i)
            if rand(c, 54 + i) % 4:
                b[i] = pick(KNAN, c, 70 + i)
        if rand(c, 58) & 1:
            t = pick(KNAN, c, 59)
    return {'family': fam, 'a': a, 'b': b, 't': t, 'fo': fo, 'to': to, 'oo': oo, 'win_in': bytes(win).hex(), 'matherr': 0,
            'eax': rand(c, 23), 'edx': rand(c, 24), 'stack_seed': stack_seed(n)}


def coverage():
    cov = {}
    for n in range(CASES):
        inp = make(n)
        _, st = oracle.expected(inp, n)
        for k, v in st.items():
            if k == 'family':
                cov['fam%d' % v] = cov.get('fam%d' % v, 0) + 1
            elif v:
                cov[k] = cov.get(k, 0) + 1
    return cov


if __name__ == '__main__':
    print(json.dumps(coverage(), indent=2, sort_keys=True))
