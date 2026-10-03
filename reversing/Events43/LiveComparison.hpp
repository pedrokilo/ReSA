#pragma once
// Banco dentro del motor: eventos, repartidores y datos de peatón/vehículo (lista de 43, 45 funciones de 9 clases).
// CEventGunShot (constructor, AffectsPed, CloneEditable, IsCriminalEvent, TakesPriorityOver), CEventVehicleDamage
// (constructor, AffectsPed, GetSourceEntity, IsCriminalEvent, ReportCriminalEvent), CTaskAllocatorAttack y
// CTaskAllocatorKillThreatsDriveby (constructor, destructor, GetType, AllocateTasks, ProcessGroup),
// CPedScriptedTaskRecord (GetRecordAssociatedWithEvent, los dos GetStatus, GetVacantSlot, Process), CPlayerPedData
// (constructor, destructor, AllocateData, DeAllocateData, SetInitialState), CUpsideDownCarCheck (AddCarToCheck,
// RemoveCarFromCheck, HasCarBeenUpsideDownForAWhile, IsCarUpsideDown, UpdateTimers), CVehicleAnimGroup
// (InitAnimGroup, CopyAnimGroup, GetGroup, ComputeCriticalBlendTime, ComputeAnimDoorOffsets) y CVehicleAnimGroupData
// (GetAnimDoorOffset, GetGroupForAnim, GetInOutTimings, UsesKartDrivingAnims, UsesTruckDrivingAnims).
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: peatones, vehiculos (vtable falsa
// con CanPedStepOutCar trazado), inteligencias, eventos y tareas falsos (vtable propia), un CPedGroup y una
// CPedGroupIntelligence falsos, un pool de vehiculos falso (cabecera, estados con byte de guarda en [-1] y 5 huecos de
// 0xA18), un CWanted falso y animaciones falsas (asociacion, jerarquia, secuencia y fotogramas).
// Se simulan y trazan: FindPlayerPed, FindPlayerWanted, CWanted::IsInPursuit/CanCopJoinPursuit/RegisterCrime,
// CPedGroups::AreInSameGroup/GetPedsGroup, CWorld::GetIsLineOfSightClear, CPedGroupIntelligence::FlushTasks/
// SetEventResponseTask (la tarea de la pila se serializa en el momento)/GetTaskScriptCommand,
// CTaskManager::FindTaskByType, CTaskAllocatorKillThreatsBasic::ComputeClosestPeds,
// CTaskAllocatorKillOnFoot::AllocateTasks, CInterestingEvents::Add, CEventGroup::HasEvent,
// CScriptedBrainTaskStore::GetTask, CStats::GetFatAndMuscleModifier, CAnimManager::GetAnimAssociation/
// UncompressAnimation, operator new/delete (solo durante la llamada, registro sin reservas) y el RNG.
// Estado global que se toca y se restaura en cada ejecucion y al final: el pool de eventos (cabecera, estados y
// almacenamiento), el pool de tareas (m_LastFreeSlot, estados; huecos libres con 0xCD), CPedScriptedTaskRecord::
// ms_scriptedTasks, CPedGroups::ms_groups, CVehicleAnimGroupData::m_vehicleAnimGroups, el puntero del pool de
// vehiculos, CEventGunShot::ms_fGunShotSenseRangeForRiot2, CTimer::m_snTimeInMilliseconds y CTimer::ms_fTimeStep.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventGunShot.h"
#include "Events/EventVehicleDamage.h"
#include "Events/EventGroup.h"
#include "Tasks/Allocators/TaskAllocatorAttack.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsDriveby.h"
#include "Tasks/Allocators/TaskAllocatorKillThreatsBasic.h"
#include "Tasks/Allocators/TaskAllocatorKillOnFoot.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/ScriptedBrainTaskStore.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskComplexSequence.h"
#include "PlayerPedData.h"
#include "UpsideDownCarCheck.h"
#include "VehicleAnimGroupData.h"
#include "Animation/AnimManager.h"
#include "PedGroups.h"
#include "PedGroupIntelligence.h"
#include "InterestingEvents.h"
#include "Wanted.h"
#include "Stats.h"
#include "General.h"
#include "Pools/Pools.h"
#include "Pools/EventPool.h"
#include "Pools/TaskPool.h"
#include "Entity/Vehicle/Automobile.h"
#include "../ProbeRuntime.hpp"

namespace Events43Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_TYPE=0x36;                                             // CEntity
constexpr size_t P_FLAGS1=0x46C, P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_VEHICLE=0x58C, P_TYPE=0x598; // CPed
constexpr size_t V_DRIVER=0x460, V_PASSENGERS=0x464, V_TYPE=0x590, V_WHEELS_BIKE=0x804, V_WHEELS_AUTO=0x960;   // CVehicle
constexpr size_t I_TASKMGR=0x04, I_EVGROUP=0x68, I_FOV=0xBC;                                         // CPedIntelligence
constexpr size_t G_MEMBERS=0x0C, G_INTEL=0x30, GROUP_SIZE=0x2D4;                                     // CPedGroup
constexpr size_t FK_TYPE=0x38, FK_PRIO=0x3C;                                                          // tarea/evento falsos
constexpr uintptr_t VT_GUN=0x85ABE0, VT_VD=0x85B588, VT_ATK=0x870E60, VT_DRV=0x870EA8;
constexpr uintptr_t RIOT=0x8A625C, TIMER=0xB7CB84, TIMESTEP=0xB7CB5C, VEHPOOL=0xB74494, SCRIPTED=0xC0B1E8, MSGROUPS=0xC09920,
                    ANIMGROUPS=0xC1CDC0;
constexpr size_t SCRIPTED_SIZE=0x14*128, MSGROUPS_SIZE=GROUP_SIZE*8, ANIMGROUP=0x94, ANIMGROUPS_SIZE=ANIMGROUP*30;
constexpr size_t EVENT_SLOT=0x44, TASK_SLOT=0x80, POOL_SLOT=0xA18;
constexpr int NPEDS=8, NVEH=3, FAKES=6, NPOOL=4;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> evs{};
alignas(16) inline std::array<uint8,0x100> object{}, src{};
alignas(16) inline std::array<uint8,0x40> refEv{}, timA{}, timB{}, timC{};
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<uint8,sizeof(CPedGroupIntelligence)> gintel{};
alignas(16) inline std::array<uint8,sizeof(CWanted)> wanted{}, arenaW{};
alignas(16) inline std::array<uint8,0x80> arenaC{};
// pool de vehiculos falso: el hueco 0 queda delante del almacenamiento (lo usa el EXE si el estado [-1] coincide)
alignas(16) inline std::array<uint8,POOL_SLOT*(NPOOL+1)> poolStorage{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPOOL+1> poolMats{};
alignas(16) inline std::array<uint8,0x20> poolStates{}, poolHdr{};
alignas(16) inline std::array<uint8,0x20> assoc{}, hier{}, seq{};
alignas(16) inline std::array<uint8,0x140> frames{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* PoolVeh(int n) { return poolStorage.data()+POOL_SLOT*size_t(n); }  // n=0: hueco de guarda; 1..NPOOL: indices 0..NPOOL-1

// pools reales (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
inline uint8* TPHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* TPStorage() { return *reinterpret_cast<uint8**>(TPHdr()); }
inline uint8* TPStates() { return *reinterpret_cast<uint8**>(TPHdr()+4); }
inline int32 TPCap() { return *reinterpret_cast<int32*>(TPHdr()+8); }
inline int32& TPLast() { return At<int32>(TPHdr(),0xC); }
inline bool IsPoolTask(uintptr_t v) {
    const auto s=uintptr_t(TPStorage()); if(v<s||v>=s+size_t(TPCap())*TASK_SLOT||(v-s)%TASK_SLOT)return false;
    return !(TPStates()[(v-s)/TASK_SLOT]&0x80);
}
inline uint8* EvHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* EvStorage() { return *reinterpret_cast<uint8**>(EvHdr()); }
inline uint8* EvStates() { return *reinterpret_cast<uint8**>(EvHdr()+4); }
inline size_t EvCap() { return *reinterpret_cast<uint32*>(EvHdr()+8); }

inline std::array<void*,16> taskVtbl{};
inline std::array<void*,20> eventVtbl{};
inline std::array<void*,80> vehVtbl{}, pedVtbl{};
inline uintptr_t stackLo{}, stackHi{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v>=stackLo&&v<stackHi)return "stack";
    if(v==uintptr_t(taskVtbl.data()))return "task_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(pedVtbl.data()))return "ped_vt";
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(evs[n].data(),evs[n].size(),"ev"))return *s+"#"+std::to_string(n);
    for(int n=0;n<=NPOOL;++n)if(auto s=in(poolMats[n].data(),poolMats[n].size(),"poolmat"))return *s+"#"+std::to_string(n);
#define EV43_IN(buf,nm) if(auto s=in(buf.data(),buf.size(),nm))return *s;
    EV43_IN(object,"object") EV43_IN(src,"src") EV43_IN(refEv,"refEv") EV43_IN(timA,"timA") EV43_IN(timB,"timB") EV43_IN(timC,"timC")
    EV43_IN(group,"group") EV43_IN(gintel,"gintel") EV43_IN(wanted,"wanted") EV43_IN(arenaW,"arenaW") EV43_IN(arenaC,"arenaC")
    EV43_IN(poolStorage,"poolveh") EV43_IN(poolStates,"poolstates") EV43_IN(poolHdr,"poolhdr") EV43_IN(assoc,"assoc") EV43_IN(hier,"hier")
    EV43_IN(seq,"seq") EV43_IN(frames,"frames")
#undef EV43_IN
    if(v>=MSGROUPS&&v<MSGROUPS+MSGROUPS_SIZE)return "msgroup#"+std::to_string((v-MSGROUPS)/GROUP_SIZE)+"+"+std::to_string((v-MSGROUPS)%GROUP_SIZE);
    if(v>=SCRIPTED&&v<SCRIPTED+SCRIPTED_SIZE)return "rec#"+std::to_string((v-SCRIPTED)/0x14)+"+"+std::to_string((v-SCRIPTED)%0x14);
    if(v>=ANIMGROUPS&&v<ANIMGROUPS+ANIMGROUPS_SIZE)return "vag#"+std::to_string((v-ANIMGROUPS)/ANIMGROUP)+"+"+std::to_string((v-ANIMGROUPS)%ANIMGROUP);
    { const auto es=uintptr_t(EvStorage()); if(v>=es&&v<es+EvCap()*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(const void* e) { json l=json::array(); for(auto* r=static_cast<const CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

// tarea serializada sin vptr (se compara el tipo); bytes sin escribir de las tareas de la pila enmascarados:
// CTaskSimpleGangDriveBy 0x15 y 0x1D..0x1F, CTaskComplexSequence 0x39..0x3B
inline json SerTask(const void* p,int depth,size_t bytes);
inline json Word(int type,size_t off,uint32 v,int depth) {
    if(type==0x3FE&&off==0x14)return v&0xFFFF00FFu;
    if(type==0x3FE&&off==0x1C)return v&0xFFu;
    if(type==TASK_COMPLEX_SEQUENCE&&off==0x38)return v&0xFFu;
    if(depth>=0&&depth<3&&IsPoolTask(v))return SerTask(reinterpret_cast<const void*>(uintptr_t(v)),depth+1,TASK_SLOT);
    return Name(v);
}
inline json SerTask(const void* p,int depth,size_t bytes) {
    const int type=const_cast<CTask*>(static_cast<const CTask*>(p))->GetTaskType(); const auto* w=static_cast<const uint32*>(p);
    json words=json::array(); for(size_t k=1;k<bytes/4;++k)words.push_back(Word(type,k*4,w[k],depth));
    return {{"type",type},{"words",words}};
}
inline json SerStack(const CTask* t) {
    if(!t)return nullptr;
    const int type=const_cast<CTask*>(t)->GetTaskType();
    const size_t n=type==0x3FE ? 0x44 : type==0x3F7 ? 0x14 : type==TASK_COMPLEX_SEQUENCE ? 0x40 : 0x8;
    return SerTask(t,0,n);
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{}, stepMask{}, stepCalls{}, hasEventMask{}, closestSalt{};
inline bool sameGroupRet{}, inPursuitRet{}, canJoinRet{}, losRet{};
inline uintptr_t playerRet{}, wantedRet{}, findTaskRet{}, brainRet{}, pedsGroupRet{}, scriptCmdRet{};
inline std::array<float,2> fatRet{};

// tareas falsas: lectores puros (no se trazan)
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
// evento de referencia de TakesPriorityOver: tipo y prioridad trazados
inline int __fastcall EType(void* e,void*) { trace.push_back({"ref_event_type",Name(e)}); return At<int32>(e,FK_TYPE); }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"ref_event_priority",Name(e)}); return At<int32>(e,FK_PRIO); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
// vehiculos: CanPedStepOutCar (hueco 57) trazado; el resto son trampas
inline bool __fastcall FStepOut(void* v,void*,uint32 ignore) { trace.push_back({"can_ped_step_out_car",Name(v),ignore&0xFF}); return (stepMask>>(stepCalls++%32))&1; }
template<int N> void* __fastcall VTrap(void* v,void*) { trace.push_back({"veh_unexpected_slot",N,Name(v)}); return nullptr; }
template<int N> void* __fastcall PTrap(void* p,void*) { trace.push_back({"ped_unexpected_slot",N,Name(p)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) {
    ((taskVtbl[N%16]=reinterpret_cast<void*>(&FTrap<N%16>)),...); ((eventVtbl[N%20]=reinterpret_cast<void*>(&ETrap<N%20>)),...);
    ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...);
}

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// jugador, busqueda y crimenes
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wantedRet); }
inline bool __fastcall FakeInPursuit(void* w,void*,void* cop) { trace.push_back({"is_in_pursuit",Name(w),Name(cop)}); return inPursuitRet; }
inline bool __fastcall FakeCanJoin(void* w,void*,void* cop) { trace.push_back({"can_cop_join_pursuit",Name(w),Name(cop)}); return canJoinRet; }
inline void __fastcall FakeRegisterCrime(void* w,void*,int32 type,const CVector* pos,uint32 id,uint32 dontCare) {
    trace.push_back({"register_crime",Name(w),type,F(pos->x),F(pos->y),F(pos->z),Name(id),dontCare&0xFF});
}
inline bool __cdecl FakeSameGroup(const CPed* a,const CPed* b) { trace.push_back({"are_in_same_group",Name(a),Name(b)}); return sameGroupRet; }
inline bool __cdecl FakeLos(const CVector& a,const CVector& b,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7) {
    trace.push_back({"line_of_sight",F(a.x),F(a.y),F(a.z),F(b.x),F(b.y),F(b.z),b1,b2,b3,b4,b5,b6,b7}); return losRet;
}
// repartidores
inline void __cdecl FakeFlushDll(void* pairs,CPed* ped) { trace.push_back({"flush_tasks",Name(pairs),Name(ped)}); }
inline void __fastcall FakeFlushExe(void*,void*,void* pairs,CPed* ped) { trace.push_back({"flush_tasks",Name(pairs),Name(ped)}); }   // thiscall, ret 8
inline CTask* __fastcall FakeFindTask(void* tm,void*,int32 idx,int32 type) { trace.push_back({"find_task_by_type",Name(tm),idx,type}); return reinterpret_cast<CTask*>(findTaskRet); }
inline void __fastcall FakeEventResponse(void* in,void*,CPed* ped,uint32 hasMain,const CTask* main,uint32 hasSec,const CTask* sec,int32 slot) {
    trace.push_back({"group_event_response",Name(in),Name(ped),uint8(hasMain),SerStack(main),uint8(hasSec),sec ? json(const_cast<CTask*>(sec)->GetTaskType()) : json(nullptr),slot});
}
inline void __fastcall FakeInteresting(void* self,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event",Name(self),type,Name(e)}); }
inline void __cdecl FakeClosest(CPedGroup& g1,CPedGroup& g2,CPed** out) {
    trace.push_back({"compute_closest_peds",Name(&g1),Name(&g2)});
    for(uint32 k=0;k<8;++k){ const uint32 m=(closestSalt>>(k*3))&7; out[k]=m<6 ? At<CPed*>(&g2,G_MEMBERS+4*m) : m==6 ? Ped(7) : nullptr; }
}
inline void __fastcall FakeKofAllocate(void* self,void*,void* intel) {
    auto* t=At<CPed*>(self,4);
    trace.push_back({"kill_on_foot_allocate",Name(self),Name(intel),Name(t),At<int32>(self,8),t ? RefList(t) : json(nullptr)});
}
// CPedScriptedTaskRecord::Process
inline bool __fastcall FakeHasEvent(void* grp,void*,void* ev) {
    trace.push_back({"has_event",Name(grp),Name(ev)}); return (hasEventMask>>((uintptr_t(grp)/8+uintptr_t(ev)/8)%32))&1;
}
inline CTask* __cdecl FakeBrainTask(CPed* p) { trace.push_back({"brain_get_task",Name(p)}); return reinterpret_cast<CTask*>(brainRet); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline CTask* __fastcall FakeScriptCmd(void* in,void*,CPed* p) { trace.push_back({"get_task_script_command",Name(in),Name(p)}); return reinterpret_cast<CTask*>(scriptCmdRet); }
// CPlayerPedData
inline float __cdecl FakeFat(int32 mod) { trace.push_back({"fat_and_muscle_modifier",mod}); return fatRet[mod&1]; }
struct Rec { uint32 tag, a; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void* __cdecl FakeNew(size_t n) { if(nrec<recs.size())recs[nrec++]={1,uint32(n)}; return n==sizeof(CWanted) ? (void*)arenaW.data() : n==0x78 ? (void*)arenaC.data() : nullptr; }
inline void __cdecl FakeDelete(void* p) { if(nrec<recs.size())recs[nrec++]={2,uint32(uintptr_t(p))}; }
inline json Recs() { json r=json::array(); for(size_t k=0;k<nrec;++k)r.push_back({recs[k].tag,recs[k].tag==2 ? Name(recs[k].a) : json(recs[k].a)}); return r; }
// animaciones
inline void* __cdecl FakeAnimAssoc(int32 grp,int32 anim) { trace.push_back({"get_anim_association",grp,anim}); return assoc.data(); }
inline void __cdecl FakeUncompress(void* h) { trace.push_back({"uncompress_animation",Name(h)}); }

inline std::vector<bool> TPOccupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline std::vector<bool> EvOccupancy() { std::vector<bool> r(EvCap()); for(size_t k=0;k<r.size();++k)r[k]=!(EvStates()[k]&0x80); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct ClosestAccess : CTaskAllocatorKillThreatsBasic { using CTaskAllocatorKillThreatsBasic::ComputeClosestPeds; };

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CEventGunShot","AffectsPed",0,0x85ABE0,5,1024},
    {"CEventGunShot","CloneEditable",0,0x85ABE0,16,256},
    {"CEventGunShot","Constructor",0x4AC610,0,-1,256},
    {"CEventGunShot","IsCriminalEvent",0,0x85ABE0,7,128},
    {"CEventGunShot","TakesPriorityOver",0,0x85ABE0,11,512},
    {"CEventVehicleDamage","AffectsPed",0,0x85B588,5,512},
    {"CEventVehicleDamage","Constructor",0x4B18D0,0,-1,256},
    {"CEventVehicleDamage","GetSourceEntity",0,0x85B588,10,128},
    {"CEventVehicleDamage","IsCriminalEvent",0,0x85B588,7,256},
    {"CEventVehicleDamage","ReportCriminalEvent",0,0x85B588,8,512},
    {"CTaskAllocatorAttack","AllocateTasks",0,0x870E60,1,256},
    {"CTaskAllocatorAttack","Constructor",0x69C240,0,-1,256},
    {"CTaskAllocatorAttack","Destructor",0x69C2D0,0,-1,256},
    {"CTaskAllocatorAttack","GetType",0,0x870E60,5,64},
    {"CTaskAllocatorAttack","ProcessGroup",0,0x870E60,2,512},
    {"CTaskAllocatorKillThreatsDriveby","AllocateTasks",0,0x870EA8,1,1024},
    {"CTaskAllocatorKillThreatsDriveby","Constructor",0x69CAF0,0,-1,256},
    {"CTaskAllocatorKillThreatsDriveby","Destructor",0x69CB60,0,-1,256},
    {"CTaskAllocatorKillThreatsDriveby","GetType",0,0x870EA8,5,64},
    {"CTaskAllocatorKillThreatsDriveby","ProcessGroup",0,0x870EA8,2,512},
    {"CPedScriptedTaskRecord","GetRecordAssociatedWithEvent",0x608580,0,-1,256},
    {"CPedScriptedTaskRecord","GetStatus-",0x608750,0,-1,256},
    {"CPedScriptedTaskRecord","GetStatus-opcode",0x608710,0,-1,256},
    {"CPedScriptedTaskRecord","GetVacantSlot",0x608540,0,-1,128},
    {"CPedScriptedTaskRecord","Process",0x6085E0,0,-1,512},
    {"CPlayerPedData","AllocateData",0x571880,0,-1,256},
    {"CPlayerPedData","Constructor",0x56F810,0,-1,128},
    {"CPlayerPedData","DeAllocateData",0x56F890,0,-1,256},
    {"CPlayerPedData","Destructor",0x571840,0,-1,256},
    {"CPlayerPedData","SetInitialState",0x56F210,0,-1,128},
    {"CUpsideDownCarCheck","AddCarToCheck",0x4638D0,0,-1,256},
    {"CUpsideDownCarCheck","HasCarBeenUpsideDownForAWhile",0x463940,0,-1,256},
    {"CUpsideDownCarCheck","IsCarUpsideDown-vehicle",0x463830,0,-1,512},
    {"CUpsideDownCarCheck","RemoveCarFromCheck",0x463910,0,-1,256},
    {"CUpsideDownCarCheck","UpdateTimers",0x4655E0,0,-1,512},
    {"CVehicleAnimGroup","ComputeAnimDoorOffsets",0x6E3D10,0,-1,512},
    {"CVehicleAnimGroup","ComputeCriticalBlendTime",0x6E3C80,0,-1,256},
    {"CVehicleAnimGroup","CopyAnimGroup",0x5B9E40,0,-1,128},
    {"CVehicleAnimGroup","GetGroup",0x6E3B00,0,-1,512},
    {"CVehicleAnimGroup","InitAnimGroup",0x5B9EB0,0,-1,128},
    {"CVehicleAnimGroupData","GetAnimDoorOffset",0x645600,0,-1,256},
    {"CVehicleAnimGroupData","GetGroupForAnim",0x639FC0,0,-1,256},
    {"CVehicleAnimGroupData","GetInOutTimings",0x645630,0,-1,128},
    {"CVehicleAnimGroupData","UsesKartDrivingAnims",0x6D09E0,0,-1,64},
    {"CVehicleAnimGroupData","UsesTruckDrivingAnims",0x639FE0,0,-1,64},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"events43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"events43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("events43");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"events43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,80>{});
    taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    vehVtbl[57]=reinterpret_cast<void*>(&FStepOut);
    // CTaskAllocatorKillOnFoot::AllocateTasks de la DLL: entrada 1 de su vtable (la DLL la llama directamente sobre el temporal)
    uintptr_t dllKofAllocate{}; { CTaskAllocatorKillOnFoot probe{nullptr,0}; dllKofAllocate=(*reinterpret_cast<uintptr_t**>(&probe))[1]; }
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{RIOT,4},{TIMER,4},{TIMESTEP,4},{VEHPOOL,4},{SCRIPTED,SCRIPTED_SIZE},{MSGROUPS,MSGROUPS_SIZE},{ANIMGROUPS,ANIMGROUPS_SIZE}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const std::vector<uint8> savedEvHdr(EvHdr(),EvHdr()+0x14), savedEvStates(EvStates(),EvStates()+EvCap()), savedEvStorage(EvStorage(),EvStorage()+EvCap()*EVENT_SLOT);
    auto restoreEvents=[&]{ std::memcpy(EvStates(),savedEvStates.data(),savedEvStates.size()); std::memcpy(EvStorage(),savedEvStorage.data(),savedEvStorage.size());
                            std::memcpy(EvHdr(),savedEvHdr.data(),savedEvHdr.size()); };
    const auto evOccStart=EvOccupancy(), tpOccStart=TPOccupancy();
    const int32 lf0=TPLast(); const std::vector<uint8> tpStatesStart(TPStates(),TPStates()+TPCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FakeFindPlayerWanted), p2e(0x56E230,FakeFindPlayerWanted);
        EntryPatch w1(bit_cast<uintptr_t>(&CWanted::IsInPursuit),FakeInPursuit), w1e(0x562330,FakeInPursuit);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<bool(CWanted::*)(CCopPed*)>(&CWanted::CanCopJoinPursuit)),FakeCanJoin), w2e(0x562FB0,FakeCanJoin);
        EntryPatch w3(bit_cast<uintptr_t>(&CWanted::RegisterCrime),FakeRegisterCrime), w3e(0x562410,FakeRegisterCrime);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup),FakeSameGroup), g1e(0x5F7F40,FakeSameGroup);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), g2e(0x5F7E80,FakePedsGroup);
        EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), l1e(0x56A490,FakeLos);
        EntryPatch a1(bit_cast<uintptr_t>(&CPedGroupIntelligence::FlushTasks),FakeFlushDll), a1e(0x5F79C0,FakeFlushExe);
        EntryPatch a2(bit_cast<uintptr_t>(&CTaskManager::FindTaskByType),FakeFindTask), a2e(0x6817D0,FakeFindTask);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<void(CPedGroupIntelligence::*)(CPed*,bool,const CTask&,bool,const CTask&,eSecondaryTask)>(&CPedGroupIntelligence::SetEventResponseTask)),FakeEventResponse), a3e(0x5F8510,FakeEventResponse);
        EntryPatch a4(bit_cast<uintptr_t>(&CInterestingEvents::Add),FakeInteresting), a4e(0x602590,FakeInteresting);
        EntryPatch a5(bit_cast<uintptr_t>(&ClosestAccess::ComputeClosestPeds),FakeClosest), a5e(0x69C850,FakeClosest);
        EntryPatch a6(dllKofAllocate,FakeKofAllocate), a6e(0x69BCA0,FakeKofAllocate);
        EntryPatch s1(bit_cast<uintptr_t>(&CEventGroup::HasEvent),FakeHasEvent), s1e(0x4AB6A0,FakeHasEvent);
        EntryPatch s2(bit_cast<uintptr_t>(&CScriptedBrainTaskStore::GetTask),FakeBrainTask), s2e(0x6357C0,FakeBrainTask);
        EntryPatch s3(bit_cast<uintptr_t>(&CPedGroupIntelligence::GetTaskScriptCommand),FakeScriptCmd), s3e(0x5F8690,FakeScriptCmd);
        EntryPatch d1(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),FakeFat), d1e(0x559AF0,FakeFat);
        EntryPatch n1(bit_cast<uintptr_t>(static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation)),FakeAnimAssoc), n1e(0x4D3A60,FakeAnimAssoc);
        EntryPatch n2(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation),FakeUncompress), n2e(0x4D41C0,FakeUncompress);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isCtor=nm=="Constructor";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                // float aleatorio sin NaN de senalizacion (el x87 los silenciaria al copiarlos)
                auto rndF=[&]{ uint32 v=next(); if((v&0x7F800000u)==0x7F800000u)v|=0x00400000u; return v; };
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4343u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0; stepCalls=0; nrec=0;
                    CleanRefs(); restoreGlobals(); restoreEvents();
                    for(size_t k=0;k<EvCap();++k)if(EvStates()[k]&0x80)std::memset(EvStorage()+k*EVENT_SLOT,0xCD,EVENT_SLOT);
                    // pool de tareas: el New del EXE busca desde m_LastFreeSlot+1; huecos libres con 0xCD
                    TPLast()=lf0; if(TPOccupancy()==tpOccStart)std::memcpy(TPStates(),tpStatesStart.data(),tpStatesStart.size());
                    for(int32 k=0;k<TPCap();++k)if(TPStates()[k]&0x80)std::memset(TPStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    const auto evOcc=EvOccupancy(), tpOcc=TPOccupancy();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    const uintptr_t P0=uintptr_t(Ped(0)), P1=uintptr_t(Ped(1)), P2=uintptr_t(Ped(2)), V0=uintptr_t(Veh(0)), V1=uintptr_t(Veh(1)), V2=uintptr_t(Veh(2));
                    // peatones
                    const CVector center=vec(30.f);
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3;
                        const CVector pos=center+vec(40.f);
                        if(next()%4){ auto* m=pedMats[n].data(); At<void*>(p,E_MATRIX)=m; At<CVector>(m,0x30)=pos; At<CVector>(m,0x10)=vec(1.f); if(next()%16==0)At<CVector>(m,0x10)=CVector{0.f,0.f,0.f}; }
                        else At<CVector>(p,E_POS)=pos;
                        At<uint32>(p,P_TYPE)=uint32(pick({6,6,0,1,4,7,20,8,6}));
                        At<uint32>(p,P_STATE)=uint32(pick({1,1,1,0x36,0x37,0,7,0x32}));
                        At<uint8>(p,P_CREATED)=uint8(pick({1,2,2,0}));
                        At<uint32>(p,P_FLAGS1)=next();
                        At<uintptr_t>(p,P_VEHICLE)=pick({0,0,V0,V1});
                        auto* in=intels[n].data(); std::memset(in,0,intels[n].size()); At<void*>(p,P_INTEL)=in;
                        At<float>(in,I_FOV)=uni(5.f,60.f); At<float>(in,I_FOV+4)=uni(5.f,60.f);
                    }
                    // vehiculos
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=2; At<void*>(v,E_MATRIX)=vehMats[n].data(); At<CVector>(vehMats[n].data(),0x30)=center+vec(40.f);
                        At<uint32>(v,V_TYPE)=uint32(pick({0,0,9,3,5}));
                        At<uintptr_t>(v,V_DRIVER)=next()%3 ? pick({P0,P1,P2,uintptr_t(Ped(3))}) : 0;
                        for(int k=0;k<8;++k)At<uintptr_t>(v,V_PASSENGERS+4*k)=next()%4==0 ? pick({P0,P1,P2}) : 0;
                    }
                    // tareas falsas y eventos falsos
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[n])b=uint8(next()); At<void*>(fakes[n].data(),0)=taskVtbl.data(); At<void*>(fakes[n].data(),4)=nullptr; At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; }
                    for(auto& e:evs){ for(auto& b:e)b=uint8(next()); At<void*>(e.data(),0)=eventVtbl.data(); }
                    std::memset(refEv.data(),0,refEv.size()); std::memset(group.data(),0,group.size()); std::memset(gintel.data(),0,gintel.size());
                    std::memset(wanted.data(),0,wanted.size());
                    // dependencias simuladas
                    sameGroupRet=next()%5==0; inPursuitRet=next()%2; canJoinRet=next()%2; losRet=next()%2;
                    playerRet=pick({P1,P2,0,P0}); wantedRet=uintptr_t(wanted.data()); At<uint32>(wanted.data(),0x2C)=uint32(pick({0,0,1,3,6,0xFFFFFFFF,0x80000000}));
                    stepMask=next(); hasEventMask=next(); closestSalt=next(); fatRet={uni(0.5f,2.f),uni(0.5f,2.f)};
                    At<float>(reinterpret_cast<void*>(RIOT),0)=std::bit_cast<float>(uint32(pick({F(0.f),F(0.f),F(-1.f),F(25.f),F(60.f),0x7FC00000u,F(-0.f)})));
                    void* const o=object.data();
                    json out=json::object(); json ret=nullptr; uintptr_t rawRet=0; bool retIsPtr=false;
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    auto callTarget=[&](auto&& body){
                        for(auto* h:hooks)h->State(candidate);
                        volatile uint8 marker{}; stackHi=uintptr_t(&marker); stackLo=stackHi-0x20000;
                        body(fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address);
                        for(auto* h:hooks)h->State(false);
                    };
                    bool switches{};
                    { for(auto* h:hooks)h->State(candidate);
                      const uintptr_t t=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                      switches=fn.slot>=0 ? ((t>=0x400000&&t<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                      for(auto* h:hooks)h->State(false); }
                    (void)target;

                    // ---------------------------------------------------------------- eventos
                    if(cls=="CEventGunShot"||cls=="CEventVehicleDamage") {
                        const bool gun=cls=="CEventGunShot";
                        if(isCtor){
                            std::memset(o,0xCD,0x40);
                            if(gun){ void* ent=reinterpret_cast<void*>(pick({P1,V0,0,P0,uintptr_t(Ped(3))})); const CVector a=center+vec(30.f), b=center+vec(30.f); const uint32 ns=next()%2;
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,CVector,CVector,uint32)>(t)(o,ent,a,b,ns)); }); }
                            else { void* veh=reinterpret_cast<void*>(pick({V0,V1,0})); void* att=reinterpret_cast<void*>(pick({P1,V1,0,V0,P0})); const int32 w=int32(next()%3 ? next()%47 : next());
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,void*,int32)>(t)(o,veh,att,w)); }); }
                            retIsPtr=true;
                            json words=Region(o,0x40,true); const uintptr_t vp=At<uintptr_t>(o,0);
                            if(gun)words.push_back(int(reinterpret_cast<CEvent*>(o)->GetEventType()));
                            else words.push_back(vp==VT_VD||vp<0x400000||vp>=0x1000000 ? json("abstract") : json(vp));
                            out["object"]=words;
                        } else {
                            for(size_t b=0;b<0x40;++b)object[b]=uint8(next());
                            At<uintptr_t>(o,0)=gun ? VT_GUN : VT_VD; At<int32>(o,4)=int32(next()%5)-2;
                            CPed* const p=Ped(0);
                            if(gun){
                                At<uintptr_t>(o,0x14)=pick({P1,P1,V0,0,P0,P2,uintptr_t(Ped(3))});
                                const CVector ppos=p->GetPosition();
                                At<CVector>(o,0x18)=ppos+vec(20.f); At<CVector>(o,0x24)=ppos+vec(20.f); At<uint8>(o,0x30)=uint8(next()%2);
                            } else {
                                At<uintptr_t>(o,0x14)=pick({V0,V1,0,V0}); At<uintptr_t>(o,0x18)=pick({P1,V1,V0,0,P2}); At<int32>(o,0x1C)=int32(next()%47);
                                if(next()%2)At<uintptr_t>(p,P_VEHICLE)=At<uintptr_t>(o,0x14);
                            }
                            if(nm=="AffectsPed"){
                                if(gun){
                                    // el EXE lee la matriz del peaton en la rama de "sin sonido"; tirador cerca del peaton
                                    if(!At<void*>(p,E_MATRIX)){ auto* m=pedMats[0].data(); At<void*>(p,E_MATRIX)=m; At<CVector>(m,0x30)=At<CVector>(p,E_POS); At<CVector>(m,0x10)=vec(1.f); }
                                    if(auto* fb=At<CEntity*>(o,0x14); fb&&fb!=p&&next()%2){ const CVector d=vec(1.f)*uni(0.f,70.f);
                                        if(fb->m_matrix)At<CVector>(fb->m_matrix,0x30)=p->GetPosition()+d; else At<CVector>(fb,E_POS)=p->GetPosition()+d; }
                                }
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            } else if(nm=="IsCriminalEvent"){
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(t)(o); });
                            } else if(nm=="TakesPriorityOver"){
                                auto* r=refEv.data(); At<void*>(r,0)=eventVtbl.data(); At<int32>(r,FK_TYPE)=next()%4 ? int32(EVENT_SHOT_FIRED) : int32(next()%80);
                                At<int32>(r,FK_PRIO)=int32(next()%3==0 ? 35 : next()%70); At<uintptr_t>(r,0x14)=pick({P1,P2,V0,0,P0});
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(t)(o,r); });
                            } else if(nm=="CloneEditable"){
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true;
                            } else if(nm=="GetSourceEntity"){
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=true;
                            } else { // ReportCriminalEvent
                                callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(t)(o,p); });
                            }
                            out["object"]=Region(o,0x40,true);
                        }
                        // clon nuevo del pool de eventos: tipo + bytes; despues se libera
                        json clone=nullptr; void* fresh=nullptr;
                        { const auto now2=EvOccupancy(); for(size_t k=0;k<now2.size();++k)if(now2[k]&&!evOcc[k]){ fresh=EvStorage()+k*EVENT_SLOT; break; } }
                        if(fresh)clone={{"at",Name(fresh)},{"type",int(reinterpret_cast<CEvent*>(fresh)->GetEventType())},{"words",Region(fresh,EVENT_SLOT,true)}};
                        out["clone"]=clone; out["refs"]=AllRefs();
                        CleanRefs(); if(fresh)GetEventPool()->Delete(static_cast<CEvent*>(fresh));
                    }
                    // ---------------------------------------------------------------- repartidores
                    else if(cls=="CTaskAllocatorAttack"||cls=="CTaskAllocatorKillThreatsDriveby") {
                        const bool atk=cls=="CTaskAllocatorAttack"; const uintptr_t vtbl=atk ? VT_ATK : VT_DRV; const size_t osz=atk ? 0x1C : 0x14;
                        auto pickPed=[&]{ return reinterpret_cast<CPed*>(pick({uintptr_t(Ped(6)),uintptr_t(Ped(6)),uintptr_t(Ped(5)),0})); };
                        auto setTimer=[&](size_t off){
                            const uint32 m=next()%6;
                            At<uint8>(o,off+8)=uint8(m!=0); At<uint8>(o,off+9)=uint8(next()%4==0);
                            const int32 iv=int32(pick({0,3000,5000,1,uint32(-1),100}));
                            At<int32>(o,off+4)=iv;
                            const int32 d=m==1 ? iv : m==2 ? iv-1 : m==3 ? iv+1 : int32(next()%8000)-1000;
                            At<uint32>(o,off)=now-uint32(d);
                        };
                        // fixture comun de AllocateTasks/ProcessGroup: inteligencia de grupo, grupos y tarea BeInGroup
                        auto setGroups=[&](CPed* threat){
                            const uint32 gid=next()%8; auto* tg=reinterpret_cast<uint8*>(MSGROUPS+GROUP_SIZE*gid);
                            for(int k=0;k<8;++k)At<CPed*>(tg,G_MEMBERS+4*k)=next()%3 ? Ped(3+int(next()%5)) : nullptr;
                            for(int k=0;k<8;++k)At<CPed*>(group.data(),G_MEMBERS+4*k)=next()%4 ? Ped(int(next()%6)) : nullptr;
                            At<void*>(gintel.data(),0)=next()%6==0 ? (void*)tg : (void*)group.data();
                            auto* bg=fakes[0].data(); At<int32>(bg,FK_TYPE)=TASK_COMPLEX_BE_IN_GROUP; At<int32>(bg,0xC)=int32(gid);
                            findTaskRet=next()%2 ? uintptr_t(bg) : 0;
                            for(int k=0;k<8;++k){ auto* pp=peds[k].data(); At<uint32>(pp,P_FLAGS1)=next()%2 ? (next()|0x100u) : (next()&~0x100u);
                                At<uintptr_t>(pp,P_VEHICLE)=next()%2 ? V0 : V1; }
                            At<uintptr_t>(vehs[0].data(),V_DRIVER)=pick({P0,P1,uintptr_t(Ped(2)),0,uintptr_t(Ped(4))});
                            At<uintptr_t>(vehs[1].data(),V_DRIVER)=pick({uintptr_t(Ped(3)),0,uintptr_t(Ped(5))});
                            (void)threat;
                        };
                        if(isCtor){
                            std::memset(o,0xCD,0x40);
                            if(atk){ CPed* t1=pickPed(); const int32 id=int32(pick({uint32(-1),0,3,7})); CPed* t2=reinterpret_cast<CPed*>(pick({P1,0,uintptr_t(Ped(6))}));
                                callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,CPed*)>(t)(o,t1,id,t2)); }); }
                            else { CPed* t1=pickPed(); callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(t)(o,t1)); }); }
                            retIsPtr=true; out["object"]=Region(o,0x40,true); out["type"]=int32(reinterpret_cast<CTaskAllocator*>(o)->GetType());
                        } else {
                            for(size_t b=0;b<0x40;++b)object[b]=uint8(next());
                            At<uintptr_t>(o,0)=vtbl;
                            CPed* const threat=pickPed(); At<CPed*>(o,4)=threat; Reg(threat,static_cast<uint8*>(o)+4);
                            if(atk){ At<int32>(o,8)=int32(pick({uint32(-1),0,2,7})); CPed* org=reinterpret_cast<CPed*>(pick({P1,0,uintptr_t(Ped(6))})); At<CPed*>(o,0xC)=org; Reg(org,static_cast<uint8*>(o)+0xC); }
                            setTimer(atk ? 0x10 : 8);
                            if(nm=="Destructor"){
                                callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                            } else if(nm=="GetType"){
                                callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(t)(o); });
                            } else {
                                setGroups(threat);
                                if(nm=="AllocateTasks")callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*)>(t)(o,gintel.data()); });
                                else { callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(t)(o,gintel.data())); }); retIsPtr=true; }
                            }
                            out["object"]=Region(o,0x40,true);
                        }
                        // tareas nuevas en el pool (la secuencia de la pila las borra al destruirse)
                        json tasks=json::array(); auto* pool=GetTaskPool();
                        for(size_t k=0;k<tpOcc.size();++k)if(!tpOcc[k]&&!pool->IsFreeSlotAtIndex(k))tasks.push_back(SerTask(pool->GetAt(k),-1,TASK_SLOT));
                        out["tasks"]=tasks; out["refs"]=AllRefs();
                        out["msgroups"]=Region(reinterpret_cast<void*>(MSGROUPS),MSGROUPS_SIZE);
                        CleanRefs();
                    }
                    // ---------------------------------------------------------------- CPedScriptedTaskRecord
                    else if(cls=="CPedScriptedTaskRecord") {
                        auto* recs0=reinterpret_cast<uint8*>(SCRIPTED);
                        const uint32 density=nm=="GetVacantSlot" ? (next()%4==0 ? 100u : 97u) : nm=="Process" ? 6u : 30u;
                        for(int k=0;k<128;++k){ auto* r=recs0+0x14*k;
                            At<int32>(r,0)=int32(next()%3 ? 0x600+next()%8 : uint32(-1));
                            At<int32>(r,0x10)=int32(next()%8)-1;
                            if(next()%100<density){
                                const uint32 m=next()%8;
                                At<uintptr_t>(r,4)=m&1 ? uintptr_t(evs[next()%2].data()) : 0;
                                At<uintptr_t>(r,8)=m&2 ? uintptr_t(Fake(1+int(next()%(FAKES-1)))) : 0;
                                At<uintptr_t>(r,0xC)=m&4||m==0 ? uintptr_t(Ped(int(next()%NPEDS))) : 0;
                                if(nm=="Process")At<int32>(r,0x10)=int32(pick({1,1,2,4,5,6,0}));
                            } else { At<uintptr_t>(r,4)=0; At<uintptr_t>(r,8)=0; At<uintptr_t>(r,0xC)=0; }
                            Reg(At<void*>(r,0xC),r+0xC);
                        }
                        if(nm=="GetRecordAssociatedWithEvent"){ void* ev=reinterpret_cast<void*>(pick({uintptr_t(evs[0].data()),uintptr_t(evs[1].data()),0,uintptr_t(refEv.data())}));
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__cdecl*)(void*)>(t)(ev)); }); retIsPtr=true; }
                        else if(nm=="GetStatus-"){ void* p=reinterpret_cast<void*>(pick({P0,P1,uintptr_t(Ped(7)),0,uintptr_t(Ped(4))}));
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__cdecl*)(void*)>(t)(p); }); }
                        else if(nm=="GetStatus-opcode"){ void* p=reinterpret_cast<void*>(pick({P0,P1,uintptr_t(Ped(7)),0,uintptr_t(Ped(4))})); const int32 op=int32(next()%3 ? 0x600+next()%9 : uint32(-1));
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__cdecl*)(void*,int32)>(t)(p,op); }); }
                        else if(nm=="GetVacantSlot"){ callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__cdecl*)()>(t)(); }); }
                        else { // Process: gestores de tareas de las inteligencias con tareas falsas
                            for(int n=0;n<NPEDS;++n){ auto* in=intels[n].data();
                                for(int k=0;k<5;++k)At<uintptr_t>(in,I_TASKMGR+4*k)=next()%3==0 ? uintptr_t(Fake(1+int(next()%(FAKES-1)))) : 0;
                                for(int k=0;k<6;++k)At<uintptr_t>(in,I_TASKMGR+0x14+4*k)=next()%4==0 ? uintptr_t(Fake(1+int(next()%(FAKES-1)))) : 0; }
                            brainRet=next()%3 ? uintptr_t(Fake(1+int(next()%(FAKES-1)))) : 0; pedsGroupRet=next()%3 ? uintptr_t(group.data()) : 0;
                            scriptCmdRet=next()%3 ? uintptr_t(Fake(1+int(next()%(FAKES-1)))) : 0;
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__cdecl*)()>(t)(); });
                        }
                        out["records"]=Region(recs0,SCRIPTED_SIZE); out["refs"]=AllRefs();
                        CleanRefs();
                    }
                    // ---------------------------------------------------------------- CPlayerPedData
                    else if(cls=="CPlayerPedData") {
                        if(isCtor)std::memset(o,0xCD,0x100); else for(auto& b:object)b=uint8(next());
                        for(auto& b:arenaC)b=0xCD;
                        if(nm=="AllocateData"){
                            for(auto& b:arenaW)b=0xCD; for(auto& b:wanted)b=uint8(next());
                            At<uintptr_t>(o,0)=next()%2 ? uintptr_t(wanted.data()) : 0; At<uintptr_t>(o,4)=next()%2 ? uintptr_t(arenaC.data()) : 0;
                            callTarget([&](uintptr_t t){ EntryPatch nw(0x82119A,FakeNew), dl(0x8214BD,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                            out["arenaW"]=Region(arenaW.data(),arenaW.size()); out["wanted"]=Region(wanted.data(),wanted.size());
                        } else if(nm=="DeAllocateData"||nm=="Destructor"){
                            // CWanted a cero: el sonido temporal sin entidad fisica y sin ser el escaner que controla
                            std::memset(arenaW.data(),0,arenaW.size());
                            At<uintptr_t>(o,0)=next()%3 ? uintptr_t(arenaW.data()) : 0; At<uintptr_t>(o,4)=next()%3 ? uintptr_t(arenaC.data()) : 0;
                            callTarget([&](uintptr_t t){ EntryPatch nw(0x82119A,FakeNew), dl(0x8214BD,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                        } else {
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(t)(o)); }); retIsPtr=isCtor;
                        }
                        out["object"]=Region(o,0x100); out["recs"]=Recs(); out["arenaC"]=Region(arenaC.data(),arenaC.size());
                    }
                    // ---------------------------------------------------------------- CUpsideDownCarCheck
                    else if(cls=="CUpsideDownCarCheck") {
                        // pool falso: estados [-1..15] (el byte de guarda es poolStates[7]), huecos 0xA18
                        auto* states=poolStates.data()+8; std::memset(poolStates.data(),0x80,poolStates.size());
                        for(int k=0;k<NPOOL;++k)states[k]=uint8(next()%4==0 ? 0x80|next() : next()&0x7F);
                        states[-1]=uint8(next()%4==0 ? 0xFF : next()%2 ? 0x80 : 0x3A);
                        std::memset(poolHdr.data(),0,poolHdr.size()); At<void*>(poolHdr.data(),0)=PoolVeh(1); At<void*>(poolHdr.data(),4)=states; At<int32>(poolHdr.data(),8)=NPOOL; At<int32>(poolHdr.data(),0xC)=-1;
                        for(int k=0;k<=NPOOL;++k){ auto* v=PoolVeh(k); std::memset(v,0,POOL_SLOT); std::memset(poolMats[k].data(),0,poolMats[k].size());
                            At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=2; At<void*>(v,E_MATRIX)=poolMats[k].data();
                            At<uint32>(v,V_TYPE)=uint32(pick({0,0,9,9,3,5,6}));
                            v[V_WHEELS_AUTO]=uint8(next()%6); v[V_WHEELS_BIKE]=uint8(next()%6);
                            static const uint32 ups[]{0x3E99999Au,0x3E99999Bu,0x3E999999u,0x80000000u,0u,0x7FC00000u,0xBF800000u,0x3F800000u};
                            At<uint32>(poolMats[k].data(),0x28)=next()%3 ? ups[next()%std::size(ups)] : F(uni(-1.f,1.f)); }
                        auto handle=[&]()->int32{
                            const int32 idx=int32(next()%(NPOOL+1))-1; const uint32 m=next()%5;
                            const uint8 low=m<3 ? states[idx] : uint8(next());
                            return m==4 ? int32(pick({uint32(-1),0,uint32(-2),0x100})) : int32((uint32(idx)<<8)|low);
                        };
                        for(int k=0;k<6;++k){ At<int32>(o,8*k)=next()%3==0 ? -1 : handle(); static const uint32 tms[]{1999,2000,2001,0,0xFFFFFFFFu}; At<uint32>(o,8*k+4)=next()%2 ? tms[next()%5] : next()%5000; }
                        if(nm=="AddCarToCheck"||nm=="RemoveCarFromCheck"||nm=="HasCarBeenUpsideDownForAWhile"){
                            const int32 h=next()%2 ? At<int32>(o,8*int(next()%6)) : handle();
                            if(nm=="HasCarBeenUpsideDownForAWhile")callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(t)(o,h); });
                            else callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,int32)>(t)(o,h); });
                        } else if(nm=="IsCarUpsideDown-vehicle"){
                            void* v=PoolVeh(1+int(next()%NPOOL));
                            callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(t)(o,v); });
                        } else { // UpdateTimers sobre el pool falso
                            static const uint32 steps[]{0x3F800000u,0x40000000u,0x3F7FFFFFu,0x40400000u,0u,0x3E4CCCCDu};
                            At<uint32>(reinterpret_cast<void*>(TIMESTEP),0)=next()%2 ? steps[next()%std::size(steps)] : F(uni(0.f,3.f));
                            At<void*>(reinterpret_cast<void*>(VEHPOOL),0)=poolHdr.data();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*)>(t)(o); });
                            restoreGlobals();
                        }
                        out["object"]=Region(o,0x30);
                    }
                    // ---------------------------------------------------------------- CVehicleAnimGroup / CVehicleAnimGroupData
                    else {
                        const bool data=cls=="CVehicleAnimGroupData";
                        const uint32 gid=next()%30; auto* g=data ? reinterpret_cast<uint8*>(ANIMGROUPS+ANIMGROUP*gid) : static_cast<uint8*>(o);
                        for(size_t b=0;b<(data ? ANIMGROUP : 0x100);b+=4)At<uint32>(g,b)=rndF();
                        At<uint8>(g,0)=uint8(next()); At<uint8>(g,1)=uint8(next());
                        // desplazamientos de puerta: cero (con signo), diminutos (desnormales), NaN o normales
                        for(int k=0;k<7;++k){
                            auto* d=g+0x40+12*k; const uint32 m=next()%6;
                            for(int c=0;c<3;++c)At<uint32>(d,4*c)=m<2 ? (next()%2 ? 0x80000000u : 0u) : m==2 ? (next()%3==0 ? 1u : 0u) : m==3 ? (c==0 ? 0x7FC00000u : 0u) : F(uni(-3.f,3.f)); }
                        // animacion falsa: asociacion -> jerarquia (+0x10) -> secuencias (+4) -> fotogramas (+8)
                        std::memset(assoc.data(),0,assoc.size()); At<void*>(assoc.data(),0x10)=hier.data();
                        std::memset(hier.data(),0,hier.size()); At<void*>(hier.data(),4)=seq.data();
                        std::memset(seq.data(),0,seq.size()); At<uint16>(seq.data(),4)=uint16(next()%2 ? 3 : 1);   // con/sin traslacion, sin comprimir
                        At<uint16>(seq.data(),6)=uint16(pick({0,1,3,8,5,0xFFFF,0x8001})); At<void*>(seq.data(),8)=frames.data();
                        for(size_t b=0;b<frames.size();b+=4)At<uint32>(frames.data(),b)=rndF();
                        auto animId=[&]()->int32{ const uint32 m=next()%8; return m<5 ? int32(351+next()%40) : m==5 ? 377 : m==6 ? int32(next()%40) : int32(pick({0,31,32,63,64,400,uint32(-1),uint32(-31),340,350,391})); };
                        if(nm=="ComputeAnimDoorOffsets"){ const int32 door=int32(next()%8==0 ? 7 : next()%7); CVector res{};
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,int32)>(t)(o,&res,door)); });
                            ret={F(res.x),F(res.y),F(res.z),rawRet==uintptr_t(&res)}; }
                        else if(nm=="ComputeCriticalBlendTime"){ const int32 a=animId(); float r{};
                            callTarget([&](uintptr_t t){ r=reinterpret_cast<float(__thiscall*)(void*,int32)>(t)(o,a); }); ret=F(r); }
                        else if(nm=="CopyAnimGroup"){ for(auto& b:src)b=uint8(next()); for(size_t b=0;b<src.size();b+=4)At<uint32>(src.data(),b)=rndF();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,void*)>(t)(o,src.data()); }); }
                        else if(nm=="GetGroup"){ const int32 a=animId(); callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__thiscall*)(void*,int32)>(t)(o,a); }); }
                        else if(nm=="InitAnimGroup"){
                            const uint32 g1=(next()&~0xFFu)|uint32(ANIM_GROUP_CARS_BEGIN+next()%(ANIM_GROUP_CARS_END-ANIM_GROUP_CARS_BEGIN));
                            const uint32 g2=(next()&~0xFFu)|uint32(ANIM_GROUP_CARS_BEGIN+next()%(ANIM_GROUP_CARS_END-ANIM_GROUP_CARS_BEGIN));
                            const int32 fl=int32(next()), sp=int32(next());
                            for(auto* b:{&timA,&timB,&timC})for(size_t k=0;k<b->size();k+=4)At<uint32>(b->data(),k)=rndF();
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__thiscall*)(void*,uint32,uint32,int32,int32,void*,void*,void*)>(t)(o,g1,g2,fl,sp,timA.data(),timB.data(),timC.data()); }); }
                        else if(nm=="GetAnimDoorOffset"){ const int32 door=int32(next()%7); CVector res{};
                            callTarget([&](uintptr_t t){ rawRet=uintptr_t(reinterpret_cast<CVector*(__cdecl*)(CVector*,int32,int32)>(t)(&res,int32(gid),door)); });
                            ret={F(res.x),F(res.y),F(res.z),rawRet==uintptr_t(&res)}; }
                        else if(nm=="GetGroupForAnim"){ const int32 a=animId(); callTarget([&](uintptr_t t){ ret=reinterpret_cast<int32(__cdecl*)(int32,int32)>(t)(int32(gid),a); }); }
                        else if(nm=="GetInOutTimings"){ const int32 mode=int32(next()%4); float s0{}, s1{};
                            callTarget([&](uintptr_t t){ reinterpret_cast<void(__cdecl*)(int32,int32,float*,float*)>(t)(int32(gid),mode,&s0,&s1); }); ret={F(s0),F(s1)}; }
                        else { callTarget([&](uintptr_t t){ ret=reinterpret_cast<bool(__cdecl*)(int32)>(t)(int32(gid)); }); }
                        out["group"]=Region(g,data ? ANIMGROUP : 0x100); out["src"]=Region(src.data(),src.size());
                        out["anim"]={Region(assoc.data(),assoc.size()),Region(hier.data(),hier.size()),Region(seq.data(),seq.size()),Region(frames.data(),frames.size())};
                    }
                    if(retIsPtr)ret=Name(rawRet);
                    out["ret"]=ret; out["trace"]=trace; out["switches"]=switches; out["rng"]=rngCalls;
                    out["clean"]=EvOccupancy()==evOcc&&TPOccupancy()==tpOcc;
                    restoreGlobals(); restoreEvents();
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
    CleanRefs(); restoreGlobals(); restoreEvents(); TPLast()=lf0;
    for(int32 k=0;k<TPCap();++k)if(TPStates()[k]&0x80)std::memset(TPStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=EvOccupancy()==evOccStart&&TPOccupancy()==tpOccStart&&freeRefs()==savedFree&&globalsIntact()
        &&std::memcmp(EvHdr(),savedEvHdr.data(),savedEvHdr.size())==0;
    if(restored)std::memcpy(TPStates(),tpStatesStart.data(),tpStatesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
