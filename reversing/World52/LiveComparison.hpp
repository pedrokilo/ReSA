#pragma once
// Banco dentro del motor: mundo, render y plugins (lista de 52; 42 funciones comparables). CAnimatedBuilding,
// CBouncingPanel, CCreepingFire, CHandObject, CHandShaker, CMouseControllerState, COccluder, CPlaneTrail,
// CPlantLocTri, CStencilShadowObject, CWeaponModelInfo, NodeNamePlugin, PipelinePlugin y WaterCreature_c.
// Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas sobre objetos falsos.
// Se simulan y trazan: rand, FindPlayerPed, estados de render e Im3D, CalcScreenCoors (ambas variantes),
// DoesInfiniteLineTouchScreen, CWorld (Add/Remove/lineas), StartFire, GetSpeed, el registro de plugins de RW
// (nunca se registra nada en el motor real; se comparan los argumentos y se ejecutan los callbacks capturados
// sobre frames/atomics falsos), RwStreamRead/Write, la creacion de CObject (operator new + constructor), los
// CEntity::PreRender/UpdateRwFrame/UpdateRpHAnim, CObject::Render, CClumpModelInfo::Init/SetClump, etc.
// Los tres hooks de WindowedModeHooks (sustituciones NOTSA del dispositivo D3D9) quedan fuera del banco.
// Todo el estado global tocado se guarda y se restaura en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "AnimatedBuilding.h"
#include "HandObject.h"
#include "BouncingPanel.h"
#include "CreepingFire.h"
#include "FireManager.h"
#include "HandShaker.h"
#include "MouseControllerState.h"
#include "Occluder.h"
#include "ActiveOccluder.h"
#include "PlaneTrail.h"
#include "PlantLocTri.h"
#include "PlantMgr.h"
#include "PlantSurfPropMgr.h"
#include "ProcObjectMan.h"
#include "StencilShadowObject.h"
#include "StencilShadows.h"
#include "WeaponModelInfo.h"
#include "ClumpModelInfo.h"
#include "VisibilityPlugins.h"
#include "WaterCreature_c.h"
#include "WaterCreatureManager_c.h"
#include "WaterLevel.h"
#include "World.h"
#include "MemoryMgr.h"
#include "RwHelper.h"
#include "../ProbeRuntime.hpp"

// acceso a WaterCreatureManager_c::TryToExitGroup (privada): la instanciacion explicita ignora el control de acceso
template<class Tag,typename Tag::type M> struct RobW52 { friend typename Tag::type Get(Tag) { return M; } };
struct TryToExitTag { using type=void(WaterCreatureManager_c::*)(WaterCreature_c*); friend type Get(TryToExitTag); };
template struct RobW52<TryToExitTag,&WaterCreatureManager_c::TryToExitGroup>;

namespace World52Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<std::array<uint8,0x800>,4> ent{};      // entidades/objetos/peds/vehiculos
alignas(16) inline std::array<std::array<uint8,0x60>,4> mats{};       // CMatrix(Link) falsas
alignas(16) inline std::array<std::array<uint8,0x100>,2> frames{};    // RwFrame (RwMatrix en +0x10)
alignas(16) inline std::array<std::array<uint8,0x40>,2> clumps{};
alignas(16) inline std::array<uint8,0x40> hier{};
alignas(16) inline std::array<uint8,64*0x40> matArr{};
alignas(16) inline std::array<uint8,64*0x10> nodeInfo{};
alignas(16) inline std::array<uint8,0x80> atomic{}, geom{}, material{};
alignas(16) inline std::array<void*,4> matPtrs{};
alignas(16) inline std::array<uint8,0x200> smallBuf{};
alignas(16) inline std::array<std::array<uint8,0x7C>,4> props{};
alignas(16) inline std::array<std::array<uint8,0x54>,8> tris{};
alignas(16) inline std::array<std::array<uint8,0x1C>,6> shadows{};
alignas(16) inline std::array<uint8,0x28> fakeFire{};
alignas(16) inline std::array<std::array<uint8,0x200>,2> plugObj{};
alignas(16) inline std::array<std::array<uint8,0x34>,3> wc{};
alignas(16) inline std::array<std::array<uint8,0x200>,3> wobj{};
inline std::array<void*,64> objVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }

// --- dependencias simuladas ---
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; }
inline bool BitB(){ return (maskB>>(callsB++%32))&1; }
inline void* playerResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return static_cast<CPlayerPed*>(playerResult); }
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ json vs=json::array(); for(RwUInt32 k=0;k<n&&k<32;++k){ const auto* w=reinterpret_cast<const uint32*>(static_cast<uint8*>(v)+k*0x24); vs.push_back({w[0],w[1],w[2],w[6]}); }
    trace.push_back({"im3d_transform",Id(v),intptr_t(v)-0xC4D958,n,uintptr_t(m),f,vs}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ json is=json::array(); for(RwInt32 k=0;k<n&&k<64;++k)is.push_back(idx[k]); trace.push_back({"im3d_render_indexed",int(t),n,is}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
// proyeccion falsa determinista
inline float projK{}, projOff{}; inline CVector projCam{};
inline bool Proj(const CVector& in,CVector& out){ const CVector d{in.x-projCam.x,in.y-projCam.y,in.z-projCam.z};
    out.z=d.y*0.75f+projOff; out.x=320.f+d.x*projK/(std::fabs(out.z)+1.f); out.y=240.f-d.z*projK/(std::fabs(out.z)+1.f); return out.z>1.f; }
inline bool __cdecl FakeCalc4(const CVector& in,CVector& out,float& w,float& h){ const bool r=Proj(in,out); if(r){w=out.x*0.01f;h=out.y*0.01f;} trace.push_back({"calc4",F(in.x),F(in.y),F(in.z),r}); return r; }
inline bool __cdecl FakeCalc2(const CVector& in,CVector& out){ const bool r=Proj(in,out); trace.push_back({"calc2",F(in.x),F(in.y),F(in.z),r}); return r; }
inline bool __cdecl FakeTouch(CVector2D o,CVector2D d){ const bool r=((F(o.x)^F(o.y)*3u^F(d.x)*7u^F(d.y)*13u)>>5)&1 ? BitA() : !BitA(); trace.push_back({"touch_screen",F(o.x),F(o.y),F(d.x),F(d.y),r}); return r; }
inline void __cdecl FakeWorldAdd(CEntity* e){ trace.push_back({"world_add",Id(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e){ trace.push_back({"world_remove",Id(e)}); }
inline float colZ{}; inline CVector colNormal{};
inline bool __cdecl FakeVertical(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,CStoredCollPoly* poly){
    const bool r=BitB(); trace.push_back({"vertical_line",F(o.x),F(o.y),F(o.z),F(d),b0,b1,b2,b3,b4,b5,uintptr_t(poly),r}); if(r){cp.m_vecPoint=CVector{o.x,o.y,colZ};e=nullptr;} return r; }
inline bool __cdecl FakeLos(const CVector& o,const CVector& t,CColPoint& cp,CEntity*& e,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7){
    const bool r=BitB(); trace.push_back({"line_of_sight",F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),b0,b1,b2,b3,b4,b5,b6,b7,r}); if(r){cp.m_vecNormal=colNormal;e=nullptr;} return r; }
inline void* fireResult{};
inline CFire* __fastcall FakeStartFire(void* self,void*,CVector p,float size,uint8 u,CEntity* creator,uint32 time,int8 gen,uint8 u2){
    trace.push_back({"start_fire",intptr_t(self)-0xB71F80,F(p.x),F(p.y),F(p.z),F(size),u,Id(creator),time,gen,u2}); return static_cast<CFire*>(fireResult); }
inline CVector* __fastcall FakeGetSpeed(void* self,void*,CVector* out,CVector pt){ trace.push_back({"get_speed",Id(self),F(pt.x),F(pt.y),F(pt.z)}); *out=CVector{pt.x*0.375f+0.125f,pt.y*-0.25f,pt.z*0.5f+0.0625f}; return out; }
inline CPlantSurfProp* __cdecl FakeSurf(uint16 i){ trace.push_back({"surf_props",i}); return reinterpret_cast<CPlantSurfProp*>(props[i%4].data()); }
inline void __cdecl FakeMoveTri(CPlantLocTri** from,CPlantLocTri** to,CPlantLocTri* t){ trace.push_back({"move_loc_tri",intptr_t(from)-0xC03984,intptr_t(to)-0xC03984,Id(t)}); }
inline void __fastcall FakeTriRemoved(void* self,void*,CPlantLocTri* t){ trace.push_back({"tri_removed",intptr_t(self)-0xBB7CB0,Id(t)}); }
inline void __fastcall FakeCleanUp(void* self,void*,CEntity** ref){ trace.push_back({"clean_up_ref",Id(self),Id(ref)}); }
inline void __cdecl FakeFree(void* p){ trace.push_back({"free",uintptr_t(p)}); }
inline void __fastcall FakeEntPreRender(void* self,void*){ trace.push_back({"entity_pre_render",Id(self)}); }
inline void __fastcall FakeUpdateRwFrame(void* self,void*){ trace.push_back({"update_rw_frame",Id(self)}); }
inline void __fastcall FakeUpdateRpHAnim(void* self,void*){ trace.push_back({"update_rphanim",Id(self)}); }
inline void __fastcall FakeRemoveAndAdd(void* self,void*){ trace.push_back({"remove_and_add",Id(self)}); }
inline RpHAnimHierarchy* __cdecl FakeGetHier(RpClump* c){ trace.push_back({"get_hierarchy",Id(c)}); return reinterpret_cast<RpHAnimHierarchy*>(hier.data()); }
inline void* firstAtomicResult{};
inline RpAtomic* __cdecl FakeFirstAtomic(RpClump* c){ trace.push_back({"first_atomic",Id(c)}); return static_cast<RpAtomic*>(firstAtomicResult); }
inline RpMaterial* __cdecl FakeSetTexture(RpMaterial* m,RwTexture* t){ trace.push_back({"material_set_texture",Id(m),uintptr_t(t)}); return m; }
inline void __fastcall FakeObjectRender(void* self,void*){ trace.push_back({"object_render",Id(self)}); }
inline void __fastcall FakeClumpInit(void* self,void*){ trace.push_back({"clump_mi_init",Id(self)}); }
inline void __fastcall FakeClumpSetClump(void* self,void*,RpClump* c){ trace.push_back({"clump_mi_set_clump",Id(self),Id(c)}); }
inline uintptr_t dllSetAtomicRendererCB{}, dllRenderWeaponCB{};
inline json Cb(uintptr_t p){ if(p==0x4C5280||p==dllSetAtomicRendererCB) return "SetAtomicRendererCB"; if(p==0x733670||p==dllRenderWeaponCB) return "RenderWeaponCB"; return p ? json("other") : json(nullptr); }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,void* cb,void* data){ trace.push_back({"for_all_atomics",Id(c),Cb(uintptr_t(cb)),Cb(uintptr_t(data))}); return c; }
inline void* frameResult{}; inline void* firstObjectResult{};
inline RwFrame* __cdecl FakeFrameFromName(RpClump* c,const char* n){ trace.push_back({"frame_from_name",Id(c),std::string(n)}); return static_cast<RwFrame*>(frameResult); }
inline RwObject* __cdecl FakeFirstObject(RwFrame* f){ trace.push_back({"first_object",Id(f)}); return static_cast<RwObject*>(firstObjectResult); }
inline void __cdecl FakeAtomicAlpha(RpAtomic* a,int32 alpha){ trace.push_back({"atomic_alpha",Id(a),alpha}); }
// registro de plugins de RW (nunca llega al motor real)
inline RwInt32 regResult{}, streamResult{};
inline std::array<uintptr_t,3> objCbs{}; inline std::array<uintptr_t,3> streamCbs{};
inline RwInt32 __cdecl FakeRegister(RwInt32 size,RwUInt32 id,void* c,void* d,void* cp){ objCbs={uintptr_t(c),uintptr_t(d),uintptr_t(cp)}; trace.push_back({"register_plugin",size,id,c!=nullptr,d!=nullptr,cp!=nullptr,regResult}); return regResult; }
inline RwInt32 __cdecl FakeRegisterStream(RwUInt32 id,void* r,void* w,void* g){ streamCbs={uintptr_t(r),uintptr_t(w),uintptr_t(g)}; trace.push_back({"register_plugin_stream",id,r!=nullptr,w!=nullptr,g!=nullptr,streamResult}); return streamResult; }
inline uint32 streamState{};
inline RwUInt32 __cdecl FakeStreamRead(RwStream* s,void* buf,RwUInt32 len){ auto* b=static_cast<uint8*>(buf); for(RwUInt32 k=0;k<len;++k){streamState=streamState*1103515245u+12345u;b[k]=uint8('a'+(streamState>>16)%26);} trace.push_back({"stream_read",uintptr_t(s),Id(buf),len}); return len; }
inline RwStream* __cdecl FakeStreamWrite(RwStream* s,const void* buf,RwUInt32 len){ trace.push_back({"stream_write",uintptr_t(s),Id(buf),len,Fnv(buf,len)}); return s; }
// CObject y su tabla virtual falsa
inline void* newResult{}; inline void* ctorMatrix{};
inline void* __cdecl FakeObjNew(size_t n){ trace.push_back({"object_new",n}); return newResult; }
inline void* __fastcall FakeObjCtor(void* self,void*,int32 model,bool create){ trace.push_back({"object_ctor",Id(self),model,create});
    auto* o=static_cast<uint8*>(self); *reinterpret_cast<void**>(o)=objVt.data(); *reinterpret_cast<void**>(o+0x14)=ctorMatrix; *reinterpret_cast<void**>(o+0x18)=nullptr; return self; }
inline void* __fastcall FakeObjDtor(void* self,void*,uint8 flags){ trace.push_back({"object_delete",Id(self),flags}); return self; }
inline void __fastcall FakeSetIsStatic(void* self,void*,bool s){ trace.push_back({"set_is_static",Id(self),s}); }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline float waterLevel{};
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float& out,uint8 touching,CVector* n){ const bool r=BitA(); trace.push_back({"water_level",F(x),F(y),F(z),touching,uintptr_t(n),r}); if(r)out=waterLevel; return r; }
inline void __fastcall FakeTryToExit(void* self,void*,WaterCreature_c* c){ trace.push_back({"try_to_exit_group",intptr_t(self)-0xC1DF30,Id(c)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Direccion de la funcion de la DLL instalada por un hook virtual (para simularla tambien en llamadas directas)
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }
inline uintptr_t Follow(uintptr_t p){ for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }
// Constructor CObject(int32, bool) de la DLL: primera llamada a la DLL que sigue a la de CObject::operator new en
// WaterCreature_c::Init (no hay hook ni se puede tomar la direccion de un constructor)
inline uintptr_t FindCtorCall(uintptr_t fn,uintptr_t newFn){
    HMODULE mod{}; if(!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(fn),&mod))return 0;
    const auto modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew);
    const uintptr_t modEnd=modBase+nt->OptionalHeader.SizeOfImage;
    auto FollowIn=[&](uintptr_t q){ for(int k=0;k<4&&q>=modBase&&q<modEnd&&*reinterpret_cast<const uint8*>(q)==0xE9;++k)q=q+5+*reinterpret_cast<const int32*>(q+1); return q; };
    fn=FollowIn(fn); const auto nf=FollowIn(newFn); bool seen=false;
    for(uintptr_t p=fn;p<fn+0x900;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(!seen){ if(t>=modBase&&t<modEnd&&FollowIn(t)==nf){seen=true;p+=4;} continue; }
        // solo destinos dentro de la imagen de la DLL que sean entradas de la ILT (jmp) o inicios alineados a 16
        if(t<modBase||t>=modEnd)continue; if(*reinterpret_cast<const uint8*>(t)!=0xE9&&(t&15))continue;
        return t; }
    return 0;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,42> fns{{
    {"CAnimatedBuilding","ProcessControl",0,0x8639B0,10,64},{"CAnimatedBuilding","ProcessCollision",0,0x8639B0,11,128},{"CAnimatedBuilding","PreRender",0,0x8639B0,17,256},
    {"CBouncingPanel","ResetPanel",0x6F4910,0,-1,64},{"CBouncingPanel","SetPanel",0x6F4920,0,-1,256},{"CBouncingPanel","ProcessPanel",0x6F49A0,0,-1,1024},
    {"CCreepingFire","SetReadyToBurn",0x539CC0,0,-1,64},{"CCreepingFire","Update",0x539CE0,0,-1,512},{"CCreepingFire","TryToStartFireAtCoors",0x53A450,0,-1,1024},
    {"CHandObject","ProcessControl",0,0x866EE0,10,256},{"CHandObject","PreRender",0,0x866EE0,17,512},{"CHandObject","Render",0,0x866EE0,18,256},
    {"CHandShaker","SetDefaults",0x517330,0,-1,256},{"CHandShaker","Reset",0x50D860,0,-1,512},{"CHandShaker","Process",0x50D930,0,-1,1024},
    {"CMouseControllerState","Constructor",0x53F220,0,-1,64},{"CMouseControllerState","Clear",0x53F250,0,-1,64},{"CMouseControllerState","CheckForInput",0x53F270,0,-1,512},
    {"COccluder","NearCamera",0x71F960,0,-1,512},{"COccluder","ProcessLineSegment",0x71E130,0,-1,1024},{"COccluder","ProcessOneOccluder",0x71E5D0,0,-1,1024},
    {"CPlaneTrail","Init",0x717160,0,-1,64},{"CPlaneTrail","RegisterPoint",0x7172D0,0,-1,512},{"CPlaneTrail","Render",0x717180,0,-1,1024},
    {"CPlantLocTri","Add",0x5DC290,0,-1,1024},{"CPlantLocTri","GetPlantDensity",0x5DC210,0,-1,1024},{"CPlantLocTri","Release",0x5DB6D0,0,-1,512},
    {"CStencilShadowObject","Constructor",0x70F8D0,0,-1,64},{"CStencilShadowObject","Destroy",0x711310,0,-1,512},{"CStencilShadowObject","Destructor",0x70F8E0,0,-1,64},
    {"CWeaponModelInfo","GetModelType",0,0x85BD78,4,16},{"CWeaponModelInfo","Init",0,0x85BD78,6,64},{"CWeaponModelInfo","SetClump",0,0x85BD78,16,256},
    {"NodeNamePlugin","GetFrameNodeName",0x72FB30,0,-1,256},{"NodeNamePlugin","NodeNamePluginAttach",0x72FAB0,0,-1,128},{"NodeNamePlugin","SetFrameNodeName",0x72FB00,0,-1,512},
    {"PipelinePlugin","GetPipelineID",0x72FC40,0,-1,256},{"PipelinePlugin","PipelinePluginAttach",0x72FBD0,0,-1,128},{"PipelinePlugin","SetPipelineID",0x72FC50,0,-1,256},
    {"WaterCreature_c","Exit",0x6E3E60,0,-1,256},{"WaterCreature_c","Init",0x6E4160,0,-1,1024},{"WaterCreature_c","Update",0x6E4670,0,-1,1024},
}};

// 0x5DC210 recibe el triangulo en EAX y devuelve en ST0
inline float CallEax(uintptr_t fn,void* arg){ float r; __asm { mov eax, arg
                                                               mov ecx, fn
                                                               call ecx
                                                               fstp r } return r; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world52-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world52-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world52");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world52-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    const uintptr_t dllEntPreRender=OurVirtual("CEntity","PreRender"), dllObjRender=OurVirtual("CObject","Render");
    const uintptr_t dllClumpInit=OurVirtual("CClumpModelInfo","Init"), dllClumpSetClump=OurVirtual("CClumpModelInfo","SetClump");
    if(!dllEntPreRender||!dllObjRender||!dllClumpInit||!dllClumpSetClump){Write({{"event","error"},{"reason","virtual dependency not found"}});return;}
    dllSetAtomicRendererCB=bit_cast<uintptr_t>(&CClumpModelInfo::SetAtomicRendererCB); dllRenderWeaponCB=bit_cast<uintptr_t>(&CVisibilityPlugins::RenderWeaponCB);
    const uintptr_t dllObjNew=bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new));
    const uintptr_t dllObjCtor=FindCtorCall(bit_cast<uintptr_t>(&WaterCreature_c::Init),dllObjNew);
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ for(auto& b:ent)Reg(b.data(),b.size()); for(auto& b:mats)Reg(b.data(),b.size()); for(auto& b:frames)Reg(b.data(),b.size()); for(auto& b:clumps)Reg(b.data(),b.size());
        Reg(hier.data(),hier.size()); Reg(matArr.data(),matArr.size()); Reg(nodeInfo.data(),nodeInfo.size()); Reg(atomic.data(),atomic.size()); Reg(geom.data(),geom.size()); Reg(material.data(),material.size());
        Reg(smallBuf.data(),smallBuf.size()); for(auto& b:props)Reg(b.data(),b.size()); for(auto& b:tris)Reg(b.data(),b.size()); for(auto& b:shadows)Reg(b.data(),b.size()); Reg(fakeFire.data(),fakeFire.size());
        for(auto& b:plugObj)Reg(b.data(),b.size()); for(auto& b:wc)Reg(b.data(),b.size()); for(auto& b:wobj)Reg(b.data(),b.size()); Reg(reinterpret_cast<void*>(0xC03984),20); Reg(reinterpret_cast<void*>(0xC1F930),24); }
    for(auto& v:objVt)v=reinterpret_cast<void*>(&FakeVTrap); objVt[0]=reinterpret_cast<void*>(&FakeObjDtor); objVt[4]=reinterpret_cast<void*>(&FakeSetIsStatic);

    // estado global guardado
    auto Snap=[](uintptr_t a,size_t n){ auto* p=reinterpret_cast<uint8*>(a); return std::vector<uint8>(p,p+n); };
    auto Back=[](uintptr_t a,const std::vector<uint8>& v){ std::memcpy(reinterpret_cast<void*>(a),v.data(),v.size()); };
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xB7CB4C,4},{0xB7CB58,8},{0xB7CB84,4},{0xB71B68,1024},{0xB71F80,0x960},{0xC73CA0,0x18},{0xC798E0,0xD0},{0xB6FA2C,0x40},{0xB6F03C,4},
            {0xC4D958,16*0x24},{0xC03984,20},{0xC6A168,8},{0xC87C5C,4},{0x8D6080,4},{0xBB4A70,2},{0xC1F930,24}})
        saved.push_back({a,Snap(a,n)});
    auto RestoreAll=[&]{ for(auto& s:saved)Back(s.a,s.v); };
    uint8* const poolLock=*reinterpret_cast<uint8**>(0xB7449C)+0x11; const uint8 savedPoolLock=*poolLock;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRender), i3(0x7EF520,FakeIm3DEnd);
        EntryPatch c4(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CVector&,float&,float&)>(&CalcScreenCoors)),FakeCalc4), c4e(0x71DA00,FakeCalc4);
        EntryPatch c2(bit_cast<uintptr_t>(static_cast<bool(*)(const CVector&,CVector&)>(&CalcScreenCoors)),FakeCalc2), c2e(0x71DAB0,FakeCalc2);
        EntryPatch ts(bit_cast<uintptr_t>(&DoesInfiniteLineTouchScreen),FakeTouch), tse(0x71DB80,FakeTouch);
        EntryPatch wa(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), wae(0x563220,FakeWorldAdd), wr(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), wre(0x563280,FakeWorldRemove);
        EntryPatch vl(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVertical), vle(0x5674E0,FakeVertical), los(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeLos), lose(0x56BA00,FakeLos);
        EntryPatch sf(bit_cast<uintptr_t>(static_cast<CFire*(CFireManager::*)(CVector,float,uint8,CEntity*,uint32,int8,uint8)>(&CFireManager::StartFire)),FakeStartFire), sfe(0x539F00,FakeStartFire);
        EntryPatch gs(bit_cast<uintptr_t>(&CPhysical::GetSpeed),FakeGetSpeed), gse(0x542CE0,FakeGetSpeed);
        EntryPatch sp(bit_cast<uintptr_t>(&CPlantSurfPropMgr::GetSurfProperties),FakeSurf), spe(0x6F9DE0,FakeSurf);
        EntryPatch mt(bit_cast<uintptr_t>(&CPlantMgr::MoveLocTriToList),FakeMoveTri), mte(0x5DB590,FakeMoveTri);
        EntryPatch tr(bit_cast<uintptr_t>(&ProcObjectMan_c::ProcessTriangleRemoved),FakeTriRemoved), tre(0x5A3F70,FakeTriRemoved);
        EntryPatch cu(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp), cue(0x571A00,FakeCleanUp);
        EntryPatch fr(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), fre(0x72F430,FakeFree);
        EntryPatch ep(dllEntPreRender,FakeEntPreRender), epe(0x535FA0,FakeEntPreRender);
        EntryPatch uf(bit_cast<uintptr_t>(&CEntity::UpdateRwFrame),FakeUpdateRwFrame), ufe(0x532B00,FakeUpdateRwFrame);
        EntryPatch uh(bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim),FakeUpdateRpHAnim), uhe(0x532B20,FakeUpdateRpHAnim);
        EntryPatch ra(bit_cast<uintptr_t>(&CPhysical::RemoveAndAdd),FakeRemoveAndAdd), rae(0x542560,FakeRemoveAndAdd);
        EntryPatch gh(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FakeGetHier), ghe(0x734A40,FakeGetHier);
        EntryPatch fa(bit_cast<uintptr_t>(&GetFirstAtomic),FakeFirstAtomic), fae(0x734820,FakeFirstAtomic);
        EntryPatch st(0x74DBC0,FakeSetTexture);
        EntryPatch orr(dllObjRender,FakeObjectRender), ore(0x59F180,FakeObjectRender);
        EntryPatch ci(dllClumpInit,FakeClumpInit), cie(0x4C4E40,FakeClumpInit), cs(dllClumpSetClump,FakeClumpSetClump), cse(0x4C4F70,FakeClumpSetClump);
        EntryPatch faa(0x749B70,FakeForAllAtomics);
        EntryPatch ff(bit_cast<uintptr_t>(&CClumpModelInfo::GetFrameFromName),FakeFrameFromName), ffe(0x4C5400,FakeFrameFromName);
        EntryPatch fo(bit_cast<uintptr_t>(&GetFirstObject),FakeFirstObject), foe(0x7348C0,FakeFirstObject);
        EntryPatch aa(bit_cast<uintptr_t>(&CVehicle::SetComponentAtomicAlpha),FakeAtomicAlpha), aae(0x6D2960,FakeAtomicAlpha);
        EntryPatch rg1(0x7F1260,FakeRegister), rg2(0x807580,FakeRegisterStream), rg3(0x74BDA0,FakeRegister), rg4(0x74BE00,FakeRegisterStream);
        EntryPatch srd(0x7EC9D0,FakeStreamRead), swr(0x7ECB30,FakeStreamWrite);
        EntryPatch on(dllObjNew,FakeObjNew), one(0x5A1EE0,FakeObjNew), oce(0x5A1D70,FakeObjCtor);
        std::unique_ptr<EntryPatch> oc; if(dllObjCtor)oc=std::make_unique<EntryPatch>(dllObjCtor,FakeObjCtor);
        EntryPatch wl(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel), wle(0x6EB690,FakeWaterLevel);
        EntryPatch te(bit_cast<uintptr_t>(Get(TryToExitTag{})),FakeTryToExit), tee(0x6E45B0,FakeTryToExit);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;
        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillF=[&](void* p,size_t n,float a,float b){ auto* q=static_cast<float*>(p); for(size_t k=0;k<n;++k)q[k]=uni(a,b); };
                // CMatrix falsa con rotacion en Z, posicion dada y sin matriz adjunta
                auto mkMat=[&](void* m,const CVector& pos){ const float h=uni(-3.2f,3.2f), s=std::sin(h), c=std::cos(h); auto* q=static_cast<float*>(m);
                    std::fill(q,q+0x15,0.f); q[0]=c;q[1]=s;q[2]=uni(-0.05f,0.05f); q[4]=-s;q[5]=c;q[6]=uni(-0.05f,0.05f); q[8]=uni(-0.05f,0.05f);q[9]=uni(-0.05f,0.05f);q[10]=1.f;
                    q[12]=pos.x;q[13]=pos.y;q[14]=pos.z; At<void*>(m,0x40)=nullptr; At<uint8>(m,0x44)=0; };
                auto mkRwMat=[&](void* m){ fillF(m,16,-1.5f,1.5f); auto* q=static_cast<float*>(m); q[12]=uni(-500.f,500.f);q[13]=uni(-500.f,500.f);q[14]=uni(-50.f,50.f);
                    At<uint32>(m,0xC)=next()%4; At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5252u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    maskA=next(); callsA=0; maskB=next(); callsB=0; if(next()%5==0)maskB=~0u; if(next()%7==0)maskB=0;
                    playerResult=nullptr; im3dOk=next()%4!=0; projK=uni(0.5f,40.f); projOff=uni(-30.f,60.f); projCam=CVector{0.f,0.f,0.f};
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    for(auto& b:clumps){ fill(b.data(),b.size()); b[0]=2; } At<void*>(clumps[0].data(),4)=frames[0].data(); At<void*>(clumps[1].data(),4)=frames[1].data();
                    for(auto& b:frames){ fill(b.data(),b.size()); mkRwMat(b.data()+0x10); }

                    if(cls=="CAnimatedBuilding") {
                        void* o=ent[0].data(); fill(o,0x40); At<uintptr_t>(o,0)=0x8639B0; At<float>(o,4)=uni(-100.f,100.f); At<float>(o,8)=uni(-100.f,100.f); At<float>(o,12)=uni(-10.f,10.f); At<float>(o,16)=uni(-3.f,3.f);
                        At<void*>(o,0x14)=nullptr; At<void*>(o,0x18)=next()%4 ? clumps[0].data() : nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o); out["obj"]=ReversingProbe::Bytes(o,0x38);
                    }
                    else if(cls=="CHandObject") {
                        void* o=ent[0].data(); void* ped=ent[1].data(); fill(o,0x18C); fill(ped,0x140);
                        for(size_t k:{0x4,0x8,0xC,0x10})At<float>(o,k)=uni(-10.f,10.f); At<uintptr_t>(o,0)=0x866EE0; mkMat(mats[0].data(),CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,20.f)}); At<void*>(o,0x14)=mats[0].data();
                        At<void*>(o,0x18)=nm!="PreRender"||next()%5 ? clumps[1].data() : nullptr; At<void*>(o,0x17C)=ped; // Render exige un clump (GetRpClump)
                        const uint32 bone=next()%24; At<uint32>(o,0x180)=bone; At<uint32>(o,0x184)=0x7000000+next()%0x1000; At<uint8>(o,0x188)=uint8(next()%2);
                        At<float>(o,0x12C)=uni(0.f,1.f); At<void*>(ped,0x18)=clumps[0].data(); At<float>(ped,0x12C)=uni(0.f,1.f); At<float>(ped,4)=uni(-10.f,10.f);
                        fill(hier.data(),hier.size()); At<void*>(hier.data(),8)=matArr.data(); At<void*>(hier.data(),0x10)=nodeInfo.data();
                        for(int k=0;k<64;++k)mkRwMat(matArr.data()+k*0x40);
                        fill(nodeInfo.data(),nodeInfo.size()); { int counter=0; bool ended=false; for(uint32 k=bone+1;k<64;++k){ uint32 fl; if(ended||counter>=int(62-k)){fl=1;} /* pop forzado: la pila se vacia como tarde en el nodo 62 (el bucle del EXE no tiene limite) */ else { const uint32 r=next()%6; fl=r<2 ? 2u : r<4 ? 1u : r==4 ? 3u : 0u; }
                            At<uint32>(nodeInfo.data(),k*16+8)=fl|(next()&0xF0u); if(!ended){ if(fl&2)++counter; else if(fl&1)--counter; if(counter<0)ended=true; } } }
                        fill(atomic.data(),atomic.size()); fill(geom.data(),geom.size()); fill(material.data(),material.size()); At<void*>(atomic.data(),0x18)=geom.data(); At<void*>(geom.data(),0x20)=matPtrs.data(); matPtrs[0]=material.data();
                        firstAtomicResult=atomic.data();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                        out["obj"]=ReversingProbe::Bytes(o,0x18C); out["ped"]=ReversingProbe::Bytes(ped,0x130); // el operator= del EXE (rep movs de 16 dwords) copia tambien los rellenos de la CMatrix temporal de la pila,
                        // que nadie inicializa (basura distinta en EXE y DLL): se enmascaran
                        { auto m=ReversingProbe::Bytes(mats[0].data(),0x48); for(size_t pad:{0xC,0x1C,0x2C,0x3C})std::fill_n(m.begin()+pad,4,uint8(0)); out["mat"]=m; }
                        out["bones"]=Fnv(matArr.data(),matArr.size()); out["frame"]=ReversingProbe::Bytes(frames[1].data()+0x10,0x40); out["material"]=ReversingProbe::Bytes(material.data(),0x20);
                    }
                    else if(cls=="CBouncingPanel") {
                        void* pn=smallBuf.data(); fill(pn,0x20); At<int16>(pn,0)=int16(next()%40-5); At<uint16>(pn,2)=uint16(next()%8==0 ? next()%7 : next()%3); At<float>(pn,4)=uni(-1.f,1.f); for(size_t k=8;k<0x20;k+=4)At<float>(pn,k)=uni(-0.3f,0.3f);
                        if(nm=="ResetPanel")reinterpret_cast<void(__thiscall*)(void*)>(a)(pn);
                        else if(nm=="SetPanel")reinterpret_cast<void(__thiscall*)(void*,int16,int16,float)>(a)(pn,int16(next()),int16(next()%4),uni(-1.f,1.f));
                        else { void* veh=ent[0].data(); fill(veh,0x40); mkMat(mats[0].data(),CVector{uni(-500.f,500.f),uni(-500.f,500.f),uni(0.f,30.f)}); At<void*>(veh,0x14)=mats[0].data();
                            const CVector mf{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.3f,0.3f)}, tf{uni(-0.05f,0.05f),uni(-0.05f,0.05f),uni(-0.05f,0.05f)};
                            const float rm=next()%3==0 ? -1.f : uni(0.f,0.5f), dm=next()%3==0 ? (next()%2 ? -1.f : -2.f) : uni(0.5f,1.f);
                            CTimer::ms_fTimeStep=next()%6==0 ? 1.f : uni(0.2f,3.f);
                            reinterpret_cast<void(__thiscall*)(void*,void*,void*,CVector,CVector,float,float)>(a)(pn,veh,frames[0].data(),mf,tf,rm,dm);
                            out["frame"]=ReversingProbe::Bytes(frames[0].data()+0x10,0x40); }
                        out["panel"]=ReversingProbe::Bytes(pn,0x20);
                    }
                    else if(cls=="CCreepingFire") {
                        auto* stat=reinterpret_cast<uint8*>(0xB71B68); for(int k=0;k<1024;++k)stat[k]=uint8(next()%4==0 ? next()%8 : (next()%3==0 ? 0 : 4+next()%3));
                        if(nm=="SetReadyToBurn")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Update"){ CTimer::m_FrameCounter=next()%3 ? next()%5000 : next(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else { auto* fires=reinterpret_cast<uint8*>(0xB71F80); const uint32 activeRate=next()%4; for(int k=0;k<60;++k){ uint8& fl=fires[k*0x28]; fl=uint8((fl&~1u)|((next()%4)<activeRate+1 ? 1u : 0u)); }
                            fill(fakeFire.data(),fakeFire.size()); fireResult=next()%5 ? fakeFire.data() : nullptr; colZ=uni(-10.f,40.f);
                            CVector pos{uni(-300.f,300.f),uni(-300.f,300.f),uni(0.f,50.f)}; if(next()%4==0){pos.x=float(int(next()%64)-32);pos.y=float(int(next()%64)-32)+0.5f;}
                            const int ix=int(int64(pos.x))&31, iy=int(int64(pos.y))&31; if(next()%3)stat[ix*32+iy]=uint8(next()%3==0 ? 4+next()%3 : 0);
                            ret=reinterpret_cast<bool(__cdecl*)(CVector,uint8,bool,bool,float)>(a)(pos,uint8(next()),next()%2,next()%2,uni(0.f,30.f));
                            out["fire"]=fakeFire[0]; out["cell"]=stat[ix*32+iy]; }
                        out["status"]=Fnv(stat,1024);
                    }
                    else if(cls=="CHandShaker") {
                        void* hs=smallBuf.data(); fill(hs,0x94);
                        for(size_t k=0;k<0x3C;k+=4)At<float>(hs,k)=uni(-0.03f,0.03f);
                        for(size_t k=0xC;k<0x18;k+=4)At<float>(hs,k)=next()%10==0 ? (next()%2 ? 0.f : -0.01f) : uni(0.002f,0.03f);
                        for(size_t k=0x18;k<0x24;k+=4)At<float>(hs,k)=uni(0.f,0.002f); for(size_t k=0x30;k<0x3C;k+=4)At<float>(hs,k)=uni(0.5f,2.f);
                        for(size_t k:{0x0,0x4,0x8,0x24,0x28,0x2C})if(next()%6==0)At<float>(hs,k)=next()%2 ? 0.f : -0.f;
                        for(size_t k=0x3C;k<0x7C;k+=4)At<float>(hs,k)=uni(-1.f,1.f); At<void*>(hs,0x7C)=nullptr; At<uint32>(hs,0x80)=0;
                        At<float>(hs,0x84)=uni(0.f,1.f); At<float>(hs,0x88)=uni(0.5f,2.f); At<int32>(hs,0x8C)=int32(next()%8==0 ? int32(next()%200)-100 : int32(next()%60)); At<float>(hs,0x90)=uni(0.f,0.004f);
                        CTimer::ms_fTimeStepNonClipped=next()%8==0 ? uni(0.f,40.f) : uni(0.f,4.f);
                        if(nm=="SetDefaults")reinterpret_cast<void(__thiscall*)(void*)>(a)(hs);
                        else if(nm=="Reset")reinterpret_cast<void(__thiscall*)(void*)>(a)(hs);
                        else reinterpret_cast<void(__thiscall*)(void*,float)>(a)(hs,uni(-3.f,3.f));
                        out["obj"]=ReversingProbe::Bytes(hs,0x94);
                    }
                    else if(cls=="CMouseControllerState") {
                        void* ms=smallBuf.data();
                        if(nm=="Constructor"){ std::memset(ms,0xCD,0x14); ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(ms)); }
                        else if(nm=="Clear"){ fill(ms,0x14); reinterpret_cast<void(__thiscall*)(void*)>(a)(ms); }
                        else { std::memset(ms,0,0x14); if(next()%3==0)static_cast<uint8*>(ms)[next()%7]=uint8(next()%2 ? 0x80 : next()); if(next()%9==0)static_cast<uint8*>(ms)[7]=uint8(next());
                            for(size_t k:{0x8,0xC,0x10}){ const uint32 r=next()%8; At<float>(ms,k)=r<4 ? 0.f : r==4 ? -0.f : r==5 ? 1e-30f : uni(-5.f,5.f); }
                            ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(ms); }
                        out["obj"]=ReversingProbe::Bytes(ms,0x14);
                    }
                    else if(cls=="COccluder") {
                        mkMat(mats[3].data(),CVector{uni(-200.f,200.f),uni(-200.f,200.f),uni(0.f,40.f)}); camMatrix=mats[3].data();
                        auto* view=reinterpret_cast<float*>(0xB6FA2C); for(int k=0;k<16;++k)view[k]=uni(-1.f,1.f); view[3]=view[7]=view[11]=view[15]=0.f; view[12]=uni(-50.f,50.f);view[13]=uni(-50.f,50.f);view[14]=uni(-20.f,40.f);
                        auto* g=reinterpret_cast<uint8*>(0xC73CA0); auto* valid=reinterpret_cast<uint8*>(0xC73CB0);
                        auto* coors=reinterpret_cast<float*>(0xC798E0); auto* center=reinterpret_cast<float*>(0xC79940); auto* screen=reinterpret_cast<float*>(0xC79950);
                        for(int k=0;k<4;++k)At<float>(g,k*4)=uni(-500.f,1500.f); for(int k=0;k<8;++k)valid[k]=uint8(next()%3!=0);
                        for(int k=0;k<24;++k)coors[k]=uni(-100.f,100.f); for(int k=0;k<8;++k){screen[k*3]=uni(-200.f,1000.f);screen[k*3+1]=uni(-200.f,800.f);screen[k*3+2]=uni(-5.f,100.f);}
                        center[0]=uni(0.f,640.f); center[1]=uni(0.f,480.f); center[2]=uni(-10.f,100.f);
                        void* occ=smallBuf.data(); fill(occ,0x20); const CVector cam=reinterpret_cast<CMatrix*>(mats[3].data())->GetPosition(); projCam=cam; projOff=uni(-60.f,120.f);
                        At<int16>(occ,0)=int16((cam.x+uni(-300.f,300.f))*4.f); At<int16>(occ,2)=int16((cam.y+uni(-300.f,300.f))*4.f); At<int16>(occ,4)=int16(uni(-20.f,60.f)*4.f);
                        for(size_t k:{6,8,10})At<int16>(occ,k)=next()%5==0 ? int16(0) : int16(next()%800);
                        if(next()%3==0)At<int16>(occ,6+2*(next()%3))=0;
                        void* act=ent[2].data(); fill(act,0xAC); At<uint8>(act,0x7A)=uint8(next()%6);
                        if(nm=="NearCamera")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(occ);
                        else if(nm=="ProcessLineSegment"){ const int32 i0=int32(next()%8), i1=next()%8==0 ? i0 : int32(next()%8); ret=reinterpret_cast<bool(__thiscall*)(void*,int32,int32,void*)>(a)(occ,i0,i1,act); }
                        else ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(occ,act);
                        out["active"]=ReversingProbe::Bytes(act,0xAC); out["globals"]=ReversingProbe::Bytes(g,0x18); out["coors"]=ReversingProbe::Bytes(coors,0xD0);
                    }
                    else if(cls=="CPlaneTrail") {
                        void* pt=smallBuf.data(); auto& now=CTimer::m_snTimeInMilliseconds; now=100000+next()%1000000;
                        for(int k=0;k<48;++k)At<float>(pt,k*4)=uni(-3000.f,3000.f); for(int k=0;k<16;++k)At<uint32>(pt,0xC0+k*4)=next()%4==0 ? 0u : now-(next()%5==0 ? next()%100000 : next()%32000);
                        fill(reinterpret_cast<void*>(0xC4D958),16*0x24);
                        if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(pt);
                        else if(nm=="RegisterPoint"){ if(next()%4==0)At<uint32>(pt,0xC0)=0; reinterpret_cast<void(__thiscall*)(void*,CVector)>(a)(pt,CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,800.f)}); }
                        else { const float in=next()%8==0 ? uni(0.f,0.01f) : uni(0.f,1.2f); reinterpret_cast<void(__thiscall*)(void*,float)>(a)(pt,in); out["temp"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC4D958),16*0x24); }
                        out["trail"]=ReversingProbe::Bytes(pt,0x100);
                    }
                    else if(cls=="CPlantLocTri") {
                        for(auto& p:props){ fill(p.data(),p.size()); At<uint16>(p.data(),0)=uint16(next()%3); for(int k=0;k<3;++k){ auto* d=p.data()+4+k*0x28; for(size_t o=8;o<0x28;o+=4)At<float>(d,o)=uni(0.f,2.f); At<float>(d,0x1C)=next()%3==0 ? 0.f : uni(0.f,next()%5==0 ? 400.f : 3.f); } }
                        if(nm=="GetPlantDensity"){ void* t=tris[0].data(); const CVector b{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                            for(int k=0;k<3;++k){ At<float>(t,k*12)=b.x+uni(-20.f,20.f); At<float>(t,k*12+4)=b.y+uni(-20.f,20.f); At<float>(t,k*12+8)=b.z+uni(-5.f,5.f); }
                            ret=F(CallEax(a,t)); }
                        else if(nm=="Add"){ void* t=tris[0].data(); fill(t,0x54); const CVector b{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                            CVector p[3]; for(auto& q:p)q=CVector{b.x+uni(-15.f,15.f),b.y+uni(-15.f,15.f),b.z+uni(-4.f,4.f)};
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,const CVector*,const CVector*,uint8,uint8,bool,bool)>(a)(t,&p[0],&p[1],&p[2],uint8(next()%180),uint8(next()),next()%3!=0,next()%2));
                            out["tri"]=ReversingProbe::Bytes(t,0x54); }
                        else { auto* heads=reinterpret_cast<uint8**>(0xC03984); for(int k=0;k<5;++k)heads[k]=nullptr; std::array<int,8> lst{}; std::array<uint8*,5> tails{};
                            for(int k=0;k<8;++k){ uint8* t=tris[k].data(); fill(t,0x54); lst[k]=int(next()%5); At<uint8*>(t,0x4C)=nullptr; At<uint8*>(t,0x50)=tails[lst[k]];
                                if(tails[lst[k]])At<uint8*>(tails[lst[k]],0x4C)=t; else heads[lst[k]]=t; tails[lst[k]]=t; }
                            int target=int(next()%8); for(int k=0;k<8&&lst[target]==0;++k)target=(target+1)%8;
                            uint8* t=tris[target].data(); At<uint8>(t,0x48)=uint8(next()); At<uint8>(t,0x46)=uint8(next()%180);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(t);
                            out["tris"]=ReversingProbe::Bytes(tris.data(),sizeof(tris)); json h=json::array(); for(int k=0;k<5;++k)h.push_back(Id(heads[k])); out["heads"]=h; }
                    }
                    else if(cls=="CStencilShadowObject") {
                        if(nm!="Destroy"){ void* s=shadows[0].data(); fill(s,0x1C); const auto r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(s); if(nm=="Constructor")ret=Id(r); out["obj"]=ReversingProbe::Bytes(s,0x1C); }
                        else { auto& avail=*reinterpret_cast<uint8**>(0xC6A168); auto& active=*reinterpret_cast<uint8**>(0xC6A16C); avail=active=nullptr; std::array<uint8*,2> tails{};
                            std::array<int,6> lst{}; for(int k=0;k<6;++k){ uint8* s=shadows[k].data(); fill(s,0x1C); lst[k]=int(next()%3!=0); At<uint8*>(s,0x14)=nullptr; At<uint8*>(s,0x18)=tails[lst[k]];
                                if(tails[lst[k]])At<uint8*>(tails[lst[k]],0x14)=s; else (lst[k] ? active : avail)=s; tails[lst[k]]=s; }
                            int target=int(next()%6); for(int k=0;k<6&&lst[target]==0;++k)target=(target+1)%6; uint8* s=shadows[target].data();
                            At<void*>(s,0)=next()%3 ? ent[0].data() : nullptr; At<uint32>(s,0x10)=next()%3 ? 0x6000000+next()%0x1000 : 0;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(s);
                            out["objs"]=ReversingProbe::Bytes(shadows.data(),sizeof(shadows)); out["heads"]=json::array({Id(avail),Id(active)}); }
                    }
                    else if(cls=="CWeaponModelInfo") {
                        void* mi=smallBuf.data(); fill(mi,0x28); At<uintptr_t>(mi,0)=0x85BD78;
                        if(nm=="GetModelType")ret=int(reinterpret_cast<uint8(__thiscall*)(void*)>(a)(mi));
                        else if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(mi);
                        else { fill(atomic.data(),atomic.size()); fill(geom.data(),geom.size()); fill(material.data(),material.size()); At<void*>(atomic.data(),0x18)=geom.data(); At<void*>(geom.data(),0x20)=matPtrs.data(); matPtrs[0]=material.data();
                            At<float>(material.data(),0xC)=uni(0.f,2.f); frameResult=next()%4 ? frames[0].data() : nullptr; firstObjectResult=next()%4 ? atomic.data() : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*,RpClump*)>(a)(mi,reinterpret_cast<RpClump*>(clumps[0].data()));
                            out["atomic"]=ReversingProbe::Bytes(atomic.data(),0x20); out["material"]=ReversingProbe::Bytes(material.data(),0x20); }
                        out["obj"]=ReversingProbe::Bytes(mi,0x28);
                    }
                    else if(cls=="NodeNamePlugin"||cls=="PipelinePlugin") {
                        const bool node=cls=="NodeNamePlugin"; auto& offset=*reinterpret_cast<RwInt32*>(node ? 0xC87C5C : 0x8D6080);
                        void* o1=plugObj[0].data(); void* o2=plugObj[1].data(); fill(o1,0x200); fill(o2,0x200);
                        auto randOffset=[&]{ const uint32 r=next()%6; return r==0 ? -1 : r==1 ? 0 : 0x40+4*int32(next()%32); };
                        if(nm=="NodeNamePluginAttach"||nm=="PipelinePluginAttach"){
                            regResult=randOffset(); streamResult=next()%4==0 ? -int32(next()%3+1) : int32(next()%0x40); objCbs={}; streamCbs={}; streamState=next();
                            ret=reinterpret_cast<RwBool(__cdecl*)()>(a)(); out["offset"]=offset;
                            // se ejecutan los callbacks capturados sobre objetos falsos (DLL en el candidato, EXE en el original)
                            if(offset>=0x40&&offset<0x100&&objCbs[0]&&objCbs[2]&&streamCbs[0]&&streamCbs[2]){
                                const RwInt32 sz=node ? 0x18 : 4; char* n2=static_cast<char*>(o2)+offset; const int len=int(next()%30); for(int k=0;k<len&&k<24;++k)n2[k]=char('A'+next()%26); if(len<24)n2[len]=0;
                                json cb=json::array();
                                cb.push_back(Id(reinterpret_cast<void*(__cdecl*)(void*,RwInt32,RwInt32)>(objCbs[0])(o1,offset,sz)));
                                if(objCbs[1])cb.push_back(Id(reinterpret_cast<void*(__cdecl*)(void*,RwInt32,RwInt32)>(objCbs[1])(o1,offset,sz)));
                                cb.push_back(Id(reinterpret_cast<void*(__cdecl*)(void*,const void*,RwInt32,RwInt32)>(objCbs[2])(o1,o2,offset,sz)));
                                const RwInt32 rl=node ? RwInt32(next()%24) : 4; auto* stream=reinterpret_cast<RwStream*>(0x1234);
                                cb.push_back(uintptr_t(reinterpret_cast<RwStream*(__cdecl*)(RwStream*,RwInt32,void*,RwInt32,RwInt32)>(streamCbs[0])(stream,rl,o2,offset,sz)));
                                if(streamCbs[1])cb.push_back(uintptr_t(reinterpret_cast<RwStream*(__cdecl*)(RwStream*,RwInt32,const void*,RwInt32,RwInt32)>(streamCbs[1])(stream,rl,o2,offset,sz)));
                                cb.push_back(reinterpret_cast<RwInt32(__cdecl*)(const void*,RwInt32,RwInt32)>(streamCbs[2])(o2,offset,sz));
                                out["callbacks"]=cb; out["o1"]=ReversingProbe::Bytes(o1,0x120); out["o2"]=ReversingProbe::Bytes(o2,0x120); }
                        }
                        else if(nm=="GetFrameNodeName"){ offset=randOffset(); ret=Id(reinterpret_cast<const char*(__cdecl*)(void*)>(a)(o1)); }
                        else if(nm=="SetFrameNodeName"){ offset=randOffset(); char name[48]{}; const int len=int(next()%40); for(int k=0;k<len;++k)name[k]=char('a'+next()%26);
                            reinterpret_cast<void(__cdecl*)(void*,const char*)>(a)(o1,name); out["o1"]=ReversingProbe::Bytes(o1,0x120); }
                        else if(nm=="GetPipelineID"){ offset=0x40+4*int32(next()%32); ret=reinterpret_cast<RwUInt32(__cdecl*)(void*)>(a)(o1); }
                        else { offset=0x40+4*int32(next()%32); reinterpret_cast<void(__cdecl*)(void*,RwUInt32)>(a)(o1,next()); out["o1"]=ReversingProbe::Bytes(o1,0x120); }
                    }
                    else if(cls=="WaterCreature_c") {
                        auto& noTemp=CObject::nNoTempObjects; const CVector base{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(-30.f,5.f)};
                        auto mkObj=[&](int k,bool withMatrix){ void* o=wobj[k].data(); fill(o,0x200); At<void*>(o,0)=objVt.data(); At<void*>(o,0x18)=nullptr;
                            At<float>(o,4)=base.x+uni(-10.f,10.f); At<float>(o,8)=base.y+uni(-10.f,10.f); At<float>(o,12)=base.z+uni(-3.f,3.f); At<float>(o,16)=uni(-3.2f,3.2f);
                            if(withMatrix){ mkMat(mats[k].data(),CVector{base.x+uni(-10.f,10.f),base.y+uni(-10.f,10.f),base.z+uni(-3.f,3.f)}); At<void*>(o,0x14)=mats[k].data(); } else At<void*>(o,0x14)=nullptr; return o; };
                        auto mkCreature=[&](int k,void* obj){ void* c=wc[k].data(); fill(c,0x34); At<void*>(c,8)=obj; At<uint8>(c,0xC)=uint8(next()%7); At<uint8>(c,0xE)=uint8(next()%3==0 ? next()%60 : (next()%9==0 ? 200 : 0));
                            At<int16>(c,0x10)=int16(next()%3000); At<int16>(c,0x12)=int16(next()%12==0 ? 0 : 1000+next()%3500); At<float>(c,0x14)=uni(0.001f,0.06f); At<float>(c,0x18)=uni(0.001f,0.2f);
                            At<void*>(c,0x1C)=nullptr; for(size_t o:{0x20,0x24,0x28})At<float>(c,o)=uni(-3.f,3.f); At<float>(c,0x2C)=uni(-4.f,4.f); At<uint8>(c,0x30)=uint8(next()%2); return c; };
                        if(nm=="Exit"){ void* o=next()%6 ? mkObj(0,true) : nullptr; std::array<uint8*,3> cr{}; for(int k=0;k<3;++k)cr[k]=static_cast<uint8*>(mkCreature(k,k==0 ? o : wobj[1].data()));
                            auto* pool=reinterpret_cast<uint8*>(0xC1F930); auto* list=reinterpret_cast<uint8*>(0xC1F93C);
                            // lista activa: wc[0] y quiza wc[1]/wc[2]; la reserva queda vacia o con wc[2]
                            std::vector<uint8*> act{cr[0]}; if(next()%2)act.insert(act.begin()+int(next()%2),cr[1]); const bool inPool=next()%2; if(!inPool&&next()%2)act.push_back(cr[2]);
                            for(size_t k=0;k<act.size();++k){ At<uint8*>(act[k],0)=k ? act[k-1] : nullptr; At<uint8*>(act[k],4)=k+1<act.size() ? act[k+1] : nullptr; }
                            At<uint8*>(list,0)=act.front(); At<uint8*>(list,4)=act.back(); At<uint32>(list,8)=uint32(act.size());
                            if(inPool){ At<uint8*>(cr[2],0)=nullptr; At<uint8*>(cr[2],4)=nullptr; At<uint8*>(pool,0)=cr[2]; At<uint8*>(pool,4)=cr[2]; At<uint32>(pool,8)=1; } else { At<uint8*>(pool,0)=nullptr; At<uint8*>(pool,4)=nullptr; At<uint32>(pool,8)=0; }
                            noTemp=uint16(next()%200);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(cr[0]);
                            out["lists"]=json::array({Id(At<void*>(pool,0)),Id(At<void*>(pool,4)),At<uint32>(pool,8),Id(At<void*>(list,0)),Id(At<void*>(list,4)),At<uint32>(list,8)});
                            json links=json::array(); for(auto* c:cr)links.push_back({Id(At<void*>(c,0)),Id(At<void*>(c,4)),Id(At<void*>(c,8))}); out["links"]=links; }
                        else if(nm=="Init"){ void* me=wc[0].data(); fill(me,0x34); void* par=nullptr; if(next()%3==0){ par=mkCreature(1,mkObj(1,next()%4!=0)); }
                            const int32 type=int32(next()%7); CVector pos{base.x,base.y,base.z}; const float level=uni(-2.f,20.f), depth=next()%6==0 ? uni(0.f,4.f) : uni(5.f,80.f);
                            noTemp=uint16(next()%10==0 ? 150+next()%20 : next()%150); if(!dllObjCtor)noTemp=150; // sin constructor localizado solo se prueba la salida temprana
                            fill(wobj[2].data(),0x200); newResult=dllObjCtor ? wobj[2].data() : nullptr; // operator new no es noexcept: nunca se devuelve nulo
                            if(next()%4){ mkMat(mats[2].data(),CVector{0.f,0.f,0.f}); ctorMatrix=mats[2].data(); } else ctorMatrix=nullptr;
                            ret=reinterpret_cast<bool(__thiscall*)(void*,int32,CVector*,void*,float,float)>(a)(me,type,&pos,par,level,depth);
                            out["me"]=ReversingProbe::Bytes(me,0x34); out["obj"]=ReversingProbe::Bytes(wobj[2].data(),0x140); out["mat"]=ReversingProbe::Bytes(mats[2].data(),0x48);
                            out["pos"]=json::array({F(pos.x),F(pos.y),F(pos.z)}); out["pool_lock"]=*poolLock; *poolLock=savedPoolLock; }
                        else { void* o=mkObj(0,next()%5!=0); void* me=mkCreature(0,o); if(next()%2){ void* po=mkObj(1,next()%5!=0); At<void*>(me,0x1C)=mkCreature(1,po); }
                            mkMat(mats[3].data(),CVector{base.x+uni(-80.f,80.f),base.y+uni(-80.f,80.f),base.z+uni(-5.f,20.f)}); camMatrix=mats[3].data();
                            void* ped=ent[3].data(); fill(ped,0x40); At<void*>(ped,0x14)=nullptr; At<float>(ped,4)=At<float>(o,4)+uni(-8.f,8.f); At<float>(ped,8)=At<float>(o,8)+uni(-8.f,8.f); At<float>(ped,12)=At<float>(o,12)+uni(-2.f,2.f); playerResult=ped;
                            colNormal=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; waterLevel=uni(-5.f,5.f);
                            reinterpret_cast<void(__thiscall*)(void*,float)>(a)(me,next()%6==0 ? 1.f : uni(0.1f,3.f));
                            out["me"]=ReversingProbe::Bytes(me,0x34); out["obj"]=ReversingProbe::Bytes(o,0x20); out["mat"]=ReversingProbe::Bytes(mats[0].data(),0x48); out["follow"]=ReversingProbe::Bytes(wc[1].data(),0x34); }
                        out["no_temp"]=noTemp;
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll(); *poolLock=savedPoolLock;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            json summary={{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}};
            if(full=="WaterCreature_c::Init")summary["dll_object_ctor_found"]=dllObjCtor!=0;
            Write(summary);
        }
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll(); *poolLock=savedPoolLock;
    bool restored=*poolLock==savedPoolLock; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
