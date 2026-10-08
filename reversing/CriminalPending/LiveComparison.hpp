#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexKillCriminal.h"
#include "EventAcquaintancePedHate.h"
#include "InterestingEvents.h"
#include "../ProbeRuntime.hpp"
namespace CriminalPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace; inline uint32 scenario{},positive{},allocated{}; inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto*b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
template<class T>T& At(void* p,size_t off){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+off);}
alignas(16) inline std::array<uint8,0x800> owner{},criminal{},partner{},passenger{},alternate{},player{},playerData{},wanted{},chase{},intelOwner{},intelCriminal{},intelPartner{},intelPassenger{};
alignas(16) inline std::array<uint8,0xA20> ownCar{},targetCar{};
alignas(16) inline std::array<uint8,0x100> current{};
alignas(16) inline std::array<std::array<uint8,0x100>,8> tasks{};
inline std::array<uintptr_t,11> taskVT{};
inline int Id(void* p){if(!p)return -1; const std::array<void*,17> values{{owner.data(),criminal.data(),partner.data(),passenger.data(),alternate.data(),player.data(),playerData.data(),wanted.data(),ownCar.data(),targetCar.data(),current.data(),chase.data(),intelOwner.data(),intelCriminal.data(),intelPartner.data(),intelPassenger.data(),reinterpret_cast<void*>(base)}};for(size_t i=0;i<values.size();++i)if(p==values[i])return int(i);for(size_t i=0;i<tasks.size();++i)if(p==tasks[i].data())return int(32+i);return 99;}
inline bool __fastcall IsPlayer(void* p,void*){return p==player.data();}
inline CPed* __cdecl Player(int32){return reinterpret_cast<CPed*>(player.data());}
inline CWanted* __cdecl Wanted(int32){return reinterpret_cast<CWanted*>(wanted.data());}
inline bool __fastcall Pursuit(void*,void*,CCopPed*){trace.push_back({"pursuit"});return scenario%2;}
inline void __fastcall Register(void* self,void*,CEntity**){++positive;trace.push_back({"reference",Id(self)});}
inline void __fastcall Cleanup(void* self,void*,CEntity**){trace.push_back({"cleanup",Id(self)});}
inline void __fastcall SetPartner(void* self,void*,CCopPed* p){++positive;trace.push_back({"partner",Id(p)});At<CCopPed*>(self,0x7A8)=p;}
inline int32 __fastcall AddCriminal(void* self,void*,CPed* p){++positive;trace.push_back({"add_criminal",Id(self),Id(p)});return 1;}
inline void __fastcall Decision(void* self,void*,int32 value){++positive;trace.push_back({"decision",Id(self),value});}
inline CTask* __fastcall FindTask(void* self,void*,eTaskType type){trace.push_back({"find_task",Id(self),int(type)});return scenario%4==0?reinterpret_cast<CTask*>(chase.data()):nullptr;}
inline void __fastcall Equip(void* self,void*,eWeaponType type){++positive;trace.push_back({"equip",Id(self),int(type)});}
inline eWeaponSlot __fastcall Give(void* self,void*,eWeaponType type,uint32 ammo,bool flag){++positive;trace.push_back({"give",Id(self),int(type),ammo,flag});return static_cast<eWeaponSlot>(2);}
inline void __fastcall Default(void* self,void*){++positive;trace.push_back({"default",Id(self)});}
inline CEventAcquaintancePedHate* __fastcall EventCtor(void* self,void*,CPed* p){memset(self,0,0x18);At<CPed*>(self,0x14)=p;return reinterpret_cast<CEventAcquaintancePedHate*>(self);}
inline CEvent* __fastcall AddEvent(void* self,void*,CEvent* e,bool flag){++positive;trace.push_back({"event",Id(static_cast<uint8*>(self)-0x68),Id(At<CPed*>(e,0x14)),int(At<int16>(e,0xE)),flag});return nullptr;}
inline void __fastcall Interesting(void*,void*,CInterestingEvents::EType type,CEntity* p){++positive;trace.push_back({"interesting",int(type),Id(p)});}
inline uint16 __cdecl Random(){++positive;const uint16 v=uint16((scenario*1619u+137u)&0x7FFF);trace.push_back({"random",v});return v;}
inline int32 __cdecl RandomExe(){return Random();}
inline void* __cdecl Allocate(size_t size){++positive;trace.push_back({"alloc",size});if(scenario%7==0)return nullptr;if(allocated>=tasks.size())return nullptr;return tasks[allocated++].data();}
inline CPed* __fastcall Next(void*,void*,CPed*,bool any){trace.push_back({"next",any});return scenario%5==0?reinterpret_cast<CPed*>(alternate.data()):nullptr;}
inline bool __fastcall Change(void* self,void*,CPed* p){trace.push_back({"change",Id(p)});if(scenario%3==0)return false;At<CPed*>(self,0xC)=p;return true;}
inline eTaskType __fastcall Type(void*,void*){constexpr std::array<int,8> types{{1000,700,701,704,709,724,1022,200}};return static_cast<eTaskType>(types[(scenario/4)%types.size()]);}
inline bool __fastcall Abort(void*,void*,CPed*,eAbortPriority priority,const CEvent*){++positive;trace.push_back({"abort",int(priority)});return scenario%6!=1;}
inline eEventType __fastcall CurrentEvent(void*,void*){trace.push_back({"current_event"});return static_cast<eEventType>(scenario%3==0?36:0);}
inline void __fastcall SetTask(void* self,void*,CTask* task,ePrimaryTasks slot,bool flag){++positive;trace.push_back({"set_task",Id(static_cast<uint8*>(self)-4),Id(task),int(slot),flag});At<CTask*>(self,size_t(slot)*4)=task;}
struct ManagerAccess:CTaskManager {using CTaskManager::ChangeTaskInSlot;};
inline void __fastcall ChangeSlot(void* self,void*,CTask*& slot,CTask* task){const auto index=(reinterpret_cast<uintptr_t>(&slot)-reinterpret_cast<uintptr_t>(self))/4;SetTask(self,nullptr,task,static_cast<ePrimaryTasks>(index),false);}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,2> fns{{{"CTaskComplexKillCriminal","CreateFirstSubTask",0x68DC60,0x870A00,9,1024},{"CTaskComplexKillCriminal","ControlSubTask",0x68E950,0x870A00,10,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"criminal-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"criminal-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("criminal-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"criminal-pending-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> enabled{},locked{};for(size_t n=0;n<2;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const bool ambient=StaticRef<bool>(0xB72994);const uint32 clock=CTimer::GetTimeInMS();const float timestep=CTimer::GetTimeStep();uint32 pairs=0,failuresTotal=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x5DF8F0,IsPlayer),ad(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
 EntryPatch b(0x56E210,Player),bd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch c(0x56E230,Wanted),cd(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
 EntryPatch d(0x562FB0,Pursuit),dd(bit_cast<uintptr_t>(static_cast<bool(CWanted::*)(CCopPed*)>(&CWanted::CanCopJoinPursuit)),Pursuit);
 EntryPatch e(0x571B70,Register),ed(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch f(0x571A00,Cleanup),fd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch g(0x5DDE80,SetPartner),gd(bit_cast<uintptr_t>(&CCopPed::SetPartner),SetPartner);
 EntryPatch h(0x5DDEB0,AddCriminal),hd(bit_cast<uintptr_t>(&CCopPed::AddCriminalToKill),AddCriminal);
 EntryPatch i(0x600B50,Decision),id(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),Decision);
 EntryPatch j(0x600EE0,FindTask),jd(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),FindTask);
 EntryPatch k(0x5E6280,Equip),kd(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),Equip);
 EntryPatch l(0x5E6080,Give),ld(bit_cast<uintptr_t>(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),Give);
 EntryPatch m(0x5E06E0,Default),md(bit_cast<uintptr_t>(&CPed::SetPedDefaultDecisionMaker),Default);
 EntryPatch o(0x420E70,EventCtor),od(bit_cast<uintptr_t>(&CEventAcquaintancePedHate::Constructor2),EventCtor);
 EntryPatch p(0x4AB420,AddEvent),pd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),AddEvent);
 EntryPatch q(0x602590,Interesting),qd(bit_cast<uintptr_t>(&CInterestingEvents::Add),Interesting);
 EntryPatch r(0x821B1E,RandomExe),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch t(0x61A5A0,Allocate),td(bit_cast<uintptr_t>(&CTask::operator new),Allocate);
 EntryPatch u(0x68C3C0,Next),ud(bit_cast<uintptr_t>(&CTaskComplexKillCriminal::FindNextCriminalToKill),Next);
 EntryPatch v(0x68C6E0,Change),vd(bit_cast<uintptr_t>(&CTaskComplexKillCriminal::ChangeTarget),Change);
 EntryPatch w(0x4B8CC0,CurrentEvent),wd(bit_cast<uintptr_t>(&CEventHandler::GetCurrentEventType),CurrentEvent);
 EntryPatch x(0x681AF0,SetTask),xd(bit_cast<uintptr_t>(&CTaskManager::SetTask),SetTask),xs(bit_cast<uintptr_t>(&ManagerAccess::ChangeTaskInSlot),ChangeSlot);
 taskVT.fill(0);taskVT[4]=reinterpret_cast<uintptr_t>(&Type);taskVT[6]=reinterpret_cast<uintptr_t>(&Abort);
 for(size_t n=0;n<2;++n){const auto&fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
 for(int caseId=0;caseId<fn.cases;++caseId){alignas(16)std::array<uint8,sizeof(CTaskComplexKillCriminal)> object{};base=uintptr_t(object.data());
 auto run=[&](bool candidate){scenario=uint32(caseId);trace=json::array();positive=allocated=0;memset(object.data(),0,object.size());
 const std::array<void*,17> blocks{{owner.data(),criminal.data(),partner.data(),passenger.data(),alternate.data(),player.data(),playerData.data(),wanted.data(),intelOwner.data(),intelCriminal.data(),intelPartner.data(),intelPassenger.data(),chase.data(),current.data(),ownCar.data(),targetCar.data(),nullptr}};
 for(size_t z=0;z<14;++z)memset(blocks[z],0,0x800*(z!=13)+0x100*(z==13));memset(ownCar.data(),0,ownCar.size());memset(targetCar.data(),0,targetCar.size());for(auto&task:tasks)task.fill(0xCD);
 At<CPed*>(object.data(),0xC)=scenario%31==1?nullptr:reinterpret_cast<CPed*>(criminal.data());At<CCopPed*>(object.data(),0x10)=reinterpret_cast<CCopPed*>(owner.data());At<CTask*>(object.data(),8)=reinterpret_cast<CTask*>(current.data());At<uintptr_t>(current.data(),0)=uintptr_t(taskVT.data());At<float>(object.data(),0x14)=float(int(scenario%9)-3)*0.1f;object[0x18]=uint8((scenario/5)%2);object[0x1A]=uint8((scenario/17)%2);
 const std::array<void*,5> peds{{owner.data(),criminal.data(),partner.data(),passenger.data(),alternate.data()}};for(size_t z=0;z<peds.size();++z){At<int32>(peds[z],0x598)=z==0||z==2?6:4;At<float>(peds[z],0x540)=100.f;At<CVector>(peds[z],4)=CVector{float(scenario%80)+float(z)*3.f,float((scenario/80)%7)*2.f,0.f};}
 At<float>(owner.data(),0x540)=scenario%23==0?0.f:100.f;At<float>(criminal.data(),0x540)=scenario%11==0?0.f:100.f;At<float>(partner.data(),0x540)=scenario%13==0?0.f:100.f;
 At<uint32>(owner.data(),0x46C)=scenario%4<2?0x100:0;At<uint32>(criminal.data(),0x46C)=(scenario/4)%2?0x100:0;At<uint32>(partner.data(),0x46C)=(scenario/8)%2?0x100:0;At<uint32>(passenger.data(),0x46C)=0x100;At<uint32>(criminal.data(),0x478)=(scenario/9)%2?0x800:0;
 At<void*>(owner.data(),0x58C)=scenario%19==0?nullptr:ownCar.data();At<void*>(criminal.data(),0x58C)=scenario%3?targetCar.data():nullptr;At<void*>(partner.data(),0x58C)=ownCar.data();At<void*>(passenger.data(),0x58C)=targetCar.data();At<void*>(owner.data(),0x7A8)=scenario%5==0?nullptr:partner.data();owner[0x7C0]=uint8((scenario/2)%2);partner[0x7C0]=uint8((scenario/7)%2);
 At<void*>(owner.data(),0x47C)=intelOwner.data();At<void*>(criminal.data(),0x47C)=intelCriminal.data();At<void*>(partner.data(),0x47C)=intelPartner.data();At<void*>(passenger.data(),0x47C)=intelPassenger.data();At<void*>(alternate.data(),0x47C)=intelCriminal.data();At<void*>(chase.data(),0xC)=scenario%2?criminal.data():alternate.data();
 At<void*>(player.data(),0x480)=playerData.data();At<void*>(playerData.data(),0)=wanted.data();At<uint32>(wanted.data(),0x2C)=scenario%29==0?2:0;
 At<void*>(ownCar.data(),0x460)=scenario%3?owner.data():partner.data();At<void*>(targetCar.data(),0x460)=scenario%41==0?player.data():criminal.data();At<void*>(ownCar.data(),0x464)=partner.data();At<void*>(targetCar.data(),0x464)=passenger.data();ownCar[0x488]=targetCar[0x488]=1;targetCar[0x484]=uint8((scenario/11)%2);targetCar[0x42E]=uint8((scenario/3)%2?8:0);
 At<int32>(ownCar.data(),0x590)=(scenario/12)%2?9:0;ownCar[0x3B9]=uint8(scenario%4);ownCar[0x3BA]=uint8(scenario%3?55:18);ownCar[0x3D0]=uint8(scenario%60);targetCar[0x3D0]=uint8(scenario%250);At<float>(targetCar.data(),0x4C0)=scenario%7?1000.f:200.f;
 At<CVector>(ownCar.data(),4)=CVector{float((scenario/8)%80),3.f,0.f};At<CVector>(targetCar.data(),4)=CVector{float((scenario/16)%80),1.f,0.f};At<CVector>(targetCar.data(),0x44)=CVector{float(scenario%5)*0.0625f,0.f,0.f};
 StaticRef<bool>(0xB72994)=scenario%37!=0;StaticRef<uint32>(0xB7CB84)=100000+scenario;StaticRef<float>(0xB7CB5C)=0.25f*float(1+scenario%8);
 hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];const bool switches=(address<0x400000||address>=0x1000000)==candidate;if(!switches)return json{{"switch",false}};
 auto*result=reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(address)(object.data(),reinterpret_cast<CPed*>(owner.data()));if(!candidate&&positive)++positiveCases;
 json taskHashes=json::array();for(size_t z=0;z<allocated;++z){At<uintptr_t>(tasks[z].data(),0)=0;taskHashes.push_back(Hash(tasks[z].data(),tasks[z].size()));}
 return json{{"switch",true},{"ret",Id(result)},{"object",Hash(object.data(),object.size())},{"owner",Hash(owner.data(),owner.size())},{"criminal",Hash(criminal.data(),criminal.size())},{"partner",Hash(partner.data(),partner.size())},{"passenger",Hash(passenger.data(),passenger.size())},{"cars",{Hash(ownCar.data(),ownCar.size()),Hash(targetCar.data(),targetCar.size())}},{"intelligence",{Hash(intelOwner.data(),intelOwner.size()),Hash(intelCriminal.data(),intelCriminal.size()),Hash(intelPartner.data(),intelPartner.size()),Hash(intelPassenger.data(),intelPassenger.size())}},{"tasks",taskHashes},{"trace",trace}};
 };
 const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",caseId},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
 }
 if(!positiveCases)++failures;failuresTotal+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;for(size_t n=0;n<2;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}StaticRef<bool>(0xB72994)=ambient;StaticRef<uint32>(0xB7CB84)=clock;StaticRef<float>(0xB7CB5C)=timestep;
 Write({{"event",failuresTotal?"error":"complete"},{"pairs",pairs},{"failures",failuresTotal},{"state_restored",restored}});log.close();
}
}
