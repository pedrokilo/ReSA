#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "FxInfoManager.h"
#include "FxManager.h"
#include "FxMemoryPool.h"
#include "FxTools.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FxInfoLoad28 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: FxInfoManager_c::Load (0x5C0B70) con su HookRoute (clase FxInfoManager_c, `RH_ScopedInstall(Load, 0x5C0B70)` de FxInfoManager.cpp).
// Colaboradores sustituidos y reales: ver Core.hpp y README.md.
static_assert(sizeof(FxInfoManager_c) == 0x14, "el banco trabaja con el objeto de 0x14 bytes");
static_assert(sizeof(FxInfoLoad28Fix::u32) == 4 && sizeof(FILESTREAM) == 4, "FILESTREAM es un puntero de 32 bits");
namespace FxInfoLoad28Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fxinfoload28-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fxinfoload28-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fxinfoload28");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("FxInfoManager_c","Load",0x5C0B70,HookRoute::RegisteredTarget{}));
    registered["FxInfoManager_c::Load"]=env.route->Registered();bodies["FxInfoManager_c::Load"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pRead={0x4AB250,B(static_cast<bool(*)(FILESTREAM,char*,int32)>(&::ReadLine))};
    env.pGetMem={0x4A9CA0,B(&FxMemoryPool_c::GetMem)};
    env.pAddFx={0x4A7B00,B(&FxInfoManager_c::AddFxInfo)};
    env.poolThis=uintptr_t(&g_fxMan.GetMemPool());
    ready&=env.poolThis==kPoolThis;      // la DLL debe reservar en el mismo pool que el EXE (0xA9AF2C)
    Run(env,ready,registered,bodies);
}
}
