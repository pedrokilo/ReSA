"""Espejo en Python del generador de Fixtures.hpp (MakeF) para calibrar la cobertura de run_local.py SIN juego.

Reproduce en aritmetica de 32 bits el generador C++ (mismos Mix/Rand/RandFloat/Pick/MixValue); NO se ha comprobado contra la compilacion C++ (el banco no se compilo): si
`comparisons.jsonl` trae filas `input` distintas de las de este espejo, corregir uno de los dos. `python -I fixture_mirror.py` imprime la cobertura de las 4096 fixtures segun el oraculo.
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
KSPECIAL = [0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x40000000, 0x7F7FFFFF, 0xFF7FFFFF, 0x00000001, 0x80000001, 0x007FFFFF, 0x807FFFFF, 0x00800000,
            0x80800000, 0x7F800000, 0xFF800000, 0x4B800000, 0x34000000, 0x7F000000, 0x3F7FFFFF, 0x3F800001, 0xBF800001, 0x5F800000]
KQNAN = [0x7FC00000, 0xFFC00000, 0x7FC00001, 0xFFC00001, 0x7FC12345, 0xFFC54321, 0x7FFFFFFF, 0xFFFFFFFF, 0x7FC00002, 0x7FE00000, 0xFFE00000, 0x7FC7FFFE]
KSNAN = [0x7F800001, 0xFF800001, 0x7FA00000, 0xFFA00000, 0x7FBFFFFF, 0xFFBFFFFF, 0x7F812345, 0xFFA54321, 0x7F800002, 0xFF800400, 0x7FB00000, 0xFF9FFFFE]
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


def mix_value(c, k):
    t = rand(c, k) % 8
    if t == 0:
        return pick(KQNAN, c, k + 100)
    if t == 1:
        return pick(KSNAN, c, k + 100)
    if t == 2:
        return pick(KSPECIAL, c, k + 100)
    if t == 3:
        return rand(c, k + 100) & 0x807FFFFF
    if t == 4:
        return (rand(c, k + 100) & 0x80000000) | 0x7F800000
    if t == 5:
        return rand_float(c, k + 100, 100, 127)
    return rand_float(c, k + 100, -126, 100)


def make_f(n):
    c = n
    family = n % 8
    qoff = (rand(c, 7) % 13) * 4
    win = bytearray(fill_words(0x40, rand(c, 25)))
    sel = (n // 8) % 4
    if sel == 1:
        win = bytearray(0x40)
    elif sel == 2:
        win = bytearray(b'\xFF' * 0x40)
    elif sel == 3:
        for i in range(0, 0x40, 4):
            struct.pack_into('<I', win, i, 0x7FC00000 + i)
    q = [rand_float(c, 41 + i, -20, 20) for i in range(4)]
    if family == 0:
        pass
    elif family == 1:
        q = [pick(KSPECIAL, c, 41 + i) for i in range(4)]
    elif family == 2:
        q = [(rand(c, 50 + i) & 0x80000000) | (0x7F800000 if rand(c, 54 + i) % 3 == 0 else rand(c, 58 + i) & 0x7FFFFF) for i in range(4)]
    elif family == 3:
        q = [pick(KQNAN, c, 41 + i) for i in range(4)]
    elif family == 4:
        q = [pick(KSNAN, c, 41 + i) for i in range(4)]
    elif family == 5:
        q = [mix_value(c, 60 + i * 7) for i in range(4)]
    elif family == 6:
        q = [rand(c, 70 + i) for i in range(4)]
        qoff = rand(c, 7) % 49
    else:
        q = [pick(KSNAN, c, 41 + i) if rand(c, 50 + i) % 2 else mix_value(c, 60 + i * 7) for i in range(3)] + [mix_value(c, 90)]
    return dict(family=family, q=q, qoff=qoff, win_in=bytes(win).hex(), eax=rand(c, 23), edx=rand(c, 24), stack_seed=stack_seed(n))


def coverage():
    cov = {}
    for n in range(CASES):
        f = make_f(n)
        assert 0 <= f['qoff'] <= 48
        want, stats = oracle.expected(f, n)
        for k, v in stats.items():
            if k == 'family':
                cov['fam%d' % v] = cov.get('fam%d' % v, 0) + 1
            elif v:
                cov[k] = cov.get(k, 0) + 1
    return cov


if __name__ == '__main__':
    print(json.dumps(coverage(), indent=2, sort_keys=True))
