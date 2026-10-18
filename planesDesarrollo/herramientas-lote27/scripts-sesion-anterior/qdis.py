import struct
from capstone import *
d=open(r"D:\Grand Theft Auto San Andreas\gta-sa.exe","rb").read()
pe=struct.unpack_from('<I',d,0x3c)[0]
ns=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
secs=[]
for i in range(ns):
    o=pe+24+so+i*40
    vs,va,rs,ro=struct.unpack_from('<IIII',d,o+8)
    secs.append((va+base,vs,ro))
def off(a):
    for va,vs,ro in secs:
        if va<=a<va+vs: return ro+a-va
md=Cs(CS_ARCH_X86,CS_MODE_32)
o=off(0x59C437)
for i in md.disasm(d[o:o+0xf9],0x59C437): print(hex(i.address),i.mnemonic,i.op_str)
for a in (0x858b50,0x858624,0x858b8c): print(hex(a),struct.unpack_from('<f',d,off(a))[0])
