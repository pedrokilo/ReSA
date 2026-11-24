import json,re,glob,collections,bisect,pickle
from parse_asm import parse
idx,lines=pickle.load(open('index.pkl','rb'))
objs=json.load(open('objs2.json'));uses=json.load(open('uses.json'));tsz=json.load(open('tsizes.json'))
asz=collections.defaultdict(set)
for f in ['sz_all.asm']+glob.glob('asm/*.asm'):
    for a,s,t,l in parse(f):
        if a and s: asz[a].add(s)
WR=lambda n:re.compile(r'(\b'+re.escape(n)+r'\b(\[[^\]]*\]|\.\w+)*\s*(=(?!=)|\+=|-=|\*=|/=|\|=|&=|\^=|<<=|>>=|\+\+|--))|((\+\+|--)\s*\b'+re.escape(n)+r'\b)|(&\s*(DatosExe::\w+::)?\b'+re.escape(n)+r'\b)')
def namewrites(o):
    names=[o['name']]+o['aliases']
    w=[];r=0
    for n in names:
        pat=WR(n)
        for rel,i in idx.get(n,[]):
            c=lines[rel][i].split('//')[0]
            if rel=='game_sa/DatosExe/StaticInit.cpp': continue
            if pat.search(c): w.append((rel,i+1))
            else: r+=1
    return w,r
def width(u):
    k=u['kind'];bt=u.get('bt')
    if k in('StaticRef','ScopedStaticRef') and u['a'] in asz: return max(asz[u['a']]),'asm'
    if bt and bt in tsz: return tsz[bt],'type'
    if k.startswith('fn:'):
        n=k[3:]
        if n in('K','constant','Legible'): return 8,'fn'
    return 4,'guess'
litwrite=re.compile(r'(\)\s*(=(?!=)|\+=|-=|\*=|\|=|&=|\+\+|--))|\b(put|Store32|StoreWord|St|StoreRun|CopyValue|copyName)\s*[<(]|(\+\+|--)\s*(StaticRef|\*)')
mut=sorted([o for o in objs if o['size'] and o['addr']>=0x8A4000],key=lambda o:o['addr'])
for o in mut: o['w'],o['nr']=namewrites(o)
out=[]
st=[o['addr'] for o in mut]
for u in uses:
    if u['f'].startswith('game_sa/DatosExe/'):continue
    w,src=width(u);u['w']=w;u['wsrc']=src
    a=u['a'];i=bisect.bisect_right(st,a+w-1)-1
    while i>=0:
        o=mut[i]
        if o['addr']+o['size']<=a-70000: break
        if o['addr']<a+w and a<o['addr']+o['size']:
            out.append((u,o))
        i-=1
        if i>=0 and mut[i]['addr']<a-70000:break
rows=[]
for u,o in out:
    live=bool(o['refs'] or o['aliasrefs'])
    rows.append(dict(f=u['f'],l=u['l'],a=u['a'],w=u['w'],wsrc=u['wsrc'],kind=u['kind'],obj=o['name'],ns=o['ns'],oa=o['addr'],osz=o['size'],live=live,nwrites=len(o['w']),nreads=o['nr'],litwr=bool(litwrite.search(u['code'])),code=u['code'][:110],objrefs=[r for r in (o['refs'] or [])[:3]]))
json.dump(rows,open('rows.json','w'))
print(len(rows),sum(r['live'] for r in rows))
