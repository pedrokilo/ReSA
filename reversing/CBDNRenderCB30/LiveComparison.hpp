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

// Banco par a par CBDNRenderCB30 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CCustomBuildingDNPipeline::CustomPipeRenderCB (0x5D6480) con su HookRoute. Los colaboradores sustituidos estan descritos en Core.hpp y README.md.
// fpRenderStateSet: el EXE lo llama por [[0xC97B24]+0x20]; la DLL por la macro RWSRCGLOBAL(dOpenDevice).fpRenderStateSet: deben ser el mismo campo.
static_assert(offsetof(RwGlobals, dOpenDevice) + offsetof(RwDevice, fpRenderStateSet) == 0x20, "fpRenderStateSet debe estar en +0x20 del motor, como en el EXE (call [edx+20h])");
namespace CBDNRenderCB30Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cbdnrendercb30-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cbdnrendercb30-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cbdnrendercb30");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CCustomBuildingDNPipeline","CustomPipeRenderCB",0x5D6480,HookRoute::RegisteredTarget{}));
    registered["CCustomBuildingDNPipeline::CustomPipeRenderCB"]=env.route->Registered();bodies["CCustomBuildingDNPipeline::CustomPipeRenderCB"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpo de la DLL de _rwD3D9EnableClippingIfNeeded (rpworld.cpp, sin hook): se resuelve ANTES de escribir ningun parche (PROCESO.md, trampas).
    env.pClip={0x756D90,Body(std::bit_cast<uintptr_t>(&_rwD3D9EnableClippingIfNeeded))};
    Run(env,ready,registered,bodies);
}
}
