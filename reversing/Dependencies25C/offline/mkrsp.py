"""Genera un .rsp con las opciones de compilacion del proyecto (configuracion Debug de build/source/gta_reversed.vcxproj) para compilar fuera del juego el arnes aislado y los fuentes RenderWare.
Uso: python mkrsp.py <raiz del repositorio> <salida.rsp>   (por defecto, tres niveles por encima de este archivo y offline/out/proj.rsp)
Solo lee build/source/gta_reversed.vcxproj; no toca el directorio build."""
import re
import sys
from pathlib import Path

here = Path(__file__).resolve().parent
root = Path(sys.argv[1]).resolve() if len(sys.argv) > 1 else here.parents[2]
text = (root / 'build/source/gta_reversed.vcxproj').read_text(encoding='utf-8')
m = re.search(r"<ItemDefinitionGroup Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Debug\|Win32'\">(.*?)</ItemDefinitionGroup>", text, re.S)
cl = re.search(r"<ClCompile>(.*?)</ClCompile>", m.group(1), re.S).group(1)


def tag(name):
    x = re.search(r"<%s>(.*?)</%s>" % (name, name), cl, re.S)
    return x.group(1) if x else ''


inc = [p for p in tag('AdditionalIncludeDirectories').split(';') if p and '%(' not in p]
opts = tag('AdditionalOptions')
defs = [d for d in tag('PreprocessorDefinitions').split(';') if d and '%(' not in d]
lines = ['/nologo', '/c', '/std:c++latest', '/Od', '/MTd', '/EHsc', '/W3', '/Zi', '/RTC1', '/bigobj', '/utf-8', '/FS']
for d in defs:
    lines.append('/D' + d.replace('"', '\\"'))
for i in inc:
    lines.append('/I"%s"' % i)
for e in re.findall(r'/external:I "([^"]+)"', opts):
    lines.append('/external:I"%s"' % e.replace('/', chr(92)))
lines += ['/external:W0', '/Zc:preprocessor', '/wd4200 /wd4251 /wd4275 /wd4996 /wd26495 /wd26812 /wd28159', '/FI"StdInc.h"']
out = Path(sys.argv[2]).resolve() if len(sys.argv) > 2 else here / 'out' / 'proj.rsp'
out.parent.mkdir(parents=True, exist_ok=True)
out.write_text('\n'.join(lines), encoding='utf-8')
print(len(lines), 'lineas ->', out)
