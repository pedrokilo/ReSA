"""Transcribe complete verified REA assembly; relocate branches, switch and VC7 SEH."""
from pathlib import Path
import re,json,struct,hashlib,sys
exe=Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe').read_bytes()
assert hashlib.sha256(exe).hexdigest()=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
pe=struct.unpack_from('<I',exe,60)[0]
count=struct.unpack_from('<H',exe,pe+6)[0];opt=struct.unpack_from('<H',exe,pe+20)[0]
base=struct.unpack_from('<I',exe,pe+24+28)[0]
sections=[struct.unpack_from('<8sIIII',exe,pe+24+opt+i*40) for i in range(count)]
def read(va,size):
 for name,virtual_size,rva,raw_size,raw in sections:
  if rva<=va-base<rva+max(virtual_size,raw_size):
   return exe[raw+va-base-rva:raw+va-base-rva+size]
 raise ValueError(hex(va))
assert read(0x8485AB,10)==bytes.fromhex('b880138a00e9eb8ffdff')
assert struct.unpack('<7I',read(0x8A1380,28))==(0x19930520,5,0x8A1358,0,0,0,0)
assert struct.unpack('<10I',read(0x8A1358,40))==(0xFFFFFFFF,0x848580,0,0x84858B,0,0x848593,0,0x84859B,0,0x8485A3)
rows=[]
for line in Path('reversing/VehicleAudio355Evidence/Plane_PreRenderFull.txt').read_text().splitlines():
 m=re.fullmatch(r'([0-9A-F]{8}): (.+)',line)
 if m and 0x6C94A0<=int(m[1],16)<=0x6CAADD:rows.append((int(m[1],16),m[2]))
assert rows[0][0]==0x6C94A0 and rows[-1]==(0x6CAADD,'ret')
calls=sorted({int(m[1],16) for _,s in rows if (m:=re.fullmatch(r'call ([0-9A-F]{8})',s))})
targets={int(m[1],16) for _,s in rows if (m:=re.fullmatch(r'j\w+ ([0-9A-F]{8})',s))}
switch=struct.unpack('<6I',read(0x6CAAE0,24));targets.update(switch)
assert targets <= {a for a,s in rows};assert all(not 0x6C94A0<=a<=0x6CAADD for a in calls)
out=['// Complete REA transcription preserves x87 precision and original collaborator ABI.',
 '// All internal code, switch entries and five SEH unwind states reside in the DLL.',
 'static const uintptr_t PlanePreRender355Dispatcher=0x8215A0;',
 'static const uintptr_t PlanePreRender355MatrixDestructor=0x59ACD0;']
for n,disp in enumerate([0xD4,0x70,0x70,0x70,0x70]):
 out.extend([f'static __declspec(naked) void __cdecl PlanePreRender355Unwind{n}() {{',
 f'    __asm {{ lea ecx,[ebp-0{disp:X}h] }}',
 '    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }','}'])
out+=['struct PlanePreRender355UnwindEntry { int32 previousState; void(__cdecl* action)(); };',
 'static const PlanePreRender355UnwindEntry PlanePreRender355UnwindMap[] = {',
 *[f'    {{{-1 if n==0 else 0}, &PlanePreRender355Unwind{n}}},' for n in range(5)],'};',
 'struct PlanePreRender355FuncInfo { uint32 magic; int32 maxState; const PlanePreRender355UnwindEntry* unwind; int32 tryCount; const void* tries; int32 ipCount; const void* ipMap; };',
 'static const PlanePreRender355FuncInfo PlanePreRender355ExceptionInfo{0x19930520,5,PlanePreRender355UnwindMap,0,nullptr,0,nullptr};',
 'static_assert(sizeof(PlanePreRender355FuncInfo)==28);',
 'static_assert(sizeof(PlanePreRender355UnwindEntry)==8);',
 'static __declspec(naked) void __cdecl PlanePreRender355ExceptionHandler() {',
 '    __asm { mov eax,offset PlanePreRender355ExceptionInfo }',
 '    __asm { jmp dword ptr [PlanePreRender355Dispatcher] }','}',
 'static const uint8 PlanePreRender355SwitchMap[] = {'+','.join(map(str,read(0x6CAAF8,117)))+'};',
 'static const uintptr_t PlanePreRender355Calls[] = {',
 *[f'    0x{a:08X}, // collaborator {i}' for i,a in enumerate(calls)],'};',
 'static uintptr_t PlanePreRender355SwitchTargets[6]{};',
 '#pragma warning(push)', '#pragma warning(disable:4733)',
 '__declspec(naked) void CPlane::PreRender() {','    __asm {',
 *[f'        mov dword ptr [PlanePreRender355SwitchTargets+{i*4}],offset plane_prerender_{a:08X}' for i,a in enumerate(switch)]]
for a,s in rows:
 if a in targets:out.append(f'    plane_prerender_{a:08X}:')
 if m:=re.fullmatch(r'call ([0-9A-F]{8})',s):s=f'call dword ptr [PlanePreRender355Calls+{calls.index(int(m[1],16))*4}]'
 elif m:=re.fullmatch(r'(j\w+) ([0-9A-F]{8})',s):s=f'{m[1]} plane_prerender_{int(m[2],16):08X}'
 s=s.replace('push 8485ABh','push offset PlanePreRender355ExceptionHandler')
 s=s.replace('[eax+006CAAF8h]','[eax+PlanePreRender355SwitchMap]').replace('[edx*4+006CAAE0h]','[edx*4+PlanePreRender355SwitchTargets]')
 out.append('        '+s)
out+=['    }','}', '#pragma warning(pop)']
body='\n'.join(out)+'\n'
assert not re.search(r'8485ABh|006CAAE0h|006CAAF8h',body)
Path('reversing/VehicleAudio355Evidence/Plane_PreRenderNative.txt').write_text(body,encoding='utf-8',newline='\n')
Path('reversing/VehicleAudio355Evidence/Plane_PreRender_dependencies.json').write_text(json.dumps([{'index':i,'address':f'0x{a:08X}'} for i,a in enumerate(calls)],indent=2),encoding='utf-8',newline='\n')
if '--apply' in sys.argv:
 p=Path('source/game_sa/Entity/Vehicle/Plane.cpp');s=p.read_text(encoding='utf-8');old='void CPlane::PreRender() {\n    plugin::CallMethod<0x6C94A0, CPlane*>(this);\n}'
 assert s.count(old)==1;s=s.replace(old,body.rstrip());p.write_text(s,encoding='utf-8',newline='\n')
print('TRANSCRIBED',len(rows),'instructions',len(targets),'internal labels',len(calls),'collaborators')
