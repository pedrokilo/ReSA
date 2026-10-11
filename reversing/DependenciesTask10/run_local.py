"""Reject incomplete branches, invalid private pools, references and restoration."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={f'CPedGroups::{name}':2048 for name in ('AddGroup','Init','CleanUpForShutDown')}
HELPERS={0x5FC150,0x5F7250,0x5F80D0,0x5F7350,0x5E95B0,0x571A00,0x571B70,0x5DF8F0,0x61A5A0,0x61A5B0,0x61A500,0x62F310}
GATES=('stack_restored','nonvolatile_preserved','seh_restored','route_ok','helper_route_ok',
       'references_valid','fixture_contract','guards_preserved','resources_balanced',
       'state_restored','floating_state_restored','private_arenas_restored','original_task_pool_intact')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1 or not all(begin[0].get(k) is True for k in ('registered_hook','helper_routes_ok','actor_real_player','original_task_pool_intact')):raise RuntimeError('Missing real actor/routes')
    if begin[0]['private_task_pool_capacity']!=16 or begin[0]['private_reference_capacity']!=64:raise RuntimeError('Wrong pool capacities')
    capacity=begin[0]['original_task_pool_capacity']
    if not 0<capacity<=4096 or begin[0]['original_task_storage_bytes']!=capacity*0x80 or begin[0]['original_task_bitmap_bytes']!=capacity:raise RuntimeError('Incomplete original-pool snapshot')
    helpers=begin[0]['helper_routes']
    if len(helpers)!=12 or {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] and h['module'] for h in helpers):raise RuntimeError('Missing actual collaborator proof')
    cases={fn:set() for fn in EXPECTED};profiles={fn:set() for fn in EXPECTED};pairs=set()
    for r in rows:
        if r['event'].startswith('first_chance_av'):raise RuntimeError('Invalid memory access')
        if r['event']!='comparison':continue
        fn=r['function'];case=r['case']
        if case in cases[fn] or r['pair'] in pairs:raise RuntimeError('Duplicate fixture/pair')
        cases[fn].add(case);pairs.add(r['pair'])
        if r.get('equal') is not True or r.get('accepted') is not True or r['original']!=r['candidate']:raise RuntimeError('Rejected pair')
        for side in ('original','candidate'):
            value=r[side]
            if not isinstance(value,dict) or not all(value.get(k) is True for k in GATES):raise RuntimeError('NULL side or failed resource/ABI gate')
            mask=value['active_mask'];mode=value['member_mode'];tasks=value['has_tasks'];profiles[fn].add((mask,mode,tasks))
            selected=next((i for i in range(8) if not(mask&(1<<i))),-1) if fn.endswith('AddGroup') else -1
            if value['selected']!=selected or (fn.endswith('AddGroup') and value['return']!=selected):raise RuntimeError('Wrong first inactive group')
            if len(value['groups'])!=8*0x2D4 or len(value['active'])!=8 or len(value['indices'])!=16 or len(value['task_states'])!=16:raise RuntimeError('Truncated object/pool observation')
            if tasks:
                if value['tasks_acquired']!=8 or len(value['task_virtual_routes'])!=8 or not all(h['route_ok'] and h['module'] for h in value['task_virtual_routes']):raise RuntimeError('Missing genuine task destruction routes')
            elif value['tasks_acquired'] or value['task_virtual_routes']:raise RuntimeError('Unexpected fixture task')
    if pairs!=set(range(6144)) or any(v!=set(range(2048)) for v in cases.values()):raise RuntimeError('Incomplete coverage')
    expected_profiles={(mask,mode,tasks) for mask in range(256) for mode in range(4) for tasks in (False,True)}
    if any(v!=expected_profiles for v in profiles.values()):raise RuntimeError('Missing active/member/task branches')
    end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or end[0]['pairs']!=6144 or not all(end[0].get(k) is True for k in ('state_restored','helper_route_ok','floating_state_restored','original_task_pool_intact')):raise RuntimeError('Final restoration missing')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciestask10',manifest['binaries']['asi'],analyze,'DependenciesTask10',timeout=1500))
