#pragma once
// Banco dentro del motor: zonas de culling (CCullZones, 21 funciones) y zonas del mapa (CTheZones, 21 funciones).
// Original (hooks de las dos clases desactivados) frente a candidato con fixtures deterministas sobre las tablas
// reales del EXE, que se guardan antes de empezar y se restauran antes y despues de cada ejecucion y al final.
// Se simulan y trazan: FindPlayerCoors, FindPlayerPed, CPed::IsAlive, CPlayerPed::SetWantedLevelNoDrop,
// CCamera::GetGameCamPosition, CGangWars::CanPlayerStartAGangWarHere y los buffers de trabajo de guardado/carga
// (nunca se toca una partida guardada real).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CullZones.h"
#include "TheZones.h"
#include "GangWars.h"
#include "GenericGameStorage.h"
#include "../ProbeRuntime.hpp"

namespace Zones21Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// regiones de estado global de cada clase
constexpr uintptr_t CULL_BEGIN=0xC815C0, CULL_END=0xC87AD0;   // espejos, tuneles, atributos, flags, contadores
constexpr uintptr_t ZONES_BEGIN=0xBA1900, ZONES_END=0xBA6728; // mapa, infos, revelado, visitadas, navegacion, nivel, Update
constexpr uintptr_t MIRRORS=0xC815C0, TUNNELS=0xC81C80, ATTRS=0xC81F50;
constexpr uintptr_t MAPZ=0xBA1908, INFOS=0xBA1DF0, NAVZ=0xBA3798;
inline json Name(const void* p) {
    const auto u=uintptr_t(p);
    if(!p)return nullptr;
    if(u>=ZONES_BEGIN&&u<ZONES_END)return json::array({"zones",u-ZONES_BEGIN});
    if(u>=CULL_BEGIN&&u<CULL_END)return json::array({"cull",u-CULL_BEGIN});
    return "other";
}

// --- stubs ---
inline CVector playerCoors{}, camPos{};
inline uint8 fakePed[0x800]{};
inline uint32 aliveMask{}, aliveCalls{}, gangMask{}, gangCalls{}, loadState{};
inline uint16 loadNav{}, loadInfos{}, loadMap{};
inline CVector* __cdecl FakeCoors(CVector* out,int32 id){ trace.push_back({"player_coors",id}); *out=playerCoors; return out; }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(fakePed); }
inline bool __fastcall FakeIsAlive(void* p,void*){ trace.push_back({"is_alive",p==fakePed}); return (aliveMask>>(aliveCalls++%32))&1; }
inline void __fastcall FakeSetWanted(void* p,void*,int32 level){ trace.push_back({"set_wanted_no_drop",p==fakePed,level}); }
inline CVector* __fastcall FakeCamPos(void* p,void*){ trace.push_back({"game_cam_pos",p==&TheCamera}); return &camPos; }
inline bool __cdecl FakeCanStartGangWar(CZoneInfo* z){ trace.push_back({"can_start_gang_war",Name(z),Fnv(z,0x11)}); return (gangMask>>(gangCalls++%32))&1; }
inline bool __cdecl FakeSaveWB(void* p,int32 n){ trace.push_back({"save_wb",Name(p),n,Fnv(p,size_t(n))}); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n){
    auto* b=static_cast<uint8*>(p);
    if(p==&CTheZones::TotalNumberOfNavigationZones)*reinterpret_cast<uint16*>(b)=loadNav;
    else if(p==&CTheZones::TotalNumberOfZoneInfos)*reinterpret_cast<uint16*>(b)=loadInfos;
    else if(p==&CTheZones::TotalNumberOfMapZones)*reinterpret_cast<uint16*>(b)=loadMap;
    else for(int32 k=0;k<n;++k){ loadState=loadState*1103515245u+12345u; b[k]=uint8(loadState>>16); }
    trace.push_back({"load_wb",Name(p),n}); return true; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,42> fns{{
    {"CCullZones","Init",0x72D6B0,0,-1,64},{"CCullZones","AddCullZone",0x72DF70,0,-1,512},{"CCullZones","AddTunnelAttributeZone",0x72DB50,0,-1,512},
    {"CCullZones","AddMirrorAttributeZone",0x72DC10,0,-1,512},{"CCullZones","CamCloseInForPlayer",0x72DD20,0,-1,64},{"CCullZones","CamStairsForPlayer",0x72DD30,0,-1,64},
    {"CCullZones","Cam1stPersonForPlayer",0x72DD40,0,-1,64},{"CCullZones","NoPolice",0x72DD50,0,-1,64},{"CCullZones","PoliceAbandonCars",0x72DD60,0,-1,64},
    {"CCullZones","InRoomForAudio",0x72DD70,0,-1,64},{"CCullZones","FewerCars",0x72DD80,0,-1,64},{"CCullZones","FewerPeds",0x72DD90,0,-1,64},
    {"CCullZones","DoINeedToLoadCollision",0x72DDA0,0,-1,64},{"CCullZones","CamNoRain",0x72DDB0,0,-1,64},{"CCullZones","PlayerNoRain",0x72DDC0,0,-1,64},
    {"CCullZones","DoExtraAirResistanceForPlayer",0x72DDD0,0,-1,64},{"CCullZones","FindAttributesForCoors",0x72D970,0,-1,1024},
    {"CCullZones","FindTunnelAttributesForCoors",0x72D9F0,0,-1,1024},{"CCullZones","FindMirrorAttributesForCoors",0x72DA70,0,-1,1024},
    {"CCullZones","FindZoneWithStairsAttributeForPlayer",0x72DAD0,0,-1,512},{"CCullZones","Update",0x72DEC0,0,-1,512},
    {"CTheZones","InitZonesPopulationSettings",0x5720D0,0,-1,64},{"CTheZones","ResetZonesRevealed",0x572110,0,-1,64},
    {"CTheZones","AssignZoneInfoForThisZone",0x572180,0,-1,512},{"CTheZones","ZoneIsEntirelyContainedWithinOtherZone",0x572220,0,-1,512},
    {"CTheZones","PointLiesWithinZone",0x572270,0,-1,1024},{"CTheZones","GetLevelFromPosition",0x572300,0,-1,512},
    {"CTheZones","FindSmallestZoneForPosition",0x572360,0,-1,512},{"CTheZones","GetZoneInfo-",0x572400,0,-1,512},
    {"CTheZones","FillZonesWithGangColours",0x572440,0,-1,256},{"CTheZones","GetNavigationZone",0x572590,0,-1,128},{"CTheZones","GetMapZone",0x5725A0,0,-1,128},
    {"CTheZones","Calc2DDistanceBetween2Zones",0x5725B0,0,-1,1024},{"CTheZones","Init",0x572670,0,-1,64},{"CTheZones","CreateZone",0x5728A0,0,-1,512},
    {"CTheZones","PostZoneCreation",0x572B70,0,-1,16},{"CTheZones","FindZone-",0x572B80,0,-1,512},{"CTheZones","FindZoneByLabel",0x572C40,0,-1,512},
    {"CTheZones","SetZoneRadarColours",0x572CC0,0,-1,256},{"CTheZones","Update",0x572D10,0,-1,512},{"CTheZones","Save",0x5D2E60,0,-1,128},
    {"CTheZones","Load",0x5D2F40,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"zones21-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"zones21-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("zones21");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"zones21-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto* cull=reinterpret_cast<uint8*>(CULL_BEGIN); constexpr size_t NC=CULL_END-CULL_BEGIN; const std::vector<uint8> savedCull(cull,cull+NC);
    auto* zones=reinterpret_cast<uint8*>(ZONES_BEGIN); constexpr size_t NZ=ZONES_END-ZONES_BEGIN; const std::vector<uint8> savedZones(zones,zones+NZ);
    auto& frame=*reinterpret_cast<uint32*>(0xB7CB4C); const uint32 savedFrame=frame;
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& area=*reinterpret_cast<int32*>(0xB72914); const int32 savedArea=area;
    auto& offMap=*reinterpret_cast<uint8*>(0xA444A4); const uint8 savedOffMap=offMap;
    auto restoreAll=[&]{ std::memcpy(cull,savedCull.data(),NC); std::memcpy(zones,savedZones.data(),NZ); frame=savedFrame; timeMs=savedTime; area=savedArea; offMap=savedOffMap; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), s1e(0x56E010,FakeCoors);
        EntryPatch s2(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), s2e(0x56E210,FakeFindPlayerPed);
        EntryPatch s3(bit_cast<uintptr_t>(&CPed::IsAlive),FakeIsAlive), s3e(0x5E0170,FakeIsAlive);
        EntryPatch s4(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevelNoDrop),FakeSetWanted), s4e(0x609F30,FakeSetWanted);
        EntryPatch s5(bit_cast<uintptr_t>(&CCamera::GetGameCamPosition),FakeCamPos), s5e(0x50AE50,FakeCamPos);
        EntryPatch s6(bit_cast<uintptr_t>(&CGangWars::CanPlayerStartAGangWarHere),FakeCanStartGangWar), s6e(0x443F80,FakeCanStartGangWar);
        EntryPatch s7(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), s7e(0x5D1270,FakeSaveWB);
        EntryPatch s8(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB), s8e(0x5D1300,FakeLoadWB);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    restoreAll();
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2021u; if(!stt)stt=1; trace=json::array();
                    aliveCalls=gangCalls=0; aliveMask=next(); gangMask=next(); loadState=next();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CCullZones") {
                        auto& nAttr=CCullZones::NumAttributeZones; auto& nTun=CCullZones::NumTunnelAttributeZones; auto& nMir=CCullZones::NumMirrorAttributeZones;
                        // zona rectangular girada: esquina, dos vectores (enteros de 16 bits) y alturas
                        auto randDef=[&](uint8* d){ auto* w=reinterpret_cast<int16*>(d);
                            if(next()%16==0){ for(int k=0;k<8;++k)w[k]=int16(next()); return; }
                            const int16 cx=int16(int(next()%121)-60), cy=int16(int(next()%121)-60); int16 ax=int16(int(next()%161)-80), ay=int16(int(next()%161)-80);
                            int16 bx=int16(-ay), by=ax; if(next()%4==0){bx=int16(int(next()%161)-80);by=int16(int(next()%161)-80);} if(next()%3==0){bx=int16(bx/2);by=int16(by/2);}
                            int16 z0=int16(int(next()%61)-30), z1=int16(z0+int(next()%40)); if(next()%8==0)std::swap(z0,z1); if(next()%12==0){ax=-32768;ay=-32768;}
                            w[0]=cx;w[1]=cy;w[2]=ax;w[3]=ay;w[4]=bx;w[5]=by;w[6]=z0;w[7]=z1; };
                        auto randFlags=[&]()->uint16{ switch(next()%4){case 0:return uint16(next());case 1:return uint16(1u<<(next()%16));case 2:return uint16(0x8000|(next()&0xFF));default:return uint16(next()&0x1FFF);} };
                        auto fillAll=[&]{
                            nAttr=next()%8==0 ? -int32(next()%3) : int32(next()%(next()%6==0 ? 1300u : 64u));
                            nTun=next()%8==0 ? -int32(next()%3) : int32(next()%41); nMir=next()%8==0 ? -int32(next()%3) : int32(next()%73);
                            const int32 la=std::min(1300,std::max(nAttr,0)+8), lt=std::min(40,std::max(nTun,0)+4), lm=std::min(72,std::max(nMir,0)+4);
                            for(int32 k=0;k<la;++k){ auto* d=reinterpret_cast<uint8*>(ATTRS+0x12*k); randDef(d); *reinterpret_cast<uint16*>(d+0x10)=randFlags(); }
                            for(int32 k=0;k<lt;++k){ auto* d=reinterpret_cast<uint8*>(TUNNELS+0x12*k); randDef(d); *reinterpret_cast<uint16*>(d+0x10)=randFlags(); }
                            for(int32 k=0;k<lm;++k){ auto* d=reinterpret_cast<uint8*>(MIRRORS+0x18*k); randDef(d); for(int q=0x10;q<0x18;++q)d[q]=uint8(next()); } };
                        auto randPoint=[&]{ CVector p{uni(-120.f,120.f),uni(-120.f,120.f),uni(-40.f,50.f)};
                            switch(next()%10){case 0:p.z=float(int(next()%61)-30);break;case 1:p.x=float(int(next()%121)-60);p.y=float(int(next()%121)-60);break;
                                case 2:p.x=std::numeric_limits<float>::quiet_NaN();break;case 3:p.z=std::numeric_limits<float>::quiet_NaN();break;case 4:p=CVector{uni(-1e5f,1e5f),uni(-1e5f,1e5f),uni(-1e3f,1e3f)};break;default:break;}
                            return p; };
                        auto randF=[&]()->float{ switch(next()%8){case 0:return uni(-40000.f,40000.f);case 1:return uni(-3e9f,3e9f);case 2:return float(int(next()%201)-100)+0.5f;default:return uni(-300.f,300.f);} };
                        const auto hashOut=[&]{ out["cull"]=Fnv(cull,NC); };
                        if(nm=="Init"){ fillAll(); CCullZones::CurrentFlags_Camera=int32(next()); CCullZones::CurrentFlags_Player=int32(next()); reinterpret_cast<void(__cdecl*)()>(a)(); hashOut(); }
                        else if(nm.rfind("Add",0)==0){ fillAll(); nAttr=int32(next()%1300); nTun=int32(next()%40); nMir=int32(next()%72);
                            CVector pos{randF(),randF(),randF()}; const float v1x=randF(),v1y=randF(),mnz=randF(),v2x=randF(),v2y=randF(),mxz=randF();
                            uint32 flags=randFlags(); if(next()%3==0)flags|=next()%2 ? 0x80u : 0x800u;
                            if(nm=="AddCullZone")reinterpret_cast<void(__cdecl*)(const CVector*,float,float,float,float,float,float,uint32)>(a)(&pos,v1x,v1y,mnz,v2x,v2y,mxz,flags);
                            else if(nm=="AddTunnelAttributeZone")reinterpret_cast<void(__cdecl*)(const CVector*,float,float,float,float,float,float,uint32)>(a)(&pos,v1x,v1y,mnz,v2x,v2y,mxz,flags);
                            else reinterpret_cast<void(__cdecl*)(const CVector*,float,float,float,float,float,float,uint32,float,float,float,float)>(a)(&pos,v1x,v1y,mnz,v2x,v2y,mxz,flags,uni(-500.f,500.f),uni(-2.f,2.f),uni(-2.f,2.f),uni(-2.f,2.f));
                            out["counts"]={nAttr,nTun,nMir}; hashOut(); }
                        else if(nm=="FindAttributesForCoors"||nm=="FindTunnelAttributesForCoors"){ fillAll(); ret=reinterpret_cast<int32(__cdecl*)(CVector)>(a)(randPoint()); hashOut(); }
                        else if(nm=="FindMirrorAttributesForCoors"){ fillAll(); ret=Name(reinterpret_cast<void*(__cdecl*)(CVector)>(a)(randPoint())); hashOut(); }
                        else if(nm=="FindZoneWithStairsAttributeForPlayer"){ fillAll(); playerCoors=randPoint(); ret=Name(reinterpret_cast<void*(__cdecl*)()>(a)()); hashOut(); }
                        else if(nm=="Update"){ fillAll(); frame=next()%3 ? uint32(next()&~7u)|(next()%2 ? 2u : 6u) : next(); camPos=randPoint(); playerCoors=randPoint();
                            CCullZones::bMilitaryZonesDisabled=next()%3==0; CCullZones::CurrentFlags_Camera=int32(next()); CCullZones::CurrentFlags_Player=int32(next());
                            if(next()%2){ const int32 c=std::max(nAttr,0); if(c>0)*reinterpret_cast<uint16*>(ATTRS+0x12*(next()%uint32(c))+0x10)|=0x1000; }
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["flags"]={CCullZones::CurrentFlags_Camera,CCullZones::CurrentFlags_Player}; hashOut(); }
                        else { CCullZones::CurrentFlags_Player=int32(next()); CCullZones::CurrentFlags_Camera=int32(next()); ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); }
                    }
                    else {
                        auto& nNav=CTheZones::TotalNumberOfNavigationZones; auto& nInfo=CTheZones::TotalNumberOfZoneInfos; auto& nMap=CTheZones::TotalNumberOfMapZones;
                        static const char* pool[]{"SAN_AND","GAN","LIND","IWD","ELCO","DOH","ROD","VERO","ABC","THEMAP","LA","SF","VE",""};
                        auto setLabel=[&](char* d,const char* s){ std::memset(d,0,8); std::memcpy(d,s,std::min<size_t>(strlen(s),7)); if(next()%5==0)for(size_t k=strlen(d)+1;k<8;++k)d[k]=char(next()); };
                        auto randZone=[&](CZone* z,bool full){ auto* b=reinterpret_cast<uint8*>(z);
                            setLabel(z->m_InfoLabel,pool[next()%std::size(pool)]); setLabel(z->m_TextLabel,pool[next()%std::size(pool)]);
                            if(full&&next()%6==0){ for(int k=0x10;k<0x1C;++k)b[k]=uint8(next()); }
                            else { z->m_fX1=int16(int(next()%5800)-3000); z->m_fX2=int16(z->m_fX1+int(next()%2000)); z->m_fY1=int16(int(next()%5800)-3000); z->m_fY2=int16(z->m_fY1+int(next()%2000));
                                z->m_fZ1=int16(int(next()%400)-200); z->m_fZ2=int16(z->m_fZ1+int(next()%600)); if(next()%10==0)std::swap(z->m_fX1,z->m_fX2); }
                            z->m_ZoneInfoIndex=int16(next()%380); b[0x1E]=uint8(next()%4==0 ? next()%4 : next()%2); b[0x1F]=uint8(next()%5==0 ? next() : next()%4); };
                        auto fillAll=[&](uint16 nav,uint16 info,uint16 map){ nNav=int16(nav); nInfo=int16(info); nMap=int16(map);
                            auto* navs=reinterpret_cast<CZone*>(NAVZ); auto* maps=reinterpret_cast<CZone*>(MAPZ);
                            for(uint32 k=0;k<std::min<uint32>(380,nav+4u);++k)randZone(&navs[k],true);
                            for(uint32 k=0;k<std::min<uint32>(39,map+4u);++k)randZone(&maps[k],true);
                            if(next()%2){ navs[0].m_fX1=-3000;navs[0].m_fY1=-3000;navs[0].m_fZ1=-2000;navs[0].m_fX2=3000;navs[0].m_fY2=3000;navs[0].m_fZ2=2000; }
                            for(uint32 k=0;k<std::min<uint32>(380,info+4u);++k){ auto* b=reinterpret_cast<uint8*>(INFOS+0x11*k); for(int q=0;q<0x11;++q)b[q]=uint8(next());
                                if(next()%3)for(int q=0;q<3;++q)b[q]=next()%3==0 ? 0 : uint8(next()%60); if(next()%5==0)b[0]=b[1]=b[2]=0; }
                            for(int k=0;k<100;++k)zones[0xBA3730-ZONES_BEGIN+k]=uint8(next()%2); CTheZones::ZonesRevealed=int32(next()%100); };
                        auto randCounts=[&]{ const uint16 nav=uint16(next()%8==0 ? next()%381 : next()%60); const uint16 info=uint16(next()%8==0 ? next()%381 : next()%60); const uint16 map=uint16(next()%40); fillAll(nav,info,map); };
                        auto randPoint=[&]{ CVector p{uni(-3200.f,3200.f),uni(-3200.f,3200.f),uni(-300.f,500.f)};
                            if(next()%4==0){ const bool nav=next()%2; auto* z=reinterpret_cast<CZone*>(nav ? NAVZ : MAPZ)+next()%std::max<uint32>(1,uint32(uint16(nav ? nNav : nMap)));
                                p=CVector{float(next()%2 ? z->m_fX1 : z->m_fX2),float(next()%2 ? z->m_fY1 : z->m_fY2),float(next()%2 ? z->m_fZ1 : z->m_fZ2)}; }
                            if(next()%16==0)p.y=std::numeric_limits<float>::quiet_NaN(); return p; };
                        const auto hashOut=[&]{ out["zones"]=Fnv(zones,NZ); out["counts"]={nNav,nInfo,nMap}; };
                        auto ZI=[&](const void* p)->json{ if(!p)return nullptr; const auto u=uintptr_t(p);
                            if(u>=NAVZ&&u<NAVZ+380*0x20)return json::array({"nav",(u-NAVZ)/0x20,(u-NAVZ)%0x20}); if(u>=MAPZ&&u<MAPZ+39*0x20)return json::array({"map",(u-MAPZ)/0x20,(u-MAPZ)%0x20});
                            if(u>=INFOS&&u<INFOS+380*0x11)return json::array({"info",(u-INFOS)/0x11,(u-INFOS)%0x11}); return json::array({"raw",intptr_t(u)-intptr_t(NAVZ)}); };

                        if(nm=="InitZonesPopulationSettings"||nm=="ResetZonesRevealed"||nm=="Init"||nm=="PostZoneCreation"){ randCounts(); for(int k=0;k<20;++k)zones[next()%NZ]=uint8(next());
                            reinterpret_cast<void(__cdecl*)()>(a)(); hashOut(); }
                        else if(nm=="AssignZoneInfoForThisZone"){ const uint16 nav=uint16(next()%6==0 ? 0 : 1+next()%(next()%6==0 ? 378 : 60)); fillAll(nav,uint16(next()%300),uint16(next()%39));
                            auto* navs=reinterpret_cast<CZone*>(NAVZ); uint16 self=nav;
                            if(nav>1&&next()%3==0){ self=uint16(1+next()%(nav-1)); const uint16 j=uint16(next()%self); std::memcpy(navs[self].m_InfoLabel,navs[j].m_InfoLabel,8); }
                            reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(int16(self))); out["self"]=self; out["index"]=navs[self].m_ZoneInfoIndex; hashOut(); }
                        else if(nm=="ZoneIsEntirelyContainedWithinOtherZone"||nm=="Calc2DDistanceBetween2Zones"){ alignas(4) static CZone za, zb; randZone(&za,false); randZone(&zb,false);
                            auto* wa=reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&za)+0x10); auto* wb=reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&zb)+0x10);
                            switch(next()%5){case 0:for(int k=0;k<6;++k)wa[k]=int16(next());for(int k=0;k<6;++k)wb[k]=int16(next());break;
                                case 1:for(int k=0;k<3;++k){wa[k]=int16(wb[k]+int(next()%50)-(next()%4==0 ? 5 : 0));wa[k+3]=int16(wb[k+3]-int(next()%50)+(next()%4==0 ? 5 : 0));}break;
                                case 2:std::memcpy(wa,wb,12);break;default:break;}
                            if(nm[0]=='Z')ret=int(reinterpret_cast<uint8(__cdecl*)(CZone*,CZone*)>(a)(&za,&zb));
                            else ret=F(reinterpret_cast<float(__cdecl*)(CZone*,CZone*)>(a)(&za,&zb)); }
                        else if(nm=="PointLiesWithinZone"){ alignas(4) static CZone z; randZone(&z,false); CVector p{uni(float(z.m_fX1)-50.f,float(z.m_fX2)+50.f),uni(float(z.m_fY1)-50.f,float(z.m_fY2)+50.f),uni(float(z.m_fZ1)-50.f,float(z.m_fZ2)+50.f)};
                            switch(next()%8){case 0:p.x=float(next()%2 ? z.m_fX1 : z.m_fX2);break;case 1:p.y=float(next()%2 ? z.m_fY1 : z.m_fY2);break;case 2:p.z=float(next()%2 ? z.m_fZ1 : z.m_fZ2);break;
                                case 3:(&p.x)[next()%3]=std::numeric_limits<float>::quiet_NaN();break;default:break;}
                            ret=int(reinterpret_cast<uint8(__cdecl*)(const CVector*,CZone*)>(a)(&p,&z)); }
                        else if(nm=="GetLevelFromPosition"){ randCounts(); if(next()%6==0)nMap=int16(next()%2); const CVector p=randPoint(); ret=reinterpret_cast<uint32(__cdecl*)(const CVector*)>(a)(&p); }
                        else if(nm=="FindSmallestZoneForPosition"){ randCounts(); if(next()%6==0)nNav=int16(next()%2); const CVector p=randPoint();
                            ret=ZI(reinterpret_cast<void*(__cdecl*)(const CVector*,int32)>(a)(&p,int32(next()%2))); }
                        else if(nm=="GetZoneInfo-"){ randCounts(); const CVector p=randPoint(); void* oz=reinterpret_cast<void*>(0x1234); const bool useOut=next()%4!=0;
                            ret=ZI(reinterpret_cast<void*(__cdecl*)(const CVector*,void**)>(a)(&p,useOut ? &oz : nullptr)); out["zone"]=useOut ? ZI(oz) : json("none"); }
                        else if(nm=="FillZonesWithGangColours"){ randCounts(); if(next()%8==0)nInfo=0; reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next()%3==0)); hashOut(); }
                        else if(nm=="GetNavigationZone"||nm=="GetMapZone"){ const uint32 idx=next()%4 ? next()%40 : next()&0xFFFF;
                            ret=intptr_t(reinterpret_cast<uint8*(__cdecl*)(uint32)>(a)(idx))-intptr_t(nm=="GetMapZone" ? MAPZ : NAVZ); }
                        else if(nm=="CreateZone"){ fillAll(uint16(next()%379),uint16(next()%370),uint16(next()%38));
                            static char lab1[16], lab2[16]; static const char chars[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_ \xE9";
                            auto mk=[&](char* d){ std::memset(d,0,16); if(next()%3==0){ const char* s=pool[next()%std::size(pool)]; std::memcpy(d,s,strlen(s)); for(size_t k=0;d[k];++k)if(next()%2&&d[k]>='A'&&d[k]<='Z')d[k]+=0x20; }
                                else { const uint32 n=next()%13; for(uint32 k=0;k<n;++k)d[k]=chars[next()%(sizeof(chars)-1)]; } };
                            mk(lab1); mk(lab2); auto rf=[&]{ switch(next()%6){case 0:return uni(-40000.f,40000.f);case 1:return float(int(next()%201)-100)+0.5f;default:return uni(-3000.f,3000.f);} };
                            const CVector p1{rf(),rf(),rf()}, p2{rf(),rf(),rf()}; const int32 type=next()%6==0 ? int32(next()%6) : int32(next()%4);
                            reinterpret_cast<void(__cdecl*)(const char*,int32,CVector,CVector,int32,const char*)>(a)(lab1,type,p1,p2,int32(uint8(next())),lab2);
                            out["labels"]={ReversingProbe::Bytes(lab1,16),ReversingProbe::Bytes(lab2,16)}; hashOut(); }
                        else if(nm=="FindZone-"){ randCounts(); const CVector pc=randPoint(); CVector p=pc; uint64 name{};
                            auto* navs=reinterpret_cast<CZone*>(NAVZ); const int32 type=next()%5==0 ? int32(next()%5) : (next()%2 ? 0 : 2);
                            if(next()%4 && nNav>0){ const auto& z=navs[next()%uint32(uint16(nNav))]; std::memcpy(&name,next()%2 ? z.m_TextLabel : z.m_InfoLabel,8); if(next()%2)p=CVector{float(z.m_fX1),float(z.m_fY1),float(z.m_fZ1)}; }
                            else { const char* s=pool[next()%std::size(pool)]; std::memcpy(&name,s,strlen(s)); }
                            ret=int(reinterpret_cast<uint8(__cdecl*)(CVector*,uint64,int32)>(a)(&p,name,type)); }
                        else if(nm=="FindZoneByLabel"){ randCounts(); char nameBuf[8]{}; auto* navs=reinterpret_cast<CZone*>(NAVZ);
                            if(next()%3 && nNav>0)std::memcpy(nameBuf,navs[next()%uint32(uint16(nNav))].m_InfoLabel,7); else { const char* s=pool[next()%std::size(pool)]; std::memcpy(nameBuf,s,strlen(s)); }
                            nameBuf[7]=0; const int32 type=next()%5==0 ? int32(next()%4) : 2;
                            ret=int(reinterpret_cast<int16(__cdecl*)(const char*,int32)>(a)(nameBuf,type)); }
                        else if(nm=="SetZoneRadarColours"){ randCounts(); const uint16 cnt=std::max<uint16>(1,uint16(nNav)); auto* navs=reinterpret_cast<CZone*>(NAVZ);
                            const uint16 idx=uint16(next()%cnt); navs[idx].m_ZoneInfoIndex=int16(next()%380);
                            reinterpret_cast<void(__cdecl*)(int32,int32,int32,int32,int32)>(a)(int32(int16(idx)),int32(int8(next())),int32(uint8(next())),int32(uint8(next())),int32(uint8(next())));
                            hashOut(); }
                        else if(nm=="Update"){ randCounts(); timeMs=next()%4 ? next()%100000 : next(); auto& cs=*reinterpret_cast<uint32*>(0xBA6720); auto& guard=*reinterpret_cast<uint32*>(0xBA6724);
                            cs=next()%2 ? timeMs-next()%10000 : next(); guard=next()%4==0 ? (next()&~1u) : (next()|1u); area=next()%3==0 ? int32(1+next()%17) : 0; offMap=uint8(next()%4==0);
                            playerCoors=CVector{uni(-2999.f,2999.f),uni(-2999.f,2999.f),uni(-100.f,300.f)};
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["level"]=*reinterpret_cast<uint32*>(0xBA6718); out["cs"]=cs; out["guard"]=guard; hashOut(); }
                        else if(nm=="Save"){ randCounts(); ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); hashOut(); }
                        else if(nm=="Load"){ randCounts(); loadNav=uint16(next()%381); loadInfos=uint16(next()%381); loadMap=uint16(next()%40);
                            ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); hashOut(); }
                    }
                    out["ret"]=ret; out["trace"]=trace; restoreAll(); return out;
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
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll();
    const bool restored=std::memcmp(cull,savedCull.data(),NC)==0&&std::memcmp(zones,savedZones.data(),NZ)==0&&frame==savedFrame&&timeMs==savedTime&&area==savedArea&&offMap==savedOffMap;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
