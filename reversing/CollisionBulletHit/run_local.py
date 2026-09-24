"""Run the reviewed local probe through the shared backup/restore runner."""
import json
import sys
from pathlib import Path
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(HERE.parent))
from runtime_runner import run_case,terminal_event,digest

def analyze(path):
    if not terminal_event(path): return None
    rows=[json.loads(s) for s in path.read_text().splitlines()]
    if any(r['event'] in ('error','assertion') or r.get('equal') is False for r in rows):
        raise RuntimeError('Fallo de regresión: consultar comparisons.jsonl')
    pairs=[r for r in rows if r['event']=='comparison']
    bounds=[r for r in rows if r['event']=='bounds']
    reports=[r for r in rows if r['event']=='report']
    complete=[r for r in rows if r['event']=='complete']
    if len(pairs)!=8234 or {r['case'] for r in pairs}!=set(range(8234)):
        raise RuntimeError('Comparaciones normales incompletas')
    if len(bounds)!=256 or {r['surface'] for r in bounds}!=set(range(256)) or len(reports)!=4:
        raise RuntimeError('Casos límite incompletos')
    if len(complete)!=1 or not complete[0]['state_restored']:
        raise RuntimeError('Restauración no acreditada')
    return dict(pairs=8234,bounds=256,reports=4,state_restored=True,log_sha256=digest(path))

if __name__=='__main__':
    manifest=json.loads((HERE/'artifacts/manifest.json').read_text())
    sys.exit(run_case('bullet-hit',manifest['asi'],analyze,'Impactos de bala: BMX y superficies',
                     timeout=180,stop_before_analysis=True,unhook=''))
