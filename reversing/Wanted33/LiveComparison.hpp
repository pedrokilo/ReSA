#pragma once
// Banco dentro del motor: CWanted (lista de 33). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Los CWanted son falsos (el del jugador tambien: FindPlayerWanted
// devuelve un segundo CWanted falso), igual que los peatones/policias, el peaton del jugador, los vehiculos y los pools
// de peatones y vehiculos (los punteros 0xB74490/0xB74494 se sustituyen solo durante la llamada). Se simulan y trazan:
// FindPlayerPed/Vehicle/Coors/Wanted, CStats (IncrementStat, SetStatValue, SetNewRecordStat), CGangWars::
// GangWarFightingGoingOn, el escaner de la policia (AddAudioEvent), CGameLogic::SetPlayerWantedLevelForForbidden-
// Territories, CPed::IsAlive y CVehicle::IsLawEnforcementVehicle; el printf del EXE se anula. Los cuerpos de la DLL se resuelven antes de parchear
// y nunca se parchea la direccion de un stub. La traza es un array fijo (sin reservas durante la llamada) que se
// convierte a json despues. Se guardan y restauran en cada ejecucion y al final: MaximumWantedLevel, MaximumChaosLevel,
// UseNewsHeliInAdditionToPolice, el reloj, el truco "hago lo que quiero", el estado de CDarkel, la region del tiempo,
// el area actual y los punteros de los pools.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Wanted.h"
#include "World.h"
#include "Stats.h"
#include "GangWars.h"
#include "GameLogic.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace Wanted33Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= objetos falsos =======================
constexpr size_t WSZ=0x2A0, PEDSZ=0x7C4, VEHSZ=0xA18, MATSZ=0x48, NPED=12, NVEH=6, NMAT=NPED+NVEH+1, POOLSZ=0x20;
constexpr size_t ALIVE_OFF=0x7C0, LAW_OFF=0xA10; // bytes libres de los fixtures que leen los stubs de IsAlive/IsLawEnforcementVehicle
alignas(16) inline std::array<uint8,WSZ> wObj{}, pwObj{};
alignas(16) inline std::array<uint8,NPED*PEDSZ> peds{};
alignas(16) inline std::array<uint8,NVEH*VEHSZ> vehs{};
alignas(16) inline std::array<uint8,PEDSZ> playerPed{};
alignas(16) inline std::array<uint8,NMAT*MATSZ> mats{};
alignas(16) inline std::array<uint8,POOLSZ> pedPool{}, vehPool{};
alignas(16) inline std::array<uint8,16> pedMap{}, vehMap{};
alignas(16) inline std::array<CCopPed*,10> copArr{};
inline uint8* W() { return wObj.data(); }
inline uint8* PW() { return pwObj.data(); }
inline uint8* Ped(size_t k) { return peds.data()+k*PEDSZ; }
inline uint8* Veh(size_t k) { return vehs.data()+k*VEHSZ; }
inline uint8* Mat(size_t k) { return mats.data()+k*MATSZ; }

struct Rg { uintptr_t p; size_t n; size_t stride; const char* nm; };
inline json Name(uint32 v) {
    if(!v)return nullptr;
    const Rg regions[]{{uintptr_t(wObj.data()),WSZ,0,"wanted"},{uintptr_t(pwObj.data()),WSZ,0,"player_wanted"},{uintptr_t(peds.data()),peds.size(),PEDSZ,"ped"},
        {uintptr_t(vehs.data()),vehs.size(),VEHSZ,"veh"},{uintptr_t(playerPed.data()),PEDSZ,0,"player"},{uintptr_t(mats.data()),mats.size(),MATSZ,"mat"},{uintptr_t(copArr.data()),sizeof(copArr),0,"cop_array"}};
    for(auto& r:regions){ if(v<r.p||v>=r.p+r.n)continue; const auto o=v-r.p;
        if(r.stride)return std::string(r.nm)+"#"+std::to_string(o/r.stride)+"+"+std::to_string(o%r.stride);
        return std::string(r.nm)+"+"+std::to_string(o); }
    return v;
}

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 { C_FPED, C_FVEH, C_FCOORS, C_FWANTED, C_STAT_INC, C_STAT_SET, C_STAT_REC, C_GANG, C_SCANNER, C_FORBID, C_ALIVE, C_LAWENF };
inline constexpr const char* CODE_NAMES[]{"find_player_ped","find_player_vehicle","find_player_coors","find_player_wanted","inc_stat","set_stat","new_record_stat",
                                          "gang_war","scanner_event","forbidden_territories","is_alive","law_enforcement"};
struct TR { uint32 c,a,b,d,e,f; };
inline std::array<TR,1024> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0,uint32 f=0) { if(ntr<int(tr.size()))tr[ntr++]={c,a,b,d,e,f}; else trOverflow=true; }
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]});
        const bool ptr=e.c==C_FVEH||e.c==C_FPED||e.c==C_FWANTED||e.c==C_SCANNER||e.c==C_ALIVE||e.c==C_LAWENF;
        x.push_back(ptr ? Name(e.a) : json(e.a)); x.push_back(e.b); x.push_back(e.d); x.push_back(e.e); x.push_back(e.f); r.push_back(x); }
    return r;
}

// ======================= stubs =======================
inline uint32 gangMask{}, nGang{};
inline void* vehResult{}; inline void* wantedResult{}; inline CVector coorsResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { T(C_FPED,U(playerPed.data()),uint32(id)); return reinterpret_cast<CPlayerPed*>(playerPed.data()); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote) { T(C_FVEH,U(vehResult),uint32(id),remote); return static_cast<CVehicle*>(vehResult); }
inline CVector* __cdecl FakeFindPlayerCoors(CVector* out,int32 id) { T(C_FCOORS,uint32(id)); *out=coorsResult; return out; }
inline CWanted* __cdecl FakeFindPlayerWanted(int32 id) { T(C_FWANTED,U(wantedResult),uint32(id)); return static_cast<CWanted*>(wantedResult); }
inline void __cdecl FakeIncStat(eStats s,float v) { T(C_STAT_INC,uint32(s),F(v)); }
inline void __cdecl FakeSetStat(eStats s,float v) { T(C_STAT_SET,uint32(s),F(v)); }
inline void __cdecl FakeRecordStat(eStats s,float v) { T(C_STAT_REC,uint32(s),F(v)); }
inline bool __cdecl FakeGangWar() { const bool r=(gangMask>>(nGang++%32))&1; T(C_GANG,r); return r; }
inline void __fastcall FakeScanner(void* self,void*,int32 ev,int32 crime,const CVector& p) { T(C_SCANNER,U(self),uint32(ev),uint32(crime),F(p.x),F(p.y)^F(p.z)*3u); }
inline void __cdecl FakeForbidden(bool imm) { T(C_FORBID,imm); }
inline bool __fastcall FakeIsAlive(void* self,void*) { const bool r=At<uint8>(self,ALIVE_OFF)!=0; T(C_ALIVE,U(self),r); return r; }
inline bool __fastcall FakeLawEnf(void* self,void*) { const bool r=At<uint8>(self,LAW_OFF)!=0; T(C_LAWENF,U(self),r); return r; }
// printf del CRT del EXE (0x821982): solo lo llama el original ("CopPursuit ... messed up"); sin traza
inline int __cdecl FakePrintf(const char*,...) { return 0; }

// ======================= hooks y parches =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
// cuerpo de una funcion de la DLL: sigue los saltos del ILT mientras no vuelvan al EXE
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,33> fns{{
    {"CWanted","Initialise",0x562390,0,-1,128},{"CWanted","Reset",0x562400,0,-1,128},{"CWanted","InitialiseStaticVariables",0x561C70,0,-1,64},
    {"CWanted","UpdateWantedLevel",0x561C90,0,-1,1024},{"CWanted","SetMaximumWantedLevel",0x561E70,0,-1,256},{"CWanted","AreMiamiViceRequired",0x561F30,0,-1,256},
    {"CWanted","AreSwatRequired",0x561F40,0,-1,256},{"CWanted","AreFbiRequired",0x561F60,0,-1,256},{"CWanted","AreArmyRequired",0x561F80,0,-1,256},
    {"CWanted","NumOfHelisRequired",0x561FA0,0,-1,256},{"CWanted","ResetPolicePursuit",0x561FD0,0,-1,64},{"CWanted","UpdateCrimesQ",0x562760,0,-1,512},
    {"CWanted","ClearQdCrimes",0x561FE0,0,-1,128},{"CWanted","AddCrimeToQ",0x562000,0,-1,1024},{"CWanted","ReportCrimeNow",0x562120,0,-1,1024},
    {"CWanted","IsInPursuit",0x562330,0,-1,256},{"CWanted","UpdateEachFrame",0x562360,0,-1,256},{"CWanted","RegisterCrime",0x562410,0,-1,512},
    {"CWanted","RegisterCrime_Immediately",0x562430,0,-1,512},{"CWanted","SetWantedLevel",0x562470,0,-1,512},{"CWanted","CheatWantedLevel",0x562540,0,-1,512},
    {"CWanted","SetWantedLevelNoDrop",0x562570,0,-1,512},{"CWanted","ClearWantedLevelAndGoOnParole",0x5625A0,0,-1,256},{"CWanted","WorkOutPolicePresence",0x5625F0,0,-1,1024},
    {"CWanted","IsClosestCop",0x5627D0,0,-1,1024},{"CWanted","ComputePursuitCopToDisplace",0x562B00,0,-1,1024},{"CWanted","RemovePursuitCop-func",0x562300,0,-1,256},
    {"CWanted","RemovePursuitCop-method",0x562C10,0,-1,256},{"CWanted","RemoveExcessPursuitCops",0x562C40,0,-1,512},{"CWanted","Update",0x562C90,0,-1,1024},
    {"CWanted","CanCopJoinPursuit-func",0x562F60,0,-1,512},{"CWanted","CanCopJoinPursuit-method",0x562FB0,0,-1,512},{"CWanted","SetPursuitCop",0x563060,0,-1,512},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wanted33-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"wanted33-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("wanted33");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"wanted33-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,33> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // estado global guardado: estaticas de CWanted, reloj, truco, CDarkel, region del tiempo, area y pools
    std::vector<Region> regions{{0x8CDEE4,8,{}},{0xB7CB8C,1,{}},{0xB7CB84,4,{}},{0x969171,1,{}},{0x96A704,2,{}},{0xC81314,2,{}},{0xB72914,4,{}},{0xB74490,8,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&FakeFindPlayerPed);
    add(0x56E0D0,uintptr_t(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),&FakeFindPlayerVehicle);
    add(0x56E010,uintptr_t(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),&FakeFindPlayerCoors);
    add(0x56E230,uintptr_t(static_cast<CWanted*(*)(int32)>(&FindPlayerWanted)),&FakeFindPlayerWanted);
    add(0x55C180,uintptr_t(&CStats::IncrementStat),&FakeIncStat);
    add(0x55A070,uintptr_t(&CStats::SetStatValue),&FakeSetStat);
    add(0x55C410,uintptr_t(&CStats::SetNewRecordStat),&FakeRecordStat);
    add(0x443AC0,uintptr_t(&CGangWars::GangWarFightingGoingOn),&FakeGangWar);
    add(0x4E71E0,MAddr(&CAEPoliceScannerAudioEntity::AddAudioEvent),&FakeScanner);
    add(0x441770,uintptr_t(&CGameLogic::SetPlayerWantedLevelForForbiddenTerritories),&FakeForbidden);
    add(0x5E0170,MAddr(&CPed::IsAlive),&FakeIsAlive);
    add(0x6D2370,MAddr(&CVehicle::IsLawEnforcementVehicle),&FakeLawEnf);
    add(0x821982,0,&FakePrintf);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,33> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84);
        auto& maxWanted=*reinterpret_cast<int32*>(0x8CDEE4); auto& maxChaos=*reinterpret_cast<int32*>(0x8CDEE8); auto& newsHeli=*reinterpret_cast<uint8*>(0xB7CB8C);
        auto& cheat=*reinterpret_cast<uint8*>(0x969171); auto& darkel=*reinterpret_cast<uint16*>(0x96A704);
        auto& region=*reinterpret_cast<int16*>(0xC81314); auto& area=*reinterpret_cast<int32*>(0xB72914);
        auto& pedPoolPtr=*reinterpret_cast<void**>(0xB74490); auto& vehPoolPtr=*reinterpret_cast<void**>(0xB74494);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3A77u; if(!st)st=1;
                    ntr=0; trOverflow=false; gangMask=rnd(3)==0 ? next() : 0; nGang=0;

                    // ---- fixtures comunes ----
                    const uint32 now=rnd(8)==0 ? rnd(3000) : rnd(16)==0 ? 0xFFFFFFFFu-rnd(20000) : 1000000+rnd(200000); timeMs=now;
                    const CVector P{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,200.f)};
                    auto randMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,MATSZ); const float a=uni(-3.2f,3.2f), c=std::cos(a), s=std::sin(a);
                        At<CVector>(m,0x00)=CVector{c,s,0.f}; At<CVector>(m,0x10)=CVector{-s,c,0.f}; At<CVector>(m,0x20)=CVector{0.f,0.f,1.f}; At<CVector>(m,0x30)=pos; };
                    auto setPos=[&](uint8* e,size_t matIdx,const CVector& pos){ At<CVector>(e,4)=rnd(3)==0 ? pos : CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)};
                        if(rnd(2)){ randMat(Mat(matIdx),pos); At<void*>(e,0x14)=Mat(matIdx); } else { At<void*>(e,0x14)=nullptr; At<CVector>(e,4)=pos; } };
                    auto around=[&](float lo,float hi){ const float r=uni(lo,hi), a=uni(-3.2f,3.2f); return P+CVector{r*std::cos(a),r*std::sin(a),rnd(3)==0 ? 0.f : uni(-0.3f,0.3f)*r}; };
                    auto copPos=[&]{ switch(rnd(6)){case 0:return P;case 1:return around(0.f,1.f);case 2:return around(1.f,20.f);case 3:return around(20.f,120.f);default:return around(0.f,60.f);} };
                    std::memset(playerPed.data(),0,PEDSZ); setPos(playerPed.data(),NMAT-1,P);
                    for(size_t k=0;k<NPED;++k){ uint8* p=Ped(k); std::memset(p,0,PEDSZ); setPos(p,k,copPos()); At<uint32>(p,0x598)=rnd(5)==0 ? rnd(20) : 6; p[ALIVE_OFF]=uint8(rnd(6)!=0); }
                    for(size_t k=0;k<NVEH;++k){ uint8* v=Veh(k); std::memset(v,0,VEHSZ); setPos(v,NPED+k,copPos());
                        static const int16 models[]{497,596,597,598,599,400,523,497};
                        At<int16>(v,0x22)=models[rnd(8)]; v[0x36]=uint8(rnd(3)==0 ? next() : rnd(8)<<3); At<uint32>(v,0x428)=next(); At<uint32>(v,0x594)=rnd(4)==0 ? rnd(12) : rnd(5); v[LAW_OFF]=uint8(rnd(2)); }
                    // pools falsos: objetos, mapa de ranuras (bit 7 = libre), tamano
                    for(size_t k=0;k<pedMap.size();++k)pedMap[k]=uint8(rnd(4)==0 ? 0x80|rnd(128) : rnd(128));
                    for(size_t k=0;k<vehMap.size();++k)vehMap[k]=uint8(rnd(4)==0 ? 0x80|rnd(128) : rnd(128));
                    std::memset(pedPool.data(),0,POOLSZ); At<void*>(pedPool.data(),0)=peds.data(); At<void*>(pedPool.data(),4)=pedMap.data(); At<int32>(pedPool.data(),8)=int32(rnd(8)==0 ? 0 : 1+rnd(NPED)); At<int32>(pedPool.data(),0xC)=-1;
                    std::memset(vehPool.data(),0,POOLSZ); At<void*>(vehPool.data(),0)=vehs.data(); At<void*>(vehPool.data(),4)=vehMap.data(); At<int32>(vehPool.data(),8)=int32(rnd(8)==0 ? 0 : 1+rnd(NVEH)); At<int32>(vehPool.data(),0xC)=-1;
                    { const uint32 v=rnd(4); vehResult=v==0||v==1 ? nullptr : Veh(rnd(NVEH)); }
                    coorsResult=rnd(4)==0 ? around(0.f,30.f) : P;

                    auto randLevel=[&]{ return rnd(10)==0 ? int32(rnd(6))-2+(rnd(2) ? 9 : 0) : int32(rnd(7)); };  // 0..6, a veces -2..3 o 7..12
                    auto randChaos=[&]{ static const int32 th[]{0,49,50,179,180,549,550,1199,1200,2399,2400,4599,4600,6900,9200,115,365,875,1800,3500};
                        switch(rnd(6)){case 0:return th[rnd(20)];case 1:return int32(rnd(7000));case 2:return int32(rnd(400));case 3:return -int32(rnd(3000));case 4:return int32(next());default:return th[rnd(20)]+int32(rnd(5))-2;} };
                    auto setWanted=[&](uint8* w,bool full){
                        for(size_t k=0;k<WSZ;++k)w[k]=uint8(next());
                        At<int32>(w,0x00)=randChaos(); At<int32>(w,0x04)=rnd(3)==0 ? randChaos() : 0;
                        At<uint32>(w,0x08)=rnd(4)==0 ? next() : now-rnd(3000); At<uint32>(w,0x0C)=next();
                        At<uint32>(w,0x10)=rnd(4)==0 ? next() : now-rnd(40000);
                        { static const float mul[]{1.f,1.f,0.5f,2.f,0.f,-1.f,1.5f}; At<float>(w,0x14)=rnd(4)==0 ? uni(-3.f,6.f) : mul[rnd(7)]; }
                        w[0x19]=uint8(rnd(12)); w[0x1A]=uint8(rnd(4)); w[0x1E]=uint8(rnd(3)==0 ? next() : next()&0x38);
                        At<uint32>(w,0x20)=rnd(4)==0 ? next() : rnd(600); At<uint32>(w,0x24)=rnd(4)==0 ? next() : now-rnd(3000); w[0x28]=uint8(rnd(2));
                        At<int32>(w,0x2C)=randLevel(); At<int32>(w,0x30)=rnd(3)==0 ? randLevel() : 0;
                        for(int c=0;c<16;++c){ uint8* q=w+0x34+c*0x1C; At<uint32>(q,0)=rnd(3)==0 ? 0 : rnd(10)==0 ? rnd(40) : rnd(24); At<uint32>(q,4)=rnd(4);
                            At<uint32>(q,8)=rnd(5)==0 ? next() : now-rnd(15000); At<CVector>(q,0xC)=around(0.f,100.f); q[0x18]=uint8(rnd(2)); q[0x19]=uint8(rnd(2)); }
                        // policias: punteros a peatones falsos con huecos; contador a veces incoherente
                        uint32 cnt=0; const uint32 fill=rnd(5);
                        for(int c=0;c<10;++c){ const bool used=fill==0 ? true : fill==1 ? c<int(rnd(11)) : rnd(3)!=0; At<void*>(w,0x1F4+4*c)=full&&used ? Ped(rnd(NPED)) : nullptr; cnt+=At<void*>(w,0x1F4+4*c)!=nullptr; }
                        // sin duplicados para que las bajas terminen como en el juego
                        for(int c=0;c<10;++c)for(int d=0;d<c;++d)if(At<void*>(w,0x1F4+4*c)&&At<void*>(w,0x1F4+4*c)==At<void*>(w,0x1F4+4*d))At<void*>(w,0x1F4+4*c)=nullptr;
                        cnt=0; for(int c=0;c<10;++c)cnt+=At<void*>(w,0x1F4+4*c)!=nullptr;
                        w[0x18]=uint8(rnd(4)==0 ? rnd(11) : cnt); w[0x298]=uint8(rnd(2));
                    };
                    // con la lista llena, el primer policia muerto o lejos: si no, RemoveExcessPursuitCops no termina (tampoco en el EXE)
                    auto safeFull=[&](uint8* w){ bool fullList=true; for(int c=0;c<10;++c)fullList&=At<void*>(w,0x1F4+4*c)!=nullptr;
                        if(fullList){ uint8* cop=At<uint8*>(w,0x1F4); At<void*>(cop,0x14)=nullptr; At<CVector>(cop,4)=around(5.f,50.f); } };
                    setWanted(W(),true); setWanted(PW(),false); wantedResult=rnd(6)==0 ? W() : PW();
                    maxWanted=rnd(8)==0 ? int32(rnd(10))-2 : int32(rnd(7)); maxChaos=rnd(4)==0 ? randChaos() : int32(std::array<int32,7>{0,115,365,875,1800,3500,6900}[rnd(7)]);
                    if(rnd(3)==0)maxChaos=9200;
                    newsHeli=uint8(rnd(2)); cheat=uint8(rnd(8)==0); darkel=uint16(rnd(3)==0 ? 1 : rnd(4));
                    region=int16(rnd(8)==0 ? int16(next()) : int16(rnd(5))); area=rnd(3)==0 ? int32(rnd(18)) : 0;

                    void* const A=reinterpret_cast<void*>(fn.address);
                    json ret=nullptr; bool switches=false; json extra=json::object();
                    auto on=[&]{ pedPoolPtr=pedPool.data(); vehPoolPtr=vehPool.data(); for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); std::memcpy(&pedPoolPtr,regions.back().saved.data(),4); std::memcpy(&vehPoolPtr,regions.back().saved.data()+4,4); };
                    auto randCop=[&]()->void*{ switch(rnd(5)){case 0:return nullptr;case 1:return Ped(rnd(NPED));case 2:return playerPed.data();default:{ void* c=At<void*>(W(),0x1F4+4*rnd(10)); return c ? c : Ped(rnd(NPED)); }} };
                    auto randCrimeType=[&]()->int32{ switch(rnd(8)){case 0:return int32(rnd(2));case 1:return int32(23+rnd(4));case 2:return int32(next());default:return int32(2+rnd(21));} };
                    using ThisV=void(__thiscall*)(void*);

                    if(nm=="Initialise"||nm=="Reset"||nm=="UpdateWantedLevel"||nm=="ResetPolicePursuit"||nm=="UpdateCrimesQ"||nm=="ClearQdCrimes"||nm=="ClearWantedLevelAndGoOnParole"||nm=="Update"||nm=="RemoveExcessPursuitCops"){
                        if(nm=="RemoveExcessPursuitCops")safeFull(W());
                        on(); reinterpret_cast<ThisV>(A)(W()); off(); }
                    else if(nm=="InitialiseStaticVariables"){ on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="SetMaximumWantedLevel"){ const int32 l=rnd(4)==0 ? int32(rnd(14))-3 : int32(rnd(7)); on(); reinterpret_cast<void(__cdecl*)(int32)>(A)(l); off(); }
                    else if(nm.rfind("Are",0)==0){ on(); ret=reinterpret_cast<bool(__thiscall*)(void*)>(A)(W()); off(); }
                    else if(nm=="NumOfHelisRequired"){ on(); ret=reinterpret_cast<int32(__thiscall*)(void*)>(A)(W()); off(); }
                    else if(nm=="AddCrimeToQ"||nm=="RegisterCrime"||nm=="RegisterCrime_Immediately"){
                        int32 type=randCrimeType(); uint32 id=rnd(4);
                        if(rnd(3)==0){ const uint8* q=W()+0x34+rnd(16)*0x1C; type=At<int32>(const_cast<uint8*>(q),0); id=At<uint32>(const_cast<uint8*>(q),4); }
                        if(rnd(10)==0){ for(int c=0;c<16;++c)if(At<uint32>(W(),0x34+c*0x1C)==0)At<uint32>(W(),0x34+c*0x1C)=1+rnd(22); } // cola llena
                        const CVector pos=around(0.f,80.f); const bool rep=rnd(2)!=0, care=rnd(2)!=0;
                        on();
                        if(nm=="AddCrimeToQ")ret=reinterpret_cast<bool(__thiscall*)(void*,int32,uint32,const CVector*,bool,bool)>(A)(W(),type,id,&pos,rep,care);
                        else reinterpret_cast<void(__thiscall*)(void*,int32,const CVector*,uint32,bool)>(A)(W(),type,&pos,id,care);
                        off(); }
                    else if(nm=="ReportCrimeNow"){ const int32 type=randCrimeType(); const CVector pos=around(0.f,80.f); const bool care=rnd(2)!=0;
                        on(); reinterpret_cast<void(__thiscall*)(void*,int32,const CVector*,bool)>(A)(W(),type,&pos,care); off(); }
                    else if(nm=="IsInPursuit"){ void* c=randCop(); on(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(A)(W(),c); off(); }
                    else if(nm=="UpdateEachFrame"){ on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="SetWantedLevel"||nm=="CheatWantedLevel"||nm=="SetWantedLevelNoDrop"){ const int32 l=rnd(5)==0 ? int32(rnd(14))-3 : int32(rnd(7));
                        on(); reinterpret_cast<void(__thiscall*)(void*,int32)>(A)(W(),l); off(); }
                    else if(nm=="WorkOutPolicePresence"){ const CVector pos=rnd(3)==0 ? around(0.f,40.f) : P; static const float rads[]{18.f,18.f,0.f,-1.f,5.f,60.f};
                        const float r=rnd(3)==0 ? uni(0.f,150.f) : rads[rnd(6)];
                        on(); ret=reinterpret_cast<int32(__cdecl*)(CVector,float)>(A)(pos,r); off(); }
                    else if(nm=="IsClosestCop"){ void* c=randCop(); const int32 n=rnd(6)==0 ? int32(rnd(3))-2 : rnd(4)==0 ? int32(rnd(11)) : int32(1+rnd(3));
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*,int32)>(A)(W(),c,n); off(); }
                    else if(nm=="ComputePursuitCopToDisplace"){ void* c=randCop(); if(c==playerPed.data())c=nullptr;
                        on(); ret=Name(U(reinterpret_cast<void*(__cdecl*)(void*,void*)>(A)(c,W()+0x1F4))); off(); }
                    else if(nm=="RemovePursuitCop-func"){ void* c=randCop(); std::memcpy(copArr.data(),W()+0x1F4,sizeof(copArr)); uint8 counter=uint8(rnd(12));
                        on(); reinterpret_cast<void(__cdecl*)(void*,void*,uint8*)>(A)(c,copArr.data(),&counter); off();
                        extra["counter"]=counter; extra["array"]=json::array(); for(auto* p:copArr)extra["array"].push_back(Name(U(p))); }
                    else if(nm=="RemovePursuitCop-method"){ void* c=randCop(); on(); reinterpret_cast<void(__thiscall*)(void*,void*)>(A)(W(),c); off(); }
                    else if(nm=="CanCopJoinPursuit-func"){ void* c=randCop(); if(c==playerPed.data())c=nullptr; std::memcpy(copArr.data(),W()+0x1F4,sizeof(copArr));
                        uint8 counter=uint8(rnd(4)==0 ? rnd(12) : W()[0x18]); const uint8 mx=uint8(rnd(12));
                        on(); ret=reinterpret_cast<bool(__cdecl*)(void*,uint8,void*,uint8*)>(A)(c,mx,copArr.data(),&counter); off();
                        extra["counter"]=counter; extra["array"]=json::array(); for(auto* p:copArr)extra["array"].push_back(Name(U(p))); }
                    else if(nm=="CanCopJoinPursuit-method"||nm=="SetPursuitCop"){ void* c=randCop(); if(c==playerPed.data())c=nullptr;
                        on(); ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(A)(W(),c); off(); }

                    // ---- volcado ----
                    auto dump=[&](uint8* w){ json cops=json::array(); for(int c=0;c<10;++c)cops.push_back(Name(At<uint32>(w,0x1F4+4*c)));
                        json crimes=json::array(); for(int c=0;c<16;++c){ const uint8* q=w+0x34+c*0x1C; crimes.push_back(json::array({At<uint32>(const_cast<uint8*>(q),0),q[0x18]})); }
                        return json{{"head",ReversingProbe::Bytes(w,0x34)},{"crimes",crimes},{"cops",cops},{"tail",w[0x298]},{"hash",Hash(w,WSZ)}}; };
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},{"gang",nGang},
                            {"wanted",dump(W())},{"player_wanted",Hash(PW(),WSZ)},{"peds",Hash(peds.data(),peds.size())},{"vehs",Hash(vehs.data(),vehs.size())},
                            {"globals",json::array({maxWanted,maxChaos,newsHeli,timeMs,cheat,darkel,region,area})}};
                    RestoreAll();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["overflow"]==false;
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
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",AllRestored()}});
}
}
