#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "CarCtrl.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par CarCtrlRest63E dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Cc63eLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline bool LayoutOk() {
    const bool ok=uintptr_t(&CPopCycle::m_NumOther_Cars)==0xC0BC30&&uintptr_t(&CPopCycle::m_NumDealers_Cars)==0xC0BC3C&&uintptr_t(&CPopCycle::m_pCurrZoneInfo)==0xC0BC68
                  &&uintptr_t(&CPopulation::m_bDontCreateRandomGangMembers)==0xC0FCB2&&uintptr_t(&CPopulation::m_bDontCreateRandomCops)==0xC0FCB4&&uintptr_t(&CPopulation::m_AppropriateLoadedCars)==0xC0E9F8
                  &&uintptr_t(&CPopulation::m_CarGroups)+18*46==0xC0F074&&sizeof(CZoneInfo)==0x11;
    if(!ok)Write({{"event","layout_error"},{"reason","globales de poblacion distintos de los del EXE"}});
    return ok;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"carctrlrest63e-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlrest63e-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlrest63e");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute("CCarCtrl",fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"ChooseModel",0x424CE0);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche. dll=0 y hooked=false: la DLL llama a la direccion del EXE (rand del CRT y 0x407180).
    env.stubs[0]={{Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)},
                  {Pair{0x443AC0,DllBody(&CGangWars::GangWarFightingGoingOn),true},reinterpret_cast<void*>(&FakeGangWar)},
                  {Pair{0x421980,DllBody(&CCarCtrl::ChoosePoliceCarModel),true},reinterpret_cast<void*>(&FakePolice)},
                  {Pair{0x611C50,DllBody(&CLoadedCarGroup::PickRandomCar),true},reinterpret_cast<void*>(&FakePick)},
                  {Pair{0x614490,DllBody(&CPopulation::PickGangCar),true},reinterpret_cast<void*>(&FakeGangCar)},
                  {Pair{0x407180,Body(0x407180),true},reinterpret_cast<void*>(&FakeRange)}};
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
