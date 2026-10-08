#pragma once
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorLifecycleCoreLive {
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
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,6> fns{{
{"Interior_c","Exit",0x592230,0,-1,1024},
{"Interior_c","AddGotoPt",0x591D20,0,-1,1024},
{"Interior_c","AddInteriorInfo",0x591E40,0,-1,1024},
{"Interior_c","GetBoundingBox",0x593DB0,0,-1,1024},
{"Interior_c","CalcExitPts",0x5924A0,0,-1,1024},
{"Interior_c","Furnish",0x591590,0,-1,1024}}};
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
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"interior-lifecycle-core-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"interior-lifecycle-core-runtime.jsonl");ReversingProbe::InstallFailureCapture("interior-lifecycle-core");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;uint32 pairs{},failures{};bool restored=true;
 for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}const bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);
 std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;auto both=[&](uintptr_t addr,uintptr_t dll,auto cb){patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(addr,cb));if(addr!=dll)patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(dll,cb));};
 both(0x591BD0,std::bit_cast<uintptr_t>(&Interior_c::GetTileCentre),Centre);
 if(fn.address==0x593DB0)both(0x5922C0,std::bit_cast<uintptr_t>(&Interior_c::FindBoundingBox),Bounding);
 if(fn.address==0x591E40)both(0x7EDDF0,uintptr_t(&RwV3dTransformVectors),Transform);
 if(fn.address==0x592230){both(0x456D30,uintptr_t(&CPickups::RemovePickUpsInArea),Remove);both(0x5915D0,std::bit_cast<uintptr_t>(&Interior_c::Unfurnish),Unfurnish);}
 if(fn.address==0x591590){both(0x59A790,std::bit_cast<uintptr_t>(&Interior_c::FurnishShop),Shop);both(0x599AF0,std::bit_cast<uintptr_t>(&Interior_c::FurnishOffice),Office);both(0x597740,std::bit_cast<uintptr_t>(&Interior_c::FurnishLounge),Lounge);both(0x593FC0,std::bit_cast<uintptr_t>(&Interior_c::FurnishBedroom),Bedroom);both(0x5970B0,std::bit_cast<uintptr_t>(&Interior_c::FurnishKitchen),Kitchen);}
 alignas(16)std::array<uint8,sizeof(Interior_c)> raw{};std::array<uint8,64> box{};std::array<CVector,4> points{};FurnitureEntity_c furniture{};uint32 bad{};
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=uint32(i);trace=json::array();raw.fill(0xA7);box.fill(0);points.fill(CVector{123,456,789});furniture={};auto* self=reinterpret_cast<Interior_c*>(raw.data());self->m_box=reinterpret_cast<tEffectInterior*>(box.data());box[0]=uint8(i%8);box[2]=box[3]=20;for(int k=5;k<12;++k)box[k]=uint8(i%4==0?-1:2+(i+k)%8);self->m_position=CVector{float(i%17)*.1f,float(i%13)*.3f,float(i%19)*-.125f};self->field_40C=int8((i%9)*2);self->m_interiorInfosCount=int8(i%18);std::fill(raw.begin()+0x68,raw.begin()+0x3EC,uint8(i%8));for(int k=0;k<16;++k){raw[0x410+k*16]=uint8(1+k/2);raw[0x411+k*16]=uint8(1+k%2*5);}std::memset(&self->m_matrix,0,64);self->m_matrix.right.x=self->m_matrix.up.y=self->m_matrix.at.z=1;furniture.m_tileX=10;furniture.m_tileY=10;
 std::array<uint32,5> args{};int count=0;bool returned=false;
 if(fn.address==0x591D20){args={uint32(i%20),uint32((i/20)%20),F(.1f),F(.3f),0};count=4;}
 if(fn.address==0x591E40){args={uint32(i%9),F(float(i%17)*.1f),F(float(i%13)*.3f),uint32(i%7-1),0x12345678};count=5;returned=true;}
 if(fn.address==0x593DB0){args[0]=uint32(uintptr_t(&furniture));args[1]=uint32(uintptr_t(points.data()));count=2;returned=true;}
 h->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const uint32 result=Invoke(fn.address,self,args.data(),count);return json{{"switch",switched},{"result",returned?(result&255):0},{"object",Hash(raw.data(),raw.size())},{"object_bytes",ReversingProbe::Bytes(raw.data(),raw.size())},{"points",Hash(points.data(),sizeof(points))},{"box",ReversingProbe::Bytes(box.data(),box.size())},{"trace",trace}};};auto a=run(false),b=run(true);bool equal=a==b&&a.value("switch",false);bad+=!equal;Write({{"event","comparison"},{"pair",pairs++},{"function",std::string("Interior_c::")+fn.name},{"case",i},{"equal",equal},{"accepted",equal},{"original",a},{"candidate",b}});}
 std::vector<std::pair<uintptr_t,std::array<uint8,5>>> codeBefore;for(const auto& patch:patches)codeBefore.emplace_back(patch->address,patch->saved);patches.clear();for(const auto& entry:codeBefore)restored&=std::memcmp(reinterpret_cast<const void*>(entry.first),entry.second.data(),5)==0;
 h->State(enabled);h->LockState(locked);restored&=h->Hooked()==enabled&&h->Locked()==locked;failures+=bad;Write({{"event","function_summary"},{"function",std::string("Interior_c::")+fn.name},{"cases",1024},{"failures",bad}});
 }Write({{"event",failures||!restored?"error":"complete"},{"pairs",pairs},{"failures",failures},{"state_restored",restored}});log.close();}
}
