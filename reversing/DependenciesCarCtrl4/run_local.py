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
    physics_profiles=set()
    count_profiles=set()
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
                    'route_ok','stack_restored','nonvolatile_registers_preserved','protected_state_unchanged','expected_effects_correct')):
                raise RuntimeError('Resultado nulo o ABI/estado inválido en par '+str(row['pair']))
        result=row['original']
        if row['function']=='CCarCtrl::SwitchVehicleToRealPhysics':
            physics_profiles.add((result['initial_status'],result['timer_profile']))
        else:
            count_profiles.add((result['created_by'],result['decrease'],result['law_enforcer'],result['seed_profile']))
    if cases!={name:set(range(count)) for name,count in EXPECTED.items()}:
        raise RuntimeError('Casos por función incompletos o repetidos')
    if pairs!=set(range(2048)):
        raise RuntimeError('Faltan pares')
    if physics_profiles!={(status,time) for status in range(256) for time in range(4)}:
        raise RuntimeError('Cobertura status/timers incompleta')
    if count_profiles!={(by,down,law,seed) for by in (1,2,3,4,0,5,128,255)
            for down in (0,1,2,17,127,128,254,255) for law in range(2) for seed in range(8)}:
        raise RuntimeError('Cobertura createdBy/decrease/law/contadores incompleta')
    complete=[row for row in rows if row['event']=='complete']
    if len(complete)!=1 or complete[0]['pairs']!=2048 or not complete[0]['state_restored']:
        raise RuntimeError('Final incompleto o estado sin restaurar')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciescarctrl4',manifest['binaries']['asi'],analyze,
                 'CCarCtrl real physics + car counts',timeout=1500))
