import csv,json,re,shutil
from pathlib import Path
r=Path('reversing/_handoff/rea/rea-x86')
idx=json.loads(Path('reversing/_handoff/ghidra/index.json').read_text())
requested={'CreateInstance':0x4A2B40,'Update':0x4A2BC0,'Load':0x5C25F0,'Render':0x4A2C40,'FreePrt':0x4A2510}
items=[dict(cls='FxEmitterBP_c',fn=fn,a=a) for fn,a in requested.items() if not idx.get(f'0x{a:08x}',{}).get('file')]
ranges={int(x['function_va'],16):int(x['end_va'],16) for x in csv.DictReader(open('build/binary-map/function_ranges.csv'))}
dest=r/'tasks355-fx-bridge';dest.mkdir(exist_ok=True)
java=(r/'tasks355-bridge/ReaGhidraBridge.java').read_text()
switch='long recoveredEnd = switch ((int)address.getOffset()) { '+' '.join(f"case 0x{x['a']:x} -> 0x{ranges[x['a']]:x}L;" for x in items)+' default -> 0L; };'
java=re.sub(r'long recoveredEnd = switch [^\n]*',switch,java,count=1)
(dest/'ReaGhidraBridge.java').write_text(java)
for x in ['ReaGhidraExtensions.java','ReaGhidraPrepareCom.java']:shutil.copy2(r/'tasks355-bridge'/x,dest/x)
(r/'tasks355-fx-loader.mjs').write_text((r/'tasks355-loader.mjs').read_text().replace('tasks355-bridge','tasks355-fx-bridge'))
script=(r/'dump-tasks355-pseudo.mjs').read_text().replace('tasks355-pseudo/','tasks355-fx-pseudo/').replace('tasks355-loader.mjs','tasks355-fx-loader.mjs')
script=re.sub(r'const list = [^\n]*','const list = '+json.dumps(items)+';',script,count=1)
out=r/'tasks355-fx-pseudo';out.mkdir(exist_ok=True);(out/'index.json').write_text('{}')
(r/'dump-tasks355-fx-pseudo.mjs').write_text(script)
print(len(items), 'missing Fx entries')
