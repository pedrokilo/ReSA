"""Genera cl.rsp (opciones de compilacion de un archivo del proyecto, config Debug) a partir del .vcxproj de una compilacion ya hecha.
Uso: python -I gen_cl_rsp.py <vcxproj> <raiz_origen> <raiz_destino> <salida.rsp>"""
import re, sys
vc, src_root, dst_root, out = sys.argv[1:5]
t = open(vc, encoding='utf-8').read()
m = re.search(r"<ItemDefinitionGroup Condition=\"'\$\(Configuration\)\|\$\(Platform\)'=='Debug\|Win32'\">(.*?)</ItemDefinitionGroup>", t, re.S)
blk = m.group(1)
cl = re.search(r"<ClCompile>(.*?)</ClCompile>", blk, re.S).group(1)
def tag(n):
    x = re.search(r"<%s>(.*?)</%s>" % (n, n), cl, re.S)
    return x.group(1) if x else ''
incs = [i for i in tag('AdditionalIncludeDirectories').split(';') if i and '%(' not in i]
defs = [d for d in tag('PreprocessorDefinitions').split(';') if d and '%(' not in d]
opts = tag('AdditionalOptions').replace('%(AdditionalOptions)', '').strip()
lines = ['/nologo', '/c', '/std:c++latest', '/EHsc', '/MTd', '/Od', '/Z7', '/FS', '/W3', '/wd4200', '/wd4251', '/wd4275', '/wd4996', '/wd26495', '/wd26812', '/wd28159', '/Zc:preprocessor']
for i in incs:
    lines.append('/I"%s"' % i)
for d in defs:
    lines.append('/D' + d.replace('"', '\\"'))
if opts:
    lines.append(opts)
s = '\n'.join(lines)
s = s.replace(src_root, dst_root).replace(src_root.replace('\\', '/'), dst_root.replace('\\', '/'))
open(out, 'w', encoding='utf-8').write(s)
print(len(incs), 'includes', len(defs), 'defs')
