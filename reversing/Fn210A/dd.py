"""Resumen de desarrollo de una corrida: python dd.py [carpeta] [funcion] [n]  (por defecto la ultima; muestra por funcion las diferencias y las primeras n)"""
import json, sys, collections
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
import fixture_oracle as o
base = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test/resultados-fn210a')
d = Path(sys.argv[1]) if len(sys.argv) > 1 and sys.argv[1] != '-' else sorted(base.iterdir())[-1]
only = sys.argv[2] if len(sys.argv) > 2 else None
show = int(sys.argv[3]) if len(sys.argv) > 3 else 2
rows = [json.loads(l) for l in open(d / 'comparisons.jsonl', encoding='utf-8')]
cnt = collections.Counter(); tot = collections.Counter(); orc = collections.Counter(); shown = collections.Counter()
for r in rows:
    if r['event'] in ('error', 'first_chance_av'):
        print(json.dumps(r)[:1500])
    if r['event'] != 'comparison':
        continue
    f = r['function']; tot[f] += 1
    gates = all(r[s].get(k) is True for s in ('original', 'candidate') for k in ('stack_restored', 'nonvolatile_preserved', 'direction_flag_clear', 'seh_restored', 'route_ok', 'collaborators_dll', 'globals_restored', 'floating_state_restored'))
    bad = o.check_row(r)
    if bad and bad[0] == 'original':
        orc[f] += 1
    if not r['equal'] or not gates or bad:
        cnt[f] += 1
        if (only is None or only in f) and shown[f] < show:
            shown[f] += 1
            print('==', f, 'case', r['case'], 'fam', r['input']['family'], 'gates', gates, 'oracle', (bad[0], bad[1][:600]) if bad else None, 'fp', r['fp_difference'])
            if not r['equal']:
                for side in ('original', 'candidate'):
                    ob = r[side]['observed']
                    print('  ', side, 'ret', ob['ret'], 'ev', json.dumps(ob['events'])[:700], 'chg', json.dumps({k: v for k, v in ob['changes'].items() if v})[:300], 'why', r[side].get('collab_why'))
for f in tot:
    print('%-62s %5d casos, %5d con diferencias/compuertas/oraculo, lado EXE fuera del oraculo: %d' % (f, tot[f], cnt[f], orc[f]))
