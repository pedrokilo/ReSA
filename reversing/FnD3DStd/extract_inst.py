# Extrae tal cual el texto de D3D9InstanceObject de rpworld_p6b.cpp (entre su comentario y el de la funcion siguiente) a inst_func.inc.
import sys
src, out = sys.argv[1], sys.argv[2]
s = open(src, encoding='utf-8').read()
a = s.index('// 0x756960 (codigo vivo D3DSTD')
b = s.index('// 0x756DF0 (lote P6B')
open(out, 'w', encoding='utf-8').write(s[a:b])
