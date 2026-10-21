#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "PathFind.h"
#include "ColStore.h"
#include "DamageManager.h"
#include "Automobile.h"
#include "Bike.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63Layout {
struct VehProbe : CVehicle {
    static constexpr size_t OffModel() {return offsetof(CVehicle, m_nModelIndex);}
    static constexpr size_t OffAuto() {return offsetof(CVehicle, m_autoPilot);}
    static constexpr size_t OffCreated() {return offsetof(CVehicle, m_nCreatedBy);}
};
static_assert(VehProbe::OffModel()==0x22 && VehProbe::OffAuto()==0x390 && VehProbe::OffCreated()==0x4A4, "CVehicle");
static_assert(offsetof(CAutoPilot, m_nMovementFlags)==0x4C && offsetof(CAutoPilot, m_nCurrentPathNodeInfo)==0x14 && offsetof(CAutoPilot, m_nNextPathNodeInfo)==0x16, "CAutoPilot");
static_assert(sizeof(CPathNode)==Cc63Fix::kNodeBytes && sizeof(CCarPathLink)==Cc63Fix::kLinkBytes, "tamanos de nodo / enlace");
}

namespace Cc63Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool tables=uintptr_t(&ThePaths.m_pPathNodes)==0x96F854&&uintptr_t(&ThePaths.m_pNaviNodes)==0x96F974;
    MEMORY_BASIC_INFORMATION mi{};
    const bool readable=VirtualQuery(reinterpret_cast<void*>(0x96F854),&mi,sizeof(mi))==sizeof(mi)&&uintptr_t(mi.BaseAddress)+mi.RegionSize>=0x96F854+0x1000*4;
    if(!tables||!readable)Write({{"event","layout_error"},{"reason","tablas de ThePaths distintas de las del EXE"},{"tables",tables},{"readable",readable}});
    return tables&&readable;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"ThisVehicleShouldTryNotToTurn",0x421FE0);
    simple(1,"FindIntersection2Lines",0x4226F0);
    simple(2,"FindSpeedMultiplier",0x4224E0);
    simple(3,"FindPercDependingOnDistToLink",0x422620);
    simple(4,"FindPathDirection",0x422090);
    simple(5,"StopCarIfNodesAreInvalid",0x422590);
    simple(6,"SwitchBetweenPhysicsAndGhost",0x4222A0);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    env.stubs[4]={{Pair{0x420A10,DllBody(&CPathNode::GetPosition),true},reinterpret_cast<void*>(&FakeGetPos)}};
    env.stubs[6]={{Pair{0x410CE0,DllBody(&CColStore::HasCollisionLoaded),true},reinterpret_cast<void*>(&FakeColLoaded)},
                  {Pair{0x6C21A0,DllBody(&CDamageManager::SetWheelStatus),true},reinterpret_cast<void*>(&FakeSetWheel)},
                  {Pair{0x6AF420,DllBody(&CAutomobile::PlaceOnRoadProperly),true},reinterpret_cast<void*>(&FakePlaceAuto)},
                  {Pair{0x6BEEB0,DllBody(&CBike::PlaceOnRoadProperly),true},reinterpret_cast<void*>(&FakePlaceBike)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    ready&=LayoutOk();
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
