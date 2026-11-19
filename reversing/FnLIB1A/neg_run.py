"""Ejecuta las 14 corridas negativas de una ronda: bench.exe compilado contra la zlib mutada, con LIB1A_MUT=k (k = 1..14).
Cada mutacion k debe producir diferencias EXE-vs-biblioteca en la funcion k (orden de la tabla del banco).  Escribe negative-runs/ronda<N>.json.
Uso: python -I neg_run.py <ronda> <bench_mutado.exe> <carpeta de casos> <ruta del EXE> [casos por escenario]"""
import json, os, re, subprocess, sys
from pathlib import Path

NAMES = ['inflate_blocks_reset', 'inflate_blocks_new', 'inflate_blocks', 'inflate_blocks_free', 'inflate_set_dictionary', 'inflate_codes_new', 'inflate_codes', 'inflate_codes_free',
         'inflate_trees_bits', 'huft_build', 'inflate_trees_dynamic', 'inflate_trees_fixed', 'inflate_flush', 'inflate_fast']
# escenario del banco que ejerce cada funcion (filtro de bench.exe) y casos por escenario
PHASE = {1: ('dict', 3000), 2: ('codes', 3000), 3: ('stream', 2500), 4: ('dict', 3000), 5: ('dict', 4000), 6: ('codes', 3000), 7: ('codes', 4000), 8: ('codes', 3000),
         9: ('bits', 4000), 10: ('huft', 6000), 11: ('dyn', 4000), 12: ('fixed', 2000), 13: ('flush', 4000), 14: ('fast', 4000)}
PHASE2 = {4: ('codes', 4000)}  # en la ronda 2 la mutacion de inflate_blocks_free depende de una ventana de 8192 (solo en el escenario de codigos)
def main():
    rnd, exe, cases, gta = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]
    out = {}
    for k in range(1, 15):
        env = dict(os.environ, LIB1A_MUT=str(k))
        ph, mx = PHASE2[k] if rnd == '2' and k in PHASE2 else PHASE[k]
        r = subprocess.run([exe, cases, gta, '--filtro', ph, '--max', str(mx)], env=env, capture_output=True, text=True, timeout=3600)
        rows = {}
        for line in r.stdout.splitlines():
            m = re.match(r'^(\w+)\s+0x([0-9A-F]{6})\s+(\d+)\s+(\d+)\s+oraculo (\d+) \(fallos (\d+)/(\d+)\)', line)
            if m: rows[m.group(1)] = dict(pares=int(m.group(3)), diferencias=int(m.group(4)), oraculo=int(m.group(5)), fallos_exe=int(m.group(6)), fallos_lib=int(m.group(7)))
        tgt = NAMES[k - 1]
        td = rows.get(tgt, {})
        detected = td.get('diferencias', 0) > 0
        out[str(k)] = dict(funcion=tgt, detectada=detected, resultado=[l for l in r.stdout.splitlines() if l.startswith('RESULTADO')], diferencias_por_funcion={n: v['diferencias'] for n, v in rows.items() if v['diferencias']})
        print(f'ronda {rnd} mutacion {k:2d} {tgt:24s}', 'DETECTADA' if detected else 'NO DETECTADA', out[str(k)]['diferencias_por_funcion'], flush=True)
    Path(__file__).resolve().parent.joinpath('negative-runs').mkdir(exist_ok=True)
    Path(__file__).resolve().parent.joinpath('negative-runs', f'ronda{rnd}.json').write_text(json.dumps(out, indent=1, ensure_ascii=False))
    ok = all(v['detectada'] for v in out.values())
    print('RONDA', rnd, 'TODAS LAS MUTACIONES DETECTADAS' if ok else 'HAY MUTACIONES NO DETECTADAS')
    sys.exit(0 if ok else 1)
if __name__ == '__main__':
    main()
