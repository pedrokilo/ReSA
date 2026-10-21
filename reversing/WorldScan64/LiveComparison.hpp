#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Renderer.h"
#include "General.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par WorldScan64 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: ScanWorld (0x72CAE0) y SetExtraRectangleToScan (0x72D5E0). GetATanOfXY (0x53CC70) se sustituye por un falso identico en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace WS64Live {
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
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"worldscan64-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"worldscan64-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("worldscan64");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear
    // las rutas de los colaboradores y los parches de entrada.
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CWorldScan",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaborador de ScanWorld: CGeneral::GetATanOfXY (0x53CC70), con hook en la DLL (apagado): se parchea la entrada del EXE y el cuerpo de la DLL.
    json hooksFound = json::array();
    {
        Pair p{0x53CC70,0,false};
        std::string cat,name;
        if(FindHook(0x53CC70,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),0x53CC70,HookRoute::RegisteredTarget{}));
            hooksFound.push_back({{"exe",0x53CC70},{"class",cat},{"name",name}});
            if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
        }
        env.stubs[0]={StubSpec{p,reinterpret_cast<void*>(&FkAtan)}};
    }
    json stubbed0=json::array();
    for(const auto& s:env.stubs[0]) {
        env.watches.emplace_back(s.pair.exe,16);
        stubbed0.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
    }
    collaborators[kFn[0]]={{"dll_real",json::array()},{"stubbed",stubbed0},{"hooks_found",hooksFound}};
    collaborators[kFn[1]]={{"dll_real",json::array()},{"stubbed",json::array()},{"hooks_found",json::array()}};
    Run(env,ready,registered,bodies,collaborators);
}
}
