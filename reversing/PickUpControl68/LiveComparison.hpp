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
#include "Entity/Ped/PlayerPed.h"
#include "TaskComplexGoPickUpEntity.h"
#include "TaskSimplePickUpEntity.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "Animation/AnimManager.h"
#include "Pad.h"
#include "Streaming.h"
#include "General.h"
#include "Timer.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PickUpControl68 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una funcion VIRTUAL: CTaskComplexGoPickUpEntity::ControlSubTask (0x691D50, vtable 0x870B98 ranura 10). El cuerpo del candidato sale de ReversibleHook::Virtual::GetHookOurAddress().
// Los colaboradores se sustituyen por falsos identicos en ambos lados.

namespace Pu68Layout {
struct GoPickProbe : CTaskComplexGoPickUpEntity {
    static constexpr size_t OffSub() {return offsetof(GoPickProbe, m_pSubTask);}
    static constexpr size_t OffTime() {return offsetof(GoPickProbe, m_nTimePassedSinceLastSubTaskCreatedInMs);}
    static constexpr size_t OffGroup() {return offsetof(GoPickProbe, m_nAnimGroupId);}
    static constexpr size_t OffRef() {return offsetof(GoPickProbe, m_bAnimBlockReferenced);}
};
static_assert(GoPickProbe::OffSub() == 8 && GoPickProbe::OffTime() == 0x28 && GoPickProbe::OffGroup() == 0x2C && GoPickProbe::OffRef() == 0x30 && sizeof(CTaskComplexGoPickUpEntity) == 0x34, "CTaskComplexGoPickUpEntity");
struct GoToProbe : CTaskComplexGoToPointAndStandStill { static constexpr size_t OffTarget() {return offsetof(GoToProbe, m_vecTargetPoint);} };
static_assert(GoToProbe::OffTarget() == 0x10, "CTaskComplexGoToPointAndStandStill::m_vecTargetPoint");
struct PickUpProbe : CTaskSimplePickUpEntity {
    static constexpr size_t OffDropped() {return offsetof(PickUpProbe, m_bEntityDropped);}
    static constexpr size_t OffAssoc() {return offsetof(PickUpProbe, m_pAnimBlendAssociation);}
};
static_assert(PickUpProbe::OffDropped() == 0x34 && PickUpProbe::OffAssoc() == 0x38, "CTaskSimplePickUpEntity");
struct PedProbe : CPed {
    static constexpr size_t OffData() {return offsetof(PedProbe, m_pPlayerData);}
    static constexpr size_t OffMatrix() {return offsetof(PedProbe, m_matrix);}
};
static_assert(PedProbe::OffData() == 0x480 && PedProbe::OffMatrix() == 0x14 && sizeof(CPed) == 0x79C, "CPed");
static_assert(sizeof(CAnimBlendAssocGroup) == 0x14, "CAnimBlendAssocGroup");
static_assert(sizeof(CAnimBlock) == 0x20 && offsetof(CAnimBlock, IsLoaded) == 0x10, "CAnimBlock");
}

namespace Pu68Live {
using namespace Pu68Fix;
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSubArena> subA;
inline Arena<kPedArena> pedA;
inline Arena<kMatArena> matA;
inline Arena<kPadArena> padA;
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

// ================================================================================================ ControlSubTask (0x691D50)
struct State {const C* c{};uintptr_t blockPtr{};uintptr_t exeLoaded{};uintptr_t exeNot{};};
inline State g;
inline const char nameBuf[]="pick68";
enum EvKind : u32 {EvBlockName=1,EvGetBlock,EvRequest,EvAddRef,EvGetType,EvAbort,EvGetPad,EvUpDown,EvLeftRight,EvRadian,EvJump,EvSprint,EvTrap=99};
// Todos cdecl salvo los miembros (thiscall: this en ECX, el que llama no limpia salvo `ret n`). Los floats viajan como dword en la pila.
inline u32 __cdecl FakeBlockName(u32 group) {Ev& e=PushEv(EvBlockName,1);e.v[0]=group;return u32(uintptr_t(nameBuf));}                                  // 0x4D3A30, CAnimManager::GetAnimBlockName
inline u32 __cdecl FakeGetBlock(const char* name) {Ev& e=PushEv(EvGetBlock,1);e.v[0]=FnvStr(name);return u32(g.blockPtr);}                              // 0x4D3940, CAnimManager::GetAnimationBlock(const char*)
inline void __cdecl FakeRequest(u32 modelId,u32 flags) {Ev& e=PushEv(EvRequest,2);e.v[0]=modelId;e.v[1]=flags;}                                         // 0x4087E0, CStreaming::RequestModel
inline void __cdecl FakeAddRef(u32 index) {Ev& e=PushEv(EvAddRef,1);e.v[0]=index;}                                                                     // 0x4D3FB0, CAnimManager::AddAnimBlockRef
inline u32 __fastcall FakeGetPad(void* self,void*) {Ev& e=PushEv(EvGetPad,1);e.v[0]=Tag(self);return g.c->padPtr?u32(padA.Obj()):0u;}                   // 0x609560, CPlayerPed::GetPadFromPlayer
inline u32 __fastcall FakeUpDown(void* self,void*) {Ev& e=PushEv(EvUpDown,1);e.v[0]=Tag(self);return g.c->ud;}                                          // 0x53FD30, CPad::GetPedWalkUpDown() (AX con basura arriba)
inline u32 __fastcall FakeLeftRight(void* self,void*) {Ev& e=PushEv(EvLeftRight,1);e.v[0]=Tag(self);return g.c->lr;}                                    // 0x53FC90, CPad::GetPedWalkLeftRight()
inline float __cdecl FakeRadian(u32 x1,u32 y1,u32 x2,u32 y2) {Ev& e=PushEv(EvRadian,4);e.v[0]=x1;e.v[1]=y1;e.v[2]=x2;e.v[3]=y2;return F(g.c->rad);}    // 0x53CBE0, CGeneral::GetRadianAngleBetweenPoints (float en ST0)
inline u32 __fastcall FakeJump(void* self,void*) {Ev& e=PushEv(EvJump,1);e.v[0]=Tag(self);return g.c->jump;}                                           // 0x540770, CPad::JumpJustDown
inline u32 __fastcall FakeSprint(void* self,void*) {Ev& e=PushEv(EvSprint,1);e.v[0]=Tag(self);return g.c->sprint;}                                      // 0x5407F0, CPad::SprintJustDown
// vtable falsa de la subtarea: GetTaskType (ranura 4, thiscall sin argumentos) y MakeAbortable (ranura 6, thiscall 3 argumentos, `ret 0xC`)
inline u32 __fastcall FakeGetType(void* self,void*) {Ev& e=PushEv(EvGetType,1);e.v[0]=Tag(self);return g.c->subType;}
inline u32 __fastcall FakeMakeAbortable(void* self,void*,u32 ped,u32 prio,u32 evt) {Ev& e=PushEv(EvAbort,4);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ped));e.v[2]=prio;e.v[3]=Tag(uintptr_t(evt));return 0xA5A5A501u;}
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(EvTrap,1);e.v[0]=Tag(self);return 0;}
alignas(16) inline std::array<u32,16> subVtbl{};
inline void BuildSubVtbl() {
    for(auto& v:subVtbl)v=u32(uintptr_t(&FakeTrap));
    subVtbl[4]=u32(uintptr_t(&FakeGetType));
    subVtbl[6]=u32(uintptr_t(&FakeMakeAbortable));
}

inline void CaseControl(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    const json input=json::parse(JsonC(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*7u+1u);subA.Fill(c.seed*7u+2u);pedA.Fill(c.seed*7u+3u);matA.Fill(c.seed*7u+4u);padA.Fill(c.seed*7u+5u);blockA.Fill(c.seed*7u+6u);groupsA.Fill(c.seed*7u+7u);
        const uintptr_t blockPtr=c.blk==0?blockA.Obj():c.blk==1?g.exeLoaded:g.exeNot;
        g.blockPtr=blockPtr;
        blockA.Put8(0x10,c.loaded);
        groupsA.Put32(size_t(c.group)*kGroupStride,c.viaGroup?u32(blockPtr):0u);
        taskA.Put32(0,0x870B98);taskA.Put32(8,c.subKind?u32(subA.Obj()):0u);taskA.Put32(0x28,c.start);taskA.Put32(0x2C,c.group);taskA.Put8(0x30,c.refd);
        subA.Put32(0,u32(uintptr_t(subVtbl.data())));subA.Put32(0x10,c.tx);subA.Put32(0x14,c.ty);subA.Put8(0x34,c.dropped);subA.Put32(0x38,c.hasAssoc?u32(padA.Obj()):0u);
        pedA.Put32(4,c.px);pedA.Put32(8,c.py);pedA.Put32(0x14,c.hasMat?u32(matA.Obj()):0u);pedA.Put32(0x480,c.pdata?u32(blockA.Obj()):0u);
        matA.Put32(0x30,c.mx);matA.Put32(0x34,c.my);
        taskA.Snap();subA.Snap();pedA.Snap();matA.Snap();padA.Snap();blockA.Snap();groupsA.Snap();
        TagClear();taskA.Tagged();subA.Tagged();pedA.Tagged();matA.Tagged();padA.Tagged();groupsA.Tagged();blockA.Tagged();
        G32(kGroupsPtr)=u32(groupsA.Obj());G32(kTimeAddr)=c.now;G32(kCamAddr)=c.cam;
        g.c=&c;nev=0;
        const u32 args[1]{u32(pedA.Obj())};
        sides[s]=Execute(env,0,0x691D50,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["ret"]=Tag(uintptr_t(sides[s].reg.eax));   // EAX: la subtarea (arena) o 0; ECX y EDX solo registrados
        obs["changes"]=json::array({taskA.Changes(),subA.Changes(),pedA.Changes(),matA.Changes(),padA.Changes(),groupsA.Changes(),blockA.Changes()});
        obs["events"]=EvsJson();
        obs["objs"]=json::array({u32(taskA.Obj()),u32(subA.Obj()),u32(pedA.Obj()),u32(matA.Obj()),u32(padA.Obj()),u32(groupsA.Obj()),u32(blockA.Obj())});
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
            for(unsigned c=0;c<kCases;++c)CaseControl(env,totals,c);
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
    if(!std::filesystem::exists(dir/"pickupcontrol68-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pickupcontrol68-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickupcontrol68");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    BuildSubVtbl();
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Ruta VIRTUAL: el cuerpo del candidato es ReversibleHook::Virtual::GetHookOurAddress()
    {
        auto* hook=FindHook("CTaskComplexGoPickUpEntity","ControlSubTask");
        uintptr_t body=0;
        if(hook&&!strcmp(hook->Symbol(),"V"))body=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook)->GetHookOurAddress());
        env.route[0].reset(new HookRoute("CTaskComplexGoPickUpEntity","ControlSubTask",0x691D50,body));
        registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
        ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    }
    // Bloques "del EXE" con indice negativo: direcciones alineadas a 0x20 en .rdata (constantes, nunca parcheadas ni escritas) cuyo byte +0x10 (IsLoaded) vale 1 / 0
    for(uintptr_t a=0x858000;a<0x870000&&(!g.exeLoaded||!g.exeNot);a+=0x20) {
        const uint8 b=*reinterpret_cast<const uint8*>(a+0x10);
        if(b==1&&!g.exeLoaded)g.exeLoaded=a;
        if(b==0&&!g.exeNot)g.exeNot=a;
    }
    ready&=g.exeLoaded&&g.exeNot;
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI). Todos tienen hook RH.
    env.stubs[0]={{Pair{0x4D3A30,DllBody(&CAnimManager::GetAnimBlockName),true},reinterpret_cast<void*>(&FakeBlockName)},
                  {Pair{0x4D3940,DllBody(static_cast<CAnimBlock*(*)(const char*)>(&CAnimManager::GetAnimationBlock)),true},reinterpret_cast<void*>(&FakeGetBlock)},
                  {Pair{0x4087E0,DllBody(&CStreaming::RequestModel),true},reinterpret_cast<void*>(&FakeRequest)},
                  {Pair{0x4D3FB0,DllBody(&CAnimManager::AddAnimBlockRef),true},reinterpret_cast<void*>(&FakeAddRef)},
                  {Pair{0x609560,DllBody(&CPlayerPed::GetPadFromPlayer),true},reinterpret_cast<void*>(&FakeGetPad)},
                  {Pair{0x53FD30,DllBody(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkUpDown)),true},reinterpret_cast<void*>(&FakeUpDown)},
                  {Pair{0x53FC90,DllBody(static_cast<int16(CPad::*)() const>(&CPad::GetPedWalkLeftRight)),true},reinterpret_cast<void*>(&FakeLeftRight)},
                  {Pair{0x53CBE0,DllBody(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetRadianAngleBetweenPoints)),true},reinterpret_cast<void*>(&FakeRadian)},
                  {Pair{0x540770,DllBody(&CPad::JumpJustDown),true},reinterpret_cast<void*>(&FakeJump)},
                  {Pair{0x5407F0,DllBody(&CPad::SprintJustDown),true},reinterpret_cast<void*>(&FakeSprint)}};
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
