#pragma once
// Banco dentro del motor: tareas simples C (lista de 6, 48 funciones): CTaskSimpleSetKindaStayInSamePlace,
// CTaskSimpleSetPedAsAutoDriver, CTaskSimpleThrowControl, CTaskSimpleTurn180, CTaskSimpleWaitForBus,
// CTaskSimpleWaitUntilAreaCodesMatch, CTaskSimpleWaitUntilPedIsInCar y CTaskSimpleWaitUntilPedIsOutCar.
// Original (hooks del lote y constructores base de CTaskSimpleThrowProjectile/CTaskSimpleStandStill desactivados)
// frente a candidato con fixtures identicos: los constructores se llaman en la direccion del EXE sobre un buffer
// rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se llaman por la entrada de la vtable
// del EXE. Peatones (vtable falsa trazada), clump (byte de tipo 2), inteligencias (con escaner de vehiculos),
// asociacion de animacion, entidad, cuatro vehiculos (vtable falsa trazada), tarea secundaria (vtable trazada:
// GetTaskType, MakeAbortable), tarea de lanzamiento y evento falsos en buffers propios. Se simulan y trazan:
// CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation::SetFinishCallback,
// CPed::SetMoveState/RestoreHeadingRate/IsPlayer, CPedIntelligence::GetTaskDuck/GetTaskThrow,
// CTaskSimpleDuck::ControlDuckMove, CTaskManager::GetTaskSecondary/SetTaskSecondary (ChangeTaskInSlot en la DLL)/
// FindActiveTaskByType, CCarCtrl::JoinCarWithRoadSystem y CGeneral::GetRadianAngleBetweenPoints/LimitRadianAngle.
// Se fijan y restauran el reloj, ms_fTimeStep y CEntryExitManager::ms_exitEnterState. Las tareas nuevas del pool se
// siguen (m_LastFreeSlot restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleSetKindaStayInSamePlace.h"
#include "Tasks/TaskTypes/TaskSimpleSetPedAsAutoDriver.h"
#include "Tasks/TaskTypes/TaskSimpleThrowControl.h"
#include "Tasks/TaskTypes/TaskSimpleThrowProjectile.h"
#include "Tasks/TaskTypes/TaskSimpleTurn180.h"
#include "Tasks/TaskTypes/TaskSimpleWaitForBus.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilAreaCodesMatch.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilPedIsInCar.h"
#include "Tasks/TaskTypes/TaskSimpleWaitUntilPedIsOutCar.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "PedIntelligence.h"
#include "EntryExitManager.h"
#include "CarCtrl.h"
#include "General.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace STasks6cLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CEntity / CVehicle)
constexpr size_t P_FLAGS_ENT=0x1C, P_AREA=0x2F, P_CLUMP=0x18, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478,
                 P_INTEL=0x47C, P_PDATA=0x480, P_ANIM_GROUP=0x4D4, P_STATE=0x530, P_SWIM=0x538, P_AIM=0x55C, P_HEADRATE=0x560, P_TYPE=0x598,
                 V_STATUS=0x36, V_SPEED=0x44, V_MISSION=0x3BA, V_CRUISE=0x3D0, V_FLAGS=0x428, V_NUMPASS=0x484, V_MAXPASS=0x488,
                 I_VEHSCAN=0xE0;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_EXIT_ENTER=0x96A7CC,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_TURN180_CB=0x631D60;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{}, entMat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,0x300> intelA{}, intelB{};
alignas(16) inline std::array<uint8,0x80> clump{}, pdA{}, duck{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{}, anim2{};
alignas(16) inline std::array<std::array<uint8,0x600>,4> vehs{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x40> sec{}, thr{}, event{};
alignas(16) inline std::array<uint8,0x10> vec{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh(size_t k) { return reinterpret_cast<CEntity*>(vehs[k].data()); }
inline std::array<void*,64> pedVtbl{}, vehVtbl{};
inline std::array<void*,16> taskVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_TURN180_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleTurn180::FinishAnimTurn180CB))return "Turn180FinishCB";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(entMat.data(),entMat.size(),"entMat"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(intelB.data(),intelB.size(),"intelB"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(pdA.data(),pdA.size(),"pdA"))return *s;
    if(auto s=in(duck.data(),duck.size(),"duck"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    for(size_t k=0;k<vehs.size();++k)if(auto s=in(vehs[k].data(),vehs[k].size(),("veh"+std::to_string(k)).c_str()))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(sec.data(),sec.size(),"sec"))return *s; if(auto s=in(thr.data(),thr.size(),"thr"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(vec.data(),vec.size(),"vec"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(vehVtbl.data(),sizeof(vehVtbl),"vehvtbl"))return *s;
    if(auto s=in(taskVtbl.data(),sizeof(taskVtbl),"taskvtbl"))return *s;
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
inline uintptr_t blendRet{}, duckRet{}, secondaryRet{}, throwRet{}, findGoToRet{}, findGetInRet{};
inline uint32 assocMask{}; inline bool isPlayerRet{}, secAbortRet{};

// peaton y vehiculos: vtables falsas (todo trazado)
template<int N> void* __fastcall PTrap(void*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<int N> void* __fastcall VTrap(void*,void*) { trace.push_back({"veh_unexpected_slot",N}); return nullptr; }
template<int... N> void FillVTraps(std::integer_sequence<int,N...>) { ((vehVtbl[N]=reinterpret_cast<void*>(&VTrap<N>)),...); }
// tarea secundaria falsa (GetTaskType en el hueco 4, MakeAbortable en el 6; el tipo se guarda en +0x30)
inline int __fastcall TType(void* t,void*) { trace.push_back({"task_get_type",Name(uintptr_t(t))}); return At<int32>(t,0x30); }
inline bool __fastcall TAbort(void* t,void*,CPed* p,int prio,const void* ev) { trace.push_back({"task_make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(ev))}); return secAbortRet; }
template<int N> void* __fastcall TTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); return nullptr; }
template<int... N> void FillTTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }

// dependencias simuladas
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return (assocMask>>(id%32))&1 ? reinterpret_cast<CAnimBlendAssociation*>(anim.data()) : nullptr; }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
inline void __fastcall RestoreHeading(CPed* p,void*) { trace.push_back({"restore_heading_rate",Name(uintptr_t(p))}); }
inline bool __fastcall IsPlayer(const CPed* p,void*) { trace.push_back({"is_player",Name(uintptr_t(p))}); return isPlayerRet; }
inline void* __fastcall GetDuck(void* intel,void*,bool ignore) { trace.push_back({"get_task_duck",Name(uintptr_t(intel)),ignore}); return reinterpret_cast<void*>(duckRet); }
inline void* __fastcall GetThrow(void* intel,void*) { trace.push_back({"get_task_throw",Name(uintptr_t(intel))}); return reinterpret_cast<void*>(throwRet); }
inline void __fastcall DuckMove(void* d,void*,float x,float y) { trace.push_back({"control_duck_move",Name(uintptr_t(d)),F(x),F(y)}); }
inline CTask* __fastcall Secondary(void* tm,void*,int32 idx) { trace.push_back({"get_task_secondary",Name(uintptr_t(tm)),idx}); return reinterpret_cast<CTask*>(secondaryRet); }
// EXE: CTaskManager::SetTaskSecondary(task, idx) 0x681B60; DLL: SetTaskSecondary en linea -> ChangeTaskInSlot(m_aSecondaryTasks[idx], task)
inline void __fastcall SetSecondary(void* tm,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),idx}); }
inline void __fastcall ChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),int32((uintptr_t(slot)-uintptr_t(tm)-0x14)/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
inline CTask* __fastcall FindActive(void* tm,void*,int32 type) {
    trace.push_back({"find_active_task_by_type",Name(uintptr_t(tm)),type});
    return reinterpret_cast<CTask*>(type==TASK_COMPLEX_GO_TO_CAR_DOOR_AND_STAND_STILL ? findGoToRet : type==TASK_SIMPLE_CAR_GET_IN ? findGetInRet : 0);
}
inline void __cdecl JoinRoad(CVehicle* v) { trace.push_back({"join_car_with_road_system",Name(uintptr_t(v))}); }
inline float __cdecl RadAngle(float x1,float y1,float x2,float y2) { trace.push_back({"get_radian_angle_between_points",F(x1),F(y1),F(x2),F(y2)}); return x1*0.25f-y1; }
inline float __cdecl LimitRad(float a) { trace.push_back({"limit_radian_angle",F(a)}); return a+0.5f; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { KINDA, AUTODRV, THROWC, TURN, BUS, AREA, INCAR, OUTCAR };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,8> classes{{
    {"CTaskSimpleSetKindaStayInSamePlace",0x86DD74,0xC,KINDA}, {"CTaskSimpleSetPedAsAutoDriver",0x86EFF4,0xC,AUTODRV},
    {"CTaskSimpleThrowControl",0x86D7B4,0x1C,THROWC}, {"CTaskSimpleTurn180",0x86E01C,0x10,TURN},
    {"CTaskSimpleWaitForBus",0x86E1B8,0xC,BUS}, {"CTaskSimpleWaitUntilAreaCodesMatch",0x86E544,0x24,AREA},
    {"CTaskSimpleWaitUntilPedIsInCar",0x86F0B8,0x10,INCAR}, {"CTaskSimpleWaitUntilPedIsOutCar",0x86F094,0x1C,OUTCAR},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleSetKindaStayInSamePlace","Clone",0,0x86DD74,1,256},{"CTaskSimpleSetKindaStayInSamePlace","Constructor",0x62F610,0x86DD74,-1,128},{"CTaskSimpleSetKindaStayInSamePlace","Destructor",0x62F650,0x86DD74,-1,256},
    {"CTaskSimpleSetKindaStayInSamePlace","GetTaskType",0,0x86DD74,4,64},{"CTaskSimpleSetKindaStayInSamePlace","MakeAbortable",0,0x86DD74,6,128},{"CTaskSimpleSetKindaStayInSamePlace","ProcessPed",0,0x86DD74,7,256},
    {"CTaskSimpleSetPedAsAutoDriver","Clone",0,0x86EFF4,1,256},{"CTaskSimpleSetPedAsAutoDriver","Constructor",0x648F40,0x86EFF4,-1,128},{"CTaskSimpleSetPedAsAutoDriver","Destructor",0x648FC0,0x86EFF4,-1,256},
    {"CTaskSimpleSetPedAsAutoDriver","GetTaskType",0,0x86EFF4,4,64},{"CTaskSimpleSetPedAsAutoDriver","MakeAbortable",0,0x86EFF4,6,128},{"CTaskSimpleSetPedAsAutoDriver","ProcessPed",0,0x86EFF4,7,256},
    {"CTaskSimpleThrowControl","Clone",0,0x86D7B4,1,256},{"CTaskSimpleThrowControl","Constructor",0x61F8B0,0x86D7B4,-1,128},{"CTaskSimpleThrowControl","Destructor",0x61F950,0x86D7B4,-1,256},
    {"CTaskSimpleThrowControl","GetTaskType",0,0x86D7B4,4,64},{"CTaskSimpleThrowControl","MakeAbortable",0,0x86D7B4,6,512},{"CTaskSimpleThrowControl","ProcessPed",0,0x86D7B4,7,2048},
    {"CTaskSimpleTurn180","Clone",0,0x86E01C,1,256},{"CTaskSimpleTurn180","Constructor",0x631CC0,0x86E01C,-1,128},{"CTaskSimpleTurn180","Destructor",0x631D00,0x86E01C,-1,256},
    {"CTaskSimpleTurn180","GetTaskType",0,0x86E01C,4,64},{"CTaskSimpleTurn180","MakeAbortable",0,0x86E01C,6,128},{"CTaskSimpleTurn180","ProcessPed",0,0x86E01C,7,512},
    {"CTaskSimpleWaitForBus","Clone",0,0x86E1B8,1,256},{"CTaskSimpleWaitForBus","Constructor",0x632A50,0x86E1B8,-1,128},{"CTaskSimpleWaitForBus","Destructor",0x632A90,0x86E1B8,-1,256},
    {"CTaskSimpleWaitForBus","GetTaskType",0,0x86E1B8,4,64},{"CTaskSimpleWaitForBus","MakeAbortable",0,0x86E1B8,6,128},{"CTaskSimpleWaitForBus","ProcessPed",0,0x86E1B8,7,2048},
    {"CTaskSimpleWaitUntilAreaCodesMatch","Clone",0,0x86E544,1,256},{"CTaskSimpleWaitUntilAreaCodesMatch","Constructor",0x635540,0x86E544,-1,128},{"CTaskSimpleWaitUntilAreaCodesMatch","Destructor",0x6355E0,0x86E544,-1,256},
    {"CTaskSimpleWaitUntilAreaCodesMatch","GetTaskType",0,0x86E544,4,64},{"CTaskSimpleWaitUntilAreaCodesMatch","MakeAbortable",0,0x86E544,6,128},{"CTaskSimpleWaitUntilAreaCodesMatch","ProcessPed",0,0x86E544,7,1024},
    {"CTaskSimpleWaitUntilPedIsInCar","Clone",0,0x86F0B8,1,256},{"CTaskSimpleWaitUntilPedIsInCar","Constructor",0x649620,0x86F0B8,-1,128},{"CTaskSimpleWaitUntilPedIsInCar","Destructor",0x6496A0,0x86F0B8,-1,256},
    {"CTaskSimpleWaitUntilPedIsInCar","GetTaskType",0,0x86F0B8,4,64},{"CTaskSimpleWaitUntilPedIsInCar","MakeAbortable",0,0x86F0B8,6,128},{"CTaskSimpleWaitUntilPedIsInCar","ProcessPed",0,0x86F0B8,7,1024},
    {"CTaskSimpleWaitUntilPedIsOutCar","Clone",0,0x86F094,1,256},{"CTaskSimpleWaitUntilPedIsOutCar","Constructor",0x649450,0x86F094,-1,128},{"CTaskSimpleWaitUntilPedIsOutCar","Destructor",0x6494F0,0x86F094,-1,256},
    {"CTaskSimpleWaitUntilPedIsOutCar","GetTaskType",0,0x86F094,4,64},{"CTaskSimpleWaitUntilPedIsOutCar","MakeAbortable",0,0x86F094,6,128},{"CTaskSimpleWaitUntilPedIsOutCar","ProcessPed",0,0x86F094,7,1024},
}};
// constructores base que llaman los metodos del EXE: se desactivan en la ejecucion original para que sea EXE puro
inline const std::array<std::pair<const char*,const char*>,2> baseHooks{{
    {"CTaskSimpleThrowProjectile","Constructor"},{"CTaskSimpleStandStill","Constructor"},
}};
// desplazamiento del puntero a entidad registrado de cada clase (0 = ninguno)
inline size_t RefSlot(Kind k) { switch(k){ case AUTODRV: case INCAR: case OUTCAR: return 8; case THROWC: return 0xC; case AREA: return 0x20; default: return 0; } }

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent()); for(size_t k=0;k<vehs.size();++k)clean(Veh(k));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks6c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks6c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks6c");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks6c-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> extra;
    for(auto& [c,n]:baseHooks){ if(auto* h=FindHook(c,n))extra.push_back(h); else Write({{"event","note"},{"reason","base hook not found (se deja como esta)"},{"function",std::string(c)+"::"+n}}); }
    FillPTraps(std::make_integer_sequence<int,64>{}); FillVTraps(std::make_integer_sequence<int,64>{});
    FillTTraps(std::make_integer_sequence<int,16>{}); taskVtbl[4]=reinterpret_cast<void*>(&TType); taskVtbl[6]=reinterpret_cast<void*>(&TAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto& exitEnter=*reinterpret_cast<int32*>(ADDR_EXIT_ENTER); const int32 savedExitEnter=exitEnter;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; exitEnter=savedExitEnter; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::vector<bool> extraEnabled,extraLocked; for(auto* h:extra){extraEnabled.push_back(h->Hooked());extraLocked.push_back(h->Locked());h->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); for(auto* h:extra)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a2e(0x4D68B0,GetAssoc);
        EntryPatch a3(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a3e(0x4CEBE0,SetFinishCb);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::RestoreHeadingRate),RestoreHeading), p2e(0x5DFD60,RestoreHeading);
        EntryPatch p3(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer), p3e(0x5DF8F0,IsPlayer);
        EntryPatch i1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),GetDuck), i1e(0x6010A0,GetDuck);
        EntryPatch i2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskThrow),GetThrow), i2e(0x600FB0,GetThrow);
        EntryPatch d1(bit_cast<uintptr_t>(&CTaskSimpleDuck::ControlDuckMove),DuckMove), d1e(0x6923F0,DuckMove);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<CTask*(CTaskManager::*)(eSecondaryTask)>(&CTaskManager::GetTaskSecondary)),Secondary), t1e(0x681810,Secondary);
        EntryPatch t2(TaskManagerAccess::ChangeSlotAddress(),ChangeSlot), t2e(0x681B60,SetSecondary);
        EntryPatch t3(bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),FindActive), t3e(0x681740,FindActive);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarCtrl::JoinCarWithRoadSystem),JoinRoad), c1e(0x42F5A0,JoinRoad);
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),RadAngle), g1e(0x53CBE0,RadAngle);
        EntryPatch g2(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),LimitRad), g2e(0x53CB50,LimitRad);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; const auto& cl=ClassOf(fn.cls); int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6C6Cu; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    timeMs=next(); timeStep=next()%3 ? 1.f : uni(0.1f,3.f); exitEnter=next()%3 ? 0 : int32(next()%5);
                    // peatones (vtable falsa, clump con tipo 2, inteligencias con escaner de vehiculos, datos de jugador)
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size());
                    std::memset(entMat.data(),0,entMat.size()); std::memset(ent.data(),0,ent.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto* b:{&intelA,&intelB})for(auto& x:*b)x=uint8(next());
                    for(auto& x:duck)x=uint8(next());
                    std::memset(pdA.data(),0,pdA.size()); At<float>(pdA.data(),0x14)=uni(0.f,3.f);
                    // vehiculos falsos: solo los campos que leen/escriben las tareas
                    for(size_t k=0;k<vehs.size();++k){ auto* v=vehs[k].data(); std::memset(v,0,vehs[k].size());
                        *reinterpret_cast<void**>(v)=vehVtbl.data(); Veh(k)->SetType(ENTITY_TYPE_VEHICLE);
                        At<uint8>(v,V_STATUS)=uint8(next()); At<uint8>(v,V_MISSION)=uint8(next()); At<uint8>(v,V_CRUISE)=uint8(next());
                        for(int w=0;w<2;++w)At(v,V_FLAGS+4*w)=next();
                        static const float speeds[]{0.f,-0.f,0.005f,-0.005f,0.0049f,0.0051f,0.0035355339f,0.0035355341f,0.001f};
                        for(int c=0;c<3;++c)At<float>(v,V_SPEED+4*c)=next()%3 ? speeds[next()%9] : uni(-0.02f,0.02f);
                        if(next()%3==0){ At<float>(v,V_SPEED)=0.005f; At<float>(v,V_SPEED+4)=0.f; At<float>(v,V_SPEED+8)=0.f; } // en el umbral exacto
                        if(next()%40==0)At<float>(v,V_SPEED+4*(next()%3))=std::numeric_limits<float>::quiet_NaN();
                        At<uint8>(v,V_NUMPASS)=uint8(next()%9); At<uint8>(v,V_MAXPASS)=uint8(next()%10==0 ? 0 : next()%9);
                    }
                    for(auto* in:{&intelA,&intelB})for(int k=0;k<16;++k){ const uint32 m=next()%6; At<void*>(in->data(),I_VEHSCAN+4*k)=m<2 ? nullptr : static_cast<void*>(Veh(m-2)); }
                    Ent()->SetType(ENTITY_TYPE_OBJECT);
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At<int8>(p,P_AREA)=int8(next()%3 ? next()%3 : next()%18);
                        { static const uint32 states[]{0,1,0x27,0x36,0x37,12,40}; At(p,P_STATE)=next()%3 ? states[next()%7] : next()%64; }
                        At<int32>(p,P_ANIM_GROUP)=int32(next()%118); At(p,P_SWIM)=next()%8;
                        At<float>(p,P_AIM)=uni(-3.f,3.f); At<float>(p,P_HEADRATE)=uni(0.f,15.f);
                        { static const uint32 types[]{0,1,2,6,6,7,4}; At(p,P_TYPE)=types[next()%7]; }
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelB.data();
                    At<void*>(PedA(),P_PDATA)=next()%3 ? nullptr : pdA.data();
                    if(next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{};
                        m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    const CVector pedPos=PedA()->GetPosition();
                    if(next()%3==0){ Ent()->m_matrix=reinterpret_cast<CMatrixLink*>(entMat.data()); CMatrix m{};
                        m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{pedPos.x+uni(-10.f,10.f),pedPos.y+uni(-10.f,10.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(Ent()->m_matrix),&m,sizeof(CMatrix)); }
                    else if(next()%8==0) Ent()->SetPosn(pedPos.x,pedPos.y,pedPos.z+uni(-2.f,2.f)); // misma X/Y: atan2 de ceros con signo
                    else Ent()->SetPosn(pedPos.x+uni(-10.f,10.f),pedPos.y+uni(-10.f,10.f),uni(0.f,40.f));
                    // asociaciones de animacion falsas (floats de mezcla controlados, sin NaN de senalizacion)
                    for(auto* a:{&anim,&anim2}){ for(auto& b:*a)b=uint8(next());
                        static const float amounts[]{0.99f,1.f,0.99999994f,0.5f,0.f,-0.f,2.f,std::numeric_limits<float>::quiet_NaN()};
                        At<float>(a->data(),0x18)=amounts[next()%8]; At<float>(a->data(),0x1C)=uni(-4.f,4.f); }
                    // tarea secundaria y tarea de lanzamiento falsas, evento falso
                    for(auto& b:sec)b=uint8(next()); *reinterpret_cast<void**>(sec.data())=taskVtbl.data();
                    { static const int32 types[]{TASK_SIMPLE_THROW_PROJECTILE,TASK_SIMPLE_THROW_PROJECTILE,TASK_SIMPLE_USE_GUN,TASK_SIMPLE_DUCK}; At<int32>(sec.data(),0x30)=types[next()%4]; }
                    for(auto& b:thr)b=uint8(next()); At<uint8>(thr.data(),8)=uint8(next()%3==0);
                    for(int k=0;k<3;++k)At<float>(thr.data(),0x14+4*k)=uni(-50.f,50.f);
                    for(auto& b:event)b=uint8(next());
                    // dependencias simuladas
                    blendRet=next()%4 ? uintptr_t(anim2.data()) : 0; assocMask=next(); duckRet=next()%3 ? uintptr_t(duck.data()) : 0;
                    isPlayerRet=next()%3==0; secAbortRet=next()%2;
                    secondaryRet=next()%3 ? uintptr_t(sec.data()) : 0;
                    throwRet=next()%3 ? uintptr_t(thr.data()) : 0;
                    if(cl.kind==THROWC){
                        if(nm=="ProcessPed")throwRet=uintptr_t(thr.data());            // el EXE la usa sin comprobar
                        else if(throwRet)secondaryRet=uintptr_t(sec.data());          // MakeAbortable: con tarea de lanzamiento hay secundaria
                    }
                    findGoToRet=next()%3 ? 0 : uintptr_t(sec.data()); findGetInRet=next()%3 ? 0 : uintptr_t(sec.data());
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    auto pickPed=[&]()->CPed*{ const uint32 m=next()%5; return m==0 ? nullptr : m==1 ? PedA() : PedB(); };
                    for(int k=0;k<3;++k)At<float>(vec.data(),4*k)=uni(-30.f,30.f);
                    const bool ctorBool=next()%2; CPed* const ctorPed=pickPed();
                    CEntity* const ctorEnt=next()%4 ? Ent() : nullptr; void* const ctorVecPtr=next()%4 ? static_cast<void*>(vec.data()) : nullptr;
                    CEntity* const ctorVeh=next()%4 ? Veh(0) : nullptr;
                    switch(cl.kind){
                    case KINDA: At<uint8>(o,8)=uint8(next()%2); break;
                    case AUTODRV: At<CEntity*>(o,8)=(nm=="ProcessPed"||next()%4) ? Veh(0) : nullptr; break;   // ProcessPed la usa sin comprobar
                    case THROWC:
                        At<uint8>(o,8)=uint8(next()%5==0); At<uint8>(o,9)=uint8(next()%2);
                        At<CEntity*>(o,0xC)=next()%3 ? Ent() : nullptr;
                        { const uint32 m=next()%4; for(int k=0;k<3;++k)At<float>(o,0x10+4*k)=m==0 ? 0.f : m==1 ? (k==2 ? -0.f : 0.f) : uni(-30.f,30.f);
                          if(m==3){ At<float>(o,0x10)=pedPos.x; At<float>(o,0x14)=pedPos.y; } }
                        break;
                    case TURN: At<uint8>(o,8)=uint8(next()%3==0); At<void*>(o,0xC)=next()%2 ? anim.data() : nullptr; break;
                    case BUS: { const uint32 m=next()%3; At<uintptr_t>(o,8)=m==0 ? 0 : m==1 ? uintptr_t(Veh(3)) : uintptr_t(0x12345678u); break; }
                    case AREA:
                        // campos de CTaskSimpleStandStill + peaton esperado
                        At<int32>(o,8)=next()%4==0 ? -int32(next()%5) : int32(next()%20000);
                        At(o,0xC)=next()%4 ? timeMs-next()%30000 : next(); At<int32>(o,0x10)=next()%5==0 ? -1 : int32(next()%30000);
                        At<uint8>(o,0x14)=uint8(next()%2); At<uint8>(o,0x15)=uint8(next()%3==0); At<uint8>(o,0x18)=uint8(next()%3!=0); At<uint8>(o,0x19)=uint8(next()%2);
                        At<float>(o,0x1C)=next()%3 ? 8.f : uni(0.f,16.f);
                        At<CPed*>(o,0x20)=pickPed();
                        break;
                    case INCAR: At<CPed*>(o,8)=pickPed(); At<uint8>(o,0xC)=uint8(next()%2); break;
                    case OUTCAR:
                        At<CPed*>(o,8)=pickPed(); At<uint8>(o,0xC)=uint8(next()%2);
                        for(int k=0;k<3;++k)At<float>(o,0x10+4*k)=next()%6==0 ? 0.f : uni(-30.f,30.f);
                        break;
                    }
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor";
                    // referencia registrada como la dejaria el constructor (el destructor la limpia)
                    if(!isCtor)if(const size_t rs=RefSlot(cl.kind))if(auto* e=At<CEntity*>(o,rs))e->RegisterReference(&At<CEntity*>(o,rs));
                    CPed* const pd=PedA();
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    // los hooks VMT solo cambian la entrada de la vtable (la direccion de la tabla es informativa)
                    uintptr_t rawRet=0; json ret=nullptr;
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case KINDA: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool)>(target)(o,ctorBool)); break;
                        case AUTODRV: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,ctorVeh)); break;
                        case THROWC: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,const void*)>(target)(o,ctorEnt,ctorVecPtr)); break;
                        case TURN: case BUS: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        case AREA: case INCAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,ctorPed)); break;
                        case OUTCAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,const void*)>(target)(o,ctorPed,vec.data())); break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    refs.push_back(refList(PedA())); refs.push_back(refList(PedB())); refs.push_back(refList(Ent()));
                    for(size_t k=0;k<vehs.size();++k)refs.push_back(refList(Veh(k)));
                    json vehHashes=json::array(); for(auto& v:vehs)vehHashes.push_back(EntHash(v));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_SWIM),F(At<float>(PedA(),P_AIM)),F(At<float>(PedA(),P_HEADRATE))}},
                            {"vehs",vehHashes},{"veh0_fields",{At<uint8>(Veh(0),V_STATUS),At<uint8>(Veh(0),V_MISSION),At<uint8>(Veh(0),V_CRUISE),At(Veh(0),V_FLAGS),At(Veh(0),V_FLAGS+4)}},
                            {"intel",{Hash(intelA.data(),intelA.size()),Hash(intelB.data(),intelB.size())}},{"pdA",Region(pdA.data(),pdA.size())},
                            {"mat",Hash(mat.data(),mat.size())},{"anim",Region(anim.data(),anim.size())},{"anim2",Region(anim2.data(),anim2.size())},
                            {"sec",Region(sec.data(),sec.size(),true)},{"thr",Region(thr.data(),thr.size())},{"event",Region(event.data(),event.size())},
                            {"duck",Hash(duck.data(),duck.size())},{"time",timeMs}};
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    freshIds.clear();
                    CleanRefs();
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
    CleanRefs();
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep||exitEnter!=savedExitEnter;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<extra.size();++k){extra[k]->State(extraEnabled[k]);extra[k]->LockState(extraLocked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep&&exitEnter==savedExitEnter}});
}
}
