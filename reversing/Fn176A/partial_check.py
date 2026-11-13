import json,sys,collections
sys.path.insert(0,'.')
import fixture_oracle as o
path=sys.argv[1]
cnt=collections.Counter(); bad=collections.Counter(); first={}
GATES=('stack_restored','nonvolatile_preserved','direction_flag_clear','seh_restored','route_ok','collaborators_dll','globals_restored','floating_state_restored')
for line in open(path,encoding='utf-8'):
    if '"event":"comparison"' not in line: continue
    r=json.loads(line); fn=r['function']; cnt[fn]+=1
    why=None
    for s in ('original','candidate'):
        g=[k for k in GATES if r[s].get(k) is not True]
        if g: why=(s,'gate',g,r[s].get('collab_why')); break
    if not why and r['original']!=r['candidate']: why=('diff',)
    if not why:
        b=o.check_row(r)
        if b: why=('oracle',)+b
    if why:
        bad[fn]+=1
        if fn not in first: first[fn]=(r['case'],str(why)[:700])
for fn in o.NAMES:
    print(fn,cnt[fn],bad[fn],first.get(fn,''))
