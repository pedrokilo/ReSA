# Extrae de rwcore.cpp lo que necesita el banco: declaraciones (rangos del espacio anonimo del controlador D3D9) y el bloque de _rwD3D9System.
# Uso (desde la raiz del repositorio): python reversing/FnD3D9Device/extract_bench.py <salida.inc>
import sys
src = open('source/game_sa/RenderWare/rw/rwcore.cpp', encoding='utf-8').read()
a = src.index('constexpr size_t kSlotD3DGetAdapterModeCount')
m = 'D3D9DisplayModeLayout& D3D9CurrentMode = *reinterpret_cast<D3D9DisplayModeLayout*>(0xC9BEE4);\n'
b = src.index(m) + len(m)
c = src.index('// Profundidad asociada a un D3DFORMAT')
d = src.index('// Restaura la palabra de control x87 del EXE')
e = src.index('// --- 0x7F5F70: `D3D9System`')
f = src.index('// Lote R26A (sin validar): controlador D3D9 de RenderWare: multimuestreo (0x7F84E0')
out = 'namespace {\n' + src[a:b] + '\n' + src[c:d] + '}\n\n' + src[e:f]
open(sys.argv[1], 'w', encoding='utf-8').write(out)
