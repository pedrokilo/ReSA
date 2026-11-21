import sys,csv,bisect,os,re
sys.path.insert(0,os.path.dirname(os.path.abspath(__file__)))
from desens import dis
root=os.path.abspath(os.path.join(os.path.dirname(__file__),'..','..','..'))
rows=list(csv.DictReader(open(os.path.join(root,'docs/MemoryMap/binary_functions.csv'),encoding='utf-8')))
vas=sorted(int(r['va'],16) for r in rows); nm={int(r['va'],16):r['name'] for r in rows}
hk=set()
for r in csv.DictReader(open(os.path.join(root,'docs/hooks.csv'),encoding='utf-8')):
    try: hk.add(int(r['address'],16))
    except: pass
pat=re.compile(r'0x8e2f14|0x825456|0x8e37d0|0x8e37cc|0xc9af00|0x8e2ec0')
for i,a in enumerate(vas):
    if not (0x820000<=a<0x840000): continue
    e=vas[i+1] if i+1<len(vas) else a+256
    t=dis(a,e-a)
    hit=[l for l in t if pat.search(l)]
    if hit: print(hex(a),nm[a],e-a,'HOOK' if a in hk else '',len(hit),hit[0][:60])
