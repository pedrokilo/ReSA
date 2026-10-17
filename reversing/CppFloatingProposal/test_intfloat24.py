"""Host C++ versus independent Fraction oracle; not an in-game validation.

Compiles only the isolated integer-only driver into this folder's artifacts.
Never invokes CMake, project probes, GTA, runtime_runner, or registry tools.
"""
import argparse
import hashlib
import json
import os
import random
import shutil
import subprocess
from fractions import Fraction
from pathlib import Path

HERE = Path(__file__).resolve().parent


def power(exponent):
    return Fraction(2 ** exponent) if exponent >= 0 else Fraction(1, 2 ** -exponent)


def exponent_of(value):
    exponent = value.numerator.bit_length() - value.denominator.bit_length()
    return exponent - int(value < power(exponent))


def round_pc24(value, mode):
    if not value:
        return value
    negative = value < 0
    magnitude = abs(value)
    quantum = power(exponent_of(magnitude) - 23)
    scaled = magnitude / quantum
    floor, remainder = divmod(scaled.numerator, scaled.denominator)
    if mode == 0:
        increment = remainder * 2 > scaled.denominator or (
            remainder * 2 == scaled.denominator and floor % 2)
    elif mode == 1:
        increment = negative and bool(remainder)
    elif mode == 2:
        increment = not negative and bool(remainder)
    elif mode == 3:
        increment = False
    else:
        raise ValueError(mode)
    result = (floor + int(increment)) * quantum
    return -result if negative else result


def encode80(value, negative=False, exceptional=None):
    if exceptional is not None:
        significand, exponent = exceptional, 0x7FFF
    elif not value:
        significand, exponent = 0, 0
    else:
        magnitude = abs(value)
        unbiased = exponent_of(magnitude)
        scaled = magnitude / power(unbiased - 63)
        assert scaled.denominator == 1
        significand, exponent = int(scaled), unbiased + 16383
    return (significand.to_bytes(8, 'little') +
            (exponent | (0x8000 if negative else 0)).to_bytes(2, 'little')).hex()


def oracle(unsigned_input, integer_bits, float_bits, mode):
    integer = integer_bits if unsigned_input or integer_bits < 0x80000000 else integer_bits - 0x100000000
    fraction = float_bits & 0x7FFFFF
    exponent = (float_bits >> 23) & 255
    float_negative = bool(float_bits >> 31)
    negative = (integer < 0) != float_negative
    sign = 0x80000000 if negative else 0
    if exponent == 255:
        if fraction:
            payload = fraction | 0x400000
            bits = (float_bits & 0x80000000) | 0x7F800000 | payload
            flags = 0 if fraction & 0x400000 else 1
            ext = encode80(0, float_negative, 0x8000000000000000 | (payload << 40))
        elif not integer:
            bits, flags = 0xFFC00000, 1
            ext = encode80(0, True, 0xC000000000000000)
        else:
            bits, flags = sign | 0x7F800000, 0
            ext = encode80(0, negative, 0x8000000000000000)
        return bits, flags, 0, 0, 0, ext
    flags = 2 if not exponent and fraction else 0
    if not integer or not exponent and not fraction:
        return sign, flags, 0, 0, 0, encode80(0, negative)
    decoded = Fraction(fraction | 0x800000) * power(exponent - 150) if exponent else Fraction(fraction) * power(-149)
    if float_negative:
        decoded = -decoded
    exact = integer * decoded
    product = round_pc24(exact, mode)
    inexact = product != exact
    magnitude_up = abs(product) > abs(exact)
    flags |= 32 if inexact else 0
    ext = encode80(product, negative)
    exponent = exponent_of(abs(product))
    if exponent > 127:
        infinite = mode == 0 or mode == 1 and negative or mode == 2 and not negative
        bits = sign | (0x7F800000 if infinite else 0x7F7FFFFF)
        return bits, flags | 8 | 32, int(inexact), int(magnitude_up), 1, ext
    if exponent >= -126:
        mantissa = abs(product) / power(exponent - 23)
        assert mantissa.denominator == 1
        bits = sign | ((exponent + 127) << 23) | (int(mantissa) & 0x7FFFFF)
    else:
        mantissa = abs(product) / power(-149)
        assert mantissa.denominator == 1
        bits = sign | int(mantissa)
    return bits, flags, int(inexact), int(magnitude_up), 0, ext


def cases(random_count):
    integers = [0, 1, 2, 3, 5, 7, 0x7FFFFF, 0x800000, 0xFFFFFF, 0x1000000,
                0x1000001, 0x1FFFFFF, 0x2000001, 0x4000001, 0x7FFFFFFF,
                0x80000000, 0x80000001, 0xFFFFFFFD, 0xFFFFFFFE, 0xFFFFFFFF]
    floats = [0, 1, 2, 3, 0x3FFFFF, 0x400000, 0x7FFFFE, 0x7FFFFF,
              0x800000, 0x800001, 0x3E800000, 0x3F000000, 0x3F7FFFFF,
              0x3F800000, 0x3F800001, 0x3FA00000, 0x3FC00000, 0x40000000,
              0x4AFFFFFF, 0x4B000000, 0x7F000000, 0x7F7FFFFE, 0x7F7FFFFF,
              0x7F800000, 0x7F800001, 0x7FBFFFFF, 0x7FC00000, 0x7FFFFFFF]
    for unsigned in range(2):
        for integer in integers:
            for value in floats:
                for sign in (0, 0x80000000):
                    for mode in range(4):
                        yield unsigned, integer, value | sign, mode
    # Structural tie cases: (2^24+1)*1 is halfway. Odd lower mantissa case
    # (2^24+3)*1 tests ties-to-even increasing instead of decreasing.
    for integer in (0x1000001, 0x1000003, 0xFFFFFFFF - 0x1000000, 0xFFFFFFFF - 0x1000002):
        for mode in range(4):
            yield 0, integer, 0x3F800000, mode
    rng = random.Random(0x24F17D)
    for _ in range(random_count):
        unsigned, integer, value = rng.randrange(2), rng.getrandbits(32), rng.getrandbits(32)
        for mode in range(4):
            yield unsigned, integer, value, mode


def compile_driver(artifacts):
    source = HERE / 'intfloat24_driver.cpp'
    executable = artifacts / 'intfloat24_driver.exe'
    cl = shutil.which('cl')
    arguments = ['/nologo', '/std:c++20', '/O2', '/W4', '/WX', '/EHsc',
                 '/Fe:' + str(executable), '/Fo:' + str(artifacts / 'intfloat24_driver.obj'), str(source)]
    if cl:
        command = [cl, *arguments]
    else:
        vswhere = Path(os.environ.get('ProgramFiles(x86)', 'C:/Program Files (x86)')) / 'Microsoft Visual Studio/Installer/vswhere.exe'
        installed = subprocess.run([str(vswhere), '-latest', '-property', 'installationPath'],
                                   check=True, capture_output=True, text=True).stdout.strip()
        vcvars = Path(installed) / 'VC/Auxiliary/Build/vcvars64.bat'
        if not vcvars.is_file():
            raise RuntimeError('No host MSVC environment; use the standalone driver with a C++20 compiler.')
        # Fixed executable and validated local paths; output stays inside this
        # proposal's artifacts. No project/build scripts are invoked.
        script = artifacts / 'compile-host.cmd'
        script.write_bytes(('@echo off\r\ncall "' + str(vcvars) + '" >nul\r\n'
                            'if errorlevel 1 exit /b 1\r\ncl ' + subprocess.list2cmdline(arguments) +
                            '\r\nexit /b %errorlevel%\r\n').encode('utf-8'))
        command = ['cmd.exe', '/d', '/c', str(script)]
    with (artifacts / 'host-compile.log').open('wb') as log:
        subprocess.run(command, cwd=artifacts, stdout=log, stderr=subprocess.STDOUT, check=True)
    return executable


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--random-count', type=int, default=30000)
    parser.add_argument('--driver', type=Path)
    args = parser.parse_args()
    fixtures = list(cases(args.random_count))
    artifacts = HERE / 'artifacts'
    artifacts.mkdir(exist_ok=True)
    driver = args.driver or compile_driver(artifacts)
    data = ''.join(' '.join(map(str, row)) + '\n' for row in fixtures)
    process = subprocess.run([str(driver)], input=data, text=True, capture_output=True, check=True)
    lines = process.stdout.splitlines()
    if len(lines) != len(fixtures):
        raise RuntimeError('Missing/extra output: ' + str((len(lines), len(fixtures))))
    coverage = {name: 0 for name in ['zero', 'finite', 'infinity', 'nan', 'signaling_nan',
                                   'denormal_input', 'overflow_store', 'inexact_mul', 'exact_mul',
                                   'rounded_magnitude_up', 'subnormal_output', 'uint32_high', 'signed_min']}
    for index, (fixture, line) in enumerate(zip(fixtures, lines)):
        parts = line.split()
        observed = tuple(int(x) for x in parts[:5]) + (parts[5],)
        expected = oracle(*fixture)
        if observed != expected:
            raise RuntimeError('Mismatch at ' + str(index) + ': ' + str((fixture, observed, expected)))
        unsigned, integer, value, _ = fixture
        exponent, fraction = (value >> 23) & 255, value & 0x7FFFFF
        kind = 'nan' if exponent == 255 and fraction else 'infinity' if exponent == 255 else 'zero' if exponent == 0 and fraction == 0 else 'finite'
        coverage[kind] += 1
        coverage['signaling_nan'] += int(kind == 'nan' and not fraction & 0x400000)
        coverage['denormal_input'] += int(exponent == 0 and fraction != 0)
        coverage['overflow_store'] += int(bool(expected[1] & 8))
        coverage['inexact_mul'] += expected[2]
        coverage['exact_mul'] += int(not expected[2])
        coverage['rounded_magnitude_up'] += expected[3]
        coverage['subnormal_output'] += int(expected[0] & 0x7F800000 == 0 and expected[0] & 0x7FFFFF != 0)
        coverage['uint32_high'] += int(unsigned and integer >= 0x80000000)
        coverage['signed_min'] += int(not unsigned and integer == 0x80000000)
    if not all(coverage.values()):
        raise RuntimeError('Uncovered necessary branch: ' + str(coverage))
    report = {'scope': 'offline integer-only C++ host driver versus independent Fraction oracle',
              'in_game_validated': False, 'pairs': len(fixtures), 'differences': 0,
              'rounding_modes': ['nearest_even', 'down', 'up', 'toward_zero'],
              'coverage': coverage,
              'sha256': {name: hashlib.sha256((HERE / name).read_bytes()).hexdigest()
                         for name in ['IntFloat24.hpp', 'intfloat24_driver.cpp', 'test_intfloat24.py']},
              'limitations': ['No GTA execution or original x87 instruction comparisons.',
                              'Software exception flags only; no physical FPU status/control manipulation.',
                              'PC=24 only; masked-exception result semantics; input binary32 plus int32/uint32.',
                              'uint32 models exact nonnegative conversion, not signed FILD dword.']}
    (HERE / 'intfloat24-host-results.json').write_bytes((json.dumps(report, indent=2) + '\n').encode())
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
