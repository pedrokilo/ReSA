from pathlib import Path
import re,json
p=Path('reversing/VehicleAudio355Evidence/inventory.py');s=p.read_text(encoding='utf-8')
exec(compile(s,str(p),'exec'))
rows=json.loads(Path('reversing/VehicleAudio355Evidence/inventory-local.json').read_text(encoding='utf-8'))
for row in rows:
 cls=row['class_name']; fn=row['function']; paths=list(Path('source/game_sa').rglob(cls[1:]+'.cpp'))
 if not paths: paths=list(Path('source/game_sa').rglob(cls+'.cpp'))
 assert len(paths)==1,(cls,paths)
 source=paths[0].read_text(encoding='utf-8')
 symbol=cls+'::'+fn
 if fn=='Constructor':symbol=cls+'::'+cls
 if fn.startswith('TrainHitStuff<'):symbol='TrainHitStuff'
 m=re.search(r'[^\n]*'+re.escape(symbol)+r'\([^\n]*\)[^\n]*\{',source)
 if not m: print('MISSING',cls,fn);continue
 a=source.index('{',m.start());b=a+1;d=1
 while d:d+=(source[b]=='{')-(source[b]=='}');b+=1
 body=source[a:b];row['self_forward']=row['address'] != '0' and bool(re.search(row['address'][2:].lstrip('0'),body,re.I))
 print('FORWARD' if row['self_forward'] else 'NATIVE',cls,fn)
Path('reversing/VehicleAudio355Evidence/audit-local.json').write_text(json.dumps(rows,indent=2),encoding='utf-8',newline='\n')

