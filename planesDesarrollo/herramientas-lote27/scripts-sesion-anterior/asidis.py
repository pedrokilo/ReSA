import sys,struct,capstone
d=open(sys.argv[1],"rb").read()
pe=struct.unpack_from('<I',d,0x3c)[0]
n=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
secs=[struct.unpack_from('<IIII',d,pe+24+so+8+i*40) for i in range(n)]
def off(r):
    for vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
for a in sys.argv[2:]:
    ad,ln=a.split(':'); ad=int(ad,16); ln=int(ln)
    print('==',hex(ad))
    o=off(ad-0x10000000)
    for i in md.disasm(d[o:o+ln],ad): print("%x: %s %s"%(i.address,i.mnemonic,i.op_str))
