from pathlib import Path
import re,json
banks=['MonsterControl355','MonsterSetup355','PlaneAmbient355','ScriptLinks355','ScriptPreload355','TrainLinks355','TrainCoordinates355','RadioSelection355','RadioQueue355','BikeCore355','BikeWheel355','RadioDJ355','TrainQueries355','TrainNearest355','ScriptLoaded355','PlaneControl355','MonsterSuspension355','TrainRemove355','ScriptBank355','ScriptUpdate355','RadioVolumes355','TrainSkip355','BikeRatios355','TrainInit355','TrainRead355','TrainCtor355','TrainHit355','PlaneCreation355','RadioConcat355','TrainPosition355','RadioTrack355','RadioStop355','RadioRetune355','RadioService355','BikeBurst355','TrainPassenger355','TrainGeneration355','TrainCreate355','TrainAddPassenger355','BikeRoad355','BikeHands355','BikeSetup355','BikeBlow355','BikeDamage355','BikeInputs355','PlaneBlow355','PlaneDamage355','PlaneInputs355','PlaneGeneration355','PlaneFlying355','BikeControl355']
rows=[]
for bank in banks:
 s=Path('reversing',bank,'LiveComparison.hpp').read_text(encoding='utf-8')
 for cls,fn,a,vt,slot,cases in re.findall(r'\{"(\w+)","([^"]+)",(0x[0-9A-F]+|0),(0x[0-9A-F]+|0),(-?\d+),(\d+)\}',s):
  rows.append(dict(class_name=cls,function=fn,address=a,vtable=vt,slot=int(slot),pairs=int(cases),bank=bank,status='approved' if bank in ('ScriptLinks355','RadioSelection355') else 'pending_runtime',report='20261007-205240-078573' if bank=='ScriptLinks355' else '20261007-215201-680039' if bank=='RadioSelection355' else None))
approved_train={'TrainLinks355','TrainCoordinates355','TrainQueries355','TrainRemove355','TrainSkip355','TrainInit355','TrainRead355','TrainNearest355'}
for row in rows:
 if row['bank'] in approved_train: row.update(status='approved',report=None,report_pending_coordinator=True,approval_date='2026-10-08')
approved_extra={'PlaneInputs355':'20261008-025200-936978','TrainHit355':'20261008-024817-633875','TrainGeneration355':'20261008-024856-945375','TrainCreate355':'20261008-024917-862972','MonsterControl355':'20261008-024202-737146','MonsterSetup355':'20261008-024222-120602','MonsterSuspension355':'20261008-024240-715874','RadioConcat355':'20261008-021101-541498','RadioDJ355':'20261008-020712-018463','RadioVolumes355':'20261008-020732-679230','TrainCtor355':'20261008-014327-194356','TrainPassenger355':'20261008-014435-267858'}
for row in rows:
 if row['bank'] in approved_extra: row.update(status='approved',report=approved_extra[row['bank']])
Path('reversing/VehicleAudio355Evidence/inventory-local.json').write_text(json.dumps(rows,indent=2), encoding='utf-8', newline='\n')
print(len(rows),sum(r['pairs'] for r in rows))











