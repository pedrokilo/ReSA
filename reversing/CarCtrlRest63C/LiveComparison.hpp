#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "PathFind.h"
#include "Automobile.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63cLayout {
struct VehProbe : CVehicle {
    static constexpr size_t OffModel() {return offsetof(CVehicle, m_nModelIndex);}
    static constexpr size_t OffAuto() {return offsetof(CVehicle, m_autoPilot);}
    static constexpr size_t OffMatrix() {return offsetof(CVehicle, m_matrix);}
};
static_assert(VehProbe::OffModel()==0x22 && VehProbe::OffAuto()==0x390 && VehProbe::OffMatrix()==0x14, "CVehicle");
static_assert(offsetof(CAutoPilot, m_startingRouteNode)==4 && offsetof(CAutoPilot, m_endingRouteNode)==8 && offsetof(CAutoPilot, m_nCurrentLane)==0x27 && offsetof(CAutoPilot, m_nNextLane)==0x28
              && offsetof(CAutoPilot, m_nCarMission)==0x2A && offsetof(CAutoPilot, field_41)==0x41 && offsetof(CAutoPilot, m_nCarCtrlFlags)==0x4B && offsetof(CAutoPilot, m_vecDestinationCoors)==0x5C
              && offsetof(CAutoPilot, m_aPathFindNodesInfo)==0x68 && offsetof(CAutoPilot, m_nPathFindNodesCount)==0x88 && offsetof(CAutoPilot, m_TargetEntity)==0x8C, "CAutoPilot");
static_assert(sizeof(CPathNode)==Cc63cFix::kNodeBytes && offsetof(CPathNode, m_wBaseLinkId)==0x10, "CPathNode");
}

namespace Cc63cLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool paths=uintptr_t(&ThePaths)==0x96F050&&uintptr_t(&ThePaths.m_pPathNodes)==0x96F854&&uintptr_t(&ThePaths.m_pNodeLinks)==0x96FA94;
    if(!paths)Write({{"event","layout_error"},{"reason","tablas de ThePaths distintas de las del EXE"}});
    return paths;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"PickNextNodeAccordingStrategy",0x432B10);
    simple(1,"JoinCarWithRoadSystem",0x42F5A0);
    simple(2,"JoinCarWithRoadSystemGotoCoors",0x42F870);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // JoinCarWithRoadSystem es a la vez funcion probada (banco: su hook se apaga fuera de su propia llamada): como colaborador de GotoCoors va con hooked=false y el cuerpo de la DLL parcheado.
    // dll=0 y hooked=false: la DLL llama a la direccion del EXE (envolturas plugin::Call de PickNextNode*, RemoveBadStartNode; FindLinksToGoWithTheseNodes ya es C++ con hook (hooked=true + DllBody)).
    env.stubs[0]={{Pair{0x56E010,DllBody(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),true},reinterpret_cast<void*>(&FakeFindCoors)},
                  {Pair{0x56E0D0,DllBody(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),true},reinterpret_cast<void*>(&FakeFindPlayerVeh)},
                  {Pair{0x426EF0,0,false},reinterpret_cast<void*>(&FakeChase)},
                  {Pair{0x427740,0,false},reinterpret_cast<void*>(&FakeFollow)},
                  {Pair{0x42DE80,0,false},reinterpret_cast<void*>(&FakeRandomly)}};
    env.stubs[1]={{Pair{0x44FCE0,DllBody(&CPathFind::FindNodeClosestToCoorsFavourDirection),true},reinterpret_cast<void*>(&FakeFavour)},
                  {Pair{0x42B470,DllBody(&CCarCtrl::FindLinksToGoWithTheseNodes),true},reinterpret_cast<void*>(&FakeFindLinks<51>)}};
    env.stubs[2]={{Pair{0x4515D0,DllBody(&CPathFind::DoPathSearch),true},reinterpret_cast<void*>(&FakeDoPathSearch)},
                  {Pair{0x44E4F0,0,false},reinterpret_cast<void*>(&FakeRemoveBad)},
                  {Pair{0x41B950,DllBody(&CAutoPilot::RemoveOnePathNode),true},reinterpret_cast<void*>(&FakeRemoveOne)},
                  {Pair{0x42B470,DllBody(&CCarCtrl::FindLinksToGoWithTheseNodes),true},reinterpret_cast<void*>(&FakeFindLinks<73>)},
                  {Pair{0x42F5A0,DllBody(&CCarCtrl::JoinCarWithRoadSystem),false},reinterpret_cast<void*>(&FakeJoin)}};
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
