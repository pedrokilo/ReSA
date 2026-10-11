"""Rechaza pares incompletos, recursos sin balance y rutas no verificadas."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CTaskSimpleFight::Destructor':2048}
GATES=('stack_restored','nonvolatile_preserved','seh_restored','route_ok','helper_route_ok','fixture_contract','guards_preserved','resources_balanced','original_memory_intact','state_restored','floating_state_restored')
HELPERS={0x61C470,0x61A390,0x61A3A0,0x571A00,0x571B70,0x61A5A0,0x61A5B0,0x4CF080,0x4CEEC0,0x4CE9F0,0x4CECF0,0x4CEBC0,0x4CEBE0,0x4D3FB0,0x4D3FD0,0x4D3A60,0x82119A,0x8214BD,0x72F4C0,0x72F4F0}
def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1:raise RuntimeError('Missing begin')
    b=begin[0]
    if not all(b.get(k) is True for k in ('registered_hook','helper_routes_ok','real_animation','default_idle_identity')):raise RuntimeError('Missing genuine animation/routes')
    if not 0<b['animation_nodes']<=256 or not 0<=b['animation_block']<180:raise RuntimeError('Invalid actual animation')
    if b['private_task_capacity']!=4 or b['private_reference_capacity']!=16 or b['original_task_storage_bytes']!=b['original_task_pool_capacity']*128 or b['original_task_bitmap_bytes']!=b['original_task_pool_capacity']:raise RuntimeError('Incomplete original/private pool')
    if {h['entry'] for h in b['helper_routes']}!=HELPERS or not all(h['route_ok'] and h['module'] and len(h['entry_bytes'])==16 and len(h['body_bytes'])==64 for h in b['helper_routes']):raise RuntimeError('Missing real collaborators')
    pairs=set();cases=set();profiles=set()
    for r in rows:
        if r['event']!='comparison':continue
        if r['pair'] in pairs or r['case'] in cases:raise RuntimeError('Duplicate pair')
        pairs.add(r['pair']);cases.add(r['case']);case=r['case']
        if not r['equal'] or not r['accepted'] or not isinstance(r['original'],dict) or not isinstance(r['candidate'],dict) or r['original']!=r['candidate']:raise RuntimeError('Unequal/null pair')
        for v in (r['original'],r['candidate']):
            if not all(v.get(k) is True for k in GATES):raise RuntimeError('False ABI/routes/lifetime/restoration gate')
            target=bool(case&1);mask=(case//2)%4;mode=(case//8)%4;seed=case//32;profiles.add((target,mask,mode,seed))
            if (v['target'],v['association_mask'],v['reference_mode'],v['seed'])!=(target,mask,mode,seed) or v['animation_reference_consumed']!=(mode>=2):raise RuntimeError('Wrong branch')
            if len(v['task_bytes'])!=40 or v['task_bytes'][:4]!=list((0x86D48C).to_bytes(4,'little')) or v['task_bytes'][10]!=(0 if mode==0 or mode>=2 else 1):raise RuntimeError('Missing task memory/destructor effects')
            assocs=v['associations']
            if len(assocs)!=mask.bit_count() or {a['slot'] for a in assocs}!={i for i in range(2) if mask&(1<<i)}:raise RuntimeError('Missing actual association branch')
            for a in assocs:
                if len(a['bytes'])!=60 or len(a['nodes'])!=b['animation_nodes']*24 or a['callback_type']!=2 or a['callback']!=0x4D1390 or a['callback_data_null'] is not True:raise RuntimeError('Missing genuine node/callback effects')
                if a['bytes'][48:60]!=list((2).to_bytes(4,'little')+(0x4D1390).to_bytes(4,'little')+bytes(4)):raise RuntimeError('Incorrect callback bytes')
    if pairs!=set(range(2048)) or cases!=set(range(2048)) or len(profiles)!=2048:raise RuntimeError('Incomplete profiles')
    end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or end[0]['pairs']!=2048 or not all(end[0].get(k) is True for k in ('state_restored','helper_route_ok','floating_state_restored')):raise RuntimeError('Final restoration missing')
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciestask12',manifest['binaries']['asi'],analyze,'DependenciesTask12',timeout=1500))
