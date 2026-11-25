import pefile, struct, capstone, sys, re
PATH=r"D:\Original\_ginput_ref\src\Original(Reworked)(Incl.All TXD's)\GInputSA.asi"
pe=pefile.PE(PATH); BASE=pe.OPTIONAL_HEADER.ImageBase
data=open(PATH,'rb').read()
def sect(va):
    rva=va-BASE
    for s in pe.sections:
        if s.VirtualAddress<=rva<s.VirtualAddress+max(s.Misc_VirtualSize,s.SizeOfRawData): return s
def rd(va,n):
    s=sect(va); rva=va-BASE
    off=s.PointerToRawData+rva-s.VirtualAddress
    return data[off:off+n]
def u32(va): return struct.unpack('<I',rd(va,4))[0]
md=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32); md.detail=True
text=pe.sections[0]; TB=BASE+text.VirtualAddress; TS=text.Misc_VirtualSize
def dis(va,n=0x200,count=None):
    out=[]
    for i in md.disasm(rd(va,n),va):
        out.append(i)
        if count and len(out)>=count: break
    return out
def fmt(va,n=0x200,count=None):
    for i in dis(va,n,count): print("%08x  %-24s %s %s"%(i.address,i.bytes.hex(),i.mnemonic,i.op_str))
_all=None
def allins():
    global _all
    if _all is None:
        _all=list(md.disasm(rd(TB,TS),TB))
    return _all
IAT={}
for e in pe.DIRECTORY_ENTRY_IMPORT:
    for i in e.imports: IAT[i.address]=(e.dll.decode()+'!'+(i.name.decode() if i.name else str(i.ordinal)))
