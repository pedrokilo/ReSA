"""Genera source/game_sa/RenderWare/rw/libpngread.cpp: un hook por funcion de lectura de libpng (lista de 'enganchables.txt' = funciones con 0 diferencias en el banco)
que llama a la biblioteca oficial libpng 1.0.12 (libs/) cuando se compila con GTASA_WITH_ZLIB_LIBPNG; con la opcion apagada (por defecto) no se engancha nada y el EXE original sigue
ejecutando su propia libpng.  Direcciones: confirmadas.csv.  Uso: python gen_hooks.py"""
import csv, re
from pathlib import Path
import protos
HERE = Path(__file__).resolve().parent
ROOT = HERE.parents[1]
P = protos.load()
conf = {l.split()[0]: l.split()[4] for l in open(HERE / 'tabla.txt') if l.strip() and l.split()[4] != '0'}   # direcciones de la tabla del banco (las validadas)
names = [n for n in (HERE / 'enganchables.txt').read_text().split() if n in conf]

def ctype(t, ret=False):
    t = ' '.join(t.replace('FAR', '').replace('const', '').split())
    if ret and t == 'void': return 'void'
    base = ' '.join(t.replace('*', ' * ').split())
    parts = base.split()
    typ = ' '.join(p for p in parts[:-1]) if (not ret and len(parts) > 1) else base
    if '*' in t or typ.endswith('p') or typ.endswith('pp') or typ in ('png_voidp',): return 'void*'
    if 'double' in typ: return 'double'
    if 'float' in typ: return 'float'
    if typ == 'png_byte': return 'uint8' if ret else 'uint32'
    if typ == 'png_uint_16': return 'uint16' if ret else 'uint32'
    if typ in ('int', 'png_int_32', 'png_fixed_point'): return 'int32'
    return 'uint32'

def sig(n):
    ret, args = P[n]
    rt = ctype(ret.strip(), True)
    al = []
    for a in args:
        a = ' '.join(a.split()); nm = a.replace('*', ' ').split()[-1]
        t = ctype(a[:a.rfind(nm)].strip() if a != 'void' else a)
        al.append((t, nm))
    return rt, al

out = []
out.append('''// Lote LIB2A: funciones de lectura de libpng 1.0.12 del EXE (0x7D0000..0x7E9400), sustituidas por la biblioteca oficial (libs/libpng-1.0.12) detras de la opcion CMake
// GTASA_WITH_ZLIB_LIBPNG (APAGADA por defecto: con la opcion apagada este archivo no engancha nada y el EXE original sigue ejecutando su propia libpng).
// Cada cuerpo solo reenvia a la biblioteca; el banco par a par (reversing/FnLIB2A, EXE original vs biblioteca, FPU a 24 bits, /arch:IA32) comprobo que cada una se comporta igual que la del EXE.
// Archivo GENERADO por reversing/FnLIB2A/gen_hooks.py desde reversing/FnLIB2A/enganchables.txt; no editar a mano.
#include "StdInc.h"

#include "libpngread.h"

#ifdef GTASA_WITH_ZLIB_LIBPNG
extern "C" {''')
for n in names:
    rt, al = sig(n)
    out.append(f'    {rt} {n}({", ".join(t + " " + nm for t, nm in al) or "void"});')
out.append('}\n\nnamespace {')
for n in names:
    rt, al = sig(n)
    call = f'::{n}({", ".join(nm for t, nm in al)})'
    out.append(f'{rt} __cdecl LpRd_{n}({", ".join(t + " " + nm for t, nm in al) or "void"}) {{ {"" if rt == "void" else "return "}{call}; }}')
out.append('}\n\nvoid LibPngReadInjectHooks() {\n    RH_ScopedNamespaceName("LibPngRead");\n    RH_ScopedCategory("Plugins");')
for n in names:
    out.append(f'    RH_ScopedGlobalInstall(LpRd_{n}, 0x{int(conf[n], 16):X});')
out.append('}\n#else\nvoid LibPngReadInjectHooks() {}\n#endif')
d = ROOT / 'source/game_sa/RenderWare/rw'
(d / 'libpngread.cpp').write_text('\n'.join(out) + '\n')
(d / 'libpngread.h').write_text('#pragma once\n\n// Lote LIB2A: hooks de la libpng de lectura (solo con GTASA_WITH_ZLIB_LIBPNG); ver reversing/FnLIB2A/README.md.\nvoid LibPngReadInjectHooks();\n')
print(len(names), 'hooks')
