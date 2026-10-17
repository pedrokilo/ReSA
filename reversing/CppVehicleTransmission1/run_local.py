"""Exige 1024 pares EXE (hook apagado) vs DLL (hook activo) de SetTransmissionRotation, ABI/restauracion por par, traza de dos angulos y cero nulos."""
import json
import re
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
# El juego completo solo arranca desde la ruta real de E:; se redirige en memoria sin tocar runtime_runner.py.
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
src=(HERE/'LiveComparison.hpp').read_text(encoding='utf-8')
EXPECTED={f'{cls}::{name}':int(cases) for cls,name,cases in re.findall(
    r'\{"(\w+)","([^"]+)",(?:0x[0-9A-F]+|0),(?:0x[0-9A-F]+|0),-?\d+,(\d+)\}',src)}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin=[row for row in rows if row['event']=='begin']
    if len(begin)!=1 or not begin[0]['registered_hook'] or not begin[0]['collaborators_stubbed']:
        raise RuntimeError('SetTransmissionRotation debe estar registrada y los dos colaboradores de CMatrix sustituidos')
    cases={name:set() for name in EXPECTED}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido durante el banco')
        if row['event']!='comparison':continue
        if not row['state_restored']:
            raise RuntimeError('Estado sin restaurar en par '+str(row['pair']))
        cases[row['function']].add(row['case'])
        for side in ('original','candidate'):
            result=row[side]
            if result is None or len(result['trace'])!=2 or not all(result[key] for key in (
                    'stack_restored','nonvolatile_registers_preserved','guards_unchanged','fp_control_kept')):
                raise RuntimeError('Resultado nulo o ABI/estado invalido en par '+str(row['pair']))
    if cases!={name:set(range(count)) for name,count in EXPECTED.items()}:
        raise RuntimeError('Casos por funcion incompletos o repetidos')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cppvehicletransmission1',manifest['binaries']['asi'],analyze,
                 'CVehicle::SetTransmissionRotation, EXE vs DLL con estres x87',timeout=1500))
