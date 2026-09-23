import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='6f71a365e4fe2eb34e47ab5914b6fde2d387ca326ee7f484ca5c560553883b4f'
def equivalent(a,b):
    if a==b:return True
    if isinstance(a,(int,float)) and isinstance(b,(int,float)):
        return abs(a-b)<=0.00001 if isinstance(a,float) or isinstance(b,float) else a==b
    if type(a)!=type(b):return False
    if isinstance(a,dict):return a.keys()==b.keys() and all(equivalent(a[k],b[k]) for k in a)
    if isinstance(a,list):return len(a)==len(b) and all(equivalent(x,y) for x,y in zip(a,b))
    return a==b
def analyze(path):
    if not path.exists():return None
    with path.open('rb') as f:
        f.seek(max(0,path.stat().st_size-4096));tail=f.read()
    if not any(token in tail for token in (b'"event":"complete"',b'"event":"error"',b'"event":"assertion"')):return None
    pairs=set();counts={0:0,1:0,2:0};completed=[]
    with path.open() as f:
        for line in f:
            if not line.endswith('\n'):continue
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
            if r['event']=='complete':completed.append(r)
            if r['event']!='comparison':continue
            if r['pair'] in pairs:raise RuntimeError('Par duplicado.')
            pairs.add(r['pair']);counts[r['function']]+=1
            if not r['equal'] or not equivalent(r['original'],r['candidate']) or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia en '+str(r['pair']))
    if not completed:return None
    if pairs!=set(range(16896)) or counts!={0:4096,1:4096,2:8704}:raise RuntimeError('Cobertura incompleta.')
    if len(completed)!=1 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    return dict(pairs=16896,pairs_per_function=counts,all_equal=True,state_restored=True,log_sha256=digest(path))
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:sys.exit(run_case('playerinfo',PROBE_HASH,analyze,'CPlayerInfo: tres funciones',timeout=600,stop_before_analysis=True))


