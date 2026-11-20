"""Corre la prueba negativa de una ronda: bench.exe compilado contra la libpng mutada (mutate.py), con LIB2A_MUT=k para cada funcion k de tabla.txt.
Cada mutacion debe producir diferencias EXE-vs-biblioteca en la funcion mutada. Escribe negative-runs/ronda<N>.json.
Uso: python neg_run.py <ronda> <bench_mutado.exe> <ruta del EXE> [pares por funcion]"""
import json, os, re, subprocess, sys
from pathlib import Path
HERE = Path(__file__).resolve().parent
rnd, exe, gta = sys.argv[1], sys.argv[2], sys.argv[3]
n = sys.argv[4] if len(sys.argv) > 4 else '3000'
tab = {l.split()[0]: l.split()[4] for l in (HERE / 'tabla.txt').read_text().splitlines() if l.strip()}
namesf = Path(sys.argv[5]) if len(sys.argv) > 5 else None
names = namesf.read_text().split() if namesf else (HERE / 'validadas.txt').read_text().split()
out = {}; ok_all = True
for k, name in enumerate(names, 1):
    if tab.get(name, '0') == '0': continue
    env = dict(os.environ, LIB2A_MUT=str(k))
    r = subprocess.run([exe, gta, str(HERE / 'tabla.txt'), '--filtro', name, '--n', n, '--diffs', '0'][0:1] + [gta, str(HERE / 'tabla.txt'), '--filtro', name, '--n', n, '--diffs', '0'], env=env, capture_output=True, text=True, timeout=1800)
    m = re.search(r'^%s\s+0x[0-9A-F]+\s+(\d+)\s+(\d+)' % re.escape(name), r.stdout, re.M)
    diffs = int(m.group(2)) if m else -1
    out[str(k)] = dict(funcion=name, pares=int(m.group(1)) if m else 0, diferencias=diffs, detectada=diffs > 0)
    ok_all &= diffs > 0
    print(f'ronda {rnd} mutacion {k:2d} {name:34s}', 'DETECTADA' if diffs > 0 else 'NO DETECTADA', diffs, flush=True)
(HERE / 'negative-runs').mkdir(exist_ok=True)
(HERE / 'negative-runs' / f'ronda{rnd}.json').write_text(json.dumps(out, indent=1))
print('RONDA', rnd, 'TODAS DETECTADAS' if ok_all else 'HAY MUTACIONES NO DETECTADAS')
sys.exit(0 if ok_all else 1)
