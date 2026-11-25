# tamano real (sizeof) de los tipos de StaticRef cuyo tamano estatico no se pudo calcular: sonda de compilacion (error C2079 'Z<N>' indefinido)
import re, os, glob, sys, subprocess, json
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import estatico as E
tipos = {}
for p in glob.glob(os.path.join(E.SRC, '**', '*'), recursive=True):
    if not p.endswith(('.h', '.cpp', '.inl', '.hpp')) or os.sep + 'DatosExe' + os.sep in p: continue
    txt = E.strip_comments(open(p, encoding='utf-8', errors='replace').read())
    for ln, l in enumerate(txt.split('\n'), 1):
        c = re.sub(r'//.*', '', l)
        for m in E.sref.finditer(c):
            a = int(m.group(2), 16)
            if not (0x8E4000 <= a < 0xCB1000): continue
            if E.typesize(m.group(1)) is None: tipos.setdefault(m.group(1).strip(), []).append((os.path.relpath(p, E.SRC), ln, a))
print(len(tipos), 'tipos distintos sin tamano')
json.dump(tipos, open('tipos_sin_tam.json', 'w'))
# una sonda por tipo en su propio .cpp seria lenta; se agrupan por archivo fuente de la primera cita
por_arch = {}
for t, L in tipos.items(): por_arch.setdefault(L[0][0], []).append(t)
print(len(por_arch), 'archivos')
