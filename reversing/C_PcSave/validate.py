"""Compare extracted original x86 instructions with the current C++ function; no save IO."""
from pathlib import Path
import hashlib, json, struct, subprocess, os
root=Path(__file__).resolve().parents[2]
folder=Path(__file__).resolve().parent
build=folder/'test-build'; build.mkdir(exist_ok=True)
exe=Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe').read_bytes()
assert hashlib.sha256(exe).hexdigest()=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
pe=struct.unpack_from('<I',exe,0x3c)[0]
n=struct.unpack_from('<H',exe,pe+6)[0]; size=struct.unpack_from('<H',exe,pe+20)[0]
def extract(va,count):
 for i in range(n):
  s=pe+24+size+40*i
  _,v,rawsize,raw=struct.unpack_from('<IIII',exe,s+8)
  rva=va-0x400000
  if v<=rva and rva+count<=v+rawsize: return exe[raw+rva-v:raw+rva-v+count]
 raise ValueError(hex(va))
original=extract(0x6190a0,37)
assert original[26]==0xe8 and original[-3:]==b'\xc2\x08\x00'
assert extract(0x86bad4,7)==b'%s%i%s\0'
assert extract(0x86badc,3)==b'.b\0'
source=root/'source/game_sa/C_PcSave.cpp'
def body(text):
 start=text.index('void C_PcSave::GenerateGameFilename('); end=text.index('\n}',start)+2
 return text[start:end]
candidate=body(source.read_text(encoding='utf-8'))
legacy='void C_PcSave::GenerateGameFilename(int32 slot, char* out) {\n    assert(slot < MAX_SAVEGAME_SLOTS);\n\n    const auto maxSize = std::size(DefaultPCSaveFileName) + std::size(std::to_string(MAX_SAVEGAME_SLOTS)) + std::size(".b") - 2u;\n    sprintf_s(out, maxSize, "%s%i%s", DefaultPCSaveFileName, slot + 1, ".b");\n}'
template=r'''
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <string>
#include <iterator>
#include <cassert>
using int32=int32_t;
constexpr int MAX_SAVEGAME_SLOTS=8;
class C_PcSave { public: static inline char DefaultPCSaveFileName[260]; void GenerateGameFilename(int32 slot,char* out); };
BODY
static const unsigned char bytes[]={BYTES};
int main(){
 auto code=(unsigned char*)VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
 if(!code) return 9;
 memcpy(code,bytes,sizeof bytes);
 auto patch=[&](int offset,uintptr_t value){uint32_t v=(uint32_t)value;memcpy(code+offset,&v,4);};
 patch(9,(uintptr_t)".b"); patch(16,(uintptr_t)C_PcSave::DefaultPCSaveFileName);
 patch(21,(uintptr_t)"%s%i%s"); patch(27,(uintptr_t)&sprintf-(uintptr_t)(code+31));
 DWORD old; if(!VirtualProtect(code,4096,PAGE_EXECUTE_READ,&old)) return 10;
 FlushInstructionCache(GetCurrentProcess(),code,4096);
 auto original=reinterpret_cast<void(__stdcall*)(int32,char*)>(code);
 C_PcSave candidate; int count=0;
 for(int length=0;length<260;++length){
  auto prefix=C_PcSave::DefaultPCSaveFileName;
  for(int i=0;i<length;++i) prefix[i]="AZ09 \\:/._-%"[i%12];
  prefix[length]=0;
  for(int slot=0;slot<8;++slot){
   unsigned char a[544],b[544]; memset(a,0xa5,sizeof a);memset(b,0xa5,sizeof b);
   original(slot,(char*)a+16); candidate.GenerateGameFilename(slot,(char*)b+16);
   auto expected=std::string(prefix)+std::to_string(slot+1)+".b";
   if(memcmp(a,b,sizeof a)||strcmp((char*)b+16,expected.c_str())){printf("Mismatch length=%d slot=%d\n",length,slot);return 1;}
   for(size_t j=0;j<16;++j) if(b[j]!=0xa5) return 2;
   for(size_t j=16+expected.size()+1;j<sizeof b;++j) if(b[j]!=0xa5) return 3;
   ++count;
  }
 }
 VirtualFree(code,0,MEM_RELEASE);printf("PASS %d cases, including all 8 slots and prefix lengths 0..259\n",count);return 0;
}
'''
vs=r'C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvars32.bat'
reports=[]
for name,code,opt,should_pass in [('debug',candidate,'/Od',True),('optimized',candidate,'/O2',True),('mutation',candidate.replace('slot + 1','slot + 2'),'/O2',False),('legacy',legacy,'/O2',False)]:
 cpp=build/(name+'.cpp'); cpp.write_text(template.replace('BODY',code).replace('BYTES',','.join(hex(b) for b in original)))
 batch=build/'compile.cmd'
 batch.write_text(f'@echo off\ncall "{vs}" >nul\nif errorlevel 1 exit /b 1\ncl /nologo /EHsc /std:c++latest /W4 /WX /D_CRT_SECURE_NO_WARNINGS {opt} /MT "{cpp}" /Fe:"{build/name}.exe" /Fo:"{build/name}.obj"\n')
 compiled=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=build,capture_output=True,text=True)
 (build/(name+'-compile.log')).write_text(compiled.stdout+compiled.stderr)
 if compiled.returncode: raise RuntimeError(compiled.stdout+compiled.stderr)
 result=subprocess.run([str(build/(name+'.exe'))],cwd=build,capture_output=True,text=True,timeout=30)
 report=dict(name=name,exitcode=result.returncode,output=result.stdout,expected_pass=should_pass)
 reports.append(report); print(report,flush=True)
 assert (result.returncode==0)==should_pass
(folder/'results.json').write_text(json.dumps(dict(source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),original_function_sha256=hashlib.sha256(original).hexdigest(),runs=reports,limitations='Original instructions relocated; sprintf redirected to the host MSVC CRT. In-memory test only, no save/load or live game session.'),indent=2))
