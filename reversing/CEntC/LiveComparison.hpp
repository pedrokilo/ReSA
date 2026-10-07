#pragma once
// Banco dentro del motor: CEntity, grupo C (efectos 2D, dibujo e iluminación; 16 funciones): CreateEffects, DestroyEffects,
// GetRandom2dEffect, HasPreRenderEffects, ProcessLightsForEntity, PreRender, PreRenderForGlassWindow, Render, RenderEffects,
// RemoveEscalatorsForEntity, RemoveLighting, SetupLighting, SetAtomicAlpha, SetCompAlphaCB, SetRwObjectAlpha y
// MaterialUpdateUVAnimCB (las virtuales por la vtable del EXE 0x863928: PreRender 17, Render 18, SetupLighting 19,
// RemoveLighting 20).
// Original (hooks de estas funciones desactivados) frente a candidato con fixtures deterministas. Nunca se toca una entidad
// real ni un pool real: la entidad es un búfer FALSO (patrón 0xCD) con vptr a una vtable falsa con trazas; la matriz, los
// clumps/atomics/geometrías/materiales/frames, los model infos (con vtable de trazas y listas de efectos 2D de todos los
// tipos), el pool de entradas/salidas y los textos de los carteles son búferes propios. Los model infos falsos se colocan
// temporalmente en ms_modelInfoPtrs. Se simulan y trazan (dirección del EXE + símbolo de la DLL): rand/srand y
// CGeneral::GetRandomNumber, CBaseModelInfo::Get2dEffect, GetDayNightBalance, CEntity::TransformFromObjectSpace, coronas,
// luces puntuales, sombras, rastros de motion blur, agua, pickups, semáforos, cristales, cartas (CTagManager), streaming,
// CCustomRoadsignMgr, CCustomBuildingDNPipeline, Fx, scripts de cerebros, gestor de entradas/salidas, escaleras, gestor de
// atractores, RenderWare (frames, atomics, materiales, clumps, estados de render, UV anim, MatFX), luces del renderer y
// AllocateMatrix/UpdateMatrix de CPlaceable. Globales tocadas (reloj, cámara, clima, ciclo horario, Replay, escaleras, pool de
// entradas/salidas, estados del motor, árbol más cercano, ms_modelInfoPtrs) se guardan y restauran en cada ejecución.
#include <array>
#include <filesystem>
#include <fstream>
#include <set>
#include <string>
#include <vector>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Entity.h"
#include "Entity/Object/Object.h"
#include "Models/BaseModelInfo.h"
#include "Models/ModelInfo.h"
#include "PointLights.h"
#include "Escalators.h"
#include "CustomBuildingDNPipeline.h"
#include "MotionBlurStreaks.h"
#include "TagManager.h"
#include "EntryExitManager.h"
#include "TrafficLights.h"
#include "Glass.h"
#include "TheScripts.h"
#include "Shadows.h"
#include "Coronas.h"
#include "Pickups.h"
#include "Streaming.h"
#include "CustomRoadsignMgr.h"
#include "PedAttractorManager.h"
#include "Fx/Fx.h"
#include "WaterLevel.h"
#include "GameLogic.h"
#include "Bridge.h"
#include "Weather.h"
#include "TimeCycle.h"
#include "Timer.h"
#include "Clock.h"
#include "Replay.h"
#include "General.h"
#include "../ProbeRuntime.hpp"

namespace CEntCLive {
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

// --- módulo de la DLL y cuerpos de las funciones enganchadas ---
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
constexpr uintptr_t ENT_VT=0x863928, ADDR_MI_PTRS=0xA9B0C8, ADDR_TIMESTEP=0xB7CB5C, ADDR_FRAMECTR=0xB7CB4C, ADDR_TIMEMS=0xB7CB84,
    ADDR_CAMPOS=0xB6F02C, ADDR_CAMMAT=0xB6F03C, ADDR_CAMRIGHT=0xB6F99C, ADDR_SUNGLARE=0xC812E0, ADDR_WET=0xC81308, ADDR_RAIN=0xC81324,
    ADDR_TCSTORED=0xB79FD0, ADDR_VECSUN=0xB7CA50, ADDR_TCCOLOURS=0xB7C4A0, ADDR_REPLAY=0xA43088, ADDR_TREEDIST=0x8D0A20,
    ADDR_ENGINE=0xC97B24, ADDR_ESCAL=0xC6E9A8, ADDR_EEPOOL=0x96A7D8, ADDR_CLOCKH=0xB70153, ADDR_GFX=0xA9AE00, ADDR_BRAINS=0xA90CF0,
    ADDR_SHADOWEXPL=0xC403F4, ADDR_SHADOWPED=0xC403E4, ADDR_CORONATEX=0xC3E000;
constexpr size_t ENT_SZ=0x300, NESC=32, ESC_SZ=0x150;
constexpr int NFX=64;

alignas(16) inline std::array<uint8,ENT_SZ> ent{};
alignas(16) inline std::array<uint8,0x48> matP{}, matCam{};
alignas(16) inline std::array<std::array<uint8,0x400>,2> clumps{};
alignas(16) inline std::array<std::array<uint8,0x100>,6> frames{};
alignas(16) inline std::array<std::array<uint8,0x60>,6> atoms{};
alignas(16) inline std::array<std::array<uint8,0x40>,6> geoms{};
alignas(16) inline std::array<std::array<uint8,0x20>,4> mats{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> mis{};
alignas(16) inline std::array<std::array<uint8,0x40>,NFX> fx2d{};
alignas(16) inline std::array<std::array<uint8,64>,2> texts{};
alignas(16) inline std::array<uint8,0x40> lnk{}, attrMgr{}, texA{}, texB{}, texC{};
alignas(16) inline std::array<uint8,0x20> eePool{};
alignas(16) inline std::array<uint8,8*0x3C> eeObjs{};
alignas(16) inline std::array<uint8,8> eeMap{};
inline std::array<void*,64> entVt{}, miVt{};
inline std::array<uintptr_t,64> thunkRet{};
inline uint8* Clump(int k){ return clumps[size_t(k)].data(); }
inline uint8* Frame(int k){ return frames[size_t(k)].data(); }
inline uint8* AtomB(int k){ return atoms[size_t(k)].data(); }
inline uint8* GeomB(int k){ return geoms[size_t(k)].data(); }
inline uint8* MatB(int k){ return mats[size_t(k)].data(); }
inline uint8* Mi(int k){ return mis[size_t(k)].data(); }
inline uint8* FxE(int k){ return fx2d[size_t(k)].data(); }

// --- nombres estables para punteros ---
struct Named { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Named> named;
inline void Reg(const void* p,size_t n,std::string nm,size_t stride=0){ named.push_back({uintptr_t(p),n,std::move(nm),stride}); }
inline const Named* FindNamed(uintptr_t v){ for(auto& b:named)if(v>=b.p&&v<b.p+b.n)return &b; return nullptr; }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    if(v==ENT_VT)return "vt_ent";
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
inline json V3(float x,float y,float z){ return json{F(x),F(y),F(z)}; }

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
    float dnb{}, lightsRet{}, waterZ{}; bool riots{}, bridge{}, glass{}, mitl{}, isTag{}, matfx{}, uvExists{}, water{};
    uintptr_t amiRet{}; int32 modelType{}, nMats{}, nAtoms{}, enexAdd{-1}, enexNear{-1}; uintptr_t newAtomic{};
    uint32 emptyMask{}, emptyCalls{}, alphaRef{}, matSeed{};
};
inline Cfg cfg{};
inline uint32 rngState{};
inline int __cdecl FakeRand(){ if(!Main())return 0x1234; T({"rand"}); rngState=rngState*1103515245u+12345u; return int((rngState>>16)&0x7FFF); }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline void __cdecl FakeSrand(uint32 s){ T({"srand",int64(int32(s-uint32(uintptr_t(ent.data()))))}); }

// vtables falsas con trazas: la entrada N devuelve thunkRet[N]
template<int N> inline uintptr_t __fastcall VThunk(void* self,void*){ T({"vcall",N,Name(self)}); return thunkRet[size_t(N)]; }
template<size_t... I> inline void FillVt(std::array<void*,64>& a,std::index_sequence<I...>){ ((a[I]=reinterpret_cast<void*>(&VThunk<int(I)>)),...); }
inline void* __fastcall MiAsAtomic(void* self,void*){ T({"mi_as_atomic_model_info",Name(self)}); return reinterpret_cast<void*>(cfg.amiRet); }
inline int32 __fastcall MiModelType(void* self,void*){ T({"mi_get_model_type",Name(self)}); return cfg.modelType; }

// --- dependencias simuladas: modelos y efectos ---
inline void* __fastcall FakeGet2dEffect(void* mi,void*,int32 idx){ T({"get_2d_effect",Name(mi),idx}); return FxE(int(idx&(NFX-1))); }
inline float __cdecl FakeDayNight(){ T({"day_night_balance"}); return cfg.dnb; }
inline CVector* __fastcall FakeTfos(void* self,void*,CVector* out,const CVector* in){
    T({"transform_from_object_space",Name(self),V(in)});
    const CVector* base=At<void*>(self,0x14) ? reinterpret_cast<const CVector*>(At<uint8*>(self,0x14)+0x30) : reinterpret_cast<const CVector*>(static_cast<uint8*>(self)+4);
    out->x=in->x*1.5f+base->x; out->y=in->y-0.25f+base->y; out->z=in->z+base->z; return out; }
inline void __cdecl FakeCorona(uint32 id,void* attach,uint32 r,uint32 g,uint32 b,uint32 inten,const CVector* pos,float size,float range,void* tex,uint32 flare,uint32 refl,uint32 los,uint32 trail,float ang,uint32 neon,float pull,uint32 full,float fade,uint32 below,uint32 white){
    T({"register_corona",int64(int32(id-uint32(uintptr_t(ent.data())))),Name(attach),r&0xFF,g&0xFF,b&0xFF,inten&0xFF,V(pos),F(size),F(range),Name(tex),flare&0xFF,refl&0xFF,los&0xFF,trail&0xFF,F(ang),neon&0xFF,F(pull),full&0xFF,F(fade),below&0xFF,white&0xFF}); }
inline void __cdecl FakeCoronaCoors(uint32 id,const CVector* pos,float range,float ang){ T({"update_corona_coors",int64(int32(id-uint32(uintptr_t(ent.data())))),V(pos),F(range),F(ang)}); }
inline void __cdecl FakeAddLight(uint32 type,float px,float py,float pz,float dx,float dy,float dz,float radius,float r,float g,float b,uint32 fog,uint32 extra,void* ent2){
    T({"add_light",type&0xFF,V3(px,py,pz),V3(dx,dy,dz),F(radius),F(r),F(g),F(b),fog&0xFF,extra&0xFF,Name(ent2)}); }
inline uint32 __cdecl FakeStaticShadow(uint32 id,uint32 type,void* tex,const CVector* pos,float fx,float fy,float sx,float sy,uint32 inten,uint32 r,uint32 g,uint32 b,float z,float scale,float dist,uint32 temp,float up){
    T({"store_static_shadow",int64(int32(id-uint32(uintptr_t(ent.data())))),type&0xFF,Name(tex),V(pos),F(fx),F(fy),F(sx),F(sy),int16(inten),r&0xFF,g&0xFF,b&0xFF,F(z),F(scale),F(dist),temp&0xFF,F(up)}); return 1; }
inline void __cdecl FakeShadowToBeRendered(uint32 type,void* tex,const CVector* pos,float a,float b,float c,float d,uint32 inten,uint32 r,uint32 g,uint32 b2,float z,uint32 water,float scale,void* rts,uint32 bld){
    T({"store_shadow_to_be_rendered",type&0xFF,uintptr_t(tex),V(pos),F(a),F(b),F(c),F(d),int16(inten),r&0xFF,g&0xFF,b2&0xFF,F(z),water&0xFF,F(scale),Name(rts),bld&0xFF}); }
inline void __cdecl FakeShadowForPole(void* e,float x,float y,float z,float h,float w,uint32 lid){ T({"store_shadow_for_pole",Name(e),F(x),F(y),F(z),F(h),F(w),lid}); }
inline void __cdecl FakeStreak(uint32 id,uint32 r,uint32 g,uint32 b,uint32 a,float l0,float l1,float l2,float r0,float r1,float r2){
    T({"register_streak",int64(int32(id-uint32(uintptr_t(ent.data())))),r&0xFF,g&0xFF,b&0xFF,a&0xFF,V3(l0,l1,l2),V3(r0,r1,r2)}); }
inline bool __cdecl FakeWaterNoWaves(float x,float y,float z,float* out,float* a,float* b){ T({"water_level_no_waves",F(x),F(y),F(z),a==nullptr,b==nullptr}); if(cfg.water)*out=cfg.waterZ; return cfg.water; }
inline bool __cdecl FakeRiots(){ T({"la_riots_active_here"}); return cfg.riots; }
inline bool __cdecl FakeBridge(){ T({"bridge_lights_flashing"}); return cfg.bridge; }
inline void __cdecl FakeDisplayLight(void* e){ T({"traffic_light_display",Name(e)}); }
inline bool __cdecl FakeIsMITL(int32 idx){ T({"is_mi_traffic_light",idx}); return cfg.mitl; }
inline void __cdecl FakeCollectable(void* e){ T({"pickup_collectable",Name(e)}); }
inline void __cdecl FakeMoney(void* e){ T({"pickup_money",Name(e)}); }
inline void __cdecl FakeMine(void* e){ T({"pickup_mine",Name(e)}); }
inline void __cdecl FakePickUp(void* e){ T({"pickup_effects",Name(e)}); }
inline bool __cdecl FakeIsGlass(void* e){ T({"is_object_glass",Name(e)}); return cfg.glass; }
inline void __cdecl FakeAskGlass(void* e){ T({"ask_glass",Name(e)}); }
inline void __fastcall FakeTreeWind(void* e,void*){ T({"modify_matrix_tree_in_wind",Name(e)}); }
inline void __fastcall FakeCrane(void* e,void*){ T({"modify_matrix_crane",Name(e)}); }
inline void __fastcall FakeWindSock(void* e,void*){ T({"build_windsock_matrix",Name(e)}); }
inline void __fastcall FakeUpdateRwMatrix(void* e,void*){ T({"update_rw_matrix",Name(e)}); }

// --- RenderWare y dibujo ---
inline json CbName(void* cb);
inline void* __cdecl FakeFrameUpdate(void* f){ T({"frame_update_objects",Name(f)}); return f; }
inline void* __cdecl FakeFrameIdentity(void* f){ T({"frame_set_identity",Name(f)}); return f; }
inline void* __cdecl FakeFrameRotate(void* f,const float* axis,float angle,int32 op){ T({"frame_rotate",Name(f),V3(axis[0],axis[1],axis[2]),F(angle),op}); return f; }
inline void* __cdecl FakeFrameTranslate(void* f,const float* v,int32 op){ T({"frame_translate",Name(f),V3(v[0],v[1],v[2]),op}); return f; }
inline void* __cdecl FakeAtomicSetFrame(void* a,void* f){ T({"atomic_set_frame",Name(a),Name(f)}); At<void*>(a,4)=f; return a; }
inline RwBool __cdecl FakeFrameDestroy(void* f){ T({"frame_destroy",Name(f)}); return 1; }
inline RwBool __cdecl FakeAtomicDestroy(void* a){ T({"atomic_destroy",Name(a)}); return 1; }
inline RwBool __cdecl FakeMatFx(void* a){ T({"matfx_query_effects",Name(a)}); return cfg.matfx; }
inline void* __cdecl FakeForAllMaterials(void* g,void* cb,void* data){
    T({"for_all_materials",Name(g),CbName(cb),uintptr_t(data)});
    for(int k=0;k<cfg.nMats;++k){ void* rv=reinterpret_cast<void*(__cdecl*)(void*,void*)>(cb)(MatB(k),data); T({"material_cb_ret",Name(rv)}); }
    return g; }
inline void* __cdecl FakeForAllAtomics(void* c,void* cb,void* data){
    T({"for_all_atomics",Name(c),CbName(cb),uintptr_t(data)});
    for(int k=0;k<cfg.nAtoms;++k){ void* rv=reinterpret_cast<void*(__cdecl*)(void*,void*)>(cb)(AtomB(k),data); T({"atomic_cb_ret",Name(rv)}); }
    return c; }
inline void* __cdecl FakeClumpRender(void* c){ T({"clump_render",Name(c)}); return c; }
inline void* __cdecl FakeAtomicRenderCb(void* a){ T({"atomic_render_callback",Name(a)}); return a; }
inline RwBool __cdecl FakeUvExists(const void* m){ T({"uv_anim_exists",Name(m)}); return cfg.uvExists; }
inline void* __cdecl FakeUvAddTime(void* m,float dt){ T({"uv_anim_add_time",Name(m),F(dt)}); return m; }
inline void* __cdecl FakeUvApply(void* m){ T({"uv_anim_apply_update",Name(m)}); return m; }
inline void __cdecl FakeDnAtomic(void* p,uint32 b){ T({"dn_pre_render_update_atomic",Name(p),b&0xFF}); }
inline void __cdecl FakeDnClump(void* p,uint32 b){ T({"dn_pre_render_update_clump",Name(p),b&0xFF}); }
inline bool __cdecl FakeIsTag(const void* e){ T({"tag_is_tag",Name(e)}); return cfg.isTag; }
inline void __cdecl FakeRenderTag(void* a){ T({"tag_render",Name(a)}); }
inline void __cdecl FakeStreamRender(void* l){ T({"streaming_render_entity",Name(l)}); }
inline RwBool __cdecl RSSet(int32 st,void* v){ T({"render_state_set",st,uintptr_t(v)}); return 1; }
inline RwBool __cdecl RSGet(int32 st,void* v){ T({"render_state_get",st}); *reinterpret_cast<uint32*>(v)=cfg.alphaRef; return 1; }
inline void* __cdecl FakeRenderRoadsign(void* a,const CVector* pos){ T({"render_roadsign_atomic",Name(a),V(pos)}); return a; }
inline void* __cdecl FakeCreateRoadsign(float w,float h,int32 lines,const char* t0,const char* t1,const char* t2,const char* t3,int32 letters,uint32 pal){
    T({"create_roadsign_atomic",F(w),F(h),lines,Name(t0),Name(t1),Name(t2),Name(t3),letters,pal&0xFF}); return reinterpret_cast<void*>(cfg.newAtomic); }
inline int32 __cdecl FakeRsLines(uint32 f){ T({"roadsign_num_lines",f&0xFFFF}); return int32((f&3)+1); }
inline int32 __cdecl FakeRsLetters(uint32 f){ T({"roadsign_num_letters",f&0xFFFF}); return int32(((f>>2)&3)*4+2); }
inline int32 __cdecl FakeRsPalette(uint32 f){ T({"roadsign_palette",f&0xFFFF}); return int32((f>>4)&3); }

// --- Fx, scripts, entradas/salidas, escaleras, atractores, matrices ---
inline void __fastcall FakeCreateEntityFx(void* fx,void*,void* e,const char* nm,const CVector* pos,void* m){
    T({"create_entity_fx",uintptr_t(fx)==ADDR_GFX,Name(e),Fnv(nm,strnlen(nm,24)),V(pos),Name(m)}); }
inline void __fastcall FakeDestroyEntityFx(void* fx,void*,void* e){ T({"destroy_entity_fx",uintptr_t(fx)==ADDR_GFX,Name(e)}); }
inline void __fastcall FakeBrainReq(void* s,void*,const char* nm){ T({"brain_request",uintptr_t(s)==ADDR_BRAINS,Fnv(nm,8)}); }
inline void __fastcall FakeBrainMark(void* s,void*,const char* nm){ T({"brain_mark_not_needed",uintptr_t(s)==ADDR_BRAINS,Fnv(nm,8)}); }
inline int32 __cdecl FakeEnexAdd(float x,float y,float z,float a,float rx,float ry,float un,float ex,float ey,float ez,float ea,int32 area,uint32 flags,int32 sky,int32 on,int32 off,int32 peds,const char* nm){
    T({"enex_add_one",V3(x,y,z),F(a),F(rx),F(ry),F(un),V3(ex,ey,ez),F(ea),area,flags,sky,on,off,peds,Name(nm)}); return cfg.enexAdd; }
inline int32 __cdecl FakeEnexNearest(const CVector2D* p,float range,int32 ign){ T({"enex_find_nearest",F(p->x),F(p->y),F(range),ign}); return cfg.enexNear; }
inline void __cdecl FakeEnexDelete(int32 id){ T({"enex_delete_one",id}); }
inline void __cdecl FakeEscAdd(const CVector* s,const CVector* b,const CVector* t,const CVector* e,uint32 down,void* e2){ T({"escalator_add_one",V(s),V(b),V(t),V(e),down&0xFF,Name(e2)}); }
inline void __fastcall FakeEscSwitchOff(void* esc,void*){ T({"escalator_switch_off",(intptr_t(esc)-intptr_t(ADDR_ESCAL))/int(ESC_SZ)}); }
inline void* __cdecl FakeGetAttrMgr(){ T({"get_ped_attractor_manager"}); return attrMgr.data(); }
inline bool __fastcall FakeHasEmptySlot(void* mgr,void*,const void* fx,const void* e){ T({"has_empty_slot",Name(mgr),Name(fx),Name(e)}); return (cfg.emptyMask>>(cfg.emptyCalls++%16))&1; }
inline void __fastcall FakeAllocMatrix(void* self,void*){ T({"allocate_matrix",Name(self)}); At<void*>(self,0x14)=matP.data();
    Rng r{}; r.st=cfg.matSeed|1; std::memset(matP.data(),0,0x48); for(size_t o:{0x00,0x04,0x08,0x10,0x14,0x18,0x20,0x24,0x28})At<float>(matP.data(),o)=r.uni(-1.f,1.f);
    At<CVector>(matP.data(),0x30)=*reinterpret_cast<CVector*>(static_cast<uint8*>(self)+4); }
inline void __fastcall FakeUpdateMatrixFromTransform(void* tr,void*,void* m){ T({"simple_transform_update_matrix",Name(tr),Name(m),F(At<float>(tr,0xC))}); }

// luces del renderer
inline void __cdecl FakeActivateDir(){ T({"activate_directional"}); }
inline void __cdecl FakeDeactivateDir(){ T({"deactivate_directional"}); }
inline void __cdecl FakeAmbient(){ T({"set_ambient_colours"}); }
inline float __cdecl FakeGenLights(const CVector* pt,float* total,void* e){ T({"generate_lights_affecting_object",V(pt),Name(total),Name(e)}); return cfg.lightsRet; }
inline void __cdecl FakeSetLightColours(float m){ T({"set_light_colours_for_peds_cars_objects",F(m)}); }
inline void __cdecl FakeRemoveLights(){ T({"remove_lights_affecting_object"}); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=16;
inline const std::array<Fn,NFNS> fns{{
    {"CEntity","CreateEffects",0x533790,0,-1,512},
    {"CEntity","DestroyEffects",0x533BF0,0,-1,384},
    {"CEntity","GetRandom2dEffect",0x533410,0,-1,256},
    {"CEntity","HasPreRenderEffects",0x532B70,0,-1,512},
    {"CEntity","ProcessLightsForEntity",0x6FC7A0,0,-1,1024},
    {"CEntity","PreRender",0,0x863928,17,768},
    {"CEntity","PreRenderForGlassWindow",0x533240,0,-1,64},
    {"CEntity","Render",0,0x863928,18,384},
    {"CEntity","RenderEffects",0x5342B0,0,-1,128},
    {"CEntity","RemoveEscalatorsForEntity",0x717900,0,-1,128},
    {"CEntity","RemoveLighting",0,0x863928,20,64},
    {"CEntity","SetupLighting",0,0x863928,19,64},
    {"CEntity","SetAtomicAlpha",0x533290,0,-1,64},
    {"CEntity","SetCompAlphaCB",0x533280,0,-1,64},
    {"CEntity","SetRwObjectAlpha",0x5332C0,0,-1,128},
    {"CEntity","MaterialUpdateUVAnimCB",0x532D70,0,-1,64},
}};

// matriz de CMatrix (0x48): ejes ortonormales aproximados y posición en 0x30 (relleno 0x0C/0x1C/0x2C/0x3C a cero)
inline void FillMat(Rng& r,uint8* m,const CVector& pos,float upz=0.f){
    std::memset(m,0,0x48); const float a=r.uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
    At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,upz!=0.f ? upz : (r.coin(5) ? -1.f : 1.f)}; At<CVector>(m,0x30)=pos; }
// ejes aleatorios (para productos escalares con orden de sumas)
inline void FillMatRand(Rng& r,uint8* m,const CVector& pos,float upz){
    std::memset(m,0,0x48); At<CVector>(m,0x00)=r.vec(1.f); At<CVector>(m,0x10)=r.vec(1.f); At<CVector>(m,0x20)=r.vec(1.f); At<float>(m,0x28)=upz; At<CVector>(m,0x30)=pos; }

// --- valores de ModelIndices leídos del EXE (si no están cargados se sustituyen por un índice genérico) ---
constexpr std::array<uintptr_t,33> MI_ADDRS{ 0x8CD598,0x8CD59C,0x8CD5A0,0x8CD5A4,0x8CD5AC,0x8CD638,0x8CD6E0,0x8CD71C,0x8CD700,0x8CD740,0x8CD6FC,0x8CD718,0x8CD75C,
    0x8CD4F4,0x8CD4F8,0x8CD4FC,0x8CD500,0x8CD504,0x8CD508,0x8CD50C,0x8CD510,0x8CD514,0x8CD518,0x8CD51C,0x8CD520,0x8CD524,0x8CD644,0x8CD648,0x8CD4F4,0x8CD518,0x8CD524,0x8CD75C,0x8CD638 };
inline int32 MiVal(size_t k){ const uint16 v=AtA<uint16>(MI_ADDRS[k%MI_ADDRS.size()]); return (v>0&&v<20000) ? int32(v) : 777; }

inline json CbName(void* cb){
    static uintptr_t dSet{}, dAtom{}, dUv{};
    if(!dSet){ dSet=DllBody("CEntity","SetCompAlphaCB"); dAtom=DllBody("CEntity","SetAtomicAlpha"); dUv=DllBody("CEntity","MaterialUpdateUVAnimCB"); }
    const uintptr_t c=uintptr_t(cb), fc=FollowIn(c);
    if(c==0x533280||(dSet&&fc==dSet))return "SetCompAlphaCB";
    if(c==0x533290||(dAtom&&fc==dAtom))return "SetAtomicAlpha";
    if(c==0x532D70||(dUv&&fc==dUv))return "MaterialUpdateUVAnimCB";
    return c; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"centc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"centc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("centc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); InitModule();
    std::string only; { std::ifstream f(dir/"centc-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // vtables falsas
    FillVt(entVt,std::make_index_sequence<64>{}); FillVt(miVt,std::make_index_sequence<64>{});
    miVt[1]=reinterpret_cast<void*>(&MiAsAtomic); miVt[4]=reinterpret_cast<void*>(&MiModelType);

    // nombres
    named.clear();
    Reg(ent.data(),ent.size(),"ent"); Reg(matP.data(),matP.size(),"matP"); Reg(matCam.data(),matCam.size(),"matCam");
    for(int k=0;k<2;++k){ Reg(Clump(k),0x400,"clump"+std::to_string(k)); Reg(texts[size_t(k)].data(),64,"text"+std::to_string(k)); }
    for(int k=0;k<6;++k){ Reg(Frame(k),0x100,"frame"+std::to_string(k)); Reg(AtomB(k),0x60,"atom"+std::to_string(k)); Reg(GeomB(k),0x40,"geom"+std::to_string(k)); }
    for(int k=0;k<4;++k){ Reg(MatB(k),0x20,"mat"+std::to_string(k)); Reg(Mi(k),0x60,"mi"+std::to_string(k)); }
    Reg(fx2d.data(),sizeof(fx2d),"fx",0x40); Reg(lnk.data(),lnk.size(),"lnk"); Reg(attrMgr.data(),attrMgr.size(),"attrMgr");
    Reg(texA.data(),texA.size(),"texA"); Reg(texB.data(),texB.size(),"texB"); Reg(texC.data(),texC.size(),"texC");
    Reg(eePool.data(),eePool.size(),"eePool"); Reg(eeObjs.data(),eeObjs.size(),"eeObj",0x3C); Reg(eeMap.data(),eeMap.size(),"eeMap");
    Reg(entVt.data(),sizeof(entVt),"entVt"); Reg(miVt.data(),sizeof(miVt),"miVt");
    Reg(reinterpret_cast<void*>(ADDR_ESCAL),NESC*ESC_SZ,"escalator",ESC_SZ);

    // estado global guardado y restaurado en cada ejecución
    auto** const miPtrs=reinterpret_cast<void**>(ADDR_MI_PTRS);
    std::vector<std::pair<int,void*>> miTouched;
    struct Saved { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Saved> regions;
    auto SnapG=[&](uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); };
    const uintptr_t rsSlot=AtA<uintptr_t>(ADDR_ENGINE)+0x20;
    SnapG(ADDR_TIMESTEP,4); SnapG(ADDR_FRAMECTR,4); SnapG(ADDR_TIMEMS,4); SnapG(ADDR_CAMPOS,0x10); SnapG(ADDR_CAMMAT,4); SnapG(ADDR_CAMRIGHT,0x10);
    SnapG(ADDR_SUNGLARE,4); SnapG(ADDR_WET,4); SnapG(ADDR_RAIN,4); SnapG(ADDR_TCSTORED,4); SnapG(ADDR_VECSUN,0xC0); SnapG(ADDR_TCCOLOURS,0x60);
    SnapG(ADDR_REPLAY,1); SnapG(ADDR_TREEDIST,4); SnapG(rsSlot,8); SnapG(ADDR_ESCAL,NESC*ESC_SZ); SnapG(ADDR_EEPOOL,4); SnapG(ADDR_CLOCKH,2);
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
        std::set<uintptr_t> stubSet;
        auto P=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto stub){
            const uintptr_t st=Addr(stub), stb=FollowIn(st); const uintptr_t body=dllAddr ? FollowIn(dllAddr) : 0;
            stubSet.insert(st); stubSet.insert(stb);
            auto ok=[&](uintptr_t a){ return a&&!stubSet.count(a); };
            if(ok(exeAddr))p.Add(exeAddr,stub); if(ok(dllAddr))p.Add(dllAddr,stub); if(ok(body))p.Add(body,stub); };
        // azar
        P(0x821B1E,0,FakeRand); P(0,Addr(&CGeneral::GetRandomNumber),FakeRandomNumber); P(0x821B11,Addr(&srand),FakeSrand);
        // modelos y efectos
        P(0x4C4C70,Addr(&CBaseModelInfo::Get2dEffect),FakeGet2dEffect);
        P(0x6FAB30,Addr(&GetDayNightBalance),FakeDayNight);
        P(0x5334F0,Addr(static_cast<CVector(CEntity::*)(const CVector&) const>(&CEntity::TransformFromObjectSpace)),FakeTfos);
        P(0x533560,Addr(static_cast<CVector*(CEntity::*)(CVector&,const CVector&) const>(&CEntity::TransformFromObjectSpace)),FakeTfos);
        P(0x6FC180,Addr(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,RwTexture*,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona)),FakeCorona);
        P(0x6FC4D0,Addr(&CCoronas::UpdateCoronaCoors),FakeCoronaCoors);
        P(0x7000E0,Addr(&CPointLights::AddLight),FakeAddLight);
        P(0x70BA00,Addr(&CShadows::StoreStaticShadow),FakeStaticShadow);
        P(0x707390,Addr(static_cast<void(*)(uint8,RwTexture*,const CVector&,float,float,float,float,int16,uint8,uint8,uint8,float,bool,float,CRealTimeShadow*,bool)>(&CShadows::StoreShadowToBeRendered)),FakeShadowToBeRendered);
        P(0,Addr(static_cast<void(*)(eShadowType,RwTexture*,const CVector&,CVector2D,CVector2D,int16,uint8,uint8,uint8,float,bool,float,CRealTimeShadow*,bool)>(&CShadows::StoreShadowToBeRendered)),FakeShadowToBeRendered);
        P(0x70C750,Addr(&CShadows::StoreShadowForPole),FakeShadowForPole);
        P(0x721DC0,Addr(&CMotionBlurStreaks::RegisterStreak),FakeStreak);
        P(0x6E8580,Addr(&CWaterLevel::GetWaterLevelNoWaves),FakeWaterNoWaves);
        P(0x441C10,Addr(&CGameLogic::LaRiotsActiveHere),FakeRiots); P(0x41BC90,Addr(&CBridge::ShouldLightsBeFlashing),FakeBridge);
        P(0x49DAB0,Addr(&CTrafficLights::DisplayActualLight),FakeDisplayLight); P(0x49D5A0,Addr(&CTrafficLights::IsMITrafficLight),FakeIsMITL);
        P(0x455E20,Addr(&CPickups::DoCollectableEffects),FakeCollectable); P(0x454E80,Addr(&CPickups::DoMoneyEffects),FakeMoney);
        P(0x4560E0,Addr(&CPickups::DoMineEffects),FakeMine); P(0x455720,Addr(&CPickups::DoPickUpEffects),FakePickUp);
        P(0x46A760,Addr(&CGlass::IsObjectGlass),FakeIsGlass); P(0x71ACD0,Addr(&CGlass::AskForObjectToBeRenderedInGlass),FakeAskGlass);
        P(0x534E90,Addr(&CEntity::ModifyMatrixForTreeInWind),FakeTreeWind); P(0x533170,Addr(&CEntity::ModifyMatrixForCrane),FakeCrane);
        P(0x532DB0,Addr(&CEntity::BuildWindSockMatrix),FakeWindSock);
        P(0x446F90,Addr(&CEntity::UpdateRwMatrix),FakeUpdateRwMatrix); // UpdateRwFrame no se simula: el EXE lo expande en linea (RwFrameUpdateObjects) en el caso del coleccionable
        // RenderWare y dibujo
        P(0x7F0910,Addr(&RwFrameUpdateObjects),FakeFrameUpdate); P(0x7F10B0,Addr(&RwFrameSetIdentity),FakeFrameIdentity);
        P(0x7F1010,Addr(&RwFrameRotate),FakeFrameRotate); P(0x7F0E30,Addr(&RwFrameTranslate),FakeFrameTranslate);
        P(0x74BF20,Addr(&RpAtomicSetFrame),FakeAtomicSetFrame); P(0x7F05A0,Addr(&RwFrameDestroy),FakeFrameDestroy); P(0x749DC0,Addr(&RpAtomicDestroy),FakeAtomicDestroy);
        P(0x811C30,Addr(&RpMatFXAtomicQueryEffects),FakeMatFx);
        P(0x74C790,Addr(&RpGeometryForAllMaterials),FakeForAllMaterials); P(0x749B70,Addr(&RpClumpForAllAtomics),FakeForAllAtomics);
        P(0x749B20,Addr(&RpClumpRender),FakeClumpRender);
        P(0x7CC530,Addr(&RpMaterialUVAnimExists),FakeUvExists); P(0x7CC4B0,Addr(&RpMaterialUVAnimAddAnimTime),FakeUvAddTime); P(0x7CC110,Addr(&RpMaterialUVAnimApplyUpdate),FakeUvApply);
        P(0x5D7200,Addr(static_cast<void(*)(RpAtomic*,bool)>(&CCustomBuildingDNPipeline::PreRenderUpdate)),FakeDnAtomic);
        P(0x5D72C0,Addr(static_cast<void(*)(RpClump*,bool)>(&CCustomBuildingDNPipeline::PreRenderUpdate)),FakeDnClump);
        P(0x49CCE0,Addr(&CTagManager::IsTag),FakeIsTag); P(0x49CE40,Addr(&CTagManager::RenderTagForPC),FakeRenderTag);
        P(0x4096D0,Addr(&CStreaming::RenderEntity),FakeStreamRender);
        P(0x6FF350,Addr(&CCustomRoadsignMgr::RenderRoadsignAtomic),FakeRenderRoadsign); P(0x6FF2D0,Addr(&CCustomRoadsignMgr::CreateRoadsignAtomic),FakeCreateRoadsign);
        P(0x6FA640,Addr(&C2dEffect::Roadsign_GetNumLinesFromFlags),FakeRsLines); P(0x6FA670,Addr(&C2dEffect::Roadsign_GetNumLettersFromFlags),FakeRsLetters);
        P(0x6FA6A0,Addr(&C2dEffect::Roadsign_GetPaletteIDFromFlags),FakeRsPalette);
        // Fx, scripts, entradas/salidas, escaleras, atractores, matrices
        P(0x4A11E0,Addr(&Fx_c::CreateEntityFx),FakeCreateEntityFx); P(0x4A1280,Addr(&Fx_c::DestroyEntityFx),FakeDestroyEntityFx);
        P(0x46AA80,Addr(&CScriptsForBrains::RequestAttractorScriptBrainWithThisName),FakeBrainReq);
        P(0x46AAE0,Addr(&CScriptsForBrains::MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded),FakeBrainMark);
        P(0x43FA00,Addr(&CEntryExitManager::AddOne),FakeEnexAdd); P(0x43F4B0,Addr(&CEntryExitManager::FindNearestEntryExit),FakeEnexNearest);
        P(0x43FD50,Addr(&CEntryExitManager::DeleteOne),FakeEnexDelete);
        P(0x717C90,Addr(&CEscalators::AddOne),FakeEscAdd); P(0x717860,Addr(&CEscalator::SwitchOff),FakeEscSwitchOff);
        P(0x5EE190,Addr(&GetPedAttractorManager),FakeGetAttrMgr); P(0x5EBB00,Addr(&CPedAttractorManager::HasEmptySlot),FakeHasEmptySlot);
        P(0x54F560,Addr(&CPlaceable::AllocateMatrix),FakeAllocMatrix); P(0x54F1B0,Addr(&CSimpleTransform::UpdateMatrix),FakeUpdateMatrixFromTransform);
        // luces del renderer
        P(0x735C80,Addr(&ActivateDirectional),FakeActivateDir); P(0x735C70,Addr(&DeActivateDirectional),FakeDeactivateDir);
        P(0x735D30,Addr(static_cast<void(*)()>(&SetAmbientColours)),FakeAmbient);
        P(0x6FFBB0,Addr(&CPointLights::GenerateLightsAffectingObject),FakeGenLights); P(0x735D90,Addr(&SetLightColoursForPedsCarsAndObjects),FakeSetLightColours);
        P(0x6FFFE0,Addr(&CPointLights::RemoveLightsAffectingObject),FakeRemoveLights);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate,bool detail)->json {
                    Rng r{}; r.st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xCCCCu; if(!r.st)r.st=1; trace=json::array(); rngState=uint32(i)*7919u+uint32(f);
                    restoreAll(); cfg=Cfg{};
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // globales comunes
                    AtA<float>(ADDR_TIMESTEP)=r.uni(0.2f,3.f); AtA<uint32>(ADDR_FRAMECTR)=r.next(); AtA<uint32>(ADDR_TIMEMS)=r.next()&0x3FFFFFFFu;
                    AtA<uintptr_t>(rsSlot)=bit_cast<uintptr_t>(&RSSet); AtA<uintptr_t>(rsSlot+4)=bit_cast<uintptr_t>(&RSGet);
                    std::fill(thunkRet.begin(),thunkRet.end(),0);
                    cfg.alphaRef=r.next(); cfg.matSeed=r.next(); cfg.dnb=r.pickF({0.f,0.25f,0.5f,0.9f,1.f,0.f});
                    cfg.riots=r.coin(6); cfg.bridge=r.coin(2); cfg.glass=r.coin(3); cfg.mitl=r.coin(4); cfg.isTag=r.coin(6); cfg.matfx=r.coin(2); cfg.uvExists=r.coin(2);
                    cfg.nMats=int32(r.next()%4); cfg.nAtoms=int32(r.next()%4); cfg.emptyMask=r.next(); cfg.lightsRet=r.uni(0.f,4.f);
                    // búferes
                    for(auto& b:clumps)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:frames)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:atoms)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:geoms)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:mats)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:mis){ std::memset(b.data(),0xCD,b.size()); At<void*>(b.data(),0)=miVt.data(); }
                    for(auto& b:fx2d)std::memset(b.data(),0xCD,b.size());
                    for(auto& b:texts){ std::memset(b.data(),0,b.size()); for(int k=0;k<40;++k)b[size_t(k)]=char('A'+r.next()%26); }
                    for(auto* b:{lnk.data(),attrMgr.data(),texA.data(),texB.data(),texC.data()})std::memset(b,0xCD,0x40);
                    std::memset(eePool.data(),0xCD,eePool.size()); std::memset(eeObjs.data(),0xCD,eeObjs.size()); std::memset(eeMap.data(),0,eeMap.size());
                    std::memset(matP.data(),0xCD,0x48); std::memset(matCam.data(),0xCD,0x48);
                    // objetos RW: clumps (tipo 2), atomics (tipo 1) con frame, geometría y callback de dibujo, materiales
                    for(int k=0;k<2;++k){ Clump(k)[0]=2; Clump(k)[1]=0; At<void*>(Clump(k),4)=Frame(k); }
                    for(int k=0;k<6;++k){ AtomB(k)[0]=1; AtomB(k)[1]=0; At<void*>(AtomB(k),4)=Frame(k); At<void*>(AtomB(k),0x18)=GeomB(k); At<void*>(AtomB(k),0x48)=reinterpret_cast<void*>(&FakeAtomicRenderCb);
                        At<uint32>(GeomB(k),8)=r.next(); }
                    for(int k=0;k<4;++k)At<uint32>(MatB(k),4)=r.next();
                    // entidad base
                    uint8* const e=ent.data(); std::memset(e,0xCD,ent.size());
                    const int etype=int(r.pick({1,4,4,5,5,2,3,4}));
                    const CVector pos=r.vec(60.f);
                    At<void*>(e,0x00)=entVt.data(); At<CVector>(e,0x04)=pos; At<float>(e,0x10)=r.uni(-3.2f,3.2f);
                    if(r.coin(2)){ FillMat(r,matP.data(),pos+(r.coin(2) ? CVector{0.f,0.f,0.f} : r.vec(1.f))); At<void*>(e,0x14)=matP.data(); } else At<void*>(e,0x14)=nullptr;
                    At<void*>(e,0x18)=nullptr;
                    At<uint32>(e,0x1C)=r.next()&~(0x80u|0x200u|0x2000u|0x200000u|0x1000000u|0x10000000u); At<uint32>(e,0x1C)|=0x80u;
                    At<uint16>(e,0x20)=uint16(r.next()); At<uint16>(e,0x22)=uint16(MiVal(r.next())); At<void*>(e,0x24)=nullptr; At<void*>(e,0x28)=lnk.data();
                    e[0x36]=uint8((r.next()&0xF8)|uint32(etype)); At<uint32>(e,0x40)=r.next()&~0x20000000u;
                    At<float>(e,0x44)=r.uni(-0.2f,0.2f); At<float>(e,0x48)=r.uni(-0.2f,0.2f); At<float>(e,0x4C)=r.uni(-0.2f,0.2f);
                    At<uint32>(e,0x140)=r.next()&~(0x1u|0x40u|0x800u);
                    // cámara y reloj del juego
                    const CVector cp=r.vec(40.f); AtA<uintptr_t>(ADDR_CAMMAT)=r.coin(3) ? uintptr_t(matCam.data()) : 0; FillMat(r,matCam.data(),cp);
                    At<CVector>(reinterpret_cast<void*>(ADDR_CAMPOS),0)=cp; At<CVector>(reinterpret_cast<void*>(ADDR_CAMRIGHT),0)=r.vec(1.f);
                    AtA<uint8>(ADDR_REPLAY)=uint8(r.coin(4));
                    // tablas del ciclo horario y clima
                    AtA<float>(ADDR_SUNGLARE)=r.pickF({-1.f,0.f,0.5f,1.f,2.f}); AtA<float>(ADDR_WET)=r.pickF({0.f,0.5f,0.6f,1.f}); AtA<float>(ADDR_RAIN)=r.pickF({0.f,0.00005f,0.0001f,0.001f,0.5f});
                    AtA<uint32>(ADDR_TCSTORED)=uint32(r.next()%16); for(size_t k=0;k<16;++k)At<CVector>(reinterpret_cast<void*>(ADDR_VECSUN),k*12)=r.vec(1.f);
                    At<uint16>(reinterpret_cast<void*>(ADDR_TCCOLOURS),0x30)=uint16(r.next()%300); At<uint16>(reinterpret_cast<void*>(ADDR_TCCOLOURS),0x32)=uint16(r.next()%300);
                    At<uint16>(reinterpret_cast<void*>(ADDR_TCCOLOURS),0x34)=uint16(r.next()%300);
                    At<float>(reinterpret_cast<void*>(ADDR_TCCOLOURS),0x44)=r.pickF({0.f,0.5f,1.f,3.f,10.f,r.uni(0.f,5.f)});
                    At<int16>(reinterpret_cast<void*>(ADDR_TCCOLOURS),0x48)=int16(r.pick({0,100,255,256,300,-1,0x1234}));
                    AtA<uint8>(ADDR_CLOCKH)=uint8(r.next()%24); AtA<uint8>(ADDR_CLOCKH+1)=uint8(r.pick({0,1,5,6}));
                    AtA<float>(ADDR_TREEDIST)=r.pickF({999999.875f,10.f,0.f,5.f,50.f});

                    // fixtures de efectos 2D: tipos, campos y atómicos de carteles
                    auto mkEffect=[&](int k,int type){
                        uint8* fe=FxE(k); At<CVector>(fe,0)=r.vec(5.f); fe[0xC]=uint8(type);
                        switch(type){
                        case 0: { // luz
                            fe[0x10]=uint8(r.pick({0,1,50,255,128})); fe[0x11]=uint8(r.pick({0,2,60,255,128})); fe[0x12]=uint8(r.pick({0,3,70,255,128})); fe[0x13]=uint8(r.next());
                            At<float>(fe,0x14)=r.uni(10.f,300.f); At<float>(fe,0x18)=r.pickF({0.f,0.f,5.f,20.f}); At<float>(fe,0x1C)=r.uni(0.1f,3.f); At<float>(fe,0x20)=r.pickF({0.f,0.f,1.f,2.5f});
                            uint16 fl=uint16(r.next()); if(r.coin(3))fl&=~0x60; if(r.coin(2))fl&=~0x1000; if(r.coin(2))fl&=~0x800; At<uint16>(fe,0x24)=fl;
                            fe[0x26]=uint8(r.pick({0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,255,1,2,0})); fe[0x27]=uint8(r.pick({0,1,2,255})); fe[0x28]=uint8(r.pick({0,1,2,9}));
                            fe[0x29]=uint8(r.next()); fe[0x2A]=uint8(r.pick({0,15,127,128,200,255})); fe[0x2B]=uint8(r.next()); fe[0x2C]=uint8(r.next()); fe[0x2D]=uint8(r.next());
                            At<void*>(fe,0x30)=r.coin(4) ? nullptr : static_cast<void*>(texA.data()); At<void*>(fe,0x34)=r.coin(4) ? nullptr : static_cast<void*>(texB.data()); break; }
                        case 1: { // partícula
                            for(int k2=0;k2<24;++k2)fe[0x10+k2]=uint8('a'+r.next()%26); fe[0x10+r.next()%23+1]=0; break; }
                        case 3: { // atractor
                            fe[0x34]=uint8(int8(r.pick({-1,0,1,5,5,7,9}))); for(int k2=0;k2<8;++k2)fe[0x38+k2]=uint8('a'+r.next()%26); break; }
                        case 6: { // entrada/salida
                            At<float>(fe,0x10)=r.uni(-180.f,180.f); At<float>(fe,0x14)=r.uni(0.5f,3.f); At<float>(fe,0x18)=r.uni(0.5f,3.f); At<CVector>(fe,0x1C)=r.vec(10.f); At<float>(fe,0x28)=r.uni(-180.f,180.f);
                            At<int16>(fe,0x2C)=int16(r.pick({0,1,-1,5,18})); fe[0x2E]=uint8(r.next()); fe[0x2F]=uint8(r.next()); for(int k2=0;k2<8;++k2)fe[0x30+k2]=uint8('A'+r.next()%26);
                            fe[0x38]=uint8(r.pick({0,6,12,20,23,5})); fe[0x39]=uint8(r.pick({0,6,12,20,23,5})); fe[0x3A]=uint8(r.pick({0,4,6,7,1})); break; }
                        case 7: { // cartel
                            At<float>(fe,0x10)=r.uni(0.5f,4.f); At<float>(fe,0x14)=r.uni(0.5f,4.f); At<float>(fe,0x18)=r.uni(-3.f,3.f); At<float>(fe,0x1C)=r.uni(-3.f,3.f); At<float>(fe,0x20)=r.uni(-3.f,3.f);
                            At<uint16>(fe,0x24)=uint16(r.next()); At<void*>(fe,0x28)=texts[size_t(r.next()%2)].data();
                            At<void*>(fe,0x2C)=r.coin(3) ? nullptr : static_cast<void*>(AtomB(int(2+r.next()%2))); break; }
                        case 8: { At<int32>(fe,0x10)=r.pick({0,1,2,3}); break; }
                        case 10: { // escalera
                            At<CVector>(fe,0x10)=r.vec(8.f); At<CVector>(fe,0x1C)=r.vec(8.f); At<CVector>(fe,0x28)=r.vec(8.f); fe[0x34]=uint8(r.pick({0,1,2})); break; }
                        default: for(size_t o=0x10;o<0x40;o+=4)At<uint32>(fe,o)=r.next()&0x3FFFFFFFu; break; // 2, 4, 5, 9, otros
                        } };
                    auto mkEffects=[&](uint8* mi,int count,std::initializer_list<int32> types){ At<uint8>(mi,0xD)=uint8(count); for(int k=0;k<count;++k)mkEffect(k,int(r.pick(types))); };
                    auto genericEffects=[&](uint8* mi){ mkEffects(mi,int(r.next()%7),{0,0,0,1,3,4,6,7,8,10,2,5,9,11,200,0,4}); };
                    // pool de entradas/salidas falso
                    auto mkEnexPool=[&]{
                        At<void*>(eePool.data(),0)=eeObjs.data(); At<void*>(eePool.data(),4)=eeMap.data(); At<uint32>(eePool.data(),8)=8; At<int32>(eePool.data(),0xC)=-1; At<uint8>(eePool.data(),0x10)=0;
                        for(int k=0;k<8;++k){ uint8* o=eeObjs.data()+size_t(k)*0x3C; uint16 fl=uint16(r.next()); if(r.coin(2))fl|=0x2000; else fl&=~0x2000; if(r.coin(2))fl|=0x4000; else fl&=~0x4000; fl&=~0x8000;
                            At<uint16>(o,0x30)=fl; At<void*>(o,0x38)=r.coin(2) ? nullptr : static_cast<void*>(eeObjs.data()+size_t(r.next()%8)*0x3C); eeMap[size_t(k)]=uint8(r.next()&0x7F); }
                        AtA<uintptr_t>(ADDR_EEPOOL)=uintptr_t(eePool.data()); };

                    // -------------------------------------------------------------------------------------------------
                    if(nm=="HasPreRenderEffects") {
                        const int32 idx=MiVal(r.next()); const int32 pickIdx=r.coin(5) ? int32(r.pick({342,344,345,777,1234})) : idx;
                        At<uint16>(e,0x22)=uint16(pickIdx); uint8* mi=Mi(0); setMi(pickIdx,mi);
                        At<uint16>(mi,0x12)=uint16((uint32(r.pick({0,0,1,2,9,5,4,3,8}))<<11)|(r.next()&0x07FF));
                        if(r.coin(2))mkEffects(mi,int(r.next()%5),{1,3,7,0,4,10}); else genericEffects(mi);
                        if(r.coin(3))At<uint32>(e,0x140)|=1u;
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(e)&true);
                    }
                    else if(nm=="GetRandom2dEffect") {
                        const int32 idx=777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        const int count=r.coin(6) ? int(30+r.next()%20) : int(r.next()%10); At<uint8>(mi,0xD)=uint8(count);
                        for(int k=0;k<count;++k)mkEffect(k,int(r.pick({0,1,3,3,3,5,7,3,3,10,2})));
                        const int32 effType=r.pick({3,3,3,0,1,7,10,5,4,200});
                        ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,bool)>(a)(e,effType,r.coin(2)));
                    }
                    else if(nm=="PreRenderForGlassWindow") {
                        const int32 idx=777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        At<uint16>(mi,0x12)=uint16((uint32(r.pick({5,5,4,0,9}))<<11)|(r.next()&0x07FF));
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                    }
                    else if(nm=="SetRwObjectAlpha") {
                        const int kind=int(r.pick({0,1,1,2,2,3,4,5}));
                        if(kind==1)At<void*>(e,0x18)=AtomB(0);
                        else if(kind==2)At<void*>(e,0x18)=Clump(0);
                        else if(kind==3){ At<void*>(e,0x18)=Clump(1); Clump(1)[0]=0; }
                        else if(kind==4){ At<void*>(e,0x18)=Clump(1); Clump(1)[0]=3; }
                        else if(kind==5){ At<void*>(e,0x18)=Clump(1); Clump(1)[0]=255; }
                        reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(e,r.pick({0,1,127,128,255,256,300,-1,-256,0x12345678}));
                    }
                    else if(nm=="SetAtomicAlpha") {
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(AtomB(int(r.next()%2)),reinterpret_cast<void*>(uintptr_t(uint32(r.pick({0,1,127,128,255,256,300,-1,0x12345678}))))));
                    }
                    else if(nm=="SetCompAlphaCB") {
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(MatB(int(r.next()%2)),reinterpret_cast<void*>(uintptr_t(uint32(r.pick({0,1,127,128,255,256,300,-1,0x12345678}))))));
                    }
                    else if(nm=="MaterialUpdateUVAnimCB") {
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(MatB(int(r.next()%2)),nullptr));
                    }
                    else if(nm=="RemoveEscalatorsForEntity") {
                        for(size_t k=0;k<NESC;++k){ uint8* esc=reinterpret_cast<uint8*>(ADDR_ESCAL)+k*ESC_SZ; std::memset(esc,0xCD,ESC_SZ);
                            esc[0x78]=uint8(r.coin(3) ? 0 : 1); At<void*>(esc,0xA4)=r.coin(3) ? static_cast<void*>(e) : reinterpret_cast<void*>(uintptr_t(0x1000+k*16)); esc[0x79]=uint8(r.next()); }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["escalators"]=Snap(reinterpret_cast<void*>(ADDR_ESCAL),NESC*ESC_SZ,detail);
                    }
                    else if(nm=="RemoveLighting") {
                        reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(e,r.coin(2));
                    }
                    else if(nm=="SetupLighting") {
                        if(r.coin(4))At<uint32>(e,0x1C)&=~0x10000000u; else At<uint32>(e,0x1C)|=0x10000000u;
                        ret=int(reinterpret_cast<bool(__thiscall*)(void*)>(a)(e)&true);
                    }
                    else if(nm=="RenderEffects") {
                        const int32 idx=777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        if(r.coin(5))At<uint32>(e,0x1C)&=~0x1000000u; else At<uint32>(e,0x1C)|=0x1000000u;
                        mkEffects(mi,int(r.next()%6),{7,7,0,1,3,10,7,2});
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["fx"]=Snap(fx2d.data(),sizeof(fx2d),detail);
                    }
                    else if(nm=="Render") {
                        const int kind=int(r.pick({1,1,2,2,0,1}));
                        if(kind==1)At<void*>(e,0x18)=AtomB(0); else if(kind==2)At<void*>(e,0x18)=Clump(0);
                        const int32 idx=r.coin(3) ? int32(AtA<uint16>(r.coin(2) ? 0x8CD644 : 0x8CD648)) : 777;
                        const int32 mIdx=(idx>0&&idx<20000) ? idx : 777; At<uint16>(e,0x22)=uint16(mIdx); uint8* mi=Mi(0); setMi(mIdx,mi);
                        if(r.coin(3))At<uint32>(e,0x1C)|=0x2000u; if(r.coin(2))At<uint32>(e,0x1C)|=0x1000000u;
                        mkEffects(mi,int(r.next()%4),{7,7,0,1,10,2});
                        ret=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["atom"]=Snap(AtomB(0),0x60,detail); out["flags"]=At<uint32>(e,0x1C);
                    }
                    else if(nm=="CreateEffects") {
                        const int32 idx=777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        At<uint32>(e,0x1C)|=0x1000000u&r.next(); At<uint32>(e,0x1C)|=0x200000u&r.next();
                        if(r.coin(2)){ At<void*>(e,0x18)=AtomB(1); } // el objeto RW da la matriz de modelado (frame+0x10)
                        mkEnexPool();
                        cfg.enexAdd=r.coin(4) ? -1 : int32(r.next()%8); cfg.newAtomic=uintptr_t(AtomB(int(2+r.next()%2)));
                        genericEffects(mi);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["fx"]=Snap(fx2d.data(),sizeof(fx2d),detail); out["pool"]=Snap(eeObjs.data(),eeObjs.size(),detail);
                    }
                    else if(nm=="DestroyEffects") {
                        const int32 idx=777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        mkEnexPool(); cfg.enexNear=r.coin(3) ? -1 : int32(r.next()%8);
                        genericEffects(mi);
                        for(int k=0;k<NFX;++k)if(FxE(k)[0xC]==7&&r.coin(3))At<void*>(FxE(k),0x2C)=r.coin(2) ? static_cast<void*>(AtomB(int(2+r.next()%2))) : nullptr;
                        if(r.coin(3))At<void*>(AtomB(2),4)=nullptr; if(r.coin(3))At<void*>(AtomB(3),4)=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["fx"]=Snap(fx2d.data(),sizeof(fx2d),detail); out["pool"]=Snap(eeObjs.data(),eeObjs.size(),detail);
                        out["atoms"]=Snap(atoms.data(),sizeof(atoms),detail);
                    }
                    else if(nm=="ProcessLightsForEntity") {
                        const int32 idx=r.coin(8) ? 464 : 777; uint8* mi=Mi(0); setMi(idx,mi); At<uint16>(e,0x22)=uint16(idx);
                        if(r.coin(10))At<uint32>(e,0x1C)|=0x200u;
                        if(r.coin(12))At<uint32>(e,0x1C)&=~0x80u;
                        if(r.coin(8))At<uint32>(e,0x40)|=0x20000000u;
                        if(r.coin(2))At<uint32>(e,0x140)|=0x800u;
                        // matriz con inclinación variable alrededor del límite 0,96
                        const float upz=r.pickF({1.f,0.95f,0.96f,0.97f,-1.f,0.99f,0.5f,1.f,1.f});
                        if(At<void*>(e,0x14)||r.coin(3)){ FillMatRand(r,matP.data(),pos,upz); At<void*>(e,0x14)=matP.data(); }
                        mkEffects(mi,int(1+r.next()%6),{0,0,0,0,0,4,4,1,3,7,0,0});
                        ret=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["mat"]=Snap(matP.data(),0x48,detail);
                    }
                    else if(nm=="PreRender") {
                        const int32 mIdx=MiVal(r.next()); const int32 useIdx=r.coin(4) ? int32(r.pick({342,344,345,777,1234})) : mIdx;
                        At<uint16>(e,0x22)=uint16(useIdx); uint8* mi=Mi(0); setMi(useIdx,mi);
                        At<uint32>(e,0x1C)|=(r.coin(5) ? 0u : 0x200000u);
                        At<uint8>(mi,0xC)=uint8(r.pick({0,100,238,239,240,255}));
                        At<uint16>(mi,0x12)=uint16((uint32(r.pick({0,0,1,2,9,5,4,1,2}))<<11)|(r.coin(2) ? 1u : 0u)|(r.next()&0x06FE));
                        if(r.coin(3))At<uint8>(mi,0xD)=0; else genericEffects(mi);
                        cfg.amiRet=r.coin(3) ? 0 : uintptr_t(mi); cfg.modelType=r.pick({5,5,1,2,3}); cfg.matfx=r.coin(2);
                        // el model info guarda un atomic si es de atomics y un clump si lo trata como clump (las comprobaciones de tipo de la DLL)
                        At<void*>(mi,0x1C)=r.coin(4) ? nullptr : (cfg.amiRet ? static_cast<void*>(AtomB(0)) : static_cast<void*>(Clump(1)));
                        if(r.coin(5))At<void*>(AtomB(0),0x18)=nullptr;
                        if(r.coin(2))At<uint32>(e,0x140)|=1u; if(r.coin(4))At<uint32>(e,0x140)|=0x40u; if(r.coin(3))At<uint32>(e,0x140)|=0x800u;
                        if(r.coin(2))At<void*>(e,0x18)=AtomB(1);
                        const float mag=r.pickF({0.f,0.02f,0.03f,0.0299f,0.031f,0.2f,0.5f}); At<float>(e,0x44)=mag; At<float>(e,0x48)=r.pickF({0.f,mag,-mag,0.01f});
                        cfg.water=r.coin(2); cfg.glass=r.coin(4); cfg.mitl=r.coin(4);
                        if(r.coin(2)){ FillMat(r,matP.data(),pos+r.vec(1.f)); At<void*>(e,0x14)=matP.data(); } else At<void*>(e,0x14)=nullptr;
                        // la posición de la entidad decide la distancia a la cámara (a veces justo en los límites 50, 100 y 110)
                        const CVector ep=At<void*>(e,0x14) ? At<CVector>(matP.data(),0x30) : At<CVector>(e,0x04);
                        cfg.waterZ=ep.z+r.pickF({-1.f,0.f,1.f,0.5f});
                        AtA<uintptr_t>(ADDR_CAMMAT)=0; At<CVector>(reinterpret_cast<void*>(ADDR_CAMPOS),0)=ep+CVector{r.pickF({49.f,50.f,51.f,99.f,100.f,109.f,110.f,111.f,0.f,r.uni(-80.f,80.f)}),0.f,0.f};
                        ret=nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["mi"]=Snap(mi,0x60,detail); out["tree"]=AtA<uint32>(ADDR_TREEDIST); out["atom"]=Snap(AtomB(0),0x60,detail); out["mat"]=Snap(matP.data(),0x48,detail);
                    }
                    // comparación común
                    out["ent"]=Snap(e,0x200,detail); out["geoms"]=Snap(geoms.data(),sizeof(geoms),detail); out["mats"]=Snap(mats.data(),sizeof(mats),detail);
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
