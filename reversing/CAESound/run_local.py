import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='ddc749b5c11ca89ed6e0246c1eaa41479f7d9c1cf7ca095c5ae531e1df540510'
def analyze(path):
    if not terminal_event(path):return None
    cases=set();complete=[];states=[]
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Diferencia o fallo dentro del motor; consultar comparisons.jsonl')
            if r['event']=='hook_state':states.append(r)
            if r['event']=='complete':complete.append(r)
            if r['event']!='comparison':continue
            if r['case'] in cases:raise RuntimeError('Caso duplicado')
            cases.add(r['case'])
            if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia o contador incorrecto')
            if not all(r['candidate'][key] for key in ('returned_this','source_unchanged','source_cleared','dest_cleared','stops_after_entity_removed')):raise RuntimeError('Referencia no resuelta')
    if cases!=set(range(1024)) or len(complete)!=1 or not complete[0]['state_restored']:raise RuntimeError('Cobertura/restauracion incompleta')
    if len(states)!=1 or not states[0]['enabled'] or states[0]['locked']:raise RuntimeError('Hook desactivado')
    return dict(pairs=1024,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('sound-copy',PROBE_HASH,analyze,'CAESound: copia y referencias',timeout=300,stop_before_analysis=True,unhook='Global/SurfaceInfos_c'))
