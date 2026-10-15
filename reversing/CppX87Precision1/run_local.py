"""Exige, por funcion, 1024 casos x 3 precisiones x 4 redondeos EXE-vs-DLL; igualdad bit a bit en las precisiones del contrato, ABI y palabras de control restauradas siempre."""
import itertools
import json
import re
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
# El juego completo solo arranca desde la ruta real de E:; se redirige en memoria sin tocar runtime_runner.py.
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from runtime_runner import digest, terminal_event
HERE=Path(__file__).resolve().parent
src=(HERE/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED={f'{cls}::{name}':(int(mask),int(cases)) for cls,name,mask,cases in re.findall(
    r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),k\w+,(\d+),(\d+)\}',src)}
PCS=(24,53,64)
RCS=('near','down','up','chop')
ABI=('route_ok','stack_restored','nonvolatile_registers_preserved','mode_restored','x87_stack_empty')

def analyze(path):
    if not terminal_event(path):return None
    seen={name:set() for name in EXPECTED}
    exploratory={name:{} for name in EXPECTED}
    required_pairs=0;complete=[];begin=[]
    with path.open(encoding='utf-8') as f:
        for line in f:
            if not line.endswith('\n'):continue
            row=json.loads(line)
            event=row['event']
            if event in ('error','assertion'):raise RuntimeError('Fallo dentro del motor; consultar comparisons.jsonl: '+line[:300])
            if event=='begin':begin.append(row)
            if event=='complete':complete.append(row)
            if event.startswith('first_chance_av'):raise RuntimeError('Acceso invalido durante el banco')
            if event!='comparison':continue
            key=(row['pc'],row['rc'],row['case'])
            if key in seen[row['function']]:raise RuntimeError('Caso repetido '+str(row['pair']))
            seen[row['function']].add(key)
            for side in ('original','candidate'):
                abi=row[side]['abi']
                if not all(abi[k] is True for k in ABI):
                    raise RuntimeError(f"ABI/modo/ruta invalidos ({side}) en par {row['pair']} {row['function']} pc={row['pc']} rc={row['rc']}: {abi}")
            if row['required']:
                required_pairs+=1
                if not (row['equal'] and row['accepted'] and row['original']['out']==row['candidate']['out']):
                    raise RuntimeError(f"Diferencia exigida en par {row['pair']} {row['function']} pc={row['pc']} rc={row['rc']} caso {row['case']}")
            elif not row['equal']:
                k=f"pc{row['pc']}-{row['rc']}"
                exploratory[row['function']][k]=exploratory[row['function']].get(k,0)+1
    if len(begin)!=1 or len(complete)!=1:raise RuntimeError('Falta begin/complete')
    if not complete[0]['state_restored']:raise RuntimeError('Estado (palabras de control, errno, globales, hooks) sin restaurar')
    for name,(mask,cases) in EXPECTED.items():
        if seen[name]!={(pc,rc,i) for pc,rc,i in itertools.product(PCS,RCS,range(cases))}:raise RuntimeError('Cobertura incompleta en '+name)
    total=sum(len(v) for v in seen.values())
    if complete[0]['pairs']!=total:raise RuntimeError('Total de pares inesperado')
    gaps={n:v for n,v in exploratory.items() if v}
    return dict(pairs=total,pairs_per_function={n:len(v) for n,v in seen.items()},required_pairs=required_pairs,all_equal=True,
                all_equal_every_combination=not gaps,exploratory_diffs=sum(sum(v.values()) for v in gaps.values()),exploratory_by_function=gaps,
                contract_masks={n:m for n,(m,c) in EXPECTED.items()},state_restored=True,log_sha256=digest(path))

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cppx87precision1',manifest['binaries']['asi'],analyze,
                 'X87Math: EXE vs DLL bajo precision 24/53/64 y redondeo cercano/abajo/arriba/cero',timeout=3000))
