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

def t_sep_down(r, ec):
    nc, ir, orow, iw = r.u32(), r.u32(), r.u32(), r.u32()
    comp = [(r.u32(), r.u32(), r.u32()) for _ in range(4)]
    L = struct.unpack('<17I', r.raw(68)); base = r.u32()
    if ec != 0 or L[0] != nc: return False
    for i in range(nc):
        cp, inp, outp, tag = L[1 + 4 * i], L[2 + 4 * i], L[3 + 4 * i], L[4 + 4 * i]
        vs, ib, ob = comp[i]
        if cp != (base + 0x54 * i) % 2**32 or inp != (ib + 4 * ir) % 2**32 or outp != (ob + 4 * orow * vs) % 2**32 or tag != (iw ^ i): return False
    return True
def t_downsample(r, ec):
    kind, hs, vs, he, ve, wib, iw, inrows, outrows = [r.u32() for _ in range(9)]
    OC = wib * 8; inw = OC * he
    rows = [list(r.raw(iw)) for _ in range(inrows)]
    out = [list(r.raw(OC)) for _ in range(outrows)]
    def padded(row, n): return row + [row[-1]] * (n - len(row)) if n > len(row) else row
    if kind == 1:
        for i in range(outrows):
            if out[i] != padded(rows[i], OC): return False
        return ec == 0
    pr = [padded(row, OC * he) for row in rows]
    for orow in range(vs):
        for oc in range(OC):
            if kind == 0:
                tot = sum(pr[orow * ve + v][oc * he + h] for v in range(ve) for h in range(he)); n = he * ve
                e = (tot + n // 2) // n
            elif kind == 2:
                e = (pr[orow][2 * oc] + pr[orow][2 * oc + 1] + (oc & 1)) >> 1
            else:
                bias = 1 if oc % 2 == 0 else 2
                e = (pr[2 * orow][2 * oc] + pr[2 * orow][2 * oc + 1] + pr[2 * orow + 1][2 * oc] + pr[2 * orow + 1][2 * oc + 1] + bias) >> 2
            if out[orow][oc] != e: return False
    return ec == 0

def t_idctmgr(r, ec):
    nc, method = r.u32(), r.u32(); comps = []
    for i in range(4):
        sz, need, hasq = r.u32(), r.u32() & 0xFF, r.u32(); q = struct.unpack('<64H', r.raw(128)) if hasq else None
        cm0, cm1 = r.u32(), r.u32(); tab = struct.unpack('<64f', r.raw(256)); setp = r.u32(); comps.append((sz, need, q, cm0, cm1, tab, setp))
    err = False
    for i in range(nc):
        sz, need, q, cm0, cm1, tab, setp = comps[i]
        if sz != 8 or method != 2: err = True; break
        if not setp: return False
        if (not need) or cm0 == 2 or q is None:
            if cm1 != cm0: return False
            continue
        if cm1 != 2: return False
        k = 0
        for row in range(8):
            for col in range(8):
                e = f32(f32(q[k] * AAN[row]) * AAN[col])   # x87 a 24 bits: cada producto se redondea a float
                if struct.pack('<f', e) != struct.pack('<f', tab[k]): return False
                k += 1
    return (ec != 0) == err

def mk_hash(call, ci, row, x): return (call * 31 + ci * 17 + row * 7 + x * 3 + 5) & 255
def t_pre_process(r, ec):
    nc, maxv, W, inavail, inctr, ogavail, ogctr, rtg, nbr = [r.u32() for _ in range(9)]
    comps = [(r.u32(), r.u32()) for _ in range(nc)]
    colb = []; ob = []
    for ci, (v, wib) in enumerate(comps):
        colb.append([bytearray(r.raw(W)) for _ in range(maxv)]); ob.append([bytearray(r.raw(wib * 8)) for _ in range(ogavail * v)])
    fcol = []; fob = []
    for ci, (v, wib) in enumerate(comps):
        fcol.append([bytearray(r.raw(W)) for _ in range(maxv)]); fob.append([bytearray(r.raw(wib * 8)) for _ in range(ogavail * v)])
    pin, pout, rtg1, nbr1 = r.u32(), r.u32(), r.u32(), r.u32()
    L = struct.unpack('<%dI' % (2 + 4 * 48), r.raw(4 * (2 + 4 * 48))); inb, prb, obb = r.u32(), r.u32(), r.u32()
    M = 2**32
    def expand_bottom(rows, width, inrows, outrows):
        for i in range(inrows, outrows): rows[i][:width] = rows[inrows - 1][:width]
    cc = []; dd = []
    while inctr < inavail and ogctr < ogavail:
        numrows = min(maxv - nbr, inavail - inctr)
        call = len(cc); cc.append((inctr, nbr, numrows))
        for ci in range(nc):
            for i in range(numrows):
                for x in range(W): colb[ci][nbr + i][x] = mk_hash(call, ci, nbr + i, x)
        inctr += numrows; nbr += numrows; rtg = (rtg - numrows) % M
        if rtg == 0 and nbr < maxv:
            for ci in range(nc): expand_bottom(colb[ci], W, nbr, maxv)
            nbr = maxv
        if nbr == maxv:
            dc = len(dd); dd.append(ogctr)
            for ci, (v, wib) in enumerate(comps):
                for rr in range(v):
                    for x in range(wib * 8): ob[ci][ogctr * v + rr][x] = mk_hash(100 + dc, ci, ogctr * v + rr, x)
            nbr = 0; ogctr += 1
        if rtg == 0 and ogctr < ogavail:
            for ci, (v, wib) in enumerate(comps): expand_bottom(ob[ci], wib * 8, ogctr * v, ogavail * v)
            ogctr = ogavail; break
    if ec != 0 or pin != inctr or pout != ogctr or rtg1 != rtg or nbr1 != nbr: return False
    for ci in range(nc):
        if colb[ci] != fcol[ci] or ob[ci] != fob[ci]: return False
    if L[0] != len(cc) or L[1] != len(dd): return False
    for k, (ic, nb0, nr) in enumerate(cc):
        e = L[2 + 4 * k:6 + 4 * k]
        if e != (inb + 4 * ic) % M or True:
            if (e[0], e[1], e[2], e[3]) != ((inb + 4 * ic) % M, (prb + 8) % M, nb0, nr): return False
    for k, og in enumerate(dd):
        e = L[2 + 96 + 4 * k:6 + 96 + 4 * k]
        if (e[0], e[1], e[2], e[3]) != ((prb + 8) % M, 0, obb, og): return False
    return True

def t_onepass(r, ec):
    mpr, totr, inrow, outrow, nc, blocks, rows_per, vo, ctr, stop = [r.u32() for _ in range(10)]
    comps = [tuple(r.u32() for _ in range(6)) for _ in range(nc)]
    ob = [r.u32() for _ in range(nc)]
    inrow1, outrow1, ctr1, vo1, rows1, ctr1b = [r.u32() for _ in range(6)]
    L = struct.unpack('<%dI' % (64 + 2 + 5 * 256), r.raw(4 * (64 + 2 + 5 * 256))); mb0 = r.u32(); vs0 = r.u32(); M = 2**32
    ret = r.ret
    last_col = mpr - 1; last_row = totr - 1
    calls = 0; idct = []; dec = []; res = None; fctr = None; fvo = None
    for yoff in range(vo, rows_per):
        mcu = ctr
        while mcu <= last_col:
            n = calls; calls += 1; dec.append(mb0)
            if stop != 0 and n + 1 == stop: res = 0; fctr = mcu; fvo = yoff; break
            blkn = 0
            for ci in range(nc):
                needed, w, h, lcw, lrh, ptr = comps[ci]
                if not needed: blkn += w * h; continue
                useful = w if mcu < last_col else lcw
                outp = (ob[ci] + yoff * 8 * 4) % M; sc = mcu * w * 8
                for yi in range(h):
                    if inrow < last_row or yoff + yi < lrh:
                        oc = sc
                        for xi in range(useful):
                            idct.append((ptr, (mb0 + (blkn + xi) * 128) % M, outp, oc, (n * 3 + blkn + xi + 1) & 0xFFFF)); oc += 8
                    blkn += w; outp = (outp + 32) % M
            mcu += 1
        if res is not None: break
        ctr = 0
    fin = 0
    if res is None:
        outrow += 1; inrow += 1
        if inrow < totr:
            res = 3; fctr = 0; fvo = 0
            rows_per = 1 if nc > 1 else (vs0 if inrow < totr - 1 else comps[0][4])
        else:
            res = 4; fin = 1; fctr = ctr; fvo = vo if False else 0 if False else None
    if ec != 0 or ret != res: return False
    if L[0] != calls or L[64] != fin or L[65] != len(idct): return False
    if res == 0:
        if (inrow1, outrow1, ctr1, vo1) != (inrow, outrow, fctr, fvo): return False
    else:
        if (inrow1, outrow1) != (inrow, outrow): return False
        if res == 3 and (ctr1, vo1, rows1) != (0, 0, rows_per): return False
        if res == 4 and ctr1 != 0: return False
    for k, e in enumerate(dec):
        if L[2 + k] != e: return False
    for k, e in enumerate(idct):
        if tuple(L[64 + 2 + 5 * k:64 + 7 + 5 * k]) != e: return False
    return True

NAT = [0,1,8,16,9,2,3,10,17,24,32,25,18,11,4,5,12,19,26,33,40,48,41,34,27,20,13,6,7,14,21,28,35,42,49,56,57,50,43,36,29,22,15,23,30,37,44,51,58,59,52,45,38,31,39,46,53,60,61,54,47,55,62,63]
def t_marker_hdr(r, ec):
    kind, arith, prog, Ss, Se, Ah, Al, ri, lri, nc = [r.u32() for _ in range(10)]
    comps = [(r.u32(), r.u32(), r.u32()) for _ in range(nc)]
    q = []
    for i in range(4):
        p, sent = r.u32(), r.u32(); qv = struct.unpack('<64H', r.raw(128)); sa = r.u32(); q.append([p, sent, qv, sa])
    h = []
    for i in range(8):
        p, sent = r.u32(), r.u32(); bits = r.raw(17); hv = r.raw(256); sa = r.u32(); h.append([p, sent, bits, hv, sa])
    lri1 = r.u32(); n = r.u32(); got = r.raw(min(n, 4100))
    out = bytearray(); sent_q = [t[1] for t in q]; sent_h = [t[1] for t in h]
    class E(Exception): pass
    def eb(b): out.append(b & 255)
    def em(m): eb(0xFF); eb(m)
    def e2(v): eb((v >> 8) & 255); eb(v & 255)
    def dqt(i):
        if not q[i][0]: raise E()
        qv = q[i][2]; prec = 1 if any(v > 255 for v in qv) else 0
        if not sent_q[i]:
            em(0xDB); e2(131 if prec else 67); eb(i + (prec << 4))
            for k in range(64):
                v = qv[NAT[k]]
                if prec: eb(v >> 8)
                eb(v & 255)
            sent_q[i] = 1
    def dht(i, ac):
        t = h[(4 if ac else 0) + i]; idx = i + (0x10 if ac else 0)
        if not t[0]: raise E()
        if not sent_h[(4 if ac else 0) + i]:
            em(0xC4); length = sum(t[2][1:17]); e2(length + 19); eb(idx)
            for k in range(1, 17): eb(t[2][k])
            for k in range(length): eb(t[3][k])
            sent_h[(4 if ac else 0) + i] = 1
    err = False
    try:
        if kind == 0:
            em(0xD8)
            for i in range(4):
                if q[i][0]: dqt(i)
            if not arith:
                for i in range(4):
                    if h[i][0]: dht(i, 0)
                    if h[4 + i][0]: dht(i, 1)
            em(0xD9)
        else:
            if not arith:
                for (cid, dc, ac) in comps:
                    if prog:
                        if Ss == 0:
                            if Ah == 0: dht(dc, 0)
                        else: dht(ac, 1)
                    else: dht(dc, 0); dht(ac, 1)
            if ri != lri:
                em(0xDD); e2(4); e2(ri & 0xFFFF); lri = ri
            em(0xDA); e2(2 * nc + 6); eb(nc)
            for (cid, dc, ac) in comps:
                td, ta = dc, ac
                if prog:
                    if Ss == 0:
                        ta = 0
                        if Ah != 0 and not arith: td = 0
                    else: td = 0
                eb(cid); eb((td << 4) + ta)
            eb(Ss); eb(Se); eb((Ah << 4) + Al)
    except E:
        err = True
    if (ec != 0) != err: return False
    if bytes(out) != bytes(got) or n != len(out): return False
    if lri1 != lri: return False
    if [t[3] for t in q] != [(s & 0xFF) if q[i][0] else 0 for i, s in enumerate(sent_q)]: return False
    if [t[4] for t in h] != [(s & 0xFF) if h[i][0] else 0 for i, s in enumerate(sent_h)]: return False
    return True

def t_virt(r, ec):
    kind, rows, spr, maxacc, rim, rpc, cur, undef, pz, dirty, bso, start, num, wr, mb, rp, db = [r.u32() for _ in range(17)]
    unit = 1 if kind == 0 else 128; bpr = spr * unit; M = 2**32
    data = bytearray(r.raw(rim * bpr)); cur1, undef1, dirty1 = r.u32(), r.u32(), r.u32() & 0xFF; fdata = r.raw(rim * bpr)
    L = struct.unpack('<%dI' % (1 + 4 * 64), r.raw(4 * (1 + 4 * 64))); ret = r.ret
    pz &= 0xFF; dirty &= 0xFF; bso &= 0xFF; wr &= 0xFF
    log = []
    def io(writing):
        off = cur * bpr; i = 0
        while i < rim:
            n = min(rpc, rim - i); thisrow = cur + i; n = min(n, undef - thisrow); n = min(n, rows - thisrow)
            if n <= 0: break
            cnt = n * bpr
            if writing: log.append((1, (db + i * bpr) % M, off, cnt))
            else:
                log.append((0, (db + i * bpr) % M, off, cnt))
                for k in range(cnt): data[i * bpr + k] = ((off + k) * 7 + 3) & 255
            off += cnt; i += rpc
    err = False; res = None
    end = start + num
    if end > rows or num > maxacc or mb == 0: err = True
    else:
        if start < cur or end > cur + rim:
            if not bso: err = True
            else:
                if dirty: io(True); dirty = 0
                if start > cur: cur = start
                else: cur = max(end - rim, 0)
                io(False)
        if not err and undef < end:
            if undef < start:
                if wr: err = True
                else: ud = start
            else: ud = undef
            if not err:
                if wr: undef = end
                if pz:
                    a = ud - cur; b = end - cur
                    while a < b: data[a * bpr:(a + 1) * bpr] = bytes(bpr); a += 1
                elif not wr: err = True
        if not err:
            if wr: dirty = 1
            res = (rp + (start - cur) * 4) % M
    if (ec != 0) != err: return False
    if err: return cur1 == cur and undef1 == undef and dirty1 == dirty and bytes(data) == fdata
    if ret != res or cur1 != cur or undef1 != undef or dirty1 != dirty or bytes(data) != fdata: return False
    if L[0] != len(log): return False
    for k, e in enumerate(log):
        if tuple(L[1 + 4 * k:5 + 4 * k]) != e: return False
    return True

def parse_markers(st, kind, mk):
    """Analiza un flujo de cabeceras JPEG como lo especifica el estandar/libjpeg 6b (sin usar la biblioteca). Devuelve (error, consumido, estado) o None si el flujo no es del tipo bien formado generado."""
    S = dict(w=0, h=0, prec=0, nc=0, prog=0, arith=0, jfif=0, jmaj=0, jmin=0, unit=0, xd=0, yd=0, adobe=0, atr=0, ri=0, ncs=0, comps=[], scan=[], Ss=0, Se=0, Ah=0, Al=0, nscan=0, q={}, h_dc={}, h_ac={}, saw_sof=0)
    def u16(p): return (st[p] << 8) | st[p + 1]
    def app(marker, p, ln):
        body = st[p + 2:p + ln]; num = min(len(body), 14) if ln - 2 >= 14 else len(body)
        data = body[:num]
        if marker == 0xE0 and num >= 14 and data[:5] == b'JFIF\0':
            S['jfif'] = 1; S['jmaj'] = data[5]; S['jmin'] = data[6]; S['unit'] = data[7]; S['xd'] = (data[8] << 8) | data[9]; S['yd'] = (data[10] << 8) | data[11]
        elif marker == 0xEE and num >= 12 and data[:5] == b'Adobe':
            S['adobe'] = 1; S['atr'] = data[11]
    p = 0
    if kind == 1:
        ln = u16(0); app(mk, 0, ln); return (False, ln, S)
    if st[0:2] != bytes([0xFF, 0xD8]): return None
    S['jmaj'] = 1; S['jmin'] = 1; S['unit'] = 0; S['xd'] = 1; S['yd'] = 1   # get_soi fija los valores JFIF por defecto
    p = 2
    while True:
        if p + 1 >= len(st) or st[p] != 0xFF: return None
        m = st[p + 1]; p += 2; ln = u16(p)
        if m in (0xE0, 0xEE): app(m, p, ln)
        elif m == 0xDB:
            q = p + 2; end = p + ln
            while q < end:
                t = st[q]; q += 1; prec = t >> 4; n = t & 15
                if n > 3: return None
                tab = [0] * 64
                for i in range(64):
                    if prec: v = u16(q); q += 2
                    else: v = st[q]; q += 1
                    tab[NAT[i]] = v
                S['q'][n] = tab
        elif m in (0xC0, 0xC1, 0xC2, 0xC9):
            S['prec'] = st[p + 2]; S['h'] = u16(p + 3); S['w'] = u16(p + 5); S['nc'] = st[p + 7]; S['prog'] = 1 if m == 0xC2 else 0; S['arith'] = 1 if m == 0xC9 else 0
            S['comps'] = [(st[p + 8 + 3 * i], st[p + 9 + 3 * i] >> 4, st[p + 9 + 3 * i] & 15, st[p + 10 + 3 * i]) for i in range(S['nc'])]; S['saw_sof'] = 1
        elif m == 0xC4:
            q = p + 2; end = p + ln
            while q < end:
                t = st[q]; q += 1; bits = [0] + list(st[q:q + 16]); q += 16; cnt = sum(bits); vals = list(st[q:q + cnt]); q += cnt
                (S['h_ac'] if t & 0x10 else S['h_dc'])[t & 15] = (bits, vals)
        elif m == 0xDD: S['ri'] = u16(p + 2)
        elif m == 0xDA:
            ns = st[p + 2]; ids = [c[0] for c in S['comps']]; scan = []
            for i in range(ns):
                cid = st[p + 3 + 2 * i]; tt = st[p + 4 + 2 * i]
                if cid not in ids: return (True, None, S)
                scan.append((cid, tt >> 4, tt & 15))
            q = p + 3 + 2 * ns; S['Ss'] = st[q]; S['Se'] = st[q + 1]; S['Ah'] = st[q + 2] >> 4; S['Al'] = st[q + 2] & 15; S['scan'] = scan; S['ncs'] = ns; S['nscan'] = 1
            return (False, p + ln, S)
        p += ln
def t_read_markers(r, ec):
    kind, mut, sus, n = [r.u32() for _ in range(4)]; st = r.raw(n); consumed = r.u32()
    g = [r.u32() for _ in range(16)]
    w, h, prec, nc, prog, arith, jfif, jmaj, jmin, unit, xd, yd, adobe, atr, ri, ncs = g
    comps = [tuple(r.u32() for _ in range(4)) for _ in range(4)]; scan = [tuple(r.u32() for _ in range(3)) for _ in range(4)]
    Ss, Se, Ah, Al, nscan, unread = [r.u32() for _ in range(6)]
    q = []
    for i in range(4):
        p = r.u32(); q.append(struct.unpack('<64H', r.raw(128)) if p else None)
    hs = []
    for i in range(8):
        p = r.u32(); hs.append((r.raw(17), r.raw(256)) if p else None)
    saw_soi, saw_sof = r.u32() & 0xFF, r.u32() & 0xFF; ret = r.ret
    if mut or sus: return True      # flujos mutados o con suspension: solo EXE frente a biblioteca
    res = parse_markers(st, kind, unread)
    if res is None: return True
    err, cons, S = res
    if err: return ec != 0
    if ec != 0 or (ret if kind == 0 else ret & 0xFF) != 1 or consumed != cons: return False
    if (jfif & 0xFF, jmaj, jmin, unit, xd, yd, adobe & 0xFF, atr) != (S['jfif'], S['jmaj'], S['jmin'], S['unit'], S['xd'], S['yd'], S['adobe'], S['atr']): return False
    if kind == 1: return True
    if (w, h, prec, nc, prog & 0xFF, arith & 0xFF, ri, ncs) != (S['w'], S['h'], S['prec'], S['nc'], S['prog'], S['arith'], S['ri'], S['ncs']): return False
    for i in range(S['nc']):
        if comps[i] != S['comps'][i]: return False
    for i in range(S['ncs']):
        if scan[i] != S['scan'][i]: return False
    if (Ss, Se, Ah, Al, nscan) != (S['Ss'], S['Se'], S['Ah'], S['Al'], S['nscan']) or saw_soi != 1 or saw_sof != 1: return False
    for i in range(4):
        if (q[i] is not None) != (i in S['q']): return False
        if q[i] is not None and list(q[i]) != S['q'][i]: return False
    for i in range(8):
        d = S['h_dc'] if i < 4 else S['h_ac']; k = i & 3
        if (hs[i] is not None) != (k in d): return False
        if hs[i] is not None:
            bits, vals = d[k]
            if list(hs[i][0]) != bits or list(hs[i][1][:len(vals)]) != vals: return False
    return True
def unread_mk(st): return 0xE0

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

CHECKS = {'read_markers': t_read_markers, 'get_interesting_appn': t_read_markers, 'jinit_marker_reader': t_nothing, 'access_virt_sarray': t_virt, 'access_virt_barray': t_virt, 'write_tables_only': t_marker_hdr, 'write_scan_header': t_marker_hdr, 'decompress_onepass': t_onepass, 'pre_process_data': t_pre_process, 'start_pass_idctmgr': t_idctmgr, 'start_pass_downsample': t_nothing, 'sep_downsample': t_sep_down, 'int_downsample': t_downsample, 'fullsize_downsample': t_downsample, 'h2v1_downsample': t_downsample, 'h2v2_downsample': t_downsample, 'emit_message': t_emit_message, 'reset_error_mgr': t_reset_error, 'format_message': t_format_message, 'finish_output_pass': t_finish_output, 'finish_pass_master': t_finish_master,
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
            rd = Rd(rec); ec = rd.u32(); rd.ret = rd.u32()
            n += 1
            if not chk(rd, ec): fails += 1
        print('%-24s %9d %9d' % (name, n, fails)); total_fail += fails
    print('forward_DCT_float: coeficientes %d, identicos al redondeo exacto %d, empates .5 con +-1 %d; max |jpeg_fdct_float - DCT exacta escalada| = %.5f' % (STAT['dct_total'], STAT['dct_exact'], STAT['dct_off1'], STAT['max_abs_fdct']))
    print('jpeg_idct_float: muestras %d, identicas al redondeo exacto %d, +-1 %d, bloques comprobados %d, fuera de dominio %d' % (STAT['idct_total'], STAT['idct_exact'], STAT['idct_off1'], STAT['idct_checked'], STAT.get('idct_skipped', 0)))
    print('FALLOS DEL ORACULO:', total_fail); sys.exit(1 if total_fail else 0)
