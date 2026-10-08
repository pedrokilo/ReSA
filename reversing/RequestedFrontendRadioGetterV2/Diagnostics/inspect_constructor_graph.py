import ctypes as c,struct,json,sys,subprocess,re,hashlib
from pathlib import Path
sys.path.insert(0,str(Path('reversing/FallenPeds').resolve()));from symbolize import Symbol
r=Path('reversing/RequestedFrontendRadioGetterV2');asi=(r/'artifacts/gta_reversed-frontend-radio-v2.asi').resolve();pdb=asi.with_suffix('.pdb');data=asi.read_bytes();pe=struct.unpack_from('<I',data,60)[0];base=struct.unpack_from('<I',data,pe+52)[0];size=struct.unpack_from('<I',data,pe+80)[0];n,opt=struct.unpack_from('<H',data,pe+6)[0],struct.unpack_from('<H',data,pe+20)[0];sections=[struct.unpack_from('<IIII',data,pe+24+opt+i*40+8)for i in range(n)]
def read(a,n):
 for vs,va,sz,off in sections:
  if va<=a-base and a-base+n<=va+sz:return data[off+a-base-va:off+a-base-va+n]
 raise Exception(hex(a))
def resolve(a):
 chain=[a]
 for i in range(8):
  if read(a,1)!=b'\xe9':return chain
  a=a+5+struct.unpack('<i',read(a+1,4))[0];chain.append(a)
 raise Exception('cycle')
d=c.WinDLL('dbghelp',use_last_error=True);h=c.c_void_p(0x4455);d.SymInitialize.argtypes=[c.c_void_p,c.c_char_p,c.c_int];d.SymLoadModuleEx.argtypes=[c.c_void_p,c.c_void_p,c.c_char_p,c.c_char_p,c.c_ulonglong,c.c_ulong,c.c_void_p,c.c_ulong];d.SymLoadModuleEx.restype=c.c_ulonglong;d.SymFromAddr.argtypes=[c.c_void_p,c.c_ulonglong,c.POINTER(c.c_ulonglong),c.POINTER(Symbol)];d.SymSetOptions(0x10|0x200|0x80000);assert d.SymInitialize(h,str(pdb.parent).encode(),False);assert d.SymLoadModuleEx(h,None,str(pdb).encode(),None,base,size,None,0)
def symbol(a):
 s=Symbol();s.SizeOfStruct=88;s.MaxNameLen=1024;delta=c.c_ulonglong();assert d.SymFromAddr(h,a,c.byref(delta),c.byref(s));assert delta.value==0;return s.Name.decode(),s.Size
exe=Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe');root=json.loads((r/'Diagnostics/compiled-symbols.json').read_text())['symbols'][0]['address'];pending=[root];seen=set();graph=[]
while pending:
 a=pending.pop(0)
 if a in seen:continue
 seen.add(a);name,length=symbol(a);text=subprocess.check_output([str(exe),'/disasm',f'/range:0x{a:X},0x{a+length:X}',str(asi)],text=True);(r/f'Diagnostics/ctor-graph-{a:X}.txt').write_text(text);calls=[]
 for m in re.finditer(r'\s*([0-9A-F]+): E8 .*?call\s+([0-9A-F]+)',text):
  at,target=[int(v,16)for v in m.groups()];chain=resolve(target);called,ln=symbol(chain[-1]);calls.append(dict(call=hex(at),chain=list(map(hex,chain)),name=called,size=ln))
  if called.startswith(('CAEFrontendAudioEntity::CAEFrontendAudioEntity','CAEAudioEntity::CAEAudioEntity','CAETwinLoopSoundEntity::CAETwinLoopSoundEntity','CAESound::CAESound','CVector::CVector','std::array<')):pending.append(chain[-1])
 graph.append(dict(address=hex(a),name=name,size=length,calls=calls));assert len(graph)<50
print(json.dumps(graph,indent=2));(r/'Diagnostics/constructor-callgraph.json').write_text(json.dumps(dict(asi_sha256=hashlib.sha256(data).hexdigest(),pdb_sha256=hashlib.sha256(pdb.read_bytes()).hexdigest(),graph=graph),indent=2))
