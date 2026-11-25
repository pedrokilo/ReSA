import pefile,struct
OP=r"D:\Original\_comparar\orig\gta_sa.exe"
ope=pefile.PE(OP); od=open(OP,'rb').read(); OB=0x400000
def orb(va,n):
    rva=va-OB
    for s in ope.sections:
        if s.VirtualAddress<=rva<s.VirtualAddress+max(s.Misc_VirtualSize,s.SizeOfRawData):
            off=s.PointerToRawData+rva-s.VirtualAddress
            if rva-s.VirtualAddress<s.SizeOfRawData: return od[off:off+n]
            return b'\0'*n
    return None
def osect(va):
    rva=va-OB
    for s in ope.sections:
        if s.VirtualAddress<=rva<s.VirtualAddress+max(s.Misc_VirtualSize,s.SizeOfRawData): return s.Name.strip(b'\0').decode()
