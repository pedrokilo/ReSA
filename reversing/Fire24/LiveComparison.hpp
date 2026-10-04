#pragma once
// Banco dentro del motor: gestor de fuegos (lista de 24; 24 funciones de CFireManager). Original (hooks del grupo
// desactivados) frente a candidato con fixtures deterministas sobre un CFireManager falso (60 fuegos con relleno
// alrededor) y entidades falsas con desplazamientos crudos. Se simulan y trazan: rand, el gestor de efectos
// (CreateFxSystem/DestroyFxSystem/Kill/Play/PlayAndKill/AddParticle sobre sistemas falsos), referencias de entidades,
// el nivel del agua, IsPedInControl/IsPlayer, CCrime::ReportCrime, FindPlayerPed, GetNewUniqueScriptThingIndex,
// CFire::ProcessFire, los disturbios de Los Santos, FindGroundZFor3DCoord, sombras estáticas y coronas. La camara
// (matriz, placement y m_mCameraMatrix), el reloj, CReplay::Mode, g_fx y el gFireManager real (solo se lee su maximo
// de generaciones) se guardan y se restauran en cada ejecucion. Nunca se crean fuegos ni efectos reales.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fire.h"
#include "FireManager.h"
#include "FxManager.h"
#include "FxSystem.h"
#include "Shadows.h"
#include "Coronas.h"
#include "Crime.h"
#include "GameLogic.h"
#include "WaterLevel.h"
#include "World.h"
#include "TheScripts.h"
#include "../ProbeRuntime.hpp"

namespace Fire24Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; }
inline bool BitB(){ return (maskB>>(callsB++%32))&1; }

constexpr uintptr_t FIRES=0xB71F80, CAM_MAT=0xB6F99C, FXMAN=0xA9AE80;

// --- buferes falsos ---
alignas(16) inline std::array<uint8,0x28+0x964+0x40> mgrBuf{};   // fuego -1 (relleno), 60 fuegos, maximo, relleno
inline uint8* Mgr(){ return mgrBuf.data()+0x28; }
alignas(16) inline std::array<std::array<uint8,0xA18>,6> ent{};
alignas(16) inline std::array<std::array<uint8,0x100>,4> fx{};
alignas(16) inline std::array<uint8,0x60> camMat{};
alignas(16) inline std::array<void*,64> trapVt{};
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return "ext"; }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }
inline json Vec(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }

// --- stubs ---
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap",Id(self)}); }
inline uint32 createCalls{};
inline FxSystem_c* __fastcall FakeCreateFx(void* self,void*,const char* name,const CVector& p,RwMatrix* m,bool ignore){
    void* r=BitA() ? fx[createCalls%4].data() : nullptr; ++createCalls;
    trace.push_back({"create_fx",intptr_t(self)-intptr_t(FXMAN),std::string(name),Vec(p),Id(m),ignore,Id(r)}); return static_cast<FxSystem_c*>(r); }
inline void __fastcall FakeDestroyFx(void* self,void*,FxSystem_c* s){ trace.push_back({"destroy_fx",intptr_t(self)-intptr_t(FXMAN),Id(s)}); }
inline void __fastcall FakeFxKill(void* self,void*){ trace.push_back({"fx_kill",Id(self)}); }
inline void __fastcall FakeFxPlay(void* self,void*){ trace.push_back({"fx_play",Id(self)}); }
inline void __fastcall FakeFxPlayAndKill(void* self,void*){ trace.push_back({"fx_play_and_kill",Id(self)}); }
inline void __fastcall FakeAddParticle(void* self,void*,const CVector& pos,const CVector& vel,float t,const FxPrtMult_c& m,float rz,float lm,float ll,bool local){
    trace.push_back({"add_particle",Id(self),Vec(pos),Vec(vel),F(t),ReversingProbe::Bytes(&m,0x1C),F(rz),F(lm),F(ll),local}); }
inline void __fastcall FakeCleanUp(void* self,void*,CEntity** ref){ trace.push_back({"clean_up_ref",Id(self),Id(ref)}); }
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** ref){ trace.push_back({"register_ref",Id(self),Id(ref)}); }
inline float waterLevel{};
inline bool __cdecl FakeWaterNoWaves(CVector p,float* level,float* bw,float* sw){ const bool r=BitA(); if(level)*level=waterLevel;
    trace.push_back({"water_level_no_waves",Vec(p),level!=nullptr,bw!=nullptr,sw!=nullptr,r}); return r; }
inline bool __fastcall FakeInControl(void* self,void*){ const bool r=BitB(); trace.push_back({"in_control",Id(self),r}); return r; }
inline bool __fastcall FakeIsPlayer(void* self,void*){ const bool r=BitB(); trace.push_back({"is_player",Id(self),r}); return r; }
inline void __cdecl FakeReportCrime(int32 type,CEntity* victim,CPed* by){ trace.push_back({"report_crime",type,Id(victim),Id(by)}); }
inline void* playerPed{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32){ return static_cast<CPlayerPed*>(playerPed); }
inline int32 __cdecl FakeUniqueIndex(int32 index,int32 type){ const int32 r=int32(index*0x10000+uint8(type)*0x100+int32(seed&0xFF)); trace.push_back({"unique_index",index,uint8(type),r}); return r; }
inline void __fastcall FakeProcessFire(void* self,void*){ trace.push_back({"process_fire",Id(self)}); }
inline bool riots{};
inline bool __cdecl FakeRiots(){ trace.push_back({"riots",riots}); return riots; }
inline float groundZ{}; inline bool groundHit{}; inline void* groundEnt{};
inline float __cdecl FakeGroundZ(CVector c,bool* hit,CEntity** e){ trace.push_back({"ground_z",Vec(c),hit!=nullptr,e!=nullptr});
    if(hit)*hit=groundHit; if(e)*e=static_cast<CEntity*>(groundEnt); return groundZ; }
inline bool __cdecl FakeShadow(uint32 id,eShadowType type,RwTexture* tex,const CVector& pos,float fx0,float fy0,float sx0,float sy0,int16 in,uint8 r,uint8 g,uint8 b,float zd,float sc,float dd,bool tmp,float up){
    trace.push_back({"static_shadow",Id(reinterpret_cast<void*>(id)),int(type),uintptr_t(tex),Vec(pos),F(fx0),F(fy0),F(sx0),F(sy0),in,r,g,b,F(zd),F(sc),F(dd),tmp,F(up)}); return true; }
inline void __cdecl FakeCorona(uint32 id,CEntity* at,uint8 r,uint8 g,uint8 b,uint8 in,const CVector& pos,float sz,float range,eCoronaType t,eCoronaFlareType fl,eCoronaReflType rf,
    eCoronaLOSCheck los,eCoronaTrail tr,float na,bool neon,float pull,bool fb,float fade,bool below,bool white){
    trace.push_back({"corona",Id(reinterpret_cast<void*>(id)),Id(at),r,g,b,in,Vec(pos),F(sz),F(range),int(t),int(fl),int(rf),int(los),int(tr),F(na),neon,F(pull),fb,F(fade),below,white}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"CFireManager","ClearAllScriptFireFlags",0x5397A0,0,-1,64},{"CFireManager","Constructor",0x539DA0,0,-1,64},
    {"CFireManager","CreateAllFxSystems",0x539D50,0,-1,256},{"CFireManager","DestroyAllFxSystems",0x539D10,0,-1,256},
    {"CFireManager","Destructor",0x538BB0,0,-1,64},{"CFireManager","ExtinguishPoint",0x539450,0,-1,512},
    {"CFireManager","ExtinguishPointWithWater",0x5394C0,0,-1,512},{"CFireManager","FindNearestFire",0x538F40,0,-1,512},
    {"CFireManager","GetNextFreeFire",0x539E50,0,-1,512},{"CFireManager","GetNumFiresInArea",0x539860,0,-1,512},
    {"CFireManager","GetNumFiresInRange",0x5397F0,0,-1,512},{"CFireManager","GetNumOfNonScriptFires",0x538F10,0,-1,128},
    {"CFireManager","GetScriptFireCoords",0x5397E0,0,-1,128},{"CFireManager","Init",0x538BC0,0,-1,64},
    {"CFireManager","IsScriptFireExtinguished",0x5396E0,0,-1,128},{"CFireManager","PlentyFiresAvailable",0x539340,0,-1,256},
    {"CFireManager","RemoveAllScriptFires",0x539720,0,-1,256},{"CFireManager","RemoveScriptFire",0x539700,0,-1,256},
    {"CFireManager","SetScriptFireAudio",0x5397B0,0,-1,128},{"CFireManager","Shutdown",0x539DD0,0,-1,128},
    {"CFireManager","StartFire-",0x53A050,0,-1,1024},{"CFireManager","StartFire-NoTarget",0x539F00,0,-1,1024},
    {"CFireManager","StartScriptFire",0x53A270,0,-1,1024},{"CFireManager","Update",0x53AF00,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fire24-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fire24-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fire24");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fire24-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,24> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    using std::bit_cast;
    if(regions.empty()){ Reg(mgrBuf.data(),mgrBuf.size()); for(auto& b:ent)Reg(b.data(),b.size()); for(auto& b:fx)Reg(b.data(),b.size()); Reg(camMat.data(),camMat.size()); }
    for(auto& v:trapVt)v=reinterpret_cast<void*>(&FakeVTrap);

    // estado global guardado
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {FIRES,0x964},{0xB6F028,0x18},{CAM_MAT,0x40},{0xB7CB4C,0x3C},{0xA43088,1},{0xA9AE00,0x80}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch f1(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(Const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),FakeCreateFx), f1e(0x4A9BE0,FakeCreateFx);
        EntryPatch f2(bit_cast<uintptr_t>(&FxManager_c::DestroyFxSystem),FakeDestroyFx), f2e(0x4A9810,FakeDestroyFx);
        EntryPatch f3(bit_cast<uintptr_t>(&FxSystem_c::Kill),FakeFxKill), f3e(0x4AA3F0,FakeFxKill);
        EntryPatch f4(bit_cast<uintptr_t>(&FxSystem_c::Play),FakeFxPlay), f4e(0x4AA2F0,FakeFxPlay);
        EntryPatch f5(bit_cast<uintptr_t>(&FxSystem_c::PlayAndKill),FakeFxPlayAndKill), f5e(0x4AA3D0,FakeFxPlayAndKill);
        EntryPatch f6(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle), f6e(0x4AA440,FakeAddParticle);
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp), e1e(0x571A00,FakeCleanUp);
        EntryPatch e2(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterRef), e2e(0x571B70,FakeRegisterRef);
        EntryPatch w1(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),FakeWaterNoWaves), w1e(0x6E8580,FakeWaterNoWaves);
        EntryPatch c1(bit_cast<uintptr_t>(&CPed::IsPedInControl),FakeInControl), c1e(0x5E3960,FakeInControl);
        EntryPatch c2(bit_cast<uintptr_t>(&CPed::IsPlayer),FakeIsPlayer), c2e(0x5DF8F0,FakeIsPlayer);
        EntryPatch c3(bit_cast<uintptr_t>(&CCrime::ReportCrime),FakeReportCrime), c3e(0x532010,FakeReportCrime);
        EntryPatch c4(bit_cast<uintptr_t>(&CTheScripts::GetNewUniqueScriptThingIndex),FakeUniqueIndex), c4e(0x483720,FakeUniqueIndex);
        EntryPatch u1(bit_cast<uintptr_t>(&CFire::ProcessFire),FakeProcessFire), u1e(0x53A570,FakeProcessFire);
        EntryPatch u2(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),FakeRiots), u2e(0x441C10,FakeRiots);
        EntryPatch u3(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),FakeGroundZ), u3e(0x5696C0,FakeGroundZ);
        EntryPatch u4(bit_cast<uintptr_t>(&CShadows::StoreStaticShadow),FakeShadow), u4e(0x70BA00,FakeShadow);
        EntryPatch u5(bit_cast<uintptr_t>(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona)),FakeCorona), u5e(0x6FC580,FakeCorona);
        auto& camMatrix=G<void*>(0xB6F03C); auto& camPlace=G<CVector>(0xB6F02C); auto& camHeading=G<float>(0xB6F038);
        auto& timeMs=G<uint32>(0xB7CB84); auto& prevMs=G<uint32>(0xB7CB78); auto& timeStep=G<float>(0xB7CB5C);
        auto& replayMode=G<uint8>(0xA43088); auto& maxGens=G<uint32>(FIRES+0x960); auto& smoke=G<void*>(0xA9AE24);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto unitVec=[&]{ CVector v=randVec(-1.f,1.f); const float m=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z)+1e-3f; return CVector{v.x/m,v.y/m,v.z/m}; };
                auto strength=[&]{ static const float st[]{0.f,0.5f,1.f,1.5f,2.f,2.5f,3.f,0.999f,1.999f,2.0001f,0.0001f,4.f};
                    return next()%3 ? uni(0.f,3.2f) : st[next()%std::size(st)]; };
                // entidades falsas: 0 peaton, 1 vehiculo, 2 objeto, 3 tipo aleatorio, 4 peaton (autor), 5 vehiculo
                auto mkEnt=[&](void* e,int type,const CVector& pos){ fill(e,0xA18); At<void*>(e,0)=trapVt.data(); At<uint8>(e,0x36)=uint8((next()&0xF8)|type);
                    At<void*>(e,0x14)=nullptr; At<CVector>(e,4)=pos; At<void*>(e,0x18)=nullptr; };
                auto entFor=[&](uint32 k)->void*{ return k<6 ? ent[k].data() : nullptr; };
                auto mkFire=[&](uint8* p,const CVector& base,float spread,int activePct){ fill(p,0x28);
                    uint8 fl=uint8(next()&0xE0); if(int(next()%100)<activePct)fl|=1; if(next()%4==0)fl|=2; if(next()%2)fl|=4; if(next()%4==0)fl|=8; if(next()%3)fl|=0x10; p[0]=fl;
                    At<CVector>(p,4)=base+randVec(-spread,spread); At<void*>(p,0x10)=next()%3==0 ? entFor(next()%8) : nullptr; At<void*>(p,0x14)=next()%3==0 ? entFor(next()%8) : nullptr;
                    At<float>(p,0x1C)=strength(); At<void*>(p,0x24)=next()%3 ? fx[next()%4].data() : nullptr; };
                auto mkMgr=[&](const CVector& base,float spread,int activePct){ for(int k=-1;k<60;++k)mkFire(Mgr()+k*0x28,base,spread,activePct);
                    fill(Mgr()+0x960,0x44); At<uint32>(Mgr(),0x960)=next()%2 ? 999999u : next()%10; };
                auto mkEnts=[&](const CVector& base){ mkEnt(ent[0].data(),3,base+randVec(-3.f,3.f)); mkEnt(ent[1].data(),2,base+randVec(-3.f,3.f)); mkEnt(ent[2].data(),4,base+randVec(-3.f,3.f));
                    mkEnt(ent[3].data(),int(next()%8),base+randVec(-3.f,3.f)); mkEnt(ent[4].data(),3,base); mkEnt(ent[5].data(),2,base+randVec(-3.f,3.f));
                    for(int k:{0,4}){ At<void*>(ent[k].data(),0x730)=nullptr; At<int32>(ent[k].data(),0x598)=next()%3==0 ? 6 : int32(next()%20); }
                    for(int k:{1,5}){ At<void*>(ent[k].data(),0x490)=nullptr; At<int32>(ent[k].data(),0x590)=next()%2 ? 0 : int32(next()%11); At<uint8>(ent[k].data(),0x5A4)=uint8(next()%3==0 ? 225+next()%31 : next()%256); }
                    // Como el original lee m_pFire segun el tipo: todas las entidades (incluida la de tipo aleatorio) sin basura ahi
                    for(auto& e:ent){ At<void*>(e.data(),0x730)=nullptr; At<void*>(e.data(),0x490)=nullptr; At<void*>(e.data(),0x164)=nullptr; } };
                auto fireBytes=[&]{ return ReversingProbe::Bytes(Mgr()-0x28,0x28+0x964); };
                auto entState=[&]{ json j=json::array(); for(int k=0;k<6;++k)j.push_back({Id(At<void*>(ent[k].data(),0x730)),Id(At<void*>(ent[k].data(),0x490)),Id(At<void*>(ent[k].data(),0x164)),At<uint8>(ent[k].data(),0x5A4)}); return j; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2424u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0; createCalls=0;
                    maskA=next(); maskB=next(); callsA=callsB=0; playerPed=ent[4].data();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address; void* self=Mgr();
                    json out={{"switches",switches}}; json ret=nullptr;
                    timeMs=next()%3 ? 100000+next()%5000000 : next(); prevMs=next()%2 ? timeMs-next()%600 : timeMs; timeStep=next()%6==0 ? 1.f : uni(0.2f,3.f);
                    replayMode=uint8(next()%8==0 ? 1 : 0); maxGens=next()%2 ? 999999u : (next()%3==0 ? 0x80000000u|next() : next()%120);
                    smoke=fx[3].data(); for(auto& b:fx)fill(b.data(),b.size());
                    const CVector base{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,100.f)};
                    mkEnts(base);
                    const int density=int(next()%4==0 ? 100 : next()%4==0 ? 0 : next()%100);
                    mkMgr(base,next()%2 ? 4.f : 40.f,density);
                    // pegar m_pFire de las entidades a fuegos del gestor (o a otros)
                    for(int k:{0,4})if(next()%3==0)At<void*>(ent[k].data(),0x730)=Mgr()+(next()%60)*0x28;
                    for(int k:{1,5})if(next()%3==0)At<void*>(ent[k].data(),0x490)=Mgr()+(next()%60)*0x28;
                    if(next()%4==0)At<void*>(ent[2].data(),0x164)=Mgr()+(next()%60)*0x28;
                    if(next()%3==0){ void* fp=Mgr()+(next()%60)*0x28; At<void*>(ent[3].data(),0x730)=fp; At<void*>(ent[3].data(),0x490)=fp; At<void*>(ent[3].data(),0x164)=fp; }
                    for(int k:{0,1,4,5})At<uint32>(ent[k].data(),0x40)=next()%4==0 ? 0x80000u|next() : next()&~0x80000u;

                    if(nm=="ClearAllScriptFireFlags"||nm=="CreateAllFxSystems"||nm=="DestroyAllFxSystems"||nm=="RemoveAllScriptFires"||nm=="Shutdown"||nm=="Init")
                        reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr);
                    else if(nm=="Constructor"){ if(next()%2)std::memset(Mgr(),0xCD,0x964); ret=Id(reinterpret_cast<void*(__fastcall*)(void*,void*)>(a)(self,nullptr)); }
                    else if(nm=="Destructor") reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr);
                    else if(nm=="GetNumOfNonScriptFires") ret=reinterpret_cast<uint32(__fastcall*)(void*,void*)>(a)(self,nullptr);
                    else if(nm=="PlentyFiresAvailable") ret=reinterpret_cast<bool(__fastcall*)(void*,void*)>(a)(self,nullptr);
                    else if(nm=="ExtinguishPoint"||nm=="ExtinguishPointWithWater"){
                        // punto entero y fuegos a distancias exactas (igualdad r² == d²)
                        const CVector pt{float(int(base.x)),float(int(base.y)),float(int(base.z))}; const float r=next()%3==0 ? 3.f : uni(0.f,12.f);
                        for(int k=0;k<60;k+=3){ static const CVector off[]{CVector{1.f,2.f,2.f},CVector{0.f,3.f,0.f},CVector{-2.f,-1.f,2.f},CVector{2.f,2.f,1.f},CVector{3.f,0.f,0.f}}; At<CVector>(Mgr()+k*0x28,4)=pt+off[next()%5]; }
                        if(nm=="ExtinguishPoint") reinterpret_cast<void(__fastcall*)(void*,void*,CVector,float)>(a)(self,nullptr,pt,r);
                        else { const float water=next()%4==0 ? 0.f : uni(0.f,80.f); ret=reinterpret_cast<bool(__fastcall*)(void*,void*,CVector,float,float)>(a)(self,nullptr,pt,r,water); } }
                    else if(nm=="FindNearestFire"){ const CVector pt=base+randVec(-20.f,20.f); const bool b1=next()%2, b2=next()%2;
                        if(next()%4==0){ At<CVector>(Mgr()+5*0x28,4)=At<CVector>(Mgr()+9*0x28,4); }   /* empate */
                        ret=Id(reinterpret_cast<CFire*(__fastcall*)(void*,void*,const CVector&,bool,bool)>(a)(self,nullptr,pt,b1,b2)); }
                    else if(nm=="GetNextFreeFire"){ const bool may=next()%4!=0; ret=Id(reinterpret_cast<CFire*(__fastcall*)(void*,void*,bool)>(a)(self,nullptr,may)); }
                    else if(nm=="GetNumFiresInArea"){ CVector lo=base+randVec(-10.f,3.f), hi=base+randVec(-3.f,10.f);
                        if(next()%3==0){ At<CVector>(Mgr()+7*0x28,4)=lo; At<CVector>(Mgr()+8*0x28,4)=hi; }   /* sobre los bordes */
                        ret=reinterpret_cast<uint32(__fastcall*)(void*,void*,float,float,float,float,float,float)>(a)(self,nullptr,lo.x,lo.y,lo.z,hi.x,hi.y,hi.z); }
                    else if(nm=="GetNumFiresInRange"){ const CVector pt{float(int(base.x)),float(int(base.y)),base.z}; const float r=next()%3==0 ? 5.f : uni(0.f,30.f);
                        for(int k=0;k<60;k+=4)At<CVector>(Mgr()+k*0x28,4)=pt+CVector{next()%2 ? 3.f : -4.f,next()%2 ? 4.f : 3.f,uni(-9.f,9.f)};
                        ret=reinterpret_cast<uint32(__fastcall*)(void*,void*,const CVector&,float)>(a)(self,nullptr,pt,r); }
                    else if(nm=="GetScriptFireCoords"){ const int16 id=int16(next()%61)-1; ret=Id(reinterpret_cast<const CVector*(__fastcall*)(void*,void*,int16)>(a)(self,nullptr,id)); }
                    else if(nm=="IsScriptFireExtinguished"){ const int16 id=int16(next()%61)-1; ret=reinterpret_cast<bool(__fastcall*)(void*,void*,int16)>(a)(self,nullptr,id); }
                    else if(nm=="RemoveScriptFire"){ const int16 id=int16(next()%61)-1; reinterpret_cast<void(__fastcall*)(void*,void*,int16)>(a)(self,nullptr,id); }
                    else if(nm=="SetScriptFireAudio"){ const int16 id=int16(next()%61)-1; reinterpret_cast<void(__fastcall*)(void*,void*,int16,bool)>(a)(self,nullptr,id,next()%2); }
                    else if(nm=="StartFire-"){ static const int tg[]{0,0,1,1,5,2,3}; void* target=ent[tg[next()%std::size(tg)]].data(); void* creator=next()%3 ? ent[4].data() : nullptr;
                        static const int8 gens[]{0,1,5,100,-1,-128,127}; static const uint32 times[]{7000u,0u,1u,30000u,0x7FFFFFFFu,0xFFFFFFF0u};
                        ret=Id(reinterpret_cast<CFire*(__fastcall*)(void*,void*,CEntity*,CEntity*,float,uint8,uint32,int8)>(a)(self,nullptr,static_cast<CEntity*>(target),static_cast<CEntity*>(creator),
                            uni(0.f,3.f),uint8(next()),next()%2 ? times[next()%std::size(times)] : next()%100000,next()%2 ? gens[next()%std::size(gens)] : int8(next()))); }
                    else if(nm=="StartFire-NoTarget"){ const CVector pt=base+randVec(-30.f,30.f); waterLevel=next()%3==0 ? pt.z : pt.z+float(int(next()%3)-1)*uni(0.f,5.f);
                        void* creator=next()%3 ? entFor(next()%6) : nullptr; static const int8 gens[]{0,1,5,100,-1,-128,127};
                        static const uint32 times[]{20000u,30000u,0u,0x80000000u,0xFFFFFFFFu,0x01000001u,0x7FFFFFFFu};
                        if(next()%4==0)timeMs=0x80000000u|next();
                        ret=Id(reinterpret_cast<CFire*(__fastcall*)(void*,void*,CVector,float,uint8,CEntity*,uint32,int8,uint8)>(a)(self,nullptr,pt,uni(0.f,3.f),uint8(next()),static_cast<CEntity*>(creator),
                            next()%2 ? times[next()%std::size(times)] : next(),next()%2 ? gens[next()%std::size(gens)] : int8(next()),uint8(next()))); }
                    else if(nm=="StartScriptFire"){ const CVector pt=base+randVec(-30.f,30.f); static const int tg[]{-1,-1,0,1,5,2,3}; const int t=tg[next()%std::size(tg)];
                        void* target=t<0 ? nullptr : ent[t].data(); static const int8 gens[]{0,1,5,100,-1,-128,127}; static const int32 sizes[]{1,2,3,0,-1,5,100,0x7FFFFFFF,int32(0x80000000),16777217};
                        ret=reinterpret_cast<int32(__fastcall*)(void*,void*,const CVector&,CEntity*,float,uint8,int8,int32)>(a)(self,nullptr,pt,static_cast<CEntity*>(target),uni(0.f,3.f),uint8(next()),
                            next()%2 ? gens[next()%std::size(gens)] : int8(next()),sizes[next()%std::size(sizes)]); }
                    else { // Update
                        // camara: matriz falsa o placement con heading
                        const CVector camPos=base+randVec(-30.f,30.f); fill(camMat.data(),0x60);
                        At<CVector>(camMat.data(),0)=unitVec(); At<CVector>(camMat.data(),0x10)=unitVec(); At<CVector>(camMat.data(),0x20)=unitVec(); At<CVector>(camMat.data(),0x30)=camPos; At<void*>(camMat.data(),0x40)=nullptr;
                        if(next()%3){ camMatrix=camMat.data(); camPlace=randVec(-5.f,5.f); } else { camMatrix=nullptr; camPlace=camPos; camHeading=uni(-7.f,7.f); }
                        auto* cm=reinterpret_cast<uint8*>(CAM_MAT); At<CVector>(cm,0)=next()%8==0 ? CVector{0.f,0.f,1.f} : unitVec(); At<CVector>(cm,0x10)=unitVec(); At<CVector>(cm,0x20)=unitVec(); At<CVector>(cm,0x30)=camPos;
                        riots=next()%2; groundHit=next()%4!=0; groundEnt=next()%4 ? ent[3].data() : nullptr; At<uint8>(ent[3].data(),0x1E)=uint8(next());
                        groundZ=next()%5==0 ? 0.5f : next()%5==0 ? uni(-5.f,0.5f) : uni(0.5f,40.f); waterLevel=uni(-10.f,10.f);
                        // fuegos agrupados con fuerzas >= 0 (sin el indice -1)
                        const float spread=next()%2 ? 4.f : 12.f; for(int k=0;k<60;++k){ uint8* p=Mgr()+k*0x28; At<CVector>(p,4)=base+randVec(-spread,spread); At<float>(p,0x1C)=strength();
                            if(next()%5==0)p[0]&=0xFE; }
                        reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(self,nullptr);
                    }
                    out["mgr"]=fireBytes(); out["ents"]=entState(); out["fx"]=ReversingProbe::Bytes(fx[0].data(),8);
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
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
