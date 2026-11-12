"""Oraculo independiente (Python) de Fn174A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 20 funciones del lote 174A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.
La aritmetica x87 (24 bits de mantisa por la palabra de control del juego, exponente ampliado) se modela con fracciones exactas (NaN con signo y carga util; el seno y el coseno con series de Taylor de 100 digitos
redondeadas a 64 bits de mantisa, |h| <= 8): el modelo es el de Fn116A.

Desensamblado del EXE (sobre el gta-sa.exe canonico; vtables leidas del propio EXE; ranura = indice del dword en la vtable).
   0  0x57C400  CPad::IsMouseWheelUpPressed (cdecl): al = [B7341B] != 0 y [B7342F] == 0 (bytes); mov al,1 / xor al,al
   1  0x57C420  CPad::IsMouseWheelDownPressed: bytes [B7341C] y [B73430]
   2  0x57C440  CPad::IsMouseBmx1Pressed: bytes [B7341D] y [B73431]
   3  0x57C460  CPad::IsMouseBmx2Pressed: bytes [B7341E] y [B73432]
   4  0x57C5E0  CMenuManager::NoDiskInDriveMessage (thiscall): 53D690 con siete ceros (cdecl); si [C17050] (dword) != 0 termina; 746870; 541DD0; 579330 (ecx = this; pila: 86566Ch = "NO_PCCD", 1, 0);
                71A210; 53D840; 53FB70(0); si word [B733A8] != 0 y word [B73138] == 0: byte [this+58h] = 1 y 619B60(1Eh, 0)
   5  0x6C4340  CHeli cuerpo del destructor (vtable 0x871680, marco SEH): [esi] = 871680h; si [esi+A0C] != 0: n = 6D3F30(ecx = this) (con signo; se relee cada pasada); mientras i < n: si entrada i != 0: 4AA3F0 (ecx = entrada),
                4A9810 (ecx = A9AE80h, entrada releida); 8213AE([esi+A0C]) (cdecl); [esi+A0C] = 0; 4FB8C0 (ecx = this + 138h); 6A61E0 (ecx = this)
   6  0x6C4550  CHeli::SetupDamageAfterLoad (ranura virtual): and byte [ecx+42Ah],0FEh
   7  0x6C4810  CHeli destructor con borrado (vtable 0x871680): cuerpo 6C4340; CVehicle::operator delete 6E2D90 si bit 0 de las banderas (byte); devuelve this
   8  0x6C7D10  CMonsterTruck cuerpo del destructor (vtable 0x8717D8): mov [ecx],8717D8h; jmp 6A61E0
   9  0x718E50  CFontChar::Set (thiscall, `ret 4`): copia dword +0, +4, +8, +0Ch, +10h; bytes +14h..+17h; dwords +18h, +1Ch, +20h, +24h; bytes +28h..+2Ah; word +2Ch; byte +2Eh (cada lectura seguida de su escritura); eax = this
  10  0x719240  CFont::StripTildeTags (cdecl): copia el texto a un buffer local de 0x100 bytes y lo reescribe en el sitio sin lo que hay entre pares de '~' (inclusive)
  11  0x719360  CFont::MapExtendedChar (cdecl): al = ch si ch < 0x80; si no el byte de 8D5AD8h + 2 * ch
  12  0x54ED20  Common::TransformPointsSimple (cdecl; salida, cuenta, &transformacion, entrada): c y s = fcos y fsin de h (+0Ch) guardados como float; sin puntos no hace nada; por punto:
                x' = ((c*x) - (s*y)) + pos.x guardado; y' = ((s*x) + (c*y)) + pos.y con x releido tras escribir x' (y la posicion releida de memoria); z' = z + pos.z
  13  0x54EEA0  Common::TransformVectors (CMatrix) (cdecl; salida, cuenta, matriz, entrada): 59B8B0(ecx = matriz, matriz de trabajo [B72920]); 7F18A0(matriz de trabajo); por vector (cuenta sin signo, 0 = nada):
                7EDDC0(salida, entrada, matriz de trabajo)
  14  0x71DB30  Common::DoesInfiniteLineCrossFiniteLine (cdecl; ocho floats A, B, origen, direccion): ((Ax-ox)*dy - (Ay-oy)*dx) * ((Bx-ox)*dy - (By-oy)*dx) < 0.0 (fcomp: un NaN da 0); eax = 1 o 0
  15  0x6F4FA0  tHandlingData constructor (thiscall): 6D0450(ecx = this + 2Ch); eax = this
  16  0x6F5440  CTrainNode::GetPosn (thiscall, `ret 4`): [out] = int16([this]) * 0.125f, [out+4] = int16([this+2]) * 0.125f, [out+8] = int16([this+4]) * 0.125f; eax = out
  17  0x6F54D0  CTrain cuerpo del destructor (vtable 0x872370, marco SEH): [esi] = 872370h; 4FB8C0 (ecx = this + 138h); 6E2B40 (ecx = this)
  18  0x6F5530  CTrain::SetModelIndex (ranura virtual, `ret 4`): 6D6A40(ecx = this, indice); 17 dwords a cero desde +668h; 4C5440(clump [+18h], &[+668h])
  19  0x6F5570  CTrain::PreRender (ranura virtual): 6D6480(ecx = this); byte [+5B8h] & 8: DoVehicleLights 6E1A60(ecx = this, matriz [+14h], 5 si word [+22h] == 1C1h si no 25h); si no, & 10h: 15h

Los falsos de los colaboradores anotan: cuerpos y bases [direccion, ECX (etiqueta de arena), dword de [ECX] al entrar] (dejan un dword del caso en [ECX]); operator delete [direccion, puntero, dword del puntero]; el resto
[direccion, argumentos] (punteros como etiquetas de arena; la matriz de trabajo como 0x5A5A0001). Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import json
import struct
import sys
from decimal import Decimal, getcontext
from fractions import Fraction

getcontext().prec = 100
M32 = 0xFFFFFFFF
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD
NAMES = ['CPad::IsMouseWheelUpPressed', 'CPad::IsMouseWheelDownPressed', 'CPad::IsMouseBmx1Pressed', 'CPad::IsMouseBmx2Pressed', 'CMenuManager::NoDiskInDriveMessage', 'CHeli::DestructorBody',
         'CHeli::SetupDamageAfterLoad', 'CHeli::ScalarDeletingDestructor', 'CMonsterTruck::DestructorBody', 'CFontChar::Set', 'CFont::StripTildeTags', 'CFont::MapExtendedChar',
         'Common::TransformPointsSimple', 'Common::TransformVectors', 'Common::DoesInfiniteLineCrossFiniteLine', 'tHandlingData::Constructor', 'CTrainNode::GetPosn', 'CTrain::DestructorBody',
         'CTrain::SetModelIndex', 'CTrain::PreRender']
ENTRIES = (0x57C400, 0x57C420, 0x57C440, 0x57C460, 0x57C5E0, 0x6C4340, 0x6C4550, 0x6C4810, 0x6C7D10, 0x718E50, 0x719240, 0x719360, 0x54ED20, 0x54EEA0, 0x71DB30, 0x6F4FA0, 0x6F5440, 0x6F54D0, 0x6F5530, 0x6F5570)
DEFAULT_ARENAS = (0x30000000, 0x30100000, 0x30200000)     # solo para la autoprueba sin juego: el banco anota las direcciones reales en cada entrada
WM = 0x5A5A0001                                            # etiqueta de la matriz de trabajo (puntero global en 0xB72920)

MOUSE, NODISK, HELIBODY, HELIDMG, SDD, VTBODY, FONTSET, STRIP, MAPCHAR, POINTS, VECS, CROSS, HANDCTOR, NODE, TRAINSET, TRAINPRE = (
    'mouse nodisk helibody helidmg sdd vtbody fontset strip mapchar points vecs cross handctor node trainset trainpre'.split())
KIND = [MOUSE, MOUSE, MOUSE, MOUSE, NODISK, HELIBODY, HELIDMG, SDD, VTBODY, FONTSET, STRIP, MAPCHAR, POINTS, VECS, CROSS, HANDCTOR, NODE, VTBODY, TRAINSET, TRAINPRE]
SPAN = [4, 4, 4, 4, 0x5C, 0xA10, 0x430, 4, 4, 0x30, 4, 4, 0x10, 4, 4, 0x30, 8, 0x13C, 0x6AC, 0x5BC]
# tabla de 0x8D5AD8 + 2 * ch para ch = 0x80..0xFF (un byte por entrada; leida del EXE)
TABLE = [0] * 33 + [176] + [0] * 18 + [177] + [0] * 5 + [95] + [0] * 4 + [175, 128, 129, 130, 0, 131, 0, 132, 133, 134, 135, 136, 137, 138, 139, 140, 141, 0, 173, 142, 143, 144, 0, 145, 0, 0, 146, 147, 148, 149, 0, 0,
         150, 151, 152, 153, 0, 154, 0, 155, 156, 157, 158, 159, 160, 161, 162, 163, 164, 0, 174, 165, 166, 167, 0, 168, 0, 0, 169, 170, 171, 172, 0, 0, 0]
assert len(TABLE) == 128


# ------------------------------------------------------------------------------------------------ modelo x87 (24 bits, exponente ampliado)
# valor: ('f', Fraction, signo) | ('i', signo) | ('n', signo, significando de 64 bits con el bit 63 explicito y el 62 = silencioso)
INDEF = ('n', 1, 0xC000000000000000)


def load(bits):
    """fld dword: los NaN se silencian y conservan signo y carga util"""
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        if m == 0:
            return ('i', neg)
        return ('n', neg, (1 << 63) | (1 << 62) | (m << 40))
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rndn(v, mant_bits, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - (mant_bits - 1)
    if quantum_exp_min is not None and q < quantum_exp_min:
        q = quantum_exp_min
    quantum = Fraction(2) ** q
    k = a / quantum
    fl = k.numerator // k.denominator
    rem = k - fl
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and fl % 2 == 1):
        fl += 1
    r = fl * quantum
    return -r if v < 0 else r


def rnd(v, quantum_exp_min=None):
    return rndn(v, 24, quantum_exp_min)


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def nan_pick(a, b):
    """dos NaN: gana el de mayor significando; con significandos iguales gana el de signo positivo (medido en el EXE: Invert con rumbo NaN)"""
    if a[0] == 'n' and b[0] == 'n':
        if a[2] != b[2]:
            return a if a[2] > b[2] else b
        return a if a[1] == 0 else (b if b[1] == 0 else a)
    return a if a[0] == 'n' else b


def is_nan(a):
    return a[0] == 'n'


def mul(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return INDEF
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else INDEF
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def chs(a):
    """fchs: invierte el signo (tambien de NaN, infinitos y ceros)"""
    if a[0] == 'n':
        return ('n', 1 - a[1], a[2])
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    """fsub / fsubp: a - b (con un NaN devuelve el NaN sin cambiar su signo)"""
    if is_nan(a) or is_nan(b):
        return nan_pick(a, b)
    return add(a, chs(b))


def fabs_(a):
    if a[0] == 'n':
        return ('n', 0, a[2])
    if a[0] == 'i':
        return ('i', 0)
    return ('f', abs(a[1]), 0)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def ge(a, b):
    """a >= b ordenado (fcomp + test ah,1): falso con NaN"""
    if is_nan(a) or is_nan(b):
        return False
    return num(a) >= num(b)


def le(a, b):
    if is_nan(a) or is_nan(b):
        return False
    return num(a) <= num(b)


def lt(a, b):
    if is_nan(a) or is_nan(b):
        return False
    return num(a) < num(b)


def store(a):
    """fstp dword: bits del float"""
    if a[0] == 'n':
        return (a[1] << 31) | 0x7F800000 | ((a[2] >> 40) & 0x7FFFFF)
    if a[0] == 'i':
        return 0x7F800000 | (a[1] << 31)
    v = a[1]
    if v == 0:
        return a[2] << 31
    mag = abs(v)
    r = rnd(mag, -149)
    sign = 1 if v < 0 else 0
    if r >= Fraction(2) ** 128:
        return 0x7F800000 | (sign << 31)
    if r < Fraction(2) ** -126:
        return (sign << 31) | int(r / Fraction(2) ** -149)
    e = r.numerator.bit_length() - r.denominator.bit_length()
    if Fraction(2) ** e > r:
        e -= 1
    elif Fraction(2) ** (e + 1) <= r:
        e += 1
    m = int(r / Fraction(2) ** (e - 23)) & 0x7FFFFF
    return (sign << 31) | ((e + 127) << 23) | m


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo"""
    if a[0] == 'n':
        return a[2].to_bytes(8, 'little') + (((a[1] << 15) | 0x7FFF)).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + (neg << 15).to_bytes(2, 'little')
    sign = 1 if v < 0 else 0
    m = abs(v)
    e = m.numerator.bit_length() - m.denominator.bit_length()
    if Fraction(2) ** e > m:
        e -= 1
    elif Fraction(2) ** (e + 1) <= m:
        e += 1
    mant = m / Fraction(2) ** (e - 63)
    assert mant.denominator == 1
    return int(mant).to_bytes(8, 'little') + ((sign << 15) | (e + 16383)).to_bytes(2, 'little')


def fp_stack(hexstr):
    """(TOP, lista de (st, 10 bytes)) de las posiciones validas de una imagen fnsave normalizada"""
    img = bytes.fromhex(hexstr)
    fsw = int.from_bytes(img[4:6], 'little')
    ftw = int.from_bytes(img[8:10], 'little')
    top = (fsw >> 11) & 7
    valid = []
    for st in range(8):
        phys = (top + st) & 7
        if ((ftw >> (2 * phys)) & 3) != 3:
            valid.append((st, img[28 + 10 * st:38 + 10 * st]))
    return top, valid


# seno y coseno de la x87 (series de Taylor en 100 digitos, redondeados a 64 bits de mantisa; |a| <= 8)
def sin_cos_dec(a):
    a2 = a * a
    eps = Decimal(10) ** -95
    s, c = a, Decimal(1)
    ts, tc = a, Decimal(1)
    n = 1
    while True:
        tc = -tc * a2 / (n * (n + 1))
        ts = -ts * a2 / ((n + 1) * (n + 2))
        n += 2
        c += tc
        s += ts
        if abs(tc) < eps and abs(ts) < eps:
            return s, c


_trig_cache = {}


def sin_cos(h_bits):
    """(seno, coseno) de la x87 como valores del modelo (64 bits de mantisa); NaN/infinito segun fsin/fcos"""
    if h_bits in _trig_cache:
        return _trig_cache[h_bits]
    h = load(h_bits)
    if h[0] == 'n':
        res = (h, h)
    elif h[0] == 'i':
        res = (INDEF, INDEF)
    else:
        v = h[1]
        if v == 0:
            res = (('f', Fraction(0), h[2]), ('f', Fraction(1), 0))
        else:
            assert abs(v) <= 8, 'rumbo fuera del dominio del modelo'
            d = Decimal(v.numerator) / Decimal(v.denominator)
            s_, c_ = sin_cos_dec(d)
            fs = rndn(Fraction(s_), 64)
            fc = rndn(Fraction(c_), 64)
            res = (('f', fs, 1 if fs < 0 else 0), ('f', fc, 1 if fc < 0 else 0))
    _trig_cache[h_bits] = res
    return res



def cls_float(bits_):
    e = (bits_ >> 23) & 0xFF
    m = bits_ & 0x7FFFFF
    if e == 0xFF:
        return 'nan' if m else 'inf'
    if e == 0:
        return 'zero' if m == 0 else 'sub'
    return 'norm'


# ------------------------------------------------------------------------------------------------ memoria de las arenas
def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rand(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def fill_words(seed, nbytes):
    """FillWords de Fixtures.hpp: el byte i de la region es el byte (i % 4) de rand(seed, i // 4)"""
    return [(rand(seed, i >> 2) >> (8 * (i & 3))) & 0xFF for i in range(nbytes)]


def tag(arena, off):
    """etiqueta de un puntero dentro de la arena `arena` (0 A, 1 B, 2 C): 0x10000000 + (indice << 20) + guarda + desplazamiento"""
    return 0x10000000 + (arena << 20) + GUARD + off


class Mem:
    """arena dispersa: el relleno inicial se calcula bajo demanda; `m` guarda los bytes escritos; `r` la imagen posterior a la preparacion del caso (referencia para los cambios)"""

    def __init__(self, kind, seed):
        self.kind, self.seed, self.m, self.r = kind, seed, {}, {}
        self.words = {}

    def base(self, i):
        if self.kind:
            return 0x00 if self.kind == 1 else (0xFF if self.kind == 2 else 0xCD)
        w = self.words.get(i >> 2)
        if w is None:
            w = self.words[i >> 2] = rand(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def wrb(self, off, data):
        for k, b in enumerate(data):
            self.m[GUARD + off + k] = b

    def snap(self):
        self.r = dict(self.m)
        self.m = dict(self.m)

    def changes(self):
        out = []
        for i in sorted(self.m):
            ref = self.r.get(i)
            if ref is None:
                ref = self.base(i)
            if self.m[i] != ref:
                out.append([i - GUARD, self.m[i]])
        return out


class World:
    """las tres arenas con sus direcciones reales (o por defecto en la autoprueba): lectura y escritura por direccion absoluta"""

    def __init__(self, c):
        a = [x & M32 for x in c['a']]
        self.bases = tuple(c.get('arena') or DEFAULT_ARENAS)
        self.mem = (Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14]))

    def raw(self, arena, off):
        return (self.bases[arena] + GUARD + off) & M32

    def locate(self, addr):
        for i, b in enumerate(self.bases):
            if b <= addr < b + N:
                return i, addr - b - GUARD
        raise ValueError('direccion fuera de las arenas: %x' % addr)

    def rd(self, addr, n):
        i, off = self.locate(addr)
        return self.mem[i].rd(off, n)

    def wr(self, addr, n, v):
        i, off = self.locate(addr)
        self.mem[i].wr(off, n, v)

    def tagv(self, v):
        """etiqueta de un valor: si cae dentro de una arena, (indice, desplazamiento); si no, el valor crudo"""
        if v == 0:
            return 0
        for i, b in enumerate(self.bases):
            if b <= v < b + N:
                return 0x10000000 + (i << 20) + (v - b)
        return v

    def snap(self):
        for m in self.mem:
            m.snap()


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    k = KIND[fn]
    if k not in (MOUSE, MAPCHAR, CROSS):
        marks.append('this_unaligned' if a[0] % 4 else 'this_aligned')
        if a[0] + SPAN[fn] > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(20):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    _kind = KIND[_fn]
    if _kind not in (MOUSE, MAPCHAR, CROSS):
        _k += ['this_aligned', 'this_unaligned']
    if _kind == MOUSE:
        _k += ['pressed', 'new_set_old_set', 'new_clear', 'new_nonbool', 'old_nonbool', 'old_set_new_clear']
    elif _kind == NODISK:
        _k += ['quit', 'noquit', 'esc_pressed', 'esc_not_pressed', 'esc_both', 'quit_esc']
    elif _kind == HELIBODY:
        _k += ['spill', 'table_null', 'table_set', 'guns_zero', 'guns_neg', 'guns_pos', 'entry_null', 'entry_arena', 'entry_raw', 'guns_many']
    elif _kind == HELIDMG:
        _k += ['spill', 'bit_set', 'bit_clear']
    elif _kind == SDD:
        _k += ['spill', 'prev_zero', 'prev_other', 'flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only']
    elif _kind == VTBODY:
        _k += ['spill', 'prev_zero', 'prev_other']
    elif _kind == FONTSET:
        _k += ['spill', 'alias', 'separate']
    elif _kind == STRIP:
        _k += ['spill', 'empty', 'no_tags', 'tags', 'empty_tag', 'high_chars', 'tail_text']
    elif _kind == MAPCHAR:
        _k += ['low', 'high_zero', 'high_mapped']
    elif _kind == POINTS:
        _k += ['spill', 'count_0', 'count_1', 'count_many', 'alias', 'separate', 'nan_component', 'inf_component', 'heading_finite', 'heading_nan', 'heading_inf', 'heading_zero']
    elif _kind == VECS:
        _k += ['spill', 'count_0', 'count_1', 'count_many', 'alias', 'separate']
    elif _kind == CROSS:
        _k += ['true', 'false', 'nan_product', 'zero_product', 'inf_operand']
    elif _kind == HANDCTOR:
        _k += ['spill', 'prev_zero', 'prev_other']
    elif _kind == NODE:
        _k += ['spill', 'neg_word', 'pos_word', 'extreme_word']
    elif _kind == TRAINSET:
        _k += ['spill', 'clump_null', 'clump_arena']
    elif _kind == TRAINPRE:
        _k += ['spill', 'front', 'last', 'neither', 'front_tram', 'front_other']
    KEYS[_fn] = _k


def snan_bits(bits):
    return (bits & 0x7F800000) == 0x7F800000 and (bits & 0x7FFFFF) != 0 and not (bits & 0x400000)


def mulm(a, bits):
    """fmul dword [mem] con a en un registro: un NaN senalizante de memoria cede ante un NaN silencioso del registro (tabla de Intel: se devuelve el operando QNaN)"""
    if snan_bits(bits) and is_nan(a):
        return a
    return mul(a, load(bits))


def addm(a, bits):
    """fadd dword [mem] con a en un registro (misma regla que mulm)"""
    if snan_bits(bits) and is_nan(a):
        return a
    return add(a, load(bits))


def base_result(W, events, ret, gch, marks):
    A, B, C = W.mem
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'marks': marks}


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    k = KIND[fn]
    marks = common_marks(fn, a)
    W = World(c)
    A, B, Cc = W.mem
    events, ret, gch = [], None, []
    o = a[0]
    if k == MOUSE:
        new, old = a[3] & 0xFF, a[4] & 0xFF
        W.snap()
        ret = 1 if (new != 0 and old == 0) else 0
        marks.append('pressed' if ret else ('new_set_old_set' if new != 0 else 'new_clear'))
        if new == 0 and old != 0:
            marks.append('old_set_new_clear')
        if new not in (0, 1):
            marks.append('new_nonbool')
        if old not in (0, 1):
            marks.append('old_nonbool')
    elif k == NODISK:
        quit_, esc_new, esc_old = a[3], a[4] & 0xFFFF, a[5] & 0xFFFF
        A.wr(o + 0x58, 4, a[6])
        W.snap()
        events.append([0x53D690, 0, 0, 0, 0, 0, 0, 0])
        pressed = esc_new != 0 and esc_old == 0
        if quit_ == 0:
            events += [[0x746870], [0x541DD0], [0x579330, tag(0, o), fnv(b'NO_PCCD'), 1, 0], [0x71A210], [0x53D840], [0x53FB70, 0]]
            if pressed:
                A.wr(o + 0x58, 1, 1)
                events.append([0x619B60, 0x1E, 0])
            marks.append('noquit')
            marks.append('esc_pressed' if pressed else ('esc_both' if esc_new != 0 else 'esc_not_pressed'))
        else:
            marks.append('quit')
            if pressed:
                marks.append('quit_esc')
    elif k == HELIBODY:
        A.wr(o, 4, a[3])
        if a[6]:
            A.wr(o + 0xA0C, 4, W.raw(1, a[7]))
            for i in range(16):
                mode = a[32 + i]
                B.wr(a[7] + 4 * i, 4, 0 if mode == 0 else (W.raw(2, a[16 + i]) if mode == 1 else a[16 + i]))
        else:
            A.wr(o + 0xA0C, 4, 0)
        W.snap()
        A.wr(o, 4, 0x871680)
        n = s32(a[8])
        table = A.rd(o + 0xA0C, 4)
        marks.append('table_set' if table else 'table_null')
        if table:
            marks.append('guns_zero' if n == 0 else ('guns_neg' if n < 0 else 'guns_pos'))
            if n >= 6:
                marks.append('guns_many')
            i = 0
            while True:
                events.append([0x6D3F30, tag(0, o)])
                if not (i < n):
                    break
                ptr = W.rd(table + 4 * i, 4)
                if ptr != 0:
                    events.append([0x4AA3F0, W.tagv(ptr)])
                    events.append([0x4A9810, 0xA9AE80, W.tagv(W.rd(table + 4 * i, 4))])
                    marks.append('entry_arena' if a[32 + i] == 1 else 'entry_raw')
                else:
                    marks.append('entry_null')
                i += 1
            events.append([0x8213AE, W.tagv(table), W.rd(table, 4)])
            A.wr(o + 0xA0C, 4, 0)
        events.append([0x4FB8C0, tag(0, o + 0x138)])
        events.append([0x6A61E0, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
    elif k == HELIDMG:
        A.wr(o + 0x424, 4, a[3])
        A.wr(o + 0x428, 4, a[4])
        A.wr(o + 0x42C, 4, a[5])
        W.snap()
        b = A.rd(o + 0x42A, 1)
        marks.append('bit_set' if b & 1 else 'bit_clear')
        A.wr(o + 0x42A, 1, b & 0xFE)
    elif k == SDD:
        A.wr(o, 4, a[3])
        W.snap()
        events.append([0x6C4340, tag(0, o), A.rd(o, 4)])
        A.wr(o, 4, a[5])
        flags = a[6] & 0xFF                               # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([0x6E2D90, tag(0, o), A.rd(o, 4)])   # la liberacion ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        marks.append('flag_high_bits' if a[6] >> 8 else 'flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == VTBODY:
        A.wr(o, 4, a[3])
        W.snap()
        if fn == 8:
            events.append([0x6A61E0, tag(0, o), 0x8717D8])
        else:
            events.append([0x4FB8C0, tag(0, o + 0x138)])
            events.append([0x6E2B40, tag(0, o), 0x872370])
        A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == FONTSET:
        A.wrb(o, fill_words(a[12] ^ 0xAAAA, 0x30))
        if a[4]:
            src_mem, src_off = A, o
            marks.append('alias')
        else:
            B.wrb(a[3], fill_words(a[13] ^ 0x5555, 0x30))
            src_mem, src_off = B, a[3]
            marks.append('separate')
        W.snap()
        for off, n in [(0, 4), (4, 4), (8, 4), (0xC, 4), (0x10, 4), (0x14, 1), (0x15, 1), (0x16, 1), (0x17, 1), (0x18, 4), (0x1C, 4), (0x20, 4), (0x24, 4), (0x28, 1), (0x29, 1), (0x2A, 1), (0x2C, 2), (0x2E, 1)]:
            A.wr(o + off, n, src_mem.rd(src_off + off, n))
        ret = tag(0, o)
    elif k == STRIP:
        s = bytes.fromhex(c['s'])
        B.wrb(a[3], s + b'\x00')
        W.snap()
        buf = list(s) + [0]
        out = []
        p = 0
        ch = buf[0]
        if ch:
            while True:
                if ch != 0x7E:
                    out.append(ch)
                else:
                    p += 1
                    ch = buf[p]
                    if ch != 0x7E:
                        while True:
                            p += 1
                            ch = buf[p]
                            if ch == 0x7E:
                                break
                p += 1
                ch = buf[p]
                if not ch:
                    break
        B.wrb(a[3], bytes(out) + b'\x00')
        marks.append('empty' if not s else 'tags' if 0x7E in s else 'no_tags')
        if b'~~' in s:
            marks.append('empty_tag')
        if any(x >= 0x80 for x in s):
            marks.append('high_chars')
        if 0x7E in s and s[-1] != 0x7E:
            marks.append('tail_text')
    elif k == MAPCHAR:
        ch = a[3] & 0xFF
        W.snap()
        if ch < 0x80:
            ret = ch
            marks.append('low')
        else:
            ret = TABLE[ch - 0x80]
            marks.append('high_mapped' if ret else 'high_zero')
    elif k == POINTS:
        count, alias = a[3], a[4]
        A.wr(o, 4, a[5]); A.wr(o + 4, 4, a[6]); A.wr(o + 8, 4, a[7]); A.wr(o + 0xC, 4, a[8])
        for i in range(18):
            B.wr(a[9] + 4 * i, 4, a[16 + i])
        in_mem, in_off = B, a[9]
        out_mem, out_off = (B, a[9]) if alias else (Cc, a[10])
        W.snap()
        h = a[8]
        sn, cs = sin_cos(h)
        cf = load(store(cs))
        sf = load(store(sn))
        marks.append('alias' if alias else 'separate')
        marks.append('count_0' if count == 0 else ('count_1' if count == 1 else 'count_many'))
        for i in range(count):
            x, y = in_mem.rd(in_off + 12 * i, 4), in_mem.rd(in_off + 12 * i + 4, 4)
            out_mem.wr(out_off + 12 * i, 4, store(addm(sub(mulm(cf, x), mulm(sf, y)), A.rd(o, 4))))
            x, y = in_mem.rd(in_off + 12 * i, 4), in_mem.rd(in_off + 12 * i + 4, 4)       # releidos tras escribir x'
            out_mem.wr(out_off + 12 * i + 4, 4, store(addm(add(mulm(sf, x), mulm(cf, y)), A.rd(o + 4, 4))))
            z = load(in_mem.rd(in_off + 12 * i + 8, 4))
            out_mem.wr(out_off + 12 * i + 8, 4, store(addm(z, A.rd(o + 8, 4))))
        hc = cls_float(h)
        marks.append({'zero': 'heading_zero', 'nan': 'heading_nan', 'inf': 'heading_inf'}.get(hc, 'heading_finite'))
        vals = [a[16 + j] for j in range(3 * count)] + [a[5], a[6], a[7]]
        if count and any(cls_float(v) == 'nan' for v in vals):
            marks.append('nan_component')
        if count and any(cls_float(v) == 'inf' for v in vals):
            marks.append('inf_component')
    elif k == VECS:
        count, alias = a[3], a[4]
        for i in range(18):
            B.wr(a[9] + 4 * i, 4, a[16 + i])
        in_off = a[9]
        out_arena, out_off = (1, a[9]) if alias else (2, a[10])
        W.snap()
        marks.append('alias' if alias else 'separate')
        marks.append('count_0' if count == 0 else ('count_1' if count == 1 else 'count_many'))
        events.append([0x59B8B0, tag(0, o), WM])
        events.append([0x7F18A0, WM])
        out_mem = W.mem[out_arena]
        for i in range(count):
            inp = W.raw(1, in_off + 12 * i)
            outp = W.raw(out_arena, out_off + 12 * i)
            events.append([0x7EDDC0, W.tagv(outp), W.tagv(inp), WM])
            x, y, z = B.rd(in_off + 12 * i, 4), B.rd(in_off + 12 * i + 4, 4), B.rd(in_off + 12 * i + 8, 4)
            out_mem.wr(out_off + 12 * i, 4, x ^ 0x11111111)
            out_mem.wr(out_off + 12 * i + 4, 4, y ^ 0x22222222)
            out_mem.wr(out_off + 12 * i + 8, 4, z ^ 0x33333333)
    elif k == CROSS:
        W.snap()
        ax, ay, bx, by, ox, oy, dx, dy = [load(a[3 + i]) for i in range(8)]
        u = sub(mul(sub(ax, ox), dy), mul(sub(ay, oy), dx))
        v = sub(mul(sub(bx, ox), dy), mul(sub(by, oy), dx))
        p = mul(u, v)
        ret = 1 if lt(p, ('f', Fraction(0), 0)) else 0
        marks.append('true' if ret else 'false')
        if is_nan(p):
            marks.append('nan_product')
        elif p[0] == 'f' and p[1] == 0:
            marks.append('zero_product')
        if any(cls_float(a[3 + i]) == 'inf' for i in range(8)):
            marks.append('inf_operand')
    elif k == HANDCTOR:
        A.wr(o + 0x2C, 4, a[3])
        W.snap()
        events.append([0x6D0450, tag(0, o + 0x2C), a[3]])
        A.wr(o + 0x2C, 4, a[5])
        ret = tag(0, o)
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif k == NODE:
        A.wr(o, 2, a[3] & 0xFFFF); A.wr(o + 2, 2, a[4] & 0xFFFF); A.wr(o + 4, 2, a[5] & 0xFFFF)
        W.snap()
        words = [a[3] & 0xFFFF, a[4] & 0xFFFF, a[5] & 0xFFFF]
        for i, w in enumerate(words):
            v = w - 0x10000 if w & 0x8000 else w
            B.wr(a[6] + 4 * i, 4, struct.unpack('<I', struct.pack('<f', v * 0.125))[0])
        ret = tag(1, a[6])
        if any(w & 0x8000 for w in words):
            marks.append('neg_word')
        if any(not (w & 0x8000) for w in words):
            marks.append('pos_word')
        if any(w in (0x8000, 0x7FFF, 0xFFFF, 0) for w in words):
            marks.append('extreme_word')
    elif k == TRAINSET:
        A.wrb(o + 0x668, fill_words(a[13] ^ 0x3333, 0x44))
        clump = 0
        if a[4]:
            clump = W.raw(1, a[5])
            B.wr(a[5], 1, 2)
        A.wr(o + 0x18, 4, clump)
        W.snap()
        events.append([0x6D6A40, tag(0, o), a[3]])
        A.wrb(o + 0x668, [0] * 0x44)
        events.append([0x4C5440, W.tagv(clump), tag(0, o + 0x668)])
        A.wr(o + 0x668, 4, a[7])
        marks.append('clump_arena' if a[4] else 'clump_null')
    elif k == TRAINPRE:
        flags, model = a[3] & 0xFFFF, a[4] & 0xFFFF
        matrix = W.raw(1, a[7]) if a[5] else a[6]
        A.wr(o + 0x5B8, 2, flags); A.wr(o + 0x22, 2, model); A.wr(o + 0x14, 4, matrix)
        W.snap()
        events.append([0x6D6480, tag(0, o)])
        if flags & 8:
            events.append([0x6E1A60, tag(0, o), W.tagv(matrix), 5 if model == 449 else 0x25])
            marks.append('front')
            marks.append('front_tram' if model == 449 else 'front_other')
        elif flags & 0x10:
            events.append([0x6E1A60, tag(0, o), W.tagv(matrix), 0x15])
            marks.append('last')
        else:
            marks.append('neither')
    return base_result(W, events, ret, gch, marks)


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        return 'eventos %s != %s' % (o['events'][:3], e['events'][:3])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    img = bytes.fromhex(side_row['post_floating_state'])
    ftw = int.from_bytes(img[8:10], 'little')
    if ftw != 0xFFFF:
        return 'x87 no vacia (ftw %04x)' % ftw
    return None


def check_row(row):
    c = row['input']
    e = expected(c)
    for side in ('original', 'candidate'):
        why = check_side(c, row[side]['observed'], e, row[side])
        if why:
            return side, why
    return None


def main(path):
    bad = checked = 0
    for line in open(path, encoding='utf-8'):
        row = json.loads(line)
        if row.get('event') != 'comparison':
            continue
        r = check_row(row)
        if r:
            bad += 1
            if bad <= 8:
                print('DIFIERE', r[0], row['function'], row['case'], r[1])
        checked += 1
    print('casos comprobados', checked, 'lados distintos del modelo', bad)
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1]))
