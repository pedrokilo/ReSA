#pragma once
// Banco dentro del motor: tareas simples A (lista de 8, 40 funciones): CTaskSimpleEvasiveDive, CTaskSimpleEvasiveStep,
// CTaskSimpleGoToPointFine, CTaskSimpleIKLookAt y CTaskSimpleIKManager.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada, SetMoveAnim en el hueco 23), clump (byte
// de tipo 2), inteligencia, entidad, dos vehiculos falsos (conductor en 0x460), nueve asociaciones de animacion falsas
// (vtable trazada: el borrado pasa por el hueco 0), CWanted/IKChain_c/CTaskSimpleDuck falsos, ocho tareas de IK falsas
// (vtable trazada: borrado, Clone y ProcessPed) y evento falso en buffers propios. Se simulan y trazan:
// CAnimManager::BlendAnimation/AddAnimation, RpAnimBlendClumpGetAssociation, CAnimBlendAssociation
// (SetFinishCallback/SetCurrentTime), CPed::Say/SetMoveState/IsPlayer, FindPlayerWanted,
// CWanted::RegisterCrime_Immediately, CPedIntelligence::GetTaskDuck, CTaskSimpleDuck::ControlDuckMove,
// CTaskSimpleGoTo (HasCircledTarget/QuitIK/SetUpIK), CGeneral (GetRadianAngleBetweenPoints/LimitRadianAngle),
// IKChainManager_c (AddIKChain/RemoveIKChain) e IKChain_c (UpdateEntity/UpdateOffset/UpdateTarget/ClampLimits).
// Se fijan y restauran el reloj y ms_fTimeStep. Las tareas nuevas del pool se siguen (m_LastFreeSlot restaurado y
// huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleEvasiveDive.h"
#include "Tasks/TaskTypes/TaskSimpleEvasiveStep.h"
#include "Tasks/TaskTypes/TaskSimpleGoToPointFine.h"
#include "Tasks/TaskTypes/TaskSimpleGoTo.h"
#include "Tasks/TaskTypes/TaskSimpleIKLookAt.h"
#include "Tasks/TaskTypes/TaskSimpleIKManager.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Wanted.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace STasks8aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed / CEntity / CVehicle)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_MODEL=0x22, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474,
                 P_FLAGS4=0x478, P_INTEL=0x47C, P_ANIMGROUP=0x4D4, P_MOVESTATE=0x534, P_SWIMSTATE=0x538, P_ROT_AIM=0x55C,
                 P_TYPE=0x598, V_DRIVER=0x460;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_IKMAN=0xC15448,
                    EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_DIVE_CB=0x6536A0, EXE_STEP_CB=0x653290, EXE_EMPTY_STR=0x858B54;
constexpr size_t NASSOC=9, NIKT=8;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> veh{}, veh2{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelA{};
alignas(16) inline std::array<uint8,0x80> clump{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,NASSOC> assoc{};
alignas(16) inline std::array<std::array<uint8,0x60>,NIKT> ikTask{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x40> wanted{}, chain{}, duck{}, outVec{};
alignas(16) inline std::array<uint8,0x60> event{};
inline const char purposeName[]{"STasks8a"};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
inline CEntity* Veh2() { return reinterpret_cast<CEntity*>(veh2.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{}, assocVtbl{}, ikVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_DIVE_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleEvasiveDive::FinishAnimEvasiveDiveCB))return "DiveFinishCB";
    if(v==EXE_STEP_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleEvasiveStep::FinishAnimEvasiveStepCB))return "StepFinishCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==uintptr_t(purposeName))return "purpose";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const std::string& nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return nm+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(ent.data(),ent.size(),"ent"))return *s;
    if(auto s=in(veh.data(),veh.size(),"veh"))return *s; if(auto s=in(veh2.data(),veh2.size(),"veh2"))return *s;
    if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s; if(auto s=in(clump.data(),clump.size(),"clump"))return *s;
    for(size_t k=0;k<NASSOC;++k)if(auto s=in(assoc[k].data(),assoc[k].size(),"assoc"+std::to_string(k)))return *s;
    for(size_t k=0;k<NIKT;++k)if(auto s=in(ikTask[k].data(),ikTask[k].size(),"ikt"+std::to_string(k)))return *s;
    if(auto s=in(object.data(),object.size(),"object"))return *s; if(auto s=in(wanted.data(),wanted.size(),"wanted"))return *s;
    if(auto s=in(chain.data(),chain.size(),"chain"))return *s; if(auto s=in(duck.data(),duck.size(),"duck"))return *s;
    if(auto s=in(outVec.data(),outVec.size(),"outvec"))return *s; if(auto s=in(event.data(),event.size(),"event"))return *s;
    if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s; if(auto s=in(eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"))return *s;
    if(auto s=in(assocVtbl.data(),sizeof(assocVtbl),"assocvtbl"))return *s; if(auto s=in(ikVtbl.data(),sizeof(ikVtbl),"ikvtbl"))return *s;
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
inline uintptr_t blendRet{}, duckRet{}, chainRet{};
inline uint32 assocMask{}, ikProcMask{}; inline bool circledRet{};
inline float angleRet{}, limitRet{};

// peaton: vtable falsa (todo trazado; SetMoveAnim en el hueco 23)
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline void __fastcall SetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(uintptr_t(p))}); }
// evento falso (GetEventType en el hueco 1)
inline int eventType{};
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
// asociacion falsa: el borrado (delete) llama al hueco 0
inline void* __fastcall ADel(void* a,void*,uint32 flags) { trace.push_back({"assoc_delete",Name(uintptr_t(a)),flags}); return a; }
template<int N> void* __fastcall ATrap(void*,void*) { trace.push_back({"assoc_unexpected_slot",N}); return nullptr; }
template<int... N> void FillATraps(std::integer_sequence<int,N...>) { ((assocVtbl[N]=reinterpret_cast<void*>(&ATrap<N>)),...); }
// tareas de IK falsas: borrado (hueco 0), Clone (hueco 1, devuelve la gemela 4..7) y ProcessPed (hueco 7)
inline int IKIndex(const void* t) { for(size_t k=0;k<NIKT;++k)if(t==ikTask[k].data())return int(k); return -1; }
inline void* __fastcall IKDel(void* t,void*,uint32 flags) { trace.push_back({"iktask_delete",Name(uintptr_t(t)),flags}); return t; }
inline void* __fastcall IKClone(void* t,void*) { trace.push_back({"iktask_clone",Name(uintptr_t(t))}); const int k=IKIndex(t); return k>=0 ? static_cast<void*>(ikTask[(size_t(k)+4)%NIKT].data()) : nullptr; }
inline bool __fastcall IKProcess(void* t,void*,CPed* p) { trace.push_back({"iktask_process_ped",Name(uintptr_t(t)),Name(uintptr_t(p))}); const int k=IKIndex(t); return k>=0&&((ikProcMask>>k)&1); }
template<int N> void* __fastcall KTrap(void*,void*) { trace.push_back({"iktask_unexpected_slot",N}); return nullptr; }
template<int... N> void FillKTraps(std::integer_sequence<int,N...>) { ((ikVtbl[N]=reinterpret_cast<void*>(&KTrap<N>)),...); }

// dependencias simuladas
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
// el EXE desreferencia la asociacion devuelta: nunca nula (una por animacion: andar, correr, esprintar)
inline CAnimBlendAssociation* __cdecl AddAnim(RpClump* c,int32 group,int32 id) { trace.push_back({"add_animation",Name(uintptr_t(c)),group,id}); return reinterpret_cast<CAnimBlendAssociation*>(assoc[5+size_t(uint32(id)%3)].data()); }
// asociaciones del clump: reposo 3, andar 0, correr 1, esprintar 2 y reposo cansado 10 (cada una en su buffer)
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) {
    trace.push_back({"clump_get_association",Name(uintptr_t(c)),id});
    const int k=id==3 ? 0 : id==0 ? 1 : id==1 ? 2 : id==2 ? 3 : id==10 ? 4 : -1;
    return k>=0&&((assocMask>>k)&1) ? reinterpret_cast<CAnimBlendAssociation*>(assoc[size_t(k)].data()) : nullptr;
}
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetCurTime(CAnimBlendAssociation* a,void*,float t) { trace.push_back({"set_current_time",Name(uintptr_t(a)),F(t)}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
// CPed::Say: el EXE pasa el contexto con movzx; la DLL puede extender el signo del enum de 16 bits
inline int16 __fastcall Say(CPed* p,void*,uint32 ctx,uint32 delay,float prob,uint32 b0,uint32 b1,uint32 b2) { trace.push_back({"ped_say",Name(uintptr_t(p)),ctx&0xFFFF,delay,F(prob),b0&0xFF,b1&0xFF,b2&0xFF}); return -1; }
inline bool __fastcall IsPlayerStub(CPed* p,void*) { trace.push_back({"ped_is_player",Name(uintptr_t(p))}); return At<uint32>(p,P_TYPE)<2; }
inline CWanted* __cdecl FindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall Crime(void* w,void*,int32 crime,const CVector* pos,uint32 key,uint32 b) { trace.push_back({"register_crime_immediately",Name(uintptr_t(w)),crime,Name(uintptr_t(pos)),F(pos->x),F(pos->y),F(pos->z),Name(key),b&0xFF}); }
inline CTask* __fastcall DuckGet(void* i,void*,uint32 b) { trace.push_back({"get_task_duck",Name(uintptr_t(i)),b&0xFF}); return reinterpret_cast<CTask*>(duckRet); }
inline void __fastcall DuckMove(void* d,void*,float x,float y) { trace.push_back({"duck_control_move",Name(uintptr_t(d)),F(x),F(y)}); }
inline bool __fastcall Circled(void* t,void*,CPed* p) { trace.push_back({"goto_has_circled_target",Name(uintptr_t(t)),Name(uintptr_t(p))}); return circledRet; }
inline void __fastcall QuitIK(void* t,void*,CPed* p) { trace.push_back({"goto_quit_ik",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline void __fastcall SetUpIK(void* t,void*,CPed* p) { trace.push_back({"goto_set_up_ik",Name(uintptr_t(t)),Name(uintptr_t(p))}); }
inline float __cdecl AngleBetween(float x1,float y1,float x2,float y2) { trace.push_back({"radian_angle_between_points",F(x1),F(y1),F(x2),F(y2)}); return angleRet; }
inline float __cdecl LimitAngle(float a) { trace.push_back({"limit_radian_angle",F(a)}); return limitRet; }
inline IKChain_c* __fastcall AddChain(void* m,void*,const char* nm,uint32 slot,CPed* p,int32 eff,float ex,float ey,float ez,int32 pivot,CEntity* e,int32 offBone,float ox,float oy,float oz,float speed,int32 prio) {
    trace.push_back({"ik_add_chain",Name(uintptr_t(m)),nm ? std::string(nm) : std::string("<null>"),slot&0xFF,Name(uintptr_t(p)),eff,F(ex),F(ey),F(ez),pivot,Name(uintptr_t(e)),offBone,F(ox),F(oy),F(oz),F(speed),prio});
    return reinterpret_cast<IKChain_c*>(chainRet);
}
inline void __fastcall RemoveChain(void* m,void*,void* c) { trace.push_back({"ik_remove_chain",Name(uintptr_t(m)),Name(uintptr_t(c))}); }
inline void __fastcall ChainEntity(void* c,void*,CEntity* e) { trace.push_back({"ik_chain_update_entity",Name(uintptr_t(c)),Name(uintptr_t(e))}); }
inline void __fastcall ChainOffset(void* c,void*,int32 bone,float x,float y,float z) { trace.push_back({"ik_chain_update_offset",Name(uintptr_t(c)),bone,F(x),F(y),F(z)}); }
inline void __fastcall ChainTarget(void* c,void*,uint32 b) { trace.push_back({"ik_chain_update_target",Name(uintptr_t(c)),b&0xFF}); }
inline void __fastcall ChainClamp(void* c,void*,int32 bone,uint32 x,uint32 y,uint32 z,uint32 cur) { trace.push_back({"ik_chain_clamp_limits",Name(uintptr_t(c)),bone,x&0xFF,y&0xFF,z&0xFF,cur&0xFF}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { DIVE, STEP, FINE, LOOKAT, IKMAN };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,5> classes{{
    {"CTaskSimpleEvasiveDive",0x86F244,0x14,DIVE}, {"CTaskSimpleEvasiveStep",0x86F1F4,0x14,STEP},
    {"CTaskSimpleGoToPointFine",0x86F964,0x24,FINE}, {"CTaskSimpleIKLookAt",0x86E37C,0x5C,LOOKAT},
    {"CTaskSimpleIKManager",0x86E358,0x1C,IKMAN},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=40;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleEvasiveDive","Clone",0,0x86F244,1,128},{"CTaskSimpleEvasiveDive","Constructor",0x653560,0x86F244,-1,128},{"CTaskSimpleEvasiveDive","Destructor",0x6535D0,0x86F244,-1,128},{"CTaskSimpleEvasiveDive","FinishAnimEvasiveDiveCB",0x6536A0,0x86F244,-1,64},
    {"CTaskSimpleEvasiveDive","GetTaskType",0,0x86F244,4,64},{"CTaskSimpleEvasiveDive","MakeAbortable",0,0x86F244,6,512},{"CTaskSimpleEvasiveDive","ProcessPed",0,0x86F244,7,512},{"CTaskSimpleEvasiveDive","StartAnim",0x655F20,0x86F244,-1,512},
    {"CTaskSimpleEvasiveStep","Clone",0,0x86F1F4,1,128},{"CTaskSimpleEvasiveStep","Constructor",0x653160,0x86F1F4,-1,128},{"CTaskSimpleEvasiveStep","Destructor",0x6531D0,0x86F1F4,-1,128},{"CTaskSimpleEvasiveStep","FinishAnimEvasiveStepCB",0x653290,0x86F1F4,-1,256},
    {"CTaskSimpleEvasiveStep","GetTaskType",0,0x86F1F4,4,64},{"CTaskSimpleEvasiveStep","MakeAbortable",0,0x86F1F4,6,256},{"CTaskSimpleEvasiveStep","ProcessPed",0,0x86F1F4,7,512},{"CTaskSimpleEvasiveStep","StartAnim",0x655EA0,0x86F1F4,-1,256},
    {"CTaskSimpleGoToPointFine","BaseRatio",0x65EF30,0x86F964,-1,128},{"CTaskSimpleGoToPointFine","Clone",0,0x86F964,1,256},{"CTaskSimpleGoToPointFine","Constructor",0x65EEB0,0x86F964,-1,256},{"CTaskSimpleGoToPointFine","Finish",0x65EF00,0x86F964,-1,128},
    {"CTaskSimpleGoToPointFine","MakeAbortable",0,0x86F964,6,128},{"CTaskSimpleGoToPointFine","ProcessPed",0,0x86F964,7,1024},{"CTaskSimpleGoToPointFine","SetBlendedMoveAnim",0x65EF80,0x86F964,-1,2048},{"CTaskSimpleGoToPointFine","SetTargetPos",0x65F330,0x86F964,-1,512},
    {"CTaskSimpleIKLookAt","Clone",0,0x86E37C,1,512},{"CTaskSimpleIKLookAt","Constructor",0x633E00,0x86E37C,-1,512},{"CTaskSimpleIKLookAt","CreateIKChain",0,0x86E37C,9,256},{"CTaskSimpleIKLookAt","Destructor-",0x633EF0,0x86E37C,-1,128},
    {"CTaskSimpleIKLookAt","GetLookAtEntity",0x634120,0x86E37C,-1,64},{"CTaskSimpleIKLookAt","GetLookAtOffset",0x634130,0x86E37C,-1,64},{"CTaskSimpleIKLookAt","GetTaskType",0,0x86E37C,4,64},{"CTaskSimpleIKLookAt","UpdateLookAtInfo",0x634050,0x86E37C,-1,512},
    {"CTaskSimpleIKManager","AddIKChainTask",0x633940,0x86E358,-1,512},{"CTaskSimpleIKManager","Clone",0,0x86E358,1,256},{"CTaskSimpleIKManager","Constructor",0x6337F0,0x86E358,-1,64},{"CTaskSimpleIKManager","Destructor",0x633830,0x86E358,-1,128},
    {"CTaskSimpleIKManager","GetTaskAtSlot",0x6339B0,0x86E358,-1,64},{"CTaskSimpleIKManager","GetTaskType",0,0x86E358,4,64},{"CTaskSimpleIKManager","MakeAbortable",0,0x86E358,6,256},{"CTaskSimpleIKManager","ProcessPed",0,0x86E358,7,512},
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
    if(!std::filesystem::exists(dir/"stasks8a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks8a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks8a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks8a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const int16 pedModel=player->m_nModelIndex; int16 vehModel=-1;
    for(int32 m=400;m<612&&vehModel<0;++m)if(CModelInfo::GetModelInfo(m))vehModel=int16(m);
    if(vehModel<0||!CModelInfo::GetModelInfo(pedModel)){Write({{"event","error"},{"reason","model info missing"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&SetMoveAnim);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    FillATraps(std::make_integer_sequence<int,16>{}); assocVtbl[0]=reinterpret_cast<void*>(&ADel);
    FillKTraps(std::make_integer_sequence<int,16>{}); ikVtbl[0]=reinterpret_cast<void*>(&IKDel); ikVtbl[1]=reinterpret_cast<void*>(&IKClone); ikVtbl[7]=reinterpret_cast<void*>(&IKProcess);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura al final
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(ADDR_TIME_STEP); const float savedStep=timeStep;
    auto restoreGlobals=[&]{ timeMs=savedTime; timeStep=savedStep; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation)),AddAnim), a2e(0x4D3AA0,AddAnim);
        EntryPatch a3(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),GetAssoc), a3e(0x4D68B0,GetAssoc);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a5e(0x4CEBE0,SetFinishCb);
        EntryPatch a7(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime),SetCurTime), a7e(0x4CEA80,SetCurTime);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPed::Say),Say), p2e(0x5EFFE0,Say);
        EntryPatch p3(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayerStub), p3e(0x5DF8F0,IsPlayerStub);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),FindWanted), w1e(0x56E230,FindWanted);
        EntryPatch w2(bit_cast<uintptr_t>(&CWanted::RegisterCrime_Immediately),Crime), w2e(0x562430,Crime);
        EntryPatch d1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),DuckGet), d1e(0x6010A0,DuckGet);
        EntryPatch d2(bit_cast<uintptr_t>(&CTaskSimpleDuck::ControlDuckMove),DuckMove), d2e(0x6923F0,DuckMove);
        EntryPatch g1(bit_cast<uintptr_t>(&CTaskSimpleGoTo::HasCircledTarget),Circled), g1e(0x667A10,Circled);
        EntryPatch g2(bit_cast<uintptr_t>(&CTaskSimpleGoTo::QuitIK),QuitIK), g2e(0x667CA0,QuitIK);
        EntryPatch g3(bit_cast<uintptr_t>(&CTaskSimpleGoTo::SetUpIK),SetUpIK), g3e(0x667AD0,SetUpIK);
        EntryPatch g4(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),AngleBetween), g4e(0x53CBE0,AngleBetween);
        EntryPatch g5(bit_cast<uintptr_t>(&CGeneral::LimitRadianAngle),LimitAngle), g5e(0x53CB50,LimitAngle);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::AddIKChain),AddChain), k1e(0x618750,AddChain);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::RemoveIKChain),RemoveChain), k2e(0x618170,RemoveChain);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChain_c::UpdateEntity),ChainEntity), k3e(0x617E00,ChainEntity);
        EntryPatch k4(bit_cast<uintptr_t>(&IKChain_c::UpdateOffset),ChainOffset), k4e(0x617E20,ChainOffset);
        EntryPatch k5(bit_cast<uintptr_t>(&IKChain_c::UpdateTarget),ChainTarget), k5e(0x617E50,ChainTarget);
        EntryPatch k6(bit_cast<uintptr_t>(&IKChain_c::ClampLimits),ChainClamp), k6e(0x618520,ChainClamp);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8A8Au; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    timeMs=next(); timeStep=next()%3 ? 1.f : uni(0.1f,3.f);
                    // peatones (vtable falsa, clump con tipo 2, inteligencia aleatoria)
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(veh.data(),0,veh.size()); std::memset(veh2.data(),0,veh2.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto& x:intelA)x=uint8(next());
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At<int16>(Ent(),P_MODEL)=vehModel; Ent()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    for(auto* v:{Veh(),Veh2()}){ v->SetType(ENTITY_TYPE_VEHICLE); At<int16>(v,P_MODEL)=vehModel; v->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)); }
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At<int16>(p,P_MODEL)=pedModel;
                        At(p,P_MOVESTATE)=next()%8; At(p,P_SWIMSTATE)=next()%8; At<int32>(p,P_ANIMGROUP)=int32(next()%60);
                        { static const uint32 types[]{0,1,4,5,6,6,6,7,8,12,15,16}; At(p,P_TYPE)=types[next()%12]; }
                        At<float>(p,P_ROT_AIM)=uni(-6.5f,6.5f);
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelA.data();
                    if(next()%2){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{};
                        m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                        std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    const CVector pedPos=PedA()->GetPosition();
                    // conductores de los vehiculos (StartAnim de Dive)
                    { CEntity* const drivers[]{nullptr,PedB(),PedA()}; At<CEntity*>(Veh(),V_DRIVER)=drivers[next()%3]; At<CEntity*>(Veh2(),V_DRIVER)=drivers[next()%3]; }
                    // asociaciones de animacion falsas (floats controlados, sin NaN)
                    for(auto& a:assoc){ for(auto& b:a)b=uint8(next()); *reinterpret_cast<void**>(a.data())=assocVtbl.data();
                        static const float deltas[]{-4.f,0.f,4.f,-1000.f,1.f,-0.5f};
                        At<float>(a.data(),0x18)=uni(0.f,1.f); At<float>(a.data(),0x1C)=deltas[next()%6];
                        At<float>(a.data(),0x20)=uni(0.f,2.f); At<float>(a.data(),0x24)=uni(0.f,2.f); At<float>(a.data(),0x28)=uni(0.f,0.3f); }
                    // tareas de IK falsas (vtable trazada)
                    for(auto& t:ikTask){ for(auto& b:t)b=uint8(next()); *reinterpret_cast<void**>(t.data())=ikVtbl.data(); }
                    for(auto* b:{&wanted,&chain,&duck})for(auto& x:*b)x=uint8(next());
                    std::memset(outVec.data(),0xCD,outVec.size());
                    // evento falso (tipo, entidad origen en 0x14 y arma en 0x1C, como CEventDamage)
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    eventType=next()%3 ? EVENT_DAMAGE : int(next()%100);
                    { CEntity* const srcs[]{nullptr,Veh(),Veh2(),Ent(),PedB()}; At<CEntity*>(event.data(),0x14)=srcs[next()%5]; }
                    { static const int32 weapons[]{49,50,49,50,0,22,51,48}; At<int32>(event.data(),0x1C)=weapons[next()%8]; }
                    // dependencias simuladas
                    blendRet=(cl.kind==STEP||next()%4) ? uintptr_t(assoc[8].data()) : 0; // Step escribe en la asociacion devuelta
                    assocMask=next(); ikProcMask=next(); circledRet=next()%3==0;
                    duckRet=next()%3 ? uintptr_t(duck.data()) : 0; chainRet=next()%4 ? uintptr_t(chain.data()) : 0;
                    angleRet=uni(-7.f,7.f); limitRet=uni(-3.2f,3.2f);
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    CEntity* const ents[]{nullptr,Veh(),Veh2(),Ent(),PedB()};
                    registerRefs=nm!="Constructor"; // el constructor sobrescribe el objeto con 0xCD
                    switch(cl.kind){
                    case DIVE: {
                        CEntity* const vehs[]{nullptr,Veh(),Veh2()}; RegisterAt(o,8,vehs[next()%3]);
                        At<uint8>(o,0xC)=uint8(next()%2); At<void*>(o,0x10)=next()%2 ? static_cast<void*>(assoc[0].data()) : nullptr;
                        break; }
                    case STEP:
                        RegisterAt(o,8,ents[next()%5]); At<uint8>(o,0xC)=uint8(next()%2);
                        // FinishAnimEvasiveStepCB lee la asociacion sin comprobarla
                        At<void*>(o,0x10)=(nm=="FinishAnimEvasiveStepCB"||next()%2) ? static_cast<void*>(assoc[0].data()) : nullptr;
                        break;
                    case FINE: {
                        { static const int32 states[]{1,4,6,7,0,2}; At<int32>(o,8)=states[next()%6]; }
                        if(next()%2){ At<float>(o,0xC)=pedPos.x+uni(-3.f,3.f); At<float>(o,0x10)=pedPos.y+uni(-3.f,3.f); }
                        else { At<float>(o,0xC)=uni(-30.f,30.f); At<float>(o,0x10)=uni(-30.f,30.f); }
                        At<float>(o,0x14)=uni(0.f,40.f); At<float>(o,0x18)=next()%3 ? 0.5f : uni(0.f,3.f);
                        { const uint32 m=next()%12; static const float fixed[]{0.f,0.5f,1.f,1.5f,2.f,2.5f,3.f,3.5f,-1.f};
                          At<float>(o,0x20)=m<9 ? fixed[m] : m==9 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,4.f); } // NaN silencioso
                        break; }
                    case LOOKAT:
                        At<int32>(o,8)=next()%3 ? int32(next()%5000) : -1; At<int32>(o,0xC)=int32(next()%2000);
                        At<void*>(o,0x10)=next()%3 ? static_cast<void*>(chain.data()) : nullptr;
                        At<int16>(o,0x18)=int16(next()%3 ? 5 : int32(next()%60));
                        for(size_t off:{size_t(0x1C),size_t(0x20),size_t(0x24),size_t(0x30),size_t(0x34),size_t(0x38),size_t(0x3C),size_t(0x44),size_t(0x4C)})At<float>(o,off)=uni(-2.f,2.f);
                        RegisterAt(o,0x28,ents[next()%5]); At<int32>(o,0x2C)=next()%3 ? int32(next()%60) : int32(next());
                        At<uint8>(o,0x40)=uint8(next()%2); At<int32>(o,0x54)=int32(next()%2);
                        At<uint8>(o,0x58)=uint8(next()%2); At<int8>(o,0x59)=int8(next());
                        break;
                    case IKMAN:
                        for(size_t k=0;k<4;++k)At<void*>(o,8+4*k)=next()%3 ? static_cast<void*>(ikTask[k].data()) : nullptr;
                        At<uint8>(o,0x18)=uint8(next()%4==0);
                        break;
                    }
                    CPed* const pd=PedA();
                    // argumentos
                    CEntity* const argEnt=ents[next()%5]; const uint32 argA=next(), argB=next(), argC=next();
                    const float argF=uni(0.f,100.f), ax=uni(-2.f,2.f), ay=uni(-2.f,2.f), az=uni(-2.f,2.f);
                    const char* const argName=next()%2 ? purposeName : nullptr;
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor"||nm=="Destructor-";
                    const bool isVoid=nm=="StartAnim"||nm=="Finish"||nm=="SetBlendedMoveAnim"||nm=="SetTargetPos"||nm=="UpdateLookAtInfo"||nm=="AddIKChainTask"||nm.starts_with("FinishAnim");
                    if(isCtor){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case DIVE: { CEntity* const vehs[]{nullptr,Veh(),Veh2()}; rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*)>(target)(o,vehs[argA%3])); break; }
                        case STEP: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*)>(target)(o,argEnt)); break;
                        case FINE: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,float,float,float,float,float,CEntity*)>(target)(o,uni(0.f,4.f),ax*10.f,ay*10.f,az*10.f,argB%3 ? 0.5f : argF/50.f,argEnt)); break;
                        case LOOKAT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,CEntity*,int32,int32,float,float,float,uint32,float,int32,int32)>(target)(
                                        o,argName,argEnt,argA%3 ? int32(argA%5000) : -1,argB%3 ? int32(argB%60) : int32(argB),ax,ay,az,argC&1,argF,int32(argC%2000),int32(argA>>8))); break;
                        case IKMAN: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        } }
                    else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed"||nm=="CreateIKChain")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="StartAnim"||nm=="Finish"||nm=="SetBlendedMoveAnim")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm.starts_with("FinishAnim")){ void* a=argA%4 ? static_cast<void*>(assoc[8].data()) : nullptr; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(a,o); }
                    else if(nm=="BaseRatio"){ const int32 s=argA%4 ? int32(argA%10)-1 : int32(argB); ret=F(reinterpret_cast<float(__cdecl*)(int32)>(target)(s)); }
                    else if(nm=="SetTargetPos"){
                        float x=ax*10.f, y=ay*10.f, z=az*10.f;
                        if(argA%3==0){ x=At<float>(o,0xC); y=At<float>(o,0x10); z=At<float>(o,0x14); if(argB%2)At<float>(o,0x18)=0.5f; }
                        else if(argA%3==1&&argB%2){ x=At<float>(o,0xC); y=At<float>(o,0x10); }
                        reinterpret_cast<void(__thiscall*)(void*,float,float,float)>(target)(o,x,y,z); }
                    else if(nm=="GetLookAtEntity")rawRet=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetLookAtOffset")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(target)(o,outVec.data()));
                    else if(nm=="UpdateLookAtInfo")reinterpret_cast<void(__thiscall*)(void*,const char*,CPed*,CEntity*,int32,int32,float,float,float,uint32,float,int32,int32)>(target)(
                                        o,argName,pd,argEnt,int32(argA%5000),argB%3 ? int32(argB%60) : int32(argB),ax,ay,az,argC&1,argF,int32(argC%2000),int32(argA>>4));
                    else if(nm=="AddIKChainTask"){ static const int32 slots[]{-1,-1,-1,-7,0,1,2,3,4,5}; void* t=argB%5 ? static_cast<void*>(ikTask[4+argB%4].data()) : nullptr;
                        reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(target)(o,t,slots[argA%10]); }
                    else if(nm=="GetTaskAtSlot")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(target)(o,int32(argA%6)));
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
                    json assocs=json::array(); for(auto& a:assoc)assocs.push_back(Region(a.data(),a.size()));
                    json iks=json::array(); for(auto& t:ikTask)iks.push_back(Hash(t.data(),t.size()));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"veh",EntHash(veh)},{"veh2",EntHash(veh2)},{"intelA",Hash(intelA.data(),intelA.size())},{"mat",Hash(mat.data(),mat.size())},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS2),At(PedA(),P_FLAGS4),At(PedA(),P_MOVESTATE),At(PedA(),P_SWIMSTATE),At(PedA(),P_ROT_AIM)}},
                            {"assoc",assocs},{"iktasks",iks},{"outvec",Region(outVec.data(),16)},
                            {"buffers",{Hash(wanted.data(),wanted.size()),Hash(chain.data(),chain.size()),Hash(duck.data(),duck.size())}},
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
    const bool globalsTouched=timeMs!=savedTime||timeStep!=savedStep;
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",globalsTouched},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime&&timeStep==savedStep}});
}
}
