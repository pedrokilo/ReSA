exec(open('reversing/VehicleAudio355Evidence/read_damage_constants.py').read().split('for addr')[0])
a=0x85AA80
for i in range(n):
 vs,va,size,raw=struct.unpack_from('<4I',b,sec+i*40+8)
 if va<=a-0x400000<va+max(vs,size):
  off=raw+a-0x400000-va;print(b[off:off+64].split(b'\0')[0]);break
