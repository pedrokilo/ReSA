import struct,sys
d=open(r"D:/Grand Theft Auto San Andreas/gta-sa.exe","rb").read()
# PE sections
pe=struct.unpack_from('<I',d,0x3c)[0]
n=struct.unpack_from('<H',d,pe+6)[0]; opt=struct.unpack_from('<H',d,pe+20)[0]
base=struct.unpack_from('<I',d,pe+24+28)[0]
secs=[]
for i in range(n):
    o=pe+24+opt+i*40
    nm=d[o:o+8].rstrip(b'\0'); vs,va,rs,rp=struct.unpack_from('<IIII',d,o+8)
    secs.append((nm,va,vs,rp,rs))
print(hex(base),[(s[0],hex(base+s[1]),hex(s[2])) for s in secs][:3])
t=[s for s in secs if s[0]==b'.text'][0]
text=d[t[3]:t[3]+t[4]]; tb=base+t[1]
targets=[int(x,16) for x in sys.argv[1:]]
for T in targets:
    c=[]
    for i in range(len(text)-5):
        if text[i] in (0xE8,0xE9):
            r=struct.unpack_from('<i',text,i+1)[0]
            if (tb+i+5+r)&0xffffffff==T: c.append(hex(tb+i))
    # also data refs (vtable/pointer)
    p=struct.pack('<I',T); dr=[]
    k=d.find(p)
    while k>=0 and len(dr)<5: dr.append(k); k=d.find(p,k+1)
    print(hex(T),'callers',len(c),c,'ptr-refs',len(dr))
