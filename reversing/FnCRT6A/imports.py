# Lista las importaciones del EXE (solo lectura): modulo, nombre, direccion de la IAT.
import struct,sys
d=open("E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe",'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[struct.unpack_from('<8sIIII',d,so+i*40) for i in range(nsec)]
def r2o(r):
    for nm,vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
    return None
idr=struct.unpack_from('<II',d,pe+24+96+8)
o=r2o(idr[0]); res={}
while o is not None:
    ox,_,_,nm,ft=struct.unpack_from('<IIIII',d,o)
    if ox==0 and nm==0 and ft==0: break
    mod=d[r2o(nm):].split(b'\0')[0].decode()
    p=r2o(ox or ft); i=0
    while True:
        v=struct.unpack_from('<I',d,p+i*4)[0]
        if v==0: break
        if not v&0x80000000:
            res[d[r2o(v)+2:].split(b'\0')[0].decode()]=(mod,base+ft+i*4)
        i+=1
    o+=20
if __name__=='__main__':
    for k in sys.argv[1:]:
        print(k,res.get(k) and (res[k][0],hex(res[k][1])))
