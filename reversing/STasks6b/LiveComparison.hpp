#pragma once
// Banco dentro del motor: tareas simples B (lista de 6, 48 funciones): CTaskSimpleGunControl, CTaskSimpleHurtPedWithCar,
// CTaskSimpleIKPointArm, CTaskSimpleLand, CTaskSimplePlayerOnFire, CTaskSimpleSay, CTaskSimpleSetCharDecisionMaker y
// CTaskSimpleSetCharIgnoreWeaponRangeFlag.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada), clump (byte de tipo 2), inteligencia,
// asociaciones de animacion, entidad, dos vehiculos falsos (con matriz), tarea secundaria falsa (vtable trazada),
// CTaskSimpleUseGun/CTaskSimpleDuck/CWeaponInfo/IKChain_c/CFire/CPad falsos y evento falso en buffers propios. Se
// simulan y trazan: CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation
// (SetFinishCallback/SetDeleteCallback/SetCurrentTime), CPed::SetMoveState/Say/GetWeaponSkill, CPlayerPed::
// GetPadFromPlayer, CPad::GetSprint, CStats::GetFatAndMuscleModifier, CMatrix::SetRotateZOnly, CPedIntelligence
// (GetTaskUseGun/GetTaskDuck/GetTaskSwim/SetPedDecisionMakerType), CTaskManager::GetTaskSecondary/SetTaskSecondary
// (ChangeTaskInSlot en la DLL), CTaskSimpleUseGun (Reset/ControlGun/ControlGunMove/RequirePistolWhip),
// CTaskSimpleDuck::ForceStopMove, CWeaponInfo::GetWeaponInfo, CPedGeometryAnalyser::ComputePedHitSide,
// CEventDamage::AffectsPed, CPedDamageResponseCalculator::ComputeDamageResponse, CTaskComplexOnFire::ComputeFireDamage,
// CLocalisation::PedsOnFire, CFire::Extinguish, CPhysical::ApplyMoveForce, CEventGroup::Add, IKChainManager_c
// (AddIKChain/RemoveIKChain), IKChain_c (UpdateEntity/UpdateOffset/UpdateTarget) y el generador aleatorio
// (rand 0x821B1E y CGeneral::GetRandomNumber). Se fijan y restauran el reloj, ms_fTimeStep y CWorld::Players[0]
// (m_pPed y m_bFireProof, que PlayerOnFire lee en linea). Las tareas nuevas del pool se siguen (m_LastFreeSlot
// restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleGunControl.h"
#include "Tasks/TaskTypes/TaskSimpleHurtPedWithCar.h"
#include "Tasks/TaskTypes/TaskSimpleIKPointArm.h"
#include "Tasks/TaskTypes/TaskSimpleLand.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFire.h"
#include "Tasks/TaskTypes/TaskSimpleSay.h"
#include "Tasks/TaskTypes/TaskSimpleSetCharDecisionMaker.h"
#include "Tasks/TaskTypes/TaskSimpleSetCharIgnoreWeaponRangeFlag.h"
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskTypes/TaskComplexOnFire.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "PedDamageResponseCalculator.h"
#include "Events/EventDamage.h"
#include "Events/EventGroup.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "WeaponInfo.h"
#include "Fire.h"
#include "Localisation.h"
#include "Stats.h"
#include "../ProbeRuntime.hpp"

namespace STasks6bLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CPhysical / CEntity / CVehicle)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_MODEL=0x22, P_PHYS_FLAGS=0x40, P_MOVESPEED=0x44, P_TURNSPEED=0x50,
                 P_MASS=0x8C, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_STATE=0x530,
                 P_MOVESTATE=0x534, P_SWIMSTATE=0x538, P_HEALTH=0x540, P_ROT_CUR=0x558, P_ROT_AIM=0x55C, P_TYPE=0x598,
                 P_WEAPONS=0x5A0, P_SLOT=0x718, P_RATE=0x719, P_FIRE=0x730;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_IKMAN=0xC15448, ADDR_PLAYERS=0xB7CD98,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_LAND_CB=0x678FA0, EXE_EMPTY_STR=0x858B54;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{}, vehMat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{}, veh2{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{};
alignas(16) inline std::array<uint8,0x80> clump{}, hier{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{}, anim2{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x40> secTask{}, useGun{}, duck{}, fire{}, chain{}, pad{};
alignas(16) inline std::array<uint8,0x70> winfoA{}, winfoB{};
alignas(16) inline std::array<uint8,0x60> event{};
alignas(16) inline std::array<CVector,2> vecs{};
inline const char purposeName[]{"STasks6b"};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
inline CEntity* Veh2() { return reinterpret_cast<CEntity*>(veh2.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{}, subVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_LAND_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleLand::FinishAnimCB))return "LandFinishAnimCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==uintptr_t(purposeName))return "purpose";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(vehMat.data(),vehMat.size(),"vehmat"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(veh2.data(),veh2.size(),"veh2"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(secTask.data(),secTask.size(),"sectask"))return *s;
    if(auto s=in(useGun.data(),useGun.size(),"usegun"))return *s; if(auto s=in(duck.data(),duck.size(),"duck"))return *s;
    if(auto s=in(fire.data(),fire.size(),"fire"))return *s; if(auto s=in(chain.data(),chain.size(),"chain"))return *s;
    if(auto s=in(pad.data(),pad.size(),"pad"))return *s;
    if(auto s=in(winfoA.data(),winfoA.size(),"winfoA"))return *s; if(auto s=in(winfoB.data(),winfoB.size(),"winfoB"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(vecs.data(),sizeof(vecs),"vecs"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"))return *s;
    if(auto s=in(subVtbl.data(),sizeof(subVtbl),"subvtbl"))return *s;
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
inline uintptr_t blendRet{}, secondaryRet{}, useGunRet{}, duckRet{}, swimRet{}, chainRet{}, padRet{};
inline uint32 assocMask{}, rngState{}; inline bool subAbortRet{}, pistolRet{}, affectsRet{}, healthZeroRet{}, pedsOnFireRet{}, sprintRet{};
inline int32 hitSideRet{}, skillRet{}; inline float fatModRet{}, respHealthRet{};
inline uintptr_t lastEvent{};

// peaton: vtable falsa (todo trazado)
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1, GetEventPriority en el 2)
inline int eventType{}, eventPrio{};
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
inline int __fastcall EPrio(void* e,void*) { trace.push_back({"event_get_priority",Name(uintptr_t(e))}); return eventPrio; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// tarea secundaria falsa (GetTaskType 4 con el tipo en +0x30, MakeAbortable 6)
inline int __fastcall TType(void* t,void*) { trace.push_back({"task_get_type",Name(uintptr_t(t))}); return At<int32>(t,0x30); }
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
    }
    return r;
}

// dependencias simuladas
inline int __cdecl FakeRand() { rngState=rngState*1103515245u+12345u; const int r=int((rngState>>16)&0x7FFF); trace.push_back({"rand",r}); return r; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return (assocMask>>(id%32))&1 ? reinterpret_cast<CAnimBlendAssociation*>(anim.data()) : nullptr; }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetDeleteCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_delete_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetCurTime(CAnimBlendAssociation* a,void*,float t) { trace.push_back({"set_current_time",Name(uintptr_t(a)),F(t)}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
// CPed::Say: el EXE pasa el contexto con movzx; la DLL puede extender el signo del enum de 16 bits
inline int16 __fastcall Say(CPed* p,void*,uint32 ctx,uint32 delay,float prob,uint32 b0,uint32 b1,uint32 b2) { trace.push_back({"ped_say",Name(uintptr_t(p)),ctx&0xFFFF,delay,F(prob),b0&0xFF,b1&0xFF,b2&0xFF}); return -1; }
inline int32 __fastcall WeaponSkill(CPed* p,void*) { trace.push_back({"get_weapon_skill",Name(uintptr_t(p))}); return skillRet; }
inline CWeaponInfo* __cdecl GetWInfo(int32 type,int32 skill) { trace.push_back({"get_weapon_info",type,skill&0xFF}); return reinterpret_cast<CWeaponInfo*>(winfoB.data()); }
inline CPad* __fastcall PadFromPlayer(CPed* p,void*) { trace.push_back({"get_pad_from_player",Name(uintptr_t(p))}); return reinterpret_cast<CPad*>(padRet); }
inline bool __fastcall Sprint(CPad* p,void*) { trace.push_back({"pad_get_sprint",Name(uintptr_t(p))}); return sprintRet; }
inline float __cdecl FatMod(int32 stat) { trace.push_back({"fat_and_muscle_modifier",stat}); return fatModRet; }
inline void __fastcall RotZ(CMatrix* m,void*,float a) { trace.push_back({"set_rotate_z_only",Name(uintptr_t(m)),F(a)}); }
inline CTask* __fastcall UseGunGet(void* i,void*) { trace.push_back({"get_task_use_gun",Name(uintptr_t(i))}); return reinterpret_cast<CTask*>(useGunRet); }
inline CTask* __fastcall DuckGet(void* i,void*,uint32 b) { trace.push_back({"get_task_duck",Name(uintptr_t(i)),b&0xFF}); return reinterpret_cast<CTask*>(duckRet); }
inline CTask* __fastcall SwimGet(void* i,void*) { trace.push_back({"get_task_swim",Name(uintptr_t(i))}); return reinterpret_cast<CTask*>(swimRet); }
inline void __fastcall SetDM(void* i,void*,int32 t) { trace.push_back({"set_decision_maker",Name(uintptr_t(i)),t}); }
inline void __fastcall DuckStop(void* d,void*) { trace.push_back({"duck_force_stop_move",Name(uintptr_t(d))}); }
inline void __fastcall UseGunReset(void* t,void*,CPed* p,CEntity* e,float x,float y,float z,uint32 cmd,uint32 burst) {
    trace.push_back({"use_gun_reset",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(e)),F(x),F(y),F(z),cmd&0xFF,burst&0xFFFF});
}
inline bool __fastcall ControlGun(void* t,void*,CPed* p,CEntity* e,uint32 cmd) { trace.push_back({"use_gun_control",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(e)),cmd&0xFF}); return true; }
inline bool __fastcall ControlGunMove(void* t,void*,const CVector2D* v) { trace.push_back({"use_gun_control_move",Name(uintptr_t(t)),F(v->x),F(v->y)}); return true; }
inline bool __cdecl PistolWhip(CPed* p,CEntity* e) { trace.push_back({"require_pistol_whip",Name(uintptr_t(p)),Name(uintptr_t(e))}); return pistolRet; }
inline CTask* __fastcall Secondary(void* tm,void*,int32 idx) { trace.push_back({"get_task_secondary",Name(uintptr_t(tm)),idx}); return reinterpret_cast<CTask*>(secondaryRet); }
// EXE: CTaskManager::SetTaskSecondary(task, idx) 0x681B60; DLL: SetTaskSecondary en linea -> ChangeTaskInSlot(m_aSecondaryTasks[idx], task)
inline void __fastcall SetSecondary(void* tm,void*,CTask* t,int32 idx) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),idx}); }
inline void __fastcall ChangeSlot(void* tm,void*,CTask** slot,CTask* t) { trace.push_back({"set_task_secondary",Name(uintptr_t(tm)),Name(uintptr_t(t)),int32((uintptr_t(slot)-uintptr_t(tm)-0x14)/4)}); }
struct TaskManagerAccess : CTaskManager { static uintptr_t ChangeSlotAddress() { return std::bit_cast<uintptr_t>(&TaskManagerAccess::ChangeTaskInSlot); } };
inline int32 __cdecl HitSide(const CPed& p,const CPhysical& v) { trace.push_back({"compute_ped_hit_side",Name(uintptr_t(&p)),Name(uintptr_t(&v))}); return hitSideRet; }
inline bool __fastcall Affects(void* e,void*,CPed* p) { lastEvent=uintptr_t(e); trace.push_back({"event_affects_ped",EventFields(e),Name(uintptr_t(p))}); return affectsRet; }
inline void __fastcall ComputeResp(void* calc,void*,CPed* p,CPedDamageResponse* resp,bool speak) {
    trace.push_back({"compute_damage_response",Name(At(calc,0)),At(calc,4),At(calc,8),At(calc,0xC),At<uint8>(calc,0x10),Name(uintptr_t(p)),int32(uintptr_t(resp)-lastEvent),speak});
    resp->m_fDamageHealth=respHealthRet; resp->m_bHealthZero=healthZeroRet; resp->m_bDamageCalculated=true;
}
inline void __cdecl FireDamage(CPed* p,CPedDamageResponse& r) {
    trace.push_back({"compute_fire_damage",Name(uintptr_t(p)),F(r.m_fDamageHealth),F(r.m_fDamageArmor),r.m_bHealthZero,r.m_bForceDeath,r.m_bDamageCalculated,r.m_bCheckIfAffectsPed});
    r.m_fDamageHealth=respHealthRet; r.m_fDamageArmor=0.25f; r.m_bHealthZero=healthZeroRet; r.m_bForceDeath=false; r.m_bDamageCalculated=true; r.m_bCheckIfAffectsPed=false;
}
inline bool __cdecl PedsOnFire() { trace.push_back({"peds_on_fire"}); return pedsOnFireRet; }
inline void __fastcall Extinguish(void* f,void*) { trace.push_back({"fire_extinguish",Name(uintptr_t(f))}); }
inline void __fastcall MoveForce(CPhysical* e,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(uintptr_t(e)),F(x),F(y),F(z)}); }
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(uintptr_t(g)),EventFields(e),valid}); return nullptr; }
inline IKChain_c* __fastcall AddChain(void* m,void*,const char* nm,uint32 slot,CPed* p,int32 eff,float ex,float ey,float ez,int32 pivot,CEntity* e,int32 offBone,float ox,float oy,float oz,float speed,int32 prio) {
    trace.push_back({"ik_add_chain",Name(uintptr_t(m)),nm ? std::string(nm) : std::string("<null>"),slot&0xFF,Name(uintptr_t(p)),eff,F(ex),F(ey),F(ez),pivot,Name(uintptr_t(e)),offBone,F(ox),F(oy),F(oz),F(speed),prio});
    return reinterpret_cast<IKChain_c*>(chainRet);
}
inline void __fastcall RemoveChain(void* m,void*,void* c) { trace.push_back({"ik_remove_chain",Name(uintptr_t(m)),Name(uintptr_t(c))}); }
inline void __fastcall ChainEntity(void* c,void*,CEntity* e) { trace.push_back({"ik_chain_update_entity",Name(uintptr_t(c)),Name(uintptr_t(e))}); }
inline void __fastcall ChainOffset(void* c,void*,int32 bone,float x,float y,float z) { trace.push_back({"ik_chain_update_offset",Name(uintptr_t(c)),bone,F(x),F(y),F(z)}); }
inline void __fastcall ChainTarget(void* c,void*,uint32 b) { trace.push_back({"ik_chain_update_target",Name(uintptr_t(c)),b&0xFF}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { GUNCTRL, HURT, IKARM, LAND, ONFIRE, SAY, SETDM, IGNRANGE };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,8> classes{{
    {"CTaskSimpleGunControl",0x86D76C,0x3C,GUNCTRL}, {"CTaskSimpleHurtPedWithCar",0x86F2E4,0x14,HURT},
    {"CTaskSimpleIKPointArm",0x86E3A4,0x5C,IKARM}, {"CTaskSimpleLand",0x8704FC,0x14,LAND},
    {"CTaskSimplePlayerOnFire",0x86E334,0x18,ONFIRE}, {"CTaskSimpleSay",0x85A1DC,0x1C,SAY},
    {"CTaskSimpleSetCharDecisionMaker",0x859DA0,0xC,SETDM}, {"CTaskSimpleSetCharIgnoreWeaponRangeFlag",0x859EBC,0xC,IGNRANGE},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleGunControl","Clone",0,0x86D76C,1,256},{"CTaskSimpleGunControl","Constructor",0x61F3F0,0x86D76C,-1,256},{"CTaskSimpleGunControl","Destructor",0x61F4D0,0x86D76C,-1,128},{"CTaskSimpleGunControl","GetTaskType",0,0x86D76C,4,64},{"CTaskSimpleGunControl","MakeAbortable",0,0x86D76C,6,1024},{"CTaskSimpleGunControl","ProcessPed",0,0x86D76C,7,4096},
    {"CTaskSimpleHurtPedWithCar","Clone",0,0x86F2E4,1,256},{"CTaskSimpleHurtPedWithCar","Constructor",0x653EA0,0x86F2E4,-1,256},{"CTaskSimpleHurtPedWithCar","Destructor",0x653F30,0x86F2E4,-1,128},{"CTaskSimpleHurtPedWithCar","GetTaskType",0,0x86F2E4,4,64},{"CTaskSimpleHurtPedWithCar","MakeAbortable",0,0x86F2E4,6,64},{"CTaskSimpleHurtPedWithCar","ProcessPed",0,0x86F2E4,7,1024},
    {"CTaskSimpleIKPointArm","Clone",0,0x86E3A4,1,512},{"CTaskSimpleIKPointArm","Constructor",0x634150,0x86E3A4,-1,256},{"CTaskSimpleIKPointArm","CreateIKChain",0,0x86E3A4,9,256},{"CTaskSimpleIKPointArm","Destructor",0x634240,0x86E3A4,-1,128},{"CTaskSimpleIKPointArm","GetTaskType",0,0x86E3A4,4,64},{"CTaskSimpleIKPointArm","UpdatePointArmInfo",0x634370,0x86E3A4,-1,512},
    {"CTaskSimpleLand","Constructor",0x678E90,0x8704FC,-1,128},{"CTaskSimpleLand","FinishAnimCB",0x678FA0,0x8704FC,-1,256},{"CTaskSimpleLand","LeftFootLanded",0x679010,0x8704FC,-1,512},{"CTaskSimpleLand","MakeAbortable",0,0x8704FC,6,512},{"CTaskSimpleLand","ProcessPed",0,0x8704FC,7,2048},{"CTaskSimpleLand","RightFootLanded",0x678FE0,0x8704FC,-1,512},
    {"CTaskSimplePlayerOnFire","Clone",0,0x86E334,1,64},{"CTaskSimplePlayerOnFire","Constructor",0x633560,0x86E334,-1,64},{"CTaskSimplePlayerOnFire","Destructor",0x6335B0,0x86E334,-1,64},{"CTaskSimplePlayerOnFire","GetTaskType",0,0x86E334,4,64},{"CTaskSimplePlayerOnFire","MakeAbortable",0,0x86E334,6,64},{"CTaskSimplePlayerOnFire","ProcessPed",0,0x86E334,7,2048},
    {"CTaskSimpleSay","Clone",0,0x85A1DC,1,128},{"CTaskSimpleSay","Constructor",0x48E360,0x85A1DC,-1,256},{"CTaskSimpleSay","Destructor",0x48E4E0,0x85A1DC,-1,64},{"CTaskSimpleSay","GetTaskType",0,0x85A1DC,4,64},{"CTaskSimpleSay","MakeAbortable",0,0x85A1DC,6,64},{"CTaskSimpleSay","ProcessPed",0,0x85A1DC,7,1024},
    {"CTaskSimpleSetCharDecisionMaker","Clone",0,0x859DA0,1,128},{"CTaskSimpleSetCharDecisionMaker","Constructor",0x46A470,0x859DA0,-1,128},{"CTaskSimpleSetCharDecisionMaker","Destructor",0x46AF10,0x859DA0,-1,64},{"CTaskSimpleSetCharDecisionMaker","GetTaskType",0,0x859DA0,4,64},{"CTaskSimpleSetCharDecisionMaker","MakeAbortable",0,0x859DA0,6,64},{"CTaskSimpleSetCharDecisionMaker","ProcessPed",0,0x859DA0,7,256},
    {"CTaskSimpleSetCharIgnoreWeaponRangeFlag","Clone",0,0x859EBC,1,128},{"CTaskSimpleSetCharIgnoreWeaponRangeFlag","Constructor",0x474620,0x859EBC,-1,128},{"CTaskSimpleSetCharIgnoreWeaponRangeFlag","Destructor",0x4748F0,0x859EBC,-1,64},{"CTaskSimpleSetCharIgnoreWeaponRangeFlag","GetTaskType",0,0x859EBC,4,64},{"CTaskSimpleSetCharIgnoreWeaponRangeFlag","MakeAbortable",0,0x859EBC,6,64},{"CTaskSimpleSetCharIgnoreWeaponRangeFlag","ProcessPed",0,0x859EBC,7,256},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent()); clean(Veh()); clean(Veh2());
}
// referencia registrada en un campo del objeto probado (como si la hubiera creado su constructor)
inline bool registerRefs{};
inline void RegisterAt(void* o,size_t off,CEntity* e) { At<CEntity*>(o,off)=e; if(e&&registerRefs)e->RegisterReference(&At<CEntity*>(o,off)); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks6b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks6b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks6b");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks6b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // CEventDamage::AffectsPed de la DLL se llama directamente sobre el objeto local: su direccion sale de la vtable de la DLL (hueco 5)
    uintptr_t dllAffects{}; { CEventDamage tmp(nullptr,0,WEAPON_UNARMED,PED_PIECE_TORSO,0,false,false); dllAffects=uintptr_t((*reinterpret_cast<void***>(&tmp))[5]); }
    const int16 pedModel=player->m_nModelIndex; int16 vehModel=-1;
    for(int32 m=400;m<612&&vehModel<0;++m)if(CModelInfo::GetModelInfo(m))vehModel=int16(m);
    if(vehModel<0||!CModelInfo::GetModelInfo(pedModel)){Write({{"event","error"},{"reason","model info missing"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{});
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    FillSTraps(std::make_integer_sequence<int,16>{}); subVtbl[4]=reinterpret_cast<void*>(&TType); subVtbl[6]=reinterpret_cast<void*>(&SAbort);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final (y tras cada ejecucion el jugador 0)
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto& player0Ped=CWorld::Players[0].m_pPed; CPlayerPed* const savedPlayer0=player0Ped;
    auto& player0FireProof=At<uint8>(&CWorld::Players[0],0x14E); const uint8 savedFireProof=player0FireProof;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; player0Ped=savedPlayer0; player0FireProof=savedFireProof; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a5e(0x4CEBE0,SetFinishCb);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback),SetDeleteCb), a6e(0x4CEBC0,SetDeleteCb);
        EntryPatch a7(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),SetCurTime), a7e(0x4CEA80,SetCurTime);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::Say),Say), p2e(0x5EFFE0,Say);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),WeaponSkill), p3e(0x5E6580,WeaponSkill);
        EntryPatch p4(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),PadFromPlayer), p4e(0x609560,PadFromPlayer);
        EntryPatch p5(bit_cast<uintptr_t>(&CPad::GetSprint),Sprint), p5e(0x5407A0,Sprint);
        EntryPatch p6(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),FatMod), p6e(0x559AF0,FatMod);
        EntryPatch p7(bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly),RotZ), p7e(0x59B020,RotZ);
        EntryPatch i1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun),UseGunGet), i1e(0x600F70,UseGunGet);
        EntryPatch i2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),DuckGet), i2e(0x6010A0,DuckGet);
        EntryPatch i3(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),SwimGet), i3e(0x601070,SwimGet);
        EntryPatch i4(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),SetDM), i4e(0x600B50,SetDM);
        EntryPatch u1(bit_cast<uintptr_t>(&CTaskSimpleDuck::ForceStopMove),DuckStop), u1e(0x6924B0,DuckStop);
        EntryPatch u2(bit_cast<uintptr_t>(&CTaskSimpleUseGun::Reset),UseGunReset), u2e(0x624DC0,UseGunReset);
        EntryPatch u3(bit_cast<uintptr_t>(&CTaskSimpleUseGun::ControlGun),ControlGun), u3e(0x61E040,ControlGun);
        EntryPatch u4(bit_cast<uintptr_t>(&CTaskSimpleUseGun::ControlGunMove),ControlGunMove), u4e(0x61E0C0,ControlGunMove);
        EntryPatch u5(bit_cast<uintptr_t>(&CTaskSimpleUseGun::RequirePistolWhip),PistolWhip), u5e(0x61E200,PistolWhip);
        EntryPatch u6(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),GetWInfo), u6e(0x743C60,GetWInfo);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<CTask*(CTaskManager::*)(eSecondaryTask)>(&CTaskManager::GetTaskSecondary)),Secondary), t1e(0x681810,Secondary);
        EntryPatch t2(TaskManagerAccess::ChangeSlotAddress(),ChangeSlot), t2e(0x681B60,SetSecondary);
        EntryPatch h1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CPhysical&)>(&CPedGeometryAnalyser::ComputePedHitSide)),HitSide), h1e(0x5F3640,HitSide);
        EntryPatch h2(dllAffects,Affects), h2e(0x4B35A0,Affects);
        EntryPatch h3(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeDamageResponse),ComputeResp), h3e(0x4B5AC0,ComputeResp);
        EntryPatch h4(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),MoveForce), h4e(0x5429F0,MoveForce);
        EntryPatch h5(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), h5e(0x4AB420,GroupAdd);
        EntryPatch f1(bit_cast<uintptr_t>(&CTaskComplexOnFire::ComputeFireDamage),FireDamage), f1e(0x6333D0,FireDamage);
        EntryPatch f2(bit_cast<uintptr_t>(&CLocalisation::PedsOnFire),PedsOnFire), f2e(0x56D2D0,PedsOnFire);
        EntryPatch f3(bit_cast<uintptr_t>(&CFire::Extinguish),Extinguish), f3e(0x5393F0,Extinguish);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::AddIKChain),AddChain), k1e(0x618750,AddChain);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::RemoveIKChain),RemoveChain), k2e(0x618170,RemoveChain);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChain_c::UpdateEntity),ChainEntity), k3e(0x617E00,ChainEntity);
        EntryPatch k4(bit_cast<uintptr_t>(&IKChain_c::UpdateOffset),ChainOffset), k4e(0x617E20,ChainOffset);
        EntryPatch k5(bit_cast<uintptr_t>(&IKChain_c::UpdateTarget),ChainTarget), k5e(0x617E50,ChainTarget);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6B6Bu; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    rngState=next();
                    timeMs=next(); timeStep=next()%3 ? 1.f : (next()%5==0 ? (next()%2 ? 0.f : 0.5f) : uni(0.1f,3.f));
                    // peatones (vtable falsa, clump con tipo 2, inteligencia falsa)
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size()); std::memset(vehMat.data(),0,vehMat.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(veh.data(),0,veh.size()); std::memset(veh2.data(),0,veh2.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& x:intelA)x=uint8(next());
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At<int16>(Ent(),P_MODEL)=vehModel; Ent()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    for(auto* v:{Veh(),Veh2()}){ v->SetType(ENTITY_TYPE_VEHICLE); At<int16>(v,P_MODEL)=vehModel;
                        for(size_t k=0;k<3;++k)At<float>(v,P_MOVESPEED+4*k)=next()%4 ? uni(-2.f,2.f) : 0.f;
                        At<float>(v,P_MASS)=next()%3 ? uni(200.f,3000.f) : 1400.f; }
                    At<void*>(Veh(),P_MATRIX)=vehMat.data(); Veh2()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    { CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; std::memcpy(vehMat.data(),&m,sizeof(CMatrix)); }
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At(p,P_PHYS_FLAGS)=next(); At<int16>(p,P_MODEL)=pedModel;
                        At(p,P_MOVESTATE)=next()%3 ? 1 : next()%8; At(p,P_SWIMSTATE)=next()%3 ? 1 : next()%8;
                        { static const uint32 types[]{0,1,4,5,6,6,7,8,12,15,16,17}; At(p,P_TYPE)=types[next()%12]; }
                        At<float>(p,P_HEALTH)=uni(0.f,100.f); At<float>(p,P_ROT_CUR)=uni(-6.5f,6.5f); At<float>(p,P_ROT_AIM)=uni(-6.5f,6.5f);
                        for(size_t k=0;k<3;++k){ At<float>(p,P_MOVESPEED+4*k)=uni(-1.f,1.f); At<float>(p,P_TURNSPEED+4*k)=next()%3 ? 0.f : uni(-0.1f,0.1f); }
                        At<uint8>(p,P_SLOT)=uint8(next()%13); At<uint8>(p,P_RATE)=uint8(next()%3 ? 1+next()%120 : 1+next()%255);
                        for(size_t s=0;s<13;++s){ At<int32>(p,P_WEAPONS+s*0x1C)=int32(next()%47); At<int32>(p,P_WEAPONS+s*0x1C+4)=int32(next()%4); }
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelA.data();
                    // Land lee la matriz del peaton sin comprobarla: siempre presente en ese caso
                    if(cl.kind==LAND||next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{};
                        m.SetRotate(next()%3 ? 0.f : uni(-0.3f,0.3f),0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    // asociaciones de animacion falsas (floats controlados, sin NaN de senalizacion)
                    for(auto* a:{&anim,&anim2}){ for(auto& b:*a)b=uint8(next());
                        static const float deltas[]{-4.f,0.f,4.f,-1000.f,1.f};
                        At<void*>(a->data(),0x14)=hier.data(); At<float>(a->data(),0x18)=next()%3 ? uni(0.f,1.f) : 1.f; At<float>(a->data(),0x1C)=deltas[next()%5];
                        { const uint32 m=next()%6; At<float>(a->data(),0x20)=m==0 ? 0.1f : m==1 ? 0.2f : uni(0.f,0.45f); }
                        At<float>(a->data(),0x24)=uni(0.f,2.f); At<float>(a->data(),0x28)=next()%4 ? uni(0.f,0.3f) : 0.f;
                        { static const int16 ids[]{119,10,0,1,190,5}; At<int16>(a->data(),0x2C)=ids[next()%6]; } }
                    for(auto& b:hier)b=uint8(next());
                    // tarea secundaria falsa (tipo en +0x30)
                    for(auto& b:secTask)b=uint8(next()); *reinterpret_cast<void**>(secTask.data())=subVtbl.data();
                    { static const int32 types[]{TASK_SIMPLE_USE_GUN,TASK_SIMPLE_USE_GUN,TASK_SIMPLE_GUN_CTRL,TASK_SIMPLE_DUCK}; At<int32>(secTask.data(),0x30)=types[next()%4]; }
                    // CWeaponInfo falsos (tipo de disparo, alcance, flags, cargador)
                    for(auto* w:{&winfoA,&winfoB}){ for(auto& b:*w)b=uint8(next());
                        At<int32>(w->data(),0)=int32(next()%6); At<float>(w->data(),8)=uni(1.f,60.f); At<int16>(w->data(),0x20)=int16(int32(next()%64)-2); }
                    // CTaskSimpleUseGun falso
                    for(auto& b:useGun)b=uint8(next());
                    At<uint8>(useGun.data(),8)=uint8(next()%2); At<uint8>(useGun.data(),0xB)=uint8(next()%2); At<uint8>(useGun.data(),0xC)=uint8(next()%3==0);
                    { static const int8 cmds[]{-1,0,1,2,3,3,4,5,6}; At<int8>(useGun.data(),0x10)=cmds[next()%9]; }
                    At<void*>(useGun.data(),0x30)=next()%3 ? static_cast<void*>(winfoA.data()) : nullptr;
                    for(auto* b:{&duck,&fire,&chain,&pad})for(auto& x:*b)x=uint8(next());
                    // evento falso
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    eventType=int(next()%3 ? next()%9 : next()%100); eventPrio=next()%3 ? int(15+next()%15) : int(next()%100);
                    // dependencias simuladas
                    blendRet=(cl.kind==LAND||next()%4) ? uintptr_t(anim2.data()) : 0; // Land escribe en la asociacion devuelta
                    assocMask=next(); subAbortRet=next()%2; pistolRet=next()%3==0; affectsRet=next()%3!=0; healthZeroRet=next()%2;
                    pedsOnFireRet=next()%3!=0; sprintRet=next()%2; hitSideRet=next()%4 ? int32(next()%4) : int32(next()%256); skillRet=int32(next()%4);
                    fatModRet=uni(0.5f,1.5f); respHealthRet=uni(0.f,50.f);
                    useGunRet=next()%5 ? uintptr_t(useGun.data()) : 0; duckRet=next()%3 ? uintptr_t(duck.data()) : 0; swimRet=next()%4 ? 0 : uintptr_t(secTask.data());
                    secondaryRet=next()%4 ? uintptr_t(secTask.data()) : 0; chainRet=next()%4 ? uintptr_t(chain.data()) : 0; padRet=uintptr_t(pad.data());
                    if(cl.kind==GUNCTRL&&nm=="MakeAbortable"&&useGunRet)secondaryRet=uintptr_t(secTask.data()); // el EXE escribe en la tarea del hueco sin comprobarla
                    // jugador 0 (PlayerOnFire lo lee en linea)
                    { const uint32 m=next()%3; player0Ped=m==0 ? reinterpret_cast<CPlayerPed*>(PedA()) : m==1 ? reinterpret_cast<CPlayerPed*>(PedB()) : savedPlayer0; }
                    player0FireProof=uint8(next()%2);
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // vectores para los punteros del constructor de GunControl
                    vecs[0]=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; vecs[1]=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    CEntity* const ents[]{nullptr,Veh(),Veh2(),Ent(),PedB()};
                    registerRefs=nm!="Constructor"; // el constructor sobrescribe el objeto con 0xCD
                    switch(cl.kind){
                    case GUNCTRL: {
                        At<uint8>(o,8)=uint8(next()%5==0);
                        RegisterAt(o,0xC,ents[next()%5]);
                        if(next()%4==0){ At<float>(o,0x10)=0.f; At<float>(o,0x14)=next()%2 ? 0.f : uni(-30.f,30.f); }
                        else { At<float>(o,0x10)=uni(-30.f,30.f); At<float>(o,0x14)=uni(-30.f,30.f); }
                        At<float>(o,0x18)=uni(0.f,40.f);
                        { const uint32 m=next()%4; At<int32>(o,0x28)=m==0 ? -1 : m==1 ? 0 : int32(next()%3000); }
                        At<int16>(o,0x2C)=int16(int32(next()%12)-2);
                        { static const int8 cmds[]{-1,0,1,2,3,3,4,5,6,7,9}; At<int8>(o,0x2E)=cmds[next()%11]; }
                        At<float>(o,0x30)=next()%3 ? 1.f : uni(0.25f,2.f);
                        { const uint32 m=next()%5; At<uint32>(o,0x34)=m==0 ? 0u : m==1 ? 0xFFFFFFFFu : m==2 ? timeMs+next()%500 : timeMs-next()%500; }
                        At<uint8>(o,0x38)=uint8(next()%2); At<uint8>(o,0x39)=uint8(next()%2); At<uint8>(o,0x3A)=uint8(next()%2);
                        break; }
                    case HURT:
                        RegisterAt(o,8,next()%2 ? Veh() : Veh2()); At<float>(o,0xC)=uni(0.f,100.f); At<uint8>(o,0x10)=uint8(next()%2);
                        break;
                    case IKARM:
                        At<int32>(o,8)=next()%3 ? 999999 : -1; At<int32>(o,0xC)=int32(next()%2000);
                        At<void*>(o,0x10)=next()%3 ? static_cast<void*>(chain.data()) : nullptr;
                        At<int16>(o,0x18)=int16(next()%3 ? 24 : 34); RegisterAt(o,0x28,ents[next()%5]); At<uint8>(o,0x40)=uint8(next()%2);
                        At<int32>(o,0x2C)=next()%3 ? int32(next()%60) : int32(next());
                        for(size_t off:{size_t(0x1C),size_t(0x20),size_t(0x24),size_t(0x30),size_t(0x34),size_t(0x38),size_t(0x3C),size_t(0x44),size_t(0x4C)})At<float>(o,off)=uni(-2.f,2.f);
                        At<int32>(o,0x54)=int32(next()%2); At<int32>(o,0x58)=next()%5 ? int32(next()%2) : int32(next()%4);
                        break;
                    case LAND: {
                        At<void*>(o,8)=next()%3 ? nullptr : static_cast<void*>(anim.data());
                        { const uint32 m=next()%5; At<int32>(o,0xC)=m==0 ? -1 : m==1 ? ANIM_ID_IDLE_TIRED : m==2 ? ANIM_ID_JUMP_LAND : int32(next()%300); }
                        At<uint8>(o,0x10)=uint8((next()&0xF8)|(next()%8));
                        // jugador sin animacion: el EXE escribe en la asociacion nula (se evita)
                        if(At<uint32>(PedA(),P_TYPE)<2){ At<uint8>(o,0x10)&=~2u; if(At<int32>(o,0xC)==-1)At<int32>(o,0xC)=ANIM_ID_JUMP_LAND; }
                        if(At<int32>(o,0xC)==-1)At<uint8>(o,0x10)|=2u; else if(next()%4)At<uint8>(o,0x10)&=~2u;
                        if(next()%3==0)At<float>(mat.data(),0x28)=1.f;
                        if(next()%2)At(PedA(),P_MOVESTATE)=7;
                        break; }
                    case ONFIRE:
                        At(o,8)=next()%4 ? timeMs-next()%6000 : next(); At<int32>(o,0xC)=next()%3 ? 5000 : int32(next()%8000);
                        At<uint8>(o,0x10)=uint8(next()%3!=0); At<uint8>(o,0x11)=uint8(next()%4==0); At<uint8>(o,0x14)=uint8(next()%3==0);
                        At<void*>(PedA(),P_FIRE)=next()%5 ? static_cast<void*>(fire.data()) : nullptr;
                        break;
                    case SAY:
                        At<int32>(o,8)=next()%4 ? int32(next()%400) : int32(next()); At<int32>(o,0xC)=next()%4 ? int32(next()%5000) : -1;
                        At(o,0x10)=next()%4 ? timeMs-next()%6000 : next(); At<int32>(o,0x14)=int32(next()%5000);
                        At<uint8>(o,0x18)=uint8(next()%2); At<uint8>(o,0x19)=uint8(next()%4==0);
                        break;
                    case SETDM: At<uint32>(o,8)=next()%3 ? next()%20 : next(); break;
                    case IGNRANGE: At<uint8>(o,8)=uint8(next()%2); break;
                    }
                    CPed* const pd=PedA();
                    // argumentos de constructor y de UpdatePointArmInfo
                    CEntity* const argEnt=ents[next()%5]; const uint32 argA=next(), argB=next(), argC=next();
                    const float argF=uni(0.f,100.f), ax=uni(-2.f,2.f), ay=uni(-2.f,2.f), az=uni(-2.f,2.f);
                    const char* const argName=next()%2 ? purposeName : nullptr;
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor", isVoid=nm=="UpdatePointArmInfo"||nm=="FinishAnimCB";
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case GUNCTRL: { const CVector* t0=argA%3 ? &vecs[0] : nullptr; const CVector* t1=argB%3 ? &vecs[1] : nullptr;
                            static const uint8 cmds[]{0,1,2,3,4,5,6,7,0xFF};
                            const int32 cmd=int32((argC&0xFFFFFF00u)|cmds[argC%9]), burst=int32((argA&0xFFFF0000u)|((argB>>3)%20));
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,const CVector*,const CVector*,int32,int32,int32)>(target)(o,argEnt,t0,t1,cmd,burst,int32(argC%3 ? argC%5000 : 0xFFFFFFFFu))); break; }
                        case HURT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,float)>(target)(o,argA%3 ? (argB%2 ? Veh() : Veh2()) : nullptr,argF)); break;
                        case IKARM: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,int32,CEntity*,int32,float,float,float,float,int32)>(target)(
                                        o,argName,int32(argA%5 ? argA%2 : argA%4),argEnt,int32(argB%3 ? argB%60 : argB),ax,ay,az,argF,int32(argC%2000))); break;
                        case LAND: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,argA%4 ? int32(argB%300) : -1)); break;
                        case ONFIRE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        case SAY: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(target)(o,int32(argA%4 ? argA%400 : argA),int32(argB%4 ? argB%5000 : 0xFFFFFFFFu))); break;
                        case SETDM: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(o,argA%3 ? argA%20 : argA)); break;
                        case IGNRANGE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,int32(argA%2))); break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed"||nm=="CreateIKChain")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="LeftFootLanded"||nm=="RightFootLanded")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(o);
                    else if(nm=="UpdatePointArmInfo")reinterpret_cast<void(__thiscall*)(void*,const char*,CEntity*,int32,float,float,float,float,int32)>(target)(
                                        o,argName,argEnt,int32(argB%3 ? argB%60 : argB),ax,ay,az,argF,int32(argC%2000));
                    else if(nm=="FinishAnimCB"){ void* a=argA%4 ? static_cast<void*>(anim2.data()) : nullptr; void* d=argB%4 ? o : nullptr; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(a,d); }
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh){ const int type=int(t->GetTaskType()); json words=Region(t,0x80,true);
                        tasks.push_back({{"type",type},{"words",words}}); }
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Ent(),Veh(),Veh2()})refs.push_back(refList(e));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"veh",EntHash(veh)},{"veh2",EntHash(veh2)},{"intelA",Hash(intelA.data(),intelA.size())},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS2),At(PedA(),P_FLAGS4),At(PedA(),P_ROT_AIM),At(PedA(),P_MOVESPEED),At(PedA(),P_MOVESPEED+4),At(PedA(),P_MOVESPEED+8),
                                           At(PedA(),P_TURNSPEED),At(PedA(),P_TURNSPEED+4),At(PedA(),P_TURNSPEED+8)}},
                            {"mat",Hash(mat.data(),mat.size())},
                            {"anim",Region(anim.data(),anim.size())},{"anim2",Region(anim2.data(),anim2.size())},
                            {"sectask",Region(secTask.data(),secTask.size(),true)},{"usegun",Region(useGun.data(),useGun.size())},
                            {"buffers",{Hash(duck.data(),duck.size()),Hash(fire.data(),fire.size()),Hash(chain.data(),chain.size()),Hash(pad.data(),pad.size()),
                                        Hash(winfoA.data(),winfoA.size()),Hash(winfoB.data(),winfoB.size())}},
                            {"event",Region(event.data(),event.size(),true)},{"time",timeMs}};
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    freshIds.clear();
                    CleanRefs();
                    player0Ped=savedPlayer0; player0FireProof=savedFireProof;
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
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep||player0Ped!=savedPlayer0||player0FireProof!=savedFireProof;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep&&player0Ped==savedPlayer0&&player0FireProof==savedFireProof}});
}
}
