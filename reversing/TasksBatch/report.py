"""Informe compacto del ultimo resultado del banco de tareas (o de la carpeta indicada)."""
import json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
from runtime_runner import GAME
folder = Path(sys.argv[1]) if len(sys.argv) > 1 else sorted((GAME/'resultados-tasks').iterdir())[-1]
rows = [json.loads(l) for l in (folder/'comparisons.jsonl').read_text().splitlines() if l.strip()]
reg = next((r['regions'] for r in rows if r['event'] == 'regions'), {})
end = [r for r in rows if r['event'] in ('error', 'assertion', 'complete')]
if end:
    e = end[-1]; print('final:', {k: v for k, v in e.items() if k != 'stack'})
    for k in ('ecx', 'edx', 'eax', 'access'):
        v = e.get(k)
        for n, (b, sz) in reg.items():
            if isinstance(v, int) and b <= v < b + sz: print(f'  {k} -> {n}+{v - b:#x}')
    if e['event'] != 'complete': print('  ultimo:', [r for r in rows if r['event'] in ('begin', 'side')][-2:])
print('fallos:', [(r['function'].split('Complex')[-1], r['failures']) for r in rows if r['event'] == 'function_summary' and r['failures']])
seen = set()
for r in rows:
    if r['event'] != 'comparison' or r['accepted'] or 'word_diff' not in r or r['function'] in seen: continue
    seen.add(r['function']); print('\n#', r['function'].split('Complex')[-1], 'caso', r['case'], r['word_diff'][:6])
    o, c = r['original'], r['candidate']
    for k in ('ret', 'trace', 'rng', 'clean'):
        if o[k] != c[k]: print(f'   {k}: {json.dumps(o[k])[:300]}\n      | {json.dumps(c[k])[:300]}')
    for a, b in zip(o['tasks'], c['tasks']):
        if a != b: print('   task', a['type'], b['type'], [(i, x, y) for i, (x, y) in enumerate(zip(a['words'], b['words'])) if x != y][:6])
    if len(o['tasks']) != len(c['tasks']): print('   ntasks', len(o['tasks']), len(c['tasks']))
