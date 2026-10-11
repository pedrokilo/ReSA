#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace CppInteriorProposalLive {
alignas(16) inline std::array<uint8,108> canonicalFP{};
inline uint32 canonicalMXCSR{};inline bool canonicalActive{},abiOK{};inline uint32 rawEAX{},rawECX{},rawEDX{};inline bool flagsOK{};
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 MXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void RestoreNumeric(){if(canonicalActive){RestoreFP(canonicalFP.data());const uint32 value=canonicalMXCSR;__asm{ldmxcsr value}}}
inline bool NumericRestored(){alignas(16)std::array<uint8,108> v{};SaveFP(v.data());return v==canonicalFP&&MXCSR()==canonicalMXCSR;}
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},calls{},rngCalls{};
alignas(16) inline std::array<uint8,64> furniture{};
inline void Write(const json& r){RestoreNumeric();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();RestoreNumeric();}
inline std::string Bytes(const void* p,size_t n){const auto* b=static_cast<const uint8*>(p);constexpr char d[]="0123456789abcdef";std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=d[b[i]>>4];out+=d[b[i]&15];}return out;}
inline uintptr_t Resolve(uintptr_t p){for(unsigned i=0;i<16&&*reinterpret_cast<const uint8*>(p)==0xE9;++i)p+=5+*reinterpret_cast<const int32*>(p+1);return p;}
inline uintptr_t ModuleOf(uintptr_t p){MEMORY_BASIC_INFORMATION info{};return VirtualQuery(reinterpret_cast<const void*>(p),&info,sizeof(info))==sizeof(info)?uintptr_t(info.AllocationBase):0;}
inline void ModuleMarker(){}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline uint32 Dispatch(uint32 address,void* self,std::initializer_list<uint32> args){
 ++calls;json a=json::array();size_t index=0;
 for(uint32 value:args){if(address==0x592AA0&&(index==6||index==7))a.push_back(index==6?"outX":"outY");else a.push_back(value);++index;}
 trace.push_back({address,a});auto it=args.begin();
 if(address==0x5911E0)return uint32(uintptr_t(furniture.data()));
 if(address==0x591220)return scenario%7;
 if(address==0x591920)return scenario%9;
 if(address==0x5918E0)return (scenario+it[0]+it[1])%5;
 if(address==0x592AA0){*reinterpret_cast<int32*>(uintptr_t(it[6]))=1+int32((scenario+calls)%3);*reinterpret_cast<int32*>(uintptr_t(it[7]))=1+int32((scenario+calls+1)%3);trace.push_back({"outputs",it[6]==it[7],*reinterpret_cast<int32*>(uintptr_t(it[6])),*reinterpret_cast<int32*>(uintptr_t(it[7]))});return scenario%4==0?0:uint32(uintptr_t(furniture.data()));}
 if(address==0x591E40){auto* b=static_cast<uint8*>(self);b[0x40D]=uint8(b[0x40D]+1);return scenario%3!=0;}
 if(address==0x591700){auto* b=static_cast<uint8*>(self);const auto x=it[0]%30,y=it[1]%30;b[0x68+x*30+y]=uint8(it[4]);return 0;}
 return 1+(scenario+calls)%3;
}
inline int __cdecl Random(){const auto value=int((scenario*7919u+rngCalls++*14033u)%32768u);trace.push_back({"rand",value});return value;}
inline int __cdecl RandomRange(int low,int high){trace.push_back({"range",low,high});return low+int(scenario%uint32(std::max(1,high-low)));}
inline uint32 __fastcall D591920(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3) {return Dispatch(0x591920,self,{a0,a1,a2,a3});}
inline uint32 __fastcall D5911E0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3) {return Dispatch(0x5911E0,self,{a0,a1,a2,a3});}
inline uint32 __fastcall D591E40(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) {return Dispatch(0x591E40,self,{a0,a1,a2,a3,a4});}
inline uint32 __fastcall D592AA0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4,uint32 a5,uint32 a6,uint32 a7,uint32 a8) {return Dispatch(0x592AA0,self,{a0,a1,a2,a3,a4,a5,a6,a7,a8});}
inline uint32 __fastcall D591700(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4,uint32 a5) {return Dispatch(0x591700,self,{a0,a1,a2,a3,a4,a5});}
inline uint32 __fastcall D5993E0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4,uint32 a5) {return Dispatch(0x5993E0,self,{a0,a1,a2,a3,a4,a5});}
inline uint32 __fastcall D591D20(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3) {return Dispatch(0x591D20,self,{a0,a1,a2,a3});}
inline uint32 __fastcall D5995B0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) {return Dispatch(0x5995B0,self,{a0,a1,a2,a3,a4});}
inline uint32 __fastcall D599210(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) {return Dispatch(0x599210,self,{a0,a1,a2,a3,a4});}
inline uint32 __fastcall D599960(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3) {return Dispatch(0x599960,self,{a0,a1,a2,a3});}
inline uint32 __fastcall D591220(void* self,void*,uint32 a0,uint32 a1,uint32 a2) {return Dispatch(0x591220,self,{a0,a1,a2});}
inline uint32 __fastcall D599770(void* self,void*) {return Dispatch(0x599770,self,{});}
inline uint32 __fastcall D599A30(void* self,void*) {return Dispatch(0x599A30,self,{});}
inline uint32 __fastcall D59A130(void* self,void*) {return Dispatch(0x59A130,self,{});}
inline uint32 __fastcall D599BB0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) {return Dispatch(0x599BB0,self,{a0,a1,a2,a3,a4});}
inline uint32 __fastcall D599EF0(void* self,void*,uint32 a0) {return Dispatch(0x599EF0,self,{a0});}
inline uint32 __fastcall D599DC0(void* self,void*,uint32 a0,uint32 a1,uint32 a2,uint32 a3) {return Dispatch(0x599DC0,self,{a0,a1,a2,a3});}
inline uint32 __fastcall D5918E0(void* self,void*,uint32 a0,uint32 a1) {return Dispatch(0x5918E0,self,{a0,a1});}
inline uint32 __fastcall D59A140(void* self,void*,uint32 a0,uint32 a1,uint32 a2) {return Dispatch(0x59A140,self,{a0,a1,a2});}
inline uint32 __fastcall D59A030(void* self,void*) {return Dispatch(0x59A030,self,{});}
inline uint32 __fastcall D59A1B0(void* self,void*) {return Dispatch(0x59A1B0,self,{});}
inline uint32 __fastcall D59A590(void* self,void*) {return Dispatch(0x59A590,self,{});}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,2> fns{{
{"Interior_c","Shop_FurnishCeiling",0x59A130,0,-1,2048},
{"Interior_c","Office_FurnishCenter",0x599A30,0,-1,4096},
}};
inline constexpr int argumentCounts[2]={0,0};
inline constexpr bool hasResult[2]={false,true};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="Interior_c")for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline uint32 Invoke(uintptr_t target,Interior_c* self,const uint32* values,int count){uint32 result,ecxResult{},edxResult{},flagsBefore{},flagsAfter{},before{},after{},eb{},esiBefore{},ediBefore{},ebpBefore{},ebAfter{},esiAfter{},ediAfter{},ebpAfter{};__asm {
 mov before,esp
 mov eb,ebx
 mov esiBefore,esi
 mov ediBefore,edi
 mov ebpBefore,ebp
 mov eax,count
 mov edx,values
 test eax,eax
 jz commerce_invoke_call
 commerce_invoke_push:
 dec eax
 push dword ptr [edx+eax*4]
 test eax,eax
 jnz commerce_invoke_push
 commerce_invoke_call:
 mov ecx,self
 mov eax,target
 mov edx,0C1234567h
 pushfd
 pop flagsBefore
 call eax
 pushfd
 pop flagsAfter
 mov result,eax
 mov ecxResult,ecx
 mov edxResult,edx
 mov after,esp
 mov ebAfter,ebx
 mov esiAfter,esi
 mov ediAfter,edi
 mov ebpAfter,ebp
}flagsOK=flagsBefore==flagsAfter;rawEAX=result;rawECX=ecxResult;rawEDX=edxResult;abiOK=before==after&&eb==ebAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter;return result;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cpp-interior-proposal-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cpp-interior-proposal-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpp-interior-proposal");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;SaveFP(canonicalFP.data());canonicalMXCSR=MXCSR();canonicalActive=true;
 const uint8 oldOffice=StaticRef<uint8>(0xBB3DC8);const int32 oldShelf=StaticRef<int32>(0x8D0948);
 std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> states{},locks{};
 for(size_t n=0;n<fns.size();++n){hooks[n]=FindHook(fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing commerce hook"},{"function",fns[n].name}});return;}states[n]=hooks[n]->Hooked();locks[n]=hooks[n]->Locked();}
 for(auto* h:hooks){h->LockState(false);h->State(false);}
 uint32 pairs=0,failures=0;bool restored=true;
 for(size_t n=0;n<fns.size();++n){const auto& fn=fns[n];uint32 bad=0;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
 auto patch=[&](uintptr_t address,auto callback){if(address!=fn.address)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,callback));};
if(fn.address==0x599A30){
 patch(0x599960,D599960);
 patch(std::bit_cast<uintptr_t>(&Interior_c::Office_PlaceDeskQuad),D599960);
}
  alignas(16) std::array<uint8,sizeof(Interior_c)> object{};alignas(16) std::array<uint8,64> box{};
  for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=uint32(i);calls=0;rngCalls=0;trace=json::array();object.fill(0);box.fill(0);furniture.fill(0);auto* self=reinterpret_cast<Interior_c*>(object.data());self->m_box=reinterpret_cast<tEffectInterior*>(box.data());
   box[2]=uint8(i%256);box[3]=uint8((i/256)*17+i%17);box[5]=uint8(i%12);box[0x1F]=uint8(i%5);*reinterpret_cast<int16*>(furniture.data()+10)=int16(i%8);self->field_3EC=int16(box[2]);self->field_3EE=int16(box[3]);self->m_furnitureGroupId=int8(i%9);self->m_furnitureId=int8(i%256);self->field_792=int8(i%7);self->m_interiorInfosCount=int8(i%8);
   StaticRef<uint8>(0xBB3DC8)=uint8(i%2);StaticRef<int32>(0x8D0948)=i%6;
   std::array<uint32,6> args{uint32(i%7),uint32(2+i%9),uint32(2+(i/7)%9),uint32(i%4),uint32(2+i%4),uint32(i%8)};
   auto expectedObject=object;std::fill(expectedObject.begin()+0x14,expectedObject.begin()+0x18,0);const auto expectedBox=box;const auto expectedFurniture=furniture;hooks[n]->State(candidate);const uintptr_t raw=fn.address==0x59A130?std::bit_cast<uintptr_t>(&Interior_c::Shop_FurnishCeiling):std::bit_cast<uintptr_t>(&Interior_c::Office_FurnishCenter);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate&&(candidate?Resolve(fn.address)==Resolve(raw)&&ModuleOf(Resolve(raw))==ModuleOf(uintptr_t(&ModuleMarker)):Resolve(fn.address)==fn.address&&ModuleOf(fn.address)==uintptr_t(GetModuleHandleA(nullptr)));RestoreNumeric();const auto result=Invoke(fn.address,self,args.data(),argumentCounts[n]);alignas(16)std::array<uint8,108> postFP{};SaveFP(postFP.data());const auto postMXCSR=MXCSR();const bool bindingsPreserved=self->m_box==reinterpret_cast<tEffectInterior*>(box.data());self->m_box=nullptr;
   RestoreNumeric();return json{{"switch",switched},{"bindings_preserved",bindingsPreserved},{"readonly_ok",object==expectedObject&&box==expectedBox&&furniture==expectedFurniture},{"abi_ok",abiOK},{"ceiling_flags_preserved",fn.address!=0x59A130||flagsOK},{"ceiling_volatile",fn.address==0x59A130?json::array({rawEAX,rawECX,rawEDX}):json::array()},{"post_fp",Bytes(postFP.data(),postFP.size())},{"post_mxcsr",postMXCSR},{"numeric_restored",NumericRestored()},{"return",hasResult[n]?result:0},{"object",Hash(object.data(),object.size())},{"object_bytes",[&]{json bytes=json::array();for(auto b:object)bytes.push_back(uint32(b));return bytes;}()},{"box",Hash(box.data(),box.size())},{"box_bytes",Bytes(box.data(),box.size())},{"furniture",Hash(furniture.data(),furniture.size())},{"office_global",uint32(StaticRef<uint8>(0xBB3DC8))},{"shelf_global",StaticRef<int32>(0x8D0948)},{"trace",trace},{"rng_calls",rngCalls}};
  };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false)&&original.value("bindings_preserved",false)&&original.value("readonly_ok",false)&&original.value("abi_ok",false)&&original.value("numeric_restored",false)&&original.value("ceiling_flags_preserved",false);bad+=!equal;Write({{"event","comparison"},{"pair",pairs++},{"function",std::string("Interior_c::")+fn.name},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
  std::vector<std::pair<uintptr_t,std::array<uint8,5>>> codeBefore;for(const auto& patch:patches)codeBefore.emplace_back(patch->address,patch->saved);patches.clear();for(const auto& entry:codeBefore)restored&=std::memcmp(reinterpret_cast<const void*>(entry.first),entry.second.data(),5)==0;
  hooks[n]->State(false);failures+=bad;Write({{"event","function_summary"},{"function",std::string("Interior_c::")+fn.name},{"cases",fn.cases},{"failures",bad}});
 }
 for(size_t n=0;n<fns.size();++n){hooks[n]->State(states[n]);hooks[n]->LockState(locks[n]);restored&=hooks[n]->Hooked()==states[n]&&hooks[n]->Locked()==locks[n];}StaticRef<uint8>(0xBB3DC8)=oldOffice;StaticRef<int32>(0x8D0948)=oldShelf;restored&=StaticRef<uint8>(0xBB3DC8)==oldOffice&&StaticRef<int32>(0x8D0948)==oldShelf;
 RestoreNumeric();restored&=NumericRestored();Write({{"event",failures||!restored?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",restored}});RestoreNumeric();canonicalActive=false;log.close();
}
}
