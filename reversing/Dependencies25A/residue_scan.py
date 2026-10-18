import sys,struct,capstone
from capstone.x86 import *
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
secs=[]
o=pe+24+opt
for i in range(nsec):
    vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8+i*40); secs.append((va,vs,rp,rs))
def off(a):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
FULL={'eax':('eax','ax','al','ah'),'ecx':('ecx','cx','cl','ch'),'edx':('edx','dx','dl','dh')}
COVER={}
for k,(e,x,l,h) in FULL.items():
    COVER[e]=frozenset((e,x,l,h)); COVER[x]=frozenset((x,l,h)); COVER[l]=frozenset((l,)); COVER[h]=frozenset((h,))
def canon(r):
    n=md.reg_name(r)
    return n if n in COVER else None
def basereg(n):
    for k,v in FULL.items():
        if n in v: return k
def scan(start,limit=200):
    seen=set(); stack=[(start,frozenset(),0)]; res=set()
    while stack:
        a,written,depth=stack.pop()
        if (a,written) in seen: continue
        if depth>limit: res.add('LIMIT'); continue
        seen.add((a,written))
        o=off(a)
        if o is None: continue
        ins=next(md.disasm(exe[o:o+16],a),None)
        if ins is None: continue
        r=set();w=set()
        for op in ins.operands:
            if op.type==X86_OP_REG:
                c=canon(op.reg)
                if c:
                    if op.access&1: r.add(c)
                    if op.access&2: w.add(c)
            elif op.type==X86_OP_MEM:
                for rr in (op.mem.base,op.mem.index):
                    if rr:
                        c=canon(rr)
                        if c: r.add(c)
        m=ins.mnemonic
        if m in('xor','sub') and len(ins.operands)==2 and ins.operands[0].type==X86_OP_REG and ins.operands[1].type==X86_OP_REG and ins.operands[0].reg==ins.operands[1].reg:
            c=canon(ins.operands[0].reg); r.discard(c)
        # a read is OK if all aliasing names it needs are covered
        for n in r:
            need=COVER[n]
            cov=set()
            for x in written: cov|=COVER[x]
            if not need<=cov: res.add(basereg(n))
        if m=='call':
            cov=set()
            for x in written: cov|=COVER[x]
            un=[x for x in ('ecx','edx') if x not in cov]
            res.add('CALL@%x:unwritten=%s'%(a,','.join(un))); continue
        wc=set(written)
        for n in w: wc.add(n)
        # full writes of 32-bit regs cover; also writing eax covers ax etc. via COVER
        written2=frozenset(wc)
        if m in('ret','retn'): continue
        if m=='jmp':
            if ins.operands[0].type==X86_OP_IMM: stack.append((ins.operands[0].imm,written2,depth+1))
            else: res.add('INDIRECT')
            continue
        if m.startswith('j'):
            stack.append((ins.operands[0].imm,written2,depth+1)); stack.append((a+ins.size,written2,depth+1)); continue
        stack.append((a+ins.size,written2,depth+1))
    return res
if __name__=='__main__':
    target=int(sys.argv[1],16)
    t=[s for s in secs]
    text_off=off(0x401000); n=0x456000
    data=exe[text_off:text_off+n]
    out=[]
    for i in range(len(data)-5):
        if data[i] in (0xE8,0xE9):
            rel=struct.unpack_from('<i',data,i+1)[0]
            if (0x401000+i+5+rel)&0xffffffff==target:
                site=0x401000+i
                kind='call' if data[i]==0xE8 else 'jmp'
                if kind=='call':
                    rr=scan(site+5)
                    out.append((hex(site),kind,sorted(x for x in rr if x in('eax','ecx','edx')),sorted(x for x in rr if x not in('eax','ecx','edx') and not x.startswith('CALL@') or (x.startswith('CALL@') and not x.endswith('unwritten=')))))
                else: out.append((hex(site),kind,[],[]))
    for x in out: print(x)
