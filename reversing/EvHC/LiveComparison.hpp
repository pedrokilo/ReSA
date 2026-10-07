#pragma once
// Banco dentro del motor: CEventHandler grupo C (17 funciones Compute*Response): OnEscalator, OnFire, PassObject,
// PedEnteredVehicle, PedFriend, PedSoundQuiet, PedThreatBadlyLit, PedThreat, PedToChase, PedToFlee,
// PlayerCollisionWithPed, PlayerWantedLevel, PotentialPedCollide, PotentialWalkIntoFire, ReallyLowHealth, Revive y
// ScriptCommand.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: el manejador (buffer de 0x34 bytes
// con centinelas en las tareas de respuesta, de decir y parcial), tres peatones (A = dueno del manejador, B = el otro
// peatón, C = lider de pandilla; vtable de trampas, matriz propia o posicion en la colocacion, inteligencia, estadisticas
// y arma activa falsas), un vehiculo (datos de manejo, conductor y matriz propios), diez tareas falsas (vtable propia:
// destructor, GetSubTask, IsSimple, GetTaskType y MakeAbortable trazados) y eventos escritos a mano sobre una vtable
// falsa (GetEventType, GetSourceEntity, CloneScriptTask). Las funciones se llaman en la direccion del EXE con `this` =
// manejador. Las tareas nuevas del pool se siguen entre ejecuciones (m_LastFreeSlot restaurado y huecos libres con 0xCD,
// nombradas por su hueco del pool), se compara su tipo y sus 0x80 bytes, y se borran al final de cada ejecucion
// (con la secuencia desenganchada de sus subtareas).
// Se simulan y trazan: CPed::Say, CPlayerPed::AnnoyPlayerPed, CPedGeometryAnalyser::ComputePedHitSide,
// CPed::RestartNonPartialAnims, CTaskComplexWander::GetWanderTaskByPedType (reserva una tarea simple del pool, para
// comparar el orden de reserva), CEventHandler::IsKillTaskAppropriate (convencion cdecl en el EXE y thiscall en la DLL),
// CInterestingEvents::Add, CTheScripts::IsPlayerOnAMission, IKChainManager_c::LookAt, CPedIntelligence::GetPedFOVRange y
// GetMoveStateFromGoToTask, CEventGroup::Add, CPedScriptedTaskRecord::GetRecordAssociatedWithEvent y
// AssociateWithEvent/Task, y el generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber). Se guardan y se
// restauran el reloj, las 64 secuencias de tareas (CTaskSequences::ms_taskSequence) y el estado de las referencias.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventHandler.h"
#include "Events/EventGroup.h"
#include "Events/EventScriptCommand.h"
#include "Entity/Ped/PlayerPed.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskComplexSequence.h"
#include "Tasks/TaskSequences.h"
#include "Tasks/PedScriptedTaskRecord.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskComplexWander.h"
#include "InterestingEvents.h"
#include "Scripts/TheScripts.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "Pools/Pools.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace EvHCLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(const void* base,size_t off) { return *reinterpret_cast<T*>(const_cast<uint8*>(static_cast<const uint8*>(base))+off); }
// desplazamientos crudos del EXE (CEntity / CPed / CVehicle / tareas / eventos)
constexpr size_t P_MATRIX=0x14, P_FLAGS_ENT=0x1C, P_PHYS_FLAGS=0x40, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_PLAYERDATA=0x480, P_STATE=0x530, P_MOVESTATE=0x534, P_HEALTH=0x540, P_VEHICLE=0x58C, P_TYPE=0x598,
                 P_STATS=0x59C, P_WEAPONS=0x5A0, P_WSLOT=0x718;
constexpr size_t V_HANDLING=0x384, V_DRIVER=0x460, H_ANIMGROUP=0xDE;
constexpr size_t I_TASKMGR=4, I_SECONDARY=0x18, I_SEQUENCE=0xD0, I_EVENTGROUP=0x68;
constexpr size_t FK_TYPE=0x38;                   // tipo devuelto por GetTaskType, por objeto
constexpr int FAKES=10;                          // 0-9: tareas falsas; la 9 es la tarea simple (ir a un punto)
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84;
constexpr size_t SEQ_BYTES=sizeof(CTaskComplexSequence)*CTaskSequences::NUM_SEQUENCES;
alignas(16) inline std::array<uint8,0x40> eh{};  // manejador (0x34 bytes) y relleno
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,3> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,4> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,3> intel{};
alignas(16) inline std::array<std::array<uint8,0x80>,3> stats{};
alignas(16) inline std::array<uint8,0x800> veh{};
alignas(16) inline std::array<uint8,0x200> handling{}, playerData{}, rec{}, groupAddEv{};
alignas(16) inline std::array<std::array<uint8,0x40>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> ev{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[size_t(n)].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[size_t(n)].data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,24> eventVtbl{};
inline uint8 *poolStorage{}; inline size_t poolCap{};

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    // direcciones de la pila (locales del EXE o de la DLL): no comparables, se marcan como "stack"
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(v>=uintptr_t(poolStorage)&&v<uintptr_t(poolStorage)+poolCap*0x80)return "task"+std::to_string((v-uintptr_t(poolStorage))/0x80)+"+"+std::to_string((v-uintptr_t(poolStorage))%0x80);
    if(auto s=in(eh.data(),eh.size(),"eh"))return *s;
    for(size_t k=0;k<3;++k){
        if(auto s=in(peds[k].data(),peds[k].size(),k==0?"pedA":k==1?"pedB":"pedC"))return *s;
        if(auto s=in(intel[k].data(),intel[k].size(),k==0?"intelA":k==1?"intelB":"intelC"))return *s;
        if(auto s=in(stats[k].data(),stats[k].size(),k==0?"statsA":k==1?"statsB":"statsC"))return *s; }
    for(size_t k=0;k<mats.size();++k)if(auto s=in(mats[k].data(),mats[k].size(),("mat"+std::to_string(k)).c_str()))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(handling.data(),handling.size(),"handling"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"playerdata"))return *s; if(auto s=in(rec.data(),rec.size(),"record"))return *s;
    if(auto s=in(groupAddEv.data(),groupAddEv.size(),"groupaddev"))return *s;
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[size_t(n)].data(),fakes[size_t(n)].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(ev.data(),ev.size(),"event"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s;
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
inline uint32 rngState{}; inline int32 eventTypeRet{}, hitSideRet{}, goToMoveRet{}; inline uintptr_t cloneRet{}, groupAddRet{}, recordRet{};
inline bool killRet{}, missionRet{}; inline float fovRet{}; inline std::array<bool,4> abortSeq{}; inline int abortIdx{};

// tareas falsas: GetTaskType devuelve el tipo guardado en cada objeto
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline uint8 __fastcall FSimple(CTask*,void*) { return 1; }
inline int __fastcall FType(CTask* t,void*) { return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline uint8 __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const void* e) {
    const bool r=abortSeq[size_t(abortIdx++%4)]; trace.push_back({"fake_make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e)),r}); return r; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// peatones: vtable de trampas
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
// evento falso: GetEventType (1), GetSourceEntity (10, el miembro +0x14) y CloneScriptTask (16)
inline int __fastcall EType(void*,void*) { return eventTypeRet; }
inline void* __fastcall ESource(void* e,void*) { return At<void*>(e,0x14); }
inline void* __fastcall EClone(void*,void*) { return reinterpret_cast<void*>(cloneRet); }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall Say(CPed* p,void*,int32 ctx,uint32 delay,float prob,bool ov,bool force,bool front) { trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),ov,force,front}); return 0; }
inline void __fastcall Annoy(CPlayerPed* p,void*,bool b) { trace.push_back({"annoy_player_ped",Name(uintptr_t(p)),b}); }
inline int32 __cdecl HitSide(const CPed* ped,const CPhysical* phys) { trace.push_back({"hit_side",Name(uintptr_t(ped)),Name(uintptr_t(phys))}); return hitSideRet; }
inline void __fastcall Restart(CPed* p,void*) { trace.push_back({"restart_non_partial_anims",Name(uintptr_t(p))}); }
inline CTaskComplexWander* __cdecl WanderTask(CPed* p) { trace.push_back({"wander_task",Name(uintptr_t(p))}); return reinterpret_cast<CTaskComplexWander*>(new CTaskSimpleStandStill{1,false}); }
inline bool __cdecl KillApprop(CPed* a,CPed* b,const CEvent* e) { trace.push_back({"kill_task_appropriate",Name(uintptr_t(a)),Name(uintptr_t(b)),Name(uintptr_t(e))}); return killRet; }
inline void __fastcall InterestingAdd(void* self,void*,int32 type,CEntity* e) { trace.push_back({"interesting_events_add",uintptr_t(self)==uintptr_t(&g_InterestingEvents),type,Name(uintptr_t(e))}); }
inline bool __cdecl OnMission() { trace.push_back({"is_player_on_a_mission"}); return missionRet; }
inline void __fastcall LookAt(void*,void*,const char* purpose,CPed* ped,CEntity* ent,int32 time,int32 bone,CVector* off,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"look_at",std::string(purpose),Name(uintptr_t(ped)),Name(uintptr_t(ent)),time,bone,Name(uintptr_t(off)),torso,F(speed),blend,prio,force}); }
inline float __fastcall FovRange(CPedIntelligence* i,void*) { trace.push_back({"ped_fov_range",Name(uintptr_t(i))}); return fovRet; }
inline int32 __fastcall GoToMove(CPedIntelligence* i,void*) { trace.push_back({"move_state_from_goto_task",Name(uintptr_t(i))}); return goToMoveRet; }
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(uintptr_t(g)),Name(uintptr_t(e)),valid}); return reinterpret_cast<CEvent*>(groupAddRet); }
inline void* __cdecl RecordForEvent(CEvent* e) { trace.push_back({"record_for_event",Name(uintptr_t(e))}); return reinterpret_cast<void*>(recordRet); }
inline void __fastcall AssocEvent(void* r,void*,void* e) { trace.push_back({"record_associate_event",Name(uintptr_t(r)),Name(uintptr_t(e))}); }
inline void __fastcall AssocTask(void* r,void*,void* t) { trace.push_back({"record_associate_task",Name(uintptr_t(r)),Name(uintptr_t(t))}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=17;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputeOnEscalatorResponse",0x4BC150,0,-1,64},{"CEventHandler","ComputeOnFireResponse",0x4BAD50,0,-1,128},
    {"CEventHandler","ComputePassObjectResponse",0x4BB0C0,0,-1,64},{"CEventHandler","ComputePedEnteredVehicleResponse",0x4C1590,0,-1,1024},
    {"CEventHandler","ComputePedFriendResponse",0x4B9DD0,0,-1,256},{"CEventHandler","ComputePedSoundQuietResponse",0x4B9D40,0,-1,128},
    {"CEventHandler","ComputePedThreatBadlyLitResponse",0x4B9C90,0,-1,128},{"CEventHandler","ComputePedThreatResponse",0x4C19A0,0,-1,2048},
    {"CEventHandler","ComputePedToChaseResponse",0x4C1910,0,-1,64},{"CEventHandler","ComputePedToFleeResponse",0x4B9B50,0,-1,64},
    {"CEventHandler","ComputePlayerCollisionWithPedResponse",0x4B8CE0,0,-1,1024},{"CEventHandler","ComputePlayerWantedLevelResponse",0x4BB280,0,-1,64},
    {"CEventHandler","ComputePotentialPedCollideResponse",0x4C2610,0,-1,512},{"CEventHandler","ComputePotentialWalkIntoFireResponse",0x4BBCD0,0,-1,256},
    {"CEventHandler","ComputeReallyLowHealthResponse",0x4BAA30,0,-1,64},{"CEventHandler","ComputeReviveResponse",0x4B97B0,0,-1,128},
    {"CEventHandler","ComputeScriptCommandResponse",0x4BA7C0,0,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int n=0;n<3;++n)clean(Ped(n)); clean(reinterpret_cast<CEntity*>(veh.data()));
}
// matriz con ejes dados
inline void WriteMatrix(void* dst,float fz,float rz,const CVector& pos) {
    CMatrix m{}; const float c=std::sqrt(1.f-fz*fz), cs=std::cos(rz), sn=std::sin(rz);
    m.GetRight()=CVector{cs,sn,0.f}; m.GetForward()=CVector{-sn*c,cs*c,fz}; m.GetUp()=CVector{sn*fz,-cs*fz,c}; m.GetPosition()=pos;
    std::memcpy(dst,&m,sizeof(CMatrix));
}
inline void SetBit(void* base,size_t off,int bit,bool on) { auto& w=At(base,off); w=on ? (w|(1u<<bit)) : (w&~(1u<<bit)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evhc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhc-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); FillPTraps(std::make_integer_sequence<int,64>{}); FillETraps(std::make_integer_sequence<int,24>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[10]=reinterpret_cast<void*>(&ESource); eventVtbl[16]=reinterpret_cast<void*>(&EClone);
    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); poolStorage=*reinterpret_cast<uint8**>(pb); poolCap=size_t(*reinterpret_cast<int32*>(pb+8)); }
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: el reloj y las 64 secuencias de tareas se guardan y se restauran en cada ejecucion
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto* const seqMem=reinterpret_cast<uint8*>(&CTaskSequences::ms_taskSequence[0]); const std::vector<uint8> savedSeq(seqMem,seqMem+SEQ_BYTES);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::Say),Say), p1e(0x5EFFE0,Say);
        EntryPatch p2(bit_cast<uintptr_t>(&CPlayerPed::AnnoyPlayerPed),Annoy), p2e(0x60A040,Annoy);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CPhysical&)>(&CPedGeometryAnalyser::ComputePedHitSide)),HitSide), p3e(0x5F3BC0,HitSide);
        EntryPatch p4(bit_cast<uintptr_t>(&CPed::RestartNonPartialAnims),Restart), p4e(0x5DED50,Restart);
        EntryPatch p5(bit_cast<uintptr_t>(&CTaskComplexWander::GetWanderTaskByPedType),WanderTask), p5e(0x673D00,WanderTask);
        EntryPatch k1(bit_cast<uintptr_t>(&CEventHandler::IsKillTaskAppropriate),KillApprop), k1e(0x4BC3E0,KillApprop); // estatica cdecl en EXE y DLL
        EntryPatch i1(0x602590,InterestingAdd);                                                                                   // la DLL reenvia al EXE
        EntryPatch m1(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),OnMission), m1e(0x464D50,OnMission);
        EntryPatch l1(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), l1e(0x618970,LookAt);
        EntryPatch f1(bit_cast<uintptr_t>(&CPedIntelligence::GetPedFOVRange),FovRange), f1e(0x4ABEA0,FovRange);
        EntryPatch f2(bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),GoToMove), f2e(0x601D70,GoToMove);
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), g1e(0x4AB420,GroupAdd);
        EntryPatch t1(bit_cast<uintptr_t>(&CPedScriptedTaskRecord::GetRecordAssociatedWithEvent),RecordForEvent), t1e(0x608580,RecordForEvent);
        EntryPatch t2(bit_cast<uintptr_t>(&CPedScriptedTaskRecordData::AssociateWithEvent),AssocEvent), t2e(0x608500,AssocEvent);
        EntryPatch t3(bit_cast<uintptr_t>(&CPedScriptedTaskRecordData::AssociateWithTask),AssocTask), t3e(0x608520,AssocTask);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto& lf=*reinterpret_cast<int32*>(pb+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* sts=*reinterpret_cast<uint8**>(pb+4); for(size_t k=0;k<poolCap;++k)if(sts[k]&0x80)std::memset(poolStorage+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xEC11u; if(!st)st=1; trace=json::array();
                    CleanRefs(); std::memcpy(seqMem,savedSeq.data(),SEQ_BYTES);
                    rngState=next(); timeMs=next(); abortIdx=0;
                    for(auto& b:abortSeq)b=next()%2;
                    // --- fixtures comunes ---
                    std::memset(eh.data(),0xCD,eh.size());
                    for(auto& m:mats)std::memset(m.data(),0,m.size());
                    for(auto& p:peds)std::memset(p.data(),0,p.size());
                    for(auto& v:intel)std::memset(v.data(),0,v.size());
                    for(auto& s:stats)for(auto& b:s)b=uint8(next());
                    std::memset(veh.data(),0,veh.size()); std::memset(handling.data(),0,handling.size());
                    for(auto& b:playerData)b=uint8(next()); std::memset(rec.data(),0xCD,rec.size()); std::memset(groupAddEv.data(),0xCD,groupAddEv.size());
                    for(auto& b:ev)b=uint8(next()); *reinterpret_cast<void**>(ev.data())=eventVtbl.data();
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[size_t(n)])b=uint8(next()); *reinterpret_cast<void**>(fakes[size_t(n)].data())=fakeVtbl.data(); At<int32>(fakes[size_t(n)].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; }
                    static const uint32 pedTypes[]{0,1,2,3,4,5,6,7,8,6,6};
                    static const float weakness[]{0.5f,1.f,1.5f,1.5f,1.75f,2.f,3.f,0.f,std::bit_cast<float>(0x7FC00000u)};
                    CVector posA{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)};
                    auto place=[&](void* ent,size_t mi,const CVector& pos,bool forceMatrix){
                        if(forceMatrix||next()%4){ At<void*>(ent,P_MATRIX)=mats[mi].data(); WriteMatrix(mats[mi].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pos); }
                        else { At<float>(ent,4)=pos.x; At<float>(ent,8)=pos.y; At<float>(ent,0xC)=pos.z; } };
                    for(int n=0;n<3;++n){
                        CPed* p=Ped(n); p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data();
                        At(p,P_FLAGS_ENT)=next(); At(p,P_PHYS_FLAGS)=next(); At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At(p,P_STATE)=next()%60; At<float>(p,P_HEALTH)=uni(0.f,200.f); At<int32>(p,P_MOVESTATE)=int32(next()%9); At(p,P_TYPE)=pedTypes[next()%11];
                        At<void*>(p,P_INTEL)=intel[size_t(n)].data(); At<void*>(p,P_STATS)=stats[size_t(n)].data(); At<void*>(p,P_PLAYERDATA)=next()%2 ? static_cast<void*>(playerData.data()) : nullptr;
                        At<uint8>(p,P_WSLOT)=uint8(next()%13); for(size_t k=0;k<13;++k)At(p,P_WEAPONS+k*0x1C)=next()%46;
                        At<float>(stats[size_t(n)].data(),0x2C)=weakness[next()%9];
                        At<void*>(intel[size_t(n)].data(),I_TASKMGR+0x2C)=p;
                    }
                    place(Ped(0),0,posA,false); place(Ped(1),1,next()%8==0 ? posA : CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)},false);
                    place(Ped(2),2,CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)},false);
                    { CEntity* v=reinterpret_cast<CEntity*>(veh.data()); place(v,3,CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)},false);
                      At<void*>(v,V_HANDLING)=handling.data(); At<void*>(v,V_DRIVER)=next()%3==0 ? nullptr : static_cast<void*>(next()%2 ? Ped(0) : Ped(1)); }
                    At<uint8>(handling.data(),H_ANIMGROUP)=next()%6==0 ? uint8(0xF) : uint8(next()%0x20);
                    At<void*>(Ped(0),P_VEHICLE)=next()%5 ? static_cast<void*>(veh.data()) : nullptr;
                    SetBit(Ped(0),P_FLAGS1,8,next()%5!=0);            // bInVehicle
                    SetBit(Ped(0),P_FLAGS4,11,next()%2);              // bWantedByPolice
                    std::memset(eh.data(),0xCD,eh.size()); At<void*>(eh.data(),0)=Ped(0);
                    for(size_t off:{size_t(0x24),size_t(0x2C),size_t(0x30)})At(eh.data(),off)=0x5A5A5A5Au; // centinelas
                    hitSideRet=int32(next()%4); killRet=next()%2; missionRet=next()%2; fovRet=uni(5.f,40.f); goToMoveRet=int32(next()%9);
                    cloneRet=next()%5 ? uintptr_t(Fake(7)) : 0; groupAddRet=next()%2 ? uintptr_t(groupAddEv.data()) : 0; recordRet=next()%2 ? uintptr_t(rec.data()) : 0;
                    eventTypeRet=next()%2 ? EVENT_ACQUAINTANCE_PED_DISLIKE : EVENT_ACQUAINTANCE_PED_HATE;
                    CEntity* ent[]{Ped(1),Ped(2),reinterpret_cast<CEntity*>(veh.data())};
                    auto pickEnt=[&]()->CEntity*{ return next()%12==0 ? nullptr : ent[next()%3]; };
                    auto pickPed=[&]()->CPed*{ return next()%12==0 ? nullptr : (next()%4 ? Ped(1) : Ped(2)); };
                    // tareas falsas colocadas en las ranuras primarias/secundarias del peatón A
                    auto putPrimary=[&](int slot,int fake,int type){ At<int32>(fakes[size_t(fake)].data(),FK_TYPE)=type; At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(slot))=Fake(fake); };
                    CTask* tsimplest=nullptr; CTask* tactive=next()%2 ? Fake(8) : nullptr;
                    { const bool goTo=next()%4!=0; At<int32>(fakes[9].data(),FK_TYPE)=goTo ? (next()%2 ? 0x384 : 0x39E) : (next()%2 ? 0x385 : TASK_SIMPLE_PAUSE);
                      for(size_t k=0;k<3;++k)At<float>(fakes[9].data(),0xC+4*k)=uni(-30.f,30.f); tsimplest=Fake(9); }
                    uint8* const e=ev.data();
                    At<int16>(e,0xE)=0;
                    // --- fixtures de cada funcion ---
                    if(nm=="ComputePlayerCollisionWithPedResponse") {
                        At<CPed*>(e,0x14)=next()%16==0 ? nullptr : Ped(1);
                        static const int16 mss[]{0,1,2,3,4,5,6,7,4,4,6,7,1,0,-1,8};
                        At<int16>(e,0x30)=next()%12==0 ? int16(next()) : mss[next()%16]; At<int16>(e,0x32)=next()%12==0 ? int16(next()) : mss[next()%16];
                        // posiciones/orientaciones: coincidentes, perpendiculares o NaN para los bordes de los productos escalares
                        for(int n=0;n<2;++n){ At<void*>(Ped(n),P_MATRIX)=mats[size_t(n)].data(); }
                        CVector pa=posA, pb=At<CPed*>(e,0x14) ? CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)}+posA : posA;
                        if(next()%10==0)pb=posA; if(next()%24==0)pb.x=std::bit_cast<float>(0x7FC00000u);
                        WriteMatrix(mats[0].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pa); WriteMatrix(mats[1].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pb);
                        if(next()%10==0){ std::memcpy(mats[1].data(),mats[0].data(),0x30); } // mismos ejes: productos escalares exactos
                        // la victima sigue a un lider en formacion (tipo 0x391) a veces; el resto de tareas no
                        { const uint32 m=next()%8;
                          if(m<=1){ At<int32>(fakes[0].data(),FK_TYPE)=0x391; At<CTask*>(intel[1].data(),I_TASKMGR+4*(next()%5))=Fake(0); }
                          else if(m<=3){ At<int32>(fakes[1].data(),FK_TYPE)=0x392; At<CTask*>(intel[1].data(),I_TASKMGR+4*(next()%5))=Fake(1); } }
                        SetBit(Ped(0),P_FLAGS1,8,false); // el original no mira bInVehicle; la inteligencia de la victima se usa tal cual
                    } else if(nm=="ComputePedEnteredVehicleResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_KILL_CRIMINAL,TASK_COMPLEX_DESTROY_CAR,TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_SCREAM_IN_CAR_THEN_LEAVE,TASK_COMPLEX_LEAVE_CAR,
                                                TASK_COMPLEX_LEAVE_CAR_AND_FLEE,TASK_COMPLEX_LEAVE_CAR_AND_WANDER,0x123,TASK_SIMPLE_CAR_DRIVE,0};
                        At<int16>(e,0xE)=next()%12==0 ? ts[next()%11] : ts[next()%8];
                        At<CPed*>(e,0x14)=pickPed(); At<void*>(e,0x18)=next()%12==0 ? nullptr : static_cast<void*>(veh.data()); At<int32>(e,0x1C)=int32(next()%6);
                        SetBit(Ped(0),P_FLAGS1,8,next()%6!=0);
                        At<void*>(Ped(0),P_VEHICLE)=veh.data(); // el EXE llama a IsDriver sobre m_pVehicle sin comprobar nulo
                    } else if(nm=="ComputePedFriendResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT,
                                                TASK_COMPLEX_PROSTITUTE_SOLICIT,TASK_COMPLEX_PROSTITUTE_SOLICIT,0x77,0};
                        At<int16>(e,0xE)=ts[next()%8]; At<CPed*>(e,0x14)=Ped(1+int(next()%2)); // el ctor de ProstituteSolicit registra la referencia sin comprobar nulo
                        { const uint32 n=next()%4; if(n==0)putPrimary(int(next()%5),0,TASK_COMPLEX_PROSTITUTE_SOLICIT); }
                        SetBit(Ped(0),P_FLAGS1,8,next()%3==0);
                    } else if(nm=="ComputePedSoundQuietResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_INVESTIGATE_DISTURBANCE,TASK_COMPLEX_INVESTIGATE_DISTURBANCE,0x55,0};
                        At<int16>(e,0xE)=ts[next()%5]; At<CEntity*>(e,0x14)=pickEnt(); for(size_t k=0;k<3;++k)At<float>(e,0x20+4*k)=uni(-40.f,40.f);
                    } else if(nm=="ComputePedThreatBadlyLitResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_INVESTIGATE_DISTURBANCE,TASK_COMPLEX_INVESTIGATE_DISTURBANCE,0x55,0};
                        At<int16>(e,0xE)=ts[next()%5]; At<CPed*>(e,0x14)=pickPed(); for(size_t k=0;k<3;++k)At<float>(e,0x1C+4*k)=uni(-40.f,40.f);
                    } else if(nm=="ComputePedThreatResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_SEQUENCE,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR,TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_RUN,
                            TASK_COMPLEX_USE_CLOSEST_FREE_SCRIPTED_ATTRACTOR_SPRINT,TASK_SIMPLE_COWER,TASK_SIMPLE_HANDS_UP,TASK_SIMPLE_DUCK,TASK_SIMPLE_DUCK_FOREVER,TASK_COMPLEX_LEAVE_CAR,
                            TASK_COMPLEX_FLEE_ENTITY,TASK_COMPLEX_SMART_FLEE_ENTITY,TASK_COMPLEX_FLEE_ANY_MEANS,TASK_COMPLEX_SEEK_ENTITY_AIMING,TASK_SMART_FLEE_ENTITY_WALKING,
                            TASK_COMPLEX_KILL_PED_ON_FOOT,TASK_COMPLEX_KILL_PED_ON_FOOT_TIMED,TASK_COMPLEX_KILL_PED_ON_FOOT_STAND_STILL,TASK_COMPLEX_KILL_PED_ON_FOOT_KINDA_STAND_STILL,
                            TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_GANG_DRIVEBY,TASK_SIMPLE_GANG_DRIVEBY,TASK_COMPLEX_KILL_PED_ON_FOOT_STEALTH,TASK_COMPLEX_KILL_PED_AND_REENTER_CAR,
                            TASK_COMPLEX_KILL_CRIMINAL,0x55,0x3A0,0x3E9,0};
                        constexpr uint32 NT=sizeof(ts)/sizeof(ts[0]);
                        At<int16>(e,0xE)=ts[next()%NT]; At<CPed*>(e,0x14)=pickPed();
                        // peatón con policia/ladron, conductor del vehiculo y tarea de agacharse (ranura secundaria 1, tipo 0x19F)
                        SetBit(Ped(0),P_FLAGS1,8,next()%3!=0);
                        At<uint32>(Ped(1),P_TYPE)=next()%2 ? 6u : pedTypes[next()%11];
                        { const uint32 n=next()%4; At<CTask*>(intel[0].data(),I_SECONDARY+4*1)=n==0 ? nullptr : Fake(2); At<int32>(fakes[2].data(),FK_TYPE)=n==1 ? 0x19F : n==2 ? 0x19F : 0x55; }
                        // CarDrive activa: tarea simple de conducir (0x2C5) en una ranura primaria
                        if(next()%2)putPrimary(int(next()%5),3,TASK_SIMPLE_CAR_DRIVE);
                        // tareas de secuencia: indice con signo, primera tarea vacia o no
                        { const int idx=int(next()%66)-2; At<int8>(intel[0].data(),I_SEQUENCE)=int8(idx);
                          if(idx>=0&&idx<64){ auto& sq=CTaskSequences::ms_taskSequence[idx]; sq.m_Tasks.data()[0]=next()%2 ? Fake(4) : nullptr; sq.m_RefCnt=next()%3; } }
                        fovRet=next()%6==0 ? 0.f : uni(0.f,60.f); // rango de arma de la tabla del EXE frente al rango de vision + 5
                    } else if(nm=="ComputePedToChaseResponse"||nm=="ComputePedToFleeResponse") {
                        At<CPed*>(e,0xC)=pickPed();
                    } else if(nm=="ComputePassObjectResponse") {
                        At<CPed*>(e,0xC)=Ped(1+int(next()%2)); At<uint8>(e,0x10)=uint8(next());
                    } else if(nm=="ComputeOnFireResponse") {
                        At(Ped(0),P_TYPE)=next()%3==0 ? 0u : pedTypes[next()%11];
                        At<CTask*>(intel[0].data(),I_SECONDARY+4*4)=next()%2 ? Fake(5) : nullptr;
                    } else if(nm=="ComputePotentialPedCollideResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING,TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING,TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING,
                                                TASK_COMPLEX_PARTNER_SHOVE,0x77,0};
                        At<int16>(e,0xE)=ts[next()%7]; for(size_t k=0;k<3;++k)At<float>(e,0x14+4*k)=uni(-40.f,40.f);
                        At<CPed*>(e,0x20)=next()%12==0 ? nullptr : Ped(1); At<uint32>(e,0x24)=next()%10==0 ? next() : next()%9;
                        // seguidor de pandilla (lider en +0x10) y seguir entidad (radio del estado de movimiento en +0x1C)
                        if(next()%4){ putPrimary(int(next()%5),4,TASK_COMPLEX_GANG_FOLLOWER); At<CPed*>(fakes[4].data(),0x10)=next()%8==0 ? nullptr : Ped(2); }
                        if(next()%4){ static const float rs[]{0.f,0.5f,1.f,2.f,5.f,12.f,30.f}; int slot=int(next()%5); if(At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(slot))==nullptr){ putPrimary(slot,6,TASK_COMPLEX_SEEK_ENTITY); At<float>(fakes[6].data(),0x1C)=rs[next()%7]; } }
                        At<uint32>(Ped(2),P_MOVESTATE)=next()%9;
                        place(Ped(2),2,next()%6==0 ? posA : CVector{uni(-20.f,20.f),uni(-20.f,20.f),uni(0.f,30.f)}+posA,false);
                    } else if(nm=="ComputePotentialWalkIntoFireResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_SMART_FLEE_POINT,TASK_COMPLEX_WALK_ROUND_FIRE,TASK_COMPLEX_FLEE_POINT,0x77,0};
                        At<int16>(e,0xE)=ts[next()%6]; for(size_t k=0;k<3;++k)At<float>(e,0x14+4*k)=uni(-40.f,40.f);
                        At<float>(e,0x20)=uni(0.f,8.f); At<float>(e,0x24)=uni(0.f,8.f); At<uint32>(e,0x28)=next()%9;
                        if(next()%10==0)tsimplest=nullptr;
                    } else if(nm=="ComputeReallyLowHealthResponse") {
                        static const int16 ts[]{200,TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP,0x77,0};
                        At<int16>(e,0xE)=ts[next()%4];
                    } else if(nm=="ComputeReviveResponse") {
                        if(next()%2)At<CTask*>(intel[0].data(),I_TASKMGR+4*3)=Fake(5);
                        if(next()%2)At<CTask*>(intel[0].data(),I_TASKMGR+4*4)=Fake(6);
                    } else if(nm=="ComputeScriptCommandResponse") {
                        static const int32 idxs[]{3,4,3,4,0,1,2,5,-1};
                        At<int32>(e,0xC)=idxs[next()%9]; At<CTask*>(e,0x10)=nullptr;
                        for(int slot=0;slot<5;++slot)if(next()%2){ At<int32>(fakes[size_t(slot)].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(slot))=Fake(slot); }
                        eventTypeRet=EVENT_SCRIPT_COMMAND;
                    }
                    // --- llamada ---
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t call=fn.address;
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(call)(eh.data(),e,tactive,tsimplest);
                    setState(false);
                    // --- resultados ---
                    auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k))fresh.push_back(pool->GetAt(k));
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json refs=json::array(); auto refList=[&](CEntity* en){json l=json::array();for(auto* r=en->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int n=0;n<3;++n)refs.push_back(refList(Ped(n))); refs.push_back(refList(reinterpret_cast<CEntity*>(veh.data())));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[size_t(n)].data(),fakes[size_t(n)].size(),true));
                    json intelWords=json::array(); for(int n=0;n<3;++n)intelWords.push_back(Region(intel[size_t(n)].data(),intel[size_t(n)].size()));
                    json r={{"handler",Region(eh.data(),0x34)},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(peds[0])},{"pedB",EntHash(peds[1])},{"pedC",EntHash(peds[2])},
                            {"veh",EntHash(veh)},{"intel",intelWords},{"stats",{Hash(stats[0].data(),0x80),Hash(stats[1].data(),0x80),Hash(stats[2].data(),0x80)}},
                            {"fakes",fakeWords},{"event",Region(ev.data(),ev.size(),true)},{"seq",Hash(seqMem,SEQ_BYTES)},{"time",timeMs},{"rng",rngState}};
                    // borrado de las tareas creadas (las secuencias sueltan antes sus subtareas, que se borran por separado)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)if(t->GetTaskType()==TASK_COMPLEX_SEQUENCE)for(auto& s:static_cast<CTaskComplexSequence*>(t)->m_Tasks)s=nullptr;
                    for(int n=0;n<5;++n){ auto& slot=At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(n)); for(auto* t:fresh)if(slot==t)slot=nullptr; }
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    CleanRefs(); std::memcpy(seqMem,savedSeq.data(),SEQ_BYTES);
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
    CleanRefs(); std::memcpy(seqMem,savedSeq.data(),SEQ_BYTES);
    const bool globalsTouched=timeMs!=savedTime;
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&std::memcmp(seqMem,savedSeq.data(),SEQ_BYTES)==0}});
}
}
