import sys,struct,capstone
d=open(sys.argv[1],'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
n=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
secs=[struct.unpack_from('<IIII',d,pe+24+so+8+i*40) for i in range(n)]
def off(r):
    for vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
callret={0x1283e390:1,0x1283e230:1,0x110c7e52:0,0x110cad64:1,0x1283e220:1}
def run(ad,ln):
    o=off(ad-0x10000000); ins={i.address:i for i in md.disasm(d[o:o+ln],ad)}
    seen={}; stack=[(ad,0)]; rets=set(); maxd=0
    while stack:
        a,dep=stack.pop()
        while a in ins:
            if seen.get(a)==dep: break
            seen[a]=dep
            i=ins[a]; m=i.mnemonic; ops=i.op_str
            if m in('fld','fldz','fld1'): dep+=1
            elif m in('fstp',) : dep-=1
            elif m in('fcomp','fucomp') : dep-=1
            elif m in('fcompp','fucompp'): dep-=2
            elif m=='fpatan': dep-=1
            elif m.endswith('p') and m.startswith('f') and m not in('fnstsw',) and m[1:] in('addp','mulp','subp','subrp','divp','divrp'): dep-=1
            elif m=='call':
                t=int(ops,16) if ops.startswith('0x') else None
                dep+=callret.get(t,0)
            maxd=max(maxd,dep)
            if dep<0: print('UNDERFLOW at',hex(a)); 
            if m=='ret': rets.add(dep); break
            if m=='jmp': a=int(ops,16); continue
            if m.startswith('j'):
                stack.append((int(ops,16),dep))
            a=i.address+i.size
    print(hex(ad),'ret depths',rets,'max',maxd)
for a in sys.argv[2:]:
    ad,ln=a.split(':'); run(int(ad,16),int(ln))
