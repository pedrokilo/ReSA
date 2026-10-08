"""Prove actual CALL operands/ILT paths in the exact PDB-matched captured ASI."""
import hashlib,json,re,struct,subprocess
from pathlib import Path
HERE=Path(__file__).resolve().parents[1];D=HERE/'Diagnostics'
asi=HERE/'artifacts/gta_reversed-geometry-wrappers-five-v4.asi';data=asi.read_bytes();pe=struct.unpack_from('<I',data,60)[0];base=struct.unpack_from('<I',data,pe+52)[0];n,opt=struct.unpack_from('<H',data,pe+6)[0],struct.unpack_from('<H',data,pe+20)[0]
sections=[struct.unpack_from('<IIII',data,pe+24+opt+i*40+8) for i in range(n)]
def sha(p):return hashlib.sha256(p.read_bytes()).hexdigest()
def read(address,length):
    r=address-base
    for vs,va,size,off in sections:
        if va<=r and r+length<=va+size:return data[off+r-va:off+r-va+length]
    raise AssertionError(hex(address))
def resolve(entry):
    chain=[entry]
    for i in range(8):
        if read(entry,1)!=b'\xe9':return chain
        entry=entry+5+struct.unpack('<i',read(entry+1,4))[0];chain.append(entry)
    raise AssertionError('ILT cycle')
def call(address):
    assert read(address,1)==b'\xe8';return address+5+struct.unpack('<i',read(address+1,4))[0]
symbols=json.loads((D/'compiled-symbols.json').read_text());assert symbols['asi_sha256']==sha(asi) and symbols['pdb_sha256']==sha(asi.with_suffix('.pdb'))
def sym(name):
    a=[x for x in symbols['symbols'] if x['name']==name];assert len(a)==1;return a[0]
construct=sym('RequestedGeometryWrappersFiveV4Live::ConstructFixture')['address'];assert read(construct+0x2f,6)==bytes.fromhex('8b8d38ffffff')
ctor_chain=resolve(call(construct+0x35));ctor=ctor_chain[-1]
assert ctor in [x['address'] for x in symbols['symbols'] if x['name']=='CEntity::CEntity']
assert read(ctor+0xe3,7)==bytes.fromhex('8b4dec66894120')
general_chain=resolve(call(ctor+0xde));general=general_chain[-1];assert general==sym('CGeneral::GetRandomNumber')['address']
rand_chain=resolve(call(general+0x17))
update=(D/'update-body-0.txt').read_text();assert f'{general_chain[0]:X}h' in update
helpers={};native=[('ComputeEntityBoundingBoxCornersUncached',0x5F1FA0),('ComputeEntityBoundingBoxCentreUncached',0x5F1600),('ComputeEntityBoundingBoxPlanesUncached',0x5F1670),('ComputeEntityBoundingBoxSegmentPlanesUncached',0x5F1750)]
for name,address in native:
    pattern=rf'mov\s+dword ptr \[.*?\],{address:X}h\s*\n[^\n]*\n\s*[0-9A-F]+: C7 85[^\n]*mov\s+dword ptr \[.*?\],([0-9A-F]+)h'
    matches=re.findall(pattern,update);assert len(matches)==1,(name,matches)
    entry=int(matches[0],16);chain=resolve(entry);assert chain[-1]==sym('CPedGeometryAnalyser::'+name)['address'];helpers[name]=dict(patched_entry=hex(entry),chain=list(map(hex,chain)),source_symbol='CPedGeometryAnalyser::'+name)
calls={}
for short,name,expected in [('Corners','ComputeEntityBoundingBoxCorners',[native[0][0]]),('CentreAll','ComputeEntityBoundingBoxCentreUncachedAll',[native[0][0],native[1][0]]),('PlanesAll','ComputeEntityBoundingBoxPlanesUncachedAll',[native[0][0],native[2][0]]),('SegmentAll','ComputeEntityBoundingBoxSegmentPlanesUncachedAll',[native[0][0],native[1][0],native[3][0]])]:
    info=sym('CPedGeometryAnalyser::'+name);index=symbols['symbols'].index(info);text=(D/f'compiled-body-{index}.txt').read_text();records=[]
    for line in text.splitlines():
        m=re.match(r'\s*([0-9A-F]+): E8 .*?call\s+([0-9A-F]+)',line)
        if m:
            at,target=map(lambda v:int(v,16),m.groups())
            if hex(target) in {x['patched_entry'] for x in helpers.values()}:assert call(at)==target;records.append(dict(call=hex(at),patched_entry=hex(target)))
    assert [r['patched_entry'] for r in records]==[helpers[x]['patched_entry'] for x in expected];calls[short]=records
segment=sym('CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanes');idx=symbols['symbols'].index(segment);text=(D/f'compiled-body-{idx}.txt').read_text();allbody=sym('CPedGeometryAnalyser::ComputeEntityBoundingBoxSegmentPlanesUncachedAll')['address'];chains=[]
for m in re.finditer(r'\s*([0-9A-F]+): E8 .*?call\s+([0-9A-F]+)',text):
    at,target=map(lambda v:int(v,16),m.groups());chain=resolve(target)
    if chain[-1]==allbody:assert call(at)==target;chains.append(chain)
assert len(chains)==1
exe=Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe');exports=subprocess.check_output([str(exe),'/exports',str(asi)],text=True)
m=re.search(r'\s[0-9A-F]+\s+([0-9A-F]{8}) \?\?_7CEntity@@6B@',exports);assert m;rva=int(m.group(1),16);assert read(ctor+0x51,6)==b'\xc7\x00'+struct.pack('<I',base+rva)
proof=dict(run_ready=True,reviewer=True,asi_sha256=sha(asi),pdb_sha256=sha(asi.with_suffix('.pdb')),symbol_table_sha256=sha(D/'compiled-symbols.json'),construct_body=hex(construct),ctor_chain=list(map(hex,ctor_chain)),general_ctor_call_chain=list(map(hex,general_chain)),general_patched_entry=hex(general_chain[0]),general_body=hex(general),readonly_linked_rand_call_chain=list(map(hex,rand_chain)),exact_relative_offsets_and_prefixes_checked=[0x35,0xde,0x17],actualDLLhelperpatchentry_from_Update=helpers,calls_cross_patched_entry=calls,sourceSegmentToAll_chain=list(map(hex,chains[0])),CEntity_named_export_vptr=dict(name='??_7CEntity@@6B@',rva=hex(rva),preferred_address=hex(base+rva),matched=True),scope='Actual compiled CALL/patch and fixture ABI proof only; full replay/harness restoration still required.')
(D/'required-callchain-proof.json').write_text(json.dumps(proof,indent=2)+'\n');print(json.dumps(proof,indent=2))
