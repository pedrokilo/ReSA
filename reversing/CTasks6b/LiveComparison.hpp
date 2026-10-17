#pragma once
// Banco dentro del motor: tareas de salto, huida, tienda y coche (lista de 6, 42 funciones).
// CTaskComplexJump (Clone, constructor, CreateFirstSubTask, CreateNextSubTask, CreateSubTask, MakeAbortable),
// CTaskComplexWanderFlee (Clone, constructor, ControlSubTask, destructor, GetWanderType, ScanForStuff),
// CTaskInteriorBeInShop (constructor, destructor, Clone, GetTaskType, GetWanderType, ScanForStuff) y
// CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse, CTaskSimpleCarSetPedSlowDraggedOut, CTaskSimpleCarSetTempAction,
// CTaskSimpleCarWaitToSlowDown (Clone, constructor, destructor, GetTaskType, MakeAbortable, ProcessPed).
// Original (hooks del lote y CTaskComplexWander::ControlSubTask desactivados: el ControlSubTask de WanderFlee del EXE
// es un salto a 0x674C30) frente a candidato con fixtures identicos en buffers propios: peatones (vtable falsa, hueco 23
// SetMoveAnim), vehiculos (vtable falsa, huecos 57/58 CanPedStepOutCar/CanPedJumpOutCar), inteligencia, handling,
// subtareas y evento falsos (vtables trazadas) e informacion de interiores.
// Se simulan y trazan: rand/CGeneral::GetRandomNumber, CPed::Say, GetWanderTaskByPedType, CPedGroups::IsInPlayersGroup,
// CTaskSimpleCarDrive::MakeAbortable/ProcessPed (envoltorios del EXE), CVehicle::RemoveDriver/RemovePassenger/
// IsLawEnforcementVehicle/ChangeLawEnforcerState, CPed::RestartNonPartialAnims/UpdateStatLeavingVehicle/
// ReplaceWeaponWhenExitingVehicle/CalculateNewOrientation/PositionPedOutOfCollision, CTaskManager::SetTask
// (ChangeTaskInSlot en la DLL), CVehicleAnimGroup::ComputeAnimDoorOffsets, CPedPlacement::FindZCoorForPed,
// CCarEnterExit::IsRoomForPedToLeaveCar/GetPositionToOpenCarDoor, CTaskSimpleStandStill::ProcessPed,
// CTaskComplexWander::ComputeTargetPos/ScanForBlockedNodes/CreateFirstSubTask/CreateSubTask,
// InteriorGroup_c::FindClosestInteriorInfo, IKChainManager_c::LookAt y CEventGroup::Add.
// CCarEnterExit::ComputeDoorFlag (0x64E550) se sustituye en silencio: el EXE lo llama y descarta el resultado.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// Se guardan y restauran el reloj y CCarEnterExit::ms_vecPedGetUpAnimOffset.
#include <fstream>
#include <bit>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskComplexJump.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskComplexWanderFlee.h"
#include "Tasks/TaskTypes/Interior/TaskInteriorBeInShop.h"
#include "Tasks/TaskTypes/TaskSimpleCarGoToPointNearDoorUntilDoorNotInUse.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedSlowDraggedOut.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetTempAction.h"
#include "Tasks/TaskTypes/TaskSimpleCarWaitToSlowDown.h"
#include "Tasks/TaskTypes/TaskSimpleCarDrive.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Interior/InteriorGroup_c.h"
#include "Ragdoll/IKChainManager.h"
#include "Events/EventGroup.h"
#include "PedGroups.h"
#include "PedPlacement.h"
#include "CarEnterExit.h"
#include "VehicleAnimGroupData.h"
#include "PedIntelligence.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks6bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

constexpr int NPEDS=3, NVEH=3, FAKES=4, NINFO=3;
constexpr size_t FAKE_SIZE=0x50, FK_ABORT=0x40, FK_PROC=0x41, FK_TYPE=0x48, FK_CLONE=0x4C;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_HEADING=0x10, E_MATRIX=0x14, E_FLAGS=0x1C, E_TYPE=0x36, PH_MOVE=0x44;              // CEntity / CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_ROT_CUR=0x558, P_ROT_AIM=0x55C,
                 P_VEHICLE=0x58C, P_PEDTYPE=0x598;                                                             // CPed
constexpr size_t V_HANDLING=0x384, V_AUTOPILOT=0x390, V_DRIVER=0x460, V_PASS=0x464, V_NUMPASS=0x484, V_MAXPASS=0x488, V_DOORLOCK=0x4F8; // CVehicle
constexpr size_t H_ANIMGROUP=0xDE, I_STATIC_COUNTER=0x274;
constexpr uintptr_t TIMER=0xB7CB84, GETUP_OFS=0xC18C3C, ANIMGROUPS=0xC1CDC0, IKMAN=0xC15448;
constexpr uintptr_t VT_JUMP=0x870570, VT_FLEE=0x86F690, VT_SHOP=0x870244, VT_NEAR=0x86EDE0, VT_DRAG=0x86EF30, VT_TEMP=0x86EA6C, VT_WAIT=0x86ED98;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> event{};
alignas(16) inline std::array<std::array<uint8,0x40>,NINFO> infos{};
alignas(16) inline std::array<uint8,0x80> interior{}, group{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* PoolHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* PoolStorage() { return *reinterpret_cast<uint8**>(PoolHdr()); }
inline uint8* PoolStates() { return *reinterpret_cast<uint8**>(PoolHdr()+4); }
inline int32 PoolCap() { return *reinterpret_cast<int32*>(PoolHdr()+8); }
inline int32& PoolLast() { return At<int32>(PoolHdr(),0xC); }
constexpr size_t TASK_SLOT=0x80;
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(PoolStorage()); if(v<s||v>=s+size_t(PoolCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(PoolStates()[(v-s)/TASK_SLOT]&0x80);
}

inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,16> eventVtbl{};
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,80> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==uintptr_t(pedVtbl.data()))return "ped_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(v==IKMAN)return "ikman";
    if(v>=ANIMGROUPS&&v<ANIMGROUPS+0x94*32)return "animgroup+"+std::to_string(v-ANIMGROUPS);
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NINFO;++n)if(auto s=in(infos[n].data(),infos[n].size(),"info"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(handling.data(),handling.size(),"handling"))return *s;
    if(auto s=in(interior.data(),interior.size(),"interior"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json Vec(const CVector& v) { return {F(v.x),F(v.y),F(v.z)}; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, auxCalls{};
inline uintptr_t wanderRet{}; inline bool groupRet{}, carDriveAbortRet{}, carDriveProcRet{}, roomRet{}, outOfColRet{}, findZRet{}, findInfoRet{};
inline uint32 stepMask{}, jumpMask{}, lawMask{};
inline uintptr_t findInfo{}; inline float findSome{};
inline int eventTypeRet{};

// tareas falsas (destructor 0, Clone 1, GetTaskType 4, MakeAbortable 6, ProcessPed 7)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f&0xFF}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type_fake",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
inline bool __fastcall FProcess(CTask* t,void*,CPed* p) { trace.push_back({"process_ped_fake",Name(t),Name(p),Region(t,0x20,true)}); return At<uint8>(t,FK_PROC)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(e)}); return eventTypeRet; }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// peaton: vtable falsa (hueco 23 = SetMoveAnim)
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(p)}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline void __fastcall SetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(p)}); }
// vehiculo: vtable falsa (hueco 57 = CanPedStepOutCar, 58 = CanPedJumpOutCar)
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(v)}); return nullptr; }
template<int... N> void FillVTraps(std::integer_sequence<int,N...>) { ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline int VehIndex(const void* v) { for(int n=0;n<NVEH;++n)if(v==vehs[n].data())return n; return -1; }
inline bool __fastcall VStep(CVehicle* v,void*,uint32 ignore) { const int n=VehIndex(v); const bool r=n>=0&&((stepMask>>n)&1); trace.push_back({"can_ped_step_out_car",Name(v),ignore&0xFF,r}); return r; }
inline bool __fastcall VJump(CVehicle* v,void*,CPed* p) { const int n=VehIndex(v); const bool r=n>=0&&((jumpMask>>n)&1); trace.push_back({"can_ped_jump_out_car",Name(v),Name(p),r}); return r; }

// dependencias simuladas
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline CTaskComplexWander* __cdecl FakeWanderByType(CPed* p) { trace.push_back({"wander_task_by_ped_type",Name(p)}); return reinterpret_cast<CTaskComplexWander*>(wanderRet); }
inline bool __cdecl FakeInPlayersGroup(CPed* p) { trace.push_back({"is_in_players_group",Name(p),groupRet}); return groupRet; }
inline bool __fastcall FakeCarDriveAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"car_drive_make_abortable",Name(t),Name(p),prio,Name(e)}); return carDriveAbortRet; }
inline bool __fastcall FakeCarDriveProcess(CTask* t,void*,CPed* p) { trace.push_back({"car_drive_process_ped",Name(t),Name(p),Name(At(t,8))}); return carDriveProcRet; }
inline void __fastcall FakeRemoveDriver(CVehicle* v,void*,uint32 b) { trace.push_back({"remove_driver",Name(v),b&0xFF}); }
inline void __fastcall FakeRemovePassenger(CVehicle* v,void*,CPed* p) {
    trace.push_back({"remove_passenger",Name(v),Name(p)});
    // se quita del primer asiento que ocupa (como el original) y se descuenta m_nNumPassengers
    auto* b=reinterpret_cast<uint8*>(v); for(int k=0;k<8;++k)if(At<CPed*>(b,V_PASS+4*k)==p){ At<CPed*>(b,V_PASS+4*k)=nullptr; if(b[V_NUMPASS])--b[V_NUMPASS]; break; }
}
inline bool __fastcall FakeIsLaw(const CVehicle* v,void*) { const int n=VehIndex(v); const bool r=n>=0&&((lawMask>>n)&1); trace.push_back({"is_law_enforcement_vehicle",Name(v),r}); return r; }
inline void __fastcall FakeChangeLaw(CVehicle* v,void*,uint32 b) { trace.push_back({"change_law_enforcer_state",Name(v),b&0xFF}); }
inline void __fastcall FakeRestartAnims(CPed* p,void*) { trace.push_back({"restart_non_partial_anims",Name(p)}); }
inline void __fastcall FakeUpdateStat(CPed* p,void*) { trace.push_back({"update_stat_leaving_vehicle",Name(p)}); }
inline void __fastcall FakeReplaceWeapon(CPed* p,void*) { trace.push_back({"replace_weapon_when_exiting_vehicle",Name(p)}); }
inline void __fastcall FakeCalcOrientation(CPed* p,void*) { trace.push_back({"calculate_new_orientation",Name(p),F(At<float>(p,P_ROT_CUR)),F(At<float>(p,P_ROT_AIM))}); }
inline bool __fastcall FakeOutOfCollision(CPed* p,void*,int32 door,CVehicle* v,uint32 b) { trace.push_back({"position_ped_out_of_collision",Name(p),door,Name(v),b&0xFF}); return outOfColRet; }
inline void __fastcall FakeSetTask(void* tm,void*,CTask* t,int32 idx,uint32) { trace.push_back({"set_task",Name(tm),Name(t),idx}); }
inline void __fastcall FakeChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task",Name(tm),Name(t),int32((uintptr_t(slot)-uintptr_t(tm))/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
inline CVector* __fastcall FakeDoorOffsets(void* grp,void*,CVector* out,int32 type) {
    trace.push_back({"compute_anim_door_offsets",Name(grp),type});
    *out=CVector{float(int(seed%13))*0.125f-0.75f+0.0625f*float(type),float(int(seed%7))*0.25f-0.5f,float(int(seed%5))*0.5f}; return out;
}
inline float NewZ(const CVector& p) { return p.z*0.5f+float(int(seed%9))-3.25f; }
inline bool __cdecl FakeFindZExe(CVector* p) { trace.push_back({"find_z_for_ped",Vec(*p),findZRet}); if(findZRet)p->z=NewZ(*p); return findZRet; }
inline std::pair<CVector,bool> __cdecl FakeFindZDll(CVector p) { trace.push_back({"find_z_for_ped",Vec(p),findZRet}); if(findZRet)p.z=NewZ(p); return {p,findZRet}; }
inline bool __cdecl FakeIsRoom(const CVehicle* v,int32 door,const CVector* pos) { trace.push_back({"is_room_for_ped_to_leave_car",Name(v),door,pos ? Vec(*pos) : json(nullptr)}); return roomRet; }
inline int32 __cdecl FakeDoorFlag(const CVehicle*,int32,uint32) { return 0; } // solo lo llama el EXE y descarta el resultado
inline CVector __cdecl FakeOpenDoorPos(const CVehicle* v,int32 door) {
    trace.push_back({"position_to_open_car_door",Name(v),door}); const uint32 c=auxCalls++;
    return CVector{float(int(seed%97))*0.5f-24.f+0.25f*float(c),float(int(seed%89))*0.25f-11.f,float(int(seed%11))*0.125f};
}
inline bool __fastcall FakeStandStillProcess(CTask* t,void*,CPed* p) {
    trace.push_back({"stand_still_process_ped",int(t->GetTaskType()),At(t,4),At<int32>(t,8),At(t,0xC),At(t,0x10),At<uint8>(t,0x14),At<uint8>(t,0x15),At<uint8>(t,0x18),At<uint8>(t,0x19),At(t,0x1C),Name(p)});
    return false;
}
inline void __fastcall FakeComputeTargetPos(void* t,void*,const CPed* p,CVector* out,const CNodeAddress* node) {
    trace.push_back({"compute_target_pos",Name(t),Name(p),Name(node),*reinterpret_cast<const uint32*>(node)});
    *out=CVector{float(int(seed%31))-15.5f,float(int(seed%17))*0.5f,float(int(seed%3))+0.25f};
}
inline void __fastcall FakeScanBlocked(void* t,void*,CPed* p) { trace.push_back({"scan_for_blocked_nodes",Name(t),Name(p)}); }
inline CTask* __fastcall FakeWanderFirst(void* t,void*,CPed* p) { trace.push_back({"wander_create_first_sub_task",Name(t),Name(p)}); return Fake(2); }
inline CTask* __fastcall FakeWanderSub(void* t,void*,CPed* p,int32 type) { trace.push_back({"wander_create_sub_task",Name(t),Name(p),type}); return Fake(3); }
inline bool __fastcall FakeFindInfo(void* g,void*,int32 a,CVector pt,float r,void** info,void** in,float* some) {
    trace.push_back({"find_closest_interior_info",Name(g),a,Vec(pt),F(r)});
    if(findInfoRet){ *info=reinterpret_cast<void*>(findInfo); *in=interior.data(); *some=findSome; }
    return findInfoRet;
}
inline void __fastcall FakeLookAt(void* m,void*,const char* why,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"ik_look_at",Name(m),why ? std::string(why) : std::string("<null>"),Name(p),Name(e),time,bone,Name(off),off ? Vec(*off) : json(nullptr),torso,F(speed),blend,prio,force});
}
inline CEvent* __fastcall FakeGroupAdd(void* g,void*,CEvent* e,uint32 valid) {
    trace.push_back({"event_group_add",Name(g),int(e->GetEventType()),At<int32>(e,4),At<uint8>(e,8),Name(At(e,0xC)),Name(At(e,0x10)),At(e,0x14),At<uint8>(e,0x18),valid&0xFF});
    return nullptr;
}

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexJump","Clone",0,0x870570,1,128},
    {"CTaskComplexJump","Constructor",0x67A030,0,-1,128},
    {"CTaskComplexJump","CreateFirstSubTask",0,0x870570,9,256},
    {"CTaskComplexJump","CreateNextSubTask",0,0x870570,8,1024},
    {"CTaskComplexJump","CreateSubTask",0x67D980,0,-1,1024},
    {"CTaskComplexJump","MakeAbortable",0,0x870570,6,512},
    {"CTaskComplexWanderFlee","Clone",0,0x86F690,1,128},
    {"CTaskComplexWanderFlee","Constructor",0x65B320,0,-1,128},
    {"CTaskComplexWanderFlee","ControlSubTask",0,0x86F690,10,1024},
    {"CTaskComplexWanderFlee","Destructor",0x65B370,0,-1,128},
    {"CTaskComplexWanderFlee","GetWanderType",0,0x86F690,11,64},
    {"CTaskComplexWanderFlee","ScanForStuff",0,0x86F690,12,64},
    {"CTaskInteriorBeInShop","Clone",0,0x870244,1,256},
    {"CTaskInteriorBeInShop","Constructor",0x6753E0,0,-1,256},
    {"CTaskInteriorBeInShop","Destructor",0x675460,0,-1,128},
    {"CTaskInteriorBeInShop","GetTaskType",0,0x870244,4,64},
    {"CTaskInteriorBeInShop","GetWanderType",0,0x870244,11,64},
    {"CTaskInteriorBeInShop","ScanForStuff",0,0x870244,12,1024},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","Clone",0,0x86EDE0,1,256},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","Constructor",0x646D70,0,-1,256},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","Destructor",0x646E50,0,-1,128},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","GetTaskType",0,0x86EDE0,4,64},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","MakeAbortable",0,0x86EDE0,6,64},
    {"CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse","ProcessPed",0,0x86EDE0,7,1024},
    {"CTaskSimpleCarSetPedSlowDraggedOut","Clone",0,0x86EF30,1,128},
    {"CTaskSimpleCarSetPedSlowDraggedOut","Constructor",0x6483B0,0,-1,128},
    {"CTaskSimpleCarSetPedSlowDraggedOut","Destructor",0x648430,0,-1,128},
    {"CTaskSimpleCarSetPedSlowDraggedOut","GetTaskType",0,0x86EF30,4,64},
    {"CTaskSimpleCarSetPedSlowDraggedOut","MakeAbortable",0,0x86EF30,6,64},
    {"CTaskSimpleCarSetPedSlowDraggedOut","ProcessPed",0,0x86EF30,7,1024},
    {"CTaskSimpleCarSetTempAction","Clone",0,0x86EA6C,1,256},
    {"CTaskSimpleCarSetTempAction","Constructor",0x63D6F0,0,-1,256},
    {"CTaskSimpleCarSetTempAction","Destructor",0x63D730,0,-1,128},
    {"CTaskSimpleCarSetTempAction","GetTaskType",0,0x86EA6C,4,64},
    {"CTaskSimpleCarSetTempAction","MakeAbortable",0,0x86EA6C,6,512},
    {"CTaskSimpleCarSetTempAction","ProcessPed",0,0x86EA6C,7,1024},
    {"CTaskSimpleCarWaitToSlowDown","Clone",0,0x86ED98,1,128},
    {"CTaskSimpleCarWaitToSlowDown","Constructor",0x646990,0,-1,128},
    {"CTaskSimpleCarWaitToSlowDown","Destructor",0x646A00,0,-1,128},
    {"CTaskSimpleCarWaitToSlowDown","GetTaskType",0,0x86ED98,4,64},
    {"CTaskSimpleCarWaitToSlowDown","MakeAbortable",0,0x86ED98,6,512},
    {"CTaskSimpleCarWaitToSlowDown","ProcessPed",0,0x86ED98,7,1024},
}};
// hooks que el codigo del EXE alcanza por salto/llamada directa: se desactivan en la ejecucion original
inline const std::array<std::pair<const char*,const char*>,1> baseHooks{{
    {"CTaskComplexWander","ControlSubTask"},
}};
inline size_t ObjSize(const std::string& cls) {
    if(cls=="CTaskComplexJump")return 0x14; if(cls=="CTaskComplexWanderFlee")return 0x28; if(cls=="CTaskInteriorBeInShop")return 0x34;
    if(cls=="CTaskSimpleCarSetTempAction")return 0x68; if(cls=="CTaskSimpleCarWaitToSlowDown")return 0x14; return 0x10;
}

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks6b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks6b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks6b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks6b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> extra;
    for(auto& [c,n]:baseHooks){ if(auto* h=FindHook(c,n))extra.push_back(h); else {Write({{"event","error"},{"reason","hook not found"},{"function",std::string(c)+"::"+n}});return;} }
    const uintptr_t dllWanderFirst=OurVirtual("CTaskComplexWander","CreateFirstSubTask"), dllStandStill=OurVirtual("CTaskSimpleStandStill","ProcessPed"),
        dllCarAbort=OurVirtual("CTaskSimpleCarDrive","MakeAbortable"), dllCarProc=OurVirtual("CTaskSimpleCarDrive","ProcessPed");
    if(!dllWanderFirst||!dllStandStill||!dllCarAbort||!dllCarProc){Write({{"event","error"},{"reason","virtual hook not found"},{"wander_first",dllWanderFirst!=0},{"stand_still",dllStandStill!=0}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    fakeVtbl[7]=reinterpret_cast<void*>(&FProcess);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&SetMoveAnim);
    FillVTraps(std::make_integer_sequence<int,80>{}); vehVtbl[57]=reinterpret_cast<void*>(&VStep); vehVtbl[58]=reinterpret_cast<void*>(&VJump);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{GETUP_OFS,12}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::vector<bool> extraEnabled,extraLocked; for(auto* h:extra){extraEnabled.push_back(h->Hooked());extraLocked.push_back(h->Locked());h->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); for(auto* h:extra)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch s2(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),FakeWanderByType), s2e(0x673D00,FakeWanderByType);
        EntryPatch s3(bit_cast<uintptr_t>(&CPedGroups::IsInPlayersGroup),FakeInPlayersGroup), s3e(0x5F7F10,FakeInPlayersGroup);
        EntryPatch s4(Follow(dllCarAbort),FakeCarDriveAbort), s4e(0x63C670,FakeCarDriveAbort), s5(Follow(dllCarProc),FakeCarDriveProcess), s5e(0x644470,FakeCarDriveProcess);
        // CTaskSimpleCarDrive::MakeAbortable/ProcessPed ya estan reconstruidos: las llamadas calificadas de la DLL (CarSetTempAction) entran en el cuerpo de la DLL, no en el EXE
        EntryPatch v1(bit_cast<uintptr_t>(&CVehicle::RemoveDriver),FakeRemoveDriver), v1e(0x6D1950,FakeRemoveDriver);
        EntryPatch v2(bit_cast<uintptr_t>(&CVehicle::RemovePassenger),FakeRemovePassenger), v2e(0x6D1610,FakeRemovePassenger);
        EntryPatch v3(bit_cast<uintptr_t>(&CVehicle::IsLawEnforcementVehicle),FakeIsLaw), v3e(0x6D2370,FakeIsLaw);
        EntryPatch v4(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),FakeChangeLaw), v4e(0x6D2330,FakeChangeLaw);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::RestartNonPartialAnims),FakeRestartAnims), p1e(0x5DED50,FakeRestartAnims);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::UpdateStatLeavingVehicle),FakeUpdateStat), p2e(0x5E01B0,FakeUpdateStat);
        EntryPatch p3(bit_cast<uintptr_t>(&CPed::ReplaceWeaponWhenExitingVehicle),FakeReplaceWeapon), p3e(0x5E6490,FakeReplaceWeapon);
        EntryPatch p4(bit_cast<uintptr_t>(&CPed::CalculateNewOrientation),FakeCalcOrientation), p4e(0x5E52E0,FakeCalcOrientation);
        EntryPatch p5(bit_cast<uintptr_t>(&CPed::PositionPedOutOfCollision),FakeOutOfCollision), p5e(0x5E0820,FakeOutOfCollision);
        EntryPatch t1(TaskManagerAccess::ChangeSlotAddress(),FakeChangeSlot), t1e(0x681AF0,FakeSetTask);
        EntryPatch g1(bit_cast<uintptr_t>(&CVehicleAnimGroup::ComputeAnimDoorOffsets),FakeDoorOffsets), g1e(0x6E3D10,FakeDoorOffsets);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedPlacement::FindZCoorForPed),FakeFindZDll), g2e(0x616920,FakeFindZExe);
        EntryPatch g3(bit_cast<uintptr_t>(&CCarEnterExit::IsRoomForPedToLeaveCar),FakeIsRoom), g3e(0x6504C0,FakeIsRoom);
        EntryPatch g4e(0x64E550,FakeDoorFlag);
        EntryPatch g5(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),FakeOpenDoorPos), g5e(0x64E740,FakeOpenDoorPos);
        EntryPatch g6(Follow(dllStandStill),FakeStandStillProcess), g6e(0x62F370,FakeStandStillProcess);
        EntryPatch w1(bit_cast<uintptr_t>(&CTaskComplexWander::ComputeTargetPos),FakeComputeTargetPos), w1e(0x669F60,FakeComputeTargetPos);
        EntryPatch w2(bit_cast<uintptr_t>(&CTaskComplexWander::ScanForBlockedNodes),FakeScanBlocked), w2e(0x674560,FakeScanBlocked);
        EntryPatch w3(Follow(dllWanderFirst),FakeWanderFirst), w3e(0x6740E0,FakeWanderFirst);
        EntryPatch w4(bit_cast<uintptr_t>(&CTaskComplexWander::CreateSubTask),FakeWanderSub), w4e(0x671CB0,FakeWanderSub);
        EntryPatch i1(bit_cast<uintptr_t>(&InteriorGroup_c::FindClosestInteriorInfo),FakeFindInfo), i1e(0x594A50,FakeFindInfo);
        EntryPatch i2(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),FakeLookAt), i2e(0x618970,FakeLookAt);
        EntryPatch i3(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeGroupAdd), i3e(0x4AB420,FakeGroupAdd);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const size_t size=ObjSize(cls);
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    PoolLast()=lf0;
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6B6Bu; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0; auxCalls=0; freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    At<CVector>(reinterpret_cast<void*>(GETUP_OFS),0)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    std::memset(intel.data(),0,intel.size()); At<int32>(intel.data(),I_STATIC_COUNTER)=next()%2 ? int32(next()%31) : int32(31+next()%40);
                    for(auto& b:handling)b=uint8(next()); handling[H_ANIMGROUP]=uint8(next()%30);
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3; At<void*>(p,P_INTEL)=intel.data();
                        if(n==0||next()%4){ At<void*>(p,E_MATRIX)=pedMats[n].data(); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f));
                            m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; std::memcpy(pedMats[n].data(),&m,sizeof(CMatrix)); }
                        else At<CVector>(p,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        { static const int32 types[]{0,1,4,5,6,7,6}; At<int32>(p,P_PEDTYPE)=types[next()%7]; }
                        p[P_CREATED]=uint8(1+next()%2); At<uint32>(p,P_FLAGS)=next(); At<uint32>(p,P_FLAGS+8)=next(); p[E_FLAGS]=uint8(next());
                        At<CVector>(p,PH_MOVE)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<float>(p,P_ROT_CUR)=uni(-3.f,3.f); At<float>(p,P_ROT_AIM)=uni(-3.f,3.f); At<int32>(p,P_STATE)=int32(next()%60); At<int32>(p,P_STATE+8)=int32(next()%8);
                    }
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%32)<<3); At<void*>(v,V_HANDLING)=handling.data();
                        if(next()%5){ At<void*>(v,E_MATRIX)=vehMats[n].data(); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.2f,3.2f));
                            m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                            // Estres del rumbo del vehiculo de SetPedSlowDraggedOut::ProcessPed (0x64C4FC: fpatan(-forward.x, forward.y)): NaN con distinta carga y signo,
                            // +-inf, +-0, subnormales, +-1e30/1e-30 y FLT_MAX en x e y del frente
                            { static constexpr uint32 fwdBits[]{0x00000000u,0x80000000u,0x00000001u,0x80000001u,0x007FFFFFu,0x00800000u,0x7F7FFFFFu,0xFF7FFFFFu,
                                  0x7F800000u,0xFF800000u,0x7FC00000u,0xFFC00000u,0x7FC00001u,0x7F800001u,0xFF800001u,0x3F800000u,0xBF800000u,0x7149F2CAU,0x0DA24260U,0xF149F2CAU,0x8DA24260U};
                              if(next()%6==0){ m.GetForward().x=std::bit_cast<float>(fwdBits[next()%std::size(fwdBits)]); m.GetForward().y=std::bit_cast<float>(fwdBits[next()%std::size(fwdBits)]); } }
                            std::memcpy(vehMats[n].data(),&m,sizeof(CMatrix)); }
                        else { At<CVector>(v,E_POS)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)}; At<float>(v,E_HEADING)=uni(-3.2f,3.2f); }
                        for(size_t b=0;b<0xA0;++b)v[V_AUTOPILOT+b]=uint8(next());
                        { static const uint32 locks[]{1,5,2,5,0}; At<uint32>(v,V_DOORLOCK)=locks[next()%5]; }
                    }
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[n])b=uint8(next()); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<void*>(fakes[n].data(),4)=nullptr;
                        At<int32>(fakes[n].data(),FK_TYPE)=203+n; fakes[n][FK_ABORT]=uint8(next()%2); fakes[n][FK_PROC]=uint8(next()%2); At<void*>(fakes[n].data(),FK_CLONE)=nullptr; }
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data();
                    { static const int types[]{9,9,10,10,31,0}; eventTypeRet=types[next()%6]; }
                    for(int n=0;n<NINFO;++n){ for(auto& b:infos[n])b=uint8(next()); }
                    infos[0][0]=8; infos[1][0]=9; { static const uint8 other[]{0,3,7,10,11}; infos[2][0]=other[next()%5]; }
                    std::memset(interior.data(),0,interior.size()); std::memset(group.data(),0,group.size());
                    wanderRet=next()%4 ? uintptr_t(Fake(1)) : 0; groupRet=next()%2; carDriveAbortRet=next()%2; carDriveProcRet=next()%2;
                    roomRet=next()%2; outOfColRet=next()%2; findZRet=next()%3!=0; findInfoRet=next()%5!=0;
                    stepMask=next(); jumpMask=next(); lawMask=next();
                    findInfo=uintptr_t(infos[next()%NINFO].data()); findSome=uni(0.f,4.f);

                    auto* o=object.data(); auto* p0=Ped(0);
                    auto setTask=[&](uintptr_t vtbl){ std::memset(o,0xCD,object.size()); for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; };
                    auto pickVeh=[&]()->CVehicle*{ const uint32 m=next()%5; return m==0 ? nullptr : Veh(m==1 ? 1 : 0); };
                    auto pickEnt=[&]()->CEntity*{ switch(next()%4){ case 0: return nullptr; case 1: return Ped(2); default: return Veh(1); } };
                    const int32 prio=int32(next()%4); const CEvent* ev=next()%5 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                    // campos de CEventDamage leidos (arma, m_bAddToEventGroup, m_bHealthZero)
                    At<int32>(event.data(),0x1C)=next()%2 ? 54 : int32(next()%60); event[0xC]=uint8(next()%3!=0); event[0x40]=uint8(next()%3!=0);

                    // configuracion previa por clase (antes de cambiar los hooks)
                    const auto vtArr=reinterpret_cast<uintptr_t*>(fn.vt);
                    uintptr_t target{};
                    auto callThis=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callPed=[&](CPed* pd){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    auto callAbort=[&]{ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,prio,ev); };
                    auto callProcess=[&](CPed* pd){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    json ret=nullptr; const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor";

                    if(cls=="CTaskComplexJump") {
                        setTask(VT_JUMP);
                        { static const int32 fc[]{-1,0,1,1,2}; At<int32>(o,0xC)=fc[next()%5]; } o[0x10]=uint8(next()%2);
                        At<CTask*>(o,8)=Fake(0);
                        { static const int32 sub[]{254,500,240,210,210,203}; At<int32>(fakes[0].data(),FK_TYPE)=sub[next()%6]; }
                        // CTaskSimpleJump: +8 posicion, +0x14 angulo, +0x18 superficie, +0x1C entidad, +0x21 bloqueado, +0x23 animacion lanzada
                        auto* fk=fakes[0].data(); At<CVector>(fk,8)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,35.f)}; At<float>(fk,0x14)=uni(-3.f,3.f);
                        At<CEntity*>(fk,0x1C)=pickEnt(); fk[0x21]=uint8(next()%3==0); fk[0x23]=uint8(next()%4!=0); fk[0xC]=uint8(next()%2);
                        if(next()%3==0){ const float pz=p0->GetPosition().z; At<float>(fk,0x10)=pz+(next()%2 ? 1.4f : uni(1.2f,1.6f)); }
                        At<uint32>(p0,P_FLAGS)^=next()&0x600u;
                        if(nm=="CreateSubTask"&&next()%6==0)At<CTask*>(o,8)=nullptr;
                    }
                    else if(cls=="CTaskComplexWanderFlee"||cls=="CTaskInteriorBeInShop") {
                        const bool shop=cls=="CTaskInteriorBeInShop";
                        setTask(shop ? VT_SHOP : VT_FLEE);
                        { static const int32 ms[]{4,4,6,7,5,1}; At<int32>(o,0xC)=ms[next()%6]; } At<float>(o,0x14)=uni(0.f,1.f);
                        At<CTask*>(o,8)=next()%4 ? Fake(0) : nullptr;
                        if(shop){ At<void*>(o,0x28)=group.data(); { const uint32 m=next()%4; At<void*>(o,0x2C)=m==0 ? nullptr : m==1 ? reinterpret_cast<void*>(findInfo) : infos[next()%NINFO].data(); }
                                  o[0x30]=uint8(next()%5!=0); }
                        if(nm=="ControlSubTask"){ At<CTask*>(o,8)=Fake(0);
                            { static const int32 sub[]{0x2C0,0x2CF,0x384,0x384,0x384,203}; At<int32>(fakes[0].data(),FK_TYPE)=sub[next()%6]; }
                            auto* fk=fakes[0].data(); At<CVector>(fk,0xC)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,5.f)};
                            At<float>(fk,0x18)=next()%2 ? 0.5f : uni(0.f,1.f);
                            if(next()%4==0)At<CVector>(fk,0xC)=CVector{float(int(seed%31))-15.5f,float(int(seed%17))*0.5f,float(int(seed%3))+0.25f}; // mismo punto que el calculado
                        }
                    }
                    else if(cls=="CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse") {
                        setTask(VT_NEAR); At<CTask*>(o,8)=Fake(0); At<CTask*>(o,0xC)=Fake(1);
                        if(isDtor){ if(next()%4==0)At<CTask*>(o,8)=nullptr; if(next()%4==0)At<CTask*>(o,0xC)=nullptr; }
                        auto* g=fakes[0].data(); At<int32>(g,8)=int32(next()%8); At<CVector>(g,0xC)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                        At<float>(g,0x18)=next()%2 ? 0.5f : uni(0.f,1.f);
                        if(next()%4==0)At<CVector>(g,0xC)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        auto* w=fakes[1].data(); At<CVehicle*>(w,8)=Veh(next()%2); { static const int32 d[]{8,9,10,11,18}; At<int32>(w,0xC)=d[next()%5]; }
                    }
                    else if(cls=="CTaskSimpleCarSetPedSlowDraggedOut") {
                        setTask(VT_DRAG); At<CVehicle*>(o,8)=next()%6 ? Veh(0) : nullptr; { static const int32 d[]{8,9,10,11,18,0}; At<int32>(o,0xC)=d[next()%6]; }
                        if(isDtor)Reg(At<CVehicle*>(o,8),o+8);
                        At<CVehicle*>(p0,P_VEHICLE)=next()%6 ? Veh(0) : nullptr;
                        auto* v=vehs[0].data();
                        { const uint32 m=next()%4; At<CPed*>(v,V_DRIVER)=m<2 ? p0 : m==2 ? Ped(1) : nullptr; }
                        v[V_NUMPASS]=uint8(next()%5); v[V_MAXPASS]=uint8(next()%9);
                        for(int k=0;k<8;++k){ const uint32 m=next()%4; At<CPed*>(v,V_PASS+4*k)=m==0 ? p0 : m==1 ? Ped(1) : m==2 ? Ped(2) : nullptr; }
                    }
                    else if(cls=="CTaskSimpleCarSetTempAction") {
                        setTask(VT_TEMP); At<CVehicle*>(o,8)=pickVeh(); At<void*>(o,0xC)=nullptr; At<void*>(o,0x10)=nullptr; o[0x5C]&=~0x20;
                        At<int32>(o,0x60)=next()%3 ? int32(next()%25) : 0; At<uint32>(o,0x64)=next()%5000;
                        if(isDtor)Reg(At<CVehicle*>(o,8),o+8);
                        At<CVehicle*>(p0,P_VEHICLE)=next()%4 ? Veh(next()%2) : nullptr;
                        for(int n=0;n<2;++n)vehs[n][V_AUTOPILOT+0x2B]=next()%2 ? 0 : uint8(1+next()%24);
                    }
                    else if(cls=="CTaskSimpleCarWaitToSlowDown") {
                        setTask(VT_WAIT); At<CVehicle*>(o,8)=pickVeh(); { const uint32 m=next()%8; At<int32>(o,0x10)=m<3 ? int32(m) : m<7 ? int32(next()%2) : int32(3+next()%3); }
                        if(isDtor)Reg(At<CVehicle*>(o,8),o+8);
                        for(int n=0;n<2;++n){ const uint32 m=next()%4; At<CPed*>(vehs[n].data(),V_DRIVER)=m==0 ? nullptr : m==1 ? Ped(1) : p0; }
                    }
                    // argumentos de los constructores
                    CVehicle* const ctorVeh=pickVeh(); const int32 ctorInt=int32(next()%30)-2; const uint32 ctorDur=next()%10000;
                    const CVector ctorPt{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; const int32 ctorMove=int32(next()%8);
                    const uint8 ctorDir=uint8(next()%2 ? next()%8 : next());
                    const int32 subType=[&]{ static const int32 jt[]{500,1302,210,254,240,240,254,203}; return jt[next()%8]; }();
                    // SetPedSlowDraggedOut usa la matriz cruda del peaton (siempre presente en el 0)
                    CPed* const callPedArg=(next()%5||cls=="CTaskSimpleCarSetPedSlowDraggedOut") ? p0 : Ped(1);

                    const auto occ=Occupancy();
                    setState(candidate);
                    target=fn.slot>=0 ? vtArr[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}};
                    CTask* r{}; bool hasTask=false;
                    if(isCtor){
                        std::memset(o,0xCD,object.size());
                        if(cls=="CTaskComplexJump")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,ctorInt%3-1));
                        else if(cls=="CTaskComplexWanderFlee")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,uint8)>(target)(o,ctorMove,ctorDir));
                        else if(cls=="CTaskInteriorBeInShop")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,next()%6 ? group.data() : nullptr));
                        else if(cls=="CTaskSimpleCarGoToPointNearDoorUntilDoorNotInUse")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,const CVector*,int32)>(target)(o,ctorVeh ? ctorVeh : Veh(0),ctorInt,&ctorPt,ctorMove));
                        else if(cls=="CTaskSimpleCarSetPedSlowDraggedOut")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,ctorVeh,ctorInt));
                        else if(cls=="CTaskSimpleCarSetTempAction")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,uint32)>(target)(o,ctorVeh,ctorInt%25,ctorDur));
                        else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,ctorVeh,ctorInt%4));
                    }
                    else if(isDtor)reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone"){ r=callThis(); hasTask=true; }
                    else if(nm=="GetTaskType"||nm=="GetWanderType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                    else if(nm=="ScanForStuff")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,callPedArg);
                    else if(nm=="MakeAbortable")ret=callAbort();
                    else if(nm=="ProcessPed")ret=callProcess(callPedArg);
                    else if(nm=="CreateSubTask"){ r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,subType,callPedArg); hasTask=true; }
                    else { r=callPed(callPedArg); hasTask=true; } // CreateFirstSubTask, CreateNextSubTask, ControlSubTask
                    setState(false);
                    // tareas nuevas del pool
                    auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                    if(hasTask)ret=Name(r);
                    json objectWords=Region(o,size,isCtor||isDtor); if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType()));
                    out["object"]=objectWords;
                    json ped=json::array(); for(auto& p:peds)ped.push_back(EntHash(p)); out["peds"]=ped;
                    json veh=json::array(); for(auto& v:vehs)veh.push_back(EntHash(v)); out["vehs"]=veh;
                    out["ped0"]={Region(peds[0].data()+P_FLAGS,0x14),Region(peds[0].data()+PH_MOVE,12),Region(peds[0].data()+P_STATE,12),F(At<float>(peds[0].data(),P_ROT_CUR)),F(At<float>(peds[0].data(),P_ROT_AIM)),
                                 peds[0][E_FLAGS],Region(peds[0].data()+E_POS,12),Region(pedMats[0].data()+0x30,12)};
                    out["veh0"]={Region(vehs[0].data()+V_AUTOPILOT+0x28,0x1C),Region(vehs[0].data()+V_DRIVER,0x2C),At(vehs[0].data(),V_DOORLOCK),Region(vehs[1].data()+V_AUTOPILOT+0x28,0x1C)};
                    json fk=json::array(); for(auto& b:fakes)fk.push_back(Region(b.data(),0x40,true)); out["fakes"]=fk;
                    out["refs"]=AllRefs();
                    // limpieza: tareas nuevas que no cuelgan de otra tarea nueva (los destructores borran sus subtareas)
                    const size_t traceBefore=trace.size();
                    std::vector<CTask*> roots;
                    for(auto* t:fresh){ bool owned=false; for(auto* u:fresh)if(u!=t){ const auto* w=reinterpret_cast<const uint32*>(u); for(size_t k=1;k<TASK_SLOT/4;++k)if(w[k]==uint32(uintptr_t(t)))owned=true; } if(!owned)roots.push_back(t); }
                    for(auto* t:roots)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls;
                    out["globals"]=Region(reinterpret_cast<void*>(GETUP_OFS),12);
                    out["clean"]=Occupancy()==occ;
                    freshIds.clear(); restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreGlobals(); PoolLast()=lf0;
    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<extra.size();++k){extra[k]->State(extraEnabled[k]);extra[k]->LockState(extraLocked[k]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
