# Extrae de rpworld_s30a31a.cpp el bloque del lote S31A (cierre) sin los adaptadores de geometria (el banco aporta los suyos con una geometria simulada), para compilarlo en el banco (bench_tristrip.cpp).
# Uso (desde la raiz del repositorio): python reversing/FnS31AFILE/extract_bench.py <salida.inc>
import re
import sys

src = open('source/game_sa/RenderWare/rw/rpworld_s30a31a.cpp', encoding='utf-8').read()
a = src.index('static_assert(sizeof(RpTriStripPolygon) == 0x34')
b = src.index('// Lotes S30A y S31A (sin validar): ganchos')
blk = src[a:b]
# 1. _rpGeometryPluginFirstEntry
blk = re.sub(r'// 0x74BFD0 .*?\n\}\n', '', blk, flags=re.S)
# 2. adaptadores de geometria
blk = re.sub(r'uint32_t T_GeometryPluginFirst\(\).*?uint32_t T_MaterialListCopy[^\n]*\n', '', blk, flags=re.S)
open(sys.argv[1], 'w', encoding='utf-8').write(blk)
