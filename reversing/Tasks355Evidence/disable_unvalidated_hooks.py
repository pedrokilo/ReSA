import json,re
from pathlib import Path
entries=json.loads(Path('reversing/Tasks355Evidence/entries.json').read_text())
changed={}
for e in entries:
 if e['cls'] in ('CTaskComplexGangJoinRespond','CTaskComplexKillPedFromBoat'):continue
 files=list(Path('source/game_sa').rglob(e['cls'][1:]+'.cpp'))
 if len(files)!=1:raise RuntimeError((e,files))
 p=files[0];s=changed.get(p,p.read_text())
 pat=r'(RH_Scoped(?:Global|VMT)?Install\('+re.escape(e['fn'])+r',\s*0x[0-9A-Fa-f]+)\);'
 s=re.sub(pat,r'\1, { .reversed = false });',s)
 changed[p]=s
for p,s in changed.items():p.write_text(s)
print('Unvalidated hooks kept disabled:',len(changed),'classes')
