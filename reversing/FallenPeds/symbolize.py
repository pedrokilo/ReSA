"""Resolve probe stack addresses against its matching local PDB (no network)."""
import ctypes as c
import json
import struct
import sys
from pathlib import Path

class Symbol(c.Structure):
    _fields_ = [('SizeOfStruct',c.c_ulong),('TypeIndex',c.c_ulong),('Reserved',c.c_ulonglong*2),('Index',c.c_ulong),('Size',c.c_ulong),('ModBase',c.c_ulonglong),('Flags',c.c_ulong),('Value',c.c_ulonglong),('Address',c.c_ulonglong),('Register',c.c_ulong),('Scope',c.c_ulong),('Tag',c.c_ulong),('NameLen',c.c_ulong),('MaxNameLen',c.c_ulong),('Name',c.c_char*1024)]

def main():
    rows=[json.loads(s) for s in Path(sys.argv[1]).read_text().splitlines()]
    base=next(r['asi_base'] for r in rows if r['event']=='start')
    pdb=Path(sys.argv[2]).resolve()
    with pdb.with_suffix('.asi').open('rb') as image:
        image.seek(0x3c);pe=struct.unpack('<I',image.read(4))[0]
        image.seek(pe+24+56);image_size=struct.unpack('<I',image.read(4))[0]
    d=c.WinDLL('dbghelp',use_last_error=True)
    handle=c.c_void_p(0x1234)
    d.SymInitialize.argtypes=[c.c_void_p,c.c_char_p,c.c_int]
    d.SymLoadModuleEx.argtypes=[c.c_void_p,c.c_void_p,c.c_char_p,c.c_char_p,c.c_ulonglong,c.c_ulong,c.c_void_p,c.c_ulong]
    d.SymLoadModuleEx.restype=c.c_ulonglong
    d.SymFromAddr.argtypes=[c.c_void_p,c.c_ulonglong,c.POINTER(c.c_ulonglong),c.POINTER(Symbol)]
    d.SymSetOptions(0x10|0x200|0x80000)
    assert d.SymInitialize(handle,str(pdb.parent).encode(),False)
    assert d.SymLoadModuleEx(handle,None,str(pdb).encode(),None,base,image_size,None,0),c.get_last_error()
    for addr in sorted({a for r in rows for a in r.get('stack',[])}):
        s=Symbol();s.SizeOfStruct=88;s.MaxNameLen=1024
        displacement=c.c_ulonglong()
        ok=base<=addr<base+image_size and d.SymFromAddr(handle,addr,c.byref(displacement),c.byref(s))
        print(hex(addr),s.Name.decode(errors='replace')+'+'+hex(displacement.value) if ok else '?')

if __name__=='__main__':
    main()
