import struct,sys,ctypes,re
from ctypes import wintypes as w
S=r"C:/Users/pedro/AppData/Local/Temp/claude/D--Original-gta-reversed-master/c66a4e30-afba-4db2-93b7-ac1baf6b4246/scratchpad"
def load(p):
    d=open(p,'rb').read()
    pe=struct.unpack_from('<I',d,0x3c)[0]
    n=struct.unpack_from('<H',d,pe+6)[0]; so=struct.unpack_from('<H',d,pe+20)[0]
    secs=[struct.unpack_from('<IIII',d,pe+24+so+8+i*40) for i in range(n)]
    return d,secs
pd,ps=load(S+'/probe.asi'); cd,cs=load(S+'/cur.asi')
def off(secs,rva):
    for vs,v,rs,rp in secs:
        if v<=rva<v+max(vs,rs): return rp+rva-v
def va(secs,o):
    for vs,v,rs,rp in secs:
        if rp<=o<rp+rs: return v+o-rp
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
for rva in [int(x,16) for x in sys.argv[1:]]:
    o=off(ps,rva)
    s=o
    while s>0 and not(pd[s-1]==0xcc and pd[s-2]==0xcc): s-=1
    delta=o-s
    # take first 20 bytes of function but wildcard rel32 not needed; search exact 14 bytes
    pat=pd[s:s+14]
    idxs=[m.start() for m in re.finditer(re.escape(pat),cd)]
    res=[]
    for ix in idxs[:5]:
        res.append(name(0x10000000+va(cs,ix)+delta))
    print(hex(rva),'func@',hex(va(ps,s)),'+',hex(delta),'->',res)
