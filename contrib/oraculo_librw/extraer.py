#!/usr/bin/env python3
"""Extrae de las fuentes del proyecto, sin copiarlas al repositorio, el codigo que el banco compara con librw.
Uso: python extraer.py <raiz del repo> <carpeta de salida>
Genera: hanim_math_extraido.inc (X87Math.cpp, lote S11A) y hanim_update_extraido.inc (rphanim.cpp: ayudantes y RpHAnimHierarchyUpdateMatrices)."""
import re, sys, pathlib
raiz = pathlib.Path(sys.argv[1]); out = pathlib.Path(sys.argv[2]); out.mkdir(parents=True, exist_ok=True)
x = (raiz / 'source/game_sa/X87Math.cpp').read_text(encoding='utf-8', errors='replace')
a = x.index('// --- Lote S11A (sin validar): RpHAnim, fotogramas clave')
b = x.index('// --- Lote R12A (sin validar): vectores y matrices de RenderWare')
(out / 'hanim_math_extraido.inc').write_text(x[a:b], encoding='utf-8')
r = (raiz / 'source/game_sa/RenderWare/rw/rphanim.cpp').read_text(encoding='utf-8', errors='replace')
a1 = r.index('namespace {')
b1 = r.index('// Dato del plugin en cada marco')
a2 = r.index('RwBool RpHAnimHierarchyUpdateMatrices(RpHAnimHierarchy* hierarchy) {')
b2 = r.index('// 0x7C51A0: posicion')
txt = r[a1:b1] + '\n} // namespace\n' + r[a2:b2]
(out / 'hanim_update_extraido.inc').write_text(txt, encoding='utf-8')
print('OK', len(x[a:b]), len(txt))
