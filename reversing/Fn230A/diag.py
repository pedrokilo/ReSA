"""Diagnostico de una corrida de Fn230A: python -I diag.py <comparisons.jsonl o fn230a-runtime.jsonl>. Lista por funcion los pares EXE!=DLL, con compuertas fallidas o fuera del oraculo."""
import json,sys
sys.path.insert(0,'D:/Original/wv-5/reversing/Fn230A')
import fixture_oracle as o
d=sys.argv[1]
rows=[json.loads(l) for l in open(d,encoding='utf-8')]
cmp_=[r for r in rows if r.get('event')=='comparison']
print('filas',len(rows),'comparaciones',len(cmp_))
if cmp_: print('ultima', cmp_[-1]['function'], cmp_[-1]['case'])
for r in rows:
    if r.get('event')=='function_summary' and r['failures']: print('FALLOS', r['function'], r['failures'])
bad=0
GATES=('stack_restored','nonvolatile_preserved','direction_flag_clear','seh_restored','route_ok','collaborators_dll','globals_restored','floating_state_restored')
seen={}
for r in cmp_:
    why=None
    if r['original']!=r['candidate']: why='EXE!=DLL'
    else:
        for sd in ('original','candidate'):
            bg=[k for k in GATES if r[sd].get(k) is not True]
            if bg: why='gate '+str(bg)
        if not why:
            try: w=o.check_row(r)
            except o.Bad as ex: w=('bad',str(ex))
            if w: why='ORACLE '+w[0]+' '+str(w[1])[:250]
    if why:
        bad+=1
        seen.setdefault(r['function'],[]).append((r['case'],why))
for f,l in seen.items():
    print(f, len(l), l[:2])
print('total malos',bad)
