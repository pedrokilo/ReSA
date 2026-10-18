import sys,struct,re
exe=open(r'D:\Grand Theft Auto San Andreas\gta-sa.exe','rb').read()
pe=struct.unpack_from('<I',exe,0x3c)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
opt=struct.unpack_from('<H',exe,pe+20)[0]
o=pe+24+opt
vs,va,rs,rp=struct.unpack_from('<IIII',exe,o+8)
code=exe[rp:rp+rs]
for t in map(lambda x:int(x,16),sys.argv[1:]):
    print('== calls to %x'%t)
    for i in range(len(code)-5):
        if code[i]==0xE8:
            rel=struct.unpack_from('<i',code,i+1)[0]
            if base+va+i+5+rel==t:
                print('%x: ctx %s'%(base+va+i, code[max(0,i-8):i].hex()))
