"""Exige 10240 pares OFF/ON (2048 por función), trazas de colaboradores, memoria y ABI idénticos, y cobertura de las ramas de cada ciclo."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
CASES=2048
FUNCS=[('CCamera::ProcessObbeCinemaCameraCar',11,False),('CCamera::ProcessObbeCinemaCameraTrain',6,False),('CCamera::ProcessObbeCinemaCameraHeli',7,True),('CCamera::ProcessObbeCinemaCameraPlane',6,True),('CCamera::ProcessObbeCinemaCameraBoat',3,False)]
EXPECTED={name:CASES for name,_,_ in FUNCS}
GATES=('route_ok','stack_restored','nonvolatile_preserved','seh_restored','fpu_env_preserved','fpu_state_balanced','watched_unchanged','guards_preserved','string_unchanged','state_restored')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()]
 begin=[r for r in rows if r['event']=='begin']
 if len(begin)!=1:raise RuntimeError('Missing begin')
 b=begin[0]
 if not all(b.get(k)is True for k in ('ready','routes_ready','string_ok')):raise RuntimeError('Invalid preflight')
 if b['camera_bytes']!=0xD78 or b['candidate_module']!=b['probe_module'] or not b['probe_module'] or len(b['collaborators'])!=5:raise RuntimeError('Incomplete body/collaborator metadata')
 seen={name:set() for name,_,_ in FUNCS};cases={name:set() for name,_,_ in FUNCS};traces={name:set() for name,_,_ in FUNCS}
 for r in rows:
  if r['event'].startswith('first_chance_av') or r['event']=='assertion':raise RuntimeError('Exception during bank')
  if r['event']!='comparison':continue
  name=r['function'];cases[name].add(r['case'])
  for v in (r['original'],r['candidate']):
   if not all(v.get(k)is True for k in GATES):raise RuntimeError('Route/ABI/effects/restoration gate failed in pair %d'%r['pair'])
   if not isinstance(v['trace'],str) or len(v['trace'])%8:raise RuntimeError('Malformed trace')
  if r['original']['trace']!=r['candidate']['trace'] or r['original']['final_mode']!=r['candidate']['final_mode']:raise RuntimeError('Collaborator trace or final mode differs')
  count=next(c for n,c,_ in FUNCS if n==name);busy=next(u for n,_,u in FUNCS if n==name);p=r['profile'];final=r['original']['final_mode']
  traces[name].add(r['original']['trace'])
  if p['busy'] and not busy:raise RuntimeError('Unexpected 0x50B830 call in '+name)
  if p['help']:seen[name].add('help_message')
  if p['busy']:seen[name].add('busy_check')
  if p['is_it_time']:seen[name].add('time_check')
  if not p['is_it_time']:seen[name].add('no_time_check')
  if p['try_start']==0:seen[name].add('not_time_early_exit')
  if p['try_start']>=count+2:seen[name].add('retries_exhausted')
  if p['try_start']==1:seen[name].add('first_try')
  if 1<p['try_start']<count+2:seen[name].add('found_after_retries')
  if final==count:seen[name].add('fallback_index')
  if 0<=final<count:seen[name].add('table_index')
 need_common={'help_message','time_check','no_time_check','not_time_early_exit','retries_exhausted','first_try','found_after_retries','fallback_index','table_index'}
 for name,_,busy in FUNCS:
  if cases[name]!=set(range(CASES)):raise RuntimeError('Missing cases for '+name)
  need=need_common|({'busy_check'} if busy else set())
  if not need<=seen[name]:raise RuntimeError('Missing branches for %s: %s'%(name,sorted(need-seen[name])))
  if len(traces[name])<CASES//2:raise RuntimeError('Too few distinct traces for '+name)
 end=[r for r in rows if r['event']=='complete']
 if len(end)!=1 or end[0]['pairs']!=CASES*len(FUNCS) or not all(end[0].get(k)is True for k in ('state_restored','floating_state_restored')):raise RuntimeError('Missing final restoration')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('dependencies21b',manifest['binaries']['asi'],analyze,'Dependencies21B',timeout=1800))
