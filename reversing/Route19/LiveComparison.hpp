#pragma once
// Banco dentro del motor: CTaskComplexFollowNodeRoute (lista de 19, 19 funciones incluidas las virtuales de la vtable 0x8700A8).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// peaton falso (vtable trazada: Teleport en el hueco 14), inteligencia falsa (0xCC), vehiculo falso, subtarea falsa con
// vtable trazada (tipo y respuesta de MakeAbortable por caso; campos de CTaskSimpleGoTo/GoToPointFine), evento falso,
// rutas de nodos/puntos falsas con guardas (CNodeRoute/CPointRoute en buffers propios) y nodos de ruta falsos en las areas
// 0..3: los punteros ThePaths.m_pPathNodes[0..3] se cambian por arrays falsos (o nulos) durante cada ejecucion y se restauran;
// los datos reales de rutas no se tocan. Se simulan y trazan: CWorld::FindGroundZFor3DCoord, CPathFind::ComputeRoute (la ruta
// de salida la fija el caso) y These2NodesAreAdjacent, InteriorManager_c::GetPedsInteriorGroup/GetPedsInterior/
// GetVectorsInterior, CPedGeometryAnalyser::IsWanderPathClear/IsEntityBlockingTarget, CEventHandler::IsTemporaryEvent y el RNG.
// Constructor, Clone y Destructor usan los pools reales de tareas, CPointRoute y CNodeRoute: m_LastFreeSlot, los IDs de los
// slots y los slots libres (0xCD) se restauran en cada ejecucion. CTimer::m_snTimeInMilliseconds y la direccion de nodo vacia
// global 0x8A5F44 se fijan por caso y se restauran. Se compara la memoria de los fixtures (punteros traducidos a nombres),
// las tareas y rutas nuevas de los pools, las referencias, el retorno y la traza.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskComplexFollowNodeRoute.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPointFine.h"
#include "PathFind.h"
#include "NodeRoute.h"
#include "PedGeometryAnalyser.h"
#include "Interior/InteriorManager_c.h"
#include "Events/EventHandler.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "Pools/PointRoutePool.h"
#include "Pools/NodeRoutePool.h"
#include "../ProbeRuntime.hpp"

namespace Route19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T At(const void* base,size_t off) { return *reinterpret_cast<const T*>(static_cast<const uint8*>(base)+off); }

// desplazamientos crudos del EXE
constexpr uintptr_t VT=0x8700A8, TIMER=0xB7CB84, EMPTY_NODE=0x8A5F44, PATH_NODES=0x96F854;
constexpr size_t E_POS=4, E_MATRIX=0x14, E_TYPE=0x36;                                   // CEntity
constexpr size_t P_FLAGS1=0x46C, P_FLAGS4=0x478, P_INTEL=0x47C, P_VEHICLE=0x58C;          // CPed
constexpr size_t I_FOLLOW_DIST=0xCC, PH_SPEED=0x44, INT_NODE=0x3F0;                       // CPedIntelligence, CPhysical, Interior_c
constexpr size_t FAKE_SIZE=0x50, FK_TYPE=0x48, FK_ABORT=0x4C;                             // configuracion detras de los campos de GoToPointFine
constexpr size_t NR_OFF=0x20, PT_OFF=0x20, TASK_SIZE=0x60;                                // rutas dentro de buffers con guardas
constexpr int NAREAS=4, NPA=16; constexpr size_t NODE_SIZE=0x1C;

alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> pedMat{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> veh{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> vehMat{};
alignas(16) inline std::array<uint8,0x100> intel{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,FAKE_SIZE> fake{};
alignas(16) inline std::array<uint8,0x40> event{};
alignas(16) inline std::array<uint8,0x80> nrBuf{};
alignas(16) inline std::array<uint8,0x100> ptBuf{};
alignas(16) inline std::array<std::array<uint8,NPA*NODE_SIZE>,NAREAS> nodes{};
alignas(16) inline std::array<std::array<uint8,0x400>,2> interiors{};
alignas(16) inline std::array<uint8,0x40> intGroup{};
alignas(16) inline std::array<uint8,0x10> argVec{}, argVec2{}, outVec{};
inline std::array<void*,64> pedVtbl{}; inline std::array<void*,12> fakeVtbl{}; inline std::array<void*,16> eventVtbl{};
inline CPed* Ped() { return reinterpret_cast<CPed*>(ped.data()); }
inline CTask* Fake() { return reinterpret_cast<CTask*>(fake.data()); }
inline uint8* NR() { return nrBuf.data()+NR_OFF; }
inline uint8* PT() { return ptBuf.data()+PT_OFF; }

// pools reales (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot): tareas, CPointRoute, CNodeRoute
constexpr int NPOOLS=3; constexpr size_t STRIDE[NPOOLS]{0x80,0x64,0x24}; inline const char* const POOLNAME[NPOOLS]{"task","ptpool","nrpool"};
inline std::array<uint8*,NPOOLS> poolHdr{};
inline uint8* Storage(int k) { return *reinterpret_cast<uint8**>(poolHdr[k]); }
inline uint8* States(int k) { return *reinterpret_cast<uint8**>(poolHdr[k]+4); }
inline int32 Cap(int k) { return *reinterpret_cast<int32*>(poolHdr[k]+8); }
inline int32& Last(int k) { return At<int32>(poolHdr[k],0xC); }
inline std::vector<bool> Occupancy(int k) { std::vector<bool> r(size_t(Cap(k))); for(int32 s=0;s<Cap(k);++s)r[size_t(s)]=!(States(k)[s]&0x80); return r; }
inline std::array<std::vector<bool>,NPOOLS> AllOccupancy() { return {Occupancy(0),Occupancy(1),Occupancy(2)}; }

inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==uintptr_t(pedVtbl.data()))return "ped_vt"; if(v==uintptr_t(fakeVtbl.data()))return "fake_vt"; if(v==uintptr_t(eventVtbl.data()))return "event_vt";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(ped.data(),ped.size(),"ped"))return *s; if(auto s=in(pedMat.data(),pedMat.size(),"pmat"))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(vehMat.data(),vehMat.size(),"vmat"))return *s;
    if(auto s=in(intel.data(),intel.size(),"intel"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(fake.data(),fake.size(),"fake"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(nrBuf.data(),nrBuf.size(),"nr"))return *s; if(auto s=in(ptBuf.data(),ptBuf.size(),"pt"))return *s;
    for(int a=0;a<NAREAS;++a)if(auto s=in(nodes[a].data(),nodes[a].size(),"nodes"))return *s+"#"+std::to_string(a);
    for(int n=0;n<2;++n)if(auto s=in(interiors[n].data(),interiors[n].size(),"interior"))return *s+"#"+std::to_string(n);
    if(auto s=in(intGroup.data(),intGroup.size(),"intgroup"))return *s;
    if(auto s=in(argVec.data(),argVec.size(),"arg"))return *s; if(auto s=in(argVec2.data(),argVec2.size(),"arg2"))return *s;
    if(auto s=in(outVec.data(),outVec.size(),"out"))return *s;
    for(int k=0;k<NPOOLS;++k){ const auto st=uintptr_t(Storage(k)); const auto n=size_t(Cap(k))*STRIDE[k];
        if(v>=st&&v<st+n)return std::string(POOLNAME[k])+"#"+std::to_string((v-st)/STRIDE[k])+"+"+std::to_string((v-st)%STRIDE[k]); }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json Vec(const CVector& v) { return json::array({F(v.x),F(v.y),F(v.z)}); }

// configuracion por caso de las dependencias simuladas
inline float groundZ{}; inline bool groundRet{};
inline std::array<std::array<uint32,8>,4> routeOut{}; inline std::array<uint32,4> routeLen{}; inline uint32 routeCalls{};
inline uint32 adjBits{}, adjCalls{}, wanderBits{}, wanderCalls{};
inline uintptr_t intGroupRet{}, pedIntRet{}, vecIntRet{};
inline bool blockRet{}, tempRet{};
inline int32 eventType{};
inline uint32 seed{}, rngCalls{};

// dependencias
inline float __cdecl FakeGroundZ(float x,float y,float z,bool* found,CEntity** ent) {
    trace.push_back({"ground_z",F(x),F(y),F(z),found!=nullptr,ent!=nullptr}); if(found)*found=groundRet; return groundZ;
}
// EXE 0x452760 / DLL CPathFind::ComputeRoute (thiscall): la ruta de salida la fija el caso (hasta 4 llamadas distintas)
inline void __fastcall FakeComputeRoute(void*,void*,uint32 type,const CVector* from,const CVector* to,const uint32* start,uint8* route) {
    trace.push_back({"compute_route",type&0xFF,Vec(*from),Vec(*to),*start,Name(route)});
    const uint32 k=routeCalls++%4; At<uint32>(route,0)=routeLen[k]; for(uint32 n=0;n<routeLen[k];++n)At<uint32>(route,4+4*n)=routeOut[k][n];
}
inline bool __fastcall FakeAdjacent(void*,void*,uint32 a,uint32 b) { trace.push_back({"adjacent",a,b}); return (adjBits>>(adjCalls++%32))&1; }
inline void* __fastcall FakeIntGroup(void*,void*,const void* p) { trace.push_back({"peds_interior_group",Name(p)}); return reinterpret_cast<void*>(intGroupRet); }
inline void* __fastcall FakePedsInt(void*,void*,const void* p) { trace.push_back({"peds_interior",Name(p)}); return reinterpret_cast<void*>(pedIntRet); }
inline void* __fastcall FakeVecInt(void*,void*,const CVector* v) { trace.push_back({"vectors_interior",Vec(*v)}); return reinterpret_cast<void*>(vecIntRet); }
inline uint32 __cdecl FakeWander(const CVector* a,const CVector* b,float h,int32 n) {
    trace.push_back({"wander_path_clear",Vec(*a),Vec(*b),F(h),n}); const uint32 v=(wanderBits>>(3*(wanderCalls++%10)))&7; return v<4 ? 4u : v-4;
}
inline bool __cdecl FakeBlocking(void* e,const CVector* p,float d) { trace.push_back({"entity_blocking_target",Name(e),Vec(*p),F(d)}); return blockRet; }
inline bool __cdecl FakeTemporary(const void* e) { trace.push_back({"is_temporary_event",Name(e)}); return tempRet; }
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// peaton falso: Teleport (hueco 14, 0x38) trazado; el resto son trampas
inline void __fastcall FakeTeleport(void* p,void*,float x,float y,float z,uint32 reset) { trace.push_back({"teleport",Name(p),F(x),F(y),F(z),reset&0xFF}); }
template<int N> void* __fastcall PTrap(void* self,void*) { trace.push_back({"ped_unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillPedTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
// subtarea falsa
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { trace.push_back({"delete_fake",Name(t),f}); return t; }
inline CTask* __fastcall FClone(CTask* t,void*) { trace.push_back({"clone_fake",Name(t)}); return nullptr; }
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FSimple(CTask*,void*) { return true; }
inline int __fastcall FType(CTask* t,void*) { trace.push_back({"sub_type",Name(t)}); return At<int32>(t,FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const void* e) { trace.push_back({"sub_make_abortable",Name(t),Name(p),prio,Name(e)}); return At<uint8>(t,FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"sub_unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_type",Name(e)}); return eventType; }
template<int N> void* __fastcall ETrap(void* self,void*) { trace.push_back({"event_unexpected_slot",N,Name(self)}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=19;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskComplexFollowNodeRoute","Constructor",0x66EA30,0,-1,256},
    {"CTaskComplexFollowNodeRoute","Destructor",0x66EB70,0,-1,256},
    {"CTaskComplexFollowNodeRoute","Clone",0,0x8700A8,1,256},
    {"CTaskComplexFollowNodeRoute","GetTaskType",0,0x8700A8,4,64},
    {"CTaskComplexFollowNodeRoute","StopTimer",0,0x8700A8,5,256},
    {"CTaskComplexFollowNodeRoute","MakeAbortable",0,0x8700A8,6,1024},
    {"CTaskComplexFollowNodeRoute","CreateNextSubTask",0,0x8700A8,8,1024},
    {"CTaskComplexFollowNodeRoute","CreateFirstSubTask",0,0x8700A8,9,1024},
    {"CTaskComplexFollowNodeRoute","ControlSubTask",0,0x8700A8,10,1024},
    {"CTaskComplexFollowNodeRoute","GetSubTaskType",0x669650,0,-1,256},
    {"CTaskComplexFollowNodeRoute","CalcGoToTaskType",0x66EBE0,0,-1,1024},
    {"CTaskComplexFollowNodeRoute","CalcBlendRatio",0x66EDC0,0,-1,1024},
    {"CTaskComplexFollowNodeRoute","CanGoStraightThere",0x66EF20,0,-1,512},
    {"CTaskComplexFollowNodeRoute","SetTarget",0x671750,0,-1,1024},
    {"CTaskComplexFollowNodeRoute","CreateSubTask",0x669690,0,-1,512},
    {"CTaskComplexFollowNodeRoute","GetLastWaypoint",0x6698E0,0,-1,256},
    {"CTaskComplexFollowNodeRoute","GetNextWaypoint",0x669980,0,-1,256},
    {"CTaskComplexFollowNodeRoute","ComputePathNodes",0x66EFA0,0,-1,1024},
    {"CTaskComplexFollowNodeRoute","ComputeRoute",0x6699E0,0,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(ped.data()); clean(veh.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"route19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"route19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("route19");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"route19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    poolHdr={reinterpret_cast<uint8*>(GetTaskPool()),reinterpret_cast<uint8*>(GetPointRoutePool()),reinterpret_cast<uint8*>(GetNodeRoutePool())};
    FillPedTraps(std::make_integer_sequence<int,64>{}); pedVtbl[14]=reinterpret_cast<void*>(&FakeTeleport);
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[1]=reinterpret_cast<void*>(&FClone); fakeVtbl[2]=reinterpret_cast<void*>(&FSub);
    fakeVtbl[3]=reinterpret_cast<void*>(&FSimple); fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{EMPTY_NODE,4},{PATH_NODES,4*NAREAS}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    const auto occupancyStart=AllOccupancy();
    std::array<int32,NPOOLS> lf0{}; std::array<std::vector<uint8>,NPOOLS> statesStart{};
    for(int k=0;k<NPOOLS;++k){ lf0[k]=Last(k); statesStart[k].assign(States(k),States(k)+Cap(k)); }
    auto restorePools=[&]{
        for(int k=0;k<NPOOLS;++k){
            Last(k)=lf0[k];
            // los IDs de 7 bits de los slots suben con cada New: se devuelven al estado inicial si la ocupacion no cambio
            if(Occupancy(k)==occupancyStart[k])std::memcpy(States(k),statesStart[k].data(),statesStart[k].size());
            // slots libres con 0xCD: los bytes que los constructores no escriben quedan iguales en ambas ejecuciones
            for(int32 s=0;s<Cap(k);++s)if(States(k)[s]&0x80)std::memset(Storage(k)+size_t(s)*STRIDE[k],0xCD,STRIDE[k]);
        }
    };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch d1(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),FakeGroundZ), d1e(0x5696C0,FakeGroundZ);
        EntryPatch d2(bit_cast<uintptr_t>(&CPathFind::ComputeRoute),FakeComputeRoute), d2e(0x452760,FakeComputeRoute);
        EntryPatch d3(bit_cast<uintptr_t>(&CPathFind::These2NodesAreAdjacent),FakeAdjacent), d3e(0x44D230,FakeAdjacent);
        EntryPatch d4(bit_cast<uintptr_t>(&InteriorManager_c::GetPedsInteriorGroup),FakeIntGroup), d4e(0x598240,FakeIntGroup);
        EntryPatch d5(bit_cast<uintptr_t>(&InteriorManager_c::GetPedsInterior),FakePedsInt), d5e(0x598620,FakePedsInt);
        EntryPatch d6(bit_cast<uintptr_t>(&InteriorManager_c::GetVectorsInterior),FakeVecInt), d6e(0x5983D0,FakeVecInt);
        EntryPatch d7(bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsWanderPathClear),FakeWander), d7e(0x5F2F70,FakeWander);
        EntryPatch d8(bit_cast<uintptr_t>(&CPedGeometryAnalyser::IsEntityBlockingTarget),FakeBlocking), d8e(0x5F3970,FakeBlocking);
        EntryPatch d9(bit_cast<uintptr_t>(&CEventHandler::IsTemporaryEvent),FakeTemporary), d9e(0x4BC370,FakeTemporary);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    restorePools(); restoreGlobals(); CleanRefs();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1919u; if(!st)st=1; trace=json::array(); seed=uint32(i); rngCalls=0; freshIds.clear();
                    routeCalls=0; adjCalls=0; wanderCalls=0;
                    const uint32 now=next()%8==0 ? 0xFFFFF000u+next()%0x2000 : 100000u+next()%1000000u;
                    At<uint32>(reinterpret_cast<void*>(TIMER),0)=now;
                    if(next()%3==0)At<uint32>(reinterpret_cast<void*>(EMPTY_NODE),0)=next();

                    // nodos de ruta falsos (areas 0..3): cargadas o no por caso
                    std::array<bool,NAREAS> loaded{}; std::vector<int> loadedAreas;
                    for(int a=0;a<NAREAS;++a){
                        loaded[a]=next()%4!=0; if(loaded[a])loadedAreas.push_back(a);
                        for(auto& b:nodes[a])b=uint8(next());
                        for(int n=0;n<NPA;++n)for(int c=0;c<3;++c)At<int16>(nodes[a].data(),size_t(n)*NODE_SIZE+8+2*c)=int16(int(next()%641)-320);
                        At<void*>(reinterpret_cast<void*>(PATH_NODES),4*size_t(a))=loaded[a] ? nodes[a].data() : nullptr;
                    }
                    auto nodeAddr=[&](bool onlyLoaded,bool allowEmpty)->uint32{
                        if(allowEmpty&&next()%10==0)return 0xFFFFu|(next()%NPA)<<16;
                        if(onlyLoaded)return uint32(loadedAreas[next()%loadedAreas.size()])|(next()%NPA)<<16;
                        return (next()%NAREAS)|(next()%NPA)<<16;
                    };

                    // peaton, inteligencia y vehiculo
                    auto* p=ped.data(); std::memset(p,0,ped.size()); std::memset(pedMat.data(),0,pedMat.size());
                    At<void*>(p,0)=pedVtbl.data(); p[E_TYPE]=3;
                    const CVector pedPos{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,20.f)};
                    if(next()%4){ At<void*>(p,E_MATRIX)=pedMat.data(); At<CVector>(pedMat.data(),0x30)=pedPos; } else At<CVector>(p,E_POS)=pedPos;
                    At<uint32>(p,P_FLAGS1)=next()%4==0 ? (next()|0x100u) : (next()&~0x100u);
                    At<uint32>(p,P_FLAGS4)=next()%3==0 ? (next()|0x8000u) : (next()&~0x8000u);
                    for(auto& b:intel)b=uint8(next()); At<void*>(p,P_INTEL)=intel.data();
                    { static const float fd[]{0.f,5.f,20.f,1000.f}; At<float>(intel.data(),I_FOLLOW_DIST)=next()%5 ? fd[next()%4] : uni(0.f,60.f); }
                    At<void*>(p,P_VEHICLE)=veh.data();
                    auto* v=veh.data(); std::memset(v,0,veh.size()); std::memset(vehMat.data(),0,vehMat.size());
                    v[E_TYPE]=2; At<void*>(v,E_MATRIX)=vehMat.data(); At<CVector>(vehMat.data(),0x30)=CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,20.f)};
                    { const uint32 m=next()%6; At<CVector>(v,PH_SPEED)=m==0 ? CVector{0.125f,0.f,0.f} : m==1 ? CVector{0.f,0.f,0.f} : m==2 ? CVector{uni(-1.f,1.f),uni(-1.f,1.f),0.f}
                                                                : CVector{uni(-0.1f,0.1f),uni(-0.1f,0.1f),uni(-0.05f,0.05f)}; }

                    // subtarea falsa (CTaskSimpleGoTo: objetivo 0xC, radio 0x18; GoToPointFine: ratio 0x20) y evento falso
                    for(size_t b=0;b<FAKE_SIZE;b+=4)At<float>(fake.data(),b)=uni(-30.f,30.f);
                    At<void*>(fake.data(),0)=fakeVtbl.data(); At<uint32>(fake.data(),4)=0;
                    { static const int32 types[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_PAUSE};
                      At<int32>(fake.data(),FK_TYPE)=types[next()%6]; }
                    At<uint32>(fake.data(),FK_ABORT)=next()%4!=0;
                    for(auto& b:event)b=uint8(next()); At<void*>(event.data(),0)=eventVtbl.data(); At<void*>(event.data(),0x14)=next()%5 ? (void*)veh.data() : nullptr;
                    { static const int32 et[]{EVENT_VEHICLE_COLLISION,EVENT_POTENTIAL_WALK_INTO_VEHICLE,EVENT_VEHICLE_COLLISION,EVENT_POTENTIAL_WALK_INTO_VEHICLE,0,2,9,55,57}; eventType=et[next()%9]; }

                    // dependencias simuladas
                    groundRet=next()%3!=0; groundZ=next()%8==0 ? qnan : uni(-5.f,25.f);
                    intGroupRet=next()%3==0 ? uintptr_t(intGroup.data()) : 0;
                    pedIntRet=next()%3==0 ? 0 : uintptr_t(interiors[next()%2].data());
                    { const uint32 m=next()%3; vecIntRet=m==0 ? 0 : uintptr_t(interiors[m-1].data()); if(next()%4==0)vecIntRet=pedIntRet; }
                    for(auto& in:interiors){ for(auto& b:in)b=uint8(next()); At<uint32>(in.data(),INT_NODE)=nodeAddr(false,false); }
                    adjBits=next(); wanderBits=next(); blockRet=next()%2; tempRet=next()%3==0;
                    const bool onlyLoaded=!intGroupRet;   // sin grupo de interior el bucle del EXE usa restos de la pila si el nodo no esta cargado
                    for(int k=0;k<4;++k){
                        routeLen[k]=onlyLoaded&&loadedAreas.empty() ? 0 : next()%9;
                        for(uint32 n=0;n<8;++n)routeOut[k][n]=n<routeLen[k] ? nodeAddr(onlyLoaded,!onlyLoaded&&n==0&&nm=="ComputePathNodes") : next();
                    }

                    // rutas falsas con guardas (puntos: flotantes finitos en todo el buffer)
                    for(size_t b=0;b<nrBuf.size();b+=4)At<uint32>(nrBuf.data(),b)=nodeAddr(false,false);
                    const uint32 nrCount=next()%9; At<uint32>(NR(),0)=nrCount;
                    for(uint32 n=0;n<8;++n)At<uint32>(NR(),4+4*n)=nodeAddr(false,nm!="ComputeRoute");
                    for(size_t b=0;b<ptBuf.size();b+=4)At<float>(ptBuf.data(),b)=uni(-40.f,40.f);
                    const uint32 ptCount=next()%9; At<uint32>(PT(),0)=ptCount;
                    { // puntos cercanos al peaton, a veces repetidos o alineados
                        for(uint32 n=0;n<8;++n){ CVector q{pedPos.x+uni(-12.f,12.f),pedPos.y+uni(-12.f,12.f),pedPos.z+uni(-2.f,2.f)};
                            if(n>0&&next()%6==0)q=At<CVector>(PT(),4+12*(n-1));
                            At<CVector>(PT(),4+12*n)=q; }
                    }

                    // tarea probada
                    auto* o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t b=4;b<TASK_SIZE;++b)o[b]=uint8(next());
                    At<uintptr_t>(o,0)=VT; At<uint32>(o,4)=0; At<CTask*>(o,8)=Fake();
                    const CVector target{pedPos.x+uni(-30.f,30.f),pedPos.y+uni(-30.f,30.f),pedPos.z+uni(-4.f,4.f)};
                    At<CVector>(o,0xC)=target;
                    { static const int32 ms[]{4,5,6,7,7,6,1,0}; At<int32>(o,0x18)=ms[next()%8]; }
                    At<float>(o,0x1C)=uni(0.f,3.f); At<float>(o,0x20)=uni(0.f,5.f); At<float>(o,0x24)=next()%4 ? 2.f : uni(0.f,6.f);
                    At<uint32>(o,0x28)=nodeAddr(false,true);
                    At<void*>(o,0x2C)=NR(); At<void*>(o,0x30)=PT();
                    At<uint32>(o,0x34)=next()%2&&routeLen[0] ? routeOut[0][next()%routeLen[0]] : nodeAddr(false,true);
                    At<uint32>(o,0x38)=next()%8==0 ? ptCount : (ptCount ? next()%ptCount : 0);
                    At<int32>(o,0x3C)=next()%3==0 ? -1 : int32(next()%5000);
                    { const uint32 ago=next()%6000; At<uint32>(o,0x40)=now-ago; At<int32>(o,0x44)=next()%6==0 ? -1 : int32(next()%6000); o[0x48]=uint8(next()%3!=0); o[0x49]=uint8(next()%3==0); }
                    { static const float sd[]{0.f,4.f,5.f}; At<float>(o,0x50)=next()%3 ? sd[next()%3] : uni(0.f,15.f); At<float>(o,0x54)=next()%3 ? sd[next()%3] : uni(0.f,15.f); }
                    At<float>(o,0x58)=uni(0.f,3.f); At<float>(o,0x5C)=uni(0.f,3.f);
                    if(nm=="SetTarget"||nm=="CreateFirstSubTask"||nm=="CreateNextSubTask"||nm=="ControlSubTask"||nm=="ComputePathNodes"){
                        // objetivo cerca del peaton para pasar por "puede ir en linea recta"
                        if(next()%3==0)At<CVector>(o,0xC)=CVector{pedPos.x+uni(-4.f,4.f),pedPos.y+uni(-4.f,4.f),pedPos.z+uni(-1.f,1.f)};
                    }

                    const auto occ=AllOccupancy();
                    json out=json::object(); json ret=nullptr; CPed* pd=Ped();
                    // Destructor: rutas reales de los pools (o nulas)
                    std::array<uint8*,2> ownRoutes{};
                    if(nm=="Destructor"){
                        if(next()%5){ ownRoutes[0]=reinterpret_cast<uint8*>(GetPointRoutePool()->New()); for(size_t b=0;b<0x64;++b)ownRoutes[0][b]=uint8(next()); }
                        if(next()%5){ ownRoutes[1]=reinterpret_cast<uint8*>(GetNodeRoutePool()->New()); for(size_t b=0;b<0x24;++b)ownRoutes[1][b]=uint8(next()); }
                        At<void*>(o,0x30)=ownRoutes[0]; At<void*>(o,0x2C)=ownRoutes[1];
                        if(next()%4==0)At<CTask*>(o,8)=nullptr;
                    }
                    const auto occCall=AllOccupancy();

                    for(auto* h:hooks)h->State(candidate);
                    const auto vtbl=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target2=fn.slot>=0 ? vtbl[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target2>=0x400000&&target2<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    if(nm=="Constructor"){
                        static const int32 ms[]{4,5,6,7,1,0}; const int32 m=ms[next()%6];
                        At<CVector>(argVec.data(),0)=target;
                        const float tol=next()%3 ? 0.5f : uni(0.f,3.f), slow=next()%3 ? 3.f : uni(0.f,5.f), hgt=next()%3 ? 2.f : uni(0.f,5.f);
                        const uint32 keep=next()%2, blend=next()%2; const int32 time=next()%3 ? -1 : int32(next()%5000);
                        std::memset(o,0xCD,object.size());   // constructor sobre un buffer con 0xCD, comparado entero
                        ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,float,float,uint32,int32,uint32)>(target2)(o,m,reinterpret_cast<const CVector*>(argVec.data()),tol,slow,hgt,keep,time,blend));
                        for(auto* h:hooks)h->State(false);
                        out["type"]=reinterpret_cast<CTask*>(o)->GetTaskType(); // vptr EXE o DLL: se compara el tipo
                    }
                    else if(nm=="Destructor"){
                        reinterpret_cast<void(__thiscall*)(void*)>(target2)(o);
                        for(auto* h:hooks)h->State(false);
                        json fr=json::array(); for(int k=0;k<2;++k){ if(!ownRoutes[k]){fr.push_back(nullptr);continue;} const int pk=k+1;
                            const auto slot=(uintptr_t(ownRoutes[k])-uintptr_t(Storage(pk)))/STRIDE[pk]; fr.push_back(States(pk)[slot]); }
                        out["route_states"]=fr;
                    }
                    else if(nm=="Clone"){ ret=Name(reinterpret_cast<CTask*(__thiscall*)(void*)>(target2)(o)); }
                    else if(nm=="GetTaskType"){ ret=reinterpret_cast<int32(__thiscall*)(void*)>(target2)(o); }
                    else if(nm=="StopTimer"){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(target2)(o,event.data()); }
                    else if(nm=="MakeAbortable"){
                        { static const int32 types[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_STAND_STILL};
                          At<int32>(fake.data(),FK_TYPE)=types[next()%6]; }
                        if(next()%3==0)At<uint32>(o,0x38)=ptCount-1;
                        const int prio=int(next()%3); const void* ev=next()%6 ? (const void*)event.data() : nullptr;
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const void*)>(target2)(o,pd,prio,ev);
                    }
                    else if(nm=="CreateNextSubTask"||nm=="CreateFirstSubTask"||nm=="ControlSubTask"){
                        if(nm=="ControlSubTask"&&next()%3==0)o[0x4C]&=~4;   // sin objetivo nuevo: se llega a la mezcla
                        if(nm=="ControlSubTask"&&next()%2)o[0x4C]|=8;
                        ret=Name(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target2)(o,pd));
                    }
                    else if(nm=="GetSubTaskType"){
                        const uint32 prog=next()%4==0 ? ptCount : next()%10; const uint32 last=next()%2;
                        ret=reinterpret_cast<int32(__cdecl*)(uint32,uint32,const void*)>(target2)(prog,last,PT());
                    }
                    else if(nm=="CalcGoToTaskType"){
                        static const int32 types[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_FINISHED};
                        if(next()%8==0)At<uint32>(o,0x38)=ptCount+1;
                        ret=reinterpret_cast<int32(__thiscall*)(void*,CPed*,int32)>(target2)(o,pd,types[next()%6]);
                    }
                    else if(nm=="CalcBlendRatio"){
                        const uint32 slowing=next()%2;
                        if(next()%6==0){ // distancia exactamente igual al limite
                            const CVector& q=At<CVector>(PT(),4+12*At<uint32>(o,0x38)); const float dx=q.x-pedPos.x, dy=q.y-pedPos.y, dz=q.z-pedPos.z;
                            const float d2=dz*dz+dy*dy+dx*dx; At<float>(o,0x50)=std::sqrt(d2); }
                        ret=F(reinterpret_cast<float(__thiscall*)(void*,CPed*,uint32)>(target2)(o,pd,slowing));
                    }
                    else if(nm=="CanGoStraightThere"){
                        const CVector a=next()%2 ? pedPos : CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,20.f)};
                        const CVector b{a.x+uni(-25.f,25.f),a.y+uni(-25.f,25.f),a.z+uni(-3.f,3.f)};
                        At<CVector>(argVec.data(),0)=a; At<CVector>(argVec2.data(),0)=b;
                        const float dx=a.x-b.x, dy=a.y-b.y, dz=a.z-b.z; const float d=std::sqrt(dz*dz+dx*dx+dy*dy);
                        const uint32 m=next()%6; const float maxD=m==0 ? d : m==1 ? qnan : m==2 ? std::nextafter(d,0.f) : uni(0.f,40.f);
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,const CVector*,const CVector*,float)>(target2)(o,pd,reinterpret_cast<const CVector*>(argVec.data()),reinterpret_cast<const CVector*>(argVec2.data()),maxD);
                    }
                    else if(nm=="SetTarget"){
                        CVector t=next()%3==0 ? At<CVector>(o,0xC) : CVector{pedPos.x+uni(-30.f,30.f),pedPos.y+uni(-30.f,30.f),pedPos.z+uni(-4.f,4.f)};
                        float tol=At<float>(o,0x1C), slow=At<float>(o,0x20), hgt=At<float>(o,0x24);
                        { const uint32 m=next()%5; if(m==1)tol=uni(0.f,3.f); else if(m==2)slow=uni(0.f,5.f); else if(m==3)hgt=uni(0.f,5.f); }
                        At<CVector>(argVec.data(),0)=t; const uint32 force=next()%3==0;
                        reinterpret_cast<void(__thiscall*)(void*,CPed*,const CVector*,float,float,float,uint32)>(target2)(o,pd,reinterpret_cast<const CVector*>(argVec.data()),tol,slow,hgt,force);
                    }
                    else if(nm=="CreateSubTask"){
                        static const int32 types[]{TASK_SIMPLE_GO_TO_POINT,TASK_SIMPLE_GO_TO_POINT_FINE,TASK_COMPLEX_GO_TO_POINT_AND_STAND_STILL,TASK_COMPLEX_LEAVE_CAR,TASK_SIMPLE_STAND_STILL,TASK_FINISHED,TASK_SIMPLE_PAUSE,TASK_NONE};
                        ret=Name(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target2)(o,types[next()%8],pd));
                    }
                    else if(nm=="GetLastWaypoint"||nm=="GetNextWaypoint"){
                        if(next()%6==0)At<uint32>(o,0x38)=0;
                        std::memset(outVec.data(),0xCD,outVec.size());
                        ret=Name(reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,CPed*)>(target2)(o,reinterpret_cast<CVector*>(outVec.data()),pd));
                        out["vec"]=Region(outVec.data(),12);
                    }
                    else if(nm=="ComputePathNodes"){ reinterpret_cast<void(__thiscall*)(void*,const CPed*)>(target2)(o,pd); }
                    else if(nm=="ComputeRoute"){ reinterpret_cast<void(__thiscall*)(void*)>(target2)(o); }
                    for(auto* h:hooks)h->State(false);

                    // tareas y rutas nuevas de los pools
                    std::vector<CTask*> fresh; json tasks=json::array(), routes=json::array();
                    for(int32 s=0;s<Cap(0);++s)if(!occCall[0][s]&&!(States(0)[s]&0x80)){ auto* t=reinterpret_cast<CTask*>(Storage(0)+size_t(s)*STRIDE[0]); fresh.push_back(t); freshIds[uintptr_t(t)]=int(fresh.size()-1); }
                    for(auto* t:fresh)tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,STRIDE[0],true)}});
                    for(int k=1;k<NPOOLS;++k)for(int32 s=0;s<Cap(k);++s)if(!occCall[k][s]&&!(States(k)[s]&0x80))routes.push_back({POOLNAME[k],s,Region(Storage(k)+size_t(s)*STRIDE[k],STRIDE[k])});
                    out["tasks"]=tasks; out["routes"]=routes;
                    out["object"]=Region(o,TASK_SIZE,true); out["sub"]=Region(fake.data(),FAKE_SIZE,true);
                    out["nr"]=Region(nrBuf.data(),nrBuf.size()); out["pt"]=Region(ptBuf.data(),ptBuf.size());
                    out["refs"]=json::array({RefList(ped.data()),RefList(veh.data())});

                    // limpieza: tareas nuevas (sus rutas se liberan con ellas), rutas nuevas sueltas y rutas del destructor
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    for(int32 s=0;s<Cap(1);++s)if(!occ[1][s]&&!(States(1)[s]&0x80))GetPointRoutePool()->Delete(reinterpret_cast<CPointRoute*>(Storage(1)+size_t(s)*STRIDE[1]));
                    for(int32 s=0;s<Cap(2);++s)if(!occ[2][s]&&!(States(2)[s]&0x80))GetNodeRoutePool()->Delete(reinterpret_cast<CNodeRoute*>(Storage(2)+size_t(s)*STRIDE[2]));
                    CleanRefs();
                    out["ret"]=ret; out["trace"]=trace; out["delete_trace"]=delTrace; out["rng"]=rngCalls; out["switches"]=switches;
                    out["clean"]=AllOccupancy()==occ;
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
    CleanRefs(); restoreGlobals(); restorePools();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=AllOccupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
