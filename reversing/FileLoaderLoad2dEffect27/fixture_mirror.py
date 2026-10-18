"""Espejo en Python de Fixtures.hpp (NO es la fuente de verdad: el banco usa el generador C++).

Sirve para tres cosas sin compilar nada ni ejecutar el juego:
  1. comprobar que TODAS las lineas que el generador produce son validas para el oraculo (campos obligatorios leidos, buffers del EXE respetados, comillas de la luz resueltas);
  2. medir la cobertura que reparten las familias y fijar los minimos de run_local.py con margen;
  3. detectar a ojo errores de diseno de las familias.
El orden exacto de consumo del RNG puede diferir del C++ (MSVC evalua los argumentos de derecha a izquierda), asi que los casos concretos no coinciden numero a numero; la mezcla de
familias, los grupos y las clases de token son los mismos.

Uso:  python -I fixture_mirror.py            (resumen de cobertura; sale con error si algun caso es invalido)
      python -I fixture_mirror.py --dump N   (imprime el caso N)
"""
import json
import sys
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
import fixture_oracle as oracle

M32 = 0xFFFFFFFF
GROUPS, PER_GROUP = 12, 512
CASES = GROUPS * PER_GROUP
K_ENTRY = 0x40
MAX_STORE_K = 12
MODEL_TABLE = 20000


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 12345) & M32)


class Gen:
    def __init__(self, seed):
        self.c, self.k = seed & M32, 0

    def __call__(self, m):
        if m == 0:
            self.k += 1
            return 0
        v = rand(self.c, self.k)
        self.k += 1
        return v % m

    def raw(self):
        v = rand(self.c, self.k)
        self.k += 1
        return v


INT_EDGE = ["0", "1", "2", "3", "7", "8", "15", "16", "17", "31", "32", "63", "64", "100", "127", "128", "129", "200", "254", "255", "256", "257", "258", "511", "512", "513", "1000", "4095", "4096",
            "32767", "32768", "32769", "65535", "65536", "65537", "131071", "131072", "16777215", "16777216", "2147483646", "2147483647",
            "-1", "-2", "-3", "-8", "-127", "-128", "-129", "-255", "-256", "-257", "-32767", "-32768", "-32769", "-65535", "-65536", "-65537", "-2147483647", "-2147483648",
            "+0", "+1", "+5", "+255", "+256", "007", "0255", "00000255", "-0", "-007", "0000000000256"]
INT_OVERFLOW = ["2147483648", "2147483649", "2147483650", "3000000000", "4294967295", "4294967296", "4294967297", "4294967551", "4294967552", "8589934591", "10000000000", "99999999999",
                "-2147483649", "-2147483650", "-3000000000", "-4294967295", "-4294967296", "-4294967297", "-99999999999", "18446744073709551616", "184467440737095516150"]
TYPE_OVERFLOW = ["2147483648", "2147483649", "3000000000", "4294967295", "10000000000", "99999999999", "-2147483649", "-3000000000"]
EXOTIC_INT = ["3.7", "0x1F", "1e3", "+", "-", "abc", "--5", "+-5", "5abc", "0x", "08", "9.", "1,2", ".5", "e5", "#7", "%d", "-+3", "3-", "0.0"]
FLOAT_PLAIN = ["0", "1", "-1", "2", "0.5", "-0.5", ".5", "-.5", "5.", "-5.", "+2.5", "+.25", "-0", "0.0", "-0.0", "123.456", "-123.456", "1e3", "2.5E-3", "1.5e+2", "12345.678", "0.1", "0.2", "0.3", "1e-5",
               "100000", "-123456.789", "3.14159", "1000000", "16777216", "16777217", "8388607.5", "0.0001", "255.5", "255.99", "256", "65536.25", "3.4e10", "1e-10", "-2.5e-7", "7.0", "12.5", "-99.9",
               "1e30", "-1e30", "1e-30", "123456789", "-123456789", "0.000001", "4096", "-4096.5", "0.75", "-0.125", "1.0", "1e0", "1E1", "00012.5", "0.5e1", "9e-9"]
HALFWAY = ["1.000000059604644775390625", "1.00000005960464477539062500000000000000000000000001", "1.00000005960464477539062499999999999999999999999999",
           "3.00000011920928955078125", "3.0000001192092895507812500000000000000000000000001", "3.0000001192092895507812499999999999999999999999999",
           "0.1000000052154064178466796875", "0.100000005215406417846679687500000000000000000000000001", "0.100000005215406417846679687499999999999999999999999999",
           "100.000003814697265625", "100.00000381469726562500000000000000000000000001", "100.00000381469726562499999999999999999999999999",
           "255.50000762939453125", "255.5000076293945312500000000000000000000000001", "255.5000076293945312499999999999999999999999999",
           "12345.60009765625", "12345.6000976562500000000000000000000000001", "12345.6000976562499999999999999999999999999",
           "16777217", "16777217.00000000000000000000000001", "16777216.99999999999999999999999999",
           "10000000512", "10000000512.00000000000000000000000001", "10000000511.99999999999999999999999999",
           "0.30000002682209014892578125", "0.3000000268220901489257812500000000000000000000000001", "0.3000000268220901489257812499999999999999999999999999",
           "65536.00390625", "65536.0039062500000000000000000000000001", "65536.0039062499999999999999999999999999"]
SIZE_EDGE = ["0", "0.5", "0.99999", "0.99999994", "0.999999999", "1", "1.5", "2", "127.5", "128", "254.9", "255", "255.5", "255.99999", "255.999993", "255.9999999999", "256", "256.5", "257", "300", "511.9", "512",
             "1000", "65535", "65536", "65537.5", "1e5", "16777216", "16777217", "2147483648", "4294967296", "4294967297", "4294967808", "1e10", "3e9", "1e15", "9.2e18", "9.3e18", "1e19", "1e20", "1e30", "2.55e2",
             "-0.5", "-0.99999", "-1", "-1.5", "-127.5", "-128", "-255", "-255.5", "-256", "-257", "-1e10", "-3e9", "-1e19", "-1e30", "-0", "+255.9"]
EXOTIC_FLOAT = ["inf", "-inf", "+inf", "Inf", "INF", "infinity", "Infinity", "nan", "-nan", "NaN", "NAN", "nan(123)", "1e39", "-1e39", "1e400", "-1e400", "1e-50", "1e-400", "0x1p3", "0x10", "0X1.8p1", "0x.8",
                "1#INF", "1.#QNAN", "-1.#IND", "1e", "1e+", "1e-", ".", "-.", "+.e5", "1,5", "1d5", "1f", "--1", "1.5.5", "i", "n", "e"]
ID_TABLE = [0, 1, 2, 7, 100, 1000, 4095, 10000, 18000, 19998, 19999]
TAIL1 = '"tail_sh" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19'
TAIL2 = '"tail_cor" "tail_sh" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19'
OOR_TYPES = ["-1", "11", "12", "13", "15", "16", "31", "32", "63", "64", "127", "128", "129", "254", "255", "256", "257", "258", "383", "384", "511", "512", "513", "1000", "1023", "1024",
             "4095", "4096", "65535", "65536", "65537", "65538", "16777216", "16777217", "2147483647", "-2147483648", "-2", "-100", "-255", "-256", "-257", "-65536", "-65535", "+11"]
STACK_SEEDS = [0, 0xFFFFFFFF, 0x14, 0x10, 1, 0x80000000, 0x9E3779B1, 0x55555555]

K_CRT_HALFWAY, K_CRT_INT_OVERFLOW, K_CRT_EXOTIC_FLOAT, K_CRT_EXOTIC_INT, K_CRT_TYPE_OVERFLOW, K_CRT_MASKED, K_CRT_FAIL_TAIL = 1, 2, 4, 8, 16, 32, 64


class Fx:
    pass


def pick(g, a):
    return a[g(len(a))]


def int_tok(g, mode):
    if mode == 0:
        return str(g(256))
    if mode == 1:
        return pick(g, INT_EDGE)
    if mode == 2:
        return str(g.raw() % 2000001 - 1000000)
    if mode == 3:
        return str(g(256) if g(2) else g(70000))
    return pick(g, INT_EDGE) if g(3) == 0 else str(g(256))


def decimal_tok(g):
    s = '-' if g(3) == 0 else ''
    s += str(g(10000 if g(2) else 100))
    if g(4) != 0:
        s += '.' + str(g(1000))
    return s


def float_tok(g, mode):
    if mode == 0:
        return pick(g, FLOAT_PLAIN)
    if mode == 1:
        return decimal_tok(g)
    if mode == 2:
        return str(g.raw() % 20001 - 10000)
    return pick(g, FLOAT_PLAIN) if g(2) else decimal_tok(g)


ALNUM = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_"
PUNCT = "!#$%&'()*+,-./:;<=>?@[\\]^`{|}~"
FMTCH = "%sdf%d.+-"


def name(g, lo, hi, style=0):
    n = lo + g(hi - lo + 1)
    s = ''
    for _ in range(n):
        if style == 1 and g(3) == 0:
            s += PUNCT[g(len(PUNCT))]
        elif style == 2:
            s += chr(ord('0') + g(10))
        elif style == 3:
            s += FMTCH[g(len(FMTCH))]
        else:
            s += ALNUM[g(len(ALNUM))]
    if not s and lo > 0:
        s = 'a'
    return s


def qname(g, lo, hi, style=0):
    s = name(g, lo, hi, 1 if style == 1 else 0)
    if style == 1:
        s = ''.join(' ' if g(5) == 0 else ch for ch in s)
    return s


def sep(g, rnd):
    if not rnd:
        return ' '
    return ['  ', '	', ' 	 ', '   ', '		', ' ', ' ', ' '][g(8)]


def join(g, t, rnd):
    s = ''
    for i, x in enumerate(t):
        if i:
            s += sep(g, rnd)
        s += x
    if rnd and g(4) == 0:
        s += sep(g, True)
    return s


def pad(g, count):
    return ''.join(' ' if g(3) == 0 else name(g, 1, 6) for _ in range(count))


def fill_pre(f, g):
    m = g(8)
    if m == 0:
        f.pre_entry = bytes(64)
    elif m == 1:
        f.pre_entry = b'\xff' * 64
    elif m == 2:
        f.pre_entry = bytes((i * 7 + f.n * 13 + 1) & 0xFF for i in range(64))
    elif m == 3:
        f.pre_entry = bytes([(0x11 + (f.n & 0xEE)) & 0xFF]) * 64
    else:
        f.pre_entry = b''.join(rand(f.n * 977 + 31, i).to_bytes(4, 'little') for i in range(16))
    f.pre_over = b''.join(rand(f.n * 1237 + 91, i).to_bytes(4, 'little') for i in range(32))


def prefix(g, f, t, type_tok, coord_mode):
    id_ = str(f.model_id)
    if f.family % 4 == 3:
        v = g(6)
        if v == 0:
            id_ = '+' + id_
        elif v == 1:
            id_ = '0' * (g(4) + 1) + id_
    t.append(id_)
    for _ in range(3):
        if coord_mode == 99:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        else:
            t.append(float_tok(g, coord_mode))
    t.append(type_tok)


def type_tok(g, f, typ):
    s = str(typ)
    if f.family % 5 == 4:
        v = g(4)
        if v == 0:
            s = '+' + s
        elif v == 1:
            s = '0' + s
        elif v == 2:
            s = '00' + s
    return s


def build_light(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 0), 99 if fam == 11 else (3 if fam == 9 else 1))

    def byte_tok():
        if fam == 8:
            return pick(g, INT_EDGE)
        if fam == 12:
            return pick(g, INT_OVERFLOW) if g(2) else int_tok(g, 1)
        return int_tok(g, 1 if g(5) == 0 else 0)
    if fam == 12:
        f.crt |= K_CRT_INT_OVERFLOW
    for _ in range(4):
        t.append(byte_tok())
    corona, shadow = qname(g, 1, 20), qname(g, 1, 20)
    pair_a = pair_b = True
    f.tail = TAIL1
    if fam == 1:
        corona = shadow = ''
    elif fam == 2:
        corona, shadow = '', qname(g, 60, 100)
    elif fam == 3:
        corona = qname(g, 60, 90)
    elif fam == 4:
        pair_b = False
    elif fam == 5:
        pair_a = pair_b = False
        f.tail = TAIL2
        f.crt |= K_CRT_FAIL_TAIL
    elif fam == 14:
        corona, shadow = qname(g, 1, 40, 1), qname(g, 1, 40, 1)
    elif fam == 15:
        if g(2):
            corona = ''
        if g(2):
            shadow = qname(g, 20, 100, g(2))
    if not pair_b and pair_a:
        f.crt |= K_CRT_FAIL_TAIL
    nums = [pick(g, HALFWAY) if fam == 11 else float_tok(g, 3 if fam == 9 else 0) for _ in range(4)]
    if fam == 11:
        f.crt |= K_CRT_HALFWAY
    for i in range(9):
        if i == 4:
            if fam == 7:
                v = pick(g, ["0", "1", "2", "3", "4", "5", "6", "7", "255", "256", "260", "261", "1028", "65535", "65536", "65540", "-1", "-4", "-5", "-253", "4294967295", "32772", "32768"])
            elif fam == 12:
                v = pick(g, INT_OVERFLOW) if g(2) else int_tok(g, 1)
            else:
                v = str(4 + 8 * g(30)) if g(3) == 0 else int_tok(g, 0)
        elif fam in (10, 8):
            v = pick(g, INT_EDGE)
        elif fam == 12:
            v = int_tok(g, 0) if g(3) else pick(g, INT_OVERFLOW)
        else:
            v = int_tok(g, 1 if g(6) == 0 else 0)
        nums.append(v)
    rs = fam == 6 or (fam == 15 and g(2))
    line = join(g, t, rs) + sep(g, rs)
    if pair_a:
        line += '"' + corona + '"'
    if fam == 13 and pair_a and pair_b:
        pass
    elif pair_a and pair_b:
        line += sep(g, rs)
    if pair_b:
        line += '"' + shadow + '"'
    line += '' if (fam == 13 and g(2)) else sep(g, rs)
    line += join(g, nums, rs)
    f.line = line


def build_particle(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 1), 3 if fam == 9 else (g(4) if fam == 15 else 0))
    if fam == 0:
        nm = name(g, 1, 7)
    elif fam == 1:
        nm = name(g, 7, 8)
    elif fam == 2:
        nm = name(g, 9, 20)
    elif fam == 3:
        nm = name(g, 21, 46)
    elif fam == 4:
        nm = name(g, 47, 47)
    elif fam == 5:
        nm = name(g, 48, 62)
    elif fam == 6:
        nm = name(g, 63, 100)
    elif fam == 7:
        nm = ''
    elif fam == 10:
        nm = name(g, 1, 40, 1)
    elif fam == 11:
        nm = ["1.5", "0x10", "-7", "1e3", "+5", "007", "3.14", "nan", "inf", "0"][g(10)]
    elif fam == 12:
        nm = name(g, 2, 20, 3)
    elif fam == 13:
        nm = name(g, 1, 1)
    elif fam == 15:
        nm = name(g, 1, 30 if g(2) else 100, 1 if g(3) == 0 else 0) if g(6) else ''
    else:
        nm = name(g, 1, 24)
    if nm:
        t.append(nm)
    if fam == 8:
        t += [name(g, 1, 8), str(g(1000)), '"q"']
    f.line = join(g, t, fam == 9 or (fam == 15 and g(2)))
    if fam == 14:
        f.line = "\t \t" + f.line


def extra_tail(g, style):
    if style == 0:
        return ''
    if style == 1:
        return ' ' + pad(g, 3)
    if style == 2:
        return ' ' + pad(g, 30)
    if style == 3:
        return "\t \t" + pad(g, 4)
    if style == 4:
        return ' ' + pad(g, 60) + pad(g, 60)
    if style == 5:
        return ' "quoted tail" 1 2 3'
    if style == 6:
        return ' %s %d %f'
    if style == 7:
        return ' ' + 'x' * (g(40) + 1)
    return ' ' + str(g(100)) + ' ' + float_tok(g, 0) + ' ' + name(g, 1, 5)


def build_plain(g, f, typ, override):
    fam = f.family
    t = []
    coord = fam % 6
    if fam == 3:
        coord = 99
    prefix(g, f, t, override or type_tok(g, f, typ), 3 if coord == 4 else (2 if coord == 5 else (99 if coord == 99 else coord)))
    if fam == 5:
        for i in range(1, 4):
            t[i] = "1e30" if g(2) else ("-1e30" if g(2) else "1e-30")
    if fam == 4:
        for i in range(1, 4):
            t[i] = pick(g, FLOAT_PLAIN) if g(2) else ("1.5e+2" if g(2) else "2.5E-3")
    f.line = join(g, t, fam == 7 or fam == 15)
    f.line += extra_tail(g, 8 if fam == 11 else (4 if fam == 12 else (0 if fam % 4 == 0 else fam % 9)))
    if fam == 13:
        f.line = "\t" + f.line


def build_attractor(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 3), 99 if fam == 6 else 0)
    if fam == 1:
        t.append(pick(g, INT_EDGE))
    elif fam == 7:
        t.append(pick(g, INT_OVERFLOW))
    elif fam == 12:
        t.append(('+' + str(g(256))) if g(2) else ('00' + str(g(256))))
    else:
        t.append(int_tok(g, 2 if fam == 8 else 0))
    for _ in range(9):
        if fam == 6:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        elif fam == 5:
            t.append(pick(g, FLOAT_PLAIN))
        elif fam == 11:
            t.append(["1e30", "-1e30", "1e-30", "123456789", "-123456789"][g(5)])
        else:
            t.append(float_tok(g, 3 if fam == 15 else 1))
    if fam == 7:
        f.crt |= K_CRT_INT_OVERFLOW
    for _ in range(2):
        if fam == 1:
            t.append(pick(g, INT_EDGE))
        elif fam == 7:
            t.append(pick(g, INT_OVERFLOW))
        elif fam == 8:
            t.append(str(-g(300)))
        else:
            t.append(int_tok(g, 4 if fam == 14 else 0))
    if fam == 2:
        nm = name(g, 1, 7)
    elif fam == 3:
        nm = name(g, 8, 30)
    elif fam == 4:
        nm = name(g, 31, 100)
    elif fam == 10:
        nm = name(g, 1, 40, 1)
    elif fam == 15:
        nm = name(g, 1, 12 if g(2) else 100)
    else:
        nm = name(g, 1, 14)
    t.append(nm)
    if fam == 13:
        t += [str(g(100)), name(g, 1, 5)]
    f.line = join(g, t, fam == 9 or fam == 15)


NEAR_TOK = ["255.99999", "255.999993", "255.9999999999", "0.99999999999999999999", "0.9999999999", "1.0000000001", "16777216.5", "16777217.00000000000000000001", "65535.99999999", "65535.9999999999999",
            "4294967295.9999999999", "4294967296.0000000001", "-0.9999999999", "-255.9999999999", "-256.0000000001", "127.99999999999", "128.00000000001", "254.99999999999"]


def build_roadsign(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 5), 3 if fam == 14 else 0)
    ct = [pick(g, INT_EDGE) if fam == 5 else int_tok(g, 0)]
    for _ in range(3):
        if fam in (3, 7, 15):
            ct.append(pick(g, HALFWAY) if fam == 7 else pick(g, SIZE_EDGE))
        elif fam == 14:
            ct.append("2.55e2" if g(2) else float_tok(g, 3))
        else:
            ct.append(str(g(300)) + ('' if g(3) else '.5'))
    ct.append(float_tok(g, 3) if fam in (6, 14) else float_tok(g, 1))
    if fam == 7:
        for i in range(1, 4):
            if g(2):
                ct[i] = NEAR_TOK[g(len(NEAR_TOK))]
        f.crt |= K_CRT_HALFWAY
    for _ in range(28):
        if fam == 4:
            v = pick(g, INT_EDGE)
        elif fam == 8:
            v = pick(g, INT_OVERFLOW) if g(3) == 0 else int_tok(g, 0)
            f.crt |= K_CRT_INT_OVERFLOW
        elif fam == 15:
            v = pick(g, INT_EDGE) if g(3) == 0 else int_tok(g, 0)
        else:
            v = int_tok(g, 1 if g(5) == 0 else 0)
        ct.append(v)
    if fam == 9:
        p = 7 + g(26)
        ct[p] = pick(g, EXOTIC_INT)
        f.crt |= K_CRT_EXOTIC_INT
    elif fam == 10:
        ct[4] = pick(g, EXOTIC_FLOAT)
        f.mask = (1 << 0x15) | (1 << 0x2E)
        f.crt |= K_CRT_EXOTIC_FLOAT | K_CRT_MASKED
    elif fam == 11:
        ct[1] = pick(g, EXOTIC_FLOAT)
        f.mask = (1 << 0x12) | (1 << 0x13) | (1 << 0x14) | (1 << 0x15) | (1 << 0x2E)
        f.crt |= K_CRT_EXOTIC_FLOAT | K_CRT_MASKED
    elif fam == 12:
        ct[0] = ["abc", "4294967296", "-", "0x10", "1e3", "3.7", "+", "99999999999", "2147483648"][g(9)]
        f.mask = (1 << 0x12) | (1 << 0x13) | (1 << 0x14) | (1 << 0x15) | (1 << 0x2E)
        f.crt |= K_CRT_EXOTIC_INT | K_CRT_MASKED
    keep = len(ct)
    if fam == 1:
        keep = 7 + g(27)
    elif fam == 2:
        keep = 7 + g(2)
    elif fam == 15 and g(2):
        keep = 7 + g(27)
    if fam in (9, 10, 11, 12):
        keep = len(ct)
    t += ct[:keep]
    f.line = join(g, t, fam == 13 or fam == 15)
    if fam == 13:
        f.line += '  ' + pad(g, 2)


def build_slot(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 6), 99 if fam == 8 else 0)
    for _ in range(7):
        if fam == 8:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        elif fam == 7:
            t.append(pick(g, FLOAT_PLAIN))
        else:
            t.append(float_tok(g, 3 if fam == 15 else 1))
    if fam == 5:
        w = ["0", "65535", "65536", "65537", "-1", "-32768", "32768", "70000", "255", "256", "131071", "4660", "-65536"]
        v2c, v2e, v2f = w[g(13)], int_tok(g, 0), int_tok(g, 0)
    elif fam == 6:
        v2c, v2e, v2f = int_tok(g, 0), pick(g, INT_EDGE), pick(g, INT_EDGE)
    elif fam == 9:
        v2c, v2e, v2f = pick(g, INT_OVERFLOW), pick(g, INT_OVERFLOW), pick(g, INT_OVERFLOW)
        f.crt |= K_CRT_INT_OVERFLOW
    elif fam == 13:
        v2c, v2e, v2f = str(-g(70000)), str(-g(300)), str(-g(300))
    elif fam == 14:
        v2c, v2e, v2f = '+' + str(g(70000)), '00' + str(g(256)), '+' + str(g(256))
    else:
        v2c, v2e, v2f = int_tok(g, 1 if g(4) == 0 else 3), int_tok(g, 0), int_tok(g, 0)
    t += [v2c, v2e]
    if fam == 1:
        nm = name(g, 1, 7)
    elif fam == 2:
        nm = name(g, 8, 8)
    elif fam == 3:
        nm = name(g, 9, 30)
    elif fam == 4:
        nm = name(g, 31, 100)
    elif fam == 10:
        nm = name(g, 1, 30, 1)
    elif fam == 15:
        nm = name(g, 1, 10 if g(2) else 100)
    else:
        nm = name(g, 1, 12)
    t += [nm, v2f]
    if fam == 12:
        t += [name(g, 1, 6), str(g(50))]
    f.line = join(g, t, fam == 11 or fam == 15)


def build_interior(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 7), 0)
    for _ in range(5):
        if fam == 9:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        elif fam == 8:
            t.append(pick(g, FLOAT_PLAIN))
        else:
            t.append(float_tok(g, 1))
    if fam == 7:
        t.append(["0", "65535", "65536", "65537", "-1", "-32768", "32768", "70000", "255", "256", "131071", "4660"][g(12)])
    elif fam == 10:
        t.append(pick(g, INT_OVERFLOW))
        f.crt |= K_CRT_INT_OVERFLOW
    else:
        t.append(int_tok(g, 1 if g(4) == 0 else 3))
    nn = {1: 0, 2: 1, 3: 2, 4: 3, 14: 5}.get(fam, 4)
    if fam == 15:
        nn = g(6)
    for _ in range(nn):
        if fam == 5:
            t.append(name(g, 15, 17))
        elif fam == 6:
            t.append(name(g, 18, 32))
        elif fam == 11:
            t.append(name(g, 1, 20, 1))
        elif fam == 13:
            t.append(name(g, 1, 1) if g(2) else name(g, 1, 4, 2))
        elif fam == 15:
            t.append(name(g, 1, 16 if g(2) else 32))
        else:
            t.append(name(g, 1, 14))
    f.line = join(g, t, fam == 12 or fam == 15)


def build_type8(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 8), 0)
    if fam == 0:
        v = int_tok(g, 0)
    elif fam in (1, 2):
        v = pick(g, INT_EDGE)
    elif fam == 3:
        v = str(g.raw() - 2147483648)
    elif fam == 4:
        v = pick(g, INT_OVERFLOW)
        f.crt |= K_CRT_INT_OVERFLOW
    elif fam == 5:
        v = ["3.9", "1e3", "0x10", "7abc", "-3.9", "0.5", "255.5", "1,5", "12#", "5."][g(10)]
    elif fam == 6:
        v = '+' + str(g(70000))
    elif fam == 7:
        v = '00' + str(g(70000))
    elif fam == 8:
        v = str(-g(70000))
    else:
        v = int_tok(g, 4)
    t.append(v)
    if fam == 9:
        t += [name(g, 1, 6), str(g(10))]
    f.line = join(g, t, fam == 10 or fam == 15)


def build_type9(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 9), 0)
    for _ in range(2):
        if fam == 4:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        elif fam == 5:
            t.append(pick(g, FLOAT_PLAIN))
        else:
            t.append(float_tok(g, 1))
    if fam in (1, 2):
        t.append(pick(g, INT_EDGE))
    elif fam == 3:
        t.append(str(-g(300)))
    elif fam == 6:
        t.append(pick(g, INT_OVERFLOW))
        f.crt |= K_CRT_INT_OVERFLOW
    elif fam == 7:
        t.append('+' + str(g(256)))
    else:
        t.append(int_tok(g, 0))
    f.line = join(g, t, fam == 8 or fam == 15)


def build_type10(g, f):
    fam = f.family
    t = []
    prefix(g, f, t, type_tok(g, f, 10), 0)
    for _ in range(9):
        if fam == 5:
            t.append(pick(g, HALFWAY))
            f.crt |= K_CRT_HALFWAY
        elif fam == 6:
            t.append(pick(g, FLOAT_PLAIN))
        else:
            t.append(float_tok(g, 1))
    if fam == 0:
        v = '0'
    elif fam == 1:
        v = '1'
    elif fam == 2:
        v = pick(g, INT_EDGE)
    elif fam == 3:
        v = ["256", "65536", "16777216", "-2147483648", "2147483647", "-1", "0.5", "0.9", "1.9", "-0.5", "0x10", "00", "-0", "+0", "000000000001"][g(15)]
    elif fam == 4:
        v = pick(g, INT_OVERFLOW)
        f.crt |= K_CRT_INT_OVERFLOW
    else:
        v = int_tok(g, 4)
    t.append(v)
    f.line = join(g, t, fam == 7 or fam == 15)


def build_oor(g, f):
    idx = (f.n // GROUPS) % (len(OOR_TYPES) + len(TYPE_OVERFLOW))
    if idx < len(OOR_TYPES):
        tok = OOR_TYPES[idx]
    else:
        tok = TYPE_OVERFLOW[idx - len(OOR_TYPES)]
        f.crt |= K_CRT_TYPE_OVERFLOW | K_CRT_INT_OVERFLOW
    build_plain(g, f, -1, tok)


def make(n):
    f = Fx()
    f.n, f.group, f.family = n, n % GROUPS, (n // GROUPS) % 16
    f.mask, f.crt = 0, 0
    g = Gen(mix((n * 2654435761 + 0x2D2F) & M32))
    f.model_id = g(MODEL_TABLE) if g(3) == 0 else ID_TABLE[g(len(ID_TABLE))]
    f.mi_label = g(16)
    f.store_k = g(MAX_STORE_K + 1)
    f.find_result = g(130) - 10
    f.eax, f.ecx, f.edx = g.raw(), g.raw(), g.raw()
    f.stack = STACK_SEEDS[(n // 5) % 8] if n % 5 == 0 else g.raw()
    fill_pre(f, g)
    f.tail = TAIL1
    table = {0: build_light, 1: build_particle, 3: build_attractor, 5: build_roadsign, 6: build_slot, 7: build_interior, 8: build_type8, 9: build_type9, 10: build_type10}
    if f.group in table:
        table[f.group](g, f)
    elif f.group in (2, 4):
        build_plain(g, f, f.group, '')
    else:
        build_oor(g, f)
    return f


def to_input(f):
    return dict(group=f.group, family=f.family, line=f.line, tail=f.tail, model_id=f.model_id, mi_label=f.mi_label, store_k=f.store_k, pre_entry=f.pre_entry.hex(), pre_over=f.pre_over.hex(),
                find_result=f.find_result, mask=f.mask, crt=f.crt, eax=f.eax, ecx=f.ecx, edx=f.edx, stack_seed=f.stack)


def main(argv):
    if len(argv) == 3 and argv[1] == '--dump':
        f = make(int(argv[2]))
        print(json.dumps(to_input(f), indent=1))
        want, st = oracle.expected(oracle.NAME, to_input(f), f.n)
        print(json.dumps(want, indent=1))
        print(sorted(st))
        return 0
    cov, bad, lens = {}, [], 0
    for n in range(CASES):
        f = make(n)
        inp = to_input(f)
        if len(f.line) + len(f.tail) + 2 + 64 > 0x800 - 0x40:
            bad.append((n, 'linea demasiado larga'))
            continue
        try:
            want, st = oracle.expected(oracle.NAME, inp, n)
        except RuntimeError as e:
            bad.append((n, str(e)))
            continue
        lens = max(lens, len(f.line))
        for k, v in st.items():
            if v is True:
                cov[k] = cov.get(k, 0) + 1
        for bit, nm in ((1, 'gen_halfway'), (2, 'gen_int_overflow'), (4, 'gen_exotic_float'), (8, 'gen_exotic_int'), (16, 'gen_type_overflow'), (32, 'gen_masked'), (64, 'gen_fail_tail')):
            if f.crt & bit:
                cov[nm] = cov.get(nm, 0) + 1
        for bit in range(64):
            if (f.mask >> bit) & 1:
                cov['mask%#x' % bit] = cov.get('mask%#x' % bit, 0) + 1
    print(json.dumps(dict(sorted(cov.items())), indent=0))
    print('longest line', lens, 'invalid', len(bad))
    for n, e in bad[:20]:
        print(' ', n, e[:300])
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
