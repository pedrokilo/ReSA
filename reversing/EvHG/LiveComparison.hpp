#pragma once
// Banco dentro del motor: CEventHandler::ComputePedCollisionWithPlayerResponse (0x4BE7D0).
// Original (hook de la funcion desactivado) frente a candidato con fixtures identicos: el manejador (buffer de 0x34 bytes
// con centinelas en las tareas de respuesta, de decir y parcial), tres peatones (A = dueno del manejador, B = el peaton
// que choca, C = lider; vtable de trampas, matriz propia o posicion en la colocacion, inteligencia falsa), nueve tareas
// falsas de 0x80 bytes (vtable propia: destructor, GetSubTask, IsSimple, GetTaskType y MakeAbortable; la tactiva, la
// activa de B, el seguidor de pandilla 0x4B7, evitar peaton 0x395, ir a punto 0x384, seguir entidad 0x38B, estar en
// grupo 0xF3 y la tarea simple) y un evento escrito a mano (victima +0x14, estados de movimiento +0x30/+0x32). La
// funcion se llama en la direccion del EXE con `this` = manejador. Las tareas nuevas del pool de tareas y las rutas
// nuevas del pool de CPointRoute se siguen entre ejecuciones (m_LastFreeSlot restaurado y huecos libres con 0xCD;
// nombradas por su hueco), se compara su tipo y sus 0x80 (o 0x64) bytes y se borran al final de cada ejecucion.
// Se simulan y trazan: CPed::Say, IKChainManager_c::LookAt, CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox
// (rellena toda la ruta de forma determinista), CPedGroupMembership::CountMembers (tamano del grupo del jugador) y el
// generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber). Se guardan y restauran el reloj y las referencias.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventHandler.h"
#include "Events/EventPedCollisionWithPed.h"
#include "Entity/Ped/PlayerPed.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskComplexFollowPointRoute.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "PedGroupMembership.h"
#include "Pools/Pools.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace EvHGLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(const void* base,size_t off) { return *reinterpret_cast<T*>(const_cast<uint8*>(static_cast<const uint8*>(base))+off); }
// desplazamientos crudos del EXE (CEntity / CPed / tareas)
constexpr size_t P_MATRIX=0x14, P_FLAGS_ENT=0x1C, P_PHYS_FLAGS=0x40, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_STATE=0x530, P_MOVESTATE=0x534, P_HEALTH=0x540, P_TYPE=0x598;
constexpr size_t I_TASKMGR=4, I_SECONDARY=0x18;
constexpr size_t FK_TYPE=0x38;                   // tipo devuelto por GetTaskType, por objeto
constexpr size_t FK_BYTES=0x80;
constexpr size_t ROUTE_BYTES=0x64;
constexpr int FAKES=9;                           // 0 tactiva, 1 activa de B, 2 pandilla, 3 evitar, 4 ir a punto, 5 seguir entidad, 6 en grupo, 7 tarea simple, 8 subtarea del grupo
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84;
alignas(16) inline std::array<uint8,0x40> eh{};  // manejador (0x34 bytes) y relleno
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,3> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,3> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,3> intel{};
alignas(16) inline std::array<std::array<uint8,FK_BYTES>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x60> ev{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[size_t(n)].data()); }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[size_t(n)].data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,12> fakeVtbl{};
inline std::array<void*,24> eventVtbl{};
inline uint8 *poolStorage{}; inline size_t poolCap{};
inline uint8 *routeStorage{}, *routeFlags{}; inline size_t routeCap{};

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    // direcciones de la pila (locales del EXE o de la DLL): no comparables, se marcan como "stack"
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(v>=uintptr_t(poolStorage)&&v<uintptr_t(poolStorage)+poolCap*0x80)return "task"+std::to_string((v-uintptr_t(poolStorage))/0x80)+"+"+std::to_string((v-uintptr_t(poolStorage))%0x80);
    if(v>=uintptr_t(routeStorage)&&v<uintptr_t(routeStorage)+routeCap*ROUTE_BYTES)return "route"+std::to_string((v-uintptr_t(routeStorage))/ROUTE_BYTES)+"+"+std::to_string((v-uintptr_t(routeStorage))%ROUTE_BYTES);
    if(auto s=in(eh.data(),eh.size(),"eh"))return *s;
    for(size_t k=0;k<3;++k){
        if(auto s=in(peds[k].data(),peds[k].size(),k==0?"pedA":k==1?"pedB":"pedC"))return *s;
        if(auto s=in(intel[k].data(),intel[k].size(),k==0?"intelA":k==1?"intelB":"intelC"))return *s; }
    for(size_t k=0;k<mats.size();++k)if(auto s=in(mats[k].data(),mats[k].size(),("mat"+std::to_string(k)).c_str()))return *s;
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
inline uint32 rngState{}; inline int32 routeCount{}, groupCount{}; inline uint32 routeSeed{};

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
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline int16 __fastcall Say(CPed* p,void*,int32 ctx,uint32 delay,float prob,bool ov,bool force,bool front) { trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),ov,force,front}); return 0; }
inline void __fastcall LookAt(void*,void*,const char* purpose,CPed* ped,CEntity* ent,int32 time,int32 bone,CVector* off,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"look_at",std::string(purpose),Name(uintptr_t(ped)),Name(uintptr_t(ent)),time,bone,Name(uintptr_t(off)),torso,F(speed),blend,prio,force}); }
// la ruta del EXE llega con basura salvo el contador (en la pila): el simulado la rellena entera (contador + 8 puntos)
inline int32 __cdecl RouteRound(const CPed* ped,CEntity* ent,const CVector* target,uint8* route,int32 a5) {
    trace.push_back({"route_round",Name(uintptr_t(ped)),Name(uintptr_t(ent)),F(target->x),F(target->y),F(target->z),a5});
    uint32 s=routeSeed; At<int32>(route,0)=routeCount;
    for(size_t k=0;k<24;++k){ s=s*1664525u+1013904223u; At<float>(route,4+4*k)=float(int32(s>>20)-2048)/64.f; }
    return 0; }
inline size_t __fastcall CountMembers(void* self,void*) { trace.push_back({"count_members",uintptr_t(self)==0xC09928}); return size_t(groupCount); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline std::vector<bool> RouteOccupancy() { std::vector<bool> r(routeCap); for(size_t k=0;k<routeCap;++k)r[k]=!(routeFlags[k]&0x80); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=1;
inline const std::array<Fn,NFNS> fns{{
    {"CEventHandler","ComputePedCollisionWithPlayerResponse",0x4BE7D0,0,-1,2048},
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
    if(!std::filesystem::exists(dir/"evhg-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"evhg-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("evhg");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"evhg-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); FillPTraps(std::make_integer_sequence<int,64>{}); FillETraps(std::make_integer_sequence<int,24>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); poolStorage=*reinterpret_cast<uint8**>(pb); poolCap=size_t(*reinterpret_cast<int32*>(pb+8)); }
    { auto* pb=reinterpret_cast<uint8*>(GetPointRoutePool()); routeStorage=*reinterpret_cast<uint8**>(pb); routeFlags=*reinterpret_cast<uint8**>(pb+4); routeCap=size_t(*reinterpret_cast<int32*>(pb+8)); }
    const auto occupancyStart=Occupancy(), routeOccStart=RouteOccupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: el reloj (la tarea de seguir entidad copia el tiempo) se guarda y se restaura en cada ejecucion
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::Say),Say), p1e(0x5EFFE0,Say);
        EntryPatch l1(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), l1e(0x618970,LookAt);
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,CEntity&,const CVector&,CPointRoute&,int32)>(&CPedGeometryAnalyser::ComputeRouteRoundEntityBoundingBox)),RouteRound), g1e(0x5F6110,RouteRound);
        EntryPatch c1(bit_cast<uintptr_t>(&CPedGroupMembership::CountMembers),CountMembers), c1e(0x5F6A50,CountMembers);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot;
                    // los huecos libres se rellenan con 0xCD (el New del EXE no escribe en el objeto): pool de tareas y de CPointRoute
                    { auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto& lf=*reinterpret_cast<int32*>(pb+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* sts=*reinterpret_cast<uint8**>(pb+4); for(size_t k=0;k<poolCap;++k)if(sts[k]&0x80)std::memset(poolStorage+k*0x80,0xCD,0x80); }
                    { auto* pb=reinterpret_cast<uint8*>(GetPointRoutePool()); auto& lf=*reinterpret_cast<int32*>(pb+0xC); static const int32 lr0=lf; lf=lr0;
                      for(size_t k=0;k<routeCap;++k)if(routeFlags[k]&0x80)std::memset(routeStorage+k*ROUTE_BYTES,0xCD,ROUTE_BYTES); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xE76Bu; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    rngState=next(); timeMs=next();
                    routeCount=int32(next()%9); routeSeed=next(); groupCount=int32(1+next()%8);
                    // --- fixtures ---
                    std::memset(eh.data(),0xCD,eh.size());
                    for(auto& m:mats)std::memset(m.data(),0,m.size());
                    for(auto& p:peds)std::memset(p.data(),0,p.size());
                    for(auto& v:intel)std::memset(v.data(),0,v.size());
                    for(auto& b:ev)b=uint8(next()); *reinterpret_cast<void**>(ev.data())=eventVtbl.data();
                    static const uint32 pedTypes[]{0,1,2,3,4,5,6,7,8,6,6};
                    static const int32 moves[]{0,1,2,3,4,5,6,7,8,4,6,7,4,6,7,1};
                    for(int n=0;n<3;++n){
                        CPed* p=Ped(n); p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data();
                        At(p,P_FLAGS_ENT)=next(); At(p,P_PHYS_FLAGS)=next(); At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At(p,P_STATE)=next()%60; At<float>(p,P_HEALTH)=uni(0.f,200.f); At<int32>(p,P_MOVESTATE)=moves[next()%16]; At(p,P_TYPE)=pedTypes[next()%11];
                        At<void*>(p,P_INTEL)=intel[size_t(n)].data();
                        At<void*>(intel[size_t(n)].data(),I_TASKMGR+0x2C)=p;
                    }
                    // tareas falsas: contenido aleatorio (detecta lecturas/escrituras fuera de lo esperado) y vtable propia
                    for(int n=0;n<FAKES;++n){ for(auto& b:fakes[size_t(n)])b=uint8(next()); *reinterpret_cast<void**>(fakes[size_t(n)].data())=fakeVtbl.data(); At<int32>(fakes[size_t(n)].data(),FK_TYPE)=TASK_SIMPLE_PAUSE; }
                    // posiciones/orientaciones: coincidentes, perpendiculares o NaN para los bordes de los productos escalares
                    CVector pa{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,30.f)};
                    uint8* const e=ev.data();
                    At<int16>(e,0xE)=0;
                    At<CPed*>(e,0x14)=next()%16==0 ? nullptr : Ped(1);
                    static const int16 mss[]{0,1,2,3,4,5,6,7,4,4,6,7,1,0,-1,8,4,6,7,4};
                    At<int16>(e,0x30)=next()%12==0 ? int16(next()) : mss[next()%20]; At<int16>(e,0x32)=next()%12==0 ? int16(next()) : mss[next()%20];
                    CVector pb=At<CPed*>(e,0x14) ? CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)}+pa : pa;
                    if(next()%10==0)pb=pa; if(next()%24==0)pb.x=std::bit_cast<float>(0x7FC00000u);
                    for(int n=0;n<2;++n)At<void*>(Ped(n),P_MATRIX)=mats[size_t(n)].data();
                    WriteMatrix(mats[0].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pa); WriteMatrix(mats[1].data(),next()%3 ? 0.f : uni(-0.6f,0.6f),uni(-3.f,3.f),pb);
                    if(next()%10==0)std::memcpy(mats[1].data(),mats[0].data(),0x30); // mismos ejes: productos escalares exactos
                    // C (lider): matriz o posicion en la colocacion; cerca o lejos de A (distancia mayor o menor que 8)
                    { const CVector pc=next()%6==0 ? pa : CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-2.f,2.f)}+pa;
                      if(next()%3){ At<void*>(Ped(2),P_MATRIX)=mats[2].data(); WriteMatrix(mats[2].data(),0.f,uni(-3.f,3.f),pc); }
                      else { At<float>(Ped(2),4)=pc.x; At<float>(Ped(2),8)=pc.y; At<float>(Ped(2),0xC)=pc.z; } }
                    // tareas del dueno (A): ranuras primarias 0-4 (FindTaskByType solo mira 0-3) y secundarias
                    auto putPrimary=[&](int slot,int fake,int type){ At<int32>(fakes[size_t(fake)].data(),FK_TYPE)=type; At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(slot))=Fake(fake); };
                    auto freePrimary=[&]{ for(int tries=0;tries<6;++tries){ const int s=int(next()%5); if(!At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(s)))return s; } return -1; };
                    if(next()%5<3){ const int s=freePrimary(); if(s>=0){ putPrimary(s,2,0x4B7); At<CPed*>(fakes[2].data(),0x10)=next()%3 ? Ped(2) : Ped(1); } }
                    if(next()%2){ const int s=freePrimary(); if(s>=0)putPrimary(s,5,0x38B); }
                    if(next()%2){ const int s=freePrimary(); if(s>=0)putPrimary(s,4,0x384); }
                    if(next()%3){ // evitar peaton: activa (primaria) o secundaria
                        if(next()%2){ const int s=freePrimary(); if(s>=0)putPrimary(s,3,0x395); }
                        else { At<int32>(fakes[3].data(),FK_TYPE)=0x395; At<CTask*>(intel[0].data(),I_SECONDARY+4*(next()%6))=Fake(3); } }
                    if(next()%5<2){ const int s=freePrimary(); if(s>=0){ putPrimary(s,6,0xF3); At<CTask*>(fakes[6].data(),0xC)=next()%3 ? nullptr : Fake(8); } }
                    // tarea simple de ir a un punto (o no) y tactiva (seguir lider en formacion 0x391 o no)
                    CTask* tsimplest=nullptr; CTask* tactive=nullptr;
                    { const uint32 g=next()%8;
                      At<int32>(fakes[7].data(),FK_TYPE)=g<5 ? 0x384 : g==5 ? 0x39E : g==6 ? 0x385 : TASK_SIMPLE_PAUSE;
                      for(size_t k=0;k<3;++k)At<float>(fakes[7].data(),0xC+4*k)=uni(-30.f,30.f);
                      tsimplest=next()%10==0 ? nullptr : Fake(7); }
                    { const uint32 m=next()%8;
                      if(m<=4){ At<int32>(fakes[0].data(),FK_TYPE)=0x391; At<CPed*>(fakes[0].data(),0x10)=next()%3==0 ? nullptr : (next()%2 ? Ped(1) : Ped(2)); tactive=Fake(0); }
                      else if(m==5){ At<int32>(fakes[0].data(),FK_TYPE)=0x392; tactive=Fake(0); } }
                    // tarea activa de B: seguir lider en formacion (tambien dentro de otras) o no
                    { const uint32 m=next()%8;
                      if(m<=4){ At<int32>(fakes[1].data(),FK_TYPE)=0x391; At<CTask*>(intel[1].data(),I_TASKMGR+4*(next()%5))=Fake(1); }
                      else if(m==5){ At<int32>(fakes[1].data(),FK_TYPE)=0x392; At<CTask*>(intel[1].data(),I_TASKMGR+4*(next()%5))=Fake(1); } }
                    std::memset(eh.data(),0xCD,eh.size()); At<void*>(eh.data(),0)=Ped(0);
                    for(size_t off:{size_t(0x24),size_t(0x2C),size_t(0x30)})At(eh.data(),off)=0x5A5A5A5Au; // centinelas
                    // --- llamada ---
                    const auto occ=Occupancy(), rocc=RouteOccupancy();
                    setState(candidate);
                    const uintptr_t call=fn.address;
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    reinterpret_cast<void(__thiscall*)(void*,void*,CTask*,CTask*)>(call)(eh.data(),e,tactive,tsimplest);
                    setState(false);
                    // --- resultados ---
                    auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k))fresh.push_back(pool->GetAt(k));
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json routes=json::array(); { const auto rnow=RouteOccupancy(); for(size_t k=0;k<routeCap;++k)if(!rocc[k]&&rnow[k])routes.push_back({{"slot",k},{"words",Region(routeStorage+k*ROUTE_BYTES,ROUTE_BYTES)}}); }
                    json refs=json::array(); auto refList=[&](CEntity* en){json l=json::array();for(auto* r=en->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(int n=0;n<3;++n)refs.push_back(refList(Ped(n)));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[size_t(n)].data(),fakes[size_t(n)].size(),true));
                    json intelWords=json::array(); for(int n=0;n<3;++n)intelWords.push_back(Region(intel[size_t(n)].data(),intel[size_t(n)].size()));
                    json r={{"handler",Region(eh.data(),0x34)},{"tasks",tasks},{"routes",routes},{"refs",refs},{"pedA",EntHash(peds[0])},{"pedB",EntHash(peds[1])},{"pedC",EntHash(peds[2])},
                            {"intel",intelWords},{"fakes",fakeWords},{"event",Region(ev.data(),ev.size(),true)},{"time",timeMs},{"rng",rngState}};
                    // borrado de las tareas creadas (la de ruta libera su CPointRoute)
                    const size_t traceBefore=trace.size();
                    for(int n=0;n<5;++n){ auto& slot=At<CTask*>(intel[0].data(),I_TASKMGR+4*size_t(n)); for(auto* t:fresh)if(slot==t)slot=nullptr; }
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ&&RouteOccupancy()==rocc;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    CleanRefs();
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
    CleanRefs();
    const bool globalsTouched=timeMs!=savedTime;
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&RouteOccupancy()==routeOccStart&&freeRefs()==savedFree&&timeMs==savedTime}});
}
}
