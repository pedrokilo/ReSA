import json,subprocess,pathlib
classes={'CCarEnterExit','CWeapon','CProjectileInfo','CGenericGameStorage','CRunningScript','CCamera','CCam','InteriorGroup_c'}
for f in json.load(open('reversing/Pending355/scope.json'))['functions']:
 if f['class'] not in classes: continue
 p=pathlib.Path('reversing/World355Evidence')/(f['class']+'_'+f['fn_name']+'.txt')
 with p.open('w',encoding='utf8') as out:
  subprocess.run(['C:/Program Files/Git/bin/bash.exe','reversing/_handoff/combo.sh',f['address']],stdout=out,stderr=subprocess.STDOUT)
 print(f['class'],f['fn_name'],flush=True)
