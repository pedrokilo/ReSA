from pathlib import Path
import struct, hashlib, subprocess, json
r=Path(__file__).resolve().parents[2]; out=Path(__file__).resolve().parent; build=out/'test-build'; build.mkdir(exist_ok=True)
exe=Path(r'D:\Grand Theft Auto San Andreas\gta-sa.exe').read_bytes()
assert hashlib.sha256(exe).hexdigest()=='72ae59e44c761389e354a50dc6215e964fe771121e2f4b1877273a493ceecc9b'
pe=struct.unpack_from('<I',exe,0x3c)[0]; n=struct.unpack_from('<H',exe,pe+6)[0]; size=struct.unpack_from('<H',exe,pe+20)[0]
def get(va,count):
 for i in range(n):
  _,v,rs,raw=struct.unpack_from('<IIII',exe,pe+24+size+40*i+8); rv=va-0x400000
  if v<=rv and rv+count<=v+rs:return exe[raw+rv-v:raw+rv-v+count]
 raise ValueError(hex(va))
source=r/'source/game_sa/Entity/Ped/PlayerPed.cpp'; text=source.read_text(); a=text.index('constexpr auto PLAYER_SPRINT_THRESHOLD'); b=text.index('// 0x60A610',a); constants=text[a:b]; a=text.index('static float __fastcall GetButtonSprintResultsImpl'); b=text.index('// 0x60A8A0',a); candidate=text[a:b]
template=r'''
#define NOMINMAX
#include <windows.h>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <float.h>
using eSprintType=uint32_t;
struct Data { char pad[28]; float m_fMoveSpeed; };
class CPlayerPed {public: char pad[0x480]; Data* data; Data* GetPlayerData(){return data;} float GetButtonSprintResults(eSprintType);};
CONSTANTS
CANDIDATE
unsigned char original[]={ORIGINAL};
unsigned char table[]={TABLE};
static float Call(void* entry,CPlayerPed* player,int sprintMode,uint32_t* observed){
 float result; uint32_t edxAfter;
 __asm {
  mov ecx,player
  push sprintMode
  mov edx,0x1234abcd
  call entry
  mov edxAfter,edx
  fstp result
 }
 *observed=edxAfter;return result;
}
int main(){
 auto code=(unsigned char*)VirtualAlloc(nullptr,4096,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);
 if(!code)return 9;
 memcpy(code,original,sizeof original);
 static float one=1.f,zero=0.f;
 for(size_t i=0;i+4<=sizeof original;++i){
  uint32_t value;memcpy(&value,code+i,4);uintptr_t replacement{};
  switch(value){case 0x8d2458:replacement=(uintptr_t)table;break;case 0x8d247c:replacement=(uintptr_t)(table+0x24);break;case 0x858624:replacement=(uintptr_t)&one;break;case 0x858b50:replacement=(uintptr_t)&zero;break;}
  if(replacement){uint32_t v=(uint32_t)replacement;memcpy(code+i,&v,4);i+=3;}
 }
 CPlayerPed p{};Data data{};p.data=&data;
 auto method=&CPlayerPed::GetButtonSprintResults; void* target{};static_assert(sizeof target==sizeof method);memcpy(&target,&method,sizeof target);
 unsigned count=0,mismatches=0;float worst=0;
 for(unsigned precision : {_PC_24,_PC_53,_PC_64}) {
  unsigned previous=_controlfp(precision,_MCW_PC);
  for(int index=0;index<10016;++index){
   uint32_t special[]={0,0x80000000,0x7f800000,0xff800000,0x7fc00000,0xffc00000,0x40a00000,0x409fffff,0x40a00001,1,0x80000001,0x7f7fffff,0xff7fffff,0x3f800000,0xbf800000,0x00800000};
   if(index<16) memcpy(&data.m_fMoveSpeed,&special[index],4); else data.m_fMoveSpeed=(index-1016)*0.01f;
   for(int type=0;type<4;++type){
    uint32_t oa,ca;float a=Call(code,&p,type,&oa);float b=Call(target,&p,type,&ca);
    if(oa!=0x1234abcd||ca!=oa){printf("EDX changed %08x %08x\n",oa,ca);return 2;}
    if(memcmp(&a,&b,4)) {++mismatches; if(mismatches<6) printf("Difference pc=%u index=%d type=%d original=%08x candidate=%08x\n",precision,index,type,*(uint32_t*)&a,*(uint32_t*)&b); if(std::isfinite(a)&&std::isfinite(b))worst=std::max(worst,std::abs(a-b));}
    ++count;
   }
  }
  _controlfp(previous,_MCW_PC);
 }
 printf("cases=%u bit_differences=%u worst_absolute=%g EDX_preserved=true\n",count,mismatches,worst);
 return mismatches ? 1 : 0;
}
'''
cpp=template.replace('CONSTANTS',constants).replace('CANDIDATE',candidate).replace('ORIGINAL',','.join(hex(b) for b in get(0x60a820,0x71))).replace('TABLE',','.join(hex(b) for b in get(0x8d2458,136)))
(build/'test.cpp').write_text(cpp)
reports=[]
for name,opt in [('debug','/Od'),('optimized','/O2'),('abi-mutation','/O2'),('math-mutation','/O2')]:
 test=cpp
 if name=='abi-mutation':test=test.replace('pop edx','pop edx\n        xor edx, edx',1)
 if name=='math-mutation':test=test.replace('fdiv PLAYER_SPRINT_THRESHOLD','fmul PLAYER_SPRINT_THRESHOLD',1)
 (build/'test.cpp').write_text(test)
 batch=build/'compile.cmd'; batch.write_text('@echo off\ncall "C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\VC\\Auxiliary\\Build\\vcvars32.bat" >nul\nif errorlevel 1 exit /b 1\n'+f'cl /nologo /EHsc /std:c++latest /W4 /WX /D_CRT_SECURE_NO_WARNINGS {opt} /MT "{build / "test.cpp"}" /Fe:"{build / name}.exe" /Fo:"{build / name}.obj" /link /DYNAMICBASE:NO /BASE:0x10000000\n')
 result=subprocess.run(['cmd.exe','/d','/c',str(batch)],cwd=build,capture_output=True,text=True);(build/(name+'-build.log')).write_text(result.stdout+result.stderr)
 if result.returncode:raise RuntimeError(result.stdout+result.stderr)
 result=subprocess.run([str(build/(name+'.exe'))],capture_output=True,text=True,timeout=30);print(name,result.returncode,result.stdout,flush=True);reports.append(dict(build=name,exitcode=result.returncode,output=result.stdout,expected_pass='mutation' not in name))
(out/'results.json').write_text(json.dumps(dict(source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),runs=reports),indent=2))
raise SystemExit(any((r['exitcode']==0)!=r['expected_pass'] for r in reports))
