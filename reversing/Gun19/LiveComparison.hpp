#pragma once
// Banco dentro del motor: CTaskSimpleUseGun (lista de 19, 18 funciones; el constructor ya estaba validado).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// 6 peatones (tamano de CPlayerPed, matriz, clump falso con tipo 2, inteligencia propia con gestor de tareas y escaner),
// una entidad que no es peaton, tarea en un buffer propio (vtable del EXE 0x86D724, 0x3C bytes + guarda a 0xCD),
// 11 asociaciones y 4 jerarquias falsas, CWeaponInfo falso (o el real de la tabla en SetPedPosition), datos de jugador,
// CTaskSimpleDuck/CTaskSimpleUseGun/CPad falsos y una tarea simple falsa con vtable trazada (GetSimplestActiveTask real).
// Se simulan y trazan: IKChainManager_c (IsArmPointing/AbortPointArm/IsLooking/AbortLookAt/LookAt/PointArm/IsFacingTarget),
// CPed (GetBonePosition 0x5E4280/GetTransformedBonePosition/IsAlive/SetMoveState/DoGunFlash/Say/GetWeaponSkill(tipo)),
// CPedIK (RotateTorsoForArm/PointGunAtPosition/PointGunInDirection), CWeapon::Fire, CCamera::Find3rdPersonCamTargetVector,
// CPlayerPed::GetPadFromPlayer, CPad::GetPedWalkUpDown/LeftRight, CPedIntelligence::GetTaskUseGun/GetTaskDuck,
// CTaskSimpleDuck::IsTaskInUseByOtherTasks/StopFireGun/ForceStopMove, RpAnimBlendClumpGetAssociation,
// CAnimManager::BlendAnimation/AddAnimation y CAnimBlendAssociation::SetCurrentTime.
// Los callbacks de las asociaciones se comparan por identidad (0x4D1390/0x61F3A0 y sus equivalentes de la DLL).
// Pool de tareas: m_LastFreeSlot y estados restaurados y huecos libres a 0xCD en cada ejecucion (Clone).
// Se guardan y restauran CTimer::ms_fTimeStep y el modo de la camara 0.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Tasks/TaskTypes/TaskSimpleUseGun.h"
#include "Tasks/TaskTypes/TaskSimpleDuck.h"
#include "Tasks/TaskManager.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "PedIK.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "WeaponInfo.h"
#include "Weapon.h"
#include "Camera.h"
#include "Pad.h"
#include "Entity/Ped/PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace Gun19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uintptr_t Body(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

constexpr int NPEDS=6, NASSOC=11, NHIER=4;
// desplazamientos crudos del EXE
constexpr size_t E_POS=4, E_MATRIX=0x14, E_RWOBJ=0x18, E_TYPE=0x36;                                         // CEntity
constexpr size_t P_FLAGS=0x46C, P_INTEL=0x47C, P_PD=0x480, P_ANIMGROUP=0x4D4, P_IKFLAGS=0x528, P_STATE=0x530,
                 P_MOVESTATE=0x534, P_SWIM=0x538, P_ROT=0x558, P_PEDTYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, P_LOOKDIR=0x73C; // CPed
constexpr size_t I_TASKS=4, I_SCAN=0x130;                                                                   // CPedIntelligence
constexpr size_t T_SIZE=0x3C;
constexpr uintptr_t VT_GUN=0x86D724, TIMESTEP=0xB7CB5C, CAMMODE=0xB6F1A8, IKMAN=0xC15448, CAMERA=0xB6F028;

alignas(16) inline std::array<std::array<uint8,sizeof(CPlayerPed)>,NPEDS+1> peds{};   // [NPEDS] = entidad que no es peaton
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NPEDS+1> mats{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPedIntelligence)>,NPEDS> intels{};
alignas(16) inline std::array<std::array<uint8,0x40>,NPEDS+1> clumps{};
alignas(16) inline std::array<uint8,0x50> task{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<std::array<uint8,0x20>,NHIER> hiers{};
alignas(16) inline std::array<std::array<uint8,0xB0>,2> pds{};
alignas(16) inline std::array<uint8,0x70> wi{};
alignas(16) inline std::array<uint8,0x40> duck{}, otherGun{}, simple{}, pad{}, event{};
alignas(16) inline std::array<float,2> moveDir{};
inline std::array<void*,128> pedVt{};
inline std::array<void*,12> simpleVt{};
inline CPed* Ped(int n) { return reinterpret_cast<CPed*>(peds[n].data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(peds[NPEDS].data()); }
inline CAnimBlendAssociation* Assoc(int n) { return reinterpret_cast<CAnimBlendAssociation*>(assocs[n].data()); }
inline int PedIndex(const void* p) { for(int n=0;n<=NPEDS;++n)if(p==peds[n].data())return n; return -1; }

// pool de tareas (cabecera CPool: m_Storage, m_SlotState, m_Capacity, m_LastFreeSlot)
constexpr size_t TASK_SLOT=0x80;
inline uint8* TaskHdr() { return reinterpret_cast<uint8*>(GetTaskPool()); }
inline uint8* Storage(uint8* h) { return *reinterpret_cast<uint8**>(h); }
inline uint8* States(uint8* h) { return *reinterpret_cast<uint8**>(h+4); }
inline int32 Cap(uint8* h) { return *reinterpret_cast<int32*>(h+8); }
inline int32& Last(uint8* h) { return At<int32>(h,0xC); }
inline int SlotIndex(uint8* h,size_t slot,uintptr_t v) {
    const auto s=uintptr_t(Storage(h)); if(v<s||v>=s+size_t(Cap(h))*slot||(v-s)%slot)return -1; return int((v-s)/slot);
}
inline bool IsPoolTask(uintptr_t v) { const int k=SlotIndex(TaskHdr(),TASK_SLOT,v); return k>=0&&!(States(TaskHdr())[k]&0x80); }
inline void ResetPool(uint8* h,size_t slot,int32 last,const std::vector<uint8>& statesStart,const std::vector<bool>& occStart,const std::vector<bool>& occNow) {
    Last(h)=last;
    if(occNow==occStart)std::memcpy(States(h),statesStart.data(),statesStart.size());
    for(int32 k=0;k<Cap(h);++k)if(States(h)[k]&0x80)std::memset(Storage(h)+k*slot,0xCD,slot);
}

// callbacks de las asociaciones: el EXE guarda 0x4D1390/0x61F3A0 y la DLL sus propias funciones
inline uintptr_t cbDefault{}, cbFinish{};
inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==0x4D1390||v==cbDefault||v==Body(cbDefault))return "default_cb";
    if(v==0x61F3A0||v==cbFinish||v==Body(cbFinish))return "finish_cb";
    if(v==uintptr_t(pedVt.data()))return "ped_vt"; if(v==uintptr_t(simpleVt.data()))return "simple_vt";
    if(v==IKMAN)return "ikman"; if(v==CAMERA)return "camera";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& [b,id]:freshIds)if(v>b&&v<b+TASK_SLOT)return "T"+std::to_string(id)+"+"+std::to_string(v-b);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int n=0;n<=NPEDS;++n){ if(auto s=in(peds[n].data(),peds[n].size(),"ped"))return *s+"#"+std::to_string(n); if(auto s=in(mats[n].data(),mats[n].size(),"mat"))return *s+"#"+std::to_string(n);
                               if(auto s=in(clumps[n].data(),clumps[n].size(),"clump"))return *s+"#"+std::to_string(n); }
    for(int n=0;n<NPEDS;++n)if(auto s=in(intels[n].data(),intels[n].size(),"intel"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NASSOC;++n)if(auto s=in(assocs[n].data(),assocs[n].size(),"as"))return *s+"#"+std::to_string(n);
    for(int n=0;n<NHIER;++n)if(auto s=in(hiers[n].data(),hiers[n].size(),"hier"))return *s+"#"+std::to_string(n);
    for(int n=0;n<2;++n)if(auto s=in(pds[n].data(),pds[n].size(),"pd"))return *s+"#"+std::to_string(n);
    if(auto s=in(task.data(),task.size(),"task"))return *s; if(auto s=in(wi.data(),wi.size(),"wi"))return *s;
    if(auto s=in(duck.data(),duck.size(),"duck"))return *s; if(auto s=in(otherGun.data(),otherGun.size(),"other_gun"))return *s;
    if(auto s=in(simple.data(),simple.size(),"simple"))return *s; if(auto s=in(pad.data(),pad.size(),"pad"))return *s;
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(moveDir.data(),sizeof(moveDir),"move_dir"))return *s;
    if(IsPoolTask(v))return "pool_task";
    // locales de la pila del llamante: su direccion depende del marco (EXE frente a DLL); se comparan los valores apuntados
    { const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); if(v>=uintptr_t(tib->StackLimit)&&v<uintptr_t(tib->StackBase))return "stack"; }
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t from,size_t to) { json r=json::array(); for(size_t k=from;k<to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
inline json Vec(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}
inline json RefList(void* e) { json l=json::array(); for(auto* r=static_cast<CEntity*>(e)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline json AllRefs() { json r=json::array(); for(auto& p:peds)r.push_back(RefList(p.data())); return r; }
inline void CleanRefs() { for(auto& p:peds){ auto* e=reinterpret_cast<CEntity*>(p.data()); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); } }
inline void Reg(void* ent,void* slot) { if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(slot)); }
inline CVector& PosRef(void* e) { auto* m=At<uint8*>(e,E_MATRIX); return m ? At<CVector>(m,0x30) : At<CVector>(e,E_POS); }
// tarea serializada sin vptr (se compara el tipo)
inline json SerTask(const void* p) { const int type=const_cast<CTask*>(static_cast<const CTask*>(p))->GetTaskType(); return {{"type",type},{"words",Region(p,4,T_SIZE)}}; }

// configuracion por caso de las dependencias simuladas
inline uint32 armMask{}, stanceMask{}, blendCalls{}, addCalls{};
inline bool lookRet{}, facingRet{}, duckInUse{}, duckStop{}, fireRet{}, flashRet{}, dirRet{}, blendNullFirst{};
inline uintptr_t duckRet{}, useGunRet{}, padRet{};
inline int16 walkUD{}, walkLR{}, sayRet{};
inline uint8 skillRet{}; inline int32 fireState{}, simpleType{};
inline std::array<float,NPEDS+1> headOff{};

// IKChainManager_c
inline bool __stdcall FIsArmPointing(int32 arm,CPed* p) { const bool r=((armMask>>(arm&7))&1)!=0; trace.push_back({"is_arm_pointing",arm,Name(p),r}); return r; }
inline void __stdcall FAbortPointArm(int32 arm,CPed* p,int32 blend) { trace.push_back({"abort_point_arm",arm,Name(p),blend}); }
inline bool __fastcall FIsLooking(void* m,void*,CPed* p) { trace.push_back({"is_looking",Name(m),Name(p),lookRet}); return lookRet; }
inline void __fastcall FAbortLookAt(void* m,void*,CPed* p,uint32 blend) { trace.push_back({"abort_look_at",Name(m),Name(p),blend}); }
inline void __fastcall FLookAt(void* m,void*,const char* why,CPed* p,CEntity* e,int32 time,int32 bone,CVector* off,uint32 torso,float speed,int32 blend,int32 prio,uint32 force) {
    trace.push_back({"look_at",Name(m),why ? std::string(why) : std::string("<null>"),Name(p),Name(e),time,bone,Name(off),Vec(off),torso&0xFF,F(speed),blend,prio,force&0xFF});
}
inline void __fastcall FPointArm(void* m,void*,const char* why,int32 arm,CPed* p,CEntity* e,int32 bone,CVector* off,float speed,int32 blend,float cull) {
    trace.push_back({"point_arm",Name(m),why ? std::string(why) : std::string("<null>"),arm,Name(p),Name(e),bone,Name(off),Vec(off),F(speed),blend,F(cull)});
}
inline bool __fastcall FIsFacing(void* m,void*,CPed* p,int32 slot) { trace.push_back({"is_facing_target",Name(m),Name(p),slot,facingRet}); return facingRet; }
// CPed
inline void __fastcall FBonePos(CPed* p,void*,CVector* out,int32 bone,uint32 upd) {
    const int n=PedIndex(p); const CVector b=PosRef(p);
    *out=CVector{b.x+0.01f*float(bone),b.y-0.02f*float(bone),b.z+(bone==5&&n>=0 ? headOff[n] : 0.5f)};
    trace.push_back({"bone_position",Name(p),bone,upd&0xFF,Vec(out)});
}
inline void __fastcall FTransformedBone(CPed* p,void*,CVector* io,int32 bone,uint32 upd) {
    const CVector in=*io, b=PosRef(p);
    *io=CVector{in.x+b.x+0.003f*float(bone),in.y+b.y-0.001f*float(bone),in.z+b.z+0.25f};
    trace.push_back({"transformed_bone_position",Name(p),bone,upd&0xFF,Vec(&in),Vec(io)});
}
inline bool __fastcall FIsAlive(CPed* p,void*) { const int32 s=At<int32>(p,P_STATE); const bool r=s!=54&&s!=55; trace.push_back({"is_alive",Name(p),r}); return r; }
inline void __fastcall FSetMoveState(CPed* p,void*,int32 s) { trace.push_back({"set_move_state",Name(p),s}); }
inline bool __fastcall FGunFlash(CPed* p,void*,int32 life,uint32 left) { trace.push_back({"gun_flash",Name(p),life,left&0xFF,flashRet}); return flashRet; }
// el EXE empuja el contexto con basura en los 16 bits altos: solo se traza el int16
inline int16 __fastcall FSay(CPed* p,void*,int ctx,uint32 delay,float prob,bool overrideSilence,bool forceAudible,bool frontEnd) {
    trace.push_back({"say",Name(p),int16(ctx),delay,F(prob),overrideSilence,forceAudible,frontEnd}); return sayRet;
}
inline eWeaponSkill __fastcall FSkill(CPed* p,void*,int32 type) { trace.push_back({"weapon_skill",Name(p),type,skillRet}); return eWeaponSkill(skillRet); }
inline CPad* __fastcall FPadFromPlayer(CPlayerPed* p,void*) { trace.push_back({"pad_from_player",Name(p),Name(padRet)}); return reinterpret_cast<CPad*>(padRet); }
inline int16 __fastcall FWalkUD(CPad* p,void*) { trace.push_back({"walk_up_down",Name(p),walkUD}); return walkUD; }
inline int16 __fastcall FWalkLR(CPad* p,void*) { trace.push_back({"walk_left_right",Name(p),walkLR}); return walkLR; }
// CPedIK
inline void __fastcall FRotateTorso(void* ik,void*,const CVector* d) { trace.push_back({"rotate_torso_for_arm",Name(ik),Vec(d)}); }
inline void __fastcall FPointAtPos(void* ik,void*,const CVector* pos,float blend) { trace.push_back({"point_gun_at_position",Name(ik),Name(pos),Vec(pos),F(blend)}); }
inline bool __fastcall FPointInDir(void* ik,void*,float z,float dist,uint32 flag,float blend) { trace.push_back({"point_gun_in_direction",Name(ik),F(z),F(dist),flag&0xFF,F(blend),dirRet}); return dirRet; }
// CWeapon::Fire: escribe el estado del arma para probar la lectura posterior
inline bool __fastcall FFire(CWeapon* w,void*,CEntity* by,CVector* start,CVector* barrel,CEntity* target,CVector* tpos,CVector* alt) {
    trace.push_back({"fire",Name(w),Name(by),Name(start),Vec(start),Name(barrel),Vec(barrel),Name(target),Name(tpos),Vec(tpos),Name(alt)});
    At<int32>(w,4)=fireState; return fireRet;
}
inline void __fastcall FFind3rd(void* cam,void*,float range,float mx,float my,float mz,CVector* src,CVector* tgt) {
    *src=CVector{mx+1.f,my,mz-0.5f}; *tgt=CVector{mx+range*0.5f,my+1.5f,mz-0.25f};
    trace.push_back({"find_3rd_person_cam_target",Name(cam),F(range),F(mx),F(my),F(mz),Name(src),Name(tgt)});
}
// CPedIntelligence / CTaskSimpleDuck
inline void* __fastcall FUseGun(void* i,void*) { trace.push_back({"get_task_use_gun",Name(i),Name(useGunRet)}); return reinterpret_cast<void*>(useGunRet); }
inline void* __fastcall FDuck(void* i,void*,uint32 ignore) { trace.push_back({"get_task_duck",Name(i),ignore&0xFF,Name(duckRet)}); return reinterpret_cast<void*>(duckRet); }
inline bool __fastcall FDuckInUse(void* d,void*) { trace.push_back({"duck_in_use",Name(d),duckInUse}); return duckInUse; }
inline bool __fastcall FDuckStopFire(void* d,void*) { trace.push_back({"duck_stop_fire_gun",Name(d),duckStop}); return duckStop; }
inline void __fastcall FDuckForceStop(void* d,void*) { trace.push_back({"duck_force_stop_move",Name(d)}); }
// animaciones
inline CAnimBlendAssociation* __cdecl FGetAssoc(RpClump* c,uint32 id) {
    CAnimBlendAssociation* r=nullptr; if(id>=49&&id<=53&&((stanceMask>>(id-49))&1))r=Assoc(1+int(id-49));
    trace.push_back({"get_association",Name(c),id,Name(r)}); return r;
}
inline CAnimBlendAssociation* __cdecl FBlend(RpClump* c,int32 group,int32 id,float delta) {
    CAnimBlendAssociation* r=blendCalls==0&&blendNullFirst ? nullptr : Assoc(6+int(blendCalls%3)); ++blendCalls;
    trace.push_back({"blend_animation",Name(c),group,id,F(delta),Name(r)}); return r;
}
inline CAnimBlendAssociation* __cdecl FAddAnim(RpClump* c,int32 group,int32 id) {
    CAnimBlendAssociation* r=Assoc(9+int(addCalls%2)); ++addCalls; trace.push_back({"add_animation",Name(c),group,id,Name(r)}); return r;
}
inline void __fastcall FSetTime(CAnimBlendAssociation* a,void*,float t) { At<float>(a,0x20)=t; trace.push_back({"set_current_time",Name(a),F(t)}); }
// tarea simple falsa (GetSimplestActiveTask real): GetSubTask 2 nulo, IsSimple 3, GetTaskType 4 trazado
inline CTask* __fastcall FSub(CTask*,void*) { return nullptr; }
inline bool __fastcall FIsSimple(CTask*,void*) { return true; }
inline int32 __fastcall FType(CTask* t,void*) { trace.push_back({"simple_task_type",Name(t),simpleType}); return simpleType; }
template<int N> void* __fastcall FTrap(void* self,void*) { trace.push_back({"unexpected_slot",N,Name(self)}); return nullptr; }
template<size_t S,int... N> void FillTraps(std::array<void*,S>& vt,std::integer_sequence<int,N...>) { ((vt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=18;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleUseGun","AbortIK",0x61DFA0,0,-1,256},
    {"CTaskSimpleUseGun","AimGun",0x61ED10,0,-1,1024},
    {"CTaskSimpleUseGun","ClearAnim",0x61E190,0,-1,256},
    {"CTaskSimpleUseGun","Clone",0,0x86D724,1,128},
    {"CTaskSimpleUseGun","ControlGun",0x61E040,0,-1,512},
    {"CTaskSimpleUseGun","ControlGunMove",0x61E0C0,0,-1,1024},
    {"CTaskSimpleUseGun","FinishGunAnimCB",0x61F3A0,0,-1,256},
    {"CTaskSimpleUseGun","FireGun",0x61EB10,0,-1,512},
    {"CTaskSimpleUseGun","GetTaskType",0,0x86D724,4,64},
    {"CTaskSimpleUseGun","MakeAbortable",0,0x86D724,6,512},
    {"CTaskSimpleUseGun","PlayerPassiveControlGun",0x61E0A0,0,-1,256},
    {"CTaskSimpleUseGun","RemoveStanceAnims",0x61E8E0,0,-1,1024},
    {"CTaskSimpleUseGun","RequirePistolWhip",0x61E200,0,-1,1024},
    {"CTaskSimpleUseGun","Reset",0x624DC0,0,-1,256},
    {"CTaskSimpleUseGun","SetMoveAnim",0x61E3F0,0,-1,1024},
    {"CTaskSimpleUseGun","SetPedPosition",0,0x86D724,8,512},
    {"CTaskSimpleUseGun","StartAnim",0x624F30,0,-1,1024},
    {"CTaskSimpleUseGun","StartCountDown",0x61E160,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"gun19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"gun19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("gun19");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"gun19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    cbDefault=reinterpret_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB); cbFinish=reinterpret_cast<uintptr_t>(&CTaskSimpleUseGun::FinishGunAnimCB);
    FillTraps(pedVt,std::make_integer_sequence<int,128>{}); FillTraps(simpleVt,std::make_integer_sequence<int,12>{});
    simpleVt[2]=reinterpret_cast<void*>(&FSub); simpleVt[3]=reinterpret_cast<void*>(&FIsSimple); simpleVt[4]=reinterpret_cast<void*>(&FType);
    const auto occupancyStart=Occupancy();
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    const int32 lf0=Last(TaskHdr());
    const std::vector<uint8> statesStart(States(TaskHdr()),States(TaskHdr())+Cap(TaskHdr()));
    // estado global tocado: paso de tiempo y modo de la camara 0
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TIMESTEP,4},{CAMMODE,2}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return true; };
    auto& timeStep=*reinterpret_cast<float*>(TIMESTEP); auto& camMode=*reinterpret_cast<int16*>(CAMMODE);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch k1(Body(bit_cast<uintptr_t>(&IKChainManager_c::IsArmPointing)),FIsArmPointing), k1e(0x6182B0,FIsArmPointing);
        EntryPatch k2(Body(bit_cast<uintptr_t>(&IKChainManager_c::AbortPointArm)),FAbortPointArm), k2e(0x6182F0,FAbortPointArm);
        EntryPatch k3(Body(bit_cast<uintptr_t>(&IKChainManager_c::IsLooking)),FIsLooking), k3e(0x6181A0,FIsLooking);
        EntryPatch k4(Body(bit_cast<uintptr_t>(&IKChainManager_c::AbortLookAt)),FAbortLookAt), k4e(0x618280,FAbortLookAt);
        EntryPatch k5(Body(bit_cast<uintptr_t>(&IKChainManager_c::LookAt)),FLookAt), k5e(0x618970,FLookAt);
        EntryPatch k6(Body(bit_cast<uintptr_t>(&IKChainManager_c::PointArm)),FPointArm), k6e(0x618B60,FPointArm);
        EntryPatch k7(Body(bit_cast<uintptr_t>(&IKChainManager_c::IsFacingTarget)),FIsFacing), k7e(0x618330,FIsFacing);
        EntryPatch p1(Body(bit_cast<uintptr_t>(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition))),FBonePos), p1e(0x5E4280,FBonePos);
        EntryPatch p2(Body(bit_cast<uintptr_t>(&CPed::GetTransformedBonePosition)),FTransformedBone), p2e(0x5E01C0,FTransformedBone);
        EntryPatch p3(Body(bit_cast<uintptr_t>(&CPed::IsAlive)),FIsAlive), p3e(0x5E0170,FIsAlive);
        EntryPatch p4(Body(bit_cast<uintptr_t>(&CPed::SetMoveState)),FSetMoveState), p4e(0x5DEC00,FSetMoveState);
        EntryPatch p5(Body(bit_cast<uintptr_t>(&CPed::DoGunFlash)),FGunFlash), p5e(0x5DF340,FGunFlash);
        EntryPatch p6(Body(bit_cast<uintptr_t>(static_cast<int16(CPed::*)(eGlobalSpeechContext,uint32,float,bool,bool,bool)>(&CPed::Say))),FSay), p6e(0x5EFFE0,FSay);
        EntryPatch p7(Body(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)(eWeaponType)>(&CPed::GetWeaponSkill))),FSkill), p7e(0x5E3B60,FSkill);
        EntryPatch p8(Body(bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer)),FPadFromPlayer), p8e(0x609560,FPadFromPlayer);
        EntryPatch p9(Body(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown))),FWalkUD), p9e(0x53FD30,FWalkUD);
        EntryPatch p10(Body(bit_cast<uintptr_t>(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight))),FWalkLR), p10e(0x53FC90,FWalkLR);
        EntryPatch i1(Body(bit_cast<uintptr_t>(&CPedIK::RotateTorsoForArm)),FRotateTorso), i1e(0x5FDF90,FRotateTorso);
        EntryPatch i2(Body(bit_cast<uintptr_t>(&CPedIK::PointGunAtPosition)),FPointAtPos), i2e(0x5FDE20,FPointAtPos);
        EntryPatch i3(Body(bit_cast<uintptr_t>(&CPedIK::PointGunInDirection)),FPointInDir), i3e(0x5FDC00,FPointInDir);
        EntryPatch w1(Body(bit_cast<uintptr_t>(&CWeapon::Fire)),FFire), w1e(0x742300,FFire);
        EntryPatch c1(Body(bit_cast<uintptr_t>(&CCamera::Find3rdPersonCamTargetVector)),FFind3rd), c1e(0x514970,FFind3rd);
        EntryPatch t1(Body(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun)),FUseGun), t1e(0x600F70,FUseGun);
        EntryPatch t2(Body(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskDuck)),FDuck), t2e(0x6010A0,FDuck);
        EntryPatch d1(Body(bit_cast<uintptr_t>(&CTaskSimpleDuck::IsTaskInUseByOtherTasks)),FDuckInUse), d1e(0x61C3D0,FDuckInUse);
        EntryPatch d2(Body(bit_cast<uintptr_t>(&CTaskSimpleDuck::StopFireGun)),FDuckStopFire), d2e(0x61C420,FDuckStopFire);
        EntryPatch d3(Body(bit_cast<uintptr_t>(&CTaskSimpleDuck::ForceStopMove)),FDuckForceStop), d3e(0x6924B0,FDuckForceStop);
        EntryPatch a1(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),FGetAssoc), a1e(0x4D68B0,FGetAssoc);
        EntryPatch a2(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation))),FBlend), a2e(0x4D4610,FBlend);
        EntryPatch a3(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation))),FAddAnim), a3e(0x4D3AA0,FAddAnim);
        EntryPatch a4(Body(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetCurrentTime)),FSetTime), a4e(0x4CEA80,FSetTime);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto pickF=[&](std::initializer_list<float> l){ return *(l.begin()+next()%l.size()); };
                auto pickI=[&](std::initializer_list<int32> l){ return *(l.begin()+next()%l.size()); };
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    ResetPool(TaskHdr(),TASK_SLOT,lf0,statesStart,occupancyStart,Occupancy());
                    restoreGlobals();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1919u; if(!st)st=1; trace=json::array();
                    freshIds.clear(); blendCalls=0; addCalls=0;
                    CleanRefs();
                    auto cmd=[&]()->int8{ return next()%12 ? int8(int32(next()%9)-1) : int8(pickI({8,-2,100,-128,127})); };
                    auto fl=[&]{ const uint32 m=next()%12; return m<7 ? uni(-3.f,3.f) : m==7 ? 0.f : m==8 ? -0.f : m==9 ? 1.f : m==10 ? 0.5f : (next()%3==0 ? qnan : 2.f); };
                    timeStep=pickF({1.f,0.5f,2.5f,0.f,uni(0.1f,3.f),uni(0.1f,3.f)});
                    camMode=int16(pickI({0x35,0x41,4,0x2E}));

                    // peaton 0: el de la tarea; 1..5 alrededor (escaner/objetivos); [NPEDS]: entidad que no es peaton
                    const float h=uni(-3.14f,3.14f), ch=std::cos(h), sh=std::sin(h);
                    const CVector base{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)};
                    for(int n=0;n<=NPEDS;++n){
                        auto* p=peds[n].data(); std::memset(p,0,peds[n].size()); std::memset(mats[n].data(),0,mats[n].size());
                        std::memset(clumps[n].data(),0,clumps[n].size()); clumps[n][0]=2;
                        At<void*>(p,0)=pedVt.data(); At<void*>(p,E_RWOBJ)=clumps[n].data();
                        p[E_TYPE]=uint8(n<NPEDS ? 3 : 4);
                        if(n==0||next()%4){
                            At<void*>(p,E_MATRIX)=mats[n].data();
                            const float hh=n==0 ? h : uni(-3.14f,3.14f);
                            At<CVector>(mats[n].data(),0x00)=CVector{std::cos(hh),std::sin(hh),uni(-0.05f,0.05f)};
                            At<CVector>(mats[n].data(),0x10)=CVector{-std::sin(hh),std::cos(hh),uni(-0.05f,0.05f)};
                            At<CVector>(mats[n].data(),0x20)=CVector{0.f,0.f,1.f};
                        }
                        if(n==0)PosRef(p)=base;
                        else { const float a=uni(-0.6f,1.4f), b=uni(-0.9f,0.9f); PosRef(p)=CVector{base.x-sh*a+ch*b,base.y+ch*a+sh*b,base.z+uni(-1.2f,1.2f)};
                               if(next()%8==0)PosRef(p).x+=uni(-5.f,5.f); }
                        headOff[n]=pickF({0.f,1.5f,uni(-0.4f,2.f),uni(0.2f,1.4f)});
                        if(n==NPEDS)continue;
                        std::memset(intels[n].data(),0,intels[n].size()); At<void*>(p,P_INTEL)=intels[n].data();
                        At<void*>(intels[n].data(),I_TASKS)=simple.data();
                        for(int k=0;k<4;++k)At<uint32>(p,P_FLAGS+4*k)=next();
                        At<uint32>(p,P_IKFLAGS)=next();
                        At<int32>(p,P_STATE)=pickI({1,54,55,0x2D,1});
                        At<int32>(p,P_ANIMGROUP)=pickI({0,3,54,57,58,59,32});
                        At<float>(p,P_ROT)=uni(-3.2f,3.2f); At<float>(p,P_LOOKDIR)=uni(-3.2f,3.2f);
                        p[P_SLOT]=uint8(next()%13);
                        At<int32>(p,P_WEAPONS+0x1C*p[P_SLOT])=22+int32(next()%11); At<int32>(p,P_WEAPONS+0x1C*p[P_SLOT]+4)=int32(next()%4);
                    }
                    // datos de jugador: pd#0 del peaton 0, pd#1 del peaton 2 (a veces)
                    for(int k=0;k<2;++k){
                        for(auto& b:pds[k])b=uint8(next()&0x3F);
                        At<float>(pds[k].data(),0x14)=uni(0.f,2.f); At<float>(pds[k].data(),0x2C)=uni(-1.f,5.f);
                        At<uint32>(pds[k].data(),0x34)=next(); At<float>(pds[k].data(),0x54)=uni(-1.f,1.f);
                        At<int32>(pds[k].data(),0x70)=pickI({3,5,24,34,0});
                        At<CVector>(pds[k].data(),0x74)=CVector{uni(-0.2f,0.2f),uni(-0.2f,0.2f),uni(-0.2f,0.2f)};
                    }
                    const bool hasPd=next()%4!=0;
                    At<void*>(peds[0].data(),P_PD)=hasPd ? pds[0].data() : nullptr;
                    if(next()%2)At<void*>(peds[2].data(),P_PD)=pds[1].data();
                    // tipo: jugador solo con datos de jugador (el EXE los usa sin comprobar nulo)
                    for(int n=0;n<NPEDS;++n){ const bool pdn=At<void*>(peds[n].data(),P_PD)!=nullptr;
                        At<int32>(peds[n].data(),P_PEDTYPE)=pdn&&next()%3 ? pickI({0,1}) : pickI({4,6,7,15,16,20}); }
                    // CWeaponInfo falso
                    for(auto& b:wi)b=uint8(next()&0x3F);
                    { auto* w=wi.data(); At<int32>(w,0)=int32(next()%6); At<uint32>(w,0x18)=next(); At<int32>(w,0x1C)=pickI({0,25,26,30,57});
                      At<CVector>(w,0x24)=CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.3f,0.3f)}; At<float>(w,0x3C)=uni(0.5f,2.f);
                      const float s1=uni(0.f,0.5f), s2=uni(0.f,0.5f);
                      At<float>(w,0x40)=s1; At<float>(w,0x44)=s1+uni(0.1f,0.6f); At<float>(w,0x48)=s1+uni(0.f,0.3f);
                      At<float>(w,0x4C)=s2; At<float>(w,0x50)=s2+uni(0.1f,0.6f); At<float>(w,0x54)=s2+uni(0.f,0.3f); At<float>(w,0x58)=uni(0.f,1.f); }
                    // jerarquias y asociaciones
                    for(int k=0;k<NHIER;++k){ std::memset(hiers[k].data(),0,hiers[k].size()); At<float>(hiers[k].data(),0x10)=pickF({0.5f,1.f,2.f,uni(0.2f,3.f)}); }
                    for(int k=0;k<NASSOC;++k){
                        auto* a=assocs[k].data(); std::memset(a,0,assocs[k].size());
                        At<void*>(a,0x14)=hiers[next()%NHIER].data();
                        At<float>(a,0x18)=pickF({0.f,0.5f,0.95f,0.96f,0.98f,0.99f,1.f,-0.1f,uni(0.f,1.f)});
                        At<float>(a,0x1C)=pickF({0.f,-4.f,-8.f,-7.9f,4.f,-0.f,-1000.f,uni(-9.f,9.f)});
                        At<float>(a,0x20)=pickF({0.f,0.5f,1.f,uni(0.f,2.f),uni(0.f,2.f)}); At<float>(a,0x24)=uni(0.5f,1.5f); At<float>(a,0x28)=pickF({0.f,0.1f,uni(0.f,0.3f)});
                        At<int16>(a,0x2C)=int16(pickI({0xE0,0xE1,0xE2,0xE3,0x31,0xD6,3}));
                        At<uint16>(a,0x2E)=uint16(next());
                        At<int32>(a,0x30)=int32(next()%3); At<uint32>(a,0x34)=uint32(pickI({0,0x4D1390,0x61F3A0})); At<void*>(a,0x38)=next()%2 ? task.data() : nullptr;
                    }
                    std::memset(simple.data(),0,simple.size()); At<void*>(simple.data(),0)=simpleVt.data();
                    std::memset(duck.data(),0,duck.size()); std::memset(pad.data(),0,pad.size());
                    for(auto& b:otherGun)b=uint8(next()&0x3F); otherGun[0x10]=uint8(pickI({5,4,1,0}));
                    for(auto& b:event)b=uint8(next()&0x3F);
                    stanceMask=next()&0x1F; armMask=next(); lookRet=next()%2; facingRet=next()%2; duckInUse=next()%2; duckStop=next()%2;
                    fireRet=next()%2; flashRet=next()%2; dirRet=next()%2; blendNullFirst=false; fireState=int32(next()%3);
                    duckRet=next()%4 ? uintptr_t(duck.data()) : 0; useGunRet=next()%2 ? uintptr_t(otherGun.data()) : 0; padRet=next()%4 ? uintptr_t(pad.data()) : 0;
                    walkUD=int16(pickI({0,50,51,-50,-51,100,-100,20})); walkLR=int16(pickI({0,50,51,-50,-51,100,-100,20})); sayRet=int16(next()%40);
                    skillRet=uint8(next()%3); simpleType=pickI({205,205,200,1017});
                    // escaner del peaton 0
                    for(int k=0;k<16;++k)At<void*>(intels[0].data(),I_SCAN+4*k)=next()%3==0 ? peds[1+next()%(NPEDS-1)].data() : nullptr;

                    // tarea
                    auto* t=task.data(); std::memset(t,0xCD,task.size());
                    At<uint32>(t,0)=VT_GUN; At<uint32>(t,4)=0;
                    t[8]=uint8(next()%2); t[9]=uint8(next()%2); t[0xA]=uint8(next()%2); t[0xB]=uint8(next()%2); t[0xC]=uint8(next()%2);
                    t[0xD]=uint8(next()); t[0xE]=uint8(next()%2); t[0xF]=uint8(cmd()); t[0x10]=uint8(cmd());
                    At<float>(t,0x14)=pickF({0.f,0.75f,-0.75f,0.8f,-0.8f,0.05f,-0.04f,uni(-1.f,1.f)}); At<float>(t,0x18)=pickF({0.f,0.05f,-0.05f,1.f,-1.f,uni(-1.f,1.f)});
                    { const uint32 m=next()%4; void* e=m==0 ? nullptr : m==1 ? static_cast<void*>(peds[2].data()) : m==2 ? static_cast<void*>(peds[3].data()) : static_cast<void*>(peds[NPEDS].data());
                      At<void*>(t,0x1C)=e; Reg(e,t+0x1C); }
                    { const uint32 m=next()%6; CVector v{fl(),fl(),fl()}; if(m==0)v.x=v.y=0.f; else if(m==1)v.x=0.f; else if(m==2)v.y=0.f; At<CVector>(t,0x20)=v; }
                    At<void*>(t,0x2C)=next()%4 ? assocs[0].data() : nullptr;
                    At<void*>(t,0x30)=wi.data();
                    At<uint16>(t,0x34)=uint16(pickI({0,1,3,5,0xFFFF,0x8000})); At<uint16>(t,0x36)=uint16(pickI({0,1,2,3,0xFFFF,0x8000,0x7FFF}));
                    t[0x38]=uint8(pickI({0,1,10,255,int32(next()%256)})); t[0x39]=uint8(next()%2); t[0x3A]=uint8(next()%2); t[0x3B]=uint8(next()%2);
                    auto* p0=peds[0].data();
                    auto setFlag=[&](size_t off,uint32 bit,bool on){ auto& w=At<uint32>(p0,off); w=on ? (w|bit) : (w&~bit); };

                    // ajustes por funcion
                    json args=json::array();
                    if(nm=="AimGun"){
                        At<void*>(t,0x2C)=assocs[0].data();
                        At<float>(assocs[0].data(),0x18)=pickF({0.5f,0.98f,0.99f,1.f,0.981f});
                        At<uint32>(wi.data(),0x18)=(At<uint32>(wi.data(),0x18)&~2u)|(next()%4 ? 2u : 0u);
                        setFlag(P_FLAGS,0x4000000,next()%4==0); setFlag(P_FLAGS+8,0x20000,next()%2==0);
                        if(next()%3==0){ At<uint32>(pds[0].data(),0x34)|=8; camMode=int16(pickI({0x35,0x41})); At<float>(t,0x20)=0.f; At<float>(t,0x24)=0.f; At<void*>(t,0x1C)=nullptr; CleanRefs(); }
                    }
                    else if(nm=="FireGun"){
                        setFlag(P_FLAGS+8,0x400,next()%2==0);
                    }
                    else if(nm=="SetPedPosition"){
                        auto* wp=p0+P_WEAPONS+0x1C*p0[P_SLOT];
                        const uint32 m=next()%4;
                        At<void*>(t,0x30)=m==0 ? static_cast<void*>(wi.data()) :
                            static_cast<void*>(CWeaponInfo::GetWeaponInfo(eWeaponType(At<int32>(wp,0)),eWeaponSkill(m==3 ? (skillRet+1)%3 : skillRet)));
                        setFlag(P_FLAGS+8,0x400,next()%2==0);
                    }
                    else if(nm=="StartAnim"){
                        if(next()%2){ At<float>(t,0x14)=0.f; At<float>(t,0x18)=0.f; }
                        setFlag(P_FLAGS,0x4000000,next()%2==0);
                    }
                    else if(nm=="RemoveStanceAnims"||nm=="MakeAbortable"){
                        if(next()%4==0)At<void*>(t,0x30)=nullptr;
                        auto* a=assocs[0].data(); const uint32 m=next()%4;
                        if(m==0)At<float>(a,0x20)=At<float>(wi.data(),0x40); else if(m==1)At<float>(a,0x20)=At<float>(At<uint8*>(a,0x14),0x10);
                        if(next()%3==0)stanceMask=0;
                        if(next()%3==0)t[0xF]=6;
                    }
                    else if(nm=="SetMoveAnim"){
                        setFlag(P_FLAGS,0x4000000,next()%5==0);
                        // FIX_BUGS: sin mover, la DLL no pone a cero m_MoveCmd (el EXE si): en ese camino el movimiento ya es (0,0);
                        // con movimiento se asegura el control (bMoveFire si la ultima orden es disparar) y |x|+|y| >= 0.1
                        if(next()%2){ At<float>(t,0x14)=0.f; At<float>(t,0x18)=0.f; if(!t[0xA]&&next()%3==0)blendNullFirst=true; }
                        else {
                            t[0xA]=1; if(t[0x10]==2)At<uint32>(wi.data(),0x18)|=0x20;
                            auto& mx=At<float>(t,0x14); auto& my=At<float>(t,0x18);
                            mx=pickF({0.75f,-0.75f,0.8f,-0.8f,0.f,0.05f,uni(-1.f,1.f)}); my=pickF({0.f,1.f,-1.f,0.05f,-0.06f,uni(-1.f,1.f)});
                            if(next()%24==0)my=qnan; else if(std::abs(mx)+std::abs(my)<0.1f)my=next()%2 ? 0.5f : -0.5f;
                        }
                        auto* gs=assocs[1].data(); At<float>(gs,0x1C)=pickF({0.f,-1.f,2.f,-0.f,0.5f}); At<float>(gs,0x18)=pickF({0.f,0.01f,0.5f,1.f,uni(0.f,1.f)});
                    }
                    else if(nm=="RequirePistolWhip"){
                        for(int n=1;n<NPEDS;++n){ auto& fw=At<uint32>(peds[n].data(),P_FLAGS+8); fw=next()%3==0 ? (fw|0x100000u) : (fw&~0x100000u); }
                    }
                    else if(nm=="FinishGunAnimCB"){
                        const uint32 m=next()%3; At<void*>(t,0x2C)=m==0 ? nullptr : m==1 ? assocs[0].data() : assocs[10].data();
                        At<uint16>(t,0x36)=uint16(pickI({0,1,2,0xFFFF,0x8000,0x7FFF}));
                        if(next()%2)t[0x10]=4;
                    }

                    const auto occ=Occupancy();
                    const std::string wiBefore=Hash(wi.data(),wi.size());
                    for(auto* hk:hooks)hk->State(candidate);
                    const auto vt=reinterpret_cast<uintptr_t*>(fn.vt); const uintptr_t target=fn.slot>=0 ? vt[fn.slot] : fn.address;
                    const bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    json out={{"switches",switches}}; json ret=nullptr;
                    CPed* ped=Ped(0);
                    std::vector<int> freshSlots;
                    if(nm=="AbortIK"||nm=="AimGun"||nm=="ClearAnim"||nm=="SetMoveAnim"||nm=="StartAnim")
                        reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(t,ped);
                    else if(nm=="Clone"){
                        CTask* r=reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(t);
                        for(auto* hk:hooks)hk->State(false);
                        auto* pool=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshSlots.push_back(int(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                        json tasks=json::array(); for(auto* x:fresh)tasks.push_back(SerTask(x)); out["tasks"]=tasks; ret=Name(r);
                    }
                    else if(nm=="ControlGun"){
                        const uint32 m=next()%5; void* e=m==0 ? nullptr : m==1 ? At<void*>(t,0x1C) : m==2 ? static_cast<void*>(peds[2].data()) : m==3 ? static_cast<void*>(peds[4].data()) : static_cast<void*>(peds[NPEDS].data());
                        const int8 c=cmd(); args={Name(e),c};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,CEntity*,int32)>(target)(t,ped,static_cast<CEntity*>(e),int32(c));
                    }
                    else if(nm=="ControlGunMove"){
                        const float maxDelta=timeStep*0.07f;
                        for(int k=0;k<2;++k){
                            const float cur=At<float>(t,0x14+4*k)=next()%3 ? uni(-120.f,120.f) : pickF({0.f,0.5f,-0.5f,1.f});
                            const uint32 m=next()%8;
                            moveDir[k]=m==0 ? cur+maxDelta : m==1 ? cur-maxDelta : m==2 ? cur+2.f*maxDelta : m==3 ? cur-uni(0.f,3.f)*maxDelta :
                                       m==4 ? uni(-120.f,120.f) : m==5 ? cur+uni(-1.f,1.f)*maxDelta : m==6 ? (next()%3==0 ? qnan : cur) : cur+maxDelta*1.0000001f;
                        }
                        args={F(moveDir[0]),F(moveDir[1])};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,const float*)>(target)(t,moveDir.data());
                    }
                    else if(nm=="FinishGunAnimCB"){
                        CAnimBlendAssociation* a=next()%3 ? Assoc(0) : Assoc(10);
                        reinterpret_cast<void(__cdecl*)(CAnimBlendAssociation*,void*)>(target)(a,t);
                    }
                    else if(nm=="FireGun"){
                        const uint32 left=next()%2; args={left};
                        reinterpret_cast<void(__thiscall*)(void*,CPed*,uint32)>(target)(t,ped,left);
                    }
                    else if(nm=="GetTaskType")ret=reinterpret_cast<int32(__thiscall*)(void*)>(target)(t);
                    else if(nm=="MakeAbortable"){
                        const int32 prio=pickI({0,1,2,3,-1,7,1,2}); const void* ev=next()%2 ? event.data() : nullptr; args={prio,Name(ev)};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int32,const void*)>(target)(t,ped,prio,ev);
                    }
                    else if(nm=="PlayerPassiveControlGun")ret=reinterpret_cast<bool(__thiscall*)(void*)>(target)(t);
                    else if(nm=="RemoveStanceAnims"){
                        const float x=pickF({4.f,8.f,-4.f,uni(-10.f,10.f)}); args={F(x)};
                        reinterpret_cast<void(__thiscall*)(void*,CPed*,float)>(target)(t,ped,x);
                    }
                    else if(nm=="RequirePistolWhip"){
                        const uint32 m=next()%6; void* e=m<2 ? nullptr : m<4 ? static_cast<void*>(peds[1+next()%(NPEDS-1)].data()) : m==4 ? static_cast<void*>(peds[NPEDS].data()) : static_cast<void*>(peds[2].data());
                        args={Name(e)}; ret=reinterpret_cast<bool(__cdecl*)(CPed*,CEntity*)>(target)(ped,static_cast<CEntity*>(e));
                    }
                    else if(nm=="Reset"){
                        const uint32 m=next()%4; void* e=m==0 ? nullptr : m==1 ? At<void*>(t,0x1C) : m==2 ? static_cast<void*>(peds[4].data()) : static_cast<void*>(peds[NPEDS].data());
                        const CVector v{fl(),fl(),fl()}; const int8 c=cmd(); const int32 burst=pickI({0,1,3,-1,-32768,32767,int32(int16(next()))});
                        args={Name(e),Vec(&v),c,burst};
                        reinterpret_cast<void(__thiscall*)(void*,CPed*,CEntity*,float,float,float,int32,int32)>(target)(t,ped,static_cast<CEntity*>(e),v.x,v.y,v.z,int32(c),burst);
                    }
                    else if(nm=="SetPedPosition")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(t,ped);
                    else if(nm=="StartCountDown"){
                        const uint32 n=next()%256, isMax=pickI({0,1,0x80,0}); args={n,isMax};
                        reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(target)(t,n,isMax);
                    }
                    for(auto* hk:hooks)hk->State(false);
                    out["args"]=args; out["ret"]=ret; out["task"]=Region(t,0,T_SIZE); out["guard"]=Hash(t+T_SIZE,task.size()-T_SIZE);
                    { json as=json::array(); for(int k=0;k<NASSOC;++k)as.push_back(Region(assocs[k].data(),0,0x3C)); out["assocs"]=as; }
                    out["ped0"]=json::array({Region(p0,P_FLAGS,P_PD+4),Region(p0,0x50C,0x540)});
                    { json hs=json::array(); for(auto& p:peds)hs.push_back(EntHash(p)); for(auto& in:intels)hs.push_back(Hash(in.data(),in.size())); out["ents"]=hs; }
                    out["pds"]=json::array({Region(pds[0].data(),0,0xAC),Region(pds[1].data(),0,0xAC)});
                    out["buffers"]=json::array({wiBefore==Hash(wi.data(),wi.size()),Hash(duck.data(),duck.size()),Hash(otherGun.data(),otherGun.size()),Hash(simple.data(),simple.size()),
                                                Hash(pad.data(),pad.size()),Hash(event.data(),event.size()),Hash(hiers.data(),sizeof(hiers))});
                    out["globals"]={F(timeStep),camMode};
                    out["refs"]=AllRefs(); out["trace"]=trace;
                    // limpieza: referencias de los fixtures y huecos de las tareas nuevas (sin destructor); el pool debe quedar como antes
                    CleanRefs();
                    for(int k:freshSlots)GetTaskPool()->SetFreeAt(size_t(k),true);
                    restoreGlobals();
                    out["clean"]=Occupancy()==occ;
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
    ResetPool(TaskHdr(),TASK_SLOT,lf0,statesStart,occupancyStart,Occupancy());
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    const bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&Last(TaskHdr())==lf0&&globalsIntact();
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
