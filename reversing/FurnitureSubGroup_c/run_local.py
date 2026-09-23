import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='cc80f54439a89c7f19d84230f3471c65e15b2ed0a0da79cb17f4e41825fc9a73'
def analyze(path):
    if not terminal_event(path):return None
    pairs=set();counts={0:0,1:0,2:0};complete=[];states=[]
    with path.open() as f:
        for line in f:
            if not line.endswith('\n'):continue
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro de GTA; consultar comparisons.jsonl')
            if r['event']=='complete':complete.append(r)
            if r['event']=='hook_state':states.append(r)
            if r['event']!='comparison':continue
            if r['pair'] in pairs:raise RuntimeError('Par duplicado')
            pairs.add(r['pair']);counts[r['function']]+=1
            if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia en '+str(r['pair']))
            o=r['original']
            if not o['model_unchanged'] or not o.get('store_untouched',o.get('unchanged',False)):raise RuntimeError('Mutacion no prevista')
    if pairs!=set(range(20480)) or counts!={0:4096,1:8192,2:8192} or len(complete)!=1 or not complete[0]['state_restored']:raise RuntimeError('Cobertura/restauracion incompleta')
    if len(states)!=3 or any(not r['enabled'] or r['locked'] for r in states):raise RuntimeError('Hooks no habilitados desde arranque')
    return dict(pairs=20480,pairs_per_function=counts,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('furniture',PROBE_HASH,analyze,'FurnitureSubGroup_c: tres funciones',timeout=300,stop_before_analysis=True))
