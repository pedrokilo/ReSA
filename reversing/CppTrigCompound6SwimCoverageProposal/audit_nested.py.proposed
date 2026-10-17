import json
import struct
from pathlib import Path
from probe_case import analyzer
EXPECTED = {'CTaskSimpleSwim::ProcessSwimmingResistance': 1024}
def analyze(path):
    value=analyzer(EXPECTED)(path)
    if value is None:return None
    rows=[json.loads(line) for line in Path(path).read_text(encoding='utf-8').splitlines() if line]
    coverage={"original":0,"candidate":0}
    covered_cases={"original":[],"candidate":[]}
    for row in rows:
        if row.get('event')!='comparison':continue
        for key in ('original','candidate'):
            snapshot=row.get(key)
            if not isinstance(snapshot,dict):raise RuntimeError('Snapshot completo requerido')
            if snapshot.get('switches') is not True:raise RuntimeError('Ruta real incorrecta')
            if snapshot.get('clean') is not True:raise RuntimeError('Recursos no restaurados')
            for field in ('ret','trace'):
                if field not in snapshot:raise RuntimeError('Snapshot incompleto '+field)
            if not isinstance(snapshot['trace'],list):raise RuntimeError('Trace incorrecta')
        fixture_original=row.get('fixture_original')
        fixture_candidate=row.get('fixture_candidate')
        if not isinstance(fixture_original,dict) or fixture_original!=fixture_candidate:
            raise RuntimeError('Fixture previo ausente o diferente entre lados')
        for key in ('original','candidate'):
            fixture=row['fixture_'+key]
            state=fixture.get('swim_state')
            if type(state) is not int or not 0<=state<=65535:
                raise RuntimeError('Estado previo invalido')
            decoded={}
            for field in ('rotation_x_bits','state_changer_bits'):
                bits=fixture.get(field)
                if type(bits) is not int or not 0<=bits<=0xFFFFFFFF:
                    raise RuntimeError('Rawbits previos invalidos '+field)
                decoded[field]=struct.unpack('<f',struct.pack('<I',bits))[0]
            water=[event for event in row[key]['trace'] if isinstance(event,list) and event and event[0]=='water_level']
            for event in water:
                if len(event)!=8 or type(event[6]) is not bool:
                    raise RuntimeError('Contrato de water_level invalido')
            # The target performs exactly one water query. State 4 does not change
            # before it; negative rotation excludes the preceding positive branch.
            if state==4:
                if len(water)!=1:raise RuntimeError('Consulta water de estado 4 ausente o multiple')
                if not (decoded['state_changer_bits']<0.0) and decoded['rotation_x_bits']<0.0 and water[0][6] is True:
                    coverage[key]+=1
                    covered_cases[key].append(row['case'])
        if row['original']!=row['candidate']:raise RuntimeError('Diferencia nested')
    if min(coverage.values())<1 or covered_cases['original']!=covered_cases['candidate']:
        raise RuntimeError('SubSin sin cobertura demostrada en ambos lados del mismo fixture')
    value['observed_coverage']={'subsin_pairs':coverage,'cases':covered_cases}
    return value
