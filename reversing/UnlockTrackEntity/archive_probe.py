import json,re,shutil,hashlib,sys
from pathlib import Path
root=Path(__file__).resolve().parents[2];bank=root/'reversing/UnlockTrackEntity'
m=json.loads((bank/'artifacts/build-manifest.json').read_text())
for f,h in {**m['sources'],**m['dependencies']}.items():
 assert hashlib.sha256((root/f).read_bytes()).hexdigest()==h,f
f=bank/'run_local.py'
f.write_text(re.sub("PROBE_HASH='[^']+'","PROBE_HASH='"+m['binaries']['asi']+"'",f.read_text()))
for ext in ('asi','pdb'):
 s=bank/f'artifacts/gta_reversed-unlock-track-entity-probe.{ext}'
 assert hashlib.sha256(s.read_bytes()).hexdigest()==m['binaries'][ext]
 shutil.copy2(s,Path(r'D:\JUEGOS\GTA San Andreas - CVehicle Test\_baseline')/s.name)
