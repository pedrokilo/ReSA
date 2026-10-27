#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Ped/Ped.h"
#include "Entity/Vehicle/Vehicle.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCar.h"
#include "Tasks/TaskTypes/TaskComplexLeaveCarAndDie.h"
#include "Tasks/TaskTypes/TaskUtilityLineUpPedWithCar.h"
#include "CutsceneMgr.h"
#include "World.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par LeaveCarCtor71 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Tres funciones con hook S: constructor de CTaskComplexLeaveCar (0x63B8C0), su destructor (0x63B970) y el constructor de CTaskComplexLeaveCarAndDie (0x62F1A0).
// Los constructores se ejecutan sobre arenas con relleno controlado y se compara el objeto completo byte a byte (relleno incluido; la palabra de la vtable se sustituye por GetTaskType).
// Colaboradores sustituidos por falsos identicos en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL): CEntity::RegisterReference (0x571B70), CleanUpOldReference (0x571A00),
// CVehicle::ClearGettingOutFlags (0x6D3060) y operator delete (0x8214BD). El constructor de CTaskComplex (0x61A3B0), ~CTaskComplex (0x61A3D0) y ~CTask (0x61A3A0) corren reales; en el destructor la
// subtarea tiene vtable falsa y su destructor con borrado (ranura 0) registra la llamada.

namespace Lc71Layout {
struct LcTask : CTaskComplex { static constexpr size_t OffSub() { return offsetof(LcTask, m_pSubTask); } };
static_assert(LcTask::OffSub() == 8, "CTaskComplex::m_pSubTask en +8");
static_assert(offsetof(CTaskComplexLeaveCar, m_pTargetVehicle) == 0xC && offsetof(CTaskComplexLeaveCar, m_nTargetDoor) == 0x10 && offsetof(CTaskComplexLeaveCar, m_nDelayTime) == 0x14
    && offsetof(CTaskComplexLeaveCar, m_bSensibleLeaveCar) == 0x18 && offsetof(CTaskComplexLeaveCar, m_bForceGetOut) == 0x19 && offsetof(CTaskComplexLeaveCar, m_bDie) == 0x1A
    && offsetof(CTaskComplexLeaveCar, m_pTaskUtilityLineUpPedWithCar) == 0x1C && offsetof(CTaskComplexLeaveCar, m_nDoorFlagsSet) == 0x20 && offsetof(CTaskComplexLeaveCar, m_nNumGettingInSet) == 0x21
    && offsetof(CTaskComplexLeaveCar, m_nDieAnimID) == 0x24 && offsetof(CTaskComplexLeaveCar, m_fDieAnimBlendDelta) == 0x28 && offsetof(CTaskComplexLeaveCar, m_fDieAnimSpeed) == 0x2C
    && offsetof(CTaskComplexLeaveCar, m_bIsInAir) == 0x30 && sizeof(CTaskComplexLeaveCar) == 0x34, "CTaskComplexLeaveCar");
static_assert(offsetof(CVehicle, m_nNumGettingIn) == 0x485 && sizeof(CVehicle) == 0x5A0 && Lc71Fix::kVehWin >= 0x680, "CVehicle");
static_assert(ANIM_ID_KO_SHOT_FRONT_0 == 0xF, "m_nDieAnimID inicial");
}

namespace Lc71Live {
using namespace Lc71Fix;

// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    u32& W(size_t off,size_t objOff=kGuard) {return *reinterpret_cast<u32*>(m.data()+objOff+off);}
    void Put8(size_t off,u32 v,size_t objOff=kGuard) {m[objOff+off]=uint8(v);}
    json Changes(size_t objOff=kGuard) const {return ArenaChanges(m.data(),r.data(),N,objOff);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kTaskArena> taskA;
inline Arena<kSubArena> subA;
inline Arena<kVehArena> vehA;
inline Arena<kUtilArena> utilA;

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

// ================================================================================================ falsos
// Todos cdecl salvo los miembros (thiscall: this en ECX, `ret n`).
inline void __fastcall FakeRegRef(void* self,void*,u32 ref) {Ev& e=PushEv(20,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));}            // CEntity::RegisterReference 0x571B70, `ret 4`
inline void __fastcall FakeCleanRef(void* self,void*,u32 ref) {Ev& e=PushEv(22,2);e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));}          // CEntity::CleanUpOldReference 0x571A00, `ret 4`
inline void __fastcall FakeClearOut(void* self,void*,u32 flags) {Ev& e=PushEv(7,2);e.v[0]=Tag(self);e.v[1]=flags;}                         // CVehicle::ClearGettingOutFlags 0x6D3060, `ret 4` (dword completo, sin enmascarar)
inline void __cdecl FakeDelete(void* p) {Ev& e=PushEv(23,1);e.v[0]=Tag(uintptr_t(p));}                                                      // operator delete 0x8214BD
inline u32 __fastcall FakeSubDtor(void* self,void*,u32 flag) {Ev& e=PushEv(21,2);e.v[0]=Tag(self);e.v[1]=flag;return u32(uintptr_t(self));}   // destructor con borrado de la subtarea (vtable ranura 0), `ret 4`
inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(99,1);e.v[0]=Tag(self);return 0;}
inline std::array<void*,11> fakeSubVt{};

// Tipo de la tarea (GetTaskType, ranura 4 de la vtable del objeto) si la palabra de la vtable cambio; 0xFFFFFFFF si no se escribio
inline u32 TypeOf(uintptr_t obj,u32 vptr) {
    const uintptr_t fn=reinterpret_cast<const uintptr_t*>(uintptr_t(vptr))[4];
    return reinterpret_cast<u32(__fastcall*)(void*,void*)>(fn)(reinterpret_cast<void*>(obj),nullptr);
}

// ================================================================================================ constructores (0x63B8C0 y 0x62F1A0)
inline void CaseCtor(Env& env,Totals& t,unsigned fn,unsigned n) {
    const C c=MakeC(fn,n);
    const json input=json::parse(JsonC(c));
    const uintptr_t entry=fn==0?0x63B8C0:0x62F1A0;
    const size_t objOff=kGuard+c.shift;
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillMode(taskA.m.data(),taskA.m.size(),c.fillMode,c.fillSeed);
        vehA.Fill(c.seed*13u+4u);utilA.Fill(c.seed*13u+5u);subA.Fill(c.seed*13u+6u);
        taskA.Snap();vehA.Snap();utilA.Snap();subA.Snap();
        TagClear();taskA.Tagged();vehA.Tagged();utilA.Tagged();subA.Tagged();
        nev=0;
        const u32 veh=c.vehKind==0?0u:c.vehKind==1?u32(vehA.Obj()):c.vehRaw;
        const u32 args5[5]{veh,c.door,c.delay,c.sensible,c.force};
        const u32 args3[3]{veh,c.door,c.delay};
        const uintptr_t obj=taskA.Obj(objOff);
        sides[s]=Execute(env,fn,entry,candidate,[&](uintptr_t e){return Gate(e,fn==0?args5:args3,fn==0?5:3,c.eax,u32(obj),c.edx,c.stack,0);});
        // el objeto completo byte a byte, salvo la palabra de la vtable (EXE: vtable del EXE; DLL: vtable de la DLL), que se sustituye por el tipo de la tarea
        json changes=taskA.Changes(objOff);
        json noVptr=json::array();
        for(const auto& ch:changes)if(!(ch[0].get<int64>()>=0&&ch[0].get<int64>()<4))noVptr.push_back(ch);
        const u32 vptr=*reinterpret_cast<const u32*>(obj);
        const u32 refVptr=*reinterpret_cast<const u32*>(taskA.r.data()+objOff);
        const bool vptrWritten=vptr!=refVptr;
        json obs;
        obs["eax_is_this"]=sides[s].reg.eax==u32(obj);
        obs["changes"]=json::array({noVptr,vehA.Changes(),utilA.Changes(),subA.Changes()});
        obs["vptr_written"]=vptrWritten;
        obs["type"]=vptrWritten?TypeOf(obj,vptr):0xFFFFFFFFu;
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[fn],n,input,rows,sides);
}

// ================================================================================================ destructor (0x63B970)
inline void CaseDtor(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(1,n);
    const json input=json::parse(JsonC(c));
    const size_t objOff=kGuard+c.shift;
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillMode(taskA.m.data(),taskA.m.size(),c.fillMode,c.fillSeed);
        if(c.vehFill==0xFFFFFFFFu)std::memset(vehA.m.data(),0xFF,vehA.m.size());else vehA.Fill(c.vehFill);
        utilA.Fill(c.seed*13u+5u);subA.Fill(c.seed*13u+6u);
        subA.W(0,kGuard)=u32(reinterpret_cast<uintptr_t>(fakeSubVt.data()));
        // objeto ya construido: vtable, subtarea, vehiculo, utilidad y los dos contadores de reserva del caso; el resto es relleno
        taskA.W(0,objOff)=c.vptrKind==0?0x86E828u:taskA.W(0,objOff);
        taskA.W(8,objOff)=c.subKind==1?u32(subA.Obj()):0u;
        taskA.W(0xC,objOff)=c.vehKind==1?u32(vehA.Obj()):0u;
        taskA.W(0x1C,objOff)=c.utilKind==0?0u:c.utilKind==1?u32(utilA.Obj()):c.utilRaw;
        taskA.Put8(0x20,c.flagsSet,objOff);taskA.Put8(0x21,c.numIn,objOff);
        taskA.Snap();vehA.Snap();utilA.Snap();subA.Snap();
        TagClear();taskA.Tagged();vehA.Tagged();utilA.Tagged();subA.Tagged();
        nev=0;
        const uintptr_t obj=taskA.Obj(objOff);
        sides[s]=Execute(env,1,0x63B970,candidate,[&](uintptr_t e){return Gate(e,nullptr,0,c.eax,u32(obj),c.edx,c.stack,0);});
        json changes=taskA.Changes(objOff);
        json noVptr=json::array();
        for(const auto& ch:changes)if(!(ch[0].get<int64>()>=0&&ch[0].get<int64>()<4))noVptr.push_back(ch);
        json obs;
        obs["changes"]=json::array({noVptr,vehA.Changes(),utilA.Changes(),subA.Changes()});
        obs["vptr_changed"]=taskA.W(0,objOff)!=*reinterpret_cast<const u32*>(taskA.r.data()+objOff);
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
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
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"vtable_slot",0},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c) {
                if(f==1)CaseDtor(env,totals,c);else CaseCtor(env,totals,f,c);
            }
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

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de una funcion con hook S (Simple) o V (virtual): el destino vivo del hook
inline uintptr_t DllBodyOf(const char* cls,const char* fn) {
    auto* h=FindHook(cls,fn);if(!h)return 0;uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a?Body(a):0;
}

inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"leavecarctor71-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"leavecarctor71-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("leavecarctor71");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    env.vtSlot=0;
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de crear las rutas y de escribir ningun parche (con su hook encendido, la entrada del EXE salta a la DLL; Body() sigue el salto)
    const uintptr_t regRefBody=DllBodyOf("CEntity","RegisterReference-");
    const uintptr_t cleanBody=DllBodyOf("CEntity","CleanUpOldReference-");
    const uintptr_t clearBody=DllBodyOf("CVehicle","ClearGettingOutFlags");
    ready&=regRefBody&&cleanBody&&clearBody;
    struct RouteSpec {const char* cls;const char* fn;uintptr_t entry;};
    const RouteSpec specs[kFunctions]{{"CTaskComplexLeaveCar","Constructor",0x63B8C0},{"CTaskComplexLeaveCar","Destructor",0x63B970},{"CTaskComplexLeaveCarAndDie","Constructor",0x62F1A0}};
    for(unsigned f=0;f<kFunctions;++f) {
        env.route[f].reset(new HookRoute(specs[f].cls,specs[f].fn,specs[f].entry,HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    env.stubs[0]={{Pair{0x571B70,regRefBody,true},reinterpret_cast<void*>(&FakeRegRef)}};
    env.stubs[2]={{Pair{0x571B70,regRefBody,true},reinterpret_cast<void*>(&FakeRegRef)}};
    env.stubs[1]={{Pair{0x6D3060,clearBody,true},reinterpret_cast<void*>(&FakeClearOut)},
                  {Pair{0x571A00,cleanBody,true},reinterpret_cast<void*>(&FakeCleanRef)},
                  {Pair{0x8214BD,0,false},reinterpret_cast<void*>(&FakeDelete)}};   // el ultimo: ningun otro parche puede liberar memoria con operator delete ya sustituido
    for(auto& v:fakeSubVt)v=reinterpret_cast<void*>(&FakeTrap);
    fakeSubVt[0]=reinterpret_cast<void*>(&FakeSubDtor);
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CTaskComplex (0x61A3B0), ~CTaskComplex (0x61A3D0) y ~CTask (0x61A3A0): reales (en el EXE; hook o inline en la DLL)"})},{"stubbed",stubbed}};
        env.watches.emplace_back(specs[f].entry,16);
    }
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
