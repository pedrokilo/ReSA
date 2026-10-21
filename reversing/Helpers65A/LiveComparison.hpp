#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "RwHelper.h"
#include "RenderWare/rw/rphanim.h"
#include "RenderWare/rw/rwcore.h"
#include "DecisionMakers/Decision.h"
#include "DecisionMakers/DecisionMaker.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "Audio/Managers/AEUserRadioTrackManager.h"
#include "FileMgr.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Helpers65A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas: CHud::FloorFloat (0x406CE0), RwHelper::GetAnimHierarchyFromFrameCallback (0x734A70), CDecisionMaker::MakeDecision (0x606060),
// CDecisionMakerTypes::ReadEventIndices (0x5BB9F0) y UserTracksLegacyVector::PushBack (0x4F4620).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace H65Layout {
static_assert(sizeof(CDecision) == 0x3C, "tamano de CDecision");
static_assert(sizeof(UserTracksLegacyVector) == 0x10 && offsetof(UserTracksLegacyVector, first) == 4 && offsetof(UserTracksLegacyVector, last) == 8 && offsetof(UserTracksLegacyVector, end) == 12, "vector VC6");
}

namespace H65Live {
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers65a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers65a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers65a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    benchThread=GetCurrentThreadId();
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH: se construyen antes que nada; el constructor deja el hook apagado.
    struct Row {const char* cls;const char* name;};
    static constexpr Row rows[kFns]{{"CHud","FloorFloat"},{"RwHelper","GetAnimHierarchyFromFrameCallback"},{"CDecisionMaker","MakeDecision"},{"CDecisionMakerTypes","ReadEventIndices"},{"UserTracksLegacyVector","PushBack"}};
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute(rows[f].cls,rows[f].name,kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    cbBodyDll=env.route[kCallback]->Body();
    // Colaboradores. Los reconstruidos en la DLL y llamados directamente por ella se parchean en la direccion del EXE y en el cuerpo de la DLL (resuelto saltando solo enlaces dentro del ASI).
    auto fake=[](auto* f){return reinterpret_cast<void*>(f);};
    env.stubs[kFloor]={{Pair{0x8219F0,0,false},fake(&FkFloor)}};
    env.stubs[kCallback]={{Pair{0x7C5160,Body(reinterpret_cast<uintptr_t>(&RpHAnimFrameGetHierarchy)),true},fake(&FkHier)},
                          {Pair{0x7F0DC0,Body(reinterpret_cast<uintptr_t>(&RwFrameForAllChildren)),false},fake(&FkChildren)}};
    env.stubs[kDecisionMaker]={{Pair{0x6040D0,Body(std::bit_cast<uintptr_t>(&CDecision::MakeDecision)),true},fake(&FkMakeDecision)}};
    env.stubs[kReadEvents]={{Pair{0x5387D0,Body(reinterpret_cast<uintptr_t>(&CFileMgr::SetDir)),true},fake(&FkSetDir)},
                            {Pair{0x538900,Body(reinterpret_cast<uintptr_t>(&CFileMgr::OpenFile)),true},fake(&FkOpenFile)},
                            {Pair{0x5389B0,Body(reinterpret_cast<uintptr_t>(&CFileMgr::ReadLine)),true},fake(&FkReadLine)},
                            {Pair{0x5389D0,Body(reinterpret_cast<uintptr_t>(&CFileMgr::CloseFile)),true},fake(&FkCloseFile)},
                            {Pair{0x8220AD,0,false},fake(&FkSscanf)}};
    env.stubs[kPushBack]={{Pair{0x4F3B30,0,false},fake(&Fk3B30)},{Pair{0x4F4320,0,false},fake(&Fk4320)}};
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
            const bool known=std::ranges::any_of(env.watches,[&](const Watch& w){return w.address==s.pair.exe;});
            if(!known){env.watches.emplace_back(s.pair.exe,16);if(s.pair.dll)env.watches.emplace_back(s.pair.dll,16);}
        }
        collaborators[kFn[f]]={{"stubbed",stubbed}};
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
