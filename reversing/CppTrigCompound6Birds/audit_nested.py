import json
from pathlib import Path
from probe_case import analyzer
EXPECTED = {'CBirds::CreateNumberOfBirds': 1024, 'CBirds::Update': 1024, 'CBirds::Render': 512}
def analyze(path):
    value=analyzer(EXPECTED)(path)
    if value is None:return None
    rows=[json.loads(line) for line in Path(path).read_text(encoding='utf-8').splitlines() if line]
    spawn_coverage={'original':0,'candidate':0}
    for row in rows:
        if row.get('event')!='comparison':continue
        for key in ('original','candidate'):
            snapshot=row.get(key)
            if not isinstance(snapshot,dict):raise RuntimeError('Snapshot completo requerido')
            if snapshot.get('switches') is not True:raise RuntimeError('Ruta real incorrecta')
            for field in ('ret','trace'):
                if field not in snapshot:raise RuntimeError('Snapshot incompleto '+field)
            if not isinstance(snapshot['trace'],list):raise RuntimeError('Trace incorrecta')
            if row.get('function')=='CBirds::Update' and any(isinstance(call,list) and call and call[0]=='water' for call in snapshot['trace']):
                spawn_coverage[key]+=1
        if row['original']!=row['candidate']:raise RuntimeError('Diferencia nested')
    if not all(spawn_coverage.values()):raise RuntimeError('No se observo la rama spawn de Update con seno y coseno')
    if spawn_coverage['original']!=spawn_coverage['candidate']:raise RuntimeError('Cobertura spawn diferente')
    value['observed_coverage']={'update_spawn_pairs_with_water_trace':spawn_coverage}
    return value
