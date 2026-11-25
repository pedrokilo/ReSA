import sys; sys.path.insert(0,'.')
from blocks import *
import capstone
omd=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
def newbytes(s,e,bm,ext=0):
    b=bytearray(orb(s-ext,e-s+1+ext+8))
    for a,v in bm.items():
        if s-ext<=a<s-ext+len(b) and v is not None: b[a-(s-ext)]=v
    return bytes(b)
def dis_s(b,va,maxn=6):
    r=[]
    for i in omd.disasm(b,va):
        r.append("%s %s"%(i.mnemonic,i.op_str))
        if len(r)>=maxn: break
    return '; '.join(r)
def report(lo,hi,gap=1):
    bl,bm,first,ps=blocks(lo,hi,gap)
    rows=[]
    for s,e in bl:
        ext=0
        # rel32 detection: block contains 4 bytes forming rel into asi
        nb=newbytes(s,e,bm,1)
        rows.append((s,e,nb,first.get(s)))
    return rows,bm
if __name__=='__main__':
    rows,bm=report(0x1000a740,0x1000d29a)
    for s,e,nb,f in rows:
        ob=orb(s-1,len(nb))
        n=e-s+1
        print("%08x-%08x @%x %s | orig: %s | new: %s"%(s,e,f,name(s) or '',dis_s(ob[1:],s),dis_s(nb[1:],s)))
