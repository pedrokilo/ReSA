import sys; sys.path.insert(0,'.')
from g import *
from names import name
def show(lo,hi=None,n=0x400):
    for i in allins():
        if i.address<lo: continue
        if hi and i.address>hi: break
        if not hi and i.address>=lo+n: break
        op=i.op_str
        import re
        def rep(m):
            v=int(m.group(0),16)
            if 0x400000<=v<0x1000000*1 and v<0x10000000:
                nm=name(v,0x600)
                return m.group(0)+('<%s>'%nm if nm else '')
            if v in IAT: return m.group(0)+'<%s>'%IAT[v]
            return m.group(0)
        op=re.sub(r'0x[0-9a-f]+',rep,op)
        print("%08x %s %s"%(i.address,i.mnemonic,op))
if __name__=='__main__':
    a=int(sys.argv[1],16); b=int(sys.argv[2],16) if len(sys.argv)>2 else None
    show(a,b)
