"""Exige 4096 pares de las dos funciones Stats de radio y ABI definida."""
import json
import sys
from pathlib import Path
sys.path.insert(0,str(Path(__file__).resolve().parents[1]))
import runtime_runner
runtime_runner.GAME=Path('E:/JUEGOS/GTA San Andreas - CVehicle Test')
from probe_case import analyzer as common_analyzer
HERE=Path(__file__).resolve().parent
EXPECTED={'CStats::PopulateFavoriteRadioStationList':2048,'CStats::FindMostFavoriteRadioStation':2048}
GATES=('route_ok','helper_route_ok','stack_restored','nonvolatile_registers_preserved',
       'protected_state_unchanged','expected_effects_correct','fpu_unchanged')

def analyze(path):
    summary=common_analyzer(EXPECTED)(path)
    if summary is None:return None
    rows=[json.loads(line) for line in path.read_text(encoding='utf-8').splitlines()]
    begin=[r for r in rows if r['event']=='begin']
    if len(begin)!=1 or not begin[0]['registered_hook'] or not begin[0]['helper_route_ok'] or begin[0]['collaborators_stubbed']:
        raise RuntimeError('Rutas registradas o colaboradores invalidos')
    helpers=begin[0]['helpers']
    if len(helpers)!=2 or {h['entry'] for h in helpers}!={0x507020,0x4E83E0} or not all(h['helper_route_ok'] for h in helpers):
        raise RuntimeError('Faltan las dos rutas de los colaboradores de audio')
    pairs=set();cases={name:set() for name in EXPECTED};raw={}
    for row in rows:
        if row['event'].startswith('first_chance_av'):raise RuntimeError('Acceso invalido')
        if row['event']=='raw_abi_return':
            key=(row['pair'],row['candidate'])
            if key in raw:raise RuntimeError('Registro ABI duplicado')
            raw[key]=row
        if row['event']!='comparison':continue
        name=row['function']
        if row['pair'] in pairs or row['case'] in cases[name]:raise RuntimeError('Caso/par duplicado')
        pairs.add(row['pair']);cases[name].add(row['case'])
        if not row['state_restored'] or not row['equal'] or not row['accepted']:raise RuntimeError('Diferencia o restauracion fallida')
        for side,candidate in [('original',False),('candidate',True)]:
            result=row[side]
            if result is None or not all(result[k] is True for k in GATES):raise RuntimeError('Resultado/ruta/ABI invalido')
            if (result['profile'],result['value'])!=(row['case']//128,row['case']%128):raise RuntimeError('Perfil incorrecto')
            record=raw[(row['pair'],candidate)]
            mask=0xFF if name.endswith('PopulateFavoriteRadioStationList') else 0xFFFFFFFF
            if record['defined_mask']!=mask or record['eax']&mask!=result['return_bits']:raise RuntimeError('Retorno ABI mal normalizado')
    if pairs!=set(range(4096)) or cases!={name:set(range(2048)) for name in EXPECTED}:raise RuntimeError('Cobertura incompleta')
    if set(raw)!={(pair,side) for pair in pairs for side in (False,True)}:raise RuntimeError('Faltan registros ABI crudos')
    complete=[r for r in rows if r['event']=='complete']
    if len(complete)!=1 or complete[0]['pairs']!=4096 or not complete[0]['state_restored']:raise RuntimeError('Final incompleto')
    return summary

if __name__=='__main__':
    if len(sys.argv)==3 and sys.argv[1]=='--analyze':print(json.dumps(analyze(Path(sys.argv[2])),indent=2))
    else:
        manifest=json.loads((HERE/'artifacts/build-manifest.json').read_text())
        sys.exit(runtime_runner.run_case('dependenciesstatsradio7',manifest['binaries']['asi'],analyze,
                 'Stats radio real collaborators and signed station selection',timeout=1500))
