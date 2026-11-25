import sys; sys.path.insert(0,'.')
from g import *
import re, json
from capstone.x86 import *
def run(lo,hi,regs0=None,verbose=False):
    regs={}; slots={}; last=None
    writes=[]; calls=[]
    M=0xffffffff
    sizes={1:'b',2:'w',4:'d'}
    for i in allins():
        if i.address<lo or i.address>hi: continue
        mn=i.mnemonic; ops=i.operands
        def val(op):
            if op.type==X86_OP_IMM: return op.imm&M
            if op.type==X86_OP_REG:
                n=i.reg_name(op.reg)
                full={'ax':'eax','bx':'ebx','cx':'ecx','dx':'edx','si':'esi','di':'edi','bp':'ebp','al':'eax','bl':'ebx','cl':'ecx','dl':'edx'}.get(n,n)
                v=regs.get(full)
                if v is None: return None
                return v&(0xffff if len(n)==2 and n[1]!='l' else 0xff if len(n)==2 else M)
            if op.type==X86_OP_MEM:
                m=op.mem
                if m.base==X86_REG_ESP or m.base==X86_REG_EBP:
                    return slots.get(m.disp)
                if m.base==0 and m.index==0: return ('mem',m.disp&M)
            return None
        def setreg(op,v):
            n=i.reg_name(op.reg)
            full={'ax':'eax','bx':'ebx','cx':'ecx','dx':'edx','si':'esi','di':'edi','bp':'ebp','al':'eax','bl':'ebx','cl':'ecx','dl':'edx'}.get(n,n)
            regs[full]=v
        if mn=='mov' and len(ops)==2:
            d,s=ops
            v=val(s)
            if d.type==X86_OP_REG:
                if isinstance(v,tuple): v=None
                setreg(d,v)
            elif d.type==X86_OP_MEM:
                m=d.mem
                if m.base in(X86_REG_ESP,X86_REG_EBP): slots[m.disp]=v
                elif (m.base==0 and m.index==0) or (m.base not in(0,) and m.index==0 and isinstance(regs.get(i.reg_name(m.base)),int) and 0x400000<=regs.get(i.reg_name(m.base))<0x1000000):
                    A=((regs.get(i.reg_name(m.base)) if m.base else 0)+m.disp)&M
                    if 0x400000<=A<0x10000000:
                        writes.append((i.address,A,d.size,v))
        elif mn=='add' and len(ops)==2 and ops[0].type==X86_OP_REG:
            a=val(ops[0]); b=val(ops[1])
            setreg(ops[0],((a+b)&M) if isinstance(a,int) and isinstance(b,int) else None)
        elif mn=='sub' and len(ops)==2 and ops[0].type==X86_OP_REG:
            a=val(ops[0]); b=val(ops[1])
            setreg(ops[0],((a-b)&M) if isinstance(a,int) and isinstance(b,int) else None)
        elif mn=='xor' and len(ops)==2 and ops[0].type==X86_OP_REG and ops[1].type==X86_OP_REG and ops[0].reg==ops[1].reg:
            setreg(ops[0],0)
        elif mn=='call':
            calls.append((i.address,i.op_str))
            regs['eax']=None
    return writes,calls
if __name__=='__main__':
    w,c=run(0x1000a740,0x1000d29a)
    for a,A,sz,v in w:
        print("%08x W%d [%08x] = %s"%(a,sz,A,('%08x'%v) if isinstance(v,int) else v))
