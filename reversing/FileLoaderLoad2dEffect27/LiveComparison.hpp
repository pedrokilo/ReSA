#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include "FileLoader.h"
#include "TxdStore.h"
#include "MemoryMgr.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Plugins/TwoDEffectPlugin/2dEffect.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FileLoaderLoad2dEffect27 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CFileLoader::Load2dEffect (0x5B7670) con su HookRoute. Los colaboradores sustituidos y los que se ejecutan de verdad estan descritos en Core.hpp y README.md.
static_assert(sizeof(C2dEffect) == 0x40, "el banco trabaja con bloques de 0x40 bytes");
using Store2dFxType = CStore<C2dEffect, CModelInfo::NUM_2DFX_INFOS>;
static_assert(offsetof(Store2dFxType, m_aObjects) == 4, "la tienda del EXE tiene el contador (4 bytes) seguido de los bloques");
namespace FileLoader2dFx27Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fileloader2dfx27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fileloader2dfx27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fileloader2dfx27");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CFileLoader","Load2dEffect",0x5B7670,HookRoute::RegisteredTarget{}));
    registered["CFileLoader::Load2dEffect"]=env.route->Registered();bodies["CFileLoader::Load2dEffect"]=env.route->Body();
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas).
    auto B=[](auto fp){return Body(std::bit_cast<uintptr_t>(fp));};
    env.pStore={0x4C5A60,B(&CModelInfo::Get2dEffectStore)};
    env.pAdd2d={0x4C4D20,B(&CBaseModelInfo::Add2dEffect)};
    env.pPush={0x7316A0,B(&CTxdStore::PushCurrentTxd)};
    env.pFind={0x731850,B(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot))};
    env.pSet={0x7319C0,B(&CTxdStore::SetCurrentTxd)};
    env.pPop={0x7316B0,B(&CTxdStore::PopCurrentTxd)};
    env.pTex={0x7F3AC0,B(&RwTextureRead)};
    env.pMalloc={0x72F420,B(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc))};
    Run(env,ready,registered,bodies);
}
}
