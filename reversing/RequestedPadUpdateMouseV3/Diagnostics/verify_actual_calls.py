import contextlib,io,runpy,re,json,hashlib,subprocess
from pathlib import Path
B=Path(__file__).resolve().parents[1]
with contextlib.redirect_stdout(io.StringIO()):s=runpy.run_path(str(B/'inspect_compiled.py'))
read,resolve=s['read'],s['resolve'];records=s['records'];O=B/'Diagnostics/compiled-proof'
def sym(n):
 matches=[x for x in records if x['name']==n];assert len(matches)==1,n;return matches[0]
def asm(n):return (O/f"{sym(n)['body']:08X}.asm.txt").read_text(encoding='utf-8')
def call_at(va):
 raw=read(va,5);assert raw[0]==0xE8,hex(va);import struct
 entry=va+5+struct.unpack_from('<i',raw,1)[0];return {'site':hex(va),'entry':hex(entry),'body':hex(resolve(entry)[0]),'chain':resolve(entry)[1]}
update=sym('CPad::UpdateMouse')
# Derive call sites from the compiled body: every E8 instruction boundary in the dumpbin listing of UpdateMouse (no fixed addresses).
import struct
lo,hi=update['body'],update['body']+update['size']
sites=[int(m.group(1),16) for m in re.finditer(r'^\s+([0-9A-F]{8}): E8 ',asm('CPad::UpdateMouse'),re.M)];sites=[a for a in sites if lo<=a<hi]
assert sites and all(lo<=a and a+5<=hi for a in sites)
expected={'foreground':'IsForegroundApp','exclusive':'IsVideoModeExclusive','init':'WinInput::diMouseInit','getmouse':'CPad::GetMouseState','checkinput':'CMouseControllerState::CheckForInput','settouched':'CPad::SetTouched'}
resolved=[(a,call_at(a)) for a in sites];calls={};last=-1
for key,n in expected.items():
 hits=[(i,c) for i,(a,c) in enumerate(resolved) if int(c['body'],16)==sym(n)['body']]
 assert len(hits)==1,(key,len(hits));assert hits[0][0]>last,('call order',key);last=hits[0][0];calls[key]=hits[0][1]
assert list(calls)==list(expected)
for name,n in expected.items():assert int(calls[name]['body'],16)==sym(n)['body']
assert 'mov         ecx,dword ptr [ebp-0Ch]' in asm('CPad::UpdateMouse') and 'ret         4' in asm('CPad::GetMouseState')
assert 'ret         4' in asm('RequestedPadUpdateMouse::ReadDll');assert 'ret         4' not in asm('RequestedPadUpdateMouse::ReadExe')
assert 'cmp         dword ptr [ebp-8],eax' in asm('RequestedPadUpdateMouse::ReadDll')
# Compiler passes candidate then original stub immediates to gate construction.
# Derive the stub-operand sites from the compiled Update body: after "mov [ebp+x],<native>" the two consecutive "push imm32" are (candidate, original), lower address first.
upd=asm('RequestedPadUpdateMouse::Update');L=upd.splitlines();pairs={}
for n,native in {'foreground':0x746060,'exclusive':0x745CA0,'init':0x7469A0,'getmouse':0x746ED0}.items():
 idx=[i for i,l in enumerate(L) if re.search(r'mov\s+dword ptr \[ebp[+-][0-9A-F]+h?\],%Xh\s*$'%native,l)];assert len(idx)==1,(n,len(idx))
 i=idx[0]+1
 while not re.match(r'\s+[0-9A-F]{8}: ',L[i]):i+=1
 ps=[]
 while len(ps)<2:
  m=re.match(r'\s+([0-9A-F]{8}): 68 ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2}) ([0-9A-F]{2})\s+push\s',L[i])
  if m:ps.append(int(m.group(1),16))
  elif ps:raise AssertionError((n,'pushes not consecutive'))
  i+=1;assert i<len(L)
 pairs[n]=(ps[0],ps[1],native)
import struct
mapping={}
for n,(candidate,original,native) in pairs.items():
 vals=[]
 for p in (original,candidate):
  raw=read(p,5);assert raw[0]==0x68;entry=struct.unpack_from('<I',raw,1)[0];body,chain=resolve(entry);vals.append({'operand_site':hex(p),'stub_entry':hex(entry),'stub_body':hex(body),'chain':chain})
 mapping[n]={'native':hex(native),'original':vals[0],'candidate':vals[1]}
 for v in vals:
  body=int(v['stub_body'],16);out=subprocess.check_output([s['dump'],'/disasm',f'/range:0x{body:X},0x{body+256:X}',str(s['asi'])],text=True);(O/f'{body:08X}-stub.asm.txt').write_text(out,encoding='utf-8')
assert int(mapping['getmouse']['original']['stub_body'],16)==sym('RequestedPadUpdateMouse::ReadExe')['body']
assert int(mapping['getmouse']['candidate']['stub_body'],16)==sym('RequestedPadUpdateMouse::ReadDll')['body']
assert all(mapping[n]['original']['stub_body']==mapping[n]['candidate']['stub_body'] for n in ('foreground','exclusive','init'))
# The 128-byte native CheckForInput snapshot: "push 80h; push 53F270h" in Update (derived, not fixed).
m=re.findall(r'^\s+([0-9A-F]{8}): 68 80 00 00 00\s+push\s+80h\n\s+([0-9A-F]{8}): 68 70 F2 53 00\s+push\s+53F270h',upd,re.M);assert m
redirect_site=int(m[0][1],16)
text=asm('RequestedPadUpdateMouse::Update');assert '53F270h' in text and '1000000h' in text
# Five entries protected: foreground, exclusive, init, read and pure native CheckForInput dependency.
report={'asi_sha256':s['digest'](s['asi']),'pdb_sha256':s['digest'](s['pdb']),'manifest_sha256':s['digest'](B/'artifacts/manifest.json'),'pdb_rsds_match':True,'passed':True,'helper_call_paths_verified':True,'separate_getmouse_ABIs_verified':True,'actual_target_calls':calls,'gate_operand_mapping':mapping,'pure_check_input':{'native':'0x53f270','own_entry':hex(sym('CMouseControllerState::CheckForInput')['address']),'own_body':hex(sym('CMouseControllerState::CheckForInput')['body']),'compiled_redirect_operand':hex(redirect_site),'native_bytes_preserved_by_runtime_gate':128},'limitations':['Static call/ABI preparation only; native/DLL hook gates, all52 byte entry restores, strictFP and all17692 globals require runtime. No helper accreditation.']}
(B/'Diagnostics/compiled-runtime-proof.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(json.dumps(report,indent=2))
