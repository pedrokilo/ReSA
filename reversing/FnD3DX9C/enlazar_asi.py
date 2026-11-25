"""FnD3DX9C: enlaza una copia del ASI (gta_reversed.asi) SIN recompilar el proyecto entero: toma de wv-int la lista de .obj y las opciones del enlazador
(build/source/gta_reversed.dir/Debug/gta_reversed.tlog/link.command.1.tlog), sustituye cada .obj por la version de reversing\\FnFase8\\over\\ de este arbol cuando existe
(archivos recompilados con compilar_over.cmd) y anade los .obj nuevos que solo estan en over\\. Misma idea que enlazar.cmd para el exe independiente.
Uso: python -I enlazar_asi.py <raiz del repo> <salida.asi> [raiz de wv-int con el build]   (ejecutar desde un entorno vcvars32: lo hace enlazar_asi.cmd)"""
import os, subprocess, sys
repo, salida = sys.argv[1], sys.argv[2]
base = sys.argv[3] if len(sys.argv) > 3 else r'D:\Original\wv-int'
tlog = os.path.join(base, 'build', 'source', 'gta_reversed.dir', 'Debug', 'gta_reversed.tlog', 'link.command.1.tlog')
raw = open(tlog, 'rb').read()
txt = raw.decode('utf-16') if raw[:2] in (b'\xff\xfe', b'\xfe\xff') else raw.decode('utf-8-sig')
lineas = txt.splitlines()
objs = [o for o in lineas[0].lstrip('^').split('|') if o.strip()]
opciones = lineas[1]
over = os.path.join(repo, 'reversing', 'FnFase8', 'over')
mios = {f.lower(): os.path.join(over, f) for f in os.listdir(over) if f.lower().endswith('.obj')}
usados = set()
final = []
for o in objs:
    n = os.path.basename(o).lower()
    if n in mios:
        final.append(mios[n])
        usados.add(n)
    else:
        final.append(o)
nuevos = [p for n, p in mios.items() if n not in usados]
final += nuevos
print('objs del build de referencia: %d; sustituidos por over\\: %d; nuevos de over\\: %d' % (len(objs), len(usados), len(nuevos)))
print('sustituidos:', sorted(usados), 'nuevos:', sorted(os.path.basename(x) for x in nuevos))
# opciones: salida propia, sin enlace incremental
import re
op = re.sub(r'/OUT:"[^"]*"', lambda m: '/OUT:"%s"' % salida, opciones)
op = op.replace('/INCREMENTAL', '/INCREMENTAL:NO')
op = re.sub(r'/ILK:"[^"]*"', '', op)
pdb = os.path.splitext(salida)[0] + '.pdb'
op = re.sub(r'/PDB:"[^"]*"', lambda m: '/PDB:"%s"' % pdb, op)
imp = os.path.splitext(salida)[0] + '.lib'
op = re.sub(r'/IMPLIB:"[^"]*"', lambda m: '/IMPLIB:"%s"' % imp, op)
rsp = salida + '.rsp'
with open(rsp, 'w', encoding='utf-8-sig') as f:
    f.write(op + '\n')
    for o in final:
        f.write('"%s"\n' % o)
cwd = os.path.join(base, 'build', 'source')   # las rutas de las libs son relativas a build\source
link = os.path.join(os.environ['VCToolsInstallDir'], 'bin', 'Hostx64', 'x86', 'link.exe')   # no el link de coreutils de Git
r = subprocess.run([link, '@' + rsp], cwd=cwd)
sys.exit(r.returncode)
