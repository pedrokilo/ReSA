"""Exige grid completo, oracle del mapa real y restauración por lado/final."""
import json
import struct
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CTheZones::SetCurrentZoneAsUnlocked':2048}
HELPERS={0x56E010,0x572300,0x572270,0x572130,0x821B40}
GATES=('stack_restored','nonvolatile_preserved','seh_restored','route_ok','helper_route_ok','fixture_contract','actor_intact','map_intact','guards_preserved','state_restored','floating_state_restored')
COORDS=[-2700,-2100,-1500,-900,-300,300,900,1500,2100,2700,-3000,-2400,-1800,-1200,0,2999]
CELLS=[0,1,2,3,4,5,6,7,8,9,0,1,2,3,5,9]
def oracle_level(raw,count,x,y,z):
    def record(i):
        at=i*32
        bounds=struct.unpack_from('<6h',raw,at+16)
        return bounds,raw[at+31]
    for i in range(1,count):
        (x1,y1,z1,x2,y2,z2),level=record(i)
        if x1<=x<=x2 and y1<=y<=y2 and z1<=z<=z2:return level
    return record(0)[1]
def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1:raise RuntimeError('Missing begin')
    b=begin[0]
    if not all(b.get(k) is True for k in ('registered_hook','actor_real_selected_player','on_foot','helper_routes_ok','native_complete_bodies_ok')):raise RuntimeError('Missing real actor/routes/native body proof')
    if b['coordinates']!=COORDS or b['cells']!=CELLS or len(b['map_zone_bytes'])!=39*32 or not 0<b['map_zone_count']<=39 or b['player_snapshot_bytes']!=0x7A4 or b['matrix_snapshot_bytes']!=(0x54 if b['player_matrix_present'] else 0):raise RuntimeError('Wrong fixture/map/actor domain')
    if len(b['native_visited_bytes'])!=69 or len(b['native_ftol_bytes'])!=117:raise RuntimeError('Incomplete native collaborators')
    helpers=b['helper_routes']
    if {h['entry'] for h in helpers}!=HELPERS or not all(h['route_ok'] and h['module'] and len(h['entry_bytes'])==16 and len(h['body_bytes'])==64 for h in helpers):raise RuntimeError('Missing shared routes')
    for entry,key in ((0x572130,'native_visited_bytes'),(0x821B40,'native_ftol_bytes')):
        h=next(h for h in helpers if h['entry']==entry)
        if h['body']!=entry or h['module']!=0x400000 or h['body_bytes']!=b[key][:64]:raise RuntimeError('Native code changed/routed unexpectedly')
    pairs=set();cases=set();profiles=set();cells_seen=set();branches=set();raw=bytes(b['map_zone_bytes'])
    for r in rows:
        if r['event']!='comparison':continue
        if r['pair'] in pairs or r['case'] in cases:raise RuntimeError('Repeated pair')
        pairs.add(r['pair']);cases.add(r['case']);case=r['case']
        if not r['equal'] or not r['accepted'] or not isinstance(r['original'],dict) or not isinstance(r['candidate'],dict) or r['original']!=r['candidate']:raise RuntimeError('Unequal/null/unaccepted comparison')
        for v in (r['original'],r['candidate']):
            if not all(v.get(k) is True for k in GATES):raise RuntimeError('ABI/state/route/actor guard failed')
            xi=case%16;yi=(case//16)%16;off=bool(case&256);visited=bool(case&512);area=bool(case&1024);cell=10*CELLS[xi]+9-CELLS[yi];changed=not(off or visited or area)
            profiles.add((xi,yi,off,visited,area));cells_seen.add(cell);branches.add((off,visited,area))
            if (v['x_index'],v['y_index'],v['off_map'],v['visited'],v['interior'],v['cell'],v['cell_changed'])!=(xi,yi,off,visited,area,cell,changed):raise RuntimeError('Wrong branch/cell')
            expected_grid=[int((i+case*7)%11==0) for i in range(100)];expected_grid[cell]=int(visited);input_count=sum(expected_grid)
            if changed:expected_grid[cell]=1
            level=oracle_level(raw,b['map_zone_count'],COORDS[xi],COORDS[yi],0)
            if v['input_count']!=input_count or v['count']!=input_count+changed or v['grid']!=expected_grid or v['level']!=level or v['expected_level']!=level:raise RuntimeError('Full grid/count/map oracle mismatch')
            if v['coordinates_bytes']!=list(struct.pack('<3f',COORDS[xi],COORDS[yi],0)):raise RuntimeError('Current collaborator did not read actual player position')
    if pairs!=set(range(2048)) or cases!=set(range(2048)) or len(profiles)!=2048 or cells_seen!=set(range(100)) or len(branches)!=8:raise RuntimeError('Incomplete cells/branches')
    end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or end[0]['pairs']!=2048 or not all(end[0].get(k) is True for k in ('state_restored','helper_route_ok','native_complete_bodies_ok','floating_state_restored')):raise RuntimeError('Final restoration missing')
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependencieszones13',manifest['binaries']['asi'],analyze,'DependenciesZones13',timeout=1500))
