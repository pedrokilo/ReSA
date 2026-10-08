#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Train.h"
#include "TrainNode.h"
#include "Population.h"
#include "Pools/PedPool.h"
#include "PedGroups.h"
#include "CarEnterExit.h"
#include "../ProbeRuntime.hpp"
namespace TrainAddPassenger355Live {
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
inline uint8* poolStorage{};inline uint8* poolIntellect{};inline int allocations{},appended{};alignas(16) inline std::array<std::array<uint8,0x58>,3> tasks{};
inline int PedIndex(const void* raw){const auto address=uintptr_t(raw);return address>=uintptr_t(poolStorage)&&address<uintptr_t(poolStorage)+6*0x7C4?int((address-uintptr_t(poolStorage))/0x7C4):-1;}
inline CPedGroup* __cdecl Group(const CPed* ped){const auto index=PedIndex(ped);const auto r=(scenario+index)%11==0;trace.push_back({"group",index,r});return r?reinterpret_cast<CPedGroup*>(0x1000):nullptr;}
inline CTask* __fastcall Find(void* raw,void*,eTaskType type){const auto address=uintptr_t(raw);const auto index=address>=uintptr_t(poolIntellect)&&address<uintptr_t(poolIntellect)+6*sizeof(CPedIntelligence)?int((address-uintptr_t(poolIntellect))/sizeof(CPedIntelligence)):-1;const auto result=(scenario+index)%13==0;trace.push_back({"find",index,int(type),result});return result?reinterpret_cast<CTask*>(0x1000):nullptr;}
inline void __cdecl RemovePed(CPed* ped){trace.push_back({"remove_ped",PedIndex(ped)});ped->bInVehicle=false;}
inline bool __cdecl Door(const CPed* ped,const CVehicle*,CVector& pos,int32& door){pos=CVector{7.f,8.f,9.f};door=0;trace.push_back({"door",PedIndex(ped)});if(scenario%7==0)train->trainFlags.bClockwiseDirection=!train->trainFlags.bClockwiseDirection;return scenario%3!=0;}
inline CMatrix& __cdecl Inverse(CMatrix& in,CMatrix& out){trace.push_back({"inverse"});out.GetRight()=CVector{1.f,0.f,0.f};out.GetForward()=CVector{0.f,1.f,0.f};out.GetUp()=CVector{0.f,0.f,1.f};out.GetPosition()=-in.GetPosition();return out;}
inline void* __cdecl AllocateTask(size_t size){const auto index=allocations++;trace.push_back({"allocate",size,index});return index==2&&scenario%17==0?nullptr:tasks[size_t(index)].data();}
inline CTask* __fastcall Seek(void* raw,void*,CEntity* entity,int32 seek,int32 scan,float distance,float one,float two,bool flag0,bool flag1){trace.push_back({"seek",entity==train,seek,scan,std::bit_cast<uint32>(distance),std::bit_cast<uint32>(one),std::bit_cast<uint32>(two),flag0,flag1});memset(raw,0,0x58);return static_cast<CTask*>(raw);}
inline CTask* __fastcall Sequence(void* raw,void*){trace.push_back({"sequence"});memset(raw,0,0x40);return static_cast<CTask*>(raw);}
inline bool __fastcall Append(void*,void*,CTask* task){trace.push_back({"append",appended++,task!=nullptr,task?Hash(task,appended==1?0x58:0x50):0});return true;}
inline CTask* __fastcall EnterDriver(void* raw,void*,CVehicle* vehicle){trace.push_back({"enter_driver",vehicle==train});memset(raw,0,0x50);return static_cast<CTask*>(raw);}
inline CTask* __fastcall EnterPassenger(void* raw,void*,CVehicle* vehicle,int32 seat,bool carry){trace.push_back({"enter_passenger",vehicle==train,seat,carry});memset(raw,0,0x50);return static_cast<CTask*>(raw);}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CTrain","AddNearbyPedAsRandomPassenger",0x6F8170,0,-1,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"train-addpassenger355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"train-addpassenger355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("train-addpassenger355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"train-addpassenger355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 auto& poolSlot=StaticRef<CPedPool*>(0xB74490);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x56E210,Player),pd(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),Player);
 EntryPatch c(0x571A00,Clean),cd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
 EntryPatch r(0x571B70,Register),rd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch g(0x5F7E80,Group),gd(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),Group);
 EntryPatch f(0x600EE0,Find),fd(bit_cast<uintptr_t>(&CPedIntelligence::FindTaskByType),Find);
 EntryPatch rm(0x610F20,RemovePed),rmd(bit_cast<uintptr_t>(&CPopulation::RemovePed),RemovePed);
 EntryPatch d(0x6528F0,Door),dd(bit_cast<uintptr_t>(&CCarEnterExit::GetNearestCarDoor),Door);
 EntryPatch inv(0x59B920,Inverse),invd(bit_cast<uintptr_t>(static_cast<CMatrix&(*)(CMatrix&,CMatrix&)>(&Invert)),Inverse);
 EntryPatch a(0x61A5A0,AllocateTask),ad(bit_cast<uintptr_t>(&CTask::operator new),AllocateTask);
 EntryPatch se(0x661DC0,Seek),sq(0x632BD0,Sequence),ap(0x632D10,Append),dr(0x6402F0,EnterDriver),ps(0x640340,EnterPassenger),st(0x681AF0,SetTask);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<std::array<uint8,sizeof(CPed)>,2> others{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,2> otherIntellect{};alignas(16) std::array<uint8,6*0x7C4> peds{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,6> intellect{};alignas(16) std::array<uint8,0x1C> activeTask{};std::array<uintptr_t,5> vtable{};vtable[4]=bit_cast<uintptr_t>(&Type);std::array<uint8,6> flags{};std::array<uintptr_t,6> pool{};
   auto run=[&](bool candidate){scenario=i;playerCalls=typeCalls=allocations=appended=0;trace=json::array();object.fill(0xCD);matrix.fill(0);memset(others.data(),0,sizeof(others));memset(otherIntellect.data(),0,sizeof(otherIntellect));peds.fill(0);memset(intellect.data(),0,sizeof(intellect));activeTask.fill(0);memset(tasks.data(),0xCD,sizeof(tasks));train=reinterpret_cast<CTrain*>(object.data());train->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data())=CVector{1.f,0.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{0.f,1.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x20)=CVector{0.f,0.f,1.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{0.f,0.f,0.f};
    players[0]=reinterpret_cast<CPed*>(others[0].data());players[1]=players[0];spawned=nullptr;for(size_t k=0;k<2;++k)*reinterpret_cast<CPedIntelligence**>(others[k].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(otherIntellect[k].data());players[0]->m_pVehicle=i%19==0?train:nullptr;
    *reinterpret_cast<uintptr_t*>(activeTask.data())=bit_cast<uintptr_t>(vtable.data());*reinterpret_cast<CVehicle**>(activeTask.data()+0xC)=i%2==0?train:nullptr;players[0]->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=i%7==0?reinterpret_cast<CTask*>(activeTask.data()):nullptr;
    train->trainFlags.bClockwiseDirection=i%2;train->m_nNumPassengersToLeave=uint8((i/7)%4);train->m_nNumPassengersToEnter=uint8(i%17==0?train->m_nNumPassengersToLeave:5);train->m_pTemporaryPassenger=i%3==0?reinterpret_cast<CPed*>(others[1].data()):nullptr;if(train->m_pTemporaryPassenger){auto* temp=train->m_pTemporaryPassenger;temp->bInVehicle=(i/9)%2;temp->bJustGotOffTrain=i%5==0?temp->bInVehicle:!temp->bInVehicle;temp->m_pVehicle=train;}
    const auto count=i%23==0?0:6;poolStorage=peds.data();poolIntellect=intellect[0].data();pool={bit_cast<uintptr_t>(peds.data()),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CPedPool*>(pool.data());for(int j=0;j<6;++j){auto* ped=reinterpret_cast<CPed*>(peds.data()+j*0x7C4);*reinterpret_cast<CPedIntelligence**>(reinterpret_cast<uint8*>(ped)+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect[size_t(j)].data());ped->SetCreatedBy(static_cast<ePedCreatedBy>((i+j)%9==0?2:1));ped->m_matrix=nullptr;ped->m_placement.m_vPosn=CVector{float((j-3)*4),float(j*2),float(j)};ped->m_nPedType=static_cast<ePedType>((i+j)%11==0?6:4);ped->m_nPedState=static_cast<ePedState>((i+j)%13==0?54:1);ped->bInVehicle=(i+j)%17==0;ped->bJustGotOffTrain=(i+j)%19==0;flags[size_t(j)]=uint8((i+j)%29==0?0x80:0);}
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(train);
    if(!candidate&&std::ranges::any_of(trace,[](const json& row){return row[0]=="set_task";}))++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"peds",Hash(peds.data(),peds.size())},{"intellect",Hash(intellect.data(),sizeof(intellect))},{"others",Hash(others.data(),sizeof(others))},{"other_intellect",Hash(otherIntellect.data(),sizeof(otherIntellect))},{"tasks",Hash(tasks.data(),sizeof(tasks))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 poolSlot=oldPool;bool restored=poolSlot==oldPool;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


