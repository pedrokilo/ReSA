"""Strict scalar/memory/trace conversion proof, raw instruction metadata retained."""
import json,struct,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
from probe_case import analyzer
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
HERE=Path(__file__).resolve().parent
EXPECTED={'Interior_c::FurnishOffice':2048,'Interior_c::Shop_AddShelfInfo':4096}
COUNTS=(-2147483648,-2,-1,0,1,2,3,2147483647)
COORDS=(-2147483648,-16777217,-16777216,-32768,-1,0,1,32767,16777216,16777217,2147483647)
def bits(v):return struct.unpack('<I',struct.pack('<f',v))[0]
def numeric(raw,mxcsr):
 b=bytes.fromhex(raw);cw,sw,tag=(struct.unpack_from('<H',b,n)[0] for n in (0,4,8));top=(sw>>11)&7;live=[]
 for logical in range(8):
  physical=(top+logical)%8;classification=(tag>>(physical*2))&3
  if classification!=3:live.append(dict(logical=logical,physical=physical,tag=classification,bits80=b[28+logical*10:38+logical*10].hex()))
 return dict(control_word=cw,status_word=sw,tag_word=tag,live_registers80=live,mxcsr=mxcsr)
def oracle(case,name,initial):
 state=bytearray.fromhex(initial);width=case%256;height=((case//256)*17+case%17)%256;wealth=case%5;counter=COUNTS[case%8];trace=[];rng=0
 if name.endswith('FurnishOffice'):
  for x,y in ((0,0),(0,height-2),(width-2,0),(width-2,height-2)):
   args=[x&0xffffffff,y&0xffffffff,2,2,2,0];trace.append([0x591700,args]);state[16+0x68+(args[0]%30)*30+args[1]%30]=2
  trace.extend([[0x591220,[1,0,wealth]],[0x591220,[1,1,wealth]],[0x599770,[]],[0x599a30,[]],[0x59a130,[]]])
  state[16+0x791]=case%7;state[16+0x792]=case%7
 else:
  if counter>1:
   draw=(case*7919)%32768;rng=1;trace.append(['rand',draw])
   if draw*100//32768>60:
    x=COORDS[(case//8)%11];y=COORDS[(case//88)%11];direction=case%256-128
    trace.append([0x591e40,[8,bits(x),bits(y),direction&0xffffffff,0]]);state[16+0x40d]=(state[16+0x40d]+1)&255;counter=1
   else:counter=(counter+1)&0xffffffff
  else:counter=(counter+1)&0xffffffff
 return list(state),trace,counter&0xffffffff,rng

def analyze(path):
 summary=analyzer(EXPECTED)(path)
 if summary is None:return None
 cases={name:set() for name in EXPECTED};coverage=set();starts=[]
 for line in path.read_text(encoding='utf-8').splitlines():
  r=json.loads(line)
  if r['event']=='begin':starts.append(r)
  if r['event'].startswith('first_chance_av'):raise RuntimeError('Access violation')
  if r['event']!='comparison':continue
  name=r['function'];case=r['case']
  if name not in EXPECTED or case in cases[name] or not 0<=case<EXPECTED[name]:raise RuntimeError('Duplicate or invalid case')
  cases[name].add(case)
  for side in ('original','candidate'):
   out=r[side]
   if out is None or not all(out.get(k) is True for k in ('switch','bindings_preserved','readonly_ok','guards_ok','abi_ok','numeric_restored','numeric_abi_preserved','floating_input_pc24_nearest')):raise RuntimeError('Null payload or false gate')
   raw=r[side+'_fp_raw'];mxcsr=r[side+'_mxcsr_raw']
   meta=r[side+'_fp_metadata'];raw_bytes=bytes.fromhex(raw)
   if meta['fip']!=int.from_bytes(raw_bytes[12:16],'little') or meta['fdp']!=int.from_bytes(raw_bytes[20:24],'little') or meta['opcode']!=int.from_bytes(raw_bytes[18:20],'little'):raise RuntimeError('Instruction metadata differs from raw')
   if len(meta['fip_code16']) not in (0,32):raise RuntimeError('Instruction code evidence truncated')
   if len(raw)!=216 or len(out['box_bytes'])!=192 or out['post_fp_numeric']!=numeric(raw,mxcsr):raise RuntimeError('Numeric snapshot hidden/truncated')
   expected_state,trace,counter,rng=oracle(case,name,out['initial_object'])
   if out['object_bytes']!=expected_state or out['trace']!=trace or (out['shelf_global']&0xffffffff)!=counter or out['office_global']!=case%2 or out['rng_calls']!=rng or out['return']!=0:raise RuntimeError('Independent memory/call oracle differs')
   expected_args=[COORDS[(case//8)%11]&0xffffffff,COORDS[(case//88)%11]&0xffffffff,(case%256-128)&0xffffffff]
   if out['input_args']!=expected_args:raise RuntimeError('Input corpus differs')
  if name.endswith('Shop_AddShelfInfo'):
   coverage.add('no_random' if not r['original']['rng_calls'] else 'added' if any(x[0]==0x591e40 for x in r['original']['trace']) else 'skipped')
   if COUNTS[case%8]==2147483647 and (r['original']['shelf_global']&0xffffffff)==0x80000000:coverage.add('wrap')
 if len(starts)!=1 or starts[0].get('registered_hook') is not True or starts[0].get('collaborators_stubbed') is not True or starts[0].get('functions')!=2 or starts[0].get('pairs')!=6144:raise RuntimeError('Route/scope manifest missing')
 for name,count in EXPECTED.items():
  if cases[name]!=set(range(count)):raise RuntimeError('Case coverage incomplete')
 if not {'no_random','added','skipped','wrap'}<=coverage:raise RuntimeError('Branch coverage incomplete')
 summary['scope']='C++ body conversion fixture proof; collaborators instrumented, no transitive approval'
 summary['metadata_policy']='FP108 raw retained; compare CW/SW/tag/live80/MXCSR; instruction/data addresses and empty-register residue are not scalar outcomes'
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
  sys.exit(runtime_runner.run_case('cpp-interior-next',manifest['binaries']['asi'],analyze,'CppInteriorNext',timeout=1800))
