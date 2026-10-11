"""Puertas estrictas para CConversationNode::ClearRecursively (EXE vs ConversationNodeCpp.cpp)."""
import json,re,sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
source=(HERE/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED={f'{m[0]}::{m[1]}':int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}',source)}
FN='CConversationNode::ClearRecursively'
GATES=('called','fixture_acyclic','route_ok','stack_restored','registers_preserved','edx_preserved','df_preserved','oracle_ok','guards_ok','snapshot_restored','fp_state_restored')
def analyze(path):
 if EXPECTED!={FN:2048}:raise RuntimeError('Tabla fns[] inesperada')
 summary=common_analyzer(EXPECTED)(path)
 if summary is None:return None
 rows=[json.loads(l)for l in path.read_text(encoding='utf-8').splitlines()];beg=[r for r in rows if r.get('event')=='begin']
 if len(beg)!=1 or not all(beg[0].get(k)is True for k in ('registered_hook','nodes_address_ok','fp_state_restored'))or beg[0].get('collaborators_stubbed')is not False:raise RuntimeError('Hook RH o disposicion de nodos no verificados')
 cases=set();flags=set();shapes=set();roots=set()
 for r in rows:
  if r.get('event')=='error'or r.get('event','').startswith('first_chance_av'):raise RuntimeError('Runtime error')
  if r.get('event')!='comparison':continue
  if r['function']!=FN or r['case']in cases:raise RuntimeError('Funcion/caso duplicado')
  cases.add(r['case'])
  if r.get('original')!=r.get('candidate')or r.get('equal')is not True or r.get('accepted')is not True:raise RuntimeError('Difference')
  for side in('original','candidate'):
   v=r[side]
   if not isinstance(v,dict)or not all(v.get(k)is True for k in GATES):raise RuntimeError('NULL/false gate')
   if v['edx']!=v['edx_seed']or v['touched']!=v['reachable']or v['flags_df_before']!=v['flags_df_after']:raise RuntimeError('EDX/touched/DF contract')
  flags.add(r['original']['flags_defined']);shapes.add(r['shape']);roots.add(r['root'])
 if cases!=set(range(2048))or shapes!=set(range(16))or not{0,49}<=roots:raise RuntimeError('Incomplete coverage')
 if len(flags)!=1:raise RuntimeError('Banderas definidas no constantes: '+str(sorted(flags)))
 ss=[r for r in rows if r.get('event')=='function_summary']
 if len(ss)!=1 or not(ss[0].get('complete_paths')is True and ss[0].get('cases')==2048 and ss[0].get('failures')==0):raise RuntimeError('Incomplete summary')
 end=[r for r in rows if r.get('event')=='complete']
 if len(end)!=1 or end[0].get('pairs')!=2048 or not all(end[0].get(k)is True for k in('state_restored','fp_state_restored','hook_restored')):raise RuntimeError('Final restore missing')
 return summary
if __name__=='__main__':
 if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
 else:
  m=json.loads((HERE/'artifacts/build-manifest.json').read_text(encoding='utf-8'))
  sys.exit(runtime_runner.run_case('cppconversationclear',m['binaries']['asi'],analyze,'CppConversationClear',timeout=1500))
