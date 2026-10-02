#pragma once
// Banco dentro del motor: CMirrors (8 funciones). Original (hooks del lote desactivados) frente a candidato
// con el estado de los espejos aleatorizado por caso. Se simulan y trazan: rasters (crear/destruir), calidad
// de efectos, estados de render, Im2D (puntero de dispositivo) e Im3D, DefinedState, la camara de RW (falsa),
// la posicion de TheCamera (matriz falsa), FindPlayerVehicle, las zonas de espejo, IsSphereVisible,
// DealWithMirrorBeforeConstructRenderList, SetCameraUpForMirror/RestoreCameraAfterMirror, RwCameraClear,
// RsCameraBeginUpdate/RwCameraEndUpdate, GraphicsHighQuality, RenderScene y RenderWeaponPedsForPC.
// Se comparan el estado global de los espejos, los buferes temporales del EXE, las matrices y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Mirrors.h"
#include "CullZones.h"
#include "VisibilityPlugins.h"
#include "Fx/Fx.h"
#include "app/app.h"
#include "app/platform/platform.h"
#include "../ProbeRuntime.hpp"

namespace MirrorsLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 rasterMask{}, rasterCalls{}; inline int32 fxQuality{}; inline bool transformOk{}, sphereMask{}, beginOk{}, highQuality{};
inline uint32 sphereBits{}, sphereCalls{}; inline void* vehicleResult{}; inline void* zoneResult{};
alignas(16) inline std::array<uint8,0x40> fakeRasters[4]{};
inline json Raster(const void* p){ for(int k=0;k<4;++k)if(p==fakeRasters[k].data())return "R"+std::to_string(k); return p ? json(uintptr_t(p)) : json(nullptr); }
inline RwRaster* __cdecl FakeRasterCreate(RwInt32 w,RwInt32 h,RwInt32 d,RwInt32 f){ const bool ok=(rasterMask>>(rasterCalls%32))&1; const uint32 n=rasterCalls++; trace.push_back({"raster_create",w,h,d,f,ok});
    return ok ? reinterpret_cast<RwRaster*>(fakeRasters[n%4].data()) : nullptr; }
inline RwBool __cdecl FakeRasterDestroy(RwRaster* r){ trace.push_back({"raster_destroy",Raster(r)}); return TRUE; }
inline FxQuality_e __fastcall FakeFxQuality(const void*,void*){ trace.push_back({"fx_quality"}); return FxQuality_e(fxQuality); }
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),st==rwRENDERSTATETEXTURERASTER ? Raster(v) : json(uintptr_t(v))}); return TRUE; }
inline void __cdecl FakeDefinedState(){ trace.push_back({"defined_state"}); }
inline json Vtx3(const RxObjSpace3DVertex* v,int n){ json r=json::array(); for(int k=0;k<n;++k){ const auto* w=reinterpret_cast<const uint32*>(&v[k]); json e=json::array(); for(int j=0;j<9;++j)e.push_back(w[j]); r.push_back(e);} return r; }
inline RwBool __cdecl FakeIm2DRender(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 nv,RwImVertexIndex* idx,RwInt32 ni){
    json vs=json::array(); for(int k=0;k<nv;++k){ const auto* w=reinterpret_cast<const uint32*>(&v[k]); json e=json::array(); for(int j=0;j<7;++j)e.push_back(w[j]); vs.push_back(e);} json is=json::array(); for(int k=0;k<ni;++k)is.push_back(idx[k]);
    trace.push_back({"im2d_render_indexed",int(t),vs,is}); return TRUE; }
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ trace.push_back({"im3d_transform",Vtx3(static_cast<RxObjSpace3DVertex*>(v),int(n)),uintptr_t(m),f,intptr_t(v)-0xC4D958}); return transformOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ json is=json::array(); for(int k=0;k<n;++k)is.push_back(idx[k]); trace.push_back({"im3d_render_indexed",int(t),is,intptr_t(idx)-0xC4B958}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(vehicleResult); }
inline CMirrorAttributeZone* __cdecl FakeFindMirror(CVector p){ trace.push_back({"find_mirror_attributes",F(p.x),F(p.y),F(p.z)}); return static_cast<CMirrorAttributeZone*>(zoneResult); }
inline bool __fastcall FakeSphereVisible(void*,void*,const CVector& c,float r,RwMatrix* m){ const bool v=(sphereBits>>(sphereCalls++%32))&1; trace.push_back({"is_sphere_visible",F(c.x),F(c.y),F(c.z),F(r),uintptr_t(m),*reinterpret_cast<uint8*>(0xB6F998)}); return v; }
inline void __fastcall FakeDealWithMirror(void*,void*,bool active,CVector n,float v,CMatrix* m){
    json mm=nullptr; if(m){ mm=json::array(); const auto* w=reinterpret_cast<const uint32*>(m); for(int k:{0,1,2,4,5,6,8,9,10,12,13,14})mm.push_back(w[k]); }
    trace.push_back({"deal_with_mirror",active,F(n.x),F(n.y),F(n.z),F(v),mm}); }
inline void __fastcall FakeCamUp(void*,void*){ trace.push_back({"set_camera_up_for_mirror"}); }
inline void __fastcall FakeCamRestore(void*,void*){ trace.push_back({"restore_camera_after_mirror"}); }
inline RwCamera* __cdecl FakeCameraClear(RwCamera* c,RwRGBA* col,RwInt32 mode){ trace.push_back({"camera_clear",col->red,col->green,col->blue,col->alpha,mode}); return c; }
inline bool __cdecl FakeHighQuality(){ trace.push_back({"graphics_high_quality"}); return highQuality; }
inline bool __cdecl FakeBeginUpdate(RwCamera*){ trace.push_back({"camera_begin_update",*reinterpret_cast<uint8*>(0xC7C728)}); return beginOk; }
inline RwCamera* __cdecl FakeEndUpdate(RwCamera* c){ trace.push_back({"camera_end_update",*reinterpret_cast<uint8*>(0xC7C728)}); return c; }
inline void __cdecl FakeRenderScene(){ trace.push_back({"render_scene",*reinterpret_cast<uint8*>(0xC7C728)}); }
inline void __cdecl FakeRenderWeaponPeds(){ trace.push_back({"render_weapon_peds"}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,8> fns{{
    {"CMirrors","Init",0x723000,0,-1,64},{"CMirrors","ShutDown",0x723050,0,-1,64},{"CMirrors","CreateBuffer",0x7230A0,0,-1,512},
    {"CMirrors","BuildCamMatrix",0x723150,0,-1,1024},{"CMirrors","RenderMirrorBuffer",0x726090,0,-1,1024},{"CMirrors","BuildCameraMatrixForScreens",0x7266B0,0,-1,1024},
    {"CMirrors","BeforeConstructRenderList",0x726DF0,0,-1,2048},{"CMirrors","BeforeMainRender",0x727140,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"mirrors-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"mirrors-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("mirrors");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"mirrors-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,8> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,8> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado: espejos (0xC7C5A8..0xC7C72C), normal (0xC803D8, 12), buferes temporales, reloj, camara
    auto* mst=reinterpret_cast<uint8*>(0xC7C5A8); constexpr size_t NM=0xC7C72C-0xC7C5A8; const std::vector<uint8> savedM(mst,mst+NM);
    auto* nrm=reinterpret_cast<uint8*>(0xC803D8); const std::vector<uint8> savedN(nrm,nrm+12);
    auto* tbi=reinterpret_cast<uint8*>(0xC4B950); constexpr size_t NTI=8+12; const std::vector<uint8> savedTI(tbi,tbi+NTI);
    auto* tbv=reinterpret_cast<uint8*>(0xC4D958); constexpr size_t NTV=0x24*4; const std::vector<uint8> savedTV(tbv,tbv+NTV);
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& mirrorActiveCam=*reinterpret_cast<uint8*>(0xB6F998); const uint8 savedMirrorActive=mirrorActiveCam;
    auto* wpl=reinterpret_cast<uint8*>(0xC88224); const std::vector<uint8> savedWpl(wpl,wpl+0x30);
    auto& camMatrix=*reinterpret_cast<CMatrix**>(0xB6F03C); CMatrix* const savedCamMatrix=camMatrix;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch e1(0x7FB230,FakeRasterCreate), e2(0x7FB020,FakeRasterDestroy);
        EntryPatch e3(bit_cast<uintptr_t>(&Fx_c::GetFxQuality),FakeFxQuality), e3e(0x49EA50,FakeFxQuality);
        EntryPatch e4(bit_cast<uintptr_t>(&DefinedState),FakeDefinedState), e4e(0x734650,FakeDefinedState);
        EntryPatch e5(0x7EF450,FakeIm3DTransform), e6(0x7EF550,FakeIm3DRender), e7(0x7EF520,FakeIm3DEnd);
        EntryPatch e8(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), e8e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch e9(bit_cast<uintptr_t>(&CCullZones::FindMirrorAttributesForCoors),FakeFindMirror), e9e(0x72DA70,FakeFindMirror);
        EntryPatch e10(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)),FakeSphereVisible), e10e(0x420C40,FakeSphereVisible);
        EntryPatch e11(bit_cast<uintptr_t>(&CCamera::DealWithMirrorBeforeConstructRenderList),FakeDealWithMirror), e11e(0x50B510,FakeDealWithMirror);
        EntryPatch e12(bit_cast<uintptr_t>(&CCamera::SetCameraUpForMirror),FakeCamUp), e12e(0x51A560,FakeCamUp);
        EntryPatch e13(bit_cast<uintptr_t>(&CCamera::RestoreCameraAfterMirror),FakeCamRestore), e13e(0x51A5A0,FakeCamRestore);
        EntryPatch e14(0x7EE340,FakeCameraClear), e15(0x7EE180,FakeEndUpdate);
        EntryPatch e14d(bit_cast<uintptr_t>(&RwCameraClear),FakeCameraClear), e15d(bit_cast<uintptr_t>(&RwCameraEndUpdate),FakeEndUpdate); // la DLL tiene implementaciones propias
        EntryPatch e16(bit_cast<uintptr_t>(static_cast<bool(*)()>(&GraphicsHighQuality)),FakeHighQuality), e16e(0x70F9B0,FakeHighQuality);
        EntryPatch e17(bit_cast<uintptr_t>(&RsCameraBeginUpdate),FakeBeginUpdate), e17e(0x619450,FakeBeginUpdate);
        EntryPatch e18(bit_cast<uintptr_t>(&RenderScene),FakeRenderScene), e18e(0x53DF40,FakeRenderScene);
        EntryPatch e19(bit_cast<uintptr_t>(&CVisibilityPlugins::RenderWeaponPedsForPC),FakeRenderWeaponPeds), e19e(0x732F30,FakeRenderWeaponPeds);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;
        auto& fpIm2D=RWSRCGLOBAL(dOpenDevice).fpIm2DRenderIndexedPrimitive; const auto savedIm2D=fpIm2D; fpIm2D=FakeIm2DRender;
        // camara de RW falsa (raster +0x60, z-raster +0x64, plano cercano +0x80)
        alignas(16) static std::array<uint8,0x100> fakeCamera{}; auto& sceneCamera=*reinterpret_cast<void**>(0xC1703C); void* const savedCamera=sceneCamera;
        alignas(16) static CMatrix camMat;

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3141u; if(!stt)stt=1; trace=json::array();
                    rasterCalls=0; rasterMask=next(); if(next()%3==0)rasterMask=~0u; fxQuality=int32(next()%5); transformOk=next()%4!=0; sphereBits=next(); sphereCalls=0; beginOk=next()%4!=0; highQuality=next()%2;
                    for(auto& r:fakeRasters)for(auto& b:r)b=0; for(int k=0;k<4;++k){ *reinterpret_cast<int32*>(fakeRasters[k].data()+0xC)=k%2 ? 640 : 1024; *reinterpret_cast<int32*>(fakeRasters[k].data()+0x10)=k%2 ? 480 : 768; *reinterpret_cast<int32*>(fakeRasters[k].data()+0x14)=32; }
                    // estado de espejos aleatorio
                    for(size_t k=0;k<NM;++k)mst[k]=uint8(next()); for(auto& b:fakeCamera)b=0;
                    *reinterpret_cast<void**>(fakeCamera.data()+0x60)=fakeRasters[next()%2].data(); *reinterpret_cast<void**>(fakeCamera.data()+0x64)=fakeRasters[2+next()%2].data(); *reinterpret_cast<float*>(fakeCamera.data()+0x80)=uni(0.05f,2.f);
                    sceneCamera=fakeCamera.data();
                    CMirrors::pBuffer=next()%3 ? reinterpret_cast<RwRaster*>(fakeRasters[next()%4].data()) : nullptr; CMirrors::pZBuffer=next()%3 ? reinterpret_cast<RwRaster*>(fakeRasters[next()%4].data()) : nullptr;
                    CMirrors::TypeOfMirror=eMirrorType(next()%3); if(next()%5==0)*reinterpret_cast<uint8*>(0xC7C725)=uint8(next()); // bytes altos: el original lee la palabra CMirrors::MirrorFlags=next()%4==0 ? int32(next()) : int32(next()%4); *reinterpret_cast<uint8*>(0xC7C72A)=next()%5==0; // bFudgeNow
                    CMirrors::d3dRestored=next()%4==0; CMirrors::bRenderingReflection=false; CMirrors::MirrorV=uni(-100.f,100.f); CMirrors::MirrorNormal=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    for(size_t k=0;k<NTI;++k)tbi[k]=uint8(next()); for(size_t k=0;k<NTV;++k)tbv[k]=uint8(next());
                    timeMs=next()%3 ? next()%100000 : next();
                    // posicion de la camara: cerca o lejos del punto (216, -42, 1003)
                    camMat.SetUnity(); camMat.GetPosition()=next()%2 ? CVector{216.f+uni(-40.f,40.f),-42.f+uni(-30.f,30.f),1003.f+uni(-20.f,20.f)} : CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                    if(next()%6==0)camMat.GetPosition()=CVector{216.f+50.f,-42.f,1003.f};
                    camMatrix=&camMat; mirrorActiveCam=uint8(next()%2);
                    // vehiculo y zona de espejo
                    alignas(16) static std::array<uint8,0x800> veh{}; for(auto& b:veh)b=0; *reinterpret_cast<int32*>(veh.data()+0x594)=int32(next()%6);
                    vehicleResult=next()%3==0 ? veh.data() : nullptr;
                    alignas(16) static std::array<uint8,0x18> zone{}; for(auto& b:zone)b=uint8(next()); *reinterpret_cast<float*>(zone.data()+0x10)=uni(-1500.f,1500.f);
                    for(int k=0;k<3;++k)zone[0x14+k]=uint8(int8(int(next()%201)-100)); if(next()%2){ zone[0x14]=0; zone[0x15]=0; zone[0x16]=uint8(int8(next()%2 ? 100 : -100)); }
                    zone[0x17]=uint8(next()%4==0 ? next() : next()%4);
                    zoneResult=next()%4 ? zone.data() : nullptr;

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json out={{"switches",switches}};
                    if(nm=="BuildCamMatrix"){ alignas(16) CMatrix m; std::memset(&m,0xCD,sizeof(m)); m.m_pAttachMatrix=nullptr; m.m_bOwnsAttachedMatrix=false;
                        CVector a{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,1100.f)}; CVector b=next()%8==0 ? a+CVector{0.f,0.f,uni(-5.f,5.f)} : a+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-30.f,30.f)};
                        reinterpret_cast<void(__cdecl*)(CMatrix&,CVector,CVector)>(fn.address)(m,a,b); out["mat"]=ReversingProbe::Bytes(&m,0x40); }
                    else if(nm=="BuildCameraMatrixForScreens"){ alignas(16) CMatrix m; std::memset(&m,0xCD,sizeof(m)); m.m_pAttachMatrix=nullptr; m.m_bOwnsAttachedMatrix=false;
                        reinterpret_cast<void(__cdecl*)(CMatrix&)>(fn.address)(m); out["mat"]=ReversingProbe::Bytes(&m,0x40); }
                    else reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    json st=json::array({Raster(CMirrors::pBuffer),Raster(CMirrors::pZBuffer),int32(CMirrors::TypeOfMirror),CMirrors::MirrorFlags,CMirrors::d3dRestored,CMirrors::bRenderingReflection,F(CMirrors::MirrorV)});
                    out["state"]=st; out["normal"]=ReversingProbe::Bytes(nrm,12); out["im2d"]=ReversingProbe::Bytes(mst,0x70);
                    out["temp_idx"]=ReversingProbe::Bytes(tbi,NTI); out["temp_vtx"]=ReversingProbe::Bytes(tbv,NTV); out["cam_mirror_active"]=mirrorActiveCam;
                    out["camera"]=json::array({Raster(*reinterpret_cast<void**>(fakeCamera.data()+0x60)),Raster(*reinterpret_cast<void**>(fakeCamera.data()+0x64))});
                    out["weapon_peds"]=ReversingProbe::Bytes(wpl,0x30); out["trace"]=trace;
                    camMatrix=savedCamMatrix; sceneCamera=savedCamera;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["state"]},{"candidate",candidate["state"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        fpSet=savedSet; fpIm2D=savedIm2D; sceneCamera=savedCamera; camMatrix=savedCamMatrix;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    std::memcpy(mst,savedM.data(),NM); std::memcpy(nrm,savedN.data(),12); std::memcpy(tbi,savedTI.data(),NTI); std::memcpy(tbv,savedTV.data(),NTV);
    std::memcpy(wpl,savedWpl.data(),0x30); timeMs=savedTime; mirrorActiveCam=savedMirrorActive;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",std::memcmp(mst,savedM.data(),NM)==0&&camMatrix==savedCamMatrix}});
}
}
