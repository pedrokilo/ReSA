#pragma once
// Banco dentro del motor: tareas de cobertura, cambio de asiento y coche (lista de 10, 40 funciones).
// CTaskComplexSeekCoverUntilTargetDead (constructores, destructor, Clone, GetTaskType, GetCoverPed, GetTargetPed,
// ControlSubTask, CreateFirstSubTask, CreateNextSubTask), CTaskComplexShuffleSeats (constructor, destructor, Clone,
// GetTaskType, MakeAbortable, CreateSubTask, CreateTaskUtilityLineUpPedWithCar, ControlSubTask, CreateFirstSubTask,
// CreateNextSubTask), CTaskSimpleCarAlign y CTaskSimpleCarCloseDoorFromInside (constructor, destructor, Clone,
// GetTaskType, MakeAbortable, ProcessPed, SetPedPosition, StartAnim, callback de fin y FixHeading / ComputeAnimID).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 8 peatones (matriz, armas, estado), 2 vehiculos del tamano de CAutomobile (vtable falsa: hueco 28 ProcessOpenDoor),
// grupo falso (GetPedsGroup), utilidad de alineacion falsa, asociaciones de animacion falsas y tareas falsas.
// Se simulan y trazan sin reservar memoria (registro fijo, se convierte a json despues de cada llamada):
// CPedGroups::GetPedsGroup, CVehicle::RemoveDriver/RemovePassenger, CCarEnterExit::ComputeTargetDoorToExit/
// ComputePassengerIndexFromCarDoor/GetPositionToOpenCarDoor/IsVehicleHealthy/IsPedHealthy, CAnimManager::BlendAnimation,
// CAnimBlendAssociation::SetFinishCallback, CVehicleAnimGroup::GetGroup, CTaskUtilityLineUpPedWithCar::ProcessPed y el RNG.
// operator new/delete (0x82119A/0x8214BD) se sustituyen durante las llamadas de CTaskComplexShuffleSeats: la utilidad de
// alineacion nueva sale de una arena propia y los borrados se registran.
// Pool de tareas: m_LastFreeSlot y estados se restauran y los slots libres se rellenan con 0xCD en cada ejecucion.
// CTimer::m_snTimeInMilliseconds, CPedGroups::ms_groups y los grupos de animacion de vehiculo (banderas especiales)
// se guardan y se restauran.
// FIX_BUGS: CTaskComplexSeekCoverUntilTargetDead::ControlSubTask con el peaton cubierto nulo o muerto busca otro con
// GetCoverPed (el EXE usa GetTargetPed): sus fixtures llevan siempre un cubierto vivo.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Tasks/Task.h"
#include "TaskComplexSeekCoverUntilTargetDead.h"
#include "TaskComplexShuffleSeats.h"
#include "TaskSimpleCarAlign.h"
#include "TaskSimpleCarCloseDoorFromInside.h"
#include "TaskUtilityLineUpPedWithCar.h"
#include "PedGroups.h"
#include "CarEnterExit.h"
#include "VehicleAnimGroupData.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks10bLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

constexpr int NPEDS=8, NVEH=2, FAKES=2;
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68;
// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_CLUMP=0x18, E_FLAGS=0x1C, E_TYPE=0x36;                                        // CEntity
constexpr size_t P_FLAGS=0x46C, P_STATE=0x530, P_CURROT=0x558, P_AIMROT=0x55C, P_VEHICLE=0x58C, P_WEAPONS=0x5A0,
                 P_ACTSLOT=0x718;                                                                                // CPed
constexpr size_t V_HANDLING=0x384, V_DRIVER=0x460, V_PASS=0x464, V_TYPE=0x590, H_ANIMGROUP=0xDE;                 // CVehicle
constexpr size_t A_BLEND=0x18, A_DELTA=0x1C, A_TIME=0x20, A_ID=0x2C;                                            // CAnimBlendAssociation
constexpr uintptr_t TIMER=0xB7CB84, PEDGROUPS=0xC09920, ANIMGROUPS=0xC1CDC0, EXE_DEFAULT_ANIM_CB=0x4D1390;
constexpr size_t PEDGROUP_SIZE=0x2D4, ANIMGROUP_SIZE=0x94, LU_SIZE=0x1C;
constexpr uintptr_t VT_SC=0x86DAC4, VT_SS=0x86EA40, VT_CA=0x86EC78, VT_CD=0x86ED08;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<uint8,0x80> clump{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,2> anims{};   // 0: m_anim del fixture, 1: devuelta por BlendAnimation
alignas(16) inline std::array<uint8,0x40> lineup{}, luArena{}, luHeap{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x40> group{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x10> vecBuf{}, outBuf{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline int PedIndex(const void* p) { for(int n=0;n<NPEDS;++n)if(p==peds[n].data())return n; return -1; }

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

inline std::array<void*,16> fakeVtbl{};
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline std::unordered_map<uintptr_t,std::string> cbNames;   // callbacks del EXE y de la DLL
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=cbNames.find(v);it!=cbNames.end())return it->second;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt"; if(v==uintptr_t(pedVtbl.data()))return "ped_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(anims[n].data(),anims[n].size(),"anim"))return *s+"#"+std::to_string(n);
    if(auto s=in(handling.data(),handling.size(),"handling"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(lineup.data(),lineup.size(),"lineup"))return *s; if(auto s=in(luArena.data(),luArena.size(),"lu_arena"))return *s;
    if(auto s=in(luHeap.data(),luHeap.size(),"lu_heap"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s;
    if(auto s=in(outBuf.data(),outBuf.size(),"out"))return *s;
    if(v>=PEDGROUPS&&v<PEDGROUPS+8*PEDGROUP_SIZE)return "pedgroups+"+std::to_string(v-PEDGROUPS);
    if(v>=ANIMGROUPS&&v<ANIMGROUPS+30*ANIMGROUP_SIZE)return "animgroup+"+std::to_string(v-ANIMGROUPS);
    if(IsPoolTask(v))return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global: se enmascara
inline std::string EntHash(const void* buf,size_t n) {
    std::vector<uint8> copy(static_cast<const uint8*>(buf),static_cast<const uint8*>(buf)+n);
    const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf)->m_pReferences)-uintptr_t(buf);
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}

// traza sin reservas: registros fijos (los punteros se nombran al convertir)
enum Tag : uint16 { T_DEL_FAKE, T_ABORT_FAKE, T_UNEXPECTED, T_PED_UNEXPECTED, T_VEH_UNEXPECTED, T_RAND, T_PEDS_GROUP, T_REMOVE_DRIVER,
    T_REMOVE_PASSENGER, T_EXIT_DOOR, T_PASS_INDEX, T_DOOR_POS, T_VEH_HEALTHY, T_PED_HEALTHY, T_BLEND, T_SET_FINISH_CB, T_GET_GROUP,
    T_LINE_UP, T_OPEN_DOOR, T_NEW, T_DELETE, T_COUNT };
inline const std::array<const char*,T_COUNT> tagNames{"delete_fake","make_abortable_fake","unexpected_slot","ped_unexpected_slot","veh_unexpected_slot",
    "rand","get_peds_group","remove_driver","remove_passenger","compute_target_door_to_exit","compute_passenger_index_from_car_door",
    "get_position_to_open_car_door","is_vehicle_healthy","is_ped_healthy","blend_animation","set_finish_callback","anim_group_get_group",
    "line_up_process_ped","process_open_door","operator_new","operator_delete"};
struct Rec { uint16 tag; uint8 n; uint16 ptrMask; std::array<uint32,9> a; };
inline std::array<Rec,96> recs{}; inline uint32 nrec{}, lostRecs{};
template<class... A> void Push(Tag tag,uint16 ptrMask,A... a) {
    if(nrec>=recs.size()){++lostRecs;return;}
    auto& r=recs[nrec++]; r.tag=tag; r.n=uint8(sizeof...(A)); r.ptrMask=ptrMask; size_t k=0; ((r.a[k++]=uint32(a)),...);
}
inline json TraceJson(uint32 from=0) {
    json t=json::array();
    for(uint32 k=from;k<nrec;++k){ const auto& r=recs[k]; json e=json::array(); e.push_back(tagNames[r.tag]);
        for(uint8 j=0;j<r.n;++j)e.push_back((r.ptrMask>>j)&1 ? Name(uintptr_t(r.a[j])) : json(r.a[j])); t.push_back(e); }
    return t;
}
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }

// configuracion por caso de las dependencias simuladas
inline uint32 rngState{};
inline uintptr_t pedsGroupRet{};
inline int32 exitDoorRet{}; inline std::array<int32,4> passIndexRet{};
inline uint8 vehHealthyRet{}, pedHealthyRet{};
inline CVector doorPosRet{};
inline int32 groupRet{}; inline bool lineupRet{};
inline uint32 arenaTop{};

// tareas falsas
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { Push(T_DEL_FAKE,1,U(t),f); return t; }
inline CTask* __fastcall FNull(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) { Push(T_ABORT_FAKE,0b1011,U(t),U(p),prio,U(e)); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { Push(T_UNEXPECTED,2,N,U(self)); return nullptr; }
template<int N> void* __fastcall PTrap(void* self,void*) { Push(T_PED_UNEXPECTED,2,N,U(self)); return nullptr; }
template<int N> void* __fastcall VTrap(void* self,void*) { Push(T_VEH_UNEXPECTED,2,N,U(self)); return nullptr; }
template<size_t S,int... N> void FillF(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
template<size_t S,int... N> void FillP(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<size_t S,int... N> void FillV(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
inline void __fastcall OpenDoor(CVehicle* v,void*,CPed* p,uint32 door,uint32 grp,uint32 id,float t) { Push(T_OPEN_DOOR,0b11,U(v),U(p),door,grp,id,F(t)); }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); Push(T_RAND,0,r); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { Push(T_PEDS_GROUP,0b11,U(p),uint32(pedsGroupRet)); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline void __fastcall FakeRemoveDriver(CVehicle* v,void*,uint32 b) { Push(T_REMOVE_DRIVER,1,U(v),b&0xFF); }
inline void __fastcall FakeRemovePassenger(CVehicle* v,void*,CPed* p) { Push(T_REMOVE_PASSENGER,0b11,U(v),U(p)); }
inline int32 __cdecl FakeExitDoor(const CVehicle* v,const CPed* p) { Push(T_EXIT_DOOR,0b11,U(v),U(p),exitDoorRet); return exitDoorRet; }
inline int32 __cdecl FakePassIndex(const CVehicle* v,int32 door) { const int32 r=passIndexRet[uint32(door)&3]; Push(T_PASS_INDEX,1,U(v),door,r); return r; }
inline CVector* __cdecl FakeDoorPos(CVector* out,const CVehicle* v,int32 door) { Push(T_DOOR_POS,1,U(v),door); *out=doorPosRet; return out; }
inline bool __cdecl FakeVehHealthy(const CVehicle* v) { Push(T_VEH_HEALTHY,1,U(v),vehHealthyRet); return vehHealthyRet!=0; }
inline bool __cdecl FakePedHealthy(CPed* p) { Push(T_PED_HEALTHY,1,U(p),pedHealthyRet); return pedHealthyRet!=0; }
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 grp,int32 id,float delta) { Push(T_BLEND,1,U(c),grp,id,F(delta)); return reinterpret_cast<CAnimBlendAssociation*>(anims[1].data()); }
inline void __fastcall FakeSetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { Push(T_SET_FINISH_CB,0b111,U(a),U(cb),U(data)); }
inline int32 __fastcall FakeGetGroup(const void* g,void*,int32 id) { Push(T_GET_GROUP,1,U(g),id,groupRet); return groupRet; }
inline bool __fastcall FakeLineUp(void* u,void*,CPed* p,CVehicle* v,CAnimBlendAssociation* a) { Push(T_LINE_UP,0b1111,U(u),U(p),U(v),U(a)); return lineupRet; }
// operator new/delete del EXE durante las llamadas de CTaskComplexShuffleSeats: la utilidad sale de la arena
inline void* __cdecl FakeNew(size_t n) { void* r=nullptr; if(n==LU_SIZE&&arenaTop+0x20<=luArena.size()){r=luArena.data()+arenaTop;arenaTop+=0x20;} Push(T_NEW,2,uint32(n),U(r)); return r; }
inline void __cdecl FakeDelete(void* p) { Push(T_DELETE,1,U(p)); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=40;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexSeekCoverUntilTargetDead","Clone",0x623900,0x86DAC4,1,256},
    {"CTaskComplexSeekCoverUntilTargetDead","Constructor_Grp",0x622B80,0,-1,128},
    {"CTaskComplexSeekCoverUntilTargetDead","Constructor_Ped",0x622BC0,0,-1,128},
    {"CTaskComplexSeekCoverUntilTargetDead","ControlSubTask",0x62E8B0,0x86DAC4,10,1024},
    {"CTaskComplexSeekCoverUntilTargetDead","CreateFirstSubTask",0x62E6C0,0x86DAC4,9,1024},
    {"CTaskComplexSeekCoverUntilTargetDead","CreateNextSubTask",0x622C90,0x86DAC4,8,512},
    {"CTaskComplexSeekCoverUntilTargetDead","Destructor",0x622C20,0,-1,256},
    {"CTaskComplexSeekCoverUntilTargetDead","GetCoverPed",0x622D50,0,-1,1024},
    {"CTaskComplexSeekCoverUntilTargetDead","GetTargetPed",0x622CC0,0,-1,1024},
    {"CTaskComplexSeekCoverUntilTargetDead","GetTaskType",0x622BB0,0x86DAC4,4,64},
    {"CTaskComplexShuffleSeats","Clone",0x63DEB0,0x86EA40,1,128},
    {"CTaskComplexShuffleSeats","Constructor",0x63D130,0,-1,128},
    {"CTaskComplexShuffleSeats","ControlSubTask",0x63D5C0,0x86EA40,10,512},
    {"CTaskComplexShuffleSeats","CreateFirstSubTask",0x643700,0x86EA40,9,512},
    {"CTaskComplexShuffleSeats","CreateNextSubTask",0x63D420,0x86EA40,8,1024},
    {"CTaskComplexShuffleSeats","CreateSubTask",0x63D240,0,-1,1024},
    {"CTaskComplexShuffleSeats","CreateTaskUtilityLineUpPedWithCar",0x63D610,0,-1,512},
    {"CTaskComplexShuffleSeats","Destructor",0x63D1A0,0,-1,256},
    {"CTaskComplexShuffleSeats","GetTaskType",0x63D190,0x86EA40,4,64},
    {"CTaskComplexShuffleSeats","MakeAbortable",0x63D220,0x86EA40,6,256},
    {"CTaskSimpleCarAlign","Clone",0x649930,0x86EC78,1,128},
    {"CTaskSimpleCarAlign","Constructor",0x645B40,0,-1,256},
    {"CTaskSimpleCarAlign","Destructor",0x645BE0,0,-1,256},
    {"CTaskSimpleCarAlign","FinishAnimCarAlignCB",0x645E40,0,-1,128},
    {"CTaskSimpleCarAlign","FixHeading",0x645CB0,0,-1,1024},
    {"CTaskSimpleCarAlign","GetTaskType",0x645BD0,0x86EC78,4,64},
    {"CTaskSimpleCarAlign","MakeAbortable",0x645C50,0x86EC78,6,256},
    {"CTaskSimpleCarAlign","ProcessPed",0x64D6D0,0x86EC78,7,512},
    {"CTaskSimpleCarAlign","SetPedPosition",0x645C70,0x86EC78,8,256},
    {"CTaskSimpleCarAlign","StartAnim",0x64AC00,0,-1,512},
    {"CTaskSimpleCarCloseDoorFromInside","Clone",0x649AF0,0x86ED08,1,128},
    {"CTaskSimpleCarCloseDoorFromInside","ComputeAnimID",0x646410,0,-1,256},
    {"CTaskSimpleCarCloseDoorFromInside","Constructor",0x646300,0,-1,256},
    {"CTaskSimpleCarCloseDoorFromInside","Destructor",0x646380,0,-1,256},
    {"CTaskSimpleCarCloseDoorFromInside","FinishAnimCarCloseDoorFromInsideCB",0x646490,0,-1,256},
    {"CTaskSimpleCarCloseDoorFromInside","GetTaskType",0x646370,0x86ED08,4,64},
    {"CTaskSimpleCarCloseDoorFromInside","MakeAbortable",0x64AF50,0x86ED08,6,256},
    {"CTaskSimpleCarCloseDoorFromInside","ProcessPed",0x64D9A0,0x86ED08,7,1024},
    {"CTaskSimpleCarCloseDoorFromInside","SetPedPosition",0x6463F0,0x86ED08,8,256},
    {"CTaskSimpleCarCloseDoorFromInside","StartAnim",0x64AFB0,0,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks10b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks10b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks10b");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks10b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // nombres de los callbacks: direccion del EXE, funcion de la DLL (y su destino si es un thunk)
    cbNames.clear();
    cbNames[EXE_DEFAULT_ANIM_CB]="DefaultAnimCB"; cbNames[std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB)]="DefaultAnimCB";
    cbNames[Follow(std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))]="DefaultAnimCB";
    for(auto [cls,cb,addr]:{std::tuple{"CTaskSimpleCarAlign","FinishAnimCarAlignCB",uintptr_t(0x645E40)},
                            std::tuple{"CTaskSimpleCarCloseDoorFromInside","FinishAnimCarCloseDoorFromInsideCB",uintptr_t(0x646490)}}){
        auto* h=FindHook(cls,cb);
        if(!h||h->Symbol()[0]!='S'){Write({{"event","error"},{"reason","callback hook not found"},{"function",std::string(cls)+"::"+cb}});return;}
        const auto our=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
        cbNames[addr]=cb; cbNames[our]=cb; cbNames[Follow(our)]=cb;
    }
    FillF(fakeVtbl,std::make_integer_sequence<int,16>{}); FillP(pedVtbl,std::make_integer_sequence<int,64>{}); FillV(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FNull); fakeVtbl[2]=reinterpret_cast<void*>(&FNull);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    vehVtbl[28]=reinterpret_cast<void*>(&OpenDoor);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{PEDGROUPS,8*PEDGROUP_SIZE},{ANIMGROUPS,30*ANIMGROUP_SIZE}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const int32 lf0=PoolLast();
    const std::vector<uint8> statesStart(PoolStates(),PoolStates()+PoolCap());
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup),FakePedsGroup), g1e(0x5F7E80,FakePedsGroup);
        EntryPatch v1(bit_cast<uintptr_t>(&CVehicle::RemoveDriver),FakeRemoveDriver), v1e(0x6D1950,FakeRemoveDriver);
        EntryPatch v2(bit_cast<uintptr_t>(&CVehicle::RemovePassenger),FakeRemovePassenger), v2e(0x6D1610,FakeRemovePassenger);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit),FakeExitDoor), c1e(0x64F110,FakeExitDoor);
        EntryPatch c2(bit_cast<uintptr_t>(&CCarEnterExit::ComputePassengerIndexFromCarDoor),FakePassIndex), c2e(0x64F1E0,FakePassIndex);
        EntryPatch c3(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor),FakeDoorPos), c3e(0x64E740,FakeDoorPos);
        EntryPatch c4(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleHealthy),FakeVehHealthy), c4e(0x64EEC0,FakeVehHealthy);
        EntryPatch c5(bit_cast<uintptr_t>(&CCarEnterExit::IsPedHealthy),FakePedHealthy), c5e(0x64EEE0,FakePedHealthy);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend), a1e(0x4D4610,FakeBlend);
        EntryPatch a2(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),FakeSetFinishCb), a2e(0x4CEBE0,FakeSetFinishCb);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<AssocGroupId(CVehicleAnimGroup::*)(AnimationId) const>(&CVehicleAnimGroup::GetGroup)),FakeGetGroup), a3e(0x6E3B00,FakeGetGroup);
        EntryPatch l1(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::ProcessPed),FakeLineUp), l1e(0x6513A0,FakeLineUp);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto around=[&](float t){ switch(next()%4){ case 0: return t; case 1: return std::nextafter(t,1000.f); case 2: return std::nextafter(t,-1000.f); default: return uni(t-3.f,t+3.f); } };
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // slots libres con 0xCD: los bytes que los constructores no escriben quedan iguales en ambas ejecuciones
                    PoolLast()=lf0;
                    if(Occupancy()==occupancyStart)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
                    for(int32 k=0;k<PoolCap();++k)if(PoolStates()[k]&0x80)std::memset(PoolStorage()+k*TASK_SLOT,0xCD,TASK_SLOT);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA10Bu; if(!st)st=1; nrec=0; lostRecs=0; freshIds.clear(); arenaTop=0;
                    CleanRefs(); restoreGlobals();
                    rngState=next();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    // peatones: vtable de trampas, matriz, clump, banderas, estado, armas
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=uint8(3|(next()%8)<<3); At<void*>(p,E_MATRIX)=pedMats[n].data(); At<void*>(p,E_CLUMP)=clump.data();
                        { CMatrix m{}; m.SetRotate(uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-3.f,3.f)); std::memcpy(pedMats[n].data(),&m,sizeof(CMatrix)); }
                        // posiciones: aleatorias, copiadas de otro peaton (empates de distancia) o con NaN
                        { const uint32 m=next()%10; At<CVector>(pedMats[n].data(),0x30)=n>1&&m==0 ? At<CVector>(pedMats[next()%n].data(),0x30) :
                                                     m==1 ? CVector{uni(-30.f,30.f),qnan,uni(0.f,10.f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)}; }
                        At<uint32>(p,E_FLAGS)=next(); At<uint32>(p,P_FLAGS)=(next()&~0x100u)|(next()%2 ? 0x100u : 0u);
                        { static const int32 states[]{PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_IDLE,PEDSTATE_DIE,PEDSTATE_DEAD,PEDSTATE_ARRESTED}; At<int32>(p,P_STATE)=states[next()%6]; }
                        At<float>(p,P_CURROT)=uni(-3.f,3.f); At<float>(p,P_AIMROT)=uni(-3.f,3.f);
                        { const uint8 slot=uint8(next()%13); p[P_ACTSLOT]=slot; static const int32 w[]{0,1,4,8,22,24,25,28,31,33,16,0};
                          At<int32>(p,P_WEAPONS+slot*0x1C)=w[next()%std::size(w)]; }
                    }
                    // vehiculos: vtable falsa, matriz, handling, tipo, conductor y pasajeros
                    for(auto& b:handling)b=uint8(next()); handling[H_ANIMGROUP]=uint8(next()%30);
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%8)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<void*>(v,V_HANDLING)=handling.data();
                        { CMatrix m{}; m.SetRotate(uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                          std::memcpy(vehMats[n].data(),&m,sizeof(CMatrix)); }
                        At<uint32>(v,V_TYPE)=next()%4 ? 0u : uint32(1+next()%10);
                        At<CPed*>(v,V_DRIVER)=next()%3==0 ? nullptr : Ped(int(next()%NPEDS));
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%2 ? nullptr : Ped(int(next()%NPEDS));
                    }
                    // banderas especiales de los grupos de animacion de vehiculo (global real, restaurado)
                    for(int g=0;g<30;++g){ auto& fl=At<uint32>(reinterpret_cast<void*>(ANIMGROUPS+g*ANIMGROUP_SIZE),8); fl=next()%3==0 ? (fl|2u) : (fl&~2u); }
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FAKE_SIZE); At<void*>(fakes[n].data(),0)=fakeVtbl.data(); At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_PAUSE+n; fakes[n][FK_ABORT]=uint8(next()%2); }
                    for(auto& a:anims){ for(auto& b:a)b=uint8(next());
                        At<float>(a.data(),A_BLEND)=uni(0.f,1.f); At<float>(a.data(),A_DELTA)=uni(-4.f,4.f); At<float>(a.data(),A_TIME)=uni(0.f,2.f);
                        { static const int16 ids[]{351,352,353,354,367,368,369,370,0}; At<int16>(a.data(),A_ID)=next()%6 ? ids[next()%9] : int16(next()%500); } }
                    for(auto& b:lineup)b=uint8(next()); for(auto& b:luHeap)b=uint8(next()); std::memset(luArena.data(),0xCD,luArena.size());
                    std::memset(group.data(),0,group.size()); for(auto& b:vecBuf)b=uint8(next()); for(auto& b:outBuf)b=uint8(next());
                    pedsGroupRet=0; exitDoorRet=int32(8+next()%4); for(auto& r:passIndexRet)r=int32(next()%8);
                    vehHealthyRet=uint8(next()%4!=0); pedHealthyRet=uint8(next()%4!=0);
                    doorPosRet=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                    { static const int32 grps[]{107,107,88,89,90,0}; groupRet=grps[next()%6]; } lineupRet=next()%2;

                    auto* o=object.data(); auto* p0=Ped(0); auto* v0=Veh(0);
                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; std::vector<CTask*> roots;
                    auto collectFresh=[&](CTask* r){
                        for(auto* h:hooks)h->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks;
                        if(r&&freshIds.contains(uintptr_t(r)))roots.push_back(r);
                    };
                    auto setTask=[&](uintptr_t vtbl,size_t size){ std::memset(o,0xCD,object.size()); for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto setSub=[&](const int32* types,size_t n){ At<CTask*>(o,8)=Fake(0); At<int32>(fakes[0].data(),FK_TYPE)=types[next()%n]; };
                    auto typeOf=[&](void* p){ return reinterpret_cast<CTask*>(p)->GetTaskType(); };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callClone=[&]{ return reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callBool=[&](CPed* ped){ return reinterpret_cast<uint8(__thiscall*)(void*,CPed*)>(target)(o,ped)!=0; };
                    auto callAbort=[&](CPed* ped,int32 prio){ return reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,ped,prio,nullptr)!=0; };
                    auto ctorDone=[&](size_t size){ for(auto* h:hooks)h->State(false); out["object"]=Region(o,size,true); out["type"]=typeOf(o); };
                    auto pos=[&](void* e)->CVector&{ return At<CVector>(At<void*>(e,E_MATRIX),0x30); };
                    auto pickPed=[&](int from){ return Ped(from+int(next()%uint32(NPEDS-from))); };
                    auto alive=[&](CPed* p){ At<int32>(p,P_STATE)=PEDSTATE_IDLE; };
                    // llamadas de CTaskComplexShuffleSeats: operator new/delete del EXE sustituidos (las dependencias no reservan)
                    auto withHeap=[&](auto&& call){ EntryPatch nw(0x82119A,FakeNew), dl(0x8214BD,FakeDelete); call(); };

                    if(nm=="GetTaskType") {
                        setTask(fn.vt,0x40);
                        ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o);
                        out["object"]=Region(o,0x40,true);
                    }
                    else if(cls=="CTaskComplexSeekCoverUntilTargetDead") {
                        // +0xC grupo, +0x10 objetivo (referencia), +0x14 cubierto (referencia)
                        const int32 grp=next()%3 ? int32(next()%8) : (next()%2 ? -1 : -2-int32(next()%5));
                        // miembros del grupo objetivo (grupo real, restaurado): peatones 1..7 o huecos; lider en el indice 7
                        if(grp>=0){ auto* members=reinterpret_cast<CPed**>(PEDGROUPS+grp*PEDGROUP_SIZE+0xC); for(int k=0;k<8;++k)members[k]=next()%3 ? pickPed(1) : nullptr; }
                        // grupo del peaton (GetPedsGroup): 8 miembros con huecos, el propio peaton incluido a veces
                        if(next()%5){ pedsGroupRet=uintptr_t(group.data()); for(int k=0;k<8;++k)At<CPed*>(group.data(),0xC+4*k)=next()%4==0 ? nullptr : next()%6==0 ? p0 : pickPed(1); }
                        if(nm=="Constructor_Grp"){
                            std::memset(o,0xCD,object.size());
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,grp));
                            ctorDone(0x18);
                        } else if(nm=="Constructor_Ped"){
                            std::memset(o,0xCD,object.size()); CPed* t=next()%4 ? pickPed(1) : nullptr;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(o,t));
                            ctorDone(0x18);
                        } else {
                            setTask(VT_SC,0x18); At<int32>(o,0xC)=grp;
                            CPed* t=next()%4 ? pickPed(1) : nullptr; CPed* c=next()%4 ? pickPed(1) : nullptr;
                            if(nm=="ControlSubTask"){ if(!c)c=pickPed(1); alive(c); }   // FIX_BUGS: el cubierto vivo (ver cabecera)
                            At<CPed*>(o,0x10)=t; Reg(t,o+0x10); At<CPed*>(o,0x14)=c; Reg(c,o+0x14);
                            if(nm=="Destructor"){ At<CTask*>(o,8)=nullptr; callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="GetCoverPed"){ ret=Name(reinterpret_cast<CPed*(__thiscall*)(void*,CPed*)>(target)(o,p0)); }
                            else if(nm=="GetTargetPed"){ ret=Name(reinterpret_cast<CPed*(__thiscall*)(void*)>(target)(o)); }
                            else {
                                static const int32 ty[]{TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_PAUSE};
                                setSub(ty,std::size(ty));
                                auto* r=callTask(p0); collectFresh(r); ret=Name(r);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x18,true);
                        }
                        out["refs"]=AllRefs();
                    }
                    else if(cls=="CTaskComplexShuffleSeats") {
                        // +0xC vehiculo (referencia), +0x10 puerta origen, +0x14 puerta destino, +0x18 vector, +0x24 utilidad, +0x28 abortar
                        At<CVehicle*>(p0,P_VEHICLE)=Veh(int(next()%NVEH));
                        if(nm=="Constructor"){
                            std::memset(o,0xCD,object.size()); CVehicle* v=next()%4 ? v0 : nullptr;
                            void* r{}; withHeap([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,CVehicle*)>(target)(o,v); }); ret=Name(r);
                            ctorDone(0x2C);
                        } else {
                            setTask(VT_SS,0x2C);
                            CVehicle* v=(nm=="Clone"||nm=="Destructor"||nm=="ControlSubTask"||nm=="CreateFirstSubTask"||nm=="MakeAbortable")&&next()%4==0 ? nullptr : v0;
                            At<CVehicle*>(o,0xC)=v; Reg(v,o+0xC);
                            { static const int32 doors[]{8,9,10,11,8,9,10,11,0,18}; At<int32>(o,0x10)=doors[next()%std::size(doors)]; At<int32>(o,0x14)=doors[next()%std::size(doors)]; }
                            At<void*>(o,0x24)=next()%4 ? static_cast<void*>(lineup.data()) : nullptr; o[0x28]=uint8(next()%3==0);
                            // peaton sentado: conductor, pasajero 0/1 u otro sitio
                            { const uint32 m=next()%5; if(m==0)At<CPed*>(v0,V_DRIVER)=p0; else if(m<3)At<CPed*>(v0,V_PASS+4*(m-1))=p0; }
                            if(nm=="Destructor"){
                                if(next()%3)At<void*>(o,0x24)=luHeap.data();
                                At<CTask*>(o,8)=nullptr; withHeap([&]{ callVoid(); });
                            } else if(nm=="Clone"){ CTask* r{}; withHeap([&]{ r=callClone(); }); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){
                                static const int32 ty[]{TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_PAUSE}; setSub(ty,std::size(ty));
                                const int32 prio=int32(next()%4); bool r{}; withHeap([&]{ r=callAbort(p0,prio); }); ret=r;
                            } else if(nm=="CreateSubTask"){
                                static const int32 ty[]{TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER,TASK_SIMPLE_CAR_SET_PED_OUT,
                                                        TASK_FINISHED,TASK_FINISHED,TASK_NONE,TASK_SIMPLE_CAR_GET_OUT,TASK_SIMPLE_PAUSE};
                                const int32 type=ty[next()%std::size(ty)]; CPed* ped=type==TASK_FINISHED&&next()%3==0 ? nullptr : p0;
                                CTask* r{}; withHeap([&]{ r=reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,type,ped); }); collectFresh(r); ret=Name(r);
                            } else if(nm=="CreateTaskUtilityLineUpPedWithCar"){
                                withHeap([&]{ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0); });
                            } else {
                                if(nm=="CreateNextSubTask"){
                                    static const int32 ty[]{TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,
                                                            TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER,TASK_SIMPLE_CAR_SET_PED_OUT,TASK_NONE,TASK_SIMPLE_PAUSE};
                                    setSub(ty,std::size(ty));
                                } else if(nm=="ControlSubTask"){
                                    static const int32 ty[]{TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_PAUSE}; setSub(ty,std::size(ty));
                                }
                                CTask* r{}; withHeap([&]{ r=callTask(p0); }); collectFresh(r); ret=Name(r);
                                out["sub"]=Region(fakes[0].data(),0x70,true);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,0x2C,true);
                        }
                        out["refs"]=AllRefs(); out["lu_arena"]=Region(luArena.data(),luArena.size()); out["lu_heap"]=Region(luHeap.data(),luHeap.size());
                        out["veh"]=json::array({EntHash(vehs[0].data(),vehs[0].size()),EntHash(vehs[1].data(),vehs[1].size())});
                        out["p0_flags"]=json::array({At<uint32>(p0,E_FLAGS),At<uint32>(p0,P_FLAGS)});
                    }
                    else {
                        // tareas simples de coche: +8 animacion terminada, +0xC animacion, +0x10 vehiculo (referencia);
                        // CarAlign: +0x14 posicion de la puerta, +0x20 puerta, +0x24 utilidad; CloseDoor: +0x14 puerta, +0x18 utilidad
                        const bool align=cls=="CTaskSimpleCarAlign"; const size_t size=align ? 0x28 : 0x1C, doorOff=align ? 0x20 : 0x14, luOff=align ? 0x24 : 0x18;
                        const uintptr_t vtc=align ? VT_CA : VT_CD;
                        const bool isCtor=nm=="Constructor";
                        auto door4=[&]{ return int32(8+next()%4); };
                        auto anyDoor=[&]{ static const int32 d[]{8,9,10,11,8,9,10,11,0,18,3}; return d[next()%std::size(d)]; };
                        void* const cbAnim=next()%2 ? static_cast<void*>(anims[0].data()) : static_cast<void*>(anims[1].data());
                        if(isCtor){
                            std::memset(o,0xCD,object.size()); CVehicle* v=next()%4 ? v0 : nullptr; const int32 d=anyDoor(); void* lu=next()%4 ? static_cast<void*>(lineup.data()) : nullptr;
                            if(align){ At<CVector>(vecBuf.data(),0)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                                ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,const CVector*,int32,void*)>(target)(o,v,reinterpret_cast<CVector*>(vecBuf.data()),d,lu)); }
                            else ret=Name(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,void*)>(target)(o,v,d,lu));
                            ctorDone(size);
                        } else {
                            std::memset(o,0xCD,object.size()); for(size_t b=8;b<size;++b)o[b]=uint8(next());
                            At<uintptr_t>(o,0)=vtc; At<uint32>(o,4)=0; o[8]=uint8(next()%4==0);
                            At<void*>(o,0xC)=next()%3 ? static_cast<void*>(anims[0].data()) : nullptr;
                            CVehicle* v=next()%6 ? v0 : nullptr; At<CVehicle*>(o,0x10)=v; Reg(v,o+0x10);
                            At<int32>(o,doorOff)=anyDoor(); At<void*>(o,luOff)=lineup.data();
                            if(align)At<CVector>(o,0x14)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,10.f)};
                            // altura de la puerta respecto al peaton: umbral 4,4 exacto (peaton en z = 0), vecinos, negativa o NaN
                            auto doorHeight=[&]{ const uint32 m=next()%6; auto& pp=pos(p0);
                                if(m<3){ pp.z=0.f; doorPosRet.z=around(4.4f); } else if(m==3) doorPosRet.z=pp.z-uni(0.f,3.f); else if(m==4) doorPosRet.z=qnan; };
                            if(nm=="Destructor"){ callVoid(); }
                            else if(nm=="Clone"){ auto* r=callClone(); collectFresh(r); ret=Name(r); }
                            else if(nm=="MakeAbortable"){
                                if(!align&&v)At<int32>(o,doorOff)=door4();   // ComputeAnimID con otra puerta usa una animacion sin inicializar
                                ret=callAbort(p0,next()%8==0 ? 3 : int32(next()%3));
                            } else if(nm=="SetPedPosition"){ ret=callBool(p0); }
                            else if(nm.ends_with("CB")){
                                if(!align&&v)At<int32>(o,doorOff)=door4();
                                reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(cbAnim,o);
                            } else if(nm=="FixHeading"){
                                // forward del peaton con producto controlado frente al lateral del coche
                                if(v&&next()%4)At<uint32>(v,V_TYPE)=0;
                                const CVector R=At<CVector>(vehMats[0].data(),0), Fw=At<CVector>(vehMats[0].data(),0x10);
                                const int32 d=At<int32>(o,doorOff); const float sg=d==10||d==11 ? -1.f : 1.f;
                                const uint32 m=next()%8; float k;
                                switch(m){ case 0: k=0.f; break; case 1: k=0.1f; break; case 2: k=std::nextafter(0.1f,0.f); break; case 3: k=std::nextafter(0.f,1.f); break;
                                           case 4: k=-uni(0.f,0.1f); break; default: k=uni(0.f,0.12f); }
                                const float s=std::sqrt(1.f-k*k);
                                At<CVector>(pedMats[0].data(),0x10)=next()%10==0 ? CVector{qnan,uni(-1.f,1.f),0.f} : CVector{R.x*sg*k+Fw.x*s,R.y*sg*k+Fw.y*s,R.z*sg*k+Fw.z*s};
                                reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            } else if(nm=="ComputeAnimID"){
                                At<CVehicle*>(o,0x10)=v0; At<int32>(o,doorOff)=next()%4 ? door4() : anyDoor();
                                for(size_t b=0;b<8;++b)outBuf[b]=uint8(next()); At<int32>(outBuf.data(),4)=int32(next()%400);
                                reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(target)(o,outBuf.data(),outBuf.data()+4);
                            } else if(nm=="StartAnim"){
                                At<CVehicle*>(o,0x10)=v0; if(!align)At<int32>(o,doorOff)=door4(); else doorHeight();
                                reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            } else if(nm=="ProcessPed"){
                                if(align){
                                    if(next()%3==0){ At<void*>(o,0xC)=nullptr; o[8]=0; if(next()%4)At<CVehicle*>(o,0x10)=v0; }
                                    if(At<void*>(o,0xC)==nullptr){ doorHeight(); if(next()%2)At<uint32>(v0,V_TYPE)=0; }
                                } else {
                                    if(next()%3==0){ At<void*>(o,0xC)=nullptr; o[8]=0; if(next()%4)At<CVehicle*>(o,0x10)=v0; }
                                    // ComputeAnimID (puerta no cerrada tras entrar o StartAnim) necesita una puerta 8..11
                                    const bool dontClose=At<uint32>(reinterpret_cast<void*>(ANIMGROUPS+handling[H_ANIMGROUP]*ANIMGROUP_SIZE),8)&2u;
                                    if(dontClose||!At<void*>(o,0xC))At<int32>(o,doorOff)=door4();
                                }
                                ret=callBool(p0);
                            }
                            for(auto* h:hooks)h->State(false);
                            out["object"]=Region(o,size,nm=="Destructor");   // tras destruir: vptr del EXE o de la DLL
                        }
                        out["refs"]=AllRefs(); out["out"]=Region(outBuf.data(),8);
                        json aw=json::array(); for(auto& a:anims)aw.push_back(Region(a.data(),a.size())); out["anims"]=aw;
                        out["p0"]=EntHash(peds[0].data(),peds[0].size()); out["p0_rot"]=json::array({At<uint32>(p0,P_CURROT),At<uint32>(p0,P_AIMROT)});
                        out["veh"]=EntHash(vehs[0].data(),vehs[0].size()); out["lineup"]=Region(lineup.data(),lineup.size());
                    }
                    for(auto* h:hooks)h->State(false);
                    out["trace"]=TraceJson(); out["lost_records"]=lostRecs;
                    // limpieza: tareas nuevas alcanzables (raices) y referencias; despues el pool debe quedar como antes
                    const uint32 before=nrec;
                    for(auto* t:roots)delete t;
                    out["delete_trace"]=TraceJson(before); nrec=before;
                    CleanRefs();
                    out["ret"]=ret; out["clean"]=Occupancy()==occ;
                    out["pedgroups"]=Hash(reinterpret_cast<void*>(PEDGROUPS),8*PEDGROUP_SIZE); out["timer"]=At<uint32>(reinterpret_cast<void*>(TIMER),0);
                    restoreGlobals();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["lost_records"]==0;
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
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    if(restored)std::memcpy(PoolStates(),statesStart.data(),statesStart.size());
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
