#pragma once
// Banco dentro del motor: grupo D de CEventHandler (13 funciones Compute*Response de EventHandler.cpp):
// ComputeSeenCopResponse, ComputeSeenPanickedPedResponse, ComputeSexyPedResponse, ComputeSexyVehicleResponse,
// ComputeShotFiredResponse, ComputeShotFiredWhizzedByResponse, ComputeSignalAtPedResponse, ComputeSpecialResponse,
// ComputeVehicleDiedResponse, ComputeVehicleOnFireResponse, ComputeVehiclePotentialPassiveCollisionResponse,
// ComputeVehicleToStealResponse y ComputeWaterCannonResponse.
// Cada funcion se llama por la direccion del EXE (el hook decide si ejecuta el EXE o la DLL) con un CEventHandler falso
// (m_Ped en +0, historial en +4, m_EventResponseTask en +0x24), un evento construido a mano (campos y vtable del EXE o
// vtable falsa con trampas) y los argumentos tactive/tsimplest (tareas falsas con vtable propia).
// Se compara TODO: el valor final de m_EventResponseTask (tambien cuando la funcion NO lo escribe: el handler se
// rellena con un centinela), las tareas nuevas del pool de tareas (tipo + 0x70 bytes, con los slots libres rellenos con
// 0xCD y m_LastFreeSlot restaurado), las rutas nuevas del pool de CPointRoute, los peatones/vehiculos/objeto falsos
// (menos el nodo de referencias), las listas de referencias, las tareas/eventos falsos, el reloj, el paso de tiempo y
// la traza de las dependencias simuladas.
// Se simulan y trazan: FindPlayerPed, CEventHandler::IsKillTaskAppropriate (estatica cdecl en EXE y DLL),
// ComputeKnockOffBikeResponse, CInterestingEvents::Add, IKChainManager_c::LookAt, CPed::Say,
// CPedIntelligence::GetTaskDuck (lector puro: sin traza), CTaskSimpleDuck::RestartTask, CPedGroups::GetPedsGroup,
// CPedIntelligence::IsPedGoingForCarDoor, CPedGeometryAnalyser::ComputePedHitSide, CPhysical::ApplyMoveForce(CVector)
// y el RNG (rand del EXE 0x821B1E + CGeneral::GetRandomNumber).
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventHandler.h"
#include "Events/Event.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "InterestingEvents.h"
#include "Ragdoll/IKChainManager.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "PedIntelligence.h"
#include "Pools/Pools.h"
#include "Pools/PointRoutePool.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace EvHDLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr size_t E_POS=0x04, E_MATRIX=0x14, E_FLAGS=0x1C, E_INFO=0x36, PH_FLAGS=0x40, PH_SPEED=0x44,
                 P_FLAGS1=0x46C, P_INTEL=0x47C, P_HEALTH=0x540, P_VEHICLE=0x58C, P_TYPE=0x598,
                 V_DRIVER=0x460, V_HEALTH=0x4C0, V_TYPE=0x590, V_SUBTYPE=0x594;
constexpr size_t I_HIST_NONTEMP=0x3C, I_HIST_TEMP=0x40;      // CPedIntelligence (historial del manejador de eventos)
constexpr size_t H_PED=0x00, H_HIST_NONTEMP=0x08, H_HIST_TEMP=0x0C, H_RESP=0x24;   // CEventHandler
constexpr size_t FK_TYPE=0x38, FK_SUB=0x3C;                  // tarea/evento falsos
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIMESTEP=0xB7CB5C;
constexpr int FAKES=10, EVS=4;

alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{}, pedC{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(CObject)> objA{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{}, intelC{};
alignas(16) inline std::array<uint8,0x40> hnd{}, object{}, matB{}, matC{}, groupFake{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<std::array<uint8,0x40>,EVS> evs{};
inline CPed* Ped(std::array<uint8,sizeof(CPed)>& b) { return reinterpret_cast<CPed*>(b.data()); }
inline CPed* PedA() { return Ped(pedA); } inline CPed* PedB() { return Ped(pedB); } inline CPed* PedC() { return Ped(pedC); }
inline CVehicle* VehA() { return reinterpret_cast<CVehicle*>(vehA.data()); } inline CVehicle* VehB() { return reinterpret_cast<CVehicle*>(vehB.data()); }
inline CObject* ObjA() { return reinterpret_cast<CObject*>(objA.data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }

// pool de CPointRoute (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* RouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* RouteStorage() { return *reinterpret_cast<uint8**>(RouteHdr()); }
inline uint8* RouteStates() { return *reinterpret_cast<uint8**>(RouteHdr()+4); }
inline size_t RouteCap() { return *reinterpret_cast<uint32*>(RouteHdr()+8); }
constexpr size_t ROUTE_SZ=sizeof(CPointRoute);

inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
#define EVHD_IN(buf,nm) if(auto s=in(buf.data(),buf.size(),nm))return *s;
    EVHD_IN(pedA,"pedA") EVHD_IN(pedB,"pedB") EVHD_IN(pedC,"pedC") EVHD_IN(vehA,"vehA") EVHD_IN(vehB,"vehB") EVHD_IN(objA,"objA")
    EVHD_IN(intelA,"intelA") EVHD_IN(intelB,"intelB") EVHD_IN(intelC,"intelC") EVHD_IN(hnd,"hnd") EVHD_IN(object,"object")
    EVHD_IN(matB,"matB") EVHD_IN(matC,"matC") EVHD_IN(groupFake,"group")
#undef EVHD_IN
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    for(int n=0;n<EVS;++n)if(auto s=in(evs[n].data(),evs[n].size(),"ev"))return *s+"#"+std::to_string(n);
    { const auto rs=uintptr_t(RouteStorage()); if(v>=rs&&v<rs+RouteCap()*ROUTE_SZ)return "route#"+std::to_string((v-rs)/ROUTE_SZ)+"+"+std::to_string((v-rs)%ROUTE_SZ); }
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
inline uintptr_t playerRet{}, duckRet{}, groupRet{}; inline bool killOk{}, goingDoor{}; inline uint32 hitSalt{}, seed{}, rngCalls{};

// tareas falsas: lectores puros (no se trazan: la DLL y el EXE pueden llamarlos un numero distinto de veces)
inline int __fastcall FType(CTask* t,void*) { return At<int32>(t,FK_TYPE); }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
template<int N> void* __fastcall FTrap(void* t,void*) { trace.push_back({"task_unexpected_slot",N,Name(t)}); return nullptr; }
inline std::array<void*,16> taskVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// eventos falsos: GetEventType (hueco 1) y GetSourceEntity (hueco 10)
inline int __fastcall EType(void* e,void*) { return At<int32>(e,FK_TYPE); }
inline CEntity* __fastcall ESource(void* e,void*) { return At<CEntity*>(e,FK_SUB); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
inline std::array<void*,20> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
// EXE 0x4BC3E0 y DLL (estatica): cdecl(ped, ped, evento)
inline bool __cdecl KillExe(CPed* a,CPed* b,const CEvent* e) { trace.push_back({"is_kill_task_appropriate",Name(a),Name(b),Name(e)}); return killOk; }
inline void __fastcall KnockOff(void* self,void*,CEvent* e,CTask* a,CTask* b) { trace.push_back({"knock_off_bike",Name(self),Name(e),Name(a),Name(b)}); }
inline void __fastcall IntAdd(void*,void*,int32 type,CEntity* e) { trace.push_back({"interesting_event_add",type,Name(e)}); }
inline void __fastcall LookAtStub(void*,void*,const char* purpose,CPed* ped,CEntity* ent,int32 time,int32 bone,CVector* offset,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"look_at",std::string(purpose),Name(ped),Name(ent),time,bone,Name(offset),torso,F(speed),blend,prio,force});
}
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline CTaskSimpleDuck* __fastcall GetDuck(CPedIntelligence*,void*,bool) { return reinterpret_cast<CTaskSimpleDuck*>(duckRet); }
inline void __fastcall DuckRestart(CTaskSimpleDuck* t,void*,CPed* p) { trace.push_back({"duck_restart",Name(t),Name(p)}); }
inline CPedGroup* __cdecl GetGroupStub(const CPed* p) { trace.push_back({"get_peds_group",Name(p)}); return reinterpret_cast<CPedGroup*>(groupRet); }
inline bool __fastcall GoingForDoor(CPedIntelligence* i,void*) { trace.push_back({"is_ped_going_for_car_door",Name(i)}); return goingDoor; }
inline int32 __cdecl HitSide(const CPed* p,const CVector* v) {
    trace.push_back({"compute_ped_hit_side",Name(p),F(v->x),F(v->y),F(v->z)});
    return int32(((F(v->x)*2654435761u)^(F(v->y)*40503u)^F(v->z)^hitSalt)%4u);
}
inline void __fastcall ApplyForce(CPed* p,void*,CVector v) { trace.push_back({"apply_move_force",Name(p),F(v.x),F(v.y),F(v.z)}); }
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Cls { SEENCOP, SEENPAN, SEXYPED, SEXYVEH, SHOT, WHIZZ, SIGNAL, SPECIAL, VDIED, VFIRE, PASSIVE, STEAL, WATER };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; Cls c; };
constexpr size_t NFNS=13;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputeSeenCopResponse",0x4BC050,0,-1,512,SEENCOP},
    {"CEventHandler","ComputeSeenPanickedPedResponse",0x4C35F0,0,-1,1024,SEENPAN},
    {"CEventHandler","ComputeSexyPedResponse",0x4B99F0,0,-1,256,SEXYPED},
    {"CEventHandler","ComputeSexyVehicleResponse",0x4B9AA0,0,-1,256,SEXYVEH},
    {"CEventHandler","ComputeShotFiredResponse",0x4BC710,0,-1,2048,SHOT},
    {"CEventHandler","ComputeShotFiredWhizzedByResponse",0x4BBE30,0,-1,1024,WHIZZ},
    {"CEventHandler","ComputeSignalAtPedResponse",0x4BB050,0,-1,256,SIGNAL},
    {"CEventHandler","ComputeSpecialResponse",0x4BB800,0,-1,1024,SPECIAL},
    {"CEventHandler","ComputeVehicleDiedResponse",0x4BA8B0,0,-1,512,VDIED},
    {"CEventHandler","ComputeVehicleOnFireResponse",0x4BB2E0,0,-1,1024,VFIRE},
    {"CEventHandler","ComputeVehiclePotentialPassiveCollisionResponse",0x4B96D0,0,-1,1024,PASSIVE},
    {"CEventHandler","ComputeVehicleToStealResponse",0x4B9F80,0,-1,256,STEAL},
    {"CEventHandler","ComputeWaterCannonResponse",0x4BAE30,0,-1,512,WATER},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(PedC()); clean(VehA()); clean(VehB()); clean(ObjA());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evhd-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhd-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhd");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhd-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,16>{}); taskVtbl[2]=reinterpret_cast<void*>(&FSub); taskVtbl[3]=reinterpret_cast<void*>(&FSimple); taskVtbl[4]=reinterpret_cast<void*>(&FType);
    FillETraps(std::make_integer_sequence<int,20>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[10]=reinterpret_cast<void*>(&ESource);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion
    auto& lfSlot=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfSlot;
    const std::vector<uint8> savedRouteHdr(RouteHdr(),RouteHdr()+0x14), savedRouteStates(RouteStates(),RouteStates()+RouteCap()),
                             savedRouteStorage(RouteStorage(),RouteStorage()+RouteCap()*ROUTE_SZ);
    auto restoreRoutes=[&]{ std::memcpy(RouteStates(),savedRouteStates.data(),savedRouteStates.size()); std::memcpy(RouteStorage(),savedRouteStorage.data(),savedRouteStorage.size());
                            std::memcpy(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size()); };
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIMESTEP); const float savedTimeStep=timeStep;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), s1e(0x56E210,FindPlayer);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<bool(*)(CPed*,CPed*,const CEvent&)>(&CEventHandler::IsKillTaskAppropriate)),KillExe), s2e(0x4BC3E0,KillExe);
        EntryPatch s3(bit_cast<uintptr_t>(&CEventHandler::ComputeKnockOffBikeResponse),KnockOff), s3e(0x4B9FF0,KnockOff);
        EntryPatch s4(bit_cast<uintptr_t>(&CInterestingEvents::Add),IntAdd), s4e(0x602590,IntAdd);
        EntryPatch s5(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAtStub), s5e(0x618970,LookAtStub);
        EntryPatch s6(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),Say), s6e(0x5EFFE0,Say);
        EntryPatch s7(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),GetDuck), s7e(0x6010A0,GetDuck);
        EntryPatch s8(bit_cast<uintptr_t>(&CTaskSimpleDuck::RestartTask),DuckRestart), s8e(0x692390,DuckRestart);
        EntryPatch s9e(0x5F7E80,GetGroupStub);   // CPedGroups::GetPedsGroup de la DLL llama al EXE
        EntryPatch s10(bit_cast<uintptr_t>(&CPedIntelligence::IsPedGoingForCarDoor),GoingForDoor), s10e(0x602350,GoingForDoor);
        EntryPatch s11(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CVector&)>(&CPedGeometryAnalyser::ComputePedHitSide)),HitSide), s11e(0x5F1E70,HitSide);
        EntryPatch s12(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),ApplyForce), s12e(0x5429F0,ApplyForce);
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto vec=[&](float r){return CVector{uni(-r,r),uni(-r,r),uni(-r,r)};};
                auto pick=[&](std::initializer_list<uintptr_t> l){return *(l.begin()+next()%l.size());};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    lfSlot=lf0;
                    // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4848u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    CleanRefs(); restoreRoutes();
                    { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if(states[k]&0x80)std::memset(RouteStorage()+k*ROUTE_SZ,0xCD,ROUTE_SZ); }
                    const std::vector<uint8> preRouteStates(RouteStates(),RouteStates()+RouteCap());
                    const auto occ=Occupancy();
                    const uintptr_t A=uintptr_t(PedA()), B=uintptr_t(PedB()), C=uintptr_t(PedC()), VA=uintptr_t(VehA()), VB=uintptr_t(VehB()), OA=uintptr_t(ObjA());
                    const CVector center=vec(20.f);
                    // peatones
                    for(auto* buf:{&pedA,&pedB,&pedC}){ auto* p=buf->data(); std::memset(p,0,buf->size()); reinterpret_cast<CEntity*>(p)->SetType(ENTITY_TYPE_PED);
                        At<CVector>(p,E_POS)=center+vec(10.f); At<uint8>(p,E_FLAGS)=uint8(next());
                        At(p,PH_FLAGS)=next(); At<CVector>(p,PH_SPEED)=vec(0.3f);
                        At(p,P_FLAGS1)=next(); At(p,P_TYPE)=uint32(pick({0,1,4,6,6,20,20,7,8})); At<float>(p,P_HEALTH)=uni(0.f,100.f);
                        At<uintptr_t>(p,P_VEHICLE)=pick({0,VA,VA,VB}); }
                    At<void*>(pedA.data(),P_INTEL)=intelA.data(); At<void*>(pedB.data(),P_INTEL)=intelB.data(); At<void*>(pedC.data(),P_INTEL)=intelC.data();
                    for(auto* mb:{&matB,&matC}){ std::memset(mb->data(),0,mb->size()); At<CVector>(mb->data(),0x30)=center+vec(10.f); }
                    At<void*>(pedB.data(),E_MATRIX)=next()%2 ? matB.data() : nullptr; At<void*>(pedC.data(),E_MATRIX)=next()%2 ? matC.data() : nullptr;
                    // vehiculos y objeto
                    for(auto* buf:{&vehA,&vehB}){ auto* v=buf->data(); std::memset(v,0,buf->size()); reinterpret_cast<CEntity*>(v)->SetType(ENTITY_TYPE_VEHICLE);
                        At<uint8>(v,E_INFO)|=uint8(pick({5,5,5,0,1,2,3,4,6,7})<<3);   // STATUS_WRECKED (5) frecuente
                        At<CVector>(v,E_POS)=center+vec(6.f);
                        { const auto k=next()%12; At<float>(v,V_HEALTH)=k==0 ? std::bit_cast<float>(0x7FC00000u) : k<4 ? 0.f : k<7 ? uni(-50.f,0.f) : uni(0.f,1000.f); }
                        At(v,V_TYPE)=uint32(pick({0,9,9,10,2,0})); At<int32>(v,V_SUBTYPE)=int32(pick({0,2,2,9,uint32(-1),5})); At<uintptr_t>(v,V_DRIVER)=pick({A,B,0,A}); }
                    std::memset(objA.data(),0,objA.size()); ObjA()->SetType(ENTITY_TYPE_OBJECT); At<CVector>(objA.data(),E_POS)=center+vec(4.f);
                    // tareas falsas (tipo en +0x38): +0xE es el contador de disparos de un duck falso y +0xC el punto de un GoTo falso
                    static const uint32 taskTypes[]{0x19F,0x19F,0x384,0x39E,0xD0,0x2C0,0x6C,0x137,0x201};
                    for(int n=0;n<FAKES;++n){ auto* t=fakes[n].data(); for(auto& b:fakes[n])b=uint8(next()); At<void*>(t,0)=taskVtbl.data(); At<void*>(t,4)=nullptr;
                        At<uint32>(t,FK_TYPE)=taskTypes[next()%std::size(taskTypes)]; At<CTask*>(t,FK_SUB)=nullptr;
                        if(At<uint32>(t,FK_TYPE)==0x19F)At<int16>(t,0xE)=int16(pick({0,0,uint32(-1),1,5,uint32(-3),100,0x7FFF}));   // contador de un duck falso
                        else At<CVector>(t,0xC)=center+vec(12.f); }   // punto de un GoTo falso (valores finitos: la DLL y el EXE lo copian en bruto)
                    // eventos falsos del historial: tipo y entidad origen
                    for(auto& e:evs){ auto* ev=e.data(); for(auto& b:e)b=uint8(next()); At<void*>(ev,0)=eventVtbl.data(); At<uint32>(ev,FK_TYPE)=uint32(next()%100);
                        At<uintptr_t>(ev,FK_SUB)=pick({A,B,C,VA,VB,OA,0,C}); }
                    auto histEv=[&]()->void*{ const auto k=next()%(EVS+2); return k<EVS ? static_cast<void*>(evs[k].data()) : nullptr; };
                    for(auto* buf:{&intelA,&intelB,&intelC}){ std::memset(buf->data(),0,buf->size()); At<void*>(buf->data(),I_HIST_TEMP)=histEv(); At<void*>(buf->data(),I_HIST_NONTEMP)=histEv(); }
                    // CEventHandler falso: se rellena con ceros y centinela en m_EventResponseTask para ver las rutas que no escriben
                    std::memset(hnd.data(),0,hnd.size()); At<uintptr_t>(hnd.data(),H_PED)=A;
                    At<void*>(hnd.data(),H_HIST_NONTEMP)=histEv(); At<void*>(hnd.data(),H_HIST_TEMP)=histEv();
                    At<uintptr_t>(hnd.data(),H_RESP)=next()%3==0 ? 0 : uintptr_t(Fake(FAKES-1));
                    // dependencias simuladas
                    playerRet=pick({B,C}); killOk=next()%2; goingDoor=next()%2; hitSalt=next();
                    groupRet=next()%2 ? uintptr_t(groupFake.data()) : 0;
                    duckRet=next()%2 ? uintptr_t(Fake(int(next()%(FAKES-1)))) : 0;
                    timeMs=next(); timeStep=next()%8==0 ? 0.f : uni(0.001f,3.f);
                    // evento probado
                    void* const o=object.data(); for(auto& b:object)b=uint8(next());
                    At<void*>(o,0)=eventVtbl.data();
                    auto task=[&](std::initializer_list<int> l){ return int16(*(l.begin()+next()%l.size())); };
                    const auto src=[&]{ return pick({B,C,VA,OA,0,B,A}); };
                    CTask* tsimplest=next()%4==0 ? nullptr : Fake(int(next()%(FAKES-1)));
                    CTask* const tactive=next()%2 ? Fake(int(next()%(FAKES-1))) : nullptr;
                    switch(fn.c){
                    case SEENCOP: At<int16>(o,0xE)=task({200,911,1000,1302,0,5,-1,0x1234}); At<uintptr_t>(o,0x14)=pick({B,C,0,A}); break;
                    case SEENPAN: At<uintptr_t>(o,0)=0x85B778; At<int16>(o,0xE)=task({427,909,911,200,0,428}); At<uintptr_t>(o,0x14)=pick({B,C,A,0}); break;
                    case SEXYPED: At<int16>(o,0xE)=task({200,1212,0,1000}); At<uintptr_t>(o,0x14)=pick({B,C,0}); break;
                    case SEXYVEH: At<uintptr_t>(o,0xC)=pick({VA,VB,0}); break;
                    case SHOT: At<int16>(o,0xE)=task({920,1000,1000,1028,1105,1105,911,726,427,200,415,415,0,1302,999}); At<uintptr_t>(o,0x14)=src(); break;
                    case WHIZZ: At<uintptr_t>(o,0)=0x85B730; At<int16>(o,0xE)=task({200,311,311,311,0,312,415}); At<uintptr_t>(o,0x14)=pick({B,C,VA,OA,0,B}); break;
                    case SIGNAL: At<uintptr_t>(o,0xC)=pick({B,C,0,A}); At<int32>(o,0x10)=next()%2 ? int32(next()%5000) : int32(next()); At<uint8>(o,0x14)=uint8(next()%2); break;
                    case SPECIAL: At<int16>(o,0xE)=task({200,258,261,262,704,706,707,726,910,911,1000,1101,1105,0,5,999}); break;
                    case VDIED: At<uintptr_t>(o,0xC)=pick({VA,VB,0}); At<uint8>(o,0x10)=uint8(next()%2); break;
                    case VFIRE: At<int16>(o,0xE)=task({704,706,707,726,909,911,200,0,1000}); At<uintptr_t>(o,0x14)=pick({VA,VB,0,VA}); break;
                    case PASSIVE: At<uintptr_t>(o,0x14)=pick({VA,VB,0,VA}); At<int32>(o,0x18)=int32(pick({1,2,4,6,7,0,3,5,uint32(next()%9)})); break;
                    case STEAL: At<uintptr_t>(o,0xC)=pick({VA,VB,0,VA}); break;
                    case WATER: At<CVector>(o,0xC)=center+vec(5.f); At<CVector>(o,0x18)=vec(1.5f);
                        if(next()%16==0)At<float>(o,0x18)=std::bit_cast<float>(0x7FC00000u); break;
                    }
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,void*,void*)>(fn.address)(hnd.data(),o,tactive,tsimplest);
                    for(auto* h:hooks)h->State(false);
                    // tareas nuevas del pool de tareas: tipo + 0x70 bytes
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x70,true)}});
                    json routes=json::array(); { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if((preRouteStates[k]&0x80)&&!(states[k]&0x80))routes.push_back(Region(RouteStorage()+k*ROUTE_SZ,ROUTE_SZ)); }
                    json handler=Region(hnd.data(),hnd.size(),false);
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),static_cast<CEntity*>(PedC()),static_cast<CEntity*>(VehA()),static_cast<CEntity*>(VehB()),static_cast<CEntity*>(ObjA())})refs.push_back(refList(e));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Hash(fakes[n].data()+4,fakes[n].size()-4));
                    json r={{"handler",handler},{"tasks",tasks},{"routes",routes},{"refs",refs},
                            {"peds",{EntHash(pedA),EntHash(pedB),EntHash(pedC)}},{"vehs",{EntHash(vehA),EntHash(vehB)}},{"obj",EntHash(objA)},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size()),Hash(intelC.data(),intelC.size())}},
                            {"fakes",fakeWords},{"events",{Hash(evs[0].data()+4,0x3C),Hash(evs[1].data()+4,0x3C),Hash(evs[2].data()+4,0x3C),Hash(evs[3].data()+4,0x3C)}},
                            {"event",Region(o,object.size(),true)},{"mats",{Hash(matB.data(),matB.size()),Hash(matC.data(),matC.size())}},
                            {"time",timeMs},{"time_step",F(timeStep)},{"rng",rngCalls}};
                    r["trace"]=trace; r["switches"]=switches;
                    // la tarea de respuesta (con sus subtareas, si es una secuencia) se borra y el pool de tareas y el de rutas deben quedar como estaban
                    CTask* const resp=At<CTask*>(hnd.data(),H_RESP);
                    if(resp&&freshIds.contains(uintptr_t(resp)))delete resp;
                    bool clean=Occupancy()==occ; { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)clean&=((states[k]&0x80)==(preRouteStates[k]&0x80)); }
                    r["clean"]=clean;
                    freshIds.clear();
                    CleanRefs(); restoreRoutes();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["handler"]},{"candidate",candidate["handler"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    CleanRefs(); restoreRoutes();
    lfSlot=lf0; timeMs=savedTime; timeStep=savedTimeStep;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedTimeStep
        &&std::memcmp(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size())==0&&lfSlot==lf0}});
}
}
