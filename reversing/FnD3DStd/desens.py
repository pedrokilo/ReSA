import sys, struct, capstone
EXE = r'D:\Original\_crt1a_work\gta_sa.exe'
d = open(EXE,'rb').read()
pe = struct.unpack_from('<I', d, 0x3c)[0]
nsec = struct.unpack_from('<H', d, pe+6)[0]
osz = struct.unpack_from('<H', d, pe+20)[0]
base = struct.unpack_from('<I', d, pe+52)[0]
secs=[]
o = pe+24+osz
for i in range(nsec):
    name,vs,va,rs,ro = struct.unpack_from('<8sIIII', d, o+i*40)
    secs.append((va,vs,ro,rs))
def rd(addr, n):
    rva = addr-base
    for va,vs,ro,rs in secs:
        if va<=rva<va+max(vs,rs):
            off = ro+rva-va
            return d[off:off+n]
    raise Exception('no map %x'%addr)
def dis(addr, n, stop_ret=True):
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    code = rd(addr, n)
    for i in md.disasm(code, addr):
        print('%08x  %-10s %s' % (i.address, i.mnemonic, i.op_str))
if __name__=='__main__':
    a=int(sys.argv[1],16); n=int(sys.argv[2],0)
    dis(a,n)
def dwords(addr, n):
    b = rd(addr, n*4)
    return struct.unpack('<%dI'%n, b)
