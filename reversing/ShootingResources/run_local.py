import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='bdbcdaa715859dcfa1dacdeaef873137ba48f7926ed577eb777fcf892f2dae4b'
REQUIRE_ENABLED=True
COUNTS={0:1024,1:1024,2:2048,3:4096,4:4096,5:4096,6:2048}
def analyze(path):
    if not terminal_event(path):return None
    cases=set();complete=[];states=[]
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro de GTA; consultar comparisons.jsonl')
            if r['event']=='complete':complete.append(r)
            if r['event']=='hook_state':states.append(r)
            if r['event']!='comparison':continue
            key=(r['function'],r['case'])
            if key in cases:raise RuntimeError('Caso duplicado')
            cases.add(key)
            if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia o contador incorrecto')
            o=r['original'];keys=('source_unchanged','refs_clean') if r['function'] in (0,1,6) else ('guards','query_unchanged')
            if r['function']==0:keys+=('allocated','one_allocation','pool_restored','parent_null','subtask_null')
            if not all(o[k] for k in keys):raise RuntimeError('Estado incorrecto')
    if cases!={(fn,i) for fn,n in COUNTS.items() for i in range(n)} or len(complete)!=1 or not complete[0]['state_restored'] or complete[0]['pairs']!=sum(COUNTS.values()):raise RuntimeError('Cobertura incompleta')
    if len(states)!=7 or (REQUIRE_ENABLED and any(not r['enabled'] or r['locked'] for r in states)):raise RuntimeError('Estado de hooks incorrecto')
    return dict(pairs=sum(COUNTS.values()),pairs_per_function=COUNTS,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('shooting-resources',PROBE_HASH,analyze,'Shooting y recursos de scripts',timeout=300,stop_before_analysis=True))
