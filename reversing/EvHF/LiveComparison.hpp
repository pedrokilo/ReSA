#pragma once
// Banco dentro del motor: CEventHandler::ComputePedCollisionWithPedResponse (0x4BDB80).
// Original (hook desactivado) frente a candidato con fixtures identicos: el manejador (buffer de 0x34 bytes con
// centinelas en las tareas de respuesta, de decir y parcial), tres peatones (A = dueno del manejador, B = el peaton
// chocado, C = otro peaton / lider; vtable de trampas, matriz propia, inteligencia y estadisticas falsas), dos grupos
// falsos, diez tareas falsas de 0x80 bytes (vtable propia: destructor, GetSubTask, IsSimple, GetTaskType y MakeAbortable;
// las de seguir a la pandilla, buscar entidad, evitar peaton e ir a un punto en las ranuras primarias del peaton A) y un
// evento escrito a mano. La funcion se llama en la direccion del EXE con `this` = manejador. Las tareas nuevas del pool
// de tareas se siguen por hueco (m_LastFreeSlot restaurado y huecos libres con 0xCD): se compara su tipo y sus 0x80 bytes.
// Las rutas nuevas del pool de CPointRoute tambien se comparan (cabecera, estados y almacen restaurados en cada ejecucion,
// huecos libres con 0xCD). Se simulan y trazan: CPedGroups::GetPedsGroup (0x5F7E80), CPedGroupMembership::GetLeader
// (0x5F69A0), CPedGeometryAnalyser::ComputeEntityHitSide (0x5F3BC0), ComputeRouteRoundEntityBoundingBox (0x5F6110, que
// rellena toda la ruta), IKChainManager_c::LookAt, CPedIntelligence::GetMoveStateFromGoToTask,
// CPedIntelligence::IncrementAngerAtPlayer (0x421050) y el generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber).
// La funcion 0x4BC470 (ajuste del radio de la tarea de buscar entidad) se ejecuta real en ambos lados. Se guardan y se
// restauran el reloj y los pools de rutas y de tareas.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventHandler.h"
#include "Events/EventPedCollisionWithPed.h"
#include "Entity/Ped/PlayerPed.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "Pools/Pools.h"
#include "Pools/PointRoutePool.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace EvHFLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(const void* base,size_t off) { return *reinterpret_cast<T*>(const_cast<uint8*>(static_cast<const uint8*>(base))+off); }
// desplazamientos crudos del EXE (CEntity / CPed / tareas / eventos)
constexpr size_t P_MATRIX=0x14, P_FLAGS_ENT=0x1C, P_PHYS_FLAGS=0x40, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_PLAYERDATA=0x480, P_STATE=0x530, P_MOVESTATE=0x534, P_HEALTH=0x540, P_TYPE=0x598, P_STATS=0x59C;
constexpr size_t I_TASKMGR=4;
constexpr size_t FK_TYPE=0x38, FK_BYTES=0x80;    // tipo devuelto por GetTaskType, por objeto; tamano de cada tarea falsa
constexpr int FAKES=10;                          // 4 seguir pandilla, 5 evitar peaton, 6 buscar entidad, 7 ir a un punto (ranura primaria), 8 activa, 9 simple
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84;
alignas(16) inline std::array<uint8,0x40> eh{};  // manejador (0x34 bytes) y relleno
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,3> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,3> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,3> intel{};
alignas(16) inline std::array<std::array<uint8,0x80>,3> stats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedGroup)>,2> groups{};
alignas(16) inline std::array<std::array<uint8,FK_BYTES>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> ev{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[size_t(n)].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[size_t(n)].data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,12> fakeVtbl{};
inline uint8 *poolStorage{}; inline size_t poolCap{};

// pool de CPointRoute (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot, banderas)
inline uint8* RouteHdr() { return reinterpret_cast<uint8*>(GetPointRoutePool()); }
inline uint8* RouteStorage() { return *reinterpret_cast<uint8**>(RouteHdr()); }
inline uint8* RouteStates() { return *reinterpret_cast<uint8**>(RouteHdr()+4); }
inline size_t RouteCap() { return *reinterpret_cast<uint32*>(RouteHdr()+8); }
constexpr size_t ROUTE_SZ=sizeof(CPointRoute);

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    // direcciones de la pila (locales del EXE o de la DLL): no comparables, se marcan como "stack"
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(v>=uintptr_t(poolStorage)&&v<uintptr_t(poolStorage)+poolCap*0x80)return "task"+std::to_string((v-uintptr_t(poolStorage))/0x80)+"+"+std::to_string((v-uintptr_t(poolStorage))%0x80);
    { const auto rs=uintptr_t(RouteStorage()); if(v>=rs&&v<rs+RouteCap()*ROUTE_SZ)return "route#"+std::to_string((v-rs)/ROUTE_SZ)+"+"+std::to_string((v-rs)%ROUTE_SZ); }
    if(auto s=in(eh.data(),eh.size(),"eh"))return *s;
    for(size_t k=0;k<3;++k){
        if(auto s=in(peds[k].data(),peds[k].size(),k==0?"pedA":k==1?"pedB":"pedC"))return *s;
        if(auto s=in(intel[k].data(),intel[k].size(),k==0?"intelA":k==1?"intelB":"intelC"))return *s;
        if(auto s=in(stats[k].data(),stats[k].size(),k==0?"statsA":k==1?"statsB":"statsC"))return *s; }
    for(size_t k=0;k<mats.size();++k)if(auto s=in(mats[k].data(),mats[k].size(),("mat"+std::to_string(k)).c_str()))return *s;
    for(size_t k=0;k<groups.size();++k)if(auto s=in(groups[k].data(),groups[k].size(),k==0?"group0":"group1"))return *s;
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
inline uint32 rngState{}; inline int32 hitSideRet{}, goToMoveRet{}; inline uintptr_t groupRet[2]{}, leaderRet[2]{}; inline uint32 routeCount{}; inline std::array<uint32,24> routeWords{};

// tareas falsas: GetTaskType devuelve el tipo guardado en cada objeto
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline uint8 __fastcall FSimple(CTask*,void*) { return 1; }
inline int __fastcall FType(CTask* t,void*) { return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline uint8 __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const void* e) {
    trace.push_back({"fake_make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return 1; }
template<int N> void* __fastcall FTrap(CTask*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// peatones: vtable de trampas
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
// EXE 0x5F7E80 (la DLL reenvia al EXE): cdecl(ped)
inline CPedGroup* __cdecl GetGroupStub(const CPed* p) {
    trace.push_back({"get_peds_group",Name(uintptr_t(p))});
    return reinterpret_cast<CPedGroup*>(p==Ped(0) ? groupRet[0] : p==Ped(1) ? groupRet[1] : 0); }
// CPedGroupMembership::GetLeader (0x5F69A0, la DLL la tiene enganchada): thiscall; el grupo se reconoce por el rango de su buffer
inline CPed* __fastcall LeaderStub(void* self,void*) {
    int g=-1; for(int k=0;k<2;++k)if(uintptr_t(self)>=uintptr_t(groups[size_t(k)].data())&&uintptr_t(self)<uintptr_t(groups[size_t(k)].data())+groups[size_t(k)].size())g=k;
    trace.push_back({"get_leader",g}); return reinterpret_cast<CPed*>(g<0 ? 0 : g==0 ? leaderRet[0] : leaderRet[1]); }
// EXE 0x5F3BC0 y DLL ComputeEntityHitSide(const CPed&, CEntity&): cdecl(victima, jugador)
inline int32 __cdecl HitSideStub(const CPed* ped,CEntity* ent) { trace.push_back({"hit_side",Name(uintptr_t(ped)),Name(uintptr_t(ent))}); return hitSideRet; }
// EXE 0x5F6110 y DLL ComputeRouteRoundEntityBoundingBox: cdecl(ped, entidad, destino, ruta, 0); escribe toda la ruta (0x64 bytes)
inline int32 __cdecl RouteStub(const CPed* ped,CEntity* ent,const CVector* pos,CPointRoute* route,int32 a5) {
    trace.push_back({"route_round_entity",Name(uintptr_t(ped)),Name(uintptr_t(ent)),F(pos->x),F(pos->y),F(pos->z),Name(uintptr_t(route)),a5});
    auto* w=reinterpret_cast<uint32*>(route); w[0]=routeCount; for(size_t k=0;k<24;++k)w[1+k]=routeWords[k]; return 0; }
inline void __fastcall LookAt(void*,void*,const char* purpose,CPed* ped,CEntity* ent,int32 time,int32 bone,CVector* off,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"look_at",std::string(purpose),Name(uintptr_t(ped)),Name(uintptr_t(ent)),time,bone,Name(uintptr_t(off)),torso,F(speed),blend,prio,force}); }
inline int32 __fastcall GoToMove(CPedIntelligence* i,void*) { trace.push_back({"move_state_from_goto_task",Name(uintptr_t(i))}); return goToMoveRet; }
inline void __fastcall Anger(CPedIntelligence* i,void*,uint8 a) { trace.push_back({"increment_anger_at_player",Name(uintptr_t(i)),int(a)}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=1;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputePedCollisionWithPedResponse",0x4BDB80,0,-1,2048},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    for(int n=0;n<3;++n)clean(Ped(n));
}
// matriz con ejes dados
inline void WriteMatrix(void* dst,float fz,float rz,const CVector& pos) {
    CMatrix m{}; const float c=std::sqrt(1.f-fz*fz), cs=std::cos(rz), sn=std::sin(rz);
    m.GetRight()=CVector{cs,sn,0.f}; m.GetForward()=CVector{-sn*c,cs*c,fz}; m.GetUp()=CVector{sn*fz,-cs*fz,c}; m.GetPosition()=pos;
    std::memcpy(dst,&m,sizeof(CMatrix));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"evhf-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhf-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhf");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhf-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); FillPTraps(std::make_integer_sequence<int,64>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); poolStorage=*reinterpret_cast<uint8**>(pb); poolCap=size_t(*reinterpret_cast<int32*>(pb+8)); }
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: el reloj, m_LastFreeSlot del pool de tareas y el pool de rutas se guardan y se restauran en cada ejecucion
    auto& lfSlot=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfSlot;
    const std::vector<uint8> savedRouteHdr(RouteHdr(),RouteHdr()+0x14), savedRouteStates(RouteStates(),RouteStates()+RouteCap()),
                             savedRouteStorage(RouteStorage(),RouteStorage()+RouteCap()*ROUTE_SZ);
    auto restoreRoutes=[&]{ std::memcpy(RouteStates(),savedRouteStates.data(),savedRouteStates.size()); std::memcpy(RouteStorage(),savedRouteStorage.data(),savedRouteStorage.size());
                            std::memcpy(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size()); };
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch g1e(0x5F7E80,GetGroupStub);                                                                                         // la DLL reenvia al EXE
        EntryPatch g2(bit_cast<uintptr_t>(&CPedGroupMembership::GetLeader),LeaderStub), g2e(0x5F69A0,LeaderStub);
        EntryPatch h1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide)),HitSideStub), h1e(0x5F3BC0,HitSideStub);
        EntryPatch o1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32)>(&CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox)),RouteStub), o1e(0x5F6110,RouteStub);
        EntryPatch l1(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), l1e(0x618970,LookAt);
        EntryPatch f2(bit_cast<uintptr_t>(&CPedIntelligence::GetMoveStateFromGoToTask),GoToMove), f2e(0x601D70,GoToMove);
        EntryPatch a1(bit_cast<uintptr_t>(&CPedIntelligence::IncrementAngerAtPlayer),Anger), a1e(0x421050,Anger);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto)
                    lfSlot=lf0;
                    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* sts=*reinterpret_cast<uint8**>(pb+4); for(size_t k=0;k<poolCap;++k)if(sts[k]&0x80)std::memset(poolStorage+k*0x80,0xCD,0x80); }
                    CleanRefs(); restoreRoutes();
                    { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if(states[k]&0x80)std::memset(RouteStorage()+k*ROUTE_SZ,0xCD,ROUTE_SZ); }
                    const std::vector<uint8> preRouteStates(RouteStates(),RouteStates()+RouteCap());
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xEF01u; if(!st)st=1; trace=json::array();
                    rngState=next(); timeMs=next();
                    // --- fixtures comunes ---
                    for(auto& m:mats)std::memset(m.data(),0,m.size());
                    for(auto& p:peds)std::memset(p.data(),0,p.size());
                    for(auto& v:intel)std::memset(v.data(),0,v.size());
                    for(auto& s:stats)for(auto& b:s)b=uint8(next());
                    for(auto& g:groups)for(auto& b:g)b=uint8(next());
                    for(auto& b:ev)b=uint8(next());
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[size_t(n)])b=uint8(next()); *reinterpret_cast<void**>(fakes[size_t(n)].data())=fakeVtbl.data(); At<int32>(fakes[size_t(n)].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; }
                    static const uint32 pedTypes[]{0,1,2,3,4,5,6,7,8,6,6};
                    static const float weakness[]{0.5f,1.f,1.5f,1.5f,1.75f,2.f,3.f,0.f,std::bit_cast<float>(0x7FC00000u)};
                    CVector posA{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)};
                    for(int n=0;n<3;++n){
                        CPed* p=Ped(n); p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data();
                        At(p,P_FLAGS_ENT)=next(); At(p,P_PHYS_FLAGS)=next(); At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At(p,P_STATE)=next()%60; At<float>(p,P_HEALTH)=uni(0.f,200.f); At<int32>(p,P_MOVESTATE)=int32(next()%9); At(p,P_TYPE)=pedTypes[next()%11];
                        At<void*>(p,P_INTEL)=intel[size_t(n)].data(); At<void*>(p,P_STATS)=stats[size_t(n)].data(); At<void*>(p,P_PLAYERDATA)=nullptr;
                        At<float>(stats[size_t(n)].data(),0x2C)=weakness[next()%9];
                        At<void*>(intel[size_t(n)].data(),I_TASKMGR+0x2C)=p;
                        At<void*>(p,P_MATRIX)=mats[size_t(n)].data();
                    }
                    // evento: peaton chocado (+0x14) y estados de movimiento (+0x30 y +0x32, con signo)
                    uint8* const e=ev.data();
                    *reinterpret_cast<void**>(e)=nullptr; At<int16>(e,0xE)=0;
                    At<CPed*>(e,0x14)=next()%16==0 ? nullptr : Ped(1);
                    static const int16 mss[]{0,1,1,4,4,6,6,7,7,0,5,2,3};
                    int16 m1=next()%12==0 ? int16(next()) : mss[next()%13], m2=next()%12==0 ? int16(next()) : mss[next()%13];
                    // tarea simple: de ir a un punto (0x384/0x39E) o no; nula a veces (el EXE solo la comprueba en parte de las rutas)
                    CTask* tsimplest=nullptr; CTask* tactive=next()%2 ? Fake(8) : nullptr;
                    { const bool goTo=next()%4!=0; At<int32>(fakes[9].data(),FK_TYPE)=goTo ? (next()%2 ? 0x384 : 0x39E) : (next()%2 ? 0x385 : TASK_SIMPLE_PAUSE);
                      for(size_t k=0;k<3;++k)At<float>(fakes[9].data(),0xC+4*k)=uni(-30.f,30.f); tsimplest=Fake(9);
                      if(next()%10==0){ tsimplest=nullptr; if(m1==PEDMOVE_WALK&&m2==PEDMOVE_WALK)m2=PEDMOVE_STILL; } } // el EXE llama a IsGoToTask sin comprobar nulo con ambos andando
                    At<int16>(e,0x30)=m1; At<int16>(e,0x32)=m2;
                    {
                        // posiciones/orientaciones: coincidentes, perpendiculares o NaN para los bordes de los productos escalares
                        CVector pa=posA, pb=At<CPed*>(e,0x14) ? CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)}+posA : posA;
                        if(next()%10==0)pb=posA; if(next()%24==0)pb.x=std::bit_cast<float>(0x7FC00000u);
                        WriteMatrix(mats[0].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pa); WriteMatrix(mats[1].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pb);
                        if(next()%10==0){ std::memcpy(mats[1].data(),mats[0].data(),0x30); } // mismos ejes: productos escalares exactos
                        WriteMatrix(mats[2].data(),0.f,uni(-3.f,3.f),posA+CVector{uni(-12.f,12.f),uni(-12.f,12.f),uni(-2.f,2.f)}); // C: a distancia variable (umbral 8.0)
                    }
                    // grupos: sin grupo (50%), mismo grupo, grupos distintos o victima sin grupo
                    const uint32 gm=next()%8; auto G=[&](int k){ return uintptr_t(groups[size_t(k)].data()); };
                    groupRet[0]=gm<4 ? 0 : G(0);
                    groupRet[1]=gm<4 ? (next()%2 ? G(0) : 0) : (gm==4||gm==7) ? G(0) : gm==5 ? G(1) : 0;
                    { const uintptr_t lds[]{0,uintptr_t(Ped(0)),uintptr_t(Ped(1)),uintptr_t(Ped(2)),uintptr_t(Ped(1)),uintptr_t(Ped(2))};
                      leaderRet[0]=lds[next()%6]; leaderRet[1]=next()%3 ? leaderRet[0] : lds[next()%6]; }
                    hitSideRet=int32(next()%4); goToMoveRet=next()%3==0 ? 1 : int32(next()%9);
                    routeCount=next()%9; for(auto& w:routeWords)w=F(uni(-30.f,30.f));
                    // tareas de la inteligencia del jugador en ranuras primarias distintas (la 0 solo la ve FindActiveTaskByType)
                    {
                        int slots[5]{0,1,2,3,4}; for(int k=4;k>0;--k)std::swap(slots[k],slots[next()%uint32(k+1)]);
                        int used=0;
                        auto put=[&](int fake,int type){ if(used>=5)return; At<int32>(fakes[size_t(fake)].data(),FK_TYPE)=type; At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(slots[used++]))=Fake(fake); };
                        if(next()%3){ put(4,TASK_COMPLEX_GANG_FOLLOWER); At<CPed*>(fakes[4].data(),0x10)=next()%8==0 ? nullptr : (next()%3 ? Ped(2) : Ped(1)); }
                        if(next()%3)put(6,TASK_COMPLEX_SEEK_ENTITY);
                        if(next()%3)put(5,TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING);
                        if(next()%3)put(7,TASK_SIMPLE_GO_TO_POINT);
                        if(next()%6==0)put(8,0x39E);                                          // tarea que no se busca
                        if(next()%4==0)At<uint32>(fakes[6].data(),0x18)=0xFFFFFFFFu;          // NaN en el radio actual de la tarea de buscar entidad
                    }
                    // la victima lleva a veces una tarea activa
                    if(next()%3==0)At<CTask*>(intel[1].data(),I_TASKMGR+4*(next()%5))=Fake(3);
                    std::memset(eh.data(),0xCD,eh.size()); At<void*>(eh.data(),0)=Ped(0);
                    for(size_t off:{size_t(0x24),size_t(0x2C),size_t(0x30)})At(eh.data(),off)=0x5A5A5A5Au; // centinelas
                    // --- llamada ---
                    const auto occ=Occupancy();
                    setState(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(fn.address)(eh.data(),e,tactive,tsimplest);
                    setState(false);
                    // --- resultados ---
                    auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k))fresh.push_back(pool->GetAt(k));
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json routes=json::array(); { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)if((preRouteStates[k]&0x80)&&!(states[k]&0x80))routes.push_back(Region(RouteStorage()+k*ROUTE_SZ,ROUTE_SZ)); }
                    json refs=json::array(); auto refList=[&](CEntity* en){json l=json::array();for(auto* r=en->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int n=0;n<3;++n)refs.push_back(refList(Ped(n)));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[size_t(n)].data(),fakes[size_t(n)].size(),true));
                    json intelWords=json::array(); for(int n=0;n<3;++n)intelWords.push_back(Region(intel[size_t(n)].data(),intel[size_t(n)].size()));
                    json r={{"handler",Region(eh.data(),0x34)},{"tasks",tasks},{"routes",routes},{"refs",refs},{"pedA",EntHash(peds[0])},{"pedB",EntHash(peds[1])},{"pedC",EntHash(peds[2])},
                            {"intel",intelWords},{"stats",{Hash(stats[0].data(),0x80),Hash(stats[1].data(),0x80),Hash(stats[2].data(),0x80)}},
                            {"fakes",fakeWords},{"event",Region(ev.data(),ev.size(),true)},{"time",timeMs},{"rng",rngState}};
                    // borrado de las tareas creadas
                    const size_t traceBefore=trace.size();
                    for(int n=0;n<5;++n){ auto& slot=At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(n)); for(auto* t:fresh)if(slot==t)slot=nullptr; }
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    bool clean=Occupancy()==occ; { auto* states=RouteStates(); for(size_t k=0;k<RouteCap();++k)clean&=((states[k]&0x80)==(preRouteStates[k]&0x80)); }
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
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
    lfSlot=lf0;
    const bool globalsTouched=timeMs!=savedTime;
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&lfSlot==lf0
               &&std::memcmp(RouteHdr(),savedRouteHdr.data(),savedRouteHdr.size())==0}});
}
}
