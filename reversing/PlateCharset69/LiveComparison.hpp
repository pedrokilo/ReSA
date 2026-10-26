#pragma once
#include <bit>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CustomCarPlateMgr.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par PlateCharset69 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CCustomCarPlateMgr::LoadPlatecharsetDat (0x6FDC00, hook S). Colaboradores sustituidos por falsos en AMBOS lados: CFileMgr::SetDir, OpenFile, CloseFile y
// CFileLoader::LoadLine(FILESTREAM) (los cuatro con hook; parche de entrada en el EXE y en el cuerpo de la DLL). strtok y atoi corren de verdad en cada lado.
namespace Pc69Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"platecharset69-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"platecharset69-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("platecharset69");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    env.route.reset(new HookRoute("CCustomCarPlateMgr","LoadPlatecharsetDat",kEntry,HookRoute::RegisteredTarget{}));
    json registered={{kName,env.route->Registered()}};
    json bodies={{kName,env.route->Body()!=0}};
    bool ready=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    env.stubs={{Pair{0x5387D0,DllBody(&CFileMgr::SetDir),true},reinterpret_cast<void*>(&FakeSetDir)},
               {Pair{0x538900,DllBody(&CFileMgr::OpenFile),true},reinterpret_cast<void*>(&FakeOpenFile)},
               {Pair{0x536F80,DllBody(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),true},reinterpret_cast<void*>(&FakeLoadLine)},
               {Pair{0x5389D0,DllBody(&CFileMgr::CloseFile),true},reinterpret_cast<void*>(&FakeCloseFile)}};
    json stubbed=json::array();
    for(const auto& s:env.stubs) {
        env.watches.emplace_back(s.pair.exe,16);
        stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
    }
    env.watches.emplace_back(kEntry,16);if(env.route)env.watches.emplace_back(env.route->Body(),16);
    json collaborators={{kName,{{"dll_real",json::array()},{"stubbed",stubbed},{"crt_real",json::array({"strtok 0x82244B (EXE) / strtok del CRT de la DLL","atoi 0x82258E (EXE) / PlatecharAtoi (DLL)"})}}}};
    Run(env,ready,registered,bodies,collaborators);
}
}
