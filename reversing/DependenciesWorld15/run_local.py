"""Strict real spheres, existing/null matrices and complete Garage loops."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
sys.path.insert(0,str(Path(__file__).resolve().parents[1]/'DependenciesWorld10'))
import runtime_runner
from NewGameConfiguration import run_new_game
from geometry_oracle import verify as verify_geometry
from probe_case import analyzer as common_analyzer
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
HERE=Path(__file__).resolve().parent
FN='CGarage::IsEntityTouching3D'
GATES=('route_ok','fixture_legal','graph_legal','descriptor_guards_ok','contract_ok','readonly_ok','stack_restored','registers_preserved','state_restored','fp_state_restored')
def analyze(path):
 summary=common_analyzer({FN:2048})(path)
 if summary is None:return None
 cases=set();pairs=set();kinds=[0]*10;reps=[0]*2;angles=[0]*4;begins=[];ends=[];ss=[];late=False;miss=False;lazy=False;zero=False;zero_null=False
 with path.open(encoding='utf-8')as stream:
  for line in stream:
   r=json.loads(line);event=r.get('event')
   if event=='error'or(event or '').startswith('first_chance_av'):raise RuntimeError('Runtime error')
   if event=='begin':begins.append(r)
   elif event=='complete':ends.append(r)
   elif event=='function_summary':ss.append(r)
   elif event=='comparison':
    if r.get('function')!=FN or r.get('case')in cases or r.get('pair')in pairs:raise RuntimeError('Unknown/duplicate case')
    cases.add(r['case']);pairs.add(r['pair'])
    if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Source/effect difference')
    for side in ('original','candidate'):
     v=r.get(side)
     if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
     kind=v.get('fixture_kind');angle=v.get('garage_angle');rep=v.get('matrix_present');hit=v.get('first_sphere_hit')
     if kind not in range(10)or angle not in range(4)or not isinstance(rep,bool)or v.get('sphere_count',0)<2 or v.get('actual_matrix')!=v.get('expected_matrix'):raise RuntimeError('Missing real fixture/effect')
     if v.get('return_bool')is not(kind in (6,7))or(kind==6 and hit!=0)or(kind==7 and(not isinstance(hit,int)or hit<=0))or(kind==8 and hit!=-1):raise RuntimeError('Sphere loop contract')
     if not rep and(kind<6 or kind==9)and v.get('actual_matrix')!=0 or(kind in (6,7,8)or rep)and not v.get('actual_matrix'):raise RuntimeError('Matrix lazy materialization contract')
     if len(v.get('pool_header',[]))!=0x1fc or not isinstance(v.get('changed_nodes'),list)or not isinstance(v.get('owners'),list):raise RuntimeError('Missing observed state')
     verify_geometry(v)
     if side=='original':
      kinds[kind]+=1;reps[int(rep)]+=1;angles[angle]+=1;late|=kind==7 and hit>0;miss|=kind==8;lazy|=not rep and kind in (6,7,8);zero|=kind==9;zero_null|=kind==9 and not rep
 if len(begins)!=1:raise RuntimeError('Missing begin')
 b=begins[0]
 if not all(b.get(k)is True for k in ('registered_hook','all_routes_registered','fp_state_restored'))or b.get('collaborators_stubbed')is not False or b.get('owned_actors_created')!=0 or b.get('real_spheres',0)<2 or b.get('real_pool_count',0)<2 or b.get('player_area')!=0 or b.get('fixture_configuration')!='FastLoader.SaveGameToLoad=0':raise RuntimeError('Bad real preflight')
 routes=b.get('route_proofs',[])
 if len(routes)!=11 or any(r.get('registered')is not True or not r.get('body')or len(r.get('off_bytes',[]))!=32 or len(r.get('on_bytes',[]))!=32 for r in routes):raise RuntimeError('Missing original/source RH evidence')
 if len(b.get('native_point_bytes',[]))!=124 or len(b.get('native_update_bytes',[]))!=42 or b.get('native_update_bytes',[])[-3:]!=[194,4,0] or len(b.get('native_remove_bytes',[]))!=133 or len(b.get('simple_transform_bytes',[]))!=32 or not b.get('simple_transform_body'):raise RuntimeError('Missing actual native/compiled bodies')
 if b.get('native_matrix_entries_shared')is not True or len(b.get('direct_matrix_bodies',[]))!=4 or len(b.get('direct_matrix_bytes',[]))!=4 or any(len(x)!=32 for x in b['direct_matrix_bytes']):raise RuntimeError('Missing actual direct source matrix bodies')
 for name,count in [('native_settranslate_bytes',57),('native_settranslateonly_bytes',24),('native_resetorientation_bytes',34),('native_setrotatez_bytes',51)]:
  if len(b.get(name,[]))!=count:raise RuntimeError('Incomplete shared native matrix bodies')
 if cases!=set(range(2048))or pairs!=cases or kinds!=[205]*8+[204]*2 or reps!=[1028,1020]or not(late and miss and lazy and zero and zero_null)or sum(angles[1:])==0:raise RuntimeError('Incomplete real branch coverage')
 if len(ss)!=1 or ss[0].get('function')!=FN or ss[0].get('cases')!=2048 or ss[0].get('failures')!=0 or ss[0].get('complete_paths')is not True or ss[0].get('fixture_counts')!=kinds or ss[0].get('representation_counts')!=reps or ss[0].get('angle_counts')!=angles or not all(ss[0].get(k)is True for k in ('late_sphere_seen','all_spheres_miss_seen','lazy_matrix_seen','zero_spheres_seen','zero_spheres_null_no_lazy')):raise RuntimeError('Bad coverage summary')
 if len(ends)!=1 or ends[0].get('pairs')!=2048 or not all(ends[0].get(k)is True for k in ('state_restored','resources_restored','fp_state_restored')):raise RuntimeError('Bad final restore')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'dependenciesworld15',m['binaries']['asi'],analyze,'DependenciesWorld15',timeout=1800))
