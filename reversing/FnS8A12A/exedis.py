import sys,struct,capstone
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[]
for i in range(nsec):
    n,vs,va,rs,rp=struct.unpack_from('<8sIIII',d,so+i*40); secs.append((va,vs,rp,rs))
def rd(a,n):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): o=rp+r-va; return d[o:o+n]
a=int(sys.argv[1],16); n=int(sys.argv[2])
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for i in md.disasm(rd(a,n),a): print("%x %s %s"%(i.address,i.mnemonic,i.op_str))
