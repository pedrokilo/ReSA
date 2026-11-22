"""Extrae de rpworld.cpp, tal cual, el texto que prueba el banco FnCAM: los enlaces de RpTie (TieUnlinkBoth, LightDestroyAllTies, AtomicDestroyAllTies) y el bloque
`Codigo vivo CAM` (envolturas del plugin 0x509 de camara, luz y atomo). Uso: python extract_cam.py <rpworld.cpp> <salida .inc>"""
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


a = src.index('// --- Codigo vivo CAM (sin validar): envolturas del plugin 0x509')
b = src.index('// 0x751500 (cdecl, (atomo, mundo)')
out = funcion('TieUnlinkBoth') + funcion('LightDestroyAllTies') + funcion('AtomicDestroyAllTies') + src[a:b]
open(sys.argv[2], 'w', encoding='latin-1', newline='').write(out)
