"""Read-only matching PE/PDB inspection. Never loads target code or launches GTA."""
import ctypes as c, hashlib, json, math, struct, subprocess, sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[3]
sys.path.insert(0,str(ROOT/'reversing/FallenPeds'))
from symbolize import Symbol
BANK=Path(sys.argv[1]).resolve() if len(sys.argv)>1 else Path(__file__).resolve().parents[1]
manifest=json.loads((BANK/'artifacts/manifest.json').read_text(encoding='utf-8'))
prefix='fx-emitter-two' if BANK.name=='RequestedFxEmitterTwo' else 'arrest-remaining-four'
asi=BANK/f'artifacts/gta_reversed-{prefix}.asi';pdb=asi.with_suffix('.pdb');OUT=BANK/'Diagnostics/compiled-proof';OUT.mkdir(parents=True,exist_ok=True)
digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
assert digest(asi)==manifest['asi'] and digest(pdb)==manifest['pdb']
for label in ('tested_sources','bank_sources'):
    for name,wanted in manifest[label].items():assert digest(ROOT/name)==wanted,name
b=asi.read_bytes();pe=struct.unpack_from('<I',b,60)[0];o=pe+24;assert struct.unpack_from('<H',b,o)[0]==0x10B
base=struct.unpack_from('<I',b,o+28)[0];size=struct.unpack_from('<I',b,o+56)[0];sec=o+struct.unpack_from('<H',b,pe+20)[0]
ss=[struct.unpack_from('<4I',b,sec+i*40+8) for i in range(struct.unpack_from('<H',b,pe+6)[0])]
def offset(rva):
    for vs,va,rs,ro in ss:
        if va<=rva<va+max(vs,rs):return ro+rva-va
    raise ValueError(hex(rva))
def read(va,n):a=offset(va-base);return b[a:a+n]
def resolve(va):
    chain=[]
    for _ in range(16):
        raw=read(va,5)
        if raw[0]!=0xE9:return va,chain
        nxt=va+5+struct.unpack_from('<i',raw,1)[0];chain.append([hex(va),hex(nxt)]);va=nxt
        if not base<=va<base+size:return va,chain
    raise ValueError('E9 chain exceeds16')
# CodeView RSDS identity compared to PDB stream1, independent of DbgHelp lookup.
debugRva,debugSize=struct.unpack_from('<II',b,o+96+6*8);rsds=[]
for j in range(debugSize//28):
    row=struct.unpack_from('<IIHHIIII',b,offset(debugRva)+28*j)
    if row[4]==2:
        raw=b[row[7]:row[7]+row[5]]
        if raw[:4]==b'RSDS':rsds.append((raw[4:20],struct.unpack_from('<I',raw,20)[0]))
pb=pdb.read_bytes();block,_,_,directoryBytes,_,directoryMap=struct.unpack_from('<6I',pb,32)
directoryBlocks=struct.unpack_from('<'+'I'*math.ceil(directoryBytes/block),pb,directoryMap*block)
directory=b''.join(pb[v*block:(v+1)*block] for v in directoryBlocks)[:directoryBytes];streams=struct.unpack_from('<I',directory)[0];sizes=struct.unpack_from('<'+'I'*streams,directory,4);cursor=4+streams*4;info=None
for j,n in enumerate(sizes):
    count=0 if n==0xFFFFFFFF else math.ceil(n/block);indices=struct.unpack_from('<'+'I'*count,directory,cursor);cursor+=4*count
    if j==1:info=b''.join(pb[v*block:(v+1)*block] for v in indices)[:n]
assert info is not None;identity=(info[12:28],struct.unpack_from('<I',info,8)[0]);assert identity in rsds
d=c.WinDLL('dbghelp',use_last_error=True);handle=c.c_void_p(0x4567)
d.SymInitialize.argtypes=[c.c_void_p,c.c_char_p,c.c_int];d.SymLoadModuleEx.argtypes=[c.c_void_p,c.c_void_p,c.c_char_p,c.c_char_p,c.c_ulonglong,c.c_ulong,c.c_void_p,c.c_ulong];d.SymLoadModuleEx.restype=c.c_ulonglong
d.SymSetOptions(0x10|0x200|0x80000);assert d.SymInitialize(handle,str(pdb.parent).encode(),False);assert d.SymLoadModuleEx(handle,None,str(pdb).encode(),None,base,size,None,0)
callbackType=c.WINFUNCTYPE(c.c_int,c.POINTER(Symbol),c.c_ulong,c.c_void_p);symbols=[]
@callbackType
def callback(p,n,v):
    s=p.contents;symbols.append({'name':s.Name.decode(errors='replace'),'address':s.Address,'size':s.Size,'tag':s.Tag});return 1
d.SymEnumSymbols.argtypes=[c.c_void_p,c.c_ulonglong,c.c_char_p,callbackType,c.c_void_p]
patterns=[b'*RequestedFxEmitterTwo::Update*',b'*FxEmitterBP_c::FxEmitterBP_c*',b'*FxPrimBP_c::FxPrimBP_c*',b'*FxEmitterBP_c::LoadTextures*',b'*RwTextureRead*',b'*FxEmitterBP_c::Constructor*',b'*FxEmitterBP_c::InjectHooks*',b'*plugin::CallAndReturn*RwTexture*',b'*TList_c<Particle_c>::TList_c*',b'*FxInfoManager_c::FxInfoManager_c*'] if prefix=='fx-emitter-two' else [b'*CTaskComplexArrestPed::CTaskComplexArrestPed*',b'*CTaskComplexArrestPed::~CTaskComplexArrestPed*',b'*CTaskComplexArrestPed::CreateFirstSubTask*',b'*CTaskComplexArrestPed::MakeAbortable*',b'*CEntity::RegisterReference*',b'*CEntity::CleanUpOldReference*',b'*CTaskComplex::~CTaskComplex*',b'*CTask::~CTask*',b'*MakeSurePedHasWeaponInHand*',b'*CTaskComplexArrestPed::Constructor*',b'*CTaskComplexArrestPed::Destructor*',b'*CTaskComplexArrestPed::InjectHooks*',b'*CTaskComplexArrestPed::CreateSubTask*',b'*CVehicle::IsUpsideDown*',b'*CVehicle::IsOnItsSide*',b'*CPed::DoWeHaveWeaponAvailable*',b'*CPed::SetCurrentWeapon*',b'*CPed::GiveWeapon*',b'*CWeapon::IsTypeMelee*']
for mask in patterns:assert d.SymEnumSymbols(handle,base,mask,callback,None)
dump='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe'
records=[]
for s in sorted({x['address']:x for x in symbols if x['tag']==5}.values(),key=lambda x:x['address']):
    body,chain=resolve(s['address']);n=max(s['size'],64);record={**s,'body':body,'e9_chain':chain,'first128':read(body,128).hex()};records.append(record)
    text=subprocess.check_output([dump,'/disasm',f'/range:0x{body:X},0x{body+n:X}',str(asi)],text=True)
    (OUT/f"{body:08X}.asm.txt").write_text(text,encoding='utf-8')
summary={'asi_sha256':digest(asi),'pdb_sha256':digest(pdb),'pdb_rsds_match':True,'pdb_guid_raw':identity[0].hex(),'pdb_age':identity[1],'base':base,'symbols':records,'manifest_sha256':digest(BANK/'artifacts/manifest.json'),'compiled_header_sha256':manifest['bank_sources'][f'reversing/{BANK.name}/LiveComparison.hpp'],'limitations':['Static PE/PDB and disassembly only. No runtime, helper behavior or approval. Human inspection required for call-path/return-branch proof.']}
(OUT/'symbols.json').write_text(json.dumps(summary,indent=2),encoding='utf-8');print(json.dumps({'bank':BANK.name,'pdb_match':True,'base':hex(base),'symbols':[(x['name'],hex(x['body']),x['size']) for x in records]},indent=2))
