#pragma once
// Banco dentro del motor: CTimeCycle (lista de 20, 20 funciones). Original (hooks del banco desactivados) frente a
// candidato con fixtures deterministas. Se simulan y trazan: CFileMgr::SetDir/OpenFile/CloseFile y
// CFileLoader::LoadLine (timecyc.dat en memoria, nunca se leen ficheros reales), CPostEffects::IsVisionFXActive y
// CCullZones::PlayerNoRain/CamNoRain. Las tablas de colores, las cajas, los colores actuales, el estado del color extra,
// los vectores al sol, las sombras, el gris bajo el horizonte y demás globales de CTimeCycle se rellenan por caso;
// también CWeather (climas, interpolación, bajo el agua, túnel, lluvia forzada), CClock, el paso de tiempo,
// CCoronas::LightsMult, TunnelWeather, gfLaRiotsLightMult, CCutsceneMgr::ms_running, las visiones nocturna/infrarroja,
// el brillo del menú y la posición y el eje z frontal de la cámara. Todo se guarda y se restaura en cada ejecución
// y al final. CColourSet, CShadows::CalcPedShadowValues y CVector::Normalise son reales (los mismos en ambas pasadas).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TimeCycle.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "PostEffects.h"
#include "CullZones.h"
#include "Coronas.h"
#include "CutsceneMgr.h"
#include "Weather.h"
#include "Clock.h"
#include "MenuManager.h"
#include "../ProbeRuntime.hpp"

namespace Time20Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- fuente de bits para los stubs ---
inline uint32 mask{}, maskCalls{};
inline bool Bit() { return (mask>>(maskCalls++%32))&1; }

// --- timecyc.dat en memoria ---
constexpr uintptr_t FAKE_STREAM=0x7E57;
inline std::vector<std::string> lines; inline size_t lineIdx{};
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",std::string(p ? p : "<null>")}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"open",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(FAKE_STREAM); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ if(uintptr_t(f)!=FAKE_STREAM)trace.push_back({"bad_stream",uintptr_t(f)}); if(lineIdx>=lines.size()){trace.push_back({"eof"});return nullptr;} return lines[lineIdx++].data(); }

// --- visión y lluvia ---
inline bool __cdecl FakeVision(){ const bool r=Bit(); trace.push_back({"vision",r}); return r; }
inline bool __cdecl FakePlayerNoRain(){ const bool r=Bit(); trace.push_back({"player_no_rain",r}); return r; }
inline bool __cdecl FakeCamNoRain(){ const bool r=Bit(); trace.push_back({"cam_no_rain",r}); return r; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"CTimeCycle","Initialise",0x5BBAC0,0,-1,64},{"CTimeCycle","InitForRestart",0x5601F0,0,-1,64},{"CTimeCycle","Shutdown",0x5601E0,0,-1,64},
    {"CTimeCycle","Update",0x561760,0,-1,1024},{"CTimeCycle","StartExtraColour",0x55FEC0,0,-1,512},{"CTimeCycle","StopExtraColour",0x55FF20,0,-1,256},
    {"CTimeCycle","AddOne",0x55FF40,0,-1,1024},{"CTimeCycle","CalcColoursForPoint",0x5603D0,0,-1,1024},{"CTimeCycle","FindFarClipForCoors",0x5616E0,0,-1,1024},
    {"CTimeCycle","FindTimeCycleBox",0x55FFD0,0,-1,1024},{"CTimeCycle","SetConstantParametersForPostFX",0x560210,0,-1,256},
    {"CTimeCycle","GetAmbientRed",0x560330,0,-1,128},{"CTimeCycle","GetAmbientGreen",0x560340,0,-1,128},{"CTimeCycle","GetAmbientBlue",0x560350,0,-1,128},
    {"CTimeCycle","GetAmbientRed_Obj",0x560360,0,-1,128},{"CTimeCycle","GetAmbientGreen_Obj",0x560370,0,-1,128},{"CTimeCycle","GetAmbientBlue_Obj",0x560380,0,-1,128},
    {"CTimeCycle","GetAmbientRed_BeforeBrightness",0x560390,0,-1,128},{"CTimeCycle","GetAmbientGreen_BeforeBrightness",0x5603A0,0,-1,128},
    {"CTimeCycle","GetAmbientBlue_BeforeBrightness",0x5603B0,0,-1,128},
}};

// --- estado global guardado / comparado ---
struct Range { uintptr_t at; size_t n; const char* tag; };
inline std::vector<Range> ranges;
inline void InitRanges() {
    ranges={
        {0xB79E30,0xB7C488-0xB79E30,"timecycle_tables"},  // brillo añadido .. tablas, m_NumBoxes, m_bExtraColourOn
        {0xB7C4A0,0xB7C870-0xB7C4A0,"timecycle_boxes"},   // m_CurrentColours y m_aBoxes
        {0xB7CA50,0xB7CB28-0xB7CA50,"timecycle_sun"},     // m_VectorToSun, m_BelowHorizonGrey, m_vecDirnLightToSun, climas guardados
        {0xC812AC,0xC81338-0xC812AC,"weather"},
        {0xB70150,5,"clock"},{0xB7CB5C,4,"timestep"},{0x8D4B5C,4,"lights_mult"},{0x8CDEE0,4,"tunnel_weather"},{0x8CD060,4,"riots_mult"},
        {0xB5F851,1,"cutscene_running"},{0xC402B8,2,"vision_flags"},{0xBA6784,4,"brightness"},{0xB6F9B4,4,"camera_forward_z"},
        {reinterpret_cast<uintptr_t>(&TheCamera.GetPosition()),12,"camera_pos"},
    };
}
inline std::vector<uint8> Dump() { std::vector<uint8> d; for(auto& r:ranges){ auto* p=reinterpret_cast<const uint8*>(r.at); d.insert(d.end(),p,p+r.n); } return d; }
inline void Restore(const std::vector<uint8>& d) { size_t o=0; for(auto& r:ranges){ std::memcpy(reinterpret_cast<void*>(r.at),d.data()+o,r.n); o+=r.n; } }
inline json Diffs(const std::vector<uint8>& a,const std::vector<uint8>& b) {
    json out=json::array(); size_t o=0;
    for(auto& r:ranges){ for(size_t k=0;k<r.n&&out.size()<24;++k)if(a[o+k]!=b[o+k])out.push_back({r.tag,r.at+k,a[o+k],b[o+k]}); o+=r.n; }
    return out;
}

constexpr uintptr_t TABLES_BEGIN=0xB79FD8, TABLES_END=0xB7C480, BOXES=0xB7C550;
inline json BoxName(const void* p) {
    if(!p)return nullptr; const auto u=uintptr_t(p);
    if(u>=BOXES&&u<BOXES+20*0x28)return json::array({"box",(u-BOXES)/0x28,(u-BOXES)%0x28});
    return json::array({"raw",u});
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"time20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"time20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("time20");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"time20-only.txt"); if(f)std::getline(f,only); }
    if(reinterpret_cast<uintptr_t>(&TheCamera.m_mCameraMatrix.GetForward().z)!=0xB6F9B4||reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_PrefsBrightness)!=0xBA6784){
        Write({{"event","error"},{"reason","layout"},{"forward_z",reinterpret_cast<uintptr_t>(&TheCamera.m_mCameraMatrix.GetForward().z)},{"brightness",reinterpret_cast<uintptr_t>(&FrontEndMenuManager.m_PrefsBrightness)}}); return; }
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    InitRanges(); const auto saved=Dump();
    Write({{"event","player_ready"},{"fp_control",_controlfp(0,0)},{"camera_pos_address",ranges.back().at}});
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), s1e(0x5387D0,FakeSetDir);
        EntryPatch s2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), s2e(0x538900,FakeOpenFile);
        EntryPatch s3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), s3e(0x5389D0,FakeCloseFile);
        EntryPatch s4(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),FakeLoadLine), s4e(0x536F80,FakeLoadLine);
        EntryPatch v1(bit_cast<uintptr_t>(&CPostEffects::IsVisionFXActive),FakeVision), v1e(0x7034F0,FakeVision);
        EntryPatch c1(bit_cast<uintptr_t>(&CCullZones::PlayerNoRain),FakePlayerNoRain), c1e(0x72DDC0,FakePlayerNoRain);
        EntryPatch c2(bit_cast<uintptr_t>(&CCullZones::CamNoRain),FakeCamNoRain), c2e(0x72DDB0,FakeCamNoRain);
        std::vector<uint8> dumps[2];

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                // Rellena todo el estado de CTimeCycle y sus dependencias (sin NaN).
                auto fixture=[&](const CVector& point,bool camAtPoint=false) {
                    auto* tables=reinterpret_cast<uint8*>(TABLES_BEGIN); for(uintptr_t k=0;k<TABLES_END-TABLES_BEGIN;++k)tables[k]=uint8(next());
                    auto* fc=reinterpret_cast<int16*>(0xB7B1D0); auto* fs=reinterpret_cast<int16*>(0xB7B060);
                    for(int k=0;k<NUM_HOURS*NUM_WEATHERS;++k){ fc[k]=next()%4==0 ? int16(-1) : int16(100+next()%2900); fs[k]=next()%4==0 ? int16(-1) : int16(int(next()%1100)-100); }
                    for(int h=0;h<NUM_HOURS;++h)for(int w:{WEATHER_EXTRACOLOURS_1,WEATHER_EXTRACOLOURS_2})if(next()%3==0){ /* cielo nulo: ignoreSky */
                        CTimeCycle::m_nSkyTopRed[h][w]=0; CTimeCycle::m_nSkyTopGreen[h][w]=0; CTimeCycle::m_nSkyTopBlue[h][w]=0; }
                    auto& cc=CTimeCycle::m_CurrentColours; auto* ccb=reinterpret_cast<uint8*>(&cc); for(size_t k=0;k<sizeof(CColourSet);++k)ccb[k]=uint8(next());
                    for(size_t off:{0x0u,0x4u,0x8u,0xCu,0x10u,0x14u,0x18u,0x1Cu,0x20u,0x3Cu,0x40u,0x44u,0x50u,0x54u,0x58u,0x68u,0x6Cu,0x70u,0x74u,0x78u,0x7Cu,0x80u,0x84u,0x88u,0x8Cu,0x90u,0x94u,0x98u,0xA4u,0xA8u})
                        *reinterpret_cast<float*>(ccb+off)=uni(0.f,400.f);
                    auto* boxes=reinterpret_cast<uint8*>(BOXES); for(int k=0;k<20*0x28;++k)boxes[k]=uint8(next());
                    for(int b=0;b<20;++b){ auto& box=CTimeCycle::m_aBoxes[b];
                        const CVector mn=point+CVector{uni(-300.f,300.f),uni(-300.f,300.f),uni(-100.f,100.f)};
                        box.Box.m_vecMin=mn; box.Box.m_vecMax=mn+CVector{next()%10==0 ? 0.f : uni(0.f,300.f),uni(0.f,300.f),uni(0.f,100.f)};
                        box.FarClip=next()%3==0 ? int16(0) : int16(100+next()%2000); box.LodDistMult=next()%4==0 ? uint8(32) : uint8(next());
                        box.ExtraColor=int32(next()%17)-1; box.Strength=uni(0.f,1.5f); box.Falloff=next()%20==0 ? 0.f : uni(0.f,150.f); }
                    CTimeCycle::m_NumBoxes=next()%21;
                    for(uintptr_t a=0xB79E50;a<0xB79FD0;a+=4)*reinterpret_cast<float*>(a)=uni(-2.f,2.f); // sombras
                    for(auto& v:CTimeCycle::m_VectorToSun)v=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                    CTimeCycle::m_CurrentStoredValue=next()%16; CTimeCycle::m_FogReduction=next()%3==0 ? 0 : int32(next()%70);
                    CTimeCycle::m_bExtraColourOn=next()%2; CTimeCycle::m_ExtraColourInter=next()%6==0 ? float(next()%2) : uni(-0.3f,1.3f);
                    CTimeCycle::m_ExtraColour=int32(next()%8); CTimeCycle::m_ExtraColourWeatherType=int32(WEATHER_EXTRACOLOURS_1+next()%2);
                    CTimeCycle::m_BelowHorizonGrey={uint8(next()),uint8(next()),uint8(next()),uint8(next())};
                    CTimeCycle::m_BrightnessAddedToAmbientRed=uni(-1.f,1.f); CTimeCycle::m_BrightnessAddedToAmbientGreen=uni(-1.f,1.f); CTimeCycle::m_BrightnessAddedToAmbientBlue=uni(-1.f,1.f);
                    CTimeCycle::m_nCurrentOldWeatherType=int16(next()); CTimeCycle::m_nCurrentNewWeatherType=int16(next());
                    auto weather=[&]{ return eWeatherType(next()%3==0 ? WEATHER_EXTRASUNNY_SMOG_LA+next()%2 : next()%NUM_WEATHERS); };
                    CWeather::OldWeatherType=weather(); CWeather::NewWeatherType=weather(); CWeather::InterpolationValue=next()%6==0 ? float(next()%2) : uni(0.f,1.f);
                    CWeather::UnderWaterness=next()%2 ? 0.f : uni(0.f,1.f); CWeather::InTunnelness=next()%2 ? 0.f : uni(0.f,1.f); CWeather::bScriptsForceRain=next()%4==0;
                    CClock::ms_nGameClockHours=uint8(next()%10==0 ? 24+next()%2 : next()%24); CClock::ms_nGameClockMinutes=uint8(next()%60);
                    CClock::ms_nGameClockSeconds=uint16(next()%4==0 ? next()%1000 : next()%60);
                    CTimer::ms_fTimeStep=uni(0.f,3.f); CCoronas::LightsMult=uni(0.25f,2.f); StaticRef<int32>(0x8CDEE0)=int32(next()%16); StaticRef<float>(0x8CD060)=uni(0.5f,1.5f);
                    CCutsceneMgr::ms_running=int8(next()%5==0); CPostEffects::m_bNightVision=next()%2; CPostEffects::m_bInfraredVision=next()%2;
                    FrontEndMenuManager.m_PrefsBrightness=int32(next()%8==0 ? 256 : next()%540);
                    auto& cam=TheCamera.GetPosition(); static const float zs[]{-50.f,20.f,200.f,500.f,220.f};
                    cam=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%6==0 ? zs[next()%5] : uni(-50.f,800.f)}; if(camAtPoint)cam=point;
                    TheCamera.m_mCameraMatrix.GetForward().z=next()%8==0 ? -0.9f : uni(-1.f,1.f);
                };
                auto randPoint=[&]{ const float lim=next()%3==0 ? 5500.f : 2900.f; return CVector{uni(-lim,lim),uni(-lim,lim),uni(-50.f,600.f)}; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7120u; if(!stt)stt=1; trace=json::array(); mask=next(); maskCalls=0;
                    Restore(saved);
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate; const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(nm=="Initialise"){
                        fixture(randPoint()); lines.clear(); lineIdx=0;
                        auto num=[&](float lo,float hi){ const float v=uni(lo,hi); const int whole=int(std::floor(std::abs(v))); static const char* fr[]{"0","25","5","75"};
                            return std::string(v<0 ? "-" : "")+std::to_string(whole)+"."+fr[next()%4]; };
                        auto inum=[&]{ return std::to_string(next()%5==0 ? int(next()%700)-200 : int(next()%256)); };
                        for(int k=0;k<NUM_HOURS*NUM_WEATHERS;++k){
                            while(next()%4==0)lines.push_back(next()%2 ? "// comentario de prueba" : "");
                            std::string l;
                            for(int p=0;p<52;++p){
                                std::string v;
                                if(p<21||(p>=24&&p<=26)||(p>=30&&p<=35)||p==49||p==50)v=inum();
                                else if(p<=23)v=num(next()%10==0 ? -2.f : 0.f,12.f);
                                else if(p==27)v=next()%5==0 ? "-1.0" : num(-50.f,4000.f);
                                else if(p==28)v=num(-100.f,1000.f);
                                else if(p==29)v=num(next()%10==0 ? -3.f : 0.f,10.f);
                                else if(p==40||p==44)v=num(next()%10==0 ? -20.f : 0.f,150.f);
                                else if(p==51)v=num(next()%10==0 ? -1.f : 0.f,3.f);
                                else v=num(next()%10==0 ? -50.f : 0.f,300.f);
                                l+=(p?" ":"")+v;
                            }
                            lines.push_back(l);
                        }
                        lines.push_back("// fin");
                        reinterpret_cast<void(__cdecl*)(bool)>(a)(false);
                        out["lines_read"]=lineIdx;
                    }
                    else if(nm=="InitForRestart"||nm=="Shutdown"||nm=="SetConstantParametersForPostFX"){ fixture(randPoint()); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="Update"){ CVector p=randPoint(); static const float uz[]{-50.f,20.f,200.f,500.f,220.f,600.f}; if(next()%4==0)p.z=uz[next()%6];
                        fixture(p,true); /* cajas alrededor de la cámara */ reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="StartExtraColour"){ fixture(randPoint()); const int32 colour=next()%3 ? int32(next()%16) : int32(next()); const bool flag=next()%2;
                        reinterpret_cast<void(__cdecl*)(int32,bool)>(a)(colour,flag); }
                    else if(nm=="StopExtraColour"){ fixture(randPoint()); reinterpret_cast<void(__cdecl*)(bool)>(a)(next()%2); }
                    else if(nm=="AddOne"){ fixture(randPoint()); CTimeCycle::m_NumBoxes=next()%20;
                        CBox box; box.m_vecMin=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-100.f,300.f)}; box.m_vecMax=box.m_vecMin+CVector{uni(0.f,500.f),uni(0.f,500.f),uni(0.f,200.f)};
                        static const float lods[]{4.f,0.5f,1.f,0.f,4.0001f,-1.f};
                        const float lod=next()%4==0 ? lods[next()%6] : uni(-2.f,8.f);
                        reinterpret_cast<void(__cdecl*)(CBox&,int16,int32,float,float,float)>(a)(box,int16(next()),int32(next()%3 ? next()%17 : next())-1,uni(-50.f,150.f),uni(0.f,200.f),lod);
                        out["box"]=ReversingProbe::Bytes(&box,sizeof(box)); }
                    else if(nm=="CalcColoursForPoint"){ const CVector p=randPoint(); fixture(p);
                        alignas(16) static std::array<uint8,sizeof(CColourSet)> buf{}; buf.fill(0x5A);
                        auto* target=next()%8==0 ? &CTimeCycle::m_CurrentColours : reinterpret_cast<CColourSet*>(buf.data());
                        reinterpret_cast<void(__cdecl*)(CVector,CColourSet*)>(a)(p,target); out["set"]=ReversingProbe::Bytes(buf.data(),buf.size()); }
                    else if(nm=="FindFarClipForCoors"){ const CVector p=next()%3==0 ? TheCamera.GetPosition() : randPoint(); fixture(p);
                        ret=F(reinterpret_cast<float(__cdecl*)(CVector)>(a)(p)); }
                    else if(nm=="FindTimeCycleBox"){ CVector p=randPoint(); fixture(p);
                        if(next()%4==0){ /* punto en un borde de caja */ const auto& bx=CTimeCycle::m_aBoxes[next()%20].Box; p.x=next()%2 ? bx.m_vecMin.x : bx.m_vecMax.x; if(next()%2)p.z=next()%2 ? bx.m_vecMin.z : bx.m_vecMax.z; }
                        CTimeCycleBox* outBox=reinterpret_cast<CTimeCycleBox*>(uintptr_t(1)); float interp=123.f;
                        const bool isLod=next()%3==0, isFar=next()%3==0; CTimeCycleBox* ignored=next()%3==0 ? &CTimeCycle::m_aBoxes[next()%20] : nullptr;
                        reinterpret_cast<void(__cdecl*)(CVector,CTimeCycleBox**,float*,bool,bool,CTimeCycleBox*)>(a)(p,&outBox,&interp,isLod,isFar,ignored);
                        ret=json::array({BoxName(outBox),F(interp)}); }
                    else { fixture(randPoint()); ret=F(reinterpret_cast<float(__cdecl*)()>(a)()); }

                    auto d=Dump(); out["state"]=Fnv(d.data(),d.size()); dumps[candidate]=std::move(d);
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_diffs",Diffs(dumps[0],dumps[1])},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        Restore(saved);
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Restore(saved); lines.clear(); lines.shrink_to_fit();
    const bool restored=Dump()==saved;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
