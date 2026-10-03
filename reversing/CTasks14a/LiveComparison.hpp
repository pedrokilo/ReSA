#pragma once
// Banco dentro del motor: entrar en el coche y deambular (lista de 14, 27 funciones).
// CTaskComplexEnterCar (constructor, destructor, MakeAbortable, CreateNextSubTask, CreateFirstSubTask, ControlSubTask,
// CreateNextSubTask_AfterSimpleCarAlign, CreateSubTask, CreateTaskUtilityLineUpPedWithCar, GetTargetPos,
// GetCameraAvoidVehicle, PreparePedForVehicleEnter, PrepareVehicleForPedEnter, SetVehicleFlags) y
// CTaskComplexWander (constructor, ControlSubTask, CreateFirstSubTask, UpdateDir, UpdatePathNodes, CreateSubTask,
// ComputeTargetPos, ComputeTargetHeading, ValidNodes, ScanForBlockedNodes, ScanForBlockedNode x2, GetWanderTaskByPedType).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 6 peatones con vtable falsa (SetMoveAnim trazado; el 4 y el 5 hacen de jugadores 0 y 1), 2 vehiculos con vtable
// falsa (IsDoorMissingU32 trazado) y datos de manejo propios, tareas falsas, objeto bajo prueba con vtable falsa
// (CreateFirstSubTask, CreateNextSubTask_AfterSimpleCarAlign, ScanForStuff, UpdateDir y UpdatePathNodes trazados para
// aislar cada funcion), CPedGroup falso y nodos de camino propios (ThePaths.m_pPathNodes[70/71] se cambian y restauran).
// Se simulan y trazan: RNG, CCarEnterExit (salud, puertas, gateo, puertas cercanas, posicion para abrir...),
// CPedGroups/CPedGroupMembership/CPedGroupIntelligence::AddEvent, CEventGroup::Add, AddTaskPrimaryMaybeInGroup,
// IsFriendlyWith, GetTaskDuck, SetMoveState, SetRealMoveAnim, SetLastTargetVehicle, CHud::SetHelpMessage, CText::Get,
// PositionPedOutOfCollision, IKChainManager::LookAt, CPad::GetAccelerate/GetBrake, StandStill/SetPedOut::ProcessPed,
// CloseDoorFromOutside::MakeAbortable, TakeWidthIntoAccountForWandering, FindNextNodeWandering, escaneres de peatones y
// vehiculos y CPed::Say. FindPlayerPed devuelve CWorld::Players[...].m_pPed sin trazar.
// Pools de tareas y eventos: m_LastFreeSlot y estados restaurados y slots libres con 0xCD en cada ejecucion; lo nuevo
// se serializa y se libera sin destructor. La utilidad de alineacion nueva (montón del EXE) se serializa y se borra.
// Constructores: buffer con 0xCD comparado entero salvo el vptr (clases abstractas; el DLL exporta su propia vtable).
// Fuera del banco: la rama FIX_BUGS de CreateTaskUtilityLineUpPedWithCar (utilidad ya existente: el EXE la pierde y
// reserva otra; el DLL la reutiliza) -> los casos que pueden crear la utilidad empiezan siempre sin ella.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskTypes/TaskComplexEnterCar.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedOut.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleCarCloseDoorFromOutside.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "PedScanner.h"
#include "VehicleScanner.h"
#include "Events/EventGroup.h"
#include "CarEnterExit.h"
#include "PathFind.h"
#include "Hud.h"
#include "Pad.h"
#include "PlayerInfo.h"
#include "GameLogic.h"
#include "Text/Text.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks14aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }
inline json Node(const CNodeAddress& n) { return json::array({n.m_wAreaId,n.m_wNodeId}); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=6, NVEH=2, FAKES=10;
// tareas falsas: campos de control por encima de 0x60 (GoToCarDoor usa hasta +0x49, GoToPoint hasta +0x24)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_TYPE=0x68, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_FLAGS=0x1C, E_SEED=0x20, E_MODEL=0x22, E_TYPE=0x36;                 // CEntity
constexpr size_t PH_FLAGS=0x40, PH_SPEED=0x44;                                                                  // CPhysical
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PDATA=0x480, P_STATE=0x530, P_HEALTH=0x540, P_VEH=0x58C, P_PEDTYPE=0x598; // CPed
constexpr size_t V_HANDLING=0x384, V_CRUISE=0x3D0, V_BUS=0x429, V_DRIVER=0x460, V_PASS=0x464, V_NUMIN=0x485, V_INFLAGS=0x486,
                 V_MAXPASS=0x488, V_FIRE=0x490, V_TYPE=0x590, V_SUBTYPE=0x594, V_DAMAGE=0x5A0, V_TRAIN=0x5B9, V_BIKE=0x614; // CVehicle
constexpr size_t H_FLAGS=0xCC, H_ANIM=0xDE, HND_SIZE=0x100;                                                    // tHandlingData
constexpr size_t I_STATIC=0x274;                                                                                // CPedIntelligence
constexpr size_t PEDGROUP_SIZE=0x2D4, NODE_SIZE=0x1C, NNODES=4;
constexpr uint16 A0=70, A1=71;                                                                                  // areas de nodos propias
constexpr uintptr_t TIMER=0xB7CB84, FRAMES=0xB7CB4C, SKIP=0x96A8A0, HELPFLAG=0xC0BC15;
constexpr uintptr_t VT_ENTER=0x86E6F0, VT_WANDER=0x86FE84;
constexpr uint32 IN_VEH=0x100u, STANDING=0x1u, DONT_DRAG=0x20000000u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,HND_SIZE>,NVEH> hnds{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> object{};
alignas(16) inline std::array<uint8,PEDGROUP_SIZE> group{};
alignas(16) inline std::array<uint8,0x40> playerData{};
alignas(16) inline std::array<uint8,0x40> event{};
alignas(16) inline std::array<uint8,0x40> vecBuf{};
alignas(16) inline std::array<uint8,0x20> lineupBuf{};
alignas(16) inline std::array<uint8,0x10> fireBuf{};
alignas(16) inline std::array<uint8,0x20> textBuf{};
alignas(16) inline std::array<uint8,NODE_SIZE*NNODES> nodes{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pools (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80, EVENT_SLOT=0x44;
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* EventHdr() { return reinterpret_cast<uint8*>(GetEventPool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline std::vector<bool> Occ(uint8* h) { std::vector<bool> r(size_t(Cap(h))); for(size_t k=0;k<r.size();++k)r[k]=!(States(h)[k]&0x80); return r; }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart) {
    Last(h)=last;
    if(Occ(h)==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}
inline void FreeFresh(uint8* h,const std::vector<bool>& before) { for(size_t k=0;k<before.size();++k)if(!before[k]&&!(States(h)[k]&0x80))States(h)[k]|=0x80; }

inline std::array<void*,16> fakeVtbl{}, objVtbl{};
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t heapLineup{}, newLineup{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(objVtbl.data()))return "obj_vt";
    if(v==uintptr_t(pedVtbl.data()))return "ped_vt"; if(v==uintptr_t(vehVtbl.data()))return "veh_vt";
    if(heapLineup&&v==heapLineup)return "lineup_heap"; if(newLineup&&v==newLineup)return "lineup_new";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(pedMats[n].data(),pedMats[n].size(),"pmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NVEH;++n){ if(auto s=in(vehs[n].data(),vehs[n].size(),"veh"))return *s+"#"+std::to_string(n); if(auto s=in(vehMats[n].data(),vehMats[n].size(),"vmat"))return *s+"#"+std::to_string(n);
                              if(auto s=in(hnds[n].data(),hnds[n].size(),"hnd"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(object.data(),object.size(),"obj"))return *s; if(auto s=in(group.data(),group.size(),"group"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"pdata"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(vecBuf.data(),vecBuf.size(),"vec"))return *s; if(auto s=in(lineupBuf.data(),lineupBuf.size(),"lineup"))return *s;
    if(auto s=in(fireBuf.data(),fireBuf.size(),"fire"))return *s; if(auto s=in(textBuf.data(),textBuf.size(),"text"))return *s;
    if(auto s=in(nodes.data(),nodes.size(),"nodes"))return *s;
    { const auto es=uintptr_t(Storage(EventHdr())); if(v>=es&&v<es+size_t(Cap(EventHdr()))*EVENT_SLOT)return "evpool+"+std::to_string((v-es)%EVENT_SLOT); }
    if(SlotIndex(TaskHdr(),TASK_SLOT,v)>=0)return "pool_task";
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    // locales de la pila (eventos, nodos y vectores temporales): no comparables
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline json SerTask(const void* p) {
    if(!p)return nullptr;
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}
// eventos reales (de la pila o del pool): tipo por la vtable y solo los campos (sin relleno)
inline json SerEvent(const CEvent* e) {
    if(!e)return nullptr;
    const int type=const_cast<CEvent*>(e)->GetEventType();
    json r={type,At<int32>(e,4),At<uint8>(e,8)};
    auto editable=[&]{ r.push_back(At<uint8>(e,0xC)); r.push_back(At<int16>(e,0xE)); r.push_back(At<int16>(e,0x10)); };
    switch(type){
    case EVENT_GROUP_EVENT: r.push_back(Name(At<uint32>(e,0xC))); r.push_back(SerEvent(At<const CEvent*>(e,0x10))); break;
    case EVENT_DRAGGED_OUT_CAR: editable(); r.push_back(Name(At<uint32>(e,0x14))); r.push_back(Name(At<uint32>(e,0x18))); r.push_back(At<uint8>(e,0x1C)); break;
    case EVENT_COP_CAR_BEING_STOLEN: r.push_back(Name(At<uint32>(e,0xC))); r.push_back(Name(At<uint32>(e,0x10))); break;
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER: editable(); r.push_back(Name(At<uint32>(e,0x14))); break;
    case EVENT_LEADER_EXITED_CAR_AS_DRIVER: editable(); break;
    default: break;
    }
    return r;
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
struct Buf { std::string name; uint8* p; size_t n; };
inline std::vector<Buf> Buffers() {
    std::vector<Buf> b;
    for(int n=0;n<NPEDS;++n){ b.push_back({"ped#"+std::to_string(n),peds[n].data(),peds[n].size()}); b.push_back({"pmat#"+std::to_string(n),pedMats[n].data(),pedMats[n].size()});
                              b.push_back({"intel#"+std::to_string(n),intels[n].data(),intels[n].size()}); }
    for(int n=0;n<NVEH;++n){ b.push_back({"veh#"+std::to_string(n),vehs[n].data(),vehs[n].size()}); b.push_back({"vmat#"+std::to_string(n),vehMats[n].data(),vehMats[n].size()});
                              b.push_back({"hnd#"+std::to_string(n),hnds[n].data(),hnds[n].size()}); }
    for(int n=0;n<FAKES;++n)b.push_back({"fk#"+std::to_string(n),fakes[n].data(),fakes[n].size()});
    b.push_back({"group",group.data(),group.size()}); b.push_back({"pdata",playerData.data(),playerData.size()});
    b.push_back({"event",event.data(),event.size()}); b.push_back({"vec",vecBuf.data(),vecBuf.size()});
    b.push_back({"lineup",lineupBuf.data(),lineupBuf.size()}); b.push_back({"nodes",nodes.data(),nodes.size()});
    return b;
}
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:Buffers())s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); const auto bufs=Buffers();
    for(size_t k=0;k<bufs.size();++k)for(size_t w=0;w+4<=bufs[k].n;w+=4)
        if(std::memcmp(bufs[k].p+w,before[k].data()+w,4))r.push_back({bufs[k].name,w,Name(At<uint32>(bufs[k].p,w))});
    return r;
}

// dependencias simuladas: respuestas booleanas por mascara (un bit por llamada) y valores por caso
enum BS { B_VH, B_PH, B_READY, B_PARTIAL, B_INUSE, B_SLOW, B_CLOSE, B_OPEN, B_OPENABLE, B_QUICK, B_QUIT, B_NEAREST, B_GLEADER,
          B_SAME, B_ISLEADER, B_FRIEND, B_DUCK, B_MISSING, B_UPDOUT, B_DIRCHG, NB };
inline std::array<uint32,NB> bmask{}, bcnt{};
inline bool B(int k) { return (bmask[k]>>(bcnt[k]++&31))&1; }
inline uint32 seed{}, rngCalls{}, idxCalls{}, flagCalls{}, twCalls{}, nextCalls{}, upCalls{};
inline uintptr_t slowJackedRet{}, pedsGroupRet{}, closestPedRet{}, closestVehRet{};
inline std::array<int32,3> passIdx{}; inline int32 exitDoor{}, nearestDoor{}; inline std::array<int32,2> doorFlags{};
inline CVector posOpen{}, nearestPos{}; inline int16 accel{}, brake{};
inline std::array<CVector,4> twRet{}; inline std::array<CNodeAddress,4> nextNodes{}, upNodes{};

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FSub(CTask* t,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    trace.push_back({"make_abortable_fake",Name(t),Name(p),prio,Name(e),r}); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// objeto bajo prueba: virtuales internas trazadas (cada funcion se prueba por separado)
inline CTask* __fastcall OGetSub(CTask* t,void*) { return At<CTask*>(t,8); }
inline int __fastcall OType(CTask* t,void*) { trace.push_back({"type_obj"}); return TASK_COMPLEX_ENTER_CAR_AS_DRIVER; }
inline CTask* __fastcall OFirst(void* o,void*,CPed* p) { trace.push_back({"create_first",Name(p)}); return Fake(9); }
inline CTask* __fastcall OAfterAlign(void* o,void*,CPed* p) { trace.push_back({"after_align",Name(p)}); return Fake(8); }
inline void __fastcall OScanForStuff(void* o,void*,CPed* p) { trace.push_back({"scan_for_stuff",Name(p)}); }
inline void __fastcall OUpdateDir(void* o,void*,CPed* p) {
    trace.push_back({"update_dir",Name(p),At<uint8>(o,0x10)}); if(B(B_DIRCHG))At<uint8>(o,0x10)=uint8((At<uint8>(o,0x10)+3)&7);
}
inline void __fastcall OUpdatePathNodes(void* o,void*,CPed* p,uint32 dir,CNodeAddress* org,CNodeAddress* tgt,uint8* out) {
    trace.push_back({"update_path_nodes",Name(p),dir&0xFF,Name(org),Node(*org),Name(tgt),Node(*tgt),Name(out)});
    *org=*tgt; *tgt=upNodes[upCalls++%upNodes.size()]; if(B(B_UPDOUT))*out=uint8((dir+5)&0xFF);
}
// vehiculos y peatones falsos
inline bool __fastcall VDoorMissing(CVehicle* v,void*,uint32 d) { const bool r=B(B_MISSING); trace.push_back({"door_missing",Name(v),d,r}); return r; }
inline void __fastcall PSetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(p)}); }

// CCarEnterExit
inline bool __cdecl FVehHealthy(const CVehicle* v) { const bool r=B(B_VH); trace.push_back({"veh_healthy",Name(v),r}); return r; }
inline bool __cdecl FPedHealthy(CPed* p) { const bool r=B(B_PH); trace.push_back({"ped_healthy",Name(p),r}); return r; }
inline bool __cdecl FDoorReady(const CVehicle* v,int32 d) { const bool r=B(B_READY); trace.push_back({"door_ready",Name(v),d,r}); return r; }
inline bool __cdecl FPartial(const CVehicle* v,int32 d) { const bool r=B(B_PARTIAL); trace.push_back({"partially_open",Name(v),d,r}); return r; }
inline bool __cdecl FInUse(const CVehicle* v,int32 a,int32 b) { const bool r=B(B_INUSE); trace.push_back({"door_in_use",Name(v),a,b,r}); return r; }
inline bool __cdecl FSlowJack(const CVehicle* v,int32 d) { const bool r=B(B_SLOW); trace.push_back({"slow_jack_required",Name(v),d,r}); return r; }
inline CPed* __cdecl FSlowJacked(const CVehicle* v,int32 d) { trace.push_back({"slow_jacked_ped",Name(v),d,Name(slowJackedRet)}); return reinterpret_cast<CPed*>(slowJackedRet); }
inline bool __cdecl FHasClose(const CVehicle* v,int32 d) { const bool r=B(B_CLOSE); trace.push_back({"door_to_close",Name(v),d,r}); return r; }
inline bool __cdecl FHasOpen(const CVehicle* v,int32 d) { const bool r=B(B_OPEN); trace.push_back({"door_to_open",Name(v),d,r}); return r; }
inline bool __cdecl FOpenable(const CVehicle* v,int32 d,const CPed* p) { const bool r=B(B_OPENABLE); trace.push_back({"openable_door",Name(v),d,Name(p),r}); return r; }
inline int32 __cdecl FPassIdx(const CVehicle* v,int32 d) { const int32 r=passIdx[idxCalls++%passIdx.size()]; trace.push_back({"passenger_index",Name(v),d,r}); return r; }
inline void __cdecl FUndragged(const CVehicle* v,const CPed* a,const CPed* b) { trace.push_back({"undragged_leave",Name(v),Name(a),Name(b)}); }
inline int32 __cdecl FExitDoor(const CVehicle* v,const CPed* p) { trace.push_back({"door_to_exit",Name(v),Name(p),exitDoor}); return exitDoor; }
inline bool __cdecl FQuickJack(CVehicle* v,int32 d,const CPed* p) { const bool r=B(B_QUICK); trace.push_back({"quick_jack",Name(v),d,Name(p),r}); return r; }
inline int32 __cdecl FDoorFlag(const CVehicle* v,int32 d,uint32 chk) { const int32 r=doorFlags[flagCalls++%doorFlags.size()]; trace.push_back({"door_flag",Name(v),d,chk&0xFF,r}); return r; }
inline CVector* __cdecl FPosOpen(CVector* out,const CVehicle* v,int32 d) { trace.push_back({"pos_open_door",Name(v),d}); *out=posOpen; return out; }
inline bool __cdecl FQuit(const CPed* p,const CVehicle* v,int32 t,CTask* task) { const bool r=B(B_QUIT); trace.push_back({"quit_enter",Name(p),Name(v),t,Name(task),r}); return r; }
inline bool __cdecl FNearest(const CPed* p,const CVehicle* v,CVector* out,int32* door) {
    const bool r=B(B_NEAREST); trace.push_back({"nearest_door",Name(p),Name(v),Name(out),Name(door),r}); if(r){ *out=nearestPos; *door=nearestDoor; } return r;
}
// grupos, eventos e inteligencia
inline CPedGroup* __cdecl FPedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(p),Name(pedsGroupRet)}); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline bool __cdecl FIsGroupLeader(CPed* p) { const bool r=B(B_GLEADER); trace.push_back({"is_group_leader",Name(p),r}); return r; }
inline bool __cdecl FSameGroup(const CPed* a,const CPed* b) { const bool r=B(B_SAME); trace.push_back({"same_group",Name(a),Name(b),r}); return r; }
inline bool __fastcall FIsLeader(void* m,void*,const CPed* p) { const bool r=B(B_ISLEADER); trace.push_back({"is_leader",Name(m),Name(p),r}); return r; }
inline bool __fastcall FAddGroupEvent(void* i,void*,CEvent* e) { trace.push_back({"group_add_event",Name(i),SerEvent(e)}); return true; }
inline CEvent* __fastcall FEventAdd(void* g,void*,CEvent* e,uint32 valid) { trace.push_back({"event_group_add",Name(g),SerEvent(e),valid&0xFF}); return e; }
inline void __fastcall FAddTask(void* i,void*,CTask* t,uint32 affects) { trace.push_back({"add_task_primary",Name(i),SerTask(t),affects&0xFF}); }
inline bool __fastcall FFriendly(void* i,void*,const CPed* p) { const bool r=B(B_FRIEND); trace.push_back({"friendly",Name(i),Name(p),r}); return r; }
// GetTaskDuck se pide dos veces seguidas (0x63AC96/0x63ACA7) y el EXE desreferencia la segunda sin comprobar: respuesta fija por caso
inline CTask* __fastcall FDuck(void* i,void*,uint32 ignore) { const bool r=bmask[B_DUCK]&1; trace.push_back({"task_duck",Name(i),ignore&0xFF,r}); return r ? Fake(7) : nullptr; }
// peatones, jugador, HUD, texto, IK, mando
inline void __fastcall FSetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(p),s}); }
inline void __fastcall FRealMoveAnim(CPed* p,void*) { trace.push_back({"set_real_move_anim",Name(p)}); }
inline void __fastcall FLastTarget(void* info,void*,CVehicle* v) { trace.push_back({"set_last_target_vehicle",Name(info),Name(v)}); }
inline CPlayerPed* __cdecl FFindPlayerPed(int32 id) { return CWorld::Players[id<0 ? CWorld::PlayerInFocus : id].m_pPed; }
inline void __cdecl FHelp(const GxtChar* t,uint32 q,uint32 perm,uint32 brief) { trace.push_back({"help_message",Name(t),q&0xFF,perm&0xFF,brief&0xFF}); }
inline const GxtChar* __fastcall FTextGet(void* txt,void*,const char* key) {
    std::string s; for(int k=0;k<16&&key&&key[k];++k)s+=key[k]; trace.push_back({"text_get",s}); return reinterpret_cast<const GxtChar*>(textBuf.data());
}
inline void __cdecl FPosOut(CPed* p,CVehicle* v,int32 d) { trace.push_back({"position_out_of_collision",Name(p),Name(v),d}); }
inline void __fastcall FLookAt(void* m,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    std::string s; for(int k=0;k<32&&purpose&&purpose[k];++k)s+=purpose[k];
    trace.push_back({"look_at",s,Name(p),Name(e),time,bone,Name(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline int16 __fastcall FAccel(void* pad,void*) { trace.push_back({"accelerate",Name(pad)}); return accel; }
inline int16 __fastcall FBrake(void* pad,void*) { trace.push_back({"brake",Name(pad)}); return brake; }
inline bool __fastcall FStandProcess(void* t,void*,CPed* p) { trace.push_back({"stand_still_process",Name(t),Name(p)}); return false; }
inline bool __fastcall FSetPedOutProcess(void* t,void*,CPed* p) { trace.push_back({"set_ped_out_process",Name(t),Name(p)}); return false; }
inline bool __fastcall FCloseAbort(void* t,void*,CPed* p,int32 prio,const CEvent* e) { trace.push_back({"close_door_abortable",Name(t),Name(p),prio,Name(e)}); return true; }
inline int16 __fastcall FSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 overrideSilence,uint32 forceAudible,uint32 frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence&0xFF,forceAudible&0xFF,frontEnd&0xFF}); return -1;
}
// caminos y escaneres
inline CVector* __fastcall FTakeWidth(void* paths,void*,CVector* out,uint32 node,uint32 rs) {
    trace.push_back({"take_width",node,rs&0xFFFF}); *out=twRet[twCalls++%twRet.size()]; return out;
}
inline void __fastcall FFindNext(void* paths,void*,uint32 type,float x,float y,float z,CNodeAddress* o,CNodeAddress* t,uint32 dir,uint8* out) {
    trace.push_back({"find_next_node_wandering",type&0xFF,F(x),F(y),F(z),Name(o),Node(*o),Name(t),Node(*t),dir&0xFF,Name(out)});
    *t=nextNodes[nextCalls++%nextNodes.size()]; if(B(B_UPDOUT))*out=uint8((dir+2)&7);
}
inline CPed* __fastcall FClosestPed(void* s,void*) { trace.push_back({"closest_ped",Name(s),Name(closestPedRet)}); return reinterpret_cast<CPed*>(closestPedRet); }
inline CVehicle* __fastcall FClosestVeh(void* s,void*) { trace.push_back({"closest_vehicle",Name(s),Name(closestVehRet)}); return reinterpret_cast<CVehicle*>(closestVehRet); }
// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=27;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexEnterCar","Constructor",0x63A220,0,-1,256},
    {"CTaskComplexEnterCar","ControlSubTask",0,0x86E6F0,10,1024},
    {"CTaskComplexEnterCar","CreateFirstSubTask",0,0x86E6F0,9,512},
    {"CTaskComplexEnterCar","CreateNextSubTask",0,0x86E6F0,8,1024},
    {"CTaskComplexEnterCar","CreateNextSubTask_AfterSimpleCarAlign",0,0x86E6F0,11,1024},
    {"CTaskComplexEnterCar","CreateSubTask",0x63E040,0,-1,512},
    {"CTaskComplexEnterCar","CreateTaskUtilityLineUpPedWithCar",0x63ACC0,0,-1,128},
    {"CTaskComplexEnterCar","Destructor",0x63DFA0,0,-1,256},
    {"CTaskComplexEnterCar","GetCameraAvoidVehicle",0x63A690,0,-1,256},
    {"CTaskComplexEnterCar","GetTargetPos",0x63A300,0,-1,256},
    {"CTaskComplexEnterCar","MakeAbortable",0,0x86E6F0,6,512},
    {"CTaskComplexEnterCar","PreparePedForVehicleEnter",0x63AC80,0,-1,128},
    {"CTaskComplexEnterCar","PrepareVehicleForPedEnter",0x63AC10,0,-1,256},
    {"CTaskComplexEnterCar","SetVehicleFlags",0x63AB90,0,-1,256},
    {"CTaskComplexWander","ComputeTargetHeading",0x66F530,0,-1,256},
    {"CTaskComplexWander","ComputeTargetPos",0x669F60,0,-1,128},
    {"CTaskComplexWander","Constructor",0x66F450,0,-1,128},
    {"CTaskComplexWander","ControlSubTask",0,0x86FE84,10,1024},
    {"CTaskComplexWander","CreateFirstSubTask",0,0x86FE84,9,256},
    {"CTaskComplexWander","CreateSubTask",0x671CB0,0,-1,256},
    {"CTaskComplexWander","GetWanderTaskByPedType",0x673D00,0,-1,256},
    {"CTaskComplexWander","ScanForBlockedNode-",0x671EF0,0,-1,512},
    {"CTaskComplexWander","ScanForBlockedNode-1",0x66F4C0,0,-1,512},
    {"CTaskComplexWander","ScanForBlockedNodes",0x674560,0,-1,512},
    {"CTaskComplexWander","UpdateDir",0,0x86FE84,13,1024},
    {"CTaskComplexWander","UpdatePathNodes",0,0x86FE84,14,256},
    {"CTaskComplexWander","ValidNodes",0x669F30,0,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks14a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks14a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks14a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks14a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t dllStand=OurVirtual("CTaskSimpleStandStill","ProcessPed"), dllSetPedOut=OurVirtual("CTaskSimpleCarSetPedOut","ProcessPed"),
                    dllCloseAbort=OurVirtual("CTaskSimpleCarCloseDoorFromOutside","MakeAbortable");
    if(!dllStand||!dllSetPedOut||!dllCloseAbort){Write({{"event","error"},{"reason","virtual hook not found"}});return;}
    // modelo de vehiculo con colision cargada (ScanForBlockedNode lee su radio)
    int16 vehModel=-1; for(int32 m=400;m<612&&vehModel<0;++m)if(auto* mi=CModelInfo::GetModelInfo(m);mi&&mi->GetColModel())vehModel=int16(m);
    if(vehModel<0){Write({{"event","error"},{"reason","no vehicle model with collision"}});return;}
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(objVtbl,std::make_integer_sequence<int,16>{});
    FillTraps(pedVtbl,std::make_integer_sequence<int,64>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    objVtbl[2]=reinterpret_cast<void*>(&OGetSub); objVtbl[3]=reinterpret_cast<void*>(&FSimple); objVtbl[4]=reinterpret_cast<void*>(&OType);
    objVtbl[9]=reinterpret_cast<void*>(&OFirst);
    pedVtbl[23]=reinterpret_cast<void*>(&PSetMoveAnim); vehVtbl[37]=reinterpret_cast<void*>(&VDoorMissing);
    const auto occTaskStart=Occ(TaskHdr()), occEvStart=Occ(EventHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr()), le0=Last(EventHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    const std::vector<uint8> stEv(States(EventHdr()),States(EventHdr())+Cap(EventHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); ResetPool(EventHdr(),EVENT_SLOT,le0,stEv,occEvStart); };
    // estado global tocado: reloj, contador de fotogramas, SkipState, ayuda mostrada, jugadores 0 y 1 y dos areas de nodos
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{FRAMES,4},{SKIP,4},{HELPFLAG,1},
        {uintptr_t(&CWorld::Players[0].m_pPed),4},{uintptr_t(&CWorld::Players[1].m_pPed),4},
        {uintptr_t(&ThePaths.m_pPathNodes[A0]),4},{uintptr_t(&ThePaths.m_pPathNodes[A1]),4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const uint8 focus=CWorld::PlayerInFocus;
    // CDamageManager::GetDoorStatus_Component (0x6C2250) devuelve en el EXE el byte extendido (movzx) y los llamadores del EXE
    // comparan eax entero; la version del DLL devuelve un uint8 con basura en los bits altos. Durante el banco se usa el
    // codigo del EXE para que el original no dependa de ese hook (el candidato llama al DLL directamente).
    auto* doorStatusHook=FindHook("CDamageManager","GetDoorStatus_Component");
    if(!doorStatusHook){Write({{"event","error"},{"reason","hook not found"},{"function","CDamageManager::GetDoorStatus_Component"}});return;}
    const bool doorStatusEnabled=doorStatusHook->Hooked(), doorStatusLocked=doorStatusHook->Locked();
    doorStatusHook->LockState(false); doorStatusHook->State(false);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FFindPlayerPed), p1e(0x56E210,FFindPlayerPed);
        // CCarEnterExit
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsVehicleHealthy)),FVehHealthy), c1e(0x64EEC0,FVehHealthy);
        EntryPatch c2(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsPedHealthy)),FPedHealthy), c2e(0x64EEE0,FPedHealthy);
        EntryPatch c3(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsCarDoorReady)),FDoorReady), c3e(0x64ED90,FDoorReady);
        EntryPatch c4(Body(bit_cast<uintptr_t>(&CCarEnterExit::CarHasPartiallyOpenDoor)),FPartial), c4e(0x64EE70,FPartial);
        EntryPatch c5(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsCarDoorInUse)),FInUse), c5e(0x64EC90,FInUse);
        EntryPatch c6(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsCarSlowJackRequired)),FSlowJack), c6e(0x64EF70,FSlowJack);
        EntryPatch c7(Body(bit_cast<uintptr_t>(&CCarEnterExit::ComputeSlowJackedPed)),FSlowJacked), c7e(0x64F070,FSlowJacked);
        EntryPatch c8(Body(bit_cast<uintptr_t>(&CCarEnterExit::CarHasDoorToClose)),FHasClose), c8e(0x64EE10,FHasClose);
        EntryPatch c9(Body(bit_cast<uintptr_t>(&CCarEnterExit::CarHasDoorToOpen)),FHasOpen), c9e(0x64EDD0,FHasOpen);
        EntryPatch c10(Body(bit_cast<uintptr_t>(&CCarEnterExit::CarHasOpenableDoor)),FOpenable), c10e(0x64EE50,FOpenable);
        EntryPatch c11(Body(bit_cast<uintptr_t>(&CCarEnterExit::ComputePassengerIndexFromCarDoor)),FPassIdx), c11e(0x64F1E0,FPassIdx);
        EntryPatch c12(Body(bit_cast<uintptr_t>(&CCarEnterExit::MakeUndraggedPassengerPedsLeaveCar)),FUndragged), c12e(0x64F540,FUndragged);
        EntryPatch c13(Body(bit_cast<uintptr_t>(&CCarEnterExit::ComputeTargetDoorToExit)),FExitDoor), c13e(0x64F110,FExitDoor);
        EntryPatch c14(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsCarQuickJackPossible)),FQuickJack), c14e(0x64EF00,FQuickJack);
        EntryPatch c15(Body(bit_cast<uintptr_t>(&CCarEnterExit::ComputeDoorFlag)),FDoorFlag), c15e(0x64E550,FDoorFlag);
        EntryPatch c16(Body(bit_cast<uintptr_t>(&CCarEnterExit::GetPositionToOpenCarDoor)),FPosOpen), c16e(0x64E740,FPosOpen);
        EntryPatch c17(Body(bit_cast<uintptr_t>(&CCarEnterExit::IsPlayerToQuitCarEnter)),FQuit), c17e(0x64F240,FQuit);
        EntryPatch c18(Body(bit_cast<uintptr_t>(&CCarEnterExit::GetNearestCarDoor)),FNearest), c18e(0x6528F0,FNearest);
        // grupos, eventos, inteligencia
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup)),FPedsGroup), g1e(0x5F7E80,FPedsGroup);
        EntryPatch g2(Body(bit_cast<uintptr_t>(&CPedGroups::IsGroupLeader)),FIsGroupLeader), g2e(0x5F7E40,FIsGroupLeader);
        EntryPatch g3(Body(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup)),FSameGroup), g3e(0x5F7F40,FSameGroup);
        EntryPatch g4(Body(MemberAddress(&CPedGroupMembership::IsLeader)),FIsLeader), g4e(0x5F69C0,FIsLeader);
        EntryPatch g5(Body(MemberAddress(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent))),FAddGroupEvent), g5e(0x5F7470,FAddGroupEvent);
        EntryPatch g6(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FEventAdd), g6e(0x4AB420,FEventAdd);
        EntryPatch i1(Body(MemberAddress(&CPedIntelligence::AddTaskPrimaryMaybeInGroup)),FAddTask), i1e(0x600E20,FAddTask);
        EntryPatch i2(Body(MemberAddress(&CPedIntelligence::IsFriendlyWith)),FFriendly), i2e(0x601BC0,FFriendly);
        EntryPatch i3(Body(MemberAddress(&CPedIntelligence::GetTaskDuck)),FDuck), i3e(0x6010A0,FDuck);
        // peatones, jugador, HUD, texto, IK, mando, tareas temporales
        EntryPatch s1(Body(MemberAddress(&CPed::SetMoveState)),FSetMoveState), s1e(0x5DEC00,FSetMoveState);
        EntryPatch s2(Body(MemberAddress(&CPlayerPed::SetRealMoveAnim)),FRealMoveAnim), s2e(0x60A9C0,FRealMoveAnim);
        EntryPatch s3(Body(MemberAddress(&CPlayerInfo::SetLastTargetVehicle)),FLastTarget), s3e(0x56DA80,FLastTarget);
        EntryPatch s4(Body(bit_cast<uintptr_t>(&CHud::SetHelpMessage)),FHelp), s4e(0x588BE0,FHelp);
        EntryPatch s5(Body(MemberAddress(&CText::Get)),FTextGet), s5e(0x6A0050,FTextGet);
        EntryPatch s6(Body(bit_cast<uintptr_t>(&CTaskSimpleCarSetPedOut::PositionPedOutOfCollision)),FPosOut), s6e(0x6479B0,FPosOut);
        EntryPatch s7(Body(MemberAddress(&IKChainManager_c::LookAt)),FLookAt), s7e(0x618970,FLookAt);
        EntryPatch s8(Body(MemberAddress(&CPad::GetAccelerate)),FAccel), s8e(0x5403F0,FAccel);
        EntryPatch s9(Body(MemberAddress(&CPad::GetBrake)),FBrake), s9e(0x540080,FBrake);
        EntryPatch s10(Body(dllStand),FStandProcess), s10e(0x62F370,FStandProcess);
        EntryPatch s11(Body(dllSetPedOut),FSetPedOutProcess), s11e(0x647D10,FSetPedOutProcess);
        EntryPatch s12(Body(dllCloseAbort),FCloseAbort), s12e(0x64B020,FCloseAbort);
        EntryPatch s13(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FSay), s13e(0x5EFFE0,FSay);
        // caminos y escaneres
        EntryPatch w1(Body(MemberAddress(&CPathFind::TakeWidthIntoAccountForWandering)),FTakeWidth), w1e(0x4509A0,FTakeWidth);
        EntryPatch w2(Body(MemberAddress(&CPathFind::FindNextNodeWandering)),FFindNext), w2e(0x451B70,FFindNext);
        EntryPatch w3(Body(MemberAddress(&CPedScanner::GetClosestPedInRange)),FClosestPed), w3e(0x5FFF20,FClosestPed);
        EntryPatch w4(Body(MemberAddress(&CVehicleScanner::GetClosestVehicleInRange)),FClosestVeh), w4e(0x5FFEE0,FClosestVeh);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool enter=cls=="CTaskComplexEnterCar";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x14A1u; if(!st)st=1; trace=json::array(); seed=uint32(i);
                    rngCalls=idxCalls=flagCalls=twCalls=nextCalls=upCalls=0; bcnt.fill(0); freshIds.clear(); heapLineup=newLineup=0;
                    CleanRefs(); restoreGlobals();
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=100000u+next()%1000000u;
                    At<uint32>(reinterpret_cast<void*>(FRAMES),0)=1000u+next()%100000u;
                    At<int32>(reinterpret_cast<void*>(SKIP),0)=next()%4==0 ? SKIP_IN_PROGRESS : SKIP_NONE;
                    At<uint8>(reinterpret_cast<void*>(HELPFLAG),0)=uint8(next()%2);
                    const uint32 now=At<uint32>(reinterpret_cast<void*>(TIMER),0), frame=At<uint32>(reinterpret_cast<void*>(FRAMES),0);
                    for(auto& m:bmask)m=next();
                    // peatones: vtable falsa, matriz (casi siempre), inteligencia, tipo, flags, vehiculo, salud, estado
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,P_PDATA)=playerData.data();
                        if(next()%6){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(0.f,4.f)}; }
                        else At<CVector>(p,E_POS)=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(0.f,4.f)};
                        At<uint32>(p,E_FLAGS)=next()%2 ? 0x1u : 0u; // bUsesCollision
                        At<uint16>(p,E_SEED)=uint16(next()); At<int16>(p,E_MODEL)=vehModel;
                        At<uint32>(p,PH_FLAGS)=next()%3==0 ? 0x100u : 0u; // bSubmergedInWater
                        { static const int32 types[]{0,1,4,5,6,7,18,20,22}; At<int32>(p,P_PEDTYPE)=n==4||n==5 ? 0 : types[next()%std::size(types)]; }
                        if(next()%3==0)At<uint32>(p,P_FLAGS)|=IN_VEH; if(next()%2)At<uint32>(p,P_FLAGS)|=STANDING; if(next()%3==0)At<uint32>(p,P_FLAGS)|=DONT_DRAG;
                        { const uint32 m=next()%4; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : m==3 ? Veh(1) : Veh(0); }
                        At<int32>(p,P_STATE)=next()%5==0 ? 54+int32(next()%2) : int32(next()%50);
                        { const uint32 m=next()%6; At<float>(p,P_HEALTH)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? qnan : uni(1.f,100.f); }
                        At<int32>(intels[n].data(),I_STATIC)=int32(next()%40);
                    }
                    std::memset(playerData.data(),0,playerData.size()); At<float>(playerData.data(),0x14)=uni(0.f,3.f);
                    CWorld::Players[0].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4)); CWorld::Players[1].m_pPed=next()%3==0 ? reinterpret_cast<CPlayerPed*>(Ped(5)) : nullptr;
                    if(!CWorld::Players[focus].m_pPed)CWorld::Players[focus].m_pPed=reinterpret_cast<CPlayerPed*>(Ped(4));
                    // vehiculos: vtable falsa, matriz, velocidad, tipo/subtipo, estado, ocupantes, manejo, daños, crucero
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size());
                        auto* h=hnds[n].data(); for(auto& b:hnds[n])b=uint8(next());
                        h[H_ANIM]=next()%2 ? uint8(ANIM_GROUP_CONVCARANIMS-ANIM_GROUP_STDCARAMIMS) : uint8(next()%20);
                        At<void*>(v,0)=vehVtbl.data(); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<void*>(v,V_HANDLING)=h;
                        v[E_TYPE]=uint8(2|(next()%3==0 ? 4u : uint32(next()%8))<<3);
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        { const uint32 m=next()%6; if(m==0)At<float>(vehMats[n].data(),8)=0.5f; else if(m==1)At<float>(vehMats[n].data(),8)=-0.5f; else if(m==2)At<float>(vehMats[n].data(),0x28)=0.f; }
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(0.f,4.f)};
                        { const uint32 m=next()%6; const float s=m==0 ? 0.1f : m==1 ? 0.2f : m==2 ? 0.f : m==3 ? 0.3f : uni(0.f,0.25f);
                          At<CVector>(v,PH_SPEED)=m<2 ? CVector{s,0.f,uni(-1.f,1.f)} : CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; }
                        { static const int32 ty[]{0,0,0,9,9,5,6,3}; At<int32>(v,V_TYPE)=ty[next()%std::size(ty)]; }
                        { static const int32 sty[]{0,0,2,3,4,9}; At<int32>(v,V_SUBTYPE)=sty[next()%std::size(sty)]; }
                        { static const int16 md[]{0,596,460,539,447,417,523}; const int16 mm=md[next()%std::size(md)]; At<int16>(v,E_MODEL)=mm&&enter ? mm : vehModel; } // deambular lee la colision del modelo
                        v[V_BUS]=uint8(next()%4==0 ? 2 : 0); v[V_CRUISE]=uint8(next()%3==0 ? 0 : next()%256);
                        v[V_NUMIN]=uint8(next()%4); v[V_INFLAGS]=uint8(next()); v[V_MAXPASS]=uint8(next()%4==0 ? 0 : 1+next()%3);
                        At<void*>(v,V_FIRE)=next()%6==0 ? fireBuf.data() : nullptr;
                        At<CPed*>(v,V_DRIVER)=next()%2 ? Ped(int(1+next()%3)) : nullptr;
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%4==0 ? Ped(int(1+next()%3)) : nullptr;
                        for(size_t b=0;b<0x18;++b)v[V_DAMAGE+b]=uint8(next()%5);
                        v[V_TRAIN]=uint8(next()); v[V_BIKE]=uint8(next()&~8u);
                    }
                    // tareas falsas
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        for(size_t w=0xC;w<0x24;w+=4)At<float>(k,w)=uni(-20.f,20.f);
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; }
                    // GoToCarDoor falso: +0x34 puerta, +0x38 punto, +0x49 puerta alcanzada
                    { static const int32 dr[]{0,8,9,10,11,18}; At<int32>(fakes[0].data(),0x34)=dr[next()%std::size(dr)]; fakes[0][0x49]=uint8(next()%3!=0); }
                    std::memset(group.data(),0,group.size()); std::memset(event.data(),0,event.size()); std::memset(vecBuf.data(),0,vecBuf.size());
                    for(auto& b:lineupBuf)b=uint8(next()); std::memset(fireBuf.data(),0,fireBuf.size()); std::memset(textBuf.data(),0,textBuf.size());
                    // nodos de camino: area A0 con 4 nodos (enlaces 0..15), area A1 sin cargar
                    for(auto& b:nodes)b=uint8(next()); for(size_t k=0;k<NNODES;++k)nodes[k*NODE_SIZE+0x18]=uint8(next()%5==0 ? next()%3 : 3+next()%13);
                    ThePaths.m_pPathNodes[A0]=reinterpret_cast<CPathNode*>(nodes.data()); ThePaths.m_pPathNodes[A1]=nullptr;
                    auto pickNode=[&]()->CNodeAddress{ const uint32 m=next()%8; if(m==0)return CNodeAddress{0xFFFF,uint16(next()%4)}; if(m==1)return CNodeAddress{A1,uint16(next()%4)};
                                                     if(m==2)return CNodeAddress{A0,0xFFFF}; return CNodeAddress{A0,uint16(next()%NNODES)}; };
                    // respuestas de las dependencias
                    { const uint32 m=next()%3; slowJackedRet=m==0 ? 0 : uintptr_t(Ped(int(1+next()%3))); }
                    { const uint32 m=next()%3; pedsGroupRet=m==0 ? 0 : uintptr_t(group.data()); }
                    for(auto& x:passIdx)x=next()%8==0 ? -1 : int32(next()%8);
                    exitDoor=int32(8+next()%4); nearestDoor=int32(8+next()%4); for(auto& x:doorFlags)x=int32(1u<<(next()%4)); if(next()%4==0)doorFlags[1]=doorFlags[0];
                    posOpen=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(0.f,4.f)}; nearestPos=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(0.f,4.f)};
                    { static const int16 pv[]{0,128,254,255,-1,300}; accel=pv[next()%std::size(pv)]; brake=pv[next()%std::size(pv)]; }
                    for(auto& n:nextNodes)n=pickNode(); for(auto& n:upNodes)n=pickNode();
                    closestPedRet=next()%3 ? uintptr_t(Ped(int(1+next()%3))) : 0; closestVehRet=next()%3 ? uintptr_t(Veh(int(next()%2))) : 0;

                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    std::memset(o,0xCD,object.size());
                    // posiciones que devuelve TakeWidthIntoAccountForWandering: cerca del peaton (dentro y fuera de 3)
                    { const CVector pp=p0->GetPosition(); for(auto& t:twRet){ const uint32 m=next()%4; t=m==0 ? CVector{pp.x+3.f,pp.y,pp.z} : CVector{pp.x+uni(-4.f,4.f),pp.y+uni(-4.f,4.f),pp.z+uni(-2.f,2.f)}; } }
                    auto prio=[&]{ return int32(next()%4); };
                    auto occT=Occ(TaskHdr()), occE=Occ(EventHdr());
                    uintptr_t fixtureLineup{};
                    auto setEnter=[&]{
                        std::memset(o,0,0x50); for(size_t b=0x2C;b<0x38;++b)o[b]=uint8(next());
                        At<void*>(o,0)=objVtbl.data(); objVtbl[11]=reinterpret_cast<void*>(&OAfterAlign);
                        At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr;
                        { const uint32 m=next()%8; At<CVehicle*>(o,0xC)=m==0 ? nullptr : m==1 ? Veh(1) : Veh(0); } Reg(At<CVehicle*>(o,0xC),o+0xC);
                        o[0x10]=uint8(next());
                        { static const int32 dr[]{0,8,8,9,10,10,11,18,18,19}; At<int32>(o,0x14)=dr[next()%std::size(dr)]; }
                        At<int32>(o,0x18)=next()%2 ? 0 : 10; At<int32>(o,0x1C)=int32(next()%4); At<int32>(o,0x20)=int32(next()%3000);
                        At<int32>(o,0x24)=int32(next()%8); o[0x28]=uint8(next()%2); o[0x29]=uint8(next()%4);
                        for(size_t w=0x2C;w<0x38;w+=4)At<float>(o,w)=uni(-10.f,10.f);
                        At<void*>(o,0x38)=next()%2 ? (void*)lineupBuf.data() : nullptr; o[0x3C]=uint8(next()%4==0);
                        At<CPed*>(o,0x40)=next()%2 ? Ped(int(1+next()%3)) : nullptr; Reg(At<CPed*>(o,0x40),o+0x40);
                        o[0x44]=uint8(next()%16);
                        { const uint32 m=next()%5; At<float>(o,0x48)=m==0 ? -1.f : m==1 ? 0.f : float(next()%256)+(m==2 ? 0.75f : 0.f); }
                        At<uint32>(o,0x4C)=now-next()%5000;
                    };
                    auto setWander=[&]{
                        std::memset(o,0,0x28); At<void*>(o,0)=objVtbl.data();
                        objVtbl[11]=reinterpret_cast<void*>(&FTrap<11>); objVtbl[12]=reinterpret_cast<void*>(&OScanForStuff);
                        objVtbl[13]=reinterpret_cast<void*>(&OUpdateDir); objVtbl[14]=reinterpret_cast<void*>(&OUpdatePathNodes);
                        At<CTask*>(o,8)=next()%8 ? Fake(0) : nullptr; At<int32>(o,0xC)=int32(next()%3==0 ? next()%8 : 4+next()%4);
                        o[0x10]=uint8(next()%5==0 ? 255 : next()%8); At<float>(o,0x14)=uni(0.f,2.f);
                        At<CNodeAddress>(o,0x18)=pickNode(); At<CNodeAddress>(o,0x1C)=next()%4==0 ? At<CNodeAddress>(o,0x18) : pickNode();
                        At<uint32>(o,0x20)=next()%3==0 ? frame : frame-1-next()%10; o[0x24]=uint8(next());
                    };
                    static const int32 enterSubTypes[]{TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL,TASK_SIMPLE_CAR_ALIGN,TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE,
                        TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE,TASK_SIMPLE_BIKE_PICK_UP,TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE,TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE,
                        TASK_SIMPLE_CAR_GET_IN,TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE,TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER,
                        TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,TASK_SIMPLE_CAR_SET_PED_OUT,TASK_SIMPLE_CAR_QUICK_DRAG_PED_OUT,TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT,
                        TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR,TASK_COMPLEX_FALL_AND_GET_UP,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_ENTER_BOAT_AS_DRIVER,
                        TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_SIMPLE_STAND_STILL,TASK_NONE,TASK_SIMPLE_PAUSE,TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN};

                    // argumentos preparados antes de la instantanea
                    int32 a0{}; uint32 b0{}, b1{}, b2{}, b3{}; float f0{}; void* q0{}; void* q1{}; const CEvent* ev{}; CPed* ped=p0;
                    if(enter) {
                        if(nm=="Constructor"){ q0=next()%4 ? (void*)Veh(int(next()%2)) : nullptr; b0=next()%2; b1=next()%2; b2=next()%2; b3=next()%2; }
                        else {
                            setEnter();
                            if(nm=="Destructor"){ if(next()%2){ heapLineup=uintptr_t(::operator new(0x1C)); std::memset(reinterpret_cast<void*>(heapLineup),0x5A,0x1C); At<uintptr_t>(o,0x38)=heapLineup; } else At<void*>(o,0x38)=nullptr; }
                            if(nm=="MakeAbortable"){ At<CTask*>(o,8)=Fake(0); a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr;
                                { static const int32 ty[]{TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%2]; } }
                            if(nm=="CreateNextSubTask"||nm=="ControlSubTask"||nm=="GetCameraAvoidVehicle"||nm=="GetTargetPos"){
                                At<int32>(fakes[0].data(),FK_TYPE)=enterSubTypes[next()%std::size(enterSubTypes)];
                                if(nm=="ControlSubTask"){ static const int32 ty[]{TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL,TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE,TASK_NONE,
                                    TASK_COMPLEX_ENTER_BOAT_AS_DRIVER,TASK_SIMPLE_CAR_GET_IN,TASK_SIMPLE_PAUSE}; At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; At<CTask*>(o,8)=Fake(0); }
                                if(nm=="CreateNextSubTask"){ At<CTask*>(o,8)=Fake(0); const int32 t=At<int32>(fakes[0].data(),FK_TYPE);
                                    // las ramas que crean la utilidad de alineacion empiezan sin ella (FIX_BUGS fuera del banco)
                                    if(t==TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL||t==TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE)At<void*>(o,0x38)=nullptr; }
                            }
                            if(nm=="CreateNextSubTask_AfterSimpleCarAlign"){
                                // bicis/quads con gateo lento: el EXE desreferencia el peaton arrastrado sin comprobar
                                auto* car=At<CVehicle*>(o,0xC); if(!car)At<CVehicle*>(o,0xC)=Veh(0),Reg(Veh(0),o+0xC);
                                car=At<CVehicle*>(o,0xC); if(At<int32>(car,V_TYPE)==9||At<int32>(car,V_SUBTYPE)==2)slowJackedRet=uintptr_t(Ped(int(1+next()%3)));
                            }
                            if(nm=="CreateSubTask"){
                                static const int32 ty[]{TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT,TASK_SIMPLE_WAIT_UNTIL_PED_OUT_CAR,TASK_FINISHED,TASK_FINISHED,TASK_SIMPLE_CAR_SET_PED_OUT,
                                    TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_OUTSIDE,TASK_SIMPLE_CAR_GET_IN,TASK_SIMPLE_CAR_SHUFFLE,TASK_SIMPLE_CAR_WAIT_FOR_DOOR_NOT_TO_BE_IN_USE,
                                    TASK_SIMPLE_CAR_SET_PED_IN_AS_PASSENGER,TASK_SIMPLE_CAR_SET_PED_IN_AS_DRIVER,TASK_SIMPLE_CAR_CLOSE_DOOR_FROM_INSIDE,
                                    TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL,TASK_SIMPLE_CAR_ALIGN,TASK_SIMPLE_CAR_OPEN_DOOR_FROM_OUTSIDE,TASK_SIMPLE_CAR_OPEN_LOCKED_DOOR_FROM_OUTSIDE,
                                    TASK_SIMPLE_BIKE_PICK_UP,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_LEAVE_CAR,TASK_COMPLEX_ENTER_BOAT_AS_DRIVER,TASK_COMPLEX_FALL_AND_GET_UP,
                                    TASK_COMPLEX_FALL_AND_GET_UP,TASK_NONE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_CAR_WAIT_TO_SLOW_DOWN};
                                a0=ty[next()%std::size(ty)];
                                // solo FINISHED, FALL_AND_GET_UP y SLOW_DRAG admiten peaton nulo; los demas necesitan el coche
                                const bool nullOk=a0==TASK_FINISHED||a0==TASK_COMPLEX_FALL_AND_GET_UP||a0==TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT;
                                if(nullOk&&next()%4==0)ped=nullptr; else if(!nullOk&&next()%3==0)ped=next()%2 ? Ped(4) : Ped(int(1+next()%3));
                                if(nullOk&&ped&&next()%3==0)ped=Ped(4);
                                if(!nullOk||a0==TASK_SIMPLE_CAR_SLOW_DRAG_PED_OUT){ if(!At<CVehicle*>(o,0xC)){ At<CVehicle*>(o,0xC)=Veh(0); Reg(Veh(0),o+0xC); } }
                                if(a0==TASK_COMPLEX_LEAVE_CAR&&ped&&!At<CVehicle*>(ped,P_VEH))At<CVehicle*>(ped,P_VEH)=Veh(1);
                            }
                            if(nm=="CreateTaskUtilityLineUpPedWithCar"){ At<void*>(o,0x38)=nullptr; if(!At<CVehicle*>(o,0xC)){ At<CVehicle*>(o,0xC)=Veh(0); Reg(Veh(0),o+0xC); } }
                            if(nm=="SetVehicleFlags"||nm=="PrepareVehicleForPedEnter"||nm=="ControlSubTask"||nm=="CreateFirstSubTask"){
                                if(nm!="ControlSubTask"&&nm!="CreateFirstSubTask"&&!At<CVehicle*>(o,0xC)){ At<CVehicle*>(o,0xC)=Veh(0); Reg(Veh(0),o+0xC); }
                            }
                            if(next()%4==0&&nm!="CreateSubTask")ped=Ped(4); // jugador
                        }
                        fixtureLineup=At<uintptr_t>(o,0x38);
                    } else {
                        if(nm=="Constructor"){ a0=int32(next()%8); b0=next()%256; b1=next()%2; f0=next()%4==0 ? 0.5f : uni(0.f,3.f); }
                        else if(nm=="GetWanderTaskByPedType"){ At<int32>(p0b,P_PEDTYPE)=int32(next()%26); }
                        else if(nm=="ScanForBlockedNode-1"){
                            q0=vecBuf.data(); const CVector pp=p0->GetPosition(); At<CVector>(vecBuf.data(),0)=CVector{pp.x+uni(-8.f,8.f),pp.y+uni(-8.f,8.f),pp.z};
                            const uint32 m=next()%4; q1=m==0 ? nullptr : m==1 ? (void*)Veh(int(next()%2)) : (void*)Ped(int(1+next()%3));
                        }
                        else {
                            setWander();
                            if(nm=="ScanForBlockedNode-"||nm=="ComputeTargetPos"){ At<CNodeAddress>(vecBuf.data(),0x20)=pickNode(); q0=next()%2 ? (void*)(o+0x1C) : (void*)(vecBuf.data()+0x20); }
                            if(nm=="ControlSubTask"||nm=="ScanForBlockedNodes"){
                                static const int32 ty[]{TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_PAUSE};
                                At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; At<CTask*>(o,8)=Fake(0);
                                if(next()%3==0)At<int32>(intels[0].data(),I_STATIC)=31+int32(next()%5);
                            }
                            if(nm=="CreateFirstSubTask"&&At<uint32>(p0b,P_FLAGS)&IN_VEH&&!At<CVehicle*>(p0b,P_VEH))At<CVehicle*>(p0b,P_VEH)=Veh(0);
                            // UpdateDir lee el nodo de m_NextNode sin comprobar el indice: en el area A0 solo nodos existentes
                            if(nm=="UpdateDir"&&At<CNodeAddress>(o,0x1C).m_wAreaId==A0)At<CNodeAddress>(o,0x1C).m_wNodeId=uint16(next()%NNODES);
                            if(nm=="CreateSubTask"){ static const int32 ty[]{TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_SIMPLE_GO_TO_POINT,TASK_FINISHED,TASK_COMPLEX_LEAVE_CAR,
                                    TASK_COMPLEX_OBSERVE_TRAFFIC_LIGHTS_AND_ACHIEVE_HEADING,TASK_COMPLEX_CROSS_ROAD_LOOK_AND_ACHIEVE_HEADING,TASK_SIMPLE_SCRATCH_HEAD,TASK_SIMPLE_PAUSE};
                                a0=ty[next()%std::size(ty)]; if(!At<CVehicle*>(p0b,P_VEH))At<CVehicle*>(p0b,P_VEH)=Veh(1); }
                            if(nm=="UpdatePathNodes"){ b0=next()%8|(next()&0xFFFFFF00u); At<uint8>(vecBuf.data(),0x30)=uint8(next());
                                q0=next()%2 ? (void*)(o+0x18) : (void*)(vecBuf.data()+0x20); q1=next()%2 ? (void*)(o+0x1C) : (void*)(vecBuf.data()+0x24);
                                At<CNodeAddress>(vecBuf.data(),0x20)=pickNode(); At<CNodeAddress>(vecBuf.data(),0x24)=pickNode(); }
                        }
                    }
                    const auto before=Snapshot();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr; size_t objSize=enter ? 0x50 : 0x28;
                    uintptr_t retPtr{}; bool isPtr=false; auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; };
                    auto callTask=[&](CPed* pd){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    auto callPedVoid=[&](CPed* pd){ reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd); };
                    if(nm=="ControlSubTask"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="CreateNextSubTask_AfterSimpleCarAlign"){ ret=Ptr(callTask(ped)); }
                    else if(enter){
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,void*,uint32,uint32,uint32,uint32)>(target)(o,q0,0xFFFFFF00u|b0,0xFFFFFF00u|b1,0xFFFFFF00u|b2,0xFFFFFF00u|b3));
                        else if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,ped,a0,ev);
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,ped));
                        else if(nm=="GetCameraAvoidVehicle")ret=Ptr(reinterpret_cast<CVehicle*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTargetPos")ret=Ptr(reinterpret_cast<CVector*(__thiscall*)(void*,CVector*)>(target)(o,&At<CVector>(vecBuf.data(),0)));
                        else callPedVoid(ped); // CreateTaskUtilityLineUpPedWithCar, PreparePed/VehicleFor..., SetVehicleFlags
                    }
                    else {
                        if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,int32,uint32,uint32,float)>(target)(o,a0,0xFFFFFF00u|b0,0xFFFFFF00u|b1,f0));
                        else if(nm=="ComputeTargetHeading")ret=F(reinterpret_cast<float(__thiscall*)(void*,CPed*)>(target)(o,p0));
                        else if(nm=="ComputeTargetPos")reinterpret_cast<void(__thiscall*)(void*,CPed*,CVector*,void*)>(target)(o,p0,&At<CVector>(vecBuf.data(),0),q0);
                        else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*,int32)>(target)(o,p0,a0));
                        else if(nm=="GetWanderTaskByPedType")ret=Ptr(reinterpret_cast<CTask*(__cdecl*)(CPed*)>(target)(p0));
                        else if(nm=="ScanForBlockedNode-")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,void*)>(target)(o,p0,q0);
                        else if(nm=="ScanForBlockedNode-1")ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*)>(target)(o,q0,q1);
                        else if(nm=="UpdatePathNodes")reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32,void*,void*,uint8*)>(target)(o,p0,b0,q0,q1,&At<uint8>(vecBuf.data(),0x30));
                        else if(nm=="ValidNodes")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                        else callPedVoid(p0); // ScanForBlockedNodes, UpdateDir
                    }
                    for(auto* h:hooks)h->State(false);

                    // utilidad de alineacion nueva (montón): se nombra, se serializa y se borra despues
                    if(enter&&nm!="Constructor"){ const auto lu=At<uintptr_t>(o,0x38); if(lu&&lu!=fixtureLineup)newLineup=lu; }
                    // tareas y eventos nuevos (se nombran antes de serializar el objeto)
                    { auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                      json tasks=json::array(); for(auto* t:fresh)tasks.push_back(SerTask(t)); out["tasks"]=tasks; }
                    { json events=json::array();
                      for(size_t k=0;k<occE.size();++k)if(!occE[k]&&!(States(EventHdr())[k]&0x80))events.push_back(SerEvent(reinterpret_cast<CEvent*>(Storage(EventHdr())+k*EVENT_SLOT)));
                      out["events"]=events; }
                    if(newLineup)out["lineup_new"]=Region(reinterpret_cast<void*>(newLineup),0x1C);
                    if(isPtr)ret=Name(retPtr);
                    // constructores: todo el objeto salvo el vptr; el resto: el objeto (sin vptr) tal y como queda
                    out["object"]=Region(o,objSize,true);
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    out["ret"]=ret; out["trace"]=trace; out["rng"]=rngCalls;
                    // limpieza: referencias de los fixtures, slots nuevos (sin destructor) y utilidad nueva
                    CleanRefs();
                    if(newLineup){ ::operator delete(reinterpret_cast<void*>(newLineup)); }
                    FreeFresh(TaskHdr(),occT); FreeFresh(EventHdr(),occE);
                    out["clean"]=Occ(TaskHdr())==occT&&Occ(EventHdr())==occE;
                    restoreGlobals();
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
    CleanRefs(); restoreGlobals(); resetPools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    doorStatusHook->State(doorStatusEnabled); doorStatusHook->LockState(doorStatusLocked);
    const bool restored=Occ(TaskHdr())==occTaskStart&&Occ(EventHdr())==occEvStart&&freeRefs()==savedFree&&globalsIntact()
                        &&Last(TaskHdr())==lt0&&Last(EventHdr())==le0;
    if(restored){ std::memcpy(States(TaskHdr()),stTask.data(),stTask.size()); std::memcpy(States(EventHdr()),stEv.data(),stEv.size()); }
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
