#pragma once
// Banco dentro del motor: camara de sombras y cristales (lista de 19; 37 funciones). CShadowCamera y CGlass.
// Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas.
// CShadowCamera: nunca se toca el dispositivo, la camara ni los rasters reales de RW: camara, frames, luz, rasters,
// textura, clump, atomics y geometrias son buferes propios. Se simulan y trazan los estados de render y las primitivas
// Im2D (punteros de RwEngineInstance->dOpenDevice; zBufferNear se fija por caso), RwCameraBeginUpdate/EndUpdate/Clear
// (direccion del EXE y simbolo de la DLL: no son envoltorios), GetFirstAtomic (EXE + DLL), RpClumpForAllAtomics
// (llama al callback recibido sobre los atomics falsos), AtomicDefaultRenderCallBack y las primitivas RW de
// camara/frame/raster/textura (envoltorios del EXE). Los enteros del degradado (0x8D5218/0x8D521C) se guardan.
// CGlass: paneles, centros, contadores de polys y entidades a renderizar se guardan y restauran en cada ejecucion;
// objetos, vehiculo, model infos (huecos 19990..19993 sustituidos), modelos de colision, nodos de listas y el pool de
// objetos (puntero 0xB7449C sustituido) son falsos. Los sectores del mundo se vacian solo durante
// HasGlassBeenShatteredAtCoors. Se simulan y trazan rand, el audio de cristales, FindGroundZFor3DCoord,
// CalculateTrianglePlanes, TestSphereTriangle, ClearScanCodes, CFallingGlassPane::Update/Render, Im3D y los estados.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ShadowCamera.h"
#include "Glass.h"
#include "FallingGlassPane.h"
#include "../ProbeRuntime.hpp"

namespace Render19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x10> scSelf{};
alignas(16) inline std::array<uint8,0x100> rwCam{}, camFrame{}, lightFrame{};
alignas(16) inline std::array<uint8,0x40> light{}, clump{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> rasters{};
alignas(16) inline std::array<uint8,0x60> rwTex{};
alignas(16) inline std::array<std::array<uint8,0x80>,3> atoms{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> geos{};
alignas(16) inline std::array<uint8,0x10> argBuf{};
constexpr int FAKE_MI=19990, NUM_MI=4, NUM_OBJ=6, OBJ_SIZE=0x19C, NUM_NODES=24;
alignas(16) inline std::array<std::array<uint8,0x40>,NUM_MI> mis{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> colModels{}, colDatas{};
alignas(16) inline std::array<std::array<uint8,0x60>,2> colVerts{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> colTris{};
alignas(16) inline std::array<std::array<uint8,0x50>,2> colPlanes{}, colPlanes2{};
alignas(16) inline std::array<uint8,NUM_OBJ*OBJ_SIZE> objStore{};
alignas(16) inline std::array<uint8,NUM_OBJ> objStates{};
alignas(16) inline std::array<uint8,0x14> fakePool{};
alignas(16) inline std::array<uint8,0x600> vehicle{};
alignas(16) inline std::array<std::array<uint8,0x48>,8> mats{};
alignas(16) inline std::array<std::array<uint8,12>,NUM_NODES> nodes{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> glassTex{};
alignas(16) inline std::array<uint8,4> listHead{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline json Val(const void* p){ const auto id=Id(p); return id.is_string() ? json(uintptr_t(p)) : id; }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
inline uint8* Obj(int k){ return objStore.data()+k*OBJ_SIZE; }

// --- RW: estados, Im2D, camara ---
inline bool maskUV{};
inline uint32 beginMask{}; inline int beginCalls{};
inline json Verts(const RwIm2DVertex* v,int32 n){ json r=json::array(); for(int32 k=0;k<std::min<int32>(n,8);++k){
    json e={F(v[k].x),F(v[k].y),F(v[k].z),F(v[k].rhw),v[k].emissiveColor}; if(!maskUV){ e.push_back(F(v[k].u)); e.push_back(F(v[k].v)); } r.push_back(e); } return r; }
inline RwBool __cdecl FStateSet(RwRenderState st,void* v){ trace.push_back({"rs",int(st),Val(v)}); return TRUE; }
inline RwBool __cdecl FIm2DPrim(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 n){ trace.push_back({"im2d_prim",int(t),n,Verts(v,n)}); return TRUE; }
inline RwBool __cdecl FIm2DIdx(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 n,RwImVertexIndex* idx,RwInt32 ni){ json ix=json::array(); for(int32 k=0;k<std::min<int32>(ni,8);++k)ix.push_back(idx[k]);
    trace.push_back({"im2d_idx",int(t),n,Verts(v,n),ni,ix}); return TRUE; }
inline RwBool __cdecl FIm2DLine(RwIm2DVertex* v,RwInt32 n,RwInt32 a,RwInt32 b){ trace.push_back({"im2d_line",n,a,b,Verts(v,n)}); return TRUE; }
inline RwCamera* __cdecl FBegin(RwCamera* c){ const bool ok=((beginMask>>(beginCalls++&31))&1)!=0; trace.push_back({"begin_update",Id(c),Id(At<void*>(c,0x60)),ok}); return ok ? c : nullptr; }
inline RwCamera* __cdecl FEnd(RwCamera* c){ trace.push_back({"end_update",Id(c),Id(At<void*>(c,0x60))}); return c; }
inline RwCamera* __cdecl FClear(RwCamera* c,RwRGBA* col,RwInt32 mode){ trace.push_back({"camera_clear",Id(c),*reinterpret_cast<uint32*>(col),mode}); return c; }
inline RwMatrix* __cdecl FMatUpdate(RwMatrix* m){ trace.push_back({"matrix_update",Id(m)}); return m; }
inline RwFrame* __cdecl FFrameUpdate(RwFrame* f){ trace.push_back({"frame_update_objects",Id(f)}); return f; }
inline RwFrame* __cdecl FFrameOrtho(RwFrame* f){ trace.push_back({"frame_orthonormalize",Id(f)}); return f; }
inline RwBool __cdecl FFrameDestroy(RwFrame* f){ trace.push_back({"frame_destroy",Id(f)}); return TRUE; }
inline void* frameCreateRes{}; inline void* camCreateRes{}; inline void* rasterCreateRes{}; inline void* texCreateRes{};
inline RwFrame* __cdecl FFrameCreate(){ trace.push_back({"frame_create"}); return static_cast<RwFrame*>(frameCreateRes); }
inline void __cdecl FSetFrame(void* o,RwFrame* f){ trace.push_back({"object_set_frame",Id(o),Id(f)}); At<void*>(o,4)=f; }
inline RwBool __cdecl FRasterDestroy(RwRaster* r){ trace.push_back({"raster_destroy",Id(r),Id(At<void*>(rwCam.data(),0x60))}); return TRUE; }
inline RwRaster* __cdecl FRasterCreate(RwInt32 w,RwInt32 h,RwInt32 d,RwInt32 fl){ trace.push_back({"raster_create",w,h,d,fl}); return static_cast<RwRaster*>(rasterCreateRes); }
inline RwTexture* __cdecl FTexSetRaster(RwTexture* t,RwRaster* r){ trace.push_back({"texture_set_raster",Id(t),Id(r)}); return t; }
inline RwBool __cdecl FTexDestroy(RwTexture* t){ trace.push_back({"texture_destroy",Id(t)}); return TRUE; }
inline RwTexture* __cdecl FTexCreate(RwRaster* r){ trace.push_back({"texture_create",Id(r)}); return static_cast<RwTexture*>(texCreateRes); }
inline RwBool __cdecl FCamDestroy(RwCamera* c){ trace.push_back({"camera_destroy",Id(c)}); return TRUE; }
inline RwCamera* __cdecl FCamCreate(){ trace.push_back({"camera_create"}); return static_cast<RwCamera*>(camCreateRes); }
inline RwCamera* __cdecl FCamProjection(RwCamera* c,RwCameraProjection p){ trace.push_back({"camera_set_projection",Id(c),int(p)}); return c; }
inline RwCamera* __cdecl FCamFar(RwCamera* c,RwReal f){ trace.push_back({"camera_far",Id(c),F(f)}); return c; }
inline RwCamera* __cdecl FCamNear(RwCamera* c,RwReal f){ trace.push_back({"camera_near",Id(c),F(f)}); return c; }
inline RwCamera* __cdecl FCamWindow(RwCamera* c,const RwV2d* v){ trace.push_back({"camera_view_window",Id(c),F(v->x),F(v->y)}); return c; }
inline void* firstAtomicRes{}; inline int clumpAtoms{};
inline RpAtomic* __cdecl FFirstAtomic(RpClump* c){ trace.push_back({"get_first_atomic",Id(c)}); return static_cast<RpAtomic*>(firstAtomicRes); }
inline RpClump* __cdecl FForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* data){
    trace.push_back({"clump_for_all_atomics",Id(c),uintptr_t(cb)==0x705620 ? json("exe_quick_render") : json(uintptr_t(cb)),uintptr_t(data)});
    for(int k=0;k<clumpAtoms;++k)if(!cb(reinterpret_cast<RpAtomic*>(atoms[k].data()),data))break;
    return c; }
inline RpAtomic* __cdecl FDefaultRender(RpAtomic* a){ trace.push_back({"atomic_default_render",Id(a),uintptr_t(At<void*>(a,0x48))}); return a; }

// --- cristales ---
inline int32 PaneIdx(const void* p){ return int32((intptr_t(p)-intptr_t(0xC71BF8))/0x70); }
inline void __fastcall FGlassAudio(void*,void*,int32 ev,const CVector* p){ trace.push_back({"glass_audio",ev,Vec(*p)}); }
inline bool groundHit{}; inline float groundZ{};
inline float __cdecl FGround(float x,float y,float z,bool* hit,CEntity** e){ trace.push_back({"ground_z",F(x),F(y),F(z),hit!=nullptr,e!=nullptr}); if(hit)*hit=groundHit; return groundZ; }
inline void* planesAfterCalc{};
inline void __cdecl FCalcPlanes(CColModel* cm){ trace.push_back({"calc_planes",Id(cm)});
    auto* cd=At<uint8*>(cm,0x2C); if(cd&&!At<void*>(cd,0x1C))At<void*>(cd,0x1C)=planesAfterCalc; }
inline uint32 sphereMask{}; inline int sphereCalls{};
inline bool __cdecl FSphereTri(const CColSphere* s,const void* verts,const void* tri,const void* plane){
    const auto* f=reinterpret_cast<const float*>(s); const bool r=((sphereMask>>(sphereCalls++&31))&1)!=0;
    trace.push_back({"sphere_triangle",F(f[0]),F(f[1]),F(f[2]),F(f[3]),Id(verts),Id(tri),Id(plane),r}); return r; }
inline void __cdecl FClearScanCodes(){ trace.push_back({"clear_scan_codes"}); }
inline void __fastcall FPaneUpdate(void* p,void*){ trace.push_back({"pane_update",PaneIdx(p)}); }
inline void __fastcall FPaneRender(void* p,void*){ trace.push_back({"pane_render",PaneIdx(p)}); }
inline bool im3dOk{};
inline void* __cdecl FIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ trace.push_back({"im3d_transform",uintptr_t(v),n,uintptr_t(m),f}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ trace.push_back({"im3d_render_indexed",int(t),uintptr_t(idx),n}); return TRUE; }
inline RwBool __cdecl FIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=37;
inline const std::array<Fn,NFN> fns{{
    {"CShadowCamera","Constructor",0x7053F0,0,-1,64},{"CShadowCamera","Create",0x705B60,0,-1,512},{"CShadowCamera","Destroy",0x705400,0,-1,256},
    {"CShadowCamera","Destructor",0x705B50,0,-1,128},{"CShadowCamera","DrawBlurryRaster2",0x706330,0,-1,256},{"CShadowCamera","DrawOutlineBorder",0x705790,0,-1,256},
    {"CShadowCamera","GetRwRenderRaster",0x705770,0,-1,64},{"CShadowCamera","GetRwRenderTexture",0x705780,0,-1,64},{"CShadowCamera","Im2DRenderQuad",0x705A20,0,-1,256},
    {"CShadowCamera","InvertRaster",0x705660,0,-1,256},{"CShadowCamera","MakeGradientRaster",0x705D20,0,-1,256},{"CShadowCamera","RasterBlur",0x706170,0,-1,256},
    {"CShadowCamera","RasterResample",0x706070,0,-1,256},{"CShadowCamera","SetCenter",0x705590,0,-1,256},{"CShadowCamera","SetFrustum",0x7054C0,0,-1,256},
    {"CShadowCamera","SetLight",0x705520,0,-1,256},{"CShadowCamera","Update-Atomic",0x705C80,0,-1,256},{"CShadowCamera","Update-Clump",0x705BF0,0,-1,256},
    {"CShadowCamera","atomicQuickRender",0x705620,0,-1,256},
    {"CGlass","AskForObjectToBeRenderedInGlass",0x71ACD0,0,-1,128},{"CGlass","BreakGlassPhysically",0x71CF50,0,-1,512},{"CGlass","CarWindscreenShatters",0x71C2B0,0,-1,512},
    {"CGlass","FindFreePane",0x71ACA0,0,-1,128},{"CGlass","FindWindowSectorList<CPtrListSingleLink<CPhysical*>>",0x71AFC0,0,-1,512},
    {"CGlass","GeneratePanesForWindow",0x71B620,0,-1,1024},{"CGlass","HasGlassBeenShatteredAtCoors",0x71CB70,0,-1,512},{"CGlass","Init",0x71A8D0,0,-1,64},
    {"CGlass","IsObjectGlass",0x46A760,0,-1,128},{"CGlass","Render",0x71CE20,0,-1,256},{"CGlass","RenderHiLightPolys",0x71ADA0,0,-1,128},
    {"CGlass","RenderReflectionPolys",0x71AED0,0,-1,128},{"CGlass","RenderShatteredPolys",0x71AE30,0,-1,128},{"CGlass","Update",0x71B0D0,0,-1,64},
    {"CGlass","WasGlassHitByBullet",0x71C0D0,0,-1,256},{"CGlass","WindowRespondsToCollision",0x71BC40,0,-1,512},{"CGlass","WindowRespondsToExplosion",0x71C1A0,0,-1,256},
    {"CGlass","WindowRespondsToSoftCollision",0x71AF70,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"render19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"render19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("render19");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"render19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    // vtables reales de model info: una atomica (AsAtomicModelInfoPtr devuelve this) y otra que no lo es
    void* atomicVptr{}; void* otherVptr{};
    for(int32 k=0;k<20000&&!(atomicVptr&&otherVptr);++k){ auto* mi=CModelInfo::ms_modelInfoPtrs[k]; if(!mi)continue;
        if(mi->AsAtomicModelInfoPtr()==mi){ if(!atomicVptr)atomicVptr=*reinterpret_cast<void**>(mi); } else if(!mi->AsAtomicModelInfoPtr()){ if(!otherVptr)otherVptr=*reinterpret_cast<void**>(mi); } }
    if(!atomicVptr||!otherVptr){Write({{"event","error"},{"reason","model info vtables not found"}});return;}
    // superficies de cristal (lectura de datos reales)
    std::vector<uint8> glassSurfaces, otherSurfaces;
    for(uint32 s=0;s<179;++s)(g_surfaceInfos.IsGlass(s) ? glassSurfaces : otherSurfaces).push_back(uint8(s));
    if(glassSurfaces.empty()||otherSurfaces.empty()){Write({{"event","error"},{"reason","surface infos"}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){
        Reg(scSelf.data(),scSelf.size()); Reg(rwCam.data(),rwCam.size()); Reg(camFrame.data(),camFrame.size()); Reg(lightFrame.data(),lightFrame.size());
        Reg(light.data(),light.size()); Reg(clump.data(),clump.size()); for(auto& r:rasters)Reg(r.data(),r.size()); Reg(rwTex.data(),rwTex.size());
        for(auto& a:atoms)Reg(a.data(),a.size()); for(auto& g:geos)Reg(g.data(),g.size()); Reg(argBuf.data(),argBuf.size());
        for(auto& m:mis)Reg(m.data(),m.size()); for(auto& c:colModels)Reg(c.data(),c.size()); for(auto& c:colDatas)Reg(c.data(),c.size());
        for(auto& c:colVerts)Reg(c.data(),c.size()); for(auto& c:colTris)Reg(c.data(),c.size()); for(auto& c:colPlanes)Reg(c.data(),c.size()); for(auto& c:colPlanes2)Reg(c.data(),c.size());
        Reg(objStore.data(),objStore.size()); Reg(vehicle.data(),vehicle.size()); for(auto& m:mats)Reg(m.data(),m.size()); for(auto& n:nodes)Reg(n.data(),n.size());
        for(auto& t:glassTex)Reg(t.data(),t.size()); Reg(listHead.data(),listHead.size());
        Reg(reinterpret_cast<void*>(0xC71BF8),45*0x70); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved, savedSectors;
    auto* const dev=&RWSRCGLOBAL(dOpenDevice);
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xC71B18,0x1494},                                  // CGlass: indices de polys, centros, entidades, 45 paneles, LastColCheckMS
            {0x8D5CD8,0x78},                                    // CGlass::PanePolyPositions
            {0xC403F4,0x18},                                    // gpShadowExplosionTex .. gpCrackedGlassTex
            {0xB7CD78,2},{0xB7CB84,4},{0xB7C4CA,6},{0xB7449C,4}, // scan code, tiempo, cielo inferior, pool de objetos
            {0xA9B0C8+FAKE_MI*4,NUM_MI*4},                      // huecos de model info sustituidos
            {0x8D5218,8},                                       // enteros del degradado de CShadowCamera
            {uintptr_t(&dev->zBufferNear),4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{0xB7D0B8,120*120*8},{0xB992B8,16*16*12}})
        savedSectors.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    bool sectorsTouched=false;
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size());
        if(sectorsTouched){ for(auto& s:savedSectors)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); sectorsTouched=false; } };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        // RW: RwCameraBeginUpdate/EndUpdate/Clear de la DLL no son envoltorios (se parchean ambos)
        EntryPatch b1(0x7EE190,FBegin), b1d(reinterpret_cast<uintptr_t>(&RwCameraBeginUpdate),FBegin);
        EntryPatch b2(0x7EE180,FEnd), b2d(reinterpret_cast<uintptr_t>(&RwCameraEndUpdate),FEnd);
        EntryPatch b3(0x7EE340,FClear), b3d(reinterpret_cast<uintptr_t>(&RwCameraClear),FClear);
        EntryPatch ga(0x734820,FFirstAtomic), gad(reinterpret_cast<uintptr_t>(&GetFirstAtomic),FFirstAtomic);
        EntryPatch w1(0x7F18A0,FMatUpdate), w2(0x7F0910,FFrameUpdate), w3(0x7F1170,FFrameOrtho), w4(0x7F05A0,FFrameDestroy), w5(0x7F0410,FFrameCreate);
        EntryPatch w6(0x804EF0,FSetFrame), w7(0x7FB020,FRasterDestroy), w8(0x7FB230,FRasterCreate), w9(0x7F35D0,FTexSetRaster), w10(0x7F3820,FTexDestroy);
        EntryPatch w11(0x7F37C0,FTexCreate), w12(0x7EE4B0,FCamDestroy), w13(0x7EE4F0,FCamCreate), w14(0x7EE3A0,FCamProjection), w15(0x7EE2A0,FCamFar);
        EntryPatch w16(0x7EE1D0,FCamNear), w17(0x7EE410,FCamWindow), w18(0x749B70,FForAllAtomics), w19(0x7491C0,FDefaultRender);
        // cristales
        EntryPatch g1(0x506EE0,FGlassAudio), g1d(bit_cast<uintptr_t>(&CAudioEngine::ReportGlassCollisionEvent),FGlassAudio);
        EntryPatch g2(0x5696C0,FGround), g2d(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),FGround);
        EntryPatch g3(0x418580,FCalcPlanes), g3d(bit_cast<uintptr_t>(static_cast<void(*)(CColModel*)>(&CCollision::CalculateTrianglePlanes)),FCalcPlanes);
        EntryPatch g4(0x4165B0,FSphereTri), g4d(bit_cast<uintptr_t>(&CCollision::TestSphereTriangle),FSphereTri);
        EntryPatch g5(0x563470,FClearScanCodes), g5d(bit_cast<uintptr_t>(&CWorld::ClearScanCodes),FClearScanCodes);
        EntryPatch g6(0x71AA10,FPaneUpdate), g6d(bit_cast<uintptr_t>(&CFallingGlassPane::Update),FPaneUpdate);
        EntryPatch g7(0x71B100,FPaneRender), g7d(bit_cast<uintptr_t>(&CFallingGlassPane::Render),FPaneRender);
        EntryPatch i1(0x7EF450,FIm3DTransform), i2(0x7EF550,FIm3DRender), i3(0x7EF520,FIm3DEnd); // RwIm3D* de la DLL son envoltorios del EXE
        // dispositivo: estados y primitivas Im2D
        const RwDevice savedDev=*dev;
        dev->fpRenderStateSet=FStateSet; dev->fpIm2DRenderPrimitive=FIm2DPrim; dev->fpIm2DRenderIndexedPrimitive=FIm2DIdx; dev->fpIm2DRenderLine=FIm2DLine;
        auto** const miSlots=reinterpret_cast<void**>(0xA9B0C8);
        auto& scanCode=*reinterpret_cast<uint16*>(0xB7CD78); auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84);
        auto& objPool=*reinterpret_cast<void**>(0xB7449C); auto* const sky=reinterpret_cast<uint16*>(0xB7C4CA);
        auto& gradStart=*reinterpret_cast<int32*>(0x8D5218); auto& gradEnd=*reinterpret_cast<int32*>(0x8D521C);
        auto* const panes=reinterpret_cast<uint8*>(0xC71BF8);
        auto& reflV=*reinterpret_cast<int32*>(0xC71B18); auto& reflI=*reinterpret_cast<int32*>(0xC71B1C);
        auto& shatV=*reinterpret_cast<int32*>(0xC71B20); auto& shatI=*reinterpret_cast<int32*>(0xC71B24);
        auto& hiV=*reinterpret_cast<uint32*>(0xC71B28); auto& hiI=*reinterpret_cast<int32*>(0xC71B2C);
        auto& numGlassEnt=*reinterpret_cast<uint32*>(0xC71BD8); auto& lastCol=*reinterpret_cast<uint32*>(0xC72FA8);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1919u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    beginCalls=0; sphereCalls=0; maskUV=false;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CShadowCamera") {
                        // camara, frames, rasters, textura, atomics y geometrias deterministas
                        fill(rwCam.data(),rwCam.size()); fill(camFrame.data(),camFrame.size()); fill(lightFrame.data(),lightFrame.size()); fill(light.data(),light.size());
                        for(auto* fr:{camFrame.data(),lightFrame.data()})for(size_t o=0x10;o<0x50;o+=4)At<float>(fr,o)=uni(-2.f,2.f);
                        At<void*>(rwCam.data(),4)=next()%6 ? static_cast<void*>(camFrame.data()) : nullptr;
                        At<float>(rwCam.data(),0x80)=next()%16==0 ? 0.f : uni(0.01f,5.f); At<float>(rwCam.data(),0x84)=uni(0.5f,400.f);
                        for(auto& r:rasters){ fill(r.data(),r.size()); At<int32>(r.data(),0xC)=next()%5==0 ? int32(next()%600) : int32(1u<<(next()%10)); At<int32>(r.data(),0x10)=next()%5==0 ? int32(next()%600) : int32(1u<<(next()%10)); }
                        At<void*>(rwCam.data(),0x60)=next()%8 ? static_cast<void*>(rasters[0].data()) : nullptr;
                        fill(rwTex.data(),rwTex.size()); fill(clump.data(),clump.size()); clump[0]=2;
                        At<void*>(light.data(),4)=lightFrame.data();
                        for(auto& g:geos)fill(g.data(),g.size());
                        for(size_t k=0;k<atoms.size();++k){ fill(atoms[k].data(),atoms[k].size()); At<void*>(atoms[k].data(),0x18)=geos[k%2].data();
                            const uint32 cbk=next()%3; At<uintptr_t>(atoms[k].data(),0x48)=cbk==0 ? 0 : cbk==1 ? 0x7491C0 : 0x00ABC000+(next()%0x100)*4; }
                        std::memset(scSelf.data(),0,scSelf.size());
                        At<void*>(scSelf.data(),0)=next()%10 ? static_cast<void*>(rwCam.data()) : nullptr;
                        At<void*>(scSelf.data(),4)=next()%5 ? static_cast<void*>(rwTex.data()) : nullptr;
                        beginMask=next()%4==0 ? next() : (next()%5 ? 0xFFFFFFFFu : 0u);
                        dev->zBufferNear=uni(0.f,1.f);
                        auto* self=scSelf.data(); auto* cam=At<uint8*>(self,0);
                        if(nm=="Constructor"){ std::memset(self,0xCD,scSelf.size()); ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }
                        else if(nm=="Destroy"||nm=="Destructor"){ reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="SetFrustum"){ if(!cam)At<void*>(self,0)=rwCam.data(); const float r=next()%10==0 ? -uni(0.f,50.f) : next()%10==0 ? 0.f : uni(0.f,200.f);
                            reinterpret_cast<void(__thiscall*)(void*,float)>(a)(self,r); }
                        else if(nm=="SetLight"){ At<void*>(self,0)=rwCam.data(); At<void*>(rwCam.data(),4)=camFrame.data(); reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,light.data()); }
                        else if(nm=="SetCenter"){ At<void*>(self,0)=rwCam.data(); At<void*>(rwCam.data(),4)=camFrame.data(); At<CVector>(argBuf.data(),0)=randVec(-3000.f,3000.f);
                            reinterpret_cast<void(__thiscall*)(void*,const void*)>(a)(self,argBuf.data()); }
                        else if(nm=="InvertRaster"){ At<void*>(self,0)=rwCam.data(); At<void*>(rwCam.data(),0x60)=rasters[0].data(); maskUV=true; reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="GetRwRenderRaster"){ if(!cam)At<void*>(self,0)=rwCam.data(); ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }
                        else if(nm=="GetRwRenderTexture"){ ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }
                        else if(nm=="DrawOutlineBorder"){ At<void*>(self,0)=rwCam.data(); At<void*>(rwCam.data(),0x60)=rasters[0].data(); At<uint32>(argBuf.data(),0)=next(); maskUV=true;
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*,const void*)>(a)(self,argBuf.data())); }
                        else if(nm=="Create"){ const int32 power=next()%8==0 ? int32(next()%32) : int32(next()%12);
                            camCreateRes=next()%8 ? static_cast<void*>(rwCam.data()) : nullptr; frameCreateRes=next()%8 ? static_cast<void*>(camFrame.data()) : nullptr;
                            rasterCreateRes=next()%8 ? static_cast<void*>(rasters[0].data()) : nullptr; texCreateRes=next()%8 ? static_cast<void*>(rwTex.data()) : nullptr;
                            At<void*>(rwCam.data(),4)=nullptr; if(next()%6==0)At<void*>(rwCam.data(),0x60)=nullptr;
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(self,power)); }
                        else if(nm=="Update-Clump"||nm=="Update-Atomic"){ At<void*>(self,0)=rwCam.data(); At<void*>(rwCam.data(),0x60)=rasters[0].data(); maskUV=true;
                            firstAtomicRes=atoms[next()%atoms.size()].data(); clumpAtoms=int(next()%4);
                            if(nm=="Update-Clump")ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(self,clump.data()));
                            else ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(self,atoms[next()%atoms.size()].data())); }
                        else if(nm=="atomicQuickRender"){ auto* at=atoms[next()%atoms.size()].data(); ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(at,reinterpret_cast<void*>(uintptr_t(next()%3)*0x100))); }
                        else if(nm=="MakeGradientRaster"){ At<void*>(rwCam.data(),0x60)=rasters[0].data(); maskUV=true;
                            At<int32>(rasters[0].data(),0x10)=next()%8==0 ? int32(next()%5)-2 : next()%6==0 ? int32(100+next()%160) : int32(1+next()%40);
                            if(next()%3==0){ gradStart=int32(next()%400)-50; gradEnd=int32(next()%400)-50; }
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }
                        else if(nm=="RasterResample"){ At<void*>(rwCam.data(),0x60)=rasters[0].data(); /* el EXE lee el ancho del raster de la camara sin comprobarlo */ ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(self,rasters[1].data())); }
                        else if(nm=="RasterBlur"){ const int32 passes=next()%8==0 ? -int32(1+next()%3) : int32(1+next()%4); /* 0 pasadas: NOTSA de la DLL, fuera del banco */
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*,int32)>(a)(self,rasters[1].data(),passes)); }
                        else if(nm=="DrawBlurryRaster2"){ At<void*>(self,0)=rwCam.data(); ret=Id(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(self,rasters[2].data())); }
                        else if(nm=="Im2DRenderQuad"){ float v[7]; for(auto& x:v)x=uni(-600.f,600.f); v[4]=uni(0.f,1.f); v[5]=uni(0.f,10.f); v[6]=next()%4==0 ? 0.f : uni(-1.f,1.f);
                            ret=int(reinterpret_cast<uint8(__cdecl*)(float,float,float,float,float,float,float)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6])); }
                        out["self"]=ReversingProbe::Bytes(self,8); out["cam"]=ReversingProbe::Bytes(rwCam.data(),0x90);
                        out["cam_frame"]=ReversingProbe::Bytes(camFrame.data(),0x50); out["tex"]=ReversingProbe::Bytes(rwTex.data(),0x58);
                        json at=json::array(); for(auto& x:atoms)at.push_back(ReversingProbe::Bytes(x.data(),0x50)); out["atoms"]=at;
                        json ge=json::array(); for(auto& x:geos)ge.push_back(ReversingProbe::Bytes(x.data(),0x10)); out["geos"]=ge;
                    }
                    else {
                        // --- cristales: model infos, colision, objetos, matrices y paneles deterministas ---
                        std::memset(objStore.data(),0,objStore.size()); std::memset(vehicle.data(),0,vehicle.size());
                        for(auto& c:colDatas)std::memset(c.data(),0,c.size());
                        auto mkMatrix=[&](int k,const CVector& pos)->uint8*{ auto* m=mats[k].data(); fill(m,0x40); for(int c=0;c<3;++c)for(int j=0;j<3;++j)At<float>(m,c*16+j*4)=uni(-1.f,1.f);
                            At<CVector>(m,0x30)=pos; At<uint32>(m,0x40)=0; At<uint32>(m,0x44)=0; return m; };
                        for(int k=0;k<NUM_MI;++k){ auto* mi=mis[k].data(); std::memset(mi,0,mis[k].size()); At<void*>(mi,0)=k<3 ? atomicVptr : otherVptr;
                            uint16 special=uint16(k==0 ? 4u : k==1 ? 5u : k==3 ? 4u+next()%2 : next()%16); if(k==2&&(special==4||special==5))special=0;
                            At<uint16>(mi,0x12)=uint16((special<<11)|(next()&0x87FF)); At<void*>(mi,0x14)=colModels[k&1].data(); miSlots[FAKE_MI+k]=mi; }
                        auto mkCol=[&](int c,int numTris,int glassPct){ auto* cm=colModels[c].data(); auto* cd=colDatas[c].data(); fill(cm,colModels[c].size()); fill(cd,colDatas[c].size());
                            At<void*>(cm,0x2C)=next()%12 ? static_cast<void*>(cd) : nullptr;
                            At<uint16>(cd,4)=uint16(numTris); At<void*>(cd,0x14)=colVerts[c].data(); At<void*>(cd,0x18)=colTris[c].data();
                            At<void*>(cd,0x1C)=next()%4 ? static_cast<void*>(colPlanes[c].data()) : nullptr; planesAfterCalc=next()%5 ? static_cast<void*>(colPlanes2[c].data()) : nullptr;
                            auto* vv=reinterpret_cast<int16*>(colVerts[c].data()); for(int k=0;k<48;++k)vv[k]=int16(int32(next()%1024)-512);
                            if(next()%6==0)vv[3*(next()%4)+2]=vv[2]; // empates de Z
                            for(int k=0;k<8;++k){ auto* t=colTris[c].data()+k*8; for(int j=0;j<3;++j)At<uint16>(t,j*2)=uint16(next()%16);
                                t[6]=int(next()%100)<glassPct ? glassSurfaces[next()%glassSurfaces.size()] : otherSurfaces[next()%otherSurfaces.size()]; t[7]=uint8(next()); }
                            for(auto* pl:{colPlanes[c].data(),colPlanes2[c].data()})for(int k=0;k<8;++k){ for(int j=0;j<3;++j)At<int16>(pl,k*10+j*2)=int16(int32(next()%8192)-4096);
                                At<int16>(pl,k*10+6)=int16(next()); pl[k*10+8]=uint8(next()%6); pl[k*10+9]=uint8(next()); } };
                        auto mkObject=[&](int k,const CVector& pos,bool needMatrix)->uint8*{ auto* o=Obj(k); std::memset(o,0,OBJ_SIZE);
                            const bool hasMat=needMatrix||next()%4; At<CVector>(o,4)=pos; At<void*>(o,0x14)=hasMat ? static_cast<void*>(mkMatrix(k,pos)) : nullptr;
                            At<uint32>(o,0x1C)=(next()&0xFFFFFFFEu)|uint32(next()%5!=0);
                            At<int16>(o,0x22)=int16(FAKE_MI+next()%NUM_MI); At<uint16>(o,0x2C)=uint16(next());
                            const uint32 tsel=next()%8; o[0x36]=uint8((next()&0xF8)|(tsel<6 ? 4 : tsel==6 ? 2 : 5));
                            At<CVector>(o,0x44)=randVec(-1.f,1.f); At<uint32>(o,0x140)=(next()&0xFFFFFFCFu)|((next()%3)<<4); return o; };
                        for(int k=0;k<45;++k){ auto* p=panes+k*0x70; fill(p,0x70); for(size_t o=0;o<0x6C;o+=4)At<float>(p,o)=uni(-10.f,10.f); p[0x6C]=uint8(next()%5); p[0x6D]=uint8(next()%3==0); p[0x6E]=uint8(next()%2); p[0x6F]=uint8(next()%2); }
                        if(next()%10==0)for(int k=0;k<45;++k)panes[k*0x70+0x6D]=1; else if(next()%6==0)for(int k=0;k<45;++k)panes[k*0x70+0x6D]=uint8(k<44);
                        for(int k=0;k<5;++k){ At<float>(reinterpret_cast<void*>(0xC71B30),k*8)=uni(0.f,1.f); At<float>(reinterpret_cast<void*>(0xC71B30),k*8+4)=uni(0.f,1.f); }
                        timeMs=next()%6==0 ? 0x80000000u+next()%100000 : next()%3000000; groundHit=next()%3!=0; groundZ=uni(-20.f,60.f);
                        sphereMask=next()%4==0 ? next() : (next()%3 ? 0xFFFFFFFFu : 0u);
                        bool panesOut=false;

                        if(nm=="Init"){ if(next()%2)for(int k=0;k<30;++k)At<float>(reinterpret_cast<void*>(0x8D5CD8),k*4)=uni(-1.f,2.f);
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["centers"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC71B30),40); panesOut=true; }
                        else if(nm=="HasGlassBeenShatteredAtCoors"){ sectorsTouched=true;
                            std::memset(reinterpret_cast<void*>(0xB7D0B8),0,120*120*8); std::memset(reinterpret_cast<void*>(0xB992B8),0,16*16*12);
                            const CVector p{uni(-3100.f,3100.f),uni(-3100.f,3100.f),uni(0.f,100.f)};
                            scanCode=next()%8==0 ? uint16(0xFFFF) : uint16(next()%0xFFF0); const uint16 nextCode=scanCode==0xFFFF ? uint16(1) : uint16(scanCode+1);
                            auto sec=[](float c){ return int32(std::floor(c*0.02f+60.f)); };
                            const int32 x0=std::max(sec(p.x-30.f),0), x1=std::min(sec(p.x+30.f),119), y0=std::max(sec(p.y-30.f),0), y1=std::min(sec(p.y+30.f),119);
                            mkCol(0,2,50); mkCol(1,2,50); int nn=0;
                            for(int k=0;k<NUM_OBJ;++k){ auto* o=mkObject(k,CVector{p.x+uni(-35.f,35.f),p.y+uni(-35.f,35.f),p.z+uni(-10.f,10.f)},false);
                                if(next()%4==0)At<uint16>(o,0x2C)=nextCode;
                                const int lists=int(1+next()%2);
                                for(int l=0;l<lists&&nn<NUM_NODES;++l){ int32 sx=x1>=x0 ? x0+int32(next()%uint32(x1-x0+1)) : int32(next()%120);
                                    int32 sy=y1>=y0 ? y0+int32(next()%uint32(y1-y0+1)) : int32(next()%120);
                                    if(next()%10==0){ sx=std::clamp(x1+1,0,119); } /* a veces fuera del rango recorrido */
                                    void** head=next()%2 ? reinterpret_cast<void**>(0xB992B8+((sy&15)*16+(sx&15))*12+8) : reinterpret_cast<void**>(0xB7D0B8+(sy*120+sx)*8+4);
                                    auto* nd=nodes[nn++].data(); At<void*>(nd,0)=o; At<void*>(nd,4)=*head; At<void*>(nd,8)=nullptr; *head=nd; } }
                            ret=int(reinterpret_cast<uint8(__cdecl*)(CVector)>(a)(p));
                            out["scan"]=scanCode; json sc=json::array(); for(int k=0;k<NUM_OBJ;++k)sc.push_back(At<uint16>(Obj(k),0x2C)); out["obj_scan"]=sc; }
                        else if(nm=="FindWindowSectorList<CPtrListSingleLink<CPhysical*>>"){ const CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                            scanCode=uint16(1+next()%0xFFF0); mkCol(0,2,50); mkCol(1,2,50); At<void*>(listHead.data(),0)=nullptr; const int n=int(next()%7);
                            for(int k=0;k<n;++k){ auto* o=mkObject(k,CVector{p.x+uni(-30.f,30.f),p.y+uni(-30.f,30.f),p.z+uni(-10.f,10.f)},false); if(next()%4==0)At<uint16>(o,0x2C)=scanCode;
                                auto* nd=nodes[k].data(); At<void*>(nd,0)=o; At<void*>(nd,4)=At<void*>(listHead.data(),0); At<void*>(nd,8)=nullptr; At<void*>(listHead.data(),0)=nd; }
                            float dist=next()%4==0 ? 1e30f : uni(0.f,40.f); CEntity* ent=next()%3==0 ? reinterpret_cast<CEntity*>(Obj(5)) : nullptr;
                            reinterpret_cast<void(__cdecl*)(void*,float*,CEntity**,CVector)>(a)(listHead.data(),&dist,&ent,p);
                            out["dist"]=F(dist); out["entity"]=Id(ent); json sc=json::array(); for(int k=0;k<NUM_OBJ;++k)sc.push_back(At<uint16>(Obj(k),0x2C)); out["obj_scan"]=sc; }
                        else if(nm=="CarWindscreenShatters"){ std::memset(vehicle.data(),0,vehicle.size()); auto* v=vehicle.data();
                            const CVector vp{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                            At<CVector>(v,4)=vp; At<void*>(v,0x14)=mkMatrix(7,vp); v[0x36]=2; At<int8>(v,0x48B)=-1; At<int16>(v,0x22)=int16(FAKE_MI+(next()%2)*2);
                            At<CVector>(v,0x44)=randVec(-2.f,2.f); mkCol(0,next()%8==0 ? int(next()%2) : int(2+next()%7),next()%4 ? 35 : 90);
                            reinterpret_cast<void(__cdecl*)(void*)>(a)(v); panesOut=true; }
                        else if(nm=="GeneratePanesForWindow"){ auto lenVec=[&](float lo,float hi){ CVector d=randVec(-1.f,1.f); d.x+=0.01f; const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); const float l=uni(lo,hi); return CVector{d.x/m*l,d.y/m*l,d.z/m*l}; };
                            const uint32 type=next()%8==0 ? 3 : next()%3; const CVector p=randVec(-3000.f,3000.f);
                            const CVector fw=lenVec(0.05f,next()%3 ? 2.5f : 5.f), rt=lenVec(0.05f,next()%3 ? 2.5f : 5.f);
                            const float coeff=next()%4==0 ? 0.f : uni(-0.5f,0.5f); const int32 ns=next()%8==0 ? int32(next()%3)-1 : int32(1+next()%3);
                            reinterpret_cast<void(__cdecl*)(uint32,CVector,CVector,CVector,CVector,CVector,float,bool,bool,int32,bool)>(a)(
                                type,p,fw,rt,randVec(-1.f,1.f),p+randVec(-3.f,3.f),coeff,next()%2!=0,next()%3==0,ns,next()%2!=0); panesOut=true; }
                        else if(nm=="Update"){ reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Render"||nm=="RenderHiLightPolys"||nm=="RenderShatteredPolys"||nm=="RenderReflectionPolys"){
                            for(auto& t:glassTex){ fill(t.data(),t.size()); At<uintptr_t>(t.data(),0)=0x10000000+(next()%0x1000)*4; }
                            At<void*>(reinterpret_cast<void*>(0xC403F4),0)=glassTex[0].data(); At<void*>(reinterpret_cast<void*>(0xC403F8),0)=glassTex[1].data(); At<void*>(reinterpret_cast<void*>(0xC40408),0)=glassTex[2].data();
                            im3dOk=next()%5!=0;
                            hiV=next()%3==0 ? 0u : next()%1024; hiI=int32(next()%2048); shatV=next()%3==0 ? 1024 : int32(1024+next()%512); shatI=int32(2048+next()%1024);
                            reflV=next()%3==0 ? 1536 : int32(1536+next()%512); reflI=int32(3072+next()%1024);
                            for(int k=0;k<3;++k)sky[k]=next()%6==0 ? uint16(next()) : uint16(next()%256);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["counters"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC71B18),0x18); }
                        else if(nm=="AskForObjectToBeRenderedInGlass"){ fill(reinterpret_cast<void*>(0xC71B58),0x80); numGlassEnt=next()%4==0 ? next() : next()%40;
                            reinterpret_cast<void(__cdecl*)(void*)>(a)(Obj(next()%NUM_OBJ));
                            out["count"]=numGlassEnt; out["entities"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC71B58),0x80); }
                        else if(nm=="FindFreePane"){ ret=Id(reinterpret_cast<void*(__cdecl*)()>(a)()); }
                        else if(nm=="IsObjectGlass"){ mkCol(0,2,50); mkCol(1,2,50); auto* o=mkObject(0,randVec(-100.f,100.f),false); ret=int(reinterpret_cast<uint8(__cdecl*)(void*)>(a)(o)); }
                        else if(nm=="WindowRespondsToSoftCollision"){ mkCol(0,2,50); mkCol(1,2,50); auto* o=mkObject(0,randVec(-3000.f,3000.f),false);
                            const float in=next()%6==0 ? 50.f : uni(0.f,120.f); reinterpret_cast<void(__cdecl*)(void*,float)>(a)(o,in); }
                        else if(nm=="WasGlassHitByBullet"||nm=="WindowRespondsToCollision"||nm=="WindowRespondsToExplosion"){
                            mkCol(0,next()%5==0 ? int(next()%4) : 2,50); mkCol(1,next()%5==0 ? int(next()%4) : 2,50);
                            const CVector op=randVec(-3000.f,3000.f); auto* o=mkObject(0,op,true);
                            if(nm=="WasGlassHitByBullet")reinterpret_cast<void(__cdecl*)(void*,CVector)>(a)(o,op+randVec(-2.f,2.f));
                            else if(nm=="WindowRespondsToCollision"){ const float in=next()%6==0 ? 300.f : uni(0.f,600.f);
                                reinterpret_cast<void(__cdecl*)(void*,float,CVector,CVector,bool)>(a)(o,in,randVec(-1.f,1.f),op+randVec(-2.f,2.f),next()%2!=0); }
                            else { CVector d=randVec(-1.f,1.f); d.x+=0.01f; const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); const float r=next()%8==0 ? (next()%2 ? 10.f : 30.f) : uni(0.5f,40.f);
                                reinterpret_cast<void(__cdecl*)(void*,CVector)>(a)(o,CVector{op.x+d.x/m*r,op.y+d.y/m*r,op.z+d.z/m*r}); }
                            panesOut=true; }
                        else if(nm=="BreakGlassPhysically"){ mkCol(0,next()%6==0 ? int(next()%2) : int(2+next()%5),50); mkCol(1,next()%6==0 ? int(next()%2) : 2,50);
                            const CVector p=randVec(-3000.f,3000.f);
                            for(int k=0;k<NUM_OBJ;++k){ mkObject(k,p+randVec(-3.f,3.f),true); objStates[k]=uint8(next()%4==0 ? 0x80|(next()&0x7F) : next()&0x7F); }
                            At<void*>(fakePool.data(),0)=objStore.data(); At<void*>(fakePool.data(),4)=objStates.data(); At<int32>(fakePool.data(),8)=NUM_OBJ; At<int32>(fakePool.data(),0xC)=-1; At<uint32>(fakePool.data(),0x10)=0;
                            objPool=fakePool.data();
                            lastCol=next()%3==0 ? timeMs-uint32(next()%1000) : next()%3==0 ? timeMs+uint32(next()%5000) : timeMs-1000-uint32(next()%100000);
                            reinterpret_cast<void(__cdecl*)(CVector,float)>(a)(p,uni(0.1f,3.f)); out["last_col"]=lastCol; panesOut=true; }
                        if(panesOut)out["panes"]=ReversingProbe::Bytes(panes,45*0x70);
                        out["objs"]=ReversingProbe::Bytes(objStore.data(),objStore.size()); out["vehicle"]=Fnv(vehicle.data(),vehicle.size());
                        out["col_data"]=json::array({ReversingProbe::Bytes(colDatas[0].data(),0x20),ReversingProbe::Bytes(colDatas[1].data(),0x20)});
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
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
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        *dev=savedDev;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    for(auto& s:savedSectors)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
