"""Exige 21504 pares EXE-vs-DLL (llamada directa, sin hook), ABI/restauracion por par, cobertura de estres y cero nulos."""
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
    if len(begin)!=1 or not begin[0]['direct_dll_call'] or begin[0]['registered_hook'] or not begin[0]['exe_unhooked']:
        raise RuntimeError('Los metodos de la DLL deben llamarse directamente y el EXE no debe estar hookeado')
    cases={name:set() for name in EXPECTED}
    special={name:0 for name in EXPECTED}
    gimbal=0; modes=set(); flags=set()
    for row in rows:
        if row['event'].startswith('first_chance_av'):
            raise RuntimeError('Acceso invalido durante el banco')
        if row['event']!='comparison':continue
        if not row['state_restored']:
            raise RuntimeError('Estado sin restaurar en par '+str(row['pair']))
        cases[row['function']].add(row['case'])
        fixture=row['fixture']
        special[row['function']]+=bool(fixture['special'])
        if row['function']=='CMatrix::ConvertToEulerAngles':
            gimbal+=bool(fixture['gimbal']); modes.add(fixture['mode']); flags.add(fixture['flags']&0x1F)
        if row['function'].startswith('CMatrix::Convert'):
            flags.add(fixture['flags']&0x1F)
        for side in ('original','candidate'):
            result=row[side]
            if result is None or not all(result[key] for key in (
                    'route_ok','stack_restored','nonvolatile_registers_preserved','fpu_stack_balanced','protected_state_unchanged')):
                raise RuntimeError('Resultado nulo o ABI/estado invalido en par '+str(row['pair']))
    if cases!={name:set(range(count)) for name,count in EXPECTED.items()}:
        raise RuntimeError('Casos por funcion incompletos o repetidos')
    # Cobertura minima de estres: al menos 5 % de pares con NaN/inf/subnormales/enormes en cada funcion, 25 % de bloqueo cardan, 8 modos de salida y las 32 banderas.
    for name,count in EXPECTED.items():
        if special[name]*20<count:
            raise RuntimeError(f'Estres insuficiente en {name}: {special[name]}/{count}')
    if gimbal*4<EXPECTED['CMatrix::ConvertToEulerAngles'] or modes!=set(range(8)) or flags!=set(range(32)):
        raise RuntimeError('Cobertura insuficiente de ConvertToEulerAngles (bloqueo cardan, modos de salida o banderas)')
    summary['stress_pairs']=special
    summary['gimbal_pairs']=gimbal
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cppmatrixexact1',manifest['binaries']['asi'],analyze,
                 'CMatrix (SetRotate*, Rotate*, Reorthogonalise, Euler), EXE vs DLL directa',timeout=1800))
