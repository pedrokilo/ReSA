"""Oraculo independiente del banco StatsWrappers60: CStats::FindLeastFavoriteRadioStation (0x559010), BuildStatLine (0x559230), UpdateRespectStat (0x55BC50) y
ConstructStatLine (0x55A780).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco. Los colaboradores son los falsos de Fakes.hpp
(que el modelo reproduce: texto "G[clave]" de CText::Get, "A[texto]" de GxtCharToAscii, formato fijo del sprintf del EXE, valores deterministas de los demas) y la traza de
llamadas es parte del resultado. x87 con control de precision a 24 bits: cada operacion se redondea a 24 bits de mantisa con exponente extendido (modelo `V` de abajo, con
las reglas de NaN, infinito y cero con signo del x87); los enteros entran por fild (exactos).

  0x559010 FindLeast : estaciones 1..12 de 0xB78E58, minimo inicial lista[1], gana la primera estrictamente menor (con signo); devuelve el indice.
  0x559230 BuildStatLine(line, p1, metrics, p2, type) : ver build(). Sin linea no hace nada. gString2[0]=0; type==1 -> "%d:0%d"/"%d:%d"; p2 -> metricas 0, 1, 3;
           p1 -> metricas 0..9 (sin signo); cola: Get(linea) -> gGxtString, quita marcas, gString2 -> gGxtString2.
  0x55BC50 UpdateRespectStat(arg0) : ver respect().
  0x55A780 ConstructStatLine(arg0, arg1) : ver construct() (tablas 0x8CD8A0.., formatos 1..10, especiales 0..243 y 252..257 y 320..337).
"""
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NF = 'CStats::FindLeastFavoriteRadioStation'
NB = 'CStats::BuildStatLine'
NR = 'CStats::UpdateRespectStat'
NC = 'CStats::ConstructStatLine'
NAMES = [NF, NB, NR, NC]
CASES = {NF: 4096, NB: 6144, NR: 6144, NC: 8192}


class Fail(RuntimeError):
    """Fixture invalido (depende de comportamiento indefinido del EXE)."""


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def s16(v):
    v &= 0xFFFF
    return v - 0x10000 if v & 0x8000 else v


def s8(v):
    v &= 0xFF
    return v - 0x100 if v & 0x80 else v


def fnv32(b):
    h = 2166136261
    for x in b:
        h = ((h ^ x) * 16777619) & M32
    return h


def fnv64(b):
    h = 0xCBF29CE484222325
    for x in b:
        h = ((h ^ x) * 0x100000001B3) & 0xFFFFFFFFFFFFFFFF
    return h


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


# ------------------------------------------------------------------------------------------------ x87 a 24 bits (modelo del hardware)
class V:
    """Valor de la pila x87. k: 0 cero, 1 finito, 2 infinito, 3 NaN; s: signo; m: modulo (Fraction) si finito; sig: significando de 64 bits si NaN."""
    __slots__ = ('k', 's', 'm', 'sig')

    def __init__(self, k, s, m=None, sig=0):
        self.k, self.s, self.m, self.sig = k, s, m, sig


DEFAULT_NAN = V(3, 1, None, 0xC000000000000000)


def r24(x):
    """Redondeo al par de un racional positivo a 24 bits de mantisa, exponente sin limite (x87 con PC=24)."""
    x = Fraction(x)
    e = x.numerator.bit_length() - x.denominator.bit_length()
    if Fraction(2) ** e > x:
        e -= 1
    if Fraction(2) ** (e + 1) <= x:
        e += 1
    ulp = Fraction(2) ** (e - 23)
    q = x / ulp
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    return n * ulp


def ld_f32(bits):
    s, e, f = bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF
    if e == 0xFF:
        return V(2, s) if f == 0 else V(3, s, None, (1 << 63) | (1 << 62) | (f << 40))
    if e == 0:
        return V(0, s) if f == 0 else V(1, s, Fraction(f) * Fraction(1, 1 << 149))
    return V(1, s, Fraction(f | (1 << 23)) * (Fraction(2) ** (e - 150)))


def ld_int(i):
    i = s32(i)
    return V(0, 0) if i == 0 else V(1, 1 if i < 0 else 0, Fraction(abs(i)))


F0 = ld_f32(0)
F1 = ld_f32(0x3F800000)


def _nan_of(a, b):
    if a.k == 3 and b.k == 3:
        # x87: gana el NaN de mayor significando; con significandos iguales, el de signo positivo (comprobado en el hardware, casos 13560 y otros de la ampliacion offline)
        if a.sig != b.sig:
            c = a if a.sig > b.sig else b
        else:
            c = b if (a.s == 1 and b.s == 0) else a
    else:
        c = a if a.k == 3 else b
    return V(3, c.s, None, c.sig | (1 << 62))


def add(a, b):
    if a.k == 3 or b.k == 3:
        return _nan_of(a, b)
    if a.k == 2 or b.k == 2:
        if a.k == 2 and b.k == 2:
            return V(2, a.s) if a.s == b.s else DEFAULT_NAN
        return V(2, a.s) if a.k == 2 else V(2, b.s)
    if a.k == 0 and b.k == 0:
        return V(0, a.s & b.s)
    va = Fraction(0) if a.k == 0 else (-a.m if a.s else a.m)
    vb = Fraction(0) if b.k == 0 else (-b.m if b.s else b.m)
    t = va + vb
    if t == 0:
        return V(0, 0)
    return V(1, 1 if t < 0 else 0, r24(abs(t)))


def sub(a, b):
    if a.k == 3 or b.k == 3:
        return _nan_of(a, b)
    nb = V(b.k, b.s ^ 1, b.m, b.sig)
    return add(a, nb)


def mul(a, b):
    if a.k == 3 or b.k == 3:
        return _nan_of(a, b)
    s = a.s ^ b.s
    if (a.k == 0 and b.k == 2) or (a.k == 2 and b.k == 0):
        return DEFAULT_NAN
    if a.k == 2 or b.k == 2:
        return V(2, s)
    if a.k == 0 or b.k == 0:
        return V(0, s)
    return V(1, s, r24(a.m * b.m))


def div(a, b):
    if a.k == 3 or b.k == 3:
        return _nan_of(a, b)
    s = a.s ^ b.s
    if (a.k == 0 and b.k == 0) or (a.k == 2 and b.k == 2):
        return DEFAULT_NAN
    if b.k == 0:
        return V(2, s)
    if a.k == 0:
        return V(0, s)
    if a.k == 2:
        return V(2, s)
    if b.k == 2:
        return V(0, s)
    return V(1, s, r24(a.m / b.m))


def _key(v):
    if v.k == 0:
        return Fraction(0)
    if v.k == 2:
        return float('-inf') if v.s else float('inf')
    return -v.m if v.s else v.m


def fcom(a, b):
    """(c0, c2, c3) de fcom a, b."""
    if a.k == 3 or b.k == 3:
        return (1, 1, 1)
    ka, kb = _key(a), _key(b)
    if ka < kb:
        return (1, 0, 0)
    if ka == kb:
        return (0, 0, 1)
    return (0, 0, 0)


def ah41(c):      # test ah,41h != 0  (C0 | C3)
    return (c[0] | c[2]) != 0


def ah5_odd(c):   # test ah,5 ; jp no se toma (paridad impar: solo C0 sin C2, o solo C2)
    return (c[0] ^ c[1]) == 1


def f32_from_fraction(s, m):
    """Redondeo al par de un racional positivo a binary32 (con denormales y desbordamiento a infinito)."""
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    if Fraction(2) ** (e + 1) <= m:
        e += 1
    if e < -126:
        q = m / (Fraction(2) ** -149)
        n = q.numerator // q.denominator
        rem = q - n
        if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
            n += 1
        return (s << 31) | n
    q = m / (Fraction(2) ** (e - 23))
    n = q.numerator // q.denominator
    rem = q - n
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and n % 2 == 1):
        n += 1
    if n == 1 << 24:
        n >>= 1
        e += 1
    if e > 127:
        return (s << 31) | 0x7F800000
    return (s << 31) | ((e + 127) << 23) | (n - (1 << 23))


def st_f32(v):
    if v.k == 0:
        return v.s << 31
    if v.k == 2:
        return (v.s << 31) | 0x7F800000
    if v.k == 3:
        return (v.s << 31) | 0x7F800000 | ((v.sig >> 40) & 0x7FFFFF)
    return f32_from_fraction(v.s, v.m)


def st_f64(v):
    if v.k == 0:
        return v.s << 63
    if v.k == 2:
        return (v.s << 63) | 0x7FF0000000000000
    if v.k == 3:
        return (v.s << 63) | 0x7FF0000000000000 | ((v.sig >> 11) & 0xFFFFFFFFFFFFF)
    x = float(v.m)
    if Fraction(x) != v.m:
        raise Fail('double inexacto')
    b = struct.unpack('<Q', struct.pack('<d', x))[0]
    return b | (v.s << 63)


def ftol(v):
    """Palabra baja de _ftol2: truncado a 64 bits; NaN, infinito o fuera de rango -> 0."""
    if v.k == 0 or v.k == 2 or v.k == 3:
        return 0
    t = v.m.numerator // v.m.denominator
    if t >= 1 << 63:
        return 0
    return (-t if v.s else t) & M32


def f32_to_v(bits):
    return ld_f32(bits & M32)


# Constantes del EXE (floats)
K_KG = 0x3EE83E42        # 0x863F0C   0.4536
K_FT = 0x40555555        # 0x859F60   3.3333333
K_MIN = 0x378BCF65       # 0x863FD0   1/60000
K_HUNDRED = 0x42C80000   # 0x858628
K_RESP = 0x3ECCCCCD      # 0x858EE8  0.4
K_THOUSAND = 0x447A0000  # 0x858C4C
K_036 = 0x3EB851EC       # 0x863E10
K_003 = 0x3CF5C28F       # 0x858B10
K_02 = 0x3E4CCCCD        # 0x858CC4
K_1250 = 0x449C4000      # 0x864314
K_005 = 0x3D4CCCCD       # 0x858C28
K_1E7 = 0x33D6BF95       # 0x864310
K_TWO = 0x40000000       # 0x858CA0
K_NOVAL = 0xC2C60000     # 0x86430C  -99.0f


# ------------------------------------------------------------------------------------------------ maquina de los casos (memoria de texto + traza)
BUF_SIZE = {1: 352, 2: 352, 3: 552, 4: 552}      # 1 gString, 2 gString2, 3 gGxtString, 4 gGxtString2
BUF_INDEX = {2: 0, 1: 1, 4: 2, 3: 3}              # indice de patron de FillBuffers


def pattern(label, pat):
    i = BUF_INDEX[label]
    return bytearray(0x21 + ((pat + 31 * i + 17 * k) % 90) for k in range(BUF_SIZE[label]))


E_GET, E_G2A, E_STRCPY, E_STRIP, E_A2G, E_SPF, E_GETSTAT, E_METRIC, E_GERMAN, E_BUILD, E_MINS, E_SECS, E_PROGRESS, E_POPULATE, E_FINDMOST, E_FINDLEAST, \
    E_STATIONKEY, E_FINDPED, E_WEAPSKILL, E_TAGGED, E_THRESH, E_DISPLAY, E_CHECKMSG = range(1, 24)


class Mach:
    def __init__(self, pat):
        self.tr = []
        self.buf = {l: pattern(l, pat) for l in (1, 2, 3, 4)}
        self.text_next = 0

    # --- buffers
    def cstr(self, label):
        b = self.buf[label]
        i = b.find(0)
        if i < 0:
            raise Fail('cadena sin NUL en el buffer %d' % label)
        return bytes(b[:i])

    def put(self, label, data):
        if len(data) + 1 > BUF_SIZE[label]:
            raise Fail('desbordamiento del buffer %d' % label)
        self.buf[label][:len(data) + 1] = bytes(data) + b'\0'

    def ev(self, kind, a=0, b=0, c=0, d=0, e=0, f=0):
        self.tr.append([kind, a & M32, b & M32, c & M32, d, e, f])

    # --- falsos de texto
    def get(self, key):
        self.ev(E_GET, fnv32(key))
        return b'G[' + key[:148] + b']'

    def g2a(self, text, off=0):
        self.ev(E_G2A, fnv32(text), off)
        return b'A[' + text[off:][:166] + b']'

    def strcpy(self, label, src):
        self.ev(E_STRCPY, label, fnv32(src))
        self.put(label, src)

    def strip(self, label):
        self.ev(E_STRIP, label, fnv32(self.cstr(label)))

    def a2g(self, src_label, dst_label):
        src = self.cstr(src_label)
        self.ev(E_A2G, fnv32(src), dst_label)
        self.put(dst_label, src)

    def spf(self, label, fmt, *args):
        """sprintf del EXE sustituido: args = ('i', int) | ('s', bytes) | ('d', bits64)."""
        out = bytearray()
        evargs = []
        k = 0
        i = 0
        while i < len(fmt):
            c = fmt[i:i + 1]
            if c != b'%':
                out += c
                i += 1
                continue
            i += 1
            c = fmt[i:i + 1]
            if c == b'%':
                out += b'%'
                i += 1
            elif c == b'd':
                a = args[k]
                k += 1
                v = a[1] & M32                                  # un double como %d: palabra baja
                if a[0] not in ('i', 'd'):
                    raise Fail('%d con argumento no entero')
                out += str(s32(v)).encode()
                evargs.append(v)
                i += 1
            elif c == b's':
                a = args[k]
                k += 1
                out += a[1]
                evargs.append(fnv32(a[1]))
                i += 1
            elif c in (b'0', b'.'):
                while fmt[i:i + 1] != b'f':
                    i += 1
                i += 1
                a = args[k]
                k += 1
                if a[0] != 'd':
                    raise Fail('%f con argumento no double')
                out += b'{%016x}' % a[1]
                evargs.append(a[1])
            else:
                out += b'?'
                i += 1
        if len(out) > 340:
            out = out[:340]
        while len(evargs) < 3:
            evargs.append(0)
        self.ev(E_SPF, label, fnv32(fmt), self._nargs(fmt), evargs[0], evargs[1], evargs[2])
        self.put(label, bytes(out))

    @staticmethod
    def _nargs(fmt):
        n = 0
        i = 0
        while i < len(fmt):
            if fmt[i:i + 1] == b'%':
                i += 1
                c = fmt[i:i + 1]
                if c == b'%':
                    pass
                elif c in (b'd', b's'):
                    n += 1
                elif c in (b'0', b'.'):
                    while fmt[i:i + 1] != b'f':
                        i += 1
                    n += 1
            i += 1
        return min(n, 3)

    def observed_bufs(self):
        out = {}
        for key, label in (('s2', 2), ('s1', 1), ('g1', 3), ('g2', 4)):
            b = bytes(self.buf[label])
            out[key] = {'h': '%016x' % fnv64(b), 't': b[:64].hex()}
        return out


def dbl_of_f32(bits):
    return st_f64(ld_f32(bits))


# ------------------------------------------------------------------------------------------------ FindLeastFavoriteRadioStation
def expected_find(inp):
    lst = [s32(v) for v in inp['list']]
    least, index = lst[1], 1
    for i in range(1, 13):
        if lst[i] < least:
            least, index = lst[i], i
    stats = {'fam%d' % inp['family']: True, 'idx%d' % index: True, 'idx_first': index == 1,
             'tie_min': sum(1 for i in range(1, 13) if lst[i] == least) > 1, 'min_outside': min(lst) < least}
    return {'ret': index, 'outside_ok': True, 'first_outside': 0, 'trace': [], 'trace_dropped': 0}, stats


# ------------------------------------------------------------------------------------------------ BuildStatLine
def expected_build(inp):
    m = Mach(inp['pat'])
    line = bytes.fromhex(inp['line'])
    st = {'g%d' % inp['group']: True}
    v1, v2 = inp['v1'] & M32, inp['v2'] & M32
    metrics, typ = s32(inp['metrics']), s32(inp['type'])
    p1, p2 = not inp['p1_null'], not inp['p2_null']
    if inp['line_null']:
        st['line_null'] = True
        return _build_obs(m), st
    if typ == 1:
        if not (p1 and p2):
            raise Fail('type == 1 con puntero nulo')
        m.buf[2][0] = 0
        fmt = b'%d:0%d' if s32(v2) < 10 else b'%d:%d'
        st['type1_lt10' if s32(v2) < 10 else 'type1_ge10'] = True
        m.spf(2, fmt, ('i', v1), ('i', v2))
    elif p2:
        m.buf[2][0] = 0
        if metrics in (0, 1, 3) and not p1:
            raise Fail('p1 nulo con p2')
        if metrics == 0:
            of = m.g2a(m.get(b'FEST_OO'), 0)
            m.spf(2, b' %d %s %d', ('i', v1), ('s', of), ('i', v2))
            st['two_m0'] = True
        elif metrics in (1, 3):
            of = m.g2a(m.get(b'FEST_OO'), 0)
            m.spf(2, b'%.2f %s %.2f' if metrics == 1 else b'$%.2f %s $%.2f', ('d', dbl_of_f32(v1)), ('s', of), ('d', dbl_of_f32(v2)))
            st['two_m%d' % metrics] = True
        else:
            st['two_other'] = True
    elif p1 and 0 <= metrics <= 9:
        m.buf[2][0] = 0
        st['single_m%d' % metrics] = True
        if metrics == 0:
            m.spf(2, b'%d', ('i', v1))
        elif metrics == 1:
            m.spf(2, b'%.2f', ('d', dbl_of_f32(v1)))
        elif metrics == 2:
            m.spf(2, b'%0.2f%%', ('d', dbl_of_f32(v1)))
        elif metrics == 3:
            m.spf(2, b'$%.2f', ('d', dbl_of_f32(v1)))
        elif metrics == 4:
            m.spf(2, b'%d|', ('i', v1))
        elif metrics == 5:
            m.ev(E_METRIC)
            if inp['metric']:
                d = st_f64(mul(ld_int(v1), ld_f32(K_KG)))
                m.spf(2, b'%dkgs', ('d', d))
                st['kg'] = True
            else:
                m.spf(2, b'%dlbs', ('i', v1))
                st['lbs'] = True
        elif metrics == 6:
            mile = m.g2a(m.get(b'ST_MILE'), 0)
            m.spf(2, b'%.2f %s', ('d', dbl_of_f32(v1)), ('s', mile))
        elif metrics == 7:
            m.spf(2, b'%.2fm', ('d', dbl_of_f32(v1)))
        elif metrics == 8:
            m.spf(2, b'%.2fft', ('d', st_f64(mul(ld_f32(v1), ld_f32(K_FT)))))
        else:
            secs = m.g2a(m.get(b'ST_SECS'), 0)
            m.spf(2, b'%d %s', ('i', v1), ('s', secs))
    else:
        m.buf[2][0] = 0
        st['single_none'] = True
        if p1 and not (0 <= metrics <= 9):
            st['single_oob'] = True
        if not p1:
            st['single_null'] = True
    for v in (v1, v2):
        if ((v >> 23) & 0xFF) == 0xFF and (v & 0x7FFFFF):
            st['nan_arg'] = True
            if not (v & 0x400000):
                st['snan_arg'] = True
    # cola 0x5594D7
    text = m.get(line)
    m.strcpy(3, text)
    m.strip(3)
    m.a2g(2, 4)
    return _build_obs(m), st


def _build_obs(m):
    o = m.observed_bufs()
    o.update({'args_ok': True, 'outside_ok': True, 'first_outside': 0, 'trace': m.tr, 'trace_dropped': 0})
    return o


# ------------------------------------------------------------------------------------------------ UpdateRespectStat
def expected_resp(inp):
    st = {'f%d' % inp['family']: True}
    arg0 = inp['arg0'] & 0xFF
    low, high = inp['low'] & M32, inp['high'] & M32
    total_mem = inp['prev_total'] & M32
    tr = []
    obs = {'outside_ok': True, 'first_outside': 0, 'trace_dropped': 0}
    if arg0:
        st['arg0_nonzero'] = True
        obs.update({'total': total_mem, 'low': K_NOVAL, 'high': K_NOVAL, 'trace': tr})
        return obs, st
    # 0x55BC73..0x55BD33
    a = mul(ld_f32(inp['respect']), ld_f32(K_RESP))
    b = mul(ld_int(inp['missions']), ld_f32(K_THOUSAND))
    c = ld_int(inp['total'])
    if not ah41(fcom(F1, c)):
        c = F1
        st['total_clamped'] = True
    s = add(a, mul(div(b, c), ld_f32(K_036)))
    s = add(s, mul(ld_f32(inp['girl']), ld_f32(K_003)))
    t = sub(ld_f32(inp['territory']), ld_f32(K_02))
    if not ah41(fcom(F0, t)):
        t = F0
        st['territory_clamped'] = True
    partial = st_f32(add(mul(mul(t, ld_f32(K_1250)), ld_f32(K_005)), s))
    mv = mul(ld_int(inp['money'][inp['focus'] & 1]), ld_f32(K_1E7))
    if ah5_odd(fcom(F1, mv)):
        money_part = 0x3F800000
        st['money_clamped'] = True
    else:
        money_part = st_f32(mv)
    tr.append([E_TAGGED, 0, 0, 0, 0, 0, 0])
    tagged = s32(inp['tagged'])
    scaled = s32(tagged * 10)
    x = mul(ld_int(scaled), ld_f32(K_005))
    z = mul(mul(ld_f32(money_part), ld_f32(K_THOUSAND)), ld_f32(K_005))
    z = add(z, ld_f32(partial))
    z = add(z, mul(ld_f32(inp['w1']), ld_f32(K_003)))
    z = add(z, mul(ld_f32(inp['w2']), ld_f32(K_003)))
    total = add(x, z)
    respect = st_f32(total)
    if ah5_odd(fcom(total, F0)):
        respect = 0
        st['sum_negative'] = True
    if inp['cheat'] & 0xFF:
        respect = 0x447A0000
        st['cheat'] = True
    rv = ld_f32(respect)
    if ftol(ld_f32(high)) == ftol(rv):
        st['same_int'] = True
        obs.update({'total': total_mem, 'low': low, 'high': high, 'trace': tr})
        return obs, st
    total_mem = respect
    tr.append([E_CHECKMSG, 0, 0, 0, 0, 0, 0])
    if respect & 0x7F800000 == 0x7F800000 and respect & 0x7FFFFF:
        st['respect_nan'] = True
    for name, bits in (('low', low), ('high', high)):
        if bits & 0x7F800000 == 0x7F800000 and bits & 0x7FFFFF:
            st['%s_nan' % name] = True
    noval = ld_f32(K_NOVAL)
    c_rl = fcom(rv, ld_f32(low))
    two = ld_f32(K_TWO)
    decrease = ah41(c_rl) and not ah5_odd(fcom(ld_f32(high), two))
    have_both = not (ah44_odd(fcom(ld_f32(high), noval)) or ah44_odd(fcom(ld_f32(low), noval)))
    if decrease:
        st['decrease'] = True
        if have_both:
            thr = bool(inp['threshold'])
            tr.append([E_THRESH, 1, respect, 0, 0, 0, 0])
            show = thr or ah5_odd(fcom(rv, two))
            if show:
                tr.append([E_DISPLAY, 0, 0x40, respect, 0, 0, 0])
            st['dec_show'] = show
            st['dec_thr_true' if thr else 'dec_thr_false'] = True
        else:
            st['dec_noval'] = True
        if not ah41(fcom(rv, ld_f32(high))):
            low = respect
            high = respect
            st['dec_both'] = True
        else:
            high = respect
    else:
        st['increase'] = True
        if have_both:
            thr = bool(inp['threshold'])
            tr.append([E_THRESH, 1, respect, 0, 0, 0, 0])
            show = thr or ah5_odd(fcom(ld_f32(high), two))
            if show:
                tr.append([E_DISPLAY, 1, 0x40, respect, 0, 0, 0])
            st['inc_show'] = show
            st['inc_thr_true' if thr else 'inc_thr_false'] = True
        else:
            st['inc_noval'] = True
        if ah5_odd(fcom(rv, ld_f32(high))):
            low = respect
            st['inc_low'] = True
        high = respect
    obs.update({'total': total_mem, 'low': low, 'high': high, 'trace': tr})
    return obs, st


def ah44_odd(c):  # test ah,44h ; jnp se toma (paridad impar): igual (solo C3)
    return (c[1] ^ c[2]) == 1


# ------------------------------------------------------------------------------------------------ ConstructStatLine
REAL_TABLES = {
    0x8CD8A0: [(21, 10, 1, 0, 0), (22, 10, 1, 0, 0), (64, 10, 1, 0, 0), (225, 10, 1, 0, 0), (25, 10, 1, 0, 0), (23, 10, 1, 0, 0), (160, 10, 1, 0, 0), (223, 10, 1, 0, 0), (229, 10, 1, 0, 0), (230, 10, 1, 0, 0), (164, 0, 1, 1, 0), (81, 10, 0, 0, 0), (330, 0, 1, 1, 0), (170, 0, 1, 0, 0), (135, 0, 1, 0, 0), (184, 0, 0, 0, 0), (186, 0, 0, 0, 0), (188, 0, 0, 0, 0), (185, 0, 0, 0, 0), (187, 0, 0, 0, 0), (189, 0, 0, 0, 0), (190, 0, 0, 0, 0), (200, 0, 1, 0, 0), (233, 0, 1, 0, 0), (-99, 0, 0, 0, 0)],
    0x8CD938: [(53, 3, 0, 0, 0), (13, 3, 1, 0, 0), (14, 3, 1, 0, 0), (15, 3, 1, 0, 0), (16, 3, 1, 0, 0), (30, 3, 1, 0, 0), (31, 3, 1, 0, 0), (33, 3, 1, 0, 0), (54, 3, 1, 0, 0), (55, 3, 1, 0, 0), (20, 3, 1, 0, 0), (62, 3, 1, 0, 0), (35, 3, 0, 0, 0), (37, 3, 0, 0, 0), (334, 3, 0, 1, 0), (38, 3, 0, 0, 0), (39, 3, 0, 0, 0), (41, 3, 0, 0, 0), (40, 3, 0, 0, 0), (-99, 0, 0, 0, 0)],
    0x8CD9B0: [(328, 0, 1, 1, 0), (329, 0, 1, 1, 0), (126, 0, 1, 0, 0), (127, 0, 1, 0, 0), (128, 0, 1, 0, 0), (-99, 0, 0, 0, 0)],
    0x8CD9D8: [(191, 0, 0, 0, 0), (192, 0, 0, 0, 0), (182, 0, 0, 0, 0), (183, 0, 0, 0, 0), (194, 0, 0, 0, 0), (131, 0, 1, 0, 0), (132, 0, 1, 0, 0), (133, 0, 1, 0, 0), (155, 0, 0, 0, 0), (217, 0, 0, 0, 0), (218, 0, 0, 0, 0), (325, 0, 1, 1, 0), (324, 0, 1, 1, 0), (120, 0, 1, 0, 0), (121, 0, 1, 0, 0), (122, 0, 1, 0, 0), (123, 0, 1, 0, 0), (124, 0, 1, 0, 0), (125, 3, 1, 0, 0), (129, 0, 1, 0, 0), (130, 0, 1, 0, 0), (216, 0, 1, 0, 0), (176, 0, 0, 0, 0), (177, 0, 1, 0, 0), (-99, 0, 0, 0, 0)],
    0x8CDA70: [(323, 0, 1, 1, 0), (234, 0, 0, 0, 0), (235, 0, 0, 0, 0), (337, 0, 0, 1, 0), (236, 0, 0, 0, 0), (237, 0, 0, 0, 0), (238, 0, 0, 0, 0), (163, 0, 0, 0, 0), (239, 0, 0, 0, 0), (240, 0, 0, 0, 0), (331, 0, 1, 1, 0), (332, 0, 1, 1, 0), (333, 0, 1, 1, 0), (-99, 0, 0, 0, 0)],
    0x8CDAC8: [(252, 0, 0, 1, 0), (253, 0, 0, 1, 0), (254, 0, 0, 1, 0), (255, 0, 0, 1, 0), (256, 0, 0, 1, 0), (257, 0, 0, 1, 0), (3, 7, 0, 0, 0), (26, 7, 0, 0, 0), (4, 7, 0, 0, 0), (27, 7, 0, 0, 0), (5, 7, 0, 0, 0), (6, 7, 0, 0, 0), (7, 7, 0, 0, 0), (8, 7, 0, 0, 0), (9, 7, 0, 0, 0), (28, 7, 0, 0, 0), (169, 0, 1, 1, 0), (173, 0, 1, 1, 0), (44, 9, 0, 0, 0), (29, 7, 0, 0, 0), (45, 9, 0, 0, 0), (143, 0, 1, 1, 0), (144, 0, 1, 1, 0), (145, 0, 1, 1, 0), (10, 7, 1, 0, 0), (17, 9, 1, 0, 0), (11, 7, 1, 0, 0), (18, 9, 1, 0, 0), (12, 7, 1, 0, 0), (19, 9, 1, 0, 0), (139, 7, 1, 0, 0), (140, 0, 1, 0, 0), (141, 4, 1, 0, 0), (142, 4, 1, 0, 0), (52, 9, 1, 0, 0), (51, 9, 1, 0, 0), (156, 0, 0, 0, 0), (2, 7, 0, 0, 0), (46, 5, 0, 0, 0), (47, 5, 0, 0, 0), (205, 0, 0, 0, 0), (206, 0, 0, 0, 0), (207, 0, 0, 0, 0), (208, 0, 0, 0, 0), (209, 0, 0, 0, 0), (322, 0, 1, 1, 0), (231, 0, 1, 1, 0), (243, 0, 1, 1, 0), (241, 0, 1, 1, 0), (-99, 0, 0, 0, 0)],
    0x8CDBF8: [(146, 0, 1, 0, 0), (147, 0, 1, 0, 0), (157, 0, 0, 0, 0), (152, 0, 0, 0, 0), (159, 0, 0, 0, 0), (153, 0, 0, 0, 0), (158, 0, 0, 0, 0), (151, 0, 0, 0, 0), (149, 3, 0, 0, 0), (150, 0, 0, 0, 0), (161, 0, 0, 0, 0), (162, 3, 0, 0, 0), (210, 0, 0, 0, 0), (171, 0, 0, 0, 0), (36, 3, 0, 0, 0), (213, 0, 0, 1, 0), (214, 0, 0, 0, 0), (201, 3, 0, 0, 0), (202, 9, 0, 0, 0), (154, 0, 0, 0, 0), (166, 0, 0, 0, 0), (167, 0, 0, 1, 0), (168, 0, 0, 1, 0), (246, 9, 0, 0, 0), (247, 9, 0, 0, 0), (248, 9, 0, 0, 0), (249, 9, 0, 0, 0), (250, 9, 0, 0, 0), (172, 0, 0, 0, 0), (48, 9, 0, 0, 0), (212, 9, 0, 0, 0), (221, 0, 0, 0, 0), (220, 9, 0, 0, 0), (219, 9, 0, 0, 0), (49, 9, 0, 0, 0), (222, 9, 0, 0, 0), (174, 0, 0, 1, 0), (175, 0, 0, 1, 0), (179, 9, 0, 0, 0), (178, 0, 0, 0, 0), (211, 0, 0, 0, 0), (215, 0, 0, 0, 0), (258, 0, 0, 0, 0), (259, 9, 0, 0, 0), (260, 0, 0, 0, 0), (261, 9, 0, 0, 0), (262, 0, 0, 0, 0), (263, 9, 0, 0, 0), (264, 0, 0, 0, 0), (265, 9, 0, 0, 0), (266, 0, 0, 0, 0), (267, 9, 0, 0, 0), (268, 0, 0, 0, 0), (269, 9, 0, 0, 0), (270, 0, 0, 0, 0), (271, 9, 0, 0, 0), (272, 0, 0, 0, 0), (273, 9, 0, 0, 0), (274, 0, 0, 0, 0), (275, 9, 0, 0, 0), (276, 0, 0, 0, 0), (277, 9, 0, 0, 0), (278, 0, 0, 0, 0), (279, 9, 0, 0, 0), (280, 0, 0, 0, 0), (281, 9, 0, 0, 0), (282, 0, 0, 0, 0), (283, 9, 0, 0, 0), (284, 0, 0, 0, 0), (285, 9, 0, 0, 0), (286, 0, 0, 0, 0), (287, 9, 0, 0, 0), (288, 0, 0, 0, 0), (289, 9, 0, 0, 0), (290, 0, 0, 0, 0), (291, 9, 0, 0, 0), (292, 0, 0, 0, 0), (293, 9, 0, 0, 0), (294, 0, 0, 0, 0), (295, 9, 0, 0, 0), (296, 9, 0, 0, 0), (297, 9, 0, 0, 0), (298, 9, 0, 0, 0), (299, 9, 0, 0, 0), (300, 9, 0, 0, 0), (301, 9, 0, 0, 0), (-99, 0, 0, 0, 0)],
    0x8CDE08: [(0, 0, 1, 1, 0), (320, 0, 1, 1, 0), (134, 0, 1, 0, 0), (136, 0, 1, 0, 0), (137, 0, 0, 0, 0), (138, 0, 1, 0, 0), (326, 0, 1, 1, 0), (327, 0, 1, 1, 0), (195, 0, 0, 0, 0), (196, 0, 0, 0, 0), (197, 0, 0, 0, 0), (198, 0, 0, 0, 0), (-99, 0, 0, 0, 0)],
}
TABLE_BASES = [0x8CD8A0, 0x8CD938, 0x8CD9B0, 0x8CD9D8, 0x8CDA70, 0x8CDAC8, 0x8CDBF8]
GERMAN_KEEP = [0, 0] + [1] * 8 + [0] + [1] * 27 + [0] * 5       # 0x55B6F8 (indice = byte bajo del id - 167)
SPECIAL_HANDLER = {0: 0, 143: 1, 144: 2, 145: 3, 164: 4, 169: 5, 173: 6, 174: 7, 175: 8, 213: 9, 231: 10, 241: 11, 243: 12}
STUNT_KEYS = [b'INSTUN', b'PRINST', b'DBINST', b'DBPINS', b'TRINST', b'PRTRST', b'QUINST', b'PQUINS']
GANG_KEYS = [b'ST_GNG%d' % i for i in range(8)]


def table_entries(inp):
    arg1 = inp['arg1'] & 0xFF
    if arg1 <= 6:
        return REAL_TABLES[TABLE_BASES[arg1]]
    if not inp['synth']:
        return REAL_TABLES[0x8CDE08]
    tab = bytes.fromhex(inp['tab'])
    ents = []
    for i in range(13):
        e = tab[6 * i:6 * i + 6]
        ents.append((s16(e[0] | (e[1] << 8)), e[2], e[3], e[4], e[5]))
        if ents[-1][0] == -99:
            return ents
    raise Fail('tabla sintetica sin terminador')


class ConsState:
    def __init__(self, inp):
        self.inp = inp
        self.m = Mach(inp['pat'])
        self.words = struct.unpack('<344I', bytes.fromhex(inp['words']))
        self.fl = struct.unpack('<82I', bytes.fromhex(inp['fl']))
        self.bar = inp['bar0'] & 0xFFFF
        self.st = {}
        self.count = 0                  # esi: lineas contadas
        self.arg0 = s32(inp['arg0'])    # ebp: linea pedida

    def here(self):
        return self.count == self.arg0

    def raw_int(self, uid):
        if uid >= 344:
            raise Fail('identificador fuera de la region modelada')
        return s32(self.words[uid])

    def raw_word(self, address):
        i = (address - 0xB78E20) // 4
        return self.words[i]

    def value(self, uid):
        """ST0 tras CStats::GetStatValue (0x558E40): float o entero exacto, y traza de la llamada."""
        self.m.ev(E_GETSTAT, uid & 0xFFFF)
        if uid < 0x52:
            return ld_f32(self.fl[uid])
        return ld_int(self.raw_int(uid))


def construct(inp):
    S = ConsState(inp)
    m, st = S.m, S.st
    arg0 = s32(inp['arg0'])
    arg1 = inp['arg1'] & 0xFF
    ents = table_entries(inp)
    st['t%d' % (arg1 if arg1 <= 6 else 7)] = True
    st['synth' if (arg1 > 6 and inp['synth']) else 'real'] = True
    ret = None
    for idx in range(len(ents)):
        e = ents[idx]
        if e[0] == -99:
            break
        res = _entry(S, e)
        if res == 'done':
            ret = 0
            break
        if res == 'next':
            S.count += 1
    if ret is None:
        ret = S.count
        st['beyond'] = True
    else:
        st['found'] = True
    o = m.observed_bufs()
    o.update({'ret': ret & M32, 'bar': S.bar, 'ped_ok': True, 'outside_ok': True, 'first_outside': 0, 'trace': m.tr, 'trace_dropped': 0})
    return o, st


def _tail_std(S):
    """0x55A8D4: Get(gString) -> gGxtString, quita marcas, gString2 -> gGxtString2."""
    m = S.m
    text = m.get(m.cstr(1))
    m.strcpy(3, text)
    m.strip(3)
    m.a2g(2, 4)
    return 'done'


def _tail_key(S, key):
    """gGxtString[0] = 0 y 0x55B68F: Get(clave) -> gGxtString2."""
    m = S.m
    m.buf[3][0] = 0
    m.strcpy(4, m.get(key))
    return 'done'


def _out_of(S, v1, v2):
    """0x55AC1F: ' %d %s %d' con 'FEST_OO'."""
    m = S.m
    m.buf[2][0] = 0
    of = m.g2a(m.get(b'FEST_OO'), 0)
    m.spf(2, b' %d %s %d', ('i', v1), ('s', of), ('i', v2))
    return _tail_std(S)


def _line_int(S, fmt, v):
    S.m.buf[2][0] = 0
    S.m.spf(2, fmt, ('i', v))
    return _tail_std(S)


def _line_dbl(S, fmt, bits):
    S.m.buf[2][0] = 0
    S.m.spf(2, fmt, ('d', bits))
    return _tail_std(S)


def _header(S):
    S.m.buf[2][0] = 0
    return _tail_std(S)


def _clock(S, hours, minutes):
    """BuildStatLine(gString, &hours, 0, &minutes, 1) (falso de ConstructStatLine) y devuelve 0."""
    S.m.ev(E_BUILD, fnv32(S.m.cstr(1)), hours & M32, 0, minutes & M32, 1)
    return 'done'


def _minutes_total(S, value):
    return s32(ftol(mul(ld_int(value), ld_f32(K_MIN))))


def _idiv60(total):
    q = abs(total) // 60
    q = -q if total < 0 else q
    return q, total - q * 60


def _entry(S, e):
    m, st = S.m, S.st
    inp = S.inp
    id16, fmt, always, special = e[0], e[1], e[2], e[3]     # (id, formato, siempre, especial, relleno)
    uid = id16 & 0xFFFF
    if always == 0:
        # 0x55A7ED: solo se muestra si la estadistica es mayor que 0 (fcomp 0.0; test ah,41h; jne): un NaN la omite
        if uid < 0x52:
            v = ld_f32(S.fl[uid])
            positive = v.k in (1, 2) and v.s == 0
        else:
            positive = S.raw_int(uid) > 0
        if not positive:
            st['skip_nonpositive'] = True
            return 'skip'
    # 0x55A81D: clave STAT%03d en gString
    m.spf(1, b'STAT00%d' if id16 < 10 else (b'STAT0%d' if id16 < 100 else b'STAT%d'), ('i', id16 & M32))
    m.ev(E_GERMAN)
    if inp['german']:
        index = ((id16 & 0xFF) - 167) & M32
        if index <= 0x2A and GERMAN_KEEP[index] == 0:
            st['german_skip'] = True
            return 'skip'
        st['german_keep'] = True
    S.bar = 0
    # --- formato (no especial)
    if special == 0:
        f = fmt if 1 <= fmt <= 10 else 0
        st['fmt%d' % f] = True
        if f == 10:
            S.bar = id16 & 0xFFFF
            if not S.here():
                return 'next'
            return _line_int(S, b'%d', ftol(S.value(uid)))
        if f == 5:
            if not S.here():
                return 'next'
            v = ftol(S.value(uid))
            m.buf[2][0] = 0
            m.ev(E_METRIC)
            if inp['metric']:
                m.spf(2, b'%dkgs', ('d', st_f64(mul(ld_int(v), ld_f32(K_KG)))))
            else:
                m.spf(2, b'%dlbs', ('i', v))
            return _tail_std(S)
        if f == 3:
            if not S.here():
                return 'next'
            return _line_dbl(S, b'$%.2f', st_f64(S.value(uid)))
        if f == 4:
            if not S.here():
                return 'next'
            return _line_int(S, b'%d|', ftol(S.value(uid)))
        if f == 9:
            if not S.here():
                return 'next'
            v = ftol(S.value(uid))
            m.ev(E_MINS, v)
            mins = (v ^ inp['mins']) & M32
            v2 = ftol(S.value(uid))
            m.ev(E_SECS, v2)
            secs = (v2 + inp['secs']) & M32
            return _clock(S, mins, secs)
        if f == 1:
            if not S.here():
                return 'next'
            return _line_dbl(S, b'%.2f', st_f64(S.value(uid)))
        if f == 6:
            if not S.here():
                return 'next'
            val = ld_f32(st_f32(S.value(uid)))
            m.buf[2][0] = 0
            mile = m.g2a(m.get(b'ST_MILE'), 0)
            m.spf(2, b'%.2f %s', ('d', st_f64(val)), ('s', mile))
            return _tail_std(S)
        if f == 7:
            m.ev(E_METRIC)
            if inp['metric']:
                if not S.here():
                    return 'next'
                return _line_dbl(S, b'%.2fm', st_f64(S.value(uid)))
            if not S.here():
                return 'next'
            return _line_dbl(S, b'%.2fft', st_f64(mul(S.value(uid), ld_f32(K_FT))))
        # 0x55AAAA (formatos 0, 2, 8 y mayores de 10)
        if not S.here():
            return 'next'
        if uid < 0x52:
            val = ftol(ld_f32(S.fl[uid]))
        else:
            val = S.raw_int(uid) & M32
        return _line_int(S, b'%d', val)
    # --- estadisticas especiales (0x55AAD0)
    sid = id16
    st['special'] = True
    if sid > 0x101:
        n = sid - 0x140
        if n < 0 or n > 0x11:
            return 'skip'
        return _special_high(S, n, sid)
    if sid >= 0xFC:
        if uid < 0x52:
            v = ld_f32(S.fl[uid])
            positive = v.k in (1, 2) and v.s == 0
        else:
            positive = S.raw_int(uid) > 0
        if not positive:
            return 'skip'
        if not S.here():
            return 'next'
        st['h_girls'] = True
        return _line_dbl(S, b'%0.2f%%', st_f64(S.value(uid)))
    if sid < 0 or sid > 0xF3:
        return 'skip'
    h = SPECIAL_HANDLER.get(sid, 13)
    if h == 13:
        return 'skip'
    st['h%d' % h] = True
    if h == 0:
        if not S.here():
            return 'next'
        m.ev(E_PROGRESS)
        return _line_dbl(S, b'%0.2f%%', dbl_of_f32(inp['progress']))
    if h == 1:
        if S.here():
            st['h1_header'] = True
            return _header(S)
        award = S.raw_int(143)
        S.count += 1
        which = (award - 1) & M32
        if not S.here():
            return 'next'
        return _tail_key(S, STUNT_KEYS[which] if which <= 7 else b'NOSTUC')
    if h == 2:
        if not S.here():
            return 'next'
        return _out_of(S, S.raw_word(0xB79060), inp['a9a89c'])
    if h == 3:
        if not S.here():
            return 'next'
        return _out_of(S, S.raw_word(0xB79064), inp['a9a89c'])
    if h == 4:
        if not S.here():
            return 'next'
        return _line_int(S, b'%d', ftol(ld_f32(inp['ped_armor'])))
    if h == 5:
        total = _minutes_total(S, S.raw_word(0xB790C4))
        if not S.here():
            return 'next'
        q, r = _idiv60(total)
        return _clock(S, q, r)
    if h == 6:
        total = _minutes_total(S, S.raw_word(0xB790D4))
        q, r = _idiv60(total)
        if not (q > 0 or r > 0):
            return 'skip'
        if not S.here():
            return 'next'
        return _clock(S, q, r)
    pairs = {7: (0xB790D8, None, 12), 8: (0xB790DC, None, 25), 9: (0xB79174, None, 30), 10: (0xB791BC, 0xB791C0, None), 11: (0xB791E4, 0xB791E8, None), 12: (0xB791EC, 0xB791F0, None)}
    a1, a2, const = pairs[h]
    if not S.here():
        return 'next'
    v1 = S.raw_word(a1)
    v2 = const if a2 is None else S.raw_word(a2)
    return _out_of(S, v1, v2)


def _special_high(S, n, sid):
    m, st, inp = S.m, S.st, S.inp
    st['hi%d' % sid] = True
    if n == 0:     # 320: tiempo jugado (ms / 60000 por multiplicacion)
        elapsed = inp['timer'] & M32
        total = (((0x45E7B273 * elapsed) >> 32) & M32) >> 14
        if not S.here():
            return 'next'
        q, r = _idiv60(total)
        return _clock(S, q, r)
    if n == 2:     # 322
        if not S.here():
            return 'next'
        return _out_of(S, inp['a9ad74'], inp['a9ad70'])
    if n == 3:     # 323: banda menos favorita
        best, largest = 0, 0
        for i in range(7, 15):
            kills = S.raw_word(0xB78E90 + 4 * i)
            if kills > largest:
                best, largest = i, kills
        if best == 0:
            return 'skip'
        if S.here():
            return _header(S)
        S.count += 1
        if not S.here():
            return 'next'
        return _tail_key(S, GANG_KEYS[best - 7])
    if n == 4:     # 324: suma de nueve contadores
        if not S.here():
            return 'next'
        total = 0
        for i in range(7, 16):
            total = (total + S.raw_word(0xB78E90 + 4 * i)) & M32
        return _line_int(S, b'%d', total)
    if n == 5:     # 325
        if not S.here():
            return 'next'
        return _line_int(S, b'%d', S.raw_word(0xB78EE0))
    if n in (6, 7):    # 326 / 327: emisora mas / menos escuchada
        m.ev(E_POPULATE)
        if inp['populate']:
            return 'skip'
        if S.here():
            return _header(S)
        S.count += 1
        if n == 6:
            m.ev(E_FINDMOST)
            sta = inp['most']
        else:
            m.ev(E_FINDLEAST)
            sta = inp['least']
        m.ev(E_STATIONKEY, sta)
        name = b'RK%05X' % (sta & 0xFFFFF)
        if not S.here():
            return 'next'
        return _tail_key(S, name)
    if n == 8:     # 328: habilidad del arma actual
        m.ev(E_FINDPED, M32)
        ptype = inp['ped_type'] & M32
        t16 = ptype & 0xFFFF
        if t16 == 0x20:
            bl = (0x1C - 0x15) & 0xFF
        elif 0x16 <= t16 <= 0x20:
            bl = (t16 - 0x15) & 0xFF
        else:
            return 'skip'
        if S.here():
            m.buf[2][0] = 0
            stat = (bl + 0x44) & 0xFFFF
            v = ftol(S.value(stat))
            m.spf(2, b'%d', ('i', v))
            return _tail_std(S)
        S.count += 1
        m.ev(E_FINDPED, M32)
        if ptype == 0x20:
            bl = 0xB
            m.spf(1, b'STWE0%d', ('i', bl))
        elif bl < 10:
            m.spf(1, b'STWE00%d', ('i', bl))
        elif bl < 100:
            m.spf(1, b'STWE0%d', ('i', bl))
        else:
            m.spf(1, b'STWE%d', ('i', bl))
        if not S.here():
            return 'next'
        return _tail_key(S, m.cstr(1))
    if n == 9:     # 329: niveles de habilidad de las armas 22..31
        if S.here():
            return _header(S)
        S.count += 1
        for w in range(1, 0xB):
            m.spf(1, b'STWE00%d' if w < 10 else (b'STWE0%d' if w < 100 else b'STWE%d'), ('i', w))
            typ = w + 0x15
            m.ev(E_FINDPED, M32)
            m.ev(E_WEAPSKILL, typ, 1)
            skill = s8(inp['ws'][typ - 22] & 0xFF)
            m.spf(2, b'WS_STD' if skill == 1 else (b'WS_PRO' if skill == 2 else b'WS_POOR'))
            if S.here():
                m.strcpy(3, m.get(m.cstr(1)))
                m.strcpy(4, m.get(m.cstr(2)))
                return 'done'
            S.count += 1
        return 'skip'
    if n == 10:    # 330: ranking de piloto
        total = _minutes_total(S, S.raw_word(0xB790C4))
        q, r = _idiv60(total)
        if q > 0 or r >= 5:
            if S.here():
                return _header(S)
            S.count += 1
        key = None
        if q <= 0:
            if r < 5:
                return 'skip'
            if r < 10:
                key = b'ST_PR01'
            elif r < 20:
                key = b'ST_PR02'
            elif r < 30:
                key = b'ST_PR03'
        if key is None:
            tiers = [(1, 0, b'ST_PR04'), (1, 30, b'ST_PR05'), (2, 0, b'ST_PR06'), (2, 30, b'ST_PR07'), (3, 0, b'ST_PR08'), (3, 30, b'ST_PR09'),
                     (4, 0, b'ST_PR10'), (5, 0, b'ST_PR11'), (10, 0, b'ST_PR12'), (20, 0, b'ST_PR13'), (25, 0, b'ST_PR14'), (30, 0, b'ST_PR15'),
                     (49, 2, b'ST_PR16'), (50, 0, b'ST_PR17'), (100, 0, b'ST_PR18')]
            for lim, rlim, k in tiers:
                if q <= lim and r < rlim:
                    key = k
                    break
            if key is None:
                key = b'ST_PR19'
        st['pr_' + key.decode()] = True
        if not S.here():
            return 'next'
        return _tail_key(S, key)
    if n in (11, 12, 13):    # 331..333: bandas mas fuertes
        gv = s32(inp['gang'][3 + (sid - 331)])
        gl = s32(inp['gang'][sid - 331])
        if gv < 0:
            return 'skip'
        m.spf(2, b'ST_GNG%d', ('i', gv))
        m.spf(1, b'ST_LAB%d', ('i', sid - 331))
        text = m.get(m.cstr(1))
        if S.here():
            m.strcpy(3, text)
            m.strcpy(4, m.get(m.cstr(2)))
            return 'done'
        S.count += 1
        m.spf(1, b'%d', ('i', gl))
        m.a2g(1, 4)
        if S.here():
            m.buf[3][0] = 0
            m.strcpy(4, m.cstr(4))
            return 'done'
        return 'next'
    if n == 14:    # 334: dinero perdido en el juego
        t = sub(ld_f32(S.fl[35]), ld_f32(S.fl[37]))
        if ah5_odd(fcom(t, F0)):
            t = F0
        if not S.here():
            return 'next'
        return _line_dbl(S, b'$%.2f', st_f64(t))
    if n == 17:    # 337: territorio bajo control
        if not S.here():
            return 'next'
        return _line_dbl(S, b'%0.2f%%', st_f64(mul(ld_f32(inp['territory']), ld_f32(K_HUNDRED))))
    return 'skip'


# ------------------------------------------------------------------------------------------------ interfaz
def expected(function, inp, case):
    if function == NF:
        return expected_find(inp)
    if function == NB:
        return expected_build(inp)
    if function == NR:
        return expected_resp(inp)
    if function == NC:
        return construct(inp)
    raise RuntimeError('funcion desconocida %s' % function)


def compare(obs, want):
    """Claves que difieren (obs = salida de un lado; want = modelo)."""
    bad = []
    for k, v in want.items():
        if obs.get(k) != v:
            bad.append(k)
    return bad


def _selftest():
    # FindLeast: el primero estrictamente menor
    inp = {'family': 0, 'list': [5, 3, 2, 2, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0]}
    assert expected_find(inp)[0]['ret'] == 2
    inp = {'family': 0, 'list': [0, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, -5]}
    assert expected_find(inp)[0]['ret'] == 1
    # r24 y conversiones
    assert r24(Fraction(1) + Fraction(1, 1 << 24)) == 1
    assert st_f32(mul(ld_f32(0x3F800000), ld_f32(0x40000000))) == 0x40000000
    assert st_f32(ld_f32(0x7F800001)) == 0x7FC00001          # sNaN silenciado
    assert st_f32(add(ld_f32(0x7F7FFFFF), ld_f32(0x7F7FFFFF))) == 0x7F800000    # desborda al guardar en float
    assert st_f64(add(ld_f32(0x7F7FFFFF), ld_f32(0x7F7FFFFF))) == struct.unpack('<Q', struct.pack('<d', float(Fraction(0xFFFFFF << 105))))[0]
    assert ftol(mul(ld_int(100000), ld_f32(K_MIN))) == 1
    assert ftol(F1) == 1 and ftol(ld_f32(0xBF800000)) == M32 and ftol(ld_f32(0x7FC00000)) == 0
    assert st_f32(sub(ld_f32(0x7F800000), ld_f32(0x7F800000))) == 0xFFC00000    # inf - inf = indefinido
    # fcom
    assert fcom(F0, ld_f32(0x80000000)) == (0, 0, 1)
    assert fcom(F1, ld_f32(0x7FC00000)) == (1, 1, 1)
    # f32_from_fraction con denormales
    assert f32_from_fraction(0, Fraction(1, 1 << 149)) == 1
    assert f32_from_fraction(0, Fraction(3, 1 << 150)) == 2          # 1.5 ulp -> al par
    assert f32_from_fraction(0, Fraction(1, 1 << 150)) == 0          # medio ulp -> al par (0)
    # sprintf falso
    m = Mach(0)
    m.spf(2, b'%d:0%d', ('i', 3), ('i', 7))
    assert m.cstr(2) == b'3:07'
    m.spf(2, b'%dkgs', ('d', 0x1122334455667788))
    assert m.cstr(2) == str(s32(0x55667788)).encode() + b'kgs'
    # BuildStatLine: type == 1
    inp = {'group': 0, 'line': b'AB'.hex(), 'line_null': False, 'p1_null': False, 'p2_null': False, 'v1': 1, 'v2': 5, 'metrics': 0, 'type': 1, 'metric': False, 'pat': 0}
    o, _ = expected_build(inp)
    assert [e[0] for e in o['trace']] == [E_SPF, E_GET, E_STRCPY, E_STRIP, E_A2G]
    # tablas: la 0x8CD9B0 tiene 5 entradas
    assert len(REAL_TABLES[0x8CD9B0]) == 6 and REAL_TABLES[0x8CD9B0][-1][0] == -99
    assert len(GERMAN_KEEP) == 43
    return True


if __name__ == '__main__':
    print('ok' if _selftest() else 'fallo')
