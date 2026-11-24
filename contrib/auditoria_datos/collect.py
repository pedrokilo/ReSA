import re,glob,os,json,collections
root=r'D:\Original\wv-audit\source'
LO,HI=0x858000,0xCB0FFF
lit=re.compile(r'0x([0-9A-Fa-f]{6,8})\b')
def balanced(s,i):
    # s[i]=='<' ; return index after matching '>'
    d=0
    for j in range(i,len(s)):
        if s[j]=='<': d+=1
        elif s[j]=='>':
            d-=1
            if d==0: return j+1
    return -1
uses=[]
tmpl=re.compile(r'\b(StaticRef|ScopedStaticRef|Ld|St|Raw|LdP|LdS|S20ModuleField|EngineModuleSlot|reinterpret_cast|static_cast|SlotFn|word|readFloat)\s*<')
for p in glob.glob(root+'/**/*',recursive=True):
    if not p.endswith(('.cpp','.h','.inl','.hpp')):continue
    rel=os.path.relpath(p,root).replace(chr(92),'/')
    for ln,line in enumerate(open(p,encoding='utf-8',errors='replace'),1):
        code=re.sub(r'//.*','',line)
        for m in lit.finditer(code):
            a=int(m.group(1),16)
            if not(LO<=a<=HI):continue
            pre=code[:m.start()]
            typ=None;kind='lit'
            # find the nearest template call whose '(' directly precedes
            mm=re.search(r'(\w+)\s*<(.*)>\s*\(\s*$',pre)
            if mm:
                kind=mm.group(1); inner=mm.group(2)
                # inner may be 'T' or 'T*' ; rebuild balanced: search start of the template
                for t in tmpl.finditer(pre):
                    st=t.end()-1; en=balanced(pre,st)
                    if en!=-1 and pre[en:].strip()=='(':
                        typ=pre[st+1:en-1].strip(); kind=t.group(1)
            else:
                mm=re.search(r'\(\s*([\w:\s<>,\*]+?\*)\s*\)\s*$',pre)
                if mm: kind='ccast';typ=mm.group(1).strip()
                else:
                    mm=re.search(r'(\w+)\s*\(\s*$',pre)
                    if mm: kind='fn:'+mm.group(1)
            uses.append(dict(f=rel,l=ln,a=a,kind=kind,type=typ,code=code.strip()[:160],decl=bool(re.search(r'\bauto&\s+\w+\s*=|static inline',code))))
json.dump(uses,open('uses.json','w'))
print(len(uses))
c=collections.Counter(u['kind'] for u in uses);print(c.most_common(30))
