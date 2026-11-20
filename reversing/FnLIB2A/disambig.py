"""Desambigua las candidatas multiples de resolve.py (lineas '?nombre,addr1;addr2;...' de confirmadas.csv): repite cada candidata con 4.000 casos
y se queda con las de 0 diferencias; si queda una sola la confirma.  Uso: python disambig.py <bench.exe> <ruta del EXE> <tanda.txt>"""
import csv, re, subprocess, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
bench, exe, tanda = sys.argv[1:4]
rows = [r for r in csv.reader(open(HERE / 'confirmadas.csv')) if r]
conf = {r[0]: r[1] for r in rows if not r[0].startswith('#')}
amb = {k[1:]: v.split(';') for k, v in conf.items() if k.startswith('?')}
for name, cands in amb.items():
    if name in conf: continue
    ok = []
    for a in cands:
        lines = [l for l in open(HERE / 'tabla.txt') if l.split()[0] == name]
        if not lines: break
        t = lines[0].split(); t[4] = a
        tmp = HERE / '_amb.txt'; tmp.write_text(' '.join(t) + chr(10))
        try: r = subprocess.run([bench, exe, str(tmp), '--n', '4000', '--diffs', '0', '--filtro', name], capture_output=True, text=True, timeout=300)
        except subprocess.TimeoutExpired: continue
        m = re.search(r'^%s\s+0x[0-9A-F]+\s+(\d+)\s+(\d+)' % name, r.stdout, re.M)
        if m and int(m.group(2)) == 0: ok.append(a)
    print(name, cands, '->', ok, flush=True)
    if len(ok) == 1: conf[name] = ok[0]
    elif ok: conf['?' + name] = ';'.join(ok)
    else: conf.pop('?' + name, None)
(HERE / '_amb.txt').unlink(missing_ok=True)
with open(HERE / 'confirmadas.csv', 'w', newline='') as f:
    w = csv.writer(f); w.writerow(['# nombre', 'va_exe'])
    for k, v in conf.items():
        if not k.startswith('#'): w.writerow([k, v])
