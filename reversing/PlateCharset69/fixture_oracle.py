"""Oraculo del banco PlateCharset69: modelo en Python de CCustomCarPlateMgr::LoadPlatecharsetDat (0x6FDC00) DESDE EL DESENSAMBLADO del EXE (no del C++), aplicado a la fila `input` de cada caso.

Devuelve lo que el banco observa en el lado ORIGINAL (EXE): AL, la traza de eventos de los falsos (SetDir/OpenFile/LoadLine/CloseFile), el CRC-32 de la arena de salida, los dwords escritos,
y el hash del buffer de linea tras los strtok. run_local.py lo compara con las filas del EXE y de la DLL (original == candidato ya se exige aparte).

Modelo (0x6FDC00..0x6FDD60, atoi 0x822506, strtok 0x82244B):
  SetDir("DATA"); f = OpenFile(filename, "r"); SetDir("");   [llamadas cdecl 0x5387D0, 0x538900, 0x5387D0]
  while (linea = LoadLine(f)) [0x536F80]:
      si linea == ";the end" (repe cmpsb de 9 bytes: incluye el NUL) -> sale del bucle
      si linea[0] == ';' -> siguiente linea
      tok = strtok(linea, " \\t") [el primer resultado NO se comprueba: con NULL, atoi(NULL) accede a la direccion 0 -> el banco nunca genera esas lineas]
      n = 0; repetir { n==0: b0 = min(ax,255) (ax = 16 bits bajos de atoi(tok)); n==1: b1; n==2: b2; tok = strtok(NULL, " \\t"); n++ } mientras tok
      si n < 3 -> devuelve 0 SIN cerrar el archivo
      *out++ = 0x80000000 | b2 << 16 | b1 << 8 | b0
  CloseFile(f); devuelve 1
atoi del EXE: salta isspace (C: 9..13 y 32), signo '-' o '+', digitos acumulados con desbordamiento de 32 bits (sin saturar), '-' niega.
strtok del EXE: conjunto de separadores con mapa de bits (incluye el NUL); salta separadores iniciales; el token termina en el siguiente separador (se sustituye por NUL); devuelve NULL si el
token esta vacio.

Autoprueba:  python -I fixture_oracle.py   (casos escritos a mano a partir del desensamblado; imprime ok)
"""
import zlib

CASES = 16384
NAMES = ['CCustomCarPlateMgr::LoadPlatecharsetDat']
ADDRESS = [0x6FDC00]
KARENA = 0x50 + 40 * 4 + 0x40
KOUTWORDS = 40
KLINEBUF = 1024
M32 = 0xFFFFFFFF
LAYOUT = {'arena': KARENA, 'out_words': KOUTWORDS, 'max_lines': 40, 'line_buf': KLINEBUF}
SEPS = (0x20, 0x09)


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


def fill_arena(size, mode, seed):
    if mode == 1:
        return bytearray(size)
    if mode == 2:
        return bytearray(b'\xFF' * size)
    if mode == 3:
        return bytearray(b'\xCD' * size)
    if mode == 4:
        return bytearray(b'\xA5' * size)
    s = (seed ^ 0x5A5A5A5A) & M32 if mode == 5 else seed
    out = bytearray()
    for i in range(size // 4):
        out += rand(s, i % 127).to_bytes(4, 'little')
    out += bytes(size - len(out))
    return out


def fnv(b):
    h = 2166136261
    for x in b:
        h = ((h ^ x) * 16777619) & M32
    return h


def is_space(c):
    return c == 32 or 9 <= c <= 13


def parse_exact(tok):
    """(signo, valor sin acotar, hay digitos) del texto, con las reglas del EXE (para estadisticas de cobertura)."""
    n = len(tok)

    def at(i):
        return tok[i] if i < n else 0
    i = 0
    while is_space(at(i)):
        i += 1
    c = at(i)
    i += 1
    sign = c
    if c in (0x2D, 0x2B):
        c = at(i)
        i += 1
    v = 0
    digits = 0
    while 0x30 <= c <= 0x39:
        v = v * 10 + c - 0x30
        digits += 1
        c = at(i)
        i += 1
    return sign == 0x2D, v, digits


def atoi_exe(tok):
    """atoi del EXE (0x822506) sobre los bytes del token (la cadena acaba en el NUL): desbordamiento de 32 bits."""
    neg, v, _ = parse_exact(tok)
    v &= M32
    return (-v) & M32 if neg else v


def clamp8(tok):
    v = atoi_exe(tok) & 0xFFFF
    return 0xFF if v > 0xFF else v


class Strtok:
    """strtok del EXE sobre un bytearray terminado en NUL (separadores espacio y tabulador)."""
    def __init__(self, buf):
        self.buf = buf
        self.saved = 0

    def next(self, start=None):
        b = self.buf
        p = self.saved if start is None else start
        while b[p] != 0 and b[p] in SEPS:
            p += 1
        tok = p
        while b[p] != 0:
            if b[p] in SEPS:
                b[p] = 0
                p += 1
                break
            p += 1
        self.saved = p
        if tok == p:
            return None
        end = tok
        while b[end] != 0:
            end += 1
        return bytes(b[tok:end])


def expected(inp):
    """(observado esperado, dwords escritos, estadisticas de cobertura)"""
    name = bytes.fromhex(inp['name'])
    lines = [bytes.fromhex(x) for x in inp['lines']]
    handle = inp['open_ret']
    ev = [[1, fnv(b'DATA'), 0], [2, fnv(name), fnv(b'r')], [1, fnv(b''), 0]]
    linebuf = bytearray(b'\xDD' * KLINEBUF)
    idx = 0
    words = []
    ret = None
    st = {}

    def bump(k, cond=True):
        if cond:
            st[k] = 1
    bump('open_null', handle == 0)
    bump('name_empty', not name)
    bump('unaligned', inp['data_off'] & 3)
    while True:
        ev.append([3, handle, idx])
        if handle == 0 or idx >= len(lines):
            bump('eof', handle != 0)
            break
        s = lines[idx]
        idx += 1
        linebuf[0:len(s)] = s
        linebuf[len(s)] = 0
        if s == b';the end':
            bump('end_line')
            bump('lines_after_end', idx < len(lines))
            break
        if s[0:1] == b';':
            bump('comment')
            bump('near_end', s.lower().startswith(b';the'))
            continue
        tk = Strtok(linebuf)
        tok = tk.next(0)
        assert tok is not None, 'linea vacia o de solo separadores: el EXE accede a la direccion 0'
        n = 0
        b = [0, 0, 0]
        bump('lead_sep', s[0] in SEPS)
        bump('trail_sep', s[-1] in SEPS)
        bump('tab_sep', 9 in s)
        while True:
            if n < 3:
                b[n] = clamp8(tok)
                neg, v, digits = parse_exact(tok)
                bump('clamp_hi', (atoi_exe(tok) & 0xFFFF) > 0xFF)
                bump('exact_255', b[n] == 0xFF and not neg and v == 255)
                bump('neg_field', neg and digits > 0)
                bump('wrap32', v >= 1 << 31)
                bump('ws_prefix', tok[0] in (10, 11, 12, 13))
                bump('no_digits', digits == 0)
                bump('high_byte', any(c >= 0x80 for c in tok))
                bump('sign_plus', tok.lstrip(b'\t\n\v\f\r ')[0:1] == b'+')
            tok = tk.next()
            n += 1
            if tok is None:
                break
        bump('extra_fields', n > 3)
        bump('short_line', n < 3)
        if n < 3:
            ret = 0
            break
        words.append(0x80000000 | (b[2] << 16) | (b[1] << 8) | b[0])
    if ret is None:
        ev.append([4, handle, 0])
        ret = 1
    bump('ret_ok', ret == 1)
    bump('ret_zero', ret == 0)
    bump('words_ge1', len(words) >= 1)
    bump('words_ge38', len(words) >= 38)
    ref = fill_arena(KARENA, inp['mode'], inp['seed'])
    arena = bytearray(ref)
    off = inp['data_off']
    for i, w in enumerate(words):
        arena[off + 4 * i:off + 4 * i + 4] = w.to_bytes(4, 'little')
    last = -1
    for i in range(KOUTWORDS):
        if arena[off + 4 * i:off + 4 * i + 4] != ref[off + 4 * i:off + 4 * i + 4]:
            last = i
    out = [int.from_bytes(arena[off + 4 * i:off + 4 * i + 4], 'little') for i in range(last + 1)]
    obs = {'al': ret, 'events': ev, 'arena_crc': zlib.crc32(bytes(arena)) & M32, 'out': out, 'line_hash': fnv(bytes(linebuf)), 'events_total': len(ev)}
    return obs, words, st


def check(row):
    """Compara el lado ORIGINAL (EXE) de una fila con el modelo; devuelve la lista de campos distintos."""
    want, _, _ = expected(row['input'])
    got = row['original']['observed']
    return [k for k in want if got.get(k) != want[k]]


def selftest():
    def case(lines, name=b'PLATECHARSET.DAT', open_ret=0xC0DE00, mode=1, off=0x40):
        return {'name': name.hex(), 'lines': [x.hex() for x in lines], 'open_ret': open_ret, 'mode': mode, 'seed': 1, 'data_off': off}
    # 1 2 3 -> 0x80030201
    e, w, _ = expected(case([b'1 2 3']))
    assert w == [0x80030201] and e['al'] == 1 and e['events'][-1] == [4, 0xC0DE00, 0], (w, e)
    # 256 -> ax = 0x100 > 0xFF -> 255; -1 -> ax = 0xFFFF -> 255; 65536 -> ax = 0 -> 0
    e, w, _ = expected(case([b'256 -1 65536']))
    assert w == [0x80000000 | (0 << 16) | (255 << 8) | 255], hex(w[0])
    # desbordamiento de 32 bits: 4294967297 -> 1 (el CRT de la DLL saturaria); 4294967551 -> 0x100000FF -> 255
    e, w, _ = expected(case([b'4294967297 4294967551 4294967296']))
    assert w == [0x80000000 | (0 << 16) | (255 << 8) | 1], hex(w[0])
    # menos de tres campos: devuelve 0 sin CloseFile
    e, w, _ = expected(case([b'1 2 3', b'4 5']))
    assert e['al'] == 0 and w == [0x80030201] and all(x[0] != 4 for x in e['events']), e
    # ";the end" termina; lo que sigue no se lee; la traza lo muestra
    e, w, _ = expected(case([b';c', b'9 8 7', b';the end', b'1 2 3']))
    assert w == [0x80070809] and e['al'] == 1 and [x[0] for x in e['events']] == [1, 2, 1, 3, 3, 3, 4], e['events']
    # ";the end " (con espacio) es un comentario; archivo abierto con NULL -> LoadLine(NULL) devuelve NULL
    e, w, _ = expected(case([b';the end ', b'1 1 1']))
    assert w == [0x80010101]
    e, w, _ = expected(case([b'1 1 1'], open_ret=0))
    assert w == [] and e['al'] == 1 and e['events'][3] == [3, 0, 0] and e['events'][4] == [4, 0, 0]
    # espacios iniciales, tabuladores, signo + y blancos del CRT: "\v12" -> 12
    e, w, _ = expected(case([b' \t7\t\t\v12   +5 99']))
    assert w == [0x80050C07], hex(w[0])
    # atoi
    assert atoi_exe(b'  -12') == (-12) & M32 and atoi_exe(b'abc') == 0 and atoi_exe(b'-') == 0 and atoi_exe(b'+7x') == 7 and atoi_exe(b'0x10') == 0
    assert atoi_exe(b'-4294967295') == 1 and atoi_exe(b'2147483648') == 0x80000000
    print('ok')


if __name__ == '__main__':
    selftest()
