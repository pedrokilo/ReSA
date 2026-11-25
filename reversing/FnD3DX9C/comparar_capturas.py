"""FnD3DX9C: compara dos capturas PNG del juego (las de captura.py: RGB de 8 bits, filtro 0) pixel a pixel sin dependencias externas.
Imprime pixeles identicos, diferencia media absoluta por canal, maximo y la rejilla 8x6 de diferencia media (para ver DONDE difiere).
Uso: python -I comparar_capturas.py a.png b.png [umbral=0]"""
import struct, sys, zlib


def leer(ruta):
    d = open(ruta, 'rb').read()
    assert d[:8] == b'\x89PNG\r\n\x1a\n'
    p = 8
    idat = b''
    w = h = 0
    while p < len(d):
        n, t = struct.unpack('>I4s', d[p:p + 8])
        c = d[p + 8:p + 8 + n]
        if t == b'IHDR':
            w, h = struct.unpack('>II', c[:8])
        elif t == b'IDAT':
            idat += c
        p += 12 + n
    raw = zlib.decompress(idat)
    stride = 1 + w * 3
    filas = []
    for y in range(h):
        assert raw[y * stride] == 0, 'filtro PNG no soportado'
        filas.append(raw[y * stride + 1:(y + 1) * stride])
    return w, h, filas


def main():
    a, b = sys.argv[1:3]
    umbral = int(sys.argv[3]) if len(sys.argv) > 3 else 0
    wa, ha, fa = leer(a)
    wb, hb, fb = leer(b)
    if (wa, ha) != (wb, hb):
        print('tamanos distintos', (wa, ha), (wb, hb))
        return 1
    ident = tot = 0
    suma = 0
    mx = 0
    cel = [[0] * 8 for _ in range(6)]
    celn = [[0] * 8 for _ in range(6)]
    for y in range(ha):
        ra, rb = fa[y], fb[y]
        if ra == rb:
            ident += wa
            tot += wa
            for x in range(0, wa, 8):
                celn[y * 6 // ha][x * 8 // wa] += 1
            continue
        for x in range(wa):
            i = x * 3
            dr, dg, db = abs(ra[i] - rb[i]), abs(ra[i + 1] - rb[i + 1]), abs(ra[i + 2] - rb[i + 2])
            m = max(dr, dg, db)
            tot += 1
            if m <= umbral:
                ident += 1
            suma += dr + dg + db
            mx = max(mx, m)
            if x % 8 == 0:
                cel[y * 6 // ha][x * 8 // wa] += (dr + dg + db) / 3
                celn[y * 6 // ha][x * 8 // wa] += 1
    print('%s vs %s: %dx%d pixeles; identicos (<=%d): %d (%.3f %%); diferencia media %.4f por canal; maximo %d' % (a.split('\\')[-1], b.split('\\')[-1], wa, ha, umbral, ident, 100.0 * ident / tot, suma / (3.0 * tot), mx))
    print('rejilla 8x6 de diferencia media (0..255):')
    for r in range(6):
        print('  ' + ' '.join('%6.2f' % (cel[r][c] / celn[r][c] if celn[r][c] else 0) for c in range(8)))
    return 0


if __name__ == '__main__':
    sys.exit(main())
