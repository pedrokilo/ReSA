"""Compare task transitions using the shared guarded game runner."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME = Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import run_case,digest
PROBE_HASH='131de8b2a23e6e0fe0e91a7df3647e96691c0aefc6fef46c4b07f97534194660'

def analyze(path):
    if not path.exists(): return None
    rows=[json.loads(line) for line in path.read_text().splitlines(keepends=True) if line.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows): raise RuntimeError('Error dentro de GTA; revisar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed: return None
    pairs=[r for r in rows if r['event']=='comparison']
    if len(pairs)!=128 or {r['pair'] for r in pairs}!=set(range(128)): raise RuntimeError('Cobertura incompleta.')
    for r in pairs:
        if not r['equal'] or r['original']!=r['candidate'] or not r['inputs_unchanged'] or r['original_candidate_calls']!=0 or r['candidate_calls']!=1 or not r.get('routes_ok') or not r.get('gen_route_ok') or r.get('orig_generate_calls') not in ([1,0],[0,0]) or r.get('cand_generate_calls')!=[0,r['orig_generate_calls'][0]] or r['orig_real_exe']!=(r['orig_generate_calls'][0] if r['real_generation'] else 0) or r['cand_real_body']!=r['orig_real_exe']: raise RuntimeError('Divergencia: '+str(r['pair']))
    if len(completed)!=1 or not completed[0]['state_restored']: raise RuntimeError('Estado no restaurado.')
    if completed[0]['created']!=32: raise RuntimeError('No se generaron los 32 pickups reales.')
    realExe=sum(r['orig_real_exe'] for r in pairs)
    if completed[0].get('created_candidate')!=32 or realExe!=32 or sum(r['cand_real_body'] for r in pairs)!=32: raise RuntimeError('Generacion real: el EXE original o el cuerpo C++ candidato no ejecutaron las 32 llamadas reales que generan pickup.')
    if completed[0].get('real_exe_calls')!=realExe: raise RuntimeError('Contadores de generacion real incoherentes.')
    return dict(pairs=128,all_equal=True,real_generator_pairs=64,controlled_generator_pairs=64,candidate_calls=128,generate_routes_verified=True,state_restored=True,log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        result=analyze(Path(sys.argv[2]))
        if result is None: raise RuntimeError('Registro incompleto.')
        print(json.dumps(result,indent=2))
    else: sys.exit(run_case('jetpack',PROBE_HASH,analyze,'CTaskSimpleJetPack::DropJetPack'))



