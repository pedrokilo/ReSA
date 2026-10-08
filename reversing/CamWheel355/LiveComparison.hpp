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
#include "WaterLevel.h"
#include "ColModel.h"
#include "../ProbeRuntime.hpp"
namespace CamWheel355Live {
inline json trace;inline uint32 scenario{};inline CColModel* model{};
inline CColModel* __fastcall ColModel(const CEntity*,void*){trace.push_back({"col_model"});return model;}
inline eVehicleAppearance __fastcall Appearance(const CVehicle*,void*){trace.push_back({"appearance"});return static_cast<eVehicleAppearance>((scenario/9)%6);}
inline void __fastcall Bone(CPed*,void*,CVector* out,eBoneTag bone,bool update){trace.push_back({"bone",int(bone),update});*out=CVector{float(scenario%7)-3,float(scenario%5),2};}
inline bool __cdecl Water(float x,float y,float z,float* out,uint8 touching,CVector* normal){trace.push_back({"water",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),std::bit_cast<uint32>(z),touching,uintptr_t(normal)});*out=float(scenario%13)*.5f;return scenario%5!=0;}
inline bool __cdecl Line(const CVector& source,const CVector& target,CColPoint&,CEntity*& hit,bool b,bool v,bool p,bool o,bool d,bool see,bool cam,bool shoot){trace.push_back({"line",std::bit_cast<uint32>(source.x),std::bit_cast<uint32>(source.y),std::bit_cast<uint32>(source.z),std::bit_cast<uint32>(target.x),std::bit_cast<uint32>(target.y),std::bit_cast<uint32>(target.z),uintptr_t(CWorld::pIgnoreEntity),b,v,p,o,d,see,cam,shoot});hit=nullptr;return scenario%3==0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_WheelCam",0x512110,0,-1,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-wheel355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-wheel355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-wheel355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto savedMS=CTimer::GetTimeInMS();const auto savedStep=CTimer::GetTimeStep();const auto savedIgnore=CWorld::pIgnoreEntity;
 std::array<uint8,32> savedBlur;std::memcpy(savedBlur.data(),reinterpret_cast<void*>(0xB6F0AC),32);
 std::array<uint8,12> savedWaterColor;std::memcpy(savedWaterColor.data(),reinterpret_cast<void*>(0xB7C508),12);
 const auto savedMagnitude=StaticRef<float>(0x8CC7A8);const auto savedBlurAmount=StaticRef<uint32>(0x8CC7A4);
 EntryPatch co(0x535300,ColModel),cod(bit_cast<uintptr_t>(&CEntity::GetColModel),ColModel);
 EntryPatch ap(0x6D1080,Appearance),apd(bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance),Appearance);
 EntryPatch bo(0x5E4280,Bone),bod(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),Bone);
 EntryPatch wa(0x6EB690,Water),wad(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),Water);
 EntryPatch li(0x56BA00,Line),lid(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),Line);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(16) std::array<uint8,4096> entities[2]{};alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};model=reinterpret_cast<CColModel*>(col.data());
    auto* target=reinterpret_cast<CEntity*>(entities[0].data());auto* driver=reinterpret_cast<CPed*>(entities[1].data());
    entities[0][0x36]=uint8(i%11==0?4:i%11==1?3:2);At<uintptr_t>(entities[0].data(),0x14)=uintptr_t(matrix.data());At<uintptr_t>(entities[0].data(),0x460)=i%3?uintptr_t(driver):0;
    At<int32>(entities[0].data(),0x590)=i%3==0?5:i%3==1?9:6;At<int16>(entities[0].data(),0x22)=int16(i%2?430:411);At<CVector>(entities[0].data(),0x44)=CVector{float(i%7)*.125f-.5f,float(i%5)*.25f-.5f,float(i%3)*.125f};
    At<CVector>(matrix.data(),0)=CVector{.8f,.6f,0};At<CVector>(matrix.data(),0x10)=CVector{-.6f,.8f,0};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%11)-5,float(i%7),float(i%5)*.25f};
    At<CVector>(col.data(),0)=CVector{-1.1f,-2.2f,-.3f};At<CVector>(col.data(),12)=CVector{1.2f,2.3f,1.4f};
    StaticRef<uint32>(0xB7CB84)=uint32(i%19==0?0x80000080u:5000u+uint32(i)*397);StaticRef<float>(0xB7CB5C)=.5f+float(i%5)*.25f;
    StaticRef<float>(0xB7C508)=float(i%17)-8;StaticRef<float>(0xB7C50C)=float(i%13)-6;StaticRef<float>(0xB7C510)=float(i%11)-5;
    StaticRef<float>(0x8CC7A8)=i%2?10.f:3.f;StaticRef<uint32>(0x8CC7A4)=uint32(i%3+20);std::memset(reinterpret_cast<void*>(0xB6F0AC),uint8(i),32);CWorld::pIgnoreEntity=reinterpret_cast<CEntity*>(driver);
    self->m_pCamTargetEntity=target;self->m_vecSource=CVector{float(i%13)-6,float(i%17)-8,2.0f+float(i%7)*.25f};self->m_vecUp=CVector{.125f,.25f,1};self->m_fFOV=45;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;const CVector unused{1,2,3};const bool result=reinterpret_cast<bool(__thiscall*)(CCam*,const CVector&,float,float,float)>(fn.address)(self,unused,0,0,0);
    return json{{"cam",Digest(raw.data(),raw.size())},{"entities",Digest(entities,sizeof(entities))},{"blur",Digest(reinterpret_cast<void*>(0xB6F0AC),32)},{"ignore",uintptr_t(CWorld::pIgnoreEntity)},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uint32>(0xB7CB84)=savedMS;StaticRef<float>(0xB7CB5C)=savedStep;CWorld::pIgnoreEntity=savedIgnore;std::memcpy(reinterpret_cast<void*>(0xB6F0AC),savedBlur.data(),32);std::memcpy(reinterpret_cast<void*>(0xB7C508),savedWaterColor.data(),12);StaticRef<float>(0x8CC7A8)=savedMagnitude;StaticRef<uint32>(0x8CC7A4)=savedBlurAmount;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
