"""Exige dos funciones RH OFF/ON, 2048 pares, ABI/restauración y cero nulos."""
import json
import re
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
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
    if len(begin)!=1 or not begin[0]['registered_hook']:
        raise RuntimeError('Los dos hooks deben estar registrados en producción')
    cases={name:set() for name in EXPECTED}
    pairs=set()
    branches=set()
    layouts={name:set() for name in EXPECTED}
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso inválido durante el banco')
        if row['event']!='comparison':continue
        if row['pair'] in pairs or row['case'] in cases[row['function']]:
            raise RuntimeError('Par o caso duplicado')
        pairs.add(row['pair'])
        if not row['state_restored']:
            raise RuntimeError('Estado sin restaurar en par '+str(row['pair']))
        cases[row['function']].add(row['case'])
        for side in ('original','candidate'):
            result=row[side]
            if result is None or not all(result[key] for key in (
                    'route_ok','stack_restored','nonvolatile_registers_preserved','protected_state_unchanged','finite_output')):
                raise RuntimeError('Resultado nulo o ABI/estado inválido en par '+str(row['pair']))
        layouts[row['function']].add(row['original']['layout'])
        if row['function']=='CQuaternion::Set-matrix':
            branches.add(row['original']['matrix_branch'])
    if cases!={name:set(range(count)) for name,count in EXPECTED.items()}:
        raise RuntimeError('Casos por función incompletos o repetidos')
    if pairs!=set(range(2048)) or branches!={0,1,2,3} or any(v!={0,1,2,3} for v in layouts.values()):
        raise RuntimeError('Faltan pares, ramas de matriz o perfiles de solapamiento')
    complete=[row for row in rows if row['event']=='complete']
    if len(complete)!=1 or complete[0]['pairs']!=2048 or not complete[0]['state_restored']:
        raise RuntimeError('Final incompleto o estado sin restaurar')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesmathleaf3',manifest['binaries']['asi'],analyze,
                 'Quaternion Set matrix + axis-angle',timeout=1500))
