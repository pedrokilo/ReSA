"""Strict linked real road-node, real matrix transition and float80 contracts."""
import json,sys,collections
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'DependenciesWorld10'))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from NewGameConfiguration import run_new_game
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent;FN='CCarCtrl::FindGhostRoadHeight'
GATES=('route_ok','helper_route_ok','remove_route_ok','representation_legal','stack_restored','registers_preserved','fpu_top_balanced','contract_ok','readonly_ok','state_restored','fp_state_restored')
def analyze(path):
 summary=common_analyzer({FN:2048})(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()];beg=[r for r in rows if r.get('event')=='begin']
 if len(beg)!=1:raise RuntimeError('Missing begin')
 b=beg[0]
 if not all(b.get(k)is True for k in ('registered_hook','helper_registered','helper_route_ok','remove_registered','remove_route_ok','fp_state_restored'))or b.get('collaborators_stubbed')is not False or b.get('real_edges',0)<1 or b.get('actual_unloaded_area',-1)not in range(64)or b.get('player_area')!=0:raise RuntimeError('Missing real exterior resources/routes')
 if b.get('helper_entry')!=0x420A10 or not b.get('helper_body')or any(len(b.get(k,[]))!=32 for k in ('helper_off_bytes','helper_on_bytes'))or b.get('remove_entry')!=0x54F3B0 or len(b.get('remove_bytes',[]))!=133:raise RuntimeError('Missing native/source proofs')
 cases=set();pairs=set();modes=collections.Counter();representations=collections.Counter()
 for r in rows:
  if r.get('event')=='error'or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  if r.get('function')!=FN or r['case']in cases or r['pair']in pairs:raise RuntimeError('Unknown/duplicate case')
  cases.add(r['case']);pairs.add(r['pair'])
  if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference')
  for side in ('original','candidate'):
   v=r.get(side)
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   mode=v['fixture_mode'];matrix=v['matrix_present']
   if mode not in range(8)or not isinstance(matrix,bool)or len(v['return_extended'])!=10:raise RuntimeError('Wrong float80/fixture')
   if mode<5 and v['return_bits']!=0:raise RuntimeError('Missing-node return contract')
   if side=='original':modes[mode]+=1;representations[matrix]+=1
 if cases!=set(range(2048))or pairs!=cases or modes!={i:256 for i in range(8)}or representations!={False:1024,True:1024}:raise RuntimeError('Incomplete coverage')
 ss=[r for r in rows if r.get('event')=='function_summary']
 if len(ss)!=1 or ss[0].get('function')!=FN or ss[0].get('cases')!=2048 or ss[0].get('failures')!=0 or ss[0].get('complete_paths')is not True or ss[0].get('mode_cases')!=[256]*8 or ss[0].get('representation_cases')!=[1024]*2:raise RuntimeError('Incomplete summary')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=2048 or not all(end[0].get(k)is True for k in ('state_restored','fp_state_restored','helper_route_ok','remove_route_ok')):raise RuntimeError('Final restore failed')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'dependenciesworld14',m['binaries']['asi'],analyze,'DependenciesWorld14',timeout=1500))
