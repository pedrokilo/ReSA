import struct,sys
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=0x400000
secs=[]
o=pe+24+opt
for i in range(nsec):
    vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8+i*40); secs.append((va,vs,rp,rs))
def rd(a,n=4):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): return exe[rp+r-va:rp+r-va+n]
for a in sys.argv[1:]:
    a=int(a,16); b=rd(a)
    print(hex(a), b.hex(), struct.unpack('<f',b)[0], hex(struct.unpack('<I',b)[0]))
