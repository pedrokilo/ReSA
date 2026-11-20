"""Oraculo independiente (Python puro) del banco LIB3A: lee las trazas trace_<funcion>.bin que escribe bench.exe y comprueba cada caso contra la
especificacion del algoritmo (formulas de JFIF, interpolacion triangular de libjpeg, DCT 8x8 exacta en doble precision).
No usa ninguna parte de la biblioteca ni del EXE. Uso: python -I oraculo.py <carpeta de trazas>   (sale con 1 si algun caso falla)."""
import struct, sys, os, math

def f32(x): return struct.unpack('<f', struct.pack('<f', x))[0]  # x87 con control de precision de 24 bits: cada operacion se redondea a float

def fix(x): return int(x * 65536 + 0.5)
ONE_HALF = 1 << 15
CBCR = (128 << 16) + ONE_HALF - 1

def ycc_from_rgb(r, g, b):
    y = (fix(0.299) * r + fix(0.587) * g + fix(0.114) * b + ONE_HALF) >> 16
    cb = (-fix(0.16874) * r - fix(0.33126) * g + fix(0.5) * b + CBCR) >> 16
    cr = (fix(0.5) * r - fix(0.41869) * g - fix(0.08131) * b + CBCR) >> 16
    return y, cb, cr

def records(path):
    d = open(path, 'rb').read(); o = 0
    while o < len(d):
        n = struct.unpack_from('<I', d, o)[0]; yield d[o + 4:o + 4 + n]; o += 4 + n

class Rd:
    def __init__(self, b, o=0): self.b = b; self.o = o
    def u32(self): v = struct.unpack_from('<I', self.b, self.o)[0]; self.o += 4; return v
    def raw(self, n): v = self.b[self.o:self.o + n]; self.o += n; return v

def clamp(v): return 0 if v < 0 else 255 if v > 255 else v

def t_rgb_ycc_start(r, ec):
    tab = struct.unpack('<2048i', r.raw(8192))
    exp = [0] * 2048
    for i in range(256):
        exp[i] = fix(0.299) * i; exp[i + 256] = fix(0.587) * i; exp[i + 512] = fix(0.114) * i + ONE_HALF
        exp[i + 768] = -fix(0.16874) * i; exp[i + 1024] = -fix(0.33126) * i; exp[i + 1280] = fix(0.5) * i + CBCR
        exp[i + 1536] = -fix(0.41869) * i; exp[i + 1792] = -fix(0.08131) * i
    return list(tab) == exp and ec == 0

def t_color_compress(kind):
    def f(r, ec):
        W, rows, orow, nin, nout = r.u32(), r.u32(), r.u32(), r.u32(), r.u32()
        inp = [r.raw(W * nin) for _ in range(rows)]
        out = [[r.raw(W) for _ in range(rows)] for _ in range(nout)]
        for i in range(rows):
            for x in range(W):
                p = inp[i][x * nin:(x + 1) * nin]
                if kind == 'ycc': e = ycc_from_rgb(p[0], p[1], p[2])
                elif kind == 'gray': e = (ycc_from_rgb(p[0], p[1], p[2])[0],)
                elif kind == 'cmyk':
                    y, cb, cr = ycc_from_rgb(255 - p[0], 255 - p[1], 255 - p[2]); e = (y, cb, cr, p[3])
                elif kind == 'grayc': e = (p[0],)
                else: e = tuple(p[:nout])
                for ci in range(nout):
                    if out[ci][i][x] != e[ci]: return False
        return ec == 0
    return f

def t_start_fdct(r, ec):
    nc, method, _ = r.u32(), r.u32(), r.u32()
    qno = [struct.unpack('<i', r.raw(4))[0] for _ in range(4)]
    pres = []; qv = []
    for _ in range(4):
        pres.append(r.u32()); qv.append(struct.unpack('<64H', r.raw(128)) if pres[-1] else None)
    got = []
    for _ in range(4):
        dp = r.u32(); di = struct.unpack('<64i', r.raw(256)) if dp else None
        fp = r.u32(); fl = r.raw(256) if fp else None
        got.append((dp, fl))
    aan = [1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379]
    err = False; exp = [None] * 4
    for ci in range(nc):
        q = qno[ci]
        if q < 0 or q >= 4 or not pres[q]: err = True; break
        if method != 2: err = True; break
        if exp[q] is None:
            exp[q] = b''.join(struct.pack('<f', f32(1.0 / f32(f32(f32(qv[q][row * 8 + col] * aan[row]) * aan[col]) * 8.0))) for row in range(8) for col in range(8))
    if err != bool(ec): return False
    for i in range(4):
        if got[i][0]: return False  # el EXE solo trae DCT flotante: nunca divisores enteros
        if (exp[i] is None) != (got[i][1] is None): return False
        if exp[i] is not None and exp[i] != got[i][1]: return False
    return True

COS = [[math.cos((2 * x + 1) * u * math.pi / 16) for x in range(8)] for u in range(8)]
def dct2(f):  # f: 64 valores en orden fila-columna; DCT-II ortonormal de JPEG
    out = [0.0] * 64
    for u in range(8):
        cu = 1 / math.sqrt(2) if u == 0 else 1.0
        for v in range(8):
            cv = 1 / math.sqrt(2) if v == 0 else 1.0
            s = 0.0
            for y in range(8):
                for x in range(8): s += f[y * 8 + x] * COS[u][y] * COS[v][x]
            out[u * 8 + v] = 0.25 * cu * cv * s
    return out
AAN = [1.0, 1.387039845, 1.306562965, 1.175875602, 1.0, 0.785694958, 0.541196100, 0.275899379]
STAT = {'max_abs_fdct': 0.0, 'dct_off1': 0, 'dct_total': 0, 'dct_exact': 0}

def t_fdct_float(r, ec):
    inp = struct.unpack('<64f', r.raw(256)); out = struct.unpack('<64f', r.raw(256))
    t = dct2(inp)
    for u in range(8):
        for v in range(8):
            e = t[u * 8 + v] * AAN[u] * AAN[v] * 8.0
            d = abs(e - out[u * 8 + v]); STAT['max_abs_fdct'] = max(STAT['max_abs_fdct'], d)
            if d > 0.02 + 1e-4 * abs(e): return False
    return ec == 0

def t_forward_dct(r, ec):
    nb = r.u32(); q = struct.unpack('<64H', r.raw(128))
    samples = [[r.raw(8) for _ in range(8)] for _ in range(nb)]
    out = [struct.unpack('<64h', r.raw(128)) for _ in range(nb)]
    for b in range(nb):
        f = [samples[b][y][x] - 128 for y in range(8) for x in range(8)]
        t = dct2(f)
        for i in range(64):
            e = t[i] / q[i]; STAT['dct_total'] += 1
            ex = math.floor(e + 0.5)
            if out[b][i] == ex: STAT['dct_exact'] += 1
            elif abs(out[b][i] - ex) <= 1 and abs(abs(e - math.floor(e)) - 0.5) < 1e-3: STAT['dct_off1'] += 1  # empate en .5: cualquiera de los dos
            else: return False
    return ec == 0


STAT.update({'idct_total': 0, 'idct_exact': 0, 'idct_off1': 0, 'idct_checked': 0})
def idct_exact(f):  # IDCT 8x8 ortonormal de JPEG en doble precision; f en orden fila(v)-columna(u)
    out = [0.0] * 64
    for y in range(8):
        for x in range(8):
            s = 0.0
            for v in range(8):
                cv = 1 / math.sqrt(2) if v == 0 else 1.0
                for u in range(8):
                    cu = 1 / math.sqrt(2) if u == 0 else 1.0
                    s += cu * cv * f[v * 8 + u] * COS[v][y] * COS[u][x]
            out[y * 8 + x] = 0.25 * s
    return out
def t_idct_float(r, ec):
    mode = r.u32(); q = struct.unpack('<64H', r.raw(128)); tab = struct.unpack('<64f', r.raw(256)); coef = struct.unpack('<64h', r.raw(128))
    out = [r.raw(8) for _ in range(8)]
    if mode == 5: return ec == 0   # tabla con NaN/inf/denormales: solo EXE vs biblioteca
    f = [coef[i] * q[i] for i in range(64)]
    t = idct_exact(f)
    if max(abs(v) for v in t) > 380: STAT['idct_skipped'] = STAT.get('idct_skipped', 0) + 1; return ec == 0   # fuera del dominio de la tabla de recorte (se enrolla con & RANGE_MASK): solo EXE vs biblioteca
    STAT['idct_checked'] += 1
    for y in range(8):
        for x in range(8):
            e = t[y * 8 + x] + 128; lo = clamp(int(math.floor(e + 0.5))); o = out[y][x]; STAT['idct_total'] += 1
            if o == lo: STAT['idct_exact'] += 1
            elif abs(o - lo) <= 1: STAT['idct_off1'] += 1   # truncacion float + (x+4)>>3 de la biblioteca frente al redondeo exacto
            else: return False
    return ec == 0

# ---------------- LIB3B: funciones de estado y de flujo (oraculo = especificacion de la fuente 6b escrita en Python; no usa la biblioteca)
def t_emit_message(r, ec):
    lvl, tr, nw0, nw1, out = [struct.unpack('<i', r.raw(4))[0] for _ in range(5)]
    if lvl < 0: eo = 1 if (nw0 == 0 or tr >= 3) else 0; en = nw0 + 1
    else: eo = 1 if tr >= lvl else 0; en = nw0
    return ec == 0 and out == eo and nw1 == en
def t_reset_error(r, ec):
    a, b = r.u32(), r.u32(); return ec == 0 and a == 0 and b == 0
import re as _re
def c_format(fmt, args):
    """sprintf de C para el subconjunto %d %x %X %s %c %5d %-5d %05d %%, con argumentos sobrantes ignorados"""
    out = []; i = 0; k = 0
    while i < len(fmt):
        ch = fmt[i]
        if ch != '%': out.append(ch); i += 1; continue
        m = _re.match(r'%(-?)(0?)(\d*)([dxXsc%])', fmt[i:])
        if not m: out.append(fmt[i:]); break
        flag, zero, wid, conv = m.groups(); i += len(m.group(0))
        if conv == '%': out.append('%'); continue
        a = args[k]; k += 1
        if conv == 'd': t = str(((a + 2**31) % 2**32) - 2**31)
        elif conv in 'xX': t = format(a % 2**32, conv)
        elif conv == 'c': t = chr(a & 255)
        else: t = a
        w = int(wid) if wid else 0
        if flag: t = t.ljust(w)
        elif zero and conv == 'd': t = ('-' + t[1:].rjust(w - 1, '0')) if t.startswith('-') else t.rjust(w, '0')
        else: t = t.rjust(w)
        out.append(t)
    return ''.join(out)
def t_format_message(r, ec):
    nm, lastj, na, haveadd, firsta, lasta, code = [struct.unpack('<i', r.raw(4))[0] for _ in range(7)]
    tab = []; 
    for _ in range(nm): n = r.u32(); tab.append(r.raw(n).decode('latin-1'))
    add = []
    for _ in range(na):
        n = r.u32(); add.append(None if n == 0xFFFFFFFF else r.raw(n).decode('latin-1'))
    ints = list(struct.unpack('<8i', r.raw(32))); sraw = r.raw(80); n = r.u32(); got = r.raw(n).decode('latin-1')
    txt = None
    if 0 < code <= lastj: txt = tab[code] if code < nm else None
    elif haveadd and firsta <= code <= lasta: txt = add[code - firsta]
    if txt is None: txt = tab[0]   # codigo no valido: msg_parm.i[0] = msg_code (ya reflejado en ints)
    s = sraw.split(b'\0')[0].decode('latin-1')
    isstr = False
    j = txt.find('%')
    if j >= 0 and j + 1 < len(txt) and txt[j + 1] == 's': isstr = True
    exp = c_format(txt, [s] if isstr else ints)
    exp = exp.split(chr(0))[0]   # el resultado es una cadena C: termina en el primer NUL (%c con 0)
    return ec == 0 and got == exp
def t_finish_output(r, ec):
    q, pn0, pn1, cnt = r.u32(), r.u32(), r.u32(), r.u32()
    return ec == 0 and pn1 == (pn0 + 1) % 2**32 and cnt == (1 if q & 0xFF else 0)
def t_finish_master(r, ec):
    pt0, pn0, sn0, opt, pt1, pn1, sn1, cnt = [r.u32() for _ in range(8)]
    opt &= 0xFF
    ept, esn = pt0, sn0
    if pt0 == 0: ept = 2; esn = sn0 + (0 if opt else 1)
    elif pt0 == 1: ept = 2
    elif pt0 == 2:
        if opt: ept = 1
        esn = sn0 + 1
    return ec == 0 and cnt == 1 and pt1 == ept and sn1 == esn and pn1 == pn0 + 1
def t_marker_bytes(r, ec):
    kind, val, mk, ln, off0, free0, fl, off1, free1, calls = [r.u32() for _ in range(10)]
    out = bytearray(r.raw(24)); fl &= 0xFF
    base = bytearray([0x55] * 64)
    # simulacion del bufer de salida: 'out' (64 bytes visibles) y el bufer de reserva (no visible)
    pos = off0; free = free0; vis = True; cal = 0; err = False
    def emit(b):
        nonlocal pos, free, vis, cal, err
        if vis: base[pos] = b; pos += 1
        free -= 1
        if free == 0:
            cal += 1; vis = False; free = 7
            if not fl: err = True
    if kind == 0: emit(val & 255)
    else:
        if ln > 65533: err = True
        else:
            for b in (0xFF, mk, ((ln + 2) >> 8) & 255, (ln + 2) & 255):
                emit(b)
                if err: break
    if (ec != 0) != err: return False
    if err and kind == 1 and ln > 65533: return calls == 0 and bytes(base[:24]) == bytes(out)
    return calls == cal and bytes(base[:24]) == bytes(out)
def t_start_prep(r, ec):
    mode, ih, rtg, nbr = [r.u32() for _ in range(4)]
    if mode != 0: return ec != 0
    return ec == 0 and rtg == ih and nbr == 0
def t_reset_marker(r, ec):
    ci, sn, um, w0, w14 = [r.u32() for _ in range(5)]
    return ec == 0 and ci == 0 and sn == 0 and um == 0 and (w0 & 0xFFFF) == 0 and w14 == 0
def t_dpost(r, ec):
    mode, q, sr, nr = [r.u32() for _ in range(4)]
    if mode != 0: return ec != 0
    return ec == 0 and sr == 0 and nr == 0

def t_start_up(r, ec):
    maxv, oh, nro, rtg = r.u32(), r.u32(), r.u32(), r.u32()
    return nro == maxv and rtg == oh and ec == 0

def fancy_h2v1(row, D):
    o = []
    for x in range(D):
        v = row[x] * 3
        if D == 1: o += [row[x], row[x]]; continue
        if x == 0: o += [row[0], (v + row[1] + 2) >> 2]
        elif x == D - 1: o += [(v + row[x - 1] + 1) >> 2, row[x]]
        else: o += [(v + row[x - 1] + 1) >> 2, (v + row[x + 1] + 2) >> 2]
    return o

def fancy_h2v2(this, other, D):
    cs = [this[x] * 3 + other[x] for x in range(D)]; o = []
    for x in range(D):
        if x == 0: o += [(cs[0] * 4 + 8) >> 4, (cs[0] * 3 + cs[1] + 7) >> 4]
        elif x == D - 1: o += [(cs[x] * 3 + cs[x - 1] + 8) >> 4, (cs[x] * 4 + 7) >> 4]
        else: o += [(cs[x] * 3 + cs[x - 1] + 8) >> 4, (cs[x] * 3 + cs[x + 1] + 7) >> 4]
    return o

def t_upsample(r, ec):
    kind, W, D, maxv, hx, vx, inrows, ctx = [r.u32() for _ in range(8)]
    rows = [r.raw(D) for _ in range(inrows + 2 * ctx)]; eqin, isnull = r.u32(), r.u32()
    if ec: return False
    if kind == 0: return eqin == 1
    if kind == 1: return isnull == 1
    out = [r.raw(W) for _ in range(maxv)]
    for o in range(maxv):
        if kind == 2: e = [rows[o // vx][x // hx] for x in range(W)]
        elif kind == 3: e = [rows[o][x // 2] for x in range(W)]
        elif kind == 4: e = [rows[o // 2][x // 2] for x in range(W)]
        elif kind == 5: e = fancy_h2v1(rows[o], D)[:W]
        else:
            i = o // 2 + ctx; other = rows[i - 1] if o % 2 == 0 else rows[i + 1]
            e = fancy_h2v2(rows[i], other, D)[:W]
        if list(out[o]) != list(e[:W]): return False
    return True

def t_dcolor(r, ec):
    kind, W, rows, irow, nin, nout, obpp = [r.u32() for _ in range(7)]
    ib = [[r.raw(W) for _ in range(rows)] for _ in range(nin)]; ob = [r.raw(W * obpp) for _ in range(rows)]
    def rgb(y, cb, cr):
        x = cr - 128; R = clamp(y + ((fix(1.402) * x + ONE_HALF) >> 16))
        B = clamp(y + ((fix(1.772) * (cb - 128) + ONE_HALF) >> 16))
        G = clamp(y + ((-fix(0.34414) * (cb - 128) + ONE_HALF - fix(0.71414) * (cr - 128)) >> 16))
        return R, G, B
    for i in range(rows):
        for x in range(W):
            p = ob[i][x * obpp:(x + 1) * obpp]
            if kind == 0: e = rgb(ib[0][i][x], ib[1][i][x], ib[2][i][x]); ok = tuple(p[:3]) == e
            elif kind == 1: ok = tuple(p) == tuple(ib[c][i][x] for c in range(nin))
            elif kind == 2: ok = p[0] == ib[0][i][x]
            elif kind == 3: ok = tuple(p[:3]) == (ib[0][i][x],) * 3
            else:
                R, G, B = rgb(ib[0][i][x], ib[1][i][x], ib[2][i][x]); ok = tuple(p) == (255 - R, 255 - G, 255 - B, ib[3][i][x])
            if not ok: return False
    return ec == 0

def t_nothing(r, ec): return ec == 0

CHECKS = {'emit_message': t_emit_message, 'reset_error_mgr': t_reset_error, 'format_message': t_format_message, 'finish_output_pass': t_finish_output, 'finish_pass_master': t_finish_master,
          'emit_byte': t_marker_bytes, 'write_marker_header': t_marker_bytes, 'start_pass_prep': t_start_prep, 'reset_marker_reader': t_reset_marker, 'start_pass_dpost': t_dpost, 'jpeg_idct_float': t_idct_float, 'rgb_ycc_start': t_rgb_ycc_start, 'rgb_ycc_convert': t_color_compress('ycc'), 'rgb_gray_convert': t_color_compress('gray'), 'cmyk_ycck_convert': t_color_compress('cmyk'),
          'grayscale_convert': t_color_compress('grayc'), 'null_convert': t_color_compress('null'), 'null_method': t_nothing, 'start_pass_fdctmgr': t_start_fdct,
          'jpeg_fdct_float': t_fdct_float, 'forward_DCT_float': t_forward_dct, 'start_pass_upsample': t_start_up, 'fullsize_upsample': t_upsample, 'noop_upsample': t_upsample,
          'int_upsample': t_upsample, 'h2v1_upsample': t_upsample, 'h2v2_upsample': t_upsample, 'h2v1_fancy_upsample': t_upsample, 'h2v2_fancy_upsample': t_upsample,
          'ycc_rgb_convert': t_dcolor, 'dc_null_convert': t_dcolor, 'start_pass_dcolor': t_nothing, 'dc_grayscale_convert': t_dcolor, 'gray_rgb_convert': t_dcolor, 'ycck_cmyk_convert': t_dcolor}

if __name__ == '__main__':
    d = sys.argv[1]; total_fail = 0
    print('%-24s %9s %9s' % ('funcion', 'casos', 'fallos'))
    for name, chk in CHECKS.items():
        p = os.path.join(d, 'trace_%s.bin' % name)
        if not os.path.exists(p): continue
        n = fails = 0
        for rec in records(p):
            rd = Rd(rec); ec = rd.u32(); rd.u32()
            n += 1
            if not chk(rd, ec): fails += 1
        print('%-24s %9d %9d' % (name, n, fails)); total_fail += fails
    print('forward_DCT_float: coeficientes %d, identicos al redondeo exacto %d, empates .5 con +-1 %d; max |jpeg_fdct_float - DCT exacta escalada| = %.5f' % (STAT['dct_total'], STAT['dct_exact'], STAT['dct_off1'], STAT['max_abs_fdct']))
    print('jpeg_idct_float: muestras %d, identicas al redondeo exacto %d, +-1 %d, bloques comprobados %d, fuera de dominio %d' % (STAT['idct_total'], STAT['idct_exact'], STAT['idct_off1'], STAT['idct_checked'], STAT.get('idct_skipped', 0)))
    print('FALLOS DEL ORACULO:', total_fail); sys.exit(1 if total_fail else 0)
