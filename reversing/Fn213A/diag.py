import json,sys
sys.path.insert(0,r'D:\Original\wv-5\reversing\Fn213A')
import fixture_oracle as o
rows=[json.loads(l) for l in open(sys.argv[1]+'/comparisons.jsonl',encoding='utf-8')]
print(len(rows))
for r in rows:
    if r.get('event') in('function_summary',) and r['failures']: print(r['function'],r['failures'])
    if r.get('event') in ('error','complete'): print(r)
cmp=[r for r in rows if r.get('event')=='comparison' and not r['equal']]
print('noequal',len(cmp))
bad=[(r,o.check_row(r)) for r in rows if r.get('event')=='comparison']
bad=[(r,b) for r,b in bad if b]
print('oraculo distinto',len(bad))
seen=set()
for r in cmp+[r for r,_ in bad]:
    if r['function'] in seen: continue
    seen.add(r['function'])
    print('---',r['function'],r['case'], 'a=',r['input']['a'][:22])
    for k in ('stack_restored','nonvolatile_preserved','direction_flag_clear','seh_restored','route_ok','collaborators_dll','globals_restored','floating_state_restored'):
        if r['original'].get(k) is not True or r['candidate'].get(k) is not True: print(' gate',k,r['original'].get(k),r['candidate'].get(k))
    print(' orig',str(r['original']['observed'])[:350]); print(' cand',str(r['candidate']['observed'])[:350])
    print(' oracle',str(o.check_row(r))[:350])
