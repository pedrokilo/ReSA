#pragma once
// Banco dentro del motor: App (7), CIniFile::LoadIniFile y CIplDefPool::New. Las funciones de App son
// orquestacion: se interceptan todas sus dependencias (camara, sprites, nubes, depuracion, eventos,
// registro de plugins, estado de render) y se compara la secuencia exacta de llamadas, argumentos
// (contenido de los temporales apuntados) y retorno. No se ejecuta render ni registro reales.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "app.h"
#include "app_input.h"
#include "app_game.h"
#include "platform.h"
#include "app_debug.h"
#include "Clouds.h"
#include "CarFXRenderer.h"
#include "Plugins/BreakablePlugin/BreakablePlugin.h"
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"
#include "IniFile.h"
#include "Pools/IplDefPool.h"
#include "../ProbeRuntime.hpp"

namespace AppLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,64> param{};
inline json Arg(uint32 v) {
    if(v&&v==uintptr_t(Scene.m_pRwCamera))return "camera";
    if(v>=uintptr_t(param.data())&&v<uintptr_t(param.data())+param.size())return "param+"+std::to_string(v-uintptr_t(param.data()));
    return v;
}

// ---- dependencias genericas (cdecl): ID -> nombre, numero de argumentos, retorno, punteros a leer
struct Dep { const char* name; uintptr_t exe; uintptr_t dll; int nargs; uint32 ret; std::array<uint8,5> deref; };
inline std::vector<Dep> deps;
inline json Args(int id,const uint32* a) {
    json r=json::array();
    for(int k=0;k<deps[id].nargs;++k){
        const auto n=deps[id].deref[k];
        if(n&&a[k]&&!(a[k]>=uintptr_t(param.data())&&a[k]<uintptr_t(param.data())+param.size()))r.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a[k]),n));
        else r.push_back(Arg(a[k]));
    }
    return r;
}
template<int ID> uint32 __cdecl Stub(uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) {
    const uint32 a[5]{a0,a1,a2,a3,a4}; trace.push_back({deps[ID].name,Args(ID,a)}); return deps[ID].ret;
}
inline void __fastcall MatrixUpdate(CMatrix* m,void*) { trace.push_back({"CMatrix::Update",m==&TheCamera.m_mViewMatrix ? json("TheCamera.m_mViewMatrix"):json(uintptr_t(m))}); }
inline RwBool __cdecl RenderState(RwRenderState s,void* v) { trace.push_back({"render_state",int(s),uintptr_t(v)}); return TRUE; }
// CIniFile
inline std::array<std::string,2> lines; inline int lineIdx{}; inline bool openOk{};
inline int32 __cdecl SetDir(const char* p) { trace.push_back({"SetDir",p ? p:""}); return 0; }
inline FILE* __cdecl OpenFile(const char* p,const char* m) { trace.push_back({"OpenFile",p ? p:"",m ? m:""}); return openOk ? reinterpret_cast<FILE*>(0x7E100000):nullptr; }
inline bool __cdecl ReadLine(FILE* f,char* s,int32 n) { trace.push_back({"ReadLine",uintptr_t(f),n,s==gString}); const auto& l=lines[std::min(lineIdx++,1)]; strncpy_s(s,size_t(n),l.c_str(),_TRUNCATE); return true; }
inline int32 __cdecl CloseFile(FILE* f) { trace.push_back({"CloseFile",uintptr_t(f)}); return 0; }

template<int... I> void RegisterStubs(std::integer_sequence<int,I...>,std::vector<std::unique_ptr<ReversingProbe::EntryPatch>>& out) {
    ([&]{ if(I<int(deps.size())){ const auto& d=deps[I];
        if(d.exe)out.push_back(std::make_unique<ReversingProbe::EntryPatch>(d.exe,&Stub<I>));
        if(d.dll&&d.dll!=d.exe)out.push_back(std::make_unique<ReversingProbe::EntryPatch>(d.dll,&Stub<I>)); } }(),...);
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,9> fns{{
    {"App","AppEventHandler",0x53EC10,512},{"App","DefinedState",0x734650,64},{"App","DefinedState2d",0x734750,64},
    {"App","DoRWStuffEndOfFrame",0x53D840,64},{"App","DoRWStuffStartOfFrame",0x53D690,512},{"App","DoRWStuffStartOfFrame_Horizon",0x53D7A0,512},
    {"App","PluginAttach",0x53D870,256},{"CIniFile","LoadIniFile",0x56D070,512},{"CIplDefPool","New",0x4059B0,512},
}};
template<class F> uintptr_t Addr(F f) { return std::bit_cast<uintptr_t>(f); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"app-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"app-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("app");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,9> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    deps={
        {"CDraw::CalculateAspectRatio",0x6FF420,Addr(&CDraw::CalculateAspectRatio),0,0,{}},
        {"CFont::SetAlphaFade",0x719500,Addr(&CFont::SetAlphaFade),1,0,{}},
        {"CameraSize",0x72FC70,Addr(&CameraSize),4,0,{0,16,0,0,0}},
        {"CVisibilityPlugins::SetRenderWareCamera",0x7328C0,Addr(&CVisibilityPlugins::SetRenderWareCamera),1,0,{}},
        {"RwCameraClear",0x7EE340,Addr(&RwCameraClear),3,1,{0,4,0,0,0}},
        {"RsCameraBeginUpdate",0x619450,Addr(&RsCameraBeginUpdate),1,1,{}},
        {"CSprite2d::InitPerFrame",0x727350,Addr(&CSprite2d::InitPerFrame),0,0,{}},
        {"CSprite2d::DrawRect4",0x727C10,Addr(static_cast<void(*)(const CRect&,const CRGBA&,const CRGBA&,const CRGBA&,const CRGBA&)>(&CSprite2d::DrawRect)),5,0,{16,4,4,4,4}},
        {"CClouds::RenderSkyPolys",0x714650,Addr(&CClouds::RenderSkyPolys),0,0,{}},
        {"CDebug::DebugDisplayTextBuffer",0x532260,Addr(&CDebug::DebugDisplayTextBuffer),0,0,{}},
        {"FlushObrsPrintfs",0x734640,Addr(&FlushObrsPrintfs),0,0,{}},
        {"RwCameraEndUpdate",0x7EE180,Addr(&RwCameraEndUpdate),1,0,{}},
        {"RsCameraShowRaster",0x619440,Addr(&RsCameraShowRaster),1,0,{}},
        {"CGame::InitialiseOnceBeforeRW",0x53BB50,Addr(&CGame::InitialiseOnceBeforeRW),0,1,{}},
        {"RsInitialize",0x619600,Addr(&RsInitialize),0,1,{}},
        {"RwInitialize",0x5BF390,Addr(&RwInitialize),1,1,{}},
        {"RwTerminate",0x53D910,Addr(&RwTerminate),0,0,{}},
        {"CGame::FinalShutdown",0x53BC30,Addr(&CGame::FinalShutdown),0,0,{}},
        {"AttachInputDevices",0x744A20,Addr(&AttachInputDevices),0,1,{}},
        {"Idle",0x53E920,Addr(&Idle),1,0,{}},
        {"FrontendIdle",0x53E770,Addr(&FrontendIdle),0,0,{}},
        // 17 registros de plugins (en el orden del original, 0x53D870)
        {"RpWorldPluginAttach",0x74FDA0,Addr(&RpWorldPluginAttach),0,1,{}},{"RpSkinPluginAttach",0x7C6820,Addr(&RpSkinPluginAttach),0,1,{}},
        {"RtAnimInitialize",0x7CCCA0,Addr(&RtAnimInitialize),0,1,{}},{"RpHAnimPluginAttach",0x7C4600,Addr(&RpHAnimPluginAttach),0,1,{}},
        {"NodeNamePluginAttach",0x72FAB0,Addr(&NodeNamePluginAttach),0,1,{}},{"CVisibilityPlugins::PluginAttach",0x732E30,Addr(&CVisibilityPlugins::PluginAttach),0,1,{}},
        {"RpAnimBlendPluginAttach",0x4D6150,Addr(&RpAnimBlendPluginAttach),0,1,{}},{"CTxdStore::PluginAttach",0x731650,Addr(&CTxdStore::PluginAttach),0,1,{}},
        {"RpMatFXPluginAttach",0x810AA0,Addr(&RpMatFXPluginAttach),0,1,{}},{"RpUVAnimPluginAttach",0x7CB940,Addr(&RpUVAnimPluginAttach),0,1,{}},
        {"CCustomBuildingRenderer::PluginAttach",0x5D7EF0,Addr(&CCustomBuildingRenderer::PluginAttach),0,1,{}},{"CCarFXRenderer::RegisterPlugins",0x5D5B00,Addr(&CCarFXRenderer::RegisterPlugins),0,1,{}},
        {"RpAnisotPluginAttach",0x748F70,Addr(&RpAnisotPluginAttach),0,1,{}},{"BreakablePluginAttach",0x59D100,Addr(&BreakablePluginAttach),0,1,{}},
        {"CCollisionPlugin::PluginAttach",0x41B310,Addr(&CCollisionPlugin::PluginAttach),0,1,{}},{"C2dEffect::PluginAttach",0x6FA970,Addr(&C2dEffect::PluginAttach),0,1,{}},
        {"PipelinePluginAttach",0x72FBD0,Addr(&PipelinePluginAttach),0,1,{}},
    };
    constexpr int FIRST_PLUGIN=21;
    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;
    const auto savedPed=CIniFile::PedNumberMultiplier, savedCar=CIniFile::CarNumberMultiplier;
    const auto savedMaxPeds=CPopulation::MaxNumberOfPedsInUse, savedMaxCars=CCarCtrl::MaxNumberOfCarsInUse;
    const auto savedString=ReversingProbe::Bytes(gString,sizeof(gString));
    std::array<bool,9> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        RegisterStubs(std::make_integer_sequence<int,40>{},patches);
        ReversingProbe::EntryPatch mu(Addr(&CMatrix::Update),MatrixUpdate), mue(0x59BB60,MatrixUpdate);
        ReversingProbe::EntryPatch sd(Addr(&CFileMgr::SetDir),SetDir), sde(0x5387D0,SetDir), of(Addr(&CFileMgr::OpenFile),OpenFile), ofe(0x538900,OpenFile);
        ReversingProbe::EntryPatch rl(Addr(&CFileMgr::ReadLine),ReadLine), rle(0x5389B0,ReadLine), cf(Addr(&CFileMgr::CloseFile),CloseFile), cfe(0x5389D0,CloseFile);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA99A99u; trace=json::array();
                    for(auto& b:param)b=uint8(next());
                    deps[5].ret=next()%4!=0;                                                   // RsCameraBeginUpdate
                    for(int k=13;k<FIRST_PLUGIN;++k)deps[k].ret=next()%3!=0;                    // retornos de eventos
                    const int failAt=FIRST_PLUGIN+int(next()%20);                                // registro que falla (o ninguno)
                    for(int k=FIRST_PLUGIN;k<int(deps.size());++k)deps[k].ret=k!=failAt;
                    std::array<float,4> v{}; for(auto& x:v){ const int m=int(next()%6); x = m==0 ? 0.5f : m==1 ? 3.f : float(int(next()%600)-100)/100.f; }
                    char buf[2][64]; sprintf_s(buf[0],"%.3f",v[0]); sprintf_s(buf[1],next()%2 ? "%.2f\n":" %g",v[1]); lines={buf[0],buf[1]}; lineIdx=0; openOk=next()%4!=0;
                    CIniFile::PedNumberMultiplier=v[2]; CIniFile::CarNumberMultiplier=v[3]; std::memset(gString,0,sizeof(gString));
                    CPopulation::MaxNumberOfPedsInUse=next()%50; CCarCtrl::MaxNumberOfCarsInUse=next()%50;
                    fpState=RenderState;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json ret=nullptr, out=nullptr; const auto a=fn.address;
                    const auto c7=[&]{ return int16(int(next()%300)-20); };
                    switch(f) {
                    case 0: { static const int evs[]{rsCAMERASIZE,rsINPUTDEVICEATTACH,rsPLUGINATTACH,rsRWINITIALIZE,rsRWTERMINATE,rsINITIALIZE,rsTERMINATE,rsIDLE,rsFRONTENDIDLE,rsACTIVATE};
                              const int e=next()%3 ? evs[next()%10] : int(next()%60); void* p=next()%4 ? param.data():nullptr;
                              ret=int(reinterpret_cast<RsEventStatus(__cdecl*)(RsEvent,void*)>(a)(RsEvent(e),p)); out=e; break; }
                    case 1: case 2: case 3: reinterpret_cast<void(__cdecl*)()>(a)(); break;
                    case 4: case 5: { int16 c[7]; for(auto& x:c)x=c7(); if(next()%4==0)c[6]=0;
                              ret=reinterpret_cast<bool(__cdecl*)(int16,int16,int16,int16,int16,int16,int16)>(a)(c[0],c[1],c[2],c[3],c[4],c[5],c[6]); break; }
                    case 6: ret=reinterpret_cast<bool(__cdecl*)()>(a)(); break;
                    case 7: reinterpret_cast<void(__cdecl*)()>(a)();
                            out={std::bit_cast<uint32>(float(CIniFile::PedNumberMultiplier)),std::bit_cast<uint32>(float(CIniFile::CarNumberMultiplier)),uint32(CPopulation::MaxNumberOfPedsInUse),uint32(CCarCtrl::MaxNumberOfCarsInUse),std::string(gString)}; break;
                    case 8: { constexpr int N=16; alignas(IplDef) static std::array<uint8,sizeof(IplDef)*N> storage; static std::array<uint8,N> states;
                              for(auto& b:storage)b=uint8(next()); for(int k=0;k<N;++k)states[k]=uint8((next()%3==0 ? 0x80:0)|(next()%0x7F));
                              const int size=1+int(next()%N); CIplDefPool pool(size,storage.data(),states.data());
                              auto* raw=reinterpret_cast<uint8*>(&pool); *reinterpret_cast<int32*>(raw+12)=int32(next()%size)-1; // m_nFirstFree / ultimo libre
                              const auto before=ReversingProbe::Bytes(raw,sizeof(pool));
                              void* r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(&pool);
                              ret=r ? json(int((reinterpret_cast<uint8*>(r)-storage.data())/int(sizeof(IplDef)))) : json(nullptr);
                              out={ReversingProbe::Bytes(states.data(),size),ReversingProbe::Bytes(raw+8,8),before==ReversingProbe::Bytes(raw,sizeof(pool)),r ? json(ReversingProbe::Bytes(r,sizeof(IplDef))):json(nullptr)};
                              break; }
                    }
                    fpState=savedFp;
                    return {{"ret",ret},{"out",out},{"trace",trace},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    fpState=savedFp; CIniFile::PedNumberMultiplier=savedPed; CIniFile::CarNumberMultiplier=savedCar;
    CPopulation::MaxNumberOfPedsInUse=savedMaxPeds; CCarCtrl::MaxNumberOfCarsInUse=savedMaxCars; std::memcpy(gString,savedString.data(),savedString.size());
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",fpState==savedFp}});
}
}
