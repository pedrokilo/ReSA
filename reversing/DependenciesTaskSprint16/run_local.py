"""Exige 3848 pares OFF/ON, retorno x87 completo y memoria/ABI restaurados."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CPlayerPed::ControlButtonSprint':3848}
CASES=3840
HELPERS={0x53FB70,0x5407F0,0x5407A0,0x60A550,0x420800}
GATES=('route_ok','helper_route_ok','stack_restored','nonvolatile_preserved','seh_restored','fpu_top_balanced','fixture_contract','readonly_ok','guards_preserved','state_restored','floating_state_restored')
SPEEDS=[0xBF800000,0x80000000,0,1,0x3A83126F,0x3F800000,0x409FF7CF,0x40A00000,0x40A00831,0x41200000,0x7149F2CA,0x7F7FFFFF,0x7F800000,0xFF800000,0x7FC00000,0xFFC00001]
ENERGY=[0xC3170000,0xC3160000,0xC3150000,0,0x3F800000,0x41F00000,0x41F00000,0x41F00000,0x7FC00000,0x80000000]
def decode(n):
 """Misma descomposicion que LiveComparison.hpp: 16 velocidades x 240 contextos + 8 casos con PlayerData NULL."""
 null=n>=CASES
 speed=0 if null else n%16
 ctx=n-CASES if null else n//16
 inp=ctx//8%6;cfg=ctx//48%5
 return dict(type=ctx%4,pad_index=ctx//4%2,speed_index=speed,input_class=inp,pad_cfg=cfg,energy_index=(ctx//2+3*speed)%10,time_mode=(ctx//3+speed)%4,pad_mode=cfg if cfg<4 else 0,null_data=null,controls_disabled=cfg==4)
def word(a,o):return int.from_bytes(bytes(a[o:o+4]),'little')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()]
 begin=[r for r in rows if r['event']=='begin']
 if len(begin)!=1:raise RuntimeError('Missing begin')
 b=begin[0]
 if not all(b.get(k)is True for k in ('registered_hook','ready','target_routes_ready','helper_routes_ok','pump_state_intact')):raise RuntimeError('Invalid preflight')
 if b['player_data_bytes']!=0xAC or b['pad_bytes']!=0x134 or len(b['sprint_table_bytes'])!=128 or len(b['main_off_bytes'])!=64 or b['candidate_module']!=b['probe_module']or not b['probe_module']:raise RuntimeError('Incomplete body/data/table metadata')
 if len(set(b['pad_addresses']))!=2 or not all(b['pad_addresses'])or not all(b['player_addresses']):raise RuntimeError('Missing actual objects')
 if {h['entry']for h in b['helper_routes']}!=HELPERS or not all(h['route_ok']and h['module']and len(h['entry_bytes'])==16 and len(h['body_bytes'])==64 for h in b['helper_routes']):raise RuntimeError('Missing real helper proofs')
 cases=set();pairs=set();keys=set();nulls=0;outputs=set();seen=set()
 for r in rows:
  if r.get('pump_state_intact',True)is not True or r['event'].startswith('first_chance_av'):raise RuntimeError('Pump state/AV failure')
  if r['event']!='comparison':continue
  n=r['case']
  if n in cases or r['pair']in pairs or r['function']not in EXPECTED:raise RuntimeError('Repeated/unknown pair')
  cases.add(n);pairs.add(r['pair'])
  if not r.get('equal')or not r.get('accepted')or not isinstance(r['original'],dict)or r['original']!=r['candidate']:raise RuntimeError('Unequal/null/rejected pair')
  want=decode(n)
  for v in (r['original'],r['candidate']):
   if not all(v.get(k)is True for k in GATES):raise RuntimeError('Route/ABI/effects/restoration gate failed')
   if any(v[k]!=x for k,x in want.items()):raise RuntimeError('Wrong profile/domain')
   inp=v['input_data'];actual=v['player_data']
   if len(inp)!=0xAC or len(actual)!=0xAC or len(v['return_extended'])!=10:raise RuntimeError('Incomplete observable state/return')
   sprint_off=v['input_class']//3
   if word(inp,0x18)!=ENERGY[v['energy_index']]or word(inp,0x1C)!=SPEEDS[v['speed_index']]or inp[0x84]!=sprint_off or bool(inp[0x34]&2)!=(v['energy_index']==6):raise RuntimeError('Wrong energy/speed/flags fixture')
   if actual[:0x18]!=inp[:0x18]or actual[0x20:]!=inp[0x20:]:raise RuntimeError('Unexpected full PlayerData mutation')
   if want['null_data']:
    if actual!=inp or v['return_bits']!=0 or v['return_extended']!=[0]*10:raise RuntimeError('NULL PlayerData early return mismatch')
   else:
    if word(actual,0x1C)!=word(inp,0x1C):seen.add('speed_written')
    if word(actual,0x18)!=word(inp,0x18):seen.add('energy_written')
    if word(actual,0x1C)==0 and word(inp,0x1C)!=0 and v['return_bits']==0:seen.add('speed_zeroed')
   outputs.add(v['return_bits'])
  keys.add((want['type'],want['pad_index'],want['input_class'],want['pad_cfg'],want['speed_index'],want['null_data']));nulls+=int(want['null_data'])
 if cases!=set(range(CASES+8))or pairs!=set(range(CASES+8))or len(keys)!=CASES+8 or nulls!=8:raise RuntimeError('Missing exhaustive fixture matrix')
 if 0 not in outputs or 0x3F800000 not in outputs or 0x7F800000 not in outputs or not any(0x3F800000<x<0x7F800000 for x in outputs):raise RuntimeError('Missing zero/one/above-one/inf output branches')
 if seen!={'speed_written','energy_written','speed_zeroed'}:raise RuntimeError('Missing write branches: '+str(seen))
 end=[r for r in rows if r['event']=='complete']
 if len(end)!=1 or end[0]['pairs']!=3848 or not all(end[0].get(k)is True for k in ('state_restored','helper_route_ok','floating_state_restored','pump_state_intact')):raise RuntimeError('Missing final restoration')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('dependenciestasksprint16',manifest['binaries']['asi'],analyze,'DependenciesTaskSprint16',timeout=1200))
