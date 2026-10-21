#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "MissionCleanup.h"
#include "World.h"
#include "Streaming.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par MissionCleanup64 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Dos rutas: CMissionCleanup::Process (0x468560) y CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects (0x4652D0). Todos los colaboradores se sustituyen por falsos identicos
// en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace MC64Layout {
static_assert(sizeof(CMissionCleanup) == 0x25C && sizeof(tMissionCleanupEntity) == 8 && offsetof(CMissionCleanup, m_Count) == 0x258 && offsetof(CMissionCleanup, m_Objects) == 0, "CMissionCleanup");
static_assert(offsetof(tMissionCleanupEntity, type) == 0 && offsetof(tMissionCleanupEntity, handle) == 4, "tMissionCleanupEntity");
}

namespace MC64Live {
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
    if(!std::filesystem::exists(dir/"missioncleanup64-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"missioncleanup64-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("missioncleanup64");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    {   // depuracion: el archivo puede contener "funcion primero ultimo" para correr solo ese rango de casos
        std::ifstream start(dir/"missioncleanup64-cases.txt");int f{};unsigned a{},b{};
        if(start>>f>>a>>b){dbgFn=f;dbgFirst=a;dbgLast=b;dbgMode=true;}
    }
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CMissionCleanup",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
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
    using RemoveFn = void (CMissionCleanup::*)(int32, MissionCleanUpEntityType);
    const RemoveFn removeFn = &CMissionCleanup::RemoveEntityFromList;
    const std::vector<StubSpec> stubsP{
        mk(0x50AC20,FK(FkFade)),mk(0x6F5DB0,FK(FkDisableTrains)),mk(0x6F6B60,FK(FkReleaseTrains)),mk(0x6CCC50,FK(FkAmbientPlanes)),mk(0x572440,FK(FkFillZones)),mk(0x443FF0,FK(FkClearGangWar)),
        mk(0x448B30,FK(FkRespraysOpen)),mk(0x56E230,FK(FkFpw)),mk(0x456DE0,FK(FkRemovePickups)),mk(0x460EC0,FK(FkRoadBlocks)),mk(0x407D10,FK(FkCopBikes)),mk(0x44DD00,FK(FkReleaseNodes)),
        mk(0x44D400,FK(FkUnMark)),mk(0x44D3B0,FK(FkTidyNodes)),mk(0x45A160,FK(FkRecordings)),mk(0x50C150,FK(FkWideOff)),mk(0x50D2D0,FK(FkInitScriptable)),mk(0x50CEF0,FK(FkDucking)),
        mk(0x7010F0,FK(FkPostFx)),mk(0x55FF20,FK(FkExtraColour)),mk(0x5072F0,FK(FkMissionAudio)),mk(0x72A510,FK(FkWeather)),mk(0x40B490,FK(FkSpecialChar),Body(std::bit_cast<uintptr_t>(&CStreaming::SetMissionDoesntRequireSpecialChar))),mk(0x46A7C0,FK(FkSuppressed)),
        mk(0x722870,FK(FkForceRender)),mk(0x5FBB70,FK(FkAllocator)),mk(0x5F7340,FK(FkDecisionMaker)),mk(0x60C7C0,FK(FkAlwaysFollow)),mk(0x60C800,FK(FkNeverFollow)),mk(0x60A4B0,FK(FkReappear)),
        mk(0x5EFF70,FK(FkSpeech)),mk(0x5EFF90,FK(FkSpeechScript)),mk(0x6817D0,FK(FkFindTask)),mk(0x634A40,FK(FkTaskFunc)),mk(0x441560,FK(FkClearSkip)),mk(0x56E210,FK(FkFpp)),
        mk(0x460840,FK(FkRespawn)),mk(0x5397A0,FK(FkFireFlags)),mk(0x4639E0,FK(FkStuckInit)),mk(0x50B930,FK(FkCamRestore)),mk(0x56E870,FK(FkSafe)),mk(0x588BE0,FK(FkHelp)),
        mk(0x486670,FK(FkCleanVeh)),mk(0x486300,FK(FkCleanPed)),mk(0x4866C0,FK(FkCleanObj)),mk(0x4839A0,FK(FkGetIdx)),mk(0x4AA3F0,FK(FkFxKill)),mk(0x492FD0,FK(FkRemoveFx)),
        mk(0x5FB870,FK(FkRemoveGroup)),mk(0x632C10,FK(FkFlush)),mk(0x607A70,FK(FkUnloadDm)),mk(0x493160,FK(FkRemoveSearch)),mk(0x4936C0,FK(FkRemoveCheckpoint)),mk(0x465A40,FK(FkRemoveTxd)),
        mk(0x4654B0,FK(FkRemoveEntity),Body(std::bit_cast<uintptr_t>(removeFn))),mk(0x608830,FK(FkPedFlag)),mk(0x6089F0,FK(FkAcqOne)),mk(0x608A20,FK(FkAcqAll))};
    const std::vector<StubSpec> stubsC{
        mk(0x410CE0,FK(FkHasCol)),mk(0x405600,FK(FkHaveIpl)),mk(0x4633E0,FK(FkIsStatic)),mk(0x542800,FK(FkMoving)),mk(0x6AF420,FK(FkPlaceAuto)),mk(0x6BEEB0,FK(FkPlaceBike))};
    #undef FK
    env.stubs[0]=stubsP;env.stubs[1]=stubsC;
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound}};
    }
    for(unsigned f=0;f<kFns;++f)for(const auto& s:env.stubs[f])if(std::none_of(env.watches.begin(),env.watches.end(),[&](const Watch& w){return w.address==s.pair.exe;}))env.watches.emplace_back(s.pair.exe,16);
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
