#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "World.h"
#include "PathFind.h"
#include "AutoPilot.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarCtrlWeave63C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta de CCarCtrl (UpdateCarOnRails); los colaboradores con efectos se sustituyen por falsos identicos en ambos lados.

namespace CwCLayout {
using namespace CwCFix;
static_assert(offsetof(CPhysical, m_vecMoveSpeed) == kOffSpeed, "CPhysical::m_vecMoveSpeed");
static_assert(offsetof(CVehicle, m_autoPilot) == 0x390 && offsetof(CVehicle, m_nVehicleSubType) == kOffSub, "CVehicle");
static_assert(0x390 + offsetof(CAutoPilot, field_C) == kOffFieldC && 0x390 + offsetof(CAutoPilot, m_nSpeedScaleFactor) == kOffScale && 0x390 + offsetof(CAutoPilot, m_nCurrentPathNodeInfo) == kOffCur
              && 0x390 + offsetof(CAutoPilot, m_nNextPathNodeInfo) == kOffNext && 0x390 + offsetof(CAutoPilot, _smthCurr) == kOffLaneCurr && 0x390 + offsetof(CAutoPilot, _smthNext) == kOffLaneNext
              && 0x390 + offsetof(CAutoPilot, m_nCurrentLane) == kOffLaneCur && 0x390 + offsetof(CAutoPilot, m_nNextLane) == kOffLaneNx && 0x390 + offsetof(CAutoPilot, m_nTempAction) == kOffTempAct
              && 0x390 + offsetof(CAutoPilot, m_nTempActionTime) == kOffTempTime && 0x390 + offsetof(CAutoPilot, m_nMovementFlags) == kOffMove && 0x390 + offsetof(CAutoPilot, m_nTimeToStartMission) == kOffStart
              && 0x390 + offsetof(CAutoPilot, m_nTimeSwitchedToRealPhysics) == kOffSwitched, "CAutoPilot");
static_assert(sizeof(CCarPathLink) == kLinkBytes && offsetof(CCarPathLink, m_posn) == 0, "CCarPathLink");
}

namespace CwCLive {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlweave63c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlweave63c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlweave63c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CCarCtrl",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    struct C {const char* cls;const char* fn;uintptr_t exe;void* fake;uintptr_t body;};
    auto mk=[&](const C& c){
        Pair p{c.exe,c.body,false};
        if(c.cls) {
            env.collab.emplace_back(new HookRoute(c.cls,c.fn,c.exe,HookRoute::RegisteredTarget{}));
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
            else if(!env.collab.back()->Registered()&&!c.body)p.dll=0;
        }
        return StubSpec{p,c.fake};
    };
    const StubSpec s422590=mk({"CCarCtrl","StopCarIfNodesAreInvalid",0x422590,reinterpret_cast<void*>(&Fk422590),0});
    const StubSpec s432B10=mk({"CCarCtrl","PickNextNodeAccordingStrategy",0x432B10,reinterpret_cast<void*>(&Fk432B10),0});
    const StubSpec slow=mk({"CCarCtrl","SlowCarOnRailsDownForTrafficAndLights",0x434790,reinterpret_cast<void*>(&FkSlowRails),0});
    const StubSpec modify=mk({"CAutoPilot","ModifySpeed",0x41B980,reinterpret_cast<void*>(&FkModify),0});
    const StubSpec curve=mk({"CCurves","CalcCurvePoint",0x43C900,reinterpret_cast<void*>(&FkCurve),0});
    const StubSpec s42EC90=mk({nullptr,nullptr,0x42EC90,reinterpret_cast<void*>(&Fk42EC90),0});
    env.stubs[0]={s422590,s432B10,slow,modify,curve,s42EC90};
    for(const StubSpec* s:{&s422590,&s432B10,&slow,&modify,&curve,&s42EC90})env.watches.emplace_back(s->pair.exe,16);
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f])stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    naviTable=uintptr_t(&ThePaths.m_pNaviNodes[0]);
    ready&=naviTable==kNaviAddr;
    {
        const u32* table=reinterpret_cast<const u32*>(naviTable);
        u32 found=0xFFFFFFFFu;
        for(u32 i=0;i+1<64&&found==0xFFFFFFFFu;++i)if(table[i]==0&&table[i+1]==0)found=i;
        ready&=found!=0xFFFFFFFFu;areaBase=found==0xFFFFFFFFu?0:found;
    }
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
