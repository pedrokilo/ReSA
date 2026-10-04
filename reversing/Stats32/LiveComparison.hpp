#pragma once
// Banco dentro del motor: CStats (lista de 32, 32 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas e identicos. Se rellenan con valores deterministas todos los arrays de
// CStats (mensajes, nombre de la ultima mision, intentos, emisoras, muertes por tipo, StatReactionValue,
// StatTypesInt/Float y contadores), CWorld::Players (peatones falsos con salud y CPlayerPedData), el jugador en
// foco, CTimer (paso y milisegundos), las banderas de DisplayScriptStatUpdateMessage (mando 0, bShowUpdateStats,
// pantalla panoramica, ayuda del HUD, menus) y la version alemana. Se simulan y trazan: FindPlayerPed,
// CheckForStatsMessage (con un hash de las estadisticas en cada llamada), LoadStatUpdateConditions,
// PopulateFavoriteRadioStationList, UpdateFatAndMuscleStats, UpdateStatsAddToHealth,
// CHud::SetHelpMessageStatUpdate, CFileMgr (SetDir/OpenFile/CloseFile) y CFileLoader::LoadLine (lineas en memoria:
// nunca se lee DATA\AR_STATS.DAT) y el bufer de guardado (nunca ficheros de partida). Los cuerpos de la DLL se
// resuelven antes de parchear y nunca se parchea la direccion de un stub. La traza es un array fijo (sin reservas
// durante la llamada) que se convierte a json despues. Todas las globales tocadas se guardan y se restauran en cada
// ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Stats.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Hud.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "GenericGameStorage.h"
#include "CutsceneMgr.h"
#include "Entity/Vehicle/Bike.h"
#include "../ProbeRuntime.hpp"

namespace Stats32Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* b,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 Fnv(const void* p,size_t n) { uint32 h=2166136261u; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=16777619u;} return h; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// ======================= memoria =======================
constexpr uintptr_t STATS=0xB78200; constexpr size_t STATSZ=0xB79518-0xB78200;   // mensajes ... ultimo arma disparada
constexpr uintptr_t MSGS=0xB78200, NAME=0xB78A00, TIMES=0xB78CC8, RADIO=0xB78E58, PEDSK=0xB78E90, REACT=0xB78F10,
                    INTS=0xB79000, FLOATS=0xB79380, TAIL=0xB794D0;
constexpr uintptr_t PLAYERS=0xB7CD98; constexpr size_t PLSZ=0x190;
constexpr uintptr_t GSTRING=0xB71670;
constexpr size_t PEDSZ=0x800, BIKESZ=0x900;
alignas(16) inline std::array<uint8,2*PEDSZ> peds{};
alignas(16) inline std::array<uint8,BIKESZ> bike{};
alignas(16) inline std::array<float,4> thr{};
alignas(16) inline std::array<uint8,64> fileObj{};
inline std::array<std::array<char,96>,16> lines{}; inline int nLines{}, lineCursor{};
inline uint8* Ped(int k) { return peds.data()+size_t(k)*PEDSZ; }
inline uint8* Plr(int k) { return reinterpret_cast<uint8*>(PLAYERS)+size_t(k)*PLSZ; }
inline float* Fl() { return reinterpret_cast<float*>(FLOATS); }
inline int32* In() { return reinterpret_cast<int32*>(INTS); }

struct Rg { uintptr_t p; size_t n; const char* nm; };
inline json Name(uint32 v) {
    if(!v)return nullptr;
    static const Rg regions[]{{MSGS,0x800,"msgs"},{NAME,8,"name"},{TIMES,0x190,"times"},{RADIO,0x38,"radio"},{PEDSK,0x80,"peds_killed"},
        {REACT,59*4,"react"},{INTS,223*4,"ints"},{FLOATS,82*4,"floats"},{STATS,STATSZ,"stats"},{GSTRING,352,"gstring"},
        {uintptr_t(fileObj.data()),64,"file"},{uintptr_t(thr.data()),16,"thr"},{uintptr_t(peds.data()),2*PEDSZ,"ped"},{uintptr_t(bike.data()),BIKESZ,"bike"},
        {PLAYERS,2*PLSZ,"player"}};
    for(auto& r:regions)if(v>=r.p&&v<r.p+r.n)return std::string(r.nm)+"+"+std::to_string(v-r.p);
    return v;
}

// ======================= traza fija (sin reservas durante la llamada) =======================
enum Code : uint32 { C_FPP, C_CHECK, C_LOADCOND, C_POPULATE, C_FATMUSCLE, C_ADDHEALTH, C_HELPSTAT, C_SETDIR, C_OPEN, C_LOADLINE, C_CLOSE, C_SAVE, C_LOAD };
inline constexpr const char* CODE_NAMES[]{"find_player_ped","check_stats_message","load_stat_update_conditions","populate_radio","fat_and_muscle",
    "add_to_health","help_stat_update","set_dir","open_file","load_line","close_file","save_wb","load_wb"};
struct TR { uint32 c,a,b,d,e; };
inline std::array<TR,1024> tr{}; inline int ntr{}; inline bool trOverflow{};
inline void T(uint32 c,uint32 a=0,uint32 b=0,uint32 d=0,uint32 e=0) { if(ntr<int(tr.size()))tr[ntr++]={c,a,b,d,e}; else trOverflow=true; }
inline json TraceJson() {
    json r=json::array();
    for(int k=0;k<ntr;++k){ const auto& e=tr[k]; json x=json::array({CODE_NAMES[e.c]});
        const bool ptr=e.c==C_OPEN||e.c==C_LOADLINE||e.c==C_CLOSE||e.c==C_SAVE||e.c==C_LOAD;
        x.push_back(ptr ? Name(e.a) : json(e.a)); x.push_back(e.b); x.push_back(e.d); x.push_back(e.e);
        r.push_back(x); }
    return r;
}

// ======================= stubs =======================
inline uint32 loadCounter{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { const int k=id<0 ? int(*reinterpret_cast<uint8*>(0xB7CD74)) : id; T(C_FPP,uint32(id)); return At<CPlayerPed*>(Plr(k&1),0); }
// en cada CheckForStatsMessage se guarda un hash de StatTypesInt + StatTypesFloat (estado intermedio)
inline void __cdecl FakeCheck() { T(C_CHECK,Fnv(reinterpret_cast<void*>(INTS),0x4C8),Fnv(reinterpret_cast<void*>(REACT),59*4)); }
inline void __cdecl FakeLoadCond() { T(C_LOADCOND); }
inline bool __cdecl FakePopulate() { T(C_POPULATE); return true; }
inline void __cdecl FakeFatMuscle(uint32 v) { T(C_FATMUSCLE,v); }
inline void __cdecl FakeAddHealth(uint32 v) { T(C_ADDHEALTH,v); }
// estado (uint8) y estadistica (uint16): la DLL los empuja con bytes altos sin definir
inline void __cdecl FakeHelpStat(uint32 state,uint32 stat,float diff,float mx) { T(C_HELPSTAT,state&0xFF,stat&0xFFFF,F(diff),F(mx)); }
inline int32 __cdecl FakeSetDir(const char* p) { T(C_SETDIR,Fnv(p,std::strlen(p)),uint32(std::strlen(p))); return 0; }
inline FILE* __cdecl FakeOpen(const char* p,const char* m) { T(C_OPEN,U(fileObj.data()),Fnv(p,std::strlen(p)),Fnv(m,std::strlen(m))); return reinterpret_cast<FILE*>(fileObj.data()); }
inline char* __cdecl FakeLoadLine(FILE* f) { char* r=lineCursor<nLines ? lines[lineCursor].data() : nullptr; T(C_LOADLINE,U(f),uint32(lineCursor)); if(r)++lineCursor; return r; }
inline int32 __cdecl FakeClose(FILE* f) { T(C_CLOSE,U(f)); return 0; }
inline bool __cdecl FakeSaveWB(void* p,int32 n) { T(C_SAVE,U(p),uint32(n),n>0 ? Fnv(p,size_t(n)) : 0); return true; }
inline bool __cdecl FakeLoadWB(void* p,int32 n) { auto* b=static_cast<uint8*>(p); for(int32 k=0;k<n;++k)b[k]=uint8(loadCounter++*37u+11u); T(C_LOAD,U(p),uint32(n)); return true; }

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

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,32> fns{{
    {"CStats","CheckForThreshold",0x5595F0,0,-1,512},{"CStats","ConvertToMins",0x559540,0,-1,256},
    {"CStats","ConvertToSecs",0x559560,0,-1,256},{"CStats","DecrementStat",0x559FA0,0,-1,1024},
    {"CStats","DisplayScriptStatUpdateMessage",0x55B980,0,-1,1024},{"CStats","FindCriminalRatingNumber",0x559080,0,-1,1024},
    {"CStats","FindMaxNumberOfGroupMembers",0x559A50,0,-1,256},{"CStats","GetPercentageProgress",0x5591E0,0,-1,256},
    {"CStats","GetStatID",0x558DE0,0,-1,256},{"CStats","GetStatValue--OG",0x558E40,0,-1,512},
    {"CStats","GetTimesMissionAttempted",0x558E70,0,-1,128},{"CStats","IncrementStat",0x55C180,0,-1,1024},
    {"CStats","Init",0x55C0C0,0,-1,64},{"CStats","IsStatCapped",0x559630,0,-1,512},
    {"CStats","IsStatFloat",0x558E30,0,-1,256},{"CStats","Load",0x5D3BF0,0,-1,64},
    {"CStats","LoadActionReactionStats",0x5599B0,0,-1,256},{"CStats","ModifyStat",0x55D090,0,-1,512},
    {"CStats","ProcessReactionStatsOnDecrement",0x559730,0,-1,256},{"CStats","ProcessReactionStatsOnIncrement",0x55B900,0,-1,512},
    {"CStats","RegisterBestPosition",0x55A160,0,-1,256},{"CStats","RegisterFastestTime",0x55A0B0,0,-1,512},
    {"CStats","RegisterMissionAttempted",0x558E80,0,-1,128},{"CStats","RegisterMissionPassed",0x558EA0,0,-1,128},
    {"CStats","SafeToShowThisStat",0x559590,0,-1,512},{"CStats","Save",0x5D3B40,0,-1,128},
    {"CStats","SetNewRecordStat",0x55C410,0,-1,512},{"CStats","SetStatValue",0x55A070,0,-1,512},
    {"CStats","UpdateStatsWhenFighting",0x55CFA0,0,-1,128},{"CStats","UpdateStatsWhenOnMotorBike",0x55CD60,0,-1,512},
    {"CStats","UpdateStatsWhenRunning",0x55C6F0,0,-1,256},{"CStats","UpdateStatsWhenSprinting",0x55C660,0,-1,512},
}};

struct Region { uintptr_t at; size_t n; std::vector<uint8> saved; };

// estadisticas con comportamiento especial (limitadas, reacciones, mensajes, byte bajo)
inline constexpr uint32 INTERESTING[]{0,1,21,22,23,24,25,64,65,66,67,69,70,71,72,73,74,75,76,77,78,79,80,81,82,100,119,120,124,126,128,133,135,137,
    151,152,153,158,159,160,164,165,167,168,176,177,179,205,206,207,208,209,210,223,225,229,230,233,241,242,243,244,245,252,253,254,255,256,257,
    278,335,336,342,377,421};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"stats32-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"stats32-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("stats32");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"stats32-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,32> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // estado global guardado
    std::vector<Region> regions{{STATS,STATSZ,{}},{0x8CDE56,1,{}},{PLAYERS,2*PLSZ,{}},{0xB7CD74,1,{}},{0xB7CB5C,4,{}},{0xB7CB84,4,{}},
        {GSTRING,352,{}},{0xB73458,0x134,{}},{0xB6F065,1,{}},{0xBA82E3,1,{}},{0xBAA474,4,{}},{0xB9B7ED,1,{}},{0x96918C,1,{}}};
    for(auto& r:regions)r.saved.assign(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.n);
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.at),r.saved.data(),r.n); };
    auto AllRestored=[&]{ for(auto& r:regions)if(std::memcmp(reinterpret_cast<void*>(r.at),r.saved.data(),r.n))return false; return true; };

    // parches: primero se resuelven todos los cuerpos de la DLL, despues se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,auto stub){ void* s=reinterpret_cast<void*>(stub); if(exeAddr)plan.push_back({exeAddr,s}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,s}); } };
    add(0x56E210,uintptr_t(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&FakeFindPlayerPed);
    add(0x559760,uintptr_t(&CStats::CheckForStatsMessage),&FakeCheck);
    add(0x559860,uintptr_t(&CStats::LoadStatUpdateConditions),&FakeLoadCond);
    add(0x558EC0,uintptr_t(&CStats::PopulateFavoriteRadioStationList),&FakePopulate);
    add(0x55C470,uintptr_t(&CStats::UpdateFatAndMuscleStats),&FakeFatMuscle);
    add(0x55D030,uintptr_t(&CStats::UpdateStatsAddToHealth),&FakeAddHealth);
    add(0x588D40,uintptr_t(&CHud::SetHelpMessageStatUpdate),&FakeHelpStat);
    add(0x5387D0,uintptr_t(&CFileMgr::SetDir),&FakeSetDir);
    add(0x538900,uintptr_t(&CFileMgr::OpenFile),&FakeOpen);
    add(0x5389D0,uintptr_t(&CFileMgr::CloseFile),&FakeClose);
    add(0x536F80,uintptr_t(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),&FakeLoadLine);
    add(0x5D1270,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),&FakeSaveWB);
    add(0x5D1300,uintptr_t(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),&FakeLoadWB);
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::array<bool,32> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto rnd=[&](uint32 n){ return next()%n; };
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                const float qnan=std::numeric_limits<float>::quiet_NaN();
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A7Eu; if(!st)st=1;
                    ntr=0; trOverflow=false; loadCounter=0; lineCursor=0; nLines=0;

                    // ---- fixtures comunes ----
                    auto randFloatStat=[&]{ switch(rnd(12)){ case 0: return 0.f; case 1: return 1000.f; case 2: return uni(-100.f,0.f); case 3: return rnd(4)==0 ? qnan : uni(990.f,1010.f);
                        case 4: return float(rnd(2000)); default: return uni(0.f,1200.f); } };
                    auto randIntStat=[&]{ switch(rnd(10)){ case 0: return 0; case 1: return int32(rnd(2000))-1000; case 2: return int32(next()>>4)-(1<<27); case 3: return 1000+int32(rnd(3))-1;
                        case 4: return int32(rnd(70000)); default: return int32(rnd(1500)); } };
                    auto fillStats=[&]{
                        auto* m=reinterpret_cast<uint8*>(MSGS); for(size_t k=0;k<0x800;++k)m[k]=uint8(next());
                        auto* nmb=reinterpret_cast<char*>(NAME); for(int k=0;k<8;++k)nmb[k]=char('A'+rnd(26));
                        auto* tm=reinterpret_cast<int32*>(TIMES); for(int k=0;k<100;++k)tm[k]=rnd(4)==0 ? -1 : rnd(8)==0 ? int32(next()) : int32(rnd(12));
                        auto* rd=reinterpret_cast<int32*>(RADIO); for(int k=0;k<14;++k)rd[k]=int32(rnd(5000));
                        auto* pk=reinterpret_cast<int32*>(PEDSK); for(int k=0;k<32;++k)pk[k]=int32(rnd(300));
                        auto* rv=reinterpret_cast<float*>(REACT); for(int k=0;k<59;++k)rv[k]=rnd(10)==0 ? 0.f : float(int32(rnd(400))-40)/8.f;
                        for(int k=0;k<223;++k)In()[k]=randIntStat();
                        for(int k=0;k<82;++k)Fl()[k]=randFloatStat();
                        *reinterpret_cast<uint8*>(0xB794D4)=uint8(rnd(8)==0);
                        for(uintptr_t a=0xB794D8;a<0xB79518;a+=4){ uint32 v; switch(rnd(6)){ case 0: v=0; break; case 1: v=0x80000000u+rnd(100000); break; case 2: v=0xFFFFFFFFu-rnd(50); break;
                            default: v=rnd(20000); } *reinterpret_cast<uint32*>(a)=v; }
                    };
                    auto setPlayers=[&]{
                        std::memset(Plr(0),0,2*PLSZ); std::memset(peds.data(),0,peds.size());
                        for(int k=0;k<2;++k){ uint8* pl=Plr(k); At<void*>(pl,0)=Ped(k);
                            At<int32>(pl,0xB8)=rnd(6)==0 ? int32(next()) : int32(rnd(3000000))-200000;
                            At<int16>(pl,0x144)=rnd(5)==0 ? int16(next()) : int16(int32(rnd(80))-10);
                            pl[0x14F]=uint8(rnd(4)==0 ? next() : 100+rnd(100));
                            pl[4+0x43]=uint8(rnd(4)==0 ? next() : rnd(10));
                            At<float>(Ped(k),0x540)=rnd(8)==0 ? 0.f : uni(-10.f,260.f);
                            At<void*>(Ped(k),0x480)=pl+4; }
                        focus=uint8(rnd(2));
                    };
                    auto setMisc=[&]{
                        *reinterpret_cast<float*>(0xB7CB5C)=rnd(10)==0 ? -uni(0.f,2.f) : uni(0.f,3.f);
                        *reinterpret_cast<uint32*>(0xB7CB84)=1000000+rnd(100000);
                        *reinterpret_cast<uint8*>(0xB73458+0x117)=uint8(rnd(8)==0);
                        *reinterpret_cast<uint8*>(0x8CDE56)=uint8(rnd(8)!=0);
                        *reinterpret_cast<uint8*>(0xB6F065)=uint8(rnd(8)==0);
                        *reinterpret_cast<int32*>(0xBAA474)=rnd(8)==0 ? int32(1+rnd(3)) : 0;
                        *reinterpret_cast<uint8*>(0xBA82E3)=uint8(rnd(8)==0 ? 1+rnd(2) : 0);
                        *reinterpret_cast<uint8*>(0xB9B7ED)=uint8(rnd(2));
                        *reinterpret_cast<uint8*>(0x96918C)=uint8(rnd(4)==0);
                    };
                    auto setLines=[&]{
                        nLines=int(rnd(14));
                        for(int k=0;k<nLines;++k){ auto& l=lines[k]; l.fill(0);
                            const int32 id=rnd(10)==0 ? int32(rnd(40))-30 : rnd(10)==0 ? 59+int32(rnd(40)) : int32(rnd(59));
                            const float v=float(int32(rnd(2000))-400)/8.f;
                            switch(rnd(10)){
                            case 0: std::snprintf(l.data(),l.size(),"# comentario %d",int(rnd(100))); break;
                            case 1: break; // linea vacia
                            case 2: std::snprintf(l.data(),l.size(),"abc %d",int(rnd(10))); break;          // sscanf no lee nada
                            case 3: std::snprintf(l.data(),l.size(),"%d",int(id)); break;                    // solo el id
                            case 4: std::snprintf(l.data(),l.size(),"%d SOLO_NOMBRE",int(id)); break;        // id y nombre
                            default: std::snprintf(l.data(),l.size(),"%d STAT_%d %g",int(id),int(rnd(99)),double(v)); } }
                    };
                    auto pickStat=[&]()->uint32 { switch(rnd(10)){ case 0: case 1: case 2: case 3: return INTERESTING[rnd(std::size(INTERESTING))];
                        case 7: { static constexpr uint32 low[]{278,421,256+167,256+165,256+21,256+64}; return low[rnd(std::size(low))]; }
                        case 8: return 82+rnd(38); default: return rnd(343); } };
                    auto pickValue=[&]()->float { static const float pv[]{-5.f,-0.5f,0.f,0.25f,0.5f,1.f,1.5f,2.75f,23.f,99.5f,500.f,999.f,1000.f,1500.f,32767.5f,40000.f,70000.f,1e6f,-40000.f};
                        switch(rnd(8)){ case 0: return rnd(6)==0 ? qnan : pv[rnd(std::size(pv))]; case 1: case 2: case 3: return pv[rnd(std::size(pv))]; default: return uni(-50.f,1500.f); } };

                    fillStats(); setPlayers(); setMisc(); thr.fill(0.f); std::memset(bike.data(),0,bike.size());

                    void* const A=reinterpret_cast<void*>(fn.address);
                    json ret=nullptr; bool switches=false;
                    auto on=[&]{ for(auto* h:hooks)h->State(candidate); switches=ReversingProbe::JumpsToDll(fn.address)==candidate; };
                    auto off=[&]{ for(auto* h:hooks)h->State(false); };
                    json extra=json::object();

                    if(nm=="CheckForThreshold"){ thr[0]=rnd(10)==0 ? qnan : uni(-200.f,1200.f); const float range=rnd(10)==0 ? qnan : rnd(3)==0 ? thr[0]+float(int32(rnd(161))-80)*0.5f : uni(-200.f,1200.f);
                        on(); ret=reinterpret_cast<bool(__cdecl*)(float*,float)>(A)(thr.data(),range); off(); extra["value"]=F(thr[0]); }
                    else if(nm=="ConvertToMins"||nm=="ConvertToSecs"){ const int32 v=rnd(6)==0 ? int32(next()) : rnd(4)==0 ? int32(rnd(200))-100 : int32(rnd(20000));
                        on(); ret=reinterpret_cast<int32(__cdecl*)(int32)>(A)(v); off(); }
                    else if(nm=="DecrementStat"||nm=="IncrementStat"||nm=="ModifyStat"||nm=="SetNewRecordStat"||nm=="SetStatValue"){
                        const uint32 s=pickStat(); float v=pickValue(); if(nm=="ModifyStat"&&rnd(2))v=-v;
                        if(s==165||s==421)In()[45]=rnd(3)==0 ? int32(rnd(3000)) : int32(rnd(4000))-2000; // energia
                        on(); reinterpret_cast<void(__cdecl*)(uint32,float)>(A)(s,v); off(); extra["args"]=json::array({s,F(v)}); }
                    else if(nm=="DisplayScriptStatUpdateMessage"){ const uint32 s=rnd(4)==0 ? pickStat() : INTERESTING[rnd(std::size(INTERESTING))];
                        const float v=rnd(4)==0 ? pickValue() : (rnd(2) ? 1.f : uni(0.f,3.f)); const uint32 state=rnd(2);
                        if(rnd(6)==0)At<void*>(Plr(focus),0)=nullptr;
                        if(rnd(3)==0){ *reinterpret_cast<uint8*>(0xB73458+0x117)=0; *reinterpret_cast<uint8*>(0x8CDE56)=1; *reinterpret_cast<uint8*>(0xB6F065)=0; *reinterpret_cast<int32*>(0xBAA474)=0;
                            *reinterpret_cast<uint8*>(0xB794D4)=0; *reinterpret_cast<uint8*>(0xBA82E3)=0; }
                        if(rnd(3)==0)Fl()[64]=float(rnd(1000));
                        on(); reinterpret_cast<void(__cdecl*)(uint32,uint32,float)>(A)(state,s,v); off(); extra["args"]=json::array({state,s,F(v)}); }
                    else if(nm=="FindCriminalRatingNumber"){
                        if(rnd(3)==0)In()[126-120]=100+int32(rnd(3))-1;
                        if(rnd(4)==0)Fl()[1]=0.f; else if(rnd(2))Fl()[1]=float(1+rnd(200));
                        if(rnd(3)==0)In()[137-120]=0;
                        on(); ret=reinterpret_cast<int32(__cdecl*)()>(A)(); off(); }
                    else if(nm=="FindMaxNumberOfGroupMembers"){ static const float edges[]{10.f,60.f,160.f,330.f,540.f,800.f};
                        Fl()[64]=rnd(3)==0 ? edges[rnd(6)] : rnd(12)==0 ? qnan : uni(-10.f,1000.f);
                        on(); ret=reinterpret_cast<int32(__cdecl*)()>(A)(); off(); }
                    else if(nm=="GetPercentageProgress"){ if(rnd(4)==0)Fl()[1]=0.f; else if(rnd(2))Fl()[1]=float(1+rnd(200)); if(rnd(3)==0)Fl()[0]=Fl()[1]*uni(0.f,1.5f);
                        on(); ret=F(reinterpret_cast<float(__cdecl*)()>(A)()); off(); }
                    else if(nm=="GetStatID"){ const uint32 s=rnd(2) ? pickStat() : rnd(65536);
                        on(); const char* r=reinterpret_cast<const char*(__cdecl*)(uint32)>(A)(s); off(); ret=Name(U(r)); extra["text"]=std::string(reinterpret_cast<const char*>(GSTRING)); }
                    else if(nm=="GetStatValue--OG"){ const uint32 s=pickStat(); if(rnd(4)==0&&s>=82)reinterpret_cast<int32*>(0xB78E20)[s]=int32(next());
                        on(); ret=F(reinterpret_cast<float(__cdecl*)(uint32)>(A)(s)); off(); }
                    else if(nm=="GetTimesMissionAttempted"||nm=="RegisterMissionAttempted"||nm=="RegisterMissionPassed"){ const uint32 id=rnd(4)==0 ? rnd(256) : rnd(100);
                        on(); if(nm=="GetTimesMissionAttempted")ret=int(reinterpret_cast<int8(__cdecl*)(uint32)>(A)(id)); else reinterpret_cast<void(__cdecl*)(uint32)>(A)(id); off(); }
                    else if(nm=="Init"||nm=="LoadActionReactionStats"){ setLines(); on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); }
                    else if(nm=="IsStatCapped"||nm=="IsStatFloat"||nm=="SafeToShowThisStat"){ const uint32 s=rnd(8)==0 ? rnd(65536) : rnd(3)==0 ? 256u*rnd(4)+INTERESTING[rnd(std::size(INTERESTING))] : rnd(2) ? pickStat() : rnd(400);
                        on(); ret=reinterpret_cast<bool(__cdecl*)(uint32)>(A)(s); off(); extra["stat"]=s; }
                    else if(nm=="Load"||nm=="Save"){ on(); ret=reinterpret_cast<bool(__cdecl*)()>(A)(); off(); }
                    else if(nm=="ProcessReactionStatsOnDecrement"||nm=="ProcessReactionStatsOnIncrement"){
                        static constexpr uint32 rs[]{22,165,225,22+256,165+256,21,23,245,335};
                        const uint32 s=rnd(3) ? rs[rnd(std::size(rs))] : pickStat();
                        In()[45]=rnd(4)==0 ? int32(rnd(3000)) : int32(rnd(4000))-2000; if(rnd(6)==0)In()[45]=1000+int32(rnd(3))-1;
                        if(rnd(4)==0)Fl()[21]=rnd(4)==0 ? qnan : uni(0.f,40.f);
                        on(); reinterpret_cast<void(__cdecl*)(uint32)>(A)(s); off(); extra["stat"]=s; }
                    else if(nm=="RegisterBestPosition"||nm=="RegisterFastestTime"){ const uint32 s=pickStat();
                        if(rnd(3)==0){ if(s<82)Fl()[s]=0.f; else reinterpret_cast<int32*>(0xB78E20)[s]=0; }
                        const int32 v=rnd(6)==0 ? int32(next()) : rnd(4)==0 ? int32(rnd(200))-100 : int32(rnd(70000));
                        on(); reinterpret_cast<void(__cdecl*)(uint32,int32)>(A)(s,v); off(); extra["args"]=json::array({s,v}); }
                    else if(nm=="UpdateStatsWhenFighting"||nm=="UpdateStatsWhenRunning"||nm=="UpdateStatsWhenSprinting"||nm=="UpdateStatsWhenOnMotorBike"){
                        auto* rv=reinterpret_cast<float*>(REACT);
                        for(int k:{30,31,32})rv[k]=rnd(8)==0 ? -uni(0.f,5.f) : uni(0.f,40.f);
                        for(int k:{35,36,40}){ rv[k]=rnd(10)==0 ? qnan : uni(0.f,20.f); }
                        for(int k:{2,3,7})rv[k]=rnd(6)==0 ? 0.f : uni(0.f,10.f);
                        uintptr_t ctr=nm=="UpdateStatsWhenSprinting" ? 0xB794D8 : nm=="UpdateStatsWhenRunning" ? 0xB79500 : 0xB794F8;
                        const int lim=nm=="UpdateStatsWhenSprinting" ? 35 : nm=="UpdateStatsWhenRunning" ? 36 : 40;
                        if(rnd(3)==0&&rv[lim]==rv[lim])*reinterpret_cast<uint32*>(ctr)=uint32(int64(rv[lim]*1000.f))+rnd(3)-1;
                        if(nm=="UpdateStatsWhenOnMotorBike"){
                            const float sp=rnd(3)==0 ? (std::array{0.1f,0.2f,0.6f})[rnd(3)] : uni(0.f,1.2f); const float a=uni(-3.2f,3.2f), z=uni(-0.3f,0.3f);
                            At<CVector>(bike.data(),0x44)=CVector{sp*std::cos(a),sp*std::sin(a),rnd(2) ? 0.f : z}; bike[0x804]=uint8(rnd(5));
                            on(); reinterpret_cast<void(__cdecl*)(CBike*)>(A)(reinterpret_cast<CBike*>(bike.data())); off(); }
                        else { on(); reinterpret_cast<void(__cdecl*)()>(A)(); off(); } }

                    // ---- volcado ----
                    json r={{"ret",ret},{"switches",switches},{"extra",extra},{"trace",TraceJson()},{"overflow",trOverflow},
                            {"stats",Hash(reinterpret_cast<void*>(STATS),STATSZ)},
                            {"parts",json::array({Hash(reinterpret_cast<void*>(MSGS),0x800),Hash(reinterpret_cast<void*>(NAME),8),Hash(reinterpret_cast<void*>(TIMES),0x190),
                                Hash(reinterpret_cast<void*>(RADIO),0x38),Hash(reinterpret_cast<void*>(PEDSK),0x80),Hash(reinterpret_cast<void*>(REACT),59*4),
                                Hash(reinterpret_cast<void*>(INTS),223*4),Hash(reinterpret_cast<void*>(FLOATS),82*4)})},
                            {"tail",ReversingProbe::Bytes(reinterpret_cast<void*>(TAIL),0xB79518-TAIL)},
                            {"key",json::array({F(Fl()[0]),F(Fl()[1]),F(Fl()[21]),F(Fl()[22]),F(Fl()[24]),F(Fl()[64]),In()[45],In()[245-120],In()[335-120],In()[176-120],In()[177-120]})},
                            {"players",Hash(Plr(0),2*PLSZ)},{"player_bytes",json::array({At<int16>(Plr(0),0x144),At<int16>(Plr(1),0x144),F(At<float>(Ped(0),0x540)),F(At<float>(Ped(1),0x540))})},
                            {"peds",Hash(peds.data(),peds.size())},{"thr",F(thr[0])},{"gstring",Hash(reinterpret_cast<void*>(GSTRING),352)},
                            {"misc",json::array({*reinterpret_cast<uint32*>(0xB7CB84),*reinterpret_cast<uint8*>(0xB794D4),*reinterpret_cast<uint8*>(0x8CDE56),focus})}};
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
