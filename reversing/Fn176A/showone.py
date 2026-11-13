import json,sys
path,fnname,n=sys.argv[1],sys.argv[2],int(sys.argv[3])
seen=0
for line in open(path,encoding='utf-8'):
    if '"event":"comparison"' not in line: continue
    r=json.loads(line)
    if r['function']!=fnname or r['original']==r['candidate']: continue
    seen+=1
    if seen>n: break
    print('case',r['case'],'input a=',r['input']['a'][:20])
    for k in r['original']:
        if r['original'][k]!=r['candidate'][k]:
            print(' ',k,'\n   EXE:',str(r['original'][k])[:900],'\n   DLL:',str(r['candidate'][k])[:900])
