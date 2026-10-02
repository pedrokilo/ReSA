"""Resumen compacto de un informe: reparto, evento final y primer desacuerdo (solo campos distintos)."""
import collections, json, sys
from pathlib import Path
folder = Path(sys.argv[1]); width = int(sys.argv[2]) if len(sys.argv) > 2 else 600
summary = json.loads((folder/'summary.json').read_text(encoding='utf-8'))
print('error:', summary.get('error'), '| dialog:', summary.get('dialog'))
log = folder/'comparisons.jsonl'
if not log.exists(): sys.exit()
rows = [json.loads(l) for l in log.read_text().splitlines() if l.strip()]
print(dict(collections.Counter(r.get('function') for r in rows if r['event'] == 'comparison')))
print('final:', json.dumps([r for r in rows if r['event'] != 'comparison'][-1])[:width])
bad = next((r for r in rows if r['event'] == 'comparison' and not r.get('accepted', r.get('equal'))), None)
if bad:
    print('primer fallo:', bad['function'], 'caso', bad.get('case'), 'par', bad['pair'])
    o, c = bad['original'], bad['candidate']
    for k in (o if isinstance(o, dict) else {'value': o}):
        ov, cv = (o, c) if not isinstance(o, dict) else (o[k], c.get(k))
        if ov != cv: print(f'  {k}\n    O {json.dumps(ov)[:width]}\n    C {json.dumps(cv)[:width]}')
