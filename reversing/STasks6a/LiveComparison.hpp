#pragma once
// Banco dentro del motor: tareas simples A (lista de 6, 42 funciones): CTaskSimpleClearLookAt,
// CTaskSimpleCreateCarAndGetIn, CTaskSimpleDoHandSignal, CTaskSimpleDuckToggle, CTaskSimpleFall, CTaskSimpleGiveCPR y
// CTaskSimpleGoToPoint.
// Original (hooks del lote y CTaskSimpleGoTo::QuitIK desactivados) frente a candidato con fixtures identicos: los
// constructores se llaman en la direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del
// vptr); los metodos virtuales se llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada, hueco 23
// SetMoveAnim), clump (byte de tipo 2), inteligencia, datos de jugador, pad, asociaciones de animacion, jerarquias,
// asociaciones estaticas, informacion de arma, tareas falsas (vtable trazada), evento, accidente y dos vehiculos falsos
// dentro de un pool de vehiculos falso (el puntero global 0xB74494 se cambia solo durante la llamada y se restaura).
// CreateCarAndGetIn nunca crea vehiculos reales: CCarCtrl::CreateCarForScript, CStreaming::RequestModel,
// CTheScripts::CleanUpThisVehicle, CMissionCleanup::RemoveEntityFromList, las consultas de caminos, la camara y
// CPedPlacement::IsPositionClearForPed se simulan y trazan. Tambien se simulan y trazan: animacion (BlendAnimation,
// GetAssociation, GetFirstAssociation, SetFinishCallback, SetCurrentTime), IKChainManager_c (IsLooking/AbortLookAt),
// CEntity::GetIsOnScreen, CTaskManager (GetTaskSecondary, SetTaskSecondary/ChangeTaskInSlot, GetSimplestActiveTask),
// CPedIntelligence (GetTaskDuck/GetTaskUseGun/GetTaskHold), CTaskSimpleDuck (AbortBecauseOfOtherDuck, SetControlType,
// ControlDuckMove), CTaskSimpleUseGun::ClearAnim, CTaskSimpleStandStill::ProcessPed,
// CTaskSimpleCarSetPedInAsDriver::ProcessPed, CTaskSimpleGoTo (HasCircledTarget/SetUpIK), CEventScanner::ScanForEventsNow,
// CPed::SetMoveState, CPlayerPed (GetPadFromPlayer, SetPlayerMoveBlendRatio, SetRealMoveAnim, ControlButtonSprint),
// CWeaponInfo::GetWeaponInfo, SurfaceInfos_c::CantSprintOn, CAnimBlendAssocGroup::GetAnimation,
// CGeneral (GetRadianAngleBetweenPoints/LimitRadianAngle), CInterestingEvents::Add y CEventGroup::Add.
// Se fijan y restauran el reloj, ms_fTimeStep, Pads[0].DisablePlayerControls y CTaskSimpleFall::m_nMaxPlayerDownTime.
// Las tareas nuevas del pool se siguen (m_LastFreeSlot restaurado y huecos libres con 0xCD) y se liberan sin destructor
// (el destructor de CTaskComplexPlayHandSignalAnim toca contadores de modelos reales).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Tasks/TaskTypes/TaskSimpleClearLookAt.h"
#include "Tasks/TaskTypes/TaskSimpleCreateCarAndGetIn.h"
#include "Tasks/TaskTypes/TaskSimpleDoHandSignal.h"
#include "Tasks/TaskTypes/TaskSimpleDuckToggle.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleFall.h"
#include "Tasks/TaskTypes/TaskSimpleGiveCPR.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPoint.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskSimpleCarSetPedInAsDriver.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedPlacement.h"
#include "Events/EventGroup.h"
#include "Events/EventScanner.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendAssocGroup.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "InterestingEvents.h"
#include "MissionCleanup.h"
#include "SurfaceInfos_c.h"
#include "WeaponInfo.h"
#include "CarCtrl.h"
#include "PathFind.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace STasks6aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CPhysical / CEntity)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_SEED=0x20, P_MODEL=0x22, P_PHYS_FLAGS=0x40, P_MOVE_SPEED=0x44,
                 P_SURFACE=0xBA, P_ATTACHED=0xFC, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C,
                 P_PLAYERDATA=0x480, P_ANIMGROUP=0x4D4, P_IKFLAGS=0x528, P_STATE=0x530, P_MOVESTATE=0x534, P_HEALTH=0x540,
                 P_ROT_AIM=0x55C, P_TYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, VEH_STRIDE=0xA18;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_IKMAN=0xC15448, ADDR_VEHPOOL=0xB74494, ADDR_MAXDOWN=0x8D2EF4,
                    ADDR_GROUPS=0xB4EA34, ADDR_CAMERA=0xB6F028, ADDR_PATHS=0x96F050, ADDR_INTEREST=0xC0B058, ADDR_CLEANUP=0xA90850,
                    ADDR_SURFACES=0xB79538, ADDR_STREAMINFO=0x8E4CC0,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_FALL_CB=0x6786B0, EXE_CPR_CB=0x658990;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{}, intelB{};
alignas(16) inline std::array<uint8,0x100> playerData{};
alignas(16) inline std::array<uint8,sizeof(CPad)> pad{};
alignas(16) inline std::array<uint8,0x80> clump{}, hier{}, hier2{}, weaponInfo{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{}, anim2{}, anim3{};
alignas(16) inline std::array<uint8,0x40> sassoc1{}, sassoc2{};
alignas(16) inline std::array<uint8,0x40> object{};
alignas(16) inline std::array<uint8,0x40> secTask{}, duckS{}, duckT{}, useGun{}, hold{};
alignas(16) inline std::array<uint8,0x60> event{};
alignas(16) inline std::array<uint8,0x10> accident{};
alignas(16) inline std::array<uint8,VEH_STRIDE*2> vehStore{};
alignas(16) inline std::array<uint8,4> vehStates{};
alignas(16) inline std::array<uint32,8> fakePool{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Veh(int k) { return reinterpret_cast<CEntity*>(vehStore.data()+VEH_STRIDE*k); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{}, taskVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline uintptr_t groupsBase{};

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_FALL_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleFall::FinishFallAnimCB))return "FinishFallAnimCB";
    if(v==EXE_CPR_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleGiveCPR::FinishGiveCPRAnimCB))return "FinishGiveCPRAnimCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(intelB.data(),intelB.size(),"intelB"))return *s;
    if(auto s=in(playerData.data(),playerData.size(),"playerdata"))return *s; if(auto s=in(pad.data(),pad.size(),"pad"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(hier2.data(),hier2.size(),"hier2"))return *s; if(auto s=in(weaponInfo.data(),weaponInfo.size(),"weaponinfo"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    if(auto s=in(anim3.data(),anim3.size(),"anim3"))return *s;
    if(auto s=in(sassoc1.data(),sassoc1.size(),"sassoc1"))return *s; if(auto s=in(sassoc2.data(),sassoc2.size(),"sassoc2"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(secTask.data(),secTask.size(),"sectask"))return *s; if(auto s=in(duckS.data(),duckS.size(),"duckS"))return *s;
    if(auto s=in(duckT.data(),duckT.size(),"duckT"))return *s; if(auto s=in(useGun.data(),useGun.size(),"usegun"))return *s;
    if(auto s=in(hold.data(),hold.size(),"hold"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(accident.data(),accident.size(),"accident"))return *s;
    if(auto s=in(vehStore.data(),VEH_STRIDE,"veh0"))return *s; if(auto s=in(vehStore.data()+VEH_STRIDE,VEH_STRIDE,"veh1"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"))return *s;
    if(auto s=in(taskVtbl.data(),sizeof(taskVtbl),"taskvtbl"))return *s;
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
inline std::string EntHash(const void* p,size_t n) {
    std::vector<uint8> copy(static_cast<const uint8*>(p),static_cast<const uint8*>(p)+n);
    const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(p)->m_pReferences)-uintptr_t(p);
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline uintptr_t secondaryRet{}, simplestRet{}, duckFalseRet{}, duckTrueRet{}, useGunRet{}, holdRet{}, padRet{}, firstRet{}, createRet{};
inline uint32 assocMask{}, visMask{}, visCalls{}, nodeRet{};
inline bool onScreenRet{}, lookingRet{}, subAbortRet{}, setCtrlRet{}, circledRet{}, cantSprintRet{}, foundRet{}, clearRet{};
inline float sprintRet{};
inline CVector nodePos{};

// peaton: vtable falsa (todo trazado; hueco 23 = SetMoveAnim)
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline void __fastcall SetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(uintptr_t(p))}); }
// evento falso (GetEventType en el hueco 1 sin trazar: el EXE lo llama varias veces; GetEventPriority en el 2)
inline int eventType{}, eventPrio{};
inline int __fastcall EType(void*,void*) { return eventType; }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"event_get_priority",Name(uintptr_t(e))}); return eventPrio; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// tareas falsas: GetSubTask (2) nulo, GetTaskType (4, tipo en +0x30) sin trazar, MakeAbortable (6) trazado
inline void* __fastcall TSub(void*,void*) { return nullptr; }
inline int __fastcall TType(void* t,void*) { return At<int32>(t,0x30); }
inline bool __fastcall TAbort(void* t,void*,CPed* p,int prio,const void* e) { trace.push_back({"task_make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return subAbortRet; }
template<int N> void* __fastcall TTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); return nullptr; }
template<int... N> void FillTTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }

inline json EventFields(void* e) { return {int(reinterpret_cast<CEvent*>(e)->GetEventType()),At<int32>(e,4),At<uint8>(e,8)}; }

// dependencias simuladas
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(anim3.data()); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id});
    if(!((assocMask>>(id%32))&1))return nullptr; return reinterpret_cast<CAnimBlendAssociation*>((id&1) ? anim.data() : anim2.data()); }
inline CAnimBlendAssociation* __cdecl FirstAssoc(RpClump* c,uint32 flags) { trace.push_back({"clump_get_first_association",Name(uintptr_t(c)),flags}); return reinterpret_cast<CAnimBlendAssociation*>(firstRet); }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetCurTime(CAnimBlendAssociation* a,void*,float t) { trace.push_back({"set_current_time",Name(uintptr_t(a)),F(t)}); }
inline bool __fastcall OnScreen(CEntity* e,void*) { trace.push_back({"get_is_on_screen",Name(uintptr_t(e))}); return onScreenRet; }
inline bool __fastcall IsLooking(void* m,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(uintptr_t(m)),Name(uintptr_t(p))}); return lookingRet; }
inline void __fastcall AbortLook(void* m,void*,CPed* p,uint32 t) { trace.push_back({"ik_abort_look_at",Name(uintptr_t(m)),Name(uintptr_t(p)),t}); }
inline CTask* __fastcall Secondary(void* tm,void*,int32 idx) { trace.push_back({"get_task_secondary",Name(uintptr_t(tm)),idx}); return reinterpret_cast<CTask*>(secondaryRet); }
// EXE: CTaskManager::SetTaskSecondary(task, idx) 0x681B60; DLL: SetTaskSecondary en linea -> ChangeTaskInSlot(m_aSecondaryTasks[idx], task)
inline void __fastcall SetSecondary(void* tm,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),idx}); }
inline void __fastcall ChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),int32((uintptr_t(slot)-uintptr_t(tm)-0x14)/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
// consultas puras (el EXE las repite; sin trazar)
inline CTask* __fastcall Simplest(void*,void*) { return reinterpret_cast<CTask*>(simplestRet); }
inline void* __fastcall TaskDuck(void*,void*,bool ignore) { return reinterpret_cast<void*>(ignore ? duckTrueRet : duckFalseRet); }
inline void* __fastcall TaskUseGun(void*,void*) { return reinterpret_cast<void*>(useGunRet); }
inline void* __fastcall TaskHold(void*,void*,bool) { return reinterpret_cast<void*>(holdRet); }
inline void __fastcall ClearAnim(void* t,void*,CPed* p) { trace.push_back({"use_gun_clear_anim",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline void __fastcall AbortOtherDuck(void* t,void*,CPed* p) { trace.push_back({"duck_abort_because_of_other_duck",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline bool __fastcall SetControlType(void* t,void*,uint8 type) { trace.push_back({"duck_set_control_type",Name(uintptr_t(t)),type}); return setCtrlRet; }
inline void __fastcall DuckMove(void* t,void*,float x,float y) { trace.push_back({"duck_control_move",Name(uintptr_t(t)),F(x),F(y)}); }
inline CPad* __fastcall PadFromPlayer(void* p,void*) { trace.push_back({"get_pad_from_player",Name(uintptr_t(p))}); return reinterpret_cast<CPad*>(padRet); }
inline void __fastcall InterestingAdd(void* ie,void*,int32 type,CEntity* e) { trace.push_back({"interesting_events_add",uintptr_t(ie)==ADDR_INTEREST,type,Name(uintptr_t(e))}); }
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(uintptr_t(g)),EventFields(e),valid}); return nullptr; }
inline bool __fastcall HasCircled(void* t,void*,CPed* p) { trace.push_back({"goto_has_circled_target",Name(uintptr_t(t)),Name(uintptr_t(p))}); return circledRet; }
inline void __fastcall SetUpIK(void* t,void*,CPed* p) { trace.push_back({"goto_set_up_ik",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline void __fastcall ScanNow(void* s,void*,const CPed* p,bool dontScan) { trace.push_back({"scan_for_events_now",Name(uintptr_t(s)),Name(uintptr_t(p)),dontScan}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
inline void __fastcall SetBlendRatio(void* p,void*,CVector* v) { trace.push_back({"set_player_move_blend_ratio",Name(uintptr_t(p)),Name(uintptr_t(v))}); }
inline void __fastcall SetRealMoveAnim(void* p,void*) { trace.push_back({"set_real_move_anim",Name(uintptr_t(p))}); }
inline float __fastcall Sprint(void* p,void*,int32 type) { trace.push_back({"control_button_sprint",Name(uintptr_t(p)),type}); return sprintRet; }
inline void* __cdecl WeaponInfoOf(int32 type,int32 skill) { trace.push_back({"get_weapon_info",type,skill&0xFF}); return weaponInfo.data(); }
inline bool __fastcall CantSprint(void* s,void*,uint32 id) { trace.push_back({"cant_sprint_on",uintptr_t(s)==ADDR_SURFACES,id&0xFF}); return cantSprintRet; }
inline void* __fastcall GetAnimation(void* g,void*,int32 id) {
    trace.push_back({"assoc_group_get_animation",int32((uintptr_t(g)-groupsBase)/0x14),int32((uintptr_t(g)-groupsBase)%0x14),id});
    return id==ANIM_ID_RUN ? sassoc1.data() : sassoc2.data();
}
inline float __cdecl AngleBetween(float x1,float y1,float x2,float y2) { trace.push_back({"radian_angle_between_points",F(x1),F(y1),F(x2),F(y2)}); return x1*0.5f-y1; }
inline float __cdecl LimitAngle(float a) { trace.push_back({"limit_radian_angle",F(a)}); return a*0.25f+0.125f; }
inline bool __fastcall StandStill(void* t,void*,CPed* p) {
    trace.push_back({"stand_still_process_ped",Name(uintptr_t(p)),At<int32>(t,8),At<uint8>(t,0x18),At<uint8>(t,0x19),At(t,0x1C)}); return false;
}
inline bool __fastcall SetPedInDriver(void* t,void*,CPed* p) {
    trace.push_back({"set_ped_in_as_driver_process_ped",Name(uintptr_t(p)),At<uint8>(t,8),Name(At(t,0xC)),Name(At(t,0x10)),Name(At(t,0x14)),At<uint8>(t,0x18),At<uint8>(t,0x19),At<uint8>(t,0x1A)}); return false;
}
inline uint32* __fastcall Closest(void* pf,void*,uint32* ret,float x,float y,float z,int32 type,float maxd,uint32 a,int32 b,uint32 c,uint32 d,int32 e) {
    trace.push_back({"find_node_closest_to_coors",uintptr_t(pf)==ADDR_PATHS,F(x),F(y),F(z),type,F(maxd),a&0xFFFF,b,c&0xFFFF,d&0xFFFF,e}); *ret=nodeRet; return ret;
}
inline CVector* __fastcall NodeCoors(void* pf,void*,CVector* ret,uint32 addr,bool* found) {
    trace.push_back({"find_node_coors_for_script",uintptr_t(pf)==ADDR_PATHS,addr}); *found=foundRet; *ret=nodePos; return ret;
}
inline bool __fastcall SphereVisible(void* cam,void*,const CVector* c,float r) {
    trace.push_back({"is_sphere_visible",uintptr_t(cam)==ADDR_CAMERA,F(c->x),F(c->y),F(c->z),F(r)}); return (visMask>>(visCalls++%32))&1;
}
inline bool __cdecl ClearForPed(const CVector* pos,float r,int32 maxHit,CEntity** out,bool v,bool p,bool o) {
    trace.push_back({"is_position_clear_for_ped",F(pos->x),F(pos->y),F(pos->z),F(r),maxHit,Name(uintptr_t(out)),v,p,o}); return clearRet;
}
inline void __cdecl RequestModel(int32 model,int32 flags) { trace.push_back({"request_model",model,flags}); }
inline CVehicle* __cdecl CreateCar(int32 model,float x,float y,float z,bool mission) { trace.push_back({"create_car_for_script",model,F(x),F(y),F(z),mission}); return reinterpret_cast<CVehicle*>(createRet); }
inline void __cdecl CleanUpVehicle(CVehicle* v) { trace.push_back({"clean_up_this_vehicle",Name(uintptr_t(v))}); }
inline void __fastcall RemoveEntity(void* mc,void*,int32 handle,int32 type) { trace.push_back({"remove_entity_from_list",uintptr_t(mc)==ADDR_CLEANUP,handle,type}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { CLEAR, CREATECAR, HANDSIGNAL, DUCK, FALL, CPR, GOTOPOINT };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,7> classes{{
    {"CTaskSimpleClearLookAt",0x86E404,0x8,CLEAR}, {"CTaskSimpleCreateCarAndGetIn",0x86F070,0x34,CREATECAR},
    {"CTaskSimpleDoHandSignal",0x86FA68,0xC,HANDSIGNAL}, {"CTaskSimpleDuckToggle",0x86DD98,0xC,DUCK},
    {"CTaskSimpleFall",0x870430,0x20,FALL}, {"CTaskSimpleGiveCPR",0x86F4C8,0x18,CPR}, {"CTaskSimpleGoToPoint",0x86FD50,0x30,GOTOPOINT},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleClearLookAt","Clone",0,0x86E404,1,128},{"CTaskSimpleClearLookAt","Constructor",0x634670,0x86E404,-1,64},{"CTaskSimpleClearLookAt","Destructor",0x6346A0,0x86E404,-1,64},{"CTaskSimpleClearLookAt","GetTaskType",0,0x86E404,4,64},{"CTaskSimpleClearLookAt","MakeAbortable",0,0x86E404,6,128},{"CTaskSimpleClearLookAt","ProcessPed",0,0x86E404,7,256},
    {"CTaskSimpleCreateCarAndGetIn","Clone",0,0x86F070,1,256},{"CTaskSimpleCreateCarAndGetIn","Constructor",0x6493E0,0x86F070,-1,256},{"CTaskSimpleCreateCarAndGetIn","Destructor",0x64CEA0,0x86F070,-1,256},{"CTaskSimpleCreateCarAndGetIn","GetTaskType",0,0x86F070,4,64},{"CTaskSimpleCreateCarAndGetIn","MakeAbortable",0,0x86F070,6,64},{"CTaskSimpleCreateCarAndGetIn","ProcessPed",0,0x86F070,7,2048},
    {"CTaskSimpleDoHandSignal","Clone",0x6608C0,0x86FA68,1,128},{"CTaskSimpleDoHandSignal","Constructor",0x660880,0x86FA68,-1,64},{"CTaskSimpleDoHandSignal","Destructor",0x6608B0,0x86FA68,-1,64},{"CTaskSimpleDoHandSignal","GetTaskType",0x6608A0,0x86FA68,4,64},{"CTaskSimpleDoHandSignal","MakeAbortable",0x660930,0x86FA68,6,64},{"CTaskSimpleDoHandSignal","ProcessPed",0x660940,0x86FA68,7,1024},
    {"CTaskSimpleDuckToggle","Clone",0,0x86DD98,1,128},{"CTaskSimpleDuckToggle","Constructor",0x62F690,0x86DD98,-1,128},{"CTaskSimpleDuckToggle","Destructor",0x62F6D0,0x86DD98,-1,64},{"CTaskSimpleDuckToggle","GetTaskType",0,0x86DD98,4,64},{"CTaskSimpleDuckToggle","MakeAbortable",0,0x86DD98,6,64},{"CTaskSimpleDuckToggle","ProcessPed",0,0x86DD98,7,1024},
    {"CTaskSimpleFall","Constructor",0x6782C0,0x870430,-1,256},{"CTaskSimpleFall","FinishFallAnimCB",0x6786B0,0x870430,-1,128},{"CTaskSimpleFall","MakeAbortable",0,0x870430,6,1024},{"CTaskSimpleFall","ProcessFall",0x6784C0,0x870430,-1,2048},{"CTaskSimpleFall","ProcessPed",0,0x870430,7,2048},{"CTaskSimpleFall","StartAnim",0x67CA40,0x870430,-1,1024},
    {"CTaskSimpleGiveCPR","Clone",0,0x86F4C8,1,256},{"CTaskSimpleGiveCPR","Constructor",0x658860,0x86F4C8,-1,128},{"CTaskSimpleGiveCPR","FinishGiveCPRAnimCB",0x658990,0x86F4C8,-1,128},{"CTaskSimpleGiveCPR","MakeAbortable",0,0x86F4C8,6,1024},{"CTaskSimpleGiveCPR","ProcessPed",0,0x86F4C8,7,1024},{"CTaskSimpleGiveCPR","ReviveDeadPed",0x658900,0x86F4C8,-1,512},
    {"CTaskSimpleGoToPoint","Clone",0,0x86FD50,1,512},{"CTaskSimpleGoToPoint","Constructor",0x667CD0,0x86FD50,-1,512},{"CTaskSimpleGoToPoint","GetTaskType",0,0x86FD50,4,64},{"CTaskSimpleGoToPoint","MakeAbortable",0,0x86FD50,6,1024},{"CTaskSimpleGoToPoint","ProcessPed",0,0x86FD50,7,2048},{"CTaskSimpleGoToPoint","UpdatePoint",0x645700,0x86FD50,-1,1024},
}};
// hooks que el codigo del EXE alcanza por llamada directa y que no se simulan: se desactivan en la ejecucion original
inline const std::array<std::pair<const char*,const char*>,1> baseHooks{{
    {"CTaskSimpleGoTo","QuitIK"},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Veh(0)); clean(Veh(1));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks6a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks6a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks6a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks6a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> extra;
    for(auto& [c,n]:baseHooks){ if(auto* h=FindHook(c,n))extra.push_back(h); else Write({{"event","note"},{"reason","base hook not found (se deja como esta)"},{"function",std::string(c)+"::"+n}}); }
    // direcciones DLL de los ProcessPed virtuales simulados (vtable de la DLL)
    uintptr_t dllStandStill{}, dllSetPedIn{};
    { CTaskSimpleStandStill tmp{}; dllStandStill=uintptr_t((*reinterpret_cast<void***>(&tmp))[7]); }
    { CTaskSimpleCarSetPedInAsDriver tmp(nullptr, static_cast<CTaskUtilityLineUpPedWithCar*>(nullptr)); dllSetPedIn=uintptr_t((*reinterpret_cast<void***>(&tmp))[7]); }
    // modelos: el del jugador (cargado) y un vehiculo sin cargar (estado de streaming real, solo lectura)
    const int16 pedModel=player->m_nModelIndex; int32 unloadedModel=-1;
    for(int32 m=400;m<612&&unloadedModel<0;++m)if(*reinterpret_cast<uint8*>(ADDR_STREAMINFO+m*0x14+0x10)!=1)unloadedModel=m;
    if(unloadedModel<0||!CModelInfo::GetModelInfo(pedModel)||!CModelInfo::GetModelInfo(pedModel)->GetColModel()){Write({{"event","error"},{"reason","model info missing"}});return;}
    groupsBase=*reinterpret_cast<uintptr_t*>(ADDR_GROUPS);
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&SetMoveAnim);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    FillTTraps(std::make_integer_sequence<int,16>{}); taskVtbl[2]=reinterpret_cast<void*>(&TSub); taskVtbl[4]=reinterpret_cast<void*>(&TType); taskVtbl[6]=reinterpret_cast<void*>(&TAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto& padDisable=CPad::GetPad(0)->DisablePlayerControls; const uint16 savedPad=padDisable;
    auto& maxDown=*reinterpret_cast<uint32*>(ADDR_MAXDOWN); const uint32 savedMaxDown=maxDown;
    auto& vehPool=*reinterpret_cast<uintptr_t*>(ADDR_VEHPOOL); const uintptr_t savedVehPool=vehPool;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; padDisable=savedPad; maxDown=savedMaxDown; vehPool=savedVehPool; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::vector<bool> extraEnabled,extraLocked; for(auto* h:extra){extraEnabled.push_back(h->Hooked());extraLocked.push_back(h->Locked());h->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); for(auto* h:extra)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a4(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),FirstAssoc), a4e(0x4D6A70,FirstAssoc);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a5e(0x4CEBE0,SetFinishCb);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),SetCurTime), a6e(0x4CEA80,SetCurTime);
        EntryPatch e1(bit_cast<uintptr_t>(&CEntity::GetIsOnScreen),OnScreen), e1e(0x534540,OnScreen);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), k2e(0x6181A0,IsLooking);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook), k3e(0x618280,AbortLook);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<CTask*(CTaskManager::*)(eSecondaryTask)>(&CTaskManager::GetTaskSecondary)),Secondary), t1e(0x681810,Secondary);
        EntryPatch t2(TaskManagerAccess::ChangeSlotAddress(),ChangeSlot), t2e(0x681B60,SetSecondary);
        EntryPatch t3(bit_cast<uintptr_t>(&CTaskManager::GetSimplestActiveTask),Simplest), t3e(0x6819D0,Simplest);
        EntryPatch i1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),TaskDuck), i1e(0x6010A0,TaskDuck);
        EntryPatch i2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun),TaskUseGun), i2e(0x600F70,TaskUseGun);
        EntryPatch i3(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskHold),TaskHold), i3e(0x600FF0,TaskHold);
        EntryPatch d1(bit_cast<uintptr_t>(&CTaskSimpleUseGun::ClearAnim),ClearAnim), d1e(0x61E190,ClearAnim);
        EntryPatch d2(bit_cast<uintptr_t>(&CTaskSimpleDuck::AbortBecauseOfOtherDuck),AbortOtherDuck), d2e(0x692340,AbortOtherDuck);
        EntryPatch d3(bit_cast<uintptr_t>(&CTaskSimpleDuck::SetControlType),SetControlType), d3e(0x6924F0,SetControlType);
        EntryPatch d4(bit_cast<uintptr_t>(&CTaskSimpleDuck::ControlDuckMove),DuckMove), d4e(0x6923F0,DuckMove);
        EntryPatch p1(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),PadFromPlayer), p1e(0x609560,PadFromPlayer);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p2e(0x5DEC00,SetMoveState);
        EntryPatch p3(bit_cast<uintptr_t>(&CPlayerPed::SetPlayerMoveBlendRatio),SetBlendRatio), p3e(0x60C520,SetBlendRatio);
        EntryPatch p4(bit_cast<uintptr_t>(&CPlayerPed::SetRealMoveAnim),SetRealMoveAnim), p4e(0x60A9C0,SetRealMoveAnim);
        EntryPatch p5(bit_cast<uintptr_t>(&CPlayerPed::ControlButtonSprint),Sprint), p5e(0x60A610,Sprint);
        EntryPatch g1(bit_cast<uintptr_t>(&CInterestingEvents::Add),InterestingAdd), g1e(0x602590,InterestingAdd);
        EntryPatch g2(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), g2e(0x4AB420,GroupAdd);
        EntryPatch g3(bit_cast<uintptr_t>(&CTaskSimpleGoTo::HasCircledTarget),HasCircled), g3e(0x667A10,HasCircled);
        EntryPatch g4(bit_cast<uintptr_t>(&CTaskSimpleGoTo::SetUpIK),SetUpIK), g4e(0x667AD0,SetUpIK);
        EntryPatch g5(bit_cast<uintptr_t>(&CEventScanner::ScanForEventsNow),ScanNow), g5e(0x6053D0,ScanNow);
        EntryPatch g6(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),WeaponInfoOf), g6e(0x743C60,WeaponInfoOf);
        EntryPatch g7(bit_cast<uintptr_t>(&SurfaceInfos_c::CantSprintOn),CantSprint), g7e(0x55E870,CantSprint);
        EntryPatch g8(bit_cast<uintptr_t>(static_cast<CAnimBlendStaticAssociation*(CAnimBlendAssocGroup::*)(AnimationId)>(&CAnimBlendAssocGroup::GetAnimation)),GetAnimation), g8e(0x4CE090,GetAnimation);
        EntryPatch g9(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),AngleBetween), g9e(0x53CBE0,AngleBetween);
        EntryPatch g10(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),LimitAngle), g10e(0x53CB50,LimitAngle);
        EntryPatch s1(dllStandStill,StandStill), s1e(0x62F370,StandStill);
        EntryPatch s2(dllSetPedIn,SetPedInDriver), s2e(0x64B950,SetPedInDriver);
        EntryPatch c1e(0x44F460,Closest); // la DLL llama a 0x44F460 (plugin::CallMethodAndReturn)
        EntryPatch c2(bit_cast<uintptr_t>(static_cast<CVector(CPathFind::*)(CNodeAddress,bool*)>(&CPathFind::FindNodeCoorsForScript)),NodeCoors), c2e(0x4505E0,NodeCoors);
        EntryPatch c3(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible)),SphereVisible), c3e(0x420D40,SphereVisible);
        EntryPatch c4(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,float,int32,CEntity**,bool,bool,bool)>(&CPedPlacement::IsPositionClearForPed)),ClearForPed), c4e(0x616860,ClearForPed);
        EntryPatch c5(bit_cast<uintptr_t>(&CStreaming::RequestModel),RequestModel), c5e(0x4087E0,RequestModel);
        EntryPatch c6(bit_cast<uintptr_t>(&CCarCtrl::CreateCarForScript),CreateCar), c6e(0x431F80,CreateCar);
        EntryPatch c7(bit_cast<uintptr_t>(&CTheScripts::CleanUpThisVehicle),CleanUpVehicle), c7e(0x486670,CleanUpVehicle);
        EntryPatch c8(bit_cast<uintptr_t>(static_cast<void(CMissionCleanup::*)(int32,MissionCleanUpEntityType)>(&CMissionCleanup::RemoveEntityFromList)),RemoveEntity), c8e(0x4654B0,RemoveEntity);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A6Au; if(!st)st=1; trace=json::array(); visCalls=0;
                    CleanRefs();
                    timeMs=next(); timeStep=next()%3 ? 1.f : (next()%5==0 ? (next()%2 ? 0.f : -0.5f) : uni(0.1f,3.f));
                    padDisable=next()%2 ? 0 : uint16(next());
                    { const uint32 m=next()%3; maxDown=m==0 ? 500u : m==1 ? 0u : next()%3000; }
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto* b:{&intelA,&intelB})for(auto& x:*b)x=uint8(next());
                    for(auto& b:playerData)b=uint8(next()); playerData[0x84]=uint8(next()%3==0);
                    for(auto& b:pad)b=uint8(next()); At<uint16>(pad.data(),0x10E)=next()%2 ? 0 : uint16(next());
                    for(auto& b:weaponInfo)b=uint8(next());
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next(); At(p,P_IKFLAGS)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At(p,P_PHYS_FLAGS)=next(); At<uint16>(p,P_SEED)=uint16(next()); At<int16>(p,P_MODEL)=pedModel;
                        { static const uint32 states[]{0,1,0x3F,0x3F,12,40}; At(p,P_STATE)=next()%3 ? states[next()%6] : next()%64; }
                        { static const uint32 moves[]{4,6,7,7,1}; At(p,P_MOVESTATE)=moves[next()%5]; }
                        { static const uint32 types[]{0,1,0,1,6,7,4}; At(p,P_TYPE)=types[next()%7]; }
                        { static const float hs[]{0.f,-5.f,0.5f,1.f,10.f,10.5f,50.f,100.f}; At<float>(p,P_HEALTH)=next()%3 ? hs[next()%8] : uni(0.f,150.f); }
                        At<float>(p,P_ROT_AIM)=uni(-6.5f,6.5f);
                        At<float>(p,P_MOVE_SPEED)=uni(-0.3f,0.3f); At<float>(p,P_MOVE_SPEED+4)=uni(-0.3f,0.3f); At<float>(p,P_MOVE_SPEED+8)=uni(-0.1f,0.1f);
                        At<uint8>(p,P_SURFACE)=uint8(next()%180);
                        At<void*>(p,P_PLAYERDATA)=playerData.data(); At(p,P_ANIMGROUP)=next()%16;
                        At<int8>(p,P_SLOT)=int8(next()%13);
                        for(size_t w=0;w<13;++w)At<int32>(p,P_WEAPONS+w*0x1C)=int32(next()%47);
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelB.data();
                    if(next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{};
                        m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    const CVector pedPos=PedA()->GetPosition();
                    // asociaciones de animacion falsas (floats controlados, sin NaN de senalizacion) y jerarquias
                    for(auto* a:{&anim,&anim2,&anim3}){ for(auto& b:*a)b=uint8(next());
                        static const float deltas[]{-4.f,0.f,4.f,-1000.f,1.f};
                        { static const float amounts[]{0.3f,0.f,1.f}; At<float>(a->data(),0x18)=next()%3 ? uni(0.f,1.f) : amounts[next()%3]; }
                        At<float>(a->data(),0x1C)=deltas[next()%5];
                        { static const float times[]{0.667f,0.7f,0.6f,2.f}; At<float>(a->data(),0x20)=next()%2 ? uni(0.f,2.f) : times[next()%4]; }
                        At<float>(a->data(),0x24)=uni(0.f,2.f); At<float>(a->data(),0x28)=next()%3 ? uni(0.f,0.2f) : uni(0.f,1.f);
                        At<void*>(a->data(),0x14)=next()%2 ? hier.data() : hier2.data();
                        { static const int16 ids[]{ANIM_ID_FALL_BACK,ANIM_ID_FALL_FRONT,ANIM_ID_CPR,0}; At<int16>(a->data(),0x2C)=next()%2 ? ids[next()%4] : int16(next()%300); }
                        At<uint16>(a->data(),0x2E)=uint16(next()); }
                    for(auto* h:{&hier,&hier2}){ for(auto& b:*h)b=uint8(next()); At<float>(h->data(),0x10)=next()%6 ? uni(0.f,2.6f) : At<float>(anim.data(),0x20)/0.8f; }
                    for(auto* s:{&sassoc1,&sassoc2}){ for(auto& b:*s)b=uint8(next()); }
                    At<void*>(sassoc1.data(),0x10)=hier.data(); At<void*>(sassoc2.data(),0x10)=next()%3 ? hier2.data() : hier.data();
                    // tareas falsas (tipo en +0x30; control del agachado en +0x24; animacion del sostener en +0x38)
                    for(auto* t:{&secTask,&duckS,&duckT,&useGun,&hold}){ for(auto& b:*t)b=uint8(next()); *reinterpret_cast<void**>(t->data())=taskVtbl.data(); }
                    At<int32>(secTask.data(),0x30)=next()%2 ? TASK_COMPLEX_HANDSIGNAL_ANIM : int32(next()%1000);
                    At<int32>(duckS.data(),0x30)=next()%3 ? TASK_SIMPLE_DUCK : int32(next()%1000);
                    At<int32>(duckT.data(),0x30)=TASK_SIMPLE_DUCK; At<uint8>(duckT.data(),0x24)=uint8(next()%2 ? DUCK_SCRIPT_CONTROLLED : next()%5);
                    At<int32>(useGun.data(),0x30)=TASK_SIMPLE_USE_GUN; At<int32>(hold.data(),0x30)=TASK_SIMPLE_HOLD_ENTITY;
                    At<void*>(hold.data(),0x38)=next()%2 ? nullptr : static_cast<void*>(anim.data());
                    // evento falso
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    { static const int types[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_IN_WATER,EVENT_STUCK_IN_AIR,EVENT_DEATH,EVENT_HEALTH_REALLY_LOW,EVENT_ON_FIRE,
                                               EVENT_WATER_CANNON,EVENT_VEHICLE_DAMAGE_WEAPON,EVENT_VEHICLE_DAMAGE_COLLISION,EVENT_SCRIPT_COMMAND,EVENT_REVIVE};
                      eventType=next()%5 ? types[next()%12] : int(next()%100); }
                    eventPrio=next()%3==0 ? 71 : int(next()%100);
                    // accidente falso (peaton B como objetivo)
                    for(auto& b:accident)b=uint8(next()); At<void*>(accident.data(),0)=next()%5 ? static_cast<void*>(PedB()) : nullptr;
                    // vehiculos falsos dentro del pool falso
                    std::memset(vehStore.data(),0,vehStore.size());
                    for(int k=0;k<2;++k){ auto* v=Veh(k); v->SetType(ENTITY_TYPE_VEHICLE); At<int16>(v,P_MODEL)=int16(unloadedModel);
                        uint32 fl=next(); if(next()%2)fl&=~((1u<<2)|(1u<<18)); At(v,P_FLAGS_ENT)=fl; }
                    for(auto& b:vehStates)b=uint8(next());
                    fakePool={uint32(uintptr_t(vehStore.data())),uint32(uintptr_t(vehStates.data())),2u,uint32(-1),0u,0u,0u,0u};
                    // dependencias simuladas
                    assocMask=next(); { const uint32 m=next()%3; firstRet=m==0 ? 0 : m==1 ? uintptr_t(anim.data()) : uintptr_t(anim2.data()); }
                    onScreenRet=next()%5!=0; lookingRet=next()%2; subAbortRet=next()%2; setCtrlRet=next()%2; circledRet=next()%5==0;
                    cantSprintRet=next()%4==0; { static const float sp[]{0.f,1.f,0.99f,2.f}; sprintRet=sp[next()%4]; }
                    secondaryRet=next()%3 ? uintptr_t(secTask.data()) : 0;
                    simplestRet=next()%3 ? uintptr_t(duckS.data()) : 0;
                    duckFalseRet=next()%2 ? uintptr_t(duckT.data()) : 0; duckTrueRet=next()%2 ? uintptr_t(duckT.data()) : 0;
                    useGunRet=next()%2 ? uintptr_t(useGun.data()) : 0; holdRet=next()%2 ? uintptr_t(hold.data()) : 0;
                    padRet=next()%4 ? uintptr_t(pad.data()) : 0;
                    nodeRet=next(); foundRet=next()%4!=0; nodePos=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    visMask=next()%2 ? 0u : next(); clearRet=next()%4!=0; createRet=uintptr_t(Veh(int(next()%2)));
                    At(weaponInfo.data(),0x18)=next()%4==0 ? At(weaponInfo.data(),0x18)|(1u<<9) : At(weaponInfo.data(),0x18)&~(1u<<9);
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=next()%3 ? nullptr : static_cast<void*>(secTask.data());
                    // argumentos de constructor / UpdatePoint
                    CVector vecArg{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    const int32 ctorA=next()%4 ? int32(next()%8) : int32(next()), ctorB=int32(next()%118), ctorC=next()%3 ? int32(next()%5000) : int32(next());
                    const float ctorRadius=next()%3 ? 0.5f : uni(0.f,4.f);
                    const bool ctorB0=next()%2, ctorB1=next()%2;
                    void* const ctorPtr=next()%4 ? static_cast<void*>(accident.data()) : nullptr;
                    switch(cl.kind){
                    case CLEAR: break;
                    case CREATECAR:
                        At<float>(o,8)=uni(-30.f,30.f); At<float>(o,0xC)=uni(-30.f,30.f); At<float>(o,0x10)=uni(0.f,40.f);
                        At<int32>(o,0x14)=next()%2 ? int32(pedModel) : unloadedModel;
                        { const uint32 m=next()%3; At<void*>(o,0x18)=m==0 ? nullptr : static_cast<void*>(Veh(int(m-1))); }
                        At(o,0x28)=next()%4 ? timeMs-next()%4000 : next(); At(o,0x2C)=next()%3 ? 2000u : next()%5000;
                        At<uint8>(o,0x30)=uint8(next()%2); At<uint8>(o,0x31)=uint8(next()%2);
                        break;
                    case HANDSIGNAL: At<uint8>(o,8)=uint8(next()%2); break;
                    case DUCK: { static const int32 modes[]{-1,0,1,-1,2}; At<int32>(o,8)=next()%6 ? modes[next()%5] : int32(next()); break; }
                    case FALL:
                        At<uint8>(o,8)=uint8(next()%2);
                        { static const int32 ids[]{ANIM_ID_NO_ANIMATION_SET,ANIM_ID_BIKE_FALLR,ANIM_ID_FALL_BACK,ANIM_ID_FALL_FRONT}; At<int32>(o,0xC)=next()%2 ? ids[next()%4] : int32(next()%300); }
                        At<int32>(o,0x10)=int32(next()%118);
                        { const uint32 m=next()%3; At<void*>(o,0x14)=m==0 ? nullptr : m==1 ? static_cast<void*>(anim.data()) : static_cast<void*>(anim2.data()); }
                        At<int32>(o,0x18)=int32(next()%5000);
                        { const uint32 m=next()%4; At(o,0x1C)=m==0 ? 0u : m==1 ? next()%40 : m==2 ? maxDown+next()%100 : next()%10000; }
                        if(nm=="ProcessPed"&&next()%2)At<uint8>(o,8)=1;
                        At<void*>(PedA(),P_ATTACHED)=next()%5==0 ? static_cast<void*>(Veh(0)) : nullptr;
                        break;
                    case CPR:
                        At<void*>(o,0xC)=(nm=="ProcessPed"||nm=="ReviveDeadPed"||next()%4) ? static_cast<void*>(accident.data()) : nullptr;
                        { const uint32 m=next()%3; At<void*>(o,0x10)=m==0 ? nullptr : m==1 ? static_cast<void*>(anim.data()) : static_cast<void*>(anim2.data()); }
                        At<float>(o,0x14)=uni(0.f,150.f);
                        break;
                    case GOTOPOINT: {
                        { static const int32 moves[]{4,6,7,4}; At<int32>(o,8)=next()%5 ? moves[next()%4] : int32(next()%8); }
                        const CVector tgt=next()%4 ? CVector{pedPos.x+uni(-3.f,3.f),pedPos.y+uni(-3.f,3.f),pedPos.z+(next()%3 ? uni(-1.f,1.f) : uni(-4.f,4.f))}
                                                   : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        At<float>(o,0xC)=tgt.x; At<float>(o,0x10)=tgt.y; At<float>(o,0x14)=tgt.z;
                        At<float>(o,0x18)=next()%3 ? 0.5f : uni(0.f,4.f);
                        const CVector last=next()%3 ? CVector{pedPos.x+uni(-1.f,1.f),pedPos.y+uni(-1.f,1.f),pedPos.z} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        At<float>(o,0x24)=last.x; At<float>(o,0x28)=last.y; At<float>(o,0x2C)=last.z;
                        if(nm=="ProcessPed"&&next()%4)At(PedA(),P_FLAGS3)&=~((1u<<3)|(1u<<7)); // sobre todo por el camino principal
                        if(nm=="UpdatePoint"&&next()%2){ vecArg=tgt; }
                        break; }
                    }
                    const float updRadius=(nm=="UpdatePoint"&&next()%2) ? At<float>(o,0x18) : (next()%8==0 ? std::bit_cast<float>(0x7FC00000u) : ctorRadius);
                    CPed* const pd=PedA();
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor",
                               isVoid=nm=="FinishFallAnimCB"||nm=="FinishGiveCPRAnimCB"||nm=="ProcessFall"||nm=="ReviveDeadPed"||nm=="UpdatePoint";
                    vehPool=uintptr_t(fakePool.data()); // GetRef del destructor / ProcessPed de CreateCarAndGetIn
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case CLEAR: case HANDSIGNAL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        case CREATECAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,int32)>(target)(o,&vecArg,next()%2 ? int32(pedModel) : unloadedModel)); break;
                        case DUCK: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,next()%2 ? int32(next()%3)-1 : int32(next()))); break;
                        case FALL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,int32)>(target)(o,ctorA,ctorB,ctorC)); break;
                        case CPR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,ctorPtr)); break;
                        case GOTOPOINT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,const CVector*,float,bool,bool)>(target)(o,ctorA,&vecArg,ctorRadius,ctorB0,ctorB1)); break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable"){ const void* e=(cl.kind==CPR&&prio!=ABORT_PRIORITY_IMMEDIATE) ? static_cast<const void*>(event.data()) : ev; // GiveCPR lee el evento sin comprobarlo
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,e); }
                    else if(nm=="ProcessPed"||nm=="StartAnim")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="ProcessFall"||nm=="ReviveDeadPed")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="FinishFallAnimCB"||nm=="FinishGiveCPRAnimCB"){ void* a=next()%3 ? static_cast<void*>(anim3.data()) : nullptr; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(a,o); }
                    else if(nm=="UpdatePoint")reinterpret_cast<void(__thiscall*)(void*,const CVector*,float,bool)>(target)(o,&vecArg,updRadius,next()%5==0);
                    vehPool=savedVehPool;
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Veh(0),Veh(1)})refs.push_back(refList(e));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA.data(),pedA.size())},{"pedB",EntHash(pedB.data(),pedB.size())},
                            {"veh0",EntHash(Veh(0),VEH_STRIDE)},{"veh1",EntHash(Veh(1),VEH_STRIDE)},{"vehstates",Region(vehStates.data(),vehStates.size())},
                            {"intelA",Hash(intelA.data(),intelA.size())},{"intelB",Hash(intelB.data(),intelB.size())},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS2),At(PedA(),P_FLAGS3),At(PedA(),P_FLAGS4),At(PedA(),P_IKFLAGS),At(PedA(),P_ROT_AIM),At(PedA(),P_HEALTH)}},
                            {"playerdata",Hash(playerData.data(),playerData.size())},{"pad",Hash(pad.data(),pad.size())},{"mat",Hash(mat.data(),mat.size())},
                            {"anim",Region(anim.data(),anim.size())},{"anim2",Region(anim2.data(),anim2.size())},{"anim3",Region(anim3.data(),anim3.size())},
                            {"hier",Region(hier.data(),hier.size())},{"hier2",Region(hier2.data(),hier2.size())},
                            {"sassoc",{Region(sassoc1.data(),sassoc1.size()),Region(sassoc2.data(),sassoc2.size())}},{"weaponinfo",Hash(weaponInfo.data(),weaponInfo.size())},
                            {"fake_tasks",{Region(secTask.data(),secTask.size(),true),Region(duckS.data(),duckS.size(),true),Region(duckT.data(),duckT.size(),true),
                                           Region(useGun.data(),useGun.size(),true),Region(hold.data(),hold.size(),true)}},
                            {"event",Region(event.data(),event.size(),true)},{"accident",Region(accident.data(),accident.size())},
                            {"time",timeMs},{"step",F(timeStep)},{"max_down",maxDown}};
                    // las tareas creadas se liberan sin destructor (CTaskComplexPlayHandSignalAnim toca contadores de modelos reales)
                    for(auto* t:fresh)pool->Delete(t);
                    const bool clean=Occupancy()==occ;
                    r["trace"]=trace; r["clean"]=clean; r["switches"]=switches;
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
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep||padDisable!=savedPad||maxDown!=savedMaxDown||vehPool!=savedVehPool;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<extra.size();++k){extra[k]->State(extraEnabled[k]);extra[k]->LockState(extraLocked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep&&padDisable==savedPad&&maxDown==savedMaxDown&&vehPool==savedVehPool}});
}
}
