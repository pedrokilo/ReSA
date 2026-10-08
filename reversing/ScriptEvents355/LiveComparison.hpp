#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEScriptAudioEntity.h"
#include "AESoundManager.h"
#include "AEAudioHardware.h"
#include "../ProbeRuntime.hpp"
#include "AEAmbienceTrackManager.h"
#include "AEPedAudioEntity.h"
#include "AEAudioUtility.h"
#include "Localisation.h"
namespace ScriptEvents355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}

alignas(16) inline std::array<uint8,sizeof(CPed)> physical{};alignas(16) inline std::array<uint8,0x80> matrix{};inline std::array<int8,65536> volumes;inline void* activeSelf{};
inline json Position(const CVector& p){return json::array({std::bit_cast<uint32>(p.x),std::bit_cast<uint32>(p.y),std::bit_cast<uint32>(p.z)});}
inline uint32 Bits(float x){return std::bit_cast<uint32>(x);}inline bool Phys(const void* p){return p==physical.data();}
inline void __fastcall Resident(void* owner,void*,eSoundBankSlot slot,eSoundBank bank,eSoundID sound,eAudioEvents event,CVector& pos,CPhysical* p,float volume,float speed,int16 play,float distance){trace.push_back({"resident",owner==activeSelf,int(slot),int(bank),int(sound),int(event),Position(pos),Phys(p),Bits(volume),Bits(speed),play,Bits(distance)});}
inline void __fastcall Mission(void* owner,void*,eAudioEvents event,CVector& pos,CPhysical* p,int16 sound,uint8 link,uint8 flags,float volume,float distance,float speed){trace.push_back({"mission",owner==activeSelf,int(event),Position(pos),Phys(p),sound,link,flags,Bits(volume),Bits(distance),Bits(speed)});}
inline bool __fastcall Loaded(void*,void*,eSoundBank bank,eSoundBankSlot slot){bool r=((scenario/193)+int(bank))%3==0;trace.push_back({"loaded",int(bank),int(slot),r});return r;}
inline void __fastcall Load(void*,void*,eSoundBank bank,eSoundBankSlot slot){trace.push_back({"load",int(bank),int(slot)});}
inline int16 __fastcall Playing(void*,void*,int16 slot){int16 r=int16((scenario/193)%3-1);trace.push_back({"playing",slot,r});return r;}
inline void __fastcall CancelSlot(void*,void*,int16 slot,bool stop){trace.push_back({"cancelSlot",slot,stop});}
inline int16 __fastcall Query(void*,void*,int16 event,CAEAudioEntity* owner){int16 r=int16((scenario/193)%3-1);trace.push_back({"query",event,owner==activeSelf,r});return r;}
inline int16 __fastcall QueryPhysical(void*,void*,int16 event,CAEAudioEntity* owner,CPhysical* p){int16 r=int16((scenario/193)%3-1);trace.push_back({"queryPhysical",event,owner==activeSelf,Phys(p),r});return r;}
inline void __fastcall Cancel(void*,void*,int16 event,CAEAudioEntity* owner){trace.push_back({"cancel",event,owner==activeSelf});}
inline void __fastcall CancelPhysical(void*,void*,int16 event,CAEAudioEntity* owner,CPhysical* p){trace.push_back({"cancelPhysical",event,owner==activeSelf,Phys(p)});}
inline int32 __cdecl Random(int32 lo,int32 hi){const auto r=(scenario/193)%2?hi:lo;trace.push_back({"random",lo,hi,r});return r;}
inline bool __cdecl Blood(){bool r=(scenario/193)%2;trace.push_back({"blood",r});return r;}
inline void __cdecl HeliOn(){trace.push_back({"heli",true});}inline void __cdecl HeliOff(){trace.push_back({"heli",false});}
inline void __fastcall Ambience(void*,void*,eAudioEvents event){trace.push_back({"ambience",int(event)});}inline void __fastcall StopAmbience(void*,void*){trace.push_back({"ambienceStop"});}
inline void __fastcall Frontend(void*,void*,eAudioEvents event,float vol,float speed){trace.push_back({"frontend",int(event),Bits(vol),Bits(speed)});}
inline void __fastcall Weapon(void*,void*,int32 event,eWeaponType weapon,CPhysical* p){trace.push_back({"weapon",event,int(weapon),Phys(p)});}
inline void __fastcall WaterPhysical(void*,void*,CPhysical* p,float vol,bool force){trace.push_back({"waterPhysical",Phys(p),Bits(vol),force});}
inline void __fastcall WaterVector(void*,void*,CVector p,float vol){trace.push_back({"waterVector",Position(p),Bits(vol)});}
inline void __fastcall Door(void* owner,void*,eAudioEvents event,CVector& p,float vol,float speed){trace.push_back({"door",owner==static_cast<uint8*>(activeSelf)+0x114,int(event),Position(p),Bits(vol),Bits(speed)});}
inline void __fastcall Explosion(void* owner,void*,eAudioEvents event,CVector& p,float vol){trace.push_back({"explosion",owner==static_cast<uint8*>(activeSelf)+offsetof(CAEScriptAudioEntity,m_ExplosionAudio),int(event),Position(p),Bits(vol)});}
inline void __fastcall Crunch(void* owner,void*,eAudioEvents event,float vol,float speed,CPhysical* p,eSurfaceType surface,int32 arg,uint32 maximum){trace.push_back({"crunch",owner==&reinterpret_cast<CPed*>(physical.data())->GetAE(),int(event),Bits(vol),Bits(speed),Phys(p),int(surface),arg,maximum});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CAEScriptAudioEntity","ProcessMissionAudioEvent",0x4ECCF0,0,-1,6176}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"script-events355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"script-events355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("script-events355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"script-events355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldTime=CTimer::m_snTimeInMilliseconds;const auto oldVolumes=StaticRef<int8*>(0xBD00F8);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p0(0x4EC550,Resident),d0(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayResidentSoundEvent),Resident);
 EntryPatch p1(0x4EC6D0,Mission),d1(bit_cast<uintptr_t>(&CAEScriptAudioEntity::PlayMissionBankSound),Mission);
 EntryPatch p2(0x4D88C0,Loaded),d2(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),Loaded);
 EntryPatch p3(0x4D88A0,Load),d3(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),Load);
 EntryPatch p4(0x4EF520,Playing),d4(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsPlayingInBankSlot),Playing);
 EntryPatch p5(0x4EFC60,CancelSlot),d5(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsInBankSlot),CancelSlot);
 EntryPatch p6(0x4EF570,Query),d6(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),Query);
 EntryPatch p7(0x4EF5D0,QueryPhysical),d7(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntityAndPhysical),QueryPhysical);
 EntryPatch p8(0x4EFB90,Cancel),d8(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntity),Cancel);
 EntryPatch p9(0x4EFBF0,CancelPhysical),d9(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOfThisEventPlayingForThisEntityAndPhysical),CancelPhysical);
 EntryPatch p10(0x4D6D50,Ambience),d10(bit_cast<uintptr_t>(&CAEAmbienceTrackManager::PlaySpecialMissionAmbienceTrack),Ambience);
 EntryPatch p11(0x4D6D60,StopAmbience),d11(bit_cast<uintptr_t>(&CAEAmbienceTrackManager::StopSpecialMissionAmbienceTrack),StopAmbience);
 EntryPatch p12(0x506EA0,Frontend),d12(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Frontend);
 EntryPatch p13(0x506F40,Weapon),d13(bit_cast<uintptr_t>(&CAudioEngine::ReportWeaponEvent),Weapon);
 EntryPatch p14(0x506F00,WaterPhysical),d14(bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(CPhysical*,float,bool)>(&CAudioEngine::ReportWaterSplash)),WaterPhysical);
 EntryPatch p15(0x506F10,WaterVector),d15(bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(CVector,float)>(&CAudioEngine::ReportWaterSplash)),WaterVector);
 EntryPatch p16(0x4DC860,Door),d16(bit_cast<uintptr_t>(static_cast<void(CAEDoorAudioEntity::*)(eAudioEvents,CVector&,float,float)>(&CAEDoorAudioEntity::AddAudioEvent)),Door);
 EntryPatch p17(0x4DCBE0,Explosion),d17(bit_cast<uintptr_t>(&CAEExplosionAudioEntity::AddAudioEvent),Explosion);
 EntryPatch p18(0x4E2BB0,Crunch),d18(bit_cast<uintptr_t>(&CAEPedAudioEntity::AddAudioEvent),Crunch);
 EntryPatch randomOriginal(0x4D9C10,Random),randomDll(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CAEAudioUtility::GetRandomNumberInRange)),Random);
 EntryPatch bloodOriginal(0x56D230,Blood),bloodDll(bit_cast<uintptr_t>(&CLocalisation::Blood),Blood);
 EntryPatch heliOriginal(0x4F4EF0,HeliOn),heliDll(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::EnableHelicoptors),HeliOn),noHeliOriginal(0x4F4EE0,HeliOff),noHeliDll(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::DisableHelicoptors),HeliOff);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CAEScriptAudioEntity)> object{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(physical.data(),0,physical.size());memset(matrix.data(),0,matrix.size());auto* self=reinterpret_cast<CAEScriptAudioEntity*>(object.data());activeSelf=self;StaticRef<int8*>(0xBD00F8)=volumes.data();volumes.fill(int8(i%7==0?-128:i%9-5));
    *reinterpret_cast<uintptr_t*>(physical.data()+0x14)=(i/193)%2?uintptr_t(matrix.data()):0;*reinterpret_cast<CVector*>(physical.data()+4)=CVector{11.f,12.f,13.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{21.f,22.f,23.f};physical[0x36]=uint8(1+(i/193)%7);
    self->field_7D=uint8((i/193)%3);self->field_7E=std::array<uint8,8>{0,1,2,127,128,129,254,255}[(i/193)%8];CTimer::m_snTimeInMilliseconds=uint32(1000+i*19);CVector position{float(i%17-8),float(i%13-6),float(i%11-5)};
    auto raw=i%193==191?104:i%193==192?0xFFFF:1000+i%193;raw|=std::array<int32,4>{0,0x10000,0x7FFF0000,int32(0xFFFF0000u)}[(i/193)%4];const auto event=static_cast<eAudioEvents>(raw);
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,CVector&,CPhysical*,float,float)>(fn.address)(self,event,position,(i/386)%3?reinterpret_cast<CPhysical*>(physical.data()):nullptr,float(i%9-4)*.25f,.5f+float(i%7)*.125f);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"physical",Hash(physical.data(),physical.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"position",Position(position)},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 CTimer::m_snTimeInMilliseconds=oldTime;StaticRef<int8*>(0xBD00F8)=oldVolumes;bool restored=StaticRef<int8*>(0xBD00F8)==oldVolumes;for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


