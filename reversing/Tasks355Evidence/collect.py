import json,re,subprocess
from pathlib import Path
classes=['CTaskComplexGangJoinRespond','CTaskComplexKillPedFromBoat','CTaskGoToVehicleAndLean','CTaskComplexFollowLeaderInFormation','CTaskComplexPlayHandSignalAnim','CTaskComplexSunbathe','CTaskComplexWanderStandard','CTaskGangHassleVehicle','CTaskComplexGangFollower','CTaskComplexHitPedWithCar','CTaskComplexSmartFleePoint','CPedGeometryAnalyser']
rows=[]
for cls in classes:
 stem=cls[1:] if cls.startswith('C') else cls
 files=list(Path('source/game_sa').rglob(stem+'.cpp'));s=files[0].read_text(encoding='utf-8-sig')
 for line in s.splitlines():
  if 'RH_Scoped' not in line or 'reversed = false' not in line and 'reversed=false' not in line:continue
  m=re.search(r'Install\((\w+),\s*(?:"([^"]+)",\s*)?(0x[0-9A-Fa-f]+)',line)
  if not m:continue
  fn=m[1]+('('+m[2]+')' if m[2] else '');a=int(m[3],16);p=Path('reversing/Tasks355Evidence')/f'{cls}_{a:06X}.txt'
  rows.append({'cls':cls,'fn':fn,'a':a,'evidence':str(p)})
  with p.open('wb') as out:subprocess.run(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/combo.sh',hex(a)],stdout=out,check=True)
  print(cls,fn,hex(a),flush=True)
Path('reversing/Tasks355Evidence/entries.json').write_text(json.dumps(rows,indent=2))
