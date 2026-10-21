#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "Automobile.h"
#include "Bike.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63B dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63bLayout {
struct VehProbe : CVehicle {
    static constexpr size_t OffModel() {return offsetof(CVehicle, m_nModelIndex);}
    static constexpr size_t OffAuto() {return offsetof(CVehicle, m_autoPilot);}
    static constexpr size_t OffDriver() {return offsetof(CVehicle, m_pDriver);}
    static constexpr size_t OffMaxPass() {return offsetof(CVehicle, m_nMaxPassengers);}
    static constexpr size_t OffType() {return offsetof(CVehicle, m_nVehicleType);}
};
static_assert(VehProbe::OffModel()==0x22 && VehProbe::OffAuto()==0x390 && VehProbe::OffDriver()==0x460 && VehProbe::OffMaxPass()==0x488 && VehProbe::OffType()==0x590, "CVehicle");
static_assert(offsetof(CAutoPilot, m_nCarDrivingStyle)==0x29 && offsetof(CAutoPilot, m_nTempAction)==0x2B && offsetof(CAutoPilot, m_nCruiseSpeed)==0x40
              && offsetof(CAutoPilot, m_SpeedMult)==0x44 && offsetof(CAutoPilot, m_nCarCtrlFlags)==0x4B, "CAutoPilot");
static_assert(sizeof(CRepeatSector)==12 && sizeof(CVehicle::m_nVehicleType)==4, "CRepeatSector / eVehicleType");
}

namespace Cc63bLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool repeat=uintptr_t(&CWorld::ms_aRepeatSectors)==0xB992B8&&uintptr_t(&CWorld::ms_nCurrentScanCode)==0xB7CD78;
    const bool models=uintptr_t(&CModelInfo::ms_modelInfoPtrs)==0xA9B0C8;
    const bool stuck=uintptr_t(&CTheScripts::StuckCars)==0xA90AB0;
    if(!repeat||!models||!stuck)Write({{"event","layout_error"},{"reason","tablas globales distintas de las del EXE"},{"repeat",repeat},{"models",models},{"stuck",stuck}});
    return repeat&&models&&stuck;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"SetUpDriverAndPassengersForVehicle",0x4217C0);
    simple(1,"SetCoordsOfScriptCar",0x4342A0);
    simple(2,"ScanForPedDanger",0x42CE40);
    simple(3,"FindMaximumSpeedForThisCarInTraffic",0x434400);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // hooked=false: sin hook (rand del CRT). Los SlowCarDownFor...SectorList ya son C++ con hook (hooked=true + DllBody).
    env.stubs[0]={{Pair{0x6D1A50,DllBody(&CVehicle::SetUpDriver),true},reinterpret_cast<void*>(&FakeSetUpDriver)},
                  {Pair{0x6D1AA0,DllBody(&CVehicle::SetupPassenger),true},reinterpret_cast<void*>(&FakeSetupPassenger)},
                  {Pair{0x5E4390,DllBody(&CPed::GiveObjectToPedToHold),true},reinterpret_cast<void*>(&FakeGive)},
                  {Pair{0x5E01A0,DllBody(&CPed::UpdateStatEnteringVehicle),true},reinterpret_cast<void*>(&FakeUpdateStat)},
                  {Pair{0x4C5AA0,DllBody(&CModelInfo::IsCarModel),true},reinterpret_cast<void*>(&FakeIsCar)},
                  {Pair{0x4D3990,DllBody(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)),true},reinterpret_cast<void*>(&FakeAnimBlock)},
                  {Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)}};
    env.stubs[1]={{Pair{0x569660,DllBody(&CWorld::FindGroundZForCoord),true},reinterpret_cast<void*>(&FakeGround)},
                  {Pair{0x536BE0,DllBody(&CEntity::GetDistanceFromCentreOfMassToBaseOfModel),true},reinterpret_cast<void*>(&FakeDist)},
                  {Pair{0x463C40,DllBody(&CStuckCarCheck::ClearStuckFlagForCar),true},reinterpret_cast<void*>(&FakeStuck)},
                  {Pair{0x486B00,DllBody(&CTheScripts::ClearSpaceForMissionEntity),true},reinterpret_cast<void*>(&FakeClearSpace)},
                  {Pair{0x6AF420,DllBody(&CAutomobile::PlaceOnRoadProperly),true},reinterpret_cast<void*>(&FakePlaceAuto)},
                  {Pair{0x6BEEB0,DllBody(&CBike::PlaceOnRoadProperly),true},reinterpret_cast<void*>(&FakePlaceBike)},
                  {Pair{0x432CB0,DllBody(&CCarCtrl::JoinCarWithRoadAccordingToMission),true},reinterpret_cast<void*>(&FakeJoin)}};
    env.stubs[2]={{Pair{0x56E0D0,DllBody(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),true},reinterpret_cast<void*>(&FakeFindPlayer)},
                  {Pair{0x563470,DllBody(&CWorld::ClearScanCodes),true},reinterpret_cast<void*>(&FakeClearScan)},
                  {Pair{0x425440,DllBody(&CCarCtrl::SlowCarDownForPedsSectorList<CPtrListDoubleLink<CPed*>>),true},reinterpret_cast<void*>(&FakeList<1>)}};
    env.stubs[3]={{Pair{0x563470,DllBody(&CWorld::ClearScanCodes),true},reinterpret_cast<void*>(&FakeClearScan)},
                  {Pair{0x432420,DllBody(&CCarCtrl::SlowCarDownForCarsSectorList<CPtrListDoubleLink<CVehicle*>>),true},reinterpret_cast<void*>(&FakeList<0>)},
                  {Pair{0x425440,DllBody(&CCarCtrl::SlowCarDownForPedsSectorList<CPtrListDoubleLink<CPed*>>),true},reinterpret_cast<void*>(&FakeList<1>)},
                  {Pair{0x42D4F0,DllBody(&CCarCtrl::SlowCarDownForObjectsSectorList<CPtrListDoubleLink<CObject*>>),true},reinterpret_cast<void*>(&FakeList<2>)}};
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
