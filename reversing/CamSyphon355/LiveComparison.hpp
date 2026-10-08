#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cam.h"
#include "Camera.h"
#include "WeaponInfo.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace CamSyphon355Live {
inline json trace;inline uint32 scenario{};inline CWeaponInfo info;
inline void __fastcall Avoid(CCamera* cam,void*,const CVector* a,const CVector* b,CVector* out,float fov){trace.push_back({"avoid",uintptr_t(cam),std::bit_cast<uint32>(fov),std::bit_cast<uint32>(a->x),std::bit_cast<uint32>(b->z)});out->x+=float(scenario%3)*.125f;}
inline eWeaponSkill __fastcall Skill(CPed*,void*){trace.push_back({"skill"});return eWeaponSkill::STD;}
inline CWeaponInfo* __cdecl Info(eWeaponType type,eWeaponSkill skill){trace.push_back({"info",int(type),int(skill)});return scenario%7==0?nullptr:&info;}
inline void __fastcall Heading(CPlaceable*,void*,float value){trace.push_back({"heading",std::bit_cast<uint32>(value)});}
inline void __fastcall Matrix(CEntity*,void*){trace.push_back({"matrix"});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,1> fns{{{"CCam","Process_SpecialFixedForSyphon",0x517500,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cam-syphon355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cam-syphon355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cam-syphon355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x514030,Avoid);
 EntryPatch b(0x5E6580,Skill),bd(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),Skill);
 EntryPatch c(0x743C60,Info),cd(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),Info);
 EntryPatch d(0x43E0C0,Heading),dd(bit_cast<uintptr_t>(&CPlaceable::SetHeading),Heading);
 EntryPatch e(0x446F90,Matrix),ed(bit_cast<uintptr_t>(&CEntity::UpdateRwMatrix),Matrix);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CCam) std::array<uint8,sizeof(CCam)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<CCam*>(raw.data());
    alignas(CPed) std::array<uint8,sizeof(CPed)> pedRaw{};alignas(CEntity) std::array<uint8,sizeof(CEntity)> targetRaw{};auto* ped=reinterpret_cast<CPed*>(pedRaw.data());auto* object=reinterpret_cast<CEntity*>(targetRaw.data());
    pedRaw[0x36]=uint8(i%4==0?2:3);ped->GetPosition()=CVector{1,2,3};object->GetPosition()=CVector{float(i%5)-2,float((i/5)%5)-2,4};ped->m_pTargetedObject=i%5==0?nullptr:object;ped->m_nActiveWeaponSlot=uint8(i%13);ped->GetActiveWeapon().m_Type=WEAPON_PISTOL;ped->bIsDucking=i%2!=0;
    self->m_pCamTargetEntity=i%8==0?nullptr:ped;self->m_vecCamFixedModeSource=CVector{float(i%7)-3,2,.125f};self->m_vecCamFixedModeUpOffSet=CVector{0,float(i%3)*.125f,1};self->m_fSyphonModeTargetZOffSet=float(i%5)*.25f;self->m_fFOV=30+float(i%10);
    std::memset(&info,0,sizeof(info));info.flags.bAimWithArm=i%3==0;info.m_nWeaponFire=i%4==0?WEAPON_FIRE_MELEE:WEAPON_FIRE_INSTANT_HIT;
    const CVector target{float(i%9)-4,float((i/9)%9)-4,float(i%3)};
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CCam*,const CVector&,float,float,float)>(fn.address)(self,target,1,2,3);
    return json{{"cam",Digest(raw.data(),raw.size())},{"ped",Digest(pedRaw.data(),pedRaw.size())},{"target",Digest(targetRaw.data(),targetRaw.size())},{"trace",trace},{"switches",switched}};
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
