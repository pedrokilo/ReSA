"""Espejo en Python del generador de Fixtures.hpp (MakeF) para calibrar la cobertura de run_local.py SIN juego.

Reproduce en aritmetica de 32 bits exactamente el generador C++ (mismos Mix/Rand/RandFloat/Pick); NO se ha comprobado contra la compilacion C++ (el banco no se compilo): si el
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
KSPECIAL = [0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x3F3504F3, 0xBF3504F3, 0x40000000, 0x3E800000, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001, 0x80000001,
            0x007FFFFF, 0x00800000, 0x3F7FFFFF, 0x3F800001, 0x34000000, 0x33800000, 0x4B800000, 0x7F000000, 0x7E800000, 0x5F800000, 0x1F800000, 0x20000000, 0x3FC00000, 0xC0400000]
KINF = [0x7F800000, 0xFF800000, 0x7F800000, 0xFF800000]
KNAN = [0x7FC00000, 0xFFC00000, 0x7FC00001, 0xFFC00001, 0x7FC12345, 0xFFC54321, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FC00002, 0x7FE00000, 0xFFE00000, 0x7FC7FFFE]
KZEROONE = [0x00000000, 0x80000000, 0x00000000, 0x80000000, 0x3F800000, 0xBF800000]
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


def fill_q(c, k, lo, hi):
    return [rand_float(c, k + i, lo, hi) for i in range(4)]


def sprinkle(q, c, k, m):
    for i in range(4):
        if rand(c, k + i) % m == 0:
            q[i] = mix_component(c, k + 10 + i * 7)


def make(n):
    c = n
    fam = n % 16
    oo, ao, bo = 0x00, 0x10, 0x20
    win = bytearray(fill_words(0x60, rand(c, 25)))
    style = (n // 16) % 4
    if style == 1:
        win = bytearray(0x60)
    elif style == 2:
        win = bytearray(b'\xff' * 0x60)
    elif style == 3:
        for i in range(0, 0x60, 4):
            win[i:i + 4] = struct.pack('<I', 0x7FC00000 + i)
    a = fill_q(c, 40, -6, 1)
    b = fill_q(c, 44, -6, 1)
    if fam == 0:
        pass
    elif fam == 1:
        a, b = fill_q(c, 40, -4, 0), fill_q(c, 44, -4, 0)
    elif fam == 2:
        a, b = fill_q(c, 40, -30, 30), fill_q(c, 44, -30, 30)
    elif fam == 3:
        a, b = fill_q(c, 40, -3, 1), fill_q(c, 44, -3, 1)
        sh = (((rand(c, 48) % 5) - 2) << 23) & M
        b[1] = (a[1] + sh) & M
        b[2] = (a[2] + sh) & M
        if rand(c, 49) % 3 == 0:
            b[2] = (b[2] + (rand(c, 50) % 3) - 1) & M
        if rand(c, 51) % 3 == 0:
            b[0] = (a[0] + sh) & M
            b[2] = (a[2] + sh) & M
        if rand(c, 52) % 3 == 0:
            b[0] = (a[0] + sh) & M
            b[1] = (a[1] + sh) & M
    elif fam == 4:
        for i in range(4):
            a[i] = rand_float(c, 41 + i, 60, 127) if (rand(c, 60 + i) & 1) else (rand_float(c, 41 + i, -126, -60) if (rand(c, 64 + i) & 1) else (rand(c, 68 + i) & 0x807FFFFF))
            b[i] = rand_float(c, 45 + i, 60, 127) if (rand(c, 72 + i) & 1) else (rand_float(c, 45 + i, -126, -60) if (rand(c, 76 + i) & 1) else (rand(c, 80 + i) & 0x807FFFFF))
    elif fam == 5:
        for i in range(4):
            a[i] = pick(KSPECIAL, c, 41 + i)
            b[i] = pick(KSPECIAL, c, 45 + i)
    elif fam == 6:
        sprinkle(a, c, 90, 2)
        sprinkle(b, c, 100, 2)
    elif fam == 7:
        for i in range(4):
            if rand(c, 90 + i) % 2:
                a[i] = pick(KZEROONE, c, 94 + i)
            if rand(c, 98 + i) % 2:
                b[i] = pick(KZEROONE, c, 102 + i)
    elif fam == 8:
        for i in range(4):
            a[i] = quiet(rand(c, 40 + i))
            b[i] = quiet(rand(c, 44 + i))
    elif fam in (9, 10, 11, 12, 13, 14):
        if fam == 9:
            oo, ao, bo = 0x00, 0x00, 0x20
        elif fam == 10:
            oo, ao, bo = 0x00, 0x20, 0x00
        elif fam == 11:
            oo, ao, bo = 0x10, 0x10, 0x10
        elif fam == 12:
            oo, ao, bo = 0x30, 0x10, 0x10
        elif fam == 13:
            ao = 0x10
            oo = (ao + ((rand(c, 80) % 7) - 3) * 4) & M
            bo = (ao + 0x10 + ((rand(c, 81) % 7) - 3) * 4) & M
            if rand(c, 82) % 2:
                ao, bo = bo, ao
        else:
            oo, ao, bo = (rand(c, 80) % 21) * 4, (rand(c, 81) % 21) * 4, (rand(c, 82) % 21) * 4
        sprinkle(a, c, 90, 8)
        sprinkle(b, c, 100, 8)
    else:
        for i in range(4):
            if rand(c, 90 + i) % 4:
                a[i] = pick(KNAN, c, 94 + i)
            if rand(c, 98 + i) % 4:
                b[i] = pick(KNAN, c, 102 + i)
    return {'family': fam, 'qa': a, 'qb': b, 'oo': oo, 'ao': ao, 'bo': bo, 'win_in': bytes(win).hex(),
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
