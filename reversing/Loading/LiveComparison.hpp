#pragma once
// Banco dentro del motor: CLoadingScreen (20 funciones) y LoadingScreen. Original (hooks del lote
// desactivados) frente a candidato con el estado estatico de la pantalla de carga aleatorizado por caso
// y la configuracion del cargador rapido en sus valores por defecto (equivalentes al juego original).
// El reloj (GetMillisecondTime), el RNG (srand/rand), QueryPerformanceCounter, CTxdStore, CFileMgr,
// CSprite2d (texturas, dibujo y barra), los estados de render, la camara de RW y el tema de carga se
// simulan y se trazan. Se compara el estado estatico, el retorno y la traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "LoadingScreen.h"
#include "Sprite2d.h"
#include "TxdStore.h"
#include "platform.h"
#include "extensions/Configs/FastLoader.hpp"
#include "../ProbeRuntime.hpp"

namespace LoadingLive {
// estado de CLoadingScreen (miembros privados en la DLL): referencias a las mismas direcciones del EXE
struct LS {
    static inline auto& m_bActive = StaticRef<bool>(0xBAB318); static inline auto& m_bWantToPause = StaticRef<bool>(0xBAB319); static inline auto& m_bPaused = StaticRef<bool>(0xBAB31A);
    static inline auto& m_aSplashes = StaticRef<std::array<uint32,7>>(0xBAB35C); static inline auto& m_currDisplayedSplash = StaticRef<int32>(0x8D093C);
    static inline auto& m_bFading = StaticRef<bool>(0xBAB31C); static inline auto& m_bLegalScreen = StaticRef<bool>(0xBAB31D);
    static inline auto& m_bFadeInNextSplashFromBlack = StaticRef<bool>(0xBAB31E); static inline auto& m_bFadeOutCurrSplashToBlack = StaticRef<bool>(0xBAB31F);
    static inline auto& m_FadeAlpha = StaticRef<uint8>(0xBAB320); static inline auto& m_StartFadeTime = StaticRef<float>(0xBAB324); static inline auto& m_ClockTimeOnPause = StaticRef<float>(0xBAB328);
    static inline auto& m_PauseTime = StaticRef<float>(0xBAB32C); static inline auto& m_PercentLoaded = StaticRef<float>(0xBAB330); static inline auto& m_TimeStartedLoading = StaticRef<float>(0xBAB334);
    static inline auto& m_TimeBarAppeared = StaticRef<float>(0xBAB338); static inline auto& m_PopUpMessage = StaticRef<char[16]>(0xBAB268);
    static inline auto& m_bSignalDelete = StaticRef<bool>(0xBAB33C); static inline auto& m_bReadyToDelete = StaticRef<bool>(0xBAB33D);
    static inline auto& m_numChunksLoaded = StaticRef<int32>(0x8D0940); static inline auto& m_chunkBarAppeared = StaticRef<int32>(0x8D0944); static inline auto& m_timeSinceLastScreen = StaticRef<float>(0xBAB340);
};
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 clockMs{}; inline int clockStep{}; inline int seed{}, rngCalls{}; inline bool cameraOk{};
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline json Rect(const CRect& r) { return json::array({F(r.left),F(r.top),F(r.right),F(r.bottom)}); }
inline json Col(const CRGBA& c) { return json::array({c.r,c.g,c.b,c.a}); }
inline uint64 FakeMs() { trace.push_back({"ms"}); const uint32 r=clockMs+uint32(clockStep++)*37u; return r; }
inline int32 __cdecl FakeMsExe() { return int32(FakeMs()); }
inline void __cdecl FakeSrand(uint32) { trace.push_back({"srand"}); } // la semilla sale del reloj real (QueryPerformanceCounter): no se compara
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline BOOL WINAPI FakeQPC(LARGE_INTEGER* p) { trace.push_back({"qpc"}); p->QuadPart=0x123456789ull+uint64(seed)*977u; return TRUE; }
inline int32 __cdecl AddTxdSlot(const char* n) { trace.push_back({"add_txd_slot",std::string(n)}); return 42; }
inline bool __cdecl LoadTxd(int32 i,const char* n) { trace.push_back({"load_txd",i,std::string(n)}); return true; }
inline void __cdecl TxdAddRef(int32 i) { trace.push_back({"txd_add_ref",i}); }
inline void __cdecl PushTxd() { trace.push_back({"push_txd"}); }
inline void __cdecl PopTxd() { trace.push_back({"pop_txd"}); }
inline void __cdecl SetTxd(int32 i) { trace.push_back({"set_txd",i}); }
inline int32 __cdecl FindTxd(const char* n) { trace.push_back({"find_txd",std::string(n)}); return (n[strlen(n)-1]=='s') ? 42 : -1; }
inline void __cdecl RemoveTxd(int32 i) { trace.push_back({"remove_txd",i}); }
inline void __cdecl RemoveTxdSlot(int32 i) { trace.push_back({"remove_txd_slot",i}); }
inline int32 __cdecl SetDir(const char* d) { trace.push_back({"set_dir",std::string(d)}); return 0; }
inline uint32 SpriteIndex(const void* s) { return uint32((intptr_t(s)-intptr_t(&LS::m_aSplashes))/4); }
inline void __fastcall SetTexture(CSprite2d* s,void*,const char* n) { trace.push_back({"set_texture",SpriteIndex(s),std::string(n)}); *reinterpret_cast<uint32*>(s)=0x5000+SpriteIndex(s); }
inline void __fastcall SpriteDelete(CSprite2d* s,void*) { trace.push_back({"sprite_delete",SpriteIndex(s)}); *reinterpret_cast<uint32*>(s)=0; }
inline void __fastcall SpriteDraw(CSprite2d* s,void*,const CRect& r,const CRGBA& c) { trace.push_back({"sprite_draw",int32(SpriteIndex(s)),Rect(r),Col(c)}); }
inline void __cdecl DrawRect(const CRect& r,const CRGBA& c) { trace.push_back({"draw_rect",Rect(r),Col(c)}); }
inline void __cdecl InitPerFrame() { trace.push_back({"init_per_frame"}); }
inline void __cdecl BarChart(float x,float y,uint16 w,uint8 h,float p,int8 add,uint8 pct,uint8 border,CRGBA c,CRGBA c2) { trace.push_back({"bar_chart",F(x),F(y),w,h,F(p),add,pct,border,Col(c),Col(c2)}); }
inline RwBool __cdecl StateSet(RwRenderState st,void* v) { trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
inline RwCamera* __cdecl CamBegin(RwCamera* c) { trace.push_back({"camera_begin"}); return cameraOk ? c : nullptr; }
inline RwCamera* __cdecl CamEnd(RwCamera* c) { trace.push_back({"camera_end"}); return c; }
inline RwCamera* __cdecl ShowRaster(RwCamera* c) { trace.push_back({"show_raster"}); return c; }
inline void __cdecl DefState() { trace.push_back({"defined_state_2d"}); }
inline void __fastcall LoadingTune(void*,void*,float v) { trace.push_back({"loading_tune",F(v)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CLoadingScreen")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    if(!r)ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,20> fns{{
    {"Init",0x5902B0,256},{"Shutdown",0x58FF10,128},{"RenderSplash",0x58FF60,512},{"LoadSplashes",0x5900B0,256},{"DisplayMessage",0x590220,64},
    {"SetLoadingBarMsg",0x590240,128},{"GetClockTime",0x590280,512},{"Pause",0x590310,16},{"Continue",0x590320,256},{"RenderLoadingBar",0x590370,1024},
    {"DisplayNextSplash",0x5904D0,256},{"StartFading",0x590530,128},{"DisplayPCScreen",0x590570,512},{"Update",0x5905E0,2048},{"DoPCTitleFadeIn",0x590860,64},
    {"DoPCTitleFadeOut",0x590990,64},{"DoPCScreenChange",0x590AC0,512},{"NewChunkLoaded",0x590D00,2048},{"IsActive",0x744DB0,32},{"LoadingScreen",0x53DED0,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"loading-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"loading-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("loading");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    // estado estatico de la pantalla de carga: 0xBAB268..0xBAB378 y 0x8D093C..0x8D0947
    auto* st1=reinterpret_cast<uint8*>(0xBAB268); constexpr size_t N1=0xBAB378-0xBAB268; auto* st2=reinterpret_cast<uint8*>(0x8D093C); constexpr size_t N2=12;
    const std::vector<uint8> saved1(st1,st1+N1), saved2(st2,st2+N2);
    const auto savedCfg=g_FastLoaderConfig;
    g_FastLoaderConfig.NoFading=false; g_FastLoaderConfig.NoLoadScreen=false; g_FastLoaderConfig.NoLoadBar=false; g_FastLoaderConfig.ScreenChangeTime=5.f; g_FastLoaderConfig.NoDbgLogScreens=true;
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(bit_cast<uintptr_t>(&GetMillisecondTime),FakeMs), d1e(0x619410,FakeMsExe);
        EntryPatch d2(bit_cast<uintptr_t>(&srand),FakeSrand), d2e(0x821B11,FakeSrand);
        EntryPatch d3(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), d3e(0x821B1E,FakeRand);
        EntryPatch t1(bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot),AddTxdSlot), t1e(0x731C80,AddTxdSlot);
        EntryPatch t2(bit_cast<uintptr_t>(static_cast<bool(*)(int32,const char*)>(&CTxdStore::LoadTxd)),LoadTxd), t2e(0x7320B0,LoadTxd);
        EntryPatch t3(bit_cast<uintptr_t>(&CTxdStore::AddRef),TxdAddRef), t3e(0x731A00,TxdAddRef);
        EntryPatch t4(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),PushTxd), t4e(0x7316A0,PushTxd);
        EntryPatch t5(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),PopTxd), t5e(0x7316B0,PopTxd);
        EntryPatch t6(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),SetTxd), t6e(0x7319C0,SetTxd);
        EntryPatch t7(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FindTxd), t7e(0x731850,FindTxd);
        EntryPatch t8(bit_cast<uintptr_t>(&CTxdStore::RemoveTxd),RemoveTxd), t8e(0x731E90,RemoveTxd);
        EntryPatch t9(bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot),RemoveTxdSlot), t9e(0x731CD0,RemoveTxdSlot);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::SetDir),SetDir), f1e(0x5387D0,SetDir);
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const char*)>(&CSprite2d::SetTexture)),SetTexture), s1e(0x727270,SetTexture);
        EntryPatch s2(bit_cast<uintptr_t>(&CSprite2d::Delete),SpriteDelete), s2e(0x727240,SpriteDelete);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&,const CRGBA&)>(&CSprite2d::Draw)),SpriteDraw), s3e(0x728350,SpriteDraw);
        EntryPatch s4(bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),DrawRect), s4e(0x727B60,DrawRect);
        EntryPatch s5(bit_cast<uintptr_t>(&CSprite2d::InitPerFrame),InitPerFrame), s5e(0x727350,InitPerFrame);
        EntryPatch s6(bit_cast<uintptr_t>(&CSprite2d::DrawBarChart),BarChart), s6e(0x728640,BarChart);
        // camara falsa: RwCameraBeginUpdate/EndUpdate (funcion del EXE o macro de la DLL) saltan a [cam+0x18]/[cam+0x1C]
        alignas(16) static std::array<uint8,0x60> fakeCamera{}; *reinterpret_cast<void**>(fakeCamera.data()+0x18)=reinterpret_cast<void*>(&CamBegin); *reinterpret_cast<void**>(fakeCamera.data()+0x1C)=reinterpret_cast<void*>(&CamEnd);
        auto& sceneCamera=*reinterpret_cast<void**>(0xC1703C); void* const savedCamera=sceneCamera; sceneCamera=fakeCamera.data();
        EntryPatch c3(bit_cast<uintptr_t>(&RsCameraShowRaster),ShowRaster), c3e(0x619440,ShowRaster);
        EntryPatch c4(bit_cast<uintptr_t>(&DefinedState2d),DefState), c4e(0x734750,DefState);
        EntryPatch a1(bit_cast<uintptr_t>(&CAudioEngine::ServiceLoadingTune),LoadingTune), a1e(0x5078A0,LoadingTune);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x10ADu; trace=json::array(); seed=i; rngCalls=0; clockStep=0;
                    // estado aleatorio coherente
                    for(size_t k=0;k<N1;++k)st1[k]=uint8(next());
                    LS::m_bActive=next()%4!=0; LS::m_bWantToPause=next()%2; LS::m_bPaused=next()%2;
                    LS::m_bFading=next()%2; LS::m_bLegalScreen=next()%3==0; LS::m_bFadeInNextSplashFromBlack=next()%2; LS::m_bFadeOutCurrSplashToBlack=next()%2;
                    LS::m_FadeAlpha=uint8(next()); LS::m_bSignalDelete=next()%2; LS::m_bReadyToDelete=next()%3==0;
                    clockMs=next()%5==0 ? 0x80000000u+next()%1000000u : next()%50000000u;
                    const float nowS=float(clockMs)*0.001f;
                    LS::m_PauseTime=next()%3==0 ? 0.f : uni(0.f,30.f); LS::m_ClockTimeOnPause=nowS-uni(0.f,20.f);
                    LS::m_StartFadeTime=nowS-LS::m_PauseTime-uni(0.f,3.f);
                    LS::m_TimeStartedLoading=nowS-uni(0.f,40.f); LS::m_TimeBarAppeared=next()%3==0 ? 0.f : nowS-LS::m_PauseTime-uni(0.f,30.f);
                    LS::m_PercentLoaded=next()%5==0 ? (next()%2 ? 0.f : 100.f) : uni(-5.f,110.f);
                    LS::m_timeSinceLastScreen=nowS-uni(0.f,8.f);
                    for(int k=0;k<7;++k)LS::m_aSplashes[k]=next()%4==0 ? 0 : 0x5000+k;
                    std::strcpy(reinterpret_cast<char*>(LS::m_PopUpMessage),"popup");
                    const std::string nm=fn.name;
                    int32 curr=int32(next()%8)-1; if(nm=="DoPCScreenChange"||nm=="NewChunkLoaded"||nm=="LoadingScreen"||nm=="Update")curr=std::min(curr,5); // el original se saldria del array
                    LS::m_currDisplayedSplash=curr;
                    LS::m_numChunksLoaded=next()%4==0 ? 139 : int32(next()%140); LS::m_chunkBarAppeared=next()%3==0 ? -1 : int32(next()%(LS::m_numChunksLoaded+1));
                    cameraOk=next()%5!=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json ret=nullptr;
                    static const char* msgs[]{"Loading","CARGANDO MAPA","x",""};
                    if(nm=="Init")reinterpret_cast<void(__cdecl*)(bool,bool)>(fn.address)(next()%2,next()%2);
                    else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(next()%2);
                    else if(nm=="LoadSplashes")reinterpret_cast<void(__cdecl*)(bool,uint8)>(fn.address)(next()%3==0,uint8(next()%2));
                    else if(nm=="DisplayMessage")reinterpret_cast<void(__cdecl*)(const char*)>(fn.address)(msgs[next()%4]);
                    else if(nm=="SetLoadingBarMsg"){ const char* a=next()%4 ? msgs[next()%4] : nullptr; const char* b=next()%4 ? msgs[next()%4] : nullptr; reinterpret_cast<void(__cdecl*)(const char*,const char*)>(fn.address)(a,b); }
                    else if(nm=="GetClockTime")ret=F(reinterpret_cast<float(__cdecl*)(bool)>(fn.address)(next()%2));
                    else if(nm=="IsActive")ret=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
                    else if(nm=="DoPCScreenChange")reinterpret_cast<void(__cdecl*)(bool)>(fn.address)(next()%3==0);
                    else if(nm=="LoadingScreen"){ const char* a=next()%3 ? msgs[next()%4] : nullptr; reinterpret_cast<void(__cdecl*)(const char*,const char*,const char*)>(fn.address)(a,next()%2 ? msgs[next()%4] : nullptr,nullptr); }
                    else reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    return {{"ret",ret},{"trace",trace},{"switches",switches},{"state",ReversingProbe::Bytes(st1,N1)},{"state2",ReversingProbe::Bytes(st2,N2)}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CLoadingScreen::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CLoadingScreen::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",std::string("CLoadingScreen::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
        fpSet=savedSet; sceneCamera=savedCamera;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    std::memcpy(st1,saved1.data(),N1); std::memcpy(st2,saved2.data(),N2); g_FastLoaderConfig=savedCfg;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",std::memcmp(st1,saved1.data(),N1)==0}});
}
}
