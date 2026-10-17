"""Oraculo independiente (parcial) del banco Dependencies22A.

Repite en Python los parametros escalares de cada caso (Fixtures.hpp) y el comportamiento de ProcessVerticalLineUsingCache que no depende del x87: acierto de cache, llamadas a
CWorld::ProcessVerticalLine, indice de escritura y bits de *outZ (copia cruda en acierto; FLD/FSTP, es decir NaN silenciado, en fallo). El resto (aritmetica x87) se decide por igualdad EXE/DLL.
"""
import re
from pathlib import Path
M32 = 0xFFFFFFFF
TEXT = (Path(__file__).resolve().parent / 'Fixtures.hpp').read_text(encoding='utf-8')
STRESS = [int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]+', re.search(r'kStress\{(.*?)\};', TEXT, re.S).group(1))]
MASKS = [int(x, 16) for x in re.findall(r'0x[0-9A-Fa-f]+|\b0\b', re.search(r'kStressMasks\{(.*?)\};', TEXT, re.S).group(1))] if False else [0, 0, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x09, 0x06, 0x3F, 0x3F, 0x0C, 0x30, 0x03, 0x15]
COUNTS = [1, 1, 2, 3, 4, 5, 8, 16, 31, 32, 0, 0xFFFFFFFF]
GEN, MULT, CACHE, FOG = 2048, 1536, 2048, 2048
assert len(STRESS) == 32


def mix(x):
    x &= M32; x ^= x >> 16; x = (x * 0x7FEB352D) & M32; x ^= x >> 15; x = (x * 0x846CA68B) & M32; x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


def float_bits(sixteenths):
    import struct
    return struct.unpack('<I', struct.pack('<f', sixteenths * 0.0625))[0]


def value(c, k, rng, per_mille):
    if rand(c, k) % 1000 < per_mille:
        return STRESS[rand(c, k + 7000) % 32]
    return float_bits(rand(c, k + 3000) % (2 * rng + 1) - rng)


def per_mille(c, bit):
    return 120 if (MASKS[c % 16] >> bit) & 1 else 0


def light_count(c):
    return COUNTS[rand(c, 1) % 12]


def is_nan(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def quiet(b):
    return (b | 0x400000) if is_nan(b) else b


def equal_float(a, b):
    import struct
    fa, fb = struct.unpack('<f', struct.pack('<I', a))[0], struct.unpack('<f', struct.pack('<I', b))[0]
    return fa == fb


def cache_fixture(c):
    point = [value(c, 10 + j, 1600, 60 + per_mille(c, 4)) for j in range(3)]
    hit = rand(c, 6) % 32 if rand(c, 5) % 3 == 0 else -1
    entry = [[(point[j] if rand(c, 100 + i * 4 + j) % 40 == 0 else value(c, 100 + i * 4 + j, 1600, 60)) for j in range(3)] for i in range(32)]
    result = [value(c, 300 + i, 800, 60) for i in range(32)]
    if hit >= 0:
        for j in range(3):
            entry[hit][j] = 0x80000000 if (rand(c, 7) % 8 == 0 and j == 0 and point[0] == 0) else point[j]
    return dict(point=point, hit=hit, entry=entry, result=result, next=rand(c, 8) % 32, pvl_ok=rand(c, 9) % 4 != 0, pvl_z=value(c, 11, 800, 120))


def cache_oracle(c):
    """(indice de acierto, llamadas a ProcessVerticalLine, bits finales de *outZ, indice de escritura final, retorno)."""
    f = cache_fixture(c)
    for i in range(32):
        if all(equal_float(f['entry'][i][j], f['point'][j]) for j in range(3)):
            return i, 0, f['result'][i], f['next'], True
    if not f['pvl_ok']:
        return -1, 1, 0x40E00000, f['next'], False   # 7.0f: outZ intacto
    return -1, 1, quiet(f['pvl_z']), (f['next'] + 1) % 32, True
