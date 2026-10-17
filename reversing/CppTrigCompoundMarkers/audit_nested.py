import json
from pathlib import Path
from probe_case import analyzer
EXPECTED = {'C3dMarkers::PlaceMarker': 4096, 'C3dMarkers::User3dMarkersDraw': 512}
def analyze(path):
    value=analyzer(EXPECTED)(path)
    if value is None:return None
    rows=[json.loads(line) for line in Path(path).read_text(encoding='utf-8').splitlines() if line]
    for row in rows:
        if row.get('event')!='comparison':continue
        for key in ('original','candidate'):
            snapshot=row.get(key)
            if not isinstance(snapshot,dict):raise RuntimeError('Snapshot completo requerido')
            if snapshot.get('switches') is not True:raise RuntimeError('Ruta real incorrecta')
            for field in ('ret','trace'):
                if field not in snapshot:raise RuntimeError('Snapshot incompleto '+field)
            if not isinstance(snapshot['trace'],list):raise RuntimeError('Trace incorrecta')
        if row['original']!=row['candidate']:raise RuntimeError('Diferencia nested')
    return value
