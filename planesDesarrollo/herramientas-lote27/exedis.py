import sys,struct,capstone
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
# PE sections
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
secs=[]
o=pe+24+opt
for i in range(nsec):
    vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8+i*40)
    secs.append((va,vs,rp,rs))
def off(a):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
a=int(sys.argv[1],16); n=int(sys.argv[2],16)
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for i in md.disasm(exe[off(a):off(a)+n],a):
    print("%x: %s %s"%(i.address,i.mnemonic,i.op_str))
