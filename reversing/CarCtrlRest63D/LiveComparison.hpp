#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "PathFind.h"
#include "Collision.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63D dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63dLayout {
struct VehProbe : CVehicle {
    static constexpr size_t OffModel() {return offsetof(CVehicle, m_nModelIndex);}
    static constexpr size_t OffAuto() {return offsetof(CVehicle, m_autoPilot);}
    static constexpr size_t OffMatrix() {return offsetof(CVehicle, m_matrix);}
    static constexpr size_t OffSeed() {return offsetof(CVehicle, m_nRandomSeed);}
};
static_assert(VehProbe::OffModel()==0x22 && VehProbe::OffAuto()==0x390 && VehProbe::OffMatrix()==0x14 && VehProbe::OffSeed()==0x20, "CVehicle");
static_assert(offsetof(CAutoPilot, m_currentAddress)==0 && offsetof(CAutoPilot, m_startingRouteNode)==4 && offsetof(CAutoPilot, m_endingRouteNode)==8 && offsetof(CAutoPilot, m_nCurrentPathNodeInfo)==0x14
              && offsetof(CAutoPilot, m_nNextPathNodeInfo)==0x16 && offsetof(CAutoPilot, _smthCurr)==0x25 && offsetof(CAutoPilot, _smthNext)==0x26 && offsetof(CAutoPilot, m_nCarMission)==0x2A
              && offsetof(CAutoPilot, m_ucCarMissionModeCounter)==0x35 && offsetof(CAutoPilot, m_nCarCtrlFlags)==0x4B && offsetof(CAutoPilot, m_vecDestinationCoors)==0x5C
              && offsetof(CAutoPilot, m_aPathFindNodesInfo)==0x68 && offsetof(CAutoPilot, m_nPathFindNodesCount)==0x88 && offsetof(CAutoPilot, m_TargetEntity)==0x8C, "CAutoPilot");
static_assert(sizeof(CPathNode)==Cc63dFix::kNodeBytes && offsetof(CPathNode, m_wBaseLinkId)==0x10, "CPathNode");
}

namespace Cc63dLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool paths=uintptr_t(&ThePaths)==0x96F050&&uintptr_t(&ThePaths.m_pPathNodes)==0x96F854&&uintptr_t(&ThePaths.m_pNodeLinks)==0x96FA94&&uintptr_t(&ThePaths.m_pNaviLinks)==0x96FDF4;
    const bool timer=uintptr_t(&CTimer::m_snPreviousTimeInMilliseconds)==0xB7CB78&&uintptr_t(&CTimer::m_snTimeInMilliseconds)==0xB7CB84;
    if(!paths||!timer)Write({{"event","layout_error"},{"reason","tablas globales distintas de las del EXE"},{"paths",paths},{"timer",timer}});
    return paths&&timer;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63d-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63d-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63d");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"ReconsiderRoute",0x42FC40);
    simple(1,"FindLinksToGoWithTheseNodes",0x42B470);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // FindLinksToGoWithTheseNodes es a la vez funcion probada (su hook se apaga fuera de su propia llamada): como colaborador de ReconsiderRoute va con hooked=false y el cuerpo de la DLL parcheado.
    // FindNodesThisCarIsNearestTo (lote 75) ya esta reconstruida en C++ puro y con hook propio (encendido, como el de FindPlayerCoors): la entrada del EXE salta a la DLL y el cuerpo de la DLL lleva el falso.
    // dll=0 y hooked=false: la DLL llama a la direccion del EXE (srand del CRT).
    env.stubs[0]={{Pair{0x42BD20,DllBody(&CCarCtrl::FindNodesThisCarIsNearestTo),true},reinterpret_cast<void*>(&FakeNearest)},
                  {Pair{0x56E010,DllBody(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),true},reinterpret_cast<void*>(&FakeCoors)},
                  {Pair{0x4515D0,DllBody(&CPathFind::DoPathSearch),true},reinterpret_cast<void*>(&FakeDoPathSearch)},
                  {Pair{0x42B470,DllBody(&CCarCtrl::FindLinksToGoWithTheseNodes),false},reinterpret_cast<void*>(&FakeFindLinks<83>)},
                  {Pair{0x41B950,DllBody(&CAutoPilot::RemoveOnePathNode),true},reinterpret_cast<void*>(&FakeRemoveOne)}};
    env.stubs[1]={{Pair{0x821B11,0,false},reinterpret_cast<void*>(&FakeSrand)},
                  {Pair{0x417610,DllBody(&CCollision::DistToLine),true},reinterpret_cast<void*>(&FakeDistToLine)}};
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
