import sys,json,re; sys.path.insert(0,'.')
from g import *
from names import name
from capstone.x86 import *
from sites import sites
import collections
exe=json.load(open(r'D:\Original\_ginput_ref\direcciones_exe.json'))
addrs={a for a,c,n in exe}
# function starts in asi: heuristic: after int3 padding
ins=allins()
fstarts=[];prev_int3=True
for i in ins:
    if prev_int3 and i.mnemonic!='int3': fstarts.append(i.address)
    prev_int3=(i.mnemonic=='int3')
import bisect
def fof(a):
    j=bisect.bisect_right(fstarts,a)-1
    return fstarts[j]
uses=collections.defaultdict(list)
for i in ins:
    vals=set()
    for op in i.operands:
        if op.type==X86_OP_IMM: vals.add(op.imm&0xffffffff)
        if op.type==X86_OP_MEM:
            vals.add(op.mem.disp&0xffffffff)
            for k in (1,2,4,8): pass
    for v in vals:
        if 0x400000<=v<0x1000000 and v<0x10000000:
            uses[v].append(i)
# also .rdata/.data dwords holding game addrs (tables)
dat=collections.defaultdict(list)
for s in pe.sections[1:3]:
    raw=data[s.PointerToRawData:s.PointerToRawData+s.SizeOfRawData]
    for k in range(0,len(raw)-3,4):
        v=struct.unpack_from('<I',raw,k)[0]
        if 0x400000<=v<0xc00000: dat[v].append(BASE+s.VirtualAddress+k)
if __name__=='__main__':
    print(len(uses),len(addrs),len(set(uses)&addrs),len(dat))
    extra=set(uses)-addrs
    print('uses not in json',len(extra),[hex(x) for x in sorted(extra)][:40])
    print('in json not in uses',[hex(x) for x in sorted(addrs-set(uses))][:40])
    print('data tables',len(dat),[ (hex(k),[hex(x) for x in v]) for k,v in list(dat.items())[:10]])
