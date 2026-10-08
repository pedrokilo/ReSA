import json
from pathlib import Path
r=json.loads(Path('reversing/VehicleAudio355Evidence/big-pseudo/index.json').read_text());e=json.loads(next(iter(r.values()))['error']);d=e['details']['diagnostics']
for k in ['ghidra_log_path','script_log_path']:
 p=Path(d[k].replace('\\\\','\\'));print(k,p.exists());
 if p.exists():Path('reversing/VehicleAudio355Evidence/big-'+k+'.txt').write_bytes(p.read_bytes())
