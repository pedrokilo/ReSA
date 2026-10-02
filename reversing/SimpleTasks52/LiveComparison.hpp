#pragma once
// Banco dentro del motor: tareas simples (lista de 52, 48 funciones): CTaskSimpleHitByGunFrom{Front,Left,Rear,Right},
// CTaskSimpleHitFrom{Back,Behind,Front,Left,Right}, CTaskSimpleHitWall, CTaskSimpleLookAbout, CTaskSimplePutDownEntity,
// CTaskSimpleStandStill, CTaskSimpleTired, CTaskSimpleChat y CTaskSimpleArrestPed.
// Original (hooks del lote y constructores base de RunAnim/RunTimedAnim/HoldEntity desactivados) frente a candidato con
// fixtures identicos: los constructores se llaman en la direccion del EXE sobre un buffer rellenado con 0xCD (se compara
// el tipo en vez del vptr); los metodos virtuales se llaman por la entrada de la vtable del EXE. Peatones (vtable falsa
// trazada), clump (byte de tipo 2), inteligencia, datos de jugador, asociacion de animacion, entidad y evento falsos en
// buffers propios. Se simulan y trazan: CAnimManager::BlendAnimation, RpAnimBlendClumpGetAssociation,
// CAnimBlendAssociation::SetFinishCallback/SetDeleteCallback, CAnimManager::AddAnimBlockRef/RemoveAnimBlockRef,
// CPed::SetMoveState, CPed::SetMoveAnim (hueco 23), CPedIntelligence::GetTaskDuck, CTaskSimpleDuck::ControlDuckMove,
// FindPlayerPed, CClothes::GetDefaultPlayerMotionGroup, CPed::SetPedState, CPed::Say, CPed::GiveWeapon,
// CPed::SetCurrentWeapon, CPed::GetBonePosition, CGeneral::GetRadianAngleBetweenPoints, CPlaceable::SetOrientation,
// CPedIK::PointGunAtPosition y CWeapon::IsTypeMelee. Las tareas nuevas del pool se siguen (m_LastFreeSlot restaurado y
// huecos libres con 0xCD); el reloj se guarda y se restaura.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleHitByGunFromFront.h"
#include "Tasks/TaskTypes/TaskSimpleHitByGunFromLeft.h"
#include "Tasks/TaskTypes/TaskSimpleHitByGunFromRear.h"
#include "Tasks/TaskTypes/TaskSimpleHitByGunFromRight.h"
#include "Tasks/TaskTypes/TaskSimpleHitFromBack.h"
#include "Tasks/TaskTypes/TaskSimpleHitFromBehind.h"
#include "Tasks/TaskTypes/TaskSimpleHitFromFront.h"
#include "Tasks/TaskTypes/TaskSimpleHitFromLeft.h"
#include "Tasks/TaskTypes/TaskSimpleHitFromRight.h"
#include "Tasks/TaskTypes/TaskSimpleHitWall.h"
#include "Tasks/TaskTypes/TaskSimpleLookAbout.h"
#include "Tasks/TaskTypes/TaskSimplePutDownEntity.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Tasks/TaskTypes/TaskSimpleTired.h"
#include "Tasks/TaskTypes/TaskSimpleChat.h"
#include "Tasks/TaskTypes/TaskSimpleArrestPed.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "PedIntelligence.h"
#include "PedIK.h"
#include "Clothes.h"
#include "General.h"
#include "Weapon.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace STasks52Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
// desplazamientos crudos del EXE (CPed)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS1=0x46C, P_FLAGS3=0x474, P_INTEL=0x47C, P_PDATA=0x480, P_ANIM_GROUP=0x4D4,
                 P_STATE=0x530, P_SWIM=0x538, P_ROT_CUR=0x558, P_ROT_AIM=0x55C, P_TYPE=0x598, P_WEAPONS=0x5A0, P_ACTIVE_SLOT=0x718;
constexpr uint32 F1_IN_VEHICLE=1u<<8, F1_DUCKING=1u<<26, F3_BEING_ARRESTED=1u<<6;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_ARREST_CB=0x68B7D0, ANIM_BLOCKS=0xB5D4A0;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> mat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,0x80> clump{}, intelA{}, intelB{}, pdA{}, pdB{}, duck{};
alignas(16) inline std::array<uint8,sizeof(CAnimBlendAssociation)> anim{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<uint8,0x60> event{};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_ARREST_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleArrestPed::FinishAnimArrestPedCB))return "FinishAnimArrestPedCB";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedA.data(),pedA.size(),"pedA"))return *s; if(auto s=in(pedB.data(),pedB.size(),"pedB"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(ent.data(),ent.size(),"ent"))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(intelA.data(),intelA.size(),"intelA"))return *s;
    if(auto s=in(intelB.data(),intelB.size(),"intelB"))return *s; if(auto s=in(pdA.data(),pdA.size(),"pdA"))return *s;
    if(auto s=in(pdB.data(),pdB.size(),"pdB"))return *s; if(auto s=in(duck.data(),duck.size(),"duck"))return *s;
    if(auto s=in(anim.data(),anim.size(),"anim"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"))return *s;
    if(v>=ANIM_BLOCKS&&v<ANIM_BLOCKS+0x20*NUM_ANIM_BLOCKS)return "animblock+"+std::to_string(v-ANIM_BLOCKS);
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
inline uintptr_t blendRet{}, assocRet{}, duckRet{}, playerRet{}; inline int32 motionRet{}; inline CVector bonePt{}; inline float angleRet{};
inline uint32 meleeMask{};

// peaton: vtable falsa (SetMoveAnim en el hueco 23 = +0x5C)
inline void __fastcall PedSetMoveAnim(CPed* p,void*) { trace.push_back({"set_move_anim",Name(uintptr_t(p))}); }
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
// evento falso (GetEventType en el hueco 1)
inline int eventType{};
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline CAnimBlendAssociation* __cdecl GetAssoc(RpClump* c,uint32 id) { trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return reinterpret_cast<CAnimBlendAssociation*>(assocRet); }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __fastcall SetDeleteCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_delete_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline void __cdecl AddBlockRef(int32 i) { trace.push_back({"add_anim_block_ref",i}); }
inline void __cdecl RemoveBlockRef(int32 i) { trace.push_back({"remove_anim_block_ref",i}); }
inline void __fastcall SetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(uintptr_t(p)),s}); }
inline void* __fastcall GetDuck(void* intel,void*,bool ignore) { trace.push_back({"get_task_duck",Name(uintptr_t(intel)),ignore}); return reinterpret_cast<void*>(duckRet); }
inline void __fastcall DuckMove(void* d,void*,float x,float y) { trace.push_back({"control_duck_move",Name(uintptr_t(d)),F(x),F(y)}); }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline int32 __cdecl MotionGroup() { trace.push_back({"default_player_motion_group"}); return motionRet; }
inline void __fastcall SetPedState(CPed* p,void*,int32 s) { trace.push_back({"set_ped_state",Name(uintptr_t(p)),s}); }
inline int16 __fastcall Say(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(uintptr_t(p)),ctx,delay,F(prob),overrideSilence,forceAudible,frontEnd}); return 0;
}
inline int32 __fastcall GiveWeapon(CPed* p,void*,int32 type,uint32 ammo,bool unused) { trace.push_back({"give_weapon",Name(uintptr_t(p)),type,ammo,unused}); return 2; }
inline void __fastcall SetCurWeapon(CPed* p,void*,int32 type) { trace.push_back({"set_current_weapon",Name(uintptr_t(p)),type}); }
inline void __fastcall BonePos(CPed* p,void*,CVector* out,int32 bone,bool update) { trace.push_back({"get_bone_position",Name(uintptr_t(p)),out!=nullptr,bone,update}); *out=bonePt; }
inline float __cdecl RadAngle(float x1,float y1,float x2,float y2) { trace.push_back({"radian_angle",F(x1),F(y1),F(x2),F(y2)}); return angleRet; }
inline void __fastcall SetOrient(CPlaceable* pl,void*,float x,float y,float z) { trace.push_back({"set_orientation",Name(uintptr_t(pl)),F(x),F(y),F(z)}); }
inline void __fastcall PointGun(CPedIK* ik,void*,const CVector* pos,float blend) { trace.push_back({"point_gun",Name(uintptr_t(ik)),F(pos->x),F(pos->y),F(pos->z),F(blend)}); }
inline bool __fastcall IsMelee(CWeapon* w,void*) { const uint32 t=*reinterpret_cast<uint32*>(w); trace.push_back({"is_type_melee",Name(uintptr_t(w)),t}); return (meleeMask>>(t%32))&1; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { HIT, LOOK, PUT, STAND, TIRED, CHAT, ARREST };
struct Cls { const char* cls; uintptr_t vt; size_t size; int32 type; Kind kind; };
inline const std::array<Cls,16> classes{{
    {"CTaskSimpleHitByGunFromFront",0x86DC10,0x20,TASK_SIMPLE_HIT_BY_GUN_FRONT,HIT}, {"CTaskSimpleHitByGunFromLeft",0x86DC88,0x20,TASK_SIMPLE_HIT_BY_GUN_LEFT,HIT},
    {"CTaskSimpleHitByGunFromRear",0x86DC4C,0x20,TASK_SIMPLE_HIT_BY_GUN_BACK,HIT}, {"CTaskSimpleHitByGunFromRight",0x86DCC4,0x20,TASK_SIMPLE_HIT_BY_GUN_RIGHT,HIT},
    {"CTaskSimpleHitFromBack",0x86DB70,0x20,TASK_SIMPLE_HIT_BACK,HIT}, {"CTaskSimpleHitFromBehind",0x86F168,0x20,TASK_SIMPLE_HIT_BEHIND,HIT},
    {"CTaskSimpleHitFromFront",0x86E5EC,0x20,TASK_SIMPLE_HIT_FRONT,HIT}, {"CTaskSimpleHitFromLeft",0x86DBA4,0x20,TASK_SIMPLE_HIT_LEFT,HIT},
    {"CTaskSimpleHitFromRight",0x86DBD8,0x20,TASK_SIMPLE_HIT_RIGHT,HIT}, {"CTaskSimpleHitWall",0x86F1A0,0x20,TASK_SIMPLE_HIT_WALL,HIT},
    {"CTaskSimpleLookAbout",0x85A164,0x34,TASK_SIMPLE_LOOK_ABOUT,LOOK}, {"CTaskSimplePutDownEntity",0x870B74,0x40,TASK_SIMPLE_PUTDOWN_ENTITY,PUT},
    {"CTaskSimpleStandStill",0x86DD2C,0x20,TASK_SIMPLE_STAND_STILL,STAND}, {"CTaskSimpleTired",0x86DED0,0x1C,TASK_SIMPLE_TIRED,TIRED},
    {"CTaskSimpleChat",0x86C78C,0x34,TASK_SIMPLE_CHAT,CHAT}, {"CTaskSimpleArrestPed",0x870984,0x14,TASK_SIMPLE_ARREST_PED,ARREST},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=48;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleHitByGunFromFront","Constructor",0x62EE70,0x86DC10,-1,64},{"CTaskSimpleHitByGunFromFront","Clone",0,0x86DC10,1,128},{"CTaskSimpleHitByGunFromFront","IsInterruptable",0,0x86DC10,9,64},
    {"CTaskSimpleHitByGunFromLeft","Constructor",0x62EFD0,0x86DC88,-1,64},{"CTaskSimpleHitByGunFromLeft","Clone",0,0x86DC88,1,128},{"CTaskSimpleHitByGunFromLeft","IsInterruptable",0,0x86DC88,9,64},
    {"CTaskSimpleHitByGunFromRear","Constructor",0x62EF20,0x86DC4C,-1,64},{"CTaskSimpleHitByGunFromRear","Clone",0,0x86DC4C,1,128},{"CTaskSimpleHitByGunFromRear","IsInterruptable",0,0x86DC4C,9,64},
    {"CTaskSimpleHitByGunFromRight","Constructor",0x62F080,0x86DCC4,-1,64},{"CTaskSimpleHitByGunFromRight","Clone",0,0x86DCC4,1,128},{"CTaskSimpleHitByGunFromRight","IsInterruptable",0,0x86DCC4,9,64},
    {"CTaskSimpleHitFromBack","Constructor",0x62EC60,0x86DB70,-1,64},{"CTaskSimpleHitFromBack","Clone",0,0x86DB70,1,128},{"CTaskSimpleHitFromBack","IsInterruptable",0,0x86DB70,9,64},
    {"CTaskSimpleHitFromBehind","Constructor",0x652EE0,0x86F168,-1,64},{"CTaskSimpleHitFromBehind","Clone",0,0x86F168,1,128},{"CTaskSimpleHitFromBehind","IsInterruptable",0,0x86F168,9,64},
    {"CTaskSimpleHitFromFront","Constructor",0x637250,0x86E5EC,-1,64},{"CTaskSimpleHitFromFront","Clone",0,0x86E5EC,1,128},{"CTaskSimpleHitFromFront","IsInterruptable",0,0x86E5EC,9,64},
    {"CTaskSimpleHitFromLeft","Constructor",0x62ED10,0x86DBA4,-1,64},{"CTaskSimpleHitFromLeft","Clone",0,0x86DBA4,1,128},{"CTaskSimpleHitFromLeft","IsInterruptable",0,0x86DBA4,9,64},
    {"CTaskSimpleHitFromRight","Constructor",0x62EDC0,0x86DBD8,-1,64},{"CTaskSimpleHitFromRight","Clone",0,0x86DBD8,1,128},{"CTaskSimpleHitFromRight","IsInterruptable",0,0x86DBD8,9,64},
    {"CTaskSimpleHitWall","Constructor",0x652F90,0x86F1A0,-1,64},{"CTaskSimpleHitWall","Clone",0,0x86F1A0,1,128},{"CTaskSimpleHitWall","IsInterruptable",0,0x86F1A0,9,64},
    {"CTaskSimpleLookAbout","Constructor",0x48E0A0,0x85A164,-1,256},{"CTaskSimpleLookAbout","Clone",0,0x85A164,1,256},{"CTaskSimpleLookAbout","IsInterruptable",0,0x85A164,9,64},
    {"CTaskSimplePutDownEntity","Constructor",0x691990,0x870B74,-1,64},{"CTaskSimplePutDownEntity","Clone",0,0x870B74,1,512},{"CTaskSimplePutDownEntity","GetTaskType",0,0x870B74,4,64},
    {"CTaskSimpleStandStill","Constructor",0x62F310,0x86DD2C,-1,256},{"CTaskSimpleStandStill","MakeAbortable",0,0x86DD2C,6,512},{"CTaskSimpleStandStill","ProcessPed",0,0x86DD2C,7,2048},
    {"CTaskSimpleTired","Constructor",0x630F20,0x86DED0,-1,256},{"CTaskSimpleTired","Destructor",0x630F60,0x86DED0,-1,128},{"CTaskSimpleTired","StartAnim",0x630F70,0x86DED0,-1,512},
    {"CTaskSimpleChat","Clone",0x5F65E0,0x86C78C,1,256},{"CTaskSimpleChat","Destructor",0x5F7FD0,0x86C78C,-1,1024},{"CTaskSimpleChat","IsInterruptable",0x5F6650,0x86C78C,9,64},
    {"CTaskSimpleArrestPed","MakeAbortable",0,0x870984,6,1024},{"CTaskSimpleArrestPed","ProcessPed",0,0x870984,7,1024},{"CTaskSimpleArrestPed","StartAnim",0x68B7E0,0x870984,-1,1024},
}};
// constructores base que llaman los constructores/Clone del EXE: se desactivan en la ejecucion original para que sea EXE puro
inline const std::array<std::pair<const char*,const char*>,5> baseHooks{{
    {"CTaskSimpleRunAnim","Constructor2"},{"CTaskSimpleRunTimedAnim","Constructor-1"},{"CTaskSimpleRunTimedAnim","Constructor-2"},
    {"CTaskSimpleHoldEntity","Constructor-1"},{"CTaskSimpleHoldEntity","Constructor-3"},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent());
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stasks52-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stasks52-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stasks52");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stasks52-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> extra;
    for(auto& [c,n]:baseHooks){ if(auto* h=FindHook(c,n))extra.push_back(h); else Write({{"event","note"},{"reason","base hook not found (se deja como esta)"},{"function",std::string(c)+"::"+n}}); }
    FillPTraps(std::make_integer_sequence<int,64>{}); pedVtbl[23]=reinterpret_cast<void*>(&PedSetMoveAnim);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    auto& timeMs=*reinterpret_cast<uint32*>(ADDR_TIME_MS); const uint32 savedTime=timeMs;
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
        EntryPatch a4(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetDeleteCallback),SetDeleteCb), a4e(0x4CEBC0,SetDeleteCb);
        EntryPatch a5(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),AddBlockRef), a5e(0x4D3FB0,AddBlockRef);
        EntryPatch a6(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),RemoveBlockRef), a6e(0x4D3FD0,RemoveBlockRef);
        EntryPatch p1(bit_cast<uintptr_t>(&CPed::SetMoveState),SetMoveState), p1e(0x5DEC00,SetMoveState);
        EntryPatch p2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck),GetDuck), p2e(0x6010A0,GetDuck);
        EntryPatch p3(bit_cast<uintptr_t>(&CTaskSimpleDuck::ControlDuckMove),DuckMove), p3e(0x6923F0,DuckMove);
        EntryPatch p4(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FindPlayer), p4e(0x56E210,FindPlayer);
        EntryPatch p5(bit_cast<uintptr_t>(&CClothes::GetDefaultPlayerMotionGroup),MotionGroup), p5e(0x5A81B0,MotionGroup);
        EntryPatch p6(bit_cast<uintptr_t>(&CPed::SetPedState),SetPedState), p6e(0x5E4500,SetPedState);
        EntryPatch p7(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say)),Say), p7e(0x5EFFE0,Say);
        EntryPatch p8(bit_cast<uintptr_t>(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),GiveWeapon), p8e(0x5E6080,GiveWeapon);
        EntryPatch p9(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),SetCurWeapon), p9e(0x5E6280,SetCurWeapon);
        EntryPatch q1(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),BonePos), q1e(0x5E4280,BonePos);
        EntryPatch q2(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),RadAngle), q2e(0x53CBE0,RadAngle);
        EntryPatch q3(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(float,float,float)>(&CPlaceable::SetOrientation)),SetOrient), q3e(0x439A80,SetOrient);
        EntryPatch q4(bit_cast<uintptr_t>(&CPedIK::PointGunAtPosition),PointGun), q4e(0x5FDE20,PointGun);
        EntryPatch q5(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),IsMelee), q5e(0x73B1C0,IsMelee);
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
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5252u; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    timeMs=next();
                    // peatones (vtable falsa, clump con tipo 2, inteligencia y datos de jugador falsos)
                    std::memset(pedA.data(),0,pedA.size()); std::memset(pedB.data(),0,pedB.size()); std::memset(mat.data(),0,mat.size()); std::memset(ent.data(),0,ent.size());
                    for(auto& b:clump)b=uint8(next()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    for(auto* b:{&intelA,&intelB,&duck})for(auto& x:*b)x=uint8(next());
                    for(auto* b:{&pdA,&pdB})std::memset(b->data(),0,b->size());
                    At<float>(pdA.data(),0x14)=uni(0.f,3.f); At<float>(pdB.data(),0x14)=uni(0.f,3.f);
                    Ent()->SetType(ENTITY_TYPE_OBJECT);
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS3)=next(); At<int32>(p,P_ANIM_GROUP)=int32(next()%118); At(p,P_STATE)=next()%64; At(p,P_SWIM)=next()%8;
                        At<float>(p,P_ROT_CUR)=uni(-3.2f,3.2f); At<float>(p,P_ROT_AIM)=uni(-3.2f,3.2f);
                        { static const uint32 types[]{0,1,2,6,6,7,4}; At(p,P_TYPE)=types[next()%7]; }
                        At<uint8>(p,P_ACTIVE_SLOT)=uint8(next()%13); for(int k=0;k<13;++k)At(p,P_WEAPONS+0x1C*k)=next()%47;
                    }
                    At<void*>(PedA(),P_INTEL)=intelA.data(); At<void*>(PedB(),P_INTEL)=intelB.data();
                    At<void*>(PedA(),P_PDATA)=next()%3 ? nullptr : pdA.data(); At<void*>(PedB(),P_PDATA)=pdB.data(); // pedB siempre con datos (StartAnim los usa si es jugador)
                    if(next()%3){ PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; std::memcpy(static_cast<CMatrix*>(PedA()->m_matrix),&m,sizeof(CMatrix)); }
                    else PedA()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    // asociacion de animacion falsa (floats de mezcla controlados, sin NaN de senalizacion)
                    for(auto& b:anim)b=uint8(next());
                    { static const float amounts[]{0.99f,0.99000007f,1.f,0.5f,0.f,-0.f,0.98999995f,2.f,std::numeric_limits<float>::quiet_NaN()};
                      static const float deltas[]{-4.f,0.f,-0.f,1.f,-1000.f,4.f,-0.5f,std::numeric_limits<float>::quiet_NaN()};
                      At<float>(anim.data(),0x18)=amounts[next()%9]; At<float>(anim.data(),0x1C)=deltas[next()%8]; }
                    // evento falso (CEventDamage: m_bAddToEventGroup +0xC, m_damageResponse.m_bHealthZero +0x40)
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    event[0xC]=next()%5 ? uint8(next()%2) : uint8(next()); event[0x40]=next()%5 ? uint8(next()%2) : uint8(next());
                    { static const int types[]{EVENT_DAMAGE,EVENT_DAMAGE,EVENT_DAMAGE,EVENT_DEATH,EVENT_SCRIPT_COMMAND,EVENT_ON_FIRE,0}; eventType=types[next()%7]; }
                    // dependencias simuladas
                    { const uint32 m=next()%4; blendRet=m==0 ? 0 : uintptr_t(anim.data()); }
                    assocRet=next()%4 ? uintptr_t(anim.data()) : 0; duckRet=next()%3 ? uintptr_t(duck.data()) : 0;
                    { const uint32 m=next()%3; playerRet=m==0 ? 0 : m==1 ? uintptr_t(PedA()) : uintptr_t(PedB()); }
                    { static const int32 groups[]{ANIM_GROUP_FAT,ANIM_GROUP_FAT,ANIM_GROUP_DEFAULT,ANIM_GROUP_MUSCULAR,54,56}; motionRet=groups[next()%6]; }
                    bonePt=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}; angleRet=uni(-3.2f,3.2f); meleeMask=next();
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    // tarea probada
                    void* const o=object.data(); std::memset(o,0xCD,object.size());
                    for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                    At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                    uint32 ctorU=next(); int32 ctorI=0; bool ctorB1=next()%2, ctorB2=next()%2; float ctorF=0.f;
                    switch(cl.kind){
                    case HIT: At<void*>(o,8)=nullptr; break;
                    case LOOK: case CHAT:
                        At<void*>(o,8)=(cl.kind==CHAT&&next()%4) ? anim.data() : nullptr; At(o,0x20)=next()%3 ? next()%60000 : next();
                        if(nm=="Constructor")ctorU=next()%3 ? next()%60000 : next();
                        break;
                    case PUT: {
                        At<CEntity*>(o,8)=next()%3 ? Ent() : nullptr;
                        At<float>(o,0xC)=uni(-2.f,2.f); At<float>(o,0x10)=uni(-2.f,2.f); At<float>(o,0x14)=uni(-2.f,2.f);
                        At<uint8>(o,0x18)=uint8(next()%20); At<uint8>(o,0x19)=uint8(next()%3 ? (next()%2 ? 0x10 : 1) : next());
                        At<int32>(o,0x20)=next()%4 ? int32(next()%400) : ANIM_ID_NO_ANIMATION_SET; At<int32>(o,0x24)=int32(next()%118); At<int32>(o,0x28)=int32(next()&0xFFFF);
                        At<uintptr_t>(o,0x2C)=next()%4 ? ANIM_BLOCKS+0x20*(next()%NUM_ANIM_BLOCKS) : 0;
                        At<uintptr_t>(o,0x30)=next()%2 ? uintptr_t(0x10000000u+(next()%0x1000)*4) : 0; // jerarquia: solo se copia el puntero
                        At<float>(o,0x3C)=next()%3 ? uni(0.f,2.f) : 0.6f;
                        break; }
                    case STAND: {
                        At<int32>(o,8)=next()%4==0 ? -int32(next()%5) : int32(next()%20000);
                        At(o,0xC)=next()%4 ? timeMs-next()%30000 : next(); At<int32>(o,0x10)=next()%5==0 ? -1 : int32(next()%30000);
                        At<uint8>(o,0x14)=uint8(next()%2); At<uint8>(o,0x15)=uint8(next()%3==0); At<uint8>(o,0x18)=uint8(next()%3==0); At<uint8>(o,0x19)=uint8(next()%2);
                        At<float>(o,0x1C)=next()%3 ? 8.f : uni(0.f,16.f);
                        ctorI=next()%4==0 ? -int32(next()%5) : int32(next()%20000); ctorF=next()%3 ? 8.f : uni(-4.f,16.f);
                        break; }
                    case TIRED: At<void*>(o,0x18)=next()%2 ? anim.data() : nullptr; ctorU=next()%3 ? next()%20000 : next(); break;
                    case ARREST:
                        At<CPed*>(o,8)=next()%5 ? PedB() : PedA(); At<uint8>(o,0xC)=uint8(next()%3==0); At<void*>(o,0x10)=next()%2 ? anim.data() : nullptr;
                        // con m_Ped jugador, StartAnim lee sus datos de jugador (pedB siempre los tiene; pedA solo si no es jugador o los tiene)
                        if(At<CPed*>(o,8)==PedA()&&!At<void*>(PedA(),P_PDATA))At(PedA(),P_TYPE)=6;
                        // ProcessPed con asociacion: m_Ped no nulo (el EXE dejaria el punto sin inicializar)
                        if(nm=="ProcessPed"&&At<uint8>(o,0xC)==0&&next()%3==0)At<void*>(o,0x10)=nullptr;
                        if(nm=="StartAnim"||nm=="ProcessPed")blendRet=uintptr_t(anim.data()); // StartAnim escribe en la asociacion devuelta
                        { auto* m=At<CPed*>(o,8); if(At(m,P_TYPE)<=1)At<CPed*>(At<void*>(m,P_PDATA),8)=next()%3 ? nullptr : PedB(); }
                        break;
                    }
                    CPed* const pd=PedA();
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    // los hooks VMT solo cambian la entrada de la vtable (la direccion de la tabla es informativa)
                    uintptr_t rawRet=0; json ret=nullptr;
                    if(nm=="Constructor"){ std::memset(o,0xCD,object.size());
                        switch(cl.kind){
                        case HIT: case PUT: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o)); break;
                        case LOOK: case TIRED: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(target)(o,ctorU)); break;
                        case STAND: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,bool,bool,float)>(target)(o,ctorI,ctorB1,ctorB2,ctorF)); break;
                        default: break;
                        } }
                    else if(nm=="Destructor")reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                    else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                    else if(nm=="IsInterruptable")ret=reinterpret_cast<bool(__thiscall*)(void*,const CPed*)>(target)(o,pd);
                    else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                    else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    else if(nm=="StartAnim")reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd);
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&nm!="Destructor"&&nm!="StartAnim")ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh)tasks.push_back({{"type",int(t->GetTaskType())},{"words",Region(t,0x80,true)}});
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor";
                    json objectWords=Region(o,object.size(),isCtor||isDtor);   // vptr EXE o DLL tras construir/destruir
                    if(isCtor)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    refs.push_back(refList(PedA())); refs.push_back(refList(PedB())); refs.push_back(refList(Ent()));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS3),At(PedA(),P_SWIM),F(At<float>(PedA(),P_ROT_CUR)),F(At<float>(PedA(),P_ROT_AIM)),
                                           At(PedB(),P_FLAGS3)}},
                            {"pdA",Region(pdA.data(),pdA.size())},{"pdB",Region(pdB.data(),pdB.size())},{"mat",Hash(mat.data(),mat.size())},
                            {"anim",Region(anim.data(),anim.size())},{"event",Region(event.data(),event.size(),true)},{"time",timeMs}};
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
    timeMs=savedTime;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<extra.size();++k){extra[k]->State(extraEnabled[k]);extra[k]->LockState(extraLocked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&timeMs==savedTime}});
}
}
