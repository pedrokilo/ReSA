# -*- coding: utf-8 -*-
"""Tabla de 124 sustituciones del GXT de GInputSA (ApplyGXTFixes) y vectores del banco de expresiones regulares.
  1) Lee la tabla de TRES fuentes y las compara: el .asi (las 68 entradas estaticas de 0x100384D0, struct de 0x28 bytes), el catalogo docs/CodigoVivoExe/ginput/ginput-tablas.md (124) y
     source/game_sa/PadButtonIcons.cpp (kGxtFixes, la tabla del juego).
  2) Lee los textos reales de las 124 claves del american.gxt de la carpeta del juego (solo lectura) para usarlos como entrada tipica.
  3) Escribe regex_vectores.tsv: clave, patron, reemplazo, texto de entrada, resultado esperado calculado con `re` de Python (motor distinto del de C++), para bench_regex.cpp.
Uso: python regex_tabla.py <salida.tsv> [american.gxt]"""
import os, re, struct, sys
HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
sys.path.insert(0, os.path.join(ROOT, 'contrib', 'ginput_analisis'))


def tabla_asi():
    """68 entradas estaticas en 0x100384D0: char key[8]; const char* patron; const char* reemplazo; (regex vacio) ..."""
    import pefile
    path = r"D:\Original\_ginput_ref\src\Original(Reworked)(Incl.All TXD's)\GInputSA.asi"
    pe = pefile.PE(path); base = pe.OPTIONAL_HEADER.ImageBase; data = open(path, 'rb').read()
    def rd(va, n):
        rva = va - base
        for s in pe.sections:
            if s.VirtualAddress <= rva < s.VirtualAddress + max(s.Misc_VirtualSize, s.SizeOfRawData):
                o = s.PointerToRawData + rva - s.VirtualAddress
                return data[o:o + n]
        return b''
    def cs(va):
        return rd(va, 200).split(b'\0')[0].decode('latin-1')
    out = []
    for i in range(68):
        e = rd(0x100384D0 + 0x28 * i, 0x28)
        key = e[:8].split(b'\0')[0].decode('latin-1')
        p, r = struct.unpack_from('<II', e, 8)
        out.append((key, cs(p) if p else None, cs(r) if r else None))
    return out


def tabla_catalogo():
    t = open(os.path.join(ROOT, 'docs', 'CodigoVivoExe', 'ginput', 'ginput-tablas.md'), encoding='utf-8').read()
    sec = t.split('## 1.')[1].split('## 2.')[0]
    out = []
    for m in re.finditer(r'^\| (\d+) \| ([^|]+) \| (?:`([^`]*)`|[^|]*) \| (?:`([^`]*)`|[^|]*) \|', sec, re.M):
        if m.group(4) is None and 'texto completo' in m.group(0):  # sin patron: el texto completo sustituye al original
            out.append((m.group(2).strip(), None, m.group(3)))
        else:
            out.append((m.group(2).strip(), m.group(3), m.group(4)))
    return out


def cstr(s):
    return bytes(s, 'latin-1').decode('unicode_escape')


def tabla_juego():
    src = open(os.path.join(ROOT, 'source', 'game_sa', 'PadButtonIcons.cpp'), encoding='utf-8').read()
    body = src.split('constexpr GxtFix kGxtFixes[] = {')[1].split('};')[0]
    out = []
    for m in re.finditer(r'\{\s*"((?:[^"\\]|\\.)*)"\s*,\s*(nullptr|"(?:[^"\\]|\\.)*")\s*,\s*"((?:[^"\\]|\\.)*)"\s*\}', body):
        pat = None if m.group(2) == 'nullptr' else cstr(m.group(2)[1:-1])
        out.append((cstr(m.group(1)), pat, cstr(m.group(3))))
    return out


CRC = []
for i in range(256):
    c = i
    for _ in range(8):
        c = (c >> 1) ^ 0xEDB88320 if c & 1 else c >> 1
    CRC.append(c)


def jamcrc_upper(s):
    h = 0xFFFFFFFF
    for ch in s.upper().encode('latin-1'):
        h = CRC[(h ^ ch) & 0xFF] ^ (h >> 8)
    return h


def leer_gxt(path):
    d = open(path, 'rb').read()
    assert d[8:12] == b'TABL' or d[:4] == b'\x04\x00\x08\x00' or True
    pos = 4  # version/tamano de caracter
    assert d[pos:pos + 4] == b'TABL', d[:16]
    tsize = struct.unpack_from('<I', d, pos + 4)[0]
    tablas = []
    for o in range(pos + 8, pos + 8 + tsize, 12):
        nombre = d[o:o + 8].split(b'\0')[0].decode('latin-1'); off = struct.unpack_from('<I', d, o + 8)[0]; tablas.append((nombre, off))
    textos = {}
    for nombre, off in tablas:
        p = off
        if nombre != 'MAIN':
            p += 8  # nombre de la tabla antes de TKEY
        assert d[p:p + 4] == b'TKEY', (nombre, d[p:p + 8])
        ksize = struct.unpack_from('<I', d, p + 4)[0]
        ents = [struct.unpack_from('<II', d, p + 8 + 8 * i) for i in range(ksize // 8)]
        q = p + 8 + ksize
        assert d[q:q + 4] == b'TDAT'
        dsize = struct.unpack_from('<I', d, q + 4)[0]
        dat = d[q + 8:q + 8 + dsize]
        for o, h in ents:
            e = dat.index(b'\0', o)
            textos.setdefault(h, []).append((nombre, dat[o:e].decode('latin-1')))
    return textos


def a_python(rep):
    return re.sub(r'\$(\d)', r'\\g<\1>', rep.replace('\\', '\\\\'))


def main(out, gxt):
    A, C, J = tabla_asi(), tabla_catalogo(), tabla_juego()
    print('asi (estaticas): %d  catalogo: %d  juego (kGxtFixes): %d' % (len(A), len(C), len(J)))
    assert len(C) == 124 and len(J) == 124, (len(C), len(J))
    difC = [i for i in range(124) if C[i] != J[i]]
    difA = [i for i in range(len(A)) if A[i] != C[i]]
    print('catalogo vs juego: %d filas distintas %s' % (len(difC), difC[:10]))
    print('asi (68 estaticas) vs catalogo: %d filas distintas %s' % (len(difA), difA[:10]))
    textos = leer_gxt(gxt)
    n_real = 0
    with open(out, 'w', encoding='utf-8', newline='\n') as f:
        for key, pat, rep in J:
            h = jamcrc_upper(key)
            reales = textos.get(h, [])
            entradas = []
            for tabla, t in reales:
                entradas.append(('gxt:' + tabla, t))
            if pat:
                # sintetico: el patron con cada `.+` sustituido por texto y cada `(.+)` por otro, rodeado de texto
                sint = re.sub(r'\(\.\+\)', ' MEDIO ', pat); sint = sint.replace('.+', ' xxx ')
                entradas.append(('sintetico', 'Pulsa ' + sint + ' para continuar.'))
                entradas.append(('sin-secuencia', 'Texto sin ninguna etiqueta de tecla.'))
                entradas.append(('doble', 'A ' + sint + ' B ' + sint + ' C'))
            else:
                entradas.append(('sin-patron', 'cualquier texto'))
            for origen, t in entradas:
                if pat:
                    rx = re.compile(pat)
                    esp = rx.sub(a_python(rep), t) if rx.search(t) else None  # None = no hay coincidencia: el juego devuelve el texto original
                else:
                    esp = rep
                n_real += origen.startswith('gxt:')
                f.write('\t'.join([key, pat or '', rep, origen, t.replace('\t', ' '), '<<SIN COINCIDENCIA>>' if esp is None else esp]) + '\n')
    claves_sin_texto = [k for k, _, _ in J if jamcrc_upper(k) not in textos]
    print('textos reales del GXT para %d de 124 claves (entradas reales: %d); claves sin texto en este idioma: %s' % (124 - len(claves_sin_texto), n_real, claves_sin_texto[:20]))


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else r'D:\Grand Theft Auto San Andreas\text\american.gxt')
