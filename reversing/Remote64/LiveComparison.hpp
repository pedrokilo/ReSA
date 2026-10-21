#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Remote.h"
#include "World.h"
#include "PlayerInfo.h"
#include "Camera.h"
#include "MissionCleanup.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Remote64 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: CRemote::GivePlayerRemoteControlledCar (0x45AB10) y CRemote::TakeRemoteControlledCarFromPlayer (0x45AE80). Todos los colaboradores se sustituyen por falsos identicos en
// ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL cuando lo hay).
namespace R64Layout {
static_assert(sizeof(CPlayerInfo) == 0x190 && offsetof(CPlayerInfo, m_pRemoteVehicle) == 0xB0 && offsetof(CPlayerInfo, m_bAfterRemoteVehicleExplosion) == 0xDD
              && offsetof(CPlayerInfo, m_bCreateRemoteVehicleExplosion) == 0xDE && offsetof(CPlayerInfo, m_bFadeAfterRemoteVehicleExplosion) == 0xDF
              && offsetof(CPlayerInfo, m_nTimeOfRemoteVehicleExplosion) == 0xE0, "CPlayerInfo");
static_assert(offsetof(CPlaceable, m_matrix) == 0x14 && offsetof(CPlaceable, m_placement) == 0x04, "CPlaceable");
static_assert(offsetof(CVehicle, m_nCreatedBy) == 0x4A4, "CVehicle::m_nCreatedBy");
}

namespace R64Live {
// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}
inline bool LayoutMatches() {
    return uintptr_t(&CWorld::Players[0]) == kPlayers && uintptr_t(&CWorld::PlayerInFocus) == kFocusAddr && uintptr_t(&CTheScripts::MissionCleanUp) == kMissionCleanup
        && uintptr_t(&TheCamera) == kCamera && uintptr_t(&CTimer::m_snTimeInMilliseconds) == kTimerAddr;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"remote64-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"remote64-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("remote64");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    {   // depuracion: el archivo puede contener "primero ultimo" para correr solo ese rango de casos de cada funcion
        std::ifstream start(dir/"remote64-cases.txt");unsigned a{},b{};
        if(start>>a>>b){dbgFirst=a;dbgLast=b;dbgMode=true;}
    }
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // El cuerpo de la DLL de cada ruta se calcula (HookRoute) antes de crear ningun parche.
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CRemote",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
    json hooksFound = json::array();
    auto mk=[&](uintptr_t exeAddr,void* fake,uintptr_t forcedBody=0){
        Pair p{exeAddr,forcedBody,false};
        std::string cat,name;
        if(FindHook(exeAddr,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exeAddr,HookRoute::RegisteredTarget{}));
            hooksFound.push_back({{"exe",exeAddr},{"class",cat},{"name",name}});
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
        }
        return StubSpec{p,fake};
    };
    #define FK(f) reinterpret_cast<void*>(&f)
    const std::vector<StubSpec> give{
        mk(0x4C5B00,FK(FkIsHeli)),mk(0x4C5B30,FK(FkIsPlane)),mk(0x6E2D50,FK(FkNew)),mk(0x6C4190,FK(FkCtorHeli)),mk(0x6C8E20,FK(FkCtorPlane)),mk(0x6B0A90,FK(FkCtorAuto)),
        mk(0x5696C0,FK(FkGround)),mk(0x536BE0,FK(FkDist)),mk(0x59B020,FK(FkRotZ),Body(std::bit_cast<uintptr_t>(&CMatrix::SetRotateZOnly))),mk(0x42F5A0,FK(FkJoin)),mk(0x563220,FK(FkAdd)),mk(0x56E0D0,FK(FkFpv)),
        mk(0x571B70,FK(FkRegRef)),mk(0x50C7C0,FK(FkTakeCtl)),mk(0x50C1B0,FK(FkZoom))};
    const std::vector<StubSpec> take{
        mk(0x6D5D70,FK(FkSetCreated)),
        mk(0x4654B0,FK(FkRemove),Body(std::bit_cast<uintptr_t>(static_cast<void (CMissionCleanup::*)(int32,MissionCleanUpEntityType)>(&CMissionCleanup::RemoveEntityFromList))))};
    #undef FK
    env.stubs[0]=give;env.stubs[1]=take;
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound}};
    }
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
        const bool layout=LayoutMatches();
        ready&=layout;
        if(!layout)Write({{"event","layout_error"},{"reason","direcciones del estado real distintas de Fixtures.hpp"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
