import struct,sys
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
nsec=struct.unpack_from('<H',exe,pe+6)[0]; opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
secs=[];o=pe+24+opt
for i in range(nsec):
    vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8+i*40);secs.append((va,vs,rp,rs))
def off(a):
    r=a-base
    for va,vs,rp,rs in secs:
        if va<=r<va+max(vs,rs): return rp+r-va
def dump(name,a,n):
    p=off(a);return 'static const unsigned char %s[]={%s};\n'%(name,','.join('0x%02x'%x for x in exe[p:p+n]))
out=''
for spec in sys.argv[1:]:
    name,a,n=spec.split(':');out+=dump(name,int(a,16),int(n,16))
    out+='static const unsigned ADDR_%s=0x%s;\n'%(name,a)
open('exebytes.inc','w').write(out)
