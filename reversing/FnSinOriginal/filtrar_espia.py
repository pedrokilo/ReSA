r"""Resume un espia.log: totales por funcion, rutas con 'gta-sa.exe'/'gta_sa.exe' (cualquier carpeta), rutas bajo 'Grand Theft Auto' (acceso directo al arbol del usuario),
y todos los .exe/.dll/.asi de fuera de C:\Windows tocados. Uso: python -I filtrar_espia.py <espia.log> [--exigir-vacio]  (sale con 1 si hay coincidencias de gta-sa.exe y se pidio --exigir-vacio)"""
import re, sys, collections
log = sys.argv[1]
cnt = collections.Counter(); n = 0; orig = []; gtadir = []; exes = collections.Counter(); malformadas = 0; errores = []
for l in open(log, encoding='utf-8', errors='replace'):
    l = l.rstrip('\n')
    if l.startswith('#ESPIA ERROR'): errores.append(l); continue
    if l.startswith('#'): continue
    p = l.split('\t', 3)
    if len(p) < 4: malformadas += 1; continue
    n += 1; cnt[p[2]] += 1; ruta = p[3]
    if re.search(r'gta[-_ ]?sa\.exe', ruta, re.I): orig.append(l)
    if re.search(r'grand theft auto', ruta, re.I): gtadir.append(l)
    if re.search(r'[.](exe|dll|asi)$', ruta, re.I) and not ruta.lower().startswith(chr(92) + '??' + chr(92) + 'c:' + chr(92) + 'windows' + chr(92)): exes[ruta] += 1
print('accesos registrados: %d (lineas malformadas %d)' % (n, malformadas)); print(dict(cnt))
if errores: print('ERRORES DEL ESPIA:', errores)
print('coincidencias con gta-sa.exe / gta_sa.exe (cualquier carpeta): %d' % len(orig))
for l in orig[:20]: print('   ', l)
print('rutas bajo "Grand Theft Auto" (acceso directo al arbol del usuario): %d' % len(gtadir))
for l in gtadir[:10]: print('   ', l)
print('exe/dll/asi fuera de C:/Windows tocados (%d distintos):' % len(exes))
for r, c in sorted(exes.items()): print('   %5d  %s' % (c, r))
if '--exigir-vacio' in sys.argv and orig: sys.exit(1)
