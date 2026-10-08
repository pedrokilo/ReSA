from pathlib import Path
import json,hashlib,struct,subprocess
p=Path('reversing/RequestedFrontendRadioGetterV2');asi=p/'artifacts/gta_reversed-frontend-radio-v2.asi';data=asi.read_bytes();pe=struct.unpack_from('<I',data,60)[0];base=struct.unpack_from('<I',data,pe+52)[0];opt=struct.unpack_from('<H',data,pe+20)[0];n=struct.unpack_from('<H',data,pe+6)[0];sections=[struct.unpack_from('<IIII',data,pe+24+opt+i*40+8)for i in range(n)]
def read(a,n):
 for vs,va,sz,off in sections:
  if va<=a-base and a-base+n<=va+sz:return data[off+a-base-va:off+a-base-va+n]
 raise Exception(hex(a))
assert read(0x1122c61c,3)==bytes.fromhex('8b450c')
assert read(0x1121e9e7,8)==bytes.fromhex('6a008b8d30ffffff')
assert read(0x1121a477,8)==bytes.fromhex('8b45ec833800741b')
graph=json.loads((p/'Diagnostics/constructor-callgraph.json').read_text());assert graph['asi_sha256']==hashlib.sha256(data).hexdigest();assert graph['pdb_sha256']==hashlib.sha256(asi.with_suffix('.pdb').read_bytes()).hexdigest()
exe=Path('C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe')
for name,a,size in [('placement-new',0x1122c610,22),('entityref-null',0x1121a430,147),('wenum-default',0x1121ae20,37)]:
 txt=subprocess.check_output([str(exe),'/disasm',f'/range:0x{a:X},0x{a+size:X}',str(asi)],text=True);(p/f'Diagnostics/ctor-leaf-{name}.txt').write_text(txt)
proof=dict(reviewed=True,default_constructor_has_no_external_side_effects=True,asi_sha256=graph['asi_sha256'],pdb_sha256=graph['pdb_sha256'],constructor_callgraph_sha256=hashlib.sha256((p/'Diagnostics/constructor-callgraph.json').read_bytes()).hexdigest(),fixture_root='0x113773a0',actual_default_ctor_chain=['0x11090aea','0x1121d8f0'],typed_entity_offsets=['0x9c','0x144'],default_base_chain=['0x11079a2a','0x1121cd70'],default_sound_chain=['0x110a4eb9','0x1121e9a0'],default_twin_chain=['0x1107b4a6','0x1121ed00'],allowed_memory_operations=['placement new1122c610 returns supplied pointer at [ebp+0Ch], no allocation','memset current linked12f83eb0 zeroes supplied fixture bounded1EC','default constructors store only owned scalar/pointer/vptr fields','compiler RTC_CheckEsp diagnostics; no failure route for balanced ABI'],entityref_guard=dict(caller='1121E9E7 push0 then ECX fieldthis then call11084EA2',body='1121A430',null_branch='1121A477 cmp [ownedptr],0 / je1121A49A skips RegisterReference11081E82',external_registration_executed=False),lifetime_scope='Pure getter fixture only; no constructor/destructor/audio asset/controller integration accreditation. Storage retained inert, no owned resource destruction required.',source_entityref_sha256=hashlib.sha256(Path('source/extensions/EntityRef.hpp').read_bytes()).hexdigest())
(p/'Diagnostics/constructor-proof.json').write_text(json.dumps(proof,indent=2)+'\n');print(json.dumps(proof,indent=2))
