"""Verifica dos adaptadores RH OFF/ON, recursos reales y restauracion."""
import json
import re
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CCollision::CalculateTrianglePlanes-colModel':1024,'CCollision::RemoveTrianglePlanes-colModel':1024}
HELPERS={0x416330,0x416400,0x40F590,0x40F6A0,0x40F6C0,0x40F6E0,0x411660}
GATES=('route_ok','helper_route_ok','stack_restored','nonvolatile_registers_preserved',
       'protected_state_unchanged','expected_effects_correct','cache_consistent',
       'allocation_balance_restored','allocator_route_ok','final_allocation_outputs_valid')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1 or not begin[0]['registered_hook'] or not begin[0]['helper_route_ok'] or set(begin[0]['helper_entries'])!=HELPERS:
        raise RuntimeError('Rutas registradas o colaboradores incompletos')
    if begin[0].get('allocator_argument_count')!=1:raise RuntimeError('Malloc original recibe solo size')
    if begin[0]['collaborators_stubbed'] or begin[0]['allocator_stubbed']:
        raise RuntimeError('No se permiten sustitutos')
    pairs=set();cases={name:set() for name in EXPECTED};raw={}
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if row['event']=='raw_allocations':
            key=(row['pair'],row['candidate'])
            if key in raw:raise RuntimeError('Observacion cruda duplicada')
            raw[key]=row['observations']
            ids=set()
            for obs in row['observations']:
                if obs['id'] in ids or not obs['actual_address'] or not obs['padding_preserved']:
                    raise RuntimeError('Asignacion o padding invalido')
                ids.add(obs['id'])
                size=obs['size'];initial=obs['initial_raw'];final=obs['final_raw']
                if size not in (10,20,30,40,50) or len(initial)!=size or len(final)!=size:
                    raise RuntimeError('Asignacion real incompleta')
                triangles=size//10-1;link=(triangles*10+3)&~3
                for i in range(size):
                    if not (i<triangles*10 and i%10<9 or link<=i<link+4) and initial[i]!=final[i]:
                        raise RuntimeError('Padding o trailer modificado')
                if obs['freed_phase'] not in (1,2):raise RuntimeError('Recurso no liberado por ejecucion/limpieza')
        if row['event']!='comparison':continue
        name=row['function']
        if row['pair'] in pairs or row['case'] in cases[name]:raise RuntimeError('Caso/par duplicado')
        pairs.add(row['pair']);cases[name].add(row['case'])
        if not row['state_restored'] or not row['equal'] or not row['accepted']:raise RuntimeError('Diferencia o restauracion fallida')
        for side in ('original','candidate'):
            result=row[side]
            if result is None or not all(result[k] is True for k in GATES):raise RuntimeError('Resultado/ABI/ruta/recursos invalidos')
            if result['profile']!=row['case']//128:raise RuntimeError('Perfil incorrecto')
    if pairs!=set(range(2048)) or cases!={name:set(range(1024)) for name in EXPECTED}:raise RuntimeError('Cobertura incompleta')
    if set(raw)!={(pair,side) for pair in pairs for side in (False,True)}:raise RuntimeError('Faltan observaciones crudas')
    def defined(observations):
        result=[]
        for obs in observations:
            count=obs['size']//10-1;link=(count*10+3)&~3;final=obs['final_raw']
            result.append({'id':obs['id'],'size':obs['size'],
                           'allocated_phase':obs['allocated_phase'],'freed_phase':obs['freed_phase'],
                           'fields':[final[t*10:t*10+9] for t in range(count)],'link_raw':final[link:link+4]})
        return result
    for row in rows:
        if row['event']!='comparison':continue
        normalized=[]
        for side,candidate in [('original',False),('candidate',True)]:
            outputs=defined(raw[(row['pair'],candidate)])
            logged=row[side]['final_allocations']
            if len(outputs)!=len(logged):raise RuntimeError('Falta output de asignacion ya liberada')
            for expected,item in zip(outputs,logged):
                if not 0<=item['link_index']<4 or {key:item[key] for key in expected}!=expected:
                    raise RuntimeError('Normalizacion de campos definidos/enlace no coincide con bloque crudo')
            normalized.append(outputs)
        if normalized[0]!=normalized[1]:raise RuntimeError('Diferencia real en campos de un plano, incluido liberado')
    end=[r for r in rows if r['event']=='complete']
    if len(end)!=1 or end[0]['pairs']!=2048 or not end[0]['state_restored']:raise RuntimeError('Final incompleto')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciescollisionadapters1',manifest['binaries']['asi'],analyze,
                 'Collision model adapters with isolated cache and real allocation',timeout=1500))
