"""Exige 4096 pares, dos contratos completos y restauración verificable."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CDecisionMakerTypes::GetInstance':2048,'CTaskComplexSunbathe::CanSunbathe':2048}
HELPERS={0x4650F0,0x4650A0,0x6040C0,0x600530,0x821D30,0x82119A,0x8214BD,0x824257,0x82413F,0x62F2D0}
GATES=('stack_restored','nonvolatile_preserved','seh_restored','route_ok','helper_route_ok','fixture_contract','resources_balanced','original_singleton_intact','state_restored','floating_state_restored')
HOURS=[0,1,8,9,10,11,12,13,14,15,16,17,18,19,23,255]
WEATHERS=list(range(23))+[-1,-2,-32768,32767,128,256,257,513,-255]
def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(s) for s in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1:raise RuntimeError('Missing begin')
    b=begin[0]
    if not all(b.get(k) is True for k in ('registered_hook','helper_routes_ok','native_complete_bodies_ok','original_singleton_intact')):raise RuntimeError('Missing native routes or original object')
    if b['singleton_bytes']!=0xF1C0 or b['defined_constructor_bytes']!=61504 or b['undefined_event_index_bytes']!=384 or len(b['constructor_bytes'])!=415 or len(b['weather_helper_bytes'])!=49:raise RuntimeError('Incomplete constructor contract')
    hs=b['helper_routes']
    if {h['entry'] for h in hs}!=HELPERS or not all(h['route_ok'] and h['module'] and len(h['entry_bytes'])==16 and len(h['body_bytes'])==64 for h in hs):raise RuntimeError('Missing shared helper routes')
    pairs=set();cases={k:set() for k in EXPECTED};profiles=set();null_modes=set()
    for r in rows:
        if r['event']!='comparison':continue
        fn=r['function'];case=r['case']
        if r['pair'] in pairs or case in cases[fn]:raise RuntimeError('Repeated case')
        pairs.add(r['pair']);cases[fn].add(case)
        if not r['equal'] or not r['accepted'] or not isinstance(r['original'],dict) or not isinstance(r['candidate'],dict) or r['original']!=r['candidate']:raise RuntimeError('Unequal/null/unaccepted pair')
        for v in (r['original'],r['candidate']):
            if not all(v.get(k) is True for k in GATES):raise RuntimeError('A route/ABI/restoration gate failed')
            if fn.endswith('GetInstance'):
                n=bool(case&1);mode=(case//2)%4;null_modes.add((n,mode))
                if v['null_branch']!=n or v['seed_mode']!=mode or not v['return_nonnull'] or not v['return_is_global'] or v['return_is_original']==n or len(v['global_bytes'])!=80:raise RuntimeError('Wrong singleton branch/return')
                expected=[(i+case*13)&255 for i in range(80)]
                if n:
                    expected=[0]*80
                    for i in range(20,60,2):expected[i]=1
                    if v['no_decision_makers'] or v['matrices_default']!=[True]*25 or v['defined_bytes_checked']!=61504 or v['allocations']!=1 or v['frees']!=1:raise RuntimeError('Incomplete real constructor/lifetime')
                elif v['matrices_default'] or v['defined_bytes_checked'] or v['allocations'] or v['frees']:raise RuntimeError('Unexpected existing-branch allocation')
                if v['global_bytes']!=expected:raise RuntimeError('Wrong singleton globals')
            else:
                h=HOURS[case%16];w=WEATHERS[(case//16)%32];seed=case//512;profiles.add((h,w,seed))
                expected=int(10<=h<18 and w in {0,1,2,3,5,6,10,11,13,14,17,18})
                if (v['hour'],v['weather'],v['seed_index'],v['return'],v['ecx'],v['edx'])!=(h,w,seed,expected,0x137AFF01,0xC0DE5921):raise RuntimeError('Wrong weather/ABI result')
    if pairs!=set(range(4096)) or any(v!=set(range(2048)) for v in cases.values()) or len(profiles)!=2048 or null_modes!={(n,s) for n in (False,True) for s in range(4)}:raise RuntimeError('Incomplete profiles')
    end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or end[0]['pairs']!=4096 or not all(end[0].get(k) is True for k in ('state_restored','original_singleton_intact','helper_route_ok','native_complete_bodies_ok','floating_state_restored')):raise RuntimeError('Final restoration missing')
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
        sys.exit(runtime_runner.run_case('dependenciestask11',manifest['binaries']['asi'],analyze,'DependenciesTask11',timeout=1500))
