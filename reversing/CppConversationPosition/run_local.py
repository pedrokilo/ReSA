import json,re,sys
from collections import Counter
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
from runtime_runner import run_case
here=Path(__file__).parent
source=(here/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED={f'{m[0]}::{m[1]}':int(m[2]) for m in re.findall(r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}',source)}
NAME='CConversationForPed::IsPlayerInPositionForConversation'
# Familias heredadas del banco historico (README de ../ConversationForPed): 154+48+32+2+2160+18 = 2414.
FAMILIES={'distance':84,'facing':10,'speed':60,'damage':48,'availability':32,'placement-early-out':2,
          'mixed-distance':1080,'mixed-speed':1080,'x87-precision':18}
def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    if EXPECTED!={NAME:2414}:raise RuntimeError('Tabla de funciones inesperada')
    rows=[json.loads(l) for l in path.read_text(encoding='utf-8').splitlines()]
    pairs=[r for r in rows if r.get('event')=='comparison']
    for r in pairs:
        for side in ('original','candidate'):
            v=r.get(side)
            if not isinstance(v,dict) or v.get('switches') is not True or v.get('inputs_intact') is not True or not isinstance(v.get('result'),bool) or not isinstance(v.get('calls'),list):
                raise RuntimeError(f"Ruta/contrato nulo o falso en {r['pair']} ({side})")
    if Counter(r['name'].rsplit('-',1)[0] for r in pairs)!=Counter(FAMILIES):raise RuntimeError('Reparto de familias inesperado')
    fs=[r for r in rows if r.get('event')=='function_summary']
    if len(fs)!=1 or fs[0]['function']!=NAME or fs[0]['cases']!=2414 or fs[0]['failures']!=0 or fs[0]['positive']<1 or fs[0]['negative']<1:raise RuntimeError('Resumen de funcion incompleto')
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2));sys.exit(0)
    probe_hash=json.loads((here/'artifacts/build-manifest.json').read_text())['binaries']['asi']
    sys.exit(run_case('cpp-conversation-position',probe_hash,analyze,'CppConversationPosition',timeout=1500))
