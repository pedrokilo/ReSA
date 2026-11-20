"""Prueba negativa: compila el banco contra cada copia mutada y comprueba que cada mutacion produce diferencias EXE-vs-biblioteca en SU funcion.
Uso: python -I neg_run.py <directorio de trabajo> <ruta del EXE> [n casos]    (escribe negative-runs/ronda1.json y ronda2.json; sale con 1 si alguna mutacion no se detecta)"""
import sys, os, subprocess, json, re
HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import mutate
work, exe = sys.argv[1], sys.argv[2]; n = sys.argv[3] if len(sys.argv) > 3 else '20000'
os.makedirs(os.path.join(HERE, 'negative-runs'), exist_ok=True)
allok = True
for ronda in (1, 2):
    mdir = os.path.join(work, 'mut%d' % ronda); bdir = os.path.join(work, 'bm%d' % ronda)
    subprocess.run([sys.executable, '-I', os.path.join(HERE, 'mutate.py'), str(ronda), mdir], check=True)
    r = subprocess.run(['cmd', '/c', os.path.join(HERE, 'build_bench.cmd'), bdir, mdir], capture_output=True, text=True)
    if not os.path.exists(os.path.join(bdir, 'bench.exe')): print(r.stdout[-2000:], r.stderr[-2000:]); sys.exit(2)
    res = []
    for k, test, fil, func, old, new in mutate.RONDAS[ronda]:
        env = dict(os.environ, LIB3A_MUT=str(k))
        p = subprocess.run([os.path.join(bdir, 'bench.exe'), os.path.join(work, 'neg_out'), exe, '--filtro', '=' + test, '--n', n], env=env, capture_output=True, text=True)
        m = re.search(r'^%s\s+0x[0-9A-F]+\s+(\d+)\s+(\d+)\s+(\d+)' % re.escape(test), p.stdout, re.M)
        pares, difs = (int(m.group(1)), int(m.group(2))) if m else (0, -1)
        res.append(dict(k=k, funcion=test, pares=pares, diferencias=difs, detectada=difs > 0)); allok &= difs > 0
        print('ronda %d mutacion %2d %-22s pares %6d diferencias %6d %s' % (ronda, k, test, pares, difs, 'DETECTADA' if difs > 0 else 'NO DETECTADA'))
    json.dump(res, open(os.path.join(HERE, 'negative-runs', 'ronda%d.json' % ronda), 'w'), indent=1)
print('TODAS DETECTADAS' if allok else 'HAY MUTACIONES SIN DETECTAR'); sys.exit(0 if allok else 1)
