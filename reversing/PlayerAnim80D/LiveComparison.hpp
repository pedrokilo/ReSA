#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <utility>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Entity/Ped/PlayerPed.h"
#include "PlayerPedData.h"
#include "PedClothesDesc.h"
#include "PedIntelligence.h"
#include "WeaponInfo.h"
#include "Clothes.h"
#include "VisibilityPlugins.h"
#include "Animation/AnimManager.h"
#include "Streaming.h"
#include "Cam.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlayerAnim80D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CPlayerPed::ProcessAnimGroups (0x6098F0, hook S). Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en la direccion del EXE y, si tienen hook,
// en el cuerpo de la DLL): CCam::Using3rdPersonMouseCam 0x50A850, CPed::CanStrafeOrMouseControl 0x5DF090, CVisibilityPlugins::GetClumpModelInfo 0x732AC0, CPedIntelligence::GetTaskJetPack 0x601110,
// CPedClothesDesc::GetIsWearingBalaclava 0x5A7950, CClothes::GetDefaultPlayerMotionGroup 0x5A81B0, CPlayerPed::ReApplyMoveAnims 0x609650, CPed::GetWeaponSkill() 0x5E6580,
// CWeaponInfo::GetWeaponInfo 0x743C60, CAnimManager::GetAnimBlockName 0x4D3A30, GetAnimationBlock(const char*) 0x4D3940, AddAnimBlockRef 0x4D3FB0, CStreaming::RequestModel 0x4087E0 (todos con hook);
// y sin hook (funcion C++ de la DLL sin RH_ScopedInstall; falso tambien en el cuerpo de la DLL): CAnimManager::RemoveAnimBlockRef 0x4D3FD0.
// El modelo del arma es un objeto falso con una tabla virtual falsa (la ranura +0x10 es GetModelType; las demas ranuras registran un evento 90+ranura): asi se comprueba que la DLL
// llama a la ranura correcta. Los bloques de animacion son los reales de ms_aAnimBlocks (0..7, solo se lee IsLoaded), el arreglo de grupos 0xB4EA34 apunta a una arena y los datos de la tabla
// de combos 0xC170D0 y de ms_modelInfoPtrs se fijan por caso y se restauran.
// Mutaciones: en el sitio elegido un falso cambia el puntero a los datos del jugador (+0x480), sus contadores +0x48/+0x4C, el bloque de un grupo o el grupo de animacion del jugador (+0x4D4),
// para distinguir lecturas repetidas de lecturas conservadas.

namespace Pa80Layout {
struct PaPlayer : CPlayerPed {
    static constexpr size_t OffPlayerData() { return offsetof(PaPlayer, m_pPlayerData); }
    static constexpr size_t OffIntelligence() { return offsetof(PaPlayer, m_pIntelligence); }
    static constexpr size_t OffAnimGroup() { return offsetof(PaPlayer, m_nAnimGroup); }
    static constexpr size_t OffWeaponObject() { return offsetof(PaPlayer, m_pWeaponObject); }
    static constexpr size_t OffPedType() { return offsetof(PaPlayer, m_nPedType); }
    static constexpr size_t OffWeapons() { return offsetof(PaPlayer, m_aWeapons); }
    static constexpr size_t OffSlot() { return offsetof(PaPlayer, m_nActiveWeaponSlot); }
    static constexpr size_t OffStyle() { return offsetof(PaPlayer, m_nFightingStyle); }
    static constexpr size_t OffModelIndex() { return offsetof(PaPlayer, m_nModelIndex); }
};
static_assert(PaPlayer::OffPlayerData() == 0x480 && PaPlayer::OffIntelligence() == 0x47C && PaPlayer::OffAnimGroup() == 0x4D4 && PaPlayer::OffWeaponObject() == 0x4F4 && PaPlayer::OffPedType() == 0x598 &&
              PaPlayer::OffWeapons() == 0x5A0 && PaPlayer::OffSlot() == 0x718 && PaPlayer::OffStyle() == 0x72D && PaPlayer::OffModelIndex() == 0x22 && sizeof(CWeapon) == 0x1C,
              "CPlayerPed: datos +0x480, inteligencia +0x47C, grupo +0x4D4, objeto del arma +0x4F4, tipo +0x598, armas +0x5A0 (0x1C), ranura +0x718, estilo +0x72D, modelo +0x22");
static_assert(offsetof(CPlayerPedData, m_pPedClothesDesc) == 4 && offsetof(CPlayerPedData, m_nMeleeWeaponAnimReferenced) == 0x48 && offsetof(CPlayerPedData, m_nMeleeWeaponAnimReferencedExtra) == 0x4C &&
              offsetof(CPlayerPedData, m_fFPSMoveHeading) == 0x50, "CPlayerPedData: ropa +4, referencias +0x48/+0x4C, rumbo +0x50");
static_assert(offsetof(CWeaponInfo, m_nWeaponFire) == 0 && offsetof(CWeaponInfo, m_nBaseCombo) == 0x6E, "CWeaponInfo: disparo +0, combo base +0x6E");
static_assert(offsetof(CPedModelInfo, m_nAnimType) == 0x24 && offsetof(CAnimBlock, IsLoaded) == 0x10 && sizeof(CAnimBlock) == 0x20 && sizeof(CAnimBlendAssocGroup) == 0x14 && offsetof(CAnimBlendAssocGroup, m_AnimBlock) == 0 &&
              sizeof(CCam) == 0x238, "modelo de peaton +0x24, bloques de 0x20, grupos de 0x14, camaras de 0x238");
}

namespace Pa80Live {
using namespace Pa80Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(ptrdiff_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(ptrdiff_t off,u32 v) {const u16 w=u16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(ptrdiff_t off,u32 v) {m[kGuard+off]=u8(v);}
    u32 Get32(ptrdiff_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPlayerArena> playerA;
inline Arena<kDataArena> pdA,pd2A;
inline Arena<kSmallArena> intelA,clumpA,modelA,pedModelA,winfoA,descA,desc2A;
inline Arena<kGroupsArena> groupsA;

inline uintptr_t Addr(auto member) {return Body(std::bit_cast<uintptr_t>(member));}
inline constexpr uintptr_t kBlocksBase=0xB5D4A0,kCamsBase=0xB6F19C;
inline u32 BlockPtr(u32 k1) {return k1?u32(kBlocksBase+(k1-1)*0x20):0u;}   // 0 = nulo, 1 + k = bloque real k

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
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

// ================================================================================================ ProcessAnimGroups (0x6098F0)
struct State {const C* c{};std::array<bool,kSites+1> mutDone{};};
inline State g;
// Mutacion en el sitio `site` (una sola vez por sitio): kind&1 cambia el puntero a los datos del jugador (+0x480) al otro; kind&2 fija +0x48 y kind&4 fija +0x4C de los datos vigentes (tras el cambio);
// kind&8 cambia el bloque del grupo mutG; kind&16 cambia el grupo de animacion del jugador (+0x4D4).
inline void Mutate(u32 site) {
    if(g.c->mutSite!=site||g.mutDone[site])return;
    g.mutDone[site]=true;
    const u32 k=g.c->mutKind;
    if(k&1)playerA.Put32(0x480,playerA.Get32(0x480)==u32(pdA.Obj())?u32(pd2A.Obj()):u32(pdA.Obj()));
    auto& cur=playerA.Get32(0x480)==u32(pdA.Obj())?pdA:pd2A;
    if(k&2)cur.Put32(0x48,g.c->mutA);
    if(k&4)cur.Put32(0x4C,g.c->mutB);
    if(k&8)groupsA.Put32(ptrdiff_t(g.c->mutG)*20,BlockPtr(g.c->mutBlk));
    if(k&16)playerA.Put32(0x4D4,g.c->mutAG);
}

// Todos __fastcall (ECX = this, EDX sin uso, pila) o __cdecl, con la limpieza del original.
inline u32 __fastcall FakeMouseCam(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=u32((uintptr_t(self)-kCamsBase)/0x238);Mutate(1);return g.c->mouse;}               // 0x50A850
inline u32 __fastcall FakeStrafe(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);Mutate(2);return g.c->strafe;}                                              // 0x5DF090
inline void* __cdecl FakeClump(void* clump) {Ev& e=PushEv(3,1);e.v[0]=Tag(clump);Mutate(3);return g.c->clumpRet?reinterpret_cast<void*>(modelA.Obj()):nullptr;}      // 0x732AC0
inline u32 __fastcall FakeModelType(void* self,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(self);Mutate(4);return g.c->mtype;}                                            // vtable +0x10
template<unsigned K> inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(90+K,1);e.v[0]=Tag(self);return 0;}                                             // otras ranuras
inline std::array<void*,16> vtable{};
template<size_t... I> inline void FillVtable(std::index_sequence<I...>) {((vtable[I]=reinterpret_cast<void*>(&FakeTrap<unsigned(I)>)),...);vtable[4]=reinterpret_cast<void*>(&FakeModelType);}
inline u32 __fastcall FakeJet(void* self,void*) {Ev& e=PushEv(5,1);e.v[0]=Tag(self);Mutate(5);return g.c->jet;}                                                    // 0x601110
inline u32 __fastcall FakeBalaclava(void* self,void*) {Ev& e=PushEv(6,1);e.v[0]=Tag(self);Mutate(6);return g.c->bal;}                                              // 0x5A7950
inline u32 __cdecl FakeDefaultGroup() {PushEv(7,0);Mutate(7);return g.c->defGroup;}                                                                                  // 0x5A81B0
inline void __fastcall FakeReApply(void* self,void*) {Ev& e=PushEv(8,1);e.v[0]=Tag(self);Mutate(8);}                                                                 // 0x609650
inline u32 __fastcall FakeSkill(void* self,void*) {Ev& e=PushEv(9,1);e.v[0]=Tag(self);Mutate(9);return g.c->skill;}                                                // 0x5E6580
inline void* __cdecl FakeWInfo(u32 type,u32 skill) {Ev& e=PushEv(10,2);e.v[0]=type;e.v[1]=skill;Mutate(10);return reinterpret_cast<void*>(winfoA.Obj());}            // 0x743C60
inline const char* BlockName(u32 group) {static const std::array<std::string,64> names=[]{std::array<std::string,64> a;for(unsigned i=0;i<a.size();++i)a[i]="AB"+std::to_string(i);return a;}();return names[group%64].c_str();}
inline const char* __cdecl FakeBlockName(u32 group) {Ev& e=PushEv(11,1);e.v[0]=group;Mutate(11);return BlockName(group);}                                          // 0x4D3A30
inline void* __cdecl FakeGetBlock(const char* name) {Ev& e=PushEv(12,1);e.v[0]=FnvStr(name);Mutate(12);return reinterpret_cast<void*>(size_t(BlockPtr(1+g.c->gb)));}  // 0x4D3940
inline void __cdecl FakeAddRef(u32 idx) {Ev& e=PushEv(13,1);e.v[0]=idx;Mutate(13);}                                                                                 // 0x4D3FB0
inline void __cdecl FakeRemoveRef(u32 idx) {Ev& e=PushEv(14,1);e.v[0]=idx;Mutate(14);}                                                                              // 0x4D3FD0 (sin hook)
inline void __cdecl FakeRequest(u32 model,u32 flags) {Ev& e=PushEv(15,2);e.v[0]=model;e.v[1]=flags;Mutate(15);}                                                      // 0x4087E0

inline void CaseAnim(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        playerA.Fill(c.seed*29u+1u);pdA.Fill(c.seed*29u+2u);pd2A.Fill(c.seed*29u+3u);intelA.Fill(c.seed*29u+4u);clumpA.Fill(c.seed*29u+5u);modelA.Fill(c.seed*29u+6u);pedModelA.Fill(c.seed*29u+7u);
        winfoA.Fill(c.seed*29u+8u);groupsA.Fill(c.seed*29u+9u);descA.Fill(c.seed*29u+10u);desc2A.Fill(c.seed*29u+11u);
        // jugador: primero los tipos de arma de las 41 ranuras (la ranura -11 coincide con las banderas +0x46C: el fixture las iguala), despues el resto de campos
        for(unsigned i=0;i<kSlots;++i)playerA.Put32(0x5A0+ptrdiff_t(int(i)-20)*0x1C,c.wt[i]);
        playerA.Put32(0x480,u32(pdA.Obj()));playerA.Put32(0x47C,u32(intelA.Obj()));playerA.Put32(0x4D4,c.animGroup);playerA.Put32(0x4F4,c.wobj?u32(clumpA.Obj()):0u);
        playerA.Put32(0x598,c.pedType);playerA.Put16(0x22,c.midx);playerA.Put8(0x718,c.slot);playerA.Put8(0x72D,c.style);playerA.Put32(0x46C,c.pflags);
        pdA.Put32(4,u32(descA.Obj()));pdA.Put32(0x48,c.f48);pdA.Put32(0x4C,c.f4C);pdA.Put32(0x50,c.hdg);
        pd2A.Put32(4,u32(desc2A.Obj()));pd2A.Put32(0x48,c.f48b);pd2A.Put32(0x4C,c.f4Cb);pd2A.Put32(0x50,Rand(c.seed,903));
        modelA.Put32(0,u32(uintptr_t(vtable.data())));modelA.Put32(0x24,c.wtype);
        pedModelA.Put32(0x24,c.animType);
        winfoA.Put32(0,c.fire);winfoA.Put8(0x6E,c.baseCombo);
        for(unsigned gi=0;gi<kGroups;++gi)groupsA.Put32(ptrdiff_t(gi)*20,BlockPtr(c.blk[gi]));
        // globales: camara activa, arreglo de grupos, bytes IsLoaded, tabla de combos y modelos
        *reinterpret_cast<uint8*>(0xB6F081)=u8(c.camIdx);*reinterpret_cast<uint32*>(0xB4EA34)=u32(groupsA.Obj());
        for(unsigned k=0;k<kBlocks;++k)*reinterpret_cast<uint8*>(kBlocksBase+k*0x20+0x10)=u8(c.loaded[k]);
        for(unsigned i=0;i<12;++i)*reinterpret_cast<uint32*>(0xC170D0+i*0x88)=c.row[i];
        for(unsigned i=0;i<8;++i)*reinterpret_cast<uint32*>(0xA9B0C8+i*4)=u32(pedModelA.Obj());
        playerA.Snap();pdA.Snap();pd2A.Snap();intelA.Snap();clumpA.Snap();modelA.Snap();pedModelA.Snap();winfoA.Snap();groupsA.Snap();descA.Snap();desc2A.Snap();
        TagClear();playerA.Tagged();pdA.Tagged();pd2A.Tagged();intelA.Tagged();clumpA.Tagged();modelA.Tagged();pedModelA.Tagged();winfoA.Tagged();groupsA.Tagged();descA.Tagged();desc2A.Tagged();
        g=State{};g.c=&c;nev=0;
        sides[s]=Execute(env,0,0x6098F0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(playerA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["changes"]=json::array({playerA.Changes(),pdA.Changes(),pd2A.Changes(),intelA.Changes(),clumpA.Changes(),modelA.Changes(),pedModelA.Changes(),winfoA.Changes(),groupsA.Changes(),descA.Changes(),desc2A.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
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
            for(unsigned c=0;c<kCases;++c)CaseAnim(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playeranim80d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playeranim80d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("playeranim80d");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    FillVtable(std::make_index_sequence<16>{});
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear la ruta y de escribir ningun parche (con su hook encendido la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t mouseBody=Addr(&CCam::Using3rdPersonMouseCam);
    const uintptr_t strafeBody=Addr(&CPed::CanStrafeOrMouseControl);
    const uintptr_t clumpBody=Addr(&CVisibilityPlugins::GetClumpModelInfo);
    const uintptr_t jetBody=Addr(&CPedIntelligence::GetTaskJetPack);
    const uintptr_t balBody=Addr(&CPedClothesDesc::GetIsWearingBalaclava);
    const uintptr_t defBody=Addr(&CClothes::GetDefaultPlayerMotionGroup);
    const uintptr_t reapplyBody=Addr(&CPlayerPed::ReApplyMoveAnims);
    const uintptr_t skillBody=Addr(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill));
    const uintptr_t winfoBody=Addr(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo));
    const uintptr_t nameBody=Addr(&CAnimManager::GetAnimBlockName);
    const uintptr_t blockBody=Addr(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock));
    const uintptr_t addBody=Addr(&CAnimManager::AddAnimBlockRef);
    const uintptr_t removeBody=Addr(&CAnimManager::RemoveAnimBlockRef);
    const uintptr_t requestBody=Addr(&CStreaming::RequestModel);
    env.route[0].reset(new HookRoute("CPlayerPed","ProcessAnimGroups",0x6098F0,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    ready&=mouseBody&&strafeBody&&clumpBody&&jetBody&&balBody&&defBody&&reapplyBody&&skillBody&&winfoBody&&nameBody&&blockBody&&addBody&&removeBody&&requestBody;
    env.stubs[0]={{Pair{0x50A850,mouseBody,true},reinterpret_cast<void*>(&FakeMouseCam)},
                  {Pair{0x5DF090,strafeBody,true},reinterpret_cast<void*>(&FakeStrafe)},
                  {Pair{0x732AC0,clumpBody,true},reinterpret_cast<void*>(&FakeClump)},
                  {Pair{0x601110,jetBody,true},reinterpret_cast<void*>(&FakeJet)},
                  {Pair{0x5A7950,balBody,true},reinterpret_cast<void*>(&FakeBalaclava)},
                  {Pair{0x5A81B0,defBody,true},reinterpret_cast<void*>(&FakeDefaultGroup)},
                  {Pair{0x609650,reapplyBody,true},reinterpret_cast<void*>(&FakeReApply)},
                  {Pair{0x5E6580,skillBody,true},reinterpret_cast<void*>(&FakeSkill)},
                  {Pair{0x743C60,winfoBody,true},reinterpret_cast<void*>(&FakeWInfo)},
                  {Pair{0x4D3A30,nameBody,true},reinterpret_cast<void*>(&FakeBlockName)},
                  {Pair{0x4D3940,blockBody,true},reinterpret_cast<void*>(&FakeGetBlock)},
                  {Pair{0x4D3FB0,addBody,true},reinterpret_cast<void*>(&FakeAddRef)},
                  {Pair{0x4D3FD0,removeBody,false},reinterpret_cast<void*>(&FakeRemoveRef)},
                  {Pair{0x4087E0,requestBody,true},reinterpret_cast<void*>(&FakeRequest)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"comparaciones de punto flotante del rumbo (+0x50)","aritmetica de indices de bloque y de la tabla de combos"})},{"stubbed",stubbed}};
    }
    env.watches.emplace_back(0x6098F0,16);
    env.watches.emplace_back(0x86D1D0,4);   // +0.8726646 (.rdata): limite superior del rumbo
    env.watches.emplace_back(0x86D1D4,4);   // -0.8726646 (.rdata): limite inferior del rumbo
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
