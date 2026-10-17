import struct, capstone
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
secs=[];o=pe+24+opt
for i in range(nsec):
    vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8+i*40);secs.append((va,vs,rp,rs))
def off(a):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
START,LEN=0x4A41E0,0x271
code=exe[off(START):off(START)+LEN]
data={0x858cdc:'K001',0x85a7d4:'K64',0x858624:'K1',0x858b50:'K0',0xc812f0:'WIND',0xc81324:'RAIN',0xa9ae80:'POOL'}
calls={0x4a4960:'PEI',0x4a9440:'CREATE',0x4a9460:'DESTROY',0x4a9dc0:'GETRW',0x7f18a0:'UPDATE',0x7f18b0:'MULT',0x4ab270:'CTOR',0x4a2580:'CP',0x821b40:'FTOL'}
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
patches=[]
for i in md.disasm(code,START):
    if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM:
        t=i.operands[0].imm; assert t in calls,hex(t); patches.append((i.address-START+1,'call',calls[t]))
        continue
    for op in i.operands:
        if op.type==capstone.x86.X86_OP_MEM and op.mem.base==0 and op.mem.index==0 and op.mem.disp in data:
            patches.append((i.address-START+i.disp_offset,'abs',data[op.mem.disp]))
        if op.type==capstone.x86.X86_OP_IMM and op.imm in data and i.mnemonic=='mov':
            patches.append((i.address-START+i.imm_offset,'abs',data[op.imm]))
out='static const unsigned char kCP[]={%s};\n'%','.join('0x%02x'%b for b in code)
out+='struct Patch{unsigned off;int kind;const char* sym;};\nstatic const Patch kPatches[]={%s};\n'%','.join('{%d,%d,"%s"}'%(a,0 if k=='abs' else 1,s) for a,k,s in patches)
f=off(0x821B40);out+='static const unsigned char kFtol[]={%s};\n'%','.join('0x%02x'%b for b in exe[f:f+0x80])
open('exebytes.inc','w').write(out)
print(len(patches),'parches'); print(sorted(set(s for _,_,s in patches)))
