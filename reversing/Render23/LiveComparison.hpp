#pragma once
// Banco dentro del motor: render, escaleras, puente y piel (lista de 23; 28 funciones). CBrightLights,
// CMotionBlurStreaks, CPlaneTrails, CShinyTexts, CEscalators, CBridge y CPlayerSkin. Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas. Las tablas globales (luces, estelas, rastros de
// aviones, textos brillantes, escaleras) se guardan, se rellenan con contenido coherente y se restauran en cada
// ejecucion. Se simulan y trazan: rand, estados de render (fpRenderStateSet), Im3D (direcciones del EXE; las de la
// DLL son envoltorios), LittleTest, la camara (TheCamera.m_matrix falsa o nula), CRegisteredMotionBlurStreak::
// Update/Render, CPlaneTrail::Render/RegisterPoint, CCoronas::RegisterCorona/UpdateCoronaCoors, CEscalator::
// SwitchOff/AddThisOne/Update (nunca se crean ni destruyen CObject reales), las funciones de CTxdStore, las
// primitivas RW de frames/clumps/texturas/imagenes/rasters y RtBMPImageRead (nunca se lee ningun fichero), y el pool
// de TXD (puntero sustituido por un pool falso). Todo el estado global tocado se guarda y se restaura.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "BrightLights.h"
#include "MotionBlurStreaks.h"
#include "RegisteredMotionBlurStreak.h"
#include "PlaneTrails.h"
#include "PlaneTrail.h"
#include "ShinyTexts.h"
#include "Escalators.h"
#include "Escalator.h"
#include "Bridge.h"
#include "PlayerSkin.h"
#include "Coronas.h"
#include "TxdStore.h"
#include "../ProbeRuntime.hpp"

namespace Render23Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t TEMP_VERTS=0xC4D958, TEMP_IDX=0xC4B958, TEMP_NV=0xC4B950, TEMP_NI=0xC4B954;
constexpr uintptr_t BL_NUM=0xC7C6FC, BL_ARR=0xC7CB58, ST_NUM=0xC7C6F8, ST_ARR=0xC7D258, MB_ARR=0xC7CA08, PT_ARR=0xC713A8, ESC_ARR=0xC6E9A8;
constexpr size_t BL_BIG=228; // 227-228 luces pequenas fuerzan el vaciado dentro del bucle (indices > 4056); se leen mas alla de
                              // aBrightLights (aShinyTexts, C3dMarkers, CCheckpoints y 0xC7F858-0xC7FD38), todo guardado y restaurado

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<uint8,0x48> camMat{};
alignas(16) inline std::array<uint8,0x60> handTex{};
alignas(16) inline std::array<uint8,0x40> fakeCam{}, fakeClump{};
alignas(16) inline std::array<uint8,0x100> camFrame{}, clumpFrame{};
alignas(16) inline std::array<uint8,0x60> preTex{}, newTex{};
alignas(16) inline std::array<uint8,0x40> fakeImage{}, fakeRaster{}, fakeDict{};
alignas(16) inline std::array<uint8,0x14> fakePool{};
alignas(16) inline std::array<uint8,12*8> poolObjects{};
alignas(16) inline std::array<uint8,8> poolStates{};
alignas(16) inline std::array<std::array<uint8,12>,4> vecs{};
alignas(16) inline std::array<uint8,0x40> fakeEntity{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- render ---
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline bool im3dOk{};
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ const size_t k=std::min<RwUInt32>(n,2048);
    trace.push_back({"im3d_transform",intptr_t(v)-intptr_t(TEMP_VERTS),n,uintptr_t(m),f,Fnv(v,k*0x24)}); return im3dOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ const size_t k=n<0 ? 0 : size_t(std::min<RwInt32>(n,4096));
    trace.push_back({"im3d_render_indexed",int(t),intptr_t(idx)-intptr_t(TEMP_IDX),n,Fnv(idx,k*2)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline void __cdecl FakeLittleTest(){ trace.push_back({"little_test"}); }

// --- estelas, rastros, coronas ---
inline void __fastcall FakeStreakUpdate(void* self,void*){ trace.push_back({"streak_update",intptr_t(self)-intptr_t(MB_ARR)}); }
inline void __fastcall FakeStreakRender(void* self,void*){ trace.push_back({"streak_render",intptr_t(self)-intptr_t(MB_ARR)}); }
inline void __fastcall FakeTrailRender(void* self,void*,float in){ trace.push_back({"trail_render",intptr_t(self)-intptr_t(PT_ARR),F(in)}); }
inline void __fastcall FakeTrailRegisterPoint(void* self,void*,CVector p){ trace.push_back({"trail_register_point",intptr_t(self)-intptr_t(PT_ARR),Vec(p)}); }
inline void __cdecl FakeRegisterCorona(uint32 id,CEntity* e,uint8 r,uint8 g,uint8 b,uint8 a,const CVector& pos,float size,float range,int32 type,int32 flare,int32 refl,int32 los,int32 trail,
                                       float normalAngle,uint8 neonFade,float pull,uint8 fullBright,float fadeSpeed,uint8 onlyBelow,uint8 whiteCore){
    trace.push_back({"register_corona",id,uintptr_t(e),r,g,b,a,Vec(pos),F(size),F(range),type,flare,refl,los,trail,F(normalAngle),neonFade,F(pull),fullBright,F(fadeSpeed),onlyBelow,whiteCore}); }
inline void __cdecl FakeUpdateCoronaCoors(uint32 id,const CVector& pos,float range,float normalAngle){ trace.push_back({"update_corona_coors",id,Vec(pos),F(range),F(normalAngle)}); }

// --- escaleras ---
inline json Esc(const void* self){ const intptr_t d=intptr_t(self)-intptr_t(ESC_ARR); return json::array({d/0x150,d%0x150}); }
inline void __fastcall FakeEscSwitchOff(void* self,void*){ trace.push_back({"escalator_switch_off",Esc(self)}); }
inline void __fastcall FakeEscUpdate(void* self,void*){ trace.push_back({"escalator_update",Esc(self)}); }
inline void __fastcall FakeEscAddThisOne(void* self,void*,const CVector* s,const CVector* b,const CVector* t,const CVector* e,uint32 down,CEntity* ent){
    trace.push_back({"escalator_add_this_one",Esc(self),Id(s),Id(b),Id(t),Id(e),down&0xFF,Id(ent)}); }

// --- piel del jugador: TXD, frames, texturas ---
inline int32 addSlotResult{};
inline void __cdecl FakePushTxd(){ trace.push_back({"push_current_txd"}); }
inline void __cdecl FakePopTxd(){ trace.push_back({"pop_current_txd"}); }
inline void __cdecl FakeSetTxd(int32 i){ trace.push_back({"set_current_txd",i}); }
inline int32 __cdecl FakeAddTxdSlot(const char* n){ trace.push_back({"add_txd_slot",std::string(n)}); return addSlotResult; }
inline void __cdecl FakeTxdCreate(int32 i){ trace.push_back({"txd_create",i}); }
inline void __cdecl FakeTxdAddRef(int32 i){ trace.push_back({"txd_add_ref",i}); }
inline void __cdecl FakeRemoveTxdSlot(int32 i){ trace.push_back({"remove_txd_slot",i}); }
inline RwFrame* __cdecl FakeFrameTransform(RwFrame* f,const RwMatrix* m,int32 c){ trace.push_back({"frame_transform",Id(f),Id(m),c}); return f; }
inline RwFrame* __cdecl FakeFrameTranslate(RwFrame* f,const RwV3d* v,int32 c){ trace.push_back({"frame_translate",Id(f),F(v->x),F(v->y),F(v->z),c}); return f; }
inline RwFrame* __cdecl FakeFrameRotate(RwFrame* f,const RwV3d* v,float ang,int32 c){ trace.push_back({"frame_rotate",Id(f),F(v->x),F(v->y),F(v->z),F(ang),c}); return f; }
inline RwFrame* __cdecl FakeFrameUpdateObjects(RwFrame* f){ trace.push_back({"frame_update_objects",Id(f)}); return f; }
inline void __cdecl FakeSetAmbient(RwRGBAReal* c){ trace.push_back({"set_ambient_colours",F(c->red),F(c->green),F(c->blue),F(c->alpha)}); }
inline RpClump* __cdecl FakeClumpRender(RpClump* c){ trace.push_back({"clump_render",Id(c)}); return c; }
inline void* texReadResult{}; inline void* bmpResult{};
inline int32 fmtW{}, fmtH{}, fmtD{}, fmtFlags{};
inline RwTexture* __cdecl FakeTextureRead(const char* n,const char* m){ trace.push_back({"texture_read",std::string(n),uintptr_t(m)}); return static_cast<RwTexture*>(texReadResult); }
inline RwImage* __cdecl FakeBmpRead(const char* n){ trace.push_back({"bmp_image_read",std::string(n)}); return static_cast<RwImage*>(bmpResult); }
inline RwImage* __cdecl FakeFindRasterFormat(RwImage* i,RwInt32 type,RwInt32* w,RwInt32* h,RwInt32* d,RwInt32* fl){ trace.push_back({"image_find_raster_format",Id(i),type}); *w=fmtW; *h=fmtH; *d=fmtD; *fl=fmtFlags; return i; }
inline RwRaster* __cdecl FakeRasterCreate(RwInt32 w,RwInt32 h,RwInt32 d,RwInt32 fl){ trace.push_back({"raster_create",w,h,d,fl}); return reinterpret_cast<RwRaster*>(fakeRaster.data()); }
inline RwRaster* __cdecl FakeRasterSetFromImage(RwRaster* r,RwImage* i){ trace.push_back({"raster_set_from_image",Id(r),Id(i)}); return r; }
inline RwTexture* __cdecl FakeTextureCreate(RwRaster* r){ trace.push_back({"texture_create",Id(r)}); return reinterpret_cast<RwTexture*>(newTex.data()); }
inline RwTexture* __cdecl FakeTextureSetName(RwTexture* t,const char* n){ trace.push_back({"texture_set_name",Id(t),std::string(n)}); return t; }
inline RwTexture* __cdecl FakeDictAddTexture(RwTexDictionary* d,RwTexture* t){ trace.push_back({"dict_add_texture",Id(d),uintptr_t(d),Id(t),At<uint32>(t,0x50)}); return t; }
inline RwBool __cdecl FakeImageDestroy(RwImage* i){ trace.push_back({"image_destroy",Id(i)}); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,28> fns{{
    {"CBrightLights","Init",0x722140,0,-1,64},{"CBrightLights","RegisterOne",0x724770,0,-1,1024},{"CBrightLights","Render",0x7241C0,0,-1,512},
    {"CBrightLights","RenderOutGeometryBuffer",0x722150,0,-1,256},
    {"CMotionBlurStreaks","Init",0x721D90,0,-1,64},{"CMotionBlurStreaks","RegisterStreak",0x721DC0,0,-1,1024},{"CMotionBlurStreaks","Render",0x7240E0,0,-1,256},
    {"CMotionBlurStreaks","Update",0x7240C0,0,-1,256},
    {"CPlaneTrails","Init",0x717370,0,-1,64},{"CPlaneTrails","RegisterPoint",0x7174C0,0,-1,256},{"CPlaneTrails","Render",0x7173A0,0,-1,512},
    {"CPlaneTrails","Update",0x7174F0,0,-1,1024},
    {"CShinyTexts","Init",0x7221B0,0,-1,64},{"CShinyTexts","RegisterOne",0x724B60,0,-1,1024},{"CShinyTexts","Render",0x724890,0,-1,512},
    {"CShinyTexts","RenderOutGeometryBuffer",0x7221C0,0,-1,256},
    {"CEscalators","AddOne",0x717C90,0,-1,256},{"CEscalators","Init",0x717C50,0,-1,64},{"CEscalators","Shutdown",0x717940,0,-1,64},{"CEscalators","Update",0x718580,0,-1,256},
    {"CBridge","FindBridgeEntities",0x41BCA0,0,-1,64},{"CBridge","Init",0x41BC70,0,-1,64},{"CBridge","ShouldLightsBeFlashing",0x41BC90,0,-1,64},{"CBridge","Update",0x41BC80,0,-1,64},
    {"CPlayerSkin","GetSkinTexture",0x6FFA10,0,-1,512},{"CPlayerSkin","Initialise",0x6FF8A0,0,-1,64},{"CPlayerSkin","RenderFrontendSkinEdit",0x6FF900,0,-1,512},
    {"CPlayerSkin","Shutdown",0x6FF8D0,0,-1,64},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"render23-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"render23-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("render23");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"render23-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,28> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,28> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ Reg(camMat.data(),camMat.size()); Reg(handTex.data(),handTex.size()); Reg(fakeCam.data(),fakeCam.size()); Reg(fakeClump.data(),fakeClump.size());
        Reg(camFrame.data(),camFrame.size()); Reg(clumpFrame.data(),clumpFrame.size()); Reg(preTex.data(),preTex.size()); Reg(newTex.data(),newTex.size());
        Reg(fakeImage.data(),fakeImage.size()); Reg(fakeRaster.data(),fakeRaster.size()); Reg(fakeDict.data(),fakeDict.size()); Reg(fakePool.data(),fakePool.size());
        Reg(poolObjects.data(),poolObjects.size()); for(auto& v:vecs)Reg(v.data(),v.size()); Reg(fakeEntity.data(),fakeEntity.size()); }

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {TEMP_NV,8},{TEMP_IDX,0x2000},{TEMP_VERTS,0x24*2048},
            {BL_NUM,4},{BL_ARR,BL_BIG*0x38},{ST_NUM,4},{MB_ARR,4*0x54},{PT_ARR,3*0x100},{ESC_ARR,32*0x150},
            {0xB6F02C,12},{0xB6F03C,4},{0xB6F118,4},       // TheCamera: posicion, m_matrix, m_fLODDistMultiplier
            {0xB7C4CA,6},                                  // CTimeCycle::m_CurrentColours (cielo inferior)
            {0xC81300,0x28},                               // CWeather::Foggyness, CloudCoverage ... Rain
            {0xB7CB84,4},{0xB7CB7C,4},{0xB70153,1},{0xA43088,1},
            {0xC40404,4},{0xC3F03C,0x14},{0xC1703C,4},{0xC8800C,4},{0xB71670,352}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRender), i3(0x7EF520,FakeIm3DEnd); // RwIm3D* de la DLL son envoltorios del EXE
        EntryPatch lt(reinterpret_cast<uintptr_t>(&LittleTest),FakeLittleTest), lte(0x541330,FakeLittleTest);
        EntryPatch su(bit_cast<uintptr_t>(&CRegisteredMotionBlurStreak::Update),FakeStreakUpdate), sue(0x721EE0,FakeStreakUpdate);
        EntryPatch sr(bit_cast<uintptr_t>(&CRegisteredMotionBlurStreak::Render),FakeStreakRender), sre(0x721F70,FakeStreakRender);
        EntryPatch tr(bit_cast<uintptr_t>(&CPlaneTrail::Render),FakeTrailRender), tre(0x717180,FakeTrailRender);
        EntryPatch tp(bit_cast<uintptr_t>(&CPlaneTrail::RegisterPoint),FakeTrailRegisterPoint), tpe(0x7172D0,FakeTrailRegisterPoint);
        EntryPatch rc(bit_cast<uintptr_t>(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona)),FakeRegisterCorona), rce(0x6FC580,FakeRegisterCorona);
        EntryPatch uc(bit_cast<uintptr_t>(&CCoronas::UpdateCoronaCoors),FakeUpdateCoronaCoors), uce(0x6FC4D0,FakeUpdateCoronaCoors);
        EntryPatch es(bit_cast<uintptr_t>(&CEscalator::SwitchOff),FakeEscSwitchOff), ese(0x717860,FakeEscSwitchOff);
        EntryPatch ea(bit_cast<uintptr_t>(&CEscalator::AddThisOne),FakeEscAddThisOne), eae(0x717970,FakeEscAddThisOne);
        EntryPatch eu(bit_cast<uintptr_t>(&CEscalator::Update),FakeEscUpdate), eue(0x717D30,FakeEscUpdate);
        EntryPatch x1(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), x1e(0x7316A0,FakePushTxd);
        EntryPatch x2(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), x2e(0x7316B0,FakePopTxd);
        EntryPatch x3(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), x3e(0x7319C0,FakeSetTxd);
        EntryPatch x4(bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot),FakeAddTxdSlot), x4e(0x731C80,FakeAddTxdSlot);
        EntryPatch x5(bit_cast<uintptr_t>(&CTxdStore::Create),FakeTxdCreate), x5e(0x731990,FakeTxdCreate);
        EntryPatch x6(bit_cast<uintptr_t>(&CTxdStore::AddRef),FakeTxdAddRef), x6e(0x731A00,FakeTxdAddRef);
        EntryPatch x7(bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot),FakeRemoveTxdSlot), x7e(0x731CD0,FakeRemoveTxdSlot);
        EntryPatch am(bit_cast<uintptr_t>(static_cast<void(*)(RwRGBAReal*)>(&SetAmbientColours)),FakeSetAmbient), ame(0x735D50,FakeSetAmbient);
        // primitivas RW (las de la DLL son envoltorios del EXE)
        EntryPatch w1(0x7F0F70,FakeFrameTransform), w2(0x7F0E30,FakeFrameTranslate), w3(0x7F1010,FakeFrameRotate), w4(0x7F0910,FakeFrameUpdateObjects), w5(0x749B20,FakeClumpRender);
        EntryPatch w6(0x7F3AC0,FakeTextureRead), w7(0x7CDF60,FakeBmpRead), w8(0x8042C0,FakeFindRasterFormat), w9(0x7FB230,FakeRasterCreate), w10(0x804290,FakeRasterSetFromImage);
        EntryPatch w11(0x7F37C0,FakeTextureCreate), w12(0x7F38A0,FakeTextureSetName), w13(0x7F3980,FakeDictAddTexture), w14(0x802740,FakeImageDestroy);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;
        auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C); auto& camPos=*reinterpret_cast<CVector*>(0xB6F02C); auto& lod=*reinterpret_cast<float*>(0xB6F118);
        auto* sky=reinterpret_cast<uint16*>(0xB7C4CA);
        auto& fog=*reinterpret_cast<float*>(0xC81300); auto& clouds=*reinterpret_cast<float*>(0xC81304); auto& rain=*reinterpret_cast<float*>(0xC81324);
        auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); auto& pauseMs=*reinterpret_cast<uint32*>(0xB7CB7C); auto& hours=*reinterpret_cast<uint8*>(0xB70153);
        auto& replayMode=*reinterpret_cast<uint8*>(0xA43088); auto& handManTex=*reinterpret_cast<void**>(0xC40404);
        auto& txdSlot=*reinterpret_cast<int32*>(0xC3F03C); auto& playerClump=*reinterpret_cast<void**>(0xC3F040);
        auto& skinAngle=*reinterpret_cast<float*>(0xC3F048); auto& skinFlash=*reinterpret_cast<uint32*>(0xC3F04C);
        auto& sceneCam=*reinterpret_cast<void**>(0xC1703C); auto& txdPool=*reinterpret_cast<void**>(0xC8800C);
        auto* gstr=reinterpret_cast<char*>(0xB71670);
        auto& nv=*reinterpret_cast<int32*>(TEMP_NV); auto& ni=*reinterpret_cast<int32*>(TEMP_NI);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto fillWords=[&](void* p,size_t n){ auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=next()&0x3F7F7F7Fu; }; // sin NaN ni denormales raros
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto setCamera=[&]{ const CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,300.f)};
                    if(next()%3){ fill(camMat.data(),0x30); At<CVector>(camMat.data(),0x30)=p; At<void*>(camMat.data(),0x40)=nullptr; camMatrix=camMat.data(); camPos=randVec(-10.f,10.f); }
                    else { camMatrix=nullptr; camPos=p; }
                    return p; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2323u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    im3dOk=next()%5!=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // buferes temporales con contenido determinista
                    fillWords(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048); fillWords(reinterpret_cast<void*>(TEMP_IDX),0x2000);
                    nv=int32(next()%2049); ni=int32(next()%4097); if(next()%8==0)ni=int32(0x10000u|(next()%2 ? 0u : next()%4097)); if(next()%8==0)nv|=int32(0x10000);
                    auto tempOut=[&]{ out["nv"]=nv; out["ni"]=ni; out["verts"]=Fnv(reinterpret_cast<void*>(TEMP_VERTS),0x24*2048); out["idx"]=Fnv(reinterpret_cast<void*>(TEMP_IDX),0x2000);
                        out["verts0"]=ReversingProbe::Bytes(reinterpret_cast<void*>(TEMP_VERTS),0x24*16); };

                    if(cls=="CBrightLights") {
                        auto& num=*reinterpret_cast<int32*>(BL_NUM); auto* arr=reinterpret_cast<uint8*>(BL_ARR);
                        static const uint8 colors[]{1,2,3,13,14,15};
                        auto mkLight=[&](uint8* L,bool smallOnly){
                            At<CVector>(L,0)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)}; At<CVector>(L,0xC)=randVec(-3.f,3.f); At<CVector>(L,0x18)=randVec(-3.f,3.f); At<CVector>(L,0x24)=randVec(-1.f,1.f);
                            float d; switch(next()%7){case 0:case 1:d=uni(0.f,45.f);break;case 2:case 3:d=uni(45.f,60.f);break;case 4:d=uni(60.f,90.f);break;case 5:d=45.f;break;default:d=uni(-10.f,0.f);} At<float>(L,0x30)=d;
                            uint8 c=colors[next()%6]; if(smallOnly)c=uint8(13+next()%3); else if(next()%8==0)c=uint8(next()); L[0x34]=c; L[0x35]=uint8(next()); L[0x36]=uint8(next()); L[0x37]=uint8(next()); };
                        if(nm=="Init"){ num=int32(next()%40); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RegisterOne"){ fill(arr,32*0x38); num=next()%8==0 ? int32(30+next()%5) : int32(next()%32); const CVector cam=setCamera(); lod=next()%4==0 ? 1.f : uni(0.5f,2.5f);
                            const float r=next()%6==0 ? 60.f*lod : uni(0.f,100.f); const CVector dir=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                            const float m=std::sqrt(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z)+1e-3f; const CVector p{cam.x+dir.x/m*r,cam.y+dir.y/m*r,cam.z+dir.z/m*r};
                            uint8 c=colors[next()%6]; if(next()%8==0)c=0; else if(next()%10==0)c=uint8(next());
                            reinterpret_cast<void(__cdecl*)(CVector,CVector,CVector,CVector,uint8,uint8,uint8,uint8)>(a)(p,randVec(-3.f,3.f),randVec(-3.f,3.f),randVec(-1.f,1.f),c,uint8(next()),uint8(next()),uint8(next()));
                            out["lights"]=ReversingProbe::Bytes(arr,32*0x38); }
                        else if(nm=="Render"){ const bool big=next()%8==0; num=big ? int32(227+next()%2) : (next()%8==0 ? 0 : int32(1+next()%32));
                            for(int32 k=0;k<(big ? int32(BL_BIG) : 32);++k)mkLight(arr+k*0x38,big);
                            reinterpret_cast<void(__cdecl*)()>(a)(); tempOut(); }
                        else { reinterpret_cast<void(__cdecl*)()>(a)(); tempOut(); }
                        out["num"]=num;
                    }
                    else if(cls=="CShinyTexts") {
                        auto& num=*reinterpret_cast<int32*>(ST_NUM); auto* arr=reinterpret_cast<uint8*>(ST_ARR);
                        if(nm=="Init"){ num=int32(next()%40); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RegisterOne"){ fill(arr,32*0x58); num=next()%8==0 ? int32(30+next()%5) : int32(next()%32); const CVector cam=setCamera();
                            const float maxD=next()%10==0 ? uni(-5.f,0.f) : uni(1.f,120.f); const float r=next()%6==0 ? maxD : next()%5==0 ? maxD*0.5f : uni(0.f,150.f);
                            const CVector dir=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float m=std::sqrt(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z)+1e-3f;
                            const CVector p{cam.x+dir.x/m*r,cam.y+dir.y/m*r,cam.z+dir.z/m*r};
                            float uv[8]; for(auto& u:uv)u=uni(-1.f,2.f);
                            const uint8 always=next()%6==0 ? uint8(next()%3) : 1;
                            reinterpret_cast<void(__cdecl*)(CVector,CVector,CVector,CVector,float,float,float,float,float,float,float,float,uint8,uint8,uint8,uint8,float)>(a)(
                                p,p+randVec(-5.f,5.f),p+randVec(-5.f,5.f),p+randVec(-5.f,5.f),uv[0],uv[1],uv[2],uv[3],uv[4],uv[5],uv[6],uv[7],always,uint8(next()),uint8(next()),uint8(next()),maxD);
                            out["texts"]=ReversingProbe::Bytes(arr,32*0x58); }
                        else if(nm=="Render"){ num=next()%8==0 ? 0 : int32(1+next()%32);
                            for(int k=0;k<32;++k){ uint8* T=arr+k*0x58; for(int c=0;c<4;++c)At<CVector>(T,c*12)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                                for(int c=0;c<8;++c)At<float>(T,0x30+c*4)=uni(-1.f,2.f); At<float>(T,0x50)=uni(0.f,100.f); T[0x54]=next()%5==0 ? uint8(next()%3) : 1; T[0x55]=uint8(next()); T[0x56]=uint8(next()); T[0x57]=uint8(next()); }
                            fill(handTex.data(),handTex.size()); handManTex=next()%6==0 ? nullptr : handTex.data();
                            reinterpret_cast<void(__cdecl*)()>(a)(); tempOut(); }
                        else { reinterpret_cast<void(__cdecl*)()>(a)(); tempOut(); }
                        out["num"]=num;
                    }
                    else if(cls=="CMotionBlurStreaks") {
                        auto* arr=reinterpret_cast<uint8*>(MB_ARR); fill(arr,4*0x54);
                        for(int k=0;k<4;++k){ for(size_t o=8;o<0x50;o+=4)At<float>(arr+k*0x54,o)=uni(-3000.f,3000.f); At<uint32>(arr+k*0x54,0)=next()%3==0 ? 0u : (next()%2 ? 1+next()%6 : next()); }
                        if(nm=="RegisterStreak"){ uint32 id=next()%3 ? 1+next()%6 : next(); if(next()%6==0)id=At<uint32>(arr+(next()%4)*0x54,0);
                            reinterpret_cast<void(__cdecl*)(uint32,uint8,uint8,uint8,uint8,CVector,CVector)>(a)(id,uint8(next()),uint8(next()),uint8(next()),uint8(next()),randVec(-3000.f,3000.f),randVec(-3000.f,3000.f)); }
                        else if(nm=="Render"){ for(int k=0;k<3;++k)sky[k]=next()%6==0 ? uint16(next()) : uint16(next()%256); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                        out["streaks"]=ReversingProbe::Bytes(arr,4*0x54);
                    }
                    else if(cls=="CPlaneTrails") {
                        auto* arr=reinterpret_cast<uint8*>(PT_ARR); fill(arr,3*0x100);
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="RegisterPoint")reinterpret_cast<void(__cdecl*)(CVector,uint32)>(a)(randVec(-3000.f,3000.f),next()%4 ? next()%3 : next()%8);
                        else if(nm=="Render"){ auto w=[&]{ switch(next()%6){case 0:return 0.f;case 1:return 1.f;case 2:return uni(-0.3f,0.f);case 3:return uni(1.f,1.3f);default:return uni(0.f,1.f);} };
                            fog=w(); clouds=next()%6==0 ? fog : w(); rain=next()%6==0 ? clouds : w();
                            for(int k=0;k<3;++k)sky[k]=next()%8==0 ? uint16(next()) : uint16(next()%256); if(next()%8==0)sky[0]=sky[1]=sky[2]=0;
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else { timeMs=next()%3 ? next()%2000000 : next(); hours=uint8(next()%24); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["trails"]=ReversingProbe::Bytes(arr,3*0x100);
                    }
                    else if(cls=="CEscalators") {
                        auto* arr=reinterpret_cast<uint8*>(ESC_ARR); const uint32 mode=next()%4;
                        for(int k=0;k<32;++k)arr[k*0x150+0x78]=mode==0 ? 1 : mode==1 ? 0 : uint8(next()%2);
                        replayMode=uint8(next()%4==0 ? 1 : next()%3);
                        if(nm=="AddOne"){ for(auto& v:vecs)fill(v.data(),v.size());
                            reinterpret_cast<void(__cdecl*)(const void*,const void*,const void*,const void*,uint32,void*)>(a)(vecs[0].data(),vecs[1].data(),vecs[2].data(),vecs[3].data(),next()%2,next()%4 ? fakeEntity.data() : nullptr); }
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                        json ex=json::array(); for(int k=0;k<32;++k)ex.push_back(arr[k*0x150+0x78]); out["exist"]=ex; out["array"]=Fnv(arr,32*0x150);
                    }
                    else if(cls=="CBridge") {
                        if(nm=="ShouldLightsBeFlashing")ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)());
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(cls=="CPlayerSkin") {
                        txdSlot=int32(next()%8);
                        if(nm=="Initialise"){ addSlotResult=next()%4 ? int32(next()%5000) : -1; reinterpret_cast<void(__cdecl*)()>(a)(); out["slot"]=txdSlot; }
                        else if(nm=="Shutdown"){ if(next()%4==0)txdSlot=int32(next()); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RenderFrontendSkinEdit"){ fill(fakeCam.data(),fakeCam.size()); fill(fakeClump.data(),fakeClump.size()); fill(camFrame.data(),camFrame.size()); fill(clumpFrame.data(),clumpFrame.size());
                            fakeClump[0]=2; At<void*>(fakeCam.data(),4)=camFrame.data(); At<void*>(fakeClump.data(),4)=clumpFrame.data(); sceneCam=fakeCam.data(); playerClump=fakeClump.data();
                            pauseMs=next()%3 ? next()%5000000 : next(); skinFlash=next()%3 ? pauseMs-next()%16 : next();
                            skinAngle=next()%6==0 ? 358.f+float(next()%3) : uni(-5.f,370.f);
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["angle"]=F(skinAngle); out["flash"]=skinFlash; }
                        else { static const char* names[]{"","$$\"\"","player","skin01","$$\"","$$\"\"x","cj_custom_skin_with_a_long_name","a","PLAYER"};
                            const char* name=names[next()%std::size(names)];
                            for(size_t k=0;k<351;++k)gstr[k]=char('a'+next()%26); gstr[351]=0;
                            fill(preTex.data(),preTex.size()); fill(newTex.data(),newTex.size()); fill(fakeImage.data(),fakeImage.size()); fill(fakeRaster.data(),fakeRaster.size());
                            texReadResult=next()%4==0 ? preTex.data() : nullptr; bmpResult=next()%5==0 ? nullptr : fakeImage.data();
                            fmtW=int32(next()%1024); fmtH=int32(next()%1024); fmtD=int32(next()%33); fmtFlags=int32(next());
                            fill(poolObjects.data(),poolObjects.size()); for(auto& s:poolStates)s=uint8(next()%0x7F);
                            for(int k=0;k<8;++k)At<void*>(poolObjects.data(),k*12)=next()%3 ? static_cast<void*>(fakeDict.data()) : reinterpret_cast<void*>(uintptr_t(0x10000000+next()%0x1000*4));
                            At<void*>(fakePool.data(),0)=poolObjects.data(); At<void*>(fakePool.data(),4)=poolStates.data(); At<int32>(fakePool.data(),8)=8; At<int32>(fakePool.data(),0xC)=-1; At<uint32>(fakePool.data(),0x10)=0;
                            txdPool=fakePool.data();
                            ret=Id(reinterpret_cast<RwTexture*(__cdecl*)(const char*)>(a)(name));
                            out["gstring"]=std::string(gstr); out["gstring_hash"]=Fnv(gstr,352); out["new_tex"]=ReversingProbe::Bytes(newTex.data(),newTex.size()); out["pre_tex"]=Fnv(preTex.data(),preTex.size()); }
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
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
