import sys, csv, collections, re, json
sys.path.insert(0,'.')
import fuente_lib as F
OBJ=sys.argv[1] if len(sys.argv)>1 else "C:/Users/pedro/AppData/Local/Temp/claude/d263413e7afce380/5b931a4e-3c0c-42eb-b692-eb62def99cf4/scratchpad/objs"
src = F.load(OBJ)
exe = {int(r['va'],16): r for r in csv.DictReader(open('exe_funcs.csv', encoding='utf-8'))}
for va,r in exe.items():
    r['size']=int(r['size']); r['calls_l']=[int(x,16) for x in r['calls'].split(';') if x]
    r['strset']=frozenset(s.strip() for s in r['strings'].split('|') if s) if r['strings'] else frozenset()
# llamadas internas (ordenadas por primera aparicion se pierde: calls es set ordenado) -> reconstruimos orden por direccion
import exe_funcs as EF
fn = EF.discover()
for va,r in exe.items():
    seen,calls = fn[va]
    order=[]
    for a,t in sorted(calls):
        if t in exe and t not in order: order.append(t)
    r['int_calls']=order
skeys=list(src)
byname=collections.defaultdict(list)
for k in skeys: byname[k[1]].append(k)
for k,f in src.items():
    order=[]
    for c in f['calls']:
        if c in byname and byname[c][0] not in order and byname[c][0]!=k: order.append(byname[c][0])
    f['int_calls']=order; f['strset']=frozenset(s.strip() for s in f['strings'] if s.strip())
# 1. votos por cadenas compartidas
nE=collections.Counter(); nS=collections.Counter()
for r in exe.values():
    for x in r['strset']: nE[x]+=1
for f in src.values():
    for x in f['strset']: nS[x]+=1
match={}; sm={}
def ok_size(e,s):
    a=exe[e]['size']; b=src[s]['size']; return 0.4*b<=a<=2.5*b+20
def put(e,s,m):
    if e in match or s in sm: return False
    match[e]=(s,m); sm[s]=e; return True
def score(e,s):
    return sum(1/(nE[x]*nS[x]) for x in exe[e]['strset']&src[s]['strset'])
freq=nS
cand=[]
for e,r in exe.items():
    if not r['strset']: continue
    for s,f in src.items():
        if f['strset'] and ok_size(e,s):
            sc=score(e,s)
            if sc>0: cand.append((sc,e,s))
cand.sort(reverse=True)
# aceptar solo si es el mejor para ambos
bestE={}; bestS={}
for sc,e,s in cand:
    bestE.setdefault(e,(sc,s)); bestS.setdefault(s,(sc,e))
for sc,e,s in cand:
    if bestE[e][1]==s and bestS[s][1]==e and sc>=0.5 and exe[e]['strset']<=src[s]['strset']|frozenset():
        put(e,s,'cadenas-exactas')
for sc,e,s in cand:
    if e not in match and s not in sm and bestE[e][1]==s and bestS[s][1]==e and sc>=0.34:
        put(e,s,'cadenas-parciales')
print('semillas',len(match))
changed=True
while changed:
    changed=False
    # por grafo
    for e,(s,_) in list(match.items()):
        ec=[x for x in exe[e]['int_calls']]; sc=src[s]['int_calls']
        eu=[x for x in ec if x not in match]; su=[x for x in sc if x not in sm]
        # callees ya emparejados deben coincidir (sanidad no impuesta)
        if len(eu)==1 and len(su)==1 and ok_size(eu[0],su[0]):
            if put(eu[0],su[0],'grafo-llamada-unica'): changed=True
        elif len(eu)==len(su) and len(eu)>1:
            if all(ok_size(a,b) for a,b in zip(eu,su)):
                for a,b in zip(eu,su):
                    if put(a,b,'grafo-orden'): changed=True
    # llamadores
    ecallers=collections.defaultdict(list); scallers=collections.defaultdict(list)
    for e,r in exe.items():
        for c in r['int_calls']: ecallers[c].append(e)
    for s,f in src.items():
        for c in f['int_calls']: scallers[c].append(s)
    for e,r in exe.items():
        if e in match: continue
        for ce in ecallers[e]:
            if ce in match:
                s0=match[ce][0]
                su=[x for x in src[s0]['int_calls'] if x not in sm]
                eu=[x for x in exe[ce]['int_calls'] if x not in match]
                # unico
                if e in eu and len(eu)==len(su) and eu.index(e)<len(su) and ok_size(e,su[eu.index(e)]) and len(eu)==1:
                    if put(e,su[0],'grafo-llamada-unica'): changed=True
print('emparejadas',len(match),'de',len(exe))
json.dump({hex(e):[s[1],s[0],m] for e,(s,m) in match.items()},open('match_tmp.json','w'),indent=1)
cnt=collections.Counter(m for _,m in match.values()); print(cnt)
known=set(int(x,16) for x in re.findall(r'0x[0-9a-f]+',open('exe_funcs.csv').read()) )
