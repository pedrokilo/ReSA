"""Exact compiled-byte assertions for frozen ASIs; diagnostic output only."""
import hashlib,json,re,struct,subprocess,sys
from pathlib import Path
root=Path(__file__).resolve().parents[3]
bank=root/'reversing'/('RequestedArrestRemainingFour' if len(sys.argv)>1 and sys.argv[1]=='arrest' else 'RequestedFxEmitterTwo')
prefix='arrest-remaining-four' if bank.name.endswith('Four') else 'fx-emitter-two'
asi=bank/f'artifacts/gta_reversed-{prefix}.asi';b=asi.read_bytes();meta=json.loads((bank/'Diagnostics/compiled-proof/symbols.json').read_text(encoding='utf-8'))
assert hashlib.sha256(b).hexdigest()==meta['asi_sha256'] and meta['pdb_rsds_match']
pe=struct.unpack_from('<I',b,60)[0];o=pe+24;base=struct.unpack_from('<I',b,o+28)[0];sec=o+struct.unpack_from('<H',b,pe+20)[0];ss=[struct.unpack_from('<4I',b,sec+40*i+8) for i in range(struct.unpack_from('<H',b,pe+6)[0])]
def off(rva):
    for vs,va,rs,ro in ss:
        if va<=rva<va+max(vs,rs):return ro+rva-va
    raise ValueError(hex(rva))
def read(a,n):p=off(a-base);return b[p:p+n]
def resolve(a):
    chain=[]
    for _ in range(16):
        raw=read(a,5)
        if raw[0]!=0xE9:return a,chain
        z=a+5+struct.unpack_from('<i',raw,1)[0];chain.append([hex(a),hex(z)]);a=z
    raise AssertionError('long jump chain')
def call(at,expected):
    raw=read(at,5);assert raw[0]==0xE8
    target=at+5+struct.unpack_from('<i',raw,1)[0];body,chain=resolve(target);assert body==expected,(hex(at),hex(body),hex(expected))
    return {'site':hex(at),'bytes':raw.hex(),'entry':hex(target),'body':hex(body),'e9_chain':chain}
def exports():
    er=struct.unpack_from('<I',b,o+96)[0];e=off(er);nf,nn,fa,na,oa=struct.unpack_from('<5I',b,e+20);out={}
    for i in range(nn):
        nr=struct.unpack_from('<I',b,off(na)+4*i)[0];p=off(nr);name=b[p:b.index(0,p)].decode('ascii');ordinal=struct.unpack_from('<H',b,off(oa)+2*i)[0];out[name]=base+struct.unpack_from('<I',b,off(fa)+4*ordinal)[0]
    return out
dump='C:/Program Files/Microsoft Visual Studio/18/Community/VC/Tools/MSVC/14.51.36231/bin/Hostx64/x86/dumpbin.exe'
def capture(address,length=192):
    s=subprocess.check_output([dump,'/disasm',f'/range:0x{address:X},0x{address+length:X}',str(asi)],text=True)
    (bank/f'Diagnostics/compiled-proof/{address:08X}.asm.txt').write_text(s,encoding='utf-8');return s
proof={'asi_sha256':meta['asi_sha256'],'pdb_sha256':meta['pdb_sha256'],'pdb_rsds_match':True,'compiled_header_sha256':meta['compiled_header_sha256'],'manifest_sha256':meta['manifest_sha256'],'runtime_executed':False}
ex=exports()
if prefix=='fx-emitter-two':
    vt=ex['??_7FxEmitterBP_c@@6B@'];assert vt==0x131C7754;slots=struct.unpack('<7I',read(vt,28));assert resolve(slots[2])[0]==0x1198E310
    assert read(0x1138913F,6).hex()=='898548cfffff' # actual EAX from indirect constructor CALL saved
    assert read(0x11389145,12).hex()=='8b8d48cfffff898d24f8ffff'
    comparison=read(0x11389B28,12);assert comparison.hex()=='398524f8ffff0f859e000000'
    objectBody,objectChain=resolve(0x110791E8);capture(objectBody,96)
    assert read(0x11C95EF1,7).hex()=='bac03a7f00ffd2'
    proof.update(vtable={'address':hex(vt),'count':7,'slots':[hex(x) for x in slots],'load_slot':2,'load_body':'0x1198E310'},raw_return_proof={'indirect_call_site':'0x11389132','eax_saved_site':'0x1138913F','returned_local':'EBP-0x7DC','compare_site':'0x11389B28','compare_bytes':comparison.hex(),'object_body':hex(objectBody),'object_entry_e9_chain':objectChain,'unequal_branch':'0x11389BD2 numeric JSON of actual returned','equal_branch':'0x11389B34 storage16 role JSON'},rw_call_paths=[call(0x1198BFAB,0x11C9B210),call(0x1198BFD7,0x11C9B210),call(0x11C9B22F,0x11C95ED0)],rw_native_call={'site':'0x11C95EF1','bytes':read(0x11C95EF1,7).hex(),'meaning':'MOV EDX,0x007F3AC0; CALL EDX; cdecl name/mask forwarded'},constructor_paths=[call(0x112BB193,0x1198C940),call(0x1198C963,0x1199FCE0),call(0x1199FD0F,0x11998590)],constructor_injection={'site':'0x1198D0C0','bytes':read(0x1198D0C0,10).hex(),'our_entry':'0x11080F29','resolved_body':hex(resolve(0x11080F29)[0]),'native_entry':'0x004A18D0'},cpu_stores={'emitter':'0x1198C96C vptr;0x1198C975 type0','list':'0x119985A2/AB/B5 writes head/tail/count0','textures_fill_entry':'0x1107DAB8','textures_fill_body':hex(resolve(0x1107DAB8)[0]),'native_base_and_list':'evidence/BaseCtor.asm.txt and ListCtor.asm.txt: CPU stores only','ctor_stubs':'None: only RwTextureRead entries are patched in frozen bank.'})
    assert proof['constructor_injection']['resolved_body']=='0x112bb170'
    capture(resolve(0x1107DAB8)[0],256)
    proof['cpu_fill_paths']=[call(0x119A0FCA,0x1199F800)]
    assert read(0x1199F8B4,4).hex()=='8b118910'
    proof['cpu_fill_generic_write']={'site':'0x1199F8B4','bytes':'8b118910','meaning':'Read zero value from source; write texture slot; four-count bounded CPU loop'}
    fillBody,fillChain=resolve(0x110599C4)
    proof['cpu_fill_optimized_path']={'entry':'0x110599C4','body':hex(fillBody),'chain':fillChain}
    capture(fillBody,256)
    memsetBody,memsetChain=resolve(0x11099466)
    capture(memsetBody,96)
    proof['cpu_fill_optimized_path'].update(zero_fill_call=call(0x1199F5BD,memsetBody),zero_argument_site='0x1199F5AE push0',byte_count_site='0x1199F5AA count<<2')
else:
    vt=ex['??_7CTaskComplexArrestPed@@6B@'];taskVT=ex['??_7CTask@@6B@'];complexVT=ex['??_7CTaskComplex@@6B@'];slots=struct.unpack('<11I',read(vt,44));assert vt==0x131D63C4 and taskVT==0x131CD604
    assert resolve(slots[6])[0]==0x1249B660 and resolve(slots[9])[0]==0x124997F0
    assert read(0x12498A8E,6)==b'\xC7\x00'+struct.pack('<I',vt)
    assert read(0x12498B52,6)==b'\xC7\x00'+struct.pack('<I',vt)
    assert read(0x12474ED2,6)==b'\xC7\x00'+struct.pack('<I',taskVT)
    paths=[call(0x12498AAE,resolve(0x110756BE)[0]),call(0x12498B5F,resolve(0x110A1E80)[0]),call(0x12499849,0x1249A120)]
    for x in paths:capture(int(x['body'],16),256)
    proof['injection_mapping']=[]
    for at,entry,native,body in [(0x1249AA80,0x11076A28,0x68B990,0x112B9640),(0x1249ABBC,0x11074E0D,0x68BA00,0x112C4750),(0x1249B2C4,0x1106F156,0x68CF80,0x1249A120)]:
        assert read(at,10)==b'\x68'+struct.pack('<I',entry)+b'\x68'+struct.pack('<I',native)
        assert resolve(entry)[0]==body
        proof['injection_mapping'].append({'site':hex(at),'own_entry':hex(entry),'own_body':hex(body),'native':hex(native),'bytes':read(at,10).hex()})
    proof['registered_reference_paths']=[call(0x117C3F78,0x117298F0),call(0x1186FF58,0x11718E00)]
    proof['base_terminal_call']=call(0x11236644,0x12474EC0)
    proof['boat_helper_paths']=[call(0x1249B721,0x12772B60),call(0x1249B734,0x1175A860),call(0x1249B752,0x1175A860),call(0x1249B767,0x1175CA60),call(0x1249B772,0x11776860)]
    proof.update(vtable={'address':hex(vt),'count':11,'slots':[hex(x) for x in slots],'abort_slot':6,'first_slot':9,'task_terminal':hex(taskVT),'complex_intermediate':hex(complexVT)},constructor_vptr_store={'address':'0x12498A8E','bytes':read(0x12498A8E,6).hex()},destructor_vptr_store={'address':'0x12498B52','bytes':read(0x12498B52,6).hex()},base_task_terminal_store={'address':'0x12474ED2','bytes':read(0x12474ED2,6).hex()},helper_paths=paths,base_destructor_path=call(0x12498B6A,0x112365C0),limitations=['Private actor storage/child vtable and controlled refs; no actors/delegates/helpers accredited. Three helper call paths followed through SafeRef bodies to registered ref/cleanup bodies and CreateSubTask; boat weapon helper paths resolved to registered bodies.'])
(bank/'Diagnostics/compiled-proof/call-path-proof.json').write_text(json.dumps(proof,indent=2),encoding='utf-8');print(json.dumps(proof,indent=2))
