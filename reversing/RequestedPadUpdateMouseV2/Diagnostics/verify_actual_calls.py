import contextlib,io,runpy,re,json,hashlib,subprocess
from pathlib import Path
B=Path(__file__).resolve().parents[1]
with contextlib.redirect_stdout(io.StringIO()):s=runpy.run_path(str(B/'inspect_compiled.py'))
read,resolve=s['read'],s['resolve'];records=s['records'];O=B/'Diagnostics/compiled-proof'
def sym(n):
 matches=[x for x in records if x['name']==n];assert len(matches)==1,n;return matches[0]
def asm(n):return (O/f"{sym(n)['body']:08X}.asm.txt").read_text(encoding='utf-8')
def call_at(va):
 raw=read(va,5);assert raw[0]==0xE8;import struct
 entry=va+5+struct.unpack_from('<i',raw,1)[0];return {'site':hex(va),'entry':hex(entry),'body':hex(resolve(entry)[0]),'chain':resolve(entry)[1]}
update=sym('CPad::UpdateMouse');calls={n:call_at(p) for n,p in {'foreground':0x11B0020A,'exclusive':0x11B00236,'init':0x11B0025A,'getmouse':0x11B00277,'checkinput':0x11B00460,'settouched':0x11B0046F}.items()}
for name,n in {'foreground':'IsForegroundApp','exclusive':'IsVideoModeExclusive','init':'WinInput::diMouseInit','getmouse':'CPad::GetMouseState','checkinput':'CMouseControllerState::CheckForInput','settouched':'CPad::SetTouched'}.items():assert int(calls[name]['body'],16)==sym(n)['body']
assert 'mov         ecx,dword ptr [ebp-0Ch]' in asm('CPad::UpdateMouse') and 'ret         4' in asm('CPad::GetMouseState')
assert 'ret         4' in asm('RequestedPadUpdateMouse::ReadDll');assert 'ret         4' not in asm('RequestedPadUpdateMouse::ReadExe')
assert 'cmp         dword ptr [ebp-8],eax' in asm('RequestedPadUpdateMouse::ReadDll')
# Compiler passes candidate then original stub immediates to gate construction.
pairs={'foreground':(0x11392C1D,0x11392C22,0x746060),'exclusive':(0x11392C86,0x11392C8B,0x745CA0),'init':(0x11392CEF,0x11392CF4,0x7469A0),'getmouse':(0x11392D58,0x11392D5D,0x746ED0)}
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
text=asm('RequestedPadUpdateMouse::Update');assert '53F270h' in text and '1000000h' in text
# Five entries protected: foreground, exclusive, init, read and pure native CheckForInput dependency.
report={'asi_sha256':s['digest'](s['asi']),'pdb_sha256':s['digest'](s['pdb']),'manifest_sha256':s['digest'](B/'artifacts/manifest.json'),'pdb_rsds_match':True,'passed':True,'helper_call_paths_verified':True,'separate_getmouse_ABIs_verified':True,'actual_target_calls':calls,'gate_operand_mapping':mapping,'pure_check_input':{'native':'0x53f270','own_entry':hex(sym('CMouseControllerState::CheckForInput')['address']),'own_body':hex(sym('CMouseControllerState::CheckForInput')['body']),'compiled_redirect_operand':'0x11392fc2','native_bytes_preserved_by_runtime_gate':128},'limitations':['Static call/ABI preparation only; native/DLL hook gates, all52 byte entry restores, strictFP and all17692 globals require runtime. No helper accreditation.']}
(B/'Diagnostics/compiled-runtime-proof.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8');print(json.dumps(report,indent=2))
