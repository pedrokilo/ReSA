import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='404e08a94aeb175d5e1ceecc0e3dd6ee87b9060ec006b15ed811de37cc33250f'
REQUIRE_ENABLED=True

def analyze(path):
    if not terminal_event(path):return None
    pairs=set();counts={0:0,1:0,2:0,3:0};complete=[];states=[]
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro de GTA; consultar comparisons.jsonl')
            if r['event']=='complete':complete.append(r)
            if r['event']=='hook_state':states.append(r)
            if r['event']!='comparison':continue
            if r['pair'] in pairs:raise RuntimeError('Par duplicado')
            pairs.add(r['pair']);counts[r['function']]+=1
            if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1 or not r['original']['source_unchanged']:raise RuntimeError('Diferencia en '+str(r['pair']))
            o=r['original']
            if r['function']==2 and (not all(o[k] for k in ('allocated','one_allocation','pool_restored')) or o['object_without_vtable']!=[0]*8+[7,0,0,0] or o['type']!=262):raise RuntimeError('Clone incorrecto')
            if r['function']==3 and o['result']!=262:raise RuntimeError('Tipo incorrecto')
    if pairs!=set(range(8448)) or counts!={0:4096,1:256,2:2048,3:2048} or len(complete)!=1 or complete[0]['pairs']!=8448 or not complete[0]['state_restored']:raise RuntimeError('Cobertura/restauracion incompleta')
    if len(states)!=4 or (REQUIRE_ENABLED and any(not r['enabled'] or r['locked'] for r in states)):raise RuntimeError('Estado de hooks inesperado')
    return dict(pairs=8448,pairs_per_function=counts,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('clothes-scripted-sprint',PROBE_HASH,analyze,'CClothes, CScripted2dEffects, Sprint',timeout=300,stop_before_analysis=True))
