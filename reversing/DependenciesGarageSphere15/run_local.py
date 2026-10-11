"""Reject missing geometry, illegal representations, incorrect full-pool deltas and ABI/routes."""
import json,sys,struct
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
from DependenciesWorld15.geometry_oracle import X87, at, fraction_bits, world_point, sphere_hit
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
from DependenciesWorld10.NewGameConfiguration import run_new_game
HERE=Path(__file__).resolve().parent
EXPECTED={'CGarage::EntityHasASphereWayOutsideGarage':2048}
HELPERS={0x535300,0x54F1B0,0x54E9D0,0x59C890,0x4487D0,0x59AF40,0x59B020,0x40F030,0x4470E0,0x447110,0x4EF660}
GATES=('route_ok','allocate_route_ok','remove_route_ok','helper_route_ok','representation_legal','stack_restored','nonvolatile_preserved','seh_restored','fpu_top_balanced','fixture_contract','readonly_ok','guards_preserved','state_restored','floating_state_restored')
def f32(x):return struct.unpack('<f',struct.pack('<f',x))[0]
def floats(raw):return struct.unpack('<'+'f'*(len(raw)//4),bytes(raw))
def bits(x):return struct.unpack('<I',struct.pack('<f',x))[0]
def word(a,o):return int.from_bytes(bytes(a[o:o+4]),'little')
def analyze(path):
 result=common_analyzer(EXPECTED)(path)
 if result is None:return None
 rows=[json.loads(l) for l in path.read_text(encoding='utf-8').splitlines()];beg=[r for r in rows if r['event']=='begin']
 if len(beg)!=1:raise RuntimeError('Missing begin')
 b=beg[0]
 if not all(b.get(k)is True for k in ('registered_hook','allocate_registered','remove_registered','ready','helper_routes_ok','native_helpers_ok','remove_route_ok','matrix_pool_legal','target_routes_ready','pump_state_intact')):raise RuntimeError('Failed preflight/native proof')
 if not b.get('probe_module') or b['main_candidate_module']!=b['probe_module'] or b['allocate_candidate_module']!=b['probe_module'] or any(len(b[k])!=64 for k in ('main_off_bytes','allocate_off_bytes','allocate_on_bytes')):raise RuntimeError('Incomplete main/Allocate OFF bytes or ON module proof')
 if len(b['helper_routes'])!=len(HELPERS)or {h['entry']for h in b['helper_routes']}!=HELPERS or not all(h['route_ok']and h['module']and len(h['entry_bytes'])==16 and len(h['body_bytes'])==64 for h in b['helper_routes']):raise RuntimeError('Incomplete genuine helpers')
 if len(b['native_transform_bytes'])!=124 or len(b['native_update_simple_bytes'])!=42 or len(b['remove_off_bytes'])!=133 or b['native_transform_bytes'][-1]!=0xC3 or b['native_update_simple_bytes'][-3:]!=[0xC2,4,0] or b['remove_off_bytes'][-1]!=0xC3:raise RuntimeError('Incomplete actual EXE helper proof')
 pool=b['matrix_pool_baseline_nodes'];base=b['matrix_pool_object_base']
 if len(pool)!=b['matrix_pool_nodes']or len(b['matrix_pool_baseline_header'])!=0x1FC or not all(len(n)==0x54 for n in pool):raise RuntimeError('Missing full matrix-pool inventory')
 if not 1<=b['native_sphere_count']<=128 or len(b['native_spheres'])!=b['native_sphere_count']*20:raise RuntimeError('Missing native geometry')
 cases=set();pairs=set();profiles=set();domains=[0,0];reps=[0,0];outcomes=set()
 for row in rows:
  if 'pump_state_intact'in row and row['pump_state_intact']is not True:raise RuntimeError('External state changed at Pump')
  if row['event'].startswith('first_chance_av'):raise RuntimeError('Access violation')
  if row['event']!='comparison':continue
  c=row['case'];native=c>=1536;n=c-1536 if native else c;geom=n%4;mode=n//8%8;matrix=bool(n//4&1);extra=(0,.25,1,4)[n//64%4]
  if c in cases or row['pair']in pairs or row['function']not in EXPECTED:raise RuntimeError('Duplicate/unknown pair')
  cases.add(c);pairs.add(row['pair']);profiles.add((native,geom,mode,matrix,extra));domains[native]+=1;reps[matrix]+=1
  if row.get('accepted')is not True or row.get('equal')is not True or row['original']!=row['candidate']:raise RuntimeError('Unequal/rejected pair')
  for side in ('original','candidate'):
   v=row[side]
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL object/failed gate')
   if (v['native_geometry'],v['geometry_index'],v['mode'],v['matrix_present'],v['extra_radius_bits'])!=(native,geom,mode,matrix,bits(extra)):raise RuntimeError('Wrong profile')
   count=v['sphere_count'];garage=floats(v['input_garage'][:40]);spheres=v['input_spheres']
   if len(v['input_garage'])!=0xD8 or count!=int.from_bytes(bytes(v['input_data'][:2]),'little')or len(spheres)!=count*20 or len(v['transformed'])!=count:raise RuntimeError('Incomplete input/observation')
   if native and spheres!=b['native_spheres']:raise RuntimeError('Native geometry modified')
   if not native and count!=(0,1,4,8)[geom]:raise RuntimeError('Wrong private valid sphere count')
   if word(v['input_garage'],0x3C)!=c:raise RuntimeError('Missing unique metadata fixture')
   first=count
   matrix_values=floats(v['matrix_payload'][:64])if v['matrix_payload']else None
   for i,t in enumerate(v['transformed']):
    sphere=floats(spheres[i*20:i*20+16]);r=f32(extra+sphere[3]);p=floats(b''.join(int(x).to_bytes(4,'little')for x in t['point_bits']))
    a=matrix_values;x,y,z=sphere[:3]
    fp=X87(v['x87_control'])
    matrix_input=[at(v['matrix_payload'],j*4) if j%4!=3 else fraction_bits(0) for j in range(16)]
    derived=world_point(fp,matrix_input,[at(spheres,i*20+j*4) for j in range(3)])
    if tuple(t['point_bits'])!=tuple(bits(x)for x in derived):raise RuntimeError('Independent matrix/sphere oracle mismatch')
    inside=sphere_hit(fp,[at(v['input_garage'],j*4) for j in range(10)],[fraction_bits(x) for x in t['point_bits']],fraction_bits(bits(r)))
    if t['radius_bits']!=bits(r)or t['inside']!=inside:raise RuntimeError('Independent point/radius oracle mismatch')
    if not inside and first==count:first=i
   if v['first_outside']!=first or v['return_al']!=int(first<count):raise RuntimeError('Wrong any-outside/AL contract')
   outcomes.add(v['return_al'])
   ix=v['matrix_index']
   if ix<0:
    if matrix or count or v['matrix_payload']:raise RuntimeError('Illegal null representation')
   elif not 0<=ix<len(pool)or len(v['matrix_payload'])!=0x54:raise RuntimeError('Illegal live matrix index')
   delta=v['matrix_pool_delta'];indices=[x['index']for x in delta['changed_nodes']]
   if len(delta['header'])!=0x1FC or len(indices)!=len(set(indices))or any(not 0<=i<len(pool)for i in indices)or any(len(x['bytes'])!=0x54 for x in delta['changed_nodes']):raise RuntimeError('Incomplete/invalid full-pool delta')
   if ix>=0:
    matrixbytes=next((x['bytes']for x in delta['changed_nodes']if x['index']==ix),pool[ix])
    if matrixbytes!=v['matrix_payload']or word(v['vehicle_state'],0x14)!=base+ix*0x54:raise RuntimeError('Pool/entity matrix observation mismatch')
 if cases!=set(range(2048))or pairs!=set(range(2048))or domains!=[1536,512]or reps!=[1024,1024]or outcomes!={0,1}:raise RuntimeError('Missing full domains/branches')
 expected={(native,g,m,x,r)for native in (False,True)for g in range(4)for m in range(8)for x in (False,True)for r in (0,.25,1,4)}
 if profiles!=expected:raise RuntimeError('Missing combinations')
 end=[r for r in rows if r['event']=='complete']
 if len(end)!=1 or end[0]['pairs']!=2048 or not all(end[0].get(k)is True for k in ('state_restored','floating_state_restored','helper_route_ok','pump_state_intact')):raise RuntimeError('Final restoration missing')
 return result
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'dependenciesgaragesphere15',m['binaries']['asi'],analyze,'DependenciesGarageSphere15',timeout=1500))
