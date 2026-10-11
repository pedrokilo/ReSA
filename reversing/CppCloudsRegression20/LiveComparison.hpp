#pragma once
// Banco dentro del motor: CClouds (lista de 21; 20 funciones). Original (hooks de CClouds desactivados) frente a
// candidato con fixtures deterministas. Nunca se toca el dispositivo RW real ni texturas/rasters: se simulan y trazan
// rand, estados de render (fpRenderStateSet), Im3D (direcciones del EXE; las de la DLL son envoltorios), lectura y
// destruccion de texturas, CTxdStore, los sprites (CalcScreenCoors, RenderOneXLUSprite, RenderBufferedOneXLUSprite,
// _Rotate_Dimension, Init/FlushSpriteBuffer), CPostEffects::ImmediateModeRenderStates*, CCullZones::CamNoRain/
// PlayerNoRain, CGame::CanSeeOutSideFromCurrArea, CBrightLights::RenderOutGeometryBuffer y FindPlayerPed/Vehicle/
// Coors (peaton y vehiculo falsos). La camara (matriz falsa o nula), Scene.m_pRwCamera, las texturas de nubes/coronas
// (punteros a texturas falsas) y todo el estado global tocado (tablas de CClouds, bufer temporal, tiempo, clima,
// colores, reloj) se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Clouds.h"
#include "Sprite.h"
#include "PostEffects.h"
#include "CullZones.h"
#include "BrightLights.h"
#include "TxdStore.h"
#include "Coronas.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace CppCloudsRegression20Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t TEMP_VERTS=0xC4D958, TEMP_IDX=0xC4B958, TEMP_NV=0xC4B950, TEMP_NI=0xC4B954;
constexpr uintptr_t CL_BEGIN=0xC6AA58, CL_END=0xC6E978;     // globales de CClouds: distancia ... ms_vc, ms_mf ... guardas estaticas
constexpr uintptr_t VC=0xC6AAB0, MF=0xC6C158;
constexpr size_t VC_SIZE=0x16A8, MF_SIZE=0x27FC;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x48> camMat{}, vehMat{};
alignas(16) inline std::array<std::array<uint8,0x60>,8> textures{};
alignas(16) inline std::array<uint8,0x800> fakePed{}, fakeVeh{};
alignas(16) inline std::array<uint8,0x100> fakeRwCam{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }

// --- render ---
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ const size_t k=std::min<RwUInt32>(n,2048);
    trace.push_back({"im3d_transform",intptr_t(v)-intptr_t(TEMP_VERTS),n,uintptr_t(m),f,Fnv(v,k*0x24)}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRenderIndexed(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ const size_t k=n<0 ? 0 : size_t(std::min<RwInt32>(n,4096));
    trace.push_back({"im3d_render_indexed",int(t),uintptr_t(idx),n,Fnv(idx,k*2)}); return TRUE; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t){ trace.push_back({"im3d_render",int(t)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline RwTexture* __cdecl FakeTextureRead(const char* n,const char* m){ trace.push_back({"texture_read",std::string(n),m ? json(std::string(m)) : json(nullptr)});
    return reinterpret_cast<RwTexture*>(textures[Fnv(n,strlen(n))%8].data()); }
inline RwBool __cdecl FakeTextureDestroy(RwTexture* t){ trace.push_back({"texture_destroy",Id(t)}); return TRUE; }
inline void __cdecl FakePushTxd(){ trace.push_back({"push_current_txd"}); }
inline void __cdecl FakePopTxd(){ trace.push_back({"pop_current_txd"}); }
inline void __cdecl FakeSetTxd(int32 i){ trace.push_back({"set_current_txd",i}); }
inline int32 findSlotResult{};
inline int32 __cdecl FakeFindTxdSlot(const char* n){ trace.push_back({"find_txd_slot",std::string(n)}); return findSlotResult; }

// --- sprites ---
inline uint32 calcMask{}, calcCalls{};
inline bool __cdecl FakeCalcScreenCoors(const RwV3d* p,RwV3d* out,float* w,float* h,bool maxV,bool minV){
    trace.push_back({"calc_screen_coors",F(p->x),F(p->y),F(p->z),maxV,minV});
    const bool ok=(calcMask>>(calcCalls++%32))&1;
    out->x=p->x*0.25f+320.f; out->y=p->y*0.125f+240.f; out->z=std::fabs(p->z)*0.5f+1.5f; *w=std::fabs(p->x)*0.01f+0.5f; *h=std::fabs(p->y)*0.01f+0.25f;
    return ok; }
inline void __cdecl FakeRenderOne(float x,float y,float z,float w,float h,uint8 r,uint8 g,uint8 b,int16 in,float rhw,uint8 a,uint8 u,uint8 v){
    trace.push_back({"render_one_xlu",F(x),F(y),F(z),F(w),F(h),r,g,b,in,F(rhw),a,u,v}); }
inline void __cdecl FakeRenderBuffered(float x,float y,float z,float w,float h,uint8 r,uint8 g,uint8 b,int16 in,float rhw,uint8 a){
    trace.push_back({"render_buffered_xlu",F(x),F(y),F(z),F(w),F(h),r,g,b,in,F(rhw),a}); }
inline void __cdecl FakeRenderBufferedRot(float x,float y,float z,float w,float h,uint8 r,uint8 g,uint8 b,int16 in,float rhw,float rot,uint8 a){
    trace.push_back({"render_buffered_xlu_rot",F(x),F(y),F(z),F(w),F(h),r,g,b,in,F(rhw),F(rot),a}); }
inline void __cdecl FakeFlushSprites(){ trace.push_back({"flush_sprite_buffer"}); }
inline void __cdecl FakeInitSprites(){ trace.push_back({"init_sprite_buffer"}); }

// --- varios ---
inline void __cdecl FakeRSStore(){ trace.push_back({"im_states_store"}); }
inline void __cdecl FakeRSSet(){ trace.push_back({"im_states_set"}); }
inline void __cdecl FakeRSReStore(){ trace.push_back({"im_states_restore"}); }
inline uint32 noRainMask{}, noRainCalls{};
inline bool __cdecl FakeCamNoRain(){ trace.push_back({"cam_no_rain"}); return (noRainMask>>(noRainCalls++%32))&1; }
inline bool __cdecl FakePlayerNoRain(){ trace.push_back({"player_no_rain"}); return (noRainMask>>(noRainCalls++%32))&1; }
inline bool canSeeOutside{};
inline bool __cdecl FakeCanSeeOutside(){ trace.push_back({"can_see_outside"}); return canSeeOutside; }
inline void __cdecl FakeBrightLightsOut(){ trace.push_back({"bright_lights_render_out"}); }
inline void* fakeVehResult{}; inline CVector playerCoors{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(fakePed.data()); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(fakeVehResult); }
inline CVector* __cdecl FakeFindPlayerCoors(CVector* out,int32 id){ trace.push_back({"find_player_coors",id}); *out=playerCoors; return out; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"CClouds","Init",0x7138D0,0,-1,64},{"CClouds","Update",0x712FF0,0,-1,1024},{"CClouds","Shutdown",0x712FA0,0,-1,64},
    {"CClouds","SetUpOneSkyPoly",0x713060,0,-1,512},{"CClouds","Render",0x713950,0,-1,1024},{"CClouds","RenderSkyPolys",0x714650,0,-1,512},
    {"CClouds","MovingFogInit",0x713660,0,-1,64},{"CClouds","MovingFog_Create",0x713760,0,-1,1024},{"CClouds","MovingFog_Delete",0x713730,0,-1,256},
    {"CClouds","MovingFog_Update",0x716B10,0,-1,512},{"CClouds","MovingFog_GetFXIntensity",0x7136D0,0,-1,64},{"CClouds","MovingFog_GetWind",0x7136E0,0,-1,128},
    {"CClouds","MovingFog_GetFirstFreeSlot",0x713710,0,-1,256},{"CClouds","MovingFogRender",0x716C90,0,-1,512},
    {"CClouds","VolumetricCloudsInit",0x7131C0,0,-1,64},{"CClouds","VolumetricClouds_Create",0x715F40,0,-1,1024},{"CClouds","VolumetricClouds_Delete",0x7135F0,0,-1,256},
    {"CClouds","VolumetricClouds_GetFirstFreeSlot",0x7135C0,0,-1,256},{"CClouds","VolumetricCloudsGetMaxDistance",0x713630,0,-1,256},
    {"CClouds","VolumetricCloudsRender",0x716380,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cppcloudsregression20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cppcloudsregression20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cppcloudsregression20");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cppcloudsregression20-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ Reg(camMat.data(),camMat.size()); Reg(vehMat.data(),vehMat.size()); for(auto& t:textures)Reg(t.data(),t.size());
        Reg(fakePed.data(),fakePed.size()); Reg(fakeVeh.data(),fakeVeh.size()); Reg(fakeRwCam.data(),fakeRwCam.size()); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {TEMP_NV,8},{TEMP_IDX,0x2000},{TEMP_VERTS,0x24*2048},
            {CL_BEGIN,CL_END-CL_BEGIN},                    // todas las globales y tablas de CClouds
            {0x8D5388,12},{0x8D5798,4},{0x8D4B60,4},       // densidad/interruptor/viento volumetricos, CurrentFogIntensity, MoonSize
            {0xB6F02C,0x14},{0xB6F178,4},{0xB6F9AC,12},    // TheCamera: posicion, heading, m_matrix; m_fOrientation; frente de 0x984
            {0xB7C4A0,0x80},{0xB7CB10,4},                  // CTimeCycle::m_CurrentColours, m_BelowHorizonGrey
            {0xC812E0,0x50},{0xC813E0,12},                 // CWeather (Rainbow ... UnderWaterness), WindDir
            {0xB70150,6},{0xB7CB5C,4},{0xB7CB84,4},{0xB72914,4},
            {0xC3E000,0x24},{0xC3E030,1},{0xC3EF9C,4},{0xC1703C,4},{0xA9AE54,4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRenderIndexed), i3(0x7EF6B0,FakeIm3DRender), i4(0x7EF520,FakeIm3DEnd); // RwIm3D* de la DLL son envoltorios
        EntryPatch t1(0x7F3AC0,FakeTextureRead), t2(0x7F3820,FakeTextureDestroy);                                                                // idem RwTextureRead/Destroy
        EntryPatch x1(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), x1e(0x7316A0,FakePushTxd);
        EntryPatch x2(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), x2e(0x7316B0,FakePopTxd);
        EntryPatch x3(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), x3e(0x7319C0,FakeSetTxd);
        EntryPatch x4(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxdSlot), x4e(0x731850,FakeFindTxdSlot);
        EntryPatch s1(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),FakeCalcScreenCoors), s1e(0x70CE30,FakeCalcScreenCoors);
        EntryPatch s2(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite),FakeRenderOne), s2e(0x70D000,FakeRenderOne);
        EntryPatch s3(bit_cast<uintptr_t>(&CSprite::RenderBufferedOneXLUSprite),FakeRenderBuffered), s3e(0x70E4A0,FakeRenderBuffered);
        EntryPatch s4(bit_cast<uintptr_t>(&CSprite::RenderBufferedOneXLUSprite_Rotate_Dimension),FakeRenderBufferedRot), s4e(0x70EAB0,FakeRenderBufferedRot);
        EntryPatch s5(bit_cast<uintptr_t>(&CSprite::FlushSpriteBuffer),FakeFlushSprites), s5e(0x70CF20,FakeFlushSprites);
        EntryPatch s6(bit_cast<uintptr_t>(&CSprite::InitSpriteBuffer),FakeInitSprites), s6e(0x70CFB0,FakeInitSprites);
        EntryPatch p1(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesStore),FakeRSStore), p1e(0x700CC0,FakeRSStore);
        EntryPatch p2(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesSet),FakeRSSet), p2e(0x700D70,FakeRSSet);
        EntryPatch p3(bit_cast<uintptr_t>(&CPostEffects::ImmediateModeRenderStatesReStore),FakeRSReStore), p3e(0x700E00,FakeRSReStore);
        EntryPatch c1(bit_cast<uintptr_t>(&CCullZones::CamNoRain),FakeCamNoRain), c1e(0x72DDB0,FakeCamNoRain);
        EntryPatch c2(bit_cast<uintptr_t>(&CCullZones::PlayerNoRain),FakePlayerNoRain), c2e(0x72DDC0,FakePlayerNoRain);
        EntryPatch g1(bit_cast<uintptr_t>(&CGame::CanSeeOutSideFromCurrArea),FakeCanSeeOutside), g1e(0x53C4A0,FakeCanSeeOutside);
        EntryPatch b1(bit_cast<uintptr_t>(&CBrightLights::RenderOutGeometryBuffer),FakeBrightLightsOut), b1e(0x722150,FakeBrightLightsOut);
        EntryPatch f1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), f1e(0x56E210,FakeFindPlayerPed);
        EntryPatch f2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), f2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch f3(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeFindPlayerCoors), f3e(0x56E010,FakeFindPlayerCoors);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;

        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C); auto& camPos=*reinterpret_cast<CVector*>(0xB6F02C); auto& camHeading=*reinterpret_cast<float*>(0xB6F038);
        auto& camOrient=*reinterpret_cast<float*>(0xB6F178); auto& camFwd984=*reinterpret_cast<CVector*>(0xB6F9AC);
        auto* colours=reinterpret_cast<uint16*>(0xB7C4A0); auto* grey=reinterpret_cast<uint8*>(0xB7CB10);
        auto& rainbow=*reinterpret_cast<float*>(0xC812E4); auto& wind=*reinterpret_cast<float*>(0xC812F0); auto& extraSunny=*reinterpret_cast<float*>(0xC812F8);
        auto& fogSF=*reinterpret_cast<float*>(0xC812FC); auto& fog=*reinterpret_cast<float*>(0xC81300); auto& cloudCov=*reinterpret_cast<float*>(0xC81304);
        auto& newWeather=*reinterpret_cast<int16*>(0xC8131C); auto& oldWeather=*reinterpret_cast<int16*>(0xC81320); auto& underWater=*reinterpret_cast<float*>(0xC8132C);
        auto& windDir=*reinterpret_cast<CVector*>(0xC813E0);
        auto& secs=*reinterpret_cast<uint16*>(0xB70150); auto& mins=*reinterpret_cast<uint8*>(0xB70152); auto& hours=*reinterpret_cast<uint8*>(0xB70153); auto& days=*reinterpret_cast<uint8*>(0xB70154);
        auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); auto& currArea=*reinterpret_cast<int32*>(0xB72914);
        auto* coronaTex=reinterpret_cast<void**>(0xC3E000); auto& farClipZ=*reinterpret_cast<float*>(0xC3EF9C); auto& sceneCam=*reinterpret_cast<void**>(0xC1703C);
        auto& fxQuality=*reinterpret_cast<int32*>(0xA9AE54); auto& moonSize=*reinterpret_cast<uint32*>(0x8D4B60);
        auto& density=*reinterpret_cast<float*>(0x8D5388); auto& heightSwitch=*reinterpret_cast<uint8*>(0x8D538C); auto& vcWindFactor=*reinterpret_cast<float*>(0x8D5390);
        auto& fogIntensity=*reinterpret_cast<float*>(0x8D5798);
        auto& maxDist=*reinterpret_cast<float*>(0xC6AA58); auto& usedNum=*reinterpret_cast<int32*>(0xC6AA5C);
        auto& cloudRot=*reinterpret_cast<float*>(0xC6AA70); auto& indivRot=*reinterpret_cast<int32*>(0xC6AA6C);
        auto* cloudTexPtrs=reinterpret_cast<void**>(0xC6AA74); auto& vcTex=*reinterpret_cast<void**>(0xC6BFC8);
        auto& fader=*reinterpret_cast<float*>(0xC6E970); auto& guards=*reinterpret_cast<uint32*>(0xC6E974);
        auto& gCamCoors=*reinterpret_cast<CVector*>(0xC6E964); auto& gPlyCoors=*reinterpret_cast<CVector*>(0xC6E958);
        auto& nv=*reinterpret_cast<int32*>(TEMP_NV); auto& ni=*reinterpret_cast<int32*>(TEMP_NI);
        auto* vc=reinterpret_cast<uint8*>(VC); auto* mf=reinterpret_cast<uint8*>(MF);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto setCamera=[&]{ const CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%4==0 ? uni(150.f,300.f) : uni(-50.f,300.f)};
                    if(next()%4){ for(int k=0;k<12;++k)At<float>(camMat.data(),k*4)=uni(-1.f,1.f); At<CVector>(camMat.data(),0x30)=p; At<uint32>(camMat.data(),0x3C)=0; At<void*>(camMat.data(),0x40)=nullptr; At<uint32>(camMat.data(),0x44)=0;
                        camMatrix=camMat.data(); camPos=randVec(-10.f,10.f); }
                    else { camMatrix=nullptr; camPos=p; }
                    camHeading=uni(-7.f,7.f); return p; };
                auto weightedUnit=[&]{ switch(next()%7){case 0:return 0.f;case 1:return 1.f;case 2:return uni(-0.3f,0.f);case 3:return uni(1.f,1.3f);default:return uni(0.f,1.f);} };
                auto fillMf=[&]{ const uint32 mode=next()%5;
                    for(int k=0;k<350;++k)mf[k]=mode==0 ? 1 : mode==1 ? 0 : uint8(next()%(mode==2 ? 8 : 2)!=0);
                    const CVector c=TheCamera.GetPosition();
                    for(int k=0;k<350;++k){ At<CVector>(mf,0x160+k*12)=c+randVec(-90.f,90.f); At<float>(mf,0x11C8+k*4)=uni(4.f,10.f); At<float>(mf,0x1740+k*4)=next()%8==0 ? uni(-1.f,0.5f) : uni(0.f,22.f);
                        At<float>(mf,0x1CB8+k*4)=uni(8.f,20.f); At<float>(mf,0x223C+k*4)=uni(0.5f,1.2f); }
                    At<CVector>(mf,0x2230)=next()%3 ? CVector{0.06f,0.06f,0.f} : randVec(-0.3f,0.3f);
                    for(int k=0;k<12;++k)At<uint32>(mf,0x27B4+k*4)=next();
                    static const uint32 prim[6]{0,1,2,0,2,3}; for(int k=0;k<6;++k)At<uint32>(mf,0x27E4+k*4)=next()%6==0 ? next()%4 : prim[k]; };
                auto fillVc=[&](float spread){ const uint32 mode=next()%5; const CVector c=TheCamera.GetPosition();
                    for(int k=0;k<180;++k){ vc[k]=mode==0 ? 1 : mode==1 ? 0 : uint8(next()%(mode==2 ? 8 : 2)!=0); vc[180+k]=uint8(next()%2);
                        At<CVector>(vc,0x168+k*12)=c+randVec(-spread,spread); At<CVector>(vc,0x9D8+k*12)=randVec(20.f,500.f); At<int32>(vc,0x1248+k*4)=next()%10==0 ? int32(next()%400)-50 : int32(36+next()%92); } };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC10Du; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    im3dOk=next()%6!=0; calcMask=next()%5==0 ? next() : (next()|next()|0x0F0F0F0Fu); calcCalls=0; noRainMask=next(); noRainCalls=0; canSeeOutside=next()%10!=0;
                    for(auto& t:textures){ for(auto& b:t)b=uint8(next()); }
                    for(int k=0;k<3;++k)cloudTexPtrs[k]=textures[k].data(); vcTex=textures[4].data(); // gpMoonMask, gpCloudTex, gpCloudMaskTex, ms_vc.texture
                    for(int k=0;k<9;++k)coronaTex[k]=textures[5+k%3].data();
                    fakePed.fill(0); fakePed[0x2F]=next()%8==0 ? uint8(1+next()%17) : 0; currArea=next()%8==0 ? int32(1+next()%17) : 0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    fillWords(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048); fillWords(reinterpret_cast<void*>(TEMP_IDX),0x2000);
                    nv=int32(next()%2049); ni=int32(next()%4097);
                    setCamera();
                    for(int k=0;k<0x40;++k)colours[k]=next()%6==0 ? uint16(next()%400) : uint16(next()%256);
                    timeStep=next()%8==0 ? 0.f : uni(0.f,4.f); wind=uni(0.f,1.5f); fogSF=weightedUnit(); fog=weightedUnit(); cloudCov=next()%6==0 ? fog : weightedUnit(); extraSunny=weightedUnit();

                    if(nm=="Init"){ findSlotResult=int32(next()%5000); fillWords(vc,VC_SIZE); fillWords(mf,MF_SIZE); cloudRot=uni(-5.f,5.f); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="Shutdown"){ if(next()%4==0)cloudTexPtrs[1]=nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="Update"){ camOrient=uni(-10.f,10.f); cloudRot=uni(-50.f,50.f); switch(next()%4){case 0:indivRot=int32(next()%100000);break;case 1:indivRot=int32(next());break;case 2:indivRot=int32(0x7FFFFF00u+next()%512);break;default:indivRot=int32(0xFFFFFF00u+next()%512);}
                        reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="SetUpOneSkyPoly"){ nv=int32(next()%2045); ni=int32(next()%4091);
                        reinterpret_cast<void(__cdecl*)(CVector,CVector,CVector,CVector,uint8,uint8,uint8,uint8,uint8,uint8)>(a)(randVec(-3000.f,3000.f),randVec(-3000.f,3000.f),randVec(-3000.f,3000.f),randVec(-3000.f,3000.f),
                            uint8(next()),uint8(next()),uint8(next()),uint8(next()),uint8(next()),uint8(next())); }
                    else if(nm=="RenderSkyPolys"){ for(int k=0;k<4;++k)grey[k]=uint8(next()); if(next()%6==0)camPos.z=next()%2 ? 25.f : 105.f;
                        reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="Render"){ hours=next()%3 ? uint8(next()%24) : uint8(next()%2 ? 22 : 5); mins=uint8(next()%60); secs=uint16(next()%60); days=uint8(next()%31);
                        rainbow=next()%3 ? 0.f : uni(0.f,1.2f); moonSize=next()%4 ? 3u : next()%8; farClipZ=uni(100.f,2000.f);
                        static const int16 sunny[]{0,2,6,11,13,17}; oldWeather=next()%3 ? sunny[next()%6] : int16(next()%23); newWeather=next()%3 ? sunny[next()%6] : int16(next()%23);
                        timeMs=next()%2 ? (next()&~0x1FFFu)|(next()%800) : next();
                        reinterpret_cast<void(__cdecl*)()>(a)(); out["roll"]=F(*reinterpret_cast<float*>(0xC6AA64)); out["sun_blocked"]=*reinterpret_cast<uint8*>(0xC3E030); }
                    else if(nm=="MovingFogInit"){ fillWords(mf,MF_SIZE); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="MovingFog_Create"){ fillMf(); CVector p=TheCamera.GetPosition()+randVec(-200.f,200.f); reinterpret_cast<void(__cdecl*)(CVector*)>(a)(&p); out["posn"]=json::array({F(p.x),F(p.y),F(p.z)}); }
                    else if(nm=="MovingFog_Delete"){ fillMf(); reinterpret_cast<void(__cdecl*)(int32)>(a)(next()%8==0 ? int32(next()%40)-20+(next()%2 ? 340 : 0) : int32(next()%350)); }
                    else if(nm=="MovingFog_Update"){ fillMf(); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="MovingFog_GetFXIntensity"){ fogSF=uni(-1.f,2.f); ret=F(reinterpret_cast<float(__cdecl*)()>(a)()); }
                    else if(nm=="MovingFog_GetWind"){ At<CVector>(mf,0x2230)=randVec(-1.f,1.f); CVector o{-1.f,-2.f,-3.f}; auto* r=reinterpret_cast<CVector*(__cdecl*)(CVector*)>(a)(&o);
                        ret=json::array({F(o.x),F(o.y),F(o.z),r==&o}); }
                    else if(nm=="MovingFog_GetFirstFreeSlot"){ fillMf(); if(next()%4==0){ for(int k=0;k<350;++k)mf[k]=1; if(next()%2)mf[next()%350]=0; } ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                    else if(nm=="MovingFogRender"){ fillMf(); fogIntensity=next()%8==0 ? 0.f : uni(-0.1f,1.1f); underWater=next()%6==0 ? uni(0.5f,1.f) : uni(0.f,0.5f);
                        if(next()%5==0)fogSF=0.f; reinterpret_cast<void(__cdecl*)()>(a)(); out["fog_intensity"]=F(fogIntensity); }
                    else if(nm=="VolumetricCloudsInit"){ fillWords(vc,VC_SIZE); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="VolumetricClouds_Create"){ fillVc(800.f); usedNum=next()%8==0 ? int32(next()%10)-4 : int32(next()%181); /* nunca > 180: std::array de la DLL abortaria */ maxDist=uni(50.f,700.f);
                        CVector p=TheCamera.GetPosition()+randVec(-500.f,500.f); reinterpret_cast<void(__cdecl*)(CVector*)>(a)(next()%3 ? &p : nullptr); out["posn"]=json::array({F(p.x),F(p.y),F(p.z)}); }
                    else if(nm=="VolumetricClouds_Delete"){ fillVc(800.f); reinterpret_cast<void(__cdecl*)(int32)>(a)(next()%8==0 ? int32(next()%40)-20+(next()%2 ? 170 : 0) : int32(next()%180)); }
                    else if(nm=="VolumetricClouds_GetFirstFreeSlot"){ fillVc(800.f); usedNum=next()%8==0 ? -int32(next()%0x7FFFFFFF) : next()%6==0 ? int32(next()%10)-4 : int32(next()%181);
                        if(next()%4==0){ for(int k=0;k<180;++k)vc[k]=1; if(next()%2)vc[next()%180]=0; } ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                    else if(nm=="VolumetricCloudsGetMaxDistance"){ for(auto& b:fakeRwCam)b=uint8(next()); At<float>(fakeRwCam.data(),0x84)=next()%6==0 ? 600.f : uni(50.f,1500.f); sceneCam=fakeRwCam.data();
                        ret=F(reinterpret_cast<float(__cdecl*)()>(a)()); }
                    else if(nm=="VolumetricCloudsRender"){ for(auto& b:fakeRwCam)b=uint8(next()); At<float>(fakeRwCam.data(),0x84)=uni(300.f,1200.f); sceneCam=fakeRwCam.data();
                        fxQuality=int32(next()%4); heightSwitch=uint8(next()%2); fader=next()%4==0 ? uni(240.f,260.f) : uni(0.f,100.f); vcWindFactor=uni(-2.f,2.f); windDir=randVec(-1.f,1.f);
                        guards=next()%4; fillVc(next()%3 ? 650.f : 50.f);
                        const CVector c=TheCamera.GetPosition(); playerCoors=c+randVec(-20.f,20.f);
                        if(next()%2){ gCamCoors=c; gPlyCoors=playerCoors; } else { gCamCoors=next()%2 ? c : randVec(-100.f,100.f); gPlyCoors=randVec(-100.f,100.f); }
                        camFwd984=randVec(-1.f,1.f);
                        if(next()%3==0){ fakeVeh.fill(0); for(int k=0;k<12;++k)At<float>(vehMat.data(),k*4)=uni(-1.f,1.f); At<CVector>(vehMat.data(),0x30)=c+randVec(-50.f,50.f);
                            At<CVector>(fakeVeh.data(),4)=c+randVec(-50.f,50.f); At<void*>(fakeVeh.data(),0x14)=vehMat.data(); fakeVehResult=fakeVeh.data(); }
                        else fakeVehResult=nullptr;
                        reinterpret_cast<void(__cdecl*)()>(a)(); }

                    out["clouds"]=Fnv(reinterpret_cast<void*>(CL_BEGIN),CL_END-CL_BEGIN); out["vc"]=Fnv(vc,VC_SIZE); out["mf"]=Fnv(mf,MF_SIZE);
                    out["head"]=ReversingProbe::Bytes(reinterpret_cast<void*>(CL_BEGIN),0x28); out["tail"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC6E954),0x24);
                    out["slots"]=Fnv(mf,350); out["used"]=Fnv(vc,360);
                    out["nv"]=nv; out["ni"]=ni; out["verts"]=Fnv(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048); out["idx"]=Fnv(reinterpret_cast<void*>(TEMP_IDX),0x2000);
                    out["verts0"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_VERTS),0x24*8);
                    out["globals"]=json::array({F(density),heightSwitch,F(vcWindFactor),F(fogIntensity),Id(cloudTexPtrs[0]),Id(cloudTexPtrs[1]),Id(cloudTexPtrs[2]),Id(vcTex)});
                    out["ret"]=ret; out["trace"]=trace;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",original},{"candidate",candidate}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
