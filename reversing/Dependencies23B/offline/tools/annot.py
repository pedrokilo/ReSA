import re,sys
POP={0x542a50:0x18,0x5429f0:0xc,0x542ce0:0x10,0x6d2600:8,0x40fef0:4,0x411a00:4}
lines=[l.rstrip('\n') for l in open('fc.dis')]
ins=[]
for l in lines:
    m=re.match(r'([0-9a-f]+): (\S+)\s*(.*)',l)
    ins.append((int(m.group(1),16),m.group(2),m.group(3)))
# first pass: esp delta per address with branch-merging (assume consistent): propagate along fallthrough and jumps
n=len(ins)
idx={a:i for i,(a,_,_) in enumerate(ins)}
delta=[None]*n
# prologue: 6d85f0 sub 0x250; push x4 -> base after 6d8604 (push edi). treat delta relative to esp after prologue
work=[(idx[0x6d8604+1] if 0x6d8604+1 in idx else None,0)]
# find index of instr after 'push edi' at 6d8603
i0=[i for i,(a,_,_) in enumerate(ins) if a==0x6d8603][0]
work=[(i0+1,0)]+[(idx[t],0) for t in (0x6d8715,0x6d8b1d,0x6d986e,0x6da900)]
def adj(mn,op,d):
    if mn=='push': return d-4
    if mn=='pop': return d+4
    if mn=='sub' and op.startswith('esp, '): return d-int(op.split(', ')[1],16)
    if mn=='add' and op.startswith('esp, '): return d+int(op.split(', ')[1],16)
    if mn=='call':
        t=op.strip()
        if t.startswith('0x'): return d+POP.get(int(t,16),0)
    return d
seen={}
while work:
    i,d=work.pop()
    while i is not None and i<n:
        if delta[i] is not None:
            if delta[i]!=d: print('MISMATCH at %x: %s vs %s'%(ins[i][0],delta[i],d),file=sys.stderr)
            break
        delta[i]=d
        a,mn,op=ins[i]
        nd=adj(mn,op,d)
        if mn=='ret': break
        if mn=='jmp':
            t=op.strip()
            if t.startswith('0x'):
                ta=int(t,16)
                if ta in idx: work.append((idx[ta],nd))
            elif 'ptr' in t: pass # jump table
            break
        if mn.startswith('j') and mn!='jmp':
            ta=int(op.strip(),16)
            if ta in idx: work.append((idx[ta],nd))
        i+=1;d=nd

out=[]
for i,(a,mn,op) in enumerate(ins):
    d=delta[i]
    if d is None:
        out.append('%x: %s %s    ;?'%(a,mn,op)); continue
    def rep(m):
        x=int(m.group(2),16)
        sign=m.group(1)
        v=x if sign=='+' else -x
        return '[F+%#x]'%(v+d)
    op2=re.sub(r'\[esp (\+|-) (0x[0-9a-f]+)\]',rep,op)
    op2=re.sub(r'\[esp\]',lambda m:'[F+%#x]'%(d),op2)
    out.append('%x: %s %s'%(a,mn,op2))
open('fc.ann','w').write('\n'.join(out)+'\n')
print('done')
