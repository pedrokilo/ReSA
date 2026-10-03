#pragma once
// Banco dentro del motor: tareas simples (lista de 7, 42 funciones): CTaskSimpleCarWaitForDoorNotToBeInUse,
// CTaskSimpleDie, CTaskSimpleKillPedWithCar, CTaskSimpleRunTimedAnim, CTaskSimpleSitIdle y CTaskSimpleStealthKill.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada, hueco 23 SetMoveAnim), clump (byte de
// tipo 2), inteligencia, asociaciones de animacion, jerarquia, entidad, dos vehiculos falsos (con matriz), capo falso,
// bloques y grupos de animacion falsos (se cambia temporalmente CAnimManager::ms_aAnimAssocGroups) y evento falso en
// buffers propios. Se simulan y trazan: CAnimManager::BlendAnimation (dos sobrecargas), GetAnimationBlock,
// GetAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation::SetFinishCallback/SetDeleteCallback,
// CPed::SetPedState/SetMoveState/ClearAll/StopNonPartialAnims/SetPedPositionInCar, CCarEnterExit::IsCarDoorInUse,
// IKChainManager_c (IsLooking/AbortLookAt/LookAt), CEventDamage::AffectsPed, CPedDamageResponseCalculator::
// ComputeDamageResponse, CEventGroup::Add, CPedGeometryAnalyser::ComputePedHitSide, CLocalisation::KnockDownPeds,
// CAEVehicleAudioEntity::AddAudioEvent, FindPlayerVehicle, CPad::StartShake, CPhysical::ApplyMoveForce/ApplyTurnForce,
// CAutomobile::RemoveBonnetInPedCollision y el generador aleatorio (rand 0x821B1E y CGeneral::GetRandomNumber, con
// valores forzados para los umbrales). Se fijan y restauran el reloj, ms_fTimeStep y ms_aAnimAssocGroups. Las tareas
// nuevas del pool se siguen (m_LastFreeSlot restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleCarWaitForDoorNotToBeInUse.h"
#include "Tasks/TaskTypes/TaskSimpleDie.h"
#include "Tasks/TaskTypes/TaskSimpleKillPedWithCar.h"
#include "Tasks/TaskTypes/TaskSimpleRunTimedAnim.h"
#include "Tasks/TaskTypes/TaskSimpleSitIdle.h"
#include "Tasks/TaskTypes/TaskSimpleStealthKill.h"
#include "Tasks/TaskTypes/TaskSimpleAnim.h"
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
#include "CarEnterExit.h"
#include "Localisation.h"
#include "Entity/Vehicle/Automobile.h"
#include "../ProbeRuntime.hpp"

namespace STasks7Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CPhysical / CEntity / CVehicle)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_MODEL=0x22, P_PHYS_FLAGS=0x40, P_MOVESPEED=0x44, P_MASS=0x8C,
                 P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474, P_FLAGS4=0x478, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_SHIFT=0x4D8,
                 P_IKFLAGS=0x528, P_STATE=0x530, P_HEALTH=0x540, P_ROT_AIM=0x55C, P_STANDING_ON=0x568, P_VEHICLE=0x58C,
                 P_TYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, P_DEATH_TIME=0x750, V_SUBTYPE=0x594, I_SCANNER_CLOSEST=0x124+0x4C;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_IKMAN=0xC15448, ADDR_ASSOC_GROUPS=0xB4EA34,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_RUNANIM_CB=0x61A8A0, EXE_DIE_CB=0x62FC10, EXE_STEALTH_CB=0x622790;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{}, vehMat{};
alignas(16) inline std::array<uint8,0x200> ent{}, bonnet{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{}, veh2{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{};
alignas(16) inline std::array<uint8,0x80> clump{}, hier{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{}, anim2{};
alignas(16) inline std::array<uint8,0x40> object{};
alignas(16) inline std::array<uint8,0x60> event{};
alignas(16) inline std::array<uint8,128*0x14> groups{};
alignas(16) inline std::array<uint8,8*0x20> blocks{};
inline const char animName[]{"STasks7Anim"}, blockName[]{"STasks7Block"}, taskName[]{"STasks7Task"};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
inline CEntity* Veh2() { return reinterpret_cast<CEntity*>(veh2.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_RUNANIM_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleAnim::FinishRunAnimCB))return "FinishRunAnimCB";
    if(v==EXE_DIE_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleDie::FinishAnimDieCB))return "FinishAnimDieCB";
    if(v==EXE_STEALTH_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleStealthKill::FinishAnimStealthKillCB))return "FinishAnimStealthKillCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==uintptr_t(animName))return "animname"; if(v==uintptr_t(blockName))return "blockname"; if(v==uintptr_t(taskName))return "taskname";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(vehMat.data(),vehMat.size(),"vehmat"))return *s;
    if(auto s=in(ent.data(),ent.size(),"ent"))return *s; if(auto s=in(bonnet.data(),bonnet.size(),"bonnet"))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(veh2.data(),veh2.size(),"veh2"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    if(auto s=in(hier.data(),hier.size(),"hier"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(anim2.data(),anim2.size(),"anim2"))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(groups.data(),groups.size(),"groups"))return *s; if(auto s=in(blocks.data(),blocks.size(),"blocks"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"))return *s;
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
inline uintptr_t blendRet{}, blockRet{}, getAnimRet{}, playerVehRet{}, bonnetRet{};
inline uint32 assocMask{}, rngState{}; inline bool doorRet{}, lookingRet{}, affectsRet{}, knockRet{};
inline int32 hitSideRet{};
inline std::array<int,4> forced{}; inline int nForced{}, forcedIdx{};
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
    else { r.push_back(Name(At(e,0xC))); r.push_back(Name(At(e,0x10))); } // CEventVehicleHitAndRun: victima y vehiculo
    return r;
}

// dependencias simuladas
inline int __cdecl FakeRand() {
    int r;
    if(forcedIdx<nForced)r=forced[forcedIdx++];
    else { rngState=rngState*1103515245u+12345u; r=int((rngState>>16)&0x7FFF); }
    trace.push_back({"rand",r}); return r;
}
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl BlendH(RpClump* c,void* h,int32 flags,float delta) { trace.push_back({"blend_animation_hier",Name(uintptr_t(c)),Name(uintptr_t(h)),flags,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlock* __cdecl GetBlock(const char* nm) { trace.push_back({"get_animation_block",Name(uintptr_t(nm))}); return reinterpret_cast<CAnimBlock*>(blockRet); }
inline CAnimBlendHierarchy* __cdecl GetAnim(const char* nm,const CAnimBlock* b) { trace.push_back({"get_animation",Name(uintptr_t(nm)),Name(uintptr_t(b))}); return reinterpret_cast<CAnimBlendHierarchy*>(getAnimRet); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return (assocMask>>(id%32))&1 ? reinterpret_cast<CAnimBlendAssociation*>(anim.data()) : nullptr; }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetDeleteCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_delete_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
inline void __fastcall SetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(uintptr_t(p)),s}); }
inline void __fastcall ClearAll(CPed* p,void*) { trace.push_back({"ped_clear_all",Name(uintptr_t(p))}); }
inline void __fastcall StopNonPartial(CPed* p,void*) { trace.push_back({"ped_stop_non_partial_anims",Name(uintptr_t(p))}); }
inline void __fastcall PosInCar(CPed* p,void*) { trace.push_back({"ped_set_position_in_car",Name(uintptr_t(p))}); }
inline bool __cdecl DoorInUse(const CVehicle* v,int32 d1,int32 d2) { trace.push_back({"is_car_door_in_use",Name(uintptr_t(v)),d1,d2}); return doorRet; }
inline bool __fastcall IsLooking(void* m,void*,CPed* p) { trace.push_back({"ik_is_looking",Name(uintptr_t(m)),Name(uintptr_t(p))}); return lookingRet; }
inline void __fastcall AbortLook(void* m,void*,CPed* p,uint32 t) { trace.push_back({"ik_abort_look_at",Name(uintptr_t(m)),Name(uintptr_t(p)),t}); }
inline void __fastcall LookAt(void* m,void*,const char* why,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    trace.push_back({"ik_look_at",Name(uintptr_t(m)),why ? std::string(why) : std::string("<null>"),Name(uintptr_t(p)),Name(uintptr_t(e)),time,bone,
                     off ? json{F(off->x),F(off->y),F(off->z)} : json(nullptr),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline bool __fastcall Affects(void* e,void*,CPed* p) { lastEvent=uintptr_t(e); trace.push_back({"event_affects_ped",EventFields(e),Name(uintptr_t(p))}); return affectsRet; }
inline void __fastcall ComputeResp(void* calc,void*,CPed* p,void* resp,bool speak) {
    trace.push_back({"compute_damage_response",Name(At(calc,0)),At(calc,4),At(calc,8),At(calc,0xC),At<uint8>(calc,0x10),Name(uintptr_t(p)),int32(uintptr_t(resp)-lastEvent),speak});
}
inline CEvent* __fastcall GroupAdd(void* g,void*,CEvent* e,bool valid) { trace.push_back({"event_group_add",Name(uintptr_t(g)),EventFields(e),valid}); return nullptr; }
inline int32 __cdecl HitSide(const CPed& p,const CPhysical& v) { trace.push_back({"compute_ped_hit_side",Name(uintptr_t(&p)),Name(uintptr_t(&v))}); return hitSideRet; }
inline bool __cdecl KnockDown() { trace.push_back({"knock_down_peds"}); return knockRet; }
inline void __fastcall AudioEvent(void* a,void*,int32 ev,float p) { trace.push_back({"vehicle_audio_event",Name(uintptr_t(a)),ev,F(p)}); }
inline CVehicle* __cdecl FindVehicle(int32 id,bool remote) { trace.push_back({"find_player_vehicle",id,remote}); return reinterpret_cast<CVehicle*>(playerVehRet); }
inline void __fastcall Shake(CPad* pad,void*,uint32 time,uint32 freq,uint32 delay) { trace.push_back({"pad_start_shake",uintptr_t(pad)==uintptr_t(CPad::GetPad(0)),time&0xFFFF,freq&0xFF,delay}); }
inline void __fastcall MoveForce(CPhysical* e,void*,float x,float y,float z) { trace.push_back({"apply_move_force",Name(uintptr_t(e)),F(x),F(y),F(z)}); }
inline void __fastcall TurnForce(CPhysical* e,void*,float x,float y,float z,float px,float py,float pz) { trace.push_back({"apply_turn_force",Name(uintptr_t(e)),F(x),F(y),F(z),F(px),F(py),F(pz)}); }
inline CObject* __fastcall RemoveBonnet(void* car,void*) { trace.push_back({"remove_bonnet_in_ped_collision",Name(uintptr_t(car))}); return reinterpret_cast<CObject*>(bonnetRet); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { CARWAIT, DIE, KILLCAR, TIMED, SITIDLE, STEALTH };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,6> classes{{
    {"CTaskSimpleCarWaitForDoorNotToBeInUse",0x86EDBC,0x14,CARWAIT}, {"CTaskSimpleDie",0x86DDBC,0x28,DIE},
    {"CTaskSimpleKillPedWithCar",0x86F2C0,0x10,KILLCAR}, {"CTaskSimpleRunTimedAnim",0x86D570,0x34,TIMED},
    {"CTaskSimpleSitIdle",0x86DF18,0x20,SITIDLE}, {"CTaskSimpleStealthKill",0x86DA74,0x20,STEALTH},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=42;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleCarWaitForDoorNotToBeInUse","Clone",0,0x86EDBC,1,256},{"CTaskSimpleCarWaitForDoorNotToBeInUse","Constructor",0x646B70,0x86EDBC,-1,256},{"CTaskSimpleCarWaitForDoorNotToBeInUse","Destructor",0x646C00,0x86EDBC,-1,128},{"CTaskSimpleCarWaitForDoorNotToBeInUse","GetTaskType",0,0x86EDBC,4,64},{"CTaskSimpleCarWaitForDoorNotToBeInUse","MakeAbortable",0,0x86EDBC,6,64},{"CTaskSimpleCarWaitForDoorNotToBeInUse","ProcessPed",0,0x86EDBC,7,512},{"CTaskSimpleCarWaitForDoorNotToBeInUse","SetPedPosition",0,0x86EDBC,8,512},
    {"CTaskSimpleDie","Constructor-1",0x62FA00,0x86DDBC,-1,256},{"CTaskSimpleDie","Constructor-2",0x62FA60,0x86DDBC,-1,256},{"CTaskSimpleDie","Constructor-3",0x62FAF0,0x86DDBC,-1,256},{"CTaskSimpleDie","FinishAnimDieCB",0x62FC10,0x86DDBC,-1,128},{"CTaskSimpleDie","MakeAbortable",0,0x86DDBC,6,1024},{"CTaskSimpleDie","ProcessPed",0,0x86DDBC,7,1024},{"CTaskSimpleDie","StartAnim",0x637520,0x86DDBC,-1,512},
    {"CTaskSimpleKillPedWithCar","Clone",0,0x86F2C0,1,256},{"CTaskSimpleKillPedWithCar","Constructor",0x653C10,0x86F2C0,-1,256},{"CTaskSimpleKillPedWithCar","DamageCarBonnet",0x653CF0,0x86F2C0,-1,512},{"CTaskSimpleKillPedWithCar","Destructor",0x653C90,0x86F2C0,-1,128},{"CTaskSimpleKillPedWithCar","GetTaskType",0,0x86F2C0,4,64},{"CTaskSimpleKillPedWithCar","MakeAbortable",0,0x86F2C0,6,64},{"CTaskSimpleKillPedWithCar","ProcessPed",0,0x86F2C0,7,3072},
    {"CTaskSimpleRunTimedAnim","Clone",0,0x86D570,1,256},{"CTaskSimpleRunTimedAnim","Constructor-1",0x61AB70,0x86D570,-1,256},{"CTaskSimpleRunTimedAnim","Constructor-2",0x61ABE0,0x86D570,-1,256},{"CTaskSimpleRunTimedAnim","Destructor",0x48E090,0x86D570,-1,128},{"CTaskSimpleRunTimedAnim","GetTaskType",0,0x86D570,4,64},{"CTaskSimpleRunTimedAnim","ProcessPed",0,0x86D570,7,1024},{"CTaskSimpleRunTimedAnim","StartAnim",0x61AC50,0x86D570,-1,256},
    {"CTaskSimpleSitIdle","Clone",0,0x86DF18,1,256},{"CTaskSimpleSitIdle","Constructor",0x631160,0x86DF18,-1,256},{"CTaskSimpleSitIdle","Destructor",0x6311B0,0x86DF18,-1,128},{"CTaskSimpleSitIdle","GetTaskType",0,0x86DF18,4,64},{"CTaskSimpleSitIdle","MakeAbortable",0,0x86DF18,6,512},{"CTaskSimpleSitIdle","ProcessPed",0,0x86DF18,7,2048},{"CTaskSimpleSitIdle","StartAnim",0x631260,0x86DF18,-1,512},
    {"CTaskSimpleStealthKill","Clone",0,0x86DA74,1,256},{"CTaskSimpleStealthKill","Constructor",0x6225F0,0x86DA74,-1,256},{"CTaskSimpleStealthKill","FinishAnimStealthKillCB",0x622790,0x86DA74,-1,256},{"CTaskSimpleStealthKill","GetTaskType",0,0x86DA74,4,64},{"CTaskSimpleStealthKill","MakeAbortable",0,0x86DA74,6,1024},{"CTaskSimpleStealthKill","ManageAnim",0x6296D0,0x86DA74,-1,1024},{"CTaskSimpleStealthKill","ProcessPed",0,0x86DA74,7,1024},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent()); clean(Veh()); clean(Veh2());
}
// referencia registrada en un campo del objeto probado (como si la hubiera creado su constructor)
inline bool registerRefs{};
inline void RegisterAt(void* o,size_t off,CEntity* e) { At<CEntity*>(o,off)=e; if(e&&registerRefs)e->RegisterReference(&At<CEntity*>(o,off)); }
// matriz con ejes dados (el adelante puede tener una z exacta como -0.2f o 0.1f)
inline void WriteMatrix(void* dst,float fz,float rz,const CVector& pos) {
    CMatrix m{}; const float c=std::sqrt(1.f-fz*fz), cs=std::cos(rz), sn=std::sin(rz);
    m.GetRight()=CVector{cs,sn,0.f}; m.GetForward()=CVector{-sn*c,cs*c,fz}; m.GetUp()=CVector{sn*fz,-cs*fz,c}; m.GetPosition()=pos;
    std::memcpy(dst,&m,sizeof(CMatrix));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks7-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks7");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks7-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // CEventDamage::AffectsPed de la DLL se llama directamente sobre el objeto local: su direccion sale de la vtable de la DLL (hueco 5)
    uintptr_t dllAffects{}; { CEventDamage tmp(nullptr,0,WEAPON_UNARMED,PED_PIECE_TORSO,0,false,false); dllAffects=uintptr_t((*reinterpret_cast<void***>(&tmp))[5]); }
    // modelo de vehiculo con modelo de colision (ProcessPed de KillPedWithCar lee su caja)
    const int16 pedModel=player->m_nModelIndex; int16 vehModel=-1;
    for(int32 m=400;m<612&&vehModel<0;++m)if(auto* mi=CModelInfo::GetModelInfo(m);mi&&mi->GetColModel())vehModel=int16(m);
    if(vehModel<0||!CModelInfo::GetModelInfo(pedModel)){Write({{"event","error"},{"reason","model info missing"}});return;}
    const auto& bb=CModelInfo::GetModelInfo(vehModel)->GetColModel()->GetBoundingBox();
    const float bbMinY=bb.m_vecMin.y, bbMaxX=bb.m_vecMax.x, bbMaxY=bb.m_vecMax.y, bbMaxZ=bb.m_vecMax.z;
    if(*reinterpret_cast<uint32*>(0xB4EA28)<8){Write({{"event","error"},{"reason","anim assoc definitions missing"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&SetMoveAnim);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final (los grupos de animacion tambien tras cada llamada)
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto& assocGroups=*reinterpret_cast<void**>(ADDR_ASSOC_GROUPS); void* const savedGroups=assocGroups;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; assocGroups=savedGroups; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,CAnimBlendHierarchy*,int32,float)>(&CAnimManager::BlendAnimation)),BlendH), a2e(0x4D4410,BlendH);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a4(bit_cast<uintptr_t>(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),GetBlock), a4e(0x4D3940,GetBlock);
        EntryPatch a5(bit_cast<uintptr_t>(static_cast<CAnimBlendHierarchy*(*)(const char*,const CAnimBlock*)>(&CAnimManager::GetAnimation)),GetAnim), a5e(0x4D42F0,GetAnim);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a6e(0x4CEBE0,SetFinishCb);
        EntryPatch a7(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback),SetDeleteCb), a7e(0x4CEBC0,SetDeleteCb);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::SetPedState),SetPedState), p2e(0x5E4500,SetPedState);
        EntryPatch p3(bit_cast<uintptr_t>(&CPed::ClearAll),ClearAll), p3e(0x5E5320,ClearAll);
        EntryPatch p4(bit_cast<uintptr_t>(&CPed::StopNonPartialAnims),StopNonPartial), p4e(0x5DED10,StopNonPartial);
        EntryPatch p5(bit_cast<uintptr_t>(&CPed::SetPedPositionInCar),PosInCar), p5e(0x5DF910,PosInCar);
        EntryPatch c1(bit_cast<uintptr_t>(&CCarEnterExit::IsCarDoorInUse),DoorInUse), c1e(0x64EC90,DoorInUse);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking),IsLooking), k1e(0x6181A0,IsLooking);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt),AbortLook), k2e(0x618280,AbortLook);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChainManager_c::LookAt),LookAt), k3e(0x618970,LookAt);
        EntryPatch g1(dllAffects,Affects), g1e(0x4B35A0,Affects);
        EntryPatch g2(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeDamageResponse),ComputeResp), g2e(0x4B5AC0,ComputeResp);
        EntryPatch g3(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),GroupAdd), g3e(0x4AB420,GroupAdd);
        EntryPatch h1(bit_cast<uintptr_t>(static_cast<int32(*)(const CPed&,const CPhysical&)>(&CPedGeometryAnalyser::ComputePedHitSide)),HitSide), h1e(0x5F3640,HitSide);
        EntryPatch h2(bit_cast<uintptr_t>(&CLocalisation::KnockDownPeds),KnockDown), h2e(0x56D290,KnockDown);
        EntryPatch h3(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),AudioEvent), h3e(0x4F6420,AudioEvent);
        EntryPatch h4(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FindVehicle), h4e(0x56E0D0,FindVehicle);
        EntryPatch h5(bit_cast<uintptr_t>(&CPad::StartShake),Shake), h5e(0x53F920,Shake);
        EntryPatch h6(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),MoveForce), h6e(0x5429F0,MoveForce);
        EntryPatch h7(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),TurnForce), h7e(0x542A50,TurnForce);
        EntryPatch h8(bit_cast<uintptr_t>(&CAutomobile::RemoveBonnetInPedCollision),RemoveBonnet), h8e(0x6AA200,RemoveBonnet);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7777u; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    rngState=next(); forcedIdx=0; nForced=0;
                    timeMs=next(); timeStep=next()%3 ? 1.f : (next()%5==0 ? (next()%2 ? 0.f : 0.5f) : uni(0.1f,3.f));
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size()); std::memset(vehMat.data(),0,vehMat.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(veh.data(),0,veh.size()); std::memset(veh2.data(),0,veh2.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& x:intelA)x=uint8(next());
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At(Ent(),P_PHYS_FLAGS)=next(); At<int16>(Ent(),P_MODEL)=vehModel; Ent()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    // vehiculos (Veh con matriz y modelo con colision; Veh2 sin matriz)
                    for(auto* v:{Veh(),Veh2()}){ v->SetType(ENTITY_TYPE_VEHICLE); At<int16>(v,P_MODEL)=vehModel; At(v,P_PHYS_FLAGS)=next();
                        { static const uint32 subs[]{0,0,0,1,2,9}; At(v,V_SUBTYPE)=subs[next()%6]; }
                        { static const float masses[]{1400.f,700.f,2800.f}; At<float>(v,P_MASS)=next()%3 ? uni(200.f,3000.f) : masses[next()%3]; } }
                    At<void*>(Veh(),P_MATRIX)=vehMat.data(); Veh2()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    const CVector carPos{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    { const uint32 m=next()%6;
                      if(m==0)WriteMatrix(vehMat.data(),0.f,0.f,carPos);                         // identidad exacta
                      else if(m==1)WriteMatrix(vehMat.data(),-0.2f,uni(-3.f,3.f),carPos);        // z del adelante exacta en el umbral
                      else if(m==2)WriteMatrix(vehMat.data(),0.1f,uni(-3.f,3.f),carPos);
                      else WriteMatrix(vehMat.data(),next()%2 ? 0.f : uni(-0.7f,0.7f),uni(-3.f,3.f),carPos); }
                    const CMatrix& cm=*reinterpret_cast<CMatrix*>(vehMat.data());
                    { const uint32 m=next()%5; CVector v{uni(-2.f,2.f),uni(-2.f,2.f),uni(-0.5f,0.5f)};
                      if(m==0)v=CVector{0.f,0.f,0.f}; else if(m==1)v=CVector{v.x,0.f,v.z}; else if(m==2)v=cm.GetForward()*uni(-0.1f,0.1f);
                      for(size_t k=0;k<3;++k)At<float>(Veh(),P_MOVESPEED+4*k)=(&v.x)[k]; }
                    for(size_t k=0;k<3;++k)At<float>(Veh2(),P_MOVESPEED+4*k)=uni(-2.f,2.f);
                    // peatones (vtable falsa, clump con tipo 2, inteligencia falsa)
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next(); At(p,P_IKFLAGS)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At(p,P_PHYS_FLAGS)=next(); At<int16>(p,P_MODEL)=pedModel;
                        { static const uint32 types[]{0,1,4,5,6,6,7,8}; At(p,P_TYPE)=types[next()%8]; }
                        At<float>(p,P_HEALTH)=uni(0.f,100.f); At(p,P_DEATH_TIME)=next(); At<float>(p,P_ROT_AIM)=uni(-6.5f,6.5f);
                        for(size_t k=0;k<3;++k)At<float>(p,P_MOVESPEED+4*k)=uni(-1.f,1.f);
                        At<float>(p,P_SHIFT)=uni(-1.f,1.f); At<float>(p,P_SHIFT+4)=uni(-1.f,1.f); At<int32>(p,P_ANIMGROUP)=int32(next()%8);
                        At<uint8>(p,P_SLOT)=uint8(next()%13);
                        for(size_t s=0;s<13;++s)At<int32>(p,P_WEAPONS+s*0x1C)=int32(next()%47);
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelA.data();
                    // posicion del peaton: relativa al coche (KillPedWithCar) o con matriz (SitIdle usa los vectores de la matriz)
                    CVector pedPos{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                    if(cl.kind==KILLCAR){ const uint32 m=next()%4; float lx,ly,lz;
                        if(m==0){ lx=(next()%2 ? 1.f : -1.f)*bbMaxX*uni(1.f,1.6f); ly=uni(bbMinY*1.2f,bbMaxY*1.2f); lz=uni(-0.5f,1.5f); }
                        else if(m==1){ lx=uni(-bbMaxX*0.9f,bbMaxX*0.9f); ly=uni(bbMinY,bbMaxY); lz=uni(0.15f,bbMaxZ+1.f); }
                        else if(m==2){ lx=uni(-bbMaxX*0.9f,bbMaxX*0.9f); ly=uni(bbMinY,bbMaxY); lz=uni(-1.f,0.09f); }
                        else { lx=uni(-3.f,3.f); ly=uni(-4.f,4.f); lz=uni(-1.f,2.f); }
                        pedPos=carPos+cm.GetRight()*lx+cm.GetForward()*ly+cm.GetUp()*lz; }
                    if(cl.kind==SITIDLE||cl.kind==KILLCAR||next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); WriteMatrix(mat.data(),0.f,uni(-3.f,3.f),pedPos); }
                    else PedA()->SetPosn(pedPos.x,pedPos.y,pedPos.z);
                    { const CVector b=next()%3 ? pedPos+CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-0.5f,0.5f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; PedB()->SetPosn(b.x,b.y,b.z); }
                    // asociaciones de animacion falsas (floats controlados, sin NaN de senalizacion) y jerarquia
                    for(auto* a:{&anim,&anim2}){ for(auto& b:*a)b=uint8(next());
                        static const float deltas[]{-4.f,0.f,4.f,-1000.f,1.f};
                        At<void*>(a->data(),0x14)=hier.data(); At<float>(a->data(),0x18)=next()%3 ? uni(0.f,1.f) : 0.f; At<float>(a->data(),0x1C)=deltas[next()%5];
                        At<float>(a->data(),0x20)=uni(0.f,2.f); At<float>(a->data(),0x24)=uni(0.f,2.f); At<float>(a->data(),0x28)=uni(0.f,0.3f);
                        { static const int16 ids[]{348,349,350,348,349,350,0,191}; At<int16>(a->data(),0x2C)=ids[next()%8]; } }
                    for(auto& b:hier)b=uint8(next());
                    // grupos y bloques de animacion falsos (el indice del grupo se mantiene en [0, 8))
                    for(auto& b:groups)b=uint8(next()); for(auto& b:blocks)b=uint8(next());
                    for(size_t g=0;g<8;++g){ const uint32 m=next()%4; At<void*>(groups.data(),g*0x14)=m==0 ? nullptr : static_cast<void*>(blocks.data()+g*0x20);
                        At<uint8>(blocks.data(),g*0x20+0x10)=uint8(next()%3!=0); }
                    // evento falso (CEventDamage: causante en +0x14)
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    { static const int types[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_DEATH,EVENT_SCRIPT_COMMAND}; eventType=types[next()%4]; }
                    eventPrio=int(next()%100);
                    { const uint32 m=next()%4; At<void*>(event.data(),0x14)=m<2 ? static_cast<void*>(PedB()) : m==2 ? static_cast<void*>(PedA()) : nullptr; }
                    // dependencias simuladas
                    blendRet=(cl.kind==DIE||cl.kind==STEALTH||next()%4) ? uintptr_t(anim2.data()) : 0; // Die escribe en la asociacion devuelta
                    blockRet=next()%4 ? uintptr_t(blocks.data()) : 0; getAnimRet=next()%4 ? uintptr_t(hier.data()) : 0;
                    assocMask=next(); doorRet=next()%2; lookingRet=next()%3==0; affectsRet=next()%3!=0; knockRet=next()%3!=0;
                    hitSideRet=next()%5 ? int32(next()%4) : int32(next()%512);
                    playerVehRet=next()%3==0 ? uintptr_t(Veh()) : next()%2 ? uintptr_t(Veh2()) : 0;
                    for(auto& b:bonnet)b=uint8(next()); for(size_t k=0;k<3;++k)At<float>(bonnet.data(),P_MOVESPEED+4*k)=uni(-1.f,1.f);
                    bonnetRet=next()%4 ? uintptr_t(bonnet.data()) : 0;
                    At<void*>(intelA.data(),I_SCANNER_CLOSEST)=next()%3 ? static_cast<void*>(PedB()) : nullptr; // CPedScanner::GetClosestPedInRange
                    // valores aleatorios forzados en los umbrales (el resto sale del LCG)
                    { static const int edges[]{16383,16384,1638,1639,0,32767,8191,8192};
                      nForced=int(next()%3); for(int k=0;k<nForced;++k)forced[size_t(k)]=next()%2 ? edges[next()%8] : int(next()%2000); }
                    if(cl.kind==SITIDLE&&nm=="ProcessPed"&&next()%2){ nForced=3; forced[0]=int(next()%1700); forced[1]=int(next()%32768); forced[2]=next()%2 ? int(next()%9000) : int(next()%32768); }
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    registerRefs=!nm.starts_with("Constructor"); // el constructor sobrescribe el objeto con 0xCD
                    CEntity* const vehs[]{Veh(),Veh2(),nullptr,Veh()};
                    auto timer=[&](size_t off,uint32 interval){ At(o,off)=next()%4 ? timeMs-next()%6000 : next(); At<int32>(o,off+4)=int32(interval);
                        At<uint8>(o,off+8)=uint8(next()%4!=0); At<uint8>(o,off+9)=uint8(next()%4==0); };
                    switch(cl.kind){
                    case CARWAIT: {
                        RegisterAt(o,8,vehs[next()%4]);
                        At<uint32>(o,0xC)=next()%4 ? 8+next()%4 : next(); At<uint32>(o,0x10)=next()%3 ? 0 : 8+next()%4;
                        { const uint32 m=next()%4; At<void*>(PedA(),P_VEHICLE)=m==0 ? At<void*>(o,8) : m==1 ? static_cast<void*>(Veh2()) : m==2 ? nullptr : static_cast<void*>(Veh()); }
                        break; }
                    case DIE: {
                        At<int32>(o,8)=int32(next()%118);
                        { const uint32 m=next()%4; At<int32>(o,0xC)=m==0 ? ANIM_ID_NO_ANIMATION_SET : m==1 ? ANIM_ID_DROWN : int32(next()%300); }
                        At<void*>(o,0x10)=next()%3 ? nullptr : static_cast<void*>(hier.data()); At(o,0x14)=next()%0x10000;
                        At<float>(o,0x18)=next()%3 ? 4.f : uni(0.f,8.f);
                        { const uint32 m=next()%4; At<float>(o,0x1C)=m==0 ? 0.f : m==1 ? -1.f : uni(0.f,2.f); }
                        At<void*>(o,0x24)=next()%3 ? nullptr : static_cast<void*>(anim.data());
                        { const uint32 m=next()%4; At<void*>(PedA(),P_STANDING_ON)=m==0 ? nullptr : m==1 ? static_cast<void*>(Veh2()) : static_cast<void*>(Ent()); }
                        break; }
                    case KILLCAR:
                        RegisterAt(o,8,(nm=="ProcessPed"||nm=="DamageCarBonnet") ? Veh() : vehs[next()%4]); At<float>(o,0xC)=uni(0.f,100.f);
                        break;
                    case TIMED: {
                        At<void*>(o,8)=next()%3 ? nullptr : static_cast<void*>(anim.data()); At<uint8>(o,0xC)=uint8(next()%4 ? next()&0xFE : next());
                        At<int32>(o,0x10)=int32(next()%118); At<int32>(o,0x14)=int32(next()%300); At<float>(o,0x18)=next()%3 ? 4.f : uni(-8.f,8.f); At<float>(o,0x1C)=uni(-2.f,2.f);
                        const uint32 dur=next()%5 ? next()%8000 : next(); At(o,0x20)=dur; timer(0x24,next()%4 ? next()%8000 : next());
                        At<uint16>(o,0x30)=uint16(next()%3 ? TASK_SIMPLE_TIMED_ANIM : next());
                        break; }
                    case SITIDLE: {
                        At<uint8>(o,8)=uint8(next()%2); At<int32>(o,0xC)=next()%5 ? int32(next()%8000) : int32(next());
                        At<void*>(o,0x10)=next()%3 ? static_cast<void*>(anim.data()) : nullptr; timer(0x14,next()%4 ? next()%8000 : next());
                        break; }
                    case STEALTH: {
                        At<uint8>(o,8)=uint8(next()%2); RegisterAt(o,0xC,(nm=="ManageAnim"||next()%5) ? static_cast<CEntity*>(PedB()) : nullptr); // ManageAnim lee el objetivo sin comprobarlo (el EXE se cae con nulo)
                        At<int32>(o,0x10)=int32(next()%8); At<uint8>(o,0x14)=uint8(next()%5==0); At<uint8>(o,0x15)=uint8(next()%2);
                        At<void*>(o,0x18)=next()%2 ? static_cast<void*>(anim.data()) : nullptr;
                        At<uint32>(o,0x1C)=next()%3 ? 10000-next()%200 : next()%20000;
                        break; }
                    }
                    CPed* const pd=PedA();
                    // argumentos de constructor
                    const uint32 argA=next(), argB=next(), argC=next(); const float argF=next()%3 ? 4.f : uni(-8.f,8.f), argG=next()%3 ? uni(0.f,2.f) : (next()%2 ? 0.f : -1.f);
                    const auto occ=Occupancy();
                    if(cl.kind==STEALTH)assocGroups=groups.data();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm.starts_with("Constructor"), isDtor=nm=="Destructor",
                               isVoid=nm=="StartAnim"||nm=="ManageAnim"||nm=="DamageCarBonnet"||nm=="FinishAnimDieCB"||nm=="FinishAnimStealthKillCB";
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case CARWAIT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,uint32,uint32)>(target)(o,vehs[argA%4],argB%4 ? 8+argB%4 : argB,argC%3 ? 0 : argC)); break;
                        case DIE:
                            if(nm=="Constructor-1")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,float,float)>(target)(o,int32(argA%118),int32(argB%300),argF,argG));
                            else if(nm=="Constructor-2")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,const char*,int32,float,float)>(target)(o,animName,blockName,int32(argC%0x10000),argF,argG));
                            else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*,int32,float,float)>(target)(o,argA%4 ? static_cast<void*>(hier.data()) : nullptr,int32(argC%0x10000),argF,argG));
                            break;
                        case KILLCAR: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,float)>(target)(o,vehs[argA%4],argG)); break;
                        case TIMED: {
                            const uint32 dur=argB%5 ? argB%8000 : argB; const bool hold=argC%2;
                            if(nm=="Constructor-1")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,float,float,uint32,bool)>(target)(o,int32(argA%118),int32(argA%300),argF,argG,dur,hold));
                            else rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,int32,float,float,uint32,int32,const char*,bool)>(target)(o,int32(argA%118),int32(argA%300),argF,argG,dur,int32(argC),argA%2 ? taskName : nullptr,hold));
                            break; }
                        case SITIDLE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,bool)>(target)(o,argA%5 ? int32(argA%8000) : int32(argA),argB%2)); break;
                        case STEALTH: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,bool,CEntity*,int32)>(target)(o,argA%2,argB%4 ? static_cast<CEntity*>(PedB()) : nullptr,int32(argC%8))); break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed"||nm=="SetPedPosition")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="StartAnim"||nm=="ManageAnim"||nm=="DamageCarBonnet")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="FinishAnimDieCB"||nm=="FinishAnimStealthKillCB")reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(anim2.data(),o);
                    setState(false);
                    assocGroups=savedGroups;
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Ent(),Veh(),Veh2()})refs.push_back(refList(e));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"veh",EntHash(veh)},{"veh2",EntHash(veh2)},{"intelA",Hash(intelA.data(),intelA.size())},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS2),At(PedA(),P_FLAGS4),At(PedA(),P_IKFLAGS),At(PedA(),P_ROT_AIM),At(PedA(),P_HEALTH),At(PedA(),P_DEATH_TIME),
                                           At(PedA(),P_MOVESPEED),At(PedA(),P_MOVESPEED+4),At(PedA(),P_MOVESPEED+8),At(PedA(),P_SHIFT),At(PedA(),P_SHIFT+4)}},
                            {"mat",Hash(mat.data(),mat.size())},{"bonnet",Region(bonnet.data(),bonnet.size())},
                            {"anim",Region(anim.data(),anim.size())},{"anim2",Region(anim2.data(),anim2.size())},{"hier",Hash(hier.data(),hier.size())},
                            {"anim_data",{Hash(groups.data(),groups.size()),Hash(blocks.data(),blocks.size())}},
                            {"event",Region(event.data(),event.size(),true)},{"rand_used",forcedIdx},{"time",timeMs}};
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
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep||assocGroups!=savedGroups;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep&&assocGroups==savedGroups}});
}
}
