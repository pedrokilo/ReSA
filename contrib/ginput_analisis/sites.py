import sys; sys.path.insert(0,'.')
from blocks import *
from names import rows,keys
import bisect,capstone,re
omd=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
def fstart(a):
    j=bisect.bisect_right(keys,a)-1
    return rows[j][0] if j>=0 and a-rows[j][0]<0x1000 else None
def instr_at(a,patched=None):
    """return list of (addr,size,text) of orig instructions covering a"""
    fs=fstart(a)
    starts=[fs] if fs else []
    for st in (starts or [a-1,a]):
        code=orb(st,a-st+0x20)
        pos=st
        for i in omd.disasm(code,st):
            if i.address<=a<i.address+i.size: return i
            if i.address>a: break
    return None
def sites(lo,hi,gap=1):
    bl,bm,first,ps=blocks(lo,hi,gap)
    out=[]
    for s,e in bl:
        i0=instr_at(s)
        st=i0.address if i0 else s
        # extend to cover e
        en=e
        i1=instr_at(e)
        if i1: en=i1.address+i1.size-1
        ob=orb(st,en-st+1+8)
        nb=bytearray(ob)
        for a,v in bm.items():
            if st<=a<st+len(nb) and v is not None: nb[a-st]=v
        out.append(dict(start=st,end=en,pstart=s,pend=e,orig=bytes(ob[:en-st+1]),new=bytes(nb[:en-st+1]),site=first.get(s)))
    return out
def dtext(b,va):
    r=[]
    for i in omd.disasm(b,va): r.append("%s %s"%(i.mnemonic,i.op_str))
    return '; '.join(r)
if __name__=='__main__':
    for d in sites(0x1000a740,0x1000d29a):
        print("%08x-%08x %s | O: %s | N: %s"%(d['start'],d['end'],name(d['start']) or '',dtext(d['orig'],d['start']),dtext(d['new'],d['start'])))
