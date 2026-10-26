#pragma once
#include <bit>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "PedIntelligence.h"
#include "PedStats.h"
#include "Entity/Entity.h"
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par PedLeaf69 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Pl69Layout {
static_assert(offsetof(CPedIntelligence, m_pPed) == 0x0 && offsetof(CPedIntelligence, m_fHearingRange) == 0xBC && offsetof(CPedIntelligence, m_apInterestingEntities) == 0x288, "CPedIntelligence");
static_assert(sizeof(CPedStat) == 0x34, "CPedStat");
}

namespace Pl69Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pedleaf69-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pedleaf69-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pedleaf69");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CPedIntelligence","IsInHearingRange",0x600C00);
    simple(1,"CPedIntelligence","HasInterestingEntites",0x602080);
    simple(2,"CPedIntelligence","RemoveAllInterestingEntities",0x602320);
    simple(3,"CPedStats","FindIndexWithPedStat",0x608940);
    // Colaborador sustituido: cuerpo de la DLL resuelto ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // 2: CEntity::CleanUpOldReference(CEntity**) 0x571A00 (con hook): el EXE y la DLL lo llaman por la misma direccion del EXE / el mismo cuerpo, y ambos llevan el falso.
    // 0, 1 y 3 no tienen colaboradores (hojas puras): ningun falso.
    env.stubs[2]={{Pair{0x571A00,DllBody(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),true},reinterpret_cast<void*>(&FakeCleanUp)}};
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators);
}
}
