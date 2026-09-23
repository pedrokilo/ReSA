import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='be16059038c44381b33726563185f14cf2516046c7a4e1f443073e0508a6c614'
def analyze(path):
    if not path.exists():return None
    rows=[json.loads(s) for s in path.read_text().splitlines(keepends=True) if s.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows):raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed:return None
    pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=4096 or {r['pair'] for r in pairs}!=set(range(4096)):raise RuntimeError('Cobertura incompleta.')
    for r in pairs:
        if not r['equal'] or r['original']!=r['candidate'] or not r['inputs_restored'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia en '+str(r['pair']))
    if len(completed)!=1 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    return dict(pairs=4096,all_equal=True,state_restored=True,log_sha256=digest(path))
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:sys.exit(run_case('pedlist',PROBE_HASH,analyze,'CPedList: cuatro funciones'))

