#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "PlantMgr.h"
#include "MemoryMgr.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlantAtomicPrelit28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CPlantMgr::AtomicCreatePrelitIfNeeded (0x5DD100) con su HookRoute (hook registrado con RH_ScopedGlobalInstall en CPlantMgr::InjectHooks).
// Los cinco colaboradores de RenderWare se sustituyen por falsos identicos en ambos lados; ver Core.hpp y README.md.
namespace PlantAtomicPrelit28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"plantprelit28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"plantprelit28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plantprelit28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CPlantMgr","AtomicCreatePrelitIfNeeded",0x5DD100,HookRoute::RegisteredTarget{}));
    registered["CPlantMgr::AtomicCreatePrelitIfNeeded"]=env.route->Registered();bodies["CPlantMgr::AtomicCreatePrelitIfNeeded"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pCreate={0x74CA90,B(&RpGeometryCreate)};
    env.pGetMat={0x74C760,B(&RpGeometryTriangleGetMaterial)};
    env.pSetMat={0x74C6C0,B(&RpGeometryTriangleSetMaterial)};
    env.pUnlock={0x74C800,B(&RpGeometryUnlock)};
    env.pSetGeo={0x749D40,B(&RpAtomicSetGeometry)};
    Run(env,ready,registered,bodies);
}
}
