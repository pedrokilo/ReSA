from pathlib import Path
p=Path('reversing/TrainAddPassenger355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainPassenger355/LiveComparison.hpp').read_text().replace('TrainPassenger355Live','TrainAddPassenger355Live').replace('train-passenger355','train-addpassenger355').replace('#include "Population.h"','#include "Population.h"\n#include "PedPool.h"\n#include "PedGroups.h"\n#include "CarEnterExit.h"')
a=s.index('inline ReversibleHooks');s=s[:a]+'''inline uint8* poolStorage{};inline uint8* poolIntellect{};inline int allocations{},appended{};alignas(16) inline std::array<std::array<uint8,0x58>,3> tasks{};
inline int PedIndex(const void* raw){const auto address=uintptr_t(raw);return address>=uintptr_t(poolStorage)&&address<uintptr_t(poolStorage)+6*0x7C4?int((address-uintptr_t(poolStorage))/0x7C4):-1;}
inline CPedGroup* __cdecl Group(const CPed* ped){const auto index=PedIndex(ped);const auto r=(scenario+index)%11==0;trace.push_back({"group",index,r});return r?reinterpret_cast<CPedGroup*>(0x1000):nullptr;}
inline CTask* __fastcall Find(void* raw,void*,eTaskType type){const auto address=uintptr_t(raw);const auto index=address>=uintptr_t(poolIntellect)&&address<uintptr_t(poolIntellect)+6*sizeof(CPedIntelligence)?int((address-uintptr_t(poolIntellect))/sizeof(CPedIntelligence)):-1;const auto result=(scenario+index)%13==0;trace.push_back({"find",index,int(type),result});return result?reinterpret_cast<CTask*>(0x1000):nullptr;}
inline void __cdecl RemovePed(CPed* ped){trace.push_back({"remove_ped",PedIndex(ped)});ped->bInVehicle=false;}
inline bool __cdecl Door(const CPed* ped,const CVehicle*,CVector& pos,int32& door){pos={7.f,8.f,9.f};door=0;trace.push_back({"door",PedIndex(ped)});if(scenario%7==0)train->trainFlags.bClockwiseDirection=!train->trainFlags.bClockwiseDirection;return scenario%3!=0;}
inline CMatrix& __cdecl Inverse(CMatrix& in,CMatrix& out){trace.push_back({"inverse"});out.GetRight()={1.f,0.f,0.f};out.GetForward()={0.f,1.f,0.f};out.GetUp()={0.f,0.f,1.f};out.GetPosition()=-in.GetPosition();return out;}
inline void* __cdecl AllocateTask(size_t size){const auto index=allocations++;trace.push_back({"allocate",size,index});return index==2&&scenario%17==0?nullptr:tasks[size_t(index)].data();}
inline CTask* __fastcall Seek(void* raw,void*,CEntity* entity,int32 seek,int32 scan,float distance,float one,float two,bool flag0,bool flag1){trace.push_back({"seek",entity==train,seek,scan,std::bit_cast<uint32>(distance),std::bit_cast<uint32>(one),std::bit_cast<uint32>(two),flag0,flag1});memset(raw,0,0x58);return static_cast<CTask*>(raw);}
inline CTask* __fastcall Sequence(void* raw,void*){trace.push_back({"sequence"});memset(raw,0,0x40);return static_cast<CTask*>(raw);}
inline bool __fastcall Append(void*,void*,CTask* task){trace.push_back({"append",appended++,task!=nullptr,task?Hash(task,appended==1?0x58:0x50):0});return true;}
inline CTask* __fastcall EnterDriver(void* raw,void*,CVehicle* vehicle){trace.push_back({"enter_driver",vehicle==train});memset(raw,0,0x50);return static_cast<CTask*>(raw);}
inline CTask* __fastcall EnterPassenger(void* raw,void*,CVehicle* vehicle,int32 seat,bool carry){trace.push_back({"enter_passenger",vehicle==train,seat,carry});memset(raw,0,0x50);return static_cast<CTask*>(raw);}
'''+s[a:]
s=s.replace('"RemoveRandomPassenger",0x6F6850','"AddNearbyPedAsRandomPassenger",0x6F8170')
a=s.index(' const auto oldReplay=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' auto& poolSlot=StaticRef<CPedPool*>(0xB74490);const auto oldPool=poolSlot;uint32 pairs=0,totalFailures=0;
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
'''+s[b:]
a=s.index('  for(int i=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CTrain)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<std::array<uint8,sizeof(CPed)>,2> others{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,2> otherIntellect{};alignas(16) std::array<uint8,6*0x7C4> peds{};alignas(16) std::array<std::array<uint8,sizeof(CPedIntelligence)>,6> intellect{};alignas(16) std::array<uint8,0x1C> activeTask{};std::array<uintptr_t,5> vtable{};vtable[4]=bit_cast<uintptr_t>(&Type);std::array<uint8,6> flags{};std::array<uintptr_t,6> pool{};
   auto run=[&](bool candidate){scenario=i;playerCalls=typeCalls=allocations=appended=0;trace=json::array();object.fill(0xCD);matrix.fill(0);memset(others.data(),0,sizeof(others));memset(otherIntellect.data(),0,sizeof(otherIntellect));peds.fill(0);memset(intellect.data(),0,sizeof(intellect));activeTask.fill(0);memset(tasks.data(),0xCD,sizeof(tasks));train=reinterpret_cast<CTrain*>(object.data());train->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data())={1.f,0.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x10)={0.f,1.f,0.f};*reinterpret_cast<CVector*>(matrix.data()+0x20)={0.f,0.f,1.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)={0.f,0.f,0.f};
    players[0]=reinterpret_cast<CPed*>(others[0].data());players[1]=players[0];spawned=nullptr;for(size_t k=0;k<2;++k)*reinterpret_cast<CPedIntelligence**>(others[k].data()+0x47C)=reinterpret_cast<CPedIntelligence*>(otherIntellect[k].data());players[0]->m_pVehicle=i%19==0?train:nullptr;
    *reinterpret_cast<uintptr_t*>(activeTask.data())=bit_cast<uintptr_t>(vtable.data());*reinterpret_cast<CVehicle**>(activeTask.data()+0xC)=i%2==0?train:nullptr;players[0]->GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY]=i%7==0?reinterpret_cast<CTask*>(activeTask.data()):nullptr;
    train->trainFlags.bClockwiseDirection=i%2;train->m_nNumPassengersToLeave=uint8((i/7)%4);train->m_nNumPassengersToEnter=uint8(i%17==0?train->m_nNumPassengersToLeave:5);train->m_pTemporaryPassenger=i%3==0?reinterpret_cast<CPed*>(others[1].data()):nullptr;if(train->m_pTemporaryPassenger){auto* temp=train->m_pTemporaryPassenger;temp->bInVehicle=(i/9)%2;temp->bJustGotOffTrain=i%5==0?temp->bInVehicle:!temp->bInVehicle;temp->m_pVehicle=train;}
    const auto count=i%23==0?0:6;poolStorage=peds.data();poolIntellect=intellect[0].data();pool={bit_cast<uintptr_t>(peds.data()),bit_cast<uintptr_t>(flags.data()),uintptr_t(count),0,0,0};poolSlot=reinterpret_cast<CPedPool*>(pool.data());for(int j=0;j<6;++j){auto* ped=reinterpret_cast<CPed*>(peds.data()+j*0x7C4);*reinterpret_cast<CPedIntelligence**>(reinterpret_cast<uint8*>(ped)+0x47C)=reinterpret_cast<CPedIntelligence*>(intellect[size_t(j)].data());ped->SetCreatedBy(static_cast<ePedCreatedBy>((i+j)%9==0?2:1));ped->m_matrix=nullptr;ped->m_placement.m_vPosn={float((j-3)*4),float(j*2),float(j)};ped->m_nPedType=static_cast<ePedType>((i+j)%11==0?6:4);ped->m_nPedState=static_cast<ePedState>((i+j)%13==0?54:1);ped->bInVehicle=(i+j)%17==0;ped->bJustGotOffTrain=(i+j)%19==0;flags[size_t(j)]=uint8((i+j)%29==0?0x80:0);}
'''+s[b:]
s=s.replace('return row[0]=="spawn";','return row[0]=="set_task";')
a=s.index('    return json');b=s.index('   };const auto original',a)
s=s[:a]+'''    return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"peds",Hash(peds.data(),peds.size())},{"intellect",Hash(intellect.data(),sizeof(intellect))},{"others",Hash(others.data(),sizeof(others))},{"other_intellect",Hash(otherIntellect.data(),sizeof(otherIntellect))},{"tasks",Hash(tasks.data(),sizeof(tasks))},{"trace",trace}};
'''+s[b:]
s=s.replace(' StaticRef<uint8>(0xA43088)=oldReplay;bool restored=true;',' poolSlot=oldPool;bool restored=poolSlot==oldPool;')
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/TrainPassenger355',name).read_text().replace('TrainPassenger355Live','TrainAddPassenger355Live').replace('train-passenger355','train-addpassenger355')
 if name=='build_probe.py':t=t.replace("'PedIntelligence.h'","'PedIntelligence.cpp','PedIntelligence.h','Pools/PedPool.h','PedGroups.cpp','PedGroups.h','CarEnterExit.cpp','CarEnterExit.h','Core/Matrix.cpp','Core/Matrix.h','Core/Vector.h'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('AddNearbyPedAsRandomPassenger 0x6F8170: 1024 isolated EXE/DLL pairs prepared, runtime pending. Player/task gates, temporary passenger cleanup/removal, used/free ped slots, creation/group/type/state/task filters, planar distance/side selection, nearest door, inverse transform, direction captured before door mutation, allocation and sequence order. Full train/ped/intelligence/task hashes and exact dependency trace; ped pool restored. Matrix is valid and first two allocations succeed; third allocation may fail. Existing task ctor/sequence/SetTask EXE dependencies outside this scope are symmetrically isolated; no self-forward. Evidence ../VehicleAudio355Evidence/Train_6F8170.txt.\n')
source=Path('source/game_sa/Entity/Vehicle/Train.cpp');t=source.read_text();a=t.index('void CTrain::AddNearbyPedAsRandomPassenger()');b=t.index('// 0x6F86A0',a);t=t[:a]+Path('reversing/VehicleAudio355Evidence/Train_AddPassengerNative.txt').read_text()+'\n\n'+t[b:];source.write_text(t)
