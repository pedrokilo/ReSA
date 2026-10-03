#pragma once
// Banco dentro del motor: grupo de eventos, rodear edificio y hierba (lista de 12, 36 funciones).
// CEventGroup (constructor, Add, Flush, GetEventOfType, GetHighestPriorityEvent, HasEvent, HasEventOfType,
// HasScriptCommandOfTaskType, Remove, RemoveInvalidEvents, Reorganise, TickEvents),
// CTaskComplexWalkRoundBuildingAttempt (constructor, destructor, Clone, GetTaskType, MakeAbortable, CreateNextSubTask,
// CreateFirstSubTask, ControlSubTask, CreateSubTask, ComputeMoveDir, ComputeCrapRoute, ComputeRoute) y CGrassRenderer
// (Initialise, Shutdown, AddTriPlant, DrawTriPlants, FlushTriPlantBuffer, Get/SetPlantModelsTab, SetCloseFarAlphaDist,
// SetCurrentScanCode, SetGlobalCameraPos, SetGlobalWindBending, GenPointInTriangle).
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// grupo de eventos y 20 eventos falsos con vtable trazada (tipo, prioridad, Clone, AffectsPed, HasEditableResponse,
// IsValid, ReportCriminalEvent, borrado), tareas falsas de los comandos de script, peaton falso con matriz e inteligencia,
// entidad falsa con matriz, subtarea falsa, rutas en arena propia, plantas, atomicos, frames y geometrias falsos.
// Se simulan y trazan: CEventEditableResponse (ComputeResponseTaskOfType, ComputeResponseTaskType, WillRespond,
// InformGroup, InformRespectedFriends, TriggerLookAt, InformVehicleOccupants), CPedIntelligence::RecordEventForScript,
// CPointRoute::operator new/delete (arena), CWorld::GetIsLineOfSightClear/ProcessLineOfSight, CGeneral (angulos),
// CPPTriPlantBuffer::Flush, RwFrameTranslate, RwMatrixUpdate, RpGeometryForAllMaterials, el callback de render del
// atomico y el RNG (rand/srand del EXE, CGeneral::GetRandomNumber y srand de la DLL). CVector (Normalise,
// NormaliseAndMag, CrossProduct), los constructores de las subtareas y el pool de tareas corren de verdad.
// Se guardan y restauran en cada ejecucion y al final: globales de la hierba (0xC02DB8..0xC02DE4, 0x8D132C, 0x8D1330,
// 0xC02DC0), gTriPlantBuf (0xC02DE8), la calidad de efectos de g_fx, el pool de tareas y la lista libre de referencias.
// FIX_BUGS: DrawTriPlants siembra srand con los bits de la semilla en vez de truncarla; en los fixtures la semilla es 0.0f.
// Fuera de los fixtures: huecos nulos en GetEventOfType/HasEventOfType/GetHighestPriorityEvent/TickEvents/Flush(true)
// (el EXE se cae), contadores del grupo negativos o > 16, Clone nulo en Add, rutas con mas de 8 puntos, flags de
// constructor distintos de 0/1, matrices nulas de peaton (camino casi nulo de ComputeMoveDir) y de entidad (AllocateMatrix
// usa el pool real), NaN en GenPointInTriangle/DrawTriPlants (propagacion de carga x87 frente a SSE) y el valor de retorno
// de GenPointInTriangle (el EXE deja v1 en eax: la funcion es void en el original).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Events/EventGroup.h"
#include "Events/EventEditableResponse.h"
#include "Tasks/TaskTypes/TaskComplexWalkRoundBuildingAttempt.h"
#include "GrassRenderer.h"
#include "PPTriPlantBuffer.h"
#include "PedGeometryAnalyser.h"
#include "PedIntelligence.h"
#include "Pools/TaskPool.h"
#include "General.h"
#include "World.h"
#include "Fx/Fx.h"
#include "../ProbeRuntime.hpp"

namespace Mix12Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// direcciones del EXE
constexpr uintptr_t EVG_VT=0x85AAB0, WRBA_VT=0x86F334;
constexpr uintptr_t G_WIND=0xC02DB8, G_CLOSE=0xC02DBC, G_CAM=0xC02DDC, G_FAR=0x8D132C, G_SCAN=0x8D1330, G_TEX=0xC02DC0, G_TRIBUF=0xC02DE8;
// desplazamientos crudos del EXE
constexpr size_t E_POS=0x4, E_MATRIX=0x14, P_FLAGS4=0x478, P_INTEL=0x47C, I_COUNTER=0x274;
constexpr size_t EV_SIZE=0x48, NEV=20, NETASK=3, ROUTE=0x64, NROUTES=8, TRIBUF_SIZE=sizeof(CPPTriPlantBuffer), NPLANTS=8;

// --- buffers propios ---
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, ent{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, entMat{};
alignas(16) inline std::array<uint8,0x300> intel{};
alignas(16) inline std::array<std::array<uint8,EV_SIZE>,NEV> evs{};
alignas(16) inline std::array<std::array<uint8,0x30>,NETASK> etasks{};
alignas(16) inline std::array<uint8,0x60> group{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x30> sub{};
alignas(16) inline std::array<uint8,ROUTE*NROUTES> routes{};
alignas(16) inline std::array<float,12> vecs{};
alignas(16) inline std::array<std::array<uint8,sizeof(PPTriPlant)>,NPLANTS> plants{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> atomics{}, geoms{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> frames{};
alignas(16) inline std::array<uint8,0x40> texs{}, tabTargets{};
alignas(16) inline std::array<void*,4> modelTab{};
inline std::array<void*,20> evVtbl{}, etVtbl{}, subVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
struct Named { const void* p; size_t n; std::string name; };
inline std::vector<Named> named;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& r:named)if(v>=uintptr_t(r.p)&&v<uintptr_t(r.p)+r.n)return r.name+"+"+std::to_string(v-uintptr_t(r.p));
    { ULONG_PTR lo{},hi{}; GetCurrentThreadStackLimits(&lo,&hi); if(v>=lo&&v<hi)return "stack"; }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t from,size_t to) { json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
inline json Vec(const void* p) { const auto* f=static_cast<const float*>(p); return json::array({F(f[0]),F(f[1]),F(f[2])}); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// --- configuracion por caso ---
inline uint32 rngState{}, maskA{}, callsA{}, maskB{}, callsB{}, maskC{}, callsC{}, routeUsed{};
inline bool NextA() { return (maskA>>(callsA++%32))&1; }
inline bool NextB() { return (maskB>>(callsB++%32))&1; }
inline bool NextC() { return (maskC>>(callsC++%32))&1; }
inline std::array<float,4> hitFrac{}; inline uint32 hitCalls{}; inline bool hitExact{}; inline void* hitEntity{};

// --- eventos falsos: tipo +0x30, prioridad +0x34, AffectsPed +0x38, HasEditableResponse +0x39, IsValid +0x3A,
//     respuestas +0x3B (bit0 informar grupo, bit1 amigos, bit2 mirar, bit3 WillRespond), resultado de Clone +0x3C ---
template<int N> void* __fastcall EvTrap(void* e,void*) { trace.push_back({"event_unexpected_slot",N,Name(e)}); return nullptr; }
template<int... N> void FillEvTraps(std::integer_sequence<int,N...>) { ((evVtbl[N]=reinterpret_cast<void*>(&EvTrap<N>)),...); }
inline void* __fastcall EvDtor(void* e,void*,uint32 f) { trace.push_back({"event_delete",Name(e),f}); return e; }
inline int32 __fastcall EvType(void* e,void*) { const int32 t=At<int32>(e,0x30); trace.push_back({"event_type",Name(e),t}); return t; }
inline int32 __fastcall EvPrio(void* e,void*) { const int32 t=At<int32>(e,0x34); trace.push_back({"event_priority",Name(e),t}); return t; }
inline void* __fastcall EvClone(void* e,void*) { void* c=At<void*>(e,0x3C); trace.push_back({"event_clone",Name(e),Name(c)}); return c; }
inline bool __fastcall EvAffects(void* e,void*,void* ped) { const bool r=At<uint8>(e,0x38)&1; trace.push_back({"event_affects_ped",Name(e),Name(ped),r}); return r; }
inline void __fastcall EvReport(void* e,void*,void* ped) { trace.push_back({"event_report_criminal",Name(e),Name(ped)}); }
inline bool __fastcall EvEditable(void* e,void*) { const bool r=At<uint8>(e,0x39)&1; trace.push_back({"event_has_editable_response",Name(e),r}); return r; }
inline bool __fastcall EvIsValid(void* e,void*,void* ped) { const bool r=At<uint8>(e,0x3A)&1; trace.push_back({"event_is_valid",Name(e),Name(ped),r}); return r; }
// tareas falsas de los comandos de script (tipo +0x2C)
template<int N> void* __fastcall EtTrap(void* t,void*) { trace.push_back({"script_task_unexpected_slot",N,Name(t)}); return nullptr; }
template<int... N> void FillEtTraps(std::integer_sequence<int,N...>) { ((etVtbl[N]=reinterpret_cast<void*>(&EtTrap<N>)),...); }
inline int32 __fastcall EtType(void* t,void*) { const int32 r=At<int32>(t,0x2C); trace.push_back({"script_task_type",Name(t),r}); return r; }
// subtarea falsa (tipo +0x2C, MakeAbortable segun la mascara C)
template<int N> void* __fastcall SubTrap(void* s,void*) { trace.push_back({"sub_unexpected_slot",N,Name(s)}); return nullptr; }
template<int... N> void FillSubTraps(std::integer_sequence<int,N...>) { ((subVtbl[N]=reinterpret_cast<void*>(&SubTrap<N>)),...); }
inline void* __fastcall SubDtor(void* s,void*,uint32 f) { trace.push_back({"sub_delete",Name(s),f}); return s; }
inline int32 __fastcall SubType(void* s,void*) { const int32 t=At<int32>(s,0x2C); trace.push_back({"sub_type",Name(s),t}); return t; }
inline bool __fastcall SubAbort(void* s,void*,void* ped,int32 prio,const void* ev) { const bool r=NextC(); trace.push_back({"sub_make_abortable",Name(s),Name(ped),prio,Name(ev),r}); return r; }

// --- CEventEditableResponse / CPedIntelligence ---
inline bool __fastcall FakeResponseOfType(void* e,void*,void* ped,int32 task) {
    const uint8 m=At<uint8>(e,0x3B); const bool r=task==1200 ? (m&1) : task==1700 ? ((m>>1)&1) : task==300 ? ((m>>2)&1) : false;
    trace.push_back({"compute_response_task_of_type",Name(e),Name(ped),task,r}); return r;
}
inline void __fastcall FakeResponseType(void* e,void*,void* ped,uint32 inGroup) { trace.push_back({"compute_response_task_type",Name(e),Name(ped),inGroup&0xFF}); }
inline bool __fastcall FakeWillRespond(void* e,void*) { const bool r=(At<uint8>(e,0x3B)>>3)&1; trace.push_back({"will_respond",Name(e),r}); return r; }
inline void __fastcall FakeInformGroup(void* e,void*,void* ped) { trace.push_back({"inform_group",Name(e),Name(ped)}); }
inline void __fastcall FakeInformFriends(void* e,void*,void* ped) { trace.push_back({"inform_respected_friends",Name(e),Name(ped)}); }
inline void __fastcall FakeLookAt(void* e,void*,void* ped) { trace.push_back({"trigger_look_at",Name(e),Name(ped)}); }
inline void __fastcall FakeInformOccupants(void* e,void*,void* ped) { trace.push_back({"inform_vehicle_occupants",Name(e),Name(ped)}); }
inline void __fastcall FakeRecordEvent(void* i,void*,int32 id,int32 prio) { trace.push_back({"record_event_for_script",Name(i),id,prio}); }

// --- RNG ---
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline void __cdecl FakeSrand(uint32 s) { trace.push_back({"srand",s}); rngState=s; }

// --- rodear edificio ---
inline void* __cdecl FakeRouteNew(uint32 size) { void* p=routeUsed<NROUTES-2 ? routes.data()+ROUTE*routeUsed++ : nullptr; trace.push_back({"route_new",size,Name(p)}); return p; }
inline void __cdecl FakeRouteDelete(void* p) { trace.push_back({"route_delete",Name(p)}); }
inline bool __cdecl FakeLosClear(const CVector* o,const CVector* t,uint32 b0,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6) {
    const bool r=NextA(); trace.push_back({"los_clear",Vec(o),Vec(t),b0&0xFF,b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,b6&0xFF,r}); return r;
}
inline bool __cdecl FakeProcessLos(const CVector* o,const CVector* t,CColPoint* cp,CEntity** e,uint32 b0,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6,uint32 b7) {
    const bool r=NextB();
    trace.push_back({"process_los",Vec(o),Vec(t),Name(cp),Name(e),b0&0xFF,b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,b6&0xFF,b7&0xFF,r});
    if(r){ CVector p; if(hitExact)p=CVector{o->x+0.35f,o->y,o->z}; else { const float f=hitFrac[hitCalls++%hitFrac.size()]; p=CVector{o->x+(t->x-o->x)*f,o->y+(t->y-o->y)*f,o->z+(t->z-o->z)*f}; }
           cp->m_vecPoint=p; *e=static_cast<CEntity*>(hitEntity); }
    return r;
}
inline float __cdecl FakeRadianAngle(float x1,float y1,float x2,float y2) { const float r=x2*0.75f-y2*0.5f+x1+y1; trace.push_back({"radian_angle_between_points",F(x1),F(y1),F(x2),F(y2),F(r)}); return r; }
inline float __cdecl FakeLimitRadian(float a) { const float r=a>3.0f ? a-6.0f : a<-3.0f ? a+6.0f : a; trace.push_back({"limit_radian_angle",F(a),F(r)}); return r; }

// --- hierba ---
inline void __fastcall FakeFlush(CPPTriPlantBuffer* b,void*) { trace.push_back({"tri_plant_flush",Name(b),b->m_CurrentIndex,b->m_PlantModelsSet}); b->m_CurrentIndex=0; }
inline RwFrame* __cdecl FakeFrameTranslate(RwFrame* f,const RwV3d* v,int32 combine) {
    trace.push_back({"frame_translate",Name(f),Vec(v),combine});
    auto* m=reinterpret_cast<uint8*>(f)+0x10; static const float id[12]{1.f,0.f,0.f,0.f,0.f,1.f,0.f,0.f,0.f,0.f,1.f,0.f};
    for(size_t k=0;k<12;++k)if(k%4!=3)At<float>(m,4*k)=id[k];
    At<float>(m,0x30)=v->x; At<float>(m,0x34)=v->y; At<float>(m,0x38)=v->z; return f;
}
inline RwMatrix* __cdecl FakeMatrixUpdate(RwMatrix* m) { json w=json::array(); for(size_t k=0;k<16;++k)if(k%4!=3)w.push_back(F(At<float>(m,4*k))); trace.push_back({"matrix_update",Name(m),w}); return m; }
inline RpGeometry* __cdecl FakeForAllMaterials(RpGeometry* g,void* cb,void* data) {
    trace.push_back({"geometry_for_all_materials",Name(g),Name(*reinterpret_cast<void**>(G_TEX)),*static_cast<uint32*>(data)}); return g;
}
inline RpAtomic* __cdecl FakeAtomicRender(RpAtomic* a) { trace.push_back({"atomic_render",Name(a)}); return a; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=36;
inline const std::array<Fn,NFNS> fns{{
    {"CEventGroup","Add-",0x4AB420,0,-1,1024},
    {"CEventGroup","Constructor",0x4AB340,0,-1,128},
    {"CEventGroup","Flush",0x4AB370,0,-1,512},
    {"CEventGroup","GetEventOfType",0x4AB650,0,-1,512},
    {"CEventGroup","GetHighestPriorityEvent",0x4AB7C0,0,-1,1024},
    {"CEventGroup","HasEvent",0x4AB6A0,0,-1,256},
    {"CEventGroup","HasEventOfType",0x4AB5E0,0,-1,512},
    {"CEventGroup","HasScriptCommandOfTaskType",0x4AB840,0,-1,512},
    {"CEventGroup","Remove",0x4AB5A0,0,-1,256},
    {"CEventGroup","RemoveInvalidEvents",0x4AB760,0,-1,512},
    {"CEventGroup","Reorganise",0x4AB700,0,-1,256},
    {"CEventGroup","TickEvents",0x4AB6D0,0,-1,128},
    {"CTaskComplexWalkRoundBuildingAttempt","Clone",0,0x86F334,1,128},
    {"CTaskComplexWalkRoundBuildingAttempt","ComputeCrapRoute",0x656F80,0x86F334,-1,1024},
    {"CTaskComplexWalkRoundBuildingAttempt","ComputeMoveDir",0x654B80,0x86F334,-1,1024},
    {"CTaskComplexWalkRoundBuildingAttempt","ComputeRoute",0x657BA0,0x86F334,-1,1024},
    {"CTaskComplexWalkRoundBuildingAttempt","Constructor",0x654740,0x86F334,-1,256},
    {"CTaskComplexWalkRoundBuildingAttempt","ControlSubTask",0,0x86F334,10,1024},
    {"CTaskComplexWalkRoundBuildingAttempt","CreateFirstSubTask",0,0x86F334,9,512},
    {"CTaskComplexWalkRoundBuildingAttempt","CreateNextSubTask",0,0x86F334,8,512},
    {"CTaskComplexWalkRoundBuildingAttempt","CreateSubTask",0x654C80,0x86F334,-1,512},
    {"CTaskComplexWalkRoundBuildingAttempt","Destructor",0x654950,0x86F334,-1,256},
    {"CTaskComplexWalkRoundBuildingAttempt","GetTaskType",0,0x86F334,4,64},
    {"CTaskComplexWalkRoundBuildingAttempt","MakeAbortable",0,0x86F334,6,256},
    {"CGrassRenderer","AddTriPlant",0x5DB1D0,0,-1,512},
    {"CGrassRenderer","DrawTriPlants",0x5DAD00,0,-1,1024},
    {"CGrassRenderer","FlushTriPlantBuffer",0x5DB250,0,-1,64},
    {"CGrassRenderer","GenPointInTriangle",0x5DAB00,0,-1,1024},
    {"CGrassRenderer","GetPlantModelsTab",0x5DACE0,0,-1,128},
    {"CGrassRenderer","Initialise",0x5DD6B0,0,-1,64},
    {"CGrassRenderer","SetCloseFarAlphaDist",0x5DABE0,0,-1,64},
    {"CGrassRenderer","SetCurrentScanCode",0x5DABB0,0,-1,64},
    {"CGrassRenderer","SetGlobalCameraPos",0x5DABC0,0,-1,64},
    {"CGrassRenderer","SetGlobalWindBending",0x5DAC00,0,-1,64},
    {"CGrassRenderer","SetPlantModelsTab",0x5DACC0,0,-1,128},
    {"CGrassRenderer","Shutdown",0x5DABA0,0,-1,64},
}};

inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(pedA.data()); clean(ent.data());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"mix12-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"mix12-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("mix12");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"mix12-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    named.clear();
    named.push_back({pedA.data(),pedA.size(),"pedA"}); named.push_back({ent.data(),ent.size(),"entity"});
    named.push_back({matA.data(),matA.size(),"matA"}); named.push_back({entMat.data(),entMat.size(),"entity_mat"});
    named.push_back({intel.data(),intel.size(),"intel"});
    for(size_t k=0;k<NEV;++k)named.push_back({evs[k].data(),EV_SIZE,"ev"+std::to_string(k)});
    for(size_t k=0;k<NETASK;++k)named.push_back({etasks[k].data(),etasks[k].size(),"script_task"+std::to_string(k)});
    named.push_back({group.data(),group.size(),"group"}); named.push_back({object.data(),object.size(),"object"}); named.push_back({sub.data(),sub.size(),"subtask"});
    named.push_back({routes.data(),routes.size(),"route"}); named.push_back({vecs.data(),sizeof(vecs),"vecs"});
    for(size_t k=0;k<NPLANTS;++k)named.push_back({plants[k].data(),plants[k].size(),"plant"+std::to_string(k)});
    for(size_t k=0;k<4;++k){ named.push_back({atomics[k].data(),atomics[k].size(),"atomic"+std::to_string(k)}); named.push_back({geoms[k].data(),geoms[k].size(),"geometry"+std::to_string(k)});
                             named.push_back({frames[k].data(),frames[k].size(),"frame"+std::to_string(k)}); }
    named.push_back({texs.data(),texs.size(),"texture"}); named.push_back({tabTargets.data(),tabTargets.size(),"models_tab"}); named.push_back({modelTab.data(),sizeof(modelTab),"model_tab"});
    named.push_back({reinterpret_cast<void*>(G_TRIBUF),TRIBUF_SIZE,"tri_plant_buf"});
    named.push_back({evVtbl.data(),sizeof(evVtbl),"event_vtbl"}); named.push_back({etVtbl.data(),sizeof(etVtbl),"script_task_vtbl"}); named.push_back({subVtbl.data(),sizeof(subVtbl),"sub_vtbl"});
    FillEvTraps(std::make_integer_sequence<int,20>{});
    evVtbl[0]=reinterpret_cast<void*>(&EvDtor); evVtbl[1]=reinterpret_cast<void*>(&EvType); evVtbl[2]=reinterpret_cast<void*>(&EvPrio); evVtbl[4]=reinterpret_cast<void*>(&EvClone);
    evVtbl[5]=reinterpret_cast<void*>(&EvAffects); evVtbl[8]=reinterpret_cast<void*>(&EvReport); evVtbl[9]=reinterpret_cast<void*>(&EvEditable); evVtbl[14]=reinterpret_cast<void*>(&EvIsValid);
    FillEtTraps(std::make_integer_sequence<int,20>{}); etVtbl[4]=reinterpret_cast<void*>(&EtType);
    FillSubTraps(std::make_integer_sequence<int,20>{}); subVtbl[0]=reinterpret_cast<void*>(&SubDtor); subVtbl[4]=reinterpret_cast<void*>(&SubType); subVtbl[6]=reinterpret_cast<void*>(&SubAbort);

    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda y se restaura en cada ejecucion y al final
    const std::vector<std::pair<uintptr_t,size_t>> regions{{G_WIND,8},{G_CAM,12},{G_FAR,4},{G_SCAN,2},{G_TEX,4},{G_TRIBUF,TRIBUF_SIZE},{uintptr_t(&g_fx.m_FxQuality),sizeof(g_fx.m_FxQuality)}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    auto& lfRef=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); const int32 lf0=lfRef;
    auto fillFreeTasks=[]{ auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                           for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch r2(bit_cast<uintptr_t>(&srand),FakeSrand), r2e(0x821B11,FakeSrand);
        // CEventGroup
        EntryPatch e1(bit_cast<uintptr_t>(&CEventEditableResponse::ComputeResponseTaskOfType),FakeResponseOfType), e1e(0x4B5730,FakeResponseOfType);
        EntryPatch e2(bit_cast<uintptr_t>(static_cast<void(CEventEditableResponse::*)(CPed*,bool)>(&CEventEditableResponse::ComputeResponseTaskType)),FakeResponseType), e2e(0x4B56C0,FakeResponseType);
        EntryPatch e3(bit_cast<uintptr_t>(&CEventEditableResponse::WillRespond),FakeWillRespond), e3e(0x4AC490,FakeWillRespond);
        EntryPatch e4(bit_cast<uintptr_t>(&CEventEditableResponse::InformGroup),FakeInformGroup), e4e(0x4B7DF0,FakeInformGroup);
        EntryPatch e5(bit_cast<uintptr_t>(&CEventEditableResponse::InformRespectedFriends),FakeInformFriends), e5e(0x4B2B00,FakeInformFriends);
        EntryPatch e6(bit_cast<uintptr_t>(&CEventEditableResponse::TriggerLookAt),FakeLookAt), e6e(0x4AC5A0,FakeLookAt);
        EntryPatch e7(bit_cast<uintptr_t>(&CEventEditableResponse::InformVehicleOccupants),FakeInformOccupants), e7e(0x4AC4A0,FakeInformOccupants);
        EntryPatch e8(bit_cast<uintptr_t>(&CPedIntelligence::RecordEventForScript),FakeRecordEvent), e8e(0x602050,FakeRecordEvent);
        // rodear edificio
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CPointRoute::operator new)),FakeRouteNew), w1e(0x41B7C0,FakeRouteNew);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<void(*)(void*,size_t)>(&CPointRoute::operator delete)),FakeRouteDelete), w2e(0x41B7D0,FakeRouteDelete);
        EntryPatch w3(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLosClear), w3e(0x56A490,FakeLosClear);
        EntryPatch w4(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeProcessLos), w4e(0x56BA00,FakeProcessLos);
        EntryPatch w5(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),FakeRadianAngle), w5e(0x53CBE0,FakeRadianAngle);
        EntryPatch w6(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),FakeLimitRadian), w6e(0x53CB50,FakeLimitRadian);
        // hierba
        EntryPatch g1(bit_cast<uintptr_t>(&CPPTriPlantBuffer::Flush),FakeFlush), g1e(0x5DB0C0,FakeFlush);
        EntryPatch g2(0x7F0E30,FakeFrameTranslate), g3(0x7F18A0,FakeMatrixUpdate), g4(0x74C790,FakeForAllMaterials); // la DLL los reenvia al EXE

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool isEG=cls=="CEventGroup", isWR=cls=="CTaskComplexWalkRoundBuildingAttempt", isGR=cls=="CGrassRenderer";
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto pick=[&](std::initializer_list<float> l){ return l.begin()[next()%l.size()]; };
                auto run=[&](bool candidate)->json {
                    { lfRef=lf0; fillFreeTasks(); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x12C3u; if(!st)st=1; trace=json::array(); freshIds.clear();
                    CleanRefs(); restoreGlobals();
                    rngState=next(); maskA=next(); maskB=next(); maskC=next(); callsA=callsB=callsC=0; routeUsed=0; hitCalls=0;
                    std::memset(routes.data(),0xCD,routes.size());
                    for(auto& v:vecs)v=uni(-30.f,30.f);
                    // --- peaton falso ---
                    std::memset(pedA.data(),0,pedA.size()); std::memset(matA.data(),0,matA.size()); std::memset(intel.data(),0,intel.size());
                    auto* const pa=reinterpret_cast<CPed*>(pedA.data()); pa->SetType(ENTITY_TYPE_PED);
                    const bool intPos=next()%4==0;
                    const CVector pedPos=intPos ? CVector{float(int32(next()%11)-5),float(int32(next()%11)-5),pick({0.5f,1.f,1.5f})} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-3.f,10.f)};
                    { const bool needMat=nm=="ComputeMoveDir"||nm=="ComputeRoute"||nm=="ControlSubTask";
                      CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pedPos;
                      if(needMat||next()%6){ std::memcpy(matA.data(),&m,sizeof(CMatrix)); At<void*>(pa,E_MATRIX)=matA.data(); }
                      else { At<float>(pa,E_POS)=pedPos.x; At<float>(pa,E_POS+4)=pedPos.y; At<float>(pa,E_POS+8)=pedPos.z; } }
                    At<uint32>(pa,P_FLAGS4)=next(); At<void*>(pa,P_INTEL)=intel.data();
                    At<int32>(intel.data(),I_COUNTER)=int32(pick({29.f,30.f,31.f,0.f,100.f}));
                    // --- entidad falsa (siempre con matriz) ---
                    std::memset(ent.data(),0,ent.size()); std::memset(entMat.data(),0,entMat.size());
                    reinterpret_cast<CEntity*>(ent.data())->SetType(ENTITY_TYPE_BUILDING);
                    const bool entIdentity=next()%3==0;
                    { CMatrix m{}; if(entIdentity){ m.SetRotate(0.f,0.f,0.f); m.GetPosition()=CVector{float(int32(next()%21)-10),float(int32(next()%21)-10),float(int32(next()%5))}; }
                      else { m.SetRotate(uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-3.f,10.f)}; }
                      std::memcpy(entMat.data(),&m,sizeof(CMatrix)); At<void*>(ent.data(),E_MATRIX)=entMat.data(); }
                    json out={{"switches",false}}; json ret=nullptr;
                    const auto occ=Occupancy();
                    void* const o=object.data();
                    void* const g=group.data();

                    // --- fixtures por clase ---
                    if(isEG) {
                        static const int32 types[]{9,9,32,32,1,5,60,33};
                        static const int32 taskTypes[]{243,243,200,400,509};
                        for(size_t k=0;k<NETASK;++k){ auto* t=etasks[k].data(); std::memset(t,0,etasks[k].size()); At<void*>(t,0)=etVtbl.data(); At<int32>(t,0x2C)=taskTypes[next()%5]; }
                        for(size_t k=0;k<NEV;++k){
                            auto* e=evs[k].data(); std::memset(e,0,EV_SIZE); At<void*>(e,0)=evVtbl.data();
                            At<int32>(e,4)=int32(next()%200)-20; At<uint8>(e,8)=uint8(next()%2); At<uint8>(e,0xC)=uint8(next()%2);
                            At<void*>(e,0x10)=next()%4 ? static_cast<void*>(etasks[next()%NETASK].data()) : nullptr;
                            At<uint8>(e,0x25)=uint8(next());
                            At<int32>(e,0x30)=types[next()%8]; At<int32>(e,0x34)=next()%3 ? int32(pick({0.f,10.f,10.f,50.f,60.f,70.f,75.f,-1.f})) : int32(next()%100);
                            At<uint8>(e,0x38)=uint8(next()%4!=0); At<uint8>(e,0x39)=uint8(next()%2); At<uint8>(e,0x3A)=uint8(next()%3!=0); At<uint8>(e,0x3B)=uint8(next());
                            At<void*>(e,0x3C)=evs[16+next()%2].data();
                        }
                        // nulos solo donde el EXE los admite
                        const bool keepFlag=next()%2!=0;
                        const bool allowNull=nm=="Remove"||nm=="HasEvent"||nm=="RemoveInvalidEvents"||nm=="Reorganise"||nm=="HasScriptCommandOfTaskType"||(nm=="Flush"&&!keepFlag);
                        std::memset(g,0xCD,group.size()); At<uintptr_t>(g,0)=EVG_VT; At<void*>(g,4)=next()%5 ? static_cast<void*>(pa) : nullptr;
                        const int32 count=nm=="Add-" ? (next()%4==0 ? 16 : int32(next()%17)) : (next()%8==0 ? 0 : int32(next()%17));
                        At<int32>(g,8)=count;
                        for(int32 k=0;k<16;++k)At<void*>(g,0xC+4*k)=(k<count&&allowNull&&next()%4==0) ? nullptr : (k<count||next()%2) ? static_cast<void*>(evs[k].data()) : nullptr;
                        void* const argEv=evs[18].data();
                        void* inGroup=count ? At<void*>(g,0xC+4*int32(next()%uint32(count))) : nullptr;
                        if(nm=="Constructor"){ std::memset(g,0xCD,group.size()); void* const ped=next()%4 ? static_cast<void*>(pa) : nullptr;
                            setState(candidate); out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(fn.address)(g,ped)); setState(false); }
                        else {
                            setState(candidate); out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                            const uintptr_t a=fn.address;
                            if(nm=="Add-"){ const uint32 valid=next()%2; ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*,uint32)>(a)(g,next()%5 ? argEv : inGroup ? inGroup : argEv,valid)); }
                            else if(nm=="Flush")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(g,keepFlag);
                            else if(nm=="GetEventOfType")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(g,next()%3 ? At<int32>(argEv,0x30) : types[next()%8]));
                            else if(nm=="GetHighestPriorityEvent")ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(a)(g));
                            else if(nm=="HasEvent"){ void* arg=next()%3 ? inGroup : next()%2 ? argEv : nullptr; ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(a)(g,arg)!=0; }
                            else if(nm=="HasEventOfType")ret=reinterpret_cast<uint8(__thiscall*)(void*,void*)>(a)(g,next()%8 ? argEv : nullptr)!=0;
                            else if(nm=="HasScriptCommandOfTaskType")ret=reinterpret_cast<uint8(__thiscall*)(void*,int32)>(a)(g,int32(pick({243.f,200.f,400.f,509.f,0.f})))!=0;
                            else if(nm=="Remove"){ void* arg=next()%3 ? inGroup : next()%2 ? argEv : nullptr; reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(g,arg); }
                            else if(nm=="RemoveInvalidEvents")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(g,keepFlag);
                            else if(nm=="Reorganise")reinterpret_cast<void(__thiscall*)(void*)>(a)(g);
                            else if(nm=="TickEvents")reinterpret_cast<void(__thiscall*)(void*)>(a)(g);
                            setState(false);
                        }
                        out["group"]=Region(g,nm=="Constructor" ? 4 : 0,group.size());
                        { json a=json::array(); for(auto& e:evs)a.push_back(Region(e.data(),4,0x40)); out["events"]=a; }
                        { json a=json::array(); for(auto& t:etasks)a.push_back(Region(t.data(),4,t.size())); out["script_tasks"]=a; }
                    }
                    if(isWR) {
                        // rutas existentes en los huecos 6 y 7 de la arena
                        auto* const rt=routes.data()+ROUTE*6; auto* const crt=routes.data()+ROUTE*7;
                        for(auto* r:{rt,crt}){ At<uint32>(r,0)=next()%9; for(size_t k=0;k<24;++k)At<float>(r,4+4*k)=uni(-20.f,20.f); }
                        // subtarea falsa
                        std::memset(sub.data(),0,sub.size()); At<void*>(sub.data(),0)=subVtbl.data();
                        { static const int32 types[]{411,421,422,900,902,905,203,1302,400}; At<int32>(sub.data(),0x2C)=types[next()%9]; }
                        // normal 2D (a veces alineada con un eje: camino casi nulo de ComputeMoveDir)
                        CVector normal; const bool axis=next()%4==0;
                        if(axis){ const uint32 m=next()%4; normal=CVector{m==0 ? 1.f : m==1 ? -1.f : 0.f,m==2 ? 1.f : m==3 ? -1.f : 0.f,0.f}; }
                        else { const float a=uni(-3.2f,3.2f); normal=CVector{std::cos(a),std::sin(a),0.f}; }
                        std::memset(o,0xCD,object.size());
                        At<uintptr_t>(o,0)=WRBA_VT; At<void*>(o,4)=nullptr; At<void*>(o,8)=sub.data();
                        At<void*>(o,0xC)=rt; At<void*>(o,0x10)=crt;
                        CVector target;
                        if(axis&&next()%2){ const float k=pick({0.f,2.f,-3.f,uni(-5.f,5.f)}); target=CVector{pedPos.x+normal.x*k,pedPos.y+normal.y*k,uni(-3.f,10.f)}; }
                        else target=CVector{pedPos.x+uni(-15.f,15.f),pedPos.y+uni(-15.f,15.f),pedPos.z+uni(-2.f,2.f)};
                        At<CVector>(o,0x14)=target;
                        At<CVector>(o,0x20)=CVector{pedPos.x+uni(-2.f,2.f),pedPos.y+uni(-2.f,2.f),pedPos.z+uni(-1.f,1.f)};
                        At<CVector>(o,0x2C)=normal;
                        At<void*>(o,0x38)=next()%5 ? static_cast<void*>(ent.data()) : nullptr;
                        CVector offset=entIdentity&&next()%2 ? CVector{float(int32(next()%7)-3),float(int32(next()%7)-3),float(int32(next()%3))} : CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-1.f,2.f)};
                        At<CVector>(o,0x3C)=offset;
                        At<int8>(o,0x48)=int8(next()%8==0 ? int32(pick({-1.f,8.f,0.f})) : int32(next()%8));
                        At<uint8>(o,0x49)=uint8(next());
                        At<int8>(o,0x4A)=int8(next()%3 ? int32(pick({0.f,1.f,1.f,2.f,18.f,19.f,20.f,25.f})) : int32(pick({127.f,-1.f,-128.f,uni(0.f,30.f)})));
                        // destino a distancia exacta de la entidad (|d|² == 16) para ControlSubTask
                        if(entIdentity&&next()%3==0){ const auto& m=*reinterpret_cast<CMatrix*>(entMat.data()); const CVector p{m.GetPosition().x+offset.x,m.GetPosition().y+offset.y,m.GetPosition().z+offset.z};
                            const uint32 ax=next()%3; CVector t=p; (&t.x)[ax]-=next()%2 ? 4.f : -4.f; if(next()%3==0)(&t.x)[ax]=std::nextafter((&t.x)[ax],next()%2 ? 1e9f : -1e9f);
                            At<CVector>(o,0x14)=t; }
                        // ProcessLineOfSight: punto de choque
                        for(auto& h:hitFrac)h=pick({0.f,0.1f,0.5f,0.9f,1.f,uni(0.f,1.f)});
                        hitExact=intPos&&next()%3==0; hitEntity=next()%2 ? static_cast<void*>(ent.data()) : nullptr;
                        const int prio=int(next()%3); void* const ev=next()%3 ? static_cast<void*>(evs[0].data()) : nullptr;
                        if(nm=="Destructor"){
                            if(next()%5==0)At<void*>(o,0xC)=nullptr; if(next()%5==0)At<void*>(o,0x10)=nullptr; if(next()%4==0)At<void*>(o,8)=nullptr;
                            if(At<void*>(o,0x38))static_cast<CEntity*>(At<void*>(o,0x38))->RegisterReference(reinterpret_cast<CEntity**>(static_cast<uint8*>(o)+0x38));
                        }
                        setState(candidate);
                        const uintptr_t target2=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                        out["switches"]=fn.slot>=0 ? ((target2>=0x400000&&target2<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                        uintptr_t rawRet=0; bool hasRaw=false;
                        if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                            if(next()%4==0){ vecs[8]=0.f; vecs[7]=0.f; vecs[6]=0.f; } else if(next()%3==0)vecs[8]=uni(-1.f,1.f);
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,const float*,const float*,const float*,uint32)>(target2)(o,int32(next()%8),vecs.data(),vecs.data()+3,vecs.data()+6,next()%2)); hasRaw=true; }
                        else if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(target2)(o);
                        else if(nm=="Clone"){ rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target2)(o)); hasRaw=true; }
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target2)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<uint8(__thiscall*)(void*,CPed*,int32,const void*)>(target2)(o,pa,prio,ev)!=0;
                        else if(nm=="CreateSubTask"){ static const int32 types[]{902,905,900,203,411,421,422,1302,0,500};
                            rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,int32,CPed*)>(target2)(o,types[next()%10],pa)); hasRaw=true; }
                        else if(nm=="ComputeMoveDir"){ float* outDir=vecs.data()+9; reinterpret_cast<void(__thiscall*)(void*,CPed*,float*)>(target2)(o,pa,outDir); }
                        else if(nm=="ComputeCrapRoute"||nm=="ComputeRoute")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target2)(o,pa);
                        else { rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*,CPed*)>(target2)(o,pa)); hasRaw=true; }
                        setState(false);
                        // tareas nuevas del pool
                        std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        if(hasRaw)ret=Name(rawRet);
                        json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,4,0x80)}});
                        const bool skipVptr=nm=="Constructor"||nm=="Destructor";
                        json objectWords=Region(o,skipVptr ? 4 : 0,object.size());
                        if(nm=="Constructor")objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo en vez del vptr
                        out["object"]=objectWords; out["tasks"]=tasks; out["routes"]=Region(routes.data(),0,routes.size());
                        out["vecs"]=Region(vecs.data(),0,sizeof(vecs)); out["sub"]=Region(sub.data(),4,sub.size());
                        // borrado de las tareas creadas (su traza va aparte)
                        const size_t traceBefore=trace.size();
                        for(auto* t:fresh)delete t;
                        json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                        trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                        out["delete_trace"]=delTrace; out["routes_after_delete"]=Region(routes.data(),0,routes.size());
                    }
                    if(isGR) {
                        auto* const buf=reinterpret_cast<CPPTriPlantBuffer*>(G_TRIBUF);
                        // plantas, atomicos, frames y geometrias falsos
                        const CVector cam{uni(-100.f,100.f),uni(-100.f,100.f),uni(-5.f,30.f)};
                        const float farD=pick({10.f,20.f,uni(5.f,60.f)}), closeD=pick({0.f,2.f,uni(0.f,30.f)});
                        for(size_t k=0;k<NPLANTS;++k){
                            auto* p=reinterpret_cast<PPTriPlant*>(plants[k].data()); for(auto& b:plants[k])b=uint8(next());
                            const float a=uni(-3.2f,3.2f), d=next()%4==0 ? pick({farD,farD+20.f,0.f}) : uni(0.f,farD+30.f);
                            p->Center=CVector{cam.x+std::cos(a)*d,cam.y+std::sin(a)*d,cam.z+uni(-2.f,2.f)};
                            const float spread=pick({0.5f,3.f,uni(0.f,10.f)});
                            for(CVector* v:{&p->V1,&p->V2,&p->V3})*v=CVector{p->Center.x+uni(-spread,spread),p->Center.y+uni(-spread,spread),p->Center.z+uni(-1.f,1.f)};
                            p->model_id=uint16(next()%4); p->num_plants=uint16(next()%6);
                            p->scale=CVector2D{uni(0.5f,2.f),uni(0.5f,2.f)}; p->texture=reinterpret_cast<RwTexture*>(texs.data()+4*(next()%8));
                            p->seed=0.f; // FIX_BUGS: srand(bits de la semilla) == srand((int)semilla) solo con 0.0f
                            p->scale_var_xy=uni(0.f,1.f); p->scale_var_z=uni(0.f,1.f); p->wind_bend_scale=uni(-1.f,1.f); p->wind_bend_var=uni(-1.f,1.f);
                        }
                        for(size_t k=0;k<4;++k){
                            std::memset(atomics[k].data(),0,atomics[k].size()); std::memset(geoms[k].data(),0,geoms[k].size()); std::memset(frames[k].data(),0,frames[k].size());
                            At<void*>(atomics[k].data(),4)=frames[k].data(); At<void*>(atomics[k].data(),0x18)=geoms[k].data(); At<void*>(atomics[k].data(),0x48)=reinterpret_cast<void*>(&FakeAtomicRender);
                            auto* m=frames[k].data()+0x10; for(size_t w=0;w<16;++w)At<float>(m,4*w)=uni(-2.f,2.f);
                            modelTab[k]=atomics[k].data();
                        }
                        *reinterpret_cast<float*>(G_WIND)=uni(-1.f,1.f); *reinterpret_cast<float*>(G_CLOSE)=closeD; *reinterpret_cast<float*>(G_FAR)=farD;
                        *reinterpret_cast<CVector*>(G_CAM)=cam; *reinterpret_cast<void**>(G_TEX)=nullptr;
                        g_fx.m_FxQuality=FxQuality_e(next()%4);
                        // buffer de plantas: indice y juego actuales
                        for(size_t k=0;k<sizeof(buf->m_Buffer);++k)reinterpret_cast<uint8*>(buf->m_Buffer.data())[k]=uint8(next());
                        buf->m_CurrentIndex=next()%3 ? int32(pick({0.f,1.f,15.f,30.f,31.f,32.f})) : int32(next()%33);
                        buf->m_PlantModelsSet=int32(next()%4);
                        for(size_t k=0;k<4;++k)buf->m_pPlantModelsTab[k]=next()%4 ? reinterpret_cast<RpAtomic**>(tabTargets.data()+4*k) : nullptr;
                        setState(candidate); out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        const uintptr_t a=fn.address;
                        if(nm=="Initialise")ret=reinterpret_cast<uint8(__cdecl*)()>(a)()!=0;
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="SetCurrentScanCode")reinterpret_cast<void(__cdecl*)(uint32)>(a)(next());
                        else if(nm=="SetGlobalCameraPos")reinterpret_cast<void(__cdecl*)(const float*)>(a)(vecs.data());
                        else if(nm=="SetCloseFarAlphaDist")reinterpret_cast<void(__cdecl*)(float,float)>(a)(uni(0.f,50.f),uni(0.f,100.f));
                        else if(nm=="SetGlobalWindBending")reinterpret_cast<void(__cdecl*)(float)>(a)(uni(-2.f,2.f));
                        else if(nm=="GetPlantModelsTab")ret=Name(reinterpret_cast<void*(__cdecl*)(uint32)>(a)(next()%4==0 ? uint32(pick({4.f,5.f,100.f})) : next()%8==0 ? 0xFFFFFFFFu : next()%4));
                        else if(nm=="SetPlantModelsTab"){ const uint32 t=next()%4==0 ? uint32(pick({4.f,5.f,100.f})) : next()%8==0 ? 0xFFFFFFFFu : next()%4;
                            ret=reinterpret_cast<uint8(__cdecl*)(uint32,void*)>(a)(t,next()%4 ? static_cast<void*>(tabTargets.data()+0x20+4*(next()%8)) : nullptr)!=0; }
                        else if(nm=="GenPointInTriangle"){
                            float sa=uni(0.f,1.f), sb=uni(0.f,1.f);
                            switch(next()%6){ case 0: sb=1.f-sa; break; case 1: sb=std::nextafter(1.f-sa,2.f); break; case 2: sa=pick({0.f,1.f,0.5f}); sb=pick({0.f,1.f,0.5f}); break; default: break; }
                            reinterpret_cast<void(__cdecl*)(float*,const float*,const float*,const float*,float,float)>(a)(vecs.data()+9,vecs.data(),vecs.data()+3,vecs.data()+6,sa,sb); }
                        else if(nm=="AddTriPlant")reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(plants[next()%NPLANTS].data(),next()%5==0 ? uint32(next()%4) : uint32(buf->m_PlantModelsSet));
                        else if(nm=="FlushTriPlantBuffer")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="DrawTriPlants"){ const int32 n=next()%8==0 ? int32(pick({0.f,-1.f})) : int32(1+next()%NPLANTS);
                            reinterpret_cast<void(__cdecl*)(void*,int32,void*)>(a)(plants[0].data(),n,modelTab.data()); }
                        setState(false);
                        out["globals"]={F(*reinterpret_cast<float*>(G_WIND)),F(*reinterpret_cast<float*>(G_CLOSE)),F(*reinterpret_cast<float*>(G_FAR)),Vec(reinterpret_cast<void*>(G_CAM)),
                                        *reinterpret_cast<uint16*>(G_SCAN),Name(*reinterpret_cast<void**>(G_TEX)),int(g_fx.m_FxQuality)};
                        out["tri_buf"]={buf->m_CurrentIndex,buf->m_PlantModelsSet,Hash(buf->m_Buffer.data(),sizeof(buf->m_Buffer)),Region(buf->m_pPlantModelsTab.data(),0,sizeof(buf->m_pPlantModelsTab))};
                        { json fr=json::array(); for(auto& b:frames)fr.push_back(Region(b.data(),0,b.size())); out["frames"]=fr; }
                        out["vecs"]=Region(vecs.data(),0,sizeof(vecs));
                        { json pl=json::array(); for(auto& p:plants)pl.push_back(Hash(p.data(),p.size())); out["plants"]=pl; }
                    }
                    json refs=json::array(); auto refList=[&](void* e){json l=json::array();for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(void* e:{static_cast<void*>(pedA.data()),static_cast<void*>(ent.data())})refs.push_back(refList(e));
                    out["ret"]=ret; out["refs"]=refs; out["pedA"]=EntHash(pedA); out["entity"]=EntHash(ent);
                    out["ped_fields"]={At<uint32>(pa,P_FLAGS4),Region(matA.data(),0,0x40),Region(intel.data(),0,intel.size())};
                    out["entity_mat"]=Region(entMat.data(),0,0x40);
                    const bool clean=Occupancy()==occ;
                    restoreGlobals();
                    out["trace"]=trace; out["clean"]=clean;
                    freshIds.clear();
                    CleanRefs();
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
    CleanRefs();
    const bool globalsTouched=!globalsIntact();
    restoreGlobals(); lfRef=lf0; fillFreeTasks();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},{"state_restored",restored}});
}
}
