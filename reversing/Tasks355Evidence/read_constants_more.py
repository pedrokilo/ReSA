import struct
from pathlib import Path
b=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe').read_bytes();pe=struct.unpack_from('<I',b,60)[0];n=struct.unpack_from('<H',b,pe+6)[0];sz=struct.unpack_from('<H',b,pe+20)[0]
for a in [0x8d22b0,0x86c6a4]:
 r=a-0x400000
 for i in range(n):
  o=pe+24+sz+i*40;vs,va,rs,rp=struct.unpack_from('<IIII',b,o+8)
  if va<=r<va+max(vs,rs): print(hex(a),hex(struct.unpack_from('<I',b,rp+r-va)[0]),struct.unpack_from('<f',b,rp+r-va)[0])
