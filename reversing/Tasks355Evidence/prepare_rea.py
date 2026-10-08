import csv,json,re
from pathlib import Path
root=Path('reversing/_handoff/rea/rea-x86');out=Path('reversing/Tasks355Evidence')
entries=json.loads((out/'entries.json').read_text());extra=[]
for cls,stem in [('CTaskComplexGangJoinRespond','TaskComplexGangJoinRespond'),('CTaskComplexKillPedFromBoat','TaskComplexKillPedFromBoat')]:
 for n,a in re.findall(r'RH_ScopedVMTInstall\((\w+),\s*(0x[0-9a-fA-F]+)',Path('source/game_sa/Tasks/TaskTypes/'+stem+'.cpp').read_text()):extra.append({'cls':cls,'fn':n,'a':int(a,16)})
entries+=extra
ranges={int(r['function_va'],16):int(r['end_va'],16) for r in csv.DictReader(open('build/binary-map/function_ranges.csv'))}
index=json.loads(Path('reversing/_handoff/ghidra/index.json').read_text())
missing=[e for e in entries if not index.get(f"0x{e['a']:08x}",{}).get('file')]
assert all(e['a'] in ranges for e in missing),[hex(e['a']) for e in missing if e['a'] not in ranges]
bridge=(root/'tasks-bridge/ReaGhidraBridge.java').read_text()
switch='long recoveredEnd = switch ((int)address.getOffset()) { '+ ' '.join(f"case 0x{e['a']:x} -> 0x{ranges[e['a']]:x}L;" for e in missing)+' default -> 0L; };'
bridge=re.sub(r'long recoveredEnd = switch [^\n]*',switch,bridge,count=1)
dest=root/'tasks355-bridge';dest.mkdir(exist_ok=True);(dest/'ReaGhidraBridge.java').write_text(bridge)
(root/'tasks355-loader.mjs').write_text((root/'tasks-loader.mjs').read_text().replace('tasks-bridge','tasks355-bridge'))
script=(root/'dump-criminal-first-pseudo.mjs').read_text();script=script.replace("const list = ['68DC60'].map(hex=>({cls:'TasksPending',fn:'Recovered',a:parseInt(hex,16)}));",'const list = '+json.dumps(missing)+';').replace('tasks-loader.mjs','tasks355-loader.mjs')
script=script.replace("const index = JSON.parse(readFileSync(OUT + 'index.json','utf8'));",'let index;')
script=script.replace("const name = `${cls}.","index = JSON.parse(readFileSync(OUT + 'index.json','utf8'));\n    const name = `${cls}.")
script=script.replace("if (true) console.log", "writeFileSync(OUT + 'index.json', JSON.stringify(index, null, 1));\n    if (true) console.log")
local=root/'tasks355-pseudo';local.mkdir(exist_ok=True)
(local/'index.json').write_text('{}')
script=script.replace("const OUT = 'D:/Original/gta-reversed-master/reversing/_handoff/ghidra/';", "const OUT = 'D:/Original/gta-reversed-master/reversing/_handoff/rea/rea-x86/tasks355-pseudo/';")
(root/'dump-tasks355-pseudo.mjs').write_text(script)
print(len(missing),'missing pseudo entries recovered with exact ranges')
