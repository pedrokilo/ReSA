import re,glob,os,json
root=r'D:\Original\wv-audit\source\game_sa\DatosExe'
decl=re.compile(r'^\s*extern\s+(.+?)\s+(\w+)((?:\[[^\]]*\])*)\s*;\s*//\s*0x([0-9A-Fa-f]+)')
objs=[];incs=[]
cur=None
for h in sorted(glob.glob(root+'/*.h')):
    ns=None
    incs.append(os.path.basename(h))
    for line in open(h,encoding='utf-8',errors='replace'):
        m=re.match(r'namespace DatosExe::(\w+)',line)
        if m: ns=m.group(1)
        m=decl.match(line)
        if m:
            t,n,arr,a=m.groups()
            objs.append(dict(ns=ns,type=t,name=n,arr=arr,addr=int(a,16),file=os.path.basename(h)))
json.dump(objs,open('objs.json','w'))
print(len(objs))
with open('sz_objs.cpp','w') as f:
    f.write('#include "StdInc.h"\n')
    for i in incs: f.write(f'#include "game_sa/DatosExe/{i}"\n')
    for i,o in enumerate(objs):
        if "[]" in o["arr"]: continue
        f.write(f'extern const unsigned long long szo_{i} = sizeof(DatosExe::{o["ns"]}::{o["name"]});\n')
