"""Compare task transitions using the shared guarded game runner."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
PROBE_HASH='791d5856572521e1b20c5b7265b018f84268da9deb88155a062d7b0997f11610'

def analyze(path):
    if not path.exists(): return None
    rows=[json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows): raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed: return None
    pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=128 or {r['pair'] for r in pairs}!=set(range(128)): raise RuntimeError('Cobertura incompleta.')
    for r in pairs:
        if not r['equal'] or r['original']!=r['candidate'] or not r['inputs_unchanged'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1: raise RuntimeError('Divergencia: '+str(r['pair']))
    if len(completed)!=1 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
    if completed[0]['created']!=32: raise RuntimeError('No se generaron los 32 pickups reales.')
    return dict(pairs=128,all_equal=True,real_generator_pairs=64,controlled_generator_pairs=64,candidate_calls=128,state_restored=True,log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else: sys.exit(run_case('jetpack',PROBE_HASH,analyze,'CTaskSimpleJetPack::DropJetPack'))



