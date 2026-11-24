import json,re,glob,os,collections,pickle
root=r'D:\Original\wv-audit\source'
B=chr(92)
idx=collections.defaultdict(list)
ident=re.compile(r'[A-Za-z_]\w*')
lines={}
for p in glob.glob(root+'/**/*',recursive=True):
    if not p.endswith(('.cpp','.h','.inl','.hpp')):continue
    rel=os.path.relpath(p,root).replace(B,'/')
    if rel.startswith('game_sa/DatosExe/') and not rel.endswith('StaticInit.cpp'): 
        # keep nothing (definitions)
        continue
    L=open(p,encoding='utf-8',errors='replace').read().split('\n')
    lines[rel]=L
    for i,l in enumerate(L):
        c=l.split('//')[0]
        for m in set(ident.findall(c)):
            idx[m].append((rel,i))
pickle.dump((dict(idx),lines),open('index.pkl','wb'))
print(len(idx))
