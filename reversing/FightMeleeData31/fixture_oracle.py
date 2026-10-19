"""Oraculo independiente del banco FightMeleeData31: CTaskSimpleFight::LoadMeleeData (0x5BEDC0).

Se modela desde el DESENSAMBLADO del EXE (no desde el C++ de la DLL) a partir de la fila `input` que registra el banco (lineas del fichero, nombres de grupo, contador de grupos y semilla
del patron previo de la region de memoria).

0x5BEDC0 (cdecl sin argumentos, void; guarda EBX/EBP/ESI/EDI; frame de 0x80 bytes con los escalares de sscanf, que persisten entre lineas):
  5BEDC0  14 filas de 0x88 bytes desde 0xC170D0 mientras (fila + 4) < 0xC177BC:  [fila+0]=0x21 ; [+4]=1.5f ; para i<5: [+0x08+4i]=100.0f ; [+0x1C+4i]=100.0f ; [+0x30+4i]=1.0f ; byte [+0x50+i]=7 ;
          byte [+0x55+i]=0 ; [+0x5C+4i]=0 ; [+0x70+4i]=0 ; luego [+0x44]=0 ; [+0x48]=100.0f ; [+0x4C]=100.0f ; word [+0x84]=0.   (las filas 12 y 13 pisan el principio de la tabla de niveles)
  5BEE38  siete vectores de niveles en 0xC177D0 (12 bytes cada uno): x=0, y=0.75f (0x3F400000), z=0 ; los tres floats locales de START_LEVELS valen (0, 0.75f, 0) (residuos de pila)
  5BEF47  file = CFileMgr::OpenFile("DATA\\melee.dat", "rb") (0x538900; push modo, push nombre) ; ebx=0 (contador) ; ebp=0 (desplazamiento de fila) ; flags [12h]=combo, [13h]=niveles = 0
  5BEF63  line = CFileLoader::LoadLine(file) (0x536F80) ; si es nulo salta al cierre. Bucle:
  5BEF80    primer byte '#' o NUL -> siguiente linea
            strncmp(line, "END_MELEE_DATA", 14) == 0 -> cierre (CFileMgr::CloseFile 0x5389D0)
            si ninguna bandera: "START_COMBO" (11) pone la de combo ; si no, "START_LEVELS" (12) pone la de niveles ; siguiente linea (el resto se ignora)
            "END_COMBO" (9): si bandera de combo, ebp += 0x88 ; ebx = 0 ; banderas a 0 ; siguiente linea
            bandera de niveles: sscanf(line, "%s %f %f %f", word, &x, &y, &z) (x,y,z persisten) ; vector[ebx] (0xC177D0 + 12*ebx) = (x,y,z) ; ebx++ (SIN limite)
            si no: ebx++ ; indice = ebx-1 ; si indice > 8 se ignora ; salto por tabla (5BF2F0):
              0  sscanf(line, "%s %s", word, word2) ; recorre i = 0.. mientras i < [0xB4EA28] (CON SIGNO; si es <= 0 no entra): name = GetAnimGroupName(i) (0x4D3A20) ; si strcmp(word2,name)==0 ->
                 [0xC170D0+ebp] = i y termina
              1  sscanf(line, "%s %f", word, &raw28) ; [0xC170D4+ebp] = raw28 (copia en crudo)
              2..6 (golpe; mov = indice-2): f1c = 0 ; sscanf(line, "%s %f %f %f %s %d %d %d %f", word, &f20, &f24, &raw2c, word2, &i30, &i34, &i38, &f1c) ;
                 [0xC170D8 + ebp + 4*mov] = float(f20 * (1/30)) ; [0xC17100+...] = raw2c ; [0xC170EC+...] = float(f24 * (1/30)) ; byte [0xC17120+ebp+mov] = GetHitLevel(word2) (0x5BD360) ;
                 byte [0xC17125+ebp+mov] = (byte)i30 ; [0xC1712C+...] = movsx16(GetHitSound(i34)) (0x5BD3B0) ; [0xC17140+...] = movsx16(GetHitSound(i38)) ; si f1c > 0 : [0xC17114+ebp] = float(f1c * (1/30))
              7  sscanf(line, "%s %f %f", word, &f20, &f24) ; [0xC17118+ebp] = float(f20 * (1/30)) ; [0xC1711C+ebp] = float(f24 * (1/30))
              8  sscanf(line, "%s %x", word, &x3c) ; word [0xC17154+ebp] = (word)x3c
  5BF2DA  CFileMgr::CloseFile(file)
GetHitLevel (5BD360): primer caracter 'H' -> 0, 'L' -> 1, 'G' -> 2, 'B' -> 3, otro -> 7 (las comparaciones de 16 bits no se pueden cumplir). GetHitSound (5BD3B0): n-1 sin signo > 7 -> 0x3E ; si no 0x3C + n.
Los escalares de sscanf (f20, f24, raw2c, i30, i34, i38, raw28, x3c, word2) conservan el valor de la lectura anterior si sscanf no llega a asignarlos; ANTES de la primera lectura contienen basura
de pila: el oraculo lanza Fail si una fixture dependiera de ellos (el generador de Fixtures.hpp lo evita).
Modelo de sscanf: ver FileLoaderLoad2dEffect27/fixture_oracle.py (el sscanf del EXE se usa en ambos lados; el oraculo solo confia en enteros <= 12 cifras en rango int32, reales de <= 9 cifras
significativas en [1e-30, 1e30] con redondeo correcto, cadenas sin blancos y hexadecimales de <= 8 cifras).

Eventos de los falsos (u32 x3, FNV-1a de 64 bits sobre la secuencia little-endian): (1, fnv32(ruta), fnv32(modo)) ; (2, indice de la linea o 0xFFFFFFFF al terminar, id del fichero) ;
(3, id del fichero, 0) ; (4, indice de grupo, 0).
"""
import re
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
CASES = 4608
NAME = 'CTaskSimpleFight::LoadMeleeData'
NAMES = [NAME]
CASES_BY_NAME = {NAME: CASES}
REGION_BASE = 0xC17090
REGION_OFF = 0x40
REGION_BYTES = 0x830
REGION = 0x8B0
TABLE = 0xC170D0
LEVELS = 0xC177D0
FILE_ID = 0x00ABC123
K = 0x3D088889                        # 1/30 como float
MAX_GROUPS = 6
SIZES = {'region_off': 0x40, 'region_bytes': 0x830, 'region': 0x8B0, 'max_groups': 6, 'max_events': 1024}


class Fail(Exception):
    """Fixture que depende de un valor sin inicializar del EXE o de comportamiento no modelado."""


# ---------------------------------------------------------------------------------------------------- utilidades numericas
def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(bits):
    return struct.unpack('<f', struct.pack('<I', bits & M32))[0]


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def mulk(bits):
    """float(x * (1/30)) con la x87 a 24 bits: producto exacto en doble y un unico redondeo a float (x finito y moderado)."""
    x = fl(bits)
    if x != x or x in (float('inf'), float('-inf')):
        raise Fail('NaN/inf no modelado')
    return f32bits(x * fl(K))


def round_f32(fr):
    """Fraction -> bits de float32 con redondeo al par; desborda a inf."""
    if fr == 0:
        return 0
    sign = 0x80000000 if fr < 0 else 0
    a = abs(fr)
    e = a.numerator.bit_length() - a.denominator.bit_length()
    if Fraction(2) ** e > a:
        e -= 1
    e = max(e, -126)
    scaled = a / (Fraction(2) ** (e - 23))
    q = scaled.numerator // scaled.denominator
    rem = scaled - q
    if rem > Fraction(1, 2) or (rem == Fraction(1, 2) and q % 2 == 1):
        q += 1
    if q >= 1 << 24:
        q >>= 1
        e += 1
    if e > 127:
        return sign | 0x7F800000
    if q < 1 << 23:
        return sign | q
    return sign | ((e + 127) << 23) | (q - (1 << 23))


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


def fill_words(nbytes, seed):
    out = bytearray()
    for i in range(0, nbytes, 4):
        out += struct.pack('<I', rand(seed, i // 4))
    return bytes(out[:nbytes])


def fnv32(text):
    h = 2166136261
    for c in text.encode('latin-1'):
        h = ((h ^ c) * 16777619) & M32
    return h


def fnv64(data):
    h = 1469598103934665603
    for b in data:
        h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


# ---------------------------------------------------------------------------------------------------- sscanf
WS = ' \t\n\v\f\r'
INT_RE = re.compile(r'[+-]?\d+')
FLOAT_RE = re.compile(r'[+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?')
HEX_RE = re.compile(r'[+-]?[0-9a-fA-F]+')


def scan(text, kinds):
    """Devuelve una lista del mismo largo que `kinds` con el valor leido o None (no asignado)."""
    i, out, stopped = 0, [], False
    for k in kinds:
        if stopped:
            out.append(None)
            continue
        while i < len(text) and text[i] in WS:
            i += 1
        if i >= len(text):
            stopped = True
            out.append(None)
            continue
        if k == 's':
            j = i
            while j < len(text) and text[j] not in WS:
                j += 1
            out.append(text[i:j])
            i = j
        elif k == 'd':
            m = INT_RE.match(text, i)
            if not m:
                stopped = True
                out.append(None)
                continue
            tok = m.group(0)
            val = int(tok)
            if len(tok.lstrip('+-')) > 12 or not -(1 << 31) <= val < (1 << 31):
                raise Fail('entero sensible al CRT')
            out.append(val & M32)
            i = m.end()
        elif k == 'x':
            m = HEX_RE.match(text, i)
            if not m:
                stopped = True
                out.append(None)
                continue
            tok = m.group(0)
            digits = tok.lstrip('+-')
            if len(digits) > 8:
                raise Fail('hexadecimal largo')
            if text[m.end():m.end() + 1] in ('x', 'X'):
                raise Fail('prefijo 0x no modelado')
            val = int(digits, 16)
            out.append((-val if tok.startswith('-') else val) & M32)     # el CRT acepta un signo en %x (como strtoul): complemento a dos
            i = m.end()
        else:   # 'f'
            body = text[i:].lstrip('+-')
            m = FLOAT_RE.match(text, i)
            if body[:1] in ('i', 'I', 'n', 'N') or body[:2] in ('0x', '0X'):
                raise Fail('real exotico')
            if not m:
                if body[:1] == '.' or len(body) != len(text) - i:
                    raise Fail('real exotico')
                stopped = True
                out.append(None)
                continue
            if text[m.end():m.end() + 1] in ('e', 'E', '#'):
                raise Fail('real exotico')
            tok = m.group(0)
            mant = re.split('[eE]', tok.lstrip('+-'))[0].replace('.', '').lstrip('0')
            fr = Fraction(tok.lstrip('+'))
            if len(mant) > 9 or (fr != 0 and not (Fraction(1, 10 ** 30) <= abs(fr) <= 10 ** 30)):
                raise Fail('real sensible al CRT')
            bits = round_f32(fr)
            if fr == 0 and tok.startswith('-'):
                bits = 0x80000000
            out.append(bits)
            i = m.end()
    return out


def hit_level(word):
    c = word[:1]
    return {'H': 0, 'L': 1, 'G': 2, 'B': 3}.get(c, 7)


def hit_sound(n):
    n = s32(n)
    if ((n - 1) & M32) > 7:
        return 0x3E
    return 0x3C + n


def movsx16(v):
    v &= 0xFFFF
    return (v - 0x10000 if v & 0x8000 else v) & M32


# ---------------------------------------------------------------------------------------------------- modelo de la funcion
class Mem:
    def __init__(self, prefill):
        self.b = bytearray(prefill)

    def _o(self, addr, n):
        o = addr - REGION_BASE
        if o < 0 or o + n > REGION:
            raise Fail('escritura fuera de la region observada: 0x%X' % addr)
        return o

    def w32(self, addr, v):
        o = self._o(addr, 4)
        self.b[o:o + 4] = struct.pack('<I', v & M32)

    def w16(self, addr, v):
        o = self._o(addr, 2)
        self.b[o:o + 2] = struct.pack('<H', v & 0xFFFF)

    def w8(self, addr, v):
        o = self._o(addr, 1)
        self.b[o] = v & 0xFF


def expected_load(inp, case, keep=None):
    prefill = fill_words(REGION, inp['prefill_seed'])
    if '%016x' % fnv64(prefill) != inp['prefill_hash']:
        raise RuntimeError('El patron previo del oraculo difiere del de Fixtures.hpp (caso %d)' % case)
    if inp['region_base'] != REGION_BASE or inp['region_off'] != REGION_OFF or inp['region_bytes'] != REGION_BYTES:
        raise RuntimeError('Region inesperada')
    mem = Mem(prefill)
    stats = {'family%d' % inp['family']: True}
    # --- inicializacion de las 14 filas y de los siete vectores
    row = TABLE
    while row + 4 < 0xC177BC:
        mem.w32(row, 0x21)
        mem.w32(row + 4, 0x3FC00000)
        for i in range(5):
            mem.w32(row + 0x08 + 4 * i, 0x42C80000)
            mem.w32(row + 0x1C + 4 * i, 0x42C80000)
            mem.w32(row + 0x30 + 4 * i, 0x3F800000)
            mem.w8(row + 0x50 + i, 7)
            mem.w8(row + 0x55 + i, 0)
            mem.w32(row + 0x5C + 4 * i, 0)
            mem.w32(row + 0x70 + 4 * i, 0)
        mem.w32(row + 0x44, 0)
        mem.w32(row + 0x48, 0x42C80000)
        mem.w32(row + 0x4C, 0x42C80000)
        mem.w16(row + 0x84, 0)
        row += 0x88
    for i in range(7):
        mem.w32(LEVELS + 12 * i, 0)
        mem.w32(LEVELS + 12 * i + 4, 0x3F400000)
        mem.w32(LEVELS + 12 * i + 8, 0)

    lines = inp['lines']
    groups = inp['groups']
    count = s32(inp['num_groups'])
    ev = [(1, fnv32('DATA\\melee.dat'), fnv32('rb'))]
    li = 0
    n_names = 0
    # escalares persistentes (None = basura de pila sin definir)
    v = {'word2': None, 'raw28': None, 'x3c': None, 'f20': None, 'f24': None, 'raw2c': None, 'i30': None, 'i34': None, 'i38': None}
    level = [0, 0x3F400000, 0]
    ebx = 0
    ebp = 0
    in_combo = in_levels = False
    closed = 0
    seen = {}

    def use(name):
        if v[name] is None:
            raise Fail('fixture depende de %s sin definir' % name)
        return v[name]

    def read():
        nonlocal li
        if li >= len(lines):
            ev.append((2, 0xFFFFFFFF, FILE_ID))
            return None
        ev.append((2, li, FILE_ID))
        li += 1
        return lines[li - 1]

    line = read()
    ended = False
    while line is not None:
        if line[:1] == '#' or line == '':
            seen['skipped'] = True
            line = read()
            continue
        if line.startswith('END_MELEE_DATA'):
            ended = True
            break
        if not in_combo and not in_levels:
            if line.startswith('START_COMBO'):
                in_combo = True
            elif line.startswith('START_LEVELS'):
                in_levels = True
            else:
                seen['ignored_outside'] = True
            line = read()
            continue
        if line.startswith('END_COMBO'):
            if in_combo:
                ebp += 0x88
                closed += 1
            ebx = 0
            in_combo = in_levels = False
            line = read()
            continue
        if in_levels:
            r = scan(line, 'sfff')
            for j in range(3):
                if r[1 + j] is not None:
                    level[j] = r[1 + j]
                else:
                    seen['level_partial'] = True
            base = LEVELS + 12 * ebx
            mem.w32(base, level[0])
            mem.w32(base + 4, level[1])
            mem.w32(base + 8, level[2])
            seen['level_vectors'] = seen.get('level_vectors', 0) + 1
            if ebx >= 7:
                seen['level_overflow'] = True
            ebx += 1
            line = read()
            continue
        idx = ebx
        ebx += 1
        crow = TABLE + ebp
        if idx > 8:
            seen['default_line'] = True
        elif idx == 0:
            r = scan(line, 'ss')
            if r[1] is not None:
                v['word2'] = r[1]
            w2 = use('word2')
            nn = count
            found = False
            if nn > 0:
                i = 0
                while True:
                    ev.append((4, i, 0))
                    n_names += 1
                    name = groups[i] if 0 <= i < len(groups) else '?'
                    if w2 == name:
                        mem.w32(crow, i)
                        found = True
                        break
                    i += 1
                    if not i < nn:
                        break
            seen['group_found' if found else 'group_not_found'] = True
            if nn <= 0:
                seen['group_count_nonpositive'] = True
        elif idx == 1:
            r = scan(line, 'sf')
            if r[1] is not None:
                v['raw28'] = r[1]
            else:
                seen['range_stale'] = True
            mem.w32(crow + 4, use('raw28'))
        elif 2 <= idx <= 6:
            mv = idx - 2
            r = scan(line, 'sfffsdddf')
            f1c = r[8] if r[8] is not None else 0
            for name, pos in (('f20', 1), ('f24', 2), ('raw2c', 3), ('word2', 4), ('i30', 5), ('i34', 6), ('i38', 7)):
                if r[pos] is not None:
                    v[name] = r[pos]
                else:
                    seen['hit_stale'] = True
            f20, f24, raw2c, w2 = use('f20'), use('f24'), use('raw2c'), use('word2')
            i30, i34, i38 = use('i30'), use('i34'), use('i38')
            mem.w32(crow + 0x08 + 4 * mv, mulk(f20))
            mem.w32(crow + 0x30 + 4 * mv, raw2c)
            mem.w32(crow + 0x1C + 4 * mv, mulk(f24))
            lvl = hit_level(w2)
            mem.w8(crow + 0x50 + mv, lvl)
            mem.w8(crow + 0x55 + mv, i30)
            mem.w32(crow + 0x5C + 4 * mv, movsx16(hit_sound(i34)))
            mem.w32(crow + 0x70 + 4 * mv, movsx16(hit_sound(i38)))
            fv = fl(f1c)
            if fv > 0.0:
                mem.w32(crow + 0x44, mulk(f1c))
                seen['f1c_stored'] = True
            else:
                seen['f1c_skipped'] = True
            seen['hit_level%d' % lvl] = True
            seen['sound_default'] = seen.get('sound_default', False) or hit_sound(i34) == 0x3E and s32(i34) != 2 or hit_sound(i38) == 0x3E and s32(i38) != 2
            seen['hit_lines'] = seen.get('hit_lines', 0) + 1
        elif idx == 7:
            r = scan(line, 'sff')
            for name, pos in (('f20', 1), ('f24', 2)):
                if r[pos] is not None:
                    v[name] = r[pos]
                else:
                    seen['block_stale'] = True
            mem.w32(crow + 0x48, mulk(use('f20')))
            mem.w32(crow + 0x4C, mulk(use('f24')))
        else:   # 8
            r = scan(line, 'sx')
            if r[1] is not None:
                v['x3c'] = r[1]
            else:
                seen['flags_stale'] = True
            mem.w16(crow + 0x84, use('x3c'))
        line = read()
    ev.append((3, FILE_ID, 0))
    if keep is not None:
        keep.append(mem)
    post = bytes(mem.b)
    changed = []
    for i in range(0, REGION, 4):
        a = struct.unpack_from('<I', post, i)[0]
        b = struct.unpack_from('<I', prefill, i)[0]
        if a != b:
            changed.append([i, a])
    evbytes = b''.join(struct.pack('<III', *e) for e in ev[:1024])
    want = {
        'events_hash': '%016x' % fnv64(evbytes), 'n_events': len(ev), 'n_open': 1, 'n_read': sum(1 for e in ev if e[0] == 2), 'n_close': 1,
        'n_name': n_names, 'group_count_after': inp['num_groups'] & M32, 'window_hash': '%016x' % fnv64(post), 'changed_words': len(changed), 'first_changes': changed[:48],
    }
    if len(ev) > 1024:
        raise Fail('mas eventos de los que registra el banco')
    stats.update({
        'combos_closed_0': closed == 0, 'combos_closed_1_5': 1 <= closed <= 5, 'combos_closed_6_12': 6 <= closed <= 12, 'combos_closed_13_plus': closed >= 13,
        'ended_by_end_melee_data': ended, 'eof': not ended,
        'lines_over_40': len(lines) > 40, 'lines_over_100': len(lines) > 100,
        'num_groups_zero': count == 0, 'num_groups_negative': count < 0, 'num_groups_positive': count > 0,
    })
    for key, value in seen.items():
        stats['seen_' + key] = bool(value)
    return want, stats


def expected(function, inp, case):
    if function == NAME:
        return expected_load(inp, case)
    raise RuntimeError('Funcion inesperada ' + function)


if __name__ == '__main__':
    # --- autoprueba del modelo de sscanf
    assert scan('RANGE 1.5', 'sf') == ['RANGE', 0x3FC00000]
    assert scan('X 24.0 -0.0 5. abc 3 -2 255 .25', 'sfffsdddf') == ['X', f32bits(24.0), 0x80000000, f32bits(5.0), 'abc', 3, (-2) & M32, 255, f32bits(0.25)]
    assert scan('X 1.5 xyz', 'sff') == ['X', 0x3FC00000, None]
    assert scan('X', 'ss') == ['X', None]
    assert scan('F FFFF0004', 'sx') == ['F', 0xFFFF0004]
    assert scan('F zz', 'sx') == ['F', None]
    assert scan('A 12abc', 'sd') == ['A', 12]
    assert scan('A 1e2 2.5e-1', 'sff') == ['A', f32bits(100.0), f32bits(0.25)]
    # --- funciones auxiliares del EXE
    assert [hit_level(s) for s in ('HIT', 'LOW', 'GROUND', 'BLOCK', 'X', 'hit', '', '123')] == [0, 1, 2, 3, 7, 7, 7, 7]
    assert [hit_sound(n) for n in (1, 2, 3, 8, 0, 9, -1, 0x7FFFFFFF, -0x80000000)] == [0x3D, 0x3E, 0x3F, 0x44, 0x3E, 0x3E, 0x3E, 0x3E, 0x3E]
    # --- fichero canonico con un combo completo y un bloque de niveles
    seed = 7
    pre = fill_words(REGION, seed)
    lines = ['# datos', 'START_COMBO', 'name FIGHTA', 'RANGE 1.5',
             'HIT1 30.0 60.0 0.5 HIT 10 1 2 15.0', 'HIT2 31 61 0.25 LOW 11 2 3', 'HIT3 32 62 1 GROUND 12 3 4', 'HIT4 33 63 2 BLOCK 13 4 5', 'HIT5 34 64 3 X 14 5 6',
             'BLOCK 3.0 6.0', 'FLAGS 1A2B', 'END_COMBO', 'START_LEVELS', 'LV 1 2 3', 'LV 4', 'END_COMBO', 'END_MELEE_DATA']
    inp = dict(family=0, num_groups=2, groups=['walk', 'FIGHTA'], lines=lines, prefill_seed=seed, prefill_hash='%016x' % fnv64(pre), eax=0, ecx=0, edx=0, stack_seed=0,
               region_base=REGION_BASE, region_off=REGION_OFF, region_bytes=REGION_BYTES)
    keep = []
    want, st = expected_load(inp, 0, keep)
    m = keep[0]

    def r32(a):
        return struct.unpack_from('<I', m.b, a - REGION_BASE)[0]
    c0 = TABLE
    # fila 0 (grupo FIGHTA = indice 1), campos escritos por las lineas del combo
    assert r32(c0) == 1 and r32(c0 + 4) == 0x3FC00000
    for k, (hit, chain) in enumerate(((30.0, 60.0), (31.0, 61.0), (32.0, 62.0), (33.0, 63.0), (34.0, 64.0))):
        assert r32(c0 + 0x08 + 4 * k) == f32bits(hit * fl(K)) and r32(c0 + 0x1C + 4 * k) == f32bits(chain * fl(K)), k
    assert [r32(c0 + 0x30 + 4 * k) for k in range(5)] == [f32bits(x) for x in (0.5, 0.25, 1.0, 2.0, 3.0)]
    assert [m.b[c0 - REGION_BASE + 0x50 + k] for k in range(5)] == [0, 1, 2, 3, 7]          # HIT LOW GROUND BLOCK X
    assert [m.b[c0 - REGION_BASE + 0x55 + k] for k in range(5)] == [10, 11, 12, 13, 14]
    assert [r32(c0 + 0x5C + 4 * k) for k in range(5)] == [0x3D, 0x3E, 0x3F, 0x40, 0x41]    # sonidos 1..5 -> 0x3D..0x41
    assert [r32(c0 + 0x70 + 4 * k) for k in range(5)] == [0x3E, 0x3F, 0x40, 0x41, 0x42]    # 2..6
    assert r32(c0 + 0x44) == f32bits(15.0 * fl(K)) and r32(c0 + 0x48) == f32bits(3.0 * fl(K)) and r32(c0 + 0x4C) == f32bits(6.0 * fl(K))
    assert struct.unpack_from('<H', m.b, c0 - REGION_BASE + 0x84)[0] == 0x1A2B
    # fila 1: sin leer, valores iniciales
    c1 = TABLE + 0x88
    assert r32(c1) == 0x21 and r32(c1 + 4) == 0x3FC00000 and r32(c1 + 8) == 0x42C80000 and r32(c1 + 0x30) == 0x3F800000 and m.b[c1 - REGION_BASE + 0x50] == 7
    assert r32(c1 + 0x48) == 0x42C80000 and r32(c1 + 0x4C) == 0x42C80000 and r32(c1 + 0x44) == 0
    # niveles: el vector 0 lee (1, 2, 3); el 1 lee solo x = 4 y conserva (2, 3)
    assert [r32(LEVELS + k * 4) for k in range(3)] == [f32bits(1.0), f32bits(2.0), f32bits(3.0)]
    assert [r32(LEVELS + 12 + k * 4) for k in range(3)] == [f32bits(4.0), f32bits(2.0), f32bits(3.0)]
    assert [r32(LEVELS + 24 + k * 4) for k in range(3)] == [0, 0x3F400000, 0]
    assert want['n_name'] == 2 and want['n_read'] == len(lines) and want['n_events'] == 1 + len(lines) + 2 + 1
    assert st['seen_group_found'] and st['ended_by_end_melee_data'] and st['seen_level_vectors'] and st['seen_f1c_stored'] and st['combos_closed_1_5']
    # grupo desconocido: recorre todos los nombres y no escribe el indice (queda el 0x21 inicial)
    keep = []
    want, st = expected_load(dict(inp, groups=['a', 'b'], num_groups=2), 1, keep)
    assert struct.unpack_from('<I', keep[0].b, c0 - REGION_BASE)[0] == 0x21 and want['n_name'] == 2 and st['seen_group_not_found']
    # contador de grupos negativo o cero: ninguna llamada a GetAnimGroupName
    for cnt in (0, 0xFFFFFFFF, 0x80000000):
        want, st = expected_load(dict(inp, num_groups=cnt), 2)
        assert want['n_name'] == 0 and 'seen_group_found' not in st or not st.get('seen_group_found')
        assert want['n_name'] == 0
    # fichero que fuerza el uso de un escalar sin definir
    try:
        expected_load(dict(inp, lines=['START_COMBO', 'a b', 'RANGE x']), 3)
        raise SystemExit('el oraculo debia rechazar un escalar sin definir')
    except Fail:
        pass
    # END_MELEE_DATA corta: no se leen mas lineas
    want, st = expected_load(dict(inp, lines=['END_MELEE_DATA', 'START_COMBO']), 4)
    assert want['n_read'] == 1 and st['ended_by_end_melee_data']
    # fin de fichero: la lectura nula se registra
    want, st = expected_load(dict(inp, lines=[]), 5)
    assert want['n_read'] == 1 and st['eof']
    print('ok')
