#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <type_traits>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "PlayerInfo.h"
#include "Entity/Ped/Ped.h"
#include "Camera.h"
#include "Weapon.h"
#include "Pad.h"
#include "Timer.h"
#include "Draw.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerCrosshair74 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerInfo::ProcessCrosshair (0x56EC80, thiscall con this = &m_nCrosshairActivated, `ret 8`: indice de jugador y CPad*; sin valor de retorno que lea ningun llamador, CPlayerInfo::Process 0x56FA41).
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL (todos con hook RH, apagado durante el banco):
//   CPad::GetSteeringLeftRight 0x53FB80, CPad::GetSteeringUpDown 0x53FBD0, CPad::GetCarGunFired 0x53FF90 (thiscall sin argumentos, resultado en AX),
//   CamShakeNoPos 0x50A970 (cdecl), CCamera::FindCamFOV 0x50AD20 (thiscall, resultado en ST0) y CWeapon::FireInstantHit 0x73FB10 (thiscall ret 20h).
// El constructor CWeapon (0x73B430) corre de verdad en ambos lados (el EXE entra por su hook a la DLL; lo valida su propio banco): el falso de FireInstantHit registra solo los campos que ese constructor escribe.

namespace Pc74Layout {
struct InfoProbe : CPlayerInfo {
    static constexpr size_t OffCross() { return offsetof(InfoProbe, m_nCrosshairActivated); }
    static constexpr size_t OffTarget() { return offsetof(InfoProbe, m_vecCrosshairTarget); }
};
static_assert(InfoProbe::OffCross() == Pc74Fix::kCrossOff, "CPlayerInfo: m_nCrosshairActivated en +0x158");
static_assert(InfoProbe::OffTarget() == Pc74Fix::kCrossOff + 4, "CPlayerInfo: m_vecCrosshairTarget en +0x15C");
static_assert(sizeof(CPlayerInfo) == Pc74Fix::kInfoWin, "CPlayerInfo: tamano 0x190");
struct CamProbe : CCamera {
    static constexpr size_t OffMatrix() { return offsetof(CamProbe, m_mCameraMatrix); }
};
static_assert(CamProbe::OffMatrix() == 0x974 && Pc74Fix::kCamMatrix == Pc74Fix::kCamBase + 0x974, "CCamera: m_mCameraMatrix en +0x974");
static_assert(offsetof(CPlaceable, m_placement) == 4 && offsetof(CPlaceable, m_matrix) == 0x14, "CPlaceable: posicion compacta en +4 y matriz en +0x14");
static_assert(sizeof(CWeapon) == 0x1C, "CWeapon");
}

namespace Pc74Live {
using namespace Pc74Fix;
inline constexpr uintptr_t kEntry=0x56EC80;
inline constexpr uintptr_t kLR=0x53FB80,kUD=0x53FBD0,kGun=0x53FF90,kShake=0x50A970,kFov=0x50AD20,kFire=0x73FB10;
inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}

template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kInfoArena> infoA;
inline Arena<kPadArena> padA;
inline Arena<kPedArena> pedA;
inline Arena<kMatArena> matV;

inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

struct State {const C* c{};unsigned fovCalls{};};
inline State g;
// Eventos: 1 GetSteeringLeftRight(this); 2 GetSteeringUpDown(this); 3 GetCarGunFired(this); 4 CamShakeNoPos(camara, fuerza); 5 FindCamFOV(this);
// 6 FireInstantHit(campos del arma 0,4,8,0xC,0x10,0x18 y bytes 0x14/0x15, ped, banderas del ped (+0x470) en ese momento, origen[3], muzzle[3], entidad objetivo, objetivo[3], origen de drive-by, dos bool).
inline u32 __fastcall FakeLR(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);return g.c->lr;}
inline u32 __fastcall FakeUD(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);return g.c->ud;}
inline u32 __fastcall FakeGun(void* self,void*) {Ev& e=PushEv(3,1);e.v[0]=Tag(self);return g.c->gun;}
inline void __cdecl FakeShake(void* cam,u32 strength) {Ev& e=PushEv(4,2);e.v[0]=Tag(cam);e.v[1]=strength;}
inline float __fastcall FakeFov(void* self,void*) {Ev& e=PushEv(5,1);e.v[0]=Tag(self);const u32 b=g.fovCalls++==0?g.c->fov1:g.c->fov2;return std::bit_cast<float>(b);}
inline u32 StackTag(const void* p) {return (Tag(p)==u32(uintptr_t(p)))?0xF00D0001u:Tag(p);}
inline u32 __fastcall FakeFire(void* weapon,void*,void* ped,const u32* origin,const u32* muzzle,void* target,const u32* aim,const u32* drive,u32 b1,u32 b2) {
    Ev& e=PushEv(6,26);
    const uint8* w=static_cast<const uint8*>(weapon);
    u32 t;
    memcpy(&t,w,4);e.v[0]=t;memcpy(&t,w+4,4);e.v[1]=t;memcpy(&t,w+8,4);e.v[2]=t;memcpy(&t,w+0xC,4);e.v[3]=t;memcpy(&t,w+0x10,4);e.v[4]=t;memcpy(&t,w+0x18,4);e.v[5]=Tag(uintptr_t(t));
    e.v[6]=w[0x14];e.v[7]=w[0x15];
    e.v[8]=Tag(ped);
    u32* flags=reinterpret_cast<u32*>(static_cast<uint8*>(ped)+0x470);
    e.v[9]=*flags;
    e.v[10]=StackTag(origin);e.v[11]=origin[0];e.v[12]=origin[1];e.v[13]=origin[2];
    e.v[14]=StackTag(muzzle);e.v[15]=muzzle[0];e.v[16]=muzzle[1];e.v[17]=muzzle[2];
    e.v[18]=Tag(target);
    e.v[19]=StackTag(aim);e.v[20]=aim[0];e.v[21]=aim[1];e.v[22]=aim[2];
    e.v[23]=Tag(drive);e.v[24]=b1;e.v[25]=b2;
    *flags^=g.c->fireXor;
    return g.c->fireRet;
}

inline void CaseCross(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        infoA.Fill(c.seed*13u+1u);padA.Fill(c.seed*13u+2u);pedA.Fill(c.seed*13u+3u);matV.Fill(c.seed*13u+4u);
        infoA.Put32(kCrossOff,c.active);infoA.Put32(kCrossOff+4,c.tx);infoA.Put32(kCrossOff+8,c.ty);
        pedA.Put32(0x470,c.flags);
        for(int i=0;i<3;++i) matV.Put32(0x30+4*i,c.cpos[i]);
        infoA.Snap();padA.Snap();pedA.Snap();matV.Snap();
        TagClear();infoA.Tagged();padA.Tagged();pedA.Tagged();matV.Tagged();
        G32(kTimeAddr)=G32(kTimeAddr);   // sin cambio: solo se vigila
        G32(kStepAddr)=c.ts;
        *reinterpret_cast<uint8*>(kInvertAddr)=uint8(c.inv);
        G32(kAspectAddr)=c.asp;
        G32(kCamBase+0x14)=c.hasCamMat?u32(matV.Obj()):0u;
        for(int i=0;i<3;++i) G32(kCamBase+4+4*i)=c.ppos[i];
        for(int i=0;i<3;++i) {G32(kCamMatrix+4*i)=c.mat[i];G32(kCamMatrix+0x10+4*i)=c.mat[3+i];G32(kCamMatrix+0x20+4*i)=c.mat[6+i];G32(kCamMatrix+0x30+4*i)=c.mat[9+i];}
        G32(kPlayers+c.player*0x190)=u32(pedA.Obj());
        g.c=&c;g.fovCalls=0;nev=0;
        const u32 args[2]{c.player,u32(padA.Obj())};
        const u32 self=u32(infoA.Obj())+u32(kCrossOff);
        sides[s]=Execute(env,0,kEntry,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,self,c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({infoA.Changes(),padA.Changes(),pedA.Changes(),matV.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),w.saved.size())}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseCross(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline bool FindHook(uintptr_t exe,std::string& cat,std::string& name) {
    bool found=false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for(auto& h:c.Items())
            if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple
                &&reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress())==exe) {
                cat=std::string(c.Name());name=std::string(h->Name());found=true;
            }
    });
    return found;
}
inline Pair MakeCollab(Env& env,uintptr_t exe,json& hooksFound) {
    Pair p{exe,0,false};
    std::string cat,name;
    if(FindHook(exe,cat,name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exe,HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe",exe},{"class",cat},{"name",name}});
        if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
    }
    return p;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playercrosshair74-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playercrosshair74-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playercrosshair74");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    {
        std::string cat,name;
        const bool found=FindHook(kEntry,cat,name);
        if(found)env.route[0].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry,HookRoute::RegisteredTarget{}));
        registered[kFn[0]]=found&&env.route[0]->Registered();bodies[kFn[0]]=found&&env.route[0]->Body()!=0;
        ready&=found&&env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[0]]=found?json({{"class",cat},{"name",name},{"address",kEntry}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){Run(env,false,registered,bodies,collaborators);return;}
    const Pair pLR=MakeCollab(env,kLR,hooksFound),pUD=MakeCollab(env,kUD,hooksFound),pGun=MakeCollab(env,kGun,hooksFound),pShake=MakeCollab(env,kShake,hooksFound),pFov=MakeCollab(env,kFov,hooksFound),pFire=MakeCollab(env,kFire,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pLR,reinterpret_cast<void*>(&FakeLR)),S(pUD,reinterpret_cast<void*>(&FakeUD)),S(pGun,reinterpret_cast<void*>(&FakeGun)),S(pShake,reinterpret_cast<void*>(&FakeShake)),S(pFov,reinterpret_cast<void*>(&FakeFov)),S(pFire,reinterpret_cast<void*>(&FakeFire))};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CWeapon::CWeapon 0x73B430 (el EXE entra por su hook a la DLL)"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    env.watches.emplace_back(kEntry,16);
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
