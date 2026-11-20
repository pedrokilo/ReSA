"""Genera libs/libjpeg-6b/wrap/w_*.c (envoltorios que exportan las funciones, tambien las static) y jpeg6b_exports.c. Uso: python gen_wrappers.py <carpeta con los .asm de compilar_objs.cmd>"""
import re, glob, os, sys
S = sys.argv[1]
J = os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..', 'libs', 'libjpeg-6b')
os.chdir(J)
files = sorted(os.path.basename(f)[:-2] for f in glob.glob('*.c') if not f.startswith('jpeg6b_'))
os.makedirs('wrap', exist_ok=True)
NL = chr(10)
tot = 0
for f in files:
    t = open(f + '.c', encoding='latin-1').read()
    names = list(dict.fromkeys(re.findall(r'^(?:METHODDEF|LOCAL|GLOBAL)\((?:[^)]*)\)\s*\n(\w+)\s*\(', t, re.M)))
    asm = open(os.path.join(S, f + '.asm'), encoding='latin-1').read()
    comp = set(n[1:] for n in re.findall(r"^(_\w+)\s+PROC", asm, re.M))
    names = [n for n in names if n in comp]; tot += len(names)
    lines = ['/* Envoltorio generado: incluye %s.c sin modificar y exporta sus funciones (ver jpeg6b_export.h y exe_config.h). */' % f,
             '#define JPEG_INTERNALS', '#include "../jinclude.h"', '#include "../jpeglib.h"', '#include "../exe_config.h"', '#include "../%s.c"' % f, '#include "../jpeg6b_export.h"',
             'const jpeg6b_export jpeg6b_exports_%s[] = {' % f]
    lines += ['    {"%s", (void*)%s},' % (n, n) for n in names] + ['    {0, 0}', '};', '']
    open('wrap/w_%s.c' % f, 'w', newline=NL).write(NL.join(lines))
lines = ['/* Generado: indice de los envoltorios wrap/w_*.c. */', '#include <string.h>', '#include "jpeg6b_export.h"']
lines += ['extern const jpeg6b_export jpeg6b_exports_%s[];' % f for f in files]
lines += ['static const struct { const char* file; const jpeg6b_export* t; } all[] = {']
lines += ['    {"%s", jpeg6b_exports_%s},' % (f, f) for f in files]
lines += ['    {0, 0}', '};', 'void* jpeg6b_find(const char* file, const char* name) {', '    int i, j;',
          '    for (i = 0; all[i].file; i++) if (!strcmp(all[i].file, file)) for (j = 0; all[i].t[j].name; j++) if (!strcmp(all[i].t[j].name, name)) return all[i].t[j].fn;',
          '    return 0;', '}', '']
open('jpeg6b_exports.c', 'w', newline=NL).write(NL.join(lines))
print(tot, 'funciones en', len(files), 'archivos')
