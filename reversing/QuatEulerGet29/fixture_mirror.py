"""Espejo en Python del generador de Fixtures.hpp (MakeF) del banco QuatEulerGet29 para calibrar la cobertura de run_local.py SIN juego.

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
KEXACT = [0x00000000, 0x80000000, 0x3F800000, 0xBF800000, 0x3F000000, 0xBF000000, 0x3F3504F3, 0xBF3504F3, 0x00000000, 0x80000000, 0x3F3504F3, 0xBF3504F3]
KALIASOUT = [0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x20]
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


def make(n):
    c = n
    fam = n % 16
    qoff, xoff, yoff, zoff = 0x00, 0x10, 0x14, 0x18
    win = bytearray(fill_words(64, rand(c, 25)))
    style = (n // 16) % 4
    if style == 1:
        win = bytearray(64)
    elif style == 2:
        win = bytearray(b'\xff' * 64)
    elif style == 3:
        for i in range(0, 64, 4):
            win[i:i + 4] = struct.pack('<I', 0x7FC00000 + i)
    q = [rand_float(c, 41 + i, -3, 0) for i in range(4)]
    if fam == 0:
        pass
    elif fam == 1:
        q = [pick(KEXACT, c, 41 + i) for i in range(4)]
    elif fam == 2:
        q[0] = 0xBF3504F3 if (rand(c, 45) & 1) else 0x3F3504F3
        q[3] = 0x3F3504F3 if (rand(c, 46) & 1) else 0xBF3504F3
        if rand(c, 47) % 4 == 0:
            q[1] = pick(KZEROONE, c, 48) & 0x80000000
            q[2] = pick(KZEROONE, c, 49) & 0x80000000
        else:
            q[1] = rand_float(c, 48, -24, -4)
            q[2] = rand_float(c, 49, -24, -4)
    elif fam == 3:
        q = [(rand(c, 60 + i) & 0x807FFFFF) if (rand(c, 50 + i) % 3 == 0) else rand_float(c, 41 + i, -126, -40) for i in range(4)]
    elif fam == 4:
        q = [rand_float(c, 41 + i, 20, 127) for i in range(4)]
    elif fam == 5:
        q = [pick(KNAN, c, 60 + i) if (rand(c, 50 + i) % 3 == 0) else rand_float(c, 41 + i, -3, 0) for i in range(4)]
        q[rand(c, 45) % 4] = pick(KNAN, c, 46)
    elif fam == 6:
        q = [pick(KINF, c, 60 + i) if (rand(c, 50 + i) % 3 == 0) else rand_float(c, 41 + i, -3, 0) for i in range(4)]
        q[rand(c, 45) % 4] = pick(KINF, c, 46)
    elif fam == 7:
        q = [mix_component(c, 60 + i * 7) for i in range(4)]
    elif fam == 8:
        q = [rand_float(c, 70 + i, -30, -20) if (rand(c, 50 + i) % 3 == 0) else (pick(KZEROONE, c, 41 + i) & 0x80000000) for i in range(4)]
    elif fam == 9:
        for i in range(3):
            q[i] = rand_float(c, 41 + i, -30, -4)
        q[3] = (0x3F800000 | ((rand(c, 46) & 1) << 31)) if (rand(c, 45) & 1) else rand_float(c, 40, -1, 0)
    elif fam == 10:
        qoff = 0x10
        xoff = pick(KALIASOUT, c, 80)
        yoff = pick(KALIASOUT, c, 81)
        zoff = pick(KALIASOUT, c, 82)
    elif fam == 11:
        qoff = 0x00
        xoff = 0x20 + 4 * (rand(c, 80) % 4)
        yoff = xoff if (rand(c, 81) & 1) else 0x20 + 4 * (rand(c, 82) % 4)
        zoff = xoff if (rand(c, 83) % 3 == 0) else (yoff if (rand(c, 84) % 3 == 0) else 0x20 + 4 * (rand(c, 85) % 4))
    elif fam == 12:
        qoff = (rand(c, 80) % 13) * 4
        xoff = (rand(c, 81) % 16) * 4
        yoff = (rand(c, 82) % 16) * 4
        zoff = (rand(c, 83) % 16) * 4
        for i in range(4):
            if rand(c, 50 + i) % 8 == 0:
                q[i] = mix_component(c, 60 + i * 7)
    elif fam == 13:
        q = [quiet(rand(c, 40 + i)) for i in range(4)]
    elif fam == 14:
        q = [pick(KNAN, c, 60 + i) if (rand(c, 50 + i) % 4) else rand_float(c, 41 + i, -3, 0) for i in range(4)]
    else:
        mode = rand(c, 45) % 3
        i = rand(c, 46) % 4
        j = (i + 1 + rand(c, 47) % 3) % 4
        q = [rand(c, 50 + k) & 0x80000000 for k in range(4)]
        if mode == 0:
            q[i] = 0x3F800000 | (rand(c, 54) & 0x80000000)
        elif mode == 1:
            q[i] = 0x3F3504F3 | (rand(c, 55) & 0x80000000)
            q[j] = 0x3F3504F3 | (rand(c, 56) & 0x80000000)
        else:
            q = [0x3F000000 | (rand(c, 57 + k) & 0x80000000) for k in range(4)]
    return {'family': fam, 'q': q, 'qoff': qoff, 'xoff': xoff, 'yoff': yoff, 'zoff': zoff, 'win_in': bytes(win).hex(),
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
