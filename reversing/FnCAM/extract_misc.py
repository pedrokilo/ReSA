"""Extrae de rpworld.cpp, tal cual, las funciones puras del banco FnCAM-misc: RpGeometryStreamStructSize (0x74CE60), RpWorldSectorStreamDataSize (0x762440), RpWorldPlaneStreamSize (0x762040),
StripAcross y StripNextEdge (0x75C290). Uso: python extract_misc.py <rpworld.cpp> <salida .inc>"""
import re
import sys

src = open(sys.argv[1], encoding='latin-1', newline='').read().replace('\r\n', '\n')


def funcion(nombre):
    m = re.search(r'^(?:static )?(?:inline )?[\w:<>\*& ]+? \**' + re.escape(nombre) + r'\([^;{]*\)\s*\{', src, re.M)
    assert m, nombre
    i = src.index('{', m.start())
    depth = 0
    for j in range(i, len(src)):
        if src[j] == '{':
            depth += 1
        elif src[j] == '}':
            depth -= 1
            if depth == 0:
                return src[m.start():j + 1] + '\n\n'
    raise SystemExit('sin cierre: ' + nombre)


out = ''.join(funcion(n) for n in ('RpGeometryStreamStructSize', 'RpWorldSectorStreamDataSize', 'RpWorldPlaneStreamSize', 'StripAcross', 'StripNextEdge'))
open(sys.argv[2], 'w', encoding='latin-1', newline='').write(out)
