import struct,sys
p="E:/JUEGOS/GTA San Andreas - CVehicle Test/gta_sa.exe"
d=open(p,'rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
nsec=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
so=pe+24+opt
secs=[struct.unpack_from('<8sIIII',d,so+i*40) for i in range(nsec)]
def rd(a,n):
    r=a-base
    for nm,vs,va,rs,rp in secs:
        if va<=r<va+max(vs,rs): o=rp+r-va; return d[o:o+n]
for a in sys.argv[1:]:
    a=int(a,16); b=rd(a,8)
    print(hex(a),'f32=',struct.unpack('<f',b[:4])[0],'u32=',hex(struct.unpack('<I',b[:4])[0]),'f64=',struct.unpack('<d',b)[0])
