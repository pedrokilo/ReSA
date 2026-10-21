#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <map>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "CarCtrl.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par CarCtrlSteer63c dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta (SteerAICarTowardsPointInEscort 0x4336D0). Todos los colaboradores se sustituyen por falsos identicos en ambos lados
// (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace CS63CLive {
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
    if(!std::filesystem::exists(dir/"carctrlsteer63c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"carctrlsteer63c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("carctrlsteer63c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    {   // depuracion: el archivo puede contener "primero ultimo" para correr solo ese rango de casos
        std::ifstream start(dir/"carctrlsteer63c-cases.txt");unsigned a{},b{};
        if(start>>a>>b){dbgFirst=a;dbgLast=b;dbgMode=true;}
    }
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CCarCtrl",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
    json hooksFound = json::array();
    std::map<uintptr_t,uintptr_t> bodiesByExe;   // una sola ruta por direccion del EXE (los hooks propios de las tres rutas ya tienen la suya)
    auto mk=[&](uintptr_t exeAddr,void* fake,uintptr_t forcedBody=0){
        Pair p{exeAddr,forcedBody,false};
        std::string cat,name;
        for(unsigned f=0;f<kFns;++f)if(kEntry[f]==exeAddr){p.dll=Body(env.route[f]->Body());hooksFound.push_back({{"exe",exeAddr},{"class","CCarCtrl"},{"name",kName[f]},{"own_route",true}});}
        if(!p.dll&&bodiesByExe.count(exeAddr))p.dll=bodiesByExe[exeAddr];
        if(!p.dll&&FindHook(exeAddr,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exeAddr,HookRoute::RegisteredTarget{}));
            hooksFound.push_back({{"exe",exeAddr},{"class",cat},{"name",name}});
            if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
            bodiesByExe[exeAddr]=p.dll;
        }
        return StubSpec{p,fake};
    };
    #define FK(f) reinterpret_cast<void*>(&f)
    env.stubs[0]={mk(0x53CC70,FK(FkAtan)),mk(0x4325C0,FK(FkWeave)),mk(0x427FE0,FK(FkMaxSteer))};
    #undef FK
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
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
