import json,sys,os,glob
d=sys.argv[1]; per=int(sys.argv[2]) if len(sys.argv)>2 else 1
seen={}
for l in open(os.path.join(d,'comparisons.jsonl'),encoding='utf-8'):
    r=json.loads(l)
    if r.get('event') not in ('comparison','function_summary'): print(str(r)[:1500]); continue
    if r.get('event')!='comparison' or r['equal'] or not r.get('original'): continue
    fn=r['function']
    if seen.get(fn,0)>=per: continue
    seen[fn]=seen.get(fn,0)+1
    print('==',fn,r['case'],r['differs'])
    o,c=r['original'],r['candidate']
    for k in r['differs']:
        a,b=o.get(k),c.get(k)
        if isinstance(a,list) and isinstance(b,list) and len(a)==len(b):
            dd=[(i,a[i],b[i]) for i in range(len(a)) if a[i]!=b[i]]
            print(' ',k,'len',len(a),'ndiff',len(dd),str(dd[:8])[:1500])
        else:
            print(' ',k,str(a)[:900]); print('  ->',str(b)[:900])
