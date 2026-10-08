from pathlib import Path
p=Path('reversing/TrainPassenger355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainPosition355/LiveComparison.hpp').read_text().replace('TrainPosition355Live','TrainPassenger355Live').replace('train-position355','train-passenger355').replace('#include "TrainNode.h"','#include "TrainNode.h"\n#include "Population.h"')
a=s.index('inline bool done{};');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},playerCalls{},typeCalls{},draw{};
inline CTrain* train{};inline CPed* players[2]{};inline CPed* spawned{};alignas(16) inline std::array<uint8,0x1C> allocated{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\\n';log.flush();}
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
'''+s[b:]
s=s.replace('"FindPositionOnTrackFromCoors",0x6F6CC0','"RemoveRandomPassenger",0x6F6850')
a=s.index(' std::array<uint8,0x60> globals');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldReplay=StaticRef<uint8>(0xA43088);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x56E210,Player),pd(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),Player);
 EntryPatch r(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch c(0x571A00,Clean),cd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Clean);
 EntryPatch q(0x571B70,Register),qd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Register);
 EntryPatch s(0x613A00,Spawn),sd(bit_cast<uintptr_t>(&CPopulation::AddPedInCar),Spawn);
 EntryPatch a(0x61A5A0,Allocate),ad(bit_cast<uintptr_t>(&CTask::operator new),Allocate);
 EntryPatch t(0x63BDD0,Construct),m(0x681AF0,SetTask);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<std::array<uint8,sizeof(CPed)>,4> peds{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,4> intellect{};alignas(16) std::array<uint8,0x1C> activeTask{};std::array<uintptr_t,5> vtable{};vtable[4]=bit_cast<uintptr_t>(&Type);
   auto run=[&](bool candidate){scenario=i;playerCalls=typeCalls=draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(peds.data(),0,peds.size()*sizeof(peds[0]));memset(intellect.data(),0,intellect.size()*sizeof(intellect[0]));memset(activeTask.data(),0,activeTask.size());allocated.fill(0xCD);train=reinterpret_cast<CTrain*>(object.data());players[0]=reinterpret_cast<CPed*>(peds[0].data());players[1]=reinterpret_cast<CPed*>(peds[1].data());spawned=reinterpret_cast<CPed*>(peds[3].data());
    for(size_t k=0;k<4;++k){auto* ped=reinterpret_cast<CPed*>(peds[k].data());*reinterpret_cast<CPedIntelligence**>(peds[k].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect[k].data());ped->bInVehicle=((i/13+int(k))%3)==0;ped->m_pVehicle=(i/19+int(k))%4==0?train:nullptr;}
    *reinterpret_cast<uintptr_t*>(activeTask.data())=bit_cast<uintptr_t>(vtable.data());*reinterpret_cast<CVehicle**>(activeTask.data()+0xC)=i%2==0?train:nullptr;players[0]->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=i%3==0?nullptr:reinterpret_cast<CTask*>(activeTask.data());
    train->trainFlags.bClockwiseDirection=i%2;train->m_pDriver=i%4==0?players[1]:nullptr;train->m_nNumPassengersToLeave=uint8((i/7)%5);train->m_nNumPassengersToEnter=9;train->m_pTemporaryPassenger=i%3==0?nullptr:reinterpret_cast<CPed*>(peds[2].data());StaticRef<uint8>(0xA43088)=uint8(i%23==0?1:0);
'''+s[b:]
a=s.index('    return json');b=s.index('   };const auto original',a)
s=s[:a]+'''    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"peds",Hash(peds.data(),sizeof(peds))},{"intellect",Hash(intellect.data(),sizeof(intellect))},{"allocation",Hash(allocated.data(),allocated.size())},{"trace",trace}};
'''+s[b:]
a=s.index(' memcpy(reinterpret_cast<void*>(0xC37FEC)');b=s.index(' for(size_t n=',a)
s=s[:a]+' StaticRef<uint8>(0xA43088)=oldReplay;bool restored=true;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/TrainPosition355',name).read_text().replace('TrainPosition355Live','TrainPassenger355Live').replace('train-position355','train-passenger355')
 if name=='build_probe.py':t=t.replace("'TrainNode.cpp'","'TrainNode.cpp','Population.cpp','Population.h','Entity/Entity.cpp','Entity/Entity.h','General.cpp','General.h','Tasks/Task.cpp','Tasks/Task.h','Tasks/TaskManager.cpp','Tasks/TaskManager.h','PedIntelligence.h'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('RemoveRandomPassenger 0x6F6850: 1024 EXE/DLL pairs prepared, runtime pending. Replay, changing player queries, task-type short circuit, direction/driver, pending passenger state, generation counters, RNG thresholds, spawn/allocation failures and registration order. Full train/ped/intelligence/allocation hashes plus exact dependency trace. Replay byte restored. Existing task constructor 0x63BDD0 and SetTask 0x681AF0 remain EXE dependencies outside this scope, symmetrically isolated by bank; no self-forward. Evidence ../VehicleAudio355Evidence/Train_6F6850.txt.\n')
source=Path('source/game_sa/Entity/Vehicle/Train.cpp');t=source.read_text();a=t.index('void CTrain::RemoveRandomPassenger()');b=t.index('// 0x6F6A20',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Train_RemovePassengerNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
