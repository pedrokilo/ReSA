"""Strict real RW dictionary, helper-route and lifetime contracts."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
FN='RwHelper::RwTexDictionaryFindHashNamedTexture'
GATES=('route_ok','helper_route_ok','library_route_ok','stack_restored','registers_preserved','contract_ok','readonly_ok','fp_state_restored')
def analyze(path):
 summary=common_analyzer({FN:2048})(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()]
 begin=[r for r in rows if r.get('event')=='begin']
 if len(begin)!=1:raise RuntimeError('Missing begin')
 b=begin[0]
 if not all(b.get(k)is True for k in ('registered_hook','helper_registered','helper_route_ok','library_route_ok','owned_empty_created','fp_state_restored'))or b.get('collaborators_stubbed')is not False or b.get('real_dictionaries',0)<1:raise RuntimeError('Missing real resource/routes')
 if b.get('helper_entry')!=0x53CF30 or not b.get('helper_body')or any(len(b.get(k,[]))!=32 for k in ('helper_off_bytes','helper_on_bytes')):raise RuntimeError('Missing helper bytes')
 cases=set();pairs=set();kinds=[0]*4
 for r in rows:
  if r.get('event')=='error'or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  if r.get('function')!=FN or r['case']in cases or r['pair']in pairs:raise RuntimeError('Unknown/duplicate case')
  cases.add(r['case']);pairs.add(r['pair'])
  if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference')
  for side in ('original','candidate'):
   v=r.get(side)
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   kind=v.get('fixture_kind')
   if kind not in range(4)or v.get('return_bits')!=v.get('expected_pointer'):raise RuntimeError('Return contract')
   if (v['return_bits']==0)!=(kind<2):raise RuntimeError('Found/missing return')
   if v.get('texture_count',0)<(0 if kind==0 else 2)or kind==0 and v['texture_count']!=0:raise RuntimeError('Traversal coverage')
   if side=='original':kinds[kind]+=1
 if cases!=set(range(2048))or pairs!=cases or kinds!=[512]*4:raise RuntimeError('Incomplete coverage')
 ss=[r for r in rows if r.get('event')=='function_summary']
 if len(ss)!=1 or ss[0].get('function')!=FN or ss[0].get('cases')!=2048 or ss[0].get('failures')!=0 or ss[0].get('complete_paths')is not True or ss[0].get('fixture_counts')!=kinds:raise RuntimeError('Incomplete summary')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=2048 or not all(end[0].get(k)is True for k in ('state_restored','resources_restored','owned_empty_destroyed','library_route_ok','fp_state_restored','helper_route_ok')):raise RuntimeError('Final restore failed')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('dependenciesworld11',m['binaries']['asi'],analyze,'DependenciesWorld11',timeout=1500))
