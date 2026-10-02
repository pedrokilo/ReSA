"""Primera diferencia de cada funcion fallida en el ultimo informe de un banco: python first_diffs.py <prefijo> [ancho]."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parent))
from runtime_runner import GAME
prefix = sys.argv[1]; width = int(sys.argv[2]) if len(sys.argv) > 2 else 500
folder = sorted((GAME/f'resultados-{prefix}').iterdir())[-1]
rows = [json.loads(l) for l in (folder/'comparisons.jsonl').read_text().splitlines() if l.strip()]
end = [r for r in rows if r['event'] in ('error', 'assertion', 'complete')]
if end: print('final:', {k: v for k, v in end[-1].items() if k != 'stack'})
seen = set()
for r in rows:
    if r['event'] != 'comparison' or r['accepted'] or not r.get('original') or r['function'] in seen: continue
    seen.add(r['function']); o, c = r['original'], r['candidate']; print('\n#', r['function'], 'caso', r['case'])
    for k in o:
        if o[k] != c.get(k):
            if isinstance(o[k], list) and isinstance(c.get(k), list):
                i = next((i for i, (x, y) in enumerate(zip(o[k], c[k])) if x != y), min(len(o[k]), len(c[k])))
                print(f'  {k}[{i}] len {len(o[k])}/{len(c[k])}\n    O {json.dumps(o[k][i:i+3])[:width]}\n    C {json.dumps(c[k][i:i+3])[:width]}')
            else: print(f'  {k}\n    O {json.dumps(o[k])[:width]}\n    C {json.dumps(c.get(k))[:width]}')
