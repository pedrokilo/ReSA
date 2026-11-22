"""Extrae de rwcore.cpp, tal cual, el texto que prueba el banco D3D de FnCAM: el helper COM, la extension D3D9 del raster, las constantes y estado del controlador (D3D9PresentLayout y los auto& a direcciones
del EXE), D3D9RestoreAfterReset y el bloque `Codigo vivo CAM` (_rwD3D9CameraEndUpdate, _rwD3D9RasterShowRaster, _rwD3D9StandardImageFindRasterFormat). Uso: python extract_d3d.py <rwcore.cpp> <salida .inc>"""
import sys

src = open(sys.argv[1], encoding='latin-1', newline='').read().replace('\r\n', '\n')


def tramo(ini, fin):
    a = src.index(ini)
    b = src.index(fin, a)
    return src[a:b]


m1 = tramo('// Llamada COM stdcall por la tabla virtual', 'constexpr size_t kSlotVertexBufferLock')
m2 = tramo('struct D3D9RasterExtLayout {', '// 0x4C9990 (lote 223A, sin validar')
m3 = tramo('constexpr size_t kSlotD3DGetAdapterModeCount', 'D3D9DisplayModeLayout& D3D9CurrentMode')
m3 += 'D3D9DisplayModeLayout& D3D9CurrentMode = *reinterpret_cast<D3D9DisplayModeLayout*>(0xC9BEE4);\n'
m4 = tramo('// Cadena comun tras un Reset correcto', '// Elige el nivel de multimuestreo')
m5 = tramo('// --- Codigo vivo CAM (sin validar): funciones estandar', '// 0x7F8A90 (lote R26A')
# m1 y m3 declaran (en su sitio original) dentro de un espacio de nombres anonimo; el helper D3D9GetTextureSurface ya esta en m2
out = 'namespace {\n' + m1 + 'auto& D3D9Device = StaticRef<void*>(0xC97C28);\n' + m2 + m3 + m4.replace('bool D3D9RestoreAfterReset() {', 'bool D3D9RestoreAfterReset() {', 1) + '}\n\n' + m5
open(sys.argv[2], 'w', encoding='latin-1', newline='').write(out)
