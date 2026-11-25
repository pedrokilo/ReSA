import sys,re,json; sys.path.insert(0,'.')
from g import *
from summ import padname,funcend
from capstone.x86 import *
def flow(lo):
    hi=funcend(lo)
    ins={i.address:i for i in allins() if lo<=i.address<hi}
    order=sorted(ins)
    # find Mode load
    mode_ld=[a for a in order if ins[a].mnemonic=='movzx' and padname_of(ins[a])=='Mode']
    if not mode_ld: return None
    a0=mode_ld[0]
    # expect: sub eax,0 ; je L0 ; dec eax ; jne Ldef ; fallthrough = mode1
    seq=[ins[a] for a in order if a>=a0][:6]
    L0=None
    for k,i in enumerate(seq):
        if i.mnemonic=='je' and L0 is None: L0=int(i.op_str,16); idx=k
    if L0 is None: return None
    # mode1 start = after the jne
    nxt=None
    for k in range(idx+1,len(seq)):
        if seq[k].mnemonic=='jne': nxt=seq[k+1].address if k+1<len(seq) else None; ldef=int(seq[k].op_str,16); break
    return a0,L0,nxt,ins,order
def padname_of(i):
    for op in i.operands:
        if op.type==X86_OP_MEM and op.mem.base not in(0,X86_REG_ESP,X86_REG_EBP) and op.mem.index==0:
            return padname(op.mem.disp)
    return None
def collect(start,ins,order,stop=None):
    # walk linearly following jumps (forward only), collect pad fields until ret
    seen=set(); out=[]; stack=[start]
    while stack:
        a=stack.pop()
        while a in ins and a not in seen:
            seen.add(a); i=ins[a]
            n=padname_of(i)
            if n and n!='Mode' and n!='DisableCtl': out.append(n)
            if i.mnemonic=='ret': break
            if i.mnemonic in('jmp',):
                t=int(i.op_str,16) if i.op_str.startswith('0x') else None
                if t: a=t; continue
                break
            if i.mnemonic.startswith('j') and i.op_str.startswith('0x'):
                stack.append(int(i.op_str,16))
            nx=[o for o in order if o>a]
            a=nx[0] if nx else -1
    return out
def modes(lo):
    f=flow(lo)
    if not f: return None
    a0,L0,nxt,ins,order=f
    m0=collect(L0,ins,order); m1=collect(nxt,ins,order) if nxt else []
    def dd(l):
        o=[]; [o.append(x) for x in l if x not in o]; return o
    return dd(m0),dd(m1)
if __name__=='__main__':
    for a in sys.argv[1:]: print(a,modes(int(a,16)))
