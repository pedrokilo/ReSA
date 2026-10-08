import struct
from pathlib import Path
b=Path('D:/Grand Theft Auto San Andreas/gta-sa.exe').read_bytes();pe=struct.unpack_from('<I',b,0x3c)[0];n=struct.unpack_from('<H',b,pe+6)[0];op=struct.unpack_from('<H',b,pe+20)[0];sec=pe+24+op
for addr in [0x8D33E4,0x8D33E8,0x8D33EC,0x858C4C,0x871300,0x871304,0x871308,0x87130C,0x858B88,0x858CC4,0x859A3C,0x858C58,0x858A28,0x8595EC,0x859018,0x858B10,0x871254,0x858624,0x858628,0x858B50,0x859EF8,0x858FE8,0x858BA4,0x858CC8,0x858B1C,0x859F80,0x85F0A0,0x862D68,0x858C84,0x85F0FC]:
 rva=addr-0x400000
 for i in range(n):
  vs,va,size,raw=struct.unpack_from('<4I',b,sec+i*40+8)
  if va<=rva<va+max(vs,size):
   off=raw+rva-va;print(hex(addr),struct.unpack_from('<f',b,off)[0],struct.unpack_from('<d',b,off)[0]);break



