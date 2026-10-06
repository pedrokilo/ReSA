#pragma once
// Banco dentro del motor: CPed, grupo B (ropa, dibujo y muerte; 25 funciones): AddGogglesModel, RemoveGogglesModel,
// PutOnGoggles, TakeOffGoggles, IsWearingGoggles, Dress, Undress, CreateDeadPedMoney, CreateDeadPedPickupCoors (float&),
// CreateDeadPedWeaponPickups, DeadPedMakesTyresBloody, RemoveBodyPart, SpawnFlyingComponent, PedCanPickUpPickUp,
// SetPedAtomicVisibilityCB, SetupLighting, RemoveLighting, Render, PreRender, PreRenderAfterTest, DeleteRwObject,
// SetModelIndex, SetCharCreatedBy, UseGroundColModel y ProcessBuoyancy (las virtuales por la vtable del EXE 0x86C358).
// Original (hooks de estas funciones desactivados) frente a candidato con fixtures deterministas. Nunca se toca un
// peatón real ni el jugador: el CPed es un búfer FALSO del tamaño de CPed (patrón 0xCD) con vptr a una vtable falsa con
// trazas, y todo lo que el EXE desreferencia apunta a búferes propios (vehículo, inteligencia, tareas, clumps con byte de
// tipo 2, frames, atomic, jerarquía y matrices de huesos, model infos con vtable de trazas, col model de impactos,
// datos de jugador, ropa, estadísticas, sistemas Fx...). Los model infos falsos se colocan temporalmente en
// ms_modelInfoPtrs. Se simulan y trazan (dirección del EXE + símbolo y cuerpo de la DLL): rand (0x821B1E y
// CGeneral::GetRandomNumber), AddRef/RemoveRef de model info, GetClumpModelInfo, GetFirstAtomic, RpSkinGeometryGetSkin,
// RpClumpForAllAtomics/Destroy/Render, Add/RemoveWeaponModel, ClearWeapons, CStreaming::RequestSpecialModel,
// CWorld::Add/Remove/ClearScanCodes/ProcessVerticalLine, CLocalisation, CPickups, MakeTyresMuddySectorList, luces,
// CPostEffects (visión), CEntity::Render/DeleteRwObject/SetModelIndex/UpdateRpHAnim, tareas, jerarquía de huesos, RW
// (matrices, puntos, frames), estados de render del motor, sombras, viento y zonas de lluvia, Fx (partículas, sangre,
// salpicaduras), agua, audio, eventos (grupo), fuerzas, buoyancy y la respiración del jugador. Globales tocadas (reloj,
// cámara, mirrors, clima, ciclo horario, jugador 0 del mundo, flags de visión, códigos de escaneo, punteros de Fx,
// estados del motor, ms_modelInfoPtrs, buoyancy global) se guardan y restauran en cada ejecución.
#include <fstream>
#include <set>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Vehicle/Vehicle.h"
#include "PedIntelligence.h"
#include "PedIK.h"
#include "PedStats.h"
#include "PedClothesDesc.h"
#include "PlayerPedData.h"
#include "Tasks/TaskManager.h"
#include "Tasks/TaskTypes/TaskSimpleSwim.h"
#include "Tasks/TaskTypes/TaskSimpleJetPack.h"
#include "Tasks/TaskTypes/TaskComplexEnterCarAsDriver.h"
#include "Events/EventGroup.h"
#include "Events/EventInWater.h"
#include "Pickups.h"
#include "Streaming.h"
#include "World.h"
#include "Localisation.h"
#include "Models/BaseModelInfo.h"
#include "Models/ModelInfo.h"
#include "Models/PedModelInfo.h"
#include "Models/ClumpModelInfo.h"
#include "VisibilityPlugins.h"
#include "PostEffects.h"
#include "PopCycle.h"
#include "Renderer.h"
#include "PointLights.h"
#include "Shadows.h"
#include "RealTimeShadowManager.h"
#include "WindModifiers.h"
#include "CullZones.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Buoyancy.h"
#include "WaterLevel.h"
#include "Audio/AudioEngine.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "Cheat.h"
#include "Weapon.h"
#include "WeaponInfo.h"
#include "RwHelper.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace CPedBLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
inline DWORD mainThread{};
inline bool Main(){ return GetCurrentThreadId()==mainThread; }
inline void T(json j){ if(Main())trace.push_back(std::move(j)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- módulo de la DLL y cuerpos de las virtuales (el símbolo de una virtual es un thunk de llamada virtual) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// --- direcciones del EXE ---
constexpr uintptr_t PED_VT=0x86C358, ADDR_MI_PTRS=0xA9B0C8, ADDR_TIMESTEP=0xB7CB5C, ADDR_FRAMECTR=0xB7CB4C, ADDR_SKIPFRAME=0xB7CB89,
    ADDR_SHADOWSTR=0xB7C4E8, ADDR_TCARRAYS=0xB79E50, ADDR_TCSTORED=0xB79FD0, ADDR_CAMPOS=0xB6F02C, ADDR_CAMMAT=0xB6F03C, ADDR_CAMLOD=0xB6F118, ADDR_CAMUP=0xB6F180,
    ADDR_MIRRORTYPE=0xC7C724, ADDR_MIRRORREFL=0xC7C728, ADDR_RAIN=0xC81324, ADDR_QUAKE=0xC81340, ADDR_AREA=0xB72914, ADDR_FXSPLASH=0xA9AE30,
    ADDR_POSTFX=0xC402B8, ADDR_SCANCODE=0xB7CD78, ADDR_PLAYERS=0xB7CD98, ADDR_ENGINE=0xC97B24, ADDR_BUOY=0xC1C890, ADDR_GFX=0xA9AE00, ADDR_AUDIO=0xB6BC90,
    ADDR_RTSHADOW=0xC40350, ADDR_REPEAT=0xB992B8, ADDR_CLUMPOFFSET=0xB5F878, EXE_REMOVEANIM_CB=0x734B90;
constexpr size_t PED_SZ=sizeof(CPed);
constexpr int NBONES=19;

alignas(16) inline std::array<uint8,PED_SZ> ped{}, pedP{};
alignas(16) inline std::array<uint8,0xA00> veh{}, veh2{};
alignas(16) inline std::array<uint8,0x240> intel{}, intelP{};
alignas(16) inline std::array<std::array<uint8,0x80>,6> tasks{};
alignas(16) inline std::array<std::array<uint8,0x400>,3> clumps{};
alignas(16) inline std::array<std::array<uint8,0x100>,3> frames{};
alignas(16) inline std::array<uint8,0x60> atomic{}, geom{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> mis{};
alignas(16) inline std::array<uint8,0x60> miPed{};
alignas(16) inline std::array<uint8,0x30> colModel{}, colData{};
alignas(16) inline std::array<uint8,0x14*8> spheres{};
alignas(16) inline std::array<uint8,0x40> stats{};
alignas(16) inline std::array<uint8,0x100> playerData{}, playerDataP{};
alignas(16) inline std::array<uint8,0x80> clothes{};
alignas(16) inline std::array<uint8,0x40> hier{};
alignas(16) inline std::array<uint8,0x40*64> hierMats{};
alignas(16) inline std::array<uint8,0x20> speedVec{}, clumpData{}, rwObj{};
alignas(16) inline std::array<std::array<uint8,0x20>,NBONES> boneFrames{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> fxBufs{};
alignas(16) inline std::array<uint8,0x48> matP{}, matV{}, matV2{}, matCam{};
alignas(16) inline std::array<uint8,0x10> stateBytes{};
alignas(16) inline std::array<char,64> strA{}, strB{};
alignas(16) inline std::array<uint8,0x40> frameQuat{};
inline std::array<void*,64> pedVt{}, vehVt{}, taskVt{}, miVt{};
inline std::array<uintptr_t,64> thunkRet{};
inline uint8* Clump(int k){ return clumps[size_t(k)].data(); }
inline uint8* Frame(int k){ return frames[size_t(k)].data(); }
inline uint8* Tk(int k){ return tasks[size_t(k)].data(); }
// tareas falsas: 0 swim, 1 jetpack, 2 in air, 3 otra, 4 enter car as driver, 5 hold
inline uint8* Mi(int k){ return mis[size_t(k)].data(); }

// --- nombres estables para punteros ---
struct Named { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Named> named;
inline void Reg(const void* p,size_t n,std::string nm,size_t stride=0){ named.push_back({uintptr_t(p),n,std::move(nm),stride}); }
inline const Named* FindNamed(uintptr_t v){ for(auto& b:named)if(v>=b.p&&v<b.p+b.n)return &b; return nullptr; }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==PED_VT)return "vt_ped";
    if(auto* b=FindNamed(v)){ const size_t off=v-b->p; return b->stride ? b->nm+"#"+std::to_string(off/b->stride)+"+"+std::to_string(off%b->stride) : b->nm+"+"+std::to_string(off); }
    return v;
}
inline json Name(const void* p){ return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n){ json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline uint64 HashNamed(const void* p,size_t n){ uint64 h=1469598103934665603ull; const auto* w=static_cast<const uint32*>(p);
    for(size_t k=0;k<n/4;++k){ if(auto* b=FindNamed(w[k])){ const uint32 off=uint32(w[k]-b->p); h=Fnv(b->nm.data(),b->nm.size(),h); h=Fnv(&off,4,h); } else { const uint32 v=w[k]; h=Fnv(&v,4,h); } }
    const auto rest=n%4; if(rest)h=Fnv(static_cast<const uint8*>(p)+n-rest,rest,h); return h; }
inline json Snap(const void* p,size_t n,bool detail){ return detail ? Region(p,n) : json(HashNamed(p,n)); }
inline json V(const CVector* v){ return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }
inline json Floats(const void* p,int n){ json r=json::array(); for(int k=0;k<n;++k)r.push_back(static_cast<const uint32*>(p)[k]); return r; }

// --- generador pseudoaleatorio de fixtures ---
struct Rng {
    uint32 st{};
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool coin(int n){ return next()%uint32(n)==0; }
    int32 pick(std::initializer_list<int32> l){ return *(l.begin()+next()%l.size()); }
    float pickF(std::initializer_list<float> l){ return *(l.begin()+next()%l.size()); }
    CVector vec(float r){ return CVector{uni(-r,r),uni(-r,r),uni(-r,r)}; }
};

// --- configuración por caso de las dependencias simuladas ---
struct Cfg {
    uintptr_t clumpMi{}, firstAtomic{}, skinRet{}, clumpNew{}, hitMi{};
    bool steal{}; uint32 mergeMask{}, mergeCalls{}; uint32 coorsCalls{};
    uintptr_t swim{}, jet{}, air{}, hold{}, simplest{}, active704{}, active703{}, active701{}, active717{}, active824{}, enterTarget{};
    bool vision{}, shootLimbs{}, headAbove{}, windRet{}, playerNoRain{}, camNoRain{}, lightRet{}, vlineRet{}, buoyRet{}, waterRet{}, setsShadowData{};
    uint32 alphaRef{}; int32 fxQuality{}; uintptr_t frameRet{}, assocRet{}, colEntity{}, findTask{}; uint32 findTaskMask{}, findTaskCalls{};
    CVector bonePos[2]{}; float windValue{}; float waterZ{}; float headZ{}; CVector buoyTurn{}, buoyForce{};
    uint32 groupMask{}; uint32 groupCalls{};
};
inline Cfg cfg{};
inline uint32 rngState{};
inline int __cdecl FakeRand(){ if(!Main())return 0x1234; T({"rand"}); rngState=rngState*1103515245u+12345u; return int((rngState>>16)&0x7FFF); }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// vtables falsas con trazas: la entrada N devuelve thunkRet[N]; ninguna de las usadas lleva argumentos salvo SetModelIndex del peatón
template<int N> inline uintptr_t __fastcall VThunk(void* self,void*){ T({"vcall",N,Name(self)}); return thunkRet[size_t(N)]; }
template<size_t... I> inline void FillVt(std::array<void*,64>& a,std::index_sequence<I...>){ ((a[I]=reinterpret_cast<void*>(&VThunk<int(I)>)),...); }
inline void __fastcall PedSetModelThunk(void* self,void*,uint32 idx){ T({"vcall",5,Name(self),idx}); }

// --- dependencias simuladas ---
inline void __fastcall FakeAddRef(void* mi,void*){ T({"add_ref",Name(mi)}); }
inline void __fastcall FakeRemoveRef(void* mi,void*){ T({"remove_ref",Name(mi)}); }
inline void* __cdecl FakeGetClumpModelInfo(void* c){ T({"get_clump_mi",Name(c)}); return reinterpret_cast<void*>(cfg.clumpMi); }
inline void* __cdecl FakeGetFirstAtomic(void* c){ T({"get_first_atomic",Name(c)}); return reinterpret_cast<void*>(cfg.firstAtomic); }
inline void* __cdecl FakeSkinGeom(void* g){ T({"skin_geometry",Name(g)}); return reinterpret_cast<void*>(cfg.skinRet); }
inline void* __cdecl FakeForAllAtomics(void* c,void* cb,void* d){ const bool isCb=uintptr_t(cb)==EXE_REMOVEANIM_CB||FollowIn(uintptr_t(cb))==FollowIn(Addr(&AtomicRemoveAnimFromSkinCB));
    T({"for_all_atomics",Name(c),isCb ? json("RemoveAnimFromSkinCB") : json(uintptr_t(cb)),Name(d)}); return c; }
inline RwBool __cdecl FakeClumpDestroy(void* c){ T({"clump_destroy",Name(c)}); return 1; }
inline void* __cdecl FakeClumpRender(void* c){ T({"clump_render",Name(c)}); return c; }
inline void __fastcall FakeRemoveWeaponModel(void* p,void*,int32 m){ T({"remove_weapon_model",Name(p),m}); }
inline void __fastcall FakeAddWeaponModel(void* p,void*,int32 m){ T({"add_weapon_model",Name(p),m}); }
inline void __fastcall FakeClearWeapons(void* p,void*){ T({"clear_weapons",Name(p)}); }
inline void __cdecl FakeRequestSpecialModel(int32 id,const char* nm,int32 fl){ T({"request_special_model",id,nm ? json(Fnv(nm,strlen(nm))) : json(nullptr),fl}); }
inline void __cdecl FakeWorldAdd(void* e){ T({"world_add",Name(e)}); }
inline void __cdecl FakeWorldRemove(void* e){ T({"world_remove",Name(e)}); }
inline bool __cdecl FakeSteal(){ T({"steal_from_dead"}); return cfg.steal; }
inline bool __cdecl FakeShootLimbs(){ T({"shoot_limbs"}); return cfg.shootLimbs; }
inline void __cdecl FakeSomeMoney(float x,float y,float z,int32 amount){ T({"create_some_money",F(x),F(y),F(z),amount}); }
inline void __cdecl FakePickupCoors(float x,float y,float z,float* ox,float* oy,float* oz){
    T({"pickup_coors",F(x),F(y),F(z)}); *ox=x+0.5f+float(cfg.coorsCalls); *oy=y-0.25f; *oz=z+1.75f; ++cfg.coorsCalls; }
inline bool __cdecl FakeTryMerge(float x,float y,float z,int32 type,int32 pt,uint32 ammo,uint32 b){
    T({"try_merge",F(x),F(y),F(z),type,pt,ammo,b&0xFF}); return (cfg.mergeMask>>(cfg.mergeCalls++%16))&1; }
// la DLL devuelve tPickupReference por puntero oculto (primer argumento)
inline void* __cdecl FakeGenerateNewDll(uint32* hidden,float x,float y,float z,int32 type,int32 pt,uint32 ammo,uint32 empty,void* msg){
    T({"generate_new",F(x),F(y),F(z),type,pt,ammo,empty&0xFF,Name(msg)}); *hidden=0x1234; return hidden; }
inline uint32 __cdecl FakeGenerateNew(float x,float y,float z,int32 type,int32 pt,uint32 ammo,uint32 empty,void* msg){
    T({"generate_new",F(x),F(y),F(z),type,pt,ammo,empty&0xFF,Name(msg)}); return 0x1234; }
inline void __cdecl FakeClearScanCodes(){ T({"clear_scan_codes"}); }
inline void __fastcall FakeMuddyList(void* p,void*,void* list){ T({"muddy_sector_list",Name(p),(intptr_t(list)-intptr_t(ADDR_REPEAT))/12,(intptr_t(list)-intptr_t(ADDR_REPEAT))%12}); }
inline void* __fastcall FakeFindTaskByType(void* self,void*,int32 idx,int32 type){
    T({"find_task_by_type",Name(self),idx,type}); const bool hit=(cfg.findTaskMask>>(cfg.findTaskCalls++%16))&1; return hit ? reinterpret_cast<void*>(cfg.findTask) : nullptr; }
// luces
inline void __cdecl FakeActivateDir(){ T({"activate_directional"}); }
inline void __cdecl FakeDeactivateDir(){ T({"deactivate_directional"}); }
inline void __cdecl FakeAmbient(){ T({"set_ambient_colours"}); }
inline bool __cdecl FakeSetupLightingEntity(void* e){ T({"setup_lighting_entity",Name(e)}); return cfg.lightRet; }
inline void __cdecl FakeRemoveLights(){ T({"remove_lights_affecting_object"}); }
// Render
inline bool __cdecl FakeVisionActive(){ T({"vision_fx_active"}); return cfg.vision; }
inline void __cdecl FakeInfraredStore(void* p){ T({"infrared_store",Name(p)}); }
inline void __cdecl FakeNightVision(){ T({"night_vision_lights"}); }
inline void __cdecl FakeInfraredRestore(){ T({"infrared_restore"}); }
inline void __fastcall FakeEntityRender(void* e,void*){ T({"entity_render",Name(e)}); }
inline void* __fastcall FakeFindActive(void* mgr,void*,int32 type){
    T({"find_active_task",Name(mgr),type}); switch(type){case 704:return reinterpret_cast<void*>(cfg.active704);case 703:return reinterpret_cast<void*>(cfg.active703);
        case 701:return reinterpret_cast<void*>(cfg.active701);case 717:return reinterpret_cast<void*>(cfg.active717);case 824:return reinterpret_cast<void*>(cfg.active824);default:return nullptr;} }
inline void* __fastcall FakeSimplest(void* mgr,void*){ T({"simplest_active_task",Name(mgr)}); return reinterpret_cast<void*>(cfg.simplest); }
inline void* __fastcall FakeGetTaskSwim(void* in,void*){ T({"task_swim",Name(in)}); return reinterpret_cast<void*>(cfg.swim); }
inline void* __fastcall FakeGetTaskHold(void* in,void*,uint32 ignore){ T({"task_hold",Name(in),ignore&0xFF}); return reinterpret_cast<void*>(cfg.hold); }
inline void* __fastcall FakeGetTaskJet(void* in,void*){ T({"task_jetpack",Name(in)}); return reinterpret_cast<void*>(cfg.jet); }
inline void* __fastcall FakeGetTaskAir(void* in,void*){ T({"task_in_air",Name(in)}); return reinterpret_cast<void*>(cfg.air); }
inline void __cdecl FakeAddWeaponPed(void* p){ T({"add_weapon_ped_for_pc",Name(p)}); }
inline void __fastcall FakeResetGunFlash(void* p,void*){ T({"reset_gun_flash",Name(p)}); }
inline void* __cdecl FakeHier(void* c){ T({"anim_hierarchy",Name(c)}); return hier.data(); }
inline int32 __cdecl FakeHierIndex(void* h,int32 id){ T({"hier_index",Name(h),id}); return int32((uint32(id)*13u+3u)%64u); }
inline void* __cdecl FakeHierMats(void* h){ T({"hier_matrices",Name(h)}); return hierMats.data(); }
inline void* __cdecl FakeTransformPoints(float* out,const float* in,int32 n,const void* m){
    T({"transform_points",n,Name(m),Floats(in,3)}); const float x=in[0],y=in[1],z=in[2]; out[0]=x*2.f+1.f; out[1]=y*3.f-1.f; out[2]=z+0.5f; return out; }
inline void* __cdecl FakeFrameUpdate(void* f){ T({"frame_update_objects",Name(f)}); return f; }
inline void __fastcall FakeRenderJetPack(void* t,void*,void* p){ T({"render_jetpack",Name(t),Name(p)}); }
inline RwBool __cdecl RSSet(int32 st,void* v){ T({"render_state_set",st,uintptr_t(v)}); return 1; }
inline RwBool __cdecl RSGet(int32 st,void* v){ T({"render_state_get",st}); *reinterpret_cast<uint32*>(v)=cfg.alphaRef; return 1; }
// PreRenderAfterTest
inline void __fastcall FakeApplyRollSwim(void* t,void*,void* p){ T({"swim_apply_roll_pitch",Name(t),Name(p)}); }
inline void __fastcall FakeApplyRollJet(void* t,void*,void* p){ T({"jetpack_apply_roll_pitch",Name(t),Name(p)}); }
inline void __fastcall FakePitchForSlope(void* ik,void*){ T({"pitch_for_slope",Name(ik)}); }
inline void __fastcall FakeUpdateRpHAnim(void* e,void*){ T({"update_rphanim",Name(e)}); }
inline void* __cdecl FakeFrameFromName(void* c,const char* nm){ T({"frame_from_name",Name(c),nm ? json(Fnv(nm,strlen(nm))) : json(nullptr)}); return reinterpret_cast<void*>(cfg.frameRet); }
inline void* __cdecl FakeMatrixRotate(void* m,const float* axis,float angle,int32 op){ T({"matrix_rotate",Name(m),Floats(axis,3),F(angle),op}); return m; }
inline void* __cdecl FakeMatrixScale(void* m,const float* s,int32 op){ T({"matrix_scale",Name(m),Floats(s,3),op}); return m; }
inline int32 __fastcall FakeFxQuality(void* fx,void*){ T({"fx_quality",uintptr_t(fx)==ADDR_GFX}); return cfg.fxQuality; }
inline void __fastcall FakeBonePos(void* p,void*,float* out,int32 bone,uint32 upd){ T({"bone_position",Name(p),bone,upd&0xFF}); const CVector& b=cfg.bonePos[bone ? 1 : 0]; out[0]=b.x; out[1]=b.y; out[2]=b.z; }
inline void __cdecl FakeStoreShadow(void* p,float a,float b,float c,float d,float e,float f){ T({"store_shadow",Name(p),F(a),F(b),F(c),F(d),F(e),F(f)}); }
inline void __fastcall FakeDoShadow(void* mgr,void*,void* p){ T({"do_shadow_this_frame",uintptr_t(mgr)==ADDR_RTSHADOW,Name(p)}); if(cfg.setsShadowData)At<void*>(p,0x134)=fxBufs[0].data(); }
inline void __cdecl FakeShoulderBone(void* c){ T({"shoulder_bone_rotation",Name(c)}); }
inline bool __cdecl FakeWindModifier(float x,float y,float z,float* ox,float* oy){ T({"find_wind_modifier",F(x),F(y),F(z),ox==oy}); if(cfg.windRet){ *ox=cfg.windValue; *oy=cfg.windValue; } return cfg.windRet; }
inline bool __cdecl FakePlayerNoRain(){ T({"player_no_rain"}); return cfg.playerNoRain; }
inline bool __cdecl FakeCamNoRain(){ T({"cam_no_rain"}); return cfg.camNoRain; }
inline void* __fastcall FakeAnimateColModel(void* mi,void*,void* c){ T({"animate_ped_col_model",Name(mi),Name(c)}); return colModel.data(); }
inline CVector* __cdecl FakePlayerSpeed(int32 id){ T({"find_player_speed",id}); return reinterpret_cast<CVector*>(speedVec.data()); }
inline bool __fastcall FakeHeadAbove(void* p,void*,float z){ T({"is_head_above_pos",Name(p),F(z)}); return cfg.headAbove; }
inline void* __cdecl FakeGetAssoc(void* c,uint32 id){ T({"get_association",Name(c),id}); return reinterpret_cast<void*>(cfg.assocRet); }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector* pos,const CVector* vel,float t,const float* mult,float rz,float lm,float lml,uint32 local){
    T({"particle",Name(self),V(pos),V(vel),F(t),Floats(mult,7),F(rz),F(lm),F(lml),local&0xFF}); }
inline void __fastcall FakeAddBlood(void* fx,void*,const CVector* o,const CVector* d,int32 amount,float b){ T({"add_blood",uintptr_t(fx)==ADDR_GFX,V(o),V(d),amount,F(b)}); }
// SetModelIndex
inline void __cdecl FakeAnimClumpInit(void* c){ T({"anim_clump_init",Name(c)}); }
inline void __cdecl FakeAnimFillFrames(void* c,void* arr){ T({"anim_fill_frame_array",Name(c),Name(arr)}); for(int k=0;k<NBONES;++k)At<void*>(arr,size_t(4*k))=boneFrames[size_t(k)].data(); }
inline bool __cdecl FakeIsPedInGroup(int32 model,int32 group){ T({"is_ped_in_group",model,group}); return (cfg.groupMask>>(cfg.groupCalls++%16))&1; }
inline void* __cdecl FakeAddAnim(void* c,int32 group,int32 id){ T({"add_animation",Name(c),group,id}); return nullptr; }
inline void __fastcall FakeCreateHitCol(void* mi,void*,void* c){ T({"create_hit_col_model",Name(mi),Name(c)}); }
inline void __fastcall FakeEntitySetModel(void* e,void*,uint32 idx){ T({"entity_set_model_index",Name(e),idx}); At<int16>(e,0x22)=int16(idx); At<void*>(e,0x18)=Clump(0); }
inline void __fastcall FakeDeleteRw(void* e,void*){ T({"entity_delete_rw_object",Name(e)}); At<void*>(e,0x18)=nullptr; }
inline void __fastcall FakeSetDecisionMaker(void* in,void*,int32 t){ T({"set_decision_maker_type",Name(in),t}); }
// ProcessBuoyancy
inline bool __fastcall FakeProcessBuoyancy(void* self,void*,void* e,float b,CVector* turn,CVector* force){
    T({"process_buoyancy",uintptr_t(self)==ADDR_BUOY,Name(e),F(b)}); if(cfg.buoyRet){ *turn=cfg.buoyTurn; *force=cfg.buoyForce; } return cfg.buoyRet; }
inline bool __cdecl FakeVerticalLine(const CVector* o,float dist,void* cp,void** ent,uint32 b1,uint32 b2,uint32 b3,uint32 b4,uint32 b5,uint32 b6,void* poly){
    T({"vertical_line",V(o),F(dist),b1&0xFF,b2&0xFF,b3&0xFF,b4&0xFF,b5&0xFF,b6&0xFF,Name(poly)}); if(cfg.vlineRet)*ent=reinterpret_cast<void*>(cfg.colEntity); return cfg.vlineRet; }
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float* out,uint32 touching,void* normals){ T({"water_level",F(x),F(y),F(z),touching&0xFF,Name(normals)}); if(cfg.waterRet)*out=cfg.waterZ; return cfg.waterRet; }
inline void __fastcall FakeTriggerSplash(void* fx,void*,const CVector* p){ T({"trigger_water_splash",uintptr_t(fx)==ADDR_GFX,V(p)}); }
inline void __fastcall FakeReportSplash(void* au,void*,void* e,float vol,uint32 force){ T({"report_water_splash",uintptr_t(au)==ADDR_AUDIO,Name(e),F(vol),force&0xFF}); }
inline void __fastcall FakeApplyMoveForce(void* e,void*,float x,float y,float z){ T({"apply_move_force",Name(e),F(x),F(y),F(z)}); }
inline void* __fastcall FakeGroupAdd(void* g,void*,void* ev,uint32 valid){
    T({"event_group_add",Name(g),Name(At<void*>(ev,4)),At<uint8>(ev,8),F(At<float>(ev,0xC)),valid&0xFF}); return ev; }
inline int __cdecl FakePrintf(const char*,...){ return 0; }
inline void __fastcall FakeTransformedBone(void* p,void*,float* io,int32 bone,uint32 upd){ T({"transformed_bone_position",Name(p),F(io[0]),F(io[1]),F(io[2]),bone,upd&0xFF}); io[2]=cfg.headZ; }
inline void __fastcall FakeBreath(void* p,void*,uint32 dec,float mult){ T({"handle_player_breath",Name(p),dec&0xFF,F(mult)}); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=25;
inline const std::array<Fn,NFNS> fns{{
    {"CPed","AddGogglesModel",0x5E3A90,0,-1,192},
    {"CPed","RemoveGogglesModel",0x5DF170,0,-1,192},
    {"CPed","PutOnGoggles",0x5E3AE0,0,-1,256},
    {"CPed","TakeOffGoggles",0x5E6010,0,-1,256},
    {"CPed","IsWearingGoggles",0x479D10,0,-1,64},
    {"CPed","Dress",0x5E0130,0,-1,192},
    {"CPed","Undress",0x5E00F0,0,-1,192},
    {"CPed","CreateDeadPedMoney",0x4590F0,0,-1,512},
    {"CPed","CreateDeadPedPickupCoors-",0x459180,0,-1,128},
    {"CPed","CreateDeadPedWeaponPickups",0x4591D0,0,-1,512},
    {"CPed","DeadPedMakesTyresBloody",0x6B4200,0,-1,512},
    {"CPed","RemoveBodyPart",0x5F0140,0,-1,128},
    {"CPed","SpawnFlyingComponent",0x5F0190,0,-1,64},
    {"CPed","PedCanPickUpPickUp",0x455560,0,-1,128},
    {"CPed","SetPedAtomicVisibilityCB",0x5F0060,0,-1,128},
    {"CPed","SetupLighting",0,0x86C358,19,64},
    {"CPed","RemoveLighting",0,0x86C358,20,64},
    {"CPed","Render",0,0x86C358,18,768},
    {"CPed","PreRender",0,0x86C358,17,256},
    {"CPed","PreRenderAfterTest",0x5E65A0,0,-1,1024},
    {"CPed","DeleteRwObject",0,0x86C358,8,64},
    {"CPed","SetModelIndex",0,0x86C358,5,384},
    {"CPed","SetCharCreatedBy",0x5E47E0,0,-1,256},
    {"CPed","UseGroundColModel",0x5DEFE0,0,-1,128},
    {"CPed","ProcessBuoyancy",0x5E1FA0,0,-1,768},
}};

// matriz de CMatrix (0x48): ejes ortonormales aproximados y posición en 0x30
inline void FillMat(Rng& r,uint8* m,const CVector& pos){
    std::memset(m,0,0x48); const float a=r.uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
    At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,r.coin(5) ? -1.f : 1.f}; At<CVector>(m,0x30)=pos; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpedb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpedb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpedb");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    if(CCheat::IsActive(CHEAT_BIG_HEAD)||CCheat::IsActive(CHEAT_THIN_BODY)){ Write({{"event","error"},{"reason","big head / thin body cheat active"}}); return; }
    std::string only; { std::ifstream f(dir/"cpedb-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpos de la DLL de funciones virtuales de la base (el símbolo de una virtual es un thunk de llamada virtual)
    struct Base { const char* cls; const char* name; uintptr_t exe; uintptr_t stub; uintptr_t dll; };
    std::vector<Base> bases{
        {"CEntity","Render",0x534310,Addr(&FakeEntityRender),0},
        {"CEntity","DeleteRwObject",0x534030,Addr(&FakeDeleteRw),0},
        {"CEntity","SetModelIndex",0x532AE0,Addr(&FakeEntitySetModel),0}};
    for(auto& bs:bases)if(!(bs.dll=DllBody(bs.cls,bs.name))){Write({{"event","error"},{"reason","base body not found"},{"function",std::string(bs.cls)+"::"+bs.name}});return;}

    // vtables falsas
    FillVt(pedVt,std::make_index_sequence<64>{}); FillVt(vehVt,std::make_index_sequence<64>{}); FillVt(taskVt,std::make_index_sequence<64>{}); FillVt(miVt,std::make_index_sequence<64>{});
    pedVt[5]=reinterpret_cast<void*>(&PedSetModelThunk);

    // nombres
    named.clear();
    Reg(ped.data(),ped.size(),"ped"); Reg(pedP.data(),pedP.size(),"pedP"); Reg(veh.data(),veh.size(),"veh"); Reg(veh2.data(),veh2.size(),"veh2");
    Reg(intel.data(),intel.size(),"intel"); Reg(intelP.data(),intelP.size(),"intelP");
    for(int k=0;k<6;++k)Reg(Tk(k),0x80,"task"+std::to_string(k));
    for(int k=0;k<3;++k){ Reg(Clump(k),0x400,"clump"+std::to_string(k)); Reg(Frame(k),0x100,"frame"+std::to_string(k)); Reg(fxBufs[size_t(k)].data(),0x60,"fx"+std::to_string(k)); }
    Reg(atomic.data(),atomic.size(),"atomic"); Reg(geom.data(),geom.size(),"geom");
    for(int k=0;k<4;++k)Reg(Mi(k),0x60,"mi"+std::to_string(k));
    Reg(miPed.data(),miPed.size(),"miPed"); Reg(colModel.data(),colModel.size(),"colModel"); Reg(colData.data(),colData.size(),"colData"); Reg(spheres.data(),spheres.size(),"spheres",0x14);
    Reg(stats.data(),stats.size(),"stats"); Reg(playerData.data(),playerData.size(),"playerData"); Reg(playerDataP.data(),playerDataP.size(),"playerDataP");
    Reg(clothes.data(),clothes.size(),"clothes"); Reg(hier.data(),hier.size(),"hier"); Reg(hierMats.data(),hierMats.size(),"hierMats",0x40);
    Reg(speedVec.data(),speedVec.size(),"speedVec"); Reg(clumpData.data(),clumpData.size(),"clumpData"); Reg(rwObj.data(),rwObj.size(),"rwObj");
    Reg(boneFrames.data(),sizeof(boneFrames),"boneFrame",0x20); Reg(matP.data(),matP.size(),"matP"); Reg(matV.data(),matV.size(),"matV"); Reg(matV2.data(),matV2.size(),"matV2");
    Reg(matCam.data(),matCam.size(),"matCam"); Reg(stateBytes.data(),stateBytes.size(),"stateBytes"); Reg(strA.data(),strA.size(),"strA"); Reg(strB.data(),strB.size(),"strB");
    Reg(pedVt.data(),sizeof(pedVt),"pedVt"); Reg(vehVt.data(),sizeof(vehVt),"vehVt"); Reg(taskVt.data(),sizeof(taskVt),"taskVt"); Reg(miVt.data(),sizeof(miVt),"miVt");
    Reg(reinterpret_cast<void*>(ADDR_POSTFX),2,"postfx"); Reg(reinterpret_cast<void*>(ADDR_REPEAT),sizeof(CRepeatSector)*256,"repeatSector",sizeof(CRepeatSector));
    Reg(reinterpret_cast<void*>(ADDR_BUOY),0x70,"buoy");

    // estado global guardado y restaurado en cada ejecución
    auto** const miPtrs=reinterpret_cast<void**>(ADDR_MI_PTRS);
    std::vector<std::pair<int,void*>> miTouched;
    struct Saved { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Saved> regions;
    auto SnapG=[&](uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); };
    const uintptr_t rsSlot=AtA<uintptr_t>(ADDR_ENGINE)+0x20;
    SnapG(ADDR_TIMESTEP,4); SnapG(ADDR_FRAMECTR,4); SnapG(ADDR_SKIPFRAME,1); SnapG(ADDR_SHADOWSTR,2); SnapG(ADDR_TCARRAYS,0x184); SnapG(ADDR_CAMPOS,0x10); SnapG(ADDR_CAMMAT,4);
    SnapG(ADDR_CAMLOD,4); SnapG(ADDR_CAMUP,4); SnapG(ADDR_MIRRORTYPE,4); SnapG(ADDR_MIRRORREFL,1); SnapG(ADDR_RAIN,4); SnapG(ADDR_QUAKE,4); SnapG(ADDR_AREA,4);
    SnapG(ADDR_FXSPLASH,0xC); SnapG(ADDR_POSTFX,2); SnapG(ADDR_SCANCODE,2); SnapG(ADDR_PLAYERS,4); SnapG(rsSlot,8); SnapG(ADDR_BUOY,0x70);
    auto restoreAll=[&]{
        for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n);
        for(auto it=miTouched.rbegin();it!=miTouched.rend();++it)miPtrs[it->first]=it->second;
        miTouched.clear(); };
    auto stateRestored=[&]{ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return miTouched.empty(); };
    auto setMi=[&](int idx,void* p){ if(idx<=0||idx>=20000)return; miTouched.push_back({idx,miPtrs[idx]}); miPtrs[idx]=p; };

    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using std::bit_cast;
        Patches p;
        // Cuerpos de la DLL resueltos ANTES de escribir parches; nunca se parchea un stub ya registrado.
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        for(auto& bs:bases){ stubSet.insert(bs.stub); stubSet.insert(FollowIn(bs.stub)); }
        for(auto& bs:bases){ p.Add(bs.exe,reinterpret_cast<void*>(bs.stub)); p.Add(bs.dll,reinterpret_cast<void*>(bs.stub)); }
        P(0x821B1E,0,FakeRand); P(0,Addr(&CGeneral::GetRandomNumber),FakeRandomNumber);
        // modelos y RW
        P(0x4C4BA0,Addr(&CBaseModelInfo::AddRef),FakeAddRef); P(0x4C4BB0,Addr(&CBaseModelInfo::RemoveRef),FakeRemoveRef);
        P(0x732AC0,Addr(&CVisibilityPlugins::GetClumpModelInfo),FakeGetClumpModelInfo);
        P(0x734820,Addr(&GetFirstAtomic),FakeGetFirstAtomic); P(0x7C7550,Addr(&RpSkinGeometryGetSkin),FakeSkinGeom);
        P(0x749B70,Addr(&RpClumpForAllAtomics),FakeForAllAtomics); P(0x74A310,Addr(&RpClumpDestroy),FakeClumpDestroy); P(0x749B20,Addr(&RpClumpRender),FakeClumpRender);
        // peatón
        P(0x5E3990,Addr(&CPed::RemoveWeaponModel),FakeRemoveWeaponModel); P(0x5E5ED0,Addr(&CPed::AddWeaponModel),FakeAddWeaponModel);
        P(0x5E6320,Addr(&CPed::ClearWeapons),FakeClearWeapons);
        P(0x409D10,Addr(&CStreaming::RequestSpecialModel),FakeRequestSpecialModel);
        P(0x563220,Addr(&CWorld::Add),FakeWorldAdd); P(0x563280,Addr(&CWorld::Remove),FakeWorldRemove);
        P(0x56D2B0,Addr(&CLocalisation::StealFromDeadPed),FakeSteal); P(0x56D280,Addr(&CLocalisation::ShootLimbs),FakeShootLimbs);
        P(0x458970,Addr(&CPickups::CreateSomeMoney),FakeSomeMoney);
        P(0x458A80,Addr(static_cast<void(*)(float,float,float,float&,float&,float&)>(&CPickups::CreatePickupCoorsCloseToCoors)),FakePickupCoors);
        P(0x4555A0,Addr(&CPickups::TryToMerge_WeaponType),FakeTryMerge); P(0x457380,0,FakeGenerateNew); P(0,Addr(&CPickups::GenerateNewOne_WeaponType),FakeGenerateNewDll);
        P(0x563470,Addr(&CWorld::ClearScanCodes),FakeClearScanCodes);
        P(0x6AE0D0,Addr(&CPed::MakeTyresMuddySectorList<CPtrListDoubleLink<CVehicle*>>),FakeMuddyList);
        P(0,Addr(&CPed::MakeTyresMuddySectorList<CPtrListSingleLink<CPhysical*>>),FakeMuddyList);
        P(0x6817D0,Addr(&CTaskManager::FindTaskByType),FakeFindTaskByType);
        P(0x735C80,Addr(&ActivateDirectional),FakeActivateDir); P(0x735C70,Addr(&DeActivateDirectional),FakeDeactivateDir);
        P(0x735D30,Addr(static_cast<void(*)()>(&SetAmbientColours)),FakeAmbient);
        P(0x553E40,Addr(&CRenderer::SetupLightingForEntity),FakeSetupLightingEntity); P(0x6FFFE0,Addr(&CPointLights::RemoveLightsAffectingObject),FakeRemoveLights);
        // Render
        P(0x7034F0,Addr(&CPostEffects::IsVisionFXActive),FakeVisionActive); P(0x701320,Addr(&CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects),FakeInfraredStore);
        P(0x7012E0,Addr(&CPostEffects::NightVisionSetLights),FakeNightVision); P(0x701410,Addr(&CPostEffects::InfraredVisionRestoreLightsForHeatObjects),FakeInfraredRestore);
        P(0x681740,Addr(&CTaskManager::FindActiveTaskByType),FakeFindActive); P(0x6819D0,Addr(&CTaskManager::GetSimplestActiveTask),FakeSimplest);
        P(0x601070,Addr(&CPedIntelligence::GetTaskSwim),FakeGetTaskSwim); P(0x600FF0,Addr(&CPedIntelligence::GetTaskHold),FakeGetTaskHold);
        P(0x601110,Addr(&CPedIntelligence::GetTaskJetPack),FakeGetTaskJet); P(0x601150,Addr(&CPedIntelligence::GetTaskInAir),FakeGetTaskAir);
        P(0x5E46D0,Addr(&CVisibilityPlugins::AddWeaponPedForPC),FakeAddWeaponPed); P(0x5DF4E0,Addr(&CPed::ResetGunFlashAlpha),FakeResetGunFlash);
        P(0x734A40,Addr(&GetAnimHierarchyFromSkinClump),FakeHier); P(0x7C51A0,Addr(&RpHAnimIDGetIndex),FakeHierIndex); P(0x7C5120,Addr(&RpHAnimHierarchyGetMatrixArray),FakeHierMats);
        P(0x7EDD90,Addr(&RwV3dTransformPoints),FakeTransformPoints); P(0x7F0910,Addr(&RwFrameUpdateObjects),FakeFrameUpdate);
        P(0x67F6A0,Addr(&CTaskSimpleJetPack::RenderJetPack),FakeRenderJetPack);
        // PreRenderAfterTest
        P(0x68A8E0,Addr(&CTaskSimpleSwim::ApplyRollAndPitch),FakeApplyRollSwim); P(0x67B720,Addr(&CTaskSimpleJetPack::ApplyRollAndPitch),FakeApplyRollJet);
        P(0x5FE0E0,Addr(&CPedIK::PitchForSlope),FakePitchForSlope); P(0x532B20,Addr(&CEntity::UpdateRpHAnim),FakeUpdateRpHAnim);
        P(0x4C5400,Addr(&CClumpModelInfo::GetFrameFromName),FakeFrameFromName);
        P(0x7F1FD0,Addr(&RwMatrixRotate),FakeMatrixRotate); P(0x7F22C0,Addr(&RwMatrixScale),FakeMatrixScale);
        P(0x49EA50,Addr(&Fx_c::GetFxQuality),FakeFxQuality);
        P(0x5E4280,Addr(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition)),FakeBonePos);
        P(0x707B40,Addr(&CShadows::StoreShadowForPedObject),FakeStoreShadow); P(0x706BA0,Addr(&CRealTimeShadowManager::DoShadowThisFrame),FakeDoShadow);
        P(0x5DF560,Addr(&CPed::ShoulderBoneRotation),FakeShoulderBone);
        P(0x72C950,Addr(&CWindModifiers::FindWindModifier),FakeWindModifier); P(0x72DDC0,Addr(&CCullZones::PlayerNoRain),FakePlayerNoRain); P(0x72DDB0,Addr(&CCullZones::CamNoRain),FakeCamNoRain);
        P(0x4C7170,Addr(&CPedModelInfo::AnimatePedColModelSkinnedWorld),FakeAnimateColModel);
        P(0x56E090,Addr(&FindPlayerSpeed),FakePlayerSpeed); P(0x5F02C0,Addr(&CPed::IsPedHeadAbovePos),FakeHeadAbove);
        P(0x4D68B0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc);
        P(0x4AA440,Addr(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle);
        P(0x49EB00,Addr(&Fx_c::AddBlood),FakeAddBlood);
        // SetModelIndex
        P(0x4D6720,Addr(&RpAnimBlendClumpInit),FakeAnimClumpInit); P(0x4D64A0,Addr(&RpAnimBlendClumpFillFrameArray),FakeAnimFillFrames);
        P(0x610210,Addr(&CPopCycle::IsPedInGroup),FakeIsPedInGroup);
        P(0x4D3AA0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation)),FakeAddAnim);
        P(0x4C6D90,Addr(&CPedModelInfo::CreateHitColModelSkinned),FakeCreateHitCol); P(0x600B50,Addr(&CPedIntelligence::SetPedDecisionMakerType),FakeSetDecisionMaker);
        // ProcessBuoyancy
        P(0x6C3EF0,Addr(&cBuoyancy::ProcessBuoyancy),FakeProcessBuoyancy);
        P(0x5674E0,Addr(&CWorld::ProcessVerticalLine),FakeVerticalLine);
        P(0x6EB690,Addr(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel);
        P(0x4A1070,Addr(&Fx_c::TriggerWaterSplash),FakeTriggerSplash);
        P(0x506F00,Addr(static_cast<void(CAudioEngine::*)(CPhysical*,float,bool)>(&CAudioEngine::ReportWaterSplash)),FakeReportSplash);
        P(0x5429F0,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeApplyMoveForce);
        P(0x4AB420,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)),FakeGroupAdd);
        P(0x5E01C0,Addr(&CPed::GetTransformedBonePosition),FakeTransformedBone); P(0x60A8D0,Addr(&CPlayerPed::HandlePlayerBreath),FakeBreath);
        // printf del CRT del EXE (RemoveBodyPart sin KeyFrame): sin traza, la DLL solo escribe en el log
        p.Add(0x821982,&FakePrintf);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate,bool detail)->json {
                    Rng r{}; r.st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xBBBBu; if(!r.st)r.st=1; trace=json::array(); rngState=uint32(i)*7919u+uint32(f);
                    restoreAll(); cfg=Cfg{};
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // globales
                    AtA<float>(ADDR_TIMESTEP)=r.uni(0.2f,3.f); AtA<uint32>(ADDR_FRAMECTR)=r.next(); AtA<uint8>(ADDR_SKIPFRAME)=uint8(r.coin(3));
                    AtA<uintptr_t>(rsSlot)=bit_cast<uintptr_t>(&RSSet); AtA<uintptr_t>(rsSlot+4)=bit_cast<uintptr_t>(&RSGet);
                    std::fill(thunkRet.begin(),thunkRet.end(),0);
                    cfg.alphaRef=r.next();
                    // peatón base
                    auto* pd=ped.data(); std::memset(pd,0xCD,ped.size());
                    std::memset(pedP.data(),0xCD,pedP.size()); std::memset(veh.data(),0xCD,veh.size()); std::memset(veh2.data(),0xCD,veh2.size());
                    std::memset(intel.data(),0xCD,intel.size()); std::memset(intelP.data(),0xCD,intelP.size());
                    for(auto& t:tasks){ std::memset(t.data(),0xCD,t.size()); At<void*>(t.data(),0)=taskVt.data(); }
                    for(int k=0;k<3;++k){ std::memset(Clump(k),0xCD,0x400); Clump(k)[0]=2; Clump(k)[1]=0; At<void*>(Clump(k),4)=Frame(k); std::memset(Frame(k),0xCD,0x100); std::memset(fxBufs[size_t(k)].data(),0xCD,0x60); }
                    for(int k=0;k<4;++k){ std::memset(Mi(k),0xCD,0x60); At<void*>(Mi(k),0)=miVt.data(); }
                    std::memset(miPed.data(),0xCD,miPed.size()); At<void*>(miPed.data(),0)=miVt.data();
                    std::memset(atomic.data(),0xCD,atomic.size()); At<void*>(atomic.data(),0x18)=geom.data(); std::memset(geom.data(),0xCD,geom.size());
                    auto zap=[](auto& arr){ std::memset(arr.data(),0xCD,arr.size()); };
                    zap(colModel); zap(colData); zap(stats); zap(playerData); zap(playerDataP); zap(clothes); zap(hier); zap(speedVec); zap(clumpData); zap(rwObj); zap(stateBytes); zap(frameQuat);
                    std::memset(hierMats.data(),0xCD,hierMats.size()); std::memset(spheres.data(),0xCD,spheres.size());
                    for(auto& b:boneFrames)std::memset(b.data(),0xCD,b.size());
                    for(auto* m:{matP.data(),matV.data(),matV2.data(),matCam.data()})std::memset(m,0xCD,0x48);
                    std::memset(strA.data(),0,strA.size()); std::memset(strB.data(),0,strB.size());
                    for(int k=0;k<12;++k){ strA[size_t(k)]=char('a'+r.next()%26); strB[size_t(k)]=char('A'+r.next()%26); }
                    const CVector pos=r.vec(60.f);
                    At<void*>(pd,0x00)=pedVt.data(); At<CVector>(pd,0x04)=pos; FillMat(r,matP.data(),pos+(r.coin(2) ? CVector{0.f,0.f,0.f} : r.vec(1.f)));
                    At<void*>(pd,0x14)=r.coin(3) ? nullptr : matP.data();
                    At<void*>(pd,0x18)=Clump(0); At<uint32>(pd,0x1C)=r.next(); At<int16>(pd,0x22)=int16(r.pick({0,1,7,100,150,299,-1,-300}));
                    pd[0x36]=uint8((r.next()&0xF8)|3); At<uint32>(pd,0x40)=r.next(); for(size_t o=0x44;o<0x5C;o+=4)At<float>(pd,o)=r.uni(-0.3f,0.3f);
                    At<float>(pd,0x8C)=r.uni(40.f,120.f);
                    At<uint32>(pd,0x46C)=r.next(); At<uint32>(pd,0x470)=r.next(); At<uint32>(pd,0x474)=r.next(); At<uint32>(pd,0x478)=r.next();
                    At<void*>(pd,0x47C)=intel.data(); At<void*>(pd,0x480)=nullptr; pd[0x484]=uint8(r.next()%4);
                    for(int k=0;k<NBONES;++k)At<void*>(pd,size_t(0x488+4*k))=boneFrames[size_t(k)].data();
                    At<void*>(pd,0x4F4)=nullptr; At<void*>(pd,0x4FC)=nullptr; At<void*>(pd,0x500)=nullptr;
                    At<uint32>(pd,0x530)=uint32(r.pick({0,1,2,5,16,17,42,45,50,54,55,58}));
                    At<void*>(pd,0x58C)=nullptr; At<uint32>(pd,0x598)=uint32(r.pick({0,1,3,4,5,6,18,19,7}));
                    At<void*>(pd,0x59C)=stats.data(); At<float>(stats.data(),0x20)=r.uni(10.f,500.f); stats[0x32]=uint8(r.pick({0,1,2,3,4,5,6,255,254}));
                    At<uint32>(pd,0x718)=0; pd[0x718]=uint8(r.pick({0,1,2,5,12,255})); At<uint16>(pd,0x756)=uint16(r.pick({0,9,10,11,100,400,0x7FFF,0x8000,0xFFFF}));
                    At<float>(pd,0x758)=r.pickF({0.f,1.f,0.5f,3.f,-1.f}); At<float>(pd,0x75C)=r.uni(0.f,0.5f); pd[0x754]=uint8(r.pick({0,2,2,5,18}));
                    At<void*>(pd,0x134)=nullptr;
                    // inteligencia: tareas, grupo de eventos
                    At<uint32>(intel.data(),0)=0; At<void*>(intel.data(),0)=pd;
                                    // objetos auxiliares de uso general
                    thunkRet[11]=uintptr_t(Clump(1));
                    uint8* const veh0=veh.data();
                    auto makeVeh=[&](uint8* v,uint8* mat){ std::memset(v,0xCD,0xA00); At<void*>(v,0)=vehVt.data(); const CVector vp=r.vec(60.f); At<CVector>(v,4)=vp; FillMat(r,mat,vp);
                        At<void*>(v,0x14)=mat; v[0x36]=uint8((r.next()&0xF8)|2); At<uint32>(v,0x40)=r.next(); for(size_t o=0x44;o<0x5C;o+=4)At<float>(v,o)=r.uni(-0.5f,0.5f);
                        At<uint32>(v,0x590)=uint32(r.pick({0,5,9,6,3,1,4})); At<uint32>(v,0x594)=uint32(r.pick({0,2,5,9,10,1,3})); At<float>(v,0x12C)=r.uni(0.f,1.f); };
                    auto tskTypeRet=[&](){ thunkRet[4]=uintptr_t(r.pick({254,254,1,5,700})); };

                    // -------------------------------------------------------------------------------------------------
                    if(nm=="AddGogglesModel"||nm=="RemoveGogglesModel"||nm=="PutOnGoggles"||nm=="TakeOffGoggles"||nm=="IsWearingGoggles") {
                        const int16 mA=int16(19990), mB=int16(19991);
                        cfg.clumpMi=uintptr_t(Mi(2)); cfg.firstAtomic=r.coin(2) ? uintptr_t(atomic.data()) : 0; cfg.skinRet=r.coin(2) ? uintptr_t(geom.data()) : 0;
                        const bool had=nm=="AddGogglesModel"||nm=="RemoveGogglesModel" ? r.coin(2) : (nm=="TakeOffGoggles" ? !r.coin(4) : r.coin(3));
                        At<void*>(pd,0x4FC)= nm=="IsWearingGoggles" ? (r.coin(2) ? nullptr : reinterpret_cast<void*>(uintptr_t(r.next()|1))) : (had ? Clump(2) : nullptr);
                        At<void*>(pd,0x500)=r.coin(3) ? nullptr : stateBytes.data(); stateBytes[0]=uint8(r.next()); stateBytes[1]=uint8(r.next()); stateBytes[2]=uint8(r.next());
                        setMi(mA,Mi(0)); setMi(mB,Mi(1)); thunkRet[11]=uintptr_t(Clump(1));
                        if(nm=="AddGogglesModel"){
                            const int32 idx=r.pick({-1,mA,mB,-1,mA});
                            reinterpret_cast<void(__thiscall*)(void*,int32,bool*)>(a)(pd,idx,reinterpret_cast<bool*>(&stateBytes[8]));
                            out["stateNew"]=stateBytes[8];
                        } else if(nm=="RemoveGogglesModel") {
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        } else if(nm=="IsWearingGoggles") {
                            ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(pd));
                        } else {
                            const int32 slot=CWeaponInfo::GetWeaponInfo(WEAPON_INFRARED,eWeaponSkill::STD)->m_nSlot;
                            const int32 m1=CWeaponInfo::GetWeaponInfo(WEAPON_INFRARED,eWeaponSkill::STD)->m_nModelId1, m2=CWeaponInfo::GetWeaponInfo(WEAPON_NIGHTVISION,eWeaponSkill::STD)->m_nModelId1;
                            setMi(m1,Mi(0)); setMi(m2,Mi(1));
                            for(int k=0;k<13;++k){ auto* w=pd+0x5A0+k*0x1C; At<int32>(w,0)=r.pick({0,1,38,40,12}); w[0x15]=uint8(r.next()); }
                            auto* w=pd+0x5A0+slot*0x1C; At<int32>(w,0)=r.pick({45,44,45,44,0,1,38});
                            pd[0x718]=uint8(r.pick({slot,slot,0,1,12,255}));
                            AtA<uint8>(ADDR_POSTFX)=uint8(r.next()); AtA<uint8>(ADDR_POSTFX+1)=uint8(r.next());
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                            out["postfx"]=json::array({AtA<uint8>(ADDR_POSTFX),AtA<uint8>(ADDR_POSTFX+1)});
                            out["stateBytes"]=ReversingProbe::Bytes(stateBytes.data(),stateBytes.size());
                            out["models"]=json::array({m1,m2});
                        }
                        out["stateBytes"]=ReversingProbe::Bytes(stateBytes.data(),stateBytes.size());
                    }
                    else if(nm=="Dress"||nm=="Undress") {
                        At<void*>(intel.data(),0)=pd;
                        if(nm=="Dress")reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        else reinterpret_cast<void(__thiscall*)(void*,char*)>(a)(pd,r.coin(4) ? nullptr : strA.data());
                    }
                    else if(nm=="CreateDeadPedMoney") {
                        cfg.steal=!r.coin(5); At<uint32>(pd,0x598)=uint32(r.pick({0,1,4,6,18,19,5,12})); pd[0x484]=uint8(r.pick({0,1,2,2,3}));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                    }
                    else if(nm=="CreateDeadPedPickupCoors-") {
                        float x=-777.f, y=-888.f, z=-999.f;
                        reinterpret_cast<void(__thiscall*)(void*,float*,float*,float*)>(a)(pd,&x,&y,&z);
                        ret=json::array({F(x),F(y),F(z)});
                    }
                    else if(nm=="CreateDeadPedWeaponPickups") {
                        cfg.mergeMask=r.next(); if(!r.coin(4))At<uint32>(pd,0x46C)&=~0x100u; if(!r.coin(4))At<uint32>(pd,0x474)&=~0x200u;
                        for(int k=0;k<13;++k){ auto* w=pd+0x5A0+k*0x1C; At<int32>(w,0)=r.pick({0,0,40,1,2,3,10,15,22,24,25,30,31,34,38,41,46,16,18,36,43});
                            At<uint32>(w,0xC)=uint32(r.pick({0,0,1,5,100,500,0x7FFFFFFF,-1,70,36})); At<uint32>(w,8)=r.next()%40; w[0x15]=uint8(r.next()); }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                    }
                    else if(nm=="DeadPedMakesTyresBloody") {
                        const float coords[]{-3000.f,-2990.f,-2900.f,-2.f,0.f,2.f,49.9f,50.f,100.f,2900.f,2990.f,3000.f,3100.f,-3100.f,6000.f,-6000.f};
                        const bool useMat=r.coin(2); CVector pp{coords[r.next()%16],coords[r.next()%16],r.uni(-10.f,100.f)};
                        if(r.coin(3))pp=CVector{r.uni(-3300.f,3300.f),r.uni(-3300.f,3300.f),0.f};
                        if(r.coin(40))pp.x=std::numeric_limits<float>::quiet_NaN();
                        At<CVector>(pd,4)=pp; FillMat(r,matP.data(),pp); At<void*>(pd,0x14)=useMat ? matP.data() : nullptr;
                        AtA<uint16>(ADDR_SCANCODE)=uint16(r.pick({0,1,100,0xFFFE,0xFFFF}));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        out["scan"]=AtA<uint16>(ADDR_SCANCODE);
                    }
                    else if(nm=="RemoveBodyPart") {
                        cfg.shootLimbs=r.coin(2);
                        for(auto& b:boneFrames){ At<void*>(b.data(),0x10)=r.coin(3) ? nullptr : reinterpret_cast<void*>(0x1000+r.next()%0x100); }
                        reinterpret_cast<void(__thiscall*)(void*,int32,int8)>(a)(pd,r.pick({0,1,2,3,5,8,18}),int8(r.next()));
                    }
                    else if(nm=="SpawnFlyingComponent") {
                        reinterpret_cast<void(__thiscall*)(void*,int32,int8)>(a)(pd,int32(r.next()),int8(r.next()));
                    }
                    else if(nm=="PedCanPickUpPickUp") {
                        auto* pp=pedP.data(); At<void*>(pp,0x47C)=intelP.data(); At<void*>(intelP.data(),0)=pp;
                        AtA<void*>(ADDR_PLAYERS)=pp;
                        cfg.findTask=uintptr_t(Tk(3)); cfg.findTaskMask=r.next();
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(pd));
                        out["pedP"]=Snap(pedP.data(),0x500,detail);
                    }
                    else if(nm=="SetPedAtomicVisibilityCB") {
                        for(size_t k=0;k<0x10;++k)rwObj[k]=uint8(r.next()); void* data=r.coin(2) ? nullptr : strA.data();
                        auto* rv=reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(rwObj.data(),data); ret=Name(rv);
                        out["rwObj"]=ReversingProbe::Bytes(rwObj.data(),0x10);
                    }
                    else if(nm=="SetupLighting"||nm=="RemoveLighting") {
                        cfg.lightRet=r.coin(2); At<uint32>(pd,0x40)=r.next();
                        if(nm=="SetupLighting")ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(pd)&true); else reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(pd,r.coin(2));
                    }
                    else if(nm=="DeleteRwObject") {
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                    }
                    else if(nm=="SetModelIndex") {
                        const int32 mIdx=r.pick({0,1,7,100,150,299,19980,19981});
                        At<void*>(pd,0x18)=nullptr; At<void*>(pd,0x500)=nullptr;
                        std::memset(miPed.data(),0xCD,miPed.size()); At<void*>(miPed.data(),0)=miVt.data();
                        At<uint32>(miPed.data(),0x24)=uint32(r.pick({0,1,2,3,10,20})); At<uint32>(miPed.data(),0x2C)=uint32(r.pick({0,1,5,10,20,42}));
                        At<void*>(miPed.data(),0x34)=r.coin(3) ? nullptr : colModel.data();
                        setMi(mIdx,miPed.data()); cfg.groupMask=r.next();
                        const uint32 off=AtA<uint32>(ADDR_CLUMPOFFSET); if(off<0x3F0)At<void*>(Clump(0),off)=clumpData.data();
                        At<uint32>(pd,0x46C)=r.next(); At<uint32>(pd,0x528)=r.next(); At<uint32>(pd,0x530)=uint32(r.pick({0,1,50,58,5}));
                        reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(pd,uint32(mIdx));
                        out["clumpData"]=Snap(clumpData.data(),clumpData.size(),detail); out["miPed"]=Snap(miPed.data(),miPed.size(),detail);
                    }
                    else if(nm=="SetCharCreatedBy") {
                        At<uint32>(pd,0x598)=uint32(r.pick({0,1,3,4,5,6})); pd[0x484]=uint8(r.next());
                        reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(pd,uint32(r.pick({0,1,2,2,3})));
                    }
                    else if(nm=="UseGroundColModel") {
                        At<uint32>(pd,0x530)=uint32(r.pick({0,1,16,42,45,54,55,41,43,46,53,56,60,255}));
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(pd)&true);
                    }
                    else if(nm=="Render") {
                        // flags y visibilidad
                        At<uint32>(pd,0x1C)=r.next()&~0x80u; if(!r.coin(4))At<uint32>(pd,0x1C)|=0x80u;
                        At<uint32>(pd,0x474)=r.next(); if(r.coin(5))At<uint32>(pd,0x474)|=2u; else At<uint32>(pd,0x474)&=~2u;
                        At<uint32>(pd,0x46C)=r.next(); if(r.coin(2))At<uint32>(pd,0x46C)|=0x100u; else At<uint32>(pd,0x46C)&=~0x100u;
                        if(r.coin(2))At<uint32>(pd,0x46C)|=0x2000u; else At<uint32>(pd,0x46C)&=~0x2000u;
                        At<uint32>(pd,0x598)=uint32(r.pick({0,1,4,5,6}));
                        AtA<uint8>(ADDR_MIRRORREFL)=uint8(r.next()%2); AtA<uint32>(ADDR_MIRRORTYPE)=uint32(r.pick({0,1,2,3}));
                        cfg.vision=r.coin(2); cfg.swim=r.coin(2) ? uintptr_t(Tk(0)) : 0; cfg.hold=r.coin(3) ? uintptr_t(Tk(5)) : 0; cfg.jet=r.coin(3) ? uintptr_t(Tk(1)) : 0;
                        cfg.active704=r.coin(4) ? uintptr_t(Tk(3)) : 0; cfg.active824=r.coin(4) ? uintptr_t(Tk(3)) : 0;
                        makeVeh(veh0,matV.data()); At<void*>(pd,0x58C)=r.coin(6) ? nullptr : veh0; thunkRet[39]=uintptr_t(r.coin(2));
                        // cámara y posiciones: a veces justo en el límite del rango (distancia exactamente igual)
                        const float lod=r.pickF({1.f,0.5f,2.f,1.5f,0.f}); const CVector cp=r.vec(40.f);
                        AtA<uintptr_t>(ADDR_CAMMAT)=0; At<CVector>(reinterpret_cast<void*>(ADDR_CAMPOS),0)=cp; AtA<float>(ADDR_CAMLOD)=lod;
                        CVector pp=cp+CVector{r.uni(-50.f,50.f),r.uni(-50.f,50.f),r.uni(-20.f,20.f)};
                        if(r.coin(4)){ const float range=(At<uint32>(veh0,0x590)==5 ? 40.f : 25.f)*lod; pp=cp+CVector{range,0.f,0.f}; if(r.coin(2))pp.x=cp.x-range; }
                        At<CVector>(pd,4)=pp; At<void*>(pd,0x14)=nullptr;
                        At<void*>(pd,0x4F4)=r.coin(2) ? Clump(1) : nullptr; At<void*>(pd,0x4FC)=r.coin(3) ? Clump(2) : nullptr;
                        At<void*>(pd,0x480)=r.coin(3) ? nullptr : playerData.data(); At<uint32>(playerData.data(),0x34)=r.next();
                        At<int16>(pd,0x504)=int16(r.pick({0,1,-1,100,-200})); At<int16>(pd,0x508)=int16(r.pick({0,0,1,-1,50}));
                        At<uint32>(pd,0x40)=r.next(); if(r.coin(2))At<uint32>(pd,0x40)|=0x100u; else At<uint32>(pd,0x40)&=~0x100u;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        out["frame2"]=Snap(Frame(2),0x60,detail); out["alpha"]=AtA<uint32>(ADDR_ENGINE);
                    }
                    else if(nm=="PreRender"||nm=="PreRenderAfterTest") {
                        // fixture de PreRenderAfterTest (las ramas se activan por separado)
                        At<uint32>(pd,0x1C)=r.next(); if(r.coin(5))At<uint32>(pd,0x1C)&=~0x80u; else At<uint32>(pd,0x1C)|=0x80u;
                        At<uint32>(pd,0x46C)=r.next(); At<uint32>(pd,0x474)=r.next(); At<uint32>(pd,0x478)=r.next();
                        if(r.coin(4))At<uint32>(pd,0x46C)|=0x100u; else At<uint32>(pd,0x46C)&=~0x100u;
                        if(r.coin(3))At<uint32>(pd,0x46C)|=0x8000u; else At<uint32>(pd,0x46C)&=~0x8000u;
                        if(r.coin(3))At<uint32>(pd,0x46C)|=0x4000000u; else At<uint32>(pd,0x46C)&=~0x4000000u;
                        if(r.coin(3))At<uint32>(pd,0x478)|=0x20u; else At<uint32>(pd,0x478)&=~0x20u;
                        At<uint32>(pd,0x40)=r.next(); if(r.coin(4))At<uint32>(pd,0x40)|=0x100u; else At<uint32>(pd,0x40)&=~0x100u;
                        At<uint32>(pd,0x528)=r.next(); At<float>(pd,0x518)=r.pickF({0.f,0.3f,-0.2f,1.f});
                        At<uint32>(pd,0x598)=uint32(r.pick({0,1,4,5,6})); At<uint32>(pd,0x530)=uint32(r.pick({0,1,50,55,54,42,16,17,58,2}));
                        At<void*>(pd,0x134)=nullptr; pd[0x754]=uint8(r.pick({0,2,2,5})); At<float>(pd,0x758)=r.pickF({0.f,1.f,2.5f,-1.f,0.1f});
                        At<int16>(pd,0x22)=int16(r.pick({0,1,7,100,150}));
                        // inteligencia
                        cfg.swim=r.coin(4) ? uintptr_t(Tk(0)) : 0; cfg.jet=r.coin(4) ? uintptr_t(Tk(1)) : 0; cfg.air=r.coin(4) ? uintptr_t(Tk(2)) : 0;
                        cfg.simplest=0; cfg.active704=r.coin(3) ? uintptr_t(Tk(3)) : 0; cfg.active703=r.coin(3) ? uintptr_t(Tk(3)) : 0;
                        cfg.active701=r.coin(3) ? uintptr_t(Tk(4)) : 0; makeVeh(veh0,matV.data()); makeVeh(veh2.data(),matV2.data());
                        At<void*>(Tk(4),0xC)=r.coin(2) ? veh2.data() : (r.coin(2) ? nullptr : veh0);
                        At<void*>(pd,0x58C)=r.coin(5) ? nullptr : veh0; thunkRet[39]=uintptr_t(r.coin(2)); At<uint32>(veh0,0x590)=uint32(r.pick({0,9,1})); At<uint32>(veh0,0x594)=uint32(r.pick({0,2,9,10,5}));
                        // jugador, arma, ropa
                        At<void*>(pd,0x480)=r.coin(3) ? nullptr : playerData.data(); At<void*>(playerData.data(),4)=clothes.data(); At<float>(playerData.data(),0x64)=r.uni(0.f,5.f);
                        At<uint8>(playerData.data(),0x8C)=uint8(r.pick({0,1,50,100,127,128,200,255})); At<uint8>(playerData.data(),0x8E)=uint8(r.pick({0,10,29,30,31,100}));
                        std::memset(clothes.data(),0,clothes.size()); static const char* const keys[]{"vest","torso","jacket","tshirt"}; At<uint32>(clothes.data(),0)=CKeyGen::GetUppercaseKey(keys[r.next()%4]);
                        At<void*>(pd,0x4F4)=r.coin(2) ? Clump(1) : nullptr; pd[0x718]=uint8(r.pick({0,1,2,255})); for(int k=0;k<13;++k)At<int32>(pd+0x5A0+k*0x1C,0)=r.pick({38,0,1,38});
                        cfg.frameRet=r.coin(3) ? 0 : uintptr_t(Frame(1)); AtA<uint8>(ADDR_SKIPFRAME)=uint8(r.coin(3));
                        // sombras y ciclo horario
                        AtA<int16>(ADDR_SHADOWSTR)=int16(r.pick({0,0,10,255,-1})); AtA<uint32>(ADDR_TCSTORED)=uint32(r.pick({0,1,5,15}));
                        for(size_t k=0;k<0x40;k+=4){ for(uintptr_t base:{0xB79E50u,0xB79E90u,0xB79ED0u,0xB79F10u,0xB79F50u,0xB79F90u})AtA<float>(base+k)=r.uni(-2.f,2.f); }
                        cfg.fxQuality=r.pick({0,1,2,3,3,2}); cfg.setsShadowData=r.coin(3);
                        const CVector cp=r.vec(40.f); AtA<uintptr_t>(ADDR_CAMMAT)=r.coin(3) ? uintptr_t(matCam.data()) : 0; FillMat(r,matCam.data(),cp);
                        At<CVector>(reinterpret_cast<void*>(ADDR_CAMPOS),0)=cp; At<float>(reinterpret_cast<void*>(ADDR_CAMLOD),0)=1.f;
                        const CVector pp=cp+CVector{r.uni(-30.f,30.f),r.uni(-30.f,30.f),r.uni(-5.f,5.f)}; At<CVector>(pd,4)=pp; FillMat(r,matP.data(),pp); At<void*>(pd,0x14)=matP.data(); /* el EXE desreferencia la matriz sin comprobar (sangre: GetUp) */
                        cfg.bonePos[0]=cp+CVector{r.pickF({0.f,14.f,15.f,16.f,r.uni(-20.f,20.f)}),r.uni(-3.f,3.f),r.uni(-2.f,2.f)}; cfg.bonePos[1]=pp+CVector{0.f,0.f,r.uni(-1.5f,1.5f)};
                        // clima, viento y lluvia
                        AtA<float>(ADDR_RAIN)=r.pickF({0.f,0.3f,0.5f,1.f}); AtA<float>(ADDR_QUAKE)=r.pickF({0.f,0.f,0.5f,2.f,-1.f}); AtA<float>(ADDR_CAMUP)=r.pickF({0.f,15.f,16.f,50.f});
                        AtA<uint32>(ADDR_AREA)=uint32(r.pick({0,0,1,5})); cfg.windRet=r.coin(2); cfg.windValue=r.pickF({1.f,0.5f,2.f,3.f}); cfg.playerNoRain=r.coin(4); cfg.camNoRain=r.coin(4);
                        cfg.headAbove=r.coin(2); cfg.assocRet=r.coin(3) ? uintptr_t(Tk(3)) : 0;
                        { const CVector sv=r.coin(3) ? CVector{0.f,0.f,0.f} : r.vec(0.2f); At<CVector>(speedVec.data(),0)=sv; At<CVector>(veh0,0x44)=r.vec(0.5f); }
                        if(r.coin(2))At<float>(matP.data(),0x38)=r.pickF({100.f,899.f,900.f,901.f,-3.f});
                        // col model de impactos y salpicaduras
                        { const uint32 nsp=r.next()%8; At<uint16>(colData.data(),0)=uint16(r.coin(8) ? 0xFFFF : nsp); At<void*>(colData.data(),8)=spheres.data(); At<void*>(colModel.data(),0x2C)=colData.data();
                          for(uint32 k=0;k<8;++k){ auto* sp=spheres.data()+k*0x14; At<CVector>(sp,0)=r.vec(1.f); At<float>(sp,0xC)=r.uni(0.05f,0.4f); sp[0x10]=uint8(r.next()); sp[0x11]=uint8(r.pick({0,4,5,6,7,9,10,200})); sp[0x12]=uint8(r.next()); sp[0x13]=uint8(r.next()); } }
                        std::memset(miPed.data(),0xCD,miPed.size()); At<void*>(miPed.data(),0)=miVt.data(); At<void*>(miPed.data(),0x34)=colModel.data();
                        setMi(At<int16>(pd,0x22)==0 ? 1 : At<int16>(pd,0x22),miPed.data()); if(At<int16>(pd,0x22)==0)At<int16>(pd,0x22)=1;
                        // g_fx.m_Splash / m_WaterSplash apuntan a sistemas falsos
                        AtA<uintptr_t>(ADDR_FXSPLASH)=uintptr_t(fxBufs[1].data()); AtA<uintptr_t>(ADDR_FXSPLASH+8)=uintptr_t(fxBufs[2].data());
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        out["veh"]=Snap(veh0,0x600,detail); out["frame1"]=Snap(Frame(1),0x60,detail); out["hierMats"]=Snap(hierMats.data(),hierMats.size(),detail);
                        out["globals"]=json::array({AtA<float>(ADDR_QUAKE),AtA<uint32>(ADDR_SCANCODE)});
                    }
                    else if(nm=="ProcessBuoyancy") {
                        At<uint32>(pd,0x46C)=r.next(); if(r.coin(8))At<uint32>(pd,0x46C)|=0x100u; else At<uint32>(pd,0x46C)&=~0x100u;
                        if(r.coin(2))At<uint32>(pd,0x46C)|=1u; else At<uint32>(pd,0x46C)&=~1u;
                        At<uint32>(pd,0x470)=r.next(); At<uint32>(pd,0x40)=r.next(); At<uint32>(pd,0x530)=uint32(r.pick({0,1,2,5,50,54,55,16}));
                        At<uint32>(pd,0x598)=uint32(r.pick({0,1,4,5,6})); At<float>(pd,0x8C)=r.pickF({70.f,50.f,100.f,1.f,0.f});
                        makeVeh(veh2.data(),matV2.data()); At<uint32>(veh2.data(),0x590)=uint32(r.pick({5,5,0,9})); veh2[0x36]=uint8((veh2[0x36]&0xF8)|uint8(r.pick({2,2,3,1})));
                        if(r.coin(2))At<uint32>(veh2.data(),0x40)|=0x20000000u; else At<uint32>(veh2.data(),0x40)&=~0x20000000u;
                        At<void*>(pd,0x584)=r.coin(4) ? nullptr : veh2.data();
                        At<void*>(pd,0x480)=r.coin(2) ? nullptr : playerData.data();
                        tskTypeRet(); cfg.swim=r.coin(3) ? uintptr_t(Tk(0)) : 0; cfg.simplest=cfg.swim ? cfg.swim : (r.coin(3) ? 0 : uintptr_t(Tk(3)));
                        At<float>(Tk(0),0x54)=r.uni(0.f,5.f); At<float>(Tk(3),0x54)=r.uni(0.f,5.f);
                        cfg.buoyRet=!r.coin(4); cfg.buoyTurn=r.vec(1.f); cfg.buoyForce=CVector{r.uni(-1.f,1.f),r.uni(-1.f,1.f),r.uni(-0.5f,3.f)};
                        const CVector pp=r.vec(60.f); At<CVector>(pd,4)=pp; FillMat(r,matP.data(),pp); At<void*>(pd,0x14)=r.coin(2) ? matP.data() : nullptr;
                        AtA<float>(ADDR_BUOY+0x64)=pp.z+r.pickF({-2.f,-0.6f,-0.5f,0.f,0.5f,0.6f,1.f,3.f});
                        At<float>(pd,0x4C)=r.pickF({-0.5f,-0.01f,-0.011f,-0.009f,0.f,0.3f,-0.1f});
                        cfg.vlineRet=!r.coin(3); cfg.colEntity=uintptr_t(veh2.data()); cfg.waterRet=r.coin(2); cfg.waterZ=pp.z+r.uni(-1.f,1.f); cfg.headZ=pp.z+r.uni(-1.f,1.f);
                        if(r.coin(5))cfg.colEntity=uintptr_t(veh0);
                        makeVeh(veh0,matV.data()); At<uint32>(veh0,0x590)=uint32(r.pick({5,0})); veh0[0x36]=uint8((veh0[0x36]&0xF8)|2);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(pd);
                        out["veh2"]=Snap(veh2.data(),0x600,detail); out["tasks"]=Snap(Tk(0),0x80,detail); out["task3"]=Snap(Tk(3),0x80,detail);
                    }
                    // comparación común
                    out["ped"]=Snap(pd,ped.size(),detail); out["intel"]=Snap(intel.data(),0x120,detail);
                    out["ret"]=ret; out["trace"]=trace;
                    restoreAll();
                    return out;
                };
                const auto original=run(false,false), candidate=run(true,false);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else {
                    const auto originalD=run(false,true), candidateD=run(true,true);
                    json diff=json::array(); for(auto& [k,v]:originalD.items())if(candidateD[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? originalD:json(nullptr)},{"candidate",failures<=2 ? candidateD:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateRestored()}});
}
}
