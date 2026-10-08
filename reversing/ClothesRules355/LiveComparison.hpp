#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ClothesBuilder.h"
#include "Clothes.h"
#include "PedClothesDesc.h"
#include "../ProbeRuntime.hpp"
namespace ClothesRules355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline void __fastcall Model(CPedClothesDesc* d,void*,uint32 key,eClothesModelPart part){trace.push_back({"model",key,int(part)});d->m_anModelKeys[part]=key;}
inline void __fastcall Texture(CPedClothesDesc* d,void*,uint32 texture,uint32 model,eClothesTexturePart part){trace.push_back({"texture",texture,model,int(part)});d->m_anTextureKeys[part]=texture;d->m_anModelKeys[int(part)%10]=model;}
inline eClothesTexturePart __cdecl Dependent(eClothesModelPart p){auto r=static_cast<eClothesTexturePart>(int(p)%18);trace.push_back({"dependent",int(p),int(r)});return r;}
inline void __cdecl Delete(int id){trace.push_back({"deletable",id});}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CClothesBuilder","PreprocessClothesDesc",0x5A44C0,0,-1,4096},
{"CClothesBuilder","ReleaseGeometry",0x5A47B0,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CClothesBuilder")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"clothes-rules355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"clothes-rules355-runtime.jsonl");ReversingProbe::InstallFailureCapture("clothes-rules355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
const auto savedRules=CClothes::ms_clothesRules;const auto savedCount=CClothes::ms_numRuleTags;int pairs=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch me(0x5A7910,Model),md(bit_cast<uintptr_t>(static_cast<void(CPedClothesDesc::*)(uint32,eClothesModelPart)>(&CPedClothesDesc::SetModel)),Model);
EntryPatch te(0x5A8050,Texture),td(bit_cast<uintptr_t>(static_cast<void(CPedClothesDesc::*)(uint32,uint32,eClothesTexturePart)>(&CPedClothesDesc::SetTextureAndModel)),Texture);
EntryPatch de(0x5A7F30,Dependent),dd(bit_cast<uintptr_t>(&CClothes::GetDependentTexture),Dependent);
EntryPatch re(0x409C10,Delete),rd(bit_cast<uintptr_t>(&CStreaming::SetModelIsDeletable),Delete);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,positive=0,zero=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){trace=json::array();alignas(CPedClothesDesc) std::array<uint8,sizeof(CPedClothesDesc)> object{};object.fill(0xCD);auto* d=reinterpret_cast<CPedClothesDesc*>(object.data());for(int k=0;k<10;++k)d->m_anModelKeys[k]=100+uint32((i+k)%4);for(int k=0;k<18;++k)d->m_anTextureKeys[k]=200+uint32(k);CClothes::ms_clothesRules.fill(0xCDCDCDCD);CClothes::ms_numRuleTags=0;auto add=[&](std::initializer_list<uint32> words){for(auto v:words)CClothes::ms_clothesRules[CClothes::ms_numRuleTags++]=v;};
const uint32 key=100+uint32(i%4),target=uint32((i/8)%10),replacement=(i&16)?500:0,texture=(i&32)?600:0;const int mode=(i/64)%5;if(mode==1)add({5,key});if(mode==2)add({7,key});if(mode==3){add({5,key});add({4,key});}if(mode==4){add({7,key});add({6,key});}
switch(i%4){case 0:add({0,key,replacement});break;case 1:add({1,key,target,replacement,texture});break;case 2:add({2,key,texture});break;case 3:add({3,key,target});break;}
if(i%11==0)CClothes::ms_numRuleTags=0;if(i%7==0)add({0,101,700});h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;if(fn.address==0x5A44C0)reinterpret_cast<void(__cdecl*)(CPedClothesDesc&,bool)>(fn.address)(*d,(i&4)!=0);else reinterpret_cast<void(__cdecl*)(int)>(fn.address)(i%33);
return json{{"object",ReversingProbe::Bytes(object.data(),object.size())},{"rules",CClothes::ms_clothesRules},{"count",CClothes::ms_numRuleTags},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();zero+=a["trace"].empty();Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=positive>0&&zero>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive},{"inactive",zero}});}}
CClothes::ms_clothesRules=savedRules;CClothes::ms_numRuleTags=savedCount;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
