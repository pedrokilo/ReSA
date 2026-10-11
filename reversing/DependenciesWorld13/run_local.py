"""Strict real interior-build table and raw void-register contracts."""
import json,sys,collections
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent;FN='CPathFind::RemoveInteriorLinks'
GATES=('route_ok','stack_restored','registers_preserved','contract_ok','readonly_object_ok','guards_ok','state_restored','fp_state_restored')
def analyze(path):
 summary=common_analyzer({FN:2048})(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()];beg=[r for r in rows if r.get('event')=='begin']
 if len(beg)!=1:raise RuntimeError('Missing begin')
 b=beg[0]
 if not all(b.get(k)is True for k in ('registered_hook','fp_state_restored'))or b.get('collaborators_stubbed')is not False or b.get('real_global_table')!=0x96EAC0 or b.get('real_count_global')!=0x96EF80 or b.get('table_capacity')!=64 or b.get('slots_per_row')!=6:raise RuntimeError('Wrong real layout/routes')
 cases=set();pairs=set();counts=collections.Counter();modes=collections.Counter();seeds=collections.Counter()
 for r in rows:
  if r.get('event')=='error'or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  if r.get('function')!=FN or r['case']in cases or r['pair']in pairs:raise RuntimeError('Unknown/duplicate case')
  cases.add(r['case']);pairs.add(r['pair'])
  if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference')
  for side in ('original','candidate'):
   v=r.get(side)
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   n=v['count'];node=v['target_node'];mode=v['graph_mode']
   if n not in range(65)or node not in range(max(1,n))or mode not in range(8)or len(v['table'])!=384:raise RuntimeError('Illegal table domain')
   if v['return_bits']!=(6 if n else v['eax_seed'])or v['edx_after']!=n or v['ecx_after']!=(0x96EAC0+6*n if n else 0x96F050):raise RuntimeError('Raw volatile-register contract')
   if side=='original':counts[n]+=1;modes[mode]+=1;seeds[v['eax_seed']]+=1
 if cases!=set(range(2048))or pairs!=cases or counts!={n:32 if n<33 else 31 for n in range(65)}or modes!={n:260 if n<7 else 228 for n in range(8)}or len(seeds)!=8 or any(n!=256 for n in seeds.values()):raise RuntimeError('Incomplete fixtures')
 ss=[r for r in rows if r.get('event')=='function_summary']
 if len(ss)!=1 or ss[0].get('function')!=FN or ss[0].get('cases')!=2048 or ss[0].get('failures')!=0 or ss[0].get('complete_paths')is not True or ss[0].get('count_cases')!=[counts[i]for i in range(65)]or ss[0].get('mode_cases')!=[modes[i]for i in range(8)]:raise RuntimeError('Incomplete summary')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=2048 or not all(end[0].get(k)is True for k in ('state_restored','fp_state_restored')):raise RuntimeError('Final restore failed')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('dependenciesworld13',m['binaries']['asi'],analyze,'DependenciesWorld13',timeout=1500))
