#pragma once
// Banco dentro del motor: CEventHandler::Compute*Response (grupo B, 19 funciones): AreaCodes, Attractor,
// BuildingCollision, CarUpsideDown, ChatPartner, CopCarBeingStolen, CreatePartnerTask, Danger, DontJoinGroup,
// DraggedOutCar, FireNearby, GunAimedAt, HighAngerAtPlayer, InWater, InteriorUseInfo, LowAngerAtPlayer, LowHealth,
// ObjectCollisionPassive y ObjectCollision.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: el manejador (CEventHandler de 0x34
// bytes con m_Ped en +0 y m_EventResponseTask en +0x24, relleno con un valor centinela), cuatro peatones, dos vehiculos
// (con su CHandlingData), edificio, objeto, inteligencia, datos de jugador y un CWanted en buffers propios; eventos con
// la disposicion del EXE y vtable trazada; tareas falsas (activa, simple, y las de la inteligencia con KillPedOnFoot,
// SeekEntity, EnterCarAsPassengerWait y PartnerChat) con vtable trazada. Se compara el valor final de
// m_EventResponseTask (distinguiendo "no escribe" de "escribe nullptr"), TODAS las tareas nuevas del pool (tipo y 0x80
// bytes con los punteros traducidos, en el orden de los slots), las referencias, los fixtures y la traza.
// Se simulan y trazan: FindPlayerPed, CPed::Say, CPedAttractorManager::HasEmptySlot, IKChainManager_c::IsLooking/LookAt,
// CTaskSimpleClimb::TestForClimb, CPedGroups::IsInPlayersGroup, CPedGeometryAnalyser::CanPedJumpObstacle/CanPedTargetPed,
// CWanted::SetWantedLevelNoDrop, CEventBuildingCollision::CanTreatBuildingAsObject/IsHeadOnCollision,
// CPedIntelligence::IsFriendlyWith/GetTaskDuck y el RNG. Los constructores de CTaskComplexLeaveCar, CTaskComplexDragPedFromCar
// y CTaskSimpleAchieveHeading no tienen hook (0x63B8C0, 0x640430, 0x667E20): en las dos ejecuciones se redirigen a la DLL
// para que solo se compare la logica de las funciones. El pool de tareas (estados y m_LastFreeSlot, huecos libres a 0xCD)
// y el reloj se restauran en cada ejecucion.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventBuildingCollision.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexDragPedFromCar.h"
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskSimpleClimb.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Entity/Vehicle/Automobile.h"
#include "Ragdoll/IKChainManager.h"
#include "PedAttractorManager.h"
#include "PedGroups.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "PlayerPedData.h"
#include "tHandlingData.h"
#include "Wanted.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace EvHBLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
// tareas falsas: 0 KillPedOnFoot (objetivo en +0x10), 1 SeekEntity (entidad en +0xC), 2 EnterCarAsPassengerWait (coche en +0xC),
// 3 PartnerChat, 4-5 otras de la inteligencia, 6 tarea activa, 7 tarea simple (GoTo: punto en +0xC), 8 tarea de agacharse, 9 relleno
constexpr int FAKES=10;
enum { F_KILL, F_SEEK, F_ENTER, F_CHAT, F_O1, F_O2, F_ACTIVE, F_SIMPLEST, F_DUCK, F_GEN };
constexpr size_t FK_TYPE=0x38;
// desplazamientos crudos del EXE (miembros protegidos o para no depender de la DLL)
constexpr size_t P_FLAGS=0x46C, P_FLAGS2=0x478, P_INTEL=0x47C, P_PDATA=0x480, P_CREATED=0x484, P_VEHICLE=0x58C, P_TYPE=0x598,
                 P_WEAPONS=0x5A0, P_WSLOT=0x718;
constexpr size_t V_HANDLING=0x384, V_DRIVER=0x460, V_PASSENGER0=0x464;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{}, pedD{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, matB{}, matC{}, matD{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(tHandlingData)> hndA{}, hndB{};
alignas(16) inline std::array<uint8,0x300> bld{}, obj{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdB{}, pdC{}, pdD{};
alignas(16) inline std::array<uint8,0x40> wanted{}, effect{}, interiorInfo{}, interior{}, hbuf{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> event{};
inline uint8* PedBuf(int k) { switch(k){case 0:return pedA.data();case 1:return pedB.data();case 2:return pedC.data();default:return pedD.data();} }
inline uint8* MatBuf(int k) { switch(k){case 0:return matA.data();case 1:return matB.data();case 2:return matC.data();default:return matD.data();} }
inline uint8* PdBuf(int k) { switch(k){case 1:return pdB.data();case 2:return pdC.data();default:return pdD.data();} }
inline uint8* VehBuf(int k) { return k==0 ? vehA.data() : vehB.data(); }
inline uint8* HndBuf(int k) { return k==0 ? hndA.data() : hndB.data(); }
inline CPed* Ped(int k) { return reinterpret_cast<CPed*>(PedBuf(k)); }
inline CEntity* EntOf(void* b) { return reinterpret_cast<CEntity*>(b); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline std::unordered_map<uintptr_t,int> freshIds;
// pool de CPointRoute (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot): las tareas de rodear edificios reservan rutas
inline uint8* RouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* RouteStorage() { return *reinterpret_cast<uint8**>(RouteHdr()); }
inline uint8* RouteStates() { return *reinterpret_cast<uint8**>(RouteHdr()+4); }
inline size_t RouteCap() { return *reinterpret_cast<uint32*>(RouteHdr()+8); }
constexpr size_t ROUTE_SZ=sizeof(CPointRoute);

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(pedC.data(),pedC.size(),"pedC"))return *s; if(auto s=in(pedD.data(),pedD.size(),"pedD"))return *s;
    if(auto s=in(matA.data(),matA.size(),"matA"))return *s; if(auto s=in(matB.data(),matB.size(),"matB"))return *s;
    if(auto s=in(matC.data(),matC.size(),"matC"))return *s; if(auto s=in(matD.data(),matD.size(),"matD"))return *s;
    if(auto s=in(vehA.data(),vehA.size(),"vehA"))return *s; if(auto s=in(vehB.data(),vehB.size(),"vehB"))return *s;
    if(auto s=in(hndA.data(),hndA.size(),"hndA"))return *s; if(auto s=in(hndB.data(),hndB.size(),"hndB"))return *s;
    if(auto s=in(bld.data(),bld.size(),"bld"))return *s; if(auto s=in(obj.data(),obj.size(),"obj"))return *s;
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(pdB.data(),pdB.size(),"pdB"))return *s; if(auto s=in(pdC.data(),pdC.size(),"pdC"))return *s; if(auto s=in(pdD.data(),pdD.size(),"pdD"))return *s;
    if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s; if(auto s=in(effect.data(),effect.size(),"effect"))return *s;
    if(auto s=in(interiorInfo.data(),interiorInfo.size(),"iinfo"))return *s; if(auto s=in(interior.data(),interior.size(),"interior"))return *s;
    if(auto s=in(hbuf.data(),hbuf.size(),"handler"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    { const auto rs=uintptr_t(RouteStorage()); if(v>=rs&&v<rs+RouteCap()*ROUTE_SZ)return "route#"+std::to_string((v-rs)/ROUTE_SZ)+"+"+std::to_string((v-rs)%ROUTE_SZ); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline int eventType{}; inline bool canTreat{}, headOn{}, hasEmpty{}, looking{}, inGroup{}, jumpRet{}, targetRet{}, friendly{};
inline uintptr_t playerRet{}, climbEnt{}, duckRet{}; inline CVector climbPos{}; inline float climbHeading{}; inline int32 climbSurf{};
inline int32 forceFirst=-1; inline uint32 seed{}, rngCalls{};

// tareas falsas: GetTaskType devuelve el tipo guardado en cada objeto (sin traza: las llamadas al tipo no tienen efectos)
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline int __fastcall FType(CTask* t,void*) { return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() {
    trace.push_back({"rand"}); int v=int((seed*2654435761u+rngCalls*40503u+rngCalls*rngCalls*977u)>>9)&0x7FFF;
    if(rngCalls==0&&forceFirst>=0)v=forceFirst; ++rngCalls; return v;
}
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline bool __fastcall HasEmpty(CPedAttractorManager*,void*,const C2dEffectPedAttractor* fx,const CEntity* en) {
    trace.push_back({"has_empty_slot",Name(uintptr_t(fx)),Name(uintptr_t(en))}); return hasEmpty;
}
inline bool __fastcall IsLooking(const IKChainManager_c*,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(uintptr_t(p))}); return looking; }
inline void __fastcall LookAt(IKChainManager_c*,void*,const char* purpose,CPed* p,CEntity* en,int32 time,eBoneTag32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"ik_look_at",std::string(purpose),Name(uintptr_t(p)),Name(uintptr_t(en)),time,int(bone),Name(uintptr_t(offset)),torso,F(speed),blend,prio,force});
}
inline CEntity* __cdecl ClimbStub(CPed* ped,CVector& pos,float& heading,eSurfaceType& surf,bool launch) {
    trace.push_back({"test_for_climb",Name(uintptr_t(ped)),launch});
    if(climbEnt){ pos=climbPos; heading=climbHeading; surf=eSurfaceType(climbSurf); }
    return reinterpret_cast<CEntity*>(climbEnt);
}
inline bool __cdecl InGroup(CPed* p) { trace.push_back({"in_players_group",Name(uintptr_t(p))}); return inGroup; }
inline bool __cdecl JumpObstacle(const CPed& p,const CEntity& e,const CVector& n,const CVector& pos) {
    trace.push_back({"can_jump_obstacle",Name(uintptr_t(&p)),Name(uintptr_t(&e)),F(n.x),F(n.y),F(n.z),F(pos.x),F(pos.y),F(pos.z)}); return jumpRet;
}
inline bool __cdecl TargetPed(CPed& p,CPed& t,bool dir) { trace.push_back({"can_target_ped",Name(uintptr_t(&p)),Name(uintptr_t(&t)),dir}); return targetRet; }
inline void __fastcall SetWanted(CWanted* w,void*,eWantedLevel lvl) { trace.push_back({"set_wanted_no_drop",Name(uintptr_t(w)),uint32(lvl)}); }
inline bool __cdecl CanTreat(CBuilding* b) { trace.push_back({"can_treat_building_as_object",Name(uintptr_t(b))}); return canTreat; }
inline bool __fastcall HeadOn(CEventBuildingCollision* e,void*,CPed* p) { trace.push_back({"is_head_on",Name(uintptr_t(e)),Name(uintptr_t(p))}); return headOn; }
inline bool __fastcall Friendly(const CPedIntelligence* i,void*,const CPed& p) { trace.push_back({"is_friendly_with",Name(uintptr_t(i)),Name(uintptr_t(&p))}); return friendly; }
inline CTaskSimpleDuck* __fastcall GetDuck(CPedIntelligence* i,void*,bool b) { trace.push_back({"get_task_duck",Name(uintptr_t(i)),b}); return reinterpret_cast<CTaskSimpleDuck*>(duckRet); }
// constructores sin hook en el EXE: las dos ejecuciones usan los de la DLL
inline void* __fastcall LeaveCarCtor(CTaskComplexLeaveCar* t,void*,CVehicle* v,int32 door,int32 delay,bool sensible,bool force) { ::new (t) CTaskComplexLeaveCar(v,door,delay,sensible,force); return t; }
inline void* __fastcall DragCtor(CTaskComplexDragPedFromCar* t,void*,CPed* p,int32 time) { ::new (t) CTaskComplexDragPedFromCar(p,time); return t; }
inline void* __fastcall HeadingCtor(CTaskSimpleAchieveHeading* t,void*,float a,float b,float c) { ::new (t) CTaskSimpleAchieveHeading(a,b,c); return t; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Fx { X_AREA, X_ATTR, X_BLDG, X_UPSD, X_CHAT, X_COPCAR, X_PARTNER, X_DANGER, X_DONTJOIN, X_DRAG, X_FIRE, X_GUN, X_HIGH, X_WATER, X_INTERIOR, X_LOW, X_LOWHP, X_OBJPASS, X_OBJ };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=19;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputeAreaCodesResponse",0x4BBF50,0,-1,64},
    {"CEventHandler","ComputeAttractorResponse",0x4B9BE0,0,-1,256},
    {"CEventHandler","ComputeBuildingCollisionResponse",0x4BF2B0,0,-1,1024},
    {"CEventHandler","ComputeCarUpsideDownResponse",0x4BBC30,0,-1,128},
    {"CEventHandler","ComputeChatPartnerResponse",0x4B98E0,0,-1,128},
    {"CEventHandler","ComputeCopCarBeingStolenResponse",0x4BB740,0,-1,256},
    {"CEventHandler","ComputeCreatePartnerTaskResponse",0x4BB130,0,-1,256},
    {"CEventHandler","ComputeDangerResponse",0x4BC230,0,-1,256},
    {"CEventHandler","ComputeDontJoinGroupResponse",0x4BC1D0,0,-1,64},
    {"CEventHandler","ComputeDraggedOutCarResponse",0x4BCC30,0,-1,1024},
    {"CEventHandler","ComputeFireNearbyResponse",0x4BBFB0,0,-1,128},
    {"CEventHandler","ComputeGunAimedAtResponse",0x4C2840,0,-1,1024},
    {"CEventHandler","ComputeHighAngerAtPlayerResponse",0x4BAC10,0,-1,128},
    {"CEventHandler","ComputeInWaterResponse",0x4BAF80,0,-1,64},
    {"CEventHandler","ComputeInteriorUseInfoResponse",0x4BAFE0,0,-1,128},
    {"CEventHandler","ComputeLowAngerAtPlayerResponse",0x4BAAD0,0,-1,128},
    {"CEventHandler","ComputeLowHealthResponse",0x4BA990,0,-1,128},
    {"CEventHandler","ComputeObjectCollisionPassiveResponse",0x4BBB90,0,-1,256},
    {"CEventHandler","ComputeObjectCollisionResponse",0x4B92B0,0,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int k=0;k<4;++k)clean(EntOf(PedBuf(k)));
    clean(EntOf(vehA.data())); clean(EntOf(vehB.data())); clean(EntOf(bld.data())); clean(EntOf(obj.data()));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evhb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhb");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhb-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado del pool de tareas: se restaura en cada ejecucion (estados y m_LastFreeSlot) para que ambas reciban los mismos slots
    auto* poolHdr=reinterpret_cast<uint8*>(GetTaskPool()); auto* poolStor=*reinterpret_cast<uint8**>(poolHdr); auto* poolStates=*reinterpret_cast<uint8**>(poolHdr+4);
    const int32 poolCap=*reinterpret_cast<int32*>(poolHdr+8); const int32 poolLf0=*reinterpret_cast<int32*>(poolHdr+0xC);
    const std::vector<uint8> savedPoolStates(poolStates,poolStates+poolCap);
    const std::vector<uint8> savedRouteHdr(RouteHdr(),RouteHdr()+0x14), savedRouteStates(RouteStates(),RouteStates()+RouteCap()),
                             savedRouteStorage(RouteStorage(),RouteStorage()+RouteCap()*ROUTE_SZ);
    auto restoreRoutes=[&]{ std::memcpy(RouteStates(),savedRouteStates.data(),savedRouteStates.size()); std::memcpy(RouteStorage(),savedRouteStorage.data(),savedRouteStorage.size());
                            std::memcpy(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size()); };
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), s1e(0x56E210,FindPlayer);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),Say), s2e(0x5EFFE0,Say);
        EntryPatch s3(bit_cast<uintptr_t>(&CPedAttractorManager::HasEmptySlot),HasEmpty), s3e(0x5EBB00,HasEmpty);
        EntryPatch s4(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), s4e(0x6181A0,IsLooking);
        EntryPatch s5(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), s5e(0x618970,LookAt);
        EntryPatch s6(bit_cast<uintptr_t>(&CTaskSimpleClimb::TestForClimb),ClimbStub), s6e(0x6803A0,ClimbStub);
        EntryPatch s7(bit_cast<uintptr_t>(&CPedGroups::IsInPlayersGroup),InGroup), s7e(0x5F7F10,InGroup);
        EntryPatch s8(bit_cast<uintptr_t>(static_cast<bool(*)(const CPed&,const CEntity&,const CVector&,const CVector&)>(&CPedGeometryAnalyser::CanPedJumpObstacle)),JumpObstacle), s8e(0x5F32D0,JumpObstacle);
        EntryPatch s9(bit_cast<uintptr_t>(&CPedGeometryAnalyser::CanPedTargetPed),TargetPed), s9e(0x5F1C40,TargetPed);
        EntryPatch s10(bit_cast<uintptr_t>(&CWanted::SetWantedLevelNoDrop),SetWanted), s10e(0x562570,SetWanted);
        EntryPatch s11(bit_cast<uintptr_t>(&CEventBuildingCollision::CanTreatBuildingAsObject),CanTreat), s11e(0x4B3120,CanTreat);
        EntryPatch s12(bit_cast<uintptr_t>(&CEventBuildingCollision::IsHeadOnCollision),HeadOn), s12e(0x4AD1E0,HeadOn);
        EntryPatch s13(bit_cast<uintptr_t>(&CPedIntelligence::IsFriendlyWith),Friendly), s13e(0x601BC0,Friendly);
        EntryPatch s14(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),GetDuck), s14e(0x6010A0,GetDuck);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch c1(0x63B8C0,LeaveCarCtor), c2(0x640430,DragCtor), c3(0x667E20,HeadingCtor);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&]{return CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};};
                auto run=[&](bool candidate)->json {
                    // pool de tareas: estados, m_LastFreeSlot y huecos libres con 0xCD (el New del EXE no escribe en el objeto)
                    std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
                    for(int32 k=0;k<poolCap;++k)if(poolStates[k]&0x80)std::memset(poolStor+k*0x80,0xCD,0x80);
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4B48u; if(!st)st=1; trace=json::array(); seed=uint32(i)*7u+uint32(f); rngCalls=0; forceFirst=-1;
                    CleanRefs(); restoreRoutes();
                    { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if(states[k]&0x80)std::memset(RouteStorage()+k*ROUTE_SZ,0xCD,ROUTE_SZ); }
                    const std::vector<uint8> preRouteStates(RouteStates(),RouteStates()+RouteCap());
                    timeMs=next();
                    // ---------- peatones (A es el del manejador) ----------
                    static const uint32 weaponTypes[]{0,1,2,4,22,25,30};
                    static const uint32 pedTypes[]{0,1,4,5,6,6,7,8,18,25};
                    for(int k=0;k<4;++k) {
                        auto* b=PedBuf(k); std::memset(b,0,sizeof(CPed)); std::memset(MatBuf(k),0,sizeof(CMatrixLink)); auto* p=Ped(k); p->SetType(ENTITY_TYPE_PED);
                        if(next()%4==0){ At<float>(MatBuf(k),0x30)=uni(-30.f,30.f); At<float>(MatBuf(k),0x34)=uni(-30.f,30.f); At<float>(MatBuf(k),0x38)=uni(0.f,40.f); p->m_matrix=reinterpret_cast<CMatrixLink*>(MatBuf(k)); }
                        else p->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                        At(b,P_FLAGS)=next()&1u; At(b,P_FLAGS2)=(next()%2)<<11; At(b,P_TYPE)=next()%5 ? pedTypes[next()%10] : 6u;
                        { static const uint8 cb[]{0,1,1,1,2,3}; At<uint8>(b,P_CREATED)=cb[next()%6]; }
                        At<int8>(b,P_WSLOT)=int8(next()%13); for(int w=0;w<13;++w)At(b,P_WEAPONS+w*0x1C)=weaponTypes[next()%7];
                        if(k>=1){ std::memset(PdBuf(k),0,sizeof(CPlayerPedData)); reinterpret_cast<CPlayerPedData*>(PdBuf(k))->m_pWanted=reinterpret_cast<CWanted*>(wanted.data()); At<void*>(b,P_PDATA)=PdBuf(k); }
                    }
                    std::memset(intel.data(),0,intel.size()); At<void*>(pedA.data(),P_INTEL)=intel.data();
                    // ---------- vehiculos, edificio, objeto ----------
                    for(int k=0;k<2;++k) {
                        auto* b=VehBuf(k); std::memset(b,0,sizeof(CAutomobile)); EntOf(b)->SetType(ENTITY_TYPE_VEHICLE); EntOf(b)->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                        std::memset(HndBuf(k),0,sizeof(tHandlingData)); reinterpret_cast<tHandlingData*>(HndBuf(k))->m_nAnimGroup=next()%6==0 ? 15 : uint8(next()%15);
                        At<void*>(b,V_HANDLING)=HndBuf(k);
                        { uintptr_t d[]{0,uintptr_t(pedA.data()),uintptr_t(pedB.data()),uintptr_t(pedC.data())}; At<uintptr_t>(b,V_DRIVER)=d[next()%4]; At<uintptr_t>(b,V_PASSENGER0)=d[next()%4]; }
                    }
                    std::memset(bld.data(),0,bld.size()); EntOf(bld.data())->SetType(ENTITY_TYPE_BUILDING); EntOf(bld.data())->SetPosn(vec());
                    std::memset(obj.data(),0,obj.size()); EntOf(obj.data())->SetType(ENTITY_TYPE_OBJECT); EntOf(obj.data())->SetPosn(vec());
                    std::memset(wanted.data(),0xAB,wanted.size()); for(auto& b:effect)b=uint8(next()); for(auto& b:interiorInfo)b=uint8(next()); for(auto& b:interior)b=uint8(next());
                    // ---------- tareas falsas ----------
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,fakes[n].size()); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        static const int32 types[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_COMPLEX_USE_EFFECT,TASK_SIMPLE_DUCK};
                        At<int32>(fakes[n].data(),FK_TYPE)=types[next()%4]; }
                    At<int32>(fakes[F_KILL].data(),FK_TYPE)=TASK_COMPLEX_KILL_PED_ON_FOOT; At<int32>(fakes[F_SEEK].data(),FK_TYPE)=TASK_COMPLEX_SEEK_ENTITY;
                    At<int32>(fakes[F_ENTER].data(),FK_TYPE)=TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT; At<int32>(fakes[F_CHAT].data(),FK_TYPE)=TASK_COMPLEX_PARTNER_CHAT;
                    { uintptr_t t[]{0,uintptr_t(pedB.data()),uintptr_t(pedC.data()),uintptr_t(pedD.data()),uintptr_t(pedA.data())}; At<uintptr_t>(fakes[F_KILL].data(),0x10)=t[next()%5==0 ? 0 : 1+next()%4]; }
                    { uintptr_t t[]{0,uintptr_t(pedB.data()),uintptr_t(bld.data()),uintptr_t(obj.data())}; At<uintptr_t>(fakes[F_SEEK].data(),0xC)=t[next()%4]; }
                    { uintptr_t t[]{0,uintptr_t(vehA.data()),uintptr_t(vehB.data())}; At<uintptr_t>(fakes[F_ENTER].data(),0xC)=t[next()%3]; }
                    { const CVector goTo=vec(); At<float>(fakes[F_SIMPLEST].data(),0xC)=goTo.x; At<float>(fakes[F_SIMPLEST].data(),0x10)=goTo.y; At<float>(fakes[F_SIMPLEST].data(),0x14)=goTo.z; }
                    { auto place=[&](int fk,int pct){ if(int(next()%100)<pct)At<CTask*>(intel.data(),4+4*(next()%5))=Fake(fk); };
                      place(F_KILL,45); place(F_SEEK,45); place(F_ENTER,45); place(F_CHAT,30); place(F_O1,40); place(F_O2,40); }
                    // ---------- evento y manejador ----------
                    for(auto& b:event)b=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    eventType=EVENT_ATTRACTOR;
                    const uintptr_t sentinel=next()%3==0 ? 0 : next()%2 ? uintptr_t(Fake(F_GEN)) : 0x13572468u;
                    std::memset(hbuf.data(),0,hbuf.size()); At<CPed*>(hbuf.data(),0)=Ped(0); At<uintptr_t>(hbuf.data(),0x24)=sentinel;
                    // ---------- dependencias simuladas ----------
                    canTreat=next()%4==0; headOn=next()%100<55; hasEmpty=next()%2; looking=next()%5<2; inGroup=next()%5<3; jumpRet=next()%5<2; targetRet=next()%3==0; friendly=next()%3==0;
                    playerRet=next()%3==0 ? 0 : next()%2 ? uintptr_t(pedB.data()) : uintptr_t(pedA.data());
                    climbEnt=next()%100<45 ? (next()%2 ? uintptr_t(bld.data()) : uintptr_t(obj.data())) : 0; climbPos=vec(); climbHeading=uni(-3.2f,3.2f); climbSurf=int32(next()%80);
                    duckRet=next()%2 ? uintptr_t(Fake(F_DUCK)) : 0;
                    auto setInVeh=[&](CPed* p,bool in,uintptr_t veh){ At(p,P_FLAGS)=(At(p,P_FLAGS)&~0x100u)|(in ? 0x100u : 0u); At<uintptr_t>(p,P_VEHICLE)=veh; };
                    auto anyVeh=[&]()->uintptr_t{ return next()%2 ? uintptr_t(vehA.data()) : uintptr_t(vehB.data()); };
                    auto anyPedNull=[&](int nullPct)->uintptr_t{ if(int(next()%100)<nullPct)return 0; return uintptr_t(PedBuf(1+next()%3)); };
                    auto anyVehNull=[&](int nullPct)->uintptr_t{ if(int(next()%100)<nullPct)return 0; return anyVeh(); };
                    void* const ev=event.data(); void* const h=hbuf.data();
                    CTask* tactive=next()%10 ? Fake(F_ACTIVE) : nullptr; CTask* tsimplest=next()%5 ? Fake(F_SIMPLEST) : nullptr;
                    auto setTask=[&](size_t off,int32 id){ At<int16>(ev,off)=int16(id); };
                    setInVeh(Ped(0),false,0);
                    switch(f){
                    case X_AREA:
                        At<uintptr_t>(ev,0xC)=anyPedNull(20) ? uintptr_t(PedBuf(next()%4)) : 0;
                        break;
                    case X_ATTR: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_USE_EFFECT,TASK_COMPLEX_USE_EFFECT,TASK_COMPLEX_USE_EFFECT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT};
                        setTask(0xE,ids[next()%6]); eventType=next()%5 ? EVENT_ATTRACTOR : EVENT_SCRIPTED_ATTRACTOR;
                        At<uintptr_t>(ev,0x14)=next()%8 ? uintptr_t(effect.data()) : 0; { uintptr_t t[]{0,uintptr_t(bld.data()),uintptr_t(obj.data()),uintptr_t(pedB.data())}; At<uintptr_t>(ev,0x18)=t[next()%4==0 ? 0 : 1+next()%3]; }
                        break; }
                    case X_BLDG: {
                        static const int16 mss[]{1,1,0,-1,2,3,4,4,5,5,6,6,7,7,8};
                        At<int16>(ev,0xC)=int16(next()); At<int16>(ev,0xE)=mss[next()%15]; At<float>(ev,0x10)=uni(0.f,20.f);
                        At<uintptr_t>(ev,0x14)=next()%12 ? uintptr_t(bld.data()) : 0;
                        { static const float zs[]{0.9f,0.5f,0.707f,0.70700006f,0.7069999f,-0.5f,0.f,1.f,std::numeric_limits<float>::quiet_NaN()}; At<float>(ev,0x18)=uni(-1.f,1.f); At<float>(ev,0x1C)=uni(-1.f,1.f); At<float>(ev,0x20)=next()%2 ? zs[next()%9] : uni(-1.f,1.f); }
                        { const CVector ip=vec(); At<float>(ev,0x24)=ip.x; At<float>(ev,0x28)=ip.y; At<float>(ev,0x2C)=ip.z; }
                        At(Ped(0),P_FLAGS)=(At(Ped(0),P_FLAGS)&~0x100u)|(next()&1u);
                        tsimplest=Fake(F_SIMPLEST); At<int32>(fakes[F_SIMPLEST].data(),FK_TYPE)=next()%100<55 ? (next()%2 ? TASK_SIMPLE_GO_TO_POINT : TASK_SIMPLE_GO_TO_POINT_FINE) : TASK_SIMPLE_STAND_STILL;
                        break; }
                    case X_UPSD: {
                        const uintptr_t v=anyVehNull(10); At<uintptr_t>(ev,0xC)=v;
                        const uint32 m=next()%4; setInVeh(Ped(0),next()%10<7,m<2 ? v : m==2 ? anyVeh() : 0);
                        break; }
                    case X_CHAT:
                        At<uint8>(ev,0xC)=uint8(next()%2); At<uintptr_t>(ev,0x10)=anyPedNull(15);
                        break;
                    case X_COPCAR: {
                        At<uintptr_t>(ev,0xC)=anyPedNull(10); const uintptr_t v=anyVehNull(10); At<uintptr_t>(ev,0x10)=v;
                        const uint32 m=next()%4; setInVeh(Ped(0),next()%10<7,m<2 ? v : m==2 ? anyVeh() : 0);
                        break; }
                    case X_PARTNER: {
                        static const int32 ts[]{-1,0,0,1,2,3,4,5,6,6,7,100,INT32_MIN};
                        At<int32>(ev,0xC)=ts[next()%13]; At<uintptr_t>(ev,0x10)=anyPedNull(10); At<uint8>(ev,0x14)=uint8(next()%2); At<float>(ev,0x18)=next()%5 ? uni(0.f,10.f) : 0.f;
                        break; }
                    case X_DANGER: {
                        static const int32 ids[]{TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_NONE,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_KILL_PED_ON_FOOT};
                        setTask(0xE,ids[next()%7]);
                        { uintptr_t t[]{0,uintptr_t(pedB.data()),uintptr_t(pedC.data()),uintptr_t(vehA.data()),uintptr_t(bld.data())}; At<uintptr_t>(ev,0x14)=t[next()%10==0 ? 0 : 1+next()%4]; }
                        if(next()%2)setInVeh(Ped(0),true,anyVeh());
                        break; }
                    case X_DONTJOIN: case X_WATER: break;
                    case X_DRAG: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_ENTER_CAR_AS_PASSENGER,TASK_COMPLEX_STEAL_CAR,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT,
                                                 TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_KILL_CRIMINAL,TASK_SIMPLE_HIT_HEAD,TASK_COMPLEX_EVASIVE_STEP,TASK_SIMPLE_DUCK_FOREVER};
                        const int32 id=ids[next()%11]; setTask(0xE,id);
                        const uintptr_t v=next()%12 ? anyVeh() : 0; At<uintptr_t>(ev,0x18)=v; At<uint8>(ev,0x1C)=uint8(next()%2);
                        // el EXE desreferencia al ladron nulo cuando la tarea es TASK_COMPLEX_STEAL_CAR (siempre) o al tirar el aleatorio "girarse"
                        const bool needJacker=id==TASK_COMPLEX_STEAL_CAR;
                        At<uintptr_t>(ev,0x14)=(!needJacker&&next()%6==0) ? 0 : anyPedNull(0);
                        if(id==TASK_COMPLEX_KILL_PED_ON_FOOT&&!At<uintptr_t>(ev,0x14))forceFirst=0x100;
                        if(next()%100<55&&v)At<uintptr_t>(VehBuf(v==uintptr_t(vehA.data()) ? 0 : 1),next()%2 ? V_DRIVER : V_PASSENGER0)=uintptr_t(pedA.data());
                        if(next()%100<8)At(Ped(0),P_TYPE)=0u;
                        break; }
                    case X_FIRE: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_EXTINGUISH_FIRES,TASK_COMPLEX_EXTINGUISH_FIRES,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_KILL_PED_ON_FOOT};
                        setTask(0xE,ids[next()%5]);
                        break; }
                    case X_GUN: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT,
                                                 TASK_SIMPLE_COWER,TASK_SIMPLE_HANDS_UP,TASK_SIMPLE_DUCK,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_REACT_TO_GUN_AIMED_AT,TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH,
                                                 TASK_COMPLEX_KILL_CRIMINAL,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_FLEE_ANY_MEANS,TASK_COMPLEX_CAR_DRIVE_MISSION_FLEE_SCENE,TASK_COMPLEX_FLEE_ENTITY,
                                                 TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_FLEE_POINT,TASK_COMPLEX_DESTROY_CAR};
                        const int32 id=ids[next()%18]; setTask(0xE,id);
                        const bool nullOk=id==TASK_NONE||id==TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR||id==TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN||id==TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT||
                                          id==TASK_SIMPLE_COWER||id==TASK_SIMPLE_HANDS_UP||id==TASK_SIMPLE_DUCK||id==TASK_SIMPLE_DUCK_FOREVER||id==TASK_COMPLEX_REACT_TO_GUN_AIMED_AT||id==TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH||
                                          id==TASK_COMPLEX_FLEE_ANY_MEANS||id==TASK_COMPLEX_FLEE_ENTITY||id==TASK_COMPLEX_DESTROY_CAR;
                        At<uintptr_t>(ev,0x14)=anyPedNull(nullOk ? 8 : 0);
                        { static const int32 ts[]{TASK_COMPLEX_FLEE_ENTITY,TASK_COMPLEX_FLEE_POINT,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_SMART_FLEE_POINT,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT};
                          if(next()%100<30){ tactive=Fake(F_ACTIVE); At<int32>(fakes[F_ACTIVE].data(),FK_TYPE)=ts[next()%6]; } else if(next()%10==0)tactive=nullptr; }
                        { const uint32 m=next()%4; setInVeh(Ped(0),next()%10<3,m<2 ? anyVeh() : m==2 ? 0 : anyVeh()); if(At(Ped(0),P_FLAGS)&0x100u&&!At<uintptr_t>(Ped(0),P_VEHICLE))At<uintptr_t>(Ped(0),P_VEHICLE)=anyVeh(); }
                        if(next()%100<10)At(Ped(0),P_TYPE)=0u;
                        if(next()%100<40&&At<uintptr_t>(Ped(0),P_VEHICLE))At<uintptr_t>(VehBuf(At<uintptr_t>(Ped(0),P_VEHICLE)==uintptr_t(vehA.data()) ? 0 : 1),V_DRIVER)=uintptr_t(pedA.data());
                        break; }
                    case X_HIGH: case X_LOW: {
                        static const int32 ids[]{TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_NONE,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_DESTROY_CAR,TASK_SIMPLE_STAND_STILL};
                        setTask(0xE,ids[next()%6]);
                        break; }
                    case X_INTERIOR:
                        At<uintptr_t>(ev,0xC)=next()%6 ? uintptr_t(interiorInfo.data()) : 0; At<uintptr_t>(ev,0x10)=next()%6 ? uintptr_t(interior.data()) : 0;
                        At<uint32>(ev,0x14)=next()%3 ? next()%20000 : next(); At<int8>(ev,0x18)=int8(next()%2);
                        break;
                    case X_LOWHP: {
                        static const int32 ids[]{TASK_NONE,TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP,TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT};
                        setTask(0xE,ids[next()%5]);
                        break; }
                    case X_OBJPASS: {
                        At<uintptr_t>(ev,0x14)=next()%10 ? uintptr_t(obj.data()) : 0; { static const int32 mss[]{1,1,0,-1,2,3,4,5,6,7,8}; At<int32>(ev,0x18)=mss[next()%11]; }
                        setInVeh(Ped(0),next()%4==0,0); if(At(Ped(0),P_FLAGS)&0x100u)At<uintptr_t>(Ped(0),P_VEHICLE)=anyVeh();
                        tsimplest=next()%5 ? Fake(F_SIMPLEST) : nullptr; At<int32>(fakes[F_SIMPLEST].data(),FK_TYPE)=next()%100<60 ? (next()%2 ? TASK_SIMPLE_GO_TO_POINT : TASK_SIMPLE_GO_TO_POINT_FINE) : TASK_SIMPLE_STAND_STILL;
                        break; }
                    case X_OBJ: {
                        if(!Ped(0)->m_matrix){ At<float>(matA.data(),0x30)=uni(-30.f,30.f); At<float>(matA.data(),0x34)=uni(-30.f,30.f); At<float>(matA.data(),0x38)=uni(0.f,40.f);
                            At<float>(matA.data(),0x20)=uni(-1.f,1.f); At<float>(matA.data(),0x24)=uni(-1.f,1.f); At<float>(matA.data(),0x28)=uni(-1.f,1.f);
                            Ped(0)->m_matrix=reinterpret_cast<CMatrixLink*>(matA.data()); }
                        At<int16>(ev,0xC)=int16(next()); { static const int16 mss[]{1,1,0,-1,2,3,4,5,6,7,8}; At<int16>(ev,0xE)=mss[next()%11]; } At<float>(ev,0x10)=uni(0.f,20.f);
                        At<uintptr_t>(ev,0x14)=next()%10 ? uintptr_t(obj.data()) : 0;
                        setInVeh(Ped(0),next()%4==0,0); if(At(Ped(0),P_FLAGS)&0x100u)At<uintptr_t>(Ped(0),P_VEHICLE)=anyVeh();
                        tsimplest=next()%5 ? Fake(F_SIMPLEST) : nullptr; At<int32>(fakes[F_SIMPLEST].data(),FK_TYPE)=next()%100<60 ? (next()%2 ? TASK_SIMPLE_GO_TO_POINT : TASK_SIMPLE_GO_TO_POINT_FINE) : TASK_SIMPLE_STAND_STILL;
                        break; }
                    }
                    const auto occ=Occupancy();
                    for(auto* hk:hooks)hk->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(fn.address)(h,ev,tactive,tsimplest);
                    for(auto* hk:hooks)hk->State(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x80,true)}});
                    const json resp=Name(At<uintptr_t>(h,0x24));
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int k=0;k<4;++k)refs.push_back(refList(EntOf(PedBuf(k)))); refs.push_back(refList(EntOf(vehA.data()))); refs.push_back(refList(EntOf(vehB.data())));
                    refs.push_back(refList(EntOf(bld.data()))); refs.push_back(refList(EntOf(obj.data())));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),fakes[n].size(),true));
                    json types=json::array(); for(auto* t:fresh)types.push_back(t->GetTaskType());
                    json routes=json::array(); { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if((preRouteStates[k]&0x80)&&!(states[k]&0x80))routes.push_back(Region(RouteStorage()+k*ROUTE_SZ,ROUTE_SZ)); }
                    json r={{"routes",routes},{"route_hdr",Hash(RouteHdr(),0x14)},{"ret",{{"resp",resp},{"types",types}}},{"handler",Region(h,0x34)},{"tasks",tasks},
                            {"ped_a",EntHash(pedA)},{"ped_b",EntHash(pedB)},{"ped_c",EntHash(pedC)},{"ped_d",EntHash(pedD)},
                            {"mats",{Hash(matA.data(),matA.size()),Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size()),Hash(matD.data(),matD.size())}},
                            {"veh_a",EntHash(vehA)},{"veh_b",EntHash(vehB)},{"bld",EntHash(bld)},{"obj",EntHash(obj)},
                            {"hnd",{Hash(hndA.data(),hndA.size()),Hash(hndB.data(),hndB.size())}},{"intel",Region(intel.data(),intel.size())},
                            {"pd",{Hash(pdB.data(),pdB.size()),Hash(pdC.data(),pdC.size()),Hash(pdD.data(),pdD.size())}},
                            {"misc",{Hash(wanted.data(),wanted.size()),Hash(effect.data(),effect.size()),Hash(interiorInfo.data(),interiorInfo.size()),Hash(interior.data(),interior.size())}},
                            {"fakes",fakeWords},{"event",Region(event.data(),event.size(),true)},{"refs",refs},{"time",timeMs}};
                    r["trace"]=trace; r["rng"]=rngCalls; r["switches"]=switches;
                    // las tareas nuevas no se destruyen: se restauran los estados del pool al empezar la siguiente ejecucion
                    std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
                    r["clean"]=Occupancy()==occupancyStart;
                    CleanRefs(); restoreRoutes();
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
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreRoutes(); std::memcpy(poolStates,savedPoolStates.data(),savedPoolStates.size()); *reinterpret_cast<int32*>(poolHdr+0xC)=poolLf0;
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime}});
}
}
