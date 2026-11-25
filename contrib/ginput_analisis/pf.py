import sys,re; sys.path.insert(0,'.')
from g import *
from summ import padname,funcend
def show(lo,hi=None):
    hi=hi or funcend(lo)
    out=[]
    for i in allins():
        if i.address<lo: continue
        if i.address>=hi: break
        op=i.op_str
        def r(m):
            reg=m.group(1); d=int(m.group(2),16)
            if reg in('esp','ebp'): return m.group(0)
            nm=padname(d)
            return '[%s.%s]'%(reg,nm) if nm else m.group(0)
        op=re.sub(r'\[(e[a-z]{2}) \+ (0x[0-9a-f]+)\]',r,op)
        out.append("%x %s %s"%(i.address&0xffff,i.mnemonic,op))
    return out
if __name__=='__main__':
    for a in sys.argv[1:]:
        print('==',a); print('\n'.join(show(int(a,16))))
