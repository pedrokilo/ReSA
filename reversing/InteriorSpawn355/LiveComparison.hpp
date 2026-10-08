#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "PopCycle.h"
#include "ScriptsForBrains.h"
#include "TaskManager.h"
#include "PedIntelligence.h"
#include "PedModelInfo.h"
#include "Clock.h"
#include "Collision/Collision.h"
#include "../ProbeRuntime.hpp"
namespace InteriorSpawn355Live {
inline json trace;inline uint32 scenario{};
inline std::array<CPed*,18> peds{};inline uint32 created{},randomCalls{},allocated{};inline uintptr_t fakeMatrix{};alignas(16) inline std::array<std::array<uint8,128>,16> tasks{};
inline bool __fastcall Valid(CPed* ped,void*){trace.push_back({"valid",uintptr_t(ped)});return scenario%3!=0;}
inline void __cdecl Remove(CPed* ped){trace.push_back({"remove",uintptr_t(ped)});}
inline int32 __fastcall Infos(InteriorGroup_c*,void*,int32 type){trace.push_back({"infos",type});return int32((scenario/16)%37);}
inline bool __cdecl Time(uint8 from,uint8 to){trace.push_back({"time",from,to});return (scenario/4)%4==0?from==9:(scenario/4)%4==1?from==18:(scenario/4)%4==2?from==6:false;}
inline int __cdecl Random(){trace.push_back({"random",randomCalls});return int((scenario*1619+randomCalls++*8191)%32768);}
inline int32 __cdecl Model(int32 selector){trace.push_back({"model",selector});return 2000;}
inline CPed* __cdecl Add(ePedType type,eModelID model,const CVector& pos,bool wander){trace.push_back({"add",int32(type),int32(model),std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),wander});const auto index=created++;return (scenario+index)%3==0?nullptr:peds[index];}
inline void __fastcall Created(CPed* ped,void*,ePedCreatedBy type){trace.push_back({"created",uintptr_t(ped),int32(type)});}
inline void __fastcall Decision(CPedIntelligence* intelligence,void*,int32 type){trace.push_back({"decision",uintptr_t(intelligence),type});}
inline void* __cdecl New(size_t bytes){trace.push_back({"new",bytes});const auto index=allocated++;return (scenario+index)%5==0?nullptr:tasks[index].data();}
inline int32 TaskIndex(CTask* task){return task?int((uintptr_t(task)-uintptr_t(tasks.data()))/128):-1;}
inline void __fastcall Set(CTaskManager*,void*,CTask* task,ePrimaryTasks type,bool unused){trace.push_back({"set",int32(type),unused,TaskIndex(task)});}
struct ManagerAccess:CTaskManager{using CTaskManager::ChangeTaskInSlot;};
inline void __fastcall Change(CTaskManager* manager,void*,CTask*& slot,CTask* task){trace.push_back({"set",int32(&slot-manager->m_aPrimaryTasks.data()),false,TaskIndex(task)});}
inline void __fastcall Allocate(CPlaceable* p,void*){trace.push_back({"allocate",uintptr_t(p)});*reinterpret_cast<uintptr_t*>(reinterpret_cast<uint8*>(p)+0x14)=fakeMatrix;}
inline void __fastcall UpdateMatrix(CSimpleTransform* p,void*,CMatrix* matrix){trace.push_back({"update_matrix",uintptr_t(p),uintptr_t(matrix)});}
inline uintptr_t fakeInterior{};
inline void __cdecl Stream(int32 type){trace.push_back({"stream",type});}
inline void __cdecl Load(bool all){trace.push_back({"load",all});}
inline ePedType __cdecl Gang(){trace.push_back({"gang"});return ePedType(scenario%4==0?0:7+scenario%3);}
inline eModelID __cdecl Occupation(eGangID gang){trace.push_back({"occupation",int32(gang)});return eModelID(scenario%5==0?-1:2000);}
inline void __fastcall Brain(CScriptsForBrains* brains,void*,const char* name,CEntity* entity,int8 attach){trace.push_back({"brain",uintptr_t(brains),name,uintptr_t(entity),attach});}
inline int32 __fastcall Interior(InteriorGroup_c*,void*){trace.push_back({"interior"});return int32(fakeInterior);}
inline int32 __fastcall Tile(Interior_c*,void*,int32 type,int32* x,int32* y){trace.push_back({"tile",type});*x=int32(scenario%17)-8;*y=int32(scenario%9)-4;return 0;}
inline CVector* __fastcall Centre(Interior_c*,void*,float x,float y,CVector* out){trace.push_back({"centre",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y)});*out=CVector{x,y,float(scenario%7)*.125f};return out;}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* bytes=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^bytes[i])*1099511628211ull;return h;}
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{{"InteriorGroup_c","SetupHousePeds",0x5965E0,0,-1,2048},{"InteriorGroup_c","SetupShopPeds",0x594C10,0,-1,2048}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}

inline void Update() {
 if(done)return;
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
 const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-spawn355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-spawn355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-spawn355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 std::array<uint8,16> savedFlags{};std::memcpy(savedFlags.data(),reinterpret_cast<void*>(0xBB3D9C),16);const auto savedZone=CPopCycle::m_pCurrZoneInfo;const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);
 EntryPatch a(0x5E4220,Valid),ad(bit_cast<uintptr_t>(&CPed::IsPointerValid),Valid);
 EntryPatch b(0x610F20,Remove),bd(bit_cast<uintptr_t>(&CPopulation::RemovePed),Remove);
 EntryPatch c(0x594920,Infos),cd(bit_cast<uintptr_t>(&InteriorGroup_c::GetNumInteriorInfos),Infos);
 EntryPatch d(0x52CEE0,Time),dd(bit_cast<uintptr_t>(&CClock::GetIsTimeInRange),Time);
 EntryPatch e(0x821B1E,Random),ed(bit_cast<uintptr_t>(&std::rand),Random);
 EntryPatch f(0x407FB0,Model),fd(bit_cast<uintptr_t>(&CStreaming::FindMIPedSlotForInterior),Model);
 EntryPatch g(0x612710,Add),gd(bit_cast<uintptr_t>(&CPopulation::AddPed),Add);
 EntryPatch h(0x5E47E0,Created),hd(bit_cast<uintptr_t>(&CPed::SetCharCreatedBy),Created);
 EntryPatch j(0x600B50,Decision),jd(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),Decision);
 EntryPatch k(0x61A5A0,New),kd(bit_cast<uintptr_t>(&CTask::operator new),New);
 EntryPatch l(0x681AF0,Set),ld(bit_cast<uintptr_t>(&CTaskManager::SetTask),Set),lc(bit_cast<uintptr_t>(&ManagerAccess::ChangeTaskInSlot),Change);
 EntryPatch m(0x54F560,Allocate),md(bit_cast<uintptr_t>(&CPlaceable::AllocateMatrix),Allocate);
 EntryPatch n(0x54F1B0,UpdateMatrix),nd(bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix),UpdateMatrix);
 EntryPatch stream(0x40BBB0,Stream),streamDll(bit_cast<uintptr_t>(&CStreaming::StreamPedsForInterior),Stream);
 EntryPatch load(0x40EA10,Load),loadDll(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),Load);
 EntryPatch gang(0x60F8D0,Gang),gangDll(bit_cast<uintptr_t>(&CPopCycle::PickGangToCreateMembersOf),Gang);
 EntryPatch occupation(0x611550,Occupation),occupationDll(bit_cast<uintptr_t>(&CPopulation::ChooseGangOccupation),Occupation);
 EntryPatch brain(0x46CED0,Brain);
 EntryPatch interior(0x5948C0,Interior),interiorDll(bit_cast<uintptr_t>(&InteriorGroup_c::GetRandomInterior),Interior);
 EntryPatch tile(0x591B20,Tile),tileDll(bit_cast<uintptr_t>(&Interior_c::GetRandomTile),Tile);
 EntryPatch centre(0x591BD0,Centre),centreDll(bit_cast<uintptr_t>(&Interior_c::GetTileCentre),Centre);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
  const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
  const std::string full=std::string(fn.cls)+"::"+fn.name;int failures=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    hook->State(candidate);scenario=uint32(i);trace=json::array();
    alignas(16) std::array<uint8,sizeof(InteriorGroup_c)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<InteriorGroup_c*>(raw.data());
    alignas(16) std::array<uint8,sizeof(CEntity)> entity{};alignas(16) std::array<uint8,sizeof(CMatrix)> matrix{};alignas(16) std::array<uint8,sizeof(CPedModelInfo)> model{};
    alignas(16) std::array<std::array<uint8,sizeof(CPed)>,18> pedData{};alignas(16) std::array<std::array<uint8,0x300>,18> intelligence{};
    for(size_t j=0;j<peds.size();++j){peds[j]=reinterpret_cast<CPed*>(pedData[j].data());*reinterpret_cast<CPedIntelligence**>(pedData[j].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intelligence[j].data());}
    self->m_pEntity=reinterpret_cast<CEntity*>(entity.data());At<uintptr_t>(entity.data(),0x14)=i%2?uintptr_t(matrix.data()):0;fakeMatrix=uintptr_t(matrix.data());At<CVector>(matrix.data(),0)=CVector{1,0,0};At<CVector>(matrix.data(),0x10)=CVector{0,1,0};At<CVector>(matrix.data(),0x20)=CVector{0,0,1};At<CVector>(matrix.data(),0x30)=CVector{float(i%13)-7,float(i%9)-5,float(i%7)-3};
    At<int32>(model.data(),0x28)=int32(i%20);StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(model.data());self->m_isVisible=i%2!=0;self->m_lastIsVisible=(i/2)%2!=0;self->m_numPeds=0;for(int j=0;j<16;++j)self->m_peds[j]=(i+j)%3?peds[j]:nullptr;
    self->m_numInteriors=uint8(int8((i/32)%6)-2);CPopCycle::m_pCurrZoneInfo=i%2?reinterpret_cast<CZoneInfo*>(0x1234):nullptr;fakeInterior=uintptr_t(entity.data());for(int j=0;j<16;++j)StaticRef<uint8>(0xBB3D9C+j)=uint8((i+j)%2);created=randomCalls=allocated=0;for(auto& task:tasks)task.fill(0xCD);
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__thiscall*)(InteriorGroup_c*)>(fn.address)(self);
    return json{{"group",Digest(raw.data(),raw.size())},{"entity",Digest(entity.data(),entity.size())},{"peds",Digest(pedData.data(),sizeof(pedData))},{"tasks",Digest(tasks.data(),sizeof(tasks))},{"trace",trace},{"switches",switched}};
   };
   const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;
   Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
  }
  hook->State(enabled);hook->LockState(locked);
  Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;CPopCycle::m_pCurrZoneInfo=savedZone;std::memcpy(reinterpret_cast<void*>(0xBB3D9C),savedFlags.data(),16);
 Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
