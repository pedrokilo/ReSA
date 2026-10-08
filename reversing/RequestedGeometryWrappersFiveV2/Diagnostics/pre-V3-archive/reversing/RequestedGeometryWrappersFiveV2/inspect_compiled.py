import ctypes as c,struct,json,hashlib,subprocess,sys
from pathlib import Path
sys.path.insert(0,str(Path('reversing/FallenPeds').resolve()))
from symbolize import Symbol
r=Path('reversing/RequestedGeometryWrappersFiveV2');asi=(r/'artifacts/gta_reversed-geometry-wrappers-five-v2.asi').resolve();pdb=asi.with_suffix('.pdb');data=asi.read_bytes();pe=struct.unpack_from('<I',data,0x3c)[0];base=struct.unpack_from('<I',data,pe+24+28)[0];size=struct.unpack_from('<I',data,pe+24+56)[0]
d=c.WinDLL('dbghelp',use_last_error=True);h=c.c_void_p(0x4567);d.SymInitialize.argtypes=[c.c_void_p,c.c_char_p,c.c_int];d.SymLoadModuleEx.argtypes=[c.c_void_p,c.c_void_p,c.c_char_p,c.c_char_p,c.c_ulonglong,c.c_ulong,c.c_void_p,c.c_ulong];d.SymLoadModuleEx.restype=c.c_ulonglong
d.SymSetOptions(0x10|0x200|0x80000);assert d.SymInitialize(h,str(pdb.parent).encode(),False);assert d.SymLoadModuleEx(h,None,str(pdb).encode(),None,base,size,None,0)
cbtype=c.WINFUNCTYPE(c.c_int,c.POINTER(Symbol),c.c_ulong,c.c_void_p);syms=[]
@cbtype
def callback(p,n,v):
 s=p.contents;syms.append({'name':s.Name.decode(errors='replace'),'address':s.Address,'size':s.Size});return 1
d.SymEnumSymbols.argtypes=[c.c_void_p,c.c_ulonglong,c.c_char_p,cbtype,c.c_void_p]
for mask in (b'*CPedGeometryAnalyser::ComputeEntityBoundingBox*',b'*CEntity::CEntity*',b'*CGeneral::GetRandomNumber*',b'*RequestedGeometryWrappersFiveV2Live::ConstructFixture*'):assert d.SymEnumSymbols(h,base,mask,callback,None)
(r/'Diagnostics/compiled-symbols.json').write_text(json.dumps({'asi_sha256':hashlib.sha256(data).hexdigest(),'pdb_sha256':hashlib.sha256(pdb.read_bytes()).hexdigest(),'base':base,'symbols':syms},indent=2))
exe=Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe')
for i,s in enumerate(syms):
 out=subprocess.check_output([str(exe),'/disasm',f'/range:0x{s["address"]:X},0x{s["address"]+max(s["size"],32):X}',str(asi)],text=True)
 (r/f'Diagnostics/compiled-body-{i}.txt').write_text(out);print(json.dumps(s));print(out)
