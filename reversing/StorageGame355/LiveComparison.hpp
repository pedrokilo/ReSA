#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "GenericGameStorage.h"
#include "C_PcSave.h"
#include "FileMgr.h"
#include "Ropes.h"
#include "Pickups.h"
#include "GangWars.h"
#include "Stats.h"
#include "GameLogic.h"
#include "Game.h"
#include "Streaming.h"
#include "Text/Text.h"
#include "Collision.h"
#include "Clothes.h"
#include "PopCycle.h"
#include "TagManager.h"
#include "TheZones.h"
#include "TheScripts.h"
#include "../ProbeRuntime.hpp"
namespace StorageGame355Live {
inline uint32 scenario{};inline json trace;
inline CPlayerPed* fakePeds[2]{};
inline CPlayerPed* __cdecl Player(int32 id){return fakePeds[id<0?scenario%2:id];}
#define STORAGE_VOID(Name,Id) inline void __cdecl Name(){trace.push_back({Id});}
STORAGE_VOID(Ropes,1) STORAGE_VOID(Pickups,2) STORAGE_VOID(Sort,3) STORAGE_VOID(Scripts,4) STORAGE_VOID(Tags,5) STORAGE_VOID(Pop,6) STORAGE_VOID(RemoveModels,7) STORAGE_VOID(ClearZones,8)
#undef STORAGE_VOID
inline void __cdecl End(bool a){trace.push_back({"end",a});}
inline void __cdecl Stat(eStats s,float f){trace.push_back({"stat",int(s),std::bit_cast<uint32>(f)});}
inline void __cdecl Time(uint32 n){trace.push_back({"time",n});}
inline void __fastcall Sprint(CPlayerPed* p,void*){trace.push_back({"sprint",uintptr_t(p)});}
inline void __fastcall Wanted(CPlayerPed* p,void*,eWantedLevel w){trace.push_back({"wanted",uintptr_t(p),int(w)});}
inline void __cdecl Tidy(bool a,bool b){trace.push_back({"tidy",a,b});}
inline void __fastcall TextLoad(CText* t,void*,bool b){trace.push_back({"text",uintptr_t(t),b});}
inline void __cdecl CollisionScene(const CVector& v){trace.push_back({"collision",std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline void __cdecl Scene(const CVector& v){trace.push_back({"scene",std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)});}
inline void __cdecl Models(bool b){trace.push_back({"models",b});}
inline void __fastcall Fade(CCamera* cam,void*,float v,eFadeFlag f){trace.push_back({"fade",uintptr_t(cam),std::bit_cast<uint32>(v),int(f)});}
inline void __cdecl Clothes(CPlayerPed* p,bool b){trace.push_back({"clothes",uintptr_t(p),b});}
inline void __cdecl Colours(bool b){trace.push_back({"colours",b});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
{"CGenericGameStorage","DoGameSpecificStuffBeforeSave",0x618F50,0,-1,1024},
{"CGenericGameStorage","DoGameSpecificStuffAfterSucessLoad",0x618E90,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"storage-game355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"storage-game355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("storage-game355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 std::array<uint8,800> savedPlayers{};memcpy(savedPlayers.data(),reinterpret_cast<void*>(0xB7CD98),800);
 std::array<uint8,24> savedCamera{};memcpy(savedCamera.data(),reinterpret_cast<void*>(0xB6F028),24);
 const auto savedFocus=StaticRef<uint8>(0xB7CD74);const auto savedGang=StaticRef<uint16>(0x96AB90);const auto savedFlags=StaticRef<uint16>(0xC16EDC);
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p0(0x556B10,Ropes),d0(bit_cast<uintptr_t>(&CRopes::Shutdown),Ropes);
 EntryPatch p1(0x455470,Pickups),d1(bit_cast<uintptr_t>(&CPickups::RemovePickupObjects),Pickups);
 EntryPatch p2(0x4464C0,End),d2(bit_cast<uintptr_t>(&CGangWars::EndGangWar),End);
 EntryPatch p3(0x55C180,Stat),d3(bit_cast<uintptr_t>(&CStats::IncrementStat),Stat);
 EntryPatch p4(0x4414C0,Time),d4(bit_cast<uintptr_t>(&CGameLogic::PassTime),Time);
 EntryPatch p5(0x60A530,Sprint),d5(bit_cast<uintptr_t>(&CPlayerPed::ResetSprintEnergy),Sprint);
 EntryPatch p6(0x609F10,Wanted),d6(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevel),Wanted);
 EntryPatch p7(0x53C500,Tidy),d7(bit_cast<uintptr_t>(&CGame::TidyUpMemory),Tidy);
 EntryPatch p8(0x6A01A0,TextLoad),d8(bit_cast<uintptr_t>(&CText::Load),TextLoad);
 EntryPatch p9(0x411E30,Sort),d9(bit_cast<uintptr_t>(&CCollision::SortOutCollisionAfterLoad),Sort);
 EntryPatch p10(0x40ED80,CollisionScene),d10(bit_cast<uintptr_t>(&CStreaming::LoadSceneCollision),CollisionScene);
 EntryPatch p11(0x40EB70,Scene),d11(bit_cast<uintptr_t>(&CStreaming::LoadScene),Scene);
 EntryPatch p12(0x40EA10,Models),d12(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),Models);
 EntryPatch p13(0x50AC20,Fade),d13(bit_cast<uintptr_t>(&CCamera::Fade),Fade);
 EntryPatch p14(0x46A000,Scripts),d14(bit_cast<uintptr_t>(&CTheScripts::Process),Scripts);
 EntryPatch p15(0x49CDE0,Tags),d15(bit_cast<uintptr_t>(&CTagManager::UpdateNumTagged),Tags);
 EntryPatch p16(0x5A82C0,Clothes),d16(bit_cast<uintptr_t>(&CClothes::RebuildPlayer),Clothes);
 EntryPatch p17(0x610BF0,Pop),d17(bit_cast<uintptr_t>(&CPopCycle::Update),Pop);
 EntryPatch p18(0x40B3A0,RemoveModels),d18(bit_cast<uintptr_t>(&CStreaming::RemoveInappropriatePedModels),RemoveModels);
 EntryPatch p19(0x443FF0,ClearZones),d19(bit_cast<uintptr_t>(&CGangWars::ClearSpecificZonesToTriggerGangWar),ClearZones);
 EntryPatch p20(0x572440,Colours),d20(bit_cast<uintptr_t>(&CTheZones::FillZonesWithGangColours),Colours);
 EntryPatch p21(0x56E210,Player),d21(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(CPlayerPed) std::array<std::array<uint8,sizeof(CPlayerPed)>,2> peds{};
    alignas(16) std::array<uint8,0x48> matrix{};
    memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);
    for(int j=0;j<2;++j){fakePeds[j]=reinterpret_cast<CPlayerPed*>(peds[j].data());StaticRef<CPlayerPed*>(0xB7CD98+j*400)=fakePeds[j];StaticRef<uint8>(0xB7CEE7+j*400)=uint8(i+j);StaticRef<uint16>(0xB7CEDC+j*400)=uint16(i*13+j);}
    StaticRef<uint8>(0xB7CD74)=uint8(i%2);StaticRef<uint16>(0x96AB90)=uint16(0x0100+i%2);StaticRef<uint16>(0xC16EDC)=0x0203;
    StaticRef<uintptr_t>(0xB6F03C)=i%2?uintptr_t(matrix.data()):0;
    At<float>(reinterpret_cast<void*>(0xB6F028),4)=float(i)-30;At<float>(reinterpret_cast<void*>(0xB6F028),8)=float(i)+40;At<float>(reinterpret_cast<void*>(0xB6F028),12)=float(i)*3;
    At<float>(matrix.data(),0x30)=float(i)*2;At<float>(matrix.data(),0x34)=float(i)-10;At<float>(matrix.data(),0x38)=float(i)+100;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)()>(fn.address)();
    return json{{"peds",Digest(peds.data(),sizeof(peds))},{"players",Digest(reinterpret_cast<void*>(0xB7CD98),800)},{"flags",StaticRef<uint16>(0xC16EDC)},{"gang",StaticRef<uint16>(0x96AB90)},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 memcpy(reinterpret_cast<void*>(0xB7CD98),savedPlayers.data(),800);memcpy(reinterpret_cast<void*>(0xB6F028),savedCamera.data(),24);StaticRef<uint8>(0xB7CD74)=savedFocus;StaticRef<uint16>(0x96AB90)=savedGang;StaticRef<uint16>(0xC16EDC)=savedFlags;
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
