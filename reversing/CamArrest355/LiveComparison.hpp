#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "BaseModelInfo.h"
#include "ColModel.h"
#include "PedIntelligence.h"
#include "Tasks/TaskManager.h"
#include "Tasks/Task.h"
#include "World.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamArrest355Live {
inline json trace;inline uint32 scenario{},sphereCalls{},randomCalls{};inline CPlayerPed* player{};inline CTask* fakeTask{};
inline void __fastcall Avoid(CCamera*,void*,const CVector* a,const CVector* b,CVector* out,float fov){trace.push_back({"avoid",std::bit_cast<uint32>(a->x),std::bit_cast<uint32>(a->y),std::bit_cast<uint32>(a->z),std::bit_cast<uint32>(b->z),std::bit_cast<uint32>(fov)});out->x+=float(scenario%3)*.125f;}
inline void __fastcall Bone(CPed*,void*,CVector* out,eBoneTag bone,bool update){trace.push_back({"bone",int(bone),update});*out=CVector{float(scenario%11)-5,float(scenario%7),float(scenario%5)};}
inline CEntity* __cdecl Sphere(CVector center,float radius,CEntity* ignore,bool b,bool v,bool p,bool o,bool d,bool c){trace.push_back({"sphere",std::bit_cast<uint32>(center.x),std::bit_cast<uint32>(center.y),std::bit_cast<uint32>(center.z),std::bit_cast<uint32>(radius),uintptr_t(ignore),b,v,p,o,d,c});return ((sphereCalls+++scenario)%3)==0?ignore:nullptr;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return scenario%11==0?nullptr:player;}
inline bool __fastcall IsPlayer(const CPed*,void*){trace.push_back({"is_player"});return scenario%3!=0;}
inline bool __fastcall Cheese(CCam*,void*,CPed* cop,bool init){trace.push_back({"cheese",uintptr_t(cop),init});return init?scenario%5==0:scenario%2!=0;}
inline bool CopPosition(const char* label,CEntity* target,CPed* cop,CVector* dest,CVector* out){trace.push_back({label,uintptr_t(target),uintptr_t(cop),std::bit_cast<uint32>(dest->z)});dest->z+=.125f;*out=CVector{float(scenario%7)+3,float(scenario%5)+4,float(scenario%3)+5};return scenario%4!=0;}
inline bool __fastcall CopSource(CCam*,void*,CEntity* target,CPed* cop,CVector* dest,CVector* out){return CopPosition("cop_source",target,cop,dest,out);}
inline bool __fastcall CopAlternate(CCam*,void*,CEntity* target,CPed* cop,CVector* dest,CVector* out){return CopPosition("cop_alternate",target,cop,dest,out);}
inline bool __fastcall Lamp(CCam*,void*,CEntity* target,CPed* cop,const CVector& dest,CVector& out){trace.push_back({"lamp",uintptr_t(target),uintptr_t(cop),std::bit_cast<uint32>(dest.z)});out=CVector{float(scenario%7)+2,4,5};return scenario%3!=0;}
inline CTask* __fastcall Task(CTaskManager* manager,void*,eTaskType type){trace.push_back({"task",uintptr_t(manager),int(type)});return scenario%3!=0?fakeTask:nullptr;}
inline void __fastcall Reference(CEntity* entity,void*,CEntity** pointer){trace.push_back({"reference",uintptr_t(entity),uintptr_t(pointer)});}
inline float __cdecl Ground(CVector at,bool* found,CEntity** entity){trace.push_back({"ground",std::bit_cast<uint32>(at.x),std::bit_cast<uint32>(at.y),std::bit_cast<uint32>(at.z),uintptr_t(entity)});*found=scenario%3!=0;return float(scenario%5)*.25f;}
inline int __cdecl Random(){const int value=int((scenario*7919+randomCalls++*14033)%32768);trace.push_back({"random",value});return value;}
inline const GxtChar* __fastcall Text(void*,void*,const char* key){trace.push_back({"text",std::string(key)});return reinterpret_cast<const GxtChar*>("stub");}
inline void __cdecl Message(const GxtChar*,uint32 time,int32 style){trace.push_back({"message",time,style});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","ProcessArrestCamOne",0x518500,0,-1,12288}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-arrest355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-arrest355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-arrest355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedTarget=StaticRef<uintptr_t>(0xB6F980);const auto savedMS=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();const auto savedText=StaticRef<uint8>(0xBAADC0);std::array<uint8,12> savedState;std::memcpy(savedState.data(),reinterpret_cast<void*>(0xB6EC58),12);std::array<uint8,32> savedParameters;std::memcpy(savedParameters.data(),reinterpret_cast<void*>(0x8CC7E4),32);
 EntryPatch a(0x514030,Avoid);
 EntryPatch b(0x5E4280,Bone),bd(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),Bone);
 EntryPatch c(0x569E20,Sphere),cd(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),Sphere);
 EntryPatch pl(0x56E210,Player),pld(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch ip(0x5DF8F0,IsPlayer),ipd(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 EntryPatch ch(0x512EF0,Cheese),co(0x515D80,CopSource),ca(0x516010,CopAlternate);
 EntryPatch la(0x5161A0,Lamp),lad(bit_cast<uintptr_t>(&CCam::GetLookFromLampPostPos),Lamp);
 EntryPatch ta(0x681740,Task),tad(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),Task);
 EntryPatch re(0x571B70,Reference),red(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference);
 EntryPatch gr(0x5696C0,Ground),grd(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
 EntryPatch ra(0x821B1E,Random),rad(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch tx(0x6A0050,Text),ms(0x69F2B0,Message);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(16) std::array<uint8,4096> entities[4]{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(16) std::array<uint8,0x180> brains[3]{};alignas(4) std::array<uint8,32> playerData{},taskData{};
    auto* target=reinterpret_cast<CEntity*>(entities[0].data());auto* driver=reinterpret_cast<CPed*>(entities[1].data());auto* cop=reinterpret_cast<CPed*>(entities[2].data());player=reinterpret_cast<CPlayerPed*>(entities[3].data());fakeTask=reinterpret_cast<CTask*>(taskData.data());At<uintptr_t>(taskData.data(),8)=i%5==0?uintptr_t(target):uintptr_t(player);At<uintptr_t>(entities[3].data(),0x480)=uintptr_t(playerData.data());At<uintptr_t>(playerData.data(),8)=i%4==0?0:uintptr_t(cop);
    entities[0][0x36]=uint8(i%7==0?4:i%2?2:3);At<uintptr_t>(entities[0].data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(entities[0].data(),0x460)=i%3?uintptr_t(driver):0;
    At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),0x10)=CVector{0,1,0};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%11)-5,float(i%7),float(i%5)};
    for(int k=0;k<3;++k){At<uintptr_t>(entities[k].data(),0x47C)=uintptr_t(brains[k].data());for(int n=0;n<16;++n)At<uintptr_t>(brains[k].data(),0x130+n*4)=n<2?uintptr_t(cop):0;}
    StaticRef<uintptr_t>(0xB6F980)=uintptr_t(target);StaticRef<uint32>(0xB7CB84)=uint32(i%19==0?0x80000080u:5000u+uint32(i));StaticRef<float>(0xB7CB5C)=.5f+float(i%5)*.25f;StaticRef<uint8>(0xBAADC0)=uint8(i%2);StaticRef<int32>(0xB6EC58)=int32((i/12)%9);StaticRef<CEntity*>(0xB6EC5C)=i%3==0?nullptr:reinterpret_cast<CEntity*>(cop);StaticRef<float>(0xB6EC60)=12;
    StaticRef<float>(0x8CC7E4)=.03f;StaticRef<float>(0x8CC7E8)=.04f;StaticRef<float>(0x8CC7EC)=.05f;StaticRef<float>(0x8CC7F8)=.6f;StaticRef<float>(0x8CC7FC)=.7f;StaticRef<float>(0x8CC800)=.08f;
    self->m_bResetStatics=i%12<3;self->m_vecSource=CVector{float(i%13)-6,float(i%17)-8,2.0f+float(i%7)*.25f};self->m_fFOV=70;sphereCalls=0;randomCalls=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CCam*)>(fn.address)(self);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entities",Digest(entities,sizeof(entities))},{"state",Digest(reinterpret_cast<void*>(0xB6EC58),12)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xB6F980)=savedTarget;StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<float>(0xB7CB5C)=savedStep;StaticRef<uint8>(0xBAADC0)=savedText;std::memcpy(reinterpret_cast<void*>(0xB6EC58),savedState.data(),12);std::memcpy(reinterpret_cast<void*>(0x8CC7E4),savedParameters.data(),32);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
