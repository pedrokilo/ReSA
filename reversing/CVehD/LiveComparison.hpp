#pragma once
// Banco dentro del motor: CVehicle, grupo D (dibujo, luces y partículas; 26 funciones): PreRender, PreRenderDriverAndPassengers,
// Render, RenderDriverAndPassengers, ResetAfterRender, SetupRender, SetupLighting, RemoveLighting, SetCollisionLighting,
// CalculateLightingFromCollision, UpdateLightingFromStoredPolys, DoHeadLightBeam, DoHeadLightReflection (+Single/Twin),
// AddDamagedVehicleParticles, AddExhaustParticles, AddWaterSplashParticles, AddWheelDirtAndWater, DoBoatSplashes,
// GetVehicleLightsStatus, DoPlaneGunFireFX, ProcessDrivingAnims, ProcessOpenDoor, ProcessWheel y ProcessWheelRotation (las
// virtuales por la vtable del EXE 0x871E80, el resto por su dirección del EXE). Original (hooks de estas funciones
// desactivados) frente a candidato con fixtures deterministas. Nunca se toca un vehículo real ni un pool real: el
// CVehicle es un búfer FALSO (patrón 0xCD) con vptr a una vtable falsa con trazas (huecos 27 OpenDoor, 30
// GetDooorAngleOpenRatio y 38 IsDoorMissing con su firma real) y todo lo que el EXE desreferencia (matrices, clump,
// handling, model info y su estructura de dummies, col model con triángulos, peatones, sistemas Fx, texturas...) son
// búferes propios. Los model infos falsos se colocan temporalmente en ms_modelInfoPtrs. Se simulan y trazan (dirección
// del EXE + símbolo y cuerpo de la DLL): rand (0x821B1E y CGeneral::GetRandomNumber), RW (estados de render por los
// punteros del motor, Im3D), CVehicleModelInfo (dirt, materiales, color, luces, env map), matrículas, CEntity::Render/
// ProcessLightsForEntity/IsVisible, luz de colisión, streaming/TXD, Fx (gestor, sistemas, partículas, ruedas, remolino),
// sombras, luces puntuales, agua, clima/cullzones, cámara (primera persona), animaciones (asociaciones, mezcla),
// estadísticas, armas (casquillos), CBike::CalculateLeanMatrix, ApplyMoveForce/ApplyTurnForce, GetPlaneNumGuns y
// operator new[] (solo durante DoPlaneGunFireFX). Globales tocadas (reloj, clima, cámara, statics de funciones,
// buffers temporales de Im3D, punteros de Fx, pool de TXD, streaming...) se guardan y restauran en cada ejecución.
#include <fstream>
#include <set>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Vehicle.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Bike.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Models/BaseModelInfo.h"
#include "Models/ModelInfo.h"
#include "Models/VehicleModelInfo.h"
#include "Fx/Fx.h"
#include "Fx/FxSystem.h"
#include "Fx/FxManager.h"
#include "Shadows.h"
#include "PointLights.h"
#include "WaterLevel.h"
#include "CullZones.h"
#include "Weapon.h"
#include "Stats.h"
#include "TxdStore.h"
#include "Streaming.h"
#include "Camera.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "VehicleAnimGroupData.h"
#include "Renderer.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace CVehDLive {
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
constexpr uintptr_t VEH_VT=0x871E80, ADDR_MI_PTRS=0xA9B0C8, ADDR_TIMESTEP=0xB7CB5C, ADDR_FRAMECTR=0xB7CB4C, ADDR_CAMPOS=0xB6F02C, ADDR_CAMMAT=0xB6F03C,
    ADDR_ENGINE=0xC97B24, ADDR_CLOCK=0xB70152, ADDR_WEATHER=0xC81300, ADDR_FOG=0xC81300, ADDR_WET=0xC81308, ADDR_RAIN=0xC81324, ADDR_GANGAREA=0xC0BC5C,
    ADDR_DNBAL=0x8D12C0, ADDR_PLANEEJECT=0xC1CC19, ADDR_EXHGUARD=0xC1CDB8, ADDR_EXHFACTOR=0xC1CDB4, ADDR_WHEELSTATICS=0xC1CDAC, ADDR_WHEELFRIC=0xC2B9CC,
    ADDR_GFX=0xA9AE00, ADDR_FXMAN=0xA9AE80, ADDR_TMPVERTS=0xC4D958, ADDR_TMPIDX=0xC4B950, ADDR_SHTEX=0xC403F8, ADDR_TXDPOOL=0xC8800C, ADDR_REMAPTEX=0xB4E47C,
    ADDR_STREAMINFO=0x946750, ADDR_ACTIVECAM=0xB6F081, ADDR_CAMMODE=0xB6F1A8, ADDR_CAMDIR=0xB6F1C4, EXE_NEWARR=0x821195;
constexpr size_t VEH_SZ=0xA40, PED_SZ=0x540, NPEDS=10, NASSOC=16, NFX=4, NSLOT=66;

alignas(16) inline std::array<uint8,VEH_SZ> veh{};
alignas(16) inline std::array<std::array<uint8,PED_SZ>,NPEDS> peds{};
alignas(16) inline std::array<uint8,0x100> handling{}, vstruct{};
alignas(16) inline std::array<uint8,0x80> mi{};
alignas(16) inline std::array<uint8,0x40> colModel{}, colData{};
alignas(16) inline std::array<uint8,0x80> verts{}, tris{};
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<uint8,0x100> frame{};
alignas(16) inline std::array<uint8,0x48> matV{}, matL{}, matCam{};
alignas(16) inline std::array<std::array<uint8,0x60>,NFX> fxs{};
alignas(16) inline std::array<std::array<uint8,0x40>,NASSOC> assocs{};
alignas(16) inline std::array<uint8,0x40> colPoint{}, weapon{};
alignas(16) inline std::array<uint8,0x10> texA{}, texB{}, texC{};
alignas(16) inline std::array<uint8,0x60> txdObjs{};
alignas(16) inline std::array<uint8,0x10> txdFlags{};
alignas(16) inline std::array<uint8,0x40> txdPool{};
alignas(16) inline std::array<void*,8> gunParts{}, newArr{};
alignas(16) inline std::array<uint8,0x40> wheelState{}, vecs{};
inline std::array<void*,NSLOT> vehVt{}, pedVt{};
inline std::array<uintptr_t,NSLOT> thunkRet{};
inline uint8* Fx(int k){ return fxs[size_t(k)].data(); }
inline uint8* Ped(int k){ return peds[size_t(k)].data(); }

// --- nombres estables para punteros ---
struct Named { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Named> named;
inline void Reg(const void* p,size_t n,std::string nm,size_t stride=0){ named.push_back({uintptr_t(p),n,std::move(nm),stride}); }
inline const Named* FindNamed(uintptr_t v){ for(auto& b:named)if(v>=b.p&&v<b.p+b.n)return &b; return nullptr; }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==VEH_VT)return "vt_veh";
    if(v==ADDR_GFX)return "g_fx";
    if(v==ADDR_FXMAN)return "g_fxMan";
    if(v==ADDR_TMPVERTS)return "tempVerts";
    if(v==ADDR_TMPIDX+8)return "tempIdx";
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
    float lightCol{}, waterZ{}, stat{}; bool lightRet{}, im3dRet{}, visible{}, lookFwd{}, camNoRain{}, waterRet{}, isPlayer{}, blendRet{};
    uintptr_t texRet{}, fxRet{}; uint32 assocSeed{}; int32 ids[32]{}; int nIds{}; uint32 doorMissing{}; float ratio[8]{}; int numGuns{};
};
inline Cfg cfg{};
inline uint32 rngState{};
inline int __cdecl FakeRand(){ if(!Main())return 0x1234; T({"rand"}); rngState=rngState*1103515245u+12345u; return int((rngState>>16)&0x7FFF); }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }

// vtables falsas con trazas: la entrada N devuelve thunkRet[N]
template<int N> inline uintptr_t __fastcall VThunk(void* self,void*){ T({"vcall",N,Name(self)}); return thunkRet[size_t(N)]; }
template<size_t... I> inline void FillVt(std::array<void*,NSLOT>& a,std::index_sequence<I...>){ ((a[I]=reinterpret_cast<void*>(&VThunk<int(I)>)),...); }
// huecos del vehículo con su firma real (el que llama limpia los argumentos con ret N)
inline bool __fastcall VIsDoorMissing(void* self,void*,uint32 door){ T({"vcall_IsDoorMissing",Name(self),door&0xFF}); return (cfg.doorMissing>>(door&31))&1; }
inline void __fastcall VOpenDoor(void* self,void*,void* ped,int32 comp,uint32 door,float ratio,uint32 flag){ T({"vcall_OpenDoor",Name(self),Name(ped),comp,door&0xFF,F(ratio),flag&0xFF}); }
inline float __fastcall VDoorRatio(void* self,void*,uint32 door){ T({"vcall_GetDooorAngleOpenRatio",Name(self),door&0xFF}); return cfg.ratio[door&7]; }

// --- dependencias simuladas ---
inline RwBool __cdecl RSSet(int32 st,void* v){ T({"render_state_set",st,uintptr_t(v)}); return 1; }
inline RwBool __cdecl RSGet(int32 st,void* v){ T({"render_state_get",st}); *reinterpret_cast<uint32*>(v)=0; return 1; }
// luz / render
inline void __cdecl FakeActivateDir(){ T({"activate_directional"}); }
inline void __cdecl FakeDeactivateDir(){ T({"deactivate_directional"}); }
inline void __cdecl FakeAmbient(){ T({"set_ambient_colours"}); }
inline bool __cdecl FakeSetupLightingEntity(void* e){ T({"setup_lighting_entity",Name(e)}); return cfg.lightRet; }
inline void __cdecl FakeRemoveLights(){ T({"remove_lights_affecting_object"}); }
inline void __fastcall FakeEntityRender(void* e,void*){ T({"entity_render",Name(e)}); }
inline void __fastcall FakeProcessLights(void* e,void*){ T({"process_lights_for_entity",Name(e)}); }
inline float __fastcall FakeLightingFromCol(void* e,void*,uint32 b){ T({"lighting_from_col",Name(e),b&0xFF}); return cfg.lightCol; }
inline void __fastcall FakeSetEnvMap(void* m,void*,float c){ T({"set_env_map_coeff",Name(m),F(c)}); }
inline void __fastcall FakePedPreRender(void* p,void*){ T({"ped_pre_render_after_test",Name(p)}); }
inline void __cdecl FakeSetDirt(void* m,int32 level){ T({"set_dirt_textures",Name(m),level}); }
inline void __cdecl FakeResetEditable(void* c){ T({"reset_editable_materials",Name(c)}); }
inline void __fastcall FakePlateAfter(void* v,void*,void* m){ T({"custom_car_plate_after",Name(v),Name(m)}); }
inline void __fastcall FakePlateBefore(void* v,void*,const void* m){ T({"custom_car_plate_before",Name(v),Name(m)}); }
inline void __cdecl FakeRequestModel(int32 id,int32 fl){ T({"request_model",id,fl}); }
inline void __cdecl FakeTxdAddRef(int32 i){ T({"txd_add_ref",i}); }
inline void __cdecl FakeTxdRemoveRef(int32 i){ T({"txd_remove_ref",i}); }
inline void* __cdecl FakeFirstTexture(void* d){ T({"first_texture",Name(d)}); return reinterpret_cast<void*>(cfg.texRet); }
inline void __fastcall FakeSetColour(void* m,void*,uint32 a,uint32 b,uint32 c,uint32 d){ T({"set_vehicle_colour",Name(m),a&0xFF,b&0xFF,c&0xFF,d&0xFF}); }
inline void __cdecl FakeSetupLightFlags(void* v){ T({"setup_light_flags",Name(v)}); }
inline void __cdecl FakeSetEditable(void* c){ T({"set_editable_materials",Name(c)}); }
// Im3D
inline void* __cdecl FakeIm3DTransform(void* v,uint32 n,void* m,uint32 flags){ T({"im3d_transform",uintptr_t(v)==ADDR_TMPVERTS,n,Name(m),flags,Fnv(v,0x24*size_t(n))}); return cfg.im3dRet ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(int32 t,void* idx,int32 n){ T({"im3d_render",t,uintptr_t(idx)==ADDR_TMPIDX+8,n,Fnv(idx,2*size_t(n))}); return 1; }
inline RwBool __cdecl FakeIm3DEnd(){ T({"im3d_end"}); return 1; }
// sombras / luces puntuales
inline void __cdecl FakeStoreCarLightShadow(void* v,int32 id,void* tex,const CVector* p,float a,float b,float c,float d,uint32 r,uint32 g,uint32 bl,float mx){
    T({"store_car_light_shadow",Name(v),Name(reinterpret_cast<void*>(intptr_t(id)-2)),Name(tex),V(p),F(a),F(b),F(c),F(d),r&0xFF,g&0xFF,bl&0xFF,F(mx)}); }
inline void __cdecl FakeAddLight(uint32 t,float px,float py,float pz,float dx,float dy,float dz,float rad,float r,float g,float b,uint32 fog,uint32 extra,void* ent){
    T({"add_light",t&0xFF,F(px),F(py),F(pz),F(dx),F(dy),F(dz),F(rad),F(r),F(g),F(b),fog&0xFF,extra&0xFF,Name(ent)}); }
// Fx
inline void* __fastcall FakeCreateFx(void* mgr,void*,const char* name,const CVector* p,void* m,uint32 b){
    T({"create_fx_system",uintptr_t(mgr)==ADDR_FXMAN,name ? json(Fnv(name,strlen(name))) : json(nullptr),V(p),Name(m),b&0xFF}); return reinterpret_cast<void*>(cfg.fxRet); }
inline void __fastcall FakeFxPlay(void* fx,void*){ T({"fx_play",Name(fx)}); }
inline void __fastcall FakeFxKill(void* fx,void*){ T({"fx_kill",Name(fx)}); }
inline void __fastcall FakeFxConstTime(void* fx,void*,uint32 on,float t){ T({"fx_const_time",Name(fx),on&0xFF,F(t)}); }
inline void __fastcall FakeFxVelAdd(void* fx,void*,const CVector* v){ T({"fx_vel_add",Name(fx),V(v)}); }
inline void __fastcall FakeFxOffsetPos(void* fx,void*,const CVector* v){ T({"fx_offset_pos",Name(fx),V(v)}); }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector* pos,const CVector* vel,float t,const float* mult,float rz,float lm,float lml,uint32 local){
    T({"particle",Name(self),V(pos),V(vel),F(t),Floats(mult,7),F(rz),F(lm),F(lml),local&0xFF}); }
inline void __fastcall FakeWheelSpray(void* fx,void*,void* v,CVector p,uint32 spin,uint32 water,float lm){ T({"wheel_spray",Name(v),V(&p),spin&0xFF,water&0xFF,F(lm)}); }
inline void __fastcall FakeWheelGrass(void* fx,void*,void* v,CVector p,uint32 spin,float lm){ T({"wheel_grass",Name(v),V(&p),spin&0xFF,F(lm)}); }
inline void __fastcall FakeWheelGravel(void* fx,void*,void* v,CVector p,uint32 spin,float lm){ T({"wheel_gravel",Name(v),V(&p),spin&0xFF,F(lm)}); }
inline void __fastcall FakeWheelMud(void* fx,void*,void* v,CVector p,uint32 spin,float lm){ T({"wheel_mud",Name(v),V(&p),spin&0xFF,F(lm)}); }
inline void __fastcall FakeWheelSand(void* fx,void*,void* v,CVector p,uint32 spin,float lm){ T({"wheel_sand",Name(v),V(&p),spin&0xFF,F(lm)}); }
inline void __fastcall FakeWheelDust(void* fx,void*,void* v,CVector p,uint32 spin,float lm){ T({"wheel_dust",Name(v),V(&p),spin&0xFF,F(lm)}); }
// agua, clima, cámara, entidad
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float* out,uint32 touching,void* normals){ T({"water_level",F(x),F(y),F(z),touching&0xFF,Name(normals)}); if(cfg.waterRet)*out=cfg.waterZ; return cfg.waterRet; }
inline bool __cdecl FakeCamNoRain(){ T({"cam_no_rain"}); return cfg.camNoRain; }
inline bool __fastcall FakeLookingFwd(void* cam,void*){ T({"looking_forward_first_person"}); return cfg.lookFwd; }
inline bool __fastcall FakeIsVisible(void* e,void*){ T({"is_visible",Name(e)}); return cfg.visible; }
inline void __fastcall FakeLean(void* b,void*){ T({"calculate_lean_matrix",Name(b)}); }
inline void __fastcall FakeGunshell(void* w,void*,void* creator,CVector* pos,const float* dir,float size){ T({"add_gunshell",Name(w),Name(creator),V(pos),F(dir[0]),F(dir[1]),F(size)}); }
inline int32 __fastcall FakeNumGunsImpl(void* v,void*){ T({"get_plane_num_guns",Name(v)}); return cfg.numGuns; }
// El EXE conserva EDX (contador del bucle de DoPlaneGunFireFX) a traves de GetPlaneNumGuns: el stub debe preservarlo
__declspec(naked) inline int32 __fastcall FakeNumGuns(void*,void*){ __asm { push edx
        call FakeNumGunsImpl
        pop edx
        ret } }
inline void* __cdecl FakeNewArr(size_t n){ T({"new_array",uint32(n)}); return n<=sizeof(newArr) ? newArr.data() : nullptr; }
// animación, estadísticas, peatón
inline bool __fastcall FakeIsPlayer(void* p,void*){ T({"ped_is_player",Name(p)}); return cfg.isPlayer; }
inline void* __cdecl FakeGetAssoc(void* c,uint32 id){
    int slot=-1; for(int k=0;k<cfg.nIds;++k)if(cfg.ids[k]==int32(id))slot=k;
    if(slot<0){ if(cfg.nIds<32){ cfg.ids[cfg.nIds]=int32(id); slot=cfg.nIds++; } else slot=31; }
    const bool hit=(cfg.assocSeed>>(slot%32))&1; void* r=hit ? assocs[size_t(slot)%NASSOC].data() : nullptr;
    T({"get_association",Name(c),id,Name(r)}); return r; }
inline void* __cdecl FakeBlend(void* c,int32 group,int32 id,float t){ void* r=cfg.blendRet ? assocs[NASSOC-1].data() : nullptr; T({"blend_animation",Name(c),group,id,F(t),Name(r)}); return r; }
inline float __cdecl FakeStat(int32 s){ T({"get_stat",s}); return cfg.stat; }
// fuerzas
inline void __fastcall FakeApplyMoveForce(void* e,void*,float x,float y,float z){ T({"apply_move_force",Name(e),F(x),F(y),F(z)}); }
inline void __fastcall FakeApplyTurnForce(void* e,void*,float x,float y,float z,float px,float py,float pz){ T({"apply_turn_force",Name(e),F(x),F(y),F(z),F(px),F(py),F(pz)}); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=26;
inline const std::array<Fn,NFNS> fns{{
    {"CVehicle","PreRender",0,0x871E80,17,256},
    {"CVehicle","PreRenderDriverAndPassengers",0x6D3DB0,0,-1,128},
    {"CVehicle","Render",0,0x871E80,18,128},
    {"CVehicle","RenderDriverAndPassengers",0x6D3D60,0,-1,128},
    {"CVehicle","ResetAfterRender",0x6D0E20,0,-1,64},
    {"CVehicle","SetupRender",0x6D64F0,0,-1,384},
    {"CVehicle","SetupLighting",0,0x871E80,19,64},
    {"CVehicle","RemoveLighting",0,0x871E80,20,64},
    {"CVehicle","SetCollisionLighting",0x6D0CA0,0,-1,64},
    {"CVehicle","CalculateLightingFromCollision",0x6D0CF0,0,-1,256},
    {"CVehicle","UpdateLightingFromStoredPolys",0x6D0CC0,0,-1,64},
    {"CVehicle","DoHeadLightBeam",0x6E0E20,0,-1,512},
    {"CVehicle","DoHeadLightReflection",0x6E1720,0,-1,384},
    {"CVehicle","DoHeadLightReflectionSingle",0x6E1440,0,-1,256},
    {"CVehicle","DoHeadLightReflectionTwin",0x6E1600,0,-1,256},
    {"CVehicle","AddDamagedVehicleParticles",0x6D2A80,0,-1,384},
    {"CVehicle","AddExhaustParticles",0x6DE240,0,-1,768},
    {"CVehicle","AddWaterSplashParticles",0x6DDF60,0,-1,384},
    {"CVehicle","AddWheelDirtAndWater",0x6D2D50,0,-1,768},
    {"CVehicle","DoBoatSplashes",0x6DD130,0,-1,768},
    {"CVehicle","GetVehicleLightsStatus",0x6D55C0,0,-1,768},
    {"CVehicle","DoPlaneGunFireFX",0x6D4AD0,0,-1,256},
    {"CVehicle","ProcessDrivingAnims",0,0x871E80,46,768},
    {"CVehicle","ProcessOpenDoor",0,0x871E80,28,1024},
    {"CVehicle","ProcessWheel",0x6D6C00,0,-1,1024},
    {"CVehicle","ProcessWheelRotation",0x6D1230,0,-1,256},
}};

// matriz de CMatrix (0x48): ejes ortonormales aproximados y posición en 0x30
inline void FillMat(Rng& r,uint8* m,const CVector& pos){
    std::memset(m,0,0x48); const float a=r.uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
    At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,r.coin(5) ? -1.f : 1.f}; At<CVector>(m,0x30)=pos; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cvehd-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cvehd-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cvehd");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"cvehd-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // cuerpo de la DLL de CEntity::Render (el símbolo de una virtual es un thunk de llamada virtual)
    struct Base { const char* cls; const char* name; uintptr_t exe; uintptr_t stub; uintptr_t dll; };
    std::vector<Base> bases{ {"CEntity","Render",0x534310,Addr(&FakeEntityRender),0} };
    for(auto& bs:bases)if(!(bs.dll=DllBody(bs.cls,bs.name))){Write({{"event","error"},{"reason","base body not found"},{"function",std::string(bs.cls)+"::"+bs.name}});return;}

    // vtables falsas
    FillVt(vehVt,std::make_index_sequence<NSLOT>{}); FillVt(pedVt,std::make_index_sequence<NSLOT>{});
    vehVt[27]=reinterpret_cast<void*>(&VOpenDoor); vehVt[30]=reinterpret_cast<void*>(&VDoorRatio); vehVt[38]=reinterpret_cast<void*>(&VIsDoorMissing);

    // nombres
    named.clear();
    Reg(veh.data(),veh.size(),"veh"); for(size_t k=0;k<NPEDS;++k)Reg(Ped(int(k)),PED_SZ,"ped"+std::to_string(k));
    Reg(handling.data(),handling.size(),"handling"); Reg(vstruct.data(),vstruct.size(),"vstruct"); Reg(mi.data(),mi.size(),"mi");
    Reg(colModel.data(),colModel.size(),"colModel"); Reg(colData.data(),colData.size(),"colData"); Reg(verts.data(),verts.size(),"verts"); Reg(tris.data(),tris.size(),"tris");
    Reg(clump.data(),clump.size(),"clump"); Reg(frame.data(),frame.size(),"frame"); Reg(matV.data(),matV.size(),"matV"); Reg(matL.data(),matL.size(),"matL"); Reg(matCam.data(),matCam.size(),"matCam");
    for(int k=0;k<int(NFX);++k)Reg(Fx(k),0x60,"fx"+std::to_string(k));
    Reg(assocs.data(),sizeof(assocs),"assoc",0x40); Reg(colPoint.data(),colPoint.size(),"colPoint"); Reg(weapon.data(),weapon.size(),"weapon");
    Reg(texA.data(),texA.size(),"texA"); Reg(texB.data(),texB.size(),"texB"); Reg(texC.data(),texC.size(),"texC");
    Reg(txdObjs.data(),txdObjs.size(),"txdObjs",12); Reg(txdFlags.data(),txdFlags.size(),"txdFlags"); Reg(txdPool.data(),txdPool.size(),"txdPool");
    Reg(gunParts.data(),sizeof(gunParts),"gunParts"); Reg(newArr.data(),sizeof(newArr),"newArr"); Reg(wheelState.data(),wheelState.size(),"wheelState"); Reg(vecs.data(),vecs.size(),"vecs");
    Reg(vehVt.data(),sizeof(vehVt),"vehVt"); Reg(pedVt.data(),sizeof(pedVt),"pedVt");

    // estado global guardado y restaurado en cada ejecución
    auto** const miPtrs=reinterpret_cast<void**>(ADDR_MI_PTRS);
    std::vector<std::pair<int,void*>> miTouched;
    struct Saved { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Saved> regions;
    auto SnapG=[&](uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); };
    const uintptr_t rsSlot=AtA<uintptr_t>(ADDR_ENGINE)+0x20;
    SnapG(ADDR_TIMESTEP,4); SnapG(ADDR_FRAMECTR,4); SnapG(ADDR_CAMPOS,0x10); SnapG(ADDR_CAMMAT,4); SnapG(ADDR_CLOCK,2); SnapG(ADDR_WEATHER,0x28); SnapG(ADDR_GANGAREA,1);
    SnapG(ADDR_DNBAL,4); SnapG(ADDR_PLANEEJECT,1); SnapG(ADDR_EXHFACTOR,4); SnapG(ADDR_EXHGUARD,4); SnapG(ADDR_WHEELSTATICS,3); SnapG(ADDR_WHEELFRIC,4);
    SnapG(ADDR_GFX,0x40); SnapG(ADDR_TMPVERTS,0xB4); SnapG(ADDR_TMPIDX,0x20); SnapG(ADDR_SHTEX,8); SnapG(ADDR_TXDPOOL,4); SnapG(ADDR_REMAPTEX,4); SnapG(ADDR_STREAMINFO,0xA0);
    SnapG(ADDR_ACTIVECAM,1); SnapG(ADDR_CAMMODE,4); SnapG(ADDR_CAMDIR,4); SnapG(rsSlot,8);
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
        // luz y dibujo
        P(0x735C80,Addr(&ActivateDirectional),FakeActivateDir); P(0x735C70,Addr(&DeActivateDirectional),FakeDeactivateDir);
        P(0x735D30,Addr(static_cast<void(*)()>(&SetAmbientColours)),FakeAmbient);
        P(0x553E40,Addr(&CRenderer::SetupLightingForEntity),FakeSetupLightingEntity); P(0x6FFFE0,Addr(&CPointLights::RemoveLightsAffectingObject),FakeRemoveLights);
        P(0x6FC7A0,Addr(&CEntity::ProcessLightsForEntity),FakeProcessLights); P(0x5447B0,Addr(&CPhysical::GetLightingFromCol),FakeLightingFromCol);
        P(0x4C9800,Addr(&CVehicleModelInfo::SetEnvMapCoeff),FakeSetEnvMap); P(0x5E65A0,Addr(&CPed::PreRenderAfterTest),FakePedPreRender);
        P(0x5D5DB0,Addr(&CVehicleModelInfo::SetDirtTextures),FakeSetDirt); P(0x4C8460,Addr(&CVehicleModelInfo::ResetEditableMaterials),FakeResetEditable);
        P(0x6A2F30,Addr(&CAutomobile::CustomCarPlate_AfterRenderingStop),FakePlateAfter); P(0x6A2F00,Addr(&CAutomobile::CustomCarPlate_BeforeRenderingStart),FakePlateBefore);
        P(0x4087E0,Addr(&CStreaming::RequestModel),FakeRequestModel); P(0x731A00,Addr(&CTxdStore::AddRef),FakeTxdAddRef); P(0x731A30,Addr(&CTxdStore::RemoveRef),FakeTxdRemoveRef);
        P(0x734940,Addr(&GetFirstTexture),FakeFirstTexture); P(0x4C84B0,Addr(&CVehicleModelInfo::SetVehicleColour),FakeSetColour);
        P(0x4C8C90,Addr(&CVehicleModelInfo::SetupLightFlags),FakeSetupLightFlags); P(0x4C8430,Addr(&CVehicleModelInfo::SetEditableMaterials),FakeSetEditable);
        P(0x7EF450,Addr(&RwIm3DTransform),FakeIm3DTransform); P(0x7EF550,Addr(&RwIm3DRenderIndexedPrimitive),FakeIm3DRender); P(0x7EF520,Addr(&RwIm3DEnd),FakeIm3DEnd);
        P(0x70C500,Addr(&CShadows::StoreCarLightShadow),FakeStoreCarLightShadow);
        P(0x7000E0,Addr(&CPointLights::AddLight),FakeAddLight);
        // Fx
        P(0x4A9BE0,Addr(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx);
        P(0x4AA2F0,Addr(&FxSystem_c::Play),FakeFxPlay); P(0x4AA3F0,Addr(&FxSystem_c::Kill),FakeFxKill); P(0x4AA6C0,Addr(&FxSystem_c::SetConstTime),FakeFxConstTime);
        P(0x4AA730,Addr(&FxSystem_c::SetVelAdd),FakeFxVelAdd); P(0x4AA660,Addr(&FxSystem_c::SetOffsetPos),FakeFxOffsetPos);
        P(0x4AA440,Addr(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle);
        P(0x49FB30,Addr(&Fx_c::AddWheelSpray),FakeWheelSpray); P(0x49FF20,Addr(&Fx_c::AddWheelGrass),FakeWheelGrass); P(0x4A0170,Addr(&Fx_c::AddWheelGravel),FakeWheelGravel);
        P(0x4A03C0,Addr(&Fx_c::AddWheelMud),FakeWheelMud); P(0x4A0610,Addr(&Fx_c::AddWheelSand),FakeWheelSand); P(0x4A09C0,Addr(&Fx_c::AddWheelDust),FakeWheelDust);
        // agua, clima, cámara, entidad
        P(0x6EB690,Addr(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel);
        P(0x72DDB0,Addr(&CCullZones::CamNoRain),FakeCamNoRain); P(0x50AED0,Addr(&CCamera::GetLookingForwardFirstPerson),FakeLookingFwd);
        P(0x536BC0,Addr(&CEntity::IsVisible),FakeIsVisible); P(0x6B7150,Addr(&CBike::CalculateLeanMatrix),FakeLean);
        P(0x73A3E0,Addr(&CWeapon::AddGunshell),FakeGunshell); P(0x6D3F30,Addr(&CVehicle::GetPlaneNumGuns),FakeNumGuns);
        // animación, estadísticas, peatón
        P(0x5DF8F0,Addr(&CPed::IsPlayer),FakeIsPlayer);
        P(0x4D68B0,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FakeGetAssoc);
        P(0x4D4610,Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),FakeBlend);
        P(0x558E40,Addr(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),FakeStat);
        // fuerzas
        P(0x5429F0,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),FakeApplyMoveForce);
        P(0x542A50,Addr(&CPhysical::ApplyTurnForce),FakeApplyTurnForce);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate,bool detail)->json {
                    Rng r{}; r.st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xDDDDu; if(!r.st)r.st=1; trace=json::array(); rngState=uint32(i)*7919u+uint32(f);
                    restoreAll(); cfg=Cfg{};
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // globales
                    AtA<float>(ADDR_TIMESTEP)=r.uni(0.2f,3.f); AtA<uint32>(ADDR_FRAMECTR)=r.next();
                    AtA<uintptr_t>(rsSlot)=bit_cast<uintptr_t>(&RSSet); AtA<uintptr_t>(rsSlot+4)=bit_cast<uintptr_t>(&RSGet);
                    std::fill(thunkRet.begin(),thunkRet.end(),0);
                    // objetos falsos base
                    std::memset(handling.data(),0xCD,handling.size()); std::memset(vstruct.data(),0xCD,vstruct.size()); std::memset(mi.data(),0xCD,mi.size());
                    std::memset(colModel.data(),0xCD,colModel.size()); std::memset(colData.data(),0xCD,colData.size()); std::memset(verts.data(),0xCD,verts.size()); std::memset(tris.data(),0xCD,tris.size());
                    std::memset(clump.data(),0xCD,clump.size()); clump[0]=2; clump[1]=0; At<void*>(clump.data(),4)=frame.data(); std::memset(frame.data(),0xCD,frame.size());
                    for(auto* m:{matV.data(),matL.data(),matCam.data()})std::memset(m,0xCD,0x48);
                    for(size_t k=0;k<NFX;++k)std::memset(Fx(int(k)),0xCD,0x60);
                    for(auto& as:assocs){ std::memset(as.data(),0xCD,as.size()); At<float>(as.data(),0x18)=r.pickF({0.f,0.5f,0.99f,1.f,2.f}); At<float>(as.data(),0x1C)=r.pickF({0.f,-1.f,1.f,0.5f,-4.f}); }
                    std::memset(colPoint.data(),0xCD,colPoint.size()); std::memset(weapon.data(),0xCD,weapon.size());
                    std::memset(texA.data(),0xCD,texA.size()); std::memset(texB.data(),0xCD,texB.size()); std::memset(texC.data(),0xCD,texC.size());
                    std::memset(txdObjs.data(),0xCD,txdObjs.size()); std::memset(txdFlags.data(),0xCD,txdFlags.size()); std::memset(txdPool.data(),0xCD,txdPool.size());
                    gunParts.fill(reinterpret_cast<void*>(0xCDCDCDCDu)); newArr.fill(reinterpret_cast<void*>(0xCDCDCDCDu));
                    std::memset(wheelState.data(),0xCD,wheelState.size()); std::memset(vecs.data(),0xCD,vecs.size());
                    for(size_t k=0;k<NPEDS;++k){ std::memset(Ped(int(k)),0xCD,PED_SZ); At<void*>(Ped(int(k)),0)=pedVt.data(); }
                    const int16 modelPool[]{400,401,410,420,441,460,471,472,520,521,522,530,532,581,595};
                    // ---- vehículo base ----
                    auto makeVeh=[&](int16 model){
                        uint8* v=veh.data(); std::memset(v,0xCD,VEH_SZ); At<void*>(v,0)=vehVt.data();
                        const CVector vp=r.vec(60.f); At<CVector>(v,4)=vp; FillMat(r,matV.data(),vp); At<void*>(v,0x14)=matV.data();
                        At<void*>(v,0x18)=clump.data(); At<uint32>(v,0x1C)=r.next(); At<uint16>(v,0x20)=uint16(r.next()); At<int16>(v,0x22)=model;
                        v[0x36]=uint8((r.pick({0,0,1,2,3,3,4,5,8,10})<<3)|2); At<uint32>(v,0x40)=r.next();
                        for(size_t o=0x44;o<0x5C;o+=4)At<float>(v,o)=r.uni(-0.5f,0.5f);
                        At<float>(v,0x8C)=r.pickF({1500.f,300.f,40.f,499.f,500.f,2500.f}); At<float>(v,0x90)=r.pickF({3000.f,600.f,100.f,5000.f}); At<float>(v,0x12C)=r.pickF({0.f,0.03f,0.05f,0.5f,1.f,r.uni(0.f,1.f)});
                        At<void*>(v,0x384)=handling.data(); v[0x48B]=0xFF; At<uint32>(v,0x590)=uint32(r.pick({0,5,9,6})); At<uint32>(v,0x594)=uint32(r.pick({0,5,9,4,3}));
                        At<void*>(v,0x460)=nullptr; for(size_t k=0;k<8;++k)At<void*>(v,0x464+4*k)=nullptr;
                        // model info y handling
                        At<void*>(mi.data(),0)=pedVt.data(); mi[0xD]=uint8(r.pick({0,0,3})); At<void*>(mi.data(),0x14)=colModel.data(); At<void*>(mi.data(),0x5C)=vstruct.data();
                        for(size_t k=0;k<14*3;++k)At<float>(vstruct.data(),4*k)=r.coin(6) ? 0.f : r.uni(-2.f,2.f);
                        At<float>(handling.data(),4)=At<float>(v,0x8C); At<float>(handling.data(),0xA4)=r.pickF({0.8f,1.f,0.5f,0.9f,0.f}); At<float>(handling.data(),0xC4)=r.pickF({0.f,0.5f,1.f,0.3f,-0.5f});
                        handling[0x75]=uint8(r.pick({'E','P','D','E'})); At<uint32>(handling.data(),0xCC)=r.next(); handling[0xDE]=uint8(r.next()%30);
                        setMi(model,mi.data()); };
                    auto setSeats=[&](bool force){
                        At<void*>(veh.data(),0x460)=(force||!r.coin(3)) ? Ped(0) : nullptr;
                        for(size_t k=0;k<8;++k)At<void*>(veh.data(),0x464+4*k)=r.coin(3) ? nullptr : Ped(int(1+k));
                        for(size_t k=0;k<NPEDS;++k)At<uint32>(Ped(int(k)),0x530)=uint32(r.pick({50,50,0,7,1,50,42})); };
                    auto camAt=[&](const CVector& c){ AtA<uintptr_t>(ADDR_CAMMAT)=0; At<CVector>(reinterpret_cast<void*>(ADDR_CAMPOS),0)=c; };
                    auto setFxGlobals=[&]{ AtA<uintptr_t>(ADDR_GFX+4)=uintptr_t(Fx(0)); AtA<uintptr_t>(ADDR_GFX+8)=uintptr_t(Fx(1)); AtA<uintptr_t>(ADDR_GFX+0x24)=uintptr_t(Fx(2)); AtA<uintptr_t>(ADDR_GFX+0x30)=uintptr_t(Fx(3)); };

                    // -------------------------------------------------------------------------------------------------
                    if(nm=="PreRender"||nm=="PreRenderDriverAndPassengers"||nm=="RenderDriverAndPassengers") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); setSeats(false);
                        if(nm=="PreRender"){
                            At<uint32>(veh.data(),0x590)=uint32(r.pick({0,5,6,9,6,0})); veh[0x584]=uint8(r.next()); At<uint32>(veh.data(),0x574)=r.next();
                            AtA<float>(ADDR_DNBAL)=r.pickF({0.f,1.f,0.5f,0.25f,r.uni(0.f,1.f)}); cfg.lightCol=r.pickF({0.f,0.3f,1.f,0.77f,r.uni(0.f,2.f)});
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                        } else reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="Render") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); At<float>(veh.data(),0x4B0)=r.pickF({0.f,1.5f,15.9f,16.f,31.2f,-3.7f,8.99f,100.4f,r.uni(-5.f,40.f)});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="ResetAfterRender") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); At<uint32>(veh.data(),0x590)=uint32(r.pick({0,5,9,0}));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="SetupLighting"||nm=="RemoveLighting") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); cfg.lightRet=r.coin(2); At<uint32>(veh.data(),0x40)=r.next();
                        if(nm=="SetupLighting")ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(veh.data())&true); else reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(veh.data(),r.coin(2));
                    }
                    else if(nm=="SetCollisionLighting") {
                        makeVeh(400); reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(veh.data(),r.next()&0xFF);
                    }
                    else if(nm=="UpdateLightingFromStoredPolys") {
                        makeVeh(400); veh[0x544]=uint8(r.next()); veh[0x570]=uint8(r.next()); At<uint32>(veh.data(),0x574)=r.next();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="CalculateLightingFromCollision") {
                        makeVeh(400); At<uint32>(veh.data(),0x574)=r.next(); AtA<float>(ADDR_DNBAL)=r.pickF({0.f,1.f,0.5f,0.25f,0.123f,r.uni(0.f,1.f)});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="SetupRender") {
                        const int16 model=modelPool[r.next()%std::size(modelPool)]; makeVeh(model); At<uint32>(veh.data(),0x590)=uint32(r.pick({0,5,0,9}));
                        veh[0x42F]=uint8(r.next()); veh[0x434]=uint8(r.next()); veh[0x435]=uint8(r.next()); veh[0x436]=uint8(r.next()); veh[0x437]=uint8(r.next());
                        At<int16>(veh.data(),0x598)=int16(r.pick({-1,2,5,0,7})); At<int16>(veh.data(),0x59A)=int16(r.pick({-1,-1,0,3,7,5,2}));
                        At<void*>(veh.data(),0x59C)=r.coin(2) ? nullptr : texA.data();
                        // pool de TXD falso (el EXE inlinea CTxdStore::GetTxd) y estados de carga del streaming
                        At<void*>(txdPool.data(),0)=txdObjs.data(); At<void*>(txdPool.data(),4)=txdFlags.data(); At<uint32>(txdPool.data(),8)=8;
                        for(size_t k=0;k<8;++k){ txdFlags[k]=0; /* el EXE inlinea GetTxd y desreferencia el diccionario sin comprobar (0x6D65AE): ranuras siempre ocupadas */ At<void*>(txdObjs.data(),12*k)=r.coin(3) ? nullptr : reinterpret_cast<void*>(0x30000+k*0x100); At<uint32>(txdObjs.data(),12*k+4)=r.next(); At<uint32>(txdObjs.data(),12*k+8)=r.next();
                            AtA<uint8>(ADDR_STREAMINFO+20*k)=uint8(r.pick({1,1,0,2,3})); }
                        AtA<uintptr_t>(ADDR_TXDPOOL)=uintptr_t(txdPool.data()); cfg.texRet=r.coin(3) ? 0 : uintptr_t(texB.data()); AtA<uintptr_t>(ADDR_REMAPTEX)=0x77777777u;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                        out["remapTex"]=Name(AtA<uintptr_t>(ADDR_REMAPTEX));
                    }
                    else if(nm=="DoHeadLightBeam") {
                        const int16 model=int16(r.pick({400,530,530,460,420})); makeVeh(model);
                        const int32 dummyId=r.pick({0,1,1}); const int32 idx=2*dummyId; if(dummyId==1&&r.coin(3)){ At<CVector>(vstruct.data(),size_t(12*idx))=CVector{0.f,0.f,0.f}; }
                        const CVector cp=r.vec(30.f); camAt(cp); const CVector mp=cp+r.vec(10.f); FillMat(r,matL.data(),mp);
                        cfg.im3dRet=!r.coin(5);
                        for(int k=0;k<0xB4;k+=4)AtA<uint32>(ADDR_TMPVERTS+uintptr_t(k))=0xCDCDCDCDu; for(int k=0;k<0x20;k+=4)AtA<uint32>(ADDR_TMPIDX+uintptr_t(k))=0xCDCDCDCDu;
                        reinterpret_cast<void(__thiscall*)(void*,int32,void*,uint32)>(a)(veh.data(),dummyId,matL.data(),uint32(r.coin(2)));
                        out["tmpV"]=ReversingProbe::Bytes(reinterpret_cast<void*>(ADDR_TMPVERTS),0xB4); out["tmpI"]=ReversingProbe::Bytes(reinterpret_cast<void*>(ADDR_TMPIDX),0x20);
                    }
                    else if(nm=="DoHeadLightReflection"||nm=="DoHeadLightReflectionSingle"||nm=="DoHeadLightReflectionTwin") {
                        const int16 model=int16(r.pick({400,471,532,581,530})); makeVeh(model); At<uint32>(veh.data(),0x594)=uint32(r.pick({0,9,5,0}));
                        const CVector vp=r.vec(40.f); At<CVector>(veh.data(),4)=vp; FillMat(r,matV.data(),vp+(r.coin(2) ? CVector{0.f,0.f,0.f} : r.vec(1.f)));
                        At<void*>(veh.data(),0x14)=r.coin(3) ? nullptr : matV.data(); const CVector lp=vp+r.vec(3.f); FillMat(r,matL.data(),lp);
                        if(r.coin(6)){ At<float>(matL.data(),0x10)=0.f; At<float>(matL.data(),0x14)=0.f; } if(r.coin(8)){ At<float>(matL.data(),0)=0.f; At<float>(matL.data(),4)=0.f; }
                        AtA<uintptr_t>(ADDR_SHTEX)=uintptr_t(texA.data()); AtA<uintptr_t>(ADDR_SHTEX+4)=uintptr_t(texB.data());
                        if(nm=="DoHeadLightReflection") reinterpret_cast<void(__thiscall*)(void*,void*,uint32,bool,bool)>(a)(veh.data(),matL.data(),uint32(r.next()),r.coin(2),r.coin(2));
                        else if(nm=="DoHeadLightReflectionSingle") reinterpret_cast<void(__thiscall*)(void*,void*,bool)>(a)(veh.data(),matL.data(),r.coin(2));
                        else reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(veh.data(),matL.data());
                    }
                    else if(nm=="AddDamagedVehicleParticles") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); At<uint32>(veh.data(),0x594)=uint32(r.pick({0,0,4,9,5}));
                        At<float>(veh.data(),0x4C0)=r.pickF({100.f,249.f,250.f,251.f,400.f,649.f,650.f,700.f,1000.f,r.uni(200.f,700.f)});
                        if(r.coin(4))At<uint32>(veh.data(),0x40)|=0x100u; else At<uint32>(veh.data(),0x40)&=~0x100u;
                        At<void*>(veh.data(),0x18)=r.coin(4) ? nullptr : clump.data(); At<void*>(veh.data(),0x578)=r.coin(2) ? nullptr : Fx(1);
                        At<CVector>(veh.data(),0x44)=r.vec(1.f); cfg.fxRet=r.coin(4) ? 0 : uintptr_t(Fx(0));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="AddExhaustParticles") {
                        const int16 model=int16(r.pick({400,521,522,581,463,471,521,522,581})); makeVeh(model);
                        const bool bike=model==521||model==522||model==581||model==463; At<uint32>(veh.data(),0x594)=uint32(bike ? 9 : r.pick({0,0,5}));
                        if(r.coin(8))At<uint32>(veh.data(),0x1C)|=0x20000u; else At<uint32>(veh.data(),0x1C)&=~0x20000u;
                        const CVector cp=r.vec(20.f); camAt(cp); const float d=r.pickF({3.f,7.9f,8.f,8.1f,12.f,15.9f,16.f,16.1f,30.f}); const CVector vp=cp+CVector{d,0.f,0.f}*r.pickF({1.f,-1.f,1.f});
                        At<CVector>(veh.data(),4)=vp; FillMat(r,matV.data(),vp+r.vec(0.5f)); /* el EXE desreferencia la matriz sin comprobar */ At<void*>(veh.data(),0x14)=matV.data();
                        FillMat(r,veh.data()+0x5CC,vp); At<float>(veh.data()+0x5CC,0x30)=vp.x; std::memset(veh.data()+0x5CC+0x40,0,8);
                        if(r.coin(3))At<CVector>(vstruct.data(),12*6)=CVector{0.f,0.f,0.f};
                        At<uint32>(handling.data(),0xCC)=r.next(); veh[0x438]=uint8(r.pick({0,1,2,3}));
                        At<CVector>(veh.data(),0x44)=r.coin(3) ? CVector{0.f,0.f,0.f} : r.vec(0.4f); At<float>(veh.data(),0x49C)=r.pickF({0.f,0.4f,0.5f,0.6f,1.f,-0.5f}); veh[0x4B4]=uint8(r.pick({0,1,2,3,4}));
                        if(r.coin(2))At<uint32>(veh.data(),0x40)|=0x8000000u; else At<uint32>(veh.data(),0x40)&=~0x8000000u;
                        cfg.waterRet=!r.coin(3); cfg.waterZ=vp.z+r.pickF({-1.f,0.f,1.f,0.2f,-0.3f}); setFxGlobals();
                        AtA<uint32>(ADDR_EXHGUARD)=r.coin(2) ? 1u : r.next()&~1u; AtA<float>(ADDR_EXHFACTOR)=r.uni(-1.8f,-0.9f);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                        out["exh"]=json::array({AtA<uint32>(ADDR_EXHGUARD),AtA<uint32>(ADDR_EXHFACTOR)});
                    }
                    else if(nm=="AddWaterSplashParticles") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]);
                        const CVector cp=r.vec(20.f); camAt(cp); const float d=r.pickF({0.f,3.f,9.9f,10.f,10.1f,14.f,30.f}); const CVector vp=cp+CVector{d,0.f,0.f}*r.pickF({1.f,-1.f});
                        At<CVector>(veh.data(),4)=vp; FillMat(r,matV.data(),vp+r.vec(0.5f)); At<void*>(veh.data(),0x14)=matV.data();
                        const int ntri=r.pick({0,1,2,4}); At<void*>(colModel.data(),0x2C)=colData.data(); At<int16>(colData.data(),4)=int16(ntri); At<void*>(colData.data(),0x14)=verts.data(); At<void*>(colData.data(),0x18)=tris.data();
                        for(size_t k=0;k<9;++k){ At<int16>(verts.data(),6*k)=int16(r.pick({-100,-50,0,50,100,127,-128,300})); At<int16>(verts.data(),6*k+2)=int16(r.pick({-100,-50,0,50,100,127,-128,300})); At<int16>(verts.data(),6*k+4)=int16(r.pick({-100,-50,0,50,100,127,-128,300})); }
                        for(size_t k=0;k<4;++k){ At<uint16>(tris.data(),8*k)=uint16(r.next()%9); At<uint16>(tris.data(),8*k+2)=uint16(r.next()%9); At<uint16>(tris.data(),8*k+4)=uint16(r.next()%9); At<uint16>(tris.data(),8*k+6)=0; }
                        AtA<float>(ADDR_RAIN)=r.pickF({0.f,0.4f,0.6f,1.f,0.25f,0.5f}); setFxGlobals();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
                    }
                    else if(nm=="AddWheelDirtAndWater") {
                        makeVeh(400); At<float>(veh.data(),0x12C)=r.pickF({0.f,0.5f,1.f,0.123f});
                        std::memset(colPoint.data(),0,colPoint.size()); At<CVector>(colPoint.data(),0)=r.vec(30.f); colPoint[0x23]=uint8(r.next()%180); colPoint[0x24]=uint8(r.next()%180);
                        AtA<float>(ADDR_WET)=r.pickF({0.f,0.2f,0.4f,0.41f,0.5f,1.f,-0.1f}); cfg.camNoRain=r.coin(3); setFxGlobals();
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*,void*,bool,bool,bool)>(a)(veh.data(),colPoint.data(),r.coin(2),r.coin(2),r.coin(4)));
                    }
                    else if(nm=="DoBoatSplashes") {
                        const int16 model=int16(r.pick({430,460,472,460,400})); makeVeh(model); veh[0x3BA]=uint8(r.pick({0,1,0,2}));
                        const CVector cp=r.vec(30.f); camAt(cp); const float d=r.pickF({0.f,10.f,49.9f,50.f,50.1f,70.f,79.9f,80.f,85.f});
                        const CVector vp=cp+CVector{d,r.uni(-3.f,3.f),r.uni(-3.f,3.f)}*r.pickF({1.f,-1.f}); At<CVector>(veh.data(),4)=vp; FillMat(r,matV.data(),vp); At<void*>(veh.data(),0x14)=matV.data();
                        { const float sp=r.pickF({0.f,0.04f,0.05f,0.06f,0.3f,0.9f,2.f}); const CVector dir=r.vec(1.f); At<CVector>(veh.data(),0x44)=CVector{dir.x*sp,dir.y*sp,dir.z*sp}; }
                        At<CVector>(colModel.data(),0)=CVector{-r.uni(0.5f,2.f),-r.uni(1.f,5.f),-r.uni(0.f,2.f)}; At<CVector>(colModel.data(),0xC)=CVector{r.uni(0.5f,2.f),r.uni(1.f,5.f),r.uni(0.f,2.f)};
                        cfg.lookFwd=r.coin(8); cfg.visible=!r.coin(8); setFxGlobals();
                        reinterpret_cast<void(__thiscall*)(void*,float)>(a)(veh.data(),r.pickF({0.f,0.1f,0.5f,1.f,2.f,3.f,r.uni(0.f,6.f)}));
                    }
                    else if(nm=="GetVehicleLightsStatus") {
                        makeVeh(400); AtA<uint8>(ADDR_CLOCK)=uint8(r.next()%60); AtA<uint8>(ADDR_CLOCK+1)=uint8(r.pick({0,5,6,7,12,19,20,21,23,int32(r.next()%24)}));
                        { const uint16 seed=uint16(r.next()); At<uint16>(veh.data(),0x20)=seed; const uint8 hh=AtA<uint8>(ADDR_CLOCK+1); if(hh==20||hh==6){ const int sl=seed&0x3F; AtA<uint8>(ADDR_CLOCK)=uint8(std::clamp(sl+int(r.pick({-1,0,1,0})),0,59)); } }
                        AtA<float>(ADDR_FOG)=r.pickF({0.f,0.1f,0.5f,1.f}); AtA<float>(ADDR_WET)=r.pickF({0.f,0.1f,0.5f,1.f,0.0003f}); At<float>(veh.data(),0x12C)=r.pickF({0.f,0.04f,0.05f,0.06f,1.f});
                        cfg.camNoRain=r.coin(2); AtA<uint8>(ADDR_GANGAREA)=uint8(r.coin(2));
                        At<void*>(veh.data(),0x460)=r.coin(4) ? nullptr : Ped(0); At<uint32>(Ped(0),0x598)=uint32(r.pick({0,4,6,7,8,12,16,17,18,5})); At<uint16>(Ped(0),0x20)=uint16(r.next());
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(veh.data()));
                    }
                    else if(nm=="DoPlaneGunFireFX") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); const int sub=r.pick({4,3,4,3,0}); At<uint32>(veh.data(),0x594)=uint32(sub);
                        cfg.numGuns=r.pick({1,2,3,4}); const int32 fxIdx=r.pick({0,1,2,3}); At<void*>(veh.data(),0x18)=r.coin(5) ? nullptr : clump.data();
                        for(size_t k=0;k<8;++k)gunParts[k]=r.coin(2) ? nullptr : static_cast<void*>(Fx(int(k%NFX)));
                        const uintptr_t partsPtr=r.coin(3) ? 0 : uintptr_t(gunParts.data()); At<uintptr_t>(veh.data(),0x9D4)=partsPtr; At<uintptr_t>(veh.data(),0xA0C)=partsPtr;
                        AtA<uint8>(ADDR_PLANEEJECT)=uint8(r.coin(2)); cfg.fxRet=r.coin(4) ? 0 : uintptr_t(Fx(3));
                        At<CVector>(vecs.data(),0)=r.vec(5.f); At<CVector>(vecs.data(),0x10)=r.vec(5.f);
                        { ReversingProbe::EntryPatch n2(EXE_NEWARR,FakeNewArr);
                          reinterpret_cast<void(__thiscall*)(void*,void*,void*,void*,int32)>(a)(veh.data(),weapon.data(),vecs.data(),vecs.data()+0x10,fxIdx); }
                        out["parts"]=Snap(gunParts.data(),sizeof(gunParts),detail); out["newArr"]=Snap(newArr.data(),sizeof(newArr),detail); out["vp"]=json::array({Name(At<void*>(veh.data(),0x9D4)),Name(At<void*>(veh.data(),0xA0C))});
                    }
                    else if(nm=="ProcessDrivingAnims") {
                        makeVeh(modelPool[r.next()%std::size(modelPool)]); At<void*>(Ped(0),0x18)=clump.data();
                        if(r.coin(8))At<uint32>(veh.data(),0x1C)|=0x20000u; else At<uint32>(veh.data(),0x1C)&=~0x20000u;
                        At<uint32>(veh.data(),0x590)=uint32(r.pick({0,5,0,9})); At<uint32>(veh.data(),0x594)=uint32(r.pick({0,5,3,4,9,0}));
                        if(r.coin(3))veh[0x429]|=8; else veh[0x429]&=uint8(~8); if(r.coin(3))At<uint32>(handling.data(),0xCC)|=0x400u; else At<uint32>(handling.data(),0xCC)&=~0x400u;
                        { const float sp=r.pickF({0.f,0.1f,0.23f,0.4f,0.41f,0.9f}); const CVector dir=r.vec(1.f); At<CVector>(veh.data(),0x44)=CVector{dir.x*sp,dir.y*sp,dir.z*sp}; }
                        At<float>(veh.data(),0x49C)=r.pickF({-1.f,-0.5f,0.f,0.5f,1.f}); At<float>(veh.data(),0x494)=r.pickF({0.f,0.2f,-0.2f,0.5f,-0.6f,1.f,-1.f,0.61f,-0.61f,0.f});
                        cfg.stat=r.pickF({0.f,49.9f,50.f,75.f,99.9f,100.f,1000.f}); cfg.isPlayer=!r.coin(8); cfg.blendRet=r.coin(2); cfg.assocSeed=r.next();
                        AtA<uint8>(ADDR_ACTIVECAM)=0; AtA<uint16>(ADDR_CAMMODE)=uint16(r.pick({16,4,16,17})); AtA<uint32>(ADDR_CAMDIR)=uint32(r.pick({1,0,2,1}));
                        reinterpret_cast<void(__thiscall*)(void*,void*,bool)>(a)(veh.data(),Ped(0),r.coin(2));
                    }
                    else if(nm=="ProcessOpenDoor") {
                        makeVeh(400); const int32 grp=r.pick({0,1,2,3,5,8,12,20,29}); handling[0xDE]=uint8(grp);
                        cfg.doorMissing=r.coin(5) ? r.next() : 0; for(int k=0;k<8;++k)cfg.ratio[k]=r.pickF({0.f,0.2f,0.5f,0.9f,1.f,r.uni(0.f,1.f)});
                        static const int32 anims[]{355,356,357,358,359,364,365,366,367,368,369,370,371,373,374,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,0,100};
                        static const int32 groups[]{88,89,90,102,103,104,107,108,110,113,91,95,0};
                        const int32 animId=anims[r.next()%std::size(anims)];
                        const bool needsGroup=animId==384||animId==385||animId==386; static const int32 okGroups[]{88,89,90,102,103,107,108,110,113};
                        const int32 animGroup=needsGroup ? okGroups[r.next()%std::size(okGroups)] : groups[r.next()%std::size(groups)];
                        // tiempos reales de la tabla de grupos de animacion (mismos fixtures para ambos lados)
                        float cand[8]{}; for(int m=0;m<4;++m){ float s0=0.f,e0=0.f; CVehicleAnimGroupData::GetInOutTimings(AssocGroupId(grp),eInOutTimingMode(m),&s0,&e0); cand[2*m]=s0; cand[2*m+1]=e0; }
                        float fTime=r.pickF({0.f,1.f,0.05f,0.1f,0.4f,0.2f,0.3f,0.475f,0.01f,r.uni(0.f,1.f),r.uni(0.f,1.f),r.uni(0.f,1.f),cand[0],cand[1],cand[2],cand[3],cand[4],cand[5],cand[6],cand[7]});
                        if(r.coin(5)){ const float c0=cand[r.next()%8]; fTime=c0+r.pickF({-0.01f,0.01f,0.f}); }
                        static const int32 comps[]{8,9,10,11};
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32,uint32,uint32,float)>(a)(veh.data(),Ped(0),uint32(comps[r.next()%4]),uint32(animGroup),uint32(animId),fTime);
                    }
                    else if(nm=="ProcessWheel") {
                        makeVeh(int16(r.pick({400,441,520,521,460,401})));
                        At<uint32>(veh.data(),0x590)=uint32(r.pick({0,9,0,5})); At<uint32>(veh.data(),0x594)=uint32(r.pick({0,4,9,0,5}));
                        At<float>(handling.data(),4)=r.pickF({100.f,499.f,500.f,1500.f,40.f,2500.f}); At<float>(veh.data(),0x8C)=r.pickF({1500.f,300.f,40.f,2500.f}); At<float>(veh.data(),0x90)=r.pickF({3000.f,600.f,100.f,5000.f});
                        At<float>(veh.data(),0x49C)=r.pickF({0.f,0.005f,0.01f,0.5f,-0.5f,1.f,-1.f});
                        AtA<float>(ADDR_WHEELFRIC)=r.pickF({0.8f,1.5f,0.1f,2.f,0.5f}); AtA<uint8>(ADDR_WHEELSTATICS)=uint8(r.coin(2)); AtA<uint8>(ADDR_WHEELSTATICS+1)=uint8(r.coin(2)); AtA<uint8>(ADDR_WHEELSTATICS+2)=uint8(r.coin(2));
                        CVector fwd=r.vec(1.f), right=r.vec(1.f), speed=r.coin(5) ? CVector{0.f,0.f,0.f} : r.vec(r.pickF({0.05f,0.3f,1.f,3.f})), contact=r.vec(2.f);
                        if(r.coin(8)){ speed=CVector{0.f,0.f,0.f}; } if(r.coin(6)){ fwd=CVector{0.f,1.f,0.f}; right=CVector{1.f,0.f,0.f}; speed.y=r.pickF({0.f,0.1f,0.2f,-0.1f,0.003f}); speed.x=r.pickF({0.f,0.01f,-0.2f}); }
                        At<CVector>(vecs.data(),0)=fwd; At<CVector>(vecs.data(),0x10)=right; At<CVector>(vecs.data(),0x20)=speed; At<CVector>(wheelState.data(),0x20)=contact;
                        At<int32>(wheelState.data(),0)=r.pick({0,1,2,3}); const float thrust=r.pickF({0.f,0.5f,1.f,-0.7f,2.f}), brake=r.pickF({0.f,0.f,0.3f,1.f,0.05f}), adh=r.pickF({0.05f,0.2f,0.5f,1.f,1.5f,r.uni(0.f,2.f)});
                        reinterpret_cast<void(__thiscall*)(void*,CVector&,CVector&,CVector&,CVector&,int32,float,float,float,int8,float*,int32*,uint16)>(a)(
                            veh.data(),At<CVector>(vecs.data(),0),At<CVector>(vecs.data(),0x10),At<CVector>(vecs.data(),0x20),At<CVector>(wheelState.data(),0x20),
                            r.pick({1,2,3,4}),thrust,brake,adh,int8(r.pick({0,1,2,3})),reinterpret_cast<float*>(wheelState.data()+0x10),reinterpret_cast<int32*>(wheelState.data()),uint16(r.pick({0,1,0,2})));
                        out["state"]=At<int32>(wheelState.data(),0); out["statics"]=json::array({AtA<uint8>(ADDR_WHEELSTATICS),AtA<uint8>(ADDR_WHEELSTATICS+1),AtA<uint8>(ADDR_WHEELSTATICS+2)});
                    }
                    else if(nm=="ProcessWheelRotation") {
                        makeVeh(400); const CVector a1=r.vec(2.f), a2=r.vec(2.f); const float d=r.pickF({1.f,2.f,0.5f,r.uni(0.1f,5.f),-1.f});
                        ret=F(reinterpret_cast<float(__thiscall*)(void*,int32,const CVector&,const CVector&,float)>(a)(veh.data(),r.pick({0,1,2,3,4}),a1,a2,d));
                    }
                    // comparación común
                    out["veh"]=Snap(veh.data(),VEH_SZ,detail); out["peds"]=Snap(peds.data(),sizeof(peds),detail); out["fxs"]=Snap(fxs.data(),sizeof(fxs),detail);
                    out["assocs"]=Snap(assocs.data(),sizeof(assocs),detail);
                    out["ret"]=ret; out["trace"]=trace;
                    restoreAll();
                    return out;
                };
                // las excepciones de C++ (p. ej. std::bad_alloc o un assert de la DLL) se registran en vez de abortar el juego
                auto safeRun=[&](bool cand,bool det)->json {
                    try { return run(cand,det); }
                    catch(const std::exception& e){ restoreAll(); return json{{"exception",std::string(e.what())},{"candidate",cand},{"switches",false},{"ret",nullptr},{"trace",trace}}; }
                    catch(...){ restoreAll(); return json{{"exception","unknown"},{"candidate",cand},{"switches",false},{"ret",nullptr},{"trace",trace}}; } };
                const auto original=safeRun(false,false), candidate=safeRun(true,false);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else {
                    const auto originalD=safeRun(false,true), candidateD=safeRun(true,true);
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
