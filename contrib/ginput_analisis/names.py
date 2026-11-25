import csv,bisect
rows=[]
for fn in (r"D:\Original\wv-int\docs\hooks.csv",r"D:\Original\wv-int\docs\hooks_notsa.csv"):
    for r in csv.DictReader(open(fn,encoding='utf8')):
        try: rows.append((int(r['address'],16),r['class']+'::'+r['fn_name']))
        except: pass
rows.sort()
keys=[a for a,_ in rows]
def name(a,tol=0x300):
    j=bisect.bisect_right(keys,a)-1
    if j<0: return None
    d=a-rows[j][0]
    if d==0: return rows[j][1]
    if d<tol: return "%s+0x%x"%(rows[j][1],d)
    return None
