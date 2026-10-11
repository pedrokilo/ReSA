"""Paired fixture proof inside the game; no catalog admission or tolerances."""
import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
from probe_case import analyzer
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
HERE=Path(__file__).resolve().parent
EXPECTED={'Interior_c::Shop_FurnishCeiling':2048,'Interior_c::Office_FurnishCenter':4096}
def trunc(n,d):return n//d if n>=0 else -((-n)//d)
def oracle(case):
 width=case%256-6;height=((case//256)*17+case%17)%256-6
 columns=trunc(width,6);rows=trunc(height,6)
 inset_x=trunc(width-columns*6,2);inset_y=trunc(height-rows*6,2)
 identifier=(case%256)-256 if case%256>=128 else case%256
 trace=[]
 if width>0 and height>0 and columns>0:
  for column in range(columns):
   for row in range(rows):trace.append([0x599960,[0xffffffff,inset_x+6+column*6,inset_y+6+row*6,identifier&0xffffffff]])
 return inset_y&0xffffffff,trace

def analyze(path):
 summary=analyzer(EXPECTED)(path)
 if summary is None:return None
 cases={name:set() for name in EXPECTED}
 for line in path.read_text(encoding='utf-8').splitlines():
  r=json.loads(line)
  if r['event'].startswith('first_chance_av'):raise RuntimeError('Access violation')
  if r['event']!='comparison':continue
  name=r['function'];case=r['case']
  if name not in EXPECTED or case in cases[name] or not 0<=case<EXPECTED[name]:raise RuntimeError('Duplicate or invalid case')
  cases[name].add(case)
  for side in ('original','candidate'):
   out=r[side]
   if out is None or not all(out.get(k) is True for k in ('switch','bindings_preserved','readonly_ok','abi_ok','numeric_restored','ceiling_flags_preserved')):raise RuntimeError('Null payload or false gate')
   if len(out['post_fp'])!=216 or len(out['box_bytes'])!=128:raise RuntimeError('Truncated snapshot')
   if out['rng_calls']!=0 or out['office_global']!=case%2 or out['shelf_global']!=case%6:raise RuntimeError('Unexpected RNG/global mutation')
   if name.endswith('Office_FurnishCenter'):
    value,trace=oracle(case)
    if out['return']!=value or out['trace']!=trace:raise RuntimeError('Independent integer oracle differs')
   elif out['trace'] or out['return']!=0 or len(out['ceiling_volatile'])!=3 or out['ceiling_volatile'][0]!=0x59a130 or out['ceiling_volatile'][2]!=0xc1234567:raise RuntimeError('Ceiling has effects or clobbers volatile registers')
 for name,count in EXPECTED.items():
  if cases[name]!=set(range(count)):raise RuntimeError('Case coverage incomplete')
 summary['scope']='C++ conversion fixture proof; DeskQuad collaborator instrumented, no transitive approval'
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
  sys.exit(runtime_runner.run_case('cpp-interior-proposal',manifest['binaries']['asi'],analyze,'CppInteriorProposal',timeout=1800))
