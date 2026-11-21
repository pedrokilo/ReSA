"""Oraculo independiente de los PNG que escribe el banco: firma, CRC de cada chunk (zlib de Python), IHDR, y que los IDAT concatenados se descompriman
(con el modulo zlib de Python) a exactamente la cantidad de bytes que exige el IHDR (con o sin interlazado Adam7). Cuenta los tipos de chunk vistos."""
import sys, glob, struct, zlib, collections
def pass_size(w, h, bpp_bits, interlace):
    def rb(ww): return (ww * bpp_bits + 7) // 8
    if not interlace: return h * (rb(w) + 1)
    tot = 0
    for (xo, yo, xs, ys) in ((0,0,8,8),(4,0,8,8),(0,4,4,8),(2,0,4,4),(0,2,2,4),(1,0,2,2),(0,1,1,2)):
        pw = (w - xo + xs - 1) // xs if w > xo else 0
        ph = (h - yo + ys - 1) // ys if h > yo else 0
        if pw and ph: tot += ph * (rb(pw) + 1)
    return tot
ok = bad = trunc = 0; kinds = collections.Counter(); errs = []
for f in sorted(glob.glob(sys.argv[1] + r'\*.png')):
    d = open(f, 'rb').read()
    if len(d) == 0: continue
    if d[:8] != b'\x89PNG\r\n\x1a\n': bad += 1; errs.append((f, 'firma')); continue
    p = 8; idat = b''; ihdr = None; good = True; iend = False
    while p + 12 <= len(d):
        ln, = struct.unpack('>I', d[p:p+4]); t = d[p+4:p+8]; body = d[p+8:p+8+ln]; crc, = struct.unpack('>I', d[p+8+ln:p+12+ln]) if p+12+ln <= len(d) else (None,)
        if crc is None: good = False; break
        if zlib.crc32(t + body) & 0xffffffff != crc: good = False; errs.append((f, 'crc ' + t.decode('latin1'))); break
        kinds[t.decode('latin1')] += 1
        if t == b'IHDR': ihdr = struct.unpack('>IIBBBBB', body)
        if t == b'IDAT': idat += body
        if t == b'IEND': iend = True
        p += 12 + ln
    if not good: bad += 1; continue
    if not iend or ihdr is None: trunc += 1; continue
    w, h, depth, ct, comp, filt, il = ihdr
    ch = {0:1,2:3,3:1,4:2,6:4}.get(ct)
    if ch is None or depth not in (1,2,4,8,16): trunc += 1; continue
    try: raw = zlib.decompress(idat)
    except Exception as e: bad += 1; errs.append((f, 'inflate ' + str(e))); continue
    exp = pass_size(w, h, depth * ch, il)
    if len(raw) != exp: bad += 1; errs.append((f, f'tamano {len(raw)} != {exp}')); continue
    ok += 1
print(f'PNG completos y validos: {ok}; invalidos: {bad}; incompletos/atipicos: {trunc}')
print('chunks vistos:', dict(sorted(kinds.items())))
for e in errs[:10]: print(e)
