#pragma once
// Banco dentro del motor: CPed, grupo A (armas e inventario, 31 funciones): AddWeaponModel, ClearWeapon(s),
// DoWeHaveWeaponAvailable, GiveDelayedWeapon, GiveWeapon, GiveWeaponAtStartOfFight, GiveWeaponWhenJoiningGang,
// GrantAmmo, RemoveWeaponAnims, RemoveWeaponForScriptedCutscene, RemoveWeaponModel, RemoveWeaponWhenEnteringVehicle,
// ReplaceWeaponForScriptedCutscene, ReplaceWeaponWhenExitingVehicle, RequestDelayedWeapon, SetAmmo, SetCurrentWeapon
// (ranura y tipo), GetWeaponSkill (actual y por tipo), SetWeaponSkill, GetWeaponSlot, DoGunFlash, ResetGunFlashAlpha,
// SetGunFlashAlpha, CanThrow/Drop/GetEntityThatThisPedIsHolding, GetHoldingTask y GiveObjectToPedToHold.
// Original (hooks del lote desactivados) frente a candidato con fixtures identicos sobre UN PEATON FALSO (buffer del
// tamano de CPed con patron aleatorio; los punteros que el EXE desreferencia apuntan a objetos falsos). Nunca peatones
// reales ni el jugador. Se simulan y trazan en ambos lados: CWeapon::Initialise/Shutdown/Reload, CPed::RemoveGogglesModel,
// CTaskManager::FindActiveTaskByType, CPedIntelligence::GetTaskHold, CTaskSimpleHoldEntity::DropEntity/CanThrowEntity,
// CWorld::Remove, CPlayerPed::GetPlayerInfoForThisPlayerPed, CStreaming::RequestModel, CStats::GetStatValue,
// CTxdStore::AddRef/RemoveRef, CClumpModelInfo::GetFrameFromName, FxManager_c::Create/DestroyFxSystem y FxSystem_c
// (SetLocalParticles/CopyParentMatrix/Play), GetAnimHierarchyFromSkinClump, RpHAnimIDGetIndex,
// RpHAnimHierarchyGetMatrixArray, RpClumpDestroy, GetFirstAtomic, RpSkinGeometryGetSkin, RpClumpForAllAtomics,
// GetFirstObject, CVehicle::SetComponentAtomicAlpha, RwMatrixRotate, RpAnimBlendClumpGetAssociation,
// CAnimManager::BlendAnimation y el RNG (rand del EXE 0x821B1E + CGeneral::GetRandomNumber). Las informaciones
// de arma (CWeaponInfo::aWeaponInfo[0..46]: ranura y modelos), los model info 344..349 (se cambian por falsos), el estado
// de carga de streaming de los modelos usados y los dos trucos CHEAT_NO_ONE_CAN_STOP_US/ROCKET_MAYHEM se guardan
// y se restauran en cada ejecucion y al final. IsPlayer esta inlineada en el EXE: se rellena m_nPedType real.
// GiveObjectToPedToHold solo se prueba por las ramas que terminan antes de crear el CObject/la tarea (la creacion usa el
// pool real de objetos y el constructor real de CObject): ver README.
#include <fstream>
#include <memory>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedIK.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "Stats.h"
#include "Streaming.h"
#include "TxdStore.h"
#include "Cheat.h"
#include "World.h"
#include "PlayerInfo.h"
#include "PedIntelligence.h"
#include "RwHelper.h"
#include "General.h"
#include "Fx/FxManager.h"
#include "Models/ClumpModelInfo.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/Object.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleHoldEntity.h"
#include "Tasks/TaskTypes/TaskComplexGoPickUpEntity.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "../ProbeRuntime.hpp"

namespace CPedALive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
// desplazamientos crudos del EXE (CPed / CWeapon / CPlayerPedData)
constexpr size_t P_SEED=0x20, P_FLAGS1=0x46C, P_INTEL=0x47C, P_PLAYERDATA=0x480, P_CREATEDBY=0x484, P_ANIMGROUP=0x4D4,
                 P_WEAPOBJ=0x4F4, P_GUNFLASH=0x4F8, P_MP1=0x504, P_PR1=0x506, P_MP2=0x508, P_PR2=0x50A, P_PEDTYPE=0x598,
                 P_WEAPONS=0x5A0, P_SAVED=0x70C, P_DELAYED=0x710, P_DELAYEDAMMO=0x714, P_ACTIVE=0x718, P_SKILL=0x72C, P_MODELID=0x740;
constexpr size_t W_TYPE=0, W_STATE=4, W_CLIP=8, W_TOTAL=0xC, W_NEXT=0x10, W_FLAG=0x14, W_DONT=0x15, W_FX=0x18, W_SIZE=0x1C;
constexpr uintptr_t ADDR_WINFO=0xC8AAB8, ADDR_MIPTRS=0xA9B0C8, ADDR_STREAM=0x8E4CC0, ADDR_FXMAN=0xA9AE80, ADDR_FRAMEPLUG=0x8D6090,
                    ADDR_CHEAT_AK=0x96917D, ADDR_CHEAT_ROCKET=0x96917E, EXE_SKINCB=0x734B90;
constexpr int NPOOL=6, NPOOL2=3, NTYPES=47;
constexpr int32 POOL[NPOOL]{344,345,346,347,348,349}, POOL2[NPOOL2]{1000,1001,1002};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,0x100> pedClump{}, pdata{}, geom{}, skin{}, goPick{}, hier{};
alignas(16) inline std::array<uint8,0x1000> intelHolder{};
alignas(16) inline std::array<uint8,0x400> pinfo{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> wclump{}, fx{}, atom{};
alignas(16) inline std::array<std::array<uint8,0x400>,2> wframe{};
alignas(16) inline std::array<std::array<uint8,0x80>,2> gframe{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> htask{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> assoc{};
alignas(16) inline std::array<std::array<uint8,0x100>,NPOOL> fmi{};
alignas(16) inline std::array<uint8,0xA00> matarr{};
alignas(16) inline std::array<uint8,0x200> heldEnt{};
inline std::array<void*,64> pedVtbl{}, entVtbl{}, miVtbl{};
inline CPed* Ped() { return reinterpret_cast<CPed*>(ped.data()); }
inline uint8* Intel() { return intelHolder.data(); }

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline void InitBufs() {
    bufs={{ped.data(),ped.size(),"ped"},{pedClump.data(),pedClump.size(),"pedclump"},{pdata.data(),pdata.size(),"pdata"},
          {geom.data(),geom.size(),"geom"},{skin.data(),skin.size(),"skin"},{goPick.data(),goPick.size(),"gopick"},{hier.data(),hier.size(),"hier"},
          {intelHolder.data(),intelHolder.size(),"intel"},{pinfo.data(),pinfo.size(),"pinfo"},{matarr.data(),matarr.size(),"matarr"},
          {heldEnt.data(),heldEnt.size(),"heldent"},{pedVtbl.data(),sizeof(pedVtbl),"pedvtbl"},{entVtbl.data(),sizeof(entVtbl),"entvtbl"},
          {miVtbl.data(),sizeof(miVtbl),"mivtbl"}};
    static const char* const wc[]{"wclump0","wclump1"}; static const char* const fxn[]{"fx0","fx1"}; static const char* const atn[]{"atom0","atom1"};
    static const char* const wf[]{"wframe0","wframe1"}; static const char* const gf[]{"gframe0","gframe1"};
    static const char* const ht[]{"htask0","htask1","htask2","htask3"}; static const char* const asn[]{"assoc0","assoc1","assoc2"};
    static const char* const fm[]{"fmi0","fmi1","fmi2","fmi3","fmi4","fmi5"};
    for(size_t k=0;k<2;++k){ bufs.push_back({wclump[k].data(),wclump[k].size(),wc[k]}); bufs.push_back({fx[k].data(),fx[k].size(),fxn[k]});
        bufs.push_back({atom[k].data(),atom[k].size(),atn[k]}); bufs.push_back({wframe[k].data(),wframe[k].size(),wf[k]}); bufs.push_back({gframe[k].data(),gframe[k].size(),gf[k]}); }
    for(size_t k=0;k<4;++k)bufs.push_back({htask[k].data(),htask[k].size(),ht[k]});
    for(size_t k=0;k<3;++k)bufs.push_back({assoc[k].data(),assoc[k].size(),asn[k]});
    for(size_t k=0;k<NPOOL;++k)bufs.push_back({fmi[k].data(),fmi[k].size(),fm[k]});
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==ADDR_FXMAN)return "fxman";
    if(v==EXE_SKINCB||v==std::bit_cast<uintptr_t>(&AtomicRemoveAnimFromSkinCB))return "AtomicRemoveAnimFromSkinCB";
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    return v;
}
inline json Region(const void* p,size_t n) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json Vec3(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }

// configuracion por caso de las dependencias simuladas
inline std::array<uintptr_t,4> found{}; inline uintptr_t holdRet{}, createRet{}, frameRet{}, atomicRet{}, skinRet{}, objRet{}, fxRet{};
inline bool dropClears{}, canThrowRet{}; inline int32 hierIdx{}; inline uint32 rngSt{};
inline std::array<uintptr_t,34> assocSeq{}; inline uint32 assocCalls{};
inline std::array<float,4> statVals{}; inline uint32 statCalls{};

// vtables falsas trazadas
template<int N> void* __fastcall PTrap(void*,void*) { trace.push_back({"ped_unexpected_slot",N}); return nullptr; }
template<int... N> void FillPTraps(std::integer_sequence<int,N...>) { ((pedVtbl[N]=reinterpret_cast<void*>(&PTrap<N>)),...); }
template<int N> void* __fastcall ETrap(void* e,void*) { trace.push_back({"entity_unexpected_slot",N,Name(uintptr_t(e))}); return nullptr; }
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((entVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }
inline void* __fastcall EDelete(void* e,void*,uint32 flags) { trace.push_back({"entity_delete",Name(uintptr_t(e)),flags&0xFFu}); return e; }
inline void __fastcall EDeleteRw(void* e,void*) { trace.push_back({"entity_delete_rw_object",Name(uintptr_t(e))}); }
template<int N> void* __fastcall MTrap(void* m,void*) { trace.push_back({"mi_unexpected_slot",N,Name(uintptr_t(m))}); return nullptr; }
template<int... N> void FillMTraps(std::integer_sequence<int,N...>) { ((miVtbl[N]=reinterpret_cast<void*>(&MTrap<N>)),...); }
inline void* __fastcall MCreate(void* m,void*) { trace.push_back({"mi_create_instance",Name(uintptr_t(m))}); return reinterpret_cast<void*>(createRet); }

// RNG
inline int __cdecl FakeRand() { trace.push_back({"rand"}); rngSt^=rngSt<<13; rngSt^=rngSt>>17; rngSt^=rngSt<<5; return int(rngSt>>9)&0x7FFF; }
inline uint16 __cdecl FakeRandomNumber() { return uint16(FakeRand()); }
// CWeapon (se imitan los campos de Initialise/Shutdown/Reload sin tocar model infos reales)
inline void __fastcall WInit(void* w,void*,uint32 type,int32 ammo,void* owner) {
    trace.push_back({"weapon_init",Name(uintptr_t(w)),type,ammo,Name(uintptr_t(owner))});
    At(w,W_TYPE)=type; At(w,W_STATE)=0; At(w,W_CLIP)=0; At<int32>(w,W_TOTAL)=ammo<99999 ? ammo : 99999;
    if(At(w,W_TOTAL))At(w,W_CLIP)=At(w,W_TOTAL)<17u ? At(w,W_TOTAL) : 17u;
    At(w,W_NEXT)=0; At<uint32>(w,W_FX)=0; At<uint8>(w,W_DONT)=0;
}
inline void __fastcall WShutdown(void* w,void*) {
    trace.push_back({"weapon_shutdown",Name(uintptr_t(w)),At(w,W_TYPE)});
    At(w,W_TYPE)=0; At(w,W_STATE)=0; At(w,W_TOTAL)=0; At(w,W_CLIP)=0; At(w,W_NEXT)=0;
}
inline void __fastcall WReload(void* w,void*,void* owner) {
    trace.push_back({"weapon_reload",Name(uintptr_t(w)),Name(uintptr_t(owner))});
    if(At(w,W_TOTAL))At(w,W_CLIP)=At(w,W_TOTAL)<17u ? At(w,W_TOTAL) : 17u;
}
inline void __fastcall RemGoggles(void* p,void*) { trace.push_back({"remove_goggles",Name(uintptr_t(p))}); }
// tareas e inteligencia
inline void* __fastcall FindActive(void* m,void*,int32 type) {
    trace.push_back({"find_active_task",Name(uintptr_t(m)),type});
    const int k=type-0x133; return k>=0&&k<4 ? reinterpret_cast<void*>(found[k]) : nullptr;
}
inline void* __fastcall GetHold(void* i,void*,bool ignore) { trace.push_back({"get_task_hold",Name(uintptr_t(i)),int(ignore)}); return reinterpret_cast<void*>(holdRet); }
inline void __fastcall DropStub(void* t,void*,void* p,bool snd) {
    trace.push_back({"hold_drop_entity",Name(uintptr_t(t)),Name(uintptr_t(p)),int(snd)}); if(dropClears)At<uint32>(t,8)=0;
}
inline bool __fastcall CanThrowStub(void* t,void*) { trace.push_back({"hold_can_throw",Name(uintptr_t(t))}); return canThrowRet; }
inline void __cdecl WorldRemove(CEntity* e) { trace.push_back({"world_remove",Name(uintptr_t(e))}); }
inline void* __fastcall PlayerInfoStub(void* p,void*) { trace.push_back({"get_player_info",Name(uintptr_t(p))}); return pinfo.data(); }
// streaming, estadisticas, TXD
inline void __cdecl RequestModelStub(int32 m,int32 flags) { trace.push_back({"request_model",m,flags}); }
inline float __cdecl StatStub(eStats s) { trace.push_back({"stat",int(s)}); return statVals[statCalls++%4]; }
inline void __cdecl TxdAdd(int32 i) { trace.push_back({"txd_add_ref",i}); }
inline void __cdecl TxdRem(int32 i) { trace.push_back({"txd_remove_ref",i}); }
// modelo del arma, FX, RW
inline RwFrame* __cdecl FrameFromName(RpClump* c,const char* n) { trace.push_back({"frame_from_name",Name(uintptr_t(c)),std::string(n)}); return reinterpret_cast<RwFrame*>(frameRet); }
inline void* __fastcall FxCreate(void* m,void*,const char* name,const CVector* pt,RwMatrix* mat,bool ign) {
    trace.push_back({"fx_create",Name(uintptr_t(m)),std::string(name),Vec3(pt),Name(uintptr_t(mat)),int(ign)}); return reinterpret_cast<void*>(fxRet);
}
inline void __fastcall FxDestroy(void* m,void*,void* s) { trace.push_back({"fx_destroy",Name(uintptr_t(m)),Name(uintptr_t(s))}); }
inline void __fastcall FxLocal(void* s,void*,bool b) { trace.push_back({"fx_set_local_particles",Name(uintptr_t(s)),int(b)}); }
inline void __fastcall FxCopy(void* s,void*) { trace.push_back({"fx_copy_parent_matrix",Name(uintptr_t(s))}); }
inline void __fastcall FxPlay(void* s,void*) { trace.push_back({"fx_play",Name(uintptr_t(s))}); }
inline RpHAnimHierarchy* __cdecl HierStub(RpClump* c) { trace.push_back({"anim_hierarchy_from_skin_clump",Name(uintptr_t(c))}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline RwInt32 __cdecl HAnimIdx(RpHAnimHierarchy* h,RwInt32 id) { trace.push_back({"hanim_id_get_index",Name(uintptr_t(h)),id}); return hierIdx; }
inline RwMatrix* __cdecl HAnimMats(RpHAnimHierarchy* h) { trace.push_back({"hanim_matrix_array",Name(uintptr_t(h))}); return reinterpret_cast<RwMatrix*>(matarr.data()); }
inline RwBool __cdecl ClumpDestroy(RpClump* c) { trace.push_back({"clump_destroy",Name(uintptr_t(c))}); return 1; }
inline RpAtomic* __cdecl FirstAtomic(RpClump* c) { trace.push_back({"first_atomic",Name(uintptr_t(c))}); return reinterpret_cast<RpAtomic*>(atomicRet); }
inline RpSkin* __cdecl SkinOf(RpGeometry* g) { trace.push_back({"skin_geometry_get_skin",Name(uintptr_t(g))}); return reinterpret_cast<RpSkin*>(skinRet); }
inline RpClump* __cdecl ForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* d) { trace.push_back({"clump_for_all_atomics",Name(uintptr_t(c)),Name(uintptr_t(cb)),Name(uintptr_t(d))}); return c; }
inline RwObject* __cdecl FirstObject(RwFrame* f) { trace.push_back({"first_object",Name(uintptr_t(f))}); return reinterpret_cast<RwObject*>(objRet); }
inline void __cdecl AtomAlpha(RpAtomic* a,int32 alpha) { trace.push_back({"atomic_alpha",Name(uintptr_t(a)),alpha}); }
inline RwMatrix* __cdecl MatRot(RwMatrix* m,const RwV3d* axis,RwReal ang,RwOpCombineType op) {
    trace.push_back({"matrix_rotate",Name(uintptr_t(m)),F(axis->x),F(axis->y),F(axis->z),F(ang),int(op)}); return m;
}
inline CAnimBlendAssociation* __cdecl AssocStub(RpClump* c,uint32 id) {
    trace.push_back({"clump_get_association",Name(uintptr_t(c)),id}); return reinterpret_cast<CAnimBlendAssociation*>(assocSeq[assocCalls++%34]);
}
inline CAnimBlendAssociation* __cdecl Blend(RpClump* c,int32 group,int32 id,float delta) { trace.push_back({"blend_animation",Name(uintptr_t(c)),group,id,F(delta)}); return nullptr; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=31;
inline const std::array<Fn,NFNS> fns{{
    {"CPed","GetWeaponSlot",0x5DF200,0,-1,256},{"CPed","GrantAmmo",0x5DF220,0,-1,512},{"CPed","SetAmmo",0x5DF290,0,-1,512},
    {"CPed","DoWeHaveWeaponAvailable",0x5DF300,0,-1,256},{"CPed","DoGunFlash",0x5DF340,0,-1,256},{"CPed","SetGunFlashAlpha",0x5DF400,0,-1,512},
    {"CPed","ResetGunFlashAlpha",0x5DF4E0,0,-1,128},{"CPed","GetHoldingTask",0x5E0290,0,-1,256},{"CPed","GetEntityThatThisPedIsHolding",0x5E02E0,0,-1,256},
    {"CPed","DropEntityThatThisPedIsHolding",0x5E0360,0,-1,512},{"CPed","CanThrowEntityThatThisPedIsHolding",0x5E0400,0,-1,256},
    {"CPed","GiveObjectToPedToHold",0x5E4390,0,-1,256},{"CPed","RemoveWeaponModel",0x5E3990,0,-1,512},{"CPed","AddWeaponModel",0x5E5ED0,0,-1,768},
    {"CPed","GetWeaponSkill-Current",0x5E6580,0,-1,256},{"CPed","GetWeaponSkill-WeaponType",0x5E3B60,0,-1,768},{"CPed","SetWeaponSkill",0x5E3C10,0,-1,128},
    {"CPed","GiveWeapon-",0x5E6080,0,-1,1024},{"CPed","SetCurrentWeapon-Slot",0x5E61F0,0,-1,512},{"CPed","SetCurrentWeapon-WepType",0x5E6280,0,-1,512},
    {"CPed","ClearWeapon",0x5E62B0,0,-1,512},{"CPed","ClearWeapons",0x5E6320,0,-1,256},{"CPed","RemoveWeaponWhenEnteringVehicle",0x5E6370,0,-1,768},
    {"CPed","ReplaceWeaponWhenExitingVehicle",0x5E6490,0,-1,512},{"CPed","ReplaceWeaponForScriptedCutscene",0x5E6530,0,-1,256},
    {"CPed","RemoveWeaponForScriptedCutscene",0x5E6550,0,-1,256},{"CPed","RequestDelayedWeapon",0x5E8910,0,-1,512},{"CPed","GiveDelayedWeapon",0x5E89B0,0,-1,768},
    {"CPed","GiveWeaponAtStartOfFight",0x5E8AB0,0,-1,768},{"CPed","GiveWeaponWhenJoiningGang",0x5E8BE0,0,-1,512},{"CPed","RemoveWeaponAnims",0x5F0250,0,-1,512},
}};

// comprobaciones de diseno: la DLL y el EXE deben coincidir en las posiciones que usa el banco (si no, el banco no vale)
inline std::string LayoutErrors() {
    std::string bad;
    auto chk=[&](const char* nm,uintptr_t got,size_t want){ if(got!=want)bad+=std::string(nm)+"="+std::to_string(got)+" (EXE "+std::to_string(want)+"); "; };
    auto* p=Ped(); const uintptr_t b=uintptr_t(p);
    chk("CPed size",sizeof(CPed),0x79C); chk("m_nRandomSeed",uintptr_t(&p->m_nRandomSeed)-b,P_SEED);
    chk("m_nAnimGroup",uintptr_t(&p->m_nAnimGroup)-b,P_ANIMGROUP); chk("m_pWeaponObject",uintptr_t(&p->m_pWeaponObject)-b,P_WEAPOBJ);
    chk("m_pGunflashObject",uintptr_t(&p->m_pGunflashObject)-b,P_GUNFLASH); chk("m_nWeaponGunflashAlphaMP1",uintptr_t(&p->m_nWeaponGunflashAlphaMP1)-b,P_MP1);
    chk("m_nWeaponGunFlashAlphaProgMP1",uintptr_t(&p->m_nWeaponGunFlashAlphaProgMP1)-b,P_PR1); chk("m_nWeaponGunflashAlphaMP2",uintptr_t(&p->m_nWeaponGunflashAlphaMP2)-b,P_MP2);
    chk("m_nWeaponGunFlashAlphaProgMP2",uintptr_t(&p->m_nWeaponGunFlashAlphaProgMP2)-b,P_PR2); chk("m_nPedType",uintptr_t(&p->m_nPedType)-b,P_PEDTYPE);
    chk("m_aWeapons",uintptr_t(p->m_aWeapons.data())-b,P_WEAPONS); chk("m_nSavedWeapon",uintptr_t(&p->m_nSavedWeapon)-b,P_SAVED);
    chk("m_nDelayedWeapon",uintptr_t(&p->m_nDelayedWeapon)-b,P_DELAYED); chk("m_nDelayedWeaponAmmo",uintptr_t(&p->m_nDelayedWeaponAmmo)-b,P_DELAYEDAMMO);
    chk("m_nActiveWeaponSlot",uintptr_t(&p->m_nActiveWeaponSlot)-b,P_ACTIVE); chk("m_nWeaponSkill",uintptr_t(&p->m_nWeaponSkill)-b,P_SKILL);
    chk("m_nWeaponModelId",uintptr_t(&p->m_nWeaponModelId)-b,P_MODELID); chk("sizeof(CWeapon)",sizeof(CWeapon),W_SIZE);
    auto* w=p->m_aWeapons.data(); const uintptr_t wb=uintptr_t(w);
    chk("CWeapon::m_Type",uintptr_t(&w->m_Type)-wb,W_TYPE); chk("CWeapon::m_State",uintptr_t(&w->m_State)-wb,W_STATE); chk("CWeapon::m_AmmoInClip",uintptr_t(&w->m_AmmoInClip)-wb,W_CLIP);
    chk("CWeapon::m_TotalAmmo",uintptr_t(&w->m_TotalAmmo)-wb,W_TOTAL); chk("CWeapon::m_TimeForNextShotMs",uintptr_t(&w->m_TimeForNextShotMs)-wb,W_NEXT);
    chk("CWeapon::m_DontPlaceInHand",uintptr_t(&w->m_DontPlaceInHand)-wb,W_DONT); chk("CWeapon::m_FxSystem",uintptr_t(&w->m_FxSystem)-wb,W_FX);
    auto* pd=reinterpret_cast<CPlayerPedData*>(pdata.data());
    chk("m_nChosenWeapon",uintptr_t(&pd->m_nChosenWeapon)-uintptr_t(pd),0x20); chk("m_nPlayerFlags",uintptr_t(&pd->m_nPlayerFlags)-uintptr_t(pd),0x34);
    auto* pi=reinterpret_cast<CPlayerInfo*>(pinfo.data()); chk("m_bCanDoDriveBy",uintptr_t(&pi->m_bCanDoDriveBy)-uintptr_t(pi),0x153);
    auto* in=reinterpret_cast<CPedIntelligence*>(Intel()); chk("m_TaskMgr",uintptr_t(&in->m_TaskMgr)-uintptr_t(in),4);
    auto* ht=reinterpret_cast<CTaskSimpleHoldEntity*>(htask[0].data());
    chk("m_pEntityToHold",uintptr_t(&ht->m_pEntityToHold)-uintptr_t(ht),8); chk("m_nBoneFrameId",uintptr_t(&ht->m_nBoneFrameId)-uintptr_t(ht),0x18);
    auto* gp=reinterpret_cast<CTaskComplexGoPickUpEntity*>(goPick.data()); chk("CTaskComplexGoPickUpEntity::m_pEntity",uintptr_t(&gp->m_pEntity)-uintptr_t(gp),0xC);
    auto* ob=reinterpret_cast<CObject*>(heldEnt.data()); chk("m_nObjectType",uintptr_t(&ob->m_nObjectType)-uintptr_t(ob),0x13C);
    chk("TASK_SIMPLE_HOLD_ENTITY",size_t(TASK_SIMPLE_HOLD_ENTITY),0x133); chk("TASK_SIMPLE_PICKUP_ENTITY",size_t(TASK_SIMPLE_PICKUP_ENTITY),0x134);
    chk("TASK_SIMPLE_PUTDOWN_ENTITY",size_t(TASK_SIMPLE_PUTDOWN_ENTITY),0x135); chk("TASK_COMPLEX_GO_PICKUP_ENTITY",size_t(TASK_COMPLEX_GO_PICKUP_ENTITY),0x136);
    chk("ENTITY_TYPE_OBJECT",size_t(ENTITY_TYPE_OBJECT),4); chk("PED_NODE_RIGHT_HAND",size_t(PED_NODE_RIGHT_HAND),6);
    chk("cheat NO_ONE_CAN_STOP_US",uintptr_t(&CCheat::m_aCheatsActive[CHEAT_NO_ONE_CAN_STOP_US]),ADDR_CHEAT_AK);
    chk("cheat ROCKET_MAYHEM",uintptr_t(&CCheat::m_aCheatsActive[CHEAT_ROCKET_MAYHEM]),ADDR_CHEAT_ROCKET);
    chk("aWeaponInfo",uintptr_t(CWeaponInfo::GetWeaponInfo(WEAPON_PISTOL,eWeaponSkill::STD)),ADDR_WINFO+0x70*22); chk("ms_modelInfoPtrs",uintptr_t(&CModelInfo::ms_modelInfoPtrs[0]),ADDR_MIPTRS);
    chk("ms_aInfoForModel",uintptr_t(&CStreaming::ms_aInfoForModel[0]),ADDR_STREAM); chk("m_LoadState",uintptr_t(&CStreaming::ms_aInfoForModel[0].m_LoadState)-ADDR_STREAM,0x10);
    chk("sizeof(CStreamingInfo)",sizeof(CStreamingInfo),20);
    chk("MODEL_MOLOTOV",size_t(MODEL_MOLOTOV),344);
    // el campo de tipo de entidad (3 bits en 0x36) se comprueba por comportamiento
    std::memset(heldEnt.data(),0,heldEnt.size()); At<uint8>(heldEnt.data(),0x36)=4;
    if(!reinterpret_cast<CEntity*>(heldEnt.data())->GetIsTypeObject())bad+="CEntity type byte 0x36; ";
    At<uint8>(heldEnt.data(),0x36)=0; if(reinterpret_cast<CEntity*>(heldEnt.data())->GetIsTypeObject())bad+="CEntity type byte 0x36 (0); ";
    return bad;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpeda-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpeda-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpeda");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cpeda-only.txt"); if(f)std::getline(f,only); }
    InitBufs();
    { const auto bad=LayoutErrors(); if(!bad.empty()){Write({{"event","error"},{"reason","layout mismatch DLL/EXE"},{"details",bad}});return;} }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uint32 plugOff=AtA(ADDR_FRAMEPLUG);
    if(plugOff+4>wframe[0].size()){Write({{"event","error"},{"reason","frame plugin offset out of fake frame"},{"offset",plugOff}});return;}
    { // el desplazamiento del plugin de frame (0x8D6090) es el que usa CVisibilityPlugins::GetClumpModelInfo en la DLL
      std::memset(wclump[0].data(),0,wclump[0].size()); std::memset(wframe[0].data(),0,wframe[0].size()); wclump[0][0]=2; At<void*>(wclump[0].data(),4)=wframe[0].data();
      At<uint32>(wframe[0].data(),plugOff)=0x12345678u;
      if(uintptr_t(CVisibilityPlugins::GetClumpModelInfo(reinterpret_cast<RpClump*>(wclump[0].data())))!=0x12345678u){Write({{"event","error"},{"reason","GetClumpModelInfo does not read the frame plugin offset 0x8D6090"}});return;} }
    FillPTraps(std::make_integer_sequence<int,64>{}); FillETraps(std::make_integer_sequence<int,64>{}); FillMTraps(std::make_integer_sequence<int,64>{});
    entVtbl[0]=reinterpret_cast<void*>(&EDelete); entVtbl[8]=reinterpret_cast<void*>(&EDeleteRw); miVtbl[11]=reinterpret_cast<void*>(&MCreate);
    // estado global tocado: se guarda aqui y se restaura antes de cada ejecucion y al final
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> saved;
    auto snap=[&](uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); };
    snap(ADDR_WINFO,0x70*NTYPES); snap(ADDR_CHEAT_AK,1); snap(ADDR_CHEAT_ROCKET,1);
    for(int k=0;k<NPOOL;++k){ snap(ADDR_MIPTRS+4*POOL[k],4); snap(ADDR_STREAM+20*POOL[k],20); }
    for(int k=0;k<NPOOL2;++k)snap(ADDR_STREAM+20*POOL2[k],20);
    auto restoreGlobals=[&]{ for(auto& [a,b]:saved)std::memcpy(reinterpret_cast<void*>(a),b.data(),b.size()); };
    auto globalsIntact=[&]{ for(auto& [a,b]:saved)if(std::memcmp(reinterpret_cast<void*>(a),b.data(),b.size()))return false; return true; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); };
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto patch=[&](auto dll,uintptr_t exeAddr,auto stub){
            patches.push_back(std::make_unique<EntryPatch>(bit_cast<uintptr_t>(dll),stub)); patches.push_back(std::make_unique<EntryPatch>(exeAddr,stub));
        };
        patch(&CGeneral::GetRandomNumber,0x821B1E,FakeRandomNumber);
        patch(&CWeapon::Initialise,0x73B4A0,WInit); patch(&CWeapon::Shutdown,0x73A380,WShutdown); patch(&CWeapon::Reload,0x73AEB0,WReload);
        patch(&CPed::RemoveGogglesModel,0x5DF170,RemGoggles);
        patch(&CTaskManager::FindActiveTaskByType,0x681740,FindActive); patch(&CPedIntelligence::GetTaskHold,0x600FF0,GetHold);
        patch(&CTaskSimpleHoldEntity::DropEntity,0x6930F0,DropStub); patch(&CTaskSimpleHoldEntity::CanThrowEntity,0x691700,CanThrowStub);
        patch(&CWorld::Remove,0x563280,WorldRemove); patch(&CPlayerPed::GetPlayerInfoForThisPlayerPed,0x609FF0,PlayerInfoStub);
        patch(&CStreaming::RequestModel,0x4087E0,RequestModelStub);
        patch(static_cast<float(*)(eStats)>(&CStats::GetStatValue),0x558E40,StatStub);
        patch(&CTxdStore::AddRef,0x731A00,TxdAdd); patch(&CTxdStore::RemoveRef,0x731A30,TxdRem);
        patch(&CClumpModelInfo::GetFrameFromName,0x4C5400,FrameFromName);
        patch(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem),0x4A9BE0,FxCreate);
        patch(&FxManager_c::DestroyFxSystem,0x4A9810,FxDestroy); patch(&FxSystem_c::SetLocalParticles,0x4AA910,FxLocal);
        patch(&FxSystem_c::CopyParentMatrix,0x4AA890,FxCopy); patch(&FxSystem_c::Play,0x4AA2F0,FxPlay);
        patch(&GetAnimHierarchyFromSkinClump,0x734A40,HierStub); patch(&RpHAnimIDGetIndex,0x7C51A0,HAnimIdx);
        patch(&RpHAnimHierarchyGetMatrixArray,0x7C5120,HAnimMats); patch(&RpClumpDestroy,0x74A310,ClumpDestroy);
        patch(&GetFirstAtomic,0x734820,FirstAtomic); patch(&RpSkinGeometryGetSkin,0x7C7550,SkinOf);
        patch(&RpClumpForAllAtomics,0x749B70,ForAllAtomics); patch(&GetFirstObject,0x7348C0,FirstObject);
        patch(&CVehicle::SetComponentAtomicAlpha,0x6D2960,AtomAlpha); patch(&RwMatrixRotate,0x7F1FD0,MatRot);
        patch(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation),0x4D68B0,AssocStub);
        patch(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation),0x4D4610,Blend);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                const uint32 seed0=[&]{ uint32 s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x9A9Au; return s ? s : 1u; }();
                auto run=[&](bool candidate)->json {
                    restoreGlobals();
                    uint32 st=seed0; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                    auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                    trace=json::array(); rngSt=seed0^0x5A5A1234u; if(!rngSt)rngSt=7; assocCalls=0; statCalls=0;
                    auto fillArr=[&](auto& a){ for(auto& b:a)b=uint8(next()); };
                    const bool negSlot=nm=="GetWeaponSlot"||nm=="GrantAmmo"||nm=="SetAmmo"||nm=="DoWeHaveWeaponAvailable"||nm=="ClearWeapon"||nm=="SetCurrentWeapon-WepType";
                    // ---- argumentos (se generan antes para que el mundo dependa de ellos) ----
                    const uint32 a=next(), b=next(), c=next();
                    static const uint32 ammos[]{0u,1u,2u,50u,99998u,99999u,100000u,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu,0xFFFFFFF0u,500u,17u};
                    const uint32 ammo=next()%4==0 ? next() : ammos[next()%13];
                    const uint32 wtype=next()%47;
                    const int32 modelArg=next()%7==0 ? -1 : POOL[next()%NPOOL];
                    const uint32 skillT=next()%3 ? 22+wtype%11 : wtype;
                    // ---- informaciones de arma (ranura y modelos) ----
                    for(int t=0;t<NTYPES;++t){ const uintptr_t wi=ADDR_WINFO+0x70*uintptr_t(t);
                        int32 slot=AtA<int32>(wi+0x14); if(slot<0||slot>12)slot=t%13;
                        if(negSlot&&next()%8==0)slot=-1;
                        AtA<int32>(wi+0x14)=slot;
                        AtA<int32>(wi+0xC)= t==18 ? 344 : (next()%3==0 ? -1 : POOL[next()%NPOOL]);
                        AtA<int32>(wi+0x10)= next()%3 ? -1 : POOL2[next()%NPOOL2]; }
                    auto slotOf=[&](uint32 t){ return AtA<int32>(ADDR_WINFO+0x70*uintptr_t(t)+0x14); };
                    for(int k=0;k<NPOOL;++k)AtA<uint8>(ADDR_STREAM+20*POOL[k]+0x10)= next()%3==0 ? uint8(next()%6) : uint8(1);
                    for(int k=0;k<NPOOL2;++k)AtA<uint8>(ADDR_STREAM+20*POOL2[k]+0x10)= next()%3==0 ? uint8(next()%6) : uint8(1);
                    AtA<uint8>(ADDR_CHEAT_AK)=uint8(next()%2); AtA<uint8>(ADDR_CHEAT_ROCKET)=uint8(next()%2);
                    // ---- model infos falsos (se cambian en la tabla real y se restauran) ----
                    for(int k=0;k<NPOOL;++k){ fillArr(fmi[k]); At<void*>(fmi[k].data(),0)=miVtbl.data(); At<uint16>(fmi[k].data(),8)=uint16(next()%20); At<int16>(fmi[k].data(),0xA)=int16(next()%100);
                        AtA<uintptr_t>(ADDR_MIPTRS+4*POOL[k])=uintptr_t(fmi[k].data()); }
                    // ---- clumps del arma, frames, atomos, FX ----
                    for(size_t k=0;k<2;++k){ fillArr(wclump[k]); wclump[k][0]=2; At<void*>(wclump[k].data(),4)=wframe[k].data();
                        fillArr(wframe[k]); { int j=int(next()%NPOOL); if(modelArg!=-1&&next()%2)j=modelArg-POOL[0]; At<void*>(wframe[k].data(),plugOff)=fmi[size_t(j)].data(); }
                        fillArr(gframe[k]); fillArr(fx[k]); fillArr(atom[k]); At<void*>(atom[k].data(),0x18)=geom.data(); }
                    fillArr(geom); fillArr(skin); fillArr(hier); fillArr(matarr); hierIdx=int32(next()%40);
                    fillArr(pedClump); pedClump[0]=2; fillArr(pdata); fillArr(pinfo); fillArr(intelHolder);
                    At<uint8>(pinfo.data(),0x153)= next()%3 ? uint8(next()%2) : uint8(next());
                    createRet= next()%6==0 ? 0 : uintptr_t(wclump[next()%2].data()); frameRet= next()%4==0 ? 0 : uintptr_t(gframe[next()%2].data());
                    atomicRet= next()%4==0 ? 0 : uintptr_t(atom[next()%2].data()); skinRet= next()%3==0 ? 0 : uintptr_t(skin.data());
                    objRet= next()%4==0 ? 0 : uintptr_t(atom[next()%2].data()); fxRet= next()%4==0 ? 0 : uintptr_t(fx[next()%2].data());
                    // ---- peaton falso: patron aleatorio y punteros hacia objetos falsos ----
                    fillArr(ped); *reinterpret_cast<void**>(ped.data())=pedVtbl.data();
                    At<void*>(ped.data(),0x18)=pedClump.data(); At<void*>(ped.data(),P_INTEL)=Intel(); At<void*>(ped.data(),P_PLAYERDATA)=next()%6==0 ? nullptr : pdata.data();
                    At<uint8>(ped.data(),P_CREATEDBY)= next()%3==0 ? uint8(2) : uint8(next()%4);
                    { static const uint32 pts[]{0,1,0,1,6,6,4,5,7,8,12,16,17,18,19,20,21,22,23,2,3,30}; At(ped.data(),P_PEDTYPE)= next()%5==0 ? next()%40 : pts[next()%22]; }
                    if(nm=="RemoveWeaponWhenEnteringVehicle"||nm=="ReplaceWeaponWhenExitingVehicle"||nm=="AddWeaponModel"||nm=="RemoveWeaponModel"||nm=="SetCurrentWeapon-Slot")
                        At(ped.data(),P_PEDTYPE)= next()%4 ? next()%2 : (next()%3 ? 6u : next()%40);
                    if(nm=="GiveWeaponAtStartOfFight")At(ped.data(),P_PEDTYPE)= next()%6==0 ? next()%40 : 7+next()%16;
                    if(nm=="GetWeaponSkill-Current"||nm=="GetWeaponSkill-WeaponType"||nm=="SetWeaponSkill")At(ped.data(),P_PEDTYPE)= next()%3==0 ? next()%2 : (next()%2 ? 6u : next()%40);
                    if(nm=="GiveDelayedWeapon")At(ped.data(),P_PEDTYPE)= next()%4==0 ? next()%2 : (next()%3 ? 4+next()%20 : 6u);
                    At<uint16>(ped.data(),P_SEED)= nm=="GiveWeaponAtStartOfFight" ? ( next()%2 ? uint16(next()) : uint16((next()%8)*1024u+ (next()%2 ? 199u+next()%3 : 398u+next()%4))) : uint16(next());
                    At(ped.data(),P_SAVED)= next()%2 ? 55u : next()%47; At(ped.data(),P_DELAYED)= next()%2 ? 55u : next()%47; At(ped.data(),P_DELAYEDAMMO)= next()%3 ? ammos[next()%13] : next();
                    At<uint8>(ped.data(),P_ACTIVE)=uint8(next()%13); At<uint8>(ped.data(),P_SKILL)= next()%8==0 ? uint8(next()) : uint8(next()%4);
                    At(ped.data(),P_MODELID)=next(); At(ped.data(),P_ANIMGROUP)=next()%80;
                    { static const int16 gf[]{-32768,-1,0,1,2,5,100,5000,9999,10000,20000,32767}; At<int16>(ped.data(),P_MP1)= next()%4 ? gf[next()%12] : int16(next()); At<int16>(ped.data(),P_MP2)= next()%4 ? gf[next()%12] : int16(next()); }
                    for(int s=0;s<13;++s){ auto* w=static_cast<uint8*>(ped.data())+P_WEAPONS+W_SIZE*size_t(s);
                        uint32 t=0; { const uint32 m=next()%8; if(m>=2){ t=next()%47; if(m<6)for(int tries=0;tries<20&&slotOf(t)!=s;++tries)t=next()%47; } }
                        At(w,W_TYPE)=t; At(w,W_STATE)= next()%2 ? 0u : next()%5; At(w,W_CLIP)= next()%2 ? next()%30 : next();
                        { static const uint32 tot[]{0u,1u,5u,17u,99999u,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu}; At(w,W_TOTAL)= next()%3 ? tot[next()%8] : next()%200000; }
                        At(w,W_NEXT)=next(); At<uint8>(w,W_FLAG)=uint8(next()); At<uint8>(w,W_DONT)= next()%6==0; At<void*>(w,W_FX)= next()%3==0 ? fx[next()%2].data() : nullptr; }
                    { const uint8 act=At<uint8>(ped.data(),P_ACTIVE);
                      auto* aw=static_cast<uint8*>(ped.data())+P_WEAPONS+W_SIZE*size_t(act);
                      if(nm=="AddWeaponModel"&&next()%3==0){ At(aw,W_TYPE)=18; if(next()%3)At<void*>(aw,W_FX)=nullptr; At<uint8>(aw,W_DONT)=0; }
                      if(nm=="GiveWeaponAtStartOfFight"||nm=="GiveWeaponWhenJoiningGang"){ if(next()%5<3)At(aw,W_TYPE)=0u; if(next()%4)At(ped.data(),P_DELAYED)=55u; }
                      if(nm=="GetWeaponSkill-Current")At(aw,W_TYPE)= next()%6==0 ? next()%47 : 22+next()%11;
                      if(nm=="RemoveWeaponWhenEnteringVehicle"){ if(next()%4)At(ped.data(),P_SAVED)=55u;
                          static const uint32 smg[]{28,32,29,0,28,29,25}; At(ped.data(),P_WEAPONS+W_SIZE*4+W_TYPE)=smg[next()%7];
                          if(next()%2)At(ped.data(),P_WEAPONS+W_SIZE*3+W_TYPE)=26u; if(next()%2)At(ped.data(),P_WEAPONS+W_SIZE*2+W_TYPE)=22u; } }
                    // objeto del arma y llama
                    if(next()%5<3){ const size_t k=next()%2; At<void*>(ped.data(),P_WEAPOBJ)=wclump[k].data(); } else At<void*>(ped.data(),P_WEAPOBJ)=nullptr;
                    At<void*>(ped.data(),P_GUNFLASH)= next()%4==0 ? nullptr : gframe[next()%2].data();
                    // ---- inteligencia, tareas de sujetar y entidad sujeta ----
                    for(auto& t:htask){ fillArr(t); At<uint8>(t.data(),0x18)= next()%4 ? uint8(6) : uint8(next()); At<uint8>(t.data(),0x19)= next()%2 ? uint8(0x10) : uint8(next()); }
                    fillArr(goPick); fillArr(heldEnt); *reinterpret_cast<void**>(heldEnt.data())=entVtbl.data();
                    At<uint8>(heldEnt.data(),0x36)= next()%2 ? uint8((next()&0xF8)|4) : uint8(next());
                    { static const uint8 ot[]{0,1,2,2,3,6,6,4,5}; At<uint8>(heldEnt.data(),0x13C)= next()%5 ? ot[next()%9] : uint8(next()); }
                    for(auto& t:htask)At<void*>(t.data(),8)= next()%4==0 ? nullptr : heldEnt.data();
                    At<void*>(goPick.data(),0xC)= next()%3==0 ? nullptr : heldEnt.data();
                    for(size_t k=0;k<3;++k)found[k]= next()%5<2 ? uintptr_t(htask[k].data()) : 0; found[3]= next()%3 ? uintptr_t(goPick.data()) : 0;
                    holdRet= next()%3==0 ? 0 : uintptr_t(htask[0].data()); dropClears=next()%2; canThrowRet=next()%2;
                    // ---- animaciones y estadisticas ----
                    for(auto& as:assoc){ fillArr(as); At<float>(as.data(),0x1C)=uni(-6.f,6.f); At<uint16>(as.data(),0x2E)= next()%2 ? uint16(0x10|(next()&0xFF)) : uint16(next()&0xFFEF); }
                    for(auto& s:assocSeq){ const uint32 m=next()%5; s= m==0 ? 0 : uintptr_t(assoc[m%3].data()); }
                    { const uint32 t=nm=="GetWeaponSkill-Current" ? At(static_cast<uint8*>(ped.data())+P_WEAPONS+W_SIZE*At<uint8>(ped.data(),P_ACTIVE),W_TYPE) : skillT;
                      const float rp=float(int32(AtA<uint32>(ADDR_WINFO+0x70*uintptr_t(t<47?t:0)+0x24*0x70+0x34))), rs=float(int32(AtA<uint32>(ADDR_WINFO+0x70*uintptr_t(t<47?t:0)+0x34)));
                      const float cand[]{rp,rp-1.f,rp+1.f,rs,rs-1.f,rs+1.f,0.f,1000.f,std::bit_cast<float>(0x7FC00000u),uni(0.f,1000.f)};
                      for(auto& sv:statVals)sv=cand[next()%10]; }
                    // ---- llamada ----
                    CPed* const pd=Ped();
                    setState(candidate);
                    const uintptr_t target=fn.address;
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    uintptr_t rawRet=0; json ret=nullptr; bool isVoid=false;
                    using TC=void(__thiscall*)(CPed*);
                    if(nm=="GetWeaponSlot")ret=reinterpret_cast<int32(__thiscall*)(CPed*,eWeaponType)>(target)(pd,eWeaponType(wtype));
                    else if(nm=="GrantAmmo"||nm=="SetAmmo"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,eWeaponType,uint32)>(target)(pd,eWeaponType(wtype),ammo); }
                    else if(nm=="DoWeHaveWeaponAvailable"){ if(next()%2&&slotOf(wtype)>=0)At(ped.data(),P_WEAPONS+W_SIZE*size_t(slotOf(wtype))+W_TYPE)=wtype;
                        ret=reinterpret_cast<bool(__thiscall*)(CPed*,eWeaponType)>(target)(pd,eWeaponType(wtype)); }
                    else if(nm=="DoGunFlash"){ static const int32 durs[]{1,2,3,5,7,10,100,10000,10001,20000,-1,-5,-10000,100000,0x7FFFFFFF,int32(0x80000000)};
                        ret=reinterpret_cast<bool(__thiscall*)(CPed*,int32,bool)>(target)(pd,durs[a%16],(b&1)!=0); }
                    else if(nm=="SetGunFlashAlpha"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,bool)>(target)(pd,(b&1)!=0); }
                    else if(nm=="ResetGunFlashAlpha"){ isVoid=true; reinterpret_cast<TC>(target)(pd); }
                    else if(nm=="GetHoldingTask")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(CPed*)>(target)(pd));
                    else if(nm=="GetEntityThatThisPedIsHolding")rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(CPed*)>(target)(pd));
                    else if(nm=="DropEntityThatThisPedIsHolding"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,bool)>(target)(pd,(b&1)!=0); }
                    else if(nm=="CanThrowEntityThatThisPedIsHolding")ret=reinterpret_cast<bool(__thiscall*)(CPed*)>(target)(pd);
                    else if(nm=="GiveObjectToPedToHold"){
                        // solo ramas que terminan antes de crear el CObject: hay tarea HOLD_ENTITY y (no hay entidad o no se pide reemplazar)
                        found[0]=uintptr_t(htask[0].data()); const bool hasEnt=At<uintptr_t>(htask[0].data(),8)!=0;
                        const uint8 replace= hasEnt ? uint8(0) : uint8(next()%2);
                        rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(CPed*,int32,uint8)>(target)(pd,int32(next()%5000),replace)); }
                    else if(nm=="RemoveWeaponModel"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,int32)>(target)(pd,next()%9==0 ? int32(100) : modelArg); }
                    else if(nm=="AddWeaponModel"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,int32)>(target)(pd,modelArg); }
                    else if(nm=="GetWeaponSkill-Current")ret=reinterpret_cast<uint8(__thiscall*)(CPed*)>(target)(pd);
                    else if(nm=="GetWeaponSkill-WeaponType")ret=reinterpret_cast<uint8(__thiscall*)(CPed*,eWeaponType)>(target)(pd,eWeaponType(skillT));
                    else if(nm=="SetWeaponSkill"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,eWeaponType,uint32)>(target)(pd,eWeaponType(wtype),next()%4 ? a%4 : a); }
                    else if(nm=="GiveWeapon-"){ uint32 t=wtype; if(next()%3==0){ t=At(static_cast<uint8*>(ped.data())+P_WEAPONS+W_SIZE*size_t(slotOf(wtype)),W_TYPE); if(t>=47)t=wtype; }
                        ret=reinterpret_cast<uint32(__thiscall*)(CPed*,eWeaponType,uint32,bool)>(target)(pd,eWeaponType(t),ammo,(c&1)!=0); }
                    else if(nm=="SetCurrentWeapon-Slot"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,int32)>(target)(pd,a%14==13 ? int32(-1) : int32(a%13)); }
                    else if(nm=="SetCurrentWeapon-WepType"||nm=="ClearWeapon"){ isVoid=true;
                        uint32 t=wtype; if(nm=="ClearWeapon"&&next()%2&&slotOf(wtype)>=0)At(ped.data(),P_WEAPONS+W_SIZE*size_t(slotOf(wtype))+W_TYPE)=wtype;
                        reinterpret_cast<void(__thiscall*)(CPed*,eWeaponType)>(target)(pd,eWeaponType(t)); }
                    else if(nm=="ClearWeapons"||nm=="ReplaceWeaponWhenExitingVehicle"||nm=="ReplaceWeaponForScriptedCutscene"||nm=="RemoveWeaponForScriptedCutscene"||nm=="RequestDelayedWeapon"
                         ||nm=="GiveWeaponAtStartOfFight"||nm=="GiveWeaponWhenJoiningGang"){ isVoid=true; reinterpret_cast<TC>(target)(pd); }
                    else if(nm=="RemoveWeaponWhenEnteringVehicle"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,int32)>(target)(pd,a%8==0 ? int32(2) : int32(a%2)); }
                    else if(nm=="GiveDelayedWeapon"){ isVoid=true; reinterpret_cast<void(__thiscall*)(CPed*,eWeaponType,uint32)>(target)(pd,eWeaponType(wtype),ammo); }
                    else if(nm=="RemoveWeaponAnims"){ isVoid=true; static const float bd[]{0.f,1.f,-1.f,4.f,8.f,-8.f,0.25f};
                        reinterpret_cast<void(__thiscall*)(CPed*,int32,float)>(target)(pd,int32(a),a%3 ? bd[b%7] : uni(-8.f,8.f)); }
                    setState(false);
                    if(ret.is_null()&&!isVoid)ret=Name(rawRet);
                    json r={{"ret",ret},{"ped",Region(ped.data(),ped.size())},{"pdata",Hash(pdata.data(),pdata.size())},{"pinfo",Hash(pinfo.data(),pinfo.size())},
                            {"intel",Hash(Intel(),intelHolder.size())},{"pedclump",Hash(pedClump.data(),pedClump.size())},{"geom",Hash(geom.data(),geom.size())},{"skin",Hash(skin.data(),skin.size())},
                            {"hier",Hash(hier.data(),hier.size())},{"matarr",Hash(matarr.data(),matarr.size())},{"gopick",Region(goPick.data(),goPick.size())},{"heldent",Region(heldEnt.data(),heldEnt.size())},
                            {"winfo",Hash(reinterpret_cast<void*>(ADDR_WINFO),0x70*NTYPES)},{"cheats",{AtA<uint8>(ADDR_CHEAT_AK),AtA<uint8>(ADDR_CHEAT_ROCKET)}},
                            {"found",{found[0]!=0,found[1]!=0,found[2]!=0,found[3]!=0}}};
                    json arrs=json::array();
                    for(size_t k=0;k<2;++k){ arrs.push_back(Region(wclump[k].data(),wclump[k].size())); arrs.push_back(Region(wframe[k].data(),wframe[k].size())); arrs.push_back(Region(gframe[k].data(),gframe[k].size()));
                        arrs.push_back(Region(fx[k].data(),fx[k].size())); arrs.push_back(Region(atom[k].data(),atom[k].size())); }
                    for(auto& t:htask)arrs.push_back(Region(t.data(),t.size()));
                    for(auto& as:assoc)arrs.push_back(Region(as.data(),as.size()));
                    for(auto& m:fmi)arrs.push_back(Region(m.data(),m.size()));
                    r["objects"]=arrs; r["trace"]=trace; r["switches"]=switches;
                    restoreGlobals();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    const bool intactBefore=globalsIntact();
    restoreGlobals();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},{"state_restored",globalsIntact()}});
}
}
