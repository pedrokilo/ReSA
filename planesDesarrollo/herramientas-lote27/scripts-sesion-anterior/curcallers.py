import struct,re,sys,ctypes
from ctypes import wintypes as w
S=r"C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad"
d=open(S+'/cur.asi','rb').read()
pe=struct.unpack_from('<I',d,0x3c)[0]
n=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
secs=[struct.unpack_from('<IIII',d,pe+24+so+8+i*40) for i in range(n)]
def va(o):
    for vs,v,rs,rp in secs:
        if rp<=o<rp+rs: return 0x10000000+v+o-rp
# symbols
dh=ctypes.WinDLL('dbghelp')
class SI(ctypes.Structure):
    _fields_=[('SizeOfStruct',w.ULONG),('TypeIndex',w.ULONG),('Reserved',ctypes.c_uint64*2),('Index',w.ULONG),('Size',w.ULONG),('ModBase',ctypes.c_uint64),('Flags',w.ULONG),('Value',ctypes.c_uint64),('Address',ctypes.c_uint64),('Register',w.ULONG),('Scope',w.ULONG),('Tag',w.ULONG),('NameLen',w.ULONG),('MaxNameLen',w.ULONG),('Name',ctypes.c_char*1024)]
h=ctypes.c_void_p(1)
dh.SymSetOptions(0x2|0x4); dh.SymInitialize(h,None,False)
dh.SymLoadModuleExW.restype=ctypes.c_uint64
dh.SymLoadModuleExW.argtypes=[ctypes.c_void_p,ctypes.c_void_p,w.LPCWSTR,w.LPCWSTR,ctypes.c_uint64,w.DWORD,ctypes.c_void_p,w.DWORD]
base=dh.SymLoadModuleExW(h,None,r'D:\Original\gta-reversed-master\bin\Debug\gta_reversed.asi',None,0x10000000,0x2000000,None,0)
dh.SymFromAddr.argtypes=[ctypes.c_void_p,ctypes.c_uint64,ctypes.POINTER(ctypes.c_uint64),ctypes.POINTER(SI)]
def name(a):
    si=SI(); si.SizeOfStruct=88; si.MaxNameLen=1024; disp=ctypes.c_uint64()
    ok=dh.SymFromAddr(h,a,ctypes.byref(disp),ctypes.byref(si))
    return (si.Name.decode(errors='replace')+'+'+hex(disp.value)) if ok else '?'
calls={}
for m in re.finditer(b'[\xe8\xe9]',d):
    o=m.start(); a=va(o)
    if a is None or o+5>len(d): continue
    rel=struct.unpack_from('<i',d,o+1)[0]
    calls.setdefault((a+5+rel)&0xffffffff,[]).append((a,d[o]==0xe9))
pat=bytes.fromhex('8BFF558BEC6A17FF15')
k=d.find(pat); f=va(k); print('fastfail fn',hex(f),name(f))
lv=[f]
for dep in range(3):
    nx=[]
    for x in lv:
        c=calls.get(x,[]); print(dep,hex(x),name(x),len(c))
        for a,j in c:
            if dep>=1 or True: print('   ','J' if j else 'C',hex(a),name(a))
        nx+=[a for a,j in c if j]
    lv=nx

def climb(F,maxd=7):
    seen={F}; fr=[F]; named=[]
    for dep in range(1,maxd+1):
        nx=[]
        for x in fr:
            targets=[x]
            # thunks jumping to x
            targets+= [a for a,j in calls.get(x,[]) if j]
            for t in targets:
                for a,j in calls.get(t,[]):
                    if j: continue
                    fs=fstart(a)
                    if fs in seen: continue
                    seen.add(fs)
                    nm=name(fs)
                    if nm!='?' and not nm.startswith('ILT'): named.append((dep,nm,hex(fs)))
                    nx.append(fs)
        fr=nx
        print('depth',dep,len(fr),file=sys.stderr)
    return named
def fstart(a):
    for vs,v,rs,rp in secs:
        if v<=a-0x10000000<v+max(vs,rs): o=rp+(a-0x10000000)-v; bo=rp; vb=v
    while o>bo+3 and not (d[o-1]==0xcc and d[o-2]==0xcc): o-=1
    return 0x10000000+vb+(o-bo)

import collections
allnamed=[]
for ca in [0x1303d4f1,0x130b5083]:
    fs=fstart(ca); r=climb(fs,7); allnamed+=r
pat=re.compile(r'Bone|X87|Euler|Quat|Limit|Ragdoll|Atan|sqrt|Cpp',re.I)
for x in allnamed:
    if pat.search(x[1]): print(x)
print(len(allnamed))
