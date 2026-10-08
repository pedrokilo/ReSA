from pathlib import Path
p=Path('reversing/CarPassenger355');p.mkdir(exist_ok=True)
for name in ['build_probe.py','run_local.py']:
 s=Path('reversing/ProjectileCore355',name).read_text(encoding='utf-8-sig').replace('ProjectileCore355','CarPassenger355').replace('projectile-core355','car-passenger355')
 if name=='build_probe.py':s=s.replace("'source/game_sa/ProjectileInfo.cpp', 'source/game_sa/ProjectileInfo.h', 'source/game_sa/Entity/Object/Projectile.h'","'source/game_sa/CarEnterExit.cpp', 'source/game_sa/CarEnterExit.h', 'source/game_sa/Events/EventPedEnteredMyVehicle.cpp', 'source/game_sa/Events/EventPedEnteredMyVehicle.h'")
 (p/name).write_text(s,encoding='utf8')
s=Path('reversing/DecisionPending/LiveComparison.hpp').read_text().replace('DecisionPendingLive','CarPassenger355Live').replace('decision-pending','car-passenger355').replace('#include "DecisionMakers/Decision.h"','#include "CarEnterExit.h"\n#include "Events/EventPedEnteredMyVehicle.h"\n#include "Events/EventGroup.h"')
start=s.index('inline const std::array<Fn,2>');end=s.index('inline ReversibleHooks',start)
s=s[:start]+'inline const std::array<Fn,1> fns{{{"CCarEnterExit","MakeUndraggedPassengerPedsLeaveCar",0x64F540,0,-1,1024}}};\n'+s[end:]
s=s.replace('inline bool done{};', '''inline json trace;inline uintptr_t base{},groupBase{};
inline int32 __cdecl Door(const CVehicle* vehicle,const CPed* ped){const auto index=(uintptr_t(ped)-base)/sizeof(CPed);trace.push_back({"door",index});return int32(index%4)+8;}
inline void __fastcall Ref(CEntity* e,void*,CEntity**){trace.push_back({"register",uintptr_t(e)});}
inline void __fastcall Clean(CEntity* e,void*,CEntity**){trace.push_back({"clean",uintptr_t(e)});}
inline CEvent* __fastcall Add(CEventGroup* self,void*,CEvent* event,bool valid){auto* e=static_cast<CEventPedEnteredMyVehicle*>(event);trace.push_back({"add",(uintptr_t(self)-groupBase)/0x300,uintptr_t(e->m_PedThatEntered),uintptr_t(e->m_Vehicle),int(e->m_TargetDoor),valid});return nullptr;}
inline bool done{};''').replace('inline bool done{}; inline std::ofstream log; inline json trace;','inline bool done{}; inline std::ofstream log;')
s=s.replace(' bool passed=true;uint32 pairCount=0;',''' bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x64F110,Door),ad(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),Door);
 EntryPatch b(0x571B70,Ref),bd(bit_cast<uintptr_t>(&CEntity::RegisterReference),Ref);
 EntryPatch c(0x571A00,Clean),cd(bit_cast<uintptr_t>(&CEntity::CleanUpOldReference),Clean);
 EntryPatch d(0x4AB420,Add),dd(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),Add);''')
start=s.index('    alignas(CDecision)');end=s.index('\n   };',start)
s=s[:start]+'''    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> vehicle{};
    alignas(CPed) std::array<std::array<uint8,sizeof(CPed)>,9> peds{};
    alignas(16) std::array<std::array<uint8,0x300>,9> intelligence{};
    auto* veh=reinterpret_cast<CVehicle*>(vehicle.data());veh->m_nMaxPassengers=uint8(i%9);
    for(size_t k=0;k<9;++k){auto* ped=reinterpret_cast<CPed*>(peds[k].data());*reinterpret_cast<void**>(peds[k].data()+0x47C)=intelligence[k].data();ped->bStayInCarOnJack=(i+k)%3==0;if(k<8)veh->m_apPassengers[k]=(i+k)%5==0?nullptr:ped;}
    base=uintptr_t(peds.data());groupBase=uintptr_t(intelligence.data())+0x68;trace=json::array();
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    reinterpret_cast<void(__cdecl*)(const CVehicle*,const CPed*,const CPed*)>(fn.address)(veh,i%2==0?reinterpret_cast<CPed*>(peds[i%8].data()):nullptr,reinterpret_cast<CPed*>(peds[8].data()));
    return json{{"vehicle",ReversingProbe::Bytes(vehicle.data(),vehicle.size())},{"peds",ReversingProbe::Bytes(peds.data(),sizeof(peds))},{"intelligence",ReversingProbe::Bytes(intelligence.data(),sizeof(intelligence))},{"trace",trace},{"switches",switched}};''' +s[end:]
(p/'LiveComparison.hpp').write_text(s,encoding='utf8')
