"""Extrae de rwcore.cpp y rwplcore.cpp el texto de las funciones reconstruidas (y de sus bloques auxiliares) a rw_cpp.inc, sin cambios.
Uso: python extract.py [rwcore.cpp] [rwplcore.cpp]   (por omision, los del repositorio: source/game_sa/RenderWare/rw/)"""
import re
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
rw = here.parents[2] / 'source/game_sa/RenderWare/rw'
core = Path(sys.argv[1]) if len(sys.argv) > 1 else rw / 'rwcore.cpp'
plcore = Path(sys.argv[2]) if len(sys.argv) > 2 else rw / 'rwplcore.cpp'

CORE_FUNCTIONS = ['RwTextureGetMipmapping', 'RwTextureGetAutoMipmapping', 'RwTextureRegisterPlugin', 'RwTexDictionaryRegisterPlugin', 'RwTextureRasterGenerateMipmaps',
                  'RwD3D9DynamicVertexBufferDestroy', 'RwD3D9DynamicVertexBufferUnlock', '_rwD3D9SetDepthStencilSurface', '_rwDeviceRegisterPlugin', 'RwD3D9DeleteVertexShader',
                  'RwD3D9CreatePixelShader', 'RwD3D9DeletePixelShader', 'RwRasterGetCurrentContext', 'RwRasterRegisterPlugin', 'RwRasterLockPalette', 'RwRGBAToPixel',
                  'RwTextureRegisterPluginStream', '_rwObjectHasFrameReleaseFrame']
PLCORE_FUNCTIONS = ['_rwPluginRegistryAddPlugin', '_rwPluginRegistryAddPluginStream', 'RwFreeListCreate', 'RwFreeListCreateAndPreallocateSpace']


def function_text(text, name):
    lines = text.split('\n')
    pattern = re.compile(r'^[A-Za-z_][^\n(=]*[\s\*]%s\(' % re.escape(name))
    hits = [i for i, l in enumerate(lines) if pattern.match(l) and l.rstrip().endswith('{')]
    if len(hits) != 1:
        raise SystemExit('%s: %d definiciones' % (name, len(hits)))
    start = hits[0]
    while start > 0 and lines[start - 1].startswith('//'):
        start -= 1
    end = start
    while lines[end] != '}':
        end += 1
    return '\n'.join(lines[start:end + 1]) + '\n'


def block(text, begin, end):
    a = text.index(begin)
    b = text.index(end, a) + len(end)
    return text[a:b] + '\n'


core_text = core.read_text(encoding='utf-8')
plcore_text = plcore.read_text(encoding='utf-8')
parts = ['// ---- rwplcore.cpp\n', block(plcore_text, 'namespace {\n// 0x8019B0', '} // namespace')]
parts += [function_text(plcore_text, n) + '\n' for n in PLCORE_FUNCTIONS]
parts += ['// ---- rwcore.cpp\n', block(core_text, 'namespace {\nauto& TextureModuleGlobalsOffset', '} // namespace')]
parts += [function_text(core_text, n) + '\n' for n in CORE_FUNCTIONS]
(here / 'rw_cpp.inc').write_text(''.join(parts), encoding='utf-8', newline='\n')
print('extraidas', len(PLCORE_FUNCTIONS) + len(CORE_FUNCTIONS), 'funciones de', core, 'y', plcore)
