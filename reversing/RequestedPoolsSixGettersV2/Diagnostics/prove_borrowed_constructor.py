import json,hashlib,re,struct
from pathlib import Path
p=Path('reversing/RequestedPoolsSixGettersV2');D=p/'Diagnostics';asi=p/'artifacts/gta_reversed-pools-six-getters-v2.asi';b=asi.read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I',b,pe+52)[0];n,opt=struct.unpack_from('<H',b,pe+6)[0],struct.unpack_from('<H',b,pe+20)[0];sections=[struct.unpack_from('<IIII',b,pe+24+opt+i*40+8)for i in range(n)]
def read(a,n):
 for vs,va,sz,off in sections:
  if va<=a-base and a-base+n<=va+sz:return b[off+a-base-va:off+a-base-va+n]
 raise Exception(hex(a))
def resolve(a):
 chain=[a]
 for i in range(8):
  if read(a,1)!=b'\xe9':return chain
  a=a+5+struct.unpack('<i',read(a+1,4))[0];chain.append(a)
 raise Exception('cycle')
def sha(f):return hashlib.sha256(f.read_bytes()).hexdigest()
symbols=json.loads((D/'compiled-symbols.json').read_text());graph=json.loads((D/'constructor-callgraph.json').read_text());assert symbols['asi_sha256']==graph['asi_sha256']==sha(asi) and symbols['pdb_sha256']==graph['pdb_sha256']==sha(asi.with_suffix('.pdb'))
leaves={c['name']for node in graph['graph']for c in node['calls']if not any(c['chain'][-1]==v['address']for v in graph['graph'])};assert leaves=={'_CrtDbgReport','_RTC_CheckEsp','_RTC_CheckStackVars','_wassert','memset'},leaves
for node in graph['graph']:
 assert not re.search(r'(?<!Pool)::(?:~?(?:CPed|CCopPed|CVehicle|CHeli|CObject|CCutsceneObject))\b',node['name'])
 if node['name']=='operator new':assert node['size']==22 and bytes.fromhex('8b450c')in read(int(node['address'],16),node['size'])
families=[]
for pool,storage,stride in [('CPedPool','CCopPed',0x7c4),('CVehiclePool','CHeli',0xa18),('CObjectPool','CCutsceneObject',0x19c)]:
 syms=symbols['symbols'];ctor=next(x for x in syms if x['name']==pool+'::'+pool and x['size']==78);run=next(x for x in syms if x['name']=='RequestedPoolsSixGettersV2Live::Run<'+pool+','+storage+'>');text=(D/f'compiled-body-{syms.index(run)}.txt').read_text();matches=[]
 # PDB Size includes compiler RTC descriptor tables after the real RET.
 # They are data, not executable CALL evidence.
 end=re.search(r'\s*[0-9A-F]+: C3\s+ret',text);assert end;text=text[:end.end()]
 for m in re.finditer(r'\s*([0-9A-F]+): E8 .*?call\s+([0-9A-F]+)',text):
  at,target=[int(v,16)for v in m.groups()];chain=resolve(target)
  if chain[-1]==ctor['address']:matches.append(dict(call=hex(at),chain=list(map(hex,chain))))
 assert len(matches)==1
 parent=next(x for x in graph['graph']if int(x['address'],16)==ctor['address']);typed=[x for x in parent['calls']if x['name'].startswith('CPool<')];assert len(typed)==1
 borrowed=int(typed[0]['chain'][-1],16);raw=read(borrowed,219)
 assert bytes.fromhex('894808')in raw and bytes.fromhex('c7400cffffffff')in raw and bytes.fromhex('c6401000')in raw and bytes.fromhex('c6401100')in raw
 assert bytes.fromhex('803800')not in raw # bool owner remains zero; pointer assertions use83cmp.
 fill=next(x for x in graph['graph']if x['name'].endswith('::DoFill')and storage in x['name']);assert struct.pack('<I',stride)in read(int(fill['address'],16),fill['size'])
 indirect=re.search(r'([0-9A-F]+): FF 95[^\n]*call\s+dword ptr[^\n]*\n\s*[0-9A-F]+: 83 C4 04[^\n]*add\s+esp,4',text);assert indirect
 families.append(dict(pool=pool,storage=storage,stride=stride,run_body=hex(run['address']),actual_borrowed_ctor_call=matches[0],inherited_ctor=hex(ctor['address']),base_ctor_chain=typed[0]['chain'],layout='storage+0,state+4,capacity+8,cursor+0C,owner0+10,policy0+11; sizeof20',raw_target_call=indirect.group(1),target_ABI='uint32 cdecl(oneuint32arg), push argument then indirectcall then addesp4; fullEAX saved, no AL canonicalization'))
proof=dict(reviewed=True,no_actor_or_heap_lifecycle_calls=True,asi_sha256=sha(asi),pdb_sha256=sha(asi.with_suffix('.pdb')),constructor_callgraph_sha256=sha(D/'constructor-callgraph.json'),families=families,reachable_fixture_operations='Borrowed header scalar stores; SlotState1-byte uninitialized_fill placement construction; linked memset bounded private storage. _wassert excluded by nonnull arena pointers, range diagnostics excluded by ordered2-element bounds; compiler RTC only valid-path checks. No actor/pool destructor invoked.',contract='Only in-bounds indices0/1, aligned rawslotaddresses and active IDs0..127. Native unchecked invalid-index behavior intentionally untested. No actor lifetime/helper credit.')
(D/'borrowed-constructor-proof.json').write_text(json.dumps(proof,indent=2));print(json.dumps(proof,indent=2))
