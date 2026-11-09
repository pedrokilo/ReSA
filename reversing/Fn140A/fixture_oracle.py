"""Oraculo independiente (Python) de Fn140A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las 21 funciones del lote 140A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion; solo reimplementa el RELLENO pseudoaleatorio de las arenas (Fixtures.hpp) para conocer su imagen inicial.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico) y vtables leidas del propio EXE (ranura = indice del dword en la vtable):
   0  0x4C5620  CBaseModelInfo (cuerpo del destructor)                    mov dword [ecx],85BB9Ch; ret
   1  0x4C5640  CTimeModelInfo (constructor)                              push esi; mov esi,ecx; call 4C4A60; mov [esi],85BCB0h; mov word [esi+22h],0FFFFh; mov eax,esi; pop esi; ret
                4C4A60 (sin hook): mov eax,ecx; mov [eax],85BB9Ch; mov word [eax+8],0; mov word [eax+0Ah],0FFFFh; ret
   2  0x544990  CPhysical (vtable 0x863BA0, ranura 0, destructor con borrado)     push esi; mov esi,ecx; call 542450; test byte [esp+8],1; je +; push esi; call 8214BD; add esp,4; mov eax,esi; pop esi; ret 4
   3  0x54E8E0  trampolin del destructor de CMatrix                       jmp 59ACD0
   4  0x6CF180  CTrailer (vtable 0x871C28, ranura 0)                      igual con call 6CED10 y call 6E2D90
   5  0x6D0450  cTransmission (constructor)                               mov eax,ecx; ret
   6  0x537A50  CAnimatedBuilding (vtable 0x8639B0, ranura 0)             igual con call 537A70 y call 4040A0
   7  0x537A70  CAnimatedBuilding (cuerpo del destructor)                 jmp 535E90
   8  0x41BCB0  CBridge::ThisIsABridgeObjectMovingUp                      xor al,al; ret
   9  0x41BCE0  sin del float de la pila                                  fld dword [esp+4]; fsin; ret
  10  0x41BCF0  cos del float de la pila                                  fld dword [esp+4]; fcos; ret
  11  0x41BD00  CMatrix::GetRight (copia)                                 mov eax,ecx; ret
  12  0x41BD10  CMatrix::GetForward (copia)                               lea eax,[ecx+10h]; ret
  13  0x41BD50  CTimer::GetTimeStep (copia)                               fld dword [0B7CB5Ch]; ret
  14  0x41BD80  CPlaceable::GetMatrix (copia)                             mov eax,[ecx+14h]; ret
  15  0x68B9F0  CTaskComplexArrestPed (vtable 0x8709A8, ranura 4)         mov eax,44Dh; ret
  16  0x6908C0  CTaskComplexPolicePursuit (vtable 0x8709D4, ranura 0)     igual con call 68D880 y call 61A5B0
  17  0x63C9C0  CTaskComplexCarDrive (vtable 0x86E934, ranura 4)          mov eax,2D5h; ret
  18  0x643220  CTaskComplexCarDriveWander (vtable 0x86E96C, ranura 0)    igual con call 63CB50 y call 61A5B0
  19  0x65FB50  CTaskGangHassleVehicle (vtable 0x86F9D4, ranura 4)        mov eax,4B5h; ret
  20  0x6642A0  CTaskGangHasslePed (vtable 0x86FA00, ranura 0)            igual con call 65FF60 y call 61A5B0

En los destructores con borrado el falso del cuerpo deja el dword del caso en [ECX] y devuelve un EAX que no es `this`; el falso de operator delete anota el dword de [argumento] al entrar (= lo que dejo
el cuerpo: prueba que se borra DESPUES de destruir). Solo cuenta el BIT 0 del BYTE bajo de la ranura de pila de banderas.

sin / cos (9 y 10): el resultado queda en ST0 con la precision completa de la x87 (fsin / fcos no dependen del control de precision). Este oraculo NO reimplementa el microcodigo de la FPU: comprueba (a) que
la x87 final tenga solo ST0 valido, (b) los casos especiales EXACTOS (NaN: el mismo NaN silenciado; infinito: indefinido real; |x| >= 2^63: el operando sin cambios) y (c) para el resto que ST0 este a distancia
<= 2^-62 |sin x| + 2^-65 |x| del seno exacto calculado con aritmetica decimal de 140 digitos y pi de 100 digitos. La igualdad bit a bit con el EXE la da la comparacion par a par.
GetTimeStep (13): ST0 = el float cargado con fld, EXACTO (denormales normalizados, NaN senalizante silenciado).

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import functools
import json
import struct
import sys
from decimal import Decimal, getcontext
from fractions import Fraction

M32 = 0xFFFFFFFF
NAMES = ['CBaseModelInfo::DestructorBody', 'CTimeModelInfo::ConstructorBody', 'CPhysical::ScalarDeletingDestructor', 'CMatrix::DestructorThunk', 'CTrailer::ScalarDeletingDestructor',
         'cTransmission::ConstructorBody', 'CAnimatedBuilding::ScalarDeletingDestructor', 'CAnimatedBuilding::DestructorBody', 'CBridge::ThisIsABridgeObjectMovingUp', 'SinFloat', 'CosFloat',
         'CMatrix::GetRightCopy', 'CMatrix::GetForwardCopy', 'CTimer::GetTimeStepCopy', 'CPlaceable::GetMatrixCopy', 'CTaskComplexArrestPed::GetTaskType',
         'CTaskComplexPolicePursuit::ScalarDeletingDestructor', 'CTaskComplexCarDrive::GetTaskType', 'CTaskComplexCarDriveWander::ScalarDeletingDestructor',
         'CTaskGangHassleVehicle::GetTaskType', 'CTaskGangHasslePed::ScalarDeletingDestructor']
ENTRIES = (0x4C5620, 0x4C5640, 0x544990, 0x54E8E0, 0x6CF180, 0x6D0450, 0x537A50, 0x537A70, 0x41BCB0, 0x41BCE0, 0x41BCF0, 0x41BD00, 0x41BD10, 0x41BD50, 0x41BD80,
           0x68B9F0, 0x6908C0, 0x63C9C0, 0x643220, 0x65FB50, 0x6642A0)
GUARD = 0x40
WIN = 0x1000
N = GUARD + WIN + GUARD


def mix(x):
    x &= M32
    x ^= x >> 16
    x = (x * 0x7FEB352D) & M32
    x ^= x >> 15
    x = (x * 0x846CA68B) & M32
    x ^= x >> 16
    return x


def rnd(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rnd(seed, i // 4))
    return bytearray(out[:nbytes])


def fill_kind(nbytes, kind, seed):
    if kind == 0:
        return fill_words(nbytes, seed)
    return bytearray([0x00 if kind == 1 else (0xFF if kind == 2 else 0xCD)]) * nbytes


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
            w = self.words[i >> 2] = rnd(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

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


# ------------------------------------------------------------------------------------------------ tablas leidas del desensamblado
# destructores con borrado: (cuerpo del destructor, operator delete)
SDD = {2: (0x542450, 0x8214BD), 4: (0x6CED10, 0x6E2D90), 6: (0x537A70, 0x4040A0), 16: (0x68D880, 0x61A5B0), 18: (0x63CB50, 0x61A5B0), 20: (0x65FF60, 0x61A5B0)}
# cuerpos de destructor y trampolines: (colaborador, vtable que el propio cuerpo deja o None)
BODIES = {0: (None, 0x85BB9C), 3: (0x59ACD0, None), 7: (0x535E90, None)}
CONSTANT = {15: 0x44D, 17: 0x2D5, 19: 0x4B5}
THIS_FNS = (0, 1, 2, 3, 4, 5, 6, 7, 11, 12, 14, 15, 16, 17, 18, 19, 20)
SPAN = {1: 0x24, 14: 0x18}
FLOAT_FNS = (9, 10, 13)


def ptr_mark(prefix, v):
    return prefix + ('_zero' if v == 0 else '_ffff' if v == M32 else '_exe' if 0x400000 <= v < 0xC00000 else '_other')


# ------------------------------------------------------------------------------------------------ float: sin / cos y fld
def f32_fields(bits):
    return bits >> 31, (bits >> 23) & 0xFF, bits & 0x7FFFFF


def f32_to_x80(bits):
    """lo que deja `fld dword`: (signo, exponente de 15 bits, mantisa de 64 bits); NaN senalizante silenciado, denormal normalizado"""
    s, e, f = f32_fields(bits)
    if e == 255:
        if f == 0:
            return s, 0x7FFF, 1 << 63
        return s, 0x7FFF, (1 << 63) | (1 << 62) | (f << 40)
    if e == 0:
        if f == 0:
            return s, 0, 0
        shift = 24 - f.bit_length()          # normalizar: el bit mas alto de f pasa a ser el entero explicito
        mant = (f << shift) << 40
        return s, 16383 - 126 - shift, mant
    return s, e - 127 + 16383, (1 << 63) | (f << 40)


def x80_bytes(t):
    s, e, m = t
    return struct.pack('<QH', m, (s << 15) | e)


def x80_value(raw):
    """valor exacto (Fraction) de un registro de 10 bytes, o ('nan'|'inf', signo) si no es finito"""
    m, se = struct.unpack('<QH', raw)
    s, e = se >> 15, se & 0x7FFF
    if e == 0x7FFF:
        return ('inf' if (m << 1) & ((1 << 64) - 1) == 0 else 'nan', s)
    exp = (1 if e == 0 else e) - 16383 - 63
    v = Fraction(m) * (Fraction(2) ** exp)
    return -v if s else v


getcontext().prec = 140
PI = Decimal('3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679')
HALF_PI = PI / 2


def dec_sin_cos_small(r):
    """sin y cos de |r| <= pi/4 por Taylor"""
    sin = term = r
    r2 = r * r
    n = 1
    while abs(term) > Decimal(10) ** -135:
        term = -term * r2 / ((n + 1) * (n + 2))
        n += 2
        sin += term
    cos = term = Decimal(1)
    n = 0
    while abs(term) > Decimal(10) ** -135:
        term = -term * r2 / ((n + 1) * (n + 2))
        n += 2
        cos += term
    return sin, cos


@functools.lru_cache(maxsize=None)
def true_sin_cos(bits):
    s, e, f = f32_fields(bits)
    x = Decimal(f if e == 0 else (f | (1 << 23))) * (Decimal(2) ** ((1 if e == 0 else e) - 150))
    if s:
        x = -x
    k = int((x / HALF_PI).to_integral_value())
    r = x - k * HALF_PI
    sn, cs = dec_sin_cos_small(r)
    q = k % 4
    sin = (sn, cs, -sn, -cs)[q]
    cos = (cs, -sn, -cs, sn)[q]
    return Fraction(sin), Fraction(cos), Fraction(x)


def float_expectation(fn, bits):
    """('exact', registro de 10 bytes) o ('approx', verdadero, tolerancia, descripcion)"""
    s, e, f = f32_fields(bits)
    if fn == 13:
        return ('exact', x80_bytes(f32_to_x80(bits)))
    if e == 255:
        if f == 0:
            return ('exact', x80_bytes((1, 0x7FFF, 0xC000000000000000)))   # sin / cos de infinito: indefinido real
        return ('exact', x80_bytes(f32_to_x80(bits)))                       # NaN: el mismo, silenciado
    if e >= 127 + 63:
        return ('exact', x80_bytes(f32_to_x80(bits)))                       # |x| >= 2^63: fsin / fcos dejan el operando (C2 = 1)
    sin, cos, x = true_sin_cos(bits)
    true = sin if fn == 9 else cos
    tol = abs(true) * Fraction(1, 1 << 62) + abs(x) * Fraction(1, 1 << 65) + Fraction(1, 1 << 16000)
    return ('approx', true, tol, 'x')


def float_class(bits):
    s, e, f = f32_fields(bits)
    if e == 255:
        return 'cls_inf' if f == 0 else ('cls_snan' if not f & 0x400000 else 'cls_qnan')
    if e == 0:
        return 'cls_zero' if f == 0 else 'cls_denorm'
    if e >= 190:
        return 'cls_huge'
    return 'cls_normal'


def common_marks(fn, a):
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    if fn in THIS_FNS:
        o = a[0]
        span = SPAN.get(fn, 4)
        marks.append('this_unaligned' if o % 4 else 'this_aligned')
        if o + span > WIN:
            marks.append('spill')
    return marks


KEYS = {}
for _fn in range(21):
    _k = ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3']
    if _fn in THIS_FNS:
        _k += ['this_aligned', 'this_unaligned', 'spill']
    if _fn in SDD:
        _k += ['flag_bit0_set', 'flag_bit0_clear', 'flag_high_bits', 'flag_byte_only', 'prev_zero', 'prev_other']
    elif _fn in BODIES:
        _k += ['prev_zero', 'prev_other']
    elif _fn == 14:
        _k += ['src_zero', 'src_ffff', 'src_exe', 'src_other']
    elif _fn == 8:
        _k += ['arg_zero', 'arg_ffff', 'arg_exe', 'arg_other']
    elif _fn in FLOAT_FNS:
        _k += ['cat%d' % i for i in range(16)] + ['cls_zero', 'cls_denorm', 'cls_normal', 'cls_huge', 'cls_inf', 'cls_qnan', 'cls_snan']
    KEYS[_fn] = _k


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    marks = common_marks(fn, a)
    A, B, C = Mem(a[2], a[12]), Mem(a[2], a[13]), Mem(a[2], a[14])
    events, ret, st0 = [], None, None
    o = a[0]
    if fn in SDD:
        body, delete = SDD[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        events.append([body, tag(0, o), a[3]])
        A.wr(o, 4, a[5])                                    # el falso del cuerpo deja su dword en [this]
        flags = a[6] & 0xFF                                 # solo el byte bajo de la ranura de pila
        if flags & 1:
            events.append([delete, tag(0, o), a[5]])        # operator delete ve lo que dejo el cuerpo
        ret = tag(0, o)
        marks.append('flag_bit0_set' if flags & 1 else 'flag_bit0_clear')
        if a[6] >> 8:
            marks.append('flag_high_bits')
        else:
            marks.append('flag_byte_only')
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn in BODIES:
        collab, vt = BODIES[fn]
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        if vt is not None:
            A.wr(o, 4, vt)
        else:
            events.append([collab, tag(0, o), a[3]])
            A.wr(o, 4, a[5])
        marks.append('prev_zero' if a[3] == 0 else 'prev_other')
    elif fn == 1:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        A.wr(o, 4, 0x85BCB0)
        A.wr(o + 8, 2, 0)
        A.wr(o + 0xA, 2, 0xFFFF)
        A.wr(o + 0x22, 2, 0xFFFF)
        ret = tag(0, o)
    elif fn == 5:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o)
    elif fn == 11:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o)
    elif fn == 12:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = tag(0, o + 0x10)
    elif fn == 14:
        A.wr(o + 0x14, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = a[3]
        marks.append(ptr_mark('src', a[3]))
    elif fn in CONSTANT:
        A.wr(o, 4, a[3])
        A.snap(); B.snap(); C.snap()
        ret = CONSTANT[fn]
    elif fn == 8:
        A.snap(); B.snap(); C.snap()
        ret = 0
        marks.append(ptr_mark('arg', a[3]))
    else:   # 9, 10, 13: float en ST0
        A.snap(); B.snap(); C.snap()
        st0 = float_expectation(fn, a[3])
        marks.append('cat%d' % c['family'])
        marks.append(float_class(a[3]))
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': [], 'ret': ret, 'st0': st0, 'marks': marks}


def check_st0(fn, exp, img):
    ftw = int.from_bytes(img[8:10], 'little')
    fsw = int.from_bytes(img[4:6], 'little')
    top = (fsw >> 11) & 7
    tags = [(ftw >> (2 * i)) & 3 for i in range(8)]
    if tags[top] == 3 or any(t != 3 for i, t in enumerate(tags) if i != top):
        return 'x87: debe haber solo ST0 valido (ftw %04x top %d)' % (ftw, top)
    raw = bytes(img[28:38])
    if exp[0] == 'exact':
        if raw != exp[1]:
            return 'ST0 %s != %s' % (raw.hex(), exp[1].hex())
        return None
    _, true, tol, _ = exp
    v = x80_value(raw)
    if isinstance(v, tuple):
        return 'ST0 no finito %s' % raw.hex()
    if abs(v - true) > tol:
        return 'ST0 fuera de tolerancia: %s vs %s (error %.3g, tol %.3g)' % (float(v), float(true), float(abs(v - true)), float(tol))
    return None


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
    if e['st0'] is not None:
        return check_st0(c['fn'], e['st0'], img)
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
