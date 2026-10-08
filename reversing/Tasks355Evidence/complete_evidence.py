import csv,json,subprocess
from pathlib import Path
folder=Path('reversing/Tasks355Evidence')
entries=json.loads((folder/'entries.json').read_text())
ranges={int(r['function_va'],16):int(r['end_va'],16) for r in csv.DictReader(open('build/binary-map/function_ranges.csv'))}
local=Path('reversing/_handoff/rea/rea-x86/tasks355-pseudo')
index=json.loads((local/'index.json').read_text())
for e in entries:
 a=e['a']; p=Path(e['evidence']);s=p.read_text().split('\n==== COMPLETE CANONICAL EVIDENCE ====')[0]
 extra='\n==== COMPLETE CANONICAL EVIDENCE ====\n'
 if index.get(f'0x{a:08x}',{}).get('file'):extra+=(local/index[f'0x{a:08x}']['file']).read_text()+'\n'
 if a not in ranges:raise RuntimeError(hex(a))
 extra+=f'==== Full ASM [0x{a:X}, 0x{ranges[a]:X}) ====\n'
 extra+=subprocess.check_output(['C:/Program Files/Git/bin/bash.exe','reversing/Tasks355Evidence/dis_complete.sh',hex(a),hex(ranges[a])],text=True)
 p.write_text(s+extra)
print('Complete canonical range appended:',len(entries))
