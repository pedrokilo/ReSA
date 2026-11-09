"""Oraculo independiente (Python) de Fn135A: reproduce, desde la ENTRADA del caso y la lectura del desensamblado del EXE, lo que debe observarse de las funciones del lote 135A y lo compara con las
filas de comparisons.jsonl (lado original y candidato). No usa ninguna constante ni rama del C++ de produccion. La aritmetica x87 (24 bits de mantisa, exponente ampliado) se modela con fracciones exactas.

Desensamblado del EXE (dumpbin /disasm sobre el gta-sa.exe canonico):
  0  0x53F2D0  GetMouseSetUp (cdecl, destino del CMouseControllerState por la pila, `ret 4`, EAX = destino)
       estado local de 20 bytes: todos a cero salvo el byte de relleno +7, sin inicializar (0 en el banco). Si [psGlobal+0x1C] (diMouse) es nulo: argumento = 1 si el byte del menu (0xBA67A4) es 0 y
       0x745CA0 (IsVideoModeExclusive) devuelve AL != 0 (si no 0; el byte +0 de la pila ANTES del estado, fuera de el); diMouseInit(dword) (0x7469A0, cdecl) y se vuelve a leer diMouse; si sigue nulo se copia el estado.
       Con raton: DIDEVCAPS (dwSize = 0x2C) y GetCapabilities (ranura 3, stdcall) SIN mirar el HRESULT; dwButtons - 1 sin signo <= 7: 1 -> izquierdo; 2 -> izquierdo y derecho; 3..8 -> los tres (+0, +1, +2);
       dwAxes == 3 (exacto): +3 y +4 a uno. Los botones X (+5, +6) nunca se marcan. Basura de pila = 0 en el banco (pila sembrada con ceros).
  1  0x747300  getDvdGamePath (cdecl, sin argumentos, `ret`; EAX = ruta o 0)
       SetErrorMode(1); n = GetLogicalDriveStringsA(0, 0); buf = operator new[](n) (0x821195); n2 = GetLogicalDriveStringsA(n, buf); para i en [0, n2) (sin signo): copia buf[i] a un buffer local; con un
       nulo: GetDriveTypeA(buffer) y, si == 5, GetVolumeInformationA(buffer, nombre, 0xFF, 0, 0, 0, 0, 0); si da distinto de cero y los 16 primeros bytes del nombre son "GTA_SAN_ANDREAS" + NUL: resultado =
       operator new[](lstrlenA(buffer) + 0x21) con lstrcpyA(resultado, buffer) y fin del recorrido; el buffer se reinicia tras cada nulo; al final operator delete[](buf) (0x8213AE).
  2  0x50D590  CCollision / GetNearestDistanceOfPedSphereToCameraNearClip (cdecl, peaton, `ret`, ST0)
       modelo = ms_modelInfoPtrs[movsx word [ped+0x22]] (0xA9B0C8); 0x4C7170(modelo, clump [ped+0x18]) (thiscall); esferas = [[[modelo+0x34]+0x2C]+8]; offset = ((fz*sz) + (fy*sy)) + (fx*sx) (camara activa
       0xB6F19C + [0xB6F081]*0x238, front +0x190, source +0x19C) guardado como float; plano cercano = dword [[0xC1703C]+0x80] (float); minimo inicial 999999.0f; para las 12 esferas (paso 0x14): dot = (cz*fz + cx*fx)
       + cy*fy (esferas 0 y 1 de cada bloque de 6) o (cz*fz + cy*fy) + cx*fx; val = ((dot - offset) - radio) - plano SIN guardar; si val < minimo (ordenado) minimo = val guardado como float (fst) y releido.

Uso:  python -B fixture_oracle.py <comparisons.jsonl>      (sale con codigo 1 si algun caso difiere del modelo)
"""
import struct
import json
import sys
from fractions import Fraction
from math import isqrt

NAN = ('n',)


def load(bits):
    e = (bits >> 23) & 0xFF
    m = bits & 0x7FFFFF
    neg = bits >> 31
    if e == 0xFF:
        return NAN if m else ('i', neg)
    if e == 0:
        v = Fraction(m, 1) * Fraction(1, 1 << 149)
    else:
        v = Fraction(m | 0x800000, 1) * Fraction(2) ** (e - 150)
    return ('f', -v if neg else v, neg)


def rnd(v, quantum_exp_min=None):
    if v == 0:
        return v
    a = abs(v)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    elif Fraction(2) ** (e + 1) <= a:
        e += 1
    q = e - 23
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


def fin(v, neg):
    return ('f', rnd(v), neg if v == 0 else (1 if v < 0 else 0))


def mul(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' or b[0] == 'i':
        za = a[0] == 'f' and a[1] == 0
        zb = b[0] == 'f' and b[1] == 0
        if za or zb:
            return NAN
        sa = a[1] if a[0] == 'i' else a[2]
        sb = b[1] if b[0] == 'i' else b[2]
        return ('i', sa ^ sb)
    return fin(a[1] * b[1], a[2] ^ b[2])


def add(a, b):
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i' and b[0] == 'i':
        return a if a[1] == b[1] else NAN
    if a[0] == 'i':
        return a
    if b[0] == 'i':
        return b
    s = a[1] + b[1]
    if s == 0:
        return ('f', Fraction(0), 1 if (a[2] and b[2] and a[1] == 0 and b[1] == 0) else 0)
    return fin(s, 0)


def neg(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return ('i', 1 - a[1])
    return ('f', -a[1], 1 - a[2])


def sub(a, b):
    return add(a, neg(b))


def sqrt24(a):
    if a == NAN:
        return NAN
    if a[0] == 'i':
        return a if a[1] == 0 else NAN
    v = a[1]
    if v == 0:
        return a
    if v < 0:
        return NAN
    s = max(0, (200 - (v.numerator.bit_length() - v.denominator.bit_length())) // 2 + 1)
    scaled = v * Fraction(4) ** s
    n = scaled.numerator // scaled.denominator
    exact = scaled == n
    r = isqrt(n)
    sticky = not (exact and r * r == n)
    approx = Fraction(2 * r + (1 if sticky else 0), 1) / Fraction(2) ** (s + 1)
    return ('f', rnd(approx), 0)


def store(a):
    """fstp dword: devuelve los bits del float, o None si es NaN"""
    if a == NAN:
        return None
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


def reload(bits):
    return NAN if bits is None else load(bits)


BIG = Fraction(10) ** 400


def num(a):
    if a[0] == 'i':
        return BIG if a[1] == 0 else -BIG
    return a[1]


def gt(a, b):
    """a > b ordenado (fcom + test ah,41h / jne): falso con NaN"""
    if a == NAN or b == NAN:
        return False
    return num(a) > num(b)


def is_zero(a):
    """fcomp 0.0 + test ah,44h / jp: cero solo si es igual (ordenado)"""
    return a != NAN and a[0] == 'f' and a[1] == 0


def div(a, b):
    """a / b a 24 bits con exponente ampliado (b nunca es cero aqui)"""
    if a == NAN or b == NAN:
        return NAN
    if a[0] == 'i':
        return NAN if b[0] == 'i' else a
    if b[0] == 'i':
        sa = a[2] if a[1] == 0 else (1 if a[1] < 0 else 0)
        return ('f', Fraction(0), sa ^ b[1])
    return fin(a[1] / b[1], a[2] ^ b[2])


NAN_BITS = 0xFFC00000


def bits(a):
    """fstp dword: bits del float (el unico NaN que producen los casos es el indefinido 0xFFC00000)"""
    b = store(a)
    return NAN_BITS if b is None else b


def b4(v):
    return [(v >> (8 * j)) & 0xFF for j in range(4)]




STACK = 0x7F000000
GUARD = 0x40
A_TASK, A_PED, A_PMAT, A_OTHER, A_OMAT, A_OBJ, A_ROUTE = range(7)
M32 = 0xFFFFFFFF
DEFAULT_MARGIN = 0x3EB33333   # valor inicial de 0x8D22B0 (0.35f) en las funciones que no lo tocan


def tag(arena, off):
    return 0x10000000 + (arena << 20) + GUARD + off


def lt(a, b):
    return gt(b, a)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(x):
    """valor tras guardarlo como float (fstp dword) y volver a cargarlo"""
    return reload(store(x))


def ext80(a):
    """codificacion de 80 bits (10 bytes, little endian) de un valor del modelo; NaN = indefinido"""
    if a == NAN:
        return (0xC000000000000000).to_bytes(8, 'little') + (0xFFFF).to_bytes(2, 'little')
    if a[0] == 'i':
        return (0x8000000000000000).to_bytes(8, 'little') + ((a[1] << 15) | 0x7FFF).to_bytes(2, 'little')
    v, neg = a[1], a[2]
    if v == 0:
        return (0).to_bytes(8, 'little') + ((neg << 15)).to_bytes(2, 'little')
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



# ------------------------------------------------------------------------------------------------ PRNG y memoria de las arenas (identicos a Fixtures.hpp)
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


def prng(c, k):
    return mix((c * 0x9E3779B1 + k * 0x85EBCA6B + 100001) & M32)


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', prng(seed, i // 4))
    return bytearray(out[:nbytes])


def fnv(data):
    h = 2166136261
    for b in data:
        h = ((h ^ b) * 16777619) & M32
    return h


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
            w = self.words[i >> 2] = prng(self.seed, i >> 2)
        return (w >> (8 * (i & 3))) & 0xFF

    def get(self, i):
        v = self.m.get(i)
        return self.base(i) if v is None else v

    def rd(self, off, n):
        return sum(self.get(GUARD + off + k) << (8 * k) for k in range(n))

    def wr(self, off, n, v):
        for k in range(n):
            self.m[GUARD + off + k] = (v >> (8 * k)) & 0xFF

    def put(self, off, data):
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


NAMES = ['GetMouseSetUp', 'getDvdGamePath', 'CCollision::GetNearestDistanceOfPedSphereToCameraNearClip']
ENTRIES = (0x53F2D0, 0x747300, 0x50D590)
INITIAL_MIN = 0x497423F0
VOL_NAMES = [b'GTA_SAN_ANDREAS\0', b'GTA_SAN_ANDREAS\0JUNKJUNK', b'GTA_SAN_ANDREA\0', b'GTA_SAN_ANDREASX\0', b'gta_san_andreas\0', b'\0', b'GTA_SAN_ANDREAS \0', b'GTA\0']

KEYS = {
    0: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'mouse_present', 'mouse_absent', 'init_excl', 'init_noexcl', 'init_menu', 'init_created', 'init_failed', 'no_device_return',
        'btn0', 'btn1', 'btn2', 'btn3_8', 'btn9plus', 'axes3', 'axes_other', 'hr_fail'],
    1: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'found', 'not_found', 'cdrom', 'noncdrom', 'vol_fail', 'name_mismatch', 'trunc', 'final_nul', 'n2_zero', 'type_hi', 'second_match', 'junk_after_nul',
        'size_zero', 'many_drives', 'empty_string'],
    2: ['kind0', 'kind1', 'kind2', 'kind3', 'fill0', 'fill1', 'fill2', 'fill3', 'init_kept', 'updated', 'tie', 'nan_seen', 'inf_final', 'store_overflow', 'cam0', 'cam1', 'cam2', 'idx_neg', 'from_sphere0',
        'from_sphere11', 'from_group2', 'from_group1', 'huge_exponents'],
}


def bits_of(a):
    return a


def fbits(b4_):
    return struct.unpack('<I', bytes(b4_))[0]


def is_nan_bits(b):
    return (b & 0x7F800000) == 0x7F800000 and (b & 0x7FFFFF) != 0


def expected(c):
    a = [x & M32 for x in c['a']]
    fn = c['fn']
    s = bytes.fromhex(c['s'])
    t = bytes.fromhex(c['t'])
    u = bytes.fromhex(c['u'])
    kind, fill = a[1], a[2]
    marks = ['kind%d' % kind, 'fill%d' % fill]
    A, B, C = Mem(fill, a[12]), Mem(fill, a[13]), Mem(fill, a[14])
    events, gch, ret, extra, st0 = [], [], None, None, None

    if fn == 0:
        # GetMouseSetUp (0x53F2D0): ver el desensamblado en la cabecera
        A.snap(); B.snap(); C.snap()
        dev = tag(2, 0x40 + 4 * (prng(a[14], 1) % 0x100))
        state = bytearray(20)
        menu, al = a[4] & 0xFF, a[5] & 0xFF
        present = a[3] != 0
        marks.append('mouse_present' if present else 'mouse_absent')
        if not present:
            exclusive = False
            if menu == 0:
                events.append([0x745CA0])
                exclusive = al != 0
                marks.append('init_excl' if exclusive else 'init_noexcl')
            else:
                marks.append('init_menu')
            events.append([0x7469A0, 1 if exclusive else 0, 0])
            present = a[6] != 0
            marks.append('init_created' if present else 'init_failed')
        extra = [dev if present else 0]
        if present:
            events.append([0xD1C0, dev, 0x2C, fnv(bytes(40))])
            b = a[7]
            if 3 <= b <= 8:
                state[2] = 1
            if 2 <= b <= 8:
                state[1] = 1
            if 1 <= b <= 8:
                state[0] = 1
            marks.append('btn0' if b == 0 else 'btn1' if b == 1 else 'btn2' if b == 2 else 'btn3_8' if b <= 8 else 'btn9plus')
            if a[8] == 3:
                state[3] = 1
                state[4] = 1
                marks.append('axes3')
            else:
                marks.append('axes_other')
            if a[9] != 0:
                marks.append('hr_fail')
        else:
            marks.append('no_device_return')
        A.put(a[0], bytes(state))
        ret = tag(0, a[0])
    elif fn == 1:
        A.snap(); B.snap(); C.snap()
        n, n2 = a[0], a[3]
        events = [[0x85813C, 1], [0x858138, 0, 0], [0x821195, n], [0x858138, n, tag(1, a[6])]]
        B.put(a[6], s)
        path = b''
        ti = vi = 0
        result = None
        L = len(s)
        found_at = None
        cdroms = 0
        for i in range(n2):
            ch = s[i]
            path += bytes([ch])
            if ch:
                continue
            p = path[:-1]
            path = b''
            k = ti
            ti += 1
            events.append([0x858134, fnv(p), len(p)])
            ty = (t[k % 16] | (a[8] if k & 1 else 0)) & M32
            if t[k % 16] == 5 and a[8] and k & 1:
                marks.append('type_hi')
            if ty != 5:
                marks.append('noncdrom')
                continue
            marks.append('cdrom')
            cdroms += 1
            vk = vi
            vi += 1
            events.append([0x858130, fnv(p), len(p), 1, 0xFF, 0, 0, 0, 0, 0])
            if t[16 + vk % 16] == 0:
                marks.append('vol_fail')
                continue
            sel = t[32 + vk % 16] % 8
            name = VOL_NAMES[sel]
            if name[:16] == VOL_NAMES[0]:
                events.append([0x821195, len(p) + 0x21])
                C.put(a[7], p + b'\0')
                result = tag(2, a[7])
                marks.append('found')
                if cdroms > 1:
                    marks.append('second_match')
                if sel == 1:
                    marks.append('junk_after_nul')
                break
            marks.append('name_mismatch')
        if result is None:
            marks.append('not_found')
        events.append([0x8213AE, tag(1, a[6]), B.rd(a[6], 4)])
        if n2 < L - 1:
            marks.append('trunc')
        if n2 == L:
            marks.append('final_nul')
        if n2 == 0:
            marks.append('n2_zero')
        if n == 0:
            marks.append('size_zero')
        if s.count(0) > 8:
            marks.append('many_drives')
        if b'\0\0' in s[:L - 1]:
            marks.append('empty_string')
        ret = result if result is not None else 0
    else:
        # CCollision / GetNearestDistanceOfPedSphereToCameraNearClip (0x50D590)
        sph = a[7]
        for i in range(12):
            C.put(sph + 0x14 * i, s[16 * i:16 * i + 16])   # esferas previas (las escribe la preparacion del caso)
        A.snap(); B.snap(); C.snap()
        events = [[0x4C7170, tag(1, a[4]), 0x00ABC000 + (a[12] & 0xFFF0)]]
        for i in range(12):
            C.put(sph + 0x14 * i, u[16 * i:16 * i + 16])
        f = [load(fbits(t[4 * k:4 * k + 4])) for k in range(6)]
        fx, fy, fz, sx, sy, sz = f
        off = fl(add(add(mul(fz, sz), mul(fy, sy)), mul(fx, sx)))
        clip = load(a[10])
        minv = load(INITIAL_MIN)
        nan_seen = tie = False
        src = None
        for i in range(12):
            cx, cy, cz, r = [load(fbits(u[16 * i + 4 * k:16 * i + 4 * k + 4])) for k in range(4)]
            if i % 6 < 2:
                dot = add(add(mul(cz, fz), mul(cx, fx)), mul(cy, fy))
            else:
                dot = add(add(mul(cz, fz), mul(cy, fy)), mul(cx, fx))
            val = sub(sub(sub(dot, off), r), clip)
            if val == NAN:
                nan_seen = True
            elif minv != NAN and num(val) == num(minv) and not gt(minv, val):
                tie = True
            if lt(val, minv):
                stored = store(val)
                if stored is not None and (stored & 0x7F800000) == 0x7F800000 and val[0] == 'f':
                    marks.append('store_overflow')
                minv = reload(stored)
                src = i
        st0 = minv
        marks.append('init_kept' if src is None else 'updated')
        if tie:
            marks.append('tie')
        if nan_seen:
            marks.append('nan_seen')
        if minv != NAN and minv[0] == 'i':
            marks.append('inf_final')
        marks.append('cam%d' % a[8])
        if (a[3] & 0xFFFF) >= 0x8000:
            marks.append('idx_neg')
        if src == 0:
            marks.append('from_sphere0')
        if src == 11:
            marks.append('from_sphere11')
        if src is not None:
            marks.append('from_group1' if src % 6 < 2 else 'from_group2')
        if any(((fbits(u[4 * k:4 * k + 4]) >> 23) & 0xFF) >= 0xB0 and ((fbits(u[4 * k:4 * k + 4]) >> 23) & 0xFF) < 0xFF for k in range(48)):
            marks.append('huge_exponents')
    return {'events': events, 'changes': {'A': A.changes(), 'B': B.changes(), 'C': C.changes()}, 'gchanges': gch, 'ret': ret, 'extra': extra, 'st0': st0, 'marks': marks}


def check_side(c, o, e, side_row):
    if o.get('changes') != e['changes']:
        return 'changes: %s != %s' % (str(o.get('changes'))[:300], str(e['changes'])[:300])
    if o.get('gchanges') != e['gchanges']:
        return 'gchanges: %s != %s' % (str(o.get('gchanges'))[:300], str(e['gchanges'])[:300])
    if o['events'] != e['events']:
        n = next((i for i in range(min(len(o['events']), len(e['events']))) if o['events'][i] != e['events'][i]), min(len(o['events']), len(e['events'])))
        return 'eventos (%d vs %d), primero distinto %d: %s != %s' % (len(o['events']), len(e['events']), n, o['events'][n:n + 1], e['events'][n:n + 1])
    if o.get('ret') != e['ret']:
        return 'ret %s != %s' % (o.get('ret'), e['ret'])
    if o.get('extra') != e['extra']:
        return 'extra %s != %s' % (o.get('extra'), e['extra'])
    top, valid = fp_stack(side_row['post_floating_state'])
    if e['st0'] is None:
        if valid:
            return 'x87 no vacia: %s' % [(st, v.hex()) for st, v in valid]
    else:
        want = ext80(e['st0'])
        if len(valid) != 1 or valid[0][0] != 0 or valid[0][1] != want:
            return 'ST0 %s != %s' % ([(st, v.hex()) for st, v in valid], want.hex())
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
