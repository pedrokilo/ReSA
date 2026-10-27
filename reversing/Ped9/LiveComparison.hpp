#pragma once
// Banco dentro del motor: IK del peaton, efectos de armas, cadena IK, lanzar y recoger bici (lista de 9, 45 funciones):
// CPedIK, CWeaponEffects, CTaskSimpleIKChain, CTaskSimpleThrowProjectile y CTaskSimplePickUpBike.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos: los constructores se llaman en la
// direccion del EXE sobre un buffer rellenado con 0xCD (se compara el tipo en vez del vptr); los metodos virtuales se
// llaman por la entrada de la vtable del EXE. Peatones (vtable falsa trazada), clump (byte de tipo 2) con datos de
// animacion falsos (33 frames con su cuaternion), jerarquia de huesos falsa (32 nodos, matrices propias), frames RW
// falsos, inteligencia con tarea activa falsa, bici falsa (matriz y handling propios), asociaciones de animacion,
// evento de dano, cadena IK y texturas falsas. Se simulan y trazan: GetAnimHierarchyFromSkinClump, RtQuatRotate,
// RwMatrixCreate/Destroy, CEntity::UpdateRwFrame, CPed::GetWeaponSkill/IsPlayer, CAnimManager::BlendAnimation,
// CAnimBlendAssociation::SetFinishCallback, IKChainManager_c::AddIKChain/RemoveIKChain, IKChain_c::SetBlend/UpdateTarget,
// CWeapon::Fire, CTaskUtilityLineUpPedWithCar::ProcessPed, CVehicleAnimGroup::GetGroup, CTxdStore (Push/Pop/Find/Set),
// RwTextureRead/Destroy, CSprite (CalcScreenCoors y los dos sprites), el puntero de RwRenderStateSet del motor,
// CPlayerPed::GetWeaponRadiusOnScreen, CGameLogic::IsCoopGameGoingOn, CWeapon::FindNearestTargetEntityWithScreenCoors
// y la mira auxiliar del jugador (0x56EF90). Se guardan y restauran el reloj, ms_fTimeStep, bRotateWithNeck, el byte
// 0xC0AFC0, las miras y texturas de CWeaponEffects y sus estaticos de Render, la camara (pantalla ancha, matriz, modo y
// punteria de la camara activa) y los peatones de CWorld::Players. Las tareas nuevas del pool se siguen
// (m_LastFreeSlot restaurado y huecos libres con 0xCD).
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedIK.h"
#include "WeaponEffects.h"
#include "Tasks/TaskTypes/TaskSimpleIKChain.h"
#include "Tasks/TaskTypes/TaskSimpleThrowProjectile.h"
#include "Tasks/TaskTypes/TaskSimplePickUpBike.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/IKChainManager.h"
#include "PedIntelligence.h"
#include "Pools/TaskPool.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "VehicleAnimGroupData.h"
#include "Sprite.h"
#include "GameLogic.h"
#include "TxdStore.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "General.h"
#include "Entity/Vehicle/Bike.h"
#include "../ProbeRuntime.hpp"

namespace Ped9Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// desplazamientos crudos del EXE (CPed / CEntity / CVehicle)
constexpr size_t P_MATRIX=0x14, P_CLUMP=0x18, P_FLAGS_ENT=0x1C, P_MODEL=0x22, P_FLAGS1=0x46C, P_FLAGS2=0x470, P_FLAGS3=0x474,
                 P_FLAGS4=0x478, P_INTEL=0x47C, P_PLAYERDATA=0x480, P_BONES=0x488, P_STATE=0x530, P_MOVESTATE=0x534,
                 P_ROT=0x558, P_VEHICLE=0x58C, P_WEAPONS=0x5A0, P_WSLOT=0x718, V_DRIVER=0x460, V_HANDLING=0x384, V_BIKEFLAGS=0x614;
constexpr uintptr_t ADDR_TIME_MS=0xB7CB84, ADDR_TIME_STEP=0xB7CB5C, ADDR_IKMAN=0xC15448, ADDR_NECK=0x8D2354, ADDR_ABSASIN=0xC0AFC0,
                    ADDR_TORSOINFO=0x8D22E4, ADDR_TEX=0xC8A810, ADDR_CROSS=0xC8A838, ADDR_RSTAT=0xC8A890, ADDR_WSTAT=0x8D6140,
                    ADDR_WIDESCREEN=0xB6F065, ADDR_CAMMAT=0xB6F99C, ADDR_ACTIVECAM=0xB6F081, ADDR_CAMS=0xB6F1A8, ADDR_PLAYERS=0xB7CD98,
                    ADDR_ENGINE=0xC97B24, ADDR_CLUMPOFFSET=0xB5F878, EXE_DEFAULT_ANIM_CB=0x4D1390, EXE_THROW_CB=0x61F890,
                    EXE_BIKE_CB=0x6462F0, EXE_EMPTY_STR=0x858B54, ADDR_VEHANIMGROUPS=0xC1CDC0;
constexpr size_t NFR=33, NNODE=32, NASSOC=3;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedA{}, pedB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matA{}, matB{}, vmat{};
alignas(16) inline std::array<uint8,0x200> ent{};
alignas(16) inline std::array<uint8,sizeof(CBike)> veh{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> frm{};
alignas(16) inline std::array<uint8,0x20> clumpData{};
alignas(16) inline std::array<uint8,0x18*NFR> frames{};
alignas(16) inline std::array<uint8,0x20*NFR> quats{};
alignas(16) inline std::array<uint8,0x40> hier{};
alignas(16) inline std::array<uint8,0x10*NNODE> nodes{};
alignas(16) inline std::array<uint8,0x40*NFR> matrices{};
alignas(16) inline std::array<uint8,0x40> tmpMat{}, outMat{}, ikObj{}, fakeTask{}, limb{}, info{}, vec{};
alignas(16) inline std::array<uint8,0x100> playerData{}, handling{}, lineUp{}, chain{}, event{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,NASSOC> assoc{};
alignas(16) inline std::array<uint8,0x80> object{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> tex{};
alignas(16) inline std::array<uint8,0x40> rasters{};
inline const char purposeName[]{"Ped9"};
inline CPed* PedA() { return reinterpret_cast<CPed*>(pedA.data()); }
inline CPed* PedB() { return reinterpret_cast<CPed*>(pedB.data()); }
inline CEntity* Ent() { return reinterpret_cast<CEntity*>(ent.data()); }
inline CEntity* Veh() { return reinterpret_cast<CEntity*>(veh.data()); }
inline std::array<void*,64> pedVtbl{};
inline std::array<void*,16> eventVtbl{}, assocVtbl{}, taskVtbl{};
inline std::unordered_map<uintptr_t,int> freshIds;

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline void InitBufs() {
    bufs={{pedA.data(),pedA.size(),"pedA"},{pedB.data(),pedB.size(),"pedB"},{matA.data(),matA.size(),"matA"},{matB.data(),matB.size(),"matB"},
          {vmat.data(),vmat.size(),"vmat"},{ent.data(),ent.size(),"ent"},{veh.data(),veh.size(),"veh"},{intel.data(),intel.size(),"intel"},
          {clump.data(),clump.size(),"clump"},{frm[0].data(),frm[0].size(),"frm0"},{frm[1].data(),frm[1].size(),"frm1"},
          {frm[2].data(),frm[2].size(),"frm2"},{frm[3].data(),frm[3].size(),"frm3"},{clumpData.data(),clumpData.size(),"clumpdata"},
          {frames.data(),frames.size(),"frames"},{quats.data(),quats.size(),"quats"},{hier.data(),hier.size(),"hier"},
          {nodes.data(),nodes.size(),"nodes"},{matrices.data(),matrices.size(),"matrices"},{tmpMat.data(),tmpMat.size(),"tmpmat"},
          {outMat.data(),outMat.size(),"outmat"},{ikObj.data(),ikObj.size(),"ik"},{fakeTask.data(),fakeTask.size(),"faketask"},
          {limb.data(),limb.size(),"limb"},{info.data(),info.size(),"info"},{vec.data(),vec.size(),"vec"},
          {playerData.data(),playerData.size(),"playerdata"},{handling.data(),handling.size(),"handling"},{lineUp.data(),lineUp.size(),"lineup"},
          {chain.data(),chain.size(),"chain"},{event.data(),event.size(),"event"},{assoc[0].data(),assoc[0].size(),"assoc0"},
          {assoc[1].data(),assoc[1].size(),"assoc1"},{assoc[2].data(),assoc[2].size(),"assoc2"},{object.data(),object.size(),"object"},
          {tex[0].data(),tex[0].size(),"tex0"},{tex[1].data(),tex[1].size(),"tex1"},{tex[2].data(),tex[2].size(),"tex2"},
          {rasters.data(),rasters.size(),"rasters"},{pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"},{eventVtbl.data(),sizeof(eventVtbl),"eventvtbl"},
          {assocVtbl.data(),sizeof(assocVtbl),"assocvtbl"},{taskVtbl.data(),sizeof(taskVtbl),"taskvtbl"}};
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==EXE_DEFAULT_ANIM_CB||v==std::bit_cast<uintptr_t>(&CDefaultAnimCallback::DefaultAnimCB))return "DefaultAnimCB";
    if(v==EXE_THROW_CB||v==std::bit_cast<uintptr_t>(&CTaskSimpleThrowProjectile::FinishAnimThrowProjectileCB))return "ThrowFinishCB";
    if(v==EXE_BIKE_CB||v==std::bit_cast<uintptr_t>(&CTaskSimplePickUpBike::FinishAnimPickUpBikeCB))return "BikeFinishCB";
    if(v==ADDR_IKMAN)return "ikman";
    if(v==uintptr_t(purposeName))return "purpose";
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline json RawWords(uintptr_t a,size_t n) { json r=json::array(); for(size_t k=0;k<n/4;++k)r.push_back(Name(AtA(a+4*k))); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// configuracion por caso de las dependencias simuladas
inline uintptr_t blendRet{}, chainRet{}, nearestRet{};
inline uint32 skillRet{}, texReads{}; inline int fakeTaskType{}, eventType{};
inline bool isPlayerRet{}, coopRet{}; inline float radiusRet{};
inline uint32 stubSt{};
inline uint32 SNext() { stubSt^=stubSt<<13; stubSt^=stubSt>>17; stubSt^=stubSt<<5; return stubSt; }
inline float SUni(float a,float b) { return a+(b-a)*float(SNext()%100000)/100000.f; }

// vtables falsas trazadas
template<int N> void* __fastcall PTrap(CPed*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
inline int __fastcall EType(void* e,void*) { trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
template<int N> void* __fastcall ETrap(void*,void*) { trace.push_back({"event_unexpected_slot",N}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
inline void* __fastcall ADel(void* a,void*,uint32 flags) { trace.push_back({"assoc_delete",Name(uintptr_t(a)),flags}); return a; }
template<int N> void* __fastcall ATrap(void*,void*) { trace.push_back({"assoc_unexpected_slot",N}); return nullptr; }
template<int... N> void FillATraps(std::integer_sequence<int,N...>) { ((assocVtbl[N]=reinterpret_cast<void*>(&ATrap<N>)),...); }
inline int __fastcall TType(void* t,void*) { trace.push_back({"task_get_type",Name(uintptr_t(t))}); return fakeTaskType; }
template<int N> void* __fastcall TTrap(void*,void*) { trace.push_back({"task_unexpected_slot",N}); return nullptr; }
template<int... N> void FillTTraps(std::integer_sequence<int,N...>) { ((taskVtbl[N]=reinterpret_cast<void*>(&TTrap<N>)),...); }

// dependencias simuladas: IK del peaton
inline RpHAnimHierarchy* __cdecl HierStub(RpClump* c) { trace.push_back({"anim_hierarchy_from_skin_clump",Name(uintptr_t(c))}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline RtQuat* __cdecl QuatRot(RtQuat* q,const RwV3d* a,float ang,int32 op) { trace.push_back({"quat_rotate",Name(uintptr_t(q)),F(a->x),F(a->y),F(a->z),F(ang),op}); return q; }
inline RwMatrix* __cdecl MatCreate() { trace.push_back({"rw_matrix_create"}); return reinterpret_cast<RwMatrix*>(tmpMat.data()); }
inline RwBool __cdecl MatDestroy(RwMatrix* m) { trace.push_back({"rw_matrix_destroy",Name(uintptr_t(m))}); return 1; }
inline void __fastcall UpdFrame(CEntity* e,void*) { trace.push_back({"update_rw_frame",Name(uintptr_t(e))}); }
inline uint32 __fastcall SkillStub(CPed* p,void*) { trace.push_back({"get_weapon_skill",Name(uintptr_t(p))}); return skillRet; }
inline bool __fastcall IsPlayerStub(CPed* p,void*) { trace.push_back({"ped_is_player",Name(uintptr_t(p))}); return isPlayerRet; }
// tareas
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return reinterpret_cast<CAnimBlendAssociation*>(blendRet); }
inline void __fastcall SetFinishCb(CAnimBlendAssociation* a,void*,void* cb,void* data) { trace.push_back({"set_finish_callback",Name(uintptr_t(a)),Name(uintptr_t(cb)),Name(uintptr_t(data))}); }
inline IKChain_c* __fastcall AddChain(void* m,void*,const char* nm,uint32 slot,CPed* p,int32 eff,float ex,float ey,float ez,int32 pivot,CEntity* e,int32 offBone,float ox,float oy,float oz,float speed,int32 prio) {
    trace.push_back({"ik_add_chain",Name(uintptr_t(m)),nm ? std::string(nm) : std::string("<null>"),slot,Name(uintptr_t(p)),eff,F(ex),F(ey),F(ez),pivot,Name(uintptr_t(e)),offBone,F(ox),F(oy),F(oz),F(speed),prio});
    return reinterpret_cast<IKChain_c*>(chainRet);
}
inline void __fastcall RemoveChain(void* m,void*,void* c) { trace.push_back({"ik_remove_chain",Name(uintptr_t(m)),Name(uintptr_t(c))}); }
inline void __fastcall ChainBlend(void* c,void*,float b) { trace.push_back({"ik_chain_set_blend",Name(uintptr_t(c)),F(b)}); }
inline void __fastcall ChainTarget(void* c,void*,uint32 b) { trace.push_back({"ik_chain_update_target",Name(uintptr_t(c)),b&0xFF}); }
inline json Vec3(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }
inline bool __fastcall FireStub(void* w,void*,CEntity* by,CVector* start,CVector* barrel,CEntity* tgt,CVector* tpos,CVector* alt) {
    trace.push_back({"weapon_fire",Name(uintptr_t(w)),Name(uintptr_t(by)),Vec3(start),start==barrel,Name(uintptr_t(tgt)),Name(uintptr_t(tpos)),Vec3(tpos),Name(uintptr_t(alt))});
    return true;
}
inline bool __fastcall LineUpStub(void* t,void*,CPed* p,CVehicle* v,CAnimBlendAssociation* a) { trace.push_back({"line_up_process_ped",Name(uintptr_t(t)),Name(uintptr_t(p)),Name(uintptr_t(v)),Name(uintptr_t(a))}); return true; }
inline int32 __fastcall GroupStub(void* g,void*,int32 animId) {
    const auto off=int32(intptr_t(g)-intptr_t(ADDR_VEHANIMGROUPS));
    trace.push_back({"vehicle_anim_group_get_group",off,animId});
    return off/0x94+animId%7;
}
// efectos de armas
inline RwTexture* __cdecl TexRead(const char* n,const char* m) { trace.push_back({"texture_read",std::string(n),std::string(m)}); return reinterpret_cast<RwTexture*>(tex[texReads++%3].data()); }
inline RwBool __cdecl TexDestroy(RwTexture* t) { trace.push_back({"texture_destroy",Name(uintptr_t(t))}); return 1; }
inline void __cdecl TxdPush() { trace.push_back({"txd_push"}); }
inline void __cdecl TxdPop() { trace.push_back({"txd_pop"}); }
inline int32 __cdecl TxdFind(const char* n) { trace.push_back({"txd_find",std::string(n)}); return 77; }
inline void __cdecl TxdSet(int32 s) { trace.push_back({"txd_set",s}); }
inline RwBool __cdecl RSStub(int32 st,void* v) { trace.push_back({"render_state",st,Name(uintptr_t(v))}); return 1; }
inline bool __cdecl CalcScreen(const RwV3d& pos,RwV3d* out,float* w,float* h,uint32 cmax,uint32 cmin) {
    trace.push_back({"calc_screen_coors",F(pos.x),F(pos.y),F(pos.z),cmax&0xFF,cmin&0xFF});
    const float W=float(RsGlobal.maximumWidth), H=float(RsGlobal.maximumHeight);
    const uint32 mx=SNext()%10, my=SNext()%10;
    out->x = mx==0 ? 0.f : mx==1 ? W : mx==2 ? -1.f : mx==3 ? W+1.f : SUni(-40.f,W+40.f);
    out->y = my==0 ? 0.f : my==1 ? H : my==2 ? -1.f : my==3 ? H+1.f : SUni(-40.f,H+40.f);
    out->z = SUni(0.5f,5.f);
    *w = SUni(1.f,60.f); *h = SNext()%3 ? SUni(1.f,19.f) : SUni(20.f,40.f);
    return SNext()%5!=0;
}
inline void __cdecl RotAspect(float x,float y,float z,float w,float h,uint32 r,uint32 g,uint32 b,uint32 a,float rz,float rot,uint32 al) {
    trace.push_back({"sprite_rotate_aspect",F(x),F(y),F(z),F(w),F(h),r&0xFF,g&0xFF,b&0xFF,a&0xFFFF,F(rz),F(rot),al&0xFF});
}
inline void __cdecl Tri(float ax,float ay,float bx,float by,float cx,float cy,float z,uint32 r,uint32 g,uint32 b,uint32 a,float rz,uint32 al) {
    trace.push_back({"sprite_triangle",F(ax),F(ay),F(bx),F(by),F(cx),F(cy),F(z),r&0xFF,g&0xFF,b&0xFF,a&0xFFFF,F(rz),al&0xFF});
}
inline float __fastcall Radius(void* p,void*) { trace.push_back({"weapon_radius_on_screen",Name(uintptr_t(p))}); return radiusRet; }
inline bool __cdecl Coop() { trace.push_back({"is_coop_game"}); return coopRet; }
inline CEntity* __cdecl Nearest(float x,float y,float range,float px,float py,float pz,float* ox,float* oy) {
    trace.push_back({"find_nearest_target_screen",F(x),F(y),F(range),F(px),F(py),F(pz),Name(uintptr_t(ox)),Name(uintptr_t(oy))});
    return reinterpret_cast<CEntity*>(nearestRet);
}
inline void __fastcall AuxCross(void* t,void*,int32 i) { trace.push_back({"player_crosshair_aux",int32(intptr_t(t)-intptr_t(ADDR_PLAYERS)),i}); }
// Desde el lote 74 la DLL llama a CWeaponEffects::RenderPlayerCrosshair (C++) en lugar de a 0x56EF90: mismo registro, recibe el CPlayerInfo por referencia
inline void __cdecl AuxCrossDll(CPlayerInfo& info,int32 i) { trace.push_back({"player_crosshair_aux",int32(intptr_t(&info.m_nCrosshairActivated)-intptr_t(ADDR_PLAYERS)),i}); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
enum Kind { PEDIK, WFX, IKCHAIN, THROW, BIKE };
struct Cls { const char* cls; uintptr_t vt; size_t size; Kind kind; };
inline const std::array<Cls,5> classes{{
    {"CPedIK",0,0x20,PEDIK}, {"CWeaponEffects",0,0x2C,WFX}, {"CTaskSimpleIKChain",0x86DB48,0x58,IKCHAIN},
    {"CTaskSimpleThrowProjectile",0x86D790,0x24,THROW}, {"CTaskSimplePickUpBike",0x86ECE4,0x1C,BIKE},
}};
inline const Cls& ClassOf(const char* cls) { for(auto& c:classes)if(!std::strcmp(c.cls,cls))return c; std::abort(); }
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CPedIK","Constructor",0x5FD8C0,0,-1,64},{"CPedIK","GetWorldMatrix",0x5FD8F0,0,-1,256},{"CPedIK","MoveLimb-non-normalized",0x5FDA60,0,-1,1024},
    {"CPedIK","MoveLimb-normalized",0x5FDB60,0,-1,1024},{"CPedIK","PitchForSlope",0x5FE0E0,0,-1,1024},{"CPedIK","PointGunAtPosition",0x5FDE20,0,-1,512},
    {"CPedIK","PointGunInDirection",0x5FDC00,0,-1,1024},{"CPedIK","RotateTorso",0x5FDDB0,0,-1,256},{"CPedIK","RotateTorsoForArm",0x5FDF90,0,-1,1024},
    {"CWeaponEffects","ClearCrossHair",0x742C60,0,-1,64},{"CWeaponEffects","ClearCrossHairImmediately",0x742CA0,0,-1,64},{"CWeaponEffects","ClearCrossHairs",0x742C80,0,-1,64},
    {"CWeaponEffects","ClearCrossHairsImmediately",0x742CC0,0,-1,64},{"CWeaponEffects","Init",0x742AB0,0,-1,64},{"CWeaponEffects","IsLockedOn",0x742BD0,0,-1,128},
    {"CWeaponEffects","MarkTarget",0x742BF0,0,-1,128},{"CWeaponEffects","Render",0x742CF0,0,-1,1024},{"CWeaponEffects","Shutdown",0x742B80,0,-1,64},
    {"CTaskSimpleIKChain","BlendOut",0x633C40,0x86DB48,-1,256},{"CTaskSimpleIKChain","Clone",0,0x86DB48,1,256},{"CTaskSimpleIKChain","Constructor",0x6339C0,0x86DB48,-1,256},
    {"CTaskSimpleIKChain","CreateIKChain",0,0x86DB48,9,256},{"CTaskSimpleIKChain","Destructor",0x633A90,0x86DB48,-1,128},{"CTaskSimpleIKChain","GetIKChain",0x633C70,0x86DB48,-1,64},
    {"CTaskSimpleIKChain","GetTaskType",0,0x86DB48,4,64},{"CTaskSimpleIKChain","MakeAbortable",0,0x86DB48,6,256},{"CTaskSimpleIKChain","ProcessPed",0,0x86DB48,7,1024},
    {"CTaskSimpleThrowProjectile","Clone",0,0x86D790,1,128},{"CTaskSimpleThrowProjectile","Constructor",0x61F660,0x86D790,-1,128},{"CTaskSimpleThrowProjectile","ControlThrow",0x61F810,0x86D790,-1,256},
    {"CTaskSimpleThrowProjectile","Destructor",0x61F700,0x86D790,-1,128},{"CTaskSimpleThrowProjectile","FinishAnimThrowProjectileCB",0x61F890,0x86D790,-1,128},{"CTaskSimpleThrowProjectile","GetTaskType",0,0x86D790,4,64},
    {"CTaskSimpleThrowProjectile","MakeAbortable",0,0x86D790,6,256},{"CTaskSimpleThrowProjectile","ProcessPed",0,0x86D790,7,1024},{"CTaskSimpleThrowProjectile","StartAnim",0x6259E0,0x86D790,-1,512},
    {"CTaskSimplePickUpBike","Clone",0,0x86ECE4,1,128},{"CTaskSimplePickUpBike","Constructor",0x6461B0,0x86ECE4,-1,128},{"CTaskSimplePickUpBike","Destructor",0x646230,0x86ECE4,-1,128},
    {"CTaskSimplePickUpBike","FinishAnimPickUpBikeCB",0x6462F0,0x86ECE4,-1,64},{"CTaskSimplePickUpBike","GetTaskType",0,0x86ECE4,4,64},{"CTaskSimplePickUpBike","MakeAbortable",0,0x86ECE4,6,256},
    {"CTaskSimplePickUpBike","ProcessPed",0,0x86ECE4,7,512},{"CTaskSimplePickUpBike","SetPedPosition",0,0x86ECE4,8,128},{"CTaskSimplePickUpBike","StartAnim",0x64AE90,0x86ECE4,-1,256},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(PedA()); clean(PedB()); clean(Ent()); clean(Veh());
}
// referencia registrada en un campo del objeto probado (como si la hubiera creado su constructor)
inline bool registerRefs{};
inline void RegisterAt(void* o,size_t off,CEntity* e) { At<CEntity*>(o,off)=e; if(e&&registerRefs)e->RegisterReference(&At<CEntity*>(o,off)); }
// matriz de rotacion aleatoria con posicion (solo los 0x40 bytes de la matriz: no toca el puntero de enganche)
inline void RandomMatrix(void* dst,float yaw,float pitch,float roll,const CVector& pos) {
    CMatrix m{}; m.SetRotate(pitch,roll,yaw); m.GetPosition()=pos; std::memcpy(dst,&m,0x40);
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ped9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ped9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ped9");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"ped9-only.txt"); if(f)std::getline(f,only); }
    InitBufs();
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // CMatrix::RotateX/RotateY (0x59B1E0/0x59B2C0) son colaboradores de PitchForSlope: el PitchForSlope original los llama por la direccion del EXE, que la DLL tiene enganchada.
    // Se desactivan durante todo el banco para que el original use el codigo del EXE y el candidato (PedIKRotateXCpp/PedIKRotateYCpp) se compare con el.
    ReversibleHooks::ReversibleHook::Base* const matrixHooks[2]{FindHook("CMatrix","RotateX"),FindHook("CMatrix","RotateY")};
    // CMatrix::InjectHooks esta desactivado en InjectHooksMain (no hay hooks de RotateX/RotateY): el EXE se ejecuta entonces sin desviar. Se exige que existan los dos o ninguno.
    if(bool(matrixHooks[0])!=bool(matrixHooks[1])){Write({{"event","error"},{"reason","hook not found"},{"function","CMatrix::RotateX/RotateY"}});return;}
    const int16 pedModel=player->m_nModelIndex; int16 vehModel=-1;
    for(int32 m=400;m<612&&vehModel<0;++m)if(CModelInfo::GetModelInfo(m))vehModel=int16(m);
    const uint32 clumpOffset=AtA(ADDR_CLUMPOFFSET);
    if(vehModel<0||!CModelInfo::GetModelInfo(pedModel)||clumpOffset+4>clump.size()){Write({{"event","error"},{"reason","model info missing or clump offset"},{"clump_offset",clumpOffset}});return;}
    // armas de fuego con desplazamiento de punteria valido (PointGunAtPosition lo indexa)
    std::vector<int32> aimWeapons;
    for(int32 t=22;t<=34;++t){ if(!CWeaponInfo::TypeHasSkillStats(eWeaponType(t)))continue; /* GetWeaponSkill simulado devuelve 0..3: solo armas con habilidad */ bool ok=true; for(uint32 s=0;s<4;++s){ auto* wi=CWeaponInfo::GetWeaponInfo(eWeaponType(t),eWeaponSkill(s)); if(!wi||wi->m_nAimOffsetIndex>=21 /* ms_WeaponAimOffsets (0xC8A8A8): PYTHON..SPRAYCAN */)ok=false; } if(ok)aimWeapons.push_back(t); }
    if(aimWeapons.empty()){Write({{"event","error"},{"reason","no aim weapons"}});return;}
    FillPTraps(std::make_integer_sequence<int,64>{});
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType);
    FillATraps(std::make_integer_sequence<int,16>{}); assocVtbl[0]=reinterpret_cast<void*>(&ADel);
    FillTTraps(std::make_integer_sequence<int,16>{}); taskVtbl[4]=reinterpret_cast<void*>(&TType);
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    // estado global tocado: se guarda aqui y se restaura antes de cada ejecucion y al final
    const uint8 activeCam=AtA<uint8>(ADDR_ACTIVECAM);
    const uintptr_t camMode=ADDR_CAMS+activeCam*0x238, camTarget=0xB6F2A4+activeCam*0x238;
    const uintptr_t rsSlot=AtA<uintptr_t>(ADDR_ENGINE)+0x20;
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> saved;
    auto snap=[&](uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); };
    snap(ADDR_TIME_MS,4); snap(ADDR_TIME_STEP,4); snap(ADDR_NECK,1); snap(ADDR_ABSASIN,1); snap(ADDR_TORSOINFO,0x18);
    snap(ADDR_TEX,0xC); snap(ADDR_CROSS,0x58); snap(ADDR_RSTAT,0x18); snap(ADDR_WSTAT,0xC); snap(ADDR_WIDESCREEN,1); snap(ADDR_CAMMAT,0x40);
    snap(camMode,2); snap(camTarget,8); snap(ADDR_PLAYERS,4); snap(ADDR_PLAYERS+0x190,4); snap(rsSlot,4);
    auto restoreGlobals=[&]{ for(auto& [a,b]:saved)std::memcpy(reinterpret_cast<void*>(a),b.data(),b.size()); };
    auto globalsIntact=[&]{ for(auto& [a,b]:saved)if(std::memcmp(reinterpret_cast<void*>(a),b.data(),b.size()))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    std::array<bool,2> matrixEnabled{},matrixLocked{};
    for(size_t k=0;k<2;++k){if(!matrixHooks[k])continue;matrixEnabled[k]=matrixHooks[k]->Hooked();matrixLocked[k]=matrixHooks[k]->Locked();matrixHooks[k]->LockState(false);matrixHooks[k]->State(false);}
    bool passed=true; int pair=0;
    // etiquetas de los nodos de la jerarquia falsa (incluyen los huesos que usan PitchForSlope y las tareas)
    static const int32 tags[NNODE]{0,1,2,3,4,5,6,7,8,21,22,23,24,25,26,31,32,33,34,35,36,41,42,43,44,51,52,53,54,201,301,302};
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch h1(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),HierStub), h1e(0x734A40,HierStub);
        EntryPatch h2(0x7EB7C0,QuatRot), h3(0x7F2A50,MatCreate), h4(0x7F2A20,MatDestroy);
        EntryPatch h5(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),UpdFrame), h5e(0x532B00,UpdFrame);
        EntryPatch h6(bit_cast<uintptr_t>(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)),SkillStub), h6e(0x5E6580,SkillStub);
        EntryPatch h7(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayerStub), h7e(0x5DF8F0,IsPlayerStub);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),Blend), a1e(0x4D4610,Blend);
        EntryPatch a2(bit_cast<uintptr_t>(&CAnimBlendAssociation::SetFinishCallback),SetFinishCb), a2e(0x4CEBE0,SetFinishCb);
        EntryPatch k1(bit_cast<uintptr_t>(&IKChainManager_c::AddIKChain),AddChain), k1e(0x618750,AddChain);
        EntryPatch k2(bit_cast<uintptr_t>(&IKChainManager_c::RemoveIKChain),RemoveChain), k2e(0x618170,RemoveChain);
        EntryPatch k3(bit_cast<uintptr_t>(&IKChain_c::SetBlend),ChainBlend), k3e(0x617C10,ChainBlend);
        EntryPatch k4(bit_cast<uintptr_t>(&IKChain_c::UpdateTarget),ChainTarget), k4e(0x617E50,ChainTarget);
        EntryPatch w1(bit_cast<uintptr_t>(&CWeapon::Fire),FireStub), w1e(0x742300,FireStub);
        EntryPatch l1(bit_cast<uintptr_t>(&CTaskUtilityLineUpPedWithCar::ProcessPed),LineUpStub), l1e(0x6513A0,LineUpStub);
        EntryPatch g1(bit_cast<uintptr_t>(&CVehicleAnimGroup::GetGroup),GroupStub), g1e(0x6E3B00,GroupStub);
        EntryPatch t1(0x7F3AC0,TexRead), t2(0x7F3820,TexDestroy);
        EntryPatch x1(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),TxdPush), x1e(0x7316A0,TxdPush);
        EntryPatch x2(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),TxdPop), x2e(0x7316B0,TxdPop);
        EntryPatch x3(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),TxdFind), x3e(0x731850,TxdFind);
        EntryPatch x4(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),TxdSet), x4e(0x7319C0,TxdSet);
        EntryPatch s1(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),CalcScreen), s1e(0x70CE30,CalcScreen);
        EntryPatch s2(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite_Rotate_Aspect),RotAspect), s2e(0x70D490,RotAspect);
        EntryPatch s3(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite_Triangle),Tri), s3e(0x70D320,Tri);
        EntryPatch r1(bit_cast<uintptr_t>(&CPlayerPed::GetWeaponRadiusOnScreen),Radius), r1e(0x609CD0,Radius);
        EntryPatch r2(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),Coop), r2e(0x441390,Coop);
        EntryPatch r3(bit_cast<uintptr_t>(&CWeapon::FindNearestTargetEntityWithScreenCoors),Nearest), r3e(0x73E240,Nearest);
        EntryPatch r4(0x56EF90,AuxCross), r4d(bit_cast<uintptr_t>(&CWeaponEffects::RenderPlayerCrosshair),AuxCrossDll);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; const auto& cl=ClassOf(fn.cls); int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    restoreGlobals();
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9A9Au; if(!st)st=1; trace=json::array();
                    stubSt=st^0x5A5A1234u; if(!stubSt)stubSt=7; texReads=0;
                    CleanRefs();
                    const uint32 now=next()%3 ? 100000u+next()%2000000u : next();
                    AtA(ADDR_TIME_MS)=now; AtA<float>(ADDR_TIME_STEP)=next()%3 ? 1.f : uni(0.1f,3.f);
                    // peatones, entidades y vehiculo
                    for(auto* b:{&pedA,&pedB})std::memset(b->data(),0,b->size());
                    std::memset(matA.data(),0,matA.size()); std::memset(matB.data(),0,matB.size()); std::memset(vmat.data(),0,vmat.size());
                    std::memset(ent.data(),0,ent.size()); std::memset(veh.data(),0,veh.size());
                    std::memset(clump.data(),0,clump.size()); clump[0]=2; // rpCLUMP (GetRpClump lo comprueba en la DLL)
                    Ent()->SetType(ENTITY_TYPE_OBJECT); At<int16>(Ent(),P_MODEL)=vehModel; Ent()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    Veh()->SetType(ENTITY_TYPE_VEHICLE); At<int16>(Veh(),P_MODEL)=vehModel;
                    Veh()->m_matrix=reinterpret_cast<CMatrixLink*>(vmat.data());
                    RandomMatrix(vmat.data(),uni(-3.f,3.f),next()%4==0 ? 0.f : uni(-1.2f,1.2f),uni(-1.f,1.f),CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)});
                    if(next()%8==0)At<float>(vmat.data(),8)=0.f; // right.z exactamente 0 (StartAnim de la bici)
                    At<void*>(Veh(),V_HANDLING)=handling.data(); for(auto& b:handling)b=uint8(next()); handling[0xDE]=uint8(next()%30 /* NUM_VEH_ANIM_GROUPS */);
                    At<uint8>(Veh(),V_BIKEFLAGS)=uint8(next());
                    int32 wslot=int32(next()%13);
                    for(auto* p:{PedA(),PedB()}){
                        p->SetType(ENTITY_TYPE_PED); *reinterpret_cast<void**>(p)=pedVtbl.data(); At<void*>(p,P_CLUMP)=clump.data();
                        At(p,P_FLAGS1)=next(); At(p,P_FLAGS2)=next(); At(p,P_FLAGS3)=next(); At(p,P_FLAGS4)=next();
                        At<uint8>(p,P_FLAGS_ENT)=uint8(next()); At<int16>(p,P_MODEL)=pedModel;
                        { static const uint32 states[]{0x36,0x37,1,7,12,0x36}; At(p,P_STATE)=next()%3 ? states[next()%6] : next()%60; }
                        { static const uint32 moves[]{4,1,6,7,4}; At(p,P_MOVESTATE)=moves[next()%5]; }
                        At<float>(p,P_ROT)=uni(-7.f,7.f);
                        At<uint8>(p,P_WSLOT)=uint8(wslot);
                        At<void*>(p,P_INTEL)=intel.data(); At<void*>(p,P_PLAYERDATA)=playerData.data();
                    }
                    PedA()->m_matrix=reinterpret_cast<CMatrixLink*>(matA.data());
                    RandomMatrix(matA.data(),uni(-3.2f,3.2f),next()%2 ? 0.f : uni(-0.5f,0.5f),next()%2 ? 0.f : uni(-0.5f,0.5f),CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)});
                    if(next()%2){ PedB()->m_matrix=reinterpret_cast<CMatrixLink*>(matB.data()); RandomMatrix(matB.data(),uni(-3.f,3.f),0.f,0.f,CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)}); }
                    else PedB()->SetPosn(uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f));
                    const CVector pedPos=PedA()->GetPosition();
                    for(auto& b:playerData)b=uint8(next());
                    // datos de animacion: frames[0] queda delante para el indice -1
                    for(size_t k=0;k<NFR;++k){ auto* fr=frames.data()+0x18*k; for(size_t b=0;b<0x18;++b)fr[b]=uint8(next());
                        At<void*>(fr,0x10)=quats.data()+0x20*k; At<int32>(fr,0x14)=k ? tags[k-1] : int32(next()%400); }
                    for(auto& b:quats)b=uint8(next());
                    std::memset(clumpData.data(),0,clumpData.size()); At<void*>(clumpData.data(),0x10)=frames.data()+0x18; At<int32>(clumpData.data(),0xC)=int32(NNODE);
                    At<void*>(clump.data(),clumpOffset)=clumpData.data(); At<void*>(clump.data(),4)=frm[3].data();
                    for(size_t k=0;k<19;++k)At<void*>(PedA(),P_BONES+4*k)=frames.data()+0x18*(1+(next()%NNODE));
                    for(size_t k=0;k<19;++k)At<void*>(PedB(),P_BONES+4*k)=frames.data()+0x18*(1+(next()%NNODE));
                    std::memset(hier.data(),0,hier.size()); At<int32>(hier.data(),4)=int32(NNODE); At<void*>(hier.data(),8)=matrices.data()+0x40; At<void*>(hier.data(),0x10)=nodes.data();
                    for(size_t k=0;k<NNODE;++k){ At<int32>(nodes.data(),0x10*k)=tags[k]; At<int32>(nodes.data(),0x10*k+4)=int32(k); At<int32>(nodes.data(),0x10*k+8)=0; At<int32>(nodes.data(),0x10*k+12)=0; }
                    if(next()%8==0)At<int32>(nodes.data(),0x10*(next()%NNODE))=9999; // un hueso sin nodo: indice -1
                    for(size_t k=0;k<NFR*16;++k)At<float>(matrices.data(),4*k)=uni(-1.f,1.f);
                    for(auto& fr:frm){ for(size_t k=0;k<24;++k)At<float>(fr.data(),4*k)=uni(-2.f,2.f); At<uint32>(fr.data(),0x1C)=0; At<void*>(fr.data(),4)=nullptr; }
                    for(size_t k=0;k<3;++k)if(next()%2)At<void*>(frm[k].data(),4)=frm[k+1].data();
                    // inteligencia: tarea activa falsa en un hueco primario
                    std::memset(intel.data(),0,intel.size());
                    for(auto& b:fakeTask)b=uint8(next()); *reinterpret_cast<void**>(fakeTask.data())=taskVtbl.data();
                    if(next()%2)At<void*>(intel.data(),4+4*(next()%5))=fakeTask.data();
                    fakeTaskType=next()%2 ? TASK_SIMPLE_STEALTH_KILL : int(next()%1500);
                    // asociaciones de animacion falsas (floats controlados, sin NaN)
                    for(auto& a:assoc){ for(auto& b:a)b=uint8(next()); *reinterpret_cast<void**>(a.data())=assocVtbl.data();
                        At<float>(a.data(),0x18)=uni(0.f,1.f); At<float>(a.data(),0x1C)=uni(-4.f,4.f);
                        At<float>(a.data(),0x20)=uni(0.f,2.f); At<float>(a.data(),0x24)=uni(0.f,2.f); At<float>(a.data(),0x28)=uni(0.f,0.3f); }
                    for(auto* b:{&chain,&lineUp})for(auto& x:*b)x=uint8(next());
                    std::memset(tmpMat.data(),0xCD,tmpMat.size()); std::memset(outMat.data(),0xCD,outMat.size());
                    for(auto& b:event)b=uint8(next()); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    eventType=next()%3 ? EVENT_DAMAGE : int(next()%100);
                    // dependencias simuladas
                    skillRet=next()%4; isPlayerRet=next()%2; coopRet=next()%2;
                    { static const float radii[]{-1.f,0.f,0.5f,1.5f}; radiusRet=next()%2 ? radii[next()%4] : uni(0.f,2.f); }
                    blendRet=uintptr_t(assoc[2].data()); chainRet=next()%4 ? uintptr_t(chain.data()) : 0;
                    nearestRet=next()%3 ? uintptr_t(ent.data()) : 0;
                    AtA<uint8>(ADDR_NECK)=uint8(next()%2); AtA<uint8>(ADDR_ABSASIN)=uint8(next()%2);
                    for(auto& t:tex){ for(auto& b:t)b=uint8(next()); }
                    for(size_t k=0;k<3;++k)At<void*>(tex[k].data(),0)=rasters.data()+0x10*k;
                    CEntity* const ents[]{nullptr,Veh(),Ent(),PedB()};
                    const int prio=int(next()%3); const void* ev=next()%5 ? event.data() : nullptr;
                    void* const o=cl.kind==PEDIK ? static_cast<void*>(ikObj.data()) : static_cast<void*>(object.data());
                    std::memset(object.data(),0xCD,object.size());
                    registerRefs=nm!="Constructor"; // el constructor sobrescribe el objeto con 0xCD
                    // fixture de la clase
                    switch(cl.kind){
                    case PEDIK: {
                        std::memset(ikObj.data(),0xCD,ikObj.size());
                        At<CPed*>(o,0)=PedA();
                        // 1 de cada 16: valores extremos (cero negativo, subnormales, +-1e30, +-FLT_MAX, +-inf y NaN); el mismo NaN en todos los campos para que x87 y SSE den los mismos bits
                        auto pick=[&](float lo,float hi){ static const float stress[]{-0.f,1e-40f,-1e-40f,1e30f,-1e30f,3.4028235e38f,-3.4028235e38f,std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()};
                            if(next()%16==0)return stress[next()%10];
                            const uint32 m=next()%8; return m==0 ? 0.f : m==1 ? uni(-0.01f,0.01f) : m==2 ? 0.01f : uni(lo,hi); };
                        At<float>(o,4)=uni(-2.f,2.f); At<float>(o,8)=uni(-2.f,2.f);
                        At<float>(o,0xC)=pick(-1.5f,1.5f); At<float>(o,0x10)=uni(-1.f,2.f); At<float>(o,0x14)=pick(-1.5f,1.5f); At<float>(o,0x18)=pick(-1.f,1.f);
                        At<uint32>(o,0x1C)=next();
                        // pose de la extremidad y limites (a veces con min > max)
                        At<float>(limb.data(),0)=uni(-2.f,2.f); At<float>(limb.data(),4)=uni(-2.f,2.f);
                        At<float>(info.data(),0)=uni(0.f,2.f); At<float>(info.data(),4)=next()%6 ? uni(-2.f,0.f) : uni(0.f,3.f); At<float>(info.data(),8)=next()%5 ? uni(0.f,0.3f) : 0.f;
                        At<float>(info.data(),0xC)=uni(0.f,2.f); At<float>(info.data(),0x10)=next()%6 ? uni(-2.f,0.f) : uni(0.f,3.f); At<float>(info.data(),0x14)=next()%5 ? uni(0.f,0.3f) : 0.f;
                        At<int32>(PedA(),P_WEAPONS+0x1C*wslot)=aimWeapons[next()%aimWeapons.size()];
                        break; }
                    case WFX: {
                        for(size_t c=0;c<2;++c){ const uintptr_t b=ADDR_CROSS+0x2C*c;
                            for(size_t k=0;k<0x2C;++k)AtA<uint8>(b+k)=uint8(next());
                            { static const uint8 act[]{0,1,1,1,2}; AtA<uint8>(b)=act[next()%5]; }
                            { const uint32 m=next()%6; AtA<int32>(b+4)=m==0 ? 0 : m==1||m==2 ? -1 : m==3 ? int32(now-500) : m==4 ? int32(now+500) : int32(now); }
                            AtA<float>(b+8)=uni(-30.f,30.f); AtA<float>(b+0xC)=uni(-30.f,30.f); AtA<float>(b+0x10)=uni(0.f,40.f);
                            { static const float sizes[]{0.2f,0.3f,1.2f,1.5f,1.f}; AtA<float>(b+0x18)=next()%2 ? sizes[next()%5] : uni(0.f,2.f); }
                            AtA<float>(b+0x1C)=uni(0.f,7.f); AtA<float>(b+0x20)=next()%4 ? uni(0.f,40.f) : 0.f;
                            { static const float rots[]{0.f,1.f,0.5f,2.5f,-1.f,256.f}; AtA<float>(b+0x24)=rots[next()%6]; }
                            { static const uint8 clr[]{0,1,1,2}; AtA<uint8>(b+0x28)=clr[next()%4]; } }
                        AtA<float>(0xC8A8A0)=next()%5 ? uni(0.f,30.f) : uni(-3.f,0.f); AtA<float>(0xC8A8A4)=next()%5 ? uni(0.f,30.f) : uni(-3.f,0.f);
                        AtA<uint8>(0x8D6144)=uint8(next()%2); AtA<uint8>(0x8D6145)=uint8(next()%2);
                        AtA<float>(0xC8A898)=uni(-5.f,5.f); AtA<float>(0xC8A89C)=uni(-5.f,5.f);
                        AtA<CEntity*>(0xC8A894)=next()%2 ? Ent() : nullptr; AtA<uint32>(0xC8A890)=now-next()%3000;
                        for(size_t k=0;k<3;++k)AtA<void*>(ADDR_TEX+4*k)=tex[k].data();
                        AtA<uint8>(ADDR_WIDESCREEN)=uint8(next()%16==0);
                        RandomMatrix(reinterpret_cast<void*>(ADDR_CAMMAT),uni(-3.f,3.f),uni(-0.6f,0.6f),0.f,CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(0.f,50.f)});
                        const bool twoPlayer=next()%3==0;
                        AtA<int16>(camMode)=twoPlayer ? int16(49) : int16(next()%3 ? 4 : 53);
                        AtA<float>(camTarget)=uni(-1.f,1.f); AtA<float>(camTarget+4)=uni(-1.f,1.f);
                        AtA<CPed*>(ADDR_PLAYERS)=(twoPlayer||next()%6) ? PedA() : nullptr;
                        AtA<CPed*>(ADDR_PLAYERS+0x190)=next()%3 ? PedB() : nullptr;
                        At<CEntity*>(PedA(),P_VEHICLE)=next()%2 ? Veh() : nullptr; At<CEntity*>(Veh(),V_DRIVER)=next()%2 ? static_cast<CEntity*>(PedA()) : PedB();
                        At<int32>(PedA(),P_WEAPONS+0x1C*wslot)=aimWeapons[next()%aimWeapons.size()];
                        At<int32>(PedB(),P_WEAPONS+0x1C*wslot)=aimWeapons[next()%aimWeapons.size()];
                        AtA<uintptr_t>(rsSlot)=bit_cast<uintptr_t>(&RSStub);
                        break; }
                    case IKCHAIN:
                        for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                        At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                        At<int32>(o,8)=next()%3 ? int32(next()%3000) : -1; At<int32>(o,0xC)=next()%5 ? int32(next()%1000) : 0;
                        At<void*>(o,0x10)=next()%4 ? static_cast<void*>(chain.data()) : nullptr;
                        for(size_t off:{size_t(0x1C),size_t(0x20),size_t(0x24),size_t(0x30),size_t(0x34),size_t(0x38),size_t(0x3C)})At<float>(o,off)=uni(-2.f,2.f);
                        RegisterAt(o,0x28,ents[next()%4]); At<int32>(o,0x2C)=next()%3 ? int32(tags[next()%NNODE]) : int32(next());
                        At<uint8>(o,0x40)=uint8(next()%2); At<float>(o,0x44)=uni(0.f,1.f);
                        At<int32>(o,0x48)=next()%5 ? int32(now+next()%6000-3000) : -1;
                        { const uint32 m=next()%3; At<float>(o,0x4C)=m==0 ? 0.f : m==1 ? 1.f : uni(0.f,1.f); }
                        At<int32>(o,0x50)=next()%7 ? int32(now+next()%6000-3000) : int32(now+20);
                        At<int32>(o,0x54)=int32(next()%2);
                        break;
                    case THROW: {
                        for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                        At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                        At<uint8>(o,8)=uint8(next()%4==0); At<uint8>(o,9)=uint8(next()%2); At<uint8>(o,0xA)=uint8(next()%2);
                        At<void*>(o,0xC)=(nm=="FinishAnimThrowProjectileCB"||next()%4) ? static_cast<void*>(assoc[0].data()) : nullptr;
                        RegisterAt(o,0x10,ents[next()%4]);
                        { const uint32 m=next()%3; const CVector p=m==0 ? CVector{0.f,0.f,0.f} : m==1 ? pedPos+CVector{uni(-6.f,6.f),uni(-6.f,6.f),uni(-3.f,3.f)} : CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,40.f)};
                          At<float>(o,0x14)=p.x; At<float>(o,0x18)=p.y; At<float>(o,0x1C)=p.z; }
                        if(next()%2)Ent()->SetPosn(pedPos+CVector{uni(-8.f,8.f),uni(-8.f,8.f),uni(-4.f,4.f)});
                        At<uint32>(o,0x20)=next()%2 ? now-next()%1500 : next();
                        { static const int32 weapons[]{16,17,18,39,22,16,39}; At<int32>(PedA(),P_WEAPONS+0x1C*wslot)=weapons[next()%7]; }
                        // animacion activa: ids de lanzar con el tiempo cerca del instante de disparo del arma
                        auto* a=assoc[0].data();
                        { static const int16 ids[]{0xE5,0xE6,0xE6,0xE5,0xE4,0x10}; At<int16>(a,0x2C)=ids[next()%6]; }
                        const auto* wi=CWeaponInfo::GetWeaponInfo(eWeaponType(At<int32>(PedA(),P_WEAPONS+0x1C*wslot)),eWeaponSkill::STD);
                        const float fire=At<int16>(a,0x2C)==0xE6 ? wi->m_fAnimLoop2Fire : wi->m_fAnimLoopFire;
                        At<float>(a,0x20)=next()%4 ? fire+uni(-0.1f,0.3f) : uni(0.f,2.f); At<float>(a,0x28)=next()%5 ? uni(0.f,0.3f) : 0.f;
                        At<uint16>(a,0x2E)=uint16(next());
                        break; }
                    case BIKE:
                        for(size_t k=8;k<cl.size;++k)At<uint8>(o,k)=uint8(next());
                        At<uintptr_t>(o,0)=cl.vt; At<void*>(o,4)=nullptr;
                        At<uint8>(o,8)=uint8(next()%4==0);
                        At<void*>(o,0xC)=(nm=="FinishAnimPickUpBikeCB"||next()%3) ? static_cast<void*>(assoc[0].data()) : nullptr;
                        // sin vehiculo solo donde el EXE no lo usa
                        RegisterAt(o,0x10,(nm=="MakeAbortable"||nm=="StartAnim"||next()%6) ? Veh() : nullptr);
                        { static const int32 doors[]{10,11,8,9,0,10,11,18}; At<int32>(o,0x14)=doors[next()%8]; }
                        At<void*>(o,0x18)=lineUp.data();
                        { static const int16 ids[]{351,352,353,354,351,10}; At<int16>(assoc[0].data(),0x2C)=ids[next()%6]; }
                        { static const float times[]{0.4667f,0.46669f,0.47f,0.f}; At<float>(assoc[0].data(),0x20)=next()%2 ? times[next()%4] : uni(0.f,1.f); }
                        break;
                    }
                    CPed* const pd=PedA();
                    // argumentos
                    CEntity* const argEnt=ents[next()%4]; const uint32 argA=next(), argB=next(), argC=next();
                    const float ax=uni(-2.f,2.f), ay=uni(-2.f,2.f), az=uni(-2.f,2.f), argF=uni(0.f,4.f);
                    const auto occ=Occupancy();
                    setState(candidate);
                    const uintptr_t target=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : fn.address;
                    bool switches=fn.slot>=0 ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    uintptr_t rawRet=0; json ret=nullptr;
                    const bool isCtor=nm=="Constructor", isDtor=nm=="Destructor";
                    bool isVoid=false;
                    switch(cl.kind){
                    case PEDIK: {
                        auto* const ik=o;
                        if(isCtor){ std::memset(ik,0xCD,ikObj.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*)>(target)(ik,argA%3 ? PedA() : PedB())); }
                        else if(nm=="GetWorldMatrix")rawRet=uintptr_t(reinterpret_cast<RwMatrix*(__cdecl*)(void*,void*)>(target)(frm[0].data(),outMat.data()));
                        else if(nm=="MoveLimb-non-normalized"||nm=="MoveLimb-normalized"){
                            const float yaw=At<float>(limb.data(),0), pitch=At<float>(limb.data(),4), yd=At<float>(info.data(),8), pdl=At<float>(info.data(),0x14);
                            auto near_=[&](float v,float d,uint32 m){ return m==0 ? v : m==1 ? v+d : m==2 ? v-d : m==3 ? v+d*0.5f : m==4 ? std::bit_cast<float>(0x7FC00000u) : uni(-2.5f,2.5f); };
                            const float ty=near_(yaw,yd,argA%9), tp=near_(pitch,pdl,argB%9);
                            if(nm=="MoveLimb-non-normalized")ret=reinterpret_cast<int32(__thiscall*)(void*,void*,float,float,void*)>(target)(ik,limb.data(),ty,tp,info.data());
                            else { static const float norms[]{0.9f,0.95f,1.f,0.5f,-0.5f,0.f}; const float n=argC%2 ? norms[argC%6] : uni(-0.5f,2.f);
                                ret=reinterpret_cast<int32(__thiscall*)(void*,void*,float,float,void*,float)>(target)(ik,limb.data(),ty,tp,info.data(),n); } }
                        else if(nm=="RotateTorso"){ isVoid=true; reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32)>(target)(ik,frames.data()+0x18*(1+argA%NNODE),limb.data(),argB); }
                        else if(nm=="RotateTorsoForArm"){ isVoid=true;
                            const CVector d=argA%3 ? pedPos+CVector{ax*10.f,ay*10.f,az} : CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,40.f)};
                            At<CVector>(vec.data(),0)=d; reinterpret_cast<void(__thiscall*)(void*,void*)>(target)(ik,vec.data()); }
                        else if(nm=="PointGunInDirection"){
                            static const float norms[]{-1.f,0.5f,1.f,0.95f,0.f,-0.f};
                            const float n=argC%3 ? norms[argC%6] : (argC%7==0 ? std::bit_cast<float>(0x7FC00000u) : uni(-1.f,2.f));
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,float,float,uint32,float)>(target)(ik,uni(-7.f,7.f),uni(-2.f,2.f),argB,n)!=0; }
                        else if(nm=="PointGunAtPosition"){ isVoid=true;
                            const CVector d=argA%2 ? pedPos+CVector{ax*10.f,ay*10.f,az*3.f} : CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,40.f)};
                            At<CVector>(vec.data(),0)=d; static const float norms[]{-1.f,0.5f,1.f,0.95f};
                            reinterpret_cast<void(__thiscall*)(void*,void*,float)>(target)(ik,vec.data(),argC%2 ? norms[argC%4] : uni(-1.f,2.f)); }
                        else if(nm=="PitchForSlope"){ isVoid=true; reinterpret_cast<void(__thiscall*)(void*)>(target)(ik); }
                        break; }
                    case WFX: {
                        const int32 id=int32(argA%2);
                        if(nm=="Init"||nm=="Shutdown"||nm=="ClearCrossHairs"||nm=="ClearCrossHairsImmediately"||nm=="Render"){ isVoid=true; reinterpret_cast<void(__cdecl*)()>(target)(); }
                        else if(nm=="IsLockedOn")ret=(reinterpret_cast<uint32(__cdecl*)(int32)>(target)(id)&0xFF)!=0; // la DLL normaliza el bool
                        else if(nm=="MarkTarget"){ isVoid=true; reinterpret_cast<void(__cdecl*)(int32,float,float,float,uint32,uint32,uint32,uint32,float,uint32)>(target)(id,ax*10.f,ay*10.f,az*10.f,argB,argB>>8|argC<<24,argC,argC>>8,argF,argA>>4); }
                        else { isVoid=true; reinterpret_cast<void(__cdecl*)(int32)>(target)(id); }
                        break; }
                    case IKCHAIN:
                        if(isCtor){ std::memset(o,0xCD,object.size());
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const char*,int32,float,float,float,int32,CEntity*,int32,float,float,float,float,int32,int32)>(target)(
                                o,argA%2 ? purposeName : nullptr,tags[argA%NNODE],ax,ay,az,tags[argB%NNODE],argEnt,argC%3 ? tags[argC%NNODE] : int32(argC),ay,az,ax,argF,argB%3 ? int32(argB%3000) : -1,int32(argC%1000))); }
                        else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                        else if(nm=="ProcessPed"||nm=="CreateIKChain")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="BlendOut"){ isVoid=true; static const int32 times[]{250,0,-5,1000}; reinterpret_cast<void(__thiscall*)(void*,int32)>(target)(o,argA%2 ? times[argA%4] : int32(argB%2000)); }
                        else if(nm=="GetIKChain")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(target)(o));
                        break;
                    case THROW:
                        if(isCtor){ std::memset(o,0xCD,object.size()); rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,float,float,float)>(target)(o,argEnt,ax*10.f,ay*10.f,az*10.f)); }
                        else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                        else if(nm=="ProcessPed")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="StartAnim"){ isVoid=true; reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd); }
                        else if(nm=="ControlThrow"){ At<CVector>(vec.data(),0)=CVector{ax*10.f,ay*10.f,az*10.f};
                            ret=reinterpret_cast<uint8(__thiscall*)(void*,uint32,CEntity*,const void*)>(target)(o,argA,argEnt,argB%3 ? vec.data() : nullptr)!=0; }
                        else if(nm=="FinishAnimThrowProjectileCB"){ isVoid=true; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(assoc[1].data(),o); }
                        break;
                    case BIKE:
                        if(isCtor){ std::memset(o,0xCD,object.size()); static const int32 doors[]{10,11,8,9};
                            rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CEntity*,int32,void*)>(target)(o,argA%4 ? Veh() : nullptr,argB%5 ? doors[argB%4] : int32(argB),argC%3 ? lineUp.data() : nullptr)); }
                        else if(isDtor)reinterpret_cast<void*(__thiscall*)(void*)>(target)(o);
                        else if(nm=="Clone")rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(o));
                        else if(nm=="GetTaskType")ret=reinterpret_cast<int(__thiscall*)(void*)>(target)(o);
                        else if(nm=="MakeAbortable")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(o,pd,prio,ev);
                        else if(nm=="ProcessPed"||nm=="SetPedPosition")ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*)>(target)(o,pd);
                        else if(nm=="StartAnim"){ isVoid=true; reinterpret_cast<void(__thiscall*)(void*,CPed*)>(target)(o,pd); }
                        else if(nm=="FinishAnimPickUpBikeCB"){ isVoid=true; reinterpret_cast<void(__cdecl*)(void*,void*)>(target)(assoc[1].data(),o); }
                        break;
                    }
                    setState(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null()&&!isDtor&&!isVoid)ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh){ const int type=int(t->GetTaskType()); tasks.push_back({{"type",type},{"words",Region(t,0x80,true)}}); }
                    json objectWords=Region(o,cl.kind==PEDIK ? ikObj.size() : object.size(),(isCtor||isDtor)&&cl.kind!=PEDIK); // vptr EXE o DLL tras construir/destruir
                    if(isCtor&&cl.kind!=PEDIK)objectWords.push_back(int(reinterpret_cast<CTask*>(o)->GetTaskType())); // se compara el tipo
                    json refs=json::array(); auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    for(auto* e:{static_cast<CEntity*>(PedA()),static_cast<CEntity*>(PedB()),Ent(),Veh()})refs.push_back(refList(e));
                    json assocs=json::array(); for(auto& a:assoc)assocs.push_back(Region(a.data(),a.size()));
                    json frms=json::array(); for(auto& fr:frm)frms.push_back(Region(fr.data(),0x50));
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"refs",refs},{"pedA",EntHash(pedA)},{"pedB",EntHash(pedB)},{"ent",EntHash(ent)},
                            {"veh",EntHash(veh)},{"ped_fields",{At(PedA(),P_FLAGS1),At(PedA(),P_FLAGS2),At(PedA(),P_FLAGS3),At(PedA(),P_FLAGS4)}},
                            {"bike_flags",At<uint8>(Veh(),V_BIKEFLAGS)},{"assoc",assocs},{"frm",frms},{"outmat",[&]{ json w=Region(outMat.data(),outMat.size()); for(size_t k:{7u,11u,15u})w[k]=0; return w; }()}, /* pads: basura de pila de RwMatrixTransform */
                            {"tmpmat",Hash(tmpMat.data(),tmpMat.size())},{"limb",Region(limb.data(),8)},{"playerdata",Hash(playerData.data(),playerData.size())},
                            {"buffers",{Hash(chain.data(),chain.size()),Hash(lineUp.data(),lineUp.size()),Hash(quats.data(),quats.size()),Hash(frames.data(),frames.size()),
                                        Hash(matrices.data(),matrices.size()),Hash(intel.data(),intel.size()),Hash(clumpData.data(),clumpData.size()),Hash(clump.data(),clump.size())}},
                            {"event",Region(event.data(),event.size(),true)},{"time",AtA(ADDR_TIME_MS)},
                            {"wfx",{RawWords(ADDR_CROSS,0x58),RawWords(ADDR_RSTAT,0x18),RawWords(ADDR_TEX,0xC),AtA<uint8>(0x8D6144),AtA<uint8>(0x8D6145)}}};
                    // borrado de las tareas creadas (su traza va aparte)
                    const size_t traceBefore=trace.size();
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["clean"]=clean; r["switches"]=switches;
                    freshIds.clear();
                    CleanRefs();
                    restoreGlobals();
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
    const bool intactBefore=globalsIntact();
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(size_t k=0;k<2;++k){if(!matrixHooks[k])continue;matrixHooks[k]->State(matrixEnabled[k]);matrixHooks[k]->LockState(matrixLocked[k]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},
           {"state_restored",Occupancy()==occupancyStart&&freeRefs()==savedFree&&globalsIntact()}});
}
}
