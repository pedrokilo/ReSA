#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamLamp355Live {
inline json trace;inline uint32 scenario{};inline std::array<CEntity*,4> entities{};
inline void __cdecl Find(const CVector& p,float radius,bool twoD,int16* count,int16 maximum,CEntity** out,bool buildings,bool vehicles,bool peds,bool objects,bool dummies){trace.push_back({"find",std::bit_cast<uint32>(radius),twoD,maximum,buildings,vehicles,peds,objects,dummies});*count=int16((scenario/2)%4);for(int16 k=0;k<*count;++k)out[k]=entities[k];}
inline CColModel* colModel{};
inline bool __cdecl Lamp(eModelID model){trace.push_back({"lamp",int(model)});return (int(model)+scenario)%3!=0;}
inline CColModel* __fastcall Col(const CEntity* e,void*){trace.push_back({"col",uintptr_t(e)});return colModel;}
inline bool __cdecl Clear(const CVector& a,const CVector& b,bool bu,bool ve,bool pe,bool ob,bool du,bool se,bool ca){trace.push_back({"clear",std::bit_cast<uint32>(a.x),std::bit_cast<uint32>(a.y),std::bit_cast<uint32>(a.z),std::bit_cast<uint32>(b.x),std::bit_cast<uint32>(b.y),std::bit_cast<uint32>(b.z),bu,ve,pe,ob,du,se,ca});return scenario%5!=0;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","GetLookFromLampPostPos",0x5161A0,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-lamp355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-lamp355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-lamp355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x564A20,Find),ad(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange),Find);
 EntryPatch b(0x509A30,Lamp),bd(bit_cast<uintptr_t>(&IsLampPost),Lamp);
 EntryPatch c(0x535300,Col),cd(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch d(0x56A490,Clear),dd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Clear);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(CEntity) std::array<std::array<uint8,sizeof(CEntity)>,4> objects{};
    alignas(CMatrix) std::array<std::array<uint8,sizeof(CMatrix)>,4> matrices{};
    alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};colModel=reinterpret_cast<CColModel*>(col.data());At<CVector>(col.data(),12)=CVector{float(i%9),float(i%7),float(i%13)};
    for(int j=0;j<4;++j){entities[j]=reinterpret_cast<CEntity*>(objects[j].data());At<uint32>(objects[j].data(),0x1C)=(i+j)%3==0?0:(i+j)%3==1?4:0x40000;At<int16>(objects[j].data(),0x22)=int16(100+j);At<uintptr_t>(objects[j].data(),0x14)=uintptr_t(matrices[j].data());At<CVector>(matrices[j].data(),0)=CVector{1,0,0};At<CVector>(matrices[j].data(),16)=CVector{0,1,0};At<CVector>(matrices[j].data(),32)=CVector{0,0,(i+j)%4==0?.9f:1.f};At<CVector>(matrices[j].data(),48)=CVector{float(3+j*9),float(i%7),float(j*2)};}
    const CVector point{float(i%5),float(i%11),-1};CVector source{-999,888,777};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    const auto result=reinterpret_cast<bool(__thiscall*)(CCam*,CEntity*,CPed*,const CVector&,CVector&)>(fn.address)(self,nullptr,nullptr,point,source);
    return json{{"cam",Digest(raw.data(),raw.size())},{"objects",Digest(objects.data(),sizeof(objects))},{"matrices",Digest(matrices.data(),sizeof(matrices))},{"source",ReversingProbe::Bytes(&source,sizeof(source))},{"result",result},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
