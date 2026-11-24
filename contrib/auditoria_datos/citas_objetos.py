"""Citas por literal (0x...) de direcciones que caen dentro de un objeto propio DatosExe (no Rdata), sin comentarios.
Uso: python comparar_citas... (rama datos-g6g8). Salida a stdout: objeto, direccion citada, archivo:linea, texto."""
import os, re, sys, bisect
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import comparar_objetos as c
repo = sys.argv[1] if len(sys.argv) > 1 else '.'
d = c.leer_decls(repo)
objs = sorted((o for o in d.values() if o['ns'] != 'Rdata'), key=lambda o: o['va'])
vas = [o['va'] for o in objs]
def quitar_comentarios(t):
    t = re.sub(r'/\*.*?\*/', lambda m: re.sub(r'[^\n]', ' ', m.group(0)), t, flags=re.S)
    return re.sub(r'//[^\n]*', '', t)
lit = re.compile(r'0[xX]0*([0-9A-Fa-f]{6})\b')
out = []
for root, _, fs in os.walk(os.path.join(repo, 'source')):
    if 'DatosExe' in root and 'Imagen' in root: continue
    for f in fs:
        if not f.endswith(('.cpp', '.h', '.inl', '.hpp')): continue
        p = os.path.join(root, f)
        if os.sep + 'DatosExe' + os.sep in p and not p.endswith('StaticInit.cpp'): continue
        txt = quitar_comentarios(open(p, encoding='utf-8', errors='replace').read())
        for ln, line in enumerate(txt.split('\n'), 1):
            for m in lit.finditer(line):
                a = int(m.group(1), 16)
                if not (0x858000 <= a < 0xCB1000): continue
                i = bisect.bisect_right(vas, a) - 1
                if i < 0: continue
                o = objs[i]
                t = o['tam'] or 4
                if o['va'] <= a < o['va'] + t:
                    out.append((o['va'], o['ns'], o['name'], o['tipo'], t, a, os.path.relpath(p, repo).replace(chr(92), '/'), ln, line.strip()[:160]))
for r in sorted(out):
    print('0x%08X %s %s %s tam=%d cita=0x%08X %s:%d | %s' % r)
print('TOTAL', len(out), file=sys.stderr)
