"""Run CPed completion comparisons with the shared test-copy runner."""
import json
import math
import sys
from collections import Counter
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='1c01208a8ae43d5b125309f2b35a0884a28634e2dbcbcffb72b2ab486a7eb633'

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
    if len(pairs)!=28672 or {r['pair'] for r in pairs}!=set(range(28672)):raise RuntimeError('Cobertura incompleta.')
    expected={'CalculateNewVelocity':4096,'DoFootLanded':2048,'PlayFootSteps':8192,'MakeTyresMuddySectorList':2048,'PositionPedOutOfCollision':4096,'KillPedWithCar':8192}
    if Counter(r['function'] for r in pairs)!=Counter(expected):raise RuntimeError('Distribucion de funciones incorrecta.')
    maximum=0.
    for r in pairs:
        if not r['equal'] or not r['bounded'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Caso divergente: '+str(r['pair']))
        maximum=max(maximum,compare(r['original'],r['candidate']))
    if len(completed)!=1 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    return dict(pairs=28672,function_pairs=expected,all_equal=True,max_error=maximum,tolerance=1e-5,state_restored=True,log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None:raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else:sys.exit(run_case('cped-completion',PROBE_HASH,analyze,'CPed: seis funciones completadas'))
