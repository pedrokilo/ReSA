#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "Population.h"
#include "../ProbeRuntime.hpp"
namespace TrainPassenger355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},playerCalls{},typeCalls{},draw{};
inline CTrain* train{};inline CPed* players[2]{};inline CPed* spawned{};alignas(16) inline std::array<uint8,0x1C> allocated{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CPlayerPed* __cdecl Player(int32 id){const auto index=playerCalls++%2;trace.push_back({"player",id,index});return reinterpret_cast<CPlayerPed*>(players[index]);}
inline eTaskType __fastcall Type(void*,void*){const auto result=700+(scenario/3+typeCalls++)%3;trace.push_back({"type",result});return static_cast<eTaskType>(result);}
inline int32 __cdecl Random(){static constexpr int32 values[]{0,1,99,100,32767,32768,65535};const auto result=values[(scenario/7+draw++)%7];trace.push_back({"random",result});return result;}
inline void __fastcall Clean(void*,void*,CEntity** ref){trace.push_back({"clean",*ref!=nullptr});}
inline void __fastcall Register(void*,void*,CEntity** ref){trace.push_back({"register",*ref==spawned});}
inline CPed* __cdecl Spawn(CVehicle* vehicle,bool driver,int32 rating,int32 seat,bool male,bool criminal){trace.push_back({"spawn",vehicle==train,driver,rating,seat,male,criminal});return scenario%5==0?nullptr:spawned;}
inline void* __cdecl Allocate(size_t size){trace.push_back({"allocate",size});return scenario%11==0?nullptr:allocated.data();}
inline CTask* __fastcall Construct(void* raw,void*,CVehicle* vehicle,int32 door,int32 delay,bool sensible){trace.push_back({"construct",vehicle==train,door,delay,sensible});memset(raw,0,0x1C);*reinterpret_cast<CVehicle**>(static_cast<uint8*>(raw)+0xC)=vehicle;return static_cast<CTask*>(raw);}
inline void __fastcall SetTask(void*,void*,CTask* task,ePrimaryTasks slot,bool unused){trace.push_back({"set_task",task!=nullptr,int(slot),unused});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","RemoveRandomPassenger",0x6F6850,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-passenger355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-passenger355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-passenger355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-passenger355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldReplay=StaticRef<uint8>(0xA43088);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x56E210,Player),pd(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),Player);
 EntryPatch r(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch c(0x571A00,Clean),cd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
 EntryPatch q(0x571B70,Register),qd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch s(0x613A00,Spawn),sd(bit_cast<uintptr_t>(&CPopulation::AddPedInCar),Spawn);
 EntryPatch a(0x61A5A0,Allocate),ad(bit_cast<uintptr_t>(&CTask::operator new),Allocate);
 EntryPatch t(0x63BDD0,Construct),m(0x681AF0,SetTask);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<std::array<uint8,sizeof(CPed)>,4> peds{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,4> intellect{};alignas(16) std::array<uint8,0x1C> activeTask{};std::array<uintptr_t,5> vtable{};vtable[4]=bit_cast<uintptr_t>(&Type);
   auto run=[&](bool candidate){scenario=i;playerCalls=typeCalls=draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(peds.data(),0,peds.size()*sizeof(peds[0]));memset(intellect.data(),0,intellect.size()*sizeof(intellect[0]));memset(activeTask.data(),0,activeTask.size());allocated.fill(0xCD);train=reinterpret_cast<CTrain*>(object.data());players[0]=reinterpret_cast<CPed*>(peds[0].data());players[1]=reinterpret_cast<CPed*>(peds[1].data());spawned=reinterpret_cast<CPed*>(peds[3].data());
    for(size_t k=0;k<4;++k){auto* ped=reinterpret_cast<CPed*>(peds[k].data());*reinterpret_cast<CPedIntelligence**>(peds[k].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect[k].data());ped->bInVehicle=((i/13+int(k))%3)==0;ped->m_pVehicle=(i/19+int(k))%4==0?train:nullptr;}
    *reinterpret_cast<uintptr_t*>(activeTask.data())=bit_cast<uintptr_t>(vtable.data());*reinterpret_cast<CVehicle**>(activeTask.data()+0xC)=i%2==0?train:nullptr;players[0]->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=i%3==0?nullptr:reinterpret_cast<CTask*>(activeTask.data());
    train->trainFlags.bClockwiseDirection=i%2;train->m_pDriver=i%4==0?players[1]:nullptr;train->m_nNumPassengersToLeave=uint8((i/7)%5);train->m_nNumPassengersToEnter=9;train->m_pTemporaryPassenger=i%3==0?nullptr:reinterpret_cast<CPed*>(peds[2].data());StaticRef<uint8>(0xA43088)=uint8(i%23==0?1:0);
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(train);
    if(!candidate&&std::ranges::any_of(trace,[](const json& row){return row[0]=="spawn";}))++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"peds",Hash(peds.data(),sizeof(peds))},{"intellect",Hash(intellect.data(),sizeof(intellect))},{"allocation",Hash(allocated.data(),allocated.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 StaticRef<uint8>(0xA43088)=oldReplay;bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


