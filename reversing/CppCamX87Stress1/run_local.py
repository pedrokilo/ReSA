"""Exige los pares EXE-vs-DLL de los ayudantes x87 de CCam (bits exactos), ABI, pila x87 y restauracion por par, y cero nulos."""
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
CASES=int(re.search(r'kCases = (\d+)',src).group(1))
EXPECTED={f'{cls}::{name}':CASES for cls,name in re.findall(r'\{"(\w+)","([^"]+)",0,0,-1,kCases\}',src)}

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin=[row for row in rows if row['event']=='begin']
    if len(begin)!=1 or not begin[0]['exe_unhooked'] or not begin[0]['direct_dll_call']:
        raise RuntimeError('Los ayudantes de la DLL deben llamarse directamente y el EXE no debe estar hookeado')
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
            if result is None or not all(result[key] for key in (
                    'route_ok','stack_restored','nonvolatile_registers_preserved','protected_state_unchanged','x87_stack_balanced')):
                raise RuntimeError('Resultado nulo o ABI/estado invalido en par '+str(row['pair']))
    if cases!={name:set(range(CASES)) for name in EXPECTED}:
        raise RuntimeError('Casos por funcion incompletos o repetidos')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':
        print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('cppcamx87stress1',manifest['binaries']['asi'],analyze,
                 'Ayudantes x87 de CCam, EXE vs DLL con estres',timeout=3000))
