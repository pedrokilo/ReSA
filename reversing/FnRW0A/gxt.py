import struct,sys
f=open(sys.argv[1],'rb').read()
ver,enc=struct.unpack_from('<HH',f,0)
pos=4
assert f[pos:pos+4]==b'TABL'
sz=struct.unpack_from('<I',f,pos+4)[0]
tabs=[(f[pos+8+i*12:pos+8+i*12+8].split(b'\0')[0].decode(),struct.unpack_from('<I',f,pos+8+i*12+8)[0]) for i in range(sz//12)]
pats=[p.lower().encode() for p in sys.argv[2:]]
for name,off in tabs:
    p=off
    if name!='MAIN': p+=8  # nombre de tabla de 8 bytes en tablas de mision
    assert f[p:p+4]==b'TKEY',(name,f[p:p+4])
    ksz=struct.unpack_from('<I',f,p+4)[0]
    keys=[struct.unpack_from('<II',f,p+8+i*8) for i in range(ksz//8)]
    q=p+8+ksz
    assert f[q:q+4]==b'TDAT'
    dsz=struct.unpack_from('<I',f,q+4)[0]
    dat=f[q+8:q+8+dsz]
    for o,h in keys:
        e=dat.find(b'\0',o); t=dat[o:e]
        if any(x in t.lower() for x in pats): print(name,hex(h),t[:120])
