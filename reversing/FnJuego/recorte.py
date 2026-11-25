import sys, zlib, struct
sys.path.insert(0, r'D:\Original\wv-sp7b\reversing\FnJuego')
def decode(ruta):
    d = open(ruta, 'rb').read(); i = 8; idat = b''
    while i < len(d):
        n = struct.unpack('>I', d[i:i + 4])[0]; t = d[i + 4:i + 8]
        if t == b'IHDR': w, h = struct.unpack('>II', d[i + 8:i + 16])
        if t == b'IDAT': idat += d[i + 8:i + 8 + n]
        i += 12 + n
    raw = zlib.decompress(idat); fila = w * 3 + 1
    return w, h, [raw[y * fila + 1:(y + 1) * fila] for y in range(h)]
def escribir(ruta, w, h, filas):
    def ch(t, dd): return struct.pack('>I', len(dd)) + t + dd + struct.pack('>I', zlib.crc32(t + dd) & 0xffffffff)
    raw = b''.join(bytes([0]) + f for f in filas)
    open(ruta, 'wb').write(bytes([137]) + b'PNG' + bytes([13, 10, 26, 10]) + ch(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0)) + ch(b'IDAT', zlib.compress(raw, 6)) + ch(b'IEND', b''))
def recorte(src, dst, x0, y0, x1, y1, k):
    w, h, filas = decode(src); out = []
    for y in range(y0, y1):
        f = filas[y]; fila = b''.join(f[3 * x:3 * x + 3] * k for x in range(x0, x1))
        out += [fila] * k
    escribir(dst, (x1 - x0) * k, (y1 - y0) * k, out)
if __name__ == '__main__':
    a = sys.argv; recorte(a[1], a[2], *[int(v) for v in a[3:8]])
