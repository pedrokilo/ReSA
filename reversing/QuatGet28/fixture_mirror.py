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
KEDGE = [0x7FFFFF, 0x000001, 0x400000, 0x7FFFFE]
KALIAS = [0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20, 0x24, 0x28, 0x2C, 0x30]
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


def make(n):
    c = n
    fam = n % 16
    alias = -1
    mat = bytearray(fill_words(64, rand(c, 25)))
    style = (n // 16) % 4
    if style == 1:
        mat = bytearray(64)
    elif style == 2:
        mat = bytearray(b'\xff' * 64)
    elif style == 3:
        for i in range(0, 64, 4):
            mat[i:i + 4] = struct.pack('<I', 0x7FC00000 + i)
    q = [0, 0, 0, 0]
    if fam == 0:
        q = [rand_float(c, 40 + i, -6, 1) for i in range(4)]
    elif fam == 1:
        q = [rand_float(c, 40 + i, -30, 20) for i in range(4)]
    elif fam == 2:
        q = [KSPECIAL[rand(c, 40 + i) % 12] for i in range(4)]
    elif fam == 3:
        q = [pick(KZEROONE, c, 40 + i) for i in range(4)]
    elif fam == 4:
        for i in range(4):
            if n & 256:
                q[i] = rand_float(c, 40 + i, -75, -62)
            elif rand(c, 50 + i) & 1:
                q[i] = rand_float(c, 40 + i, -126, -70)
            else:
                q[i] = rand(c, 40 + i) & 0x807FFFFF
    elif fam == 5:
        q = [rand_float(c, 40 + i, 70, 127) for i in range(4)]
    elif fam == 6:
        for i in range(4):
            r = rand(c, 50 + i) % 3
            q[i] = pick(KINF, c, 60 + i) if r == 0 else (pick(KZEROONE, c, 60 + i) if r == 1 else rand_float(c, 40 + i, -6, 1))
        q[rand(c, 70) & 3] = pick(KINF, c, 71)
    elif fam == 7:
        q = [rand_float(c, 40 + i, -6, 1) for i in range(4)]
        q[rand(c, 70) & 3] = pick(KNAN, c, 71)
    elif fam == 8:
        for i in range(4):
            q[i] = pick(KINF, c, 60 + i) if rand(c, 50 + i) % 5 == 0 else rand_float(c, 40 + i, -6, 1)
        a = rand(c, 70) & 3
        b = (a + 1 + rand(c, 71) % 3) & 3
        pa = 1 + rand(c, 72) % 0x3FFFFE
        pb = 1 + rand(c, 73) % 0x3FFFFE
        if pb == pa:
            pb = pa + 1
        q[a] = (0x7FC00000 | pa | ((rand(c, 74) & 1) << 31)) & M
        q[b] = (0x7FC00000 | pb | ((rand(c, 75) & 1) << 31)) & M
        if rand(c, 76) % 4 == 0:
            q[(b + 1) & 3] = (0x7FC00000 | (1 + rand(c, 77) % 0x3FFFFE) | ((rand(c, 78) & 1) << 31)) & M
    elif fam == 9:
        for i in range(4):
            r = rand(c, 50 + i) % 5
            q[i] = (pick(KNAN, c, 60 + i) if r == 0 else pick(KINF, c, 60 + i) if r == 1 else (rand(c, 61 + i) & 0x80000000) if r == 2 else pick(KZEROONE, c, 60 + i)
                    if r == 3 else rand_float(c, 40 + i, -6, 1))
    elif fam == 10:
        for i in range(4):
            b = rand(c, 40 + i)
            if (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) and not (b & 0x400000):
                b |= 0x400000
            q[i] = b
    elif fam == 11:
        for i in range(4):
            e = 127 - 10 + (rand(c, 50 + i) % 14)
            q[i] = (((rand(c, 40 + i) & 1) << 31) | (e << 23) | KEDGE[rand(c, 60 + i) % 4]) & M
    elif fam == 12:
        q = [rand_float(c, 40 + i, -6, 1) for i in range(4)]
        a = rand(c, 70) & 3
        b = (a + 1 + rand(c, 71) % 3) & 3
        pay = 0x400000 | (rand(c, 72) % 0x3FFFFF)
        q[a] = 0x7F800000 | pay
        q[b] = 0xFF800000 | pay
    elif fam == 13:
        q = [rand_float(c, 40 + i, -6, 1) for i in range(4)]
        alias = pick(KALIAS, c, 80)
    elif fam == 14:
        for i in range(4):
            r = rand(c, 50 + i) % 6
            q[i] = (pick(KNAN, c, 60 + i) if r == 0 else pick(KINF, c, 60 + i) if r == 1 else pick(KZEROONE, c, 60 + i) if r == 2
                    else KSPECIAL[rand(c, 60 + i) % len(KSPECIAL)] if r == 3 else rand_float(c, 40 + i, -6, 1))
        alias = pick(KALIAS, c, 80)
    else:
        a, b = rand_float(c, 40, -3, 1), rand_float(c, 41, -3, 1)
        s = 0x80000000 if rand(c, 42) & 1 else 0
        t = 0x80000000 if rand(c, 43) & 1 else 0
        k = rand(c, 44) % 4
        if k == 0:
            q = [a, b, b ^ s, a ^ t]
        elif k == 1:
            q = [a, a ^ s, b, b ^ t]
        elif k == 2:
            q = [a, a, a, a ^ s]
        else:
            q = [a ^ s, b, a ^ t, b]
    return {'family': fam, 'q': q, 'alias': alias, 'mat_in': bytes(mat).hex(), 'eax': rand(c, 23), 'edx': rand(c, 24), 'stack_seed': stack_seed(n)}


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
