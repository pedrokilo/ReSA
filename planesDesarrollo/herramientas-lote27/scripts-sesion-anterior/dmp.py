import re
import struct,sys
S=r"C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad"
f=open(S+'/crash.dmp','rb'); data=f.read()
sig,ver,nstreams,dirrva=struct.unpack_from('<IIII',data,0)
streams={}
for i in range(nstreams):
    t,sz,rva=struct.unpack_from('<III',data,dirrva+12*i); streams.setdefault(t,[]).append((sz,rva))
print({k:len(v) for k,v in streams.items()})
# exception
sz,rva=streams[6][0]
tid,_=struct.unpack_from('<II',data,rva)
code,flags,rec,addr,nparams=struct.unpack_from('<IIQQI',data,rva+8)
params=struct.unpack_from('<15Q',data,rva+8+4+4+8+8+4+4)
print('thread',tid,'code',hex(code),'addr',hex(addr),'params',params[:nparams])
ctxsz,ctxrva=struct.unpack_from('<II',data,rva+8+152)
ctx=data[ctxrva:ctxrva+ctxsz]
# x86 CONTEXT: ContextFlags(0) ... Edi at 0x9c, Esi 0xa0, Ebx 0xa4, Edx 0xa8, Ecx 0xac, Eax 0xb0, Ebp 0xb4, Eip 0xb8, Cs 0xbc, EFlags 0xc0, Esp 0xc4
edi,esi,ebx,edx,ecx,eax,ebp,eip,cs,efl,esp=struct.unpack_from('<11I',ctx,0x9c)
print('eip',hex(eip),'esp',hex(esp),'ebp',hex(ebp))
# memory list (type 5) and memory64 (9)
regions=[]
if 9 in streams:
    sz,rva=streams[9][0]
    n,base=struct.unpack_from('<QQ',data,rva); off=base
    for i in range(n):
        st,l=struct.unpack_from('<QQ',data,rva+16+16*i); regions.append((st,l,off)); off+=l
for sz,rva in streams.get(5,[]):
    n=struct.unpack_from('<I',data,rva)[0]
    for i in range(n):
        st,l,o=struct.unpack_from('<QII',data,rva+4+16*i); regions.append((st,l,o))
print(len(regions),'regions')
def read(a,l):
    for s_,ln,o in regions:
        if s_<=a<s_+ln: return data[o+a-s_:o+a-s_+min(l,s_+ln-a)]
# modules
sz,rva=streams[4][0]
nm=struct.unpack_from('<I',data,rva)[0]
mods=[]
for i in range(nm):
    b,size,chk,ts,namerva=struct.unpack_from('<QIIII',data,rva+4+108*i)
    l=struct.unpack_from('<I',data,namerva)[0]; name=data[namerva+4:namerva+4+l].decode('utf-16-le')
    mods.append((b,size,name))
    if 'reversed' in name or 'gta_sa' in name: print(hex(b),hex(size),name)
st=read(esp,0x1000)
rets=[]
for i in range(0,len(st)-3,4):
    v=struct.unpack_from('<I',st,i)[0]
    for b,size,name in mods:
        if b<=v<b+size and ('reversed' in name or 'gta_sa' in name):
            rets.append((esp+i,v,name.replace(chr(92),'/').split('/')[-1],hex(v-b))); break
for r in rets[:60]: print(hex(r[0]),hex(r[1]),r[2],r[3])
# also walk ebp chain
print('ebp chain')
bp=ebp
for k in range(25):
    x=read(bp,8)
    if not x or len(x)<8: break
    nb,ra=struct.unpack('<II',x)
    print(hex(bp),hex(ra))
    if nb<=bp: break
    bp=nb

for a,nm in ((0xB71A60,'ms_dirName?'),(0xB71B60,'ms_rootDirName?'),(0xB71848,'gString')):
    x=read(a,200)
    print(nm,hex(a),x[:120] if x else None)
