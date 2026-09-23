import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest,terminal_event
PROBE_HASH='d3ffb3325017a44047c047df082c453b2137f2e5a864e6404a0889fa21cf8f8e'
REQUIRE_ENABLED=False
COUNTS={i:128 for i in range(13)}
def analyze(path):
    if not terminal_event(path):return None
    cases=set();complete=[];states=[];interop=[]
    with path.open() as f:
        for line in f:
            r=json.loads(line)
            if r['event'] in ('error','assertion'):raise RuntimeError('Fallo dentro de GTA; consultar comparisons.jsonl')
            if r['event']=='complete':complete.append(r)
            if r['event']=='hook_state':states.append(r)
            if r['event']=='interop':interop.append(r)
            if r['event']!='comparison':continue
            key=(r['function'],r['case'])
            if key in cases:raise RuntimeError('Caso duplicado')
            cases.add(key)
            if not r['equal'] or r['original']!=r['candidate'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1:raise RuntimeError('Diferencia o contador incorrecto')
    if cases!={(fn,i) for fn,n in COUNTS.items() for i in range(n)} or len(complete)!=1 or not complete[0]['state_restored'] or complete[0]['pairs']!=sum(COUNTS.values()):raise RuntimeError('Cobertura incompleta')
    if len(states)!=13 or (REQUIRE_ENABLED and any(not r['enabled'] or r['locked'] for r in states)):raise RuntimeError('Estado de hooks incorrecto')
    if len(interop)!=64 or {r['mask'] for r in interop}!=set(range(64)) or not all(r['passed'] for r in interop):raise RuntimeError('Interoperabilidad incompleta')
    return dict(interop_modes=64,pairs=sum(COUNTS.values()),pairs_per_function=COUNTS,all_equal=True,state_restored=True,hook_states=states,log_sha256=digest(path))
if __name__=='__main__':sys.exit(run_case('unlock-file-mgr',PROBE_HASH,analyze,'Desbloqueos: archivos del motor',timeout=300,stop_before_analysis=True,unhook="Audio,Global/SurfaceInfos_c"))
