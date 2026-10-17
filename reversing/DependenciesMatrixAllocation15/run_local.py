"""Strict full branch/effect verification for already catalogued collaborators."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'DependenciesWorld10'))
import runtime_runner
from NewGameConfiguration import run_new_game
from probe_case import analyzer as common_analyzer
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
HERE=Path(__file__).resolve().parent
EXPECTED={'CMatrixLinkList::AddToList1':2048,'CPlaceable::AllocateMatrix':2048,'CMatrixLinkList::AddToList2':2048}
GATES=('route_ok','fixture_legal','graph_legal','contract_ok','readonly_entity_tail_ok','stack_restored','registers_preserved','state_restored','fp_state_restored')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 cases={fn:set()for fn in EXPECTED};counts={fn:[0,0,0]for fn in EXPECTED};pairs=set();begins=[];ends=[];ss={}
 with path.open(encoding='utf-8')as stream:
  for line in stream:
   r=json.loads(line);event=r.get('event')
   if event=='error'or(event or '').startswith('first_chance_av'):raise RuntimeError('Runtime error')
   if event=='begin':begins.append(r)
   elif event=='complete':ends.append(r)
   elif event=='function_summary':
    if r['function']in ss:raise RuntimeError('Duplicate summary')
    ss[r['function']]=r
   elif event=='comparison':
    fn=r.get('function');case=r.get('case');pair=r.get('pair')
    if fn not in EXPECTED or case in cases[fn]or pair in pairs:raise RuntimeError('Unknown/duplicate case')
    cases[fn].add(case);pairs.add(pair)
    if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Real allocation/effect difference')
    for side in ('original','candidate'):
     v=r.get(side)
     if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
     if v.get('result_pointer')!=v.get('expected_pointer')or not isinstance(v.get('changed_nodes'),list)or not isinstance(v.get('pool_header'),list)or len(v['pool_header'])!=0x1fc or not isinstance(v.get('owners'),list)or not v['owners']:raise RuntimeError('Missing actual effects')
     kind=v.get('fixture_kind');limit=2 if fn.startswith('CMatrix')else 3
     if kind not in range(limit):raise RuntimeError('Unknown branch')
     if fn.startswith('CMatrix')and((v['result_pointer']==0)!=(kind==1)):raise RuntimeError('Add free/empty contract')
     if fn.startswith('CPlaceable')and not v['result_pointer']:raise RuntimeError('Allocate pointer contract')
     if side=='original':counts[fn][kind]+=1
 if len(begins)!=1:raise RuntimeError('Missing preflight')
 b=begins[0]
 if b.get('free_head_next_address')!=0xB74428 or b.get('free_tail_prev_address')!=0xB74478:raise RuntimeError('Layout mismatch')
 routes=b.get('route_proofs',[])
 if len(routes)!=9 or any(r.get('registered')is not True or not r.get('body')or len(r.get('off_bytes',[]))!=32 or len(r.get('on_bytes',[]))!=32 for r in routes):raise RuntimeError('Missing raw RH body evidence')
 if not all(b.get(k)is True for k in ('registered_hook','all_routes_registered','full_pool_fixture_checked','fp_state_restored'))or b.get('collaborators_stubbed')is not False or b.get('owned_actors_created')!=0 or b.get('initial_free_count',0)<2 or b.get('real_pool_count',0)<2 or b.get('player_area')!=0 or b.get('fixture_configuration')!='FastLoader.SaveGameToLoad=0':raise RuntimeError('Bad real preflight')
 for fn in EXPECTED:
  expected=[1024,1024,0]if fn.startswith('CMatrix')else[683,683,682]
  if cases[fn]!=set(range(2048))or counts[fn]!=expected:raise RuntimeError('Incomplete branches')
  s=ss.get(fn,{})
  if s.get('cases')!=2048 or s.get('failures')!=0 or s.get('complete_paths')is not True or s.get('fixture_counts')!=expected:raise RuntimeError('Bad summary')
 if pairs!=set(range(6144))or len(ends)!=1 or ends[0].get('pairs')!=6144 or not all(ends[0].get(k)is True for k in ('state_restored','resources_restored','fp_state_restored')):raise RuntimeError('Bad final restore')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'dependenciesmatrixallocation15',m['binaries']['asi'],analyze,'DependenciesMatrixAllocation15',timeout=1800))
