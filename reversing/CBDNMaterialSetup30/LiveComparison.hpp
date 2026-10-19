#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "MemoryMgr.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CBDNMaterialSetup30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CCustomBuildingDNPipeline::CustomPipeMaterialSetup (0x5D7120) con su HookRoute. Colaborador sustituido: SetFxEnvTexture (0x5D9570); real: RpMatFXMaterialGetEffects (0x812140).
namespace CBDNMaterialSetup30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cbdnmatsetup30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cbdnmatsetup30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cbdnmatsetup30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CCustomBuildingDNPipeline","CustomPipeMaterialSetup",0x5D7120,HookRoute::RegisteredTarget{}));
    registered["CCustomBuildingDNPipeline::CustomPipeMaterialSetup"]=env.route->Registered();bodies["CCustomBuildingDNPipeline::CustomPipeMaterialSetup"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpo de la DLL del colaborador: se resuelve ANTES de escribir ningun parche (PROCESO.md, trampas).
    env.pSetFx={0x5D9570,Body(std::bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::SetFxEnvTexture))};
    Run(env,ready,registered,bodies);
}
}
