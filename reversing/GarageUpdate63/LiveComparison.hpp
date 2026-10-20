#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Garage.h"
#include "Garages.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par GarageUpdate63 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una sola ruta: CGarage::Update (0x44AA50). Todos los colaboradores se sustituyen por falsos identicos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace GU63Layout {
static_assert(sizeof(CGarage) == 0xD8 && offsetof(CGarage, m_fLeftCoord) == 0x28 && offsetof(CGarage, m_fRightCoord) == 0x2C && offsetof(CGarage, m_fFrontCoord) == 0x30 && offsetof(CGarage, m_fBackCoord) == 0x34
              && offsetof(CGarage, m_vPosn) == 0 && offsetof(CGarage, m_fTopZ) == 0x1C && offsetof(CGarage, m_fDoorPosition) == 0x38 && offsetof(CGarage, m_nTimeToOpen) == 0x3C && offsetof(CGarage, m_pTargetCar) == 0x40
              && offsetof(CGarage, m_nType) == 0x4C && offsetof(CGarage, m_nDoorState) == 0x4D && offsetof(CGarage, m_nFlags) == 0x4E, "CGarage");
}

namespace GU63Live {
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
    if(!std::filesystem::exists(dir/"garageupdate63-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"garageupdate63-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("garageupdate63");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    {   // depuracion: el archivo .start puede contener "primero ultimo" para correr solo ese rango de casos
        std::ifstream start(dir/"garageupdate63-cases.txt");unsigned a{},b{};
        if(start>>a>>b){dbgFirst=a;dbgLast=b;dbgMode=true;}
    }
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    env.route[0].reset(new HookRoute("CGarage",kName[0],kEntry[0],HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
    json hooksFound = json::array();
    auto mk=[&](uintptr_t exeAddr,void* fake,uintptr_t forcedBody=0){
        Pair p{exeAddr,forcedBody,false};
        std::string cat,name;
        if(FindHook(exeAddr,cat,name)) {
            env.collab.emplace_back(new HookRoute(cat.c_str(),name.c_str(),exeAddr,HookRoute::RegisteredTarget{}));
            hooksFound.push_back({{"exe",exeAddr},{"class",cat},{"name",name}});
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
        }
        return StubSpec{p,fake};
    };
    #define FK(f) reinterpret_cast<void*>(&f)
    const std::vector<StubSpec> stubs{
        mk(0x56E0D0,FK(FkFpv)),mk(0x56E210,FK(FkFpp)),mk(0x56E010,FK(FkFpc)),mk(0x56E230,FK(FkFpw)),
        mk(0x448BE0,FK(FkIn3d)),mk(0x448D30,FK(FkOut)),mk(0x44A830,FK(FkStatic)),mk(0x44A9C0,FK(FkEmpty)),mk(0x448EE0,FK(FkTouch)),mk(0x449690,FK(FkRemoveCars)),mk(0x449900,FK(FkStoreHide)),
        mk(0x44A660,FK(FkSlideOpen)),mk(0x44A750,FK(FkSlideClosed)),mk(0x448E50,FK(FkIsOutside)),mk(0x4494F0,FK(FkBlocking)),mk(0x4495F0,FK(FkCountCenter)),mk(0x449100,FK(FkOtherPed)),mk(0x447720,FK(FkOtherCar)),
        mk(0x449220,FK(FkThrow)),mk(0x447D80,FK(FkCalcDist)),mk(0x448550,FK(FkRestoreHide)),mk(0x448330,FK(FkNeatly)),mk(0x4485C0,FK(FkRestoreImp)),mk(0x449A50,FK(FkStoreImp)),mk(0x449FF0,FK(FkFindDoors)),
        mk(0x447B80,FK(FkMsg)),mk(0x4489F0,FK(FkFindSafe)),mk(0x44A210,FK(FkCountHide)),mk(0x4479A0,FK(FkSprayable)),mk(0x448660,FK(FkDetonator),Body(std::bit_cast<uintptr_t>(&CGarages::GivePlayerDetonator))),
        mk(0x566A60,FK(FkCallOff)),mk(0x55C180,FK(FkIncStat)),mk(0x558E40,FK(FkGetStat)),mk(0x55A070,FK(FkSetStat)),mk(0x506EA0,FK(FkAudio)),mk(0x4087E0,FK(FkReqModel)),mk(0x588BE0,FK(FkHelp)),
        mk(0x6A0050,FK(FkTextGet)),mk(0x5625A0,FK(FkClearWanted)),mk(0x6D0B70,FK(FkGetRemap)),mk(0x6D0C00,FK(FkSetRemap)),
        mk(0x6D1080,FK(FkAppearance),Body(std::bit_cast<uintptr_t>(&CVehicle::GetVehicleAppearance))),mk(0x6D2250,FK(FkDestroy)),mk(0x4C8500,FK(FkChoose)),mk(0x4AA440,FK(FkAddParticle)),mk(0x41BD90,FK(FkRandF)),
        mk(0x821B1E,FK(FkRand))};
    #undef FK
    env.stubs[0]=stubs;
    json stubbed=json::array();
    for(const auto& s:env.stubs[0]) {
        env.watches.emplace_back(s.pair.exe,16);
        stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
    }
    collaborators[kFn[0]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound}};
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
