#pragma once
// Banco dentro del motor: tareas simples (lista de 43, 40 funciones): CTaskSimpleAchieveHeading,
// CTaskSimpleAffectSecondaryBehaviour, CTaskSimpleDieInCar, CTaskSimpleFacial, CTaskSimpleGetUp, CTaskSimpleRunAnim,
// CTaskSimpleSetStayInSamePlace y CTaskSimpleTogglePedThreatScanner.
// Original (hooks del lote y CTaskSimpleDie::StartAnim desactivados: el ProcessPed del EXE de DieInCar salta a
// CTaskSimpleDie::ProcessPed 0x6397E0, que llama a 0x637520) frente a candidato con fixtures identicos: los
// constructores se llaman en la direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del
// vptr); los metodos virtuales se llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada, hueco 23
// SetMoveAnim), clump (byte de tipo 2), inteligencia, estadisticas, asociaciones de animacion, jerarquia, entidad,
// dos vehiculos falsos (con matriz), tareas padre, subtareas (vtable trazada: destructor, Clone, MakeAbortable) y
// evento falsos en buffers propios. Se simulan y trazan: CAnimManager::BlendAnimation (las dos sobrecargas),
// RpAnimBlendClumpGetAssociation, RpAnimBlendClumpGetFirstAssociation, CAnimBlendAssociation::SetFinishCallback,
// CPed::SetMoveState/SetPedState/ClearAll/StopNonPartialAnims, FindPlayerPed, CEntity::GetIsOnScreen,
// IKChainManager_c (GetLookAtEntity/IsLooking/AbortLookAt/LookAt), CTaskManager::GetTaskSecondary/SetTaskSecondary
// (ChangeTaskInSlot en la DLL), CPedPlacement::IsPositionClearOfCars, IsVehiclePointerValid,
// CCollision::ProcessColModels, CEventDamage::AffectsPed, CPedDamageResponseCalculator::ComputeDamageResponse,
// CEventGroup::Add y el generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber).
// Se fijan y restauran el reloj, ms_fTimeStep, el contador de fotogramas y Pads[0].DisablePlayerControls. Las tareas
// nuevas del pool se siguen (m_LastFreeSlot restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Tasks/TaskTypes/TaskSimpleAchieveHeading.h"
#include "Tasks/TaskTypes/TaskSimpleAffectSecondaryBehaviour.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskSimpleDieInCar.h"
#include "Tasks/TaskTypes/TaskSimpleFacial.h"
#include "Tasks/TaskTypes/TaskSimpleGetUp.h"
#include "Tasks/TaskTypes/TaskSimpleRunAnim.h"
#include "Tasks/TaskTypes/TaskSimpleAnim.h"
#include "Tasks/TaskTypes/TaskSimpleSetStayInSamePlace.h"
#include "Tasks/TaskTypes/TaskSimpleTogglePedThreatScanner.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedPlacement.h"
#include "PedDamageResponseCalculator.h"
#include "Events/EventDamage.h"
#include "Events/EventGroup.h"
#include "Collision/Collision.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace STasks43Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CPhysical / CEntity / CVehicle)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_SEED=0x20, P_MODEL=0x22, P_PHYS_FLAGS=0x40, P_ATTACHED=0xFC,
                 P_IGNORED=0x128, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C,
                 P_IKFLAGS=0x528, P_STATE=0x530, P_HEALTH=0x540, P_ROT_CUR=0x558, P_ROT_AIM=0x55C, P_HEAD_RATE=0x560,
                 P_STANDING_ON=0x568, P_TYPE=0x598, P_STATS=0x59C, P_DEATH_TIME=0x750, V_TYPE=0x590, V_SUBTYPE=0x594;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_FRAME=0xB7CB4C, ADDR_IKMAN=0xC15448, ADDR_COLPOINTS=0xC18F98,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_RUNANIM_CB=0x61A8A0, EXE_GETUP_CB=0x678110, EXE_DIE_CB=0x62FC10;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{}, vehMat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{}, veh2{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,0x80> clump{}, statsA{}, hier{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{}, anim2{};
alignas(16) inline std::array<uint8,0x40> object{};
alignas(16) inline std::array<uint8,0x40> parentA{}, subTask{}, cloneTask{}, secTask{};
alignas(16) inline std::array<uint8,0x60> event{};
inline const char taskName[]{"STasks43"};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
inline CEntity* Veh2() { return reinterpret_cast<CEntity*>(veh2.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{}, taskVtbl{}, subVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t dllGetUpCb{};

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_RUNANIM_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB))return "FinishRunAnimCB";
    if(v==EXE_GETUP_CB||(dllGetUpCb&&v==dllGetUpCb))return "FinishGetUpAnimCB";
    if(v==EXE_DIE_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleDie::FinishAnimDieCB))return "FinishAnimDieCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==ADDR_COLPOINTS)return "colpoints";
    if(v==uintptr_t(taskName))return "taskname";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(vehMat.data(),vehMat.size(),"vehmat"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(veh2.data(),veh2.size(),"veh2"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(intelB.data(),intelB.size(),"intelB"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(statsA.data(),statsA.size(),"stats"))return *s;
    if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(parentA.data(),parentA.size(),"parentA"))return *s;
    if(auto s=in(subTask.data(),subTask.size(),"subtask"))return *s; if(auto s=in(cloneTask.data(),cloneTask.size(),"clonetask"))return *s;
    if(auto s=in(secTask.data(),secTask.size(),"sectask"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"))return *s;
    if(auto s=in(taskVtbl.data(),sizeof(taskVtbl),"taskvtbl"))return *s; if(auto s=in(subVtbl.data(),sizeof(subVtbl),"subvtbl"))return *s;
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
inline uintptr_t blendRet{}, playerRet{}, lookEntRet{}, secondaryRet{}, clearRet{}, cloneRet{};
inline uint32 assocMask{}, rngState{}; inline bool onScreenRet{}, lookingRet{}, frontRet{}, vehValidRet{}, affectsRet{}, subAbortRet{};
inline int32 colRet{};
inline uintptr_t lastEvent{};

// peaton: vtable falsa (todo trazado; hueco 23 = SetMoveAnim)
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline void __fastcall SetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(uintptr_t(p))}); }
// evento falso (GetEventType en el hueco 1, GetEventPriority en el 2)
inline int eventType{}, eventPrio{};
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"event_get_priority",Name(uintptr_t(e))}); return eventPrio; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// tarea padre falsa (GetTaskType en el hueco 4; el tipo se guarda en +0x30)
inline int __fastcall TType(void* t,void*) { trace.push_back({"task_get_type",Name(uintptr_t(t))}); return At<int32>(t,0x30); }
template<int N> void* __fastcall TTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); return nullptr; }
template<int... N> void FillTTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }
// subtareas falsas (destructor 0, Clone 1, GetTaskType 4, MakeAbortable 6)
inline void* __fastcall SDtor(void* t,void*,int flags) { trace.push_back({"subtask_destructor",Name(uintptr_t(t)),flags}); return t; }
inline void* __fastcall SClone(void* t,void*) { trace.push_back({"subtask_clone",Name(uintptr_t(t))}); return reinterpret_cast<void*>(cloneRet); }
inline bool __fastcall SAbort(void* t,void*,CPed* p,int prio,const void* e) { trace.push_back({"subtask_make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return subAbortRet; }
template<int N> void* __fastcall STrap(void*,void*) { trace.push_back({"subtask_unexpected_slot",N}); return nullptr; }
template<int... N> void FillSTraps(std::integer_sequence<int,N...>) { ((subVtbl[N]=reinterpret_cast<void*>(&STrap<N>)),...); }

// campos de los eventos que llegan a CEventGroup::Add / AffectsPed (sin bytes de relleno ni campos sin inicializar)
inline json EventFields(void* e) {
    const int type=int(reinterpret_cast<CEvent*>(e)->GetEventType());
    json r={type,At<int32>(e,4),At<uint8>(e,8)};
    if(type==EVENT_DAMAGE){
        r.push_back(At<uint8>(e,0xC)); r.push_back(At<int16>(e,0xE)); r.push_back(At<int16>(e,0x10)); r.push_back(Name(At(e,0x14)));
        for(size_t off:{size_t(0x18),size_t(0x1C),size_t(0x20)})r.push_back(At(e,off));
        r.push_back(At<uint8>(e,0x24)); r.push_back(At<uint8>(e,0x25)&0x1F); // bits 5-7 sin escribir (and cl,0E0h)
        for(size_t off:{size_t(0x28),size_t(0x2C),size_t(0x30),size_t(0x34),size_t(0x38),size_t(0x3C)})r.push_back(At(e,off));
        for(size_t off=0x40;off<0x44;++off)r.push_back(At<uint8>(e,off));
    } else if(type==EVENT_DEATH){ r.push_back(At<uint8>(e,0xC)); r.push_back(At(e,0x10)); }
    return r;
}

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl BlendH(RpClump* c,void* h,int32 flags,float delta) { trace.push_back({"blend_animation_hier",Name(uintptr_t(c)),Name(uintptr_t(h)),flags,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return (assocMask>>(id%32))&1 ? reinterpret_cast<CAnimBlendAssociation*>(anim.data()) : nullptr; }
inline CAnimBlendAssociation* __cdecl FirstAssoc(RpClump* c,uint32 flags) { trace.push_back({"clump_get_first_association",Name(uintptr_t(c)),flags}); return frontRet ? reinterpret_cast<CAnimBlendAssociation*>(anim.data()) : nullptr; }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
inline void __fastcall SetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(uintptr_t(p)),s}); }
inline void __fastcall ClearAll(CPed* p,void*) { trace.push_back({"ped_clear_all",Name(uintptr_t(p))}); }
inline void __fastcall StopNonPartial(CPed* p,void*) { trace.push_back({"ped_stop_non_partial_anims",Name(uintptr_t(p))}); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline bool __fastcall OnScreen(CEntity* e,void*) { trace.push_back({"get_is_on_screen",Name(uintptr_t(e))}); return onScreenRet; }
inline CEntity* __fastcall LookEnt(void* m,void*,CPed* p) { trace.push_back({"ik_get_look_at_entity",Name(uintptr_t(m)),Name(uintptr_t(p))}); return reinterpret_cast<CEntity*>(lookEntRet); }
inline bool __fastcall IsLooking(void* m,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(uintptr_t(m)),Name(uintptr_t(p))}); return lookingRet; }
inline void __fastcall AbortLook(void* m,void*,CPed* p,uint32 t) { trace.push_back({"ik_abort_look_at",Name(uintptr_t(m)),Name(uintptr_t(p)),t}); }
inline void __fastcall LookAt(void* m,void*,const char* why,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,bool torso,float speed,int32 blend,int32 prio,bool force) {
    trace.push_back({"ik_look_at",Name(uintptr_t(m)),why ? std::string(why) : std::string("<null>"),Name(uintptr_t(p)),Name(uintptr_t(e)),time,bone,
                     off ? json{F(off->x),F(off->y),F(off->z)} : json(nullptr),torso,F(speed),blend,prio,force});
}
inline CTask* __fastcall Secondary(void* tm,void*,int32 idx) { trace.push_back({"get_task_secondary",Name(uintptr_t(tm)),idx}); return reinterpret_cast<CTask*>(secondaryRet); }
// EXE: CTaskManager::SetTaskSecondary(task, idx) 0x681B60; DLL: SetTaskSecondary en linea -> ChangeTaskInSlot(m_aSecondaryTasks[idx], task)
inline void __fastcall SetSecondary(void* tm,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),idx}); }
inline void __fastcall ChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),int32((uintptr_t(slot)-uintptr_t(tm)-0x14)/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
inline CVehicle* __cdecl ClearOfCars(const CPed* p) { trace.push_back({"is_position_clear_of_cars",Name(uintptr_t(p))}); return reinterpret_cast<CVehicle*>(clearRet); }
inline bool __cdecl VehValid(CVehicle* v) { trace.push_back({"is_vehicle_pointer_valid",Name(uintptr_t(v))}); return vehValidRet; }
inline int32 __cdecl ColModels(const CMatrix& a,CColModel& ca,const CMatrix& b,CColModel& cb,std::array<CColPoint,32>& cps,CColPoint* l,float* m,bool f) {
    trace.push_back({"process_col_models",Name(uintptr_t(&a)),uintptr_t(&ca),Name(uintptr_t(&b)),uintptr_t(&cb),Name(uintptr_t(&cps)),Name(uintptr_t(l)),Name(uintptr_t(m)),f}); return colRet;
}
inline bool __fastcall Affects(void* e,void*,CPed* p) { lastEvent=uintptr_t(e); trace.push_back({"event_affects_ped",EventFields(e),Name(uintptr_t(p))}); return affectsRet; }
inline void __fastcall ComputeResp(void* calc,void*,CPed* p,void* resp,bool speak) {
    trace.push_back({"compute_damage_response",Name(At(calc,0)),At(calc,4),At(calc,8),At(calc,0xC),At<uint8>(calc,0x10),Name(uintptr_t(p)),int32(uintptr_t(resp)-lastEvent),speak});
}
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(uintptr_t(g)),EventFields(e),valid}); return nullptr; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { ACHIEVE, AFFECT, DIEINCAR, FACIAL, GETUP, RUNANIM, SETSTAY, TOGGLE };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,8> classes{{
    {"CTaskSimpleAchieveHeading",0x86FD74,0x18,ACHIEVE}, {"CTaskSimpleAffectSecondaryBehaviour",0x870B08,0x14,AFFECT},
    {"CTaskSimpleDieInCar",0x86DDE0,0x28,DIEINCAR}, {"CTaskSimpleFacial",0x870AB8,0x1C,FACIAL},
    {"CTaskSimpleGetUp",0x8703E0,0x10,GETUP}, {"CTaskSimpleRunAnim",0x86D528,0x20,RUNANIM},
    {"CTaskSimpleSetStayInSamePlace",0x86DD50,0xC,SETSTAY}, {"CTaskSimpleTogglePedThreatScanner",0x85A334,0xC,TOGGLE},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=40;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleAchieveHeading","Clone",0,0x86FD74,1,256},{"CTaskSimpleAchieveHeading","GetTaskType",0,0x86FD74,4,64},{"CTaskSimpleAchieveHeading","MakeAbortable",0,0x86FD74,6,512},{"CTaskSimpleAchieveHeading","ProcessPed",0,0x86FD74,7,1024},{"CTaskSimpleAchieveHeading","SetUpIK",0x667F20,0x86FD74,-1,2048},
    {"CTaskSimpleAffectSecondaryBehaviour","Clone",0,0x870B08,1,512},{"CTaskSimpleAffectSecondaryBehaviour","Constructor",0x691270,0x870B08,-1,256},{"CTaskSimpleAffectSecondaryBehaviour","GetTaskType",0,0x870B08,4,64},{"CTaskSimpleAffectSecondaryBehaviour","MakeAbortable",0,0x870B08,6,128},{"CTaskSimpleAffectSecondaryBehaviour","ProcessPed",0,0x870B08,7,1024},
    {"CTaskSimpleDieInCar","Clone",0x635EF0,0x86DDE0,1,256},{"CTaskSimpleDieInCar","Constructor",0x62FC20,0x86DDE0,-1,256},{"CTaskSimpleDieInCar","Destructor",0x62FC70,0x86DDE0,-1,256},{"CTaskSimpleDieInCar","GetTaskType",0x62FC60,0x86DDE0,4,64},{"CTaskSimpleDieInCar","ProcessPed",0x6398F0,0x86DDE0,7,2048},
    {"CTaskSimpleFacial","Clone",0,0x870AB8,1,256},{"CTaskSimpleFacial","GetAnimId",0x690CC0,0x870AB8,-1,128},{"CTaskSimpleFacial","GetTaskType",0,0x870AB8,4,64},{"CTaskSimpleFacial","MakeAbortable",0,0x870AB8,6,512},{"CTaskSimpleFacial","ProcessPed",0,0x870AB8,7,2048},
    {"CTaskSimpleGetUp","Constructor",0x677F50,0x8703E0,-1,64},{"CTaskSimpleGetUp","FinishGetUpAnimCB",0x678110,0x8703E0,-1,256},{"CTaskSimpleGetUp","MakeAbortable",0,0x8703E0,6,1024},{"CTaskSimpleGetUp","ProcessPed",0,0x8703E0,7,1024},{"CTaskSimpleGetUp","StartAnim",0x67C770,0x8703E0,-1,2048},
    {"CTaskSimpleRunAnim","Clone",0,0x86D528,1,256},{"CTaskSimpleRunAnim","Constructor",0x61A8B0,0x86D528,-1,256},{"CTaskSimpleRunAnim","Constructor2",0x61A900,0x86D528,-1,256},{"CTaskSimpleRunAnim","ProcessPed",0,0x86D528,7,512},{"CTaskSimpleRunAnim","StartAnim",0x61A950,0x86D528,-1,256},
    {"CTaskSimpleSetStayInSamePlace","Clone",0x635BD0,0x86DD50,1,128},{"CTaskSimpleSetStayInSamePlace","Constructor",0x62F590,0x86DD50,-1,64},{"CTaskSimpleSetStayInSamePlace","GetTaskType",0x62F5B0,0x86DD50,4,64},{"CTaskSimpleSetStayInSamePlace","MakeAbortable",0x62F5C0,0x86DD50,6,64},{"CTaskSimpleSetStayInSamePlace","ProcessPed",0x62F5E0,0x86DD50,7,256},
    {"CTaskSimpleTogglePedThreatScanner","Clone",0x492D50,0x85A334,1,128},{"CTaskSimpleTogglePedThreatScanner","Constructor",0x492D10,0x85A334,-1,64},{"CTaskSimpleTogglePedThreatScanner","GetTaskType",0x492DD0,0x85A334,4,64},{"CTaskSimpleTogglePedThreatScanner","MakeAbortable",0x492DE0,0x85A334,6,64},{"CTaskSimpleTogglePedThreatScanner","ProcessPed",0x6337A0,0x85A334,7,256},
}};
// hooks que el codigo del EXE alcanza por llamada directa: se desactivan en la ejecucion original para que sea EXE puro
inline const std::array<std::pair<const char*,const char*>,1> baseHooks{{
    {"CTaskSimpleDie","StartAnim"},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent()); clean(Veh()); clean(Veh2());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks43");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> extra;
    for(auto& [c,n]:baseHooks){ if(auto* h=FindHook(c,n))extra.push_back(h); else Write({{"event","note"},{"reason","base hook not found (se deja como esta)"},{"function",std::string(c)+"::"+n}}); }
    // direccion DLL del callback privado de GetUp (la del hook) para poder nombrarlo
    if(auto* h=FindHook("CTaskSimpleGetUp","FinishGetUpAnimCB"))dllGetUpCb=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    // CEventDamage::AffectsPed de la DLL se llama directamente sobre el objeto local: su direccion sale de la vtable de la DLL (hueco 5)
    uintptr_t dllAffects{}; { CEventDamage tmp(nullptr,0,WEAPON_UNARMED,PED_PIECE_TORSO,0,false,false); dllAffects=uintptr_t((*reinterpret_cast<void***>(&tmp))[5]); }
    // modelos con informacion de modelo (ProcessColModels del EXE y de la DLL leen el puntero al modelo de colision)
    const int16 pedModel=player->m_nModelIndex; int16 vehModel=-1;
    for(int32 m=400;m<612&&vehModel<0;++m)if(CModelInfo::GetModelInfo(m))vehModel=int16(m);
    if(vehModel<0||!CModelInfo::GetModelInfo(pedModel)){Write({{"event","error"},{"reason","model info missing"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&SetMoveAnim);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    FillTTraps(std::make_integer_sequence<int,16>{}); taskVtbl[4]=reinterpret_cast<void*>(&TType);
    FillSTraps(std::make_integer_sequence<int,16>{}); subVtbl[0]=reinterpret_cast<void*>(&SDtor); subVtbl[1]=reinterpret_cast<void*>(&SClone);
    subVtbl[4]=reinterpret_cast<void*>(&TType); subVtbl[6]=reinterpret_cast<void*>(&SAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto& frame=*reinterpret_cast<uint32*>(ADDR_FRAME); const uint32 savedFrame=frame;
    auto& padDisable=CPad::GetPad(0)->DisablePlayerControls; const uint16 savedPad=padDisable;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; frame=savedFrame; padDisable=savedPad; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::vector<bool> extraEnabled,extraLocked; for(auto* h:extra){extraEnabled.push_back(h->Hooked());extraLocked.push_back(h->Locked());h->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); for(auto* h:extra)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32,float)>(&CAnimManager::BlendAnimation)),BlendH), a2e(0x4D4410,BlendH);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a4(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),FirstAssoc), a4e(0x4D6A70,FirstAssoc);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a5e(0x4CEBE0,SetFinishCb);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::SetPedState),SetPedState), p2e(0x5E4500,SetPedState);
        EntryPatch p3(bit_cast<uintptr_t>(&CPed::ClearAll),ClearAll), p3e(0x5E5320,ClearAll);
        EntryPatch p4(bit_cast<uintptr_t>(&CPed::StopNonPartialAnims),StopNonPartial), p4e(0x5DED10,StopNonPartial);
        EntryPatch p5(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), p5e(0x56E210,FindPlayer);
        EntryPatch e1(bit_cast<uintptr_t>(&CEntity::GetIsOnScreen),OnScreen), e1e(0x534540,OnScreen);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::GetLookAtEntity),LookEnt), k1e(0x6181D0,LookEnt);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), k2e(0x6181A0,IsLooking);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook), k3e(0x618280,AbortLook);
        EntryPatch k4(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), k4e(0x618970,LookAt);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<CTask*(CTaskManager::*)(eSecondaryTask)>(&CTaskManager::GetTaskSecondary)),Secondary), t1e(0x681810,Secondary);
        EntryPatch t2(TaskManagerAccess::ChangeSlotAddress(),ChangeSlot), t2e(0x681B60,SetSecondary);
        EntryPatch g1(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(const CPed*)>(&CPedPlacement::IsPositionClearOfCars)),ClearOfCars), g1e(0x616A40,ClearOfCars);
        EntryPatch g2(bit_cast<uintptr_t>(&IsVehiclePointerValid),VehValid), g2e(0x6E38F0,VehValid);
        EntryPatch g3(bit_cast<uintptr_t>(&CCollision::ProcessColModels),ColModels), g3e(0x4185C0,ColModels);
        EntryPatch g4(dllAffects,Affects), g4e(0x4B35A0,Affects);
        EntryPatch g5(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeDamageResponse),ComputeResp), g5e(0x4B5AC0,ComputeResp);
        EntryPatch g6(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), g6e(0x4AB420,GroupAdd);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4343u; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    rngState=next();
                    timeMs=next(); timeStep=next()%3 ? 1.f : (next()%5==0 ? (next()%2 ? 0.f : -0.5f) : uni(0.1f,3.f));
                    frame=next(); padDisable=next()%2 ? 0 : uint16(next());
                    // peatones (vtable falsa, clump con tipo 2, inteligencia y estadisticas falsas)
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size()); std::memset(vehMat.data(),0,vehMat.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(veh.data(),0,veh.size()); std::memset(veh2.data(),0,veh2.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto* b:{&intelA,&intelB})for(auto& x:*b)x=uint8(next());
                    for(auto& b:statsA)b=uint8(next()); At<float>(statsA.data(),0x20)=uni(0.5f,15.f);
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At(Ent(),P_PHYS_FLAGS)=next(); At<int16>(Ent(),P_MODEL)=vehModel;
                    for(auto* v:{Veh(),Veh2()}){ v->SetType(ENTITY_TYPE_VEHICLE); At<void*>(v,P_MATRIX)=vehMat.data(); At<int16>(v,P_MODEL)=vehModel;
                        { static const uint32 types[]{0,0,9,2,3,9,0}; At(v,V_TYPE)=types[next()%7]; } At(v,V_SUBTYPE)=next()%4==0 ? 2u : next()%3; }
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next(); At(p,P_IKFLAGS)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At(p,P_PHYS_FLAGS)=next(); At<uint16>(p,P_SEED)=uint16(next()); At<int16>(p,P_MODEL)=pedModel;
                        { static const uint32 states[]{0,1,0x36,0x37,12,40}; At(p,P_STATE)=next()%3 ? states[next()%6] : next()%64; }
                        { static const uint32 types[]{0,1,2,6,6,7,4}; At(p,P_TYPE)=types[next()%7]; }
                        At<float>(p,P_HEALTH)=uni(0.f,100.f); At(p,P_DEATH_TIME)=next();
                        At<float>(p,P_ROT_CUR)=uni(-6.5f,6.5f); At<float>(p,P_ROT_AIM)=uni(-6.5f,6.5f); At<float>(p,P_HEAD_RATE)=uni(0.f,15.f);
                        At<void*>(p,P_STATS)=statsA.data();
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelB.data();
                    // GetUp::StartAnim pasa la matriz cruda del peaton (la DLL usaria GetMatrix y la crearia): siempre presente
                    if(cl.kind==GETUP||next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{};
                        m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    { CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; std::memcpy(vehMat.data(),&m,sizeof(CMatrix)); }
                    // asociaciones de animacion falsas (floats controlados, sin NaN de senalizacion) y jerarquia
                    for(auto* a:{&anim,&anim2}){ for(auto& b:*a)b=uint8(next());
                        static const float deltas[]{-4.f,0.f,4.f,-1000.f,1.f};
                        At<float>(a->data(),0x18)=uni(0.f,1.f); At<float>(a->data(),0x1C)=deltas[next()%5];
                        At<void*>(a->data(),0x14)=hier.data(); At<float>(a->data(),0x20)=uni(0.f,2.f); At<float>(a->data(),0x24)=uni(0.f,2.f); }
                    for(auto& b:hier)b=uint8(next()); At<float>(hier.data(),0x10)=next()%6 ? uni(0.f,2.6f) : At<float>(anim.data(),0x20)/0.75f;
                    // tarea padre falsa
                    for(auto& b:parentA)b=uint8(next()); *reinterpret_cast<void**>(parentA.data())=taskVtbl.data();
                    { static const int32 types[]{TASK_COMPLEX_AVOID_OTHER_PED_WHILE_WANDERING,TASK_COMPLEX_AVOID_ENTITY,TASK_SIMPLE_GO_TO_POINT,TASK_COMPLEX_WANDER};
                      At<int32>(parentA.data(),0x30)=types[next()%4]; }
                    // subtareas falsas
                    for(auto* t:{&subTask,&cloneTask,&secTask}){ for(auto& b:*t)b=uint8(next()); *reinterpret_cast<void**>(t->data())=subVtbl.data(); At<int32>(t->data(),0x30)=int32(next()%1000); }
                    // evento falso (campos de CEventDamage leidos por GetUp::MakeAbortable)
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    { static const int types[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_DEATH,EVENT_SCRIPT_COMMAND}; eventType=types[next()%4]; }
                    eventPrio=next()%3 ? int(55+next()%15) : int(next()%100);
                    At<uint8>(event.data(),0x40)=uint8(next()%2); At<uint8>(event.data(),0xC)=uint8(next()%2);
                    At(event.data(),0x18)=next()%5==0 ? timeMs+next()%100 : timeMs-(next()%4 ? next()%200 : next());
                    // dependencias simuladas
                    blendRet=(cl.kind==DIEINCAR||cl.kind==GETUP||next()%4) ? uintptr_t(anim2.data()) : 0; // Die/GetUp escriben en la asociacion devuelta
                    assocMask=next(); frontRet=next()%2;
                    { const uint32 m=next()%3; playerRet=m==0 ? 0 : m==1 ? uintptr_t(PedA()) : uintptr_t(PedB()); }
                    onScreenRet=next()%5!=0; lookingRet=next()%2; lookEntRet=next()%5 ? 0 : uintptr_t(Ent()); secondaryRet=next()%5 ? 0 : uintptr_t(secTask.data());
                    if(cl.kind==AFFECT)secondaryRet=next()%2 ? 0 : uintptr_t(secTask.data());
                    cloneRet=next()%5 ? uintptr_t(cloneTask.data()) : 0; subAbortRet=next()%2;
                    { const uint32 m=next()%5; clearRet=m<2 ? 0 : m==2 ? uintptr_t(Veh()) : uintptr_t(Veh2()); }
                    vehValidRet=next()%4!=0; affectsRet=next()%3!=0;
                    { static const int32 cols[]{0,1,-1,3,0}; colRet=cols[next()%5]; }
                    At<void*>(PedA(),P_ATTACHED)=next()%4==0 ? static_cast<void*>(Veh()) : nullptr;
                    At<void*>(PedA(),P_STANDING_ON)=next()%3==0 ? static_cast<void*>(next()%2 ? Veh2() : Ent()) : nullptr;
                    { const uint32 m=next()%6; At<void*>(PedA(),P_IGNORED)=m<2 ? nullptr : m==2 ? static_cast<void*>(Ent()) : m==3 ? static_cast<void*>(Veh2()) : static_cast<void*>(Veh()); }
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    // argumentos de constructor
                    const int32 ctorGroup=int32(next()%118), ctorId=next()%4 ? int32(next()%400) : ANIM_ID_NO_ANIMATION_SET, ctorType=int32(next());
                    const float ctorBlend=next()%3 ? 4.f : uni(-8.f,8.f);
                    const bool ctorB0=next()%2, ctorB1=next()%2, ctorB2=next()%2;
                    void* const ctorTask=next()%4 ? static_cast<void*>(subTask.data()) : nullptr; const char* const ctorName=next()%2 ? taskName : nullptr;
                    switch(cl.kind){
                    case ACHIEVE: {
                        const float desired=uni(-6.5f,6.5f); At<float>(o,8)=desired; At<float>(o,0xC)=next()%3 ? 0.5f : uni(0.f,2.f);
                        { const uint32 m=next()%4; At<float>(o,0x10)=m==0 ? 0.2f : m==1 ? 0.f : uni(0.f,0.6f); }
                        if(next()%2)At<float>(PedA(),P_ROT_CUR)=desired+(next()%3 ? uni(-0.4f,0.4f) : uni(-7.f,7.f));
                        At<void*>(o,4)=next()%3 ? nullptr : static_cast<void*>(parentA.data());
                        if(next()%3)At(PedA(),P_FLAGS1)&=~(1u<<8); // sobre todo fuera de vehiculo
                        break; }
                    case AFFECT:
                        At<uint8>(o,8)=uint8(next()%2); At<int32>(o,0xC)=int32(next()%6);
                        // ProcessPed con m_bAdd usa m_task sin comprobarlo
                        At<void*>(o,0x10)=(At<uint8>(o,8)&&nm=="ProcessPed")||next()%4 ? static_cast<void*>(subTask.data()) : nullptr;
                        break;
                    case DIEINCAR:
                        At<int32>(o,8)=int32(next()%118);
                        { const uint32 m=next()%4; At<int32>(o,0xC)=m==0 ? ANIM_ID_NO_ANIMATION_SET : m==1 ? ANIM_ID_DROWN : int32(next()%300); }
                        At<void*>(o,0x10)=next()%4 ? nullptr : static_cast<void*>(hier.data()); At(o,0x14)=next()%0x10000;
                        At<float>(o,0x18)=next()%3 ? 4.f : uni(0.f,8.f);
                        { const uint32 m=next()%4; At<float>(o,0x1C)=m==0 ? 0.f : m==1 ? -1.f : uni(0.f,2.f); }
                        At<void*>(o,0x24)=next()%3 ? nullptr : static_cast<void*>(anim.data());
                        break;
                    case FACIAL:
                        At(o,8)=next()%4 ? timeMs-next()%5000 : next(); At<int32>(o,0xC)=next()%6==0 ? -int32(next()%5) : int32(next()%5000);
                        At<uint8>(o,0x10)=uint8(next()%2); At<uint8>(o,0x11)=uint8(next()%3==0);
                        At<int32>(o,0x14)=int32(next()%12)-2; At<int32>(o,0x18)=next()%6==0 ? -int32(next()%5) : int32(next()%8000);
                        if(next()%2)assocMask|=1u<<(ANIM_ID_FACTALK%32); // mas casos con la animacion de hablar
                        break;
                    case GETUP:
                        At<uint8>(o,8)=uint8(next()%3==0); At<uint8>(o,9)=uint8(next()%4==0);
                        At<void*>(o,0xC)=nm=="FinishGetUpAnimCB"||next()%2 ? static_cast<void*>(anim.data()) : nullptr;
                        break;
                    case RUNANIM:
                        At<void*>(o,8)=next()%3 ? nullptr : static_cast<void*>(anim.data());
                        At<int32>(o,0x10)=int32(next()%118); At<int32>(o,0x14)=int32(next()%400); At<float>(o,0x18)=next()%3 ? 4.f : uni(-8.f,8.f);
                        At<uint16>(o,0x1C)=uint16(next()%3 ? TASK_SIMPLE_ANIM : next());
                        break;
                    case SETSTAY: At<uint8>(o,8)=uint8(next()%2); break;
                    case TOGGLE: At<uint8>(o,8)=uint8(next()%2); At<uint8>(o,9)=uint8(next()%2); At<uint8>(o,0xA)=uint8(next()%2); break;
                    }
                    CPed* const pd=PedA();
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm.starts_with("Constructor"), isDtor=nm=="Destructor", isVoid=nm=="SetUpIK"||nm=="FinishGetUpAnimCB"||(nm=="StartAnim"&&cl.kind==RUNANIM);
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case AFFECT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool,int32,void*)>(target)(o,ctorB0,int32(next()%8),ctorTask)); break;
                        case DIEINCAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(target)(o,ctorGroup,ctorId)); break;
                        case GETUP: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        case RUNANIM:
                            if(nm=="Constructor")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,float,bool)>(target)(o,ctorGroup,ctorId,ctorBlend,ctorB0));
                            else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,float,int32,const char*,bool)>(target)(o,ctorGroup,ctorId,ctorBlend,ctorType,ctorName,ctorB0));
                            break;
                        case SETSTAY: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool)>(target)(o,ctorB0)); break;
                        case TOGGLE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool,bool,bool)>(target)(o,ctorB0,ctorB1,ctorB2)); break;
                        default: break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="StartAnim"&&cl.kind==GETUP)ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="StartAnim"||nm=="SetUpIK")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="GetAnimId")ret=reinterpret_cast<int32(__cdecl*)(int32)>(target)(int32(next()%14)-3);
                    else if(nm=="FinishGetUpAnimCB"){ void* a=next()%3 ? static_cast<void*>(anim2.data()) : nullptr; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(a,o); }
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Ent(),Veh(),Veh2()})refs.push_back(refList(e));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"veh",EntHash(veh)},{"veh2",EntHash(veh2)},{"intelA",Hash(intelA.data(),intelA.size())},{"intelB",Hash(intelB.data(),intelB.size())},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS3),At(PedA(),P_FLAGS4),At(PedA(),P_IKFLAGS),Name(At(PedA(),P_IGNORED)),
                                           At(PedA(),P_ROT_AIM),At(PedA(),P_HEAD_RATE),At(PedA(),P_HEALTH),At(PedA(),P_DEATH_TIME)}},
                            {"stats",Hash(statsA.data(),statsA.size())},{"mat",Hash(mat.data(),mat.size())},
                            {"anim",Region(anim.data(),anim.size())},{"anim2",Region(anim2.data(),anim2.size())},{"hier",Region(hier.data(),hier.size())},
                            {"subtasks",{Region(subTask.data(),subTask.size(),true),Region(cloneTask.data(),cloneTask.size(),true),Region(secTask.data(),secTask.size(),true)}},
                            {"parent",Region(parentA.data(),parentA.size(),true)},{"event",Region(event.data(),event.size(),true)},{"time",timeMs},{"frame",frame}};
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
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep||frame!=savedFrame||padDisable!=savedPad;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<extra.size();++k){extra[k]->State(extraEnabled[k]);extra[k]->LockState(extraLocked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep&&frame==savedFrame&&padDisable==savedPad}});
}
}
