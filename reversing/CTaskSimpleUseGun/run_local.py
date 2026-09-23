"""Run ProcessPed comparisons with the shared test-copy runner."""
import json
import math
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='a10f00f4cb717fa3e556b9ea42f4f019036bbf997f6f4f6d0ab621029a8ba345'

def compare(a,b):
    if isinstance(a,dict):
        if not isinstance(b,dict) or a.keys()!=b.keys():raise ValueError('Object mismatch')
        error=0.
        for k in a:
            if k=='exact':
                if a[k]!=b[k]:raise ValueError('Discrete mismatch')
            else:error=max(error,compare(a[k],b[k]))
        return error
    if isinstance(a,list):
        if not isinstance(b,list) or len(a)!=len(b):raise ValueError('Array mismatch')
        return max((compare(x,y) for x,y in zip(a,b)),default=0.)
    if isinstance(a,float) or isinstance(b,float):
        if not isinstance(a,(int,float)) or not isinstance(b,(int,float)) or not math.isfinite(a) or not math.isfinite(b):raise ValueError('Invalid numeric result')
        error=abs(a-b)
        if error>1e-5:raise ValueError('Numeric mismatch')
        return error
    if a!=b:raise ValueError('Value mismatch')
    return 0.

def analyze(path):
    if not path.exists():return None
    rows=[json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows):raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed:return None
    pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=3232 or {r['pair'] for r in pairs}!=set(range(3232)):raise RuntimeError('Cobertura incompleta o duplicada.')
    ready=[r for r in rows if r['event']=='player_ready']
    if len(ready)!=1 or not ready[0]['independent_inputs']:raise RuntimeError('Entradas no independientes.')
    maximum=0.;events=Counter()
    for r in pairs:
        if not r['equal'] or not r['independent_inputs'] or not r['inputs_unchanged'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1 or r['controlled']!=(r['pair']>=64):raise RuntimeError('Caso divergente: '+str(r['pair']))
        for side in ('original','candidate'):
            if not r[side]['exact'][-1] or any(type(x) not in (int,float) or not math.isfinite(x) for x in r[side]['numbers']):raise RuntimeError('Punteros o numeros invalidos.')
        maximum=max(maximum,compare(r['original'],r['candidate']))
        events.update(e[0] for e in r['original']['events'])
    if len(completed)!=1 or completed[0]['pairs']!=3232 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    return dict(pairs=3232,real_dependency_pairs=64,controlled_dependency_pairs=3168,all_equal=True,max_error=maximum,tolerance=1e-5,candidate_calls=3232,state_restored=True,events=dict(events),log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None:raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else:sys.exit(run_case('usegun',PROBE_HASH,analyze,'CTaskSimpleUseGun::ProcessPed'))
