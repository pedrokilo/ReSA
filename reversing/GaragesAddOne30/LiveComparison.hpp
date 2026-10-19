#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "Garages.h"
#include "Garage.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par GaragesAddOne30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CGarages::AddOne (0x4471E0), hook "AddOne" de la clase CGarages. No hay colaboradores sustituidos.
static_assert(sizeof(CGarage) == GaragesAddOne30Fix::kEntryBytes, "el banco trabaja con ranuras de 0xD8 bytes");
static_assert(CGarages::MAX_NUM_GARAGES == GaragesAddOne30Fix::kMaxGarages, "50 garajes");
static_assert(offsetof(CGarage, m_vPosn) == 0 && offsetof(CGarage, m_fTopZ) == 0x1C && offsetof(CGarage, m_fWidth) == 0x20 && offsetof(CGarage, m_fHeight) == 0x24 && offsetof(CGarage, m_fLeftCoord) == 0x28
              && offsetof(CGarage, m_fBackCoord) == 0x34 && offsetof(CGarage, m_anName) == 0x44 && offsetof(CGarage, m_nType) == 0x4C && offsetof(CGarage, m_nFlags) == 0x4E && offsetof(CGarage, m_nOriginalType) == 0x4F,
              "disposicion de CGarage");
namespace GaragesAddOne30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"garagesaddone30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"garagesaddone30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("garagesaddone30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CGarages","AddOne",0x4471E0,HookRoute::RegisteredTarget{}));
    registered["CGarages::AddOne"]=env.route->Registered();bodies["CGarages::AddOne"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    Run(env,ready,registered,bodies);
}
}
