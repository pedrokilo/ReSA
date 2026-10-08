#pragma once
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/Interior_c.h"
#include "Interior/InteriorGroup_c.h"
#include "Interior/InteriorManager_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorLifecycleRuntimeLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{};
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i)h=(h^static_cast<const uint8*>(p)[i])*1099511628211ull;return h;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline void __fastcall Centre(Interior_c*,void*,float x,float y,CVector* out){trace.push_back({"centre",F(x),F(y)});*out=CVector{x+.125f,y-.25f,float(scenario%13)*.1f};}
inline void __fastcall Bounding(Interior_c*,void*,int32 x,int32 y,int32* loX,int32* hiX,int32* loY,int32* hiY,int32* visited){trace.push_back({"bounding",x,y,*loX,*hiX,*loY,*hiY,visited[x*30+y]});*loX=x-int32(scenario%3);*hiX=x+int32(scenario%4);*loY=y-int32(scenario%2);*hiY=y+int32(scenario%5);visited[x*30+y]=2;}
inline RwV3d* __cdecl Transform(RwV3d* out,const RwV3d* in,int32 count,const RwMatrix* m){trace.push_back({"transform",F(in->x),F(in->y),F(in->z),count,Hash(m,64),out==in});const auto v=*in;*out=RwV3d{v.y,-v.x,v.z};return out;}
inline void __cdecl Remove(float x0,float x1,float y0,float y1,float z0,float z1){trace.push_back({"remove",F(x0),F(x1),F(y0),F(y1),F(z0),F(z1)});}
inline void __fastcall Unfurnish(Interior_c*,void*){trace.push_back({"unfurnish"});}
inline void __fastcall Shop(Interior_c*,void*,uint8 mode){trace.push_back({"shop",mode});}
inline void __fastcall Office(Interior_c*,void*){trace.push_back({"office"});}
inline void __fastcall Lounge(Interior_c*,void*){trace.push_back({"lounge"});}
inline void __fastcall Bedroom(Interior_c*,void*){trace.push_back({"bedroom"});}
inline void __fastcall Kitchen(Interior_c*,void*){trace.push_back({"kitchen"});}
alignas(16)inline std::array<uint8,sizeof(CPed)> pedBytes{};
alignas(16)inline std::array<uint8,sizeof(CObject)> objectBytes{};
alignas(16)inline std::array<uint8,sizeof(CEntity)> entityBytes{};
inline InteriorGroup_c group{};inline CMatrix matrix{};inline std::array<uintptr_t,1> vt{};
inline uint32 randomCount{};
inline int __cdecl Random(){const auto v=int((scenario*7919+randomCount++*12347)%32768);trace.push_back({"rand",v});return v;}
inline void __cdecl Seed(uint32 seed){trace.push_back({"seed",seed});}
inline CEntity* __fastcall GroupEntity(InteriorGroup_c*,void*){trace.push_back({"entity"});return reinterpret_cast<CEntity*>(entityBytes.data());}
inline void __fastcall Calc(Interior_c* self,void*,CVector* p){trace.push_back({"calc",F(p->x),F(p->y),F(p->z)});self->m_position=*p;}
inline void __fastcall Reset(Interior_c* self,void*){trace.push_back({"reset"});std::fill(reinterpret_cast<uint8*>(self)+0x68,reinterpret_cast<uint8*>(self)+0x3EC,uint8(scenario%4));}
inline void __fastcall Exits(Interior_c*,void*){trace.push_back({"exits"});}
inline void __fastcall Pickups(Interior_c*,void*){trace.push_back({"pickups"});}
inline bool __fastcall Had(InteriorManager_c*,void*,Interior_c*){trace.push_back({"had",scenario%2});return scenario%2!=0;}
inline bool __fastcall Inside(Interior_c*,void*,const CVector& point,CVector bias){trace.push_back({"inside",F(point.x),F(point.y),F(point.z),F(bias.x),F(bias.y),F(bias.z)});return scenario%4==0;}
inline CPed* __cdecl Player(int32 index){trace.push_back({"player",index});return scenario%5==0?nullptr:reinterpret_cast<CPed*>(pedBytes.data());}
inline CEntity* __fastcall Holding(CPed*,void*){trace.push_back({"holding"});return scenario%5==1?nullptr:scenario%5==2?reinterpret_cast<CEntity*>(entityBytes.data()):reinterpret_cast<CEntity*>(objectBytes.data());}
inline void __cdecl WorldRemove(CEntity* e){trace.push_back({"world_remove",e==reinterpret_cast<CEntity*>(objectBytes.data())?1:e==nullptr?0:uintptr_t(e)});}
inline void* __fastcall Delete(CEntity* e,void*,uint32 flags){trace.push_back({"delete",e==reinterpret_cast<CEntity*>(objectBytes.data())?1:uintptr_t(e),flags});return e;}
inline int32 __cdecl ModelPickup(CVector v,uint32 model,ePickupType type,uint32 ammo,uint32 money,bool empty,char* message){trace.push_back({"model",F(v.x),F(v.y),F(v.z),model,int(type),ammo,money,empty,uintptr_t(message)});return -1;}
inline int32 __cdecl WeaponPickup(CVector v,eWeaponType weapon,ePickupType type,uint32 ammo,bool empty,char* message){trace.push_back({"weapon",F(v.x),F(v.y),F(v.z),int(weapon),int(type),ammo,empty,uintptr_t(message)});return -1;}
inline tPickupReference __cdecl ModelPickupDll(CVector v,uint32 model,ePickupType type,uint32 ammo,uint32 money,bool empty,char* message){return tPickupReference(ModelPickup(v,model,type,ammo,money,empty,message));}
inline tPickupReference __cdecl WeaponPickupDll(CVector v,eWeaponType weapon,ePickupType type,uint32 ammo,bool empty,char* message){return tPickupReference(WeaponPickup(v,weapon,type,ammo,empty,message));}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,4> fns{{
{"Interior_c","Init",0x593BF0,0,-1,1024},
{"Interior_c","Unfurnish",0x5915D0,0,-1,1024},
{"Interior_c","IsVisible",0x5929F0,0,-1,1024},
{"Interior_c","AddPickups",0x591F90,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="Interior_c")for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline uint32 Invoke(uintptr_t target,Interior_c* self,const uint32* values,int count){uint32 result;__asm {
 mov eax,count
 mov edx,values
 test eax,eax
 jz lifecycle_invoke_call
 lifecycle_invoke_push:
 dec eax
 push dword ptr [edx+eax*4]
 test eax,eax
 jnz lifecycle_invoke_push
 lifecycle_invoke_call:
 mov ecx,self
 mov eax,target
 call eax
 mov result,eax
}return result;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"interior-lifecycle-runtime-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"interior-lifecycle-runtime-runtime.jsonl");ReversingProbe::InstallFailureCapture("interior-lifecycle-runtime");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;uint32 pairs{},failures{};bool restored=true;
 const auto oldPool=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBAD3EC),12);const auto oldCamera=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F02C),20);const auto oldTime=StaticRef<uint32>(0xB7CB84);const auto oldPickupTime=StaticRef<uint32>(0xBB3DC4);const auto oldSteals=StaticRef<uint16>(0xBB4A70);const auto oldCount=StaticRef<int32>(0xBB3914);const auto oldIds=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3918),256);const auto oldSeedPtr=StaticRef<uintptr_t>(0xBB3DAC);
 for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;auto both=[&](uintptr_t a,uintptr_t b,auto cb){patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(a,cb));if(a!=b)patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(b,cb));};
 if(fn.address==0x593BF0){both(0x5914D0,std::bit_cast<uintptr_t>(&Interior_c::CalcMatrix),Calc);both(0x593910,std::bit_cast<uintptr_t>(&Interior_c::ResetTiles),Reset);both(0x594BD0,std::bit_cast<uintptr_t>(&InteriorGroup_c::GetEntity),GroupEntity);both(0x821B11,uintptr_t(&std::srand),Seed);both(0x5924A0,std::bit_cast<uintptr_t>(&Interior_c::CalcExitPts),Exits);both(0x591F90,std::bit_cast<uintptr_t>(&Interior_c::AddPickups),Pickups);both(0x5982B0,std::bit_cast<uintptr_t>(&InteriorManager_c::HasInteriorHadStealDataSetup),Had);both(0x59A790,std::bit_cast<uintptr_t>(&Interior_c::FurnishShop),Shop);both(0x599AF0,std::bit_cast<uintptr_t>(&Interior_c::FurnishOffice),Office);both(0x597740,std::bit_cast<uintptr_t>(&Interior_c::FurnishLounge),Lounge);both(0x593FC0,std::bit_cast<uintptr_t>(&Interior_c::FurnishBedroom),Bedroom);both(0x5970B0,std::bit_cast<uintptr_t>(&Interior_c::FurnishKitchen),Kitchen);}
 if(fn.address==0x5915D0){both(0x56E210,uintptr_t(&FindPlayerPed),Player);both(0x5E02E0,std::bit_cast<uintptr_t>(&CPed::GetEntityThatThisPedIsHolding),Holding);both(0x563280,uintptr_t(&CWorld::Remove),WorldRemove);}
 if(fn.address==0x5929F0)both(0x5913E0,std::bit_cast<uintptr_t>(&Interior_c::IsPtInside),Inside);
 if(fn.address==0x591F90){both(0x821B1E,uintptr_t(&std::rand),Random);both(0x591BD0,std::bit_cast<uintptr_t>(&Interior_c::GetTileCentre),Centre);patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(0x456F20,ModelPickup));patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(uintptr_t(&CPickups::GenerateNewOne),ModelPickupDll));patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(0x457380,WeaponPickup));patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(uintptr_t(&CPickups::GenerateNewOne_WeaponType),WeaponPickupDll));}
 alignas(16)std::array<uint8,sizeof(Interior_c)> raw{};std::array<uint8,64> box{},seedData{};FurnitureEntity_c node{};uint32 bad{};
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=uint32(i);randomCount=0;trace=json::array();raw.fill(0xA7);box.fill(0);seedData.fill(0);pedBytes.fill(0);objectBytes.fill(0);entityBytes.fill(0);group={};node={};auto* self=reinterpret_cast<Interior_c*>(raw.data());self->m_box=reinterpret_cast<tEffectInterior*>(box.data());self->m_pGroup=&group;self->m_interiorId=i;box[0]=uint8(i%9==8?99:i%9);box[2]=box[3]=20;box[5]=uint8(i%3==0?-1:i%3);box[0x1E]=uint8(i);self->m_position=CVector{float(i%17)*.1f,float(i%13)*.3f,float(i%19)*-.125f};self->m_list.RemoveAll();std::fill(raw.begin()+0x68,raw.begin()+0x3EC,uint8(i%5));
 std::memset(&matrix,0,sizeof(matrix));matrix.SetUnity();matrix.GetPosition()=CVector{float(i%19)*.3f,float(i%17)*-.1f,float(i%13)*1.25f};*reinterpret_cast<CMatrix**>(entityBytes.data()+0x14)=&matrix;*reinterpret_cast<uintptr_t*>(objectBytes.data())=uintptr_t(vt.data());vt[0]=uintptr_t(&Delete);objectBytes[0x36]=uint8(i%7==0?3:4);*reinterpret_cast<uint32*>(objectBytes.data()+0x140)=i%3?0x2000:0;node.m_entity=reinterpret_cast<CEntity*>(objectBytes.data());if(i%8!=0)self->m_list.AddItem(&node);StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC).RemoveAll();
 StaticRef<uint32>(0xB7CB84)=200000+uint32(i);StaticRef<uint32>(0xBB3DC4)=i%8==0?200000:0;StaticRef<uint16>(0xBB4A70)=uint16(i);StaticRef<int32>(0xBB3914)=i%66;std::memset(reinterpret_cast<void*>(0xBB3918),0x5A,256);StaticRef<uintptr_t>(0xBB3DAC)=i%2?uintptr_t(seedData.data()):0;*reinterpret_cast<float*>(seedData.data()+0x18)=.3f+float(i%11);*reinterpret_cast<float*>(seedData.data()+0x14)=1.1f+float(i%7);*reinterpret_cast<float*>(seedData.data()+8)=-2.3f+float(i%5);
 auto& camera=StaticRef<CVector>(0xB6F02C);camera=CVector{self->m_position.x+float(i%13-6)*.3f,self->m_position.y+float(i%11-5)*.7f,self->m_position.z};StaticRef<uintptr_t>(0xB6F03C)=i%2?uintptr_t(&matrix):0;
 h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;std::array<uint32,1> args{uint32(uintptr_t(&self->m_position))};uint32 result=Invoke(fn.address,self,args.data(),fn.address==0x593BF0?1:0);return json{{"switch",switched},{"result",fn.address==0x593BF0?result:fn.address==0x5929F0?result&255:0},{"interior",ReversingProbe::Bytes(raw.data(),raw.size())},{"node",ReversingProbe::Bytes(&node,sizeof(node))},{"object",ReversingProbe::Bytes(objectBytes.data(),objectBytes.size())},{"matrix",ReversingProbe::Bytes(&matrix,sizeof(matrix))},{"pool",ReversingProbe::Bytes(reinterpret_cast<void*>(0xBAD3EC),12)},{"steals",StaticRef<uint16>(0xBB4A70)},{"ids_count",StaticRef<int32>(0xBB3914)},{"ids",ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3918),256)},{"box",ReversingProbe::Bytes(box.data(),box.size())},{"trace",trace},{"rng_calls",randomCount}};};auto a=run(false),b=run(true);bool equal=a==b&&a.value("switch",false);bad+=!equal;Write({{"event","comparison"},{"pair",pairs++},{"function",std::string("Interior_c::")+fn.name},{"case",i},{"equal",equal},{"accepted",equal},{"original",a},{"candidate",b}});}
 std::vector<std::pair<uintptr_t,std::array<uint8,5>>> codeBefore;for(const auto& patch:patches)codeBefore.emplace_back(patch->address,patch->saved);patches.clear();for(const auto& entry:codeBefore)restored&=std::memcmp(reinterpret_cast<const void*>(entry.first),entry.second.data(),5)==0;
 h->State(enabled);h->LockState(locked);restored&=h->Hooked()==enabled&&h->Locked()==locked;failures+=bad;Write({{"event","function_summary"},{"function",std::string("Interior_c::")+fn.name},{"cases",1024},{"failures",bad}});
 }
 std::memcpy(reinterpret_cast<void*>(0xBAD3EC),oldPool.data(),12);std::memcpy(reinterpret_cast<void*>(0xB6F02C),oldCamera.data(),20);StaticRef<uint32>(0xB7CB84)=oldTime;StaticRef<uint32>(0xBB3DC4)=oldPickupTime;StaticRef<uint16>(0xBB4A70)=oldSteals;StaticRef<int32>(0xBB3914)=oldCount;std::memcpy(reinterpret_cast<void*>(0xBB3918),oldIds.data(),256);StaticRef<uintptr_t>(0xBB3DAC)=oldSeedPtr;
 restored&=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBAD3EC),12)==oldPool;
 restored&=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB6F02C),20)==oldCamera;
 restored&=StaticRef<uint32>(0xB7CB84)==oldTime&&StaticRef<uint32>(0xBB3DC4)==oldPickupTime&&StaticRef<uint16>(0xBB4A70)==oldSteals&&StaticRef<int32>(0xBB3914)==oldCount&&StaticRef<uintptr_t>(0xBB3DAC)==oldSeedPtr;
 restored&=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3918),256)==oldIds;Write({{"event",failures||!restored?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",restored}});log.close();}
}
