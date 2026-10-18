import sys,struct,capstone
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
o=pe+24+opt
vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8)
code=exe[rp:rp+rs]
targets=[int(x,16) for x in sys.argv[1:]]
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
md.skipdata=True
pats=[struct.pack('<I',t) for t in targets]
# fast scan for the 4-byte pattern then disassemble around
import re
for t,p in zip(targets,pats):
    print('== refs to',hex(t))
    for m in re.finditer(re.escape(p),code):
        i=m.start()
        # try to find instruction start: disassemble from i-6.. i+4 candidates
        for back in range(1,8):
            s=i-back
            ins=list(md.disasm(code[s:s+12],base+va+s))
            if ins and ins[0].address+ins[0].size>base+va+i and ('%x'%t) in ins[0].op_str:
                print('%x: %s %s'%(ins[0].address,ins[0].mnemonic,ins[0].op_str)); break
