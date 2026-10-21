#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Garage.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par StoredCar64 dentro del juego: StoreCar y RestoreCar se llaman desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: CStoredCar::StoreCar (0x449760) y CStoredCar::RestoreCar (0x447E40). Los colaboradores se sustituyen por falsos identicos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace SC64Layout {
static_assert(sizeof(CStoredCar) == 0x40 && offsetof(CStoredCar, m_vPosn) == 0 && offsetof(CStoredCar, m_nHandlingFlags) == 0xC && offsetof(CStoredCar, m_nStoredCarFlags) == 0x10 && offsetof(CStoredCar, m_wModelIndex) == 0x12
              && offsetof(CStoredCar, m_awCarMods) == 0x14 && offsetof(CStoredCar, m_nPrimaryColor) == 0x32 && offsetof(CStoredCar, m_nRadioStation) == 0x36 && offsetof(CStoredCar, m_anCompsToUse) == 0x37
              && offsetof(CStoredCar, m_nBombType) == 0x39 && offsetof(CStoredCar, m_nPaintJob) == 0x3A && offsetof(CStoredCar, m_nNitroBoosts) == 0x3B && offsetof(CStoredCar, m_nPackedForwardX) == 0x3C, "CStoredCar");
}

namespace SC64Live {
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
    if(!std::filesystem::exists(dir/"storedcar64-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"storedcar64-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("storedcar64");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // El cuerpo de la DLL de cada funcion se calcula ANTES de crear los parches de colaboradores (HookRoute con RegisteredTarget lo resuelve al construirse).
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CStoredCar",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    json hooksFound = json::array();
    auto mk=[&](uintptr_t exeAddr,void* fake){
        Pair p{exeAddr,0,false};
        std::string cat,name;
        if(FindHook(exeAddr,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exeAddr,HookRoute::RegisteredTarget{}));
            hooksFound.push_back({{"exe",exeAddr},{"class",cat},{"name",name}});
            if(env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
        }
        return StubSpec{p,fake};
    };
    #define FK(f) reinterpret_cast<void*>(&f)
    env.stubs[0]={mk(0x6D0B70,FK(FkGetRemap))};
    env.stubs[1]={mk(0x4087E0,FK(FkReqModel)),mk(0x408C70,FK(FkReqUpg)),mk(0x407820,FK(FkHasUpg)),mk(0x6E2D50,FK(FkNew)),
                  mk(0x6C8D60,FK(FkCtor2<0>)),mk(0x6CE370,FK(FkCtor2<1>)),mk(0x6C4190,FK(FkCtor2<2>)),mk(0x6C8E20,FK(FkCtor2<3>)),mk(0x6F2940,FK(FkCtor2<4>)),
                  mk(0x6BF430,FK(FkCtor2<5>)),mk(0x6BF820,FK(FkCtor2<6>)),mk(0x6D03A0,FK(FkCtor2<7>)),mk(0x6B0A90,FK(FkCtorCar)),
                  mk(0x6E3400,FK(FkSetup)),mk(0x6D0C00,FK(FkSetRemap))};
    #undef FK
    std::vector<uintptr_t> seen;
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            if(std::find(seen.begin(),seen.end(),s.pair.exe)==seen.end()){seen.push_back(s.pair.exe);env.watches.emplace_back(s.pair.exe,16);}
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
