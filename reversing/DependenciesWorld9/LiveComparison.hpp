#pragma once
#include <fstream>
#include "WaterLevel.h"
#include "Tasks/TaskTypes/SeekEntity/TaskComplexSeekEntityXYOffset.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld9Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CWaterLevel","CalculateWavesOnlyForCoordinate2Native",0x6E7210,0,-1,2048},
{"CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>","SetMaxEntityDist2D",0x4BC470,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r);
struct Result{uint32 value,ecx,edx;std::array<uint32,5> slots;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t object,const std::array<uintptr_t,5>& args,bool setter){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},rv{},ecxCaptured{},edxCaptured{};std::array<uint32,5> slots{};auto* copied=slots.data();const auto* p=args.data();
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  mov eax,p
  cmp setter,0
  jne setDistance
  push dword ptr [eax+16]
  push dword ptr [eax+12]
  push dword ptr [eax+8]
  push dword ptr [eax+4]
  push dword ptr [eax]
  call entry
  mov rv,eax
  mov ecxCaptured,ecx
  mov edxCaptured,edx
  mov edx,copied
  mov eax,dword ptr [esp]
  mov dword ptr [edx],eax
  mov eax,dword ptr [esp+4]
  mov dword ptr [edx+4],eax
  mov eax,dword ptr [esp+8]
  mov dword ptr [edx+8],eax
  mov eax,dword ptr [esp+12]
  mov dword ptr [edx+12],eax
  mov eax,dword ptr [esp+16]
  mov dword ptr [edx+16],eax
  add esp,20
  jmp finished
 setDistance:
  push dword ptr [eax]
  mov ecx,object
  xor edx,edx
  call entry
  mov rv,eax
  mov ecxCaptured,ecx
  mov edxCaptured,edx
 finished:
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {rv,ecxCaptured,edxCaptured,slots,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
alignas(16) inline std::array<uint8,108> canonicalFP{};inline uint32 canonicalMXCSR{};inline bool canonicalActive{};
inline void RestoreCanonical(){if(canonicalActive){RestoreFP(canonicalFP.data());WriteMXCSR(canonicalMXCSR);}}
inline bool CanonicalRestored(){alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==canonicalFP&&ReadMXCSR()==canonicalMXCSR;}
inline void Write(const json& r){alignas(16)std::array<uint8,108> current{};const auto mx=ReadMXCSR();if(!canonicalActive)SaveFP(current.data());RestoreCanonical();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();if(canonicalActive)RestoreCanonical();else{RestoreFP(current.data());WriteMXCSR(mx);}}
inline uintptr_t ResolveCRT(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ExeModule(uintptr_t e){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&owner==GetModuleHandleA(nullptr);}
inline bool ActualCRT(){return ExeModule(0x821B40)&&ExeModule(ResolveCRT(0x821B40))&&!ReversingProbe::JumpsToDll(0x821B40);}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesworld9-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld9");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 alignas(16)std::array<uint8,108> fpSaved{};SaveFP(fpSaved.data());const auto controlSaved=_controlfp(0,0),mxcsrSaved=ReadMXCSR();const auto topSaved=Top();canonicalFP=fpSaved;canonicalMXCSR=mxcsrSaved;canonicalActive=true;
 const auto crtEntrySaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x821B40),64);const auto crtCore=ResolveCRT(0x821B40);const auto crtCoreSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(crtCore),64);
 auto crtUnchanged=[&]{return ActualCRT()&&ResolveCRT(0x821B40)==crtCore&&crtEntrySaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x821B40),64)&&crtCoreSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(crtCore),64);};
 static_assert(sizeof(CTaskComplexSeekEntityXYOffset)==0x58);
 const auto timerSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB84),4),waterTimeSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC228A4),4),weatherSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC812E8),4);
 const auto sineSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3E00),1024),gainSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D38C8),64);
 auto readonly=[&]{return sineSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3E00),1024)&&gainSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D38C8),64);};
 bool tablesReal=true;for(int n=0;n<256;++n){const float v=reinterpret_cast<const float*>(0xBB3E00)[n];tablesReal&=std::isfinite(v)&&v>=-1.01f&&v<=1.01f;}tablesReal&=reinterpret_cast<const float*>(0xBB3E00)[64]>.9f;

 auto restore=[&]{std::memcpy(reinterpret_cast<void*>(0xB7CB84),timerSaved.data(),4);std::memcpy(reinterpret_cast<void*>(0xC228A4),waterTimeSaved.data(),4);std::memcpy(reinterpret_cast<void*>(0xC812E8),weatherSaved.data(),4);RestoreFP(fpSaved.data());WriteMXCSR(mxcsrSaved);};
 std::array<std::unique_ptr<OriginalDependencies499::HookRoute>,2> routes;
 routes[0]=std::make_unique<OriginalDependencies499::HookRoute>(fns[0].cls,fns[0].name,fns[0].address,&CWaterLevel::CalculateWavesOnlyForCoordinate2Native);
 routes[1]=std::make_unique<OriginalDependencies499::HookRoute>(fns[1].cls,fns[1].name,fns[1].address,FunctionToVoidPtr(&SetMaxEntityDist2DRegs));
 Write({{"event","begin"},{"registered_hook",routes[0]->Registered()&&routes[1]->Registered()},{"actual_crt_library",crtUnchanged()},{"crt_entry",0x821B40},{"crt_core",crtCore},{"crt_entry_bytes",crtEntrySaved},{"crt_core_bytes",crtCoreSaved},{"loaded_sine_table",tablesReal},{"collaborators_stubbed",false},{"game_collaborators",0}});
 bool passed=tablesReal;uint32 pair=0;
 for(int index=0;index<2;++index){bool changed=false,unchanged=false,positive=false,negative=false;uint32 failures=0;
  for(int scenario=0;scenario<2048;++scenario){
   auto run=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);const auto topBefore=Top();std::array<uintptr_t,5>args{};
    alignas(4)std::array<uint8,0x60>arena{};arena.fill(0xA7);std::array<uint32,3>height{0x13579BDF,std::bit_cast<uint32>(3.125f),0x2468ACE0};CTaskComplexSeekEntityXYOffset* task=nullptr;uint32 oldBits=0,newBits=0,tickFixture=0,elapsedFixture=0;int waveWeather=0,bigAmp=0,smallAmp=0;bool update=false;int32 x=0,y=0;
    if(index==0){const int32 coords[]{-32768,-3001,-17,-16,-15,-2,-1,0,1,2,15,16,17,3001,32767,100000};x=coords[scenario%16];y=coords[(scenario/16)%16];const float amps[]{0,.125f,1,2};const float weather[]{0,.25f,1,2};const uint32 elapsed[]{0,2999,3000,3499,3500,4999,5000,123456};
     elapsedFixture=elapsed[(scenario/256)%8];waveWeather=(scenario/512)%4;bigAmp=(scenario/64)%4;smallAmp=(scenario/256)%4;*reinterpret_cast<uint32*>(0xB7CB84)=*reinterpret_cast<uint32*>(0xC228A4)+elapsedFixture;*reinterpret_cast<float*>(0xC812E8)=weather[waveWeather];
     args={uint32(x),uint32(y),std::bit_cast<uint32>(amps[(scenario/64)%4]),std::bit_cast<uint32>(amps[(scenario/256)%4]),uintptr_t(&height[1])};
    }else{
     // Genuine source object with null optional target; setter never reads target or vtable.
     task=::new(static_cast<void*>(arena.data()+4)) CTaskComplexSeekEntityXYOffset(nullptr,1000,500,1.f,.5f,1.f,false,false);
     const uint32 values[]{0,0x80000000,0x3F800000,0x40000000,0xBF800000,0x3F800001,0x7FC12345,0x41200000};oldBits=values[scenario%8];newBits=values[(scenario/8)%8];std::memcpy(arena.data()+4+0x18,&oldBits,4);*reinterpret_cast<uint32*>(arena.data()+4+0x28)=0x11223344;*reinterpret_cast<uint32*>(arena.data()+4+0x2C)=0x55667788;arena[4+0x30]=uint8(scenario&1);
     const float oldValue=std::bit_cast<float>(oldBits),newValue=std::bit_cast<float>(newBits);update=oldValue!=newValue;args[0]=newBits;
    }
    const auto before=arena;if(index==1){const uint32 ticks[]{0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFE,0xFFFFFFFF};tickFixture=ticks[(scenario/64)%6];*reinterpret_cast<uint32*>(0xB7CB84)=tickFixture;}
    const uint32 timer=*reinterpret_cast<uint32*>(0xB7CB84);bool route=false;const bool libraryBefore=crtUnchanged();const auto result=routes[index]->Invoke(candidate,[&]{route=ReversingProbe::JumpsToDll(fns[index].address)==candidate;return Call(fns[index].address,uintptr_t(task),args,index==1);});
    bool contract=true;if(index==1){auto expected=before;if(update){std::memcpy(expected.data()+4+0x18,&newBits,4);std::memcpy(expected.data()+4+0x28,&timer,4);uint32 zero=0;std::memcpy(expected.data()+4+0x2C,&zero,4);expected[4+0x30]=1;}contract=arena==expected;}
    const bool fpOK=Top()==topBefore&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(_PC_24|_RC_NEAR);const auto output=index==0?ReversingProbe::Bytes(&height[1],4):ReversingProbe::Bytes(arena.data()+4,sizeof(CTaskComplexSeekEntityXYOffset));
    const bool guard=std::all_of(arena.begin(),arena.begin()+4,[](uint8 v){return v==0xA7;})&&std::all_of(arena.end()-4,arena.end(),[](uint8 v){return v==0xA7;})&&height.front()==0x13579BDF&&height.back()==0x2468ACE0;
    if(task)task->~CTaskComplexSeekEntityXYOffset();
    const bool libraryOK=libraryBefore&&crtUnchanged();restore();alignas(16)std::array<uint8,108> fpCheck{};SaveFP(fpCheck.data());const bool fpRestored=fpCheck==fpSaved&&ReadMXCSR()==mxcsrSaved;const bool globalsRestored=timerSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB84),4)&&waterTimeSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC228A4),4)&&weatherSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC812E8),4);
    json value={{"return_bits",result.value},{"raw_ecx",result.ecx},{"raw_edx",result.edx},{"argument_slots_after",result.slots},{"fp_state_restored",fpRestored},{"globals_restored",globalsRestored},{"output",output},{"x",x},{"y",y},{"elapsed",elapsedFixture},{"weather_index",waveWeather},{"big_amp_index",bigAmp},{"small_amp_index",smallAmp},{"ticks",tickFixture},{"old_bits",oldBits},{"new_bits",newBits},{"distance_changed",update},{"route_ok",route},{"library_route_ok",libraryOK},{"tables_real",tablesReal},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"fp_preserved",fpOK},{"contract_ok",contract},{"readonly_ok",readonly()},{"guards_ok",guard}};restore();return value;
   };
   const auto original=run(false),candidate=run(true);bool equal=routes[index]->Registered()&&original==candidate;for(auto k:{"route_ok","library_route_ok","fp_state_restored","globals_restored","tables_real","stack_restored","registers_preserved","fp_preserved","contract_ok","readonly_ok","guards_ok"})equal&=original[k]==true;
   changed|=original["distance_changed"]==true;unchanged|=original["distance_changed"]==false;positive|=original["x"].get<int32>()>0;negative|=original["x"].get<int32>()<0;failures+=!equal;passed&=equal;Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fns[index].cls)+"::"+fns[index].name},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
  }
  const bool covered=index==0?positive&&negative:changed&&unchanged;passed&=covered;Write({{"event","function_summary"},{"function",std::string(fns[index].cls)+"::"+fns[index].name},{"cases",2048},{"failures",failures},{"complete_paths",covered}});
 }
 restore();bool restored=crtUnchanged()&&readonly()&&timerSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CB84),4)&&waterTimeSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC228A4),4)&&weatherSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC812E8),4)&&Top()==topSaved&&ReadMXCSR()==mxcsrSaved&&(_controlfp(0,0)&(_MCW_PC|_MCW_RC))==(controlSaved&(_MCW_PC|_MCW_RC));for(auto&r:routes)restored&=r->Restore();const bool finalFP=CanonicalRestored();restored&=finalFP;Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"fp_state_restored",finalFP},{"crt_unchanged",crtUnchanged()}});
}
}
