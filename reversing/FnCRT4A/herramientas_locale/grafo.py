# Cierre del grafo de llamadas desde las raices del locale, marcando lo ya enganchado (docs/hooks.csv).
import csv,sys,collections,re
fn={}
for r in csv.DictReader(open('docs/MemoryMap/binary_functions.csv',encoding='utf-8')):
    fn[int(r['va'],16)]=(r['name'],int(r['body_bytes']),r['source'])
calls=collections.defaultdict(set)
for r in csv.DictReader(open('docs/MemoryMap/binary_calls.csv',encoding='utf-8')):
    
    if r['target_va'] and r['caller_va']: calls[int(r['caller_va'],16)].add(int(r['target_va'],16))
hooked={}
for r in csv.DictReader(open('docs/hooks.csv',encoding='utf-8')):
    for k in r:
        pass
    break
rows=list(csv.DictReader(open('docs/hooks.csv',encoding='utf-8')))
print(list(rows[0].keys()))
addrk=[k for k in rows[0] if 'addr' in k.lower() or 'dir' in k.lower()][0]
for r in rows:
    try: hooked[int(r[addrk],16)]=r
    except: pass
roots=[int(x,16) for x in sys.argv[1:]]
seen={};stack=list(roots)
while stack:
    a=stack.pop()
    if a in seen: continue
    seen[a]=1
    if a in hooked and a not in roots: continue
    for t in calls.get(a,()):
        if 0x820000<=t<0x840000: stack.append(t)
for a in sorted(seen):
    n=fn.get(a,('?',0,''))
    h=hooked.get(a)
    print(hex(a),n[0],n[1],'HOOK' if h else '')
