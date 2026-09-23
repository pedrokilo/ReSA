import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='c96a593ca006bd9e5a871dbc431199125c542637309a4f84f9e00119b387c771'
REQUIRE_ENABLED=True

def analyze(path):
    if not terminal_event(path):return None
    cases=set();complete=[];states=[];max_ulp=0;different=0
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro del motor; consultar comparisons.jsonl')
            if r['event']=='hook_state':states.append(r)
            if r['event']=='complete':complete.append(r)
            if r['event']!='comparison':continue
            if r['case'] in cases:raise RuntimeError('Caso duplicado')
            cases.add(r['case'])
            def order(b):return (~b&0xffffffff) if b&0x80000000 else b|0x80000000
            ulp=abs(order(r['original_bits'])-order(r['candidate_bits']))
            if r['original_extended']!=r['candidate_extended'] or not r['passed'] or ulp!=r['ulp'] or ulp!=0 or r['original_candidate_calls']!=0 or r['candidate_calls']!=1 or r['original_rng_calls']!=1 or r['candidate_rng_calls']!=1:raise RuntimeError('Diferencia o llamadas incorrectas')
            max_ulp=max(max_ulp,ulp);different+=r['original_bits']!=r['candidate_bits']
    if cases!=set(range(65536)) or len(complete)!=1 or not complete[0]['state_restored'] or complete[0]['pairs']!=65536 or complete[0]['max_ulp']!=max_ulp or complete[0]['different']!=different:raise RuntimeError('Cobertura/restauracion incompleta')
    if len(states)!=1 or (REQUIRE_ENABLED and (not states[0]['enabled'] or states[0]['locked'])):raise RuntimeError('Hook no habilitado desde arranque')
    return dict(pairs=65536,max_ulp=max_ulp,different_pairs=different,tolerance_ulp=0,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('general',PROBE_HASH,analyze,'CGeneral: RNG float',timeout=300,stop_before_analysis=True))
