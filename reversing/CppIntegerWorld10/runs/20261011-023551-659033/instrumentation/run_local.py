"""Strict gates for real camera target/handling routes and exact AL-only constant leaf."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
from NewGameConfiguration import run_new_game
HERE=Path(__file__).resolve().parent
CAMERA='CCam::GetBoatHeightForFirstPerson';CONSTANT='CCarCtrl::Native_00424100'
EXPECTED={CAMERA:2048,CONSTANT:2048}
GATES=('route_ok','helper_route_ok','stack_restored','registers_preserved','contract_ok','readonly_ok','guards_ok','fp_state_restored')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()];beg=[r for r in rows if r.get('event')=='begin']
 if len(beg)!=1 or not all(beg[0].get(k)is True for k in ('registered_hook','helper_registered','helper_route_ok','fp_state_restored'))or beg[0].get('collaborators_stubbed')is not False or beg[0].get('real_vehicle_count',0)<1:raise RuntimeError('Missing real pool/helper routes')
 b=beg[0]
 if b.get('player_area')!=0 or b.get('fixture_configuration')!='FastLoader.SaveGameToLoad=0':raise RuntimeError('Exterior new-game fixture missing')
 if b.get('helper_entry')!=0x6F5300 or not b.get('helper_body')or len(b.get('helper_on_bytes',[]))!=32 or len(b.get('helper_off_bytes',[]))!=32:raise RuntimeError('Missing exact helper body/off bytes')
 cases={fn:set()for fn in EXPECTED};pairs=set();targets=set();seeds=set();target_counts={False:0,True:0};seed_counts={}
 for r in rows:
  if r.get('event')=='error'or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  fn=r['function'];case=r['case'];pair=r['pair']
  if fn not in cases or pair in pairs or case in cases[fn]:raise RuntimeError('Unknown/duplicate pair/case')
  pairs.add(pair);cases[fn].add(case)
  if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference')
  for side in ('original','candidate'):
   v=r.get(side)
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   if fn==CAMERA:
    targets.add(v['target_present'])
    if side=='original':target_counts[v['target_present']]+=1
    if v['return_bits']!=int(v['target_present'])or v['height_bits']!=v['expected_height_bits']:raise RuntimeError('Height/null contract')
   else:
    seeds.add(v['eax_seed'])
    if side=='original':seed_counts[v['eax_seed']]=seed_counts.get(v['eax_seed'],0)+1
    if v['return_bits']!=v['eax_seed']&0xFFFFFF00 or v['al']!=0 or v['x_after']!=v['x_bits']or v['y_after']!=v['y_bits']:raise RuntimeError('AL-only preserve/argument contract')
 if pairs!=set(range(4096))or cases!={fn:set(range(2048))for fn in EXPECTED}or targets!={False,True}or seeds!={0,0xFF,0x13579BDF,0x2468ACE0,0xFFFFFFFF,0x80000000,0x01020304,0x7FFFFFFF}:raise RuntimeError('Incomplete coverage')
 if target_counts!={False:1024,True:1024}or any(n!=256 for n in seed_counts.values()):raise RuntimeError('Uneven required coverage')
 ss=[r for r in rows if r.get('event')=='function_summary']
 if len(ss)!=2 or {r['function']for r in ss}!=set(EXPECTED)or not all(r.get('complete_paths')is True and r.get('cases')==2048 and r.get('failures')==0 for r in ss):raise RuntimeError('Incomplete summaries')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=4096 or not all(end[0].get(k)is True for k in ('state_restored','fp_state_restored','helper_route_ok')):raise RuntimeError('Final restore missing')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'cppintegerworld10',m['binaries']['asi'],analyze,'CppIntegerWorld10',timeout=1500))
