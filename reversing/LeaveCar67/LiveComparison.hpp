#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "TaskSimpleGangDriveBy.h"
#include "Animation/AnimManager.h"
#include "Animation/AnimBlendAssociation.h"
#include "Streaming.h"
#include "General.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCar67 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion: CTaskSimpleGangDriveBy::SetupAnim (0x627B20). Los colaboradores se sustituyen por falsos identicos en ambos lados.

namespace Lc67Layout {
struct GangProbe : CTaskSimpleGangDriveBy {
    static constexpr size_t OffAnimsRef() {return offsetof(GangProbe, m_bAnimsReferenced);}
    static constexpr size_t OffInWeaponRange() {return offsetof(GangProbe, m_bInWeaponRange);}
    static constexpr size_t OffNext() {return offsetof(GangProbe, m_nNextCommand);}
    static constexpr size_t OffLast() {return offsetof(GangProbe, m_nLastCommand);}
    static constexpr size_t OffBurst() {return offsetof(GangProbe, m_nBurstShots);}
    static constexpr size_t OffTimer() {return offsetof(GangProbe, m_nAttackTimer);}
    static constexpr size_t OffAnimId() {return offsetof(GangProbe, m_nRequiredAnimID);}
    static constexpr size_t OffGroup() {return offsetof(GangProbe, m_nRequiredAnimGroup);}
    static constexpr size_t OffAssoc() {return offsetof(GangProbe, m_pAnimAssoc);}
    static constexpr size_t OffWeapon() {return offsetof(GangProbe, m_pWeaponInfo);}
};
static_assert(GangProbe::OffAnimsRef() == 9 && GangProbe::OffInWeaponRange() == 0xC && GangProbe::OffNext() == 0xF && GangProbe::OffLast() == 0x10 && GangProbe::OffBurst() == 0x11
    && GangProbe::OffTimer() == 0x16 && GangProbe::OffAnimId() == 0x24 && GangProbe::OffGroup() == 0x28 && GangProbe::OffAssoc() == 0x2C && GangProbe::OffWeapon() == 0x30
    && sizeof(CTaskSimpleGangDriveBy) == 0x44, "CTaskSimpleGangDriveBy");
static_assert(sizeof(CAnimBlendAssocGroup) == 0x14, "CAnimBlendAssocGroup");
static_assert(sizeof(CAnimBlock) == 0x20 && offsetof(CAnimBlock, IsLoaded) == 0x10, "CAnimBlock");
static_assert(offsetof(CAnimBlendAssociation, m_BlendAmount) == 0x18 && offsetof(CAnimBlendAssociation, m_BlendDelta) == 0x1C && offsetof(CAnimBlendAssociation, m_AnimId) == 0x2C
    && offsetof(CAnimBlendAssociation, m_Flags) == 0x2E, "CAnimBlendAssociation");
}

namespace Lc67Live {
using namespace Lc67Fix;
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put16(size_t off,u32 v) {const uint16 w=uint16(v);memcpy(m.data()+kGuard+off,&w,2);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kPedArena> pedA;
inline Arena<kAssocArena> assocA,assocB;
inline Arena<kWeaponArena> weaponA;
inline Arena<kBlockArena> blockA;
inline Arena<kGroupsArena> groupsA;

inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
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

// ================================================================================================ SetupAnim (0x627B20)
struct State {const C* c{};uintptr_t blockPtr{};uintptr_t exeLoaded{};uintptr_t exeNot{};};
inline State g;
inline const char nameBuf[]="ped67";
// Todos cdecl salvo los miembros (thiscall: this en ECX, el que llama no limpia: `ret n`). Los floats viajan como dword en la pila.
inline u32 __cdecl FakeBlockName(u32 group) {Ev& e=PushEv(1,1);e.v[0]=group;return u32(uintptr_t(nameBuf));}                                              // 0x4D3A30, CAnimManager::GetAnimBlockName
inline u32 __cdecl FakeGetBlock(const char* name) {Ev& e=PushEv(2,1);e.v[0]=FnvStr(name);return u32(g.blockPtr);}                                         // 0x4D3940, CAnimManager::GetAnimationBlock(const char*)
inline void __cdecl FakeRequest(u32 modelId,u32 flags) {Ev& e=PushEv(3,2);e.v[0]=modelId;e.v[1]=flags;}                                                    // 0x4087E0, CStreaming::RequestModel
inline void __cdecl FakeAddRef(u32 index) {Ev& e=PushEv(4,1);e.v[0]=index;}                                                                                // 0x4D3FB0, CAnimManager::AddAnimBlockRef
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 animId,u32 deltaBits) {Ev& e=PushEv(5,4);e.v[0]=clump;e.v[1]=group;e.v[2]=animId;e.v[3]=deltaBits;return u32(assocB.Obj());}   // 0x4D4610
inline void __fastcall FakeSetCb(void* self,void*,u32 cb,u32 data) {Ev& e=PushEv(6,3);e.v[0]=Tag(self);e.v[1]=cb;e.v[2]=Tag(uintptr_t(data));}              // 0x4CEBC0, `ret 8`
inline u32 __cdecl FakeRand() {PushEv(7,0);return g.c->randRet&0xFFFFu;}                                                                                   // 0x821B1E, cdecl (y CGeneral::GetRandomNumber de la DLL, uint16; el EXE enmascara con 0xFFFF)
inline void __fastcall FakeSetTime(void* self,void*,u32 bits) {Ev& e=PushEv(8,2);e.v[0]=Tag(self);e.v[1]=bits;}                                           // 0x4CEA80, `ret 4`
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(9,1);e.v[0]=Tag(self);return g.c->isPlayerRet;}                                          // 0x5DF8F0, `ret`

inline void CaseSetupAnim(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*7u+1u);pedA.Fill(c.seed*7u+2u);assocA.Fill(c.seed*7u+3u);assocB.Fill(c.seed*7u+4u);weaponA.Fill(c.seed*7u+5u);blockA.Fill(c.seed*7u+6u);groupsA.Fill(c.seed*7u+7u);
        const uintptr_t blockPtr=c.blk==0?blockA.Obj():c.blk==1?g.exeLoaded:g.exeNot;
        g.blockPtr=blockPtr;
        blockA.Put8(0x10,c.loaded);
        groupsA.Put32(size_t(c.group)*kGroupStride,c.viaGroup?u32(blockPtr):0u);
        pedA.Put32(0x18,c.clump);
        assocA.Put32(0x18,c.aAmp);assocA.Put32(0x1C,c.aDel);assocA.Put32(0x2C,(c.aFlags<<16)|(c.aId&0xFFFFu));
        assocB.Put32(0x18,c.bAmp);assocB.Put32(0x1C,c.bDel);assocB.Put32(0x2C,(c.bFlags<<16)|(c.bId&0xFFFFu));
        weaponA.Put32(0x20,c.clip);
        taskA.Put8(9,c.refd);taskA.Put8(0xC,c.inRange);taskA.Put8(0xF,c.next);taskA.Put8(0x10,c.last);taskA.Put8(0x11,c.burst);taskA.Put16(0x16,c.timer);
        taskA.Put32(0x24,c.reqId);taskA.Put32(0x28,c.group);taskA.Put32(0x2C,c.hasAssoc?u32(assocA.Obj()):0u);taskA.Put32(0x30,c.wKind?u32(weaponA.Obj()):0u);
        taskA.Snap();pedA.Snap();assocA.Snap();assocB.Snap();weaponA.Snap();blockA.Snap();groupsA.Snap();
        TagClear();taskA.Tagged();pedA.Tagged();assocA.Tagged();assocB.Tagged();weaponA.Tagged();groupsA.Tagged();blockA.Tagged();
        G32(kGroupsPtr)=u32(groupsA.Obj());
        g.c=&c;nev=0;
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x627B20,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // los dos llamadores del EXE (0x62D2EA, 0x62D5C4) hacen test al,al; EAX alto, ECX y EDX solo registrados
        obs["changes"]=json::array({taskA.Changes(),pedA.Changes(),assocA.Changes(),assocB.Changes(),weaponA.Changes(),groupsA.Changes(),blockA.Changes()});
        obs["events"]=EvsJson();
        obs["objs"]=json::array({u32(taskA.Obj()),u32(pedA.Obj()),u32(assocA.Obj()),u32(assocB.Obj()),u32(weaponA.Obj()),u32(groupsA.Obj()),u32(blockA.Obj())});
        obs["blocks"]=json::array({u32(blockA.Obj()),u32(g.exeLoaded),u32(g.exeNot)});
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
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
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
            for(unsigned c=0;c<kCases;++c)CaseSetupAnim(env,totals,c);
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
    if(!std::filesystem::exists(dir/"leavecar67-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecar67-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecar67");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CTaskSimpleGangDriveBy","SetupAnim",0x627B20,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    // Bloques "del EXE" con indice negativo: direcciones alineadas a 0x20 en .rdata (constantes, nunca parcheadas ni escritas) cuyo byte +0x10 (IsLoaded) vale 1 / 0
    for(uintptr_t a=0x858000;a<0x870000&&(!g.exeLoaded||!g.exeNot);a+=0x20) {
        const uint8 b=*reinterpret_cast<const uint8*>(a+0x10);
        if(b==1&&!g.exeLoaded)g.exeLoaded=a;
        if(b==0&&!g.exeNot)g.exeNot=a;
    }
    ready&=g.exeLoaded&&g.exeNot;
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI). Todos tienen hook RH salvo rand() del EXE 0x821B1E
    // (la DLL lo llama por la direccion del EXE).
    env.stubs[0]={{Pair{0x4D3A30,DllBody(&CAnimManager::GetAnimBlockName),true},reinterpret_cast<void*>(&FakeBlockName)},
                  {Pair{0x4D3940,DllBody(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),true},reinterpret_cast<void*>(&FakeGetBlock)},
                  {Pair{0x4087E0,DllBody(&CStreaming::RequestModel),true},reinterpret_cast<void*>(&FakeRequest)},
                  {Pair{0x4D3FB0,DllBody(&CAnimManager::AddAnimBlockRef),true},reinterpret_cast<void*>(&FakeAddRef)},
                  {Pair{0x4D4610,DllBody(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),true},reinterpret_cast<void*>(&FakeBlend)},
                  {Pair{0x4CEBC0,DllBody(&CAnimBlendAssociation::SetDeleteCallback),true},reinterpret_cast<void*>(&FakeSetCb)},
                  {Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)},
                  {Pair{0x4CEA80,DllBody(&CAnimBlendAssociation::SetCurrentTime),true},reinterpret_cast<void*>(&FakeSetTime)},
                  {Pair{0x5DF8F0,DllBody(&CPed::IsPlayer),true},reinterpret_cast<void*>(&FakeIsPlayer)}};
    // rand() de la DLL (CGeneral::GetRandomNumber, la que usa GetRandomIntInRange en linea y la envoltura de 0x407180): sustituida en ambos lados por el mismo falso que la rand() del EXE
    env.dllOnly={{Pair{0,DllBody(&CGeneral::GetRandomNumber),false},reinterpret_cast<void*>(&FakeRand)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
