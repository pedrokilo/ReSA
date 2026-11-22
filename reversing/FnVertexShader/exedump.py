import sys,struct,capstone
D=open("D:/Grand Theft Auto San Andreas/gta-sa.exe","rb").read()
pe=struct.unpack_from("<I",D,0x3c)[0]
ns=struct.unpack_from("<H",D,pe+6)[0]; osz=struct.unpack_from("<H",D,pe+20)[0]
base=struct.unpack_from("<I",D,pe+52)[0]
secs=[]
for i in range(ns):
    o=pe+24+osz+40*i
    vs,va,rs,rp=struct.unpack_from("<IIII",D,o+8)
    secs.append((va,vs,rp,rs))
def rd(va,n):
    r=va-base
    out=b""
    for (a,vs,rp,rs) in secs:
        if a<=r<a+max(vs,rs):
            off=r-a
            m=min(n,rs-off) if off<rs else 0
            out=D[rp+off:rp+off+m]
            out+=b"\0"*(n-len(out))
            return out
    raise Exception("no va %x"%va)
if __name__=="__main__":
    a=int(sys.argv[1],16);n=int(sys.argv[2],0)
    md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32)
    for i in md.disasm(rd(a,n),a):
        print("%08x %s %s"%(i.address,i.mnemonic,i.op_str))
