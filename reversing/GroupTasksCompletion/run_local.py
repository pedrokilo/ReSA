import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
from runtime_runner import run_case,digest
# 134R: el hash de la sonda sale del manifiesto de la ultima compilacion (antes estaba congelado y ya no coincide con el codigo actual)
PROBE_HASH=json.loads((Path(__file__).parent/'artifacts/build-manifest.json').read_text())['binaries']['asi']
EXPECTED={'Random::AllocateDefaultTasks':2048,'SitInLeaderCar::AllocateDefaultTasks':2048,'GangFollower::SetCanEnterLeadersCar':512}
def analyze(path):
    if not path.exists():return None
    rows=[json.loads(s) for s in path.read_text().splitlines(keepends=True) if s.endswith('\n')]
    if any(r['event'] in ('assertion','error') for r in rows):raise RuntimeError('Error dentro de GTA; consultar comparisons.jsonl.')
    completed=[r for r in rows if r['event']=='complete']
    if not completed:return None
    pairs=[r for r in rows if r['event']=='comparison']
    total=sum(EXPECTED.values())
    if len(pairs)!=total or {r['pair'] for r in pairs}!=set(range(total)):raise RuntimeError('Cobertura incompleta.')
    counts={f:0 for f in EXPECTED};stats={'tasks_compared':0,'door_queries':0,'assigned_pairs':0}
    for r in pairs:
        if not(r['equal'] and r['original']==r['candidate'] and r['inputs_restored'] and r['clean'] and r['hook_switches']
               and r['original_candidate_calls']==0 and r['candidate_calls']==1):raise RuntimeError('Diferencia en '+str(r['pair']))
        counts[r['function']]+=1
        if isinstance(r['original'],dict):
            stats['tasks_compared']+=len(r['original']['tasks']);stats['door_queries']+=len(r['original']['doors'])
            stats['assigned_pairs']+=sum(1 for _,t in r['original']['pairs'] if isinstance(t,str) and t.startswith('T'))
    if counts!=EXPECTED:raise RuntimeError('Reparto de pares inesperado: '+json.dumps(counts))
    if len(completed)!=1 or not completed[0]['state_restored']:raise RuntimeError('Estado no restaurado.')
    return dict(pairs=total,pairs_per_function=counts,all_equal=True,state_restored=True,**stats,log_sha256=digest(path))
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:sys.exit(run_case('group-alloc',PROBE_HASH,analyze,'Asignadores de grupo Random/SitInLeaderCar y SetCanEnterLeadersCar'))
