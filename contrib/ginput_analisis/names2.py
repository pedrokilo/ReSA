import csv,bisect
R=[]
for r in csv.DictReader(open(r"D:\Original\wv-int\docs\MemoryMap\binary_function_ranges.csv")):
    R.append((int(r['start_va'],16),int(r['end_va'],16)))
R.sort(); RK=[a for a,_ in R]
CN={}
for r in csv.DictReader(open(r"D:\Original\wv-int\docs\MemoryMap\binary_functions.csv",encoding='utf8')):
    va=int(r['va'],16)
    n=r['catalogue_names'] or r['virtual_owners']
    CN[va]=n
import names as N
HN={a:n for a,n in N.rows}
def func(a):
    j=bisect.bisect_right(RK,a)-1
    if j<0: return None
    s,e=R[j]
    if a<=e+1: return s
    return None
def fname(a):
    s=func(a)
    if s is None: return None
    nm=HN.get(s) or CN.get(s) or ''
    return s,nm
def label(a):
    f=fname(a)
    if not f: return ''
    s,nm=f
    base=nm if nm else 'sub_%X'%s
    return base if a==s else '%s+0x%x'%(base,a-s)
