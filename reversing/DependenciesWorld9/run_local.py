"""Strict independent gate/coverage analyzer; root runs game in series."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
WATER='CWaterLevel::CalculateWavesOnlyForCoordinate2Native'
SETTER='CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D'
EXPECTED={WATER:2048,SETTER:2048}
GATES=('route_ok','library_route_ok','fp_state_restored','globals_restored','tables_real','stack_restored','registers_preserved','fp_preserved','contract_ok','readonly_ok','guards_ok')
def analyze(path):
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()]
 begin=[r for r in rows if r.get('event')=='begin']
 if len(begin)!=1 or not all(begin[0].get(k)is True for k in ('registered_hook','actual_crt_library','loaded_sine_table')) or begin[0].get('collaborators_stubbed')is not False or begin[0].get('game_collaborators')!=0:raise RuntimeError('Begin without real CRT/tables/hooks')
 b=begin[0]
 if b.get('crt_entry')!=0x821B40 or not b.get('crt_core') or len(b.get('crt_entry_bytes',[]))!=64 or len(b.get('crt_core_bytes',[]))!=64:raise RuntimeError('CRT bytes/core missing')
 cases={fn:set()for fn in EXPECTED};pairs=set();coords=set();elapsed=set();weather=set();big=set();small=set();changed=set();ticks=set();nan=False;signedzero=False
 for r in rows:
  if r.get('event')=='error' or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  fn=r['function'];case=r['case'];pair=r['pair']
  if fn not in cases or pair in pairs or case in cases[fn]:raise RuntimeError('Unknown/repeated case')
  pairs.add(pair);cases[fn].add(case)
  if r.get('original')!=r.get('candidate') or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference/rejected pair')
  for side in ('original','candidate'):
   v=r.get(side)
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   if not all(k in v for k in ('return_bits','raw_ecx','raw_edx','argument_slots_after'))or len(v['argument_slots_after'])!=5:raise RuntimeError('Raw ABI observation missing')
  v=r['original']
  if fn==WATER:
   for k in ('x','y'):coords.add(0 if v[k]==0 else 1 if v[k]>0 else -1)
   elapsed.add(v['elapsed']);weather.add(v['weather_index']);big.add(v['big_amp_index']);small.add(v['small_amp_index'])
  else:
   changed.add(v['distance_changed']);ticks.add(v['ticks'])
   nan|=any((v[k]&0x7F800000)==0x7F800000 and v[k]&0x007FFFFF for k in ('old_bits','new_bits'))
   signedzero|={v['old_bits'],v['new_bits']}=={0,0x80000000}and not v['distance_changed']
 if pairs!=set(range(4096))or cases!={fn:set(range(2048))for fn in EXPECTED}:raise RuntimeError('Missing cases/pairs')
 if coords!={-1,0,1}or elapsed!={0,2999,3000,3499,3500,4999,5000,123456}or weather!=set(range(4))or big!=set(range(4))or small!=set(range(4)):raise RuntimeError('Missing wave coverage')
 if changed!={False,True}or ticks!={0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF}or not nan or not signedzero:raise RuntimeError('Missing setter comparison/timer coverage')
 fs=[r for r in rows if r.get('event')=='function_summary']
 if len(fs)!=2 or {r['function']for r in fs}!=set(EXPECTED)or not all(r.get('complete_paths')is True and r.get('failures')==0 and r.get('cases')==2048 for r in fs):raise RuntimeError('Incomplete function summary')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=4096 or not all(end[0].get(k)is True for k in ('state_restored','fp_state_restored','crt_unchanged')):raise RuntimeError('Missing exact final restore')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('dependenciesworld9',manifest['binaries']['asi'],analyze,'DependenciesWorld9',timeout=1500))
