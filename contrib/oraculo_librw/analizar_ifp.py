#!/usr/bin/env python3
"""Analiza archivos IFP (ANP3/ANP2) de GTA SA con el mismo criterio que CAnimManager::LoadAnimFile_ANP23:
mezcla de tipos de frame dentro de una animacion (la compresion la decide solo la primera secuencia), secuencias con 0 frames,
tipos de frame desconocidos y huesos del esqueleto de peds sin secuencia. Uso: python analizar_ifp.py archivo.ifp [...]"""
import struct, sys, collections
KF = {1: 20, 2: 32, 3: 10, 4: 16}
def parse(data):
    out = []
    if data[:4] not in (b'ANP3', b'ANP2'):
        return None, data[:4]
    anp3 = data[:4] == b'ANP3'
    p = 8
    blk = data[p:p+24].split(b'\0')[0].decode('latin1'); p += 24
    n, = struct.unpack_from('<i', data, p); p += 4
    for _ in range(n):
        name = data[p:p+24].split(b'\0')[0].decode('latin1'); p += 24
        nseq, = struct.unpack_from('<i', data, p); p += 4
        size = flags = 0
        if anp3:
            size, flags = struct.unpack_from('<ii', data, p); p += 8
        seqs = []
        for _ in range(nseq):
            sname = data[p:p+24].split(b'\0')[0].decode('latin1'); p += 24
            ft, nf, bone = struct.unpack_from('<iii', data, p); p += 12
            k = KF.get(ft, 0)
            p += nf * k
            seqs.append((sname, ft, nf, bone))
        out.append((name, flags, seqs))
    return (blk, out), None
def main():
    for fn in sys.argv[1:]:
        data = open(fn, 'rb').read()
        r, bad = parse(data)
        if r is None:
            print(fn, 'cabecera', bad, '(no ANP2/ANP3: ANPK u otro; no analizado)'); continue
        blk, anims = r
        mixed = zero = unk = noseq = 0; ex = []
        for name, flags, seqs in anims:
            types = {s[1] for s in seqs}
            if any(t not in KF for t in types): unk += 1; ex.append((name, 'tipo desconocido', sorted(types)))
            comp = {t in (3, 4) for t in types if t in KF}
            if len(comp) > 1: mixed += 1; ex.append((name, 'MEZCLA comprimido/no comprimido', sorted(types)))
            if any(s[2] <= 0 for s in seqs): zero += 1; ex.append((name, 'secuencia con 0 frames', [s[0] for s in seqs if s[2] <= 0]))
            if not seqs: noseq += 1
        print('%s: bloque=%s animaciones=%d mezcla=%d con_secuencia_0_frames=%d tipo_desconocido=%d sin_secuencias=%d' % (fn, blk, len(anims), mixed, zero, unk, noseq))
        for e in ex[:8]: print('    ', e)
        if len(sys.argv) == 2:
            bones = collections.Counter(s[3] for a in anims for s in a[2])
            print('    ids de hueso distintos:', len(bones), sorted(bones)[:60])
main()
