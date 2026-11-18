import json,sys,struct,math
sys.path.insert(0,'.')
import fixture_oracle as O
pairs={}
with open(sys.argv[1],encoding='utf-8') as fh:
    for line in fh:
        if '"function":"CAudioZones::Bugstar' not in line or not line.startswith('{"accepted"'): continue
        r=json.loads(line)
        a=[x&0xFFFFFFFF for x in r['input']['a']]
        b=bytes.fromhex(r['original']['observed']['blob'])
        pos=8+a[11]+10
        for i in range(3):
            ln=struct.unpack_from('<I',b,pos)[0]; t=b[pos+4:pos+4+ln]; pos+=4+ln+4
            pairs[a[4+i]]=t
bad=0
for bits,t in pairs.items():
    m=O.fmt_coord(bits)
    if m!=t:
        bad+=1
        if bad<25: print(hex(bits),struct.unpack('<f',struct.pack('<I',bits))[0],t,m)
print(len(pairs),'valores distintos;',bad,'distintos del modelo')
