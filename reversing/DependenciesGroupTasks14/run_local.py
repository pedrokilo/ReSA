"""Strict one-function, 2048-pair group script-task validation; preparation only."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
from DependenciesWorld10.NewGameConfiguration import run_new_game
HERE=Path(__file__).resolve().parent
EXPECTED={'CPedGroupIntelligence::ComputeScriptCommandTasks':2048}
HELPERS={0x5FC150,0x5FC190,0x5F7250,0x5F6AE0,0x5FB160,0x5F80D0,0x5F7350,0x5E95B0,0x571A00,0x571B70,0x61A5A0,0x61A5B0,0x61A500,0x62F310}
GATES=('stack_restored','nonvolatile_preserved','seh_restored','route_ok','helper_route_ok','references_valid','fixture_contract','guards_preserved','resources_balanced','original_task_pool_intact','original_groups_intact','state_restored','floating_state_restored','private_arenas_restored')
def word(a,o):return int.from_bytes(bytes(a[o:o+4]),'little')
def store(a,o,v):a[o:o+4]=v.to_bytes(4,'little')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
 begins=[r for r in rows if r['event']=='begin']
 if len(begins)!=1:raise RuntimeError('Missing begin')
 b=begins[0]
 if not all(b.get(k) is True for k in ('registered_hook','helper_routes_ok','actor_real_player','second_actor_real','original_task_pool_intact')):raise RuntimeError('Invalid preflight')
 if b['ped_pool_slot_stride']!=0x7C4 or not 0<=b['second_actor_slot_index']<b['ped_pool_capacity'] or b['ped_slot_canary_bytes']not in (16,32) or b['npc_intelligence_bytes']<=0:raise RuntimeError('Invalid real NPC slot/range/canaries')
 cap=b['original_task_pool_capacity']
 if not 0<cap<=4096 or b['original_task_storage_bytes']!=cap*0x80 or b['original_task_bitmap_bytes']!=cap:raise RuntimeError('Incomplete old-pool snapshot')
 if b['private_task_pool_capacity']!=16 or b['private_reference_capacity']!=64:raise RuntimeError('Wrong private arenas')
 hs=b['helper_routes']
 if len(hs)!=len(HELPERS) or {h['entry'] for h in hs}!=HELPERS or not all(h['route_ok'] and h['module'] for h in hs):raise RuntimeError('Missing real helper routes')
 cases=set();pairs=set();profiles=set();multi_profiles=set()
 for r in rows:
  if 'pump_state_intact'in r and r['pump_state_intact']is not True:raise RuntimeError('Pump altered actors/pools/routes')
  if r['event'].startswith('first_chance_av'):raise RuntimeError('Access violation')
  if r['event']!='comparison':continue
  case=r['case']
  if case in cases or r['pair'] in pairs or r['function'] not in EXPECTED:raise RuntimeError('Repeated/unknown pair')
  cases.add(case);pairs.add(r['pair'])
  if r.get('equal') is not True or r.get('accepted') is not True or r['original']!=r['candidate']:raise RuntimeError('Rejected pair')
  for side in ('original','candidate'):
   v=r[side]
   if not isinstance(v,dict) or not all(v.get(k) is True for k in GATES):raise RuntimeError('NULL object or failed gate')
   multi=case>=1024;n=case-1024 if multi else case
   member=-2 if multi else n%9-1;old=n%8 if multi else n//9%8;mode=n//8%8 if multi else n//72%4
   if v['multi_actor']!=multi:raise RuntimeError('Wrong actor domain')
   if (v['member_index'],v['old_index'],v['mode'])!=(member,old,mode):raise RuntimeError('Wrong case profile')
   if multi:multi_profiles.add((old,mode))
   else:profiles.add((member,old,mode))
   inp=v['input_group'];actual=v['group']
   if len(inp)!=0x2D4 or len(actual)!=0x2D4 or len(v['task_storage'])!=16*0x80 or len(v['task_states'])!=16:raise RuntimeError('Incomplete full observation')
   expected=inp.copy();retained=0
   for i in range(8):
    ped=word(inp,0xC+4*i);oldped=word(inp,0x30+0x14C+20*i)
    if ped==oldped:task=word(inp,0x30+0x150+20*i)
    else:task=next((word(inp,0x30+0x150+20*j) for j in range(8) if word(inp,0x30+0x14C+20*j)==ped),0)
    store(expected,0x30+0x14C+20*i,ped);store(expected,0x30+0x150+20*i,task);retained+=bool(task)
    # The physical metadata/padding is required equal by full-object oracle.
   if actual!=expected or v['tasks_remaining']!=retained or sum(not(x&0x80) for x in v['task_states'])!=retained:raise RuntimeError('Group/physical metadata/pool contract violated')
   acquired=(1 if mode>=6 else 2) if multi else int(mode>=2)
   if v['tasks_acquired']!=acquired or len(v['task_virtual_routes'])!=acquired or not all(h['route_ok'] and h['module'] for h in v['task_virtual_routes']):raise RuntimeError('Missing actual deleting-destructor route')
   if len(v['input_task_payloads'])!=8 or sum(bool(p) for p in v['input_task_payloads'])!=acquired or any(p and len(p)!=0x20 for p in v['input_task_payloads']):raise RuntimeError('Incomplete task input')
   if len(v['task_payloads'])!=retained:raise RuntimeError('Incomplete retained payload')
   for p in v['task_payloads']:
    if p['bytes']!=v['input_task_payloads'][p['old_slot']] or len(p['bytes'])!=0x20:raise RuntimeError('Retained task changed')
 if cases!=set(range(2048)) or pairs!=set(range(2048)):raise RuntimeError('Incomplete unique pairs')
 if multi_profiles!={(o,m) for o in range(8) for m in range(8)}:raise RuntimeError('Missing real multi-actor swaps/partial/all deletion')
 if profiles!={(m,o,k) for m in range(-1,8) for o in range(8) for k in range(4)}:raise RuntimeError('Missing membership/search/delete profiles')
 end=[r for r in rows if r['event']=='complete']
 if len(end)!=1 or end[0]['pairs']!=2048 or not all(end[0].get(k) is True for k in ('state_restored','original_task_pool_intact','helper_route_ok','floating_state_restored','pump_state_intact')):raise RuntimeError('Incomplete final restoration')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(run_new_game(runtime_runner,'dependenciesgrouptasks14',manifest['binaries']['asi'],analyze,'DependenciesGroupTasks14',timeout=1500))
