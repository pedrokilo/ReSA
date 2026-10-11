import json,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={f'CClouds::{name}':1024 for name in ('Update','RenderSkyPolys','MovingFogRender')}
def numeric(raw,mxcsr):
 b=bytes.fromhex(raw)
 if len(b)!=108:raise RuntimeError('FP108 incompleto')
 word=lambda n:int.from_bytes(b[n:n+2],'little')
 sw,tag=word(4),word(8);top=sw>>11&7;live=[]
 for logical in range(8):
  physical=(top+logical)%8;classification=tag>>(physical*2)&3
  if classification!=3:live.append(dict(logical=logical,physical=physical,tag=classification,bits80=b[28+10*logical:38+10*logical].hex()))
 return dict(control_word=word(0),status_word=sw,tag_word=tag,live_registers80=live,mxcsr=mxcsr)
def analyze(path):
 summary=analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(line) for line in path.read_text(encoding='utf8').splitlines()]
 cases={name:set() for name in EXPECTED}
 for row in rows:
  if row.get('event')!='comparison':continue
  name=row['function'];case=row['case']
  if case in cases[name]:raise RuntimeError('Caso duplicado')
  cases[name].add(case)
  for side in ('original','candidate'):
   out=row[side]
   if not isinstance(out,dict):raise RuntimeError('Payload nulo')
   if not all(out.get(key) is True for key in ('switches','abi_ok','route_ok','side_fp_restored','numeric_abi_ok')):raise RuntimeError('Gate de ruta/ABI/restauración falló')
   if out['post_fp_numeric']!=numeric(row[side+'_fp108'],row[side+'_mxcsr']):raise RuntimeError('Numeric FP108 no coincide con evidencia raw')
   raw=bytes.fromhex(row[side+'_fp108']);metadata=row[side+'_fp_metadata']
   for key,offset,size in [('fip',12,4),('fdp',20,4),('instruction_selector',16,2),('opcode',18,2),('data_selector',24,2)]:
    if metadata[key]!=int.from_bytes(raw[offset:offset+size],'little'):raise RuntimeError('Metadata FP108 inconsistente')
 for name,count in EXPECTED.items():
  if cases[name]!=set(range(count)):raise RuntimeError('Casos incompletos')
 starts=[r for r in rows if r.get('event')=='begin']
 if len(starts)!=1 or starts[0]!=dict(event='begin',functions=3,pairs=3072,registered_hook=True,collaborators_stubbed=True):raise RuntimeError('Manifiesto de alcance incorrecto')
 summary['scope']='Conversión de tres helpers C++: 3 consumidores, 3072 pares, colaboradores instrumentados; no aprobación transitiva'
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
  sys.exit(runtime_runner.run_case('cpp-clouds-math3',manifest['binaries']['asi'],analyze,'CppCloudsMath3',timeout=1800))
