#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "PathFind.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63F dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63fLayout {
struct VehProbe : CVehicle {
    static constexpr size_t OffModel() {return offsetof(CVehicle, m_nModelIndex);}
    static constexpr size_t OffAuto() {return offsetof(CVehicle, m_autoPilot);}
    static constexpr size_t OffMatrix() {return offsetof(CVehicle, m_matrix);}
    static constexpr size_t OffDriver() {return offsetof(CVehicle, m_pDriver);}
    static constexpr size_t OffCreated() {return offsetof(CVehicle, m_nCreatedBy);}
};
static_assert(VehProbe::OffModel()==0x22 && VehProbe::OffAuto()==0x390 && VehProbe::OffMatrix()==0x14 && VehProbe::OffDriver()==0x460 && VehProbe::OffCreated()==0x4A4, "CVehicle");
static_assert(offsetof(CAutoPilot, m_nCarMission)==0x2A, "CAutoPilot");
static_assert(sizeof(CPathNode)==Cc63fFix::kNodeBytes, "CPathNode");
}

namespace Cc63fLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool ok=uintptr_t(&ThePaths.m_pPathNodes)==0x96F854&&uintptr_t(&CModelInfo::ms_modelInfoPtrs)==0xA9B0C8;
    if(!ok)Write({{"event","layout_error"},{"reason","tablas globales distintas de las del EXE"}});
    return ok;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63f-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63f-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63f");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"IsThisAnAppropriateNode",0x42DAB0);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche. Los ayudantes de vector (0x40FE60, 0x59C730, 0x40FDB0, 0x420A10) y
    // CPlaceable::GetRight (0x41CC70, por direccion) NO se sustituyen: el EXE usa los suyos y la DLL sus equivalentes en x87.
    env.stubs[0]={{Pair{0x6D2370,DllBody(&CVehicle::IsLawEnforcementVehicle),true},reinterpret_cast<void*>(&FakeLaw)},
                  {Pair{0x42C250,DllBody(&CCarCtrl::IsAnyoneParking),true},reinterpret_cast<void*>(&FakeParking)},
                  {Pair{0x568B80,DllBody(&CWorld::FindObjectsKindaColliding),true},reinterpret_cast<void*>(&FakeFindObjects)}};
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
