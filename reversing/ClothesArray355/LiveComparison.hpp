#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ClothesBuilder.h"
#include "Clothes.h"
#include "Directory.h"
#include "../ProbeRuntime.hpp"
namespace ClothesArray355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},loads{};alignas(16) inline std::array<uint8,0x40*20> models{};inline std::array<RpClump,20> clumps{};inline std::array<uint32,20> geometryTokens{};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline bool __fastcall Find(const CDirectory*,void*,uint32 key,CdStreamPos& position,uint32& size){position=std::bit_cast<CdStreamPos>(key^0x12345678u);size=(key&31)+1;trace.push_back({"find",key,position.ToInt(),size});return true;}
inline void __cdecl Request(int32 model,CdStreamPos position,uint32 size,StreamingImgID image,int32 flags){trace.push_back({"request",model,position.ToInt(),size,int(image),flags});}
inline void __cdecl LoadAll(bool priority){trace.push_back({"load_all",priority});++loads;if(scenario%3==0)for(int k=0;k<10;++k)CModelInfo::ms_modelInfoPtrs[384+k]=reinterpret_cast<CBaseModelInfo*>(models.data()+0x40*(k+10*(loads%2)));}
inline void __cdecl Load(){trace.push_back({"load_requested"});}
inline void __cdecl Remove(int model){trace.push_back({"remove",model});}
inline RpGeometry* __cdecl Blend(RpClump* c,const char* n0,const char* n1,const char* n2,float a,float b,float d){const int k=int(c-clumps.data());trace.push_back({"blend",k,n0,n1,n2,Bits(a),Bits(b),Bits(d)});return reinterpret_cast<RpGeometry*>(&geometryTokens[k]);}
inline void __cdecl Store(RpClump* c,int model){trace.push_back({"store_bones",int(c-clumps.data()),model});}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CClothesBuilder","ConstructGeometryArray",0x5A55A0,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CClothesBuilder")for(auto& x:c.Items())if(x->Name()=="ConstructGeometryArray")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"clothes-array355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"clothes-array355-runtime.jsonl");ReversingProbe::InstallFailureCapture("clothes-array355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::array<CBaseModelInfo*,10> saved{};for(int k=0;k<10;++k)saved[k]=CModelInfo::ms_modelInfoPtrs[384+k];const auto image=CClothes::ms_clothesImageId;int pairs=0,failures=0,positive=0,inactive=0,prefetch=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;using FindFn=bool(CDirectory::*)(uint32,CdStreamPos&,uint32&) const;using BlendFn=RpGeometry*(*)(RpClump*,const char*,const char*,const char*,float,float,float);
EntryPatch f(0x5324D0,Find),fd(bit_cast<uintptr_t>(static_cast<FindFn>(&CDirectory::FindItem)),Find),r(0x40A080,Request),rd(bit_cast<uintptr_t>(&CStreaming::RequestFile),Request),a(0x40EA10,LoadAll),ad(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),LoadAll),l(0x40E3A0,Load),ld(bit_cast<uintptr_t>(&CStreaming::LoadRequestedModels),Load),rem(0x4089A0,Remove),remd(bit_cast<uintptr_t>(&CStreaming::RemoveModel),Remove),b(0x5A4940,Blend),bd(bit_cast<uintptr_t>(static_cast<BlendFn>(&CClothesBuilder::BlendGeometry)),Blend),s(0x5A48B0,Store),sd(bit_cast<uintptr_t>(&CClothesBuilder::StoreBoneArray),Store);
auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;loads=0;trace=json::array();models.fill(0xCD);memset(clumps.data(),0,sizeof(clumps));for(int k=0;k<20;++k){clumps[k].object.type=rpCLUMP;*reinterpret_cast<RpClump**>(models.data()+k*0x40+0x1C)=&clumps[k];}for(int k=0;k<10;++k)CModelInfo::ms_modelInfoPtrs[384+k]=reinterpret_cast<CBaseModelInfo*>(models.data()+k*0x40);CClothes::ms_clothesImageId=i%4;std::array<uint32,10> keys{};std::array<RpGeometry*,12> out{};memset(out.data(),0xCD,sizeof(out));for(int k=0;k<10;++k)keys[k]=(i&(1<<k))?uint32(100+k):0;
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x5A55A0)==candidate;reinterpret_cast<void(__cdecl*)(RpGeometry**,uint32*,float,float,float)>(0x5A55A0)(out.data(),keys.data(),float(i%9)*.125f,float((i/9)%9)*.125f,float((i/81)%9)*.125f);return json{{"trace",trace},{"out",Hash(out.data(),sizeof(out))},{"keys",keys},{"models",Hash(models.data(),models.size())},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();inactive+=a["trace"].empty();for(const auto& t:a["trace"])prefetch+=t[0]=="load_requested";Write({{"event","comparison"},{"function","CClothesBuilder::ConstructGeometryArray"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
for(int k=0;k<10;++k)CModelInfo::ms_modelInfoPtrs[384+k]=saved[k];CClothes::ms_clothesImageId=image;pass&=positive>0&&inactive>0&&prefetch>0;Write({{"event","function_summary"},{"function","CClothesBuilder::ConstructGeometryArray"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"prefetch",prefetch}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
