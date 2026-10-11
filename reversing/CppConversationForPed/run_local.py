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
POSITION='CConversationForPed::IsPlayerInPositionForConversation'
UPDATE='CConversationForPed::Update'
# Familias heredadas del banco historico (README de ../ConversationForPed): 2414 de posicion + 2288 de Update = 4702.
FAMILIES={POSITION:{'distance':84,'facing':10,'speed':60,'damage':48,'availability':32,'placement-early-out':2,
                    'mixed-distance':1080,'mixed-speed':1080,'x87-precision':18},
          UPDATE:{'state-timer':1200,'reply-branches':1024,'question-audio':32,'inactive-gates':32}}
def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    if EXPECTED!={POSITION:2414,UPDATE:2288}:raise RuntimeError('Tabla de funciones inesperada')
    rows=[json.loads(l) for l in path.read_text(encoding='utf-8').splitlines()]
    pairs=[r for r in rows if r.get('event')=='comparison']
    for r in pairs:
        for side in ('original','candidate'):
            v=r.get(side)
            if not isinstance(v,dict) or v.get('switches') is not True or v.get('inputs_intact') is not True or not isinstance(v.get('result'),bool) or not isinstance(v.get('calls'),list):
                raise RuntimeError(f"Ruta/contrato nulo o falso en {r['pair']} ({side})")
    for fn,families in FAMILIES.items():
        got=Counter(r['name'].rsplit('-',1)[0] for r in pairs if r['function']==fn)
        if got!=Counter(families):raise RuntimeError(f'Reparto de familias inesperado en {fn}')
    fs={r['function']:r for r in rows if r.get('event')=='function_summary'}
    if set(fs)!=set(EXPECTED):raise RuntimeError('Resumen de funcion incompleto')
    for fn,count in EXPECTED.items():
        r=fs[fn]
        if r['cases']!=count or r['failures']!=0 or r['positive']<1 or r['negative']<1:raise RuntimeError(f'Resumen de funcion incompleto: {fn}')
    return summary
if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2));sys.exit(0)
    probe_hash=json.loads((here/'artifacts/build-manifest.json').read_text())['binaries']['asi']
    sys.exit(run_case('cpp-conversation-for-ped',probe_hash,analyze,'CppConversationForPed',timeout=1500))
