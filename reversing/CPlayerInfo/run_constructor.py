import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='f608d334bce5dc30ae8a344505d66acfec1b82dbfb8ab4ff43595a0769aae663'
def analyze(path):
    if not terminal_event(path):return None
    pairs=set();counts={'constructor':0,'weapons':0};complete=[];hook_states=[]
    with path.open() as f:
        for line in f:
            if not line.endswith('\n'):continue
            row=json.loads(line)
            if row['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro del motor; revisar comparisons.jsonl.')
            if row['event']=='complete':complete.append(row)
            if row['event']=='hook_state':hook_states.append(row)
            if row['event']!='comparison':continue
            if row['pair'] in pairs:raise RuntimeError('Par duplicado')
            pairs.add(row['pair']);counts[row['kind']]+=1
            if not row['equal'] or row['original']!=row['candidate'] or row['original_candidate_calls']!=0 or row['candidate_calls']!=1:raise RuntimeError('Diferencia en par '+str(row['pair']))
            if row['kind']=='constructor':
                if row['original_initializations']!=1 or row['candidate_initializations']!=1 or not row['original']['guards'] or not row['original']['returned_this']:raise RuntimeError('Constructor incompleto')
            elif not row['original']['usable'] or not row['original']['references_restored']:raise RuntimeError('Regresion de armas')
    if not complete:return None
    if pairs!=set(range(4224)) or counts!={'constructor':4096,'weapons':128} or len(complete)!=1 or not complete[0]['state_restored']:raise RuntimeError('Cobertura/restauracion incompleta')
    if len(hook_states)!=1 or not hook_states[0]['enabled'] or hook_states[0]['locked']:raise RuntimeError('Hook no habilitado desde el arranque')
    return dict(hook_state=hook_states[0],pairs=4224,pairs_by_kind=counts,all_equal=True,state_restored=True,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('playerinfo-constructor',PROBE_HASH,analyze,'CPlayerInfo: constructor y armas',timeout=180,stop_before_analysis=True))
