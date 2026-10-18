import ctypes,sys
from ctypes import wintypes as w
dh=ctypes.WinDLL('dbghelp')
class SI(ctypes.Structure):
    _fields_=[('SizeOfStruct',w.ULONG),('TypeIndex',w.ULONG),('Reserved',ctypes.c_uint64*2),('Index',w.ULONG),('Size',w.ULONG),('ModBase',ctypes.c_uint64),('Flags',w.ULONG),('Value',ctypes.c_uint64),('Address',ctypes.c_uint64),('Register',w.ULONG),('Scope',w.ULONG),('Tag',w.ULONG),('NameLen',w.ULONG),('MaxNameLen',w.ULONG),('Name',ctypes.c_char*1024)]
h=ctypes.c_void_p(1)
dh.SymSetOptions(0x2|0x4)
dh.SymInitialize(h,None,False)
dh.SymLoadModuleExW.restype=ctypes.c_uint64
dh.SymLoadModuleExW.argtypes=[ctypes.c_void_p,ctypes.c_void_p,w.LPCWSTR,w.LPCWSTR,ctypes.c_uint64,w.DWORD,ctypes.c_void_p,w.DWORD]
base=dh.SymLoadModuleExW(h,None,sys.argv[1],None,0x10000000,0x2000000,None,0)
print(hex(base))
CB=ctypes.WINFUNCTYPE(w.BOOL,ctypes.POINTER(SI),w.ULONG,ctypes.c_void_p)
def cb(si,sz,ctx):
    print(si.contents.Name.decode(errors='replace'),hex(si.contents.Address),si.contents.Size); return True
c=CB(cb)
dh.SymEnumSymbols.argtypes=[ctypes.c_void_p,ctypes.c_uint64,ctypes.c_char_p,CB,ctypes.c_void_p]
dh.SymEnumSymbols(h,base,sys.argv[2].encode(),c,None)
dh.SymFromAddr.argtypes=[ctypes.c_void_p,ctypes.c_uint64,ctypes.POINTER(ctypes.c_uint64),ctypes.POINTER(SI)]
for a in sys.argv[3:]:
    si=SI(); si.SizeOfStruct=88; si.MaxNameLen=1024; disp=ctypes.c_uint64()
    ok=dh.SymFromAddr(h,int(a,16),ctypes.byref(disp),ctypes.byref(si))
    print(a,ok,si.Name.decode(errors='replace'),disp.value)
