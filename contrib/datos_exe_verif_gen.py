#!/usr/bin/env python3
"""Genera source/game_sa/DatosExe/VerifTabla.cpp: tabla {direccion del EXE, objeto propio, tamano} de todos los
objetos `extern T nombre; // 0xDIR` de source/game_sa/DatosExe/*.h (los que tienen tipo completo y tamano fijo).
La usa DatosExe/Verif.cpp (variable de entorno DATOSEXE_VERIF) para comparar en ejecucion el objeto propio con la
memoria de esa direccion (deteccion de 'dato en dos sitios'). Uso: python contrib/datos_exe_verif_gen.py"""
import re, glob, os, sys
root = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', 'source', 'game_sa', 'DatosExe')
decl = re.compile(r'^\s*extern\s+(const\s+)?(.+?)\s+(\w+)((?:\[[^\]]*\])*)\s*;\s*//\s*0x([0-9A-Fa-f]+)')
ents = []; cads = []; incs = []
for h in sorted(glob.glob(os.path.join(root, '*.h'))):
    ns = None; base = os.path.basename(h)
    if base in ('Verif.h', 'StaticInit.h'): continue
    found = False
    for line in open(h, encoding='utf-8', errors='replace'):
        m = re.match(r'namespace DatosExe::(\w+)', line)
        if m: ns = m.group(1)
        m = decl.match(line)
        if m:
            c, t, n, arr, a = m.groups()
            if '[]' in arr:
                if t.strip() == 'char' or t.strip().endswith('char'): cads.append((int(a, 16), ns, n)); found = True
                continue
            ents.append((int(a, 16), ns, n)); found = True
    if found: incs.append(base)
ents.sort(); cads.sort()
out = ['// GENERADO por contrib/datos_exe_verif_gen.py; no editar a mano.',
       '#include "StdInc.h"', '#include "Verif.h"']
for i in incs: out.append('#include "%s"' % i)
out.append('')
out.append('namespace DatosExe::Verif {')
out.append('extern const Entrada kTabla[] = {')
for a, ns, n in ents:
    out.append('    {0x%08X, "%s", &DatosExe::%s::%s, sizeof(DatosExe::%s::%s)},' % (a, n, ns, n, ns, n))
out.append('};')
out.append('extern const unsigned kTablaN = sizeof(kTabla) / sizeof(kTabla[0]);')
out.append('extern const Cadena kCadenas[] = {')
for a, ns, n in cads:
    out.append('    {0x%08X, "%s", DatosExe::%s::%s},' % (a, n, ns, n))
out.append('};')
out.append('extern const unsigned kCadenasN = sizeof(kCadenas) / sizeof(kCadenas[0]);')
out.append('}')
open(os.path.join(root, 'VerifTabla.cpp'), 'w', encoding='utf-8', newline='\n').write('\n'.join(out) + '\n')
print(len(ents), 'objetos,', len(cads), 'cadenas en', len(incs), 'cabeceras')
