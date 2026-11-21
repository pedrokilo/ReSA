# Lee dwords del EXE (solo lectura): rd.py direccion cantidad
import struct,sys
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[]
for i in range(nsec):
    n,vs,va,rs,rp=struct.unpack_from('<8sIIII',d,so+i*40); secs.append((va+base,vs,rp,rs))
def rd(a,n):
    for va,vs,rp,rs in secs:
        if va<=a<va+max(vs,rs): return d[rp+a-va:rp+a-va+n]
a=int(sys.argv[1],16); n=int(sys.argv[2])
b=rd(a,4*n)
print(' '.join('%08X'%x for x in struct.unpack('<%dI'%n,b)))
