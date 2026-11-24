# PNG puro (sin dependencias): lectura (8 bits, RGB/RGBA, todos los filtros), escritura (filtro Up, nivel 9), composicion y diferencia.
import zlib, struct
def leer(ruta):
    d = open(ruta, 'rb').read(); assert d[:8] == b'\x89PNG\r\n\x1a\n'
    i = 8; idat = b''; w = h = ct = None
    while i < len(d):
        n, t = struct.unpack('>I4s', d[i:i + 8]); c = d[i + 8:i + 8 + n]; i += 12 + n
        if t == b'IHDR': w, h, bd, ct = struct.unpack('>IIBB', c[:10]); assert bd == 8 and ct in (2, 6)
        elif t == b'IDAT': idat += c
    bpp = 3 if ct == 2 else 4; raw = zlib.decompress(idat); st = w * bpp
    out = bytearray(); prev = bytearray(st); p = 0
    for y in range(h):
        f = raw[p]; line = bytearray(raw[p + 1:p + 1 + st]); p += 1 + st
        if f == 1:
            for x in range(bpp, st): line[x] = (line[x] + line[x - bpp]) & 255
        elif f == 2:
            for x in range(st): line[x] = (line[x] + prev[x]) & 255
        elif f == 3:
            for x in range(st): line[x] = (line[x] + (((line[x - bpp] if x >= bpp else 0) + prev[x]) >> 1)) & 255
        elif f == 4:
            for x in range(st):
                a = line[x - bpp] if x >= bpp else 0; b = prev[x]; c = prev[x - bpp] if x >= bpp else 0
                pa = abs(b - c); pb = abs(a - c); pc = abs(a + b - 2 * c)
                pr = a if (pa <= pb and pa <= pc) else (b if pb <= pc else c)
                line[x] = (line[x] + pr) & 255
        out += line; prev = line
    if bpp == 4:   # a RGB
        rgb = bytearray(w * h * 3); rgb[0::3] = out[0::4]; rgb[1::3] = out[1::4]; rgb[2::3] = out[2::4]; out = rgb
    return w, h, bytes(out)
def escribir(ruta, w, h, rgb, nivel=9):
    st = w * 3; raw = bytearray(); prev = bytes(st)
    for y in range(h):
        line = rgb[y * st:(y + 1) * st]
        raw.append(2); raw += bytes(a - b & 255 for a, b in zip(line, prev)); prev = line
    def ch(t, dd): return struct.pack('>I', len(dd)) + t + dd + struct.pack('>I', zlib.crc32(t + dd) & 0xffffffff)
    open(ruta, 'wb').write(b'\x89PNG\r\n\x1a\n' + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(bytes(raw), nivel)) + ch(b'IEND', b''))
def media(w, h, rgb):
    n = w * h; return [sum(rgb[c::3]) / n for c in range(3)]
def diferencia(a, b):
    """(diferencia de color medio por canal con signo b-a, diferencia absoluta media, imagen de |a-b| amplificada x4)"""
    wa, ha, ra = a; wb, hb, rb = b; assert (wa, ha) == (wb, hb)
    n = wa * ha
    ma = [sum(ra[c::3]) / n for c in range(3)]; mb = [sum(rb[c::3]) / n for c in range(3)]
    dif = bytes(min(255, abs(x - y) * 4) for x, y in zip(ra, rb))
    absm = sum(abs(x - y) for x, y in zip(ra, rb)) / (n * 3)
    return [mb[c] - ma[c] for c in range(3)], absm, dif
def lado_a_lado(imgs, sep=4, esc=1):
    """imgs: lista de (w,h,rgb) de la misma altura; esc=2 reduce a la mitad (promedio 2x2)"""
    ims = []
    for w, h, rgb in imgs:
        if esc == 2:
            nw, nh = w // 2, h // 2; o = bytearray(nw * nh * 3)
            for y in range(nh):
                r0 = rgb[(2 * y) * w * 3:(2 * y + 1) * w * 3]; r1 = rgb[(2 * y + 1) * w * 3:(2 * y + 2) * w * 3]
                for c in range(3):
                    a0 = r0[c::3]; a1 = r1[c::3]
                    o[y * nw * 3 + c:(y + 1) * nw * 3:3] = bytes((a0[2 * x] + a0[2 * x + 1] + a1[2 * x] + a1[2 * x + 1]) >> 2 for x in range(nw))
            ims.append((nw, nh, bytes(o)))
        else: ims.append((w, h, rgb))
    h = ims[0][1]; W = sum(i[0] for i in ims) + sep * (len(ims) - 1); out = bytearray(b'\xff\x00\xff' * 0)
    filas = []
    for y in range(h):
        fila = bytearray()
        for k, (w, hh, rgb) in enumerate(ims):
            if k: fila += b'\xff\xff\xff' * sep
            fila += rgb[y * w * 3:(y + 1) * w * 3]
        filas.append(bytes(fila))
    return W, h, b''.join(filas)
