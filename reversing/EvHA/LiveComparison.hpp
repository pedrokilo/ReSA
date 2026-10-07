#pragma once
// Banco dentro del motor: CEventHandler (grupo A, 20 funciones): ComputeDamageResponse, ComputePersonalityResponseToDamage,
// ComputeDeathResponse, ComputeDeadPedResponse, ComputeVehicleDamageResponse, ComputeGotKnockedOverByCarResponse,
// ComputeVehicleCollisionResponse, ComputeVehiclePotentialCollisionResponse, ComputeEventResponseTask (la de evento y la
// estatica de peaton), HandleEvents, Flush, FlushImmediately, SetEventResponseTask, RegisterKill, Constructor, Destructor,
// IsKillTaskAppropriate, IsTemporaryEvent y GetCurrentEventType.
// Original (hooks del grupo desactivados) frente a candidato con fixtures identicos: el manejador (CEventHandler de 0x34
// bytes con m_Ped en +0 y las respuestas en +0x20..+0x30, rellenas con un valor centinela para distinguir "no escribe" de
// "escribe nullptr"), cuatro peatones (con matriz o sin ella, armas, tipo, banderas, datos de jugador e inteligencia con
// tareas primarias/secundarias falsas), dos vehiculos (matriz, velocidad, conductor, modelo falso en ms_modelInfoPtrs),
// un objeto, eventos con la disposicion del EXE y vtable trazada (tipo, origen, prioridad, clon, destructor) y tareas
// falsas con vtable trazada. Se compara el valor final de las respuestas, TODAS las tareas nuevas del pool (tipo y 0x80
// bytes con los punteros traducidos, en el orden de los slots), las referencias, los fixtures y la traza.
// Se simulan y trazan: FindPlayerPed/FindPlayerVehicle, CPed::Say/SetPedState/IsPedGoingForCarDoor, el RNG (rand 0x821B1E y
// CGeneral::GetRandomNumber), IKChainManager_c::IsLooking/LookAt, CPedIntelligence::GetTaskFighting/GetTaskUseGun/GetTaskDuck/
// AddTaskPhysResponse/AddTaskEventResponseTemp/NonTemp/IncrementAngerAtPlayer, CEventDamage::ProcessDamage/ComputeDeathAnim/
// ComputeDamageAnim, CInterestingEvents::Add, CTaskComplexFallToDeath::CalcFall, CTaskSimpleChoking::UpdateChoke,
// CTaskComplexFacial::SetRequest, las funciones de animacion (GetAnimAssociation, AddAnimation, BlendAnimation,
// RpAnimBlendClumpGetAssociation/GetFirstAssociation, SetCurrentTime), Fx_c::AddBlood, CCam::GetWeaponFirstPersonOn,
// CPlayerPed::AnnoyPlayerPed, CPedGeometryAnalyser (esquinas, planos, direccion a evitar, saltar obstaculo),
// ComputeEvasiveStepMoveDir, CPedGroups::GetPedsGroup/RegisterKillByPlayer, CDarkel, CStats::IncrementStat,
// CTheScripts::IsPlayerOnAMission y, segun la funcion, las Compute*Response (despacho), CEventGroup y SetEventResponseTask/
// ComputeEventResponseTask (HandleEvents). Los constructores sin hook en el EXE (CTaskSimpleDead 0x630590, CTaskSimpleAchieveHeading
// 0x667E20 y CTaskComplexStuckInAir 0x67BA40) se redirigen a la DLL en las dos ejecuciones para comparar solo la logica.
// El pool de tareas y el de rutas (estados y m_LastFreeSlot, huecos libres a 0xCD), las secuencias de tareas, los modos de
// camara, los contadores del jugador y el reloj se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventHandler.h"
#include "Events/EventDamage.h"
#include "Events/EventGroup.h"
#include "Tasks/TaskTypes/TaskSimpleChoking.h"
#include "Tasks/TaskTypes/TaskComplexFallToDeath.h"
#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Tasks/TaskTypes/TaskComplexHitPedWithCar.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskComplexStuckInAir.h"
#include "Tasks/TaskTypes/TaskSimpleDead.h"
#include "Tasks/TaskSequences.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Fx/Fx.h"
#include "Ragdoll/IKChainManager.h"
#include "InterestingEvents.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "Scripts/TheScripts.h"
#include "Darkel.h"
#include "Stats.h"
#include "Cam.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace EvHALive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// tareas falsas (0x40 bytes): vtable en +0, tipo en +0x38, subtarea en +0x3C, resultado de MakeAbortable en +0x40
constexpr int FAKES=14;
enum { F_ACTIVE, F_SIMPLEST, F_KILLER, F_PHYS, F_ATTACK, F_SAY, F_PARTIAL, F_GUN, F_FALL, F_GETUP, F_CHOKE, F_O1, F_O2, F_O3 };
constexpr size_t FK_TYPE=0x38, FK_SUB=0x3C, FK_ABORT=0x40;
// eventos falsos (0x80 bytes): los campos reales del evento ocupan 0..0x44; los datos de la vtable falsa van aparte
constexpr size_t FE_TYPE=0x60, FE_PRIO=0x64, FE_INTR=0x68, FE_SRC=0x6C, FE_EDIT=0x70, FE_CLONE=0x74;
constexpr int EVENTS=6;     // 0..1 historial, 2 evento de dano actual del historial, 3 clon, 4..5 relleno
constexpr int EV_MAIN=6;    // indice del evento probado (buffer aparte)
// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_CLUMP=0x18, E_MODEL=0x22, E_STATUS=0x36, E_MOVE=0x44, PH_MASS=0x8C, PH_IGNORED=0x128, PH_BRIGHT=0x12C;
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x470, P_FLAGS4=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484, P_SHIFT_Y=0x4DC, P_STATE=0x530,
                 P_HEALTH=0x540, P_ROT_CUR=0x558, P_ROT_AIM=0x55C, P_STANDING_ON=0x568, P_VEHICLE=0x58C, P_TYPE=0x598, P_WEAPONS=0x5A0,
                 P_WSLOT=0x718, P_RATE=0x719;
constexpr size_t V_DRIVER=0x460, V_PASSENGER0=0x464, V_HORN=0x514, V_TYPE=0x590, V_SUBTYPE=0x594;
constexpr size_t I_PRIMARY=0x04, I_SECONDARY=0x18, I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40, I_GROUP=0x68, I_SEQUENCE=0xD0;   // CPedIntelligence
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_CAM_ACTIVE=0xB6F081, ADDR_CAM_MODE=0xB6F1A8, ADDR_PLAYER_FOCUS=0xB7CD74, ADDR_PLAYERS=0xB7CD98;
constexpr size_t PLAYER_STRIDE=0x190, PLAYER_HAVOC=0xB7CED8-0xB7CD98, PLAYER_CHASE=0xB7CEE0-0xB7CD98;
constexpr int16 MI_FAKE=1;            // entrada de ms_modelInfoPtrs sustituida durante cada llamada

alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{}, pedD{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, matB{}, matC{}, matD{}, vmatA{}, vmatB{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,0x300> obj{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdA{}, pdB{}, pdC{}, pdD{};
alignas(16) inline std::array<uint8,0x40> hbuf{}, mi{}, col{}, clump{}, gbuf{}, hier{}, stat{};
alignas(16) inline std::array<uint8,0x60> assocA{}, assocB{}, assocC{}, assocD{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x80>,EVENTS+1> events{};
inline std::array<void*,12> pedVtbl{};
inline uint8* PedBuf(int k) { switch(k){case 0:return pedA.data();case 1:return pedB.data();case 2:return pedC.data();default:return pedD.data();} }
inline uint8* MatBuf(int k) { switch(k){case 0:return matA.data();case 1:return matB.data();case 2:return matC.data();default:return matD.data();} }
inline uint8* PdBuf(int k) { switch(k){case 0:return pdA.data();case 1:return pdB.data();case 2:return pdC.data();default:return pdD.data();} }
inline uint8* VehBuf(int k) { return k==0 ? vehA.data() : vehB.data(); }
inline uint8* VMatBuf(int k) { return k==0 ? vmatA.data() : vmatB.data(); }
inline CPed* Ped(int k) { return reinterpret_cast<CPed*>(PedBuf(k)); }
inline CEntity* EntOf(void* b) { return reinterpret_cast<CEntity*>(b); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* Ev(int n) { return events[n].data(); }
inline uint8* EvMain() { return events[EV_MAIN].data(); }
inline uint8* IntelFor(int k) { return k==0 ? intelA.data() : intelB.data(); }
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
#define EVHA_IN(buf,nm) if(auto s=in(buf.data(),buf.size(),nm))return *s;
    EVHA_IN(pedA,"pedA") EVHA_IN(pedB,"pedB") EVHA_IN(pedC,"pedC") EVHA_IN(pedD,"pedD")
    EVHA_IN(matA,"matA") EVHA_IN(matB,"matB") EVHA_IN(matC,"matC") EVHA_IN(matD,"matD") EVHA_IN(vmatA,"vmatA") EVHA_IN(vmatB,"vmatB")
    EVHA_IN(vehA,"vehA") EVHA_IN(vehB,"vehB") EVHA_IN(obj,"obj") EVHA_IN(intelA,"intelA") EVHA_IN(intelB,"intelB")
    EVHA_IN(pdA,"pdA") EVHA_IN(pdB,"pdB") EVHA_IN(pdC,"pdC") EVHA_IN(pdD,"pdD")
    EVHA_IN(hbuf,"handler") EVHA_IN(mi,"mi") EVHA_IN(col,"col") EVHA_IN(clump,"clump") EVHA_IN(gbuf,"group") EVHA_IN(hier,"hier") EVHA_IN(stat,"static")
    EVHA_IN(assocA,"assocA") EVHA_IN(assocB,"assocB") EVHA_IN(assocC,"assocC") EVHA_IN(assocD,"assocD")
#undef EVHA_IN
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<=EVENTS;++n)if(auto s=in(events[n].data(),events[n].size(),"ev"))return *s+"#"+std::to_string(n);
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline bool looking{}, hasFirstPerson{}, canJump{}, goingForDoor{}, onMission{}, fallRet{}, fallRail{};
inline int32 fallDir{}; inline int32 cfgGroup{}, cfgAnim{}; inline float cfgBlend{}, cfgSpeed{};
inline uintptr_t playerRet{}, playerVehRet{}, fightRet{}, gunRet{}, duckRet{}, pedGroupRet{}, assocStaticRet{}, assocFoundRet{}, assocFirstRet{}, assocAddRet{}, assocBlendRet{};
inline CVector avoidDir{}, evasiveDir{}; inline std::array<CVector,4> corners{};
inline int32 forceFirst=-1; inline uint32 seed{}, rngCalls{};
inline uintptr_t dispWrite{}, dispPhys{};   // lo que escribe la Compute*Response simulada en el manejador (+0x24 y +0x20)
inline uintptr_t hbufAddr{};                // direccion del manejador de los fixtures (para distinguir el manejador de pila)
inline int32 stackedHandlerRuns{};

// ---------- tareas falsas: vtable con GetSubTask (2), IsSimple (3), GetTaskType (4) y MakeAbortable (6) ----------
inline void* __fastcall FDtor(void* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask* t,void*) { return At<CTask*>(t,FK_SUB); }
inline bool __fastcall FSimple(CTask* t,void*) { return At<CTask*>(t,FK_SUB)==nullptr; }
inline bool __fastcall FAbort(CTask* t,void*,CPed* ped,int32 prio,const CEvent* ev) { trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(ped)),prio,Name(uintptr_t(ev))}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_task_slot",N}); return nullptr; }
inline std::array<void*,16> taskVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// ---------- eventos falsos: GetEventType (1), Clone (4), HasEditableResponse (9), GetSourceEntity (10), TakesPriorityOver (11), CanBeInterruptedBySameEvent (15) ----------
inline int __fastcall EType(void* e,void*) { return At<int32>(e,FE_TYPE); }
inline CEntity* __fastcall ESource(void* e,void*) { return At<CEntity*>(e,FE_SRC); }
inline bool __fastcall EPrio(void* e,void*,const CEvent*) { trace.push_back({"takes_priority",Name(uintptr_t(e))}); return At<uint8>(e,FE_PRIO)!=0; }
inline bool __fastcall EIntr(void* e,void*) { return At<uint8>(e,FE_INTR)!=0; }
inline bool __fastcall EEdit(void* e,void*) { return At<uint8>(e,FE_EDIT)!=0; }
inline void* __fastcall EClone(void* e,void*) { trace.push_back({"clone_event",Name(uintptr_t(e))}); return events[3].data(); }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"unexpected_event_slot",N}); return nullptr; }
inline std::array<void*,20> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// ---------- peatones falsos: SetIsStatic (hueco 4) ----------
inline void __fastcall PSetStatic(CPed* p,void*,bool b) { trace.push_back({"set_is_static",Name(uintptr_t(p)),b}); }
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"unexpected_ped_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }

// ---------- dependencias simuladas ----------
inline int __cdecl FakeRand() {
    trace.push_back({"rand"}); int v=int((seed*2654435761u+rngCalls*40503u+rngCalls*rngCalls*977u)>>9)&0x7FFF;
    if(rngCalls==0&&forceFirst>=0)v=forceFirst; ++rngCalls; return v;
}
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CVehicle* __cdecl FindPlayerVeh(int32 id,bool remote) { trace.push_back({"find_player_vehicle",id,remote}); return reinterpret_cast<CVehicle*>(playerVehRet); }
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline void __fastcall SetPedStateStub(CPed* p,void*,int s) { trace.push_back({"set_ped_state",Name(uintptr_t(p)),s}); }
inline bool __fastcall IsLooking(const IKChainManager_c*,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(uintptr_t(p))}); return looking; }
inline void __fastcall LookAt(IKChainManager_c*,void*,const char* purpose,CPed* p,CEntity* en,int32 time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"ik_look_at",std::string(purpose),Name(uintptr_t(p)),Name(uintptr_t(en)),time,int(bone),Name(uintptr_t(offset)),torso,F(speed),blend,prio,force});
}
inline bool __fastcall GoingForDoor(CPedIntelligence* i,void*) { trace.push_back({"is_ped_going_for_car_door",Name(uintptr_t(i))}); return goingForDoor; }
inline void* __fastcall GetFight(CPedIntelligence* i,void*) { trace.push_back({"get_task_fighting",Name(uintptr_t(i))}); return reinterpret_cast<void*>(fightRet); }
inline void* __fastcall GetGun(CPedIntelligence* i,void*) { trace.push_back({"get_task_use_gun",Name(uintptr_t(i))}); return reinterpret_cast<void*>(gunRet); }
inline void* __fastcall GetDuck(CPedIntelligence* i,void*,bool b) { trace.push_back({"get_task_duck",Name(uintptr_t(i)),b}); return reinterpret_cast<void*>(duckRet); }
inline void __fastcall AddPhys(CPedIntelligence* i,void*,CTask* t,int32 u) { trace.push_back({"add_task_phys_response",Name(uintptr_t(i)),Name(uintptr_t(t)),u&0xFF}); }
inline void __fastcall AddTemp(CPedIntelligence* i,void*,CTask* t,int32 u) { trace.push_back({"add_task_event_response_temp",Name(uintptr_t(i)),Name(uintptr_t(t)),u&0xFF}); }
inline void __fastcall AddNonTemp(CPedIntelligence* i,void*,CTask* t,int32 u) { trace.push_back({"add_task_event_response_nontemp",Name(uintptr_t(i)),Name(uintptr_t(t)),u&0xFF}); }
inline void __fastcall IncAnger(CPedIntelligence* i,void*,uint8 a) { trace.push_back({"increment_anger_at_player",Name(uintptr_t(i)),a}); }
inline void __fastcall ProcessDamageStub(CEventDamage* e,void*,CPed* p) { trace.push_back({"process_damage",Name(uintptr_t(e)),Name(uintptr_t(p))}); }
inline void __fastcall DeathAnimStub(CEventDamage* e,void*,CPed* p,bool b) {
    trace.push_back({"compute_death_anim",Name(uintptr_t(e)),Name(uintptr_t(p)),b});
    At<int32>(e,0x28)=cfgGroup; At<int32>(e,0x2C)=cfgAnim; At<float>(e,0x30)=cfgBlend; At<float>(e,0x34)=cfgSpeed;
}
inline void __fastcall DamageAnimStub(CEventDamage* e,void*,CPed* p,bool b) {
    trace.push_back({"compute_damage_anim",Name(uintptr_t(e)),Name(uintptr_t(p)),b});
    At<int32>(e,0x28)=cfgGroup; At<int32>(e,0x2C)=cfgAnim; At<float>(e,0x30)=cfgBlend; At<float>(e,0x34)=cfgSpeed;
}
inline void __fastcall IntEvAdd(CInterestingEvents*,void*,uint32 type,CEntity* en) { trace.push_back({"interesting_event_add",type,Name(uintptr_t(en))}); }
inline bool __cdecl CalcFallStub(CPed* p,int32& dir,bool& rail) { trace.push_back({"calc_fall",Name(uintptr_t(p))}); dir=fallDir; rail=fallRail; return fallRet; }
inline void __fastcall UpdateChokeStub(CTaskSimpleChoking* t,void*,CPed* victim,CPed* attacker,bool tear) {
    trace.push_back({"update_choke",Name(uintptr_t(t)),Name(uintptr_t(victim)),Name(uintptr_t(attacker)),tear});
}
inline void __fastcall FacialRequest(CTaskComplexFacial* t,void*,int a,int32 da,int b,int32 db) { trace.push_back({"facial_set_request",Name(uintptr_t(t)),a,da,b,db}); }
// animaciones
inline void* __cdecl GetAnimAssoc(AssocGroupId g,AnimationId id) { trace.push_back({"get_anim_association",int(g),int(id)}); return stat.data(); }
inline void* __cdecl AddAnimStub(RpClump* c,AssocGroupId g,AnimationId id) { trace.push_back({"add_animation",Name(uintptr_t(c)),int(g),int(id)}); return reinterpret_cast<void*>(assocAddRet); }
inline void* __cdecl BlendAnimStub(RpClump* c,AssocGroupId g,AnimationId id,float d) { trace.push_back({"blend_animation",Name(uintptr_t(c)),int(g),int(id),F(d)}); return reinterpret_cast<void*>(assocBlendRet); }
inline void* __cdecl GetAssocById(RpClump* c,uint32 id) { trace.push_back({"get_association",Name(uintptr_t(c)),id}); return reinterpret_cast<void*>(assocFoundRet); }
inline void* __cdecl GetFirstAssoc(RpClump* c,uint32 flags) { trace.push_back({"get_first_association",Name(uintptr_t(c)),flags}); return reinterpret_cast<void*>(assocFirstRet); }
inline void __fastcall SetCurTime(CAnimBlendAssociation* a,void*,float t) { trace.push_back({"set_current_time",Name(uintptr_t(a)),F(t)}); }
inline void __fastcall AddBloodStub(Fx_c*,void*,const CVector& o,const CVector& d,int32 n,float l) { trace.push_back({"add_blood",F(o.x),F(o.y),F(o.z),F(d.x),F(d.y),F(d.z),n,F(l)}); }
inline bool __fastcall FirstPersonStub(CCam* c,void*) { trace.push_back({"cam_weapon_first_person",int((uintptr_t(c)-(ADDR_CAM_MODE-0xC))/0x238)}); return hasFirstPerson; }
inline void __fastcall AnnoyStub(CPlayerPed* p,void*,bool b) { trace.push_back({"annoy_player_ped",Name(uintptr_t(p)),b}); }
// geometria
inline void __cdecl BBCorners(float z,CEntity& e,CVector* out) { trace.push_back({"bb_corners",F(z),Name(uintptr_t(&e))}); for(int k=0;k<4;++k)out[k]=corners[k]; }
inline void __cdecl BBPlanes(float z,CEntity& e,CVector (*planes)[4],float* dots) { trace.push_back({"bb_planes",F(z),Name(uintptr_t(&e))}); }
inline int32 __cdecl AvoidDir(const CPed& p,CEntity& e,CVector& out) { trace.push_back({"move_dir_to_avoid",Name(uintptr_t(&p)),Name(uintptr_t(&e))}); out=avoidDir; return 0; }
inline bool __cdecl JumpObstacle(const CPed& p,const CEntity& e) { trace.push_back({"can_jump_obstacle",Name(uintptr_t(&p)),Name(uintptr_t(&e))}); return canJump; }
inline CVector* __cdecl EvasiveDirStub(CVector* out,const CPed* p,CVehicle* v) { trace.push_back({"evasive_step_dir",Name(uintptr_t(p)),Name(uintptr_t(v))}); *out=evasiveDir; return out; }
// grupos, estadisticas y puntuacion
inline CPedGroup* __cdecl PedsGroup(const CPed* p) { trace.push_back({"get_peds_group",Name(uintptr_t(p))}); return reinterpret_cast<CPedGroup*>(pedGroupRet); }
inline void __cdecl GroupRegKill() { trace.push_back({"group_register_kill_by_player"}); }
inline void __cdecl DarkelByPlayer(const CPed& p,eWeaponType w,bool head,int32 pt) { trace.push_back({"darkel_kill_by_player",Name(uintptr_t(&p)),uint32(w),head,pt}); }
inline void __cdecl DarkelNotByPlayer(const CPed* p) { trace.push_back({"darkel_kill_not_by_player",Name(uintptr_t(p))}); }
inline void __cdecl IncStat(int32 s,float v) { trace.push_back({"increment_stat",s,F(v)}); }
inline bool __cdecl OnMission() { trace.push_back({"is_player_on_a_mission"}); return onMission; }
// CEventGroup y CTaskManager::StopTimers (HandleEvents)
inline uintptr_t groupHighest{};
inline CEvent* __fastcall GGetHighest(CEventGroup*,void*) { trace.push_back({"group_get_highest_priority_event"}); return reinterpret_cast<CEvent*>(groupHighest); }
inline void __fastcall GTick(CEventGroup*,void*) { trace.push_back({"group_tick_events"}); }
inline void __fastcall GRemove(CEventGroup*,void*,CEvent* e) { trace.push_back({"group_remove",Name(uintptr_t(e))}); }
inline void __fastcall GRemoveInvalid(CEventGroup*,void*,bool b) { trace.push_back({"group_remove_invalid",b}); }
inline void __fastcall GReorganise(CEventGroup*,void*) { trace.push_back({"group_reorganise"}); }
inline CEvent* __fastcall GAdd(CEventGroup* g,void*,CEvent* e,bool v) {
    trace.push_back({"group_add",Name(uintptr_t(g)),int(e->GetEventType()),Name(At<uintptr_t>(e,0x14)),v}); return e;
}
inline void __fastcall KnockOffStub(void*,void*,void* e,CTask* a,CTask* b) { trace.push_back({"knock_off_bike",Name(uintptr_t(e)),Name(uintptr_t(a)),Name(uintptr_t(b))}); }
inline void __fastcall StopTimersStub(CTaskManager* m,void*,const CEvent* e) { trace.push_back({"stop_timers",Name(uintptr_t(m)),Name(uintptr_t(e))}); }
// SetEventResponseTask / ComputeEventResponseTask simuladas (HandleEvents)
inline void __fastcall SimComputeERT(void* self,void*,CEvent* e,CTask* aborted) {
    trace.push_back({"compute_event_response_task",Name(uintptr_t(e)),Name(uintptr_t(aborted))});
    At<uintptr_t>(self,0x24)=dispWrite; At<uintptr_t>(self,0x20)=dispPhys;
}
inline void __fastcall SimSetERT(void* self,void*,const CEvent& e) { trace.push_back({"set_event_response_task",Name(uintptr_t(&e))}); }
// despacho de ComputeEventResponseTask: cada Compute*Response simulada anota sus argumentos y escribe lo configurado
template<int N> void __fastcall DispStub(void* self,void*,void* e,CTask* a,CTask* b) {
    trace.push_back({"compute",N,self==reinterpret_cast<void*>(hbufAddr) ? "handler" : "stack",Name(uintptr_t(e)),Name(uintptr_t(a)),Name(uintptr_t(b))});
    At<uintptr_t>(self,0x24)=dispWrite; At<uintptr_t>(self,0x20)=dispPhys;
}
inline void __fastcall DispDamage(void* self,void*,void* e,CTask* a,CTask* b,CTask* c) {
    trace.push_back({"compute",7,self==reinterpret_cast<void*>(hbufAddr) ? "handler" : "stack",Name(uintptr_t(e)),Name(uintptr_t(a)),Name(uintptr_t(b)),Name(uintptr_t(c))});
    At<uintptr_t>(self,0x24)=dispWrite; At<uintptr_t>(self,0x20)=dispPhys;
}
struct Disp { uintptr_t exe; uintptr_t dll; void* stub; };
#define EVHA_D(N,ADDR,FN) Disp{ADDR,std::bit_cast<uintptr_t>(&CEventHandler::FN),(N==7 ? reinterpret_cast<void*>(&DispDamage) : reinterpret_cast<void*>(&DispStub<N>))},
inline const std::array<Disp,59> dispTable{{
    EVHA_D(0,0x4BD6A0,ComputeVehicleCollisionResponse) EVHA_D(1,0x4BDB80,ComputePedCollisionWithPedResponse) EVHA_D(2,0x4BE7D0,ComputePedCollisionWithPlayerResponse)
    EVHA_D(3,0x4B8CE0,ComputePlayerCollisionWithPedResponse) EVHA_D(4,0x4B92B0,ComputeObjectCollisionResponse) EVHA_D(5,0x4BF2B0,ComputeBuildingCollisionResponse)
    EVHA_D(6,0x4BCC30,ComputeDraggedOutCarResponse) EVHA_D(7,0x4C0170,ComputeDamageResponse) EVHA_D(8,0x4B9400,ComputeDeathResponse)
    EVHA_D(9,0x4B9470,ComputeDeadPedResponse) EVHA_D(10,0x4C0BD0,ComputeVehiclePotentialCollisionResponse) EVHA_D(11,0x4B96D0,ComputeVehiclePotentialPassiveCollisionResponse)
    EVHA_D(12,0x4BC710,ComputeShotFiredResponse) EVHA_D(13,0x4B97B0,ComputeReviveResponse) EVHA_D(14,0x4B98E0,ComputeChatPartnerResponse)
    EVHA_D(15,0x4B99F0,ComputeSexyPedResponse) EVHA_D(16,0x4B9AA0,ComputeSexyVehicleResponse) EVHA_D(17,0x4C1910,ComputePedToChaseResponse)
    EVHA_D(18,0x4B9B50,ComputePedToFleeResponse) EVHA_D(19,0x4B9BE0,ComputeAttractorResponse) EVHA_D(20,0x4C2610,ComputePotentialPedCollideResponse)
    EVHA_D(21,0x4C1590,ComputePedEnteredVehicleResponse) EVHA_D(22,0x4B9F80,ComputeVehicleToStealResponse) EVHA_D(23,0x4B9FF0,ComputeKnockOffBikeResponse)
    EVHA_D(24,0x4C2840,ComputeGunAimedAtResponse) EVHA_D(25,0x4BA7C0,ComputeScriptCommandResponse) EVHA_D(26,0x4BA8B0,ComputeVehicleDiedResponse)
    EVHA_D(27,0x4B9DD0,ComputePedFriendResponse) EVHA_D(28,0x4C19A0,ComputePedThreatResponse) EVHA_D(29,0x4BC050,ComputeSeenCopResponse)
    EVHA_D(30,0x4B9C90,ComputePedThreatBadlyLitResponse) EVHA_D(31,0x4B9D40,ComputePedSoundQuietResponse) EVHA_D(32,0x4BA990,ComputeLowHealthResponse)
    EVHA_D(33,0x4BAA30,ComputeReallyLowHealthResponse) EVHA_D(34,0x4BAAD0,ComputeLowAngerAtPlayerResponse) EVHA_D(35,0x4BAC10,ComputeHighAngerAtPlayerResponse)
    EVHA_D(36,0x4BAD50,ComputeOnFireResponse) EVHA_D(37,0x4BAE30,ComputeWaterCannonResponse) EVHA_D(38,0x4BAF80,ComputeInWaterResponse)
    EVHA_D(39,0x4BAFE0,ComputeInteriorUseInfoResponse) EVHA_D(40,0x4BB050,ComputeSignalAtPedResponse) EVHA_D(41,0x4BB0C0,ComputePassObjectResponse)
    EVHA_D(42,0x4BB130,ComputeCreatePartnerTaskResponse) EVHA_D(43,0x4BB280,ComputePlayerWantedLevelResponse) EVHA_D(44,0x4BB2E0,ComputeVehicleOnFireResponse)
    EVHA_D(45,0x4BB740,ComputeCopCarBeingStolenResponse) EVHA_D(46,0x4C2FC0,ComputeVehicleDamageResponse) EVHA_D(47,0x4BB800,ComputeSpecialResponse)
    EVHA_D(48,0x4C3430,ComputeGotKnockedOverByCarResponse) EVHA_D(49,0x4BBB90,ComputeObjectCollisionPassiveResponse) EVHA_D(50,0x4BBC30,ComputeCarUpsideDownResponse)
    EVHA_D(51,0x4BBCD0,ComputePotentialWalkIntoFireResponse) EVHA_D(52,0x4BBE30,ComputeShotFiredWhizzedByResponse) EVHA_D(53,0x4BBFB0,ComputeFireNearbyResponse)
    EVHA_D(54,0x4C35F0,ComputeSeenPanickedPedResponse) EVHA_D(55,0x4BBF50,ComputeAreaCodesResponse) EVHA_D(56,0x4BC150,ComputeOnEscalatorResponse)
    EVHA_D(57,0x4BC230,ComputeDangerResponse) EVHA_D(58,0x4BC1D0,ComputeDontJoinGroupResponse)
}};
#undef EVHA_D
// constructores sin hook en el EXE: las dos ejecuciones usan los de la DLL
inline void* __fastcall DeadCtor(CTaskSimpleDead* t,void*,uint32 time,bool drowned) { ::new (t) CTaskSimpleDead(time,drowned); return t; }
inline void* __fastcall HeadingCtor(CTaskSimpleAchieveHeading* t,void*,float a,float b,float c) { ::new (t) CTaskSimpleAchieveHeading(a,b,c); return t; }
inline void* __fastcall StuckCtor(CTaskComplexStuckInAir* t,void*) { ::new (t) CTaskComplexStuckInAir(); return t; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Fx { X_DAMAGE, X_PERS, X_DEATH, X_DEAD, X_VDMG, X_KNOCK, X_VCOLL, X_VPOT, X_ERT0, X_ERTPED, X_HANDLE, X_FLUSH, X_FLUSHIMM, X_SETRESP, X_REGKILL, X_CTOR, X_DTOR, X_ISKILL, X_ISTEMP, X_GETCUR };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=20;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputeDamageResponse",0x4C0170,0,-1,3072},
    {"CEventHandler","ComputePersonalityResponseToDamage",0x4BF9B0,0,-1,1536},
    {"CEventHandler","ComputeDeathResponse",0x4B9400,0,-1,64},
    {"CEventHandler","ComputeDeadPedResponse",0x4B9470,0,-1,768},
    {"CEventHandler","ComputeVehicleDamageResponse",0x4C2FC0,0,-1,1536},
    {"CEventHandler","ComputeGotKnockedOverByCarResponse",0x4C3430,0,-1,384},
    {"CEventHandler","ComputeVehicleCollisionResponse",0x4BD6A0,0,-1,2048},
    {"CEventHandler","ComputeVehiclePotentialCollisionResponse",0x4C0BD0,0,-1,2048},
    {"CEventHandler","ComputeEventResponseTask-0",0x4C3870,0,-1,1536},
    {"CEventHandler","ComputeEventResponseTask-Ped",0x4C4220,0,-1,512},
    {"CEventHandler","HandleEvents",0x4C3F10,0,-1,2048},
    {"CEventHandler","Flush",0x4C3790,0,-1,128},
    {"CEventHandler","FlushImmediately",0x4C3820,0,-1,64},
    {"CEventHandler","SetEventResponseTask",0x4BC600,0,-1,768},
    {"CEventHandler","RegisterKill",0x4B9340,0,-1,512},
    {"CEventHandler","Constructor",0x4C3E80,0,-1,64},
    {"CEventHandler","Destructor",0x4C3EC0,0,-1,128},
    {"CEventHandler","IsKillTaskAppropriate",0x4BC3E0,0,-1,512},
    {"CEventHandler","IsTemporaryEvent",0x4BC370,0,-1,128},
    {"CEventHandler","GetCurrentEventType",0x4B8CC0,0,-1,128},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int k=0;k<4;++k)clean(EntOf(PedBuf(k)));
    clean(EntOf(vehA.data())); clean(EntOf(vehB.data())); clean(EntOf(obj.data()));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evha-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evha-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evha");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evha-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,16>{});
    taskVtbl[0]=reinterpret_cast<void*>(&FDtor); taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple);
    taskVtbl[4]=reinterpret_cast<void*>(&FType); taskVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillETraps(std::make_integer_sequence<int,20>{});
    eventVtbl[0]=reinterpret_cast<void*>(&FDtor); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[4]=reinterpret_cast<void*>(&EClone);
    eventVtbl[9]=reinterpret_cast<void*>(&EEdit); eventVtbl[10]=reinterpret_cast<void*>(&ESource); eventVtbl[11]=reinterpret_cast<void*>(&EPrio);
    eventVtbl[15]=reinterpret_cast<void*>(&EIntr);
    FillPTraps(std::make_integer_sequence<int,12>{}); pedVtbl[4]=reinterpret_cast<void*>(&PSetStatic);
    hbufAddr=uintptr_t(hbuf.data());
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado de los pools de tareas y de rutas: se restaura en cada ejecucion (estados y m_LastFreeSlot) para que ambas reciban los mismos slots
    auto* poolHdr=reinterpret_cast<uint8*>(GetTaskPool()); auto* poolStor=*reinterpret_cast<uint8**>(poolHdr); auto* poolStates=*reinterpret_cast<uint8**>(poolHdr+4);
    const int32 poolCap=*reinterpret_cast<int32*>(poolHdr+8); const int32 poolLf0=*reinterpret_cast<int32*>(poolHdr+0xC);
    const std::vector<uint8> savedPoolStates(poolStates,poolStates+poolCap);
    auto* routeHdr=reinterpret_cast<uint8*>(GetPointRoutePool()); auto* routeStor=*reinterpret_cast<uint8**>(routeHdr); auto* routeStates=*reinterpret_cast<uint8**>(routeHdr+4);
    const int32 routeCap=*reinterpret_cast<int32*>(routeHdr+8); const int32 routeLf0=*reinterpret_cast<int32*>(routeHdr+0xC);
    const std::vector<uint8> savedRouteStates(routeStates,routeStates+routeCap);
    // secuencias de tareas (CTaskComplexUseSequence cuenta referencias), modos de camara y contadores del jugador
    auto& seqArr=CTaskSequences::ms_taskSequence; const std::vector<uint8> savedSeq(reinterpret_cast<uint8*>(&seqArr),reinterpret_cast<uint8*>(&seqArr)+sizeof(seqArr));
    auto& camActive=*reinterpret_cast<uint8*>(ADDR_CAM_ACTIVE); const uint8 savedCamActive=camActive;
    auto camMode=[](int n)->int16& { return *reinterpret_cast<int16*>(ADDR_CAM_MODE+0x238*size_t(n)); }; const std::array<int16,3> savedCamModes{camMode(0),camMode(1),camMode(2)};
    const uint8 focus=*reinterpret_cast<uint8*>(ADDR_PLAYER_FOCUS);
    auto& havoc=*reinterpret_cast<uint32*>(0xB7CED8+size_t(focus)*0x190); auto& chase=*reinterpret_cast<float*>(0xB7CEE0+size_t(focus)*0x190);
    const uint32 savedHavoc=havoc; const float savedChase=chase;
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& miSlot=reinterpret_cast<void**>(0xA9B0C8)[MI_FAKE]; void* const savedMi=miSlot;
    auto restoreGlobals=[&]{
        std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
        std::memcpy(routeStates,savedRouteStates.data(),savedRouteStates.size()); *reinterpret_cast<int32*>(routeHdr+0xC)=routeLf0;
        std::memcpy(reinterpret_cast<uint8*>(&seqArr),savedSeq.data(),savedSeq.size());
        camActive=savedCamActive; for(int n=0;n<3;++n)camMode(n)=savedCamModes[size_t(n)];
        havoc=savedHavoc; chase=savedChase; miSlot=savedMi;
    };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);hooks[f]->State(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> common;
        auto patch=[&](std::vector<std::unique_ptr<EntryPatch>>& v,uintptr_t exeAddr,auto dllSym,auto stub){
            v.push_back(std::make_unique<EntryPatch>(exeAddr,stub)); v.push_back(std::make_unique<EntryPatch>(bit_cast<uintptr_t>(dllSym),stub)); };
        auto patchExe=[&](std::vector<std::unique_ptr<EntryPatch>>& v,uintptr_t exeAddr,auto stub){ v.push_back(std::make_unique<EntryPatch>(exeAddr,stub)); };
        // RNG, jugador, hablar, cinematica inversa
        patch(common,0x56E210,static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed),FindPlayer);
        patch(common,0x56E0D0,static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle),FindPlayerVeh);
        patch(common,0x5EFFE0,static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say),Say);
        patch(common,0x5E4500,&CPed::SetPedState,SetPedStateStub);
        patch(common,0x6181A0,&IKChainManager_c::IsLooking,IsLooking);
        patch(common,0x618970,&IKChainManager_c::LookAt,LookAt);
        patchExe(common,0x821B1E,FakeRand); common.push_back(std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber));
        // inteligencia
        patch(common,0x602350,&CPedIntelligence::IsPedGoingForCarDoor,GoingForDoor);
        patch(common,0x600F30,&CPedIntelligence::GetTaskFighting,GetFight);
        patch(common,0x600F70,&CPedIntelligence::GetTaskUseGun,GetGun);
        patch(common,0x6010A0,&CPedIntelligence::GetTaskDuck,GetDuck);
        patch(common,0x600DC0,&CPedIntelligence::AddTaskPhysResponse,AddPhys);
        patch(common,0x600DE0,&CPedIntelligence::AddTaskEventResponseTemp,AddTemp);
        patch(common,0x600E00,&CPedIntelligence::AddTaskEventResponseNonTemp,AddNonTemp);
        patch(common,0x421050,&CPedIntelligence::IncrementAngerAtPlayer,IncAnger);
        // eventos y tareas
        patch(common,0x4B3A20,&CEventDamage::ProcessDamage,ProcessDamageStub);
        patch(common,0x4B3A60,&CEventDamage::ComputeDeathAnim,DeathAnimStub);
        patch(common,0x4B3FC0,&CEventDamage::ComputeDamageAnim,DamageAnimStub);
        patch(common,0x602590,&CInterestingEvents::Add,IntEvAdd);
        patch(common,0x6796C0,&CTaskComplexFallToDeath::CalcFall,CalcFallStub);
        patch(common,0x620660,&CTaskSimpleChoking::UpdateChoke,UpdateChokeStub);
        patch(common,0x691230,&CTaskComplexFacial::SetRequest,FacialRequest);
        patch(common,0x4AB420,static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add),GAdd);
        patch(common,0x4AB7C0,&CEventGroup::GetHighestPriorityEvent,GGetHighest);
        patch(common,0x4AB6D0,&CEventGroup::TickEvents,GTick);
        patch(common,0x4AB5A0,&CEventGroup::Remove,GRemove);
        patch(common,0x4AB760,&CEventGroup::RemoveInvalidEvents,GRemoveInvalid);
        patch(common,0x4AB700,&CEventGroup::Reorganise,GReorganise);
        patch(common,0x6819A0,&CTaskManager::StopTimers,StopTimersStub);
        patch(common,0x4B9FF0,&CEventHandler::ComputeKnockOffBikeResponse,KnockOffStub);
        // animaciones, sangre, camara
        patch(common,0x4D3A60,static_cast<CAnimBlendStaticAssociation*(*)(AssocGroupId,AnimationId)>(&CAnimManager::GetAnimAssociation),GetAnimAssoc);
        patch(common,0x4D3AA0,static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation),AddAnimStub);
        patch(common,0x4D4610,static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation),BlendAnimStub);
        patch(common,0x4D68B0,static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation),GetAssocById);
        patch(common,0x4D6A70,static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation),GetFirstAssoc);
        patch(common,0x4CEA80,&CAnimBlendAssociation::SetCurrentTime,SetCurTime);
        patch(common,0x49EB00,&Fx_c::AddBlood,AddBloodStub);
        patch(common,0x509DC0,&CCam::GetWeaponFirstPersonOn,FirstPersonStub);
        patch(common,0x60A040,&CPlayerPed::AnnoyPlayerPed,AnnoyStub);
        // geometria
        patch(common,0x5F3650,&CPedGeometryAnalyser::ComputeEntityBoundingBoxCorners,BBCorners);
        patch(common,0x5F3660,&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes,BBPlanes);
        patch(common,0x5F3730,&CPedGeometryAnalyser::ComputeMoveDirToAvoidEntity,AvoidDir);
        patch(common,0x5F1B00,static_cast<bool(*)(const CPed&,const CEntity&)>(&CPedGeometryAnalyser::CanPedJumpObstacle),JumpObstacle);
        patch(common,0x653B40,&CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir,EvasiveDirStub);
        // grupos, puntuacion, estadisticas, guiones
        patch(common,0x5F7E80,&CPedGroups::GetPedsGroup,PedsGroup);
        patch(common,0x5F7E30,&CPedGroups::RegisterKillByPlayer,GroupRegKill);
        patch(common,0x43DCD0,&CDarkel::RegisterKillByPlayer,DarkelByPlayer);
        patch(common,0x43D210,&CDarkel::RegisterKillNotByPlayer,DarkelNotByPlayer);
        patch(common,0x55C180,&CStats::IncrementStat,IncStat);
        patch(common,0x464D50,&CTheScripts::IsPlayerOnAMission,OnMission);
        // constructores sin hook en el EXE: las dos ejecuciones usan los de la DLL
        patchExe(common,0x630590,DeadCtor); patchExe(common,0x667E20,HeadingCtor); patchExe(common,0x67BA40,StuckCtor);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            // parches propios de la funcion y hooks que no se conmutan (estan parcheados)
            std::vector<std::unique_ptr<EntryPatch>> own; std::array<bool,NFNS> excluded{};
            if(f==X_ERT0||f==X_ERTPED) {
                for(const auto& d:dispTable) patch(own,d.exe,d.dll,d.stub);
                for(auto k:{X_DAMAGE,X_DEATH,X_DEAD,X_VDMG,X_KNOCK,X_VCOLL,X_VPOT})excluded[size_t(k)]=true;
            }
            if(f==X_HANDLE) {
                patch(own,0x4C3870,static_cast<void(CEventHandler::*)(CEvent*,CTask*)>(&CEventHandler::ComputeEventResponseTask),SimComputeERT);
                patch(own,0x4BC600,&CEventHandler::SetEventResponseTask,SimSetERT);
                excluded[X_ERT0]=true; excluded[X_SETRESP]=true;
            }
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l)->uintptr_t{return *(l.begin()+next()%l.size());};
                auto pickI=[&](std::initializer_list<int32> l)->int32{return *(l.begin()+next()%l.size());};
                auto pickF=[&](std::initializer_list<float> l)->float{return *(l.begin()+next()%l.size());};
                const float NaN=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4B41u; if(!st)st=1; trace=json::array(); seed=uint32(i)*7u+uint32(f); rngCalls=0; forceFirst=-1;
                    restoreGlobals();
                    // pool de tareas y de rutas: los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    for(int32 k=0;k<poolCap;++k)if(poolStates[k]&0x80)std::memset(poolStor+k*0x80,0xCD,0x80);
                    for(int32 k=0;k<routeCap;++k)if(routeStates[k]&0x80)std::memset(routeStor+size_t(k)*sizeof(CPointRoute),0xCD,sizeof(CPointRoute));
                    CleanRefs();
                    timeMs=next();
                    const uintptr_t A=uintptr_t(pedA.data()), B=uintptr_t(pedB.data()), C=uintptr_t(pedC.data()), D=uintptr_t(pedD.data());
                    const uintptr_t VA=uintptr_t(vehA.data()), VB=uintptr_t(vehB.data()), OB=uintptr_t(obj.data());
                    // ---------- peatones ----------
                    static const uint32 weaponTypes[]{0,1,2,3,4,5,6,7,8,9,15,16,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46};
                    static const uint32 pedTypes[]{0,1,2,3,4,5,6,6,7,8,18,25};
                    static const float healths[]{0.f,0.5f,0.9999f,1.f,1.0001f,5.f,100.f,-3.f,std::numeric_limits<float>::quiet_NaN()};
                    for(int k=0;k<4;++k) {
                        auto* b=PedBuf(k); std::memset(b,0,sizeof(CPed)); std::memset(MatBuf(k),0,sizeof(CMatrixLink)); Ped(k)->SetType(ENTITY_TYPE_PED); At<void*>(b,0)=pedVtbl.data();
                        const CVector pos{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        if(k==0||next()%3==0) {   // con matriz (el peaton A siempre: el EXE lee el vector adelante sin comprobar)
                            auto* m=MatBuf(k); const CVector r=vec(1.f), u=vec(1.f), a=vec(1.f);
                            At<CVector>(m,0x00)=r; At<CVector>(m,0x10)=u; At<CVector>(m,0x20)=a; At<CVector>(m,0x30)=pos; At<void*>(b,E_MATRIX)=m;
                        } else At<CVector>(b,E_POS)=pos;
                        At<void*>(b,E_CLUMP)=clump.data();
                        At(b,P_FLAGS)=(next()&1u)|((next()%4==0 ? 1u : 0u)<<9);
                        At(b,P_FLAGS2)=next()%4==0 ? (1u<<27) : 0u;
                        At(b,P_FLAGS4)=next()%3==0 ? (1u<<11) : 0u;
                        At<float>(b,P_HEALTH)=healths[next()%9];
                        At(b,P_STATE)=next()%10==0 ? pickI({0x36,0x37}) : pickI({1,2,7,0x20});
                        At(b,P_TYPE)=pedTypes[next()%12];
                        At<uint8>(b,P_CREATED)=uint8(pickI({0,1,1,2,2,3}));
                        for(int w=0;w<13;++w)At(b,P_WEAPONS+size_t(w)*0x1C)=weaponTypes[next()%37];
                        At<int8>(b,P_WSLOT)=int8(next()%13); At<uint8>(b,P_RATE)=uint8(pickI({0,1,10,20,40,41,60,100,255}));
                        At<float>(b,PH_MASS)=uni(1.f,100.f); At<float>(b,PH_BRIGHT)=uni(0.f,1.f); At<float>(b,P_SHIFT_Y)=uni(-1.f,2.f);
                        At<float>(b,P_ROT_CUR)=uni(-3.f,3.f); At<float>(b,P_ROT_AIM)=At<float>(b,P_ROT_CUR)+pickF({0.f,0.005f,0.0099f,0.01f,0.0101f,0.02f,-0.0099f,-0.02f,uni(-3.f,3.f)});
                        At<uintptr_t>(b,PH_IGNORED)=pick({0,VA,VB,0}); At<uintptr_t>(b,P_STANDING_ON)=pick({0,VA,VB});
                        At<void*>(b,P_INTEL)=IntelFor(k);
                        const bool isPlayer=At(b,P_TYPE)<=1u;
                        std::memset(PdBuf(k),0,sizeof(CPlayerPedData)); reinterpret_cast<CPlayerPedData*>(PdBuf(k))->m_fMoveBlendRatio=pickF({0.f,0.5f,0.9999f,1.f,1.5f});
                        At<void*>(b,P_PDATA)=(isPlayer||next()%3==0) ? PdBuf(k) : nullptr;
                        At<uintptr_t>(b,P_VEHICLE)=pick({0,VA,VB});
                    }
                    // ---------- vehiculos, objeto, modelo falso ----------
                    for(int k=0;k<2;++k) {
                        auto* b=VehBuf(k); std::memset(b,0,sizeof(CAutomobile)); std::memset(VMatBuf(k),0,sizeof(CMatrixLink)); EntOf(b)->SetType(ENTITY_TYPE_VEHICLE);
                        At<uint8>(b,E_STATUS)=uint8(At<uint8>(b,E_STATUS)|uint8(pickI({0,0,0,1,2,5,8})<<3));
                        auto* m=VMatBuf(k); CVector fwd=vec(1.f); const float len=std::sqrt(fwd.x*fwd.x+fwd.y*fwd.y+fwd.z*fwd.z); if(len>0.01f)fwd=CVector{fwd.x/len,fwd.y/len,fwd.z/len}; else fwd=CVector{0.f,1.f,0.f};
                        At<CVector>(m,0x00)=vec(1.f); At<CVector>(m,0x10)=fwd; At<CVector>(m,0x20)=vec(1.f); At<CVector>(m,0x30)=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; At<void*>(b,E_MATRIX)=m;
                        At<int16>(b,E_MODEL)=MI_FAKE;
                        const float mag=pickF({0.f,0.0005f,0.001f,0.0011f,0.02f,0.05f,0.0501f,0.2f,0.29f,0.3f,0.31f,0.6f,1.5f,-0.0011f,-0.06f,-0.31f,-0.5f,-1.5f,NaN});
                        At<CVector>(b,E_MOVE)=next()%4==0 ? vec(0.5f) : CVector{fwd.x*mag,fwd.y*mag,fwd.z*mag};
                        At(b,V_TYPE)=uint32(pickI({0,0,2,4,5,9,9,10,11})); At<int32>(b,V_SUBTYPE)=pickI({0,0,0,2,2,9,-1,4});
                        At<uintptr_t>(b,V_DRIVER)=pick({0,B,C,A,D,0}); At<uintptr_t>(b,V_PASSENGER0)=pick({0,B,C,D});
                        At(b,V_HORN)=uint32(pickI({0,0,1,3}));
                    }
                    std::memset(obj.data(),0,obj.size()); EntOf(obj.data())->SetType(ENTITY_TYPE_OBJECT); At<CVector>(obj.data(),E_POS)=vec(30.f);
                    for(auto& b:mi)b=uint8(next()); At<void*>(mi.data(),0x14)=col.data();
                    std::memset(col.data(),0,col.size()); for(int k=0;k<3;++k){ At<float>(col.data(),4*k)=-uni(0.3f,3.f); At<float>(col.data(),0xC+4*k)=uni(0.3f,3.f); }
                    std::memset(clump.data(),0,clump.size()); clump[0]=2;   // rpCLUMP
                    // ---------- tareas falsas ----------
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); At<void*>(fakes[n].data(),0)=taskVtbl.data();
                        static const int32 types[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_COMPLEX_USE_EFFECT,TASK_SIMPLE_DUCK,TASK_COMPLEX_WANDER};
                        At<int32>(fakes[n].data(),FK_TYPE)=types[next()%5]; At<uint8>(fakes[n].data(),FK_ABORT)=uint8(next()%2);
                        At<CTask*>(fakes[n].data(),FK_SUB)=((f==X_ERT0||f==X_ERTPED)&&n<FAKES-1&&next()%4==0) ? Fake(n+1+int(next()%uint32(FAKES-1-n))) : nullptr; }
                    // ---------- eventos falsos ----------
                    for(int n=0;n<=EVENTS;++n){ std::memset(events[n].data(),0,events[n].size()); At<void*>(events[n].data(),0)=eventVtbl.data();
                        At<int32>(events[n].data(),4)=pickI({0,1,5,100}); At<uint8>(events[n].data(),8)=uint8(next()%2);
                        At<int32>(events[n].data(),FE_TYPE)=pickI({9,63,54,12,33,int32(next()%100),-1}); At<uint8>(events[n].data(),FE_PRIO)=uint8(next()%2);
                        At<uint8>(events[n].data(),FE_INTR)=uint8(next()%3==0); At<uintptr_t>(events[n].data(),FE_SRC)=pick({0,B,C,VA}); At<uint8>(events[n].data(),FE_EDIT)=uint8(next()%2); }
                    // ---------- inteligencias ----------
                    for(int k=0;k<2;++k){ auto* in=IntelFor(k); std::memset(in,0,sizeof(CPedIntelligence)); At<uintptr_t>(in,0)=k==0 ? A : B;
                        for(int n=0;n<5;++n)At<CTask*>(in,I_PRIMARY+4*size_t(n))=next()%3==0 ? Fake(int(next()%FAKES)) : nullptr;
                        for(int n=0;n<6;++n)At<CTask*>(in,I_SECONDARY+4*size_t(n))=next()%4==0 ? Fake(int(next()%FAKES)) : nullptr;
                        At<void*>(in,I_HIST_TEMP)=next()%3==0 ? Ev(int(next()%3)) : nullptr; At<void*>(in,I_HIST_NONTEMP)=next()%2 ? Ev(int(next()%3)) : nullptr;
                        At<int8>(in,I_SEQUENCE)=int8(pickI({-1,-1,0,1,5,63,2})); }
                    // ---------- animaciones falsas ----------
                    for(auto* ab:{&assocA,&assocB,&assocC,&assocD}){ std::memset(ab->data(),0,ab->size()); At<void*>(ab->data(),0x14)=hier.data();
                        At<float>(ab->data(),0x18)=pickF({0.3f,0.5f,0.5001f,0.7f,1.f,-1.f,NaN}); At<float>(ab->data(),0x1C)=pickF({-1.f,-0.0001f,0.f,0.5f,4.f,NaN});
                        At<float>(ab->data(),0x20)=pickF({0.f,0.4f,0.99f,1.f,2.f,NaN}); At<int16>(ab->data(),0x0E)=int16(pickI({0,3,5,17}));
                        At<int16>(ab->data(),0x2C)=int16(pickI({115,100,36,39,0x80,28,191,5})); }
                    std::memset(hier.data(),0,hier.size()); At<float>(hier.data(),0x10)=pickF({0.f,0.5f,1.f,2.f,NaN});
                    std::memset(stat.data(),0,stat.size()); At<uint16>(stat.data(),0x0A)=uint16(next()%2 ? 0x400u : 0u)|uint16(next()&0x3FFu);
                    assocStaticRet=uintptr_t(stat.data());
                    assocFoundRet=pick({0,uintptr_t(assocC.data()),uintptr_t(assocC.data())}); assocFirstRet=pick({0,uintptr_t(assocD.data())});
                    assocAddRet=uintptr_t(assocC.data()); assocBlendRet=uintptr_t(assocD.data());
                    cfgGroup=pickI({0,3,5,17}); cfgAnim=pickI({191,192,36,39,28,31,5,32,115,191}); cfgBlend=pickF({1.f,4.f,0.5f,uni(0.f,8.f)}); cfgSpeed=pickF({0.f,1.f,2.f,uni(0.f,3.f)});
                    // ---------- manejador ----------
                    std::memset(hbuf.data(),0,hbuf.size()); At<uintptr_t>(hbuf.data(),0)=A;
                    // ---------- dependencias simuladas ----------
                    looking=next()%3==0; hasFirstPerson=next()%4==0; canJump=next()%2; goingForDoor=next()%2; onMission=next()%2; fallRet=next()%2; fallRail=next()%2; fallDir=pickI({0,1,2,3});
                    playerRet=pick({0,B,A,C}); playerVehRet=pick({0,VA,VB,0}); fightRet=pick({0,uintptr_t(Fake(F_O1))}); duckRet=pick({0,uintptr_t(Fake(F_O2))});
                    gunRet=next()%3==0 ? 0 : uintptr_t(Fake(F_GUN)); At<int32>(Fake(F_GUN),0x10)=pickI({0,1,5,5,6});
                    pedGroupRet=next()%3==0 ? uintptr_t(gbuf.data()) : 0;
                    avoidDir=vec(1.f); evasiveDir=vec(1.f); for(auto& c:corners)c=vec(5.f);
                    dispWrite=pick({0,uintptr_t(Fake(F_O1)),uintptr_t(Fake(F_O2))}); dispPhys=pick({0,0,uintptr_t(Fake(F_O3))}); groupHighest=0;
                    // camara y reloj
                    camActive=uint8(next()%3); for(int n=0;n<3;++n)camMode(n)=int16(pickI({7,0x2E,8,0x33,0x22,0x10,0x2D,4,0,0x20,int32(next()%64)}));
                    miSlot=mi.data();
                    // ---------- evento y argumentos de la funcion ----------
                    void* const h=hbuf.data(); void* const ev=EvMain();
                    std::memset(ev,0,events[EV_MAIN].size()); At<void*>(ev,0)=eventVtbl.data(); At<int32>(ev,4)=pickI({0,1,3}); At<uint8>(ev,8)=1;
                    At<uint8>(ev,FE_EDIT)=0; At<uint8>(ev,FE_PRIO)=uint8(next()%2); At<uint8>(ev,FE_INTR)=uint8(next()%3==0);
                    CTask* tactive=next()%8 ? Fake(F_ACTIVE) : nullptr; CTask* tsimplest=next()%6 ? Fake(F_SIMPLEST) : nullptr; CTask* aborted=next()%3==0 ? Fake(F_O1) : nullptr;
                    uintptr_t sentinel=next()%3==0 ? 0 : next()%2 ? uintptr_t(Fake(F_O3)) : 0x13572468u;   // lo que hay en m_EventResponseTask antes de la llamada
                    auto setInVeh=[&](int k,bool in,uintptr_t veh){ At(PedBuf(k),P_FLAGS)=(At(PedBuf(k),P_FLAGS)&~0x100u)|(in ? 0x100u : 0u); At<uintptr_t>(PedBuf(k),P_VEHICLE)=veh; };
                    auto anyPed=[&](int nullPct)->uintptr_t{ if(int(next()%100)<nullPct)return 0; return uintptr_t(PedBuf(1+int(next()%3))); };
                    auto anyVeh=[&](int nullPct)->uintptr_t{ if(int(next()%100)<nullPct)return 0; return next()%2 ? VA : VB; };
                    int srcPedIdx=1+int(next()%3);
                    uintptr_t ctorRet=0; bool boolRet=false; int32 intRet=0; CTask* taskRet=nullptr; bool hasTaskRet=false;
                    uint32 argWeapon=0; bool argHead=false; uintptr_t argPed=0, argEnt=0;
                    bool noSentinelGarbage=false;
                    auto setPos=[&](uint8* b,const CVector& p){ if(At<void*>(b,E_MATRIX))At<CVector>(At<uint8*>(b,E_MATRIX),0x30)=p; else At<CVector>(b,E_POS)=p; };
                    auto getPos=[&](uint8* b)->CVector{ if(At<void*>(b,E_MATRIX))return At<CVector>(At<uint8*>(b,E_MATRIX),0x30); return At<CVector>(b,E_POS); };
                    auto setPlayer=[&](int k,bool player){ if(player){ At(PedBuf(k),P_TYPE)=0u; At<void*>(PedBuf(k),P_PDATA)=PdBuf(k); } else if(At(PedBuf(k),P_TYPE)<=1u)At(PedBuf(k),P_TYPE)=4u; };
                    // respuestas previas del manejador: valor centinela (distingue "no escribe" de "escribe nullptr")
                    auto sentinels=[&]{
                        At<uintptr_t>(h,0x24)=noSentinelGarbage ? (next()%2 ? uintptr_t(Fake(F_O3)) : 0u) : sentinel;
                        At<uintptr_t>(h,0x20)=noSentinelGarbage ? 0u : pick({0,0,uintptr_t(Fake(F_PHYS)),0x2468ACE0u});
                    };
                    // eventos del historial del manejador
                    auto histEvents=[&]{ At<void*>(h,8)=reinterpret_cast<void*>(pick({0,uintptr_t(Ev(0)),uintptr_t(Ev(1))}));
                                         At<void*>(h,0xC)=reinterpret_cast<void*>(pick({0,0,uintptr_t(Ev(1)),uintptr_t(Ev(0))}));
                                         At<void*>(h,0x10)=reinterpret_cast<void*>(pick({0,0,uintptr_t(Ev(2))})); };
                    switch(f){
                    case X_DAMAGE: {
                        static const int32 ids[]{TASK_NONE,TASK_SIMPLE_BE_DAMAGED,TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH,TASK_COMPLEX_KILL_CRIMINAL,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_FLEE_ANY_MEANS,
                                                 TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_FLEE_ENTITY,TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED,TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,
                                                 TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP,TASK_SIMPLE_DUCK,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR,0x123};
                        static const int32 weapons[]{0,1,2,3,5,15,17,22,23,24,25,28,30,31,32,33,37,38,41,42,43,47,48,49,50,51,53,54,60};
                        At<uint8>(ev,0xC)=uint8(next()%10!=0); At<int16>(ev,0xE)=int16(ids[next()%15]);
                        const int32 weapon=weapons[next()%29];
                        uintptr_t src; { const uint32 m=next()%20; src=m<2 ? 0 : m<12 ? uintptr_t(PedBuf(srcPedIdx)) : m<16 ? (next()%2 ? VA : VB) : OB; }
                        if((weapon==17||weapon==41||weapon==42)&&src&&src!=uintptr_t(PedBuf(srcPedIdx)))src=uintptr_t(PedBuf(srcPedIdx));   // el EXE desreferencia el origen como peaton con armas de asfixia
                        At<uintptr_t>(ev,0x14)=src; At<uint32>(ev,0x18)=next(); At<int32>(ev,0x1C)=weapon;
                        At<int32>(ev,0x20)=pickI({3,4,5,6,7,8,9,2}); At<uint8>(ev,0x24)=uint8(next()%4);
                        At<uint8>(ev,0x25)=uint8((next()%3==0 ? 2u : 0u)|(next()%4==0 ? 4u : 0u)|(next()%2 ? 8u : 0u));
                        At<int32>(ev,0x28)=pickI({0,3,5,17}); At<int32>(ev,0x2C)=pickI({191,192,191,36,39,28,29,30,31,5,32,50});
                        At<float>(ev,0x30)=pickF({1.f,4.f,0.25f,8.f}); At<float>(ev,0x34)=pickF({0.f,1.f,2.f});
                        At<float>(ev,0x38)=pickF({0.f,5.f,10.5f,19.f,20.f,25.f,NaN}); At<float>(ev,0x3C)=pickF({0.f,5.f,9.5f,10.f,0.0001f,20.f});
                        At<uint8>(ev,0x40)=uint8(next()%4==0); At<int32>(ev,FE_TYPE)=9; At<uintptr_t>(ev,FE_SRC)=src;
                        // evento actual del historial: el mismo dano para la IA (origen y arma) o no
                        { auto* ce=Ev(2); const bool same=next()%3==0; At<int32>(ce,FE_TYPE)=next()%5 ? 9 : 63; At<uintptr_t>(ce,0x14)=same ? src : pick({0,B,C}); At<int32>(ce,0x1C)=same ? weapon : weapons[next()%29];
                          const uint32 m=next()%4; if(m==0)At<void*>(h,0xC)=ce; else if(m==1)At<void*>(h,8)=ce; else if(m==2)At<void*>(h,0xC)=ce; }
                        // peaton A: en vehiculo (moto o quad incluidos), jugador, tareas primarias
                        setInVeh(0,next()%5==0,anyVeh(10)); if(!(At(PedBuf(0),P_FLAGS)&0x100u))At<uintptr_t>(PedBuf(0),P_VEHICLE)=pick({0,VA,VB});
                        setInVeh(srcPedIdx,next()%3==0,anyVeh(15));
                        { static const int32 pt[]{TASK_SIMPLE_CHOKING,TASK_SIMPLE_CHOKING,TASK_SIMPLE_STAND_STILL}; At<int32>(Fake(F_CHOKE),FK_TYPE)=pt[next()%3];
                          At<CTask*>(intelA.data(),I_PRIMARY)=next()%4==0 ? Fake(F_CHOKE) : next()%3==0 ? Fake(F_O1) : nullptr; }
                        At<int32>(Fake(F_KILLER),FK_TYPE)=next()%3 ? TASK_SIMPLE_STEALTH_KILL : TASK_SIMPLE_STAND_STILL; At<int32>(Fake(F_KILLER),0x10)=pickI({0,3,5});
                        if(next()%3==0)At<CTask*>(intelB.data(),I_PRIMARY)=Fake(F_KILLER);
                        At<int32>(Fake(F_ACTIVE),FK_TYPE)=pickI({TASK_SIMPLE_STEALTH_KILL,TASK_SIMPLE_STEALTH_KILL,TASK_SIMPLE_STAND_STILL}); At<uint8>(Fake(F_ACTIVE),8)=uint8(next()%2);
                        { static const int32 tt[]{TASK_SIMPLE_FALL,TASK_SIMPLE_GET_UP,TASK_SIMPLE_FALL,TASK_SIMPLE_GET_UP,TASK_SIMPLE_STAND_STILL};
                          At<int32>(Fake(F_SIMPLEST),FK_TYPE)=tt[next()%5]; At<uint8>(Fake(F_SIMPLEST),8)=uint8(next()%2);
                          At<uintptr_t>(Fake(F_SIMPLEST),0xC)=next()%5 ? uintptr_t(assocB.data()) : 0u; At<uintptr_t>(Fake(F_SIMPLEST),0x14)=next()%5 ? uintptr_t(assocA.data()) : 0u; }
                        sentinels();
                        break; }
                    case X_PERS: {
                        static const int32 ids[]{TASK_NONE,TASK_SIMPLE_BE_DAMAGED,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN,
                                                 TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT,TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP,TASK_SIMPLE_DUCK_FOREVER,TASK_SIMPLE_DUCK,
                                                 TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED,TASK_COMPLEX_FLEE_ENTITY,TASK_COMPLEX_SMART_FLEE_ENTITY,
                                                 TASK_COMPLEX_FLEE_ANY_MEANS,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH,
                                                 TASK_COMPLEX_KILL_CRIMINAL,TASK_COMPLEX_KILL_CRIMINAL,0x123,0x2D5,0x39E,0x3E9,0x500,0xCA};
                        At<int16>(ev,0xE)=int16(ids[next()%24]); At<int32>(ev,FE_TYPE)=9;
                        setInVeh(0,next()%3==0,pick({0,VA,VB,VA}));
                        if(next()%4==0)forceFirst=int32(next()%32768);
                        sentinels();
                        break; }
                    case X_DEATH:
                        At<uint8>(ev,0xC)=uint8(next()%2); At<uint32>(ev,0x10)=next(); At<int32>(ev,FE_TYPE)=10; sentinels();
                        break;
                    case X_DEAD: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_INVESTIGATE_DEAD_PED,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_SIMPLE_DUCK_FOREVER,0x123,0x100,TASK_COMPLEX_INVESTIGATE_DEAD_PED};
                        At<int16>(ev,0xE)=int16(ids[next()%7]); const uintptr_t dead=anyPed(10); At<uintptr_t>(ev,0x14)=dead; At<uint8>(ev,0x18)=uint8(next()%2); At<uint32>(ev,0x1C)=next();
                        At<int32>(ev,FE_TYPE)=11; At<uintptr_t>(ev,FE_SRC)=dead;
                        setPlayer(0,next()%100<45);
                        if(dead) { const float d=pickF({0.f,0.5f,0.74f,0.75f,0.7500001f,0.76f,0.7499999f,1.f,10.f,0.3f}); const CVector base=getPos(PedBuf(0));
                                   setPos(reinterpret_cast<uint8*>(dead),CVector{base.x+(next()%2 ? d : -d*0.6f),base.y+(next()%2 ? 0.f : d*0.8f),base.z+uni(-1.f,1.f)}); }
                        fightRet=next()%5==0 ? uintptr_t(Fake(F_O1)) : 0u; looking=next()%5==0;
                        sentinels();
                        break; }
                    case X_VDMG: {
                        static const int32 ids[]{TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_COMPLEX_LEAVE_CAR_AND_FLEE,TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_CAR_DRIVE_MISSION_KILL_PED,
                                                 TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_ROAD_RAGE,TASK_COMPLEX_KILL_CRIMINAL,TASK_NONE,0x123,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_CRIMINAL};
                        At<int16>(ev,0xE)=int16(ids[next()%11]);
                        const uintptr_t veh=next()%2 ? VA : VB; setInVeh(0,next()%2,veh);
                        At<uintptr_t>(ev,0x14)=next()%10<8 ? veh : anyVeh(30);
                        uintptr_t att; { const uint32 m=next()%12; att=m<1 ? 0 : m<7 ? uintptr_t(PedBuf(1+int(next()%3))) : m<11 ? (next()%2 ? VA : VB) : OB; }
                        At<uintptr_t>(ev,0x18)=att; At<int32>(ev,0x1C)=pickI({49,49,0,17,50,22});
                        playerRet=pick({0,B,A,C,att}); At<int32>(ev,FE_TYPE)=41; At<uintptr_t>(ev,FE_SRC)=pick({att,playerRet,0});
                        { const int8 idx=At<int8>(intelA.data(),I_SEQUENCE); if(idx>=0)At<CTask*>(reinterpret_cast<uint8*>(&CTaskSequences::ms_taskSequence)+size_t(idx)*0x40,0x10)=next()%2 ? Fake(F_O1) : nullptr; }
                        sentinels();
                        break; }
                    case X_KNOCK: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_KILL_CRIMINAL,TASK_COMPLEX_FLEE_ENTITY,0x123,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_SMART_FLEE_ENTITY};
                        At<int16>(ev,0xE)=int16(ids[next()%8]); At<uintptr_t>(ev,0x14)=anyVeh(8); At<int32>(ev,FE_TYPE)=43;
                        sentinels();
                        break; }
                    case X_VCOLL: {
                        noSentinelGarbage=true;
                        const uintptr_t veh=anyVeh(4);
                        At<int16>(ev,0xC)=int16(next()); At<int16>(ev,0xE)=int16(pickI({0,0,1,1,2,-1,1}));
                        At<float>(ev,0x10)=pickF({0.f,0.5f,5.f,25.f,100.f,NaN,uni(0.f,50.f)}); At<uintptr_t>(ev,0x14)=veh;
                        At<CVector>(ev,0x18)=vec(1.f); At<CVector>(ev,0x24)=vec(30.f);
                        At<int8>(ev,0x30)=int8(pickI({1,2,3,4,6,7,0,-1,5,8})); At<uint8>(ev,0x31)=uint8(next()%4); At<int32>(ev,FE_TYPE)=1;
                        setPlayer(0,next()%100<45);
                        At(PedBuf(0),P_FLAGS)=(At(PedBuf(0),P_FLAGS)&~0x200u)|(next()%4==0 ? 0x200u : 0u);
                        if(next()%3==0)At<uintptr_t>(PedBuf(0),P_STANDING_ON)=veh; if(next()%3==0)At<uintptr_t>(PedBuf(0),PH_IGNORED)=veh;
                        At<CTask*>(intelA.data(),I_PRIMARY+0xC)=next()%2 ? Fake(F_O1) : nullptr;
                        { static const int32 tt[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_GO_TO_POINT};
                          At<int32>(Fake(F_SIMPLEST),FK_TYPE)=tt[next()%4]; At<CVector>(Fake(F_SIMPLEST),0xC)=vec(30.f); }
                        At<int32>(Fake(F_O3),FK_TYPE)=next()%2 ? TASK_COMPLEX_HIT_PED_WITH_CAR : TASK_SIMPLE_STAND_STILL;
                        sentinels();
                        break; }
                    case X_VPOT: {
                        const uintptr_t veh=anyVeh(4); auto* vb=reinterpret_cast<uint8*>(veh);
                        At<uintptr_t>(ev,0x14)=veh; At<int32>(ev,FE_TYPE)=12;
                        setPlayer(0,next()%100<45); setInVeh(0,next()%12==0,pick({0,VA,VB}));
                        At<uint8>(PedBuf(0),P_CREATED)=uint8(pickI({1,1,1,0,2}));
                        if(vb) {
                            auto* m=At<uint8*>(vb,E_MATRIX); const CVector fwd=At<CVector>(m,0x10), right=At<CVector>(m,0x00), vpos=At<CVector>(m,0x30);
                            const float L=uni(1.f,3.f), W=uni(0.5f,1.5f);
                            auto corner=[&](float sf,float sr){ return CVector{vpos.x+fwd.x*L*sf+right.x*W*sr+uni(-0.2f,0.2f),vpos.y+fwd.y*L*sf+right.y*W*sr+uni(-0.2f,0.2f),vpos.z+fwd.z*L*sf+right.z*W*sr+uni(-0.2f,0.2f)}; };
                            corners[0]=corner(1.f,1.f); corners[3]=corner(1.f,-1.f); corners[1]=corner(-1.f,1.f); corners[2]=corner(-1.f,-1.f);
                            const float s=pickF({-1.2f,-0.5f,-0.31f,-0.3f,-0.2f,-0.06f,-0.04f,0.f,0.04f,0.05f,0.06f,0.29f,0.3f,0.31f,0.5f,1.5f});
                            At<CVector>(vb,E_MOVE)=CVector{fwd.x*s,fwd.y*s,fwd.z*s};
                            At<uint8>(vb,E_STATUS)=uint8((At<uint8>(vb,E_STATUS)&7)|(pickI({0,0,1,2})<<3));
                            setPos(PedBuf(0),CVector{vpos.x+fwd.x*uni(-6.f,6.f)+right.x*uni(-4.f,4.f),vpos.y+fwd.y*uni(-6.f,6.f)+right.y*uni(-4.f,4.f),vpos.z+uni(-1.f,1.f)});
                        }
                        if(next()%5==0)forceFirst=int32(next()%32768);
                        sentinels();
                        break; }
                    case X_ERT0: case X_ERTPED: {
                        static const int32 types[]{1,2,3,4,5,6,7,8,9,10,11,12,13,15,16,17,18,19,20,21,25,26,27,28,31,32,33,35,36,37,38,39,41,42,43,44,45,46,48,49,50,51,52,53,56,58,59,62,63,64,65,66,67,69,70,72,73,74,75,79,81,88,89,93,94,
                                                   0,14,22,23,24,29,30,34,40,47,54,55,57,60,61,68,71,76,77,78,80,82,83,84,85,86,87,90,91,92,95,100,-1};
                        At<int32>(ev,FE_TYPE)=types[next()%std::size(types)];
                        for(int n=0;n<5;++n)At<uintptr_t>(h,0x20+4*size_t(n))=next()%2 ? uintptr_t(Fake(int(next()%FAKES))) : 0x2468ACE0u;
                        break; }
                    case X_HANDLE: {
                        histEvents(); noSentinelGarbage=true;
                        CTask* primary=nullptr; for(int n=0;n<3&&!primary;++n)primary=At<CTask*>(intelA.data(),I_PRIMARY+4*size_t(n));
                        At<void*>(h,4)=reinterpret_cast<void*>(pick({0,uintptr_t(primary),uintptr_t(primary),uintptr_t(Fake(F_O1))}));
                        groupHighest=pick({0,uintptr_t(Ev(4)),uintptr_t(Ev(4)),uintptr_t(Ev(0)),uintptr_t(Ev(1))});
                        { const int32 ty=next()%2 ? 9 : pickI({63,54,12}); At<int32>(Ev(4),FE_TYPE)=ty;
                          if(auto* cur=At<uint8*>(h,0xC) ? At<uint8*>(h,0xC) : At<uint8*>(h,8); cur && next()%2)At<int32>(cur,FE_TYPE)=ty; }
                        for(int n=0;n<5;++n)if(next()%2)At<CTask*>(intelA.data(),I_PRIMARY+4*size_t(n))=Fake(int(next()%FAKES));
                        dispWrite=pick({0,uintptr_t(Fake(F_O1))}); dispPhys=pick({0,uintptr_t(Fake(F_PHYS))});
                        break; }
                    case X_FLUSH: case X_FLUSHIMM:
                        histEvents();
                        for(int n=0;n<5;++n)At<uintptr_t>(h,0x20+4*size_t(n))=next()%3 ? uintptr_t(Fake(F_PHYS+n)) : 0u;
                        break;
                    case X_SETRESP: {
                        static const int32 types[]{1,2,3,4,5,6,33,56,60,44,13,46,67,93,12,74,59,9,63,0,100,10,41};
                        At<int32>(ev,FE_TYPE)=types[next()%23]; At<int16>(ev,0x10)=int16(pickI({-1,-1,0,1,5,0x7FFF})); At<uint8>(ev,FE_EDIT)=uint8(next()%3!=0);
                        At<uintptr_t>(h,0x20)=next()%2 ? uintptr_t(Fake(F_PHYS)) : 0u; At<uintptr_t>(h,0x24)=next()%2 ? uintptr_t(Fake(F_O1)) : 0u;
                        At<uintptr_t>(h,0x28)=next()%2 ? uintptr_t(Fake(F_ATTACK)) : 0u; At<uintptr_t>(h,0x2C)=next()%2 ? uintptr_t(Fake(F_SAY)) : 0u;
                        At<uintptr_t>(h,0x30)=next()%2 ? uintptr_t(Fake(F_PARTIAL)) : 0u;
                        At<CTask*>(intelA.data(),I_SECONDARY)=next()%2 ? Fake(F_O2) : nullptr; At<CTask*>(intelA.data(),I_SECONDARY+12)=next()%2 ? Fake(F_O3) : nullptr;
                        break; }
                    case X_REGKILL: {
                        argPed=pick({0,A,B,C,A}); argEnt=pick({0,B,C,D,VA,VB,OB,A,argPed}); argWeapon=uint32(pickI({0,22,38,49,5,51})); argHead=next()%2;
                        playerVehRet=pick({0,argEnt,VA,VB});
                        break; }
                    case X_CTOR:
                        std::memset(h,0xCD,hbuf.size());
                        break;
                    case X_DTOR:
                        histEvents(); for(int n=0;n<5;++n)At<uintptr_t>(h,0x20+4*size_t(n))=next()%3 ? uintptr_t(Fake(F_PHYS+n)) : 0u;
                        break;
                    case X_ISKILL:
                        At<int32>(ev,FE_TYPE)=pickI({9,63,12});
                        break;
                    case X_ISTEMP:
                        At<int32>(ev,FE_TYPE)=i<110 ? i-5 : pickI({int32(next()%200)-20,33,93,60});
                        break;
                    case X_GETCUR:
                        At<void*>(h,0xC)=reinterpret_cast<void*>(pick({0,uintptr_t(Ev(0)),uintptr_t(Ev(1))})); At<void*>(h,8)=reinterpret_cast<void*>(pick({0,uintptr_t(Ev(1)),uintptr_t(Ev(2))}));
                        break;
                    }
                    const auto occ=Occupancy();
                    for(size_t k=0;k<NFNS;++k)if(!excluded[k])hooks[k]->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    switch(f){
                    case X_DAMAGE: reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*,CTask*)>(fn.address)(h,ev,tactive,tsimplest,aborted); break;
                    case X_PERS: reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(fn.address)(h,ev,PedBuf(srcPedIdx)); break;
                    case X_DEATH: case X_DEAD: case X_VDMG: case X_KNOCK: case X_VCOLL: case X_VPOT:
                        reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(fn.address)(h,ev,tactive,tsimplest); break;
                    case X_ERT0: reinterpret_cast<void(__thiscall*)(void*,void*,CTask*)>(fn.address)(h,ev,aborted); break;
                    case X_ERTPED: taskRet=reinterpret_cast<CTask*(__cdecl*)(const void*,const void*)>(fn.address)(PedBuf(0),ev); hasTaskRet=true; break;
                    case X_HANDLE: case X_FLUSH: case X_FLUSHIMM: reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(h); break;
                    case X_SETRESP: reinterpret_cast<void(__thiscall*)(void*,const void*)>(fn.address)(h,ev); break;
                    case X_REGKILL: reinterpret_cast<void(__cdecl*)(const void*,const void*,uint32,bool)>(fn.address)(reinterpret_cast<void*>(argPed),reinterpret_cast<void*>(argEnt),argWeapon,argHead); break;
                    case X_CTOR: { const int k=int(next()%4); ctorRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(fn.address)(h,PedBuf(k))); break; }
                    case X_DTOR: ctorRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(fn.address)(h)); break;
                    case X_ISKILL: { const int a=int(next()%4), b=int(next()%4); boolRet=reinterpret_cast<bool(__cdecl*)(void*,void*,const void*)>(fn.address)(PedBuf(a),PedBuf(b),ev); break; }
                    case X_ISTEMP: boolRet=reinterpret_cast<bool(__cdecl*)(const void*)>(fn.address)(ev); break;
                    case X_GETCUR: intRet=reinterpret_cast<int32(__thiscall*)(void*)>(fn.address)(h); break;
                    }
                    for(size_t k=0;k<NFNS;++k)if(!excluded[k])hooks[k]->State(false);
                    miSlot=savedMi;
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(), types=json::array();
                    for(auto* t:fresh){ tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x80,true)}}); types.push_back(t->GetTaskType()); }
                    json rv={{"resp",Name(At<uintptr_t>(h,0x24))},{"types",types}};
                    if(hasTaskRet)rv["ret"]=Name(uintptr_t(taskRet));
                    if(f==X_CTOR)rv["ret"]=Name(ctorRet);   // el destructor del EXE (0x4C3EC0) no define EAX: no se compara
                    if(f==X_ISKILL||f==X_ISTEMP)rv["bool"]=boolRet;
                    if(f==X_GETCUR)rv["int"]=intRet;
                    if(f==X_REGKILL){ rv["havoc"]=havoc; rv["chase"]=F(chase); }
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int k=0;k<4;++k)refs.push_back(refList(EntOf(PedBuf(k)))); refs.push_back(refList(EntOf(vehA.data()))); refs.push_back(refList(EntOf(vehB.data()))); refs.push_back(refList(EntOf(obj.data())));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),fakes[n].size(),true));
                    json eventWords=json::array(); for(int n=0;n<=EVENTS;++n)eventWords.push_back(Region(events[n].data(),events[n].size(),true));
                    json r={{"ret",rv},{"handler",Region(h,hbuf.size())},{"tasks",tasks},
                            {"peds",{EntHash(pedA),EntHash(pedB),EntHash(pedC),EntHash(pedD)}},
                            {"mats",{Hash(matA.data(),matA.size()),Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size()),Hash(matD.data(),matD.size())}},
                            {"vehs",{EntHash(vehA),EntHash(vehB)}},{"vmats",{Hash(vmatA.data(),vmatA.size()),Hash(vmatB.data(),vmatB.size())}},{"obj",EntHash(obj)},
                            {"intel",{Region(intelA.data(),intelA.size()),Region(intelB.data(),intelB.size())}},
                            {"pd",{Hash(pdA.data(),pdA.size()),Hash(pdB.data(),pdB.size()),Hash(pdC.data(),pdC.size()),Hash(pdD.data(),pdD.size())}},
                            {"assocs",{Hash(assocA.data(),assocA.size()),Hash(assocB.data(),assocB.size()),Hash(assocC.data(),assocC.size()),Hash(assocD.data(),assocD.size()),Hash(hier.data(),hier.size()),Hash(stat.data(),stat.size())}},
                            {"fakes",fakeWords},{"events",eventWords},{"refs",refs},{"clump",Hash(clump.data(),clump.size())},
                            {"seq",Hash(&seqArr,sizeof(seqArr))},{"routes",Hash(routeStates,size_t(routeCap))},{"player",{havoc,F(chase)}},{"time",timeMs}};
                    r["trace"]=trace; r["rng"]=rngCalls; r["switches"]=switches;
                    restoreGlobals(); CleanRefs();
                    r["clean"]=Occupancy()==occupancyStart;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            for(size_t k=0;k<NFNS;++k)hooks[k]->State(false);
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    restoreGlobals(); CleanRefs(); timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime
        &&std::memcmp(poolStates,savedPoolStates.data(),savedPoolStates.size())==0&&*reinterpret_cast<int32*>(poolHdr+0xC)==poolLf0
        &&std::memcmp(routeStates,savedRouteStates.data(),savedRouteStates.size())==0&&camActive==savedCamActive&&havoc==savedHavoc&&miSlot==savedMi}});
}
}
