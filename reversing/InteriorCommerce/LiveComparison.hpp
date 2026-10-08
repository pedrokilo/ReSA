#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorCommerceLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},calls{},rngCalls{};
alignas(16) inline std::array<uint8,64> furniture{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
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
inline const std::array<Fn,16> fns{{
{"Interior_c","Office_PlaceEdgeFillers",0x599210,0,-1,1024},
{"Interior_c","Office_PlaceDesk",0x5993e0,0,-1,1024},
{"Interior_c","Office_PlaceEdgeDesks",0x5995b0,0,-1,1024},
{"Interior_c","Office_FurnishEdges",0x599770,0,-1,1024},
{"Interior_c","Office_PlaceDeskQuad",0x599960,0,-1,1024},
{"Interior_c","Office_FurnishCenter",0x599a30,0,-1,1024},
{"Interior_c","FurnishOffice",0x599af0,0,-1,1024},
{"Interior_c","Shop_Place3PieceUnit",0x599bb0,0,-1,1024},
{"Interior_c","Shop_PlaceEdgeUnits",0x599dc0,0,-1,1024},
{"Interior_c","Shop_PlaceCounter",0x599ef0,0,-1,1024},
{"Interior_c","Shop_PlaceFixedUnits",0x59a030,0,-1,1024},
{"Interior_c","Shop_FurnishCeiling",0x59a130,0,-1,1024},
{"Interior_c","Shop_AddShelfInfo",0x59a140,0,-1,1024},
{"Interior_c","Shop_FurnishEdges",0x59a1b0,0,-1,1024},
{"Interior_c","Shop_FurnishAisles",0x59a590,0,-1,1024},
{"Interior_c","FurnishShop",0x59a790,0,-1,1024},
}};
inline constexpr int argumentCounts[16]={5,6,5,0,4,0,0,5,4,1,0,0,3,0,0,1};
inline constexpr bool hasResult[16]={true,true,true,false,true,true,false,true,true,true,false,false,false,false,false,false};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="Interior_c")for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline uint32 Invoke(uintptr_t target,Interior_c* self,const uint32* values,int count){uint32 result;__asm {
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
 call eax
 mov result,eax
}return result;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-commerce-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-commerce-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-commerce");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uint8 oldOffice=StaticRef<uint8>(0xBB3DC8);const int32 oldShelf=StaticRef<int32>(0x8D0948);
 std::array<ReversibleHooks::ReversibleHook::Base*,16> hooks{};std::array<bool,16> states{},locks{};
 for(size_t n=0;n<fns.size();++n){hooks[n]=FindHook(fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing commerce hook"},{"function",fns[n].name}});return;}states[n]=hooks[n]->Hooked();locks[n]=hooks[n]->Locked();hooks[n]->LockState(false);hooks[n]->State(false);}
 uint32 pairs=0,failures=0;bool restored=true;
 for(size_t n=0;n<fns.size();++n){const auto& fn=fns[n];uint32 bad=0;std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
 auto patch=[&](uintptr_t address,auto callback){if(address!=fn.address)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,callback));};
patch(0x821B1E,Random);patch(0x407180,RandomRange);
patch(0x591920,D591920);
patch(0x5911E0,D5911E0);
patch(0x591E40,D591E40);
patch(0x592AA0,D592AA0);
patch(0x591700,D591700);
patch(0x5993E0,D5993E0);
patch(0x591D20,D591D20);
patch(0x5995B0,D5995B0);
patch(0x599210,D599210);
patch(0x599960,D599960);
patch(0x591220,D591220);
patch(0x599770,D599770);
patch(0x599A30,D599A30);
patch(0x59A130,D59A130);
patch(0x599BB0,D599BB0);
patch(0x599EF0,D599EF0);
patch(0x599DC0,D599DC0);
patch(0x5918E0,D5918E0);
patch(0x59A140,D59A140);
patch(0x59A030,D59A030);
patch(0x59A1B0,D59A1B0);
patch(0x59A590,D59A590);
  alignas(16) std::array<uint8,sizeof(Interior_c)> object{};alignas(16) std::array<uint8,64> box{};
  for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=uint32(i);calls=0;rngCalls=0;trace=json::array();object.fill(0);box.fill(0);furniture.fill(0);auto* self=reinterpret_cast<Interior_c*>(object.data());self->m_box=reinterpret_cast<tEffectInterior*>(box.data());
   box[2]=uint8(5+i%24);box[3]=uint8(5+(i/3)%24);box[5]=uint8(i%12);box[0x1F]=uint8(i%5);*reinterpret_cast<int16*>(furniture.data()+10)=int16(i%8);self->field_3EC=int16(box[2]);self->field_3EE=int16(box[3]);self->m_furnitureGroupId=int8(i%9);self->m_furnitureId=int8(i%8);self->field_792=int8(i%7);self->m_interiorInfosCount=int8(i%8);
   StaticRef<uint8>(0xBB3DC8)=uint8(i%2);StaticRef<int32>(0x8D0948)=i%6;
   std::array<uint32,6> args{uint32(i%7),uint32(2+i%9),uint32(2+(i/7)%9),uint32(i%4),uint32(2+i%4),uint32(i%8)};
   if(n==0){args[0]=i%2?0xFFFFFFFFu:uint32(i%7);args[3]=uint32(i%4);}
   if(n==1){args[2]=uint32(i%4);args[4]=uint32(i%2);}
   if(n==2){args[3]=uint32(i%4);args[4]=uint32((i/4)%4);}
   if(n==7){args[3]=uint32(i%4);args[4]=uint32(2+i%4);}
   if(n==8)args[3]=uint32(i%4);
   if(n==9||n==15)args[0]=uint32(i%2);
   hooks[n]->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const auto result=Invoke(fn.address,self,args.data(),argumentCounts[n]);self->m_box=nullptr;
   return json{{"switch",switched},{"return",hasResult[n]?result:0},{"object",Hash(object.data(),object.size())},{"object_bytes",[&]{json bytes=json::array();for(auto b:object)bytes.push_back(uint32(b));return bytes;}()},{"box",Hash(box.data(),box.size())},{"furniture",Hash(furniture.data(),furniture.size())},{"office_global",uint32(StaticRef<uint8>(0xBB3DC8))},{"shelf_global",StaticRef<int32>(0x8D0948)},{"trace",trace},{"rng_calls",rngCalls}};
  };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);bad+=!equal;Write({{"event","comparison"},{"pair",pairs++},{"function",std::string("Interior_c::")+fn.name},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
  std::vector<std::pair<uintptr_t,std::array<uint8,5>>> codeBefore;for(const auto& patch:patches)codeBefore.emplace_back(patch->address,patch->saved);patches.clear();for(const auto& entry:codeBefore)restored&=std::memcmp(reinterpret_cast<const void*>(entry.first),entry.second.data(),5)==0;
  hooks[n]->State(false);failures+=bad;Write({{"event","function_summary"},{"function",std::string("Interior_c::")+fn.name},{"cases",fn.cases},{"failures",bad}});
 }
 for(size_t n=0;n<fns.size();++n){hooks[n]->State(states[n]);hooks[n]->LockState(locks[n]);restored&=hooks[n]->Hooked()==states[n]&&hooks[n]->Locked()==locks[n];}StaticRef<uint8>(0xBB3DC8)=oldOffice;StaticRef<int32>(0x8D0948)=oldShelf;restored&=StaticRef<uint8>(0xBB3DC8)==oldOffice&&StaticRef<int32>(0x8D0948)==oldShelf;
 Write({{"event",failures||!restored?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",restored}});log.close();
}
}
