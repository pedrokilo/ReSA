"""Genera los casos del banco LIB1A con el modulo zlib de Python (oraculo independiente: otra implementacion y version de zlib).
Uso: python -I gen_cases.py <directorio de salida> [n_flujos] [n_longitudes] [n_flush]
Escribe streams.bin, lens.bin y flush.bin (formatos descritos en bench.cpp)."""
import sys, os, struct, zlib, random, hashlib


def kraft(lengths, maxbits=15):
    """clasifica un conjunto de longitudes de codigo: 0 completo, 1 incompleto, 2 sobresuscrito (desigualdad de Kraft)."""
    cnt = [0] * (maxbits + 1)
    for l in lengths:
        if l:
            cnt[l] += 1
    left = 1
    for l in range(1, maxbits + 1):
        left <<= 1
        left -= cnt[l]
        if left < 0:
            return 2
    return 0 if left == 0 else 1


def data_block(rng):
    kind = rng.randrange(8)
    if rng.random() < .5:
        n = rng.choice([0, 1, 2, 3, 5, 17, 100, 255, 256, 257, 258, 259, 1000, 4096, 5000, 12000, 33000, 40000, 70000])
    else:
        n = rng.randrange(0, 20000)
    if kind == 0:
        return bytes(rng.randrange(256) for _ in range(n))
    if kind == 1:
        return bytes([rng.randrange(256)]) * n
    if kind == 2:
        words = [bytes(rng.randrange(97, 123) for _ in range(rng.randrange(2, 9))) for _ in range(rng.randrange(1, 40))]
        out = bytearray()
        while len(out) < n:
            out += rng.choice(words) + b' '
        return bytes(out[:n])
    if kind == 3:
        out = bytearray()
        while len(out) < n:
            run = rng.randrange(1, 300)
            if rng.random() < .5 and out:
                dist = rng.randrange(1, min(len(out), 32768) + 1)
                for _ in range(run):
                    out.append(out[-dist])
            else:
                out += bytes(rng.randrange(256) for _ in range(rng.randrange(1, 20)))
        return bytes(out[:n])
    if kind == 4:
        return bytes(rng.choice(b'ab') for _ in range(n))
    if kind == 5:
        return bytes((i * 7 + (i >> 8)) & 255 for i in range(n))
    if kind == 6:
        return bytes(rng.choice([0, 0, 0, 1, 255]) for _ in range(n))
    return bytes(rng.randrange(0, 16) for _ in range(n))


def make_stream(rng):
    plain = data_block(rng)
    level = rng.choice([0, 1, 1, 2, 3, 4, 5, 6, 6, 6, 7, 8, 9, 9])
    strat = rng.choice([zlib.Z_DEFAULT_STRATEGY] * 4 + [zlib.Z_FILTERED, zlib.Z_HUFFMAN_ONLY, zlib.Z_RLE, zlib.Z_FIXED])
    mem = rng.randrange(1, 10)
    wb = rng.choice([15, 15, 15, 14, 12, 9])
    c = zlib.compressobj(level, zlib.DEFLATED, -wb, mem, strat)
    out = bytearray()
    pos = 0
    while pos < len(plain):
        step = rng.randrange(1, max(2, len(plain)))
        out += c.compress(plain[pos:pos + step])
        pos += step
        r = rng.random()
        if r < .15:
            out += c.flush(zlib.Z_SYNC_FLUSH)
        elif r < .2:
            out += c.flush(zlib.Z_FULL_FLUSH)
    out += c.flush()
    return plain, bytes(out), wb


def py_decode(stream):
    d = zlib.decompressobj(-15)
    try:
        out = d.decompress(stream)
    except zlib.error:
        return 1, None
    if d.eof:
        return 0, out
    return 2, out


def corrupt(rng, s):
    s = bytearray(s)
    k = rng.randrange(6)
    if not s:
        return bytes([rng.randrange(256) for _ in range(rng.randrange(1, 6))])
    if k == 0:
        for _ in range(rng.randrange(1, 4)):
            s[rng.randrange(len(s))] ^= 1 << rng.randrange(8)
    elif k == 1:
        s = s[:rng.randrange(len(s))]
    elif k == 2:
        i = rng.randrange(len(s))
        s[i] = rng.randrange(256)
    elif k == 3:
        i = rng.randrange(len(s))
        s[i:i] = bytes(rng.randrange(256) for _ in range(rng.randrange(1, 5)))
    elif k == 4:
        s = bytearray(rng.randrange(256) for _ in range(rng.randrange(1, 300)))
    else:
        i = rng.randrange(len(s))
        del s[i:i + rng.randrange(1, 5)]
    return bytes(s)


def gen_streams(path, n, rng):
    with open(path, 'wb') as f:
        f.write(b'LZS1' + struct.pack('<I', n))
        for i in range(n):
            plain, stream, wb = make_stream(rng)
            if rng.random() < .7:
                wsize = 1 << rng.choice([15, 15, 15, 15, 14, 13, 12, 11, 10, 9, 8])
            else:
                wsize = 32768
            kind = 0
            if rng.random() < .45:
                stream = corrupt(rng, stream)
                kind = 1
            st, out = py_decode(stream)
            # el oraculo solo juzga la salida si el flujo es valido y la ventana de decodificacion no es menor que la que necesita: la del compresor si el flujo no se ha tocado, o 32768 si se ha corrompido (un flujo corrompido puede pedir distancias mayores)
            judge = 1 if (st == 0 and ((kind == 0 and wsize >= (1 << wb)) or wsize == 32768)) else 0
            if st != 0:
                out = b''
            f.write(struct.pack('<IIIIII', rng.getrandbits(32), wsize, kind, st, judge, len(out)))
            f.write(hashlib.sha256(out).digest() + struct.pack('<II', zlib.adler32(out) & 0xFFFFFFFF, len(stream)) + stream)


def rand_lengths(rng, count, maxlen, style):
    if style == 0:  # codigo completo canonico por particion de Kraft
        lens = [0] * count
        if count == 0:
            return lens
        if count < 2:
            return [1] * count
        leaves = [1, 1]  # dos hojas de longitud 1: codigo completo; cada division de una hoja lo mantiene completo
        k = rng.randrange(2, min(count, 80) + 1)
        guard = 0
        while len(leaves) < k and guard < 1000:
            guard += 1
            i = rng.randrange(len(leaves))
            if leaves[i] >= maxlen:
                if all(l >= maxlen for l in leaves):
                    break
                continue
            l = leaves.pop(i)
            leaves += [l + 1, l + 1]
        idx = rng.sample(range(count), min(len(leaves), count))
        for i, l in zip(idx, leaves):
            lens[i] = l
        return lens
    if style == 1:  # aleatorio disperso
        return [rng.randrange(0, maxlen + 1) if rng.random() < .3 else 0 for _ in range(count)]
    if style == 2:  # aleatorio denso
        return [rng.randrange(0, maxlen + 1) for _ in range(count)]
    if style == 3:  # uno o dos simbolos
        lens = [0] * count
        for _ in range(rng.randrange(0, 3)):
            if count:
                lens[rng.randrange(count)] = rng.randrange(1, maxlen + 1)
        return lens
    if style == 4:  # casi completo con un cambio
        lens = rand_lengths(rng, count, maxlen, 0)
        nz = [i for i, l in enumerate(lens) if l]
        if nz:
            i = rng.choice(nz)
            lens[i] = max(1, min(maxlen, lens[i] + rng.choice([-1, 1])))
        return lens
    return [rng.choice([maxlen, maxlen - 1, 1, 2, 3, 0]) for _ in range(count)]


def gen_lens(path, n, rng):
    with open(path, 'wb') as f:
        f.write(b'LZL1' + struct.pack('<I', n))
        for i in range(n):
            typ = rng.choice([0, 1, 1, 2, 2, 3])  # 0 bits(19) 1 dynamic 2 huft literal/longitudes 3 huft distancias
            style = rng.choice([0, 0, 0, 0, 0, 4, 4, 1, 2, 3, 5])
            if typ == 0:
                nl, nd, m = 19, 0, rng.randrange(1, 9)
                lens = rand_lengths(rng, 19, 7, style)
            elif typ == 1:
                nl = rng.randrange(257, 289)
                nd = rng.randrange(1, 33)
                lens = rand_lengths(rng, nl, 15, style) + rand_lengths(rng, nd, 15, rng.choice([0, 0, 0, 0, 4, 1, 2, 3, 5]))
                m = rng.randrange(5, 11)
            elif typ == 2:
                nl, nd, m = rng.randrange(1, 289), 0, rng.randrange(0, 12)
                lens = rand_lengths(rng, nl, 15, style)
            else:
                nl, nd, m = rng.randrange(1, 31), 0, rng.randrange(0, 12)
                lens = rand_lengths(rng, nl, 15, style)
            if typ == 1:
                kl, kd = kraft(lens[:nl]), kraft(lens[nl:])
            else:
                kl, kd = kraft(lens), 0
            f.write(struct.pack('<IIIIIII', rng.getrandbits(32), typ, nl, nd, m, kl, kd) + bytes(lens) + bytes(320 - len(lens)))


def gen_flush(path, n, rng):
    with open(path, 'wb') as f:
        f.write(b'LZF1' + struct.pack('<I', n))
        for i in range(n):
            w = rng.choice([1, 2, 3, 16, 256, 512, 1000, 4096, 32768])
            win = bytes(rng.randrange(256) for _ in range(w))
            read = rng.randrange(0, w + 1) if rng.random() < .8 else rng.choice([0, w])
            write = rng.randrange(0, w + 1)
            if rng.random() < .3:
                write = w
            if rng.random() < .2:
                write = read
            avail = rng.choice([0, 1, 2, 5, 100, 4096, 40000, rng.randrange(0, w + 50)])
            r_in = rng.choice([0, -5, -5, 1, -3])
            use_check = rng.random() < .7
            check0 = ((rng.randrange(65521) << 16) | rng.randrange(65521)) if rng.random() < .3 else 1
            # modelo independiente: la especificacion de infutil.c escrita aqui con el modulo zlib de Python, no copiada del banco C
            end = w
            rd = read
            wr = write
            out = bytearray()
            av = avail
            r = r_in
            chk = check0

            def seg(a, b):
                nonlocal av, r, chk
                n_ = max(0, b - a)
                n_ = min(n_, av)
                if n_ and r == -5:
                    r = 0
                av -= n_
                chunk = win[a:a + n_]
                out.extend(chunk)
                if use_check and n_:
                    chk = zlib.adler32(chunk, chk) & 0xFFFFFFFF
                return n_

            n1 = seg(rd, (wr if rd <= wr else end))
            rd += n1
            if rd == end:
                rd = 0
                if wr == end:
                    wr = 0
                n2 = seg(0, wr)
                rd += n2
            f.write(struct.pack('<IIIIIIIIII', w, read, write, avail, r_in & 0xFFFFFFFF, int(use_check), check0, r & 0xFFFFFFFF, rd, wr))
            f.write(struct.pack('<III', av, chk, len(out)) + win + bytes(out))


if __name__ == '__main__':
    out = sys.argv[1]
    os.makedirs(out, exist_ok=True)
    ns = int(sys.argv[2]) if len(sys.argv) > 2 else 17000
    nl = int(sys.argv[3]) if len(sys.argv) > 3 else 17000
    nf = int(sys.argv[4]) if len(sys.argv) > 4 else 17000
    rng = random.Random(0x1A1A)
    gen_streams(os.path.join(out, 'streams.bin'), ns, rng)
    gen_lens(os.path.join(out, 'lens.bin'), nl, rng)
    gen_flush(os.path.join(out, 'flush.bin'), nf, rng)
    print('ok', ns, nl, nf)
