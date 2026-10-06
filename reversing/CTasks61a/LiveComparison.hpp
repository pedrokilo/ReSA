#pragma once
// Banco dentro del motor: tareas de coche (conducir, bajar, subir de conductor/pasajero, esperar, ir a punto, apoyarse, puerta de
// pasajero) del lote 61a, 9 clases con 58 funciones:
// CTaskSimpleCarDrive (constructor, destructor, Clone, GetTaskType, MakeAbortable, ProcessPed, SetPedPosition, TriggerIK,
// UpdateBopping, StartBopping, ProcessHeadBopping, ProcessArmBopping, ProcessBopping), CTaskSimpleCarDriveTimed (constructor,
// destructor, Clone, GetTaskType, ProcessPed), CTaskSimpleCarSetPedInAsDriver (constructor, Clone, GetTaskType, MakeAbortable,
// ProcessPed), CTaskSimpleCarSetPedInAsPassenger (destructor, Clone, GetTaskType, MakeAbortable, ProcessPed),
// CTaskSimpleCarSetPedOut (Clone, GetTaskType, MakeAbortable, PositionPedOutOfCollision, ProcessPed), CTaskComplexDriveToPoint
// (constructor, Clone, GetTaskType, SetUpCar, CreateSubTaskCannotGetInCar, Drive, 2 x IsTargetBlocked),
// CTaskComplexEnterCarAsPassengerWait (constructor, destructor, Clone, GetTaskType, CreateSubTask, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask), CTaskComplexOpenPassengerDoor (constructor, destructor, Clone, GetTaskType) y
// CTaskLeanOnVehicle (constructor, destructor, FinishAnimCB, MakeAbortable, ProcessPed).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 5 peatones (matriz casi siempre, inteligencia, clump, huesos con fotogramas, datos de jugador, armas, vehiculo), 3 vehiculos
// (vtable falsa con trampas y las virtuales usadas trazadas, manejo propio, matriz, puertas, pasajeros), 8 tareas falsas (vtable
// trazada), 4 asociaciones de animacion, 7 modelos falsos (colision con radio y estructura de vehiculo con los dummies), una utilidad
// de alineacion falsa y una arena para el unico new/delete del DLL (CTaskUtilityLineUpPedWithCar).
// Se simulan y trazan SIN asignar memoria (registros fijos que se convierten a json tras la llamada): RNG, FindPlayerPed, CPed
// (Say, UpdateStat*, RestartNonPartialAnims, SetMoveState, SetPedState, Remove/ReplaceWeapon*, PositionPedOutOfCollision,
// GetEntityThatThisPedIsHolding, SetPedPositionInCar), CPlayerPed (ClearAdrenaline, GetPadFromPlayer), CPad (acelerar, girar, frenar),
// CVehicle (RemoveDriver/Passenger, SetDriver, AddPassenger x2, Set/ClearGettingIn/OutFlags, IsLawEnforcementVehicle,
// ChangeLawEnforcerState, CanPedLeanOut, IsPassenger, Set/ClearWindowOpenFlag), CCarEnterExit (CarHasDoorToClose, RemoveCarSitAnim,
// RemoveGetInAnims, AddInCarAnim), CCrime::ReportCrime, CGarages::IsPointWithinAnyGarage, CWorld (TestSphereAgainstWorld,
// GetIsLineOfSightClear), animaciones (GetAssociation, BlendAnimation, AddAnimation, SetFinish/DeleteCallback), IKChainManager
// (IsLooking, LookAt, AbortLookAt), CEventGroup::Add, CPedGroups::GetPedsGroup, CPedGroupMembership (IsLeader, GetLeader),
// CPedGroupIntelligence::AddEvent, GetEventGlobalGroup, CAudioEngine::HasRadioRetuneJustStarted, CUpsideDownCarCheck, CGameLogic::
// IsCoopGameGoingOn, CPedGeometryAnalyser::ComputeClosestSurfacePoint, RtQuatRotate, CTaskManager (SetTask/ChangeTaskInSlot,
// FindActiveTaskByType, FindTaskByType), CTaskUtilityLineUpPedWithCar::ProcessPed, GetWanderTaskByPedType y CCarAI::GetCarToGoToCoors*.
// Las tareas y eventos que crea el codigo salen de sus pools reales; las tareas nuevas se serializan enteras. Los pools de tareas y
// eventos restauran m_LastFreeSlot y los slots libres se rellenan con 0xCD en cada ejecucion. Se guardan y restauran el reloj, el
// contador de fotogramas, el paso de tiempo, la lluvia, la camara (posicion, matriz, camara activa y modos), el truco de apuntar
// conduciendo, CWorld::pIgnoreEntity y los siete punteros de modelo falsos. Constructores sobre buffer con 0xCD comparado entero.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Tasks/Task.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleCarDrive.h"
#include "Tasks/TaskTypes/TaskSimpleCarDriveTimed.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsPassenger.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedOut.h"
#include "Tasks/TaskTypes/TaskComplexDriveToPoint.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsPassengerWait.h"
#include "Tasks/TaskTypes/TaskComplexOpenPassengerDoor.h"
#include "Tasks/TaskTypes/TaskLeanOnVehicle.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "Tasks/TaskTypes/TaskComplexCarDrive.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "CarEnterExit.h"
#include "CarAI.h"
#include "Crime.h"
#include "Garages.h"
#include "Camera.h"
#include "Cheat.h"
#include "Weather.h"
#include "GameLogic.h"
#include "Audio/AudioEngine.h"
#include "UpsideDownCarCheck.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "Events/EventGroup.h"
#include "Events/EventGlobalGroup.h"
#include "RwHelper.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventCarUpsideDown.h"
#include "Events/EventCopCarBeingStolen.h"
#include "Events/EventLeaderEnteredCarAsDriver.h"
#include "Models/ModelInfo.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/EventPool.h"
#include "../ProbeRuntime.hpp"

namespace CTasks61aLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=5, NVEH=3, FAKES=8, NASSOC=4, NMI=7, NBONES=19;
// indices de modelo falsos: 4 genericos y los que el EXE compara por indice (0x23A tren, 0x213 moto, 0x1AF autobus)
constexpr int MI_IDX[NMI]{10000,10001,10002,10003,0x23A,0x213,0x1AF};
// tareas falsas: campos de control por encima de 0x60 (GoToPoint usa hasta +0x24)
constexpr size_t FAKE_SIZE=0x80, FK_ABORT=0x60, FK_SUB=0x64, FK_TYPE=0x68, FK_CLONE=0x6C, FK_CNT=0x70;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_HEADING=0x10, E_MATRIX=0x14, E_RWOBJ=0x18, E_FLAGS=0x1C, E_SEED=0x20, E_MODEL=0x22, E_TYPE=0x36;  // CEntity
constexpr size_t PH_SPEED=0x44, PH_ATTACHED=0xFC;                                                                              // CPhysical
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_CREATEDBY=0x484,
                 P_BONES=0x488, P_STATE=0x530, P_ROT=0x558, P_AIMROT=0x55C, P_STANDING=0x568, P_VEH=0x58C, P_PEDTYPE=0x598,
                 P_WEAPON4=0x610;                                                                                              // CPed
constexpr size_t V_HANDLING=0x384, V_STYLE=0x3B9, V_MISSION=0x3BA, V_TIMESTART=0x3AC, V_SPEED=0x3D0, V_TARGET=0x41C, V_FLAGS=0x428,
                 V_ALARM=0x45C, V_DRIVER=0x460, V_PASS=0x464, V_NUMPASS=0x484, V_GETIN=0x486, V_GETOUT=0x487, V_BITS=0x4A8,
                 V_LOCK=0x4F8, V_TYPE=0x590, V_SUBTYPE=0x594, V_DOORS=0x5A9, V_BIKE=0x614, V_TRAINDOOR=0x608;                   // CVehicle
constexpr size_t H_MODELFLAGS=0xCC, H_ANIMGROUP=0xDE, H_SIZE=0x100;                                                             // handling
constexpr size_t I_TASKMGR=4, I_EVGROUP=0x68, I_VEHENTS=0xE0, I_PEDENTS=0x130;                                                 // CPedIntelligence
constexpr size_t G_MEMBERS=0xC, PEDGROUP_SIZE=0x2D4, G_CURRENT=0x34;
constexpr size_t EV_TYPE=0x30;
constexpr uintptr_t TIMER=0xB7CB84, FRAMECTR=0xB7CB4C, TIMESTEP=0xB7CB5C, RAIN=0xC81324, CAMPOS=0xB6F02C, CAMMAT=0xB6F03C,
                    CAMACTIVE=0xB6F081, CAMMODE=0xB6F1A8, CHEAT=0x969179, IGNOREENT=0xB7CD68;
constexpr uintptr_t VT_CD=0x86E904, VT_DT=0x859E50, VT_DRV=0x86EE28, VT_PSG=0x86EE04, VT_OUT=0x86EEB8, VT_DTP=0x86E9DC,
                    VT_EPW=0x86E778, VT_OPD=0x86EB3C, VT_LEAN=0x86FAF4;
constexpr uint32 IN_VEH=0x100u, RENDER_IN_CAR=0x2000u;

alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NPEDS> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS> pedMats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPEDS> clumps{};
alignas(16) inline std::array<std::array<std::array<uint8,0x20>,NBONES>,NPEDS> boneFrames{};
alignas(16) inline std::array<std::array<std::array<uint8,0x40>,NBONES>,NPEDS> keyFrames{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,NVEH> vehs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NVEH> vehMats{};
alignas(16) inline std::array<std::array<uint8,H_SIZE>,NVEH> handlings{};
alignas(16) inline std::array<std::array<uint8,0x80>,NMI> modelInfos{};
alignas(16) inline std::array<std::array<uint8,0x40>,NMI> colModels{};
alignas(16) inline std::array<std::array<uint8,0x200>,NMI> vehStructs{};
alignas(16) inline std::array<std::array<uint8,FAKE_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<uint8,0x100> object{};
alignas(16) inline std::array<uint8,0x40> utilFix{};
alignas(16) inline std::array<uint8,0x40> arena{};
alignas(16) inline std::array<uint8,PEDGROUP_SIZE> group{};
alignas(16) inline std::array<uint8,0x100> playerData{};
alignas(16) inline std::array<uint8,0x40> pad{};
alignas(16) inline std::array<uint8,0x40> rideData{};
alignas(16) inline std::array<uint8,0x40> globalGroup{};
alignas(16) inline std::array<uint8,0x100> heldObj{};
alignas(16) inline std::array<uint8,0x40> event{};
alignas(16) inline std::array<uint8,0x40> vecBuf{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CVehicle* Veh(int n) { return reinterpret_cast<CVehicle*>(vehs[n].data()); }
// índice del vehículo falso: los buffers miden sizeof(CAutomobile), no sizeof(CVehicle) (la resta de punteros CVehicle* daba índices fuera de rango)
inline size_t VehIdx(const CVehicle* v) { return size_t(reinterpret_cast<const uint8*>(v)-vehs[0].data())/vehs[0].size(); }
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

// ---- traza sin asignar memoria: registros fijos (los punteros se nombran al convertirlos a json, tras la llamada) ----
struct Rec { const char* tag; uint32 n; uint64 mask; uint32 v[64]; };
inline std::array<Rec,2048> recs{}; inline uint32 nrec{};
struct Pt { uint32 v; Pt(const void* p) : v(uint32(uintptr_t(p))) {} };
inline Rec* NewRec(const char* tag) { Rec* r=&recs[nrec<recs.size() ? nrec++ : recs.size()-1]; r->tag=tag; r->n=0; r->mask=0; return r; }
inline void Arg(Rec* r,Pt p) { if(r->n<64){ r->mask|=1ull<<r->n; r->v[r->n++]=p.v; } }
template<class N> requires std::is_arithmetic_v<N> inline void Arg(Rec* r,N x) {
    if(r->n<64){ if constexpr(std::is_floating_point_v<N>)r->v[r->n++]=std::bit_cast<uint32>(float(x)); else r->v[r->n++]=uint32(x); } }
template<class N> requires std::is_enum_v<N> inline void Arg(Rec* r,N x) { if(r->n<64)r->v[r->n++]=uint32(int32(x)); }
template<class... A> inline void T(const char* tag,A... a) { Rec* r=NewRec(tag); (Arg(r,a),...); }

// nombres de buffers (se registran una vez en Update)
struct Buf { std::string name; uint8* p; size_t n; bool cmp; };
inline std::vector<Buf> bufs;
inline uintptr_t dllFinishCb{}, dllDefaultCb{}, dllFinishCbRaw{}, dllDefaultCbRaw{};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==0x661160||v==dllFinishCb||v==dllFinishCbRaw)return "FinishAnimCB"; if(v==0x4D1390||v==dllDefaultCb||v==dllDefaultCbRaw)return "DefaultAnimCB"; if(v==0x63C840)return "RollDoorCB";
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return b.name+"+"+std::to_string(v-uintptr_t(b.p));
    { const auto ts=uintptr_t(Storage(TaskHdr())); if(v>=ts&&v<ts+size_t(Cap(TaskHdr()))*TASK_SLOT)return "pool_task#"+std::to_string((v-ts)/TASK_SLOT)+((v-ts)%TASK_SLOT ? "+"+std::to_string((v-ts)%TASK_SLOT) : std::string()); }
    { const auto es=uintptr_t(Storage(EventHdr())); if(v>=es&&v<es+size_t(Cap(EventHdr()))*EVENT_SLOT)return "evpool#"+std::to_string((v-es)/EVENT_SLOT)+"+"+std::to_string((v-es)%EVENT_SLOT); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); for(auto& v:vehs)r.push_back(RefList(v.data())); return r; }
inline json SerTask(const void* p) {
    const auto* t=static_cast<const CTask*>(p); const int type=const_cast<CTask*>(t)->GetTaskType();
    return {{"type",type},{"words",Region(p,TASK_SLOT,true)}};
}
inline json Trace() {
    json t=json::array();
    for(uint32 k=0;k<nrec;++k){ const Rec& r=recs[k]; json a=json::array({std::string(r.tag)});
        for(uint32 i=0;i<r.n;++i)a.push_back((r.mask>>i)&1 ? Name(uintptr_t(r.v[i])) : json(r.v[i]));
        t.push_back(std::move(a)); }
    return t;
}

// buffers de fixtures cuyos cambios se comparan (palabra a palabra, traducidas)
inline std::vector<std::vector<uint8>> Snapshot() { std::vector<std::vector<uint8>> s; for(auto& b:bufs)if(b.cmp)s.push_back(ReversingProbe::Bytes(b.p,b.n)); return s; }
inline json Changes(const std::vector<std::vector<uint8>>& before) {
    json r=json::array(); size_t k=0;
    for(auto& b:bufs){ if(!b.cmp)continue; for(size_t w=0;w+4<=b.n;w+=4)if(std::memcmp(b.p+w,before[k].data()+w,4))r.push_back({b.name,w,Name(At<uint32>(b.p,w))}); ++k; }
    return r;
}

// configuracion por caso de las dependencias simuladas
inline uint32 seed{}, rngCalls{};
enum { M_LOOK,M_LAW,M_LEAN,M_PASS,M_HASDOOR,M_MISSING,M_TESTCOL,M_SPHERE,M_LOS,M_GARAGE,M_UPSIDE,M_COOP,M_RETUNE,M_LEADER,M_CLOSEST,M_UTIL,
       M_FIND,M_ASSOC,M_HOLD,M_PAD,M_COUNT };
inline std::array<uint32,M_COUNT> mk{}, ck{};
inline bool Bit(int k) { return (mk[k]>>(ck[k]++&31))&1; }
inline uintptr_t playerRet{}, pedsGroupRet{}, leaderRet{}, holdRet{}, padRet{}, sphereRet0{}, sphereRet1{}; inline uint32 findIdx{}, assocIdx{};
inline std::array<int16,3> padVal{}; inline CVector closestVec{}; inline int32 ridePtrNull{};

// tareas falsas: GetTaskType trazado; MakeAbortable segun una mascara por llamada (contador en el propio buffer)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { T("delete_fake",Pt(t),f); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { T("clone_fake",Pt(t)); return At<CTask*>(t,FK_CLONE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { T("type",Pt(t)); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int32 prio,const CEvent* e) {
    const uint32 k=At<uint32>(t,FK_CNT)++; const bool r=(At<uint32>(t,FK_ABORT)>>(k&31))&1;
    T("make_abortable_fake",Pt(t),Pt(p),prio,Pt(e),r); return r;
}
template<int N> void* __fastcall FTrap(void* self,void*) { T("unexpected_slot",N,Pt(self)); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
inline std::array<void*,16> fakeVtbl{};
inline std::array<void*,40> pedVtbl{};
inline std::array<void*,96> vehVtbl{};
inline std::array<void*,20> eventVtbl{};
// peatones falsos: TestCollision (hueco 13) y SetMoveAnim (hueco 23)
inline bool __fastcall PTest(CPed* p,void*,uint32 apply) { const bool r=Bit(M_TESTCOL); T("test_collision",Pt(p),apply&0xFF,r); return r; }
inline void __fastcall PMoveAnim(CPed* p,void*) { T("set_move_anim",Pt(p)); }
// vehiculos falsos: ProcessOpenDoor (28), IsDoorMissing (38), ProcessDrivingAnims (46), GetRideAnimData (47)
inline void __fastcall VOpenDoor(CVehicle* v,void*,CPed* p,uint32 comp,uint32 grp,uint32 anim,float t) { T("process_open_door",Pt(v),Pt(p),comp,grp,anim,t); }
inline bool __fastcall VMissing(CVehicle* v,void*,uint32 door) { const bool r=Bit(M_MISSING); T("is_door_missing",Pt(v),door,r); return r; }
inline void __fastcall VDrivingAnims(CVehicle* v,void*,CPed* p,uint32 blend) { T("process_driving_anims",Pt(v),Pt(p),blend&0xFF); }
inline void* __fastcall VRideData(CVehicle* v,void*) { void* r=ridePtrNull ? nullptr : rideData.data(); T("get_ride_anim_data",Pt(v),Pt(r)); return r; }
inline int32 __fastcall EType(CEvent* e,void*) { T("event_type",Pt(e)); return At<int32>(e,EV_TYPE); }

// eventos reales (de la pila o del pool): tipo y solo los campos con significado (sin relleno)
inline void EvArgs(Rec* r,const CEvent* e,int depth=0) {
    if(!e){ Arg(r,0u); return; }
    const int type=const_cast<CEvent*>(e)->GetEventType(); Arg(r,type); Arg(r,At<int32>(e,4)); Arg(r,At<uint8>(e,8));
    switch(type){
    case EVENT_GROUP_EVENT: Arg(r,Pt(At<void*>(e,0xC))); if(depth<2)EvArgs(r,At<const CEvent*>(e,0x10),depth+1); break;
    case EVENT_CAR_UPSIDE_DOWN: Arg(r,Pt(At<void*>(e,0xC))); break;
    case EVENT_COP_CAR_BEING_STOLEN: Arg(r,Pt(At<void*>(e,0xC))); Arg(r,Pt(At<void*>(e,0x10))); break;
    case EVENT_LEADER_ENTERED_CAR_AS_DRIVER: Arg(r,At<uint8>(e,0xC)); Arg(r,At<int16>(e,0xE)); Arg(r,At<int16>(e,0x10)); Arg(r,Pt(At<void*>(e,0x14))); break;
    case EVENT_SCRIPT_COMMAND: {
        Arg(r,At<int32>(e,0xC)); const auto* t=At<const CTask*>(e,0x10); Arg(r,Pt(t)); Arg(r,At<uint8>(e,0x14));
        if(t){ Arg(r,const_cast<CTask*>(t)->GetTaskType()); const auto* w=reinterpret_cast<const uint32*>(t); for(size_t k=1;k<TASK_SLOT/4&&r->n<62;++k)Arg(r,Pt(reinterpret_cast<const void*>(uintptr_t(w[k])))); }
        break; }
    default: break;
    }
}

// dependencias simuladas
inline int __cdecl FakeRand() { T("rand"); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { T("find_player_ped",id,Pt(reinterpret_cast<void*>(playerRet))); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline int16 __fastcall FakeSay(CPed* p,void*,int ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c) { T("say",Pt(p),int16(ctx),delay,prob,a&0xFF,b&0xFF,c&0xFF); return -1; }
inline void __fastcall FakeStatLeave(CPed* p,void*) { T("update_stat_leaving",Pt(p)); }
inline void __fastcall FakeStatEnter(CPed* p,void*) { T("update_stat_entering",Pt(p)); }
inline void __fastcall FakeRestart(CPed* p,void*) { T("restart_non_partial_anims",Pt(p)); }
inline void __fastcall FakeMoveState(CPed* p,void*,uint32 s) { T("set_move_state",Pt(p),s); }
inline void __fastcall FakePedState(CPed* p,void*,uint32 s) { T("set_ped_state",Pt(p),s); }
inline void __fastcall FakeRemWeapon(CPed* p,void*,uint32 a) { T("remove_weapon_entering",Pt(p),a); }
inline void __fastcall FakeReplWeapon(CPed* p,void*) { T("replace_weapon_exiting",Pt(p)); }
inline void __fastcall FakeClearAdrenaline(CPed* p,void*) { T("clear_adrenaline",Pt(p)); }
inline void* __fastcall FakeGetPad(CPed* p,void*) { T("get_pad",Pt(p),Pt(reinterpret_cast<void*>(padRet))); return reinterpret_cast<void*>(padRet); }
inline int16 __fastcall FakeAcc(void* d,void*) { T("pad_accelerate",Pt(d),int32(padVal[0])); return padVal[0]; }
inline int16 __fastcall FakeSteer(void* d,void*) { T("pad_steer",Pt(d),int32(padVal[1])); return padVal[1]; }
inline int16 __fastcall FakeBrake(void* d,void*) { T("pad_brake",Pt(d),int32(padVal[2])); return padVal[2]; }
inline bool __fastcall FakePosOut(CPed* p,void*,int32 door,CVehicle* v,uint32 f) { T("ped_position_out_of_collision",Pt(p),door,Pt(v),f&0xFF); return true; }
inline CEntity* __fastcall FakeHolding(CPed* p,void*) { const bool b=Bit(M_HOLD); T("holding",Pt(p),b); return b ? reinterpret_cast<CEntity*>(holdRet) : nullptr; }
inline void __fastcall FakeSetPosInCar(CPed* p,void*) { T("set_ped_position_in_car",Pt(p)); }
inline bool __cdecl FakeHasDoor(const CVehicle* v,int32 door) { const bool r=Bit(M_HASDOOR); T("car_has_door_to_close",Pt(v),door,r); return r; }
inline void __cdecl FakeRemSit(const CPed* p) { T("remove_car_sit_anim",Pt(p)); }
inline void __cdecl FakeRemGetIn(const CPed* p) { T("remove_get_in_anims",Pt(p)); }
inline void __cdecl FakeAddInCar(const CVehicle* v,CPed* p,uint32 drv) { T("add_in_car_anim",Pt(v),Pt(p),drv&0xFF); }
inline void __fastcall FakeRemoveDriver(CVehicle* v,void*,uint32 a) { T("remove_driver",Pt(v),a&0xFF); }
inline void __fastcall FakeRemovePassenger(CVehicle* v,void*,CPed* p) { T("remove_passenger",Pt(v),Pt(p)); }
inline void __fastcall FakeSetDriver(CVehicle* v,void*,CPed* p) { T("set_driver",Pt(v),Pt(p)); }
inline bool __fastcall FakeAddPass(CVehicle* v,void*,CPed* p) { T("add_passenger_auto",Pt(v),Pt(p)); return true; }
inline bool __fastcall FakeAddPassSeat(CVehicle* v,void*,CPed* p,uint32 s) { T("add_passenger_seat",Pt(v),Pt(p),s&0xFF); return true; }
inline void __fastcall FakeClearOut(CVehicle* v,void*,uint32 f) { T("clear_getting_out_flags",Pt(v),f&0xFF); }
inline void __fastcall FakeClearIn(CVehicle* v,void*,uint32 f) { T("clear_getting_in_flags",Pt(v),f&0xFF); }
inline void __fastcall FakeSetOut(CVehicle* v,void*,uint32 f) { T("set_getting_out_flags",Pt(v),f&0xFF); }
inline bool __fastcall FakeIsLaw(CVehicle* v,void*) { const bool r=Bit(M_LAW); T("is_law_enforcement_vehicle",Pt(v),r); return r; }
inline void __fastcall FakeChangeLaw(CVehicle* v,void*,uint32 a) { T("change_law_enforcer_state",Pt(v),a&0xFF); }
inline bool __fastcall FakeLean(CVehicle* v,void*,CPed* p) { const bool r=Bit(M_LEAN); T("can_ped_lean_out",Pt(v),Pt(p),r); return r; }
inline bool __fastcall FakeIsPassenger(CVehicle* v,void*,CPed* p) { const bool r=Bit(M_PASS); T("is_passenger",Pt(v),Pt(p),r); return r; }
inline void __fastcall FakeSetWindow(CVehicle* v,void*,uint32 d) { T("set_window_open_flag",Pt(v),d&0xFF); }
inline void __fastcall FakeClearWindow(CVehicle* v,void*,uint32 d) { T("clear_window_open_flag",Pt(v),d&0xFF); }
inline void __cdecl FakeCrime(uint32 type,CEntity* e,CPed* p) { T("report_crime",type,Pt(e),Pt(p)); }
inline bool __cdecl FakeGarage(CVector* v) { const bool r=Bit(M_GARAGE); T("point_within_any_garage",v->x,v->y,v->z,r); return r; }
inline CEntity* __cdecl FakeSphere(float x,float y,float z,float r,CEntity* ign,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f) {
    const bool hit=Bit(M_SPHERE); CEntity* ret=hit ? reinterpret_cast<CEntity*>((ck[M_SPHERE]&1) ? sphereRet1 : sphereRet0) : nullptr;
    T("test_sphere",x,y,z,r,Pt(ign),a&0xFF,b&0xFF,c&0xFF,d&0xFF,e&0xFF,f&0xFF,Pt(ret)); return ret;
}
inline bool __cdecl FakeLos(const CVector* o,const CVector* t,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e,uint32 f,uint32 g) {
    const bool r=Bit(M_LOS); T("line_of_sight",o->x,o->y,o->z,t->x,t->y,t->z,a&0xFF,b&0xFF,c&0xFF,d&0xFF,e&0xFF,f&0xFF,g&0xFF,r); return r;
}
inline CAnimBlendAssociation* __cdecl FakeGetAssoc(RpClump* c,uint32 id) {
    const bool b=Bit(M_ASSOC); auto* a=b ? assocs[assocIdx++%NASSOC].data() : nullptr; T("get_association",Pt(c),id,Pt(a)); return reinterpret_cast<CAnimBlendAssociation*>(a);
}
inline CAnimBlendAssociation* __cdecl FakeBlend(RpClump* c,int32 g,int32 a,float blend) {
    auto* r=assocs[assocIdx++%NASSOC].data(); T("blend_animation",Pt(c),g,a,blend,Pt(r)); return reinterpret_cast<CAnimBlendAssociation*>(r);
}
inline CAnimBlendAssociation* __cdecl FakeAddAnim(RpClump* c,int32 g,int32 a) {
    auto* r=assocs[assocIdx++%NASSOC].data(); T("add_animation",Pt(c),g,a,Pt(r)); return reinterpret_cast<CAnimBlendAssociation*>(r);
}
inline void __fastcall FakeSetFinishCb(void* a,void*,void* cb,void* data) { T("set_finish_callback",Pt(a),Pt(cb),Pt(data)); }
inline void __fastcall FakeSetDeleteCb(void* a,void*,void* cb,void* data) { T("set_delete_callback",Pt(a),Pt(cb),Pt(data)); }
inline bool __fastcall FakeIsLooking(void*,void*,CPed* p) { const bool r=Bit(M_LOOK); T("is_looking",Pt(p),r); return r; }
inline uint32 Fnv(const char* s) { uint32 h=2166136261u; for(int k=0;k<24&&s&&s[k];++k){ h^=uint8(s[k]); h*=16777619u; } return h; }
inline void __fastcall FakeLookAt(void*,void*,const char* purpose,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    T("look_at",Fnv(purpose),Pt(p),Pt(e),time,bone,Pt(off),torso&0xFF,speed,blend,prio,force&0xFF);
}
inline void __fastcall FakeAbortLookAt(void*,void*,CPed* p,uint32 blend) { T("abort_look_at",Pt(p),blend); }
inline CEvent* __fastcall FakeEventAdd(void* g,void*,CEvent* e,uint32 valid) { Rec* r=NewRec("event_group_add"); Arg(r,Pt(g)); Arg(r,valid&0xFF); EvArgs(r,e); return e; }
inline bool __fastcall FakeGroupAddEvent(void* i,void*,CEvent* e) { Rec* r=NewRec("group_add_event"); Arg(r,Pt(i)); EvArgs(r,e); return true; }
inline CPedGroup* __cdecl FakePedsGroup(const CPed* p) { T("get_peds_group",Pt(p),Pt(reinterpret_cast<void*>(pedsGroupRet))); return reinterpret_cast<CPedGroup*>(pedsGroupRet); }
inline bool __fastcall FakeIsLeader(void* m,void*,const CPed* p) { const bool r=Bit(M_LEADER); T("is_leader",Pt(m),Pt(p),r); return r; }
inline CPed* __fastcall FakeGetLeader(void* m,void*) { T("get_leader",Pt(m),Pt(reinterpret_cast<void*>(leaderRet))); return reinterpret_cast<CPed*>(leaderRet); }
inline void* __cdecl FakeGlobalGroup() { T("get_event_global_group"); return globalGroup.data(); }
inline bool __fastcall FakeRetune(void*,void*) { const bool r=Bit(M_RETUNE); T("radio_retune_started",r); return r; }
inline bool __fastcall FakeUpside(void*,void*,const CVehicle* v) { const bool r=Bit(M_UPSIDE); T("is_car_upside_down",Pt(v),r); return r; }
inline bool __cdecl FakeCoop() { const bool r=Bit(M_COOP); T("is_coop",r); return r; }
inline bool __cdecl FakeClosest(const CPed* p,CEntity* e,CVector* out) { const bool r=Bit(M_CLOSEST); *out=closestVec; T("closest_surface_point",Pt(p),Pt(e),closestVec.x,closestVec.y,closestVec.z,r); return r; }
inline RtQuat* __cdecl FakeQuatRotate(RtQuat* q,const RwV3d* axis,float angle,int32 op) { T("quat_rotate",Pt(q),axis->x,axis->y,axis->z,angle,op); return q; }
inline CTask* __fastcall FakeFindActive(void* mgr,void*,int32 type) {
    const bool b=Bit(M_FIND); CTask* r=b ? Fake(int(findIdx++%3)) : nullptr; T("find_active_task",Pt(mgr),type,Pt(r)); return r;
}
inline CTask* __fastcall FakeFindTask(void* mgr,void*,int32 idx,int32 type) {
    const bool b=Bit(M_FIND); CTask* r=b ? Fake(int(findIdx++%3)) : nullptr; T("find_task_by_type",Pt(mgr),idx,type,Pt(r)); return r;
}
inline bool __fastcall FakeUtilProcess(void* u,void*,CPed* p,CVehicle* v,CAnimBlendAssociation* a) { const bool r=Bit(M_UTIL); T("util_process",Pt(u),Pt(p),Pt(v),Pt(a),r); return r; }
inline CTask* __cdecl FakeWander(CPed* p) { T("get_wander_task",Pt(p)); return nullptr; }
inline void __fastcall FakeSetTaskExe(void* mgr,void*,CTask* t,int32 idx,uint32 b) { T("set_task",Pt(mgr),Pt(t),idx,b&0xFF); }
inline void __fastcall FakeChangeInSlotDll(void* mgr,void*,CTask** slot,CTask* t) { T("set_task",Pt(mgr),Pt(t),int32((uintptr_t(slot)-uintptr_t(mgr))/4),0u); }
inline float __cdecl FakeGoToCoors0(CVehicle* v,const CVector* c,int32 style,uint32 b) { T("go_to_coors",0u,Pt(v),c->x,c->y,c->z,style,b&0xFF); return 0.f; }
inline float __cdecl FakeGoToCoors1(CVehicle* v,const CVector* c,int32 style,uint32 b) { T("go_to_coors",1u,Pt(v),c->x,c->y,c->z,style,b&0xFF); return 0.f; }
inline float __cdecl FakeGoToCoors2(CVehicle* v,const CVector* c,int32 style,uint32 b) { T("go_to_coors",2u,Pt(v),c->x,c->y,c->z,style,b&0xFF); return 0.f; }
inline float __cdecl FakeGoToCoors3(CVehicle* v,const CVector* c,int32 style,uint32 b) { T("go_to_coors",3u,Pt(v),c->x,c->y,c->z,style,b&0xFF); return 0.f; }
// el unico new/delete del DLL (CTaskUtilityLineUpPedWithCar) cae en una arena propia; sin trazar dentro (solo contadores)
inline uint32 newCount{}, newSize{}, delCount{}, delOff{};
inline void* __cdecl FakeNew(size_t n) { ++newCount; newSize=uint32(n); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p) { ++delCount; delOff=uint32(uintptr_t(p)-uintptr_t(arena.data())); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// ChangeTaskInSlot es protegido: se toma su direccion desde una clase derivada
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeInSlot() { return MemberAddress(&TaskManagerAccess::ChangeTaskInSlot); } };

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=58;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleCarDrive","Clone",0,0x86E904,1,256},
    {"CTaskSimpleCarDrive","Constructor",0x63C340,0,-1,256},
    {"CTaskSimpleCarDrive","Destructor",0x63C460,0,-1,256},
    {"CTaskSimpleCarDrive","GetTaskType",0,0x86E904,4,256},
    {"CTaskSimpleCarDrive","MakeAbortable",0,0x86E904,6,512},
    {"CTaskSimpleCarDrive","ProcessArmBopping",0x642AE0,0,-1,1024},
    {"CTaskSimpleCarDrive","ProcessBopping",0x642E70,0,-1,512},
    {"CTaskSimpleCarDrive","ProcessHeadBopping",0x6428C0,0,-1,1024},
    {"CTaskSimpleCarDrive","ProcessPed",0,0x86E904,7,1024},
    {"CTaskSimpleCarDrive","SetPedPosition",0,0x86E904,8,512},
    {"CTaskSimpleCarDrive","StartBopping",0x642760,0,-1,512},
    {"CTaskSimpleCarDrive","TriggerIK",0x63C500,0,-1,512},
    {"CTaskSimpleCarDrive","UpdateBopping",0x63C900,0,-1,256},
    {"CTaskSimpleCarDriveTimed","Clone",0,0x859E50,1,256},
    {"CTaskSimpleCarDriveTimed","Constructor",0x5FF940,0,-1,256},
    {"CTaskSimpleCarDriveTimed","Destructor",0x46F690,0,-1,256},
    {"CTaskSimpleCarDriveTimed","GetTaskType",0,0x859E50,4,256},
    {"CTaskSimpleCarDriveTimed","ProcessPed",0,0x859E50,7,512},
    {"CTaskSimpleCarSetPedInAsDriver","Clone",0,0x86EE28,1,256},
    {"CTaskSimpleCarSetPedInAsDriver","Constructor",0x6470E0,0,-1,256},
    {"CTaskSimpleCarSetPedInAsDriver","GetTaskType",0,0x86EE28,4,256},
    {"CTaskSimpleCarSetPedInAsDriver","MakeAbortable",0,0x86EE28,6,256},
    {"CTaskSimpleCarSetPedInAsDriver","ProcessPed",0,0x86EE28,7,1024},
    {"CTaskSimpleCarSetPedInAsPassenger","Clone",0,0x86EE04,1,256},
    {"CTaskSimpleCarSetPedInAsPassenger","Destructor",0x647080,0,-1,256},
    {"CTaskSimpleCarSetPedInAsPassenger","GetTaskType",0,0x86EE04,4,256},
    {"CTaskSimpleCarSetPedInAsPassenger","MakeAbortable",0,0x86EE04,6,256},
    {"CTaskSimpleCarSetPedInAsPassenger","ProcessPed",0,0x86EE04,7,1024},
    {"CTaskSimpleCarSetPedOut","Clone",0,0x86EEB8,1,256},
    {"CTaskSimpleCarSetPedOut","GetTaskType",0,0x86EEB8,4,256},
    {"CTaskSimpleCarSetPedOut","MakeAbortable",0,0x86EEB8,6,256},
    {"CTaskSimpleCarSetPedOut","PositionPedOutOfCollision",0x6479B0,0,-1,1024},
    {"CTaskSimpleCarSetPedOut","ProcessPed",0,0x86EEB8,7,1024},
    {"CTaskComplexDriveToPoint","Clone",0,0x86E9DC,1,256},
    {"CTaskComplexDriveToPoint","Constructor",0x63CE00,0,-1,256},
    {"CTaskComplexDriveToPoint","CreateSubTaskCannotGetInCar",0,0x86E9DC,12,256},
    {"CTaskComplexDriveToPoint","Drive",0,0x86E9DC,13,1024},
    {"CTaskComplexDriveToPoint","GetTaskType",0,0x86E9DC,4,256},
    {"CTaskComplexDriveToPoint","IsTargetBlocked-",0x6452C0,0,-1,512},
    {"CTaskComplexDriveToPoint","IsTargetBlocked-entities",0x6432A0,0,-1,512},
    {"CTaskComplexDriveToPoint","SetUpCar",0,0x86E9DC,11,256},
    {"CTaskComplexEnterCarAsPassengerWait","Clone",0,0x86E778,1,256},
    {"CTaskComplexEnterCarAsPassengerWait","Constructor",0x63B320,0,-1,256},
    {"CTaskComplexEnterCarAsPassengerWait","ControlSubTask",0,0x86E778,10,1024},
    {"CTaskComplexEnterCarAsPassengerWait","CreateFirstSubTask",0,0x86E778,9,512},
    {"CTaskComplexEnterCarAsPassengerWait","CreateNextSubTask",0,0x86E778,8,1024},
    {"CTaskComplexEnterCarAsPassengerWait","CreateSubTask",0x6408D0,0,-1,1024},
    {"CTaskComplexEnterCarAsPassengerWait","Destructor",0x63B3C0,0,-1,256},
    {"CTaskComplexEnterCarAsPassengerWait","GetTaskType",0,0x86E778,4,256},
    {"CTaskComplexOpenPassengerDoor","Clone",0,0x86EB3C,1,256},
    {"CTaskComplexOpenPassengerDoor","Constructor",0x6403E0,0,-1,256},
    {"CTaskComplexOpenPassengerDoor","Destructor",0x640420,0,-1,256},
    {"CTaskComplexOpenPassengerDoor","GetTaskType",0,0x86EB3C,4,256},
    {"CTaskLeanOnVehicle","Constructor",0x660F90,0,-1,256},
    {"CTaskLeanOnVehicle","Destructor",0x661030,0,-1,256},
    {"CTaskLeanOnVehicle","FinishAnimCB",0x661160,0,-1,256},
    {"CTaskLeanOnVehicle","MakeAbortable",0,0x86FAF4,6,512},
    {"CTaskLeanOnVehicle","ProcessPed",0,0x86FAF4,7,1024},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(auto& p:peds)clean(p.data()); for(auto& v:vehs)clean(v.data());
}
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctasks61a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctasks61a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctasks61a");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ctasks61a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    dllFinishCbRaw=reinterpret_cast<uintptr_t>(&CTaskLeanOnVehicle::FinishAnimCB); dllDefaultCbRaw=reinterpret_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB);
    dllFinishCb=Body(reinterpret_cast<uintptr_t>(&CTaskLeanOnVehicle::FinishAnimCB));
    dllDefaultCb=Body(reinterpret_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB));
    // registro de nombres de buffers (cmp=true: se comparan sus cambios)
    auto addBuf=[&](const std::string& nm,void* p,size_t n,bool cmp){ bufs.push_back({nm,static_cast<uint8*>(p),n,cmp}); };
    for(int n=0;n<NPEDS;++n){ const auto s=std::to_string(n); addBuf("ped#"+s,peds[n].data(),peds[n].size(),true); addBuf("pmat#"+s,pedMats[n].data(),pedMats[n].size(),true);
        addBuf("intel#"+s,intels[n].data(),intels[n].size(),true); addBuf("clump#"+s,clumps[n].data(),clumps[n].size(),false);
        for(int k=0;k<NBONES;++k){ addBuf("bone#"+s+"."+std::to_string(k),boneFrames[n][k].data(),0x20,false); addBuf("key#"+s+"."+std::to_string(k),keyFrames[n][k].data(),0x40,false); } }
    for(int n=0;n<NVEH;++n){ const auto s=std::to_string(n); addBuf("veh#"+s,vehs[n].data(),vehs[n].size(),true); addBuf("vmat#"+s,vehMats[n].data(),vehMats[n].size(),true);
        addBuf("hnd#"+s,handlings[n].data(),handlings[n].size(),true); }
    for(int n=0;n<NMI;++n){ const auto s=std::to_string(n); addBuf("mi#"+s,modelInfos[n].data(),0x80,false); addBuf("col#"+s,colModels[n].data(),0x40,false); addBuf("vstruct#"+s,vehStructs[n].data(),0x200,false); }
    for(int n=0;n<FAKES;++n)addBuf("fk#"+std::to_string(n),fakes[n].data(),fakes[n].size(),true);
    for(int n=0;n<NASSOC;++n)addBuf("assoc#"+std::to_string(n),assocs[n].data(),assocs[n].size(),true);
    addBuf("object",object.data(),object.size(),false); addBuf("util",utilFix.data(),utilFix.size(),true); addBuf("arena",arena.data(),arena.size(),true);
    addBuf("group",group.data(),group.size(),true); addBuf("pdata",playerData.data(),playerData.size(),true); addBuf("pad",pad.data(),pad.size(),false);
    addBuf("ride",rideData.data(),rideData.size(),false); addBuf("gevents",globalGroup.data(),globalGroup.size(),false); addBuf("held",heldObj.data(),heldObj.size(),true);
    addBuf("event",event.data(),event.size(),true); addBuf("vec",vecBuf.data(),vecBuf.size(),true);
    const int16 miSmoke=At<int16>(reinterpret_cast<void*>(0x8CD72C),0);
    FillTraps(fakeVtbl,std::make_integer_sequence<int,16>{}); FillTraps(pedVtbl,std::make_integer_sequence<int,40>{}); FillTraps(vehVtbl,std::make_integer_sequence<int,96>{});
    FillTraps(eventVtbl,std::make_integer_sequence<int,20>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    pedVtbl[13]=reinterpret_cast<void*>(&PTest); pedVtbl[23]=reinterpret_cast<void*>(&PMoveAnim);
    vehVtbl[28]=reinterpret_cast<void*>(&VOpenDoor); vehVtbl[38]=reinterpret_cast<void*>(&VMissing); vehVtbl[46]=reinterpret_cast<void*>(&VDrivingAnims); vehVtbl[47]=reinterpret_cast<void*>(&VRideData);
    eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occTaskStart=Occ(TaskHdr()), occEvStart=Occ(EventHdr());
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lt0=Last(TaskHdr()), le0=Last(EventHdr());
    const std::vector<uint8> stTask(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr())), stEv(States(EventHdr()),States(EventHdr())+Cap(EventHdr()));
    auto resetPools=[&]{ ResetPool(TaskHdr(),TASK_SLOT,lt0,stTask,occTaskStart); ResetPool(EventHdr(),EVENT_SLOT,le0,stEv,occEvStart); };
    // estado global tocado: reloj, fotogramas, paso, lluvia, camara, truco, pIgnoreEntity y los 4 punteros de modelo falsos
    std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{FRAMECTR,4},{TIMESTEP,4},{RAIN,4},{CAMPOS,16},{CAMMAT,4},{CAMACTIVE,1},{CHEAT,1},{IGNOREENT,4}};
    for(int k=0;k<3;++k)regions.push_back({CAMMODE+0x238*size_t(k),2});
    for(int k=0;k<NMI;++k)regions.push_back({uintptr_t(&CModelInfo::ms_modelInfoPtrs[MI_IDX[k]]),4});
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch s1(Body(MemberAddress(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FakeSay), s1e(0x5EFFE0,FakeSay);
        EntryPatch c1(Body(MemberAddress(&CPed::UpdateStatLeavingVehicle)),FakeStatLeave), c1e(0x5E01B0,FakeStatLeave);
        EntryPatch c2(Body(MemberAddress(&CPed::UpdateStatEnteringVehicle)),FakeStatEnter), c2e(0x5E01A0,FakeStatEnter);
        EntryPatch c3(Body(MemberAddress(&CPed::RestartNonPartialAnims)),FakeRestart), c3e(0x5DED50,FakeRestart);
        EntryPatch c4(Body(MemberAddress(&CPed::SetMoveState)),FakeMoveState), c4e(0x5DEC00,FakeMoveState);
        EntryPatch c5(Body(MemberAddress(&CPed::SetPedState)),FakePedState), c5e(0x5E4500,FakePedState);
        EntryPatch c6(Body(MemberAddress(&CPed::RemoveWeaponWhenEnteringVehicle)),FakeRemWeapon), c6e(0x5E6370,FakeRemWeapon);
        EntryPatch c7(Body(MemberAddress(&CPed::ReplaceWeaponWhenExitingVehicle)),FakeReplWeapon), c7e(0x5E6490,FakeReplWeapon);
        EntryPatch c8(Body(MemberAddress(&CPlayerPed::ClearAdrenaline)),FakeClearAdrenaline), c8e(0x60A070,FakeClearAdrenaline);
        EntryPatch c9(Body(MemberAddress(&CPlayerPed::GetPadFromPlayer)),FakeGetPad), c9e(0x609560,FakeGetPad);
        EntryPatch pa1(Body(MemberAddress(&CPad::GetAccelerate)),FakeAcc), pa1e(0x5403F0,FakeAcc);
        EntryPatch pa2(Body(MemberAddress(&CPad::GetSteeringLeftRight)),FakeSteer), pa2e(0x53FB80,FakeSteer);
        EntryPatch pa3(Body(MemberAddress(&CPad::GetBrake)),FakeBrake), pa3e(0x540080,FakeBrake);
        EntryPatch c10(Body(MemberAddress(&CPed::PositionPedOutOfCollision)),FakePosOut), c10e(0x5E0820,FakePosOut);
        EntryPatch c11(Body(MemberAddress(&CPed::GetEntityThatThisPedIsHolding)),FakeHolding), c11e(0x5E02E0,FakeHolding);
        EntryPatch c12(Body(MemberAddress(&CPed::SetPedPositionInCar)),FakeSetPosInCar), c12e(0x5DF910,FakeSetPosInCar);
        EntryPatch e1(Body(bit_cast<uintptr_t>(&CCarEnterExit::CarHasDoorToClose)),FakeHasDoor), e1e(0x64EE10,FakeHasDoor);
        EntryPatch e2(Body(bit_cast<uintptr_t>(&CCarEnterExit::RemoveCarSitAnim)),FakeRemSit), e2e(0x64F680,FakeRemSit);
        EntryPatch e3(Body(bit_cast<uintptr_t>(&CCarEnterExit::RemoveGetInAnims)),FakeRemGetIn), e3e(0x64F6E0,FakeRemGetIn);
        EntryPatch e4(Body(bit_cast<uintptr_t>(&CCarEnterExit::AddInCarAnim)),FakeAddInCar), e4e(0x64F720,FakeAddInCar);
        EntryPatch v1(Body(MemberAddress(&CVehicle::RemoveDriver)),FakeRemoveDriver), v1e(0x6D1950,FakeRemoveDriver);
        EntryPatch v2(Body(MemberAddress(&CVehicle::RemovePassenger)),FakeRemovePassenger), v2e(0x6D1610,FakeRemovePassenger);
        EntryPatch v3(Body(MemberAddress(&CVehicle::SetDriver)),FakeSetDriver), v3e(0x6D16A0,FakeSetDriver);
        EntryPatch v4(Body(MemberAddress(static_cast<bool(CVehicle::*)(CPed*)>(&CVehicle::AddPassenger))),FakeAddPass), v4e(0x6D13A0,FakeAddPass);
        EntryPatch v5(Body(MemberAddress(static_cast<bool(CVehicle::*)(CPed*,uint8)>(&CVehicle::AddPassenger))),FakeAddPassSeat), v5e(0x6D14D0,FakeAddPassSeat);
        EntryPatch v6(Body(MemberAddress(&CVehicle::ClearGettingOutFlags)),FakeClearOut), v6e(0x6D3060,FakeClearOut);
        EntryPatch v7(Body(MemberAddress(&CVehicle::ClearGettingInFlags)),FakeClearIn), v7e(0x6D3040,FakeClearIn);
        EntryPatch v8(Body(MemberAddress(&CVehicle::SetGettingOutFlags)),FakeSetOut), v8e(0x6D3020,FakeSetOut);
        EntryPatch v9(Body(MemberAddress(&CVehicle::IsLawEnforcementVehicle)),FakeIsLaw), v9e(0x6D2370,FakeIsLaw);
        EntryPatch v10(Body(MemberAddress(&CVehicle::ChangeLawEnforcerState)),FakeChangeLaw), v10e(0x6D2330,FakeChangeLaw);
        EntryPatch v11(Body(MemberAddress(&CVehicle::CanPedLeanOut)),FakeLean), v11e(0x6D5CF0,FakeLean);
        EntryPatch v12(Body(MemberAddress(static_cast<bool(CVehicle::*)(CPed*) const>(&CVehicle::IsPassenger))),FakeIsPassenger), v12e(0x6D1BD0,FakeIsPassenger);
        EntryPatch v13(Body(MemberAddress(&CVehicle::SetWindowOpenFlag)),FakeSetWindow), v13e(0x6D3080,FakeSetWindow);
        EntryPatch v14(Body(MemberAddress(&CVehicle::ClearWindowOpenFlag)),FakeClearWindow), v14e(0x6D30B0,FakeClearWindow);
        EntryPatch w1(Body(bit_cast<uintptr_t>(&CCrime::ReportCrime)),FakeCrime), w1e(0x532010,FakeCrime);
        EntryPatch w2(Body(bit_cast<uintptr_t>(&CGarages::IsPointWithinAnyGarage)),FakeGarage), w2e(0x448990,FakeGarage);
        EntryPatch w3(Body(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld)),FakeSphere), w3e(0x569E20,FakeSphere);
        EntryPatch w4(Body(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear)),FakeLos), w4e(0x56A490,FakeLos);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FakeGetAssoc), a1e(0x4D68B0,FakeGetAssoc);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FakeBlend), a2e(0x4D4610,FakeBlend);
        EntryPatch a3(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation))),FakeAddAnim), a3e(0x4D3AA0,FakeAddAnim);
        EntryPatch a4(Body(MemberAddress(&CAnimBlendAssociation::SetFinishCallback)),FakeSetFinishCb), a4e(0x4CEBE0,FakeSetFinishCb);
        EntryPatch a5(Body(MemberAddress(&CAnimBlendAssociation::SetDeleteCallback)),FakeSetDeleteCb), a5e(0x4CEBC0,FakeSetDeleteCb);
        EntryPatch k1(Body(MemberAddress(&IKChainManager_c::IsLooking)),FakeIsLooking), k1e(0x6181A0,FakeIsLooking);
        EntryPatch k2(Body(MemberAddress(&IKChainManager_c::LookAt)),FakeLookAt), k2e(0x618970,FakeLookAt);
        EntryPatch k3(Body(MemberAddress(&IKChainManager_c::AbortLookAt)),FakeAbortLookAt), k3e(0x618280,FakeAbortLookAt);
        EntryPatch q1(Body(MemberAddress(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),FakeEventAdd), q1e(0x4AB420,FakeEventAdd);
        EntryPatch q2(Body(MemberAddress(static_cast<bool(CPedGroupIntelligence::*)(CEvent*)>(&CPedGroupIntelligence::AddEvent))),FakeGroupAddEvent), q2e(0x5F7470,FakeGroupAddEvent);
        EntryPatch q3(Body(bit_cast<uintptr_t>(&CPedGroups::GetPedsGroup)),FakePedsGroup), q3e(0x5F7E80,FakePedsGroup);
        EntryPatch q4(Body(MemberAddress(&CPedGroupMembership::IsLeader)),FakeIsLeader), q4e(0x5F69C0,FakeIsLeader);
        EntryPatch q5(Body(MemberAddress(&CPedGroupMembership::GetLeader)),FakeGetLeader), q5e(0x5F69A0,FakeGetLeader);
        EntryPatch q6(Body(bit_cast<uintptr_t>(&GetEventGlobalGroup)),FakeGlobalGroup), q6e(0x4ABA50,FakeGlobalGroup);
        EntryPatch u1(Body(MemberAddress(&CAudioEngine::HasRadioRetuneJustStarted)),FakeRetune), u1e(0x506FE0,FakeRetune);
        EntryPatch u2(Body(MemberAddress(static_cast<bool(CUpsideDownCarCheck::*)(const CVehicle*)>(&CUpsideDownCarCheck::IsCarUpsideDown))),FakeUpside), u2e(0x463830,FakeUpside);
        EntryPatch u3(Body(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn)),FakeCoop), u3e(0x441390,FakeCoop);
        EntryPatch u4(Body(bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,CEntity&,CVector&)>(&CPedGeometryAnalyser::ComputeClosestSurfacePoint))),FakeClosest), u4e(0x5F3B70,FakeClosest);
        EntryPatch u5(0x7EB7C0,FakeQuatRotate);
        EntryPatch t1(Body(MemberAddress(&CTaskManager::FindActiveTaskByType)),FakeFindActive), t1e(0x681740,FakeFindActive);
        EntryPatch t2(Body(MemberAddress(&CTaskManager::FindTaskByType)),FakeFindTask), t2e(0x6817D0,FakeFindTask);
        EntryPatch t3(Body(MemberAddress(&CTaskUtilityLineUpPedWithCar::ProcessPed)),FakeUtilProcess), t3e(0x6513A0,FakeUtilProcess);
        EntryPatch t4(Body(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType)),FakeWander), t4e(0x673D00,FakeWander);
        // SetTask: el EXE llama a 0x681AF0; el DLL en linea llama a ChangeTaskInSlot
        EntryPatch m1(Body(TaskManagerAccess::ChangeInSlot()),FakeChangeInSlotDll), m1e(0x681AF0,FakeSetTaskExe);
        EntryPatch g1(Body(bit_cast<uintptr_t>(&CCarAI::GetCarToGoToCoors)),FakeGoToCoors0), g1e(0x41CE30,FakeGoToCoors0);
        EntryPatch g2(Body(bit_cast<uintptr_t>(&CCarAI::GetCarToGoToCoorsAccurate)),FakeGoToCoors1), g2e(0x41D0E0,FakeGoToCoors1);
        EntryPatch g3(Body(bit_cast<uintptr_t>(&CCarAI::GetCarToGoToCoorsStraightLine)),FakeGoToCoors2), g3e(0x41CFB0,FakeGoToCoors2);
        EntryPatch g4(Body(bit_cast<uintptr_t>(&CCarAI::GetCarToGoToCoorsRacing)),FakeGoToCoors3), g4e(0x41D210,FakeGoToCoors3);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    resetPools();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6161u; if(!st)st=1; nrec=0; seed=uint32(i); rngCalls=0;
                    mk.fill(0); ck.fill(0); findIdx=assocIdx=0; newCount=delCount=0; newSize=delOff=0;
                    CleanRefs(); restoreGlobals();
                    // reloj, fotogramas, paso, lluvia, camara, truco
                    const uint32 now= next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=now;
                    At<uint32>(reinterpret_cast<void*>(FRAMECTR),0)=next();
                    At<float>(reinterpret_cast<void*>(TIMESTEP),0)=next()%10==0 ? 0.f : uni(0.1f,8.f);
                    { const uint32 m=next()%5; At<float>(reinterpret_cast<void*>(RAIN),0)=m==0 ? 0.f : m==1 ? qnan : m==2 ? -0.5f : uni(0.01f,1.f); }
                    At<CVector>(reinterpret_cast<void*>(CAMPOS),0)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,8.f)}; At<uint32>(reinterpret_cast<void*>(CAMMAT),0)=0;
                    At<uint8>(reinterpret_cast<void*>(CAMACTIVE),0)=uint8(next()%3);
                    for(int k=0;k<3;++k)At<uint16>(reinterpret_cast<void*>(CAMMODE+0x238*size_t(k)),0)=next()%2 ? 0x31 : uint16(next()%60);
                    At<uint8>(reinterpret_cast<void*>(CHEAT),0)=uint8(next()%2);
                    for(auto& m:mk)m=next();
                    // modelos falsos: colision con radio y estructura de vehiculo con los dummies (asiento delantero +0x30, apoyabrazos +0x78)
                    for(int k=0;k<NMI;++k){
                        std::memset(modelInfos[k].data(),0,0x80); std::memset(colModels[k].data(),0,0x40); std::memset(vehStructs[k].data(),0,0x200);
                        At<void*>(modelInfos[k].data(),0x14)=colModels[k].data(); At<void*>(modelInfos[k].data(),0x5C)=vehStructs[k].data();
                        { const uint32 m=next()%6; At<float>(colModels[k].data(),0x24)=m==0 ? 0.f : m==1 ? qnan : uni(0.3f,6.f); }
                        auto dummy=[&](size_t idx){ const uint32 m=next()%5; At<CVector>(vehStructs[k].data(),idx*12)=m==0 ? CVector{0.f,0.f,0.f} : CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.5f,1.f)}; };
                        dummy(4); dummy(10);
                        // z del apoyabrazos respecto al asiento cerca de las ventanas de aceptacion (0.39-0.46 y 0.4-0.44)
                        if(next()%3==0){ At<CVector>(vehStructs[k].data(),10*12)=CVector{0.3f,0.2f,uni(0.3f,0.6f)}; At<CVector>(vehStructs[k].data(),4*12)=CVector{0.1f,0.1f,uni(0.f,0.1f)}; }
                        CModelInfo::ms_modelInfoPtrs[MI_IDX[k]]=reinterpret_cast<CBaseModelInfo*>(modelInfos[k].data());
                    }
                    // vehiculos: vtable con trampas, matriz, velocidad, tipo, estado, autopiloto, banderas, puertas, conductor y pasajeros
                    bool anyBoat=false;
                    for(int n=0;n<NVEH;++n){
                        auto* v=vehs[n].data(); std::memset(v,0,vehs[n].size()); std::memset(vehMats[n].data(),0,vehMats[n].size()); std::memset(handlings[n].data(),0,H_SIZE);
                        At<void*>(v,0)=vehVtbl.data(); v[E_TYPE]=uint8(2|(next()%14)<<3); At<void*>(v,E_MATRIX)=vehMats[n].data(); At<uint32>(v,E_FLAGS)=0x80;
                        for(int r=0;r<3;++r)At<CVector>(vehMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        At<CVector>(vehMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                        At<int16>(v,E_MODEL)=int16(MI_IDX[next()%NMI]);
                        { const uint32 m=next()%5; const float s=m==0 ? 0.01f : m==1 ? 0.5f : m==2 ? 0.f : m==3 ? 1.2f : uni(0.f,0.2f);
                          At<CVector>(v,PH_SPEED)=CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; if(next()%12==0)At<float>(v,PH_SPEED)=qnan; }
                        for(auto& b:handlings[n])b=uint8(next());
                        { const uint32 m=next()%6; handlings[n][H_ANIMGROUP]=m==0 ? uint8(next()%30) : uint8(next()%30); }
                        At<void*>(v,V_HANDLING)=handlings[n].data();
                        At<uint32>(v,V_TIMESTART)=next(); v[V_STYLE]=uint8(next()%5); { static const int8 ms[]{0,1,2,3,4,5,6,15,16,9,20}; v[V_MISSION]=uint8(ms[next()%std::size(ms)]); }
                        v[V_SPEED]=next()%3==0 ? 0 : uint8(next());
                        { const uint32 m=next()%3; At<void*>(v,V_TARGET)=m==0 ? nullptr : m==1 ? (void*)Veh(int(next()%NVEH)) : (void*)Ped(int(next()%NPEDS)); }
                        for(size_t b=0;b<8;++b)v[V_FLAGS+b]=uint8(next());
                        At<uint16>(v,V_ALARM)=next()%3==0 ? 0xFFFFu : uint16(next());
                        At<CPed*>(v,V_DRIVER)=next()%3==0 ? nullptr : Ped(int(next()%NPEDS));
                        for(int s=0;s<8;++s)At<CPed*>(v,V_PASS+4*s)=next()%3==0 ? nullptr : Ped(int(next()%NPEDS));
                        v[V_NUMPASS]=uint8(next()%4); v[V_GETIN+0]=uint8(next()%4); v[V_GETOUT]=uint8(next()%4); v[0x485]=uint8(next()%4); v[V_BITS]=uint8(next());
                        { const uint32 m=next()%4; At<uint32>(v,V_LOCK)=m==0 ? 5u : m==1 ? 1u : next()%8; }
                        { const uint32 m=next()%10; At<int32>(v,V_TYPE)=m<5 ? 0 : m==5 ? 5 : m==6 ? 9 : m==7 ? 6 : m==8 ? 2 : 3; if(At<int32>(v,V_TYPE)==5)anyBoat=true; }
                        { const uint32 m=next()%8; At<int32>(v,V_SUBTYPE)=m<3 ? 0 : m==3 ? 2 : m==4 ? 5 : m==5 ? 6 : m==6 ? 9 : 0; }
                        for(size_t b=0;b<6;++b)v[V_DOORS+b]=uint8(next()%5); // estados de puerta del gestor de danos (+0x5A0 + 9 + puerta)
                        v[V_BIKE]=uint8(next()); for(size_t b=0;b<0x18;++b)v[V_TRAINDOOR+b]=uint8(next());
                        At<CEntity*>(v,PH_ATTACHED)=next()%4==0 ? (CEntity*)Veh(int(next()%NVEH)) : nullptr;
                        // pasajeros: el ped en el hueco 1 a veces
                    }
                    // peatones: matriz (casi siempre; siempre si hay un barco), inteligencia, clump, huesos, tipo, flags, armas, vehiculo
                    for(int n=0;n<NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(pedMats[n].data(),0,pedMats[n].size()); std::memset(intels[n].data(),0,intels[n].size());
                        std::memset(clumps[n].data(),0,0x40); clumps[n][0]=2;
                        At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3; At<void*>(p,P_INTEL)=intels[n].data(); At<void*>(p,E_RWOBJ)=clumps[n].data(); At<void*>(p,P_PDATA)=playerData.data();
                        if(anyBoat||next()%6){ At<void*>(p,E_MATRIX)=pedMats[n].data();
                            for(int r=0;r<3;++r)At<CVector>(pedMats[n].data(),0x10*r)=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            At<CVector>(pedMats[n].data(),0x30)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; }
                        else { At<CVector>(p,E_POS)=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)}; At<float>(p,E_HEADING)=uni(-3.2f,3.2f); }
                        At<uint32>(p,E_FLAGS)=0x80u|(next()%2);
                        At<uint16>(p,E_SEED)=uint16(next()); At<int16>(p,E_MODEL)=int16(MI_IDX[next()%NMI]);
                        { const uint32 m=next()%4; const float s=m==0 ? 0.f : uni(0.f,0.3f); At<CVector>(p,PH_SPEED)=CVector{s*uni(-1.f,1.f),s*uni(-1.f,1.f),s*uni(-1.f,1.f)}; }
                        At<uint32>(p,P_FLAGS)=(next()%4==0 ? 1u : 0u)|(next()%2 ? IN_VEH : 0u)|(next()%2 ? RENDER_IN_CAR : 0u);
                        At<uint32>(p,P_FLAGS2)=next()%2 ? 0x4000u : 0u; At<uint32>(p,P_FLAGS3)=next()%2 ? 8u : 0u; At<uint32>(p,P_FLAGS4)=next()%2 ? 0x100000u : 0u;
                        p[P_CREATEDBY]=uint8(next()%4);
                        { static const int32 types[]{0,1,4,6,7,9,10}; At<int32>(p,P_PEDTYPE)=types[next()%std::size(types)]; }
                        At<float>(p,P_ROT)=uni(-3.f,3.f); At<float>(p,P_AIMROT)=uni(-3.f,3.f); At<int32>(p,P_STATE)=int32(next()%60);
                        At<CEntity*>(p,P_STANDING)=next()%4==0 ? (CEntity*)Veh(int(next()%NVEH)) : nullptr;
                        { const uint32 m=next()%5; At<CVehicle*>(p,P_VEH)=m==0 ? nullptr : Veh(int(next()%NVEH)); }
                        { const uint32 m=next()%4; At<int32>(p,P_WEAPON4)=m==0 ? 0 : int32(next()%47); const uint32 a=next()%4; At<int32>(p,P_WEAPON4+0xC)=a==0 ? 0 : a==1 ? -5 : int32(next()%500); }
                        for(int k=0;k<NBONES;++k){ std::memset(boneFrames[n][k].data(),0,0x20); std::memset(keyFrames[n][k].data(),0,0x40);
                            At<void*>(boneFrames[n][k].data(),0x10)=keyFrames[n][k].data(); At<void*>(p,P_BONES+4*k)=boneFrames[n][k].data(); }
                        // inteligencia: gestor de tareas (puestos vacios), grupo de eventos y escaneres de vehiculos y peatones
                        auto* in=intels[n].data();
                        At<void*>(in,I_EVGROUP)=p;
                        for(int s=0;s<16;++s){ At<void*>(in,I_VEHENTS+4*s)=next()%3==0 ? (void*)Veh(int(next()%NVEH)) : nullptr; At<void*>(in,I_PEDENTS+4*s)=next()%3==0 ? (void*)Ped(int(next()%NPEDS)) : nullptr; }
                    }
                    std::memset(playerData.data(),0,playerData.size());
                    // peatones 0..4 consistentes: jugador devuelto por FindPlayerPed y otros detalles
                    { const uint32 m=next()%5; playerRet=m<2 ? uintptr_t(Ped(0)) : m<4 ? uintptr_t(Ped(int(1+next()%(NPEDS-1)))) : uintptr_t(Ped(1)); }
                    { const uint32 m=next()%3; pedsGroupRet=m==0 ? 0 : uintptr_t(group.data()); }
                    leaderRet=next()%3==0 ? 0 : uintptr_t(Ped(int(next()%NPEDS)));
                    padRet=next()%4==0 ? 0 : uintptr_t(pad.data());
                    for(int k=0;k<3;++k){ const uint32 m=next()%3; padVal[k]=m==0 ? int16(0) : int16(int32(next()%255)-127); }
                    sphereRet0=next()%2 ? uintptr_t(Ped(int(next()%NPEDS))) : uintptr_t(Veh(int(next()%NVEH))); sphereRet1=next()%3==0 ? uintptr_t(Veh(0)) : uintptr_t(Ped(1));
                    closestVec=CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,6.f)};
                    ridePtrNull=next()%3==0;
                    // grupo falso (evento actual y miembros), objeto sostenido, datos de montar, utilidad, asociaciones
                    std::memset(group.data(),0,group.size()); At<void*>(group.data(),G_CURRENT)=next()%2 ? nullptr : (void*)Fake(7);
                    for(int s=0;s<8;++s)At<CPed*>(group.data(),G_MEMBERS+4*s)=next()%2 ? Ped(int(next()%NPEDS)) : nullptr;
                    std::memset(heldObj.data(),0,heldObj.size()); heldObj[E_TYPE]=2; At<int16>(heldObj.data(),E_MODEL)=next()%2 ? miSmoke : int16(MI_IDX[next()%NMI]); At<uint32>(heldObj.data(),E_FLAGS)=0x80;
                    holdRet=uintptr_t(heldObj.data());
                    for(auto& b:rideData)b=uint8(next()); At<int32>(rideData.data(),0)=int32(next()%60);
                    for(auto& b:utilFix)b=uint8(next()); At<int32>(utilFix.data(),0x14)=int32(next()%4); At<int32>(utilFix.data(),0x18)=int32(next()%4);
                    for(auto& b:arena)b=uint8(next());
                    for(auto& a:assocs){ for(auto& b:a)b=uint8(next()); At<float>(a.data(),0x1C)=uni(-5.f,5.f); At<float>(a.data(),0x20)=uni(0.f,1.f); At<float>(a.data(),0x24)=uni(0.f,3.f);
                        static const int16 ids[]{0x12F,0x130,0x131,0x182,0xA9,0xA2,0xA3}; At<int16>(a.data(),0x2C)=ids[next()%std::size(ids)]; }
                    for(auto& b:globalGroup)b=uint8(next()); for(auto& b:pad)b=uint8(next());
                    // tareas falsas (la 0..2 hacen de tareas de conducir/ir a punto: campos +0x10.. y +0x20/+0x28 aleatorios)
                    for(int n=0;n<FAKES;++n){ auto* k=fakes[n].data(); for(size_t b=0;b<FAKE_SIZE;++b)k[b]=uint8(next());
                        At<void*>(k,0)=fakeVtbl.data(); At<uint32>(k,4)=0; At<uint32>(k,8)=0;
                        for(size_t w=0xC;w<0x24;w+=4)At<float>(k,w)=uni(-20.f,20.f);
                        At<int32>(k,0x20)=next()%3==0 ? -1 : int32(now-next()%20000); At<int32>(k,0x28)=next()%8==0 ? -int32(1+next()%500) : int32(100+next()%3000);
                        At<int32>(k,FK_TYPE)=TASK_SIMPLE_PAUSE; At<uint32>(k,FK_ABORT)=next(); At<uint32>(k,FK_CNT)=0; At<uint32>(k,FK_CLONE)=0; At<uint32>(k,FK_SUB)=0; }
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data();
                    std::memset(vecBuf.data(),0,vecBuf.size());
                    std::memset(object.data(),0xCD,object.size());

                    auto* o=object.data(); auto* p0=Ped(0); auto* p0b=peds[0].data();
                    auto setObj=[&](uintptr_t vtbl,size_t size){ for(size_t b=0;b<size;++b)o[b]=uint8(next()); At<uintptr_t>(o,0)=vtbl; At<uint32>(o,4)=0; At<uint32>(o,8)=0; };
                    auto timer=[&](size_t off){ At<uint32>(o,off)=next()%2 ? now-next()%30000 : now; At<int32>(o,off+4)=next()%8==0 ? -1 : int32(next()%20000); o[off+8]=uint8(next()%4!=0); o[off+9]=uint8(next()%8==0); };
                    auto prio=[&]{ return int32(next()%4); };
                    auto occT=Occ(TaskHdr()), occE=Occ(EventHdr());
                    auto pickVeh=[&]()->CVehicle*{ return Veh(int(next()%NVEH)); };
                    auto pickPed=[&]()->CPed*{ return Ped(int(next()%NPEDS)); };
                    // el peatón 0 va (casi) siempre en un vehiculo y el EXE lo desreferencia sin guarda cuando hay bInVehicle
                    auto needVeh=[&]{ if(!At<CVehicle*>(p0b,P_VEH))At<CVehicle*>(p0b,P_VEH)=pickVeh(); };
                    auto ensureVeh=[&]{ if(At<uint32>(p0b,P_FLAGS)&IN_VEH)needVeh(); };

                    // ---- tarea de conducir: +0x8 vehiculo, +0xC animacion, +0x10 utilidad, +0x14 temporizador de espera, bopping y banderas
                    auto setCd=[&](uintptr_t vt=VT_CD,size_t size=0x60){ setObj(vt,size);
                        CVehicle* tv=next()%12==0 ? nullptr : pickVeh(); At<CVehicle*>(o,8)=tv; Reg(tv,o+8);
                        At<void*>(o,0xC)=next()%3==0 ? (void*)assocs[next()%NASSOC].data() : nullptr;
                        At<void*>(o,0x10)=next()%3==0 ? (void*)arena.data() : nullptr;
                        timer(0x14);
                        At<int32>(o,0x20)=next()%3==0 ? -1 : int32(now-next()%20000); At<int32>(o,0x24)=int32(next());
                        At<int32>(o,0x28)=next()%8==0 ? -int32(1+next()%500) : int32(100+next()%3000);
                        { const uint32 m=next()%6; At<float>(o,0x2C)=m==0 ? 0.f : m==1 ? 0.999f : m==2 ? qnan : uni(0.f,1.f); }
                        At<int32>(o,0x30)=int32(next()%3)-1; At<int32>(o,0x34)=int32(now-next()%9000); At<int32>(o,0x38)=int32(next()%5)-1;
                        { const uint32 m=next()%5; At<float>(o,0x3C)=m==0 ? 0.f : m==1 ? 1.f : uni(-1.f,1.f); }
                        At<float>(o,0x40)=uni(3.f,8.f);
                        { const uint32 m=next()%7; At<float>(o,0x44)=m==0 ? 0.f : m==1 ? 1.f : m==2 ? qnan : m==3 ? -0.2f : m==4 ? 1.3f : uni(0.f,1.f); }
                        At<int32>(o,0x48)=int32(now-next()%9000); At<int32>(o,0x4C)=next()%3==0 ? int32(next()%4000) : int32(next()%1500);
                        timer(0x50); o[0x5C]=uint8(next());
                        // ProcessBopping y compañia: el bucle de bopping solo funciona con vehiculo propio del peatón
                        if(next()%3==0)Reg(At<CVehicle*>(p0b,P_VEH),p0b+P_VEH);
                    };

                    // argumentos preparados antes de la instantanea
                    int32 a0{}, a1{}, a2{}, a3{}; float f0{}, f1{}; uint32 b0{}, b1{}, b2{}; void* q0{}; void* q1{}; void* q2{}; const CEvent* ev{};
                    size_t objSize=0x60;
                    const uint32 frameFor0 = [&]{ return uint32(0x4000u*(1+next()%100)-At<uint16>(p0b,E_SEED)); }();

                    if(cls=="CTaskSimpleCarDrive"||cls=="CTaskSimpleCarDriveTimed") {
                        const bool timed=cls=="CTaskSimpleCarDriveTimed"; objSize=timed ? 0x70 : 0x60;
                        const uintptr_t vt= timed ? VT_DT : VT_CD;
                        if(nm=="Constructor"){ q0=next()%3 ? nullptr : (void*)utilFix.data(); q1=next()%6 ? (void*)pickVeh() : nullptr; b0=next()%2; a0=next()%5==0 ? -1 : int32(next()%5000); }
                        else {
                            setCd(vt,objSize);
                            if(timed){ At<int32>(o,0x60)=next()%4==0 ? -1 : int32(next()%5000); timer(0x64); }
                            if(nm=="Destructor"){ if(next()%2){ o[0x5C]|=0x20; if(!At<void*>(o,0xC))At<void*>(o,0xC)=assocs[0].data(); } else o[0x5C]&=uint8(~0x20); }
                            if(nm=="MakeAbortable"){ a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; if(!At<CVehicle*>(o,8))At<CVehicle*>(o,8)=pickVeh(); needVeh();
                                if(next()%4)At<uint32>(p0b,P_FLAGS)|=IN_VEH; }
                            if(nm=="SetPedPosition"||nm=="ProcessPed"||nm=="StartBopping"||nm=="ProcessHeadBopping"||nm=="ProcessArmBopping"||nm=="ProcessBopping"||nm=="TriggerIK"){
                                if(nm!="TriggerIK"&&nm!="SetPedPosition"&&nm!="ProcessPed")needVeh();
                                if(nm=="TriggerIK"&&next()%6)needVeh();
                                if(nm=="ProcessPed"){
                                    ensureVeh();
                                    if(At<CVehicle*>(o,8)&&next()%2)At<CVehicle*>(o,8)=At<CVehicle*>(p0b,P_VEH) ? At<CVehicle*>(p0b,P_VEH) : At<CVehicle*>(o,8);
                                    if(next()%3==0)At<uint32>(reinterpret_cast<void*>(FRAMECTR),0)=frameFor0;
                                    if(next()%3)At<uint32>(p0b,P_FLAGS)|=IN_VEH;
                                    if(!(At<uint32>(p0b,P_FLAGS)&IN_VEH)&&At<CVehicle*>(o,8)&&next()%3==0)At<uint32>(p0b,P_FLAGS)|=IN_VEH;
                                    ensureVeh();
                                    // jugador al volante (peatón 0 de tipo jugador y conductor de su vehículo) y tren con puerta
                                    if(next()%3==0){ At<int32>(p0b,P_PEDTYPE)=int32(next()%2); if(At<CVehicle*>(o,8))At<CPed*>(At<CVehicle*>(o,8),V_DRIVER)=p0; }
                                    if(next()%5==0&&At<CVehicle*>(p0b,P_VEH))At<int32>(At<CVehicle*>(p0b,P_VEH),V_TYPE)=6;
                                }
                                if(nm=="SetPedPosition"){ if(next()%3)needVeh(); if(next()%2)At<uint32>(p0b,P_FLAGS)|=IN_VEH; if(next()%2)At<CVehicle*>(o,8)=At<CVehicle*>(p0b,P_VEH); }
                                if(nm=="StartBopping"||nm=="ProcessHeadBopping"||nm=="ProcessArmBopping"||nm=="ProcessBopping"){
                                    if(next()%3==0)At<int32>(o,0x28)=int32(100+next()%3000);
                                    // el vehiculo de la tarea debe tener modelo falso y el del peatón tambien (siempre lo tienen)
                                    if(next()%2)At<CVehicle*>(o,8)=At<CVehicle*>(p0b,P_VEH);
                                    if(!At<CVehicle*>(o,8))At<CVehicle*>(o,8)=At<CVehicle*>(p0b,P_VEH);
                                    if(nm=="ProcessArmBopping"||nm=="ProcessBopping"){ if(next()%2)At<CPed*>(At<CVehicle*>(p0b,P_VEH),V_DRIVER+4*int(next()%4))=p0; if(next()%3==0)At<int32>(p0b,P_PEDTYPE)=int32(next()%2); }
                                    if(nm=="ProcessBopping"&&next()%2)At<CPed*>(At<CVehicle*>(p0b,P_VEH),V_DRIVER)=p0;
                                }
                            }
                            if(nm=="StartBopping"||nm=="ProcessHeadBopping"||nm=="ProcessArmBopping"||nm=="ProcessBopping"){
                                a0=int32(next()%2); f0=next()%5==0 ? 64.f : next()%7==0 ? qnan : next()%3==0 ? uni(0.f,70.f) : uni(0.f,200.f); b0=next()%2;
                                if(nm=="ProcessBopping"||nm=="StartBopping")At<int32>(o,0x20)=next()%2 ? -1 : At<int32>(o,0x20);
                                if(nm=="ProcessArmBopping"&&next()%3==0)At<uint32>(reinterpret_cast<void*>(RAIN),0)=0;
                            }
                        }
                    }
                    else if(cls=="CTaskSimpleCarSetPedInAsDriver") {
                        objSize=0x1C;
                        if(nm=="Constructor"){ q0=next()%6 ? (void*)pickVeh() : nullptr; q1=next()%3 ? nullptr : (void*)utilFix.data(); }
                        else {
                            setObj(VT_DRV,0x1C); CVehicle* tv=pickVeh(); At<CVehicle*>(o,0x10)=tv; Reg(tv,o+0x10);
                            At<void*>(o,0xC)=nullptr; o[8]=uint8(next()%2); At<void*>(o,0x14)=next()%2 ? (void*)utilFix.data() : nullptr;
                            o[0x18]=uint8(next()%3==0); o[0x19]=uint8(next()%3==0 ? 0 : next()%4); o[0x1A]=uint8(next()%3==0 ? 0 : next()%3);
                            if(nm=="ProcessPed"){ if(next()%5==0)At<CPed*>(tv,V_DRIVER)=nullptr; if(next()%5==0)At<CPed*>(tv,V_DRIVER)=p0; if(next()%2)At<CPed*>(tv,V_DRIVER)=Ped(1);
                                if(next()%2)At<void*>(p0b,P_VEH)=nullptr; if(next()%3==0)At<int32>(p0b,P_PEDTYPE)=int32(next()%2);
                                if(next()%4==0)At<uint32>(p0b,P_FLAGS)|=IN_VEH; }
                            if(nm=="MakeAbortable"){ a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                        }
                    }
                    else if(cls=="CTaskSimpleCarSetPedInAsPassenger") {
                        objSize=0x20;
                        {
                            setObj(VT_PSG,0x20); CVehicle* tv=pickVeh(); At<CVehicle*>(o,0x10)=tv; Reg(tv,o+0x10);
                            o[8]=uint8(next()%2); At<void*>(o,0xC)=nullptr; { static const int32 dr[]{0,8,9,10,11,18}; At<int32>(o,0x14)=dr[next()%std::size(dr)]; }
                            At<void*>(o,0x18)=next()%2 ? (void*)utilFix.data() : nullptr; o[0x1C]=uint8(next()%3==0); o[0x1D]=uint8(next()%3==0 ? 0 : next()%4); o[0x1E]=uint8(next()%3==0 ? 0 : next()%3);
                            if(nm=="ProcessPed"){ if(next()%2)At<void*>(p0b,P_VEH)=nullptr; if(next()%3==0)At<int32>(p0b,P_PEDTYPE)=int32(next()%2); if(next()%4==0)At<uint32>(p0b,P_FLAGS)|=IN_VEH;
                                if(next()%3==0)At<void*>(o,4)=Fake(1); At<int32>(fakes[1].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; }
                            if(nm=="MakeAbortable"){ a0=prio(); ev=next()%2 ? reinterpret_cast<const CEvent*>(event.data()) : nullptr; }
                        }
                    }
                    else if(cls=="CTaskSimpleCarSetPedOut") {
                        objSize=0x18;
                        if(nm=="PositionPedOutOfCollision"){ q0=p0; q1=next()%3==0 ? nullptr : (void*)pickVeh(); a0=int32(next()%20)-1; if(!At<CVehicle*>(p0b,P_VEH)&&next()%4)At<CVehicle*>(p0b,P_VEH)=pickVeh(); }
                        else {
                            setObj(VT_OUT,0x18); CVehicle* tv=pickVeh(); At<CVehicle*>(o,8)=tv; Reg(tv,o+8);
                            { static const int32 dr[]{0,8,9,10,11,18}; At<int32>(o,0xC)=dr[next()%std::size(dr)]; }
                            for(size_t b=0x10;b<0x16;++b)o[b]=uint8(next()%3==0);
                            o[0x14]=uint8(next()%3==0 ? 0 : next()%4); o[0x15]=uint8(next()%3==0 ? 0 : next()%3);
                            if(nm=="ProcessPed"){ if(next()%4)needVeh(); if(next()%3==0)At<int32>(p0b,P_PEDTYPE)=int32(next()%7); }
                            if(nm=="MakeAbortable"){ a0=prio(); ev=nullptr; }
                        }
                    }
                    else if(cls=="CTaskComplexDriveToPoint") {
                        objSize=0x3C;
                        // +0x8 subtarea, +0xC vehiculo, +0x10 velocidad, +0x14 modelo, +0x18 estilo, +0x1D..0x20 originales, +0x24 punto, +0x30 modo, +0x34 radio, +0x38 byte
                        auto setDtp=[&]{ setObj(VT_DTP,0x3C); At<CTask*>(o,8)=next()%5 ? Fake(0) : nullptr; CVehicle* tv=pickVeh(); At<CVehicle*>(o,0xC)=tv; Reg(tv,o+0xC);
                            { const uint32 m=next()%5; At<float>(o,0x10)=m==0 ? 0.f : m==1 ? -5.f : m==2 ? 300.f : uni(1.f,100.f); }
                            At<int32>(o,0x14)=int32(next()%3==0 ? -1 : 400+next()%100); At<int32>(o,0x18)=int32(next()%5);
                            { const CVector pt=At<CVector>(vehMats[VehIdx(tv)].data(),0x30); At<CVector>(o,0x24)=next()%3==0 ? CVector{pt.x+uni(-4.f,4.f),pt.y+uni(-4.f,4.f),pt.z+uni(-2.f,2.f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,6.f)}; }
                            At<int32>(o,0x30)=next()%8==0 ? int32(next()%8) : int32(next()%4);
                            { const uint32 m=next()%5; At<float>(o,0x34)=m==0 ? 0.f : m==1 ? 3.f : m==2 ? 6.f : m==3 ? qnan : uni(0.f,8.f); }
                            o[0x38]=uint8(next()%2); };
                        if(nm=="Constructor"){ q0=pickVeh(); { const CVector pt{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,6.f)}; At<CVector>(vecBuf.data(),0)=pt; } f0=uni(0.f,20.f); a0=int32(next()%5); a1=int32(next()%3==0 ? -1 : 400+next()%100); f1=uni(0.f,8.f); a2=int32(next()%5); }
                        else { setDtp();
                            if(nm=="Drive"||nm=="IsTargetBlocked-"||nm=="IsTargetBlocked-entities"){
                                if(!At<CVehicle*>(p0b,P_VEH)||next()%3)At<CVehicle*>(p0b,P_VEH)=pickVeh();
                                // el peatón cerca del destino la mitad de las veces (cerca del vehiculo de la tarea)
                                if(next()%2&&At<void*>(p0b,E_MATRIX)){ const CVector pt=At<CVector>(o,0x24); At<CVector>(pedMats[0].data(),0x30)=CVector{pt.x+uni(-5.f,5.f),pt.y+uni(-5.f,5.f),pt.z+uni(-2.f,2.f)}; }
                                if(next()%2){ const CVector pt=At<CVector>(o,0x24); auto* vm=At<void*>(p0b,P_VEH) ? vehMats[VehIdx(At<CVehicle*>(p0b,P_VEH))].data() : nullptr; if(vm)At<CVector>(vm,0x30)=CVector{pt.x+uni(-3.f,3.f),pt.y+uni(-3.f,3.f),pt.z+uni(-1.f,1.f)}; }
                                // entidades del escaner cerca del destino
                                for(int s=0;s<16;++s)if(next()%2){ const CVector pt=At<CVector>(o,0x24); void* e=At<void*>(intels[0].data(),I_PEDENTS+4*s); void* m=e ? (e>=peds[0].data()&&e<peds[NPEDS-1].data()+sizeof(CPed) ? (void*)pedMats[(static_cast<uint8*>(e)-peds[0].data())/sizeof(CPed)].data() : nullptr) : nullptr;
                                    if(m)At<CVector>(m,0x30)=CVector{pt.x+uni(-3.f,3.f),pt.y+uni(-3.f,3.f),pt.z}; }
                            }
                            if(nm=="IsTargetBlocked-entities"){ q0=p0; q1=intels[0].data()+(next()%2 ? I_PEDENTS : I_VEHENTS); a0=next()%8==0 ? int32(next()%3)-1 : 16; }
                            if(nm=="CreateSubTaskCannotGetInCar"){ }
                        }
                    }
                    else if(cls=="CTaskComplexEnterCarAsPassengerWait") {
                        objSize=0x24;
                        if(nm=="Constructor"){ q0=next()%6 ? (void*)pickVeh() : nullptr; q1=next()%3 ? (void*)pickPed() : nullptr; b0=next()%2; a0=int32(next()%8); }
                        else { setObj(VT_EPW,0x24);
                            At<CTask*>(o,8)=Fake(0); CVehicle* tv=next()%8==0 ? nullptr : pickVeh(); At<CVehicle*>(o,0xC)=tv; Reg(tv,o+0xC);
                            CPed* wp=next()%3==0 ? nullptr : pickPed(); At<CPed*>(o,0x10)=wp; Reg(wp,o+0x10);
                            At<uint32>(o,0x14)=next()%3==0 ? uint32(14+next()%4) : uint32(next()%20); o[0x18]=uint8(next()%2); At<int32>(o,0x1C)=int32(next()%8); o[0x20]=uint8(next()%3==0);
                            { static const int32 ty[]{TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED,TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_FINISHED,TASK_NONE};
                              At<int32>(fakes[0].data(),FK_TYPE)=ty[next()%std::size(ty)]; }
                            if(nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="CreateSubTask"||nm=="ControlSubTask"){
                                if(next()%3==0&&tv&&At<CVehicle*>(p0b,P_VEH)!=tv)At<CVehicle*>(p0b,P_VEH)=tv;
                                if(next()%2)At<uint32>(p0b,P_FLAGS)|=IN_VEH; }
                            if(nm=="CreateSubTask"){ static const int32 ty[]{TASK_SIMPLE_CAR_DRIVE_TIMED,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED,TASK_COMPLEX_SEQUENCE,TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_FINISHED,TASK_NONE,1000};
                                a0=ty[next()%std::size(ty)]; if(!tv)At<CVehicle*>(o,0xC)=pickVeh(); if(!At<CPed*>(o,0x10)&&a0==TASK_SIMPLE_WAIT_UNTIL_PED_IN_CAR&&next()%2)At<CPed*>(o,0x10)=pickPed(); }
                            if(nm=="ControlSubTask"){ At<int32>(fakes[0].data(),FK_TYPE)=next()%4 ? int32(TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL) : int32(TASK_SIMPLE_PAUSE);
                                if(!At<CVehicle*>(o,0xC)&&next()%3)At<CVehicle*>(o,0xC)=pickVeh();
                                // el punto objetivo de la subtarea cerca del punto de superficie mas cercano la mitad de las veces
                                if(next()%2)At<CVector>(fakes[0].data(),0x10)=closestVec; }
                            if(nm=="CreateNextSubTask"){ if(next()%4==0)At<int32>(fakes[0].data(),FK_TYPE)=TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_TIMED; }
                        }
                    }
                    else if(cls=="CTaskComplexOpenPassengerDoor") {
                        objSize=0x50;
                        if(nm=="Constructor"){ q0=next()%6 ? (void*)pickVeh() : nullptr; a0=int32(next()%6); }
                        else { setObj(VT_OPD,0x50); CVehicle* tv=next()%8==0 ? nullptr : pickVeh(); At<CVehicle*>(o,0xC)=tv; Reg(tv,o+0xC); At<int32>(o,0x1C)=int32(next()%6); At<int32>(o,0x24)=int32(next()%8);
                            // el destructor del EXE (0x63DFA0) borra la utilidad (+0x38), limpia el vehiculo y los flags (+0x28, +0x44) y la subtarea (+0x8)
                            At<void*>(o,0x38)=next()%3==0 ? (void*)arena.data() : nullptr; o[0x28]=uint8(next()%3); o[0x44]=uint8(next()%4); }
                    }
                    else if(cls=="CTaskLeanOnVehicle") {
                        objSize=0x2C;
                        if(nm=="Constructor"){ q0=next()%6 ? (void*)pickVeh() : nullptr; a0=next()%4==0 ? -1 : int32(next()%20000); b0=next()%2; }
                        else if(nm=="FinishAnimCB"){
                            setObj(VT_LEAN,0x2C); CVehicle* tv=pickVeh(); At<CVehicle*>(o,8)=tv; o[0x29]=uint8(next()%2); q0=assocs[next()%NASSOC].data(); q1=o; }
                        else { setObj(VT_LEAN,0x2C);
                            CVehicle* tv=next()%8==0 ? nullptr : pickVeh(); At<CVehicle*>(o,8)=tv; Reg(tv,o+8);
                            At<int32>(o,0xC)=next()%4==0 ? -1 : int32(next()%20000); o[0x10]=uint8(next()%2);
                            { const uint32 m=next()%3; At<void*>(o,0x14)=m==0 ? nullptr : (void*)assocs[next()%NASSOC].data(); }
                            { static const int32 la[]{-1,0x12F,0x130,0x131,0x132}; At<int32>(o,0x18)=la[next()%std::size(la)]; }
                            timer(0x1C); o[0x28]=uint8(next()%3==0); o[0x29]=uint8(next()%2); o[0x2A]=uint8(next()%5==0);
                            if(nm=="MakeAbortable"){ a0=prio(); ev=nullptr; At<void*>(p0b,0x128)=pickVeh(); }
                            if(nm=="ProcessPed"){ if(next()%2)At<void*>(p0b,0x128)=pickVeh(); }
                        }
                    }
                    const auto before=Snapshot();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr;
                    uintptr_t retPtr{}; bool isPtr=false; auto Ptr=[&](const void* p)->json{ retPtr=uintptr_t(p); isPtr=true; return nullptr; };
                    auto callTask=[&](CPed* ped){ return reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callBool=[&](CPed* ped){ return reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,ped); };
                    auto callVoid=[&]{ reinterpret_cast<void(__thiscall*)(void*)>(target)(o); };
                    auto callAbort=[&]{ return reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const CEvent*)>(target)(o,p0,a0,ev); };
                    {
                        // new/delete del DLL (utilidad de alineacion) en la arena; la traza es de registros fijos (sin asignar)
                        ReversingProbe::EntryPatch n1(0x82119A,FakeNew), n2(0x821195,FakeNew), d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDelete);
                        if(nm=="GetTaskType"){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(o); }
                        else if(nm=="Clone"){ ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o)); }
                        else if(nm=="MakeAbortable"){ ret=callAbort(); }
                        else if(cls=="CTaskSimpleCarDrive"||cls=="CTaskSimpleCarDriveTimed"){
                            if(nm=="Constructor"){ if(cls=="CTaskSimpleCarDrive")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,void*,uint32)>(target)(o,reinterpret_cast<CVehicle*>(q1),q0,0xFFFFFF00u|b0));
                                                   else ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,reinterpret_cast<CVehicle*>(q1),a0)); }
                            else if(nm=="Destructor")callVoid();
                            else if(nm=="ProcessPed"||nm=="SetPedPosition")ret=callBool(p0);
                            else if(nm=="StartBopping"||nm=="TriggerIK")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,p0);
                            else if(nm=="UpdateBopping")callVoid();
                            else if(nm=="ProcessHeadBopping"||nm=="ProcessArmBopping")reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32,float)>(target)(o,p0,0xFFFFFF00u|b0,f0);
                            else if(nm=="ProcessBopping")reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32)>(target)(o,p0,0xFFFFFF00u|b0);
                        }
                        else if(cls=="CTaskSimpleCarSetPedInAsDriver"){
                            if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,void*)>(target)(o,reinterpret_cast<CVehicle*>(q0),q1));
                            else if(nm=="ProcessPed")ret=callBool(p0);
                        }
                        else if(cls=="CTaskSimpleCarSetPedInAsPassenger"){
                            if(nm=="Destructor")callVoid(); else if(nm=="ProcessPed")ret=callBool(p0);
                        }
                        else if(cls=="CTaskSimpleCarSetPedOut"){
                            if(nm=="PositionPedOutOfCollision")reinterpret_cast<void(__cdecl*)(CPed*,CVehicle*,int32)>(target)(p0,reinterpret_cast<CVehicle*>(q1),a0);
                            else if(nm=="ProcessPed")ret=callBool(p0);
                        }
                        else if(cls=="CTaskComplexDriveToPoint"){
                            if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,const CVector*,float,int32,int32,float,int32)>(target)(o,reinterpret_cast<CVehicle*>(q0),&At<CVector>(vecBuf.data(),0),f0,a0,a1,f1,a2));
                            else if(nm=="SetUpCar")callVoid();
                            else if(nm=="CreateSubTaskCannotGetInCar"||nm=="Drive")ret=Ptr(callTask(p0));
                            else if(nm=="IsTargetBlocked-")ret=callBool(p0);
                            else if(nm=="IsTargetBlocked-entities")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,CEntity**,int32)>(target)(o,reinterpret_cast<CPed*>(q0),reinterpret_cast<CEntity**>(q1),a0);
                        }
                        else if(cls=="CTaskComplexEnterCarAsPassengerWait"){
                            if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,CPed*,uint32,uint32)>(target)(o,reinterpret_cast<CVehicle*>(q0),reinterpret_cast<CPed*>(q1),0xFFFFFF00u|b0,uint32(a0)));
                            else if(nm=="Destructor")callVoid();
                            else if(nm=="CreateSubTask")ret=Ptr(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target)(o,a0,p0));
                            else ret=Ptr(callTask(p0));
                        }
                        else if(cls=="CTaskComplexOpenPassengerDoor"){
                            if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(target)(o,reinterpret_cast<CVehicle*>(q0),a0));
                            else if(nm=="Destructor")callVoid();
                        }
                        else if(cls=="CTaskLeanOnVehicle"){
                            if(nm=="Constructor")ret=Ptr(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,int32,uint32)>(target)(o,reinterpret_cast<CEntity*>(q0),a0,0xFFFFFF00u|b0));
                            else if(nm=="Destructor")callVoid();
                            else if(nm=="FinishAnimCB")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(q0,q1);
                            else if(nm=="ProcessPed")ret=callBool(p0);
                        }
                    }
                    for(auto* h:hooks)h->State(false);

                    // tareas y eventos nuevos (se nombran antes de serializar el objeto)
                    { auto* pool=GetTaskPool(); json tasks=json::array();
                      for(size_t k=0;k<occT.size();++k)if(!occT[k]&&!pool->IsFreeSlotAtIndex(k))tasks.push_back(SerTask(pool->GetAt(k))); out["tasks"]=tasks; }
                    { json events=json::array(); for(size_t k=0;k<occE.size();++k)if(!occE[k]&&!(States(EventHdr())[k]&0x80)){ Rec* r=NewRec("fresh_event"); EvArgs(r,reinterpret_cast<CEvent*>(Storage(EventHdr())+k*EVENT_SLOT)); }
                      out["events"]=events; }
                    if(isPtr)ret=Name(retPtr);
                    out["object"]=Region(o,objSize,true);
                    if(nm=="Constructor")out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType();
                    out["changes"]=Changes(before); out["refs"]=AllRefs();
                    out["alloc"]={newCount,newSize,delCount,delOff};
                    out["glob"]=json::array({Name(At<uint32>(reinterpret_cast<void*>(IGNOREENT),0))});
                    out["ret"]=ret; out["trace"]=Trace(); out["rng"]=rngCalls;
                    // limpieza: referencias de los fixtures y slots nuevos (sin destructor); los pools deben quedar como antes
                    CleanRefs();
                    FreeFresh(TaskHdr(),occT); FreeFresh(EventHdr(),occE);
                    out["clean"]=Occ(TaskHdr())==occT&&Occ(EventHdr())==occE;
                    restoreGlobals();
                    return out;
                };
                Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
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
    const bool restored=Occ(TaskHdr())==occTaskStart&&Occ(EventHdr())==occEvStart&&freeRefs()==savedFree&&globalsIntact()&&Last(TaskHdr())==lt0&&Last(EventHdr())==le0;
    if(restored){ std::memcpy(States(TaskHdr()),stTask.data(),stTask.size()); std::memcpy(States(EventHdr()),stEv.data(),stEv.size()); }
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
