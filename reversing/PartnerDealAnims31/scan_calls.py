import struct,sys,re
from capstone import *
from capstone.x86 import *
d=open(r'D:/Grand Theft Auto San Andreas/gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
for i in range(nsec):
    o=pe+24+opt+i*40
    name=d[o:o+8].rstrip(b'\0')
    vs,va,rs,ra=struct.unpack_from('<IIII',d,o+8)
    if name==b'.text': tva,tvs,tra=va+base,vs,ra;break
code=d[tra:tra+tvs]
md=Cs(CS_ARCH_X86,CS_MODE_32); md.detail=True; md.skipdata=True
ins=list(md.disasm(code,tva))
disp=int(sys.argv[1],16)
CE,DE=X86_REG_ECX,X86_REG_EDX
def sub(r):
    names={X86_REG_CL:CE,X86_REG_CX:CE,X86_REG_ECX:CE,X86_REG_DL:DE,X86_REG_DX:DE,X86_REG_EDX:DE}
    return names.get(r)
for i,x in enumerate(ins):
    if x.mnemonic=='call' and len(x.operands)==1 and x.operands[0].type==X86_OP_MEM and x.operands[0].mem.disp==disp and x.operands[0].mem.base!=0 and x.operands[0].mem.index==0:
        live={CE,DE}; res=[]
        for y in ins[i+1:i+25]:
            if y.mnemonic in ('ret','jmp','call') or y.mnemonic.startswith('j'):
                # reads of ecx/edx by call: unknown (thiscall) -> flag
                if y.mnemonic=='call':
                    res.append('call@%x'%y.address); 
                break
            try:
                rr,ww=y.regs_access()
            except Exception: break
            for r in rr:
                s=sub(r)
                if s in live: res.append('READ %s at %x %s %s'%('ecx' if s==CE else 'edx',y.address,y.mnemonic,y.op_str))
            for r in ww:
                s=sub(r)
                if s in live and r in (X86_REG_ECX,X86_REG_EDX): live.discard(s)
            if not live: break
        if res: print('%08x'%x.address,res)
