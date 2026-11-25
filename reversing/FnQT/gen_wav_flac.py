"""Genera archivos de prueba para WaveProfilesFix (WAV 8/16/24 bits, mono/estereo, ID3, extensible) y UserTracksFLAC (FLAC "verbatim" valido).
Uso: python gen_wav_flac.py <carpeta de salida>
Escribe tambien esperado.txt: CRC32 del PCM estereo de 16 bits que debe producir el lector (se compara con wave_ext_probe).
"""
import math, os, struct, sys, zlib

SEG = 8.0  # segundos (>= 7 s: el juego descarta menos)


def tone(rate, ch, n):
    """n fotogramas, ch canales, entero de 24 bits con signo."""
    out = []
    for i in range(n):
        v = int(0.5 * 8388607 * math.sin(2 * math.pi * 440 * i / rate))
        w = int(0.25 * 8388607 * math.sin(2 * math.pi * 660 * i / rate))
        out.append((v, w) if ch == 2 else (v,))
    return out


def pack(frames, bits):
    b = bytearray()
    for fr in frames:
        for v in fr:
            if bits == 8:
                b.append(((v >> 16) + 128) & 255)
            elif bits == 16:
                b += struct.pack('<h', v >> 8)
            else:
                b += struct.pack('<i', v)[:3]
    return bytes(b)


def expected(frames, bits, ch):
    """PCM estereo de 16 bits esperado (reglas de SilentPatch: 8 bits -> (v-128)<<8, 24 bits -> 16 altos)."""
    out = bytearray()
    for fr in frames:
        vals = []
        for v in fr:
            if bits == 8:
                s = ((((v >> 16) + 128) & 255) - 128) << 8
            else:
                s = v >> 8
            vals.append(s)
        l = vals[0]
        r = vals[1] if ch == 2 else vals[0]
        out += struct.pack('<hh', l, r)
    return bytes(out)


def riff(chunks):
    body = b'WAVE' + b''.join(chunks)
    return b'RIFF' + struct.pack('<I', len(body)) + body


def chunk(cid, data):
    return cid + struct.pack('<I', len(data)) + data + (b'\0' if len(data) & 1 else b'')


def fmt(rate, ch, bits, extensible=False):
    ba = ch * bits // 8
    if not extensible:
        return chunk(b'fmt ', struct.pack('<HHIIHH', 1, ch, rate, rate * ba, ba, bits))
    guid = struct.pack('<HH', 1, 0) + bytes.fromhex('00001000800000aa00389b71')
    return chunk(b'fmt ', struct.pack('<HHIIHHHHI', 0xFFFE, ch, rate, rate * ba, ba, bits, 22, bits, 3) + guid)


def id3v2(n=300):
    body = b'TIT2' + struct.pack('>I', n - 11) + b'\0\0\3' + b'x' * (n - 11)
    size = len(body)
    ss = bytes([(size >> 21) & 127, (size >> 14) & 127, (size >> 7) & 127, size & 127])
    return b'ID3\x03\x00\x00' + ss + body


# --- FLAC verbatim --------------------------------------------------------------------------------------------------
def crc8(data):
    c = 0
    for b in data:
        c ^= b
        for _ in range(8):
            c = ((c << 1) ^ 0x07) & 255 if c & 128 else (c << 1) & 255
    return c


def crc16(data):
    c = 0
    for b in data:
        c ^= b << 8
        for _ in range(8):
            c = ((c << 1) ^ 0x8005) & 0xFFFF if c & 0x8000 else (c << 1) & 0xFFFF
    return c


def utf8_num(n):
    if n < 0x80:
        return bytes([n])
    out = bytearray()
    first = 0xC0
    limit = 0x20
    k = n
    tail = []
    while k >= limit:
        tail.append(0x80 | (k & 0x3F))
        k >>= 6
        first = 0x80 | (first >> 1)
        limit >>= 1
    out.append((first | k) & 255)
    out += bytes(reversed(tail))
    return bytes(out)


def flac(frames, rate, ch, bits, block=4096):
    sd = bytearray()
    # STREAMINFO
    total = len(frames)
    si = struct.pack('>HH', block, block) + b'\0\0\0' + b'\0\0\0'
    packed = (rate << 44) | ((ch - 1) << 41) | ((bits - 1) << 36) | total
    si += packed.to_bytes(8, 'big') + b'\0' * 16
    out = bytearray(b'fLaC' + bytes([0x80, 0, 0, 34]) + si)
    bcode = {4096: 12}[block]
    rcode = {44100: 9, 48000: 10, 22050: 6, 32000: 8, 8000: 4}.get(rate)
    bscode = {8: 1, 16: 4, 24: 6}[bits]
    n = 0
    fnum = 0
    while n < total:
        cnt = min(block, total - n)
        part = frames[n:n + cnt]
        if cnt != block:  # ultimo bloque: tamano explicito de 16 bits
            bc = 7
            ext = struct.pack('>H', cnt - 1)
        else:
            bc, ext = bcode, b''
        hdr = bytes([0xFF, 0xF8, (bc << 4) | (rcode if rcode else 0), ((ch - 1) << 4) | (bscode << 1)]) + utf8_num(fnum) + ext
        if not rcode:
            raise SystemExit('frecuencia no soportada en el generador')
        hdr += bytes([crc8(hdr)])
        bits_out = []
        for c in range(ch):
            bits_out.append((0, 1, 0))  # relleno + tipo VERBATIM (000001) + sin wasted: 8 bits
        # cada subtrama: 8 bits de cabecera (0, 000001, 0) y muestras de `bits` bits
        w = bytearray()
        acc = 0
        nb = 0

        def put(v, k):
            nonlocal acc, nb
            acc = (acc << k) | (v & ((1 << k) - 1))
            nb += k
            while nb >= 8:
                w.append((acc >> (nb - 8)) & 255)
                nb -= 8
            acc &= (1 << nb) - 1
        for c in range(ch):
            put(0b00000010, 8)
            for fr in part:
                put(fr[c] >> (24 - bits), bits)
        assert nb == 0
        body = hdr + bytes(w)
        out += body + struct.pack('>H', crc16(body))
        n += cnt
        fnum += 1
    return bytes(out)


def main(d):
    os.makedirs(d, exist_ok=True)
    exp = []
    casos = [
        ('wav_16_estereo_44100_canonico', 44100, 2, 16, 'canonico'),
        ('wav_16_estereo_44100_id3_tras_datos', 44100, 2, 16, 'id3tras'),
        ('wav_16_mono_22050', 22050, 1, 16, 'plano'),
        ('wav_8_estereo_32000', 32000, 2, 8, 'plano'),
        ('wav_8_mono_8000', 8000, 1, 8, 'plano'),
        ('wav_24_estereo_48000_id3_antes', 48000, 2, 24, 'id3antes'),
        ('wav_24_mono_44100_extensible', 44100, 1, 24, 'ext'),
        ('wav_16_estereo_44100_id3_inicio', 44100, 2, 16, 'id3inicio'),
    ]
    for name, rate, ch, bits, kind in casos:
        n = int(rate * SEG)
        frames = tone(rate, ch, n)
        pcm = pack(frames, bits)
        ch_list = [fmt(rate, ch, bits, kind == 'ext')]
        if kind == 'id3antes':
            ch_list.append(chunk(b'id3 ', id3v2()))
        ch_list.append(chunk(b'data', pcm))
        if kind == 'id3tras':
            ch_list.append(chunk(b'id3 ', id3v2()))
            ch_list.append(chunk(b'LIST', b'INFOISFT' + struct.pack('<I', 5) + b'test\0'))
        data = riff(ch_list)
        if kind == 'id3inicio':
            data = id3v2() + data
        open(os.path.join(d, name + '.wav'), 'wb').write(data)
        e = expected(frames, bits, ch)
        exp.append('%s %08X %d' % (name + '.wav', zlib.crc32(e) & 0xFFFFFFFF, len(e)))
    # WAV que no debe aceptarse: float de 32 bits y 16 bits a 96 kHz
    open(os.path.join(d, 'wav_float32_rechazado.wav'), 'wb').write(riff([chunk(b'fmt ', struct.pack('<HHIIHH', 3, 2, 44100, 44100 * 8, 8, 32)), chunk(b'data', b'\0' * 8 * 44100 * 8)]))
    open(os.path.join(d, 'wav_96000_rechazado.wav'), 'wb').write(riff([fmt(96000, 2, 16), chunk(b'data', b'\0' * 4 * 96000 * 8)]))
    # FLAC
    for name, rate, ch, bits in [('flac_16_estereo_44100', 44100, 2, 16), ('flac_24_mono_48000', 48000, 1, 24), ('flac_8_estereo_44100', 44100, 2, 8)]:
        frames = tone(rate, ch, int(rate * SEG))
        open(os.path.join(d, name + '.flac'), 'wb').write(flac(frames, rate, ch, bits))
        e = expected(frames, bits, ch)
        exp.append('%s %08X %d' % (name + '.flac', zlib.crc32(e) & 0xFFFFFFFF, len(e)))
    open(os.path.join(d, 'esperado.txt'), 'w').write('\n'.join(exp) + '\n')
    print('\n'.join(exp))


main(sys.argv[1])
