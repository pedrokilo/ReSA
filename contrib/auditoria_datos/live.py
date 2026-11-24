import json,re,pickle,collections
idx,lines=pickle.load(open('index.pkl','rb'))
objs=json.load(open('objs.json'))
alias_re=re.compile(r'(\w+)\s*=\s*(?:DatosExe::\w+::)?(\w+)\s*;')
live={}
for o in objs:
    n=o['name']
    refs=idx.get(n,[])
    # exclude refs in comments are already excluded; collect aliases
    al=set()
    for rel,i in refs:
        c=lines[rel][i].split('//')[0]
        m=re.search(r'(?:auto\s*&|&)\s*(\w+)\s*=\s*(?:DatosExe::\w+::)?'+re.escape(n)+r'\s*;',c)
        if m: al.add(m.group(1))
    aref=[]
    for a in al: aref+=[(r,i,a) for r,i in idx.get(a,[])]
    o['refs']=[(r,i) for r,i in refs]; o['aliases']=sorted(al); o['aliasrefs']=aref
    live[n]=len(refs)+len(aref)
json.dump(objs,open('objs2.json','w'))
mut=[o for o in objs if o['addr']>=0x8A4000]
print('mutables',len(mut),'con refs por nombre:',sum(1 for o in mut if o['refs'] or o['aliasrefs']),'sin refs:',sum(1 for o in mut if not(o['refs'] or o['aliasrefs'])))
