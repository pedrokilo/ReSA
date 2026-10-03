#pragma once
// Banco dentro del motor: CPopCycle (15 funciones) y CSprite (15 funciones). Original (hooks del lote
// desactivados) frente a candidato con fixtures deterministas por caso.
// Se simulan y trazan: rand (EXE y CGeneral::GetRandomNumber), ChoosePolice/Gang/CivilianOccupation,
// GangWarFightingGoingOn, FrenzyOnGoing, IsPlayerOnAMission, FindPlayerPed, IsZoneStreamingAllowed,
// LaRiotsActiveHere, FindPlayerCentreOfWorld, CTheZones::GetZoneInfo, CFileMgr (SetDir/OpenFile/ReadLine/
// CloseFile) y las funciones del dispositivo RW (RenderStateSet, Im2DRenderPrimitive e IndexedPrimitive).
// El estado global tocado (tablas de PopCycle y de poblacion, siguientes peatones a cargar, zonas de
// informacion, reloj, tiempo, lluvia, area, trucos, buferes de sprites, camara, recortes, tamano de
// pantalla y dispositivo RW) se aplica desde una base fija en cada ejecucion y se restaura al final.
// Se compara el retorno, la traza y las diferencias por dword de cada region frente al estado previo.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"

namespace World15Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }

// --- regiones de memoria: base fija, estado previo y diferencias por dword ---
struct Region { uintptr_t addr; size_t size; std::vector<uint8> saved, base, pre; };
inline uint8* P(Region& r) { return reinterpret_cast<uint8*>(r.addr); }
inline void Snap(Region& r) { r.saved.assign(P(r),P(r)+r.size); r.base=r.saved; }
inline void Apply(Region& r) { std::memcpy(P(r),r.base.data(),r.size); }
inline void Pre(Region& r) { r.pre.assign(P(r),P(r)+r.size); }
inline void Restore(Region& r) { std::memcpy(P(r),r.saved.data(),r.size); }
inline bool Restored(Region& r) { return std::memcmp(P(r),r.saved.data(),r.size)==0; }
inline json Diff(Region& r) {
    json d=json::array(); const auto* now=reinterpret_cast<const uint32*>(P(r)); const auto* old=reinterpret_cast<const uint32*>(r.pre.data());
    for(size_t k=0;k<r.size/4;++k)if(now[k]!=old[k])d.push_back({k*4,now[k]});
    return d;
}

// --- numeros aleatorios ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() {
    trace.push_back({"rand"});
    uint32 h=seed*2654435761u^(rngCalls++*40503u+0x9E37u); h^=h>>15; h*=0x2C1B3C6Du; h^=h>>12; h*=0x297A2D39u; h^=h>>15;
    if((h>>24)%16==0)return 0; if((h>>24)%16==1)return 0x7FFF; return int(h&0x7FFF);
}
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- poblacion ---
inline std::vector<int32> validPeds, loadedPeds, unloadedPeds;
inline int32 policeResult{}, civResult{}; inline std::array<int32,4> gangResults{}; inline uint32 gangCalls{};
inline eModelID __cdecl FakeChoosePolice() { trace.push_back({"choose_police"}); return eModelID(policeResult); }
inline eModelID __cdecl FakeChooseGang(int32 g) { trace.push_back({"choose_gang",g}); return eModelID(gangResults[gangCalls++%4]); }
inline eModelID __cdecl FakeChooseCivilian(bool male,bool female,int32 anim,int32 notModel,int32 stat,bool foot,bool used,bool attractor,const char* script) {
    trace.push_back({"choose_civilian",male,female,anim,notModel,stat,foot,used,attractor,script!=nullptr}); return eModelID(civResult); }
inline bool gangWarFighting{}, frenzy{}, onMission{}, zoneStreaming{}, pedNull{}; inline uint32 riotsMask{}, riotsCalls{};
inline bool __cdecl FakeGangWar() { trace.push_back({"gang_war_fighting"}); return gangWarFighting; }
inline bool __cdecl FakeFrenzy() { trace.push_back({"frenzy"}); return frenzy; }
inline bool __cdecl FakeOnMission() { trace.push_back({"on_mission"}); return onMission; }
inline bool __cdecl FakeZoneStreaming() { trace.push_back({"zone_streaming"}); return zoneStreaming; }
inline bool __cdecl FakeRiots() { trace.push_back({"la_riots"}); return (riotsMask>>(riotsCalls++%32))&1; }
alignas(16) inline std::array<uint8,0x800> fakePed{}; alignas(16) inline std::array<uint8,0x600> fakeVeh{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return pedNull ? nullptr : reinterpret_cast<CPlayerPed*>(fakePed.data()); }
inline CVector centre{};
inline const CVector* __cdecl FakeCentre(int32 id) { trace.push_back({"centre_of_world",id}); return &centre; }
alignas(16) inline std::array<uint8,0x11*4+3> fakeZoneInfos{}; alignas(16) inline std::array<uint8,0x20> fakeZone{}; inline uint32 zoneResult{};
inline CZoneInfo* __cdecl FakeGetZoneInfo(const CVector& p,CZone** z) {
    trace.push_back({"get_zone_info",F(p.x),F(p.y),F(p.z),uintptr_t(z)==0xC0BC64 ? json("curr_zone") : json("other")});
    if(z)*z=reinterpret_cast<CZone*>(fakeZone.data()); return reinterpret_cast<CZoneInfo*>(fakeZoneInfos.data()+0x11*zoneResult); }
inline json ZoneLabel(uint32 v) { if(!v)return nullptr; if(v>=uintptr_t(fakeZoneInfos.data())&&v<uintptr_t(fakeZoneInfos.data())+fakeZoneInfos.size())return "zi+"+std::to_string(v-uintptr_t(fakeZoneInfos.data())); if(v==uintptr_t(fakeZone.data()))return "zone"; return "other"; }

// --- archivo POPCYCLE.DAT ---
inline std::vector<std::string> fileLines; inline size_t fileLine{}; inline uint32 readLines{};
inline int32 __cdecl FakeSetDir(const char* p) { trace.push_back({"set_dir",std::string(p)}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m) { trace.push_back({"open_file",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(0x1234); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f) { trace.push_back({"close_file",uintptr_t(f)}); return 0; }
inline bool __cdecl FakeReadLine(FILESTREAM,char* s,int32 n) {
    ++readLines; if(fileLine>=fileLines.size()||n<=0)return false;
    const std::string l=fileLines[fileLine++]+"\n"; const size_t k=std::min(l.size(),size_t(n-1)); std::memcpy(s,l.data(),k); s[k]=0; return true; }

// --- dispositivo RW ---
inline json VName(const void* p) {
    const auto u=uintptr_t(p); if(u==0xC4B8E0)return "sprite"; if(u==0xC4B958)return "indices";
    if(u>=0xC5F958&&u<0xC6A158)return "temp2d+"+std::to_string(u-0xC5F958); return "other"; }
inline uint64 Fnv(const void* p,size_t n) { uint64 h=1469598103934665603ull; const auto* b=static_cast<const uint8*>(p); for(size_t k=0;k<n;++k){h^=b[k];h*=1099511628211ull;} return h; }
inline RwBool __cdecl FakeStateSet(RwRenderState s,void* v) { trace.push_back({"state_set",int(s),uintptr_t(v)}); return TRUE; }
inline RwBool __cdecl FakePrim(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 n) {
    trace.push_back({"prim",int(t),VName(v),n,ReversingProbe::Bytes(v,size_t(std::clamp(n,0,8))*sizeof(RwIm2DVertex))}); return TRUE; }
inline RwBool __cdecl FakeIndexed(RwPrimitiveType t,RwIm2DVertex* v,RwInt32 nv,RwImVertexIndex* idx,RwInt32 ni) {
    trace.push_back({"indexed",int(t),VName(v),nv,VName(idx),ni,std::to_string(Fnv(v,size_t(std::clamp(nv,0,1536))*sizeof(RwIm2DVertex))),std::to_string(Fnv(idx,size_t(std::clamp(ni,0,4096))*2))}); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,30> fns{{
    {"CPopCycle","Initialise",0x5BC090,0,-1,32},{"CPopCycle","PickGangToCreateMembersOf",0x60F8D0,0,-1,512},{"CPopCycle","FindNewPedType",0x60FBD0,0,-1,1024},
    {"CPopCycle","PickPedMIToStreamInForCurrentZone",0x60FFD0,0,-1,512},{"CPopCycle","IsPedAppropriateForCurrentZone",0x610150,0,-1,512},
    {"CPopCycle","IsPedInGroup",0x610210,0,-1,512},{"CPopCycle","GetCurrentPercOther_Peds",0x610310,0,-1,512},{"CPopCycle","PickARandomGroupOfOtherPeds",0x610420,0,-1,512},
    {"CPopCycle","PlayerKilledADealer",0x610490,0,-1,128},{"CPopCycle","UpdateDealerStrengths",0x6104B0,0,-1,256},{"CPopCycle","UpdateAreaDodgyness",0x610560,0,-1,256},
    {"CPopCycle","UpdateIsGangArea",0x6106D0,0,-1,128},{"CPopCycle","PedIsAcceptableInCurrentZone",0x610720,0,-1,256},{"CPopCycle","UpdatePercentages",0x610770,0,-1,1024},
    {"CPopCycle","Update",0x610BF0,0,-1,512},
    {"CSprite","Initialise",0x70CE10,0,-1,64},{"CSprite","CalcScreenCoors",0x70CE30,0,-1,1024},{"CSprite","FlushSpriteBuffer",0x70CF20,0,-1,256},
    {"CSprite","InitSpriteBuffer",0x70CFB0,0,-1,64},{"CSprite","RenderOneXLUSprite",0x70D000,0,-1,1024},{"CSprite","RenderOneXLUSprite_Triangle",0x70D320,0,-1,1024},
    {"CSprite","RenderOneXLUSprite_Rotate_Aspect",0x70D490,0,-1,1024},{"CSprite","Set4Vertices2D-CRect",0x70E1C0,0,-1,512},{"CSprite","CalcHorizonCoors",0x70E3E0,0,-1,512},
    {"CSprite","RenderBufferedOneXLUSprite",0x70E4A0,0,-1,1024},{"CSprite","RenderBufferedOneXLUSprite_Rotate_Aspect",0x70E780,0,-1,1024},
    {"CSprite","RenderBufferedOneXLUSprite_Rotate_Dimension",0x70EAB0,0,-1,1024},{"CSprite","RenderBufferedOneXLUSprite_Rotate_2Colours",0x70EDE0,0,-1,1024},
    {"CSprite","RenderBufferedOneXLUSprite2D",0x70F440,0,-1,512},{"CSprite","RenderOneXLUSprite2D",0x70F540,0,-1,512},
}};

// direcciones del EXE
constexpr uintptr_t POP_BEGIN=0xC0BC30, POP_END=0xC0E97C, POPN_BEGIN=0xC0EC38, POPN_END=0xC0FCC0, STREAM_BEGIN=0x8E4BB0, STREAM_END=0x8E4C24;
constexpr uintptr_t ZONES_BEGIN=0xBA1DE8, ZONES_END=0xBA1DF0+380*0x11, SA_BEGIN=0xC4B8D0, SA_END=0xC4D958, SB_BEGIN=0xC5F958, SB_END=0xC6A160;
constexpr uintptr_t TRANSLATION=0x8D2540;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world15-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world15-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world15");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world15-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,30> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,30> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    { const unsigned pc=_control87(0,0)&_MCW_PC; Write({{"event","info"},{"precision_bits",pc==_PC_24 ? 24 : pc==_PC_53 ? 53 : 64}}); }

    // peatones validos (cargados y sin cargar) para los modelos de los fixtures
    for(int32 m=0;m<300;++m){ auto* mi=CModelInfo::GetModelInfo(m); if(!mi||mi->GetModelType()!=MODEL_INFO_PED||m==0)continue; validPeds.push_back(m); (CStreaming::IsModelLoaded(m) ? loadedPeds : unloadedPeds).push_back(m); }
    if(loadedPeds.empty())loadedPeds.push_back(validPeds.front()); if(unloadedPeds.empty())unloadedPeds.push_back(validPeds.back());

    // regiones guardadas (se restauran al final y se reaplican desde la base en cada ejecucion)
    auto R=[](uintptr_t a,uintptr_t e){ Region r{a,size_t(e-a)}; Snap(r); return r; };
    Region rPop=R(POP_BEGIN,POP_END), rPopN=R(POPN_BEGIN,POPN_END), rStream=R(STREAM_BEGIN,STREAM_END), rZones=R(ZONES_BEGIN,ZONES_END);
    Region rSA=R(SA_BEGIN,SA_END), rSB=R(SB_BEGIN,SB_END), rCamV=R(0xB6FA2C,0xB6FA2C+0x48), rCamP=R(0xB6F02C,0xB6F040), rCamF=R(0xB6F104,0xB6F10C);
    Region rClip=R(0xC3EF9C,0xC3EFA4), rFov=R(0x8D5038,0x8D503C), rScreen=R(0xC17044,0xC1704C), rLine=R(0xB71848,0xB71848+512);
    const uintptr_t devZ=reinterpret_cast<uintptr_t>(&RWSRCGLOBAL(dOpenDevice).zBufferNear); Region rDev=R(devZ,devZ+8);
    struct Small { uintptr_t a; size_t n; std::vector<uint8> saved; };
    std::vector<Small> smalls{{0x96915B,1},{0xB72914,4},{0xC81324,4},{0xB7014E,1},{0xB70153,1},{0xB7CD74,1},{0x96AB90,1},{0xB7CB84,4},{0xB7CB78,4},{0x8CD060,4},{0x8D12C0,4}};
    for(auto& s:smalls)s.saved.assign(reinterpret_cast<uint8*>(s.a),reinterpret_cast<uint8*>(s.a)+s.n);
    std::vector<Region*> regions{&rPop,&rPopN,&rStream,&rZones,&rSA,&rSB,&rCamV,&rCamP,&rCamF,&rClip,&rFov,&rScreen,&rLine,&rDev};

    // bases deterministas
    { uint32 st=0x15C0FFEEu; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;}; auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
        auto percRow=[&](uint8* p){ std::memset(p,0,18); const uint32 k=1+next()%8; int32 left=100; for(uint32 j=0;j<k;++j){ const int32 v=j+1==k ? left : int32(next()%uint32(left+1)); p[next()%18]+=uint8(v); left-=v; } };
        // tablas de PopCycle: porcentajes que suman 100 en todas las filas (fuera del camino FIX_BUGS) y demas tablas
        for(int r=0;r<480;++r)percRow(rPop.base.data()+(0xC0BC78-POP_BEGIN)+r*18);
        for(size_t k=0xC0DE38-POP_BEGIN;k<0xC0E978-POP_BEGIN;++k)rPop.base[k]=uint8(next()%101);
        for(size_t k=0xC0E5B8-POP_BEGIN;k<0xC0E978-POP_BEGIN;++k)rPop.base[k]=uint8(next()%61);
        // grupos de peatones: modelos validos
        auto* groups=reinterpret_cast<uint16*>(rPopN.base.data()+(0xC0F358-POPN_BEGIN));
        for(int g=0;g<57*21;++g)groups[g]=uint16(validPeds[next()%validPeds.size()]);
        // zonas de informacion: fuerzas de bandas y camellos
        for(int z=0;z<380;++z){ auto* zi=rZones.base.data()+8+z*0x11; for(int k=0;k<10;++k)zi[k]=uint8(next()%3==0 ? next()%25 : next()%8); zi[10]=uint8(next()%20); }
        // buferes de sprites: flotantes seguros en todos los dwords
        for(size_t k=0;k+4<=rSA.size;k+=4){ const float v=uni(-1000.f,1000.f); std::memcpy(rSA.base.data()+k,&v,4); }
        for(size_t k=0;k+4<=rSB.size;k+=4){ const float v=uni(-1000.f,1000.f); std::memcpy(rSB.base.data()+k,&v,4); }
        for(size_t k=0xC4B958-SA_BEGIN;k<SA_END-SA_BEGIN;k+=2){ const uint16 v=uint16(next()); std::memcpy(rSA.base.data()+k,&v,2); }
        std::memset(rSA.base.data()+(0xC4B950-SA_BEGIN),0,8); }

    alignas(16) static CMatrix fakeCamMatrix; alignas(16) static std::array<RwIm2DVertex,4> setVerts{};
    auto& device=RWSRCGLOBAL(dOpenDevice);
    const auto savedSet=device.fpRenderStateSet; const auto savedPrim=device.fpIm2DRenderPrimitive; const auto savedIndexed=device.fpIm2DRenderIndexedPrimitive;
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(&CPopulation::ChoosePolicePedOccupation),FakeChoosePolice), p1e(0x610F40,FakeChoosePolice);
        EntryPatch p2(bit_cast<uintptr_t>(&CPopulation::ChooseGangOccupation),FakeChooseGang), p2e(0x611550,FakeChooseGang);
        EntryPatch p3(bit_cast<uintptr_t>(&CPopulation::ChooseCivilianOccupation),FakeChooseCivilian), p3e(0x612F90,FakeChooseCivilian);
        EntryPatch g1(bit_cast<uintptr_t>(&CGangWars::GangWarFightingGoingOn),FakeGangWar), g1e(0x443AC0,FakeGangWar);
        EntryPatch g2(bit_cast<uintptr_t>(&CDarkel::FrenzyOnGoing),FakeFrenzy), g2e(0x43D1F0,FakeFrenzy);
        EntryPatch g3(bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission),FakeOnMission), g3e(0x464D50,FakeOnMission);
        EntryPatch g4(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), g4e(0x56E210,FakeFindPlayerPed);
        EntryPatch g5(bit_cast<uintptr_t>(&CCheat::IsZoneStreamingAllowed),FakeZoneStreaming), g5e(0x407410,FakeZoneStreaming);
        EntryPatch g6(bit_cast<uintptr_t>(&CGameLogic::LaRiotsActiveHere),FakeRiots), g6e(0x441C10,FakeRiots);
        EntryPatch g7(bit_cast<uintptr_t>(&FindPlayerCentreOfWorld),FakeCentre), g7e(0x56E250,FakeCentre);
        EntryPatch g8(bit_cast<uintptr_t>(static_cast<CZoneInfo*(*)(const CVector&,CZone**)>(&CTheZones::GetZoneInfo)),FakeGetZoneInfo), g8e(0x572400,FakeGetZoneInfo);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), f1e(0x5387D0,FakeSetDir);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f2e(0x538900,FakeOpenFile);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::ReadLine),FakeReadLine), f3e(0x5389B0,FakeReadLine);
        EntryPatch f4(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f4e(0x5389D0,FakeCloseFile);
        device.fpRenderStateSet=reinterpret_cast<decltype(device.fpRenderStateSet)>(&FakeStateSet);
        device.fpIm2DRenderPrimitive=reinterpret_cast<decltype(device.fpIm2DRenderPrimitive)>(&FakePrim);
        device.fpIm2DRenderIndexedPrimitive=reinterpret_cast<decltype(device.fpIm2DRenderIndexedPrimitive)>(&FakeIndexed);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool pop=cls=="CPopCycle";
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1515u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    gangCalls=0; riotsCalls=0; readLines=0; fileLine=0;
                    for(auto* r:regions)Apply(*r);
                    for(auto& s:smalls)std::memcpy(reinterpret_cast<void*>(s.a),s.saved.data(),s.n);
                    json out=json::object(); json ret=nullptr;
                    auto& cheat=*reinterpret_cast<uint8*>(0x96915B); auto& area=*reinterpret_cast<int32*>(0xB72914); auto& rain=*reinterpret_cast<float*>(0xC81324);
                    auto& day=*reinterpret_cast<uint8*>(0xB7014E); auto& hours=*reinterpret_cast<uint8*>(0xB70153); auto& focus=*reinterpret_cast<uint8*>(0xB7CD74);
                    auto& gangWars=*reinterpret_cast<uint8*>(0x96AB90); auto& timeNow=*reinterpret_cast<uint32*>(0xB7CB84); auto& timePrev=*reinterpret_cast<uint32*>(0xB7CB78);
                    auto& lightMult=*reinterpret_cast<float*>(0x8CD060); auto& dnParam=*reinterpret_cast<float*>(0x8D12C0);

                    if(pop) {
                        // --- fixture comun de PopCycle ---
                        auto* zi=fakeZoneInfos.data();
                        for(int z=0;z<4;++z){ auto* p=zi+0x11*z; const int mode=int(next()%4);
                            for(int k=0;k<10;++k)p[k]=uint8(mode==0 ? 0 : mode==1 ? next()%40 : next()%3==0 ? next()%25 : 0);
                            p[10]=uint8(next()%4==0 ? next()%20 : next()%4); for(int k=11;k<15;++k)p[k]=uint8(next());
                            p[15]=uint8((next()%20)|((next()%4)<<5)|(next()%4==0 ? 0x80 : 0)); p[16]=uint8(next()); }
                        for(auto& b:fakeZone)b=uint8(next());
                        auto* hdr=reinterpret_cast<float*>(POP_BEGIN);
                        for(int k=0;k<13;++k)hdr[k]=next()%6==0 ? float(int(next()%5)) : uni(-3.f,14.f);   // 0xC0BC30..0xC0BC60 (numeros y fracciones)
                        *reinterpret_cast<uint8*>(0xC0BC5C)=uint8(next()%2); *reinterpret_cast<float*>(0xC0BC60)=uni(0.f,1.f);
                        CPopCycle::m_NumDealers_Peds=uni(-3.f,14.f);
                        CPopCycle::m_pCurrZone=reinterpret_cast<CZone*>(fakeZone.data());
                        CPopCycle::m_pCurrZoneInfo=reinterpret_cast<CZoneInfo*>(zi+0x11*(next()%4));
                        CPopCycle::m_nCurrentZoneType=int32(next()%20); CPopCycle::m_nCurrentTimeIndex=int32(next()%12); CPopCycle::m_nCurrentTimeOfWeek=int32(next()%2);
                        // filas de porcentajes de los tipos de zona usados: nuevas en cada caso (suman 100)
                        auto percRow=[&](uint8* p){ std::memset(p,0,18); const uint32 k=1+next()%8; int32 left=100; for(uint32 j=0;j<k;++j){ const int32 v=j+1==k ? left : int32(next()%uint32(left+1)); p[next()%18]+=uint8(v); left-=v; } };
                        for(int z=0;z<5;++z){ const int pt=z<4 ? (zi[0x11*z+15]&0x1F) : CPopCycle::m_nCurrentZoneType; for(int t=0;t<12;++t)for(int w=0;w<2;++w)percRow(&CPopCycle::m_nPercTypeGroup[t][w][pt][0]); }
                        for(int k=0;k<6;++k){ auto* tab=reinterpret_cast<uint8*>(0xC0DE38+k*480); tab[(CPopCycle::m_nCurrentTimeIndex*2+CPopCycle::m_nCurrentTimeOfWeek)*20+CPopCycle::m_nCurrentZoneType]=uint8(next()%(k>=4 ? 61 : 101)); }
                        // contadores y banderas de CPopulation
                        *reinterpret_cast<uint32*>(0xC0EC38)=next()%8; for(int g=0;g<10;++g)*reinterpret_cast<uint32*>(0xC0EC3C+4*g)=next()%3==0 ? next()%9 : 0;
                        *reinterpret_cast<uint32*>(0xC0EC68)=next()%8; *reinterpret_cast<uint32*>(0xC0EC6C)=next()%12; *reinterpret_cast<uint32*>(0xC0EC70)=next()%12;
                        for(uintptr_t a=0xC0FCB2;a<=0xC0FCB6;++a)*reinterpret_cast<uint8*>(a)=uint8(next()%5==0);
                        CPopulation::CurrentWorldZone=next()%3;
                        for(int g=0;g<57;++g)CPopulation::m_nNumPedsInGroup[g]=uint16(next()%6==0 ? 0 : 1+next()%21);
                        { const int dg=52; CPopulation::m_nNumPedsInGroup[dg]=uint16(next()%9); auto* row=reinterpret_cast<uint16*>(0xC0F358+dg*42);
                          for(int k=0;k<21;++k)row[k]=uint16(next()%2 ? loadedPeds[next()%loadedPeds.size()] : unloadedPeds[next()%unloadedPeds.size()]); }
                        for(int g=0;g<18;++g)CStreaming::ms_NextPedToLoadFromGroup[g]=int32(next()%22);
                        for(auto& m:CStreaming::ms_pedsLoaded)m=next()%3==0 ? MODEL_INVALID : eModelID(validPeds[next()%validPeds.size()]);
                        // dependencias simuladas
                        policeResult=validPeds[next()%validPeds.size()]; { const uint32 c=next()%10; civResult=c==0 ? -1 : c==1 ? 7 : validPeds[next()%validPeds.size()]; }
                        for(auto& g:gangResults)g=next()%3==0 ? -1 : validPeds[next()%validPeds.size()];
                        gangWarFighting=next()%5==0; frenzy=next()%4==0; onMission=next()%2; zoneStreaming=next()%3==0; pedNull=next()%6==0; riotsMask=next()%3==0 ? next() : 0;
                        std::memset(fakePed.data(),0,fakePed.size()); std::memset(fakeVeh.data(),0,fakeVeh.size());
                        if(next()%3)*reinterpret_cast<uint32*>(fakePed.data()+0x46C)|=0x100u; *reinterpret_cast<void**>(fakePed.data()+0x58C)=next()%5 ? fakeVeh.data() : nullptr;
                        { static const int16 models[]{420,438,400,596,420,438}; *reinterpret_cast<int16*>(fakeVeh.data()+0x22)=models[next()%6]; }
                        centre=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%4==0 ? uni(950.f,1100.f) : uni(-10.f,300.f)}; if(next()%10==0)centre.z=950.f; zoneResult=next()%4;
                        cheat=uint8(next()%8==0); area=next()%4==0 ? int32(1+next()%17) : 0; rain=next()%4==0 ? float(next()%2) : uni(0.f,1.f);
                        day=uint8(next()%10); hours=uint8(next()%24); focus=uint8(next()%2); gangWars=uint8(next()%5!=0);
                        timePrev=next()%2 ? next()%10000000 : next(); timeNow=next()%2 ? timePrev+next()%120 : timePrev+next()%200000;
                        lightMult=next()%5==0 ? float(next()%2) : uni(0.55f,1.05f); dnParam=next()%6==0 ? 0.5f : uni(0.f,1.f);
                        // zonas de informacion del mundo (UpdateDealerStrengths)
                        CTheZones::TotalNumberOfZoneInfos=int16(next()%5==0 ? 0 : next()%381);
                        for(int z=0;z<380;++z)CTheZones::ZoneInfoArray[z].DealerStrength=uint8(next()%4==0 ? next()%20 : next()%15);
                        if(nm=="Initialise"){
                            fileLines.clear(); const char* seps[]{" ","\t",","," , ","  "};
                            for(int z=0;z<20;++z)for(int w=0;w<2;++w)for(int t=0;t<12;++t){
                                while(next()%12==0)fileLines.push_back(next()%2 ? "// comentario "+std::to_string(next()%1000) : std::string(next()%2 ? "" : "   "));
                                uint8 row[24]{}; row[0]=uint8(next()%61); row[1]=uint8(next()%41); for(int k=2;k<6;++k)row[k]=uint8(next()%101); percRow(row+6);
                                std::string l=next()%5==0 ? "  " : ""; for(int k=0;k<24;++k){ if(k)l+=seps[next()%5]; l+=std::to_string(row[k]); } fileLines.push_back(l); }
                        }
                        for(auto* r:{&rPop,&rPopN,&rStream,&rZones})Pre(*r);
                        const auto preZones=std::vector<uint8>(fakeZoneInfos.begin(),fakeZoneInfos.end());

                        for(auto* h:hooks)h->State(candidate);
                        out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        const uintptr_t a=fn.address;
                        if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="PickGangToCreateMembersOf")ret=int32(reinterpret_cast<ePedType(__cdecl*)()>(a)());
                        else if(nm=="FindNewPedType"){ if(next()%20==0)CPopCycle::m_pCurrZoneInfo=nullptr; CPopulation::bInPoliceStation=next()%4==0;
                            ePedType type=ePedType(0x5A5A); eModelID model=eModelID(0x7B7B);
                            ret=reinterpret_cast<bool(__cdecl*)(ePedType&,eModelID&,bool,bool)>(a)(type,model,next()%5==0,next()%5==0); out["type"]=int32(type); out["model"]=int32(model); }
                        else if(nm=="PickPedMIToStreamInForCurrentZone")ret=int32(reinterpret_cast<eModelID(__cdecl*)()>(a)());
                        else if(nm=="IsPedAppropriateForCurrentZone")ret=reinterpret_cast<bool(__cdecl*)(int32)>(a)(validPeds[next()%validPeds.size()]);
                        else if(nm=="IsPedInGroup"){ const int32 m=next()%4 ? validPeds[next()%validPeds.size()] : int32(next()%400); ret=reinterpret_cast<bool(__cdecl*)(int32,int32)>(a)(m,int32(next()%33)); }
                        else if(nm=="GetCurrentPercOther_Peds")ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                        else if(nm=="PickARandomGroupOfOtherPeds")ret=int32(reinterpret_cast<int32(__cdecl*)()>(a)());
                        else if(nm=="PlayerKilledADealer"){ if(next()%10==0)CPopCycle::m_pCurrZoneInfo=nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="PedIsAcceptableInCurrentZone"){ if(next()%10==0)CPopCycle::m_pCurrZoneInfo=nullptr; ret=reinterpret_cast<bool(__cdecl*)(int32)>(a)(validPeds[next()%validPeds.size()]); }
                        else { if(nm=="Update"&&next()%5==0)CPopCycle::m_pCurrZoneInfo=nullptr; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["pop"]=Diff(rPop); out["popn"]=Diff(rPopN); out["stream"]=Diff(rStream); out["zones"]=Diff(rZones);
                        if(std::vector<uint8>(fakeZoneInfos.begin(),fakeZoneInfos.end())!=preZones)out["zi"]=ReversingProbe::Bytes(fakeZoneInfos.data(),fakeZoneInfos.size());
                        out["curr"]={ZoneLabel(*reinterpret_cast<uint32*>(0xC0BC64)),ZoneLabel(*reinterpret_cast<uint32*>(0xC0BC68))};
                        out["light"]=F(lightMult); out["reads"]=readLines;
                    } else {
                        // --- fixture comun de CSprite ---
                        static const int32 widths[]{640,1600,1920}, heights[]{480,900,1080};
                        if(next()%3){ const int k=int(next()%3); RsGlobal.maximumWidth=widths[k]; RsGlobal.maximumHeight=heights[k]; }
                        const float W=float(RsGlobal.maximumWidth), H=float(RsGlobal.maximumHeight);
                        device.zBufferNear=uni(0.f,0.2f); device.zBufferFar=uni(0.8f,1.2f);
                        CDraw::ms_fNearClipZ=uni(0.05f,3.f); CDraw::ms_fFarClipZ=uni(100.f,3000.f); CDraw::ms_fFOV=uni(20.f,120.f);
                        CSprite::m_f2DNearScreenZ=uni(0.f,0.1f); CSprite::m_f2DFarScreenZ=uni(0.9f,1.f); CSprite::m_fRecipNearClipPlane=uni(0.1f,10.f);
                        { auto* m=reinterpret_cast<float*>(0xB6FA2C); for(int k=0;k<12;++k)if(k%4!=3)m[k]=uni(-1.f,1.f); m[12]=uni(-100.f,100.f); m[13]=uni(-100.f,100.f); m[14]=uni(-100.f,100.f); }
                        *reinterpret_cast<CVector*>(0xB6F02C)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                        fakeCamMatrix.GetPosition()=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                        *reinterpret_cast<CMatrix**>(0xB6F03C)=next()%2 ? &fakeCamMatrix : nullptr;
                        *reinterpret_cast<float*>(0xB6F104)=uni(-1.f,1.f); *reinterpret_cast<float*>(0xB6F108)=uni(-1.f,1.f);
                        auto& index=*reinterpret_cast<int32*>(0xC6A158); auto& zswitch=*reinterpret_cast<uint32*>(0xC6A15C);
                        { const uint32 c=next()%10; index=c==0 ? 383 : c==1 ? 0 : int32(next()%384); static const uint32 sw[]{0,1,0x100,0xFFFFFFFF}; zswitch=sw[next()%4]; }
                        { auto* sv=reinterpret_cast<float*>(0xC4B8E0); for(int k=0;k<28;++k)sv[k]=uni(-500.f,500.f); }
                        for(auto& v:setVerts){ auto* fl=reinterpret_cast<float*>(&v); for(int k=0;k<7;++k)fl[k]=uni(-50.f,50.f); }
                        auto col=[&]{return uint8(next());};
                        auto inten=[&]{ const uint32 c=next()%5; return int16(c<3 ? int32(next()%256) : c==3 ? int32(256+next()%768) : int32(int16(next()))); };
                        auto sx=[&]{return uni(-200.f,W+200.f);}; auto sy=[&]{return uni(-200.f,H+200.f);};
                        auto depth=[&]{ const uint32 c=next()%8; return c==0 ? uni(0.5f,1.3f) : c==1 ? uni(1.3f,2.3f) : c==2 ? 1.3f : c==3 ? 2.3f : uni(2.3f,500.f); };
                        auto angle=[&]{ return next()%5==0 ? uni(-1000.f,1000.f) : uni(-7.f,7.f); };
                        auto sz=[&]{ return next()%6==0 ? uni(200.f,900.f) : uni(0.5f,200.f); };
                        for(auto* r:{&rSA,&rSB})Pre(*r);
                        const int32 idxBefore=index;

                        for(auto* h:hooks)h->State(candidate);
                        out["switches"]=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        const uintptr_t a=fn.address;
                        if(nm=="Initialise"||nm=="InitSpriteBuffer"||nm=="FlushSpriteBuffer"){ if(nm=="FlushSpriteBuffer"&&next()%6==0)index=-int32(next()%3); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="CalcScreenCoors"){ RwV3d p{uni(-300.f,300.f),uni(-300.f,300.f),uni(-300.f,300.f)}, o{-1.f,-2.f,-3.f}; float w=-4.f, h=-5.f;
                            ret=reinterpret_cast<bool(__cdecl*)(const RwV3d&,RwV3d*,float*,float*,bool,bool)>(a)(p,&o,&w,&h,next()%2,next()%2); out["out"]={F(o.x),F(o.y),F(o.z),F(w),F(h)}; }
                        else if(nm=="CalcHorizonCoors")ret=F(reinterpret_cast<float(__cdecl*)()>(a)());
                        else if(nm=="Set4Vertices2D-CRect"){ CRect rc; rc.left=sx(); rc.top=sy(); rc.right=sx(); rc.bottom=sy(); const CRGBA c1(col(),col(),col(),col()), c2(col(),col(),col(),col()), c3(col(),col(),col(),col()), c4(col(),col(),col(),col());
                            reinterpret_cast<void(__cdecl*)(RwIm2DVertex*,const CRect&,const CRGBA&,const CRGBA&,const CRGBA&,const CRGBA&)>(a)(setVerts.data(),rc,c1,c2,c3,c4); out["verts"]=ReversingProbe::Bytes(setVerts.data(),sizeof(setVerts)); }
                        else if(nm=="RenderOneXLUSprite"){ const uint8 udir=uint8(next()%3==0 ? next() : next()%2), vdir=uint8(next()%3==0 ? next() : next()%2);
                            reinterpret_cast<void(__cdecl*)(CVector,CVector2D,uint8,uint8,uint8,int16,float,uint8,uint8,uint8)>(a)(CVector{sx(),sy(),uni(1.f,500.f)},CVector2D{sz(),sz()},col(),col(),col(),inten(),uni(0.f,2.f),col(),udir,vdir); }
                        else if(nm=="RenderOneXLUSprite_Triangle")
                            reinterpret_cast<void(__cdecl*)(CVector2D,CVector2D,CVector2D,float,uint8,uint8,uint8,int16,float,uint8)>(a)(CVector2D{sx(),sy()},CVector2D{sx(),sy()},CVector2D{sx(),sy()},depth(),col(),col(),col(),inten(),uni(0.f,2.f),col());
                        else if(nm=="RenderOneXLUSprite_Rotate_Aspect")
                            reinterpret_cast<void(__cdecl*)(CVector,CVector2D,uint8,uint8,uint8,int16,float,float,uint8)>(a)(CVector{sx(),sy(),depth()},CVector2D{sz(),sz()},col(),col(),col(),inten(),uni(0.f,2.f),angle(),col());
                        else if(nm=="RenderOneXLUSprite2D"){ const CRGBA c(col(),col(),col(),col()); const float w=next()%10==0 ? -sz() : sz(), h=next()%10==0 ? -sz() : sz();
                            reinterpret_cast<void(__cdecl*)(CVector2D,CVector2D,const CRGBA&,int16,uint8)>(a)(CVector2D{sx(),sy()},CVector2D{w,h},c,inten(),col()); }
                        else if(nm=="RenderBufferedOneXLUSprite")
                            reinterpret_cast<void(__cdecl*)(CVector,CVector2D,uint8,uint8,uint8,int16,float,uint8)>(a)(CVector{sx(),sy(),uni(1.f,500.f)},CVector2D{sz(),sz()},col(),col(),col(),inten(),uni(0.f,2.f),col());
                        else if(nm=="RenderBufferedOneXLUSprite_Rotate_Aspect")
                            reinterpret_cast<void(__cdecl*)(float,float,float,float,float,uint8,uint8,uint8,int16,float,float,uint8)>(a)(sx(),sy(),uni(1.f,500.f),sz(),sz(),col(),col(),col(),inten(),uni(0.f,2.f),angle(),col());
                        else if(nm=="RenderBufferedOneXLUSprite_Rotate_Dimension")
                            reinterpret_cast<void(__cdecl*)(CVector,CVector2D,uint8,uint8,uint8,int16,float,float,uint8)>(a)(CVector{sx(),sy(),uni(1.f,500.f)},CVector2D{sz(),sz()},col(),col(),col(),inten(),uni(0.f,2.f),angle(),col());
                        else if(nm=="RenderBufferedOneXLUSprite_Rotate_2Colours"){ const float fx=next()%6==0 ? 0.f : uni(-2.f,2.f), fy=next()%6==0 ? 0.f : uni(-2.f,2.f);
                            reinterpret_cast<void(__cdecl*)(float,float,float,float,float,uint8,uint8,uint8,uint8,uint8,uint8,float,float,float,float,uint8)>(a)(sx(),sy(),uni(1.f,500.f),sz(),sz(),col(),col(),col(),col(),col(),col(),fx,fy,uni(0.f,2.f),angle(),col()); }
                        else if(nm=="RenderBufferedOneXLUSprite2D"){ const RwRGBA c{col(),col(),col(),col()}; const float w=next()%10==0 ? -sz() : sz(), h=next()%10==0 ? -sz() : sz();
                            reinterpret_cast<void(__cdecl*)(CVector2D,CVector2D,const RwRGBA&,int16,uint8)>(a)(CVector2D{sx(),sy()},CVector2D{w,h},c,inten(),col()); }
                        out["sa"]=Diff(rSA); out["sb"]=Diff(rSB); out["index"]=index; out["switch"]=zswitch; out["before"]=idxBefore;
                    }
                    for(auto* h:hooks)h->State(false);
                    out["ret"]=ret; out["trace"]=trace; return out;
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
        device.fpRenderStateSet=savedSet; device.fpIm2DRenderPrimitive=savedPrim; device.fpIm2DRenderIndexedPrimitive=savedIndexed;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    for(auto* r:regions)Restore(*r);
    for(auto& s:smalls)std::memcpy(reinterpret_cast<void*>(s.a),s.saved.data(),s.n);
    bool restored=device.fpRenderStateSet==savedSet&&device.fpIm2DRenderPrimitive==savedPrim&&device.fpIm2DRenderIndexedPrimitive==savedIndexed;
    for(auto* r:regions)restored&=Restored(*r);
    for(auto& s:smalls)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.saved.data(),s.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
