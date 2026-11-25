import sys; sys.path.insert(0,'.')
from emu import *
from orig import *
from names import name
M=0xffffffff
def patches(lo,hi):
    w,c=run(lo,hi)
    out=[]
    for ad,A,sz,v in w:
        if not isinstance(v,int): out.append((ad,A,sz,None)); continue
        out.append((ad,A,sz,v&((1<<(8*sz))-1)))
    return out
def blocks(lo,hi,gap=1):
    ps=patches(lo,hi)
    # byte map
    bm={}; first={}
    for ad,A,sz,v in ps:
        for k in range(sz):
            bm[A+k]=None if v is None else (v>>(8*k))&0xff
            first.setdefault(A+k,ad)
    addrs=sorted(bm)
    bl=[]
    for a in addrs:
        if bl and a<=bl[-1][1]+gap: bl[-1][1]=a
        else: bl.append([a,a])
    return bl,bm,first,ps
