import sys
sys.path.insert(0,'.')
from desens import *
a=int(sys.argv[1],16); e=int(sys.argv[2],16)
code=rd(a,e-a)
out=[];prev=None
for i in md.disasm(code,a):
    out.append(i)
starts=[]
for k,i in enumerate(out):
    if i.mnemonic in('int3',): continue
    if k==0 or out[k-1].mnemonic in('int3','nop') and not (i.mnemonic in('nop',)): starts.append(i.address)
print([hex(s) for s in starts])
