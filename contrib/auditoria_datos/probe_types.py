import json,subprocess,re
types=json.load(open('types.json'))
skip=set()
for it in range(25):
    with open('sz_types.cpp','w') as f:
        f.write('#include "StdInc.h"\n#include "game_sa/DatosExe/StaticInit.h"\n')
        for i,t in enumerate(types):
            if i in skip or t=='void': f.write('\n'); continue
            f.write(f'extern const unsigned long long szt_{i} = sizeof({t});\n')
    out=subprocess.run(['cmd','/c','.\cc.cmd sz_types.cpp'],capture_output=True,text=True,encoding='cp850',errors='replace').stdout
    bad={int(m.group(1))-3 for m in re.finditer(r'sz_types\.cpp\((\d+)\)',out)}
    print(it,len(bad))
    if not bad: break
    skip|=bad
asm=open('sz_types.asm',encoding='cp850',errors='replace').read()
res={}
for m in re.finditer(r'\?szt_(\d+)@@3_KB DQ\s+([0-9A-F]+)H',asm):
    res[types[int(m.group(1))]]=int(m.group(2),16)
json.dump(res,open('tsizes.json','w'))
print(len(res),'de',len(types),'sin tamano:',[types[i] for i in sorted(skip)][:40])
