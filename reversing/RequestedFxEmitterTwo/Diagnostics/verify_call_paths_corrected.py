"""Corrected frozen ASI84fdf static proof; retains original b1b8 proof separately."""
from pathlib import Path
utility=Path(__file__).with_name('verify_call_paths.py').read_text().split("if prefix=='fx-emitter-two':")[0]
exec(utility)
def symbol(name):
    matches=[x for x in meta['symbols'] if x['name']==name and x['body']>=0x11900000];assert len(matches)==1,(name,matches);return matches[0]
load=symbol('FxEmitterBP_c::LoadTextures')['body'];rw=symbol('RwTextureRead')['body'];template=symbol('plugin::CallAndReturn<RwTexture *,8338112,char const *,char const *>')['body'];ctor=symbol('FxEmitterBP_c::FxEmitterBP_c')['body'];baseCtor=symbol('FxPrimBP_c::FxPrimBP_c')['body'];listCtor=symbol('TList_c<Particle_c>::TList_c<Particle_c>')['body']
vt=ex['??_7FxEmitterBP_c@@6B@'];slots=struct.unpack('<7I',read(vt,28));assert resolve(slots[2])[0]==load
assert read(0x1138913F,6).hex()=='898548cfffff' and read(0x11389145,12).hex()=='8b8d48cfffff898d24f8ffff' and read(0x11389B28,12).hex()=='398524f8ffff0f859e000000'
raw=read(template,symbol('plugin::CallAndReturn<RwTexture *,8338112,char const *,char const *>')['size']);idx=raw.find(bytes.fromhex('bac03a7f00ffd2'));assert idx>=0 and raw.count(bytes.fromhex('bac03a7f00ffd2'))==1
rwCalls=[call(0x1198BFAB,rw),call(0x1198BFD7,rw),call(rw+31,template)]
ctorCalls=[call(0x112BB193,ctor),call(ctor+35,baseCtor),call(baseCtor+47,listCtor)]
assert read(ctor+44,6)==b'\xC7\x00'+struct.pack('<I',vt)
assert read(ctor+53,4).hex()=='c6400400'
assert read(listCtor+18,6).hex()=='c70000000000'
assert read(listCtor+27,7).hex()=='c7400400000000' and read(listCtor+37,7).hex()=='c7400800000000'
for address,length in [(0x11389120,96),(0x11389B10,256),(ctor,80),(baseCtor,117),(listCtor,54),(rw,59),(template,70),(load,288),(0x1198C070,196)]:capture(address,length)
proof.update(vtable={'address':hex(vt),'count':7,'slots':[hex(x) for x in slots],'load_body':hex(load)},raw_return_R2={'eax_saved':'0x1138913F','comparison':'0x11389B28','unequal_branch':'0x11389BD2 stores actual numeric return','equal_branch':'0x11389B34 stores role16'},rw_call_paths=rwCalls,native_rw_call={'site':hex(template+idx),'bytes':'bac03a7f00ffd2','native':'0x007F3AC0'},real_ctor_base_list_paths=ctorCalls,ctor_stubs=False,ctor_vptr_store={'site':hex(ctor+44),'raw':read(ctor+44,6).hex()},scope='Static preferred-image addresses with matching PDB; real base/List CPU stores, no target/base ctor stubs. Runtime target/helper gates and restoration remain mandatory.')
(bank/'Diagnostics/compiled-proof/call-path-proof-corrected.json').write_text(json.dumps(proof,indent=2)+'\n');print(json.dumps(proof,indent=2))

