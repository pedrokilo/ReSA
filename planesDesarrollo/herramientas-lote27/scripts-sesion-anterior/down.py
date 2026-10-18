import struct,sys,capstone
S=r"C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad"
d=open(S+'/probe.asi','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
n=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
secs=[struct.unpack_from('<IIII',d,pe+24+so+8+i*40) for i in range(n)]
def off(a):
    r=a-0x10000000
    for vs,v,rs,rp in secs:
        if v<=r<v+max(vs,rs): return rp+r-v
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
WATSON={0x11060250,0x130410c0,0x1108a2f5}  # probe ILT and body
def fn_targets(start):
    o=off(start)
    if o is None: return None
    seen=set(); stack=[start]; tg=set(); watson=False
    while stack:
        a=stack.pop()
        while a not in seen:
            seen.add(a); o=off(a)
            if o is None: break
            ins=list(md.disasm(d[o:o+16],a,1))
            if not ins: break
            i=ins[0]
            if i.mnemonic in('call','jmp') and i.op_str.startswith('0x'):
                t=int(i.op_str,16)
                if i.mnemonic=='call' or True: tg.add((t,i.mnemonic))
                if i.mnemonic=='jmp':
                    # thunk-like jump out of function?: treat as tail; also local jumps
                    if abs(t-start)<0x2000: stack.append(t)
                    break
            elif i.mnemonic.startswith('j') and i.op_str.startswith('0x'):
                stack.append(int(i.op_str,16))
            if i.mnemonic in('ret','int3') : break
            if i.mnemonic=='int' and i.op_str=='0x29': break
            a+=i.size
    return tg
root=int(sys.argv[1],16)
seen={root}; q=[(root,[hex(root)])]; hits=[]
while q:
    f,path=q.pop(0)
    tg=fn_targets(f)
    if tg is None: continue
    for t,m in tg:
        if t in WATSON: hits.append(path+[hex(t)]); continue
        if t not in seen and off(t) is not None and len(path)<12:
            seen.add(t); q.append((t,path+[hex(t)]))
print(len(seen),'functions reached'); 
for h in hits: print(' -> '.join(h))
