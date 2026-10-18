"""Oraculo independiente del banco FileLoaderLoad2dEffect27: CFileLoader::Load2dEffect (0x5B7670).

Se modela desde el DESENSAMBLADO del EXE (reversing/_handoff/dis.sh 0x5B7670 0x5B8030), no desde el C++ de la DLL, a partir de la fila `input` que registra el banco. Re-analiza el
TEXTO de la linea con un modelo propio de sscanf, asi que no depende del generador de Fixtures.hpp.

CFileLoader::Load2dEffect(const char* line), cdecl, `ret` normal (la funcion termina con `jmp 0x7316B0` = CTxdStore::PopCurrentTxd). Marco local de 0x1CC bytes.
  5B7684  sscanf(line, "%d %f %f %f %d", &modelId, &x, &y, &z, &type)               (0x8220AD = sscanf del CRT estatico del EXE)
  5B76A4  CTxdStore::PushCurrentTxd() (0x7316A0) ; SetCurrentTxd(FindTxdSlot("particle")) (0x731850 / 0x7319C0)
  5B76B9  edi = ms_modelInfoPtrs[modelId] (0xA9B0C8) ; eax = CModelInfo::Get2dEffectStore() (0x4C5A60)      (no se comprueba nada del modelId)
  5B76CC  esi = store + 4 + (count << 6) ; ++count ; edi->Add2dEffect(esi) (0x4C4D20, thiscall, 1 argumento)
  5B76E6  [esi+0]=x [esi+4]=y [esi+8]=z (copia de dwords) ; byte [esi+0xC] = (u8)type
  5B7701  `cmp type,0Ah ; ja default` (SIN signo) ; salto por tabla en 0x5B8004:
          0->5B78C2 luz | 1->5B7A40 particula | 2,4 y >10 -> 5B7FF5 (nada) | 3->5B7A79 atractor | 5->5B7B1B cartel | 6->5B7DC8 tragaperras | 7->5B7711 interior | 8->5B7E63 | 9->5B7EAA | 10->5B7F15
  5B7FF5  pop edi/esi/ebx ; add esp,1CCh ; jmp 7316B0 (PopCurrentTxd)
Todas las ramas terminan en 5B7FF5 o en su propio `jmp 007316B0`. Los campos se escriben por DESPLAZAMIENTO dentro del bloque de 0x40 bytes y sscanf escribe directamente en el bloque en
los campos marcados "directo". Las variables locales que el EXE NO inicializa llegan al bloque como basura de pila si sscanf no las lee (el banco jamas depende de ellas; ver `mask`).

Formatos (strings del EXE) y destinos (offset en el bloque; d = directo de sscanf, B = byte bajo de una variable local, W = palabra baja, F = dword copiado):
  0 luz   (5B78C2): sscanf1 "%d %f %f %f %d %d %d %d %d" -> r g b a (locales) ; luego busca DOS cadenas "..." recorriendo la linea sin comprobar su fin (bucle `cmp al,22h` sobre la
          memoria, tambien tras el NUL) ; sscanf2 (desde el caracter posterior a la 2.a comilla de cierre) "%f %f %f %f %d x9" -> floats directos en 14 18 1C 20 ; ints v29 v26 v27 v28 flags v2a v2b v2c v2d.
          B: 10<-r 11<-g 12<-b 13<-a 29<-v29 26<-v26 27<-v27 28<-v28 2A<-v2a 2B<-v2b 2C<-v2c 2D<-v2d ; W: 24<-flags ; 30<-RwTextureRead(corona,0) 34<-RwTextureRead(sombra,0) (0x7F3AC0);
          si (u8)flags & 4: byte[24] &= 0xFD.
  1 part. (5B7A40): "%d %f %f %f %d %s" -> el %s se escribe directo en 10 (cadena + NUL, SIN limite).
  3 atr.  (5B7A79): "%d %f %f %f %d %d %f x9 %d %d %s" -> floats directos en 10 14 18 1C 20 24 28 2C 30 ; B: 34<-v34 35<-v35 36<-v36 ; la cadena se copia con un bucle (strcpy) a 38 (con NUL).
  5 cartel(5B7B1B): 25 locales a -1 y una a 0x32 ; "%d %f %f %f %d %d %f %f %f %f %d x28" -> a5 directo (dword) en 10 ; sizeX sizeY sizeZ (locales, SIN inicializar) -> _ftol2 (0x821B40)
          -> byte bajo en 12 13 14 ; a9 directo (float) en 30 ; luego 28 enteros -> bytes (a10->15 a11->2E[ambos SIN inicializar] a12->11 a13->2F[def 0x32] a14->16 a15->17 a16->1A a17->1B a18->18
          a19->19 a20->1C a21->1D a22->20 a23->21 a24->1E a25->1F a26->22 a27->25 a28->28 a29->2B a30->23 a31->26 a32->29 a33->2C a34->24 a35->27 a36->2A a37->2D) ; defecto -1.
  6 trag. (5B7DC8): "%d %f %f %f %d %f x7 %d %d %s %d" -> floats directos en 10 14 18 1C 20 24 28 ; W: 2C<-v2c ; B: 2E<-v2e 2F<-v2f ; strncpy(bloque+30, nombre, 8) (0x821F40, rellena con ceros hasta 8).
  7 inter.(5B7711): cuatro buffers de 33 bytes (byte0=' ', resto 0) ; "%d %f %f %f %d %f %f %f %f %f %d %s %s %s %s" -> locales ; F: 10..20 <- 5 floats (en orden) ; W: 24<-f1a4 ;
          28 <- Malloc(0x40, 0) (0x72F420) ; 4 x 16 bytes copiados de los 4 buffers al nuevo bloque ; dword[2C]=0.
  8 (5B7E63): "%d %f %f %f %d %d" -> F: 10<-v.     9 (5B7EAA): "... %d %f %f %d" -> F: 10<-f0 14<-f1 ; B: 18<-v.
 10 (5B7F15): "... %d" + 9 floats + %d -> F: 10 14 18 1C 20 24 28 2C 30 <- f1..f9 (en orden) ; byte 34 <- (v != 0).
 2, 4 y >10: no se lee ni se escribe nada mas.

Modelo de sscanf. El EXE usa el sscanf del CRT estatico viejo (0x8220AD), la DLL usa el de su CRT (UCRT estatico /MTd). El oraculo solo CONFIA en lo que es comun a ambos:
  enteros "%d" = [+-]digitos con <=12 cifras y valor en int32 ; reales "%f" = [+-](d+[.d*]|.d+)[eE[+-]d+] con <=9 cifras significativas y |v| en [1e-30,1e30] (redondeo decimal->float
  correcto) ; "%s" = secuencia sin blancos. Todo lo demas se CLASIFICA como sensible al CRT: el campo sigue pareciendo "leido" (valor desconocido) o, si ni siquiera se sabe si se
  parsea ('inf', 'nan', hexadecimales, 'e' sin exponente...), toda la lectura posterior pasa a "desconocida". Los bytes que dependen de un valor/estado desconocido se comparan como
  comodin ('??'); NADA se oculta en la comparacion EXE-DLL (run_local.py exige igualdad de pares siempre).
"""
import math
import re
import struct
import sys
from fractions import Fraction

M32 = 0xFFFFFFFF
NAME = 'CFileLoader::Load2dEffect'
NAMES = [NAME]
CASES = {NAME: 6144}
ENTRY = 0x40
MODEL_TABLE = 20000
BLOCK_LABEL = 0xB10C0000          # el banco sustituye en +0x28 el puntero del bloque Malloc por esta etiqueta
BLOCK_PRE = 0xB7                  # relleno previo del bloque de Malloc
WS = ' \t\n\v\f\r'
MAX_CORONA, MAX_SHADOW = 99, 119   # buffers del EXE (0x88..0xEB = 100 bytes y 0xEC..0x15F)


class Fail(Exception):
    """Fixture que depende de comportamiento indefinido del EXE (o inconsistente con la fila `input`)."""


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def fl(b):
    return struct.unpack('<f', struct.pack('<I', b & M32))[0]


def f32bits(x):
    return struct.unpack('<I', struct.pack('<f', x))[0]


def fnv32(data):
    h = 2166136261
    for c in data:
        h = ((h ^ c) * 16777619) & M32
    return h


def round_f32(fr):
    """Fraction -> bits de float32 con redondeo al par (correcto); desborda a inf."""
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
    if q < 1 << 23:                      # denormal
        return sign | q
    return sign | ((e + 127) << 23) | (q - (1 << 23))


def ftol_byte(bits):
    """Byte bajo del truncado a int64 de _ftol2 (0x821B40) para el float de bits `bits` (NaN, inf y |x|>=2^63 -> indefinido -> 0)."""
    x = fl(bits)
    if math.isnan(x) or math.isinf(x):
        return 0
    t = math.trunc(x)
    if t >= 1 << 63 or t < -(1 << 63):
        return 0
    return t & 0xFF


# ---------------------------------------------------------------------------------------------------- modelo de sscanf
class Field:
    __slots__ = ('state', 'v', 'sens', 'kind')

    def __init__(self, state, v=None, sens=False, kind=''):
        self.state, self.v, self.sens, self.kind = state, v, sens, kind    # state: ok | unparsed | unknown


INT_RE = re.compile(r'[+-]?\d+')
FLOAT_RE = re.compile(r'[+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?')


CUR = None      # diccionario de estadisticas del caso en curso (lo rellena expected_load)


def _note(key):
    if CUR is not None:
        CUR[key] = True


def scan(text, kinds):
    i, stopped, out = 0, None, []
    for k in kinds:
        if stopped:
            out.append(Field('unparsed' if stopped == 'fail' else 'unknown'))
            continue
        while i < len(text) and text[i] in WS:
            i += 1
        if i >= len(text):
            stopped = 'fail'
            out.append(Field('unparsed'))
            continue
        if k == 's':
            j = i
            while j < len(text) and text[j] not in WS:
                j += 1
            out.append(Field('ok', text[i:j], False, 's'))
            i = j
            continue
        if k == 'd':
            m = INT_RE.match(text, i)
            if not m:
                stopped = 'fail'
                out.append(Field('unparsed'))
                continue
            tok = m.group(0)
            digits = tok.lstrip('+-')
            val = int(tok)
            sens = not (len(digits) <= 12 and -(1 << 31) <= val <= (1 << 31) - 1)
            if sens:
                _note('crt_sens_int')
            out.append(Field('ok', s32(val), sens, 'd'))
            i = m.end()
            continue
        # 'f'
        rest = text[i:]
        body = rest.lstrip('+-')
        m = FLOAT_RE.match(text, i)
        exotic = False
        if body[:1] in ('i', 'I', 'n', 'N') or body[:2] in ('0x', '0X'):
            exotic = True
        elif not m:
            exotic = body[:1] == '.' or len(body) != len(rest)      # '.', '-.', '+x' ...: signo o punto sin numero
        else:
            nxt = text[m.end():m.end() + 1]
            if nxt in ('e', 'E', '#'):
                exotic = True
        if exotic:
            stopped = 'unknown'
            _note('crt_unknown')
            out.append(Field('unknown'))
            continue
        if not m:
            stopped = 'fail'
            out.append(Field('unparsed'))
            continue
        tok = m.group(0)
        mant = re.split('[eE]', tok.lstrip('+-'))[0].replace('.', '').lstrip('0')
        fr = Fraction(tok.lstrip('+'))
        sens = len(mant) > 9 or (fr != 0 and not (Fraction(1, 10 ** 30) <= abs(fr) <= 10 ** 30))
        if sens:
            _note('crt_sens_float')
        bits = round_f32(fr)
        if fr == 0 and tok.startswith('-'):
            bits = 0x80000000
        out.append(Field('ok', bits, sens, 'f'))
        i = m.end()
    return out


# ---------------------------------------------------------------------------------------------------- bloque con comodines
class Ent:
    """Entradas k, k+1 y k+2 (192 bytes): contenido esperado + bandera de byte desconocido."""

    def __init__(self, pre_entry, pre_over, mask):
        self.b = bytearray(pre_entry + pre_over)
        self.w = [False] * len(self.b)
        self.mask = mask

    def masked(self, off):
        return off < 64 and (self.mask >> off) & 1

    def put(self, off, data, wild=False):
        for j, c in enumerate(data):
            o = off + j
            if o >= len(self.b):
                raise Fail('escritura fuera de las 3 entradas observadas')
            self.b[o] = c
            self.w[o] = wild

    def wild(self, off, n):
        for o in range(off, off + n):
            self.w[o] = True

    def finish(self):
        for o in range(64):
            if (self.mask >> o) & 1:
                self.b[o] = 0
                self.w[o] = False

    def hexes(self):
        def conv(lo, hi):
            return ''.join('??' if self.w[o] else '%02x' % self.b[o] for o in range(lo, hi))
        return conv(0, 64), conv(64, 192)


def le32(v):
    return (v & M32).to_bytes(4, 'little')


class Sim:
    def __init__(self, ent):
        self.e = ent
        self.stats = {}

    def mark(self, key, cond=True):
        if cond:
            self.stats[key] = True

    # destino directo (sscanf escribe en el bloque)
    def direct32(self, off, fld):
        self.mark('crt_plain_fail', fld.state == 'unparsed')
        if fld.state == 'ok' and not fld.sens:
            self.e.put(off, le32(fld.v))
        elif fld.state == 'ok' or fld.state == 'unknown':
            self.e.wild(off, 4)
            self.mark('crt_sens_value')
        # unparsed: conserva el estado previo

    # local copiada al bloque; `default` None = sin inicializar en el EXE (basura de pila si sscanf no la lee)
    def local(self, off, width, fld, default, conv=None):
        masked = self.e.masked(off)
        self.mark('crt_plain_fail', fld.state == 'unparsed')
        if fld.state == 'ok':
            v = fld.v if conv is None else conv(fld.v)
            if masked:
                return
            if fld.sens:
                self.e.wild(off, width)
                self.mark('crt_sens_value')
            else:
                self.e.put(off, (v & ((1 << (8 * width)) - 1)).to_bytes(width, 'little'))
        else:
            if default is None:
                if not masked:
                    raise Fail('variable local sin inicializar en +%#x (campo ausente o dudoso) sin mascara' % off)
                return
            if masked:
                return
            if fld.state == 'unknown':
                self.e.wild(off, width)
            else:
                self.e.put(off, (default & ((1 << (8 * width)) - 1)).to_bytes(width, 'little'))


def need(fields, names):
    for i in range(len(names)):
        if fields[i].state != 'ok':
            raise Fail('campo obligatorio ausente: ' + names[i])


# ---------------------------------------------------------------------------------------------------- ramas
def light(sim, line, tail, fields1):
    # busqueda de comillas sobre la memoria (linea + NUL + cola + NUL + ceros), exactamente como el bucle del EXE
    mem = line.encode('latin-1') + b'\0' + tail.encode('latin-1') + b'\0' + b'\0' * 64
    limit = len(mem) - 32

    def find_quote(p):
        while True:
            if p >= limit:
                raise Fail('busqueda de comillas fuera de la arena')
            c = mem[p]
            p += 1
            if c == 0x22:
                return p

    def copy_name(p, cap):
        out = bytearray()
        while True:
            if p >= limit:
                raise Fail('nombre entre comillas fuera de la arena')
            if mem[p] == 0x22:
                break
            out.append(mem[p])
            p += 1
            if len(out) > cap:
                raise Fail('nombre entre comillas desborda el buffer del EXE')
        return bytes(out), p
    p = find_quote(0)
    corona, p = copy_name(p, MAX_CORONA)
    adjacent = mem[p + 1] == 0x22
    p = find_quote(p + 1)
    shadow, p = copy_name(p, MAX_SHADOW)
    start = p + 1
    end = mem.index(b'\0', start)
    f2 = scan(mem[start:end].decode('latin-1'), ['f'] * 4 + ['d'] * 9)
    r, g, b, a = fields1[5:9]
    v29, v26, v27, v28, flags, v2a, v2b, v2c, v2d = f2[4:13]
    for i, off in enumerate((0x14, 0x18, 0x1C, 0x20)):
        sim.direct32(off, f2[i])
    sim.local(0x11, 1, g, None)
    sim.local(0x10, 1, r, None)
    sim.local(0x29, 1, v29, None)
    sim.local(0x12, 1, b, None)
    sim.local(0x13, 1, a, None)
    sim.local(0x28, 1, v28, None)
    sim.local(0x26, 1, v26, None)
    sim.local(0x27, 1, v27, None)
    sim.local(0x2B, 1, v2b, None)
    sim.local(0x24, 2, flags, None)
    sim.local(0x2A, 1, v2a, None)
    sim.local(0x2C, 1, v2c, None)
    sim.local(0x2D, 1, v2d, None)
    # texturas (el banco las sustituye por falsos con puntero 0x4E000000 + ((ordinal+1)<<16) + (fnv32(nombre) & 0xFFFF))
    names = []
    for k, nm in enumerate((corona, shadow)):
        nm = nm.split(b'\0')[0]
        names.append(nm)
        sim.e.put(0x30 + 4 * k, le32(0x4E000000 + ((k + 1) << 16) + (fnv32(nm) & 0xFFFF)))
    if flags.state == 'ok' and not flags.sens:
        if flags.v & 4:
            sim.e.b[0x24] &= 0xFD
            sim.mark('light_flag4')
        else:
            sim.mark('light_noflag4_hibits', (flags.v & 0x400) != 0)
    else:
        sim.e.w[0x24] = True
    sim.mark('light_empty_name', len(names[0]) == 0 or len(names[1]) == 0)
    sim.mark('light_long_name', len(names[0]) >= 60 or len(names[1]) >= 60)
    sim.mark('light_tail_corona', corona.startswith(b'tail_'))
    sim.mark('light_tail_shadow', shadow.startswith(b'tail_'))
    sim.mark('light_adjacent', adjacent and len(corona) > 0)
    return names


def run_branch(sim, branch, line, tail, fields):
    """Devuelve (tex_names, malloc_called)."""
    e = sim.e
    names, malloc = [], False
    if branch == 0:
        raise Fail('interno')
    if branch == 1:
        f = scan(line, ['d', 'f', 'f', 'f', 'd', 's'])
        if f[5].state == 'ok':
            e.put(0x10, f[5].v.encode('latin-1') + b'\0')
        sim.mark('part_name', f[5].state == 'ok')
        sim.mark('part_missing', f[5].state != 'ok')
        if f[5].state == 'ok':
            n = len(f[5].v)
            sim.mark('part_len_1_7', n <= 7)
            sim.mark('part_len_8_47', 8 <= n <= 47)
            sim.mark('part_len_48p', n >= 48)
            sim.mark('part_len_63p', n >= 63)
    elif branch == 3:
        f = scan(line, ['d', 'f', 'f', 'f', 'd', 'd'] + ['f'] * 9 + ['d', 'd', 's'])
        need(f[5:], ['v34'] + ['f'] * 9 + ['v35', 'v36', 'name'])
        for i in range(9):
            sim.direct32(0x10 + 4 * i, f[6 + i])
        sim.local(0x34, 1, f[5], None)
        sim.local(0x35, 1, f[15], None)
        sim.local(0x36, 1, f[16], None)
        nm = f[17].v.encode('latin-1')
        e.put(0x38, nm + b'\0')
        sim.mark('attr_name_ge8', len(nm) >= 8)
        sim.mark('attr_name_ge31', len(nm) >= 31)
        sim.mark('attr_name_ge64', len(nm) >= 64)
    elif branch == 5:
        f = scan(line, ['d', 'f', 'f', 'f', 'd', 'd', 'f', 'f', 'f', 'f'] + ['d'] * 28)
        sim.direct32(0x10, f[5])
        for k, off in enumerate((0x12, 0x13, 0x14)):
            fld = f[6 + k]
            sim.local(off, 1, fld, None, conv=ftol_byte)
            sim.mark('road_size_ge256', fld.state == 'ok' and not fld.sens and abs(fl(fld.v)) >= 256)
            sim.mark('road_size_neg', fld.state == 'ok' and not fld.sens and fl(fld.v) < 0)
            sim.mark('road_size_huge', fld.state == 'ok' and not fld.sens and abs(fl(fld.v)) >= 2.0 ** 63)
            sim.mark('road_size_frac_edge', fld.state == 'ok' and fld.sens)
        sim.direct32(0x30, f[9])
        order = [(0x15, None), (0x2E, None), (0x11, -1), (0x2F, 0x32), (0x16, -1), (0x17, -1), (0x1A, -1), (0x1B, -1), (0x18, -1), (0x19, -1), (0x1C, -1), (0x1D, -1),
                 (0x20, -1), (0x21, -1), (0x1E, -1), (0x1F, -1), (0x22, -1), (0x25, -1), (0x28, -1), (0x2B, -1), (0x23, -1), (0x26, -1), (0x29, -1), (0x2C, -1),
                 (0x24, -1), (0x27, -1), (0x2A, -1), (0x2D, -1)]
        for i, (off, dflt) in enumerate(order):
            sim.local(off, 1, f[10 + i], dflt)
        last = max([i for i in range(28) if f[10 + i].state == 'ok'] or [-1])
        sim.mark('road_full', last == 27)
        sim.mark('road_truncated', last < 27)
        sim.mark('road_default13', f[10 + 3].state != 'ok')
        sim.mark('road_trunc_early', last <= 2)
        sim.mark('road_masked', sim.e.mask != 0)
        sim.mark('road_exotic_int_pos', any(x.state == 'unparsed' for x in f[10:]) and sim.e.mask == 0 and f[9].state == 'ok')
    elif branch == 6:
        f = scan(line, ['d', 'f', 'f', 'f', 'd'] + ['f'] * 7 + ['d', 'd', 's', 'd'])
        need(f[5:], ['f'] * 7 + ['v2c', 'v2e', 'name', 'v2f'])
        for i in range(7):
            sim.direct32(0x10 + 4 * i, f[5 + i])
        sim.local(0x2C, 2, f[12], None)
        sim.local(0x2E, 1, f[13], None)
        sim.local(0x2F, 1, f[15], None)
        nm = f[14].v.encode('latin-1')[:8]
        e.put(0x30, nm + b'\0' * (8 - len(nm)))
        sim.mark('slot_name_ge8', len(f[14].v) >= 8)
        sim.mark('slot_name_ge31', len(f[14].v) >= 31)
        sim.mark('slot_word_hi', f[12].state == 'ok' and not f[12].sens and not (0 <= f[12].v <= 255))
    elif branch == 7:
        f = scan(line, ['d', 'f', 'f', 'f', 'd'] + ['f'] * 5 + ['d', 's', 's', 's', 's'])
        need(f[5:], ['f'] * 5 + ['f1a4'])
        for i in range(5):
            sim.direct32(0x10 + 4 * i, f[5 + i])
        sim.local(0x24, 2, f[10], None)
        malloc = True
        block = bytearray()
        count = 0
        for i in range(4):
            fld = f[11 + i]
            if fld.state == 'ok':
                buf = (fld.v.encode('latin-1') + b'\0' * 34)[:16]
                if len(fld.v) > 32:
                    raise Fail('nombre de interior > 32 caracteres')
                count += 1
                sim.mark('int_name_ge16', len(fld.v) >= 16)
                sim.mark('int_name_ge32', len(fld.v) >= 31)
            else:
                buf = (b' ' + b'\0' * 33)[:16]
            block += buf
        sim.mark('int_names%d' % count)
        e.put(0x28, le32(BLOCK_LABEL))
        e.put(0x2C, le32(0))
        return names, malloc, bytes(block)
    elif branch == 8:
        f = scan(line, ['d', 'f', 'f', 'f', 'd', 'd'])
        need(f[5:], ['v'])
        sim.local(0x10, 4, f[5], None)
    elif branch == 9:
        f = scan(line, ['d', 'f', 'f', 'f', 'd', 'f', 'f', 'd'])
        need(f[5:], ['f0', 'f1', 'v'])
        sim.local(0x10, 4, f[5], None)
        sim.local(0x14, 4, f[6], None)
        sim.local(0x18, 1, f[7], None)
    elif branch == 10:
        f = scan(line, ['d', 'f', 'f', 'f', 'd'] + ['f'] * 9 + ['d'])
        need(f[5:], ['f'] * 9 + ['v'])
        for i in range(9):
            sim.local(0x10 + 4 * i, 4, f[5 + i], None)
        v = f[14]
        if v.sens:
            sim.e.w[0x34] = True
        else:
            sim.e.put(0x34, bytes([1 if v.v != 0 else 0]))
            sim.mark('t10_true', v.v != 0)
            sim.mark('t10_false', v.v == 0)
    return names, malloc, None


def expected_load(inp, case):
    pre_e = bytes.fromhex(inp['pre_entry'])
    pre_o = bytes.fromhex(inp['pre_over'])
    if len(pre_e) != 64 or len(pre_o) != 128:
        raise Fail('pre inconsistente')
    k = inp['store_k']
    line, tail = inp['line'], inp['tail']
    for s in (line, tail):
        if any(ord(c) > 0x7E or (ord(c) < 0x20 and c != '\t') for c in s):
            raise Fail('linea con caracteres fuera de ASCII imprimible')
    global CUR
    ent = Ent(pre_e, pre_o, inp['mask'])
    sim = Sim(ent)
    st = sim.stats
    CUR = st
    st['g%d' % inp['group']] = True
    st['fam%d' % inp['family']] = True
    f1 = scan(line, ['d', 'f', 'f', 'f', 'd'])
    if inp['group'] == 0:
        f1 = scan(line, ['d', 'f', 'f', 'f', 'd', 'd', 'd', 'd', 'd'])
    for i, nm in enumerate(('id', 'x', 'y', 'z', 'type')):
        if f1[i].state != 'ok':
            raise Fail('los cinco primeros campos deben leerse: falta ' + nm)
    mid = f1[0]
    if mid.sens or not (0 <= mid.v < MODEL_TABLE) or mid.v != inp['model_id']:
        raise Fail('modelId inconsistente: %r vs %r' % (mid.v, inp['model_id']))
    for i, off in enumerate((0, 4, 8)):
        if f1[1 + i].sens:
            ent.wild(off, 4)
            sim.mark('crt_sens_value')
        else:
            ent.put(off, le32(f1[1 + i].v))
    tfield = f1[4]
    typ_known = not tfield.sens
    trace = [[1, 0, 0, 0], [2, fnv32(b'particle'), 8, 0], [3, inp['find_result'] & M32, 0, 0], [4, 0, 0, 0],
             [5, 0xFFFF if inp['mi_label'] == 15 else inp['mi_label'], 4 + 0x40 * k, k + 1]]
    tex, block_hex, wildtrace = [], (bytes([BLOCK_PRE]) * 64).hex(), False
    if typ_known:
        t = tfield.v
        ent.put(0xC, bytes([t & 0xFF]))
        st['type_oob'] = (t & M32) > 10
        st['type_ge256'] = (t & M32) > 255 and (t & M32) <= 0x7FFFFFFF
        st['type_neg'] = t < 0
        st['type_byte_differs'] = (t & M32) > 10 and (t & M32) != (t & 0xFF)
        st['type_lowbyte_valid'] = (t & M32) > 255 and (t & 0xFF) <= 10
        branch = t if 0 <= t <= 10 else -1
        if branch in (2, 4, -1):
            pass
        elif branch == 0:
            names = light(sim, line, tail, f1)
            tex = names
            trace += [[6, 0, fnv32(names[0]), 1], [6, 1, fnv32(names[1]), 1]]
        else:
            names, malloc, block = run_branch(sim, branch, line, tail, f1)
            if malloc:
                trace.append([7, 0x40, 0, 0])
                block_hex = block.hex()
        st['branch%d' % branch] = True
    else:
        ent.wild(0xC, 1)
        ent.wild(0xD, 0x40 - 0xD)
        wildtrace = True
        st['type_sens'] = True
    trace.append([8, 0, 0, 0])
    ent.finish()
    eh, oh = ent.hexes()
    want = {'entry': eh, 'over': oh, 'count': k + 1, 'outside_ok': True, 'first_outside': -1,
            'trace': None if wildtrace else trace, 'find_name': b'particle'.hex(),
            'tex': None if wildtrace else [[nm.hex(), 1] for nm in tex],
            'block': None if wildtrace else block_hex, 'block_guard_ok': True, 'table_ok': True, 'line_ok': True}
    st['crt_wild_bytes'] = any(ent.w)
    return want, st


def expected(function, inp, case):
    if function != NAME:
        raise RuntimeError('Funcion inesperada ' + function)
    try:
        return expected_load(inp, case)
    except Fail as e:
        raise RuntimeError('Fixture invalido en el caso %d (%s): %s' % (case, inp.get('line', '')[:120], e))


def compare(obs, want):
    """Lista de claves en las que lo observado difiere de lo esperado (comodines '??' y None = sin expectativa)."""
    bad = []
    for key, w in want.items():
        if w is None:
            continue
        o = obs.get(key)
        if key in ('entry', 'over'):
            if not isinstance(o, str) or len(o) != len(w) or any(w[i:i + 2] != '??' and w[i:i + 2] != o[i:i + 2] for i in range(0, len(w), 2)):
                bad.append(key)
        elif o != w:
            bad.append(key)
    return bad


# ---------------------------------------------------------------------------------------------------- autoprueba
def _inp(line, group, family=0, k=3, label=2, mask=0, tail='"tail_sh" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19', model=42, pre=None):
    pre = pre if pre is not None else bytes(range(1, 65))
    return dict(group=group, family=family, line=line, tail=tail, model_id=model, mi_label=label, store_k=k, pre_entry=pre.hex(), pre_over=(b'\xEE' * 128).hex(),
                find_result=7, mask=mask, crt=0)


def _selftest():
    # 1) redondeo decimal -> float correcto y casos limite
    for s_, bits in (('1', 0x3F800000), ('0.1', 0x3DCCCCCD), ('16777217', 0x4B800000), ('1.000000059604644775390625', 0x3F800000), ('-2.5', 0xC0200000), ('1e-40', 0x000116C2)):
        got = round_f32(Fraction(s_))
        assert got == bits, (s_, hex(got), hex(bits))
    import random
    rnd = random.Random(7)
    for _ in range(20000):
        x = rnd.uniform(-1e6, 1e6) * 10 ** rnd.randint(-6, 6)
        assert round_f32(Fraction(x)) == f32bits(x), repr(x)       # Fraction(double) es exacto: debe coincidir con el redondeo al par de struct
    assert round_f32(Fraction('1.00000005960464477539062500000000000000000000000001')) == 0x3F800001
    assert round_f32(Fraction('1.00000005960464477539062499999999999999999999999999')) == 0x3F800000
    # 2) ftol
    for v, b in ((255.9, 255), (256.0, 0), (-1.0, 255), (-0.5, 0), (1e19, 0), (3e9, 0x00), (65543.0, 7), (-256.5, 0)):
        assert ftol_byte(f32bits(v)) == b, (v, ftol_byte(f32bits(v)), b)
    assert ftol_byte(0x7FC00000) == 0 and ftol_byte(0x7F800000) == 0
    # 3) scan
    f = scan('12 1.5e2 -.5 7. 3.9 abc', ['d', 'f', 'f', 'f', 'd', 'd', 'd'])
    assert [x.state for x in f] == ['ok'] * 5 + ['unparsed'] * 2
    assert f[1].v == f32bits(150.0) and f[2].v == f32bits(-0.5) and f[3].v == f32bits(7.0) and f[4].v == 3
    f = scan('1 inf 2', ['d', 'f', 'd'])
    assert [x.state for x in f] == ['ok', 'unknown', 'unknown']
    f = scan('99999999999 16777217.00000000000000000001', ['d', 'f'])
    assert f[0].sens and f[1].sens
    f = scan('0x1F', ['d', 'd'])
    assert f[0].v == 0 and f[1].state == 'unparsed'
    # 4) particula con nombre y sin nombre
    want, st = expected(NAME, _inp('42 1.5 -2 3 1 flare_big', 1), 0)
    e = bytearray(bytes(range(1, 65)))
    e[0:4] = struct.pack('<f', 1.5); e[4:8] = struct.pack('<f', -2.0); e[8:12] = struct.pack('<f', 3.0); e[12] = 1; e[0x10:0x10 + 10] = b'flare_big\0'
    assert want['entry'] == bytes(e).hex() and want['count'] == 4, want['entry']
    assert want['trace'][4] == [5, 2, 4 + 0x40 * 3, 4] and want['trace'][-1] == [8, 0, 0, 0]
    want, st = expected(NAME, _inp('42 1.5 -2 3 1', 1), 0)
    assert want['entry'][0x20 * 1:0x20 * 1 + 2 * 4] == bytes(range(0x11, 0x15)).hex()
    # 5) tipo fuera de rango: 256 -> byte 0 y rama por defecto ; 11 ; -1
    for typ, byte in ((256, 0), (257, 1), (11, 11), (255, 255), (-1, 255), (65536 + 3, 3), (2147483647, 255)):
        want, st = expected(NAME, _inp('42 1 2 3 %d 5 6 7' % typ, 11), 0)
        assert want['entry'][24:26] == '%02x' % byte and st['type_oob'] and want['tex'] == [] and want['block'] == ('b7' * 64), (typ, want['entry'])
    want, st = expected(NAME, _inp('42 1 2 3 4294967295 5 6 7', 11), 0)          # envoltura -> -1
    assert want['entry'][24:26] == '??' and want['trace'] is None
    want, st = expected(NAME, _inp('42 1 2 3 2', 2), 0)                          # tipo 2: solo +0..0xC
    assert want['entry'][26:] == bytes(range(14, 65)).hex()
    # 6) interior: 2 nombres, relleno de 16 bytes
    want, st = expected(NAME, _inp('42 1 2 3 7 1.0 2.0 3.0 4.0 5.0 65537 abc 0123456789ABCDEFXYZ', 7), 0)
    exp = (b'abc' + b'\0' * 13) + b'0123456789ABCDEF' + (b' ' + b'\0' * 15) * 2
    assert want['block'] == exp.hex() and want['trace'][-2] == [7, 0x40, 0, 0] and want['entry'][2 * 0x24:2 * 0x26] == '0100'
    assert want['entry'][2 * 0x28:2 * 0x2C] == le32(BLOCK_LABEL).hex() and want['entry'][2 * 0x2C:2 * 0x30] == '00000000'
    # 7) luz: dos nombres, flags con bit 4
    ln = '42 1 2 3 0 255 -1 256 7 "corona a" "sombra" 1.5 2.5 3.5 4.5 1 2 3 4 22 6 7 8 9'
    want, st = expected(NAME, _inp(ln, 0), 0)
    ent = bytes.fromhex(want['entry'])
    assert ent[0x10] == 255 and ent[0x11] == 255 and ent[0x12] == 0 and ent[0x13] == 7 and ent[0x24] == 20 and ent[0x25] == 0 and st['light_flag4']
    assert struct.unpack('<I', ent[0x30:0x34])[0] == 0x4E010000 + (fnv32(b'corona a') & 0xFFFF)
    assert want['trace'][5] == [6, 0, fnv32(b'corona a'), 1] and want['tex'] == [[b'corona a'.hex(), 1], [b'sombra'.hex(), 1]]
    # luz sin comillas: cola con dos pares
    ln = '42 1 2 3 0 1 2 3 4 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19'
    want, st = expected(NAME, _inp(ln, 0, tail='"tail_cor" "tail_sh" 1.5 2.5 3.5 4.5 11 12 13 14 15 16 17 18 19'), 0)
    assert want['tex'] == [[b'tail_cor'.hex(), 1], [b'tail_sh'.hex(), 1]] and st['light_tail_corona'] and st['light_tail_shadow']
    # 8) cartel: valores por defecto, truncado y mascara
    toks = ' '.join(['7', '255.9', '256', '-1', '1.25', '9', '8'])
    want, st = expected(NAME, _inp('42 1 2 3 5 ' + toks, 5), 0)
    ent = bytes.fromhex(want['entry'])
    assert ent[0x10] == 7 and ent[0x12] == 255 and ent[0x13] == 0 and ent[0x14] == 255 and ent[0x15] == 9 and ent[0x2E] == 8
    assert ent[0x11] == 0xFF and ent[0x2F] == 0x32 and ent[0x2D] == 0xFF and ent[0x24] == 0xFF and struct.unpack('<f', ent[0x30:0x34])[0] == 1.25 and st['road_truncated'] and st['road_default13']
    try:
        expected(NAME, _inp('42 1 2 3 5 7 1 2 3 1.25', 5), 0)
        raise AssertionError('debia rechazar: a10/a11 sin leer y sin mascara')
    except RuntimeError:
        pass
    want, st = expected(NAME, _inp('42 1 2 3 5 7 1 2 3 inf 4 5 6', 5, mask=(1 << 0x15) | (1 << 0x2E)), 0)
    assert want['entry'][2 * 0x30:2 * 0x34] == '????????' and want['entry'][2 * 0x15:2 * 0x15 + 2] == '00' and want['entry'][2 * 0x11:2 * 0x11 + 2] == '??' and st['crt_unknown']
    # 9) tragaperras: strncpy de 8
    want, st = expected(NAME, _inp('42 1 2 3 6 1 2 3 4 5 6 7 65537 258 abcdefghij 300', 6), 0)
    ent = bytes.fromhex(want['entry'])
    assert ent[0x2C:0x2E] == b'\x01\x00' and ent[0x2E] == 2 and ent[0x2F] == 44 and ent[0x30:0x38] == b'abcdefgh' and ent[0x38:0x40] == bytes(range(57, 65))
    want, st = expected(NAME, _inp('42 1 2 3 6 1 2 3 4 5 6 7 1 2 ab 3', 6), 0)
    assert bytes.fromhex(want['entry'])[0x30:0x38] == b'ab' + b'\0' * 6
    # 10) atractor: nombre largo desborda a la entrada siguiente
    want, st = expected(NAME, _inp('42 1 2 3 3 5 1 2 3 4 5 6 7 8 9 10 11 ' + 'n' * 20, 3), 0)
    ent = bytes.fromhex(want['entry'])
    assert ent[0x34:0x37] == bytes([5, 10, 11]) and ent[0x38:0x40] == b'n' * 8 and bytes.fromhex(want['over'])[:13] == b'n' * 12 + b'\0' and st['attr_name_ge8']
    # 11) tipo 10: v != 0 ; tipo 8, 9
    want, st = expected(NAME, _inp('42 1 2 3 10 1 2 3 4 5 6 7 8 9 4294967296', 10), 0)
    assert want['entry'][2 * 0x34:2 * 0x35] == '??'
    want, st = expected(NAME, _inp('42 1 2 3 10 1 2 3 4 5 6 7 8 9 256', 10), 0)
    assert want['entry'][2 * 0x34:2 * 0x35] == '01'
    want, st = expected(NAME, _inp('42 1 2 3 9 1.5 -2 258', 9), 0)
    ent = bytes.fromhex(want['entry'])
    assert struct.unpack('<ff', ent[0x10:0x18]) == (1.5, -2.0) and ent[0x18] == 2
    want, st = expected(NAME, _inp('42 1 2 3 8 -7', 8), 0)
    assert struct.unpack('<i', bytes.fromhex(want['entry'])[0x10:0x14])[0] == -7
    # 12) compare con comodines
    assert compare({'entry': 'aabb', 'x': 1}, {'entry': 'aa??', 'x': 1}) == [] and compare({'entry': 'aabb', 'x': 2}, {'entry': 'ab??', 'x': 1}) == ['entry', 'x']
    print('ok')


if __name__ == '__main__':
    _selftest()
